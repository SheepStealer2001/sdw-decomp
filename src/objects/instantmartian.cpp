/*
 * T082 - original object InstantMartian.cpp (guessed name): .text 0x455c30-0x458bb5, .rdata 0x574f10-0x574f3c
 * (__real@4049000000000000, then the vtable). No .data or .bss. InstantMartian::Scale (0x45869a) is defined after
 * SetState, its place in the original.
 *
 * InstantMartian (class 145, CLASSID_INSTANTMARTIAN, vtable 0x574f18, sizeof 0x1e8), SheepD3D.exe
 * 0x455c30-0x458bb4: the little Instant Martians of Planet X, ten of them. They run away when Ralph comes close, are
 * sucked into the vacuum when it is switched on nearby, and chase a shrunk Ralph to sit on him.
 * Designer properties (instantMartianProps): BOX +0, BOX2 +4, BOX3 +8, BOX4 +12, BOX5 +16, CHECKPOINTBOX +20,
 * COLFORTRAJ +24, DETECTBOX +28, MARTIENBEHAVIOR +32, TRAJECTORY +36, TRAJECTORY2 +40.
 *
 * The non-virtual methods are placed by the match-addr lines below. Two functions hold arrangements, said where
 * they are: PostLoadInit's `for (;;) { ...; break; <dead statement>; }` and CollideAndSlide's BoxesOverlapXZ inline.
 *
 * What the states are (InstantMartianState), read from SetState and Update: IDLE (one of four loops, one with the
 * running sound), SHRUNK_RUN chase of the shrunk Ralph, FLEE run away from Ralph on a heading, CATCH dragged toward
 * the point MSG_HOOVER_SUCK gave (the vacuum), DORMANT gone, PATROL follow the TRAJECTORY path, IDLE_ALT startled,
 * HIT jump at Ralph, HIT_LAND land, HIT_HOLD sit on Ralph with a scripted camera, WALK_AWAY step back,
 * HIDDEN_CHECKPOINT wait for the CHECKPOINTBOX.
 *
 * match-addr: InstantMartian_NearXZ=0x455c30
 * match-addr: InstantMartian_SetState=0x457b62
 * match-addr: InstantMartian_HoldWolf=0x4579ad
 * match-addr: InstantMartian_Scale=0x45869a
 * match-addr: InstantMartian_CollideAndSlide=0x4586c5
 * match-addr: InstantMartian_MoveHalves=0x458920
 * match-addr: InstantMartian_InsideRunBoxes=0x458a7d
 */
/* BYTES: dead-code, flow, inline, slot-name. */

#define SDW_MEMBERS_ScnObject            \
    static void *operator new(u32 size); \
    void SetUpdateMode(u8 mode);         \
    void SetFacing(s16 f);               \
    /* inline: its argument is a 2-byte temp */


#define SDW_MEMBERS_TrajPatrol \
    /* 0x456e9a */             \
    void RunForward()          \
    {                          \
        forceAdvance = 1;      \
        forward = 1;           \
    } /* 0x457644 */           \
    void RunBackward()         \
    {                          \
        forceAdvance = 1;      \
        forward = 0;           \
    } /* 0x4576c5 */           \
    u32 Kick()                 \
    {                          \
        forceAdvance = 1;      \
        return forward;        \
    } /* 0x456eeb: switched on */

#define SDW_MEMBERS_InstantMartian \
    void SetPatrolSpeed(s16 v)     \
    {                              \
        patrol.speed = v;          \
    } /* inline: its argument is a 2-byte temp (0x456dd6) */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *inst, Animator *animator, u16 id, u32 opts); /* 0x550196 (used by the PlayAnim inline) */
#include "../engine/sound_mgr.h"
#include "animation.h"
#include "patrol.h"
#include "camera.h"
#include "../app/app_main.h"
#include "../engine/scn_tools.h"
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8 1
#define SDW_INLINE_SCNOBJECT_SETVISIBLE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8
#undef SDW_INLINE_SCNOBJECT_SETVISIBLE_S32
#define SDW_INLINE_SCNOBJECT_SETCOLLIDABLE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETCOLLIDABLE_S32
#define SDW_INLINE_SCNOBJECT_FACING 1
#define SDW_INLINE_SCNOBJECT_SETFACING_S16 1
#define SDW_INLINE_SCNOBJECT_STOPSOUND_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_FACING
#undef SDW_INLINE_SCNOBJECT_SETFACING_S16
#undef SDW_INLINE_SCNOBJECT_STOPSOUND_U16
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#define SDW_INLINE_SHADOW_INVALIDATE 1
#include "../engine/shadow_inlines.h"
#undef SDW_INLINE_SHADOW_INVALIDATE
#define SDW_INLINE_TRAJPATROL_REVERSE 1
#include "../engine/traj_patrol_inlines.h"
#undef SDW_INLINE_TRAJPATROL_REVERSE
#define SDW_INLINE_SCNMOBILE_SETSHADOWRADIUS_U8 1
#include "../engine/scn_mobile_inlines.h"
#undef SDW_INLINE_SCNMOBILE_SETSHADOWRADIUS_U8
#include "../engine/property_math.h"

extern Wolf *g_pWolf; /* 0x6cf310 */

