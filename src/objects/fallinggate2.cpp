/* PAL PC FallingGate2, 0x4ba2b0-0x4bb070. */
/* BYTES: view. */
/* BYTES(view): view: fields in the class padding (data/structs/fallinggate2.csv) (the access widths are the original's) */
#include "sdw_types.h"
#include "sdw_enums.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *, Animator *, u16, u32);

#define SDW_MEMBERS_ScnObject                      \
    static void *operator new(u32);                \
    CollBox *GetFirstModelBox();                   \
    void StartCamera(u16, u16, u16, Vec3s *, u16); \
    void SetUpdateMode(s32 mode);

#define SDW_MEMBERS_FallingGate2 u32 Property(u32);
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_SETCONTACTENABLED_S32 1
#define SDW_INLINE_SCNOBJECT_SETVISIBLE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETCONTACTENABLED_S32
#undef SDW_INLINE_SCNOBJECT_SETVISIBLE_S32
#define SDW_INLINE_SCNOBJECT_SETFLAG40_S32 1
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETFLAG40_S32
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
/* Fields (data/structs/FallingGate2.csv): state +64, rise timer +68;
 * animation durations +70/+74; closed +78, busy +7c, one-shot +80,
 * freeze acknowledgement +84, shared-box-raised latch +88, first sender
 * switch state +8c, single-button +90, camera +94; world box +9c,
 * first sender +ac, cached scenario record +b0. */
