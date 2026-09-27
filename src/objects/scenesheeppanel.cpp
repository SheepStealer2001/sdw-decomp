/*
 * T205 - original object SceneSheepPanel.cpp (guessed name), one translation unit.
 *   .text  0x4e8ad0-0x4e947e (SceneSheepPanel_PostLoadInit .. SceneSheepPanel_Create)
 *   .rdata 0x576a58-0x576a7c (??_7SceneSheepPanel)
 *   .data  0x57b710-0x57b71c (this TU's copy of the Cine header's 9-byte opcode stride table, + 3 pad)
 */
/* BYTES: layout, slot-scope. */
/* BYTES(layout): the Cine.h header static: every object including the cinematic header carries this copy in its .data, referenced or not */
/* BYTES(slot-scope, inferred): GetPropertyU32: the nested block(s) only order the frame: their locals are allocated after the enclosing scope's */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
struct CamSetup;
u32 Anim_Start(Instance *inst, Animator *animator, u16 id, u32 opts); /* 0x550196 */
#include "../engine/scn_tools.h"
#include "camera.h"
#include "../engine/progress.h"
#include "../engine/cine.h"
#include "../app/app_main.h"
#define SDW_INLINE_FREE_INSTFLAGS_SET_U16_U16 1
#include "instance_inlines.h"
#undef SDW_INLINE_FREE_INSTFLAGS_SET_U16_U16
#define SDW_INLINE_FREE_INSTFLAGS_CLEAR_U16_U16 1
#include "instance_inlines.h"
#undef SDW_INLINE_FREE_INSTFLAGS_CLEAR_U16_U16
#define SDW_INLINE_FREE_SCN_GETPROPU32_U16_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_SCN_GETPROPU32_U16_U32

#define SDW_MEMBERS_ScnObject                                                                           \
    static void *operator new(u32 size);                                                                \
    CamSetup *GetPropertyCamera(s32 offset)                                                             \
    {                                                                                                   \
        void *props = record;                                                                           \
        return Scn_GetPropCamera(props, offset);                                                        \
    }                                                                                                   \
    Box *GetPropertyBox(s32 offset)                                                                     \
    {                                                                                                   \
        void *props = record;                                                                           \
        return Scn_GetPropBox(props, offset);                                                           \
    }                                                                                                   \
    u32 GetPropertyU32(u32 offset)                                                                      \
    {                                                                                                   \
        u16 *props = record;                                                                            \
        {                                                                                               \
            u32 field = offset;                                                                         \
            /* cast kept: designer properties are 4-byte slots at byte offsets of the raw WAR record */ \
            u32 number = *(u32 *)((u8 *)props + field + 0x14);                                          \
            return number;                                                                              \
        }                                                                                               \
    }                                                                                                   \
    void SetUpdateMode(u8 mode);                                                                        \
    void SetTint(u32 color, s16 amount, s32 on);                                                        \
    void StartCamera(u16 x, u16 y, u16 z, Vec3s *eye, u16 focal, u32 mode, s32 time);


#define SDW_MEMBERS_Progress       \
    s8 GetSheepToCatch()           \
    {                              \
        return sheepToCatch;       \
    }                              \
    void SetSheepToCatch(s8 count) \
    {                              \
        sheepToCatch = count;      \
    }

#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_SETVISIBLE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETVISIBLE_S32
#define SDW_INLINE_SCNOBJECT_GETFLAGS 1
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETFLAGS
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8
#define SDW_INLINE_SCNOBJECT_SETTINT_U32_S16_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETTINT_U32_S16_S32
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_CINE_ISACTIVE 1
#include "../engine/cine_inlines.h"
#undef SDW_INLINE_CINE_ISACTIVE
#define SDW_INLINE_PROGRESS_GETLEVELINDEXA 1
#define SDW_INLINE_PROGRESS_SETLEVELDONE_S8 1
#include "../engine/progress_inlines.h"
#undef SDW_INLINE_PROGRESS_GETLEVELINDEXA
#undef SDW_INLINE_PROGRESS_SETLEVELDONE_S8
/* 0x57b710 - this TU's copy of the cinematic header's static opcode stride table (payload bytes per key, by opcode;
   the copies src/engine/cine.cpp and cine2.cpp index are g_cineOpStride 0x5816fc, g_cineOpStride2 and
   g_cineOpStride3). A static table in a header that every cinematic .cpp includes: each includer gets its own
   unreferenced copy in .data, ahead of its own data. Local definition standing in for that header, named
   s_cineOpStride as in the other batches (a static must not reuse the global name g_cineOpStride). */
static u8 s_cineOpStride[9] = {0, 8, 8, 4, 2, 2, 4, 2, 2};

s32 Scenaric_FindByClass(u16 classId, ScnObject **out, s32 maximum); /* 0x5145c5 */
void Camera_StartScripted(ScnObject *owner, Camera *cam, u16 x, u16 y, u16 z, Vec3s *pos, u16 focal, u32 mode,
                          s32 time); /* 0x55a70d */
extern Wolf *g_pWolf;
extern s32 g_dtMs;

inline s32 HasReturnedSheep(s8 index)
{
    return index >= 0 && index < 32;
}

static inline s32 Camera_IsScriptTransition()
{
    return g_camMode == CAM_SCRIPT_BLEND_IN || g_camMode == CAM_SCRIPT_TO_SCRIPT || g_camMode == CAM_SCRIPT_RETURN;
}

