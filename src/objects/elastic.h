#ifndef SDW_OBJECTS_ELASTIC_H
#define SDW_OBJECTS_ELASTIC_H

/* The functions and globals elastic.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

class ScnObject;

ScnObject *elastic_Create(void *record);
u32 elastic_LaunchTargetScoreCB(ScnObject *self, ScnObject *candidate, u32 score,
                                ScnObject *selected); /* defined after Update */

#endif
