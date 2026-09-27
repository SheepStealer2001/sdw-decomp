/*
 * T086 - original object PrayingGhost.cpp (guessed name): .text 0x45ac20-0x45e56d, .rdata 0x574f84-0x574fa8 (vtable),
 * .data 0x57a860-0x57a86c (the cinematic header's stride-table copy), .bss 0x6cccf0-0x6ccd25 (the ghosts' shared
 * state).
 * .bss ORDER: VC6 lays out a file's UNinitialised globals by a hash of their names - bucket (h ^ h >> 16) & 1023 of
 * h = (h << 2) + (h >> 4) + c, ascending, the last-declared first inside a bucket - not by definition order, and these
 * names would come out in another order. The eight are written with zero initializers instead, which keeps them in
 * .bss but in definition order, so the table names are kept.
 *
 * PrayingGhost (class 130, vtable 0x574f84, sizeof 0x55c) - SheepD3D.exe 0x45ac20-0x45e56c. Four of them stand around
 * the sheep in Level 12 (disc Lvl-14) and bow to it on a loop; while a head is down Ralph can cross, and when one sees
 * him it raises the alarm and the group chases him down. The class also drives that level's capture cinematic: the
 * ghosts carry Ralph (and the sheep) along the level's TRAJECTORY to the jail.
 *
 *
 * The four instances share state through file-scope statics (0x6cccf0-0x6ccd24) that nothing outside this address
 * range touches: g_prayingGhosts[] is filled by MSG_PGHOST_REGISTER (its argument is the ghost's index) and the ghost
 * with index 1 (isLeader) runs the parts that must happen once.
 *
 * Inline helpers (descriptive names; they have no bodies of their own in the exe) are here because their expansion gives the
 * original's shapes: SetUpdateMode a 4-way jump table on a constant, PlayAnim an option word in a stack slot, the box
 * tests a pair of pointer temporaries, GhostBoxList::Find / ::Contains the single list pointer the original reuses for
 * both the array and the count. IABS is a macro because each use gets its own temporary and re-reads its operand.
 * Local names are chosen for their stack slots (tools/vc6_locals.py). A shape that reproduces the bytes is a
 * representation, not proof that the original source read this way.
 *
 * match-addr: PrayingGhost_Update=0x45b1b3 PrayingGhost_SetState=0x45d83e PrayingGhost_MoveToward=0x45e029
 * match-addr: PrayingGhost_TurnToHeading=0x45e100 PrayingGhost_FaceTowards=0x45e200 PrayingGhost_DrawAlertIcon=0x45e323
 * match-addr: g_ghostSpareFlag=0x6cccf0 g_ghostCostumeCarry=0x6cccf4 g_ghostGrabOwner=0x6cccf8
 * match-addr: g_ghostGrabHandle=0x6cccfc g_prayingGhosts=0x6ccd00 g_ghostSheepTaken=0x6ccd10
 * match-addr: g_ghostAtPost=0x6ccd14 g_prayingGhostCount=0x6ccd24 g_pGhostHalo=0x6cf664 g_pFireBall=0x6cf638
 */
/* BYTES: dead-code, layout, slot-group. */
/* BYTES(layout): the Cine.h header static: every object including the cinematic header carries this copy in its .data, referenced or not */
/* BYTES(layout): written '= 0' only to keep definition order in .bss */

#define SDW_MEMBERS_ScnObject            \
    static void *operator new(u32 size); \
    void SetFacing(s16 h);               \
    void SetPos(const Vec3s *p);         \
    void SetUpdateMode(u8 mode);

#define SDW_MEMBERS_ZoneList                                      \
    void Find(u16 id)                                             \
    {                                                             \
        /* cast kept: an id list holds record addresses as u32 */ \
        boxes = (Box **)Scn_FindIdList(id, &count);               \
    }

#define SDW_MEMBERS_Mat44 Mat44(); /* 0x4077f0 Mat44_Ctor, empty and out of line */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
struct Box;
class Instance;
struct Animator;
u32 Anim_Start(Instance *inst, Animator *animator, u16 id, u32 opts); /* 0x550196 */
#include "../engine/id_list.h"
#include "../engine/scn_tools.h"
#include "../engine/scenaric.h"
#include "animation.h"
#include "camera.h"
#include "../engine/fixed_math.h"
#include "../engine/interface.h"
#include "ghosthalo.h"
#include "fireball.h"
#include "../app/app_main.h"
#include "../engine/maths.h"
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_GETFACING 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_GETFACING
#define SDW_INLINE_SCNOBJECT_FACING 1
#define SDW_INLINE_SCNOBJECT_SETFACING_S16 1
#define SDW_INLINE_SCNOBJECT_SETPOS_CONST_VEC3S 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_FACING
#undef SDW_INLINE_SCNOBJECT_SETFACING_S16
#undef SDW_INLINE_SCNOBJECT_SETPOS_CONST_VEC3S
#define SDW_INLINE_SCNOBJECT_ISINWORLD 1
#define SDW_INLINE_SCNOBJECT_SETVISIBLE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_ISINWORLD
#undef SDW_INLINE_SCNOBJECT_SETVISIBLE_S32
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#define SDW_INLINE_ZONELIST_CONTAINS_VEC3S 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_CONTAINS_VEC3S
#include "../engine/grid_queries.h"
#include "../engine/property_math.h"

/* 0x57a860 - the cinematic header's static copy of the 9-byte opcode stride table (src/engine/cine.cpp,
 * g_cineOpStride 0x5816fc). The original header defined it static, so every object including it carries its own
 * unreferenced copy at the head of its .data; defined here in its place (0x57a86c-0x57a870 is linker padding before
 * Robot's 8-aligned .data). The map notes this copy's attribution is not unique; the bytes are the same either way. */
