/* match-init: StaticInit_g_camera */
/*
 * Object T001 (data/tu_map.json), guessed original file: WinMain, the application and launcher shell (the window class is
 * 'Sheep, Dog'n Wolf D3D').
 *   .text 0x401000-0x403f29 (DX_IsVersionOk, the g_camera initialiser, WinMain .. Launcher_MakeButtonBitmap; no COMDATs)
 *   .data 0x579100-0x5793e4: the initialised globals (an unreferenced byte table, g_langMask, g_startLevelId,
 *         g_launcherInputMode, an unreferenced u16, g_maxFps), then the string literals in text order
 *   .bss  0x584c50-0x585068: g_camera, then the launcher's and the loader's globals (see the definitions below)
 *   .CRT$XCU: 0x401030 (g_camera)
 * The functions are in address order: the application shell, then the launcher dialogs, options, desktop shortcut and
 * button bitmaps. This file DEFINES the .data/.bss globals of 0x579100-0x5793e4 and 0x584c50-0x585068.
 *
 * .bss order: VC6 puts a file's uninitialised globals and those with constructors first, ordered by a hash of their
 * names, and the globals explicitly initialised to zero after them in definition order. The exe has g_camera
 * (constructed) first and the rest in address order, so every other .bss global is written `= 0`.
 * Unreferenced storage (no code or data in the exe refers to it: 0x584d30 512 bytes, 0x584fdc 4, 0x585000 44,
 * 0x585038 8; .data 0x579100 10 bytes and 0x57910e) is defined as opaque placeholders named by address: their
 * original types and names are unknown, only their sizes are fixed by the layout.
 * Launcher_OpenUrl's verb and App_OfferDesktopShortcut's literals are plain string literals, not static const arrays:
 * VC6 puts those in .rdata, and the exe has the bytes among .data's literals.
 *
 * Start-up order (WinMain): the eleven content-path prefixes are built from the exe directory; a second instance (FindWindow
 * on the class name) exits silently; GREET_txt.BSM (the localised strings) must load or the game exits; the language mask
 * comes from the system's default language; the window is created (800x600); DirectX 8 is checked (App_EnsureDirectX may
 * run dxsetup); the D3DApp and the SoundDevice are created; the desktop shortcut is offered; the launcher dialog runs; then
 * the D3D device, the sound device (22050 Hz, 16 bit), App_InitGameSystems, Main_Loop until WM_QUIT, App_Shutdown.
 */
/* BYTES: dead-code, layout, slot-group, slot-name, switches, temp, view. */
/* BYTES(layout): written '= 0' only to keep definition order in .bss (VC6 hashes the names of uninitialised globals) */
/* BYTES(layout): placeholder: unreferenced storage kept only for its size and position; name and type unknown */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "../sdk/win32.h"
#include "../sdk/ddraw.h"
#include "../sdk/crt.h"

struct ITEMIDLIST;

#define WINAPI_IMPORT extern "C" __declspec(dllimport)

/* ---- the game's classes: members this file defines or calls ---- */
#define SDW_MEMBERS_D3DApp                                                                          \
    D3DApp(HWND hWnd, HINSTANCE hInstance, u8 filterFlags); /* 0x40464f D3DApp_Construct */         \
    inline HRESULT ShowFrame();                             /* source-only inline, defined below */ \
    IDirectDrawSurface7 *GetBackBuffer()                                                            \
    {                                                                                               \
        return pBackBuffer;                                                                         \
    } /* source-only inline accessor */
#define SDW_MEMBERS_SoundDevice SoundDevice();      /* 0x4065a0 SoundDevice_Construct */
#define SDW_MEMBERS_Frustrum Frustrum(D3DApp *app); /* 0x418ad0 Frustrum_Construct */
#define SDW_MEMBERS_Progress \
    void SetField80()        \
    {                        \
        field80 = 1;         \
    } /* source-only inline (App_InitGameSystems 0x401a20) */
#define SDW_MEMBERS_Mat44 Mat44(); /* 0x4077f0 Mat44_Ctor */
#include "sdw_classes.h"

/* source-only inline: the DX7 framework's ShowFrame, expanded in App_InitGameSystems (0x401943-0x4019f1): Blt the back
 * buffer into the window rectangle when the selected mode is the desktop-compatible "Windowed Mode" pseudo-mode 0, else
 * Flip. */
inline HRESULT D3DApp::ShowFrame()
{
    HRESULT hr;
    if (pPrimary == NULL)
        hr = E_FAIL;
    else if (devices[deviceIndex].bDesktopCompatible && devices[deviceIndex].modeIndex == 0)
        hr = pPrimary->Blt(&clientRect, pBackBuffer, NULL, DDBLT_WAIT, NULL);
    else
        hr = pPrimary->Flip(NULL, DDFLIP_WAIT);
    return hr;
}

/* ---- the game's functions ---- */
void Launcher_ShowMainDialog(HWND hParent, HINSTANCE hInst); /* 0x401c8c */
#include "../engine/time.h"
#include "../engine/scenaric_loop.h"
#include "../engine/input.h"
#include "../engine/cheat.h"
#include "../engine/fade.h"
#include "../engine/scenaric.h"
#include "../engine/progress.h"
#include "../engine/debug_draw.h"
#include "../engine/game_level.h"
#include "../engine/registry.h"
#include "../engine/draw2d.h"
#include "../engine/pause_menu.h"
#include "../engine/screen.h"
#include "../engine/game_state.h"
#include "../engine/stream_player.h"
#include "../objects/video_sequence.h"
u8 Video_PlaySequence(FmvList *list); /* 0x56160f */
u16 App_GetLanguageMask();
HRESULT App_OfferDesktopShortcut();
u8 App_EnsureDirectX();
s32 App_InitGameSystems();
void Main_Loop(D3DApp *app);
void App_Shutdown();
LRESULT __stdcall App_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
/* the launcher (all in this object) and the registry wrappers */
s32 __stdcall Launcher_EasterEggDlgProc(HWND, u32, u32, s32);
s32 __stdcall Launcher_MainDlgProc(HWND, u32, u32, s32);
s32 __stdcall Launcher_OptionsDlgProc(HWND, u32, u32, s32);
void Launcher_ShowOptionsDialog(HWND, HINSTANCE, u8);
void Launcher_FillModeCombo(HWND, const char *);
void Launcher_OpenUrl(const char *, u16);
HBITMAP Launcher_MakeButtonBitmap(HWND, const char *, const char *, u32);
LONG Reg_CreateSubKey(HKEY *, const char *); /* 0x55f84e */
void Reg_CloseKey(HKEY *);
DWORD Reg_ReadBinary(HKEY, const char *, void *, DWORD);
u8 Reg_WriteBinary(HKEY, const char *, const void *, DWORD);

/* ---- globals (addresses in SheepD3D.exe) ---- */
extern HINSTANCE g_hInstance; /* 0x6d5218 */
extern HWND g_hGameWindow;    /* 0x6d6eb8 */
extern u32 g_gameFlags;       /* 0x6ddf74 */

/* ---- this object's .data (0x579100-0x579114), in address order; the string literals follow in text order ---- */
u8 g_unref_579100[10] = {0, 8, 8, 4, 2, 2, 4, 2, 2, 0}; /* 0x579100  no code or data refers to it (a size table?) */
u16 g_langMask = LANGMASK_ENGLISH; /* 0x57910a  App_GetLanguageMask's bit; 1 (English) until WinMain sets it */
s8 g_startLevelId = -1;            /* 0x57910c  the scene App_InitGameSystems starts (-1: the title scene) */
u8 g_launcherInputMode = INPUTMODE_KEYBOARD; /* 0x57910d  keyboard only, or a joystick is selected */
u16 g_unref_57910e = 1;                      /* 0x57910e  no code or data refers to it */
u32 g_maxFps = 60;                           /* 0x579110  the frame cap passed to Render_Present */

