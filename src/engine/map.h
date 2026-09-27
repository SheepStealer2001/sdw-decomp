#ifndef SDW_ENGINE_MAP_H
#define SDW_ENGINE_MAP_H

/* The functions and globals map.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

void Map_DrawWipeBar(u16 y, s16 height);
void Map_Init();  /* 0x53f430 */
s32 Map_IsOpen(); /* 0x5400f2 */
u16 Map_ResolvePropExportId(void *levelRecord, u16 propOffset);
void Map_Update();                  /* 0x53fe7f */
u8 SelectMenu_GetState();           /* 0x5428a0 returns g_selectMenuState */
s32 SelectMenu_HasItems();          /* 0x5428aa */
void SelectMenu_SetState(u8 state); /* 0x542893 sets g_selectMenuState */
void SelectMenu_UpdateWipe();       /* 0x542759 select-menu wipe in (state 1) / out (state 6) */

#endif
