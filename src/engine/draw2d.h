#ifndef SDW_ENGINE_DRAW2D_H
#define SDW_ENGINE_DRAW2D_H

/* The functions and globals draw2d.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

class D3DApp;
class Frustrum;
class InputMgr;
class Mat44;
class PolyBatcher;
class SoundDevice;
class TextResBank;

extern char g_dirBonusGame[];       /* 0x6d5328 exeDir + ".\Bonus\" */
extern char g_dirReference[];       /* 0x6d6cb8 */
extern InputMgr g_inputMgr;         /* 0x6d6780 */
extern char g_introDir[];           /* 0x6d6578 */
extern char g_levelPathFmt[];       /* 0x6d6bb8 exeDir + ".\Levels\Lvl-%02d\Lvl-%02d" */
extern Mat44 g_matUnk6d5428;        /* 0x6d5428 */
extern u32 g_maxImmediateTriangles; /* 0x6d6678 */
extern char g_musicsPath[];         /* 0x6d5220 */
extern D3DApp *g_pD3DAppMain;       /* 0x6d6568 */
extern PolyBatcher *g_pPolyBin;     /* 0x6d6ec0 */
extern SoundDevice *g_pSoundSystem; /* 0x6d5320 */
extern Frustrum *g_pViewFrustum;    /* 0x6d6ebc */
extern char g_pathDemoDir[];        /* 0x6d6ab8 */
extern char g_pathEnding[];         /* 0x6d69b8 */
extern char g_pathFendDir[];        /* 0x6d6db8 */
extern char g_pathScene[];          /* 0x6d6468 */
extern char g_pathWheelDir[];       /* 0x6d6680 */
extern Mat44 g_projMatrix;          /* 0x6d6ec8 the projection matrix */
extern TextResBank g_textCatalog;   /* 0x6d656c */
extern char g_voiceDir[];           /* 0x6d68b8 */
void Draw2D_FlatRect(float z, float x0, float y0, float x1, float y1, u32 color, u8 blendMode); /* 0x522f45 */
void Draw2D_FlatRect_Immediate(float z, float x0, float y0, float x1, float y1, u32 renderFlags,
                               u32 color); /* 0x524c82 */
void Draw2D_FlatTri(float z, float x0, float y0, float x1, float y1, float x2, float y2, u32 color,
                    u8 blendMode); /* 0x522a65 */
void Draw2D_GouraudRect(float z, float x0, float y0, float x1, float y1, u32 cTL, u32 cBL, u32 cTR, u32 cBR,
                        u8 blendMode); /* 0x52390e */

#endif
