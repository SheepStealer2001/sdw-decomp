#ifndef SDW_ENGINE_LERP_H
#define SDW_ENGINE_LERP_H

/* The functions and globals lerp.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

struct Vec2s;

extern "C" void Lerp_SetVecTarget(const Vec3s *p); /* 0x5276bd */
extern "C" void Vec2s_LerpToTarget(Vec2s *out, const Vec2s *a, s16 t);
extern "C" void Vec3s_LerpToTarget(Vec3s *out, const Vec3s *a, s16 t); /* 0x5276eb */

#endif
