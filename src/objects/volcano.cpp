/* T236 - original object Volcano.cpp (guessed name), one translation unit.
 * .text 0x503ac0-0x5061a4, .rdata 0x576f78-0x576f9c (Volcano's vtable); no .data/.bss.
 * Volcano::PostLoadInit 0x503ac0 and its inline LaunchArc helpers come first, then Update..Create. */
/* BYTES: inline, slot-name. */
/* PAL PC Volcano: embedded ScnBody records and LaunchArc storage. */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *, Animator *, u16, u32);
#include "animation.h"
#include "../engine/maths.h"
#include "../engine/scn_tools.h"
#include "../engine/scenaric.h"

#define SDW_MEMBERS_ScnObject       \
    static void *operator new(u32); \
    void SetRotation(Vec3s *value); \
    void SetPos(Vec3s *value);      \
    void SetUpdateMode(u8 mode);    \
    void SetUpdateMode(s32 mode);
#define SDW_MEMBERS_ScnBody                          \
    u32 AnimDuration(u16 id, u8 mode)                \
    {                                                \
        return Anim_GetDurationMs(Inst(), id, mode); \
    }

#define SDW_MEMBERS_CollBox                                                                                        \
    s32 Contains(const Vec3s *p)                                                                                   \
    {                                                                                                              \
        return p->x >= min.x && p->x <= max.x && p->y >= min.y && p->y <= max.y && p->z >= min.z && p->z <= max.z; \
    }
