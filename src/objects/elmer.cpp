/* T078 - original object Elmer.cpp (guessed name): .text 0x448aa0-0x44b12d, .rdata 0x574e7c-0x574ea0 (vtable),
 * .data 0x57a824-0x57a830 (the cinematic header's stride-table copy; the slot is a guess in the map, the bytes are the
 * same either way). */
/* BYTES: dead-code, layout, slot-name. */
/* BYTES(layout): the Cine.h header static: every object including the cinematic header carries this copy in its .data, referenced or not */
/*
 * Elmer (class 90, vtable 0x574e7c, sizeof 0x168) - Elmer Fudd in Level 9 (disc Lvl-11). He hides in the TREESECTION
 * and comes out when Ralph, wearing the rabbit costume, walks into his DETECTBOX while the SWIRLSIGN says "rabbit
 * season" (msg 0x3080). Then he patrols his TRAJECTORY inside MOVINGBOX, and when Ralph is in range he stops, aims and
 * fires the level's one Bullet (msg 0x3203 places it at the muzzle, 0x3200 fires it, 0x3201 asks whether it is still
 * flying). SheepD3D.exe 0x448aa0-0x44b12c: PostLoadInit, Reset, Update, the walk-and-collide step, HandleMessage,
 * SetState and the class factory.
 *
 * States (+0x7c): 0 hidden in the tree, 1 stepping out (anim 9, the scripted CAMERA, Ralph frozen with Wolf msg 0xe),
 * 2 walking toward Ralph, 3 the shot itself (anim 7: the bullet is placed 0x54 units ahead at head height and fired),
 * 5 patrolling the trajectory, 6 blocked by an object, 7 raising the gun (anim 5), 8 the talking ambush (anim 4 plus
 * the RABBITTEXT / DAFFYTEXT line, chosen by what the swirl sign shows), 10 the close-range pounce check, 11 the miss,
 * 12 the caught pose (entered straight from msg 0x6f, the Bullet's "I hit him"), 13 walking back, 14 idling in the
 * box, 15 refused: the walk ran into an EXCEPTMOVINGBOX hole.
 * Devices that only pin the original code generation: the inline helpers below (their names are not recovered; each
 * one is here because its expansion gives the original's stack temporaries) and the local names - under /Od a local's
 * slot follows from a hash of its name (tools/vc6_locals.py frame/pick), which is why Update's locals are called
 * heading, spare, move, swing, d2, delta, yaw, stand, reach, lookDir. `spare` is a named local the original never
 * reads; its slot is there in the frame (EBP-4) between the heading and the velocity, so the source had a variable it
 * did not use.
 */

#define SDW_MEMBERS_ScnObject            \
    static void *operator new(u32 size); \
    void SetFacing(s16 f);               \
    /* inline: the handle is a temp (0x44a6cc) */


#define SDW_MEMBERS_Elmer                            \
    s16 FollowerIndex()                              \
    {                                                \
        return follower.pointIndex;                  \
    } /* inline: a stack copy per read (0x4497d6) */ \
    void SetFollowerIndex(s16 i)                     \
    {                                                \
        follower.pointIndex = i;                     \
    } /* inline: its argument is a temp (0x44988f) */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *inst, Animator *animator, u16 id, u32 opts); /* 0x550196 */
#include "../engine/sound_mgr.h"
#include "../engine/scn_tools.h"
#include "../engine/id_list.h"
#include "animation.h"
#include "../engine/approach.h"
#include "camera.h"
#include "../app/app_main.h"
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
#define SDW_INLINE_SCNOBJECT_SOUNDISPLAYING_U16 1
#define SDW_INLINE_SCNOBJECT_STOPSOUND_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_FACING
#undef SDW_INLINE_SCNOBJECT_SETFACING_S16
#undef SDW_INLINE_SCNOBJECT_SETVISIBLE_S32
#undef SDW_INLINE_SCNOBJECT_SOUNDISPLAYING_U16
#undef SDW_INLINE_SCNOBJECT_STOPSOUND_U16
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_ANIMID 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_ANIMID
#define SDW_INLINE_SHADOW_INVALIDATE 1
#include "../engine/shadow_inlines.h"
#undef SDW_INLINE_SHADOW_INVALIDATE
/* 0x57a824 - the cinematic header's static copy of the 9-byte opcode stride table (src/engine/cine.cpp,
 * g_cineOpStride 0x5816fc). The original header defined it static, so every object including it carries its own
 * unreferenced copy at the head of its .data; defined here in its place. */
