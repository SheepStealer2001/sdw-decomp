/* PAL PC SensibleButton, 0x4efaf0-0x4f06b0. */

#define SDW_MEMBERS_ScnObject            \
    static void *operator new(u32 size); \
    CollBox *GetFirstModelBox();         \
    void SetUpdateMode(u8 mode);


#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *inst, Animator *animator, u16 id, u32 opts);
#include "../engine/sound_mgr.h"
#include "../engine/scenaric.h"
#include "../engine/id_list.h"
#include "../engine/scn_tools.h"
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_INSTFLAGS_U16 1
#define SDW_INLINE_SCNOBJECT_SETCOLLIDABLE_S32 1
#define SDW_INLINE_SCNOBJECT_SETVISIBLE_S32 1
#define SDW_INLINE_SCNOBJECT_STOPSOUND_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_INSTFLAGS_U16
#undef SDW_INLINE_SCNOBJECT_SETCOLLIDABLE_S32
#undef SDW_INLINE_SCNOBJECT_SETVISIBLE_S32
#undef SDW_INLINE_SCNOBJECT_STOPSOUND_U16
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32

s32 Scenaric_FindByClass(u16 id, ScnObject **out, s32 max);
s32 ObjGrid_QueryBoxOverlap(CollBox *box, ScnObject **out);
u16 Sound_Play(u16 id, void *owner, u16 volume, u8 flags, s32 rate);


#define SDW_INLINE_FREE_SCN_GETPROPU32_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_SCN_GETPROPU32_VOID_U32

#define SDW_INLINE_FREE_SCENARIC_CLASSFLAGS_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_FREE_SCENARIC_CLASSFLAGS_U16

inline CollBox *ScnObject::GetFirstModelBox()
{
    ModelBoxList *list = inst_model->boxes;
    if (list)
        return list->boxes;
    return 0;
}

#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8

/* 0x4efaf0 */
void SensibleButton::PostLoadInit()
{
    void *rec = record;
    for (u16 k = 0; k < 8; k++) {
        targets2[k] = 0;
        targets[k] = 0;
    }
    u32 ids = 0;
    u32 *objectList = 0;
    targetCount = 0;
    target2Count = 0;
    ids = Scn_GetPropU32(rec, 0);
    if (ids)
        objectList = Scn_FindIdList((u16)ids, &targetCount);
    for (u16 i = 0; i < targetCount; i++)
        /* cast kept: an id list holds untyped record pointers; the caller knows the kind it asked for */
        targets[i] = Scenaric_FindByRecord((void *)objectList[i]);
    ids = Scn_GetPropU32(rec, 4);
    if (ids)
        objectList = Scn_FindIdList((u16)ids, &target2Count);
    for (u16 j = 0; j < target2Count; j++)
        /* cast kept: an id list holds untyped record pointers; the caller knows the kind it asked for */
        targets2[j] = Scenaric_FindByRecord((void *)objectList[j]);
    type = (u8)Scn_GetPropU32(rec, 8);
    if (targetCount || target2Count)
        locked = 0;
    else
        locked = 1;
    if (type == SBUTTON_TYPE_GROUNDED || type == SBUTTON_TYPE_NO_SHEEP)
        SnapToGround(0);
    else
        SnapToGround(1);
    buttonFlags.enabled = 1;
    pressBox = GetFirstModelBox();
    SetUpdateMode(SCN_UPD_ALWAYS);
    SetCollidable(0);
    PlayAnim(ABOUTO01_ANIM_OFF, 1, 0);
    homePos = pos;
    StopSound(clickSound);
    clickSound = 0;
    buttonFlags.pressed = 0;
    if (type == SBUTTON_TYPE_HIDDEN) {
        buttonFlags.enabled = 0;
        SetVisible(0);
    }
}

/* 0x4efedd */
void SensibleButton::Update()
{
    for (s32 i = 0; i < 4; i++)
        occupants[i] = 0;
    SetCollidable(0);
    if (!buttonFlags.enabled)
        return;
    switch (locked) {
        case 0: {
            u8 taken = 0;
            s32 count;
            ScnObject *targets[64];
            CollBox box;
            box.Box_Translate(pressBox, &pos);
            count = ObjGrid_QueryBoxOverlap(&box, targets);
            for (s32 index = 0; index < count; index++) {
                if (targets[index]->GetFirstSolidBox() && targets[index] != this &&
                    !(Scenaric_ClassFlags(targets[index]->GetClassId()) & SCN_CF_IGNORED_BY_BUTTONS) &&
                    !targets[index]->InstFlags(INST_F_ATTACHED) && taken < 4) {
                    occupants[taken] = targets[index];
                    taken++;
                }
            }
            if (taken == 0) {
                if (buttonFlags.pressed) {
                    StopSound(clickSound);
                    clickSound = Sound_Play(SND_SBONMBTN, this, 0x7f, SNDF_POSITIONAL, 0x1000);
                }
                buttonFlags.pressed = 0;
                for (u16 a = 0; a < targetCount; a++)
                    this->targets[a]->HandleMessage(this, MSG_SWITCH_OFF, 0);
                for (u16 b = 0; b < target2Count; b++)
                    targets2[b]->HandleMessage(this, MSG_SWITCH_OFF, 0);
                PlayAnim(ABOUTO01_ANIM_OFF, 0, 1);
            } else {
                if (!buttonFlags.pressed) {
                    StopSound(clickSound);
                    clickSound = Sound_Play(SND_SBONMBTN, this, 0x7f, SNDF_POSITIONAL, 0x1000);
                }
                buttonFlags.pressed = 1;
                for (u16 c = 0; c < targetCount; c++)
                    this->targets[c]->HandleMessage(this, MSG_SWITCH_ON, occupants);
                for (u16 d = 0; d < target2Count; d++)
                    targets2[d]->HandleMessage(this, MSG_SWITCH_ON, occupants);
                PlayAnim(ABOUTO01_ANIM_ON, 0, 1);
            }
        } break;
    }
    AdvanceAnim();
}

/* 0x4f044c */
s32 SensibleButton::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    if (sender) {
        switch (msgId) {
            case MSG_QUERY_ACTION:
                if (type == SBUTTON_TYPE_NO_SHEEP)
                    return CTX_NONE;
                return SHEEP_ATTR_BUTTON;
            case MSG_TELEPORTED:
                return 1;
            case MSG_BUTTON_LOCK:
                /* cast kept: the message arg is a void *: what it carries depends on the message id */
                if (arg == (void *)1) {
                    locked = 1;
                    PlayAnim(ABOUTO01_ANIM_ON, 1, 0);
                    return 1;
                }
                PlayAnim(ABOUTO01_ANIM_OFF, 1, 0);
                locked = 0;
                return 1;
            case MSG_BUTTON_REVEAL:
                if (!buttonFlags.enabled) {
                    buttonFlags.enabled = 1;
                    SnapToGround(1);
                    SetPosition(&homePos);
                    SetVisible(1);
                }
                return 1;
        }
    }
    return 0;
}

/* 0x4f05e6 */
void SensibleButton::Reset()
{
    ScnObject *train;
    StopSound(clickSound);
    clickSound = 0;
    if (Scenaric_FindByClass(CLASSID_TRAIN, &train, 1))
        return;
    SetPosition(&homePos);
}

/* 0x4f0649 */
ScnObject *SensibleButton_Create(void *record)
{
    ScnBody *obj = new SensibleButton;
    obj = obj->Init(record, 0);
    return obj;
}
