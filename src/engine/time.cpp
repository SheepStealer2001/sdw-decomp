/*
 * T304 - original object guessed as Time.cpp (data/tu_map.json). Ranges: .text 0x560f80-0x561200, .rdata
 * 0x5775d0-0x5775d8 (the COMDAT double 4096.0 of Time_Update), .bss 0x71b2c8-0x71b30c (the clock globals): the Time_*
 * functions 0x560f80-0x5611ff (Rand_Reset .. Vec3i_SetLength after them are T305). Time unit: 1/4096 second.
 * .bss: the clock globals are defined here in address order with explicit "= 0" initialisers: VC6 keeps a zero-initialised
 * global in DEFINITION order, while it would order uninitialised ones by a hash of their names. The original spelling is
 * unknown.
 * Types: g_gameTime and g_rawTime are defined u32 (what the cinematic objects compare), the other clock globals s32; the
 * code here is the same either way, and every other object declares them with these types.
 */
/* BYTES: layout. */
/* BYTES(layout): written '= 0' only to keep definition order in .bss */
#include "sdw_enums.h"
#include "timer.h"
#include "sdw_classes.h"

/* ---- this object's .bss, in address order ---- */
s32 g_frameCount = 0;         /* 0x71b2c8  never read by the game                                  */
s32 g_dtRaw = 0;              /* 0x71b2cc  unscaled frame delta, trunc(seconds * 4096)            */
u32 g_gameTime = 0;           /* 0x71b2d0  u32: the type most of the game's declarations use (21 of 29) */
s32 g_gameTimeMs = 0;         /* 0x71b2d4                                                         */
s32 g_dtRawMs = 0;            /* 0x71b2d8                                                         */
u32 g_rawTime = 0;            /* 0x71b2dc  u32 (the cinematic player compares it unsigned)       */
s32 g_timeScale = 0;          /* 0x71b2e0  4.12; only ever written as 0x1000                      */
Timer *g_pTimer = 0;          /* 0x71b2e4  the gameplay clock's Timer, made by Time_Init          */
s32 g_dtMs = 0;               /* 0x71b2e8                                                         */
double g_timePausedDelta = 0; /* 0x71b2f0  negated unconsumed timer delta, Time_Pause -> Time_Resume */
s32 g_renderWorldFlag =
    0; /* 0x71b2f8  only ever written (1 here; 0/1 at 0x49bf12/0x49c9f9 and 0x4d47ce/0x4d4917, beside GF_RENDER_WORLD), never read by address */
u8 g_framesThisSecond = 0; /* 0x71b2fc  frame index inside the current game-second (sheep AI)  */
s32 g_dt = 0;              /* 0x71b300  frame delta used by gameplay, clamped to 0..0xAA       */
s32 g_rawTimeMs = 0;       /* 0x71b304  the level time                                          */
s32 g_frameCount2 = 0;     /* 0x71b308  only read by two HUD blink functions                   */

/* ---- elsewhere ---- */
extern u32 g_gameFlags; /* 0x6ddf74  bit 0x40 = paused                                      */
#include "game_state.h"

/* 0x560f80 - on every level load (its one caller is Load_DAVnWAR). Builds and stops a new gameplay Timer (so the first
 * Time_Update delta is 0) and zeroes the clock; the previous Timer is not deleted. */
void Time_Init()
{
    g_pTimer = new Timer;
    g_pTimer->Stop();
    g_timeScale = 0x1000;
    g_renderWorldFlag = 1;
    g_framesThisSecond = 0;
    g_dt = 0;
    g_dtRaw = 0;
    g_gameTime = 0;
    g_rawTime = 0;
    g_dtMs = 0;
    g_dtRawMs = 0;
    g_gameTimeMs = 0;
    g_rawTimeMs = 0;
}

/* 0x56102f - no callers. */
void Time_RestartTimer()
{
    g_pTimer->Start();
}

/* 0x56103f - saves the not-yet-consumed delta (negated) and stops the timer. Not the whole pause-menu path. */
void Time_Pause()
{
    g_timePausedDelta = -g_pTimer->GetDelta(TIMER_TICKS);
    g_pTimer->Stop();
}

/* 0x561064 - restarts and shifts the base back by the saved amount, so the suspended interval is not counted. */
void Time_Resume()
{
    g_pTimer->Start();
    g_pTimer->OffsetBase(g_timePausedDelta);
}

#define SDW_ABS(v) ((v) >= 0 ? (v) : -(v))

/* 0x56108c - called once per frame from Main_Loop (0x401614). */
void Time_Update(void)
{
    s32 timer_enabled = 1;
    s32 new_time;
    /* The fixed-step alternative remains in the PC instructions. */
    if (timer_enabled)
        g_dtRaw = (s32)(g_pTimer->GetDelta(TIMER_SECONDS) * 4096.0);
    else
        g_dtRaw = 0x88;

    if (g_gameFlags & GF_PAUSED)
        g_dt = 0;
    else
        g_dt = (g_dtRaw * g_timeScale) >> 12;
    if (timer_enabled) {
        if (g_dt > 0xAA)
            g_dt = 0xAA;
        else if (g_dt < 0)
            g_dt = 0xAA;
    }

    g_animDt = (g_dt * 1000) >> 2;
    g_dtRawMs = (g_dtRaw * 1000) >> 12; /* truncated a second time */
    g_dtMs = (g_dt * 1000) >> 12;

    new_time = g_gameTime + g_dt;
    if ((g_gameTime & 0xFFFFF000) != (new_time & 0xFFFFF000))
        g_framesThisSecond = 0;
    else
        g_framesThisSecond++;

    g_gameTime = new_time;
    g_gameTimeMs += g_dtMs;
    g_frameCount2 += 1;
    g_rawTime += g_dtRaw;
    g_rawTimeMs += g_dtRawMs;
    g_frameCount += 1;
}
