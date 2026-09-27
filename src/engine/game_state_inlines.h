/* Shared inline definitions. Select helpers before inclusion, then undefine their selectors.
 * BYTES(inline): repeated inclusion preserves each caller's dependency visibility and definition order.
 * Per-helper guards prevent redefinition when a later include selects more helpers. */

#if defined(SDW_INLINE_FREE_CLEARGAMEFLAGS_U32) && !defined(SDW_INLINE_FREE_CLEARGAMEFLAGS_U32_DEFINED)
#define SDW_INLINE_FREE_CLEARGAMEFLAGS_U32_DEFINED
/* BYTES(inline): complement a constant mask at runtime, as in the other clear
 * aliases; replacing the wrapper with a folded mask changes the expansion. */
inline void ClearGameFlags(u32 mask)
{
    g_gameFlags &= ~mask;
}
#endif

#if defined(SDW_INLINE_FREE_GAMEFLAGS_CLEAR_U32) && !defined(SDW_INLINE_FREE_GAMEFLAGS_CLEAR_U32_DEFINED)
#define SDW_INLINE_FREE_GAMEFLAGS_CLEAR_U32_DEFINED
/* BYTES(inline): the constant mask is loaded and complemented at runtime
 * (Progress/Inventory, mov eax,0x100; not eax at 0x50ce39). */
inline void GameFlags_Clear(u32 mask)
{
    g_gameFlags &= ~mask;
}
#endif

#if defined(SDW_INLINE_FREE_GAME_CLEARFLAGS_U32) && !defined(SDW_INLINE_FREE_GAME_CLEARFLAGS_U32_DEFINED)
#define SDW_INLINE_FREE_GAME_CLEARFLAGS_U32_DEFINED
/* BYTES(inline): substitution keeps ~ as code (Fade, mov edx,0x10; not edx at
 * 0x53d80e; PauseMenu, the 0x4001 mask at 0x542b4b). */
inline void Game_ClearFlags(u32 mask)
{
    g_gameFlags &= ~mask;
}
#endif

#if defined(SDW_INLINE_FREE_SETGAMEFLAGS_U32) && !defined(SDW_INLINE_FREE_SETGAMEFLAGS_U32_DEFINED)
#define SDW_INLINE_FREE_SETGAMEFLAGS_U32_DEFINED
inline void SetGameFlags(u32 mask)
{
    g_gameFlags |= mask;
}
#endif
