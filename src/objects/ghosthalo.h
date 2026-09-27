#ifndef SDW_OBJECTS_GHOSTHALO_H
#define SDW_OBJECTS_GHOSTHALO_H

/* The functions and globals ghosthalo.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

class ScnObject;

extern ScnObject *g_pGhostHalo; /* 0x6cf664, written by GhostHalo_Create 0x4c748c */
ScnObject *GhostHalo_Create(void *record);

#endif
