/* Stand-in for Direct3D 7 (d3dtypes.h / d3dcaps.h of the DirectX SDK): the constants the decompiled source names, with the SDK's own spelling and value.
 * Only what src/ uses is here: constants, types and functions (depends on sdw_types.h). */
#ifndef SDW_SDK_D3D7_H
#define SDW_SDK_D3D7_H

typedef enum _D3DANTIALIASMODE {
    D3DANTIALIAS_NONE = 0,
    D3DANTIALIAS_SORTINDEPENDENT = 2,
} D3DANTIALIASMODE;

typedef enum _D3DBLEND {
    D3DBLEND_ZERO = 1,
    D3DBLEND_ONE = 2,
    D3DBLEND_SRCALPHA = 5,
    D3DBLEND_INVSRCALPHA = 6,
    D3DBLEND_DESTCOLOR = 9,
} D3DBLEND;

typedef enum _D3DCMPFUNC {
    D3DCMP_LESSEQUAL = 4,
} D3DCMPFUNC;

typedef enum _D3DCULL {
    D3DCULL_NONE = 1,
    D3DCULL_CW = 2,
    D3DCULL_CCW = 3,
} D3DCULL;

typedef enum _D3DFOGMODE {
    D3DFOG_NONE = 0,
} D3DFOGMODE;

typedef enum _D3DPRIMITIVETYPE {
    D3DPT_LINELIST = 2,
    D3DPT_TRIANGLELIST = 4,
} D3DPRIMITIVETYPE;

typedef enum _D3DRENDERSTATETYPE {
    D3DRENDERSTATE_ANTIALIAS = 2,
    D3DRENDERSTATE_ZENABLE = 7,
    D3DRENDERSTATE_ZWRITEENABLE = 14,
    D3DRENDERSTATE_ALPHATESTENABLE = 15,
    D3DRENDERSTATE_SRCBLEND = 19,
    D3DRENDERSTATE_DESTBLEND = 20,
    D3DRENDERSTATE_CULLMODE = 22,
    D3DRENDERSTATE_ALPHAREF = 24,
    D3DRENDERSTATE_ALPHAFUNC = 25,
    D3DRENDERSTATE_DITHERENABLE = 26,
    D3DRENDERSTATE_ALPHABLENDENABLE = 27,
    D3DRENDERSTATE_FOGENABLE = 28,
    D3DRENDERSTATE_SPECULARENABLE = 29,
    D3DRENDERSTATE_LIGHTING = 137,
    D3DRENDERSTATE_COLORVERTEX = 141,
    D3DRENDERSTATE_CLIPPLANEENABLE = 152,
} D3DRENDERSTATETYPE;

typedef enum _D3DTEXTUREMAGFILTER {
    D3DTFG_POINT = 1,
    D3DTFG_LINEAR = 2,
} D3DTEXTUREMAGFILTER;

typedef enum _D3DTEXTUREMINFILTER {
    D3DTFN_POINT = 1,
    D3DTFN_LINEAR = 2,
} D3DTEXTUREMINFILTER;

typedef enum _D3DTEXTUREOP {
    D3DTOP_DISABLE = 1,
    D3DTOP_SELECTARG1 = 2,
    D3DTOP_MODULATE = 4,
} D3DTEXTUREOP;

typedef enum _D3DTRANSFORMSTATETYPE {
    D3DTRANSFORMSTATE_WORLD = 1,
    D3DTRANSFORMSTATE_VIEW = 2,
    D3DTRANSFORMSTATE_PROJECTION = 3,
} D3DTRANSFORMSTATETYPE;

typedef enum _D3DTEXTURESTAGESTATETYPE {
    D3DTSS_COLOROP = 1,
    D3DTSS_COLORARG1 = 2,
    D3DTSS_COLORARG2 = 3,
    D3DTSS_TEXCOORDINDEX = 11,
    D3DTSS_MAGFILTER = 16,
    D3DTSS_MINFILTER = 17,
} D3DTEXTURESTAGESTATETYPE;

typedef enum _D3DZBUFFERTYPE {
    D3DZB_FALSE = 0,
    D3DZB_TRUE = 1,
} D3DZBUFFERTYPE;

#define D3DCLEAR_TARGET 0x00000001l
#define D3DCLEAR_ZBUFFER 0x00000002l
#define D3DDEVCAPS_HWRASTERIZATION 0x00080000L
#define D3DENUMRET_CANCEL DDENUMRET_CANCEL /* sdk/ddraw.h */
#define D3DENUMRET_OK DDENUMRET_OK
#define D3DFVF_DIFFUSE 0x040
#define D3DFVF_SPECULAR 0x080
#define D3DFVF_TEX1 0x100
#define D3DFVF_TLVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1)
#define D3DFVF_XYZ 0x002
#define D3DFVF_XYZRHW 0x004
#define D3DPTEXTURECAPS_POW2 0x00000002L
#define D3DPTEXTURECAPS_SQUAREONLY 0x00000020L
#define D3DPV_DONOTCOPYDATA (1 << 0)
#define D3DTA_DIFFUSE 0x00000000
#define D3DTA_TEXTURE 0x00000002
#define D3DVBCAPS_DONOTCLIP 0x00000001l
#define D3DVBCAPS_SYSTEMMEMORY 0x00000800l
#define D3DVOP_TRANSFORM (1 << 0)

