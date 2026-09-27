/*
 * T068 - original object DaffyElf.cpp (guessed name): .text 0x4360e0-0x436902, .rdata 0x574d14-0x574d38 (vtable),
 * .data 0x57a68c-0x57a6a4 (the cinematic header's stride-table copy, then g_daffyElfChoiceHandlers),
 * .bss 0x6cc860-0x6cc86c.
 * .bss ORDER: VC6 lays out a file's UNinitialised globals by a hash of their names (h = (h << 2) + (h >> 4) + c;
 * bucket (h ^ h >> 16) & 1023, ascending; last-declared first inside a bucket), which would put g_pDaffyElf first. The
 * two are written with zero initializers instead, which keeps them in .bss but in definition order, so the
 * descriptive names are kept.
 *
 * DaffyElf (class 55, vtable 0x574d14, sizeof 0x128) - Daffy in costume at the start of Level 4 (disc Lvl-04): he
 * calls Ralph over, asks his question in a Dialog box (the flute lesson) and, when Ralph walks into CINEBOX, plays his
 * cinematic. SheepD3D.exe 0x4360e0-0x436901: the three choice handlers of his question box, PostLoadInit, Update,
 * HandleMessage, Reset, SetState and the class factory. The file starts at 0x4360e0 (int3 padding after Crowd_Create
 * 0x436071 closes the previous file; the first handler is entry 0 of the table at 0x57a698 that SetState hands to
 * Dialog_Begin).
 *
 * States (+0x7e): 0 calling (faces Ralph, sound 0x25), 1 question box open, 2 waiting for the cinematic to end, 3 waiting
 * for Ralph inside CINEBOX (x/z only), 4 replaying the chosen answer. The level starts in 3.
 * Devices that only pin the original code generation: the inline helpers below (their names are not recovered; each
 * one is here because its expansion gives the original's stack temporaries), and `result` in Update, a named byte
 * local.
 */
/* BYTES: cast, flow, layout. */
/* BYTES(layout): the Cine.h header static: every object including the cinematic header carries this copy in its .data, referenced or not */
/* BYTES(layout): written '= 0' only to keep definition order in .bss */

#define SDW_MEMBERS_ScnObject            \
    static void *operator new(u32 size); \
    void SetFacing(s16 f);               \
    /* inline: the handle is a stack temp (0x436794) */


#define SDW_MEMBERS_DaffyElf         \
    char *AnswerText(u8 i)           \
    {                                \
        return dialog.answerText[i]; \
    } /* inline: i in a register (scaled: a narrow type), the result a temp */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *inst, Animator *animator, u16 id, u32 opts); /* 0x550196 */
#include "../engine/sound_mgr.h"
#include "../engine/scn_tools.h"
#include "../engine/text.h"
#include "../engine/prompt.h"
#include "../engine/cine.h"
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_SETFACING_S16 1
#define SDW_INLINE_SCNOBJECT_STOPSOUND_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_SETFACING_S16
#undef SDW_INLINE_SCNOBJECT_STOPSOUND_U16
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#define SDW_INLINE_SHADOW_INVALIDATE 1
#include "../engine/shadow_inlines.h"
#undef SDW_INLINE_SHADOW_INVALIDATE
#define SDW_INLINE_CINE_ISACTIVE 1
#define SDW_INLINE_CINE_ISFINISHED 1
#include "../engine/cine_inlines.h"
#undef SDW_INLINE_CINE_ISACTIVE
#undef SDW_INLINE_CINE_ISFINISHED

typedef void (*MenuHandler)(u8 msg, MenuPage *self);

u16 Sound_Play(u16 soundId, void *owner, u16 volume, u8 flags, s32 rate); /* 0x5491b8 */
s32 Rand_Bounded(s32 bound);                                              /* 0x561219 */
void Dialog_Begin(DialogBox *dlg, u32 firstStringId, s8 answerCount, const MenuHandler *handlers,
                  ScnObject *sender); /* 0x53ec02 */

extern Wolf *g_pWolf; /* 0x6cf310 */

/* 0x57a68c - the cinematic header's static copy of the 9-byte opcode stride table (src/engine/cine.cpp,
 * g_cineOpStride 0x5816fc). The original header defined it static, so every object including it carries its own
 * unreferenced copy at the head of its .data; defined here in its place. */
static u8 s_cineOpStride[9] = {0, 8, 8, 4, 2, 2, 4, 2, 2};

