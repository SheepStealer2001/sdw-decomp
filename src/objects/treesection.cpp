/* PAL PC TreeSection, 0x501fb0-0x5022c1. */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *, Animator *, u16, u32);
#define SDW_INLINE_FREE_INSTFLAGS_SET_U16_U16 1
#include "instance_inlines.h"
#undef SDW_INLINE_FREE_INSTFLAGS_SET_U16_U16
#define SDW_INLINE_FREE_INSTFLAGS_CLEAR_U16_U16 1
#include "instance_inlines.h"
#undef SDW_INLINE_FREE_INSTFLAGS_CLEAR_U16_U16

#define SDW_MEMBERS_ScnObject       \
    static void *operator new(u32); \
    void SetTint(u32 color, s16 amount, s32 on);

#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#define SDW_INLINE_SCNOBJECT_ENABLEBOXCOLLIDE_S32 1
#define SDW_INLINE_SCNOBJECT_SETTINT_U32_S16_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_ENABLEBOXCOLLIDE_S32
#undef SDW_INLINE_SCNOBJECT_SETTINT_U32_S16_S32
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
void TreeSection::PostLoadInit()
{
    SnapToGround(0);
    SetState(TREESECTION_ST_STANDING);
}
void TreeSection::Update()
{
    switch (state) {
        case TREESECTION_ST_CUT:
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(TREESECTION_ST_FALL);
            break;
        case TREESECTION_ST_FALL:
            if (AnimFlags(ANIM_F_FINISHED)) {
                EnableBoxCollide(0);
                SetState(TREESECTION_ST_FALLEN);
            }
            break;
    }
    AdvanceAnim();
}
s32 TreeSection::HandleMessage(ScnObject *sender, u32 msgId, void *)
{
    if (sender->GetClassId() == CLASSID_ELMER) {
        if (msgId == MSG_TREESECTION_CUT)
            SetState(TREESECTION_ST_CUT);
    } else if ((sender->GetClassId() == CLASSID_GROUNDMINE || sender->GetClassId() == CLASSID_DEFUSABLEMINE) &&
               msgId == MSG_KILL)
        SetTint(0, 4096, 1);
    return 0;
}
void TreeSection::SetState(u8 value)
{
    switch (value) {
        case TREESECTION_ST_STANDING:
            PlayAnim(ATELMER1_ANIM_STAND, 1, 1);
            break;
        case TREESECTION_ST_CUT:
            PlayAnim(ATELMER1_ANIM_APPEAR, 0, 0);
            break;
        case TREESECTION_ST_FALL:
            SwapModel(&altModel);
            PlayAnim(ATELMER1_ANIM_STAND1, 0, 0);
            break;
    }
    state = value;
}
ScnObject *TreeSection_Create(void *record)
{
    TreeSection *object = new TreeSection;
    u16 modelId = WAR_IDO_ATELMER2;
    /* cast kept: InitWithAltModels returns the object as its ScnObject base */
    object = (TreeSection *)object->InitWithAltModels(record, &object->mainModel, 1, &modelId, &object->altModel);
    return object;
}
