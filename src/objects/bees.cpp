/*
 * Bees (class 107 "Bees", vtable 0x575a34, sizeof 0xf8) - the swarm that circles the beehives in Level 10
 * (disc Lvl-12). A cannonball on a hive sends the swarm to the next one; once the honey pot has been spilled and
 * Sam is standing in it the swarm chases and stings him, and Ralph gets stung if he walks into BOXDETECTWOLF.
 * SheepD3D.exe 0x4971f0-0x499021, the whole file (it ends with the factory, followed by int3 padding).
 *
 * Two clocks decide the behaviour: `timer` (chaseTime[alert], set by msg 0x3980 from the level's manager) bounds the
 * chase, and `settleTimer` (150 ms, counted only while flying to a hive in state 0xE) latches the level-wide
 * g_beesLeftFirstHive, after which a cannonball on the first hive sends EVERY swarm home to the mother hive
 * (broadcast 0x3982) instead of on to the second one.
 *
 * Devices that only pin the original code generation (the inline helpers have no bodies in the exe, so their names are
 * not recovered): Box_ContainsXZ / Box_Contains give their two argument temps and their 0/1 result temp (0x497516,
 * 0x4975ee); IsInWorld returns the comparison as 0/1, hence the neg/sbb/neg without a byte truncation (0x497968);
 * PlayAnim, AnimFlags, GetClassId, SetFacing, SetPartHeight, SetCollidable and SetUpdateMode are the shared ones;
 * StopSound / IsSoundPlaying give the u16 handle temps.
 * The local names were chosen for their stack slots (tools/vc6_locals.py): distHive / dHome / spot / ds / vertOff /
 * obj / dir / landOn / wolfSpot in Update, d / cam in SetState, turn / newPos / v / m in Orbit, move / toTarget /
 * speedVec / len in MoveToward.
 *
 * Two defects worth recording. Reset (0x498f3c) sets the state to 9 and only then asks whether the state is 7
 * (0x498f9e), which SetState has just overwritten, so the "stay home" branch is dead and a restart always puts the
 * swarm back on its hive. And the sting states 0x11 and 0x12 put the swarm at a FIXED height (y = -0x8c,
 * 0x497c72 and 0x497d08) instead of that much above its target, so the bee snaps to a height that has nothing to do
 * with where Ralph or Sam is standing.
 */
/* BYTES: slot-name. */

#define SDW_MEMBERS_ScnObject            \
    static void *operator new(u32 size); \
    void SetFacing(u16 f);               \
    /* inline, defined below */          \
    void SetUpdateMode(u8 mode); /* inline, defined below */

#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *inst, Animator *animator, u16 id, u32 opts); /* 0x550196 (used by the PlayAnim inline) */
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_ISINWORLD 1
#define SDW_INLINE_SCNOBJECT_SETPARTHEIGHT_U32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_ISINWORLD
#undef SDW_INLINE_SCNOBJECT_SETPARTHEIGHT_U32
#define SDW_INLINE_SCNOBJECT_SETFACING_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETFACING_U16
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#include "../engine/property_math.h"

#define SDW_ABS(v) ((v) >= 0 ? (v) : -(v))

extern Wolf *g_pWolf; /* 0x6cf310 */

#include "../app/app_main.h"
#include "honeypot.h"
#include "../engine/scn_tools.h"
#include "animation.h"
#include "world_draw.h"
#include "../engine/fixed_math.h"
#include "../engine/sound_mgr.h"
#define g_camPos (g_camera.pos) /* 0x584d20 */
extern s32 g_dtMs;              /* 0x71b2e8  g_dt * 1000 >> 12 */

s32 Scenaric_FindByClass(u16 classId, ScnObject **out, s32 max);          /* 0x5145c5 */
u16 Sound_Play(u16 soundId, void *owner, u16 volume, u8 flags, s32 rate); /* 0x5491b8 */
#include "../sdk/crt.h"
extern "C" s16 Math_RadiansToAngle4096(float radians); /* 0x5269ce */

/* cleared by every Bees PostLoadInit, set once a swarm's settleTimer has run out while it flew to a hive */
s32 g_beesLeftFirstHive; /* 0x6cf44c */

/* ---- inline helpers ---- */

#define SDW_INLINE_SCNOBJECT_STOPSOUND_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_STOPSOUND_U16

#define SDW_INLINE_SCNOBJECT_ISSOUNDPLAYING_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_ISSOUNDPLAYING_U16

