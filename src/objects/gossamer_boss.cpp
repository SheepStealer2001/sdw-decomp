/*
 * Gossamer_Boss (class 113 "Gossamer_Boss", vtable 0x574ec4, sizeof 0x2f0) - the orange monster of the Level 10 boss
 * fight. SheepD3D.exe 0x44ebe0-0x45220a: the whole file (it starts after the int3 padding at 0x44ebdf and ends with
 * UpdateFacingDelta, followed by int3 padding before Gossamer_Lev08's factory at 0x452210).
 * The Gossamer_Boss fields are in data/structs/Gossamer_Boss.csv.
 *
 * The fight, as the code has it: the boss walks its designer path (PROPERTY trajectory, +0x1a0) and chases Ralph
 * inside the arena box. Close in it freezes him (msg 0xE) and smashes (state 0xB); far out it gives up (state 0x12).
 * A hit arrives as message 0x43 with a s16 offset: the arena box and the path points are pushed along, hitIndex
 * counts the phase up, the boss falls over (states 1/0x13 -> the hit camera) and the fight moves on. Three phases
 * (hitIndex 0..2); at hitIndex 2 leaving the last phase box ends it (state 6 -> 7).
 * State 0x15 is the pre-fight state: the boss stays hidden until Ralph enters the arena box, then it is placed on the
 * first path point, class-0x18 objects are told (msg 0x1000) and the fight starts at state 0x10.
 *
 * Devices that only pin the original code generation (the inline helpers have no bodies in the original, so their
 * names are not recovered, and a shape that reproduces the bytes is a representation, not proof that the original read
 * this way):
 *  - PlayAnim / AnimFlags / StopSound / EnableBoxCollide as in src/objects/crane.cpp and icecube.cpp.
 *  - GetFacing / SetFacing: every read of rot.y that the original puts through a s16 temp (0x44f3ed) and every
 *    write that goes through a u16 temp (0x44f41d); the plain writes (rot.y = 0x800) are written plainly.
 *  - StartCam: the class's scripted camera. Its three parameters are what give the original its per-argument stack
 *    slots - two at 0x451658 where the middle argument is a local, three at 0x4516ac where all three are members.
 *  - Scn_GetPropU32 (a u32 offset, so the constant gets a stack temp of its own), Model_FirstBox (its local and
 *    return temp at 0x44eea3 / 0x44eeb2), SetVisible and Shadow_SetVisible (each called with a compile-time
 *    constant, so the dead half is still emitted), the inline emitter's constructor (0x44ec1b), and Sin / Cos.
 * Locals: under /Od a local's slot follows from a hash of its NAME, so the functions with several live locals group
 * them in a `w` struct (as src/engine/collide.cpp does) - one local, one base address, and the field offsets do the
 * rest. Where a local has to stay a local (Update's camera angles, StartHitCamera's three angles) the names were chosen
 * with tools/vc6_locals.py and the buckets are noted there.
 */
/* BYTES: dead-code, inline, slot-group, slot-name. */
/* BYTES(inline): StartCam (inline, three parameters): source-only inline: its three parameters give the original's per-argument stack slots (0x451658, 0x4516ac) */

#define SDW_MEMBERS_ScnObject            \
    static void *operator new(u32 size); \
    void SetFacing(u16 heading);


#define SDW_MEMBERS_InlineEmitter3                                              \
    /* inline: the pools are the inline buffers, 3 slots (0x44ec1b-0x44ec47) */ \
    InlineEmitter3()                                                            \
    {                                                                           \
        base.slotPool = slotBuf;                                                \
        base.particles = particleBuf;                                           \
        base.count = 3;                                                         \
        base.Emitter_Reset();                                                   \
    }
#define SDW_MEMBERS_Gossamer_Boss                                                                 \
    void StartCam(u16 rotX, u16 rotY, u16 rotZ)                                                   \
    {                                                                                             \
        Camera_StartScripted(this, &g_camera, rotX, rotY, rotZ, &camSetup.eye, 0x280, 0, 0x1000); \
    }
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Camera;
struct Vec3s;
class ScnObject;
class Instance;
struct Animator;
u32 Anim_Start(Instance *inst, Animator *animator, u16 id, u32 opts); /* 0x550196 (used by the PlayAnim inline) */
/* used by the StartCam inline below, so declared before the class */
void Camera_StartScripted(ScnObject *owner, Camera *cam, u16 rotX, u16 rotY, u16 rotZ, Vec3s *pos, u16 focal, u32 mode,
                          s32 time); /* 0x55a70d */
#include "../app/app_main.h"
#include "../engine/scenaric.h"
#include "../engine/scn_tools.h"
#include "../engine/fixed_math.h"
#include "../engine/sound_mgr.h"
#include "camera.h"
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETFACING 1
#define SDW_INLINE_SCNOBJECT_SETFACING_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETFACING
#undef SDW_INLINE_SCNOBJECT_SETFACING_U16
#define SDW_INLINE_SCNOBJECT_ENABLEBOXCOLLIDE_S32 1
#define SDW_INLINE_SCNOBJECT_SETVISIBLE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_ENABLEBOXCOLLIDE_S32
#undef SDW_INLINE_SCNOBJECT_SETVISIBLE_S32
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#include "../engine/property_math.h"

#define SDW_ABS(v) ((v) >= 0 ? (v) : -(v))

extern Wolf *g_pWolf; /* 0x6cf310 */

#define g_camPos (g_camera.pos) /* 0x584d20 */

extern s32 g_dtMs;
#define g_dtMs (*(u32 *)&g_dtMs)     /* 0x71b2e8; cast kept: read here as unsigned, as the original does */
extern u32 g_gameFlags;              /* 0x6ddf74 */
extern "C" s16 g_sinTable4096[5122]; /* 0x57ece0 */
extern "C" const s16 *g_pCosTable;   /* 0x5814e4 */

