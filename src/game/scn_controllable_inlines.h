/* Shared inline definitions. Select helpers before inclusion, then undefine their selectors.
 * BYTES(inline): repeated inclusion preserves each caller's dependency visibility and definition order.
 * Per-helper guards prevent redefinition when a later include selects more helpers. */

#if defined(SDW_INLINE_FREE_STICK_CLEAR_S32) && !defined(SDW_INLINE_FREE_STICK_CLEAR_S32_DEFINED)
#define SDW_INLINE_FREE_STICK_CLEAR_S32_DEFINED
/* BYTES(inline): clear stickX/stickY/stickMag through one address temporary
 * (Wolf, 0x47e001), as for an inline taking &stickX. */
inline void Stick_Clear(s32 *stick)
{
    stick[0] = 0;
    stick[1] = 0;
    stick[2] = 0;
}
#endif
