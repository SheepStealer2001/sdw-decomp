/* PAL PC SuperButton, 0x4f88a0-0x4f8c61. */
/* BYTES: dead-code. */
#include "sdw_types.h"
#include "sdw_enums.h"
#define SDW_MEMBERS_ScnObject static void *operator new(u32);

#include "sdw_classes.h"
#include "../engine/scn_tools.h"
#define SDW_INLINE_SCNOBJECT_SETVISIBLE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETVISIBLE_S32
void SuperButton::ClearInputs()
{
    u16 a = 0;
    for (a = 0; a < 2; a++) {
        inputs[a].sender = 0;
        inputs[a].value = 0;
    }
}
s32 SuperButton::GetInputValue(ScnObject *sender, s32 *value)
{
    u16 a;
    s32 b = 0;
    a = 0;
    while (a < 2 && b != 1 && inputs[a].sender) {
        if (inputs[a].sender == sender) {
            *value = inputs[a].value;
            b = 1;
        }
        a++;
    }
    return b;
}
s32 SuperButton::SetInputValue(ScnObject *sender, s32 *value)
{
    u16 a;
    s32 b = 0;
    a = 0;
    while (a < 2 && b != 1 && inputs[a].sender) {
        if (inputs[a].sender == sender) {
            inputs[a].value = *value;
            b = 1;
        }
        a++;
    }
    if (!b && a < 2) {
        inputs[a].sender = sender;
        inputs[a].value = *value;
        b = 1;
    }
    return b;
}
/* BYTES(dead-code): a and unused are never read: slots the original frame has */
s32 SuperButton::AreInputsOn()
{
    u16 a = 0;
    s32 unused;
    if (inputs[0].value && inputs[1].value)
        return 1;
    return 0;
}
void SuperButton::PostLoadInit()
{
    void *a = record;
    outTarget = Scn_GetPropObject(a, 4);
    notTarget = Scn_GetPropObject(a, 0);
    SetVisible(0);
    ClearInputs();
    latched = 0;
    state = SUPERBUTTON_ST_OFF;
    SetDisabled(0);
}
void SuperButton::Update()
{
    s32 a;
    switch (disabled) {
        case 0:
            a = AreInputsOn();
            if (a == 2 || a == 0) {
            } else if (a == 1)
                state = a;
            if (state == SUPERBUTTON_ST_ON) {
                latched = 1;
                if (outTarget)
                    outTarget->HandleMessage(this, MSG_SWITCH_ON, 0);
                if (notTarget)
                    notTarget->HandleMessage(this, MSG_SWITCH_OFF, 0);
            } else {
                if (outTarget)
                    outTarget->HandleMessage(this, MSG_SWITCH_OFF, 0);
                if (notTarget)
                    notTarget->HandleMessage(this, MSG_SWITCH_ON, 0);
            }
            break;
    }
}
s32 SuperButton::HandleMessage(ScnObject *sender, u32 message, void *)
{
    s32 a;
    switch (message) {
        case MSG_SWITCH_OFF:
            a = 0;
            SetInputValue(sender, &a);
            return 1;
        case MSG_SWITCH_ON:
            a = 1;
            SetInputValue(sender, &a);
            return 1;
    }
    return 0;
}
void SuperButton::SetDisabled(u8 value)
{
    disabled = value;
}
void SuperButton::Reset()
{
    ClearInputs();
    if (!latched)
        state = SUPERBUTTON_ST_OFF;
    else
        state = SUPERBUTTON_ST_ON;
    SetDisabled(0);
}
ScnObject *SuperButton_Create(void *record)
{
    ScnLogic *object = new SuperButton;
    object = (ScnLogic *)object->Init(record); /* cast kept: Init returns this as a ScnObject * */
    return object;
}
