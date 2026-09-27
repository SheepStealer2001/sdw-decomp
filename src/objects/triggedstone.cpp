/*
 * TriggedStone (class 52, vtable 0x576ee8, sizeof 0x9c) - the boulder on the cliff in Level 4 (disc Lvl-04): a button
 * (MSG_SWITCH_ON 0x1F) starts it, TIMER ms later it rolls down, and from 11/16 of the roll animation on it crushes Ralph,
 * Sam or a sheep standing in CRASHINGZONE (message 0, argument 2). SheepD3D.exe 0x5022d0-0x5028ec: PostLoadInit, Reset,
 * Update, the crush test, HandleMessage and the class factory.
 *
 * States (+0x7c): 1 resting (anim 1), 0 triggered (counting TIMER), 2 rolling (anim 0), 3 landed (anim 2); when the
 * landing animation ends it goes back to 1 and can be triggered again once the button has been released (msg 0x20).
 * Devices that only pin the original code generation: the inline helpers (their names are not recovered) and the names
 * of the locals in CrushVictims (their stack slots follow from their names, tools/vc6_locals.py).
 */
/* BYTES: cast, flow, inline, slot-name. */

#define SDW_MEMBERS_ScnObject            \
    static void *operator new(u32 size); \
    /* inline: a u16 temp per read (0x502668) */

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
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#include "../engine/grid_queries.h"
#include "../engine/property_math.h"

#include "animation.h"
#include "camera.h"
#include "../app/app_main.h"
void Camera_StartScripted(ScnObject *owner, Camera *cam, u16 rotX, u16 rotY, u16 rotZ, Vec3s *pos, u16 focal, u32 mode,
                          s32 time); /* 0x55a70d */

extern Wolf *g_pWolf; /* 0x6cf310 */
extern s32 g_dtMs;    /* 0x71b2e8  g_dt * 1000 >> 12 */

/* A designer property of the WAR record: the dword at record + 0x14 + offset (TriggedStoneProps). Inlined; the offset
 * is a stack temp (0x5022f9), which it is with a u32 parameter (an s32 one is folded into the address; observed, the
 * mechanism is not established). */
/* BYTES(cast): source-only inline with a u32 offset: that gives the original's stack temp (0x5022f9) */
#define SDW_INLINE_FREE_SCN_GETPROPS32_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_SCN_GETPROPS32_VOID_U32

/* Camera_StartScripted through an inline: the rotation, position and focal arguments are stack temps, stored right to
 * left (0x5027cb-0x502819); the owner, camera, mode and time are passed as they are. */
/* BYTES(inline): source-only inline: its non-variable arguments become stack temps stored right to left (0x5027cb) */
#define SDW_INLINE_FREE_CAMERA_SCRIPT_SCNOBJECT_CAMERA_U16_U16_U16_VEC3S_U16_U32_S32 1
#include "camera_inlines.h"
#undef SDW_INLINE_FREE_CAMERA_SCRIPT_SCNOBJECT_CAMERA_U16_U16_U16_VEC3S_U16_U32_S32

/* 0x5022d0 - vtable +0x00: reads CRASHINGZONE, TIMER and IDCAMERA, times the crush at 11/16 of the roll animation, and
 * rests in state 1. */
void TriggedStone::PostLoadInit()
{
    void *rec = record;
    crushZone = Scn_GetPropBox(rec, 0);
    triggerDelayMs = Scn_GetPropS32(rec, 8);
    camera = Scn_GetPropCamera(rec, 4);
    crushTimeMs = Anim_GetDurationMs(Inst(), AROCHE03_ANIM_FALL1, 1) * 11 >> 4;
    timer = 0;
    wolfOnButton = 0;
    state = TSTONE_ST_DONE;
    triggered = 0;
    PlayAnim(AROCHE03_ANIM_STAND1, 0, 0);
}

/* 0x5023c4 - vtable +0x14: back to rest (anim 1) and re-armed. */
void TriggedStone::Reset()
{
    triggered = 0;
    PlayAnim(AROCHE03_ANIM_STAND1, 0, 0);
    wolfOnButton = 0;
    state = TSTONE_ST_DONE;
}

