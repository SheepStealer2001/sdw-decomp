#ifndef SDW_OBJECTS_GOLDENCOINS_H
#define SDW_OBJECTS_GOLDENCOINS_H

/* The functions and globals goldencoins.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

class ScnObject;

ScnObject *GoldenCoins_Create(void *record);
u32 GoldenCoins_NearestCoinScoreCB(ScnObject *, ScnObject *, u32, ScnObject *);

#endif
