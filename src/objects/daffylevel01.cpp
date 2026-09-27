/* T069 - original object DaffyLevel01.cpp (guessed name).
 * .text 0x436910-0x437b0b, .rdata 0x574d38-0x574d5c (vtable), .data 0x57a6a4-0x57a6d0, .bss 0x6cc86c-0x6cc870.
 * PAL PC DaffyLevel01, runtime VAs 0x436910..0x437b0a.
 * .data: the cinematic header's stride-table copy, the voice-property offsets, the choice handlers; .bss: g_pDaffyLevel01. */
/* BYTES: layout, slot-group, temp. */
/* BYTES(layout): the Cine.h header static: every object including the cinematic header carries this copy in its .data, referenced or not */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *, Animator *, u16, u32);
#define SDW_INLINE_FREE_INSTFLAGSSET_U16_U16 1
#include "instance_inlines.h"
#undef SDW_INLINE_FREE_INSTFLAGSSET_U16_U16
#define SDW_INLINE_FREE_INSTFLAGSCLEAR_U16_U16 1
#include "instance_inlines.h"
#undef SDW_INLINE_FREE_INSTFLAGSCLEAR_U16_U16

#define SDW_MEMBERS_ScnObject       \
    static void *operator new(u32); \
    void SetFacing(s16 value);


#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_SETFACING_S16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_SETFACING_S16
#define SDW_INLINE_SCNOBJECT_ENABLEBOXCOLLIDE_S32 1
#define SDW_INLINE_SCNOBJECT_ENABLETINT_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_ENABLEBOXCOLLIDE_S32
#undef SDW_INLINE_SCNOBJECT_ENABLETINT_S32
#define SDW_INLINE_SCNBODY_GETANIMID 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_GETANIMID
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_SHADOW_REPROJECT 1
#include "../engine/shadow_inlines.h"
#undef SDW_INLINE_SHADOW_REPROJECT
#define SDW_INLINE_CINE_ISACTIVE 1
#include "../engine/cine_inlines.h"
#undef SDW_INLINE_CINE_ISACTIVE
typedef void (*MenuHandler)(u8, MenuPage *);
/* 0x57a6a4 - the cinematic header's static copy of the 9-byte opcode stride table (src/engine/cine.cpp,
 * g_cineOpStride 0x5816fc). The original header defined it static, so every object including it carries its own
 * unreferenced copy at the head of its .data; defined here in its place. */
static u8 s_cineOpStride[9] = {0, 8, 8, 4, 2, 2, 4, 2, 2};
/* 0x6cc86c */
DaffyLevel01 *g_pDaffyLevel01;
/* 0x57a6b0 - property-record offsets of the five answer voices and the burnt voice */
u16 g_daffy01VoicePropOffsets[6] = {28, 32, 36, 40, 44, 48};
void DaffyLevel01_DrawChoice0(u8 msg, MenuPage *self);
void DaffyLevel01_DrawChoice1(u8 msg, MenuPage *self);
void DaffyLevel01_DrawChoice2(u8 msg, MenuPage *self);
void DaffyLevel01_DrawChoice3(u8 msg, MenuPage *self);
void DaffyLevel01_DrawChoice4(u8 msg, MenuPage *self);
/* 0x57a6bc */
MenuHandler g_daffy01ChoiceHandlers[5] = {DaffyLevel01_DrawChoice0, DaffyLevel01_DrawChoice1, DaffyLevel01_DrawChoice2,
                                          DaffyLevel01_DrawChoice3, DaffyLevel01_DrawChoice4};
