#ifndef SDW_ENGINE_POLYBATCHER_H
#define SDW_ENGINE_POLYBATCHER_H

/* The functions and globals polybatcher.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

class RenderPoly;

int PolyBatcher_CompareSortZ(RenderPoly **a, RenderPoly **b);

#endif