s32 Scenaric_FindByClass(u16 classId, ScnObject **out, s32 max);          /* 0x5145c5 */
s32 Rand_Bounded(s32 bound);                                              /* 0x561219 */
u16 Sound_Play(u16 soundId, void *owner, u16 volume, u8 flags, s32 rate); /* 0x5491b8 */
#include "../sdk/crt.h"
extern "C" s16 Math_RadiansToAngle4096(float radians); /* 0x5269ce */
void Camera_StartScripted(ScnObject *owner, Camera *cam, u16 rotX, u16 rotY, u16 rotZ, Vec3s *pos, u16 focal, u32 mode,
                          s32 time); /* 0x55a70d */
extern "C" s16 g_sinTable4096[5122]; /* 0x57ece0  4.12 sine, 4096 steps per turn */
extern "C" const s16 *g_pCosTable;   /* 0x5814e4  = g_sinTable4096 + 1024 */
extern s32 g_dtMs;                   /* 0x71b2e8  g_dt * 1000 >> 12 */
extern "C" s32 Coll_BoxGroundQuery(CollBox *box, s32 *height, ScnObject *self, u8 mode,
                                   ScnObject **outObject); /* 0x51a9de */

/* Is the point inside the box (all three axes / in plan)? Inline: both arguments are stack temps, the 1/0 a
 * compiler temporary (0x456204, 0x456565). */
#define SDW_INLINE_FREE_INBOX_BOX_VEC3S 1
#define SDW_INLINE_FREE_INBOXXZ_BOX_VEC3S 1
#include "../engine/coll_box_inlines.h"
#undef SDW_INLINE_FREE_INBOX_BOX_VEC3S
#undef SDW_INLINE_FREE_INBOXXZ_BOX_VEC3S

/* Do two boxes overlap in plan? Every face distance must be non-negative, so no sign bit may be set in their OR
 * (inline: both arguments are used as they are, no temps; 0x45881e). */
inline s32 BoxesOverlapXZ(CollBox *a, CollBox *b)
{
    return ~((a->max.x - b->min.x) | (b->max.x - a->min.x) | (a->max.z - b->min.z) | (b->max.z - a->min.z)) &
           0x80000000;
}

/* The first box of the model's own box list, or NULL (its local and return temp at 0x456769 / 0x456781). */
#define SDW_INLINE_FREE_MODEL_FIRSTBOX_MODEL 1
#include "../engine/model_inlines.h"
#undef SDW_INLINE_FREE_MODEL_FIRSTBOX_MODEL

/* A designer property: the dword at record + 0x14 + off (instantMartianProps). Inline: the offset is a stack temp. */
#define SDW_INLINE_FREE_PROPU32_VOID_U32 2
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_PROPU32_VOID_U32

/* 0x455c30 - the two points are within 50 units of each other on both horizontal axes (strictly inside the
 * 100-unit square). */
s32 InstantMartian_NearXZ(Vec3s *a, Vec3s *b)
{
    if (a->x < b->x + 0x32 && a->x > b->x - 0x32 && a->z < b->z + 0x32 && a->z > b->z - 0x32)
        return 1;
    return 0;
}

/* The original tests `traj` with ecx straight after storing it
 * through ecx (0x455d14); a plain `traj = ...; if (traj)` gets edx. Counting the shift, the original's statement
 * tree holds two register-allocating nodes there that emit nothing (equivalently: this source was one step
 * ahead in VC6's eax -> ecx -> edx rotation). Measured and ruled out before the loop shape below: casts on the
 * call result, the argument and the offset; Scn_GetPropTrajectory declared returning u32 / s32 / void * /
 * s16 * / u16 * / u8 *; `traj` typed void * or Trajectory *; `if (traj != 0)`, `!(traj == 0)`, `!!traj`,
 * `traj ? 1 : 0`, `(s32)traj`, `traj != (void *)0` (all edx) and `traj != 0 && 1`, `1 && traj`, `traj || 0`
 * (different code); the assignment folded into the condition; an inline method holding the if (with and
 * without the trajectory as its argument); no-effect expression statements (dropped at parse time);
 * `if (0) ...` (emitted); a goto over a dead statement (emits a jmp); `do { ... break; ... } while (0)` (no
 * shift). What works is an unreachable two-register statement after a `break` inside `for (;;)`: one-register
 * dead statements (`rec = rec`) shift by one, `traj = traj` (three) by none. */
/* 0x455c8e - vtable +0x00: the designer properties. MARTIENBEHAVIOR picks the variant (1 = the one that only
 * reacts inside its boxes, 4 = the one that runs its trajectory), TRAJECTORY drives a 600 units/s follower,
 * BOX..BOX5 are the boxes it is allowed to run in, and every Laser of the level is collected so that the run can
 * be stopped by one. The placed position, after snapping to the ground, is the respawn point.
 * The local names pin the frame (tools/vc6_locals.py: rec, i, box at EBP-4, -6, -0xc). */
