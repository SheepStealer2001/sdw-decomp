#ifndef SDW_ENGINE_TIME_H
#define SDW_ENGINE_TIME_H

/* The functions and globals time.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

class Timer;

extern s32 g_dtRaw; /* 0x71b2cc */
extern u8 g_framesThisSecond;
extern Timer *g_pTimer; /* 0x71b2e4 */
extern u32 g_rawTime;   /* 0x71b2dc */
extern s32 g_rawTimeMs; /* 0x71b304 s32, as T304 defines it */
extern s32 g_renderWorldFlag;
void Time_Init();   /* 0x560f80 */
void Time_Pause();  /* 0x56103f */
void Time_Resume(); /* 0x561064 */
void Time_Update(); /* 0x56108c */

#endif
