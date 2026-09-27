/* PAL PC GeyserIn. */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *, Animator *, u16, u32);
#include "../engine/sound_mgr.h"
#include "../app/app_main.h"
#include "camera.h"
#include "../engine/scn_tools.h"
#include "../engine/approach.h"
class ScnObject;
class Camera;
void Camera_StartScripted(ScnObject *, Camera *, u16, u16, u16, Vec3s *, u16, u32, s32);
#define UPDATE_MODE_BODY                         \
    switch (mode) {                              \
        case SCN_UPD_NORMAL:                     \
            flags &= (u16)~SCN_OF_ALWAYS_UPDATE; \
            flags &= (u16)~SCN_OF_NEVER_UPDATE;  \
            break;                               \
        case SCN_UPD_ALWAYS:                     \
            flags |= SCN_OF_ALWAYS_UPDATE;       \
            flags &= (u16)~SCN_OF_NEVER_UPDATE;  \
            break;                               \
        case SCN_UPD_NEVER:                      \
            flags &= (u16)~SCN_OF_ALWAYS_UPDATE; \
            flags |= SCN_OF_NEVER_UPDATE;        \
            break;                               \
        case SCN_UPD_CINE:                       \
            flags |= SCN_OF_CINE_UPDATE;         \
            break;                               \
    }

#define SDW_MEMBERS_ScnObject                                                           \
    static void *operator new(u32);                                                     \
    void SetRotation(const Vec3s &value);                                               \
    s32 GetAttachedPosition(Vec3s &out);                                                \
    void StartCamera(u16 x, u16 y, u16 z, Vec3s *point, u16 focal, u32 mode, s32 time); \
    void SetUpdateMode(s32 mode);                                                       \
    void SetUpdateMode(u8 mode);

#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_STOPSOUND_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_STOPSOUND_U16
#define SDW_INLINE_SCNOBJECT_SETROTATION_CONST_VEC3S 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETROTATION_CONST_VEC3S
#define SDW_INLINE_SCNOBJECT_INSTANCEFLAGS_U16 1
#define SDW_INLINE_SCNOBJECT_STARTCAMERA_U16_U16_U16_VEC3S_U16_U32_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_INSTANCEFLAGS_U16
#undef SDW_INLINE_SCNOBJECT_STARTCAMERA_U16_U16_U16_VEC3S_U16_U32_S32
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32 1
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
inline s32 ScnObject::GetAttachedPosition(Vec3s &out)
{
    if (!InstanceFlags(INST_F_ATTACHED))
        return 0;
    out.x = attachLink->parentObj->pos.x;
    out.y = attachLink->parentObj->pos.y;
    out.z = attachLink->parentObj->pos.z;
    return 1;
}
GeyserIn *g_geyserInBusy; /* T154 .bss 0x6cf65c */
#define g_camPos (g_camera.pos)

