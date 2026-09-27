/*
 * T315 - original object guessed as StreamPlayer.cpp (data/tu_map.json). Ranges: .rdata 0x5775e8-0x5775f0 (the COMDAT
 * float 1/255 and ??_7StreamPlayer), .data 0x581760-0x581860 (g_levelMusicClip, g_streamLoadIsMusic, then the /GF ??_C
 * string COMDATs), .bss 0x71cdb8-0x71ce38; its code is COMDATs only (/O2): 0x5634d0-0x5644e0, static initialiser
 * 0x5634d0. The object's .bss globals are defined here. VC6 orders a file's uninitialised globals (a global with a
 * constructor is one) by a hash of their names (key = (h ^ h>>16) & 0x3ff ascending, h = h*4 + h>>4 + c) and puts
 * zero-initialised ones after them in definition order; the exe's order g_pStreamPlayer, g_streamLoadPath,
 * g_streamWaves, then the rest, is reproduced by leaving the first three uninitialised (keys 407 < 560 < 793) and giving
 * the last four explicit "= 0".
 * Declarations follow the objects that define them: WaveFile::Open(char *, u8, u32) as src/engine/wave_file.cpp defines
 * it, g_rawTimeMs s32 as Time (T304) does.
 */
/* BYTES: flow, inline, layout, switches. */
/* BYTES(layout): uninitialised on purpose (hash key NNN) / written '= 0' only to keep definition order */
/* BYTES(switches): built with /O2 /Oy- /Ob2, not the project recipe; the file header says why */
/* match-flags: /O2 /Oy- /Ob2 */
/* match-init: StreamWaves_StaticInit */
/*
 * StreamPlayer - the streamed music / voice player (one instance, g_pStreamPlayer 0x71cdb8), SheepD3D.exe 0x5634d0-0x5644d2.
 *
 * Like the cinematic player this file was built OPTIMISED, /O2 with frame pointers kept (/Oy-): nop-padded 16-byte function
 * alignment, aligned loop tops, inlined strcpy/strcat/memcpy and register allocation all say so. /Ob2 (automatic inlining)
 * is recovered from the static initialiser 0x5634d0: the plain definition `WaveFile g_streamWaves[2];` compiles to exactly
 * 0x5634d0 and 0x563500 under /O2 /Oy- /Ob2, while under /Ob1 VC6 splits the constructor loop and the atexit registration
 * into two functions. Every other function here compiles the same either way.
 *
 * Two DirectSound StreamSounds are double-buffered: the playing one is `current` (= &streams[activeSlot]) and a loader
 * thread (StreamPlayer_LoaderThread) opens the next clip into the idle one, (activeSlot - 1) & 1, under the mutex
 * "SeekMutex". The clip names come from References\MusicVoice.BSV (Load_MusicVoiceBank): music is Musics\<name>, one track
 * per level (g_levelMusicClip); a voice line is Voices\<Language>/<name>.
 * The state machine (state, +0x33c):
 *   0 idle (the silent loop keeps the sound device busy)   1 music playing   2 music loading
 *   5 fading the music out for a voice line (1 s)          6 waiting for the voice to load (at least 2 s after the request)
 *   3 voice playing: when it ends the level's music is reloaded (-> 2)           4 is never set (IsBusy still tests it)
 *
 * Devices that pin the original code generation (not claims about the source text):
 *  - LoaderThread stores g_streamLoadStatus = 1 once after the music/voice if-else; VC6 copies the store into both arms.
 *    Written in both arms, the constant 1 is kept in EBX instead.
 *  - CloseLoader() as its own inline, used inside LoaderResult() and after it: written out in place, VC6 loads the two
 *    hoisted import pointers (ReleaseMutex, CloseHandle) of UpdateVoice in the other order.
 */
#define SDW_MEMBERS_WaveFile WaveFile();         /* 0x421740 WaveFile_Construct */
#define SDW_MEMBERS_StreamSound StreamSound();   /* 0x420bd0 StreamSound_Construct */
#define SDW_MEMBERS_StreamPlayer StreamPlayer(); /* 0x563640 StreamPlayer_Construct */
#include "sdw_enums.h"
#include "../sdk/win32.h"
#include "sdw_classes.h"

