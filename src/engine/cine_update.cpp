/* match-flags: /O2 /Oy- */
/*
 * T312 - original object guessed as CineUpdate.cpp (data/tu_map.json). Ranges: .data 0x581738-0x581744 (this object's
 * copy of the opcode-stride table, g_cineOpStride2); code: COMDATs only (/O2 /Oy-, and /Ob1: under /Ob2 the helpers get
 * inlined into Cine_Update and it differs), 0x5626d0-0x5631d0 (Cine_Update, the opcode helpers, the four key-cursor
 * helpers): 12 functions. Cine_Start .. Cine_InitRecordCursors and the g_cinePlayer initialiser are T313,
 * Sound_SetSfxVolume T314. Cine_Update matches only as the FIRST function of its object (any predecessor changes its
 * register allocation), which is how it is placed here. The map leaves open whether Cine_Start/Cine_Stop belong here
 * (its alternative H8) or to T313 (H9, followed here).
 *
 * The stride table is defined here (a header static) and the pad words the skip test reads (g_padMasks,
 * g_padCurButtons, g_padPrevButtons) are macros for the element / fields they are (see there).
 * Opcodes (CineRecord.opcode): 1 position key, 2 rotation key, 3 animation, 4/5 visibility flicker (two phases), 6 camera
 * focal scale, 7/8 attach/detach (two phases). A key is a u16 time word (low 14 bits: the key's time in 16 ms units; top
 * 2 bits: 1 step / 2 interpolate) followed by its payload. The devices that pin the code generation (the (bool)
 * conditions and the `shown` local in Cine_Update, ScnBody::PlayAnim for its animation starts, `obj` in
 * Cine_OpFlickerVisibility) are noted where used.
 */
/* BYTES: flow, inline, layout, slot-scope, switches, temp, view. */
/* BYTES(layout): must stay the first function of the object: any predecessor changes its register allocation */
/* BYTES(inline): ScnBody::PlayAnim (SDW_MEMBERS_ScnBody inline): source-only inline: Cine_Update's register allocation only comes out with the call written through it */
/* BYTES(view): spelled as the element / fields they are (macros), not separate objects */
/* BYTES(layout): the Cine.h header static: every object including the cinematic header carries this copy in its .data, referenced or not */
/* BYTES(switches): built with /O2 /Oy-, not the project recipe; the file header says why */

#define SDW_MEMBERS_Cine Cine(); /* 0x5617c0 Cine_Construct */
#define SDW_MEMBERS_ScnObject \
    void AttachTo(ScnObject *parent, u8 joint, Vec3s *offset, Vec3s *rot, u32 arg, u32 arg2); /* 0x50ff14 */

#include "sdw_types.h"
#include "sdw_enums.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *inst, Animator *animator, u16 id, u32 opts); /* 0x550196 */
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "../objects/instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#include "sdw_global_views.h"

/* 0x581738 - this object's copy of the cinematic header's opcode-stride table (a header static: internal linkage, not
 * const since it is in .data; see T307's g_cineOpStride 0x5816fc), indexed by Cine_Update and the key-cursor helpers */
static u8 g_cineOpStride2[9] = {0, 8, 8, 4, 2, 2, 4, 2, 2};
#include "../app/app_main.h"
#include "progress.h"
#include "draw2d.h"
#include "time.h"
#include "sfx_volume.h"
#include "cine.h"
#include "scn_tools.h"
#include "interface.h"
#include "input.h"
#include "fade.h"
#include "../objects/camera.h"
#include "prompt.h"
extern Wolf *g_pWolf; /* 0x6cf310 */
/* The pad words below are not objects of their own: g_padMasks 0x57eb80 is &g_inputMap[4] and the two button words are
 * fields of g_pad (0x719628), all defined by the Input object (T298). Spelled as the element / fields they are, they
 * compile to the same addresses and leave no undefined symbol for the link. */