extern Wolf *g_pWolf;
extern s32 g_dtMs;
u16 Sound_Play(u16, void *, u16, u8, s32);
inline s32 SoundPlaying(u16 sound)
{
    return Sound_IsPlaying(sound);
}
s32 ObjGrid_QueryBoxPoints(const CollBox *, ScnObject **);
#define SDW_INLINE_FREE_GETPROP_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_GETPROP_VOID_U32
void GeyserIn::PostLoadInit()
{
    void *props = record;
    flags = 0;
    soundHandle = 0;
    /* cast kept: Box and CollBox are two views of one 16-byte record; the getter returns the Box view */
    detectBox = (CollBox *)Scn_GetPropBox(props, 4);
    geyserOut = Scn_GetPropObject(props, 0x14);
    timerMs = 1000;
    activePeriod = GetProp(props, 0x20) << 10;
    transitDelayMs = GetProp(props, 0x1c) * 1000;
    corkPeriod = GetProp(props, 0x18) << 10;
    idlePeriod = GetProp(props, 0xc) << 10;
    /* cast kept: HandleMessage's arg is a void *; this message passes a number in it */
    geyserOut->HandleMessage(this, MSG_GEYSEROUT_SET_DELAY, (void *)activePeriod);
    geyserController = Scn_GetPropObject(props, 0x10);
    flags |= GEYSERIN_F_CONTROLLED_ON;
    g_geyserInBusy = 0;
    if (rot.x == 0 && rot.y == 0 && rot.z != 0)
        flags |= GEYSERIN_F_Z_ONLY;
    SetUpdateMode(SCN_UPD_ALWAYS);
    if (geyserOut->HandleMessage(this, MSG_GEYSEROUT_IS_BUSY, 0))
        SetState(GEYSERIN_ST_ACTIVE);
    else
        SetState(GEYSERIN_ST_IDLE);
}
void GeyserIn::Reset()
{
    soundHandle = 0;
}
void GeyserIn::Update()
{
    s32 cameraDuration;
    Vec3s rotation, hiddenPos, attachmentPos;
    s32 count;
    ScnObject *nearby[5];
    u16 slot = 0;
    s32 speed;
    if (SoundPlaying(soundHandle)) {
        if (Vec3s_DistSq(&pos, &g_camPos) > 0x736504)
            StopSound(soundHandle);
    } else if (Vec3s_DistSq(&pos, &g_camPos) < 0x5f5e10 && animState != GEYSERIN_AS_IDLE)
        soundHandle = Sound_Play(SND_GEYSER, this, 255,
                                 SNDF_LOOP | SNDF_POSITIONAL | SNDF_DIST_HORIZONTAL | SNDF_NO_RETRIGGER, 4096);
    switch (state) {
        case GEYSERIN_ST_IDLE:
            timerMs -= g_dtMs;
            if (timerMs <= 0) {
                if ((flags & GEYSERIN_F_CONTROLLED_ON) || geyserController) {
                    SetState(GEYSERIN_ST_ACTIVE);
                    break;
                }
                if ((flags & GEYSERIN_F_CONTROLLED_ON) && !geyserController) {
                    SetState(GEYSERIN_ST_ACTIVE);
                    break;
                }
            }
            if (geyserOut->HandleMessage(this, MSG_GEYSEROUT_IS_BUSY, 0)) {
                SetState(GEYSERIN_ST_ACTIVE);
                break;
            }
            break;
        case GEYSERIN_ST_ACTIVE:
            timerMs -= g_dtMs;
            if (!geyserOut->HandleMessage(this, MSG_GEYSEROUT_IS_BUSY, 0) &&
                (timerMs <= 0 || (~flags & GEYSERIN_F_CONTROLLED_ON))) {
                geyserOut->HandleMessage(this, MSG_GEYSEROUT_START, 0);
                SetState(GEYSERIN_ST_IDLE);
                victim = 0;
                break;
            } else if (AnimFlags(ANIM_F_FINISHED)) {
                SetAnimState(GEYSERIN_AS_CORKED);
                break;
            } else if (!g_geyserInBusy && !g_pWolf->HandleMessage(this, MSG_WOLF_IS_DEAD, 0)) {
                count = ObjGrid_QueryBoxPoints(detectBox, nearby);
                for (slot = 0; slot < count; slot++) {
                    if (nearby[slot] != this && nearby[slot]->GetClassId() != CLASSID_SAM &&
                        !nearby[slot]->GetAttachedPosition(attachmentPos)) {
                        if (nearby[slot]->GetClassId() == CLASSID_ROCK) {
                            flags |= GEYSERIN_F_ROCK_CORK;
                            victim = nearby[slot];
                            g_geyserInBusy = this;
                            SetState(GEYSERIN_ST_CORKED);
                            break;
                        } else {
                            victim = nearby[slot];
                            slot = (u16)count;
                            SetState(GEYSERIN_ST_SUCK);
                            break;
                        }
                    }
                }
            }
            break;
        case GEYSERIN_ST_SUCK:
            timerMs -= g_dtMs;
            speed = 10;
            rotation.x = victim->rot.x;
            rotation.y = victim->rot.y;
            rotation.z = victim->rot.z;
            Vec3s_ApproachPoint(&rotation, &rot, &speed, 100000, 100000, 100000);
            victim->SetRotation(rotation);
            if (pullTimerMs >= 0) {
                frameDelta.x = pullOffset.x;
                frameDelta.y = pullOffset.y;
                frameDelta.z = pullOffset.z;
                frameDelta.x *= (s16)g_dtMs;
                frameDelta.y *= (s16)g_dtMs;
                frameDelta.z *= (s16)g_dtMs;
                frameDelta.x /= 840;
                frameDelta.y /= 840;
                frameDelta.z /= 840;
                victim->Translate(&frameDelta);
                pullTimerMs -= g_dtMs;
            }
            if (timerMs <= 0) {
                /* cast kept: HandleMessage's arg is a void *; this message passes a number in it */
                geyserOut->HandleMessage(this, MSG_GEYSEROUT_SET_DELAY, (void *)activePeriod);
                cameraDuration = (transitDelayMs << 12) / 1000;
                if (geyserOut->HandleMessage(this, MSG_GEYSEROUT_WANTS_WOLF_CAM, 0))
                    flags |= GEYSERIN_F_CAM_WOLF;
                else
                    flags &= ~GEYSERIN_F_CAM_WOLF;
                if (geyserOut->HandleMessage(this, MSG_GEYSEROUT_WANTS_OBJ_CAM, 0))
                    flags |= GEYSERIN_F_CAM_OBJ;
                else
                    flags &= ~GEYSERIN_F_CAM_OBJ;
                /* cast kept: this query returns the camera setup's address as its s32 reply */
                cameraSetup = (CamSetup *)geyserOut->HandleMessage(this, MSG_GEYSEROUT_GET_CAMERA, 0);
                if (victim->GetClassId() == CLASSID_WOLF) {
                    g_pWolf->HandleMessage(this, MSG_FREEZE, 0);
                    if (flags & GEYSERIN_F_CAM_WOLF)
                        StartCamera(cameraSetup->rot[0], cameraSetup->rot[1], cameraSetup->rot[2], &cameraSetup->eye,
                                    cameraSetup->focal, CAMSCR_BLEND_IN | CAMSCR_BLEND_OUT, cameraDuration);
                    else
                        StartCamera(cameraSetup->rot[0], cameraSetup->rot[1], cameraSetup->rot[2], &cameraSetup->eye,
                                    cameraSetup->focal, 0, 0);
                } else if ((flags & GEYSERIN_F_CAM_OBJ) &&
                           (g_pWolf->HandleMessage(this, MSG_QUERY_CONTROLLED, 0) ||
                            victim->GetClassId() == CLASSID_BOX) &&
                           g_pWolf->HandleMessage(this, MSG_FREEZE, 0)) {
                    if ((g_camMode == CAM_SCRIPT_BLEND_IN || g_camMode == CAM_SCRIPT_TO_SCRIPT ||
                         g_camMode == CAM_SCRIPTED || g_camMode == CAM_SCRIPT_RETURN) == 0 ||
                        victim->GetClassId() == CLASSID_BOX)
                        StartCamera(cameraSetup->rot[0], cameraSetup->rot[1], cameraSetup->rot[2], &cameraSetup->eye,
                                    cameraSetup->focal, 0, 0);
                }
                SetState(GEYSERIN_ST_TRANSIT);
                break;
            }
            break;
        case GEYSERIN_ST_TRANSIT:
            timerMs -= g_dtMs;
            hiddenPos.x = 32000;
            hiddenPos.y = 32000;
            hiddenPos.z = 32000;
            if (victim) {
                if (victim == g_pWolf)
                    victim->SetPosition(&pos);
                else
                    victim->SetPosition(&hiddenPos);
            }
            if (timerMs <= 0) {
                if (victim) {
                    victim->SetPosition(&geyserOut->pos);
                    geyserOut->HandleMessage(this, MSG_GEYSEROUT_SET_PASSENGER, victim);
                }
                geyserOut->HandleMessage(this, MSG_GEYSEROUT_START, 0);
                SetState(GEYSERIN_ST_IDLE);
                break;
            }
            break;
        case GEYSERIN_ST_CORKED:
            timerMs -= g_dtMs;
            geyserOut->HandleMessage(this, MSG_GEYSEROUT_CORK, 0);
            count = ObjGrid_QueryBoxPoints(detectBox, nearby);
            flags &= ~GEYSERIN_F_ROCK_CORK;
            for (slot = 0; slot < count; slot++) {
                if (nearby[slot]->GetClassId() == CLASSID_ROCK) {
                    flags |= GEYSERIN_F_ROCK_CORK;
                    if (pullTimerMs == 190) {
                        pullOffset.x = pos.x - nearby[slot]->pos.x;
                        pullOffset.y = pos.y - nearby[slot]->pos.y;
                        pullOffset.z = pos.z - nearby[slot]->pos.z;
                    }
                    frameDelta.x = pullOffset.x;
                    frameDelta.y = pullOffset.y;
                    frameDelta.z = pullOffset.z;
                    frameDelta.x *= (s16)g_dtMs;
                    frameDelta.y *= (s16)g_dtMs;
                    frameDelta.z *= (s16)g_dtMs;
                    frameDelta.x /= 190;
                    frameDelta.y /= 190;
                    frameDelta.z /= 190;
                    frameDelta.y = 0;
                    victim = nearby[slot];
                    if (pullTimerMs >= 0) {
                        if (flags & GEYSERIN_F_Z_ONLY)
                            frameDelta.x = 0;
                        victim->HandleMessage(this, MSG_PUSH, &frameDelta);
                        corkPushCount++;
                        pullTimerMs -= g_dtMs;
                    } else {
                        if (g_geyserInBusy == this)
                            g_geyserInBusy = 0;
                        SetAnimState(GEYSERIN_AS_TRANSIT);
                        break;
                    }
                    break;
                }
            }
            if (~flags & GEYSERIN_F_ROCK_CORK) {
                if (timerMs <= 0) {
                    geyserOut->HandleMessage(this, MSG_GEYSEROUT_UNCORK, 0);
                    SetState(GEYSERIN_ST_ACTIVE);
                    break;
                }
                break;
            } else
                timerMs = corkPeriod;
            break;
    }
    AdvanceAnim();
}
s32 GeyserIn::HandleMessage(ScnObject *, u32 msg, void *arg)
{
    switch (msg) {
        case MSG_GEYSER_CTRL_STATE:
            if (arg)
                flags |= GEYSERIN_F_CONTROLLED_ON;
            else
                flags &= ~GEYSERIN_F_CONTROLLED_ON;
            break;
        case MSG_FREEZE:
            return 1;
        case MSG_GEYSER_ZONE_ACTIVE:
            if (arg)
                SetUpdateMode(SCN_UPD_ALWAYS);
            else
                SetUpdateMode((u8)SCN_UPD_NORMAL);
            break;
    }
    return 0;
}
void GeyserIn::SetState(u8 value)
{
    Vec3s victimPos;
    switch (value) {
        case GEYSERIN_ST_IDLE:
            timerMs = idlePeriod;
            SetAnimState(GEYSERIN_AS_IDLE);
            flags &= ~GEYSERIN_F_ROCK_CORK;
            victim = 0;
            if (g_geyserInBusy == this)
                g_geyserInBusy = 0;
            /* cast kept: HandleMessage's arg is a void *; this message passes a number in it */
            if (geyserController)
                geyserController->HandleMessage(this, MSG_GEYSER_CTRL_STATE, (void *)1);
            break;
        case GEYSERIN_ST_ACTIVE:
            timerMs = activePeriod;
            SetAnimState(GEYSERIN_AS_ACTIVE);
            if (geyserController)
                geyserController->HandleMessage(this, MSG_GEYSER_CTRL_STATE, 0);
            break;
        case GEYSERIN_ST_SUCK:
            /* cast kept: HandleMessage's arg is a void *; this message passes a number in it */
            timerMs = victim->HandleMessage(this, MSG_GEYSER_IN, (void *)1);
            if (timerMs == 0 && victim->GetClassId() == CLASSID_SHEEP)
                return;
            g_geyserInBusy = this;
            SetAnimState(GEYSERIN_AS_SUCK);
            pullOffset.x = pos.x;
            pullOffset.y = pos.y;
            pullOffset.z = pos.z;
            victimPos.x = victim->pos.x;
            victimPos.y = victim->pos.y;
            victimPos.z = victim->pos.z;
            victimPos.y -= 5;
            pullOffset.x -= victimPos.x;
            pullOffset.y -= victimPos.y;
            pullOffset.z -= victimPos.z;
            pullTimerMs = 840;
            break;
        case GEYSERIN_ST_TRANSIT:
            SetAnimState(GEYSERIN_AS_TRANSIT);
            if ((victim->GetClassId() == CLASSID_WOLF && (flags & GEYSERIN_F_CAM_WOLF)) ||
                ((victim->GetClassId() == CLASSID_SHEEP || victim->GetClassId() == CLASSID_SALAD) &&
                 (flags & GEYSERIN_F_CAM_OBJ)))
                timerMs = transitDelayMs;
            else
                timerMs = 0;
            g_geyserInBusy = 0;
            break;
        case GEYSERIN_ST_CORKED:
            timerMs = corkPeriod;
            corkPushCount = 0;
            pullTimerMs = 190;
            SetAnimState(GEYSERIN_AS_CORKED);
            /* cast kept: HandleMessage's arg is a void *; this message passes a number in it */
            if (geyserController)
                geyserController->HandleMessage(this, MSG_GEYSER_CTRL_STATE, (void *)1);
            break;
    }
    state = value;
}
void GeyserIn::SetAnimState(u8 value)
{
    if (value != animState) {
        StopSound(soundHandle);
        switch (value) {
            case GEYSERIN_AS_IDLE:
                PlayAnim(ATORNA02_ANIM_STAND0, 0, 0);
                break;
            case GEYSERIN_AS_ACTIVE:
                PlayAnim(ATORNA02_ANIM_TORNA3, 0, 0);
                if (Vec3s_DistSq(&pos, &g_camPos) < 6250000)
                    soundHandle = Sound_Play(SND_GEYSER, this, 255, SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 4096);
                break;
            case GEYSERIN_AS_CORKED:
                PlayAnim(ATORNA02_ANIM_TOSTAND1, 1, 0);
                break;
            case GEYSERIN_AS_TRANSIT:
                if (animState != GEYSERIN_AS_SUCK)
                    PlayAnim(ATORNA02_ANIM_TORNA4A, 0, 0);
                break;
            case GEYSERIN_AS_SUCK:
                PlayAnim(ATORNA02_ANIM_TORNA4, 0, 0);
                break;
        }
        animState = value;
    }
}
ScnObject *GeyserIn_Create(void *record)
{
    GeyserIn *object = new GeyserIn;
    object = (GeyserIn *)object->Init(record, 0); /* cast kept: Init returns the ScnObject base */
    return object;
}