static u8 s_cineOpStride[9] = {0, 8, 8, 4, 2, 2, 4, 2, 2};

s32 Scenaric_FindByClass(u16 classId, ScnObject **out, s32 max); /* 0x5145c5 */

extern Wolf *g_pWolf;          /* 0x6cf310 */
extern s32 g_dtMs;             /* 0x71b2e8 */
extern u32 *g_screenLayerBase; /* 0x585044 */
extern u32 g_samZoneColors[3]; /* 0x57aa30 */

/* The four ghosts' shared state; nothing outside 0x45ac20-0x45e56c refers to any of these. */
/* Defined here in their .bss order; the zero initializers keep that order (see the header). */
s32 g_ghostSpareFlag = 0;            /* 0x6cccf0: zeroed by PostLoadInit, read nowhere in the image */
s32 g_ghostCostumeCarry = 0;         /* 0x6cccf4: the costume has reached the meeting point with Ralph */
void *g_ghostGrabOwner = 0;          /* 0x6cccf8: the ghost holding the grab handle */
s32 g_ghostGrabHandle = 0;           /* 0x6cccfc: handle returned by MSG_FREEZE, 0 when nobody holds one */
ScnObject *g_prayingGhosts[4] = {0}; /* 0x6ccd00: by index, filled by MSG_PGHOST_REGISTER */
s32 g_ghostSheepTaken = 0;           /* 0x6ccd10: the sheep has left STARTBOX */
s32 g_ghostAtPost[4] = {0};          /* 0x6ccd14: 1 while ghost i is home and facing the sheep */
u8 g_prayingGhostCount = 0;          /* 0x6ccd24: how many have registered */

/* A designer property of the WAR record: the dword at record + 0x14 + offset. Inlined; the offset is a stack temp
 * with a u32 parameter (as in src/objects/triggedstone.cpp). */
#define SDW_INLINE_FREE_SCN_GETPROPS32_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_SCN_GETPROPS32_VOID_U32

/* Each use gets its own temporary and re-reads its operand: macros, not inline functions. */
#define IABS(v) ((v) >= 0 ? (v) : -(v))
/* signed 12-bit angle difference, -0x800..0x7ff */
#define ANGDIFF(a, b) ((s16)((s16)(((a) - (b) + 0x800) & 0xfff) - 0x800))

/* inline: seven eighths of the screen width - expanded in the back end, so the arithmetic on the constant stays. */
inline s32 Icon_Inset(s32 w)
{
    return w - (w >> 3);
}

/* inline: is the point inside the box - on all three axes, or on x and z only. Both pointers land in temporaries. */
inline s32 Box_Contains(const Box *b, const Vec3s *p)
{
    return p->x >= b->min[0] && p->x <= b->max[0] && p->y >= b->min[1] && p->y <= b->max[1] && p->z >= b->min[2] &&
           p->z <= b->max[2];
}

inline s32 Box_ContainsXZ(const Box *b, const Vec3s *p)
{
    return p->x >= b->min[0] && p->x <= b->max[0] && p->z >= b->min[2] && p->z <= b->max[2];
}

/* 0x45ac20 - vtable +0x00: reads every designer property, finds the level's InflatableSheep / GhostCostume / Jail,
 * puts the sheep's home on the ground in the middle of SHEEPBOX, initialises the three trajectory followers over
 * TRAJECTORY, builds the halo body from exported resource WAR_IDO_AFANTB01, and starts in PGHOST_ST_INIT. */
void PrayingGhost::PostLoadInit()
{
    void *rec = record;
    s32 okCostume;
    s32 foundSheep;
    startBox = Scn_GetPropBox(rec, 0x24);
    checkpoint = Scn_GetPropObject(rec, 0x0c);
    sheep = Scn_GetPropObject(rec, 0x1c);
    sheepBox = Scn_GetPropBox(rec, 0x20);
    sheepHome.x = (s16)((sheepBox->max[0] + sheepBox->min[0]) / 2);
    sheepHome.y = -500;
    sheepHome.z = (s16)((sheepBox->max[2] + sheepBox->min[2]) / 2);
    sheepHome.y = sheep->QueryGroundY(&sheepHome, 1);
    fallingGate = Scn_GetPropObject(rec, 0x14);
    fallingGateDetec = Scn_GetPropObject(rec, 0x18);
    foundSheep = Scenaric_FindByClass(CLASSID_INFLATABLESHEEP, &inflatableSheep, 1);
    if (foundSheep != 1)
        inflatableSheep = 0;
    okCostume = Scenaric_FindByClass(CLASSID_GHOSTCOSTUME, &ghostCostume, 1);
    if (okCostume != 1)
        ghostCostume = 0;
    alertDelayMs = Scn_GetPropS32(rec, 0x28);
    monoHideBoxes.Find((u16)Scn_GetPropS32(rec, 0x34));
    wolfHideBoxes.Find((u16)Scn_GetPropS32(rec, 0x30));
    Scenaric_FindByClass(CLASSID_JAIL, &jail, 1);
    camRestrict1 = Scn_GetPropObject(rec, 0);
    camRestrict2 = Scn_GetPropObject(rec, 4);
    camRestrict3 = Scn_GetPropObject(rec, 8);
    trajectory = Scn_GetPropTrajectory(rec, 0x2c);
    TrajFollower_Init(&followSelf, trajectory, 300, 0x800, 1, 0, 50);
    TrajFollower_Init(&followWolf, trajectory, 300, 0x800, 1, 0, 50);
    TrajFollower_Init(&followSheep, trajectory, 300, 0x800, 1, 0, 50);
    trajCount = trajectory->count;
    trajFirst = trajectory->pts[0];
    trajLast = trajectory->pts[trajCount - 1];
    homePos = pos;
    g_ghostCostumeCarry = 0;
    isLeader = 0;
    wolfHidden = 0;
    sheepBoxOccupied = 0;
    g_ghostSpareFlag = 0;
    graceArmed = 0;
    praying = 0;
    watching = 0;
    sheepStolen = 0;
    g_ghostSheepTaken = 0;
    trajDone = 0;
    g_prayingGhostCount = 0;
    noticed = 0;
    g_ghostGrabOwner = 0;
    g_ghostGrabHandle = 0;
    prayDurMs = Anim_GetDurationMs(Inst(), AFANTO03_ANIM_PRAISE, 1);
    grabDurMs = Anim_GetDurationMs(Inst(), AFANTO03_ANIM_KILL, 1);
    if (Scn_BuildRecordFromExport(WAR_IDO_AFANTB01, haloRecord, 0, 0)) {
        halo.Init(haloRecord, 0);
        haloValid = 1;
    } else {
        haloValid = 0;
    }
    alertBoxes.Find(WAR_IDO_BOXSAMGREEN);
    SetUpdateMode(SCN_UPD_ALWAYS);
    SetState(PGHOST_ST_INIT);
}

