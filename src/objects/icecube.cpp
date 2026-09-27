/*
 * T169 - original object IceCube.cpp (guessed name), one translation unit.
 *   .text  0x4cbb20-0x4cc143 (IceCube_CustomCollide .. IceCube_Create)
 *   .rdata 0x57642c-0x576450 (??_7IceCube)
 *   .bss   0x6cf66c-0x6cf678 (g_iceCubeHeight, g_iceCubeHalfHeight, g_iceCubeTop, g_iceCubeMeltDuration)
 * The four globals are written `= 0` for their .bss order.
 */
/* BYTES: layout, slot-name. */
/* BYTES(layout): written '= 0' only to keep definition order in .bss */
/*
 * IceCube (class 60, vtable 0x57642c, sizeof 0x80) - the block of ice Daffy is frozen in, in Level 5 (disc Lvl-06).
 * The hair dryer melts it: HairDryer_Blow sends MSG_THAW (0x41) every frame to what is in its cone, and one second of
 * heat (meltTime 0x1000) melts the cube, which then sends MSG_THAW on to the object inside (PROPERTY OBJ). While it
 * melts the top of its collision box sinks. SheepD3D.exe 0x4cbb20-0x4cc142: CustomCollide, PostLoadInit, Update,
 * HandleMessage, Reset, SetState and the class factory.
 *
 * States (+0x7c): 0 frozen (anim 2), 1 melting (anim 0, paused on frames without heat), 2 melted (the prisoner is
 * told; waits for the anim to end), 3 gone (anim 5). The cube does not collide in states 2 and 3.
 * The four statics g_iceCube* are shared by every IceCube (there is one per level); they are this file's .bss.
 * Devices that only pin the original code generation: the inline helpers below (their names are not recovered; each
 * one is here because its expansion gives the original's stack temporaries) and the local names (under /Od a local's
 * slot follows from a hash of its name, tools/vc6_locals.py: props, it, obj, i in PostLoadInit; origin, n in
 * CustomCollide).
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

#define SDW_ABS(v) ((v) >= 0 ? (v) : -(v))

#include "../engine/collide.h"
#include "../engine/scn_tools.h"
#include "animation.h"
#include "../app/app_main.h"

extern s32 g_dt; /* 0x71b300 */

/* .bss order: VC6 emits uninitialised globals ordered by a hash of their names (Top, MeltDuration, Height, HalfHeight
   for these), and globals explicitly initialised to zero after them in definition order (src/README.md). The exe has
   definition order, so each is written `= 0`. */
s16 g_iceCubeHeight = 0;       /* 0x6cf66c  |box bottom - top| at load */
s16 g_iceCubeHalfHeight = 0;   /* 0x6cf66e  g_iceCubeHeight >> 1 */
s16 g_iceCubeTop = 0;          /* 0x6cf670  the box top (min.y) at load; Reset puts it back */
s32 g_iceCubeMeltDuration = 0; /* 0x6cf674  the length of the melt anim 0, in 4.12 s */

/* 0x4cbb20 - vtable +0x0c: the cube's own collision, its world box against the mover (the box is already in world
 * coordinates, so the offset is zero); none once melted. */
s32 IceCube::CustomCollide(ScnObject *querier, CollBox *mover, Vec3s *disp, s32 *outFrac, s32 *outY,
                           CollContact *contacts, s32 *nContacts, u32 mode)
{
    s32 n;
    Vec3s origin;
    origin.x = 0;
    origin.y = 0;
    origin.z = 0;
    n = 0;
    if (state == ICECUBE_ST_MELTED || state == ICECUBE_ST_PUDDLE)
        return 0;
    n = Collide_BoxVsObjBox(this, mover, disp, &box, &origin, outFrac, outY, contacts, nContacts);
    return n;
}

/* 0x4cbb99 - vtable +0x00: finds the prisoner (PROPERTY OBJ), snaps to the ground, builds the world box, records the
 * top, the height and the melt duration in the statics, and notes whether the level has a DaffyMilitary. */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py) */
