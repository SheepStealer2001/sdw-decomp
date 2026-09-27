/*
 * Object T003 (data/tu_map.json), guessed original file D3DApp.cpp.
 *   .text 0x4044c0-0x40656f, then the COMDAT ??_GD3DApp 0x406570-0x40659e
 *   .rdata 0x5742e8-0x574304 (vtable, pad, doubles 0.0 / 1.0, 0.0f)   .data 0x5793e4-0x579418 (the literals)
 *   .bss 0x585068-0x5c7638                                            .CRT$XCU: 0x4044c0
 * The eight plain globals after g_limiterTimer are written with explicit zero initialisers. VC6 puts uninitialised
 * globals (and those with constructors) first in .bss, ordered by a hash of their names, and zero-initialised ones after
 * them in definition order; the exe has g_limiterTimer first and the rest in exactly the order they are defined here.
 */
/* BYTES: dead-code, inline, layout, slot-name. */
/* BYTES(layout): written '= 0' only to keep definition order in .bss */
/* BYTES(slot-name): D3DApp::D3DApp (filtered constructor): names chosen for their stack slots: iDev -1, pModes -8, j -0xc */
/* match-init: StaticInit_g_limiterTimer */
/* match-addr: ??0D3DApp@@QAE@PAUHWND__@@PAUHINSTANCE__@@@Z=0x4044ff   the 2-argument constructor: the decorated
   name pins it, because the undecorated D3DApp_Ctor / D3DApp_Construct cannot tell the two overloads apart */
/*
 * D3DApp - the DirectDraw 7 / Direct3D 7 / DirectInput 8 application object (one instance, g_pD3DAppMain 0x6d6568, built by
 * WinMain), SheepD3D.exe 0x4044c0-0x40659f: one original source file by address. It opens with the static initialiser of the
 * frame limiter's Timer and ends with the compiler-generated deleting destructor (0x406570, after the int3 pad). It also
 * holds the frame limiter, Frame_LimitFps / Frame_GetFps.
 *
 * What the file does
 *  - Construction enumerates everything once: DirectDrawEnumerateExA -> D3DEnum_DDrawDeviceCallback (one DirectDraw driver:
 *    its caps, a width-0 "Windowed Mode" pseudo-mode when it is the primary display and can render windowed, then its display
 *    modes via D3DEnum_DisplayModeCallback) -> IDirect3D7::EnumDevices -> D3DEnum_D3DDeviceCallback (one D3D device of that
 *    driver: its caps and the driver's modes filtered to the bit depths the device renders), into the scratch array
 *    g_enumDevices; then IDirectInput8::EnumDevices(GAMECTRL) -> DI_JoystickEnumCallback into g_enumJoysticks. The
 *    constructor copies both into the object; D3DApp_Construct (the one WinMain uses, filterFlags 3) drops the "RGB"
 *    emulation device (bit 1) and the windowed pseudo-mode (bit 0).
 *  - The launcher steps / sets the device, mode and joystick indices and reads their names; CreateDevice then builds the
 *    DirectDraw / Direct3D / DirectInput objects for the chosen device and mode (windowed when the device is desktop
 *    compatible and the mode is the width-0 pseudo-mode, else exclusive fullscreen with one flip back buffer).
 *  - The frame limiter (Frame_LimitFps) busy-waits on its own Timer, g_limiterTimer.
 * The code follows the DirectX 7 SDK's d3dframe/d3denum samples closely (same flags, same order of calls).
 *
 * Types: there are no SDK headers for this compiler, so the Win32 / DirectX types are declared below as far as they are used
 * (layouts from the DX7 / Win32 SDK, each offset that the code touches checked against the instructions); the SDK
 * constants come from the stand-ins in src/sdk. data/structs types D3DApp's hWnd / hInstance as HWND__ * /
 * HINSTANCE__ * and has the GUID, RECT and DIJoystickInfo structs; D3DDeviceInfo keeps its SDK records
 * (D3DDEVICEDESC7, DDCAPS, DDSURFACEDESC2[100]) as bytes, read through the casts DEVDESC / DRVCAPS / HELCAPS / MODES
 * below.
 * Local names were chosen for their stack slots (src/README.md, tools/vc6_locals.py) where a comment says so.
 */
#include "sdw_enums.h"
#include "../engine/timer.h"
#include "../sdk/d3d7.h"
#include "../sdk/ddraw.h"
#include "../sdk/dinput.h"
#include "../sdk/win32.h"

#include "../sdk/crt.h"

#define SDW_MEMBERS_D3DApp                                                                                         \
    D3DApp(HWND hWnd, HINSTANCE hInstance);                 /* 0x4044ff D3DApp_ConstructUnfiltered (no callers) */ \
    D3DApp(HWND hWnd, HINSTANCE hInstance, u8 filterFlags); /* 0x40464f D3DApp_Construct */                        \
    inline void DrawDebugText(int column, int line, char *text, int len); /* source-only inline, see DrawFpsText */
#include "sdw_classes.h"

#define WM_USER 0x400

/* the SDK's C++ inline (guiddef.h): its `return !memcmp(...)` is materialised as neg/sbb/inc before the || test (0x404c16) */
inline int IsEqualGUID(const GUID &rguid1, const GUID &rguid2)
{
    return !memcmp(&rguid1, &rguid2, sizeof(GUID));
}

/* D3DDeviceInfo's SDK records, which data/structs keeps as bytes.
 * cast kept (these four macros): the records are raw bytes in D3DDeviceInfo; each macro reads one as its SDK type */
#define DEVDESC(info) (*(D3DDEVICEDESC7 *)(info)->ddDesc)
#define DRVCAPS(info) (*(DDCAPS *)(info)->ddDriverCaps)
#define HELCAPS(info) (*(DDCAPS *)(info)->ddHelCaps)
/* cast kept: the Windows API takes and returns untyped memory here */
#define MODES(info) ((DDSURFACEDESC2 *)(info)->modes)

/* ---- the enumeration callbacks, defined at the end (their original place) ---- */
HRESULT __stdcall D3DEnum_DisplayModeCallback(DDSURFACEDESC2 *pddsd, void *pContext);
/* BYTES(slot-name): names and declaration order chosen for their stack slots: pDriver -4, ret -8, pInfo -0xc, i -0x10, dwBpp -0x14, depths -0x18 */
HRESULT __stdcall D3DEnum_D3DDeviceCallback(char *lpDeviceDescription, char *lpDeviceName, D3DDEVICEDESC7 *pDesc,
                                            void *pContext);
