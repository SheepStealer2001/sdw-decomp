/* Shared inline definitions. Select helpers before inclusion, then undefine their selectors.
 * BYTES(inline): repeated inclusion preserves each caller's dependency visibility and definition order.
 * Per-helper guards prevent redefinition when a later include selects more helpers. */

#if defined(SDW_INLINE_SHADOW_INVALIDATE) && !defined(SDW_INLINE_SHADOW_INVALIDATE_DEFINED)
#define SDW_INLINE_SHADOW_INVALIDATE_DEFINED
inline void Shadow::Invalidate()
{
    flags |= SHADOW_F_REPROJECT;
}
#endif

#if defined(SDW_INLINE_SHADOW_REPROJECT) && !defined(SDW_INLINE_SHADOW_REPROJECT_DEFINED)
#define SDW_INLINE_SHADOW_REPROJECT_DEFINED
/* BYTES(inline): the embedded shadow's address is the receiver temporary
 * (Wolf's ScnMobile +0x64 block, 0x47e062). */
inline void Shadow::Reproject()
{
    flags |= SHADOW_F_REPROJECT;
}
#endif

#if defined(SDW_INLINE_SHADOW_SETENABLED_S32) && !defined(SDW_INLINE_SHADOW_SETENABLED_S32_DEFINED)
#define SDW_INLINE_SHADOW_SETENABLED_S32_DEFINED
inline void Shadow::SetEnabled(s32 on)
{
    if (on)
        flags &= (u8)~SHADOW_F_HIDDEN;
    else
        flags |= SHADOW_F_HIDDEN;
}
#endif

#if defined(SDW_INLINE_SHADOW_SETFLAG4_S32) && !defined(SDW_INLINE_SHADOW_SETFLAG4_S32_DEFINED)
#define SDW_INLINE_SHADOW_SETFLAG4_S32_DEFINED
/* BYTES(inline): the shadow receiver is a temporary (Wolf, 0x4883d7; Snowball,
 * 0x4f5868). The Snowball reconstruction also found this bit set by SmallRock_Init,
 * but had not identified a reader. */
inline void Shadow::SetFlag4(s32 on)
{
    if (on)
        flags |= SHADOW_F_DYNAMIC;
    else
        flags &= (u8)~SHADOW_F_DYNAMIC;
}
#endif

#if defined(SDW_INLINE_SHADOW_SETVISIBLE_S32) && !defined(SDW_INLINE_SHADOW_SETVISIBLE_S32_DEFINED)
#define SDW_INLINE_SHADOW_SETVISIBLE_S32_DEFINED
/* BYTES(inline): flag 1 means hidden; the embedded shadow's address gets a
 * temporary (Wolf, 0x48722d; DaffyMilitary, 0x43bc17; Gossamer Level 08, 0x4536a3). */
inline void Shadow::SetVisible(s32 on)
{
    if (on)
        flags &= (u8)~SHADOW_F_HIDDEN;
    else
        flags |= SHADOW_F_HIDDEN;
}
#endif
