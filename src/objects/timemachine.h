#ifndef SDW_OBJECTS_TIMEMACHINE_H
#define SDW_OBJECTS_TIMEMACHINE_H

/* The functions and globals timemachine.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

class ScnObject;

extern Vec3s g_timeMachineOffsetToPresent;
ScnObject *TimeMachineSphere_Create(void *record);
s32 TimeMachine_IsInPresent(ScnObject *); /* 0x4fba8e */

#endif
