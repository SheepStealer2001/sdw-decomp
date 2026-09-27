/* Shared inline definitions. Select helpers before inclusion, then undefine their selectors.
 * BYTES(inline): repeated inclusion preserves each caller's dependency visibility and definition order.
 * Per-helper guards prevent redefinition when a later include selects more helpers. */

#if defined(SDW_INLINE_SCNMOBILE_GROUNDY) && !defined(SDW_INLINE_SCNMOBILE_GROUNDY_DEFINED)
#define SDW_INLINE_SCNMOBILE_GROUNDY_DEFINED
/* BYTES(inline): each read gives Wolf a fresh 16-bit temporary (0x48dc93 / 0x48cf69). */
inline s16 ScnMobile::GroundY()
{
    return shadow.groundPos.y;
}
#endif

#if defined(SDW_INLINE_SCNMOBILE_SETSHADOWRADIUS_U8) && !defined(SDW_INLINE_SCNMOBILE_SETSHADOWRADIUS_U8_DEFINED)
#define SDW_INLINE_SCNMOBILE_SETSHADOWRADIUS_U8_DEFINED
/* BYTES(inline): the byte argument supplies Wolf's temporary at 0x48db6a. */
inline void ScnMobile::SetShadowRadius(u8 radius)
{
    shadow.radius = radius;
}
#endif