/* 0x45b1b3 - vtable +0x04. The leader watches STARTBOX (the sheep leaving it raises the alarm; Ralph entering it while
 * the sheep is gone starts the capture) and SHEEPBOX (what stands where the sheep should be: a real sheep clears the
 * alarm, an InflatableSheep is asked MSG_INFLATABLE_QUERY, anything else starts a two-second grace period and then alerts
 * every ghost). Then the per-state body. */
/* BYTES(dead-code): spare is never used: the original allocates it at ebp-0x104 */
void PrayingGhost::Update()
{
    s32 hitCount;
    s32 spare; /* the original allocates a 4-byte slot at ebp-0x104 and never reads it */
    ScnObject *hits[64];
    if (isLeader && state == PGHOST_ST_AT_POST) {
        if (Box_Contains(startBox, &sheep->pos)) {
            u8 sq;
            sheepStolen = 1;
            g_ghostSheepTaken = 1;
            sheep->HandleMessage(this, MSG_SCRIPT_HOLD, 0);
            for (sq = 0; sq < 4; sq++)
                g_prayingGhosts[sq]->HandleMessage(this, MSG_PGHOST_APPEAR_OUTER, 0);
        }
        if (Box_ContainsXZ(startBox, &g_pWolf->pos) && sheepStolen) {
            u8 sr;
            g_pWolf->HandleMessage(this, MSG_WOLF_GHOST_CARRY,
                                   (void *)1); /* cast kept: a message argument is a void * */
            g_ghostGrabHandle = g_pWolf->HandleMessage(this, MSG_FREEZE, 0);
            if (g_ghostGrabHandle)
                g_ghostGrabOwner = this;
            for (sr = 0; sr < 4; sr++)
                g_prayingGhosts[sr]->HandleMessage(this, MSG_PGHOST_APPEAR_MID, 0);
        }
    }
    if (g_ghostCostumeCarry && index == 3 && !g_pWolf->HandleMessage(this, MSG_WOLF_IS_CARRYING_SHEEP_M1, 0))
        g_pGhostHalo->HandleMessage(this, MSG_HALO_DRAW_AT, &g_pWolf->pos);
    if (g_ghostSheepTaken && index == 3 && !g_pWolf->HandleMessage(this, MSG_WOLF_IS_CARRYING_SHEEP_M1, 0))
        g_pGhostHalo->HandleMessage(this, MSG_HALO_DRAW_AT, &sheep->pos);
    if (isLeader && watching) {
        sheepBoxOccupied = 0;
        /* cast kept: the property's Box and the query's CollBox are one record (flags, min, max) under two names */
        hitCount = ObjGrid_QueryBoxOverlap((CollBox *)sheepBox, hits);
        if (hitCount > 0) {
            u8 ss;
            sheepDecoySeen = 0;
            for (ss = 0; ss < hitCount; ss++) {
                if (hits[ss]->GetClassId() == CLASSID_INFLATABLESHEEP) {
                    if (inflatableSheep->HandleMessage(this, MSG_INFLATABLE_QUERY, 0)) {
                        decoyAccepted = 1;
                        sheepDecoySeen = 1;
                        graceArmed = 0;
                        break;
                    }
                    decoyAccepted = 0;
                    sheepDecoySeen = 1;
                }
                sheepBoxOccupied = 1;
                if (hits[ss]->GetClassId() == CLASSID_SHEEP) {
                    sheepBoxOccupied = 0;
                    graceArmed = 0;
                    break;
                }
            }
        }
        if (sheepBoxOccupied && !sheepDecoySeen) {
            u8 st;
            if (!graceArmed) {
                sheepGraceMs = 0x7d0;
                graceArmed = 1;
            }
            if (!sheepDecoySeen)
                sheepGraceMs = 0;
            sheepGraceMs -= g_dtMs;
            if (sheepGraceMs <= 0) {
                if (state != PGHOST_ST_GRAB && state != PGHOST_ST_CHASE) {
                    for (st = 0; st <= 3; st++)
                        g_prayingGhosts[st]->HandleMessage(this, MSG_PGHOST_ALERT, 0);
                }
                sheepBoxOccupied = 1;
            }
        }
    }
    if (g_ghostAtPost[0] && g_ghostAtPost[1] && g_ghostAtPost[2] && g_ghostAtPost[3])
        SetState(PGHOST_ST_RESET_POSTS);
    switch (state) {
        case PGHOST_ST_STAND:
            praying = 0;
            watching = 0;
            FaceTowards(g_pWolf->pos);
            break;
        case PGHOST_ST_AT_POST:
            FaceTowards(g_pWolf->pos);
            break;
        case PGHOST_ST_GO_TO_POST:
            if (AnimFlags(ANIM_F_FINISHED)) {
                SetPosition(&postPos);
                SetState(PGHOST_ST_AT_POST);
            }
            break;
        case PGHOST_ST_APPEAR:
            FaceTowards(g_pWolf->pos);
            if (index == 0 || index == 3) {
                if (AnimFlags(ANIM_F_FINISHED))
                    SetState(PGHOST_ST_LIFT_SHEEP);
            } else {
                if (AnimFlags(ANIM_F_FINISHED))
                    SetState(PGHOST_ST_TAKE_COSTUME);
            }
            break;
        case PGHOST_ST_LIFT_WOLF: {
            Vec3s dogAt;
            Vec3s meetPos;
            s32 carried;
            s32 gapA;
            FaceTowards(g_pWolf->pos);
            dogAt = g_pWolf->pos;
            meetPos = g_prayingGhosts[1]->pos;
            meetPos.x = (s16)(meetPos.x + 0x6e);
            carried = 0;
            if (isLeader) {
                if (IABS(wolfLift) < 100) {
                    wolfLift -= 10;
                    dogAt.y = (s16)(wolfCapturePos.y + wolfLift);
                } else {
                    dogAt.y = (s16)(wolfCapturePos.y + wolfLift);
                    carried = 1;
                }
                g_pWolf->SetPosition(&dogAt);
            }
            gapA = Vec3s_DistXZ(&g_pWolf->pos, &meetPos);
            if (gapA < 0x14) {
                sheepStolen = 0;
                if (isLeader)
                    jail->HandleMessage(this, MSG_JAIL_OPEN, 0);
                SetState(PGHOST_ST_CARRY_WOLF);
            }
            if (isLeader && carried) {
                chaseDelta.x = (s16)(meetPos.x - g_pWolf->pos.x);
                chaseDelta.y = (s16)(meetPos.y - g_pWolf->pos.y);
                chaseDelta.z = (s16)(meetPos.z - g_pWolf->pos.z);
                chaseVel.x = (s16)(chaseDelta.x * 200 / gapA);
                chaseVel.y = 0;
                chaseVel.z = (s16)(chaseDelta.z * 200 / gapA);
                Vec3s_ScaleByDt(&chaseVel, &chaseStep);
                g_pWolf->Translate(&chaseStep);
            }
            break;
        }
        case PGHOST_ST_TAKE_COSTUME: {
            Vec3s costAt;
            Vec3s dropAt;
            s32 risen;
            s32 gapB;
            FaceTowards(g_pWolf->pos);
            costAt = ghostCostume->pos;
            dropAt = g_prayingGhosts[1]->pos;
            risen = 0;
            if (isLeader) {
                if (IABS(wolfLift) < 100) {
                    wolfLift -= 10;
                    costAt.y = (s16)(wolfCapturePos.y + wolfLift);
                } else {
                    costAt.y = (s16)(wolfCapturePos.y + wolfLift);
                    risen = 1;
                }
                ghostCostume->SetPos(&costAt);
                ghostCostume->Render(&g_camera);
                g_pGhostHalo->HandleMessage(this, MSG_HALO_DRAW_AT, &costAt);
            }
            gapB = Vec3s_DistXZ(&ghostCostume->pos, &dropAt);
            if (gapB < 0x14) {
                g_ghostCostumeCarry = 1;
                SetState(PGHOST_ST_LIFT_WOLF);
            } else if (isLeader && risen) {
                chaseDelta.x = (s16)(dropAt.x - ghostCostume->pos.x);
                chaseDelta.y = (s16)(dropAt.y - ghostCostume->pos.y);
                chaseDelta.z = (s16)(dropAt.z - ghostCostume->pos.z);
                chaseVel.x = (s16)(chaseDelta.x * 200 / gapB);
                chaseVel.y = 0;
                chaseVel.z = (s16)(chaseDelta.z * 200 / gapB);
                Vec3s_ScaleByDt(&chaseVel, &chaseStep);
                costAt.x += chaseStep.x;
                costAt.y += chaseStep.y;
                costAt.z += chaseStep.z;
                ghostCostume->SetPos(&costAt);
            }
            break;
        }
        case PGHOST_ST_CARRY_WOLF: {
            Vec3s ridePos;
            s32 distC;
            if (isLeader) {
                ridePos = g_pWolf->pos;
                ridePos.y = (s16)(wolfCapturePos.y + wolfLift);
                g_pWolf->SetPosition(&ridePos);
                distC = Vec3s_DistXZ(&trajLast, &pos);
                TrajFollower_Step(&followSelf, &chaseVel, &trajHeading);
                SetFacing(TurnToHeading(trajHeading));
                Vec3s_ScaleByDt(&chaseVel, &chaseStep);
                Translate(&chaseStep);
                g_pWolf->TrajFollower_Step(&followWolf, &chaseVel, &trajHeading);
                g_pWolf->SetFacing(TurnToHeading(trajHeading));
                Vec3s_ScaleByDt(&chaseVel, &chaseStep);
                g_pWolf->Translate(&chaseStep);
                g_prayingGhosts[2]->TrajFollower_Step(&followSheep, &chaseVel, &trajHeading);
                g_prayingGhosts[2]->SetFacing(TurnToHeading(trajHeading));
                Vec3s_ScaleByDt(&chaseVel, &chaseStep);
                g_prayingGhosts[2]->Translate(&chaseStep);
                if (distC < 0x3c) {
                    Vec3s away;
                    trajDone = 1;
                    if (fallingGateDetec)
                        fallingGateDetec->HandleMessage(this, MSG_SWITCH_OFF, 0);
                    g_ghostCostumeCarry = 0;
                    away.x = 0;
                    away.y = 0;
                    away.z = 0;
                    g_pGhostHalo->HandleMessage(this, MSG_HALO_DRAW_AT, &away);
                    g_pWolf->HandleMessage(this, MSG_WOLF_GHOST_CARRY, 0);
                    g_prayingGhosts[2]->HandleMessage(this, MSG_PGHOST_RETURN, 0);
                    jail->HandleMessage(this, MSG_JAIL_CLOSE, 0);
                    if (g_ghostGrabHandle) {
                        g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0);
                        g_ghostGrabOwner = 0;
                        g_ghostGrabHandle = 0;
                    }
                    SetState(PGHOST_ST_RETURN);
                }
            }
            break;
        }
        case PGHOST_ST_LIFT_SHEEP: {
            s32 gapD;
            Vec3s spotAt;
            Vec3s eweAt;
            FaceTowards(sheep->pos);
            spotAt = pos;
            spotAt.x = (s16)(spotAt.x - 0x4b);
            eweAt = sheep->pos;
            if (index == 0) {
                if (IABS(eweAt.y) < 300 && 300 - IABS(sheepCapturePos.y) > IABS(sheepLift)) {
                    sheepLift -= 10;
                    eweAt.y = (s16)(sheepCapturePos.y + sheepLift);
                } else {
                    eweAt.y = -300;
                }
                sheep->SetPosition(&eweAt);
            }
            gapD = Vec3s_DistXZ(&sheep->pos, &spotAt);
            if (gapD < 0x14)
                SetState(PGHOST_ST_CARRY_SHEEP);
            if (index == 0) {
                if (IABS(eweAt.y) == 300) {
                    Vec3s hiAt;
                    hiAt = sheep->pos;
                    hiAt.y = -300;
                    sheep->SetPosition(&hiAt);
                    chaseDelta.x = (s16)(spotAt.x - hiAt.x);
                    chaseDelta.y = (s16)(spotAt.y - hiAt.y);
                    chaseDelta.z = (s16)(spotAt.z - hiAt.z);
                    chaseVel.x = (s16)(chaseDelta.x * 200 / gapD);
                    chaseVel.y = 0;
                    chaseVel.z = (s16)(chaseDelta.z * 200 / gapD);
                    Vec3s_ScaleByDt(&chaseVel, &chaseStep);
                    sheep->Translate(&chaseStep);
                }
            }
            break;
        }
        case PGHOST_ST_CARRY_SHEEP:
            FaceTowards(sheep->pos);
            if (index == 0) {
                s32 gapE;
                Vec3s heldAt;
                heldAt = sheep->pos;
                heldAt.y = -300;
                sheep->SetPosition(&heldAt);
                gapE = Vec3s_DistXZ(&sheep->pos, &sheepHome);
                chaseDelta.x = (s16)(sheepHome.x - pos.x);
                chaseDelta.y = (s16)(sheepHome.y - pos.y);
                chaseDelta.z = (s16)(sheepHome.z - pos.z);
                chaseVel.x = (s16)(chaseDelta.x * 200 / gapE);
                chaseVel.y = 0;
                chaseVel.z = (s16)(chaseDelta.z * 200 / gapE);
                Vec3s_ScaleByDt(&chaseVel, &chaseStep);
                Translate(&chaseStep);
                sheep->Translate(&chaseStep);
                g_prayingGhosts[3]->Translate(&chaseStep);
                if (gapE < 0x5a) {
                    g_ghostSheepTaken = 0;
                    sheep->HandleMessage(this, MSG_SCRIPT_RELEASE, 0);
                    g_prayingGhosts[3]->HandleMessage(this, MSG_PGHOST_RETURN, 0);
                    sheep->SetPosition(&sheepHome);
                    SetState(PGHOST_ST_RETURN);
                }
            }
            break;
        case PGHOST_ST_RESET_POSTS: {
            u8 sw;
            g_ghostAtPost[0] = 0;
            g_ghostAtPost[1] = 0;
            graceArmed = 0;
            for (sw = 0; sw <= 3; sw++)
                g_prayingGhosts[sw]->HandleMessage(this, MSG_PGHOST_PRAY, 0);
            SetState(PGHOST_ST_PRAY);
            break;
        }
        case PGHOST_ST_PRAY: {
            u8 sx;
            praying = 1;
            watching = 1;
            FaceTowards(sheepHome);
            wolfHidden = 0;
            for (sx = 0; sx < monoHideBoxes.count; sx++) {
                if (Box_ContainsXZ(monoHideBoxes.boxes[sx], &g_pWolf->pos)) {
                    wolfHidden = 1;
                    break;
                }
            }
            for (sx = 0; sx < wolfHideBoxes.count; sx++) {
                if (Box_ContainsXZ(wolfHideBoxes.boxes[sx], &g_pWolf->pos)) {
                    wolfHidden = 1;
                    break;
                }
            }
            if (isLeader && alertBoxes.Contains(&g_pWolf->pos) == 0) {
                timer -= g_dtMs;
                alertLevel = 0;
                if (!wolfHidden && g_pWolf->HandleMessage(this, MSG_WOLF_IS_NOISY, 0)) {
                    u8 sy;
                    for (sy = 0; sy <= 3; sy++)
                        g_prayingGhosts[sy]->HandleMessage(this, MSG_PGHOST_ALERT, 0);
                }
                if (timer <= alertDelayMs || timer >= 0xaf0) {
                    alertLevel = 1;
                    if (!wolfHidden && g_pWolf->HandleMessage(this, MSG_WOLF_IS_VISIBLE, 0)) {
                        u8 sz;
                        for (sz = 0; sz <= 3; sz++)
                            g_prayingGhosts[sz]->HandleMessage(this, MSG_PGHOST_ALERT, 0);
                    }
                }
                DrawAlertIcon();
            }
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(PGHOST_ST_PRAY);
            break;
        }
        case PGHOST_ST_CHASE: {
            Vec3s catchAt;
            s32 distF;
            if (noticed) {
                if (noticed->GetClassId() == CLASSID_ROBOT)
                    target = noticed;
                else
                    target = g_pWolf;
            } else {
                target = g_pWolf;
            }
            praying = 0;
            catchAt = target->pos;
            if (isLeader)
                catchAt.z = (s16)(target->pos.z + 0x82);
            if (index == 0)
                catchAt.z = (s16)(target->pos.z - 0x82);
            FaceTowards(target->pos);
            distF = MoveToward(catchAt, 0x578, 0);
            if (distF < 0x1e) {
                if (g_pWolf->HandleMessage(this, MSG_WOLF_IS_DEAD, 0) && !g_ghostGrabHandle) {
                    SetState(PGHOST_ST_RETURN);
                } else {
                    if (!g_ghostGrabHandle) {
                        g_ghostGrabOwner = this;
                        g_ghostGrabHandle = target->HandleMessage(this, MSG_FREEZE, 0);
                    }
                    if (target->GetClassId() == CLASSID_WOLF) {
                        if (g_pWolf->HandleMessage(this, MSG_WOLF_IS_CARRYING_SHEEP_M1, 0))
                            g_pWolf->HandleMessage(this, MSG_WOLF_STRIP, 0);
                        g_pWolf->HandleMessage(this, MSG_SCARE, 0);
                    }
                    SetState(PGHOST_ST_GRAB);
                }
            }
            break;
        }
        case PGHOST_ST_GRAB:
            FaceTowards(target->pos);
            if (haloValid) {
                halo.AdvanceAnim();
                halo.rot.y = GetFacing();
            }
            if (!g_ghostGrabHandle) {
                g_ghostGrabOwner = this;
                g_ghostGrabHandle = target->HandleMessage(this, MSG_FREEZE, 0);
            }
            timer -= g_dtMs;
            if (timer <= 0) {
                if (g_ghostGrabHandle && g_ghostGrabOwner == this) {
                    target->HandleMessage(this, MSG_UNFREEZE, 0);
                    g_ghostGrabOwner = 0;
                    g_ghostGrabHandle = 0;
                }
                g_pFireBall->HandleMessage(this, MSG_FIREBALL_STRIKE, target);
            }
            g_ghostAtPost[index] = 0;
            break;
        case PGHOST_ST_RETURN: {
            s32 gapG;
            gapG = MoveToward(homePos, 1000, 1);
            if (gapG < 0x1e) {
                if (isLeader && !trajDone && g_ghostGrabHandle && g_ghostGrabOwner == this) {
                    g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0);
                    g_ghostGrabOwner = 0;
                    g_ghostGrabHandle = 0;
                }
                SetPosition(&homePos);
                FaceTowards(sheepHome);
                g_ghostAtPost[index] = 1;
            }
            break;
        }
    }
    AdvanceAnim();
}