/* BYTES(flow): the dead statement after break emits nothing but advances /Od's register rotation, so traj is tested with ecx (0x455d14) */
/* BYTES(slot-name): names chosen for their stack slots: rec, i, box at EBP-4, -6, -0xc */
void InstantMartian::PostLoadInit()
{
    Box *box;
    u16 i;
    u16 *rec;

    rec = record;
    switch (PropU32(rec, 0x20)) { /* PROPERTY_INSTANTMARTIAN_MARTIENBEHAVIOR */
        case 1:
            behaviour = 1;
            break;
        case 4:
            behaviour = 3;
            break;
    }
    SetState(IMARTIAN_ST_IDLE);
    sitAnimMs = Anim_GetDurationMs(Inst(), AMARTI01_ANIM_CATCH, 0);
    /* REPRESENTATION, not a claim about the original's text: the unreachable second statement is never emitted,
     * but VC6 still hands it two registers of the eax -> ecx -> edx rotation, which is what makes the original
     * test `traj` with ecx right after storing it through ecx (0x455d14, the only such reuse after a member
     * store in the whole image). Every other shape tried is listed in the note above the function. */
    for (;;) {
        traj = Scn_GetPropTrajectory(rec, 0x24); /* PROPERTY_INSTANTMARTIAN_TRAJECTORY */
        break;
        traj = (Trajectory *)rec; /* cast kept: the unreachable statement only needs two registers (see above) */
    }
    if (traj)
        TrajPatrol_Init(&patrol, traj, 600, 0x800, 1, 1, 0x32);
    colForTraj = PropU32(rec, 0x18); /* PROPERTY_INSTANTMARTIAN_COLFORTRAJ */
    boxCount = 0xffff;
    for (i = 0; i < 5; i++) {
        box = Scn_GetPropBox(rec, i * 4); /* PROPERTY_INSTANTMARTIAN_BOX .. BOX5 */
        if (box) {
            boxCount++;
            boxes[boxCount] = box;
        }
    }
    boxCount++;
    checkpointBox = Scn_GetPropBox(rec, 0x14); /* PROPERTY_INSTANTMARTIAN_CHECKPOINTBOX */
    laserCount = (u16)Scenaric_FindByClass(CLASSID_LASER, lasers, 0x37);
    SnapToGround(1);
    homePos = pos;
    SetUpdateMode(SCN_UPD_NORMAL);
    turning = 0;
    heading = 0;
    target.x = 0;
    target.y = 0;
    target.z = 0;
    step.x = 0;
    step.y = 0;
    step.z = 0;
    EnableBoxes(COLLBOX_ALT_SET);
    DisableBoxes(COLLBOX_CYLINDER);
    boxIndex = 0;
    stateTimer = 0x400;
    shrunk = 0;
    holdingWolf = 0;
    voice = 0;
}

/* 0x455fc1 - vtable +0x14: back to the placed position, visible, solid and idle. A martian in IMARTIAN_ST_DORMANT
 * (already sucked into the vacuum) is left where it is; only the shrink flag, the hold on Ralph and the camera are
 * undone. */
void InstantMartian::Reset()
{
    if (state != IMARTIAN_ST_DORMANT) {
        if (traj)
            TrajPatrol_Init(&patrol, traj, 600, 0x800, 1, 1, 0x32);
        SetState(IMARTIAN_ST_IDLE);
        SetPosition(&homePos);
        SetVisible(1);
        SetCollidable(1);
        EnableBoxes(COLLBOX_ALT_SET);
        DisableBoxes(COLLBOX_CYLINDER);
        boxIndex = 0;
    }
    shrunk = 0;
    HoldWolf(0);
    voice = 0;
    Camera_ReleaseScripted(this);
}

/* 0x4560e5 - vtable +0x08: the martian is drawn at a quarter scale, and at a twelfth once shrunk. */
void InstantMartian::Render(Camera *view)
{
    Vec3s s;
    s.x = s.y = s.z = Scale(0x400);
    RenderScaled(view, &s);
}

/* 0x456128 - vtable +0x04: one frame of the martian, by state (see the list at the top of the file).
 * The local names are chosen for their stack slots (tools/vc6_locals.py: finished, vel, contact, wolfD2, diff,
 * hitFlags, keep, probe, limbo, yHit, dh, endD2 at EBP-4, -0xc, -0x28, -0x2c, -0x34, -0x36, -0x40, -0x50, -0x58,
 * -0x5c, -0x5e, -0x64). The case bodies are in the original's source order (0, 3, 14, 7 falling into 1, 5, 2, 8,
 * 9, 10, 13). */
