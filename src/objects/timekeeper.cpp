/* PAL PC TimeKeeper, 0x4f9b90-0x4f9fd6. */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *inst, Animator *animator, u16 id, u32 options);
#include "../engine/sound_mgr.h"
#include "../engine/progress.h"

#define SDW_MEMBERS_ScnObject static void *operator new(u32 size);


#define SDW_MEMBERS_Progress                                   \
    s32 IsMenuScene()                                          \
    {                                                          \
        s8 level = currentLevel;                               \
        return level == SCENE_DEMO_A || level == SCENE_DEMO_B; \
    }                                                          \
    void SetTimeKeeperUnsaved(s32 value);
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_STOPSOUND_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_STOPSOUND_U16
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_PROGRESS_GETLEVEL 1
#include "../engine/progress_inlines.h"
#undef SDW_INLINE_PROGRESS_GETLEVEL
u16 Sound_Play(u16 id, void *owner, u16 volume, u8 flags, s32 rate);
inline void Progress::SetTimeKeeperUnsaved(s32 value)
{
    runtimeBits.timeKeeperUnsaved = value;
}

void TimeKeeper::PostLoadInit()
{
    SnapToGround(1);
    if (g_pProgress->IsMenuScene() || g_pProgress->IsTimeKeeperDone(g_pProgress->GetLevel()))
        SetState(TK_COLLECTED);
    else
        SetState(TK_AVAILABLE);
    ringSoundHandle = 0;
}
void TimeKeeper::Update()
{
    if (state == TK_RINGING && AnimFlags(ANIM_F_FINISHED))
        SetState(TK_RINGING);
    AdvanceAnim();
}
s32 TimeKeeper::HandleMessage(ScnObject *sender, u32 message, void *arg)
{
    switch (message) {
        case MSG_USE:
            if (state == TK_AVAILABLE) {
                if (!g_pProgress->IsMenuScene()) {
                    g_pProgress->AwardTimeKeeper();
                    g_pProgress->SetTimeKeeperUnsaved(1);
                }
                SetState(TK_RINGING);
            }
            break;
        case MSG_QUERY_ACTION:
            if (state == TK_AVAILABLE &&
                (sender->GetClassId() == CLASSID_WOLF || sender->GetClassId() == CLASSID_ROBOT))
                return CTX_TIMEKEEPER;
            break;
    }
    return 0;
}
void TimeKeeper::Reset() {}
void TimeKeeper::SetState(u8 value)
{
    state = value;
    switch (value) {
        case TK_AVAILABLE:
            PlayAnim(APTEUS01_ANIM_CLOCK1, 1, 0);
            seqStep = 0;
            break;
        case TK_RINGING:
            switch (seqStep) {
                case 0:
                    PlayAnim(APTEUS01_ANIM_CLOCK2, 0, 0);
                    break;
                case 1:
                    PlayAnim(APTEUS01_ANIM_CLOCK3, 0, 0);
                    ringSoundHandle = Sound_Play(SND_SPTAIGU, this, 255, SNDF_LOOP, 4096);
                    break;
                case 2:
                    StopSound(ringSoundHandle);
                    PlayAnim(APTEUS01_ANIM_CLOCK4, 0, 0);
                    break;
                case 3:
                    SetState(TK_COLLECTED);
                    break;
            }
            ++seqStep;
            break;
        case TK_COLLECTED:
            PlayAnim(APTEUS01_ANIM_CLOCK1, 1, 0);
            break;
    }
}
ScnObject *TimeKeeper_Create(u16 *record)
{
    ScnBody *object = new TimeKeeper;
    object = object->Init(record, 0);
    return object;
}