/* 0x502433 - vtable +0x04. */
/* BYTES(flow): the branch ends the case (break) instead of an else, as the original */
void TriggedStone::Update()
{
    switch (state) {
        case TSTONE_ST_ARMED:
            if (timer > triggerDelayMs) {
                state = TSTONE_ST_FALL;
                PlayAnim(AROCHE03_ANIM_FALL1, 0, 0);
                timer = 0;
                break; /* as in the original: the branch ends the case instead of an else */
            }
            timer += g_dtMs;
            break;
        case TSTONE_ST_FALL:
            if (timer > crushTimeMs)
                CrushVictims();
            if (AnimFlags(ANIM_F_FINISHED)) {
                PlayAnim(AROCHE03_ANIM_EXPLOD1, 0, 0);
                state = TSTONE_ST_SETTLE;
                timer = 0;
                break;
            }
            timer += g_dtMs;
            break;
        case TSTONE_ST_SETTLE:
            if (AnimFlags(ANIM_F_FINISHED)) {
                Camera_ReleaseScripted(this);
                PlayAnim(AROCHE03_ANIM_STAND1, 0, 0);
                state = TSTONE_ST_DONE;
            }
            break;
    }
    AdvanceAnim();
}

/* 0x502603 - every Ralph (class 0), Sam (1) or sheep (11) whose origin is in CRASHINGZONE gets message 0 with argument
 * 2 (crushed); one that accepts it is dropped onto the ground. */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py) */
void TriggedStone::CrushVictims()
{
    ScnObject *list[64];
    s32 count;
    s32 i;
    /* cast kept: Box and CollBox are two views of one 16-byte record (CollBox has the methods) */
    count = ObjGrid_QueryBoxPoints((const CollBox *)crushZone, list);
    for (i = 0; i < count; i++) {
        if (list[i]->GetClassId() == CLASSID_WOLF || list[i]->GetClassId() == CLASSID_SAM ||
            list[i]->GetClassId() == CLASSID_SHEEP) {
            /* cast kept: HandleMessage's arg is a void *: this message passes a number in it */
            if (list[i]->HandleMessage(this, MSG_KILL, (void *)KILL_CRUSH))
                list[i]->SnapToGround(0);
        }
    }
}

/* 0x502713 - vtable +0x10. MSG_SWITCH_ON 0x1F (arg: the NULL-terminated list of objects on the button) notes whether
 * Ralph is among them and, from rest and not yet triggered, starts the countdown, with the IDCAMERA shot when Ralph is
 * on the button; MSG_SWITCH_OFF 0x20 re-arms it and releases the camera. */
s32 TriggedStone::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    ScnObject **p;
    switch (msgId) {
        case MSG_SWITCH_ON:
            /* cast kept: the message arg is a void *: what it carries depends on the message id */
            p = (ScnObject **)arg;
            while (*p) {
                if (*p == g_pWolf)
                    wolfOnButton = 1;
                p++;
            }
            if (state == TSTONE_ST_DONE && !triggered) {
                state = TSTONE_ST_ARMED;
                timer = 0;
                triggered = 1;
                if (camera && wolfOnButton)
                    Camera_Script(this, &g_camera, camera->rot[0], camera->rot[1], camera->rot[2], &camera->eye,
                                  camera->focal, CAMSCR_BLEND_IN | CAMSCR_BLEND_OUT, 0x1000);
            }
            break;
        case MSG_SWITCH_OFF:
            triggered = 0;
            wolfOnButton = 0;
            Camera_ReleaseScripted(this);
            break;
    }
    return 0;
}

/* 0x50287d - the class factory for CLASSID 52 "TriggedStone": new TriggedStone (the base vtables in turn, then
 * TriggedStone's), then ScnMobile_Init(record, 0) through vtable slot +0x20. */
ScnObject *TriggedStone_Create(void *record)
{
    TriggedStone *obj = new TriggedStone;
    obj = (TriggedStone *)obj->Init(record, 0); /* cast kept: Init returns the ScnBody * base of this object */
    return obj;
}
