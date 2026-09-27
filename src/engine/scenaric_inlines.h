/* Shared inline definitions. Select helpers before inclusion, then undefine their selectors.
 * BYTES(inline): repeated inclusion preserves each caller's dependency visibility and definition order.
 * Per-helper guards prevent redefinition when a later include selects more helpers. */

#if defined(SDW_INLINE_FREE_ISHELD_SCNOBJECT_VEC3S) && !defined(SDW_INLINE_FREE_ISHELD_SCNOBJECT_VEC3S_DEFINED)
#define SDW_INLINE_FREE_ISHELD_SCNOBJECT_VEC3S_DEFINED
inline s32 IsHeld(ScnObject *obj, Vec3s *holder)
{
    if (!obj->InstFlags(INST_F_ATTACHED))
        return 0;
    holder->x = obj->attachLink->parentObj->pos.x;
    holder->y = obj->attachLink->parentObj->pos.y;
    holder->z = obj->attachLink->parentObj->pos.z;
    return 1;
}
#endif

#if defined(SDW_INLINE_FREE_SCENARIC_CLASSFLAGS_U16) && !defined(SDW_INLINE_FREE_SCENARIC_CLASSFLAGS_U16_DEFINED)
#define SDW_INLINE_FREE_SCENARIC_CLASSFLAGS_U16_DEFINED
/* BYTES(inline): paired with GetClassId, this produces the u32 flag temporary
 * after the u16 id temporary (ScnControllable, 0x49267d / 0x492694). */
inline u32 Scenaric_ClassFlags(u16 classId)
{
    return g_scenaricClassRegistry[classId].classFlags;
}
#endif

#if defined(SDW_INLINE_FREE_ZONE_GETLIST_U8) && !defined(SDW_INLINE_FREE_ZONE_GETLIST_U8_DEFINED)
#define SDW_INLINE_FREE_ZONE_GETLIST_U8_DEFINED
inline ZoneList *Zone_GetList(u8 type)
{
    return &g_waterZones[type];
}
#endif

#if defined(SDW_INLINE_FREE_ZONES_GET_U8) && !defined(SDW_INLINE_FREE_ZONES_GET_U8_DEFINED)
#define SDW_INLINE_FREE_ZONES_GET_U8_DEFINED
/* BYTES(inline): the narrow type argument is materialized before indexing
 * (Wolf, xor edx,edx at 0x48dcb0). */
inline ZoneList *Zones_Get(u8 type)
{
    return &g_waterZones[type];
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_ATTACHTO_SCNOBJECT_U8_CONST_VEC3S_VEC3S_U32_U32) && \
    !defined(SDW_INLINE_SCNOBJECT_ATTACHTO_SCNOBJECT_U8_CONST_VEC3S_VEC3S_U32_U32_DEFINED)
#define SDW_INLINE_SCNOBJECT_ATTACHTO_SCNOBJECT_U8_CONST_VEC3S_VEC3S_U32_U32_DEFINED
/* BYTES(view): const attachment tables stay in .rdata; this inline forwards to
 * the one non-const AttachTo symbol (SmallRock and Rocket). */
inline void ScnObject::AttachTo(ScnObject *parent, u8 joint, const Vec3s *offset, Vec3s *rotation, u32 arg, u32 arg2)
{
    AttachTo(parent, joint, (Vec3s *)offset, rotation, arg, arg2); /* cast kept: drops the const */
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_BROADCASTAROUND_S32_S32_U16_U32_VOID) && \
    !defined(SDW_INLINE_SCNOBJECT_BROADCASTAROUND_S32_S32_U16_U32_VOID_DEFINED)
