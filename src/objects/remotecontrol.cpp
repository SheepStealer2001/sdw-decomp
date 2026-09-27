/* T196 - original object RemoteControl.cpp (guessed name): PAL PC RemoteControl, .text 0x4e1240-0x4e1811, .rdata
 * 0x5768c8-0x5768ec (vtable). */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *inst, Animator *animator, u16 id, u32 opts); /* 0x550196 */

#define SDW_MEMBERS_ScnObject                                                                      \
    static void *operator new(u32 size);                                                           \
    void AttachTo(ScnObject *parent, u8 joint, Vec3s *offset, Vec3s *rotation, u32 arg, u32 arg2); \
    void Attach(ScnObject *parent, u8 joint)                                                       \
    {                                                                                              \
        AttachTo(parent, joint, 0, 0, 0, 0);                                                       \
    }                                                                                              \
    void SetRotation(Vec3s *rotation);


#include "sdw_classes.h"
#include "../engine/scn_tools.h"
#include "../engine/maths.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_DROP_VEC3S 1
#define SDW_INLINE_SCNOBJECT_SETROTATION_VEC3S 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_DROP_VEC3S
#undef SDW_INLINE_SCNOBJECT_SETROTATION_VEC3S
#define SDW_INLINE_SCNOBJECT_ISINWORLD 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_ISINWORLD
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_INSTFLAGS_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_INSTFLAGS_U16
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_SHADOW_SETENABLED_S32 1
#include "../engine/shadow_inlines.h"
#undef SDW_INLINE_SHADOW_SETENABLED_S32

#define SDW_INLINE_SCNOBJECT_GETPARENT 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETPARENT

#define SDW_INLINE_FREE_SCN_GETPROPU32_U16_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_SCN_GETPROPU32_U16_U32

/* 0x4e1240 */
void RemoteControl::Update()
{
    AdvanceAnim();
}

/* 0x4e1253 */
s32 RemoteControl::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    ScnObject *p;
    u8 q;
    Vec3s *r;
    switch (msgId) {
        case MSG_KILL:
            /* cast kept (both): HandleMessage's arg is a void *; MSG_KILL passes the WolfKillType in it */
            if (arg == (void *)KILL_ZAP && InstFlags(INST_F_ATTACHED)) {
                GetParent()->HandleMessage(this, MSG_KILL, (void *)KILL_ZAP);
                PlayAnim(ATELER01_ANIM_ELECTR1, 1, 1);
            }
            return 1;
        case MSG_QUERY_ACTION:
            if (sender->GetClassId() == CLASSID_WOLF)
                return CTX_PICKUP;
            break;
        case MSG_PICKUP:
            p = sender;
            q = (u8)(u32)arg; /* cast kept: HandleMessage's arg is a void *; MSG_PICKUP passes the joint number in it */
            Attach(p, q);
            PlayAnim(ATELER01_ANIM_LINK, 0, 0);
            shadow.SetEnabled(0);
            return 1;
        case MSG_DROP:
            /* cast kept: the message arg is a void *; MSG_DROP carries the drop position */
            r = (Vec3s *)arg;
            Drop(r);
            PlayAnim(ATELER01_ANIM_OBJET, 0, 0);
            shadow.SetEnabled(1);
            return 1;
        case MSG_CONTAINER_STATE:
            /* cast kept: HandleMessage's arg is a void *; this message passes a number in it */
            switch ((u32)arg) {
                case CONTAINER_RELEASED:
                    homePos = pos;
                    break;
            }
            return 1;
        case MSG_CHECKPOINT_ROLLBACK:
            Reset();
            return 1;
        case MSG_HELD_STATE_BEGIN:
            if (target)
                /* cast kept: the arg is a number (1 on, 0 off) */
                target->HandleMessage(this, MSG_REMOTE_SWITCH, (void *)1);
            return 1;
        case MSG_HELD_STATE_END:
            if (target)
                target->HandleMessage(this, MSG_REMOTE_SWITCH, 0);
            return 1;
        case MSG_QUERY_HELD_ACTION:
            if (target && !target->HandleMessage(this, MSG_REMOTE_QUERY_UNAVAILABLE, 0)) {
                if (switchOnOff)
                    return HELD_REMOTE;
                return HELD_REMOTE_ALT;
            }
            break;
    }
    return 0;
}

/* 0x4e160a */
void RemoteControl::Reset()
{
    PlayAnim(ATELER01_ANIM_OBJET, 0, 0);
    shadow.SetEnabled(1);
    SetRotation(g_pZeroVec3s);
    if (IsInWorld())
        SetPosition(&homePos);
}

/* 0x4e16d4 */
void RemoteControl::PostLoadInit()
{
    u16 *props;
    shadow.radius = 20;
    if (IsInWorld())
        SnapToGround(1);
    homePos = pos;
    PlayAnim(ATELER01_ANIM_OBJET, 0, 0);
    props = record;
    switchOnOff = Scn_GetPropU32(props, 0);
    target = Scn_GetPropObject(props, 4);
}

/* 0x4e17a1 */
ScnObject *RemoteControl_Create(void *record)
{
    RemoteControl *obj = new RemoteControl;
    obj = (RemoteControl *)obj->Init(record, 0); /* cast kept: Init returns the object as its base class */
    return obj;
}
