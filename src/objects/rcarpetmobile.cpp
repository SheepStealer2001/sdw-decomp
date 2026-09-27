/* PAL PC RCarpetMobile (class 118), 0x4dfe80-0x4e1234: the carpet that a RollingCarpet or a SensibleButton drives. */
/* BYTES: slot-group, slot-name. */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
#include "../engine/sound_mgr.h"
#include "../engine/scn_tools.h"
#include "../engine/scenaric.h"
#define SDW_MEMBERS_ScnObject                      \
    static void *operator new(u32 size);           \
    CollBox *GetFirstModelBox();                   \
    void NotifyRider(ScnObject *rider, void *arg); \
    void SetUpdateMode(u8 mode);
#include "sdw_classes.h"
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_STOPSOUND_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_STOPSOUND_U16
#define SDW_INLINE_SCNOBJECT_SETDRAWMODE_U32 1
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETDRAWMODE_U32
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8
s32 Box_GroundQueryFlatTop(::GroundQuery *query, CollBox *box, Vec3s *position, s32 margin);
s32 ObjGrid_QueryBoxOverlap(CollBox *box, ScnObject **out);
s32 Vec3s_Dist(Vec3s *a, Vec3s *b);
u16 Sound_Play(u16 id, void *owner, u16 volume, u8 flags, s32 rate);
#include "../sdk/crt.h"
extern "C" s16 Math_RadiansToAngle4096(float radians);

#define SDW_INLINE_FREE_SCN_GETPROPS32_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_SCN_GETPROPS32_VOID_U32
inline CollBox *ScnObject::GetFirstModelBox()
{
    ModelBoxList *list = inst_model->boxes;
    if (list)
        return list->boxes;
    return 0;
}
#define SDW_INLINE_FREE_SCENARIC_CLASSFLAGS_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_FREE_SCENARIC_CLASSFLAGS_U16
inline void ScnObject::NotifyRider(ScnObject *rider, void *arg)
{
    if (Scenaric_ClassFlags(rider->GetClassId()) & SCN_CF_SHEEP_ANCHORABLE)
        rider->HandleMessage(this, MSG_SET_ANCHOR, arg);
}

/* 0x4dfe80 */
void RCarpetMobile::PostLoadInit()
{
    u16 *props = record;
    senderClass = CLASSID_ROLLINGCARPET;
    motorSound = 0;
    speedRatio = Scn_GetPropS32(props, 0);
    traj = Scn_GetPropTrajectory(props, 4);
    TrajFollower_Init(&follower, traj, 0, 0, 0, 1, 30);
    SetPosition(follower.traj->pts);
    UpdateRideBox();
    homePos = pos;
    carpetFlags.collide = 1;
    carpetFlags.driven = 0;
    SetDrawMode(0);
    SetUpdateMode(SCN_UPD_ALWAYS);
}

/* 0x4e002b */
void RCarpetMobile::Reset()
{
    carpetFlags.driven = 0;
    SetPosition(&homePos);
    follower.pointIndex = 0;
    StopSound(motorSound);
    motorSound = 0;
}

/* 0x4e0095 */
void RCarpetMobile::Update()
{
    ScnObject *object;
    u16 next;
    struct {
        Vec3s position;
        u16 radius;
    } msg;
    ScnObject *found[64];
    s32 range;
    s32 count;
    count = ObjGrid_QueryBoxOverlap(&rideBox, found);
    if (count > 0) {
        msg.radius = 50;
        msg.position.x = pos.x;
        msg.position.y = pos.y;
        msg.position.z = pos.z;
        for (next = 0; next < count; next++) {
            object = found[next];
            if (Scenaric_ClassFlags(object->GetClassId()) & SCN_CF_SHEEP_ANCHORABLE)
                object->HandleMessage(this, MSG_SET_ANCHOR, &msg);
        }
    }
    if (mode != RCM_MODE_RETURN && !carpetFlags.driven)
        StopSound(motorSound);
    if (mode != RCM_MODE_RETURN || carpetFlags.driven) {
        carpetFlags.driven = 0;
    } else {
        range = Vec3s_Dist(&homePos, &pos);
        if (range > 30) {
            if (!carpetFlags.reversed) {
                carpetFlags.reversed = 1;
                follower.pointIndex--;
                if (follower.pointIndex < 0) {
                    if (!carpetFlags.clampEnds)
                        follower.pointIndex = follower.traj->count - 1;
                    else
                        follower.pointIndex = 0;
                }
            }
            Move(1, 1, 1);
        } else {
            StopSound(motorSound);
            SetUpdateMode(SCN_UPD_NEVER);
        }
    }
}

