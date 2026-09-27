/* PAL PC RabbitCostume, 0x4def90-0x4df181. */
#define SDW_MEMBERS_ScnObject            \
    static void *operator new(u32 size); \
    void SetUpdateMode(u8 mode);
#include "sdw_enums.h"
#include "scenaric_props.h"
#include "sdw_classes.h"
#define SDW_INLINE_SCNOBJECT_ISINWORLD 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_ISINWORLD
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8

/* 0x4def90 */
s32 RabbitCostume::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    switch (msgId) {
        case MSG_QUERY_ACTION:
            if (sender->GetClassId() == CLASSID_WOLF)
                return CTX_PICKUP;
            break;
        case MSG_CONTAINER_STATE:
            switch ((u32)arg) { /* cast kept: this message passes a number in its void * argument */
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

/* 0x4df01d */
void RabbitCostume::PostLoadInit()
{
    if (IsInWorld())
        SnapToGround(1);
    homePos = pos;
    SetUpdateMode(SCN_UPD_NEVER);
}

/* 0x4df11d */
ScnObject *RabbitCostume_Create(void *record)
{
    RabbitCostume *obj = new RabbitCostume;
    obj = (RabbitCostume *)obj->Init(record, 0); /* cast kept: Init returns the ScnObject base */
    return obj;
}
