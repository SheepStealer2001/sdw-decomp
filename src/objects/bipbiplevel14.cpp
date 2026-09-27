/*
 * T063 - original object BipbipLevel14.cpp (guessed name): SheepD3D.exe .text 0x42dae0-0x42ec11, then the COMDATs
 * ScnObject::HandleMessage 0x42ec20-0x42ec2f (ScnObject vtable slot 4, the default), ScnBody::Update 0x42ec30-0x42ec43
 * (ScnBody slot 1) and ScnBody::CustomCollide 0x42ec50-0x42ec5f (ScnBody slot 3); .rdata 0x574bf8-0x574c60
 * (BipbipLevel14's vtable, then the COMDAT vtables ??_7ScnBody 0x574c1c and ??_7ScnObject 0x574c40, which this object
 * is the first in link order to emit). No .data.
 *
 * Layout: BipbipLevel14::Reset is defined after TrajStep, at its address (0x42eb66), for the object's .text order;
 * the three base-class default slots are INLINE here: they are COMDATs of the first object that emits the
 * ScnObject / ScnBody vtables, which is this one (through the implicit BipbipLevel14 constructor in
 * BipbipLevel14_Create). In the original they were inline virtuals in the class headers; the generated sdw_classes.h
 * declares them without a body, so the `inline` definitions follow it.
 *
 * BipbipLevel14 (class 139, vtable 0x574bf8, sizeof 0xe4) - the Road Runner of Level 14 (disc Lvl-16), the train level.
 * He waits on the first point of GOTOTRAJECTORY until Ralph is within about 1900 units, runs the path to its end, and
 * parks on the SensibleButton he finds there for TIMEWAIT (10 s) before running RETURNTRAJECTORY back to his start.
 * A FallingGate within 1200 units that answers MSG_GATE_QUERY turns him round; being inside BOXSTOPBIPBIP while the
 * train station (TRAINSTATION) answers MSG_STATION_QUERY_TRAIN_ZONE stops him; and at very close range he sends Ralph
 * MSG_WOLF_HIT_BY_BIPBIP every frame, which is the hit that spins the player.
 *
 * States (+0x64, BipbipLevel14State): EAT idle at the start point, START the start-running animation, RUN running
 * GOTOTRAJECTORY forward, RUN_ROUTE running it backward, WAIT waiting waitLeftMs then entering stateAfterWait,
 * STOP_AT_STATION held at the station, STAND_WATCH giving up the chase (idle animation, sound stopped), RETURN running
 * RETURNTRAJECTORY back to the start point.
 * Devices that only pin the original code generation: the inline helpers below (their names are descriptive; each one is
 * here because its expansion gives the original's stack temporaries), and, in the two big functions, one `w` struct
 * holding the locals - under /Od a local's slot follows from a hash of its NAME (tools/vc6_locals.py), and grouping
 * them in one struct pins the offsets instead. BipbipLevel14_TrajStep keeps named locals (restart, dist, relX, relZ,
 * arrived, next, tries), whose buckets reproduce the original frame.
 * Two more shapes are only about arrangement, not behaviour. In TrajStep the waypoint loop is written with the guard
 * as `if (tries-- <= 0) break;` and only `arrived` left in the while, which is what gives the original's
 * `jg / jmp` pair instead of a single inverted branch; and its two velocity components are written
 * `relX * f->speed` rather than `f->speed * relX`, because VC6 folds an int operand into the imul when it comes
 * second and loads it into a register when the s16 one does (as in src/objects/diamond.cpp).
 * A shape that reproduces the bytes is a representation, not proof that the original source read that way.
 */
/* BYTES: flow, slot-name. */

#define SDW_MEMBERS_ScnObject                           \
    static void *operator new(u32 size);                \
    void SetFacing(s16 f);                              \
    /* inline: the handle is a stack temp (0x42dfd0) */ \
    u16 SoundHandleOf(u16 handle)                       \
    {                                                   \
        return handle;                                  \
    }                                                   \
    void SetUpdateMode(u32 mode);