/* BYTES(slot-name): names and declaration order chosen for their stack slots: result -1, d3d7 -8, pdd -0xc, err -0x10, deviceInfo -0x34c8, pMode -0x34cc, dmDesktop -0x3568 */
BOOL __stdcall D3DEnum_DDrawDeviceCallback(GUID *lpGUID, char *lpDriverDescription, char *lpDriverName, void *pContext,
                                           void *hMonitor);
HRESULT __stdcall D3DEnum_ZBufferFormatCallback(DDPIXELFORMAT *pFormat, void *pContext);
BOOL __stdcall DI_JoystickEnumCallback(const DIDEVICEINSTANCEA *pdidi, void *pvRef);

/* ---- this file's globals (.bss, contiguous 0x585068-0x5c7631 in this order) ---- */
Timer g_limiterTimer;                     /* 0x585068  the limiter's own Timer, independent of the gameplay clock */
u8 g_enumDeviceCount = 0;                 /* 0x585090  entries of g_enumDevices filled by the enumeration */
u8 g_enumJoystickCount = 0;               /* 0x585091  entries of g_enumJoysticks */
IDirectInput8A *g_pEnumDirectInput = 0;   /* 0x585094  temporary IDirectInput8 of the constructor */
D3DDeviceInfo g_enumDevices[20] = {0};    /* 0x585098  scratch device table the callbacks fill */
DIJoystickInfo g_enumJoysticks[20] = {0}; /* 0x5c6ea8  scratch controller table */
double g_limiterElapsedSec = 0;           /* 0x5c7628  seconds taken by the last frame, as the limiter measured it */
char g_szEmpty_5c7630[1] = {0};           /* 0x5c7630  never written: always "" */
char g_szEmpty_5c7631[1] = {0};           /* 0x5c7631  never written: always "" */

/* 0x4044c0-0x4044fe: the static initialiser of g_limiterTimer - the root StaticInit_g_limiterTimer (the _initterm entry),
 * StaticCtor_ (Timer_Construct on it), StaticAtexit_ (atexit(StaticDtor_)) and StaticDtor_ (Timer_Destruct). VC6 generates
 * all four from the definition above; the `match-init:` line places the root and the matcher follows its calls. */

/* 0x4044ff - D3DApp_ConstructUnfiltered: the constructor without the device / mode filter. No callers. */
D3DApp::D3DApp(HWND hWnd, HINSTANCE hInstance)
{
    this->hWnd = hWnd;
    this->hInstance = hInstance;
    deviceCount = 0;
    deviceIndex = 0;
    g_enumDeviceCount = 0;
    DirectDrawEnumerateExA(D3DEnum_DDrawDeviceCallback, 0,
                           DDENUM_ATTACHEDSECONDARYDEVICES | DDENUM_DETACHEDSECONDARYDEVICES |
                               DDENUM_NONDISPLAYDEVICES);
    deviceCount = g_enumDeviceCount;
    memcpy(devices, g_enumDevices, deviceCount * sizeof(D3DDeviceInfo));
    pDD = 0;
    pD3D = 0;
    pD3DDevice = 0;
    joystickCount = 0;
    joystickIndex = 0;
    g_enumJoystickCount = 0;
    /* cast kept: COM returns every interface through a void ** out parameter */
    if (DirectInput8Create(this->hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8A, (void **)&g_pEnumDirectInput, 0) >=
        0) {
        g_pEnumDirectInput->EnumDevices(DI8DEVCLASS_GAMECTRL, DI_JoystickEnumCallback, 0, DIEDFL_ATTACHEDONLY);
        g_pEnumDirectInput->Release();
    }
    joystickCount = g_enumJoystickCount;
    memcpy(joysticks, g_enumJoysticks, joystickCount * sizeof(DIJoystickInfo));
    pDInput = 0;
}

/* 0x40464f - D3DApp_Construct, the constructor WinMain uses (filterFlags 3): enumerates like the one above, but copies the
 * devices one at a time, skipping the "RGB" emulation device when bit 1 is set and the width-0 windowed pseudo-mode when
 * bit 0 is set. */
D3DApp::D3DApp(HWND hWnd, HINSTANCE hInstance, u8 filterFlags)
{
    u8 iDev; /* names for the slots: iDev -1, pModes -8, j -0xc */
    DDSURFACEDESC2 *pModes;
    u32 j;

    this->hWnd = hWnd;
    this->hInstance = hInstance;
    deviceCount = 0;
    deviceIndex = 0;
    g_enumDeviceCount = 0;
    DirectDrawEnumerateExA(D3DEnum_DDrawDeviceCallback, 0,
                           DDENUM_ATTACHEDSECONDARYDEVICES | DDENUM_DETACHEDSECONDARYDEVICES |
                               DDENUM_NONDISPLAYDEVICES);
    for (iDev = 0; iDev < g_enumDeviceCount; iDev++) {
        if (!(filterFlags & D3DAPP_FILTER_NO_RGB) || strncmp(g_enumDevices[iDev].strDesc, "RGB", 3)) {
            strncpy(devices[deviceCount].strDesc, g_enumDevices[iDev].strDesc, 0x27);
            devices[deviceCount].pDeviceGUID = g_enumDevices[iDev].pDeviceGUID;
            devices[deviceCount].guidDevice = g_enumDevices[iDev].guidDevice;
            DEVDESC(&devices[deviceCount]) = DEVDESC(&g_enumDevices[iDev]);
            devices[deviceCount].bHardware = g_enumDevices[iDev].bHardware;
            devices[deviceCount].pDriverGUID = g_enumDevices[iDev].pDriverGUID;
            devices[deviceCount].guidDriver = g_enumDevices[iDev].guidDriver;
            DRVCAPS(&devices[deviceCount]) = DRVCAPS(&g_enumDevices[iDev]);
            HELCAPS(&devices[deviceCount]) = HELCAPS(&g_enumDevices[iDev]);
            devices[deviceCount].modeCount = 0;
            pModes = MODES(&devices[deviceCount]);
            for (j = 0; j < g_enumDevices[iDev].modeCount; j++) {
                if (!(filterFlags & D3DAPP_FILTER_NO_WINDOWED) || MODES(&g_enumDevices[iDev])[j].dwWidth != 0) {
                    memcpy(&pModes[devices[deviceCount].modeCount], &MODES(&g_enumDevices[iDev])[j],
                           sizeof(DDSURFACEDESC2));
                    devices[deviceCount].modeCount++;
                }
            }
            devices[deviceCount].modeIndex = 0;
            devices[deviceCount].bDesktopCompatible = g_enumDevices[iDev].bDesktopCompatible;
            deviceCount++;
        }
    }
    pDD = 0;
    pD3D = 0;
    pD3DDevice = 0;
    joystickCount = 0;
    joystickIndex = 0;
    g_enumJoystickCount = 0;
    /* cast kept: COM returns every interface through a void ** out parameter */
    if (DirectInput8Create(this->hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8A, (void **)&g_pEnumDirectInput, 0) >=
        0) {
        g_pEnumDirectInput->EnumDevices(DI8DEVCLASS_GAMECTRL, DI_JoystickEnumCallback, 0, DIEDFL_ATTACHEDONLY);
        g_pEnumDirectInput->Release();
    }
    joystickCount = g_enumJoystickCount;
    memcpy(joysticks, g_enumJoysticks, joystickCount * sizeof(DIJoystickInfo));
    pDInput = 0;
}

