/* T070 - original object DaffyLevel02.cpp (guessed name): .text 0x437b10-0x43a09a, .rdata 0x574d5c-0x574d80 (vtable),
 * .data 0x57a6d0-0x57a6e8 (the cinematic header's stride-table copy, then the answer handlers), .bss 0x6cc870-0x6cc874
 * (g_pDaffyLevel02).
 * PAL PC DaffyLevel02: a ScnMobile with an embedded head ScnBody. */
/* BYTES: dead-code, layout, slot-group, slot-name, temp. */
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
    void SetFacing(s16 value);      \
    void SetUpdateMode(s32 mode);   \
    void SetTint(u32 color, s16 amount, s32 on);


#define SDW_MEMBERS_Mat44 Mat44();
#define SDW_MEMBERS_DaffyLevel02         \
    char *AnswerText(u8 index)           \
    {                                    \
        return dialog.answerText[index]; \
    }
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#define SDW_INLINE_SCNOBJECT_FACING 1
#define SDW_INLINE_SCNOBJECT_SETFACING_S16 1
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_FACING
#undef SDW_INLINE_SCNOBJECT_SETFACING_S16
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32
#define SDW_INLINE_SCNOBJECT_SETTINT_U32_S16_S32 3
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETTINT_U32_S16_S32
#define SDW_INLINE_SCNBODY_GETANIMID 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_GETANIMID
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_CINE_ISACTIVE 1
#define SDW_INLINE_CINE_ISFINISHED 1
#include "../engine/cine_inlines.h"
#undef SDW_INLINE_CINE_ISACTIVE
#undef SDW_INLINE_CINE_ISFINISHED
#include "sdw_global_views.h"
typedef void (*MenuHandler)(u8, MenuPage *);
/* 0x57a6d0 - the cinematic header's static copy of the 9-byte opcode stride table (src/engine/cine1.cpp,
 * g_cineOpStride 0x5816fc). The original header defined it static, so every object including it carries its own
 * unreferenced copy at the head of its .data; defined here in its place. */
static u8 s_cineOpStride[9] = {0, 8, 8, 4, 2, 2, 4, 2, 2};
/* 0x6cc870 */
DaffyLevel02 *g_pDaffyLevel02;
void DaffyLevel02_DrawAnswer0(u8 msg, MenuPage *self);
void DaffyLevel02_DrawAnswer1(u8 msg, MenuPage *self);
void DaffyLevel02_DrawAnswer2(u8 msg, MenuPage *self);
/* 0x57a6dc */
MenuHandler g_daffy02AnswerHandlers[3] = {DaffyLevel02_DrawAnswer0, DaffyLevel02_DrawAnswer1, DaffyLevel02_DrawAnswer2};
extern Wolf *g_pWolf;
#include "../engine/cine.h"
#include "../engine/draw2d.h"
#include "../engine/input.h"
#include "../engine/interface.h"
#include "../engine/maths.h"
#include "../engine/text.h"
#include "../engine/prompt.h"
#include "../engine/id_list.h"
#include "../engine/scn_tools.h"
#include "camera.h"

extern TextScroll g_textScroll;

#define g_padCurButtons (g_pad.cur.buttons)

#define g_padPrevButtons (g_pad.prev.buttons)

