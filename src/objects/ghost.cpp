/*
 * Ghost (class 120 "Ghost", vtable 0x574ea0, sizeof 0x1c0) - the ghosts that patrol the haunted maze of Level 12.
 * SheepD3D.exe 0x44b130-0x44ebdf, the whole file (Elmer_Create ends at 0x44b12f, Gossamer_Boss_Create starts at
 * 0x44ebe0). Designer properties (GhostProps): ACTIVATIONBOX +0, MASTER +4.
 *
 * How it works. The maze is a graph: the one ghost whose MASTER property is set builds a shared node table
 * (g_ghostNodes) out of the trajectory id-list 0x99 in Ghost_Create - each trajectory is a chain of points, and
 * consecutive points become nodes linked both ways. Every ghost then walks that graph. It keeps the node it came
 * from (curNode), the node it is heading for (nextNode) and one it must not turn back to (avoidNode), and each leg
 * is a GhostTravel record: target point, speed, approach heading, arrive radius 50.
 *  - state 2 wanders (random neighbour, 200 u/s), state 1 homes on the Wolf (the neighbour whose direction is
 *    closest to his, 400 u/s), state 3 flees him (the neighbour whose direction is furthest, 400 u/s). The
 *    Hoover's message 0x4001 sets `vulnerable`, which is what sends the ghost into state 3 and fades its tint to
 *    the blue 0xFAB46E over ten frames.
 *  - within 200 units the patrol states go to 7, the off-graph chase: straight at the Wolf at 400 u/s, giving up
 *    when he gets further away than the closest he has been (chaseRange).
 *  - within 125 units, state 4: the capture. It broadcasts 0x4003 to the other ghosts, pins the Wolf to the spot he
 *    was standing on, sends him message 0x6D, shows the embedded sheep body, starts the scripted capture camera and,
 *    after 1150 ms, tells the FireBall 0x5981 - which is what actually kills Ralph.
 *  - in the ghost costume the Wolf answers 0x412; then the ghost boos instead (state 8, sound 0x153), waits for his
 *    boo back (message 0x70 -> state 9), and after three of them captures him anyway.
 *  - the Hoover's suck is message 0, accepted only from a class-131 sender: state 5 plays sound 0xAA and animation 8,
 *    and at the end of it the ghost tells the Hoover 0x4583 and hides itself.
 * The Ghost fields and the GhostNode / GhostTravel structs are in data/structs; the nine non-virtual helpers and the
 * three globals (g_ghostNodes, g_ghostNodeCount, g_ghostNodeCountCopy) have descriptive names.
 *
 * Devices that only pin the original code generation (the inline helpers have no bodies of their own in the exe,
 * so their names are not recovered):
 *  - SetVisible / SetInstFlag: a constant argument tested as `xor r,r; test`, and the instance flag word reached
 *    through a pointer local declared inside each branch, with the mask materialised in a register.
 *  - SetTint sets the colour, the strength and the tint-override flag in ONE inline. That is what puts its
 *    colour temp BELOW the two flag pointers on the stack: an inline's argument temp is unnamed and comes after
 *    the named locals of the same expansion, so splitting it into two calls puts the temp first.
 *  - PlayAnim its option word in a stack slot; AnimFlags / GetClassId their masked temp; Facing / SetFacing
 *    their 2-byte temps; SoundIsPlaying / StopSound / PlaySound the u16 handle temps; Box_ContainsPointXZ its
 *    two arguments and its value in temps; Scn_GetPropU32 its offset in a temp.
 *  - ABS is a conditional expression, not an inline function: its value lands in an UNNAMED temp (after `this`),
 *    which an inline's named local would not (seven of them in Ghost::PickNextNode).
 *  - Ghost::BuildNodeGraph, PickNextNode, StartCaptureCamera and StartChaseCamera group their locals in one `w`
 *    struct, which pins the frame by the struct's layout instead of by a dozen local names. Ghost::Update cannot:
 *    VC6 rounds a struct local's frame slot up to 8, and the original's block ends at ebp-3, so its ten locals are
 *    named for the slots they give (tools/vc6_locals.py). None of that is a claim about the original spelling.
 *  - Every `break` inside a case body is its own: the original jumps straight to the end of the switch from each
 *    branch, not through the end of an if/else chain.
 */
/* BYTES: inline, slot-group, slot-name. */
/* BYTES(inline): SetInstFlag (member-macro inline): source-only inline: the flag word goes through a pointer local, the mask in a register */

#define SDW_MEMBERS_ScnObject                                                                                       \
    static void *operator new(u32 size);                                                                            \
    void SetFacing(s16 f);                                                                                          \
    /* inline: the tint colour, its strength and the instance's tint-override flag in one. A non-constant colour  \
     * gets a temp of its own, and it is allocated after the two flag-pointer locals. */ \
    void SetTint(u32 rgb, s32 tinted)                                                                               \
    {                                                                                                               \
        tintColor = rgb;                                                                                            \
        tintAmount = 0x1000;                                                                                        \
        SetInstFlag(INST_F_TINT, tinted);                                                                           \
    }

#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class ScnObject;
class Instance;
struct Animator;
u32 Anim_Start(Instance *inst, Animator *animator, u16 id, u32 opts);        /* 0x550196 */
u16 Sound_Play(u16 soundId, void *owner, u16 volume, u8 priority, s32 rate); /* 0x5491b8 */
#include "../engine/sound_mgr.h"
#include "fireball.h"
#include "../app/app_main.h"
#include "../engine/scn_tools.h"
#include "../engine/scenaric.h"
#include "../engine/id_list.h"
#include "animation.h"
#include "../engine/approach.h"
#include "camera.h"
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#define SDW_INLINE_SCNOBJECT_FACING 1
#define SDW_INLINE_SCNOBJECT_SETFACING_S16 1
#define SDW_INLINE_SCNOBJECT_STOPSOUND_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_FACING
#undef SDW_INLINE_SCNOBJECT_SETFACING_S16
#undef SDW_INLINE_SCNOBJECT_STOPSOUND_U16
#define SDW_INLINE_SCNOBJECT_PLAYSOUND_U16_U16_U8_S32 1
#define SDW_INLINE_SCNOBJECT_SETVISIBLE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_PLAYSOUND_U16_U16_U8_S32
#undef SDW_INLINE_SCNOBJECT_SETVISIBLE_S32
#define SDW_INLINE_SCNOBJECT_SETINSTFLAG_U16_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETINSTFLAG_U16_S32
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16

