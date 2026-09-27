#ifndef SDW_PROPERTY_MATH_H
#define SDW_PROPERTY_MATH_H

#include "../include/sdw_classes.h"

#include "scn_tools.h"

s32 Vec3s_ManhattanDist(Vec3s *a, Vec3s *b);
s32 Vec3s_DistXZ(Vec3s *a, Vec3s *b);
s32 Vec3s_Dist(Vec3s *a, Vec3s *b);
s32 Vec3s_DistSqXZ(Vec3s *a, Vec3s *b);

#endif
