#ifndef SDW_OBJECTS_CREDITSMANAGER_H
#define SDW_OBJECTS_CREDITSMANAGER_H

/* The functions and globals creditsmanager.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

class ScnObject;

ScnObject *CreditsManager_Create(void *record);
void CreditsManager_CreateProjectionVBs();

#endif