#define Sin(a) g_sinTable4096[a]
#define Cos(a) g_pCosTable[a]

s32 Scenaric_FindByClass(u16 classId, ScnObject **out, s32 max); /* 0x5145c5 */
#include "../sdk/crt.h"
extern "C" s16 Math_RadiansToAngle4096(float radians);                    /* 0x5269ce */
u16 Sound_Play(u16 soundId, void *owner, u16 volume, u8 flags, s32 rate); /* 0x5491b8 */

/* the 16-byte CAMERA designer property, as the class keeps a copy of it at +0x280 */

/* ---- inline helpers ---- */

/* A u32 designer property: the record's property block starts at record+0x14. */
#define SDW_INLINE_FREE_SCN_GETPROPU32_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_SCN_GETPROPU32_VOID_U32

#define SDW_INLINE_SCNOBJECT_STOPSOUND_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_STOPSOUND_U16

/* The first box of the model's own box list, or NULL. */
#define SDW_INLINE_FREE_MODEL_FIRSTBOX_MODEL 1
#include "../engine/model_inlines.h"
#undef SDW_INLINE_FREE_MODEL_FIRSTBOX_MODEL

/* Shadow flag bit 0 = blob not drawn. */
inline void Shadow_SetVisible(Shadow *shadow, s32 on)
{
    if (on)
        shadow->flags &= (u8)~SHADOW_F_HIDDEN;
    else
        shadow->flags |= SHADOW_F_HIDDEN;
}

/* 0x44ebe0 - the class factory for CLASSID 113 "Gossamer_Boss": new Gossamer_Boss (the base constructors inlined -
 * the ScnObject, ScnBody, ScnMobile and Gossamer_Boss vtables, the two embedded ScnBody members and the particle
 * emitter with its two inline pools), then ScnBody_Init(record, 0) through the vtable. */
ScnObject *Gossamer_Boss_Create(void *record)
{
    ScnObject *obj = new Gossamer_Boss;
    /* cast kept: obj was just made as a Gossamer_Boss; the record is raw bytes */
    obj = ((ScnBody *)obj)->Init((u16 *)record, 0);
    return obj;
}

/* 0x44ecbf - vtable +0x00: reads the designer properties (three phase boxes, the arena box, the walk path, the
 * camera and the three phase durations), builds the shifted smash boxes, takes a copy of the arena and model boxes
 * so Reset can put them back, builds the two extra bodies from exports 0x95 and 0xd8, sets up the dust column and
 * its fixed quarter-turn frame, and waits hidden in state 0x15. */
void Gossamer_Boss::PostLoadInit()
{
    void *props;
    s32 i;

    props = record;
    phaseBoxes[0] = Scn_GetPropBox(props, 4);
    phaseBoxes[1] = Scn_GetPropBox(props, 8);
    phaseBoxes[2] = Scn_GetPropBox(props, 0xc);
    for (i = 0; i < 3; i++) {
        smashBoxes[i].min[0] = phaseBoxes[i]->min[0] - (i + 5000);
        smashBoxes[i].min[1] = phaseBoxes[i]->min[1];
        smashBoxes[i].min[2] = phaseBoxes[i]->min[2];
        smashBoxes[i].max[0] = phaseBoxes[i]->max[0] - (i + 5000);
        smashBoxes[i].max[1] = phaseBoxes[i]->max[1];
        smashBoxes[i].max[2] = phaseBoxes[i]->max[2];
    }
    phaseSecs[0] = (u8)Scn_GetPropU32(props, 0x18);
    phaseSecs[1] = (u8)Scn_GetPropU32(props, 0x1c);
    phaseSecs[2] = (u8)Scn_GetPropU32(props, 0x20);
    arenaBox = Scn_GetPropBox(props, 0x10);
    path = Scn_GetPropTrajectory(props, 0x14);
    modelBox = Model_FirstBox(inst_model);
    modelBoxSave.min.x = modelBox->min.x;
    modelBoxSave.min.y = modelBox->min.y;
    modelBoxSave.min.z = modelBox->min.z;
    modelBoxSave.max.x = modelBox->max.x;
    modelBoxSave.max.y = modelBox->max.y;
    modelBoxSave.max.z = modelBox->max.z;
    camProp = Scn_GetPropCamera(props, 0);
    onde = 0;
    Scenaric_FindByClass(CLASSID_GOSSAMERONDE, &onde, 1);
    fxParams.riseSpeed = 0;
    fxParams.life = 1000;
    fxParams.period = 1000;
    fxParams.sizeStart = 0x40;
    fxParams.sizeEnd = 0x40;
    fxParams.sheetIndex = 7;
    fx.base.Emitter_Reset();
    /* cast kept: the record builder and ScnBody::Init take the synthesised record as raw u16 words */
    if (Scn_BuildRecordFromExport(WAR_IDO_AGOOMB01, (u16 *)&bodyRecord, 0, 0))
        hitBody.Init((u16 *)&bodyRecord, 0);
    if (Scn_BuildRecordFromExport(WAR_IDO_ATIMER01, (u16 *)&bodyRecord, 0, 0))
        /* cast kept: a built record passed where a level record is expected */
        spinBody.Init((u16 *)&bodyRecord, 0);
    hitIndex = -1;
    timerMs = 0;
    showHitBody = 0;
    chasingFlat = 0;
    hasChaseTarget = 0;
    showSpinBody = 0;
    pathShifted = 0;
    pathIndex = 0;
    arenaMaxSave.x = arenaBox->max[0];
    arenaMaxSave.y = arenaBox->max[1];
    arenaMaxSave.z = arenaBox->max[2];
    arenaMinSave.x = arenaBox->min[0];
    arenaMinSave.y = arenaBox->min[1];
    arenaMinSave.z = arenaBox->min[2];
    fxFrame.rot[0] = Cos(0xc00);
    fxFrame.rot[5] = 0;
    fxFrame.rot[7] = 0;
    fxFrame.rot[1] = 0;
    fxFrame.rot[3] = 0;
    fxFrame.rot[6] = -Sin(0xc00);
    fxFrame.rot[4] = 1;
    fxFrame.rot[2] = Sin(0xc00);
    fxFrame.rot[8] = Cos(0xc00);
    spinBody.PlayAnim(ATIMER01_ANIM_ON, 1, 0);
    SnapToGround(0);
    EnableBoxCollide(1);
    SetVisible(0);
    soundHandle = 0;
    SetState(GOSSBOSS_ST_EXIT);
}