#include "windef.h"

struct D3DDEVICEDESC7;
struct D3DVERTEXBUFFERDESC;
struct D3DVIEWPORT7;
struct DDPIXELFORMAT;
struct IDirect3DDevice7;
struct IDirect3DVertexBuffer7;
struct IDirectDrawSurface7;
class Mat44;

struct D3DPRIMCAPS { /* 0x38 bytes */
    DWORD dwSize, dwMiscCaps, dwRasterCaps, dwZCmpCaps, dwSrcBlendCaps, dwDestBlendCaps, dwAlphaCmpCaps, dwShadeCaps;
    DWORD dwTextureCaps, dwTextureFilterCaps, dwTextureBlendCaps, dwTextureAddressCaps, dwStippleWidth, dwStippleHeight;
};

struct D3DDEVICEDESC7 { /* 0xec bytes */
    DWORD dwDevCaps;
    D3DPRIMCAPS dpcLineCaps, dpcTriCaps;                                                /* +0x04, +0x3c */
    DWORD dwDeviceRenderBitDepth, dwDeviceZBufferBitDepth;                              /* +0x74 */
    DWORD dwMinTextureWidth, dwMinTextureHeight, dwMaxTextureWidth, dwMaxTextureHeight; /* +0x7c */
    DWORD dwMaxTextureRepeat, dwMaxTextureAspectRatio, dwMaxAnisotropy;
    float dvGuardBandLeft, dvGuardBandTop, dvGuardBandRight, dvGuardBandBottom, dvExtentsAdjust;
    DWORD dwStencilCaps, dwFVFCaps, dwTextureOpCaps;
    u16 wMaxTextureBlendStages, wMaxSimultaneousTextures;
    DWORD dwMaxActiveLights;
    float dvMaxVertexW;
    GUID deviceGUID; /* +0xc4 */
    u16 wMaxUserClipPlanes, wMaxVertexBlendMatrices;
    DWORD dwVertexProcessingCaps, dwReserved1, dwReserved2, dwReserved3, dwReserved4;
};

struct D3DVIEWPORT7 {
    DWORD dwX, dwY, dwWidth, dwHeight;
    float dvMinZ, dvMaxZ;
};

typedef HRESULT(__stdcall *LPD3DENUMDEVICESCALLBACK7)(char *description, char *name, D3DDEVICEDESC7 *desc,
                                                      void *context);

typedef HRESULT(__stdcall *LPD3DENUMPIXELFORMATSCALLBACK)(DDPIXELFORMAT *format, void *context);

struct D3DVERTEXBUFFERDESC {
    DWORD dwSize, dwCaps, dwFVF, dwNumVertices;
};

