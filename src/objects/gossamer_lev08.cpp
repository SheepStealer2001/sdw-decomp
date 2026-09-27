/* T081 - original object Gossamer_Lev08.cpp (guessed name): .text 0x452210-0x455c24, .rdata 0x574ee8-0x574f10
 * (vtable, then 4 bytes of padding before the next object's 8-aligned double), .data 0x57a830-0x57a83c (the cinematic
 * header's stride-table copy). */
/* BYTES: flow, inline, layout, slot-name. */
/* BYTES(layout): the Cine.h header static: every object including the cinematic header carries this copy in its .data, referenced or not */
/* BYTES(inline): BoxOverlap4-style inline (sign-bit overlap): source-only inline: its arguments are evaluated last to first, as the original (0x4529a6) */
/*
 * Gossamer_Lev08 (class 87 "Gossamer_Lev08", vtable 0x574ee8, sizeof 0x2e4) - the orange monster of Level 8
 * (disc Lvl-09/Lvl-10, the time-machine level). SheepD3D.exe 0x452210-0x455c23, the whole file: it starts with the
 * factory on a 16-byte boundary and ends at 0x455c23, followed by int3 padding.
 *
 * What he does, in the order the code does it: PostLoadInit reads his zones (IDPASTACTIONZONE while the level is in
 * the present, IDFUTURACTIONZONE / IDFUTURACTIONZONE3 after the time portal has flipped him) and puts him in state 9
 * if he has an IDCINEBOX, else state 5. State 5 is the watch: every frame he faces Ralph, and once Ralph is inside
 * 1200 units and a probe step would stay inside his zone, he enters state 1. State 1 is the chase (ChaseWolf): he
 * walks at 400 u/s, climbs into a future zone box when Ralph is above him, and inside 130 units with less than 50 of
 * height between them he freezes Ralph (msg 0x404 / 0x40f) and hugs him - state 12 pulls Ralph to 125 units, state 4
 * counts the button mashing (UpdateStruggle: more than six presses inside 1500 ms break the hug, 800 ms without one
 * resets the count). Msg 0x39 is the time portal taking him through: it toggles inFuture, hides his shadow and plays
 * the grab animation; msg 0x3a is being carried. Msg 0x1e (from the bull) starts the second cinematic and sends him
 * down IDFLEEINGTRAJ, and from then on state 0 walks that path with a dust trail, carrying Ralph along inside the
 * box overlap.
 *
 * The helper methods (IsInActionZone, SetState, ChaseWolf, UpdateStruggle, IsWolfOnHighGround, NearestFleePoint,
 * TrajPatrol_Init/Step) are found through the symbol tables; PostLoadInit, Update, Render, HandleMessage and Reset are
 * placed by their vtable slots.
 *
 * Devices that only pin the original code generation (the inline helpers have no bodies of their own in the exe, so
 * their names are not recovered): SetVisible / EnableBoxCollide / Shadow::SetVisible take their constant through a
 * register (`mov r,1; test r,r`), SetUpdateMode is a 4-way jump table on a constant, Facing / SetFacing / GetClassId /
 * StopSound / IsActive / IsFinished each give a stack temp per use, Box_ContainsXZ puts its two arguments and its
 * value in temps, Scn_GetPropU32 takes its offset as u32 so the constant gets a temp of its own, and
 * TrajPatrol::Restart gives the `this` temp the two writes share. Local names are chosen for their stack slots
 * (tools/vc6_locals.py).
 */

#define SDW_MEMBERS_ScnObject                                                                                    \
    static void *operator new(u32 size); /* 0x50d5f4 Scenaric_Alloc: every class factory's `new` (level heap) */ \
    CollBox *GetFirstModelBox();                                                                                 \
    void SetFacing(s16 f);                                                                                       \
    void SetUpdateMode(s32 mode);                                                                                \
    /* inline: the handle is a stack temp (0x452803) */


#define SDW_MEMBERS_TrailEmitter TrailEmitter(); /* inline: the pools are the inline buffers (0x452247) */
#define SDW_MEMBERS_ZoneList    \
    /* inline, defined below */ \
    void Load(u32 id);          \
    /* inline, defined below */
#define SDW_MEMBERS_TrajPatrol                                                               \
    /* inline: restart the walk at point i; the two writes share a `this` temp (0x45414b) */ \
    void Restart(s16 i)                                                                      \
    {                                                                                        \
        forceAdvance = 1;                                                                    \
        pointIndex = i;                                                                      \
    }                                                                                        \
    /* inline: likewise (0x452bee) */

#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *inst, Animator *animator, u16 id, u32 opts); /* 0x550196 */
#include "../engine/sound_mgr.h"
#include "../engine/id_list.h"
#include "../engine/scn_tools.h"
#include "camera.h"
#include "patrol.h"
#include "../engine/cine.h"
#include "../engine/input.h"
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#define SDW_INLINE_SCNOBJECT_FACING 1
#define SDW_INLINE_SCNOBJECT_SETFACING_S16 1
#define SDW_INLINE_SCNOBJECT_SETVISIBLE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_FACING
#undef SDW_INLINE_SCNOBJECT_SETFACING_S16
#undef SDW_INLINE_SCNOBJECT_SETVISIBLE_S32
#define SDW_INLINE_SCNOBJECT_ENABLEBOXCOLLIDE_S32 1
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32 1
#define SDW_INLINE_SCNOBJECT_STOPSOUND_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_ENABLEBOXCOLLIDE_S32
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32
#undef SDW_INLINE_SCNOBJECT_STOPSOUND_U16
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#define SDW_INLINE_SHADOW_SETVISIBLE_S32 1
#include "../engine/shadow_inlines.h"
#undef SDW_INLINE_SHADOW_SETVISIBLE_S32
#define SDW_INLINE_TRAJPATROL_REVERSE 1
#include "../engine/traj_patrol_inlines.h"
#undef SDW_INLINE_TRAJPATROL_REVERSE
#define SDW_INLINE_CINE_ISACTIVE 1
#define SDW_INLINE_CINE_ISFINISHED 1
#include "../engine/cine_inlines.h"
#undef SDW_INLINE_CINE_ISACTIVE
#undef SDW_INLINE_CINE_ISFINISHED
inline TrailEmitter::TrailEmitter()
{
    base.slotPool = slotBuf;
    base.particles = particleBuf;
    base.count = 16;
    base.Emitter_Reset();
}
/* 0x57a830 - the cinematic header's static copy of the 9-byte opcode stride table (src/engine/cine1.cpp,
 * g_cineOpStride 0x5816fc). The original header defined it static, so every object including it carries its own
 * unreferenced copy at the head of its .data; defined here in its place. */