/* 0x45d3f2 - vtable +0x10. MSG_PGHOST_REGISTER registers this ghost with the index in the argument and puts it on its
 * post along the trajectory; MSG_PGHOST_PRAY sends it back to praying, MSG_PGHOST_ALERT alerts it, MSG_PGHOST_RETURN
 * releases it, MSG_PGHOST_APPEAR_MID / _OUTER start the capture on the two pairs; MSG_GHOST_NOTICE names the object it
 * has noticed; MSG_FREEZE asks whether Ralph may be grabbed. */
s32 PrayingGhost::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    switch (msgId) {
        case MSG_PGHOST_REGISTER:
            if (sender->GetClassId() == CLASSID_DANCINGGHOSTMANAGER && !registered) {
                AddToWorld(&homePos);
                index = (u8)(u32)arg; /* cast kept: the index travels in the void * argument */
                g_ghostAtPost[index] = 0;
                if (index == 1)
                    isLeader = 1;
                registered = 1;
                g_prayingGhosts[g_prayingGhostCount] = this;
                g_prayingGhostCount++;
                if (index == 0) {
                    postPos.x = (s16)(trajFirst.x + 0x1f4);
                    postPos.y = trajFirst.y;
                    postPos.z = trajFirst.z;
                }
                if (index == 1) {
                    postPos.x = trajFirst.x;
                    postPos.y = trajFirst.y;
                    postPos.z = trajFirst.z;
                }
                if (index == 2) {
                    postPos.x = (s16)(trajFirst.x + 0xf5);
                    postPos.y = trajFirst.y;
                    postPos.z = trajFirst.z;
                }
                if (index == 3) {
                    postPos.x = (s16)(trajFirst.x + 0x15e);
                    postPos.y = trajFirst.y;
                    postPos.z = trajFirst.z;
                }
                if (isLeader)
                    alertLevel = 0;
                SetVisible(0);
                SetState(PGHOST_ST_GO_TO_POST);
            }
            break;
        case MSG_PGHOST_PRAY:
            SetState(PGHOST_ST_PRAY);
            break;
        case MSG_PGHOST_ALERT:
            if (index == 0 || index == 1)
                SetState(PGHOST_ST_CHASE);
            else
                SetState(PGHOST_ST_STAND);
            break;
        case MSG_PGHOST_RETURN:
            SetState(PGHOST_ST_RETURN);
            break;
        case MSG_PGHOST_APPEAR_MID:
            if (state == PGHOST_ST_AT_POST && (index == 1 || index == 2))
                SetState(PGHOST_ST_APPEAR);
            break;
        case MSG_PGHOST_APPEAR_OUTER:
            if (state == PGHOST_ST_AT_POST && (index == 0 || index == 3))
                SetState(PGHOST_ST_APPEAR);
            break;
        case MSG_GHOST_NOTICE:
            noticed = (ScnObject *)arg; /* cast kept: MSG_GHOST_NOTICE passes the object in the void * argument */
            if (state != PGHOST_ST_GRAB && state != PGHOST_ST_CHASE && alertLevel == 1 && isLeader) {
                u8 i;
                for (i = 0; i <= 3; i++)
                    g_prayingGhosts[i]->HandleMessage(this, MSG_PGHOST_ALERT, 0);
            }
            break;
        case MSG_FREEZE:
            if (g_pWolf->HandleMessage(this, MSG_WOLF_IS_DEAD, 0))
                return 1;
            return 0;
    }
    return 0;
}