#include "../sdk/crt.h"

/* ---- globals ---- */
#include "progress.h"
#include "draw2d.h"
#include "time.h"
#include "sound_mgr.h"
#include "bs_io.h"
/* This object's .bss (0x71cdb8-0x71ce38), defined here. VC6 lays out the uninitialised globals first, ordered by a hash
 * of their names, then the zero-initialised ones in definition order (see the header). g_streamWaves has constructors,
 * so it is uninitialised and lands after the two globals whose names hash below it; the four that follow it carry
 * explicit zero initialisers. (g_streamWaves itself is defined below, with its initialiser.) */
StreamPlayer *g_pStreamPlayer;    /* 0x71cdb8  (uninitialised: hash key 407) */
char *g_streamLoadPath;           /* 0x71cdbc  the path the loader thread is opening (uninitialised: key 560) */
u32 g_streamActiveSlot = 0;       /* 0x71ce28  mirror of StreamPlayer.activeSlot */
HANDLE g_hStreamLoaderThread = 0; /* 0x71ce2c */
HANDLE g_hSeekMutex = 0;          /* 0x71ce30  "SeekMutex", held by the loader while it opens a clip */
s32 g_streamLoadStatus = 0;       /* 0x71ce34  0 pending, 1 loaded, -1 WaveFile_Open failed */

/* This file's initialised data (.data 0x581760..): the music clip of each level (1-based index into the clip bank, read as
 * a word), then the loader's music/voice switch. */
s32 g_levelMusicClip[18] = {
    VOICE_JUNGLE_V1,        VOICE_TRIP_V1,           VOICE_JAMI_FULL,    VOICE_MIX_JAMI_V2, VOICE_JAZZ90_V3,
    VOICE_TRIP_V1,          VOICE_LUNEY_V2,          VOICE_LUNEY_V2,     VOICE_ZILLA_V2,    VOICE_JUNGLERHODES01_V2,
    VOICE_ROAD_MARECAGE_V2, VOICE_ROAD_MARECAGE_V2,  VOICE_ZILLA_V2,     VOICE_JAZZ90_V3,   VOICE_DNBASS_V2,
    VOICE_NEWS1LAST_V2,     VOICE_JUNGLETUTORIAL_V2, VOICE_FRONZYLAST_V3};
u8 g_streamLoadIsMusic = 1; /* 0x5817a8  1: the clip being loaded is music (looping, volume +0x95), 0: a voice */

DWORD __stdcall StreamPlayer_LoaderThread(void *path);

/* Source-only helpers, inlined at every use (neither has an address of its own; descriptive names). */
/* Close the loader thread's handle (the thread itself is left to run to its end). */
/* BYTES(inline): source-only inline: written out in place, VC6 hoists ReleaseMutex / CloseHandle of UpdateVoice in the other order */
static inline void CloseLoader()
{
    CloseHandle(g_hStreamLoaderThread);
    g_hStreamLoaderThread = 0;
}

/* If the loader thread has let go of the mutex, take its result (1 loaded, -1 failed) and close its handle; 0 while it is
 * still busy (or has not started yet: the status is reset to 0 before each thread starts). */
static inline s32 LoaderResult()
{
    s32 status = STREAM_LOAD_PENDING;
    if (WaitForSingleObject(g_hSeekMutex, 0) == WAIT_OBJECT_0) {
        status = g_streamLoadStatus;
        ReleaseMutex(g_hSeekMutex);
        CloseLoader();
    }
    return status;
}

/* 0x5634d0 StreamWaves_StaticInit and 0x563500 StreamWaves_StaticDtor: what VC6 /O2 generates for this definition (the
 * constructor loop and the atexit registration inlined into the root, the destructor loop in its partner); the
 * `match-init:` line at the top places the root. */
WaveFile g_streamWaves[2]; /* 0x71cdc0  one WaveFile per stream slot (uninitialised: key 793) */