#define SDW_INLINE_SCNOBJECT_BROADCASTAROUND_S32_S32_U16_U32_VOID_DEFINED
inline void ScnObject::BroadcastAround(s32 below, s32 above, u16 radius, u32 msg, void *arg)
{
    s16 minY;
    s16 maxY;
    maxY = pos.y + above;
    minY = pos.y - below;
    Scenaric_BroadcastInRadius(CLASSID_NONE, minY, maxY, radius, msg, arg, 0);
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_DROP_VEC3S) && !defined(SDW_INLINE_SCNOBJECT_DROP_VEC3S_DEFINED)
#define SDW_INLINE_SCNOBJECT_DROP_VEC3S_DEFINED
inline void ScnObject::Drop(Vec3s *where)
{
    Detach();
    SetPosition(where);
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_ENABLEBOXCOLLIDE_S32) && !defined(SDW_INLINE_SCNOBJECT_ENABLEBOXCOLLIDE_S32_DEFINED)
#define SDW_INLINE_SCNOBJECT_ENABLEBOXCOLLIDE_S32_DEFINED
inline void ScnObject::EnableBoxCollide(s32 on)
{
    if (on)
        flags &= (u16)~SCN_OF_NO_BOX_COLLIDE;
    else
        flags |= SCN_OF_NO_BOX_COLLIDE;
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_ENABLETINT_S32) && !defined(SDW_INLINE_SCNOBJECT_ENABLETINT_S32_DEFINED)
#define SDW_INLINE_SCNOBJECT_ENABLETINT_S32_DEFINED
inline void ScnObject::EnableTint(s32 on)
{
    if (on)
        InstFlagsSet(&inst_flags, INST_F_TINT);
    else
        InstFlagsClear(&inst_flags, INST_F_TINT);
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_FACING) && !defined(SDW_INLINE_SCNOBJECT_FACING_DEFINED)
#define SDW_INLINE_SCNOBJECT_FACING_DEFINED
#if SDW_INLINE_SCNOBJECT_FACING == 1
/* BYTES(inline): the direct return gives each read a fresh two-byte temporary
 * (Wolf, 0x48f955 / 0x48c3f9). */
inline s16 ScnObject::Facing()
{
    return rot.y;
}
#elif SDW_INLINE_SCNOBJECT_FACING == 2
/* BYTES(inline): this expansion retains a named result local for its stack layout. */
inline s16 ScnObject::Facing()
{
    s16 result = rot.y;
    return result;
}
#endif
#endif

#if defined(SDW_INLINE_SCNOBJECT_FIRSTBOX) && !defined(SDW_INLINE_SCNOBJECT_FIRSTBOX_DEFINED)
#define SDW_INLINE_SCNOBJECT_FIRSTBOX_DEFINED
inline CollBox *ScnObject::FirstBox()
{
    ModelBoxList *list = inst_model->boxes;
    if (list)
        return list->boxes;
    return 0;
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_FLAGSCLEAR_U32) && !defined(SDW_INLINE_SCNOBJECT_FLAGSCLEAR_U32_DEFINED)
#define SDW_INLINE_SCNOBJECT_FLAGSCLEAR_U32_DEFINED
/* BYTES(inline): ! is materialized as neg/sbb/inc before the caller's test
 * (ScnControllable, 0x4926bb; collision, 0x51a84f). */
inline s32 ScnObject::FlagsClear(u32 mask)
{
    return !(flags & mask);
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_GETCLASSID) && !defined(SDW_INLINE_SCNOBJECT_GETCLASSID_DEFINED)
#define SDW_INLINE_SCNOBJECT_GETCLASSID_DEFINED
/* BYTES(inline): the u16 return gets its own temporary before a class-flag lookup
 * (Wolf, 0x48be7d; ScnControllable, 0x49267d). */
inline u16 ScnObject::GetClassId()
{
    return classId;
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_GETFACING) && !defined(SDW_INLINE_SCNOBJECT_GETFACING_DEFINED)
#define SDW_INLINE_SCNOBJECT_GETFACING_DEFINED
inline s16 ScnObject::GetFacing()
{
    return rot.y;
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_GETFIRSTMODELBOXINLINE) && \
    !defined(SDW_INLINE_SCNOBJECT_GETFIRSTMODELBOXINLINE_DEFINED)
#define SDW_INLINE_SCNOBJECT_GETFIRSTMODELBOXINLINE_DEFINED
/* Source-only twin of the emitted GetFirstModelBox (0x4c1ec0): absence of a
 * list returns null; count is intentionally not tested. The emitted copy stays local. */
