/* PAL PC 0x4c9af0-0x4c9d3d. The inline helpers are written whole, with the branches this file never takes. */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
#define SDW_MEMBERS_ScnObject       \
    static void *operator new(u32); \
    void SetUpdateMode(s32 mode);
#include "sdw_classes.h"
#include "../engine/scn_tools.h"
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_SETMOVEMENTENABLED_S32 1
#define SDW_INLINE_SCNOBJECT_SETVISIBLE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_SETMOVEMENTENABLED_S32
#undef SDW_INLINE_SCNOBJECT_SETVISIBLE_S32
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32
#define SDW_INLINE_FREE_PROPU32_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_PROPU32_VOID_U32
ScnObject *HitSwitch_Create(void *record)
{
    HitSwitch *object = new HitSwitch;
    object = (HitSwitch *)object->Init(record); /* cast kept: Init returns the object as a ScnObject * */
    return object;
}
void HitSwitch::PostLoadInit()
{
    void *properties = record;
    target = Scenaric_FindByIdList((u16)PropU32(properties, 0));
    SetMovementEnabled(1);
    SetVisible(0);
    SetUpdateMode(SCN_UPD_NEVER);
}
void HitSwitch::Reset()
{
    SetMovementEnabled(1);
}
s32 HitSwitch::HandleMessage(ScnObject *sender, u32 message, void *arg)
{
    switch (message) {
        case MSG_BUMP:
            if (sender->GetClassId() == CLASSID_BULL)
                target->HandleMessage(this, MSG_HITSWITCH_BULL, sender);
            return 1;
    }
    return 0;
}