#define SDW_INLINE_FREE_BOX_CONTAINSPOINT_BOX_VEC3S 1
#include "../engine/coll_box_inlines.h"
#undef SDW_INLINE_FREE_BOX_CONTAINSPOINT_BOX_VEC3S

#define SDW_INLINE_SCNOBJECT_STARTCAMERA_U16_U16_U16_VEC3S_U16_U32_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_STARTCAMERA_U16_U16_U16_VEC3S_U16_U32_S32

#define SDW_INLINE_FREE_STARTCINE_U32_U32_BOX_BOX_VOID 1
#include "../engine/cine_inlines.h"
#undef SDW_INLINE_FREE_STARTCINE_U32_U32_BOX_BOX_VOID

/* Local reconstruction of the singleton flag update at 0x4e909a. */
static inline void SetTimeKeeperUnsaved(u32 on)
{
    Progress *progress;
    u32 number = on;
    progress = g_pProgress;
    progress->runtimeBits.timeKeeperUnsaved = number;
}

/* 0x4e8ad0 */
void SceneSheepPanel::PostLoadInit()
{
    camShot = GetPropertyCamera(0);
    sheepBox = GetPropertyBox(8);
    cineId = (u16)GetPropertyU32(4);
    Reset();
}

/* 0x4e8b49 */
void SceneSheepPanel::Update()
{
    switch (state) {
        case SHEEPPANEL_ST_INIT:
            Scenaric_FindByClass(CLASSID_SHEEP, &sheep, 1);
            if (!HasReturnedSheep(g_pProgress->GetLevelIndexA())) {
                sheep->SetVisible(0);
                sheep->RemoveFromWorld();
            } else {
                StartCine(cineId, CINE_CAM_CUT_IN | CINE_CAM_CUT_OUT | CINE_LETTERBOX, 0, 0, 0);
                g_pProgress->SetLevelDone(g_pProgress->GetLevelIndexA());
            }
            state = SHEEPPANEL_ST_IDLE;
            break;
        case SHEEPPANEL_ST_IDLE:
            if (!g_cinePlayer.IsActive()) {
                if (g_pProgress->runtimeBits.timeKeeperUnsaved) {
                    if (!HasReturnedSheep(g_pProgress->GetLevelIndexA())) {
                        if (g_pWolf)
                            g_pWolf->HandleMessage(this, MSG_FREEZE, 0);
                        state = SHEEPPANEL_ST_FINISH;
                    }
                }
                if (Box_ContainsPoint(sheepBox, &sheep->pos) && !(sheep->GetFlags() & SCN_OF_HIDDEN) &&
                    sheep->HandleMessage(this, MSG_SHEEP_QUERY_AVAILABLE, 0)) {
                    StartCamera(camShot->rot[0], camShot->rot[1], camShot->rot[2], &camShot->eye, camShot->focal,
                                CAMSCR_BLEND_IN, 4000);
                    if (g_pWolf)
                        g_pWolf->HandleMessage(this, MSG_FREEZE, 0);
                    state = SHEEPPANEL_ST_COUNT_DOWN;
                }
            }
            break;
        case SHEEPPANEL_ST_COUNT_DOWN:
            if (!Camera_IsScriptTransition()) {
                sheep->SetVisible(0);
                sheep->RemoveFromWorld();
                g_pProgress->SetSheepToCatch(g_pProgress->GetSheepToCatch() - 1);
                PlayAnim(g_pProgress->GetSheepToCatch(), 1, 1);
                timerMs = 0;
                state = SHEEPPANEL_ST_DOOR;
            }
            break;
        case SHEEPPANEL_ST_DOOR:
            timerMs += (u16)g_dtMs;
            if (doorWorldReply == -1 && timerMs >= 1000)
                doorWorldReply = (s8)Scenaric_SendToClass(CLASSID_DOORWORLD, MSG_DOORWORLD_OPEN, 0);
            if (doorWorldReply == 0 && timerMs >= 1000)
                state = SHEEPPANEL_ST_FINISH;
            else if ((s8)doorSequenceDone)
                state = SHEEPPANEL_ST_FINISH;
            break;
        case SHEEPPANEL_ST_FINISH:
            if (!Camera_IsScriptTransition()) {
                if (g_pWolf)
                    g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0);
                if (!Scenaric_SendToClass(CLASSID_MCARDMANAGER, MSG_MCARD_AUTOSAVE, 0))
                    Camera_ReleaseAny();
                SetTimeKeeperUnsaved(0);
                SetUpdateMode(SCN_UPD_NORMAL);
                state = SHEEPPANEL_ST_IDLE;
            }
            break;
    }
    AdvanceAnim();
}

/* 0x4e91a3 */
s32 SceneSheepPanel::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    switch (msgId) {
        case MSG_DOOR_SEQUENCE_DONE:
            SetUpdateMode(SCN_UPD_ALWAYS);
            doorSequenceDone = 1;
            return 1;
    }
    return 0;
}

/* 0x4e928a */
void SceneSheepPanel::Reset()
{
    PlayAnim(g_pProgress->GetSheepToCatch(), 0, 0);
    SetUpdateMode(SCN_UPD_CINE);
    SetTint(0xc0ff, 0xc00, 1);
    /* cast kept: the original stores this poison address until Update finds the sheep */
    sheep = (ScnObject *)0xcacbcccd;
    doorSequenceDone = 0;
    doorWorldReply = -1;
    state = SHEEPPANEL_ST_INIT;
}

/* 0x4e941a */
ScnObject *SceneSheepPanel_Create(void *record)
{
    ScnBody *obj = new SceneSheepPanel;
    obj = obj->Init(record, 0);
    return obj;
}
