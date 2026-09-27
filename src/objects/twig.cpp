/* PAL PC Twig, 0x5028f0-0x502c94. */
#include "sdw_types.h"
#include "sdw_enums.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *, Animator *, u16, u32);

#define SDW_MEMBERS_ScnObject static void *operator new(u32);


#include "sdw_classes.h"
#include "../sdk/crt.h"
#include "../engine/scn_tools.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_ISINWORLD 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_ISINWORLD
#define SDW_INLINE_SCNOBJECT_INSTFLAGS_U16 1
#define SDW_INLINE_SCNOBJECT_SETVISIBLE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_INSTFLAGS_U16
#undef SDW_INLINE_SCNOBJECT_SETVISIBLE_S32
#define SDW_INLINE_SCNOBJECT_SETHEADING_S16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETHEADING_S16
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
extern "C" s16 Math_RadiansToAngle4096(float);
s32 Vec3s_DistSqXZ(Vec3s *, Vec3s *);
#define SDW_INLINE_FREE_PROPERTYU32_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_PROPERTYU32_VOID_U32
void Twig::PostLoadInit()
{
    void *properties = record;
    disappearDistSq = PropertyU32(properties, 0);
    disappearDistSq *= disappearDistSq;
    if (IsInWorld())
        SnapToGround(1);
    home = pos;
    velocity.x = 0;
    velocity.y = 0;
    velocity.z = -500;
    SetHeading((Math_RadiansToAngle4096((float)atan2((double)-velocity.x, (double)velocity.z)) + 0x400) & 0xfff);
    drawnLastFrame = 0;
    SetState(TWIG_ST_RESET);
}
void Twig::Update()
{
    switch (state) {
        case TWIG_ST_RESET:
            if (!drawnLastFrame) {
                SetVisible(0);
                SetPosition(&home);
                SetState(TWIG_ST_HIDDEN);
            }
            break;
        case TWIG_ST_HIDDEN:
            if (!drawnLastFrame) {
                SetVisible(1);
                SetState(TWIG_ST_DRIFT);
            }
            break;
        case TWIG_ST_DRIFT:
            if (Vec3s_DistSqXZ(&pos, &home) < disappearDistSq) {
                Vec3s_ScaleByDt(&velocity, &frameDelta);
                Translate(&frameDelta);
            } else
                SetState(TWIG_ST_RESET);
            break;
    }
    drawnLastFrame = 0;
    AdvanceAnim();
}
s32 Twig::HandleMessage(ScnObject *, u32, void *)
{
    return 0;
}
void Twig::SetState(u8 value)
{
    state = value;
    switch (state) {
        case TWIG_ST_DRIFT:
            PlayAnim(ABRINDIL_ANIM_MOVE, 1, 0);
            break;
        case TWIG_ST_RESET:
        case TWIG_ST_HIDDEN:
            PlayAnim(ABRINDIL_ANIM_DEFAULT, 1, 0);
            break;
    }
}
void Twig::Render(Camera *view)
{
    drawnLastFrame = InstFlags(INST_F_DRAWN);
    ScnBody::Render(view);
}
ScnObject *Twig_Create(void *record)
{
    Twig *object = new Twig;
    object = (Twig *)object->Init(record, 0); /* cast kept: Init returns the ScnBody * base of this object */
    return object;
}
