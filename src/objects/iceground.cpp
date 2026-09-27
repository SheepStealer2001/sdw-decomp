/* PAL PC IceGround, 0x4cc150-0x4cc5a7. */
#include "sdw_types.h"
#include "sdw_enums.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *, Animator *, u16, u32);

#define SDW_MEMBERS_ScnObject                           \
    static void *operator new(u32);                     \
    CollBox *GetFirstModelBox();                        \
    void StartCamera(u16, u16, u16, Vec3s *, u16, u32); \
    void SetUpdateMode(s32 mode);

#include "sdw_classes.h"
#include "../engine/scn_tools.h"
#include "camera.h"
#include "../app/app_main.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_SETCONTACTENABLED_S32 1
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETCONTACTENABLED_S32
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
void Camera_StartScripted(ScnObject *, Camera *, u16, u16, u16, Vec3s *, u16, u32, s32);
s32 Box_GroundQueryFlatTop(GroundQuery *, CollBox *, Vec3s *, s32);
#define SDW_INLINE_SCNOBJECT_STARTCAMERA_U16_U16_U16_VEC3S_U16_U32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_STARTCAMERA_U16_U16_U16_VEC3S_U16_U32
inline CollBox *ScnObject::GetFirstModelBox()
{
    ModelBoxList *list = inst_model->boxes;
    if (list)
        return list->boxes;
    return 0;
}
void IceGround::PostLoadInit()
{
    void *props = record;
    camera = Scn_GetPropCamera(props, 0);
    PlayAnim(ABOCGL02_ANIM_STAND0, 1, 0);
    state = ICEGROUND_ST_INTACT;
    broken = 0;
    SetContactEnabled(1);
}
void IceGround::Update()
{
    switch (state) {
        case ICEGROUND_ST_BREAK:
            if (camera)
                StartCamera(camera->rot[0], camera->rot[1], camera->rot[2], &camera->eye, camera->focal,
                            CAMSCR_BLEND_OUT);
            PlayAnim(ABOCGL02_ANIM_EXPLOD1, 0, 0);
            state = ICEGROUND_ST_BROKEN_ANIM;
            SetContactEnabled(0);
            break;
        case ICEGROUND_ST_BROKEN_ANIM:
            if (AnimFlags(ANIM_F_FINISHED)) {
                broken = 1;
                state = ICEGROUND_ST_INTACT;
                Camera_ReleaseScripted(this);
            }
            break;
    }
    AdvanceAnim();
}
s32 IceGround::HandleMessage(ScnObject *, u32 msg, void *arg)
{
    if (!broken) {
        switch (msg) {
            case MSG_KILL:
                if (!arg) {
                    state = ICEGROUND_ST_BREAK;
                    SetUpdateMode(SCN_UPD_ALWAYS);
                }
                return 1;
            case MSG_GROUND_QUERY:
                CollBox *box = GetFirstModelBox();
                if (box) {
                    if (broken)
                        return 0;
                    return Box_GroundQueryFlatTop((GroundQuery *)arg, box, &pos, 0); /* cast kept: arg is the query */
                }
                break;
        }
    }
    return 0;
}
void IceGround::Reset()
{
    broken = 0;
    PlayAnim(ABOCGL02_ANIM_STAND0, 1, 0);
    SetContactEnabled(1);
}
ScnObject *IceGround_Create(void *record)
{
    IceGround *object = new IceGround;
    object = (IceGround *)object->Init(record, 0); /* cast kept: Init returns the object it was called on */
    return object;
}
