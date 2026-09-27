/* PAL PC SignTips. */

#define SDW_MEMBERS_ScnObject            \
    static void *operator new(u32 size); \
    void SetUpdateMode(u8 mode);         \
    void StartCamera(u16 rx, u16 ry, u16 rz, Vec3s *at, u16 focal);

#define SDW_MEMBERS_ZoneList void Load(u32 id);
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *inst, Animator *animator, u16 id, u32 opts);
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_SETVISIBLE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_SETVISIBLE_S32
#define SDW_INLINE_SCNOBJECT_SETCOLLIDABLE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETCOLLIDABLE_S32
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32

extern Wolf *g_pWolf;
#include "../app/app_main.h"
#include "../engine/input.h"
#include "../engine/id_list.h"
#include "../engine/scn_tools.h"
#include "../engine/interface.h"
#include "camera.h"
extern u32 g_gameFlags;
void Camera_StartScripted(ScnObject *owner, Camera *cam, u16 rx, u16 ry, u16 rz, Vec3s *at, u16 focal, u32 flags,
                          s32 time);

#define SDW_INLINE_FREE_SCN_GETPROPU32_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_SCN_GETPROPU32_VOID_U32
#define SDW_INLINE_FREE_SCREENWIDTHS16 1
#include "../engine/screen_inlines.h"
#undef SDW_INLINE_FREE_SCREENWIDTHS16
inline s16 Screen_Height()
{
    return 240;
}
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8 1
#define SDW_INLINE_SCNOBJECT_STARTCAMERA_U16_U16_U16_VEC3S_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8
#undef SDW_INLINE_SCNOBJECT_STARTCAMERA_U16_U16_U16_VEC3S_U16
#define SDW_INLINE_ZONELIST_LOAD_U32 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_LOAD_U32
#define SDW_INLINE_FREE_BOX_CONTAINSPOINTXZ_BOX_VEC3S 1
#include "../engine/coll_box_inlines.h"
#undef SDW_INLINE_FREE_BOX_CONTAINSPOINTXZ_BOX_VEC3S
#define SDW_INLINE_FREE_BOX_CONTAINSPOINT_BOX_VEC3S 1
#include "../engine/coll_box_inlines.h"
#undef SDW_INLINE_FREE_BOX_CONTAINSPOINT_BOX_VEC3S

/* 0x4f1eb0. trajBoxes is the ZoneList at +0xa0. */
void SignTips::PostLoadInit()
{
    void *rec = record;
    u8 slot = (u8)Scn_GetPropU32(rec, 12);
    tipText = Text_GetClassString(slot);
    camRecord = Scn_GetPropCamera(rec, 0);
    checkpointBox = Scn_GetPropBox(rec, 4);
    if (checkpointBox) {
        rebirthsNeeded = (u8)Scn_GetPropU32(rec, 16);
        rebirthCount = 0;
        SetState(SIGNTIPS_ST_COUNT_REBIRTHS);
    } else {
        triggerDist = Scn_GetPropU32(rec, 8);
        trajBoxes.Load(Scn_GetPropU32(rec, 20));
        for (u16 i = 0; i < trajBoxes.count; i++)
            boxVisited[i] = 0;
        SetState(SIGNTIPS_ST_TRACK_BOXES);
    }
    SetVisible(0);
    SetCollidable(0);
    textRect[0] = 50;
    textRect[1] = 50;
    textRect[2] = ScreenWidthS16() - 100;
    textRect[3] = Screen_Height() - 100;
    SetUpdateMode(SCN_UPD_ALWAYS);
}

/* 0x4f2143 */
void SignTips::Reset()
{
    if (state == SIGNTIPS_ST_COUNT_REBIRTHS) {
        if (Box_ContainsPointXZ(checkpointBox, &g_pWolf->pos)) {
            rebirthCount++;
            if (rebirthCount >= rebirthsNeeded) {
                if (camRecord) {
                    wolfFrozen = g_pWolf->HandleMessage(this, MSG_FREEZE, 0);
                    if (wolfFrozen)
                        StartCamera(camRecord->rot[0], camRecord->rot[1], camRecord->rot[2], &camRecord->eye,
                                    camRecord->focal);
                }
                SetState(SIGNTIPS_ST_WAIT_FADE);
            }
        }
    }
}

