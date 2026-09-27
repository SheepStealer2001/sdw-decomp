#ifndef SDW_OBJECTS_DANCINGGHOST_H
#define SDW_OBJECTS_DANCINGGHOST_H

/* The functions and globals dancingghost.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

class ScnObject;

ScnObject *DancingGhost_Create(void *record);

/* The functions and globals dancingghost.cpp defines, declared once for every file that uses them. */

extern Vec3s g_dgDefaultPos;
extern const char *g_dgFailedLines[5];
extern u32 g_dgFailedVoices[4];

#endif
