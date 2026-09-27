/* match-addr: _tan=0x566d54 _atan=0x566e14 _exp=0x566ea0   the CRT's tan / atan / exp (fptan, fpatan, the "exp" name
 * string 0x581e3a at 0x566ea0), which the tables do not name. */
/* BYTES: cast, inline. */
/* BYTES(inline): D3DApp::Render_SetStateFlags (inline twin in SDW_MEMBERS_D3DApp): source-only inline twin of Render_SetStateFlags 0x4155f0: expanded here with the constant flags materialised per test */
/* BYTES(cast): D3DApp::SetFogStates (inline): modeState is enum-typed because the original copies the constant 0x8c (D3DRENDERSTATE_FOGVERTEXMODE) to a stack temp */
/*
 * T015 - original object Frustrum.cpp (guessed name): SheepD3D.exe .text 0x418ad0-0x41a034 plus the COMDAT
 * ??_GFrustrum 0x41a040-0x41a06e, .rdata 0x5743a8-0x5743c4 (the double 2.0, Frustrum's vtable, 255.0f, 768.0, 256.0f),
 * .data 0x579448-0x5794b0 (the constructor's four message-box strings). MeshPart, the small constructor/destructor
 * objects and RenderPoly that follow are T016-T026, each its own file.
 *
 *   0x418ad0-0x41a06b  Frustrum: projection (near / far / horizontal fov, the viewport aspect), the draw distance the
 *                      fog band hangs from, the D3D fog render states and the 256-entry vertex-fog ramp.
 *
 * Inline helpers (they have no bodies in the exe, so their names are not recovered):
 *  - D3DApp::Render_SetStateFlags: the inline twin of 0x4155f0 (as src/fx/holefx.cpp), expanded with the constant
 *    flags materialised per test; Render_ClearStateFlags stays the out-of-line 0x4159b0 (EnableFog).
 *  - D3DApp::SetFogStates / SetFogColorState: the fog render-state pushes. The fog-mode state is an enum-typed
 *    parameter, which is why the constant 0x8c (D3DRENDERSTATE_FOGVERTEXMODE) is copied to a stack temp.
 * Local names are chosen for their stack slots (tools/vc6_locals.py). A shape that reproduces the bytes is a
 * representation, not proof that the original source read this way.
 */
#include "sdw_types.h"

class Mat44;
#include "sdw_enums.h"
#include "../sdk/win32.h"
#include "../sdk/d3d7.h"
#include "../sdk/crt.h"

/* D3DRENDERSTATETYPE, as far as it is used here */
enum D3DRenderState { D3DRS_FOGCOLOR = 0x22, D3DRS_FOGSTART = 0x24, D3DRS_FOGEND = 0x25, D3DRS_FOGVERTEXMODE = 0x8c };

#define SDW_MEMBERS_Mat44 Mat44(); /* 0x4077f0 */
#define SDW_MEMBERS_D3DApp                                                                           \
    void Render_ClearStateFlags(u32 flags); /* 0x4159b0 */                                           \
    /* inline: the fog colour, the fog mode (none), the fog start and end */                         \
    void SetFogStates(u32 color, D3DRenderState modeState, float start, float end)                   \
    {                                                                                                \
        pD3DDevice->SetRenderState(D3DRS_FOGCOLOR, color);                                           \
        pD3DDevice->SetRenderState(modeState, D3DFOG_NONE);                                          \
        /* cast kept: SetRenderState takes a float state's bits as a DWORD (the SDK's convention) */ \
        pD3DDevice->SetRenderState(D3DRS_FOGSTART, *(u32 *)&start);                                  \
        pD3DDevice->SetRenderState(D3DRS_FOGEND, *(u32 *)&end);                                      \
    }                                                                                                \
    /* inline: the fog colour alone */                                                               \
    void SetFogColorState(u32 color)                                                                 \
    {                                                                                                \
        pD3DDevice->SetRenderState(D3DRS_FOGCOLOR, color);                                           \
    }                                                                                                \
    /* inline twin of Render_SetStateFlags 0x4155f0 (as src/fx/holefx.cpp) */                        \
    void Render_SetStateFlags(u32 flags)                                                             \
    {                                                                                                \
        if (flags & RSF_ANTIALIAS)                                                                   \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_ANTIALIAS, D3DANTIALIAS_SORTINDEPENDENT);      \
        if (flags & RSF_BLEND_ALPHA) {                                                               \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);                       \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_INVSRCALPHA);               \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_SRCALPHA);                 \
        }                                                                                            \
        if (flags & RSF_BLEND_ADD) {                                                                 \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);                       \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_INVSRCALPHA);               \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);                      \
        }                                                                                            \
        if (flags & RSF_ALPHATEST) {                                                                 \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, TRUE);                        \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHAREF, 8);                                  \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHAFUNC, D3DCMP_LESSEQUAL);                  \
        }                                                                                            \
        if (flags & RSF_CLIPPLANE)                                                                   \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_CLIPPLANEENABLE, TRUE);                        \
        if (flags & RSF_DITHER)                                                                      \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_DITHERENABLE, TRUE);                           \
        if (flags & RSF_LIGHTING)                                                                    \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_LIGHTING, TRUE);                               \
        if (flags & RSF_SPECULAR)                                                                    \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_SPECULARENABLE, TRUE);                         \
        if (flags & RSF_COLORVERTEX)                                                                 \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_COLORVERTEX, TRUE);                            \
        if (flags & RSF_CULL_CW)                                                                     \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_CW);                         \
        if (flags & RSF_CULL_CCW)                                                                    \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_CCW);                        \
        if (flags & RSF_ZTEST)                                                                       \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, D3DZB_TRUE);                          \
        if (flags & RSF_ZWRITE_ON)                                                                   \
            if (pD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, TRUE))                       \
                pD3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, TRUE);                            \
        if (flags & RSF_ZWRITE_OFF)                                                                  \
            if (pD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, FALSE))                      \
                pD3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, TRUE);                            \
        if (flags & RSF_TEXTURED) {                                                                  \
            pD3DDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);                            \
            pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);                    \
            pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);                    \
            pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);                    \
        } else {                                                                                     \
            pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);                    \
            pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);                  \
        }                                                                                            \
        if (flags & RSF_FILTER_LINEAR) {                                                             \
            pD3DDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_LINEAR);                    \
            pD3DDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFN_LINEAR);                    \
        }                                                                                            \
        if (flags & RSF_FOG)                                                                         \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_FOGENABLE, TRUE);                              \
    }
