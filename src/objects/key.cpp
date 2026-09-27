/* PAL PC Key, 0x4cff70-0x4d04e3. */
/* BYTES: slot-scope. */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *instance, Animator *animator, u16 id, u32 options);

#define SDW_MEMBERS_ScnObject            \
    static void *operator new(u32 size); \
    void AttachTo(ScnObject *parent, u8 joint, Vec3s *offset, Vec3s *rot, u32 a, u32 b);


#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_ISINWORLD 1
#define SDW_INLINE_SCNOBJECT_SETVISIBLE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_ISINWORLD
#undef SDW_INLINE_SCNOBJECT_SETVISIBLE_S32
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_SHADOW_SETVISIBLE_S32 1
#include "../engine/shadow_inlines.h"
#undef SDW_INLINE_SHADOW_SETVISIBLE_S32
s32 Scenaric_FindByClass(u16 id, ScnObject **out, s32 capacity);
s32 Vec3s_ManhattanDist(Vec3s *a, Vec3s *b);
#define ABS_VALUE(v) ((v) >= 0 ? (v) : -(v))

void Key::PostLoadInit()
{
    Vec3s point;
    shadow.radius = 20;
    jail = 0;
    Scenaric_FindByClass(CLASSID_JAIL, &jail, 1);
    point.x = pos.x;
    point.y = pos.y;
    point.z = pos.z;
    point.y -= 200;
    point.y = QueryGroundY(&point, 1);
    SetPosition(&point);
    SetState(KEY_ON_GROUND);
    if (IsInWorld())
        SnapToGround(1);
    homePos = pos;
}
void Key::Reset()
{
    SetVisible(1);
    shadow.SetVisible(1);
    SetState(KEY_ON_GROUND);
    if (IsInWorld())
        SetPosition(&homePos);
}
void Key::SetState(u8 value)
{
    switch (value) {
        case KEY_ON_GROUND:
            PlayAnim(ACLEF01_ANIM_STAND, 0, 0);
            break;
    }
    state = value;
}
void Key::Update()
{
    if (ABS_VALUE(jail->pos.y - pos.y) < 200)
        jailDist = Vec3s_ManhattanDist(&jail->pos, &pos);
    else
        jailDist = 410;
}
/* BYTES(slot-scope, inferred): the nested block(s) only order the frame: their locals are allocated after the enclosing scope's */
s32 Key::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    switch (msgId) {
        case MSG_QUERY_ACTION:
            if (sender->GetClassId() == CLASSID_WOLF || sender->GetClassId() == CLASSID_ROBOT)
                return CTX_PICKUP;
            break;
        case MSG_PICKUP: {
            ScnObject *parent = sender;
            {
                u8 joint = (u8)(u32)arg; /* cast kept: this message's arg carries the joint number */
                AttachTo(parent, joint, 0, 0, 0, 0);
                shadow.SetVisible(0);
                SetState(KEY_HELD);
                return 1;
            }
        }
        case MSG_DROP: {
            DropMsgArg *drop = (DropMsgArg *)arg; /* cast kept: this message's arg is the drop record */
            Detach();
            if (!drop->flag1)
                SetPosition(&drop->pos);
            shadow.SetVisible(1);
            SetState(KEY_ON_GROUND);
            return 1;
        }
        case MSG_INVENTORY_STORED:
            return 1;
        case MSG_INVENTORY_TAKE_OUT:
            return 1;
        case MSG_QUERY_HELD_ACTION:
            if (jailDist < 400 && ((Jail *)jail)->open == 0) /* cast kept: jail was found by CLASSID_JAIL */
                return HELD_KEY_USE;
            return HELD_THROWABLE;
        case MSG_HELD_STATE_BEGIN:
            if (jailDist < 400)
                jail->HandleMessage(this, MSG_JAIL_OPEN, 0);
            break;
        case MSG_CONTAINER_STATE:
            switch ((s32)arg) { /* cast kept: this message's arg carries the container state */
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
ScnObject *Key_Create(void *record)
{
    Key *obj = new Key;
    obj = (Key *)obj->Init(record, 0); /* cast kept: Init returns the object it was called on */
    return obj;
}