/* 0x45d83e - the state machine's entry side. PGHOST_ST_INIT releases the three camera restrictions and leaves the
 * world; PGHOST_ST_LIFT_WOLF and PGHOST_ST_TAKE_COSTUME snapshot Ralph, PGHOST_ST_LIFT_SHEEP snapshots the sheep;
 * PGHOST_ST_PRAY starts the praying cycle and PGHOST_ST_GRAB the grab, whose halo body is put on the ghost and started
 * on AFANTB01_ANIM_KILL3. */
void PrayingGhost::SetState(u8 newState)
{
    state = newState;
    switch (newState) {
        case PGHOST_ST_INIT:
            if (camRestrict1)
                camRestrict1->HandleMessage(this, MSG_CAMRESTRICT_ENABLE, 0);
            if (camRestrict2)
                camRestrict2->HandleMessage(this, MSG_CAMRESTRICT_ENABLE, 0);
            if (camRestrict3)
                camRestrict3->HandleMessage(this, MSG_CAMRESTRICT_ENABLE, 0);
            if (IsInWorld())
                RemoveFromWorld();
            registered = 0;
            break;
        case PGHOST_ST_STAND:
            PlayAnim(AFANTO03_ANIM_STAND, 1, 1);
            break;
        case PGHOST_ST_AT_POST:
            PlayAnim(AFANTO03_ANIM_STAND2, 1, 1);
            break;
        case PGHOST_ST_APPEAR:
            /* cast kept (the three (void *)1): a message argument is a void * */
            if (camRestrict1)
                camRestrict1->HandleMessage(this, MSG_CAMRESTRICT_ENABLE, (void *)1);
            if (camRestrict2)
                /* cast kept: message arguments travel as void * */
                camRestrict2->HandleMessage(this, MSG_CAMRESTRICT_ENABLE, (void *)1);
            if (camRestrict3)
                camRestrict3->HandleMessage(this, MSG_CAMRESTRICT_ENABLE, (void *)1);
            SetVisible(1);
            PlayAnim(AFANTO03_ANIM_APPEAR, 0, 1);
            break;
        case PGHOST_ST_TAKE_COSTUME:
            wolfCapturePos = g_pWolf->pos;
            g_pWolf->HandleMessage(this, MSG_WOLF_STRIP, 0);
            /* cast kept: the class id travels in the void * argument */
            g_pWolf->HandleMessage(this, MSG_WOLF_TAKE_ITEM_CLASS, (void *)CLASSID_GHOSTCOSTUME);
            if (isLeader)
                ghostCostume->SetPos(&g_pWolf->pos);
            PlayAnim(AFANTO03_ANIM_HOO, 1, 1);
            break;
        case PGHOST_ST_CHASE:
            PlayAnim(AFANTO03_ANIM_WALK, 1, 1);
            break;
        case PGHOST_ST_PRAY:
            timer = prayDurMs;
            PlayAnim(AFANTO03_ANIM_PRAISE, 0, 1);
            break;
        case PGHOST_ST_GO_TO_POST:
            PlayAnim(AFANTO03_ANIM_STAND2, 1, 1);
            break;
        case PGHOST_ST_LIFT_WOLF:
            wolfLift = 0;
            g_pWolf->HandleMessage(this, MSG_WOLF_STRIP, 0);
            if (isLeader)
                g_pWolf->HandleMessage(this, MSG_WOLF_GHOST_CARRY, (void *)1); /* cast kept: an argument is a void * */
            wolfCapturePos = g_pWolf->pos;
            PlayAnim(AFANTO03_ANIM_HOO, 1, 1);
            break;
        case PGHOST_ST_CARRY_WOLF:
            if (isLeader) {
                g_pWolf->HandleMessage(this, MSG_WOLF_GHOST_CARRY, (void *)1); /* cast kept: an argument is a void * */
                if (fallingGate)
                    fallingGate->HandleMessage(this, MSG_SWITCH_ON, 0);
            }
            PlayAnim(AFANTO03_ANIM_WALK, 1, 1);
            break;
        case PGHOST_ST_LIFT_SHEEP:
            sheepLift = 0;
            sheepCapturePos = sheep->pos;
            PlayAnim(AFANTO03_ANIM_HOO, 1, 1);
            break;
        case PGHOST_ST_GRAB:
            timer = grabDurMs - 0xaa;
            PlayAnim(AFANTO03_ANIM_KILL, 0, 1);
            if (haloValid) {
                halo.pos = pos;
                halo.PlayAnim(AFANTB01_ANIM_KILL3, 0, 1);
                haloVisible = 1;
            }
            break;
        case PGHOST_ST_RETURN:
            if (isLeader && checkpoint)
                checkpoint->HandleMessage(this, MSG_CHECKPOINT_ACTIVATE, 0);
            PlayAnim(AFANTO03_ANIM_WALK, 1, 1);
            break;
    }
}

