/*
 * T183 - original object MCardManager.cpp (guessed name), one translation unit.
 *   .text  0x4d42d0-0x4d4cd1 (MCardManager_PostLoadInit .. MCardManager_Create)
 *   .rdata 0x576624-0x576648 (??_7MCardManager)
 *   .data  0x57b6d0-0x57b6dc (this TU's copy of the Cine header's 9-byte opcode stride table, + 3 pad)
 */
/* BYTES: layout. */
/* BYTES(layout): the Cine.h header static: every object including the cinematic header carries this copy in its .data, referenced or not */
/* PAL PC 0x4d42d0-0x4d4cd0. */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class ScnObject;
class Camera;
class Instance;
struct Animator;
u32 Anim_Start(Instance *inst, Animator *animator, u16 id, u32 opts);
void Camera_StartScripted(ScnObject *owner, Camera *cam, u16 rx, u16 ry, u16 rz, Vec3s *eye, u16 focal, u32 mode,
                          s32 time);
#include "../app/app_main.h"
#include "../engine/scn_tools.h"
#include "camera.h"
#include "../engine/fade.h"
#include "mcard.h"
#include "../engine/progress.h"
#include "../engine/time.h"

#define SDW_MEMBERS_ScnObject            \
    static void *operator new(u32 size); \
    void SetUpdateMode(u8 mode);         \
    void StartCamera(u16 rx, u16 ry, u16 rz, Vec3s *eye, u16 focal, u32 mode);


#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_SETVISIBLE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_SETVISIBLE_S32
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8 1
#define SDW_INLINE_SCNOBJECT_STARTCAMERA_U16_U16_U16_VEC3S_U16_U32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8
#undef SDW_INLINE_SCNOBJECT_STARTCAMERA_U16_U16_U16_VEC3S_U16_U32
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_PROGRESS_CURRENTLEVEL 1
#include "../engine/progress_inlines.h"
#undef SDW_INLINE_PROGRESS_CURRENTLEVEL
/* 0x57b6d0 - this TU's copy of the cinematic header's static opcode stride table (payload bytes per key, by opcode;
   the copies src/engine/cine.cpp and cine2.cpp index are g_cineOpStride 0x5816fc, g_cineOpStride2 and
   g_cineOpStride3). A static table in a header that every cinematic .cpp includes: each includer gets its own
   unreferenced copy in .data, ahead of its own data. Local definition standing in for that header, named
   s_cineOpStride as in the other batches (a static must not reuse the global name g_cineOpStride). */
static u8 s_cineOpStride[9] = {0, 8, 8, 4, 2, 2, 4, 2, 2};

extern Wolf *g_pWolf;
extern u32 g_gameFlags;

#define SDW_INLINE_FREE_CLEARGAMEFLAGS_U32 1
#include "../engine/game_state_inlines.h"
#undef SDW_INLINE_FREE_CLEARGAMEFLAGS_U32

inline s32 CameraIsTransitioning()
{
    return g_camMode == CAM_SCRIPT_BLEND_IN || g_camMode == CAM_SCRIPT_TO_SCRIPT || g_camMode == CAM_SCRIPT_RETURN ? 1
                                                                                                                   : 0;
}

/* 0x4d42d0 MCardManager_PostLoadInit */
void MCardManager::PostLoadInit()
{
    void *rec = record;
    uiCamera = Scn_GetPropCamera(rec, 0);
    SetUpdateMode(SCN_UPD_NORMAL);
    PlayAnim(AMEMORY1_ANIM_STAND, 1, 0);
    if (g_pProgress->CurrentLevel() == SCENE_WHEEL)
        SetVisible(0);
    state = MCARDMGR_ST_IDLE;
}