#include "../engine/draw2d.h"
#include "../engine/cine.h"
#include "../app/app_main.h"
#include "../engine/text.h"
#include "../engine/game_state.h"
#include "../engine/input.h"
#include "../engine/scn_tools.h"
#include "camera.h"
extern Wolf *g_pWolf;
extern u32 *g_screenLayerBase;
u16 Text_CountWrappedLines(const char *), Str_Length(const char *);
void Menu_BuildList(MenuPage *, Menu *, s8, const MenuHandler *);
void Dialogue_SetBoxActive(s32), Dialogue_Reset();
u8 Dialogue_Say(const char *, s32, ScnObject *, u32);
void Ui_DrawMenuBox(MenuBox *);
void Scenaric_Stub_515eef();
s32 Scenaric_FindByClass(u16, ScnObject **, s32);
s32 Rand_Bounded(s32);
void Camera_StartScripted(ScnObject *, Camera *, u16, u16, u16, Vec3s *, u16, u32, s32);
/* BYTES(temp, inferred): the offset and the result go through the read struct because the original's expansion keeps both in stack slots */
#define SDW_INLINE_FREE_PROPU32_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_PROPU32_VOID_U32
#define SDW_INLINE_FREE_INDEXEDPROP_VOID_U16 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_INDEXEDPROP_VOID_U16
inline void StartCamera(ScnObject *owner, u16 pitch, u16 yaw, u16 roll, Vec3s *position, u16 focal)
{
    Camera_StartScripted(owner, &g_camera, pitch, yaw, roll, position, focal, CAMSCR_BLEND_IN | CAMSCR_BLEND_OUT, 4096);
}
/* The virtual screen size (512 x 240) as inline values, the idiom the HUD code uses too. */
#define SDW_INLINE_FREE_SCREENWIDTHU16 1
#include "../engine/screen_inlines.h"
#undef SDW_INLINE_FREE_SCREENWIDTHU16
#define SDW_INLINE_FREE_SCREENHEIGHTU16 1
#include "../engine/screen_inlines.h"
#undef SDW_INLINE_FREE_SCREENHEIGHTU16

void DaffyLevel01_DrawChoice0(u8 msg, MenuPage *self)
{
    switch (msg) {
        case MENU_MSG_DRAW:
            Text_PrintfStyled(TEXTALIGN_CENTER, self->selected, g_pDaffyLevel01->choiceTexts[0]);
    }
}
void DaffyLevel01_DrawChoice1(u8 msg, MenuPage *self)
{
    switch (msg) {
        case MENU_MSG_DRAW:
            Text_PrintfStyled(TEXTALIGN_CENTER, self->selected, g_pDaffyLevel01->choiceTexts[1]);
    }
}
void DaffyLevel01_DrawChoice2(u8 msg, MenuPage *self)
{
    switch (msg) {
        case MENU_MSG_DRAW:
            Text_PrintfStyled(TEXTALIGN_CENTER, self->selected, g_pDaffyLevel01->choiceTexts[2]);
    }
}
void DaffyLevel01_DrawChoice3(u8 msg, MenuPage *self)
{
    switch (msg) {
        case MENU_MSG_DRAW:
            Text_PrintfStyled(TEXTALIGN_CENTER, self->selected, g_pDaffyLevel01->choiceTexts[3]);
    }
}
void DaffyLevel01_DrawChoice4(u8 msg, MenuPage *self)
{
    switch (msg) {
        case MENU_MSG_DRAW:
            Text_PrintfStyled(TEXTALIGN_CENTER, self->selected, g_pDaffyLevel01->choiceTexts[4]);
    }
}

/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets */
void DaffyLevel01::UpdateLipSync()
{
    struct Work {
        u32 anims;
        u16 unused, id;
    } w;
    w.anims = HandleMessage(this, MSG_QUERY_TALK_ANIMS, 0);
    w.id = anim.animId;
    if (g_pSoundSystem && g_pSoundSystem->GetVoiceAmplitude(1))
        w.id = w.anims >> 16;
    else
        w.id = w.anims & 0xffff;
    Scenaric_Stub_515eef();
    if (w.id != GetAnimId())
        PlayAnim(w.id, 1, 1);
}