/* 0x44f2b8 - vtable +0x08: the boss itself, then the knock-down body, then the spin body (moved to the dust-column
 * origin) and the dust column. */
/* BYTES(dead-code): scale is set and never read, as in the original */
void Gossamer_Boss::Render(Camera *view)
{
    Vec3s scale;

    scale.x = scale.y = scale.z = 0x800;
    ScnMobile::Render(view);
    if (showHitBody)
        hitBody.Render(view);
    if (showSpinBody) {
        spinBody.pos = fxOrigin;
        spinBody.Render(view);
        if (fx.base.flags.active)
            fx.base.Emitter_Render(view, 0);
    }
}

/* 0x44f378 - vtable +0x04: the XZ distance to Ralph, the turn bookkeeping, then one body per state. */
/* BYTES(slot-group): locals grouped in w / ws2 only to pin the original frame offsets; pad0, pad1, pad0, pad1, pad2 fill gaps */
/* BYTES(dead-code): tilt is computed and never read, as in the original */
/* BYTES(slot-name): w / ws2 structs pin the slots; eyeYaw and tilt stay plain locals named for their buckets (7, 10, 13, 15) */
void Gossamer_Boss::Update()
{
    /* the locals are grouped into structs so that their slots follow from field offsets instead of from a hash of
     * their names; the two camera angles stay plain locals because StartCam's expansion substitutes a simple local
     * and gives a stack slot to anything else. Buckets: w(7) eyeYaw(10) tilt(13) ws2(15) - tools/vc6_locals.py. */
    struct {
        ScnObject *found[10]; /* -0x48 */
        u32 k;                /* -0x20 */
        Vec3s spot;           /* -0x1c */
        u16 pad0;
        u32 n;      /* -0x14 */
        s32 off[3]; /* -0x10 */
        u32 pad1;
    } w;
    u16 eyeYaw; /* -0x4a */
    u16 tilt;   /* -0x4c */
    struct {
        Vec3s camDelta; /* -0x60 */
        u16 pad0;
        u16 pad1;
        s16 camDist; /* -0x56 */
        Vec3s rotUp; /* -0x54 */
        u16 pad2;
    } ws2;

    wolfDist = Vec3s_DistXZ(&pos, &g_pWolf->pos);
    UpdateFacingDelta();
    switch (state) {
        case GOSSBOSS_ST_TAUNT:
            turnRefAngle = GetFacing() - 0x800;
            SetFacing(HeadingTo(&g_pWolf->pos));
            TurnStep(8);
            if (timerMs > timerLimitMs || wolfDist > 0x320 ||
                g_pWolf->HandleMessage(this, MSG_WOLF_IS_RUNNING, 0) == 0) {
                SetState(GOSSBOSS_ST_RETURN_HOME);
                timerMs = 0;
                break;
            }
            timerMs += g_dtMs;
            break;
        case GOSSBOSS_ST_EXIT:
            if (IsInBox(&pos, 0)) {
                g_gameFlags |= GF_CINE_SURVIVES_RESTART;
                w.n = Scenaric_FindByClass(CLASSID_SENSIBLEBUTTON, w.found, 10);
                for (w.k = 0; w.k < w.n; w.k++)
                    w.found[w.k]->HandleMessage(this, MSG_BUTTON_REVEAL, 0);
                SetVisible(1);
                w.spot.x = path->pts[0].x;
                w.spot.z = path->pts[0].z;
                w.spot.y = pos.y;
                SetPosition(&w.spot);
                spawnPos = pos;
                homePos = pos;
                SetState(GOSSBOSS_ST_GUARD);
            }
            break;
        case GOSSBOSS_ST_STOP:
            if (AnimFlags(ANIM_F_FINISHED)) {
                timerMs = 0;
                timerLimitMs = 2000;
                SetState(GOSSBOSS_ST_RETURN_HOME);
            }
            break;
        case GOSSBOSS_ST_FALL:
            tilt = (u16)((s16)(Math_RadiansToAngle4096(atan2((double)camSetup.eye.y - (double)pos.y,
                                                             (double)camSetup.eye.x - (double)pos.x)) &
                               0xfff) +
                         0x800);
            eyeYaw = (u16)(0x800 - (s16)(Math_RadiansToAngle4096(atan2((double)camSetup.eye.x - (double)pos.x,
                                                                       (double)camSetup.eye.z - (double)pos.z)) &
                                         0xfff));
            StartCam(camSetup.rot[0], eyeYaw, camSetup.rot[2]);
            FallStep();
            break;
        case GOSSBOSS_ST_BRIDGE:
            if (AnimFlags(ANIM_F_FINISHED)) {
                if (wolfHeld)
                    g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0);
                Camera_ReleaseAny();
                SetState(GOSSBOSS_ST_DEFEATED);
            }
            break;
        case GOSSBOSS_ST_RUN:
            ChaseStep();
            break;
        case GOSSBOSS_ST_BRIDGE_HIT:
            if (AnimFlags(ANIM_F_FINISHED)) {
                hitBody.PlayAnim(AGOOMB01_ANIM_LIGHT, 0, 0);
                SetState(GOSSBOSS_ST_LIGHT);
            }
            break;
        case GOSSBOSS_ST_LIGHT:
            if (AnimFlags(ANIM_F_FINISHED)) {
                ws2.rotUp.x = ws2.rotUp.z = 0;
                ws2.rotUp.y = 0x800;
                rot = ws2.rotUp;
                showHitBody = 0;
                SetState(GOSSBOSS_ST_WALK_HOME);
            }
            break;
        case GOSSBOSS_ST_WALK_HOME:
            if (hitIndex == 2) {
                phaseOverlapX = pos.x - phaseBoxes[hitIndex]->max[0];
                SetState(GOSSBOSS_ST_FALL);
                fallStep.x = 0;
                fallStep.y = 0;
                fallStep.z = 100;
                rot.y = 0x800;
                break;
            }
            WalkHome();
            break;
        case GOSSBOSS_ST_WAIT:
            if (timerMs > timerLimitMs) {
                SetState(GOSSBOSS_ST_RUN);
                timerMs = 0;
                break;
            }
            timerMs += g_dtMs;
            break;
        case GOSSBOSS_ST_SPIN:
            if (g_pWolf->HandleMessage(this, MSG_WOLF_IS_RUNNING, 0) == 0) {
                if (Vec3s_DistXZ(&pos, &homePos) < 0x32) {
                    SetState(GOSSBOSS_ST_GUARD);
                    break;
                }
                SetState(GOSSBOSS_ST_GUARD);
            }
            TurnStep(9);
            SetFacing(spinFacing);
            break;
        case GOSSBOSS_ST_SPIN_ATTACK:
            if (AnimFlags(ANIM_F_FINISHED)) {
                if (showSpinBody == 0)
                    soundHandle = Sound_Play(SND_CLOCK_TICK, this, 0xff, SNDF_LOOP | SNDF_POSITIONAL, 0x1000);
                ws2.camDelta.x = g_camPos.x - fxOrigin.x;
                ws2.camDelta.z = g_camPos.z - fxOrigin.z;
                ws2.camDelta.y = 0;
                ws2.camDist = Vec3s_DistXZ(&g_camPos, &fxOrigin);
                if (ws2.camDist == 0)
                    ws2.camDist = 1;
                ws2.camDelta.x = (s16)(ws2.camDelta.x * -100 / ws2.camDist);
                ws2.camDelta.z = (s16)(ws2.camDelta.z * -100 / ws2.camDist);
                spinBody.SetFacing((s16)(Math_RadiansToAngle4096(atan2((double)g_camPos.x - (double)fxOrigin.x,
                                                                       (double)g_camPos.z - (double)fxOrigin.z)) &
                                         0xfff) +
                                   0x800);
                fxAnchor.x = fxOrigin.x;
                fxAnchor.y = fxOrigin.y;
                fxAnchor.z = fxOrigin.z;
                w.off[0] = ws2.camDelta.x;
                w.off[2] = ws2.camDelta.z;
                w.off[1] = 0;
                /* cast kept: w.off is the transform's in-place buffer: x, y, z are read and a Vec4i is written back */
                Mat34s_TransformTransposedVec3i(&fxFrame, (const Vec3i *)w.off, (Vec4i *)w.off);
                fxAnchor.x += (s16)w.off[0];
                fxAnchor.z += (s16)w.off[2];
                showSpinBody = 1;
                if (timerMs > timerLimitMs) {
                    StopSound(soundHandle);
                    soundHandle = Sound_Play(SND_GOSSBOSS_SPIN, this, 0xff, SNDF_NO_RETRIGGER, 0x1000);
                    showSpinBody = 0;
                    if (fx.base.flags.active)
                        fx.base.Emitter_UpdateColumn(&fxParams, &fxAnchor, 0, 0);
                    timerMs = 0;
                    timerLimitMs = 1000;
                    SetState(GOSSBOSS_ST_SPIN_END);
                    break;
                }
                fx.base.Emitter_UpdateColumn(&fxParams, &fxAnchor, spinDurationMs - timerMs, 1);
                timerMs += g_dtMs;
            }
            break;
        case GOSSBOSS_ST_SPIN_END:
            if (AnimFlags(ANIM_F_FINISHED)) {
                showSpinBody = 0;
                timerMs = 0;
                timerLimitMs = 1000;
                SetState(GOSSBOSS_ST_RUN);
            }
            break;
        case GOSSBOSS_ST_SMASH:
            if (smashStarted == 0 && g_pWolf->HandleMessage(this, MSG_WOLF_IS_GROUNDED, 0) != 0) {
                /* cast kept: HandleMessage's arg is a void *; this message passes a number in it */
                g_pWolf->HandleMessage(this, MSG_WOLF_SQUASH, (void *)0x8000);
                PlayAnim(AGOSSA02_ANIM_PUNCH, 0, 0);
                smashStarted = 1;
            }
            if (smashStarted != 0 && AnimFlags(ANIM_F_FINISHED)) {
                g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0);
                chasingFlat = 1;
                timerMs = 0;
                timerLimitMs = 0x9c4;
                SetState(GOSSBOSS_ST_PAUSE);
                PlayAnim(AGOSSA02_ANIM_STAND3, 0, 0);
            }
            break;
        case GOSSBOSS_ST_PAUSE:
            if (timerMs > timerLimitMs) {
                SetState(GOSSBOSS_ST_RUN);
                break;
            }
            timerMs += g_dtMs;
            break;
        case GOSSBOSS_ST_KICK:
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(GOSSBOSS_ST_KICK2);
            break;
        case GOSSBOSS_ST_KICK2:
            if (AnimFlags(ANIM_F_FINISHED)) {
                if (caughtWolf)
                    SetState(GOSSBOSS_ST_RETURN_HOME);
                else
                    SetState(GOSSBOSS_ST_GUARD);
            }
            break;
        case GOSSBOSS_ST_SHAKE:
            if (shakeTicks == 0x14 && onde != 0 && onde->HandleMessage(this, MSG_ONDE_START, 0) != 0)
                Camera_StartShake(0x14, timerLimitMs << 1);
            if (AnimFlags(ANIM_F_FINISHED)) {
                timerLimitMs = 500;
                timerMs = 0;
                if (Vec3s_DistXZ(&pos, &homePos) < 0x32) {
                    SetState(GOSSBOSS_ST_GUARD);
                    break;
                }
                SetState(GOSSBOSS_ST_RETURN_HOME);
                caughtWolf = 0;
                break;
            }
            shakeTicks++;
            break;
        case GOSSBOSS_ST_GUARD:
            SetFacing(HeadingTo(&g_pWolf->pos));
            if (hasChaseTarget)
                SetState(GOSSBOSS_ST_CHASE_TARGET);
            if (g_pWolf->HandleMessage(this, MSG_WOLF_IS_RUNNING, 0) != 0 && facingDelta > 0x400 && wolfDist < 0x4b0) {
                SetState(GOSSBOSS_ST_SHAKE);
                break;
            }
            if (IsInBox(&g_pWolf->pos, 0)) {
                if (wolfDist < 0x1c2) {
                    SetState(GOSSBOSS_ST_RUN);
                    break;
                }
                if (wolfDist < 0x3e8 && facingDelta > 0x400 &&
                    (g_pWolf->HandleMessage(this, MSG_QUERY_MOVED, 0) != 0 || wolfDist < 0x2bc))
                    SetState(GOSSBOSS_ST_SHAKE);
            }
            break;
        case GOSSBOSS_ST_CHASE_TARGET:
            if (chaseTarget != 0)
                MoveTo(&chaseTarget->pos);
            else
                SetState(GOSSBOSS_ST_RETURN_HOME);
            break;
        case GOSSBOSS_ST_RETURN_HOME:
            MoveTo(&homePos);
            break;
    }
    AdvanceAnim();
    spinBody.AdvanceAnim();
    hitBody.AdvanceAnim();
}

