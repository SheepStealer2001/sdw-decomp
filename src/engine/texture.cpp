/*
 * Object T009 (data/tu_map.json), guessed original file Texture.cpp.
 *   .text 0x40a140-0x40b028, then the COMDAT ??_GTexture 0x40b030-0x40b05e
 *   .rdata 0x574340-0x574348 (the vtable + the pad before the Timer's double)
 *   .bss 0x5c7aa0-0x5c7aa8 (g_texRequestedFormat + pad)
 * The two objects before it are T007 (src/engine/mat44.cpp) and T008, the mouse (src/engine/mouse.cpp): the int3 padding
 * at 0x409e88 and 0x40a13e closes each of them, after its compiler-generated deleting destructor.
 */
/* BYTES: dead-code, slot-name. */
/* BYTES(slot-name): Texture::Texture (from .bmp): hbm is declared before pDDraw and hDCDest before hResult: same hash buckets, so the order decides the slots */
/* BYTES(dead-code): Texture::Texture (converting): unused is never used: it fills the untouched dword the original frame has at -0x8 */
/* BYTES(slot-name): Texture::Texture (converting): declaration order chosen inside the hash buckets (tools/vc6_locals.py) */
/* match-addr: g_texRequestedFormat=0x5c7aa0 */
/* match-addr: ??0Texture@@QAE@PAVD3DApp@@IIIPAH@Z=0x40a140 ??0Texture@@QAE@PBDPAVD3DApp@@IIPAH@Z=0x40a462 ??0Texture@@QAE@PBDPAVD3DApp@@IIIPAH@Z=0x40a8b9
   the three constructor overloads: their decorated names pin them, since the undecorated Texture_Ctor cannot tell them apart */
/*
 * The Texture object: an IDirectDrawSurface7 texture page, created blank, from a .bmp file, or from a .bmp file converted
 * to a chosen 16-bit pixel format, with its accessors and Lock / Unlock wrappers.
 *
 * The first match-addr line places g_texRequestedFormat 0x5c7aa0, which is read and written only here.
 *
 * The Texture constructors follow the DirectX 7 SDK's d3dtextr.cpp (TextureContainer::CreateFromBitmap): the device's
 * caps decide power-of-two and square sizes, a HAL / T&L HAL device gets a managed texture and anything else a
 * system-memory one, and IDirect3DDevice7::EnumTextureFormats picks the pixel format through
 * Texture_EnumPixelFormatCallback, which reads the wanted format index from g_texRequestedFormat:
 * 0 = R5G6B5, 1 = A1R5G5B5, 2 = A4R4G4B4. Result codes written to *result: 0 ok, 1 no device, 2 bitmap not loaded,
 * 3 no matching pixel format (or unknown format index), 4 out of memory creating the surface, 5 no DC.
 *
 * Local names were chosen for their /Od stack slots (src/README.md, tools/vc6_locals.py): plausible, not recovered.
 * The 0xec-byte D3DDEVICEDESC7 local gets an 8-aligned slot (-0xf0 as the first local, -0xf8 after a dword pair); the
 * third constructor has an untouched dword at -0x8, reproduced by an unused local.
 *
 * Defects visible here (no in-game consequence established): the .bmp constructor never DeleteObject's the DIB section
 * LoadImageA returns, and its GetDC failure path Releases the surface without clearing `surface`, so the destructor
 * Releases it a second time; every early return after GetDDInterface leaks the IDirectDraw7 reference. The converting
 * constructor sets up nothing when the temporary load fails (*result != 0): `surface` is left uninitialised for the
 * destructor, and the temporary Texture is leaked, as are it and its lock on the later early returns.
 */

#include "sdw_enums.h"
#include "../sdk/win32.h"
#include "../sdk/ddraw.h"
#include "../sdk/d3d7.h"
#include "../sdk/crt.h"

struct IDirectInputDevice8A;