/* 0x563520 - the loader thread: open the clip into the idle slot's WaveFile, (re)create that slot's stream on it and set
 * its volume and looping from g_streamLoadIsMusic; the result goes to g_streamLoadStatus, all under the mutex. */
/* BYTES(flow): the store of 1 is written in both arms: written once after the if-else, VC6 copies it itself and does not keep 1 in EBX */
DWORD __stdcall StreamPlayer_LoaderThread(void *path)
{
    g_streamLoadPath = (char *)path; /* cast kept: CreateThread hands the thread its argument as a void * */
    u32 slot = (g_streamActiveSlot - 1) & 1;
    while (WaitForSingleObject(g_hSeekMutex, INFINITE) != WAIT_OBJECT_0)
        ;
    g_streamWaves[slot].Close();
    if (g_streamWaves[slot].Open((char *)path, 0, 0) >= 0) { /* cast kept: the thread's void * argument */
        StreamSound *s = &g_pStreamPlayer->streams[(g_pStreamPlayer->activeSlot - 1) & 1];
        s->Free();
        s->CreateFromWaveEx(g_pSoundSystem, &g_streamWaves[slot], 2.0f, 16);
        if (g_streamLoadIsMusic == 1) {
            s->Sound_SetBufferVolume(g_pProgress->streamVolumeA * (1.0f / 255.0f));
            s->looping = 1;
        } else {
            s->Sound_SetBufferVolume(g_pProgress->streamVolumeB * (1.0f / 255.0f));
            s->looping = 0;
        }
        g_streamLoadStatus = STREAM_LOAD_OK;
    } else {
        g_streamLoadStatus = STREAM_LOAD_FAILED;
    }
    ReleaseMutex(g_hSeekMutex);
    return 1;
}

/* 0x563640 */
StreamPlayer::StreamPlayer()
{
    bankLoaded = 0;
    currentClip = 0;
    clipCount = 0;
    clipNames = 0;
    pausedState = STREAM_ST_IDLE;
    state = STREAM_ST_IDLE;
    stateStartMs = 0;
    activeSlot = 0;
    current = &streams[activeSlot];
    autoPlay = 1;
}

/* 0x5636b0 (StreamPlayer_ScalarDeletingDtor, compiler-generated) - the destructor is inline, so its body appears only
 * there: free the clip bank, then the two streams are destroyed. */
inline StreamPlayer::~StreamPlayer()
{
    if (clipNames) {
        for (u16 i = 0; i < clipCount; i++)
            delete clipNames[i];
        delete clipNames;
        clipNames = 0;
    }
}

/* 0x563740 - read References\MusicVoice.BSV: a 12-byte magic "BSV_FILEV2.5", then 116 {u32 size; char name[size]}
 * entries, each copied to its own allocation (NULL for size 0). */
void StreamPlayer::Load_MusicVoiceBank()
{
    char path[256];
    u32 size = 0;
    char *magic;
    strcpy(path, g_dirReference);
    strcat(path, "MusicVoice.BSV");
    char *file = (char *)Bs_LoadFile(path, &size); /* cast kept: Bs_LoadFile returns the file's bytes untyped */
    clipCount = 0;
    if ((s32)size > 0) {
        magic = new char[12];
        strncpy(magic, file, 12);
        if (strncmp(magic, "BSV_FILEV2.5", 12) != 0) {
            MessageBoxA(0, "Could not load file, check version", "BSV File Error", MB_ICONHAND);
        } else {
            u32 pos = 12;
            clipCount = 0x74;
            clipNames = new char *[0x74];
            for (u16 i = 0; i < clipCount; i++) {
                u32 len = *(u32 *)(file + pos); /* cast kept: a length word at a byte offset of the raw file */
                pos += 4;
                if (len != 0) {
                    clipNames[i] = new char[len];
                    memcpy(clipNames[i], file + pos, len);
                    pos += len;
                } else {
                    clipNames[i] = 0;
                }
            }
            activeSlot = 0;
            g_streamActiveSlot = 0;
            current = &streams[activeSlot];
            bankLoaded = 1;
        }
    }
    if (magic)
        delete magic;
    if (file)
        delete file;
    state = STREAM_ST_IDLE;
}

