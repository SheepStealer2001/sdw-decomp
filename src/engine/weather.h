#ifndef SDW_ENGINE_WEATHER_H
#define SDW_ENGINE_WEATHER_H

/* The functions and globals weather.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

class Camera;

void Weather_InitRain(Vec3s *camPos);              /* 0x52dfc4 */
void Weather_InitSnow(Vec3s *camPos);              /* 0x52e0e3 */
void Weather_LevelInit();                          /* 0x52dfbf */
void Weather_RenderRain(void *layer, Camera *cam); /* 0x52e070 */
void Weather_RenderSnow(void *layer, Camera *cam); /* 0x52e1b0 */
void Weather_UpdateRain(Vec3s *camPos);            /* 0x52e06b, empty */
void Weather_UpdateSnow(Vec3s *camPos);            /* 0x52e1ab, empty */

#endif
