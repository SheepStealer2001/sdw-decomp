/* PAL PC SwirlSign, 0x4f8cd0-0x4f8fbf. */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *, Animator *, u16, u32);

#define SDW_MEMBERS_ScnObject static void *operator new(u32);


#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
extern Wolf *g_pWolf;
#define SDW_INLINE_FREE_PROPU32_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_PROPU32_VOID_U32
void SwirlSign::PostLoadInit()
{
    void *a = record;
    if (PropU32(a, 0))
        SetState(SWIRLSIGN_ST_FACE_B);
    else
        SetState(SWIRLSIGN_ST_FACE_A);
}
void SwirlSign::Reset() {}
void SwirlSign::Update()
{
    switch (state) {
        case SWIRLSIGN_ST_TURN_TO_A:
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(SWIRLSIGN_ST_FACE_A);
            break;
        case SWIRLSIGN_ST_TURN_TO_B:
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(SWIRLSIGN_ST_FACE_B);
            break;
    }
    AdvanceAnim();
}
s32 SwirlSign::HandleMessage(ScnObject *sender, u32 message, void *)
{
    switch (message) {
        case MSG_USE:
            if (sender != g_pWolf)
                break;
        case MSG_SWIRLSIGN_TURN:
            if (state == SWIRLSIGN_ST_FACE_B)
                SetState(SWIRLSIGN_ST_TURN_TO_A);
            if (state == SWIRLSIGN_ST_FACE_A)
                SetState(SWIRLSIGN_ST_TURN_TO_B);
            break;
        case MSG_SWIRLSIGN_IS_B:
            if (state == SWIRLSIGN_ST_FACE_B)
                return 1;
            break;
        case MSG_SWIRLSIGN_IS_A:
            if (state == SWIRLSIGN_ST_FACE_A)
                return 1;
            break;
        case MSG_QUERY_ACTION:
            if (sender->GetClassId() == CLASSID_WOLF && (state == SWIRLSIGN_ST_FACE_A || state == SWIRLSIGN_ST_FACE_B))
                return CTX_SWIRLSIGN;
            break;
    }
    return 0;
}
void SwirlSign::SetState(u8 next)
{
    switch (next) {
        case SWIRLSIGN_ST_FACE_A:
            PlayAnim(APANCH01_ANIM_STAND2, 0, 0);
            break;
        case SWIRLSIGN_ST_FACE_B:
            PlayAnim(APANCH01_ANIM_STAND1, 0, 0);
            break;
        case SWIRLSIGN_ST_TURN_TO_B:
            PlayAnim(APANCH01_ANIM_TURN1, 0, 0);
            break;
        case SWIRLSIGN_ST_TURN_TO_A:
            PlayAnim(APANCH01_ANIM_TURN2, 0, 0);
            break;
    }
    state = next;
}
ScnObject *SwirlSign_Create(void *record)
{
    ScnBody *object = new SwirlSign;
    object = object->Init(record, 0);
    return object;
}
