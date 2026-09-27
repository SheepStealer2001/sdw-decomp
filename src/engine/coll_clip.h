#ifndef SDW_ENGINE_COLL_CLIP_H
#define SDW_ENGINE_COLL_CLIP_H

/* The functions and globals coll_clip.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

struct Box6i;
struct Vec3i;
class box;

extern "C" u32 Collide_BoxTriangleSweep(Box6i *box, Vec3s *delta, Vec3i *vertices, u32 vertexCount, s32 *fraction,
                                        s32 *height, Vec3s *normal);

/* The functions and globals coll_clip.cpp defines, declared once for every file that uses them. */

struct Box6i;
class CollBox;
struct CollContact;
class ScnObject;
struct Vec3i;
class box;

extern "C" u32 Collide_BoxTriangleSweep_MinMaxY(Box6i *box, Vec3s *delta, Vec3i *vertices, u32 vertexCount,
                                                s32 *fraction, s32 *maxHeight, s32 *minHeight,
                                                Vec3s *normal); /* 0x51902f */
extern "C" u32 Collide_SweepBoxVsBox(CollBox *mover, Vec3s *delta, CollBox *target, s32 *fraction, s32 *height);
extern "C" s32 Collide_SweepBoxVsCone(ScnObject *owner, CollBox *mover, Vec3s *delta, CollBox *target, s32 *fraction,
                                      s32 *height, CollContact *contacts, s32 *count);
extern "C" s32 Collide_SweepBoxVsCylinder(ScnObject *owner, CollBox *mover, Vec3s *delta, CollBox *target,
                                          s32 *fraction, s32 *height, CollContact *contacts, s32 *count);
extern "C" s32 Collide_SweepBoxVsEllipsoid(ScnObject *owner, CollBox *mover, Vec3s *delta, CollBox *target,
                                           s32 *fraction, s32 *height, CollContact *contacts, s32 *count);

#endif