/* 0x563920 - stop and release the current stream (Video_PlaySequence, before it destroys the sound device). */
void StreamPlayer::StopAndFree()
{
    current->Stop();
    current->Free();
    state = STREAM_ST_IDLE;
}

/* 0x563940 - request a voice line: build .\Voices\<Language>/<clip name> and start the loader on it; the music fades out
 * meanwhile (state 5). The language argument is not used - the switch reads the progress object again. */
void StreamPlayer::PlayVoice(u32 clip, u8 language, u32 unused, u32 lipSync)
{
    currentClip = clip;
    if (g_pSoundSystem->initialized != 1)
        return;
    if (g_pProgress->optionFlags & OPT_SOUNDMODE_MASK)
        return;
    strcpy(path, g_voiceDir);
    switch (g_pProgress->language) {
        case GAME_LANG_FRENCH:
            strcat(path, "French/");
            break;
        case GAME_LANG_SPANISH:
            strcat(path, "Spanish/");
            break;
        case GAME_LANG_ITALIAN:
            strcat(path, "Italian/");
            break;
        case GAME_LANG_GERMAN:
            strcat(path, "German/");
            break;
        case GAME_LANG_DUTCH:
            strcat(path, "Dutch/");
            break;
        case GAME_LANG_BRAZILIAN:
            strcat(path, "Brazilian/");
            break;
        case GAME_LANG_ENGLISH:
        default:
            strcat(path, "English/");
            break;
    }
    strcat(path, clipNames[currentClip - 1]);
    g_pSoundSystem->UnregisterStreamEvent(current);
    g_streamLoadIsMusic = 0;
    g_streamLoadStatus = STREAM_LOAD_PENDING;
    g_hSeekMutex = CreateMutexA(0, 0, "SeekMutex");
    DWORD threadId;
    g_hStreamLoaderThread = CreateThread(0, 0, StreamPlayer_LoaderThread, path, 0, &threadId);
    SetPriorityClass(g_hStreamLoaderThread, NORMAL_PRIORITY_CLASS);
    SetThreadPriority(g_hStreamLoaderThread, THREAD_PRIORITY_ABOVE_NORMAL);
    stateStartMs = g_rawTimeMs;
    state = STREAM_ST_VOICE_FADE_MUSIC;
    playArg = lipSync;
}

/* 0x563bd0 - the voice half of the per-frame update: fade the music out (5), wait for the voice and swap it in (6),
 * and when the voice has finished reload the level's music (3). Returns 1 while it is fading or has just swapped. */
