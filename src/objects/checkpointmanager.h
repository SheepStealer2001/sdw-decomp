#ifndef SDW_OBJECTS_CHECKPOINTMANAGER_H
#define SDW_OBJECTS_CHECKPOINTMANAGER_H

/* The functions and globals checkpointmanager.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

class ScnObject;

ScnObject *CheckpointManager_Create(u16 *record);

#endif
