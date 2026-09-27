#ifndef SDW_OBJECTS_MINE_H
#define SDW_OBJECTS_MINE_H

/* The functions and globals mine.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

class ScnObject;

ScnObject *DefusableMine_Create(void *record);
ScnObject *GroundMine_Create(void *record);
u32 Mine_TriggerScoreCB(ScnObject *self, ScnObject *candidate, u32 distance, ScnObject *selected);

#endif