extern s32 g_dtMs;
extern u32 g_samZoneColors[];
extern u32 *g_screenLayerBase;
extern "C" s16 g_sinTable4096[5122];
#include "../sdk/crt.h"
extern "C" s16 Math_RadiansToAngle4096(float);
u8 Dialogue_Say(const char *, s32, ScnObject *, u32);
void Dialog_Begin(DialogBox *, u32, s8, const MenuHandler *, ScnObject *);
void Ui_DrawTextBox(TextBox *, u16);
void Scenaric_Stub_515eef();
s32 Scenaric_FindByClass(u16, ScnObject **, s32);
/* BYTES(temp, inferred): the offset and the result go through the read struct because the original's expansion keeps both in stack slots */
#define SDW_INLINE_FREE_PROPU32_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_PROPU32_VOID_U32
#define SDW_INLINE_FREE_SCREENWIDTHU16 1
#include "../engine/screen_inlines.h"
#undef SDW_INLINE_FREE_SCREENWIDTHU16
inline void PlayCine(u32 id, u32 flags, Box *box, Box *sheep, void *text)
{
    g_cinePlayer.Start(id, flags, box, sheep, text, 0);
}
#define LIP_SYNC()                                                \
    if (g_pSoundSystem && g_pSoundSystem->GetVoiceAmplitude(1)) { \
        if (GetAnimId() != AROBIN01_ANIM_TALK1)                   \
            PlayAnim(AROBIN01_ANIM_TALK1, 1, 0);                  \
    } else if (GetAnimId() != AROBIN01_ANIM_STAND1A)              \
        PlayAnim(AROBIN01_ANIM_STAND1A, 1, 1);                    \
    Scenaric_Stub_515eef()
#define ABS_VALUE(a) ((a) >= 0 ? (a) : -(a))
#define SDW_INLINE_FREE_INBOX_BOX_VEC3S 1
#define SDW_INLINE_FREE_INBOXXZ_BOX_VEC3S 1
#include "../engine/coll_box_inlines.h"
#undef SDW_INLINE_FREE_INBOX_BOX_VEC3S
#undef SDW_INLINE_FREE_INBOXXZ_BOX_VEC3S

void DaffyLevel02_DrawAnswer0(u8 msg, MenuPage *self)
{
    switch (msg) {
        case MENU_MSG_DRAW:
            Text_PrintfStyled(TEXTALIGN_CENTER, self->selected, g_pDaffyLevel02->AnswerText(0));
    }
}
void DaffyLevel02_DrawAnswer1(u8 msg, MenuPage *self)
{
    switch (msg) {
        case MENU_MSG_DRAW:
            Text_PrintfStyled(TEXTALIGN_CENTER, self->selected, g_pDaffyLevel02->AnswerText(1));
    }
}
void DaffyLevel02_DrawAnswer2(u8 msg, MenuPage *self)
{
    switch (msg) {
        case MENU_MSG_DRAW:
            Text_PrintfStyled(TEXTALIGN_CENTER, self->selected, g_pDaffyLevel02->AnswerText(2));
    }
}

/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets */
void DaffyLevel02::PostLoadInit()
{
    struct Work {
        void *properties;
        ScnObject *rocks;
    } w;
    w.properties = record;
    cine[0] = PropU32(w.properties, 4);
    cine[1] = PropU32(w.properties, 8);
    textBurnt = (u8)PropU32(w.properties, 36);
    activationBox = Scn_GetPropBox(w.properties, 0);
    goalBox = Scn_GetPropBox(w.properties, 28);
    startBox = Scn_GetPropBox(w.properties, 32);
    cineBox = Scn_GetPropBox(w.properties, 12);
    cineSheepBox = Scn_GetPropBox(w.properties, 20);
    cineFlags = PropU32(w.properties, 16);
    cineTextBase = PropU32(w.properties, 24);
    curText = Text_GetClassString((u8)cineTextBase);
    yawToWolf = 0;
    relAngleToWolf = 0;
    headYaw = 0;
    headSweepPhase = 0;
    headSweepSpeed = 1;
    headMaxAngle = 70;
    talkPhase = DAFFY2_PH_CINE;
    InitHeadRecord(headRecord);
    head.Init(headRecord, 0);
    alertIconColor = 1;
    textBoxRect[0] = 50;
    textBoxRect[1] = 50;
    textBoxRect[2] = 412;
    textBoxRect[3] = 140;
    introDone = 0;
    gameArmed = 0;
    gameRunning = 0;
    PlayAnim(AROBIN01_ANIM_STAND1A, 1, 1);
    SetState(DAFFY2_STT_WAIT_CINEBOX);
    spawnRot = rot;
    cineIndex = 0;
    detectResult = DAFFY2_DET_NONE;
    startBoxTimerMs = 0;
    startBoxTimeoutMs = 7000;
    challengeWon = 0;
    startBoxNagGiven = 0;
    burnt = 0;
    g_pDaffyLevel02 = this;
    isLevel2Variant = 1;
    if (Scenaric_FindByClass(CLASSID_HIDDENROCKS, &w.rocks, 1)) {
        isLevel2Variant = 0;
        answerVoices[0] = VOICE_CIN_LVL_0308;
        answerVoices[1] = VOICE_CIN_LVL_0309;
        answerVoices[2] = VOICE_CIN_LVL_0310;
    } else {
        answerVoices[0] = VOICE_CIN_LVL_0212;
        answerVoices[1] = VOICE_CIN_LVL_0210;
        answerVoices[2] = VOICE_CIN_LVL_0211B;
        isLevel2Variant = 1;
    }
    SetUpdateMode(SCN_UPD_CINE);
}

