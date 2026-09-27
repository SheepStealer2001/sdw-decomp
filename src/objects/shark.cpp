/* T091 - the original object Shark.cpp (guessed name), one file.
 * .text 0x474d90-0x476e4a (Shark_UpdateWolfChecks .. Shark_Create), .rdata 0x575050-0x575074 (the Shark vtable
 * COMDAT); no .data, no .bss.
 * The shark's splash block 0x473b8e-0x474d8d belongs to the object before (T090, Shadow.cpp: src/engine/shadow.cpp).
 *
 * Shark (class 50 "Shark", CLASSID_SHARK, vtable 0x575050, sizeof 0x43c): the two
 * sharks of Level 4's river. Each one swims its TRAJ waypoint loop inside its BOX; when Ralph swims into that box and
 * is within DETECTHEIGHT vertically and 9600 units away it stops to aim (state 3, 2 s), winds up (7), lunges straight
 * at where he was (8) at 900 units/s, bites (9, MSG_KILL cause 5 to Ralph) or aborts (0xa), recovers (0xb) and swims the same
 * distance back to where it started the lunge (0xc) before resuming the patrol. A shark that reaches Ralph in the
 * water kills him, which is why the raft is the safe way across.
 * Designer properties (SharkProps): BOOLWATCHOVERCAVE +0, BOX +4, DETECTHEIGHT +8, DETECTRANGE +0xc, TRAJ +0x10.
 *
 * Three emitters hang off the shark, all fed from the water zone it is inside (waterBox, refreshed three times a
 * frame): breathFx, 8 bubbles 90 units in front of it while it is 150+ below the surface; chargeFx, 16 bubbles behind
 * it during the wind-up and the lunge only; rippleFx, 8 flat rings drawn on the water surface (Emitter_RenderFlat)
 * while it swims within 150 of the top - the fin-wake visible before the shark arrives.
 *
 * Devices that only pin the original code generation (the inline helpers have no bodies in the exe, so their names are
 * not recovered). A literal inside an inline body becomes an immediate, but a CONSTANT bound to an inline PARAMETER is
 * materialised into a register first - that is what distinguishes SetBoxCollide (and edx,0xfbff) from SetInstFlag
 * (mov ecx,0x10 ... or eax,ecx) and what produces the `mov edx,1 / test edx,edx` in PlayAnim and RenderFlat:
 *  - Scn_GetPropU32 takes the offset as u32, so the constant offset gets a stack temp (0x474ea8), as in crane.cpp.
 *  - PlayAnim / AnimFlags as in src/game/wolf.h; SetUpdateMode's 4-way switch on a constant (jump table 0x475194);
 *    SetBoxCollide (SCN_OF_NO_BOX_COLLIDE 0x400); SetFacing / FacingU16 give a 2-byte temp (0x4763a2, 0x476b2e).
 *  - Box_ContainsPoint as in bipbip.cpp (its two arguments and its value in temps, 0x474da4-0x474e30).
 *  - Zones_Get / ZoneList::FindContaining as in src/game/wolf.h: the constant list index in a register and the list's
 *    address in a stack temp (0x474e42, 0x476650).
 *  - LEN_XZ / LEN_XYZ: the first square is a double multiply and the other two integer ones (fild/fild/fmulp then
 *    fild/faddp, 0x4756cb), so the first factor is cast and the rest are not.
 *  - Vec4s (8 bytes) not Vec3s for the move deltas: the stack slot of `delta` is 8 bytes wide (0x475436 frame).
 * Local names are chosen for their stack slots (tools/vc6_locals.py).
 */
/* BYTES: dead-code, inline, slot-group. */
/* BYTES(slot-group): delta is a Vec4s (PSX SVECTOR, pad unused) because its slot is 8 bytes wide (0x475436) */

