#ifndef SDW_OBJECTS_PATROL_H
#define SDW_OBJECTS_PATROL_H

/* The functions and globals patrol.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

class TrajPatrol;
struct Trajectory;

void TrajPatrol_Init(TrajPatrol *f, Trajectory *traj, s16 speed, s16 headingBias, u32 continuousHeading,
                     u32 use3dDistance, s16 arriveRadius);                      /* 0x459789 */
s32 TrajPatrol_Step(Vec3s *pos, TrajPatrol *f, Vec3s *outVel, s16 *outHeading); /* 0x4597f5 */

#endif
