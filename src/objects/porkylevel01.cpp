/*
 * T085 - original object PorkyLevel01.cpp (guessed name): .text 0x459a90-0x45ac1d, .rdata 0x574f60-0x574f84 (vtable),
 * .data 0x57a83c-0x57a860 (the cinematic header's stride-table copy, the answer voices, the choice handlers),
 * .bss 0x6cccec-0x6cccf0 (g_pPorkyLevel01). PostLoadInit comes with its ScreenWidthU16 / ScreenHeightU16 inlines.
 *
 * PorkyLevel01 (class 30, vtable 0x574f60, sizeof 0x204) - Porky, the farmer of Level 1 (disc Lvl-01) who teaches the
 * lettuce: the level's lettuces (class 3 Salad) are taken out of the world at load; when Ralph walks into ACTIVATIONBOX
 * with a sheep nearby Porky plays CINWITHSHEEP and the lettuces reappear one by one during it (all of them when it
 * ends); without a sheep he plays CINWITHOUTSHEEP once. Talking to him opens a three-choice question box (ASKTEXT,
 * CHOICESTEXTS) whose answer he then says; after an explosion (MSG_KILL, argument 0) he is black and says the burnt line
 * once. SheepD3D.exe 0x459a90-0x45ac1c: the three choice handlers, ShowSalad, HideSalads, Update, HandleMessage,
 * SetState, Reset, PostLoadInit, the class factory and FaceWolf (which follows the factory; its only caller is Update).
 * The function before 0x459a90 (0x4597f5, a cdecl path follower called from 0x4528f2 and 0x456128) ends the previous
 * file: int3 padding separates them.
 * The PorkyLevel01 fields, MenuBox.pages and the MenuPage flag bits are in data/structs.
 *
 * States (+0x98): 0 waiting for Ralph in ACTIVATIONBOX, 1 lesson done, 2 with-sheep cinematic, 3 without-sheep
 * cinematic, 4 saying a line, 5 disabled (ACTIVATIONBOX or SHEEPBOX is not a single box), 6 question box open.
 * Devices that only pin the original code generation: the inline helpers (their names are not recovered; each is here
 * because its expansion gives the original's stack temporaries) and the names of the locals in PostLoadInit and Update
 * (their stack slots follow from their names, tools/vc6_locals.py). PostLoadInit's `i` holds a property id before it
 * is the loop counter: the original uses one slot for both (0x45a6b9, 0x45a896).
 */
/* BYTES: inline, layout, slot-name. */
/* BYTES(layout): the Cine.h header static: every object including the cinematic header carries this copy in its .data, referenced or not */

#define SDW_MEMBERS_ScnObject            \
    static void *operator new(u32 size); \
    void SetFacing(s16 f);               \
    void SetUpdateMode(s32 mode);        \
    void SetTint(u32 color, s16 amount, s32 on);


#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *inst, Animator *animator, u16 id, u32 opts); /* 0x550196 */

/* The instance flag word (ScnObject +4) set / cleared through a pointer: the pointer is a stack temp and the constant
 * mask is loaded into a register first; the clear complements it there and narrows it to 16 bits (0x45a331-0x45a36f). */
#define SDW_INLINE_FREE_INSTFLAGS_SET_U16_U16 1
#include "instance_inlines.h"
#undef SDW_INLINE_FREE_INSTFLAGS_SET_U16_U16

#define SDW_INLINE_FREE_INSTFLAGS_CLEAR_U16_U16 1
#include "instance_inlines.h"
#undef SDW_INLINE_FREE_INSTFLAGS_CLEAR_U16_U16

#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_RECORD 1
#define SDW_INLINE_SCNOBJECT_SETFACING_S16 1
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_RECORD
#undef SDW_INLINE_SCNOBJECT_SETFACING_S16
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32
#define SDW_INLINE_SCNOBJECT_SETTINT_U32_S16_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETTINT_U32_S16_S32
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_CINE_ISACTIVE 1
#define SDW_INLINE_CINE_ISWOLFREADY 1
#include "../engine/cine_inlines.h"
#undef SDW_INLINE_CINE_ISACTIVE
#undef SDW_INLINE_CINE_ISWOLFREADY

typedef void (*MenuHandler)(u8 msg, MenuPage *self);

