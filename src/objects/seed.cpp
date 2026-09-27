/*
 * T209 - original object Seed.cpp (guessed name), one translation unit: Seed, then Tree (the Seed_Reset -> Tree_Init
 * transition at 0x4ead6a is unaligned, so both classes are one object).
 *   .text  0x4ea080-0x4eb0aa (Seed_Init .. Seed_Reset, Tree_Init .. Tree_Reset)
 *   .rdata 0x576afc-0x576b44 (??_7Seed, ??_7Tree)
 * Seed first, then Tree; the SDW_MEMBERS lists serve both.
 */
/* PAL PC: Seed. */

#define SDW_MEMBERS_ScnObject                                                                 \
    static void *operator new(u32 size);                                                      \
    void AttachTo(ScnObject *parent, u8 joint, Vec3s *offset, Vec3s *rotation, u32 a, u32 b); \
    void SetUpdateMode(s32 mode);                                                             \
    void SetInventoryFlag(s32 on)                                                             \
    {                                                                                         \
        if (on)                                                                               \
            flags |= SCN_OF_KEPT;                                                             \
        else                                                                                  \
            flags &= (u16)~SCN_OF_KEPT;                                                       \
    }


#define SDW_MEMBERS_ZoneList void Load(u32 id);

#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *inst, Animator *animator, u16 id, u32 opts);
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_ISINWORLD 1
#define SDW_INLINE_SCNOBJECT_SETCOLLIDABLE_S32 1
#define SDW_INLINE_SCNOBJECT_SETVISIBLE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_ISINWORLD
#undef SDW_INLINE_SCNOBJECT_SETCOLLIDABLE_S32
#undef SDW_INLINE_SCNOBJECT_SETVISIBLE_S32
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16

#include "timemachine.h"
#include "sheep.h"
#include "../engine/scn_tools.h"
#include "../engine/id_list.h"
s32 Scenaric_FindByClass(u16 classId, ScnObject **out, s32 max);

#define SDW_INLINE_FREE_SCN_GETPROPU32_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_SCN_GETPROPU32_VOID_U32

#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32

#define SDW_INLINE_ZONELIST_LOAD_U32 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_LOAD_U32

#define SDW_INLINE_ZONELIST_FINDCONTAINING_VEC3S 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_FINDCONTAINING_VEC3S

#define SDW_INLINE_ZONELIST_FINDCONTAININGXZ_VEC3S 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_FINDCONTAININGXZ_VEC3S

#define SDW_INLINE_FREE_BOX_CONTAINSPOINTXZ_BOX_VEC3S 1
#include "../engine/coll_box_inlines.h"
#undef SDW_INLINE_FREE_BOX_CONTAINSPOINTXZ_BOX_VEC3S

/* 0x4ea080. zones is the ZoneList at +0x84. */
void Seed::PostLoadInit()
{
    void *config = record;
    u8 i;
    Vec3s point;
    zones.Load(Scn_GetPropU32(config, 0));
    tree = (Tree *)Scn_GetPropObject(config, 4); /* cast kept: a downcast: the TREE property names a Tree */
    if (Scenaric_FindByClass(CLASSID_TIMEMACHINESPHERE, &sphere, 1) != 1)
        sphere = 0;
    if (Scenaric_FindByClass(CLASSID_GOSSAMER_LEV08, &gossamer, 1) != 1)
        gossamer = 0;
    if (IsInWorld()) {
        SnapToGround(1);
        homePos = pos;
        SetState(SEED_PLANTED);
    } else {
        SetState(SEED_IDLE);
    }
    point = (Vec3s)g_timeMachineOffsetToPresent;
    for (i = 0; i < zones.count; i++) {
        presentBoxes[i] = *zones.boxes[i];
        presentBoxes[i].min[0] += point.x;
        presentBoxes[i].min[1] += point.y;
        presentBoxes[i].min[2] += point.z;
        presentBoxes[i].max[0] += point.x;
        presentBoxes[i].max[1] += point.y;
        presentBoxes[i].max[2] += point.z;
    }
    SetUpdateMode(SCN_UPD_ALWAYS);
}

/* 0x4ea3b3 */
void Seed::Update()
{
    switch (state) {
        case SEED_PLANTING:
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(SEED_PLANTED);
            break;
    }
    AdvanceAnim();
}

