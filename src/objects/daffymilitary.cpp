/*
 * T072 - original object DaffyMilitary.cpp (guessed name): .text 0x43b2e0-0x43bf14, .rdata 0x574da4-0x574dc8 (vtable),
 * .data 0x57a6f4-0x57a718 (the cinematic header's stride-table copy, the answer voices, the answer handlers),
 * .bss 0x6cc874-0x6cc878 (g_pDaffyMilitary). The two tables lie in .data in the original, so they are defined
 * non-const.
 *
 * DaffyMilitary (class 57, vtable 0x574da4, sizeof 0x124) - Daffy in costume in Level 5 (disc Lvl-06), frozen in the
 * level's IceCube. Melting the cube with the hair dryer frees him: the cube sends him MSG_THAW (0x41), Ralph is stripped
 * of his costume and held item (Wolf msg 0x415) and the release cinematic CINE plays. Once free he talks: MSG_USE opens
 * his question box (TALKABOUTTXT, TALKABOUTNBCHOICE answers), and after an explosion has blackened him the next talk
 * gives the PUNISHTEXT line instead, once. SheepD3D.exe 0x43b2e0-0x43bf13: the three answer handlers of his question
 * box, PostLoadInit, the face-Ralph and talk-anim helpers, Update, HandleMessage, Reset, SetState and the factory.
 * The file starts at 0x43b2e0 (int3 padding after DaffyLevel09_Create closes the previous file); the first handler,
 * 0x43b2e0, reads g_pDaffyMilitary and is entry 0 of this class's handler table 0x57a70c.
 *
 * States (+0x120): 0 frozen in the cube, 1 free, 2 release cinematic, 3 question box, 4 answer, 5 punish line.
 * Devices that only pin the original code generation: the inline helpers below (their names are not recovered; each
 * one is here because its expansion gives the original's stack temporaries), SDW_PROP (offsetof: as an argument it is
 * not a plain constant, so the property inline gets a temp for it), and `result` in Update, a named byte local.
 */
/* BYTES: layout. */
/* BYTES(layout): the Cine.h header static: every object including the cinematic header carries this copy in its .data, referenced or not */

#define SDW_MEMBERS_ScnObject            \
    static void *operator new(u32 size); \
    void SetFacing(s16 f);               \
    void SetUpdateMode(u8 mode);         \
    /* inline: the handle is a stack temp (0x43bb5d) */


#define SDW_MEMBERS_DaffyMilitary    \
    char *AnswerText(u8 i)           \
    {                                \
        return dialog.answerText[i]; \
    } /* inline: i in a register, the result a temp */
#include "sdw_types.h"
#include "sdw_enums.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *inst, Animator *animator, u16 id, u32 opts); /* 0x550196 */
#include "../engine/sound_mgr.h"
#include "../engine/approach.h"
#include "../engine/text.h"
#include "../engine/prompt.h"
#include "../engine/cine.h"
#include "../engine/game_state.h"
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#define SDW_INLINE_SCNOBJECT_FACING 1
#define SDW_INLINE_SCNOBJECT_SETFACING_S16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_FACING
#undef SDW_INLINE_SCNOBJECT_SETFACING_S16
#define SDW_INLINE_SCNOBJECT_ENABLEBOXCOLLIDE_S32 1
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8 1
#define SDW_INLINE_SCNOBJECT_STOPSOUND_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_ENABLEBOXCOLLIDE_S32
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8
#undef SDW_INLINE_SCNOBJECT_STOPSOUND_U16
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_SCNBODY_ANIMID 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMID
#define SDW_INLINE_SHADOW_SETVISIBLE_S32 1
#include "../engine/shadow_inlines.h"
#undef SDW_INLINE_SHADOW_SETVISIBLE_S32
#define SDW_INLINE_CINE_ISACTIVE 1
#include "../engine/cine_inlines.h"
#undef SDW_INLINE_CINE_ISACTIVE
#include "scenaric_props.h"

/* cast kept: offsetof written out (there is no CRT header), the address of a member of a null object */
#define SDW_PROP(T, f) \
    ((u32) & ((T *)0)->f) /* offsetof: as a u32 argument it is not a plain constant, so the inline gets a temp */

typedef void (*MenuHandler)(u8 msg, MenuPage *self);