#define g_padMasks (g_inputMap + 4)           /* 0x57eb80  active-low button masks; [10] = action */
#define g_padCurButtons (g_pad.cur.buttons)   /* 0x719654 */
#define g_padPrevButtons (g_pad.prev.buttons) /* 0x71964c */
extern void *g_dialogueCurText;               /* 0x6ddfb0 */

extern TextScroll g_textScroll;
#define g_scrollTextFlags \
    (g_textScroll.flags) /* 0x6ddf10  ScrollText flags: 1 paging started, 2 finished, 0x10 page turned */
extern u32 g_gameTime;   /* 0x71b2d0 */

void Dialogue_SetBoxActive(s32 active); /* 0x539479 */
void Camera_StartScripted(ScnObject *owner, Camera *cam, u16 rotX, u16 rotY, u16 rotZ, Vec3s *pos, u16 focal, u32 mode,
                          s32 time); /* 0x55a70d */

/* 0x5626d0 - one frame of the cinematic (called while it is active). Starts the voice stream once; in dialogue mode
 * only keeps the speaker's talk/idle animation and the animation opcodes going until the text is closed. Otherwise
 * handles the skip (action or Esc; with flags & 0x1000 a skip starts the level-exit fade instead of stopping), the
 * start of dialogue mode, then steps every record whose keys are not used up and applies it, drives the scripted
 * camera, advances the clock and stops when no record is live any more.
 * The opcode bodies of cases 2, 3, 6 and 7/8 are written out here as in the original, which has them inline while
 * calling Cine_OpStartAnim, Cine_KeyAtLast etc. out of line elsewhere in the same function; case 7/8 evaluates the
 * attach/detach record like a camera focal-scale key (a copy of case 6).
 * Devices: `shown` is read before the flags & 0x800 test, as the original does; the (bool) on the 4/5 and 7/8 tests
 * keeps VC6 from laying the if/else-if chain out tests-first (a plain || puts the last else first); the animation starts
 * go through ScnBody::PlayAnim. Case 1's second test of g_dialogueCurText is its own statement rather than nested in
 * the first: nested, VC6 merged the capture-and-stop blocks of the whole function into one, where the original keeps
 * four (observed, mechanism not established). */