inline CollBox *ScnObject::GetFirstModelBoxInline()
{
    ModelBoxList *list = inst_model->boxes;
    if (list)
        return (CollBox *)list->boxes; /* cast kept: Box and CollBox are two views of one 16-byte record */
    return 0;
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_GETFLAGS) && !defined(SDW_INLINE_SCNOBJECT_GETFLAGS_DEFINED)
#define SDW_INLINE_SCNOBJECT_GETFLAGS_DEFINED
/* BYTES(inline): the u16 return is parked in a temporary by the /Od scene callers. */
inline u16 ScnObject::GetFlags()
{
    return flags;
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_GETHEADING) && !defined(SDW_INLINE_SCNOBJECT_GETHEADING_DEFINED)
#define SDW_INLINE_SCNOBJECT_GETHEADING_DEFINED
inline s16 ScnObject::GetHeading()
{
    return rot.y;
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_GETMODELBOX) && !defined(SDW_INLINE_SCNOBJECT_GETMODELBOX_DEFINED)
#define SDW_INLINE_SCNOBJECT_GETMODELBOX_DEFINED
inline CollBox *ScnObject::GetModelBox()
{
    ModelBoxList *list = inst_model->boxes;
    if (list)
        return list->boxes;
    return 0;
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_GETMODELBOXES_COLLBOX_U32) && \
    !defined(SDW_INLINE_SCNOBJECT_GETMODELBOXES_COLLBOX_U32_DEFINED)
#define SDW_INLINE_SCNOBJECT_GETMODELBOXES_COLLBOX_U32_DEFINED
inline void ScnObject::GetModelBoxes(CollBox **out, u32 *count)
{
    ModelBoxList *list = inst_model->boxes;
    if (!list) {
        *count = 0;
        *out = 0;
    } else {
        *count = list->count;
        *out = list->boxes;
    }
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_GETMODELBOXES_U32) && !defined(SDW_INLINE_SCNOBJECT_GETMODELBOXES_U32_DEFINED)
#define SDW_INLINE_SCNOBJECT_GETMODELBOXES_U32_DEFINED
inline CollBox *ScnObject::GetModelBoxes(u32 *count)
{
    ModelBoxList *list = inst_model->boxes;
    if (!list) {
        *count = 0;
        return 0;
    }
    *count = list->count;
    return list->boxes;
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_GETPARENT) && !defined(SDW_INLINE_SCNOBJECT_GETPARENT_DEFINED)
#define SDW_INLINE_SCNOBJECT_GETPARENT_DEFINED
inline ScnObject *ScnObject::GetParent()
{
    if (!InstFlags(INST_F_ATTACHED))
        return 0;
    return attachLink->parentObj;
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_INWORLD) && !defined(SDW_INLINE_SCNOBJECT_INWORLD_DEFINED)
#define SDW_INLINE_SCNOBJECT_INWORLD_DEFINED
inline s32 ScnObject::InWorld()
{
    return (flags & SCN_OF_IN_WORLD) != 0;
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_INSTFLAGS_U16) && !defined(SDW_INLINE_SCNOBJECT_INSTFLAGS_U16_DEFINED)
#define SDW_INLINE_SCNOBJECT_INSTFLAGS_U16_DEFINED
/* BYTES(inline): the u16 mask is loaded into a register before the AND
 * (ScnControllable, 0x4926c8; Wolf, 0x48764d / 0x484da7). */
inline s32 ScnObject::InstFlags(u16 mask)
{
    return inst_flags & mask;
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_INSTANCEFLAGS_U16) && !defined(SDW_INLINE_SCNOBJECT_INSTANCEFLAGS_U16_DEFINED)
#define SDW_INLINE_SCNOBJECT_INSTANCEFLAGS_U16_DEFINED
inline s32 ScnObject::InstanceFlags(u16 mask)
{
    return inst_flags & mask;
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_ISACTIVE) && !defined(SDW_INLINE_SCNOBJECT_ISACTIVE_DEFINED)
#define SDW_INLINE_SCNOBJECT_ISACTIVE_DEFINED
inline s32 ScnObject::IsActive()
{
    return (flags & (SCN_OF_HIDDEN2 | SCN_OF_HIDDEN)) == 0;
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_ISCOLLIDABLE) && !defined(SDW_INLINE_SCNOBJECT_ISCOLLIDABLE_DEFINED)
#define SDW_INLINE_SCNOBJECT_ISCOLLIDABLE_DEFINED
inline s32 ScnObject::IsCollidable()
{
    return !(flags & SCN_OF_NO_BOX_COLLIDE);
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_ISINWORLD) && !defined(SDW_INLINE_SCNOBJECT_ISINWORLD_DEFINED)
#define SDW_INLINE_SCNOBJECT_ISINWORLD_DEFINED
/* BYTES(inline): normalize the world flag before testing it; the Wolf expansion
 * materializes 0/1 with neg/sbb/neg (0x48617b). */