s32 Scenaric_FindByClass(u16 classId, ScnObject **out, s32 max);             /* 0x5145c5 */
void Dialogue_SetBoxActive(s32 active);                                      /* 0x539479 */
void Dialogue_Reset();                                                       /* 0x539507 */
u8 Dialogue_Say(const char *text, s32 voiceId, ScnObject *speaker, u32 arg); /* 0x539893 */
void Dialog_Begin(DialogBox *dlg, u32 firstStringId, s8 answerCount, const MenuHandler *handlers,
                  ScnObject *sender); /* 0x53ec02 */

extern Wolf *g_pWolf; /* 0x6cf310 */
extern s32 g_dt;      /* 0x71b300 */

/* 0x57a6f4 - the cinematic header's static copy of the 9-byte opcode stride table (src/engine/cine1.cpp,
 * g_cineOpStride 0x5816fc). The original header defined it static, so every object including it carries its own
 * unreferenced copy at the head of its .data; defined here in its place. */
static u8 s_cineOpStride[9] = {0, 8, 8, 4, 2, 2, 4, 2, 2};
void DaffyMilitary_DrawAnswer0(u8 msg, MenuPage *self);
void DaffyMilitary_DrawAnswer1(u8 msg, MenuPage *self);
void DaffyMilitary_DrawAnswer2(u8 msg, MenuPage *self);
u32 g_daffyMilitaryAnswerVoices[3] = {VOICE_TXT_LVL0602, VOICE_TXT_LVL0603,
                                      VOICE_TXT_LVL0604}; /* 0x57a700  the voice of each answer */
MenuHandler g_daffyMilitaryAnswerHandlers[3] = {          /* 0x57a70c */
                                                DaffyMilitary_DrawAnswer0, DaffyMilitary_DrawAnswer1,
                                                DaffyMilitary_DrawAnswer2};

DaffyMilitary *g_pDaffyMilitary; /* 0x6cc874  set by PostLoadInit, read by the answer handlers */

/* A designer property of the WAR record: the dword at record + 0x14 + offset. Inlined everywhere (the offset is a stack
 * temp, 0x43b421); Scn_GetPropObject 0x515630 has the same read out of line. */
#define SDW_INLINE_FREE_SCN_GETPROPU32_U16_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_SCN_GETPROPU32_U16_U32

/* Cine_Start with no boxes and no dialogue block: the three variable arguments go through stack temps (0x43bd67-0x43bd89). */
inline void Cine_Play(u32 id, u32 startFlags, void *text)
{
    g_cinePlayer.Start(id, startFlags, 0, 0, text, 0);
}

/* Set / clear bits of a 16-bit flag word: the word's address is a stack temp and the mask is loaded into a register
 * (0x43b991-0x43b9d2: `mov ecx,0x10 ... or eax,ecx`, `mov eax,0x10; not eax; xor ecx,ecx; mov cx,ax ... and eax,ecx`). */
#define SDW_INLINE_FREE_FLAGS16_SET_U16_U16 1
#include "instance_inlines.h"
#undef SDW_INLINE_FREE_FLAGS16_SET_U16_U16

#define SDW_INLINE_FREE_FLAGS16_CLEAR_U16_U16 1
#include "instance_inlines.h"
#undef SDW_INLINE_FREE_FLAGS16_CLEAR_U16_U16

#define SDW_INLINE_SCNOBJECT_SETTINTOVERRIDE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETTINTOVERRIDE_S32

/* 0x43b2e0 - MenuHandler of the question box's first answer: on msg 0 (draw) prints it, blinking while selected. */
void DaffyMilitary_DrawAnswer0(u8 msg, MenuPage *self)
{
    switch (msg) {
        case MENU_MSG_DRAW:
            Text_PrintfStyled(TEXTALIGN_CENTER, self->selected, g_pDaffyMilitary->AnswerText(0));
    }
}

/* 0x43b327 - the same for the second answer. */
void DaffyMilitary_DrawAnswer1(u8 msg, MenuPage *self)
{
    switch (msg) {
        case MENU_MSG_DRAW:
            Text_PrintfStyled(TEXTALIGN_CENTER, self->selected, g_pDaffyMilitary->AnswerText(1));
    }
}

/* 0x43b371 - the same for the third answer. */
void DaffyMilitary_DrawAnswer2(u8 msg, MenuPage *self)
{
    switch (msg) {
        case MENU_MSG_DRAW:
            Text_PrintfStyled(TEXTALIGN_CENTER, self->selected, g_pDaffyMilitary->AnswerText(2));
    }
}

