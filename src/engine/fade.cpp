/* T276 - original object "Fade.cpp" (guessed name): the fade entry points 0x53e700-0x53ea45 (Fade_StartLevelExit,
 * Fade_StartRestart, Fade_StartOut, Fade_Update).
 *   .text  0x53e700-0x53ea45
 *   .data  0x57c050-0x57c05c  the Cine.h op-stride table's header-static copy (9 bytes + 3 of alignment): this file
 *                             includes Cine.h (it uses g_cinePlayer / Cine::Stop), and VC6 emits the unreferenced
 *                             static in every file that includes it (as in the other Cine.h includers' staged files)
 * The declarations are the preamble of the screen-space HUD layer (0x53d4bb-0x53f42f), shared by T275
 * (src/engine/interface.cpp), T276 and T277 (src/engine/prompt.cpp).
 */
/* BYTES: cast, inline, layout. */
/* BYTES(inline): Cine::IsActive (SDW_MEMBERS_Cine inline): source-only inline: its value is a stack temp (0x53e777, 0x53e8c6) */
/* BYTES(inline): UiQuad::SetColor (SDW_MEMBERS_UiQuad inline): source-only inline: 'this' goes through a stack temp */
/* BYTES(inline): UiIcon::SetColor (SDW_MEMBERS_UiIcon inline): source-only inline: &mainQuad goes through a stack temp (0x53f177) */
/* BYTES(cast): ARGB4444 macro: the redundant & 0xffff is the original's (and ecx,0xffff at 0x53d62c) */
/* BYTES(layout): the Cine.h header static: every object including the cinematic header carries this copy in its .data, referenced or not */
/*
 *
 * Everything here is drawn in the 512x240 virtual HUD space (the PlayStation's screen) and scaled by Screen::ScaleX/Y.
 * The fade and letterbox curtains are a 4x4 ARGB4444 texture (a PolyBatcher texture page reserved for it, locked and
 * refilled every frame) stretched over the screen.
 * Names are descriptive (the binary has no symbols). Local names were picked for the /Od frame order (tools/vc6_locals.py):
 * they are plausible, not recovered.
 */
#include "sdw_enums.h"
#include "../sdk/ddraw.h"
#define SDW_MEMBERS_Texture void Surface_LockForWrite(DDSURFACEDESC2 *desc); /* 0x40ae3f */


#include "sdw_enums.h"
#include "sdw_classes.h"
#define SDW_INLINE_CINE_ISACTIVE 1
#include "cine_inlines.h"
#undef SDW_INLINE_CINE_ISACTIVE
#define SDW_INLINE_UIQUAD_SETCOLOR_U32 1
#include "ui_quad_inlines.h"
#undef SDW_INLINE_UIQUAD_SETCOLOR_U32
#define SDW_INLINE_UIICON_SETCOLOR_U32 1
#include "ui_icon_inlines.h"
#undef SDW_INLINE_UIICON_SETCOLOR_U32

/* The DAV directory (DavHeader.dir): packed, its pointers sit at +6/+0xa/+0x12 (src/engine/load_dav.cpp), which the
 * struct generator cannot lay out, so it is declared here. */
#pragma pack(push, 1)
struct DavDirectory {
    u16 indexCount;
    u16 bitmapCount;
    u16 unk04;
    u16 *indices;
    DavBitmapRec *bitmaps; /* +0x0a 10-byte records */
    u32 fileSize;
    u32 *idLists;
};
#pragma pack(pop)

/* ---- callees ---- */
typedef void (*MenuHandler)(u8 msg, MenuPage *self);
#include "fixed_math.h"
#include "interface.h"
#include "prompt.h"
#include "draw2d.h"
#include "scn_tools.h"
#include "scenaric_loop.h"
#include "text.h"
#include "input.h"
#include "game_state.h"
#include "screen.h"
#include "cine.h"
#include "scenaric.h"
#include "time.h"
u32 Rgb24_Lerp(u32 a, u32 b, s16 t); /* 0x52795c */
void Draw2D_TexRect(float z, float x0, float y0, float x1, float y1, s32 texIndex, float uTL, float vTL, u32 cTL,
                    float uBL, float vBL, u32 cBL, float uTR, float vTR, u32 cTR, float uBR, float vBR,
                    u32 cBR);                                                            /* 0x5242df */
