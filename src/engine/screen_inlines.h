/* Shared inline definitions. Select helpers before inclusion, then undefine their selectors.
 * BYTES(inline): repeated inclusion preserves each caller's dependency visibility and definition order.
 * Per-helper guards prevent redefinition when a later include selects more helpers. */

#if defined(SDW_INLINE_FREE_SCREENHEIGHTS16) && !defined(SDW_INLINE_FREE_SCREENHEIGHTS16_DEFINED)
#define SDW_INLINE_FREE_SCREENHEIGHTS16_DEFINED
/* BYTES(inline): screen dimensions remain functions with their caller's return
 * type, not literals; VC6 keeps the original arithmetic and runtime initializers. */
inline s16 ScreenHeightS16()
{
    return 240;
}
#endif

#if defined(SDW_INLINE_FREE_SCREENHEIGHTS32) && !defined(SDW_INLINE_FREE_SCREENHEIGHTS32_DEFINED)
#define SDW_INLINE_FREE_SCREENHEIGHTS32_DEFINED
inline s32 ScreenHeightS32()
{
    return 240;
}
#endif

#if defined(SDW_INLINE_FREE_SCREENHEIGHTU16) && !defined(SDW_INLINE_FREE_SCREENHEIGHTU16_DEFINED)
#define SDW_INLINE_FREE_SCREENHEIGHTU16_DEFINED
inline u16 ScreenHeightU16()
{
    return 240;
}
#endif

#if defined(SDW_INLINE_FREE_SCREENWIDTHS16) && !defined(SDW_INLINE_FREE_SCREENWIDTHS16_DEFINED)
#define SDW_INLINE_FREE_SCREENWIDTHS16_DEFINED
inline s16 ScreenWidthS16()
{
    return 512;
}
#endif

#if defined(SDW_INLINE_FREE_SCREENWIDTHS32) && !defined(SDW_INLINE_FREE_SCREENWIDTHS32_DEFINED)
#define SDW_INLINE_FREE_SCREENWIDTHS32_DEFINED
inline s32 ScreenWidthS32()
{
    return 512;
}
#endif

#if defined(SDW_INLINE_FREE_SCREENWIDTHU16) && !defined(SDW_INLINE_FREE_SCREENWIDTHU16_DEFINED)
#define SDW_INLINE_FREE_SCREENWIDTHU16_DEFINED
/* BYTES(inline): this is not a VC6 constant expression: HUD rectangles get runtime
 * initializers (0x53f04f), and width minus x loads 512 first (0x53eb0e). */
inline u16 ScreenWidthU16()
{
    return 512;
}
#endif

#if defined(SDW_INLINE_SCREEN_LAYERS4_U16) && !defined(SDW_INLINE_SCREEN_LAYERS4_U16_DEFINED)
#define SDW_INLINE_SCREEN_LAYERS4_U16_DEFINED
/* BYTES(inline): the layer index is materialized in a register even when constant
 * (Screen/GameLevel, 0x529fbd). */
inline u32 *Screen::Layers4(u16 index)
{
    return (u32 *)scratch4 + index; /* cast kept: a scratch area each user lays out its own way */
}
#endif

#if defined(SDW_INLINE_SCREEN_LAYERS60_U16) && !defined(SDW_INLINE_SCREEN_LAYERS60_U16_DEFINED)
#define SDW_INLINE_SCREEN_LAYERS60_U16_DEFINED
/* BYTES(inline): retain the indexed-address expansion, as for Layers4. */
inline u32 *Screen::Layers60(u16 index)
{
    return (u32 *)scratch60 + index; /* cast kept: as above */
}
#endif