extern Wolf *g_pWolf;
/* cast kept: a view macro: this file reads g_pWolf as a plain ScnObject * */
#define g_pWolf ((ScnObject *)g_pWolf) /* 0x6cf310 */

#define g_camPos (g_camera.pos) /* 0x584d20 */

extern s32 g_dtMs;
/* cast kept: a view macro: this file reads the s32 g_dtMs as a u32 */
#define g_dtMs (*(u32 *)&g_dtMs)     /* 0x71b2e8 */
extern "C" s16 g_sinTable4096[5122]; /* 0x57ece0 */
extern "C" const s16 *g_pCosTable;   /* 0x5814e4 */

/* The shared maze graph, built once by the MASTER ghost (Ghost::BuildNodeGraph). */
/* Recovered T079 .bss owner, 0x6cc8f0..0x6cccec. Aggregate spelling is not recovered. */
struct GhostGraphStorage {
    u16 countCopy; /* +0x000 */
    u8 pad002[6];
    GhostNode nodes[36]; /* +0x008 */
    u16 count;           /* +0x3f8 */
    u8 pad3fa[2];
};
GhostGraphStorage g_ghostGraphStorage;
#define g_ghostNodes (g_ghostGraphStorage.nodes)
#define g_ghostNodeCount (g_ghostGraphStorage.count)
#define g_ghostNodeCountCopy (g_ghostGraphStorage.countCopy)

s32 Scenaric_FindByClass(u16 classId, ScnObject **out, s32 max); /* 0x5145c5 */
s32 Vec3s_DistXZ(Vec3s *a, Vec3s *b);                            /* 0x5157bd */
s32 Vec3s_DistSqXZ(Vec3s *a, Vec3s *b);                          /* 0x51588e */
s32 Vec3s_Dist(Vec3s *a, Vec3s *b);                              /* 0x515813 */
s32 Rand_Bounded(s32 bound);                                     /* 0x561219 */
extern "C" s16 Math_RadiansToAngle4096(float radians);           /* 0x5269ce */
#include "../sdk/crt.h"
void Camera_StartScripted(ScnObject *owner, Camera *cam, u16 rotX, u16 rotY, u16 rotZ, Vec3s *pos, u16 focal, u32 mode,
                          s32 time); /* 0x55a70d */

/* ---- inline helpers (descriptive names; no bodies of their own in the exe) ---- */

/* A u32 designer property: the record's property block starts at +0x14. The offset gets a stack temp of its own. */
#define SDW_INLINE_FREE_SCN_GETPROPU32_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_SCN_GETPROPU32_VOID_U32

/* Whether p lies inside box on x and z (the vertical is not tested): both arguments and the value in temps. */
#define SDW_INLINE_FREE_BOX_CONTAINSPOINTXZ_BOX_VEC3S 1
#include "../engine/coll_box_inlines.h"
#undef SDW_INLINE_FREE_BOX_CONTAINSPOINTXZ_BOX_VEC3S

#define SDW_INLINE_FREE_SOUNDISPLAYING_U16 1
#include "../engine/sound_mgr_inlines.h"
#undef SDW_INLINE_FREE_SOUNDISPLAYING_U16

/* |v| as a conditional expression: it lands in an unnamed compiler temp of its own at every use (seven of them
 * in Ghost::PickNextNode). */
#define ABS(v) ((v) >= 0 ? (v) : -(v))

/* ---- Ghost ---- */

/* 0x44b130 - vtable +0x00: the activation box, the other ghosts and the Hoover, the patrol start node, the tint
 * bookkeeping, the position snap to that node, the captured-sheep body from export 0xBF, and state 0. */
void Ghost::PostLoadInit()
{
    void *props;
    props = record;
    activationBox = Scn_GetPropBox(props, 0); /* PROPERTY_GHOST_ACTIVATIONBOX */
    ghostCount = (u16)Scenaric_FindByClass(CLASSID_GHOST, ghosts, 15);
    Scenaric_FindByClass(CLASSID_HOOVER, &hoover, 1);
    rethink = 0;
    curNode = FindNearestNode(pos);
    nextNode = 0;
    avoidNode = GHOST_NODE_NONE;
    beingSucked = 0;
    vulnerable = 0;
    showCapturedSheep = 0;
    cameraArmed = 1;
    booCount = 0;
    fadeDir = 0;
    turnRate = 0;
    wolfInBox = Box_ContainsPointXZ(activationBox, &g_pWolf->pos);
    wolfBooed = 0;
    unkCC = 0;
    baseR = tintBytes[0];
    baseG = tintBytes[1];
    baseB = tintBytes[2];
    stepR = (s16)(0x6e - tintBytes[0]);
    stepG = (s16)(0xb4 - tintBytes[1]);
    stepB = (s16)(0xfa - tintBytes[2]);
    tintR = tintBytes[0];
    tintG = tintBytes[1];
    tintB = tintBytes[2];
    homePos.x = g_ghostNodes[curNode].pos.x;
    homePos.y = g_ghostNodes[curNode].pos.y;
    homePos.z = g_ghostNodes[curNode].pos.z;
    SetPosition(&homePos);
    /* cast kept: Scn_BuildRecordFromExport writes the ScnRecordSynth through a u16 *, as it is declared */
    if (Scn_BuildRecordFromExport(WAR_IDO_AFANTB01, (u16 *)&capturedSheepRecord, 0, 0)) {
        capturedSheep.Init(&capturedSheepRecord, 0);
        hasCapturedSheep = 1;
    } else {
        hasCapturedSheep = 0;
    }
    soundHandle = 0;
    SetState(GHOST_ST_IDLE);
}

/* 0x44b441 - vtable +0x14: back to the start node, the base tint and state 0 on a level restart. */
void Ghost::Reset()
{
    beingSucked = 0;
    unkCC = 0;
    vulnerable = 0;
    showCapturedSheep = 0;
    cameraArmed = 1;
    SetPosition(&homePos);
    curNode = FindNearestNode(pos);
    nextNode = 0;
    avoidNode = GHOST_NODE_NONE;
    wolfInBox = Box_ContainsPointXZ(activationBox, &g_pWolf->pos);
    wolfBooed = 0;
    booCount = 0;
    fadeDir = 0;
    turnRate = 0;
    SetTint(0, 0);
    tintBytes[0] = baseR;
    tintR = tintBytes[0];
    tintBytes[1] = baseG;
    tintG = tintBytes[1];
    tintBytes[2] = baseB;
    tintB = tintBytes[2];
    soundHandle = 0;
    SetState(GHOST_ST_IDLE);
}

