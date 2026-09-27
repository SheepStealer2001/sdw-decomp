/*
 * Sail (class 189, CLASSID_SAIL, vtable 0x576a10, sizeof 0x64), SheepD3D.exe 0x4e7cc0-0x4e7da9: the boat sail of
 * Level 13 that the black sheep is fired through. It has no designer properties and no fields of its own (its size is
 * exactly ScnBody's): all it does is double its own render bounding radius so the wide sail is not culled when the
 * camera looks along it, run animation 1 as a blended loop, and keep that animation advancing every frame. Its
 * message handler answers 0 to everything, which is how it refuses MSG_KILL and every query - the sail cannot be
 * destroyed or stood on.
 *
 * The PlayAnim inline has no body of its own in the exe, so its name is not recovered; it is here because its
 * expansion gives the original's option word in a stack slot (0x4e7cd9-0x4e7d04), exactly as in the other object
 * files.
 */

#define SDW_MEMBERS_ScnObject \
    static void *operator new(u32 size); /* 0x50d5f4 Scenaric_Alloc: every class factory's `new` (level heap) */

#include "sdw_types.h"
#include "sdw_enums.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *inst, Animator *animator, u16 id, u32 opts); /* 0x550196 (used by the PlayAnim inline) */
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32

/* 0x4e7cc0 - vtable +0x00: twice the culling radius, then animation 1 looping and blended. */
void Sail::PostLoadInit()
{
    boundRadius = boundRadius << 1;
    PlayAnim(AVOILE01_ANIM_STAND, 1, 1);
}

/* 0x4e7d24 - vtable +0x04: nothing but the animation. */
void Sail::Update()
{
    AdvanceAnim();
}

/* 0x4e7d37 - vtable +0x10: the sail ignores every message. */
s32 Sail::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    return 0;
}

/* 0x4e7d46 - the class factory for CLASSID 189 "Sail": new Sail (the base vtables in turn, then Sail's), then
 * ScnBody_Init(record, 0) through vtable slot +0x20. */
ScnObject *Sail_Create(void *record)
{
    Sail *obj = new Sail;
    obj = (Sail *)obj->Init(record, 0); /* cast kept: Init returns the base class */
    return obj;
}