/* 0x45dfeb - vtable +0x08: the halo body first while it is up, then the ghost itself. */
void PrayingGhost::Render(Camera *view)
{
    if (haloVisible)
        halo.Render(view);
    ScnBody::Render(view);
}

/* 0x45e029 - step `speed` units per second towards `target` on x and z. The second component of the difference
 * repeats the first (a copy of that line the result never uses). Returns the XZ distance that was left. */
s32 PrayingGhost::MoveToward(Vec3s target, s32 speed, s32 turn)
{
    Vec3s stepXZ;
    Vec3s diff;
    Vec3s velXZ;
    s32 gap;
    gap = Vec3s_DistXZ(&target, &pos);
    if (gap == 0) {
        velXZ.x = 0;
        velXZ.y = 0;
        velXZ.z = 0;
    } else {
        diff.x = (s16)(target.x - pos.x);
        diff.y = (s16)(target.x - pos.x);
        diff.z = (s16)(target.z - pos.z);
        velXZ.x = (s16)(diff.x * speed / gap);
        velXZ.y = 0;
        velXZ.z = (s16)(diff.z * speed / gap);
        Vec3s_ScaleByDt(&velXZ, &stepXZ);
        Translate(&stepXZ);
    }
    if (turn)
        FaceTowards(target);
    return gap;
}

