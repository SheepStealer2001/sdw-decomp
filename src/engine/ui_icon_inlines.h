/* Shared inline definitions. Select helpers before inclusion, then undefine their selectors.
 * BYTES(inline): repeated inclusion preserves each caller's dependency visibility and definition order.
 * Per-helper guards prevent redefinition when a later include selects more helpers. */

#if defined(SDW_INLINE_UIICON_SETCOLOR_U32) && !defined(SDW_INLINE_UIICON_SETCOLOR_U32_DEFINED)
#define SDW_INLINE_UIICON_SETCOLOR_U32_DEFINED
/* BYTES(inline): forwarding through mainQuad retains its address temporary
 * (Fade, 0x53f177). */
inline void UiIcon::SetColor(u32 rgb)
{
    mainQuad.SetColor(rgb);
}
#endif

#if defined(SDW_INLINE_UIICON_SETENABLED_S32) && !defined(SDW_INLINE_UIICON_SETENABLED_S32_DEFINED)
#define SDW_INLINE_UIICON_SETENABLED_S32_DEFINED
/* BYTES(inline): retain the receiver/value expansion temporaries used by Interface. */
inline void UiIcon::SetEnabled(s32 on)
{
    enabled = on;
}
#endif

#if defined(SDW_INLINE_UIICON_SETHIGHLIGHTED_S32) && !defined(SDW_INLINE_UIICON_SETHIGHLIGHTED_S32_DEFINED)
#define SDW_INLINE_UIICON_SETHIGHLIGHTED_S32_DEFINED
/* BYTES(inline): retain the receiver/value expansion temporaries used by Interface. */
inline void UiIcon::SetHighlighted(s32 on)
{
    highlighted = on;
}
#endif
