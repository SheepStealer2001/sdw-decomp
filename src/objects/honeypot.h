#ifndef SDW_OBJECTS_HONEYPOT_H
#define SDW_OBJECTS_HONEYPOT_H

/* The functions and globals honeypot.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

class ScnObject;

extern u32 g_honeyPotSamInHoney; /* 0x6cf668 */
ScnObject *HoneyPot_Create(void *record);

#endif