/* BYTES(flow): a one-case switch: the original tests with movsx / dec / je */
/* BYTES(flow): the second g_dialogueCurText test is its own statement: nested, VC6 merges the four capture-and-stop blocks into one */
/* BYTES(temp): shown is read before the flags & 0x800 test, as the original does */
/* BYTES(flow): the opcode bodies are written out here because the original has them inline in this function */
/* BYTES(slot-scope, inferred): spk / anim live in a nested block so they are allocated after the function's other locals */
void Cine::Update()
{
    s32 any = 0;
    s32 live;
    s32 shown;
    CineRecord *rec;
    Vec3s rot;
    u16 scale;
    u16 scale2;

    if (!(runFlags & CINE_RUN_VOICE_STARTED) && *resource != 0)
        Voice_PlayStream(*resource, 0);
    runFlags |= CINE_RUN_VOICE_STARTED;
    switch (dialogueMode) { /* a one-case switch: the original's movsx / dec / je */
        case 1:
            if (g_dialogueCurText != 0) {
                Dialogue_Show(g_dialogueCurText, 1);
                if ((g_dialogueShownFlags.all & DLGSHOWN_SHOWN) && (g_scrollTextFlags & SCROLLTEXT_F_CLOSED)) {
                    CaptureCamera();
                    Stop();
                    return;
                }
            }
            if (g_dialogueCurText == 0) {
                CaptureCamera();
                Stop();
                return;
            }
            {
                /* cast kept: a downcast (cinematic actors are ScnBody objects) */
                ScnBody *spk = (ScnBody *)dialogue.speaker;
                if (spk != 0) {
                    u16 anim;
                    if ((g_dialogueShownFlags.all & DLGSHOWN_SHOWN) && (g_scrollTextFlags & SCROLLTEXT_F_PAGETURNED))
                        talkStartTime = g_gameTime;
                    if (g_gameTime - talkStartTime < 0x2000)
                        anim = dialogue.talkAnim;
                    else
                        anim = dialogue.idleAnim;
                    if ((spk->anim.flags & ANIM_F_FINISHED) || spk->anim.animId != anim)
                        spk->PlayAnim(anim, 1, 1);
                }
            }
            ResetIterators();
            do {
                rec = scriptCursor;
                if (rec->count != 0) {
                    keyNext = keyCur = rec->keyCursor;
                    /* cast kept: a downcast (cinematic actors are ScnBody objects) */
                    if (rec->opcode == CINE_OP_PLAY_ANIM && !(targetDesc->obj->flags & SCN_OF_CINE_UPDATE))
                        ((ScnBody *)targetDesc->obj)->AdvanceAnim();
                }
            } while (AdvanceScriptCursor());
            return;
    }
    if (dialogueText != 0)
        Dialogue_Show(dialogueText, 1);
    shown = g_dialogueShownFlags.all & DLGSHOWN_SHOWN;
    if (!(flags & CINE_NO_INPUT)) {
        if (dialogueText != 0 && !(g_pProgress->optionFlags & OPT_GATE) &&
            ((!(~g_padMasks[10] & g_padCurButtons) && (~g_padMasks[10] & g_padPrevButtons)) ||
             g_inputMgr.escPressed == 1)) {
            CaptureCamera();
            Stop();
            return;
        }
        if (dialogueText != 0 && shown && (g_scrollTextFlags & SCROLLTEXT_F_OPEN)) {
            if (dialogue.speaker == 0) {
                ResetIterators();
                do {
                    if (targetDesc->selector == CINE_TRACK_OBJECT_SPEAKER) {
                        u32 r = targetDesc->obj->HandleMessage(0, MSG_QUERY_TALK_ANIMS, 0);
                        if (r != 0) {
                            dialogue.speaker = targetDesc->obj;
                            dialogue.idleAnim = (u16)r;
                            dialogue.talkAnim = (u16)(r >> 16);
                            break;
                        }
                    }
                } while (SkipRecords());
            }
            RunOpcodes();
            /* cast kept: a downcast (cinematic actors are ScnBody objects) */
            if (dialogue.speaker != 0)
                ((ScnBody *)dialogue.speaker)->PlayAnim(dialogue.talkAnim, 1, 0);
            if (dialogue.camSetup != 0)
                Camera_StartScripted(0, &g_camera, dialogue.camSetup->rot[0], dialogue.camSetup->rot[1],
                                     dialogue.camSetup->rot[2], &dialogue.camSetup->eye, dialogue.camSetup->focal, 0,
                                     0x1000);
            dialogueMode = 1;
            talkStartTime = g_gameTime;
            return;
        }
        if (dialogueText != 0 && shown && (g_scrollTextFlags & SCROLLTEXT_F_CLOSED)) {
            CaptureCamera();
            Stop();
            return;
        }
        if (dialogueText == 0 && !(flags & CINE_NO_WOLF_FREEZE) &&
            ((!(~g_padMasks[10] & g_padCurButtons) && (~g_padMasks[10] & g_padPrevButtons)) ||
             g_inputMgr.escPressed == 1)) {
            if (flags & CINE_LEVEL_EXIT)
                Fade_StartLevelExit(0x1000);
            else {
                CaptureCamera();
                Stop();
                return;
            }
        }
    }
    ResetIterators();
    do {
        rec = scriptCursor;
        if (rec->count != 0) {
            if ((bool)(rec->opcode == CINE_OP_FLICKER_PHASE_A || rec->opcode == CINE_OP_FLICKER_PHASE_B)) {
                /* cast kept: the script is a byte stream (record stride per opcode) */
                if (rec->keyCursor > (u16 *)((u8 *)rec + g_cineOpStride2[rec->opcode] * (rec->count - 1) + 8)) {
                    live = 0;
                } else {
                    LoadStepKeyPair();
                    if (time >= (*keyCur & 0x3fff) << 4) {
                        OpFlickerVisibility();
                        scriptCursor->keyCursor = keyNext;
                        if (!KeyPastLast())
                            LoadStepKeyPair();
                    }
                    live = 1;
                }
            } else if ((bool)(rec->opcode == CINE_OP_ATTACH_A || rec->opcode == CINE_OP_ATTACH_B)) {
                /* cast kept: the script is a byte stream (record stride per opcode) */
                if (rec->keyCursor > (u16 *)((u8 *)rec + g_cineOpStride2[rec->opcode] * (rec->count - 1) + 8)) {
                    live = 0;
                } else {
                    LoadStepKeyPair();
                    if (time >= (*keyCur & 0x3fff) << 4) {
                        OpToggleActorEntry();
                        scriptCursor->keyCursor = keyNext;
                        if (!KeyPastLast())
                            LoadStepKeyPair();
                    }
                    live = 1;
                }
            } else {
                /* cast kept: the script is a byte stream (record stride per opcode) */
                if (rec->keyCursor == (u16 *)((u8 *)rec + g_cineOpStride2[rec->opcode] * (rec->count - 1) + 8)) {
                    live = 0;
                } else {
                    LoadKeyPair();
                    if (time > (*keyNext & 0x3fff) << 4) {
                        scriptCursor->keyCursor = keyNext;
                        if (scriptCursor->opcode == CINE_OP_PLAY_ANIM)
                            OpStartAnim();
                        if (!KeyAtLast())
                            LoadKeyPair();
                    }
                    live = 1;
                }
            }
            any |= live;
            if (live) {
                switch (scriptCursor->opcode) {
                    case CINE_OP_KEY_POSITION:
                        OpKeyPosition();
                        break;
                    case CINE_OP_KEY_ROTATION:
                        KeyframeRot(&rot, keyCur + 1, keyNext + 1, (*keyCur & 0x3fff) << 4, (*keyNext & 0x3fff) << 4,
                                    *keyCur >> 14);
                        switch (targetDesc->selector) {
                            case CINE_TRACK_OBJECT:
                            case CINE_TRACK_OBJECT_SPEAKER:
                            case CINE_TRACK_STATIC:
                            case CINE_TRACK_STATIC_B:
                                if (!(targetDesc->obj->inst_flags & INST_F_ATTACHED))
                                    targetDesc->obj->rot = rot;
                                break;
                            case CINE_TRACK_CAMERA:
                                SetCamRot(&rot);
                                break;
                        }
                        break;
                    case CINE_OP_PLAY_ANIM:
                        /* cast kept: a downcast (cinematic actors are ScnBody objects) */
                        if (time == 0)
                            ((ScnBody *)targetDesc->obj)->PlayAnim(scriptCursor->keyCursor[1], 1, 0);
                        if (!(targetDesc->obj->flags & SCN_OF_CINE_UPDATE))
                            /* cast kept: the object is known to be of this class here */
                            ((ScnBody *)targetDesc->obj)->AdvanceAnim();
                        break;
                    case CINE_OP_KEY_CAMSCALAR:
                        KeyframeScalar(&scale, keyCur[1], keyNext[1], (*keyCur & 0x3fff) << 4, (*keyNext & 0x3fff) << 4,
                                       *keyCur >> 14);
                        if (targetDesc->selector == CINE_TRACK_CAMERA)
                            SetCamScalar(scale);
                        break;
                    case CINE_OP_ATTACH_A:
                    case CINE_OP_ATTACH_B:
                        KeyframeScalar(&scale2, keyCur[1], keyNext[1], (*keyCur & 0x3fff) << 4,
                                       (*keyNext & 0x3fff) << 4, *keyCur >> 14);
                        if (targetDesc->selector == CINE_TRACK_CAMERA)
                            SetCamScalar(scale2);
                        break;
                }
            } else if (scriptCursor->opcode == CINE_OP_PLAY_ANIM) {
                /* cast kept: a downcast (cinematic actors are ScnBody objects) */
                if (!(targetDesc->obj->flags & SCN_OF_CINE_UPDATE))
                    ((ScnBody *)targetDesc->obj)->AdvanceAnim();
            }
        }
    } while (AdvanceScriptCursor());
    if (hasCameraTrack && cameraHeld)
        Camera_StartScripted(0, &g_camera, camRot.x, camRot.y, camRot.z, &camPos, camFocalScale, (~flags >> 1) & 3,
                             0x1000);
    time = (g_rawTime * 1000 - startRawTime * 1000) >> 12;
    if ((flags & CINE_LEVEL_EXIT) && time >= endTriggerMs)
        Fade_StartLevelExit(0x1000);
    if (!any)
        Stop();
}

