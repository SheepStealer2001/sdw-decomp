/* PAL PC Fish, 0x4bd760-0x4bdc2e. */
/* BYTES: slot-scope. */
#include "sdw_types.h"
#include "sdw_enums.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *, Animator *, u16, u32);

#define SDW_MEMBERS_ScnObject static void *operator new(u32);


#include "sdw_classes.h"
#include "../engine/scn_tools.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_INSTFLAGS_U16 1
#define SDW_INLINE_SCNOBJECT_SETCONTACTENABLED_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_INSTFLAGS_U16
#undef SDW_INLINE_SCNOBJECT_SETCONTACTENABLED_S32
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
s32 Vec3s_DistSqXZ(Vec3s *, Vec3s *);
inline s32 FishProperty(void *record, u32 offset)
{
    /* cast kept: designer properties are 4-byte slots at byte offsets of the raw WAR record */
    return *(s32 *)((u8 *)record + offset + 0x14);
}
/* BYTES(slot-scope, inferred): the nested block(s) only order the frame: their locals are allocated after the enclosing scope's */
void Fish::PostLoadInit()
{
    u16 *props = record;
    disappearDistSq = FishProperty(props, 0);
    disappearDistSq = disappearDistSq * disappearDistSq;
    homePos = pos;
    {
        s16 angle = rot.y;

        if (angle >= 0x200 && angle < 0x600) {
            swimVel.x = -250;
            swimVel.y = 0;
            swimVel.z = 0;
            leapOffset.x = -489;
            leapOffset.y = 0;
            leapOffset.z = 0;
        } else if (angle >= 0x600 && angle < 0xa00) {
            swimVel.x = 0;
            swimVel.y = 0;
            swimVel.z = 250;
            leapOffset.x = 0;
            leapOffset.y = 0;
            leapOffset.z = 489;
        } else if (angle >= 0xa00 && angle < 0xe00) {
            swimVel.x = 250;
            swimVel.y = 0;
            swimVel.z = 0;
            leapOffset.x = 489;
            leapOffset.y = 0;
            leapOffset.z = 0;
        } else {
            swimVel.x = 0;
            swimVel.y = 0;
            swimVel.z = -250;
            leapOffset.x = 0;
            leapOffset.y = 0;
            leapOffset.z = -489;
        }
    }
    SetContactEnabled(0);
    SetState(FISH_ST_SWIM);
}
void Fish::Update()
{
    switch (state) {
        case FISH_ST_WAIT_HOME:
            if (offscreenTicks >= 2)
                SetState(FISH_ST_SWIM);
            if (wasOnScreen == 0)
                ++offscreenTicks;
            break;
        case FISH_ST_SWIM:
            Vec3s_ScaleByDt(&swimVel, &frameStep);
            Translate(&frameStep);
            if (Vec3s_DistSqXZ(&pos, &homePos) > disappearDistSq)
                SetState(FISH_ST_LEAP);
            break;
        case FISH_ST_LEAP:
            if (AnimFlags(ANIM_F_FINISHED)) {
                Translate(&leapOffset);
                SetState(FISH_ST_LAND);
            }
            break;
        case FISH_ST_LAND:
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(FISH_ST_WAIT_HOME);
            break;
    }
    wasOnScreen = 0;
    AdvanceAnim();
}
s32 Fish::HandleMessage(ScnObject *, u32, void *)
{
    return 0;
}
void Fish::SetState(u8 next)
{
    state = next;
    switch (next) {
        case FISH_ST_WAIT_HOME:
            SetPosition(&homePos);
            offscreenTicks = 0;
            break;
        case FISH_ST_SWIM:
            PlayAnim(APOISSO2_ANIM_SWIM, 1, 0);
            break;
        case FISH_ST_LEAP:
            PlayAnim(APOISSO2_ANIM_SWIM3, 0, 1);
            break;
        case FISH_ST_LAND:
            PlayAnim(APOISSO2_ANIM_SWIM2, 0, 0);
            break;
    }
}
void Fish::Render(Camera *view)
{
    wasOnScreen = InstFlags(INST_F_DRAWN);
    ScnBody::Render(view);
}
ScnObject *Fish_Create(void *record)
{
    Fish *object = new Fish;
    object = (Fish *)object->Init(record, 0); /* cast kept: Init returns the ScnBody * base of this object */
    return object;
}