/* SCN_OF_NO_BOX_COLLIDE (0x400) off or on. */
#define SDW_INLINE_SCNOBJECT_SETCOLLIDABLE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETCOLLIDABLE_S32

/* ScnUpdateMode: 0 near the camera only, 1 always, 2 never, 3 also during cinematics. */
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8

/* p inside the box on x and z only. */
inline s32 Box_ContainsXZ(CollBox *b, Vec3s *p)
{
    return (p->x >= b->min.x && p->x <= b->max.x && p->z >= b->min.z && p->z <= b->max.z) ? 1 : 0;
}

/* p inside the box on all three axes. */
inline s32 Box_Contains(CollBox *b, Vec3s *p)
{
    return (p->x >= b->min.x && p->x <= b->max.x && p->y >= b->min.y && p->y <= b->max.y && p->z >= b->min.z &&
            p->z <= b->max.z)
               ? 1
               : 0;
}

/* ---- Bees ---- */

/* 0x4971f0 - vtable +0x00: the three hives, the two boxes, the HoneyPot and Sam, the chase times by alert level,
 * then park on the first hive and tell it the swarm is there (msg 0x3e03). */
void Bees::PostLoadInit()
{
    u16 *rec;

    rec = record;
    orbitAngle = 0;
    potLeftFlyBox = 0;
    stungHome = 0;
    g_beesLeftFirstHive = 0;
    chaseTime[0] = 450;
    chaseTime[1] = 300;
    chaseTime[2] = 150;
    chaseTime[3] = 0;
    settleTimer = 150;
    honeyPot = 0;
    Scenaric_FindByClass(CLASSID_HONEYPOT, &honeyPot, 1);
    sam = 0;
    Scenaric_FindByClass(CLASSID_SAM, &sam, 1);
    /* cast kept (both): Box and CollBox are two views of one 16-byte zone record */
    flyBox = (CollBox *)Scn_GetPropBox(rec, 4);        /* PROPERTY_BEES_BOXFLY */
    detectWolfBox = (CollBox *)Scn_GetPropBox(rec, 0); /* PROPERTY_BEES_BOXDETECTWOLF */
    firstHive = Scn_GetPropObject(rec, 8);             /* PROPERTY_BEES_FIRSTHIVE */
    secondHive = Scn_GetPropObject(rec, 0x10);         /* PROPERTY_BEES_SECONDHIVE */
    motherHive = Scn_GetPropObject(rec, 0xc);          /* PROPERTY_BEES_MOTHERHIVE */
    animDuration = Anim_GetDurationMs(Inst(), AABEIL01_ANIM_OUT, 1);
    SetPosition(&firstHive->pos);
    hive = firstHive;
    firstHive->HandleMessage(this, MSG_HIVE_SWARM_HOME, 0);
    SetCollidable(1);
    renderRot.x = 0x400;
    renderRot.y = 0;
    renderRot.z = 0;
    SetUpdateMode(SCN_UPD_ALWAYS);
    soundHandle = 0;
    SetState(BEES_ST_ORBIT_HIVE);
    hiveAngle = 0;
    camAngle = 0;
}