/* 0x44b653 - vtable +0x04: distance to the Wolf, the proximity broadcast, the vulnerability tint fade, then the
 * per-state body. */
/* BYTES(slot-name): names chosen for their stack slots (a struct cannot be used: VC6 rounds it to 8 and the block ends at ebp-3) */
void Ghost::Update()
{
    /* Local names are chosen for the stack slots they give (tools/vc6_locals.py), not for their spelling. */
    Vec3s wdir2;
    Vec3s rev;
    u8 i;
    Vec3s wolfPos;
    s32 dist2;
    Vec3s vv;
    Vec3s shift;
    Vec3s d;
    u16 heading;
    Vec3s wdir;

    wolfPos.x = g_pWolf->pos.x;
    wolfPos.y = g_pWolf->pos.y;
    wolfPos.z = g_pWolf->pos.z;
    wolfDist = Vec3s_DistXZ(&wolfPos, &pos);
    if (wolfBooed) {
        wolfBooed = g_pWolf->HandleMessage(this, MSG_WOLF_IS_GHOSTCOSTUME, 0);
        if (wolfBooed)
            wolfDist = 3000;
    }
    if (!rethink) {
        for (i = 0; i < ghostCount; i++) {
            dist2 = Vec3s_DistSqXZ(&ghosts[i]->pos, &pos);
            if ((dist2 != 0) & (dist2 < 0x15f90))
                ghosts[i]->HandleMessage(this, MSG_GHOST_RETHINK, 0);
        }
    }
    if (fadeDir) {
        tintR = (u8)(tintR + fadeDir * stepR / 10);
        tintG = (u8)(tintG + fadeDir * stepG / 10);
        tintB = (u8)(tintB + fadeDir * stepB / 10);
        fadeSteps--;
        tintScratch = tintB;
        tintScratch <<= 8;
        tintScratch |= tintG;
        tintScratch <<= 8;
        tintScratch |= tintR;
        SetTint(tintScratch, 1);
        if (fadeSteps == 0) {
            if (fadeDir == 1) {
                tintScratch = 0xfa;
                tintScratch <<= 8;
                tintScratch |= 0xb4;
                tintScratch <<= 8;
                tintScratch |= 0x6e;
                SetTint(tintScratch, 1);
                tintR = 0x6e;
                tintG = 0xb4;
                tintB = 0xfa;
            } else {
                SetTint(0, 0);
                tintBytes[0] = baseR;
                tintR = tintBytes[0];
                tintBytes[1] = baseG;
                tintG = tintBytes[1];
                tintBytes[2] = baseB;
                tintB = tintBytes[2];
            }
            fadeDir = 0;
        }
    }
    switch (state) {
        case GHOST_ST_IDLE: /* idle: pick the first leg once the level is running */
            if (!beingSucked) {
                if (vulnerable) {
                    nextNode = PickNextNode(curNode, wolfPos, 0, 1);
                    SetState(GHOST_ST_FLEE);
                    break;
                }
                if (wolfDist < 1500) {
                    nextNode = PickNextNode(curNode, wolfPos, 0, 0);
                    SetState(GHOST_ST_WANDER_FAST);
                    break;
                }
                nextNode = PickNextNode(curNode, wolfPos, 1, 0);
                SetState(GHOST_ST_WANDER_SLOW);
                break;
            }
            break;
        case GHOST_ST_WANDER_SLOW: /* wander: a random neighbour at 200 u/s */
            if (CheckWolfProximity())
                break;
            if (StepTravel(travel, &vv, &heading)) {
                avoidNode = curNode;
                curNode = nextNode;
                if (vulnerable) {
                    nextNode = PickNextNode(curNode, wolfPos, 0, 1);
                    SetState(GHOST_ST_FLEE);
                    break;
                }
                if (wolfDist < 1500) {
                    nextNode = PickNextNode(curNode, wolfPos, 0, 0);
                    SetState(GHOST_ST_WANDER_FAST);
                    break;
                }
                nextNode = PickNextNode(curNode, wolfPos, 1, 0);
                SetTravel(&travel, g_ghostNodes[curNode].pos, g_ghostNodes[nextNode].pos, 200);
                break;
            }
            Vec3s_ScaleByDt(&vv, &shift);
            Translate(&shift);
            SetFacing(TurnToward(heading));
            break;
        case GHOST_ST_WANDER_FAST: /* home in: the neighbour pointing most at the Wolf, 400 u/s */
            if (CheckWolfProximity())
                break;
            if (StepTravel(travel, &vv, &heading)) {
                avoidNode = curNode;
                curNode = nextNode;
                if (vulnerable) {
                    nextNode = PickNextNode(curNode, wolfPos, 0, 1);
                    SetState(GHOST_ST_FLEE);
                    break;
                }
                if (wolfDist < 1500) {
                    nextNode = PickNextNode(curNode, wolfPos, 0, 0);
                    SetTravel(&travel, g_ghostNodes[curNode].pos, g_ghostNodes[nextNode].pos, 400);
                    break;
                }
                nextNode = PickNextNode(curNode, wolfPos, 1, 0);
                SetState(GHOST_ST_WANDER_SLOW);
                break;
            }
            Vec3s_ScaleByDt(&vv, &shift);
            Translate(&shift);
            SetFacing(TurnToward(heading));
            prevState = GHOST_ST_WANDER_FAST;
            break;
        case GHOST_ST_FLEE: /* flee the vacuum: the neighbour pointing least at the Wolf */
            if (StepTravel(travel, &vv, &heading)) {
                avoidNode = curNode;
                curNode = nextNode;
                if (vulnerable) {
                    nextNode = PickNextNode(curNode, wolfPos, 0, 1);
                    SetTravel(&travel, g_ghostNodes[curNode].pos, g_ghostNodes[nextNode].pos, 400);
                    break;
                }
                if (wolfDist < 1500) {
                    nextNode = PickNextNode(curNode, wolfPos, 0, 0);
                    SetState(GHOST_ST_WANDER_FAST);
                    break;
                }
                nextNode = PickNextNode(curNode, wolfPos, 1, 0);
                SetState(GHOST_ST_WANDER_SLOW);
                break;
            }
            Vec3s_ScaleByDt(&vv, &shift);
            Translate(&shift);
            SetFacing(TurnToward(heading));
            break;
        case GHOST_ST_CATCH_WOLF: /* capture: hold him on the spot until the FireBall arrives */
            fireballTimerMs -= g_dtMs;
            if (fireballTimerMs <= 0)
                g_pFireBall->HandleMessage(this, MSG_FIREBALL_STRIKE, g_pWolf);
            if (AnimFlags(ANIM_F_FINISHED)) {
                SetState(GHOST_ST_RECOVER);
                break;
            }
            if (wolfDist < 0x7d) {
                shift.x = (s16)(g_pWolf->pos.x - capturePos.x);
                shift.y = (s16)(g_pWolf->pos.y - capturePos.y);
                shift.z = (s16)(g_pWolf->pos.z - capturePos.z);
                shift.y = 0;
                if (shift.x == 0 && shift.y == 0) {
                    wdir2.x = (s16)(g_pWolf->pos.x - pos.x);
                    wdir2.y = (s16)(g_pWolf->pos.y - pos.y);
                    wdir2.z = (s16)(g_pWolf->pos.z - pos.z);
                    rev.x = (s16)-wdir2.x;
                    rev.y = (s16)-wdir2.y;
                    rev.z = (s16)-wdir2.z;
                    vv.x = (s16)(rev.x * 400 / wolfDist);
                    vv.y = 0;
                    vv.z = (s16)(rev.z * 400 / wolfDist);
                    Vec3s_ScaleByDt(&vv, &shift);
                }
                Collide_ResolveMove(&shift, 0, 0xb54, COLL_WALL, 0, 0, 10, 0, 0);
                Translate(&shift);
            }
            capturePos = g_pWolf->pos;
            d.x = (s16)(capturePos.x - pos.x);
            d.y = (s16)(capturePos.y - pos.y);
            d.z = (s16)(capturePos.z - pos.z);
            SetFacing((s16)(Math_RadiansToAngle4096((float)atan2((double)d.x, (double)d.z)) & 0xfff) + 0x800);
            if (hasCapturedSheep) {
                capturedSheep.pos = pos;
                capturedSheep.AdvanceAnim();
                capturedSheep.rot.y = Facing();
            }
            break;
        case GHOST_ST_CHASE: /* chase: straight at him, off the graph */
            if (vulnerable) {
                SetTravel(&travel, pos, g_ghostNodes[nextNode].pos, 400);
                state = GHOST_ST_FLEE;
                PlayAnim(AFANTO01_ANIM_WALK, 1, 1);
                break;
            }
            if (wolfDist <= 0x7d) {
                if (!g_pWolf->HandleMessage(this, MSG_WOLF_IS_DEAD, 0)) {
                    SetState(GHOST_ST_CATCH_WOLF);
                    break;
                }
            } else if (wolfDist > 0xc8 || chaseRange < wolfDist) {
                SetTravel(&travel, pos, g_ghostNodes[nextNode].pos, 400);
                state = GHOST_ST_WANDER_FAST;
                PlayAnim(AFANTO01_ANIM_WALK, 1, 1);
                break;
            }
            d.x = (s16)(g_pWolf->pos.x - pos.x);
            d.y = (s16)(g_pWolf->pos.y - pos.y);
            d.z = (s16)(g_pWolf->pos.z - pos.z);
            vv.x = (s16)(d.x * 400 / wolfDist);
            vv.y = 0;
            vv.z = (s16)(d.z * 400 / wolfDist);
            Vec3s_ScaleByDt(&vv, &shift);
            Translate(&shift);
            SetFacing((s16)(Math_RadiansToAngle4096((float)atan2((double)d.x, (double)d.z)) & 0xfff) + 0x800);
            chaseRange = wolfDist;
            break;
        case GHOST_ST_SUCKED: /* sucked into the Hoover */
            if (AnimFlags(ANIM_F_FINISHED)) {
                if (::SoundIsPlaying(soundHandle))
                    StopSound(soundHandle);
                hoover->HandleMessage(this, MSG_HOOVER_GHOST_CAUGHT, 0);
                SetVisible(0);
                SetState(GHOST_ST_IDLE);
            }
            break;
        case GHOST_ST_BOO: /* boo at the costume */
            if (!wolfBooed && wolfDist > 400) {
                cameraArmed = 0;
                if (::SoundIsPlaying(soundHandle))
                    StopSound(soundHandle);
                SetState(GHOST_ST_CATCH_WOLF);
                break;
            }
            if (!g_pWolf->HandleMessage(this, MSG_WOLF_IS_GHOSTCOSTUME, 0)) {
                cameraArmed = 0;
                if (::SoundIsPlaying(soundHandle))
                    StopSound(soundHandle);
                SetState(GHOST_ST_CATCH_WOLF);
                break;
            }
            wdir.x = (s16)(g_pWolf->pos.x - pos.x);
            wdir.y = (s16)(g_pWolf->pos.y - pos.y);
            wdir.z = (s16)(g_pWolf->pos.z - pos.z);
            heading = (u16)((Math_RadiansToAngle4096((float)atan2((double)wdir.x, (double)wdir.z)) + 0x800) & 0xfff);
            SetFacing(TurnToward(heading));
            if (AnimFlags(ANIM_F_FINISHED)) {
                if (::SoundIsPlaying(soundHandle))
                    StopSound(soundHandle);
                SetState(GHOST_ST_BOOED);
            }
            StartChaseCamera(0);
            break;
        case GHOST_ST_BOOED: /* wait for his boo back */
            if (!g_pWolf->HandleMessage(this, MSG_WOLF_IS_GHOSTCOSTUME, 0)) {
                cameraArmed = 0;
                SetState(GHOST_ST_CATCH_WOLF);
                break;
            }
            if (AnimFlags(ANIM_F_FINISHED)) {
                if (wolfBooed) {
                    SetTravel(&travel, pos, g_ghostNodes[nextNode].pos, 400);
                    PlayAnim(AFANTO01_ANIM_WALK, 1, 1);
                    Camera_ReleaseScripted(this);
                    state = GHOST_ST_WANDER_SLOW;
                    break;
                }
                SetState(GHOST_ST_BOO);
                break;
            }
            if (!wolfBooed && wolfDist > 400) {
                cameraArmed = 0;
                SetState(GHOST_ST_CATCH_WOLF);
                break;
            }
            wdir.x = (s16)(g_pWolf->pos.x - pos.x);
            wdir.y = (s16)(g_pWolf->pos.y - pos.y);
            wdir.z = (s16)(g_pWolf->pos.z - pos.z);
            heading = (u16)((Math_RadiansToAngle4096((float)atan2((double)wdir.x, (double)wdir.z)) + 0x800) & 0xfff);
            SetFacing(TurnToward(heading));
            StartChaseCamera(0);
            break;
    }
    AdvanceAnim();
}

