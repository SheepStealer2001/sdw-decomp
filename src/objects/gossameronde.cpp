/* PAL PC GossamerOnde, 0x4c8110-0x4c880a. */
/* BYTES: dead-code. */
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
#define SDW_INLINE_SCNOBJECT_SETVISIBLE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETVISIBLE_S32
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
extern Wolf *g_pWolf;
#include "sheep.h"
extern s32 g_dt;
#include "../sdk/crt.h"
#define ABS_VALUE(x) ((x) >= 0 ? (x) : -(x))
/* BYTES(dead-code): properties is loaded and never used, as in the original */
void GossamerOnde::PostLoadInit()
{
    void *properties = record;
    SetState(GONDE_ST_DONE);
    SetVisible(0);
    sheepPushed = 0;
    wolfPushed = 0;
    sheep = 0;
}
void GossamerOnde::Update()
{
    switch (state) {
        case GONDE_ST_WAVE:
            if (AnimFlags(ANIM_F_FINISHED)) {
                g_pWolf->HandleMessage(this, MSG_RIDER_REMOVE, 0);
                if (sheep)
                    sheep->HandleMessage(this, MSG_RIDER_REMOVE, 0);
                sheepPushed = 0;
                wolfPushed = 0;
                SetState(GONDE_ST_DONE);
            } else
                radius += (u16)(g_dt * 875 >> 12);
            break;
    }
    AdvanceAnim();
}
void GossamerOnde::Reset()
{
    g_pWolf->HandleMessage(this, MSG_RIDER_REMOVE, 0);
    if (sheep)
        sheep->HandleMessage(this, MSG_RIDER_REMOVE, 0);
    SetState(GONDE_ST_DONE);
    SetVisible(0);
    sheepPushed = 0;
    wolfPushed = 0;
    sheep = 0;
}
s32 GossamerOnde::HandleMessage(ScnObject *sender, u32 message, void *arg)
{
    ContactInfo a;
    switch (message) {
        case MSG_MODIFY_MOVE:
            moveArg = (MoveModifyArg *)arg; /* cast kept: MSG_MODIFY_MOVE's arg is a MoveModifyArg */
            if (sender == g_pWolf) {
                if (!wolfPushed) {
                    dist = (s32)sqrt((double)((sender->pos.x - pos.x) * (sender->pos.x - pos.x)) +
                                     (double)((sender->pos.z - pos.z) * (sender->pos.z - pos.z)));
                    if ((u32)dist < (u32)(radius + 50) && (u32)dist > (u32)(radius - 50)) {
                        if (ABS_VALUE(sender->pos.y - pos.y) < (s32)vertTolerance) {
                            wolfPush.x = sender->pos.x - pos.x;
                            wolfPush.y = 0;
                            wolfPush.z = sender->pos.z - pos.z;
                            wolfPush.x = wolfPush.x * 40 / dist;
                            wolfPush.z = wolfPush.z * 40 / dist;
                            wolfPushed = 1;
                            moveArg->delta.x = wolfPush.x;
                            moveArg->delta.z = wolfPush.z;
                            sender->HandleMessage(this, MSG_WOLF_FORCE_FALL, 0);
                            return 1;
                        }
                    }
                } else {
                    moveArg->delta.x = wolfPush.x;
                    moveArg->delta.z = wolfPush.z;
                    sender->HandleMessage(this, MSG_WOLF_FORCE_FALL, 0);
                    return 1;
                }
            } else if (sender == sheep) {
                if (!sheepPushed) {
                    dist = (s32)sqrt((double)((sender->pos.x - pos.x) * (sender->pos.x - pos.x)) +
                                     (double)((sender->pos.z - pos.z) * (sender->pos.z - pos.z)));
                    if ((u32)dist < (u32)(radius + 50) && (u32)dist > (u32)(radius - 50)) {
                        sheepPush.x = sender->pos.x - pos.x;
                        sheepPush.y = 0;
                        sheepPush.z = sender->pos.z - pos.z;
                        sheepPush.x = sheepPush.x * 40 / dist;
                        sheepPush.z = sheepPush.z * 40 / dist;
                        sheep->Collide_ResolveMove(&sheepPush, &a, 0xb54, RESOLVE_SLIDE_ALL, 0, 0, 10, 0, 0);
                        sheepPushed = 1;
                        moveArg->delta.x = sheepPush.x;
                        moveArg->delta.z = sheepPush.z;
                        return 1;
                    }
                } else {
                    moveArg->delta.x = sheepPush.x;
                    moveArg->delta.z = sheepPush.z;
                    return 1;
                }
            }
            return 0;
        case MSG_ONDE_START:
            SetPosition(&sender->pos);
            SetVisible(1);
            SetState(GONDE_ST_WAVE);
            g_pWolf->HandleMessage(this, MSG_RIDER_ADD, 0);
            sheep = g_pSheepOutOfZone;
            if (sheep)
                sheep->HandleMessage(this, MSG_RIDER_ADD, 0);
            radius = 0;
            vertTolerance = 180;
    }
    return 0;
}
void GossamerOnde::SetState(u8 next)
{
    state = next;
    switch (next) {
        case GONDE_ST_WAVE:
            PlayAnim(AONDEG01_ANIM_WAVE, 0, 0);
            break;
    }
}
ScnObject *GossamerOnde_Create(void *record)
{
    ScnBody *object = new GossamerOnde;
    object = object->Init(record, 0);
    return object;
}