void DaffyLevel02::Update()
{
    TextBox box;
    u8 dialogResult;
    switch (state) {
        case DAFFY2_STT_BURNT_LINE:
            if (!Dialogue_Say(Text_GetClassString(textBurnt), VOICE_CIN_LVL_0105, this, 0)) {
                if (wolfFrozen)
                    wolfFrozen = !g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0);
                burnt = 0;
                SetState(savedState);
            }
            LIP_SYNC();
            break;
        case DAFFY2_STT_DEBOUNCE:
            if ((g_padCurButtons & ~g_inputMap[INPUT_SLOT_CROSS]) ||
                !(g_padPrevButtons & ~g_inputMap[INPUT_SLOT_CROSS]))
                state = DAFFY2_STT_TALK;
            break;
        case DAFFY2_STT_TALK:
            curText = Text_GetClassString(talkPhase + cineTextBase);
            if ((!g_cinePlayer.IsActive() || (g_cinePlayer.IsActive() && g_textScroll.flagBits.open)) && !gameRunning) {
                FaceWolf();
                LIP_SYNC();
            }
            switch (talkPhase) {
                case DAFFY2_PH_CINE:
                case DAFFY2_PH_CINE2:
                    if (!g_cinePlayer.IsFinished())
                        break;
                    if (cineIndex == 0) {
                        cineIndex++;
                        wolfFrozen = g_pWolf->HandleMessage(this, MSG_FREEZE, 0);
                        talkPhase = DAFFY2_PH_ASK;
                    } else {
                        talkPhase = DAFFY2_PH_REMINDER;
                        if (isLevel2Variant)
                            voiceId = VOICE_CIN_LVL_0208;
                        else
                            voiceId = VOICE_CIN_LVL_0311;
                        introDone = 1;
                        gameArmed = 1;
                        SetState(DAFFY2_STT_GAME);
                    }
                    break;
                case DAFFY2_PH_ASK:
                    curText = Text_GetClassString(talkPhase + cineTextBase);
                    box.text = curText;
                    /* cast kept: the four rect words are copied as one 8-byte block, as the original */
                    *(Vec4s *)box.rect = *(Vec4s *)textBoxRect;
                    box.bgColor = 0xf2f2f2;
                    Ui_DrawTextBox(&box, 0xffff);
                    if (!(g_padCurButtons & ~g_inputMap[INPUT_SLOT_CROSS]) &&
                        (g_padPrevButtons & ~g_inputMap[INPUT_SLOT_CROSS]) && box.fits) {
                        if (!box.confirmChoice) {
                            talkPhase = DAFFY2_PH_ASK;
                            SetState(DAFFY2_STT_GAME);
                            if (wolfFrozen)
                                wolfFrozen = !g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0);
                            break;
                        } else {
                            if (wolfFrozen)
                                wolfFrozen = !g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0);
                            if (!introDone) {
                                talkPhase = DAFFY2_PH_CINE2;
                                curText = Text_GetClassString(talkPhase + cineTextBase);
                                PlayCine(cine[cineIndex], cineFlags, cineBox, cineSheepBox, curText);
                                break;
                            } else {
                                talkPhase = DAFFY2_PH_REMINDER;
                                if (isLevel2Variant)
                                    voiceId = VOICE_CIN_LVL_0208;
                                else
                                    voiceId = VOICE_CIN_LVL_0311;
                                introDone = 1;
                                gameArmed = 1;
                                SetState(DAFFY2_STT_GAME);
                            }
                        }
                    }
                    break;
                case DAFFY2_PH_CAUGHT_HEARD:
                case DAFFY2_PH_CAUGHT_MOVED_SHEEP:
                case DAFFY2_PH_CAUGHT_SEEN_SHEEP:
                case DAFFY2_PH_CAUGHT:
                    FaceWolf();
                    if (!wolfFrozen || !Dialogue_Say(curText, voiceId, this, 1)) {
                        if (wolfFrozen)
                            wolfFrozen = !g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0);
                        SetState(DAFFY2_STT_GAME);
                        startBoxTimerMs = 0;
                        gameRunning = 0;
                        gameArmed = 1;
                        detectResult = DAFFY2_DET_NONE;
                        talkPhase = DAFFY2_PH_REMINDER;
                        if (isLevel2Variant)
                            voiceId = VOICE_CIN_LVL_0208;
                        else
                            voiceId = VOICE_CIN_LVL_0311;
                        yawToWolf = 0;
                        relAngleToWolf = 0;
                        headYaw = 0;
                        headSweepPhase = 0;
                    }
                    break;
                case DAFFY2_PH_GOAL:
                    SayGoalReached();
                    break;
                case DAFFY2_PH_REMINDER:
                    if (!Dialogue_Say(curText, voiceId, this, 1)) {
                        if (wolfFrozen)
                            wolfFrozen = !g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0);
                        SetState(DAFFY2_STT_GAME);
                    }
                    break;
                case DAFFY2_PH_START_NAG:
                    if (!Dialogue_Say(curText, voiceId, this, 1)) {
                        if (wolfFrozen)
                            wolfFrozen = !g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0);
                        SetState(DAFFY2_STT_GAME);
                    }
            }
            break;
        case DAFFY2_STT_WAIT_CINEBOX:
            if (!g_cinePlayer.IsActive() && IsWolfInBoxXZ(cineBox) &&
                !g_pWolf->HandleMessage(this, MSG_WOLF_IS_FLYING, 0)) {
                SetState(DAFFY2_STT_TALK);
                talkPhase = DAFFY2_PH_CINE;
                curText = Text_GetClassString(talkPhase + cineTextBase);
                PlayCine(cine[cineIndex], cineFlags, cineBox, cineSheepBox, curText);
            }
            FaceWolf();
            break;
        case DAFFY2_STT_GAME:
            if (gameRunning) {
                if (!IsWolfInBoxXZ(startBox)) {
                    detectResult = DetectWolf();
                    startBoxTimerMs = 0;
                }
            } else
                FaceWolf();
            if (IsWolfInBoxXZ(startBox) && gameArmed) {
                gameRunning = 1;
                gameArmed = 0;
                rot = spawnRot;
                startBoxTimerMs = 0;
                break;
            }
            if (IsWolfInBoxXZ(goalBox) && gameRunning) {
                gameRunning = 0;
                gameArmed = 0;
                talkPhase = DAFFY2_PH_GOAL;
                SetState(DAFFY2_STT_DEBOUNCE);
                wolfFrozen = g_pWolf->HandleMessage(this, MSG_FREEZE, 0);
                PlayAnim(AROBIN01_ANIM_OK1, 0, 0);
                if (isLevel2Variant)
                    voiceId = VOICE_CIN_LVL_0207;
                else
                    voiceId = VOICE_CIN_LVL_0307;
                break;
            }
            if (!IsWolfInBoxXZ(activationBox) && gameRunning) {
                gameRunning = 0;
                gameArmed = 0;
                yawToWolf = 0;
                relAngleToWolf = 0;
                headYaw = 0;
                headSweepPhase = 0;
                talkPhase = DAFFY2_PH_ASK;
                break;
            }
            if (gameRunning) {
                if (IsWolfInBoxXZ(startBox)) {
                    if (!startBoxNagGiven && (u32)startBoxTimerMs > (u32)startBoxTimeoutMs) {
                        wolfFrozen = g_pWolf->HandleMessage(this, MSG_FREEZE, 0);
                        talkPhase = DAFFY2_PH_START_NAG;
                        SetState(DAFFY2_STT_TALK);
                        startBoxTimerMs = 0;
                        voiceId = VOICE_CIN_LVL_0209;
                        startBoxNagGiven = 1;
                    }
                } else {
                    startBoxNagGiven = 0;
                    startBoxTimerMs = 0;
                }
                switch (detectResult) {
                    case DAFFY2_DET_HEARD:
                        talkPhase = DAFFY2_PH_CAUGHT_HEARD;
                        SetState(DAFFY2_STT_DEBOUNCE);
                        wolfFrozen = g_pWolf->HandleMessage(this, MSG_FREEZE, 0);
                        if (isLevel2Variant)
                            voiceId = VOICE_CIN_LVL_0203;
                        else
                            voiceId = VOICE_CIN_LVL_0303;
                        break;
                    case DAFFY2_DET_MOVED:
                        SetState(DAFFY2_STT_DEBOUNCE);
                        if (g_pWolf->HandleMessage(this, MSG_WOLF_IS_IN_BUSH, 0)) {
                            talkPhase = DAFFY2_PH_CAUGHT_MOVED_SHEEP;
                            startBoxTimeoutMs = 3000;
                            if (isLevel2Variant)
                                voiceId = VOICE_CIN_LVL_0204;
                            else
                                voiceId = VOICE_CIN_LVL_0304;
                        } else {
                            talkPhase = DAFFY2_PH_CAUGHT;
                            startBoxTimeoutMs = 7000;
                            if (isLevel2Variant)
                                voiceId = VOICE_CIN_LVL_0206;
                            else
                                voiceId = VOICE_CIN_LVL_0304;
                        }
                        wolfFrozen = g_pWolf->HandleMessage(this, MSG_FREEZE, 0);
                        break;
                    case DAFFY2_DET_SEEN:
                        if (g_pWolf->HandleMessage(this, MSG_WOLF_IS_IN_BUSH, 0)) {
                            talkPhase = DAFFY2_PH_CAUGHT_SEEN_SHEEP;
                            startBoxTimeoutMs = 3000;
                            if (isLevel2Variant)
                                voiceId = VOICE_CIN_LVL_0205;
                            else
                                voiceId = VOICE_CIN_LVL_0304;
                        } else {
                            talkPhase = DAFFY2_PH_CAUGHT;
                            startBoxTimeoutMs = 7000;
                            if (isLevel2Variant)
                                voiceId = VOICE_CIN_LVL_0206;
                            else
                                voiceId = VOICE_CIN_LVL_0304;
                        }
                        SetState(DAFFY2_STT_DEBOUNCE);
                        wolfFrozen = g_pWolf->HandleMessage(this, MSG_FREEZE, 0);
                }
                startBoxTimerMs += g_dtMs;
                break;
            }
            break;
        case DAFFY2_STT_QUESTION:
            dialogResult = Dialog_Update(&dialog);
            if (dialogResult == DIALOG_PICKED)
                SetState(DAFFY2_STT_ANSWER);
            else if (dialogResult == DIALOG_CLOSED)
                SetState(DAFFY2_STT_GAME);
            break;
        case DAFFY2_STT_ANSWER:
            if (talkPhase > DAFFY2_PH_CINE2 && !gameRunning) {
                LIP_SYNC();
            }
            if (!Dialog_UpdateAnswer(&dialog, answerVoices))
                SetState(DAFFY2_STT_GAME);
    }
    if (!gameRunning)
        AdvanceAnim();
}

