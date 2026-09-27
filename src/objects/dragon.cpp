/* T077 - original object Dragon.cpp (guessed name): .text 0x444310-0x4486bd, then eight COMDATs 0x4486c0-0x448a93
 * (Box::ContainsPointXZ, Dragon::IsIdleVariant / IsFlying / IsLanding / IsTakingOff / IsFiring / SetBoxCollision /
 * PlayAnim); .rdata 0x574e58-0x574e7c (vtable), .data 0x57a818-0x57a824 (the cinematic header's stride-table copy).
 * The 25 main functions in their original order, then the eight helpers as inline members defined after all their
 * callers (ChooseState, SetState), which VC6 emits as COMDATs after the main .text.
 *
 * INLINE BUDGET: VC6 /Ob1 does not expand every inline call. Each caller has a size budget that grows with the caller's
 * own size. Call sites are taken in source order and a callee is expanded only if its estimated size still fits. Call
 * sites exposed inside an expanded inline are considered after the direct ones. A refused site stays a call, and the
 * callee is emitted as a COMDAT. That is how the original has out-of-line copies of IsTakingOff / IsLanding / IsFlying /
 * IsFiring / IsIdleVariant / PlayAnim / SetBoxCollision while ChooseState and SetState also expand inline helpers.
 * With the plain definitions, VC6 would expand some of the nested IsTakingOff / IsLanding sites in ChooseState, and
 * Dragon::PlayAnim at SetState case 35 (so SetVisible would no longer fit and would be emitted out of line). The devices
 * below change no generated instruction (every function byte-matches); they only change the inliner's estimates. They
 * were found by search and are representations, not the original text:
 *   - FlyingWithCalls: its body is wrapped in 12 empty blocks (11..14 all match);
 *   - Dragon::IsTakingOff: 1 extra block, Dragon::IsLanding: 6 extra blocks (their COMDAT code is unchanged);
 *   - StartCine: an unused defaulted third parameter;
 *   - SetState: 17 empty blocks at its start (14..20 all match).
 * PAL PC Dragon. */
/* BYTES: inline, layout, slot-group, slot-scope. */
/* BYTES(layout): the Cine.h header static: every object including the cinematic header carries this copy in its .data, referenced or not */
/* BYTES(inline): Dragon::FlyingWithCalls: the 12 empty blocks only raise /Ob1's cost estimate (11..14 all match); they emit nothing */
/* BYTES(inline): Dragon::StartCine: the unused defaulted parameter only changes /Ob1's estimate for SetState; no caller passes it */
/* BYTES(inline): ScnBody::PlayAnim (member inline): the trailing return only changes /Ob1's estimate; it emits nothing */
/* BYTES(layout): the eight helpers as inline members defined after all their callers: defined last, in this order: they are emitted as COMDATs after the main .text in definition order */

#define SDW_MEMBERS_Box s32 ContainsPointXZ(Vec3s *);
#include "sdw_types.h"
#include "sdw_enums.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *, Animator *, u16, u32), Anim_GetDurationMs(Instance *, u16, u8);
#define SDW_MEMBERS_ScnObject       \
    static void *operator new(u32); \
    void SetUpdateMode(s32 mode);

#define SDW_MEMBERS_InlineEmitter16 InlineEmitter16();
#define SDW_MEMBERS_Cine     \
    s32 Active()             \
    {                        \
        s32 result = active; \
        return result;       \
    }
#define SDW_MEMBERS_Dragon     \
    s32 IsIdleVariant();       \
    s32 IsFlying();            \
    s32 IsLanding();           \
    s32 IsTakingOff();         \
    s32 IsFiring();            \
    void SetBoxCollision(s32); \
    void PlayAnim(u16, s32, s32);
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_FACING 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_FACING
#define SDW_INLINE_SCNOBJECT_ENABLEBOXCOLLIDE_S32 1
#define SDW_INLINE_SCNOBJECT_SETVISIBLE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_ENABLEBOXCOLLIDE_S32
#undef SDW_INLINE_SCNOBJECT_SETVISIBLE_S32
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
/* 0x57a818 - the cinematic header's static copy of the 9-byte opcode stride table (src/engine/cine.cpp,
 * g_cineOpStride 0x5816fc). The original header defined it static, so every object including it carries its own
 * unreferenced copy at the head of its .data; defined here in its place (the map calls the slot a guess; the bytes are
 * the same whichever includer holds it). */
static u8 s_cineOpStride[9] = {0, 8, 8, 4, 2, 2, 4, 2, 2};
extern Wolf *g_pWolf;
#include "../engine/cine.h"
#include "../engine/scn_tools.h"
extern s32 g_dtMs;
#include "../sdk/crt.h"
extern "C" s16 Math_RadiansToAngle4096(float);
s32 Rand_Bounded(s32), TimeMachine_IsInPresent(ScnObject *);
s32 Vec3s_DistXZ(Vec3s *, Vec3s *), Vec3s_Dist(Vec3s *, Vec3s *);
inline InlineEmitter16::InlineEmitter16()
{
    base.slotPool = slotBuf;
    base.particles = particleBuf;
    base.count = 16;
    base.Emitter_Reset();
}
inline s32 TakingOff(Dragon *d)
{
    return d->state == DRAGON_ST_TAKEOFF || d->state == DRAGON_ST_RISE || d->state == DRAGON_ST_TAKEOFF_TO_SPHERE ||
           d->state == DRAGON_ST_RISE_TO_SPHERE;
}
inline s32 Landing(Dragon *d)
{
    return d->state == DRAGON_ST_LAND || d->state == DRAGON_ST_DESCEND || d->state == DRAGON_ST_TOUCHDOWN;
}
inline s32 Flying(Dragon *d)
{
    return TakingOff(d) || d->state == DRAGON_ST_FLY_CHASE || Landing(d) || d->state == DRAGON_ST_FLY_FIRE ||
           d->state == DRAGON_ST_FLY_INHALE || d->state == DRAGON_ST_FLY_TO_ROCKZONE ||
           d->state == DRAGON_ST_FLY_WAIT || d->state == DRAGON_ST_FLY_BACK_AWAY ||
           d->state == DRAGON_ST_FLY_TO_SPHERE || d->state == DRAGON_ST_FLY_TIMEWARP ||
           d->state == DRAGON_ST_FLY_WAIT_PRESENT;
}
/* FlyingWithCalls: its body is wrapped in 12 empty blocks. That is a device, not a claim about the source: see the
 * INLINE BUDGET note in the header. */