#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *inst, Animator *animator, u16 id, u32 opts); /* 0x550196 */
#include "../engine/sound_mgr.h"
#include "../engine/scn_tools.h"
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_SETFACING_S16 1
#define SDW_INLINE_SCNOBJECT_STOPSOUND_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETFACING_S16
#undef SDW_INLINE_SCNOBJECT_STOPSOUND_U16
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U32
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16

/* ---- the base-class default slots: inline, so VC6 emits each as a COMDAT with the vtable that needs it, as the
 * original's class headers had them ---- */

/* 0x42ec20 ScnObject_HandleMessage_default: ScnObject's slot 4 */
inline s32 ScnObject::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    return 0;
}

/* 0x42ec30 ScnBody_Update_default: ScnBody's slot 1 */
inline void ScnBody::Update()
{
    AdvanceAnim();
}

/* 0x42ec50 ScnObject_CustomCollide_default: really ScnBody's slot 3 (ScnObject's slot 3 is _purecall) */
inline s32 ScnBody::CustomCollide(ScnObject *querier, CollBox *mover, Vec3s *disp, s32 *outFrac, s32 *outY,
                                  CollContact *contacts, s32 *nContacts, u32 mode)
{
    return 0;
}

u16 Sound_Play(u16 soundId, void *owner, u16 volume, u8 flags, s32 rate); /* 0x5491b8 */
s32 Vec3s_DistSqXZ(Vec3s *a, Vec3s *b);                                   /* 0x51588e */
extern "C" s16 Math_RadiansToAngle4096(float radians);                    /* 0x5269ce */
#include "../sdk/crt.h"

extern Wolf *g_pWolf; /* 0x6cf310 */
extern s32 g_dtMs;    /* 0x71b2e8 */

/* A designer property of the WAR record: the dword at record + 0x14 + offset (BipbipLevel14Props). Inlined; the
 * offset is a stack temp, which it is with a u32 parameter (0x42db8c). */
#define SDW_INLINE_FREE_SCN_GETPROPS32_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_SCN_GETPROPS32_VOID_U32

/* Whether p lies inside box in x and z (the vertical is not tested): both parameters are stack temps (0x42e1d2). */
#define SDW_INLINE_FREE_BOX_CONTAINSPOINTXZ_BOX_VEC3S 1
#include "../engine/coll_box_inlines.h"
#undef SDW_INLINE_FREE_BOX_CONTAINSPOINTXZ_BOX_VEC3S

/* The frame of PostLoadInit: `props` sits at EBP-4 with two unread 4-byte slots below it. One struct reproduces that
 * (grouping locals pins their offsets; see the file header). */
struct BipInitWork {
    u8 unread[8];
    u16 *props;
};

/* 0x42dae0 - vtable +0x00: reads the two trajectories, the wait time, the station and the stop box, arms both
 * followers and parks the Road Runner on the first point of GOTOTRAJECTORY, snapped to the ground. */
void BipbipLevel14::PostLoadInit()
{
    BipInitWork w;
    w.props = record;
    gotoTraj = Scn_GetPropTrajectory(w.props, 4);
    returnTraj = Scn_GetPropTrajectory(w.props, 8);
    trainStation = Scn_GetPropObject(w.props, 16);
    TrajInit(&gotoFollower, gotoTraj, 0x514, 0x800, 1, 0, 0x32);
    TrajFollower_Init(&returnFollower, returnTraj, 0x514, 0x800, 1, 0, 0x32);
    waitTimeMs = Scn_GetPropS32(w.props, 12);
    stopBox = Scn_GetPropBox(w.props, 0);
    startPos.x = gotoTraj->pts[0].x;
    startPos.y = gotoTraj->pts[0].y;
    startPos.z = gotoTraj->pts[0].z;
    startPos.y = -500;
    startPos.y = QueryGroundY(&startPos, 1);
    SetPosition(&startPos);
    chased = 0;
    gateShut = 0;
    unk088 = 0;
    SetUpdateMode(SCN_UPD_ALWAYS);
    soundHandle = 0;
    SetState(BIPBIP14_ST_EAT);
}

/* The frame of Update: nine locals whose slots the original's names decided; one struct with explicit padding
 * reproduces them (see the file header). */
