/*
 * T306 - original object guessed as VideoSequence.cpp (data/tu_map.json). Ranges: .text 0x5615d0-0x5617bf, .data
 * 0x5816f8-0x5816fc (the "\\" literal), .bss 0x71b318-0x71c438 (g_fmvListCredits, g_fmvListIntro, the player object).
 *
 * The player (0x71c328) is g_vidPlayer here. The two FMV lists are defined here, and VC6 lays out a file's
 * uninitialised globals - a global with a constructor is one - by a hash of their names (key = (h ^ h>>16) & 0x3ff
 * ascending, h = h*4 + h>>4 + c) ahead of any zero-initialised ones. The player must come last, after the lists (keys
 * 851 and 893), but the descriptive name g_videoPlayer has key 759; g_vidPlayer has key 913. No other object
 * refers to it. The original name is unknown. The lists stay uninitialised for the same reason (a zero-initialised
 * global would be placed after the player).
 * SoundDevice::Init is declared (void *, u32, u8), as src/engine/sound_device.cpp defines it (same code; an HWND__ *
 * spelling would decorate to a name nothing defines).
 */
/* BYTES: bss-name, layout, slot-name. */
/* BYTES(bss-name): named for its .bss hash key 913 */
/* BYTES(layout): uninitialised on purpose: a zero-initialised global would be placed after the player */
/* PAL PC FMV sequence driver and natural global construction/destruction.
 * match-init: VideoPlayer_StaticInit
 * Default /Od recipe is deliberate: this caller is not in the optimized TU. */
#define SDW_MEMBERS_VideoPlayer VideoPlayer();
#define SDW_MEMBERS_SoundDevice SoundDevice();
#include "sdw_classes.h"
FmvList g_fmvListCredits; /* 0x71b318  played by Progress_GotoScene(-8) after the Ending scene */
FmvList g_fmvListIntro;   /* 0x71bb20  the three intro clips played at start-up */
VideoPlayer g_vidPlayer;  /* 0x71c328  the DirectShow FMV player (renamed, see the header) */
#include "../engine/stream_player.h"
#include "../engine/draw2d.h"
#include "../engine/sound_mgr.h"
extern HWND__ *g_hGameWindow;
#include "../sdk/win32.h"
#include "../sdk/crt.h"

/* BYTES(slot-name, inferred): local names chosen for their stack slots (tools/vc6_locals.py), not recovered */
u8 Video_PlaySequence(FmvList *list)
{
    u8 resultValue;
    u32 index;
    u32 rateData;
    char pathValue[256];
    u8 bits;
    resultValue = 1;
    index = 0;
    if (g_pStreamPlayer)
        g_pStreamPlayer->StopAndFree();
    Sound_ShutdownChannels();
    rateData = g_pSoundSystem->sampleRate;
    bits = g_pSoundSystem->bitsPerSample;
    delete g_pSoundSystem;
    CoInitialize(0);
    while (resultValue == 1 && index < list->count) {
        g_vidPlayer.Init(g_pD3DAppMain);
        strcpy(pathValue, g_pathDemoDir);
        strcat(pathValue, "\\");
        strcat(pathValue, list->clips[index]);
        resultValue = g_vidPlayer.PlayFile(pathValue, 1);
        ++index;
    }
    CoUninitialize();
    g_pSoundSystem = new SoundDevice;
    g_pSoundSystem->Init(g_hGameWindow, rateData, bits);
    return resultValue;
}
