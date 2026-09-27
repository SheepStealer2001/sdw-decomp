/* PAL PC FallingRock, 0x4bb070-0x4bb545. */
#include "sdw_types.h"
#include "sdw_enums.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *, Animator *, u16, u32);

#define SDW_MEMBERS_ScnObject       \
    static void *operator new(u32); \
    void StartCamera(u16 rx, u16 ry, u16 rz, Vec3s *pos, u16 focal);

#include "sdw_classes.h"
#include "../engine/scn_tools.h"
#include "camera.h"
#include "../app/app_main.h"
#include "../engine/input.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_SETCONTACTENABLED_S32 1
#define SDW_INLINE_SCNOBJECT_SETVISIBLE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETCONTACTENABLED_S32
#undef SDW_INLINE_SCNOBJECT_SETVISIBLE_S32
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
void Camera_StartScripted(ScnObject *, Camera *, u16, u16, u16, Vec3s *, u16, u32, s32);
u16 Sound_Play(u16, void *, u16, u8, s32);
extern Wolf *g_pWolf;
extern s32 g_dtMs;
#define SDW_INLINE_SCNOBJECT_STARTCAMERA_U16_U16_U16_VEC3S_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_STARTCAMERA_U16_U16_U16_VEC3S_U16
#define SDW_INLINE_FREE_BOXCONTAINSXZ_BOX_VEC3S 1
#include "../engine/coll_box_inlines.h"
#undef SDW_INLINE_FREE_BOXCONTAINSXZ_BOX_VEC3S
void FallingRock::PostLoadInit()
{
    state = FALLINGROCK_ST_WAIT;
    u16 *props = record;
    activationBox = Scn_GetPropBox(props, 0);
    if (activationBox == 0)
        state = FALLINGROCK_ST_DONE;
    camRecord = Scn_GetPropCamera(props, 4);
    SetContactEnabled(0);
    SetVisible(0);
    PlayAnim(AROCHEA2_ANIM_STAND2, 0, 0);
    fallTicksLeft = 5;
    camHoldMs = 500;
    impactSoundHandle = 0;
}
void FallingRock::Update()
{
    Vec3s delta;
    switch (state) {
        case FALLINGROCK_ST_WAIT:
            if (BoxContainsXZ(activationBox, &g_pWolf->pos)) {
                SetVisible(1);
                Trigger();
            }
            break;
        case FALLINGROCK_ST_FALL:
            delta.x = 0;
            delta.y = 400;
            delta.z = 0;
            if (fallTicksLeft > 0) {
                Translate(&delta);
                --fallTicksLeft;
            } else {
                PlayAnim(AROCHEA2_ANIM_STAND2, 0, 0);
                impactSoundHandle = Sound_Play(SND_SR2ROFA2, this, 0xff, SNDF_NO_RETRIGGER, 0x1000);
                Camera_StartShake(0x14, 4000);
                state = FALLINGROCK_ST_LANDED;
            }
            break;
        case FALLINGROCK_ST_LANDED:
            SetContactEnabled(1);
            if (camElapsedMs > camHoldMs) {
                Camera_ReleaseAny();
                state = FALLINGROCK_ST_DONE;
            }
            camElapsedMs += g_dtMs;
            break;
    }
    AdvanceAnim();
}
void FallingRock::Unused_Nop() {}
s32 FallingRock::HandleMessage(ScnObject *, u32, void *)
{
    return 0;
}
void FallingRock::Trigger()
{
    Vec3s delta;
    if (camRecord)
        StartCamera(camRecord->rot[0], camRecord->rot[1], camRecord->rot[2], &camRecord->eye, camRecord->focal);
    camElapsedMs = 0;
    SetContactEnabled(1);
    PlayAnim(AROCHEA2_ANIM_FALL1, 0, 0);
    delta.x = 0;
    delta.y = -2000;
    delta.z = 0;
    Translate(&delta);
    g_pad.Rumble_stub(1000, g_rumbleSeqFallingRock, 0x200);
    state = FALLINGROCK_ST_FALL;
}
ScnObject *FallingRock_Create(void *record)
{
    FallingRock *object = new FallingRock;
    object = (FallingRock *)object->Init(record, 0); /* cast kept: Init returns the object it was called on */
    return object;
}
