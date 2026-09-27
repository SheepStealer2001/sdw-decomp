#ifndef SDW_ENGINE_TEX_TABLE_H
#define SDW_ENGINE_TEX_TABLE_H

/* The functions and globals tex_table.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

class Mesh;
struct Model;

extern u16 g_texCount;                 /* 0x6d7070 */
extern u32 g_texKeys[];                /* 0x6d7878 its keys: each mesh's resource address in the WAR blob */
extern Mesh *g_texObjects[];           /* 0x6d7078 the mesh cache */
void *Texture_FindByResource(Model *); /* 0x52a810 */

#endif