/* 0x45e100 - turn a quarter of the way towards `want` each call, snapping once the signed difference is within 0x100
 * (12-bit angles). Returns the new heading. */
s16 PrayingGhost::TurnToHeading(s16 want)
{
    if (IABS(ANGDIFF(want, turnHeading)) > 0x100) {
        turnHeading += (s16)(ANGDIFF(want, turnHeading) / 4);
        return turnHeading;
    }
    turnHeading = want;
    return want;
}

/* 0x45e200 - face the given point. */
void PrayingGhost::FaceTowards(Vec3s target)
{
    s16 h;
    h = HeadingTo(&target);
    rot.y = h;
}

/* 0x45e22a - vtable +0x14: level restart. From PGHOST_ST_AT_POST or PGHOST_ST_INIT the ghost simply re-enters that
 * state; otherwise it goes back to its home position - and the leader also puts the InflatableSheep back
 * (MSG_INFSHEEP_DEFLATE_RESET) and the sheep on its mark - and restarts the praying cycle. */
void PrayingGhost::Reset()
{
    sheepBoxOccupied = 0;
    g_ghostAtPost[index] = 0;
    g_ghostGrabHandle = 0;
    g_ghostGrabOwner = 0;
    noticed = 0;
    if (state == PGHOST_ST_AT_POST) {
        SetState(PGHOST_ST_AT_POST);
    } else if (state == PGHOST_ST_INIT) {
        SetState(PGHOST_ST_INIT);
    } else {
        SetPosition(&homePos);
        if (isLeader) {
            inflatableSheep->HandleMessage(this, MSG_INFSHEEP_DEFLATE_RESET, 0);
            sheep->SetPosition(&sheepHome);
        }
        timer = prayDurMs;
        SetState(PGHOST_ST_PRAY);
    }
}

