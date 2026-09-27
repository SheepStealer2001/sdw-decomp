/*
 * SnowyGround (class 76, vtable 0x576cd8, sizeof 0x68) - a crust of snow in Level 6 (disc Lvl-07) that holds Ralph
 * but gives way under anything else: when an object that is neither a SnowyGround nor the Wolf, with a solid box,
 * rests on its top it plays its collapse animation once, stops colliding and stops answering the ground query. The one
 * in Lvl-07 lies about 2900 units from the Snowball, which is the likely heavy object (inferred). SheepD3D.exe
 * 0x4f6160-0x4f64b2: PostLoadInit, Update, HandleMessage and the class factory.
 *
 * States (+0x64): 1 intact (anim 1 loop), 2 collapsing (anim 0 once), 0 collapsed.
 * Devices that only pin the original code generation: the inline helpers below (their names are not recovered; each
 * one is here because its expansion gives the original's stack temporaries) and the local names of Update (under /Od
 * a local's slot follows from a hash of its name, tools/vc6_locals.py).
 */

#define SDW_MEMBERS_ScnObject static void *operator new(u32 size);


#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *inst, Animator *animator, u16 id, u32 opts); /* 0x550196 */
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#define SDW_INLINE_SCNOBJECT_ENABLEBOXCOLLIDE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_ENABLEBOXCOLLIDE_S32
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#include "../engine/grid_queries.h"

s32 Box_GroundQueryFlatTop(GroundQuery *q, CollBox *box, Vec3s *boxPos, s32 margin); /* 0x515934 */
#include "camera.h"

/* 0x4f6160 - vtable +0x00: the intact loop (anim 1), state 1. */
void SnowyGround::PostLoadInit()
{
    PlayAnim(ABOCGL01_ANIM_STAND0, 1, 0);
    state = SNOWYGROUND_ST_BREAK;
    collapsed = 0;
}

/* 0x4f61c6 - vtable +0x04. Intact: the objects whose solid boxes overlap a 20-unit slab on top of the ground are
 * listed; any of them that is not of this class, not the Wolf (class 0) and has a solid box of its own makes it
 * collapse (anim 0 once, no more box collision). Collapsing: when the anim ends, state 0, and a scripted camera this
 * object owns is released. */
void SnowyGround::Update()
{
    s32 hit;
    ScnObject *other;
    s32 total;
    ScnObject *overlap[64];
    CollBox top;
    s32 index;
    switch (state) {
        case SNOWYGROUND_ST_BREAK:
            top.Box_Translate(GetFirstSolidBox(), &pos);
            top.max.y = top.min.y;
            top.min.y -= 0x14;
            total = ObjGrid_QueryBoxOverlap(&top, overlap);
            hit = 0;
            for (index = 0; index < total; index++) {
                other = overlap[index];
                if (GetClassId() != other->GetClassId() && other->GetClassId() != CLASSID_WOLF &&
                    other->GetFirstSolidBox())
                    hit = 1;
            }
            if (hit) {
                collapsed = 1;
                PlayAnim(ABOCGL01_ANIM_EXPLOD1, 0, 0);
                EnableBoxCollide(0);
                state = SNOWYGROUND_ST_BROKEN_ANIM;
            }
            break;
        case SNOWYGROUND_ST_BROKEN_ANIM:
            if (AnimFlags(ANIM_F_FINISHED)) {
                state = SNOWYGROUND_ST_INTACT;
                Camera_ReleaseScripted(this);
            }
            break;
    }
    AdvanceAnim();
}

/* 0x4f63f3 - vtable +0x10: msg 0xD (ground query) is answered with the flat top of the solid box while the ground is
 * intact; once collapsed nothing stands on it. */
s32 SnowyGround::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    switch (msgId) {
        case MSG_GROUND_QUERY:
            if (!collapsed) {
                CollBox *box = GetFirstSolidBox();
                /* cast kept: the message arg is a void *; what it carries depends on the message */
                if (box)
                    return Box_GroundQueryFlatTop((GroundQuery *)arg, box, &pos, 0);
            }
            break;
    }
    return 0;
}

/* 0x4f644f - the class factory for CLASSID 76 "SnowyGround": new SnowyGround (the base vtables in turn, then
 * SnowyGround's), then ScnBody_Init(record, 0) through vtable slot +0x20. */
ScnObject *SnowyGround_Create(void *record)
{
    SnowyGround *obj = new SnowyGround;
    obj = (SnowyGround *)obj->Init(record, 0); /* cast kept: Init returns the object as a ScnObject * */
    return obj;
}
