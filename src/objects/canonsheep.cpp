/* T122 - original object CanonSheep.cpp (guessed name).
 * Ranges: .text 0x4a5b20-0x4a713f, .rdata 0x575cbc-0x575ce0 (vtable), .data 0x57b410-0x57b420 (the cinematic header's
 * static opcode-stride table copy, then padding).
 * PAL PC CanonSheep 0x4a5b20-0x4a7140. */
/* BYTES: layout, slot-group, slot-scope. */
/* BYTES(layout): the Cine.h header static: every object including the cinematic header carries this copy in its .data, referenced or not */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *, Animator *, u16, u32);

#define SDW_MEMBERS_ScnObject                           \
    static void *operator new(u32 size);                \
    void SetRotation(Vec3s *rotation);                  \
    void StartCamera(u16, u16, u16, Vec3s *, u16, u32); \
    void SetUpdateMode(s32 mode);


#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_INSTANCEFLAGS_U16 1
#define SDW_INLINE_SCNOBJECT_SETROTATION_VEC3S 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_INSTANCEFLAGS_U16
#undef SDW_INLINE_SCNOBJECT_SETROTATION_VEC3S
#define SDW_INLINE_SCNOBJECT_SETHEADING_S16 1
#define SDW_INLINE_SCNOBJECT_SETVISIBLE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETHEADING_S16
#undef SDW_INLINE_SCNOBJECT_SETVISIBLE_S32
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_CINE_ISFINISHED 1
#include "../engine/cine_inlines.h"
#undef SDW_INLINE_CINE_ISFINISHED
extern Wolf *g_pWolf;
extern s32 g_dtMs;
#include "../app/app_main.h"
#include "../engine/cine.h"
#include "camera.h"
#include "../engine/scn_tools.h"
#include "../engine/input.h"

/* 0x57b410 - the cinematic header's static copy of the 9-byte opcode stride table (src/engine/cine.cpp,
 * g_cineOpStride 0x5816fc). The original header defined it static, so every object including it carries its own
 * unreferenced copy at the head of its .data; defined here in its place. */
static u8 s_cineOpStride[9] = {0, 8, 8, 4, 2, 2, 4, 2, 2};

#define g_padMasks (g_inputMap + 4)

#define g_padCurButtons (g_pad.cur.buttons)

#define g_padPrevButtons (g_pad.prev.buttons)

s32 Scenaric_FindByClass(u16, ScnObject **, s32);
s32 ObjGrid_QueryBoxPoints(const CollBox *, ScnObject **);
s32 Vec3s_DistXZ(Vec3s *, Vec3s *);
s32 Vec3s_Dist(Vec3s *, Vec3s *);
void Camera_StartScripted(ScnObject *, Camera *, u16, u16, u16, Vec3s *, u16, u32, s32);
#include "../sdk/crt.h"
extern "C" s16 Math_RadiansToAngle4096(float);
#define SDW_INLINE_SCNOBJECT_STARTCAMERA_U16_U16_U16_VEC3S_U16_U32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_STARTCAMERA_U16_U16_U16_VEC3S_U16_U32
#define SDW_INLINE_FREE_PROPERTY_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_PROPERTY_VOID_U32
#define SDW_INLINE_FREE_CONTAINSXZ_BOX_VEC3S 1
#include "../engine/coll_box_inlines.h"
#undef SDW_INLINE_FREE_CONTAINSXZ_BOX_VEC3S
static inline s32 ScriptCameraActive()
{
    return g_camMode == CAM_SCRIPT_BLEND_IN || g_camMode == CAM_SCRIPT_TO_SCRIPT || g_camMode == CAM_SCRIPTED;
}
static inline ScnObject *ScriptOwner()
{
    if (ScriptCameraActive())
        return g_camScriptOwner;
    return 0;
}
static inline void StartCine(u32 id, u32 flags, void *text)
{
    g_cinePlayer.Start(id, flags, 0, 0, text, 0);
}

