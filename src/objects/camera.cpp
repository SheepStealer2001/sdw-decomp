/*
 * T293 - original object guessed as Camera.cpp (data/tu_map.json). Ranges: .text 0x552450-0x55b19e, .rdata
 * 0x5771c0-0x5771d0 (the COMDAT doubles 0.5 and 651.898... = 4096 / 2pi), .data 0x57e7a0-0x57ead0 (g_camEaseCurve,
 * g_camModeDefaults, nine unreferenced debug messages), .bss 0x6e4350-0x6e4620 (the camera state).
 * The camera: 52 functions in address order, among them Vec3s_OffsetAlongAngles 0x554f8a and Color_ScaleClamp 0x55abb7.
 *
 * Notes on the source:
 *  - One set of declarations for the whole object; the bit views of CamRestrict.flags and of the pitch/push/probe
 *    globals are the generated CamRestrictBits / CamPitchAdjBits / CamSamPush / CamProbeFrame. Camera_BuildViewMatrix
 *    is declared once, as
 *    `(Camera *, const Vec3s *)`: a second prototype with `Vec3s *` would be a second overload (an undefined symbol at
 *    link).
 *  - Names that are not objects of their own are spelled as what they are: g_padMasks = &g_inputMap[4] (0x57eb80,
 *    see T298), g_camPos = g_camera.pos, g_screenW / g_screenH / g_projFocalScale = fields of g_screen. As separate
 *    symbols nothing would define them at link; the code is the same. g_sharedScratch is declared with C++ linkage,
 *    as every other user declares it; g_gameTime is declared u32, as Time (T304) defines it.
 *  - The object's data is defined here, in address order (values from the exe, tools/data_init.py). g_camEaseCurve and
 *    g_camModeDefaults are in .data, so they were not const (their decorated names have no const; only this object
 *    uses them).
 *  - .bss: explicit zero initialisers keep definition order (VC6 orders uninitialised globals by a hash of their
 *    names). Nine Vec3s of the camera state sit on 2 mod 4, where VC6 never puts a standalone struct, so the original
 *    kept them in aggregates; the smallest aggregates that reproduce the layout are CamAimState, CamScriptState,
 *    CamProbeState and CamResetPositions (g_camAimState 0x6e435c, g_camScriptState 0x6e4398, g_camProbeState 0x6e43e4,
 *    g_camResetPositions 0x6e4400), and the descriptive per-field names are macros for their members
 *    (g_camFocal = g_camAimState.focal, ...). Other objects that read g_camScriptEye (CameraManager2) or
 *    g_camScriptReturnMode (Robot, Wolf) declare them the same way (src/include/sdw_global_views.h). Two 2-byte runs no
 *    code names (0x6e4422, 0x6e45f6) are placeholders; the second is inside the 24-byte settings block that
 *    Camera_InitSettings copies, which suggests g_camEyeBoxMin/Max were 8-byte (4-component) vectors in the original.
 *  - The scratch-record grouping (the `w` Work structs) and the local names reproduce the original frames; they are
 *    not recovered.
 */
/* BYTES: dead-code, layout, slot-group, slot-name, slot-scope, view. */
/* BYTES(layout): written '= 0' only to keep definition order in .bss */
/* BYTES(layout): aggregate only for the layout: VC6 never puts a standalone Vec3s on 2 mod 4, so these globals were members of a larger object */
/* BYTES(layout): placeholder: 2 unreferenced bytes kept only for the layout */
/* BYTES(layout): unreferenced statics standing in for compiled-out trace literals, in the exe's order and spacing */
/* BYTES(layout): not const: the original has it in .data */
/* BYTES(view): bitfield view: the original clears these words whole and reads them as one-bit fields */
#include "sdw_types.h"

#include "sdw_enums.h"
#include "scenaric_props.h"
#include "sdw_classes.h"
#include "sdw_global_views.h"

/* ---- local bit views and scratch records ---- */
struct CameraStateBits {
    s32 samPushOut : 1, samPushIn : 1, unused2 : 1, rotateNegative : 1, rotatePositive : 1, skipSweep : 1, hit : 1,
        lookExitBlocked : 1, cut : 1, unused9 : 23; /* the follow camera reads bit 8 as "no zoom-out" */
};
/* g_camStateFlags and g_camFlags2 are cleared bit by bit with whole-word and/or AND read as one-bit fields */
union CameraStateWord {
    u32 all;
    CameraStateBits bits;
};
struct CameraOverheadBit {
    s32 overhead : 1, unused : 31;
};
union CameraFlags2Word {
    u32 all;
    CameraOverheadBit bits;
};
struct CameraSweepScratch {
    Vec3i movement, normalSum;
    Vec3s predicted, corner, toTarget;
    u16 unused2a;
    Vec3s toCurrent;
    u8 classes[16];
};
struct CameraBlendWork {
    u16 unused38, time;
    Vec3s shake;
    u16 unused2e;
    Vec3s goal;
    u16 unused26;
    Vec3s eye;
    u16 unused1e;
    Vec3i delta;
    Vec3s aim;
    u16 unused0a;
    Vec3s angles;
    u16 unused02;
};
struct CameraSettingsCopy {
    u32 words[6];
};
/* Same packed directory as the loader; the generator leaves it forward declared because its
 * pointers have unaligned offsets. */
#pragma pack(push, 1)
struct DavDirectory {
    u16 indexCount, bitmapCount, unk04;
    u16 *indices;
    DavBitmapRec *bitmaps;
    u32 fileSize;
    u32 *idLists;
};
#pragma pack(pop)

/* ---- aggregates of this object's .bss (see the header) ----
 * VC6 places a standalone global of 3 bytes or more (a struct or array) on a 4-byte boundary (an object of 64 bytes or
 * more on 8), yet nine of the camera's Vec3s sit on 2 mod 4. They can only be members of aggregates: each group below
 * is the smallest run of neighbours, from the nearest 4-aligned item boundary, that puts them at their addresses
 * (adjacent runs merged). The original grouping is not known. */
struct CamAimState { /* 0x6e435c */
    s16 focal;       /* +0x00 g_camFocal */
    Vec3s aimOffset; /* +0x02 g_camAimOffset */
};
struct CamProbeState { /* 0x6e43e4 */
    s16 yawOffset;     /* +0x00 g_camProbeYawOffset */
    Vec3s startEye;    /* +0x02 g_camProbeStartEye */
};
struct CamResetPositions { /* 0x6e4400, the positions Camera_ResetMotion / Camera_Reset set to 0x8000 */
    Vec3s unusedMotionPos; /* +0x00 g_camUnusedMotionPos */
    Vec3s unusedResetPos;  /* +0x06 g_camUnusedResetPos */
    Vec3s blockPosNeg;     /* +0x0c g_camBlockPosNeg */
    Vec3s blockPosPos;     /* +0x12 g_camBlockPosPos */
};
SDW_SIZE(CamAimState, 8);
SDW_SIZE(CamScriptState, 0x2c);
SDW_AT(CamScriptState, prevRot, 0x16);
SDW_AT(CamScriptState, preRot, 0x26);
SDW_SIZE(CamProbeState, 8);
SDW_SIZE(CamResetPositions, 0x18);

/* ---- other objects' data ---- */
#include "../app/app_main.h"
#include "../engine/game_state.h"
#include "../engine/draw2d.h"
#include "../engine/maths.h"
#include "../engine/screen.h"
#include "../engine/scenaric.h"
#include "../engine/input.h"
#include "../engine/fixed_math.h"
#include "../engine/collide.h"
#include "../engine/map.h"
#include "../engine/approach.h"
#include "../engine/scn_tools.h"
#include "../engine/load_dav.h"
#include "../engine/load_warmeshes.h"
#include "../engine/sound_mgr.h"
extern u8 g_sharedScratch[]; /* 0x6d5468  C++ linkage, as the rest of the game declares it */
extern s32 g_dt;
extern s32 g_dtMs;
extern u32 g_gameFlags;
extern u32 g_gameTime; /* 0x71b2d0  u32, as Time (T304) defines it */
extern Wolf *g_pWolf;
extern u32 *g_screenLayerBase;
extern u32 g_waterColor;
#define g_padMasks \
    (g_inputMap +  \
     4) /* 0x57eb80 is &g_inputMap[4]: not an object of its own */ /* fields of other objects that the code reads under names of their own (not symbols: spelled as the fields) */
#define g_camPos (g_camera.pos)              /* 0x584d20 = g_camera 0x584c50 + 0xd0 */
#define g_screenW (g_screen.viewportWidth)   /* 0x6d6ff4 = g_screen 0x6d6fe0 + 0x14 */
#define g_screenH (g_screen.viewportHeight)  /* 0x6d6ff6 = g_screen + 0x16 */
#define g_projFocalScale (g_screen.projDist) /* 0x6d7068 = g_screen + 0x88 */

/* ---- functions of other objects, and forward declarations ---- */
#include "../sdk/crt.h"
extern "C" s32 Collide_SweepBox(ScnObject *, CollBox *, Vec3s *, s32 *, s32 *, CollContact *, ScnObject *, u8, s32 *,
                                ScnObject **, s32);
extern "C" s32 Coll_BoxGroundQuery(CollBox *, s32 *, ScnObject *, u8, ScnObject **);
s32 Rand_Bounded(s32);
void Camera_BuildViewMatrix(Camera *, const Vec3s *);
void Vec3s_OffsetAlongAngles(Vec3s *, const Vec3s *, s16, const Vec3s *);
void Camera_ClampToTrajectory(Vec3s *, Trajectory *, s32);
void Camera_EyeToAngles(const Vec3s *, Vec3s *, s16 *, const Vec3s *);
s16 Camera_FindClearDistance(const Vec3s *, const Vec3s *, s16);
u16 Camera_SweepEyeBox(const Vec3s *, const s16 *, Vec3s *, const Vec3s *, Vec3s *);
void Camera_ShakeOffset(Vec3s *);
void Camera_OnManualRotate(s32);
void Camera_ApplyRestriction(Vec3s *, s32 *, s16 *, s16 *, const CamRestrict *);
void Camera_FollowTrajectory(Trajectory *, Vec3s *, s16 *, s32 *, s32);
void Camera_SmoothToDesired(Camera *, Vec3s *, const s32 *, s16 *, s16, const Vec3s *, s32);
s32 Camera_IsPointOnScreen(Camera *, Vec3s, s16, const Vec3s *, const Vec3s *, s32 *);
void Camera_SolveCollision(Camera *, Vec3s *, s16, s32, s16);
s32 Vec3s_ManhattanDist(Vec3s *, Vec3s *);
void Camera_UpdateLedgeLook(Camera *);
s16 Camera_IsEyeClear(const Camera *, const Vec3s *);
void Camera_SetMode(u8 mode, u8 smooth);
void Camera_EaseLerpVec3s(Vec3s *, const Vec3s *, const u8 *, u16, u16, u16);
void Camera_EaseLerpAngles(Vec3s *, const Vec3s *, const Vec3s *, const u8 *, u16, u16, u16);
void Camera_EaseFocal(s16, s16, u16);
void Camera_StopShake();
void Camera_InitOnceStub();
void Camera_ResetUnderwaterOverlay();
void Camera_ResetMotion();
void Camera_UpdateSamChase(Camera *, Pad *);
void Camera_UpdateFollow(Camera *, Pad *);
void Camera_UpdateLook(Camera *, Pad *);
void Camera_UpdateFixedLookAt(Camera *);
void Camera_UpdateRocket(Camera *, Pad *);
void Camera_UpdateDirected(Camera *, Pad *);
void Camera_UpdateScriptedHold(Camera *);
void Camera_UpdateScriptReturn(Camera *);
void Camera_UpdateScriptBlendIn(Camera *);
void Camera_UpdateScriptToScript(Camera *);
void Camera_UpdateDebugFreeCam(Camera *, Pad *, Pad *);
s16 Camera_StickAxisDeadzone(u8);
void Color_ScaleClamp(const u8 *src, u8 *dst, float scale, u8 maximum);
u32 *Res_GetValidatedIdList(u16 id, u16 *count);
void Draw2D_TexRect(float z, float x0, float y0, float x1, float y1, s32 page, float u0, float v0, u32 c0, float u1,
                    float v1, u32 c1, float u2, float v2, u32 c2, float u3, float v3, u32 c3);
u16 Sound_Play(u16, void *, u16, u8, s32);

/* ---- this object's .data (0x57e7a0-0x57ead0), in address order ---- */
/* 0x57e7a0 - the ease-in/out curve of the scripted blends (Camera_EaseLerp*); in .data, so not const */
u8 g_camEaseCurve[256] = {
    0,   0,   0,   0,   0,   1,   1,   1,   2,   2,   2,   2,   2,   3,   3,   4,   4,   4,   4,   5,   5,   5,
    5,   6,   6,   7,   7,   7,   7,   8,   8,   9,   9,   10,  10,  10,  11,  11,  12,  12,  13,  13,  14,  14,
    15,  15,  16,  16,  17,  17,  18,  18,  19,  20,  20,  21,  21,  22,  22,  23,  24,  25,  25,  26,  27,  28,
    28,  29,  30,  31,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  47,  48,  49,
    51,  52,  53,  55,  56,  57,  59,  60,  62,  63,  65,  67,  68,  70,  72,  73,  75,  77,  79,  81,  83,  85,
    87,  89,  91,  93,  95,  97,  99,  102, 104, 106, 109, 111, 113, 115, 118, 120, 123, 125, 128, 130, 132, 135,
    137, 140, 142, 144, 146, 149, 151, 153, 156, 158, 160, 162, 164, 166, 168, 170, 172, 174, 176, 178, 180, 182,
    183, 185, 187, 188, 190, 192, 193, 195, 196, 198, 199, 201, 202, 203, 204, 206, 207, 208, 210, 211, 212, 213,
    214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 224, 225, 226, 227, 227, 228, 229, 230, 230, 231, 232,
    233, 233, 234, 234, 235, 235, 236, 237, 237, 238, 238, 239, 239, 240, 240, 241, 241, 242, 242, 243, 243, 244,
    244, 245, 245, 245, 246, 246, 247, 247, 248, 248, 248, 248, 249, 249, 250, 250, 250, 250, 251, 251, 251, 251,
    252, 252, 253, 253, 253, 253, 253, 254, 254, 254, 255, 255, 255, 255};
/* 0x57e8a0 - the per-mode defaults Camera_InitSettings copies into g_camModeParams; in .data, so not const */
CamModeParams g_camModeDefaults[13] = {
    {
        300 /* pitchSpeed */, 1500 /* yawSpeed */, 800 /* rollSpeed */, 6144 /* pitchRollAccel */, 2048 /* yawAccel */,
        6144 /* yawDecel */, 610 /* dist */, 384 /* focal */
    },
    {
        1600 /* pitchSpeed */, 2400 /* yawSpeed */, 1600 /* rollSpeed */, 8192 /* pitchRollAccel */,
        16384 /* yawAccel */, 24576 /* yawDecel */, 0 /* dist */, 384 /* focal */
    },
    {
        300 /* pitchSpeed */, 1536 /* yawSpeed */, 800 /* rollSpeed */, 2048 /* pitchRollAccel */, 32768 /* yawAccel */,
        32768 /* yawDecel */, 370 /* dist */, 230 /* focal */
    },
    {
        300 /* pitchSpeed */, 2048 /* yawSpeed */, 800 /* rollSpeed */, 2048 /* pitchRollAccel */, 2048 /* yawAccel */,
        2048 /* yawDecel */, 510 /* dist */, 321 /* focal */
    },
    {
        0 /* pitchSpeed */, 0 /* yawSpeed */, 0 /* rollSpeed */, 0 /* pitchRollAccel */, 0 /* yawAccel */,
        0 /* yawDecel */, 0 /* dist */, 321 /* focal */
    },
    {
        300 /* pitchSpeed */, 1000 /* yawSpeed */, 500 /* rollSpeed */, 2048 /* pitchRollAccel */, 4096 /* yawAccel */,
        6144 /* yawDecel */, 400 /* dist */, 384 /* focal */
    },
    {
        300 /* pitchSpeed */, 1500 /* yawSpeed */, 800 /* rollSpeed */, 6144 /* pitchRollAccel */, 2048 /* yawAccel */,
        6144 /* yawDecel */, 230 /* dist */, 284 /* focal */
    },
    {
        2048 /* pitchSpeed */, 2048 /* yawSpeed */, 2048 /* rollSpeed */, 4096 /* pitchRollAccel */,
        4096 /* yawAccel */, 4096 /* yawDecel */, 0 /* dist */, 0 /* focal */
    },
    {
        1500 /* pitchSpeed */, 1500 /* yawSpeed */, 1500 /* rollSpeed */, 2048 /* pitchRollAccel */,
        2048 /* yawAccel */, 6144 /* yawDecel */, 610 /* dist */, 384 /* focal */
    },
    {
        1500 /* pitchSpeed */, 1500 /* yawSpeed */, 1500 /* rollSpeed */, 2048 /* pitchRollAccel */,
        2048 /* yawAccel */, 6144 /* yawDecel */, 610 /* dist */, 384 /* focal */
    },
    {
        1500 /* pitchSpeed */, 1500 /* yawSpeed */, 1500 /* rollSpeed */, 2048 /* pitchRollAccel */,
        2048 /* yawAccel */, 6144 /* yawDecel */, 610 /* dist */, 384 /* focal */
    },
    {
        300 /* pitchSpeed */, 1200 /* yawSpeed */, 800 /* rollSpeed */, 2048 /* pitchRollAccel */, 2048 /* yawAccel */,
        6144 /* yawDecel */, 400 /* dist */, 384 /* focal */
    },
    {
        300 /* pitchSpeed */, 3000 /* yawSpeed */, 800 /* rollSpeed */, 0 /* pitchRollAccel */, 2048 /* yawAccel */,
        6144 /* yawDecel */, 700 /* dist */, 384 /* focal */
    }};
/* 0x57ea0c-0x57ead0 - nine debug messages no code refers to (the camera's trace output, compiled out). Written as
 * unreferenced initialised statics, which VC6 keeps, in the order and at the 4-byte spacing of the exe; the
 * original most likely had them as literals of trace calls that produced no code. */
static char g_camDbgTooFar_57ea0c[] = "TOO FAR test : s8BlockedCount=%d, Max=%d\n";
static char g_camDbgCutting_57ea38[] = "CUTTING\n";
static char g_camDbgCollisionTest_57ea44[] = "COLLISION TEST\n";
static char g_camDbgCollision_57ea54[] = "*COLLISION*\n";
static char g_camDbgTurning_57ea64[] = "TURNING\n";
static char g_camDbgTestingAim_57ea70[] = "Testing aim\n";
static char g_camDbgStars_57ea80[] = "***************\n";
static char g_camDbgStartRunning_57ea94[] = "                   =>Start running0\n";
static char g_camDbgSetMode_57eabc[] = "setcameragamemode\n";

/* ---- this object's .bss (0x6e4350-0x6e4620), in address order: explicit zero initialisers keep definition order
 * (VC6 orders uninitialised globals by a hash of their names); aggregates as above; two placeholders ---- */