#define SDW_MEMBERS_Frustrum Frustrum(D3DApp *app); /* 0x418ad0 */
#include "sdw_classes.h"

/* ======================================================================== Frustrum */

/* 0x418ad0 - reads the viewport from the D3DApp (which must already be initialised: otherwise two message boxes and
 * exit), a 60-degree horizontal fov, near 1 / far 100, linear fog 1..1 with colour 0 pushed to the device. */
Frustrum::Frustrum(D3DApp *app)
{
    if (app->deviceReady == 1) {
        d3dApp = app;
        viewportWidth = (float)(d3dApp->clientRect.right - d3dApp->clientRect.left);
        viewportHeight = (float)(d3dApp->clientRect.bottom - d3dApp->clientRect.top);
    } else {
        MessageBoxA(0, "Frustrum Error : Core is not ready", "BSHEEP_Frustrum ERROR", MB_OK);
        MessageBoxA(0, "Check init order", "BSHEEP_Frustrum ERROR", MB_OK);
        exit(0);
    }
    cullDisabled = 0;
    flipWinding = 1;
    aspect = viewportHeight / viewportWidth;
    fovRad = 1.0471976f;
    tanHalfFov = (float)tan(fovRad / 2.0);
    nearZ = 1.0f;
    viewDistance = 100.0f;
    farZ = 100.0f;
    fogEnabled = 0;
    fogColor = 0;
    fogMode = FOG_LINEAR;
    fogStart = 1.0f;
    fogEnd = 1.0f;
    d3dApp->SetFogStates(fogColor, D3DRS_FOGVERTEXMODE, fogStart, fogEnd);
    BuildFogRamp();
}

/* 0x418c97 */
Frustrum::~Frustrum()
{
    d3dApp = 0;
}

/* 0x418cb5 */
void Frustrum::SetProjection(float nearZ, float farZ, float fovRad, float viewDistance)
{
    this->nearZ = nearZ;
    this->farZ = farZ;
    this->fovRad = fovRad;
    tanHalfFov = (float)tan(this->fovRad / 2.0);
    this->viewDistance = viewDistance;
}

/* 0x418d06 */
void Frustrum::SetFov(float fovRad)
{
    this->fovRad = fovRad;
    tanHalfFov = (float)tan(this->fovRad / 2.0);
}

/* 0x418d3c - the inverse: fov = 2 atan(tanHalfFov). */
void Frustrum::SetFovFromTan(float tanHalfFov)
{
    this->tanHalfFov = tanHalfFov;
    fovRad = (float)(atan(this->tanHalfFov) * 2.0);
}

/* 0x418d6e - fog band given by its two ends; enable = 1 also switches D3D fog on (it never switches it off). */
void Frustrum::SetFogStartEnd(u8 enable, u32 color, s32 mode, float fogStart, float fogEnd)
{
    fogColor = color & 0xffffff;
    fogMode = mode;
    this->fogStart = fogStart;
    this->fogEnd = fogEnd;
    d3dApp->SetFogStates(fogColor, D3DRS_FOGVERTEXMODE, this->fogStart, this->fogEnd);
    BuildFogRamp();
    if (enable == 1) {
        fogEnabled = 1;
        d3dApp->Render_SetStateFlags(RSF_FOG);
    }
}