inline s32 ScnObject::IsInWorld()
{
    return (flags & SCN_OF_IN_WORLD) != 0;
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_ISKEPT) && !defined(SDW_INLINE_SCNOBJECT_ISKEPT_DEFINED)
#define SDW_INLINE_SCNOBJECT_ISKEPT_DEFINED
inline s32 ScnObject::IsKept()
{
    return (flags & SCN_OF_KEPT) != 0;
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_ISSOUNDPLAYING_U16) && !defined(SDW_INLINE_SCNOBJECT_ISSOUNDPLAYING_U16_DEFINED)
#define SDW_INLINE_SCNOBJECT_ISSOUNDPLAYING_U16_DEFINED
inline s32 ScnObject::IsSoundPlaying(u16 handle)
{
    return Sound_IsPlaying(handle);
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_ISVISIBLE) && !defined(SDW_INLINE_SCNOBJECT_ISVISIBLE_DEFINED)
#define SDW_INLINE_SCNOBJECT_ISVISIBLE_DEFINED
inline s32 ScnObject::IsVisible()
{
    return (flags & (SCN_OF_HIDDEN2 | SCN_OF_HIDDEN)) == 0;
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_PLAYSOUND_U16_U16_U8_S32) && \
    !defined(SDW_INLINE_SCNOBJECT_PLAYSOUND_U16_U16_U8_S32_DEFINED)
#define SDW_INLINE_SCNOBJECT_PLAYSOUND_U16_U16_U8_S32_DEFINED
inline u16 ScnObject::PlaySound(u16 id, u16 volume, u8 flags, s32 pitch)
{
    return Sound_Play(id, this, volume, flags, pitch);
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_RECORD) && !defined(SDW_INLINE_SCNOBJECT_RECORD_DEFINED)
#define SDW_INLINE_SCNOBJECT_RECORD_DEFINED
inline void *ScnObject::Record()
{
    return record;
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_SETATTACHMENT_U8_CONST_VEC3S_CONST_VEC3S_S32_CONST_VEC3S) && \
    !defined(SDW_INLINE_SCNOBJECT_SETATTACHMENT_U8_CONST_VEC3S_CONST_VEC3S_S32_CONST_VEC3S_DEFINED)
#define SDW_INLINE_SCNOBJECT_SETATTACHMENT_U8_CONST_VEC3S_CONST_VEC3S_S32_CONST_VEC3S_DEFINED
inline void ScnObject::SetAttachment(u8 joint, const Vec3s *localOffset, const Vec3s *rotation, s32 rootRotation,
                                     const Vec3s *worldOffset)
{
    AttachLink_SetParams(attachLink, joint, localOffset, rotation, rootRotation, worldOffset);
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_SETBOXCOLLIDE_S32) && !defined(SDW_INLINE_SCNOBJECT_SETBOXCOLLIDE_S32_DEFINED)
#define SDW_INLINE_SCNOBJECT_SETBOXCOLLIDE_S32_DEFINED
inline void ScnObject::SetBoxCollide(s32 on)
{
    if (on)
        flags &= (u16)~SCN_OF_NO_BOX_COLLIDE;
    else
        flags |= SCN_OF_NO_BOX_COLLIDE;
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_SETCOLLIDABLE_S32) && !defined(SDW_INLINE_SCNOBJECT_SETCOLLIDABLE_S32_DEFINED)
#define SDW_INLINE_SCNOBJECT_SETCOLLIDABLE_S32_DEFINED
inline void ScnObject::SetCollidable(s32 on)
{
    if (on)
        flags &= (u16)~SCN_OF_NO_BOX_COLLIDE;
    else
        flags |= SCN_OF_NO_BOX_COLLIDE;
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_SETCOLLISION_S32) && !defined(SDW_INLINE_SCNOBJECT_SETCOLLISION_S32_DEFINED)
#define SDW_INLINE_SCNOBJECT_SETCOLLISION_S32_DEFINED
inline void ScnObject::SetCollision(s32 on)
{
    if (on)
        flags &= (u16)~SCN_OF_NO_BOX_COLLIDE;
    else
        flags |= SCN_OF_NO_BOX_COLLIDE;
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_SETCONTACTENABLED_S32) && !defined(SDW_INLINE_SCNOBJECT_SETCONTACTENABLED_S32_DEFINED)
#define SDW_INLINE_SCNOBJECT_SETCONTACTENABLED_S32_DEFINED
inline void ScnObject::SetContactEnabled(s32 on)
{
    if (on)
        flags &= (u16)~SCN_OF_NO_BOX_COLLIDE;
    else
        flags |= SCN_OF_NO_BOX_COLLIDE;
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_SETDRAWMODE_U32) && !defined(SDW_INLINE_SCNOBJECT_SETDRAWMODE_U32_DEFINED)
#define SDW_INLINE_SCNOBJECT_SETDRAWMODE_U32_DEFINED
inline void ScnObject::SetDrawMode(u32 mode)
{
    partHeight = (u8)(mode >> 4);
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_SETFACING_S16) && !defined(SDW_INLINE_SCNOBJECT_SETFACING_S16_DEFINED)
#define SDW_INLINE_SCNOBJECT_SETFACING_S16_DEFINED
/* BYTES(inline): a non-variable angle gets a two-byte argument temporary
 * (Wolf, 0x48bf08 / 0x48fd3c). */
