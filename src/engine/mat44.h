#ifndef SDW_ENGINE_MAT44_H
#define SDW_ENGINE_MAT44_H

/* The functions and globals mat44.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

class Mat44;

Mat44 *Mat44_Mul(Mat44 *out, const Mat44 *a,
                 const Mat44 *b); /* 0x4093bd: explicit spelling of the hidden result argument. */

#endif
