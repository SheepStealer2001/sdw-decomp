#ifndef SDW_ENGINE_APPROACH_H
#define SDW_ENGINE_APPROACH_H

/* The functions and globals approach.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

s16 Math_ApproachAngle(s16 cur, s16 target, s32 *rate, s32 limit, s32 accel, s32 decel,
                       s32 force_min_step);                                    /* 0x546680 */
s32 Math_ApproachLinear(s32 cur, s32 target, s32 limit, s32 accel, s32 decel); /* 0x546833 */
s32 Math_ApproachValue(s32, s32, s32 *, s32, s32, s32);
s16 Math_StepAngleTowards(s16 current, s16 target, s32 rate); /* 0x546b34 */
s32 Math_StepTowards(s32, s32, s32);
void Vec3s_ApproachPoint(Vec3s *cur, const Vec3s *target, s32 *speed, s32 maxSpeed, s32 accel, s32 decel);

#endif