/* 0x4974b2 - vtable +0x04: the honey-pot watch, Ralph's detection box, then the state machine. */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py) */
void Bees::Update()
{
    s32 distHive;
    s32 dHome;
    Vec3s spot;
    s32 ds;
    s32 vertOff;
    ScnObject *obj;
    Vec3s dir;
    Vec3s landOn;
    Vec3s wolfSpot;

    if (!Box_ContainsXZ(flyBox, &honeyPot->pos))
        potLeftFlyBox = 1;
    if (stungHome == 0 && state != BEES_ST_CHASE_WOLF && state != BEES_ST_STING_WOLF) {
        if (Box_Contains(detectWolfBox, &g_pWolf->pos)) {
            hitHive = hive;
            SetState(BEES_ST_CHASE_WOLF);
        }
    }
    switch (state) {
        case BEES_ST_IN_HIVE:
            potLeftFlyBox = 0;
            stungHome = 1;
            SetPosition(&motherHive->pos);
            break;
        case BEES_ST_ENTER_HIVE:
            if (AnimFlags(ANIM_F_FINISHED)) {
                hitHive->HandleMessage(this, MSG_HIVE_RESET, 0);
                motherHive->HandleMessage(this, MSG_HIVE_SWARM_HOME, 0);
                SetState(BEES_ST_IN_HIVE);
            }
            break;
        case BEES_ST_ORBIT_HIVE:
            if (hive == motherHive) {
                orbitAngle = 0;
                SetState(BEES_ST_ORBIT_MOTHER);
            } else {
                orbitAngle = Orbit(orbitOffset, hive->pos, orbitAngle, -0x19);
            }
            break;
        case BEES_ST_ORBIT_MOTHER:
            if (SDW_ABS(orbitAngle) > 0xfff) {
                orbitAngle = 0;
                SetState(BEES_ST_ENTER_HIVE);
            } else {
                orbitAngle = Orbit(orbitOffset, motherHive->pos, orbitAngle, -0x19);
            }
            break;
        case BEES_ST_RETURN_HOME:
            timer -= g_dtMs;
            if (timer <= 0) {
                dHome = MoveToward(motherHive->pos, 400, 0);
                if (dHome < 50) {
                    orbitAngle = 0;
                    SetState(BEES_ST_IN_HIVE);
                }
            }
            break;
        case BEES_ST_FLY_TO_HIVE:
            settleTimer -= g_dtMs;
            if (settleTimer <= 0)
                g_beesLeftFirstHive = 1;
            distHive = MoveToward(hive->pos, 400, 0);
            if (distHive < 40) {
                if (hive != motherHive)
                    hitHive->HandleMessage(this, MSG_HIVE_RESET, 0);
                SetState(BEES_ST_ORBIT_HIVE);
            }
            break;
        case BEES_ST_FLY_TO_TARGET:
            ds = MoveToward(targetPos, 400, 0);
            if (ds < 40)
                SetState(BEES_ST_ORBIT_HIVE);
            break;
        case BEES_ST_CHASE_SAM:
            if (!honeyPot->IsInWorld() || potLeftFlyBox) {
                vertOff = 0x8c;
                obj = sam;
                spot = obj->pos;
            }
            if (g_honeyPotSamInHoney && honeyPot->IsInWorld()) {
                vertOff = 0x8c;
                obj = victim;
                spot = obj->pos;
            } else {
                vertOff = 0x8c;
                obj = sam;
                spot = obj->pos;
            }
            targetDist = MoveToward(spot, 400, vertOff);
            if (targetDist < 40) {
                if (g_honeyPotSamInHoney && honeyPot->IsInWorld()) {
                    if (obj->GetClassId() == CLASSID_SAM) {
                        timer = chaseTime[alert];
                        SetState(BEES_ST_STING_VICTIM);
                    }
                } else {
                    timer = chaseTime[alert];
                    SetState(BEES_ST_ORBIT_SAM);
                }
            }
            break;
        case BEES_ST_ORBIT_SAM:
            timer -= g_dtMs;
            if (timer <= 0) {
                if (SDW_ABS(orbitAngle) > 0x2ffd) {
                    orbitAngle = 0;
                    SetState(BEES_ST_RETURN_HOME);
                } else {
                    orbitAngle = Orbit(orbitOffset, sam->pos, orbitAngle, -0x32);
                }
            }
            break;
        case BEES_ST_CHASE_WOLF:
            if (g_pWolf->HandleMessage(this, MSG_WOLF_IS_IN_WATER_STATE, 0))
                SetState(BEES_ST_FLY_TO_TARGET);
            wolfDist = MoveToward(g_pWolf->pos, 0x514, 0x8c);
            if (wolfDist < 20)
                SetState(BEES_ST_STING_WOLF);
            break;
        case BEES_ST_STING_WOLF:
            if (g_pWolf->HandleMessage(this, MSG_WOLF_IS_IN_WATER_STATE, 0))
                SetState(BEES_ST_FLY_TO_TARGET);
            wolfSpot = g_pWolf->pos;
            FaceTowards(wolfSpot);
            wolfSpot.y = -0x8c;
            SetPosition(&wolfSpot);
            break;
        case BEES_ST_STING_VICTIM:
            landOn = victim->pos;
            timer -= g_dtMs;
            if (timer <= 0) {
                if (alert == 3)
                    victim->HandleMessage(this, MSG_BEES_STING, 0);
                FaceTowards(landOn);
                landOn.y = -0x8c;
                SetPosition(&landOn);
            }
            break;
        case BEES_ST_REACT:
            timer -= g_dtMs;
            if (timer <= 0)
                SetState(BEES_ST_CHASE_SAM);
            break;
        case BEES_ST_HOVER_HIVE:
            dir.x = hive->pos.x - pos.x;
            dir.y = hive->pos.y - pos.y;
            dir.z = hive->pos.z - pos.z;
            hiveAngle = Math_RadiansToAngle4096((float)atan2((double)dir.x, (double)dir.z)) & 0xfff;
            if (hiveAngle >= camAngle - 100 && hiveAngle <= camAngle + 100) {
                SetPartHeight(0x180);
                SetState(BEES_ST_QUESTION_IN);
            } else {
                orbitAngle = Orbit(orbitOffset, hive->pos, orbitAngle, -0x64);
            }
            break;
        case BEES_ST_QUESTION_IN:
            SetPartHeight(0x180);
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(BEES_ST_QUESTION_LOOP);
            break;
        case BEES_ST_QUESTION_LOOP:
            SetPartHeight(0x180);
            timer -= g_dtMs;
            if (timer <= 0)
                SetState(BEES_ST_QUESTION_OUT);
            break;
        case BEES_ST_QUESTION_OUT:
            SetPartHeight(0x180);
            if (AnimFlags(ANIM_F_FINISHED)) {
                hitHive->HandleMessage(this, MSG_HIVE_TRACK_SWARM, 0);
                SetState(BEES_ST_FLY_TO_HIVE);
                hive = secondHive;
            }
            break;
        case BEES_ST_EXCLAIM_IN:
            SetPartHeight(0x180);
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(BEES_ST_EXCLAIM_LOOP);
            break;
        case BEES_ST_EXCLAIM_LOOP:
            SetPartHeight(0x180);
            timer -= g_dtMs;
            if (timer <= 0)
                SetState(BEES_ST_EXCLAIM_OUT);
            break;
        case BEES_ST_EXCLAIM_OUT:
            SetPartHeight(0x180);
            if (AnimFlags(ANIM_F_FINISHED)) {
                hitHive->HandleMessage(this, MSG_HIVE_TRACK_SWARM, 0);
                SetState(BEES_ST_FLY_TO_HIVE);
            }
            break;
    }
    AdvanceAnim();
}