/* ---- this object's .bss after g_camera (0x584d30-0x585068), zero-initialised so that they keep this order ---- */
u8 g_unref_584d30[0x200] = {0};           /* 0x584d30  no code or data refers to it */
LOGFONTA g_launcherLogFont = {0};         /* 0x584f30  the dialog font (WM_SETFONT), bold for the button captions */
LauncherSetup g_launcherSetup = {0};      /* 0x584f70  the registry "Setup" value */
Dav g_levelDav = {0};                     /* 0x584fc0  the loaded level's .DAV */
u8 g_unref_584fdc[4] = {0};               /* 0x584fdc  no code or data refers to it */
HBITMAP g_launcherButtonBitmaps[8] = {0}; /* 0x584fe0  the launcher's captioned button bitmaps */
u8 g_unref_585000[44] = {0};              /* 0x585000  no code or data refers to it */
u16 g_worldObjCount = 0;                  /* 0x58502c */
u16 g_scnActiveBaseCount = 0;             /* 0x58502e */
u16 g_scnObjectCount = 0;                 /* 0x585030 */
u16 g_scnActiveHigh = 0;                  /* 0x585032 */
u16 g_cineObjectCount = 0;                /* 0x585034 */
u16 g_scnActiveCapacity = 0;              /* 0x585036 */
u8 g_unref_585038[8] = {0};               /* 0x585038  no code or data refers to it */
u32 *g_screenLayerBase0 = 0;              /* 0x585040 */
u32 *g_screenLayerBase = 0;               /* 0x585044 */
u16 *g_pWarCollMap = 0;                   /* 0x585048  the WAR collision grid (resource type 0x80) */
CollTri *g_pWarCollTris = 0;              /* 0x58504c  the WAR collision triangles (type 0x81) */
void *g_animNameTable = 0;                /* 0x585050 */
WorldObj **g_worldObjs = 0;               /* 0x585054 */
ScnObject **g_scnActive = 0;              /* 0x585058 */
ScnObject **g_scnObjects = 0;             /* 0x58505c */
ScnObject **g_cineObjects = 0;            /* 0x585060 */
u8 g_launcherSetupLoaded = 0;             /* 0x585064  the "Setup" value was read from the registry */
char g_launcherEmptyText[1] = {0};        /* 0x585065  "" */

/* 0x401000 - 1 when DirectX 8.0 or later is installed (dsetup's version word 0x00040008).
 * OPTIMISED: only `#pragma optimize("g", on)` reproduces it (each return gets its own epilogue, and the function is padded
 * to 16 bytes with nops, 0x40102c-0x40102f); /Od gives a jmp to one shared epilogue. With the padding it may well be a
 * separate original file compiled with optimisation (the DX SDK's version check), ahead of this one. */
#pragma optimize("g", on)
/* BYTES(switches): built with global optimisation: only /Og gives each return its own epilogue (probably a separately compiled original file) */
u8 DX_IsVersionOk()
{
    DWORD version;
    DWORD revision;
    if (DirectXSetupGetVersion(&version, &revision) && version >= 0x40008)
        return 1;
    return 0;
}
#pragma optimize("", on)

/* 0x401030 / 0x40103a - the static initialiser of the game camera. Camera's implicit constructor runs its two Mat44
 * members' (empty) constructors, at +0x40 and +0x80. */
Camera g_camera; /* 0x584c50 */

/* 0x401053 - the directory the exe lives in, from the command line: a leading quote is removed by shifting the string
 * left, then the string is cut at the last '\' or '/'. Everything after the exe path (arguments) is searched too. */
void App_GetExeDir(char *out)
{
    char *p;
    strcpy(out, GetCommandLineA());
    p = out;
    if (p != NULL && *p != 0) {
        if (*p == '"') {
            while (p[1] != 0) {
                *p = p[1];
                p++;
            }
        } else {
            while (*p != 0)
                p++;
        }
        while (p > out && *p != '\\' && *p != '/')
            p--;
        if (*p == '\\' || *p == '/')
            *p = 0;
    }
}

/* 0x401114 */
int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char *lpCmdLine, int nShowCmd)
{
    WNDCLASSA wc;
    char path[512];
    char szTitle[256];
    char exeDir[512];

    App_GetExeDir(exeDir);
    strcpy(g_levelPathFmt, exeDir);
    strcpy(g_pathScene, exeDir);
    strcpy(g_pathWheelDir, exeDir);
    strcpy(g_introDir, exeDir);
    strcpy(g_pathFendDir, exeDir);
    strcpy(g_pathEnding, exeDir);
    strcpy(g_pathDemoDir, exeDir);
    strcpy(g_musicsPath, exeDir);
    strcpy(g_voiceDir, exeDir);
    strcpy(g_dirReference, exeDir);
    strcpy(g_dirBonusGame, exeDir);
    strcat(g_levelPathFmt, ".\\Levels\\Lvl-%02d\\Lvl-%02d");
    strcat(g_pathScene, ".\\Levels\\Scene\\Scene");
    strcat(g_pathWheelDir, ".\\Levels\\Wheel\\Wheel");
    strcat(g_introDir, ".\\Levels\\Intro\\Intro");
    strcat(g_pathFendDir, ".\\Levels\\Fend\\Fend");
    strcat(g_pathEnding, ".\\Levels\\Ending\\Ending");
    strcat(g_pathDemoDir, ".\\Levels\\Demos");
    strcat(g_musicsPath, ".\\Musics\\");
    strcat(g_voiceDir, ".\\Voices\\");
    strcat(g_dirReference, ".\\References\\");
    strcat(g_dirBonusGame, ".\\Bonus\\");
    strncpy(szTitle, "Sheep, Dog'n Wolf D3D", 0xff);

    wc.style = 0;
    wc.lpfnWndProc = App_WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIconA(hInstance, MAKEINTRESOURCEA(IDI_APP_ICON));
    wc.hCursor = LoadCursorA(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_3DDKSHADOW + 1); /* cast kept: a class brush may be a system colour index + 1 */
    wc.lpszMenuName = szTitle;
    wc.lpszClassName = szTitle;
    if (FindWindowA(szTitle, szTitle))
        return 0; /* already running */

    strcpy(path, g_dirReference);
    strcat(path, "GREET_txt.BSM");
    if (g_textCatalog.Load(path) == 0) {
        fcloseall();
        exit(0);
    }
    g_langMask = App_GetLanguageMask();

    if (RegisterClassA(&wc)) {
        g_hInstance = hInstance;
        g_hGameWindow = CreateWindowExA(0, szTitle, szTitle, WS_SYSMENU | WS_BORDER | WS_POPUP, CW_USEDEFAULT,
                                        CW_USEDEFAULT, 800, 600, NULL, NULL, hInstance, NULL);
        if (g_hGameWindow) {
            if (App_EnsureDirectX() != 1)
                return 0;
            g_pD3DAppMain = new D3DApp(g_hGameWindow, hInstance, 3);
            g_pSoundSystem = new SoundDevice;
            App_OfferDesktopShortcut();
            Launcher_ShowMainDialog(g_hGameWindow, hInstance);
        } else {
            MessageBoxA(g_hGameWindow, "Unable to create rendering window : Exiting SheepD3D", "SheepD3D ERROR",
                        MB_ICONHAND);
            return 0;
        }
    } else {
        MessageBoxA(g_hGameWindow, "Unable to register window class : Exiting SheepD3D", "SheepD3D ERROR", MB_ICONHAND);
        return 0;
    }

    if (g_pD3DAppMain->CreateDevice() < 0) {
        MessageBoxA(g_hGameWindow, g_textCatalog.LoadString(8, g_langMask, 0x40),
                    g_textCatalog.LoadString(1, g_langMask, 0x10), MB_ICONHAND);
        return 0;
    }
    if (g_pSoundSystem->Init(g_hGameWindow, 22050, 16) < 0)
        MessageBoxA(g_hGameWindow, g_textCatalog.LoadString(8, g_langMask, 0x80),
                    g_textCatalog.LoadString(1, g_langMask, 0x10), MB_ICONHAND);
    App_InitGameSystems();
    Main_Loop(g_pD3DAppMain);
    App_Shutdown();
    return 0;
}

