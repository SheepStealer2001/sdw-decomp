#ifndef SDW_OBJECTS_VIDEO_H
#define SDW_OBJECTS_VIDEO_H

/* The functions and globals video.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

struct IDirectDrawSurface;
struct RECT;

s32 Video_NullFrameProc(IDirectDrawSurface *, RECT *);

#endif
