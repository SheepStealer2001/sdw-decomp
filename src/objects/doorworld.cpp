/* PAL PC 0x4b43a0-0x4b4e65. */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class ScnObject;
class Camera;
class Instance;
struct Animator;
u32 Anim_Start(Instance *instance, Animator *animator, u16 id, u32 flags);
void Camera_StartScripted(ScnObject *owner, Camera *camera, u16 rx, u16 ry, u16 rz, Vec3s *eye, u16 focal, u32 mode,
                          s32 time);
#include "../app/app_main.h"
#include "../engine/scn_tools.h"
#include "../engine/collide.h"

#define SDW_MEMBERS_ScnObject                                   \
    static void *operator new(u32 size);                        \
    s32 CanBlockDoorCollision()                                 \
    {                                                           \
        return (flags & (SCN_OF_HIDDEN2 | SCN_OF_HIDDEN)) == 0; \
    }                                                           \
    void GetModelBoxes(CollBox **out, u32 *count);              \
    void SetUpdateMode(u8 mode);                                \
    void StartCamera(u16 rx, u16 ry, u16 rz, Vec3s *eye, u16 focal, u32 mode);

#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_SETVISIBLE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_SETVISIBLE_S32
#define SDW_INLINE_SCNOBJECT_SETMOVEMENTENABLED_S32 1
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8 1
#define SDW_INLINE_SCNOBJECT_STARTCAMERA_U16_U16_U16_VEC3S_U16_U32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETMOVEMENTENABLED_S32
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8
#undef SDW_INLINE_SCNOBJECT_STARTCAMERA_U16_U16_U16_VEC3S_U16_U32
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
extern s32 g_dtMs;
ScnObject *g_pDoorOpenRequester; /* T137 .bss 0x6cf614 */
#include "../engine/collide.h"
#define SDW_INLINE_SCNOBJECT_GETMODELBOXES_COLLBOX_U32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETMODELBOXES_COLLBOX_U32
inline CamSetup *PropertyCamera(void *record, s32 offset)
{
    return Scn_GetPropCamera(record, offset);
}
inline ScnObject *PropertyObject(void *record, s32 offset)
{
    return Scn_GetPropObject(record, offset);
}

void DoorWorld::PostLoadInit()
{
    u32 count;
    CollBox *boxes;
    openCamera = PropertyCamera(record, 0);
    doorLevel = PropertyObject(record, 4);
    Reset();
    GetModelBoxes(&boxes, &count);
    if (rot.y)
        triggerBox = boxes + 1;
    else
        triggerBox = boxes;
}

void DoorWorld::Update()
{
    s32 doorReply;
    switch (state) {
        case DOORWORLD_ST_INIT:
            doorReply = doorLevel->HandleMessage(this, MSG_DOORLEVEL_QUERY, 0);
            if (doorReply == DOORLEVEL_NOT_DONE)
                state = DOORWORLD_ST_CLOSE;
            else {
                SetUpdateMode(SCN_UPD_NEVER);
                SetVisible(0);
                state = DOORWORLD_ST_IDLE;
            }
            timer = 0;
            break;
        case DOORWORLD_ST_OPEN_CAMERA:
            if (openCamera && useOpenSequence)
                StartCamera(openCamera->rot[0], openCamera->rot[1], openCamera->rot[2], &openCamera->eye,
                            openCamera->focal, 0);
            state = DOORWORLD_ST_OPEN_DELAY;
            break;
        case DOORWORLD_ST_OPEN_DELAY:
            timer += (s16)g_dtMs;
            if (timer > 1000) {
                PlayAnim(APORTE03_ANIM_OPEN1, 0, 1);
                state = DOORWORLD_ST_OPENING;
            }
            break;
        case DOORWORLD_ST_OPENING:
            if ((useOpenSequence && AnimFlags(ANIM_F_FINISHED)) || !useOpenSequence) {
                SetVisible(0);
                timer = 0;
                state = DOORWORLD_ST_OPENED;
            }
            break;
        case DOORWORLD_ST_OPENED:
            timer += (s16)g_dtMs;
            if (timer > 1000) {
                if (g_pDoorOpenRequester) {
                    g_pDoorOpenRequester->HandleMessage(this, MSG_DOOR_SEQUENCE_DONE, 0);
                    g_pDoorOpenRequester = 0;
                }
                SetUpdateMode(SCN_UPD_NEVER);
                useOpenSequence = 0;
            }
            break;
        case DOORWORLD_ST_CLOSE:
            SetUpdateMode(SCN_UPD_NORMAL);
            PlayAnim(APORTE03_ANIM_CLOSE1, 0, 1);
            SetVisible(1);
            state = DOORWORLD_ST_IDLE;
            break;
    }
    AdvanceAnim();
}

s32 DoorWorld::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    s32 doorReply;
    switch (msgId) {
        case MSG_DOORWORLD_OPEN:
            doorReply = doorLevel->HandleMessage(this, MSG_DOORLEVEL_QUERY, 0);
            switch (doorReply) {
                case DOORLEVEL_NOT_DONE:
                    state = DOORWORLD_ST_CLOSE;
                    break;
                case DOORLEVEL_DONE:
                    SetUpdateMode(SCN_UPD_NEVER);
                    SetVisible(0);
                    state = DOORWORLD_ST_OPENING;
                    break;
                case DOORLEVEL_CURRENT:
                    SetUpdateMode(SCN_UPD_ALWAYS);
                    SetVisible(1);
                    if (sender->GetClassId() == CLASSID_SCENESHEEPPANEL)
                        g_pDoorOpenRequester = sender;
                    if (openCamera)
                        useOpenSequence = 1;
                    state = DOORWORLD_ST_OPEN_CAMERA;
                    return 1;
            }
            break;
    }
    return 0;
}

void DoorWorld::Reset()
{
    SetUpdateMode(SCN_UPD_NORMAL);
    PlayAnim(APORTE03_ANIM_CLOSE1, 0, 0);
    SetVisible(1);
    SetMovementEnabled(0);
    g_pDoorOpenRequester = 0;
    useOpenSequence = 0;
    state = DOORWORLD_ST_INIT;
}

s32 DoorWorld::CustomCollide(ScnObject *querier, CollBox *mover, Vec3s *disp, s32 *outFrac, s32 *outY,
                             CollContact *contacts, s32 *nContacts, u32 mode)
{
    if (CanBlockDoorCollision())
        return Collide_BoxVsObjBox(this, mover, disp, triggerBox, &pos, outFrac, outY, contacts, nContacts);
    return 0;
}
ScnObject *DoorWorld_Create(void *record)
{
    DoorWorld *object = new DoorWorld;
    object = (DoorWorld *)object->Init(record, 0); /* cast kept: Init returns the ScnBody * base of this object */
    object->flags |= SCN_OF_CUSTOM_COLLIDE;
    return object;
}