/* 0x4d4445 MCardManager_Update */
void MCardManager::Update()
{
    u32 mode;
    switch (state) {
        case MCARDMGR_ST_SAVE_OPEN:
            if (uiCamera)
                StartCamera(uiCamera->rot[0], uiCamera->rot[1], uiCamera->rot[2], &uiCamera->eye, uiCamera->focal,
                            CAMSCR_BLEND_OUT);
            if (g_pWolf) {
                g_pWolf->HandleMessage(this, MSG_FREEZE, 0);
                g_pWolf->HandleMessage(this, MSG_WOLF_SET_OBJFLAG2, 0);
            }
            MCard_SetMode(MCARD_MODE_AUTOSAVE);
            state = MCARDMGR_ST_SAVE_RUN;
            break;
        case MCARDMGR_ST_SAVE_RUN:
            ClearGameFlags(GF_TRANSITION_IDLE);
            if (!CameraIsTransitioning()) {
                switch (Card_StateMachine()) {
                    case MCARD_FAILED:
                    case MCARD_CANCELLED:;
                    case MCARD_DONE:
                        if (uiCamera)
                            Camera_SetMode(CAM_SCRIPT_RETURN, 1);
                        state = MCARDMGR_ST_SAVE_CLOSE;
                        break;
                }
            }
            break;
        case MCARDMGR_ST_SAVE_CLOSE:
            if (!CameraIsTransitioning()) {
                Camera_ReleaseAny();
                if (g_pWolf) {
                    g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0);
                    /* cast kept: HandleMessage's arg is a void *; this message passes a number in it */
                    g_pWolf->HandleMessage(this, MSG_WOLF_SET_OBJFLAG2, (void *)1);
                }
                state = MCARDMGR_ST_IDLE;
                g_gameFlags |= GF_TRANSITION_IDLE;
            }
            break;
        case MCARDMGR_ST_AUTOSAVE_OPEN:
            keepCamera =
                g_camMode == CAM_SCRIPT_BLEND_IN || g_camMode == CAM_SCRIPT_TO_SCRIPT || g_camMode == CAM_SCRIPTED ? 1
                                                                                                                   : 0;
            mode = CAMSCR_BLEND_IN | CAMSCR_CHAIN | CAMSCR_BLEND_OUT_KEEP_ROT;
            if (uiCamera)
                StartCamera(uiCamera->rot[0], uiCamera->rot[1], uiCamera->rot[2], &uiCamera->eye, uiCamera->focal,
                            mode);
            if (g_pWolf) {
                g_pWolf->HandleMessage(this, MSG_FREEZE, 0);
                g_pWolf->HandleMessage(this, MSG_WOLF_SET_OBJFLAG2, 0);
            }
            state = MCARDMGR_ST_AUTOSAVE_RUN;
            break;
        case MCARDMGR_ST_AUTOSAVE_RUN:
            ClearGameFlags(GF_TRANSITION_IDLE);
            if (!CameraIsTransitioning()) {
                g_renderWorldFlag = 0;
                ClearGameFlags(GF_BIT0 | GF_RENDER_WORLD);
                switch (Card_StateMachine()) {
                    case MCARD_CANCELLED:
                        if (g_pProgress->CurrentLevel() == SCENE_WHEEL) {
                            Scenaric_SendToClass(CLASSID_SCENE_WHEEL, MSG_SCENEWHEEL_SAVE_DONE, 0);
                            Scenaric_SendToClass(CLASSID_DAFFYWHEEL, MSG_DAFFYWHEEL_RESET, 0);
                        } else if (uiCamera)
                            Camera_SetMode(CAM_SCRIPT_RETURN, 1);
                        exitAfterUi = 0;
                        state = MCARDMGR_ST_AUTOSAVE_CLOSE;
                        break;
                    case MCARD_FAILED:
                        if (uiCamera)
                            Camera_SetMode(CAM_SCRIPT_RETURN, 1);
                        exitAfterUi = 0;
                        state = MCARDMGR_ST_AUTOSAVE_CLOSE;
                        break;
                    case MCARD_DONE:
                        if (g_pProgress->CurrentLevel() == SCENE_HUB && uiCamera)
                            Camera_SetMode(CAM_SCRIPT_RETURN, 1);
                        if (g_pProgress->CurrentLevel() == SCENE_WHEEL)
                            Scenaric_SendToClass(CLASSID_DAFFYWHEEL, MSG_DAFFYWHEEL_RESET, 0);
                        exitAfterUi = 1;
                        state = MCARDMGR_ST_AUTOSAVE_CLOSE;
                        break;
                }
            }
            break;
        case MCARDMGR_ST_AUTOSAVE_CLOSE:
            g_renderWorldFlag = 1;
            g_gameFlags |= GF_RENDER_WORLD;
            if (g_pProgress->CurrentLevel() == SCENE_WHEEL)
                ClearGameFlags(GF_TRANSITION_IDLE);
            if (!CameraIsTransitioning()) {
                if (g_pProgress->CurrentLevel() != SCENE_WHEEL)
                    g_gameFlags |= GF_TRANSITION_IDLE;
                if (!keepCamera)
                    Camera_ReleaseAny();
                if (g_pWolf) {
                    g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0);
                    /* cast kept: HandleMessage's arg is a void *; this message passes a number in it */
                    g_pWolf->HandleMessage(this, MSG_WOLF_SET_OBJFLAG2, (void *)1);
                }
                state = MCARDMGR_ST_IDLE;
                if (g_mcardMode == MCARD_MODE_LOAD) {
                    switch (exitAfterUi) {
                        case 1:
                            Fade_StartLevelExit(0x1000);
                            break;
                    }
                }
            }
            break;
    }
    AdvanceAnim();
}

/* 0x4d4a66 MCardManager_HandleMessage */
s32 MCardManager::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    switch (msgId) {
        case MSG_QUERY_ACTION:
            if (!state) {
                if (sender->GetClassId() == CLASSID_WOLF) {
                    if (Vec3s_ManhattanDistXZ(&pos, &sender->pos) < 300)
                        return CTX_SAVE;
                } else if (sender->GetClassId() == CLASSID_CINEMATICSMANAGER)
                    return CTX_SAVE;
            }
            break;
        case MSG_USE:
            if (g_pProgress->CurrentLevel() == SCENE_HUB || g_pProgress->CurrentLevel() == SCENE_FEND)
                MCard_SetMode(MCARD_MODE_SAVE);
            else
                MCard_SetMode(MCARD_MODE_LOAD);
            state = MCARDMGR_ST_AUTOSAVE_OPEN;
            return 1;
        case MSG_MCARD_AUTOSAVE:
            if (g_pProgress->runtimeBits.saveSlotValid && g_pProgress->runtimeBits.autoSaveOn) {
                SetUpdateMode(SCN_UPD_ALWAYS);
                state = MCARDMGR_ST_SAVE_OPEN;
                return 1;
            }
            break;
    }
    return 0;
}

/* 0x4d4c62 MCardManager_Reset */
void MCardManager::Reset() {}

/* 0x4d4c6d MCardManager_Create */
ScnObject *MCardManager_Create(void *record)
{
    MCardManager *obj = new MCardManager;
    obj = (MCardManager *)obj->Init(record, 0); /* cast kept: Init returns the ScnBody * base of this object */
    return obj;
}
