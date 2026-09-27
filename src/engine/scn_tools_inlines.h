/* Shared inline definitions. Select helpers before inclusion, then undefine their selectors.
 * BYTES(inline): repeated inclusion preserves each caller's dependency visibility and definition order.
 * Per-helper guards prevent redefinition when a later include selects more helpers. */

#if defined(SDW_INLINE_FREE_GETPROP_VOID_U32) && !defined(SDW_INLINE_FREE_GETPROP_VOID_U32_DEFINED)
#define SDW_INLINE_FREE_GETPROP_VOID_U32_DEFINED
inline u32 GetProp(void *record, u32 offset)
{
    /* cast kept: designer properties are 4-byte slots at byte offsets of the raw WAR record */
    return *(u32 *)((u8 *)record + offset + 0x14);
}
#endif

#if defined(SDW_INLINE_FREE_INDEXEDPROP_VOID_U16) && !defined(SDW_INLINE_FREE_INDEXEDPROP_VOID_U16_DEFINED)
#define SDW_INLINE_FREE_INDEXEDPROP_VOID_U16_DEFINED
inline u32 IndexedProp(void *record, u16 offset)
{
    /* cast kept: designer properties are 4-byte slots at byte offsets of the raw WAR record */
    return *(u32 *)((u8 *)record + offset + 0x14);
}
#endif

#if defined(SDW_INLINE_FREE_PROPU32_VOID_U32) && !defined(SDW_INLINE_FREE_PROPU32_VOID_U32_DEFINED)
#define SDW_INLINE_FREE_PROPU32_VOID_U32_DEFINED
#if SDW_INLINE_FREE_PROPU32_VOID_U32 == 1
/* BYTES(slot-group): this expansion retains the grouped result/offset locals. */
inline u32 PropU32(void *record, u32 fieldOffset)
{
    struct ReadWork {
        u32 result, offset;
    } read;
    read.offset = fieldOffset;
    /* cast kept: designer properties are 4-byte slots at byte offsets of the raw WAR record */
    read.result = *(u32 *)((u8 *)record + read.offset + 0x14);
    return read.result;
}
#elif SDW_INLINE_FREE_PROPU32_VOID_U32 == 2
/* BYTES(inline): direct-return expansion, without the grouped read locals. */
inline u32 PropU32(void *rec, u32 off)
{
    /* cast kept: designer properties are 4-byte slots at byte offsets of the raw WAR record */
    return *(u32 *)((u8 *)rec + off + 0x14);
}
#endif
#endif

#if defined(SDW_INLINE_FREE_PROPERTY_VOID_U32) && !defined(SDW_INLINE_FREE_PROPERTY_VOID_U32_DEFINED)
#define SDW_INLINE_FREE_PROPERTY_VOID_U32_DEFINED
static inline u32 Property(void *record, u32 offset)
{
    /* cast kept: designer properties are 4-byte slots at byte offsets of the raw WAR record */
    return *(u32 *)((u8 *)record + offset + 0x14);
}
#endif

#if defined(SDW_INLINE_FREE_PROPERTYU32_VOID_U32) && !defined(SDW_INLINE_FREE_PROPERTYU32_VOID_U32_DEFINED)
#define SDW_INLINE_FREE_PROPERTYU32_VOID_U32_DEFINED
inline u32 PropertyU32(void *record, u32 offset)
{
    /* cast kept (both): designer properties are 4-byte slots at byte offsets of the raw WAR record */
    return *(u32 *)((u8 *)record + offset + 0x14);
}
#endif

#if defined(SDW_INLINE_FREE_READINTPROPERTY_VOID_U32) && !defined(SDW_INLINE_FREE_READINTPROPERTY_VOID_U32_DEFINED)
#define SDW_INLINE_FREE_READINTPROPERTY_VOID_U32_DEFINED
inline s32 ReadIntProperty(void *record, u32 offset)
{
    /* cast kept: designer properties are 4-byte slots at byte offsets of the raw WAR record */
    return *(s32 *)((u8 *)record + offset + 0x14);
}
#endif

