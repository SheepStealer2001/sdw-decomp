#ifndef SDW_OBJECTS_CAMERA_H
#define SDW_OBJECTS_CAMERA_H

/* The functions and globals camera.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

struct CamRestrict;
class Camera;
class CollBox;
class Pad;
class ScnObject;
struct Trajectory;
class box;

extern s16 g_camMinPitch;       /* 0x6e45f8 */
extern u8 g_camMode;            /* 0x6e4383 CameraMode (CAM_LOOK = look mode) */
extern s16 g_camReqDist;        /* 0x6e45ae */
extern s16 g_camReqFocal;       /* 0x6e459e */
extern s16 g_camReqSpeedFactor; /* 0x6e45a6 */
extern CamRestrict *g_camRestriction;
extern u32 g_camScriptFlags;        /* 0x6e438c */
extern ScnObject *g_camScriptOwner; /* 0x6e4374 */
extern s16 g_camShakeAmplitude;
void Camera_ApplyRestriction(Vec3s *, s32 *, s16 *, s16 *, const CamRestrict *);
void Camera_BuildViewMatrix(Camera *, const Vec3s *); /* 0x55a9d2 */
void Camera_ClampToTrajectory(Vec3s *, Trajectory *, s32);
void Camera_EaseFocal(s16, s16, u16);
void Camera_EaseLerpAngles(Vec3s *, const Vec3s *, const Vec3s *, const u8 *, u16, u16, u16);
void Camera_EaseLerpVec3s(Vec3s *, const Vec3s *, const u8 *, u16, u16, u16);
void Camera_EyeToAngles(const Vec3s *, Vec3s *, s16 *, const Vec3s *);
s16 Camera_FindClearDistance(const Vec3s *, const Vec3s *, s16);
void Camera_FollowTrajectory(Trajectory *, Vec3s *, s16 *, s32 *, s32);
void Camera_FreeLevel(); /* 0x55a308 */
void Camera_InitOnceStub();
void Camera_InitSettings(); /* 0x55a227 */
s16 Camera_IsEyeClear(const Camera *, const Vec3s *);
s32 Camera_IsPointOnScreen(Camera *, Vec3s, s16, const Vec3s *, const Vec3s *, s32 *);
void Camera_OnManualRotate(s32);
void Camera_ReleaseAny();                      /* 0x55a30d */
void Camera_ReleaseScripted(ScnObject *owner); /* 0x55a367 */
void Camera_Reset();                           /* 0x559ff1 */
void Camera_ResetMotion();
void Camera_ResetUnderwaterOverlay();
void Camera_SetMode(u8 mode, u8 smooth); /* 0x559e07 */
void Camera_SetSamChase(Vec3s *position, short y, CollBox *box);
void Camera_ShakeOffset(Vec3s *);
void Camera_SmoothToDesired(Camera *, Vec3s *, const s32 *, s16 *, s16, const Vec3s *, s32);
void Camera_SolveCollision(Camera *, Vec3s *, s16, s32, s16);
void Camera_StartShake(s16 amplitude, s32 durationTicks); /* 0x55262a */
s16 Camera_StickAxisDeadzone(u8);
void Camera_StopShake();
u16 Camera_SweepEyeBox(const Vec3s *, const s16 *, Vec3s *, const Vec3s *, Vec3s *);
void Camera_Update(Camera *cam, u8 debugMode, Pad *pad, Pad *pad2); /* 0x55a3d0 */
void Camera_UpdateDebugFreeCam(Camera *, Pad *, Pad *);
void Camera_UpdateDirected(Camera *, Pad *);
void Camera_UpdateFollow(Camera *, Pad *);
void Camera_UpdateLedgeLook(Camera *);
void Camera_UpdateLook(Camera *, Pad *);
void Camera_UpdateRocket(Camera *, Pad *);
void Camera_UpdateScriptBlendIn(Camera *);
void Camera_UpdateScriptToScript(Camera *);
void Camera_UpdateScriptedHold(Camera *);
void Camera_UpdateUnderwater(); /* 0x55adec */
void Color_ScaleClamp(const u8 *src, u8 *dst, float scale, u8 maximum);
void Vec3s_OffsetAlongAngles(Vec3s *, const Vec3s *, s16, const Vec3s *); /* 0x554f8a */

/* The functions and globals camera.cpp defines, declared once for every file that uses them. */

struct CamFlagBits;
struct CamModeParams;
struct CamRequestBits;

extern Vec3s g_camChaseTarget2;
extern CamFlagBits g_camFlags;          /* 0x6e4384 */
extern CamModeParams g_camModeParams[]; /* 0x6e4428 [13]; [6] is the robot camera */
extern Vec3s g_camReqAimOffset;         /* 0x6e45a8 */
extern CamRequestBits g_camReqFlags;    /* 0x6e45be */
extern Vec3s g_camReqRot;               /* 0x6e45b8 pitch, yaw, roll */
extern Vec3s g_camReqTarget;            /* 0x6e45a0 */
extern Vec3s g_camReqVel;               /* 0x6e4598 */

#endif
