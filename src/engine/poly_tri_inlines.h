/* Shared inline definitions. Select helpers before inclusion, then undefine their selectors.
 * BYTES(inline): repeated inclusion preserves each caller's dependency visibility and definition order.
 * Per-helper guards prevent redefinition when a later include selects more helpers. */

#if defined(SDW_INLINE_POLYTRI_OPERATOR_CONST_POLYTRI) && !defined(SDW_INLINE_POLYTRI_OPERATOR_CONST_POLYTRI_DEFINED)
#define SDW_INLINE_POLYTRI_OPERATOR_CONST_POLYTRI_DEFINED
/* BYTES(inline): assign the three indices separately; the implicit array copy
 * would generate a loop (PolyF and the other polygon classes). */
inline PolyTri &PolyTri::operator=(const PolyTri &src)
{
    idx[0] = src.idx[0];
    idx[1] = src.idx[1];
    idx[2] = src.idx[2];
    return *this;
}
#endif
