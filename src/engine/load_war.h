#ifndef SDW_ENGINE_LOAD_WAR_H
#define SDW_ENGINE_LOAD_WAR_H

/* The functions and globals load_war.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

struct WarFile;

void Load_FreeWAR(WarFile *war);              /* 0x54a1ba */
int Load_WAR(const char *path, WarFile *war); /* 0x549d25 */

#endif