extern Wolf *g_pWolf;
#include "../app/app_main.h"
#include "../engine/scn_tools.h"
#include "animation.h"
#include "../engine/scenaric.h"
#include "camera.h"
extern s32 g_dtMs;
void Camera_StartScripted(ScnObject *, Camera *, u16, u16, u16, Vec3s *, u16, u32, s32);
#define SDW_INLINE_SCNOBJECT_STARTCAMERA_U16_U16_U16_VEC3S_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_STARTCAMERA_U16_U16_U16_VEC3S_U16
inline CollBox *ScnObject::GetFirstModelBox()
{
    ModelBoxList *list = inst_model->boxes;
    if (list)
        return list->boxes;
    return 0;
}
inline u32 FallingGate2::Property(u32 offset)
{
    /* cast kept (both): designer properties are 4-byte slots at byte offsets of the raw WAR record */
    return *(u32 *)((u8 *)storedRecord + offset + 0x14);
}
void FallingGate2::PostLoadInit()
{
    u16 *props = record;
    storedRecord = props;
    singleButton = Property(0) == 0;
    closed = Property(8);
    oneShot = Property(16);
    fallingTimeMs = Property(12);
    camera = Scn_GetPropCamera(storedRecord, 4);
    solidBox = GetFirstModelBox();
    worldBox.min.x = pos.x + solidBox->min.x;
    worldBox.min.y = pos.y + solidBox->min.y;
    worldBox.min.z = pos.z + solidBox->min.z;
    worldBox.max.x = pos.x + solidBox->max.x;
    worldBox.max.y = pos.y + solidBox->max.y;
    worldBox.max.z = pos.z + solidBox->max.z;
    openAnimMs = Anim_GetDurationMs(Inst(), AGRILL05_ANIM_MV_OPEN, 0);
    riseAnimMs = Anim_GetDurationMs(Inst(), AGRILL05_ANIM_MCLOSE2A, 0);
    SetState(FGATE2_ST_IDLE);
    SetFlag40(1);
    busy = 0;
    wolfFrozen = 0;
    boxRaised = 0;
    firstSender = 0;
    SetUpdateMode(SCN_UPD_ALWAYS);
}
void FallingGate2::Reset()
{
    solidBox = GetFirstModelBox();
    worldBox.min.x = pos.x + solidBox->min.x;
    worldBox.min.y = pos.y + solidBox->min.y;
    worldBox.min.z = pos.z + solidBox->min.z;
    worldBox.max.x = pos.x + solidBox->max.x;
    worldBox.max.y = pos.y + solidBox->max.y;
    worldBox.max.z = pos.z + solidBox->max.z;
    closed = Property(8);
    boxRaised = 0;
    SetState(FGATE2_ST_IDLE);
}
void FallingGate2::Update()
{
    ScnObject *objects[64];
    switch (gateState) {
        case FGATE2_ST_OPENING:
            riseTimer -= g_dtMs;
            if (riseTimer <= 0 && !boxRaised) {
                solidBox->min.y -= 180;
                solidBox->max.y -= 180;
                boxRaised = 1;
            }
            if (AnimFlags(ANIM_F_FINISHED)) {
                closed = 0;
                if (oneShot && wolfFrozen)
                    g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0);
                SetState(FGATE2_ST_IDLE);
            }
            break;
        case FGATE2_ST_FALLING:
            if (AnimFlags(ANIM_F_FINISHED) && ObjGrid_QueryBoxesInRectXZ(worldBox.min.x, worldBox.min.z, worldBox.max.x,
                                                                         worldBox.max.z, objects) <= 1)
                SetState(FGATE2_ST_SLAM);
            break;
        case FGATE2_ST_SLAM:
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(FGATE2_ST_IDLE);
            break;
    }
    AdvanceAnim();
}
s32 FallingGate2::HandleMessage(ScnObject *sender, u32 msg, void *)
{
    switch (msg) {
        case MSG_SWITCH_ON:
            if (singleButton) {
                if (!busy && closed)
                    SetState(FGATE2_ST_OPENING);
            } else if (firstSender == 0)
                firstSender = sender;
            else if (firstSender == sender)
                firstSenderOn = 1;
            else if (firstSender != sender && firstSenderOn && !busy && closed)
                SetState(FGATE2_ST_OPENING);
            break;
        case MSG_SWITCH_OFF:
            if (singleButton) {
                Camera_ReleaseScripted(this);
                if (!busy && !closed)
                    SetState(FGATE2_ST_FALLING);
            } else {
                Camera_ReleaseScripted(this);
                if (firstSender == 0)
                    firstSender = sender;
                else {
                    if (firstSender == sender)
                        firstSenderOn = 0;
                    if (!busy && !closed)
                        SetState(FGATE2_ST_FALLING);
                }
            }
            break;
        case MSG_GATE_QUERY:
            return closed == 0;
        case MSG_FREEZE:
            wolfFrozen = 0;
            return 1;
    }
    return 0;
}
void FallingGate2::SetState(u8 next)
{
    gateState = next;
    switch (gateState) {
        case FGATE2_ST_OPENING:
            if (oneShot) {
                SetContactEnabled(0);
                wolfFrozen = g_pWolf->HandleMessage(this, MSG_FREEZE, 0);
                firstSender->HandleMessage(this, MSG_BUTTON_LOCK, 0);
            }
            if (camera)
                StartCamera(camera->rot[0], camera->rot[1], camera->rot[2], &camera->eye, camera->focal);
            riseTimer = riseAnimMs * 50 / 100;
            PlayAnim(AGRILL05_ANIM_MV_OPEN, 0, 0);
            busy = 1;
            closed = 0;
            break;
        case FGATE2_ST_FALLING:
            SetVisible(1);
            anim.speed = (u16)((openAnimMs << 12) / (u32)fallingTimeMs);
            PlayAnim(AGRILL05_ANIM_MCLOSE2A, 0, 0);
            busy = 1;
            closed = 0;
            if (!boxRaised) {
                solidBox->min.y -= 180;
                solidBox->max.y -= 180;
                boxRaised = 1;
            }
            SetContactEnabled(1);
            break;
        case FGATE2_ST_SLAM:
            SetVisible(1);
            anim.speed = 0x1000;
            PlayAnim(AGRILL05_ANIM_MCLOSE2B, 0, 0);
            if (boxRaised) {
                solidBox->min.y += 180;
                solidBox->max.y += 180;
                boxRaised = 0;
            }
            busy = 1;
            closed = 1;
            break;
        case FGATE2_ST_IDLE:
            anim.speed = 0x1000;
            busy = 0;
            if (closed) {
                SetContactEnabled(1);
                PlayAnim(AGRILL05_ANIM_CLOSED, 0, 0);
            } else {
                SetContactEnabled(0);
                SetVisible(0);
                PlayAnim(AGRILL05_ANIM_OPEN, 0, 0);
            }
            break;
    }
}
ScnObject *FallingGate2_Create(void *record)
{
    FallingGate2 *object = new FallingGate2;
    object = (FallingGate2 *)object->Init(record, 0); /* cast kept: Init returns the ScnObject base */
    return object;
}