/* BYTES(slot-scope, inferred): the nested block(s) only order the frame: their locals are allocated after the enclosing scope's */
void CanonSheep::PostLoadInit()
{
    u16 *props = record;
    {
        loadBox = Scn_GetPropBox(props, 0x24);
        activationBox = Scn_GetPropBox(props, 0);
        launchPos = pos;
        launchPos.y -= 200;
        Scenaric_FindByClass(CLASSID_CANNONBALL, &ball, 1);
        ball->HandleMessage(this, MSG_CB_PLACE, &launchPos);
        Scenaric_FindByClass(CLASSID_CANONDUMMY, &dummy, 1);
        dummy->SetVisible(0);
        detectBox = Scn_GetPropBox(props, 0x10);
        launchPoint1 = Scn_GetPropBox(props, 0x18);
        launchPoint2 = Scn_GetPropBox(props, 0x1c);
        launchPoint3 = Scn_GetPropBox(props, 0x20);
        for (u16 i = 0; i < 2; i++) {
            launchPoints[i].x = ((&launchPoint1)[i]->max[0] + (&launchPoint1)[i]->min[0]) / 2;
            launchPoints[i].y = ((&launchPoint1)[i]->max[1] + (&launchPoint1)[i]->min[1]) / 2;
            launchPoints[i].z = ((&launchPoint1)[i]->max[2] + (&launchPoint1)[i]->min[2]) / 2;
        }
        dummy->SetPosition(launchPoints);
        cine = Property(props, 4);
        cineFlag = Property(props, 8);
        cineText = (cineFlag & CINE_HAS_TEXT) ? Text_GetClassString((u8)Property(props, 12)) : 0;
        homeRot = rot;
        homeRot2.x = homeRot.x;
        homeRot2.y = homeRot.y;
        homeRot2.z = homeRot.z;
        rumbleMotors[0] = 0xfe;
        rumbleMotors[1] = 0xff;
        SetUpdateMode(SCN_UPD_ALWAYS);
        Reset();
    }
}
void CanonSheep::Reset()
{
    loaded = 0;
    wolfFrozen = 0;
    dummyReady = 1;
    ballReady = 1;
    SetState(CANONSHEEP_ST_IDLE);
}
/* BYTES(slot-scope, inferred): the nested block(s) only order the frame: their locals are allocated after the enclosing scope's */
void CanonSheep::Update()
{
    s32 index;
    {
        ScnObject *objects[64];
        {
            s32 count;
            switch (state) {
                case CANONSHEEP_ST_IDLE:
                    if (ContainsXZ(detectBox, &g_pWolf->pos)) {
                        AimDummyAlong(ComputeLaunchVelocity(g_pWolf->pos));
                        if (ballReady && dummyReady && !g_pWolf->HandleMessage(this, MSG_WOLF_IS_DEAD, 0)) {
                            SetState(CANONSHEEP_ST_AIM);
                            break;
                        }
                        if (dummyReady) {
                            launchPos = NearestPoint(launchPoints, 2, g_pWolf->pos);
                            AimDummyAlong(ComputeLaunchVelocity(g_pWolf->pos));
                            if (lastLaunchPos.x != launchPos.x || lastLaunchPos.y != launchPos.y ||
                                lastLaunchPos.z != launchPos.z) {
                                dummy->HandleMessage(this, MSG_CD_MOVE_TO, &launchPos);
                                dummyReady = 0;
                            }
                            lastLaunchPos.x = launchPos.x;
                            lastLaunchPos.y = launchPos.y;
                            lastLaunchPos.z = launchPos.z;
                        }
                    } else if (!loaded) {
                        /* cast kept: Box and CollBox are two views of one 16-byte record */
                        count = ObjGrid_QueryBoxPoints((CollBox *)loadBox, objects);
                        for (index = 0; index < count; index++) {
                            if (objects[index]->GetClassId() == CLASSID_SHEEP &&
                                !objects[index]->InstanceFlags(INST_F_ATTACHED) &&
                                objects[index]->HandleMessage(this, MSG_SHEEP_ENTER_CANNON, 0)) {
                                SetState(CANONSHEEP_ST_LOAD1);
                                break;
                            }
                        }
                    }
                    break;
                case CANONSHEEP_ST_WOLF_INSIDE:
                    StartCamera(0x3e00, 0, 0, &launchPos, 700, 0);
                    if (!(g_padCurButtons & ~g_padMasks[10]) && (g_padPrevButtons & ~g_padMasks[10]) && fireArmed) {
                        SetState(CANONSHEEP_ST_CINEMATIC);
                        break;
                    } else if (!(g_padCurButtons & ~g_padMasks[8]) && (g_padPrevButtons & ~g_padMasks[8]))
                        SetState(CANONSHEEP_ST_IDLE);
                    else
                        fireArmed = 1;
                    break;
                case CANONSHEEP_ST_LOAD1:
                    if (AnimFlags(ANIM_F_FINISHED))
                        SetState(CANONSHEEP_ST_LOAD2);
                    break;
                case CANONSHEEP_ST_LOAD2:
                    if (AnimFlags(ANIM_F_FINISHED))
                        SetState(CANONSHEEP_ST_CLOSE);
                    break;
                case CANONSHEEP_ST_CLOSE:
                    if (AnimFlags(ANIM_F_FINISHED)) {
                        loaded = 1;
                        SetState(CANONSHEEP_ST_IDLE);
                    }
                    break;
                case CANONSHEEP_ST_CINEMATIC:
                    if (g_cinePlayer.IsFinished())
                        SetState(CANONSHEEP_ST_IDLE);
                    break;
                case CANONSHEEP_ST_AIM:
                    AimDummyAlong(ComputeLaunchVelocity(g_pWolf->pos));
                    aimTimeMs -= g_dtMs;
                    if (aimTimeMs < 0)
                        SetState(CANONSHEEP_ST_FIRE);
                    break;
                case CANONSHEEP_ST_FIRE:
                    SetState(CANONSHEEP_ST_IDLE);
                    break;
            }
            AdvanceAnim();
        }
    }
}
s32 CanonSheep::HandleMessage(ScnObject *sender, u32 message, void *arg)
{
    switch (message) {
        case MSG_QUERY_ACTION:
            if (sender->GetClassId() == CLASSID_WOLF && state == CANONSHEEP_ST_IDLE && loaded == 1) {
                if (ContainsXZ(activationBox, &g_pWolf->pos))
                    return CTX_CANNON;
                return CTX_NONE;
            }
            break;
        case MSG_USE:
            SetState(CANONSHEEP_ST_WOLF_INSIDE);
            return 1;
        case MSG_FREEZE:
            SetState(CANONSHEEP_ST_IDLE);
            wolfFrozen = 0;
            return 1;
        case MSG_BALL_RETURNED:
            ball->HandleMessage(this, MSG_CB_PLACE, &launchPos);
            ballReady = 1;
            SetState(CANONSHEEP_ST_IDLE);
            break;
        case MSG_CANONSHEEP_DUMMY_READY:
            dummyReady = 1;
            break;
    }
    return 0;
}
/* BYTES(slot-group): locals grouped in work only to pin the original frame offsets; deltaPad, previousPad fill gaps */
void CanonSheep::SetState(u8 next)
{
    /* Names and padding preserve the original local frame, not an inferred game structure. */
    struct Work {
        s32 distance;
        Vec3s delta;
        u16 deltaPad;
        Vec3s previous;
        u16 previousPad;
    } work;
    state = next;
    switch (next) {
        case CANONSHEEP_ST_IDLE:
            if (!loaded)
                PlayAnim(ACANON02_ANIM_OPEN, 0, 0);
            if (wolfFrozen)
                wolfFrozen = !g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0);
            if (ScriptOwner() == this)
                Camera_SetMode(CAM_FOLLOW, 0);
            g_pWolf->HandleMessage(this, MSG_WOLF_SET_INVISIBLE, (void *)1); /* cast kept: arg carries a number */
            SetVisible(1);
            break;
        case CANONSHEEP_ST_LOAD1:
            PlayAnim(ACANON02_ANIM_LOAD1, 0, 0);
            g_pWolf->HandleMessage(this, MSG_WOLF_SET_INVISIBLE, (void *)1); /* cast kept: arg carries a number */
            SetVisible(1);
            break;
        case CANONSHEEP_ST_LOAD2:
            PlayAnim(ACANON02_ANIM_LOAD2, 0, 0);
            break;
        case CANONSHEEP_ST_CLOSE:
            PlayAnim(ACANON02_ANIM_CLOSE, 0, 0);
            break;
        case CANONSHEEP_ST_EXPLODE:
            PlayAnim(ACANON02_ANIM_EXPLODE, 0, 0);
            break;
        case CANONSHEEP_ST_EXPLODE_END:
            PlayAnim(ACANON02_ANIM_EXPLODE1, 0, 0);
            break;
        case CANONSHEEP_ST_WOLF_INSIDE:
            launchPos = pos;
            launchPos.y -= 200;
            fireArmed = 0;
            if (!wolfFrozen)
                wolfFrozen = g_pWolf->HandleMessage(this, MSG_FREEZE, 0);
            g_pWolf->HandleMessage(this, MSG_WOLF_SET_INVISIBLE, 0);
            SetVisible(0);
            break;
        case CANONSHEEP_ST_FIRE:
            g_pad.Rumble_stub(250, rumbleMotors, 0x1000);
            work.distance = Vec3s_Dist(&launchPos, &g_pWolf->pos);
            ball->HandleMessage(this, MSG_CB_SET_LETHAL, (void *)1); /* cast kept: arg carries a number */
            fireTarget.x = g_pWolf->pos.x;
            fireTarget.y = g_pWolf->pos.y;
            fireTarget.z = g_pWolf->pos.z;
            fireTarget.y -= 60;
            if (work.distance > 3500)
                work.previous = ComputeLaunchVelocity(aimStartTarget);
            else
                work.previous = ComputeLaunchVelocity(fireTarget);
            launchVel = ComputeLaunchVelocity(fireTarget);
            work.delta.x = launchVel.x - work.previous.x;
            work.delta.y = launchVel.y - work.previous.y;
            work.delta.z = launchVel.z - work.previous.z;
            dummy->SetHeading(work.delta.y);
            if (work.distance > 3500) {
                work.delta.x = (work.delta.x * work.distance) / 800;
                work.delta.y = (work.delta.y * work.distance) / 800;
                work.delta.z = (work.delta.z * work.distance) / 800;
            } else {
                work.delta.x = (work.delta.x * work.distance) / 1200;
                work.delta.y = (work.delta.y * work.distance) / 1200;
                work.delta.z = (work.delta.z * work.distance) / 1200;
            }
            launchVel.x += work.delta.x;
            launchVel.y += work.delta.y;
            launchVel.z += work.delta.z;
            ballReady = 0;
            ball->HandleMessage(this, MSG_CB_LAUNCH, &launchVel);
            break;
        case CANONSHEEP_ST_AIM:
            dummy->SetVisible(1);
            ball->HandleMessage(this, MSG_CB_PLACE, &launchPos);
            SetVisible(1);
            aimTimeMs = 400;
            aimStartTarget.x = g_pWolf->pos.x;
            aimStartTarget.y = g_pWolf->pos.y;
            aimStartTarget.z = g_pWolf->pos.z;
            aimStartTarget.y -= 60;
            dummy->HandleMessage(this, MSG_CD_RECOIL, 0);
            break;
        case CANONSHEEP_ST_CINEMATIC:
            StartCine(cine, cineFlag, cineText);
            break;
    }
}
Vec3s CanonSheep::ComputeLaunchVelocity(Vec3s target)
{
    Vec3s delta;
    s32 distance;
    delta.x = target.x - launchPos.x;
    delta.y = target.y - launchPos.y;
    delta.z = target.z - launchPos.z;
    distance = Vec3s_DistXZ(&target, &launchPos);
    delta.x = (delta.x * 0x1ffe) / distance;
    delta.y = (delta.y * 0x1ffe) / distance;
    delta.z = (delta.z * 0x1ffe) / distance;
    return delta;
}
/* BYTES(slot-group, inferred): locals grouped in squares only to pin the original frame offsets */
/* BYTES(slot-scope, inferred): the nested block(s) only order the frame: their locals are allocated after the enclosing scope's */
void CanonSheep::AimDummyAlong(Vec3s direction)
{
    struct Squares {
        s32 x, y, z;
    } squares;
    squares.x = direction.x * direction.x;
    squares.y = direction.y * direction.y;
    squares.z = direction.z * direction.z;
    {
        Vec3s rotation;
        rotation.x = 0;
        rotation.y = (s16)(Math_RadiansToAngle4096((float)atan2(direction.x, direction.z)) & 0xfff) + 0x400;
        rotation.z = 0;
        dummy->SetRotation(&rotation);
    }
}
Vec3s CanonSheep::NearestPoint(Vec3s *points, u16 count, Vec3s from)
{
    Vec3s best;
    s32 bestDistance = 0;
    for (u8 i = 0; i < count; i++) {
        s32 distance = Vec3s_DistXZ(&points[i], &from);
        if (distance < bestDistance || bestDistance == 0) {
            bestDistance = distance;
            best = points[i];
        }
    }
    return best;
}
ScnObject *CanonSheep_Create(void *record)
{
    ScnBody *object = new CanonSheep;
    object = object->Init(record, 0);
    return object;
}
