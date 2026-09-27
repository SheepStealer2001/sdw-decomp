/* PAL PC Battery, 0x496190-0x4971ed. */
#define SDW_MEMBERS_Box                                                                              \
    s32 ContainsXZ(const Vec3s *point)                                                               \
    {                                                                                                \
        return point->x >= min[0] && point->x <= max[0] && point->z >= min[2] && point->z <= max[2]; \
    }
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
#define SDW_MEMBERS_ScnObject       \
    static void *operator new(u32); \
    void SetUpdateMode(s32 mode);

#define SDW_MEMBERS_Mat44 Mat44();
#include "sdw_classes.h"
#define SDW_INLINE_SCNOBJECT_SETVISIBLE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETVISIBLE_S32
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32
#define SDW_INLINE_UIQUAD_SETCOLOR_U32 1
#include "../engine/ui_quad_inlines.h"
#undef SDW_INLINE_UIQUAD_SETCOLOR_U32
extern Wolf *g_pWolf;
#include "../app/app_main.h"
#include "../engine/scn_tools.h"
#include "../engine/id_list.h"
#include "../engine/lerp.h"

#define g_camPos (g_camera.pos)

extern s32 g_dt, g_dtMs;
s32 Scenaric_FindByClass(u16, ScnObject **, s32);
s32 Vec3s_DistSqXZ(Vec3s *, Vec3s *);
u16 Sound_Play(u16, void *, u16, u8, s32);
#define SDW_INLINE_FREE_SCREENWIDTHS16 1
#include "../engine/screen_inlines.h"
#undef SDW_INLINE_FREE_SCREENWIDTHS16
#define SDW_INLINE_FREE_SCREENHEIGHTS16 1
#include "../engine/screen_inlines.h"
#undef SDW_INLINE_FREE_SCREENHEIGHTS16
void Battery::PostLoadInit()
{
    u16 count;
    void *properties = record;
    /* cast kept: a downcast: the MASTER property names another Battery (Scn_GetPropObject returns ScnObject *) */
    master = (Battery *)Scn_GetPropObject(properties, 4);
    if (master)
        isMaster = 0;
    else {
        isMaster = 1;
        master = this;
    }
    hudFullBitmap = IdList_FindWithCount(DAV_IDI_IASPILEA, &count);
    hudEmptyBitmap = IdList_FindWithCount(DAV_IDI_IASPILEB, &count);
    if (isMaster) {
        triggerBox = Scn_GetPropBox(properties, 0);
        wolfInBox = triggerBox->ContainsXZ(&g_pWolf->pos);
        Scenaric_FindByClass(CLASSID_HOOVER, &hoover, 1);
        hudOffsetY = 30;
        Scenaric_FindByClass(CLASSID_GHOST, &ghost, 1);
        ghost->HandleMessage(this, MSG_GHOST_GET_SUCK_MS, &chargeDuration);
        SetUpdateMode(SCN_UPD_ALWAYS);
    }
    collectedCount = 0;
    reported = 0;
    charged = 0;
    draining = 0;
    hudState = BATTERY_ST_HUD_7;
    soundHandle = 0;
    homePos = pos;
    renderRot.x = 0;
    renderRot.y = 0x800;
    renderRot.z = 0;
    rot = renderRot;
    SetState(BATTERY_ST_HOME);
}
void Battery::Reset()
{
    collectedCount = 0;
    reported = 0;
    charged = 0;
    draining = 0;
    if (isMaster && triggerBox->ContainsXZ(&g_pWolf->pos))
        wolfInBox = 0;
    soundHandle = 0;
    SetPosition(&homePos);
    SetState(BATTERY_ST_HOME);
}
void Battery::Update()
{
    s16 index;
    if (isMaster) {
        if (draining) {
            drainTimer -= g_dtMs;
            if (drainTimer <= 0) {
                if (collectedCount > 0)
                    collectedCount--;
                drainTimer = chargeDuration / 5;
            }
        } else
            unknown138 = 1;
        if (triggerBox->ContainsXZ(&g_pWolf->pos) == 1 && wolfInBox == 0) {
            hudState = BATTERY_ST_HUD_SHOW;
            wolfInBox = 1;
        } else if (triggerBox->ContainsXZ(&g_pWolf->pos) == 0 && wolfInBox == 1) {
            hudState = BATTERY_ST_HUD_HIDE;
            wolfInBox = 0;
        }
        switch (hudState) {
            case BATTERY_ST_HUD_SHOW:
                if (hudOffsetY > 0)
                    hudOffsetY -= 4;
                else
                    hudOffsetY = 0;
                break;
            case BATTERY_ST_HUD_HIDE:
                if (hudOffsetY < 30)
                    hudOffsetY += 4;
                else
                    hudOffsetY = 30;
                break;
        }
        /* cast kept (the three (u16 *) entries below): an id list holds untyped record pointers; these are bitmap
         * ids */
        for (index = 0; index < 5; index++) {
            if (index == collectedCount - 1)
                /* cast kept: an export id list entry points at this bitmap */
                hudSlots[index].UiQuad_SetFromBitmap((u16 *)*hudFullBitmap, index * 25,
                                                     hudOffsetY + ScreenHeightS16() - 30, 0, 0, 1024, 1024);
            else if (index < collectedCount)
                hudSlots[index].UiQuad_SetFromBitmap((u16 *)*hudFullBitmap, index * 25,
                                                     hudOffsetY + ScreenHeightS16() - 30, 0, 0, 1024, 1024);
            else
                hudSlots[index].UiQuad_SetFromBitmap((u16 *)*hudEmptyBitmap, index * 25,
                                                     hudOffsetY + ScreenHeightS16() - 30, 0, 0, 1024, 1024);
            hudSlots[index].SetColor(0x808080);
            hudSlots[index].UiQuad_Draw(0xb);
        }
        if (g_pWolf->HandleMessage(this, MSG_WOLF_IS_DEAD, 0))
            hudState = BATTERY_ST_HUD_HIDE;
    }
    switch (state) {
        case BATTERY_ST_IDLE:
            if (master->HandleMessage(this, MSG_BATTERY_CAN_COLLECT, 0) && Vec3s_DistSqXZ(&g_pWolf->pos, &pos) < 2500)
                SetState(BATTERY_ST_FLY_TO_HUD);
            break;
        case BATTERY_ST_HOME:
            if (master->HandleMessage(this, MSG_BATTERY_CAN_COLLECT, 0) && Vec3s_DistSqXZ(&g_pWolf->pos, &pos) < 2500)
                SetState(BATTERY_ST_FLY_TO_HUD);
            scale += 163;
            if (scale >= 4096)
                SetState(BATTERY_ST_IDLE);
            break;
        case BATTERY_ST_FLY_TO_HUD:
            if (StepCollectFlight())
                SetState(BATTERY_ST_COLLECTED);
            break;
        case BATTERY_ST_FULL_SCALE:
            if (scale >= 0)
                scale -= 585;
            else
                SetVisible(0);
            break;
    }
}
s32 Battery::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    switch (msgId) {
        case MSG_BATTERY_COLLECTED:
            collected[collectedCount] = sender;
            collectedCount++;
            if (collectedCount == 5) {
                hoover->HandleMessage(this, MSG_HOOVER_CHARGED, 0);
                charged = 1;
            }
            break;
        case MSG_BATTERY_GET_COUNT:
            return collectedCount;
        case MSG_BATTERY_CAN_COLLECT:
            return collectedCount < 5 && !draining;
        case MSG_BATTERY_SHOW:
            if (arg)
                SetState(BATTERY_ST_HOME);
            else
                SetState(BATTERY_ST_FULL_SCALE);
            break;
        case MSG_BATTERY_DRAIN:
            if (arg)
                drainTimer = chargeDuration / 5;
            else {
                collectedCount = 0;
                if (collectedCount == 0)
                    hoover->HandleMessage(this, MSG_HOOVER_DISCHARGED, 0);
            }
            draining = (s32)arg; /* cast kept: the message arg is a void *: what it carries depends on the message id */
            break;
    }
    return 0;
}
void Battery::Render(Camera *view)
{
    Vec3s scaleVector;
    scaleVector.x = scale;
    scaleVector.y = scale;
    scaleVector.z = scale;
    RenderScaled(view, &scaleVector);
}
void Battery::SetState(u8 value)
{
    state = value;
    switch (value) {
        case BATTERY_ST_IDLE:
            SetVisible(1);
            break;
        case BATTERY_ST_HOME:
            scale = 0;
            SetVisible(1);
            SetPosition(&homePos);
            renderRot.x = 0;
            renderRot.y = 0;
            renderRot.z = 0;
            rot = renderRot;
            break;
        case BATTERY_ST_FLY_TO_HUD:
            soundHandle = Sound_Play(SND_BATTERY_PICKUP, this, 0x3f,
                                     SNDF_POSITIONAL | SNDF_DIST_HORIZONTAL | SNDF_NO_RETRIGGER, 4096);
            scale = 4096;
            flySpin = (s16)((renderRot.x + 1024) & 4095) - 2048;
            flyStart.x = pos.x;
            flyStart.y = pos.y;
            flyStart.z = pos.z;
            if (isMaster) {
                hudTarget.x = collectedCount * 25 - ScreenWidthS16() / 2;
                hudTarget.y = ScreenHeightS16() * 2 / 3;
                hudTarget.z = 0;
            } else {
                hudTarget.x = master->HandleMessage(this, MSG_BATTERY_GET_COUNT, 0) * 25 - ScreenWidthS16() / 2;
                hudTarget.y = ScreenHeightS16() * 2 / 3;
                hudTarget.z = 0;
            }
            flyT = 0;
            break;
        case BATTERY_ST_FULL_SCALE:
            scale = 4096;
            break;
        case BATTERY_ST_COLLECTED:
            if (!reported) {
                SetVisible(0);
                master->HandleMessage(this, MSG_BATTERY_COLLECTED, 0);
                reported = 1;
            }
            break;
    }
}
s32 Battery::StepCollectFlight()
{
    u16 delta = 0;
    Vec3s target;
    delta = (u16)(g_dt * 9600 >> 12);
    if (flyT < 3800) {
        Mat44 matrix;
        g_camera.viewMat.Transpose3x3(&matrix);
        matrix.m[3][0] = matrix.m[3][1] = matrix.m[3][2] = 0.0f;
        target.x = (s16)(hudTarget.x * matrix.m[0][0] + hudTarget.y * matrix.m[1][0] + hudTarget.z * matrix.m[2][0] +
                         matrix.m[3][0]);
        target.y = (s16)(hudTarget.x * matrix.m[0][1] + hudTarget.y * matrix.m[1][1] + hudTarget.z * matrix.m[2][1] +
                         matrix.m[3][1]);
        target.z = (s16)(hudTarget.x * matrix.m[0][2] + hudTarget.y * matrix.m[1][2] + hudTarget.z * matrix.m[2][2] +
                         matrix.m[3][2]);
        target.x += g_camPos.x;
        target.y += g_camPos.y;
        target.z += g_camPos.z;
        Lerp_SetVecTarget(&target);
        Vec3s_LerpToTarget(&flyPosition, &flyStart, (s16)flyT);
        flyT += delta;
        renderRot.x += (s16)(flySpin * (s16)delta / 4095);
        rot = renderRot;
        SetPosition(&flyPosition);
        return 0;
    } else
        return 1;
}
ScnObject *Battery_Create(void *record)
{
    Battery *object = new Battery;
    object = (Battery *)object->Init(record, 0); /* cast kept: Init returns the ScnBody * base of this object */
    return object;
}