#if defined(SDW_INLINE_FREE_READPROPERTY_VOID_U32) && !defined(SDW_INLINE_FREE_READPROPERTY_VOID_U32_DEFINED)
#define SDW_INLINE_FREE_READPROPERTY_VOID_U32_DEFINED
#if SDW_INLINE_FREE_READPROPERTY_VOID_U32 == 1
/* BYTES(inline): retain the explicit offset local in this expansion. */
inline u32 ReadProperty(void *record, u32 field)
{
    u32 offset = field;
    /* cast kept: designer properties are 4-byte slots at byte offsets of the raw WAR record */
    return *(u32 *)((u8 *)record + offset + 0x14);
}
#elif SDW_INLINE_FREE_READPROPERTY_VOID_U32 == 2
/* BYTES(inline): this expansion uses the offset parameter directly. */
inline u32 ReadProperty(void *record, u32 offset)
{
    /* cast kept (both): designer properties are 4-byte slots at byte offsets of the raw WAR record */
    return *(u32 *)((u8 *)record + offset + 0x14);
}
#endif
#endif

#if defined(SDW_INLINE_FREE_SCN_GETPROPID_U16_U32) && !defined(SDW_INLINE_FREE_SCN_GETPROPID_U16_U32_DEFINED)
#define SDW_INLINE_FREE_SCN_GETPROPID_U16_U32_DEFINED
inline u32 Scn_GetPropId(u16 *props, u32 offset)
{
    /* cast kept: a designer property is a 4-byte slot at a byte offset of the raw WAR record */
    return *(u32 *)((u8 *)props + offset + 0x14);
}
#endif

#if defined(SDW_INLINE_FREE_SCN_GETPROPS32_VOID_U32) && !defined(SDW_INLINE_FREE_SCN_GETPROPS32_VOID_U32_DEFINED)
#define SDW_INLINE_FREE_SCN_GETPROPS32_VOID_U32_DEFINED
/* BYTES(cast): u32 offset gives TriggedStone a stack temporary (0x5022f9);
 * s32 folds it into the address. This was observed; the mechanism is not established. */
inline s32 Scn_GetPropS32(void *props, u32 offset)
{
    return *(s32 *)((u8 *)props + offset + 0x14); /* cast kept: properties are 4-byte slots of the raw record */
}
#endif

#if defined(SDW_INLINE_FREE_SCN_GETPROPU32_U16_U32) && !defined(SDW_INLINE_FREE_SCN_GETPROPU32_U16_U32_DEFINED)
#define SDW_INLINE_FREE_SCN_GETPROPU32_U16_U32_DEFINED
/* BYTES(inline): the unsigned offset gives a constant int argument a stack
 * temporary (Wolf, mov [ebp-0x28],8 at 0x48886a), unlike a signed offset. */
inline u32 Scn_GetPropU32(u16 *props, u32 offset)
{
    /* cast kept (both): designer properties are 4-byte slots at byte offsets of the raw WAR record */
    return *(u32 *)((u8 *)props + offset + 0x14);
}
#endif

#if defined(SDW_INLINE_FREE_SCN_GETPROPU32_VOID_U32) && !defined(SDW_INLINE_FREE_SCN_GETPROPU32_VOID_U32_DEFINED)
#define SDW_INLINE_FREE_SCN_GETPROPU32_VOID_U32_DEFINED
inline u32 Scn_GetPropU32(void *props, u32 propOffset)
{
    return *(u32 *)((u8 *)props + propOffset + 0x14); /* cast kept: a property is a 4-byte slot of the raw record */
}
#endif

#if defined(SDW_INLINE_FREE_SETPROP_VOID_U32_U32) && !defined(SDW_INLINE_FREE_SETPROP_VOID_U32_U32_DEFINED)
#define SDW_INLINE_FREE_SETPROP_VOID_U32_U32_DEFINED
inline void SetProp(void *record, u32 offset, u32 value)
{
    /* cast kept: designer properties are 4-byte slots at byte offsets of the raw WAR record */
    *(u32 *)((u8 *)record + offset + 0x14) = value;
}
#endif