void DaffyLevel01::Update()
{
    u8 choice;
    if (flags.variant13)
        shadow.Reproject();
    switch (state) {
        case DAFFY1_STT_IDLE:
            if (flags.variant13) {
                if (AnimFlags(ANIM_F_FINISHED)) {
                    if ((u32)Vec3s_ManhattanDistXZ(&g_pWolf->pos, &pos) > 600)
                        choice = (u8)Rand_Bounded(2);
                    else
                        choice = 0;
                    if (choice)
                        PlayAnim(AROBIN01_ANIM_APPEAR1, 1, 0);
                    else
                        PlayAnim(AROBIN01_ANIM_JUMP2, 1, 0);
                }
            } else
                SetFacing(HeadingTo(&g_pWolf->pos));
            break;
        case DAFFY1_STT_SAY_ANSWER:
            UpdateLipSync();
            if (!Dialogue_Say(answerTexts[answerIdx], answerVoices[answerIdx], this, answerIdx != 5)) {
                if (flags.frozen && g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0))
                    flags.frozen = 0;
                if (!flags.frozen) {
                    Dialogue_Reset();
                    SetState(DAFFY1_STT_IDLE);
                }
            }
            break;
        case DAFFY1_STT_ASK:
            Dialogue_SetBoxActive(1);
            Ui_DrawMenuBox(&questionBox);
            if (questionBox.fits) {
                if (!menuJustOpened && Pad_MenuPressed((u16)~PAD_CROSS) && g_letterboxState == LETTERBOX_OPEN) {
                    answerIdx = questionBox.cursorRow;
                    SetState(DAFFY1_STT_SAY_ANSWER);
                } else if (Pad_MenuPressed((u16)~PAD_TRIANGLE)) {
                    if (flags.frozen && g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0))
                        flags.frozen = 0;
                    Dialogue_Reset();
                    SetState(DAFFY1_STT_IDLE);
                }
            }
            break;
    }
    menuJustOpened = 0;
    AdvanceAnim();
}

s32 DaffyLevel01::HandleMessage(ScnObject *sender, u32 message, void *arg)
{
    switch (message) {
        case MSG_FREEZE:
            flags.frozen = 0;
            return 1;
        case MSG_USE:
            if (!sender->GetClassId() && !g_cinePlayer.IsActive() && !state) {
                if (!flags.frozen)
                    g_pWolf->HandleMessage(this, MSG_FREEZE, 0);
                flags.frozen = 1;
                if (textCamera)
                    StartCamera(this, textCamera->rot[0], textCamera->rot[1], textCamera->rot[2], &textCamera->eye,
                                textCamera->focal);
                menuJustOpened = 1;
                if (burnt && !burntLineSaid) {
                    answerIdx = 5;
                    burntLineSaid = 1;
                    SetState(DAFFY1_STT_SAY_ANSWER);
                } else
                    SetState(DAFFY1_STT_ASK);
            }
            break;
        case MSG_QUERY_ACTION:
            if (!sender->GetClassId() && !g_cinePlayer.IsActive() && !state)
                return CTX_TALK;
            break;
        case MSG_CINE_END:
            SetState(DAFFY1_STT_IDLE);
            break;
        case MSG_QUERY_TALK_ANIMS:
            if (flags.train)
                return (AROBIN01_ANIM_EYES1 << 16) | AROBIN01_ANIM_APPEAR1;
            if (flags.variant13)
                return (AROBIN01_ANIM_HAND1 << 16) | AROBIN01_ANIM_JUMP2;
            return (AROBIN01_ANIM_TALK1 << 16) | AROBIN01_ANIM_STAND1A;
        case MSG_KILL:
            switch ((s32)arg) { /* cast kept: the message arg is a void *; MSG_KILL passes the kill type in it */
                case KILL_GENERIC:
                    burnt = 1;
                    tintColor = 0;
                    tintAmount = 4096;
                    EnableTint(1);
            }
            return 1;
    }
    return 0;
}

void DaffyLevel01::SetState(u8 next)
{
    state = next;
    switch (next) {
        case DAFFY1_STT_IDLE:
            if (textCamera)
                Camera_ReleaseScripted(this);
            if (flags.train)
                PlayAnim(AROBIN01_ANIM_APPEAR1, 1, 1);
            else if (flags.variant13)
                PlayAnim(AROBIN01_ANIM_JUMP2, 1, 1);
            else
                PlayAnim(AROBIN01_ANIM_STAND1, 1, 1);
            break;
        case DAFFY1_STT_SAY_ANSWER:
            if (flags.variant13)
                SetFacing(HeadingTo(&g_pWolf->pos));
            if (flags.train)
                PlayAnim(AROBIN01_ANIM_EYES1, 1, 1);
            else if (flags.variant13)
                PlayAnim(AROBIN01_ANIM_HAND1, 1, 1);
            else
                PlayAnim(AROBIN01_ANIM_STAND1A, 1, 1);
            break;
        case DAFFY1_STT_ASK:
            if (flags.variant13)
                SetFacing(HeadingTo(&g_pWolf->pos));
            if (flags.train)
                PlayAnim(AROBIN01_ANIM_EYES1, 1, 1);
            else if (flags.variant13)
                PlayAnim(AROBIN01_ANIM_HIDE1, 1, 1);
            else
                PlayAnim(AROBIN01_ANIM_STAND1A, 1, 1);
    }
}