/* 0x4f22ab. SIGNTIPS_ST_TRACK_BOXES returns before AdvanceAnim after either visited-box exit. */
void SignTips::Update()
{
    u16 i, j;
    switch (state) {
        case SIGNTIPS_ST_WAIT_FADE:
            if (!(g_gameFlags & GF_FADE_IN))
                SetState(SIGNTIPS_ST_APPEAR);
            break;
        case SIGNTIPS_ST_APPEAR:
            if (AnimFlags(ANIM_F_FINISHED)) {
                SetState(SIGNTIPS_ST_IDLE);
                Camera_ReleaseScripted(this);
                if (wolfFrozen)
                    wolfFrozen = !g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0);
            }
            break;
        case SIGNTIPS_ST_OPEN_WAIT:
            if (!Pad_MenuPressed((u16)~PAD_CROSS))
                SetState(SIGNTIPS_ST_SHOW_TIP);
            break;
        case SIGNTIPS_ST_SHOW_TIP:
            Ui_DrawSubtitleBox(tipText, textRect, DAV_IDI_IGLCADP_);
            if (Pad_MenuPressed((u16)~PAD_CROSS))
                SetState(SIGNTIPS_ST_CLOSE_WAIT);
            break;
        case SIGNTIPS_ST_CLOSE_WAIT:
            if (!Pad_MenuPressed((u16)~PAD_CROSS)) {
                if (wolfFrozen)
                    wolfFrozen = !g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0);
                SetState(SIGNTIPS_ST_IDLE);
            }
            break;
        case SIGNTIPS_ST_TRACK_BOXES:
            for (i = 0; i < trajBoxes.count; i++) {
                if (Box_ContainsPoint(trajBoxes.boxes[i], &g_pWolf->pos)) {
                    boxVisited[i] = 1;
                    for (j = 0; j < trajBoxes.count; j++) {
                        if (!boxVisited[j])
                            return;
                    }
                    SetState(SIGNTIPS_ST_WAIT_DISTANCE);
                    return;
                }
            }
            break;
        case SIGNTIPS_ST_WAIT_DISTANCE:
            if ((u32)g_pWolf->HandleMessage(this, MSG_WOLF_GET_DISTANCE, 0) > triggerDist) {
                if (camRecord)
                    StartCamera(camRecord->rot[0], camRecord->rot[1], camRecord->rot[2], &camRecord->eye,
                                camRecord->focal);
                SetState(SIGNTIPS_ST_APPEAR);
            }
            break;
    }
    AdvanceAnim();
}

/* 0x4f2615 */
s32 SignTips::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    switch (msgId) {
        case MSG_USE:
            if (sender->GetClassId() == CLASSID_WOLF) {
                wolfFrozen = g_pWolf->HandleMessage(this, MSG_FREEZE, 0);
                SetState(SIGNTIPS_ST_OPEN_WAIT);
                return 1;
            }
            break;
        case MSG_QUERY_ACTION:
            if (sender->GetClassId() == CLASSID_WOLF && (state == SIGNTIPS_ST_APPEAR || state == SIGNTIPS_ST_IDLE))
                return CTX_READ;
            break;
        case MSG_FREEZE:
            wolfFrozen = 0;
            SetState(SIGNTIPS_ST_IDLE);
            return 1;
    }
    return 0;
}

/* 0x4f26d7 */
void SignTips::SetState(u8 value)
{
    state = value;
    switch (state) {
        case SIGNTIPS_ST_IDLE:
            PlayAnim(SIGNTIPS_ANIM_IDLE, 0, 1);
            break;
        case SIGNTIPS_ST_APPEAR:
            PlayAnim(SIGNTIPS_ANIM_APPEAR, 0, 0);
            SetUpdateMode(SCN_UPD_NORMAL);
            SetVisible(1);
            SetCollidable(1);
            break;
    }
}

/* 0x4f28ab */
ScnObject *SignTips_Create(void *record)
{
    ScnBody *obj = new SignTips;
    obj = obj->Init(record, 0);
    return obj;
}
