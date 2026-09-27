#ifndef SDW_OBJECTS_CINEMATICSMANAGER_H
#define SDW_OBJECTS_CINEMATICSMANAGER_H

/* The functions and globals cinematicsmanager.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

class ScnObject;

const char *Cine_ResolveText(ScnObject *owner, unsigned int flags, unsigned char text);
ScnObject *CinematicsManager_Create(u16 *record);

#endif
