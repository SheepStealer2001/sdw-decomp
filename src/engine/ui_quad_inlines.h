/* Shared inline definitions. Select helpers before inclusion, then undefine their selectors.
 * BYTES(inline): repeated inclusion preserves each caller's dependency visibility and definition order.
 * Per-helper guards prevent redefinition when a later include selects more helpers. */

#if defined(SDW_INLINE_UIQUAD_SETCOLOR_U32) && !defined(SDW_INLINE_UIQUAD_SETCOLOR_U32_DEFINED)
#define SDW_INLINE_UIQUAD_SETCOLOR_U32_DEFINED
/* BYTES(inline): the RGB update keeps the receiver temporary (Interface,
 * 0x53a669); the existing alpha byte is preserved for RGB input. */
inline void UiQuad::SetColor(u32 rgb)
{
    color = (color & 0xff000000) | rgb;
}
#endif

#if defined(SDW_INLINE_UIQUAD_SETFADELEVEL_U8) && !defined(SDW_INLINE_UIQUAD_SETFADELEVEL_U8_DEFINED)
#define SDW_INLINE_UIQUAD_SETFADELEVEL_U8_DEFINED
/* BYTES(inline): receiver/value temporaries preserve Interface's fade update
 * expansion (0x538f5b). */
inline void UiQuad::SetFadeLevel(u8 level)
{
    drawFlags = (drawFlags & (u16)~UIQUAD_FADE_MASK) | (level << 6);
}
#endif
