/* match-flags: /O2 /Oy- /Ob2 */
/*
 * T313 - original object guessed as Cine.cpp (data/tu_map.json). Ranges: .data 0x581744-0x581750 (this object's copy of
 * the opcode-stride table, g_cineOpStride3), .bss 0x71c438-0x71cdb0 (g_cinePlayer); code: COMDATs only (/O2 /Oy- /Ob2),
 * 0x5631d0-0x5634c0 (Cine_Start, Cine_Stop, Cine_Rewind, Cine_InitRecordCursors, then the g_cinePlayer initialiser 0x5634b0,
 * .CRT$XCU 0x5790c8).
 * match-init: Cine_StaticInit_g_cinePlayer
 * The initialiser Cine_StaticInit_g_cinePlayer is what VC6 /O2 /Ob2 generates for the definition `Cine g_cinePlayer;`
 * at the end of the file.
 *
 * The stride table is defined here (a header static); the pad word declarations are macros (see T312) though
 * nothing here reads them. Cine_Start and Cine_Stop call Rewind and Sound_SetSfxVolume out of line: Rewind is defined
 * after them (VC6 inlines only a body it has already seen) and Sound_SetSfxVolume is in the next object (T314).
 * `tools/layout.py --tu T313` counts this object 16 bytes short because it leaves out the initialiser: the /O2
 * compiler emits it as a COMDAT whose symbol `_$E2` is STATIC, and the tool gives a COMDAT to the object that first has
 * its name in the link census (only T001's `_$E2` at 0x401030 is in it). LINK never binds a static symbol across
 * objects (the exe itself keeps T001's _$E2 at 0x401030 and T315's at 0x563500); counted that way, the object's layout
 * is identical.
 */
/* BYTES: inline, layout, switches, view. */
/* BYTES(inline): ScnBody::PlayAnim (SDW_MEMBERS_ScnBody inline): source-only inline: Cine_Update's register allocation only comes out with the call written through it */
/* BYTES(view): spelled as the element / fields they are (macros), not separate objects: same addresses, no undefined symbol at link */
/* BYTES(layout): defined after Cine_InitRecordCursors so its /O2 initialiser follows it in the image */
/* BYTES(layout): the Cine.h header static: every object including the cinematic header carries this copy in its .data, referenced or not */
/* BYTES(switches): built with /O2 /Oy- /Ob2, not the project recipe; the file header says why */

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

/* 0x581744 - this object's copy of the cinematic header's opcode-stride table (a header static: internal linkage, not
 * const since it is in .data; see T307's g_cineOpStride 0x5816fc), indexed by Cine_Rewind and Cine_InitRecordCursors */
static u8 g_cineOpStride3[9] = {0, 8, 8, 4, 2, 2, 4, 2, 2};
#include "../app/app_main.h"
#include "progress.h"
#include "draw2d.h"
#include "time.h"
#include "sfx_volume.h"
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
extern u32 g_scrollTextFlags; /* 0x6ddf10  ScrollText flags: 1 paging started, 2 finished, 0x10 page turned */
extern u32 g_gameTime;        /* 0x71b2d0 */

void Dialogue_SetBoxActive(s32 active); /* 0x539479 */
void Camera_StartScripted(ScnObject *owner, Camera *cam, u16 rotX, u16 rotY, u16 rotZ, Vec3s *pos, u16 focal, u32 mode,
                          s32 time); /* 0x55a70d */

/* 0x5631d0 - starts cinematic id: freezes the Wolf (msg 0xe) unless flags & 0x200, takes the level's default boxes
 * when none are given and the flags ask for them, loads and resolves the script, with flags & 0x10 plays it relative
 * to the Wolf (worldOffset = Wolf position - his first position key; no key clears the flag), opens the letterbox,
 * with flags & 0x4000 mutes the sound effects, and rewinds. */
