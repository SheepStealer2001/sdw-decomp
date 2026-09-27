/* PAL PC Cactus, 0x49ff30-0x4a004d. */
/* BYTES: dead-code. */
#include "sdw_types.h"
#include "sdw_enums.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *, Animator *, u16, u32);

#define SDW_MEMBERS_ScnObject static void *operator new(u32);


#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_SETMOVEMENTENABLED_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETMOVEMENTENABLED_S32
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
extern Wolf *g_pWolf;
#include "../engine/scn_tools.h"
/* BYTES(dead-code): properties is loaded and never used, as in the original */
void Cactus::PostLoadInit()
{
    void *properties = record;
    PlayAnim(ACACTU01_ANIM_STAND1, 0, 0);
    SetMovementEnabled(1);
}
void Cactus::Update()
{
    AdvanceAnim();
}
s32 Cactus::HandleMessage(ScnObject *sender, u32 message, void *arg)
{
    switch (message) {
        case MSG_BUMP:
            if (Vec3s_ManhattanDistXZ(&pos, &g_pWolf->pos) < 400)
                PlayAnim(ACACTU01_ANIM_BOOM, 0, 0);
            break;
    }
    return 0;
}
ScnObject *Cactus_Create(void *record)
{
    ScnBody *object = new Cactus;
    object = object->Init(record, 0);
    return object;
}