/* 0x49806a - vtable +0x10. 0x67 hands over the victim, 0x3980 sets the alert level, 0x3981 is a hive reporting a
 * cannonball, 0x3982 is another swarm's broadcast telling everyone to go home. */
s32 Bees::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    switch (msgId) {
        case MSG_HONEY_VICTIM:
            /* cast kept: this message's void * arg is the victim */
            if (state != BEES_ST_STING_VICTIM)
                victim = (ScnObject *)arg;
            break;
        case MSG_BEES_RELEASE:
            alert = (u8)(u32)arg; /* cast kept: this message's void * arg carries a number */
            SetState(BEES_ST_REACT);
            break;
        case MSG_BEES_HIVE_HIT:
            if (state != BEES_ST_FLY_TO_TARGET) {
                hitHive = sender;
                if (firstHive == sender && g_beesLeftFirstHive == 0) {
                    targetPos = secondHive->pos;
                    SetState(BEES_ST_HOVER_HIVE);
                } else if (g_beesLeftFirstHive != 0) {
                    Scenaric_SendToClass(CLASSID_BEES, MSG_BEES_GO_MOTHER, 0);
                }
            } else {
                hitHive->HandleMessage(this, MSG_HIVE_RESET, 0);
            }
            break;
        case MSG_BEES_GO_MOTHER:
            if (state != BEES_ST_FLY_TO_TARGET) {
                hive = motherHive;
                targetPos = motherHive->pos;
                SetState(BEES_ST_EXCLAIM_IN);
            } else {
                hitHive->HandleMessage(this, MSG_HIVE_RESET, 0);
            }
            break;
    }
    return 0;
}

