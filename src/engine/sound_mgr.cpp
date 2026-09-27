/* match-init: Sound_StaticInit_BankWaves Sound_StaticInit_Channels */
/*
 * T284, guessed original file SoundMgr.cpp: .text 0x548dd0-0x549d1d, .data 0x57c7bc-0x57c7cc
 * (the two "vdx7" literals of Load_SND), .bss 0x6deff0-0x6e3670 (the bank arrays, the voices, the .SND image pointer,
 * the bank count and the distance-falloff parameters); static initialisers at 0x548dd5 / 0x548e87 (.CRT$XCU 0x57909c /
 * 0x5790a0). The .bss globals are defined here, in address order.
 * The wave bank (0x6dfbf0) is g_sndWaveBank here. VC6 orders the hashed .bss group by (h ^ h >> 16) & 0x3ff of each name
 * (h = h*4 + (h >> 4) + c), and the two constructed arrays can only be in that group: the descriptive name
 * g_sndBankWaves (key 873) would land after g_soundChannels (496), the exe has it before. g_sndWaveBank (285) sits
 * between g_sndBankEntries (168) and g_soundChannels (496). Pinned below.
 *
 * match-addr: g_sndWaveBank=0x6dfbf0
 *
 * The sound-effect bank and its 24 voices, SheepD3D.exe 0x548dd0-0x549d1c: the whole original file, from the int3 padding
 * that ends the .MLT file to the padding before the .WAR loader (src/engine/load_war.cpp).
 *
 * Load_SND reads a level's .SND file whole (Bs_LoadFile) and keeps it in g_pSndFile: {u32 crc; "vdx7"; u16 count; u16 pad;
 * then per sample a 12-byte SndBankEntry {soundId, dataSize, loop} followed by dataSize bytes of RIFF/WAVE}. Each entry
 * header is copied to g_sndBankEntries[i] and the WaveFile g_sndWaveBank[i] is opened IN MEMORY on the image (Open(ptr, 1,
 * size): the 'MEM ' mmio path). Nothing checks the count against the 256 entries of the two arrays.
 * A voice is a SoundChannel (0x44 bytes, an embedded StaticSound at +0x20). Voice 0 is reserved for the silent loop that
 * keeps DirectSound awake (Sound_InitSilentLoop); handle 0 means "no voice". Sound_Play only claims and describes a voice;
 * the buffer is (re)created and started later by the mixer tick, which is what `owned` (+2) waits for: while it is still 1
 * the voice has not been started, and Sound_Stop/Sound_StopAll then forget the request instead of stopping the buffer.
 *
 * The mixer half (0x5495ad-): Sound_MixerTick runs once per frame and is where a requested voice's buffer is rebuilt, its
 * gain (distance to the camera x volume x g_sfxVolume) and frequency (sample rate x rate) pushed to DirectSound and the
 * buffer started; Sound_AllocChannel chooses the voice; pause/resume, rate, volume and the silent loop on voice 0
 * complete it.
 *
 * In data/structs, SoundChannel.sound is the embedded StaticSound and WaveFile has its last dword, so sizeof is 0x34.
 * The names are descriptive, not recovered (the binary has no symbols); local names were chosen for their stack slots
 * (see src/README.md).
 *
 * The static initialisers are what VC6 generates for the two definitions WaveFile g_sndWaveBank[256] and SoundChannel
 * g_soundChannels[24] (placed right after Load_EmptyStub_548dd0): $E4/$E1/$E3/$E2 and $E9/$E6/$E8/$E7, the array loops
 * being VC6's inline expansion of the vector constructor/destructor iterators; see the match-init line.
 */
/* BYTES: bss-name, dead-code, layout, slot-name, view. */
/* BYTES(bss-name): named for its .bss hash key 285 so it lands before g_soundChannels */
/* BYTES(layout): written '= 0' only to keep definition order in .bss, after the constructed arrays */
/* BYTES(view): SOUND_MODE() macro: optionBits read as a bitfield (shr / and on the byte at 0x5491ee), not a mask */
#define SDW_MEMBERS_WaveFile WaveFile();       /* 0x421740 WaveFile_Construct */
#define SDW_MEMBERS_StaticSound StaticSound(); /* 0x420680 StaticSound_Construct */
#include "sdw_enums.h"
#include "sdw_classes.h"