s32 g_camPitchVel = 0;                       /* 0x6e4350 */
s32 g_camYawVel = 0;                         /* 0x6e4354 */
s32 g_camRollVel = 0;                        /* 0x6e4358 */
CamAimState g_camAimState = {0};             /* 0x6e435c  g_camFocal, g_camAimOffset */
s16 g_camTrajSegment = 0;                    /* 0x6e4364 */
s32 g_camAimOffsetVel = 0;                   /* 0x6e4368 */
s32 g_camDistVel = 0;                        /* 0x6e436c */
Trajectory *g_camTrajectory = 0;             /* 0x6e4370 */
ScnObject *g_camScriptOwner = 0;             /* 0x6e4374 */
s32 g_camShakeDuration = 0;                  /* 0x6e4378 */
s32 g_camShakeElapsed = 0;                   /* 0x6e437c */
s16 g_camShakeAmplitude = 0;                 /* 0x6e4380 */
u8 g_camLedgeLookState = 0;                  /* 0x6e4382 */
u8 g_camMode = 0;                            /* 0x6e4383 */
CamFlagBits g_camFlags = {0};                /* 0x6e4384 */
CameraFlags2Word g_camFlags2 = {0};          /* 0x6e4388 */
u32 g_camScriptFlags = 0;                    /* 0x6e438c */
s32 g_camScriptStartTime = 0;                /* 0x6e4390 */
u32 g_camScriptBlendTime = 0;                /* 0x6e4394 */
CamScriptState g_camScriptState = {0};       /* 0x6e4398  g_camScriptEye .. g_camPreScriptRot */
s16 g_camPreScriptDist = 0;                  /* 0x6e43c4 */
s32 g_camAutoPitchRate = 0;                  /* 0x6e43c8 */
s32 g_camAutoYawRate = 0;                    /* 0x6e43cc */
s32 g_camAutoRollRate = 0;                   /* 0x6e43d0 */
s16 g_camZoomOut = 0;                        /* 0x6e43d4 */
s8 g_camTurnState = 0;                       /* 0x6e43d6 */
CamPitchAdjBits g_camPitchAdjState = {0};    /* 0x6e43d7 */
Vec3s g_camLastHitTarget = {0};              /* 0x6e43d8 */
s8 g_camHitFrames = 0;                       /* 0x6e43de */
CamProbeFrame g_camProbeFrame = {0};         /* 0x6e43df */
s16 g_camProbeStartTargetVert = 0;           /* 0x6e43e0 */
s16 g_camProbeYaw = 0;                       /* 0x6e43e2 */
CamProbeState g_camProbeState = {0};         /* 0x6e43e4  g_camProbeYawOffset, g_camProbeStartEye */
s16 g_camTurnTimerMs = 0;                    /* 0x6e43ec */
s16 g_camPitchAdjTimerMs = 0;                /* 0x6e43ee */
s8 g_camFrameCounter = 0;                    /* 0x6e43f0 */
s8 g_camBlockedCountA = 0;                   /* 0x6e43f1 */
CameraStateWord g_camStateFlags = {0};       /* 0x6e43f4 */
CamSamPush g_camSamPush = {0};               /* 0x6e43f8 */
s16 g_camSamPushTimerMs = 0;                 /* 0x6e43fa */
s32 g_camUnused43fc = 0;                     /* 0x6e43fc */
CamResetPositions g_camResetPositions = {0}; /* 0x6e4400  g_camUnusedMotionPos .. g_camBlockPosPos */
s32 g_camLosClearTime = 0;                   /* 0x6e4418 */
s32 g_camLookTime = 0;                       /* 0x6e441c */
s8 g_camBlockedCountB = 0;                   /* 0x6e4420 */
s16 g_camUnref_6e4422 = 0;                   /* 0x6e4422  placeholder: no code refers to these 2 bytes, but a u16 alone
                                          *           would sit at 0x6e4422, not at 0x6e4424 */
u16 g_camUnderwaterSnd = 0;                  /* 0x6e4424 */
CamModeParams g_camModeParams[13] = {0};     /* 0x6e4428 */
CamRestrict *g_camRestriction = 0;           /* 0x6e4594 */
Vec3s g_camReqVel = {0};                     /* 0x6e4598 */
s16 g_camReqFocal = 0;                       /* 0x6e459e */
Vec3s g_camReqTarget = {0};                  /* 0x6e45a0 */
s16 g_camReqSpeedFactor = 0;                 /* 0x6e45a6 */
Vec3s g_camReqAimOffset = {0};               /* 0x6e45a8 */
s16 g_camReqDist = 0;                        /* 0x6e45ae */
Vec3s g_camChaseTarget2 = {0};               /* 0x6e45b0 */
s16 g_camChaseEyeVert = 0;                   /* 0x6e45b6 */
Vec3s g_camReqRot = {0};                     /* 0x6e45b8 */
CamRequestBits g_camReqFlags = {0};          /* 0x6e45be */
s16 g_camChaseBox[4] = {0};                  /* 0x6e45c0 */
u8 g_uwOverlayReady = 0;                     /* 0x6e45c8 */
u32 g_uwOverlayTexPage = 0;                  /* 0x6e45cc */
u32 g_uwOverlayColor = 0;                    /* 0x6e45d0 */
float g_uwOverlayUV[4] = {0};                /* 0x6e45d4 */
u32 g_camSettings = 0;            /* 0x6e45e4  the 24-byte settings block Camera_InitSettings copies to the backup */
Vec3s g_camEyeBoxMin = {0};       /* 0x6e45e8 */
Vec3s g_camEyeBoxMax = {0};       /* 0x6e45f0 */
s16 g_camUnref_6e45f6 = 0;        /* 0x6e45f6  placeholder: 2 bytes no code names (inside the copied block);
                                          *           an s16 alone would put g_camMinPitch here instead of 0x6e45f8 */
s16 g_camMinPitch = 0;            /* 0x6e45f8 */
s16 g_camAutoYawGain = 0;         /* 0x6e45fa */
u32 g_camSettingsBackup[6] = {0}; /* 0x6e45fc */
Vec3s g_camChaseHyst = {0};       /* 0x6e4614 */
u8 g_camSettingsInit = 0;         /* 0x6e461a */

/* the members of the aggregates under the names the code uses */
#define g_camFocal (g_camAimState.focal)
#define g_camAimOffset (g_camAimState.aimOffset)
#define g_camScriptEye (g_camScriptState.eye)
#define g_camScriptRot (g_camScriptState.rot)
#define g_camScriptFocal (g_camScriptState.focal)
#define g_camScriptPrevEye (g_camScriptState.prevEye)
#define g_camScriptReturnMode (g_camScriptState.returnMode)
#define g_camScriptPrevRot (g_camScriptState.prevRot)
#define g_camScriptPrevFocal (g_camScriptState.prevFocal)
#define g_camPreScriptEye (g_camScriptState.preEye)
#define g_camPreScriptFocal (g_camScriptState.preFocal)
#define g_camPreScriptRot (g_camScriptState.preRot)
#define g_camProbeYawOffset (g_camProbeState.yawOffset)
#define g_camProbeStartEye (g_camProbeState.startEye)
#define g_camUnusedMotionPos (g_camResetPositions.unusedMotionPos)
#define g_camUnusedResetPos (g_camResetPositions.unusedResetPos)
#define g_camBlockPosNeg (g_camResetPositions.blockPosNeg)
#define g_camBlockPosPos (g_camResetPositions.blockPosPos)

/* ---- source-only inline helpers (all are inlined where used) ---- */
inline s32 InstanceFlags(const u16 *inst, u16 mask)
{
    return *inst & mask;
}
inline void SetVectorPad(Vec4i *vector, s16 value)
{
    vector->pad = value;
}
inline s32 CameraAtanAngle(s32 y, s32 x)
{
    double value = atan2((double)y, (double)x);
    value *= 651.898646904404;
    if (value < 0.0)
        value -= 0.5;
    else
        value += 0.5;
    return (s32)value;
}
#define SDW_INLINE_FREE_CAMERA_ISSCRIPTCONTROLLED 1
#include "camera_inlines.h"
#undef SDW_INLINE_FREE_CAMERA_ISSCRIPTCONTROLLED
inline ScnObject *Camera_ScriptOwner()
{
    if (Camera_IsScriptControlled())
        return g_camScriptOwner;
    else
        return 0;
}
inline s32 Camera_RadiansToAngle(double value)
{
    value *= 651.898646904404;
    if (value < 0)
        value -= 0.5;
    else
        value += 0.5;
    return (s32)value;
}
inline s32 Camera_CurrentProjection()
{
    s32 value = g_projFocalScale;
    return value;
}
inline s32 Camera_GetProjection()
{
    s32 value = g_projFocalScale;
    return value;
}
inline u16 Camera_VirtualScreenWidth()
{
    return 512;
}
inline ZoneList *Camera_WaterZones(u8 type)
{
    return &g_waterZones[type];
}
#define SDW_INLINE_ZONELIST_FINDCONTAINING_VEC3S 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_FINDCONTAINING_VEC3S

/* ---- macros (identical definitions merged) ---- */
#define ABS_VALUE(v) ((v) >= 0 ? (v) : -(v))
#define ANGLE_DELTA(a, b) ((s16)((s16)(((a) - (b) + 0x800) & 0xfff) - 0x800))
#define DELTA_ANGLE(a, b) ((s16)((s16)(((a) - (b) + 0x800) & 0xfff) - 0x800))
#define DOT(a, b) ((a).x * (b).x + (a).y * (b).y + (a).z * (b).z)
#define SUBTRACT_NORMAL(p)                                    \
    w.scratch->movement.x -= (p * w.contact->normal.x) >> 18; \
    w.scratch->movement.y -= (p * w.contact->normal.y) >> 18; \
    w.scratch->movement.z -= (p * w.contact->normal.z) >> 18
#define ADD_NORMAL()                               \
    w.scratch->normalSum.x += w.contact->normal.x; \
    w.scratch->normalSum.y += w.contact->normal.y; \
    w.scratch->normalSum.z += w.contact->normal.z; \
    ++w.normalCount
#define FILL_RAY()              \
    w.rayFrom.x = w.probeEye.x; \
    w.rayFrom.y = w.probeEye.y; \
    w.rayFrom.z = w.probeEye.z; \
    w.rayTo.x = w.target.x;     \
    w.rayTo.y = w.target.y;     \
    w.rayTo.z = w.target.z
#define SAVE_PROBE_START()                  \
    g_camProbeStartEye = cam->pos;          \
    g_camProbeStartTargetVert = w.target.y; \
    g_camTurnTimerMs = 100