/* 0x44cfdc - vtable +0x10. */
s32 Ghost::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    s16 facing;
    u16 senderClass;
    Vec3s d;

    switch (msgId) {
        case MSG_GHOST_GET_SUCK_MS: /* how long does the capture animation last? */
            /* cast kept: MSG_GHOST_GET_SUCK_MS's arg points at the u32 answer */
            *(u32 *)arg = Anim_GetDurationMs(Inst(), AFANTO01_ANIM_STANDV1, 0);
            break;
        case MSG_GHOST_BOO: /* the Wolf booed back */
            if (state == GHOST_ST_BOOED || state == GHOST_ST_BOO) {
                wolfBooed = 1;
                booCount = 0;
            }
            break;
        case MSG_GHOST_SET_VULNERABLE: /* the Hoover arms or disarms: fade to blue and back */
            if (state != GHOST_ST_CATCH_WOLF) {
                /* cast kept (both reads): MSG_GHOST_SET_VULNERABLE's arg is a flag, a number in the void * */
                if (vulnerable != (s32)arg)
                    arg ? (fadeDir = 1) : (fadeDir = -1);
                vulnerable = (s32)arg;
                fadeSteps = 10;
                avoidNode = GHOST_NODE_NONE;
                return 1;
            }
            return 0;
        case MSG_GHOST_RETHINK: /* another ghost is close: take the runner-up neighbour next time */
            rethink = 1;
            break;
        case MSG_FREEZE:
            unkCC = 0;
            return 1;
        case MSG_KILL: /* the Hoover sucks the ghost in */
            senderClass = sender->GetClassId();
            if (senderClass == CLASSID_HOOVER && (vulnerable & (beingSucked == 0))) {
                beingSucked = 1;
                d.x = (s16)(pos.x - g_pWolf->pos.x);
                d.y = (s16)(pos.y - g_pWolf->pos.y);
                d.z = (s16)(pos.z - g_pWolf->pos.z);
                facing = (s16)(Math_RadiansToAngle4096((float)atan2((double)d.x, (double)d.z)) & 0xfff);
                SetFacing(facing);
                SetState(GHOST_ST_SUCKED);
                return 1;
            }
            break;
        case MSG_GHOST_RECOVER: /* another ghost captured him: stop */
            if (state != GHOST_ST_CATCH_WOLF)
                SetState(GHOST_ST_RECOVER);
            break;
    }
    return 0;
}