#include "../sdk/crt.h"

/* ---- the game's own helpers ---- */
#include "bs_io.h"
#include "fixed_math.h"
#include "progress.h"
#include "draw2d.h"
#include "../app/app_main.h"
#include "sfx_volume.h"
void Sound_SetRate(u16 handle, s32 fixed4_12);                                           /* 0x549665 */
void Sound_InitSilentLoop();                                                             /* 0x54988c */
void Sound_AllocChannel(u16 soundId, void *owner, u16 *outHandle);                       /* 0x54991f */
void Sound_SetDistanceFalloff(u32 nearDist, u32 farDist, float farGain, float nearGain); /* 0x549b16 */
void Sound_UpdateChannelGain(u16 ch);                                                    /* 0x549b61 */

/* ---- globals (addresses in SheepD3D.exe) ---- */

#define g_camPos (g_camera.pos) /* 0x584d20  the listener */

/* ---- this object's .bss (0x6deff0-0x6e3670) starts here ---- */
SndBankEntry g_sndBankEntries[256]; /* 0x6deff0  entry headers copied from the .SND image */

/* 0x548dd0 - empty, no callers. */
void Load_EmptyStub_548dd0() {}

/* 0x548dd5-0x548f3e: the static initialisers of the two arrays - for each, a root (the _initterm entries 0x57909c,
 * 0x5790a0), the constructor loop, the atexit registration and the destructor loop. VC6 generates all eight from the
 * two plain definitions below (Sound_StaticInit_BankWaves / _Channels and their Ctor / Atexit / Dtor partners); the
 * file's `match-init:` line places the two roots and the matcher follows their calls to the rest. */
WaveFile g_sndWaveBank[256];      /* 0x6dfbf0  one in-memory WaveFile per entry, stride 0x34 */
SoundChannel g_soundChannels[24]; /* 0x6e2ff0  the voices, stride 0x44; 0 is reserved */

/* ---- the rest of this object's .bss, in address order. Written `= 0` so that VC6 emits them after the three arrays and
 * in this order: it places a file's uninitialised globals (and those with constructors) first, sorted by a hash of
 * their names, and the ones explicitly initialised to zero after them in definition order. ---- */
u8 *g_pSndFile = 0;       /* 0x6e3650  the whole .SND image */
u16 g_sndBankCount = 0;   /* 0x6e3654  entries in the loaded bank */
u32 g_sndNearDist = 0;    /* 0x6e3658  positional gain = g_sndNearGain up to here (200) */
u32 g_sndFarDist = 0;     /* 0x6e365c  ... = g_sndFarGain from here on (5000) */
float g_sndNearGain = 0;  /* 0x6e3660  1.0 */
float g_sndFarGain = 0;   /* 0x6e3664  0.0 */
float g_sndGainSlope = 0; /* 0x6e3668  (near - far gain) / (near - far dist) */

/* 0x548f3f - loads the level's sound bank (.SND) and resets voices 1..23. 0, or -1 when the file is missing, empty or
 * not a "vdx7" bank. */