inline void ScnObject::SetFacing(s16 f)
{
    rot.y = f;
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_SETFACING_U16) && !defined(SDW_INLINE_SCNOBJECT_SETFACING_U16_DEFINED)
#define SDW_INLINE_SCNOBJECT_SETFACING_U16_DEFINED
inline void ScnObject::SetFacing(u16 f)
{
    rot.y = f;
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_SETFLAG40_S32) && !defined(SDW_INLINE_SCNOBJECT_SETFLAG40_S32_DEFINED)
#define SDW_INLINE_SCNOBJECT_SETFLAG40_S32_DEFINED
inline void ScnObject::SetFlag40(s32 on)
{
    if (on)
        flags |= SCN_OF_NO_DIST_CULL;
    else
        flags &= (u16)~SCN_OF_NO_DIST_CULL;
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_SETHEADING_S16) && !defined(SDW_INLINE_SCNOBJECT_SETHEADING_S16_DEFINED)
#define SDW_INLINE_SCNOBJECT_SETHEADING_S16_DEFINED
inline void ScnObject::SetHeading(s16 value)
{
    rot.y = value;
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_SETINSTFLAG_U16_S32) && !defined(SDW_INLINE_SCNOBJECT_SETINSTFLAG_U16_S32_DEFINED)
#define SDW_INLINE_SCNOBJECT_SETINSTFLAG_U16_S32_DEFINED
/* BYTES(inline): each branch takes its own flag-word pointer; the mask is a
 * parameter and the complement narrows through u16 (Shark, 0x476bde-0x476bf0).
 * Ghost's nested SetTint expansion also relies on those pointer locals. */
inline void ScnObject::SetInstFlag(u16 mask, s32 on)
{
    if (on) {
        u16 *f = &inst_flags;
        *f |= mask;
    } else {
        u16 *f = &inst_flags;
        *f &= (u16)~mask;
    }
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_SETKEPT_S32) && !defined(SDW_INLINE_SCNOBJECT_SETKEPT_S32_DEFINED)
#define SDW_INLINE_SCNOBJECT_SETKEPT_S32_DEFINED
inline void ScnObject::SetKept(s32 on)
{
    if (on)
        flags |= SCN_OF_KEPT;
    else
        flags &= (u16)~SCN_OF_KEPT;
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_SETMOVEMENTENABLED_S32) && \
    !defined(SDW_INLINE_SCNOBJECT_SETMOVEMENTENABLED_S32_DEFINED)