/* BYTES(layout): Rewind is defined after Start / Stop so they call it out of line (VC6 inlines only a body it has already seen) */
void Cine::Start(u32 id, u32 startFlags, Box *box, Box *sheep, void *text, const CineDialogue *dlg)
{
    Vec3s *key;
    cineId = id;
    flags = startFlags;
    dialogueText = text;
    active = 1;
    finished = 0;
    wolfReady = 0;
    if (g_pWolf != 0 && !(startFlags & CINE_NO_WOLF_FREEZE))
        wolfFrozen = g_pWolf->HandleMessage(0, MSG_FREEZE, 0);
    else
        wolfFrozen = 0;
    unk8dc = 0;
    if (sheep == 0 && (flags & CINE_AUTO_SHEEP_BBOX))
        sheep = FindCinSheepBBox();
    if (box == 0 && (flags & CINE_AUTO_CIN_BBOX))
        box = FindCinBBox();
    cineBox = box;
    sheepBox = sheep;
    SetDialogueParams(dlg);
    Load();
    key = FindFirstPosKeyFor(g_pWolf);
    if ((flags & CINE_RELATIVE_TO_WOLF) && key == 0) {
        flags &= ~CINE_RELATIVE_TO_WOLF;
    } else if (flags & CINE_RELATIVE_TO_WOLF) {
        Vec3s wp = g_pWolf->pos;
        worldOffset.x = wp.x - key->x;
        worldOffset.y = wp.y - key->y;
        worldOffset.z = wp.z - key->z;
    }
    if ((flags & CINE_LETTERBOX) || dialogueText != 0)
        Dialogue_SetBoxActive(1);
    if (flags & CINE_SUSPEND_ACTIVE_SCRIPT) {
        savedSfxVolume = g_sfxVolume;
        Sound_SetSfxVolume(0);
    }
    Rewind();
    attachCount = 0;
    cameraHeld = 1;
}

/* 0x563350 - ends the cinematic: unfreezes the Wolf, Finish (restores the actors, the level exit with flags & 0x1000),
 * restores the sound effects and the letterbox, marks the player idle, releases the camera and closes any prompt */
void Cine::Stop()
{
    if (!(flags & CINE_NO_WOLF_FREEZE) && wolfFrozen)
        g_pWolf->HandleMessage(0, MSG_UNFREEZE, 0);
    Finish();
    if (flags & CINE_SUSPEND_ACTIVE_SCRIPT)
        Sound_SetSfxVolume(savedSfxVolume);
    if ((flags & CINE_LETTERBOX) || dialogueText != 0)
        Dialogue_SetBoxActive(0);
    active = 0;
    finished = 1;
    wolfReady = 0;
    if (hasCameraTrack && cameraHeld)
        Camera_ReleaseAny();
    Prompt_End();
}

/* 0x5633f0 - clock to 0, every key cursor to its record's first key; endTriggerMs = the latest last-key time - 1 s.
 * `last` is zeroed after the clock stores and `rec` is kept from before the keyCursor store while the count and opcode
 * are re-read through scriptCursor: that is the original's load order (0x563410..0x56343a). */
void Cine::Rewind()
{
    s32 last;
    s32 off, t;
    time = 0;
    startRawTime = g_rawTime;
    last = 0;
    ResetIterators();
    do {
        CineRecord *rec = scriptCursor;
        rec->keyCursor = (u16 *)(rec + 1); /* cast kept: a record's keys follow its 8-byte header in the byte stream */
        if (scriptCursor->count != 0)
            off = g_cineOpStride3[scriptCursor->opcode] * (scriptCursor->count - 1);
        else
            off = 0;
        /* cast kept: the keys are a byte stream after the header, with a stride that depends on the opcode */
        t = *(u16 *)((u8 *)rec + off + 8) & 0x3fff;
        if (t > last)
            last = t;
    } while (AdvanceScriptCursor());
    endTriggerMs = last * 16 - 1000;
}

/* 0x563470 - every key cursor to its record's last key (RunOpcodes evaluates the end state); `off` as a statement,
 * not a ?: inside the address, gives the original's operand order */
void Cine::InitRecordCursors()
{
    s32 off;
    ResetIterators();
    do {
        CineRecord *rec = scriptCursor;
        if (rec->count != 0)
            off = g_cineOpStride3[rec->opcode] * (rec->count - 1);
        else
            off = 0;
        /* cast kept: the keys are a byte stream after the header, with a stride that depends on the opcode */
        rec->keyCursor = (u16 *)((u8 *)rec + off + 8);
    } while (AdvanceScriptCursor());
}

/* 0x71c438 - the one cinematic player. Its dynamic initialiser 0x5634b0 (_initterm entry 0x5790c8, table name
 * Cine_StaticInit_g_cinePlayer, see the match-init line) is what VC6 /O2 /Ob2 generates for this definition: one
 * function (under /Ob1 it would be $E2 jumping to $E1). Defined after Cine_InitRecordCursors, which its initialiser
 * follows in the image. */
Cine g_cinePlayer;
