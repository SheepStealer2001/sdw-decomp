#ifndef SDW_OBJECTS_ROCKET_H
#define SDW_OBJECTS_ROCKET_H

/* The functions and globals rocket.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

class Rocket;
class ScnObject;

extern Rocket *g_pRocket;
ScnObject *Rocket_Create(void *record);

#endif