void Dialogue_SetBoxActive(s32 active);                                                  /* 0x539479 */
u8 Dialogue_Say(const char *text, s32 voiceId, ScnObject *speaker, u32 arg);             /* 0x539893 */
s32 Rand_Bounded(s32 bound);                                                             /* 0x561219 */
u16 Text_CountWrappedLines(const char *s);                                               /* 0x53353f */
void Menu_BuildList(MenuPage *pages, Menu *menu, s8 count, const MenuHandler *handlers); /* 0x539199 */
void Ui_DrawTextBox(TextBox *box, u16 lineCount);                                        /* 0x53d243 */
void Ui_DrawMenuBox(MenuBox *box);                                                       /* 0x53d385 */
void **Res_GetValidatedIdList(u16 resId, u16 *outCount);                                 /* 0x548381 */
u16 Sound_Play(u16 soundId, void *owner, u16 volume, u8 flags, s32 rate);                /* 0x5491b8 */

/* ---- globals ---- */
extern u32 *g_screenLayerBase;       /* 0x585044 */
extern Wolf *g_pWolf;                /* 0x6cf310 */
extern u32 g_gameFlags;              /* 0x6ddf74 */
extern s32 g_dt;                     /* 0x71b300 */
extern s16 g_dtRawMs;                /* 0x71b2d8 */
extern u32 g_gameTime;               /* 0x71b2d0  u32, as T304 defines it */
extern s32 g_dialogueCurText;        /* 0x6ddfb0 */
extern u16 *g_resTelescopeMaskOuter; /* 0x6de0e0 */
extern u16 *g_resTelescopeMaskInner; /* 0x6de0e4 */
extern u16 *g_resCannonMask;         /* 0x6de144 */

/* inline: the constant mask is substituted but its `~` is left to run time (mov edx,0x10; not edx at 0x53d80e). */
/* BYTES(inline): source-only inline: the constant is substituted but its ~ stays code (mov edx,0x10; not edx at 0x53d80e) */
#define SDW_INLINE_FREE_GAME_CLEARFLAGS_U32 1
#include "game_state_inlines.h"
#undef SDW_INLINE_FREE_GAME_CLEARFLAGS_U32
/* inline: the virtual screen size (Screen::virtWidth / virtHeight are u16). As inlines they are not constant
 * expressions to VC6: the rects below get run-time initialisers (0x53f04f), and `ScreenWidthU16() - x` loads the
 * constant first (0x53eb0e), which the literal 512 does not. */
/* BYTES(inline): source-only inlines, not literals: the rects get run-time initialisers (0x53f04f) and ScreenWidthU16() - x loads the constant first (0x53eb0e) */
#define SDW_INLINE_FREE_SCREENWIDTHU16 1
#include "screen_inlines.h"
#undef SDW_INLINE_FREE_SCREENWIDTHU16
#define SDW_INLINE_FREE_SCREENHEIGHTU16 1
#include "screen_inlines.h"
#undef SDW_INLINE_FREE_SCREENHEIGHTU16

/* An ARGB8888 colour as ARGB4444. The last term's redundant 16-bit mask is the original's (and ecx,0xffff at 0x53d62c). */
#define ARGB4444(c) ((((c) >> 4) & 0xf) | (((c) >> 8) & 0xf0) | (((c) >> 12) & 0xf00) | (((c) >> 16) & 0xf000 & 0xffff))

/* Cine.h: the cinematic op-stride table (g_cineOpStride 0x5816fc is the player's own copy). A header static, so every
 * file that includes Cine.h has one; nothing here reads it. */
static u8 s_cineOpStride[9] = {0, 8, 8, 4, 2, 2, 4, 2, 2};

