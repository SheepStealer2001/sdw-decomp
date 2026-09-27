/* PAL PC CrocodileLevel09. Angle arithmetic is narrow, as in the original. */
/* BYTES: slot-group, temp. */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *, Animator *, u16, u32);
#define SDW_INLINE_FREE_INSTFLAGSSET_U16_U16 1
#include "instance_inlines.h"
#undef SDW_INLINE_FREE_INSTFLAGSSET_U16_U16
#define SDW_INLINE_FREE_INSTFLAGSCLEAR_U16_U16 1
#include "instance_inlines.h"
#undef SDW_INLINE_FREE_INSTFLAGSCLEAR_U16_U16

#define SDW_MEMBERS_ScnObject             \
    static void *operator new(u32);       \
    void SetFacing(s16 value);            \
    void EnableCollide(s32 on)            \
    {                                     \
        if (on)                           \
            flags &= (u16)~SCN_OF_HIDDEN; \
        else                              \
            flags |= SCN_OF_HIDDEN;       \
    }


#define SDW_MEMBERS_CrocodileLevel09 \
    void SetWaypoint(s16 value)      \
    {                                \
        follower.pointIndex = value; \
    }                                \
    s16 GetWaypoint()                \
    {                                \
        return follower.pointIndex;  \
    }
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#define SDW_INLINE_SCNOBJECT_FACING 1
#define SDW_INLINE_SCNOBJECT_SETFACING_S16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_FACING
#undef SDW_INLINE_SCNOBJECT_SETFACING_S16
#define SDW_INLINE_SCNOBJECT_ENABLETINT_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_ENABLETINT_S32
#define SDW_INLINE_SCNBODY_GETANIMID 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_GETANIMID
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
extern Wolf *g_pWolf;
#include "../engine/scn_tools.h"
s32 Vec3s_DistSqXZ(Vec3s *, Vec3s *), Vec3s_DistSq(Vec3s *, Vec3s *);
s32 ObjGrid_QueryBoxPoints(const CollBox *, ScnObject **);
#include "../sdk/crt.h"
extern "C" s16 Math_RadiansToAngle4096(float);
/* BYTES(temp, inferred): the offset and the result go through the read struct because the original's expansion keeps both in stack slots */
#define SDW_INLINE_FREE_PROPU32_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_PROPU32_VOID_U32
#define SDW_INLINE_SCNOBJECT_FIRSTBOX 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_FIRSTBOX
#define SDW_INLINE_FREE_INBOX_BOX_VEC3S 1
#include "../engine/coll_box_inlines.h"
#undef SDW_INLINE_FREE_INBOX_BOX_VEC3S
#define ANGLE_DIFF(a, b) ((s16)((s16)(((a) - (b) + 0x800) & 0xfff) - 0x800))
#define ABS_VALUE(a) ((a) >= 0 ? (a) : -(a))
#define CALC_WOLF_ANGLE()                  \
    scratchVec.x = g_pWolf->pos.x - pos.x; \
    scratchVec.y = g_pWolf->pos.y - pos.y; \
    scratchVec.z = g_pWolf->pos.z - pos.z; \
    angleToWolf = Math_RadiansToAngle4096((float)atan2(scratchVec.x, scratchVec.z)) & 0xfff;

void CrocodileLevel09::PostLoadInit()
{
    void *properties = record;
    activationBox = Scn_GetPropBox(properties, 0);
    traj = Scn_GetPropTrajectory(properties, 4);
    if (PropU32(properties, 8)) {
        TrajFollower_Init(&follower, traj, 250, 0x800, 1, 1, 50);
        moveAnim = ACROCO01_ANIM_WALK;
    } else {
        TrajFollower_Init(&follower, traj, 500, 0x800, 1, 1, 50);
        moveAnim = ACROCO01_ANIM_RUN;
    }
    SetPosition(&traj->pts[0]);
    SnapToGround(1);
    homePos = pos;
    scratchVec.x = traj->pts[1].x - traj->pts[0].x;
    scratchVec.y = traj->pts[1].y - traj->pts[0].y;
    scratchVec.z = traj->pts[1].z - traj->pts[0].z;
    startFacing = (s16)(Math_RadiansToAngle4096((float)atan2(scratchVec.x, scratchVec.z)) & 0xfff) + 0x800;
    SetFacing(startFacing);
    pointCount = traj->count;
    otherWaypoint = 0;
    waypoint = 0;
    SetState(CROC_ST_WAIT_START);
}