/* 0x4ea3f5 */
void Seed::SetState(u8 value)
{
    state = value;
    switch ((s32)value) {
        case SEED_HELD:
            PlayAnim(AGRAIN01_ANIM_LINK, 1, 0);
            break;
        case SEED_STORED:
            PlayAnim(AGRAIN01_ANIM_OBJET, 1, 0);
            break;
        case SEED_PLANTING:
            PlayAnim(AGRAIN01_ANIM_GROW, 0, 1);
            break;
        case SEED_PLANTED:
            PlayAnim(AGRAIN01_ANIM_GROW2, 1, 1);
            break;
    }
}

/* 0x4ea560 */
s32 Seed::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    switch (msgId) {
        case MSG_SEED_QUERY_TREE_POS:
            if (state == SEED_PLANTED) {
                Box *box = zones.FindContainingXZ(&pos);
                treePosReply = (Vec3s *)arg; /* cast kept: MSG_SEED_QUERY_TREE_POS's arg is the Vec3s to fill */
                treePosReply->x = (box->max[0] + box->min[0]) / 2;
                treePosReply->y = pos.y;
                treePosReply->z = (box->max[2] + box->min[2]) / 2;
                return 1;
            }
            return 0;
        case MSG_TIMEMACHINE_SWAP_OUT: {
            u8 i;
            Vec3s point;
            for (i = 0; i < zones.count; i++) {
                sheepScratch = 0;
                sheepScratch = g_pSheepOutOfZone;
                if (sheepScratch) {
                    Vec3s center;
                    center.x = (presentBoxes[i].max[0] + presentBoxes[i].min[0]) / 2;
                    center.y = presentBoxes[i].min[1];
                    center.z = (presentBoxes[i].max[2] + presentBoxes[i].min[2]) / 2;
                    ScnObject *nearSheep = 0;
                    nearSheep = Scenaric_FindNearestOfClass(&center, CLASSID_SHEEP, -800, 800, 300, 0, 0);
                    if (sheepScratch == nearSheep) {
                        Vec3s outside;
                        outside.x = presentBoxes[i].min[0];
                        outside.y = presentBoxes[i].min[1];
                        outside.z = presentBoxes[i].min[2] - 50;
                        outside.y = QueryGroundY(&outside, 1);
                        sheepScratch->SetPosition(&outside);
                    }
                }
                point = pos;
                point.x += g_timeMachineOffsetToPresent.x;
                point.y += g_timeMachineOffsetToPresent.y;
                point.z += g_timeMachineOffsetToPresent.z;
                if (gossamer) {
                    if (Box_ContainsPointXZ(&presentBoxes[i], &point)) {
                        if (Box_ContainsPointXZ(&presentBoxes[i], &gossamer->pos)) {
                            Vec3s outside;
                            outside.x = presentBoxes[i].min[0];
                            outside.y = pos.y - 200;
                            outside.z = presentBoxes[i].min[2];
                            outside.y = QueryGroundY(&outside, 1);
                            gossamer->SetPosition(&outside);
                        }
                    }
                }
            }
            if (state == SEED_PLANTED)
                /* cast kept: arg carries a number */
                tree->HandleMessage(this, MSG_TREE_SET_VISIBLE, (void *)1);
            else
                tree->HandleMessage(this, MSG_TREE_SET_VISIBLE, 0);
            return 1;
        }
        case MSG_QUERY_ACTION:
            if (sender->GetClassId() == CLASSID_WOLF || sender->GetClassId() == CLASSID_ROBOT)
                return CTX_PICKUP;
            break;
        case MSG_INVENTORY_STORED:
            SetState(SEED_STORED);
            SetInventoryFlag(1);
            break;
        case MSG_QUERY_HELD_ACTION:
            if (zones.FindContaining(&sender->pos))
                return HELD_SEED_PLANT;
            return HELD_NONE;
        case MSG_HELD_STATE_BEGIN:
            return 1;
        case MSG_PICKUP: {
            ScnObject *parent = sender;
            u8 j = (u8)(u32)arg; /* cast kept: arg carries the joint number */
            AttachTo(parent, j, 0, 0, 0, 0);
            SetState(SEED_HELD);
            return 1;
        }
        case MSG_DROP: {
            DropMsgArg *at = (DropMsgArg *)arg; /* cast kept: MSG_DROP's arg is a DropMsgArg */
            Detach();
            SetPosition(&at->pos);
            if (at->placed)
                SetState(SEED_PLANTING);
            return 1;
        }
        case MSG_CONTAINER_STATE:
            switch ((s32)arg) { /* cast kept: arg carries a number */
                case CONTAINER_STORED:
                    SetState(SEED_IDLE);
                    break;
                case CONTAINER_RELEASED:
                    homePos = pos;
                    SetState(SEED_IDLE);
                    break;
            }
            return 1;
        case MSG_CHECKPOINT_ROLLBACK:
            SetPosition(&homePos);
            SetState(SEED_IDLE);
            return 1;
        case MSG_SEED_QUERY_ARRIVAL: {
            arrivalQuery = (TimeTravelArg *)arg; /* cast kept: MSG_SEED_QUERY_ARRIVAL's arg is a TimeTravelArg */
            Box *box = zones.FindContainingXZ(&arrivalQuery->obj->pos);
            if (box) {
                if (Box_ContainsPointXZ(box, &pos)) {
                    if (arrivalQuery->obj->GetClassId() == CLASSID_WOLF) {
                        arrivalQuery->pos.x = box->max[0];
                        arrivalQuery->pos.y = pos.y - 20;
                        arrivalQuery->pos.z = box->max[2];
                    } else {
                        arrivalQuery->pos.x = box->min[0];
                        arrivalQuery->pos.y = pos.y - 20;
                        arrivalQuery->pos.z = box->min[2];
                    }
                    return 1;
                }
            }
            return 0;
        }
    }
    return 0;
}

