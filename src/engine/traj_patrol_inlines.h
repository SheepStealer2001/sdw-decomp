/* Shared inline definitions. Select helpers before inclusion, then undefine their selectors.
 * BYTES(inline): repeated inclusion preserves each caller's dependency visibility and definition order.
 * Per-helper guards prevent redefinition when a later include selects more helpers. */

#if defined(SDW_INLINE_TRAJPATROL_REVERSE) && !defined(SDW_INLINE_TRAJPATROL_REVERSE_DEFINED)
#define SDW_INLINE_TRAJPATROL_REVERSE_DEFINED
/* BYTES(inline): the embedded patrol address is retained as the receiver
 * temporary (InstantMartian). */
inline void TrajPatrol::Reverse()
{
    forceAdvance = 1;
    forward ^= 1;
}
#endif