void CrocodileLevel09::Reset()
{
    if (state != CROC_ST_DEAD) {
        SetPosition(&homePos);
        SetFacing(startFacing);
        otherWaypoint = 0;
        waypoint = 0;
        SetState(CROC_ST_WAIT_START);
    }
    g_pWolf->EnableCollide(1);
}

/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets; unused, unused2 fill gaps */
void CrocodileLevel09::Update()
{
    struct Work {
        s32 index, count;
        ScnObject *objects[64];
        u32 unused;
        Vec3s velocity;
        u16 unused2[2];
        s16 heading;
    } w;
    if (!g_pWolf->HandleMessage(this, MSG_WOLF_IS_DEAD, 0) && state != CROC_ST_DEAD &&
        Vec3s_DistSqXZ(&pos, &g_pWolf->pos) <= 40000 && Vec3s_DistSq(&pos, &g_pWolf->pos) <= 160000)
        SetState(CROC_ST_BITE);
    switch (state) {
        case CROC_ST_WAIT_START:
            if (!g_pWolf->HandleMessage(this, MSG_WOLF_IS_DEAD, 0) && InBox(activationBox, &g_pWolf->pos)) {
                CALC_WOLF_ANGLE();
                if (ABS_VALUE(ANGLE_DIFF(angleToWolf, Facing() + 0x800)) < 700)
                    SetState(CROC_ST_FORWARD);
            }
            break;
        case CROC_ST_FORWARD:
            if (!InBox(activationBox, &g_pWolf->pos)) {
                SetState(CROC_ST_BACKWARD);
                break;
            }
            TrajFollower_Step(&follower, &w.velocity, &w.heading);
            if (GetWaypoint() != waypoint) {
                otherWaypoint = waypoint;
                SetState(CROC_ST_FORWARD);
            } else {
                Vec3s_ScaleByDt(&w.velocity, &moveDelta);
                CALC_WOLF_ANGLE();
                if (ABS_VALUE(ANGLE_DIFF(angleToWolf, w.heading)) < 700) {
                    SetState(CROC_ST_BACKWARD);
                    break;
                }
                if (ABS_VALUE(ANGLE_DIFF(angleToWolf, w.heading + 0x800)) < 700) {
                    if (GetAnimId() != moveAnim)
                        PlayAnim(moveAnim, 1, 1);
                    else {
                        MoveAndTilt(&moveDelta, w.heading);
                        Translate(&moveDelta);
                    }
                } else if (GetAnimId() != ACROCO01_ANIM_STAND2) {
                    if (GetAnimId() == moveAnim) {
                        if (AnimFlags(ANIM_F_FINISHED))
                            PlayAnim(ACROCO01_ANIM_STAND2, 1, 1);
                        else {
                            MoveAndTilt(&moveDelta, w.heading);
                            Translate(&moveDelta);
                        }
                    } else
                        PlayAnim(ACROCO01_ANIM_STAND2, 1, 1);
                }
            }
            break;
        case CROC_ST_BACKWARD:
            TrajFollower_Step(&follower, &w.velocity, &w.heading);
            if (GetWaypoint() != waypoint) {
                otherWaypoint = waypoint;
                SetState(CROC_ST_BACKWARD);
            } else {
                Vec3s_ScaleByDt(&w.velocity, &moveDelta);
                CALC_WOLF_ANGLE();
                if (InBox(activationBox, &g_pWolf->pos) && ABS_VALUE(ANGLE_DIFF(angleToWolf, w.heading)) < 700) {
                    SetState(CROC_ST_FORWARD);
                    break;
                }
                if (ABS_VALUE(ANGLE_DIFF(angleToWolf, w.heading + 0x800)) < 700 ||
                    !InBox(activationBox, &g_pWolf->pos)) {
                    if (GetAnimId() != moveAnim)
                        PlayAnim(moveAnim, 1, 1);
                    else {
                        MoveAndTilt(&moveDelta, w.heading);
                        Translate(&moveDelta);
                    }
                } else if (GetAnimId() != ACROCO01_ANIM_STAND2) {
                    if (GetAnimId() == moveAnim) {
                        if (AnimFlags(ANIM_F_FINISHED))
                            PlayAnim(ACROCO01_ANIM_STAND2, 1, 1);
                        else {
                            MoveAndTilt(&moveDelta, w.heading);
                            Translate(&moveDelta);
                        }
                    } else
                        PlayAnim(ACROCO01_ANIM_STAND2, 1, 1);
                }
            }
            break;
        case CROC_ST_BITE:
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(CROC_ST_WAIT_START);
            break;
        case CROC_ST_TURN:
            if (AnimFlags(ANIM_F_FINISHED)) {
                SetState(stateAfterTurn);
                rot.y = Facing() + 0x800;
            }
            break;
        case CROC_ST_WAIT_END:
            if (!InBox(activationBox, &g_pWolf->pos))
                SetState(CROC_ST_BACKWARD);
            else {
                CALC_WOLF_ANGLE();
                if (ABS_VALUE(ANGLE_DIFF(angleToWolf, Facing())) < 700) {
                    SetState(CROC_ST_BACKWARD);
                    break;
                }
            }
            break;
        case CROC_ST_SPIN_TO_START:
            SetFacing(Facing() + spinSpeed);
            spinSpeed -= (s16)(spinSpeed / 4);
            if (ANGLE_DIFF(startFacing, Facing()) < 185)
                SetState(CROC_ST_WAIT_START);
            break;
        case CROC_ST_DEAD:
            if (!bouncer) {
                w.count = ObjGrid_QueryBoxPoints(&deathBox, w.objects);
                if (w.count > deathBoxCount) {
                    for (w.index = 0; w.index < w.count; w.index++) {
                        if (w.objects[w.index]->GetClassId() == CLASSID_WOLF ||
                            w.objects[w.index]->GetClassId() == CLASSID_FLOATINGBOX) {
                            bouncer = w.objects[w.index];
                            bounceEntryVert = bouncer->pos.y;
                        }
                    }
                }
                deathBoxCount = w.count;
            } else {
                if (bouncer->pos.y > bounceEntryVert) {
                    PlayAnim(ACROCO01_ANIM_SQUASH, 0, 1);
                    bouncer->HandleMessage(this, MSG_BOUNCE, 0);
                }
                bouncer = 0;
                bounceEntryVert = -32768;
            }
            break;
    }
    AdvanceAnim();
}