/* 0x562e00 - opcode 1: the position key, plus worldOffset with flags & 0x10; to the camera or through SetPosition */
void Cine::OpKeyPosition()
{
    Vec3s pos;
    /* cast kept: a position key's three s16 coordinates follow its time word in the u16 key stream */
    KeyframePos(&pos, (Vec3s *)(keyCur + 1), (Vec3s *)(keyNext + 1), (*keyCur & 0x3fff) << 4, (*keyNext & 0x3fff) << 4,
                *keyCur >> 14);
    switch (targetDesc->selector) {
        case CINE_TRACK_OBJECT:
        case CINE_TRACK_OBJECT_SPEAKER:
        case CINE_TRACK_STATIC:
        case CINE_TRACK_STATIC_B:
            if (!(targetDesc->obj->inst_flags & INST_F_ATTACHED)) {
                if (flags & CINE_RELATIVE_TO_WOLF) {
                    pos.x += worldOffset.x;
                    pos.y += worldOffset.y;
                    pos.z += worldOffset.z;
                }
                targetDesc->obj->SetPosition(&pos);
            }
            break;
        case CINE_TRACK_CAMERA:
            if (flags & CINE_RELATIVE_TO_WOLF) {
                pos.x += worldOffset.x;
                pos.y += worldOffset.y;
                pos.z += worldOffset.z;
            }
            SetCamPos(&pos);
            break;
    }
}