static u8 s_cineOpStride[9] = {0, 8, 8, 4, 2, 2, 4, 2, 2};
#include "../engine/property_math.h"

s32 Scenaric_FindByClass(u16 classId, ScnObject **out, s32 max);              /* 0x5145c5 */
u16 Sound_Play(u16 soundId, void *owner, u16 volume, u8 playFlags, s32 rate); /* 0x5491b8 */
s32 Vec3s_DistSqXZ(Vec3s *a, Vec3s *b);                                       /* 0x51588e */
#include "../sdk/crt.h"
extern "C" s16 Math_RadiansToAngle4096(float radians); /* 0x5269ce */
void Camera_StartScripted(ScnObject *owner, Camera *cam, u16 rotX, u16 rotY, u16 rotZ, Vec3s *pos, u16 focal, u32 mode,
                          s32 time);                                         /* 0x55a70d */
void Dialogue_Reset();                                                       /* 0x539507 */
u8 Dialogue_Say(const char *text, s32 voiceId, ScnObject *speaker, u32 arg); /* 0x539893 */

extern Wolf *g_pWolf;                /* 0x6cf310 */
extern s32 g_dtMs;                   /* 0x71b2e8  g_dt * 1000 >> 12 */
extern "C" s16 g_sinTable4096[5122]; /* 0x57ece0  4.12 sine, 4096 steps per turn */
extern "C" const s16 *g_pCosTable;   /* 0x5814e4  = g_sinTable4096 + 1024 */

/* A designer property of the WAR record: the dword at record + 0x14 + offset (ElmerProps). Inlined; the offset is a
 * stack temp (0x448b38), which it is with a u32 parameter. */
#define SDW_INLINE_FREE_SCN_GETPROPU32_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_SCN_GETPROPU32_VOID_U32

/* Resolve a *BOX id-list property into the pair {Box **list; u16 count} that follows it in the object: the pair's
 * address is a stack temp and the count is reached through it (0x448b4b-0x448b6e). */
inline void IdList_Find(Box ***list, u16 id)
{
    /* cast kept: an id list holds record addresses as u32, and its count is the u16 after the list pointer */
    *list = (Box **)Scn_FindIdList(id, (u16 *)(list + 1));
}

/* Whether p lies inside box on the two horizontal axes (both faces inclusive): its two parameters are stack temps
 * (0x448eb5), except where the point is a local, which needs no temp (0x44a451). */
#define SDW_INLINE_FREE_BOX_CONTAINSPOINTXZ_BOX_VEC3S 1
#include "../engine/coll_box_inlines.h"
#undef SDW_INLINE_FREE_BOX_CONTAINSPOINTXZ_BOX_VEC3S

/* Camera_StartScripted through an inline: the rotation, position and focal arguments are stack temps, stored right to
 * left (0x44a7c5-0x44a836); the owner, camera, mode and time are passed as they are. */
#define SDW_INLINE_FREE_CAMERA_SCRIPT_SCNOBJECT_CAMERA_U16_U16_U16_VEC3S_U16_U32_S32 1
#include "camera_inlines.h"
#undef SDW_INLINE_FREE_CAMERA_SCRIPT_SCNOBJECT_CAMERA_U16_U16_U16_VEC3S_U16_U32_S32

/* Set / clear bits of a 16-bit flag word: the word's address is a stack temp and the mask is loaded into a register
 * (0x44a57b-0x44a5b0). */
#define SDW_INLINE_FREE_FLAGS16_SET_U16_U16 1
#include "instance_inlines.h"
#undef SDW_INLINE_FREE_FLAGS16_SET_U16_U16

#define SDW_INLINE_FREE_FLAGS16_CLEAR_U16_U16 1
#include "instance_inlines.h"
#undef SDW_INLINE_FREE_FLAGS16_CLEAR_U16_U16

#define SDW_INLINE_SCNOBJECT_SETTINTOVERRIDE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETTINTOVERRIDE_S32

/* 0x448aa0 - vtable +0x00: the ten designer properties, the trajectory follower, the two localised lines, and the
 * home position taken from the TREESECTION and snapped to the ground. */