/* 0x44d255: the state entry. */
void Ghost::SetState(u8 newState)
{
    prevState = state;
    state = newState;
    switch (state) {
        case GHOST_ST_IDLE: /* idle: hover in place */
            PlayAnim(AFANTO01_ANIM_STAND, 1, 1);
            if (!beingSucked)
                SetVisible(1);
            break;
        case GHOST_ST_SUCKED: /* sucked into the Hoover */
            if (::SoundIsPlaying(soundHandle))
                StopSound(soundHandle);
            soundHandle =
                PlaySound(SND_GHOST_SUCKED, 0xff, SNDF_POSITIONAL | SNDF_DIST_HORIZONTAL | SNDF_NO_RETRIGGER, 0x1000);
            PlayAnim(AFANTO01_ANIM_STANDV1, 0, 1);
            break;
        case GHOST_ST_WANDER_FAST: /* home in on the Wolf */
            SetTravel(&travel, g_ghostNodes[curNode].pos, g_ghostNodes[nextNode].pos, 400);
            PlayAnim(AFANTO01_ANIM_WALK, 1, 1);
            break;
        case GHOST_ST_WANDER_SLOW: /* wander */
            SetTravel(&travel, g_ghostNodes[curNode].pos, g_ghostNodes[nextNode].pos, 200);
            PlayAnim(AFANTO01_ANIM_WALK, 1, 1);
            break;
        case GHOST_ST_FLEE: /* flee the vacuum */
            SetTravel(&travel, g_ghostNodes[curNode].pos, g_ghostNodes[nextNode].pos, 400);
            PlayAnim(AFANTO01_ANIM_WALK, 1, 1);
            break;
        case GHOST_ST_CHASE: /* chase him off the graph */
            PlayAnim(AFANTO01_ANIM_WALK, 1, 1);
            chaseRange = 99999;
            break;
        case GHOST_ST_CATCH_WOLF: { /* capture */
            s8 i;
            for (i = 0; i < ghostCount; i++)
                ghosts[i]->HandleMessage(this, MSG_GHOST_RECOVER, 0);
            fireballTimerMs = 0x47e;
            capturePos = g_pWolf->pos;
            g_pWolf->HandleMessage(this, MSG_SCARE, 0);
            PlayAnim(AFANTO01_ANIM_KILL, 0, 1);
            if (hasCapturedSheep) {
                capturedSheep.pos = pos;
                capturedSheep.PlayAnim(AFANTB01_ANIM_KILL, 0, 1);
                showCapturedSheep = 1;
            }
            if (cameraArmed)
                StartCaptureCamera();
            cameraArmed = 1;
            break;
        }
        case GHOST_ST_BOO: /* boo at the Wolf in the ghost costume */
            PlayAnim(AFANTO01_ANIM_HOO, 0, 1);
            soundHandle = PlaySound(SND_GHOST_BOO, 0xff, SNDF_LOOP | SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 0x1000);
            wolfBooed = 0;
            break;
        case GHOST_ST_BOOED: /* wait for his boo back; three of them and he is caught anyway */
            PlayAnim(AFANTO01_ANIM_STAND, 0, 1);
            booCount++;
            if (booCount >= 3) {
                cameraArmed = 0;
                SetState(GHOST_ST_CATCH_WOLF);
            }
            break;
        case GHOST_ST_RECOVER: /* finished */
            PlayAnim(AFANTO01_ANIM_STAND, 1, 1);
            showCapturedSheep = 0;
            break;
    }
}