struct IDirect3DDevice7 : IUnknown {
    virtual HRESULT __stdcall GetCaps(D3DDEVICEDESC7 *desc) = 0;                                             /* +0x0c */
    virtual HRESULT __stdcall EnumTextureFormats(LPD3DENUMPIXELFORMATSCALLBACK callback, void *context) = 0; /* +0x10 */
    virtual HRESULT __stdcall BeginScene() = 0;                                                              /* +0x14 */
    virtual HRESULT __stdcall EndScene() = 0;                                                                /* +0x18 */
    virtual HRESULT __stdcall GetDirect3D(void **d3d) = 0;                                                   /* +0x1c */
    virtual HRESULT __stdcall SetRenderTarget(IDirectDrawSurface7 *surface, DWORD flags) = 0;                /* +0x20 */
    virtual HRESULT __stdcall GetRenderTarget(IDirectDrawSurface7 **surface) = 0;                            /* +0x24 */
    virtual HRESULT __stdcall Clear(DWORD count, void *rects, DWORD flags, DWORD color, float z,
                                    DWORD stencil) = 0;                          /* +0x28 */
    virtual HRESULT __stdcall SetTransform(DWORD state, void *matrix) = 0;       /* +0x2c */
    virtual HRESULT __stdcall GetTransform(DWORD state, void *matrix) = 0;       /* +0x30 */
    virtual HRESULT __stdcall SetViewport(D3DVIEWPORT7 *viewport) = 0;           /* +0x34 */
    virtual HRESULT __stdcall MultiplyTransform(DWORD state, Mat44 *matrix) = 0; /* +0x38 */
    virtual HRESULT __stdcall GetViewport(void *vp) = 0;                         /* +0x3c */
    virtual HRESULT __stdcall SetMaterial(void *mat) = 0;                        /* +0x40 */
    virtual HRESULT __stdcall GetMaterial(void *mat) = 0;                        /* +0x44 */
    virtual HRESULT __stdcall SetLight(DWORD index, void *light) = 0;            /* +0x48 */
    virtual HRESULT __stdcall GetLight(DWORD index, void *light) = 0;            /* +0x4c */
    virtual HRESULT __stdcall SetRenderState(DWORD state, DWORD value) = 0;      /* +0x50 */
    virtual HRESULT __stdcall GetRenderState(DWORD state, u32 *value) = 0;       /* +0x54 */
    virtual HRESULT __stdcall BeginStateBlock() = 0;                             /* +0x58 */
    virtual HRESULT __stdcall EndStateBlock(u32 *handle) = 0;                    /* +0x5c */
    virtual HRESULT __stdcall PreLoad(void *tex) = 0;                            /* +0x60 */
    virtual HRESULT __stdcall DrawPrimitive(DWORD type, DWORD fvf, void *verts, DWORD count,
                                            DWORD flags) = 0; /* +0x64 */
    virtual HRESULT __stdcall DrawIndexedPrimitive(DWORD type, DWORD fvf, void *verts, DWORD count, u16 *indices,
                                                   DWORD indexCount, DWORD flags) = 0; /* +0x68 */
    virtual HRESULT __stdcall SetClipStatus(void *cs) = 0;                             /* +0x6c */
    virtual HRESULT __stdcall GetClipStatus(void *cs) = 0;                             /* +0x70 */
    virtual HRESULT __stdcall DrawPrimitiveStrided(DWORD type, DWORD fvf, void *data, DWORD count,
                                                   DWORD flags) = 0; /* +0x74 */
    virtual HRESULT __stdcall DrawIndexedPrimitiveStrided(DWORD type, DWORD fvf, void *data, DWORD count, u16 *indices,
                                                          DWORD indexCount, DWORD flags) = 0; /* +0x78 */
    virtual HRESULT __stdcall DrawPrimitiveVB(DWORD type, IDirect3DVertexBuffer7 *vb, DWORD start, DWORD count,
                                              DWORD flags) = 0; /* +0x7c */
    virtual HRESULT __stdcall DrawIndexedPrimitiveVB(DWORD type, IDirect3DVertexBuffer7 *vb, DWORD start, DWORD count,
                                                     u16 *indices, DWORD indexCount, DWORD flags) = 0; /* +0x80 */
    virtual HRESULT __stdcall ComputeSphereVisibility(void *centers, float *radii, DWORD n, DWORD flags,
                                                      u32 *ret) = 0;                          /* +0x84 */
    virtual HRESULT __stdcall GetTexture(DWORD stage, void **tex) = 0;                        /* +0x88 */
    virtual HRESULT __stdcall SetTexture(DWORD stage, void *tex) = 0;                         /* +0x8c */
    virtual HRESULT __stdcall GetTextureStageState(DWORD stage, DWORD type, u32 *value) = 0;  /* +0x90 */
    virtual HRESULT __stdcall SetTextureStageState(DWORD stage, DWORD type, DWORD value) = 0; /* +0x94 */
};

struct IDirect3D7 : IUnknown {
    virtual HRESULT __stdcall EnumDevices(LPD3DENUMDEVICESCALLBACK7 callback, void *context) = 0; /* +0x0c */
    virtual HRESULT __stdcall CreateDevice(const GUID &rclsid, IDirectDrawSurface7 *surface,
                                           IDirect3DDevice7 **device) = 0; /* +0x10 */
    virtual HRESULT __stdcall CreateVertexBuffer(D3DVERTEXBUFFERDESC *desc, IDirect3DVertexBuffer7 **out,
                                                 DWORD flags) = 0; /* +0x14 */
    virtual HRESULT __stdcall EnumZBufferFormats(const GUID &rclsid, LPD3DENUMPIXELFORMATSCALLBACK callback,
                                                 void *context) = 0; /* +0x18 */
};

struct IDirect3DVertexBuffer7 : IUnknown {
    virtual HRESULT __stdcall Lock(DWORD flags, void **data, u32 *size) = 0; /* +0x0c */
    virtual HRESULT __stdcall Unlock() = 0;                                  /* +0x10 */
    virtual HRESULT __stdcall ProcessVertices(DWORD op, DWORD destIndex, DWORD count, IDirect3DVertexBuffer7 *src,
                                              DWORD srcIndex, IDirect3DDevice7 *device, DWORD flags) = 0; /* +0x14 */
};

SDW_AT(D3DDEVICEDESC7, dpcTriCaps, 0x3c);
SDW_AT(D3DDEVICEDESC7, dwDeviceRenderBitDepth, 0x74);
SDW_AT(D3DDEVICEDESC7, dwMaxTextureWidth, 0x84);
SDW_AT(D3DDEVICEDESC7, deviceGUID, 0xc4);
SDW_SIZE(D3DDEVICEDESC7, 0xec);

extern "C" const GUID IID_IDirect3D7;            /* 0x577728 */
extern "C" const GUID IID_IDirect3DHALDevice;    /* 0x577718 */
extern "C" const GUID IID_IDirect3DTnLHalDevice; /* 0x577708 */

#endif