s32 StreamPlayer::UpdateVoice()
{
    s32 result = 0;
    switch (state) {
        case STREAM_ST_VOICE_PLAYING:
            if ((u32)(g_rawTimeMs - stateStartMs) > 2000) {
                if (LoaderResult() == STREAM_LOAD_PENDING) {
                    Time_Pause();
                    while (LoaderResult() == STREAM_LOAD_PENDING)
                        ;
                    Time_Resume();
                }
                if (LoaderResult() == STREAM_LOAD_OK) {
                    CloseLoader();
                    activeSlot = (activeSlot - 1) & 1;
                    current = &streams[activeSlot];
                    g_streamActiveSlot = activeSlot;
                    Sound_StopSilentLoop();
                    if (autoPlay == 1)
                        current->Play(playArg);
                    result = 1;
                    state = STREAM_ST_VOICE_DONE;
                } else if (LoaderResult() == STREAM_LOAD_FAILED) {
                    current->Stop();
                    state = STREAM_ST_VOICE_DONE;
                }
            }
            break;
        case STREAM_ST_VOICE_FADE_MUSIC: {
            u32 elapsed = g_rawTimeMs - stateStartMs;
            if (elapsed < 1000) {
                u8 vol = g_pProgress->streamVolumeA;
                u8 faded = vol - vol * elapsed / 1000;
                current->Sound_SetBufferVolume(faded * (1.0f / 255.0f));
                result = 1;
            } else {
                current->Stop();
                current->Free();
                result = 1;
                state = STREAM_ST_VOICE_PLAYING;
            }
            break;
        }
        case STREAM_ST_VOICE_DONE:
            if (!current->IsPlaying()) {
                current->Free();
                s8 level = g_pProgress->currentLevel;
                if (g_pSoundSystem->initialized == 1 && !(g_pProgress->optionFlags & OPT_SOUNDMODE_MASK)) {
                    if (level < SCENE_LVL_00 || level > SCENE_LVL_17)
                        level = SCENE_LVL_00;
                    current->Stop();
                    currentClip = g_levelMusicClip[level] - 1;
                    strcpy(path, g_musicsPath);
                    strcat(path, clipNames[currentClip]);
                    stateStartMs = g_rawTimeMs;
                    state = STREAM_ST_LOADING;
                    g_streamLoadIsMusic = 1;
                    g_streamLoadStatus = STREAM_LOAD_PENDING;
                    g_hSeekMutex = CreateMutexA(0, 0, "SeekMutex");
                    DWORD threadId;
                    g_hStreamLoaderThread = CreateThread(0, 0, StreamPlayer_LoaderThread, path, 0, &threadId);
                    SetPriorityClass(g_hStreamLoaderThread, NORMAL_PRIORITY_CLASS);
                    SetThreadPriority(g_hStreamLoaderThread, THREAD_PRIORITY_ABOVE_NORMAL);
                    current->looping = 1;
                }
            }
            break;
    }
    return result;
}

/* 0x563f90 - cut a voice line short (voice states 3, 5, 6 only): drop the loader's thread handle if it has not finished,
 * stop the current stream and go to 3, from where the next update reloads the music. The loader thread itself is not
 * stopped. */
void StreamPlayer::StopVoice()
{
    if (state == STREAM_ST_VOICE_PLAYING || state == STREAM_ST_VOICE_FADE_MUSIC || state == STREAM_ST_VOICE_DONE) {
        if (LoaderResult() == STREAM_LOAD_PENDING)
            CloseLoader();
        current->Stop();
        state = STREAM_ST_VOICE_DONE;
    }
}

/* 0x564020 - (re)start the level's music: stop the current stream and load the level's track in the background. */
void StreamPlayer::RestartLevelMusic()
{
    s8 level = g_pProgress->currentLevel;
    if (g_pSoundSystem->initialized == 1 && !(g_pProgress->optionFlags & OPT_SOUNDMODE_MASK)) {
        if (level < SCENE_LVL_00 || level > SCENE_LVL_17)
            level = SCENE_LVL_00;
        current->Stop();
        currentClip = g_levelMusicClip[level] - 1;
        strcpy(path, g_musicsPath);
        strcat(path, clipNames[currentClip]);
        stateStartMs = g_rawTimeMs;
        state = STREAM_ST_LOADING;
        g_streamLoadIsMusic = 1;
        g_streamLoadStatus = STREAM_LOAD_PENDING;
        g_hSeekMutex = CreateMutexA(0, 0, "SeekMutex");
        DWORD threadId;
        g_hStreamLoaderThread = CreateThread(0, 0, StreamPlayer_LoaderThread, path, 0, &threadId);
        SetPriorityClass(g_hStreamLoaderThread, NORMAL_PRIORITY_CLASS);
        SetThreadPriority(g_hStreamLoaderThread, THREAD_PRIORITY_ABOVE_NORMAL);
        current->looping = 1;
    }
}

/* 0x564160 - the same at level load (Load_DAVnWAR), but it starts the silent loop, releases the current stream instead
 * of stopping it, and waits for the loader to finish. */