/* 0x404aec - D3DApp_Destruct. The primary / back / z surfaces are left to DirectDraw's own teardown. */
D3DApp::~D3DApp()
{
    ReleaseInterfaces();
}

/* 0x404b08 - creates an off-screen texture surface like the back buffer, sized up to powers of two / a square when the
 * device needs it, managed on a HAL device and in system memory otherwise. E_FAIL when *ppSurface is already set or the
 * size exceeds the device's maximum. */
/* BYTES(slot-name): ddDesc declared first: ddsd shares its hash bucket and must get the higher slot (ddsd -0x88, ddDesc -0x178) */
HRESULT D3DApp::CreateTextureSurface(IDirectDrawSurface7 **ppSurface, u32 width, u32 height)
{
    HRESULT hr = E_FAIL;
    if (*ppSurface == 0) {
        D3DDEVICEDESC7 ddDesc; /* declared first: ddsd shares its hash bucket and must get the */
        DDSURFACEDESC2 ddsd;   /* higher slot (ddsd -0x88, ddDesc -0x178) */
        ddsd.dwSize = sizeof(ddsd);
        pBackBuffer->GetSurfaceDesc(&ddsd);
        ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT | DDSD_TEXTURESTAGE;
        ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE;
        ddsd.ddsCaps.dwCaps2 = DDSCAPS2_HINTDYNAMIC;
        pD3DDevice->GetCaps(&ddDesc);
        if (ddDesc.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_POW2) {
            ddsd.dwWidth = 1;
            while (ddsd.dwWidth < width)
                ddsd.dwWidth <<= 1;
            ddsd.dwHeight = 1;
            while (ddsd.dwHeight < height)
                ddsd.dwHeight <<= 1;
        } else {
            ddsd.dwWidth = width;
            ddsd.dwHeight = height;
        }
        if (ddDesc.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_SQUAREONLY) {
            if (ddsd.dwWidth > ddsd.dwHeight)
                ddsd.dwHeight = ddsd.dwWidth;
            else
                ddsd.dwWidth = ddsd.dwHeight;
        }
        if (IsEqualGUID(ddDesc.deviceGUID, IID_IDirect3DHALDevice) ||
            IsEqualGUID(ddDesc.deviceGUID, IID_IDirect3DTnLHalDevice))
            ddsd.ddsCaps.dwCaps2 |= DDSCAPS2_TEXTUREMANAGE;
        else
            ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
        if (ddsd.dwWidth <= ddDesc.dwMaxTextureWidth && ddsd.dwHeight <= ddDesc.dwMaxTextureHeight)
            hr = pDD->CreateSurface(&ddsd, ppSurface, 0);
    }
    return hr;
}

/* 0x404c9b - one device forward (forward == 1) or back; resets the new device's mode to 0. 2 when stepping past the last
 * device (clamped), 1 when stepping before the first - dead: deviceIndex is a u8, so the < 0 test never holds and a step
 * back from 0 would reset devices[255].modeIndex, far outside the object. Unreachable: the launcher's only call passes 1
 * (0x402c25), filling the device combo until the result is 2. */
u8 D3DApp::StepDevice(u8 forward)
{
    u8 result;
    if (forward == 1) {
        deviceIndex++;
        if (deviceIndex >= deviceCount) {
            deviceIndex = deviceCount - 1;
            result = SEL_CLAMPED_AT_END;
        } else {
            devices[deviceIndex].modeIndex = 0;
            result = SEL_OK;
        }
    } else {
        deviceIndex--;
        if (deviceIndex < 0) {
            deviceIndex = 0;
            result = SEL_CLAMPED_AT_START;
        } else {
            devices[deviceIndex].modeIndex = 0;
            result = SEL_OK;
        }
    }
    return result;
}

/* 0x404d77 - selects device index (mode 0); 3 and nothing changed when it is out of range. */
u8 D3DApp::SetDeviceIndex(u8 index)
{
    u8 result;
    if (index >= deviceCount) {
        result = SEL_OUT_OF_RANGE;
    } else {
        deviceIndex = index;
        devices[deviceIndex].modeIndex = 0;
        result = SEL_OK;
    }
    return result;
}

/* 0x404dcf - one display mode of the current device forward or back; 2 past the last (clamped), the 1 of "before the
 * first" is dead (modeIndex is unsigned). The launcher only steps forward (0x40252d). */
u8 D3DApp::StepMode(u8 forward)
{
    D3DDeviceInfo *pInfo = &devices[deviceIndex];
    u8 status;
    if (forward == 1) {
        pInfo->modeIndex++;
        if (pInfo->modeIndex >= pInfo->modeCount) {
            pInfo->modeIndex = pInfo->modeCount - 1;
            status = SEL_CLAMPED_AT_END;
        } else {
            status = SEL_OK;
        }
    } else {
        pInfo->modeIndex--;
        if (pInfo->modeIndex < 0) {
            pInfo->modeIndex = 0;
            status = SEL_CLAMPED_AT_START;
        } else {
            status = SEL_OK;
        }
    }
    return status;
}