#define SDW_MEMBERS_ScnObject                                                                           \
    static void *operator new(u32 size); /* 0x50d5f4 Scenaric_Alloc */                                  \
    /* The engine writes only +0/+2/+4 (a Vec3s). The caller retains its 8-byte Vec4s slot and passes a
     * Vec3s view so the original frame storage remains unchanged (see the header note). */ \
    u16 FacingU16()                                                                                     \
    {                                                                                                   \
        return rot.y;                                                                                   \
    }                                                                                                   \
    void SetFacing(u16 a);                                                                              \
    /* inline, defined below */                                                                         \
    void SetUpdateMode(s32 mode); /* inline, defined below */

#define SDW_MEMBERS_TrajFollower \
    s16 Index()                  \
    {                            \
        return pointIndex;       \
    } /* inline: an s16 temp per read (0x475233) */

#define SDW_MEMBERS_InlineEmitter8 \
    InlineEmitter8();              \
    /* inline: the constant in a register (0x476cd2) */
#define SDW_MEMBERS_TrailEmitter TrailEmitter(); /* inline: the pools are the inline buffers */
#include "sdw_types.h"
#include "sdw_enums.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *inst, Animator *animator, u16 id, u32 opts); /* 0x550196 (used by the PlayAnim inline) */
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_SETFACING_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETFACING_U16
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
inline InlineEmitter8::InlineEmitter8()
{
    base.slotPool = slotBuf;
    base.particles = particleBuf;
    base.count = 8;
    base.Emitter_Reset();
}
inline TrailEmitter::TrailEmitter()
{
    base.slotPool = slotBuf;
    base.particles = particleBuf;
    base.count = 16;
    base.Emitter_Reset();
}
#define SDW_INLINE_INLINEEMITTER8_RENDERFLAT_CAMERA_S32 1
#include "../engine/emitter8_inlines.h"
#undef SDW_INLINE_INLINEEMITTER8_RENDERFLAT_CAMERA_S32

#include "../sdk/crt.h"
extern "C" s16 Math_RadiansToAngle4096(float radians); /* 0x5269ce */

extern Wolf *g_pWolf; /* 0x6cf310 */
#include "../engine/scenaric.h"
#include "../app/app_main.h"
#include "../engine/scn_tools.h"
#include "world_draw.h"
#include "../engine/fixed_math.h"
extern u8 g_sharedScratch[]; /* 0x6d5468  shared collision scratch */

#define g_camPos (g_camera.pos) /* 0x584d20  camera eye */

extern s32 g_dtMs;
/* cast kept: this file reads the frame time as a 16-bit word, as the original loads it */
#define g_dtMs (*(s16 *)&g_dtMs) /* 0x71b2e8  frame time in ms */

s32 Vec3s_Dist(Vec3s *a, Vec3s *b); /* 0x515813 */
s32 Rand_Bounded(s32 bound);        /* 0x561219 */

/* The two lengths this file measures: the first square is a double multiply and the other terms integer ones
 * (fild/fild/fmulp, then fild/faddp per term), so only the first factor is cast (0x4756cb, 0x47621b). */
#define LEN_XZ(v) sqrt((double)(v).x *(v).x + (v).z * (v).z)
#define LEN_XYZ(v) sqrt((double)(v).x *(v).x + (v).y * (v).y + (v).z * (v).z)
#define SHARK_ABS(a) ((a) >= 0 ? (a) : -(a))
/* &traj->pts[i], written index-first: the original scales the index before it loads the pointer (0x4761ac).
 * cast kept: t->pts[i], in any spelling, loads t first; only this byte-offset form (+2 = the offset of pts) does not */
#define TRAJ_PT(t, i) (*(Vec3s *)((i) * 6 + (u8 *)(t) + 2))

/* ---- inline helpers ---- */

/* A u32 designer property: the record's property block starts at +0x14. */
#define SDW_INLINE_FREE_SCN_GETPROPU32_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_SCN_GETPROPU32_VOID_U32

#define SDW_INLINE_FREE_BOX_CONTAINSPOINT_BOX_VEC3S 1
#include "../engine/coll_box_inlines.h"
#undef SDW_INLINE_FREE_BOX_CONTAINSPOINT_BOX_VEC3S

/* One of the seven global zone lists by type (0 = water); the constant type goes into a register first. */
#define SDW_INLINE_FREE_ZONES_GET_U8 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_FREE_ZONES_GET_U8