void DaffyLevel01::Reset()
{
    tintColor = 0;
    tintAmount = 0;
    EnableTint(0);
    burnt = 0;
    burntLineSaid = 0;
}

/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets; unused0 fill gaps */
void DaffyLevel01::PostLoadInit()
{
    struct Work {
        u8 unused0[3];
        s8 index;
        void *properties;
        char *question;
        u16 unused1, maxLength;
        ScnObject *train;
    } w;
    w.properties = record;
    burnt = 0;
    answerIdx = 0;
    nbChoices = (s8)PropU32(w.properties, 12);
    for (w.index = 0; w.index < nbChoices; w.index++) {
        answerTexts[w.index] = Text_GetClassString(PropU32(w.properties, 0) + w.index);
        choiceTexts[w.index] = Text_GetClassString(PropU32(w.properties, 4) + w.index);
        answerVoices[w.index] = IndexedProp(w.properties, g_daffy01VoicePropOffsets[w.index]);
    }
    burntText = Text_GetClassString((u8)PropU32(w.properties, 20));
    burntVoice = IndexedProp(w.properties, g_daffy01VoicePropOffsets[5]);
    w.question = Text_GetClassString((u8)PropU32(w.properties, 16));
    textCamera = Scn_GetPropCamera(w.properties, 24);
    burntLineSaid = 0;
    g_pDaffyLevel01 = this;
    menuJustOpened = 0;
    flags.frozen = 0;
    Menu_BuildList(choicePages, &choiceMenu, nbChoices, g_daffy01ChoiceHandlers);
    questionBox.text = w.question;
    Text_SetFont(FONT_GAME);
    w.maxLength = Str_Length(questionBox.text);
    for (w.index = 0; w.index < nbChoices; w.index++)
        w.maxLength = w.maxLength < Str_Length(choiceTexts[w.index]) ? Str_Length(choiceTexts[w.index]) : w.maxLength;
    questionBox.rect[2] =
        (w.maxLength + 2) * g_pCurFont->glyphWidth > 512 ? 512 : (w.maxLength + 2) * g_pCurFont->glyphWidth;
    questionBox.rect[0] = 0;
    questionBox.rect[1] = 0;
    questionBox.rect[3] = 0;
    Text_SetWindowRect(g_screenLayerBase + 6, questionBox.rect, 0);
    questionBox.rect[3] =
        (s16)(Text_CountWrappedLines(questionBox.text) + choiceMenu.count + 2) * g_pCurFont->lineHeight > 240
            ? 240
            : (s16)(Text_CountWrappedLines(questionBox.text) + choiceMenu.count + 2) * g_pCurFont->lineHeight;
    questionBox.rect[0] = (ScreenWidthU16() - questionBox.rect[2]) / 2;
    questionBox.rect[1] = (ScreenHeightU16() - questionBox.rect[3]) / 2;
    questionBox.bgColor = 0;
    questionBox.pages = choicePages;
    questionBox.list = &choiceMenu;
    flags.train = 0;
    if (Scenaric_FindByClass(CLASSID_TRAIN, &w.train, 1))
        flags.train = 1;
    flags.variant13 = 0;
    level = (u8)PropU32(w.properties, 8);
    if (level == SCENE_LVL_13) {
        EnableBoxCollide(1);
        flags.variant13 = 1;
        flags.train = 0;
    }
    SetState(DAFFY1_STT_IDLE);
}

ScnObject *DaffyLevel01_Create(void *record)
{
    ScnBody *object = new DaffyLevel01;
    object = object->Init(record, 0);
    return object;
}