s32 CrocodileLevel09::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    switch (msgId) {
        case MSG_KILL:
            if (sender->GetClassId() == CLASSID_GROUNDMINE || sender->GetClassId() == CLASSID_DEFUSABLEMINE) {
                tintColor = 0;
                tintAmount = 4096;
                EnableTint(1);
                if (state == CROC_ST_DEAD)
                    PlayAnim(ACROCO01_ANIM_SQUASH, 0, 1);
            } else if ((sender->GetClassId() == CLASSID_CANNONBALL || sender->GetClassId() == CLASSID_ROCK ||
                        sender->GetClassId() == CLASSID_SMALLROCK) &&
                       state != CROC_ST_DEAD) {
                if (state == CROC_ST_BITE && g_pWolf->HandleMessage(this, MSG_WOLF_IS_DEAD, 0))
                    g_pWolf->EnableCollide(0);
                SetState(CROC_ST_DEAD);
                return 1;
            }
            break;
    }
    return 0;
}

void CrocodileLevel09::SetState(u8 next)
{
    prevState = state;
    state = next;
    switch (state) {
        case CROC_ST_WAIT_START:
            PlayAnim(ACROCO01_ANIM_STAND0, 1, 1);
            break;
        case CROC_ST_DEAD:
            PlayAnim(ACROCO01_ANIM_DEAD, 0, 0);
            deathBox = *FirstBox();
            deathBox.min.x *= 150;
            deathBox.min.y *= 200;
            deathBox.min.z *= 150;
            deathBox.max.x *= 150;
            deathBox.max.y *= 200;
            deathBox.max.z *= 150;
            deathBox.min.x /= 80;
            deathBox.min.y /= 80;
            deathBox.min.z /= 80;
            deathBox.max.x /= 80;
            deathBox.max.y /= 80;
            deathBox.max.z /= 80;
            deathBox.min.x += pos.x;
            deathBox.min.y += pos.y;
            deathBox.min.z += pos.z;
            deathBox.max.x += pos.x;
            deathBox.max.y += pos.y;
            deathBox.max.z += pos.z;
            bouncer = 0;
            bounceEntryVert = -32768;
            break;
        case CROC_ST_WAIT_END:
            PlayAnim(ACROCO01_ANIM_STAND0, 1, 0);
            break;
        case CROC_ST_FORWARD:
            PlayAnim(moveAnim, 1, 0);
            if (prevState == CROC_ST_FORWARD)
                waypoint++;
            else if (prevState == CROC_ST_BACKWARD) {
                swapTmp = waypoint;
                waypoint = otherWaypoint;
                otherWaypoint = swapTmp;
                stateAfterTurn = CROC_ST_FORWARD;
                SetState(CROC_ST_TURN);
            } else if (prevState != CROC_ST_TURN) {
                swapTmp = waypoint;
                waypoint = otherWaypoint;
                otherWaypoint = swapTmp;
            }
            if (waypoint < pointCount)
                SetWaypoint(waypoint);
            else {
                waypoint = pointCount - 1;
                otherWaypoint = pointCount - 1;
                SetState(CROC_ST_WAIT_END);
            }
            break;
        case CROC_ST_BACKWARD:
            PlayAnim(moveAnim, 1, 0);
            if (prevState == CROC_ST_BACKWARD)
                waypoint--;
            else if (prevState == CROC_ST_FORWARD) {
                swapTmp = waypoint;
                waypoint = otherWaypoint;
                otherWaypoint = swapTmp;
                stateAfterTurn = CROC_ST_BACKWARD;
                SetState(CROC_ST_TURN);
            } else if (prevState != CROC_ST_TURN) {
                swapTmp = waypoint;
                waypoint = otherWaypoint;
                otherWaypoint = swapTmp;
            }
            if (waypoint > -1)
                SetWaypoint(waypoint);
            else {
                waypoint = 0;
                otherWaypoint = 0;
                SetState(CROC_ST_SPIN_TO_START);
            }
            break;
        case CROC_ST_BITE:
            scratchVec.x = g_pWolf->pos.x - pos.x;
            scratchVec.y = g_pWolf->pos.y - pos.y;
            scratchVec.z = g_pWolf->pos.z - pos.z;
            SetFacing((s16)(Math_RadiansToAngle4096((float)atan2(scratchVec.x, scratchVec.z)) & 0xfff) + 0x800);
            g_pWolf->HandleMessage(this, MSG_KILL, (void *)KILL_CROCODILE); /* cast kept: arg carries a number */
            PlayAnim(ACROCO01_ANIM_EAT1, 0, 1);
            break;
        case CROC_ST_TURN:
            if (moveAnim == ACROCO01_ANIM_WALK)
                PlayAnim(ACROCO01_ANIM_BRAKE1, 0, 1);
            else
                PlayAnim(ACROCO01_ANIM_BRAKE, 0, 1);
            break;
        case CROC_ST_SPIN_TO_START:
            spinSpeed = 555;
            PlayAnim(ACROCO01_ANIM_TURN1, 1, 1);
            break;
    }
}

