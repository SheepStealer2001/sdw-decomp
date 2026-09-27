#ifndef SDW_OBJECTS_BOX_H
#define SDW_OBJECTS_BOX_H

/* The functions and globals box.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

class ScnObject;
struct ScnRecordSynth;

void box_BuildRecord(ScnRecordSynth *out, const Vec3s *pos, u32 content); /* 0x49d27d */
ScnObject *box_Create(void *record);                                      /* 0x49d1f0 */

#endif
