/* match-flags: /O2 /Oy- /Ob2 */
/*
 * T032 - original object StaticSound.cpp (guessed name): SheepD3D.exe .text COMDATs 0x420680-0x420bd0 (the constructor,
 * ??_GStaticSound with the destructor inlined, the destructor, CreateFromFile, CreateFromWave, Free, RestoreBuffer, Play,
 * Stop, SetPaused, RewindBuffer*, IsPlaying, SetBufferVolume*, SetBufferFrequency*, FillBuffer; * = the /OPT:ICF
 * survivor that StreamSound's vtable shares); .rdata 0x574458-0x57449c (StaticSound's vtable, then 6000.0f and 10000.0f);
 * .bss 0x5cc6c8-0x6cc6c8 (g_waveStagingBuffer, taken as 1 MiB, defined here). Built /O2 /Oy- /Ob2.
 *
 * StaticSound defines its own SetBufferPosition and SetBufferPan: its 10000.0f, used only by SetBufferPan, sits in this
 * object's .rdata ahead of StreamSound's vtable, and the linker folded these two copies into StreamSound's identical
 * 0x4212e0 / 0x421390 (/OPT:ICF) - so their bodies are StreamSound's, with StaticSound:: in front. As an object of its
 * own it needs no inline_depth pragmas (~Sound and the WaveFile methods are in other objects, and the calls between
 * StaticSound's own methods are virtual).
 *
 * Shapes recovered by compiling: a function that can fail starts with `hr = E_FAIL` and nests the successful steps with
 * ONE return; Stop / SetPaused / RewindBuffer return nothing (the buffer == NULL path leaves EAX holding the NULL it just
 * tested). A value used once but held in a register of its own is written as a local.
 */
/* BYTES: layout, switches, temp. */
/* BYTES(layout): StaticSound::SetBufferPosition, StaticSound::SetBufferPan: StaticSound's own copy: the linker folds it into StreamSound's identical body (/OPT:ICF); it exists for this object's .rdata constant and the fold */
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
u8 g_waveStagingBuffer
    [0x100000]; /* 0x5cc6c8  StaticSound_FillBuffer reads the whole data chunk here (1 MiB: the map's size) */

/* ================================================================ StaticSound (vtable 0x574458) */

/* 0x420680 */
StaticSound::StaticSound()
{
    looping = 0;
    bufferBytes = 0;
    wave = 0;
    buffer = 0;
    panCb = 0;
    baseFrequency = 0;
    waveIsExternal = 1;
    volumeCb = -3000;
}

/* 0x420720 (0x4206c0 is the scalar deleting destructor, with this inlined) - Free's body written out, then ~Sound. */
StaticSound::~StaticSound()
{
    if (wave && !waveIsExternal) {
        delete wave;
        wave = 0;
    }
    if (buffer) {
        buffer->Release();
        buffer = 0;
    }
}

/* 0x420770 - opens the .wav at path itself (an owned WaveFile) and loads it whole into a new buffer. No callers. */
s32 StaticSound::CreateFromFile(SoundDevice *device, char *path)
{
    DSBUFFERDESC desc;
    s32 hr = E_FAIL;

    if (device->initialized == 1) {
        Free();
        wave = new WaveFile;
        if ((hr = wave->Open(path, 0, 0)) >= 0) {
            memset(&desc, 0, sizeof desc);
            desc.dwSize = sizeof desc;
            desc.dwFlags = device->GetBufferLocFlags() | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME;
            desc.dwBufferBytes = wave->ckData.cksize;
            desc.lpwfxFormat = wave->format;
            if ((hr = device->GetDSound()->CreateSoundBuffer(&desc, &buffer, 0)) >= 0) {
                bufferBytes = desc.dwBufferBytes;
                waveIsExternal = 0;
                hr = FillBuffer();
                buffer->GetFrequency(&baseFrequency);
            }
        }
    }
    return hr;
}

/* 0x420840 - the SFX path: adopts a WaveFile the caller keeps (a g_sndBankWaves entry) and loads it into a new buffer. */
s32 StaticSound::CreateFromWave(SoundDevice *device, WaveFile *w)
{
    DSBUFFERDESC desc;
    s32 hr = E_FAIL;

    if (device->initialized == 1) {
        Free();
        wave = w;
        if (wave->format) {
            memset(&desc, 0, sizeof desc);
            desc.dwSize = sizeof desc;
            desc.dwFlags = device->GetBufferLocFlags() | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME;
            desc.dwBufferBytes = wave->ckData.cksize;
            desc.lpwfxFormat = wave->format;
            if ((hr = device->GetDSound()->CreateSoundBuffer(&desc, &buffer, 0)) >= 0) {
                bufferBytes = desc.dwBufferBytes;
                waveIsExternal = 1;
                hr = FillBuffer();
                buffer->GetFrequency(&baseFrequency);
            }
        }
    }
    return hr;
}

