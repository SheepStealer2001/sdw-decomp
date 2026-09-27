/* PAL PC Torch, 0x4fc5e0-0x4fc836. */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *inst, Animator *animator, u16 id, u32 options);

#define SDW_MEMBERS_ScnObject                            \
    static void *operator new(u32 size);                 \
    s32 IsAlwaysVisible()                                \
    {                                                    \
        return (flags & SCN_OF_NO_DIST_CULL) != 0;       \
    }                                                    \
    s32 IsTooFar()                                       \
    {                                                    \
        return !IsAlwaysVisible() && camDist2 > 9000000; \
    }

#include "sdw_classes.h"
#include "../engine/id_list.h"
#include "../engine/scn_tools.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
void Torch::PostLoadInit()
{
    void *model;
    u32 *list;
    u16 count;
    PlayAnim(ATORCH01_ANIM_STAND1, 1, 0);
    flameRecord.pos.x = pos.x;
    flameRecord.pos.y = pos.y;
    flameRecord.pos.z = pos.z;
    flameRecord.rot.z = 0;
    flameRecord.rot.y = 0;
    flameRecord.rot.x = 0;
    flameRecord.classId = CLASSID_WOLF;
    list = Scn_FindIdList(WAR_IDO_ATORCH02, &count);
    model = (void *)*list; /* cast kept: an id list holds record addresses as u32 */
    flameRecord.modelResIndex = Dav_FindResourceIndex(model);
    flameRecord.secondaryRes = 0xffff;
    flame.Init(&flameRecord, 0);
    PlayAnim(ATORCH01_ANIM_STAND1, 1, 0);
}
void Torch::Update()
{
    AdvanceAnim();
    flame.AdvanceAnim();
}
void Torch::Render(Camera *view)
{
    ScnBody::Render(view);
    if (!IsTooFar())
        flame.RenderFacingCamera(view, 0, 0, 0);
}
ScnObject *Torch_Create(u16 *record)
{
    Torch *object = new Torch;
    object = (Torch *)object->Init(record, 0); /* cast kept: Init returns the object as its ScnBody base */
    return object;
}