struct BipUpdateWork {
    Vec3s mv; /* +0x00 the translation of BIPBIP14_ST_RETURN */
    u8 pad06[4];
    s16 ang; /* +0x0a angle from the Road Runner to Ralph */
    Vec3s d; /* +0x0c pos - Ralph's pos */
    u8 pad12[2];
    Vec3s step; /* +0x14 vel scaled by the frame time */
    u8 pad1a[2];
    s32 distSq; /* +0x1c squared XZ distance to Ralph */
    Vec3s vel;  /* +0x20 velocity from the follower */
    u8 pad26[2];
    Vec3s np; /* +0x28 the position being committed */
    u8 pad2e[4];
    s16 heading; /* +0x32 facing from the follower */
    s32 res;     /* +0x34 1 when the follower reached the end of the path */
};

/* 0x42dd3d - vtable +0x04: the state machine, then the animation. */
void BipbipLevel14::Update()
{
    BipUpdateWork w;
    w.distSq = Vec3s_DistSqXZ(&g_pWolf->pos, &pos);
    switch (state) {
        case BIPBIP14_ST_EAT:
            if (w.distSq <= 0x15f900)
                SetState(BIPBIP14_ST_START);
            break;
        case BIPBIP14_ST_STAND_WATCH:
            if (w.distSq <= 0x15f900)
                SetState(BIPBIP14_ST_START);
            if (w.distSq > 0x225510)
                SetState(BIPBIP14_ST_RUN_ROUTE);
            break;
        case BIPBIP14_ST_START:
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(BIPBIP14_ST_RUN);
            break;
        case BIPBIP14_ST_RUN:
            gate = 0;
            gate = Scenaric_FindNearestOfClass(&pos, CLASSID_FALLINGGATE, -0x320, 0x320, 0x4b0, 0, 0);
            if (gate && gate->HandleMessage(this, MSG_GATE_QUERY, 0)) {
                gateShut = 1;
                SetState(BIPBIP14_ST_RUN_ROUTE);
                break;
            }
            w.d.x = pos.x - g_pWolf->pos.x;
            w.d.y = pos.y - g_pWolf->pos.y;
            w.d.z = pos.z - g_pWolf->pos.z;
            w.ang = Math_RadiansToAngle4096((float)atan2((double)w.d.x, (double)w.d.z)) & 0xfff;
            if (w.ang > 0x400 && w.ang < 0xc00 && chased) {
                gateShut = 0;
                gate = 0;
                gate = Scenaric_FindNearestOfClass(&pos, CLASSID_FALLINGGATE, -0x320, 0x320, 0x4b0, 0, 0);
                if (gate && gate->HandleMessage(this, MSG_GATE_QUERY, 0)) {
                    gateShut = 1;
                    chased = 0;
                    waitLeftMs = waitTimeMs / 2;
                    stateAfterWait = BIPBIP14_ST_RUN_ROUTE;
                    if (Sound_IsPlaying(SoundHandleOf(soundHandle)))
                        StopSound(soundHandle);
                    SetState(BIPBIP14_ST_WAIT);
                    break;
                }
            } else if (w.distSq >= 0x19c990) {
                if (Sound_IsPlaying(SoundHandleOf(soundHandle)))
                    StopSound(soundHandle);
                SetState(BIPBIP14_ST_STAND_WATCH);
            }
            w.res = TrajStep(&gotoFollower, &w.vel, &w.heading, 1);
            if (w.res) {
                button = 0;
                button = Scenaric_FindNearestOfClass(&pos, CLASSID_SENSIBLEBUTTON, -0x320, 0x320, 0xc8, 0, 0);
                if (button)
                    SetPosition(&button->pos);
                chased = 0;
                waitLeftMs = waitTimeMs;
                stateAfterWait = BIPBIP14_ST_RETURN;
                if (Sound_IsPlaying(SoundHandleOf(soundHandle)))
                    StopSound(soundHandle);
                SetState(BIPBIP14_ST_WAIT);
            } else {
                SetFacing(w.heading);
                Vec3s_ScaleByDt(&w.vel, &w.step);
                w.np.x = pos.x;
                w.np.y = pos.y;
                w.np.z = pos.z;
                w.np.x += w.step.x;
                w.np.y += w.step.y;
                w.np.z += w.step.z;
                w.np.y -= 200;
                w.np.y = QueryGroundY(&w.np, 0);
                SetPosition(&w.np);
            }
            if (Box_ContainsPointXZ(stopBox, &pos) &&
                trainStation->HandleMessage(this, MSG_STATION_QUERY_TRAIN_ZONE, 0)) {
                if (Sound_IsPlaying(SoundHandleOf(soundHandle)))
                    StopSound(soundHandle);
                SetState(BIPBIP14_ST_STOP_AT_STATION);
                break;
            }
            if (w.distSq <= 0x9c4)
                g_pWolf->HandleMessage(this, MSG_WOLF_HIT_BY_BIPBIP, 0);
            break;
        case BIPBIP14_ST_RUN_ROUTE:
            if (w.distSq <= 0x19c990 && !gateShut) {
                if (Sound_IsPlaying(SoundHandleOf(soundHandle)))
                    StopSound(soundHandle);
                SetState(BIPBIP14_ST_STAND_WATCH);
                break;
            }
            w.res = TrajStep(&gotoFollower, &w.vel, &w.heading, 0);
            if (w.res) {
                gateShut = 0;
                if (Sound_IsPlaying(SoundHandleOf(soundHandle)))
                    StopSound(soundHandle);
                SetState(BIPBIP14_ST_EAT);
            } else {
                SetFacing(w.heading);
                Vec3s_ScaleByDt(&w.vel, &w.step);
                w.np.x = pos.x;
                w.np.y = pos.y;
                w.np.z = pos.z;
                w.np.x += w.step.x;
                w.np.y += w.step.y;
                w.np.z += w.step.z;
                w.np.y -= 200;
                w.np.y = QueryGroundY(&w.np, 0);
                SetPosition(&w.np);
            }
            if (w.distSq <= 0x9c4)
                g_pWolf->HandleMessage(this, MSG_WOLF_HIT_BY_BIPBIP, 0);
            break;
        case BIPBIP14_ST_STOP_AT_STATION:
            chased = 1;
            if (!trainStation->HandleMessage(this, MSG_STATION_QUERY_TRAIN_ZONE, 0))
                SetState(BIPBIP14_ST_START);
            break;
        case BIPBIP14_ST_WAIT:
            waitLeftMs -= g_dtMs;
            if (waitLeftMs <= 0)
                SetState(stateAfterWait);
            break;
        case BIPBIP14_ST_RETURN:
            w.res = TrajFollower_Step(&returnFollower, &w.vel, &w.heading);
            if (w.res) {
                SetPosition(&startPos);
                SetState(BIPBIP14_ST_EAT);
            } else {
                SetFacing(w.heading);
                Vec3s_ScaleByDt(&w.vel, &w.mv);
                Translate(&w.mv);
            }
            break;
    }
    AdvanceAnim();
}

