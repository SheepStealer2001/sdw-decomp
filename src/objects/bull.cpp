/* PAL PC bull (a ScnMobile): bull_Init 0x42ec60 .. bull_Create 0x431c79. */
/* BYTES: slot-group, temp, view. */
/* BYTES(view): view: raw member storage is viewed through the matching engine record (the access widths are the original's) */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *instance, Animator *animation, u16 id, u32 options);
#include "../engine/id_list.h"
#include "../engine/sound_mgr.h"
#include "../engine/scn_tools.h"
#define SDW_INLINE_FREE_INSTFLAGSSET_U16_U16 1
#include "instance_inlines.h"
#undef SDW_INLINE_FREE_INSTFLAGSSET_U16_U16
#define SDW_INLINE_FREE_INSTFLAGSCLEAR_U16_U16 1
#include "instance_inlines.h"
#undef SDW_INLINE_FREE_INSTFLAGSCLEAR_U16_U16

#define SDW_MEMBERS_ZoneList void Load(u16);

#define SDW_MEMBERS_ScnObject                                                                       \
    static void *operator new(u32 size);                                                            \
    void SetFacing(s16 value);                                                                      \
    ScnObject *Parent();                                                                            \
    ScnObject *Scenaric_FindBestInRadius(Vec3s *, s16, s16, u16, u16 *,                             \
                                         u32 (*)(ScnObject *, ScnObject *, u32, ScnObject *), s32); \
    void SetUpdateMode(s32 mode);

#define SDW_MEMBERS_InlineEmitter16 InlineEmitter16();

#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_FACING 1
#define SDW_INLINE_SCNOBJECT_SETFACING_S16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_FACING
#undef SDW_INLINE_SCNOBJECT_SETFACING_S16
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_INSTANCEFLAGS_U16 1
#define SDW_INLINE_SCNOBJECT_STOPSOUND_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_INSTANCEFLAGS_U16
#undef SDW_INLINE_SCNOBJECT_STOPSOUND_U16
#define SDW_INLINE_SCNOBJECT_ENABLEBOXCOLLIDE_S32 1
#define SDW_INLINE_SCNOBJECT_ENABLETINT_S32 1
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_ENABLEBOXCOLLIDE_S32
#undef SDW_INLINE_SCNOBJECT_ENABLETINT_S32
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32
#define SDW_INLINE_SCNBODY_GETANIMID 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_GETANIMID
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
inline ScnObject *ScnObject::Parent()
{
    if (!InstanceFlags(INST_F_ATTACHED))
        return 0;
    return attachLink->parentObj;
}
inline InlineEmitter16::InlineEmitter16()
{
    base.slotPool = slotBuf;
    base.particles = particleBuf;
    base.count = 16;
    base.Emitter_Reset();
}
#define SDW_INLINE_ZONELIST_LOAD_U16 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_LOAD_U16
#define SDW_INLINE_ZONELIST_FINDCONTAINING_VEC3S 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_FINDCONTAINING_VEC3S
#define SDW_INLINE_LAUNCHARC_STEP_S32_S16_S16_S16 1
#include "../engine/launch_arc_inlines.h"
#undef SDW_INLINE_LAUNCHARC_STEP_S32_S16_S16_S16
#define SDW_INLINE_LAUNCHARC_INITCOEFFICIENTS_S32_S32_S32_S32_S32_S32_S32_S32_S32 1
#include "../engine/launch_arc_inlines.h"
#undef SDW_INLINE_LAUNCHARC_INITCOEFFICIENTS_S32_S32_S32_S32_S32_S32_S32_S32_S32
extern Wolf *g_pWolf;
extern u32 g_gameTime;
u16 Sound_Play(u16, void *, u16, u8, s32);
#include "../sdk/crt.h"
extern "C" s16 Math_RadiansToAngle4096(float);
s32 Vec3s_Dist(Vec3s *, Vec3s *), Vec3s_DistSq(Vec3s *, Vec3s *);
s32 ObjGrid_QueryBoxPoints(const CollBox *, ScnObject **);
u32 bull_TargetScoreCB(ScnObject *, ScnObject *, u32, ScnObject *);
s32 Scenaric_FindByClass(u16 classId, ScnObject **out, s32 maximum);
/* BYTES(temp, inferred): the offset and the result go through the read struct because the original's expansion keeps both in stack slots */
#define SDW_INLINE_FREE_PROPU32_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_PROPU32_VOID_U32
#define ANGLE_DIFF(a, b) ((s16)((s16)(((a) - (b) + 0x800) & 0xfff) - 0x800))
#define ABS_VALUE(a) ((a) >= 0 ? (a) : -(a))
#define SDW_INLINE_FREE_INBOX_BOX_VEC3S 1
#include "../engine/coll_box_inlines.h"
#undef SDW_INLINE_FREE_INBOX_BOX_VEC3S

