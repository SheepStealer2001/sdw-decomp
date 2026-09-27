/* PAL PC 0x4b4270-0x4b4392. Names from data/symbols.csv. */
#include "sdw_types.h"
#include "sdw_enums.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *instance, Animator *animator, u16 id, u32 flags);

#define SDW_MEMBERS_ScnObject static void *operator new(u32 size);

#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32

void DoorMechanism::Update()
{
    if (running)
        AdvanceAnim();
}

s32 DoorMechanism::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    switch (msgId) {
        case MSG_MECHANISM_RUN:
            running = 1;
            break;
        case MSG_MECHANISM_STOP:
            running = 0;
            break;
    }
    return 0;
}

void DoorMechanism::PostLoadInit()
{
    PlayAnim(DOORMECH_ANIM_RUN, 1, 0);
    running = 0;
}

ScnObject *DoorMechanism_Create(void *record)
{
    DoorMechanism *object = new DoorMechanism;
    object = (DoorMechanism *)object->Init(record, 0); /* cast kept: Init returns this as a ScnBody * */
    return object;
}