void StreamPlayer::LoadLevelMusic()
{
    s8 level = g_pProgress->currentLevel;
    if (g_pSoundSystem->initialized == 1 && !(g_pProgress->optionFlags & OPT_SOUNDMODE_MASK)) {
        Sound_StartSilentLoop();
        if (level < SCENE_LVL_00 || level > SCENE_LVL_17)
            level = SCENE_LVL_00;
        current->Free();
        currentClip = g_levelMusicClip[level] - 1;
        strcpy(path, g_musicsPath);
        strcat(path, clipNames[currentClip]);
        stateStartMs = g_rawTimeMs;
        state = STREAM_ST_LOADING;
        g_streamLoadIsMusic = 1;
        g_streamLoadStatus = STREAM_LOAD_PENDING;
        g_hSeekMutex = CreateMutexA(0, 0, "SeekMutex");
        DWORD threadId;
        g_hStreamLoaderThread = CreateThread(0, 0, StreamPlayer_LoaderThread, path, 0, &threadId);
        SetPriorityClass(g_hStreamLoaderThread, NORMAL_PRIORITY_CLASS);
        SetThreadPriority(g_hStreamLoaderThread, THREAD_PRIORITY_ABOVE_NORMAL);
        current->looping = 1;
        while (LoaderResult() == STREAM_LOAD_PENDING)
            ;
    }
}

/* 0x5642e0 - pause (1: remember the state, go idle, stop the stream) or resume (restore it and replay). */
void StreamPlayer::SetPaused(s32 pause)
{
    if (pause == 1) {
        pausedState = state;
        state = STREAM_ST_IDLE;
        autoPlay = 0;
        current->Stop();
    } else {
        state = pausedState;
        autoPlay = 1;
        if (state != STREAM_ST_IDLE)
            current->Play(0);
    }
}

/* 0x564340 - stop the current stream and go idle. */
void StreamPlayer::Halt()
{
    current->Stop();
    state = STREAM_ST_IDLE;
}

/* 0x564360 - the per-frame update (Game_Frame): the voice states first, then idle keeps the silent loop going and a
 * finished music load is swapped in and played (-> 1). */
void StreamPlayer::Update()
{
    UpdateVoice();
    switch (state) {
        case STREAM_ST_LOADING:
            if (LoaderResult() == STREAM_LOAD_OK) {
                CloseLoader();
                activeSlot = (activeSlot - 1) & 1;
                current = &streams[activeSlot];
                g_streamActiveSlot = activeSlot;
                Sound_StopSilentLoop();
                if (autoPlay == 1)
                    current->Play(0);
                state = STREAM_ST_PLAYING;
            }
            break;
        case STREAM_ST_IDLE:
            Sound_StartSilentLoop();
            break;
    }
}

/* 0x564430 - re-apply the volume setting to the current stream: music volume in state 1, voice volume in 3 and 6. */
void StreamPlayer::ApplyVolume()
{
    switch (state) {
        case STREAM_ST_VOICE_DONE:
        case STREAM_ST_VOICE_PLAYING:
            current->Sound_SetBufferVolume(g_pProgress->streamVolumeB * (1.0f / 255.0f));
            break;
        case STREAM_ST_PLAYING:
            current->Sound_SetBufferVolume(g_pProgress->streamVolumeA * (1.0f / 255.0f));
            break;
    }
}

/* 0x564490 - 1 in the music states (1, 2). There is no return for the other states: EAX still holds the state, which
 * is what Voice_PlayStream 0x515e50 ends up testing (0 or 1 lets a voice start). */
#pragma warning(disable : 4715)
/* BYTES(flow): no return on the other states, as in the original: EAX keeps the state (C4715 disabled for this) */
s32 StreamPlayer::IsMusic()
{
    if (state == STREAM_ST_LOADING || state == STREAM_ST_PLAYING)
        return 1;
}

/* 0x5644b0 - 1 while a voice line owns the player (states 3..6): Game_Frame keeps the menus shut then. */
s32 StreamPlayer::IsBusy()
{
    if (state == STREAM_ST_VOICE_DONE || state == STREAM_ST_UNUSED_4 || state == STREAM_ST_VOICE_FADE_MUSIC ||
        state == STREAM_ST_VOICE_PLAYING)
        return 1;
    return 0;
}
