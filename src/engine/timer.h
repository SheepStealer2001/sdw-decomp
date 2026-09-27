/*
 * Timer's methods (SheepD3D.exe 0x40b060-0x40b5fd). The class itself - vtable, running +0x08, baseTsc +0x10, elapsedTsc
 * +0x18, deltaTsc +0x20, sizeof 0x28 - is generated into src/include/sdw_classes.h from data/structs/Timer.csv; this header
 * only declares its members. Include it BEFORE sdw_classes.h (the member lists must be defined when the classes are).
 * Two instances exist: g_pTimer (0x71b2e4, heap, made by Time_Init) drives the gameplay clock, g_limiterTimer (0x585068,
 * static) the frame limiter.
 */
#ifndef SDW_TIMER_H
#define SDW_TIMER_H

#include "sdw_enums.h" /* enum TimerUnit (data/enums/TimerUnit.csv), the unit the Get / Peek / Convert methods take */

#define SDW_MEMBERS_Timer Timer(); /* 0x40b060 Timer_Construct: calibrates g_cpuHz once */

#include "timer_api.h"

#endif