s32 Scenaric_FindByClass(u16 classId, ScnObject **out, s32 max); /* 0x5145c5 */
#include "../engine/id_list.h"
#include "../engine/scn_tools.h"
#include "../engine/text.h"
#include "../engine/input.h"
#include "../engine/cine.h"
#include "../engine/game_state.h"
u16 Text_CountWrappedLines(const char *s);                                               /* 0x53353f */
u16 Str_Length(const char *s);                                                           /* 0x5614fb */
void Menu_BuildList(MenuPage *pages, Menu *menu, s8 count, const MenuHandler *handlers); /* 0x539199 */
void Dialogue_SetBoxActive(s32 active);                                                  /* 0x539479 */
void Dialogue_Reset();                                                                   /* 0x539507 */
u8 Dialogue_Say(const char *text, s32 voiceId, ScnObject *speaker, u32 arg);             /* 0x539893 */
void Ui_DrawMenuBox(MenuBox *box);                                                       /* 0x53d385 */

extern Wolf *g_pWolf;          /* 0x6cf310 */
extern s32 g_dtMs;             /* 0x71b2e8  g_dt * 1000 >> 12 */
extern u32 *g_screenLayerBase; /* 0x585044 */

/* 0x57a83c - the cinematic header's static copy of the 9-byte opcode stride table (src/engine/cine.cpp,
 * g_cineOpStride 0x5816fc). The original header defined it static, so every object including it carries its own
 * unreferenced copy at the head of its .data; defined here in its place. */
static u8 s_cineOpStride[9] = {0, 8, 8, 4, 2, 2, 4, 2, 2};

/* .bss: it has no bytes to find it by, so its address comes from the symbol tables. */
PorkyLevel01 *g_pPorkyLevel01; /* 0x6cccec  set in state 6, read by the choice handlers */

/* A designer property of the WAR record: the dword at record + 0x14 + offset (PorkyLevel01Props). Inlined; the offset
 * is a stack temp, which it is with a u32 parameter (observed, as in src/objects/daffyelf.cpp). */
#define SDW_INLINE_FREE_SCN_GETPROPS32_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_SCN_GETPROPS32_VOID_U32

/* Whether p lies inside box in x and z (the vertical is not tested): both parameters are stack temps (0x459dd4). */
#define SDW_INLINE_FREE_BOX_CONTAINSPOINTXZ_BOX_VEC3S 1
#include "../engine/coll_box_inlines.h"
#undef SDW_INLINE_FREE_BOX_CONTAINSPOINTXZ_BOX_VEC3S

/* Cine_Start with no dialogue block: the arguments that are not constants go through stack temps (0x459ee1). */
/* BYTES(inline): source-only inline: its non-constant arguments go through stack temps (0x459ee1) */
#define SDW_INLINE_FREE_CINE_PLAY_U32_U32_BOX_BOX_VOID 1
#include "../engine/cine_inlines.h"
#undef SDW_INLINE_FREE_CINE_PLAY_U32_U32_BOX_BOX_VOID

/* 0x459a90 - MenuHandler of the question box's first choice: on msg 0 (draw) prints it, blinking while selected. */
void PorkyLevel01_DrawChoice0(u8 msg, MenuPage *self)
{
    switch (msg) {
        case MENU_MSG_DRAW:
            Text_PrintfStyled(TEXTALIGN_CENTER, self->selected, g_pPorkyLevel01->choiceTexts[0]);
    }
}

/* 0x459acd - the same for the second choice. */
void PorkyLevel01_DrawChoice1(u8 msg, MenuPage *self)
{
    switch (msg) {
        case MENU_MSG_DRAW:
            Text_PrintfStyled(TEXTALIGN_CENTER, self->selected, g_pPorkyLevel01->choiceTexts[1]);
    }
}

/* 0x459b0a - the same for the third choice. */
void PorkyLevel01_DrawChoice2(u8 msg, MenuPage *self)
{
    switch (msg) {
        case MENU_MSG_DRAW:
            Text_PrintfStyled(TEXTALIGN_CENTER, self->selected, g_pPorkyLevel01->choiceTexts[2]);
    }
}

/* 0x57a848, 0x57a854 - the voice of each answer, and the question box's choice handlers (Menu_BuildList). */
u32 g_porkyLevel01AnswerVoices[3] = {0x3b, 0x3c, 0x3d};
MenuHandler g_porkyLevel01ChoiceHandlers[3] = {PorkyLevel01_DrawChoice0, PorkyLevel01_DrawChoice1,
                                               PorkyLevel01_DrawChoice2};

