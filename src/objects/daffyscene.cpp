/* T073 - original object DaffyScene.cpp (guessed name): .text 0x43bf20-0x43d5b2, .rdata 0x574dc8-0x574dec (vtable),
 * .data 0x57a718-0x57a724 (the cinematic header's stride-table copy; 0x57a724-0x57a728 is linker padding before the next
 * object's 8-aligned .data). */
/* BYTES: layout, slot-group. */
/* BYTES(layout): the Cine.h header static: every object including the cinematic header carries this copy in its .data, referenced or not */
/* PAL PC DaffyScene. */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *, Animator *, u16, u32);

#define SDW_MEMBERS_ScnObject static void *operator new(u32);


#define SDW_MEMBERS_Progress \
    s8 LevelA()              \
    {                        \
        return levelIndexA;  \
    }                        \
    s8 LevelB()              \
    {                        \
        return levelIndexB;  \
    }                        \
    u8 ModeBits()            \
    {                        \
        return modeBits;     \
    }
#define SDW_MEMBERS_DaffyScene                                                              \
    u32 Prop(u32);                                                                          \
    void StartCamera(u16, u16, Vec3s *, u16, u32);                                          \
    s32 IsTalking()                                                                         \
    {                                                                                       \
        return GetAnimId() == ADAPRE01_ANIM_TALK1 || GetAnimId() == ADAPRE01_ANIM_TALK1A || \
               GetAnimId() == ADAPRE01_ANIM_TALK2;                                          \
    }
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_RECORD 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_RECORD
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#define SDW_INLINE_SCNBODY_GETANIMID 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_GETANIMID
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_CINE_ISACTIVE 1
#include "../engine/cine_inlines.h"
#undef SDW_INLINE_CINE_ISACTIVE
/* 0x57a718 - the cinematic header's static copy of the 9-byte opcode stride table (src/engine/cine.cpp,
 * g_cineOpStride 0x5816fc). The original header defined it static, so every object including it carries its own
 * unreferenced copy at the head of its .data; defined here in its place. */
static u8 s_cineOpStride[9] = {0, 8, 8, 4, 2, 2, 4, 2, 2};
#include "../engine/progress.h"
#include "../app/app_main.h"
#include "../engine/cine.h"
#include "camera.h"
#include "../engine/draw2d.h"
#include "../engine/scn_tools.h"
extern Wolf *g_pWolf;
s32 Scenaric_FindByClass(u16, ScnObject **, s32), Rand_Bounded(s32);
void Camera_StartScripted(ScnObject *, Camera *, u16, u16, u16, Vec3s *, u16, u32, s32);
void Scenaric_Stub_515eef(), Dialogue_Reset();
u8 Dialogue_Say(const char *, s32, ScnObject *, u32);
inline s32 IsPlayable(s8 level)
{
    return level >= SCENE_LVL_00 && level < SCENE_LEVEL_COUNT;
}
inline s32 ScriptedCamera()
{
    return g_camMode == CAM_SCRIPT_BLEND_IN || g_camMode == CAM_SCRIPT_TO_SCRIPT || g_camMode == CAM_SCRIPTED ||
           g_camMode == CAM_SCRIPT_RETURN;
}
/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets */
inline u32 DaffyScene::Prop(u32 offset)
{
    struct Work {
        u32 result, offset;
    } w;
    w.offset = offset;
    /* cast kept: designer properties are 4-byte slots at byte offsets of the raw WAR record */
    w.result = *(u32 *)((u8 *)properties + w.offset + 0x14);
    return w.result;
}
inline void DaffyScene::StartCamera(u16 pitch, u16 yaw, Vec3s *point, u16 focal, u32 mode)
{
    Camera_StartScripted(this, &g_camera, pitch, yaw, 0, point, focal, mode, 4096);
}

