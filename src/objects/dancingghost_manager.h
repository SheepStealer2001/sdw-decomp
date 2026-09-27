#ifndef SDW_OBJECTS_DANCINGGHOST_MANAGER_H
#define SDW_OBJECTS_DANCINGGHOST_MANAGER_H

/* The functions and globals dancingghost_manager.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

class ScnObject;

extern Vec3s g_dgStepDefaultPos;
ScnObject *DancingGhostManager_Create(void *record);

#endif