#define SDW_INLINE_ZONELIST_FINDCONTAINING_VEC3S 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_FINDCONTAINING_VEC3S

/* SCN_OF_NO_BOX_COLLIDE (0x400) off or on; 0x400 is a literal of the body, so it stays an immediate. */
#define SDW_INLINE_SCNOBJECT_SETBOXCOLLIDE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETBOXCOLLIDE_S32

/* An instance flag (0x10 = tinted): the mask is a parameter, so it is loaded into a register and its complement
 * built through a u16 (0x476bde-0x476bf0). Each branch takes the flag word's address for itself. */
/* BYTES(inline): source-only inline: the flag word goes through a pointer local and the complement through a u16 (0x476bde) */
#define SDW_INLINE_SCNOBJECT_SETINSTFLAG_U16_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETINSTFLAG_U16_S32

/* ScnUpdateMode: 0 near the camera only, 1 always, 2 never, 3 also during cinematics. */
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32

/* ---- the class ---- */

/* 0x474d90 - the three tests about Ralph that the rest of the frame reads: is he inside this shark's BOX, is he in
 * water at all, and is he within biting distance (160). */
void Shark::UpdateWolfChecks()
{
    wolfInZone = Box_ContainsPoint(zone, &g_pWolf->pos);
    wolfInWater = Zones_Get(ZONE_WATER)->FindContaining(&g_pWolf->pos) != 0;
    wolfInReach = wolfDist <= 160;
}

/* 0x474e96 - vtable +0x00: the designer properties, the patrol follower, the three emitters' parameters, and state 1. */
void Shark::PostLoadInit()
{
    u16 *rec;
    Trajectory *traj;

    rec = record;
    detectRange = Scn_GetPropU32(rec, 0xc); /* PROPERTY_SHARK_DETECTRANGE */
    detectHeight = Scn_GetPropU32(rec, 8);  /* PROPERTY_SHARK_DETECTHEIGHT */
    detectRangeCopy = detectRange;
    watchOverCave = Scn_GetPropU32(rec, 0) != 0; /* PROPERTY_SHARK_BOOLWATCHOVERCAVE */
    zone = Scn_GetPropBox(rec, 4);               /* PROPERTY_SHARK_BOX */
    traj = Scn_GetPropTrajectory(rec, 0x10);     /* PROPERTY_SHARK_TRAJ */
    TrajFollower_Init(&path, traj, 350, 0x800, 0, 0, 50);
    homeVert = pos.y;
    unusedC0 = 0;
    biteAccepted = 0;
    unusedBc = 0;
    cornering = 0;
    SetBoxCollide(1);
    idleTimer = 3000;
    idlePending = 0;
    idleAnim = AREQUI01_ANIM_SWIM3;
    breathParams.riseSpeed = -120;
    breathParams.life = 0x2000;
    breathParams.spawnInterval = 0x400;
    breathParams.size = 10;
    breathParams.cap = 0;
    breathParams.sheetIndex = 2;
    chargeParams.riseSpeed = -120;
    chargeParams.life = 0x2000;
    chargeParams.spawnInterval = 0x80;
    chargeParams.size = 10;
    chargeParams.cap = 0;
    chargeParams.sheetIndex = 2;
    rippleParams.life = 0x2000;
    rippleParams.fadeStart = 0;
    rippleParams.spawnInterval = 0x400;
    rippleParams.sizeStart = 50;
    rippleParams.sizeEnd = 200;
    rippleParams.sheetIndex = 1;
    SetUpdateMode(SCN_UPD_ALWAYS);
    SetState(SHARK_ST_IDLE);
}

/* 0x4751a4 - vtable +0x04. */
void Shark::Update()
{
    wolfDist = Vec3s_Dist(&pos, &g_pWolf->pos);
    UpdateWolfChecks();
    UpdateAi();
    RunState();
    UpdateBreathFx();
    UpdateChargeFx();
    UpdateRippleFx();
    AdvanceAnim();
}

/* 0x475208 - the decisions: keep following the path, react to Ralph entering the water, bite him when he is in reach,
 * and start aiming when he swims past close enough. `dv` and `yaw` are computed and never used. */