static u8 s_cineOpStride[9] = {0, 8, 8, 4, 2, 2, 4, 2, 2};
#include "../engine/property_math.h"

#include "../sdk/crt.h"
extern "C" s16 Math_RadiansToAngle4096(float radians);                    /* 0x5269ce */
u16 Sound_Play(u16 soundId, void *owner, u16 volume, u8 flags, s32 rate); /* 0x5491b8 */
/* The two trajectory-walker helpers this class shares with the two classes after it (0x455c8e, 0x455fc1); they are a
 * copy of TrajFollower_Init / TrajFollower_Step (0x510731 / 0x51079b) with the direction and the force-advance bit
 * kept in the struct instead of in arguments. */

extern Wolf *g_pWolf; /* 0x6cf310 */
extern s32 g_dtMs;    /* 0x71b2e8  g_dt * 1000 >> 12 */

#define g_padCurButtons (g_pad.cur.buttons) /* 0x719654 */

#define g_padPrevButtons (g_pad.prev.buttons) /* 0x71964c */

/* ---- inline helpers ---- */

/* A u32 designer property: the record's property block starts at +0x14. The offset is a u32 parameter, so the
 * constant gets a stack temp of its own (0x4522d9), as in the original. */
#define SDW_INLINE_FREE_SCN_GETPROPU32_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_SCN_GETPROPU32_VOID_U32

/* The model's first collision box, or NULL: the list pointer is a temp (0x4524e9). */
inline CollBox *ScnObject::GetFirstModelBox()
{
    ModelBoxList *list = inst_model->boxes;
    if (list)
        return list->boxes;
    return 0;
}

#define SDW_INLINE_ZONELIST_LOAD_U32 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_LOAD_U32

#define SDW_INLINE_ZONELIST_CLEAR 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_CLEAR

#define SDW_INLINE_ZONELIST_FINDCONTAININGXZ_VEC3S 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_FINDCONTAININGXZ_VEC3S

/* Point in box on x and z only; both arguments go through stack temps, the point's below the box's (0x4552e3), so
 * the box is the first parameter. */
inline s32 Box_ContainsXZ(Box *b, Vec3s *p)
{
    return p->x >= b->min[0] && p->x <= b->max[0] && p->z >= b->min[2] && p->z <= b->max[2];
}

/* Cine_Start with no sheep box and no dialogue block: the four variable arguments go through stack temps
 * (0x453f0a-0x453f4c). */
inline void Cine_Play(u32 id, u32 startFlags, Box *box, void *text)
{
    g_cinePlayer.Start(id, startFlags, box, 0, text, 0);
}

/* The action button, edge-detected on release (the pad word is active-low). A macro, not an inline function: the
 * original tests it in place, with no 0/1 temporary (0x4558d2). */
#define ACTION_RELEASED() \
    ((g_padCurButtons & ~g_inputMap[INPUT_SLOT_CROSS]) == 0 && (g_padPrevButtons & ~g_inputMap[INPUT_SLOT_CROSS]) != 0)

/* The 12-bit heading from `a` toward `b`, plus half a turn (the models face backwards). A macro, not an inline
 * function: the original has no stack temporaries for the two positions (0x455536). */
#define HEADING_TO(a, b)                                                                                             \
    ((s16)(Math_RadiansToAngle4096((float)atan2((double)(b)->x - (double)(a)->x, (double)(b)->z - (double)(a)->z)) & \
           0xfff) +                                                                                                  \
     0x800)

/* Sign-bit box overlap: nonzero when all four differences are >= 0. An inline, because its arguments are evaluated
 * last to first and all four are live at once, which is the original's order (0x4529a6: the last argument is computed
 * first, into eax; the first one, in edi, takes the three ORs). As in src/game/wolf.cpp. */
#define SDW_INLINE_FREE_OVERLAP4_S32_S32_S32_S32 1
#include "../engine/coll_box_inlines.h"
#undef SDW_INLINE_FREE_OVERLAP4_S32_S32_S32_S32

/* ---- 0x452210 the class factory for CLASSID 87 "Gossamer_Lev08" ---- */

ScnObject *Gossamer_Lev08_Create(void *record)
{
    ScnBody *obj = new Gossamer_Lev08;
    obj = obj->Init(record, 0);
    return obj;
}

/* 0x4522b3 - vtable +0x00. Reads the designer properties, caches the two collision boxes the carry test uses, and
 * starts in state 9 (wait for Ralph in IDCINEBOX) when there is a cinematic box, else in state 5 (watch). */