/* 0x401614 - the message pump and one game frame per iteration, until WM_QUIT. While another window is active the
 * screen DC is saved; when the game gets focus back it is restored and the menu noise texture is dropped (it is rebuilt
 * on demand). The frame itself (Render_BeginFrame .. Render_EndFrame) runs only while the D3D device is ready; the
 * present / frame limiter (Render_Present with g_maxFps) runs every iteration. bInactive is written and never read. */
/* BYTES(dead-code): bInactive is written and never read; kept because the original stores it */
void Main_Loop(D3DApp *app)
{
    u8 quitRequested = 0;
    u8 bInactive = 0;
    HDC hdc = GetDC(app->hWnd);
    BOOL dcSaved = 0;
    MSG msg;

    while (quitRequested != 1) {
        if (!g_pSoundSystem->PollStreamEvents()) {
            while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) {
                switch (msg.message) {
                    case WM_QUIT:
                        ReleaseDC(app->hWnd, hdc);
                        quitRequested = 1;
                        break;
                    default:
                        TranslateMessage(&msg);
                        DispatchMessageA(&msg);
                }
            }
        }
        if (quitRequested)
            continue;
        if (GetActiveWindow() != app->hWnd) {
            if (!dcSaved) {
                SaveDC(hdc);
                dcSaved = 1;
            }
            bInactive = 1;
        } else if (dcSaved) {
            if (RestoreDC(hdc, -1)) {
                dcSaved = 0;
                if (g_pMenuNoiseTexture) {
                    delete g_pMenuNoiseTexture;
                    g_pMenuNoiseTexture = NULL;
                }
                bInactive = 0;
            }
        }
        Time_Update();
        if (g_pPolyBin && g_pPolyBin->renderer && g_pPolyBin->renderer->deviceReady) {
            g_pPolyBin->Render_BeginFrame();
            if ((g_gameFlags & GF_LEVEL_LOADED_A) && (g_gameFlags & GF_LEVEL_LOADED_B))
                Game_Frame_2();
            Input_Poll();
            Cheat_Poll();
            if (Cheat_IsLevelSkipRequested())
                Fade_StartLevelExit(0x1000);
            /* 0x4017b7 Main_Loop_ExitCheck: a pending level exit is applied in the same iteration */
            if (g_levelExitFlags)
                Level_ExitUpdate();
            g_pPolyBin->Render_EndFrame(1);
        }
        g_pPolyBin->Render_Present(g_maxFps);
    }
}

/* 0x4017ec - the render window's procedure. Entering a menu loop (the system menu, Alt) releases the input devices and
 * shows the cursor; leaving it re-acquires them. */
LRESULT __stdcall App_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
        case WM_DESTROY:
        case WM_CLOSE:
            PostQuitMessage(0);
            break;
        case WM_ENTERMENULOOP:
            g_inputMgr.SetAcquiredAll(0);
            ShowCursor(1);
            break;
        case WM_EXITMENULOOP:
            g_inputMgr.SetAcquiredAll(1);
            ShowCursor(0);
            break;
    }
    return DefWindowProcA(hWnd, msg, wParam, lParam);
}

/* 0x401871 - everything that comes up once the D3D device exists: frustum (60 degree fov, near 8, far 20000), fog, the
 * screen, the input devices and their configuration, the game state, the intro videos, then the first scene. */
/* BYTES(view, inferred): g_animDt is the first field of the GameState block; the call reinterprets its address until that block is one object */
s32 App_InitGameSystems()
{
    g_maxImmediateTriangles = 1000;
    g_matUnk6d5428.SetScale(0.125f, 0.125f, 0.125f);
    g_pViewFrustum = new Frustrum(g_pD3DAppMain);
    g_pViewFrustum->SetProjection(8.0f, 20000.0f, 1.0471976f, 12000.0f);
    g_pViewFrustum->SetFog(1, 0, FOG_LINEAR, 2000.0f);
    ShowCursor(0);
    ShowWindow(g_hGameWindow, SW_SHOWNORMAL);
    SetFocus(g_hGameWindow);
    g_screen.Init(0);
    g_screen.Clear(0);
    g_pD3DAppMain->ShowFrame();
    g_inputMgr.CreateDevices(INPUTDEV_MASK_KEYBOARD | INPUTDEV_MASK_JOYSTICK | INPUTDEV_MASK_MOUSE);
    g_inputMgr.SelectDevice(g_launcherInputMode);
    g_inputMgr.LoadConfig(g_dirReference);
    g_pProgress->SetField80();
    Progress_ResetGlobal();
    /* cast kept: GameState is a view over the run of globals from g_animDt; one struct would move them (.bss order) */
    ((GameState *)&g_animDt)->Game_ResetState();
    g_pStreamPlayer = NULL;
    g_fmvListIntro.count = 3;
    strcat(g_fmvListIntro.clips[0], "Intro0.BVS");
    strcat(g_fmvListIntro.clips[1], "Intro1.BVS");
    strcat(g_fmvListIntro.clips[2], "Intro2.BVS");
    g_fmvListCredits.count = 1;
    strcpy(g_fmvListCredits.clips[0], "Credits.BVS");
    Video_PlaySequence(&g_fmvListIntro);
    g_pProgress->GotoScene(g_startLevelId);
    Draw_CreateScratchVertexBuffers();
    return 0;
}

/* 0x401ad5 - the exit path: unload the level, then delete the sound device, the frustum, the D3D app and the menu noise
 * texture. */
void App_Shutdown()
{
    Game_FreeLevel();
    delete g_pSoundSystem;
    delete g_pViewFrustum;
    delete g_pD3DAppMain;
    if (g_pMenuNoiseTexture) {
        delete g_pMenuNoiseTexture;
        g_pMenuNoiseTexture = NULL;
    }
}

/* ---- the launcher dialogs ---- */

/* 0x401ba9 */
/* BYTES(slot-group): locals grouped in one struct so they keep the original frame order */
s32 __stdcall Launcher_EasterEggDlgProc(HWND window, u32 message, u32 wParam, s32 lParam)
{
    struct Work {
        HBITMAP bitmap;
        HDC dc, compatible;
        BITMAP info;
    } w;
    switch (message) {
        case WM_LBUTTONDOWN:
            EndDialog(window, IDOK);
            break;
        case WM_ERASEBKGND:
            /* cast kept: WM_ERASEBKGND passes the DC in wParam */
            w.dc = (HDC)wParam;
            w.bitmap = LoadBitmapA(g_hInstance, MAKEINTRESOURCEA(IDB_EASTER_EGG));
            GetObjectA(w.bitmap, sizeof(BITMAP), &w.info);
            w.compatible = CreateCompatibleDC(w.dc);
            SelectObject(w.compatible, w.bitmap);
            BitBlt(w.dc, 0, 0, w.info.bmWidth, w.info.bmHeight, w.compatible, 0, 0, SRCCOPY);
            DeleteObject(w.bitmap);
            return 1;
        case WM_COMMAND:
            switch ((u16)wParam) {
                case IDCANCEL:
                    EndDialog(window, IDOK);
                    break;
            }
            break;
        default:
            return 0;
    }
    return 1;
}

