#ifndef SDW_ENGINE_MATHS_H
#define SDW_ENGINE_MATHS_H

/* The functions and globals maths.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

extern Vec3s *g_pZeroVec3s;                               /* 0x5814ec origin for the world-space static boxes */
u32 Int_ToBcd(s32 value);                                 /* 0x5612c4 */
s32 Rand_Range(s32, s32);                                 /* 0x5612a1 */
void Rand_Reset();                                        /* 0x561200 */
u16 Str_Concat2(char *dst, const char *a, const char *b); /* 0x5613a6 */
u32 Str_Copy(char *dst, const char *src);                 /* 0x561360 */
u16 Str_CopyAsciiN(char *, const char *, u16);
s32 Str_IsPrefixOf(const char *a, const char *b); /* 0x5614b0 */
void Vec3i_SetLength(s32 *v, s32 len);            /* 0x561530 (src/engine/maths.cpp) */

/* The functions and globals maths.cpp defines, declared once for every file that uses them. */

extern const s32 g_zeroConst_5775d8[4];

#endif
