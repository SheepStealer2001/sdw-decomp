/* Stand-in for DirectInput 8 (dinput.h): the constants the decompiled source names, with the SDK's own spelling and value.
 * Only what src/ uses is here: constants, types and functions (depends on sdw_types.h). */
#ifndef SDW_SDK_DINPUT_H
#define SDW_SDK_DINPUT_H

#define DI8DEVCLASS_GAMECTRL 4
#define DIDFT_AXIS 0x00000003 /* DIDFT_RELAXIS | DIDFT_ABSAXIS */
#define DIEDFL_ATTACHEDONLY 0x00000001
#define DIENUM_CONTINUE 1
#define DIENUM_STOP 0
#define DIERR_INPUTLOST ((HRESULT)0x8007001EL)      /* HRESULT_FROM_WIN32(ERROR_READ_FAULT) */
#define DIERR_NOTINITIALIZED ((HRESULT)0x80070015L) /* MAKE_HRESULT(1, FACILITY_WIN32, ERROR_NOT_READY) */
#define DIK_DOWN 0xD0                               /* DownArrow on arrow keypad */
#define DIK_ESCAPE 0x01
#define DIK_LEFT 0xCB /* LeftArrow on arrow keypad */
#define DIK_NEXT 0xD1 /* PgDn on arrow keypad */
#define DIK_P 0x19
#define DIK_PAUSE 0xC5  /* Pause */
#define DIK_PRIOR 0xC9  /* PgUp on arrow keypad */
#define DIK_RETURN 0x1C /* Enter on main keyboard */
#define DIK_RIGHT 0xCD  /* RightArrow on arrow keypad */
#define DIK_UP 0xC8     /* UpArrow on arrow keypad */
#define DIPH_BYID 2
#define DIPH_BYOFFSET 1
#define DIPROP_KEYNAME MAKEDIPROP(20)
#define DIPROP_RANGE MAKEDIPROP(4)
#define DIRECTINPUT_VERSION 0x0800
#define DISCL_EXCLUSIVE 0x00000001
#define DISCL_FOREGROUND 0x00000004
#define MAKEDIPROP(prop) (*(const GUID *)(prop)) /* C++: a predefined property is a small number as the REFGUID */

#include "windef.h"

struct DIDATAFORMAT;
struct DIDEVCAPS;
struct DIDEVICEINSTANCEA;
struct DIDEVICEOBJECTINSTANCEA;
struct DIPROPHEADER;
struct IDirectInputDevice8A;

struct DIDEVICEINSTANCEA { /* 0x244 bytes */
    DWORD dwSize;
    GUID guidInstance; /* +0x04 */
    GUID guidProduct;
    DWORD dwDevType;
    char tszInstanceName[260]; /* +0x28 */
    char tszProductName[260];
    GUID guidFFDriver;
    u16 wUsagePage, wUsage;
};

typedef BOOL(__stdcall *LPDIENUMDEVICESCALLBACKA)(const DIDEVICEINSTANCEA *instance, void *context);

struct DIDEVCAPS { /* 0x2c */
    DWORD dwSize, dwFlags, dwDevType, dwAxes, dwButtons, dwPOVs;
    DWORD dwFFSamplePeriod, dwFFMinTimeResolution, dwFirmwareRevision, dwHardwareRevision, dwFFDriverVersion;
};

struct DIPROPHEADER {
    DWORD dwSize, dwHeaderSize, dwObj, dwHow;
};

struct DIPROPRANGE {
    DIPROPHEADER diph;
    long lMin, lMax;
}; /* 0x18 */

struct DIPROPSTRING {
    DIPROPHEADER diph;
    WCHAR wsz[260];
}; /* 0x218 */

struct DIJOYSTATE { /* 0x50 */
    long lX, lY, lZ, lRx, lRy, lRz, rglSlider[2];
    DWORD rgdwPOV[4];
    u8 rgbButtons[32]; /* +0x30 */
};

struct DIDEVICEOBJECTINSTANCEA { /* 0x13c */
    DWORD dwSize;
    u8 guidType[16];
    DWORD dwOfs, dwType, dwFlags; /* dwType +0x18 */
    char tszName[260];
    DWORD dwFFMaxForce, dwFFForceResolution;
    u16 wCollectionNumber, wDesignatorIndex, wUsagePage, wUsage;
    DWORD dwDimension;
    u16 wExponent, wReportId;
};

typedef BOOL(__stdcall *LPDIENUMDEVICEOBJECTSCALLBACKA)(const DIDEVICEOBJECTINSTANCEA *doi, void *ref);

struct DIMOUSESTATE { /* 0x10 */
    long lX, lY, lZ;
    u8 rgbButtons[4];
};

