#ifndef SDW_OBJECTS_SIGNPOST_H
#define SDW_OBJECTS_SIGNPOST_H

/* The functions and globals signpost.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

class ScnObject;

ScnObject *SignPostAnimated_Create(void *record);
ScnObject *SignPostSimple_Create(void *record);
ScnObject *SignPost_Create(void *record);

#endif