/* 0x45e323 - the head icon over the ghost: its own camera 1200 units behind the ghost's facing, the body drawn into
 * one of the two primitive buffers in turn, and the crayon frame around it in the alert colour. The local names and the
 * HudWork grouping give the original's frame; the screen width as an inline value gives the minuend-first
 * `0x200 - (0x200 >> 3)`. */
#define SDW_INLINE_FREE_SCREENWIDTHU16 1
#include "../engine/screen_inlines.h"
#undef SDW_INLINE_FREE_SCREENWIDTHU16
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; unused fill gaps */
void PrayingGhost::DrawAlertIcon()
{
    s16 top15, bottom8, right14, left0;
    struct HudWork {
        Camera camera;
        s32 unused;
        s16 screenX, screenY;
    } w;
    Mat34s_Identity(&w.camera.viewMatS);
    w.camera.rot.x = w.camera.rot.z = 0;
    w.camera.rot.y = (-Facing() - 1024) & 0xfff;
    Vec3s_OffsetAlongAngles(&w.camera.pos, &w.camera.rot, 1200, &pos);
    w.camera.pos.y -= 60;
    w.camera.dist = 1200;
    Camera_BuildViewMatrix(&w.camera, g_pZeroVec3s);
    w.screenX = ScreenWidthU16() - (ScreenWidthU16() >> 3);
    w.screenY = 60;
    RenderEx(&w.camera, iconBufToggle ? iconPrimBuf[0] : iconPrimBuf[1], 100, 400, &w.screenX);
    iconBufToggle = !iconBufToggle;
    left0 = w.screenX - 50;
    bottom8 = w.screenY;
    right14 = w.screenX + 50;
    top15 = w.screenY - 50;
    g_animSpriteCrayon1.Draw(g_screenLayerBase + 9, left0, bottom8, right14, top15, g_samZoneColors[alertLevel], 0, 0);
}

/* 0x45e4e8 - the class factory for CLASSID 130 "PrayingGhost": new PrayingGhost (the base vtables, the halo body's
 * own base vtables, then PrayingGhost's), then ScnBody::Init(record, 0) through the vtable. */
ScnObject *PrayingGhost_Create(void *record)
{
    ScnBody *obj = new PrayingGhost;
    obj = obj->Init(record, 0);
    return obj;
}
