/* PAL PC Telescope, 0x4f9030-0x4f9b28. */
/* BYTES: cast. */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
#define SDW_MEMBERS_ScnObject       \
    static void *operator new(u32); \
    void StartCamera(u16, u16, u16, Vec3s *, u16, u32);
#include "sdw_classes.h"
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_SETVISIBLE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_SETVISIBLE_S32
extern Wolf *g_pWolf;
#include "../engine/input.h"
#include "../engine/fixed_math.h"
#include "../app/app_main.h"
#include "../engine/scn_tools.h"
#include "../engine/interface.h"
#include "camera.h"

#define g_padCurButtons (g_pad.cur.buttons)

void Camera_StartScripted(ScnObject *, Camera *, u16, u16, u16, Vec3s *, u16, u32, s32);
#define SDW_INLINE_SCNOBJECT_STARTCAMERA_U16_U16_U16_VEC3S_U16_U32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_STARTCAMERA_U16_U16_U16_VEC3S_U16_U32
#define SDW_INLINE_FREE_PROPU32_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_PROPU32_VOID_U32
#define SDW_INLINE_FREE_BOX_CONTAINSPOINT_BOX_VEC3S 1
#include "../engine/coll_box_inlines.h"
#undef SDW_INLINE_FREE_BOX_CONTAINSPOINT_BOX_VEC3S
void Telescope::PostLoadInit()
{
    void *a = record;
    focalByMode[0] = (u16)PropU32(a, 0x1c);
    camDistByMode[0] = (u16)PropU32(a, 0);
    normalBox = Scn_GetPropBox(a, 0x18);
    focalByMode[1] = (u16)PropU32(a, 0x24);
    camDistByMode[1] = (u16)PropU32(a, 4);
    reverseBox = Scn_GetPropBox(a, 0x20);
    initVertAngle = (s16)PropU32(a, 0xc);
    initHorAngle = (s16)PropU32(a, 8);
    maxVertAngle = (s16)PropU32(a, 0x14);
    maxHorAngle = (s16)PropU32(a, 0x10);
    maxHorAngle *= 11;
    maxVertAngle *= 11;
    initHorAngle *= 11;
    initVertAngle *= 11;
    wolfFrozen = 0;
    state = TELESCOPE_IDLE;
    mode = TELESCOPE_NORMAL;
}
void Telescope::Reset()
{
    SetVisible(1);
    wolfFrozen = 0;
    state = TELESCOPE_IDLE;
    mode = TELESCOPE_NORMAL;
}
/* BYTES(cast): the stored byte is passed without normalising, as the original does */
void Telescope::Update()
{
    switch (state) {
        case TELESCOPE_STARTING:
            /* cast kept (both calls): Hud_DrawTelescopeMask takes a bool, and the original passes the stored 0/1 mode
             * byte as it is, without a != 0 test */
            Hud_DrawTelescopeMask(*(bool *)&mode);
            state = TELESCOPE_VIEWING;
            break;
        case TELESCOPE_VIEWING:
            UpdateView(&g_pad);
            /* cast kept: the mode's low byte read as a bool */
            Hud_DrawTelescopeMask(*(bool *)&mode);
            break;
    }
}
/* BYTES(cast): word loads that zero-extend, as the original */
s32 Telescope::UpdateView(Pad *pad)
{
    Vec3s a;
    s32 b;
    Vec4i c = {0, 0, camDistByMode[mode]};
    Mat34s d;
    s32 e;
    b = 0;
    e = b;
    if (pad->cur.typeLen.type == PADTYPE_ANALOG) {
        Pad_StickToDeadzonedAxes(pad->cur.rightX, pad->cur.rightY, &e, &b);
        if (!(e | b))
            Pad_StickToDeadzonedAxes(pad->cur.leftX, pad->cur.leftY, &e, &b);
    }
    if (!(e | b)) {
        if (!(g_padCurButtons & ~(u16)~PAD_RIGHT))
            e = 256;
        else if (!(g_padCurButtons & ~(u16)~PAD_LEFT))
            e = -256;
        if (!(g_padCurButtons & ~(u16)~PAD_UP))
            b = -256;
        else if (!(g_padCurButtons & ~(u16)~PAD_DOWN))
            b = 256;
    }
    if (pad->cur.typeLen.type == PADTYPE_ANALOG) {
        if (e > 100) {
            e -= 100;
            if (yawOffset > -maxHorAngle)
                yawOffset -= (s16)(e / 10);
        } else if (e < -100) {
            e += 100;
            if (yawOffset < maxHorAngle)
                yawOffset += (s16)(-e / 10);
        }
        if (b < -100) {
            b += 100;
            if (pitchOffset > -maxVertAngle)
                pitchOffset -= (s16)(-b / 10);
        } else if (b > 100) {
            b -= 100;
            if (pitchOffset < maxVertAngle)
                pitchOffset += (s16)(b / 10);
        }
    } else {
        if (!(pad->cur.buttons & ~g_inputMap[INPUT_SLOT_RIGHT])) {
            if (yawOffset > -maxHorAngle)
                yawOffset -= 10;
        } else if (!(pad->cur.buttons & ~g_inputMap[INPUT_SLOT_LEFT])) {
            if (yawOffset < maxHorAngle)
                yawOffset += 10;
        }
        if (!(pad->cur.buttons & ~g_inputMap[INPUT_SLOT_DOWN])) {
            if (pitchOffset < maxVertAngle)
                pitchOffset += 10;
        } else if (!(pad->cur.buttons & ~g_inputMap[INPUT_SLOT_UP])) {
            if (pitchOffset > -maxVertAngle)
                pitchOffset -= 10;
        }
    }
    /* Original word loads zero-extend these two base angles. */
    a.x = (u16)basePitch + pitchOffset;
    a.y = (u16)baseYaw + yawOffset;
    a.z = 0;
    Mat34s_FromEulerYXZ(&a, &d);
    /* cast kept: it reads the first three components and writes all four */
    Mat34s_TransformTransposedVec3i(&d, (Vec3i *)&c, &c);
    eye.x = c.x + pos.x;
    eye.y = c.y - 180 + pos.y;
    eye.z = c.z + pos.z;
    StartCamera((u16)basePitch + pitchOffset, (u16)baseYaw + yawOffset, baseRoll, &eye, focal, 0);
    if (!(pad->cur.buttons & ~g_inputMap[INPUT_SLOT_CROSS]) && (pad->prev.buttons & ~g_inputMap[INPUT_SLOT_CROSS])) {
        Camera_ReleaseScripted(this);
        state = TELESCOPE_IDLE;
        if (wolfFrozen) {
            g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0);
            wolfFrozen = 0;
        }
        SetVisible(1);
        return 1;
    }
    return 0;
}
s32 Telescope::HandleMessage(ScnObject *sender, u32 message, void *)
{
    Vec3s a;
    Vec4i b = {0, 0, camDistByMode[mode]};
    Vec3s c;
    Mat34s d;
    switch (message) {
        case MSG_USE:
            eye = pos;
            c = rot;
            a.x = initVertAngle;
            a.y = 0x800 - c.y + initHorAngle;
            a.z = 0;
            Mat34s_FromEulerYXZ(&a, &d);
            Mat34s_TransformTransposedVec3i(&d, (Vec3i *)&b, &b); /* cast kept: as in UpdateView */
            SetVisible(0);
            yawOffset = initHorAngle;
            pitchOffset = initVertAngle;
            if (!mode)
                baseYaw = 0x800 - c.y;
            else {
                baseYaw = -c.y;
                pitchOffset = -initVertAngle;
            }
            basePitch = 0;
            baseRoll = 0;
            if (focalByMode[mode] < 0)
                focalByMode[mode] = 0x300;
            focal = focalByMode[mode];
            eye.x = b.x + pos.x;
            eye.y = b.y - 180 + pos.y;
            eye.z = b.z + pos.z;
            StartCamera(basePitch, baseYaw, baseRoll, &eye, focal, 0);
            wolfFrozen = g_pWolf->HandleMessage(this, MSG_FREEZE, 0);
            state = TELESCOPE_STARTING;
            break;
        case MSG_QUERY_ACTION:
            if (sender->GetClassId() == CLASSID_WOLF) {
                if (reverseBox && Box_ContainsPoint(reverseBox, &g_pWolf->pos)) {
                    mode = TELESCOPE_REVERSE;
                    return CTX_TELESCOPE;
                }
                if (normalBox && Box_ContainsPoint(normalBox, &g_pWolf->pos)) {
                    mode = TELESCOPE_NORMAL;
                    return CTX_TELESCOPE;
                }
            }
            break;
        case MSG_FREEZE:
            wolfFrozen = 0;
            return 1;
    }
    return 0;
}
ScnObject *Telescope_Create(void *record)
{
    ScnLogic *object = new Telescope;
    object = (ScnLogic *)object->Init(record); /* cast kept: Init returns the object as its base class */
    return object;
}