/* BYTES(slot-name): props is declared last: it shares zoneId's bucket, and the last-declared gets EBP-4 */
void Gossamer_Lev08::PostLoadInit()
{
    u32 zoneId;
    u16 *props = record; /* declared last: props and zoneId share a hash bucket, so the last one gets EBP-4 */

    pastZone = Scn_GetPropBox(props, 0x34); /* PROPERTY_GOSSAMER_IDPASTACTIONZONE */
    zoneId = Scn_GetPropU32(props, 0x28);   /* PROPERTY_GOSSAMER_IDFUTURACTIONZONE */
    if (zoneId)
        futureZones.Load(zoneId);
    else
        futureZones.Clear();
    futureZone = Scn_GetPropBox(props, 0x30);      /* PROPERTY_GOSSAMER_IDFUTURACTIONZONE3 */
    cineBox = Scn_GetPropBox(props, 0x14);         /* PROPERTY_GOSSAMER_IDCINEBOX */
    cineBox2 = Scn_GetPropBox(props, 0x18);        /* PROPERTY_GOSSAMER_IDCINEBOX2 */
    fleeTraj = Scn_GetPropTrajectory(props, 0x24); /* PROPERTY_GOSSAMER_IDFLEEINGTRAJ */
    cineId = Scn_GetPropU32(props, 0x1c);          /* PROPERTY_GOSSAMER_IDCINEMATIC */
    cineFlags = Scn_GetPropU32(props, 0x00);       /* PROPERTY_GOSSAMER_CINEFLAGS */
    cineId2 = Scn_GetPropU32(props, 0x1c);         /* IDCINEMATIC again - IDCINEMATIC2 (0x20) is never read */
    cineFlags2 = Scn_GetPropU32(props, 0x04);      /* PROPERTY_GOSSAMER_CINEFLAGS2 */
    cineTextId = Scn_GetPropU32(props, 0x08);      /* PROPERTY_GOSSAMER_CINETEXT */
    cineText = Text_GetClassString((u8)cineTextId);
    cineText2Id = Scn_GetPropU32(props, 0x0c); /* PROPERTY_GOSSAMER_CINETEXT2 */
    cineText2 = Text_GetClassString((u8)cineText2Id);
    if (*cineText == 0)
        cineText = 0;
    if (*cineText2 == 0)
        cineText2 = 0;
    TrajPatrol_Init(&patrol, fleeTraj, 1000, 0x800, 1, 0, 0x32);
    myBox = GetFirstModelBox();
    wolfBox = g_pWolf->GetFirstModelBox();
    unusedF4 = 0;
    fleeing = 0;
    squeezeTicks = 0;
    hasFled = 0;
    inFuture = 0;
    cinePending = 0;
    SnapToGround(1);
    startPos = pos;
    hugStage = 0;
    hugging = 0;
    if (cineBox)
        SetState(GOSS08_ST_WAIT_CINE_BOX);
    else
        SetState(GOSS08_ST_APPROACH);
    SetUpdateMode(SCN_UPD_ALWAYS);
    SetUpdateMode(SCN_UPD_CINE);
    dust.base.Emitter_Reset();
    dustParams.hSpeed = 10;
    dustParams.vSpeed = -20;
    dustParams.life = 0x800;
    dustParams.spawnInterval = 0x155;
    dustParams.sizeStart = 0x28;
    dustParams.sizeEnd = 0x5a;
    dustParams.sheetIndex = 0;
    cineStage = 0;
    wolfHeld = 0;
    patrolActive = 1;
    soundHandle = 0;
}

/* 0x4527fa - vtable +0x14. A checkpoint puts him back where he started, unless he has already been sent down the
 * flee path: then he keeps the flight and only restarts its timer. */
void Gossamer_Lev08::Reset()
{
    StopSound(soundHandle);
    soundHandle = 0;
    if (fleeing == 0) {
        SetPosition(&startPos);
        unusedF4 = 0;
        fleeing = 0;
        squeezeTicks = 0;
        hasFled = 0;
        inFuture = 0;
        cinePending = 0;
        hugStage = 0;
        hugging = 0;
        SetState(GOSS08_ST_APPROACH);
    } else {
        timer = 0;
        patrolActive = 1;
        SetState(GOSS08_ST_PATROL);
    }
    SnapToGround(1);
}

/* 0x4528f2 - vtable +0x04. One switch on `state`, 7,958 bytes of it; the cases are in the original's source order
 * (0,1,3,4,5,6,7,9,10,11,12,8,13).
 *
 * What made it match: (1) the two box-overlap tests in states 0 and 7 go through the inline Overlap4, whose
 * arguments /Ob1 evaluates last to first with all four live at once - the original computes the last difference first
 * (eax, 0x4529a6), each subtrahend before its minuend, and folds into the first argument's edi; inside each sum the
 * position is written first, so the box offset (the heavier operand) is loaded first and the lea is [pos + box].
 * (2) Every branch that ends a case ends with its own `break` (the original's `jmp` to the switch end sits right
 * after the branch, not after the else chain: 0x453149, 0x453238, 0x453688, 0x4537bd, 0x453bf5, 0x453d7c, 0x453f89,
 * 0x45478f), so those if/else chains are written as `if (...) { ...; break; }` followed by the rest. */
