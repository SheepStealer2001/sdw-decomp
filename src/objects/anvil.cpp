/*
 * Anvil (class 29, CLASSID_ANVIL), SheepD3D.exe 0x494df0-0x495151: the Level 0 cartoon anvil. It waits hidden until
 * the sign post asks it to drop (SignPost_Update 0x4f1790 sends msg 0x1280), then appears on Ralph with his facing and
 * plays its three animations in turn: 0 the drop, 2 the hold (the scripted camera is held for 2000 ms and then
 * released), 1 the lift, after which it hides again. It has no designer properties.
 *
 * Anvil_Create's `new` is ScnObject's class-specific operator new (Scenaric_Alloc 0x50d5f4).
 *
 * The inline helpers below have no bodies of their own in the exe (/Ob1 expands them), so their names are not
 * recovered. They are there because their expansions give the original's shapes: SetVisible's constant argument tested as `xor r,r; test`
 * (0x494e02), PlayAnim's option word in a stack slot (0x494e32), SetRotation's argument in a stack temp (0x49507c).
 */
/* BYTES: dead-code. */

#define SDW_MEMBERS_ScnObject            \
    static void *operator new(u32 size); \
    void SetRotation(Vec3s *r); /* inline: its argument is a stack temp (0x49507c) */

#include "sdw_types.h"
#include "sdw_enums.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *inst, Animator *animator, u16 id, u32 opts); /* 0x550196 (used by the PlayAnim inline) */
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_SETVISIBLE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETVISIBLE_S32
#define SDW_INLINE_SCNOBJECT_SETROTATION_VEC3S 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETROTATION_VEC3S
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16

#include "camera.h"
extern Wolf *g_pWolf; /* 0x6cf310 */
extern s32 g_dtMs;    /* 0x71b2e8  g_dt * 1000 >> 12 */

/* 0x494df0 - vtable +0x00: hidden, posed on the first frame of the lift (anim 1), idle. */
/* BYTES(dead-code): rec is loaded and never used, as in the original */
void Anvil::PostLoadInit()
{
    u16 *rec = record; /* read and never used */
    SetVisible(0);
    PlayAnim(AENCLU01_ANIM_STAND0, 0, 0);
    dropping = 0;
    holdTimeMs = 0;
    state = ANVIL_ST_DONE;
}

/* 0x494e92 - vtable +0x04: drop -> hold for 2000 ms -> lift -> hidden, then the animation step. */
void Anvil::Update()
{
    switch (state) {
        case ANVIL_ST_WAIT:
            if (dropping && AnimFlags(ANIM_F_FINISHED)) {
                PlayAnim(AENCLU01_ANIM_ANVIL2, 0, 0);
                state = ANVIL_ST_HOLD;
                break;
            }
            break;
        case ANVIL_ST_HOLD:
            if (holdTimeMs > 2000) {
                dropping = 0;
                state = ANVIL_ST_LIFT;
                holdTimeMs = 0;
                Camera_ReleaseScripted(this);
                break;
            }
            holdTimeMs += g_dtMs;
            break;
        case ANVIL_ST_LIFT:
            if (AnimFlags(ANIM_F_FINISHED)) {
                PlayAnim(AENCLU01_ANIM_STAND0, 0, 0);
                state = ANVIL_ST_DONE;
                SetVisible(0);
            }
            break;
    }
    AdvanceAnim();
}

/* 0x495011 - vtable +0x10: msg 0x1280 (from the sign post) drops the anvil on Ralph: shown at his position with his
 * rotation, anim 0, state 0. Always returns 0. */
s32 Anvil::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    switch (msgId) {
        case MSG_ANVIL_DROP:
            SetVisible(1);
            SetPosition(&g_pWolf->pos);
            SetRotation(&g_pWolf->rot);
            PlayAnim(AENCLU01_ANIM_ANVIL1, 0, 0);
            dropping = 1;
            state = ANVIL_ST_WAIT;
            break;
    }
    return 0;
}

/* 0x4950ee - the class factory for CLASSID 29 "Anvil": new Anvil (the base vtables in turn, then Anvil's), then
 * ScnBody::Init(record, 0) through the vtable. */
ScnObject *Anvil_Create(void *record)
{
    ScnBody *obj = new Anvil;
    obj = obj->Init(record, 0);
    return obj;
}
