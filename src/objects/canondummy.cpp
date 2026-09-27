/* PAL PC CanonDummy 0x4a5710-0x4a5b19, a ScnBody. */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *, Animator *, u16, u32);

#define SDW_MEMBERS_ScnObject       \
    static void *operator new(u32); \
    void SetUpdateMode(s32 mode);

#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
void CanonDummy::PostLoadInit()
{
    ownedBySheepCannon = 0;
    SetUpdateMode(SCN_UPD_ALWAYS);
    SetState(CD_ST_REST);
}
void CanonDummy::Reset()
{
    SetState(CD_ST_REST);
}
void CanonDummy::Update()
{
    switch (state) {
        case CD_ST_MOVE_OUT:
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(CD_ST_AT_POS);
            break;
        case CD_ST_AT_POS:
            if (AnimFlags(ANIM_F_FINISHED))
                owner->HandleMessage(this, MSG_CANONSHEEP_DUMMY_READY, 0);
            break;
    }
    AdvanceAnim();
}
s32 CanonDummy::HandleMessage(ScnObject *sender, u32 msg, void *arg)
{
    if (!sender)
        return 0;
    if (sender->GetClassId() == CLASSID_CANONSHEEP) {
        owner = sender;
        ownedBySheepCannon = 1;
    }
    switch (msg) {
        case MSG_CD_RECOIL:
            PlayAnim(ACANON01_ANIM_SHOOT, 0, 0);
            break;
        case MSG_CD_MOVE_TO:
            /* cast kept (these three lines): MSG_CD_MOVE_TO's arg is a Vec3s, the position to move to */
            movePos.x = ((Vec3s *)arg)->x;
            movePos.y = ((Vec3s *)arg)->y;
            movePos.z = ((Vec3s *)arg)->z;
            SetState(CD_ST_MOVE_OUT);
            break;
    }
    return 0;
}
void CanonDummy::SetState(u8 next)
{
    state = next;
    switch (state) {
        case CD_ST_AT_POS:
            SetPosition(&movePos);
            PlayAnim(ACANON01_ANIM_APPEAR, 0, 0);
            break;
        case CD_ST_REST:
            PlayAnim(ACANON01_ANIM_STAND, 0, 0);
            break;
        case CD_ST_MOVE_OUT:
            PlayAnim(ACANON01_ANIM_OUT, 0, 0);
            break;
    }
}
void CanonDummy::Render(Camera *view)
{
    Vec3s scale;
    if (ownedBySheepCannon) {
        scale.x = 0x800;
        scale.y = 0x800;
        scale.z = 0x800;
    } else {
        scale.x = 0x400;
        scale.y = 0x400;
        scale.z = 0x400;
    }
    RenderScaled(view, &scale);
}
ScnObject *CanonDummy_Create(void *record)
{
    CanonDummy *object = new CanonDummy;
    object = (CanonDummy *)object->Init(record, 0); /* cast kept: Init returns the object as a ScnBody * */
    return object;
}
