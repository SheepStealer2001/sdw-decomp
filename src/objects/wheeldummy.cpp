/*
 * WheelDummy (class 112, vtable 0x577034, sizeof 0x54) - the stand-in for the big platform wheel of Level 10
 * (disc Lvl-11), used in the later phases of the level in place of the wheel of the previous phase. It has no designer
 * properties (PROPSIZE_WHEELDUMMY 0) and no geometry logic of its own: the live Wheel (class 111) copies one of its
 * platform model boxes, offset by the wheel's position, into this object's box every frame (0x5094ad-0x509520), and
 * the dummy exists only to answer the ground query for Ralph so he can stand on a platform that the wheel itself no
 * longer drives.
 *
 * SheepD3D.exe 0x509680-0x5098b3: PostLoadInit, the ground-query helper, Update, HandleMessage and the factory.
 * The single WheelDummy field is the CollBox box at +0x40.
 *
 * match-addr: WheelDummy_AnswerGroundQuery=0x509747
 */
/* BYTES: cast, slot-name. */
#define SDW_MEMBERS_ScnObject            \
    static void *operator new(u32 size); \
    void SetUpdateMode(s32 mode);
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
#include "sdw_classes.h"
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32

extern Wolf *g_pWolf; /* 0x6cf310 */

/* 0x509680 - vtable +0x00: the dummy is pure logic, so it is never updated (ScnUpdateMode 2 = never). */
void WheelDummy::PostLoadInit()
{
    SetUpdateMode(SCN_UPD_NEVER);
}

/* 0x509747 - the answer to MSG_GROUND_QUERY: if Ralph is over the box in plan, the ground here is the box top
 * plus 5 and the surface is flat (normal straight up: 0xf000 = -0x1000 in 4.12, with the vertical axis pointing
 * down). Note it tests g_pWolf rather than the sender; the caller has already checked that the sender is the
 * Wolf.
 * Two arrangement devices here. The local names pin the frame (tools/vc6_locals.py: q, b, pos at EBP-4, -8,
 * -0xc). And the original keeps the test's 1/0 in a slot BEHIND `this` (EBP-0x14), which a plain `if (a && b)`
 * cannot produce: named locals are handed out before `this`, expression temporaries after it. Comparing the
 * whole && chain with 0 materialises it into such a temporary. */
/* BYTES(cast): the && chain is compared with 0 so it is materialised in a temporary behind this (EBP-0x14), as the original */
/* BYTES(slot-name): names chosen for their stack slots: q, b, pos at EBP-4, -8, -0xc */
s32 WheelDummy::AnswerGroundQuery(void *arg)
{
    Vec3s *pos;
    CollBox *b;
    GroundQuery *q;

    pos = &g_pWolf->pos;
    b = &box;
    if ((pos->x >= b->min.x && pos->x <= b->max.x && pos->z >= b->min.z && pos->z <= b->max.z) != 0) {
        q = (GroundQuery *)arg; /* cast kept: MSG_GROUND_QUERY's arg is a GroundQuery */
        q->pos.y = (s16)(box.min.y + 5);
        q->normal.y = (s16)0xf000;
        q->normal.x = 0;
        q->normal.z = 0;
        return 1;
    }
    return 0;
}

/* 0x509801 - vtable +0x04: nothing to do. */
void WheelDummy::Update() {}

/* 0x50980c - vtable +0x10: only the Wolf (class 0) is answered, and only MSG_GROUND_QUERY. */
s32 WheelDummy::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    if (sender && sender->GetClassId() == CLASSID_WOLF) {
        switch (msgId) {
            case MSG_GROUND_QUERY:
                return AnswerGroundQuery(arg);
        }
    }
    return 0;
}

/* 0x509852 - the class factory for CLASSID 112 "WheelDummy": new WheelDummy (the base vtables in turn, then
 * WheelDummy's), then ScnLogic_Init(record) through vtable slot +0x20. */
ScnObject *WheelDummy_Create(void *record)
{
    WheelDummy *obj = new WheelDummy;
    obj = (WheelDummy *)obj->Init(record); /* cast kept: Init returns the object as a ScnObject * */
    return obj;
}
