/*
 * T206 - original object SceneWheel.cpp or Scene_Wheel.cpp (guessed name), one translation unit.
 *   .text  0x4e9480-0x4e9e6d (Scene_Wheel_AngleToIndex .. Scene_Wheel_Create)
 *   .rdata 0x576a7c-0x576ab4 (five float COMDATs first emitted here - 4.0, 4.712389, 1000.0, 5.497787, 1.5707964 -
 *          then ??_7Scene_Wheel)
 *   .data  0x57b71c-0x57b758 (the four language-name literals, g_sceneWheelLanguageNames, g_sceneWheelLanguageIds)
 *   .bss   0x6cfa50-0x6cfa5c (g_sceneWheelAngle, then 8 unreferenced bytes)
 * g_sceneWheelLanguageIds is not const (the exe has it in .data) and the .bss is defined in place.
 */
/* BYTES: cast, layout. */
/* BYTES(layout): placeholder: 8 unreferenced bytes, '= 0' so they follow g_sceneWheelAngle */
/* BYTES(layout): not const: the original has it in .data */
/* PAL PC Scene_Wheel. */

#define SDW_MEMBERS_ScnObject            \
    static void *operator new(u32 size); \
    void SetUpdateMode(u8 mode);
#define SDW_MEMBERS_ScnBody                                                         \
    AnimJointPose *GetJointPose(s32 index)                                          \
    {                                                                               \
        /* cast kept: the pose buffer is untyped; it holds AnimJointPose records */ \
        return ((AnimJointPose *)anim.bufC) + index;                                \
    }

#define SDW_MEMBERS_Progress   \
    void SetLanguage(u8 value) \
    {                          \
        language = value;      \
    }
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *inst, Animator *animator, u16 id, u32 opts);
#include "sdw_classes.h"
#include "../sdk/crt.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32

extern Wolf *g_pWolf;
#include "../engine/progress.h"
#include "../engine/scenaric.h"
#include "../engine/screen.h"
#include "../engine/input.h"
#include "../engine/fade.h"
#include "../engine/text.h"
extern s32 g_dtRawMs;
extern s32 g_dtMs;
extern u32 g_gameFlags;
/* .bss 0x6cfa50-0x6cfa5c */
float g_sceneWheelAngle; /* 0x6cfa50  the wheel's angle, kept across the level's reloads */
/* 0x6cfa54-0x6cfa5c: 8 bytes nothing in the exe refers to. The map gives them to this object (they could also be
   SignPost's, or any object's from Seaweed to SheepCostume); what they held is unknown, so they are opaque here.
   Explicitly initialised so VC6 puts them after g_sceneWheelAngle (definition order), not by name hash. */
u8 g_sceneWheelUnreferenced[8] = {0};

/* 0x4e9a28 calls 0x535dde with six stack arguments. */

/* Original data at VA0x57b744 and VA0x57b754. Byte escapes retain the
 * original single-byte Portuguese/Spanish characters and Italian case. */
const char *g_sceneWheelLanguageNames[] = {"english",
                                           "Portugu\xEA"
                                           "s",
                                           "espa\xF1"
                                           "ol",
                                           "italIAno"};
/* non-const: the exe has it in .data, after the names (0x57b754) */
u8 g_sceneWheelLanguageIds[] = {GAME_LANG_ENGLISH, GAME_LANG_BRAZILIAN, GAME_LANG_SPANISH, GAME_LANG_ITALIAN};

#define SDW_INLINE_FREE_GAME_CLEARFLAGS_U32 1
#include "../engine/game_state_inlines.h"
#undef SDW_INLINE_FREE_GAME_CLEARFLAGS_U32
#define SDW_INLINE_FREE_SCREENHEIGHTS16 1
#include "../engine/screen_inlines.h"
#undef SDW_INLINE_FREE_SCREENHEIGHTS16
/* 0x4e9a1e reads g_screen+0x84 (VA0x6d7064), the generated scratch60
 * allocation that contains this screen's 24 four-byte layer handles.
 * The u16 argument reproduces the original LEA and register allocation;
 * the helper does not establish an original source name. */
/* BYTES(cast): the u16 argument reproduces the original lea and register allocation */
inline u32 *SceneWheel_GetLayer(u16 index)
{
    /* cast kept: scratch60 is raw scratch that holds this screen's layer handles */
    return (u32 *)g_screen.scratch60 + index;
}

#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8

/* 0x4e9480. The divisor is the original decimal constant, whose double
 * representation is 0x401921fb54442d11. The caller clamps a result of four. */
s8 Scene_Wheel_AngleToIndex(float angle)
{
    double normalized = (float)fmod(angle, 6.28318530717958);
    if (!_isnan(normalized))
        return (s8)((float)normalized * 4.0f / 6.2831855f + 0.5f);
    return 0;
}

/* 0x4e94e0 */
void Scene_Wheel::PostLoadInit()
{
    Reset();
}