/* BYTES(slot-name): all locals declared here in the order that gives the original frame (tools/vc6_locals.py) */
void Gossamer_Lev08::Update()
{
    /* cast kept (every (void *) message argument here): HandleMessage's arg is a void *; MSG_KILL passes the
     * WolfKillType and MSG_WOLF_GOSSAMER_SCRIPT the WolfGossamerScript step in it */
    /* Every local of the function is declared here, in the order that gives the original's frame: `nearest` and `start`
     * share nothing, but heading/fxPos and ci/home each share a hash bucket, so within a pair the one that wants the
     * higher slot is declared last (tools/vc6_locals.py). */
    u32 nearest;
    u32 start;
    Vec3s home;
    ContactInfo ci;
    Vec3s fxPos;
    s16 heading;

    switch (state) {
        case GOSS08_ST_PATROL: /* walking the flee path, carrying Ralph if he is inside the box overlap */
            if (timer < timerLimit) {
                if (patrolActive) {
                    TrajPatrol_Step(&pos, &patrol, &vel, &heading);
                    vel.y = 0x12c;
                    if (Overlap4((g_pWolf->pos.x + wolfBox->max.x + 10) - (pos.x + myBox->min.x - 10),
                                 (pos.x + myBox->max.x + 10) - (g_pWolf->pos.x + wolfBox->min.x - 10),
                                 (g_pWolf->pos.z + wolfBox->max.z + 10) - (pos.z + myBox->min.z - 10),
                                 (pos.z + myBox->max.z + 10) - (g_pWolf->pos.z + wolfBox->min.z - 10))) {
                        target.x = g_pWolf->pos.x;
                        target.z = g_pWolf->pos.z;
                        target.y = QueryGroundY(&g_pWolf->pos, 0);
                        g_pWolf->SetPosition(&target);
                        if (wolfHeld)
                            wolfHeld = !g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0);
                        /* cast kept: message arguments travel as void * */
                        g_pWolf->HandleMessage(this, MSG_KILL, (void *)KILL_CRUSH);
                    } else {
                        Vec3s_ScaleByDt(&vel, &step);
                        Collide_ResolveMove(&step, &contact, 0x578, COLL_FLOOR | COLL_FLOOR_EDGE, 0, 0, 10, 0, 0);
                        if (step.x == 0 && step.z == 0)
                            patrol.Reverse();
                        if (contact.wallObj && contact.wallObj->GetClassId() == CLASSID_SHEEP)
                            /* cast kept: message arguments travel as void * */
                            contact.wallObj->HandleMessage(this, MSG_KILL, (void *)KILL_CRUSH);
                    }
                    SetFacing(heading);
                    Translate(&step);
                    fxPos = pos;
                    fxPos.y -= 0x19;
                    dust.base.Emitter_UpdateDrift(&dustParams, &fxPos, Facing(), 1);
                }
                if (cinePending) {
                    patrol.speed = 0x4b0;
                    cinePending = 0;
                    PlayAnim(AGOSSA01_ANIM_OUCH, 0, 0);
                    SetState(GOSS08_ST_FLEE);
                    patrolActive = 1;
                }
            } else {
                patrolActive = 0;
                PlayAnim(AGOSSA01_ANIM_STAND2, 1, 0);
                timer = 0;
                timerLimit = 0x186a0;
            }
            timer += g_dtMs;
            break;
        case GOSS08_ST_CHASE: /* chasing Ralph */
            if (AnimFlags(ANIM_F_FINISHED))
                squeezeTicks = 0;
            ChaseWolf();
            if (squeezeTicks == 5 || squeezeTicks == 0x10)
                Camera_StartShake(0x28, 0x190);
            if (g_cinePlayer.IsActive())
                SetState(GOSS08_ST_CINEMATIC);
            squeezeTicks++;
            break;
        case GOSS08_ST_GO_GRAB: /* walking to the position msg 0x38 gave him */
            target.x = grabPos->x - pos.x;
            target.z = grabPos->z - pos.z;
            target.y = 0;
            distToTarget = Vec3s_DistXZ(&pos, grabPos);
            SetFacing(HEADING_TO(&pos, grabPos));
            if (distToTarget == 0)
                distToTarget = 1;
            vel.x = (s16)(target.x * 0x190 / distToTarget);
            vel.z = (s16)(target.z * 0x190 / distToTarget);
            vel.y = 0x190;
            Vec3s_ScaleByDt(&vel, &step);
            Collide_ResolveMove(&step, &ci, 0x578, RESOLVE_SLIDE_ALL, 0, 0, 10, 0, 0);
            Translate(&step);
            break;
        case GOSS08_ST_HUG: /* squeezing Ralph: he has to mash his way out */
            if (Vec3s_ManhattanDistXZ(&g_pWolf->pos, &pos) > 0x1f4) {
                wolfHeld = !g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0);
                SetState(GOSS08_ST_APPROACH);
                break;
            }
            if (hugging == 0) {
                if (ACTION_RELEASED()) {
                    mashLastMs = 0;
                    hugging = 1;
                    PlayAnim(AGOSSA01_ANIM_CATCH2, 0, 1);
                    /* cast kept: message arguments travel as void * */
                    g_pWolf->HandleMessage(this, MSG_WOLF_GOSSAMER_SCRIPT, (void *)GOSSAMER_SCRIPT_GRAB_2);
                    SetState(GOSS08_ST_FLEE);
                    hugStage = 1;
                    break;
                }
            } else if (UpdateStruggle()) {
                wolfHeld = 0;
                hugStage = 2;
                hugging = 0;
                PlayAnim(AGOSSA01_ANIM_CATCH4, 0, 1);
                /* cast kept: message arguments travel as void * */
                g_pWolf->HandleMessage(this, MSG_WOLF_GOSSAMER_SCRIPT, (void *)GOSSAMER_SCRIPT_RELEASED);
                SetState(GOSS08_ST_FLEE);
            }
            break;
        case GOSS08_ST_APPROACH: /* watching: face Ralph, and start the chase when he comes close enough */
            SetFacing(HEADING_TO(&pos, &g_pWolf->pos));
            distToTarget = Vec3s_DistXZ(&pos, &g_pWolf->pos);
            if ((u32)distToTarget < 0x4b0) {
                target.x = g_pWolf->pos.x - pos.x;
                target.z = g_pWolf->pos.z - pos.z;
                target.y = 0;
                SetFacing(HEADING_TO(&pos, &g_pWolf->pos));
                home = pos;
                if (distToTarget == 0)
                    distToTarget = 1;
                vel.x = (s16)(target.x * 0x190 / distToTarget);
                vel.z = (s16)(target.z * 0x190 / distToTarget);
                vel.y = 0;
                Vec3s_ScaleByDt(&vel, &step);
                Translate(&step);
                if (!IsInActionZone(&pos)) {
                    SetPosition(&home);
                } else {
                    SetPosition(&home);
                    SetState(GOSS08_ST_CHASE);
                }
            }
            break;
        case GOSS08_ST_GRAB_RESULT: /* being released by whatever grabbed him */
            if (grabKind == GOSS_GRAB_NONE)
                break;
            if (grabKind == GOSS_GRAB_INSIDE) {
                PlayAnim(AGOSSA01_ANIM_INSIDE, 0, 1);
                SetState(GOSS08_ST_FLEE);
                break;
            }
            if (grabKind == GOSS_GRAB_OUT) {
                shadow.SetVisible(1);
                PlayAnim(AGOSSA01_ANIM_OUT, 0, 1);
                SetVisible(1);
                EnableBoxCollide(1);
                SnapToGround(1);
                SetState(GOSS08_ST_FLEE);
                break;
            }
            break;
        case GOSS08_ST_FLEE: /* playing one animation; when it ends, prevState decides what comes next */
            if (fleeing) {
                dust.base.Emitter_UpdateDrift(&dustParams, &pos, Facing(), 0);
                TrajPatrol_Step(&pos, &patrol, &vel, &heading);
                if (Overlap4((g_pWolf->pos.x + wolfBox->max.x + 10) - (pos.x + myBox->min.x - 10),
                             (pos.x + myBox->max.x + 10) - (g_pWolf->pos.x + wolfBox->min.x - 10),
                             (g_pWolf->pos.z + wolfBox->max.z + 10) - (pos.z + myBox->min.z - 10),
                             (pos.z + myBox->max.z + 10) - (g_pWolf->pos.z + wolfBox->min.z - 10))) {
                    target.x = g_pWolf->pos.x;
                    target.z = g_pWolf->pos.z;
                    target.y = QueryGroundY(&g_pWolf->pos, 0);
                    g_pWolf->SetPosition(&target);
                    if (wolfHeld)
                        wolfHeld = !g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0);
                    /* cast kept: message arguments travel as void * */
                    g_pWolf->HandleMessage(this, MSG_KILL, (void *)KILL_CRUSH);
                } else {
                    Collide_ResolveMove(&step, &contact, 0x578, COLL_FLOOR | COLL_FLOOR_EDGE, 0, 0, 10, 0, 0);
                    if (contact.wallObj && contact.wallObj->GetClassId() == CLASSID_SHEEP)
                        /* cast kept: message arguments travel as void * */
                        contact.wallObj->HandleMessage(this, MSG_KILL, (void *)KILL_CRUSH);
                }
                Vec3s_ScaleByDt(&vel, &step);
                Collide_ResolveMove(&step, &contact, 0x578, COLL_FLOOR | COLL_FLOOR_EDGE, 0, 0, 10, 0, 0);
                SetFacing(heading);
                Translate(&step);
            }
            if (AnimFlags(ANIM_F_FINISHED)) {
                switch (prevState) {
                    case GOSS08_ST_HUG: /* the squeeze: keep squeezing, or start the 2000 ms recovery */
                        if (hugStage == 1) {
                            timer = 0;
                            PlayAnim(AGOSSA01_ANIM_CATCH3, 1, 1);
                            SetState(GOSS08_ST_HUG);
                            break;
                        }
                        wolfHeld = !g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0);
                        timer = 0;
                        timerLimit = 0x7d0;
                        SetState(GOSS08_ST_PAUSE);
                        break;
                    case GOSS08_ST_CATCH:
                        /* cast kept: message arguments travel as void * */
                        g_pWolf->HandleMessage(this, MSG_WOLF_GOSSAMER_SCRIPT, (void *)GOSSAMER_SCRIPT_GRAB_1);
                        PlayAnim(AGOSSA01_ANIM_CATCH1, 1, 1);
                        SetState(GOSS08_ST_HUG);
                        break;
                    case GOSS08_ST_GRAB_RESULT:
                        if (grabKind == GOSS_GRAB_INSIDE) {
                            SetVisible(0);
                            EnableBoxCollide(0);
                            SetState(GOSS08_ST_GRAB_RESULT);
                            grabKind = GOSS_GRAB_NONE;
                            break;
                        }
                        if (grabKind == GOSS_GRAB_OUT) {
                            EnableBoxCollide(1);
                            timer = 0;
                            timerLimit = 0x7d0;
                            SetState(GOSS08_ST_PAUSE);
                        }
                        break;
                    case GOSS08_ST_PATROL:
                        patrol.speed = 0x3e8;
                        timer = 0;
                        timerLimit = 0xfa0;
                        SetState(GOSS08_ST_PATROL);
                        break;
                    default:
                        SetState(prevState);
                        break;
                }
            }
            break;
        case GOSS08_ST_WAIT_CINE_BOX: /* waiting for Ralph to walk into IDCINEBOX */
            if (Box_ContainsXZ(cineBox, &g_pWolf->pos)) {
                cineStage++;
                Cine_Play(cineId, cineFlags, cineBox, cineText);
                SetState(GOSS08_ST_CINEMATIC);
                break;
            }
            SetFacing(HEADING_TO(&pos, &g_pWolf->pos));
            break;
        case GOSS08_ST_CINEMATIC: /* a cinematic is running */
            if (cineStage == 1) {
                if (cineBox && g_cinePlayer.IsFinished())
                    SetState(GOSS08_ST_CHASE);
            } else if (cineBox2) {
                if (g_cinePlayer.IsFinished()) {
                    if (wolfHeld) {
                        /* cast kept: message arguments travel as void * */
                        g_pWolf->HandleMessage(this, MSG_WOLF_GOSSAMER_SCRIPT, (void *)GOSSAMER_SCRIPT_IDLE);
                        wolfHeld = !g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0);
                    }
                    timer = 0;
                    timerLimit = 0xfa0;
                    SetState(GOSS08_ST_PATROL);
                    start = NearestFleePoint();
                    patrol.Restart((s16)start);
                    SetFacing(
                        (s16)((Math_RadiansToAngle4096((float)atan2((double)fleeTraj->pts[start].x - (double)pos.x,
                                                                    (double)fleeTraj->pts[start].z - (double)pos.z)) +
                               0x800) &
                              0xfff));
                    hasFled = 1;
                    fleeing = 1;
                }
            } else {
                if (wolfHeld) {
                    /* cast kept: message arguments travel as void * */
                    g_pWolf->HandleMessage(this, MSG_WOLF_GOSSAMER_SCRIPT, (void *)GOSSAMER_SCRIPT_IDLE);
                    wolfHeld = !g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0);
                }
                timer = 0;
                timerLimit = 0xfa0;
                SetState(GOSS08_ST_PATROL);
                nearest = NearestFleePoint();
                patrol.Restart((s16)nearest);
                SetFacing(
                    (s16)((Math_RadiansToAngle4096((float)atan2((double)fleeTraj->pts[nearest].x - (double)pos.x,
                                                                (double)fleeTraj->pts[nearest].z - (double)pos.z)) +
                           0x800) &
                          0xfff));
                hasFled = 1;
                fleeing = 1;
            }
            break;
        case GOSS08_ST_PAUSE: /* recovering: face Ralph until the timer runs out */
            if (timer > timerLimit)
                SetState(GOSS08_ST_APPROACH);
            timer += g_dtMs;
            SetFacing(HEADING_TO(&pos, &g_pWolf->pos));
            break;
        case GOSS08_ST_CATCH: /* the grab: pull Ralph to 125 units and hug him */
            if (IsWolfOnHighGround()) {
                wolfHeld = g_pWolf->HandleMessage(this, MSG_FREEZE, 0);
                if (wolfHeld) {
                    distToTarget = Vec3s_DistXZ(&pos, &g_pWolf->pos);
                    SetFacing(HEADING_TO(&pos, &g_pWolf->pos));
                    if (distToTarget == 0)
                        distToTarget = 1;
                    vel.x = (s16)(pos.x - target.x * (0x7d - (s16)distToTarget) / distToTarget);
                    vel.z = (s16)(pos.z - target.z * (0x7d - (s16)distToTarget) / distToTarget);
                    vel.y = pos.y;
                    SetPosition(&vel);
                    /* cast kept: message arguments travel as void * */
                    g_pWolf->HandleMessage(this, MSG_WOLF_GOSSAMER_SCRIPT, (void *)GOSSAMER_SCRIPT_GRAB_0);
                    PlayAnim(AGOSSA01_ANIM_CATCH, 0, 1);
                    SetState(GOSS08_ST_FLEE);
                    break;
                }
                SetState(GOSS08_ST_APPROACH);
            }
            break;
        case GOSS08_ST_WATCH: /* Ralph could not be frozen: wait until he can be */
            if (g_pWolf->HandleMessage(this, MSG_WOLF_IS_VISIBLE, 0))
                SetState(GOSS08_ST_APPROACH);
            break;
        case GOSS08_ST_WAIT_ZONE: /* out of his zone: back to watching once Ralph is inside it again */
            if (IsInActionZone(&g_pWolf->pos))
                SetState(GOSS08_ST_APPROACH);
            break;
    }
    AdvanceAnim();
}

