/*
 * WoodenPlatForm (class 40, CLASSID 40 "WoodenPlatForm", vtable 0x5770a0, sizeof 0x5c) - the weighing platform: it
 * sinks toward one of three heights by what stands on it (nothing: the rest height; a sheep: halfway; Ralph or a
 * rock: -MINZVALUE), carries the objects in its ACTIVATIONZONE, moves its DOOR the opposite way and tells its
 * IDMECHANISM (DoorMechanism) msg 0x21 while moving and 0x22 on arrival.
 *
 * SheepD3D.exe 0x50b220-0x50b91e: PostLoadInit, Update, MoveTowards, CarryObjects, GetLoad, HandleMessage, the factory.
 * Local names are chosen to place the /Od frame (tools/vc6_locals.py); they say what each slot holds.
 */
/* BYTES: inline. */
#define SDW_MEMBERS_ScnObject            \
    static void *operator new(u32 size); \
    CollBox *GetFirstModelBox();
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
#include "sdw_classes.h"
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_SETDRAWMODE_U32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_SETDRAWMODE_U32

#include "../engine/scn_tools.h"
#include "../engine/scenaric.h"
s32 ObjGrid_QueryBoxOverlap(CollBox *box, ScnObject **out);                          /* 0x510f34 */
s32 ObjGrid_QueryBoxPoints(const CollBox *query, ScnObject **out);                   /* 0x510af8 */
s32 Box_GroundQueryFlatTop(GroundQuery *q, CollBox *box, Vec3s *boxPos, s32 margin); /* 0x515934 */
extern s32 g_dt;                                                                     /* 0x71b300 */

/* A designer property of the WAR record: the dword at record + 0x14 + offset. Inlined; the offset and the value are
 * stack temps (0x50b2bf, 0x50b2cf). */
#define SDW_INLINE_FREE_SCN_GETPROPS32_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_SCN_GETPROPS32_VOID_U32

/* Model box 0 (object-local), or NULL when the model has no box list. */
inline CollBox *ScnObject::GetFirstModelBox()
{
    ModelBoxList *list = inst_model->boxes;
    if (list)
        return (CollBox *)list->boxes; /* cast kept: Box and CollBox are two views of one 16-byte zone record */
    return 0;
}

#define SDW_INLINE_FREE_SCENARIC_CLASSFLAGS_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_FREE_SCENARIC_CLASSFLAGS_U16

/* Box overlap by sign bits: every difference max - min must be non-negative. Written as inline functions because
 * their arguments are evaluated last-first, which is the original's order (0x50b6d5: the last z difference is
 * computed first, into ecx); the same expression written in place evaluates first-first. */
/* BYTES(inline): source-only inline: its arguments are evaluated last-first, as the original (0x50b6d5) */
#define SDW_INLINE_FREE_BOXOVERLAP4_S32_S32_S32_S32 1
#include "../engine/coll_box_inlines.h"
#undef SDW_INLINE_FREE_BOXOVERLAP4_S32_S32_S32_S32

#define SDW_INLINE_FREE_BOXOVERLAP2_S32_S32 1
#include "../engine/coll_box_inlines.h"
#undef SDW_INLINE_FREE_BOXOVERLAP2_S32_S32
#define ABS(x) ((x) >= 0 ? (x) : -(x))

/* 0x50b220 - vtable +0x00: read the properties; the three target heights are the start height, -MINZVALUE and
 * halfway between them. No ACTIVATIONZONE disables the platform. */
void WoodenPlatForm::PostLoadInit()
{
    u16 *props;
    SetDrawMode(0);
    props = record;
    activationZone = Scn_GetPropBox(props, 0);
    if (!activationZone)
        disabled = 1;
    else
        disabled = 0;
    bodyBox = (Box *)GetFirstModelBox(); /* cast kept: as in GetFirstModelBox */
    door = Scn_GetPropObject(props, 8);
    mechanism = Scn_GetPropObject(props, 0xc);
    targetY[2] = (s16)-Scn_GetPropS32(props, 0x10);
    speed = (s16)(Scn_GetPropS32(props, 0x14) * 30);
    targetY[0] = pos.y;
    targetY[1] = (s16)((targetY[0] + targetY[2]) >> 1);
}

/* 0x50b329 - vtable +0x04: move toward the height the current load selects. */
void WoodenPlatForm::Update()
{
    s16 goal;
    s32 load;
    if (!disabled) {
        load = GetLoad();
        goal = targetY[load];
        if (pos.y != goal)
            MoveTowards(goal);
    }
}

/* 0x50b377 - one step toward targetY: going down it stops if its body would collide; the last step lands exactly.
 * The activation zone's y extent moves with it, the load is carried, the door moves the opposite way. */