/* 0x401c8c */
void Launcher_ShowMainDialog(HWND window, HINSTANCE instance)
{
    DialogBoxParamA(instance, MAKEINTRESOURCEA(IDD_LAUNCHER), window, Launcher_MainDlgProc, 0);
}

/* 0x401ca8 */
/* BYTES(slot-name, inferred): local names chosen for their stack slots (tools/vc6_locals.py), not recovered */
/* BYTES(dead-code): xValue / yResult are stored and never read, and the test re-extracts both words from lParam: the original did both */
/* BYTES(temp, inferred): the text group id goes through the u16 local groupItem because the original loads it from a stack slot */
s32 __stdcall Launcher_MainDlgProc(HWND window, u32 message, u32 wParam, s32 lParam)
{
    HFONT fontBuffer;
    char installPathLocal[512];
    const char *argsValue[2];
    char readmePathValue[512];
    HBITMAP backgroundItem;
    HDC dc;
    HDC compatibleResult;
    BITMAP bitmapSaved;
    s32 indexImage;
    HWND buttonValue;
    u16 groupItem;
    u32 yResult;
    u32 xValue;
    switch (message) {
        case WM_RBUTTONDBLCLK:
            xValue = (u16)lParam;
            yResult = (u16)(((u32)lParam >> 16) & 0xffff);
            if ((wParam & MK_CONTROL) && (wParam & MK_LBUTTON) && (u16)lParam >= 348 && (u16)lParam <= 350 &&
                (u16)(((u32)lParam >> 16) & 0xffff) >= 114 && (u16)(((u32)lParam >> 16) & 0xffff) <= 116)
                DialogBoxParamA(g_hInstance, MAKEINTRESOURCEA(IDD_EASTER_EGG), window, Launcher_EasterEggDlgProc, 0);
            break;
        case WM_INITDIALOG:
            groupItem = 1;
            SetWindowTextA(window, g_textCatalog.LoadString(groupItem, g_langMask, 2));
            indexImage = 0;
            groupItem = 2;
#define BUTTON(resource, stringId, color, control)                                                             \
    g_launcherButtonBitmaps[indexImage] = Launcher_MakeButtonBitmap(                                           \
        window, MAKEINTRESOURCEA(resource), g_textCatalog.LoadString(groupItem, g_langMask, stringId), color); \
    buttonValue = GetDlgItem(window, control);                                                                 \
    SendMessageA(buttonValue, BM_SETIMAGE, 0, (s32)g_launcherButtonBitmaps[indexImage]); /* cast kept: LPARAM */
            BUTTON(IDB_BTN_INSTALL_DX, 1, 0, IDC_INSTALL_DX);
            ++indexImage;
            BUTTON(IDB_BTN_PLAY, 2, 0xf6eeee, IDC_PLAY);
            ++indexImage;
            BUTTON(IDB_BTN_CONFIGURE, 4, 0, IDC_CONFIGURE);
            ++indexImage;
            BUTTON(IDB_BTN_CLEAR, 8, 0, IDC_CLEAR_SAVES);
            ++indexImage;
            BUTTON(IDB_BTN_QUIT, 0x10, 0xf6eeee, IDC_QUIT);
            ++indexImage;
            BUTTON(IDB_BTN_LINK, 0x20, 0, IDC_README);
            ++indexImage;
            groupItem = 4;
            BUTTON(IDB_BTN_LINK, 2, 0, IDC_GAME_SITE);
            ++indexImage;
            BUTTON(IDB_BTN_INFOGRAMES, 4, 0, IDC_INFOGRAMES_SITE);
#undef BUTTON
            return 0;
        case WM_ERASEBKGND:
            /* cast kept: WM_ERASEBKGND passes the DC in wParam */
            dc = (HDC)wParam;
            backgroundItem = LoadBitmapA(g_hInstance, MAKEINTRESOURCEA(IDB_LAUNCHER_BG));
            GetObjectA(backgroundItem, sizeof(BITMAP), &bitmapSaved);
            compatibleResult = CreateCompatibleDC(dc);
            SelectObject(compatibleResult, backgroundItem);
            BitBlt(dc, 0, 0, bitmapSaved.bmWidth, bitmapSaved.bmHeight, compatibleResult, 0, 0, SRCCOPY);
            DeleteObject(backgroundItem);
            return 1;
        case WM_COMMAND:
            switch ((u16)wParam) {
                case IDC_INSTALL_DX:
                    App_GetExeDir(installPathLocal);
                    strcat(installPathLocal, "\\Install\\dxsetup.exe");
                    argsValue[0] = installPathLocal;
                    argsValue[1] = 0;
                    _spawnv(_P_NOWAIT, installPathLocal, argsValue);
                    break;
                case IDC_PLAY:
                    Launcher_ShowOptionsDialog(window, g_hInstance, 0);
                    EndDialog(window, IDOK);
                    break;
                case IDC_CONFIGURE:
                    Launcher_ShowOptionsDialog(window, g_hInstance, 1);
                    break;
                case IDC_CLEAR_SAVES:
                    if (MessageBoxA(window, g_textCatalog.LoadString(8, g_langMask, 4),
                                    g_textCatalog.LoadString(1, g_langMask, 0x10), MB_YESNO) == IDYES) {
                        if (!Reg_DeleteAppKeys())
                            MessageBoxA(window, g_textCatalog.LoadString(8, g_langMask, 0x10),
                                        g_textCatalog.LoadString(1, g_langMask, 0x10), MB_ICONEXCLAMATION);
                        else
                            MessageBoxA(window, g_textCatalog.LoadString(8, g_langMask, 8),
                                        g_textCatalog.LoadString(1, g_langMask, 0x10), MB_ICONEXCLAMATION);
                    }
                    break;
                case IDC_GAME_SITE:
                    Launcher_OpenUrl("www.looneytunesgames.com", g_langMask);
                    break;
                case IDC_INFOGRAMES_SITE:
                    Launcher_OpenUrl("www.infogrames.com", g_langMask);
                    break;
                case IDC_README:
                    App_GetExeDir(readmePathValue);
                    strcat(readmePathValue, "./Readme.htm");
                    ShellExecuteA(g_hGameWindow, "open", readmePathValue, 0, 0, SW_SHOWNORMAL);
                    break;
                case IDC_QUIT:
                    exit(0);
            }
            break;
        case WM_SETFONT:
            /* cast kept: WM_SETFONT passes the font in wParam */
            fontBuffer = (HFONT)wParam;
            GetObjectA(fontBuffer, sizeof(LOGFONTA), &g_launcherLogFont);
            return 0;
        default:
            return 0;
    }
    return 1;
}

/* 0x402436 */
void Launcher_ShowOptionsDialog(HWND window, HINSTANCE instance, u8 show)
{
    DialogBoxParamA(instance, MAKEINTRESOURCEA(IDD_OPTIONS), window, Launcher_OptionsDlgProc, show);
}

