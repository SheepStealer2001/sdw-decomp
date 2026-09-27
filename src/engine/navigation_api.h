#ifndef SDW_ENGINE_NAVIGATION_API_H
#define SDW_ENGINE_NAVIGATION_API_H

/* The functions and globals navigation.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

struct NavNode;
struct NavSearch;
struct SamEdgeNormal;

void NavPath_ClearSearch(void *search);
NavNode *NavPath_ListPopFirstWithState(NavSearch *search, u8 state);
NavNode *NavPath_ListPopHead(NavSearch *search);
void NavPath_ListRemove(NavSearch *search, NavNode *node);
void NavPath_OpenListInsert(NavSearch *search, NavNode *node);
u8 NavPath_StepSearch(void *search, NavNode **outNode);
void SamNav_BuildGraph(u16 listId);
void SamNav_DebugDumpGraph();
int SamNav_DistToEdge(Vec3s *point, SamEdgeNormal *normal, short x0, short z0, short x1, short z1, Vec3s *closest);
NavNode *SamNav_FindEdgeNear(Vec3s *position, u8 *edgeOut, Vec3s *closest);
NavNode *SamNav_FindNearestNode(s16 x, s16 z, int *outDistanceSquared);
NavNode *SamNav_FindNearestNodeAhead(s16 x, s16 z, Vec3s *direction, int directionLength, int anyDirection);
NavNode *SamNav_FindNearestReachableNode(s16 fromX, s16 fromZ, s16 x, s16 y, s16 z);
NavNode *SamNav_FindNodeTowards(s16 fromX, s16 fromZ, s16 x, s16 z);
int SamNav_IsGoalNearOrNotAhead(Vec3s *position, Vec3s *toWaypoint, int waypointDistance, Vec3s *goal);
void SamNav_ResetGraph();

#endif
