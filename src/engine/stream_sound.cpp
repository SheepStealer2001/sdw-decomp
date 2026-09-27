/* match-flags: /O2 /Oy- /Ob2 */
/*
 * T033 - original object StreamSound.cpp (guessed name): SheepD3D.exe .text COMDATs 0x420bd0-0x421740 (the constructor,
 * ??_GStreamSound with the destructor inlined, the destructor, CreateFromFile, CreateFromWave, CreateFromWaveEx, Free,
 * RestoreBuffer, ServiceNotify, Play, Stop, SetPaused, SetBufferPosition*, IsPlaying, SetBufferPan*, FillBuffer,
 * ReadOrPadSilence, GetVoiceAmplitude; * = the /OPT:ICF survivor that StaticSound's vtable shares); .rdata
 * 0x57449c-0x5744dc (StreamSound's vtable, then 0.125f); .bss 0x6cc6c8-0x6cc848 (an unreferenced 0x80-byte table,
 * then g_streamNotifyPos and g_streamNotifyPos2). Built /O2 /Oy- /Ob2.
 *
 * Besides the StreamSound functions, this object defines StreamSound's own RewindBuffer, Sound_SetBufferVolume and
 * Sound_SetBufferFrequency: the original's StreamSound object defined them (its vtable slots are virtual overrides like
 * StaticSound's) and the linker folded the copies into StaticSound's identical 0x420a40 / 0x420a80 / 0x420af0
 * (/OPT:ICF) - so their bodies are StaticSound's, with StreamSound:: in front.
 *
 * Inlining: ~StreamSound calls StreamSound::Free out of line in the exe, which one /Ob2 object would inline (this may
 * once have been two objects, split somewhere in 0x420c90-0x421080). The two `#pragma inline_depth` lines are there
 * for that: depth 1 at the constructor, where the vtable and ??_G are emitted (the destructor is expanded into ??_G,
 * nothing nested in it), depth 0 for the rest. Without them 0x420c40 and 0x420c70 come out with Free expanded
 * (measured).
 *
 * .bss: the map gives this object 0x6cc6c8-0x6cc848, and code refers only to g_streamNotifyPos 0x6cc748 and
 * g_streamNotifyPos2 0x6cc7c8. The 0x80 bytes before them (the size of one DSBPOSITIONNOTIFY[16]) are defined as an
 * unreferenced third table, g_streamNotifyPosReserved - a name chosen because VC6 orders an object's .bss by a
 * hash of the names (src/README.md: key 28 < 103 g_streamNotifyPos < 584 g_streamNotifyPos2), so it lands first.
 * Nothing reads it; what it was is not recovered.
 *
 * IID_IDirectSoundNotify (0x577638) is DirectX SDK library data, outside every game object: it is declared extern "C"
 * here (no library defines a C++-linkage one), the same bytes either way.
 *
 * Source shapes (each byte-proven, each a representation rather than proof of the original spelling):
 * GetVoiceAmplitude scopes the play/write cursors to the block that ends at their last use; ReadOrPadSilence defers the
 * refill loop behind a `bool refill` the optimiser removes. A function that can fail starts with `hr = E_FAIL` and nests
 * the successful steps with ONE return.
 */
/* BYTES: bss-name, flow, layout, slot-scope, switches. */
/* BYTES(layout): StreamSound::RewindBuffer, StreamSound::Sound_SetBufferVolume, StreamSound::Sound_SetBufferFrequency: StreamSound's own copy: the linker folds it into StaticSound's identical body (/OPT:ICF) */
/* BYTES(switches): StreamSound::StreamSound .. (the constructor region): inline_depth 1 here and 0 below: the original calls Free out of line from ~StreamSound, which /Ob2 would otherwise expand */
/* BYTES(bss-name): placeholder named for its .bss hash key 28 so it lands first; nothing reads it */
/* BYTES(switches): built with /O2 /Oy- /Ob2, not the project recipe; the file header says why */
#define SDW_MEMBERS_Sound Sound();             /* 0x4205b0 Sound_Construct (T031) */
#define SDW_MEMBERS_StaticSound StaticSound(); /* 0x420680 StaticSound_Construct */
#define SDW_MEMBERS_StreamSound StreamSound(); /* 0x420bd0 StreamSound_Construct */
#define SDW_MEMBERS_WaveFile WaveFile();       /* 0x421740 WaveFile_Construct (T034) */
#include "sdw_classes.h"