/* 0x404e86 - selects display mode index of the current device; 3 when out of range. */
u8 D3DApp::SetModeIndex(u8 index)
{
    D3DDeviceInfo *pInfo = &devices[deviceIndex];
    u8 status;
    if (index >= pInfo->modeCount) {
        status = SEL_OUT_OF_RANGE;
    } else {
        pInfo->modeIndex = index;
        status = SEL_OK;
    }
    return status;
}

/* 0x404ed8 */
void D3DApp::GetDeviceName(char *out)
{
    strncpy(out, devices[deviceIndex].strDesc, 0x27);
}

/* 0x404f0b - "<w> x <h> x <bpp> bpp", or "Windowed Mode : <bpp> bpp" for the width-0 pseudo-mode. */
/* BYTES(slot-name): names chosen for their stack slots: pInfo -4, num -0xc, mode -0x10 */
void D3DApp::GetModeString(char *out)
{
    D3DDeviceInfo *pInfo = &devices[deviceIndex];
    DDSURFACEDESC2 *mode = &MODES(pInfo)[pInfo->modeIndex]; /* names for the slots: pInfo -4, num -0xc, mode -0x10 */
    char num[8];
    if (mode->dwWidth == 0) {
        strcpy(out, "Windowed Mode : ");
    } else {
        itoa(mode->dwWidth, num, 10);
        strcpy(out, num);
        strcat(out, " x ");
        itoa(mode->dwHeight, num, 10);
        strcat(out, num);
        strcat(out, " x ");
    }
    itoa(mode->ddpfPixelFormat.dwRGBBitCount, num, 10);
    strcat(out, num);
    strcat(out, " bpp");
}

/* 0x40500b - total video memory of the current device's driver (a temporary DirectDraw object when there is none yet);
 * 0 if that cannot be created. */
/* BYTES(slot-name): names and declaration order chosen for their stack slots: dwTotalMem -4, dwFreeMem -8, ddscaps -0x18, hr -0x1c, pTempDD -0x20, pGUID -0x24 */
u32 D3DApp::GetTotalVideoMem()
{
    GUID *pGUID;           /* names and order chosen for the slots: */
    IDirectDraw7 *pTempDD; /* dwTotalMem -4, dwFreeMem -8, ddscaps -0x18, */
    HRESULT hr;            /* hr -0x1c, pTempDD -0x20, pGUID -0x24 */
    DDSCAPS2 ddscaps;
    DWORD dwFreeMem;
    DWORD dwTotalMem;
    memset(&ddscaps, 0, sizeof(ddscaps));
    ddscaps.dwCaps = DDSCAPS_VIDEOMEMORY;
    if (pDD) {
        pDD->GetAvailableVidMem(&ddscaps, &dwTotalMem, &dwFreeMem);
    } else {
        pGUID = devices[deviceIndex].pDriverGUID;
        /* cast kept: COM returns every interface through a void ** out parameter */
        hr = DirectDrawCreateEx(pGUID, (void **)&pTempDD, IID_IDirectDraw7, 0);
        if (hr >= 0) {
            pTempDD->GetAvailableVidMem(&ddscaps, &dwTotalMem, &dwFreeMem);
            pTempDD->Release();
        } else {
            dwTotalMem = 0;
        }
    }
    return dwTotalMem;
}

/* 0x4050c2 - the same, free video memory. No callers. */
/* BYTES(slot-name): names and declaration order chosen for their stack slots: (offsets as in the existing comment) */
u32 D3DApp::GetFreeVideoMem()
{
    GUID *pGUID;           /* names and order chosen for the slots: */
    IDirectDraw7 *pTempDD; /* dwTotalMem -4, dwFreeMem -8, ddscaps -0x18, */
    HRESULT hr;            /* hr -0x1c, pTempDD -0x20, pGUID -0x24 */
    DDSCAPS2 ddscaps;
    DWORD dwFreeMem;
    DWORD dwTotalMem;
    memset(&ddscaps, 0, sizeof(ddscaps));
    ddscaps.dwCaps = DDSCAPS_VIDEOMEMORY;
    if (pDD) {
        pDD->GetAvailableVidMem(&ddscaps, &dwTotalMem, &dwFreeMem);
    } else {
        pGUID = devices[deviceIndex].pDriverGUID;
        /* cast kept: COM returns every interface through a void ** out parameter */
        hr = DirectDrawCreateEx(pGUID, (void **)&pTempDD, IID_IDirectDraw7, 0);
        if (hr >= 0) {
            pTempDD->GetAvailableVidMem(&ddscaps, &dwTotalMem, &dwFreeMem);
            pTempDD->Release();
        } else {
            dwFreeMem = 0;
        }
    }
    return dwFreeMem;
}

/* 0x405179 - total texture memory (DDSCAPS_TEXTURE). No callers. */
/* BYTES(slot-name): names and declaration order chosen for their stack slots: (offsets as in the existing comment) */
u32 D3DApp::GetTotalTextureMem()
{
    GUID *pGUID;           /* names and order chosen for the slots: */
    IDirectDraw7 *pTempDD; /* dwTotalMem -4, dwFreeMem -8, ddscaps -0x18, */
    HRESULT hr;            /* hr -0x1c, pTempDD -0x20, pGUID -0x24 */
    DDSCAPS2 ddscaps;
    DWORD dwFreeMem;
    DWORD dwTotalMem;
    memset(&ddscaps, 0, sizeof(ddscaps));
    ddscaps.dwCaps = DDSCAPS_TEXTURE;
    if (pDD) {
        pDD->GetAvailableVidMem(&ddscaps, &dwTotalMem, &dwFreeMem);
    } else {
        pGUID = devices[deviceIndex].pDriverGUID;
        /* cast kept: COM returns every interface through a void ** out parameter */
        hr = DirectDrawCreateEx(pGUID, (void **)&pTempDD, IID_IDirectDraw7, 0);
        if (hr >= 0) {
            pTempDD->GetAvailableVidMem(&ddscaps, &dwTotalMem, &dwFreeMem);
            pTempDD->Release();
        } else {
            dwTotalMem = 0;
        }
    }
    return dwTotalMem;
}

