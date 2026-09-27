/* PAL PC balance, 0x495550-0x4958f2. */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *, Animator *, u16, u32);

#define SDW_MEMBERS_ScnObject       \
    static void *operator new(u32); \
    void GetModelBoxes(CollBox **, u32 *);

#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
s32 Scenaric_FindByClass(u16, ScnObject **, s32);
#define SDW_INLINE_SCNOBJECT_GETMODELBOXES_COLLBOX_U32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETMODELBOXES_COLLBOX_U32
void balance::PostLoadInit()
{
    u32 index;
    CollBox *boxes;
    Vec3s localPosition;
    u32 count;
    wolfInside = 0;
    PlayAnim(BALANCE_ANIM_RELEASE, 0, 0);
    wolf = 0;
    Scenaric_FindByClass(CLASSID_WOLF, &wolf, 1);
    GetModelBoxes(&boxes, &count);
    localPosition = pos;
    for (index = 0; index < count; index++)
        if (boxes[index].flags & COLLBOX_NONSOLID) {
            triggerBox.min[0] = boxes[index].min.x + localPosition.x;
            triggerBox.min[1] = boxes[index].min.y + localPosition.y;
            triggerBox.min[2] = boxes[index].min.z + localPosition.z;
            triggerBox.max[0] = boxes[index].max.x + localPosition.x;
            triggerBox.max[1] = boxes[index].max.y + localPosition.y;
            triggerBox.max[2] = boxes[index].max.z + localPosition.z;
            break;
        }
}
void balance::Update()
{
    Vec3s *point;
    u8 insideNow;
    Vec3s *testPoint;
    s32 hit;
    point = wolf ? &wolf->pos : &pos;
    testPoint = point;
    hit = testPoint->x >= triggerBox.min[0] && testPoint->x <= triggerBox.max[0] && testPoint->y >= triggerBox.min[1] &&
          testPoint->y <= triggerBox.max[1] && testPoint->z >= triggerBox.min[2] && testPoint->z <= triggerBox.max[2];
    if (hit)
        insideNow = 1;
    else
        insideNow = 0;
    if (insideNow != wolfInside) {
        wolfInside = insideNow;
        switch (insideNow) {
            case 0:
                PlayAnim(BALANCE_ANIM_RELEASE, 0, 0);
                break;
            case 1:
                PlayAnim(BALANCE_ANIM_PRESS, 0, 0);
                break;
        }
    }
    AdvanceAnim();
}
s32 balance::HandleMessage(ScnObject *, u32, void *)
{
    return 0;
}
ScnObject *balance_Create(void *record)
{
    balance *object = new balance;
    object = (balance *)object->Init(record, 0); /* cast kept: Init returns the base class */
    object->wolfInside = 0;
    object->wolf = 0;
    return object;
}