#include "../sdk/win32.h"
#include "../sdk/mmsystem.h"
#include "../sdk/dsound.h"
#include "../sdk/crt.h"

#define SOUND_VOLUME_FLOOR (-6000) /* the game's floor (-60 dB), not DirectSound's DSBVOLUME_MIN (-10000) */

/* ---- globals ---- */
DSBPOSITIONNOTIFY
g_streamNotifyPosReserved[16]; /* 0x6cc6c8  unreferenced (see the header); the name is placed by the .bss name hash */
DSBPOSITIONNOTIFY g_streamNotifyPos[16];  /* 0x6cc748  StreamSound_CreateFromWaveEx's notification table */
DSBPOSITIONNOTIFY g_streamNotifyPos2[16]; /* 0x6cc7c8  StreamSound_CreateFromFile's */

/* ================================================================ StreamSound (vtable 0x57449c) */

#pragma inline_depth(1)
/* 0x420bd0 */
StreamSound::StreamSound()
{
    waveIsExternal = 1;
    volumeCb = -3000;
    pDevice = 0;
    looping = 0;
    bufferBytes = 0;
    wave = 0;
    buffer = 0;
    panCb = 0;
    baseFrequency = 0;
    lipSyncEnabled = 0;
    pNotify = 0;
    eofReached = 0;
    bitsPerSample = blockAlign = channels = bytesPerSampleChannel = 0;
    samplesPerSec = 0;
    voiceAmplitude = 0;
    playProgress = writeOffset = lastPlayCursor = 0;
    notifySize = 0;
    stopped = 1;
}

#pragma inline_depth(0)

/* 0x420c70 (0x420c40 is the scalar deleting destructor, with this inlined) */
StreamSound::~StreamSound()
{
    Free();
}

/* 0x420c90 - the unused variant that opens its own WaveFile. It is CreateFromWaveEx's code with the two settings as
 * locals: 2 seconds and 16 segments (VC6 folds `* 2.0f / 16` into the single float 0.125f at 0x5744d8, and keeps the
 * 16 as the loop's down-counter, which is how the constants are recovered). No callers. */
s32 StreamSound::CreateFromFile(SoundDevice *device, char *path)
{
    DSBUFFERDESC desc;
    void *event;
    s32 hr = E_FAIL;
    u8 i;
    float fBlockAlign;
    float fSamplesPerSec;
    float seconds = 2.0f;
    u8 notifications = 16;

    if (device->initialized == 1) {
        pDevice = device;
        if ((hr = device->RegisterStreamEvent(this, &event)) >= 0) {
            if (wave && !waveIsExternal) {
                delete wave;
                wave = 0;
            }
            if (buffer) {
                buffer->Release();
                buffer = 0;
            }
            wave = new WaveFile;
            if ((hr = wave->Open(path, 0, 0)) >= 0) {
                stopped = 1;
                eofReached = 0;
                lipSyncEnabled = 0;
                lastPlayCursor = 0;
                playProgress = 0;
                fSamplesPerSec = (float)wave->format->nSamplesPerSec;
                fBlockAlign = (float)wave->format->nBlockAlign;
                blockAlign = (u8)fBlockAlign;
                bitsPerSample = (u8)wave->format->wBitsPerSample;
                channels = (u8)wave->format->nChannels;
                bytesPerSampleChannel = blockAlign / channels;
                samplesPerSec = (u32)fSamplesPerSec;
                voiceAmplitude = 0;
                notifySize = (u32)(fBlockAlign * fSamplesPerSec * seconds / notifications);
                notifySize -= notifySize % wave->format->nBlockAlign;
                bufferBytes = notifySize * notifications;
                memset(&desc, 0, sizeof desc);
                desc.dwSize = sizeof desc;
                desc.dwFlags = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_CTRLFREQUENCY |
                               DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME;
                desc.dwBufferBytes = bufferBytes;
                desc.lpwfxFormat = wave->format;
                if ((hr = pDevice->GetDSound()->CreateSoundBuffer(&desc, &buffer, 0)) >= 0) {
                    /* cast kept: COM returns the interface through a void ** out parameter */
                    buffer->QueryInterface(IID_IDirectSoundNotify, (void **)&pNotify);
                    for (i = 0; i < notifications; i++) {
                        g_streamNotifyPos2[i].dwOffset = notifySize * (i + 1) - 1;
                        g_streamNotifyPos2[i].hEventNotify = event;
                    }
                    if ((hr = pNotify->SetNotificationPositions(notifications, g_streamNotifyPos2)) < 0)
                        pNotify->Release();
                    waveIsExternal = 0;
                    buffer->GetFrequency(&baseFrequency);
                }
            }
        }
    }
    return hr;
}