/* 0x405230 - free texture memory. No callers. */
/* BYTES(slot-name): names and declaration order chosen for their stack slots: (offsets as in the existing comment) */
u32 D3DApp::GetFreeTextureMem()
{
    GUID *pGUID;           /* names and order chosen for the slots: */
    IDirectDraw7 *pTempDD; /* dwTotalMem -4, dwFreeMem -8, ddscaps -0x18, */
    HRESULT hr;            /* hr -0x1c, pTempDD -0x20, pGUID -0x24 */
    DDSCAPS2 ddscaps;
    DWORD dwFreeMem;
    DWORD dwTotalMem;
    memset(&ddscaps, 0, sizeof(ddscaps));
    ddscaps.dwCaps = DDSCAPS_TEXTURE;
    if (pDD) {
        pDD->GetAvailableVidMem(&ddscaps, &dwTotalMem, &dwFreeMem);
    } else {
        pGUID = devices[deviceIndex].pDriverGUID;
        /* cast kept: COM returns every interface through a void ** out parameter */
        hr = DirectDrawCreateEx(pGUID, (void **)&pTempDD, IID_IDirectDraw7, 0);
        if (hr >= 0) {
            pTempDD->GetAvailableVidMem(&ddscaps, &dwTotalMem, &dwFreeMem);
            pTempDD->Release();
        } else {
            dwFreeMem = 0;
        }
    }
    return dwFreeMem;
}

/* 0x4052e7 - video memory the current mode needs: width * height * (bpp/8 * 2 + 2) (front + back buffer and a 16-bit z
 * buffer); bpp/8 for the windowed pseudo-mode. */
/* BYTES(slot-name): names and declaration order chosen for their stack slots: pInfo -4, cost -8, mode -0xc */
u32 D3DApp::GetModeMemCost()
{
    u32 cost; /* names and order for the slots: pInfo -4, cost -8, mode -0xc */
    D3DDeviceInfo *pInfo = &devices[deviceIndex];
    DDSURFACEDESC2 *mode = &MODES(pInfo)[pInfo->modeIndex];
    if (mode->dwWidth == 0) {
        cost = mode->ddpfPixelFormat.dwRGBBitCount >> 3;
    } else {
        cost = mode->dwWidth * mode->dwHeight;
        cost *= (mode->ddpfPixelFormat.dwRGBBitCount >> 3) * 2 + 2;
    }
    return cost;
}

/* 0x405367 - creates DirectDraw / Direct3D / DirectInput for the selected device and mode: windowed (a clipper, an
 * off-screen back buffer the size of the client area) when the device is desktop compatible and the mode is the width-0
 * pseudo-mode, else exclusive fullscreen with one flip back buffer; then the z buffer, the device, the viewport, a clear,
 * deviceReady = 1 and WM_USER+1 to the window. */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py); same-bucket pairs declared in reverse slot order (offsets listed below) */
