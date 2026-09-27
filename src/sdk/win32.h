/* Stand-in for Win32 (windef.h, winuser.h, wingdi.h, winbase.h, winnt.h, winerror.h, objbase.h, shlobj.h): the constants the decompiled source names, with the SDK's own spelling and value.
 * Only what src/ uses is here: constants, types and functions (depends on sdw_types.h). */
#ifndef SDW_SDK_WIN32_H
#define SDW_SDK_WIN32_H

typedef enum tagCLSCTX {
    CLSCTX_INPROC_SERVER = 1,
} CLSCTX;

#define BI_RGB 0L
#define BM_GETCHECK 0x00F0
#define BM_SETCHECK 0x00F1
#define BM_SETIMAGE 0x00F7
#define BST_CHECKED 0x0001
#define CBM_INIT 0x04L
#define CBN_SELCHANGE 1
#define CB_ADDSTRING 0x0143
#define CB_ERR (-1)
#define CB_GETCURSEL 0x0147
#define CB_GETITEMDATA 0x0150
#define CB_GETLBTEXT 0x0148
#define CB_RESETCONTENT 0x014B
#define CB_SELECTSTRING 0x014D
#define CB_SETCURSEL 0x014E
#define CB_SETITEMDATA 0x0151
#define CF_BITMAP 2
#define COLOR_3DDKSHADOW 21
#define CP_ACP 0
#define CSIDL_DESKTOPDIRECTORY 0x0010
#define CW_USEDEFAULT ((int)0x80000000)
#define DIB_RGB_COLORS 0
#define EM_SETREADONLY 0x00CF
#define ENUM_CURRENT_SETTINGS ((u32)(-1)) /* the SDK casts to DWORD */
#define ERROR_SUCCESS 0L
#define E_FAIL ((HRESULT)0x80004005L)
#define E_NOTIMPL ((HRESULT)0x80004001L)
#define E_OUTOFMEMORY ((HRESULT)0x8007000EL)
#define FALSE 0
#define LR_LOADFROMFILE 0x0010
#define MAKEINTRESOURCEA(i) ((char *)((u32)((u16)(i))))
#define FW_BOLD 700
#define IDC_ARROW MAKEINTRESOURCEA(32512)
#define IDOK 1
#define IDYES 6
#define IMAGE_BITMAP 0
#define INFINITE 0xFFFFFFFF
#define LANG_DUTCH 0x13
#define LANG_ENGLISH 0x09
#define LANG_FRENCH 0x0c
#define LANG_GERMAN 0x07
#define LANG_ITALIAN 0x10
#define LANG_PORTUGUESE 0x16
#define LANG_SPANISH 0x0a
#define LMEM_FIXED 0x0000
#define LMEM_ZEROINIT 0x0040
#define LPTR (LMEM_FIXED | LMEM_ZEROINIT)
#define LR_CREATEDIBSECTION 0x2000
#define MB_ICONEXCLAMATION 0x00000030L
#define MB_ICONHAND 0x00000010L
#define MB_ICONQUESTION 0x00000020L
#define MB_OK 0x00000000L
#define MB_YESNO 0x00000004L
#define MK_CONTROL 0x0008
#define MK_LBUTTON 0x0001
#define NORMAL_PRIORITY_CLASS 0x00000020
#define PM_REMOVE 0x0001
#define QS_ALLINPUT 0x00FF /* QS_INPUT | QS_POSTMESSAGE | QS_TIMER | QS_PAINT | QS_HOTKEY | QS_SENDMESSAGE */
#define QS_TIMER 0x0010
#define REG_OPTION_NON_VOLATILE 0x00000000L
#define SRCCOPY ((u32)0x00CC0020) /* the SDK casts to DWORD */
#define SW_SHOW 5
#define SW_SHOWNORMAL 1
#define S_FALSE ((HRESULT)0x00000001L)
#define S_OK ((HRESULT)0x00000000L)
#define TA_CENTER 6
#define THREAD_BASE_PRIORITY_MAX 2
#define THREAD_PRIORITY_ABOVE_NORMAL (THREAD_PRIORITY_HIGHEST - 1)
#define THREAD_PRIORITY_HIGHEST THREAD_BASE_PRIORITY_MAX
#define TRANSPARENT 1
#define TRUE 1
#define WAIT_OBJECT_0 ((u32)0x00000000L) /* the SDK: STATUS_WAIT_0 + 0, a DWORD */
#define WM_CLOSE 0x0010
#define WM_COMMAND 0x0111
#define WM_DESTROY 0x0002
#define WM_ENABLE 0x000A
#define WM_ENTERMENULOOP 0x0211
#define WM_ERASEBKGND 0x0014
#define WM_EXITMENULOOP 0x0212
#define WM_INITDIALOG 0x0110
#define WM_KEYFIRST 0x0100
#define WM_LBUTTONDOWN 0x0201
#define WM_PAINT 0x000F
#define WM_QUIT 0x0012
#define WM_RBUTTONDBLCLK 0x0206
#define WM_SETFONT 0x0030
#define WM_SETTEXT 0x000C
#define WS_BORDER 0x00800000L
#define WS_POPUP 0x80000000L
#define WS_SYSMENU 0x00080000L