struct IDirectInputDevice8A : IUnknown {
    virtual HRESULT __stdcall GetCapabilities(DIDEVCAPS *caps) = 0;                                         /* +0x0c */
    virtual HRESULT __stdcall EnumObjects(LPDIENUMDEVICEOBJECTSCALLBACKA cb, void *ref, DWORD flags) = 0;   /* +0x10 */
    virtual HRESULT __stdcall GetProperty(const GUID &prop, DIPROPHEADER *diph) = 0;                        /* +0x14 */
    virtual HRESULT __stdcall SetProperty(const GUID &prop, const DIPROPHEADER *diph) = 0;                  /* +0x18 */
    virtual HRESULT __stdcall Acquire() = 0;                                                                /* +0x1c */
    virtual HRESULT __stdcall Unacquire() = 0;                                                              /* +0x20 */
    virtual HRESULT __stdcall GetDeviceState(DWORD size, void *data) = 0;                                   /* +0x24 */
    virtual HRESULT __stdcall GetDeviceData(DWORD size, void *data, DWORD *count, DWORD flags) = 0;         /* +0x28 */
    virtual HRESULT __stdcall SetDataFormat(const DIDATAFORMAT *fmt) = 0;                                   /* +0x2c */
    virtual HRESULT __stdcall SetEventNotification(void *ev) = 0;                                           /* +0x30 */
    virtual HRESULT __stdcall SetCooperativeLevel(HWND hWnd, DWORD flags) = 0;                              /* +0x34 */
    virtual HRESULT __stdcall GetObjectInfo(DIDEVICEOBJECTINSTANCEA *doi, DWORD obj, DWORD how) = 0;        /* +0x38 */
    virtual HRESULT __stdcall GetDeviceInfo(void *ddi) = 0;                                                 /* +0x3c */
    virtual HRESULT __stdcall RunControlPanel(HWND hWnd, DWORD flags) = 0;                                  /* +0x40 */
    virtual HRESULT __stdcall Initialize(void *hInst, DWORD version, const GUID &guid) = 0;                 /* +0x44 */
    virtual HRESULT __stdcall CreateEffect(const GUID &guid, const void *eff, void **out, void *outer) = 0; /* +0x48 */
    virtual HRESULT __stdcall EnumEffects(void *cb, void *ref, DWORD type) = 0;                             /* +0x4c */
    virtual HRESULT __stdcall GetEffectInfo(void *info, const GUID &guid) = 0;                              /* +0x50 */
    virtual HRESULT __stdcall GetForceFeedbackState(DWORD *state) = 0;                                      /* +0x54 */
    virtual HRESULT __stdcall SendForceFeedbackCommand(DWORD flags) = 0;                                    /* +0x58 */
    virtual HRESULT __stdcall EnumCreatedEffectObjects(void *cb, void *ref, DWORD flags) = 0;               /* +0x5c */
    virtual HRESULT __stdcall Escape(void *esc) = 0;                                                        /* +0x60 */
    virtual HRESULT __stdcall Poll() = 0;                                                                   /* +0x64 */
};

struct IDirectInput8A : IUnknown {
    virtual HRESULT __stdcall CreateDevice(const GUID &rguid, IDirectInputDevice8A **device,
                                           void *outer) = 0; /* +0x0c */
    virtual HRESULT __stdcall EnumDevices(DWORD devType, LPDIENUMDEVICESCALLBACKA callback, void *context,
                                          DWORD flags) = 0; /* +0x10 */
};

SDW_AT(DIDEVICEINSTANCEA, tszInstanceName, 0x28);

/* DINPUT8 export: called through the import library's jmp [IAT] stub (the SDK headers do not dllimport it) */
extern "C" HRESULT __stdcall DirectInput8Create(HINSTANCE hinst, DWORD version, const GUID &iid, void **out,
                                                void *outer); /* 0x565830 */
extern "C" const GUID GUID_SysKeyboard;                       /* 0x577648 GUID_SysKeyboard */
extern "C" const GUID GUID_SysMouse;                          /* 0x577658 GUID_SysMouse */
extern "C" const GUID IID_IDirectInput8A;                     /* 0x5776f8 */
/* dinput8.lib's predefined data formats */
extern "C" const DIDATAFORMAT c_dfDIJoystick; /* 0x5775f0 {0x18, 0x10, DIDF_ABSAXIS, 0x50, 44 objects} */
extern "C" const DIDATAFORMAT c_dfDIKeyboard; /* 0x577608 {0x18, 0x10, DIDF_RELAXIS, 0x100, 256 objects} */
extern "C" const DIDATAFORMAT c_dfDIMouse;    /* 0x577620 */

#endif