#define SDW_MEMBERS_LaunchArc s32 StepPoint(s32, Vec3s *);
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_SETROTATION_VEC3S 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETROTATION_VEC3S
#define SDW_INLINE_SCNOBJECT_SETPOS_VEC3S 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETPOS_VEC3S
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_SETVISIBLE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_SETVISIBLE_S32
#define SDW_INLINE_SCNOBJECT_SETCOLLISION_S32 1
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETCOLLISION_S32
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
extern s32 g_gameTimeMs;
extern s32 g_dtMs;
extern Wolf *g_pWolf;
s32 ObjGrid_QueryBoxOverlap(CollBox *, ScnObject **);
#include "../sdk/crt.h"
extern "C" s16 Math_RadiansToAngle4096(float);
#define ABS_VALUE(v) ((v) >= 0 ? (v) : -(v))
/* The StepPoint adapter records the observed inline LaunchArc expansion; it calls the out-of-line Step. */
#define SDW_INLINE_FREE_GETPROP_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_GETPROP_VOID_U32
#define SDW_INLINE_LAUNCHARC_INITCOEFFICIENTS_S32_S32_S32_S32_S32_S32_S32_S32_S32 1
#include "../engine/launch_arc_inlines.h"
#undef SDW_INLINE_LAUNCHARC_INITCOEFFICIENTS_S32_S32_S32_S32_S32_S32_S32_S32_S32
/* BYTES(inline): source-only adapter: records the original's inline LaunchArc expansion around the out-of-line Step */
inline s32 LaunchArc::StepPoint(s32 dt, Vec3s *out)
{
    s32 result;
    s16 ignored;
    if (flags.full3D)
        result = Step(dt, &out->x, &out->y, &out->z);
    else if (flags.planeYZ) {
        result = Step(dt, &out->z, &out->y, &ignored);
        out->x = obj->pos.x;
    } else {
        result = Step(dt, &out->x, &out->y, &ignored);
        out->z = obj->pos.z;
    }
    return result;
}
/* BYTES(slot-name, inferred): local names chosen for their stack slots (tools/vc6_locals.py), not recovered */
void Volcano::PostLoadInit()
{
    u16 time;
    void *recordData = record;
    vertical = GetProp(recordData, 8);
    homePos = pos;
    /* cast kept: Scn_BuildRecordFromExport fills any synthesised record through a u16 * */
    lavaModelRes = Scn_BuildRecordFromExport(WAR_IDO_ALAVE02, (u16 *)&lavaRecord, 0, 0);
    lavaBody.Init(&lavaRecord, 0);
    if (!vertical) {
        u16 segmentIndex, ends[4];
        Trajectory *points;
        u8 clearIndex;
        s32 steps[4];
        ends[0] = 3;
        ends[1] = 44;
        ends[2] = 84;
        ends[3] = 128;
        steps[0] = 1;
        steps[1] = 4;
        steps[2] = 3;
        steps[3] = 4;
        segmentIndex = 0;
        time = 0;
        loopIndex = 0;
        for (clearIndex = 0; clearIndex < 128; clearIndex++) {
            arcPoints[clearIndex].x = 0;
            arcPoints[clearIndex].y = 0;
            arcPoints[clearIndex].z = 0;
        }
        uTurn = GetProp(recordData, 4);
        reverseNext = 0;
        points = Scn_GetPropTrajectory(recordData, 0);
        arc.InitCoefficients(points->pts[0].x, points->pts[1].x, points->pts[2].x, points->pts[0].y, points->pts[1].y,
                             points->pts[2].y, points->pts[0].z, points->pts[1].z, points->pts[2].z);
        arc.t = 0;
        arc.camera = 0;
        arc.camParam = 0;
        arc.flags.noFreeze = 0;
        arc.obj = 0;
        arc.flags.planeYZ = 1;
        arc.flags.full3D = 1;
        do {
            if (time >= ends[segmentIndex])
                segmentIndex++;
            if (!arc.StepPoint(steps[segmentIndex] * 256 / 128, &arcPoints[loopIndex]))
                time = 257;
            time += (u16)steps[segmentIndex];
            loopIndex++;
        } while (time < 256);
        lastArcIndex = loopIndex - 2;
        /* cast kept: Scn_BuildRecordFromExport fills any synthesised record through a u16 * */
        blobModelRes = Scn_BuildRecordFromExport(WAR_IDO_ALAVE03, (u16 *)&blobRecord, 0, 0);
        for (loopIndex = 0; loopIndex < 5; loopIndex++)
            blobs[loopIndex].Init(&blobRecord, 0);
        hitBox.flags = COLLBOX_NONSOLID;
        hitBox.min.x = pos.x - 15;
        hitBox.min.y = pos.y - 15;
        hitBox.min.z = pos.z - 15;
        hitBox.max.x = pos.x + 15;
        hitBox.max.y = pos.y + 15;
        hitBox.max.z = pos.z + 15;
    } else {
        hitBox.flags = COLLBOX_NONSOLID;
        hitBox.min.x = pos.x - 15;
        hitBox.min.y = pos.y - 380;
        hitBox.min.z = pos.z - 15;
        hitBox.max.x = pos.x + 15;
        hitBox.max.y = pos.y;
        hitBox.max.z = pos.z + 15;
        timerMs = Anim_GetDurationMs(lavaBody.Inst(), ALAVE02_ANIM_SPLASH, 0);
        arcIndex = 380;
        /* The original repeatedly clears entry zero in this vertical-only loop. */
        for (loopIndex = 0; loopIndex < 128; loopIndex++) {
            arcPoints[0].x = 0;
            arcPoints[0].y = 0;
            arcPoints[0].z = 0;
        }
        for (loopIndex = 0; loopIndex < 5; loopIndex++)
            blobActive[loopIndex] = 0;
        blobModelRes = 0;
        headVisible = 0;
    }
    SetState(VOLC_ST_DORMANT);
}
void Volcano::Update()
{
    ScnObject *nearObjects[64];
    u8 index;
    Vec3s movement;
    switch (state) {
        case VOLC_ST_DORMANT:
            if (timerMs <= 0) {
                SetUpdateMode(SCN_UPD_ALWAYS);
                if (vertical)
                    SetState(VOLC_ST_VERTICAL);
                else if (reverseNext)
                    SetState(VOLC_ST_ERUPT_REVERSE);
                else {
                    for (index = 0; index < ObjGrid_QueryBoxOverlap(&hitBox, nearObjects); index++)
                        if (nearObjects[index]->GetClassId() == CLASSID_ROCK) {
                            SetState(VOLC_ST_DORMANT);
                            return;
                        }
                    SetState(VOLC_ST_ERUPT);
                }
                break;
            }
            timerMs -= g_dtMs;
            break;
        case VOLC_ST_VERTICAL:
            if (lavaBody.AnimFlags(ANIM_F_FINISHED)) {
                SetState(VOLC_ST_DORMANT);
                timerMs = 0;
            }
            lavaBody.AdvanceAnim();
            if ((u32)timerMs >= (lavaBody.AnimDuration(ALAVE02_ANIM_SPLASH, 0) >> 1)) {
                hitBox.min.y = hitBox.max.y - (380 - (timerMs - (lavaBody.AnimDuration(ALAVE02_ANIM_SPLASH, 0) >> 1)) *
                                                         380 / (lavaBody.AnimDuration(ALAVE02_ANIM_SPLASH, 0) >> 1));
                if (hitBox.min.y < hitBox.max.y - 380)
                    hitBox.min.y = hitBox.max.y - 380;
                timerMs -= g_dtMs * 3 / 2;
            } else if (timerMs > 0) {
                hitBox.min.y = hitBox.max.y - timerMs * 380 / (lavaBody.AnimDuration(ALAVE02_ANIM_SPLASH, 0) >> 1);
                timerMs -= g_dtMs;
            } else
                hitBox.min.y = hitBox.max.y;
            if (hitBox.Contains(&g_pWolf->pos) && !g_pWolf->HandleMessage(this, MSG_WOLF_IS_DEAD, 0))
                /* cast kept: HandleMessage's arg is a void *; MSG_KILL passes the kill type in it */
                g_pWolf->HandleMessage(this, MSG_KILL, (void *)KILL_BURN_RUN);
            break;
        case VOLC_ST_ERUPT: {
            ContactInfo contact;
            if (lavaFxActive) {
                lavaBody.AdvanceAnim();
                if (Vec3s_DistSq(&lavaBody.pos, &g_pWolf->pos) < 3600)
                    /* cast kept: HandleMessage's arg is a void *; MSG_KILL passes the kill type in it */
                    g_pWolf->HandleMessage(this, MSG_KILL, (void *)KILL_BURN_RUN);
            }
            if (lavaBody.AnimFlags(ANIM_F_FINISHED)) {
                lavaFxActive = 0;
                if (arcIndex >= lastArcIndex + 10 || splashDone) {
                    if (uTurn)
                        reverseNext = 1;
                    SetState(VOLC_ST_DORMANT);
                    break;
                }
            }
            arcIndex = (g_gameTimeMs - startTimeMs) / 25;
            for (loopIndex = 0; loopIndex < 5; loopIndex++) {
                if (arcIndex - loopIndex * 2 > 0) {
                    if (arcIndex - loopIndex * 2 < trailEndIndex) {
                        blobs[loopIndex].SetPos(&arcPoints[arcIndex - loopIndex * 2]);
                        blobDelta[loopIndex].x =
                            arcPoints[arcIndex - loopIndex * 2].x - arcPoints[arcIndex - loopIndex * 2 - 1].x;
                        blobDelta[loopIndex].y =
                            arcPoints[arcIndex - loopIndex * 2].y - arcPoints[arcIndex - loopIndex * 2 - 1].y;
                        blobDelta[loopIndex].z =
                            arcPoints[arcIndex - loopIndex * 2].z - arcPoints[arcIndex - loopIndex * 2 - 1].z;
                        blobVel[loopIndex].x = blobDelta[loopIndex].x * 50;
                        blobVel[loopIndex].y = blobDelta[loopIndex].y * 50;
                        blobVel[loopIndex].z = blobDelta[loopIndex].z * 50;
                        blobActive[loopIndex] = 1;
                    } else {
                        blobs[loopIndex].SetPos(&arcPoints[0]);
                        blobActive[loopIndex] = 0;
                    }
                } else {
                    blobs[loopIndex].SetPos(&arcPoints[0]);
                    blobActive[loopIndex] = 0;
                }
            }
            if (arcIndex >= lastArcIndex && !splashDone) {
                lavaBody.SetPos(&arcPoints[lastArcIndex]);
                lavaBody.PlayAnim(ALAVE02_ANIM_DOWN, 0, 0);
                lavaFxActive = 1;
                landed = 1;
                headVisible = 0;
                splashDone = 1;
            }
            if (arcIndex >= lastArcIndex - 1)
                SetPosition(&arcPoints[0]);
            else {
                SetPosition(&arcPoints[arcIndex]);
                if (arcIndex - 1 >= 0) {
                    movement.x = arcPoints[arcIndex].x - arcPoints[arcIndex - 1].x;
                    movement.y = arcPoints[arcIndex].y - arcPoints[arcIndex - 1].y;
                    movement.z = arcPoints[arcIndex].z - arcPoints[arcIndex - 1].z;
                } else {
                    movement.x = 0;
                    movement.y = 0;
                    movement.z = 0;
                }
            }
            if (hitPending) {
                splashDone = 1;
                trailEndIndex = arcIndex;
                landed = 1;
                headVisible = 0;
                lavaFxActive = 1;
                lavaBody.SetPos(&arcPoints[arcIndex]);
                lavaBody.PlayAnim(ALAVE02_ANIM_DOWN, 0, 0);
                hitPending = 0;
            }
            if (!landed && Collide_ResolveMove(&movement, &contact, 0xb54, RESOLVE_SLIDE_ALL, 0, 0, 10, 0, 0)) {
                if (contact.movableObj && contact.movableObj->GetClassId() == CLASSID_WOLF) {
                    /* cast kept: HandleMessage's arg is a void *; MSG_KILL passes the kill type in it */
                    contact.movableObj->HandleMessage(this, MSG_KILL, (void *)KILL_BURN_RUN);
                    break;
                } else {
                    hitPending = 1;
                    if (contact.wallNormalMean.x || contact.wallNormalMean.y || contact.wallNormalMean.z) {
                        Vec3s orientation;
                        s32 xSquared, ySquared, nzSquared;
                        xSquared = contact.wallNormalMean.x * contact.wallNormalMean.x;
                        ySquared = contact.wallNormalMean.y * contact.wallNormalMean.y;
                        nzSquared = contact.wallNormalMean.z * contact.wallNormalMean.z;
                        orientation.x = (Math_RadiansToAngle4096((float)atan2(
                                             contact.wallNormalMean.y, (s32)sqrt((double)xSquared + nzSquared))) -
                                         0x400) &
                                        0xfff;
                        orientation.y =
                            Math_RadiansToAngle4096((float)atan2(contact.wallNormalMean.x, contact.wallNormalMean.z)) &
                            0xfff;
                        orientation.z = 0;
                        lavaBody.SetRotation(&orientation);
                    }
                }
            }
            break;
        }
        case VOLC_ST_ERUPT_REVERSE: {
            ContactInfo contact;
            if (lavaBody.AnimFlags(ANIM_F_FINISHED)) {
                lavaFxActive = 0;
                if (arcIndex <= -10) {
                    reverseNext = 0;
                    SetState(VOLC_ST_DORMANT);
                    break;
                }
            }
            if (lavaFxActive) {
                lavaBody.AdvanceAnim();
                if (Vec3s_DistSq(&lavaBody.pos, &g_pWolf->pos) < 3600)
                    /* cast kept: HandleMessage's arg is a void *; MSG_KILL passes the kill type in it */
                    g_pWolf->HandleMessage(this, MSG_KILL, (void *)KILL_BURN_RUN);
            }
            arcIndex = lastArcIndex - (g_gameTimeMs - startTimeMs) / 25 - 1;
            for (loopIndex = 0; loopIndex < 5; loopIndex++) {
                if (arcIndex + loopIndex * 2 < lastArcIndex) {
                    if (arcIndex + loopIndex * 2 > 0) {
                        blobs[loopIndex].SetPos(&arcPoints[arcIndex + loopIndex * 2]);
                        blobDelta[loopIndex].x =
                            arcPoints[arcIndex + loopIndex * 2].x - arcPoints[arcIndex + loopIndex * 2 + 1].x;
                        blobDelta[loopIndex].y =
                            arcPoints[arcIndex + loopIndex * 2].y - arcPoints[arcIndex + loopIndex * 2 + 1].y;
                        blobDelta[loopIndex].z =
                            arcPoints[arcIndex + loopIndex * 2].z - arcPoints[arcIndex + loopIndex * 2 + 1].z;
                        blobVel[loopIndex].x = blobDelta[loopIndex].x * 50;
                        blobVel[loopIndex].y = blobDelta[loopIndex].y * 50;
                        blobVel[loopIndex].z = blobDelta[loopIndex].z * 50;
                        blobActive[loopIndex] = 1;
                    } else {
                        blobs[loopIndex].SetPos(&arcPoints[0]);
                        blobActive[loopIndex] = 0;
                    }
                } else {
                    blobs[loopIndex].SetPos(&arcPoints[0]);
                    blobActive[loopIndex] = 0;
                }
            }
            if (arcIndex == 0) {
                lavaBody.SetPos(&arcPoints[0]);
                lavaBody.PlayAnim(ALAVE02_ANIM_DOWN, 0, 0);
                lavaFxActive = 1;
                landed = 1;
                headVisible = 0;
            }
            if (arcIndex <= 0)
                SetPosition(&arcPoints[0]);
            else {
                SetPosition(&arcPoints[arcIndex]);
                if (arcIndex + 1 <= lastArcIndex) {
                    movement.x = arcPoints[arcIndex].x - arcPoints[arcIndex + 1].x;
                    movement.y = arcPoints[arcIndex].y - arcPoints[arcIndex + 1].y;
                    movement.z = arcPoints[arcIndex].z - arcPoints[arcIndex + 1].z;
                } else {
                    movement.x = 0;
                    movement.y = 0;
                    movement.z = 0;
                }
            }
            if (!landed && Collide_ResolveMove(&movement, &contact, 0xb54, COLL_10, 0, 0, 10, 0, 0) &&
                contact.movableObj)
                /* cast kept: HandleMessage's arg is a void *; MSG_KILL passes the kill type in it */
                contact.movableObj->HandleMessage(this, MSG_KILL, (void *)KILL_BURN_RUN);
            break;
        }
    }
}
s32 Volcano::HandleMessage(ScnObject *, u32, void *)
{
    return 0;
}
void Volcano::SetState(u8 value)
{
    state = value;
    switch (state) {
        case VOLC_ST_DORMANT:
            SetPosition(&homePos);
            lavaBody.SetRotation(g_pZeroVec3s);
            SetUpdateMode((u8)SCN_UPD_NORMAL);
            if (reverseNext)
                timerMs = 500;
            else
                timerMs = Rand_Range(1500, 4500);
            lavaFxActive = 0;
            SetVisible(0);
            SetCollision(0);
            break;
        case VOLC_ST_VERTICAL:
            lavaBody.PlayAnim(ALAVE02_ANIM_SPLASH, 0, 0);
            lavaBody.SetPos(&pos);
            SetVisible(1);
            SetCollision(1);
            headVisible = 1;
            lavaFxActive = 1;
            timerMs = Anim_GetDurationMs(lavaBody.Inst(), ALAVE02_ANIM_SPLASH, 0);
            break;
        case VOLC_ST_ERUPT:
            lavaBody.PlayAnim(ALAVE02_ANIM_UP, 0, 0);
            lavaBody.SetPos(&arcPoints[0]);
            SetVisible(1);
            SetCollision(1);
            lavaFxActive = 1;
            headVisible = 1;
            landed = 0;
            splashDone = 0;
            trailEndIndex = lastArcIndex;
            arcIndex = 0;
            startTimeMs = g_gameTimeMs;
            hitPending = 0;
            break;
        case VOLC_ST_ERUPT_REVERSE:
            lavaBody.PlayAnim(ALAVE02_ANIM_UP, 0, 0);
            lavaBody.SetPos(&arcPoints[lastArcIndex]);
            SetVisible(1);
            lavaFxActive = 1;
            headVisible = 1;
            landed = 0;
            arcIndex = lastArcIndex;
            startTimeMs = g_gameTimeMs;
            break;
    }
}
void Volcano::Render(Camera *view)
{
    s32 x, yStep, dz;
    u16 i;
    if (lavaModelRes && lavaFxActive)
        lavaBody.Render(view);
    for (i = 0; i < 5; i++)
        if (blobActive[i]) {
            s32 xSquared, ySquared, squaredZ;
            Vec3s orientation, stretch;
            x = blobDelta[i].x;
            yStep = blobDelta[i].y;
            dz = blobDelta[i].z;
            xSquared = x * x;
            ySquared = yStep * yStep;
            squaredZ = dz * dz;
            orientation.x =
                (Math_RadiansToAngle4096((float)atan2(yStep, (s32)sqrt((double)xSquared + squaredZ))) + 0x400) & 0xfff;
            orientation.y = (Math_RadiansToAngle4096((float)atan2(x, dz)) + 0x800) & 0xfff;
            orientation.z = 0;
            blobs[i].rot = orientation;
            stretch.x = 2000 - ABS_VALUE(blobVel[i].y) - i * 35;
            stretch.y = 1024 + ABS_VALUE(blobVel[i].y) - i * 35;
            stretch.z = 2000 - ABS_VALUE(blobVel[i].y) - i * 35;
            blobs[i].RenderScaled(view, &stretch);
        }
    if (headVisible && pos.y < lavaBody.pos.y)
        ScnBody::Render(view);
}
ScnObject *Volcano_Create(void *record)
{
    Volcano *object = new Volcano;
    object = (Volcano *)object->Init(record, 0); /* cast kept: Init returns the ScnBody * base of this object */
    return object;
}