/* 0x562ee0 - opcode 2: the rotation key, written straight into the object (not when attached) or the camera */
void Cine::OpKeyRotation()
{
    Vec3s rot;
    KeyframeRot(&rot, keyCur + 1, keyNext + 1, (*keyCur & 0x3fff) << 4, (*keyNext & 0x3fff) << 4, *keyCur >> 14);
    switch (targetDesc->selector) {
        case CINE_TRACK_OBJECT:
        case CINE_TRACK_OBJECT_SPEAKER:
        case CINE_TRACK_STATIC:
        case CINE_TRACK_STATIC_B:
            if (!(targetDesc->obj->inst_flags & INST_F_ATTACHED))
                targetDesc->obj->rot = rot;
            break;
        case CINE_TRACK_CAMERA:
            SetCamRot(&rot);
            break;
    }
}

/* 0x562f70 - opcode 3, on reaching a key: starts the key's animation id (once) */
void Cine::OpStartAnim()
{
    /* cast kept: a downcast (cinematic actors are ScnBody objects) */
    ((ScnBody *)targetDesc->obj)->PlayAnim(scriptCursor->keyCursor[1], 1, 0);
}

/* 0x562fa0 - opcode 3, every frame: steps the animation unless the object is frozen (flags & 0x20) */
void Cine::OpAdvanceAnim()
{
    /* cast kept: a downcast (cinematic actors are ScnBody objects) */
    if (!(targetDesc->obj->flags & SCN_OF_CINE_UPDATE))
        ((ScnBody *)targetDesc->obj)->AdvanceAnim();
}