/* BYTES(slot-name): names chosen for their stack slots */
void InstantMartian::Update()
{
    u32 endD2;
    s16 dh;
    s32 yHit;
    Vec3s limbo;
    CollBox probe;
    Vec3s keep;
    u16 hitFlags;
    Vec3s diff;
    s32 wolfD2;
    ContactInfo contact;
    Vec3s vel;
    s32 finished;

    wolfD2 = Vec3s_DistSq(&g_pWolf->pos, &pos);
    shadow.Invalidate();
    switch (state) {
        case IMARTIAN_ST_IDLE: /* idle: flee from Ralph when he comes near */
            if (g_pWolf->HandleMessage(this, MSG_QUERY_SIZE, 0) == WOLF_SIZE_SMALL && !shrunk) {
                SetState(IMARTIAN_ST_SHRUNK_RUN);
                break;
            }
            if (wolfD2 < 160000 && (behaviour == 1 || (InBox(boxes[boxIndex], &g_pWolf->pos) && behaviour == 3))) {
                SetState(IMARTIAN_ST_FLEE);
                break;
            }
            if (AnimFlags(ANIM_F_FINISHED)) {
                SetState(IMARTIAN_ST_IDLE);
                break;
            }
            break;
        case IMARTIAN_ST_CATCH: /* dragged toward the vacuum */
            if (Vec3s_DistSqXZ(&target, &pos) > 900) {
                step.x = toTarget.x;
                step.y = toTarget.y;
                step.z = toTarget.z;
                step.x *= (s16)g_dtMs;
                step.y *= (s16)g_dtMs;
                step.z *= (s16)g_dtMs;
                step.x /= (s16)(sitAnimMs - sitAnimMs / 3);
                step.y /= (s16)(sitAnimMs - sitAnimMs / 3);
                step.z /= (s16)(sitAnimMs - sitAnimMs / 3);
                if (step.x == 0 && step.y == 0 && step.z == 0)
                    SetPosition(&target);
                else
                    Translate(&step);
            }
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(IMARTIAN_ST_HIDDEN_CHECKPOINT);
            break;
        case IMARTIAN_ST_HIDDEN_CHECKPOINT: /* wait until Ralph is in the CHECKPOINTBOX */
            if (checkpointBox == 0)
                SetState(IMARTIAN_ST_DORMANT);
            else if (!InBoxXZ(checkpointBox, &g_pWolf->pos))
                SetState(IMARTIAN_ST_DORMANT);
            break;
        case IMARTIAN_ST_IDLE_ALT: /* startled */
            if (AnimFlags(ANIM_F_FINISHED)) {
                switch (Rand_Bounded(2)) {
                    case 0:
                        PlayAnim(AMARTI01_ANIM_STAND0, 1, 0);
                        break;
                    case 1:
                        PlayAnim(AMARTI01_ANIM_STAND1, 1, 0);
                        break;
                }
            }
        case IMARTIAN_ST_SHRUNK_RUN: /* the shrunk Ralph is near: chase him, jump on him */
            if (wolfD2 < 10000 && g_pWolf->HandleMessage(this, MSG_WOLF_IS_FLYING, 0) == 0) {
                step.x = g_pWolf->pos.x - pos.x;
                step.y = g_pWolf->pos.y - pos.y;
                step.z = g_pWolf->pos.z - pos.z;
                probe.flags = Model_FirstBox(inst_model)->flags;
                probe.Box_Translate(Model_FirstBox(inst_model), &g_pWolf->pos);
                probe.min.y -= 0xfa;
                probe.max.y -= 4;
                probe.min.x -= 4;
                probe.min.z -= 4;
                probe.max.x += 4;
                probe.max.z += 4;
                if (Coll_BoxGroundQuery(&probe, &yHit, this, CQ_STATIC, 0) == 0) {
                    probe.min.y = g_pWolf->pos.y < pos.y ? g_pWolf->pos.y : pos.y;
                    probe.max.y = g_pWolf->pos.y > pos.y ? g_pWolf->pos.y : pos.y;
                    probe.min.y -= 0xfa;
                    limbo.x = 0;
                    limbo.y = -32000;
                    limbo.z = 0;
                    keep = pos;
                    SetPosition(&limbo);
                    SetCollidable(0);
                    if (Coll_BoxGroundQuery(&probe, &yHit, g_pWolf, CQ_OBJECTS, 0) == 0) {
                        SetPosition(&keep);
                        SetCollidable(1);
                        if (CollideAndSlide(&step, &contact, 0xb54, RESOLVE_SLIDE_ALL, 1) == 0) {
                            SetState(IMARTIAN_ST_HIT);
                            break;
                        }
                    }
                    SetCollidable(1);
                    SetPosition(&keep);
                }
            }
            if (InsideRunBoxes(g_pWolf->pos) && g_pWolf->HandleMessage(this, MSG_WOLF_IS_FLYING, 0) == 0) {
                diff.x = g_pWolf->pos.x - pos.x;
                diff.y = g_pWolf->pos.y - pos.y;
                diff.z = g_pWolf->pos.z - pos.z;
                heading = Math_RadiansToAngle4096((float)atan2((double)diff.x, (double)diff.z)) & 0xfff;
                vel.x = (s16)(g_sinTable4096[heading] * Scale(600) >> 12);
                vel.z = (s16)(g_pCosTable[heading] * Scale(600) >> 12);
                vel.y = 100;
                Vec3s_ScaleByDt(&vel, &step);
                CollideAndSlide(&step, &contact, 0xb54, RESOLVE_SLIDE_ALL, 0);
                nextPos.x = step.x + pos.x;
                nextPos.y = step.y + pos.y;
                nextPos.z = step.z + pos.z;
                SetFacing(heading + 0x800);
                if (InsideRunBoxes(nextPos)) {
                    if (contact.wallObj) {
                        if (state != IMARTIAN_ST_IDLE_ALT) {
                            SetState(IMARTIAN_ST_IDLE_ALT);
                            break;
                        }
                    } else {
                        if (state != IMARTIAN_ST_SHRUNK_RUN) {
                            SetState(IMARTIAN_ST_SHRUNK_RUN);
                            break;
                        }
                        Translate(&step);
                    }
                } else {
                    if (state != IMARTIAN_ST_IDLE_ALT) {
                        SetState(IMARTIAN_ST_IDLE_ALT);
                        break;
                    }
                }
            } else {
                if (state != IMARTIAN_ST_IDLE_ALT) {
                    SetState(IMARTIAN_ST_IDLE_ALT);
                    break;
                }
            }
            if (g_pWolf->HandleMessage(this, MSG_QUERY_SIZE, 0) != WOLF_SIZE_SMALL && !shrunk) {
                SetState(IMARTIAN_ST_FLEE);
                break;
            }
            break;
        case IMARTIAN_ST_PATROL: /* follow the TRAJECTORY */
            SetPatrolSpeed(Scale(600));
            finished = TrajPatrol_Step(&pos, &patrol, &vel, &heading);
            SetFacing(heading);
            Vec3s_ScaleByDt(&vel, &step);
            if (colForTraj) {
                CollideAndSlide(&step, &contact, 0xb54, RESOLVE_SLIDE_ALL, 0);
                if (contact.movableObj)
                    patrol.Reverse();
            }
            if (finished && behaviour == 3) {
                switch (patrol.Kick()) {
                    case 0:
                        if (InBox(boxes[0], &pos)) {
                            boxIndex = 0;
                            SetState(IMARTIAN_ST_FLEE);
                        }
                        break;
                    case 1:
                        if (InBox(boxes[1], &pos)) {
                            boxIndex = 1;
                            SetState(IMARTIAN_ST_FLEE);
                        }
                        break;
                }
            }
            Translate(&step);
            if (colForTraj)
                SnapToGround(1);
            break;
        case IMARTIAN_ST_FLEE: /* run away from Ralph, turning off the walls */
            if (g_pWolf->HandleMessage(this, MSG_QUERY_SIZE, 0) == WOLF_SIZE_SMALL && !shrunk) {
                SetState(IMARTIAN_ST_SHRUNK_RUN);
                break;
            }
            if (turning == 0) {
                diff.x = pos.x - g_pWolf->pos.x;
                diff.y = pos.y - g_pWolf->pos.y;
                diff.z = pos.z - g_pWolf->pos.z;
                heading = Math_RadiansToAngle4096((float)atan2((double)diff.x, (double)diff.z)) & 0xfff;
            } else {
                turnTimer -= g_dtMs;
                if (turnTimer <= 0)
                    turning = 0;
            }
            vel.x = (s16)(g_sinTable4096[(s16)(heading & 0xfff)] * Scale(600) >> 12);
            vel.z = (s16)(g_pCosTable[(s16)(heading & 0xfff)] * Scale(600) >> 12);
            vel.y = 100;
            Vec3s_ScaleByDt(&vel, &step);
            hitFlags = CollideAndSlide(&step, &contact, 0xb54, RESOLVE_SLIDE_ALL, 0);
            nextPos.x = pos.x;
            nextPos.y = pos.y;
            nextPos.z = pos.z;
            nextPos.x += step.x;
            nextPos.y += step.y;
            nextPos.z += step.z;
            if ((hitFlags & COLL_WALL) || (hitFlags & COLL_10) || !InsideRunBoxes(nextPos)) {
                turning = 0x800;
                if (contact.wallNormalMean.x == 0 && contact.wallNormalMean.y == 0 && contact.wallNormalMean.z == 0) {
                    heading = (heading + 0x400) & 0xfff;
                } else {
                    dh = (s16)(((s16)(Math_RadiansToAngle4096((float)atan2((double)contact.wallNormalMean.x,
                                                                           (double)contact.wallNormalMean.z)) &
                                      0xfff) -
                                heading + 0x800) &
                               0xfff) -
                         0x800;
                    heading = (heading + dh * 2 + 0x800) & 0xfff;
                }
                turnTimer = 0xfa;
            } else {
                Translate(&step);
                SetFacing((heading + 0x800) & 0xfff);
            }
            if (wolfD2 > 360000) {
                SetState(IMARTIAN_ST_IDLE);
                break;
            }
            stateTimer -= g_dtMs;
            if (stateTimer <= 0) {
                stateTimer = 0x800;
                if (behaviour == 3) {
                    endD2 = Vec3s_DistSqXZ(&pos, &patrol.traj->pts[0]);
                    if (endD2 < 40000) {
                        patrol.RunForward();
                        SetState(IMARTIAN_ST_PATROL);
                        break;
                    }
                    endD2 = Vec3s_DistSqXZ(&pos, &patrol.traj->pts[patrol.traj->count - 1]);
                    if (endD2 < 40000) {
                        patrol.RunBackward();
                        SetState(IMARTIAN_ST_PATROL);
                        break;
                    }
                }
            }
            break;
        case IMARTIAN_ST_HIT: /* the jump: land on Ralph or next to him */
            if (AnimFlags(ANIM_F_FINISHED)) {
                if (InstantMartian_NearXZ(&wolfPos, &g_pWolf->pos))
                    SetState(IMARTIAN_ST_HIT_HOLD);
                else
                    SetState(IMARTIAN_ST_HIT_LAND);
            }
            break;
        case IMARTIAN_ST_HIT_LAND:
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(IMARTIAN_ST_IDLE);
            break;
        case IMARTIAN_ST_HIT_HOLD: /* sitting on Ralph */
            stateTimer -= g_dtMs;
            if (stateTimer <= 0 && HoldWolf(1))
                g_pWolf->HandleMessage(this, MSG_KILL, (void *)KILL_MARTIAN); /* cast kept: arg carries a number */
            vel.x = 0;
            vel.y = 100;
            vel.z = 0;
            Vec3s_ScaleByDt(&vel, &step);
            CollideAndSlide(&step, &contact, 0xb54, RESOLVE_SLIDE_ALL, 0);
            Translate(&step);
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(IMARTIAN_ST_WALK_AWAY);
            break;
        case IMARTIAN_ST_WALK_AWAY: /* step back */
            vel.x = (s16)(g_sinTable4096[(s16)((Facing() + 0x400) & 0xfff)] * Scale(0x96) >> 12);
            vel.z = (s16)(g_pCosTable[(s16)((Facing() + 0x400) & 0xfff)] * Scale(0x96) >> 12);
            vel.y = 100;
            Vec3s_ScaleByDt(&vel, &step);
            CollideAndSlide(&step, &contact, 0xb54, RESOLVE_SLIDE_ALL, 0);
            Translate(&step);
            break;
    }
    AdvanceAnim();
}