/* 0x4981ed - the state setter: the animation, the buzz and, for the circling states, the radius vector. */
void Bees::SetState(u8 newState)
{
    Vec3s d;
    Vec3s cam;

    state = newState;
    switch (newState) {
        case BEES_ST_IN_HIVE:
            if (IsSoundPlaying(soundHandle))
                StopSound(soundHandle);
            motherHive->HandleMessage(this, MSG_HIVE_SWARM_HOME, 0);
            PlayAnim(AABEIL01_ANIM_STAND2, 1, 1);
            break;
        case BEES_ST_ENTER_HIVE:
            if (IsSoundPlaying(soundHandle))
                StopSound(soundHandle);
            PlayAnim(AABEIL01_ANIM_STAND2, 0, 1);
            break;
        case BEES_ST_ORBIT_HIVE:
            if (!IsSoundPlaying(soundHandle))
                soundHandle = Sound_Play(SND_BEES_BUZZ, this, 0x3f, SNDF_LOOP | SNDF_POSITIONAL, 0x1000);
            PlayAnim(AABEIL01_ANIM_STAND, 1, 1);
            orbitAnchor.x = pos.x;
            orbitAnchor.y = pos.y;
            orbitAnchor.z = pos.z;
            orbitAnchor.x = orbitAnchor.x - 0x50;
            orbitOffset.x = pos.x - orbitAnchor.x;
            orbitOffset.y = pos.y - orbitAnchor.y;
            orbitOffset.z = pos.z - orbitAnchor.z;
            break;
        case BEES_ST_CHASE_SAM:
            if (IsSoundPlaying(soundHandle))
                StopSound(soundHandle);
            if (!Sound_IsSampleIdPlaying(SND_BEES_BUZZ))
                soundHandle = Sound_Play(SND_BEES_BUZZ, this, 0xff, SNDF_LOOP | SNDF_POSITIONAL, 0x1000);
            PlayAnim(AABEIL01_ANIM_STAND, 1, 1);
            break;
        case BEES_ST_STING_VICTIM:
            if (IsSoundPlaying(soundHandle))
                StopSound(soundHandle);
            soundHandle = Sound_Play(SND_BEES_BUZZ, this, 0xff, SNDF_LOOP | SNDF_POSITIONAL, 0x1000);
            break;
        case BEES_ST_REACT:
            if (IsSoundPlaying(soundHandle))
                StopSound(soundHandle);
            if (alert == 0)
                PlayAnim(AABEIL01_ANIM_OUT, 0, 1);
            else
                PlayAnim(AABEIL01_ANIM_STAND2, 0, 1);
            timer = animDuration;
            break;
        case BEES_ST_ORBIT_SAM:
            orbitAngle = 0;
            PlayAnim(AABEIL01_ANIM_STAND, 1, 1);
            orbitAnchor.x = pos.x;
            orbitAnchor.y = pos.y;
            orbitAnchor.z = pos.z;
            orbitAnchor.x = orbitAnchor.x - 0x64;
            orbitOffset.x = pos.x - orbitAnchor.x;
            orbitOffset.y = pos.y - orbitAnchor.y;
            orbitOffset.z = pos.z - orbitAnchor.z;
            break;
        case BEES_ST_ORBIT_MOTHER:
            PlayAnim(AABEIL01_ANIM_STAND, 1, 1);
            orbitAnchor.x = pos.x;
            orbitAnchor.y = pos.y;
            orbitAnchor.z = pos.z;
            orbitAnchor.x = orbitAnchor.x - 0x50;
            orbitOffset.x = pos.x - orbitAnchor.x;
            orbitOffset.y = pos.y - orbitAnchor.y;
            orbitOffset.z = pos.z - orbitAnchor.z;
            break;
        case BEES_ST_STING_WOLF:
            PlayAnim(AABEIL01_ANIM_STAND, 1, 1);
            wolfDist = 1000;
            g_pWolf->HandleMessage(this, MSG_BEES_STING, 0);
            break;
        case BEES_ST_FLY_TO_TARGET:
        case BEES_ST_FLY_TO_HIVE:
        case BEES_ST_CHASE_WOLF:
            targetPos = hive->pos;
            PlayAnim(AABEIL01_ANIM_STAND, 1, 1);
            break;
        case BEES_ST_HOVER_HIVE:
            PlayAnim(AABEIL01_ANIM_STAND, 1, 1);
            cam.x = g_camPos.x;
            cam.y = g_camPos.y;
            cam.z = g_camPos.z;
            d.x = hive->pos.x - cam.x;
            d.y = hive->pos.y - cam.y;
            d.z = hive->pos.z - cam.z;
            camAngle = Math_RadiansToAngle4096((float)atan2((double)d.x, (double)d.z)) & 0xfff;
            orbitAnchor.x = pos.x;
            orbitAnchor.y = pos.y;
            orbitAnchor.z = pos.z;
            orbitAnchor.x = orbitAnchor.x - 0x50;
            orbitOffset.x = pos.x - orbitAnchor.x;
            orbitOffset.y = pos.y - orbitAnchor.y;
            orbitOffset.z = pos.z - orbitAnchor.z;
            break;
        case BEES_ST_QUESTION_IN:
            PlayAnim(AABEIL01_ANIM_INTERRO, 0, 1);
            break;
        case BEES_ST_QUESTION_LOOP:
            timer = Anim_GetDurationMs(Inst(), AABEIL01_ANIM_INTERRO2, 1) * 3;
            PlayAnim(AABEIL01_ANIM_INTERRO2, 1, 1);
            break;
        case BEES_ST_QUESTION_OUT:
            PlayAnim(AABEIL01_ANIM_INTERRO3, 0, 1);
            break;
        case BEES_ST_EXCLAIM_IN:
            PlayAnim(AABEIL01_ANIM_EXCLAM, 0, 1);
            break;
        case BEES_ST_EXCLAIM_LOOP:
            timer = Anim_GetDurationMs(Inst(), AABEIL01_ANIM_INTERRO2, 1) * 3;
            PlayAnim(AABEIL01_ANIM_EXCLAM2, 1, 1);
            break;
        case BEES_ST_EXCLAIM_OUT:
            PlayAnim(AABEIL01_ANIM_EXCLAM3, 0, 1);
            break;
    }
}

