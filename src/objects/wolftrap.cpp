/*
 * WolfTrap (class 81, CLASSID 81 "WolfTrap", vtable 0x577058, sizeof 0xa0) - the snap trap that catches Ralph (or the
 * Robot, class 0x65). Idle it answers MSG_QUERY_ACTION with 0x19; MSG_USE (1) from the victim snaps it shut: the
 * victim is frozen, snapped to the trap, and Sam and the Bell are both sent msg 0x33 (the alarm). Six PAD_CROSS
 * presses inside one 0x400 ms window open it again (state 2, 5.12 s), after which it re-arms at its home position.
 *
 * SheepD3D.exe 0x5098c0-0x509fbb: PostLoadInit, Reset, Update, HandleMessage, SetVictimFrozen, SetState, the factory.
 *
 * The inline helpers (PlayAnim, Shadow::SetVisible, SetRotation) have no bodies of their own in the exe; their
 * expansions give the original's shapes (the option word built in a stack slot from constant tests, the shadow's
 * `this` in a stack temp, the rotation source pointer in a stack temp at 0x509dce).
 */

#define SDW_MEMBERS_ScnObject            \
    static void *operator new(u32 size); \
    void SetRotation(Vec3s *r); /* inline: its argument is a stack temp (0x509dce) */


#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *inst, Animator *animator, u16 id, u32 opts); /* 0x550196 (used by the PlayAnim inline) */
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_SETROTATION_VEC3S 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_SETROTATION_VEC3S
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_SHADOW_SETVISIBLE_S32 1
#include "../engine/shadow_inlines.h"
#undef SDW_INLINE_SHADOW_SETVISIBLE_S32

s32 Scenaric_FindByClass(u16 classId, ScnObject **out, s32 max); /* 0x5145c5 */

#include "../engine/input.h"
#define g_padCurButtons (g_pad.cur.buttons) /* 0x719654 */

#define g_padPrevButtons (g_pad.prev.buttons) /* 0x71964c */

extern s32 g_dtMs; /* 0x71b2e8 */

/* 0x5098c0 - vtable +0x00: find Sam and the Bell, remember the home position and rotation, arm. */
void WolfTrap::PostLoadInit()
{
    pSam = 0;
    Scenaric_FindByClass(CLASSID_SAM, &pSam, 1);
    pBell = 0;
    Scenaric_FindByClass(CLASSID_BELL, &pBell, 1);
    pVictim = 0;
    victimFrozen = 0;
    homePos = pos;
    homeRot = rot;
    shadow.radius = 0x14;
    SetState(WOLFTRAP_ST_ARMED);
}

/* 0x50996e - vtable +0x14 */
void WolfTrap::Reset()
{
    SetState(WOLFTRAP_ST_ARMED);
    victimFrozen = 0;
}

/* 0x50998d - vtable +0x04: state 1 counts PAD_CROSS press edges (each relayed to the victim as msg 0x40) and opens
 * at 6 within a 0x400 ms window; state 2 re-arms when its timer runs out; state 3 retries the freeze. */
void WolfTrap::Update()
{
    switch (state) {
        case WOLFTRAP_ST_CAUGHT:
            if (!(g_padCurButtons & ~g_inputMap[INPUT_SLOT_CROSS]) &&
                (g_padPrevButtons & ~g_inputMap[INPUT_SLOT_CROSS])) {
                pVictim->HandleMessage(this, MSG_STRUGGLE, 0);
                struggleCount++;
            }
            if (struggleCount >= 6)
                SetState(WOLFTRAP_ST_RELEASE);
            timerMs -= g_dtMs;
            if (timerMs <= 0) {
                struggleCount = 0;
                timerMs = 0x400;
            }
            break;
        case WOLFTRAP_ST_RELEASE:
            timerMs -= g_dtMs;
            if (timerMs <= 0)
                SetState(WOLFTRAP_ST_ARMED);
            break;
        case WOLFTRAP_ST_CATCHING:
            if (SetVictimFrozen(1))
                SetState(WOLFTRAP_ST_CAUGHT);
            break;
    }
    AdvanceAnim();
}

