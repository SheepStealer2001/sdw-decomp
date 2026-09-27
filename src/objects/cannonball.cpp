/* T119 - original object CannonBall.cpp (guessed name).
 * Ranges: .text 0x4a1790-0x4a4bb7, .rdata 0x575c50-0x575c74 (vtable). No .data/.bss. Functions in the original address
 * order. Inline emitters and typed views of the contiguous emitter/parameter storage.
 * g_gameTime is declared u32, as src/engine/time.cpp defines it, so the decorated names agree at link. */
/* BYTES: slot-group, slot-scope, view. */
/* BYTES(view): view: inline emitters and typed views of the contiguous emitter/parameter storage (the access widths are the original's) */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *, Animator *, u16, u32);

#define SDW_MEMBERS_ScnObject          \
    static void *operator new(u32);    \
    void SetDrawBucket(u32 value)      \
    {                                  \
        partHeight = (u8)(value >> 4); \
    }                                  \
    void SetUpdateMode(s32 mode);

#define SDW_MEMBERS_InlineEmitter16 InlineEmitter16();

#define SDW_MEMBERS_CannonBall void StartChaseCamera(u16, u16);
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_SETHEADING_S16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_SETHEADING_S16
#define SDW_INLINE_SCNOBJECT_ENABLEBOXCOLLIDE_S32 1
#define SDW_INLINE_SCNOBJECT_GETFACING 1
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_ENABLEBOXCOLLIDE_S32
#undef SDW_INLINE_SCNOBJECT_GETFACING
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
inline InlineEmitter16::InlineEmitter16()
{
    base.slotPool = slotBuf;
    base.particles = particleBuf;
    base.count = 16;
    base.Emitter_Reset();
}
#include "../engine/scenaric.h"
#include "../engine/scn_tools.h"
#include "camera.h"
#include "../app/app_main.h"
#include "../engine/sound_mgr.h"
#include "../engine/maths.h"
#include "../engine/input.h"
s32 Scenaric_FindByClass(u16, ScnObject **, s32);
extern s32 g_camShakeElapsed, g_camShakeDuration, g_dt;
s32 Rand_Bounded(s32);
#include "../sdk/crt.h"
extern "C" s16 Math_RadiansToAngle4096(float);
void Camera_StartScripted(ScnObject *, Camera *, u16, u16, u16, Vec3s *, u16, u32, s32);
u16 Sound_Play(u16, void *, u16, u8, s32);
s32 Vec3s_DistXZ(Vec3s *, Vec3s *);
extern Wolf *g_pWolf;
extern u32 g_gameTime;
s32 Vec3s_Dist(Vec3s *, Vec3s *);
extern s32 g_dtMs;
#define ABS_VALUE(x) ((x) >= 0 ? (x) : -(x))
#define SDW_INLINE_FREE_PROPU32_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_PROPU32_VOID_U32
#define SDW_INLINE_FREE_ZONES_GET_U8 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_FREE_ZONES_GET_U8
#define SDW_INLINE_ZONELIST_FINDCONTAINING_VEC3S 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_FINDCONTAINING_VEC3S
inline void CannonBall::StartChaseCamera(u16 pitch, u16 yaw)
{
    Camera_StartScripted(this, &g_camera, pitch, yaw, 0, &camPos, 400, 0, 0x1000);
}
#define SDW_INLINE_SCNOBJECT_STOPSOUNDHANDLE_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_STOPSOUNDHANDLE_U16
#define SDW_INLINE_SCNOBJECT_BROADCASTAROUND_S32_S32_U16_U32_VOID 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_BROADCASTAROUND_S32_S32_U16_U32_VOID
void CannonBall::PostLoadInit()
{
    void *properties = record;
    parabolic = PropU32(properties, 4);
    lifetimeMs = PropU32(properties, 0);
    gravityWeight = (u16)PropU32(properties, 8);
    SetUpdateMode(SCN_UPD_ALWAYS);
    debrisSmokeParams.hSpeed = 50;
    trailSmokeParams.hSpeed = 50;
    debrisSmokeParams.vSpeed = -10;
    trailSmokeParams.vSpeed = -10;
    debrisSmokeParams.life = 0xa30;
    trailSmokeParams.life = 0xa30;
    debrisSmokeParams.spawnInterval = 0x146;
    trailSmokeParams.spawnInterval = 0x146;
    trailSmokeParams.sizeStart = 40;
    debrisSmokeParams.sizeStart = 20;
    trailSmokeParams.sizeEnd = 100;
    debrisSmokeParams.sizeEnd = 50;
    debrisSmokeParams.sheetIndex = 0;
    trailSmokeParams.sheetIndex = 0;
    debrisBubbleParams.riseSpeed = -120;
    trailBubbleParams.riseSpeed = -120;
    debrisBubbleParams.life = 0xa30;
    trailBubbleParams.life = 0xa30;
    trailBubbleParams.spawnInterval = 0x146;
    debrisBubbleParams.spawnInterval = 0x32f;
    trailBubbleParams.size = 15;
    debrisBubbleParams.size = 10;
    debrisBubbleParams.cap = 0;
    trailBubbleParams.cap = 0;
    debrisBubbleParams.sheetIndex = 2;
    trailBubbleParams.sheetIndex = 2;
    rumbleMotors[0] = 0xfe;
    rumbleMotors[1] = 0xff;
    shadow.radius = 20;
    /* cast kept: Scn_BuildRecordFromExport fills any synthesised record through a u16 * */
    if (Scn_BuildRecordFromExport(WAR_IDO_AEXPLOS1, (u16 *)&explosionRecord, 0, 0)) {
        explosionFx.Init(&explosionRecord, 0);
        hasExplosionFx = 1;
    } else
        hasExplosionFx = 0;
    fxActive = 0;
    pipeCount = Scenaric_FindByClass(CLASSID_PIPE, pipes, 8);
    pipe2Count = Scenaric_FindByClass(CLASSID_PIPE2, pipes2, 8);
    pipe = 0;
    ownsCamera = 0;
    cameraReleased = 0;
    hitPipe = 0;
    inWater = 0;
    hitWolf = 0;
    noHitDir = 0;
    SetDrawBucket(0xce);
    explodeSound = 0;
    SetState(CB_ST_IDLE);
}
void CannonBall::Reset()
{
    SetState(CB_ST_IDLE);
    SetPosition(&homePos);
    cameraReleased = 0;
    hitPipe = 0;
    inWater = 0;
    hitWolf = 0;
    noHitDir = 0;
    trailSmokeFx.base.Emitter_Reset();
    debrisSmokeFx0.base.Emitter_Reset();
    debrisSmokeFx1.base.Emitter_Reset();
    debrisSmokeFx2.base.Emitter_Reset();
    debrisSmokeFx3.base.Emitter_Reset();
    trailBubbleFx.base.Emitter_Reset();
    debrisBubbleFx0.base.Emitter_Reset();
    debrisBubbleFx1.base.Emitter_Reset();
    debrisBubbleFx2.base.Emitter_Reset();
    debrisBubbleFx3.base.Emitter_Reset();
    explodeSound = 0;
}
/* BYTES(slot-scope, inferred): the nested block(s) only order the frame: their locals are allocated after the enclosing scope's */
void CannonBall::Update()
{
    s32 distance;
    {
        Vec3s saved;
        {
            Vec3i scaled;
            {
                s32 elapsed;
                {
                    u16 i;
                    switch (state) {
                        case CB_ST_PIPE_TRANSIT:
                            elapsed = g_gameTime - transitStartTime;
                            scaled.x = transitDelta.x * elapsed;
                            scaled.y = transitDelta.y * elapsed;
                            scaled.z = transitDelta.z * elapsed;
                            scaled.x /= transitDuration;
                            scaled.y /= transitDuration;
                            scaled.z /= transitDuration;
                            curPos.x = transitStart.x + scaled.x;
                            curPos.y = transitStart.y + scaled.y;
                            curPos.z = transitStart.z + scaled.z;
                            SetPosition(&curPos);
                            trailSmokeFx.base.Emitter_UpdateDrift(&trailSmokeParams, &pos, GetFacing(), 0);
                            if (!cameraReleased) {
                                scaled.x = camTransitDelta.x * elapsed;
                                scaled.y = camTransitDelta.y * elapsed;
                                scaled.z = camTransitDelta.z * elapsed;
                                scaled.x /= transitDuration;
                                scaled.y /= transitDuration;
                                scaled.z /= transitDuration;
                                camPos.x = camTransitStart.x + scaled.x;
                                camPos.y = camTransitStart.y + scaled.y;
                                camPos.z = camTransitStart.z + scaled.z;
                                UpdateChaseCamera(pos);
                            }
                            break;
                        case CB_ST_LOADED:
                            if (!cameraReleased)
                                UpdateChaseCamera(pos);
                            break;
                        case CB_ST_FLY:
                            curPos = pos;
                            if (curPos.x - camPos.x > 200)
                                camPos.x = curPos.x - 200;
                            else if (curPos.x - camPos.x < -200)
                                camPos.x = curPos.x + 200;
                            if (curPos.y - camPos.y > 200)
                                camPos.y = curPos.y - 200;
                            else if (curPos.y - camPos.y < -200)
                                camPos.y = curPos.y + 200;
                            if (curPos.z - camPos.z > 200)
                                camPos.z = curPos.z - 200;
                            else if (curPos.z - camPos.z < -200)
                                camPos.z = curPos.z + 200;
                            if (parabolic) {
                                flightTimeMs += g_dtMs;
                                gravityTicks = (s16)(flightTimeMs / 40);
                            }
                            lifeLeftMs -= g_dtMs;
                            if (lifeLeftMs <= 0) {
                                hitDir.x = frameDelta.x;
                                hitDir.y = frameDelta.y;
                                hitDir.z = frameDelta.z;
                                SetState(CB_ST_EXPLODE);
                                break;
                            }
                            if (hasExplosionFx && !inWater && Zones_Get(ZONE_WATER)->FindContaining(&curPos)) {
                                explosionFx.pos = curPos;
                                explosionFx.PlayAnim(AEXPLOS1_ANIM_BOOM1, 0, 0);
                                fxActive = 1;
                                SetState(CB_ST_EXPLODE);
                            }
                            if (Zones_Get(ZONE_WATER)->FindContaining(&curPos)) {
                                inWater = 1;
                                trailSmokeFx.base.Emitter_UpdateDrift(&trailSmokeParams, &curPos, GetFacing(), 0);
                                trailBubbleFx.base.Emitter_UpdateRiseToCap(&trailBubbleParams, &curPos, 1);
                            } else
                                trailSmokeFx.base.Emitter_UpdateDrift(&trailSmokeParams, &curPos, GetFacing(), 1);
                            velocity.x = launchDir.x * 1000 / 0xfff;
                            velocity.y = launchDir.y * 1000 / 0xfff + gravityTicks * gravityWeight;
                            velocity.z = launchDir.z * 1000 / 0xfff;
                            Vec3s_ScaleByDt(&velocity, &frameDelta);
                            if (lethal && Vec3s_DistXZ(&curPos, &g_pWolf->pos) < 70 &&
                                curPos.y <= g_pWolf->pos.y + 10 && curPos.y >= g_pWolf->pos.y - 190) {
                                /* cast kept: HandleMessage's arg is a void *: this message passes a number in it */
                                g_pWolf->HandleMessage(this, MSG_KILL, (void *)KILL_CANNONBALL);
                                PlayAnim(ABOULE02_ANIM_ANVIL, 0, 1);
                                rot.y = g_pWolf->GetFacing() + 0x800;
                                SetPosition(&g_pWolf->pos);
                                hitWolf = 1;
                                SetState(CB_ST_EXPLODE);
                                break;
                            }
                            if (hitPipe) {
                                SetState(CB_ST_EXPLODE);
                                break;
                            }
                            saved.x = frameDelta.x;
                            saved.y = frameDelta.y;
                            saved.z = frameDelta.z;
                            if (SweepMove(&frameDelta, curPos, 2))
                                break;
                            frameDelta.x = saved.x;
                            frameDelta.y = saved.y;
                            frameDelta.z = saved.z;
                            Translate(&frameDelta);
                            if (!cameraReleased)
                                UpdateChaseCamera(pos);
                            if ((frameDelta.x == 0) & (frameDelta.y == 0) & (frameDelta.z == 0)) {
                                hitDir.x = frameDelta.x;
                                hitDir.y = frameDelta.y;
                                hitDir.z = frameDelta.z;
                                SetState(CB_ST_EXPLODE);
                            }
                            break;
                        case CB_ST_HOMING:
                            curPos = pos;
                            trailSmokeFx.base.Emitter_UpdateDrift(&trailSmokeParams, &curPos, GetFacing(), 0);
                            if (Vec3s_DistXZ(&curPos, &g_pWolf->pos) < 70) {
                                /* cast kept: HandleMessage's arg is a void *: this message passes a number in it */
                                g_pWolf->HandleMessage(this, MSG_KILL, (void *)KILL_CANNONBALL);
                                PlayAnim(ABOULE02_ANIM_ANVIL, 0, 1);
                                rot.y = g_pWolf->GetFacing() + 0x800;
                                SetPosition(&g_pWolf->pos);
                                hitWolf = 1;
                                SetState(CB_ST_EXPLODE);
                                break;
                            }
                            distance = Vec3s_Dist(&homingTarget, &curPos);
                            launchDir.x = homingTarget.x - curPos.x;
                            launchDir.y = homingTarget.y - curPos.y;
                            launchDir.z = homingTarget.z - curPos.z;
                            velocity.x = launchDir.x * 1000 * 2 / distance;
                            velocity.y = launchDir.y * 1000 * 2 / distance;
                            velocity.z = launchDir.z * 1000 * 2 / distance;
                            Vec3s_ScaleByDt(&velocity, &frameDelta);
                            Translate(&frameDelta);
                            if (!cameraReleased)
                                UpdateChaseCamera(pos);
                            break;
                        case CB_ST_PIPE_ENTER:
                            trailSmokeFx.base.Emitter_UpdateDrift(&trailSmokeParams, &pos, GetFacing(), 1);
                            distSq = Vec3s_DistSq(&pos, &targetPos);
                            if (distSq >= prevDistSq) {
                                transitStart.x = targetPos.x;
                                transitStart.y = targetPos.y;
                                transitStart.z = targetPos.z;
                                pipe->HandleMessage(this, MSG_PIPE_EJECT, 0);
                                SetState(CB_ST_IDLE);
                                break;
                            }
                            velocity.x = targetPos.x - pos.x;
                            velocity.y = targetPos.y - pos.y;
                            velocity.z = targetPos.z - pos.z;
                            velocity.x *= 1000;
                            velocity.y *= 1000;
                            velocity.z *= 1000;
                            velocity.x /= 35;
                            velocity.y /= 35;
                            velocity.z /= 35;
                            Vec3s_ScaleByDt(&velocity, &frameDelta);
                            Translate(&frameDelta);
                            if (!cameraReleased)
                                UpdateChaseCamera(pos);
                            prevDistSq = distSq;
                            break;
                        case CB_ST_EXPLODE:
                            explodeTimerMs -= g_dtMs;
                            if (explodeTimerMs <= 0) {
                                SetPosition(&homePos);
                                /* cast kept: HandleMessage's arg is a void *: this message passes a number in it */
                                launcher->HandleMessage(this, MSG_BALL_RETURNED, (void *)hitClassId);
                                for (i = 0; i < pipeCount; i++)
                                    /* cast kept: HandleMessage's arg is a void *: this message passes a number in it */
                                    pipes[i]->HandleMessage(this, MSG_BALL_RETURNED, (void *)hitClassId);
                                for (i = 0; i < pipe2Count; i++)
                                    /* cast kept: HandleMessage's arg is a void *: this message passes a number in it */
                                    pipes2[i]->HandleMessage(this, MSG_BALL_RETURNED, (void *)hitClassId);
                                Camera_StopShake();
                                trailSmokeFx.base.Emitter_Reset();
                                debrisSmokeFx0.base.Emitter_Reset();
                                debrisSmokeFx1.base.Emitter_Reset();
                                debrisSmokeFx2.base.Emitter_Reset();
                                debrisSmokeFx3.base.Emitter_Reset();
                                trailBubbleFx.base.Emitter_Reset();
                                debrisBubbleFx0.base.Emitter_Reset();
                                debrisBubbleFx1.base.Emitter_Reset();
                                debrisBubbleFx2.base.Emitter_Reset();
                                debrisBubbleFx3.base.Emitter_Reset();
                                if (ownsCamera)
                                    Camera_ReleaseScripted(this);
                                SetState(CB_ST_IDLE);
                                break;
                            }
                            if (debrisSmokeParams.sizeEnd < 10 && debrisSmokeParams.sizeStart > 1)
                                --debrisSmokeParams.sizeStart;
                            if (debrisSmokeParams.sizeEnd > 1)
                                --debrisSmokeParams.sizeEnd;
                            ++gravityTicks;
                            if (hitWolf) {
                                SetPosition(&g_pWolf->pos);
                                rot.y = g_pWolf->GetFacing() + 0x800;
                            } else {
                                UpdateDebris(0, &trailSmokeFx.base, &trailBubbleFx.base);
                                UpdateDebris(1, &debrisSmokeFx0.base, &debrisBubbleFx0.base);
                                UpdateDebris(2, &debrisSmokeFx1.base, &debrisBubbleFx1.base);
                                UpdateDebris(3, &debrisSmokeFx2.base, &debrisBubbleFx2.base);
                                UpdateDebris(4, &debrisSmokeFx3.base, &debrisBubbleFx3.base);
                            }
                            if (!cameraReleased)
                                UpdateChaseCamera(pos);
                            break;
                    }
                    if (hasExplosionFx && fxActive) {
                        if (explosionFx.AnimFlags(ANIM_F_FINISHED))
                            fxActive = 0;
                        explosionFx.AdvanceAnim();
                    }
                    AdvanceAnim();
                }
            }
        }
    }
}
s32 CannonBall::HandleMessage(ScnObject *sender, u32 msg, void *arg)
{
    switch (msg) {
        case MSG_CB_LOAD:
            /* cast kept (these three reads): the message arg is a void *: this message carries a Vec3s * */
            launchDir.x = ((Vec3s *)arg)->x;
            launchDir.y = ((Vec3s *)arg)->y;
            launchDir.z = ((Vec3s *)arg)->z;
            PlaceChaseCamera();
            SetState(CB_ST_LOADED);
            break;
        case MSG_CB_LAUNCH:
            /* cast kept (these three reads): the message arg is a void *: this message carries a Vec3s * */
            launchDir.x = ((Vec3s *)arg)->x;
            launchDir.y = ((Vec3s *)arg)->y;
            launchDir.z = ((Vec3s *)arg)->z;
            if (sender->GetClassId() == CLASSID_CANONSIMPLE || sender->GetClassId() == CLASSID_CANONSHEEP ||
                sender->GetClassId() == CLASSID_ROOK) {
                launcher = sender;
                PlaceChaseCamera();
                firedByCannon = 1;
            } else {
                camPos.x = camGoal.x;
                camPos.y = camGoal.y;
                camPos.z = camGoal.z;
                firedByCannon = 0;
            }
            SetState(CB_ST_FLY);
            break;
        case MSG_CB_FIRE_AT:
            /* cast kept (these three reads): the message arg is a void *: this message carries a Vec3s * */
            homingTarget.x = ((Vec3s *)arg)->x;
            homingTarget.y = ((Vec3s *)arg)->y;
            homingTarget.z = ((Vec3s *)arg)->z;
            launchDir.x = homingTarget.x - sender->pos.x;
            launchDir.y = homingTarget.y - sender->pos.y;
            launchDir.z = homingTarget.z - sender->pos.z;
            launcher = sender;
            PlaceChaseCamera();
            SetState(CB_ST_HOMING);
            break;
        case MSG_CB_PLACE:
            /* cast kept (these three reads): the message arg is a void *: this message carries a Vec3s * */
            curPos.x = ((Vec3s *)arg)->x;
            curPos.y = ((Vec3s *)arg)->y;
            curPos.z = ((Vec3s *)arg)->z;
            if (sender->GetClassId() == CLASSID_CANONSIMPLE ||
                ((sender->GetClassId() == CLASSID_CANONSHEEP) | (sender->GetClassId() == CLASSID_ROOK))) {
                launcher = sender;
                homePos = curPos;
                SetPosition(&curPos);
            } else if (state == CB_ST_IDLE)
                SetPosition(&curPos);
            break;
        case MSG_CB_ENTER_PIPE:
            hitPipe = 0;
            if ((state == CB_ST_PIPE_ENTER) | (state == CB_ST_PIPE_TRANSIT) | (state == CB_ST_IDLE))
                return 1;
            pipe = sender;
            /* cast kept (these three reads): the message arg is a void *: this message carries a Vec3s * */
            targetPos.x = ((Vec3s *)arg)->x;
            targetPos.y = ((Vec3s *)arg)->y;
            targetPos.z = ((Vec3s *)arg)->z;
            launchDir.x = targetPos.x - pos.x;
            launchDir.y = targetPos.y - pos.y;
            launchDir.z = targetPos.z - pos.z;
            distSq = Vec3s_DistSq(&pos, &targetPos);
            prevDistSq = distSq + 1;
            SetState(CB_ST_PIPE_ENTER);
            break;
        case MSG_CB_PIPE_TRANSIT:
            /* cast kept: the message arg is a void *: what it carries depends on the message id */
            transitDuration = (s32)arg;
            transitStartTime = g_gameTime;
            targetPos.x = pos.x;
            targetPos.y = pos.y;
            targetPos.z = pos.z;
            SetPosition(&transitStart);
            camTransitStart.x = camPos.x;
            camTransitStart.y = camPos.y;
            camTransitStart.z = camPos.z;
            transitDelta.x = targetPos.x - pos.x;
            transitDelta.y = targetPos.y - pos.y;
            transitDelta.z = targetPos.z - pos.z;
            break;
        case MSG_CB_PIPE_CAMERA:
            /* cast kept (these three reads): the message arg is a void *: this message carries a Vec3s * */
            launchDir.x = ((Vec3s *)arg)->x;
            launchDir.y = ((Vec3s *)arg)->y;
            launchDir.z = ((Vec3s *)arg)->z;
            camGoal.x = targetPos.x - launchDir.x / 8;
            camGoal.y = targetPos.y - 200;
            camGoal.z = targetPos.z - launchDir.z / 8;
            if (targetPos.x - camGoal.x > 200)
                camGoal.x = targetPos.x - 200;
            else if (curPos.x - camGoal.x < -200)
                camGoal.x = targetPos.x + 200;
            camGoal.y = targetPos.y - 200;
            if (targetPos.z - camGoal.z > 200)
                camGoal.z = targetPos.z - 200;
            else if (curPos.z - camGoal.z < -200)
                camGoal.z = targetPos.z + 200;
            camTransitDelta.x = camGoal.x - camTransitStart.x;
            camTransitDelta.y = camGoal.y - camTransitStart.y;
            camTransitDelta.z = camGoal.z - camTransitStart.z;
            SetState(CB_ST_PIPE_TRANSIT);
            break;
        case MSG_CB_RELEASE_CAMERA:
            cameraReleased = 1;
            Camera_ReleaseScripted(this);
            break;
        case MSG_CB_SET_OWNS_CAMERA:
            /* cast kept: the message arg is a void *: what it carries depends on the message id */
            ownsCamera = (u32)arg;
            break;
        case MSG_CB_SET_LETHAL:
            lethal = (u32)arg; /* cast kept: the message arg is a void *: what it carries depends on the message id */
            break;
    }
    return 0;
}
void CannonBall::SetState(u8 next)
{
    s16 distance;
    s32 i;
    s16 strength;
    state = next;
    switch (next) {
        case CB_ST_IDLE:
            cameraReleased = 0;
            break;
        case CB_ST_FLY:
            lifeLeftMs = lifetimeMs;
            hitPipe = 0;
            PlayAnim(ABOULE02_ANIM_STAND, 0, 0);
            EnableBoxCollide(1);
            inWater = 0;
            flightTimeMs = 0;
            gravityTicks = 0;
            break;
        case CB_ST_HOMING:
            PlayAnim(ABOULE02_ANIM_STAND, 0, 0);
            break;
        case CB_ST_EXPLODE:
            lifeLeftMs = lifetimeMs;
            EnableBoxCollide(0);
            gravityTicks = 0;
            explodeTimerMs = 2500;
            debrisSmokeParams.sizeStart = 20;
            debrisSmokeParams.sizeEnd = 50;
            for (i = 0; i < 5; i++) {
                debrisPos[i].x = pos.x;
                debrisPos[i].y = pos.y;
                debrisPos[i].z = pos.z;
                debrisVel[i].x = Rand_Range(-800, 800);
                debrisVel[i].y = Rand_Range(-800, 800);
                debrisVel[i].z = Rand_Range(-800, 800);
            }
            Camera_StartShake(100, 1250);
            distance = (s16)Vec3s_DistXZ(&g_pWolf->pos, &pos);
            strength = 4096 - distance * 4 / 6;
            if (strength < 0)
                strength = 0;
            g_pad.Rumble_stub(1250, rumbleMotors, strength);
            /* cast kept: HandleMessage's arg is a void *: this message passes a number in it */
            BroadcastAround(70, 70, 70, MSG_KILL, (void *)KILL_GENERIC);
            if (hasExplosionFx && !hitWolf) {
                StopSoundHandle(explodeSound);
                explodeSound = Sound_Play(SND_SR2ROFA2, this, 0xff, SNDF_NO_RETRIGGER, 0x1000);
                if (noHitDir)
                    explosionFx.PlayAnim(AEXPLOS1_ANIM_BOOM1, 0, 0);
                else {
                    explosionFx.SetHeading(Math_RadiansToAngle4096((float)atan2(hitDir.x, hitDir.z)) & 0xfff);
                    explosionFx.PlayAnim(AEXPLOS1_ANIM_BOOM1A, 0, 0);
                }
                explosionFx.pos = pos;
                fxActive = 1;
            }
            break;
    }
}
void CannonBall::Render(Camera *view)
{
    trailSmokeFx.base.Emitter_Render(view, 0);
    trailBubbleFx.base.Emitter_Render(view, 0);
    if (state != CB_ST_IDLE) {
        debrisSmokeFx0.base.Emitter_Render(view, 0);
        debrisSmokeFx1.base.Emitter_Render(view, 0);
        debrisSmokeFx2.base.Emitter_Render(view, 0);
        debrisSmokeFx3.base.Emitter_Render(view, 0);
        debrisBubbleFx0.base.Emitter_Render(view, 0);
        debrisBubbleFx1.base.Emitter_Render(view, 0);
        debrisBubbleFx2.base.Emitter_Render(view, 0);
        debrisBubbleFx3.base.Emitter_Render(view, 0);
    }
    if (hasExplosionFx)
        explosionFx.Render(view);
    if (state == CB_ST_FLY || state == CB_ST_HOMING || state == CB_ST_PIPE_ENTER || hitWolf)
        RenderFacingCamera(view, 0, 0, 0);
}
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; unused20, unused16, unused0e fill gaps */
void CannonBall::UpdateChaseCamera(Vec3s target)
{
    /* Original stack storage -2c..-1; the padding slots are not touched. */
    struct Work {
        Vec3i square;
        u16 unused20;
        s16 amplitude;
        Vec3s shake;
        u16 unused16;
        Vec3s eye;
        u16 unused0e;
        Vec3i delta;
    } w;
    if (ownsCamera) {
        if (g_camShakeAmplitude > 0) {
            g_camShakeElapsed += g_dt;
            if (g_camShakeElapsed <= g_camShakeDuration) {
                w.amplitude = g_camShakeAmplitude * (g_camShakeDuration - g_camShakeElapsed) / g_camShakeDuration;
                if (w.amplitude) {
                    w.shake.x = Rand_Bounded(w.amplitude * 2) - w.amplitude;
                    w.shake.z = Rand_Bounded(w.amplitude * 2) - w.amplitude;
                    w.shake.y = Rand_Bounded(w.amplitude * 2) - w.amplitude;
                }
            } else {
                g_camShakeAmplitude = 0;
                w.shake.x = 0;
                w.shake.y = 0;
                w.shake.z = 0;
            }
        } else {
            w.shake.x = 0;
            w.shake.y = 0;
            w.shake.z = 0;
        }
        w.eye.x = w.shake.x + camPos.x;
        w.eye.y = w.shake.y + camPos.y;
        w.eye.z = w.shake.z + camPos.z;
        w.delta.x = target.x - w.eye.x;
        w.delta.y = target.y - w.eye.y;
        w.delta.z = target.z - w.eye.z;
        w.square.x = w.delta.x * w.delta.x;
        w.square.y = w.delta.y * w.delta.y;
        w.square.z = w.delta.z * w.delta.z;
        camPitch =
            Math_RadiansToAngle4096((float)atan2(w.delta.y, (s32)sqrt((double)w.square.x + (double)w.square.z))) &
            0xfff;
        camYaw = Math_RadiansToAngle4096((float)atan2(-w.delta.x, w.delta.z)) & 0xfff;
        StartChaseCamera(camPitch, camYaw);
    }
}
void CannonBall::UpdateDebris(s32 index, ParticleEmitter *smoke, ParticleEmitter *bubbles)
{
    u16 a;
    velocity.x = (debrisVel[index].x + 100) * 1000 / 0xfff;
    velocity.y = debrisVel[index].y * 1000 / 0xfff + gravityTicks * 15 / 2;
    velocity.z = (debrisVel[index].z + 100) * 1000 / 0xfff;
    Vec3s_ScaleByDt(&velocity, &frameDelta);
    a = Collide_ResolveMove(&frameDelta, &contact, 0xb54, RESOLVE_SLIDE_ALL, &debrisPos[index], 0, 10, 0, 0);
    if (a == COLL_WALL) {
        debrisVel[index].x = contact.wallNormalMean.x;
        debrisVel[index].y = contact.wallNormalMean.y;
        debrisVel[index].z = contact.wallNormalMean.z;
        debrisVel[index].x /= 5;
        debrisVel[index].y /= 5;
        debrisVel[index].z /= 5;
    } else if (a == 0) {
        debrisPos[index].x = frameDelta.x + debrisPos[index].x;
        debrisPos[index].y = frameDelta.y + debrisPos[index].y;
        debrisPos[index].z = frameDelta.z + debrisPos[index].z;
    }
    if (Zones_Get(ZONE_WATER)->FindContaining(&debrisPos[index])) {
        smoke->Emitter_UpdateDrift(&debrisSmokeParams, &debrisPos[index], GetFacing(), 0);
        bubbles->Emitter_UpdateRiseToCap(&debrisBubbleParams, &debrisPos[index], 1);
    } else
        smoke->Emitter_UpdateDrift(&debrisSmokeParams, &debrisPos[index], GetFacing(), 1);
}
void CannonBall::PlaceChaseCamera()
{
    s16 a;
    if (ownsCamera) {
        camPos.x = pos.x;
        camPos.y = pos.y;
        camPos.z = pos.z;
        camPos.y -= 200;
        lifeLeftMs = lifetimeMs;
        if (ABS_VALUE(launchDir.x) > ABS_VALUE(launchDir.z)) {
            a = launchDir.x / 200;
            camPos.x -= (s16)((launchDir.x >= 0 ? 1 : -1) * 200);
            if (a)
                camPos.z -= (s16)(launchDir.z / ABS_VALUE(a));
        } else {
            a = launchDir.z / 200;
            camPos.z -= (s16)((launchDir.z >= 0 ? 1 : -1) * 200);
            if (a)
                camPos.x -= (s16)(launchDir.x / ABS_VALUE(a));
        }
    }
}
s32 CannonBall::SweepMove(Vec3s *delta, Vec3s probe, u8 steps)
{
    Vec3s original;
    u8 i;
    original.x = delta->x;
    original.y = delta->y;
    original.z = delta->z;
    delta->x /= steps;
    delta->y /= steps;
    delta->z /= steps;
    for (i = 0; i < steps; i++) {
        if (Collide_ResolveMove(delta, &contact, 0xb54, RESOLVE_SLIDE_ALL, &probe, 0, 10, 0, 0)) {
            if (contact.movableObj) {
                hitClassId = contact.movableObj->GetClassId();
                switch (contact.movableObj->GetClassId()) {
                    case CLASSID_ROCK:
                        /* cast kept: HandleMessage's arg is a void *: this message passes a number in it */
                        contact.movableObj->HandleMessage(this, MSG_CANNONBALL_HIT, (void *)1000);
                        break;
                    case CLASSID_WOLF:
                        /* cast kept: HandleMessage's arg is a void *: this message passes a number in it */
                        contact.movableObj->HandleMessage(this, MSG_KILL, (void *)KILL_CANNONBALL);
                        PlayAnim(ABOULE02_ANIM_ANVIL, 0, 1);
                        rot.y = contact.movableObj->GetFacing() + 0x800;
                        SetPosition(&contact.movableObj->pos);
                        hitWolf = 1;
                        break;
                    default:
                        /* cast kept: HandleMessage's arg is a void *: this message passes a number in it */
                        contact.movableObj->HandleMessage(this, MSG_KILL, (void *)KILL_GENERIC);
                        break;
                }
                hitDir.x = frameDelta.x;
                hitDir.y = frameDelta.y;
                hitDir.z = frameDelta.z;
                SetState(CB_ST_EXPLODE);
                return 1;
            } else if (contact.wallObj) {
                hitClassId = contact.wallObj->GetClassId();
                hitDir.x = contact.wallNormalMean.x;
                hitDir.y = contact.wallNormalMean.y;
                hitDir.z = contact.wallNormalMean.z;
                switch (contact.wallObj->GetClassId()) {
                    case CLASSID_CANONSIMPLE:
                    case CLASSID_CANONDUMMY:
                    case CLASSID_CANONSHEEP:
                        if (firedByCannon) {
                            delta->x = original.x;
                            delta->y = original.y;
                            delta->z = original.z;
                        } else {
                            SetState(CB_ST_EXPLODE);
                            return 1;
                        }
                        break;
                    case CLASSID_HIVE:
                        launcher->HandleMessage(this, MSG_FREEZE, 0);
                        contact.wallObj->HandleMessage(this, MSG_HIVE_HIT, 0);
                        ownsCamera = 0;
                        SetState(CB_ST_EXPLODE);
                        return 1;
                    case CLASSID_PIPE:
                    case CLASSID_PIPE2:
                        hitPipe = 1;
                        return 1;
                    default:
                        /* cast kept: HandleMessage's arg is a void *: this message passes a number in it */
                        contact.wallObj->HandleMessage(this, MSG_KILL, (void *)KILL_GENERIC);
                        SetState(CB_ST_EXPLODE);
                        return 1;
                }
            } else if (contact.floorObj) {
                if (contact.floorObj->GetClassId() == CLASSID_HIVE) {
                    launcher->HandleMessage(this, MSG_FREEZE, 0);
                    contact.floorObj->HandleMessage(this, MSG_HIVE_HIT, 0);
                    ownsCamera = 0;
                }
                hitDir.x = contact.wallNormalMean.x;
                hitDir.y = contact.wallNormalMean.y;
                hitDir.z = contact.wallNormalMean.z;
                hitClassId = contact.floorObj->GetClassId();
                /* cast kept: HandleMessage's arg is a void *: this message passes a number in it */
                contact.floorObj->HandleMessage(this, MSG_KILL, (void *)KILL_GENERIC);
                SetState(CB_ST_EXPLODE);
                return 1;
            } else {
                if (contact.wallNormalMean.x == 0 && contact.wallNormalMean.y == 0 && contact.wallNormalMean.z == 0)
                    noHitDir = 1;
                else {
                    hitDir.x = contact.wallNormalMean.x;
                    hitDir.y = contact.wallNormalMean.y;
                    hitDir.z = contact.wallNormalMean.z;
                    noHitDir = 0;
                }
                hitClassId = CLASSID_NONE;
                SetState(CB_ST_EXPLODE);
                return 1;
            }
        }
        probe.x += delta->x;
        probe.y += delta->y;
        probe.z += delta->z;
    }
    return 0;
}
ScnObject *CannonBall_Create(void *record)
{
    CannonBall *object = new CannonBall;
    object = (CannonBall *)object->Init(record, 0); /* cast kept: Init returns the ScnBody * base of this object */
    return object;
}
