#ifndef SDW_ENGINE_COLLIDE_H
#define SDW_ENGINE_COLLIDE_H

/* The functions and globals collide.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

class CollBox;
struct CollContact;
struct CollRay;
struct CollRayHit;
class ScnObject;
class box;

void Coll_BuildCellGrid();
void Coll_FreeCellGrid();
void Collide_InitLevel(); /* 0x5196c0 */
s32 Collide_RayCastStatic(CollRay *, CollRayHit *);
s32 Collide_SegmentClearStatic(s32 *, s32 *, void *);
int Collide_SegmentVsBoxListXZ(Vec3s *origin, Vec3s *delta, CollBox **boxes, int count);
void Collide_ShutdownLevel(); /* 0x5196f2 */
unsigned int Collide_SweepBox_Sam(ScnObject *owner, CollBox *box, Vec3s *delta, int *fraction, int *height,
                                  int *minHeight, CollContact *contacts, ScnObject *excludeSelf, unsigned char flags,
                                  int *auxHeight);

/* The functions and globals collide.cpp defines, declared once for every file that uses them. */

class CollBox;
struct CollContact;
class ScnObject;

extern "C" s32 Collide_BoxVsObjBox(ScnObject *owner, CollBox *moverBox, Vec3s *delta, CollBox *otherBox,
                                   Vec3s *otherPos, s32 *fraction, s32 *height, CollContact *contacts,
                                   s32 *count); /* 0x519bea */

#endif
