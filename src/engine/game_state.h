#ifndef SDW_ENGINE_GAME_STATE_H
#define SDW_ENGINE_GAME_STATE_H

/* The functions and globals game_state.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

struct Dav;

extern s32 g_animDt;        /* 0x6ddf5c also the start of the GameState block */
extern u8 g_camDebugMode;   /* 0x6ddf79 */
extern s32 g_fadeTimer;     /* 0x6ddf70 1/4096 s */
extern u8 g_letterboxState; /* 0x6ddf78 0 idle, 1 animating, 2 fully extended */
extern Dav *g_pDav;         /* 0x6ddf6c */
extern u16 g_weatherType;   /* 0x6ddf6a */

/* The functions and globals game_state.cpp defines, declared once for every file that uses them. */

extern u16 g_texScrollListIds[4]; /* 0x6ddf60 WAR type 0x83 (the table names the block by its first field) */

#endif
