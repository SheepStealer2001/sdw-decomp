/* PAL PC Bell, 0x499030-0x4992f9. */
#include "sdw_types.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *, Animator *, u16, u32);
u16 Sound_Play(u16, void *, u16, u8, s32);
#include "../engine/sound_mgr.h"

#define SDW_MEMBERS_ScnObject static void *operator new(u32);


#include "sdw_enums.h"
#include "scenaric_props.h"
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_ISSOUNDPLAYING_U16 1
#define SDW_INLINE_SCNOBJECT_STOPSOUND_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_ISSOUNDPLAYING_U16
#undef SDW_INLINE_SCNOBJECT_STOPSOUND_U16
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
extern s32 g_dtMs;
void Bell::PostLoadInit()
{
    voice = 0;
    SetState(BELL_ST_IDLE);
}
void Bell::Reset()
{
    SetState(BELL_ST_IDLE);
}
void Bell::Update()
{
    switch (state) {
        case BELL_ST_RINGING:
            if (!voice || !IsSoundPlaying(voice))
                voice = Sound_Play(SND_BELL_RING, this, 255, SNDF_LOOP | SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 4096);
            ringTimer -= g_dtMs;
            if (ringTimer < 0 && AnimFlags(ANIM_F_FINISHED))
                SetState(BELL_ST_RING_END);
            break;
        case BELL_ST_RING_END:
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(BELL_ST_IDLE);
            break;
    }
    AdvanceAnim();
}
s32 Bell::HandleMessage(ScnObject *, u32 msgId, void *)
{
    if (msgId == MSG_TRAP_STATE)
        SetState(BELL_ST_RINGING);
    return 0;
}
void Bell::SetState(u8 value)
{
    state = value;
    if (voice) {
        StopSound(voice);
        voice = 0;
    }
    switch (value) {
        case BELL_ST_IDLE:
            PlayAnim(ACLOCH01_ANIM_STAND, 0, 0);
            break;
        case BELL_ST_RINGING:
            PlayAnim(ACLOCH01_ANIM_RING, 1, 0);
            ringTimer = 0x1000;
            break;
        case BELL_ST_RING_END:
            PlayAnim(ACLOCH01_ANIM_RING2, 0, 0);
            break;
    }
}
ScnObject *Bell_Create(void *record)
{
    Bell *object = new Bell;
    object = (Bell *)object->Init(record, 0); /* cast kept: Init returns the ScnObject base of this bell */
    return object;
}
