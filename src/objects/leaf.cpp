/* PAL PC Leaf, 0x4d1b00-0x4d1d54.
 * Inline helpers preserve original expression/temporary boundaries; names are reconstructed.
 */
#include "sdw_types.h"
#include "sdw_enums.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *instance, Animator *animator, u16 id, u32 flags);

#define SDW_MEMBERS_ScnObject            \
    static void *operator new(u32 size); \
    void SetFacing(s16 angle);

#include "sdw_classes.h"
#include "../engine/maths.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_ISINWORLD 1
#define SDW_INLINE_SCNOBJECT_SETFACING_S16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_ISINWORLD
#undef SDW_INLINE_SCNOBJECT_SETFACING_S16
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
s32 Rand_Bounded(s32 bound);
extern s32 g_dtMs;

inline u32 LeafProperty(void *record, u32 offset)
{
    /* cast kept: designer properties are 4-byte slots at byte offsets of the raw WAR record */
    return *(u32 *)((u8 *)record + offset + 0x14);
}

/* 0x4d1b00 */
void Leaf::PostLoadInit()
{
    u16 *props = record;
    range = (s16)LeafProperty(props, 0);
    if (IsInWorld())
        SnapToGround(1);
    homePos = pos;
    respawnTimerMs = Rand_Range(750, 2250);
    animating = 1;
}

/* 0x4d1b8f */
void Leaf::Update()
{
    Vec3s point;
    if (AnimFlags(ANIM_F_FINISHED)) {
        if (respawnTimerMs <= 0) {
            point.x = (s16)Rand_Range(-range, range);
            point.y = 0;
            point.z = (s16)Rand_Range(-range, range);
            point.x += homePos.x;
            point.y += homePos.y;
            point.z += homePos.z;
            SetPosition(&point);
            SetFacing((s16)Rand_Bounded(4096));
            PlayAnim(AFEUIL02_ANIM_FLY, 0, 0);
            respawnTimerMs = Rand_Range(750, 2250);
            animating = 1;
        } else {
            respawnTimerMs -= g_dtMs;
            animating = 0;
        }
    }
    if (animating)
        AdvanceAnim();
}

/* 0x4d1cf1 */
ScnObject *Leaf_Create(void *record)
{
    Leaf *object = new Leaf;
    object = (Leaf *)object->Init(record, 0); /* cast kept: Init returns the ScnBody * base of this object */
    return object;
}