/* BYTES(slot-name): declared in this order for the original slots: result -4, cursor -8, length -0xc, hdr -0x10, ch -0x12, sndEntry -0x18, index -0x1a */
s32 Load_SND(char *path)
{
    /* declared in this order for the original slots: result -4, cursor -8, length -0xc, hdr -0x10, ch -0x12,
     * sndEntry -0x18, index -0x1a */
    u16 index;
    SndBankEntry *sndEntry;
    u16 ch;
    char *hdr;
    s32 length;
    u32 cursor;
    s32 result;

    result = -1;
    length = 0;
    /* cast kept (both): Bs_LoadFile returns the file as void * and its size as u32; length stays signed for length > 0 */
    g_pSndFile = (u8 *)Bs_LoadFile(path, (u32 *)&length);
    if (length > 0) {
        cursor = 4;
        hdr = (char *)g_pSndFile + cursor; /* cast kept: the .SND image is raw bytes until parsed (so below) */
        cursor += 8;
        if (strncmp(hdr, "vdx7", strlen("vdx7")) == 0) {
            g_sndBankCount = *(u16 *)(hdr + 4); /* cast kept: as above */
            for (index = 0; index < g_sndBankCount; index++) {
                sndEntry = (SndBankEntry *)(g_pSndFile + cursor); /* cast kept: as above */
                memcpy(&g_sndBankEntries[index], sndEntry, 12);
                cursor += 12;
                g_sndWaveBank[index].Open((char *)g_pSndFile + cursor, 1, sndEntry->dataSize); /* cast kept: as above */
                cursor += sndEntry->dataSize;
            }
            for (ch = 1; ch < 24; ch++) {
                g_soundChannels[ch].paused = g_soundChannels[ch].active = g_soundChannels[ch].owned =
                    g_soundChannels[ch].keepBuffer = 0;
                g_soundChannels[ch].owner = 0;
                g_soundChannels[ch].soundId = 0;
                g_soundChannels[ch].sampleSlot = 0;
                g_soundChannels[ch].playFlags = SNDF_NO_RETRIGGER;
                g_soundChannels[ch].volume = g_soundChannels[ch].mixedGain = g_soundChannels[ch].rate = 1.0f;
                g_soundChannels[ch].baseSampleRate = 0;
            }
            Sound_InitSilentLoop();
            Sound_SetDistanceFalloff(200, 5000, 0.0f, 1.0f);
            result = 0;
        }
    }
    return result;
}

/* 0x549155 - releases every voice's DirectSound buffer (channel 0 included) and frees the .SND image. */
void Sound_ShutdownChannels()
{
    u16 ch;

    for (ch = 0; ch < 24; ch++)
        g_soundChannels[ch].sound.Free();
    if (g_pSndFile) {
        delete g_pSndFile;
        g_pSndFile = 0;
    }
}

/* 0x5491b8 - requests sample soundId for owner: volume 0..255, flags a SoundPlayFlags set (1 loop, 8 no retrigger: an
 * active voice already playing this id for this owner is returned instead), rate 4.12 fixed. Returns the voice handle,
 * 0 when nothing was started (id 0x158 is always refused; so is everything in sound modes 1 and 3, an id missing from the
 * bank, or no free voice). */
/* BYTES(slot-name): declared in this order for the original slots: known -1, slot -4, handle -6 */
u16 Sound_Play(u16 soundId, void *owner, u16 volume, u8 flags, s32 rate)
{
    /* declared in this order for the original slots: known -1, slot -4, handle -6 */
    u16 handle;
    u16 slot;
    u8 known;

    handle = 0;
    slot = 0;
    known = 0;
    if (soundId == SND_RUMBLE)
        return handle;
    if (g_pProgress->optionBits.soundMode == SOUNDMODE_ALL || g_pProgress->optionBits.soundMode == SOUNDMODE_SFX_ONLY) {
        if (flags & SNDF_NO_RETRIGGER) {
            for (slot = 1; slot < 24; slot++) {
                if (g_soundChannels[slot].active == 1 && g_soundChannels[slot].owner == owner &&
                    g_soundChannels[slot].soundId == soundId)
                    return slot;
            }
        }
        slot = 0;
        while (slot < g_sndBankCount && !known) {
            if (g_sndBankEntries[slot].soundId == soundId)
                known = 1;
            else
                slot++;
        }
        if (!known) {
            handle = 0;
        } else {
            Sound_AllocChannel(soundId, owner, &handle);
            if (handle != 0) {
                g_soundChannels[handle].sound.Stop();
                g_soundChannels[handle].paused = 0;
                g_soundChannels[handle].active = 1;
                g_soundChannels[handle].owned = 1;
                g_soundChannels[handle].sound.looping =
                    ((flags & SNDF_LOOP) || g_sndBankEntries[slot].loop == 1) ? 1 : 0;
                g_soundChannels[handle].soundId = soundId;
                g_soundChannels[handle].sampleSlot = slot;
                g_soundChannels[handle].playFlags = flags;
                g_soundChannels[handle].owner = owner;
                g_soundChannels[handle].volume = volume / 255.0f;
                Sound_SetRate(handle, rate);
            }
        }
    }
    return handle;
}