/* 0x44d917 - vtable +0x08: the ghost, plus the sheep it is carrying off while the capture animation runs. */
void Ghost::Render(Camera *view)
{
    ScnBody::Render(view);
    if (showCapturedSheep && state == GHOST_ST_CATCH_WOLF)
        capturedSheep.Render(view);
}

/* 0x44d964 - the class factory for CLASSID 120 "Ghost". The MASTER ghost also builds the shared node graph. */
ScnObject *Ghost_Create(void *record)
{
    Ghost *obj;
    void *props;
    obj = new Ghost;
    obj = (Ghost *)obj->Init(record, 0); /* cast kept: Init returns the object as its ScnBody base */
    props = obj->record;
    obj->master = Scn_GetPropU32(props, 4); /* PROPERTY_GHOST_MASTER */
    if (obj->master)
        obj->BuildNodeGraph(WAR_IDO_GHOSTTRAJ);
    return obj;
}

/* 0x44da32: the capture camera, used when the Wolf is caught inside the current patrol segment. It looks along the
 * segment from 400 units behind the mid-point, 450 above the floor, at whichever of the ghost and the Wolf is
 * further from that eye. Off the segment it falls back to the plain chase camera. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; pad08, pad1e, pad26, pad2c, pad3c, pad46 fill gaps */
void Ghost::StartCaptureCamera()
{
    /* One struct, so that the whole frame is pinned by its layout rather than by twenty local names. */
    struct {
        Vec3s *wolfPos;
        s16 pitch;
        s16 yaw;
        u8 pad08[4];
        s32 sqi;
        s32 sqj;
        s32 sqk;
        Vec3s eye;
        u8 pad1e[6];
        s16 segLeft;
        u8 pad26[2];
        s16 segBottom;
        s16 segRight;
        u8 pad2c[2];
        s16 segTop;
        s32 di;
        s32 dj;
        s32 dk;
        u8 pad3c[2];
        s16 nearNode;
        Vec3s d;
        u8 pad46[2];
        Vec3s subject;
    } w;

    if (g_ghostNodes[nextNode].pos.x < g_ghostNodes[curNode].pos.x) {
        w.segRight = (s16)(g_ghostNodes[curNode].pos.x + 100);
        w.segLeft = (s16)(g_ghostNodes[nextNode].pos.x - 100);
    } else {
        w.segRight = (s16)(g_ghostNodes[nextNode].pos.x + 100);
        w.segLeft = (s16)(g_ghostNodes[curNode].pos.x - 100);
    }
    if (g_ghostNodes[nextNode].pos.z < g_ghostNodes[curNode].pos.z) {
        w.segTop = (s16)(g_ghostNodes[curNode].pos.z + 100);
        w.segBottom = (s16)(g_ghostNodes[nextNode].pos.z - 100);
    } else {
        w.segTop = (s16)(g_ghostNodes[nextNode].pos.z + 100);
        w.segBottom = (s16)(g_ghostNodes[curNode].pos.z - 100);
    }
    w.wolfPos = &g_pWolf->pos;
    if ((w.wolfPos->x >= w.segLeft && w.wolfPos->x <= w.segRight && w.wolfPos->z >= w.segBottom &&
         w.wolfPos->z <= w.segTop) == 0) {
        StartChaseCamera(1);
        return;
    }
    if (Vec3s_DistXZ(&g_ghostNodes[nextNode].pos, &pos) < Vec3s_DistXZ(&g_ghostNodes[curNode].pos, &pos))
        w.nearNode = curNode;
    else
        w.nearNode = nextNode;
    w.d.x = (s16)(g_ghostNodes[w.nearNode].pos.x - pos.x);
    w.d.y = (s16)(g_ghostNodes[w.nearNode].pos.y - pos.y);
    w.d.z = (s16)(g_ghostNodes[w.nearNode].pos.z - pos.z);
    w.di = w.d.x;
    w.dj = w.d.y;
    w.dk = w.d.z;
    w.sqi = w.di * w.di;
    w.sqj = w.dj * w.dj;
    w.sqk = w.dk * w.dk;
    w.yaw = (s16)((Math_RadiansToAngle4096((float)atan2((double)w.di, (double)w.dk)) + 0x800) & 0xfff);
    w.eye = pos;
    w.eye.x -= (s16)((g_sinTable4096[w.yaw] * 400) >> 12);
    w.eye.y = -450;
    w.eye.z -= (s16)((g_pCosTable[w.yaw] * 400) >> 12);
    if (Vec3s_DistXZ(&w.eye, &g_pWolf->pos) < Vec3s_DistXZ(&w.eye, &pos)) {
        w.subject.x = pos.x;
        w.subject.y = pos.y;
        w.subject.z = pos.z;
    } else {
        w.subject.x = g_pWolf->pos.x;
        w.subject.y = g_pWolf->pos.y;
        w.subject.z = g_pWolf->pos.z;
    }
    w.subject.y -= 120;
    w.d.x = (s16)(w.subject.x - w.eye.x);
    w.d.y = (s16)(w.subject.y - w.eye.y);
    w.d.z = (s16)(w.subject.z - w.eye.z);
    w.di = w.d.x;
    w.dj = w.d.y;
    w.dk = w.d.z;
    w.sqi = w.di * w.di;
    w.sqj = w.dj * w.dj;
    w.sqk = w.dk * w.dk;
    w.pitch =
        (s16)(Math_RadiansToAngle4096((float)atan2((double)w.dj, (double)(s32)sqrt((double)w.sqi + w.sqk))) & 0xfff);
    w.yaw = (s16)(Math_RadiansToAngle4096((float)atan2((double)-w.di, (double)w.dk)) & 0xfff);
    Camera_StartScripted(this, &g_camera, w.pitch, w.yaw, 0, &w.eye, 1000, CAMSCR_BLEND_IN, 0x1000);
}

/* 0x44ded8: the chase / boo camera. It looks at the mid-point between the ghost and the Wolf, from the game camera
 * (behindWolf = 0) or from the Wolf's far side (behindWolf = 1), with a focal length that closes in as the two get
 * closer (200000 / distance, capped at 1500). */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; pad10, pad1e fill gaps */
