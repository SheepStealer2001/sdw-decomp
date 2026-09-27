/* PAL PC HoneyPot 0x4ca660-0x4cb126. Full code and inline switch tables. */
/* BYTES: slot-scope. */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *, Animator *, u16, u32);

#define SDW_MEMBERS_ScnObject                                   \
    static void *operator new(u32);                             \
    void AttachTo(ScnObject *, u8, Vec3s *, Vec3s *, u32, u32); \
    void SetUpdateMode(s32 mode);


#include "sdw_classes.h"
#include "../engine/scn_tools.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_ISINWORLD 1
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_ISINWORLD
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_SHADOW_SETVISIBLE_S32 1
#include "../engine/shadow_inlines.h"
#undef SDW_INLINE_SHADOW_SETVISIBLE_S32
s32 Scenaric_FindByClass(u16, ScnObject **, s32);
s32 ObjGrid_QueryBoxOverlap(CollBox *, ScnObject **);
u16 Sound_Play(u16, void *, u16, u8, s32);
u32 g_honeyPotSamInHoney; /* T167 .bss 0x6cf668 */
#define SDW_INLINE_FREE_BOXCONTAINS_COLLBOX_VEC3S 1
#include "../engine/coll_box_inlines.h"
#undef SDW_INLINE_FREE_BOXCONTAINS_COLLBOX_VEC3S
void HoneyPot::PostLoadInit()
{
    void *props = record;
    if (IsInWorld())
        SnapToGround(1);
    shadow.radius = 20;
    homePos = pos;
    beeCount = Scenaric_FindByClass(CLASSID_BEES, bees, 5);
    hiveMother = Scn_GetPropObject(props, 0);
    Scenaric_FindByClass(CLASSID_SAM, &sam, 1);
    launchSoundHandle = 0;
    landSoundHandle = 0;
    SetUpdateMode(SCN_UPD_ALWAYS);
    SetState(HONEYPOT_ST_IDLE);
    weight = 20;
}
/* BYTES(slot-scope, inferred): the nested block(s) only order the frame: their locals are allocated after the enclosing scope's */
void HoneyPot::Update()
{
    ScnObject *objects[64];
    {
        s32 count;
        {
            u8 i;
            {
                u8 j;
                {
                    Vec3s point;
                    switch (state) {
                        case HONEYPOT_ST_LANDED:
                            SetState(HONEYPOT_ST_SPILLED);
                            break;
                        case HONEYPOT_ST_SPILLED:
                            g_honeyPotSamInHoney = 0;
                            spillBox.min.x = pos.x - 150;
                            spillBox.min.y = pos.y - 150;
                            spillBox.min.z = pos.z - 150;
                            spillBox.max.x = pos.x + 150;
                            spillBox.max.y = pos.y;
                            spillBox.max.z = pos.z + 150;
                            count = ObjGrid_QueryBoxOverlap(&spillBox, objects);
                            if (count > 0)
                                for (i = 0; i < count; i++)
                                    if (objects[i]->GetClassId() == CLASSID_SAM) {
                                        for (j = 0; j < beeCount; j++)
                                            bees[j]->HandleMessage(this, MSG_HONEY_VICTIM, objects[i]);
                                        hiveMother->HandleMessage(this, MSG_HONEY_VICTIM, objects[i]);
                                        g_honeyPotSamInHoney = 1;
                                    }
                            break;
                        case HONEYPOT_ST_ON_SEESAW:
                            point.x = pos.x;
                            point.y = pos.y;
                            point.z = pos.z;
                            point.y -= 200;
                            point.y = QueryGroundY(&point, 1);
                            SetPosition(&point);
                            break;
                    }
                }
            }
        }
    }
    AdvanceAnim();
}
s32 HoneyPot::HandleMessage(ScnObject *sender, u32 msg, void *arg)
{
    Vec3s *drop;
    switch (msg) {
        case MSG_QUERY_ACTION:
            if (sender->GetClassId() == CLASSID_WOLF) {
                spillBox.min.x = pos.x - 150;
                spillBox.min.y = pos.y - 150;
                spillBox.min.z = pos.z - 150;
                spillBox.max.x = pos.x + 150;
                spillBox.max.y = pos.y;
                spillBox.max.z = pos.z + 150;
                if (!BoxContains(&spillBox, &sam->pos))
                    return CTX_PICKUP;
            }
            break;
        case MSG_PICKUP:
            /* cast kept: HandleMessage's arg is a void *; MSG_PICKUP passes the joint number in it */
            AttachTo(sender, (u8)(u32)arg, 0, 0, 0, 0);
            shadow.SetVisible(0);
            SetState(HONEYPOT_ST_CARRIED);
            return 1;
        case MSG_DROP:
            drop = (Vec3s *)arg; /* cast kept: HandleMessage's arg is a void *; MSG_DROP passes the drop position */
            Detach();
            SetPosition(drop);
            shadow.SetVisible(1);
            SetState(HONEYPOT_ST_IDLE);
            return 1;
        case MSG_QUERY_HELD_ACTION:
            return HELD_THROWABLE;
        case MSG_INVENTORY_STORED:
            SetState(HONEYPOT_ST_IN_INVENTORY);
            return 1;
        case MSG_INVENTORY_TAKE_OUT:
            return 1;
        case MSG_CONTAINER_STATE:
            /* cast kept: HandleMessage's arg is a void *; this message passes the container state in it */
            switch ((u32)arg) {
                case CONTAINER_STORED:
                    SetState(HONEYPOT_ST_IDLE);
                    break;
                case CONTAINER_RELEASED:
                    homePos = pos;
                    break;
            }
            return 1;
        case MSG_CHECKPOINT_ROLLBACK:
            Reset();
            SetPosition(&homePos);
            return 1;
        case MSG_SEESAW_TOUCH:
            SetState(HONEYPOT_ST_ON_SEESAW);
            break;
        case MSG_LAUNCH:
            launchSoundHandle = Sound_Play(SND_HONEYPOT_LAUNCH, this, 0xff, SNDF_POSITIONAL, 0x1000);
            SetState(HONEYPOT_ST_LAUNCHED);
            break;
        case MSG_LANDED:
            landSoundHandle =
                Sound_Play(SND_HONEYPOT_LAND, this, 0x3ff, SNDF_POSITIONAL | SNDF_DIST_HORIZONTAL, 0x1000);
            SetState(HONEYPOT_ST_LANDED);
            break;
    }
    return 0;
}
void HoneyPot::SetState(u8 next)
{
    Vec3s point;
    state = next;
    switch (next) {
        case HONEYPOT_ST_IDLE:
            PlayAnim(APOTMI01_ANIM_DEFAULT, 1, 0);
            break;
        case HONEYPOT_ST_CARRIED:
            PlayAnim(APOTMI01_ANIM_LINK, 1, 0);
            break;
        case HONEYPOT_ST_IN_INVENTORY:
            PlayAnim(APOTMI01_ANIM_DEFAULT, 1, 0);
            break;
        case HONEYPOT_ST_LANDED:
            PlayAnim(APOTMI01_ANIM_BREAK, 0, 0);
            break;
        case HONEYPOT_ST_SPILLED:
            point.x = pos.x;
            point.y = pos.y;
            point.z = pos.z;
            point.y -= 200;
            point.y = QueryGroundY(&point, 1);
            SetPosition(&point);
            PlayAnim(APOTMI01_ANIM_BROKEN, 0, 0);
            break;
    }
}
void HoneyPot::Reset()
{
    launchSoundHandle = 0;
    landSoundHandle = 0;
    if (state == HONEYPOT_ST_SPILLED)
        SetState(HONEYPOT_ST_SPILLED);
    else
        SetState(HONEYPOT_ST_IDLE);
}
ScnObject *HoneyPot_Create(void *record)
{
    HoneyPot *object = new HoneyPot;
    object = (HoneyPot *)object->Init(record, 0); /* cast kept: Init returns this as a ScnBody * */
    return object;
}