void bull::PostLoadInit()
{
    void *properties = record;
    activationBoxes.Load((u16)PropU32(properties, 0));
    movementBoxes.Load((u16)PropU32(properties, 8));
    asleep = PropU32(properties, 4);
    sheepBox = Scn_GetPropBox(properties, 20);
    sheepTraj = Scn_GetPropTrajectory(properties, 28);
    sheepCam = Scn_GetPropCamera(properties, 24);
    noReturnBox = Scn_GetPropBox(properties, 12);
    wakeUpSpeedPct = (u16)PropU32(properties, 32);
    gossamer = 0;
    Scenaric_FindByClass(CLASSID_GOSSAMER_LEV08, &gossamer, 1);
    tossedSheep = 0;
    tossArc.t = 0;
    sheepCamShot.Init(1);
    SnapToGround(1);
    homePos = pos;
    homeFacing = Facing();
    chargeSpeed = 800;
    target = g_pWolf;
    SetState(BULL_ST_ASLEEP);
    dustEmitter.base.Emitter_Reset();
    dustParams.hSpeed = 50;
    dustParams.vSpeed = -60;
    dustParams.life = 8192;
    dustParams.spawnInterval = dustParams.life / 16;
    dustParams.sizeStart = 80;
    dustParams.sizeEnd = 180;
    dustParams.sheetIndex = 0;
    soundHandle = 0;
    chasingGossamer = 0;
    SetUpdateMode(SCN_UPD_ALWAYS);
}

void bull::Reset()
{
    soundHandle = 0;
    if (chasingGossamer)
        SetState(BULL_ST_CHARGE);
    else if (state != BULL_ST_ESCAPE) {
        target = g_pWolf;
        SetPosition(&homePos);
        dustEmitter.base.Emitter_Reset();
        SetState(BULL_ST_ASLEEP);
    }
}

void bull::Render(Camera *view)
{
    ScnMobile::Render(view);
    if (dustEmitter.base.flags.active)
        dustEmitter.base.Emitter_Render(view, 0);
}