/* 0x450473 - vtable +0x10. 0x42 hands over a chase target (the sender's +0x1c0 / +0x1c4 pair); 0x43 is a hit: the
 * arena box and, once only, the two far path points are pushed along by the s16 argument, the phase counter goes up,
 * the boss is knocked down and the hit camera starts. */
s32 Gossamer_Boss::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    if (sender) {
        switch (msgId) {
            case MSG_TELEPORTED:
                /* cast kept: the message arg is a void *; this message carries the s16 shift in it */
                arenaBox->min[0] += (s16)(s32)arg;
                arenaBox->max[0] += (s16)(s32)arg;
                StopSound(soundHandle);
                hitIndex++;
                if (pathShifted == 0) {
                    /* cast kept: the same shift, from the message's void * arg */
                    path->pts[1].x += (s16)(s32)arg;
                    path->pts[2].x += (s16)((s32)arg * 2);
                    pathShifted = 1;
                }
                pathIndex++;
                homePos.x = path->pts[pathIndex].x;
                homePos.z = path->pts[pathIndex].z;
                homePos.y = pos.y;
                hasChaseTarget = 0;
                wolfHeld = g_pWolf->HandleMessage(this, MSG_FREEZE, 0);
                if (hitIndex & 1) {
                    rot.y = 0xaab;
                    hitBody.rot.y = 0xc00;
                } else {
                    rot.y = 0x2ab;
                    hitBody.rot.y = 0x400;
                }
                hitBody.pos = pos;
                showHitBody = 1;
                if (state == GOSSBOSS_ST_SPIN_ATTACK) {
                    hitBody.PlayAnim(AGOOMB01_ANIM_TURN3, 0, 0);
                    SetState(GOSSBOSS_ST_BRIDGE_HIT);
                } else {
                    hitBody.PlayAnim(AGOOMB01_ANIM_LIGHT, 0, 0);
                    SetState(GOSSBOSS_ST_LIGHT);
                }
                StartHitCamera();
                break;
            case MSG_WHEEL_SPIN:
                /* cast kept: only the Wheel sends this message; the boss reads its bossMsg pair */
                chaseTarget = ((Wheel *)sender)->bossMsgSender;
                hasChaseTarget = ((Wheel *)sender)->bossMsgOn;
                break;
        }
    }
    return 0;
}

