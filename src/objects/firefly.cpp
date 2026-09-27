/* PAL PC Firefly, 0x4bd400-0x4bd757. */
#include "sdw_types.h"
#include "sdw_enums.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *, Animator *, u16, u32);

#define SDW_MEMBERS_ScnObject static void *operator new(u32);

#include "sdw_classes.h"
#include "../engine/maths.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
extern s32 g_dtMs;
void Firefly::PostLoadInit()
{
    pauseMs = Rand_Range(1000, 3000);
    playingAnim2 = 0;
    playingAnim1 = 0;
    playingAnim0 = 0;
    restartPending = 0;
    anim1Repeat = 0;
}
void Firefly::Update()
{
    if (pauseMs <= 0) {
        if (!restartPending && !playingAnim0 && !playingAnim1 && !playingAnim2) {
            PlayAnim(ALUCIO01_ANIM_MOVE, 0, 1);
            playingAnim0 = 1;
        } else if (restartPending && AnimFlags(ANIM_F_FINISHED)) {
            PlayAnim(ALUCIO01_ANIM_MOVE, 0, 1);
            restartPending = 0;
            playingAnim0 = 1;
        } else if (playingAnim0 && AnimFlags(ANIM_F_FINISHED)) {
            PlayAnim(ALUCIO01_ANIM_MOVE1, 0, 1);
            if (anim1Repeat == 2) {
                playingAnim0 = 0;
                anim1Repeat = 0;
            }
            ++anim1Repeat;
            playingAnim1 = 1;
        } else if (playingAnim1 && AnimFlags(ANIM_F_FINISHED)) {
            PlayAnim(ALUCIO01_ANIM_MOVE2, 0, 1);
            playingAnim1 = 0;
            playingAnim2 = 1;
        } else if (playingAnim2 && AnimFlags(ANIM_F_FINISHED)) {
            pauseMs = Rand_Range(1000, 3000);
            restartPending = 1;
        }
        AdvanceAnim();
    } else
        pauseMs -= g_dtMs;
}
s32 Firefly::HandleMessage(ScnObject *, u32 msg, void *)
{
    switch (msg) {
    }
    return 0;
}
ScnObject *Firefly_Create(void *record)
{
    ScnBody *object = new Firefly;
    object = object->Init(record, 0);
    return object;
}
