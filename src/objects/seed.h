#ifndef SDW_OBJECTS_SEED_H
#define SDW_OBJECTS_SEED_H

/* The functions and globals seed.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

class ScnObject;

ScnObject *Seed_Create(void *record);
ScnObject *Tree_Create(void *record);

#endif
