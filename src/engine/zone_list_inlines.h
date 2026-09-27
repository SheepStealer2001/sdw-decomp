/* Shared inline definitions. Select helpers before inclusion, then undefine their selectors.
 * BYTES(inline): repeated inclusion preserves each caller's dependency visibility and definition order.
 * Per-helper guards prevent redefinition when a later include selects more helpers. */

#if defined(SDW_INLINE_ZONELIST_CLEAR) && !defined(SDW_INLINE_ZONELIST_CLEAR_DEFINED)
#define SDW_INLINE_ZONELIST_CLEAR_DEFINED
inline void ZoneList::Clear()
{
    boxes = 0;
    count = 0;
}
#endif

#if defined(SDW_INLINE_ZONELIST_CONTAINS_VEC3S) && !defined(SDW_INLINE_ZONELIST_CONTAINS_VEC3S_DEFINED)
#define SDW_INLINE_ZONELIST_CONTAINS_VEC3S_DEFINED
inline Box *ZoneList::Contains(Vec3s *point)
{
    return BoxList_FindContainingPoint(point, boxes, count);
}
#endif

#if defined(SDW_INLINE_ZONELIST_CONTAINSXZ_VEC3S) && !defined(SDW_INLINE_ZONELIST_CONTAINSXZ_VEC3S_DEFINED)
#define SDW_INLINE_ZONELIST_CONTAINSXZ_VEC3S_DEFINED
inline Box *ZoneList::ContainsXZ(Vec3s *point)
{
    return BoxList_FindContainingPointXZ(point, boxes, count);
}
#endif

#if defined(SDW_INLINE_ZONELIST_FIND_VEC3S) && !defined(SDW_INLINE_ZONELIST_FIND_VEC3S_DEFINED)
#define SDW_INLINE_ZONELIST_FIND_VEC3S_DEFINED
inline Box *ZoneList::Find(Vec3s *point)
{
    return BoxList_FindContainingPoint(point, boxes, count);
}
#endif

#if defined(SDW_INLINE_ZONELIST_FINDCONTAINING_VEC3S) && !defined(SDW_INLINE_ZONELIST_FINDCONTAINING_VEC3S_DEFINED)
#define SDW_INLINE_ZONELIST_FINDCONTAINING_VEC3S_DEFINED
/* BYTES(inline): the list address is kept in a temporary, and count/boxes are
 * read through it (Wolf climb zones, 0x48af87-0x48afa7). */
inline Box *ZoneList::FindContaining(Vec3s *point)
{
    return BoxList_FindContainingPoint(point, boxes, count);
}
#endif

#if defined(SDW_INLINE_ZONELIST_FINDCONTAININGXZ_VEC3S) && !defined(SDW_INLINE_ZONELIST_FINDCONTAININGXZ_VEC3S_DEFINED)
#define SDW_INLINE_ZONELIST_FINDCONTAININGXZ_VEC3S_DEFINED
inline Box *ZoneList::FindContainingXZ(Vec3s *p)
{
    return BoxList_FindContainingPointXZ(p, boxes, count);
}
#endif

#if defined(SDW_INLINE_ZONELIST_LOAD_U16) && !defined(SDW_INLINE_ZONELIST_LOAD_U16_DEFINED)
#define SDW_INLINE_ZONELIST_LOAD_U16_DEFINED
inline void ZoneList::Load(u16 id)
{
    /* cast kept: an export id list holds record pointers of any kind; this one lists boxes */
    boxes = (Box **)Scn_FindIdList(id, &count);
}
#endif

#if defined(SDW_INLINE_ZONELIST_LOAD_U32) && !defined(SDW_INLINE_ZONELIST_LOAD_U32_DEFINED)
#define SDW_INLINE_ZONELIST_LOAD_U32_DEFINED
/* BYTES(inline): the list receiver has a stack temporary in both LightSpot
 * expansions (0x4f6e92 / 0x4f6713). */
inline void ZoneList::Load(u32 id)
{
    /* cast kept: an export id list holds record pointers of any kind; this one lists boxes */
    boxes = (Box **)Scn_FindIdList((u16)id, &count);
}
#endif

#if defined(SDW_INLINE_ZONELIST_RESOLVE_U32) && !defined(SDW_INLINE_ZONELIST_RESOLVE_U32_DEFINED)
#define SDW_INLINE_ZONELIST_RESOLVE_U32_DEFINED
/* BYTES(inline): resolving the id list keeps a receiver temporary
 * (Daffy Level 09, 0x43a147; Snowball, 0x4f57c6). */
inline void ZoneList::Resolve(u32 id)
{
    /* cast kept: an export id list holds record pointers of any kind; this one lists boxes */
    boxes = (Box **)Scn_FindIdList((u16)id, &count);
}
#endif
