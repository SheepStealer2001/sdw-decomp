/* match-init: Weather_StaticInit */
/* match-addr: srand=0x567b72 time=0x567ba1   (the CRT functions; their users are in T268, src/engine/weather_fx.cpp) */
/*
 * T267 - guessed original name: Weather.cpp. SheepD3D.exe .text 0x52df80-0x52e1d3, .rdata 0x577144-0x577148
 * (__real@44188000 = 610.0f, first used here), .bss 0x6d8318-0x6dd9b0 (g_weather, exactly).
 * The one weather object g_weather (class Weather, sizeof 0x5698) with its four static-initialiser thunks, and the
 * hooks the level loader and Game_Frame call on it: Weather_LevelInit, Weather_InitRain / _UpdateRain / _RenderRain,
 * Weather_InitSnow / _UpdateSnow / _RenderSnow. The Weather class itself (0x52e1e0-0x531eca: constructor, vertex
 * buffers, particles, drawing) is the next object, T268 (src/engine/weather_fx.cpp). Its static initialiser calls the
 * out-of-line Weather constructor 0x52e1e0 and the atexit thunk the destructor 0x52e259, both defined in T268, so
 * ??_7Weather is first emitted there.
 *
 * The level's WAR header says which weather it has (g_weatherType 0x6ddf6a: 1 rain, 2 snow); the loader then calls
 * Weather_InitRain or Weather_InitSnow, and Game_Frame calls Weather_RenderRain / Weather_RenderSnow after the scene.
 * Local names are chosen for their stack slots (tools/vc6_locals.py); the inline helpers (Camera::ViewDir,
 * Progress::CurrentLevel, HALF_WIDTH_AT) have no bodies of their own in the exe, so their names are not recovered.
 * Weather_InitRain / _InitSnow: see HALF_WIDTH_AT (a representation, not the original's text).
 */
/* BYTES: flow, inline. */
/* BYTES(inline): Camera::ViewDir (SDW_MEMBERS_Camera inline): source-only inline returning by value: the original has a 6-byte temp (0x52e08c) */
/* BYTES(inline): Progress::CurrentLevel (SDW_MEMBERS_Progress inline): source-only inline: the original has a byte temp (0x52e0f4) */
/* BYTES(flow): HALF_WIDTH_AT macro (Weather_InitRain, Weather_InitSnow): the ((void)0, ...) comma makes VC6 load g_pViewFrustum a second time after the fld (0x52dfd4-0x52dfe2); zero code, not the original's text */
/* BYTES(inline, inferred): Vec3f() {} (SDW_MEMBERS_Vec3f): empty user-declared constructor, kept identical to T268's Vec3f */
#include "sdw_types.h"
class Mat44;
struct IDirect3DVertexBuffer7;

#define SDW_MEMBERS_Vec3f \
    Vec3f() {}                               /* empty but user-declared, as in the Weather class's own file (T268) */
#define SDW_MEMBERS_Mat44 Mat44();           /* 0x4077f0 */
#define SDW_MEMBERS_RenderPoly RenderPoly(); /* 0x41aad0 RenderPoly_Ctor */


#define SDW_MEMBERS_Weather Weather(); /* 0x52e1e0 */
#include "sdw_classes.h"
#define SDW_INLINE_CAMERA_VIEWDIR 1
#include "../objects/camera_inlines.h"
#undef SDW_INLINE_CAMERA_VIEWDIR
#define SDW_INLINE_PROGRESS_CURRENTLEVEL 1
#include "progress_inlines.h"
#undef SDW_INLINE_PROGRESS_CURRENTLEVEL
#include "sdw_enums.h"
#include "scenaric_props.h"

#include "draw2d.h"
#include "progress.h"
#include "../app/app_main.h"

/* half the width of the view at distance d (tan(fov/2) / aspect * d): the half-width of the particle sphere's slice */
#define HALF_WIDTH_AT(d) (((void)0, g_pViewFrustum->tanHalfFov) / g_pViewFrustum->aspect * (d))

