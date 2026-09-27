/* Original PAL PC Crowd, 0x435f30-0x4360d5. */
#include "sdw_types.h"
#include "sdw_enums.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *instance, Animator *animation, u16 id, u32 options);

#define SDW_MEMBERS_ScnObject static void *operator new(u32 size);

#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNBODY_GETANIMID 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_GETANIMID
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
extern s32 g_dtMs;
s32 Rand_Bounded(s32 bound);

void Crowd::PostLoadInit()
{
    boundRadius = boundRadius << 1;
}

void Crowd::Update()
{
    if (wantAnimId != GetAnimId()) {
        reactionTimerMs -= g_dtMs;
        PlayAnim(wantAnimId, 1, 1);
    }
    AdvanceAnim();
}

s32 Crowd::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    switch (msgId) {
        case MSG_CROWD_CHEER:
            if (Rand_Bounded(2))
                wantAnimId = AFOULE01_ANIM_APLAUD1;
            else
                wantAnimId = AFOULE01_ANIM_APLAUD2;
            reactionTimerMs = Rand_Bounded(1500);
            break;
        case MSG_CROWD_CALM:
            if (Rand_Bounded(2))
                wantAnimId = AFOULE01_ANIM_STAND1;
            else
                wantAnimId = AFOULE01_ANIM_STAND2;
            reactionTimerMs = Rand_Bounded(1500);
            break;
    }
    return 0;
}

ScnObject *Crowd_Create(void *record)
{
    ScnBody *object = new Crowd;
    object = object->Init(record, 0);
    return object;
}
