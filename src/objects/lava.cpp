/* PAL PC Lava, 0x4d1010-0x4d11db. */
#include "sdw_types.h"
#include "sdw_enums.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *instance, Animator *animator, u16 id, u32 options);

#define SDW_MEMBERS_ScnObject static void *operator new(u32 size);


#include "sdw_classes.h"
#include "../engine/maths.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_ISINWORLD 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_ISINWORLD
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
inline u32 LavaProperty(void *record, u32 offset)
{
    /* cast kept: designer properties are 4-byte slots at byte offsets of the raw WAR record */
    return *(u32 *)((u8 *)record + offset + 0x14);
}

void Lava::PostLoadInit()
{
    u16 *props = record;
    range = (s16)LavaProperty(props, 0);
    if (IsInWorld())
        SnapToGround(1);
    homePos = pos;
}
void Lava::Update()
{
    Vec3s point;
    if (AnimFlags(ANIM_F_FINISHED)) {
        point.x = (s16)Rand_Range(-range, range);
        point.y = 0;
        point.z = (s16)Rand_Range(-range, range);
        point.x += homePos.x;
        point.y += homePos.y;
        point.z += homePos.z;
        SetPosition(&point);
        SnapToGround(1);
        PlayAnim(ALAVE01_ANIM_SPLASH, 0, 0);
    }
    AdvanceAnim();
}
ScnObject *Lava_Create(void *record)
{
    ScnBody *obj = new Lava;
    obj = obj->Init(record, 0);
    return obj;
}