/* 0x43b3bb - vtable +0x00: publishes g_pDaffyMilitary, reads the properties, finds the ice cube, starts frozen (state
 * 0), keeps updating in cinematics (update mode 3) and snaps to the ground. */
void DaffyMilitary::PostLoadInit()
{
    u16 *rec = record;
    g_pDaffyMilitary = this;
    sound = 0;
    wolfFrozen = 0;
    punished = 0;
    burnt = 0;
    talkAboutText = Scn_GetPropU32(rec, SDW_PROP(DaffyMilitaryProps, TALKABOUTTXT));
    talkAboutChoiceCount = (u8)Scn_GetPropU32(rec, SDW_PROP(DaffyMilitaryProps, TALKABOUTNBCHOICE));
    iceCube = 0;
    Scenaric_FindByClass(CLASSID_ICECUBE, &iceCube, 1);
    cineId = (u16)Scn_GetPropU32(rec, SDW_PROP(DaffyMilitaryProps, CINE));
    cineFlags = Scn_GetPropU32(rec, SDW_PROP(DaffyMilitaryProps, FLAGCINE));
    if (cineFlags & CINE_HAS_TEXT)
        cineText = Text_GetClassString((u8)Scn_GetPropU32(rec, SDW_PROP(DaffyMilitaryProps, CINETXT)));
    else
        cineText = 0;
    punishText = Text_GetClassString((u8)Scn_GetPropU32(rec, SDW_PROP(DaffyMilitaryProps, PUNISHTEXT)));
    SetState(DAFFYMIL_ST_HIDDEN);
    SetUpdateMode(SCN_UPD_CINE);
    SnapToGround(0);
}

/* 0x43b606 - turn toward Ralph at 0x1800 units/s (frame-rate scaled by Math_StepAngleTowards). */
void DaffyMilitary::FaceWolf()
{
    SetFacing(Math_StepAngleTowards(Facing(), HeadingTo(&g_pWolf->pos), 0x1800));
}

/* 0x43b652 - while the letterbox is up: after 2 s (0x2000) in the state, switch to the talk loop, anim 6. */
void DaffyMilitary::UpdateTalkAnim()
{
    if (g_letterboxState) {
        talkTimer += g_dt;
        if (talkTimer >= 0x2000 && AnimId() != AMILIT01_ANIM_TALK1)
            PlayAnim(AMILIT01_ANIM_TALK1, 1, 1);
    }
}

/* 0x43b6f0 - vtable +0x04: 2 waits for the release cinematic to end, 3 runs the question box (TRIANGLE closes it, CROSS
 * picks an answer), 4 replays the chosen answer, 5 says the punish line and then unfreezes Ralph; then the animation. */
void DaffyMilitary::Update()
{
    u8 result;
    switch (state) {
        case DAFFYMIL_ST_CINEMATIC:
            if (!g_cinePlayer.IsActive()) {
                Dialogue_SetBoxActive(0);
                SetState(DAFFYMIL_ST_IDLE);
            }
            break;
        case DAFFYMIL_ST_DIALOG:
            FaceWolf();
            result = Dialog_Update(&dialog);
            if (result == DIALOG_PICKED)
                SetState(DAFFYMIL_ST_ANSWER);
            else if (result == DIALOG_CLOSED)
                SetState(DAFFYMIL_ST_IDLE);
            break;
        case DAFFYMIL_ST_ANSWER:
            FaceWolf();
            UpdateTalkAnim();
            if (!Dialog_UpdateAnswer(&dialog, g_daffyMilitaryAnswerVoices))
                SetState(DAFFYMIL_ST_IDLE);
            break;
        case DAFFYMIL_ST_PUNISH:
            FaceWolf();
            UpdateTalkAnim();
            if (!Dialogue_Say(punishText, VOICE_CIN_LVL_0105, this, 0)) {
                g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0);
                Dialogue_Reset();
                SetState(DAFFYMIL_ST_IDLE);
            }
            break;
    }
    AdvanceAnim();
}

/* 0x43b83b - vtable +0x10. While free (state 1): msg 2 (context query) offers CTX_TALK (8) to Ralph, MSG_USE from Ralph
 * opens the question box, or the punish line once after he has been blackened; msg 0 with arg 0 (explosion) blackens
 * him (tint 0x1000 black, animation frozen). MSG_THAW (0x41) from the IceCube frees him; msg 0xE (another object took
 * the Wolf freeze) clears wolfFrozen. */