/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets; unused, unused2 fill gaps */
void CrocodileLevel09::MoveAndTilt(Vec3s *delta, s16 heading)
{
    struct Work {
        Vec3i square;
        u16 unused;
        s16 difference;
        Vec3s angles;
        u16 unused2;
        ContactInfo contact;
    } w;
    w.angles.x = 0;
    w.angles.y = heading;
    w.angles.z = 0;
    delta->y += 10;
    if (Collide_ResolveMove(delta, &w.contact, 0x578, COLL_FLOOR | COLL_FLOOR_EDGE, 0, 0, 10, 0, 0)) {
        w.square.x = w.contact.floorNormal.x * w.contact.floorNormal.x;
        w.square.y = w.contact.floorNormal.y * w.contact.floorNormal.y;
        w.square.z = w.contact.floorNormal.z * w.contact.floorNormal.z;
        w.angles.x = (Math_RadiansToAngle4096(
                          (float)atan2(w.contact.floorNormal.y, (s32)sqrt((double)w.square.x + (double)w.square.z))) +
                      0x400) &
                     0xfff;
        w.difference =
            (s16)((heading -
                   (s16)(Math_RadiansToAngle4096((float)atan2(w.contact.floorNormal.x, w.contact.floorNormal.z)) &
                         0xfff) +
                   0x800) &
                  0xfff) -
            0x800;
        if (w.difference < 0x400 || w.difference > 0xc00)
            w.angles.x = -w.angles.x;
        if (w.angles.x > 0x124)
            w.angles.x = 0x124;
        else if (w.angles.x < -0x124)
            w.angles.x = -0x124;
    }
    rot = w.angles;
}

ScnObject *CrocodileLevel09_Create(void *record)
{
    ScnBody *object = new CrocodileLevel09;
    object = object->Init(record, 0);
    return object;
}