/* 0x498d45 - one step around `centre`: rotate `offset` by `angle` about the vertical, keep the swarm's own height,
 * face the new point and go there. Returns the angle advanced by `step`. */
s16 Bees::Orbit(Vec3s offset, Vec3s centre, s16 angle, s16 step)
{
    Mat34s m;
    Vec4i v;
    Vec3s newPos;
    Vec3s turn;

    turn.x = 0;
    turn.y = 0;
    turn.z = 0;
    turn.y = angle;
    angle += step;
    v.x = offset.x;
    v.y = offset.y;
    v.z = offset.z;
    Mat34s_FromEulerScaled(&turn, &m, 0);
    /* cast kept: v is the Vec3i input and the Vec4i output of the in-place transform */
    Mat34s_TransformVec3i(&m, (const Vec3i *)&v, &v);
    newPos.x = (s16)(centre.x + (s16)v.x);
    newPos.y = pos.y;
    newPos.z = (s16)(centre.z + (s16)v.z);
    FaceTowards(newPos);
    SetPosition(&newPos);
    return angle;
}

/* 0x498e0b - fly `speed` units/s toward `target` raised by `hover`, facing it. Returns the x/z distance. */
s32 Bees::MoveToward(Vec3s target, s32 speed, s32 hover)
{
    Vec3s speedVec;
    Vec3s toTarget;
    Vec3s move;
    s32 len;

    len = Vec3s_DistXZ(&target, &pos);
    FaceTowards(target);
    if (len == 0) {
        speedVec.x = 0;
        speedVec.y = 0;
        speedVec.z = 0;
    } else {
        toTarget.x = (s16)(target.x - pos.x);
        toTarget.y = (s16)(target.y - hover - pos.y);
        toTarget.z = (s16)(target.z - pos.z);
        speedVec.x = (s16)(toTarget.x * speed / len);
        speedVec.y = (s16)(toTarget.y * speed / len);
        speedVec.z = (s16)(toTarget.z * speed / len);
        Vec3s_ScaleByDt(&speedVec, &move);
        Translate(&move);
    }
    return len;
}

/* 0x498eec - turn to face a point. */
void Bees::FaceTowards(Vec3s target)
{
    SetFacing(HeadingTo(&target));
}

/* 0x498f16 - vtable +0x08: a camera-facing sprite with the fixed extra rotation in renderRot. */
void Bees::Render(Camera *view)
{
    RenderFacingCamera(view, 0, 0, &renderRot);
}

/* 0x498f3c - vtable +0x14: back onto the circled hive, circling again. */
void Bees::Reset()
{
    soundHandle = 0;
    StopSound(soundHandle);
    orbitAngle = 0;
    SetPosition(&hive->pos);
    SetState(BEES_ST_ORBIT_HIVE);
    if (state == BEES_ST_IN_HIVE)
        SetState(BEES_ST_IN_HIVE);
}

/* 0x498fbb - the class factory for CLASSID 107 "Bees". */
ScnObject *Bees_Create(void *record)
{
    ScnObject *obj = new Bees;
    obj = ((ScnBody *)obj)->Init((u16 *)record, 0); /* cast kept: a downcast, and the level record as raw words */
    return obj;
}