/* 0x4eacf8 */
ScnObject *Seed_Create(void *record)
{
    ScnBody *obj = new Seed;
    obj = obj->Init(record, 0);
    return obj;
}

/* 0x4ead5f */
void Seed::Reset() {}

/* ---- Tree. Its ScnObject and Tree members are declared above, and it reads its property through the
   Scn_GetPropU32 above. ---- */

/* 0x4ead6a */
void Tree::PostLoadInit()
{
    u16 *props = record;
    visibleDirectly = Scn_GetPropU32(props, 0);
    if (visibleDirectly)
        SetVisible(1);
    else
        SetVisible(0);
}

/* 0x4eadbb */
void Tree::Update()
{
    switch ((s32)visible) {
    }
}

/* 0x4eadd2 Tree_SetVisible */
void Tree::SetVisible(u8 value)
{
    visible = value;
    switch (value) {
        case 0:
            SetCollidable(0);
            ScnObject::SetVisible(0);
            break;
        case 1:
            SetCollidable(1);
            ScnObject::SetVisible(1);
            break;
    }
}

/* 0x4eaec7 */
s32 Tree::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    Vec3s offset;
    switch (msgId) {
        case MSG_TIMEMACHINE_SWAP:
            /* cast kept: MSG_TIMEMACHINE_SWAP's arg points at the swapped object's pointer */
            if (*(ScnObject **)arg && (*(ScnObject **)arg)->GetClassId() == CLASSID_SEED) {
                seedPos.x = 0;
                seedPos.y = 0;
                seedPos.z = 0;
                /* cast kept: as above */
                if ((*(ScnObject **)arg)->HandleMessage(this, MSG_SEED_QUERY_TREE_POS, &seedPos)) {
                    offset = (Vec3s)g_timeMachineOffsetToPresent;
                    seedPos.x += offset.x;
                    seedPos.y += offset.y;
                    seedPos.z += offset.z;
                    SetPosition(&seedPos);
                    SetVisible(1);
                } else {
                    SetVisible(0);
                }
            }
            return 1;
        case MSG_TREE_SET_VISIBLE:
            if (arg)
                SetVisible(1);
            else
                SetVisible(0);
            return 1;
        case MSG_QUERY_ACTION:
            if (sender->GetClassId() == CLASSID_WOLF) {
                if (visible == 1)
                    return CTX_CLIMB;
                return CTX_NONE;
            }
            break;
    }
    return 0;
}

/* 0x4eb03d */
ScnObject *Tree_Create(void *record)
{
    ScnLogic *obj = new Tree;
    obj = (ScnLogic *)obj->Init(record); /* cast kept: Init returns the object as a ScnObject * */
    return obj;
}

/* 0x4eb09f */
void Tree::Reset() {}
