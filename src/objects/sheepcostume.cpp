/* PAL PC: SheepCostume, 0x4f0da0-0x4f0f91. */
#define SDW_MEMBERS_ScnObject            \
    static void *operator new(u32 size); \
    void SetUpdateMode(s32 mode);
#include "sdw_enums.h"
#include "scenaric_props.h"
#include "sdw_classes.h"
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_ISINWORLD 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_ISINWORLD

#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32

/* 0x4f0da0 */
s32 SheepCostume::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    switch (msgId) {
        case MSG_QUERY_ACTION:
            if (sender->GetClassId() == CLASSID_WOLF)
                return CTX_PICKUP;
            break;
        case MSG_CONTAINER_STATE:
            switch ((s32)arg) { /* cast kept: arg carries a number */
                case CONTAINER_RELEASED:
                    homePos = pos;
                    break;
            }
            return 1;
        case MSG_CHECKPOINT_ROLLBACK:
            SetPosition(&homePos);
            return 1;
    }
    return 0;
}

/* 0x4f0e2d */
void SheepCostume::PostLoadInit()
{
    if (IsInWorld())
        SnapToGround(1);
    homePos = pos;
    SetUpdateMode(SCN_UPD_NEVER);
}

/* 0x4f0f2d */
ScnObject *SheepCostume_Create(void *record)
{
    ScnBody *obj = new SheepCostume;
    obj = obj->Init(record, 0);
    return obj;
}