/* 0x5493f2 - 1 while voice `handle` is playing; 0 for handle 0. */
s32 Sound_IsPlaying(u16 handle)
{
    s32 result;

    result = 0;
    if (handle != 0)
        result = g_soundChannels[handle].active == 1;
    return result;
}

/* 0x549425 - 1 if the first voice (1..23) holding soundId is active. Only the first match is looked at. */
/* BYTES(slot-name): declared in this order for the original slots: result -4, ch -6 */
s32 Sound_IsSampleIdPlaying(u16 soundId)
{
    /* declared in this order for the original slots: result -4, ch -6 */
    u16 ch;
    s32 result;

    result = 0;
    ch = 1;
    while (ch < 24 && g_soundChannels[ch].soundId != soundId)
        ch++;
    if (ch < 24 && g_soundChannels[ch].active == 1)
        result = 1;
    return result;
}

/* 0x54948f - stops voice `handle` if owner started it; a voice not yet started by the mixer is just forgotten. */
void Sound_Stop(u16 handle, void *owner)
{
    if (handle != 0 && g_soundChannels[handle].owner == owner) {
        if (!g_soundChannels[handle].owned)
            g_soundChannels[handle].sound.Stop();
        if (g_soundChannels[handle].owned) {
            g_soundChannels[handle].soundId = 0;
            g_soundChannels[handle].sampleSlot = 0;
        }
        g_soundChannels[handle].active = 0;
        g_soundChannels[handle].owned = 0;
    }
}

/* 0x549524 - Sound_Stop on voices 1..23 whoever owns them. */
void Sound_StopAll()
{
    u16 ch;

    for (ch = 1; ch < 24; ch++) {
        if (!g_soundChannels[ch].owned) {
            g_soundChannels[ch].sound.Stop();
        } else {
            g_soundChannels[ch].soundId = 0;
            g_soundChannels[ch].sampleSlot = 0;
        }
        g_soundChannels[ch].active = 0;
        g_soundChannels[ch].owned = 0;
    }
}

/* 0x5495ad - pauses every playing voice (1..23): the buffer is stopped where it is and `paused` remembers it. */
void Sound_PauseAll()
{
    u16 ch;

    for (ch = 1; ch < 24; ch++) {
        if (g_soundChannels[ch].active == 1) {
            g_soundChannels[ch].paused = 1;
            g_soundChannels[ch].sound.SetPaused(1);
        }
    }
}

/* 0x549609 - restarts the voices Sound_PauseAll paused. */
void Sound_ResumeAll()
{
    u16 ch;

    for (ch = 1; ch < 24; ch++) {
        if (g_soundChannels[ch].paused == 1) {
            g_soundChannels[ch].sound.SetPaused(0);
            g_soundChannels[ch].paused = 0;
        }
    }
}

/* 0x549665 - playback-rate multiplier of voice `handle`, 4.12 fixed (0x1000 = the sample's own rate); applied by the
 * next mixer tick. */
void Sound_SetRate(u16 handle, s32 fixed4_12)
{
    if (handle != 0)
        g_soundChannels[handle].rate = Math_Fixed12ToFloat_s32(fixed4_12);
}

/* 0x54968b - volume of voice `handle`, 0..255. */
void Sound_SetVolume(u16 handle, u16 volume)
{
    if (handle != 0)
        g_soundChannels[handle].volume = volume / 255.0f;
}

/* 0x5496b8 - once per frame. For each voice 1..23: one that is neither waiting to start nor still playing is marked
 * inactive; one waiting to start (owned == 1) gets its buffer re-created from the bank sample unless AllocChannel found
 * the buffer already holds it (keepBuffer); an active one gets its distance gain and its volume and frequency pushed to
 * DirectSound, and is (re)started if it is not playing. `owned` is cleared, so a voice is started once per Sound_Play. */
