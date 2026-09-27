/*
 * T266 - guessed original name: Cheat.cpp. SheepD3D.exe .text 0x52dd70-0x52df77, .data 0x57bc50-0x57bc7c,
 * .bss 0x6d82fc-0x6d8318.
 * The pad cheat sequences (0x52dd70-0x52de45, built `#pragma optimize("g", on)`) and the countdown HUD of a cut timed
 * mode (0x52de50-0x52df76).
 * Data in place: the two button sequences and the two format strings ($SG, .data), the cheat state and the countdown
 * (.bss).
 */
/* BYTES: dead-code, layout, slot-name, switches. */
/* BYTES(layout): written '= 0' only to keep definition order in .bss */
/* BYTES(switches): Cheat_AdvanceSequence .. Cheat_Poll (0x52dd70-0x52de45): built with global optimisation (#pragma optimize("g")): only /Og reproduces these functions */
#include "sdw_types.h"
#include "sdw_enums.h"

#include "sdw_classes.h"

#include "text.h"
#include "input.h"

extern s32 g_dtMs;             /* 0x71b2e8 */
extern u32 *g_screenLayerBase; /* 0x585044 */

/* ---- .data ---- */
/* active-low pad words: one button down each */
u16 g_cheatSeqMystery[6] = {(u16)~PAD_SQUARE,   (u16)~PAD_SQUARE, (u16)~PAD_TRIANGLE,
                            (u16)~PAD_TRIANGLE, (u16)~PAD_CIRCLE, (u16)~PAD_CIRCLE}; /* 0x57bc50 */
u16 g_cheatSeqLevelSkip[7] = {(u16)~PAD_SQUARE,   (u16)~PAD_SQUARE, (u16)~PAD_TRIANGLE, (u16)~PAD_SQUARE,
                              (u16)~PAD_TRIANGLE, (u16)~PAD_CIRCLE, (u16)~PAD_CIRCLE}; /* 0x57bc5c */

/* ---- .bss ----
 * Explicit `= 0`: VC6 emits zero-initialised globals to .bss in DEFINITION order, while plain uninitialised ones are
 * ordered by a hash of their names ((h ^ h >> 16) & 0x3ff), which with these names would put g_cheatLevelSkipProgress
 * first. A representation of the original's order, not a claim about its spelling. */
s32 g_cheatSeqTimer = 0;          /* 0x6d82fc */
s32 g_cheatMysteryProgress = 0;   /* 0x6d8300 */
s32 g_cheatMysteryFlag = 0;       /* 0x6d8304 */
s32 g_cheatLevelSkipProgress = 0; /* 0x6d8308 */
s32 g_cheatLevelSkip = 0;         /* 0x6d830c */
s32 g_countdownMs = 0;            /* 0x6d8310 */

/* ---- 0x52dd70-0x52de45: the pad cheat sequences (OPTIMISED) ---- */
#pragma optimize("g", on)
/* 0x52dd70 - advances *progress when this frame's new button word is the next one of seq; 1 once all len matched.
 * The inter-press timeout g_cheatSeqTimer is shared by both sequences and accumulated once per CALL. */
s32 Cheat_CheckSequence(s32 *progress, const u16 *seq, s32 len)
{
    if (g_cheatSeqTimer > 1500)
        *progress = 0;
    else
        g_cheatSeqTimer += g_dtMs;
    if (g_pad.IsConnected() && g_pad.cur.buttons != PAD_ALL_RELEASED && g_pad.prev.buttons != g_pad.cur.buttons) {
        if (*progress < len && seq[*progress] == g_pad.cur.buttons) {
            g_cheatSeqTimer = 0;
            if (++*progress >= len) {
                *progress = 0;
                return 1;
            }
        } else
            *progress = 0;
    }
    return 0;
}

/* 0x52de00 - called every frame by Main_Loop. */
void Cheat_Poll()
{
    if (Cheat_CheckSequence(&g_cheatMysteryProgress, g_cheatSeqMystery, 6))
        g_cheatMysteryFlag = 1;
    g_cheatLevelSkip = Cheat_CheckSequence(&g_cheatLevelSkipProgress, g_cheatSeqLevelSkip, 7);
}

/* 0x52de40 */
s32 Cheat_IsLevelSkipRequested()
{
    return g_cheatLevelSkip;
}
#pragma optimize("", on)

/* ---- 0x52de50-0x52df76: the countdown of a cut timed mode (no callers reach it) ---- */
/* 0x52de50 */
s32 Countdown_Format(char *out)
{
    if (out) {
        if (g_countdownMs > 0)
            Text_Sprintf(out, "%d", g_countdownMs / 1000 % 60);
        else
            Text_Sprintf(out, "GAMEOVER!");
    }
    return g_countdownMs;
}

/* 0x52dea3 - the tint it computes (towards red below 30 s) is never used. */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py) */
/* BYTES(dead-code): the tint is computed and never used, as in the original */
void Countdown_DrawHud()
{
    char msg[16]; /* local names chosen for their stack slots (tools/vc6_locals.py) */
    s16 box[4];
    s32 color;
    box[0] = 0x50;
    box[1] = 0x28;
    box[3] = g_pCurFont->lineHeight;
    box[2] = g_pCurFont->glyphWidth << 4;
    color = Countdown_Format(msg);
    if (color < 0)
        color = 0;
    else if (g_countdownMs > 30000)
        color = -1;
    else
        color = ((u32)(color * 255) / 30000 << 8) + 0xff00ff;
    Text_SetWindow(g_screenLayerBase + 8, box[0], box[1], box[0] + box[2], box[1] + box[3], 1);
    Text_SetFont(FONT_GAME);
    Text_PrintFmt(msg);
    Hud_EndBox_stub();
}