/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets */
void DaffyLevel02::SayGoalReached()
{
    struct Work {
        s32 dz, dx;
    } w;
    w.dx = g_pWolf->pos.x - pos.x;
    w.dz = g_pWolf->pos.z - pos.z;
    SetFacing((Math_RadiansToAngle4096((float)atan2(w.dx, w.dz)) + 0x800) & 0xfff);
    if (!Dialogue_Say(curText, voiceId, this, 1)) {
        rot = spawnRot;
        if (wolfFrozen)
            wolfFrozen = !g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0);
        SetState(DAFFY2_STT_GAME);
        talkPhase = DAFFY2_PH_ASK;
        challengeWon = 1;
        startBoxTimerMs = 0;
    }
}

/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets; unused0, unused1 fill gaps */
u8 DaffyLevel02::DetectWolf()
{
    struct Work {
        Vec3s wolf;
        u16 unused0;
        Vec3s self;
        u16 unused1;
    } w;
    if (g_pWolf->HandleMessage(this, MSG_WOLF_IS_DEAD, 0))
        return DAFFY2_DET_NONE;
    if (g_pWolf->HandleMessage(this, MSG_WOLF_IS_NOISY, 0))
        return DAFFY2_DET_HEARD;
    if (InBox(activationBox, &g_pWolf->pos)) {
        w.self = pos;
        w.wolf = g_pWolf->pos;
        yawToWolf =
            (Math_RadiansToAngle4096((float)atan2((double)w.wolf.x - w.self.x, (double)w.wolf.z - w.self.z)) + 0x800) &
            0xfff;
        UpdateHeadSweep();
        DrawAlertIcon();
        relAngleToWolf = (yawToWolf - (Facing() + headYaw)) & 0xfff;
        if (ABS_VALUE(relAngleToWolf) < 56) {
            if (isLevel2Variant) {
                if (g_pWolf->HandleMessage(this, MSG_WOLF_IS_VISIBLE, 0))
                    return DAFFY2_DET_SEEN;
                if (g_pWolf->HandleMessage(this, MSG_QUERY_MOVED, 0))
                    return DAFFY2_DET_MOVED;
            } else {
                if (g_pWolf->HandleMessage(this, MSG_QUERY_MOVED, 0))
                    return DAFFY2_DET_MOVED;
                if (g_pWolf->HandleMessage(this, MSG_WOLF_IS_VISIBLE, 0))
                    return DAFFY2_DET_SEEN;
            }
        }
    }
    return DAFFY2_DET_NONE;
}