/* 0x4579ad - attach Ralph to the martian's joint 0x17 (MSG_PICKUP) or let him go again (MSG_DROP, with the drop
 * argument's flag1 set so that the Wolf keeps his own position). Returns whether anything changed. */
s32 InstantMartian::HoldWolf(s32 hold)
{
    DropMsgArg drop;
    drop.flag1 = 1;
    if (hold != holdingWolf) {
        if (hold)
            g_pWolf->HandleMessage(this, MSG_PICKUP, (void *)0x17); /* cast kept: arg carries the joint number */
        else
            g_pWolf->HandleMessage(this, MSG_DROP, &drop);
        holdingWolf = hold;
        return 1;
    }
    return 0;
}

/* 0x457a27 - vtable +0x10. MSG_MARTIAN_IS_CAUGHT asks whether the martian is already in the vacuum; MSG_HOOVER_SUCK
 * sends it to the sender's position; MSG_SET_SIZE shrinks or unshrinks it (the solid box swaps between
 * COLLBOX_ALT_SET and COLLBOX_CYLINDER and the blob shadow follows the new size); MSG_QUERY_SIZE asks whether it is
 * shrunk. */
s32 InstantMartian::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    switch (msgId) {
        case MSG_MARTIAN_IS_CAUGHT:
            if (state == IMARTIAN_ST_DORMANT)
                return 1;
            break;
        case MSG_HOOVER_SUCK:
            if (state != IMARTIAN_ST_CATCH) {
                target.x = sender->pos.x;
                target.y = sender->pos.y;
                target.z = sender->pos.z;
                SetState(IMARTIAN_ST_CATCH);
            }
            break;
        case MSG_SET_SIZE:
            if ((s32)arg == shrunk) /* cast kept: this message's arg carries the new shrink flag */
                return 0;
            shrunk = (s32)arg; /* cast kept: as above */
            if (arg) {
                EnableBoxes(COLLBOX_CYLINDER);
                DisableBoxes(COLLBOX_ALT_SET);
            } else {
                EnableBoxes(COLLBOX_ALT_SET);
                DisableBoxes(COLLBOX_CYLINDER);
            }
            SetShadowRadius((u8)Scale(0x19));
            SetState(IMARTIAN_ST_IDLE);
            return 1;
        case MSG_QUERY_SIZE:
            if (shrunk)
                return 1;
            return 0;
    }
    return 0;
}