/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets */
void bull::SetState(u8 next)
{
    struct Work {
        ScnObject *candidate;
        u8 unused[3], index;
    } w;
    state = next;
    if (Sound_IsPlaying(soundHandle) && next != BULL_ST_CHARGE && next != BULL_ST_SCARF_PAW)
        StopSound(soundHandle);
    anim.speed = 4096;
    switch (next) {
        case BULL_ST_ESCAPE:
            velocity.x /= 4;
            velocity.y = velocity.y << 2;
            velocity.z /= 4;
            PlayAnim(ABULL01_ANIM_FALL, 1, 1);
            soundHandle = Sound_Play(SND_SCOFALL, this, 255, SNDF_LOOP | SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 4096);
            break;
        case BULL_ST_ASLEEP:
            if (asleep) {
                woken = 0;
                PlayAnim(ABULL01_ANIM_SLEEP1, 1, 1);
            } else {
                SetFacing(homeFacing);
                PlayAnim(ABULL01_ANIM_STAND1, 1, 1);
                next = BULL_ST_IDLE;
            }
            break;
        case BULL_ST_CHARGE:
        case BULL_ST_SCARF_PAW:
            if (!Sound_IsPlaying(soundHandle))
                soundHandle =
                    Sound_Play(SND_BULL_CHARGE, this, 255, SNDF_LOOP | SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 4096);
            stateTime = g_gameTime;
            for (w.index = 0; w.index < movementBoxes.count; w.index++) {
                w.candidate = Scenaric_FindBestInRadius(&pos, movementBoxes.boxes[w.index]->min[1],
                                                        movementBoxes.boxes[w.index]->max[1], 5000, &targetDist,
                                                        bull_TargetScoreCB, 0);
                if (w.candidate)
                    break;
            }
            if (w.candidate && targetDist < 900)
                target = w.candidate;
            if (target->GetClassId() == CLASSID_GOSSAMER_LEV08) {
                chasingGossamer = 1;
                chargeSpeed = 1100;
            } else
                chargeSpeed = 800;
            if (w.candidate && w.candidate->GetClassId() == CLASSID_REDSCARF) {
                if (w.candidate->InstanceFlags(INST_F_ATTACHED)) {
                    target = w.candidate;
                    PlayAnim(ABULL01_ANIM_STAND2, 1, 0);
                    next = BULL_ST_SCARF_PAW;
                } else {
                    if (GetAnimId() != ABULL01_ANIM_RUN2)
                        PlayAnim(ABULL01_ANIM_RUN2, 1, 1);
                    next = BULL_ST_CHARGE;
                }
            } else {
                if (GetAnimId() != ABULL01_ANIM_RUN2)
                    PlayAnim(ABULL01_ANIM_RUN2, 1, 1);
                next = BULL_ST_CHARGE;
            }
            chargeTargetPos = target->pos;
            SetFacing((Math_RadiansToAngle4096(
                           (float)atan2((double)chargeTargetPos.x - pos.x, (double)chargeTargetPos.z - pos.z)) +
                       0x800) &
                      0xfff);
            break;
        case BULL_ST_IDLE:
            PlayAnim(ABULL01_ANIM_STAND2, 1, 0);
            break;
        case BULL_ST_SKID:
            stateTime = g_gameTime;
            skidFacing = Facing();
            PlayAnim(ABULL01_ANIM_BRAKE1B, 1, 0);
            break;
        case BULL_ST_HIT:
            if (target->Parent())
                target = target->Parent();
            if (target != tossedSheep) {
                if (target->GetClassId() == CLASSID_WOLF) {
                    /* cast kept: HandleMessage's arg is a void *; MSG_KILL passes the kill type in it */
                    if (!target->HandleMessage(this, MSG_KILL, (void *)KILL_FALL_TO_SENDER)) {
                        SetState(BULL_ST_CHARGE);
                        break;
                    }
                } else
                    target->HandleMessage(this, MSG_BUMP, 0);
                PlayAnim(ABULL01_ANIM_HIT1, 0, 1);
            } else {
                PlayAnim(ABULL01_ANIM_HIT2, 0, 1);
                tossedSheep->HandleMessage(this, MSG_LAUNCH, 0);
            }
            hitTime = g_gameTime;
            break;
        case BULL_ST_STUNNED:
            PlayAnim(ABULL01_ANIM_STRIKE2, 0, 1);
            break;
        case BULL_ST_SCARF_CHARGE:
            PlayAnim(ABULL01_ANIM_RUN1, 1, 0);
            break;
        case BULL_ST_RETURN_HOME:
            PlayAnim(ABULL01_ANIM_RUN2, 1, 0);
            break;
    }
    state = next;
}

/* Radius-search score callback: four arguments, unsigned score. */
u32 bull_TargetScoreCB(ScnObject *self, ScnObject *candidate, u32 distanceSquared, ScnObject *best)
{
    if (candidate->HandleMessage(self, MSG_BULL_TARGET_QUERY, 0) == 1)
        return distanceSquared;
    return -1;
}