/* 0x509ad0 - vtable +0x10: 2 (query action) answers 0x19 to Ralph or the Robot while armed; 1 (use) catches the
 * sender; 0xe (the victim lost its freeze) resets for Ralph, retries (state 3) for the Robot; 0x2c80 resets. */
s32 WolfTrap::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    if (msgId < MSG_WOLF_CAUGHT) {
        switch (msgId) {
            case MSG_QUERY_ACTION:
                if ((sender->GetClassId() == CLASSID_WOLF || sender->GetClassId() == CLASSID_ROBOT) &&
                    state == WOLFTRAP_ST_ARMED)
                    return CTX_WOLFTRAP;
                break;
            case MSG_USE:
                pVictim = sender;
                SetState(WOLFTRAP_ST_CAUGHT);
                return 1;
            case MSG_FREEZE:
                switch (sender->GetClassId()) {
                    case CLASSID_WOLF:
                        victimFrozen = 0;
                        Reset();
                        break;
                    case CLASSID_ROBOT:
                        victimFrozen = 0;
                        if (state == WOLFTRAP_ST_CAUGHT)
                            SetState(WOLFTRAP_ST_CATCHING);
                        break;
                }
                return 1;
        }
    } else {
        switch (msgId) {
            case MSG_WOLFTRAP_RESET:
                Reset();
                return 1;
        }
    }
    return 0;
}

/* 0x509bf5 - ask the victim to freeze (msg 0xe) or unfreeze (0xf); the flag follows only an accepted answer. */
s32 WolfTrap::SetVictimFrozen(s32 frozen)
{
    if (frozen != victimFrozen && pVictim) {
        /* cast kept (both): HandleMessage's arg is a void *; these messages pass a number in it */
        if (frozen) {
            if (pVictim->HandleMessage(this, MSG_FREEZE, (void *)1))
                victimFrozen = 1;
        } else {
            /* cast kept: message arguments travel as void * */
            if (pVictim->HandleMessage(this, MSG_UNFREEZE, (void *)1))
                victimFrozen = 0;
        }
    }
    return victimFrozen;
}

/* 0x509c81 - enter a state: 0 armed at home (anim 4), 1 holding the victim (anim 1, alarm to victim, Sam and
 * Bell), 2 opening (anim 3, no shadow, 0x1400 ms), then store it. */
void WolfTrap::SetState(u8 newState)
{
    switch (newState) {
        case WOLFTRAP_ST_ARMED:
            SetPosition(&homePos);
            rot = homeRot;
            shadow.SetVisible(1);
            PlayAnim(APIEGE01_ANIM_TRAP3, 0, 0);
            pVictim = 0;
            break;
        case WOLFTRAP_ST_CAUGHT:
            PlayAnim(APIEGE01_ANIM_TRAP0, 0, 0);
            SetPosition(&pVictim->pos);
            SetRotation(&pVictim->rot);
            /* cast kept (these three): HandleMessage's arg is a void *; MSG_TRAP_STATE passes 1 (caught) or 0 (released) */
            pVictim->HandleMessage(this, MSG_TRAP_STATE, (void *)1);
            SetVictimFrozen(1);
            if (pSam)
                /* cast kept: message arguments travel as void * */
                pSam->HandleMessage(this, MSG_TRAP_STATE, (void *)1);
            if (pBell)
                pBell->HandleMessage(this, MSG_TRAP_STATE, (void *)1);
            struggleCount = 0;
            timerMs = 0x400;
            break;
        case WOLFTRAP_ST_RELEASE:
            PlayAnim(APIEGE01_ANIM_TRAP2, 0, 0);
            shadow.SetVisible(0);
            timerMs = 0x1400;
            pVictim->HandleMessage(this, MSG_TRAP_STATE, (void *)0); /* cast kept: as above */
            SetVictimFrozen(0);
            pVictim = 0;
            break;
    }
    state = newState;
}

/* 0x509f4c - the class factory for CLASSID 81 "WolfTrap": new WolfTrap (the base vtables in turn, then WolfTrap's),
 * then ScnMobile::Init(record, 0) through the vtable. */
ScnObject *WolfTrap_Create(void *record)
{
    ScnBody *obj = new WolfTrap;
    obj = obj->Init(record, 0);
    return obj;
}