void Ghost::StartChaseCamera(s32 behindWolf)
{
    /* One struct, so that the whole frame is pinned by its layout rather than by a dozen local names. */
    struct {
        s32 sqi;
        s32 sqj;
        s32 sqk;
        s16 pitch;
        s16 yaw;
        u8 pad10[6];
        u16 focal;
        Vec3s eye;
        u8 pad1e[2];
        s32 di;
        s32 dj;
        s32 dk;
        Vec3s d;
    } w;

    w.d.x = (s16)(pos.x - g_pWolf->pos.x);
    w.d.y = (s16)(pos.y - g_pWolf->pos.y);
    w.d.z = (s16)(pos.z - g_pWolf->pos.z);
    if (behindWolf) {
        w.eye.x = (s16)(g_pWolf->pos.x - w.d.x);
        w.eye.y = (s16)(g_pWolf->pos.y - w.d.y);
        w.eye.z = (s16)(g_pWolf->pos.z - w.d.z);
        w.eye.y = (s16)(w.eye.y - 1000);
    } else {
        w.eye.x = g_camPos.x;
        w.eye.y = g_camPos.y;
        w.eye.z = g_camPos.z;
    }
    w.focal = (u16)(200000 / (Vec3s_Dist(&pos, &g_pWolf->pos) | 1));
    if (w.focal > 1500)
        w.focal = 1500;
    w.d.x = (s16)(w.d.x / 2);
    w.d.y = (s16)(w.d.y / 2);
    w.d.z = (s16)(w.d.z / 2);
    w.d.x += g_pWolf->pos.x;
    w.d.y += g_pWolf->pos.y;
    w.d.z += g_pWolf->pos.z;
    w.di = w.d.x - w.eye.x;
    w.dj = w.d.y - w.eye.y;
    w.dk = w.d.z - w.eye.z;
    w.sqi = w.di * w.di;
    w.sqj = w.dj * w.dj;
    w.sqk = w.dk * w.dk;
    w.pitch =
        (s16)(Math_RadiansToAngle4096((float)atan2((double)w.dj, (double)(s32)sqrt((double)w.sqi + w.sqk))) & 0xfff);
    w.yaw = (s16)(Math_RadiansToAngle4096((float)atan2((double)-w.di, (double)w.dk)) & 0xfff);
    Camera_StartScripted(this, &g_camera, w.pitch, w.yaw, 0, &w.eye, w.focal, 0, 0x1000);
}

/* 0x44e136: the graph node nearest to p on x and z. */
s16 Ghost::FindNearestNode(Vec3s p)
{
    s32 bestSq;
    s16 bestNode;
    s16 i;
    s32 d2;

    bestSq = 999999;
    for (i = 0; i < g_ghostNodeCount; i++) {
        d2 = Vec3s_DistSqXZ(&p, &g_ghostNodes[i].pos);
        if (d2 < bestSq) {
            bestSq = d2;
            bestNode = i;
        }
    }
    return bestNode;
}

/* 0x44e1a7: build the shared maze graph out of the trajectory id-list `trajListId`. Each trajectory of at least two
 * points contributes its points as nodes (de-duplicated on x / z) and links each consecutive pair both ways. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; pad1c fill gaps */
void Ghost::BuildNodeGraph(u16 trajListId)
{
    /* One struct, so that the whole frame is pinned by its layout rather than by eleven local names. */
    struct {
        Trajectory *traj;
        s16 pz;
        s16 px;
        u32 *lists;
        s16 found;
        u16 k;
        u16 li;
        u16 pi;
        GhostNode *node;
        GhostNode *prev;
        u16 pad1c;
        u16 count;
    } w;

    g_ghostNodeCount = 0;
    g_ghostNodeCountCopy = 0;
    w.lists = Scn_FindIdList(trajListId, &w.count);
    for (w.li = 0; w.li < w.count; w.li++) {
        /* cast kept: an id list holds record addresses as u32 words; this one lists trajectories */
        w.traj = (Trajectory *)w.lists[w.li];
        if (w.traj->count < 2)
            continue;
        w.px = w.traj->pts[0].x;
        w.pz = w.traj->pts[0].z;
        w.found = FindNodeAt(w.px, w.pz);
        if (w.found < 0) {
            w.found = g_ghostNodeCount;
            g_ghostNodeCount++;
            g_ghostNodes[w.found].linkCount = 0;
            g_ghostNodes[w.found].pos.x = w.px;
            g_ghostNodes[w.found].pos.y = 0;
            g_ghostNodes[w.found].pos.z = w.pz;
            g_ghostNodes[w.found].index = w.found;
        }
        w.prev = &g_ghostNodes[w.found];
        for (w.pi = 1; w.pi < w.traj->count; w.pi++) {
            w.px = w.traj->pts[w.pi].x;
            w.pz = w.traj->pts[w.pi].z;
            w.found = FindNodeAt(w.px, w.pz);
            if (w.found < 0) {
                w.found = g_ghostNodeCount;
                g_ghostNodeCount++;
                g_ghostNodes[w.found].linkCount = 0;
                g_ghostNodes[w.found].pos.x = w.px;
                g_ghostNodes[w.found].pos.y = 0;
                g_ghostNodes[w.found].pos.z = w.pz;
                g_ghostNodes[w.found].index = w.found;
            }
            w.node = &g_ghostNodes[w.found];
            for (w.k = 0; w.k < w.prev->linkCount; w.k++) {
                if (w.prev->links[w.k] == w.node)
                    break;
            }
            if (w.k == w.prev->linkCount) {
                w.prev->links[w.prev->linkCount] = w.node;
                w.prev->linkCount++;
                w.node->links[w.node->linkCount] = w.prev;
                w.node->linkCount++;
            }
            w.prev = w.node;
        }
    }
    g_ghostNodeCountCopy = g_ghostNodeCount;
}

/* 0x44e458: the graph node at exactly (x, z), or -1. */
s16 Ghost::FindNodeAt(s16 x, s16 z)
{
    u16 i;

    for (i = 0; i < g_ghostNodeCount; i++) {
        if (g_ghostNodes[i].pos.x == x && g_ghostNodes[i].pos.z == z)
            return i;
    }
    return -1;
}

/* 0x44e4c2: which neighbour of `node` to travel to next. With randomPick it is any of them; otherwise it is the one
 * whose direction is closest to (or, with `away`, furthest from) the direction to `target`. A neighbour that would
 * send the ghost back to avoidNode is only remembered as the last resort; `rethink` (message 0x4002 from another
 * ghost) makes it take the runner-up instead, which is how two ghosts stop following the same corridor. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; pad06, pad0e, pad12, pad1a, pad26 fill gaps */