/* ---- the functions, in address order ---- */
/* 0x552450 */
void Camera_OnManualRotate(s32 direction)
{
    g_camTurnState = TURN_IDLE;
    g_camBlockedCountA = 0;
    g_camBlockedCountB = 0;
    g_camHitFrames = 0;
    g_camAutoPitchRate = g_camAutoYawRate = g_camAutoRollRate = 0;
    g_camStateFlags.bits.rotateNegative = direction < 0;
    g_camStateFlags.bits.rotatePositive = direction > 0;
    if (g_camStateFlags.bits.rotateNegative) {
        g_camBlockPosPos.x = 0x8000;
        g_camBlockPosPos.y = 0x8000;
        g_camBlockPosPos.z = 0x8000;
    }
    if (g_camStateFlags.bits.rotatePositive) {
        g_camBlockPosNeg.x = 0x8000;
        g_camBlockPosNeg.y = 0x8000;
        g_camBlockPosNeg.z = 0x8000;
    }
    g_camUnused43fc = 10000;
    g_camAutoPitchRate = 0;
    g_camAutoYawRate = 0;
    g_camAutoRollRate = 0;
}
/* 0x55254c */
void Camera_ShakeOffset(Vec3s *out)
{
    s16 amplitude;
    if (g_camShakeAmplitude > 0) {
        g_camShakeElapsed += g_dt;
        if (g_camShakeElapsed <= g_camShakeDuration) {
            amplitude = g_camShakeAmplitude * (g_camShakeDuration - g_camShakeElapsed) / g_camShakeDuration;
            if (amplitude) {
                out->x = Rand_Bounded(amplitude << 1) - amplitude;
                out->z = Rand_Bounded(amplitude << 1) - amplitude;
                out->y = Rand_Bounded(amplitude << 1) - amplitude;
                return;
            }
        } else
            g_camShakeAmplitude = 0;
    }
    out->x = 0;
    out->y = 0;
    out->z = 0;
}
/* 0x55262a */
void Camera_StartShake(s16 amplitude, s32 duration)
{
    g_camShakeAmplitude = amplitude;
    g_camShakeDuration = duration;
    g_camShakeElapsed = 0;
}
/* 0x55264c */
void Camera_StopShake()
{
    g_camShakeAmplitude = 0;
    g_camShakeDuration = 0;
    g_camShakeElapsed = 0;
}
/* 0x55266e */
s16 Camera_StickAxisDeadzone(u8 raw)
{
    s16 value = raw - 128;
    if (value >= 0) {
        if (value <= 40)
            value = 0;
        else
            value -= 40;
    } else {
        if (value >= -40)
            value = 0;
        else
            value += 40;
    }
    return value;
}
/* 0x5526c9 */
/* BYTES(slot-scope, inferred): the nested block(s) only order the frame: their locals are allocated after the enclosing scope's */
void Camera_OrbitInstance_Unused(Camera *cam, const u16 *inst)
{
    Vec3s angles;
    {
        Vec4i result;
        {
            Mat34s matrix;
            {
                Vec4i offset;
                offset.x = 0;
                offset.y = 0;
                offset.z = cam->dist;
                SetVectorPad(&offset, 0);
                angles.x = 0x1000 - cam->rot.x;
                angles.y = (cam->rot.y + 0x800) & 0xfff;
                angles.z = 0;
                Mat34s_FromEulerYXZ(&angles, &matrix);
                /* cast kept: the transform reads x, y, z of this padded Vec4i local */
                Mat34s_TransformTransposedVec3i(&matrix, (Vec3i *)&offset, &result);
                cam->pos.x = result.x;
                cam->pos.y = result.y;
                cam->pos.z = result.z;
                if (InstanceFlags(inst, INST_F_ANIMATED)) {
                    const u16 *instance = inst;
                    cam->pos.x += (s16)instance[4];
                    cam->pos.y += (s16)instance[5];
                    cam->pos.z += (s16)instance[6];
                    cam->pos.x += (s16)instance[18];
                    cam->pos.y += (s16)instance[19];
                    cam->pos.z += (s16)instance[20];
                }
                cam->rot.z = 0;
                Camera_BuildViewMatrix(cam, g_pZeroVec3s);
            }
        }
    }
}
/* 0x55285d */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; unused0, unused1 fill gaps */
void Camera_UpdateDebugFreeCam(Camera *cam, Pad *pad, Pad *mouse)
{
    struct Work {
        s32 alternateLength;
        Vec3s right;
        u16 unused0;
        s32 side, enabled;
        Vec3s front;
        u16 unused1;
        s32 time, up, length, forward;
    } w;
    w.enabled = !Map_IsOpen() && !(g_gameFlags & GF_BIT0);
    w.time = g_dt * 1000;
    w.front.x = cam->viewMatS.rot[6];
    w.front.y = cam->viewMatS.rot[7];
    w.front.z = cam->viewMatS.rot[8];
    w.right.x = cam->viewMatS.rot[0];
    w.right.y = cam->viewMatS.rot[1];
    w.right.z = cam->viewMatS.rot[2];
    w.side = w.forward = w.up = 0;
    if (mouse->cur.typeLen.type == PADTYPE_MOUSE) {
        switch (mouse->cur.buttons) {
            case (u16) ~(PAD_L2 | PAD_R2 | PAD_R1):
                w.forward = 256;
                break;
            case (u16) ~(PAD_L2 | PAD_R2 | PAD_L1):
                w.forward = -256;
                break;
        }
        cam->rot.y -= (s8)mouse->cur.rightX;
        cam->rot.x += (s8)mouse->cur.rightY;
    }
    if (w.enabled) {
        if (!(pad->cur.buttons & ~(u16)~PAD_L1))
            w.forward = 256;
        else if (!(pad->cur.buttons & ~(u16)~PAD_L2))
            w.forward = -256;
    }
    w.forward = w.forward * w.time >> 20;
    cam->pos.x += (s16)(w.front.x * w.forward >> 12);
    cam->pos.y += (s16)(w.front.y * w.forward >> 12);
    cam->pos.z += (s16)(w.front.z * w.forward >> 12);
    w.side = w.forward = w.up = 0;
    if (pad->cur.typeLen.type == PADTYPE_ANALOG) {
        Pad_AnalogToStick(pad->cur.leftX, pad->cur.leftY, &w.side, &w.forward);
        w.forward = -w.forward;
    }
    if (w.enabled) {
        if (!(pad->cur.buttons & ~(u16)~PAD_LEFT))
            w.side = -256;
        else if (!(pad->cur.buttons & ~(u16)~PAD_RIGHT))
            w.side = 256;
        if (!(pad->cur.buttons & ~(u16)~PAD_UP))
            w.forward = 256;
        else if (!(pad->cur.buttons & ~(u16)~PAD_DOWN))
            w.forward = -256;
        if (!(pad->cur.buttons & ~(u16)~PAD_R1))
            w.up = 256;
        else if (!(pad->cur.buttons & ~(u16)~PAD_R2))
            w.up = -256;
    }
    w.length = (s32)sqrt((double)w.front.x * w.front.x + (double)(w.front.z * w.front.z));
    if (w.length > 0) {
        w.front.x = (w.front.x << 12) / w.length;
        w.front.z = (w.front.z << 12) / w.length;
    } else {
        w.front.x = -cam->viewMatS.rot[3];
        w.front.z = -cam->viewMatS.rot[5];
        w.alternateLength = (s32)sqrt((double)w.front.x * w.front.x + (double)(w.front.z * w.front.z));
        if (w.alternateLength > 0) {
            w.front.x = (w.front.x << 12) / w.alternateLength;
            w.front.z = (w.front.z << 12) / w.alternateLength;
        }
    }
    w.length = (s32)sqrt((double)w.right.x * w.right.x + (double)(w.right.z * w.right.z));
    if (w.length > 0) {
        w.right.x = (w.right.x << 12) / w.length;
        w.right.z = (w.right.z << 12) / w.length;
    }
    w.side = w.side * w.time >> 20;
    w.forward = w.forward * w.time >> 20;
    w.up = w.up * w.time >> 20;
    cam->pos.x += (s16)((w.right.x * w.side + w.front.x * w.forward) >> 12);
    cam->pos.y -= (s16)w.up;
    cam->pos.z += (s16)((w.right.z * w.side + w.front.z * w.forward) >> 12);
    w.side = w.forward = 0;
    if (pad->cur.typeLen.type == PADTYPE_ANALOG)
        Pad_StickToDeadzonedAxes(pad->cur.rightX, pad->cur.rightY, &w.side, &w.forward);
    if (w.enabled) {
        if (!(pad->cur.buttons & ~(u16)~PAD_SQUARE))
            w.side = -256;
        else if (!(pad->cur.buttons & ~(u16)~PAD_CIRCLE))
            w.side += 256;
        if (!(pad->cur.buttons & ~(u16)~PAD_TRIANGLE))
            w.forward = -256;
        else if (!(pad->cur.buttons & ~(u16)~PAD_CROSS))
            w.forward += 256;
    }
    w.time = g_camModeParams[0].yawSpeed * g_dt;
    w.side = w.side * w.time >> 20;
    w.forward = w.forward * w.time >> 20;
    cam->rot.x = (cam->rot.x + w.forward) & 0xfff;
    cam->rot.y = (cam->rot.y - w.side) & 0xfff;
    cam->rot.z = 0;
    Camera_BuildViewMatrix(cam, g_pZeroVec3s);
}
/* 0x552e22 */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; unused1b2, unused1aa, unused38, unused2e, unused22, unused1a, unused12 fill gaps */
u16 Camera_SweepEyeBox(const Vec3s *eye, const s16 *boxMinMax, Vec3s *ioDelta, const Vec3s *target, Vec3s *outTried)
{
    /* Scratch order follows the observed frame; no replacement object layout. */
    struct Work {
        s32 floorProjection, wallProjection;
        Vec3s centroid;
        u16 unused1b2;
        Vec3s sideDelta;
        u16 unused1aa;
        s32 projection, normalCount;
        CameraSweepScratch *scratch;
        u16 unused19c, contactFlags;
        CollContact *contact;
        s32 index;
        u16 unused190, sideFlags;
        s32 height;
        CollBox box;
        CollContact contacts[16];
        u32 unused38;
        Vec3s remaining;
        u16 unused2e;
        u32 iterations;
        Vec3s right;
        u16 unused22;
        Vec3s step;
        u16 unused1a;
        Vec3s current;
        u16 unused12;
        s32 fraction, auxHeight;
        u16 unused8, allFlags;
        s32 count;
    } w;
    w.sideFlags = 0;
    w.right.x = g_camera.viewMatS.rot[0];
    w.right.y = g_camera.viewMatS.rot[1];
    w.right.z = g_camera.viewMatS.rot[2];
    outTried->x = 0;
    outTried->y = 0;
    outTried->z = 0;
    w.iterations = 4;
    w.remaining = *ioDelta;
    w.current = *eye;
    w.box.flags = 0;
    w.box.min.x = boxMinMax[0] + w.current.x;
    w.box.min.y = boxMinMax[1] + w.current.y;
    w.box.min.z = boxMinMax[2] + w.current.z;
    w.box.max.x = boxMinMax[4] + w.current.x;
    w.box.max.y = boxMinMax[5] + w.current.y;
    w.box.max.z = boxMinMax[6] + w.current.z;
    w.allFlags = 0;
    do {
        --w.iterations;
        w.step = w.remaining;
        while (ABS_VALUE(w.step.x) > 25 || ABS_VALUE(w.step.y) > 25 || ABS_VALUE(w.step.z) > 25) {
            w.step.x /= 2;
            w.step.y /= 2;
            w.step.z /= 2;
        }
        outTried->x += w.step.x;
        outTried->y += w.step.y;
        outTried->z += w.step.z;
        w.count = Collide_SweepBox(0, &w.box, &w.step, &w.fraction, &w.height, w.contacts, 0, CQ_STATIC_EXT,
                                   &w.auxHeight, 0, 0);
        if (!w.count) {
            w.current.x += w.step.x;
            w.current.y += w.step.y;
            w.current.z += w.step.z;
            w.remaining.x -= w.step.x;
            w.remaining.y -= w.step.y;
            w.remaining.z -= w.step.z;
        } else {
            /* cast kept: one scratch buffer, laid out by each user */
            w.scratch = (CameraSweepScratch *)g_sharedScratch;
            if (w.fraction > 0) {
                w.scratch->corner.x = w.step.x * w.fraction / 4096;
                w.scratch->corner.y = w.step.y * w.fraction / 4096;
                w.scratch->corner.z = w.step.z * w.fraction / 4096;
                w.current.x += w.scratch->corner.x;
                w.current.y += w.scratch->corner.y;
                w.current.z += w.scratch->corner.z;
                w.remaining.x -= w.scratch->corner.x;
                w.remaining.y -= w.scratch->corner.y;
                w.remaining.z -= w.scratch->corner.z;
                w.box.min.x += w.scratch->corner.x;
                w.box.min.y += w.scratch->corner.y;
                w.box.min.z += w.scratch->corner.z;
                w.box.max.x += w.scratch->corner.x;
                w.box.max.y += w.scratch->corner.y;
                w.box.max.z += w.scratch->corner.z;
            }
            w.contactFlags = 0;
            for (w.index = 0; w.index < w.count; ++w.index) {
                w.contact = &w.contacts[w.index];
                if (w.contact->normal.y < -0xb54) {
                    if (w.contact->normal.x > 0)
                        w.scratch->corner.x = w.box.min.x;
                    else
                        w.scratch->corner.x = w.box.max.x;
                    if (w.contact->normal.y > 0)
                        w.scratch->corner.y = w.box.min.y;
                    else
                        w.scratch->corner.y = w.box.max.y;
                    if (w.contact->normal.z > 0)
                        w.scratch->corner.z = w.box.min.z;
                    else
                        w.scratch->corner.z = w.box.max.z;
                    w.scratch->corner.x -= w.contact->point.x;
                    w.scratch->corner.y -= w.contact->point.y;
                    w.scratch->corner.z -= w.contact->point.z;
                    if (DOT(w.scratch->corner, w.contact->normal) >= -0x800)
                        w.scratch->classes[w.index] = COLL_FLOOR;
                    else
                        w.scratch->classes[w.index] = COLL_FLOOR_EDGE;
                } else
                    w.scratch->classes[w.index] = COLL_WALL;
                w.contactFlags |= w.scratch->classes[w.index];
            }
            w.allFlags |= w.contactFlags;
            if ((w.contactFlags & (COLL_FLOOR | COLL_FLOOR_EDGE)) && w.box.max.y - w.height <= 20)
                w.remaining.y = w.height - w.box.max.y - 1;
            else {
                w.normalCount = 0;
                w.scratch->movement.x = w.step.x << 6;
                w.scratch->movement.y = w.step.y << 6;
                w.scratch->movement.z = w.step.z << 6;
                w.scratch->normalSum.x = 0;
                w.scratch->normalSum.y = 0;
                w.scratch->normalSum.z = 0;
                if ((w.contactFlags & COLL_WALL) && !(w.contactFlags & COLL_FLOOR)) {
                    for (w.index = 0; w.index < w.count; ++w.index)
                        if (w.scratch->classes[w.index] == COLL_WALL) {
                            w.contact = &w.contacts[w.index];
                            w.remaining.x = w.scratch->movement.x / 64;
                            w.remaining.y = w.scratch->movement.y / 64;
                            w.remaining.z = w.scratch->movement.z / 64;
                            w.scratch->predicted.x = w.current.x + w.remaining.x;
                            w.scratch->predicted.y = w.current.y + w.remaining.y;
                            w.scratch->predicted.z = w.current.z + w.remaining.z;
                            w.scratch->toTarget.x = target->x - w.scratch->predicted.x;
                            w.scratch->toTarget.y = target->y - w.scratch->predicted.y;
                            w.scratch->toTarget.z = target->z - w.scratch->predicted.z;
                            w.scratch->toCurrent.x = w.current.x - w.scratch->predicted.x;
                            w.scratch->toCurrent.y = w.current.y - w.scratch->predicted.y;
                            w.scratch->toCurrent.z = w.current.z - w.scratch->predicted.z;
                            w.projection = DOT(w.contact->normal, w.scratch->toTarget) >> 1;
                            w.centroid.x =
                                (w.contact->triVerts[0] + w.contact->triVerts[3] + w.contact->triVerts[6]) / 3;
                            w.centroid.y =
                                (w.contact->triVerts[1] + w.contact->triVerts[4] + w.contact->triVerts[7]) / 3;
                            w.centroid.z =
                                (w.contact->triVerts[2] + w.contact->triVerts[5] + w.contact->triVerts[8]) / 3;
                            w.centroid.y = -w.centroid.y;
                            w.centroid.z = -w.centroid.z;
                            w.sideDelta.x = w.centroid.x - g_camPos.x;
                            w.sideDelta.y = w.centroid.y - g_camPos.y;
                            w.sideDelta.z = w.centroid.z - g_camPos.z;
                            if (DOT(w.right, w.sideDelta) > 0)
                                w.sideFlags |= CAMSIDE_RIGHT;
                            else
                                w.sideFlags |= CAMSIDE_LEFT;
                            if (w.projection > 0)
                                w.projection = (DOT(w.contact->normal, w.scratch->toCurrent) << 5) / w.projection;
                            if (w.projection > 0 && w.projection < 64) {
                                w.scratch->movement.x =
                                    -(w.scratch->toCurrent.x << 6) + w.projection * w.scratch->toTarget.x;
                                w.scratch->movement.y =
                                    -(w.scratch->toCurrent.y << 6) + w.projection * w.scratch->toTarget.y;
                                w.scratch->movement.z =
                                    -(w.scratch->toCurrent.z << 6) + w.projection * w.scratch->toTarget.z;
                            } else {
                                w.wallProjection = DOT(w.contact->normal, w.scratch->movement) >> 6;
                                SUBTRACT_NORMAL(w.wallProjection);
                            }
                            ADD_NORMAL();
                        }
                } else {
                    for (w.index = 0; w.index < w.count; ++w.index)
                        if (w.scratch->classes[w.index] == COLL_FLOOR) {
                            w.contact = &w.contacts[w.index];
                            w.floorProjection = DOT(w.contact->normal, w.scratch->movement) >> 6;
                            SUBTRACT_NORMAL(w.floorProjection);
                            ADD_NORMAL();
                        }
                }
                if (w.normalCount > 0) {
                    w.scratch->normalSum.x /= w.normalCount << 5;
                    w.scratch->normalSum.y /= w.normalCount << 5;
                    w.scratch->normalSum.z /= w.normalCount << 5;
                    w.scratch->movement.x += w.scratch->normalSum.x;
                    w.scratch->movement.y += w.scratch->normalSum.y;
                    w.scratch->movement.z += w.scratch->normalSum.z;
                }
                w.remaining.x = w.scratch->movement.x / 64;
                w.remaining.y = w.scratch->movement.y / 64;
                w.remaining.z = w.scratch->movement.z / 64;
            }
        }
    } while ((w.remaining.x | w.remaining.y | w.remaining.z) && w.iterations > 0);
    ioDelta->x = w.current.x - eye->x;
    ioDelta->y = w.current.y - eye->y;
    ioDelta->z = w.current.z - eye->z;
    if (w.allFlags && w.sideFlags && w.sideFlags != (CAMSIDE_RIGHT | CAMSIDE_LEFT))
        w.allFlags |= (u16)(w.sideFlags << 10);
    return w.allFlags;
}
/* 0x553d78 */
/* BYTES(slot-scope, inferred): the nested block(s) only order the frame: their locals are allocated after the enclosing scope's */
s32 Camera_ProbeLedgeAhead(const Vec3s *angles)
{
    s32 clear = 0;
    {
        s32 distance;
        {
            Vec3s probeAngles;
            {
                Mat34s matrix;
                {
                    CollRay ray;
                    if (g_camReqFlags.ledgeProbe) {
                        ray.origin.x = g_camReqTarget.x;
                        ray.origin.y = g_camReqTarget.y - 60;
                        ray.origin.z = g_camReqTarget.z;
                        probeAngles.x = 0x2aa;
                        probeAngles.y = angles->y;
                        probeAngles.z = angles->z;
                        Mat34s_FromEulerYXZ(&probeAngles, &matrix);
                        ray.dir.x = matrix.rot[6];
                        ray.dir.y = matrix.rot[7];
                        ray.dir.z = matrix.rot[8];
                        ray.maxDist = 276;
                        ray.end.x = ray.dir.x * ray.maxDist >> 12;
                        ray.end.y = ray.dir.y * ray.maxDist >> 12;
                        ray.end.z = ray.dir.z * ray.maxDist >> 12;
                        ray.end.x += ray.origin.x;
                        ray.end.y += ray.origin.y;
                        ray.end.z += ray.origin.z;
                        distance = Collide_RayCastStatic(&ray, 0);
                        if (distance >= 276)
                            clear = 1;
                    }
                    return clear;
                }
            }
        }
    }
}
/* 0x553e86 */
void Camera_UpdateLedgeLook(Camera *cam)
{
    Vec3s angles;
    angles.x = cam->rot.x;
    angles.z = cam->rot.z;
    if (g_camSettings & CAMSET_LEDGE_PROBE_CAMYAW)
        angles.y = cam->rot.y;
    else
        angles.y = g_camReqRot.y;
    switch (g_camLedgeLookState) {
        case LEDGE_NONE:
            if (Camera_ProbeLedgeAhead(&angles))
                g_camLedgeLookState = LEDGE_LOOKING;
            break;
        case LEDGE_LOOKING:
            if (!Camera_ProbeLedgeAhead(&angles))
                g_camLedgeLookState = LEDGE_BLOCKED_1;
            break;
        case LEDGE_BLOCKED_1:
            if (Camera_ProbeLedgeAhead(&angles))
                g_camLedgeLookState = LEDGE_LOOKING;
            else
                g_camLedgeLookState = LEDGE_BLOCKED_2;
            break;
        case LEDGE_BLOCKED_2:
            angles.x = (cam->rot.x - 0xaa) & 0xfff;
            if (angles.x < g_camMinPitch)
                angles.x = g_camMinPitch;
            if (Camera_ProbeLedgeAhead(&angles))
                g_camLedgeLookState = LEDGE_BLOCKED_1;
            else
                g_camLedgeLookState = LEDGE_NONE;
            break;
    }
}
/* 0x553fad */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; unused fill gaps */
/* BYTES(slot-group): locals grouped in w for the exact original slots: high distance -2, angle -4, outside -8, low distance -a */
s32 Camera_ClampAngleToArc(s16 *angle, s16 low, s16 high)
{
    /* Exact original local storage: high distance -2, angle -4, outside -8, low distance -a. */
    struct {
        u16 unused;
        s16 lowDistance;
        s32 outside;
        s16 value, highDistance;
    } w;
    w.value = *angle;
    if (low <= high)
        w.outside = w.value < low || w.value > high;
    else
        w.outside = w.value < low && w.value > high;
    if (w.outside) {
        w.lowDistance = ABS_VALUE(ANGLE_DELTA(low, w.value));
        w.highDistance = ABS_VALUE(ANGLE_DELTA(high, w.value));
        if (w.lowDistance <= w.highDistance)
            *angle = low;
        else
            *angle = high;
    }
    return w.outside;
}
/* 0x55414c */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
void Camera_ApplyRestriction(Vec3s *angles, s32 *speeds, s16 *distance, s16 *focal, const CamRestrict *restriction)
{
    struct Work {
        s16 unused, direct;
        s32 changed;
        CamModeParams *mode;
        s16 unused2, mirrorHigh, mirrorLow, mirrored;
    } w;
    w.mode = g_camModeParams + g_camMode;
    if (Camera_ClampAngleToArc(&angles->x, restriction->pitchMin, restriction->pitchMax) && restriction->pitchSpeed)
        speeds[0] = restriction->pitchSpeed;
    if (restriction->flags.mirrorYaw) {
        w.mirrored = angles->y;
        w.direct = w.mirrored;
        w.mirrorLow = (restriction->yawMin + 0x800) & 0xfff;
        w.mirrorHigh = (restriction->yawMax + 0x800) & 0xfff;
        w.changed = Camera_ClampAngleToArc(&w.direct, restriction->yawMin, restriction->yawMax);
        w.changed &= Camera_ClampAngleToArc(&w.mirrored, w.mirrorLow, w.mirrorHigh);
        if (w.changed) {
            if (ABS_VALUE(ANGLE_DELTA(w.direct, angles->y)) <= ABS_VALUE(ANGLE_DELTA(w.mirrored, angles->y)))
                angles->y = w.direct;
            else
                angles->y = w.mirrored;
        }
    } else
        w.changed = Camera_ClampAngleToArc(&angles->y, restriction->yawMin, restriction->yawMax);
    if (w.changed) {
        if (restriction->yawSpeed)
            speeds[1] = restriction->yawSpeed;
        else
            speeds[1] = w.mode->yawSpeed;
    }
    if (Camera_ClampAngleToArc(&angles->z, restriction->rollMin, restriction->rollMax) && restriction->rollSpeed)
        speeds[2] = restriction->rollSpeed;
    if (*distance < restriction->distMin)
        *distance = restriction->distMin;
    else if (*distance > restriction->distMax)
        *distance = restriction->distMax;
    if (restriction->focal)
        *focal = restriction->focal;
}
/* 0x55442a */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
void Camera_SmoothToDesired(Camera *cam, Vec3s *angles, const s32 *speeds, s16 *distance, s16 focal,
                            const Vec3s *aimOffset, s32 onTrajectory)
{
    struct Work {
        s32 pitchAccel, yawAccel, yawDecel;
        CamModeParams *mode;
        CamRestrict *restriction;
        s32 shift, rate;
    } w;
    w.mode = g_camModeParams + g_camMode;
    w.restriction = g_camRestriction;
    w.shift = 0;
    if (g_camFlags.smooth && !g_camFlags.snap && !g_camStateFlags.bits.cut) {
        Vec3s_ApproachPoint(&g_camAimOffset, aimOffset, &g_camAimOffsetVel, 450, 300, 300);
        w.pitchAccel = w.mode->pitchRollAccel;
        w.yawAccel = w.mode->yawAccel;
        w.yawDecel = w.mode->yawDecel;
        if (onTrajectory) {
            w.pitchAccel <<= 1;
            w.yawAccel <<= 1;
            w.yawDecel <<= 1;
        }
        if (g_camMode == CAM_LOOK) {
            w.shift = 1;
            w.pitchAccel <<= w.shift;
            w.yawAccel <<= w.shift;
            w.yawDecel <<= w.shift;
        }
        if (g_camMode == CAM_DIRECTED && g_camStateFlags.bits.hit) {
            angles->x = Math_ApproachAngle(cam->rot.x, angles->x, &g_camPitchVel, speeds[0] >> 7, 2, 2, 0);
            angles->y = Math_ApproachAngle(cam->rot.y, angles->y, &g_camYawVel, speeds[1] >> 7, 2, 2, 0);
            angles->z = Math_ApproachAngle(cam->rot.z, angles->z, &g_camRollVel, speeds[2] >> 7, 2, 2, 0);
        } else {
            angles->x = Math_ApproachAngle(cam->rot.x, angles->x, &g_camPitchVel, speeds[0] << w.shift, w.pitchAccel,
                                           w.pitchAccel, 1);
            angles->y = Math_ApproachAngle(cam->rot.y, angles->y, &g_camYawVel, speeds[1] << w.shift, w.yawAccel,
                                           w.yawDecel, 1);
            angles->z = Math_ApproachAngle(cam->rot.z, angles->z, &g_camRollVel, speeds[2] << w.shift, w.pitchAccel,
                                           w.pitchAccel, 1);
        }
        if (angles->x > 0x800) {
            if (angles->x < 0xc00)
                angles->x = 0xc00;
        } else if (angles->x > 0x400)
            angles->x = 0x400;
        w.rate = ABS_VALUE(*distance - cam->dist) + 500;
        g_camFlags.tooFar = cam->dist >= *distance * 2;
        if (g_camMode == CAM_LOOK)
            *distance = Math_ApproachValue(cam->dist, *distance, &g_camDistVel, w.rate << w.shift, 5000 << w.shift,
                                           5000 << w.shift);
        else if (g_camMode == CAM_DIRECTED ||
                 (cam->dist < *distance && (!w.restriction || !w.restriction->trajectory))) {
            if (g_camStateFlags.bits.hit && g_camMode == CAM_DIRECTED) {
                if (g_camFlags.tooFar)
                    *distance = cam->dist - 1;
                else
                    *distance = Math_ApproachValue(cam->dist, *distance, &g_camDistVel, w.rate / 2, 0, 0);
            } else
                *distance = Math_ApproachValue(cam->dist, *distance, &g_camDistVel, w.rate / 2, 2500, 2500);
        } else
            *distance = Math_ApproachValue(cam->dist, *distance, &g_camDistVel, w.rate, 5000, 5000);
        if (*distance < 0)
            *distance = 0;
        g_camFocal = Math_StepTowards(g_camFocal, focal, g_camMode == CAM_SAM_CHASE ? 50 : 300);
    } else {
        g_camAimOffset = g_camReqAimOffset;
        g_camFlags.smooth = 1;
        g_camFlags.tooFar = 0;
        g_camFocal = focal;
    }
    g_camStateFlags.bits.cut = 0;
    g_screen.SetProjection(g_camFocal);
}
/* 0x55497d */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; unused0, unused1 fill gaps */
void Camera_ClampToTrajectory(Vec3s *position, Trajectory *trajectory, s32 axis)
{
    struct Work {
        Vec3s fromEye;
        u16 unused0;
        s32 nearDistance;
        s16 candidate, segment;
        const Vec3s *base;
        s32 otherDistance;
        Vec3s delta;
        u16 unused1;
        s32 denominator, sign;
        const Vec3s *next;
        s32 t;
    } w;
    if (g_camTrajectory != trajectory) {
        g_camTrajectory = trajectory;
        w.nearDistance = Vec3s_DistSq(position, &trajectory->pts[0]);
        w.otherDistance = Vec3s_DistSq(position, &trajectory->pts[trajectory->count - 1]);
        if (w.nearDistance <= w.otherDistance)
            w.segment = 0;
        else
            w.segment = trajectory->count - 1;
        position->x = trajectory->pts[w.segment].x;
        position->y = trajectory->pts[w.segment].y;
        position->z = trajectory->pts[w.segment].z;
    } else {
        w.t = 0;
        w.segment = g_camTrajSegment;
        w.base = &trajectory->pts[w.segment];
        w.candidate = w.segment;
        switch (axis) {
            case 0:
                w.nearDistance = 0x7fffffff;
                if (w.segment > 0) {
                    w.nearDistance = Vec3s_DistSq(position, &trajectory->pts[w.segment - 1]);
                    w.candidate = w.segment - 1;
                }
                if (w.segment < trajectory->count - 1) {
                    w.otherDistance = Vec3s_DistSq(position, &trajectory->pts[w.segment + 1]);
                    if (w.otherDistance < w.nearDistance) {
                        w.nearDistance = w.otherDistance;
                        w.candidate = w.segment + 1;
                    }
                }
                if (w.candidate != w.segment) {
                    w.next = &trajectory->pts[w.candidate];
                    w.fromEye.x = position->x - w.base->x;
                    w.fromEye.y = position->y - w.base->y;
                    w.fromEye.z = position->z - w.base->z;
                    w.delta.x = w.next->x - w.base->x;
                    w.delta.y = w.next->y - w.base->y;
                    w.delta.z = w.next->z - w.base->z;
                    w.denominator = (w.delta.x * w.delta.x + w.delta.y * w.delta.y + w.delta.z * w.delta.z) >> 6;
                    if (!w.denominator)
                        w.t = 0;
                    else {
                        w.t = w.fromEye.x * w.delta.x + w.fromEye.y * w.delta.y + w.fromEye.z * w.delta.z;
                        w.t = (w.t << 6) / w.denominator;
                    }
                }
                break;
            case 1:
                w.sign = (position->x - w.base->x) >> 31;
                if (w.segment > 0 && ((trajectory->pts[w.segment - 1].x - w.base->x) >> 31) == w.sign)
                    w.candidate = w.segment - 1;
                if (w.segment < trajectory->count - 1 &&
                    ((trajectory->pts[w.segment + 1].x - w.base->x) >> 31) == w.sign)
                    w.candidate = w.segment + 1;
                if (w.candidate != w.segment) {
                    w.next = &trajectory->pts[w.candidate];
                    w.delta.x = w.next->x - w.base->x;
                    w.delta.y = w.next->y - w.base->y;
                    w.delta.z = w.next->z - w.base->z;
                    if (w.delta.x)
                        w.t = ((position->x - w.base->x) << 12) / w.delta.x;
                }
                break;
            case 2:
                w.sign = (position->z - w.base->z) >> 31;
                if (w.segment > 0 && ((trajectory->pts[w.segment - 1].z - w.base->z) >> 31) == w.sign)
                    w.candidate = w.segment - 1;
                if (w.segment < trajectory->count - 1 &&
                    ((trajectory->pts[w.segment + 1].z - w.base->z) >> 31) == w.sign)
                    w.candidate = w.segment + 1;
                if (w.candidate != w.segment) {
                    w.next = &trajectory->pts[w.candidate];
                    w.delta.x = w.next->x - w.base->x;
                    w.delta.y = w.next->y - w.base->y;
                    w.delta.z = w.next->z - w.base->z;
                    if (w.delta.z)
                        w.t = ((position->z - w.base->z) << 12) / w.delta.z;
                }
                break;
        }
        if (w.t <= 0)
            *position = *w.base;
        else {
            if (w.t >= 4096) {
                w.t = 4096;
                w.segment = w.candidate;
            }
            position->x = w.base->x + (w.delta.x * w.t >> 12);
            position->y = w.base->y + (w.delta.y * w.t >> 12);
            position->z = w.base->z + (w.delta.z * w.t >> 12);
        }
    }
    g_camTrajectory = trajectory;
    g_camTrajSegment = w.segment;
}
/* 0x554eba */
/* BYTES(slot-scope, inferred): the nested block(s) only order the frame: their locals are allocated after the enclosing scope's */
void Camera_FollowTrajectory(Trajectory *trajectory, Vec3s *angles, s16 *distance, s32 *speeds, s32 snapYaw)
{
    Vec3s target;
    {
        Vec3s eye;
        {
            s32 axis;
            target.x = g_camReqTarget.x + g_camAimOffset.x;
            target.y = g_camReqTarget.y + g_camAimOffset.y;
            target.z = g_camReqTarget.z + g_camAimOffset.z;
            Vec3s_OffsetAlongAngles(&eye, angles, *distance, &target);
            if (snapYaw) {
                if (angles->y % 0x800 == 0)
                    axis = 1;
                else
                    axis = 2;
            } else
                axis = 0;
            Camera_ClampToTrajectory(&eye, trajectory, axis);
            Camera_EyeToAngles(&eye, angles, distance, &target);
            speeds[0] = speeds[1];
        }
    }
}
/* 0x554f8a */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py) */
void Vec3s_OffsetAlongAngles(Vec3s *out, const Vec3s *angles, s16 distance, const Vec3s *target)
{
    Mat34s matrix_6;
    Vec4i vector_8;
    Vec3s rotation_29;
    vector_8.x = 0;
    vector_8.y = 0;
    vector_8.z = distance;
    SetVectorPad(&vector_8, 0);
    rotation_29.x = (-angles->x) & 0xfff;
    rotation_29.y = (angles->y + 0x800) & 0xfff;
    rotation_29.z = angles->z;
    Mat34s_FromEulerYXZ(&rotation_29, &matrix_6);
    /* cast kept: vector_8 is a padded Vec4i local (SetVectorPad writes its w) whose x, y, z are read */
    Mat34s_TransformTransposedVec3i(&matrix_6, (Vec3i *)&vector_8, &vector_8);
    out->x = vector_8.x + target->x;
    out->y = vector_8.y + target->y;
    out->z = vector_8.z + target->z;
}
/* 0x55503c */
/* BYTES(slot-scope, inferred): the nested block(s) only order the frame: their locals are allocated after the enclosing scope's */
void Camera_EyeToAngles(const Vec3s *eye, Vec3s *angles, s16 *distance, const Vec3s *target)
{
    s16 pitchDelta;
    {
        Vec3i delta;
        {
            Vec3i squared;
            delta.x = target->x - eye->x;
            delta.y = target->y - eye->y;
            delta.z = target->z - eye->z;
            squared.x = delta.x * delta.x;
            squared.y = delta.y * delta.y;
            squared.z = delta.z * delta.z;
            angles->x = CameraAtanAngle(delta.y, (s32)sqrt((double)squared.x + (double)squared.z)) & 0xfff;
            pitchDelta = ANGLE_DELTA(0x400, angles->x);
            if (ABS_VALUE(pitchDelta) > 20)
                angles->y = CameraAtanAngle(-delta.x, delta.z) & 0xfff;
            *distance = (s16)sqrt((double)squared.x + (double)squared.y + (double)squared.z);
        }
    }
}
/* 0x5551e8 */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; unusedf6, unusedee, unused90, unused82, unused6c, unused5a, unused58, unused42, unused22, unused16, unuseda fill gaps */
void Camera_SolveCollision(Camera *cam, Vec3s *angles, s16 distance, s32 enabled, s16 maxVert)
{
    /* Observed scratch frame order, including separate boolean temporaries used
       at the three probe sites. None of these fields modifies Camera layout. */
    struct Work {
        s32 nearFinal, nearQuiet, nearProbe;
        Vec3s probeEye;
        u16 unusedf6;
        Vec3s probeAngles;
        u16 unusedee;
        s32 denominator;
        Vec3i toTarget;
        s32 dot;
        Vec3i targetSquares, tried32;
        s32 targetLength, ratio;
        Vec3i triedSquares;
        s32 movedLength, triedLength, badSlide, blocked;
        Vec3i rayFrom;
        u32 unused90;
        s32 oldZ;
        Vec3s tried;
        u16 unused82;
        s32 oldX;
        u16 unused7c, hitFlags;
        Vec3i rayTo;
        u32 unused6c;
        s16 unused68, baseYaw, clearDistance, angleOffset;
        Vec3s savedAngles;
        u16 unused5a;
        u8 unused58[3];
        s8 threshold;
        s32 closeEye, autoEnabled, restrictActive;
        Vec3s delta;
        u16 unused42;
        s32 foundClear, tooFar, recalculate, zSquared;
        CamRestrict *restriction;
        CamModeParams *mode;
        Vec3s shake;
        u16 unused22;
        s32 verticalBlocked;
        Vec3s eye;
        u16 unused16;
        s32 xSquared;
        Vec3s target;
        u16 unuseda;
        s32 modeDistance, snapYaw;
    } w;
    w.recalculate = 0;
    w.verticalBlocked = 0;
    w.foundClear = 0;
    w.restriction = g_camRestriction;
    w.mode = g_camModeParams + g_camMode;
    w.modeDistance = w.mode->dist;
    g_camFlags2.bits.overhead = 0;
    w.autoEnabled = enabled;
    w.restrictActive = w.restriction &&
                       (((g_camMode == CAM_FOLLOW || g_camMode == CAM_SAM_CHASE) && w.restriction->flags.modeFollow) ||
                        (g_camMode == CAM_ROBOT && w.restriction->flags.modeRobot) ||
                        (g_camMode == CAM_FOLLOW_RUN && w.restriction->flags.modeRun));
    w.snapYaw = g_camReqFlags.snapYaw || (w.restrictActive && w.restriction->flags.snapYaw);
    w.target.x = g_camReqTarget.x + g_camAimOffset.x;
    w.target.y = g_camReqTarget.y + g_camAimOffset.y;
    w.target.z = g_camReqTarget.z + g_camAimOffset.z;
    if (w.snapYaw || g_camMode == CAM_DIRECTED)
        w.autoEnabled = 0;
    w.xSquared = cam->pos.x - w.target.x;
    w.xSquared *= w.xSquared;
    w.zSquared = cam->pos.z - w.target.z;
    w.zSquared *= w.zSquared;
    if (w.restrictActive) {
        if (w.restriction->distMin && w.modeDistance < w.restriction->distMin)
            w.modeDistance = w.restriction->distMin;
        if (w.restriction->pitchMin >= 0x380 && w.restriction->pitchMax <= 0x480)
            g_camFlags2.bits.overhead = 1;
    }
    w.tooFar = w.zSquared + w.xSquared > (w.modeDistance << 2) * w.modeDistance;
    if (!w.autoEnabled || !enabled || g_camMode == CAM_DIRECTED || (w.restrictActive && w.restriction->trajectory))
        w.tooFar = 0;
    if (!enabled)
        g_camStateFlags.bits.hit = 0;
    if (!g_camStateFlags.bits.hit)
        w.tooFar = 0;
    if (w.tooFar) {
        g_camFlags.tooFarCut = 1;
        g_camBlockedCountA = g_camBlockedCountB = 16;
    }
    if (enabled && g_camMode != CAM_DIRECTED && (g_camSettings & CAMSET_COLLISION) &&
        (!(g_camFrameCounter & 15) || g_camStateFlags.bits.lookExitBlocked)) {
        w.closeEye = cam->dist < 200;
        w.threshold = w.closeEye ? 15 : 7;
        if (g_camBlockedCountB > w.threshold) {
            if (g_camStateFlags.bits.rotateNegative && g_camYawVel < 0) {
                g_camBlockPosNeg.x = cam->pos.x;
                g_camBlockPosNeg.y = cam->pos.y;
                g_camBlockPosNeg.z = cam->pos.z;
                g_camYawVel = 0;
            }
            if (g_camStateFlags.bits.rotatePositive && g_camYawVel > 0) {
                g_camBlockPosPos.x = cam->pos.x;
                g_camBlockPosPos.y = cam->pos.y;
                g_camBlockPosPos.z = cam->pos.z;
                g_camYawVel = 0;
            }
        }
        if ((g_camBlockedCountA > w.threshold || g_camStateFlags.bits.lookExitBlocked) &&
            (w.tooFar || g_camStateFlags.bits.lookExitBlocked ||
             (!g_camStateFlags.bits.rotateNegative && !g_camStateFlags.bits.rotatePositive))) {
            w.savedAngles = *angles;
            w.baseYaw = angles->y;
            g_camLosClearTime = g_gameTime;
            if (g_camStateFlags.bits.lookExitBlocked)
                distance = w.mode->dist;
            if (g_pWolf) {
                s16 wolfYaw;
                wolfYaw = g_pWolf->rot.y;
                angles->y = (-wolfYaw + 0x800) & 0xfff;
                w.baseYaw = angles->y;
            }
            w.angleOffset = 0;
            while (w.angleOffset <= 0x800) {
                angles->y = (w.baseYaw + w.angleOffset) & 0xfff;
                angles->x = 0;
                angles->z = 0;
                w.clearDistance = Camera_FindClearDistance(&w.target, angles, distance);
                if (w.clearDistance > 0) {
                    distance = w.clearDistance;
                    w.foundClear = 1;
                    g_camYawVel = 0;
                    g_camStateFlags.bits.cut = 1;
                    break;
                }
                *angles = w.savedAngles;
                if (w.angleOffset > 0)
                    w.angleOffset = -w.angleOffset;
                else
                    w.angleOffset = -w.angleOffset + 0x200;
            }
        }
        g_camBlockedCountA = g_camBlockedCountB = 0;
        g_camStateFlags.bits.hit = 0;
    } else if (!enabled)
        g_camBlockedCountA = g_camBlockedCountB = 0;
    g_camFrameCounter = (g_camFrameCounter + 1) & 31;
    Vec3s_OffsetAlongAngles(&w.eye, angles, distance, &w.target);
    if (w.eye.y > maxVert) {
        w.eye.y = maxVert;
        w.recalculate = 1;
    }
    if (w.autoEnabled) {
        if (g_camPitchAdjTimerMs > 0)
            g_camPitchAdjTimerMs -= (s16)g_dtMs;
        if (g_camPitchAdjState.state == PADJ_LOWER && g_camPitchAdjTimerMs <= 0)
            g_camPitchAdjState.state = PADJ_NONE;
    } else {
        g_camPitchAdjState.state = PADJ_NONE;
        g_camPitchAdjTimerMs = 0;
    }
    if (!enabled)
        g_camLosClearTime = g_gameTime;
    else if (!w.autoEnabled)
        g_camLosClearTime = g_gameTime;
    if ((u32)(g_gameTime - g_camLosClearTime) >= 0x3000)
        g_camBlockedCountA = g_camBlockedCountB = 16;
    if (enabled && (g_camSettings & CAMSET_COLLISION)) {
        w.delta.x = w.eye.x - cam->pos.x;
        w.delta.y = w.eye.y - cam->pos.y;
        w.delta.z = w.eye.z - cam->pos.z;
        w.oldX = w.delta.x;
        w.oldZ = w.delta.z;
        w.verticalBlocked = 0;
        if (w.foundClear)
            w.hitFlags = 0;
        else if (g_camStateFlags.bits.skipSweep &&
                 ABS_VALUE(w.delta.x) + ABS_VALUE(w.delta.y) + ABS_VALUE(w.delta.z) > 50)
            w.hitFlags = 0;
        else
            w.hitFlags = Camera_SweepEyeBox(&cam->pos, &g_camEyeBoxMin.x, &w.delta, &w.target, &w.tried);
        if (w.hitFlags) {
            g_camStateFlags.bits.hit = 1;
            w.blocked =
                ABS_VALUE(w.tried.x) + ABS_VALUE(w.tried.z) > 15 && ABS_VALUE(w.delta.x) + ABS_VALUE(w.delta.z) < 5;
            if (g_camFlags2.bits.overhead)
                w.blocked = 0;
            g_camLastHitTarget = g_camReqTarget;
            if (g_camHitFrames < 127)
                ++g_camHitFrames;
            w.blocked |= g_camFlags.tooFarCut;
            w.verticalBlocked = w.eye.y - cam->pos.y > 15 && ABS_VALUE(w.delta.y) < 5;
            if (g_camAutoPitchRate | g_camAutoYawRate | g_camAutoRollRate) {
                if (w.autoEnabled && w.blocked && (!w.verticalBlocked || g_camPitchAdjState.state)) {
                    ++g_camBlockedCountA;
                    ++g_camBlockedCountB;
                }
            } else if (g_camTurnState != TURN_SIDE && (w.hitFlags & (CAMHIT_LEFT | CAMHIT_RIGHT)) &&
                       g_camMode != CAM_DIRECTED) {
                if (w.hitFlags & CAMHIT_LEFT)
                    g_camAutoYawRate = -g_camModeParams[0].yawSpeed;
                else
                    g_camAutoYawRate = g_camModeParams[0].yawSpeed;
                g_camTurnTimerMs = 500;
                g_camTurnState = TURN_SIDE;
            }
            w.badSlide = 0;
            w.tried32.x = w.tried.x;
            w.tried32.y = w.tried.y;
            w.tried32.z = w.tried.z;
            w.triedSquares.x = w.tried32.x * w.tried32.x;
            w.triedSquares.y = w.tried32.y * w.tried32.y;
            w.triedSquares.z = w.tried32.z * w.tried32.z;
            w.triedLength = w.triedSquares.x + w.triedSquares.z;
            w.movedLength = w.delta.x * w.delta.x + w.delta.z * w.delta.z;
            w.dot = w.delta.x * w.tried.x + w.delta.z * w.tried.z;
            if (w.dot > 0)
                w.dot *= w.dot << 3;
            else
                w.dot *= -(w.dot << 3);
            if (w.triedLength < 225)
                w.badSlide = 0;
            else if (w.movedLength < 25)
                w.badSlide = 1;
            else if (w.dot / (w.triedLength * w.movedLength) <= 1)
                w.badSlide = 1;
            w.triedLength += w.triedSquares.y;
            if (w.badSlide && (w.hitFlags & (CAMHIT_LEFT | CAMHIT_RIGHT)) && g_camTurnState == TURN_SIDE &&
                g_camTurnTimerMs < 250) {
                w.toTarget.x = (s16)(g_camReqTarget.x - cam->pos.x);
                w.toTarget.y = (s16)(g_camReqTarget.y - cam->pos.y);
                w.toTarget.z = (s16)(g_camReqTarget.z - cam->pos.z);
                w.targetSquares.x = w.toTarget.x * w.toTarget.x;
                w.targetSquares.y = w.toTarget.y * w.toTarget.y;
                w.targetSquares.z = w.toTarget.z * w.toTarget.z;
                w.targetLength = w.targetSquares.x + w.targetSquares.y + w.targetSquares.z;
                w.denominator = w.targetLength * w.triedLength;
                if (w.denominator) {
                    w.dot = w.toTarget.x * w.tried32.x + w.toTarget.y * w.tried32.y + w.toTarget.z * w.tried32.z;
                    if (w.dot > 0)
                        w.dot *= w.dot;
                    else
                        w.dot *= -w.dot;
                    w.ratio = (w.dot << 3) / w.denominator;
                    if (w.ratio >= 3) {
                        ++g_camBlockedCountA;
                        ++g_camBlockedCountB;
                    }
                }
            }
            w.eye.x = w.delta.x + cam->pos.x;
            w.eye.y = w.delta.y + cam->pos.y;
            w.eye.z = w.delta.z + cam->pos.z;
            *angles = cam->rot;
            distance = cam->dist;
            w.recalculate = 1;
        } else {
            w.eye.x = w.delta.x + cam->pos.x;
            w.eye.y = w.delta.y + cam->pos.y;
            w.eye.z = w.delta.z + cam->pos.z;
            w.recalculate = 1;
            g_camFlags.tooFarCut = 0;
            g_camZoomOut -= 10;
            if (g_camZoomOut < 0)
                g_camZoomOut = 0;
        }
        if (w.autoEnabled) {
            if (g_camTurnState == TURN_PROBE || g_camTurnState == TURN_TURNING) {
                if (g_camUnused43fc < 3000)
                    g_camUnused43fc = 3000;
                if (g_camProbeFrame.value) {
                    if (g_camTurnState == TURN_PROBE) {
                        w.probeAngles.x = angles->x;
                        w.probeAngles.z = angles->z;
                        w.probeAngles.y = (g_camProbeYaw + g_camProbeYawOffset) & 0xfff;
                    } else {
                        if (g_camPitchAdjState.state == PADJ_NONE ||
                            (g_camPitchAdjState.state == PADJ_LOWER && g_camPitchAdjTimerMs <= 0))
                            w.probeAngles.x = angles->x + 0x1c7;
                        else {
                            w.probeAngles.x = angles->x - 0x1c7;
                            if (w.probeAngles.x < 0)
                                w.probeAngles.x = 0;
                        }
                        w.probeAngles.y = angles->y;
                        w.probeAngles.z = angles->z;
                    }
                    Vec3s_OffsetAlongAngles(&w.probeEye, &w.probeAngles, distance, &w.target);
                    FILL_RAY();
                    if (g_camTurnState == TURN_PROBE) {
                        w.nearProbe = cam->dist < 200 && ABS_VALUE(g_camProbeYawOffset) < 0x600;
                        if (w.nearProbe || Collide_SegmentClearStatic(&w.rayFrom.x, &w.rayTo.x, 0) < 0x7fffffff) {
                            if (w.nearProbe)
                                g_camLosClearTime = g_gameTime;
                            g_camAutoRollRate = 0;
                            if (g_camProbeYawOffset > 0)
                                g_camProbeYawOffset = -g_camProbeYawOffset;
                            else {
                                g_camProbeYawOffset = (((g_dt * 30) / 135 << 12) / 360) - g_camProbeYawOffset;
                                if (g_camProbeYawOffset > 0x400)
                                    g_camTurnState = TURN_IDLE;
                            }
                        } else if (!g_camAutoYawRate) {
                            g_camAutoRollRate = 0;
                            g_camAutoYawRate = ANGLE_DELTA(w.probeAngles.y, cam->rot.y) > 0
                                                   ? g_camModeParams[0].yawSpeed << 1
                                                   : (-g_camModeParams[0].yawSpeed) << 1;
                            if (cam->dist < 400 || g_camZoomOut >= 50)
                                g_camAutoYawRate <<= 2;
                            g_camTurnState = TURN_TURNING;
                            g_camProbeYawOffset = 0x155;
                        } else
                            g_camTurnState = TURN_IDLE;
                    } else if (w.verticalBlocked &&
                               (g_camPitchAdjState.state == PADJ_NONE ||
                                (g_camPitchAdjState.state == PADJ_LOWER && g_camPitchAdjTimerMs <= 0)) &&
                               Collide_SegmentClearStatic(&w.rayFrom.x, &w.rayTo.x, 0) >= 0x7fffffff) {
                        g_camPitchAdjState.state = PADJ_RAISE;
                        g_camAutoPitchRate = g_camModeParams[0].pitchSpeed << 1;
                        g_camPitchAdjTimerMs = 1000;
                        g_camTurnState = TURN_IDLE;
                    } else if (!w.verticalBlocked && g_camPitchAdjState.state == PADJ_RAISE &&
                               g_camPitchAdjTimerMs <= 0 &&
                               Collide_SegmentClearStatic(&w.rayFrom.x, &w.rayTo.x, 0) >= 0x7fffffff) {
                        g_camAutoPitchRate = (-g_camModeParams[0].pitchSpeed) << 1;
                        g_camPitchAdjState.state = PADJ_LOWER;
                        g_camPitchAdjTimerMs = 10000;
                        g_camTurnState = TURN_IDLE;
                    } else
                        g_camTurnState = TURN_PROBE;
                } else {
                    w.probeAngles.x = angles->x;
                    w.probeAngles.y = angles->y;
                    w.probeAngles.z = angles->z;
                    if (g_camPitchAdjState.state == PADJ_RAISE && g_camPitchAdjTimerMs <= 0 && g_camFrameCounter == 0) {
                        w.probeAngles.x = angles->x - 0x1c7;
                        if (w.probeAngles.x < 0)
                            w.probeAngles.x = 0;
                        Vec3s_OffsetAlongAngles(&w.probeEye, &w.probeAngles, distance, &w.target);
                        FILL_RAY();
                        if (!w.verticalBlocked && g_camPitchAdjTimerMs <= 0 &&
                            Collide_SegmentClearStatic(&w.rayFrom.x, &w.rayTo.x, 0) >= 0x7fffffff) {
                            g_camAutoPitchRate = (-g_camModeParams[0].pitchSpeed) << 1;
                            g_camPitchAdjState.state = PADJ_LOWER;
                            g_camPitchAdjTimerMs = 10000;
                            g_camTurnState = TURN_IDLE;
                            g_camLosClearTime = g_gameTime;
                        }
                        Vec3s_OffsetAlongAngles(&w.probeEye, &w.probeAngles, distance, &w.target);
                    } else {
                        w.nearQuiet = cam->dist < 200;
                        if (!w.nearQuiet) {
                            Vec3s_OffsetAlongAngles(&w.probeEye, &w.probeAngles, distance, &w.target);
                            FILL_RAY();
                            if (Collide_SegmentClearStatic(&w.rayFrom.x, &w.rayTo.x, 0) >= 0x7fffffff) {
                                if (g_camPitchAdjState.state != PADJ_LOWER)
                                    g_camAutoPitchRate = 0;
                                g_camAutoYawRate = g_camAutoRollRate = 0;
                                g_camTurnState = TURN_IDLE;
                                g_camLosClearTime = g_gameTime;
                            }
                        } else
                            g_camLosClearTime = g_gameTime;
                    }
                }
            } else {
                w.nearFinal = cam->dist < 200;
                if (w.nearFinal && (u32)(g_gameTime - g_camLookTime) > 0x2000) {
                    g_camBlockedCountA = 0;
                    g_camBlockedCountB = 0;
                    g_camPitchAdjState.state = PADJ_RAISE;
                    g_camAutoPitchRate = g_camModeParams[0].pitchSpeed << 3;
                    g_camPitchAdjTimerMs = 1000;
                    g_camTurnState = TURN_PROBE;
                    g_camProbeFrame.value = 1;
                    g_camProbeYaw = angles->y;
                    g_camProbeYawOffset = 0x600;
                    SAVE_PROBE_START();
                    g_camLosClearTime = g_gameTime;
                    if (g_camZoomOut < 100)
                        g_camZoomOut = 100;
                }
                w.probeAngles.x = angles->x;
                w.probeAngles.y = angles->y;
                w.probeAngles.z = angles->z;
                if (g_camPitchAdjState.state == PADJ_RAISE && g_camPitchAdjTimerMs <= 0 && g_camFrameCounter == 0) {
                    if (g_camUnused43fc < 3000)
                        g_camUnused43fc = 3000;
                    w.probeAngles.x = angles->x - 0x1c7;
                    if (w.probeAngles.x < 0)
                        w.probeAngles.x = 0;
                    Vec3s_OffsetAlongAngles(&w.probeEye, &w.probeAngles, distance, &w.target);
                    FILL_RAY();
                    if (!w.verticalBlocked && g_camPitchAdjTimerMs <= 0 &&
                        Collide_SegmentClearStatic(&w.rayFrom.x, &w.rayTo.x, 0) >= 0x7fffffff) {
                        g_camAutoPitchRate = (-g_camModeParams[0].pitchSpeed) << 1;
                        g_camPitchAdjState.state = PADJ_LOWER;
                        g_camPitchAdjTimerMs = 10000;
                        g_camTurnState = TURN_IDLE;
                        g_camLosClearTime = g_gameTime;
                    }
                    Vec3s_OffsetAlongAngles(&w.probeEye, &w.probeAngles, distance, &w.target);
                } else {
                    Vec3s_OffsetAlongAngles(&w.probeEye, &w.probeAngles, distance, &w.target);
                    FILL_RAY();
                    if (Collide_SegmentClearStatic(&w.rayFrom.x, &w.rayTo.x, 0) >= 0x7fffffff && !w.nearFinal &&
                        g_camTurnState != TURN_SIDE) {
                        if (g_camPitchAdjState.state != PADJ_LOWER)
                            g_camAutoPitchRate = 0;
                        g_camLosClearTime = g_gameTime;
                        g_camAutoRollRate = 0;
                        g_camAutoYawRate = 0;
                        g_camTurnState = TURN_IDLE;
                    } else {
                        if (g_camUnused43fc < 3000)
                            g_camUnused43fc = 3000;
                        if (g_camTurnState == TURN_IDLE) {
                            g_camTurnState = TURN_PROBE;
                            g_camProbeFrame.value = 1;
                            g_camProbeYaw = angles->y;
                            g_camProbeYawOffset = 0;
                            SAVE_PROBE_START();
                        } else if (g_camTurnState == TURN_SIDE) {
                            if (g_camUnused43fc < 3000)
                                g_camUnused43fc = 3000;
                            g_camTurnTimerMs -= (s16)g_dtMs;
                            if (g_camTurnTimerMs < 0)
                                g_camTurnState = TURN_IDLE;
                        } else if (g_camTurnState == TURN_PROBE) {
                            if (g_camUnused43fc < 3000)
                                g_camUnused43fc = 3000;
                            g_camTurnTimerMs -= (s16)g_dtMs;
                            if (g_camTurnTimerMs < 0)
                                g_camTurnState = TURN_IDLE;
                        }
                    }
                }
            }
            if (g_camTurnState == TURN_PROBE || g_camTurnState == TURN_TURNING)
                g_camProbeFrame.value = (g_camProbeFrame.value + 1) & 7;
        }
    } else if (g_camUnused43fc < 3000)
        g_camUnused43fc = 3000;
    if (!w.autoEnabled || !enabled || !(g_camSettings & CAMSET_COLLISION)) {
        g_camAutoPitchRate = 0;
        g_camAutoYawRate = 0;
        g_camAutoRollRate = 0;
    }
    if (w.recalculate)
        Camera_EyeToAngles(&w.eye, angles, &distance, &w.target);
    cam->pos = w.eye;
    cam->rot = *angles;
    cam->dist = distance;
    Camera_ShakeOffset(&w.shake);
    Camera_BuildViewMatrix(cam, &w.shake);
    g_camStateFlags.bits.skipSweep = 0;
    g_camStateFlags.bits.lookExitBlocked = 0;
}
/* 0x556c6a */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; unused0, unused1, unused2, unused4, unused5 fill gaps */
s16 Camera_FindClearDistance(const Vec3s *target, const Vec3s *angles, s16 maxDistance)
{
    /* Original scratch slots and unused alignment gaps, not object layout. */
    struct Work {
        CollBox box;
        u16 unused0;
        s16 step;
        s32 ground;
        Vec3i previous;
        u32 unused1;
        Vec3s delta;
        u16 unused2;
        s16 unused3, travel, best, last;
        Vec3s first;
        u16 unused4;
        Vec3i current;
        u32 unused5;
    } w;
    w.last = 0;
    w.step = (g_camEyeBoxMax.x - g_camEyeBoxMin.x) >> 2;
    w.best = 0;
    w.previous.x = target->x;
    w.previous.y = target->y;
    w.previous.z = target->z;
    w.travel = w.step;
    Vec3s_OffsetAlongAngles(&w.first, angles, w.travel, target);
    w.delta.x = w.first.x - w.previous.x;
    w.delta.y = w.first.y - w.previous.y;
    w.delta.z = w.first.z - w.previous.z;
    w.current.x = w.first.x;
    w.current.y = w.first.y;
    w.current.z = w.first.z;
    while (w.travel < maxDistance) {
        w.box.flags = 0;
        w.box.min.x = g_camEyeBoxMin.x + w.current.x;
        w.box.min.y = g_camEyeBoxMin.y + w.current.y;
        w.box.min.z = g_camEyeBoxMin.z + w.current.z;
        w.box.max.x = g_camEyeBoxMax.x + w.current.x;
        w.box.max.y = g_camEyeBoxMax.y + w.current.y;
        w.box.max.z = g_camEyeBoxMax.z + w.current.z;
        if (Collide_SegmentClearStatic(&w.current.x, &w.previous.x, 0) < 0x7fffffff)
            break;
        if (!Coll_BoxGroundQuery(&w.box, &w.ground, 0, CQ_STATIC | CQ_STATIC_EXT, 0))
            w.best = w.travel;
        w.last = w.travel;
        w.travel += w.step;
        w.previous.x = (s16)w.current.x;
        w.previous.y = (s16)w.current.y;
        w.previous.z = (s16)w.current.z;
        w.current.x += w.delta.x;
        w.current.y += w.delta.y;
        w.current.z += w.delta.z;
    }
    return w.best;
}
/* 0x556e0b */
/* BYTES(slot-group): locals grouped in w / AngleWork only to pin the original frame offsets; unused8a, unused82, unused76, unused74, unused5e, unused52, unused44, unused12, unused04 fill gaps */
/* BYTES(slot-scope, inferred): the nested block(s) only order the frame: their locals are allocated after the enclosing scope's */
/* BYTES(slot-group, inferred): AngleWork is packed to 4 so its double sits where the original frame has it (4-aligned) */
void Camera_UpdateSamChase(Camera *camera, Pad *pad)
{
    struct Work {
        Vec3s minus;
        u16 unused8a;
        Vec3s plus;
        u16 unused82;
        s32 horizontal;
        Vec3s direction;
        u16 unused76;
        u16 unused74;
        s16 verticalStep;
        s32 moveX;
        s32 outside, push;
        Vec3s eye;
        u16 unused5e;
        s32 central;
        Vec3s target;
        u16 unused52;
        s32 moveZ, closeDistance, side;
        u16 unused44;
        s16 maxVertical;
        s32 recenter, snap, onTrajectory, mirror;
        CamModeParams *params;
        CamRestrict *restriction;
        s16 distance, focal;
        s32 restricted, vertical, noPad;
        Vec3s angles;
        u16 unused12;
        Vec3i speeds;
        u16 unused04;
        s16 yawDifference;
    } w;
    {
#pragma pack(push, 4)
        struct AngleWork {
            s32 negZ, x;
            double value;
        };
#pragma pack(pop)
        AngleWork a;
        w.params = &g_camModeParams[g_camMode];
        w.restriction = g_camRestriction;
        w.speeds = *(Vec3i *)&w.params->pitchSpeed; /* cast kept: pitch, yaw, roll speed copied as one Vec3i */
        w.focal = w.params->focal;
        w.recenter = 0;
        w.restricted = w.restriction &&
                       (((g_camMode == CAM_FOLLOW || g_camMode == CAM_SAM_CHASE) && w.restriction->flags.modeFollow) ||
                        (g_camMode == CAM_ROBOT && w.restriction->flags.modeRobot) ||
                        (g_camMode == CAM_FOLLOW_RUN && w.restriction->flags.modeRun));
        w.onTrajectory = w.restricted && w.restriction->trajectory;
        w.noPad = (w.restricted && w.restriction->flags.forbidPad) || w.onTrajectory;
        w.mirror = w.restricted && w.restriction->flags.mirrorYaw;
        w.snap = g_camReqFlags.snapYaw || (w.restricted && w.restriction->flags.snapYaw);
        w.side = 0;
        if (!w.noPad) {
            if (pad->cur.typeLen.type == PADTYPE_ANALOG)
                Pad_StickToDeadzonedAxes(pad->cur.rightX, pad->cur.rightY, &w.side, &w.vertical);
            if (!w.side) {
                if (w.snap) {
                    if (!(pad->cur.buttons & ~g_padMasks[4]) && (pad->prev.buttons & ~g_padMasks[4]))
                        w.side = -256;
                    else if (!(pad->cur.buttons & ~g_padMasks[5]) && (pad->prev.buttons & ~g_padMasks[5]))
                        w.side = 256;
                } else {
                    if (!(pad->cur.buttons & ~g_padMasks[4])) {
                        if (!(pad->cur.buttons & ~g_padMasks[5]))
                            w.recenter = 1;
                        else
                            w.side = -256;
                    } else if (!(pad->cur.buttons & ~g_padMasks[5]))
                        w.side = 256;
                }
            }
            w.side = w.side * g_dt * w.speeds.y * 2 >> 20;
        }
        if (w.side)
            Camera_OnManualRotate(w.side);
        w.side += ((g_camAutoYawRate << 8) * g_dt) >> 20;
        w.distance = camera->dist;
        if (g_camReqFlags.useYaw)
            w.angles.y = g_camReqRot.y;
        else
            w.angles.y = camera->rot.y;
        w.angles.z = g_camReqRot.z;
        if (!w.onTrajectory) {
            if (g_camReqFlags.usePitch)
                w.angles.x = g_camReqRot.x;
            else if (g_camPitchAdjState.state) {
                w.closeDistance = camera->dist < 200;
                w.angles.x = camera->rot.x + (g_camAutoPitchRate * g_dt >> 12);
                if (w.angles.x > 0x300)
                    w.angles.x = 0x300;
                if (w.angles.x < g_camMinPitch)
                    w.angles.x = g_camMinPitch;
            } else if ((s16)((s16)((camera->rot.x - g_camMinPitch + 0x800) & 0xfff) - 0x800) < 0) {
                w.angles.x = g_camMinPitch;
                w.speeds.x = w.params->pitchSpeed * 3;
            } else {
                switch (g_camLedgeLookState) {
                    case LEDGE_NONE:
                        w.angles.x = camera->rot.x;
                        break;
                    case LEDGE_LOOKING:
                        w.angles.x = 0x2aa;
                        break;
                    case LEDGE_BLOCKED_1:
                        w.angles.x = camera->rot.x;
                        break;
                    case LEDGE_BLOCKED_2:
                        w.angles.x = camera->rot.x;
                        break;
                }
            }
        } else if (g_camAutoPitchRate) {
            if (camera->rot.x < 0x300)
                w.angles.x = camera->rot.x + (g_camAutoPitchRate * g_dt >> 12);
        } else
            w.angles.x = camera->rot.x;
        if (w.side)
            w.snap = 0;
        if (w.side || w.recenter || w.snap || w.onTrajectory || w.mirror)
            w.angles.y = (w.angles.y + w.side) & 0xfff;
        else if ((g_camSettings & CAMSET_AUTO_YAW_FOLLOW) && (g_camReqVel.x | g_camReqVel.z)) {
            a.negZ = -g_camReqVel.z;
            a.x = g_camReqVel.x;
            if ((a.value = (a.value = atan2(a.x, a.negZ)) * 651.898646904404) < 0)
                a.value -= 0.5;
            else
                a.value += 0.5;
            w.angles.y = ((s32)a.value + 0x800) & 0xfff;
            w.yawDifference = (s16)((w.angles.y - camera->rot.y + 0x800) & 0xfff) - 0x800;
            if (w.yawDifference < 0)
                w.yawDifference = -w.yawDifference;
            if (w.yawDifference > 0x400)
                w.yawDifference = 0x800 - w.yawDifference;
            w.speeds.y = w.yawDifference * g_camAutoYawGain >> 12;
            w.speeds.y = w.speeds.y * g_camReqSpeedFactor >> 12;
        }
        if (w.restricted)
            Camera_ApplyRestriction(&w.angles, &w.speeds.x, &w.distance, &w.focal, w.restriction);
        if (w.onTrajectory)
            Camera_FollowTrajectory(w.restriction->trajectory, &w.angles, &w.distance, &w.speeds.x, w.snap);
        else {
            g_camTrajectory = 0;
            g_camTrajSegment = 0;
        }
        Camera_SmoothToDesired(camera, &w.angles, &w.speeds.x, &w.distance, w.focal, &g_camReqAimOffset,
                               w.onTrajectory);
        if (w.restricted)
            w.maxVertical = w.restriction->eyeVertMax;
        else
            w.maxVertical = 32000;
        w.outside = 0;
        w.push = 0;
        w.target.x = g_camReqTarget.x + g_camAimOffset.x;
        w.target.y = g_camReqTarget.y + g_camAimOffset.y;
        w.target.z = g_camReqTarget.z + g_camAimOffset.z;
        if (g_camSamPushTimerMs > 0)
            g_camSamPushTimerMs -= (s16)g_dtMs;
        if (!Camera_IsPointOnScreen(camera, w.angles, w.distance, &w.target, &g_camChaseTarget2, &w.central)) {
            if (!g_camStateFlags.bits.samPushIn || g_camSamPushTimerMs <= 0) {
                if (!g_camStateFlags.bits.samPushOut) {
                    g_camStateFlags.bits.samPushOut = 1;
                    g_camStateFlags.bits.samPushIn = 0;
                    g_camSamPushTimerMs = 1000;
                }
                g_camSamPush.value += 5;
            }
        } else if (w.central) {
            if (!g_camStateFlags.bits.samPushOut || g_camSamPushTimerMs <= 0) {
                if (!g_camStateFlags.bits.samPushOut) {
                    g_camStateFlags.bits.samPushIn = 1;
                    g_camStateFlags.bits.samPushOut = 0;
                    g_camSamPushTimerMs = 1000;
                }
                g_camSamPush.value -= 5;
            }
        } else if ((!g_camStateFlags.bits.samPushIn && !g_camStateFlags.bits.samPushOut) || g_camSamPushTimerMs <= 0) {
            g_camSamPush.value /= 2;
            g_camSamPushTimerMs = 0;
            g_camStateFlags.bits.samPushOut = 0;
            g_camStateFlags.bits.samPushIn = 0;
        }
        if (w.angles.x > 0x300)
            w.angles.x = 0x300;
        if (g_camSamPush.value > 40)
            g_camSamPush.value = 40;
        else if (g_camSamPush.value < -30)
            g_camSamPush.value = -30;
        w.push = g_camSamPush.value;
        Vec3s_OffsetAlongAngles(&w.eye, &w.angles, w.distance, &w.target);
        if (w.push) {
            w.direction.x = w.eye.x - w.target.x;
            w.direction.z = w.eye.z - w.target.z;
            w.horizontal = (s32)sqrt((double)w.direction.x * (double)w.direction.x + w.direction.z * w.direction.z);
            if (!w.horizontal)
                w.horizontal = 1;
            w.direction.x = (w.direction.x << 10) / w.horizontal;
            w.direction.z = (w.direction.z << 10) / w.horizontal;
            if (w.horizontal < w.params->dist && w.push < 0)
                w.push = 3;
            if ((w.horizontal < 1000 && w.push > 0) || (w.horizontal > w.params->dist && w.push < 0)) {
                w.moveX = w.push * w.direction.x;
                w.moveZ = w.push * w.direction.z;
                if (ABS_VALUE(w.moveX) + ABS_VALUE(w.moveZ) > 2000) {
                    w.eye.x += (s16)(w.moveX / 1024);
                    w.eye.z += (s16)(w.moveZ / 1024);
                }
            } else if (w.horizontal > 1000 && w.push > 0) {
                w.eye.x = w.target.x + w.direction.x * 1020 / 1024;
                w.eye.z = w.target.z + w.direction.z * 1020 / 1024;
            } else if (w.horizontal < w.params->dist && w.push < 0) {
                w.eye.x = w.target.x + w.direction.x * (w.params->dist - 20) / 1024;
                w.eye.z = w.target.z + w.direction.z * (w.params->dist - 20) / 1024;
            }
        }
        w.verticalStep = g_camChaseEyeVert - camera->pos.y;
        if (w.verticalStep < -20)
            w.verticalStep = -10;
        else if (w.verticalStep > 10)
            w.verticalStep = 20;
        w.eye.y = camera->pos.y + w.verticalStep;
        g_camChaseHyst.y = 0;
        if (w.eye.x < g_camChaseBox[0] || w.eye.x > g_camChaseBox[2]) {
            g_camChaseHyst.z = 30;
            g_camChaseHyst.x = 0;
            w.outside = 1;
        } else if (w.eye.z < g_camChaseBox[1] || w.eye.z > g_camChaseBox[3]) {
            g_camChaseHyst.x = 30;
            g_camChaseHyst.z = 0;
            w.outside = 1;
        }
        if (w.outside) {
            w.plus.x = w.eye.x + g_camChaseHyst.x;
            w.plus.y = w.eye.y + g_camChaseHyst.y;
            w.plus.z = w.eye.z + g_camChaseHyst.z;
            w.minus.x = w.eye.x - g_camChaseHyst.x;
            w.minus.y = w.eye.y;
            w.minus.z = w.eye.z - g_camChaseHyst.z;
            if (ABS_VALUE(w.plus.x - w.target.x) + ABS_VALUE(w.plus.z - w.target.z) >
                ABS_VALUE(w.minus.x - w.target.x) + ABS_VALUE(w.minus.z - w.target.z))
                w.eye = w.plus;
            else
                w.eye = w.minus;
            if (w.eye.x < g_camChaseBox[0])
                w.eye.x = g_camChaseBox[0];
            if (w.eye.x > g_camChaseBox[2])
                w.eye.x = g_camChaseBox[2];
            if (w.eye.z < g_camChaseBox[1])
                w.eye.z = g_camChaseBox[1];
            if (w.eye.z > g_camChaseBox[3])
                w.eye.z = g_camChaseBox[3];
        }
        Camera_EyeToAngles(&w.eye, &w.angles, &w.distance, &w.target);
        Camera_SolveCollision(camera, &w.angles, w.distance, !g_camFlags.snap && !w.onTrajectory, w.maxVertical);
        g_camLedgeLookState = LEDGE_NONE;
    }
}
/* 0x557d8d */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; unused5c, unused50, unused1a, unused12, unused04 fill gaps */
void Camera_UpdateFollow(Camera *camera, Pad *pad)
{
    struct Work {
        s32 velocityZ, velocityX;
        double radians;
        s32 longDistance;
        u16 unused5c;
        s16 zoom;
        s32 hit, side;
        u16 unused50;
        s16 maxVert;
        s32 both, snap, trajectory, mirror;
        CamModeParams *params;
        CamRestrict *restriction;
        s16 distance, focal;
        s32 restrictEnabled, vertical, forbidPad, smooth;
        Vec3s angles;
        u16 unused1a;
        Vec3s aim;
        u16 unused12;
        Vec3i speeds;
        u16 unused04;
        s16 angleDelta;
    } w;
    w.params = &g_camModeParams[g_camMode];
    w.restriction = g_camRestriction;
    w.speeds = *(Vec3i *)&w.params->pitchSpeed; /* cast kept: pitch, yaw, roll speed copied as one Vec3i */
    w.aim = g_camReqAimOffset;
    if (g_camReqFocal)
        w.focal = g_camReqFocal;
    else
        w.focal = w.params->focal;
    w.both = 0;
    w.restrictEnabled = w.restriction &&
                        (((g_camMode == CAM_FOLLOW || g_camMode == CAM_SAM_CHASE) && w.restriction->flags.modeFollow) ||
                         (g_camMode == CAM_ROBOT && w.restriction->flags.modeRobot) ||
                         (g_camMode == CAM_FOLLOW_RUN && w.restriction->flags.modeRun));
    w.trajectory = w.restrictEnabled && w.restriction->trajectory;
    w.forbidPad = (w.restrictEnabled && w.restriction->flags.forbidPad) || w.trajectory;
    w.mirror = w.restrictEnabled && w.restriction->flags.mirrorYaw;
    w.snap = g_camReqFlags.snapYaw || (w.restrictEnabled && w.restriction->flags.snapYaw);
    w.smooth = g_camFlags.smooth;
    w.side = 0;
    if (!w.forbidPad) {
        if (pad->cur.typeLen.type == PADTYPE_ANALOG)
            Pad_StickToDeadzonedAxes(pad->cur.rightX, pad->cur.rightY, &w.side, &w.vertical);
        if (w.side == 0) {
            if (w.snap) {
                if (!(pad->cur.buttons & ~g_padMasks[4]) && (pad->prev.buttons & ~g_padMasks[4]))
                    w.side = -256;
                else if (!(pad->cur.buttons & ~g_padMasks[5]) && (pad->prev.buttons & ~g_padMasks[5]))
                    w.side = 256;
            } else {
                if (!(pad->cur.buttons & ~g_padMasks[4])) {
                    if (!(pad->cur.buttons & ~g_padMasks[5]))
                        w.both = 1;
                    else
                        w.side = -256;
                } else if (!(pad->cur.buttons & ~g_padMasks[5]))
                    w.side = 256;
            }
        }
        if (w.side != 0)
            Camera_OnManualRotate(w.side);
        if (w.side < 0 && Vec3s_ManhattanDist(&camera->pos, &g_camBlockPosNeg) < 80)
            w.side = 0;
        if (w.side > 0 && Vec3s_ManhattanDist(&camera->pos, &g_camBlockPosPos) < 80)
            w.side = 0;
        w.side = (w.side * g_dt * w.speeds.y * 2) >> 20;
    }
    w.side += (g_camAutoYawRate * 256 * g_dt) >> 20;
    if (g_camReqDist)
        w.distance = g_camReqDist;
    else {
        w.hit = g_camHitFrames > 1;
        w.longDistance = camera->dist > 800;
        if (g_camFlags.smooth && !g_camFlags.snap && !g_camStateFlags.bits.cut)
            w.zoom = g_camZoomOut;
        else
            w.zoom = 0;
        if (w.longDistance || g_camFlags.snap)
            w.distance = w.params->dist + w.zoom;
        else if (w.hit) {
            if (ABS_VALUE(g_camLastHitTarget.x - g_camReqTarget.x) +
                    ABS_VALUE(g_camLastHitTarget.y - g_camReqTarget.y) +
                    ABS_VALUE(g_camLastHitTarget.z - g_camReqTarget.z) <
                30)
                w.distance = camera->dist + w.zoom;
            else {
                g_camHitFrames = 0;
                w.distance = ((w.params->dist + camera->dist) >> 1) + w.zoom;
            }
        } else
            w.distance = w.params->dist + w.zoom;
    }
    if (g_camReqFlags.useYaw)
        w.angles.y = g_camReqRot.y;
    else
        w.angles.y = camera->rot.y;
    w.angles.z = g_camReqRot.z;
    if (!w.trajectory) {
        if (g_camReqFlags.usePitch)
            w.angles.x = g_camReqRot.x;
        else if (g_camPitchAdjState.state) {
            w.angles.x = camera->rot.x + (g_camAutoPitchRate * g_dt >> 12);
            if (w.angles.x > 0x300)
                w.angles.x = 0x300;
            if (w.angles.x < g_camMinPitch) {
                w.angles.x = g_camMinPitch;
                if (g_camPitchAdjState.state == PADJ_LOWER)
                    g_camPitchAdjState.state = PADJ_NONE;
            }
        } else if (DELTA_ANGLE(camera->rot.x, g_camMinPitch) < 0) {
            w.angles.x = g_camMinPitch;
            w.speeds.x = w.params->pitchSpeed * 3;
        } else {
            switch (g_camLedgeLookState) {
                case LEDGE_NONE:
                    w.angles.x = g_camMinPitch;
                    break;
                case LEDGE_LOOKING:
                    w.angles.x = 0x2aa;
                    break;
                case LEDGE_BLOCKED_1:
                    w.angles.x = camera->rot.x;
                    break;
                case LEDGE_BLOCKED_2:
                    w.angles.x = camera->rot.x;
                    break;
            }
        }
    } else {
        if (g_camAutoPitchRate) {
            w.angles.x = camera->rot.x;
            if (w.angles.x < 0x300)
                w.angles.x += (s16)(g_camAutoPitchRate * g_dt >> 12);
        } else
            w.angles.x = camera->rot.x;
    }
    if (w.side != 0)
        w.snap = 0;
    if (w.side != 0 || w.both || w.snap || w.trajectory || w.mirror)
        w.angles.y = (w.angles.y + w.side) & 0xfff;
    else if ((g_camSettings & CAMSET_AUTO_YAW_FOLLOW) && (g_camReqVel.x | g_camReqVel.z)) {
        w.velocityZ = -g_camReqVel.z;
        w.velocityX = g_camReqVel.x;
        if ((w.radians = (w.radians = atan2(w.velocityX, w.velocityZ)) * 651.898646904404) < 0)
            w.radians -= 0.5;
        else
            w.radians += 0.5;
        w.angles.y = ((s32)w.radians + 0x800) & 0xfff;
        w.angleDelta = (s16)(((w.angles.y - camera->rot.y + 0x800) & 0xfff)) - 0x800;
        if (w.angleDelta < 0)
            w.angleDelta = -w.angleDelta;
        if (w.angleDelta > 0x400)
            w.angleDelta = 0x800 - w.angleDelta;
        w.speeds.y = w.angleDelta * g_camAutoYawGain >> 12;
        w.speeds.y = w.speeds.y * g_camReqSpeedFactor >> 12;
    }
    if (w.restrictEnabled) {
        Camera_ApplyRestriction(&w.angles, &w.speeds.x, &w.distance, &w.focal, w.restriction);
        w.aim.x += w.restriction->aimOffset.x;
        w.aim.y += w.restriction->aimOffset.y;
        w.aim.z += w.restriction->aimOffset.z;
    }
    if (w.snap) {
        w.angleDelta = w.angles.y;
        w.angles.y = (w.angles.y + 0x200) & 0xfff;
        w.angles.y = w.angles.y - w.angles.y % 0x400;
        w.angleDelta = (s16)(((w.angles.y - w.angleDelta + 0x800) & 0xfff)) - 0x800;
        if (w.angleDelta && g_camYawVel && (w.angleDelta >> 31) != (g_camYawVel >> 31))
            w.angles.y = (w.angles.y + 0x800) & 0xfff;
        if (w.restriction && w.restriction->yawSpeed)
            w.speeds.y = w.restriction->yawSpeed;
        else
            w.speeds.y = w.params->yawSpeed;
    }
    if (w.trajectory)
        Camera_FollowTrajectory(w.restriction->trajectory, &w.angles, &w.distance, &w.speeds.x, w.snap);
    else {
        g_camTrajectory = 0;
        g_camTrajSegment = 0;
    }
    Camera_SmoothToDesired(camera, &w.angles, &w.speeds.x, &w.distance, w.focal, &w.aim, w.trajectory);
    if (w.restrictEnabled)
        w.maxVert = w.restriction->eyeVertMax;
    else
        w.maxVert = 32000;
    Camera_SolveCollision(camera, &w.angles, w.distance, !g_camFlags.snap && w.smooth && !w.trajectory, w.maxVert);
    if ((g_camSettings & CAMSET_LEDGE_LOOK) && !w.trajectory)
        Camera_UpdateLedgeLook(camera);
    else
        g_camLedgeLookState = LEDGE_NONE;
}
/* 0x558857 */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
void Camera_UpdateRocket(Camera *camera, Pad *)
{
    struct Work {
        CamModeParams *params;
        s16 distance, focal;
        Vec3s angles;
    } w;
    w.params = &g_camModeParams[g_camMode];
    if (g_camReqDist)
        w.distance = g_camReqDist;
    else
        w.distance = w.params->dist;
    if (g_camReqFocal)
        w.focal = g_camReqFocal;
    else
        w.focal = w.params->focal;
    w.angles = g_camReqRot;
    w.angles.x = (g_camMinPitch + w.angles.x) & 0xfff;
    Camera_SmoothToDesired(camera, &w.angles, &w.params->pitchSpeed, &w.distance, w.focal, &g_camReqAimOffset, 0);
    Camera_SolveCollision(camera, &w.angles, w.distance, !g_camFlags.snap, 32000);
}
/* 0x558934 */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; unused18, unused0e, unused06 fill gaps */
void Camera_UpdateLook(Camera *camera, Pad *pad)
{
    struct Work {
        s32 side;
        CamModeParams *params;
        u16 unused18;
        s16 distance;
        Vec3s angles;
        u16 unused0e;
        Vec3s target;
        u16 unused06;
        s32 vertical;
    } w;
    w.params = &g_camModeParams[g_camMode];
    w.side = w.vertical = 0;
    if (pad->cur.typeLen.type == PADTYPE_ANALOG) {
        Pad_StickToDeadzonedAxes(pad->cur.rightX, pad->cur.rightY, &w.side, &w.vertical);
        if ((w.side | w.vertical) == 0)
            Pad_StickToDeadzonedAxes(pad->cur.leftX, pad->cur.leftY, &w.side, &w.vertical);
    }
    if ((w.side | w.vertical) == 0) {
        if (!(pad->cur.buttons & ~(u16)~PAD_RIGHT))
            w.side = 256;
        else if (!(pad->cur.buttons & ~(u16)~PAD_LEFT))
            w.side = -256;
        if (!(pad->cur.buttons & ~(u16)~PAD_UP))
            w.vertical = -256;
        else if (!(pad->cur.buttons & ~(u16)~PAD_DOWN))
            w.vertical = 256;
        if ((w.side | w.vertical) == 0) {
            if (!(pad->cur.buttons & ~g_padMasks[4]))
                w.side = -256;
            if (!(pad->cur.buttons & ~g_padMasks[5]))
                w.side += 256;
        }
    }
    if (w.side | w.vertical) {
        g_camFlags.manual = 1;
        Camera_OnManualRotate(w.side);
    }
    w.distance = w.params->dist;
    if (g_camFlags.manual) {
        w.angles.x = camera->rot.x;
        w.angles.y = camera->rot.y;
    } else {
        w.angles.x = 0;
        w.angles.y = g_camReqRot.y;
    }
    w.angles.z = 0;
    w.side = w.side * g_dt * w.params->yawSpeed >> 20;
    w.vertical = w.vertical * g_dt * w.params->pitchSpeed >> 20;
    if (w.vertical != 0)
        w.angles.x = (camera->rot.x + w.vertical) & 0xfff;
    if (w.side != 0)
        w.angles.y = (camera->rot.y - w.side) & 0xfff;
    if (w.angles.x > 0x800) {
        if (w.angles.x < 0xd56)
            w.angles.x = 0xd56;
    } else if (w.angles.x > 0x2aa)
        w.angles.x = 0x2aa;
    Camera_SmoothToDesired(camera, &w.angles, &w.params->pitchSpeed, &w.distance, w.params->focal, &g_camReqAimOffset,
                           0);
    Camera_SolveCollision(camera, &w.angles, w.distance, 0, 32000);
    w.target.x = g_camReqTarget.x + g_camAimOffset.x;
    w.target.y = g_camReqTarget.y + g_camAimOffset.y;
    w.target.z = g_camReqTarget.z + g_camAimOffset.z;
    g_camStateFlags.bits.lookExitBlocked = Camera_IsEyeClear(camera, &w.target) == 0;
    g_camLookTime = g_gameTime;
}
/* 0x558c3d */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; unused14, unused04 fill gaps */
s16 Camera_IsEyeClear(const Camera *camera, const Vec3s *target)
{
    /* Complete observed frame: CollBox -34, output -24, target -20, eye -10.
     * The two unused trailing dwords are neither initialized nor accessed. */
    struct Work {
        CollBox box;
        s32 ground;
        Vec3i target;
        s32 unused14;
        Vec3i eye;
        s32 unused04;
    } w;
    w.box.flags = 0;
    w.box.min.x = g_camEyeBoxMin.x + camera->pos.x;
    w.box.min.y = g_camEyeBoxMin.y + camera->pos.y;
    w.box.min.z = g_camEyeBoxMin.z + camera->pos.z;
    w.box.max.x = g_camEyeBoxMax.x + camera->pos.x;
    w.box.max.y = g_camEyeBoxMax.y + camera->pos.y;
    w.box.max.z = g_camEyeBoxMax.z + camera->pos.z;
    w.eye.x = camera->pos.x;
    w.eye.y = camera->pos.y;
    w.eye.z = camera->pos.z;
    w.target.x = target->x;
    w.target.y = target->y;
    w.target.z = target->z;
    if (Collide_SegmentClearStatic(&w.eye.x, &w.target.x, 0) < 0x7fffffff)
        return 0;
    if (Coll_BoxGroundQuery(&w.box, &w.ground, 0, CQ_STATIC | CQ_STATIC_EXT, 0))
        return 0;
    return 1;
}
/* 0x558d5d */
/* BYTES(slot-group): locals grouped in w / AngleWork / a only to pin the original frame offsets; unused0e, unused fill gaps */
/* BYTES(slot-scope, inferred): the nested block(s) only order the frame: their locals are allocated after the enclosing scope's */
void Camera_UpdateFixedLookAt(Camera *camera)
{
    struct Work {
        CamModeParams *params;
        Vec3s shake;
        u16 unused0e;
        Vec3i delta;
    } w;
    {
        s32 squareX, squareY, dzSquared;
        w.params = &g_camModeParams[g_camMode];
        w.delta.x = g_camReqTarget.x - camera->pos.x;
        w.delta.y = g_camReqTarget.y - camera->pos.y;
        w.delta.z = g_camReqTarget.z - camera->pos.z;
        w.delta.x += g_camAimOffset.x;
        w.delta.y += g_camAimOffset.y;
        w.delta.z += g_camAimOffset.z;
        squareX = w.delta.x * w.delta.x;
        squareY = w.delta.y * w.delta.y;
        dzSquared = w.delta.z * w.delta.z;
        {
#pragma pack(push, 4)
            struct AngleWork {
                s32 converted;
                double value;
                s32 unused;
            };
#pragma pack(pop)
            struct AnglePair {
                AngleWork yaw, pitch;
            } a;
            a.pitch.converted = (s32)sqrt((double)squareX + (double)dzSquared);
            if ((a.pitch.value = (a.pitch.value = atan2(w.delta.y, a.pitch.converted)) * 651.898646904404) < 0)
                a.pitch.value -= 0.5;
            else
                a.pitch.value += 0.5;
            camera->rot.x = (s32)a.pitch.value & 0xfff;
            if ((a.yaw.value = (a.yaw.value = atan2(a.yaw.converted = -w.delta.x, w.delta.z)) * 651.898646904404) < 0)
                a.yaw.value -= 0.5;
            else
                a.yaw.value += 0.5;
            camera->rot.y = (s32)a.yaw.value & 0xfff;
            camera->rot.z = 0;
        }
        camera->dist = (s16)sqrt((double)squareX + (double)squareY + (double)dzSquared);
        g_camFocal = Math_StepTowards(g_camFocal, w.params->focal, 300);
        g_screen.SetProjection(g_camFocal);
        Camera_ShakeOffset(&w.shake);
        Camera_BuildViewMatrix(camera, &w.shake);
    }
}
/* 0x558f75 */
void Camera_UpdateScriptedHold(Camera *camera)
{
    Vec3s shake;
    Camera_ShakeOffset(&shake);
    Camera_BuildViewMatrix(camera, &shake);
}
/* 0x558f9b */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; unused0e fill gaps */
void Camera_UpdateDirected(Camera *camera, Pad *pad)
{
    struct Work {
        s32 side;
        CamModeParams *params;
        CamRestrict *restriction;
        s16 distance, focal;
        s32 vertical;
        Vec3s angles;
        u16 unused0e;
        Vec3i speeds;
    } w;
    w.params = &g_camModeParams[g_camMode];
    w.restriction = g_camRestriction;
    w.angles = g_camReqRot;
    if (g_camReqFlags.padRotate) {
        if (g_camFlags.manual)
            w.angles.y = camera->rot.y;
        w.side = 0;
        if (pad->cur.typeLen.type == PADTYPE_ANALOG)
            Pad_StickToDeadzonedAxes(pad->cur.rightX, pad->cur.rightY, &w.side, &w.vertical);
        if (w.side == 0) {
            if (!(pad->cur.buttons & ~g_padMasks[4]))
                w.side = -256;
            else if (!(pad->cur.buttons & ~g_padMasks[5]))
                w.side = 256;
        }
        if (w.side != 0) {
            g_camFlags.manual = 1;
            Camera_OnManualRotate(w.side);
            w.side = (w.side * g_dt * w.params->yawSpeed * 2) >> 20;
            w.angles.y = (camera->rot.y + w.side) & 0xfff;
        }
    }
    if (g_camReqDist)
        w.distance = g_camReqDist;
    else
        w.distance = w.params->dist;
    if (g_camReqFocal)
        w.focal = g_camReqFocal;
    else
        w.focal = w.params->focal;
    w.speeds = *(Vec3i *)&w.params->pitchSpeed; /* cast kept: pitch, yaw, roll speed copied as one Vec3i */
    if (w.restriction && g_camReqFlags.restriction) {
        Camera_ApplyRestriction(&w.angles, &w.speeds.x, &w.distance, &w.focal, w.restriction);
        if (w.restriction->trajectory)
            Camera_FollowTrajectory(w.restriction->trajectory, &w.angles, &w.distance, &w.speeds.x, 0);
        else {
            g_camTrajectory = 0;
            g_camTrajSegment = 0;
        }
    }
    Camera_SmoothToDesired(camera, &w.angles, &w.speeds.x, &w.distance, w.focal, &g_camReqAimOffset, 0);
    Camera_SolveCollision(camera, &w.angles, w.distance, !g_camFlags.snap, 32000);
}
/* 0x5591f2 */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; unused1c, unused12 fill gaps */
void Camera_UpdateOrbit_Unused(Camera *camera, Pad *pad)
{
    struct Work {
        s32 side;
        CamModeParams *params;
        u16 unused1c;
        s16 distance;
        Vec3s angles;
        u16 unused12;
        Vec3i speeds;
        s32 vertical;
    } w;
    w.params = &g_camModeParams[11];
    w.speeds = *(Vec3i *)&w.params->pitchSpeed; /* cast kept: pitch, yaw, roll speed copied as one Vec3i */
    w.side = w.vertical = 0;
    if (pad->cur.typeLen.type == PADTYPE_ANALOG)
        Pad_StickToDeadzonedAxes(pad->cur.rightX, pad->cur.rightY, &w.side, &w.vertical);
    if (w.side == 0) {
        if (!(pad->cur.buttons & ~g_padMasks[4]))
            w.side -= 256;
        if (!(pad->cur.buttons & ~g_padMasks[5]))
            w.side += 256;
    }
    w.side = (w.side * g_dt * w.speeds.y * 2) >> 20;
    w.distance = w.params->dist;
    w.angles.x = g_camMinPitch;
    w.angles.y = camera->rot.y;
    w.angles.z = 0;
    if (w.side != 0)
        w.angles.y = (camera->rot.y + w.side) & 0xfff;
    else {
        if (g_camYawVel >= 0)
            w.angles.y = (w.angles.y + 0x400) & 0xfff;
        else
            w.angles.y = (w.angles.y - 0x400) & 0xfff;
        w.speeds.y /= 4;
    }
    Camera_SmoothToDesired(camera, &w.angles, &w.speeds.x, &w.distance, w.params->focal, &g_camReqAimOffset, 0);
    Camera_SolveCollision(camera, &w.angles, w.distance, 0, 32000);
}
/* 0x559381 */
s16 Camera_EaseLerp(s16 from, s16 to, const u8 *curve, u16 time, u16 timeBits, u16 curveBits)
{
    s32 weight;
    s32 index;
    s32 selectedWeight;
    index = time >> (timeBits - curveBits);
    if (index >= (1 << curveBits) - 1)
        selectedWeight = curve[(1 << curveBits) - 1] << 8;
    else
        selectedWeight = curve[index] << 8;
    weight = selectedWeight;
    return (to * weight + (0x10000 - weight) * from) >> 16;
}
/* 0x5593fd */
void Camera_EaseLerpVec3s(Vec3s *io, const Vec3s *to, const u8 *curve, u16 time, u16 timeBits, u16 curveBits)
{
    s32 weight;
    s32 index;
    s32 selectedWeight;
    index = time >> (timeBits - curveBits);
    if (index >= (1 << curveBits) - 1)
        selectedWeight = curve[(1 << curveBits) - 1] << 8;
    else
        selectedWeight = curve[index] << 8;
    weight = selectedWeight;
    io->x = (to->x * weight + (0x10000 - weight) * io->x) >> 16;
    io->y = (to->y * weight + (0x10000 - weight) * io->y) >> 16;
    io->z = (to->z * weight + (0x10000 - weight) * io->z) >> 16;
}
/* 0x5594d5 */
void Camera_EaseLerpAngles(Vec3s *out, const Vec3s *from, const Vec3s *to, const u8 *curve, u16 time, u16 timeBits,
                           u16 curveBits)
{
    s32 weight;
    s32 index;
    s32 selectedWeight;
    index = time >> (timeBits - curveBits);
    if (index >= (1 << curveBits) - 1)
        selectedWeight = curve[(1 << curveBits) - 1] << 8;
    else
        selectedWeight = curve[index] << 8;
    weight = selectedWeight;
    out->x = ((ANGLE_DELTA(to->x, from->x) * weight >> 16) + from->x) & 0xfff;
    out->y = ((ANGLE_DELTA(to->y, from->y) * weight >> 16) + from->y) & 0xfff;
    out->z = ((ANGLE_DELTA(to->z, from->z) * weight >> 16) + from->z) & 0xfff;
}
/* 0x5595fc */
void Camera_EaseFocal(s16 from, s16 to, u16 time)
{
    g_camFocal = Camera_EaseLerp(from, to, g_camEaseCurve, time, 10, 8);
    g_screen.SetProjection(g_camFocal);
}
/* 0x559639 */
/* BYTES(slot-group): locals grouped in AngleWork / a only to pin the original frame offsets; unused fill gaps */
/* BYTES(slot-scope, inferred): the nested block(s) only order the frame: their locals are allocated after the enclosing scope's */
void Camera_UpdateScriptReturn(Camera *camera)
{
    CameraBlendWork w;
    {
        s32 squareX, squareY, dzSquared;
        w.time = ((u32)(g_gameTime - g_camScriptStartTime) << 10) >> 12;
        Camera_EaseLerpVec3s(&g_camAimOffset, &g_camReqAimOffset, g_camEaseCurve, w.time, 10, 8);
        w.aim.x = g_camReqTarget.x + g_camAimOffset.x;
        w.aim.y = g_camReqTarget.y + g_camAimOffset.y;
        w.aim.z = g_camReqTarget.z + g_camAimOffset.z;
        w.delta.x = w.aim.x - camera->pos.x;
        w.delta.y = w.aim.y - camera->pos.y;
        w.delta.z = w.aim.z - camera->pos.z;
        squareX = w.delta.x * w.delta.x;
        squareY = w.delta.y * w.delta.y;
        dzSquared = w.delta.z * w.delta.z;
        if (g_camScriptFlags & CAMSCR_BLEND_OUT_KEEP_ROT)
            w.angles = g_camPreScriptRot;
        else {
#pragma pack(push, 4)
            struct AngleWork {
                s32 converted;
                double value;
                s32 unused;
            };
#pragma pack(pop)
            struct AnglePair {
                AngleWork yaw, pitch;
            } a;
            a.pitch.converted = (s32)sqrt((double)squareX + (double)dzSquared);
            if ((a.pitch.value = (a.pitch.value = atan2(w.delta.y, a.pitch.converted)) * 651.898646904404) < 0)
                a.pitch.value -= 0.5;
            else
                a.pitch.value += 0.5;
            w.angles.x = (s32)a.pitch.value & 0xfff;
            if ((a.yaw.value = (a.yaw.value = atan2(a.yaw.converted = -w.delta.x, w.delta.z)) * 651.898646904404) < 0)
                a.yaw.value -= 0.5;
            else
                a.yaw.value += 0.5;
            w.angles.y = (s32)a.yaw.value & 0xfff;
            w.angles.z = 0;
        }
        camera->dist = (s16)sqrt((double)squareX + (double)squareY + (double)dzSquared);
        Vec3s_OffsetAlongAngles(&w.goal, &w.angles, g_camPreScriptDist, &w.aim);
        w.eye = g_camScriptEye;
        Camera_EaseLerpVec3s(&w.eye, &w.goal, g_camEaseCurve, w.time, 10, 8);
        Camera_EaseLerpAngles(&camera->rot, &g_camScriptRot, &w.angles, g_camEaseCurve, w.time, 10, 8);
        g_camDistVel = 0;
        camera->pos = w.eye;
        Camera_EaseFocal(g_camScriptFocal, g_camPreScriptFocal, w.time);
        Camera_ShakeOffset(&w.shake);
        Camera_BuildViewMatrix(camera, &w.shake);
        if (w.time >= 0x400) {
            g_camFlags.manual = 0;
            g_camFlags.smooth = 1;
            g_camMode = g_camScriptReturnMode;
        } else if (!(g_camScriptFlags & (CAMSCR_BLEND_OUT | CAMSCR_BLEND_OUT_KEEP_ROT))) {
            g_camFlags.manual = 0;
            g_camFlags.smooth = 0;
            g_camMode = g_camScriptReturnMode;
        }
    }
}
/* 0x559972 */
/* BYTES(slot-scope, inferred): the nested block(s) only order the frame: their locals are allocated after the enclosing scope's */
void Camera_UpdateScriptBlendIn(Camera *camera)
{
    CameraBlendWork w;
    {
        s32 squareX, squareY, dzSquared;
        w.time = ((g_gameTime - g_camScriptStartTime) * 0x400) / g_camScriptBlendTime;
        Camera_EaseLerpVec3s(&g_camAimOffset, &g_camReqAimOffset, g_camEaseCurve, w.time, 10, 8);
        w.aim.x = g_camReqTarget.x + g_camAimOffset.x;
        w.aim.y = g_camReqTarget.y + g_camAimOffset.y;
        w.aim.z = g_camReqTarget.z + g_camAimOffset.z;
        w.delta.x = w.aim.x - g_camPreScriptEye.x;
        w.delta.y = w.aim.y - g_camPreScriptEye.y;
        w.delta.z = w.aim.z - g_camPreScriptEye.z;
        squareX = w.delta.x * w.delta.x;
        squareY = w.delta.y * w.delta.y;
        dzSquared = w.delta.z * w.delta.z;
        w.angles = g_camPreScriptRot;
        camera->dist = (s16)sqrt((double)squareX + (double)squareY + (double)dzSquared);
        Vec3s_OffsetAlongAngles(&w.goal, &w.angles, g_camPreScriptDist, &w.aim);
        w.eye = w.goal;
        Camera_EaseLerpVec3s(&w.eye, &g_camScriptEye, g_camEaseCurve, w.time, 10, 8);
        Camera_EaseLerpAngles(&camera->rot, &w.angles, &g_camScriptRot, g_camEaseCurve, w.time, 10, 8);
        g_camDistVel = 0;
        camera->pos = w.eye;
        /* The original passes script focal for both endpoints. */
        Camera_EaseFocal(g_camScriptFocal, g_camScriptFocal, w.time);
        Camera_ShakeOffset(&w.shake);
        Camera_BuildViewMatrix(camera, &w.shake);
        if (w.time >= 0x400)
            Camera_SetMode(CAM_SCRIPTED, 1);
    }
}
/* 0x559b5a */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; unused0a, unused02 fill gaps */
void Camera_UpdateScriptToScript(Camera *camera)
{
    struct Work {
        u16 unused14, time;
        Vec3s shake;
        u16 unused0a;
        Vec3s eye;
        u16 unused02;
    } w;
    w.time = ((g_gameTime - g_camScriptStartTime) * 0x400) / g_camScriptBlendTime;
    w.eye = g_camScriptPrevEye;
    Camera_EaseLerpVec3s(&w.eye, &g_camScriptEye, g_camEaseCurve, w.time, 10, 8);
    Camera_EaseLerpAngles(&camera->rot, &g_camScriptPrevRot, &g_camScriptRot, g_camEaseCurve, w.time, 10, 8);
    g_camDistVel = 0;
    camera->pos = w.eye;
    Camera_EaseFocal(g_camScriptPrevFocal, g_camScriptFocal, w.time);
    Camera_ShakeOffset(&w.shake);
    Camera_BuildViewMatrix(camera, &w.shake);
    if (w.time >= 0x400)
        Camera_SetMode(CAM_SCRIPTED, 1);
}
/* 0x559c49 */
void Camera_ResetMotion()
{
    g_camAimOffsetVel = 0;
    g_camDistVel = 0;
    g_camPitchVel = 0;
    g_camYawVel = 0;
    g_camRollVel = 0;
    g_camAutoPitchRate = 0;
    g_camAutoYawRate = 0;
    g_camAutoRollRate = 0;
    g_camZoomOut = 0;
    g_camTurnState = TURN_IDLE;
    g_camProbeFrame.value = 0;
    g_camProbeYaw = 0;
    g_camProbeYawOffset = 0;
    g_camPitchAdjTimerMs = 0;
    g_camPitchAdjState.state = PADJ_NONE;
    g_camTurnTimerMs = 100;
    g_camFrameCounter = 0;
    g_camBlockedCountA = 0;
    g_camBlockedCountB = 0;
    g_camHitFrames = 0;
    g_camStateFlags.all &= ~CAMS_SAMPUSH_OUT;
    g_camStateFlags.all &= ~CAMS_SAMPUSH_IN;
    g_camStateFlags.all &= ~CAMS_BIT2;
    g_camSamPushTimerMs = 0;
    g_camUnused43fc = 3000;
    g_camStateFlags.all &= ~CAMS_ROT_NEG;
    g_camStateFlags.all &= ~CAMS_ROT_POS;
    g_camStateFlags.all &= ~CAMS_HIT;
    g_camStateFlags.all |= CAMS_SKIP_SWEEP;
    g_camLosClearTime = g_gameTime;
    g_camStateFlags.all &= ~CAMS_CUT;
    g_camLookTime = g_gameTime;
    g_camFlags2.all &= ~CAMF2_OVERHEAD;
    g_camBlockPosNeg.x = -32768;
    g_camBlockPosNeg.y = -32768;
    g_camBlockPosNeg.z = -32768;
    g_camBlockPosPos.x = -32768;
    g_camBlockPosPos.y = -32768;
    g_camBlockPosPos.z = -32768;
    g_camUnusedMotionPos.x = -32768;
    g_camUnusedMotionPos.y = -32768;
    g_camUnusedMotionPos.z = -32768;
}
/* 0x559e07 */
void Camera_SetMode(u8 mode, u8 smooth)
{
    g_camFlags.manual = 0;
    g_camLosClearTime = g_gameTime;
    if (g_camUnused43fc < 3000)
        g_camUnused43fc = 3000;
    switch (mode) {
        case CAM_SAM_CHASE:
            g_camMode = mode;
            g_camSamPush.value = 0;
            g_camStateFlags.all &= ~CAMS_BIT2;
            g_camStateFlags.all &= ~CAMS_SAMPUSH_IN;
            g_camStateFlags.all &= ~CAMS_SAMPUSH_OUT;
            g_camSamPushTimerMs = 0;
            break;
        case CAM_FOLLOW:
        case CAM_ROBOT:
            if (g_camMode == CAM_SCRIPTED && (g_camScriptFlags & (CAMSCR_BLEND_OUT | CAMSCR_BLEND_OUT_KEEP_ROT))) {
                g_camMode = CAM_SCRIPT_RETURN;
                g_camScriptStartTime = g_gameTime;
            } else if (g_camMode == CAM_SCRIPT_BLEND_IN &&
                       (g_camScriptFlags & (CAMSCR_BLEND_OUT | CAMSCR_BLEND_OUT_KEEP_ROT))) {
                g_camScriptEye = g_camera.pos;
                g_camScriptRot = g_camera.rot;
                g_camScriptFocal = g_camFocal;
                g_camMode = CAM_SCRIPT_RETURN;
                g_camScriptStartTime = g_gameTime;
            } else if (g_camMode != CAM_SCRIPT_RETURN) {
                g_camMode = mode;
                Camera_ResetMotion();
            }
            break;
        case CAM_SCRIPT_RETURN:
        case CAM_SCRIPT_BLEND_IN:
        case CAM_SCRIPT_TO_SCRIPT:
            g_camScriptStartTime = g_gameTime;
            g_camMode = mode;
            break;
        case CAM_FOLLOW_RUN:
        case CAM_ROCKET:
            g_camMode = mode;
            break;
        case CAM_LOOK:
        case CAM_DIRECTED:
            g_camLedgeLookState = LEDGE_NONE;
            g_camMode = mode;
            break;
        case CAM_FIXED_LOOKAT:
        case CAM_SCRIPTED:
            Camera_ResetMotion();
            g_camLedgeLookState = LEDGE_NONE;
            g_camMode = mode;
            break;
    }
    g_camFlags.smooth = smooth;
}
/* 0x559ff1 */
void Camera_Reset()
{
    g_camera.pos.x = 0;
    g_camera.pos.y = -400;
    g_camera.pos.z = 0;
    g_camera.dist = 0x400;
    g_camera.rot.x = 0;
    g_camera.rot.y = 0;
    g_camera.rot.z = 0;
    g_camReqTarget.x = 0;
    g_camReqTarget.y = 0;
    g_camReqTarget.z = 0;
    g_camReqVel.x = 0;
    g_camReqVel.y = 0;
    g_camReqVel.z = 0;
    g_camReqAimOffset.x = 0;
    g_camReqAimOffset.y = 0;
    g_camReqAimOffset.z = 0;
    g_camReqRot.x = 0;
    g_camReqRot.y = 0;
    g_camReqRot.z = 0;
    g_camReqSpeedFactor = 0x1000;
    g_camReqDist = 0;
    g_camReqFocal = 0;
    g_camRestriction = 0;
    g_camReqFlags.ledgeProbe = 0;
    g_camReqFlags.padRotate = 1;
    g_camReqFlags.snapYaw = 0;
    g_camReqFlags.usePitch = 0;
    g_camReqFlags.useYaw = 0;
    g_camAimOffset.x = 0;
    g_camAimOffset.y = 0;
    g_camAimOffset.z = 0;
    g_camFocal = (s16)Camera_CurrentProjection();
    g_camLedgeLookState = LEDGE_NONE;
    g_camMode = CAM_FOLLOW;
    g_camTrajectory = 0;
    g_camTrajSegment = 0;
    g_camScriptOwner = 0;
    g_camFlags.manual = 0;
    g_camFlags.smooth = 0;
    g_camFlags.snap = 1;
    g_camFlags.tooFar = 0;
    g_camFlags.tooFarCut = 0;
    g_camScriptFlags = 0;
    g_camScriptStartTime = 0;
    g_camScriptBlendTime = 0x1000;
    g_camStateFlags.all &= ~CAMS_LOOK_EXIT_BLOCKED;
    g_camLookTime = g_gameTime;
    g_camUnderwaterSnd = 0;
    g_camUnusedResetPos.x = -32768;
    g_camUnusedResetPos.y = -32768;
    g_camUnusedResetPos.z = -32768;
    Camera_ResetMotion();
    Camera_StopShake();
    Camera_SetMode(CAM_FOLLOW, 0);
}
/* 0x55a222 */
void Camera_InitOnceStub() {}
/* 0x55a227 */
void Camera_InitSettings()
{
    s32 index;
    if (!g_camSettingsInit) {
        g_camSettingsInit = 1;
        g_camMinPitch = 0xa0;
        g_camSettings = 0x2e;
        g_camAutoYawGain = 0x1000;
        g_camEyeBoxMin.x = -50;
        g_camEyeBoxMin.y = -50;
        g_camEyeBoxMin.z = -50;
        g_camEyeBoxMax.x = 50;
        g_camEyeBoxMax.y = 50;
        g_camEyeBoxMax.z = 50;
        /* cast kept: the settings are separate globals, copied as one 24-byte block */
        *(CameraSettingsCopy *)g_camSettingsBackup = *(CameraSettingsCopy *)&g_camSettings;
        for (index = 0; index < 13; index++)
            g_camModeParams[index] = g_camModeDefaults[index];
        Camera_InitOnceStub();
    }
    Camera_Reset();
    Camera_BuildViewMatrix(&g_camera, g_pZeroVec3s);
    Camera_ResetUnderwaterOverlay();
}
/* 0x55a308 */
void Camera_FreeLevel() {}
/* 0x55a30d */
void Camera_ReleaseAny()
{
    if (Camera_IsScriptControlled() || g_camMode == CAM_SAM_CHASE)
        Camera_SetMode(CAM_FOLLOW, 1);
}
/* 0x55a367 */
void Camera_ReleaseScripted(ScnObject *owner)
{
    if (Camera_ScriptOwner() == owner)
        Camera_SetMode(CAM_FOLLOW, 1);
}
/* 0x55a3d0 */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; unused08 fill gaps */
void Camera_Update(Camera *camera, u8 mode, Pad *pad, Pad *mouse)
{
    struct Work {
        u16 unused08;
        s16 step;
        Pad *unusedMouse;
    } w;
    w.unusedMouse = mouse;
    g_camStateFlags.all &= ~CAMS_ROT_NEG;
    g_camStateFlags.all &= ~CAMS_ROT_POS;
    g_camFlags2.all &= ~CAMF2_OVERHEAD;
    switch (mode) {
        case CAM_FOLLOW:
            if (!(g_gameFlags & GF_PAUSED)) {
                switch (g_camMode) {
                    case CAM_SAM_CHASE:
                        Camera_UpdateSamChase(camera, pad);
                        break;
                    case CAM_FOLLOW:
                        Camera_UpdateFollow(camera, pad);
                        break;
                    case CAM_FOLLOW_RUN:
                        Camera_UpdateFollow(camera, pad);
                        break;
                    case CAM_LOOK:
                        Camera_UpdateLook(camera, pad);
                        break;
                    case CAM_FIXED_LOOKAT:
                        Camera_UpdateFixedLookAt(camera);
                        break;
                    case CAM_ROCKET:
                        Camera_UpdateRocket(camera, pad);
                        break;
                    case CAM_DIRECTED:
                        Camera_UpdateDirected(camera, pad);
                        break;
                    case CAM_ROBOT:
                        Camera_UpdateFollow(camera, pad);
                        break;
                    case CAM_SCRIPTED:
                        Camera_UpdateScriptedHold(camera);
                        break;
                    case CAM_SCRIPT_RETURN:
                        Camera_UpdateScriptReturn(camera);
                        break;
                    case CAM_SCRIPT_BLEND_IN:
                        Camera_UpdateScriptBlendIn(camera);
                        break;
                    case CAM_SCRIPT_TO_SCRIPT:
                        Camera_UpdateScriptToScript(camera);
                        break;
                }
            }
            g_camRestriction = 0;
            g_camFlags.snap = 0;
            break;
        case CAM_FOLLOW_RUN:
            Camera_UpdateDebugFreeCam(camera, pad, mouse);
            break;
        default:
            w.step = g_dt * 960 >> 12;
            if (mode != CAM_LOOK) {
                if (!(pad->cur.buttons & ~(u16)~PAD_LEFT))
                    camera->pos.x -= w.step;
                if (!(pad->cur.buttons & ~(u16)~PAD_RIGHT))
                    camera->pos.x += w.step;
                if (!(pad->cur.buttons & ~(u16)~PAD_UP))
                    camera->pos.y -= w.step;
                if (!(pad->cur.buttons & ~(u16)~PAD_DOWN))
                    camera->pos.y += w.step;
            }
            if (pad->cur.typeLen.type == PADTYPE_ANALOG) {
                camera->pos.x += Camera_StickAxisDeadzone(pad->cur.leftX);
                camera->pos.z -= Camera_StickAxisDeadzone(pad->cur.leftY);
                camera->rot.x -= Camera_StickAxisDeadzone(pad->cur.rightY);
                camera->rot.y += Camera_StickAxisDeadzone(pad->cur.rightX);
            }
    }
}
/* 0x55a70d */
/* BYTES(dead-code): modeParams is computed and never used, as in the original */
void Camera_StartScripted(ScnObject *owner, Camera *camera, u16 pitch, u16 yaw, u16 roll, Vec3s *eye, u16 focalScale,
                          u32 flags, s32 blendTime)
{
    CamModeParams *modeParams = &g_camModeParams[7];
    s32 focal = (Camera_VirtualScreenWidth() * focalScale) >> 10;
    if (!Camera_IsScriptControlled()) {
        g_camPreScriptEye = camera->pos;
        g_camPreScriptRot = camera->rot;
        g_camPreScriptDist = camera->dist;
        g_camPreScriptFocal = (s16)Camera_GetProjection();
        if (g_camMode == CAM_ROBOT)
            g_camScriptReturnMode = CAM_ROBOT;
        else
            g_camScriptReturnMode = CAM_FOLLOW;
        g_camScriptFlags = flags;
        if (flags & CAMSCR_BLEND_IN)
            Camera_SetMode(CAM_SCRIPT_BLEND_IN, 1);
        else
            Camera_SetMode(CAM_SCRIPTED, 0);
    } else {
        g_camScriptFlags = flags;
        if (flags & CAMSCR_CHAIN) {
            g_camScriptPrevEye = camera->pos;
            g_camScriptPrevRot = camera->rot;
            g_camScriptPrevFocal = (s16)Camera_GetProjection();
            Camera_SetMode(CAM_SCRIPT_TO_SCRIPT, 1);
        }
    }
    g_camScriptOwner = owner;
    g_camScriptBlendTime = blendTime;
    g_camScriptEye = *eye;
    g_camScriptRot.x = pitch;
    g_camScriptRot.y = yaw;
    g_camScriptRot.z = roll;
    g_camScriptFocal = (s16)focal;
    g_screen.SetProjection(focal);
    camera->rot.x = pitch & 0xfff;
    camera->rot.y = yaw & 0xfff;
    camera->rot.z = roll & 0xfff;
    camera->pos = *eye;
    Camera_BuildViewMatrix(camera, g_pZeroVec3s);
}
/* 0x55a93e */
void Camera_SetSamChase(Vec3s *position, s16 height, CollBox *box)
{
    g_camChaseTarget2 = *position;
    g_camChaseEyeVert = height;
    if (box) {
        g_camChaseBox[0] = box->min.x;
        g_camChaseBox[1] = box->min.z;
        g_camChaseBox[2] = box->max.x;
        g_camChaseBox[3] = box->max.z;
    } else {
        g_camChaseBox[0] = -30000;
        g_camChaseBox[1] = -30000;
        g_camChaseBox[2] = 30000;
        g_camChaseBox[3] = 30000;
    }
    Camera_SetMode(CAM_SAM_CHASE, 1);
}
/* 0x55a9d2 */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; unused38, unused26, unused24, unused0c fill gaps */
/* BYTES(slot-scope, inferred): the nested block(s) only order the frame: their locals are allocated after the enclosing scope's */
/* BYTES(view): the 16-byte write's fourth word lands on planeD and is overwritten next, as in the original */
void Camera_BuildViewMatrix(Camera *camera, const Vec3s *offset)
{
    struct Work {
        s32 unused38[3];
        Vec3s eye;
        u16 unused26;
        s32 unused24[3];
        Vec3f transformed;
        s32 unused0c[3];
    } w;
    {
        float eyeXFloat, eyeYFloat, eyeZf;
        w.eye.x = camera->pos.x + offset->x;
        w.eye.y = camera->pos.y + offset->y;
        w.eye.z = camera->pos.z + offset->z;
        eyeXFloat = w.eye.x;
        eyeYFloat = w.eye.y;
        eyeZf = w.eye.z;
        camera->viewMat.SetRotYXZ(Math_Angle4096ToRadians_2(camera->rot.x), Math_Angle4096ToRadians_2(camera->rot.y),
                                  Math_Angle4096ToRadians_2(camera->rot.z));
        w.transformed.x = eyeXFloat * camera->viewMat.m[0][0] + eyeYFloat * camera->viewMat.m[1][0] +
                          eyeZf * camera->viewMat.m[2][0] + camera->viewMat.m[3][0];
        w.transformed.y = eyeXFloat * camera->viewMat.m[0][1] + eyeYFloat * camera->viewMat.m[1][1] +
                          eyeZf * camera->viewMat.m[2][1] + camera->viewMat.m[3][1];
        w.transformed.z = eyeXFloat * camera->viewMat.m[0][2] + eyeYFloat * camera->viewMat.m[1][2] +
                          eyeZf * camera->viewMat.m[2][2] + camera->viewMat.m[3][2];
        camera->viewMat.m[3][0] = -w.transformed.x;
        camera->viewMat.m[3][1] = -w.transformed.y;
        camera->viewMat.m[3][2] = -w.transformed.z;
        camera->viewMatCopy = camera->viewMat;
        Mat34s_FromEulerYXZ(&camera->rot, &camera->viewMatS);
        /* cast kept: the provider writes a Vec4i: its unused fourth word lands on planeD, which the next statement
         * overwrites, as in the executable */
        Mat34s_TransformVec3s_i(&camera->viewMatS, &w.eye, (Vec4i *)camera->viewPos);
        camera->planeD = camera->viewMatS.rot[6] * camera->pos.x + camera->viewMatS.rot[7] * camera->pos.y +
                         camera->viewMatS.rot[8] * camera->pos.z;
    }
}
/* 0x55abb7 */
void Color_ScaleClamp(const u8 *rgb, u8 *out, float scale, u8 maximum)
{
    u16 channel;
    channel = (u16)(rgb[0] * scale);
    if (channel > maximum)
        channel = maximum;
    out[0] = (u8)channel;
    channel = (u16)(rgb[1] * scale);
    if (channel > maximum)
        channel = maximum;
    out[1] = (u8)channel;
    channel = (u16)(rgb[2] * scale);
    if (channel > maximum)
        channel = maximum;
    out[2] = (u8)channel;
    out[3] = 0;
}
/* 0x55ac6b */
void Camera_ResetUnderwaterOverlay()
{
    g_uwOverlayReady = 0;
    g_uwOverlayTexPage = 0;
    g_uwOverlayColor = 0;
    g_uwOverlayUV[0] = g_uwOverlayUV[1] = g_uwOverlayUV[2] = g_uwOverlayUV[3] = 0;
}
/* 0x55acb7 */
void Camera_InitUnderwaterOverlay()
{
    DavBitmapRec *bitmap;
    u16 count;
    u16 *recordIndex;
    u32 *idList;
    if (!g_uwOverlayReady) {
        /* cast kept: Color_ScaleClamp works on a colour's bytes; both colours are u32 words everywhere else */
        Color_ScaleClamp((u8 *)&g_waterColor, (u8 *)&g_uwOverlayColor, 2.8f, 0xa0);
        idList = Res_GetValidatedIdList(DAV_IDI_IGLALFA_, &count);
        if (idList && count) {
            recordIndex = (u16 *)*idList; /* cast kept: an id list holds record pointers of any kind */
            bitmap = &g_pDav->header->dir->bitmaps[*recordIndex];
            g_uwOverlayTexPage = TexAtlas_GetPage(bitmap);
            g_uwOverlayUV[0] = Tex_CornerUV(0, bitmap->width - 1, bitmap->u);
            g_uwOverlayUV[1] = Tex_CornerUV(0, bitmap->height - 1, bitmap->v);
            g_uwOverlayUV[2] = Tex_CornerUV(bitmap->width - 1, bitmap->width - 1, bitmap->u);
            g_uwOverlayUV[3] = Tex_CornerUV(bitmap->height - 1, bitmap->height - 1, bitmap->v);
            g_uwOverlayReady = 1;
        }
    }
}
/* 0x55adec */
void Camera_UpdateUnderwater()
{
    float depth;
    if (Camera_WaterZones(0)->FindContaining(&g_camera.pos)) {
        Camera_InitUnderwaterOverlay();
        if (g_uwOverlayReady == 1) {
            depth = g_screen.Draw2D_LayerToZ(g_screenLayerBase + 6);
            Draw2D_TexRect(depth, 0, 0, g_pViewFrustum->viewportWidth, g_pViewFrustum->viewportHeight,
                           g_uwOverlayTexPage, g_uwOverlayUV[0], g_uwOverlayUV[1], g_uwOverlayColor, g_uwOverlayUV[0],
                           g_uwOverlayUV[3], g_uwOverlayColor, g_uwOverlayUV[2], g_uwOverlayUV[1], g_uwOverlayColor,
                           g_uwOverlayUV[2], g_uwOverlayUV[3], g_uwOverlayColor);
        }
        if (!g_camUnderwaterSnd || !Sound_IsPlaying(g_camUnderwaterSnd))
            g_camUnderwaterSnd = Sound_Play(SND_UNDERWATER, &g_camera, 255, SNDF_LOOP | SNDF_NO_RETRIGGER, 4096);
    } else if (g_camUnderwaterSnd) {
        Sound_Stop(g_camUnderwaterSnd, &g_camera);
        g_camUnderwaterSnd = 0;
    }
}
/* 0x55af38 */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; unused0a, unused02 fill gaps */
s32 Camera_IsPointOnScreen(Camera *camera, Vec3s angles, s16 distance, const Vec3s *aim, const Vec3s *point,
                           s32 *central)
{
    struct Work {
        Vec3s projected;
        u16 unused0a;
        Vec3s eye;
        u16 unused02;
    } w;
    *central = 1;
    Vec3s_OffsetAlongAngles(&w.eye, &angles, distance, aim);
    w.eye.x = camera->pos.x;
    w.eye.y = camera->pos.y;
    w.eye.z = camera->pos.z;
    angles = camera->rot;
    w.projected.x = point->x * camera->viewMatCopy.m[0][0] + point->y * camera->viewMatCopy.m[1][0] +
                    point->z * camera->viewMatCopy.m[2][0] + camera->viewMatCopy.m[3][0];
    w.projected.y = point->x * camera->viewMatCopy.m[0][1] + point->y * camera->viewMatCopy.m[1][1] +
                    point->z * camera->viewMatCopy.m[2][1] + camera->viewMatCopy.m[3][1];
    w.projected.z = point->x * camera->viewMatCopy.m[0][2] + point->y * camera->viewMatCopy.m[1][2] +
                    point->z * camera->viewMatCopy.m[2][2] + camera->viewMatCopy.m[3][2];
    if (w.projected.x < 0)
        return 0;
    if (w.projected.x > g_screenW)
        return 0;
    if (w.projected.y < 0)
        return 0;
    if (w.projected.y > g_screenH)
        return 0;
    if (((w.projected.x - (g_screenW >> 1)) >= 0 ? (w.projected.x - (g_screenW >> 1))
                                                 : -(w.projected.x - (g_screenW >> 1))) > (g_screenW >> 2))
        *central = 0;
    if (((w.projected.y - (g_screenH >> 1)) >= 0 ? (w.projected.y - (g_screenH >> 1))
                                                 : -(w.projected.y - (g_screenH >> 1))) > (g_screenH >> 2))
        *central = 0;
    return 1;
}
