/* T271 - original object "GameState.cpp" (guessed name): the GameState methods (0x5362e0-0x536395: Game_SetFlags,
 * Game_ResetState, Game_CanOpenMenu).
 *   .text  0x5362e0-0x536395
 *   .bss   0x6ddf5c-0x6ddf88  the game-state block the three methods run on (this = 0x6ddf5c, a GameState), which
 *                             the symbol tables and every other object name field by field: g_animDt +0,
 *                             g_texScrollListIds +4 (the WAR type-0x83 header: ids, reverse mask, weather type),
 *                             g_pDav +0x10, g_fadeTimer +0x14, g_gameFlags +0x18, g_letterboxState +0x1c,
 *                             g_camDebugMode +0x1d; then 12 bytes nothing refers to (attributed here by the map).
 * The other objects declare these globals extern; they are defined here, in address order.
 * .bss order: VC6 puts a file's globals WITHOUT an initialiser first, ordered by a hash of their names, and the
 * initialised ones after them in definition order; these names hash out of the exe's order, so every one is written
 * `= 0` and they come out in definition (= address) order.
 * g_texScrollListIds is defined as u16[4], as every user declares it (load_war.cpp, list.cpp, tex_scroll.cpp), next to
 * the separate g_texScrollReverseMask / g_weatherType: the symbol tables name all three parts of the header.
 */
/* BYTES: layout. */
/* BYTES(layout): written '= 0' only to keep definition order in .bss */
/* BYTES(layout): placeholder: 14 unreferenced bytes kept only for the .bss size (two items: VC6 4-aligns a 14-byte array) */
#include "sdw_classes.h"
#include "sdw_enums.h"

/* The reverse mask's type, as src/engine/tex_scroll.cpp declares it (a u16 bitfield: TexScroll_Init 0x560e61 reads it
 * with VC6's bitfield code). The generated headers do not have it; this local definition is the same as that file's, so
 * the decorated name agrees. */
struct TexScrollReverseBits {
    u16 list0 : 1;
    u16 list1 : 1;
    u16 list2 : 1;
    u16 list3 : 1;
};

/* ---- the game-state block 0x6ddf5c (all initialised to zero so that they keep this order: see the header) ---- */
s32 g_animDt = 0;                                  /* 0x6ddf5c  g_dt * 1000 >> 2 (GameState.animDt) */
u16 g_texScrollListIds[4] = {0};                   /* 0x6ddf60  WAR type-0x83 header: four id-list ids */
TexScrollReverseBits g_texScrollReverseMask = {0}; /* 0x6ddf68  bit i reverses list i */
u16 g_weatherType = 0;                             /* 0x6ddf6a  1 rain, 2 snow */
Dav *g_pDav = 0;                                   /* 0x6ddf6c  GameState.pDav */
s32 g_fadeTimer = 0;                               /* 0x6ddf70  1/4096 s (GameState.fadeTimer) */
u32 g_gameFlags = 0;                               /* 0x6ddf74  GameState.flags */
u8 g_letterboxState = 0;                           /* 0x6ddf78  GameState.cineState: 0 idle, 1 animating, 2 out */
u8 g_camDebugMode = 0;                             /* 0x6ddf79  GameState.camDebugMode */
/* 0x6ddf7a-0x6ddf88: 14 bytes of this object's .bss that no instruction refers to (the 2-byte tail of the GameState
 * block, then 12 bytes the map attributes here). Genuinely opaque; kept as bytes so that the object's .bss has its
 * original size and the next object's .bss starts at 0x6ddf88. Two items, because VC6 4-aligns a 14-byte array. */
u16 g_gameStateUnref_6ddf7a = 0;
u8 g_gameStateUnref_6ddf7c[12] = {0};

/* 0x5362e0 - sets or clears bits of g_gameFlags (this is always &g_animDt, so +0x18 is g_gameFlags). */
void GameState::Game_SetFlags(u32 mask, s32 on)
{
    if (on)
        flags |= mask;
    else
        flags &= ~mask;
}

/* 0x536317 - g_gameFlags = GF_TRANSITION_IDLE | GF_UPDATE_OBJECTS | GF_RENDER_WORLD (0xc010); clears the letterbox
 * state, g_animDt, g_pDav, g_fadeTimer and the camera debug mode. */
void GameState::Game_ResetState()
{
    flags = 0;
    flags |= GF_TRANSITION_IDLE | GF_UPDATE_OBJECTS | GF_RENDER_WORLD;
    cineState = 0;
    animDt = 0;
    pDav = 0;
    fadeTimer = 0;
    camDebugMode = 0;
}

/* 0x536369 - menus may open only while no transition runs and no letterbox is up. */
s32 GameState::Game_CanOpenMenu()
{
    if ((flags & GF_TRANSITION_IDLE) && !cineState)
        return 1;
    return 0;
}