/* 0x457b62 - enter a state. The running sound is cut first, then each state picks its animation and whatever it
 * needs; IMARTIAN_ST_HIT_HOLD, the "sit on Ralph" approach, also places a scripted camera behind and above the
 * martian, aimed at it (atan2 of the offset, in 4096ths of a turn).
 * The local names are chosen for their stack slots (tools/vc6_locals.py: dz, dv, dx, eyeRot, step, sqz, ysq, xsq,
 * viewer at EBP-4, -8, -0xc, -0x14, -0x1c, -0x20, -0x24, -0x28, -0x30); dv squared (ysq) is computed and never
 * used, exactly as in the original. The case bodies are in the original's source order, which is not the numeric
 * one (0, 2, 3, 4, 14, 5, 7, 1, 8, 9, 10, 13). */
/* BYTES(dead-code): ysq is computed and never read, as in the original */
void InstantMartian::SetState(u8 newState)
{
    s32 dz;
    s32 dv;
    s32 xsq;
    s32 ysq;
    s32 sqz;
    Vec3s step;
    Vec3s eyeRot;
    s32 dx;
    Vec3s viewer;

    if (Sound_IsPlaying(voice))
        StopSound(voice);
    switch (newState) {
        case IMARTIAN_ST_IDLE: /* idle: one of four loops, one of them with the running sound */
            switch (Rand_Bounded(4)) {
                case 0:
                    PlayAnim(AMARTI01_ANIM_STAND0, 1, 1);
                    break;
                case 1:
                    PlayAnim(AMARTI01_ANIM_STAND1, 1, 1);
                    break;
                case 2:
                    voice = Sound_Play(SND_MARTIAN_VOICE, this, 0xff, SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 0x1000);
                    PlayAnim(AMARTI01_ANIM_STAND3, 1, 1);
                    break;
                case 3:
                    PlayAnim(AMARTI01_ANIM_STAND4, 1, 1);
                    break;
            }
            break;
        case IMARTIAN_ST_FLEE: /* turn away and run off */
            PlayAnim(AMARTI01_ANIM_RUN, 1, 0);
            if (state == IMARTIAN_ST_PATROL)
                stateTimer = 0x800;
            else
                stateTimer = 0;
            heading = (s16)((Facing() + 0x800) & 0xfff);
            turning = 0;
            break;
        case IMARTIAN_ST_CATCH: /* sucked toward the point MSG_HOOVER_SUCK gave */
            PlayAnim(AMARTI01_ANIM_CATCH, 0, 0);
            toTarget.x = (s16)(target.x - pos.x);
            toTarget.y = (s16)(target.y - pos.y);
            toTarget.z = (s16)(target.z - pos.z);
            break;
        case IMARTIAN_ST_DORMANT: /* gone (in the vacuum): never updated again */
            SetUpdateMode(SCN_UPD_NEVER);
            break;
        case IMARTIAN_ST_HIDDEN_CHECKPOINT:
            SetUpdateMode(SCN_UPD_ALWAYS);
            SetVisible(0);
            SetCollidable(0);
            break;
        case IMARTIAN_ST_PATROL:
            PlayAnim(AMARTI01_ANIM_RUN, 1, 0);
            break;
        case IMARTIAN_ST_IDLE_ALT:
            if (state != IMARTIAN_ST_IDLE_ALT) {
                switch (Rand_Bounded(3)) {
                    case 0:
                        PlayAnim(AMARTI01_ANIM_STAND2, 1, 1);
                        break;
                    case 1:
                        PlayAnim(AMARTI01_ANIM_STAND5, 1, 1);
                        break;
                }
            }
            break;
        case IMARTIAN_ST_SHRUNK_RUN:
            PlayAnim(AMARTI01_ANIM_RUN3, 1, 0);
            break;
        case IMARTIAN_ST_HIT: /* jump at Ralph: aim 4 units above where he is now */
            PlayAnim(AMARTI01_ANIM_HIT2A, 1, 0);
            wolfPos.x = g_pWolf->pos.x;
            wolfPos.y = g_pWolf->pos.y;
            wolfPos.z = g_pWolf->pos.z;
            wolfPos.y = (s16)(wolfPos.y - 4);
            SetCollidable(0);
            break;
        case IMARTIAN_ST_HIT_LAND:
            SetCollidable(1);
            PlayAnim(AMARTI01_ANIM_HIT, 1, 0);
            SetPosition(&wolfPos);
            SnapToGround(1);
            break;
        case IMARTIAN_ST_HIT_HOLD: /* sitting on Ralph: the scripted camera behind and above */
            SetCollidable(1);
            PlayAnim(AMARTI01_ANIM_HIT2B, 1, 0);
            SetPosition(&wolfPos);
            stateTimer = 0x78;
            viewer.x = pos.x;
            viewer.y = pos.y;
            viewer.z = pos.z;
            viewer.y = (s16)(viewer.y - 0x64);
            viewer.x += (s16)(g_sinTable4096[(s16)((Facing() - 0x400) & 0xfff)] * 0x78 >> 12);
            viewer.z += (s16)(g_pCosTable[(s16)((Facing() - 0x400) & 0xfff)] * 0x78 >> 12);
            dx = pos.x - viewer.x;
            dv = pos.y - viewer.y;
            dz = pos.z - viewer.z;
            xsq = dx * dx;
            ysq = dv * dv;
            sqz = dz * dz;
            eyeRot.x =
                (s16)(Math_RadiansToAngle4096((float)atan2((double)dv - 50.0, (double)(s32)sqrt((double)xsq + sqz))) &
                      0xfff);
            eyeRot.y = (s16)(Math_RadiansToAngle4096((float)atan2((double)-dx, (double)dz)) & 0xfff);
            Camera_StartScripted(this, &g_camera, eyeRot.x, eyeRot.y, 0, &viewer, 0x1f4, 0, 0x1000);
            break;
        case IMARTIAN_ST_WALK_AWAY: /* a 15-unit step backwards */
            PlayAnim(AMARTI01_ANIM_WALK, 1, 0);
            step.x = (s16)(g_sinTable4096[(s16)((Facing() + 0x400) & 0xfff)] * 0xf >> 12);
            step.y = 0;
            step.z = (s16)(g_pCosTable[(s16)((Facing() + 0x400) & 0xfff)] * 0xf >> 12);
            Translate(&step);
            break;
    }
    state = newState;
}

