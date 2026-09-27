#ifndef SDW_ENGINE_PROGRESS_H
#define SDW_ENGINE_PROGRESS_H

/* The functions and globals progress.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

class Progress;

extern Progress *g_pProgress; /* 0x57b80c */
extern Progress g_progress;
void Progress_BuildScenePath(char *dest, s8 scene); /* 0x50c1b2 */
void Progress_ResetGlobal();                        /* 0x50b920 */

#endif