/* 0x419223 - fog band of the given depth ending at the view distance. */
void Frustrum::SetFog(u8 enable, u32 color, s32 mode, float range)
{
    fogColor = color & 0xffffff;
    fogMode = mode;
    fogStart = viewDistance - range;
    fogEnd = viewDistance;
    d3dApp->SetFogStates(fogColor, D3DRS_FOGVERTEXMODE, fogStart, fogEnd);
    BuildFogRamp();
    if (enable == 1) {
        fogEnabled = 1;
        d3dApp->Render_SetStateFlags(RSF_FOG);
    }
}

/* 0x4196dd */
void Frustrum::SetFogColor(u32 color)
{
    fogColor = color;
    d3dApp->SetFogColorState(fogColor);
}

/* 0x41971f - on: RSF_FOG through the inline state setter (which also resets texture stage 0); off: the out-of-line
 * Render_ClearStateFlags. */
void Frustrum::EnableFog(u8 enable)
{
    fogEnabled = enable;
    if (fogEnabled == 1)
        d3dApp->Render_SetStateFlags(RSF_FOG);
    else
        d3dApp->Render_ClearStateFlags(RSF_FOG);
}

/* 0x419b2b */
void Frustrum::GetFogRangeRaw(float *start, float *end)
{
    *start = fogStart;
    *end = fogEnd;
}

/* 0x419b4e - the fog band read as 0..1 positions between near and far. */
void Frustrum::GetFogRangeWorld(float *start, float *end)
{
    *start = DenormalizeDepth(fogStart);
    *end = DenormalizeDepth(fogEnd);
}

/* 0x419b83 */
void Frustrum::BuildProjectionMatrix(Mat44 *dest)
{
    dest->SetPerspective(fovRad, aspect, nearZ, farZ);
}

/* 0x419bb4 - moveFog = 1 slides the fog band (keeping its depth) so that it ends at the new distance. */
void Frustrum::SetViewDistance(float distance, u8 moveFog)
{
    viewDistance = distance;
    if (moveFog == 1) {
        float depth = fogEnd - fogStart;
        fogStart = viewDistance - depth;
        fogEnd = viewDistance;
        d3dApp->SetFogStates(fogColor, D3DRS_FOGVERTEXMODE, fogStart, fogEnd);
        BuildFogRamp();
    }
}

/* 0x419c96 */
void Frustrum::SetViewDistanceNormalized(float t, u8 moveFog)
{
    viewDistance = DenormalizeDepth(t);
    if (moveFog == 1) {
        float depth = fogEnd - fogStart;
        fogStart = viewDistance - depth;
        fogEnd = viewDistance;
        d3dApp->SetFogStates(fogColor, D3DRS_FOGVERTEXMODE, fogStart, fogEnd);
        BuildFogRamp();
    }
}

/* 0x419d81 */
float Frustrum::GetViewDistanceNormalized()
{
    return (viewDistance - nearZ) / (farZ - nearZ);
}

/* 0x419da6 - the vertex fog factor (top byte) for a depth: 0xff000000 = none, 0 = full fog. */
u32 Frustrum::GetFogValue(float z, u8 useRawRange)
{
    if (fogEnabled == 1) {
        float bandStart;
        float bandEnd;
        s32 idx;
        if (useRawRange == 1) {
            bandStart = fogStart;
            bandEnd = fogEnd;
        } else {
            GetFogRangeWorld(&bandStart, &bandEnd);
        }
        if (z < bandStart)
            return 0xff000000;
        if (z > bandEnd)
            return 0;
        idx = (s32)((z - bandStart) / (bandEnd - bandStart) * 255.0f);
        return fogRamp[idx];
    }
    return 0xff000000;
}

/* 0x419e40 */
float Frustrum::NormalizeDepth(float z)
{
    return (z - nearZ) / (farZ - nearZ);
}

/* 0x419e64 */
float Frustrum::DenormalizeDepth(float t)
{
    return (farZ - nearZ) * t + nearZ;
}

/* 0x419e86 - fogScale and the 256-entry ramp: 1 = 1/exp(d), 2 = 1/exp(d*d) with d = (i+1)/768, 3 = linear. */
void Frustrum::BuildFogRamp()
{
    double dd;
    double d;
    u32 idx;
    fogScale = 255.0f / (fogEnd - fogStart);
    switch (fogMode) {
        case FOG_LINEAR:
            for (idx = 0; idx < 0x100; idx++)
                fogRamp[idx] = (u32)((1.0f - (idx + 1) / 256.0f) * 255.0f) << 24;
            break;
        case FOG_EXP:
            for (idx = 0; idx < 0x100; idx++) {
                d = (idx + 1) / 768.0;
                fogRamp[idx] = (u32)(1.0f / (float)exp(d) * 255.0f) << 24;
            }
            break;
        case FOG_EXP2:
            for (idx = 0; idx < 0x100; idx++) {
                dd = (dd = (idx + 1) / 768.0) * dd;
                fogRamp[idx] = (u32)(1.0f / (float)exp(dd) * 255.0f) << 24;
            }
            break;
        default:
            for (idx = 0; idx < 0x100; idx++)
                fogRamp[idx] = 0xff000000;
            break;
    }
}