/* 0x45869a - every length and speed the class uses goes through this: a third of itself once shrunk. */
s16 InstantMartian::Scale(s16 v)
{
    if (shrunk)
        return (s16)(v / 3);
    return v;
}

/* 0x4586c5 - the martian's own move step: slide the requested move, then treat any live Laser beam as a wall.
 * The move itself is done in two halves (MoveHalves); a shrunk martian skips the beam test altogether. The beam
 * test first skips a beam whose box already contains the martian's own position (in plan), then, if the beam is
 * on and its box overlaps the martian's world box in plan, reports the beam as the wall contact and kills the
 * offending component of the move. The overlap is a sign-bit test over the four face distances OR-ed together.
 * Written inline in the if, the overlap test comes out one register step off: VC6 evaluates the OR chain left
 * operand first and accumulates into the first register; as the inline
 * BoxesOverlapXZ below, with the terms in plain x-then-z order, it evaluates them in the original's order and
 * accumulates into the last (0x45881e..0x458858). The containment test is the InBoxXZ inline (its point argument
 * is the stack temp at EBP-0x2c, the box argument the local itself).
 * Local names pin the frame (r, lbox, i, obj, myBox, at at EBP-2, -8, -0xc, -0x10, -0x20, -0x28). */
/* BYTES(inline): source-only inline: only as an inline does VC6 evaluate the OR chain in the original's order (0x45881e..0x458858) */
u16 InstantMartian::CollideAndSlide(Vec3s *delta, ContactInfo *info, u16 cutoff, u16 flags, s32 noMove)
{
    u16 r;
    CollBox *lbox;
    s32 i;
    CollBox myBox;
    Laser *obj;
    Vec3s at;

    r = 0;
    if (noMove == 0)
        r = MoveHalves(delta, info, cutoff, flags);
    if (shrunk)
        return r;
    at.x = (s16)(pos.x + delta->x);
    at.y = (s16)(pos.y + delta->y);
    at.z = (s16)(pos.z + delta->z);
    myBox.Box_Translate(GetFirstSolidBox(), &at);
    for (i = 0; i < laserCount; i++) {
        obj = (Laser *)lasers[i]; /* cast kept: lasers[] holds the level's Lasers (Scenaric_FindByClass) */
        lbox = obj->beamBox;
        if (!InBoxXZ((Box *)lbox, &pos)) { /* cast kept: Box is the plain C view of the CollBox record */
            if (obj->beamOn) {
                if (BoxesOverlapXZ(lbox, &myBox)) {
                    info->wallObj = obj;
                    info->movableObj = obj;
                    r = COLL_WALL;
                    info->wallNormalMean.x = obj->normX;
                    info->wallNormalMean.y = 0;
                    info->wallNormalMean.z = obj->normZ;
                    if (lbox->min.x < myBox.max.x)
                        delta->x = 0;
                    else if (myBox.min.x < lbox->max.x)
                        delta->x = 0;
                    if (lbox->min.z < myBox.max.z)
                        delta->z = 0;
                    else if (myBox.min.z < lbox->max.z)
                        delta->z = 0;
                    break;
                }
            }
        }
    }
    return r;
}

