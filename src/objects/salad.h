#ifndef SDW_OBJECTS_SALAD_H
#define SDW_OBJECTS_SALAD_H

/* The functions and globals salad.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

class ScnObject;

ScnObject *Salad_Create(void *record);
u32 Salad_ScoreAttractTargetCB(ScnObject *self, ScnObject *candidate, u32 distanceSquared, ScnObject *selectedSoFar);

#endif