/* BYTES(dead-code): bIsHw is stored and never read; kept because the original stores it (and it takes the -0xb8 slot) */
HRESULT D3DApp::CreateDevice()
{
    /* Names chosen for the slots (tools/vc6_locals.py): dwRenderWidth -4, result -8, ddpfZBuffer -0x28, pMode -0x2c,
     * ddsdesc -0xb0, pDrvGUID -0xb4, bIsHw -0xb8, dwRenderHeight -0xbc, pDeviceGUID -0xc0, viewport -0xd8,
     * pDeviceInfo -0xdc; then the two block locals pClipper -0xe0 and ddscaps -0xf0. The two same-bucket pairs are
     * declared in reverse slot order (result before dwRenderWidth, bIsHw before pDrvGUID). */
    D3DDeviceInfo *pDeviceInfo;
    GUID *pDeviceGUID;
    DDSURFACEDESC2 *pMode;
    BOOL bIsHw;
    GUID *pDrvGUID;
    DDPIXELFORMAT ddpfZBuffer;
    HRESULT result;
    DDSURFACEDESC2 ddsdesc;
    D3DVIEWPORT7 viewport;
    DWORD dwRenderHeight;
    DWORD dwRenderWidth;

    pDeviceInfo = &devices[deviceIndex];
    pDrvGUID = pDeviceInfo->pDriverGUID;
    pDeviceGUID = pDeviceInfo->pDeviceGUID;
    pMode = &MODES(pDeviceInfo)[pDeviceInfo->modeIndex];
    bIsHw = pDeviceInfo->bHardware; /* never read */

    /* cast kept: COM returns every interface through a void ** out parameter */
    result = DirectDrawCreateEx(pDrvGUID, (void **)&pDD, IID_IDirectDraw7, 0);
    if (result < 0)
        return result;
    result = pDD->QueryInterface(IID_IDirect3D7, (void **)&pD3D); /* cast kept: as above */
    if (result < 0)
        return result;
    /* cast kept: as above */
    result = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8A, (void **)&pDInput, 0);
    if (result < 0)
        return result;

    if (pDeviceInfo->bDesktopCompatible && pMode->dwWidth == 0) {
        result = pDD->SetCooperativeLevel(hWnd, DDSCL_NORMAL);
        if (result < 0)
            return result;
    } else {
        result = pDD->SetCooperativeLevel(hWnd, DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE);
        if (result < 0)
            return result;
        result = pDD->SetDisplayMode(pMode->dwWidth, pMode->dwHeight, pMode->ddpfPixelFormat.dwRGBBitCount,
                                     pMode->dwRefreshRate, 0);
        if (result < 0)
            return result;
    }

    memset(&ddsdesc, 0, sizeof(ddsdesc));
    ddsdesc.dwSize = sizeof(ddsdesc);
    if (pDeviceInfo->bDesktopCompatible && pMode->dwWidth == 0) {
        ddsdesc.dwFlags = DDSD_CAPS;
        ddsdesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
    } else {
        ddsdesc.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
        ddsdesc.ddsCaps.dwCaps = DDSCAPS_COMPLEX | DDSCAPS_FLIP | DDSCAPS_PRIMARYSURFACE | DDSCAPS_3DDEVICE;
        ddsdesc.dwBackBufferCount = 1;
    }
    result = pDD->CreateSurface(&ddsdesc, &pPrimary, 0);
    if (result < 0)
        return result;

    if (pDeviceInfo->bDesktopCompatible && pMode->dwWidth == 0) {
        IDirectDrawClipper *pClipper;
        result = pDD->CreateClipper(0, &pClipper, 0);
        if (result < 0)
            return result;
        pClipper->SetHWnd(0, hWnd);
        pPrimary->SetClipper(pClipper);
        pClipper->Release();
    }

    if (pDeviceInfo->bDesktopCompatible && pMode->dwWidth == 0) {
        ddsdesc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
        ddsdesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE;
        GetClientRect(hWnd, &clientRect);
        /* cast kept (both calls): a RECT is two POINTs, left/top then right/bottom (the SDK samples' idiom) */
        ClientToScreen(hWnd, (POINT *)&clientRect.left);
        ClientToScreen(hWnd, (POINT *)&clientRect.right);
        ddsdesc.dwWidth = clientRect.right - clientRect.left;
        ddsdesc.dwHeight = clientRect.bottom - clientRect.top;
        result = pDD->CreateSurface(&ddsdesc, &pBackBuffer, 0);
        if (result < 0)
            return result;
    } else {
        DDSCAPS2 ddscaps;
        SetRect(&clientRect, 0, 0, pMode->dwWidth, pMode->dwHeight);
        ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
        ddscaps.dwCaps2 = 0;
        ddscaps.dwCaps3 = 0;
        ddscaps.dwCaps4 = 0;
        result = pPrimary->GetAttachedSurface(&ddscaps, &pBackBuffer);
        if (result < 0)
            return result;
    }

    pD3D->EnumZBufferFormats(*pDeviceGUID, D3DEnum_ZBufferFormatCallback, &ddpfZBuffer);
    if (ddpfZBuffer.dwSize != sizeof(DDPIXELFORMAT))
        return E_FAIL;
    ddsdesc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
    ddsdesc.ddsCaps.dwCaps = DDSCAPS_ZBUFFER;
    ddsdesc.dwWidth = clientRect.right - clientRect.left;
    ddsdesc.dwHeight = clientRect.bottom - clientRect.top;
    memcpy(&ddsdesc.ddpfPixelFormat, &ddpfZBuffer, sizeof(DDPIXELFORMAT));
    if (IsEqualGUID(*pDeviceGUID, IID_IDirect3DHALDevice) || IsEqualGUID(*pDeviceGUID, IID_IDirect3DTnLHalDevice))
        ddsdesc.ddsCaps.dwCaps |= DDSCAPS_VIDEOMEMORY;
    else
        ddsdesc.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
    result = pDD->CreateSurface(&ddsdesc, &pZBuffer, 0);
    if (result < 0)
        return result;
    result = pBackBuffer->AddAttachedSurface(pZBuffer);
    if (result < 0)
        return result;

    ddsdesc.dwSize = sizeof(ddsdesc);
    pDD->GetDisplayMode(&ddsdesc);
    if (ddsdesc.ddpfPixelFormat.dwRGBBitCount <= 8)
        return DDERR_INVALIDMODE;

    result = pD3D->CreateDevice(*pDeviceGUID, pBackBuffer, &pD3DDevice);
    if (result < 0)
        return result;
    dwRenderWidth = clientRect.right - clientRect.left;
    dwRenderHeight = clientRect.bottom - clientRect.top;
    viewport.dwX = 0;
    viewport.dwY = 0;
    viewport.dwWidth = dwRenderWidth;
    viewport.dwHeight = dwRenderHeight;
    viewport.dvMinZ = 0.0f;
    viewport.dvMaxZ = 1.0f;
    result = pD3DDevice->SetViewport(&viewport);
    pD3DDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
    deviceReady = 1;
    SendMessageA(hWnd, WM_USER + 1, 0, (long)this); /* cast kept: the message's LPARAM carries the D3DApp pointer */
    return result;
}

/* 0x405add - releases DirectInput, the device, Direct3D and DirectDraw (the pointers are not cleared). */
void D3DApp::ReleaseInterfaces()
{
    if (pDInput)
        pDInput->Release();
    if (pD3DDevice)
        pD3DDevice->Release();
    if (pD3D)
        pD3D->Release();
    if (pDD)
        pDD->Release();
}

/* 0x405b5d */
int D3DApp::HasJoystick()
{
    return joystickCount > 0;
}

/* 0x405b79 - one controller forward or back; 2 past the last (clamped), the 1 of "before the first" is dead (u8). The
 * launcher only steps forward (0x402d8d). */
u8 D3DApp::StepJoystick(u8 forward)
{
    u8 result = SEL_OK;
    if (forward == 1) {
        joystickIndex++;
        if (joystickIndex >= joystickCount) {
            joystickIndex = joystickCount - 1;
            result = SEL_CLAMPED_AT_END;
        }
    } else {
        joystickIndex--;
        if (joystickIndex < 0) {
            joystickIndex = 0;
            result = SEL_CLAMPED_AT_START;
        }
    }
    return result;
}

/* 0x405c11 - selects controller index; 3 when out of range. */
u8 D3DApp::SetJoystickIndex(u8 index)
{
    u8 result = SEL_OUT_OF_RANGE;
    if (index < joystickCount) {
        joystickIndex = index;
        result = SEL_OK;
    }
    return result;
}

/* 0x405c49 - the current controller's name (both globals are empty strings, so the first branch only copies "" for an
 * unnamed controller). */
void D3DApp::GetJoystickInstanceName(char *out)
{
    if (strcmp(joysticks[joystickIndex].instanceName, g_szEmpty_5c7630) == 0)
        strcpy(out, g_szEmpty_5c7631);
    else
        strncpy(out, joysticks[joystickIndex].instanceName, 0x4f);
}

/* 0x405cb8 - the system mouse; E_FAIL before CreateDevice. */
HRESULT D3DApp::CreateMouseDevice(IDirectInputDevice8A **ppDevice)
{
    if (pDInput)
        return pDInput->CreateDevice(GUID_SysMouse, ppDevice, 0);
    return E_FAIL;
}

/* 0x405cfb - the controller chosen in the launcher. A success becomes E_FAIL when ppDevice itself is NULL (the SDK idiom
 * tests *ppDevice). */