void Elmer::PostLoadInit()
{
    void *rec;
    rec = record;
    bullet = Scn_GetPropObject(rec, 0);
    camera = Scn_GetPropCamera(rec, 4);
    detectBox = Scn_GetPropBox(rec, 0xc);
    bullet->HandleMessage(this, MSG_BULLET_SET_BOX, detectBox);
    movingBox = Scn_GetPropBox(rec, 0x14);
    IdList_Find(&exceptBoxes, (u16)Scn_GetPropU32(rec, 0x10));
    treeSection = Scn_GetPropObject(rec, 0x24);
    swirlSign = Scn_GetPropObject(rec, 0x1c);
    Scenaric_FindByClass(CLASSID_DAFFYLEVEL09, &daffy, 1);
    trajectory = Scn_GetPropTrajectory(rec, 0x20);
    TrajFollower_Init(&follower, trajectory, 0x50, 0x800, 1, 1, 0x32);
    rabbitText = Text_GetClassString((u8)Scn_GetPropU32(rec, 0x18));
    daffyText = Text_GetClassString((u8)Scn_GetPropU32(rec, 8));
    unk0b0 = 0;
    shootTimerMs = -1;
    waypointDir = 1;
    SetState(ELMER_ST_HIDDEN);
    homePos = treeSection->pos;
    SetPosition(&homePos);
    SnapToGround(0);
    homePos = pos;
    bulletBusy = 0;
    wolfIsRabbit = 1;
    restart = 1;
    canAmbush = 1;
    speed = 0x1f4;
    soundHandle = 0;
    sweepRate = 0x1000 / (s32)(Anim_GetDurationMs(Inst(), AELMER01_ANIM_SNEAK, 0) / 2);
    sweepPhase = 0;
}

/* 0x448d53 - vtable +0x14: stops the bullet, and if Elmer is out of the tree puts him back on the first trajectory
 * point, on the ground, patrolling. */
void Elmer::Reset()
{
    Vec3s p;
    bullet->HandleMessage(this, MSG_BULLET_RESET, 0);
    wolfIsRabbit = 0;
    canAmbush = 1;
    soundHandle = 0;
    if (state) {
        TrajFollower_Init(&follower, trajectory, 0x50, 0x800, 1, 1, 0x32);
        p.x = trajectory->pts[0].x;
        p.y = trajectory->pts[0].y;
        p.z = trajectory->pts[0].z;
        p.y -= 0xc8;
        p.y = QueryGroundY(&p, 1);
        SetPosition(&p);
        restart = 1;
        SetState(ELMER_ST_PATROL);
    }
}

/* 0x448e59 - vtable +0x04: the per-frame facts about Ralph (in the detect box, in the moving box, near, far), then the
 * state machine, then the shot cooldown. */