/* BYTES(dead-code): yaw and dv are computed and never read, as in the original */
void Shark::UpdateAi()
{
    Vec3s *myPos;
    s16 yaw;
    Vec4s dv;
    Vec3s *target;

    if (state == SHARK_ST_IDLE || (state == SHARK_ST_FACE && path.Index() != 0) ||
        (watchOverCave == 0 && state == SHARK_ST_IDLE))
        FollowPath();
    if (wolfInWater) {
        if (path.Index() == 0 && watchOverCave != 0)
            SetState(SHARK_ST_FACE);
        else if (state == SHARK_ST_FACE)
            SetState(SHARK_ST_IDLE);
    } else {
        return;
    }
    if (state != SHARK_ST_HIT && state != SHARK_ST_WIND_UP && state != SHARK_ST_LUNGE && state != SHARK_ST_BITE &&
        wolfInReach && wolfInZone) {
        if (biteAccepted)
            return;
        SetState(SHARK_ST_BITE);
        return;
    }
    if (state == SHARK_ST_IDLE || state == SHARK_ST_FACE) {
        if (wolfInZone) {
            if (state != SHARK_ST_AIM) {
                yaw = rot.y;
                myPos = &pos;
                target = &g_pWolf->pos;
                dv.x = target->x - myPos->x;
                dv.y = target->y - myPos->y;
                dv.z = target->z - myPos->z;
                if (wolfDist <= 9600) {
                    if (SHARK_ABS(myPos->y - target->y) < detectHeight) {
                        idlePending = 1;
                        idleAnim = AREQUI01_ANIM_SWIM2;
                        SetState(SHARK_ST_AIM);
                    }
                }
            }
        }
    }
}

/* 0x475436 - the per-state body. States 8 and 0xc are the two that move the shark themselves. */
void Shark::RunState()
{
    ContactInfo contact;
    u16 hitFlags;
    Vec4s delta;

    switch (state) {
        case SHARK_ST_IDLE:
            if (idlePending) {
                idleTimer -= g_dtMs;
                if (idleTimer <= 0) {
                    idleAnim = AREQUI01_ANIM_SWIM3;
                    PlayAnim(idleAnim, 1, 0);
                    idleTimer = 3000;
                    idlePending = 0;
                }
            }
            break;
        case SHARK_ST_AIM:
            if (!wolfInZone) {
                SetState(SHARK_ST_LOOK);
                break;
            }
            aimTimer -= g_dtMs;
            if (aimTimer <= 0) {
                SetState(SHARK_ST_WIND_UP);
                break;
            }
            FaceWolf(1);
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(SHARK_ST_LOOK);
            break;
        case SHARK_ST_FACE:
            FaceWolf(0);
            break;
        case SHARK_ST_HIT:
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(SHARK_ST_BRAKE);
            break;
        case SHARK_ST_LOOK:
            SetState(SHARK_ST_IDLE);
            break;
        case SHARK_ST_WIND_UP:
            if (!wolfInZone)
                SetState(SHARK_ST_LOOK);
            else if (AnimFlags(ANIM_F_FINISHED))
                SetState(SHARK_ST_LUNGE);
            break;
        case SHARK_ST_LUNGE:
            if (!wolfInZone) {
                SetState(SHARK_ST_MISS);
                break;
            }
            /* cast kept: the Vec4s slot (8 bytes in the original frame) holds the Vec3s in its first three words */
            Vec3s_ScaleByDt(&lungeVel, (Vec3s *)&delta);
            /* cast kept: as above */
            hitFlags = Collide_ResolveMove((Vec3s *)&delta, &contact, 0xb54, RESOLVE_SLIDE_ALL | RESOLVE_KEEP_Y, 0, 0,
                                           0xa, 0, 0);
            if (wolfInReach) {
                SetState(SHARK_ST_BITE);
                break;
            }
            if ((hitFlags & (COLL_FLOOR | COLL_WALL | COLL_FLOOR_EDGE)) && contact.wallObj != g_pWolf &&
                contact.floorObj != g_pWolf && contact.movableObj != g_pWolf) {
                SetState(SHARK_ST_HIT);
                break;
            }
            Translate((Vec3s *)&delta); /* cast kept: as above */
            lungeLeft -= (s32)LEN_XYZ(delta);
            if (lungeLeft <= 0)
                SetState(SHARK_ST_MISS);
            break;
        case SHARK_ST_BITE:
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(SHARK_ST_BRAKE);
            break;
        case SHARK_ST_MISS:
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(SHARK_ST_BRAKE);
            break;
        case SHARK_ST_BRAKE:
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(SHARK_ST_SWIM_BACK);
            break;
        case SHARK_ST_SWIM_BACK:
            /* cast kept: the Vec4s slot (8 bytes in the original frame) holds the Vec3s in its first three words */
            Vec3s_ScaleByDt(&lungeVel, (Vec3s *)&delta);
            Translate((Vec3s *)&delta); /* cast kept: as above */
            lungeLeft += (s32)LEN_XYZ(delta);
            if (lungeLeft >= lungeLen)
                SetState(SHARK_ST_LOOK);
            break;
    }
}