/* 0x454861 - vtable +0x10. 0x11 arms the flee cinematic, 0x1e (the bull) plays it, 0x38 sends him to a position,
 * 0x39 is the time portal taking him through, 0x3a is being picked up, 0x75 puts him back to watching. */
/* BYTES(flow, inferred): SetState(6) twice, as in the original */
s32 Gossamer_Lev08::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    switch (msgId) {
        case MSG_TIMEMACHINE_CALL:
            if (inFuture == 0) {
                grabPos = &sender->pos;
                if (state == GOSS08_ST_CHASE || state == GOSS08_ST_PAUSE) {
                    SetState(GOSS08_ST_GO_GRAB);
                    return 1;
                }
            }
            return 0;
        case MSG_TIMEMACHINE_LOCK:
            if (wolfHeld) {
                /* cast kept: HandleMessage's arg is a void *; this message passes the WolfGossamerScript step in it */
                g_pWolf->HandleMessage(this, MSG_WOLF_GOSSAMER_SCRIPT, (void *)GOSSAMER_SCRIPT_RELEASED);
                wolfHeld = !g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0);
            }
            inFuture = (inFuture == 0);
            grabKind = GOSS_GRAB_INSIDE;
            shadow.SetVisible(0);
            SetFacing(HEADING_TO(&pos, &sender->pos));
            SetState(GOSS08_ST_GRAB_RESULT);
            SetState(GOSS08_ST_GRAB_RESULT);
            break;
        case MSG_TIMEMACHINE_ARRIVE:
            grabKind = GOSS_GRAB_OUT;
            SetPosition((Vec3s *)arg); /* cast kept: the message arg is a void *; this one carries a position */
            SetFacing((s16)(Math_RadiansToAngle4096((float)atan2((double)sender->pos.x - (double)pos.x,
                                                                 (double)sender->pos.z - (double)pos.z)) &
                            0xfff));
            target.x = g_pWolf->pos.x - pos.x;
            target.z = g_pWolf->pos.z - pos.z;
            target.y = 0;
            distToTarget = Vec3s_DistXZ(&pos, &g_pWolf->pos);
            SetFacing(HEADING_TO(&pos, &g_pWolf->pos));
            if ((u32)distToTarget < 0x96) {
                if (distToTarget == 0)
                    distToTarget = 1;
                vel.x = (s16)(pos.x - target.x * (0x7d - (s16)distToTarget) / distToTarget);
                vel.z = (s16)(pos.z - target.z * (0x7d - (s16)distToTarget) / distToTarget);
                vel.y = pos.y;
                SetPosition(&vel);
            }
            SetState(GOSS08_ST_GRAB_RESULT);
            break;
        case MSG_TIMEMACHINE_RELEASE:
            SetState(GOSS08_ST_APPROACH);
            break;
        case MSG_BULL_TARGET_QUERY:
            if (Box_ContainsXZ(futureZone, &pos) && hasFled == 0) {
                cineStage = 2;
                if (cineBox2)
                    Cine_Play(cineId2, cineFlags2, cineBox2, cineText2);
                SetState(GOSS08_ST_CINEMATIC);
                cinePending = 0;
            }
            return 1;
        case MSG_BUMP:
            cinePending = 1;
            return 1;
    }
    return 0;
}