#include "windef.h"

struct BITMAPINFO;
struct BITMAPINFOHEADER;
struct DEVMODEA;
struct ITEMIDLIST;
struct LOGFONTA;
struct MSG;
struct PAINTSTRUCT;
struct TEXTMETRICA;
struct WNDCLASSA;

typedef LRESULT(__stdcall *WNDPROC)(HWND, UINT, WPARAM, LPARAM);

struct MSG {
    HWND hwnd;
    UINT message;
    WPARAM wParam;
    LPARAM lParam;
    DWORD time;
    POINT pt;
}; /* 0x1c */

struct WNDCLASSA { /* 0x28 */
    UINT style;
    WNDPROC lpfnWndProc;
    int cbClsExtra;
    int cbWndExtra;
    HINSTANCE hInstance;
    HICON hIcon;
    HCURSOR hCursor;
    HBRUSH hbrBackground;
    const char *lpszMenuName;
    const char *lpszClassName;
};

struct BITMAP {
    LONG bmType, bmWidth, bmHeight, bmWidthBytes;
    u16 bmPlanes, bmBitsPixel;
    void *bmBits;
}; /* 0x18 */

typedef s32(__stdcall *DLGPROC)(HWND, u32, u32, s32);

struct BITMAPINFOHEADER {
    u32 biSize;
    s32 biWidth, biHeight;
    u16 biPlanes, biBitCount;
    u32 biCompression, biSizeImage;
    s32 biXPelsPerMeter, biYPelsPerMeter;
    u32 biClrUsed, biClrImportant;
};

struct BITMAPINFO {
    BITMAPINFOHEADER bmiHeader;
    u32 bmiColors[1];
};

struct DIBSECTION {
    BITMAP dsBm;
    BITMAPINFOHEADER dsBmih;
    u32 dsBitfields[3];
    void *dshSection;
    u32 dsOffset;
};

struct LOGFONTA {
    s32 lfHeight, lfWidth, lfEscapement, lfOrientation, lfWeight;
    u8 lfItalic, lfUnderline, lfStrikeOut, lfCharSet, lfOutPrecision, lfClipPrecision, lfQuality, lfPitchAndFamily;
    char lfFaceName[32];
};

struct TEXTMETRICA {
    s32 tmHeight, tmAscent, tmDescent, tmInternalLeading, tmExternalLeading, tmAveCharWidth, tmMaxCharWidth, tmWeight,
        tmOverhang, tmDigitizedAspectX, tmDigitizedAspectY;
    u8 tmFirstChar, tmLastChar, tmDefaultChar, tmBreakChar, tmItalic, tmUnderlined, tmStruckOut, tmPitchAndFamily,
        tmCharSet;
};

struct PAINTSTRUCT {
    HDC hdc;
    s32 fErase;
    RECT rcPaint;
    s32 fRestore, fIncUpdate;
    u8 rgbReserved[32];
};