/* 0x475880 - enter a state even if it is the one already running (SetState ignores that). No caller in the exe. */
void Shark::ForceState(u8 newState)
{
    state = SHARK_ST_INIT;
    SetState(newState);
}

/* 0x4758a0 - the state entry: mostly the animation, plus the lunge aim (8) and its reverse (0xc). */
void Shark::SetState(u8 newState)
{
    Vec3s r;
    Vec3s d;

    if (state == newState)
        return;
    state = newState;
    switch (state) {
        case SHARK_ST_IDLE:
            PlayAnim(idleAnim, 1, 0);
            break;
        case SHARK_ST_AIM:
            aimTimer = 2000;
            PlayAnim(AREQUI01_ANIM_STAND1, 1, 0);
            break;
        case SHARK_ST_FACE:
            PlayAnim(AREQUI01_ANIM_STAND1, 1, 0);
            break;
        case SHARK_ST_HIT:
            PlayAnim(AREQUI01_ANIM_HIT1, 0, 0);
            break;
        case SHARK_ST_LOOK:
            r.x = rot.x;
            r.y = rot.y;
            r.z = rot.z;
            r.z = 0;
            r.x = r.z;
            rot = r;
            PlayAnim(AREQUI01_ANIM_LOOK, 1, 0);
            break;
        case SHARK_ST_WIND_UP:
            PlayAnim(AREQUI01_ANIM_STAND2, 0, 0);
            break;
        case SHARK_ST_LUNGE:
            FaceWolf(1);
            AimBiteRot(1);
            lungeFrom.x = pos.x;
            lungeFrom.y = pos.y;
            lungeFrom.z = pos.z;
            lungeTo.x = g_pWolf->pos.x;
            lungeTo.y = g_pWolf->pos.y;
            lungeTo.z = g_pWolf->pos.z;
            lungeVel.x = lungeTo.x - lungeFrom.x;
            lungeVel.y = lungeTo.y - lungeFrom.y;
            lungeVel.z = lungeTo.z - lungeFrom.z;
            d.x = lungeTo.x - lungeFrom.x;
            d.y = lungeTo.y - lungeFrom.y;
            d.z = lungeTo.z - lungeFrom.z;
            lungeLeft = (s32)LEN_XYZ(d);
            lungeLen = lungeLeft;
            if (lungeLeft == 0) {
                lungeVel.x = 0;
                lungeVel.y = 0;
                lungeVel.z = 0;
            } else {
                lungeVel.x = (s16)(d.x * 900 / lungeLeft);
                lungeVel.y = (s16)(d.y * 900 / lungeLeft);
                lungeVel.z = (s16)(d.z * 900 / lungeLeft);
            }
            PlayAnim(AREQUI01_ANIM_SWIM1, 1, 0);
            break;
        case SHARK_ST_BITE:
            /* cast kept: arg carries a number */
            biteAccepted = g_pWolf->HandleMessage(this, MSG_KILL, (void *)KILL_SHARK);
            PlayAnim(AREQUI01_ANIM_EAT, 0, 1);
            break;
        case SHARK_ST_MISS:
            PlayAnim(AREQUI01_ANIM_ATTACK1, 0, 0);
            break;
        case SHARK_ST_BRAKE:
            PlayAnim(AREQUI01_ANIM_BRAKE, 0, 0);
            break;
        case SHARK_ST_SWIM_BACK:
            ApplyBiteRot();
            d.x = lungeFrom.x - lungeTo.x;
            d.y = lungeFrom.y - lungeTo.y;
            d.z = lungeFrom.z - lungeTo.z;
            if (wolfDist == 0) {
                lungeVel.x = 0;
                lungeVel.y = 0;
                lungeVel.z = 0;
            } else {
                lungeVel.x = (s16)(d.x * 900 / wolfDist);
                lungeVel.y = (s16)(d.y * 900 / wolfDist);
                lungeVel.z = (s16)(d.z * 900 / wolfDist);
            }
            PlayAnim(AREQUI01_ANIM_SWIM1, 1, 0);
            break;
    }
}