/* 0x454e1c - is p inside the zone he is allowed to be in: IDFUTURACTIONZONE3 or one of the IDFUTURACTIONZONE boxes
 * once the time portal has flipped him, else IDPASTACTIONZONE. */
s32 Gossamer_Lev08::IsInActionZone(Vec3s *p)
{
    if (inFuture) {
        if (Box_ContainsXZ(futureZone, p) || futureZones.FindContainingXZ(p))
            return 1;
        return 0;
    }
    if (pastZone)
        return Box_ContainsXZ(pastZone, p);
    return 0;
}

/* 0x454f47 - enter a state, remembering the one it leaves (state 7 needs it when its animation ends). */
void Gossamer_Lev08::SetState(u8 newState)
{
    prevState = state;
    state = newState;
    switch (state) {
        case GOSS08_ST_HUG:
            mashStartMs = 0;
            break;
        case GOSS08_ST_CHASE:
            PlayAnim(AGOSSA01_ANIM_RUN1, 1, 0);
            squeezeTicks = 0;
            break;
        case GOSS08_ST_APPROACH:
            PlayAnim(AGOSSA01_ANIM_STAND2, 1, 0);
            break;
        case GOSS08_ST_WATCH:
            PlayAnim(AGOSSA01_ANIM_STAND2, 1, 0);
            break;
        case GOSS08_ST_PATROL:
            StopSound(soundHandle);
            soundHandle = Sound_Play(SND_RUN_LOOP_ELMER, this, 0x7f, SNDF_LOOP | SNDF_POSITIONAL, 0x1000);
            PlayAnim(AGOSSA01_ANIM_RUN2, 1, 0);
            break;
        case GOSS08_ST_GO_GRAB:
            PlayAnim(AGOSSA01_ANIM_RUN1, 1, 0);
            squeezeTicks = 0;
            break;
        case GOSS08_ST_WAIT_ZONE:
            PlayAnim(AGOSSA01_ANIM_STAND2, 1, 0);
            break;
    }
}

