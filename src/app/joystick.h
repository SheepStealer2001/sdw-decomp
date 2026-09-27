#ifndef SDW_APP_JOYSTICK_H
#define SDW_APP_JOYSTICK_H

/* The functions and globals joystick.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

struct IDirectInputDevice8A;

extern IDirectInputDevice8A *g_pEnumJoystickDevice; /* 0x5c7a9c the device DI_JoystickAxisEnumCallback configures */

#endif
