#ifndef SDW_APP_APP_MAIN_H
#define SDW_APP_APP_MAIN_H

/* The functions and globals app_main.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

class Camera;
struct CollTri;
class D3DApp;
class ScnObject;
class WorldObj;

extern void *g_animNameTable; /* 0x585050 WAR type 0x86 */
extern Camera g_camera;       /* 0x584c50 */
extern u16 g_cineObjectCount; /* 0x585034 class-2 resources (cinematic objects) */
extern ScnObject *
    *g_cineObjects;        /* 0x585060 the cinematic-only objects (WAR kind 0x0A), indexed by selector-3/4 tracks */
extern u16 *g_pWarCollMap; /* 0x585048 WAR type 0x80: header, then one CollCell per cell */
extern CollTri *g_pWarCollTris;  /* 0x58504c WAR type 0x81 */
extern ScnObject **g_scnActive;  /* 0x585058 */
extern u16 g_scnActiveBaseCount; /* 0x58502e */
extern u16 g_scnActiveCapacity;  /* 0x585036 scenaric object count + 10 */
extern u16 g_scnActiveHigh;      /* 0x585032 high-water mark of g_scnActive */
extern u16 g_scnObjectCount;     /* 0x585030 class-1 resources (scenaric objects) */
extern ScnObject **g_scnObjects; /* 0x58505c */
extern u16 g_worldObjCount;      /* 0x58502c class-0 resources (meshes) */
extern WorldObj **g_worldObjs;   /* 0x585054 */
u8 App_EnsureDirectX();
u16 App_GetLanguageMask();
s32 App_InitGameSystems();
void App_Shutdown();
void Launcher_OpenUrl(const char *, u16);
void Main_Loop(D3DApp *app);

/* The functions and globals app_main.cpp defines, declared once for every file that uses them. */

struct Dav;

extern Dav g_levelDav; /* 0x584fc0 the level's Dav */

#endif
