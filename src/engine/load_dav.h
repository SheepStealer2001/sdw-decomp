#ifndef SDW_ENGINE_LOAD_DAV_H
#define SDW_ENGINE_LOAD_DAV_H

/* The functions and globals load_dav.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

struct Dav;
struct DavBitmapRec;

s32 Dav_Free(Dav *dav);                   /* 0x54867e, below */
s32 Load_DAV(const char *path, Dav *dav); /* 0x54843c, below */
u32 TexAtlas_GetPage(DavBitmapRec *rec);  /* 0x548430 */

#endif
