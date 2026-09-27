#ifndef SDW_OBJECTS_SAM_API_H
#define SDW_OBJECTS_SAM_API_H

/* The functions and globals sam.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

class CollBox;
class ScnObject;

extern unsigned short g_samFadeAmount;
extern int g_samFadeEnabled;
extern CollBox **g_samWolfCanBeHitZoneBoxes;
extern unsigned short g_samWolfCanBeHitZoneCount;
ScnObject *Sam_Create(void *record);

#endif
