#ifndef SDW_ENGINE_SOUND_MGR_H
#define SDW_ENGINE_SOUND_MGR_H

/* The functions and globals sound_mgr.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

int Load_SND(char *path);                                                                /* 0x548f3f */
void Sound_AllocChannel(u16 soundId, void *owner, u16 *outHandle);                       /* 0x54991f */
void Sound_InitSilentLoop();                                                             /* 0x54988c */
s32 Sound_IsPlaying(u16 handle);                                                         /* 0x5493f2 */
s32 Sound_IsSampleIdPlaying(u16);                                                        /* 0x549425 */
void Sound_MixerTick();                                                                  /* 0x5496b8 */
void Sound_PauseAll();                                                                   /* 0x5495ad */
void Sound_ResumeAll();                                                                  /* 0x549609 */
void Sound_SetDistanceFalloff(u32 nearDist, u32 farDist, float farGain, float nearGain); /* 0x549b16 */
void Sound_SetRate(u16 handle, s32 fixed4_12);                                           /* 0x549665 */
void Sound_SetVolume(u16, u16);                                                          /* 0x54968b */
void Sound_ShutdownChannels();                                                           /* 0x549155 */
void Sound_StartSilentLoop();                                                            /* 0x5498e1 */
void Sound_Stop(u16 handle, void *owner);                                                /* 0x54948f */
void Sound_StopAll();                                                                    /* 0x549524 */
void Sound_StopSilentLoop();                                                             /* 0x549901 */
void Sound_UpdateChannelGain(u16 ch);                                                    /* 0x549b61 */

#endif
