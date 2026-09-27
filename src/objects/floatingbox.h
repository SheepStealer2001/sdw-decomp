#ifndef SDW_OBJECTS_FLOATINGBOX_H
#define SDW_OBJECTS_FLOATINGBOX_H

/* The functions and globals floatingbox.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

class ScnObject;
struct ScnRecordSynth;

void FloatingBox_BuildRecord(ScnRecordSynth *out, const Vec3s *pos, u32 content); /* 0x4c1e2a */
ScnObject *FloatingBox_Create(void *record);                                      /* 0x4c1d35 */

#endif
