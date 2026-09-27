#ifndef SDW_OBJECTS_OBJECTMANAGER_H
#define SDW_OBJECTS_OBJECTMANAGER_H

/* The functions and globals objectmanager.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

class ScnObject;

ScnObject *ObjectManager_Create(void *record);

#endif
