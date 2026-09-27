/* PAL PC Case 0x4a88c0-0x4a8a34. */
#define SDW_MEMBERS_ScnObject            \
    static void *operator new(u32 size); \
    void SetUpdateMode(s32 mode);
#include "sdw_enums.h"
#include "sdw_classes.h"
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32
s32 Box_GroundQueryFlatTop(GroundQuery *, CollBox *, Vec3s *, s32);
ScnObject *Case_Create(void *record)
{
    ScnBody *object = new Case;
    object = object->Init(record, 0);
    return object;
}
void Case::PostLoadInit()
{
    SetUpdateMode(SCN_UPD_NEVER);
    SnapToGround(1);
}
s32 Case::HandleMessage(ScnObject *sender, u32 message, void *arg)
{
    switch (message) {
        case MSG_GROUND_QUERY:
            /* cast kept: MSG_GROUND_QUERY's arg is a GroundQuery */
            return Box_GroundQueryFlatTop((GroundQuery *)arg, GetFirstSolidBox(), &pos, 0);
    }
    return 0;
}
