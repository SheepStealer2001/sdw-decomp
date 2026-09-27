/* Shared inline definitions. Select helpers before inclusion, then undefine their selectors.
 * BYTES(inline): repeated inclusion preserves each caller's dependency visibility and definition order.
 * Per-helper guards prevent redefinition when a later include selects more helpers. */

#if defined(SDW_INLINE_PAD_SETACTUATOR_INT) && !defined(SDW_INLINE_PAD_SETACTUATOR_INT_DEFINED)
#define SDW_INLINE_PAD_SETACTUATOR_INT_DEFINED
/* BYTES(inline): this wrapper gives Input and PauseMenu their original expansion
 * shape (Input_Init, 0x55e63b). */
inline void Pad::SetActuator(int on)
{
    actuatorEnable = on;
}
#endif