/* 0x458920 - move by delta in two halves: the first half from the current position, the second from where the
 * first left it (passed as the start position), so a martian moving fast still meets what is between. `delta`
 * comes back holding the position reached minus the object's own position. The second result overwrites the
 * first; `resB` only shows in the frame. */
/* BYTES(dead-code): resB is never used: a slot the original frame has */
u16 InstantMartian::MoveHalves(Vec3s *delta, ContactInfo *info, u16 cutoff, u16 flags)
{
    Vec3s half;
    u16 resB;
    u16 hit;

    half.x = (s16)(delta->x >> 1);
    half.y = (s16)(delta->y >> 1);
    half.z = (s16)(delta->z >> 1);
    delta->x -= half.x;
    delta->y -= half.y;
    delta->z -= half.z;
    hit = Collide_ResolveMove(delta, info, cutoff, flags, 0, 0, 10, 0, 0);
    delta->x += pos.x;
    delta->y += pos.y;
    delta->z += pos.z;
    hit = Collide_ResolveMove(&half, info, cutoff, flags, delta, 0, 10, 0, 0);
    delta->x = (s16)(delta->x + half.x - pos.x);
    delta->y = (s16)(delta->y + half.y - pos.y);
    delta->z = (s16)(delta->z + half.z - pos.z);
    return hit;
}

/* 0x458a7d - is p inside any of the BOX..BOX5 boxes? The 1/0 of the test lives in a stack temporary behind
 * `this`, so the && chain is materialised by comparing it with 0. */
s32 InstantMartian::InsideRunBoxes(Vec3s p)
{
    u16 i;
    Box *box;

    for (i = 0; i < boxCount; i++) {
        box = boxes[i];
        if ((p.x >= box->min[0] && p.x <= box->max[0] && p.y >= box->min[1] && p.y <= box->max[1] &&
             p.z >= box->min[2] && p.z <= box->max[2]) != 0)
            return 1;
    }
    return 0;
}

/* 0x458b45 - the class factory for CLASSID 145 "InstantMartian": new InstantMartian (the base vtables in turn,
 * then InstantMartian's), then ScnMobile_Init(record, 0) through vtable slot +0x20. */
ScnObject *InstantMartian_Create(void *record)
{
    InstantMartian *obj = new InstantMartian;
    obj = (InstantMartian *)obj->Init(record, 0); /* cast kept: Init returns the object it was called on */
    return obj;
}