#define SDW_INLINE_SCNOBJECT_SETMOVEMENTENABLED_S32_DEFINED
inline void ScnObject::SetMovementEnabled(s32 on)
{
    if (on)
        flags &= (u16)~SCN_OF_NO_BOX_COLLIDE;
    else
        flags |= SCN_OF_NO_BOX_COLLIDE;
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_SETNOCULL_S32) && !defined(SDW_INLINE_SCNOBJECT_SETNOCULL_S32_DEFINED)
#define SDW_INLINE_SCNOBJECT_SETNOCULL_S32_DEFINED
inline void ScnObject::SetNoCull(s32 on)
{
    if (on)
        flags |= SCN_OF_NO_DIST_CULL;
    else
        flags &= (u16)~SCN_OF_NO_DIST_CULL;
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_SETNODISTCULL_S32) && !defined(SDW_INLINE_SCNOBJECT_SETNODISTCULL_S32_DEFINED)
#define SDW_INLINE_SCNOBJECT_SETNODISTCULL_S32_DEFINED
inline void ScnObject::SetNoDistCull(s32 on)
{
    if (on)
        flags |= SCN_OF_NO_DIST_CULL;
    else
        flags &= (u16)~SCN_OF_NO_DIST_CULL;
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_SETPARTHEIGHT_U32) && !defined(SDW_INLINE_SCNOBJECT_SETPARTHEIGHT_U32_DEFINED)
#define SDW_INLINE_SCNOBJECT_SETPARTHEIGHT_U32_DEFINED
/* BYTES(inline): the u32 argument is loaded and shifted unsigned before the
 * byte store (Wolf, 0x47e54c). */
inline void ScnObject::SetPartHeight(u32 value)
{
    partHeight = value >> 4;
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_SETPOS_VEC3S) && !defined(SDW_INLINE_SCNOBJECT_SETPOS_VEC3S_DEFINED)
#define SDW_INLINE_SCNOBJECT_SETPOS_VEC3S_DEFINED
inline void ScnObject::SetPos(Vec3s *p)
{
    pos = *p;
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_SETPOS_CONST_VEC3S) && !defined(SDW_INLINE_SCNOBJECT_SETPOS_CONST_VEC3S_DEFINED)
#define SDW_INLINE_SCNOBJECT_SETPOS_CONST_VEC3S_DEFINED
inline void ScnObject::SetPos(const Vec3s *p)
{
    pos = *p;
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_SETROTATION_VEC3S) && !defined(SDW_INLINE_SCNOBJECT_SETROTATION_VEC3S_DEFINED)
#define SDW_INLINE_SCNOBJECT_SETROTATION_VEC3S_DEFINED
inline void ScnObject::SetRotation(Vec3s *rotation)
{
    rot = *rotation;
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_SETROTATION_CONST_VEC3S) && \
    !defined(SDW_INLINE_SCNOBJECT_SETROTATION_CONST_VEC3S_DEFINED)
#define SDW_INLINE_SCNOBJECT_SETROTATION_CONST_VEC3S_DEFINED
inline void ScnObject::SetRotation(const Vec3s &value)
{
    rot = value;
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_SETSOUNDRATE_U16_S32) && !defined(SDW_INLINE_SCNOBJECT_SETSOUNDRATE_U16_S32_DEFINED)
#define SDW_INLINE_SCNOBJECT_SETSOUNDRATE_U16_S32_DEFINED
inline void ScnObject::SetSoundRate(u16 handle, s32 rate)
{
    Sound_SetRate(handle, rate);
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_SETTINT_U32_S16_S32) && !defined(SDW_INLINE_SCNOBJECT_SETTINT_U32_S16_S32_DEFINED)
#define SDW_INLINE_SCNOBJECT_SETTINT_U32_S16_S32_DEFINED
#if SDW_INLINE_SCNOBJECT_SETTINT_U32_S16_S32 == 1
/* BYTES(inline): preserve the InstFlags_* expansions and their flag-word arguments. */
inline void ScnObject::SetTint(u32 color, s16 amount, s32 on)
{
    tintColor = color;
    tintAmount = amount;
    if (on)
        InstFlags_Set(&inst_flags, INST_F_TINT);
    else
        InstFlags_Clear(&inst_flags, INST_F_TINT);
}
#elif SDW_INLINE_SCNOBJECT_SETTINT_U32_S16_S32 == 2
/* BYTES(inline): each arm takes the flag address again (Wolf, 0x484114 / 0x484135);
 * a computed amount has its own temporary (0x484213). Keep the Bits16_* expansions. */
inline void ScnObject::SetTint(u32 color, s16 amount, s32 on)
{
    tintColor = color;
    tintAmount = amount;
    if (on)
        Bits16_Set(&inst_flags, INST_F_TINT);
    else
        Bits16_Clear(&inst_flags, INST_F_TINT);
}
#elif SDW_INLINE_SCNOBJECT_SETTINT_U32_S16_S32 == 3
/* BYTES(inline): preserve the InstFlagsSet/InstFlagsClear expansions used by this variant. */
inline void ScnObject::SetTint(u32 color, s16 amount, s32 on)
{
    tintColor = color;
    tintAmount = amount;
    if (on)
        InstFlagsSet(&inst_flags, INST_F_TINT);
    else
        InstFlagsClear(&inst_flags, INST_F_TINT);
}
#endif
#endif