/* 0x4e0435 */
s32 RCarpetMobile::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    if (sender) {
        senderClass = sender->GetClassId();
        switch (msgId) {
            case MSG_CARPET_SET_MODE:
                if (senderClass == CLASSID_ROLLINGCARPET) {
                    /* cast kept: MSG_CARPET_SET_MODE's arg is the RCarpetMode, a number in the void * */
                    mode = (s32)arg;
                    switch (mode) {
                        case RCM_MODE_LOOP:
                            carpetFlags.clampEnds = 0;
                            break;
                        case RCM_MODE_ONEWAY:
                            carpetFlags.clampEnds = 1;
                            break;
                        case RCM_MODE_RETURN:
                            carpetFlags.clampEnds = 1;
                            break;
                    }
                }
                break;
            case MSG_SWITCH_ON:
            case MSG_SWITCH_OFF:
                carpetFlags.clampEnds = 1;
                carpetFlags.collide = 1;
                mode = RCM_MODE_RETURN;
                /* Original fallthrough into the ordinary drive message. */
            case MSG_CARPET_DRIVE:
                if (senderClass == CLASSID_ROLLINGCARPET || senderClass == CLASSID_SENSIBLEBUTTON) {
                    carpetFlags.driven = 1;
                    if (mode == RCM_MODE_RETURN)
                        SetUpdateMode(SCN_UPD_ALWAYS);
                    if (msgId == MSG_CARPET_DRIVE)
                        speed = *(s32 *)arg; /* cast kept: MSG_CARPET_DRIVE's arg points at the s32 speed */
                    else if (msgId == MSG_SWITCH_ON)
                        speed = 400;
                    else
                        speed = -400;
                    if (speed >= 0) {
                        if (carpetFlags.reversed) {
                            follower.pointIndex++;
                            if (follower.pointIndex >= follower.traj->count) {
                                if (!carpetFlags.clampEnds)
                                    follower.pointIndex = 0;
                                else
                                    follower.pointIndex = follower.traj->count - 1;
                            }
                        }
                        carpetFlags.reversed = 0;
                    } else {
                        if (!carpetFlags.reversed) {
                            follower.pointIndex--;
                            if (follower.pointIndex < 0) {
                                if (!carpetFlags.clampEnds)
                                    follower.pointIndex = follower.traj->count - 1;
                                else
                                    follower.pointIndex = 0;
                            }
                        }
                        carpetFlags.reversed = 1;
                    }
                    if (Move(carpetFlags.reversed, carpetFlags.clampEnds, 0))
                        return 0;
                    return 1;
                }
                break;
            case MSG_GROUND_QUERY:
                return GroundQuery((::GroundQuery *)arg); /* cast kept: MSG_GROUND_QUERY's arg is a GroundQuery */
        }
    }
    return 0;
}

/* 0x4e0848 */
s32 RCarpetMobile::GroundQuery(::GroundQuery *arg)
{
    ::GroundQuery *query = arg;
    return Box_GroundQueryFlatTop(query, GetFirstModelBox(), &pos, 0);
}

/* 0x4e089a */
void RCarpetMobile::UpdateRideBox()
{
    Vec3s offset = {0, -30, 0};
    rideBox.min.x = GetFirstModelBox()->min.x + pos.x;
    rideBox.min.y = GetFirstModelBox()->min.y + pos.y;
    rideBox.min.z = GetFirstModelBox()->min.z + pos.z;
    rideBox.max.x = GetFirstModelBox()->max.x + pos.x;
    rideBox.max.y = GetFirstModelBox()->max.y + pos.y;
    rideBox.max.z = GetFirstModelBox()->max.z + pos.z;
    rideBox.min.x += offset.x;
    rideBox.min.y += offset.y;
    rideBox.min.z += offset.z;
}