s32 DaffyLevel02::IsWolfInBoxXZ(Box *box)
{
    if (!box)
        return 0;
    if (InBoxXZ(box, &g_pWolf->pos))
        return 1;
    return 0;
}

s32 DaffyLevel02::HandleMessage(ScnObject *sender, u32 msg, void *arg)
{
    if (!g_cinePlayer.IsActive())
        switch (msg) {
            case MSG_FREEZE:
                wolfFrozen = 0;
                return 1;
            case MSG_KILL:
                if (!arg) {
                    burnt = 1;
                    SetTint(0, 4096, 1);
                    head.SetTint(0, 4096, 1);
                }
                return 1;
            case MSG_USE:
                wolfFrozen = g_pWolf->HandleMessage(this, MSG_FREEZE, 0);
                if (burnt) {
                    savedState = state;
                    SetState(DAFFY2_STT_BURNT_LINE);
                } else if (state == DAFFY2_STT_AWAIT_TALK) {
                    talkPhase = DAFFY2_PH_ASK;
                    SetState(DAFFY2_STT_DEBOUNCE);
                } else if (challengeWon)
                    SetState(DAFFY2_STT_QUESTION);
                else
                    SetState(DAFFY2_STT_DEBOUNCE);
                break;
            case MSG_QUERY_ACTION:
                if (!sender->GetClassId() &&
                    ((state == DAFFY2_STT_GAME && !gameRunning) || state == DAFFY2_STT_AWAIT_TALK))
                    return CTX_TALK;
                break;
        }
    return 0;
}