/* 0x450767 - the state setter: the previous state is kept at +0x80 and each state picks its animation, its timer and
 * the turn it has to complete. States 3, 0xf and 0x15 have no body. */
void Gossamer_Boss::SetState(u8 newState)
{
    Vec3s toWolf;

    toWolf.x = 0;
    toWolf.y = 0;
    toWolf.z = 0;
    prevState = state;
    state = newState;
    switch (state) {
        case GOSSBOSS_ST_TAUNT:
            turnNeeded = 0x800;
            turnAccum = 0;
            turnDirLatch = 0;
            PlayAnim(AGOSSA02_ANIM_STAND1, 1, 0);
            timerMs = 0;
            timerLimitMs = 5000;
            break;
        case GOSSBOSS_ST_STOP:
            PlayAnim(AGOSSA02_ANIM_STOP, 0, 0);
            break;
        case GOSSBOSS_ST_BRIDGE_HIT:
            PlayAnim(AGOSSA02_ANIM_TURN3, 0, 0);
            break;
        case GOSSBOSS_ST_LIGHT:
            PlayAnim(AGOSSA02_ANIM_LIGHT, 1, 0);
            break;
        case GOSSBOSS_ST_WALK_HOME:
            PlayAnim(AGOSSA02_ANIM_RUN2, 1, 0);
            break;
        case GOSSBOSS_ST_WAIT:
            PlayAnim(AGOSSA02_ANIM_STAND1, 1, 0);
            break;
        case GOSSBOSS_ST_RUN:
            turnNeeded = 0x1000;
            timerMs = 0;
            timerLimitMs = 1000;
            turnAccum = 0;
            turnDirLatch = 0;
            PlayAnim(AGOSSA02_ANIM_RUN1, 1, 0);
            break;
        case GOSSBOSS_ST_FALL:
            PlayAnim(AGOSSA02_ANIM_RUN2, 1, 0);
            break;
        case GOSSBOSS_ST_BRIDGE:
            PlayAnim(AGOSSA02_ANIM_BRIDGE, 0, 0);
            break;
        case GOSSBOSS_ST_SPIN:
            spinFacing = GetFacing();
            turnNeeded = 0x3000;
            turnAccum = 0;
            PlayAnim(AGOSSA02_ANIM_TURN1, 1, 0);
            break;
        case GOSSBOSS_ST_SPIN_ATTACK:
            fxOrigin.x = pos.x;
            fxOrigin.y = pos.y - 0xdc;
            fxOrigin.z = pos.z;
            timerMs = 0;
            timerLimitMs = phaseSecs[hitIndex + 1] * 1000 - 10;
            spinDurationMs = phaseSecs[hitIndex + 1] * 1000 - 10;
            PlayAnim(AGOSSA02_ANIM_TURN, 0, 0);
            break;
        case GOSSBOSS_ST_SMASH:
            smashStarted = 0;
            toWolf.x = g_pWolf->pos.x - pos.x;
            toWolf.y = g_pWolf->pos.y - pos.y;
            toWolf.z = g_pWolf->pos.z - pos.z;
            g_pWolf->HandleMessage(this, MSG_WOLF_FORCE_STOW, &toWolf);
            break;
        case GOSSBOSS_ST_SHAKE:
            timerMs = 0;
            timerLimitMs = 0x5dc;
            shakeTicks = 0;
            PlayAnim(AGOSSA02_ANIM_SHAKE, 0, 0);
            break;
        case GOSSBOSS_ST_SPIN_END:
            PlayAnim(AGOSSA02_ANIM_TURN3, 0, 0);
            break;
        case GOSSBOSS_ST_KICK:
            PlayAnim(AGOSSA02_ANIM_KICK, 0, 0);
            caughtWolf = 0;
            break;
        case GOSSBOSS_ST_KICK2:
            if (wolfDist < 100) {
                caughtWolf = 1;
                g_pWolf->HandleMessage(this, MSG_WOLF_SQUASH_LEAP, 0);
            }
            PlayAnim(AGOSSA02_ANIM_KICK2, 0, 0);
            break;
        case GOSSBOSS_ST_GUARD:
            timerMs = 0;
            timerLimitMs = 2000;
            PlayAnim(AGOSSA02_ANIM_STAND1, 1, 0);
            break;
        case GOSSBOSS_ST_CHASE_TARGET:
            PlayAnim(AGOSSA02_ANIM_RUN1, 1, 0);
            break;
        case GOSSBOSS_ST_RETURN_HOME:
            PlayAnim(AGOSSA02_ANIM_RUN1, 1, 0);
            break;
    }
}