struct DEVMODEA { /* 0x94 bytes (WINVER 0x0400) */
    char dmDeviceName[32];
    u16 dmSpecVersion, dmDriverVersion, dmSize, dmDriverExtra; /* dmSize +0x24 */
    DWORD dmFields;
    u8 dmPrinterFields[0x68 - 0x2c]; /* dmOrientation .. dmLogPixels */
    DWORD dmBitsPerPel;              /* +0x68 */
    DWORD dmPelsWidth, dmPelsHeight, dmDisplayFlags, dmDisplayFrequency;
    DWORD dmICMMethod, dmICMIntent, dmMediaType, dmDitherType, dmReserved1, dmReserved2;
};

typedef DWORD(__stdcall *LPTHREAD_START_ROUTINE)(void *arg);

struct IShellLinkA : IUnknown {
    virtual HRESULT __stdcall GetPath(char *file, int n, void *fd, DWORD flags) = 0; /* +0x0c */
    virtual HRESULT __stdcall GetIDList(ITEMIDLIST **pidl) = 0;                      /* +0x10 */
    virtual HRESULT __stdcall SetIDList(const ITEMIDLIST *pidl) = 0;                 /* +0x14 */
    virtual HRESULT __stdcall GetDescription(char *name, int n) = 0;                 /* +0x18 */
    virtual HRESULT __stdcall SetDescription(const char *name) = 0;                  /* +0x1c */
    virtual HRESULT __stdcall GetWorkingDirectory(char *dir, int n) = 0;             /* +0x20 */
    virtual HRESULT __stdcall SetWorkingDirectory(const char *dir) = 0;              /* +0x24 */
    virtual HRESULT __stdcall GetArguments(char *args, int n) = 0;                   /* +0x28 */
    virtual HRESULT __stdcall SetArguments(const char *args) = 0;                    /* +0x2c */
    virtual HRESULT __stdcall GetHotkey(u16 *key) = 0;                               /* +0x30 */
    virtual HRESULT __stdcall SetHotkey(u16 key) = 0;                                /* +0x34 */
    virtual HRESULT __stdcall GetShowCmd(int *cmd) = 0;                              /* +0x38 */
    virtual HRESULT __stdcall SetShowCmd(int cmd) = 0;                               /* +0x3c */
    virtual HRESULT __stdcall GetIconLocation(char *path, int n, int *icon) = 0;     /* +0x40 */
    virtual HRESULT __stdcall SetIconLocation(const char *path, int icon) = 0;       /* +0x44 */
    virtual HRESULT __stdcall SetRelativePath(const char *path, DWORD reserved) = 0; /* +0x48 */
    virtual HRESULT __stdcall Resolve(HWND hWnd, DWORD flags) = 0;                   /* +0x4c */
    virtual HRESULT __stdcall SetPath(const char *file) = 0;                         /* +0x50 */
};

struct IPersistFile : IPersist {
    virtual HRESULT __stdcall IsDirty() = 0;                              /* +0x10 */
    virtual HRESULT __stdcall Load(const WCHAR *name, DWORD mode) = 0;    /* +0x14 */
    virtual HRESULT __stdcall Save(const WCHAR *name, BOOL remember) = 0; /* +0x18 */
};

SDW_AT(DEVMODEA, dmSize, 0x24);
SDW_AT(DEVMODEA, dmBitsPerPel, 0x68);
SDW_SIZE(DEVMODEA, 0x94);

extern "C" __declspec(dllimport) HDC __stdcall BeginPaint(HWND hWnd, PAINTSTRUCT *lpPaint);
extern "C" __declspec(dllimport) BOOL __stdcall BitBlt(HDC hdc, int x, int y, int cx, int cy, HDC hdcSrc, int x1,
                                                       int y1, DWORD rop);
extern "C" const GUID CLSID_ShellLink; /* 0x5777a8 {00021401-0000-0000-C000-000000000046} */
extern "C" __declspec(dllimport) BOOL __stdcall ClientToScreen(HWND hWnd, POINT *point);
extern "C" __declspec(dllimport) BOOL __stdcall CloseClipboard();
extern "C" __declspec(dllimport) BOOL __stdcall CloseHandle(HANDLE h);
extern "C" __declspec(dllimport) HRESULT __stdcall CoCreateInstance(const GUID &clsid, void *outer, DWORD ctx,
                                                                    const GUID &iid, void **out);