/* BYTES(slot-scope, inferred): the nested block(s) only order the frame: their locals are allocated after the enclosing scope's */
inline s32 FlyingWithCalls(Dragon *d)
{
    {
        {
            {
                {
                    {
                        {
                            {
                                {
                                    {
                                        {
                                            {
                                                {
                                                    return d->IsTakingOff() || d->state == DRAGON_ST_FLY_CHASE ||
                                                           d->IsLanding() || d->state == DRAGON_ST_FLY_FIRE ||
                                                           d->state == DRAGON_ST_FLY_INHALE ||
                                                           d->state == DRAGON_ST_FLY_TO_ROCKZONE ||
                                                           d->state == DRAGON_ST_FLY_WAIT ||
                                                           d->state == DRAGON_ST_FLY_BACK_AWAY ||
                                                           d->state == DRAGON_ST_FLY_TO_SPHERE ||
                                                           d->state == DRAGON_ST_FLY_TIMEWARP ||
                                                           d->state == DRAGON_ST_FLY_WAIT_PRESENT;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
inline s32 Skidding(Dragon *d)
{
    return d->state == DRAGON_ST_SKID1 || d->state == DRAGON_ST_SKID2 || d->state == DRAGON_ST_SKID3;
}
inline s32 InTimeMachineSequence(Dragon *d)
{
    return d->state == DRAGON_ST_WALK_TO_SPHERE || d->state == DRAGON_ST_TIMEWARP || d->state == DRAGON_ST_ARRIVE ||
           d->state == DRAGON_ST_CINE_CHRONOBALL || d->state == DRAGON_ST_IN_TRANSIT ||
           d->state == DRAGON_ST_FLY_TO_SPHERE || d->state == DRAGON_ST_FLY_TIMEWARP ||
           d->state == DRAGON_ST_TAKEOFF_TO_SPHERE || d->state == DRAGON_ST_RISE_TO_SPHERE ||
           d->state == DRAGON_ST_WAIT_PRESENT || d->state == DRAGON_ST_FLY_WAIT_PRESENT;
}
inline s32 Firing(Dragon *d)
{
    return d->state == DRAGON_ST_INHALE || d->state == DRAGON_ST_FIRE || d->state == DRAGON_ST_FLY_FIRE ||
           d->state == DRAGON_ST_FLY_INHALE;
}
inline s32 Idle(Dragon *d)
{
    return d->state == DRAGON_ST_IDLE_PICK || d->state == DRAGON_ST_IDLE_A1 || d->state == DRAGON_ST_IDLE_A2 ||
           d->state == DRAGON_ST_IDLE_A3 || d->state == DRAGON_ST_IDLE_B;
}
#define SDW_INLINE_FREE_INBOXXZ_BOX_VEC3S 1
#include "../engine/coll_box_inlines.h"
#undef SDW_INLINE_FREE_INBOXXZ_BOX_VEC3S
#define SDW_INLINE_FREE_INBOX_BOX_VEC3S 1
#include "../engine/coll_box_inlines.h"
#undef SDW_INLINE_FREE_INBOX_BOX_VEC3S
/* `unused`: an inline-budget device for SetState (header). */
inline void StartCine(u32 id, u32 flags, u32 unused = 0)
{
    g_cinePlayer.Start(id, flags, 0, 0, 0, 0);
}

/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets */
void Dragon::PostLoadInit()
{
    struct Work {
        u32 secondFlagOffset, secondCineValue, secondCineOffset, firstFlagOffset, firstCineValue, firstCineOffset,
            flagValue;
        void *properties;
    } w;
    w.properties = record;
    runLatch = 0;
    boxFirstContact = Scn_GetPropBox(w.properties, 0);
    w.firstCineOffset = 0x1c;
    /* cast kept: each designer property below is a 4-byte slot at a byte offset of the raw WAR record */
    w.firstCineValue = *(u32 *)((u8 *)w.properties + w.firstCineOffset + 0x14);
    cineFirstContact = (u16)w.firstCineValue;
    w.firstFlagOffset = 0x24;
    /* cast kept: a designer-property record read at its byte offset */
    w.flagValue = *(u32 *)((u8 *)w.properties + w.firstFlagOffset + 0x14);
    flagFirstContact = w.flagValue;
    w.secondCineOffset = 0x18;
    w.secondCineValue = *(u32 *)((u8 *)w.properties + w.secondCineOffset + 0x14);
    cineChronoBall = (u16)w.secondCineValue;
    w.secondFlagOffset = 0x20;
    w.flagValue = *(u32 *)((u8 *)w.properties + w.secondFlagOffset + 0x14);
    flagChronoBall = w.flagValue;
    flyBoxes[0] = Scn_GetPropBox(w.properties, 4);
    flyBoxes[1] = Scn_GetPropBox(w.properties, 8);
    flyBoxes[2] = Scn_GetPropBox(w.properties, 12);
    flyBoxes[3] = Scn_GetPropBox(w.properties, 16);
    flyBoxes[4] = Scn_GetPropBox(w.properties, 20);
    rockZone = Scn_GetPropBox(w.properties, 40);
    zone = Scn_GetPropBox(w.properties, 44);
    dustParams.hSpeed = 100;
    dustParams.vSpeed = -20;
    dustParams.life = 0xa30;
    dustParams.spawnInterval = 0x146;
    dustParams.sizeStart = 40;
    dustParams.sizeEnd = 120;
    dustParams.sheetIndex = 0;
    riseAnimMs = Anim_GetDurationMs(Inst(), ADRAGO01_ANIM_TAKEOFF, 0);
    descendAnimMs = Anim_GetDurationMs(Inst(), ADRAGO01_ANIM_LAND0, 0);
    landTimerMs = -1;
    rockZoneTarget.x = (rockZone->max[0] - rockZone->min[0]) / 2;
    rockZoneTarget.y = 0;
    rockZoneTarget.z = (rockZone->max[2] - rockZone->min[2]) / 2;
    rockZoneTarget.x += rockZone->min[0];
    rockZoneTarget.y += rockZone->min[1];
    rockZoneTarget.z += rockZone->min[2];
    SnapToGround(1);
    SetUpdateMode(SCN_UPD_ALWAYS);
    SetUpdateMode(SCN_UPD_CINE);
    state = DRAGON_ST_NONE;
    SetState(DRAGON_ST_WAIT_FIRST_CONTACT);
}
/* BYTES(slot-scope, inferred): the nested block(s) only order the frame: their locals are allocated after the enclosing scope's */
void Dragon::Update()
{
    if (state == DRAGON_ST_GONE)
        return;
    {
        wolfDistXZ = Vec3s_DistXZ(&pos, &g_pWolf->pos);
        ChooseState();
        UpdateState();
        UpdateDust();
        AdvanceAnim();
    }
}
void Dragon::ChooseState()
{
    if (state == DRAGON_ST_WAIT_FIRST_CONTACT || state == DRAGON_ST_CINE_FIRST_CONTACT ||
        state == DRAGON_ST_CINE_CHRONOBALL)
        return;
    if (Skidding(this))
        return;
    if (Landing(this) || TakingOff(this))
        return;
    if (InTimeMachineSequence(this))
        return;
    if (state == DRAGON_ST_FLY_WAIT) {
        if (InBoxXZ(zone, &g_pWolf->pos))
            SetState(DRAGON_ST_FLY_CHASE);
        return;
    }
    if (state == DRAGON_ST_FLY_TO_ROCKZONE && InBoxXZ(rockZone, &pos)) {
        flyToRockZone = 0;
        SetState(DRAGON_ST_FLY_CHASE);
        return;
    }
    if (state == DRAGON_ST_FLY_TO_ROCKZONE)
        return;
    if (CheckWolfInRockZone())
        return;
    if (ShouldFly() && !FlyingWithCalls(this)) {
        flyToRockZone = 0;
        SetState(DRAGON_ST_TAKEOFF);
        return;
    }
    if (!ShouldFly() && FlyingWithCalls(this) && !Landing(this)) {
        if (landTimerMs == -1)
            landTimerMs = 5000;
        landTimerMs -= g_dtMs;
        if (landTimerMs < 0) {
            flyToRockZone = 0;
            SetState(DRAGON_ST_LAND);
            landTimerMs = -1;
            return;
        }
    }
    if (wolfDistXZ < 300 && !Firing(this) && !g_pWolf->HandleMessage(this, MSG_WOLF_IS_RUNNING, 0) &&
        !g_pWolf->HandleMessage(this, MSG_WOLF_IS_DEAD, 0) && !InTimeMachineSequence(this)) {
        if (FlyingWithCalls(this))
            SetState(DRAGON_ST_FLY_BACK_AWAY);
        else
            SetState(DRAGON_ST_BACK_AWAY);
        return;
    }
    if (wolfDistXZ < 350 && !Firing(this) && !FlyingWithCalls(this) &&
        !g_pWolf->HandleMessage(this, MSG_WOLF_IS_DEAD, 0) && !InTimeMachineSequence(this)) {
        if (state == DRAGON_ST_CHASE && !AnimFlags(ANIM_F_FINISHED))
            return;
        SetState(DRAGON_ST_INHALE);
        return;
    }
    if (IsWolfNear() && !Firing(this) && state == DRAGON_ST_FLY_CHASE &&
        !g_pWolf->HandleMessage(this, MSG_WOLF_IS_DEAD, 0) && !InTimeMachineSequence(this)) {
        SetState(DRAGON_ST_FLY_INHALE);
        return;
    }
    if (!InBox(zone, &g_pWolf->pos) && !Idle(this)) {
        if (FlyingWithCalls(this) && !IsLanding() && !IsTakingOff() && !rockZone->ContainsPointXZ(&pos)) {
            flyToRockZone = 1;
            if (!IsFlying())
                SetState(DRAGON_ST_TAKEOFF);
            else if (state == DRAGON_ST_FLY_CHASE)
                SetState(DRAGON_ST_FLY_TO_ROCKZONE);
        } else {
            if (IsFlying())
                SetState(DRAGON_ST_FLY_WAIT);
            else
                SetState(DRAGON_ST_IDLE);
        }
        return;
    }
    if (IsFlying() || IsFiring())
        return;
    if (wolfDistXZ < 1280)
        SetState(DRAGON_ST_CHASE);
    else if (wolfDistXZ >= 1280 && !IsIdleVariant())
        SetState(DRAGON_ST_IDLE_PICK);
}
void Dragon::UpdateState()
{
    switch (state) {
        case DRAGON_ST_IDLE:
        case DRAGON_ST_FLY_WAIT:
            FaceTarget(&g_pWolf->pos);
            break;
        case DRAGON_ST_WAIT_FIRST_CONTACT:
            FaceTarget(&g_pWolf->pos);
            if (InBoxXZ(boxFirstContact, &g_pWolf->pos))
                SetState(DRAGON_ST_CINE_FIRST_CONTACT);
            break;
        case DRAGON_ST_CINE_FIRST_CONTACT:
            if (!g_cinePlayer.Active()) {
                SnapToGround(1);
                SetState(DRAGON_ST_IDLE);
            }
            break;
        case DRAGON_ST_INHALE:
            FaceTarget(&g_pWolf->pos);
            if (AnimFlags(ANIM_F_FINISHED)) {
                if (IsWolfInFireReach())
                    SetState(DRAGON_ST_FIRE);
                else
                    SetState(DRAGON_ST_IDLE);
            }
            break;
        case DRAGON_ST_FIRE:
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(DRAGON_ST_IDLE);
            break;
        case DRAGON_ST_FLY_INHALE:
            FaceTarget(&g_pWolf->pos);
            if (AnimFlags(ANIM_F_FINISHED)) {
                if (IsWolfInFireReach())
                    SetState(DRAGON_ST_FLY_FIRE);
                else
                    SetState(DRAGON_ST_FLY_CHASE);
            }
            break;
        case DRAGON_ST_FLY_FIRE:
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(DRAGON_ST_FLY_CHASE);
            break;
        case DRAGON_ST_BACK_AWAY:
            FaceTarget(&g_pWolf->pos);
            BackAwayFromWolf();
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(DRAGON_ST_IDLE);
            break;
        case DRAGON_ST_FLY_BACK_AWAY:
            FaceTarget(&g_pWolf->pos);
            BackAwayFromWolf();
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(DRAGON_ST_FLY_CHASE);
            break;
        case DRAGON_ST_CHASE:
            FaceTarget(&g_pWolf->pos);
            if (g_pWolf->HandleMessage(this, MSG_WOLF_IS_RUNNING, 0) && !runLatch && AnimFlags(ANIM_F_FINISHED)) {
                runLatch = 1;
                ScnBody::PlayAnim(ADRAGO01_ANIM_RUN, 1, 0);
            } else if (!g_pWolf->HandleMessage(this, MSG_WOLF_IS_RUNNING, 0) && runLatch &&
                       AnimFlags(ANIM_F_FINISHED)) {
                runLatch = 0;
                ScnBody::PlayAnim(ADRAGO01_ANIM_HOP, 1, 0);
            }
            if (runLatch)
                MoveToTarget(1200);
            else
                MoveToTarget(600);
            break;
        case DRAGON_ST_SKID1:
            if (!UpdateSkid() || AnimFlags(ANIM_F_FINISHED))
                SetState(DRAGON_ST_SKID2);
            break;
        case DRAGON_ST_SKID2:
            if (!UpdateSkid() || AnimFlags(ANIM_F_FINISHED))
                SetState(DRAGON_ST_SKID3);
            break;
        case DRAGON_ST_SKID3:
            if (!UpdateSkid() && AnimFlags(ANIM_F_FINISHED))
                SetState(DRAGON_ST_IDLE);
            break;
        case DRAGON_ST_IDLE_A1:
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(DRAGON_ST_IDLE_A2);
            break;
        case DRAGON_ST_IDLE_A2:
            if (AnimFlags(ANIM_F_FINISHED))
                idleLoopsLeft--;
            if (!idleLoopsLeft)
                SetState(DRAGON_ST_IDLE_A3);
            break;
        case DRAGON_ST_IDLE_A3:
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(DRAGON_ST_IDLE);
            break;
        case DRAGON_ST_IDLE_B:
            if (AnimFlags(ANIM_F_FINISHED))
                idleLoopsLeft--;
            if (!idleLoopsLeft)
                SetState(DRAGON_ST_IDLE);
            break;
        case DRAGON_ST_TAKEOFF:
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(DRAGON_ST_RISE);
            break;
        case DRAGON_ST_RISE:
            if (VerticalMove(1) || AnimFlags(ANIM_F_FINISHED)) {
                if (flyToRockZone)
                    SetState(DRAGON_ST_FLY_TO_ROCKZONE);
                else
                    SetState(DRAGON_ST_FLY_CHASE);
            }
            break;
        case DRAGON_ST_FLY_CHASE:
            FaceTarget(&g_pWolf->pos);
            if (g_pWolf->HandleMessage(this, MSG_WOLF_IS_RUNNING, 0) && AnimFlags(ANIM_F_FINISHED)) {
                runLatch = 1;
                ScnBody::PlayAnim(ADRAGO01_ANIM_FLY, 1, 0);
            } else if (AnimFlags(ANIM_F_FINISHED)) {
                runLatch = 0;
                ScnBody::PlayAnim(ADRAGO01_ANIM_FLY, 1, 0);
            }
            MoveToTarget(600);
            break;
        case DRAGON_ST_FLY_TO_ROCKZONE:
            FaceTarget(&rockZoneTarget);
            MoveToTarget(1800);
            break;
        case DRAGON_ST_FLY_TO_SPHERE:
            FaceTarget(&sphereTarget);
            if (ApproachSphere())
                SetState(DRAGON_ST_FLY_TIMEWARP);
            break;
        case DRAGON_ST_LAND:
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(DRAGON_ST_DESCEND);
            break;
        case DRAGON_ST_DESCEND:
            if (VerticalMove(0))
                SetState(DRAGON_ST_TOUCHDOWN);
            break;
        case DRAGON_ST_TOUCHDOWN:
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(DRAGON_ST_IDLE);
            break;
        case DRAGON_ST_WALK_TO_SPHERE:
            if (ApproachSphere())
                SetState(DRAGON_ST_TIMEWARP);
            break;
        case DRAGON_ST_TIMEWARP:
        case DRAGON_ST_FLY_TIMEWARP:
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(DRAGON_ST_IN_TRANSIT);
            break;
        case DRAGON_ST_ARRIVE:
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(DRAGON_ST_CINE_CHRONOBALL);
            break;
        case DRAGON_ST_CINE_CHRONOBALL:
            if (!g_cinePlayer.Active())
                SetState(DRAGON_ST_GONE);
            break;
        case DRAGON_ST_TAKEOFF_TO_SPHERE:
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(DRAGON_ST_RISE_TO_SPHERE);
            break;
        case DRAGON_ST_RISE_TO_SPHERE:
            if (VerticalMove(1) || AnimFlags(ANIM_F_FINISHED))
                SetState(DRAGON_ST_FLY_TO_SPHERE);
            break;
        case DRAGON_ST_WAIT_PRESENT:
            if (TimeMachine_IsInPresent(g_pWolf))
                SetState(DRAGON_ST_IDLE);
            break;
        case DRAGON_ST_FLY_WAIT_PRESENT:
            if (TimeMachine_IsInPresent(g_pWolf))
                SetState(savedState);
    }
}
void Dragon::SetState(u8 next)
{
    /* clang-format off */
    {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {}
    /* clang-format on */
    if (state == next)
        return;
    state = next;
    runLatch = 0;
    switch (next) {
        case DRAGON_ST_IDLE:
            ScnBody::PlayAnim(ADRAGO01_ANIM_STAND3, 1, 0);
            break;
        case DRAGON_ST_WAIT_FIRST_CONTACT:
            ScnBody::PlayAnim(ADRAGO01_ANIM_STAND3, 1, 0);
            break;
        case DRAGON_ST_CINE_FIRST_CONTACT:
            StartCine(cineFirstContact, flagFirstContact);
            break;
        case DRAGON_ST_INHALE:
            ScnBody::PlayAnim(ADRAGO01_ANIM_STANDA, 0, 0);
            break;
        case DRAGON_ST_FIRE:
            if (!g_pWolf->HandleMessage(this, MSG_WOLF_IS_DEAD, 0))
                g_pWolf->HandleMessage(this, MSG_DRAGON_BURN, 0);
            ScnBody::PlayAnim(ADRAGO01_ANIM_STANDB, 0, 0);
            break;
        case DRAGON_ST_FLY_INHALE:
            ScnBody::PlayAnim(ADRAGO01_ANIM_FLYFIRE, 0, 0);
            break;
        case DRAGON_ST_FLY_FIRE:
            if (!g_pWolf->HandleMessage(this, MSG_WOLF_IS_DEAD, 0))
                g_pWolf->HandleMessage(this, MSG_DRAGON_BURN, 0);
            ScnBody::PlayAnim(ADRAGO01_ANIM_FLYFIRE1, 0, 0);
            break;
        case DRAGON_ST_BACK_AWAY:
            ScnBody::PlayAnim(ADRAGO01_ANIM_HOP, 0, 0);
            break;
        case DRAGON_ST_FLY_BACK_AWAY:
            ScnBody::PlayAnim(ADRAGO01_ANIM_STANDF, 0, 0);
            break;
        case DRAGON_ST_CHASE:
            runLatch = g_pWolf->HandleMessage(this, MSG_WOLF_IS_RUNNING, 0);
            if (runLatch)
                ScnBody::PlayAnim(ADRAGO01_ANIM_RUN, 1, 0);
            else
                ScnBody::PlayAnim(ADRAGO01_ANIM_HOP, 1, 0);
            break;
        case DRAGON_ST_SKID1:
            runLatch = 0;
            ScnBody::PlayAnim(ADRAGO01_ANIM_STOP1A, 0, 0);
            break;
        case DRAGON_ST_SKID2:
            ScnBody::PlayAnim(ADRAGO01_ANIM_STOP1B, 0, 0);
            break;
        case DRAGON_ST_SKID3:
            ScnBody::PlayAnim(ADRAGO01_ANIM_STOP1C, 0, 0);
            break;
        case DRAGON_ST_IDLE_PICK:
            if (Rand_Bounded(5))
                SetState(DRAGON_ST_IDLE_B);
            else
                SetState(DRAGON_ST_IDLE_A1);
            break;
        case DRAGON_ST_IDLE_A1:
            FaceTarget(&g_pWolf->pos);
            idleLoopsLeft = Rand_Bounded(3) + 1;
            ScnBody::PlayAnim(ADRAGO01_ANIM_STAND2A, 1, 0);
            break;
        case DRAGON_ST_IDLE_A2:
            ScnBody::PlayAnim(ADRAGO01_ANIM_STAND2B, 1, 0);
            break;
        case DRAGON_ST_IDLE_A3:
            ScnBody::PlayAnim(ADRAGO01_ANIM_STAND2C, 1, 0);
            break;
        case DRAGON_ST_IDLE_B:
            idleLoopsLeft = Rand_Bounded(12) + 1;
            ScnBody::PlayAnim(ADRAGO01_ANIM_STAND3, 1, 0);
            break;
        case DRAGON_ST_FLY_WAIT:
            ScnBody::PlayAnim(ADRAGO01_ANIM_STANDF, 1, 0);
            break;
        case DRAGON_ST_TAKEOFF:
            flyHeight = 0;
            ScnBody::PlayAnim(ADRAGO01_ANIM_TAKEOFF1, 0, 0);
            break;
        case DRAGON_ST_RISE:
            ScnBody::PlayAnim(ADRAGO01_ANIM_TAKEOFF, 0, 0);
            break;
        case DRAGON_ST_FLY_CHASE:
        case DRAGON_ST_FLY_TO_ROCKZONE:
            if (flyToRockZone)
                ScnBody::PlayAnim(ADRAGO01_ANIM_FASTFLY, 1, 0);
            else
                ScnBody::PlayAnim(ADRAGO01_ANIM_FLY, 1, 0);
            break;
        case DRAGON_ST_FLY_TO_SPHERE:
            FaceTarget(&sphereTarget);
            ScnBody::PlayAnim(ADRAGO01_ANIM_FLY, 1, 0);
            break;
        case DRAGON_ST_LAND:
            ScnBody::PlayAnim(ADRAGO01_ANIM_LAND, 0, 0);
            break;
        case DRAGON_ST_DESCEND:
            ScnBody::PlayAnim(ADRAGO01_ANIM_LAND0, 0, 0);
            break;
        case DRAGON_ST_TOUCHDOWN:
            SnapToGround(1);
            ScnBody::PlayAnim(ADRAGO01_ANIM_LAND1, 0, 0);
            break;
        case DRAGON_ST_WALK_TO_SPHERE:
            FaceTarget(&sphereTarget);
            ScnBody::PlayAnim(ADRAGO01_ANIM_HOP, 1, 0);
            break;
        case DRAGON_ST_TIMEWARP:
            ScnBody::PlayAnim(ADRAGO01_ANIM_INSIDE, 0, 0);
            break;
        case DRAGON_ST_FLY_TIMEWARP:
            ScnBody::PlayAnim(ADRAGO01_ANIM_INSIDE1, 0, 0);
            break;
        case DRAGON_ST_ARRIVE:
            ScnBody::PlayAnim(ADRAGO01_ANIM_OUT, 0, 0);
            break;
        case DRAGON_ST_CINE_CHRONOBALL:
            StartCine(cineChronoBall, flagChronoBall);
            break;
        case DRAGON_ST_RISE_TO_SPHERE:
            ScnBody::PlayAnim(ADRAGO01_ANIM_TAKEOFF, 0, 0);
            break;
        case DRAGON_ST_WAIT_PRESENT:
            PlayAnim(ADRAGO01_ANIM_STAND3, 1, 0);
            break;
        case DRAGON_ST_FLY_WAIT_PRESENT:
            PlayAnim(ADRAGO01_ANIM_STANDF, 1, 0);
            break;
        case DRAGON_ST_GONE:
            SetVisible(0);
            SetBoxCollision(0);
    }
}
/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets */
void Dragon::CalcAnglesTo(Vec3s *from, Vec3s *to, Vec3s *out, s32 withPitch)
{
    struct Work {
        s32 xSquared, ySquared, zSquared, dx, dy, dz;
    } w;
    w.dx = to->x - from->x;
    w.dy = to->y - from->y;
    w.dz = to->z - from->z;
    w.xSquared = w.dx * w.dx;
    w.ySquared = w.dy * w.dy;
    w.zSquared = w.dz * w.dz;
    if (!withPitch)
        out->x = 0;
    else
        out->x = Math_RadiansToAngle4096((float)atan2(w.dy, (s32)sqrt((double)w.xSquared + w.zSquared))) & 0xfff;
    out->y = (Math_RadiansToAngle4096((float)atan2(w.dx, w.dz)) + 0x800) & 0xfff;
    out->z = 0;
}
s32 Dragon::IsWolfInFireReach()
{
    if (wolfDistXZ < 350 && g_pWolf->pos.y > pos.y - 160)
        return 1;
    return 0;
}
s32 Dragon::IsWolfNear()
{
    if (wolfDistXZ < 350)
        return 1;
    return 0;
}
/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets; pad0, pad1, pad2, pad3 fill gaps */
void Dragon::MoveToTarget(s32 speed)
{
    struct Work {
        Vec3s velocity;
        u16 pad0;
        s32 dz, dx;
        Vec3s step;
        u16 pad1;
        s32 distance;
        Vec3s target;
        u16 pad2;
        Vec3s beforeCollision;
        u16 pad3;
        ContactInfo contact;
    } w;
    if (state == DRAGON_ST_FLY_TO_ROCKZONE) {
        w.target.x = rockZoneTarget.x;
        w.target.y = rockZoneTarget.y;
        w.target.z = rockZoneTarget.z;
    } else {
        w.target.x = g_pWolf->pos.x;
        w.target.y = g_pWolf->pos.y;
        w.target.z = g_pWolf->pos.z;
    }
    w.dx = w.target.x - pos.x;
    w.dz = w.target.z - pos.z;
    if (state == DRAGON_ST_FLY_TO_ROCKZONE)
        w.distance = Vec3s_DistXZ(&pos, &rockZoneTarget);
    else
        w.distance = wolfDistXZ;
    ScaleVector(&w.velocity, w.dx, 0, w.dz, speed, w.distance);
    Vec3s_ScaleByDt(&w.velocity, &w.step);
    w.beforeCollision.x = w.step.x;
    w.beforeCollision.y = w.step.y;
    w.beforeCollision.z = w.step.z;
    if (Flying(this)) {
        Collide_ResolveMove(&w.step, &w.contact, 0xb54, COLL_WALL | RESOLVE_KEEP_Y, 0, 0, 10, 0, 0);
        if (w.contact.movableObj) {
            w.step.x = w.beforeCollision.x;
            w.step.y = w.beforeCollision.y;
            w.step.z = w.beforeCollision.z;
        }
    } else
        Collide_ResolveMove(&w.step, &w.contact, 0xb54, RESOLVE_SLIDE_ALL | RESOLVE_KEEP_Y, 0, 0, 10, 0, 0);
    if (state != DRAGON_ST_FLY_TO_ROCKZONE && (wolfDistXZ < 350 || (!w.step.x && !w.step.y && !w.step.z))) {
        if (!g_pWolf->HandleMessage(this, MSG_WOLF_IS_RUNNING, 0) && runLatch) {
            runLatch = 0;
            if (!Flying(this)) {
                StartSkid(&w.step);
                SetState(DRAGON_ST_SKID1);
            }
        }
    } else
        TryMoveInZone(&w.step);
}
/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets; pad0, pad1, pad2 fill gaps */
void Dragon::BackAwayFromWolf()
{
    struct Work {
        s32 dz, dx;
        Vec3s velocity;
        u16 pad0;
        Vec3s wolf;
        u16 pad1;
        ContactInfo contact;
        Vec3s step;
        u16 pad2;
    } w;
    w.wolf.x = g_pWolf->pos.x;
    w.wolf.y = g_pWolf->pos.y;
    w.wolf.z = g_pWolf->pos.z;
    w.dx = pos.x - w.wolf.x;
    w.dz = pos.z - w.wolf.z;
    ScaleVector(&w.velocity, w.dx, 0, w.dz, 300, wolfDistXZ);
    Vec3s_ScaleByDt(&w.velocity, &w.step);
    Collide_ResolveMove(&w.step, &w.contact, 0xb54, RESOLVE_SLIDE_ALL | RESOLVE_KEEP_Y, 0, 0, 10, 0, 0);
    TryMoveInZone(&w.step);
}
void Dragon::StartSkid(Vec3s *step)
{
    skidStep.x = step->x;
    skidStep.y = step->y;
    skidStep.z = step->z;
    skidDirSign.x = NegSign(step->x);
    skidDirSign.y = NegSign(step->y);
    skidDirSign.z = NegSign(step->z);
}
s16 Dragon::NegSign(s16 value)
{
    if (value > 0)
        return -1;
    if (value < 0)
        return 1;
    return 0;
}
/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets; pad0, pad1 fill gaps */
s32 Dragon::UpdateSkid()
{
    struct Work {
        Vec3s newPos;
        u16 pad0;
        ContactInfo contact;
        Vec3s step;
        u16 pad1;
    } w;
    w.step.x = skidStep.x;
    w.step.y = skidStep.y;
    w.step.z = skidStep.z;
    Collide_ResolveMove(&w.step, &w.contact, 0xb54, RESOLVE_SLIDE_ALL | RESOLVE_KEEP_Y, 0, 0, 10, 0, 0);
    w.newPos = pos;
    w.newPos.x += w.step.x;
    w.newPos.y += w.step.y;
    w.newPos.z += w.step.z;
    if (InBox(zone, &w.newPos))
        Translate(&w.step);
    if (skidStep.x)
        skidStep.x = w.step.x / 2;
    if (skidStep.y)
        skidStep.y = w.step.y / 2;
    if (skidStep.z)
        skidStep.z = w.step.z / 2;
    if (!skidStep.x && !skidStep.y && !skidStep.z)
        return 0;
    return 1;
}
void Dragon::TryMoveInZone(Vec3s *delta)
{
    Vec3s newPos = pos;
    newPos.x += delta->x;
    newPos.y += delta->y;
    newPos.z += delta->z;
    if (InBox(zone, &newPos) || state == DRAGON_ST_FLY_TO_ROCKZONE || state == DRAGON_ST_FLY_TO_SPHERE)
        SetPosition(&newPos);
}
void Dragon::ScaleVector(Vec3s *out, s32 x, s32 y, s32 z, s32 mul, s32 div)
{
    out->x = x * mul / div;
    out->y = y * mul / div;
    out->z = z * mul / div;
}
/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets; pad0, pad fill gaps */
s32 Dragon::VerticalMove(s32 up)
{
    struct Work {
        Vec3s wolf;
        u16 pad0[2];
        u16 collision;
        ContactInfo contact;
        Vec3s step;
        u16 pad;
    } w;
    w.wolf = g_pWolf->pos;
    w.step.x = 0;
    w.step.z = 0;
    if (up)
        w.step.y = -(g_dtMs * 160 / riseAnimMs);
    else
        w.step.y = g_dtMs * 160 / descendAnimMs;
    w.collision = Collide_ResolveMove(&w.step, &w.contact, 0xb54, RESOLVE_SLIDE_ALL | RESOLVE_KEEP_Y, 0, 0, 10, 0, 0);
    Translate(&w.step);
    if (up) {
        if (w.collision & (COLL_FLOOR | COLL_WALL | COLL_FLOOR_EDGE))
            return 1;
        flyHeight += (s16)-w.step.y;
        if (flyHeight >= 160)
            return 1;
    } else {
        if ((w.collision & (COLL_FLOOR | COLL_FLOOR_EDGE)) && w.contact.floorObj != g_pWolf)
            return 1;
        flyHeight -= w.step.y;
        if (flyHeight < 0)
            return 1;
    }
    return 0;
}
s32 Dragon::ShouldFly()
{
    s32 i;
    for (i = 0; i < 5; i++) {
        if (flyBoxes[i] && InBoxXZ(flyBoxes[i], &pos))
            return 1;
    }
    if (InBoxXZ(rockZone, &g_pWolf->pos) || InBoxXZ(rockZone, &pos))
        return 1;
    return 0;
}
s32 Dragon::CheckWolfInRockZone()
{
    if (InBoxXZ(rockZone, &g_pWolf->pos) && wolfDistXZ >= 1280) {
        flyToRockZone = 1;
        if (!Flying(this))
            SetState(DRAGON_ST_TAKEOFF);
        else if (state == DRAGON_ST_FLY_CHASE)
            SetState(DRAGON_ST_FLY_TO_ROCKZONE);
        return 1;
    }
    return 0;
}
/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets; pad0, pad1, pad2 fill gaps */
s32 Dragon::ApproachSphere()
{
    struct Work {
        s32 dz, dy, dx;
        Vec3s velocity;
        u16 pad0;
        Vec3s target;
        u16 pad1;
        Vec3s step;
        u16 pad2;
    } w;
    w.target.x = sphereTarget.x;
    w.target.y = sphereTarget.y;
    w.target.z = sphereTarget.z;
    w.dx = w.target.x - pos.x;
    w.dy = w.target.y - pos.y;
    w.dz = w.target.z - pos.z;
    ScaleVector(&w.velocity, w.dx, w.dy, w.dz, 600, sphereDist);
    Vec3s_ScaleByDt(&w.velocity, &w.step);
    TryMoveInZone(&w.step);
    return 0;
}
void Dragon::FaceTarget(Vec3s *target)
{
    Vec3s angles;
    CalcAnglesTo(&pos, target, &angles, 0);
    rot = angles;
}
void Dragon::UpdateDust()
{
    Vec3s source = pos;
    source.y -= (s16)(dustParams.sizeStart >> 1);
    dustEmitter.base.Emitter_UpdateDrift(&dustParams, &source, Facing(),
                                         (state == DRAGON_ST_TAKEOFF || runLatch) && !Flying(this));
}
s32 Dragon::HandleMessage(ScnObject *sender, u32 msg, void *arg)
{
    if (state == DRAGON_ST_GONE)
        return 0;
    if (state == DRAGON_ST_WAIT_FIRST_CONTACT || state == DRAGON_ST_CINE_FIRST_CONTACT ||
        state == DRAGON_ST_CINE_CHRONOBALL)
        return 0;
    switch (msg) {
        case MSG_TIMEMACHINE_CALL:
            /* cast kept: the message arg is a void *; what it carries depends on the message */
            sphereTarget.x = ((Vec3s *)arg)->x;
            sphereTarget.y = ((Vec3s *)arg)->y;
            sphereTarget.z = ((Vec3s *)arg)->z;
            sphereDist = Vec3s_Dist(&pos, &sphereTarget);
            if (sphereDist < 500 || (Flying(this) && sphereDist < 1000)) {
                if (Flying(this)) {
                    if (state == DRAGON_ST_TAKEOFF)
                        SetState(DRAGON_ST_TAKEOFF_TO_SPHERE);
                    else if (state == DRAGON_ST_RISE)
                        SetState(DRAGON_ST_RISE_TO_SPHERE);
                    else
                        SetState(DRAGON_ST_FLY_TO_SPHERE);
                } else
                    SetState(DRAGON_ST_WALK_TO_SPHERE);
                return 1;
            } else {
                savedState = state;
                if (Flying(this))
                    SetState(DRAGON_ST_FLY_WAIT_PRESENT);
                else
                    SetState(DRAGON_ST_WAIT_PRESENT);
                return 0;
            }
        case MSG_TIMEMACHINE_LOCK:
            EnableBoxCollide(0);
            if (Flying(this))
                SetState(DRAGON_ST_FLY_TIMEWARP);
            else
                SetState(DRAGON_ST_TIMEWARP);
            return 1;
        case MSG_TIMEMACHINE_ARRIVE:
            EnableBoxCollide(1);
            /* cast kept: the message arg is a void *; what it carries depends on the message */
            arrivePos.x = ((Vec3s *)arg)->x;
            arrivePos.y = ((Vec3s *)arg)->y;
            arrivePos.z = ((Vec3s *)arg)->z;
            SetPosition(&arrivePos);
            SetState(DRAGON_ST_ARRIVE);
            return 1;
        case MSG_TIMEMACHINE_RELEASE:
            SetState(DRAGON_ST_IDLE);
            break;
    }
    return 0;
}
void Dragon::Render(Camera *view)
{
    ScnMobile::Render(view);
    if (dustEmitter.base.flags.active)
        dustEmitter.base.Emitter_Render(view, 0);
}
void Dragon::Reset()
{
    CheckWolfInRockZone();
}
ScnObject *Dragon_Create(void *record)
{
    ScnBody *object = new Dragon;
    object = object->Init(record, 0);
    return object;
}

/* The eight helpers the original emits as COMDATs after the main .text, in its order. The extra blocks in IsTakingOff
 * and IsLanding are inline-budget devices (header). */
inline s32 Box::ContainsPointXZ(Vec3s *point)
{
    return point->x >= min[0] && point->x <= max[0] && point->z >= min[2] && point->z <= max[2];
}
inline s32 Dragon::IsIdleVariant()
{
    return state == DRAGON_ST_IDLE_PICK || state == DRAGON_ST_IDLE_A1 || state == DRAGON_ST_IDLE_A2 ||
           state == DRAGON_ST_IDLE_A3 || state == DRAGON_ST_IDLE_B;
}
inline s32 Dragon::IsFlying()
{
    return TakingOff(this) || state == DRAGON_ST_FLY_CHASE || Landing(this) || state == DRAGON_ST_FLY_FIRE ||
           state == DRAGON_ST_FLY_INHALE || state == DRAGON_ST_FLY_TO_ROCKZONE || state == DRAGON_ST_FLY_WAIT ||
           state == DRAGON_ST_FLY_BACK_AWAY || state == DRAGON_ST_FLY_TO_SPHERE || state == DRAGON_ST_FLY_TIMEWARP ||
           state == DRAGON_ST_FLY_WAIT_PRESENT;
}
/* BYTES(slot-scope, inferred): the nested block(s) only order the frame: their locals are allocated after the enclosing scope's */
/* BYTES(inline): the six extra blocks only raise /Ob1's cost estimate; the COMDAT code is unchanged */
inline s32 Dragon::IsLanding()
{
    {
        {
            {
                {
                    {
                        {
                            return state == DRAGON_ST_LAND || state == DRAGON_ST_DESCEND ||
                                   state == DRAGON_ST_TOUCHDOWN;
                        }
                    }
                }
            }
        }
    }
}
/* BYTES(slot-scope, inferred): the nested block(s) only order the frame: their locals are allocated after the enclosing scope's */
/* BYTES(inline): the extra block only raises /Ob1's cost estimate; the COMDAT code is unchanged */
inline s32 Dragon::IsTakingOff()
{
    {
        return state == DRAGON_ST_TAKEOFF || state == DRAGON_ST_RISE || state == DRAGON_ST_TAKEOFF_TO_SPHERE ||
               state == DRAGON_ST_RISE_TO_SPHERE;
    }
}
inline s32 Dragon::IsFiring()
{
    return state == DRAGON_ST_INHALE || state == DRAGON_ST_FIRE || state == DRAGON_ST_FLY_FIRE ||
           state == DRAGON_ST_FLY_INHALE;
}
inline void Dragon::SetBoxCollision(s32 on)
{
    if (on)
        flags &= (u16)~SCN_OF_NO_BOX_COLLIDE;
    else
        flags |= SCN_OF_NO_BOX_COLLIDE;
}
inline void Dragon::PlayAnim(u16 id, s32 loop, s32 blend)
{
    u32 options = 0;
    if (loop)
        options |= ANIM_SET_LOOP;
    if (blend)
        options |= ANIM_SET_BLEND;
    Anim_Start(Inst(), &anim, id, options);
}
