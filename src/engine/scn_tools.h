#ifndef SDW_ENGINE_SCN_TOOLS_H
#define SDW_ENGINE_SCN_TOOLS_H

/* The functions and globals scn_tools.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

struct CamSetup;
class CollBox;
struct GroundQuery;
struct Mat34s;
class ScnObject;
struct Trajectory;
class box;

extern void *g_voiceOwner;                                                      /* 0x6d411c */
Box *BoxList_FindContainingPoint(Vec3s *p, Box **list, u16 count);              /* 0x514823 */
Box *BoxList_FindContainingPointBelowTop(Vec3s *point, Box **boxes, u16 count); /* 0x5148ff */
Box *BoxList_FindContainingPointXZ(Vec3s *point, Box **boxes, u16 count);       /* 0x514aa5 */
Box *BoxList_FindOverlappingBox(CollBox *query, Box **boxes, u16 count);        /* 0x5149cc */
Box *BoxList_FindOverlappingBoxXZ(Box *query, Box **boxes, u16 count);
s32 Box_GapXZ(CollBox *a, CollBox *b); /* 0x514bfa max of the X and Z edge gaps (0 if overlapping) */
s32 Box_GroundQueryDome(GroundQuery *query, CollBox *box, Vec3s *position); /* 0x515a00 */
u16 Dav_FindResourceIndex(void *resource);                                  /* 0x51474f */
void Matrix_RollByDisplacement(Mat34s *matrix, Vec3s *delta, s32 radius);   /* 0x515b81 */
void Scenaric_DrawClassIcon(u32 *layer, u16 classId, int x, int y, u32 unused, int useIconB,
                            u32 colorRGB);      /* 0x514ceb */
ScnObject *Scenaric_FindByIdList(u16 listId);   /* 0x514717 */
ScnObject *Scenaric_FindByRecord(void *record); /* 0x514656 */
void Scenaric_GetClassIcons(u16 classId, void **iconAOut, void **iconBOut);
Box *Scn_GetPropBox(void *props, s32 propOffset);         /* 0x5155b5 */
CamSetup *Scn_GetPropCamera(void *props, s32 propOffset); /* 0x5156ac */
void **Scn_GetPropIdList(void *props, u32 propOffset, u16 *countOut);
ScnObject *Scn_GetPropObject(void *props, s32 propOffset); /* 0x515630 */
void Scn_GetPropString8(void *props, s32 propOffset, char *out9);
Trajectory *Scn_GetPropTrajectory(void *props, s32 propOffset); /* 0x515668 */
s32 Vec3s_DistSq(Vec3s *a, Vec3s *b);                           /* 0x5158cc */
s32 Vec3s_ManhattanDistXZ(Vec3s *a, Vec3s *b);                  /* 0x5156f0 */
void Vec3s_ScaleByDt(const Vec3s *vel, Vec3s *out);             /* 0x5147bd */
s32 Voice_PlayStream(u32 voiceId, void *owner);                 /* 0x515e50 */
void Voice_StopStream(void *owner);                             /* 0x515eb5 */

#endif