extern "C" __declspec(dllimport) HRESULT __stdcall CoInitialize(void *reserved);
extern "C" __declspec(dllimport) void __stdcall CoUninitialize();
extern "C" __declspec(dllimport) HBITMAP __stdcall CreateCompatibleBitmap(HDC dc, int w, int h);
extern "C" __declspec(dllimport) HDC __stdcall CreateCompatibleDC(HDC dc);
extern "C" __declspec(dllimport) HBITMAP __stdcall CreateDIBitmap(HDC hdc, const BITMAPINFOHEADER *pbmih, DWORD flInit,
                                                                  const void *pjBits, const BITMAPINFO *pbmi,
                                                                  UINT iUsage);
extern "C" __declspec(dllimport) HANDLE __stdcall CreateEventA(void *sa, BOOL manualReset, BOOL initialState,
                                                               const char *name);
extern "C" __declspec(dllimport) HFONT __stdcall CreateFontIndirectA(const LOGFONTA *lplf);
extern "C" __declspec(dllimport) HANDLE __stdcall CreateMutexA(void *lpMutexAttributes, BOOL bInitialOwner,
                                                               const char *lpName);
extern "C" __declspec(dllimport) HANDLE __stdcall CreateThread(void *sa, DWORD stack, LPTHREAD_START_ROUTINE proc,
                                                               void *arg, DWORD flags, DWORD *threadId);
extern "C" __declspec(dllimport) HWND __stdcall CreateWindowExA(DWORD exStyle, const char *cls, const char *title,
                                                                DWORD style, int x, int y, int w, int h, HWND parent,
                                                                HMENU menu, HINSTANCE hInst, void *param);