#if defined(SDW_INLINE_SCNOBJECT_SETTINTOVERRIDE_S32) && !defined(SDW_INLINE_SCNOBJECT_SETTINTOVERRIDE_S32_DEFINED)
#define SDW_INLINE_SCNOBJECT_SETTINTOVERRIDE_S32_DEFINED
inline void ScnObject::SetTintOverride(s32 on)
{
    if (on)
        Flags16_Set(&inst_flags, INST_F_TINT);
    else
        Flags16_Clear(&inst_flags, INST_F_TINT);
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_SETTINTED_S32) && !defined(SDW_INLINE_SCNOBJECT_SETTINTED_S32_DEFINED)
#define SDW_INLINE_SCNOBJECT_SETTINTED_S32_DEFINED
inline void ScnObject::SetTinted(s32 on)
{
    if (on)
        InstFlags_Set(&inst_flags, INST_F_TINT);
    else
        InstFlags_Clear(&inst_flags, INST_F_TINT);
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32) && !defined(SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32_DEFINED)
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32_DEFINED
/* BYTES(cast): complement the masks as u16, not a bare ~: WheelDummy uses
 * and edx,0xbfff (0x5096ab). The switch still produces a jump table for constant
 * arguments (Bullet, 0x49e136; Gossamer Level 08, 0x4527da / 0x4527ea). */
inline void ScnObject::SetUpdateMode(s32 mode)
{
    switch (mode) {
        case SCN_UPD_NORMAL:
            flags &= (u16)~SCN_OF_ALWAYS_UPDATE;
            flags &= (u16)~SCN_OF_NEVER_UPDATE;
            break;
        case SCN_UPD_ALWAYS:
            flags |= SCN_OF_ALWAYS_UPDATE;
            flags &= (u16)~SCN_OF_NEVER_UPDATE;
            break;
        case SCN_UPD_NEVER:
            flags &= (u16)~SCN_OF_ALWAYS_UPDATE;
            flags |= SCN_OF_NEVER_UPDATE;
            break;
        case SCN_UPD_CINE:
            flags |= SCN_OF_CINE_UPDATE;
            break;
    }
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U32) && !defined(SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U32_DEFINED)
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U32_DEFINED
/* BYTES(inline): retain the unsigned mode's expansion temporaries and the
 * narrowed flag complements, as in the signed overload. */
inline void ScnObject::SetUpdateMode(u32 mode)
{
    switch (mode) {
        case SCN_UPD_NORMAL:
            flags &= (u16)~SCN_OF_ALWAYS_UPDATE;
            flags &= (u16)~SCN_OF_NEVER_UPDATE;
            break;
        case SCN_UPD_ALWAYS:
            flags |= SCN_OF_ALWAYS_UPDATE;
            flags &= (u16)~SCN_OF_NEVER_UPDATE;
            break;
        case SCN_UPD_NEVER:
            flags &= (u16)~SCN_OF_ALWAYS_UPDATE;
            flags |= SCN_OF_NEVER_UPDATE;
            break;
        case SCN_UPD_CINE:
            flags |= SCN_OF_CINE_UPDATE;
            break;
    }
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8) && !defined(SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8_DEFINED)
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8_DEFINED
/* BYTES(inline): the byte mode is materialized even when constant
 * (Bridge, xor for zero at 0x49d3f2); keep the narrowed flag complements. */