/* 0x402455 */
/* BYTES(slot-group): locals grouped in one struct so they keep the original frame order */
void Launcher_FillModeCombo(HWND window, const char *selection)
{
    /* cast kept (the (s32) LPARAMs here): SendDlgItemMessageA passes the strings as its LPARAM */
    struct Work {
        char fallback[256], mode[256];
        u32 memory;
        u16 index, item;
    } w;
    w.item = 0;
    w.index = 0;
    SendDlgItemMessageA(window, IDC_CMB_MODE, CB_RESETCONTENT, 0, 0);
    w.item = (u16)SendDlgItemMessageA(window, IDC_CMB_DEVICE, CB_GETCURSEL, 0, 0);
    g_pD3DAppMain->SetDeviceIndex((u8)w.item);
    w.memory = g_pD3DAppMain->GetTotalVideoMem();
    do {
        g_pD3DAppMain->GetModeString(w.mode);
        if (g_pD3DAppMain->GetModeMemCost() + 0x300000 <= w.memory) {
            w.item = (u16)SendDlgItemMessageA(window, IDC_CMB_MODE, CB_ADDSTRING, 0, (s32)w.mode);
            SendDlgItemMessageA(window, IDC_CMB_MODE, CB_SETITEMDATA, w.item, w.index);
        }
        ++w.index;
    } while (g_pD3DAppMain->StepMode(1) != SEL_CLAMPED_AT_END);
    g_pD3DAppMain->SetModeIndex(0);
    if (selection) {
        if (SendDlgItemMessageA(window, IDC_CMB_MODE, CB_SELECTSTRING, -1, (s32)selection) == CB_ERR)
            SendDlgItemMessageA(window, IDC_CMB_MODE, CB_SETCURSEL, 0, 0);
    } else {
        strcpy(w.fallback, "800 x 600 x 16 bpp");
        SendDlgItemMessageA(window, IDC_CMB_MODE, CB_SELECTSTRING, -1, (s32)w.fallback);
    }
}

