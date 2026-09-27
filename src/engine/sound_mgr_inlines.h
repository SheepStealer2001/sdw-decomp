/* Shared inline definitions. Select helpers before inclusion, then undefine their selectors.
 * BYTES(inline): repeated inclusion preserves each caller's dependency visibility and definition order.
 * Per-helper guards prevent redefinition when a later include selects more helpers. */

#if defined(SDW_INLINE_FREE_SOUNDISPLAYING_U16) && !defined(SDW_INLINE_FREE_SOUNDISPLAYING_U16_DEFINED)
#define SDW_INLINE_FREE_SOUNDISPLAYING_U16_DEFINED
inline s32 SoundIsPlaying(u16 handle)
{
    return Sound_IsPlaying(handle);
}
#endif