/* 0x562fc0 - opcodes 4/5: visibility from the parity of the key index (inverted for 5), so successive keys alternate */
/* BYTES(temp, inferred): obj is a local because the original loads targetDesc->obj once into a register of its own */
void Cine::OpFlickerVisibility()
{
    /* cast kept: the key index's parity is taken from the cursor's byte offset in the record */
    s32 on = ((u8 *)scriptCursor->keyCursor - (u8 *)scriptCursor) >> 1 & 1;
    ScnObject *obj;
    if (scriptCursor->opcode == CINE_OP_FLICKER_PHASE_B)
        on = on == 0;
    obj = targetDesc->obj;
    if (on)
        obj->flags &= ~SCN_OF_HIDDEN;
    else
        obj->flags |= SCN_OF_HIDDEN;
}

/* 0x562ff0 - opcode 6: the camera's focal scale key (camera tracks only) */
void Cine::OpKeyCamScalar()
{
    u16 v;
    KeyframeScalar(&v, keyCur[1], keyNext[1], (*keyCur & 0x3fff) << 4, (*keyNext & 0x3fff) << 4, *keyCur >> 14);
    if (targetDesc->selector == CINE_TRACK_CAMERA)
        SetCamScalar(v);
}

/* 0x563060 - opcodes 7/8: attach (or detach, by the key parity as for 4/5) every attachTable entry whose parent is
 * the track's object. attachCount is never made positive in this build, so this does nothing. */
void Cine::OpToggleActorEntry()
{
    /* cast kept: the key index's parity is taken from the cursor's byte offset in the record */
    s32 on = ((u8 *)scriptCursor->keyCursor - (u8 *)scriptCursor) >> 1 & 1;
    s32 i;
    if (scriptCursor->opcode == CINE_OP_ATTACH_B)
        on = on == 0;
    if (on) {
        for (i = 0; i < attachCount; i++)
            if (targetDesc->obj == attachTable[i].parent)
                AttachEntry(i);
    } else {
        for (i = 0; i < attachCount; i++)
            if (targetDesc->obj == attachTable[i].parent)
                DetachEntry(i);
    }
}

/* 0x5630f0 - 1 if the record's key cursor is on its last key */
s32 Cine::KeyAtLast()
{
    CineRecord *r = scriptCursor;
    /* cast kept: the script is a byte stream (record stride per opcode) */
    return r->keyCursor == (u16 *)((u8 *)r + g_cineOpStride2[r->opcode] * (r->count - 1) + 8);
}

/* 0x563120 - 1 if the key cursor has moved past the last key */
s32 Cine::KeyPastLast()
{
    CineRecord *r = scriptCursor;
    /* cast kept: the script is a byte stream (record stride per opcode) */
    return (u16 *)((u8 *)r + g_cineOpStride2[r->opcode] * (r->count - 1) + 8) < r->keyCursor;
}

/* 0x563150 - keyCur = the cursor's key, keyNext = the one after (itself at the last key) */
void Cine::LoadKeyPair()
{
    CineRecord *r = scriptCursor;
    keyCur = r->keyCursor;
    /* cast kept: the script is a byte stream (record stride per opcode) */
    if (r->keyCursor == (u16 *)((u8 *)r + g_cineOpStride2[r->opcode] * (r->count - 1) + 8))
        keyNext = keyCur;
    else
        /* cast kept: cinematic key streams are read at byte strides */
        keyNext = (u16 *)((u8 *)r->keyCursor + g_cineOpStride2[r->opcode]);
}

/* 0x563190 - the same for the stride-2 step opcodes, except that at the last key keyNext is one key past the end,
 * so the cursor can leave the record and KeyPastLast retires it */
void Cine::LoadStepKeyPair()
{
    CineRecord *r = scriptCursor;
    keyCur = r->keyCursor;
    /* cast kept: the script is a byte stream (record stride per opcode) */
    if (r->keyCursor == (u16 *)((u8 *)r + g_cineOpStride2[r->opcode] * (r->count - 1) + 8))
        keyNext = keyCur + 1;
    else
        /* cast kept: cinematic key streams are read at byte strides */
        keyNext = (u16 *)((u8 *)r->keyCursor + g_cineOpStride2[r->opcode]);
}