#define SDW_MEMBERS_Texture                                                                                    \
    Texture(D3DApp *app, u32 width, u32 height, u32 format, s32 *result);                       /* 0x40a140 */ \
    Texture(const char *fileName, D3DApp *app, u32 width, u32 height, s32 *result);             /* 0x40a462 */ \
    Texture(const char *fileName, D3DApp *app, u32 width, u32 height, u32 format, s32 *result); /* 0x40a8b9 */ \
    void Surface_LockForRead(DDSURFACEDESC2 *desc);                                             /* 0x40ae0a */ \
    long Surface_LockForWrite(DDSURFACEDESC2 *desc);                                            /* 0x40ae3f */ \
    void Surface_LockReadWrite(DDSURFACEDESC2 *desc);                                           /* 0x40ae74 */
#include "sdw_classes.h"

/* ======================================================================== Texture (0x40a140-0x40b05f) */

/* the SDK's C++ inline (guiddef.h), as in src/app/d3dapp.cpp */
inline int IsEqualGUID(const GUID &rguid1, const GUID &rguid2)
{
    return !memcmp(&rguid1, &rguid2, sizeof(GUID));
}

/* 0x5c7aa0 - the pixel-format index the constructors ask Texture_EnumPixelFormatCallback for (0 R5G6B5, 1 A1R5G5B5,
 * 2 A4R4G4B4). Only this file uses it. */
u32 g_texRequestedFormat;

HRESULT __stdcall Texture_EnumPixelFormatCallback(DDPIXELFORMAT *pddpf, void *pOutPixelFormat);
u16 Texture_CountMaskBits(u32 mask);

/* cast kept: Texture.desc is bytes in data/structs, which has no SDK types; this is the record */
#define DESC (*(DDSURFACEDESC2 *)desc)

/* 0x40a140 - a blank texture page of (at least) width x height in the requested format. */
Texture::Texture(D3DApp *app, u32 width, u32 height, u32 format, s32 *result)
{
    D3DDEVICEDESC7 ddDesc;
    IDirect3DDevice7 *pd3dDevice;
    IDirectDrawSurface7 *pRender;
    IDirectDraw7 *pDDraw;
    HRESULT ddrval;

    surface = NULL;
    memset(desc, 0, sizeof(DDSURFACEDESC2));
    if (!app->deviceReady) {
        *result = TEXRES_NO_DEVICE;
        return;
    }
    pd3dDevice = app->pD3DDevice;
    pd3dDevice->GetCaps(&ddDesc);
    pd3dDevice->GetRenderTarget(&pRender);
    /* cast kept: COM returns the interface through a void ** (the SDK signature) */
    pRender->GetDDInterface((void **)&pDDraw);
    pRender->Release();
    DESC.dwSize = sizeof(DDSURFACEDESC2);
    DESC.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT | DDSD_TEXTURESTAGE;
    DESC.ddsCaps.dwCaps = DDSCAPS_TEXTURE;
    DESC.ddsCaps.dwCaps2 = DDSCAPS2_HINTDYNAMIC;
    if (ddDesc.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_POW2) {
        DESC.dwWidth = 1;
        while (DESC.dwWidth < width)
            DESC.dwWidth <<= 1;
        DESC.dwHeight = 1;
        while (DESC.dwHeight < height)
            DESC.dwHeight <<= 1;
    } else {
        DESC.dwWidth = width;
        DESC.dwHeight = height;
    }
    if (ddDesc.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_SQUAREONLY) {
        if (DESC.dwWidth > DESC.dwHeight)
            DESC.dwHeight = DESC.dwWidth;
        else
            DESC.dwWidth = DESC.dwHeight;
    }
    this->width = DESC.dwWidth;
    this->height = DESC.dwHeight;
    if (IsEqualGUID(ddDesc.deviceGUID, IID_IDirect3DHALDevice) ||
        IsEqualGUID(ddDesc.deviceGUID, IID_IDirect3DTnLHalDevice))
        DESC.ddsCaps.dwCaps2 |= DDSCAPS2_TEXTUREMANAGE;
    else
        DESC.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
    g_texRequestedFormat = format;
    pd3dDevice->EnumTextureFormats(Texture_EnumPixelFormatCallback, &DESC.ddpfPixelFormat);
    if (DESC.ddpfPixelFormat.dwRGBBitCount == 0) {
        *result = TEXRES_NO_PIXEL_FORMAT;
        return;
    }
    this->format = g_texRequestedFormat;
    ddrval = pDDraw->CreateSurface(&DESC, &surface, NULL);
    if (ddrval == DDERR_OUTOFVIDEOMEMORY || ddrval == E_OUTOFMEMORY) {
        *result = TEXRES_OUT_OF_MEMORY;
        return;
    }
    pDDraw->Release();
    *result = TEXRES_OK;
}

