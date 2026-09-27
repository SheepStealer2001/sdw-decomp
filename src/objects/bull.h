#ifndef SDW_OBJECTS_BULL_H
#define SDW_OBJECTS_BULL_H

/* The functions and globals bull.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

class ScnObject;

ScnObject *bull_Create(void *record);
u32 bull_TargetScoreCB(ScnObject *, ScnObject *, u32, ScnObject *);

#endif
