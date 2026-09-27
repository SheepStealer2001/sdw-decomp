/* Shared inline definitions. Select helpers before inclusion, then undefine their selectors.
 * BYTES(inline): repeated inclusion preserves each caller's dependency visibility and definition order.
 * Per-helper guards prevent redefinition when a later include selects more helpers. */

#if defined(SDW_INLINE_ALTMODEL_ISVALID) && !defined(SDW_INLINE_ALTMODEL_ISVALID_DEFINED)
#define SDW_INLINE_ALTMODEL_ISVALID_DEFINED
inline s32 AltModel::IsValid()
{
    return modelResIdx != 0xffff;
}
#endif