/* 0x459b47 - puts lettuce `index` back into the world where it was taken out, idle (anim 0), and forgets it. */
void PorkyLevel01::ShowSalad(s8 index)
{
    if (salads[index]) {
        salads[index]->AddToWorld(&saladPos[index]);
        salads[index]->PlayAnim(ALAITU01_ANIM_APPEAR, 0, 0);
        salads[index] = 0;
    }
}

/* 0x459bf4 - finds the level's lettuces (at most 20), remembers where each is and takes it out of the world. */
void PorkyLevel01::HideSalads()
{
    u32 i;
    /* cast kept: the Salad pointers are filled through the finder's generic ScnObject ** array */
    saladCount = Scenaric_FindByClass(CLASSID_SALAD, (ScnObject **)salads, 20);
    for (i = 0; i < saladCount; i++) {
        saladPos[i] = salads[i]->pos;
        salads[i]->RemoveFromWorld();
    }
}

/* 0x459c7f - vtable +0x04: the lettuce show (after DISPLAYSALADMILLIS, one every 500 ms, counted only while the
 * cinematic has the Wolf ready), then the state machine, the animation, and the end of the talk guard. */
void PorkyLevel01::Update()
{
    u8 row;
    ScnObject *nearSheep;
    u32 i;
    if (saladShowActive) {
        if (saladDelayMs > 0) {
            if (g_cinePlayer.IsWolfReady())
                saladDelayMs -= g_dtMs;
        } else {
            saladShowIndex++;
            saladDelayMs = 500;
            if (saladShowIndex >= (s8)saladCount)
                saladShowActive = 0;
            else
                ShowSalad(saladShowIndex);
        }
    }
    switch (state) {
        case PORKY01_ST_TALK:
            if (!Dialogue_Say(curText, curVoice, this, 1)) {
                if (wolfFrozen)
                    g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0);
                wolfFrozen = 0;
                Dialogue_Reset();
                SetState(PORKY01_ST_IDLE, 0, 0);
            }
            break;
        case PORKY01_ST_IDLE:
            FaceWolf();
            if (Box_ContainsPointXZ(activationBox, &g_pWolf->pos)) {
                nearSheep = Scenaric_FindNearestOfClass(&g_pWolf->pos, CLASSID_SHEEP, -0x8000, 0x7fff, 200, 0, 0);
                if (nearSheep && !cinWithSheepPlayed) {
                    SetState(PORKY01_ST_CINE_SHEEP, 0, 0);
                    cinWithSheepPlayed = 1;
                    saladShowActive = 1;
                    saladShowIndex = -1;
                    sheep = nearSheep;
                    nearSheep->HandleMessage(this, MSG_CINE_PLACE, 0);
                    Cine_Play(cinWithSheep, CINE_CAM_CUT_IN | CINE_CAM_CUT_OUT | CINE_LETTERBOX, cinBox, sheepBox,
                              cinWithSheepText);
                } else if (!nearSheep && !cinWithoutSheepPlayed && !cinWithSheepPlayed) {
                    cinWithoutSheepPlayed = 1;
                    SetState(PORKY01_ST_CINEMATIC, 0, 0);
                    Cine_Play(cinWithoutSheep, CINE_CAM_CUT_IN | CINE_CAM_CUT_OUT | CINE_LETTERBOX, cinBox, sheepBox,
                              cinWithoutSheepText);
                }
            }
            break;
        case PORKY01_ST_CINE_SHEEP:
            if (!g_cinePlayer.IsActive()) {
                for (i = 0; i < saladCount; i++)
                    ShowSalad((s8)i);
                SetState(PORKY01_ST_SALADS_SHOWN, 0, 0);
            }
            break;
        case PORKY01_ST_CINEMATIC:
            if (!g_cinePlayer.IsActive())
                SetState(PORKY01_ST_IDLE, 0, 0);
            break;
        case PORKY01_ST_QUESTION:
            g_pPorkyLevel01 = this;
            Dialogue_SetBoxActive(1);
            Ui_DrawMenuBox(&questionBox);
            if (questionBox.fits) {
                if (!menuJustOpened && Pad_MenuPressed((u16)~PAD_CROSS) && g_letterboxState == LETTERBOX_OPEN) {
                    row = questionBox.cursorRow;
                    SetState(PORKY01_ST_TALK, Text_GetClassString(Scn_GetPropS32(Record(), 4) + row),
                             g_porkyLevel01AnswerVoices[row]);
                } else if (Pad_MenuPressed((u16)~PAD_TRIANGLE)) {
                    if (wolfFrozen)
                        g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0);
                    wolfFrozen = 0;
                    Dialogue_Reset();
                    SetState(PORKY01_ST_IDLE, 0, 0);
                }
            }
            break;
    }
    AdvanceAnim();
    menuJustOpened = 0;
}