/* 0x420e70 */
s32 StreamSound::CreateFromWave(SoundDevice *device, WaveFile *w)
{
    return CreateFromWaveEx(device, w, 3.0f, 16);
}

/* 0x420e90 - the live one (the stream player's loader thread passes 2.0 s): adopts the caller's WaveFile, sizes the buffer
 * to `seconds` of audio (at least 2) cut into `notifications` segments (at most 16), and arms one notification per segment
 * on the event the device handed out. */
s32 StreamSound::CreateFromWaveEx(SoundDevice *device, WaveFile *w, float seconds, u8 notifications)
{
    DSBUFFERDESC desc;
    void *event;
    s32 hr = E_FAIL;
    u8 i;
    float fBlockAlign;
    float fSamplesPerSec;

    if (device->initialized == 1) {
        pDevice = device;
        if ((hr = device->RegisterStreamEvent(this, &event)) >= 0) {
            if (wave && !waveIsExternal) {
                delete wave;
                wave = 0;
            }
            if (buffer) {
                buffer->Release();
                buffer = 0;
            }
            wave = w;
            if (wave->format) {
                stopped = 1;
                eofReached = 0;
                lipSyncEnabled = 0;
                lastPlayCursor = 0;
                playProgress = 0;
                if (seconds < 2.0f)
                    seconds = 2.0f;
                if (notifications > 16)
                    notifications = 16;
                fSamplesPerSec = (float)wave->format->nSamplesPerSec;
                fBlockAlign = (float)wave->format->nBlockAlign;
                blockAlign = (u8)fBlockAlign;
                bitsPerSample = (u8)wave->format->wBitsPerSample;
                channels = (u8)wave->format->nChannels;
                bytesPerSampleChannel = blockAlign / channels;
                samplesPerSec = (u32)fSamplesPerSec;
                voiceAmplitude = 0;
                notifySize = (u32)(fBlockAlign * fSamplesPerSec * seconds / notifications);
                notifySize -= notifySize % wave->format->nBlockAlign;
                bufferBytes = notifySize * notifications;
                memset(&desc, 0, sizeof desc);
                desc.dwSize = sizeof desc;
                desc.dwFlags = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_CTRLFREQUENCY |
                               DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME;
                desc.dwBufferBytes = bufferBytes;
                desc.lpwfxFormat = wave->format;
                if ((hr = pDevice->GetDSound()->CreateSoundBuffer(&desc, &buffer, 0)) >= 0) {
                    /* cast kept: COM returns the interface through a void ** out parameter */
                    buffer->QueryInterface(IID_IDirectSoundNotify, (void **)&pNotify);
                    for (i = 0; i < notifications; i++) {
                        g_streamNotifyPos[i].dwOffset = notifySize * (i + 1) - 1;
                        g_streamNotifyPos[i].hEventNotify = event;
                    }
                    if ((hr = pNotify->SetNotificationPositions(notifications, g_streamNotifyPos)) < 0)
                        pNotify->Release();
                    waveIsExternal = 1;
                }
            }
        }
    }
    return hr;
}

