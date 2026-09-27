/* PAL PC: Seaweed, 0x4e9e70-0x4e9fd2. */

#define SDW_MEMBERS_ScnObject            \
    static void *operator new(u32 size); \
    void SetFacing(s16 angle);

#include "sdw_types.h"
#include "sdw_enums.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *inst, Animator *animator, u16 id, u32 opts);
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_SETFACING_S16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETFACING_S16
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
s32 Rand_Bounded(s32 bound);

/* 0x4e9e70 */
void Seaweed::PostLoadInit()
{
    Vec3s at = pos;
    at.y = QueryGroundY(&at, 1);
    SetPosition(&at);
    PlayAnim(Rand_Bounded(2) == 0, 1, 0);
    SetFacing(Rand_Bounded(0x1000));
    anim.frame = Rand_Bounded(anim.cur->keyCount);
}

/* 0x4e9f41 */
void Seaweed::Update()
{
    AdvanceAnim();
}

/* 0x4e9f54 */
s32 Seaweed::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    return 0;
}

/* 0x4e9f63 */
void Seaweed::Reset() {}

/* 0x4e9f6e */
ScnObject *Seaweed_Create(void *record)
{
    ScnBody *obj = new Seaweed;
    obj = obj->Init(record, 0);
    return obj;
}