/* 0x4025c8 */
/* BYTES(slot-name): local names and declaration order chosen for their stack slots (tools/vc6_locals.py), not recovered */
/* BYTES(temp, inferred): group ids and the 0x50 size go through locals because the original loads them from stack slots */
s32 __stdcall Launcher_OptionsDlgProc(HWND window, u32 message, u32 wParam, s32 lParam)
{
    /* Local declarations are ordered to reproduce the original VC6 frame. */
    /* cast kept (the (s32) LPARAMs here): SendDlgItemMessageA passes strings as its LPARAM */
    char acceptTextData[256];
    char blankCurrent[2];
    s32 buttonIndexSelected;
    HWND buttonLocal;
    char joystickNameValue[256];
    char soundNameCopy[256];
    char displayNameCopy[256];
    u16 initGroupItem;
    HFONT incomingFontValue;
    HBITMAP backgroundBitmap;
    HDC backgroundDCItem;
    HDC backgroundCompatibleCurrent;
    BITMAP backgroundInfoSelected;
    u16 paintGroupBuffer;
    PAINTSTRUCT paintValue;
    HWND paintControlData;
    HDC paintDCRecord;
    char *paintTextItem;
    HFONT paintFont;
    RECT rectHandle;
    HKEY key;
    u16 modeIndex;
    s32 sizeValue;
    u16 indexValue;
    sizeValue = 0x50;
    switch (message) {
        case WM_PAINT:
            paintGroupBuffer = 2;
            paintDCRecord = BeginPaint(window, &paintValue);
            paintControlData = GetDlgItem(window, IDC_LBL_VIDEO_DRIVERS);
            paintFont = CreateFontIndirectA(&g_launcherLogFont);
            SelectObject(paintDCRecord, paintFont);
            SetBkMode(paintDCRecord, TRANSPARENT);
            SetTextColor(paintDCRecord, 0x202030);
#define PAINT_LABEL(textId)                                                                              \
    GetWindowRect(paintControlData, &rectHandle);                                                        \
    ScreenToClient(window, (POINT *)&rectHandle); /* cast kept: the RECT's top-left corner as a POINT */ \
    paintTextItem = g_textCatalog.LoadString(paintGroupBuffer, g_langMask, textId);                      \
    TextOutA(paintDCRecord, rectHandle.left, rectHandle.top, paintTextItem, strlen(paintTextItem));
            PAINT_LABEL(0x40);
            paintControlData = GetDlgItem(window, IDC_LBL_VIDEO_MODES);
            PAINT_LABEL(0x80);
            paintControlData = GetDlgItem(window, IDC_LBL_SOUND_DRIVERS);
            PAINT_LABEL(0x100);
            paintControlData = GetDlgItem(window, IDC_LBL_CONTROLLERS);
            PAINT_LABEL(0x200);
#undef PAINT_LABEL
            EndPaint(window, &paintValue);
            return 0;
        case WM_ERASEBKGND:
            /* cast kept: WM_ERASEBKGND passes the DC in wParam */
            backgroundDCItem = (HDC)wParam;
            backgroundBitmap = LoadBitmapA(g_hInstance, MAKEINTRESOURCEA(IDB_OPTIONS_BG));
            GetObjectA(backgroundBitmap, sizeof(BITMAP), &backgroundInfoSelected);
            backgroundCompatibleCurrent = CreateCompatibleDC(backgroundDCItem);
            SelectObject(backgroundCompatibleCurrent, backgroundBitmap);
            BitBlt(backgroundDCItem, 0, 0, backgroundInfoSelected.bmWidth, backgroundInfoSelected.bmHeight,
                   backgroundCompatibleCurrent, 0, 0, SRCCOPY);
            DeleteObject(backgroundBitmap);
            return 1;
        case WM_SETFONT:
            /* cast kept: WM_SETFONT passes the font in wParam */
            incomingFontValue = (HFONT)wParam;
            GetObjectA(incomingFontValue, sizeof(LOGFONTA), &g_launcherLogFont);
            return 0;
        case WM_INITDIALOG:
            if (Reg_CreateSubKey(&key, "Setup") == 0 &&
                Reg_ReadBinary(key, 0, &g_launcherSetup, sizeValue) == sizeValue) {
                Reg_CloseKey(&key);
                g_launcherSetupLoaded = 1;
            }
            SendDlgItemMessageA(window, IDC_DBG_WHEEL, BM_SETCHECK, BST_CHECKED, 0);
            SendDlgItemMessageA(window, IDC_DBG_LEVEL_EDIT, EM_SETREADONLY, 1, 0);
            SendDlgItemMessageA(window, IDC_DBG_LEVEL_EDIT, WM_ENABLE, 0, 0);
            initGroupItem = 1;
            SetWindowTextA(window, g_textCatalog.LoadString(initGroupItem, g_langMask, 4));
#define LABEL(control, textId)                          \
    SendDlgItemMessageA(window, control, WM_SETTEXT, 0, \
                        (s32)g_textCatalog.LoadString(initGroupItem, g_langMask, textId));
            LABEL(IDC_GRP_VIDEO, 0x20);
            LABEL(IDC_GRP_SOUND, 0x40);
            LABEL(IDC_GRP_CONTROL, 0x80);
            initGroupItem = 2;
            LABEL(IDC_LBL_VIDEO_DRIVERS, 0x40);
            LABEL(IDC_LBL_VIDEO_MODES, 0x80);
            LABEL(IDC_LBL_SOUND_DRIVERS, 0x100);
            LABEL(IDC_LBL_CONTROLLERS, 0x200);
            LABEL(IDC_CONTROLPANEL, 0x400);
            LABEL(IDC_ACCEPT, 0x800);
            LABEL(IDC_CANCEL, 0x1000);
#undef LABEL
            g_pD3DAppMain->SetDeviceIndex(0);
            do {
                g_pD3DAppMain->GetDeviceName(displayNameCopy);
                indexValue = (u16)SendDlgItemMessageA(window, IDC_CMB_DEVICE, CB_ADDSTRING, 0, (s32)displayNameCopy);
                SendDlgItemMessageA(window, IDC_CMB_DEVICE, CB_SETITEMDATA, indexValue, indexValue);
            } while (g_pD3DAppMain->StepDevice(1) != SEL_CLAMPED_AT_END);
            g_pD3DAppMain->SetDeviceIndex(0);
            g_pSoundSystem->SetDeviceIndex(0);
            do {
                g_pSoundSystem->GetDeviceName(soundNameCopy);
                indexValue = (u16)SendDlgItemMessageA(window, IDC_CMB_SOUND, CB_ADDSTRING, 0, (s32)soundNameCopy);
                SendDlgItemMessageA(window, IDC_CMB_SOUND, CB_SETITEMDATA, indexValue, indexValue);
            } while (g_pSoundSystem->StepDevice(1) != SEL_CLAMPED_AT_END);
            g_pSoundSystem->SetDeviceIndex(0);
            indexValue = (u16)SendDlgItemMessageA(window, IDC_CMB_CONTROLLER, CB_ADDSTRING, 0,
                                                  (s32)g_textCatalog.LoadString(0x10, g_langMask, 4));
            SendDlgItemMessageA(window, IDC_CMB_CONTROLLER, CB_SETITEMDATA, indexValue, indexValue);
            if ((u8)g_pD3DAppMain->HasJoystick() == 1) {
                g_pD3DAppMain->SetJoystickIndex(0);
                do {
                    g_pD3DAppMain->GetJoystickInstanceName(joystickNameValue);
                    indexValue =
                        (u16)SendDlgItemMessageA(window, IDC_CMB_CONTROLLER, CB_ADDSTRING, 0, (s32)joystickNameValue);
                    SendDlgItemMessageA(window, IDC_CMB_CONTROLLER, CB_SETITEMDATA, indexValue, indexValue);
                } while (g_pD3DAppMain->StepJoystick(1) != SEL_CLAMPED_AT_END);
                g_pD3DAppMain->SetJoystickIndex(0);
            }
            if (!g_launcherSetupLoaded) {
                SendDlgItemMessageA(window, IDC_CMB_DEVICE, CB_SETCURSEL, 0, 0);
                SendDlgItemMessageA(window, IDC_CMB_SOUND, CB_SETCURSEL, 0, 0);
                SendDlgItemMessageA(window, IDC_CMB_CONTROLLER, CB_SETCURSEL, 0, 0);
                g_launcherInputMode = INPUTMODE_KEYBOARD;
                Launcher_FillModeCombo(window, 0);
            } else {
                if (SendDlgItemMessageA(window, IDC_CMB_DEVICE, CB_SELECTSTRING, -1,
                                        (s32)g_launcherSetup.displayDevice) == CB_ERR)
                    SendDlgItemMessageA(window, IDC_CMB_DEVICE, CB_SETCURSEL, 0, 0);
                if (SendDlgItemMessageA(window, IDC_CMB_SOUND, CB_SELECTSTRING, -1, (s32)g_launcherSetup.soundDevice) ==
                    CB_ERR)
                    SendDlgItemMessageA(window, IDC_CMB_SOUND, CB_SETCURSEL, 0, 0);
                if (SendDlgItemMessageA(window, IDC_CMB_CONTROLLER, CB_SELECTSTRING, -1,
                                        (s32)g_launcherSetup.controller) == CB_ERR)
                    SendDlgItemMessageA(window, IDC_CMB_CONTROLLER, CB_SETCURSEL, 0, 0);
                Launcher_FillModeCombo(window, g_launcherSetup.displayMode);
                g_launcherSetupLoaded = 0;
            }
            indexValue = (u16)SendDlgItemMessageA(window, IDC_CMB_DEVICE, CB_GETCURSEL, 0, 0);
            g_pD3DAppMain->SetDeviceIndex((u8)indexValue);
            indexValue = (u16)SendDlgItemMessageA(window, IDC_CMB_MODE, CB_GETCURSEL, 0, 0);
            g_pD3DAppMain->SetModeIndex((u8)indexValue);
            indexValue = (u16)SendDlgItemMessageA(window, IDC_CMB_SOUND, CB_GETCURSEL, 0, 0);
            g_pSoundSystem->SetDeviceIndex((u8)indexValue);
            indexValue = (u16)SendDlgItemMessageA(window, IDC_CMB_CONTROLLER, CB_GETCURSEL, 0, 0);
            if (indexValue != 0 && indexValue != (u16)CB_ERR) {
                g_pD3DAppMain->SetJoystickIndex((u8)(indexValue - 1));
                g_launcherInputMode = INPUTMODE_JOYSTICK;
            } else
                g_launcherInputMode = INPUTMODE_KEYBOARD;
            if (lParam == 0)
                DestroyWindow(window);
            buttonIndexSelected = 0;
#define BUTTON(resource, textId, color, control)                                                                 \
    g_launcherButtonBitmaps[buttonIndexSelected] = Launcher_MakeButtonBitmap(                                    \
        window, MAKEINTRESOURCEA(resource), g_textCatalog.LoadString(initGroupItem, g_langMask, textId), color); \
    buttonLocal = GetDlgItem(window, control);                                                                   \
    SendMessageA(buttonLocal, BM_SETIMAGE, 0,                                                                    \
                 (s32)g_launcherButtonBitmaps[buttonIndexSelected]); /* cast kept: LPARAM */
            BUTTON(IDB_BTN_CONTROLPANEL, 0x400, 0, IDC_CONTROLPANEL);
            ++buttonIndexSelected;
            BUTTON(IDB_BTN_CANCEL, 0x1000, 0xf6eeee, IDC_CANCEL);
            ++buttonIndexSelected;
            BUTTON(IDB_BTN_ACCEPT, 0x800, 0xf6eeee, IDC_ACCEPT);
#undef BUTTON
            break;
        case WM_COMMAND:
            switch ((u16)wParam) {
                case IDC_DBG_WHEEL:
                case IDC_DBG_INTRO:
                case IDC_DBG_SCENE:
                case IDC_DBG_FAKE_ENDING:
                case IDC_DBG_REAL_ENDING:
                case IDC_DBG_DEMO1:
                case IDC_DBG_DEMO2:
                case IDC_DBG_LEVEL:
                    if (SendDlgItemMessageA(window, IDC_DBG_LEVEL, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                        SendDlgItemMessageA(window, IDC_DBG_LEVEL_EDIT, EM_SETREADONLY, 0, 0);
                        SendDlgItemMessageA(window, IDC_DBG_LEVEL_EDIT, WM_ENABLE, 1, 0);
                        SendDlgItemMessageA(window, IDC_DBG_LEVEL_EDIT, WM_KEYFIRST, 1, 0);
                    } else {
                        strcpy(blankCurrent, g_launcherEmptyText);
                        SendDlgItemMessageA(window, IDC_DBG_LEVEL_EDIT, WM_SETTEXT, 0, (s32)blankCurrent);
                        SendDlgItemMessageA(window, IDC_DBG_LEVEL_EDIT, EM_SETREADONLY, 1, 0);
                        SendDlgItemMessageA(window, IDC_DBG_LEVEL_EDIT, WM_ENABLE, 0, 0);
                    }
                    break;
                case IDC_CMB_DEVICE:
                    if ((u16)((wParam >> 16) & 0xffff) == CBN_SELCHANGE) {
                        if (g_launcherSetupLoaded == 1) {
                            Launcher_FillModeCombo(window, g_launcherSetup.displayMode);
                            g_launcherSetupLoaded = 0;
                        } else
                            Launcher_FillModeCombo(window, 0);
                    }
                    break;
                case IDC_CONTROLPANEL:
                    ShellExecuteA(g_hGameWindow, "open", "rundll32.exe", "shell32.dll,Control_RunDLL joy.cpl", 0,
                                  SW_SHOW);
                    break;
                case IDC_ACCEPT:
                    indexValue = (u16)SendDlgItemMessageA(window, IDC_CMB_DEVICE, CB_GETCURSEL, 0, 0);
                    SendDlgItemMessageA(window, IDC_CMB_DEVICE, CB_GETLBTEXT, indexValue, (s32)acceptTextData);
                    strncpy(g_launcherSetup.displayDevice, acceptTextData, 15);
                    g_launcherSetup.displayDevice[15] = 0;
                    g_pD3DAppMain->SetDeviceIndex((u8)indexValue);
                    indexValue = (u16)SendDlgItemMessageA(window, IDC_CMB_MODE, CB_GETCURSEL, 0, 0);
                    modeIndex = (u16)SendDlgItemMessageA(window, IDC_CMB_MODE, CB_GETITEMDATA, indexValue, 0);
                    SendDlgItemMessageA(window, IDC_CMB_MODE, CB_GETLBTEXT, indexValue, (s32)acceptTextData);
                    strncpy(g_launcherSetup.displayMode, acceptTextData, 15);
                    g_launcherSetup.displayMode[15] = 0;
                    g_launcherSetup.displayModeIndex = modeIndex;
                    g_pD3DAppMain->SetModeIndex((u8)modeIndex);
                    indexValue = (u16)SendDlgItemMessageA(window, IDC_CMB_SOUND, CB_GETCURSEL, 0, 0);
                    SendDlgItemMessageA(window, IDC_CMB_SOUND, CB_GETLBTEXT, indexValue, (s32)acceptTextData);
                    strncpy(g_launcherSetup.soundDevice, acceptTextData, 15);
                    g_launcherSetup.soundDevice[15] = 0;
                    g_pSoundSystem->SetDeviceIndex((u8)indexValue);
                    indexValue = (u16)SendDlgItemMessageA(window, IDC_CMB_CONTROLLER, CB_GETCURSEL, 0, 0);
                    SendDlgItemMessageA(window, IDC_CMB_CONTROLLER, CB_GETLBTEXT, indexValue, (s32)acceptTextData);
                    strncpy(g_launcherSetup.controller, acceptTextData, 15);
                    g_launcherSetup.controller[15] = 0;
                    if (indexValue != (u16)CB_ERR && indexValue != 0) {
                        g_pD3DAppMain->SetJoystickIndex((u8)indexValue);
                        g_launcherInputMode = INPUTMODE_JOYSTICK;
                    } else
                        g_launcherInputMode = INPUTMODE_KEYBOARD;
                    if (Reg_CreateSubKey(&key, "Setup") == 0 &&
                        Reg_WriteBinary(key, 0, &g_launcherSetup, sizeValue) == 1)
                        Reg_CloseKey(&key);
                    EndDialog(window, IDOK);
                    break;
                case IDC_CANCEL:
                    EndDialog(window, IDOK);
                    break;
                case IDCANCEL:
                    EndDialog(window, IDOK);
                    break;
            }
            break;
        default:
            return 0;
    }
    return 1;
}

/* 0x403592 - the verb is the plain literal "open" (a /Od string literal, .data 0x57938c), not a function-local
 * `static const char openVerb[]`, which VC6 puts in .rdata - and this object has no .rdata; in the exe the five bytes
 * sit among the .data literals. */
/* BYTES(layout): plain literal, not a static const array: the exe has these bytes among the .data literals, and VC6 puts static const arrays in .rdata */
void Launcher_OpenUrl(const char *url, u16 language)
{
    /* cast kept: ShellExecuteA returns an HINSTANCE that is an error code when 32 or less (the SDK's convention) */
    if ((s32)ShellExecuteA(g_hGameWindow, "open", url, 0, 0, SW_SHOWNORMAL) <= 32)
        MessageBoxA(g_hGameWindow, g_textCatalog.LoadString(8, language, 0x20),
                    g_textCatalog.LoadString(1, language, 0x10), MB_ICONEXCLAMATION);
}

/* 0x4035ef - the language bit of the system's default language (EN 1, FR 2, DE 4, NL 8, ES 0x10, IT 0x20, PT 0x40);
 * anything else is French. */
u16 App_GetLanguageMask()
{
    u16 mask;
    switch (GetUserDefaultLangID() & 0xff) {
        case LANG_FRENCH:
            mask = LANGMASK_FRENCH;
            break;
        case LANG_GERMAN:
            mask = LANGMASK_GERMAN;
            break;
        case LANG_DUTCH:
            mask = LANGMASK_DUTCH;
            break;
        case LANG_ITALIAN:
            mask = LANGMASK_ITALIAN;
            break;
        case LANG_SPANISH:
            mask = LANGMASK_SPANISH;
            break;
        case LANG_PORTUGUESE:
            mask = LANGMASK_PORTUGUESE;
            break;
        case LANG_ENGLISH:
            mask = LANGMASK_ENGLISH;
            break;
        default:
            mask = LANGMASK_FRENCH;
            break;
    }
    return mask;
}

/* 0x40369c - separate string literals: in this object the literal "\\" follows the launcher's "open" and lands 2-aligned
 * at 0x579392 as in the exe. One `static const char shortcutText[]` would go to .rdata instead; the code is the same
 * either way. */
/* 0x40369c - offers a desktop shortcut "<localised title>.lnk" to <exe dir>\SheepD3D.exe, every start while the .lnk does
 * not exist yet. Returns the last HRESULT (or the BOOL of SHGetPathFromIDListA). The working directory it gives the link
 * is a stack buffer that nothing fills, and the desktop pidl is never freed. */
/* BYTES(layout): plain literals, not static const arrays: the exe has these bytes among the .data literals */
HRESULT App_OfferDesktopShortcut()
{
    WCHAR wszLinkPath[260];
    IPersistFile *pFile;
    IShellLinkA *pShellLink;
    ITEMIDLIST *pidl;
    FILE *f;
    HRESULT res = E_FAIL;
    HRESULT hrFolder = E_FAIL;
    char shortcut[260];
    char workingDir[260];
    char exePath[260];

    hrFolder = SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOPDIRECTORY, &pidl);
    if (hrFolder < 0)
        return hrFolder;
    res = SHGetPathFromIDListA(pidl, shortcut);
    if (res >= 0 && res != 0) {
        App_GetExeDir(exePath);
        strcat(shortcut, "\\");
        strcat(shortcut, g_textCatalog.LoadString(1, g_langMask, 1));
        strcat(shortcut, ".lnk");
        strcat(exePath, "\\SheepD3D.exe");
        f = fopen(shortcut, "r");
        if (f == NULL) {
            if (MessageBoxA(g_hGameWindow, g_textCatalog.LoadString(8, g_langMask, 1),
                            g_textCatalog.LoadString(1, g_langMask, 0x10), MB_YESNO | MB_ICONQUESTION) == IDYES) {
                res = CoInitialize(NULL);
                if (res >= 0) {
                    /* cast kept (both): COM returns every interface through a void ** out parameter */
                    res = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLinkA,
                                           (void **)&pShellLink);
                    if (res >= 0) {
                        pShellLink->SetPath(exePath);
                        pShellLink->SetWorkingDirectory(workingDir);
                        /* cast kept: COM returns interfaces through a void ** */
                        res = pShellLink->QueryInterface(IID_IPersistFile, (void **)&pFile);
                        if (res >= 0) {
                            MultiByteToWideChar(CP_ACP, 0, shortcut, -1, wszLinkPath, 260);
                            res = pFile->Save(wszLinkPath, TRUE);
                            pFile->Release();
                        }
                        pShellLink->Release();
                    }
                    CoUninitialize();
                }
            }
        } else {
            fclose(f);
        }
    }
    return res;
}