/* 0x40a462 - a texture page holding a .bmp file, stretched to the surface's size, in R5G6B5 (format 0). */
Texture::Texture(const char *fileName, D3DApp *app, u32 width, u32 height, s32 *result)
{
    D3DDEVICEDESC7 ddDesc;
    IDirect3DDevice7 *pd3dDevice;
    IDirectDrawSurface7 *pRender;
    BITMAP bm;
    HBITMAP hbm; /* declared before pDDraw, and hDCDest before hResult: same buckets (vc6_locals) */
    IDirectDraw7 *pDDraw;
    HDC hDCDest;
    HRESULT hResult;
    HDC hdcBitmap;

    surface = NULL;
    memset(desc, 0, sizeof(DDSURFACEDESC2));
    if (!app->deviceReady) {
        *result = TEXRES_NO_DEVICE;
        return;
    }
    pd3dDevice = app->pD3DDevice;
    pd3dDevice->GetCaps(&ddDesc);
    pd3dDevice->GetRenderTarget(&pRender);
    /* cast kept: COM returns the interface through a void ** (the SDK signature) */
    pRender->GetDDInterface((void **)&pDDraw);
    pRender->Release();
    /* cast kept: LoadImageA returns a generic handle (the SDK signature) */
    hbm = (HBITMAP)LoadImageA(NULL, fileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
    if (hbm == NULL) {
        *result = TEXRES_NO_BITMAP;
        return;
    }
    GetObjectA(hbm, sizeof(BITMAP), &bm);
    DESC.dwSize = sizeof(DDSURFACEDESC2);
    DESC.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
    DESC.ddsCaps.dwCaps = DDSCAPS_TEXTURE;
    DESC.ddsCaps.dwCaps2 = DDSCAPS2_HINTDYNAMIC;
    if (ddDesc.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_POW2) {
        DESC.dwWidth = 1;
        while (DESC.dwWidth < width)
            DESC.dwWidth <<= 1;
        DESC.dwHeight = 1;
        while (DESC.dwHeight < height)
            DESC.dwHeight <<= 1;
    } else {
        DESC.dwWidth = width;
        DESC.dwHeight = height;
    }
    if (ddDesc.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_SQUAREONLY) {
        if (DESC.dwWidth > DESC.dwHeight)
            DESC.dwHeight = DESC.dwWidth;
        else
            DESC.dwWidth = DESC.dwHeight;
    }
    this->width = DESC.dwWidth;
    this->height = DESC.dwHeight;
    if (IsEqualGUID(ddDesc.deviceGUID, IID_IDirect3DHALDevice) ||
        IsEqualGUID(ddDesc.deviceGUID, IID_IDirect3DTnLHalDevice))
        DESC.ddsCaps.dwCaps2 |= DDSCAPS2_TEXTUREMANAGE;
    else
        DESC.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
    g_texRequestedFormat = TEXFMT_RGB565;
    pd3dDevice->EnumTextureFormats(Texture_EnumPixelFormatCallback, &DESC.ddpfPixelFormat);
    if (DESC.ddpfPixelFormat.dwRGBBitCount == 0) {
        *result = TEXRES_NO_PIXEL_FORMAT;
        return;
    }
    this->format = g_texRequestedFormat;
    hResult = pDDraw->CreateSurface(&DESC, &surface, NULL);
    if (hResult == DDERR_OUTOFVIDEOMEMORY || hResult == E_OUTOFMEMORY) {
        *result = TEXRES_OUT_OF_MEMORY;
        return;
    }
    pDDraw->Release();
    hdcBitmap = CreateCompatibleDC(NULL);
    if (hdcBitmap == NULL || surface->GetDC(&hDCDest) < 0) {
        *result = TEXRES_GDI_FAILED;
        surface->Release();
        return;
    }
    SelectObject(hdcBitmap, hbm);
    StretchBlt(hDCDest, 0, 0, DESC.dwWidth, DESC.dwHeight, hdcBitmap, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
    surface->ReleaseDC(hDCDest);
    DeleteDC(hdcBitmap);
    *result = TEXRES_OK;
}

/* 0x40a8b9 - a .bmp file converted to the requested format: loaded as R5G6B5 into a temporary Texture, then copied
 * (format 0) or repacked pixel by pixel (1 = A1R5G5B5 with alpha 0, 2 = A4R4G4B4 from the top bits, alpha 0). */
Texture::Texture(const char *fileName, D3DApp *app, u32 width, u32 height, u32 format, s32 *result)
{
    /* declaration order inside a hash bucket decides the slot (vc6_locals); unused sits at -0x8, where the original
     * has an untouched dword */
    u32 unused;
    DDSURFACEDESC2 *lockDesc;
    IDirectDrawSurface7 *pTarget;
    D3DDEVICEDESC7 devDesc;
    IDirect3DDevice7 *pd3dDevice;
    IDirectDraw7 *lpDD;
    u16 *srcPixels;
    Texture *src;
    DDSURFACEDESC2 *outDesc;
    u16 *dstBits;
    u32 n;
    HRESULT hResult;
    u32 pixelCount;

    src = new Texture(fileName, app, width, height, result);
    if (*result == TEXRES_OK) {
        lockDesc = new DDSURFACEDESC2;
        src->Surface_LockForRead(lockDesc);
        srcPixels = (u16 *)lockDesc->lpSurface; /* cast kept: a locked surface is raw memory; these pixels are 16-bit */
        surface = NULL;
        memset(desc, 0, sizeof(DDSURFACEDESC2));
        if (!app->deviceReady) {
            *result = TEXRES_NO_DEVICE;
            return;
        }
        pd3dDevice = app->pD3DDevice;
        pd3dDevice->GetCaps(&devDesc);
        pd3dDevice->GetRenderTarget(&pTarget);
        /* cast kept: COM returns the interface through a void ** (the SDK signature) */
        pTarget->GetDDInterface((void **)&lpDD);
        pTarget->Release();
        memcpy(desc, src->desc, sizeof(DDSURFACEDESC2));
        g_texRequestedFormat = format;
        pd3dDevice->EnumTextureFormats(Texture_EnumPixelFormatCallback, &DESC.ddpfPixelFormat);
        if (DESC.ddpfPixelFormat.dwRGBBitCount == 0) {
            *result = TEXRES_NO_PIXEL_FORMAT;
            return;
        }
        hResult = lpDD->CreateSurface(&DESC, &surface, NULL);
        if (hResult == DDERR_OUTOFVIDEOMEMORY || hResult == E_OUTOFMEMORY) {
            *result = TEXRES_OUT_OF_MEMORY;
            return;
        }
        lpDD->Release();
        outDesc = new DDSURFACEDESC2;
        Surface_LockReadWrite(outDesc);
        dstBits = (u16 *)outDesc->lpSurface; /* cast kept: a locked surface is raw memory; these pixels are 16-bit */
        this->width = DESC.dwWidth;
        this->height = DESC.dwHeight;
        pixelCount = DESC.dwWidth * DESC.dwHeight;
        this->format = g_texRequestedFormat;
        switch (this->format) {
            case TEXFMT_RGB565:
                memcpy(dstBits, srcPixels, pixelCount * 2);
                break;
            case TEXFMT_ARGB1555:
                memset(dstBits, 0, pixelCount * 2);
                for (n = 0; n < pixelCount; n++) {
                    u16 px = srcPixels[n];
                    dstBits[n] = (px & 0x1f) | ((px & 0x7c0) >> 1) | ((px & 0xf800) >> 1);
                }
                break;
            case TEXFMT_ARGB4444:
                memset(dstBits, 0, pixelCount * 2);
                for (n = 0; n < pixelCount; n++) {
                    u16 pix = srcPixels[n];
                    dstBits[n] = ((pix & 0x1e) >> 1) | ((pix & 0x780) >> 3) | ((pix & 0xf000) >> 4);
                }
                break;
            default:
                *result = TEXRES_NO_PIXEL_FORMAT;
        }
        Surface_Unlock();
        delete outDesc;
        src->Surface_Unlock();
        delete lockDesc;
        delete src;
    }
}

/* 0x40ad97 */
Texture::~Texture()
{
    if (surface)
        surface->Release();
}

/* 0x40adc6 */
u32 Texture::GetWidth()
{
    return width;
}

/* 0x40add7 */
u32 Texture::GetHeight()
{
    return height;
}

/* 0x40ade8 */
u32 Texture::GetFormat()
{
    return format;
}

/* 0x40adf9 */
IDirectDrawSurface7 *Texture::GetSurface()
{
    return surface;
}

/* 0x40ae0a */
void Texture::Surface_LockForRead(DDSURFACEDESC2 *desc)
{
    desc->dwSize = sizeof(DDSURFACEDESC2);
    surface->Lock(NULL, desc, DDLOCK_WAIT | DDLOCK_READONLY | DDLOCK_NOSYSLOCK, NULL);
}

/* 0x40ae3f */
long Texture::Surface_LockForWrite(DDSURFACEDESC2 *desc)
{
    desc->dwSize = sizeof(DDSURFACEDESC2);
    return surface->Lock(NULL, desc, DDLOCK_WAIT | DDLOCK_WRITEONLY | DDLOCK_NOSYSLOCK, NULL);
}

/* 0x40ae74 */
void Texture::Surface_LockReadWrite(DDSURFACEDESC2 *desc)
{
    desc->dwSize = sizeof(DDSURFACEDESC2);
    surface->Lock(NULL, desc, DDLOCK_WAIT | DDLOCK_NOSYSLOCK, NULL);
}

/* 0x40aea9 */
void Texture::Surface_Unlock()
{
    surface->Unlock(NULL);
}

/* 0x40aecb - IDirect3DDevice7::EnumTextureFormats callback: takes the first plain 16-bit RGB format whose channel widths
 * fit g_texRequestedFormat (5-6-5; 5-5-5 with alpha; 4-4-4 with alpha), copies it out and stops. An unknown index
 * stops at the first 16-bit RGB format without copying anything. */
HRESULT __stdcall Texture_EnumPixelFormatCallback(DDPIXELFORMAT *pddpf, void *pOutPixelFormat)
{
    HRESULT ret;
    ret = DDENUMRET_OK;
    if (pddpf->dwRGBBitCount == 16 && pddpf->dwFourCC == 0 &&
        !(pddpf->dwFlags & (DDPF_LUMINANCE | DDPF_BUMPLUMINANCE | DDPF_BUMPDUDV))) {
        u16 rBits = Texture_CountMaskBits(pddpf->dwRBitMask);
        u16 greenBits = Texture_CountMaskBits(pddpf->dwGBitMask);
        switch (g_texRequestedFormat) {
            case TEXFMT_RGB565:
                if (rBits == 5 && greenBits == 6) {
                    memcpy(pOutPixelFormat, pddpf, sizeof(DDPIXELFORMAT));
                    ret = DDENUMRET_CANCEL;
                }
                break;
            case TEXFMT_ARGB1555:
                if (rBits == 5 && greenBits == 5 && pddpf->dwRGBAlphaBitMask) {
                    memcpy(pOutPixelFormat, pddpf, sizeof(DDPIXELFORMAT));
                    ret = DDENUMRET_CANCEL;
                }
                break;
            case TEXFMT_ARGB4444:
                if (rBits == 4 && greenBits == 4 && pddpf->dwRGBAlphaBitMask) {
                    memcpy(pOutPixelFormat, pddpf, sizeof(DDPIXELFORMAT));
                    ret = DDENUMRET_CANCEL;
                }
                break;
            default:
                ret = DDENUMRET_CANCEL;
        }
    }
    return ret;
}

/* 0x40aff4 - the number of set bits in mask. */
u16 Texture_CountMaskBits(u32 mask)
{
    u16 n;
    n = 0;
    while (mask) {
        mask &= mask - 1;
        n++;
    }
    return n;
}

/* 0x40b030 Texture_ScalarDeletingDtor: generated by the compiler from the virtual destructor. */
