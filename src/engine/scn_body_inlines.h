/* Shared inline definitions. Select helpers before inclusion, then undefine their selectors.
 * BYTES(inline): repeated inclusion preserves each caller's dependency visibility and definition order.
 * Per-helper guards prevent redefinition when a later include selects more helpers. */

#if defined(SDW_INLINE_SCNBODY_ANIMFLAGS_U16) && !defined(SDW_INLINE_SCNBODY_ANIMFLAGS_U16_DEFINED)
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16_DEFINED
/* BYTES(inline): the u16 mask is materialized in a register before the AND
 * (Wolf, 0x48c3d1). */
inline s32 ScnBody::AnimFlags(u16 mask)
{
    return anim.flags & mask;
}
#endif

#if defined(SDW_INLINE_SCNBODY_ANIMID) && !defined(SDW_INLINE_SCNBODY_ANIMID_DEFINED)
#define SDW_INLINE_SCNBODY_ANIMID_DEFINED
/* BYTES(inline): each id read has a stack copy (Wolf, 0x48ad11 / 0x48cfc6). */
inline u16 ScnBody::AnimId()
{
    return anim.animId;
}
#endif

#if defined(SDW_INLINE_SCNBODY_CURRENTANIM) && !defined(SDW_INLINE_SCNBODY_CURRENTANIM_DEFINED)
#define SDW_INLINE_SCNBODY_CURRENTANIM_DEFINED
inline u16 ScnBody::CurrentAnim()
{
    return anim.animId;
}
#endif

#if defined(SDW_INLINE_SCNBODY_GETANIMID) && !defined(SDW_INLINE_SCNBODY_GETANIMID_DEFINED)
#define SDW_INLINE_SCNBODY_GETANIMID_DEFINED
inline u16 ScnBody::GetAnimId()
{
    return anim.animId;
}
#endif

#if defined(SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32) && !defined(SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32_DEFINED)
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32_DEFINED
/* BYTES(inline): keeping the option word and Anim_Start call inside this wrapper
 * gives Cine_Update its register allocation and Ghost its option-word stack slot. */
inline void ScnBody::PlayAnim(u16 id, s32 loop, s32 blend)
{
    u32 options = 0;
    if (loop)
        options |= ANIM_SET_LOOP;
    if (blend)
        options |= ANIM_SET_BLEND;
    Anim_Start(Inst(), &anim, id, options);
}
#endif
