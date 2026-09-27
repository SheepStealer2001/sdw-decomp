#ifndef SDW_OBJECTS_ANIMATION_H
#define SDW_OBJECTS_ANIMATION_H

/* The functions and globals animation.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

struct Animator;
class Camera;
class Instance;
class Mat44;

extern char g_fmtAnimNotFound[];
void Anim_Advance(Animator *anim); /* 0x55060c */
u16 Anim_DecodeKey(Animator *, const void *, u16);
void Anim_GetDebugNames(Instance *instance, u16 id, const char **modelName, const char **animationName);
u32 Anim_GetDurationMs(Instance *instance, u16 id, u8 includeFirstTrack); /* 0x5503d8 */
void Anim_GetRootOffset(Instance *inst, Animator *anim, Vec3s *out);      /* 0x55086d */
void Anim_InterpolatePose(Animator *);
void Anim_PostInitStub(Instance *, Animator *);
void Anim_RestorePose(Animator *anim); /* 0x5504e1 */
u32 Anim_StartDirect(Instance *, Animator *, u16, u32);
void Instance_CalcWorldMatrix(Instance *, Camera *, const Vec3s *, Mat44 *);
void Instance_DrawAnimParts(Instance *inst, Animator *anim, Camera *cam, const Vec3s *scaleOrNull); /* 0x550e98 */

#endif