void WoodenPlatForm::MoveTowards(s16 targetY)
{
    Vec3s probe;
    Vec3s delta;
    s32 step;

    delta.x = 0;
    delta.z = 0;
    step = speed * g_dt / 4096;
    if (targetY > pos.y) {
        delta.y = (s16)step;
        probe.x = pos.x;
        probe.y = pos.y + delta.y;
        probe.z = pos.z;
        if (TestBodyAt(&probe, CQ_OBJECTS))
            return;
    } else {
        delta.y = (s16)-step;
    }
    if (ABS(pos.y - targetY) > step) {
        if (mechanism)
            mechanism->HandleMessage(this, MSG_MECHANISM_RUN, 0);
    } else {
        delta.y = targetY - pos.y;
        if (mechanism)
            mechanism->HandleMessage(this, MSG_MECHANISM_STOP, 0);
    }
    activationZone->min[1] += delta.y;
    activationZone->max[1] += delta.y;
    CarryObjects(&delta);
    if (door) {
        delta.y = -delta.y;
        door->Translate(&delta);
    }
}

/* 0x50b505 - translate everything in the activation zone by delta; sheep-like classes (class flag 0x800) are first
 * told msg 0x2e {platform pos, radius 30}. */
void WoodenPlatForm::CarryObjects(Vec3s *delta)
{
    ScnObject *o;
    s32 n;
    ScnObject *found[64];
    WolfSpotArg msg;
    s32 count;

    msg.pos = pos;
    msg.radius = 0x1e;
    /* cast kept: Box and CollBox views of one zone */
    count = ObjGrid_QueryBoxOverlap((CollBox *)activationZone, found);
    for (n = 0; n < count; n++) {
        o = found[n];
        if (Scenaric_ClassFlags(o->GetClassId()) & SCN_CF_SHEEP_ANCHORABLE)
            o->HandleMessage(this, MSG_SET_ANCHOR, &msg);
        o->Translate(delta);
    }
}

/* 0x50b5f4 - the load index: 2 as soon as Ralph (0) or a rock (0x10) stands on the platform, 1 for a sheep (0xb),
 * else 0. An object counts when its origin is in the activation zone and its model box 0 overlaps the platform's
 * body box (x/z with a margin of 2, vertically with 10). */
s32 WoodenPlatForm::GetLoad()
{
    s32 hits;
    u16 classId;
    s32 res;
    ScnObject *inZone[64];
    s32 i;
    CollBox *box;
    ScnObject *obj;

    hits = ObjGrid_QueryBoxPoints((CollBox *)activationZone, inZone); /* cast kept: Box and CollBox views of one zone */
    res = WPLAT_LOAD_EMPTY;
    for (i = 0; i < hits; i++) {
        obj = inZone[i];
        classId = obj->classId;
        if (classId == CLASSID_WOLF || classId == CLASSID_SHEEP || classId == CLASSID_ROCK) {
            box = obj->GetFirstModelBox();
            if (box) {
                if (BoxOverlap4((box->max.x + obj->pos.x + 2) - (bodyBox->min[0] + pos.x - 2),
                                (bodyBox->max[0] + pos.x + 2) - (box->min.x + obj->pos.x - 2),
                                (box->max.z + obj->pos.z + 2) - (bodyBox->min[2] + pos.z - 2),
                                (bodyBox->max[2] + pos.z + 2) - (box->min.z + obj->pos.z - 2))) {
                    if (BoxOverlap2((box->max.y + obj->pos.y + 10) - (bodyBox->min[1] + pos.y - 10),
                                    (bodyBox->max[1] + pos.y + 10) - (box->min.y + obj->pos.y - 10))) {
                        if (classId == CLASSID_SHEEP)
                            res = WPLAT_LOAD_SHEEP;
                        else
                            return WPLAT_LOAD_HEAVY;
                    }
                }
            }
        }
    }
    return res;
}

/* 0x50b85f - vtable +0x10: only the ground query (0xd), answered on the flat top of model box 0 with margin 5. */
s32 WoodenPlatForm::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    switch (msgId) {
        case MSG_GROUND_QUERY:
            /* cast kept: the message arg is a void *; this one carries a GroundQuery */
            return Box_GroundQueryFlatTop((GroundQuery *)arg, GetFirstModelBox(), &pos, 5);
    }
    return 0;
}

/* 0x50b8bd - the class factory for CLASSID 40 "WoodenPlatForm": new WoodenPlatForm (the base vtables in turn, then
 * WoodenPlatForm's), then ScnLogic::Init(record) through the vtable. */
ScnObject *WoodenPlatForm_Create(void *record)
{
    WoodenPlatForm *obj = new WoodenPlatForm;
    obj = (WoodenPlatForm *)obj->Init(record); /* cast kept: Init returns the object as its base class */
    return obj;
}
