/* PAL PC Rook (class 178, vtable 0x5769ec, sizeof 0xd8): SheepD3D.exe 0x4e7320 (PostLoadInit) to the factory
 * Rook_Create 0x4e7c47. */
/* BYTES: slot-group. */
#define SDW_MEMBERS_ScnObject            \
    static void *operator new(u32 size); \
    void SetRotation(Vec3s *value);
#include "sdw_enums.h"
#include "scenaric_props.h"
#include "sdw_classes.h"
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_SETROTATION_VEC3S 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_SETROTATION_VEC3S

#include "../engine/scn_tools.h"
#include "../engine/approach.h"
s32 ObjGrid_QueryBoxOverlap(CollBox *box, ScnObject **out);
extern "C" s16 Math_RadiansToAngle4096(float radians);
#include "../sdk/crt.h"
extern Wolf *g_pWolf;
extern s32 g_dtMs;

#define SDW_ABS(value) ((value) >= 0 ? (value) : -(value))

inline s32 Box_ContainsPointXZ(CollBox *box, Vec3s *point)
{
    return point->x >= box->min.x && point->x <= box->max.x && point->z >= box->min.z && point->z <= box->max.z;
}

/* 0x4e7320 */
void Rook::PostLoadInit()
{
    u16 *props = record;
    Trajectory *trajectory;
    cannon = Scn_GetPropObject(props, 4);
    cannonBall = Scn_GetPropObject(props, 8);
    detectBox = (CollBox *)Scn_GetPropBox(props, 0); /* cast kept: Box and CollBox are two views of one zone record */
    trajectory = Scn_GetPropTrajectory(props, 0x14);
    TrajFollower_Init(&traj, trajectory, 200, 0x800, 0, 0, 50);
    bodyBox = GetFirstSolidBox();
    cannonRestRot = cannon->rot;
    SetState(ROOK_ST_PATROL);
}

/* 0x4e73ee */
void Rook::Reset()
{
    cannon->SetRotation(&cannonRestRot);
    SetState(ROOK_ST_PATROL);
}

/* 0x4e742c */
/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets; pad0, pad1, pad2, pad3 fill gaps */
void Rook::Update()
{
    struct {
        Vec3s error;
        u16 pad0;
        Vec3s delta;
        u16 pad1;
        CollBox world;
        ScnObject *found[64];
        s32 count;
        Vec3s target;
        u8 pad2[3];
        u8 index;
        s16 heading;
        Vec3s velocity;
        u16 pad3;
    } w;
    switch (state) {
        case ROOK_ST_PATROL:
            TrajFollower_Step(&traj, &w.velocity, &w.heading);
            Vec3s_ScaleByDt(&w.velocity, &w.delta);
            Translate(&w.delta);
            detectBox->Box_Translate(detectBox, &w.delta);
            w.target.x = pos.x;
            w.target.y = pos.y;
            w.target.z = pos.z;
            w.target.y -= 60;
            cannon->SetPosition(&w.target);
            cannonBall->HandleMessage(this, MSG_CB_PLACE, &w.target);
            w.world.Box_Translate(bodyBox, &pos);
            w.world.Box_Translate(&w.world, &w.delta);
            w.count = ObjGrid_QueryBoxOverlap(&w.world, w.found);
            for (w.index = 0; w.index < w.count; w.index++) {
                if (w.found[w.index]->GetClassId() != CLASSID_ROOK)
                    w.found[w.index]->Translate(&w.delta);
            }
            if (g_pWolf->HandleMessage(this, MSG_WOLF_IS_VISIBLE, 0)) {
                if (Box_ContainsPointXZ(detectBox, &g_pWolf->pos) && !g_pWolf->HandleMessage(this, MSG_WOLF_IS_DEAD, 0))
                    SetState(ROOK_ST_AIM);
            }
            break;
        case ROOK_ST_AIM:
            AimCannonAt(&w.error, &g_pWolf->pos);
            if (SDW_ABS((s16)(w.error.x & 0xfff)) + SDW_ABS((s16)(w.error.y & 0xfff)) +
                        SDW_ABS((s16)(w.error.z & 0xfff)) <
                    12 &&
                lastTargetPos.x == g_pWolf->pos.x && lastTargetPos.y == g_pWolf->pos.y &&
                lastTargetPos.z == g_pWolf->pos.z)
                SetState(ROOK_ST_RECOIL);
            lastTargetPos = g_pWolf->pos;
            break;
        case ROOK_ST_RECOIL:
            fireDelayMs -= g_dtMs;
            if (fireDelayMs < 0)
                SetState(ROOK_ST_FIRE);
            break;
    }
    AdvanceAnim();
}

/* 0x4e78c6 */
s32 Rook::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    switch (msgId) {
        case MSG_BALL_RETURNED:
            SetState(ROOK_ST_PATROL);
            break;
    }
    return 0;
}

/* 0x4e78ef */
void Rook::SetState(u8 newState)
{
    Vec3s target;
    state = newState;
    switch (newState) {
        case ROOK_ST_AIM:
            aimVelRoll = 0;
            aimVelYaw = 0;
            aimVelPitch = 0;
            lastTargetPos = g_pWolf->pos;
            g_pWolf->HandleMessage(this, MSG_SCARE, 0);
            break;
        case ROOK_ST_RECOIL:
            fireDelayMs = 350;
            cannon->HandleMessage(this, MSG_CD_RECOIL, 0);
            cannonBall->HandleMessage(this, MSG_CB_SET_OWNS_CAMERA, 0);
            break;
        case ROOK_ST_FIRE:
            target = g_pWolf->pos;
            target.y -= 120;
            cannonBall->HandleMessage(this, MSG_CB_FIRE_AT, &target);
            break;
    }
}

/* 0x4e7a25 */
/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets; pad0, pad1 fill gaps */
Vec3s *Rook::AimCannonAt(Vec3s *outError, const Vec3s *target)
{
    struct {
        Vec3s angle;
        u16 pad0;
        Vec3s desired;
        u16 pad1;
        s32 xx, yy, zz;
    } w;
    aimDeltaX = (s16)(target->x - pos.x);
    aimDeltaY = (s16)(target->y - pos.y);
    aimDeltaZ = (s16)(target->z - pos.z);
    w.xx = aimDeltaX * aimDeltaX;
    w.yy = aimDeltaY * aimDeltaY;
    w.zz = aimDeltaZ * aimDeltaZ;
    w.desired.x = 0;
    w.desired.y = (Math_RadiansToAngle4096((float)atan2(aimDeltaX, aimDeltaZ)) - 0xc00) & 0xfff;
    w.desired.z = -Math_RadiansToAngle4096((float)atan2(aimDeltaY, (s32)sqrt((double)w.xx + w.zz))) & 0xfff;
    w.angle.x = 0;
    w.angle.y = Math_ApproachAngle(cannon->rot.y, w.desired.y, &aimVelYaw, 8000, 1000, 1000, 1);
    w.angle.z = Math_ApproachAngle(cannon->rot.z, w.desired.z, &aimVelPitch, 8000, 1000, 1000, 1);
    cannon->SetRotation(&w.angle);
    w.desired.x -= w.angle.x;
    w.desired.y -= w.angle.y;
    w.desired.z -= w.angle.z;
    *outError = w.desired;
    return outError;
}

/* 0x4e7c47 */
ScnObject *Rook_Create(void *record)
{
    Rook *obj = new Rook;
    obj = (Rook *)obj->Init(record, 0); /* cast kept: Init returns the base class */
    return obj;
}
