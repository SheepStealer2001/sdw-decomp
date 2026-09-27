#ifndef SDW_OBJECTS_TIMEKEEPER_H
#define SDW_OBJECTS_TIMEKEEPER_H

/* The functions and globals timekeeper.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

class ScnObject;

ScnObject *TimeKeeper_Create(u16 *record);

#endif
