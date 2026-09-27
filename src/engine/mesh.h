#ifndef SDW_ENGINE_MESH_H
#define SDW_ENGINE_MESH_H

/* The functions and globals mesh.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

s32 Outline_CompareEdgeKey(const u32 *a, const u32 *b);                                /* 0x41210e */
u32 Outline_MakeEdgeKey(u32 vertA, u32 vertB, u32 faceColor, u32 fogSum, u8 eligible); /* 0x41204a */

#endif