/* 0x6d8318: the one weather object. Its four static-initialiser thunks (0x52df80 Weather_StaticInit,
 * 0x52df8f Weather_StaticConstruct, 0x52df9e Weather_RegisterAtexit, 0x52dfb0 Weather_StaticDtor) are what VC6
 * generates for this definition. */
Weather g_weather;

/* 0x52dfbf - empty; the level loader calls it before the per-type setup. */
void Weather_LevelInit() {}

/* 0x52dfc4 - g_weatherType 1: 200 drops in a sphere of radius 1000, textures 0xe1 / 0xe9 (IGLGOUT1/2), falling
 * straight down (0, 1, 0) within 0.085 rad at 1400..1600 units/s. */
void Weather_InitRain(Vec3s *camPos)
{
    Vec3f dir;

    /* The left operand is wrapped in a comma expression: it must be an expression node, not a plain memory operand,
     * for VC6 to load g_pViewFrustum a second time after the fld (0x52dfd4-0x52dfe2) instead of loading both pointers
     * first. This is the only zero-code spelling found that does it (a representation, not the original's text). */
    g_weather.SetVolume(HALF_WIDTH_AT(610.0f), 1000.0f, 200);
    g_weather.SetTexture(DAV_IDI_IGLGOUT1, 16.0f, 32.0f, 0xffffff);
    g_weather.SetTexture2(DAV_IDI_IGLGOUT2, 32.0f, 32.0f, 0xffffff);
    dir.x = 0.0f;
    dir.y = 1.0f;
    dir.z = 0.0f;
    g_weather.SfxParticles_InitRandom(&dir, 0.085f, 1400.0f, 1600.0f);
}

/* 0x52e06b - empty; Game_Frame's pre-update hook for rain. */
void Weather_UpdateRain(Vec3s *camPos) {}

/* 0x52e070 - Game_Frame's rain hook after the scene: move the drops, then draw them against the view direction. */
void Weather_RenderRain(void *layer, Camera *cam)
{
    Vec3s nrm;
    float dirF[3];

    g_weather.Advance(&g_camera.viewMat);
    nrm = g_camera.ViewDir();
    dirF[0] = nrm.x;
    dirF[1] = nrm.y;
    dirF[2] = nrm.z;
    g_weather.DrawRain(&g_camera.viewMat, dirF);
}

/* 0x52e0e3 - g_weatherType 2: 300 flakes (80 on disc level 8) in a sphere of radius 1500, texture 0x20 (IGLNEIG1) for
 * both, drifting along (50, 250, -25) within 0.175 rad at 180..250 units/s. */
void Weather_InitSnow(Vec3s *camPos)
{
    u32 count;
    Vec3f dir;

    if (g_pProgress->CurrentLevel() == SCENE_LVL_08)
        count = 80;
    else
        count = 300;
    g_weather.SetVolume(HALF_WIDTH_AT(610.0f), 1500.0f, count); /* see Weather_InitRain */
    g_weather.SetTexture(DAV_IDI_IGLNEIG1, 24.0f, 24.0f, 0xffffff);
    g_weather.SetTexture2(DAV_IDI_IGLNEIG1, 24.0f, 24.0f, 0xffffff);
    dir.x = 50.0f;
    dir.y = 250.0f;
    dir.z = -25.0f;
    g_weather.SfxParticles_InitRandom(&dir, 0.175f, 180.0f, 250.0f);
}

/* 0x52e1ab - empty; Game_Frame's pre-update hook for snow. */
void Weather_UpdateSnow(Vec3s *camPos) {}

/* 0x52e1b0 - Game_Frame's snow hook after the scene. */
void Weather_RenderSnow(void *layer, Camera *cam)
{
    g_weather.Advance(&g_camera.viewMat);
    g_weather.DrawSnow(&g_camera.viewMat);
}
