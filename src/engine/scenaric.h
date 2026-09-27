#ifndef SDW_ENGINE_SCENARIC_H
#define SDW_ENGINE_SCENARIC_H

/* The functions and globals scenaric.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

struct Animator;
class Heap;
struct ScnClassRegEntry;
class ScnObject;
class ZoneList;

extern u32 g_levelExitFlags;                                /* 0x57b838 */
extern void *g_levelHeapStorage;                            /* 0x6d0c90 */
extern ScnClassRegEntry g_scenaricClassRegistry[];          /* 0x6cffd8 */
extern Heap g_scenaricLevelHeap;                            /* 0x6cff98 */
extern ZoneList g_waterZones[];                             /* 0x6d0c58 the seven global zone lists, water first */
void Anim_FireSoundEvent(Animator *anim, ScnObject *owner); /* 0x50d540 */
ScnObject *Install_CinematicResource(u32 resIdx);           /* 0x50e069 */
ScnObject *Install_ScenaricResource(u32 resIdx);            /* 0x50df9d */
void Level_ExitUpdate();                                    /* 0x510675 */
s32 ObjGrid_QueryBoxesInRectXZ(s32 minX, s32 minZ, s32 maxX, s32 maxZ, ScnObject **out);
s32 ObjGrid_QueryPointsInRectXZ(s32 minX, s32 minZ, s32 maxX, s32 maxZ, ScnObject **out);         /* 0x510987, max 64 */
void Render_CalcPivotOffset(Vec3s *rot, Vec3s *pivot, Vec3s *outOffset, Vec3s *scaleOrNull);      /* 0x511af8 */
void Scenaric_AllocLevelHeap();                                                                   /* 0x50d71c */
ScnObject *Scenaric_CreateObject(u16 classId, void *record);                                      /* 0x50d9af */
void Scenaric_InitLevelState();                                                                   /* 0x50d738 */
ScnObject *ScnGenericBody_Create(void *record);                                                   /* 0x50d94b */
ScnObject *ScnGenericLogic_Create(void *record);                                                  /* 0x50d82f */
void *Scn_BuildRecordFromExport(u16 gameResId, u16 *outRecord, u16 rotOrFlags, const Vec3s *pos); /* 0x5100ae */
void Zone_GetFlowHeading(const Box *zone, s16 *heading, s32 *speed);
void Zone_GetFlowVelocity(const Box *zone, Vec3s *outVel); /* 0x5112d6 water current from the zone's flag bits */
void Zones_ResolveGlobalLists();                           /* 0x50d792 */

#endif