HRESULT D3DApp::CreateJoystickDevice(IDirectInputDevice8A **ppDevice)
{
    HRESULT hr = E_FAIL;
    if (pDInput) {
        hr = pDInput->CreateDevice(joysticks[joystickIndex].guidInstance, ppDevice, 0);
        if (hr >= 0 && ppDevice == 0)
            hr = E_FAIL;
    }
    return hr;
}

/* 0x405d6c - the system keyboard; E_FAIL before CreateDevice. */
HRESULT D3DApp::CreateKeyboardDevice(IDirectInputDevice8A **ppDevice)
{
    if (pDInput)
        return pDInput->CreateDevice(GUID_SysKeyboard, ppDevice, 0);
    return E_FAIL;
}

/* 0x405daf - busy-waits until the frame has taken at least 1/maxFps s (the game's ~160% CPU), then restarts the
 * timer. The rate is compared as an integer: (u32)(1.0 / elapsed) > maxFps. Never reads `this`. */
void D3DApp::Frame_LimitFps(u32 maxFps)
{
    do {
        g_limiterElapsedSec = g_limiterTimer.GetElapsed(TIMER_SECONDS);
    } while ((u32)(1.0 / g_limiterElapsedSec) > maxFps && g_limiterElapsedSec != 0.0);
    g_limiterTimer.Stop();
    g_limiterTimer.Start();
}

/* 0x405e0b - 1 / the last frame's time, 0 before the first frame. No callers. */
float D3DApp::Frame_GetFps()
{
    if (g_limiterElapsedSec != 0.0)
        return (float)(1.0 / g_limiterElapsedSec);
    return 0.0f;
}

/* source-only inline (declared in SDW_MEMBERS_D3DApp; never emitted on its own): white-on-black text at a 16-pixel text
 * cell of the back buffer. Why an inline: the column is a parameter that DrawFpsText passes as the constant 0, which
 * VC6 /Od then computes as `xor ecx,ecx; shl ecx,4` (0x405ed6), and hdc is the inline's own local, placed after
 * DrawFpsText's named locals (-0x48) and before `this`. */
/* BYTES(inline): source-only inline: its expansion computes the constant column as xor ecx,ecx; shl ecx,4 (0x405ed6) and puts hdc after DrawFpsText's locals */
inline void D3DApp::DrawDebugText(int column, int line, char *text, int len)
{
    HDC hdc;
    pBackBuffer->GetDC(&hdc);
    SetBkColor(hdc, 0);
    SetTextColor(hdc, 0xffffff);
    if (text)
        TextOutA(hdc, column << 4, line << 4, text, len);
    pBackBuffer->ReleaseDC(hdc);
}

/* 0x405e42 - debug overlay: "FPS : nnn" on text line `line` of the back buffer. No callers. */
void D3DApp::DrawFpsText(s32 line)
{
    char text[64];
    int len;
    if (g_limiterElapsedSec != 0.0) {
        len = wsprintfA(text, "FPS : %3d", (int)(1.0 / g_limiterElapsedSec));
        DrawDebugText(0, line, text, len);
    }
}

/* 0x405f08 - renders into pNewTarget: moves the z buffer from the current render target to it, then SetRenderTarget.
 * Its only caller is RestoreRenderTarget, which has none. */
HRESULT D3DApp::SetRenderTargetSurface(IDirectDrawSurface7 *pNewTarget)
{
    IDirectDrawSurface7 *pOldTarget = 0;
    HRESULT hr;
    hr = pD3DDevice->GetRenderTarget(&pOldTarget);
    if (pOldTarget) {
        IDirectDrawSurface7 *pZ = 0;
        DDSCAPS2 ddscaps;
        ddscaps.dwCaps = DDSCAPS_ZBUFFER;
        ddscaps.dwCaps2 = 0;
        ddscaps.dwCaps3 = 0;
        ddscaps.dwCaps4 = 0;
        hr = pOldTarget->GetAttachedSurface(&ddscaps, &pZ);
        if (pZ == 0) {
            pOldTarget->Release();
        } else {
            pOldTarget->DeleteAttachedSurface(0, pZ);
            hr = pNewTarget->AddAttachedSurface(pZ);
            pZ->Release();
            pOldTarget->Release();
            if (hr >= 0)
                hr = pD3DDevice->SetRenderTarget(pNewTarget, 0);
        }
    }
    return hr;
}

/* 0x405ff0 - back to the back buffer. No callers. */
void D3DApp::RestoreRenderTarget()
{
    SetRenderTargetSurface(pBackBuffer);
}

/* 0x40600d - IDirectDraw7::EnumDisplayModes callback: appends the mode. No bound check against the 100 slots. */
HRESULT __stdcall D3DEnum_DisplayModeCallback(DDSURFACEDESC2 *pddsd, void *pContext)
{
    /* cast kept: the enumeration context is the D3DDeviceInfo the caller passed */
    D3DDeviceInfo *pInfo = (D3DDeviceInfo *)pContext;
    MODES(pInfo)[pInfo->modeCount] = *pddsd;
    pInfo->modeCount++;
    return DDENUMRET_OK;
}

/* 0x40605b - IDirect3D7::EnumDevices callback: one g_enumDevices entry per device of the driver in pContext, with the
 * driver's modes that the device renders at (16/24/32 bpp). A secondary driver's software devices are dropped, and so is a
 * device left with no mode. */
