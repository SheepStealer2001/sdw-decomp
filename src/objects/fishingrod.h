#ifndef SDW_OBJECTS_FISHINGROD_H
#define SDW_OBJECTS_FISHINGROD_H

/* The functions and globals fishingrod.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

class ScnObject;

ScnObject *FishingRod_Create(void *record);
ScnObject *MagnetRod_Create(void *record);
ScnObject *SaladRod_Create(void *record);

#endif
