#ifndef SDW_ENGINE_TEX_SCROLL_H
#define SDW_ENGINE_TEX_SCROLL_H

/* The functions and globals tex_scroll.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

struct DavBitmapRec;

void TexScroll_AddRect(const DavBitmapRec *rec, s8 step);
void TexScroll_FreeAll();   /* 0x560edd */
void TexScroll_Init();      /* 0x560e12 */
void TexScroll_UpdateAll(); /* 0x560f41 */

#endif