/* This file's globals, in their .bss order (zero initializers: .bss in definition order, see the header). */
u32 g_daffyElfAnswerVoices[2] = {0}; /* 0x6cc860  {0x35, 0xb} from PostLoadInit: the voice of each answer */
DaffyElf *g_pDaffyElf = 0;           /* 0x6cc868  set by PostLoadInit, read by the choice handlers */

/* A designer property of the WAR record: the dword at record + 0x14 + offset (the offsets are DaffyElfProps', see
 * src/include/scenaric_props.h). Inlined; the offset is a stack temp (0x4361e9), which it is with a u32 parameter (an
 * s32 one is folded into the address; observed, the mechanism is not established). Scn_GetPropBox 0x5155b5 reads the
 * same way out of line. The s32 return matters as well: assigned to an int local the value is written straight into
 * it, while a member or a narrower target goes through a result temp, which is what the original does. */
/* BYTES(cast): the s32 return: a member or narrower target then goes through the original's result temp */
#define SDW_INLINE_FREE_SCN_GETPROPS32_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_SCN_GETPROPS32_VOID_U32

/* Whether p lies inside box in x and z (the vertical is not tested): both parameters are stack temps (0x436382). */
#define SDW_INLINE_FREE_BOX_CONTAINSPOINTXZ_BOX_VEC3S 1
#include "../engine/coll_box_inlines.h"
#undef SDW_INLINE_FREE_BOX_CONTAINSPOINTXZ_BOX_VEC3S

/* Cine_Start with no dialogue block: every argument goes through a stack temp (0x4363f6-0x43642f). */
#define SDW_INLINE_FREE_CINE_PLAY_U32_U32_BOX_BOX_VOID 1
#include "../engine/cine_inlines.h"
#undef SDW_INLINE_FREE_CINE_PLAY_U32_U32_BOX_BOX_VOID

/* 0x4360e0 - MenuHandler of the question box's first answer: on msg 0 (draw) prints it, blinking while selected. */
void DaffyElf_DrawChoice0(u8 msg, MenuPage *self)
{
    switch (msg) {
        case MENU_MSG_DRAW:
            Text_PrintfStyled(TEXTALIGN_CENTER, self->selected, g_pDaffyElf->AnswerText(0));
    }
}

/* 0x436127 - the same for the second answer. */
void DaffyElf_DrawChoice1(u8 msg, MenuPage *self)
{
    switch (msg) {
        case MENU_MSG_DRAW:
            Text_PrintfStyled(TEXTALIGN_CENTER, self->selected, g_pDaffyElf->AnswerText(1));
    }
}

/* 0x436171 - the same for the third answer. */
void DaffyElf_DrawChoice2(u8 msg, MenuPage *self)
{
    switch (msg) {
        case MENU_MSG_DRAW:
            Text_PrintfStyled(TEXTALIGN_CENTER, self->selected, g_pDaffyElf->AnswerText(2));
    }
}

/* 0x57a698 - the question box's choice handlers, handed to Dialog_Begin by SetState(1). */
MenuHandler g_daffyElfChoiceHandlers[3] = {DaffyElf_DrawChoice0, DaffyElf_DrawChoice1, DaffyElf_DrawChoice2};

/* 0x4361bb - vtable +0x00: reads the properties, publishes g_pDaffyElf and the answer voices, starts in state 3. */
void DaffyElf::PostLoadInit()
{
    void *rec = record;
    SetState(DAFFYELF_ST_WAIT_CINE_BOX);
    g_pDaffyElf = this;
    soundHandle = 0;
    cineId = Scn_GetPropS32(rec, 8);
    cineFlags = Scn_GetPropS32(rec, 0x10);
    cineTextIndex = Scn_GetPropS32(rec, 0x18);
    cineText = Text_GetClassString((u8)cineTextIndex);
    cineBox = Scn_GetPropBox(rec, 0xc);
    cineSheepBox = Scn_GetPropBox(rec, 0x14);
    answerTextIndex = Scn_GetPropS32(rec, 4);
    answerCount = (u8)Scn_GetPropS32(rec, 0);
    PlayAnim(ADELFE01_ANIM_STAND, 1, 0);
    g_daffyElfAnswerVoices[0] = VOICE_CINFL_LVL0402;
    g_daffyElfAnswerVoices[1] = VOICE_CIN_LVL_0103;
}