HRESULT __stdcall D3DEnum_D3DDeviceCallback(char *lpDeviceDescription, char *lpDeviceName, D3DDEVICEDESC7 *pDesc,
                                            void *pContext)
{
    DWORD depths; /* names and order for the slots: pDriver -4, ret -8, pInfo -0xc, */
    DWORD dwBpp;  /* i -0x10, dwBpp -0x14, depths -0x18 */
    u32 i;
    D3DDeviceInfo *pInfo;
    HRESULT ret;
    /* cast kept: the enumeration context is the driver's D3DDeviceInfo */
    D3DDeviceInfo *pDriver = (D3DDeviceInfo *)pContext;

    pInfo = &g_enumDevices[g_enumDeviceCount];
    memcpy(&DEVDESC(pInfo), pDesc, sizeof(D3DDEVICEDESC7));
    pInfo->guidDevice = pDesc->deviceGUID;
    pInfo->pDeviceGUID = &pInfo->guidDevice;
    pInfo->bHardware = pDesc->dwDevCaps & D3DDEVCAPS_HWRASTERIZATION;
    pInfo->modeCount = 0;
    pInfo->modeIndex = 0;
    DRVCAPS(pInfo) = DRVCAPS(pDriver);
    HELCAPS(pInfo) = HELCAPS(pDriver);
    if (pDriver->pDriverGUID) {
        pInfo->guidDriver = pDriver->guidDriver;
        pInfo->pDriverGUID = &pInfo->guidDriver;
        strncpy(pInfo->strDesc, pDriver->strDesc, 0x26);
    } else {
        pInfo->pDriverGUID = 0;
        strncpy(pInfo->strDesc, lpDeviceName, 0x26);
    }
    if (pInfo->pDriverGUID && !pInfo->bHardware) {
        ret = D3DENUMRET_OK;
    } else {
        for (i = 0; i < pDriver->modeCount; i++) {
            dwBpp = MODES(pDriver)[i].ddpfPixelFormat.dwRGBBitCount;
            depths = DEVDESC(pInfo).dwDeviceRenderBitDepth;
            if ((dwBpp == 16 && (depths & DDBD_16)) || (dwBpp == 24 && (depths & DDBD_24)) ||
                (dwBpp == 32 && (depths & DDBD_32))) {
                MODES(pInfo)[pInfo->modeCount] = MODES(pDriver)[i];
                pInfo->modeCount++;
            }
        }
        if (pInfo->modeCount == 0) {
            ret = D3DENUMRET_OK;
        } else {
            if (MODES(pInfo)[0].dwWidth == 0)
                pInfo->bDesktopCompatible = 1;
            else
                pInfo->bDesktopCompatible = 0;
            g_enumDeviceCount++;
            ret = D3DENUMRET_OK;
        }
    }
    return ret;
}

/* 0x4062e5 - DirectDrawEnumerateExA callback: one DirectDraw driver. Builds its D3DDeviceInfo on the stack (the driver /
 * HEL caps, a "Windowed Mode" width-0 pseudo-mode at the desktop depth for the primary display when it can render
 * windowed, then every display mode), and enumerates its D3D devices into g_enumDevices. Always continues. */
BOOL __stdcall D3DEnum_DDrawDeviceCallback(GUID *lpGUID, char *lpDriverDescription, char *lpDriverName, void *pContext,
                                           void *hMonitor)
{
    DEVMODEA dmDesktop;    /* names and order for the slots: result -1, d3d7 -8, pdd -0xc, */
    DDSURFACEDESC2 *pMode; /* err -0x10, deviceInfo -0x34c8, pMode -0x34cc, dmDesktop -0x3568 */
    D3DDeviceInfo deviceInfo;
    HRESULT err;
    IDirectDraw7 *pdd;
    IDirect3D7 *d3d7;
    u8 result;

    /* cast kept: COM returns every interface through a void ** out parameter */
    err = DirectDrawCreateEx(lpGUID, (void **)&pdd, IID_IDirectDraw7, 0);
    if (err < 0) {
        result = DDENUMRET_OK;
    } else {
        err = pdd->QueryInterface(IID_IDirect3D7, (void **)&d3d7); /* cast kept: as above */
        if (err < 0) {
            pdd->Release();
            result = DDENUMRET_OK;
        } else {
            memset(&deviceInfo, 0, sizeof(deviceInfo));
            strncpy(deviceInfo.strDesc, lpDriverDescription, 0x27);
            DRVCAPS(&deviceInfo).dwSize = sizeof(DDCAPS);
            HELCAPS(&deviceInfo).dwSize = sizeof(DDCAPS);
            pdd->GetCaps(&DRVCAPS(&deviceInfo), &HELCAPS(&deviceInfo));
            deviceInfo.pDriverGUID = lpGUID;
            if (lpGUID)
                deviceInfo.guidDriver = *lpGUID;
            if (deviceInfo.pDriverGUID == 0 && (DRVCAPS(&deviceInfo).dwCaps2 & DDCAPS2_CANRENDERWINDOWED)) {
                dmDesktop.dmSize = sizeof(DEVMODEA);
                EnumDisplaySettingsA(0, ENUM_CURRENT_SETTINGS, &dmDesktop);
                pMode = &MODES(&deviceInfo)[0];
                pMode->ddpfPixelFormat.dwRGBBitCount = dmDesktop.dmBitsPerPel;
                pMode->dwWidth = 0;
                pMode->dwHeight = 0;
                deviceInfo.modeCount = 1;
            }
            pdd->EnumDisplayModes(0, 0, &deviceInfo, D3DEnum_DisplayModeCallback);
            d3d7->EnumDevices(D3DEnum_D3DDeviceCallback, &deviceInfo);
            pdd->Release();
            d3d7->Release();
            result = DDENUMRET_OK;
        }
    }
    return result;
}

/* 0x4064a0 - IDirect3D7::EnumZBufferFormats callback: takes the first format that is a plain z buffer (dwFlags exactly
 * DDPF_ZBUFFER) and stops. */
HRESULT __stdcall D3DEnum_ZBufferFormatCallback(DDPIXELFORMAT *pFormat, void *pContext)
{
    if (pFormat->dwFlags == DDPF_ZBUFFER) {
        memcpy(pContext, pFormat, sizeof(DDPIXELFORMAT));
        return D3DENUMRET_CANCEL;
    }
    return D3DENUMRET_OK;
}

/* 0x4064ce - IDirectInput8::EnumDevices callback: records each controller that can be created. No bound check against the
 * 20 slots of g_enumJoysticks. */
BOOL __stdcall DI_JoystickEnumCallback(const DIDEVICEINSTANCEA *pdidi, void *pvRef)
{
    HRESULT hr;
    IDirectInputDevice8A *pDevice;
    hr = g_pEnumDirectInput->CreateDevice(pdidi->guidInstance, &pDevice, 0);
    if (hr >= 0) {
        strncpy(g_enumJoysticks[g_enumJoystickCount].instanceName, pdidi->tszInstanceName, 0x4f);
        g_enumJoysticks[g_enumJoystickCount].guidInstance = pdidi->guidInstance;
        g_enumJoystickCount++;
        pDevice->Release();
    }
    return DIENUM_CONTINUE;
}

/* 0x406570 - D3DApp_ScalarDeletingDtor: generated by the compiler from the virtual destructor above (the class's only
 * vtable slot), placed through the vtable. */