/* 0x4e0a7b */
/* BYTES(slot-name, inferred): local names chosen for their stack slots (tools/vc6_locals.py), not recovered */
s32 RCarpetMobile::Move(s32 reverse, s32 clampEnds, s32 returnHome)
{
    ScnObject *target;
    u16 notifyIndex;
    struct {
        Vec3s position;
        u16 radius;
    } tempNotify;
    u16 local_carryIndex;
    s16 heading_value;
    ScnObject *found[64];
    Vec3s velocity;
    s32 endReached_temp;
    Vec3s localDelta;
    ScnObject *local_object;
    s32 count;
    count = ObjGrid_QueryBoxOverlap(&rideBox, found);
    carpetFlags.snap = 0;
    endReached_temp = TrajStep(&follower, &velocity, &heading_value, reverse, clampEnds, returnHome);
    if (carpetFlags.snap) {
        localDelta.x = snapPoint.x - pos.x;
        localDelta.y = snapPoint.y - pos.y;
        localDelta.z = snapPoint.z - pos.z;
    } else {
        Vec3s_ScaleByDt(&velocity, &localDelta);
    }
    if (carpetFlags.collide)
        Collide_ResolveMove(&localDelta, &contact, 0xb54, RESOLVE_SLIDE_ALL | RESOLVE_NO_STATIC, 0, 0, 10, found,
                            count);
    if (!localDelta.x && !localDelta.y && !localDelta.z) {
        StopSound(motorSound);
        return endReached_temp;
    }
    if (!Sound_IsPlaying(motorSound))
        motorSound = Sound_Play(SND_SPFASCEN, this, 0xff, SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 0x1000);
    for (local_carryIndex = 0; local_carryIndex < count; local_carryIndex++) {
        local_object = found[local_carryIndex];
        if (local_object != this)
            local_object->Translate(&localDelta);
    }
    Translate(&localDelta);
    UpdateRideBox();
    if (count > 0) {
        tempNotify.radius = 50;
        tempNotify.position.x = pos.x;
        tempNotify.position.y = pos.y;
        tempNotify.position.z = pos.z;
        for (notifyIndex = 0; notifyIndex < count; notifyIndex++) {
            target = found[notifyIndex];
            if (Scenaric_ClassFlags(target->GetClassId()) & SCN_CF_SHEEP_ANCHORABLE)
                target->HandleMessage(this, MSG_SET_ANCHOR, &tempNotify);
        }
    }
    return endReached_temp;
}

/* 0x4e0da1 */
/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets */
s32 RCarpetMobile::TrajStep(TrajFollower *f, Vec3s *outVelocity, s16 *outHeading, s32 reverse, s32 clampEnds,
                            s32 returnHome)
{
    struct {
        Vec3s *point;
        s32 dz, dy, dx, distance, endReached;
    } w;
    w.endReached = 0;
    f->advanced = 0;
    f->moving = 1;
    do {
        w.point = &f->traj->pts[f->pointIndex];
        w.dx = w.point->x - pos.x;
        w.dy = w.point->y - pos.y;
        w.dz = w.point->z - pos.z;
        w.distance = w.dx * w.dx + w.dz * w.dz + w.dy * w.dy;
        carpetFlags.arrived = w.distance < f->arriveRadiusSq;
        if (carpetFlags.arrived) {
            f->advanced = 1;
            carpetFlags.snap = 1;
            snapPoint.x = traj->pts[f->pointIndex].x;
            snapPoint.y = traj->pts[f->pointIndex].y;
            snapPoint.z = traj->pts[f->pointIndex].z;
            if (!reverse) {
                f->pointIndex++;
                if (f->pointIndex >= f->traj->count) {
                    w.endReached = 1;
                    if (!clampEnds)
                        f->pointIndex = 0;
                    else
                        f->pointIndex = f->traj->count - 1;
                }
            } else {
                f->pointIndex--;
                if (f->pointIndex < 0) {
                    w.endReached = 1;
                    if (!clampEnds)
                        f->pointIndex = f->traj->count - 1;
                    else
                        f->pointIndex = 0;
                }
            }
        }
    } while (carpetFlags.arrived && (!w.endReached || !clampEnds));
    if (!carpetFlags.snap) {
        if (w.endReached && clampEnds) {
            outVelocity->z = 0;
            outVelocity->y = 0;
            outVelocity->x = 0;
        } else {
            if (returnHome)
                speed = 400;
            else if (speed > 1200)
                speed = 0;
            speed = speed * speedRatio / 100;
            w.distance = (s32)sqrt((double)w.distance);
            outVelocity->x = w.dx * (s16)(speed >= 0 ? speed : -speed) / (s16)w.distance;
            outVelocity->y = w.dy * (s16)(speed >= 0 ? speed : -speed) / (s16)w.distance;
            outVelocity->z = w.dz * (s16)(speed >= 0 ? speed : -speed) / (s16)w.distance;
        }
    }
    if (f->advanced || f->continuousHeading) {
        f->heading = (f->headingBias + Math_RadiansToAngle4096((float)atan2(w.dx, w.dz))) & 0xfff;
        *outHeading = f->heading;
    } else {
        *outHeading = f->heading;
    }
    return w.endReached;
}

/* 0x4e11ad */
ScnObject *RCarpetMobile_Create(void *record)
{
    RCarpetMobile *obj = new RCarpetMobile;
    obj = (RCarpetMobile *)obj->Init(record); /* cast kept: Init returns the object as a ScnObject * */
    obj->mode = RCM_MODE_LOOP;
    obj->carpetFlags.clampEnds = 0;
    return obj;
}