/* 0x403921 - unreferenced developer helper: copies the back buffer to the clipboard as a bitmap (through a GDI DC of
 * the surface). */
void App_CopyBackBufferToClipboard()
{
    s32 w;
    s32 h;
    HBITMAP bmp;
    HDC surfDC;
    BITMAP bm;
    HDC hdcBitmap;

    if (OpenClipboard(g_hGameWindow) && EmptyClipboard()) {
        w = (s32)g_pViewFrustum->viewportWidth;
        h = (s32)g_pViewFrustum->viewportHeight;
        g_pD3DAppMain->GetBackBuffer()->GetDC(&surfDC);
        bmp = CreateCompatibleBitmap(surfDC, w, h);
        GetObjectA(bmp, sizeof(BITMAP), &bm);
        hdcBitmap = CreateCompatibleDC(NULL);
        SelectObject(hdcBitmap, bmp);
        StretchBlt(hdcBitmap, 0, 0, w, h, surfDC, 0, 0, w, h, SRCCOPY);
        if (!SetClipboardData(CF_BITMAP, bmp))
            MessageBoxA(g_hGameWindow, "erreur de format", "clipboard", MB_OK);
        DeleteDC(hdcBitmap);
        g_pD3DAppMain->GetBackBuffer()->ReleaseDC(surfDC);
    }
    CloseClipboard();
}