/* 0x421080 - the only path that gives the device's stream slot back. */
s32 StreamSound::Free()
{
    if (pDevice) {
        pDevice->UnregisterStreamEvent(this);
        pDevice = 0;
    }
    if (wave && !waveIsExternal) {
        delete wave;
        wave = 0;
    }
    if (pNotify) {
        pNotify->Release();
        pNotify = 0;
    }
    if (buffer) {
        buffer->Release();
        buffer = 0;
    }
    return S_OK;
}

/* 0x4210e0 - the SDK's loop: every iteration calls Restore twice (the first result only decides the Sleep). */
s32 StreamSound::RestoreBuffer()
{
    DWORD status;
    s32 hr;

    if (buffer == 0)
        return S_OK;
    if ((hr = buffer->GetStatus(&status)) < 0)
        return hr;
    if (status & DSBSTATUS_BUFFERLOST) {
        do {
            hr = buffer->Restore();
            if (hr == DSERR_BUFFERLOST)
                Sleep(10);
        } while ((hr = buffer->Restore()) != 0);
        return FillBuffer();
    }
    return hr;
}

/* 0x421140 - on a position notification: advances playProgress by the play cursor's movement, refills the segment at
 * writeOffset, and stops the buffer once a non-looping file has run out and its tail has been played. */
s32 StreamSound::ServiceNotify()
{
    s32 hr = E_FAIL;
    void *ptr = 0;
    DWORD len;
    DWORD play;
    DWORD write;
    u32 delta;

    if (buffer) {
        if (buffer->GetCurrentPosition(&play, &write) >= 0) {
            if (play < lastPlayCursor)
                delta = bufferBytes - lastPlayCursor + play;
            else
                delta = play - lastPlayCursor;
            playProgress += delta;
            lastPlayCursor = play;
        }
        if ((hr = buffer->Lock(writeOffset, notifySize, &ptr, &len, 0, 0, 0)) >= 0) {
            /* cast kept: Lock hands the locked bytes back as a void * (the SDK's signature) */
            hr = ReadOrPadSilence((u8 *)ptr, len);
            buffer->Unlock(ptr, len, 0, 0);
            ptr = 0;
            if (eofReached == 1 && playProgress >= wave->ckRiff.cksize) {
                stopped = 1;
                buffer->Stop();
                buffer->SetCurrentPosition(0);
            }
            writeOffset = (writeOffset + len) % bufferBytes;
        }
    }
    return hr;
}

/* 0x421230 - restarts the stream from the top of the file; lipSync arms GetVoiceAmplitude for this clip. */
s32 StreamSound::Play(u32 lipSync)
{
    s32 hr = E_FAIL;

    if (buffer) {
        if ((hr = RestoreBuffer()) >= 0) {
            if ((hr = FillBuffer()) >= 0) {
                stopped = 0;
                lipSyncEnabled = lipSync;
                hr = buffer->Play(0, 0, DSBPLAY_LOOPING);
            }
        }
    }
    return hr;
}

/* 0x421280 */
void StreamSound::Stop()
{
    if (buffer) {
        stopped = 1;
        buffer->Stop();
        buffer->SetCurrentPosition(0);
    }
}

/* 0x4212b0 - does not touch `stopped`, so IsPlaying stays true while paused. */
void StreamSound::SetPaused(u8 pause)
{
    if (buffer) {
        if (pause == 1)
            buffer->Stop();
        else
            buffer->Play(0, 0, DSBPLAY_LOOPING);
    }
}

/* StreamSound's own copy of Sound_RewindBuffer: folded into StaticSound's identical 0x420a40 (/OPT:ICF). The body is
 * StaticSound::RewindBuffer's. */
void StreamSound::RewindBuffer()
{
    if (buffer)
        buffer->SetCurrentPosition(0);
}

/* 0x4212e0 Sound_SetBufferPosition (StaticSound's twin folded into it) - frac of the buffer; a looping sound keeps only the
 * fractional part, a one-shot restarts from 0 when frac >= 1. */
s32 StreamSound::SetBufferPosition(float frac)
{
    s32 hr = E_FAIL;
    float pos;
    DWORD offset;

    if (buffer) {
        if (frac < 0.0f)
            return E_FAIL;
        if (looping == 1) {
            pos = frac - (s32)frac;
        } else {
            pos = frac;
            if (frac >= 1.0f)
                pos = 0.0f;
        }
        offset = (DWORD)(bufferBytes * pos);
        hr = buffer->SetCurrentPosition(offset);
    }
    return hr;
}

