#ifndef SDW_OBJECTS_BOUNDS_H
#define SDW_OBJECTS_BOUNDS_H

/* The functions and globals bounds.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

struct Model;
class WorldObj;

void Bounds_FromAnimModel(Vec3s *center, u16 *radius, Model *model); /* 0x55c304 */
void Bounds_FromRigidModel(Vec3s *center, u16 *radius, void *model); /* 0x55c797 */
void WorldObj_CalcMeshAabb(WorldObj *);

#endif