/* 0x475fbd - the rotation that points from `from` to `to`: pitch from the horizontal distance (only when asked) and
 * a heading half a turn from atan2(dx, dz). */
void Shark::AimRotation(Vec3s *from, Vec3s *to, Vec3s *out, s32 pitch)
{
    Vec3i sq;
    Vec3i d;

    d.x = to->x - from->x;
    d.y = to->y - from->y;
    d.z = to->z - from->z;
    sq.x = d.x * d.x;
    sq.y = d.y * d.y;
    sq.z = d.z * d.z;
    if (!pitch)
        out->x = 0;
    else
        out->x = Math_RadiansToAngle4096((float)atan2((double)d.y, (double)(s32)sqrt((double)sq.x + sq.z))) & 0xfff;
    out->y = (Math_RadiansToAngle4096((float)atan2((double)d.x, (double)d.z)) + 0x800) & 0xfff;
    out->z = 0;
}

/* 0x4760cb - face Ralph. */
void Shark::FaceWolf(s32 pitch)
{
    Vec3s r;
    Vec3s *src;
    Vec3s *target;

    src = &pos;
    target = &g_pWolf->pos;
    AimRotation(src, target, &r, pitch);
    rot = r;
}

/* 0x47611a - the rotation pointing from Ralph back to the shark, kept for the swim home. */
void Shark::AimBiteRot(s32 pitch)
{
    Vec3s *src;
    Vec3s *target;

    src = &pos;
    target = &g_pWolf->pos;
    AimRotation(target, src, &biteRot, pitch);
}

/* 0x47615b */
void Shark::ApplyBiteRot()
{
    rot = biteRot;
}

/* 0x476180 - one patrol step. Within 80 units of the current waypoint the shark starts "cornering": it remembers the
 * headings of the leg it is leaving and the one it is taking and blends between them over the next 160 units, so the
 * turn is smooth instead of a snap. */
void Shark::FollowPath()
{
    ContactInfo contact;
    Vec4s v;
    s16 ang;
    Vec3s myPos;
    Vec4s delta;
    s32 len;
    s16 prev;
    s16 next;

    if (!cornering) {
        myPos = pos;
        delta.x = TRAJ_PT(path.traj, path.pointIndex).x - myPos.x;
        delta.y = TRAJ_PT(path.traj, path.pointIndex).y - myPos.y;
        delta.z = TRAJ_PT(path.traj, path.pointIndex).z - myPos.z;
        len = (s32)LEN_XZ(delta);
        if (len < 80) {
            cornering = 1;
            cornerDist = 0;
            prev = path.pointIndex;
            prev--;
            if (prev == -1)
                prev = (s16)(path.traj->count - 1);
            cornerHeadingIn = HeadingXZ(&TRAJ_PT(path.traj, prev), &TRAJ_PT(path.traj, path.pointIndex));
            next = path.pointIndex;
            next++;
            if (next >= path.traj->count)
                next = 0;
            cornerHeadingOut = HeadingXZ(&TRAJ_PT(path.traj, path.pointIndex), &TRAJ_PT(path.traj, next));
        }
    }
    /* cast kept: the Vec4s slot (8 bytes in the original frame) holds the Vec3s in its first three words */
    TrajFollower_Step(&path, (Vec3s *)&v, &ang);
    if (cornering)
        ang = CornerHeading();
    SetFacing(ang);
    v.y = 0;
    Vec3s_ScaleByDt((Vec3s *)&v, (Vec3s *)&delta); /* cast kept: as above */
    /* cast kept: as above */
    Collide_ResolveMove((Vec3s *)&delta, &contact, 0xb54, RESOLVE_SLIDE_ALL | RESOLVE_KEEP_Y, 0, 0, 0xa, 0, 0);
    Translate((Vec3s *)&delta); /* cast kept: as above */
    if (cornering) {
        cornerDist += (s32)LEN_XZ(delta);
        if (cornerDist > 160)
            cornering = 0;
    }
}

