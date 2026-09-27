/* PAL PC SecretDoor. Uses the generated class declaration. */
/* BYTES: dead-code. */
#define SDW_MEMBERS_ScnObject static void *operator new(u32 size);
#include "sdw_classes.h"

/* 0x4e9fe0 */
/* BYTES(dead-code): props is loaded and never used, as in the original */
void SecretDoor::PostLoadInit()
{
    u16 *props = record;
}

/* 0x4e9ff6 */
void SecretDoor::Update() {}

/* 0x4ea001 */
s32 SecretDoor::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    return 0;
}

/* 0x4ea010 */
ScnObject *SecretDoor_Create(void *record)
{
    ScnLogic *obj = new SecretDoor;
    obj = (ScnLogic *)obj->Init(record); /* cast kept: Init returns the ScnObject * base of this object */
    return obj;
}
