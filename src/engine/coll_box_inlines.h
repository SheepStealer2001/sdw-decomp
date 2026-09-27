/* Shared inline definitions. Select helpers before inclusion, then undefine their selectors.
 * BYTES(inline): repeated inclusion preserves each caller's dependency visibility and definition order.
 * Per-helper guards prevent redefinition when a later include selects more helpers. */

#if defined(SDW_INLINE_FREE_BOXCONTAINS_COLLBOX_VEC3S) && !defined(SDW_INLINE_FREE_BOXCONTAINS_COLLBOX_VEC3S_DEFINED)
#define SDW_INLINE_FREE_BOXCONTAINS_COLLBOX_VEC3S_DEFINED
inline s32 BoxContains(CollBox *box, Vec3s *point)
{
    return point->x >= box->min.x && point->x <= box->max.x && point->y >= box->min.y && point->y <= box->max.y &&
           point->z >= box->min.z && point->z <= box->max.z;
}
#endif

#if defined(SDW_INLINE_FREE_BOXCONTAINSXZ_BOX_VEC3S) && !defined(SDW_INLINE_FREE_BOXCONTAINSXZ_BOX_VEC3S_DEFINED)
#define SDW_INLINE_FREE_BOXCONTAINSXZ_BOX_VEC3S_DEFINED
inline s32 BoxContainsXZ(Box *box, Vec3s *point)
{
    return point->x >= box->min[0] && point->x <= box->max[0] && point->z >= box->min[2] && point->z <= box->max[2];
}
#endif

#if defined(SDW_INLINE_FREE_BOXOVERLAP2_S32_S32) && !defined(SDW_INLINE_FREE_BOXOVERLAP2_S32_S32_DEFINED)
#define SDW_INLINE_FREE_BOXOVERLAP2_S32_S32_DEFINED
/* BYTES(inline): OR the sign bits before NOT/mask; return 0x80000000 or zero,
 * not a normalized boolean (ScnTools and the collision helpers). */
static inline u32 BoxOverlap2(s32 a, s32 b)
{
    return ~(a | b) & 0x80000000U;
}
#endif

#if defined(SDW_INLINE_FREE_BOXOVERLAP4_S32_S32_S32_S32) && \
    !defined(SDW_INLINE_FREE_BOXOVERLAP4_S32_S32_S32_S32_DEFINED)
#define SDW_INLINE_FREE_BOXOVERLAP4_S32_S32_S32_S32_DEFINED
/* BYTES(inline): retain the OR/OR/OR/NOT/mask sequence and right-to-left argument
 * evaluation (WoodenPlatform, 0x50b6d5). */
static inline u32 BoxOverlap4(s32 a, s32 b, s32 c, s32 d)
{
    return ~(a | b | c | d) & 0x80000000U;
}
#endif

#if defined(SDW_INLINE_FREE_BOX_CONTAINSPOINT_BOX_VEC3S) && \
    !defined(SDW_INLINE_FREE_BOX_CONTAINSPOINT_BOX_VEC3S_DEFINED)
#define SDW_INLINE_FREE_BOX_CONTAINSPOINT_BOX_VEC3S_DEFINED
/* BYTES(inline): both faces are inclusive; the two arguments become stack
 * temporaries in Wolf's expansion (0x4905d6). */
inline s32 Box_ContainsPoint(Box *box, Vec3s *p)
{
    return p->x >= box->min[0] && p->x <= box->max[0] && p->y >= box->min[1] && p->y <= box->max[1] &&
           p->z >= box->min[2] && p->z <= box->max[2];
}
#endif

#if defined(SDW_INLINE_FREE_BOX_CONTAINSPOINT_COLLBOX_VEC3S) && \
    !defined(SDW_INLINE_FREE_BOX_CONTAINSPOINT_COLLBOX_VEC3S_DEFINED)
#define SDW_INLINE_FREE_BOX_CONTAINSPOINT_COLLBOX_VEC3S_DEFINED
inline s32 Box_ContainsPoint(CollBox *box, Vec3s *p)
{
    return p->x >= box->min.x && p->x <= box->max.x && p->y >= box->min.y && p->y <= box->max.y && p->z >= box->min.z &&
           p->z <= box->max.z;
}
#endif

#if defined(SDW_INLINE_FREE_BOX_CONTAINSPOINTXZ_BOX_VEC3S) && \
    !defined(SDW_INLINE_FREE_BOX_CONTAINSPOINTXZ_BOX_VEC3S_DEFINED)