/* 0x47646e - the heading from one waypoint to the next, half a turn from atan2(dx, dz) as everywhere else. */
u16 Shark::HeadingXZ(Vec3s *from, Vec3s *to)
{
    s16 dx;
    s16 dz;

    dx = to->x - from->x;
    dz = to->z - from->z;
    return (Math_RadiansToAngle4096((float)atan2((double)dx, (double)dz)) + 0x800) & 0xfff;
}

/* 0x4764e7 - blend cornerHeadingIn into cornerHeadingOut over the first 160 units of the corner, going the short way
 * round the 4096-unit circle. */
u16 Shark::CornerHeading()
{
    s16 t;
    s16 res;
    s16 w;
    s16 ha;
    s16 total;
    s16 p1;
    s16 hb;
    s16 p2;

    total = 160;
    t = (s16)cornerDist;
    ha = cornerHeadingIn;
    hb = cornerHeadingOut;
    if (ha != hb) {
        if (ha < hb) {
            w = ha + 0x1000;
            p1 = hb - ha;
            p2 = w - hb;
            if (p1 < p2)
                res = ha + p1 * t / total;
            else
                res = w - p2 * t / total;
        } else {
            w = hb + 0x1000;
            p1 = ha - hb;
            p2 = w - ha;
            if (p1 < p2)
                res = ha - p1 * t / total;
            else
                res = ha + p2 * t / total;
        }
        return res & 0xfff;
    }
    return ha;
}

/* 0x476632 - the breathing bubbles, 90 units in front of the shark's nose, while it is 150+ below the surface. */
void Shark::UpdateBreathFx()
{
    Vec3s p;
    Vec3s q;
    s32 spawn;
    SharkFxScratch *s;

    p = pos;
    waterBox = Zones_Get(ZONE_WATER)->FindContaining(&p);
    if (waterBox) {
        /* cast kept: g_sharedScratch is one scratch buffer that each user lays out its own way */
        s = (SharkFxScratch *)g_sharedScratch;
        Mat34s_FromEulerScaled(&rot, &s->m, 0);
        s->offset.x = 0;
        s->offset.y = 0;
        s->offset.z = -90;
        Mat34s_TransformVec3s(&s->m, &s->offset, &s->out);
        q.x = s->out.x + p.x;
        q.y = s->out.y + p.y;
        q.z = s->out.z + p.z;
        q.y -= (s16)(75 - Rand_Bounded(25));
        q.x += (s16)(Rand_Bounded(50) - 25);
        q.z += (s16)(Rand_Bounded(50) - 25);
        breathParams.cap = waterBox->min[1];
        breathParams.size = (s16)(Rand_Bounded(10) + 10);
        spawn = waterBox != 0 && pos.y >= waterBox->min[1] + 150;
        breathFx.base.Emitter_UpdateRiseToCap(&breathParams, &q, spawn);
    }
}

/* 0x4767f4 - the charge bubbles, 90 units behind, only during the wind-up (7, short-lived and fast) and the lunge
 * (8, long-lived and slow). */