/* 0x4551e1 - the chase step of state 1. Returns 0 only when he gives up (out of his zone, or Ralph cannot be
 * frozen); 1 otherwise. */
/* BYTES(slot-name): all five locals are declared here (one shared ContactInfo), named for their slots */
s32 Gossamer_Lev08::ChaseWolf()
{
    /* All five locals are declared here (and named for their stack slots): the original's frame has one shared
     * ContactInfo, not one per block. */
    Vec3s save;
    u8 n;
    ContactInfo ci;
    Vec3s center;
    Vec3s want;

    if (!IsInActionZone(&g_pWolf->pos)) {
        SetState(GOSS08_ST_WAIT_ZONE);
        return 0;
    }
    target.x = g_pWolf->pos.x - pos.x;
    target.z = g_pWolf->pos.z - pos.z;
    target.y = 0;
    if (g_pWolf->pos.y > pos.y) {
        if ((g_pWolf->pos.y - pos.y >= 0 ? g_pWolf->pos.y - pos.y : -(g_pWolf->pos.y - pos.y)) > 0x14) {
            for (n = 0; n < futureZones.count; n++) {
                if (Box_ContainsXZ(futureZones.boxes[n], &pos)) {
                    center.x = (s16)((futureZones.boxes[n]->max[0] - futureZones.boxes[n]->min[0]) / 2 +
                                     futureZones.boxes[n]->min[0]);
                    center.z = (s16)((futureZones.boxes[n]->max[2] - futureZones.boxes[n]->min[2]) / 2 +
                                     futureZones.boxes[n]->min[2]);
                    center.y = 0;
                    target.x = center.x - pos.x;
                    target.z = center.z - pos.z;
                    target.y = 0;
                    distToTarget = Vec3s_DistXZ(&center, &pos);
                    if (distToTarget != 0) {
                        vel.x = (s16)(target.x * 0x190 / distToTarget);
                        vel.z = (s16)(target.z * 0x190 / distToTarget);
                        vel.y = 0x190;
                        Vec3s_ScaleByDt(&vel, &step);
                        Collide_ResolveMove(&step, &ci, 0x578, RESOLVE_SLIDE_ALL, 0, 0, 10, 0, 0);
                        Translate(&step);
                    }
                    return 1;
                }
            }
        }
    }
    distToTarget = Vec3s_DistXZ(&pos, &g_pWolf->pos);
    SetFacing(HEADING_TO(&pos, &g_pWolf->pos));
    if ((u32)distToTarget >= 0x4b0) {
        SetState(GOSS08_ST_APPROACH);
        return 1;
    }
    if ((u32)distToTarget < 0x82) {
        if ((pos.y - g_pWolf->pos.y >= 0 ? pos.y - g_pWolf->pos.y : -(pos.y - g_pWolf->pos.y)) < 0x32) {
            if (!g_pWolf->HandleMessage(this, MSG_WOLF_IS_VISIBLE, 0)) {
                SetState(GOSS08_ST_WATCH);
                return 0;
            }
            timer = 0;
            mashCount = 0;
            hugStage = 0;
            SetState(GOSS08_ST_CATCH);
            PlayAnim(AGOSSA01_ANIM_OUCH, 1, 0);
        }
    } else {
        save = pos;
        vel.x = (s16)(target.x * 0x190 / distToTarget);
        vel.z = (s16)(target.z * 0x190 / distToTarget);
        vel.y = 0;
        Vec3s_ScaleByDt(&vel, &step);
        Translate(&step);
        if (!IsInActionZone(&pos)) {
            SetPosition(&save);
            SetState(GOSS08_ST_APPROACH);
            return 1;
        }
        SetPosition(&save);
        vel.x = (s16)(target.x * 0x190 / distToTarget);
        vel.z = (s16)(target.z * 0x190 / distToTarget);
        vel.y = 0x190;
        Vec3s_ScaleByDt(&vel, &step);
        want = step;
        Collide_ResolveMove(&step, &ci, 0x578, RESOLVE_SLIDE_ALL, 0, 0, 10, 0, 0);
        if (ci.wallObj &&
            (ci.wallObj->GetClassId() == CLASSID_SIGNPOST || ci.wallObj->GetClassId() == CLASSID_SIGNPOSTSIMPLE)) {
            step.x = want.x;
            step.z = want.z;
        }
        Translate(&step);
    }
    return 1;
}