/* 0x4e94f6 */
void Scene_Wheel::Update()
{
    s8 selected;
    AnimJointPose *pose = 0;
    idleMs += (s16)g_dtRawMs;
    Game_ClearFlags(GF_TRANSITION_IDLE);
    if (state == SCENEWHEEL_ST_SELECT || state == SCENEWHEEL_ST_CONFIRM || state == SCENEWHEEL_ST_UNUSED_2) {
        pose = GetJointPose(1);
        if (pose && pose - 1)
            pose->rot[0] = g_sceneWheelAngle;
    }
    switch (state) {
        case SCENEWHEEL_ST_START:
            if (g_pWolf && !wolfFrozen)
                wolfFrozen = g_pWolf->HandleMessage(this, MSG_FREEZE, 0);
            state = SCENEWHEEL_ST_PUSH;
            PlayAnim(ALANGUE1_ANIM_PUSH, 0, 1);
            break;
        case SCENEWHEEL_ST_PUSH:
            if (AnimFlags(ANIM_F_FINISHED))
                state = SCENEWHEEL_ST_SETUP;
            break;
        case SCENEWHEEL_ST_SETUP:
            idleMs = 0;
            snapping = 0;
            state = SCENEWHEEL_ST_SELECT;
            targetAngle = index * 1.5707964f;
            pose = GetJointPose(1);
            if (pose && pose - 1)
                pose->rot[0] = targetAngle;
            break;
        case SCENEWHEEL_ST_SELECT:
            selected = index;
            pose = GetJointPose(1);
            if (Pad_MenuRepeat((u16)~PAD_RIGHT)) {
                index++;
                if (index >= 4)
                    index = 0;
                snapping = 1;
                targetAngle += 1.5707964f;
                Scenaric_SendToClass(CLASSID_DAFFYWHEEL, MSG_DAFFYWHEEL_TURN, 0);
                idleMs = 0;
            }
            if (Pad_MenuRepeat((u16)~PAD_LEFT)) {
                index--;
                if (index < 0)
                    index = 3;
                snapping = 1;
                targetAngle -= 1.5707964f;
                Scenaric_SendToClass(CLASSID_DAFFYWHEEL, MSG_DAFFYWHEEL_TURN, 0);
                idleMs = 0;
            }
            if (pose && pose - 1) {
                if (pose->rot[0] > 6.2831855f && targetAngle > 6.2831855f) {
                    pose->rot[0] = (double)(float)fmod(pose->rot[0], 6.28318530717958);
                    targetAngle = index * 1.5707964f;
                }
                if (pose->rot[0] < 0.0f && targetAngle < 0.0f) {
                    pose->rot[0] += 6.2831855f;
                    targetAngle = index * 1.5707964f;
                }
                angVel = (targetAngle - pose->rot[0]) * 2.0f;
                if ((angVel >= 0.0f ? (float)(double)angVel : -angVel) > 5.497786998748779f)
                    angVel = (angVel >= 0.0f ? 1 : -1) * 5.497786998748779f;
                if (snapping) {
                    if ((angVel > 0.0f && pose->rot[0] >= targetAngle) ||
                        (angVel < 0.0f && pose->rot[0] <= targetAngle)) {
                        snapping = 0;
                        angVel = 0;
                        pose->rot[0] = index * 1.5707964f;
                    }
                    pose->rot[0] = ((float)g_dtMs * angVel) / 1000.0f + pose->rot[0];
                }
                selected = Scene_Wheel_AngleToIndex(pose->rot[0]);
            }
            if (selected >= 4 || selected < 0)
                selected = 0;
            if (Pad_MenuPressed((u16)~PAD_CROSS)) {
                state = SCENEWHEEL_ST_CONFIRM;
                /* cast kept: arg carries a number */
                Scenaric_SendToClass(CLASSID_DAFFYWHEEL, MSG_DAFFYWHEEL_SELECT,
                                     (void *)(u32)g_sceneWheelLanguageIds[selected]);
                g_pProgress->SetLanguage(g_sceneWheelLanguageIds[selected]);
            }
            Text_SetFont(FONT_GAME);
            Text_SetWindow(SceneWheel_GetLayer(6), 64, ScreenHeightS16() - 40, 256, 24, 1);
            Text_PrintfStyled(TEXTALIGN_CENTER, 1, g_sceneWheelLanguageNames[selected]);
            Hud_EndBox_stub();
            break;
        case SCENEWHEEL_ST_CONFIRM:
            SetUpdateMode(SCN_UPD_NEVER);
            if (g_pWolf && wolfFrozen) {
                g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0);
                wolfFrozen = 0;
            }
            if (exitLevelOnConfirm) {
                g_levelExitFlags |= LEVEL_EXIT_DEMO;
                Fade_StartLevelExit(0x1000);
            } else
                Scenaric_SendToClass(CLASSID_MCARDMANAGER, MSG_USE, 0);
            state = SCENEWHEEL_ST_UNUSED_2;
            break;
    }
    if (state == SCENEWHEEL_ST_SELECT || state == SCENEWHEEL_ST_CONFIRM || state == SCENEWHEEL_ST_UNUSED_2) {
        pose = GetJointPose(1);
        if (pose && pose - 1) {
            g_sceneWheelAngle = pose->rot[0];
            pose->rot[0] += 4.712389f;
            AdvanceAnim();
        }
    } else
        AdvanceAnim();
}

/* 0x4e9c08 */
s32 Scene_Wheel::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    if (msgId == MSG_SCENEWHEEL_SAVE_DONE && sender->GetClassId() == CLASSID_MCARDMANAGER) {
        SetUpdateMode(SCN_UPD_NORMAL);
        state = SCENEWHEEL_ST_SETUP;
        return 1;
    }
    return 0;
}

/* 0x4e9cfe */
void Scene_Wheel::Reset()
{
    flags &= ~SCN_OF_HIDDEN;
    SetUpdateMode(SCN_UPD_NORMAL);
    index = 0;
    angVel = 0;
    snapping = 0;
    wolfFrozen = 0;
    unk6a = 0;
    idleMs = 0;
    state = SCENEWHEEL_ST_START;
}

/* 0x4e9e09 */
ScnObject *Scene_Wheel_Create(void *record)
{
    ScnBody *obj = new Scene_Wheel;
    obj = obj->Init(record, 0);
    return obj;
}