/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; unused3, unused4, unused5 fill gaps */
void bull::Update()
{
    /* Original 0x188-byte source work area. Inline call temporaries are left
       to VC6; the gaps here reflect unused source storage, not object fields. */
    struct Work {
        Vec3s dustPos;
        u8 unused0[5], chargeIndex;
        s32 movedCharge, objectCount, objectIndex;
        ScnObject *objects[64];
        s32 movedRunoff;
        u8 unused1[2], runoffIndex, targetIndex;
        s32 moved;
        Vec3s skidVelocity;
        u8 unused2[5], skidIndex;
        Vec3s tossDelta;
        u16 unused3;
        u32 distanceSquared;
        Vec3s endPos;
        u16 unused4;
        s32 dz, dx;
        u32 distance;
        s32 foundGossamer;
        Vec3s delta;
        u16 unused5;
        ContactInfo contact;
        Vec3s point;
        u8 unused6[5], index;
        s32 found;
    } w;
    if (tossedSheep && state == BULL_ST_HIT) {
        w.point = tossedSheep->pos;
        if (tossArc.Step(((g_gameTime - tossClockBase) * 256) / 0x28000, &w.point.x, &w.point.y, &w.point.z)) {
            if (sheepCam)
                sheepCamShot.Update(this);
            w.tossDelta.x = w.point.x - tossedSheep->pos.x;
            w.tossDelta.y = w.point.y - tossedSheep->pos.y;
            w.tossDelta.z = w.point.z - tossedSheep->pos.z;
            if (tossArc.t < 128 ||
                !tossedSheep->Collide_ResolveMove(&w.tossDelta, 0, 0xb54, RESOLVE_SLIDE_ALL, 0, 0, 10, 0, 0))
                tossedSheep->Translate(&w.tossDelta);
            else {
                if (sheepCam)
                    sheepCamShot.Stop(this);
                tossedSheep->HandleMessage(this, MSG_LANDED, 0);
                target = g_pWolf;
                SetState(BULL_ST_CHARGE);
                tossedSheep = 0;
            }
        } else {
            if (sheepCam)
                sheepCamShot.Stop(this);
            tossedSheep->HandleMessage(this, MSG_LANDED, 0);
            target = g_pWolf;
            SetState(BULL_ST_CHARGE);
            tossedSheep = 0;
        }
    } else
        tossClockBase = g_gameTime;
    switch (state) {
        case BULL_ST_ASLEEP:
            if (woken) {
                if (AnimFlags(ANIM_F_FINISHED)) {
                    target = g_pWolf;
                    anim.speed = 4096;
                    SetState(BULL_ST_CHARGE);
                }
            } else if (activationBoxes.count) {
                w.distanceSquared = Vec3s_DistSq(&g_pWolf->pos, &pos);
                if ((activationBoxes.FindContaining(&g_pWolf->pos) &&
                     g_pWolf->HandleMessage(this, MSG_WOLF_IS_NOISY, 0) == 1) ||
                    w.distanceSquared < 40000) {
                    if (asleep) {
                        anim.speed = (wakeUpSpeedPct * 4096) / 100;
                        PlayAnim(ABULL01_ANIM_UP1A, 0, 1);
                    } else
                        PlayAnim(ABULL01_ANIM_STAND2, 0, 1);
                    woken = 1;
                }
            }
            break;
        case BULL_ST_SKID:
            if (GetAnimId() == ABULL01_ANIM_BRAKE1A) {
                if (g_gameTime - stateTime < 4096) {
                    w.skidVelocity.x = (velocity.x * (4096 - (g_gameTime - stateTime))) >> 12;
                    w.skidVelocity.y = 250;
                    w.skidVelocity.z = (velocity.z * (4096 - (g_gameTime - stateTime))) >> 12;
                    Vec3s_ScaleByDt(&w.skidVelocity, &w.delta);
                    w.endPos.x = pos.x + w.delta.x;
                    w.endPos.y = pos.y + w.delta.y;
                    w.endPos.z = pos.z + w.delta.z;
                    for (w.skidIndex = 0; w.skidIndex < movementBoxes.count; w.skidIndex++)
                        if (InBox(movementBoxes.boxes[w.skidIndex], &w.endPos)) {
                            Translate(&w.delta);
                            break;
                        }
                } else
                    SetState(BULL_ST_SKID);
            } else if (AnimFlags(ANIM_F_FINISHED))
                SetState(BULL_ST_CHARGE);
            break;
        case BULL_ST_CHARGE:
            w.dx = chargeTargetPos.x - pos.x;
            w.dz = chargeTargetPos.z - pos.z;
            if (Vec3s_Dist(&pos, &target->pos) <= 150) {
                SetFacing((Math_RadiansToAngle4096(
                               (float)atan2((double)target->pos.x - pos.x, (double)target->pos.z - pos.z)) +
                           0x800) &
                          0xfff);
                SetState(BULL_ST_HIT);
                break;
            }
            w.distance = (s32)sqrt((double)w.dx * w.dx + w.dz * w.dz);
            if (g_gameTime - stateTime > 8192 || w.distance < 100) {
                /* Both atan2 inputs really use X in the original. */
                if (ABS_VALUE(
                        ANGLE_DIFF(Math_RadiansToAngle4096((float)atan2(target->pos.x - pos.x, target->pos.x - pos.x)),
                                   Math_RadiansToAngle4096((float)atan2(w.dx, w.dz)) + 0x800)) < 128 &&
                    state != BULL_ST_SKID)
                    SetState(BULL_ST_SKID);
                else
                    SetState(BULL_ST_CHARGE);
            } else {
                velocity.x = w.dx * chargeSpeed / (s32)w.distance;
                velocity.z = w.dz * chargeSpeed / (s32)w.distance;
                velocity.y = 250;
                Vec3s_ScaleByDt(&velocity, &w.delta);
                Collide_ResolveMove(&w.delta, &w.contact, 0x578, RESOLVE_SLIDE_ALL, 0, 0, 10, 0, 0);
                if (w.contact.wallObj) {
                    if (!w.contact.wallObj->GetClassId())
                        /* cast kept: HandleMessage's arg is a void *; MSG_KILL passes the kill type in it */
                        w.contact.wallObj->HandleMessage(this, MSG_KILL, (void *)KILL_CRUSH);
                    else
                        w.contact.wallObj->HandleMessage(this, MSG_BUMP, 0);
                }
                w.endPos.x = pos.x + w.delta.x;
                w.endPos.y = pos.y + w.delta.y;
                w.endPos.z = pos.z + w.delta.z;
                w.moved = 0;
                for (w.index = 0; w.index < movementBoxes.count; w.index++)
                    if (InBox(movementBoxes.boxes[w.index], &w.endPos)) {
                        for (w.targetIndex = 0; w.targetIndex < movementBoxes.count; w.targetIndex++)
                            if (InBox(movementBoxes.boxes[w.targetIndex], &target->pos)) {
                                Translate(&w.delta);
                                w.moved = 1;
                                break;
                            }
                    }
                if (!w.moved) {
                    if (noReturnBox && !target->GetClassId() && InBox(noReturnBox, &target->pos) &&
                        target->HandleMessage(this, MSG_WOLF_IS_USING_SPECIAL_OBJECT, 0)) {
                        w.movedRunoff = 0;
                        for (w.runoffIndex = 0; w.runoffIndex < movementBoxes.count; w.runoffIndex++)
                            if (InBox(movementBoxes.boxes[w.runoffIndex], &w.endPos)) {
                                Translate(&w.delta);
                                w.movedRunoff = 1;
                                break;
                            }
                        if (!w.movedRunoff)
                            SetState(BULL_ST_ESCAPE);
                    } else
                        SetState(BULL_ST_RETURN_HOME);
                }
            }
            break;
        case BULL_ST_ESCAPE:
            if (InBox(noReturnBox, &pos)) {
                Vec3s_ScaleByDt(&velocity, &w.delta);
                Translate(&w.delta);
            } else {
                StopSound(soundHandle);
                RemoveFromWorld();
            }
            break;
        case BULL_ST_IDLE:
            if (asleep) {
                w.dx = target->pos.x - pos.x;
                w.dz = target->pos.z - pos.z;
                SetFacing((Math_RadiansToAngle4096((float)atan2(w.dx, w.dz)) + 0x800) & 0xfff);
            }
            w.foundGossamer = 0;
            if (gossamer) {
                for (w.index = 0; w.index < movementBoxes.count; w.index++)
                    if (InBox(movementBoxes.boxes[w.index], &gossamer->pos) && target != tossedSheep) {
                        SetState(BULL_ST_CHARGE);
                        w.foundGossamer = 1;
                        break;
                    }
                if (w.foundGossamer)
                    break;
            }
            w.found = 0;
            for (w.index = 0; w.index < movementBoxes.count; w.index++)
                if (InBox(movementBoxes.boxes[w.index], &target->pos) && target != tossedSheep) {
                    if (!target->GetClassId()) {
                        if (!target->HandleMessage(this, MSG_WOLF_IS_DEAD, 0)) {
                            SetState(BULL_ST_CHARGE);
                            w.found = 1;
                            break;
                        }
                    } else {
                        SetState(BULL_ST_CHARGE);
                        w.found = 1;
                        break;
                    }
                }
            if (!w.found && sheepBox) {
                /* cast kept: Box and CollBox are two views of one 16-byte record */
                w.objectCount = ObjGrid_QueryBoxPoints((const CollBox *)sheepBox, w.objects);
                for (w.objectIndex = 0; w.objectIndex < w.objectCount; w.objectIndex++)
                    if (w.objects[w.objectIndex]->GetClassId() == CLASSID_SHEEP) {
                        target = w.objects[w.objectIndex];
                        tossArc.obj = target;
                        tossedSheep = tossArc.obj;
                        tossArc.t = 0;
                        tossArc.InitCoefficients(sheepTraj->pts[0].x, sheepTraj->pts[1].x, sheepTraj->pts[2].x,
                                                 sheepTraj->pts[0].y, sheepTraj->pts[1].y, sheepTraj->pts[2].y,
                                                 sheepTraj->pts[0].z, sheepTraj->pts[1].z, sheepTraj->pts[2].z);
                        chargeTargetPos = target->pos;
                        state = BULL_ST_CHARGE;
                        if (sheepCam)
                            sheepCamShot.Start(sheepCam, this, 1);
                    }
            }
            break;
        case BULL_ST_HIT:
            if (target->GetClassId() == CLASSID_GOSSAMER_LEV08) {
                SetState(BULL_ST_CHARGE);
                break;
            }
            if (AnimFlags(ANIM_F_FINISHED))
                PlayAnim(ABULL01_ANIM_STAND2, 1, 1);
            if (g_gameTime - hitTime > 40960)
                SetState(BULL_ST_IDLE);
            break;
        case BULL_ST_SCARF_PAW:
            if (g_gameTime - stateTime < 8192) {
            } else
                SetState(BULL_ST_SCARF_CHARGE);
            break;
        case BULL_ST_SCARF_CHARGE:
            w.dx = chargeTargetPos.x - pos.x;
            w.dz = chargeTargetPos.z - pos.z;
            w.distance = (s32)sqrt((double)w.dx * w.dx + w.dz * w.dz);
            if (w.distance <= 150) {
                w.distance = Vec3s_Dist(&target->pos, &pos);
                if (w.distance <= 150) {
                    SetFacing((Math_RadiansToAngle4096(
                                   (float)atan2((double)target->pos.x - pos.x, (double)target->pos.z - pos.z)) +
                               0x800) &
                              0xfff);
                    SetState(BULL_ST_HIT);
                } else
                    SetState(BULL_ST_SKID);
            } else {
                velocity.x = w.dx * chargeSpeed / (s32)w.distance;
                velocity.z = w.dz * chargeSpeed / (s32)w.distance;
                velocity.y = 250;
                Vec3s_ScaleByDt(&velocity, &w.delta);
                w.endPos.x = pos.x + w.delta.x;
                w.endPos.y = pos.y + w.delta.y;
                w.endPos.z = pos.z + w.delta.z;
                Collide_ResolveMove(&w.delta, &w.contact, 0x578, RESOLVE_SLIDE_ALL, 0, 0, 10, 0, 0);
                w.movedCharge = 0;
                for (w.chargeIndex = 0; w.chargeIndex < movementBoxes.count; w.chargeIndex++)
                    if (InBox(movementBoxes.boxes[w.chargeIndex], &w.endPos)) {
                        Translate(&w.delta);
                        w.movedCharge = 1;
                        break;
                    }
                if (!w.movedCharge)
                    SetState(BULL_ST_RETURN_HOME);
            }
            break;
        case BULL_ST_RETURN_HOME:
            w.dx = homePos.x - pos.x;
            w.dz = homePos.z - pos.z;
            SetFacing((s16)(Math_RadiansToAngle4096((float)atan2(w.dx, w.dz)) & 0xfff) + 0x800);
            w.distance = (s32)sqrt((double)w.dx * w.dx + w.dz * w.dz);
            if (w.distance <= 25) {
                SetState(BULL_ST_ASLEEP);
                break;
            }
            velocity.x = w.dx * chargeSpeed / (s32)w.distance;
            velocity.z = w.dz * chargeSpeed / (s32)w.distance;
            velocity.y = 250;
            Vec3s_ScaleByDt(&velocity, &w.delta);
            Collide_ResolveMove(&w.delta, &w.contact, 0x578, RESOLVE_SLIDE_ALL, 0, 0, 10, 0, 0);
            if (w.contact.wallObj) {
                if (!w.contact.wallObj->GetClassId())
                    /* cast kept: HandleMessage's arg is a void *; MSG_KILL passes the kill type in it */
                    w.contact.wallObj->HandleMessage(this, MSG_KILL, (void *)KILL_CRUSH);
                else
                    w.contact.wallObj->HandleMessage(this, MSG_BUMP, 0);
            }
            w.endPos.x = pos.x + w.delta.x;
            w.endPos.y = pos.y + w.delta.y;
            w.endPos.z = pos.z + w.delta.z;
            Translate(&w.delta);
    }
    if (state == BULL_ST_CHARGE || state == BULL_ST_RETURN_HOME || state == BULL_ST_SKID ||
        state == BULL_ST_SCARF_PAW || state == BULL_ST_SCARF_CHARGE) {
        w.dustPos = pos;
        w.dustPos.y -= 25;
        dustEmitter.base.Emitter_UpdateDrift(&dustParams, &w.dustPos, Facing(), 1);
    }
    if (dustEmitter.base.flags.active)
        dustEmitter.base.Emitter_UpdateDrift(&dustParams, &pos, Facing(), 0);
    AdvanceAnim();
}