/* 0x45a197 - vtable +0x10. 0xE: Ralph unfroze himself. 1 (talk, from Ralph outside cinematics): freeze Ralph, then
 * the burnt line once after an explosion, else the question box. 0x58: talk/idle animations 5/1 for the lip sync.
 * 2 (context query): CTX_TALK (8) to Ralph outside cinematics. 0 (MSG_KILL) argument 0: burnt black. 0x13 (cinematic
 * over): re-enter the state, and pass 0x13 on to the sheep of the cinematic. */
s32 PorkyLevel01::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    switch (msgId) {
        case MSG_FREEZE:
            wolfFrozen = 0;
            return 1;
        case MSG_USE:
            if (sender->GetClassId() == CLASSID_WOLF && !g_cinePlayer.IsActive() && !menuJustOpened) {
                if (!wolfFrozen)
                    g_pWolf->HandleMessage(this, MSG_FREEZE, 0);
                wolfFrozen = 1;
                menuJustOpened = 1;
                if (burnt && !burntLineSaid) {
                    burntLineSaid = 1;
                    SetState(PORKY01_ST_TALK, Text_GetClassString(9), 0x41);
                } else
                    SetState(PORKY01_ST_QUESTION, 0, 0);
            }
            break;
        case MSG_QUERY_TALK_ANIMS:
            return (APORKY01_ANIM_TALK1 << 16) | APORKY01_ANIM_STAND1;
        case MSG_QUERY_ACTION:
            if (sender->GetClassId() == CLASSID_WOLF && !g_cinePlayer.IsActive())
                return CTX_TALK;
            break;
        case MSG_KILL:
            switch ((u32)arg) { /* cast kept: this message's void * arg carries the kill kind */
                case KILL_GENERIC:
                    burnt = 1;
                    SetTint(0, 0x1000, 1);
            }
            return 1;
        case MSG_CINE_END:
            SetState(state, 0, 0);
            if (sheep)
                sheep->HandleMessage(this, MSG_CINE_END, 0);
            sheep = 0;
            break;
    }
    return 0;
}

/* 0x45a446 - enter a state: 4 (saying `text` with `voice`) talks with anim 5, any other stops the voice and idles with
 * anim 1. */
void PorkyLevel01::SetState(u8 newState, char *text, u32 voice)
{
    state = newState;
    switch (state) {
        case PORKY01_ST_TALK:
            PlayAnim(APORKY01_ANIM_TALK1, 1, 0);
            curText = text;
            curVoice = voice;
            break;
        default:
            Voice_StopStream(this);
            PlayAnim(APORKY01_ANIM_STAND1, 1, 0);
            break;
    }
}

/* 0x45a523 - vtable +0x14: clears the burnt tint (and nothing else: the state, the cinematics played and the lettuces
 * shown are kept). */
void PorkyLevel01::Reset()
{
    SetTint(0, 0, 0);
}

/* The 512x240 text screen's dimensions as inline values (the established HUD inline-dimension idiom, engine/hud.cpp):
 * with this spelling the box centring at the end of PostLoadInit compiles constant-first, as in the original. */
#define SDW_INLINE_FREE_SCREENWIDTHU16 1
#include "../engine/screen_inlines.h"
#undef SDW_INLINE_FREE_SCREENWIDTHU16
#define SDW_INLINE_FREE_SCREENHEIGHTU16 1
#include "../engine/screen_inlines.h"
#undef SDW_INLINE_FREE_SCREENHEIGHTU16

/* 0x45a58a - vtable +0x00: takes the lettuces out, reads the properties (ACTIVATIONBOX and SHEEPBOX must each be one
 * box, else Porky is disabled in state 5), builds the question box and sizes it to its longest line, centred in the
 * 512x240 text screen. */