/* BYTES(slot-name): names chosen for their stack slots: ch -2, frequency -8 */
void Sound_MixerTick()
{
    /* slots: ch -2, frequency -8 */
    u16 ch;
    u32 frequency;

    for (ch = 1; ch < 24; ch++) {
        if (g_soundChannels[ch].owned == 0 && g_soundChannels[ch].sound.IsPlaying() == 0) {
            g_soundChannels[ch].active = 0;
        } else {
            if (g_soundChannels[ch].owned == 1 && g_soundChannels[ch].keepBuffer == 0) {
                g_soundChannels[ch].sound.Free();
                g_soundChannels[ch].sound.CreateFromWave(g_pSoundSystem,
                                                         &g_sndWaveBank[g_soundChannels[ch].sampleSlot]);
                frequency = g_soundChannels[ch].sound.baseFrequency;
                g_soundChannels[ch].baseSampleRate = frequency;
                g_soundChannels[ch].owned = 0;
            }
            if (g_soundChannels[ch].active == 1) {
                Sound_UpdateChannelGain(ch);
                g_soundChannels[ch].sound.Sound_SetBufferVolume(g_soundChannels[ch].mixedGain);
                g_soundChannels[ch].sound.Sound_SetBufferFrequency(
                    (u32)(g_soundChannels[ch].baseSampleRate * g_soundChannels[ch].rate));
            }
            if (g_soundChannels[ch].active == 1 && g_soundChannels[ch].sound.IsPlaying() == 0)
                g_soundChannels[ch].sound.Play();
            g_soundChannels[ch].owned = 0;
        }
    }
}

/* 0x54988c - voice 0 plays the bank's first sample, looped, at volume 0 (-60 dB, see Sound_SetBufferVolume): run while
 * nothing else plays, it keeps DirectSound from going idle (the stream player starts and stops it). */
void Sound_InitSilentLoop()
{
    g_soundChannels[0].volume = 0.0f;
    g_soundChannels[0].soundId = (u16)g_sndBankEntries[0].soundId;
    g_soundChannels[0].playFlags = SNDF_LOOP;
    g_soundChannels[0].owner = 0;
    g_soundChannels[0].sound.CreateFromWave(g_pSoundSystem, &g_sndWaveBank[0]);
    g_soundChannels[0].sound.Sound_SetBufferVolume(0.0f);
    g_soundChannels[0].sound.looping = 1;
}

/* 0x5498e1 */
void Sound_StartSilentLoop()
{
    if (!g_soundChannels[0].sound.IsPlaying())
        g_soundChannels[0].sound.Play();
}

/* 0x549901 */
void Sound_StopSilentLoop()
{
    g_soundChannels[0].sound.Stop();
}

/* 0x549910 - no callers. */
void Sound_StartStopSilentLoop_Dead()
{
    Sound_StartSilentLoop();
    Sound_StopSilentLoop();
}

/* 0x54991f - picks the voice for soundId/owner, in *outHandle (0 = none). In order: a voice never used (soundId and
 * sampleSlot 0); an idle voice already holding soundId - this owner's, else the first such (both keep the buffer); the
 * first idle voice; else the first playing voice that does not loop, which is stolen. keepBuffer tells the mixer
 * whether the DirectSound buffer must be rebuilt. With 23 looping voices playing there is no voice (handle 0). */