/* 0x436329 - vtable +0x04: the state machine, then the animation. */
/* BYTES(flow): jumps straight to the animation step: one jmp more than a fall-through, as the original */
void DaffyElf::Update()
{
    u8 result;
    switch (state) {
        case DAFFYELF_ST_WAIT_CINE_BOX:
            SetFacing(HeadingTo(&g_pWolf->pos));
            if (!g_cinePlayer.IsActive()) {
                if (Box_ContainsPointXZ(cineBox, &g_pWolf->pos)) {
                    Cine_Play(cineId, cineFlags, cineBox, cineSheepBox, cineText);
                    SetState(DAFFYELF_ST_CINEMATIC);
                    break; /* the original jumps straight to the animation from here, one jmp more than a fall-through */
                }
            }
            break;
        case DAFFYELF_ST_CINEMATIC:
            if (g_cinePlayer.IsFinished())
                SetState(DAFFYELF_ST_WATCH);
            break;
        case DAFFYELF_ST_WATCH:
            SetFacing(HeadingTo(&g_pWolf->pos));
            if (!soundHandle)
                soundHandle = Sound_Play(SND_FLUTE_TUNE, this, 0x7f, SNDF_POSITIONAL, 0x1000);
            break;
        case DAFFYELF_ST_DIALOG:
            result = (u8)Dialog_Update(&dialog);
            if (result == DIALOG_PICKED)
                SetState(DAFFYELF_ST_ANSWER);
            else if (result == DIALOG_CLOSED)
                SetState(DAFFYELF_ST_WATCH);
            break;
        case DAFFYELF_ST_ANSWER:
            /* the answer is being replayed: keep the shadow projected and pick a new talk loop at each anim end */
            shadow.Invalidate();
            if (AnimFlags(ANIM_F_FINISHED)) {
                switch (Rand_Bounded(3)) {
                    case 0:
                        PlayAnim(ADELFE01_ANIM_TALK1, 1, 1);
                        break;
                    case 1:
                        PlayAnim(ADELFE01_ANIM_TALK2, 1, 1);
                        break;
                    case 2:
                        PlayAnim(ADELFE01_ANIM_TALK4, 1, 1);
                        break;
                }
            }
            if (!Dialog_UpdateAnswer(&dialog, g_daffyElfAnswerVoices)) {
                PlayAnim(ADELFE01_ANIM_STAND, 1, 0);
                SetState(DAFFYELF_ST_WATCH);
            }
            break;
    }
    AdvanceAnim();
}

/* 0x4366cf - vtable +0x10. Outside cinematics: msg 1 (talk) opens the question box; msg 2 (context query) offers
 * CTX_TALK (8) to Ralph (class 0) while Daffy is calling. */
s32 DaffyElf::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    if (!g_cinePlayer.IsActive()) {
        switch (msgId) {
            case MSG_USE:
                SetState(DAFFYELF_ST_DIALOG);
                break;
            case MSG_QUERY_ACTION:
                if (sender->GetClassId() == CLASSID_WOLF && state == DAFFYELF_ST_WATCH)
                    return CTX_TALK;
                break;
        }
    }
    return 0;
}

/* 0x436733 - vtable +0x14: stops the calling sound. */
void DaffyElf::Reset()
{
    if (soundHandle) {
        StopSound(soundHandle);
        soundHandle = 0;
    }
}

/* 0x436770 - enter a state: stops the calling sound, then 1 opens the question box (ANSWERTEXTINDEX, ANSWERNUMBER
 * answers, the three choice handlers), 3 plays anim 8 and 4 anim 4, both looping. */
void DaffyElf::SetState(u8 newState)
{
    state = newState;
    if (soundHandle) {
        StopSound(soundHandle);
        soundHandle = 0;
    }
    switch (newState) {
        case DAFFYELF_ST_DIALOG:
            Dialog_Begin(&dialog, answerTextIndex, answerCount, g_daffyElfChoiceHandlers, this);
            break;
        case DAFFYELF_ST_WAIT_CINE_BOX:
            PlayAnim(ADELFE01_ANIM_STAND, 1, 0);
            break;
        case DAFFYELF_ST_ANSWER:
            PlayAnim(ADELFE01_ANIM_TALK1, 1, 0);
            break;
    }
}

/* 0x436892 - the class factory for CLASSID 55 "DaffyElf": new DaffyElf (the base vtables in turn, then DaffyElf's),
 * then ScnMobile_Init(record, 0) through vtable slot +0x20. */
ScnObject *DaffyElf_Create(void *record)
{
    DaffyElf *obj = new DaffyElf;
    obj = (DaffyElf *)obj->Init(record, 0); /* cast kept: Init returns the base class */
    return obj;
}
