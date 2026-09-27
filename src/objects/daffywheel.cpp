/* T075 - original object DaffyWheel.cpp (guessed name): .text 0x440f30-0x441303, .rdata 0x574e10-0x574e34 (vtable),
 * .data 0x57a800-0x57a80c (the cinematic header's stride-table copy; the map calls this slot a guess, the bytes are the
 * same whichever includer holds it). */
/* BYTES: layout, slot-group. */
/* BYTES(layout): the Cine.h header static: every object including the cinematic header carries this copy in its .data, referenced or not */
/* PAL PC DaffyWheel, 0x440f30..0x441303. */
#include "sdw_types.h"
#include "sdw_enums.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *, Animator *, u16, u32);

#define SDW_MEMBERS_ScnObject static void *operator new(u32);


#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_RECORD 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_RECORD
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
/* 0x57a800 - the cinematic header's static copy of the 9-byte opcode stride table (src/engine/cine.cpp,
 * g_cineOpStride 0x5816fc). The original header defined it static, so every object including it carries its own
 * unreferenced copy at the head of its .data; defined here in its place. */
static u8 s_cineOpStride[9] = {0, 8, 8, 4, 2, 2, 4, 2, 2};
#include "../app/app_main.h"
#include "camera.h"
#include "../engine/scn_tools.h"
void Camera_StartScripted(ScnObject *, Camera *, u16, u16, u16, Vec3s *, u16, u32, s32);
inline s32 IsScriptCamera()
{
    return g_camMode == CAM_SCRIPT_BLEND_IN || g_camMode == CAM_SCRIPT_TO_SCRIPT || g_camMode == CAM_SCRIPT_RETURN;
}

void DaffyWheel::PostLoadInit()
{
    camShotBegin = Scn_GetPropCamera(Record(), 0);
    camShot = Scn_GetPropCamera(Record(), 4);
    Reset();
}

/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets */
void DaffyWheel::StartCamera(CamSetup *shot, u32 mode, s32 speed)
{
    struct Work {
        u16 focal, roll, yaw, pitch;
    } w;
    if (!shot)
        return;
    w.focal = shot->focal;
    w.roll = shot->rot[2];
    w.yaw = shot->rot[1];
    w.pitch = shot->rot[0];
    Camera_StartScripted(this, &g_camera, w.pitch, w.yaw, w.roll, &shot->eye, w.focal, mode, speed);
}

void DaffyWheel::Update()
{
    switch (state) {
        case DAFFYWHEEL_ST_START:
            StartCamera(camShotBegin, 2, 4096);
            StartCamera(camShot, 4, 0x4800);
            state = DAFFYWHEEL_ST_PUSH_B;
            PlayAnim(ADAPRE01_ANIM_PUSH, 0, 1);
            break;
        case DAFFYWHEEL_ST_PUSH_A:
            if (AnimFlags(ANIM_F_FINISHED) && !IsScriptCamera()) {
                state = DAFFYWHEEL_ST_PUSH_B;
                PlayAnim(ADAPRE01_ANIM_PUSH2, 0, 1);
            }
            break;
        case DAFFYWHEEL_ST_PUSH_B:
            if (AnimFlags(ANIM_F_FINISHED) && !IsScriptCamera()) {
                state = DAFFYWHEEL_ST_PUSH_A;
                PlayAnim(ADAPRE01_ANIM_PUSH1, 0, 1);
            }
    }
    AdvanceAnim();
}

s32 DaffyWheel::HandleMessage(ScnObject *sender, u32 msg, void *arg)
{
    switch (msg) {
        case MSG_DAFFYWHEEL_TURN:
            state = DAFFYWHEEL_ST_PUSH_A;
            return 1;
        case MSG_DAFFYWHEEL_SELECT:
            state = DAFFYWHEEL_ST_PUSH_B;
            return 1;
        case MSG_DAFFYWHEEL_RESET:
            state = DAFFYWHEEL_ST_PUSH_B;
            unk7e = 0;
            StartCamera(camShot, 4, 4096);
            return 1;
    }
    return 0;
}
void DaffyWheel::Reset()
{
    unk7e = 0;
    state = DAFFYWHEEL_ST_START;
}
ScnObject *DaffyWheel_Create(void *record)
{
    ScnBody *object = new DaffyWheel;
    object = object->Init(record, 0);
    return object;
}