void DaffyScene::PostLoadInit()
{
    u8 i;
    properties = Record();
    crowds[2] = 0;
    crowds[1] = 0;
    crowds[0] = 0;
    crowdCount = (u8)Scenaric_FindByClass(CLASSID_CROWD, crowds, 3);
    for (i = 0; i < crowdCount; i++)
        crowds[i]->HandleMessage(this, MSG_CROWD_CALM, 0);
    if ((g_pProgress->LevelB() != g_pProgress->LevelA() && IsPlayable(g_pProgress->LevelB())) ||
        !IsPlayable(g_pProgress->LevelB()))
        SetState(DAFFYSCENE_ST_IDLE);
    else {
        SetState(DAFFYSCENE_ST_WAIT_CINE);
        if (g_pProgress->LevelB() == SCENE_LVL_00)
            nextState = DAFFYSCENE_ST_TALK_INTRO;
        else
            nextState = DAFFYSCENE_ST_SPEECH;
    }
    speechCameras[0] = Scn_GetPropCamera(properties, 12);
    speechCameras[1] = Scn_GetPropCamera(properties, 16);
    speechCameras[2] = Scn_GetPropCamera(properties, 20);
    speechCameras[3] = Scn_GetPropCamera(properties, 24);
    speechCameras[4] = Scn_GetPropCamera(properties, 28);
    cameraCount = 0;
    for (i = 0; i < 5; i++)
        if (speechCameras[i])
            cameraCount++;
    chatterCount = 0;
}
void DaffyScene::Reset() {}
void DaffyScene::Update()
{
    switch (state) {
        case DAFFYSCENE_ST_IDLE:
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(DAFFYSCENE_ST_IDLE);
            break;
        case DAFFYSCENE_ST_WAIT_CINE:
            if (!g_cinePlayer.IsActive() && !ScriptedCamera())
                SetState(nextState);
            break;
        case DAFFYSCENE_ST_TALK_INTRO:
        case DAFFYSCENE_ST_TALK_PROP_54:
        case DAFFYSCENE_ST_TALK_PROP_5C:
        case DAFFYSCENE_ST_TALK_PROP_40:
        case DAFFYSCENE_ST_SPEECH:
        case DAFFYSCENE_ST_CHATTER:
            if (g_pSoundSystem && g_pSoundSystem->GetVoiceAmplitude(1)) {
                if (!IsTalking())
                    StartRandomTalkAnim(0);
            } else if (GetAnimId() != ADAPRE01_ANIM_STAND)
                PlayAnim(ADAPRE01_ANIM_STAND, 1, 0);
            Scenaric_Stub_515eef();
            if (!text)
                SetState(nextState);
            else if (!Dialogue_Say(text, voice, this, 1)) {
                Dialogue_Reset();
                SetState(nextState);
            }
    }
    AdvanceAnim();
}
void DaffyScene::SetState(u8 next)
{
    u8 i;
    state = next;
    switch (state) {
        case DAFFYSCENE_ST_IDLE:
            Camera_ReleaseScripted(this);
            for (i = 0; i < crowdCount; i++)
                crowds[i]->HandleMessage(this, MSG_CROWD_CALM, 0);
            if (wolfFrozen)
                wolfFrozen = !g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0);
            switch (Rand_Bounded(8)) {
                case 0:
                    PlayAnim(ADAPRE01_ANIM_STAND, 1, 1);
                    break;
                case 1:
                    PlayAnim(ADAPRE01_ANIM_STAND2, 1, 1);
                    break;
                case 2:
                    PlayAnim(ADAPRE01_ANIM_DANSE1, 1, 1);
                    break;
                case 3:
                    PlayAnim(ADAPRE01_ANIM_DANSE2, 1, 1);
                    break;
                case 4:
                    PlayAnim(ADAPRE01_ANIM_IMPRESS, 1, 1);
                    break;
                case 5:
                    PlayAnim(ADAPRE01_ANIM_JOKE, 1, 1);
                    break;
                case 6:
                    PlayAnim(ADAPRE01_ANIM_SEE, 1, 1);
                    break;
                case 7:
                    PlayAnim(ADAPRE01_ANIM_SHOW, 1, 1);
            }
            break;
        case DAFFYSCENE_ST_TALK_INTRO:
            if (!wolfFrozen)
                wolfFrozen = g_pWolf->HandleMessage(this, MSG_FREEZE, 0);
            activeCamera = Scn_GetPropCamera(properties, 0);
            StartCamera(activeCamera->rot[0], activeCamera->rot[1], &activeCamera->eye, activeCamera->focal,
                        CAMSCR_BLEND_IN);
            text = Text_GetClassString((u8)Prop(0x38));
            voice = VOICE_CIN02_SC_REL002;
            nextState = DAFFYSCENE_ST_TALK_PROP_54;
            break;
        case DAFFYSCENE_ST_TALK_PROP_54:
            activeCamera = Scn_GetPropCamera(properties, 8);
            StartCamera(activeCamera->rot[0], activeCamera->rot[1], &activeCamera->eye, activeCamera->focal,
                        CAMSCR_CHAIN);
            text = Text_GetClassString((u8)Prop(0x54));
            voice = VOICE_CIN02_SC_REL004;
            nextState = DAFFYSCENE_ST_TALK_PROP_5C;
            break;
        case DAFFYSCENE_ST_TALK_PROP_5C:
            activeCamera = Scn_GetPropCamera(properties, 0x20);
            StartCamera(activeCamera->rot[0], activeCamera->rot[1], &activeCamera->eye, activeCamera->focal,
                        CAMSCR_CHAIN);
            text = Text_GetClassString((u8)Prop(0x5c));
            voice = VOICE_CIN02_SC_REL005;
            nextState = DAFFYSCENE_ST_TALK_PROP_40;
            break;
        case DAFFYSCENE_ST_TALK_PROP_40:
            activeCamera = Scn_GetPropCamera(properties, 4);
            StartCamera(activeCamera->rot[0], activeCamera->rot[1], &activeCamera->eye, activeCamera->focal,
                        CAMSCR_CHAIN);
            text = Text_GetClassString((u8)Prop(0x40));
            voice = VOICE_CIN02_SC_REL006;
            nextState = DAFFYSCENE_ST_IDLE;
            break;
        case DAFFYSCENE_ST_SPEECH:
            talkTimer = 4000;
            if (!wolfFrozen)
                wolfFrozen = g_pWolf->HandleMessage(this, MSG_FREEZE, 0);
            if (cameraCount) {
                activeCamera = speechCameras[Rand_Bounded(cameraCount)];
                StartCamera(activeCamera->rot[0], activeCamera->rot[1], &activeCamera->eye, activeCamera->focal,
                            CAMSCR_BLEND_IN);
            }
            text = PickResultLine();
            for (i = 0; i < crowdCount; i++)
                crowds[i]->HandleMessage(this, MSG_CROWD_CHEER, 0);
            nextState = DAFFYSCENE_ST_IDLE;
            break;
        case DAFFYSCENE_ST_CHATTER:
            talkTimer = 4000;
            chatterCount++;
            if (!wolfFrozen)
                wolfFrozen = g_pWolf->HandleMessage(this, MSG_FREEZE, 0);
            if (cameraCount) {
                activeCamera = speechCameras[Rand_Bounded(cameraCount)];
                StartCamera(activeCamera->rot[0], activeCamera->rot[1], &activeCamera->eye, activeCamera->focal,
                            CAMSCR_BLEND_IN);
            }
            text = PickIdleChatterLine();
            for (i = 0; i < crowdCount; i++)
                crowds[i]->HandleMessage(this, MSG_CROWD_CALM, 0);
            nextState = DAFFYSCENE_ST_IDLE;
    }
}
s32 DaffyScene::HandleMessage(ScnObject *sender, u32 msg, void *arg)
{
    switch (msg) {
        case MSG_VOICE_STARTED:
            talkTimer = 500;
            break;
        case MSG_QUERY_ACTION:
            if (!sender->GetClassId() && !state)
                return CTX_TALK;
            break;
        case MSG_USE:
            if (!sender->GetClassId() && !state) {
                SetState(DAFFYSCENE_ST_CHATTER);
                return 1;
            }
    }
    return 0;
}
void DaffyScene::StartRandomTalkAnim(s32 blend)
{
    switch (Rand_Bounded(3)) {
        case 0:
            PlayAnim(ADAPRE01_ANIM_TALK1, 1, blend);
            break;
        case 1:
            PlayAnim(ADAPRE01_ANIM_TALK1A, 1, blend);
            break;
        case 2:
            PlayAnim(ADAPRE01_ANIM_TALK2, 1, blend);
    }
}
char *DaffyScene::PickResultLine()
{
    u16 choice;
    if (IsPlayable(g_pProgress->LevelA()) && !g_pProgress->IsTimeKeeperDone(g_pProgress->LevelA()) &&
        g_pProgress->LevelA() > SCENE_LVL_04 && g_pProgress->ModeBits() % 2) {
        voice = VOICE_RET_NIV_03GE;
        return Text_GetClassString((u8)Prop(0x48));
    }
    if (IsPlayable(g_pProgress->LevelA())) {
        choice = g_pProgress->ModeBits() % 4;
        switch (choice) {
            case 0:
                voice = VOICE_RET_NIV_00A;
                return Text_GetClassString((u8)Prop(0x60));
            case 1:
                voice = VOICE_RET_NIV_01;
                return Text_GetClassString((u8)Prop(0x64));
            case 2:
                voice = VOICE_RET_NIV_02GE;
                return Text_GetClassString((u8)Prop(0x68));
            case 3:
                voice = VOICE_RE_NIV_04GE;
                return Text_GetClassString((u8)Prop(0x6c));
        }
    }
    if (IsPlayable(g_pProgress->LevelB()) && g_pProgress->LevelB() == SCENE_LVL_01) {
        voice = VOICE_RET_LVL_00;
        return Text_GetClassString((u8)Prop(0x3c));
    }
    choice = g_pProgress->ModeBits() % 3;
    switch (choice) {
        case 0:
            voice = VOICE_CIN02_SC_REL002;
            return Text_GetClassString((u8)Prop(0x58));
        case 1:
            voice = VOICE_SC_01;
            return Text_GetClassString((u8)Prop(0x44));
        case 2:
            voice = VOICE_RE_NIV_05QU;
            return Text_GetClassString((u8)Prop(0x50));
    }
    return 0;
}
char *DaffyScene::PickIdleChatterLine()
{
    if (chatterCount % 5 == 0) {
        voice = VOICE_BE_02;
        return Text_GetClassString((u8)Prop(0x4c));
    }
    switch (Rand_Bounded(3)) {
        case 0:
            voice = VOICE_SC_01;
            return Text_GetClassString((u8)Prop(0x58));
        case 1:
            voice = VOICE_RE_NIV_05QU;
            return Text_GetClassString((u8)Prop(0x50));
        case 2:
            voice = VOICE_SC_03;
            return Text_GetClassString((u8)Prop(0x44));
    }
    return 0;
}
ScnObject *DaffyScene_Create(void *record)
{
    ScnBody *object = new DaffyScene;
    object = object->Init(record, 0);
    return object;
}