void IceCube::PostLoadInit()
{
    ScnObject **it;
    u16 *props;
    ScnObject *obj;
    u16 i;
    props = record;
    it = g_scnObjects;
    obj = 0;
    i = 0;
    prisoner = Scn_GetPropObject(props, 0);
    SnapToGround(1);
    melted = 0;
    heated = 0;
    meltTime = 0;
    box.Box_Translate(GetFirstSolidBox(), &pos);
    g_iceCubeTop = box.min.y;
    g_iceCubeMeltDuration = (Anim_GetDurationMs(Inst(), AGLACON1_ANIM_FOND1, 0) << 12) / 1000;
    g_iceCubeHeight = SDW_ABS(box.max.y - g_iceCubeTop);
    g_iceCubeHalfHeight = g_iceCubeHeight >> 1;
    flags |= SCN_OF_CUSTOM_COLLIDE;
    noDaffy = 1;
    it = g_scnObjects;
    for (i = 0; i < g_scnObjectCount; i++, it++) {
        obj = *it;
        if (obj->GetClassId() == CLASSID_DAFFYMILITARY) {
            noDaffy = 0;
            break;
        }
    }
    EnableBoxCollide(0);
    SetState(ICECUBE_ST_FROZEN);
}

/* 0x4cbd6e - vtable +0x04. Melting: on a frame with heat the melt time grows by g_dt and the box top sinks by
 * height * (meltTime / 4) / duration (masked with the half height when Daffy is in the level); without heat the anim
 * pauses. One second of heat melts the cube; once the anim ends it is gone. */
void IceCube::Update()
{
    switch (state) {
        case ICECUBE_ST_MELTING:
            if (heated) {
                meltTime += g_dt;
                anim.speed = 0x1000;
                heated = 0;
                if (noDaffy)
                    box.min.y += (s16)(g_iceCubeHeight * (meltTime >> 2) / g_iceCubeMeltDuration);
                else
                    box.min.y += (s16)(g_iceCubeHeight * (meltTime >> 2) / g_iceCubeMeltDuration & g_iceCubeHalfHeight);
            } else
                anim.speed = 0;
            if (meltTime >= 0x1000)
                SetState(ICECUBE_ST_MELTED);
            break;
        case ICECUBE_ST_MELTED:
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(ICECUBE_ST_PUDDLE);
            break;
    }
    AdvanceAnim();
}

/* 0x4cbe99 - vtable +0x10: msg 0x32 (sent by DaffyMilitary_Reset) refreezes the cube; MSG_THAW (0x41) heats it for
 * this frame and starts the melt, unless it has melted already. */
s32 IceCube::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    switch (msgId) {
        case MSG_ICECUBE_RESET:
            SetState(ICECUBE_ST_FROZEN);
            melted = 0;
            heated = 0;
            meltTime = 0;
            return 1;
        case MSG_THAW:
            if (!melted) {
                heated = 1;
                if (state != ICECUBE_ST_MELTING)
                    SetState(ICECUBE_ST_MELTING);
            }
            return 1;
    }
    return 0;
}

/* 0x4cbf32 - vtable +0x14 (level restart): a melted cube stays gone (state 3); otherwise the box top is restored and
 * the cube is frozen again, with the melt time lost. */
void IceCube::Reset()
{
    heated = 0;
    meltTime = 0;
    if (melted)
        SetState(ICECUBE_ST_PUDDLE);
    else {
        box.min.y = g_iceCubeTop;
        SetState(ICECUBE_ST_FROZEN);
    }
}

/* 0x4cbf8b - enter a state (anim speed 1.0): 0 anim 2, 1 anim 0 (blended), 2 marks it melted and sends MSG_THAW to the
 * prisoner, 3 anim 5 (blended). */
void IceCube::SetState(u8 newState)
{
    state = newState;
    anim.speed = 0x1000;
    switch (state) {
        case ICECUBE_ST_FROZEN:
            PlayAnim(AGLACON1_ANIM_STAND2, 0, 0);
            break;
        case ICECUBE_ST_MELTING:
            PlayAnim(AGLACON1_ANIM_FOND1, 0, 1);
            break;
        case ICECUBE_ST_MELTED:
            melted = 1;
            if (prisoner)
                prisoner->HandleMessage(this, MSG_THAW, 0);
            break;
        case ICECUBE_ST_PUDDLE:
            PlayAnim(AGLACON1_ANIM_FLAC1, 0, 1);
            break;
    }
}

/* 0x4cc0dc - the class factory for CLASSID 60 "IceCube": new IceCube (the base vtables in turn, then IceCube's),
 * then ScnBody_Init(record, 0) through vtable slot +0x20. */
ScnObject *IceCube_Create(void *record)
{
    ScnBody *obj = new IceCube;
    obj = obj->Init(record, 0);
    return obj;
}