s16 Ghost::PickNextNode(s16 node, Vec3s target, s32 randomPick, s32 away)
{
    /* One struct, so that the whole frame is pinned by its layout rather than by fourteen local names. */
    struct {
        Vec3s to;
        u8 pad06[4];
        u8 i;
        u8 fallback;
        s16 delta;
        u8 pad0e;
        u8 best;
        s16 bestDelta;
        u8 pad12;
        u8 nLinks;
        Vec3s d;
        u8 pad1a[5];
        u8 runnerUp;
        Vec3s from;
        u8 pad26[4];
        s16 targetAngle;
    } w;

    w.best = GHOST_LINK_NONE;
    w.runnerUp = GHOST_LINK_NONE;
    if (away)
        w.bestDelta = -1;
    else
        w.bestDelta = 0xfff;
    w.nLinks = g_ghostNodes[node].linkCount;
    if (randomPick) {
        if (w.nLinks > 1)
            return g_ghostNodes[node].links[Rand_Bounded(w.nLinks)]->index;
        return g_ghostNodes[node].links[0]->index;
    }
    w.from.x = g_ghostNodes[node].pos.x;
    w.from.y = g_ghostNodes[node].pos.y;
    w.from.z = g_ghostNodes[node].pos.z;
    w.d.x = (s16)(target.x - w.from.x);
    w.d.y = (s16)(target.y - w.from.y);
    w.d.z = (s16)(target.z - w.from.z);
    w.targetAngle = (s16)(Math_RadiansToAngle4096((float)atan2((double)w.d.x, (double)w.d.z)) & 0xfff);
    for (w.i = 0; w.i < w.nLinks; w.i++) {
        w.to.x = g_ghostNodes[node].links[w.i]->pos.x;
        w.to.y = g_ghostNodes[node].links[w.i]->pos.y;
        w.to.z = g_ghostNodes[node].links[w.i]->pos.z;
        w.d.x = (s16)(w.to.x - w.from.x);
        w.d.y = (s16)(w.to.y - w.from.y);
        w.d.z = (s16)(w.to.z - w.from.z);
        w.delta = (s16)(Math_RadiansToAngle4096((float)atan2((double)w.d.x, (double)w.d.z)) & 0xfff);
        w.delta = (s16)(((w.delta - w.targetAngle) + 0x800) & 0xfff) - 0x800;
        if (away) {
            if (ABS(w.delta) > w.bestDelta) {
                if (g_ghostNodes[node].links[w.i]->index == avoidNode) {
                    w.fallback = w.i;
                } else {
                    w.bestDelta = (s16)ABS(w.delta);
                    w.best = w.i;
                }
            }
        } else if (ABS(w.delta) < w.bestDelta) {
            if (prevState == GHOST_ST_WANDER_FAST) {
                if ((g_ghostNodes[node].links[w.i]->index == avoidNode) & (ABS(w.delta) > 200)) {
                    w.fallback = w.i;
                } else if (((g_ghostNodes[node].links[w.i]->index == avoidNode) & (ABS(w.delta) > 200)) &
                           (g_ghostNodes[w.i].linkCount < 2)) {
                    w.fallback = w.i;
                } else {
                    w.bestDelta = (s16)ABS(w.delta);
                    w.runnerUp = w.best;
                    w.best = w.i;
                }
            } else {
                w.bestDelta = (s16)ABS(w.delta);
                w.runnerUp = w.best;
                w.best = w.i;
            }
        }
    }
    if (rethink) {
        rethink = 0;
        if (w.runnerUp != GHOST_LINK_NONE)
            return g_ghostNodes[node].links[w.runnerUp]->index;
    }
    if (w.best == GHOST_LINK_NONE)
        return g_ghostNodes[node].links[w.fallback]->index;
    return g_ghostNodes[node].links[w.best]->index;
}

/* 0x44e947: start a travel leg from `from` to `to`, arriving within 50 units. */
void Ghost::SetTravel(GhostTravel *t, Vec3s from, Vec3s to, s16 speed)
{
    Vec3s d;

    t->target.x = to.x;
    t->target.y = to.y;
    t->target.z = to.z;
    d.x = (s16)(to.x - from.x);
    d.y = (s16)(to.y - from.y);
    d.z = (s16)(to.z - from.z);
    t->speed = speed;
    t->heading = (s16)((Math_RadiansToAngle4096((float)atan2((double)d.x, (double)d.z)) + 0x800) & 0xfff);
    t->arriveDist = 50;
}

/* 0x44ea01: the velocity and heading for this frame of the leg; 1 once the target is within arriveDist. */
s32 Ghost::StepTravel(GhostTravel t, Vec3s *outVel, u16 *outHeading)
{
    s32 relZ;
    s32 distX;
    s32 len;
    s32 done;

    done = 0;
    distX = t.target.x - pos.x;
    relZ = t.target.z - pos.z;
    len = distX * distX + relZ * relZ;
    len = (s32)sqrt((double)len);
    outVel->x = (s16)(distX * t.speed / len);
    outVel->y = 0;
    outVel->z = (s16)(relZ * t.speed / len);
    *outHeading = t.heading;
    if (len < t.arriveDist)
        done = 1;
    return done;
}

/* 0x44eab2: turn toward `target` at up to 50000 units of angular acceleration - in practice instantly. */
s16 Ghost::TurnToward(s16 target)
{
    return Math_ApproachAngle(Facing(), target, &turnRate, 50000, 50000, 50000, 1);
}

/* 0x44eaf9: the patrol states' range check. Note that it is purely horizontal: nothing tests the vertical
 * distance, so a ghost catches Ralph at any height above or below it. */
u8 Ghost::CheckWolfProximity()
{
    if (vulnerable)
        return 0;
    if (!g_pWolf->HandleMessage(this, MSG_WOLF_IS_DEAD, 0)) {
        if (wolfDist < 0x7d) {
            if (g_pWolf->HandleMessage(this, MSG_WOLF_IS_GHOSTCOSTUME, 0)) {
                StartChaseCamera(1);
                SetState(GHOST_ST_BOO);
            } else {
                SetState(GHOST_ST_CATCH_WOLF);
            }
            return 1;
        }
        if (wolfDist < 200) {
            if (g_pWolf->HandleMessage(this, MSG_WOLF_IS_GHOSTCOSTUME, 0)) {
                StartChaseCamera(1);
                SetState(GHOST_ST_BOO);
            } else {
                SetState(GHOST_ST_CHASE);
            }
            return 1;
        }
    }
    return 0;
}
