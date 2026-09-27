#ifndef SDW_OBJECTS_VIDEO_PLAYER_H
#define SDW_OBJECTS_VIDEO_PLAYER_H

/* The functions and globals video_player.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

struct IDirectDrawSurface;
struct RECT;

s32 VideoPlayer_BlitFrame(IDirectDrawSurface *, RECT *);

#endif