extern "C" __declspec(dllimport) LRESULT __stdcall DefWindowProcA(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern "C" __declspec(dllimport) BOOL __stdcall DeleteDC(HDC dc);
extern "C" __declspec(dllimport) BOOL __stdcall DeleteObject(HGDIOBJ ho);
extern "C" __declspec(dllimport) BOOL __stdcall DestroyWindow(HWND hWnd);
extern "C" __declspec(dllimport) int __stdcall DialogBoxParamA(HINSTANCE hInstance, const char *lpTemplateName,
                                                               HWND hWndParent, DLGPROC lpDialogFunc,
                                                               LPARAM dwInitParam);
/* dsetup.lib: called through its import thunk 0x5644e0 (a direct call, not through the IAT) */
extern "C" int __stdcall DirectXSetupGetVersion(DWORD *version, DWORD *revision);
extern "C" __declspec(dllimport) LRESULT __stdcall DispatchMessageA(const MSG *msg);
extern "C" __declspec(dllimport) BOOL __stdcall EmptyClipboard();
extern "C" __declspec(dllimport) BOOL __stdcall EndDialog(HWND hDlg, int nResult);
extern "C" __declspec(dllimport) BOOL __stdcall EndPaint(HWND hWnd, const PAINTSTRUCT *lpPaint);
extern "C" __declspec(dllimport) BOOL __stdcall EnumDisplaySettingsA(const char *device, DWORD modeNum, DEVMODEA *mode);
extern "C" __declspec(dllimport) HWND __stdcall FindWindowA(const char *cls, const char *title);
extern "C" __declspec(dllimport) HWND __stdcall GetActiveWindow();
extern "C" __declspec(dllimport) BOOL __stdcall GetClientRect(HWND hWnd, RECT *rect);
extern "C" __declspec(dllimport) char *__stdcall GetCommandLineA();
extern "C" __declspec(dllimport) HDC __stdcall GetDC(HWND hWnd);
extern "C" __declspec(dllimport) int __stdcall GetDIBits(HDC hdc, HBITMAP hbm, UINT start, UINT cLines, void *lpvBits,
                                                         BITMAPINFO *lpbmi, UINT usage);
extern "C" __declspec(dllimport) HWND __stdcall GetDlgItem(HWND hDlg, int nIDDlgItem);
extern "C" __declspec(dllimport) int __stdcall GetObjectA(HGDIOBJ obj, int size, void *out);
extern "C" __declspec(dllimport) BOOL __stdcall GetTextMetricsA(HDC hdc, TEXTMETRICA *lptm);
extern "C" __declspec(dllimport) WORD __stdcall GetUserDefaultLangID();
extern "C" __declspec(dllimport) BOOL __stdcall GetWindowRect(HWND hWnd, RECT *lpRect);
extern "C" const GUID IID_IPersistFile; /* 0x5777c8 {0000010B-0000-0000-C000-000000000046} */
extern "C" const GUID IID_IShellLinkA;  /* 0x5777b8 {000214EE-0000-0000-C000-000000000046} */
extern "C" __declspec(dllimport) HBITMAP __stdcall LoadBitmapA(HINSTANCE hInstance, const char *lpBitmapName);
extern "C" __declspec(dllimport) HCURSOR __stdcall LoadCursorA(HINSTANCE hInst, const char *name);
extern "C" __declspec(dllimport) HICON __stdcall LoadIconA(HINSTANCE hInst, const char *name);
extern "C" __declspec(dllimport) HANDLE __stdcall LoadImageA(HINSTANCE hInst, const char *name, UINT type, int cx,
                                                             int cy, UINT fuLoad);
extern "C" __declspec(dllimport) HANDLE __stdcall LocalAlloc(UINT uFlags, UINT uBytes);
extern "C" __declspec(dllimport) int __stdcall MessageBoxA(HWND hWnd, const char *text, const char *caption, UINT type);
extern "C" __declspec(dllimport) DWORD __stdcall MsgWaitForMultipleObjects(DWORD count, HANDLE *handles, BOOL waitAll,
                                                                           DWORD ms, DWORD wakeMask);
extern "C" __declspec(dllimport) int __stdcall MultiByteToWideChar(UINT cp, DWORD flags, const char *s, int n, WCHAR *w,
                                                                   int nw);
extern "C" __declspec(dllimport) BOOL __stdcall OpenClipboard(HWND hWnd);
extern "C" __declspec(dllimport) void __stdcall OutputDebugStringA(const char *lpOutputString);
extern "C" __declspec(dllimport) BOOL __stdcall PeekMessageA(MSG *msg, HWND hWnd, UINT first, UINT last, UINT remove);
extern "C" __declspec(dllimport) void __stdcall PostQuitMessage(int code);
extern "C" __declspec(dllimport) BOOL __stdcall QueryPerformanceCounter(s64 *lpPerformanceCount);
extern "C" __declspec(dllimport) BOOL __stdcall QueryPerformanceFrequency(s64 *lpFrequency);
extern "C" __declspec(dllimport) LONG __stdcall RegCloseKey(HKEY hKey);
extern "C" __declspec(dllimport) LONG __stdcall RegCreateKeyExA(HKEY hKey, const char *lpSubKey, DWORD Reserved,
                                                                char *lpClass, DWORD dwOptions, DWORD samDesired,
                                                                void *lpSecurityAttributes, HKEY *phkResult,
                                                                DWORD *lpdwDisposition);
extern "C" __declspec(dllimport) LONG __stdcall RegDeleteKeyA(HKEY hKey, const char *lpSubKey);
extern "C" __declspec(dllimport) LONG __stdcall RegOpenKeyExA(HKEY hKey, const char *lpSubKey, DWORD ulOptions,
                                                              DWORD samDesired, HKEY *phkResult);
extern "C" __declspec(dllimport) LONG __stdcall RegQueryValueExA(HKEY hKey, const char *lpValueName, DWORD *lpReserved,
                                                                 DWORD *lpType, BYTE *lpData, DWORD *lpcbData);
extern "C" __declspec(dllimport) LONG __stdcall RegSetValueExA(HKEY hKey, const char *lpValueName, DWORD Reserved,
                                                               DWORD dwType, const BYTE *lpData, DWORD cbData);
extern "C" __declspec(dllimport) ATOM __stdcall RegisterClassA(const WNDCLASSA *wc);
extern "C" __declspec(dllimport) int __stdcall ReleaseDC(HWND hWnd, HDC dc);
extern "C" __declspec(dllimport) BOOL __stdcall ReleaseMutex(HANDLE hMutex);
extern "C" __declspec(dllimport) BOOL __stdcall ResetEvent(HANDLE h);
extern "C" __declspec(dllimport) BOOL __stdcall RestoreDC(HDC dc, int saved);
extern "C" __declspec(dllimport) BOOL __stdcall SHGetPathFromIDListA(const ITEMIDLIST *pidl, char *path);
extern "C" __declspec(dllimport) HRESULT __stdcall SHGetSpecialFolderLocation(HWND hWnd, int csidl, ITEMIDLIST **pidl);
extern "C" __declspec(dllimport) int __stdcall SaveDC(HDC dc);
extern "C" __declspec(dllimport) BOOL __stdcall ScreenToClient(HWND hWnd, POINT *lpPoint);
extern "C" __declspec(dllimport) HGDIOBJ __stdcall SelectObject(HDC dc, HGDIOBJ obj);
extern "C" __declspec(dllimport) LRESULT __stdcall SendDlgItemMessageA(HWND hDlg, int nIDDlgItem, UINT Msg,
                                                                       WPARAM wParam, LPARAM lParam);
extern "C" __declspec(dllimport) LRESULT __stdcall SendMessageA(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
extern "C" __declspec(dllimport) DWORD __stdcall SetBkColor(HDC hdc, DWORD color);
extern "C" __declspec(dllimport) int __stdcall SetBkMode(HDC hdc, int mode);
extern "C" __declspec(dllimport) HANDLE __stdcall SetClipboardData(UINT format, HANDLE data);
extern "C" __declspec(dllimport) HWND __stdcall SetFocus(HWND hWnd);
extern "C" __declspec(dllimport) BOOL __stdcall SetPriorityClass(HANDLE hProcess, DWORD dwPriorityClass);
extern "C" __declspec(dllimport) BOOL __stdcall SetRect(RECT *rect, int left, int top, int right, int bottom);
extern "C" __declspec(dllimport) UINT __stdcall SetTextAlign(HDC hdc, UINT align);
extern "C" __declspec(dllimport) DWORD __stdcall SetTextColor(HDC hdc, DWORD color);
extern "C" __declspec(dllimport) BOOL __stdcall SetThreadPriority(HANDLE hThread, int nPriority);
extern "C" __declspec(dllimport) BOOL __stdcall SetWindowTextA(HWND hWnd, const char *lpString);
extern "C" __declspec(dllimport) HINSTANCE __stdcall ShellExecuteA(HWND hwnd, const char *lpOperation,
                                                                   const char *lpFile, const char *lpParameters,
                                                                   const char *lpDirectory, int nShowCmd);
extern "C" __declspec(dllimport) int __stdcall ShowCursor(BOOL show);
extern "C" __declspec(dllimport) BOOL __stdcall ShowWindow(HWND hWnd, int cmd);
extern "C" __declspec(dllimport) void __stdcall Sleep(DWORD ms);
extern "C" __declspec(dllimport) BOOL __stdcall StretchBlt(HDC dst, int x, int y, int w, int h, HDC src, int sx, int sy,
                                                           int sw, int sh, DWORD rop);
extern "C" __declspec(dllimport) BOOL __stdcall TextOutA(HDC hdc, int x, int y, const char *text, int len);
extern "C" __declspec(dllimport) BOOL __stdcall TranslateMessage(const MSG *msg);
extern "C" __declspec(dllimport) DWORD __stdcall WaitForSingleObject(HANDLE h, DWORD ms);
extern "C" __declspec(dllimport) int __stdcall WideCharToMultiByte(UINT cp, DWORD flags, const WCHAR *w, int nw,
                                                                   char *s, int n, const char *defChar, BOOL *usedDef);
extern "C" __declspec(dllimport) int __cdecl wsprintfA(char *out, const char *format, ...);

#endif
