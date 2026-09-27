/*
 * input_device.h - the declarations shared by the three DirectInput device objects T002 (InputDevice.cpp), T005
 * (Joystick.cpp) and T006 (Keyboard.cpp): the DirectInput 8 declarations, the InputDevice members, the Joystick and
 * Keyboard classes. Only these three files include it.
 */
#ifndef SDW_INPUT_DEVICE_H
#define SDW_INPUT_DEVICE_H
#include "sdw_types.h"

#include "sdw_enums.h" /* before sdk/win32.h, whose IDOK would otherwise replace the enum constant of that name */
#include "../sdk/win32.h"
#include "../sdk/dinput.h"
#include "../sdk/crt.h"

/* ---- the classes ---- */
#define SDW_MEMBERS_InputDevice InputDevice(); /* 0x403f30 InputDevice_Construct */
#define SDW_MEMBERS_Joystick Joystick(u16 buttonCount, D3DApp *app, HRESULT *result); /* 0x406db0 Joystick_Construct */
#define SDW_MEMBERS_Keyboard Keyboard(u16 buttonCount, D3DApp *app, HRESULT *result); /* 0x4072ac Keyboard_Construct */
#include "sdw_classes.h"

#define JOY_CAPS (*(DIDEVCAPS *)caps) /* cast kept: DIDEVCAPS is an SDK type the generated Joystick cannot name */
#define KB_AXISKEY(i) (axisKeys[i])   /* 0 X-, 1 X+, 2 Y-, 3 Y+, 4 Z-, 5 Z+ */

/* ---- globals ---- */
#include "joystick.h"

BOOL __stdcall DI_JoystickAxisEnumCallback(const DIDEVICEOBJECTINSTANCEA *doi, void *ref);

#endif