inline void ScnObject::SetUpdateMode(u8 mode)
{
    switch (mode) {
        case SCN_UPD_NORMAL:
            flags &= (u16)~SCN_OF_ALWAYS_UPDATE;
            flags &= (u16)~SCN_OF_NEVER_UPDATE;
            break;
        case SCN_UPD_ALWAYS:
            flags |= SCN_OF_ALWAYS_UPDATE;
            flags &= (u16)~SCN_OF_NEVER_UPDATE;
            break;
        case SCN_UPD_NEVER:
            flags &= (u16)~SCN_OF_ALWAYS_UPDATE;
            flags |= SCN_OF_NEVER_UPDATE;
            break;
        case SCN_UPD_CINE:
            flags |= SCN_OF_CINE_UPDATE;
            break;
    }
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_SETVISIBLE_S32) && !defined(SDW_INLINE_SCNOBJECT_SETVISIBLE_S32_DEFINED)
#define SDW_INLINE_SCNOBJECT_SETVISIBLE_S32_DEFINED
inline void ScnObject::SetVisible(s32 on)
{
    if (on)
        flags &= (u16)~SCN_OF_HIDDEN;
    else
        flags |= SCN_OF_HIDDEN;
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_SOUNDISPLAYING_U16) && !defined(SDW_INLINE_SCNOBJECT_SOUNDISPLAYING_U16_DEFINED)
#define SDW_INLINE_SCNOBJECT_SOUNDISPLAYING_U16_DEFINED
inline s32 ScnObject::SoundIsPlaying(u16 handle)
{
    return Sound_IsPlaying(handle);
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_STARTCAMERA_U16_U16_U16_VEC3S_U16) && \
    !defined(SDW_INLINE_SCNOBJECT_STARTCAMERA_U16_U16_U16_VEC3S_U16_DEFINED)
#define SDW_INLINE_SCNOBJECT_STARTCAMERA_U16_U16_U16_VEC3S_U16_DEFINED
inline void ScnObject::StartCamera(u16 rx, u16 ry, u16 rz, Vec3s *eye, u16 focal)
{
    Camera_StartScripted(this, &g_camera, rx, ry, rz, eye, focal, 0, 0x1000);
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_STARTCAMERA_U16_U16_U16_VEC3S_U16_U32) && \
    !defined(SDW_INLINE_SCNOBJECT_STARTCAMERA_U16_U16_U16_VEC3S_U16_U32_DEFINED)
#define SDW_INLINE_SCNOBJECT_STARTCAMERA_U16_U16_U16_VEC3S_U16_U32_DEFINED
inline void ScnObject::StartCamera(u16 rx, u16 ry, u16 rz, Vec3s *eye, u16 focal, u32 mode)
{
    Camera_StartScripted(this, &g_camera, rx, ry, rz, eye, focal, mode, 0x1000);
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_STARTCAMERA_U16_U16_U16_VEC3S_U16_U32_S32) && \
    !defined(SDW_INLINE_SCNOBJECT_STARTCAMERA_U16_U16_U16_VEC3S_U16_U32_S32_DEFINED)
#define SDW_INLINE_SCNOBJECT_STARTCAMERA_U16_U16_U16_VEC3S_U16_U32_S32_DEFINED
inline void ScnObject::StartCamera(u16 x, u16 y, u16 z, Vec3s *point, u16 focal, u32 mode, s32 time)
{
    Camera_StartScripted(this, &g_camera, x, y, z, point, focal, mode, time);
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_STARTCAMERABLENDED_U16_U16_U16_VEC3S_U16) && \
    !defined(SDW_INLINE_SCNOBJECT_STARTCAMERABLENDED_U16_U16_U16_VEC3S_U16_DEFINED)
#define SDW_INLINE_SCNOBJECT_STARTCAMERABLENDED_U16_U16_U16_VEC3S_U16_DEFINED
inline void ScnObject::StartCameraBlended(u16 rotX, u16 rotY, u16 rotZ, Vec3s *eye, u16 focal)
{
    Camera_StartScripted(this, &g_camera, rotX, rotY, rotZ, eye, focal, CAMSCR_BLEND_IN | CAMSCR_BLEND_OUT, 0x1000);
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_STOPSOUND_U16) && !defined(SDW_INLINE_SCNOBJECT_STOPSOUND_U16_DEFINED)
#define SDW_INLINE_SCNOBJECT_STOPSOUND_U16_DEFINED
/* BYTES(inline): the handle is a two-byte expansion temporary
 * (InstantMartian, 0x456c93; Ghost's sound-stop sites). */
inline void ScnObject::StopSound(u16 handle)
{
    Sound_Stop(handle, this);
}
#endif

#if defined(SDW_INLINE_SCNOBJECT_STOPSOUNDHANDLE_U16) && !defined(SDW_INLINE_SCNOBJECT_STOPSOUNDHANDLE_U16_DEFINED)
#define SDW_INLINE_SCNOBJECT_STOPSOUNDHANDLE_U16_DEFINED
inline void ScnObject::StopSoundHandle(u16 handle)
{
    Sound_Stop(handle, this);
}
#endif