#define SDW_INLINE_FREE_BOX_CONTAINSPOINTXZ_BOX_VEC3S_DEFINED
inline s32 Box_ContainsPointXZ(Box *box, Vec3s *p)
{
    return p->x >= box->min[0] && p->x <= box->max[0] && p->z >= box->min[2] && p->z <= box->max[2];
}
#endif

#if defined(SDW_INLINE_FREE_COLLISIONOVERLAPFOUR_S32_S32_S32_S32) && \
    !defined(SDW_INLINE_FREE_COLLISIONOVERLAPFOUR_S32_S32_S32_S32_DEFINED)
#define SDW_INLINE_FREE_COLLISIONOVERLAPFOUR_S32_S32_S32_S32_DEFINED
/* BYTES(inline): collision tests use one OR of the signed distances, then NOT
 * and 0x80000000; do not replace the mask result with 1. */
inline u32 CollisionOverlapFour(s32 a, s32 b, s32 c, s32 d)
{
    return ~(a | b | c | d) & 0x80000000U;
}
#endif

#if defined(SDW_INLINE_FREE_COLLISIONOVERLAPTWO_S32_S32) && \
    !defined(SDW_INLINE_FREE_COLLISIONOVERLAPTWO_S32_S32_DEFINED)
#define SDW_INLINE_FREE_COLLISIONOVERLAPTWO_S32_S32_DEFINED
/* BYTES(inline): the two-distance form keeps the same sign-mask result. */
inline u32 CollisionOverlapTwo(s32 a, s32 b)
{
    return ~(a | b) & 0x80000000U;
}
#endif

#if defined(SDW_INLINE_FREE_CONTAINSXZ_BOX_VEC3S) && !defined(SDW_INLINE_FREE_CONTAINSXZ_BOX_VEC3S_DEFINED)
#define SDW_INLINE_FREE_CONTAINSXZ_BOX_VEC3S_DEFINED
static inline s32 ContainsXZ(Box *box, Vec3s *point)
{
    return point->x >= box->min[0] && point->x <= box->max[0] && point->z >= box->min[2] && point->z <= box->max[2];
}
#endif

#if defined(SDW_INLINE_FREE_INBOX_BOX_VEC3S) && !defined(SDW_INLINE_FREE_INBOX_BOX_VEC3S_DEFINED)
#define SDW_INLINE_FREE_INBOX_BOX_VEC3S_DEFINED
static inline s32 InBox(Box *box, Vec3s *point)
{
    return point->x >= box->min[0] && point->x <= box->max[0] && point->y >= box->min[1] && point->y <= box->max[1] &&
           point->z >= box->min[2] && point->z <= box->max[2];
}
#endif

#if defined(SDW_INLINE_FREE_INBOXXZ_BOX_VEC3S) && !defined(SDW_INLINE_FREE_INBOXXZ_BOX_VEC3S_DEFINED)
#define SDW_INLINE_FREE_INBOXXZ_BOX_VEC3S_DEFINED
static inline s32 InBoxXZ(Box *box, Vec3s *point)
{
    return point->x >= box->min[0] && point->x <= box->max[0] && point->z >= box->min[2] && point->z <= box->max[2];
}
#endif

#if defined(SDW_INLINE_FREE_OVERLAP4_S32_S32_S32_S32) && !defined(SDW_INLINE_FREE_OVERLAP4_S32_S32_S32_S32_DEFINED)
#define SDW_INLINE_FREE_OVERLAP4_S32_S32_S32_S32_DEFINED
/* BYTES(inline): arguments evaluate last to first and all four stay live together
 * (Wolf, esi/edi at 0x47e1e7). The result is a sign-bit mask. */
inline u32 Overlap4(s32 a, s32 b, s32 c, s32 d)
{
    return ~(a | b | c | d) & 0x80000000U;
}
#endif

#if defined(SDW_INLINE_COLLBOX_CONTAINSXZ_VEC3S) && !defined(SDW_INLINE_COLLBOX_CONTAINSXZ_VEC3S_DEFINED)
#define SDW_INLINE_COLLBOX_CONTAINSXZ_VEC3S_DEFINED
inline s32 CollBox::ContainsXZ(Vec3s *point)
{
    return point->x >= min.x && point->x <= max.x && point->z >= min.z && point->z <= max.z;
}
#endif

#if defined(SDW_INLINE_COLLBOX_CONTAINSXZ_CONST_VEC3S) && !defined(SDW_INLINE_COLLBOX_CONTAINSXZ_CONST_VEC3S_DEFINED)
#define SDW_INLINE_COLLBOX_CONTAINSXZ_CONST_VEC3S_DEFINED
inline s32 CollBox::ContainsXZ(const Vec3s *point)
{
    return point->x >= min.x && point->x <= max.x && point->z >= min.z && point->z <= max.z;
}
#endif
