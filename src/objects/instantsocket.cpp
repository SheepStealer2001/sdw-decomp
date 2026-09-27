/* PAL PC InstantSocket, 0x4cf0d0-0x4cf2ad. */
#include "sdw_types.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *instance, Animator *animator, u16 id, u32 options);

#define SDW_MEMBERS_ScnObject static void *operator new(u32 size);

#include "sdw_enums.h"
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
void InstantSocket::PostLoadInit()
{
    PlayAnim(APRISE01_ANIM_SHOW, 1, 1);
}
void InstantSocket::Reset()
{
    PlayAnim(APRISE01_ANIM_SHOW, 1, 1);
}
void InstantSocket::Update()
{
    AdvanceAnim();
}
s32 InstantSocket::HandleMessage(ScnObject *, u32 msgId, void *)
{
    switch (msgId) {
        case MSG_SOCKET_DOCKED:
            PlayAnim(APRISE01_ANIM_STAND, 1, 1);
            break;
        case MSG_SOCKET_UNDOCKED:
            PlayAnim(APRISE01_ANIM_SHOW, 1, 1);
            break;
    }
    return 0;
}
ScnObject *InstantSocket_Create(void *record)
{
    InstantSocket *object = new InstantSocket;
    object = (InstantSocket *)object->Init(record, 0); /* cast kept: Init returns the object as a ScnObject * */
    return object;
}