/* 0x4558c9 - the button mashing that breaks the hug: more than six presses within 1500 ms and Ralph is free
 * (returns 1); 800 ms without a press and the count starts again. */
s32 Gossamer_Lev08::UpdateStruggle()
{
    /* cast kept (both (void *) message arguments here): HandleMessage's arg is a void *; this message passes the
     * WolfGossamerScript step in it */
    if (ACTION_RELEASED()) {
        if (mashCount == 0)
            mashStartMs = timer;
        mashLastMs = timer;
        mashCount++;
    }
    if (mashCount > 6) {
        if (timer - mashStartMs < 0x5dc)
            return 1;
        PlayAnim(AGOSSA01_ANIM_CATCH1, 1, 1);
        hugging = 0;
        mashCount = 0;
        mashStartMs = 0;
        timer = 0;
        /* cast kept: message arguments travel as void * */
        g_pWolf->HandleMessage(this, MSG_WOLF_GOSSAMER_SCRIPT, (void *)GOSSAMER_SCRIPT_GRAB_1);
        return 0;
    }
    if (timer - mashLastMs > 0x320) {
        mashCount = 0;
        timer = 0;
        mashStartMs = 0;
        hugging = 0;
        PlayAnim(AGOSSA01_ANIM_CATCH1, 1, 1);
        /* cast kept: message arguments travel as void * */
        g_pWolf->HandleMessage(this, MSG_WOLF_GOSSAMER_SCRIPT, (void *)GOSSAMER_SCRIPT_GRAB_1);
    }
    timer += g_dtMs;
    return 0;
}

/* 0x455ae6 - the grab check: the ground under Ralph, queried with this object's own collision box, is above
 * vertical 10 (the vertical axis points down, so a smaller value is higher). */
s32 Gossamer_Lev08::IsWolfOnHighGround()
{
    if (QueryGroundY(&g_pWolf->pos, 0) < 10)
        return 1;
    return 0;
}

/* 0x455b15 - the flee path point nearest to him, plus one (so he starts by walking away), unless that is already
 * the last point. */
u16 Gossamer_Lev08::NearestFleePoint()
{
    u16 best = 0;
    u16 minDist;
    u16 i;

    minDist = (u16)Vec3s_DistXZ(&pos, &fleeTraj->pts[0]);
    for (i = 1; i < fleeTraj->count; i++) {
        u16 d = (u16)Vec3s_DistXZ(&pos, &fleeTraj->pts[i]);

        if (d < minDist) {
            minDist = d;
            best = i;
        }
    }
    if (best == fleeTraj->count - 1)
        return best;
    best++;
    return best;
}

/* 0x455be3 - vtable +0x08: the animated draw, plus the dust trail when it has particles. */
void Gossamer_Lev08::Render(Camera *view)
{
    ScnMobile::Render(view);
    if (dust.base.flags.active)
        dust.base.Emitter_Render(view, 0);
}