void DaffyLevel02::Reset()
{
    SetTint(0, 0, 0);
    head.SetTint(0, 0, 0);
    burnt = 0;
    wolfFrozen = 0;
    if (introDone) {
        if (gameRunning) {
            gameRunning = 0;
            gameArmed = 1;
        }
        SetState(DAFFY2_STT_GAME);
    } else {
        gameRunning = 0;
        gameArmed = 0;
        if (cineIndex == 1)
            SetState(DAFFY2_STT_AWAIT_TALK);
        else
            SetState(DAFFY2_STT_WAIT_CINEBOX);
    }
    detectResult = DAFFY2_DET_NONE;
    rot = spawnRot;
}

/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets */
void DaffyLevel02::InitHeadRecord(u16 *rec)
{
    struct Work {
        u32 *ids;
        u16 unused, count;
        void *model;
    } w;
    rec[2] = 0;
    rec[3] = 0;
    rec[4] = 0;
    rec[8] = 0;
    rec[7] = 0;
    rec[6] = 0;
    rec[5] = 0;
    w.ids = Scn_FindIdList(WAR_IDO_ATEROB01, &w.count);
    w.model = (void *)*w.ids; /* cast kept: an id list holds resource pointers as u32 words */
    rec[0] = Dav_FindResourceIndex(w.model);
    rec[1] = 0xffff;
}

