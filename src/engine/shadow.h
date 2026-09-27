#ifndef SDW_ENGINE_SHADOW_H
#define SDW_ENGINE_SHADOW_H

/* The functions and globals shadow.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

class ScnObject;
class Shadow;

s16 ObjGrid_QueryGroundY(Vec3s *pos, Vec3s *outNormal, s16 minY, ScnObject *exclude); /* 0x472380 object tops */
void Shadow_FreeLevel();                                                              /* 0x47276a */
void Shadow_Init(Shadow *shadow, u8 radius);                                          /* 0x472745 */
void Shadow_LoadLevel();                                                              /* 0x4727ae */
void Shadow_Render(Shadow *shadow);                                                   /* 0x472904 */
void Shadow_Update(Shadow *shadow, Vec3s *pos, ScnObject *owner);                     /* 0x47255f */

#endif