/* BYTES(dead-code): spare is never used: the original frame has an unused slot at EBP-4 */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py) */
void Elmer::Update()
{
    Vec3s lookDir;
    Vec3s reach;
    Vec3s stand;
    s32 yaw;
    Vec3s delta;
    s32 d2;
    s32 swing;
    Vec3s move;
    s16 spare;
    s16 heading;

    wolfPos = g_pWolf->pos;
    d2 = Vec3s_DistSq(&wolfPos, &pos);
    wolfSpotted = Box_ContainsPointXZ(detectBox, &wolfPos) &&
                  g_pWolf->HandleMessage(this, MSG_WOLF_IS_RABBITCOSTUME, 0) &&
                  swirlSign->HandleMessage(this, MSG_SWIRLSIGN_IS_B, 0);
    bulletBusy = bullet->HandleMessage(this, MSG_BULLET_IS_BUSY, 0);
    wolfNear = d2 < 0x62e08;
    wolfFar = d2 > 0x31704;
    wolfInBox = Box_ContainsPointXZ(movingBox, &wolfPos);
    if (d2 > 0x15f90)
        canAmbush = 1;
    if (shootTimerMs < 0)
        selfInBox = Box_ContainsPointXZ(movingBox, &pos);
    else
        selfInBox = 1;
    switch (state) {
        case ELMER_ST_HIDDEN:
            if (wolfSpotted)
                SetState(ELMER_ST_APPEAR);
            break;
        case ELMER_ST_APPEAR:
            if (AnimFlags(ANIM_F_FINISHED)) {
                stand.x = pos.x;
                stand.y = pos.y;
                stand.z = pos.z;
                stand.y -= 0x96;
                stand.z -= 0x80;
                lookDir.x = wolfPos.x - pos.x;
                lookDir.y = wolfPos.y - pos.y;
                lookDir.z = wolfPos.z - pos.z;
                SetFacing((s16)((Math_RadiansToAngle4096((float)atan2((double)lookDir.x, (double)lookDir.z)) + 0x800) &
                                0xfff));
                SetPosition(&stand);
                SnapToGround(1);
                SetState(ELMER_ST_CHASE);
            }
            shadow.Invalidate();
            break;
        case ELMER_ST_CHASE:
            if (d2 <= 0x1fa4) {
                SetState(ELMER_ST_HIT);
                break;
            }
            if (selfInBox) {
                if (wolfFar) {
                    if (wolfNear)
                        SetState(ELMER_ST_CHASE_CLOSE);
                    else if (bulletBusy)
                        SetState(ELMER_ST_CHASE_CLOSE);
                    else
                        SetState(ELMER_ST_SHOOT);
                } else
                    MoveToward(&wolfPos, 1);
            } else
                SetState(ELMER_ST_LOOK_AROUND);
            break;
        case ELMER_ST_SHOOT:
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(ELMER_ST_SHOOT_END);
            break;
        case ELMER_ST_SHOOT_END:
            if (AnimFlags(ANIM_F_FINISHED)) {
                Camera_ReleaseScripted(this);
                if (wolfFrozen)
                    /* cast kept: arg carries a number */
                    wolfFrozen = !g_pWolf->HandleMessage(this, MSG_UNFREEZE, (void *)1);
                if (selfInBox) {
                    SetState(ELMER_ST_LOOK_AROUND);
                    break;
                }
                if (wolfNear)
                    SetState(ELMER_ST_CHASE_CLOSE);
                else {
                    restart = 1;
                    SetState(ELMER_ST_PATROL);
                }
            }
            break;
        case ELMER_ST_CHASE_CLOSE:
            if (!selfInBox) {
                if (!bulletBusy)
                    SetState(ELMER_ST_SHOOT);
                else
                    SetState(ELMER_ST_LOOK_AROUND);
            } else if (!wolfNear && !bulletBusy)
                SetState(ELMER_ST_SHOOT);
            else if (d2 <= 0x1fa4)
                SetState(ELMER_ST_HIT);
            else
                MoveToward(&wolfPos, 1);
            break;
        case ELMER_ST_HIT:
            delta.x = wolfPos.x;
            delta.y = wolfPos.y;
            delta.z = wolfPos.z;
            delta.x -= pos.x;
            delta.y -= pos.y;
            delta.z -= pos.z;
            yaw = (Math_RadiansToAngle4096((float)atan2((double)delta.x, (double)delta.z)) + 0x800) & 0xfff;
            if ((s16)((s16)((yaw - Facing() + 0x800) & 0xfff) - 0x800) < 0x64 && d2 <= 0x1fa4) {
                SetState(ELMER_ST_HIT3);
                g_pWolf->HandleMessage(this, MSG_KILL, (void *)KILL_CANNONBALL); /* cast kept: arg carries a number */
            } else
                SetState(ELMER_ST_HIT2);
            break;
        case ELMER_ST_HIT2:
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(ELMER_ST_CHASE);
            break;
        case ELMER_ST_PATROL:
            if (wolfSpotted)
                SetState(ELMER_ST_CHASE);
            if (canAmbush && d2 < 0x9c40 && g_pWolf->HandleMessage(this, MSG_WOLF_IS_NOISY, 0)) {
                SetState(ELMER_ST_AMBUSH);
                break;
            }
            if (AnimFlags(ANIM_F_FINISHED))
                sweepPhase = 0;
            sweepPhase += (u16)g_dtMs;
            swing = g_pCosTable[(sweepPhase * sweepRate) & 0xfff] * 0x28;
            follower.speed = (s16)((swing >> 12) + 0x3c);
            TrajFollower_Step(&follower, &move, &heading);
            if (FollowerIndex() != waypoint) {
                waypoint += waypointDir;
                if (waypoint >= trajectory->count)
                    waypoint = 0;
                if (waypoint < 0)
                    waypoint = trajectory->count - 1;
                SetFollowerIndex(waypoint);
                restart = 0;
            }
            newFacing = Math_ApproachAngle(Facing(), heading, &turnRate, 0x7530, 0x7530, 0x7530, 1);
            SetFacing(newFacing);
            move.y += 0x31;
            Vec3s_ScaleByDt(&move, &step);
            if (Collide_ResolveMove(&step, &contact, 0x578, COLL_FLOOR | COLL_FLOOR_EDGE | COLL_10, 0, 0, 0xa, 0, 0) &
                COLL_WALL) {
                if (!restart) {
                    if (AnimId() == AELMER01_ANIM_SNEAK)
                        PlayAnim(AELMER01_ANIM_STAND2, 1, 1);
                    pauseTimerMs -= g_dtMs;
                    if (pauseTimerMs <= 0) {
                        if (AnimId() == AELMER01_ANIM_STAND2)
                            PlayAnim(AELMER01_ANIM_SNEAK, 1, 1);
                        pauseTimerMs = 0x3e8;
                        waypointDir = waypointDir * -1;
                        waypoint += waypointDir;
                        if (waypoint >= trajectory->count)
                            waypoint = 0;
                        if (waypoint < 0)
                            waypoint = trajectory->count - 1;
                        SetFollowerIndex(waypoint);
                    }
                } else {
                    blocker = contact.wallObj;
                    if (blocker)
                        SetState(ELMER_ST_BLOCKED);
                    else
                        Translate(&step);
                }
            } else {
                if (AnimId() == AELMER01_ANIM_STAND2)
                    PlayAnim(AELMER01_ANIM_SNEAK, 1, 1);
                Translate(&step);
            }
            break;
        case ELMER_ST_BLOCKED:
            if (Vec3s_DistSqXZ(&blocker->pos, &pos) > 0x3840)
                SetState(ELMER_ST_PATROL);
            break;
        case ELMER_ST_AMBUSH:
            waitTimerMs -= (s16)g_dtMs;
            if (waitTimerMs <= 0 && AnimId() != AELMER01_ANIM_STAND3)
                PlayAnim(AELMER01_ANIM_STAND3, 1, 1);
            reach.x = wolfPos.x - pos.x;
            reach.y = wolfPos.y - pos.y;
            reach.z = wolfPos.z - pos.z;
            heading = (s16)((Math_RadiansToAngle4096((float)atan2((double)reach.x, (double)reach.z)) & 0xfff) + 0x800);
            newFacing = Math_ApproachAngle(Facing(), heading, &turnRate, 0x7530, 0x7530, 0x7530, 1);
            SetFacing(newFacing);
            if (wolfSpotted) {
                Dialogue_Reset();
                SetState(ELMER_ST_CHASE);
            }
            if (swirlSign->HandleMessage(this, MSG_SWIRLSIGN_IS_B, 0)) {
                if (!Dialogue_Say(rabbitText, VOICE_TXTEL_LVL1101, this, 1)) {
                    Dialogue_Reset();
                    SetState(ELMER_ST_PATROL);
                }
            } else if (!Dialogue_Say(daffyText, VOICE_TXTEL_LVL1102, this, 1)) {
                Dialogue_Reset();
                SetState(ELMER_ST_PATROL);
            }
            break;
        case ELMER_ST_LOOK_AROUND:
            if (!wolfSpotted && AnimFlags(ANIM_F_FINISHED)) {
                restart = 1;
                SetState(ELMER_ST_PATROL);
                break;
            }
            if (wolfSpotted && (wolfFar || wolfNear) && !bulletBusy)
                SetState(ELMER_ST_SHOOT);
            else if (wolfInBox) {
                shootTimerMs = 0x400;
                if (bulletBusy) {
                    restart = 1;
                    SetState(ELMER_ST_PATROL);
                } else
                    SetState(ELMER_ST_CHASE);
            }
            break;
        case ELMER_ST_GO_HOME:
            if (wolfSpotted && !bulletBusy)
                SetState(ELMER_ST_SHOOT);
            else if (MoveToward(&wolfPos, 0))
                SetState(ELMER_ST_CHASE);
            break;
    }
    shootTimerMs -= g_dtMs;
    AdvanceAnim();
}

