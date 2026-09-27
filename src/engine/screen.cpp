/* match-init: Screen_StaticInit */
/*
 * T262 - guessed original name: Screen.cpp. SheepD3D.exe .text 0x52a4c0-0x52a7da plus the COMDAT
 * Screen_ScalarDeletingDtor 0x52a7e0-0x52a80e, .rdata 0x57713c-0x577144 (??_7Screen, then __real@42f00000 = 120.0f,
 * first used here), .bss 0x6d6fe0-0x6d7070 (g_screen; g_mirrorRender*, g_screenW/H and g_projFocalScale are its fields).
 * The virtual screen (512 x 240, the PS1 resolution): its static instance and the four static-initialiser thunks, the
 * constructor / destructor and the projection (0x52a4c0-0x52a80b).
 *
 * The inline helper D3DApp::SetTransform has no out-of-line copy in the exe, so its name is not recovered; it is here
 * because its expansion gives the original's shape (the device pointer reloaded through a temp). A shape that
 * reproduces the bytes is a representation, not proof that the original source read this way.
 */
/* BYTES: inline. */
/* BYTES(inline): D3DApp::SetTransform (source-only inline): source-only inline: its expansion reloads the device pointer through a temp, as the original does */
/* BYTES(inline): Screen::Layers4 / Layers60 (source-only inline): source-only inline: its expansion materialises the constant index in eax (0x529fbd) */
#include "sdw_types.h"
#include "../sdk/d3d7.h"
class Mat44;

#include "../sdk/crt.h"

#define SDW_MEMBERS_Screen                    \
    Screen(); /* 0x52a4ff Screen_Construct */ \
    SamScreenGeometry *GetGeometry(SamScreenGeometry *out);

#define SDW_MEMBERS_D3DApp void SetTransform(u32 state, Mat44 *m); /* inline */
struct SamScreenGeometry {
    unsigned short width, height, x, y, aspect;
}; /* as src/objects/sam.cpp */
#include "sdw_classes.h"
#define SDW_INLINE_SCREEN_LAYERS4_U16 1
#define SDW_INLINE_SCREEN_LAYERS60_U16 1
#include "screen_inlines.h"
#undef SDW_INLINE_SCREEN_LAYERS4_U16
#undef SDW_INLINE_SCREEN_LAYERS60_U16
#define SDW_INLINE_D3DAPP_SETTRANSFORM_U32_MAT44 1
#include "../app/d3dapp_inlines.h"
#undef SDW_INLINE_D3DAPP_SETTRANSFORM_U32_MAT44

#include "fixed_math.h"
#include "draw2d.h"

/* ---- globals ---- */
extern u32 *g_screenLayerBase; /* 0x585044 */

/* ---- the virtual screen (512 x 240, the PS1 resolution) ---- */
Screen g_screen; /* 0x6d6fe0 */

/* 0x52a4ff */
Screen::Screen()
{
    scratch4 = 0;
    scratch60 = 0;
}

/* 0x52a530 */
Screen::~Screen()
{
    if (scratch4)
        free(scratch4);
    if (scratch60)
        free(scratch60);
}

/* 0x52a580 - once the device is up: viewport from the frustum's size, virtual screen 512 x 240, projection distance
 * 384, and the two layer-handle tables. 0 when the device is not ready. */
s32 Screen::Init(s32 unused)
{
    if (!g_pD3DAppMain->deviceReady)
        return 0;
    viewportY = 0;
    viewportX = 0;
    viewportWidth = (s16)g_pViewFrustum->viewportWidth;
    viewportHeight = (s16)g_pViewFrustum->viewportHeight;
    virtWidth = 0x200;
    virtHeight = 0xf0;
    SetProjection(0x180);
    scratch4 = malloc(4);
    scratch60 = malloc(0x60);
    return 1;
}

/* 0x52a626 - a virtual-screen x in viewport pixels. */
float Screen::ScaleX(s32 x)
{
    return x * g_pViewFrustum->viewportWidth / 512.0f;
}

/* 0x52a644 - a virtual-screen y in viewport pixels. */
float Screen::ScaleY(s32 y)
{
    return y * g_pViewFrustum->viewportHeight / 240.0f;
}

/* 0x52a662 - the index of a layer handle (g_screenLayerBase + index). */
u16 Screen::LayerIndex(u32 *layer)
{
    /* cast kept (both): the original subtracts the addresses unsigned (shr); a pointer difference divides signed (sar) */
    return (u16)(((u32)layer - (u32)g_screenLayerBase) >> 2);
}

/* 0x52a67b - the depth of a 2D layer: index / 120. */
float Screen::Draw2D_LayerToZ(u32 *layer)
{
    return LayerIndex(layer) / 120.0f;
}

/* 0x52a6a7 - the virtual screen's size, origin 0,0 and 1/aspect in 4.12. */
SamScreenGeometry *Screen::GetGeometry(SamScreenGeometry *out)
{
    SamScreenGeometry geo;
    geo.width = 0x200;
    geo.height = 0xf0;
    geo.x = geo.y = 0;
    geo.aspect = Math_FloatToFixed12_s16(1.0f / g_pViewFrustum->aspect);
    *out = geo;
    return out;
}

/* 0x52a708 - clears target and z-buffer to color. */
void Screen::Clear(u32 color)
{
    D3DApp *app = g_pD3DAppMain;
    app->pD3DDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, color, 1.0f, 0);
}

/* 0x52a742 - the PS1-style projection distance: horizontal fov from tan = 256 / dist, aspect = width / height, and
 * the projection matrix (y flipped) pushed to the device. */
void Screen::SetProjection(s32 dist)
{
    float tanHalf;
    projDist = dist;
    tanHalf = 512.0f / (projDist * 2.0f);
    g_pViewFrustum->SetFovFromTan(tanHalf);
    g_pViewFrustum->aspect = g_pViewFrustum->viewportWidth / g_pViewFrustum->viewportHeight;
    g_pViewFrustum->BuildProjectionMatrix(&g_projMatrix);
    g_projMatrix.m[1][1] = -g_projMatrix.m[1][1];
    g_pD3DAppMain->SetTransform(D3DTRANSFORMSTATE_PROJECTION, &g_projMatrix);
}
