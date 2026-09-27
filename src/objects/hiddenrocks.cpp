/* PAL PC 0x4c9930-0x4c9aef. */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
#define SDW_MEMBERS_ScnObject       \
    static void *operator new(u32); \
    CollBox *GetFirstModelBox();    \
    s16 GetBoxHeight();
#include "sdw_classes.h"
#include "../engine/scn_tools.h"
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
inline CollBox *ScnObject::GetFirstModelBox()
{
    ModelBoxList *list = inst_model->boxes;
    if (list)
        return list->boxes;
    return 0;
}
inline s16 ScnObject::GetBoxHeight()
{
    CollBox *box = GetFirstSolidBox();
    if (!box)
        return 0;
    return box->max.y;
}
struct HiddenRockPosition {
    ScnObject *owner;
    Vec3s point;
};
void HiddenRocks::PostLoadInit()
{
    SnapToGround(1);
}
void HiddenRocks::Update() {}
s32 HiddenRocks::HandleMessage(ScnObject *sender, u32 message, void *arg)
{
    CollBox *a;
    CollBox *b;
    CollBox c;
    CollBox d;
    Vec3s e;
    HiddenRockPosition *f;
    switch (message) {
        case MSG_QUERY_COVER:
            if (sender->GetClassId() == CLASSID_WOLF) {
                b = sender->GetFirstModelBox();
                a = GetFirstModelBox();
                if (b && a) {
                    c.Box_Translate(b, &sender->pos);
                    d.Box_Translate(a, &pos);
                    if (Box_GapXZ(&c, &d) <= 10)
                        return 1;
                }
            }
            break;
        case MSG_TIMEMACHINE_SWAP:
            f = (HiddenRockPosition *)arg; /* cast kept: MSG_TIMEMACHINE_SWAP's arg is a HiddenRockPosition */
            e = f->point;
            e.y += f->owner->GetBoxHeight();
            SetPosition(&e);
            break;
    }
    return 0;
}
ScnObject *HiddenRocks_Create(void *record)
{
    HiddenRocks *object = new HiddenRocks;
    object = (HiddenRocks *)object->Init(record); /* cast kept: Init returns the object as a ScnObject * */
    return object;
}
