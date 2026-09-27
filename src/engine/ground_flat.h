#ifndef SDW_GROUND_FLAT_H
#define SDW_GROUND_FLAT_H
#include "sdw_classes.h"
/* PAL PC 0x515934. Existing compiled callers use this C++ signature. */
s32 Box_GroundQueryFlatTop(GroundQuery *query, CollBox *box, Vec3s *boxPos, s32 margin);
#endif
