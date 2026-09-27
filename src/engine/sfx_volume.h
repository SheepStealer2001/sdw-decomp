#ifndef SDW_ENGINE_SFX_VOLUME_H
#define SDW_ENGINE_SFX_VOLUME_H

/* The functions and globals sfx_volume.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

extern u8 g_sfxVolume;         /* 0x71cdb0 */
void Sound_SetSfxVolume(u8 v); /* 0x5634c0 */

#endif