/* 0x450f43 - is the point inside the current phase box (usePhaseBox) or inside the arena box? A missing box answers
 * 0, which is what keeps the pre-fight state 0x15 waiting. */
s32 Gossamer_Boss::IsInBox(const Vec3s *p, s32 usePhaseBox)
{
    Box *abox;
    Box *phase;

    if (usePhaseBox) {
        if (phaseBoxes[hitIndex] != 0) {
            phase = phaseBoxes[hitIndex];
            return p->x >= phase->min[0] && p->x <= phase->max[0] && p->z >= phase->min[2] && p->z <= phase->max[2];
        }
    } else {
        if (arenaBox != 0) {
            abox = arenaBox;
            return p->x >= abox->min[0] && p->x <= abox->max[0] && p->z >= abox->min[2] && p->z <= abox->max[2];
        }
    }
    return 0;
}

/* 0x45105c - is the point inside smash box `index`? */
s32 Gossamer_Boss::IsInSmashBox(const Vec3s *p, s32 index)
{
    Box *smash;

    smash = &smashBoxes[index];
    return p->x >= smash->min[0] && p->x <= smash->max[0] && p->z >= smash->min[2] && p->z <= smash->max[2];
}

/* 0x4510d7 - state 6: the boss slides out of the arena; when it leaves the arena box it drops to the arena floor
 * level, loses its shadow, is given a tall thin collision box and goes to state 7. */
void Gossamer_Boss::FallStep()
{
    Translate(&fallStep);
    if (IsInBox(&pos, 0) == 0) {
        fallStep.x = pos.x;
        fallStep.y = pos.y;
        fallStep.z = arenaBox->max[2];
        Shadow_SetVisible(&shadow, 0);
        SetPosition(&fallStep);
        modelBox->max.y = 100;
        modelBox->max.x = 60;
        modelBox->max.z = 500;
        modelBox->min.y = 0;
        modelBox->min.x = -60;
        modelBox->min.z = 0;
        modelBox->flags = 0;
        SetState(GOSSBOSS_ST_BRIDGE);
    }
}

/* 0x451202 - state 2 (between phases): walk back to the current path point at 100 u/s; arriving releases Ralph and
 * the camera and starts the chase again. */
void Gossamer_Boss::WalkHome()
{
    fallStep.x = path->pts[pathIndex].x - pos.x;
    fallStep.z = path->pts[pathIndex].z - pos.z;
    wolfDist = Vec3s_DistXZ(&pos, &homePos);
    if (wolfDist < 100) {
        if (wolfHeld)
            g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0);
        SetState(GOSSBOSS_ST_RUN);
        Camera_ReleaseAny();
        wolfDist = 100;
    }
    SetFacing(HeadingTo(&homePos));
    velocity.x = (s16)(fallStep.x * 100 / (s32)wolfDist);
    velocity.z = (s16)(fallStep.z * 100 / (s32)wolfDist);
    velocity.y = 0;
    Translate(&velocity);
}

/* 0x451359 - the camera for a hit. In the first two phases it is built from the boss position (600 back, 450 up,
 * pitched at it, swung to whichever side the phase is on); in the last one the designer CAMERA property is used. */