/* 0x44a0af - turn toward target and take one step, with the walk / run animation chosen by whether Ralph is the
 * rabbit. Returns 0 when the step would put him in an EXCEPTMOVINGBOX hole (with canRefuse, that is state 15). */
s32 Elmer::MoveToward(Vec3s *target, s32 canRefuse)
{
    Vec3s next;
    Vec3s dir;
    s32 isRabbit;
    u8 i;

    dir.x = target->x - pos.x;
    dir.y = target->y - pos.y;
    dir.z = target->z - pos.z;
    heading = Math_RadiansToAngle4096((float)atan2((double)dir.x, (double)dir.z)) & 0xfff;
    newFacing = Math_ApproachAngle(Facing(), heading + 0x800, &turnRate, 0xea60, 0xea60, 0xea60, 1);
    SetFacing(newFacing);
    isRabbit = g_pWolf->HandleMessage(this, MSG_WOLF_IS_RUNNING, 0);
    if (wolfIsRabbit != isRabbit) {
        if (isRabbit) {
            speed = 0x3e8;
            if (!SoundIsPlaying(soundHandle))
                soundHandle =
                    Sound_Play(SND_RUN_LOOP_ELMER, this, 0xff, SNDF_LOOP | SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 0x1000);
            PlayAnim(AELMER01_ANIM_RUN2, 1, 0);
        } else {
            speed = 0x1f4;
            if (SoundIsPlaying(soundHandle))
                StopSound(soundHandle);
            PlayAnim(AELMER01_ANIM_RUN, 1, 0);
        }
        wolfIsRabbit = isRabbit;
    }
    dir.x = (s16)(g_sinTable4096[heading] * speed >> 12);
    dir.y = 0x31;
    dir.z = (s16)(g_pCosTable[heading] * speed >> 12);
    Vec3s_ScaleByDt(&dir, &step);
    Collide_ResolveMove(&step, &contact, 0xb54, COLL_FLOOR, 0, 0, 0xa, 0, 0);
    next.x = step.x;
    next.y = step.y;
    next.z = step.z;
    next.x = next.x * 2;
    next.z = next.z * 2;
    next.x += pos.x;
    next.y += pos.y;
    next.z += pos.z;
    next.y -= 0x14;
    for (i = 0; i < exceptBoxCount; i++) {
        if (Box_ContainsPointXZ(exceptBoxes[i], &next)) {
            if (canRefuse)
                SetState(ELMER_ST_GO_HOME);
            return 0;
        }
    }
    Translate(&step);
    return 1;
}