/* 0x403a59 - 1 when DirectX 8 is present. Otherwise asks to install it: yes runs <exe dir>\Install\dxsetup.exe
 * (_P_WAIT) and still returns 1; no shows a second message and returns 0. */
u8 App_EnsureDirectX()
{
    u8 result = 1;
    const char *args[2];
    char path[512];

    if (!DX_IsVersionOk()) {
        if (MessageBoxA(g_hGameWindow, g_textCatalog.LoadString(8, g_langMask, 0x800),
                        g_textCatalog.LoadString(1, g_langMask, 0x10), MB_YESNO) == IDYES) {
            App_GetExeDir(path);
            strcat(path, "\\Install\\dxsetup.exe");
            args[0] = path;
            args[1] = NULL;
            _spawnv(_P_WAIT, path, args);
        } else {
            MessageBoxA(g_hGameWindow, g_textCatalog.LoadString(8, g_langMask, 0x1000),
                        g_textCatalog.LoadString(1, g_langMask, 0x10), MB_ICONHAND);
            result = 0;
        }
    }
    return result;
}

/* 0x403b4e */
/* BYTES(slot-group): locals grouped in one struct so they keep the original frame layout; 'unused' is a filler slot */
BITMAPINFO *Gdi_CreateBitmapInfoStruct(HWND window, HBITMAP bitmap)
{
    struct Work {
        BITMAP bitmap;
        u16 unused, depth;
        BITMAPINFO *info;
    } w;
    if (!GetObjectA(bitmap, sizeof(BITMAP), &w.bitmap))
        return 0;
    w.depth = w.bitmap.bmPlanes * w.bitmap.bmBitsPixel;
    if (w.depth == 1)
        w.depth = 1;
    else if (w.depth <= 4)
        w.depth = 4;
    else if (w.depth <= 8)
        w.depth = 8;
    else if (w.depth <= 16)
        w.depth = 16;
    else if (w.depth <= 24)
        w.depth = 24;
    else
        w.depth = 32;
    /* cast kept (both): LocalAlloc returns the block as void * */
    if (w.depth != 24)
        w.info = (BITMAPINFO *)LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER) + sizeof(u32) * (1 << w.depth));
    else
        /* cast kept: the Windows API takes and returns untyped memory here */
        w.info = (BITMAPINFO *)LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER));
    w.info->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    w.info->bmiHeader.biWidth = w.bitmap.bmWidth;
    w.info->bmiHeader.biHeight = w.bitmap.bmHeight;
    w.info->bmiHeader.biPlanes = w.bitmap.bmPlanes;
    w.info->bmiHeader.biBitCount = w.bitmap.bmBitsPixel;
    if (w.depth < 24)
        w.info->bmiHeader.biClrUsed = 1 << w.depth;
    w.info->bmiHeader.biCompression = BI_RGB;
    w.info->bmiHeader.biSizeImage = ((w.info->bmiHeader.biWidth * w.depth + 31) & ~31) / 8 * w.info->bmiHeader.biHeight;
    w.info->bmiHeader.biClrImportant = 0;
    return w.info;
}

/* 0x403c9e */
/* BYTES(slot-name, inferred): local names chosen for their stack slots (tools/vc6_locals.py), not recovered */
/* BYTES(dead-code): rows is written and never read, and resultData is zeroed just before it is assigned: both stores are in the original */
HBITMAP Launcher_MakeButtonBitmap(HWND window, const char *resource, const char *text, u32 color)
{
    HBITMAP sourceBitmapValue;
    TEXTMETRICA metricRecord;
    s32 heightRecord;
    HDC dcSource;
    DIBSECTION dibValue;
    s32 bytesRecord;
    HFONT fontSaved;
    HBITMAP resultData;
    s32 weight;
    u8 storageLocal[0x430];
    u8 *pixelsValue;
    BITMAPINFO *infoValue;
    BITMAP bitmapLocal;
    HDC compatibleValue;
    s32 rows;
    dcSource = GetDC(window);
    /* cast kept: LoadImageA returns a generic image HANDLE */
    sourceBitmapValue = (HBITMAP)LoadImageA(g_hInstance, resource, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
    GetObjectA(sourceBitmapValue, sizeof(BITMAP), &bitmapLocal);
    compatibleValue = CreateCompatibleDC(dcSource);
    SelectObject(compatibleValue, sourceBitmapValue);
    BitBlt(compatibleValue, 0, 0, bitmapLocal.bmWidth, bitmapLocal.bmHeight, compatibleValue, 0, 0, SRCCOPY);
    weight = g_launcherLogFont.lfWeight;
    g_launcherLogFont.lfWeight = FW_BOLD;
    fontSaved = CreateFontIndirectA(&g_launcherLogFont);
    SelectObject(compatibleValue, fontSaved);
    SetBkMode(compatibleValue, TRANSPARENT);
    SetTextColor(compatibleValue, color);
    SetTextAlign(compatibleValue, TA_CENTER);
    GetTextMetricsA(compatibleValue, &metricRecord);
    TextOutA(compatibleValue, bitmapLocal.bmWidth / 2, bitmapLocal.bmHeight / 2 - metricRecord.tmHeight / 2, text,
             strlen(text));
    g_launcherLogFont.lfWeight = weight;
    GetObjectA(sourceBitmapValue, sizeof(DIBSECTION), &dibValue);
    infoValue = (BITMAPINFO *)storageLocal; /* cast kept: a BITMAPINFO with its 256-colour table in a byte buffer */
    memset(infoValue, 0, 0x42c);
    infoValue->bmiHeader = dibValue.dsBmih;
    bytesRecord = (infoValue->bmiHeader.biWidth * infoValue->bmiHeader.biBitCount + 31) & ~31;
    bytesRecord >>= 3;
    bytesRecord *= infoValue->bmiHeader.biHeight;
    pixelsValue = new u8[bytesRecord];
    heightRecord = infoValue->bmiHeader.biHeight;
    rows = GetDIBits(dcSource, sourceBitmapValue, 0, heightRecord, pixelsValue, infoValue, DIB_RGB_COLORS);
    resultData = 0;
    resultData = CreateDIBitmap(dcSource, &infoValue->bmiHeader, CBM_INIT, pixelsValue, infoValue, DIB_RGB_COLORS);
    if (pixelsValue)
        delete[] pixelsValue;
    DeleteObject(sourceBitmapValue);
    DeleteDC(compatibleValue);
    return resultData;
}
