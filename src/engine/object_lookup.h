#ifndef SDW_OBJECT_LOOKUP_H
#define SDW_OBJECT_LOOKUP_H

/* The scenaric object lookups: Scenaric_FindByClass .. Dav_FindResourceIndex are defined in src/engine/scn_tools.cpp
 * (T250), the member ScnObject::Scenaric_SendToClass 0x511529 in src/engine/scenaric.cpp (declared in the class body,
 * data/class_methods.csv). */

#include "sdw_classes.h"

s32 Scenaric_FindByClass(u16 classId, ScnObject **out, s32 maximum);
#include "scn_tools.h"

#endif