/* 0x42e588 - vtable +0x10: the Road Runner answers nothing. */
s32 BipbipLevel14::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    switch (msgId) {
    }
    return 0;
}

/* 0x42e59f - the state change: the animation, and the running loop's sound. */
void BipbipLevel14::SetState(u8 newState)
{
    switch (newState) {
        case BIPBIP14_ST_EAT:
            chased = 0;
            SetPosition(&startPos);
            TrajInit(&gotoFollower, gotoTraj, 0x514, 0x800, 1, 0, 0x32);
            StopSound(soundHandle);
            PlayAnim(ABIPBI01_ANIM_EAT, 1, 1);
            break;
        case BIPBIP14_ST_START:
            PlayAnim(ABIPBI01_ANIM_START, 1, 1);
            break;
        case BIPBIP14_ST_RUN:
            soundHandle = Sound_Play(SND_ROADRUNNER_RUN, this, 0xff, SNDF_LOOP | SNDF_POSITIONAL, 0x1000);
            PlayAnim(ABIPBI01_ANIM_RUN1, 1, 1);
            break;
        case BIPBIP14_ST_RUN_ROUTE:
            soundHandle = Sound_Play(SND_ROADRUNNER_RUN, this, 0xff, SNDF_LOOP | SNDF_POSITIONAL, 0x1000);
            PlayAnim(ABIPBI01_ANIM_RUN1, 1, 1);
            break;
        case BIPBIP14_ST_WAIT:
            PlayAnim(ABIPBI01_ANIM_STAND, 1, 1);
            break;
        case BIPBIP14_ST_STOP_AT_STATION:
            PlayAnim(ABIPBI01_ANIM_STOP2, 0, 0);
            break;
        case BIPBIP14_ST_STAND_WATCH:
            StopSound(soundHandle);
            PlayAnim(ABIPBI01_ANIM_STAND, 1, 1);
            break;
        case BIPBIP14_ST_RETURN:
            PlayAnim(ABIPBI01_ANIM_RUN1, 1, 1);
            break;
    }
    state = newState;
}