/* BYTES(dead-code): here / pitch / rollAngle are computed and never read, as in the original */
/* BYTES(slot-name): names chosen for their buckets: here(8) yaw(11) pitch(13) rollAngle(13); rollAngle declared first so pitch gets the higher slot */
void Gossamer_Boss::StartHitCamera()
{
    /* the names are what pin the frame: here(8) yaw(11) pitch(13) rollAngle(13), and rollAngle must be declared
     * before pitch so that pitch takes the higher slot of their shared bucket (tools/vc6_locals.py) */
    Vec3s here;
    u16 yaw;
    u16 rollAngle;
    u16 pitch;

    here = pos;
    camSetup.eye = pos;
    camSetup.rot[2] = 0;
    camSetup.rot[1] = 0;
    camSetup.rot[0] = 0;
    camSetup.eye.y -= 0x1c2;
    camSetup.eye.x -= 0x258;
    if (hitIndex != 2) {
        camSetup.rot[0] = (s16)(Math_RadiansToAngle4096(atan2((double)camSetup.eye.y - (double)pos.y,
                                                              (double)camSetup.eye.x - (double)pos.x)) &
                                0xfff) -
                          0x800;
        if (hitIndex & 1) {
            camSetup.eye.x += 0x4b0;
            camSetup.rot[1] = 0x400;
        } else {
            camSetup.rot[1] = 0xc00;
        }
        camSetup.rot[2] = 0;
    } else {
        camSetup = *camProp;
        pitch = (u16)((s16)(Math_RadiansToAngle4096(
                                atan2((double)camSetup.eye.z - (double)pos.z, (double)camSetup.eye.y - (double)pos.y)) &
                            0xfff) +
                      0x800);
        rollAngle = (u16)((s16)(Math_RadiansToAngle4096(atan2((double)camSetup.eye.y - (double)pos.y,
                                                              (double)camSetup.eye.x - (double)pos.x)) &
                                0xfff) +
                          0x800);
        yaw = (u16)(0x800 - (s16)(Math_RadiansToAngle4096(atan2((double)camSetup.eye.x - (double)pos.x,
                                                                (double)camSetup.eye.z - (double)pos.z)) &
                                  0xfff));
        StartCam(camSetup.rot[0], yaw, camSetup.rot[2]);
        return;
    }
    StartCam(camSetup.rot[0], camSetup.rot[1], camSetup.rot[2]);
}

/* 0x451710 - vtable +0x14 (level restart): everything back to the state it was in when the fight started, except
 * that the pre-fight state 0x15 is left alone. */
void Gossamer_Boss::Reset()
{
    if (state != GOSSBOSS_ST_EXIT) {
        SetPosition(&spawnPos);
        hitIndex = -1;
        timerMs = 0;
        showHitBody = 0;
        chasingFlat = 0;
        hasChaseTarget = 0;
        showSpinBody = 0;
        pathIndex = 0;
        arenaBox->max[0] = arenaMaxSave.x;
        arenaBox->max[1] = arenaMaxSave.y;
        arenaBox->max[2] = arenaMaxSave.z;
        arenaBox->min[0] = arenaMinSave.x;
        arenaBox->min[1] = arenaMinSave.y;
        arenaBox->min[2] = arenaMinSave.z;
        modelBox->min.x = modelBoxSave.min.x;
        modelBox->min.y = modelBoxSave.min.y;
        modelBox->min.z = modelBoxSave.min.z;
        modelBox->max.x = modelBoxSave.max.x;
        modelBox->max.y = modelBoxSave.max.y;
        modelBox->max.z = modelBoxSave.max.z;
        homePos = pos;
        SetState(GOSSBOSS_ST_GUARD);
        EnableBoxCollide(1);
        soundHandle = 0;
        g_gameFlags |= GF_CINE_SURVIVES_RESTART;
    }
}

/* 0x451921 - state 0: the chase. Close in it freezes Ralph and smashes; far out it gives up; leaving the next smash
 * box gives up too. The move is undone (the saved position is put back) when it would leave the arena box or enter
 * the current phase box, and the final step is a collide-and-slide at 600 u/s. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; pad fill gaps */
void Gossamer_Boss::ChaseStep()
{
    struct {
        ContactInfo hit; /* -0x24 */
        Vec3s here;      /* -0x08 */
        u16 pad;
    } w;

    toTarget.x = g_pWolf->pos.x - pos.x;
    toTarget.z = g_pWolf->pos.z - pos.z;
    toTarget.y = 0;
    turnRefAngle = GetFacing() - 0x800;
    SetFacing(HeadingTo(&g_pWolf->pos));
    if (chasingFlat != 0) {
        if (g_pWolf->HandleMessage(this, MSG_WOLF_IS_SQUASHED, 0) != 0) {
            if (wolfDist < 0x50) {
                SetState(GOSSBOSS_ST_KICK);
                return;
            }
        } else {
            chasingFlat = 0;
            SetState(GOSSBOSS_ST_RETURN_HOME);
            return;
        }
    } else {
        if (wolfDist < 0x8c) {
            g_pWolf->HandleMessage(this, MSG_FREEZE, 0);
            SetState(GOSSBOSS_ST_SMASH);
            return;
        }
        TurnStep(8);
        if (wolfDist > 0x1c2) {
            SetState(GOSSBOSS_ST_RETURN_HOME);
            return;
        }
        if (IsInSmashBox(&pos, hitIndex + 1) == 0) {
            SetState(GOSSBOSS_ST_TAUNT);
            return;
        }
    }
    w.here = pos;
    velocity.x = (s16)(toTarget.x * 600 / (s32)wolfDist);
    velocity.z = (s16)(toTarget.z * 600 / (s32)wolfDist);
    velocity.y = 0;
    Vec3s_ScaleByDt(&velocity, &frameDelta);
    Translate(&frameDelta);
    if (IsInBox(&pos, 0) == 0) {
        SetPosition(&w.here);
        timerMs = 0;
        timerLimitMs = 2000;
        SetState(GOSSBOSS_ST_WAIT);
        return;
    }
    if (hitIndex >= 0 && IsInBox(&pos, 1)) {
        SetPosition(&w.here);
        SetState(GOSSBOSS_ST_STOP);
        return;
    }
    SetPosition(&w.here);
    velocity.x = (s16)(toTarget.x * 600 / (s32)wolfDist);
    velocity.z = (s16)(toTarget.z * 600 / (s32)wolfDist);
    velocity.y = 600;
    Vec3s_ScaleByDt(&velocity, &frameDelta);
    Collide_ResolveMove(&frameDelta, &w.hit, 0x578, RESOLVE_SLIDE_ALL, 0, 0, 10, 0, 0);
    Translate(&frameDelta);
}