/* 0x421370 - trusts the `stopped` flag instead of asking DirectSound. */
u8 StreamSound::IsPlaying()
{
    u8 playing = 0;

    if (buffer)
        playing = !stopped;
    return playing;
}

/* StreamSound's own copy of Sound_SetBufferVolume: folded into StaticSound's identical 0x420a80 (/OPT:ICF); its 6000.0f
 * is StaticSound's (0x574494). The body is StaticSound::Sound_SetBufferVolume's. */
s32 StreamSound::Sound_SetBufferVolume(float volume)
{
    s32 old = volumeCb;
    s32 hr = E_FAIL;

    if (buffer) {
        volumeCb = (s32)(volume * 6000.0f - 6000.0f);
        if (volumeCb < SOUND_VOLUME_FLOOR)
            volumeCb = SOUND_VOLUME_FLOOR;
        else if (volumeCb > DSBVOLUME_MAX)
            volumeCb = DSBVOLUME_MAX;
        if ((hr = buffer->SetVolume(volumeCb)) < 0)
            volumeCb = old;
    }
    return hr;
}

/* 0x421390 Sound_SetBufferPan (StaticSound's twin folded into it) */
s32 StreamSound::SetBufferPan(float pan)
{
    s32 old = panCb;
    s32 hr = E_FAIL;

    if (buffer) {
        if (pan < 0.0f) {
            panCb = (s32)(pan * 10000.0f);
            if (panCb < DSBPAN_LEFT)
                panCb = DSBPAN_LEFT;
        } else {
            panCb = (s32)(pan * 10000.0f);
            if (panCb > DSBPAN_RIGHT)
                panCb = DSBPAN_RIGHT;
        }
        if ((hr = buffer->SetPan(panCb)) < 0)
            panCb = old;
    }
    return hr;
}

/* StreamSound's own copy of Sound_SetBufferFrequency: folded into StaticSound's identical 0x420af0 (/OPT:ICF). The body
 * is StaticSound::Sound_SetBufferFrequency's. */
s32 StreamSound::Sound_SetBufferFrequency(u32 hz)
{
    u32 old = baseFrequency;
    s32 hr = E_FAIL;

    if (buffer) {
        baseFrequency = hz;
        if (baseFrequency < DSBFREQUENCY_MIN)
            baseFrequency = DSBFREQUENCY_MIN;
        else if (baseFrequency > DSBFREQUENCY_MAX)
            baseFrequency = DSBFREQUENCY_MAX;
        if ((hr = buffer->SetFrequency(baseFrequency)) < 0)
            baseFrequency = old;
    }
    return hr;
}

/* 0x421410 - rewinds the file and fills the whole buffer from the top. */
s32 StreamSound::FillBuffer()
{
    void *ptr;
    DWORD len;
    s32 hr;

    eofReached = 0;
    writeOffset = 0;
    playProgress = 0;
    lastPlayCursor = 0;
    wave->ResetFile();
    buffer->SetCurrentPosition(0);
    if ((hr = buffer->Lock(0, bufferBytes, &ptr, &len, 0, 0, 0)) < 0)
        return hr;
    /* cast kept: Lock hands the locked bytes back as a void * (the SDK's signature) */
    hr = ReadOrPadSilence((u8 *)ptr, len);
    buffer->Unlock(ptr, len, 0, 0);
    writeOffset = len % bufferBytes;
    return hr;
}

/* 0x421490 - fills size bytes at dest from the file; at the end of a looping file it rewinds and keeps reading, at the end
 * of a one-shot it pads with silence and from then on fills whole blocks with silence. The two memset tails are
 * cross-jumped as in the original, and the deferred refill loop (see `refill`) puts the end-of-file block before the
 * looping loop, where the original has it. */
