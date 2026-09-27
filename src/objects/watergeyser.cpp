/* PAL PC WaterGeyser, 0x506840-0x50713b. */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *, Animator *, u16, u32);
#include "../engine/sound_mgr.h"
#include "sheep.h"
#include "../app/app_main.h"
#include "../engine/id_list.h"
#include "../engine/scn_tools.h"
u16 Sound_Play(u16, void *, u16, u8, s32);

#define SDW_MEMBERS_ScnObject                                        \
    static void *operator new(u32);                                  \
    void StartCameraZeroBlendTime(u16, u16, u16, Vec3s *, u16, u32); \
    void SetUpdateMode(s32 mode);

#define SDW_MEMBERS_ZoneList void Load(u32);
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_ISSOUNDPLAYING_U16 1
#define SDW_INLINE_SCNOBJECT_STOPSOUND_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_ISSOUNDPLAYING_U16
#undef SDW_INLINE_SCNOBJECT_STOPSOUND_U16
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
extern Wolf *g_pWolf;
extern s32 g_dtMs;
void Camera_StartScripted(ScnObject *, Camera *, u16, u16, u16, Vec3s *, u16, u32, s32);
#define SDW_INLINE_ZONELIST_FINDCONTAINING_VEC3S 1
#define SDW_INLINE_ZONELIST_LOAD_U32 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_FINDCONTAINING_VEC3S
#undef SDW_INLINE_ZONELIST_LOAD_U32
inline void ScnObject::StartCameraZeroBlendTime(u16 pitch, u16 yaw, u16 roll, Vec3s *eye, u16 focal, u32 flags)
{
    Camera_StartScripted(this, &g_camera, pitch, yaw, roll, eye, focal, flags, 0);
}
#define SDW_INLINE_FREE_PROPERTYU32_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_PROPERTYU32_VOID_U32
void WaterGeyser::PostLoadInit()
{
    void *properties = record;
    detectBoxes.Load(PropertyU32(properties, 0));
    geyserOut = Scn_GetPropObject(properties, 4);
    timeInHole = PropertyU32(properties, 8);
    SetUpdateMode(SCN_UPD_ALWAYS);
    soundHandle = 0;
    SetState(WGEYSER_ST_IDLE);
}
void WaterGeyser::Update()
{
    CamSetup *cameraSetup;
    Vec3s position;
    switch (state) {
        case WGEYSER_ST_IDLE:
            if (g_pWolf->HandleMessage(this, MSG_WOLF_IS_IN_WATER_STATE, 0)) {
                hitBox = detectBoxes.FindContaining(&g_pWolf->pos);
                if (hitBox) {
                    passenger = g_pWolf;
                    SetState(WGEYSER_ST_SWALLOW);
                }
            } else {
                sheep = g_pSheepOutOfZone;
                if (sheep && sheep->HandleMessage(this, MSG_SHEEP_QUERY_IN_WATER, 0)) {
                    hitBox = detectBoxes.FindContaining(&sheep->pos);
                    if (hitBox) {
                        passenger = sheep;
                        SetState(WGEYSER_ST_SWALLOW);
                    }
                }
            }
            break;
        case WGEYSER_ST_SWALLOW:
            swallowTimerMs -= g_dtMs;
            if (swallowTimerMs <= 0)
                SetState(WGEYSER_ST_WHIRL_IN);
            break;
        case WGEYSER_ST_WHIRL_IN:
            if (AnimFlags(ANIM_F_FINISHED)) {
                position.x = passenger->pos.x;
                position.y = passenger->pos.y;
                position.z = passenger->pos.z;
                position.y = hitBox->min[1] + 200;
                SetPosition(&position);
                SetState(WGEYSER_ST_WHIRL);
            }
            break;
        case WGEYSER_ST_WHIRL:
            animTimerMs -= g_dtMs;
            if (animTimerMs <= 0) {
                if (IsSoundPlaying(soundHandle))
                    StopSound(soundHandle);
                SetState(WGEYSER_ST_WHIRL_OUT);
            }
            break;
        case WGEYSER_ST_WHIRL_OUT:
            if (AnimFlags(ANIM_F_FINISHED)) {
                if (passenger->GetClassId() == CLASSID_SHEEP) {
                    sheep = g_pSheepOutOfZone;
                    if (sheep && sheep->HandleMessage(this, MSG_SHEEP_QUERY_IN_WATER, 0))
                        SetState(WGEYSER_ST_SHEEP_OUT);
                } else {
                    passenger->SetPosition(&geyserOut->pos);
                    if (passenger->GetClassId() == CLASSID_WOLF) {
                        /* cast kept: the GeyserOut answers with its camera's address as the s32 result */
                        cameraSetup = (CamSetup *)geyserOut->HandleMessage(this, MSG_GEYSEROUT_GET_CAMERA, 0);
                        if (cameraSetup)
                            StartCameraZeroBlendTime(cameraSetup->rot[0], cameraSetup->rot[1], cameraSetup->rot[2],
                                                     &cameraSetup->eye, cameraSetup->focal, 0);
                    }
                    geyserOut->HandleMessage(this, MSG_GEYSEROUT_SET_PASSENGER, passenger);
                    geyserOut->HandleMessage(this, MSG_GEYSEROUT_START, 0);
                    SetState(WGEYSER_ST_RESET);
                }
            }
            break;
        case WGEYSER_ST_SHEEP_OUT:
            passenger->SetPosition(&geyserOut->pos);
            geyserOut->HandleMessage(this, MSG_GEYSEROUT_SET_PASSENGER, passenger);
            geyserOut->HandleMessage(this, MSG_GEYSEROUT_START, 0);
            SetState(WGEYSER_ST_RESET);
            break;
        case WGEYSER_ST_RESET:
            SetState(WGEYSER_ST_IDLE);
            break;
    }
    AdvanceAnim();
}
s32 WaterGeyser::HandleMessage(ScnObject *, u32 msgId, void *)
{
    switch (msgId) {
    }
    return 0;
}
void WaterGeyser::SetState(u8 value)
{
    state = value;
    switch (value) {
        case WGEYSER_ST_IDLE:
            PlayAnim(ATOURB01_ANIM_STAND, 1, 1);
            break;
        case WGEYSER_ST_SWALLOW:
            swallowTimerMs = 1500;
            PlayAnim(ATOURB01_ANIM_STAND, 1, 1);
            break;
        case WGEYSER_ST_WHIRL_IN:
            if (!g_pWolf->HandleMessage(this, MSG_WOLF_IS_DEAD, 0))
                PlayAnim(ATOURB01_ANIM_WHIRL, 0, 1);
            else
                SetState(WGEYSER_ST_IDLE);
            break;
        case WGEYSER_ST_WHIRL:
            soundHandle = Sound_Play(SND_WATERGEYSER_WHIRL, this, 255, SNDF_LOOP | SNDF_POSITIONAL, 4096);
            animTimerMs = passenger->HandleMessage(this, MSG_GEYSER_IN,
                                                   (void *)1); /* cast kept: a message argument is a void * */
            PlayAnim(ATOURB01_ANIM_WHIRL1, 1, 1);
            break;
        case WGEYSER_ST_WHIRL_OUT:
            PlayAnim(ATOURB01_ANIM_WHIRL2, 0, 1);
            break;
        case WGEYSER_ST_SHEEP_OUT:
            PlayAnim(ATOURB01_ANIM_WHIRL2, 0, 1);
            break;
        case WGEYSER_ST_RESET:
            PlayAnim(ATOURB01_ANIM_STAND, 1, 1);
            break;
    }
}
void WaterGeyser::Reset()
{
    SetState(WGEYSER_ST_IDLE);
}
ScnObject *WaterGeyser_Create(void *record)
{
    WaterGeyser *object = new WaterGeyser;
    object = (WaterGeyser *)object->Init(record, 0); /* cast kept: Init returns the object as its ScnBody base */
    return object;
}