/* 0x451c84 - the turn accumulator. It measures the angle to Ralph (state 9) or the boss's own facing against the
 * reference angle taken when the turn started, adds the step to turnAccum, aborts the turn if the sense reverses and
 * enters `nextState` once turnNeeded has been swept. Losing Ralph (further than 500) drops back to state 0x10. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
void Gossamer_Boss::TurnStep(u8 nextState)
{
    struct {
        s16 facing; /* -0x08 */
        u16 step;   /* -0x06 */
        s16 toWolf; /* -0x04 */
        s16 angle;  /* -0x02 */
    } w;

    if (nextState == GOSSBOSS_ST_SPIN_ATTACK) {
        w.toWolf = (s16)(Math_RadiansToAngle4096(
                             atan2((double)g_pWolf->pos.x - (double)pos.x, (double)g_pWolf->pos.z - (double)pos.z)) &
                         0xfff) +
                   0x800;
        w.angle = w.toWolf - 0x800;
    } else {
        w.facing = rot.y;
        w.angle = w.facing - 0x800;
    }
    w.step = (u16)SDW_ABS(turnRefAngle - w.angle);
    if (w.step > 0xbb8) {
        if (turnRefAngle > 0xbb8 && w.angle < 0x3e8)
            turnDir = 1;
        else
            turnDir = -1;
    } else {
        if (turnRefAngle < w.angle)
            turnDir = 1;
        else
            turnDir = -1;
    }
    if (turnDirLatch != 0) {
        if (turnDirLatch != turnDir) {
            if (nextState == GOSSBOSS_ST_SPIN_ATTACK) {
                SetState(GOSSBOSS_ST_GUARD);
            } else {
                turnAccum = 0;
                turnDirLatch = 0;
                return;
            }
        }
    } else {
        turnDirLatch = turnDir;
    }
    if (w.step > 0xbb8) {
        if (w.angle < 0x3e8)
            turnAccum += w.angle;
        else
            turnAccum += (s16)(0x1000 - w.angle);
        if (turnRefAngle < 0x3e8)
            turnAccum += turnRefAngle;
        else
            turnAccum += (s16)(0x1000 - turnRefAngle);
    } else {
        turnAccum += (s16)w.step;
    }
    if (Vec3s_DistXZ(&pos, &g_pWolf->pos) > 0x1f4)
        SetState(GOSSBOSS_ST_GUARD);
    if (turnAccum >= turnNeeded) {
        showSpinBody = 0;
        SetState(nextState);
    }
    if (nextState == GOSSBOSS_ST_SPIN_ATTACK)
        turnRefAngle = w.angle;
}

/* 0x451faa - walk to a point at 600 u/s with collide-and-slide (states 0x11 and 0x12). Arriving switches state, and
 * having or losing a chase target switches between the two. */
void Gossamer_Boss::MoveTo(Vec3s *target)
{
    ContactInfo hit;

    toTarget.x = target->x - pos.x;
    toTarget.z = target->z - pos.z;
    toTarget.y = 0;
    wolfDist = Vec3s_DistXZ(&pos, target);
    if (state == GOSSBOSS_ST_CHASE_TARGET) {
        if (hasChaseTarget == 0)
            SetState(GOSSBOSS_ST_RETURN_HOME);
        if (wolfDist < 0x2bc) {
            SetState(GOSSBOSS_ST_SHAKE);
            return;
        }
    } else {
        if (hasChaseTarget != 0)
            SetState(GOSSBOSS_ST_CHASE_TARGET);
        if (wolfDist < 0x32) {
            SetState(GOSSBOSS_ST_GUARD);
            return;
        }
    }
    SetFacing(HeadingTo(target));
    velocity.x = (s16)(toTarget.x * 600 / (s32)wolfDist);
    velocity.z = (s16)(toTarget.z * 600 / (s32)wolfDist);
    velocity.y = 0;
    Vec3s_ScaleByDt(&velocity, &frameDelta);
    Collide_ResolveMove(&frameDelta, &hit, 0x578, RESOLVE_SLIDE_ALL, 0, 0, 10, 0, 0);
    Translate(&frameDelta);
}

/* 0x45214b - |Ralph's facing - the boss's facing|, wrapped the short way round when it exceeds 3000 (0.73 turn). */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
void Gossamer_Boss::UpdateFacingDelta()
{
    struct {
        u32 delta; /* -0x08 */
        s16 his;   /* -0x04 */
        s16 mine;  /* -0x02 */
    } w;

    w.his = g_pWolf->rot.y;
    w.mine = rot.y;
    w.delta = SDW_ABS(w.his - w.mine);
    if (w.delta > 0xbb8) {
        w.delta = 0;
        if (w.his < 0xbb8) {
            w.delta = w.delta + w.his;
            w.delta = w.delta + (0x1000 - w.mine);
        } else {
            w.delta = w.delta + w.mine;
            w.delta = w.delta + (0x1000 - w.his);
        }
    }
    facingDelta = w.delta;
}