/* 0x44a4dc - vtable +0x10: MSG_KILL from a mine blackens him, msg 0xe releases the frozen Ralph, msg 0x6f is the
 * Bullet reporting a hit, msg 0x76 restarts the ambush wait. */
s32 Elmer::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    switch (msgId) {
        case MSG_VOICE_STARTED:
            waitTimerMs = 0x1f4;
            break;
        case MSG_FREEZE:
            wolfFrozen = 0;
            return 1;
        case MSG_KILL:
            if (sender->GetClassId() == CLASSID_GROUNDMINE || sender->GetClassId() == CLASSID_DEFUSABLEMINE) {
                tintColor = 0;
                tintAmount = 0x1000;
                SetTintOverride(1);
                return 1;
            }
            break;
        case MSG_BULLET_DONE:
            state = ELMER_ST_HIT3;
            PlayAnim(AELMER01_ANIM_STAND2, 0, 0);
            break;
    }
    return 0;
}

/* 0x44a697 - enter a state: stops the walk sound, does the state's entry work, and records the new state last. */
void Elmer::SetState(u8 newState)
{
    s32 distSq;
    u8 i;
    s32 bestDist;
    Vec3s d;

    if (SoundIsPlaying(soundHandle))
        StopSound(soundHandle);
    switch (newState) {
        case ELMER_ST_HIDDEN:
            PlayAnim(AELMER01_ANIM_STAND, 0, 0);
            SetVisible(0);
            break;
        case ELMER_ST_BLOCKED:
            PlayAnim(AELMER01_ANIM_STAND2, 1, 1);
            break;
        case ELMER_ST_APPEAR:
            Camera_Script(this, &g_camera, camera->rot[0], camera->rot[1], camera->rot[2], &camera->eye, camera->focal,
                          0, 0x1000);
            wolfFrozen = g_pWolf->HandleMessage(this, MSG_FREEZE, (void *)1); /* cast kept: arg carries a number */
            PlayAnim(AELMER01_ANIM_APPEAR, 0, 0);
            SetVisible(1);
            treeSection->HandleMessage(this, MSG_TREESECTION_CUT, 0);
            break;
        case ELMER_ST_CHASE:
            turnRate = 0;
            if (wolfIsRabbit)
                PlayAnim(AELMER01_ANIM_RUN2, 1, 0);
            else
                PlayAnim(AELMER01_ANIM_RUN, 1, 0);
            break;
        case ELMER_ST_SHOOT:
            PlayAnim(AELMER01_ANIM_SHOOT1, 0, 0);
            d.x = wolfPos.x - pos.x;
            d.y = wolfPos.y - pos.y;
            d.z = wolfPos.z - pos.z;
            SetFacing((s16)((Math_RadiansToAngle4096((float)atan2((double)d.x, (double)d.z)) + 0x800) & 0xfff));
            break;
        case ELMER_ST_SHOOT_END:
            PlayAnim(AELMER01_ANIM_SHOOT3, 0, 0);
            d.x = pos.x;
            d.y = pos.y;
            d.z = pos.z;
            d.y -= 0x32;
            d.x += (s16)(g_sinTable4096[Facing()] * 0x54 >> 12);
            d.z += (s16)(g_pCosTable[Facing()] * 0x54 >> 12);
            bullet->HandleMessage(this, MSG_BULLET_SET_POS, &d);
            bullet->HandleMessage(this, MSG_BULLET_FIRE, 0);
            break;
        case ELMER_ST_RUN:
            if (newState != state) {
                if (wolfIsRabbit)
                    PlayAnim(AELMER01_ANIM_RUN2, 1, 0);
                else
                    PlayAnim(AELMER01_ANIM_RUN, 1, 0);
            }
            break;
        case ELMER_ST_PATROL:
            PlayAnim(AELMER01_ANIM_SNEAK, 1, 0);
            if (wolfFrozen)
                /* cast kept: arg carries a number */
                wolfFrozen = !g_pWolf->HandleMessage(this, MSG_UNFREEZE, (void *)1);
            pauseTimerMs = 0x3e8;
            turnRate = 0;
            if (restart == 1) {
                bestDist = 0x7fffffff;
                for (i = 0; i < trajectory->count; i++) {
                    distSq = Vec3s_DistSqXZ(&trajectory->pts[i], &pos);
                    if (bestDist > distSq) {
                        bestDist = distSq;
                        waypoint = i;
                    }
                }
                SetFollowerIndex(waypoint);
            }
            break;
        case ELMER_ST_AMBUSH:
            wolfFrozen = g_pWolf->HandleMessage(this, MSG_FREEZE, (void *)1); /* cast kept: arg carries a number */
            waitTimerMs = 0xfa0;
            canAmbush = 0;
            break;
        case ELMER_ST_CHASE_CLOSE:
            Camera_ReleaseScripted(this);
            if (wolfIsRabbit)
                PlayAnim(AELMER01_ANIM_RUN2, 1, 0);
            else
                PlayAnim(AELMER01_ANIM_RUN, 1, 0);
            break;
        case ELMER_ST_HIT:
            PlayAnim(AELMER01_ANIM_HIT, 0, 0);
            d.x = wolfPos.x - pos.x;
            d.y = wolfPos.y - pos.y;
            d.z = wolfPos.z - pos.z;
            SetFacing((s16)((Math_RadiansToAngle4096((float)atan2((double)d.x, (double)d.z)) + 0x800) & 0xfff));
            break;
        case ELMER_ST_HIT2:
            PlayAnim(AELMER01_ANIM_HIT1, 0, 0);
            break;
        case ELMER_ST_HIT3:
            PlayAnim(AELMER01_ANIM_HIT2, 0, 0);
            break;
        case ELMER_ST_LOOK_AROUND:
            PlayAnim(AELMER01_ANIM_STAND, 0, 0);
            break;
        case ELMER_ST_GO_HOME:
            PlayAnim(AELMER01_ANIM_STAND, 0, 0);
            break;
    }
    state = newState;
}

/* 0x44b0bd - the class factory for CLASSID 90 "Elmer": new Elmer (the base vtables in turn, then Elmer's), then
 * ScnMobile_Init(record, 0) through vtable slot +0x20. */
ScnObject *Elmer_Create(void *record)
{
    ScnBody *obj = new Elmer;
    obj = obj->Init(record, 0);
    return obj;
}