/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
/* BYTES(dead-code): flags19 is set and never read, as in the original */
/* BYTES(slot-name): equal buckets and reverse declarations keep the scalar slots; the camera stays contiguous */
void DaffyLevel02::DrawAlertIcon()
{
    /* Equal hash buckets and reverse declarations preserve the scalar slots;
     * the camera stays contiguous, including its real Mat44 constructors. */
    s16 top15, bottom8;
    s32 flags19;
    s16 right14, left0;
    struct HudWork {
        Camera camera;
        s32 viewFlags;
        s16 screenX, screenY;
    } w;
    w.viewFlags = 0x30;
    flags19 = 0x30;
    w.camera.rot.x = w.camera.rot.z = w.camera.rot.y = 0;
    w.camera.dist = 300;
    Vec3s_OffsetAlongAngles(&w.camera.pos, &w.camera.rot, w.camera.dist, g_pZeroVec3s);
    Camera_BuildViewMatrix(&w.camera, g_pZeroVec3s);
    head.SetFacing((-relAngleToWolf) & 0xfff);
    w.screenX = ScreenWidthU16() - (ScreenWidthU16() >> 3);
    w.screenY = 55;
    head.RenderEx(&w.camera, headBufToggle ? headRenderBufA : headRenderBufB, 100, 0x180, &w.screenX);
    headBufToggle = !headBufToggle;
    left0 = w.screenX - 35;
    bottom8 = w.screenY;
    right14 = w.screenX + 35;
    top15 = w.screenY - 40;
    g_animSpriteCrayon1.Draw(g_screenLayerBase + 9, left0, bottom8, right14, top15, g_samZoneColors[alertIconColor], 0,
                             0);
}

void DaffyLevel02::UpdateHeadSweep()
{
    Vec3s angle;
    headSweepPhase = (headSweepPhase + headSweepSpeed * (g_dtMs >> 1)) & 0xfff;
    headYaw = g_sinTable4096[headSweepPhase] * headMaxAngle / 360;
    angle.x = 0;
    angle.y = headYaw;
    angle.z = 0;
    SetJointOverride(4, &angle, 0, 0);
}

void DaffyLevel02::SetState(u8 next)
{
    state = next;
    switch (next) {
        case DAFFY2_STT_DEBOUNCE:
            PlayAnim(AROBIN01_ANIM_STAND1A, 1, 1);
            break;
        case DAFFY2_STT_TALK:
            PlayAnim(AROBIN01_ANIM_STAND1A, 1, 0);
            break;
        case DAFFY2_STT_UNUSED:
            PlayAnim(AROBIN01_ANIM_STAND1, 1, 1);
            break;
        case DAFFY2_STT_GAME:
            PlayAnim(AROBIN01_ANIM_STAND1A, 1, 1);
            break;
        case DAFFY2_STT_QUESTION:
            PlayAnim(AROBIN01_ANIM_STAND1A, 1, 0);
            Dialog_Begin(&dialog, 10, 3, g_daffy02AnswerHandlers, this);
    }
}

void DaffyLevel02::FaceWolf()
{
    SetFacing(HeadingTo(&g_pWolf->pos));
}
ScnObject *DaffyLevel02_Create(void *record)
{
    ScnBody *object = new DaffyLevel02;
    object = object->Init(record, 0);
    return object;
}
