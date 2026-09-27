/* Shared inline definitions. Select helpers before inclusion, then undefine their selectors.
 * BYTES(inline): repeated inclusion preserves each caller's dependency visibility and definition order.
 * Per-helper guards prevent redefinition when a later include selects more helpers. */

#if defined(SDW_INLINE_SPRITE_DRAWAT_U32_S32_S32) && !defined(SDW_INLINE_SPRITE_DRAWAT_U32_S32_S32_DEFINED)
#define SDW_INLINE_SPRITE_DRAWAT_U32_S32_S32_DEFINED
inline void Sprite::DrawAt(u32 *layer, s32 drawX, s32 yPos)
{
    DrawThunk(layer, drawX, yPos, drawX + widthMinus1, yPos + height, 0x808080, 0);
}
#endif

#if defined(SDW_INLINE_SPRITE_DRAWAT_U32_S32_S32_U32_U32) && \
    !defined(SDW_INLINE_SPRITE_DRAWAT_U32_S32_S32_U32_U32_DEFINED)
#define SDW_INLINE_SPRITE_DRAWAT_U32_S32_S32_U32_U32_DEFINED
inline void Sprite::DrawAt(u32 *layer, s32 x, s32 y, u32 color, u32 flip)
{
    Draw(layer, x, y, x + widthMinus1, y + height, color, flip);
}
#endif

#if defined(SDW_INLINE_SPRITE_DRAWTHUNKAT_U32_S32_S32_U32_U32) && \
    !defined(SDW_INLINE_SPRITE_DRAWTHUNKAT_U32_S32_S32_U32_U32_DEFINED)
#define SDW_INLINE_SPRITE_DRAWTHUNKAT_U32_S32_S32_U32_U32_DEFINED
inline void Sprite::DrawThunkAt(u32 *layer, s32 x, s32 y, u32 color, u32 flip)
{
    DrawThunk(layer, x, y, x + widthMinus1, y + height, color, flip);
}
#endif