s32 DaffyMilitary::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    switch (msgId) {
        case MSG_QUERY_ACTION:
            if (sender->GetClassId() == CLASSID_WOLF && state == DAFFYMIL_ST_IDLE)
                return CTX_TALK;
            break;
        case MSG_USE:
            if (sender->GetClassId() == CLASSID_WOLF && state == DAFFYMIL_ST_IDLE) {
                if (burnt && !punished) {
                    SetState(DAFFYMIL_ST_PUNISH);
                    punished = 1;
                } else
                    SetState(DAFFYMIL_ST_DIALOG);
                return 1;
            }
            break;
        case MSG_THAW:
            if (sender->GetClassId() == CLASSID_ICECUBE && state != DAFFYMIL_ST_CINEMATIC)
                SetState(DAFFYMIL_ST_CINEMATIC);
            return 1;
        case MSG_KILL:
            /* cast kept: HandleMessage's arg is a void *; MSG_KILL passes the WolfKillType in it (0 = explosion) */
            switch ((u32)arg) {
                case KILL_GENERIC:
                    if (state == DAFFYMIL_ST_IDLE) {
                        tintColor = 0;
                        tintAmount = 0x1000;
                        SetTintOverride(1);
                        burnt = 1;
                        anim.speed = 0;
                    }
            }
            return 1;
        case MSG_FREEZE:
            wolfFrozen = 0;
            return 1;
    }
    return 0;
}

/* 0x43ba74 - vtable +0x14 (level restart): a Daffy that is not free goes back into the cube (the cube gets msg 0x32)
 * and to state 0; a free one stays free. Clears the burnt look and the punish latch, stops the sound. */
void DaffyMilitary::Reset()
{
    if (state != DAFFYMIL_ST_IDLE) {
        iceCube->HandleMessage(this, MSG_ICECUBE_RESET, 0);
        SetState(DAFFYMIL_ST_HIDDEN);
    } else
        SetState(DAFFYMIL_ST_IDLE);
    tintColor = 0;
    tintAmount = 0;
    SetTintOverride(0);
    punished = 0;
    burnt = 0;
    if (sound) {
        StopSound(sound);
        sound = 0;
    }
    wolfFrozen = 0;
}

/* 0x43bb97 - enter a state (anim speed back to 1.0, talkTimer = 0): 0 frozen (anim 4, no box collision, no shadow),
 * 1 free (anim 0xb loop), 2 released (strip Ralph, letterbox on, collision and shadow back, cinematic CINE),
 * 3 opens the question box, 4 anim 0xb, 5 anim 0xb and freezes Ralph (msg 0xE). */
void DaffyMilitary::SetState(u8 newState)
{
    anim.speed = 0x1000;
    state = newState;
    talkTimer = 0;
    switch (state) {
        case DAFFYMIL_ST_HIDDEN:
            EnableBoxCollide(0);
            shadow.SetVisible(0);
            PlayAnim(AMILIT01_ANIM_STAND1, 0, 0);
            break;
        case DAFFYMIL_ST_IDLE:
            PlayAnim(AMILIT01_ANIM_STAND2, 1, 1);
            break;
        case DAFFYMIL_ST_CINEMATIC:
            g_pWolf->HandleMessage(this, MSG_WOLF_STRIP, 0);
            Dialogue_SetBoxActive(1);
            EnableBoxCollide(1);
            shadow.SetVisible(1);
            Cine_Play(cineId, cineFlags, cineText);
            break;
        case DAFFYMIL_ST_DIALOG:
            Dialog_Begin(&dialog, talkAboutText, talkAboutChoiceCount, g_daffyMilitaryAnswerHandlers, this);
            break;
        case DAFFYMIL_ST_ANSWER:
            PlayAnim(AMILIT01_ANIM_STAND2, 1, 1);
            break;
        case DAFFYMIL_ST_PUNISH:
            PlayAnim(AMILIT01_ANIM_STAND2, 1, 1);
            g_pWolf->HandleMessage(this, MSG_FREEZE, 0);
            break;
    }
}

/* 0x43bea4 - the class factory for CLASSID 57 "DaffyMilitary": new DaffyMilitary (the base vtables in turn, then
 * DaffyMilitary's), then ScnMobile_Init(record, 0) through vtable slot +0x20. */
ScnObject *DaffyMilitary_Create(void *record)
{
    DaffyMilitary *obj = new DaffyMilitary;
    obj = (DaffyMilitary *)obj->Init(record, 0); /* cast kept: Init returns the object as its base class */
    return obj;
}