/* BYTES(flow): the refill loop is deferred behind refill, which the optimiser removes: that puts the end-of-file memset block first, as the original */
s32 StreamSound::ReadOrPadSilence(u8 *dest, u32 size)
{
    s32 hr = S_OK;
    u8 silence;
    u32 read;
    u32 total;
    bool refill = 0; /* the loop is DEFERRED behind this flag, and the optimiser then removes the flag: that is
                                 * what puts the end-of-file / shared memset block before the refill loop, as the original
                                 * has it. No storage of its own survives. */

    if (wave->format)
        silence = (u8)(wave->format->wBitsPerSample == 8 ? 0x80 : 0);
    if (!eofReached) {
        hr = wave->Read(size, dest, &read);
        if (read < size) {
            if (!looping) {
                eofReached = 1;
                memset(dest + read, silence, size - read);
            } else {
                total = read;
                refill = 1;
            }
        }
    } else {
        memset(dest, silence, size);
    }

    if (refill) {
        while (total < size) {
            if ((hr = wave->ResetFile()) < 0)
                return hr;
            hr = wave->Read(size - total, dest + total, &read);
            if (hr < 0)
                return hr;
            total += read;
        }
    }
    return hr;
}

/* 0x421570 - the lip-sync meter (game code, not SDK): samples the stream's file at the play position, keeps a moving
 * average of |sample| and gates it with hysteresis (opens above 500, closes below 400). refresh 0 returns the last value.
 * The original spills the window count into the play cursor's slot once that is dead (frame 0xc, play at EBP-8): the
 * block that scopes the cursors below is what reproduces it. */
/* BYTES(slot-scope): the cursors are scoped to this block: when it closes VC6 gives the dead play slot (EBP-8) to the window count, as the original */
u32 StreamSound::GetVoiceAmplitude(s32 refresh)
{
    u32 sum = 0;
    u16 window = 16;
    u16 i;
    s32 pos;
    u32 delta;
    u32 avg;
    char sample[2];
    s32 v;

    if (refresh) {
        if (wave && IsPlaying() && lipSyncEnabled && buffer) {
            /* The cursors live ONLY here. Their scope ends at the closing brace below, which is what lets VC6 give the
             * widened window count the dead `play` slot (EBP-8) further down - the last 4 bytes of this function (it is
             * a lifetime, not an optimisation flag). */
            {
                DWORD play;
                DWORD write;
                if (buffer->GetCurrentPosition(&play, &write) < 0)
                    goto invalid;
                if (play < lastPlayCursor)
                    delta = bufferBytes - lastPlayCursor + play;
                else
                    delta = play - lastPlayCursor;
                playProgress += delta;
                lastPlayCursor = play;
            }
            if (samplesPerSec < 44100)
                window = 8;
            for (i = 0; i < window - 1; i++)
                amplitudeWindow[i] = amplitudeWindow[i + 1];
            pos = mmioSeek(wave->hmmio, 0, SEEK_CUR);
            if (pos < 0)
                return 0;
            mmioSeek(wave->hmmio, playProgress, SEEK_SET);
            if (mmioRead(wave->hmmio, sample, bytesPerSampleChannel) > 0) {
                if (bytesPerSampleChannel > 1) {
                    /* the two bytes are sign-extended separately and OR'ed (movsx ch / movsx cl at 0x421680), so a low
                     * byte >= 0x80 fills the top bits and the magnitude comes out as a small negative number: the meter
                     * under-reads most 16-bit samples. Reproduced, not fixed. */
                    v = (sample[1] << 8) | sample[0];
                    if (v < 0)
                        v = -v;
                    amplitudeWindow[window - 1] = v;
                } else {
                    v = sample[0] < 0 ? -sample[0] : sample[0];
                    amplitudeWindow[window - 1] = v << 7;
                }
            }
            mmioSeek(wave->hmmio, pos, SEEK_SET);
            for (i = 0; i < window - 1; i++)
                sum += amplitudeWindow[i];
            avg = sum / window;
            if (voiceAmplitude)
                voiceAmplitude = avg < 400 ? 0 : avg;
            else
                voiceAmplitude = avg > 500 ? avg : 0;
            return voiceAmplitude;
        }
    invalid:
        voiceAmplitude = 0;
    }
    return voiceAmplitude;
}