/* 0x42e91d - this file's own copy of TrajFollower_Init (0x510731 is the other one; the two are byte for byte the
 * same, so the source is shared and each translation unit got a copy). */
void BipbipLevel14::TrajInit(TrajFollower *f, Trajectory *traj, s16 speed, s16 bias, s32 continuousHeading, s32 use3d,
                             s16 arriveRadius)
{
    f->traj = traj;
    f->pointIndex = 0;
    f->arriveRadiusSq = arriveRadius * arriveRadius;
    f->speed = speed;
    f->headingBias = bias;
    f->heading = 0;
    f->continuousHeading = continuousHeading;
    f->use3dDistance = use3d;
    f->moving = 0;
}

/* 0x42e987 - this class's own follower step: XZ only, and it can walk the path backwards (forward = 0). It consumes
 * every waypoint already reached, up to 200 in one call, and returns 1 when the path ran off either end (the index
 * is then put back to 0). The velocity is speed * direction, the heading the bias plus atan2 of the direction. */
/* BYTES(flow): the guard is a break inside the loop: that gives the original's jg / jmp pair */
/* BYTES(flow): operand order: VC6 folds the int operand into the imul only when it comes second */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py) */
s32 BipbipLevel14::TrajStep(TrajFollower *f, Vec3s *outVel, s16 *outHeading, s32 forward)
{
    s16 tries;
    Vec3s *next;
    s32 arrived;
    s32 relZ;
    s32 relX;
    s32 dist;
    s32 restart;

    restart = 0;
    f->advanced = 0;
    f->moving = 1;
    tries = 200;
    do {
        next = &f->traj->pts[f->pointIndex];
        relX = next->x - pos.x;
        relZ = next->z - pos.z;
        dist = relX * relX + relZ * relZ;
        arrived = dist < f->arriveRadiusSq;
        if (arrived) {
            if (forward)
                f->pointIndex++;
            else
                f->pointIndex--;
            f->advanced = 1;
            if (f->pointIndex >= f->traj->count || f->pointIndex < 0) {
                restart = 1;
                f->pointIndex = 0;
            }
        }
        if (tries-- <= 0)
            break;
    } while (arrived);
    dist = (s32)sqrt((double)dist);
    outVel->x = (s16)(relX * f->speed / dist);
    outVel->z = (s16)(relZ * f->speed / dist);
    outVel->y = 0;
    if (f->advanced || f->continuousHeading) {
        f->heading = (f->headingBias + Math_RadiansToAngle4096((float)atan2((double)relX, (double)relZ))) & 0xfff;
        *outHeading = f->heading;
    } else {
        *outHeading = f->heading;
    }
    return restart;
}

/* 0x42eb66 - vtable +0x14: back to the start point, idle. */
void BipbipLevel14::Reset()
{
    chased = 0;
    unk088 = 0;
    SetPosition(&startPos);
    SetState(BIPBIP14_ST_EAT);
}

/* 0x42ebaa - the class factory for CLASSID_BIPBIPLEVEL14. */
ScnObject *BipbipLevel14_Create(void *record)
{
    ScnObject *obj = new BipbipLevel14;
    obj = ((ScnBody *)obj)->Init((u16 *)record, 0); /* cast kept: obj is a new BipbipLevel14; Init reads raw words */
    return obj;
}