/* 0x4208f0 */
s32 StaticSound::Free()
{
    if (wave && !waveIsExternal) {
        delete wave;
        wave = 0;
    }
    if (buffer) {
        buffer->Release();
        buffer = 0;
    }
    return S_OK;
}

/* 0x420930 - if DirectSound lost the buffer, restores it and reloads the sample. The loop leaves only on S_OK, and sleeps
 * 10 ms only for DSERR_BUFFERLOST, so any other failure spins it at full speed and never returns (StreamSound's version
 * 0x4210e0 is the SDK's do/while and calls Restore twice per turn; this one calls it once). */
s32 StaticSound::RestoreBuffer()
{
    DWORD status;
    s32 hr;

    if (buffer == 0)
        return S_OK;
    if ((hr = buffer->GetStatus(&status)) < 0)
        return hr;
    if (status & DSBSTATUS_BUFFERLOST) {
        for (;;) {
            hr = buffer->Restore();
            if (hr == DSERR_BUFFERLOST)
                Sleep(10);
            else if (hr == S_OK)
                break;
        }
        return FillBuffer();
    }
    return hr;
}

/* 0x420990 */
s32 StaticSound::Play()
{
    s32 hr = E_FAIL;

    if (buffer) {
        if ((hr = RestoreBuffer()) >= 0) {
            if (looping == 1)
                hr = buffer->Play(0, 0, DSBPLAY_LOOPING);
            else
                hr = buffer->Play(0, 0, 0);
        }
    }
    return hr;
}

/* 0x4209d0 - stops and rewinds. */
void StaticSound::Stop()
{
    if (buffer) {
        buffer->Stop();
        buffer->SetCurrentPosition(0);
    }
}

/* 0x4209f0 - pause stops without rewinding; resume plays on from the cursor. */
void StaticSound::SetPaused(u8 pause)
{
    if (buffer) {
        if (pause == 1)
            buffer->Stop();
        else if (looping == 1)
            buffer->Play(0, 0, DSBPLAY_LOOPING);
        else
            buffer->Play(0, 0, 0);
    }
}

/* 0x420a40 Sound_RewindBuffer: StaticSound's and StreamSound's are the same bytes; the linker kept this one for both. */
void StaticSound::RewindBuffer()
{
    if (buffer)
        buffer->SetCurrentPosition(0);
}

/* StaticSound's own copy of Sound_SetBufferPosition: the linker folded it into StreamSound's identical 0x4212e0
 * (/OPT:ICF), so no byte of it survives here. The body is StreamSound::SetBufferPosition's. */
s32 StaticSound::SetBufferPosition(float frac)
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

/* 0x420a50 */
u8 StaticSound::IsPlaying()
{
    DWORD status = 0;
    u8 playing = 0;

    if (buffer) {
        buffer->GetStatus(&status);
        playing = (u8)(status & DSBSTATUS_PLAYING);
    }
    return playing;
}

/* 0x420a80 Sound_SetBufferVolume (StreamSound's twin folded into it) - 0..1 maps linearly onto -60..0 dB. */
s32 StaticSound::Sound_SetBufferVolume(float volume)
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

/* StaticSound's own copy of Sound_SetBufferPan: folded into StreamSound's identical 0x421390 (/OPT:ICF). Its 10000.0f
 * is the one this object owns (0x574498). The body is StreamSound::SetBufferPan's. */
s32 StaticSound::SetBufferPan(float pan)
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

/* 0x420af0 Sound_SetBufferFrequency (StreamSound's twin folded into it) */
s32 StaticSound::Sound_SetBufferFrequency(u32 hz)
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

/* 0x420b40 - reads the whole data chunk into the staging buffer, rewinds the file and copies bufferBytes into the buffer. */
/* BYTES(temp): size is a local because the original holds the chunk size in a register of its own (0x420b4c) */
s32 StaticSound::FillBuffer()
{
    void *ptr1;
    void *ptr2;
    DWORD len1;
    DWORD len2;
    u32 read;
    u32 size; /* a local: the original holds the chunk size in a register of its own (0x420b4c) */
    s32 hr;

    size = wave->ckData.cksize;
    if ((hr = wave->Read(size, g_waveStagingBuffer, &read)) >= 0) {
        wave->ResetFile();
        if ((hr = buffer->Lock(0, bufferBytes, &ptr1, &len1, &ptr2, &len2, 0)) >= 0) {
            memcpy(ptr1, g_waveStagingBuffer, bufferBytes);
            buffer->Unlock(ptr1, bufferBytes, 0, 0);
        }
    }
    return hr;
}
