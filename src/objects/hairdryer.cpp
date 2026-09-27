/* PAL PC HairDryer. */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *, Animator *, u16, u32);
#include "../engine/sound_mgr.h"

#define SDW_MEMBERS_ScnObject       \
    static void *operator new(u32); \
    void AttachTo(ScnObject *, u8, Vec3s *, Vec3s *, u32, u32);


#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_GETFACING 1
#define SDW_INLINE_SCNOBJECT_ISINWORLD 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_GETFACING
#undef SDW_INLINE_SCNOBJECT_ISINWORLD
#define SDW_INLINE_SCNOBJECT_ISACTIVE 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_ISACTIVE
#define SDW_INLINE_SCNOBJECT_INSTFLAGS_U16 1
#define SDW_INLINE_SCNOBJECT_STOPSOUNDHANDLE_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_INSTFLAGS_U16
#undef SDW_INLINE_SCNOBJECT_STOPSOUNDHANDLE_U16
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_SHADOW_SETVISIBLE_S32 1
#include "../engine/shadow_inlines.h"
#undef SDW_INLINE_SHADOW_SETVISIBLE_S32
u16 Sound_Play(u16, void *, u16, u8, s32);
s32 ObjGrid_QueryBoxPoints(const CollBox *, ScnObject **);
#include "../sdk/crt.h"
extern "C" s16 Math_RadiansToAngle4096(float);
#define SDW_INLINE_SCNOBJECT_GETPARENT 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETPARENT
#define AngleDifference(a, b) ((s16)((s16)(((a) - (b) + 0x800) & 0xfff) - 0x800))
#define ABS_VALUE(x) ((x) >= 0 ? (x) : -(x))
void HairDryer::PostLoadInit()
{
    blowSound = 0;
    if (IsInWorld())
        SnapToGround(1);
    homePos = pos;
    SetState(HAIRDRYER_ST_WORLD);
    PlayAnim(ACHEVE01_ANIM_OBJET, 0, 0);
}
void HairDryer::SetState(u8 next)
{
    if (blowSound) {
        StopSoundHandle(blowSound);
        blowSound = 0;
    }
    state = next;
}
void HairDryer::Update()
{
    switch (state) {
        case HAIRDRYER_ST_BLOWING:
            Blow();
            blowSound =
                Sound_Play(SND_HAIRDRYER_BLOW, this, 0xff, SNDF_LOOP | SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 0x1000);
            break;
    }
}
s32 HairDryer::HandleMessage(ScnObject *sender, u32 message, void *arg)
{
    switch (message) {
        case MSG_QUERY_ACTION:
            if (sender->GetClassId() == CLASSID_WOLF)
                return CTX_PICKUP;
            break;
        case MSG_HELD_STATE_BEGIN:
            PlayAnim(ACHEVE01_ANIM_STANDV1, 0, 0);
            SetState(HAIRDRYER_ST_BLOWING);
            return 1;
        case MSG_HELD_STATE_END:
            PlayAnim(ACHEVE01_ANIM_LINK, 0, 0);
            SetState(HAIRDRYER_ST_HELD);
            return 1;
        case MSG_CARRY_ANIM:
            if (state == HAIRDRYER_ST_BLOWING) {
                /* cast kept: HandleMessage's arg is a void *; MSG_CARRY_ANIM passes the Wolf's carry cue in it */
                if (arg == (void *)WOLF_CUE_WALK)
                    PlayAnim(ACHEVE01_ANIM_WALKV1, 0, 0);
                else
                    PlayAnim(ACHEVE01_ANIM_STANDV1, 0, 0);
            }
            return 1;
        case MSG_QUERY_HELD_ACTION:
            return HELD_FAN;
        case MSG_PICKUP:
            SetState(HAIRDRYER_ST_HELD);
            /* cast kept: HandleMessage's arg is a void *; MSG_PICKUP passes the joint number in it */
            AttachTo(sender, (u8)(u32)arg, 0, 0, 0, 0);
            shadow.SetVisible(0);
            PlayAnim(ACHEVE01_ANIM_LINK, 0, 0);
            return 1;
        case MSG_DROP:
            SetState(HAIRDRYER_ST_WORLD);
            Detach();
            /* cast kept: HandleMessage's arg is a void *; MSG_DROP passes the drop position */
            SetPosition((Vec3s *)arg);
            shadow.SetVisible(1);
            PlayAnim(ACHEVE01_ANIM_OBJET, 0, 0);
            return 1;
        case MSG_CONTAINER_STATE:
            /* cast kept: HandleMessage's arg is a void *; this message passes the container state in it */
            switch ((u32)arg) {
                case CONTAINER_RELEASED:
                    homePos = pos;
                    break;
            }
            return 1;
        case MSG_CHECKPOINT_ROLLBACK:
            Reset();
            return 1;
    }
    return 0;
}
void HairDryer::Reset()
{
    SetState(HAIRDRYER_ST_WORLD);
    shadow.SetVisible(1);
    PlayAnim(ACHEVE01_ANIM_OBJET, 0, 0);
    if (IsInWorld())
        SetPosition(&homePos);
}
void HairDryer::Blow()
{
    s16 a;
    s16 b = GetParent()->GetFacing();
    Vec3i c;
    CollBox d;
    s32 e;
    ScnObject *f[64];
    s32 g;
    ScnObject *h;
    d.min.x = pos.x - 400;
    d.min.y = pos.y - 300;
    d.min.z = pos.z - 400;
    d.max.x = pos.x + 400;
    d.max.y = pos.y + 100;
    d.max.z = pos.z + 400;
    g = ObjGrid_QueryBoxPoints(&d, f);
    for (e = 0; e < g; e++) {
        h = f[e];
        if (h != this && h->IsActive() && !h->InstFlags(INST_F_ATTACHED)) {
            c.x = pos.x - h->pos.x;
            c.y = pos.y - h->pos.y;
            c.z = pos.z - h->pos.z;
            a = Math_RadiansToAngle4096((float)atan2(c.x, c.z));
            if (ABS_VALUE(AngleDifference(b, a)) <= 0x199) {
                c.x *= c.x;
                c.y *= c.y;
                c.z *= c.z;
                if (c.x + c.z <= 160000)
                    h->HandleMessage(this, MSG_THAW, 0);
            }
        }
    }
}
ScnObject *HairDryer_Create(void *record)
{
    HairDryer *object = new HairDryer;
    object = (HairDryer *)object->Init(record, 0); /* cast kept: Init returns this as a ScnBody * */
    return object;
}