/* BYTES(slot-name): i holds the property id and then the loop counter: the original uses one slot for both (0x45a6b9, 0x45a896) */
void PorkyLevel01::PostLoadInit()
{
    Box **ids;
    s8 j;
    s32 i;
    u16 nBoxes;
    u16 maxLen;
    void *rec;
    char *question;
    rec = record;
    HideSalads();
    SetState(PORKY01_ST_IDLE, 0, 0);
    SetUpdateMode(SCN_UPD_CINE);
    cinWithoutSheepPlayed = 0;
    cinWithSheepPlayed = 0;
    saladShowActive = 0;
    menuJustOpened = 0;
    curVoice = 0;
    /* cast kept (the three lists): an export id list, here of zone boxes */
    i = Scn_GetPropS32(rec, 0);
    ids = (Box **)Scn_FindIdList((u16)i, &nBoxes);
    if (nBoxes == 1)
        activationBox = *ids;
    else
        SetState(PORKY01_ST_CINE_NO_SHEEP, 0, 0);
    i = Scn_GetPropS32(rec, 0x28);
    /* cast kept: an export id list holds its records' pointers as u32 words */
    ids = (Box **)Scn_FindIdList((u16)i, &nBoxes);
    if (nBoxes == 1)
        sheepBox = *ids;
    else
        SetState(PORKY01_ST_CINE_NO_SHEEP, 0, 0);
    i = Scn_GetPropS32(rec, 0x10);
    /* cast kept: an export id list holds its records' pointers as u32 words */
    ids = (Box **)Scn_FindIdList((u16)i, &nBoxes);
    if (nBoxes == 1)
        cinBox = *ids;
    else
        cinBox = 0;
    cinWithSheep = (u16)Scn_GetPropS32(rec, 0x1c);
    cinWithoutSheep = (u16)Scn_GetPropS32(rec, 0x14);
    cinWithoutSheepText = Text_GetClassString((u8)Scn_GetPropS32(rec, 0x18));
    cinWithSheepText = Text_GetClassString((u8)Scn_GetPropS32(rec, 0x20));
    withoutSheepText2 = Text_GetClassString((u8)Scn_GetPropS32(rec, 0x2c));
    withSheepText2 = Text_GetClassString((u8)Scn_GetPropS32(rec, 0x30));
    question = Text_GetClassString((u8)Scn_GetPropS32(rec, 8));
    for (i = 0; i < 3; i++)
        choiceTexts[i] = Text_GetClassString(Scn_GetPropS32(rec, 0xc) + i);
    saladDelayMs = Scn_GetPropS32(rec, 0x24);
    sheep = 0;
    Menu_BuildList(choicePages, &choiceMenu, 3, g_porkyLevel01ChoiceHandlers);
    questionBox.text = question;
    Text_SetFont(FONT_GAME);
    maxLen = Str_Length(questionBox.text);
    for (j = 0; j < 3; j++)
        maxLen = maxLen < Str_Length(choiceTexts[j]) ? Str_Length(choiceTexts[j]) : maxLen;
    questionBox.rect[2] = (maxLen + 2) * g_pCurFont->glyphWidth > 0x200 ? 0x200 : (maxLen + 2) * g_pCurFont->glyphWidth;
    questionBox.rect[0] = 0;
    questionBox.rect[1] = 0;
    questionBox.rect[3] = 0;
    Text_SetWindowRect(g_screenLayerBase + 6, questionBox.rect, 0);
    questionBox.rect[3] =
        (s16)(Text_CountWrappedLines(questionBox.text) + choiceMenu.count + 2) * g_pCurFont->lineHeight > 0xf0
            ? 0xf0
            : (s16)(Text_CountWrappedLines(questionBox.text) + choiceMenu.count + 2) * g_pCurFont->lineHeight;
    questionBox.rect[0] = (ScreenWidthU16() - questionBox.rect[2]) / 2;
    questionBox.rect[1] = (ScreenHeightU16() - questionBox.rect[3]) / 2;
    questionBox.bgColor = 0;
    questionBox.pages = choicePages;
    questionBox.list = &choiceMenu;
    curText = 0;
    wolfFrozen = 0;
}

/* 0x45ab80 - the class factory for CLASSID 30 "PorkyLevel01": new PorkyLevel01 (the base vtables in turn, then
 * PorkyLevel01's), then ScnMobile_Init(record, 0) through vtable slot +0x20. */
ScnObject *PorkyLevel01_Create(void *record)
{
    PorkyLevel01 *obj = new PorkyLevel01;
    obj = (PorkyLevel01 *)obj->Init(record, 0); /* cast kept: Init returns the base class */
    return obj;
}

/* 0x45abf0 - turn to face Ralph (state 0). */
void PorkyLevel01::FaceWolf()
{
    SetFacing(HeadingTo(&g_pWolf->pos));
}