/* BYTES(slot-name): declared in this order for the original slots: rebuild -1, found -2, start -4, candidate -6 */
/* BYTES(dead-code): start is set and never read, as in the original */
void Sound_AllocChannel(u16 soundId, void *owner, u16 *outHandle)
{
    /* declared in this order for the original slots: rebuild -1, found -2, start -4 (set, never read), candidate -6 */
    u16 candidate;
    u16 start;
    u8 found;
    u8 rebuild;

    start = 1;
    candidate = 0;
    found = 0;
    rebuild = 1;
    *outHandle = 1;
    while (!found && *outHandle < 24) {
        if (g_soundChannels[*outHandle].soundId == 0 && g_soundChannels[*outHandle].sampleSlot == 0)
            found = 1;
        else
            (*outHandle)++;
    }
    if (!found) {
        *outHandle = 1;
        while (!found && *outHandle < 24) {
            if (g_soundChannels[*outHandle].soundId == soundId && g_soundChannels[*outHandle].active == 0) {
                if (g_soundChannels[*outHandle].owner == owner) {
                    found = 1;
                    rebuild = 0;
                } else {
                    if (candidate == 0)
                        candidate = *outHandle;
                    (*outHandle)++;
                }
            } else {
                (*outHandle)++;
            }
        }
        if (!found) {
            if (candidate != 0) {
                *outHandle = candidate;
                rebuild = 0;
            } else {
                *outHandle = 1;
                while (!found && *outHandle < 24) {
                    if (g_soundChannels[*outHandle].active == 0) {
                        found = 1;
                    } else {
                        if (candidate == 0 && g_soundChannels[*outHandle].sound.looping == 0)
                            candidate = *outHandle;
                        (*outHandle)++;
                    }
                }
                if (!found)
                    *outHandle = candidate;
                rebuild = 1;
            }
        }
    }
    if (*outHandle != 0)
        g_soundChannels[*outHandle].keepBuffer = rebuild == 0 ? 1 : 0;
}

/* 0x549b16 - positional gain: nearGain up to nearDist, farGain from farDist on, linear in between. The slope divides by
 * the UNSIGNED nearDist - farDist; VC6 builds the u32 as a 64-bit temporary but divides with a 32-bit fidiv, so the
 * negative difference is (luckily) read as signed and the slope comes out right: -1/4800 for (200, 5000, 0, 1). */
void Sound_SetDistanceFalloff(u32 nearDist, u32 farDist, float farGain, float nearGain)
{
    g_sndNearDist = nearDist;
    g_sndFarDist = farDist;
    g_sndFarGain = farGain;
    g_sndNearGain = nearGain;
    g_sndGainSlope = (nearGain - farGain) / (nearDist - farDist);
}

/* 0x549b61 - mixedGain = distance gain * volume * g_sfxVolume/255. The distance gain applies only to positional voices
 * (flag 2, not 0x10): the distance from the owner's position (ScnObject.pos) to the camera, horizontal only with flag 4. */
/* BYTES(slot-name): declared in this order for the original slots: delta -8 (x -8, y -6, z -4), len -0xc, src -0x14, gain -0x18 */
void Sound_UpdateChannelGain(u16 ch)
{
    /* declared in this order for the original slots: delta -8 (a Vec3s: x -8, y -6, z -4), len -0xc, src -0x14,
     * gain -0x18 */
    float gain;
    Vec3s src;
    u32 len;
    Vec3s delta;

    gain = 1.0f;
    if ((g_soundChannels[ch].playFlags & SNDF_POSITIONAL) && !(g_soundChannels[ch].playFlags & SNDF_NO_ATTENUATION)) {
        /* cast kept: a voice's owner is a void * (Sound_Play takes any owner); a positional voice's is a scenaric object */
        src = ((ScnObject *)g_soundChannels[ch].owner)->pos;
        if (g_soundChannels[ch].playFlags & SNDF_DIST_HORIZONTAL) {
            delta.x = src.x - g_camPos.x;
            delta.z = src.z - g_camPos.z;
            len = (u32)sqrt(delta.x * delta.x + delta.z * delta.z);
        } else {
            delta.x = src.x - g_camPos.x;
            delta.y = src.y - g_camPos.y;
            delta.z = src.z - g_camPos.z;
            len = (u32)sqrt(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);
        }
        if (len <= g_sndNearDist)
            gain = g_sndNearGain;
        else if (len >= g_sndFarDist)
            gain = g_sndFarGain;
        else
            gain = (s32)(len - g_sndFarDist) * g_sndGainSlope + g_sndFarGain;
    }
    g_soundChannels[ch].mixedGain = gain * g_soundChannels[ch].volume * (g_sfxVolume / 255.0f);
}
