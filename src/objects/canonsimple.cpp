/* PAL PC CanonSimple 0x4a7140-0x4a88bd. */
/* BYTES: slot-scope. */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
#define SDW_MEMBERS_ScnObject                           \
    static void *operator new(u32);                     \
    void SetRotation(Vec3s *angle);                     \
    void StartCamera(u16, u16, u16, Vec3s *, u16, u32); \
    void SetUpdateMode(s32 mode);
#include "sdw_classes.h"
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_SETROTATION_VEC3S 1
#define SDW_INLINE_SCNOBJECT_SETVISIBLE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_SETROTATION_VEC3S
#undef SDW_INLINE_SCNOBJECT_SETVISIBLE_S32
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32
extern Wolf *g_pWolf;
#include "../engine/input.h"
#include "../app/app_main.h"
#include "../engine/scn_tools.h"
#include "camera.h"
#include "../engine/interface.h"
extern s32 g_dtMs;
extern u32 *g_screenLayerBase;

#define g_padCurButtons (g_pad.cur.buttons)

#define g_padPrevButtons (g_pad.prev.buttons)

#define g_cullPlaneNormal (g_camera.viewMatS.rot + 6)

s32 Scenaric_FindByClass(u16, ScnObject **, s32);
void Camera_StartScripted(ScnObject *, Camera *, u16, u16, u16, Vec3s *, u16, u32, s32);
#define SDW_INLINE_SCNOBJECT_STARTCAMERA_U16_U16_U16_VEC3S_U16_U32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_STARTCAMERA_U16_U16_U16_VEC3S_U16_U32
#define SDW_INLINE_FREE_READINTPROPERTY_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_READINTPROPERTY_VOID_U32
#define SDW_INLINE_FREE_BOXCONTAINSXZ_BOX_VEC3S 1
#include "../engine/coll_box_inlines.h"
#undef SDW_INLINE_FREE_BOXCONTAINSXZ_BOX_VEC3S
#define SDW_INLINE_FREE_SCREENWIDTHS32 1
#include "../engine/screen_inlines.h"
#undef SDW_INLINE_FREE_SCREENWIDTHS32
#define SDW_INLINE_FREE_SCREENHEIGHTS32 1
#include "../engine/screen_inlines.h"
#undef SDW_INLINE_FREE_SCREENHEIGHTS32
#define ABS_VALUE(v) ((v) >= 0 ? (v) : -(v))
/* BYTES(slot-scope, inferred): the nested block(s) only order the frame: their locals are allocated after the enclosing scope's */
void CanonSimple::PostLoadInit()
{
    void *props = record;
    {
        s32 b, a;
        dpadStep = 5;
        stickPitchStep = 0;
        stickYawStep = 0;
        stickRepeatMs = 0;
        dpadRepeatMs = 0;
        csFlags = 0;
        csFlags |= CSF_ANALOG_ACCEL;
        pipeCount = (u16)Scenaric_FindByClass(CLASSID_PIPE, pipes, 5);
        pipe2Count = (u16)Scenaric_FindByClass(CLASSID_PIPE2, pipes2, 5);
        state = CS_ST_IDLE;
        SnapToGround(0);
        basePos = pos;
        lastPos = basePos;
        activationBox = Scn_GetPropBox(props, 0);
        barrel = 0;
        barrel = Scn_GetPropObject(props, 0x10);
        barrel->SetPosition(&basePos);
        ball = 0;
        ball = Scn_GetPropObject(props, 0xc);
        ball->HandleMessage(this, MSG_CB_PLACE, &muzzlePos);
        MoveTo(&basePos);
        horzLimit = (u16)ReadIntProperty(props, 0x14);
        vertLimit = (u16)ReadIntProperty(props, 0x18);
        horzLimit = (horzLimit << 12) / 360;
        vertLimit = (vertLimit << 12) / 360;
        b = ReadIntProperty(props, 4);
        a = ReadIntProperty(props, 8);
        defaultAim.x = ((b * 0xfff) / 360) & 0xfff;
        defaultAim.y = ((a * 0xfff) / 360) & 0xfff;
        defaultAim.z = 0;
        aim = defaultAim;
        aimOffset.x = 0;
        aimOffset.y = 0;
        aimOffset.z = 0;
        ApplyAim(&aim);
        rumbleMotors[0] = 0xfe;
        rumbleMotors[1] = 0xff;
        SetUpdateMode(SCN_UPD_ALWAYS);
    }
}
void CanonSimple::Reset()
{
    csFlags &= ~CSF_RUNTIME_MASK;
    csFlags &= ~(CSF_FIRE_ARMED | CSF_AIM_CAM_STARTED | CSF_AIM_CHANGED | CSF_WOLF_FROZEN);
    if (csFlags & CSF_RESET_AIM) {
        aim = defaultAim;
        aimOffset.x = 0;
        aimOffset.y = 0;
        aimOffset.z = 0;
        ApplyAim(&aim);
    }
    state = CS_ST_IDLE;
}
void CanonSimple::Update()
{
    switch (state) {
        case CS_ST_AIM:
            UpdateAim(&g_pad);
            Hud_DrawCannonMask();
            break;
        case CS_ST_FIRING:
            fireDelayMs -= g_dtMs;
            if (fireDelayMs <= 0)
                SetState(CS_ST_FIRED);
            break;
    }
}
s32 CanonSimple::HandleMessage(ScnObject *sender, u32 msg, void *)
{
    if (!sender)
        return 0;
    if (sender->GetClassId() == CLASSID_WOLF) {
        switch (msg) {
            case MSG_QUERY_ACTION:
                switch (state) {
                    case CS_ST_IDLE:
                        if (BoxContainsXZ(activationBox, &g_pWolf->pos))
                            return CTX_CANNON;
                        return CTX_NONE;
                    case CS_ST_AIM:
                        return CTX_NONE;
                    case CS_ST_FIRED:
                        return CTX_NONE;
                }
                break;
            case MSG_USE:
                csFlags &= ~(CSF_ABORTED | CSF_FIRE_ARMED);
                SetState(CS_ST_AIM);
                return 1;
            case MSG_FREEZE:
                csFlags |= CSF_ABORTED;
                SetState(CS_ST_IDLE);
                ball->HandleMessage(this, MSG_CB_RELEASE_CAMERA, 0);
                return 1;
        }
    } else if (sender->GetClassId() == CLASSID_CANNONBALL) {
        switch (msg) {
            case MSG_BALL_RETURNED:
                ball->HandleMessage(this, MSG_CB_PLACE, &muzzlePos);
                if (!(csFlags & CSF_ABORTED))
                    SetState(CS_ST_IDLE);
                break;
            case MSG_FREEZE:
                if (!(csFlags & CSF_ABORTED) && (csFlags & CSF_WOLF_FROZEN)) {
                    csFlags &= ~CSF_WOLF_FROZEN;
                    g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0);
                }
                return 1;
        }
    }
    return 0;
}
void CanonSimple::SetState(u8 next)
{
    u16 i;
    state = next;
    switch (next) {
        case CS_ST_IDLE:
            if (!(csFlags & CSF_ABORTED) && (csFlags & CSF_WOLF_FROZEN)) {
                csFlags &= ~CSF_WOLF_FROZEN;
                g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0);
            }
            Reset();
            StartCamera(0, aim.y & 0xfff, 0, &muzzlePos, 800, 0);
            Camera_ReleaseScripted(this);
            /* cast kept: MSG_WOLF_SET_INVISIBLE's arg is 0 (hide) or 1 (show), a number in the void * */
            g_pWolf->HandleMessage(this, MSG_WOLF_SET_INVISIBLE, (void *)1);
            SetVisible(1);
            barrel->SetVisible(1);
            break;
        case CS_ST_AIM:
            ball->HandleMessage(this, MSG_CB_SET_LETHAL, 0);
            g_pWolf->HandleMessage(this, MSG_FREEZE, 0);
            csFlags |= CSF_WOLF_FROZEN;
            csFlags &= ~CSF_AIM_CAM_STARTED;
            g_pWolf->HandleMessage(this, MSG_WOLF_SET_INVISIBLE, 0);
            SetVisible(0);
            barrel->SetVisible(0);
            break;
        case CS_ST_FIRED:
            for (i = 0; i < pipeCount; i++)
                pipes[i]->HandleMessage(this, MSG_PIPE_ARM, 0);
            for (i = 0; i < pipe2Count; i++)
                pipes2[i]->HandleMessage(this, MSG_PIPE_ARM, 0);
            g_pad.Rumble_stub(250, rumbleMotors, 0x1000);
            ball->HandleMessage(this, MSG_CB_SET_LETHAL, 0);
            ball->HandleMessage(this, MSG_CB_LAUNCH, &aim);
            break;
        case CS_ST_FIRING:
            SetVisible(1);
            barrel->SetVisible(1);
            /* cast kept: MSG_WOLF_SET_INVISIBLE's arg is 0 (hide) or 1 (show), a number in the void * */
            g_pWolf->HandleMessage(this, MSG_WOLF_SET_INVISIBLE, (void *)1);
            barrel->HandleMessage(this, MSG_CD_RECOIL, 0);
            /* cast kept: MSG_CB_SET_OWNS_CAMERA's arg is a flag in the void * */
            ball->HandleMessage(this, MSG_CB_SET_OWNS_CAMERA, (void *)1);
            ball->HandleMessage(this, MSG_CB_LOAD, &aim);
            fireDelayMs = 400;
            break;
    }
}
/* BYTES(slot-scope, inferred): the nested block(s) only order the frame: their locals are allocated after the enclosing scope's */
void CanonSimple::UpdateAim(Pad *pad)
{
    s32 x;
    {
        s32 divisor;
        {
            s32 horizontal;
            {
                s32 vertical;
                {
                    s32 y;
                    y = 0;
                    x = 0;
                    horizontal = 0;
                    vertical = 0;
                    Ui_DrawFlatRect(g_screenLayerBase, (ScreenWidthS32() >> 1) - 7, (ScreenHeightS32() >> 1) - 1,
                                    (ScreenWidthS32() >> 1) - 3, (ScreenHeightS32() >> 1) + 1, 0xffffff);
                    Ui_DrawFlatRect(g_screenLayerBase, (ScreenWidthS32() >> 1) + 3, (ScreenHeightS32() >> 1) - 1,
                                    (ScreenWidthS32() >> 1) + 7, (ScreenHeightS32() >> 1) + 1, 0xffffff);
                    Ui_DrawFlatRect(g_screenLayerBase, (ScreenWidthS32() >> 1) - 1, (ScreenHeightS32() >> 1) - 5,
                                    (ScreenWidthS32() >> 1) + 1, (ScreenHeightS32() >> 1) - 2, 0xffffff);
                    Ui_DrawFlatRect(g_screenLayerBase, (ScreenWidthS32() >> 1) - 1, (ScreenHeightS32() >> 1) + 2,
                                    (ScreenWidthS32() >> 1) + 1, (ScreenHeightS32() >> 1) + 5, 0xffffff);
                    dpadRepeatMs += g_dtMs;
                    stickRepeatMs += g_dtMs;
                    if (pad->cur.typeLen.type == PADTYPE_ANALOG) {
                        Pad_StickToDeadzonedAxes(pad->cur.leftX, pad->cur.leftY, &x, &y);
                        if (!(y | x))
                            stickRepeatMs = 0;
                        if (y) {
                            if (csFlags & CSF_ANALOG_ACCEL) {
                                if (stickRepeatMs > 150 || !stickPitchStep) {
                                    divisor = 30 - (ABS_VALUE(y) * 15) / 256;
                                    y /= divisor;
                                    stickPitchStep = y;
                                } else
                                    y = stickPitchStep;
                            } else
                                y = y >= 0 ? 1 : -1;
                            AddAimOffset(&aimOffset.x, (s16)y, CS_AIM_PITCH);
                            csFlags |= CSF_AIM_CHANGED;
                        } else
                            stickPitchStep = 0;
                        if (x) {
                            if (csFlags & CSF_ANALOG_ACCEL) {
                                if (stickRepeatMs > 150 || !stickYawStep) {
                                    divisor = 30 - (ABS_VALUE(x) * 15) / 256;
                                    x /= divisor;
                                    stickYawStep = x;
                                } else
                                    x = stickYawStep;
                            } else
                                x = x >= 0 ? 1 : -1;
                            AddAimOffset(&aimOffset.y, (s16)-x, CS_AIM_YAW);
                            csFlags |= CSF_AIM_CHANGED;
                        } else
                            stickYawStep = 0;
                        if (stickRepeatMs > 150)
                            stickRepeatMs = 0;
                    }
                    if (!(pad->cur.buttons & ~g_inputMap[INPUT_SLOT_UP]))
                        vertical = INPUT_SLOT_UP;
                    else if (!(pad->cur.buttons & ~g_inputMap[INPUT_SLOT_DOWN]))
                        vertical = INPUT_SLOT_DOWN;
                    if (!(pad->cur.buttons & ~g_inputMap[INPUT_SLOT_RIGHT]))
                        horizontal = INPUT_SLOT_RIGHT;
                    else if (!(pad->cur.buttons & ~g_inputMap[INPUT_SLOT_LEFT]))
                        horizontal = INPUT_SLOT_LEFT;
                    if (!(y | x)) {
                        if (horizontal | vertical) {
                            if (dpadRepeatMs > 150) {
                                dpadStep += 2;
                                if (dpadStep > 25)
                                    dpadStep = 25;
                                dpadRepeatMs = 0;
                            }
                        } else {
                            dpadRepeatMs = 0;
                            dpadStep = 5;
                        }
                    }
                    if (vertical == INPUT_SLOT_UP) {
                        AddAimOffset(&aimOffset.x, -dpadStep, CS_AIM_PITCH);
                        csFlags |= CSF_AIM_CHANGED;
                    } else if (vertical == INPUT_SLOT_DOWN) {
                        AddAimOffset(&aimOffset.x, dpadStep, CS_AIM_PITCH);
                        csFlags |= CSF_AIM_CHANGED;
                    }
                    if (horizontal == INPUT_SLOT_RIGHT) {
                        AddAimOffset(&aimOffset.y, -dpadStep, CS_AIM_YAW);
                        csFlags |= CSF_AIM_CHANGED;
                    } else if (horizontal == INPUT_SLOT_LEFT) {
                        AddAimOffset(&aimOffset.y, dpadStep, CS_AIM_YAW);
                        csFlags |= CSF_AIM_CHANGED;
                    }
                    if ((csFlags & CSF_AIM_CHANGED) || !(csFlags & CSF_AIM_CAM_STARTED)) {
                        aim.x = aimOffset.x + defaultAim.x;
                        aim.y = aimOffset.y + defaultAim.y;
                        aim.z = aimOffset.z + defaultAim.z;
                        StartCamera(aim.x & 0xfff, aim.y & 0xfff, 0, &muzzlePos, 800, 0);
                        ApplyAim(&aim);
                        csFlags &= ~CSF_AIM_CHANGED;
                        csFlags |= CSF_AIM_CAM_STARTED;
                    }
                    if (!(g_padCurButtons & ~g_inputMap[INPUT_SLOT_CROSS]) &&
                        (g_padPrevButtons & ~g_inputMap[INPUT_SLOT_CROSS]) && (csFlags & CSF_FIRE_ARMED)) {
                        aim.x = g_cullPlaneNormal[0];
                        aim.y = g_cullPlaneNormal[1];
                        aim.z = g_cullPlaneNormal[2];
                        SetState(CS_ST_FIRING);
                    } else if (!(g_padCurButtons & ~g_inputMap[INPUT_SLOT_TRIANGLE]) &&
                               (g_padPrevButtons & ~g_inputMap[INPUT_SLOT_TRIANGLE]))
                        SetState(CS_ST_IDLE);
                    else
                        csFlags |= CSF_FIRE_ARMED;
                }
            }
        }
    }
}
void CanonSimple::AddAimOffset(s16 *offset, s16 step, u8 axis)
{
    *offset += step;
    switch (axis) {
        case CS_AIM_PITCH:
            if (*offset > 40)
                *offset = 40;
            else if (ABS_VALUE(*offset) > vertLimit) {
                if (*offset > 0)
                    *offset = vertLimit;
                else
                    *offset = -vertLimit;
            }
            break;
        case CS_AIM_YAW:
            if (ABS_VALUE(*offset) > horzLimit) {
                if (*offset > 0)
                    *offset = horzLimit;
                else
                    *offset = -horzLimit;
            }
            break;
    }
}
void CanonSimple::MoveTo(Vec3s *at)
{
    Vec3s delta;
    delta.x = at->x - lastPos.x;
    delta.y = at->y - lastPos.y;
    delta.z = at->z - lastPos.z;
    activationBox->min[0] += delta.x;
    activationBox->min[1] += delta.y;
    activationBox->min[2] += delta.z;
    activationBox->max[0] += delta.x;
    activationBox->max[1] += delta.y;
    activationBox->max[2] += delta.z;
    muzzlePos = *at;
    lastPos = muzzlePos;
    muzzlePos.y -= 80;
    muzzlePosCopy = muzzlePos;
    SetPosition(at);
    barrel->SetPosition(&muzzlePos);
    if (state != CS_ST_FIRED)
        ball->HandleMessage(this, MSG_CB_PLACE, &muzzlePos);
}
void CanonSimple::Translate(Vec3s *delta)
{
    ScnObject::Translate(delta);
    barrel->Translate(delta);
    ball->Translate(delta);
    activationBox->min[0] += delta->x;
    activationBox->min[1] += delta->y;
    activationBox->min[2] += delta->z;
    activationBox->max[0] += delta->x;
    activationBox->max[1] += delta->y;
    activationBox->max[2] += delta->z;
    lastPos = pos;
    muzzlePos = barrel->pos;
    muzzlePosCopy = muzzlePos;
}
/* BYTES(slot-scope, inferred): the nested block(s) only order the frame: their locals are allocated after the enclosing scope's */
void CanonSimple::ApplyAim(Vec3s *target)
{
    Vec3s barrelAim;
    {
        Vec3s baseAim;
        barrelAim.z = (-target->x) & 0xfff;
        barrelAim.x = 0;
        barrelAim.y = (-target->y + 0x400) & 0xfff;
        baseAim = barrelAim;
        baseAim.z = 0;
        SetRotation(&baseAim);
        barrel->SetRotation(&barrelAim);
    }
}
ScnObject *CanonSimple_Create(void *record)
{
    CanonSimple *object = new CanonSimple;
    object = (CanonSimple *)object->Init(record, 0); /* cast kept: Init returns the object as a ScnBody * */
    return object;
}