void Shark::UpdateChargeFx()
{
    Vec3s q;
    s32 spawn;
    SharkFxScratch *s;
    Vec3s origin;

    origin = pos;
    waterBox = Zones_Get(ZONE_WATER)->FindContaining(&origin);
    if (waterBox) {
        /* cast kept: g_sharedScratch is one scratch buffer that each user lays out its own way */
        s = (SharkFxScratch *)g_sharedScratch;
        Mat34s_FromEulerScaled(&rot, &s->m, 0);
        s->offset.x = 0;
        s->offset.y = 0;
        s->offset.z = 90;
        Mat34s_TransformVec3s(&s->m, &s->offset, &s->out);
        q.x = s->out.x + origin.x;
        q.y = s->out.y + origin.y;
        q.z = s->out.z + origin.z;
        q.y -= (s16)(75 - Rand_Bounded(25));
        q.x += (s16)(Rand_Bounded(50) - 25);
        q.z += (s16)(Rand_Bounded(50) - 25);
        chargeParams.cap = waterBox->min[1];
        chargeParams.size = (s16)(Rand_Bounded(10) + 10);
        if (state == SHARK_ST_LUNGE) {
            chargeParams.life = 0x2000;
            chargeParams.riseSpeed = -30;
        } else if (state == SHARK_ST_WIND_UP) {
            chargeParams.life = 0x400;
            chargeParams.riseSpeed = -120;
        }
        spawn = waterBox != 0 && pos.y >= waterBox->min[1] + 25;
        spawn = spawn & (state == SHARK_ST_WIND_UP || state == SHARK_ST_LUNGE);
        chargeFx.base.Emitter_UpdateRiseToCap(&chargeParams, &q, spawn);
    }
}

/* 0x476a33 - the ring on the water surface: the spawn point is the shark's position lifted to the water top and
 * jittered by +-25, and it only spawns while the shark swims within 150 of the surface. */
void Shark::UpdateRippleFx()
{
    Vec3s p;
    s32 spawn;

    p = pos;
    waterBox = Zones_Get(ZONE_WATER)->FindContaining(&p);
    if (waterBox)
        p.y = waterBox->min[1];
    p.x += (s16)(Rand_Bounded(50) - 25);
    p.z += (s16)(Rand_Bounded(50) - 25);
    spawn = waterBox != 0 && pos.y >= waterBox->min[1] && pos.y <= waterBox->min[1] + 150;
    rippleFx.base.Emitter_UpdateFade(&rippleParams, &p, FacingU16(), spawn);
}

/* 0x476b5b - vtable +0x10: the shark answers nothing. */
s32 Shark::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    return 0;
}

/* 0x476b6a - vtable +0x08: seen from above the water the shark is tinted blue (0xff6060 at strength 0x400); with the
 * camera under water the tint is dropped. Then the model and the three emitters. */
void Shark::Render(Camera *view)
{
    if (Zones_Get(ZONE_WATER)->FindContaining(&g_camPos)) {
        tintColor = 0;
        tintAmount = 0;
        SetInstFlag(INST_F_TINT, 0);
    } else {
        tintColor = 0xff6060;
        tintAmount = 0x400;
        SetInstFlag(INST_F_TINT, 1);
    }
    ScnMobile::Render(view);
    if (breathFx.base.flags.active)
        breathFx.base.Emitter_Render(view, 0);
    if (chargeFx.base.flags.active)
        chargeFx.base.Emitter_Render(view, 0);
    if (rippleFx.base.flags.active)
        rippleFx.RenderFlat(view, 1);
}

/* 0x476cfb - vtable +0x14: the effects and the bite reply. The state is NOT reset: a shark restarted mid-lunge keeps
 * running that state's body. */
void Shark::Reset()
{
    breathFx.base.Emitter_Reset();
    chargeFx.base.Emitter_Reset();
    rippleFx.base.Emitter_Reset();
    biteAccepted = 0;
}

/* 0x476d3d - the class factory for CLASSID 50 "Shark": new Shark (the base vtables, the three emitters' inline
 * constructors, the Shark vtable) and then ScnMobile::Init. */
ScnObject *Shark_Create(void *record)
{
    ScnBody *obj = new Shark;
    obj = obj->Init(record, 0);
    return obj;
}