s32 bull::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    switch (msgId) {
        case MSG_TIMEMACHINE_CALL:
            SetState(BULL_ST_RETURN_HOME);
            return 1;
        case MSG_KILL:
            switch ((s32)arg) { /* cast kept: HandleMessage's arg is a void *; MSG_KILL carries the kill type in it */
                case KILL_CRUSH:
                    EnableBoxCollide(0);
                    SetState(BULL_ST_STUNNED);
                    return 1;
                case KILL_GENERIC:
                    tintColor = 0;
                    tintAmount = 4096;
                    EnableTint(1);
                    SetState(BULL_ST_STUNNED);
                    return 1;
                case KILL_FALL_TO_SENDER:
                    SetState(BULL_ST_STUNNED);
                    return 1;
            }
            return 0;
        case MSG_LOUD_NOISE:
            if (state == BULL_ST_ASLEEP && !woken) {
                anim.speed = (wakeUpSpeedPct * 4096) / 100;
                PlayAnim(ABULL01_ANIM_UP1A, 0, 1);
                woken = 1;
            }
            break;
        case MSG_QUERY_MOVED:
            if (state != BULL_ST_ASLEEP && state != BULL_ST_IDLE && state != BULL_ST_STUNNED)
                return 1;
            break;
        default:
            return 0;
    }
    return 0;
}

ScnObject *bull_Create(void *record)
{
    ScnBody *object = new bull;
    object = object->Init(record, 0);
    return object;
}