/* 0x53e700 - start the level-exit fade (at least one second); Fade_Update sets LEVEL_EXIT_NEXT when it ends. */
void Fade_StartLevelExit(s32 ticks)
{
    if (!(g_gameFlags & GF_FADE_EXIT)) {
        if (ticks < 0x1000)
            ticks = 0x1000;
        Game_ClearFlags(GF_FADE_RESTART | GF_FADE_IN);
        Game_ClearFlags(GF_TRANSITION_IDLE);
        g_gameFlags |= GF_FADE_EXIT;
        g_fadeTimer = ticks;
    }
}

/* 0x53e767 - start the death / restart fade-out (stopping a cinematic unless GF_CINE_SURVIVES_RESTART);
 * Scenaric_ResetAll runs when it ends. */
void Fade_StartRestart(s32 ticks)
{
    if (!(g_gameFlags & (GF_FADE_RESTART | GF_FADE_EXIT))) {
        if (g_cinePlayer.IsActive() && !(g_gameFlags & GF_CINE_SURVIVES_RESTART))
            g_cinePlayer.Stop();
        if (ticks < 0x1000)
            ticks = 0x1000;
        Game_ClearFlags(GF_TRANSITION_IDLE);
        Game_ClearFlags(GF_FADE_IN);
        g_gameFlags |= GF_FADE_RESTART;
        g_fadeTimer = ticks;
    }
}

/* 0x53e7f9 - start the fade-in at a (re)start; half the time (and never at game time 0) it sets GF_FADE_VARIANT. */
void Fade_StartOut(s32 ticks)
{
    if (!(g_gameFlags & (GF_FADE_IN | GF_FADE_EXIT))) {
        if (ticks < 0x1000)
            ticks = 0x1000;
        Game_ClearFlags(GF_TRANSITION_IDLE);
        Game_ClearFlags(GF_FADE_RESTART);
        g_gameFlags |= GF_FADE_IN;
        g_fadeTimer = ticks;
        if (Rand_Bounded(100) < 50 && g_gameTime != 0)
            g_gameFlags |= GF_FADE_VARIANT;
        else
            Game_ClearFlags(GF_FADE_VARIANT);
    }
}

/* 0x53e8a3 - run the current fade: draw the curtain and step the timer by g_dt (g_dtRaw while a cinematic plays). */
void Fade_Update()
{
    s32 dt;
    if (g_gameFlags & (GF_FADE_RESTART | GF_FADE_IN | GF_FADE_EXIT)) {
        Game_ClearFlags(GF_TRANSITION_IDLE);
        if (g_cinePlayer.IsActive())
            dt = g_dtRaw;
        else
            dt = g_dt;
        if (g_gameFlags & (GF_FADE_RESTART | GF_FADE_EXIT)) {
            if (g_fadeTimer <= 0x1000)
                Fade_DrawOverlay(0, (0x1000 - g_fadeTimer) * 0x1f >> 12, 0);
            g_fadeTimer -= dt;
            if (g_fadeTimer <= 0) {
                g_screen.Clear(0);
                g_fadeTimer = 0;
                if (g_gameFlags & GF_FADE_EXIT) {
                    Game_ClearFlags(GF_FADE_EXIT);
                    g_levelExitFlags |= LEVEL_EXIT_NEXT;
                } else {
                    Game_ClearFlags(GF_FADE_RESTART);
                    g_gameFlags |= GF_TRANSITION_IDLE;
                    Scenaric_ResetAll();
                }
            }
        } else if (g_gameFlags & GF_FADE_IN) {
            if (g_fadeTimer <= 0x1000)
                Fade_DrawOverlay(0, g_fadeTimer * 0x1f >> 12, 0);
            else
                Fade_DrawOverlay(0, 0x1f, 0);
            g_fadeTimer -= dt;
            if (g_fadeTimer <= 0) {
                g_fadeTimer = 0;
                Game_ClearFlags(GF_FADE_IN);
                g_gameFlags |= GF_TRANSITION_IDLE;
            }
        }
    }
}
