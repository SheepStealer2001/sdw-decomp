#ifndef SDW_OBJECTS_INSTANCE_H
#define SDW_OBJECTS_INSTANCE_H

/* The functions and globals instance.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

struct Aabb;
struct Animator;
struct AttachLink;
class Camera;
class Instance;
class ScnObject;
class WorldObj;

extern u16 g_attachLinkCount;                                                                   /* 0x6e4a08 */
void Animator_Init(Instance *instance, Animator *animation, void *poseBuffers, s32 keepBounds); /* 0x55b3ae */
AttachLink *AttachLink_Alloc(Instance *parent, ScnObject *owner, u8 partIndex, const Vec3s *offset, const Vec3s *rot,
                             s32 rootRotation, const Vec3s *worldOffset); /* 0x55ba1a */
AttachLink **AttachLink_FindFirstChildSlot(Instance *parentInst);         /* 0x55b7ca */
void AttachLink_Free(AttachLink *link);                                   /* 0x55bb1a */
void AttachLink_InitPool();                                               /* 0x55b769 */
void AttachLink_SetParams(AttachLink *link, u8 joint, const Vec3s *localOffset, const Vec3s *rotation, s32 rootRotation,
                          const Vec3s *worldOffset);
s8 Cull_IsAabbVisible(const Aabb *aabb, const Camera *camera);                                /* 0x55bbe6, T294 */
void *Dav_GetResourcePtr(u16 index);                                                          /* 0x55b1ee */
void Instance_InitFromWarRecord(Instance *instance, u16 *record, u8 mode, s32 keepTransform); /* 0x55b28c */
void *WorldObj_CreateFromResource(u32 resIdx); /* 0x55b510 (32-bit index: the callee reads a dword at 0x55b546) */
void WorldObj_Free(WorldObj *object);

/* The functions and globals instance.cpp defines, declared once for every file that uses them. */

struct AttachLink;

extern u8 g_aabbEdgePairs[16][2];
extern AttachLink *g_attachLinkPool[10]; /* 0x6e49e0 */

#endif
