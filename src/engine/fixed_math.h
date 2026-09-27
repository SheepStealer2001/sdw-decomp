#ifndef SDW_ENGINE_FIXED_MATH_H
#define SDW_ENGINE_FIXED_MATH_H

/* The functions and globals fixed_math.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

struct Mat34s;
struct Vec3i;
struct Vec4i;
struct Vec4s;

extern "C" u32 Color_RgbToBgr(u32 rgb); /* 0x5269f2 */
extern "C" u32 Color_RgbToBgrHalved(u32 c);
extern "C" void Mat34s_ApplyScale(Mat34s *m, const Vec3s *scale); /* 0x527012 */
extern "C" void Mat34s_FromEulerXZY(const Vec3s *, Mat34s *);
extern "C" void Mat34s_FromEulerYXZ(const Vec3s *angles, Mat34s *out);                          /* 0x526dc6 */
extern "C" void Mat34s_Identity(Mat34s *m);                                                     /* 0x526d53 */
extern "C" Vec4i *Mat34s_TransformTransposedVec3i(const Mat34s *m, const Vec3i *v, Vec4i *out); /* 0x5275c1 */
extern "C" Vec4i *Mat34s_TransformVec3i(const Mat34s *, const Vec3i *, Vec4i *);                /* 0x5274de */
extern "C" Vec4s *Mat34s_TransformVec3s(const Mat34s *m, const Vec3s *in, Vec4s *out);          /* 0x5273e0 */
extern "C" Vec4i *Mat34s_TransformVec3s_i(const Mat34s *, const Vec3s *, Vec4i *);
extern "C" float Math_Angle4096ToRadians_2(s16 angle); /* 0x5269a9 */
extern "C" float Math_Fixed10ToFloat_s16(s16);
extern "C" float Math_Fixed12ToFloat_s16(s16 v); /* 0x52675b */
extern "C" float Math_Fixed12ToFloat_s32(s32 v); /* 0x5267a2, src/engine/fixed_math.cpp */
extern "C" s16 Math_FloatToFixed12_s16(float f); /* 0x526600 */
extern "C" s32 Vec3s_LengthSq(const Vec3s *);
extern "C" void Vec3i_Normalize(const Vec3i *v, Vec3i *out); /* 0x526a71 */
extern "C" s32 Vec3s_Dot(const Vec3s *a, const Vec3s *b);
extern "C" void Vec3s_Normalize(const Vec3s *v, Vec3s *out); /* 0x526b03 (engine/fixed_math.cpp) */

#endif
