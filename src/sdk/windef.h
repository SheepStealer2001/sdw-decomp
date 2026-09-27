/* Stand-in for the basic Windows types (windef.h, winnt.h, wtypes.h, unknwn.h): the ones the other stand-ins and the
 * decompiled source use, spelled as the SDK spells them with STRICT handles. Only what src/ uses is here. */
#ifndef SDW_SDK_WINDEF_H
#define SDW_SDK_WINDEF_H

#include "sdw_types.h"

typedef unsigned long DWORD;
typedef unsigned long ULONG;
typedef long LONG;
typedef int BOOL;
typedef unsigned int UINT;
typedef unsigned short WORD;
typedef unsigned short WCHAR;
typedef unsigned short ATOM;
typedef unsigned int WPARAM;
typedef long LPARAM;
typedef long LRESULT;
typedef long HRESULT;
typedef void *HANDLE;
typedef void *HGDIOBJ;
typedef unsigned char BYTE;

/* DECLARE_HANDLE under STRICT: each handle is a pointer to its own incomplete struct */
struct HWND__;
typedef HWND__ *HWND;
struct HINSTANCE__;
typedef HINSTANCE__ *HINSTANCE;
struct HDC__;
typedef HDC__ *HDC;
struct HBITMAP__;
typedef HBITMAP__ *HBITMAP;
struct HBRUSH__;
typedef HBRUSH__ *HBRUSH;
struct HICON__;
typedef HICON__ *HICON;
typedef HICON HCURSOR;
struct HFONT__;
typedef HFONT__ *HFONT;
struct HMENU__;
typedef HMENU__ *HMENU;
struct HKEY__;
typedef HKEY__ *HKEY;

struct GUID { /* 0x10 bytes; the evidence is in data/structs/GUID.csv */
    DWORD Data1;
    WORD Data2;
    WORD Data3;
    BYTE Data4[8];
};

struct RECT { /* the evidence is in data/structs/RECT.csv */
    LONG left, top, right, bottom;
};

struct POINT {
    LONG x, y;
};

struct IUnknown {
    virtual HRESULT __stdcall QueryInterface(const GUID &riid, void **ppv) = 0; /* +0x00 */
    virtual ULONG __stdcall AddRef() = 0;                                       /* +0x04 */
    virtual ULONG __stdcall Release() = 0;                                      /* +0x08 */
};

struct IPersist : IUnknown {
    virtual HRESULT __stdcall GetClassID(GUID *) = 0; /* +0x0c */
};

/* The layout checks of sdw_classes.h, for the SDK structs defined in these stand-ins */
#define SDW_OFF(T, m) ((unsigned)&((T *)0)->m)
#define SDW_AT(T, m, off) typedef char T##_##m##_at[(SDW_OFF(T, m) == (off)) ? 1 : -1]
#define SDW_SIZE(T, n) typedef char T##_size_is[(sizeof(T) == (n)) ? 1 : -1]

#endif
