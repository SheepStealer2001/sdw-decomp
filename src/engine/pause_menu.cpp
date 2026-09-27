/*
 * T279, guessed original file PauseMenu.cpp.
 *   .text 0x5428c0-0x546679   .data 0x57c128-0x57c3c0   .bss 0x6de9a0-0x6defa8
 * match-addr: g_pauseBoxRect=0x6dec48 g_progressPauseCopy=0x6dec58 g_pauseViewDistance=0x6dec82
 * match-addr: g_pauseResumeIndex=0x6dec84 g_menuFooterStyle=0x6dec86 g_pauseRebinding=0x6dec88
 * match-addr: g_pauseFlag89=0x6dec89 g_pauseSlidersShown=0x6dec8a g_pauseFlag8b=0x6dec8b
 * match-addr: g_pauseExitRequested=0x6dec8c g_pauseRestartRequested=0x6dec8d g_pauseFlag8e=0x6dec8e
 * match-addr: g_pauseFlag8f=0x6dec8f g_pauseCfg=0x6de9b4 g_quitCfg=0x6dede8 g_rebindCooldown=0x6dede4
 * match-addr: g_onOffNames=0x6de9a0 g_soundModeNames=0x6de9a8 g_settingNames=0x6def8c g_uiString1B=0x6deb50
 * match-addr: g_menuCursors=0x6deb58 g_confirmPending=0x6def9c g_quitPending=0x6defa0 g_uiLangMask=0x6dedf0
 * match-addr: g_pauseTextRect=0x6dec50 g_menuFooterStylePrev=0x6dec87 g_msgBoxText=0x6dec90 g_msgBoxRect=0x6ded10
 * match-addr: g_msgBoxAlign=0x6ded18 g_msgBoxFrame=0x6ded20
 * The pause menu 0x5428c0-0x54579c, then the menu tail PauseMenu_Update 0x54579d .. PauseMenu_OnOpen_stub 0x546674
 * (the approach helpers and path follower after it are T280 src/engine/approach.cpp, the Scratch32k_* functions T281
 * src/engine/list.cpp). The menu module's data is defined here in address order: .data g_pauseMenu / g_pausedMenu
 * (with the choice and binding flags between them), then the literals; .bss every global of the menu module,
 * including the two menus' node arrays g_pausedMenuItems 0x6de9c0 and g_pauseMenuItems 0x6dedf8 (named in
 * data/symbols_modules.csv, referred to only by the two Menu initialisers).
 * .bss order (a representation device): VC6 emits a file's
 * uninitialised globals first, sorted by a hash of their names, then the ones explicitly initialised to zero in
 * definition order; so every .bss global here is written `= 0` / `= {0}` and defined in address order. Arrays of 64 bytes
 * or more are 8-aligned, smaller arrays 4-aligned, scalars at their natural alignment (g_pauseViewDistance 0x6dec82),
 * which reproduces every gap. Two details that follow from the layout:
 *   - g_pauseViewDistance 0x6dec82 is byte +0x2a of the 0x2c-byte snapshot g_progressPauseCopy that
 *     Progress::CopyRecord 0x50be91 fills on open, and that byte is the record's viewDistanceSetting (Progress +0xae):
 *     the snapshot is what saves the view distance the display page restores. In the original it was most likely a
 *     member of the snapshot; here g_progressPauseCopy is defined 0x2a bytes long so that the separately named
 *     g_pauseViewDistance lands on it (byte +0x2b is padding before g_pauseResumeIndex). Same bytes, same code.
 *   - 0x6def98-0x6def9c, between g_settingNames and g_confirmPending, is four bytes no instruction refers to (defined
 *     here as g_pauseBss_6def98, a name by address). The four bytes after g_quitPending are alignment padding before T281's
 *     8-aligned .bss.
 *
 * The in-game pause menu, SheepD3D.exe 0x5428c0-0x54579c: the level UI strings and cursors (Menus_LoadLevelUi), the
 * pause opener / exit, the menu tree (PauseMenu_Build) and every item handler registered on it: resume / restart /
 * exit / quit with their yes-no boxes, the sound page (speaker mode, sounds, three volume sliders, subtitles), the
 * display page (fog = view distance), the controller page and its "Edit config." key-binding page, and the "press a
 * key" rebinding routine. The handlers are flat void __cdecl(u8 msg, MenuPage *self) (MenuMsg) with no vtable.
 *
 * InputBindingIdx (data/structs) is the {u8 devIdx; u16 code} binding the input manager fills. The nine key-binding
 * rows and the four arrow rows are byte-identical apart from their constants, so they are written as two macros.
 * The names of the file's statics are descriptive (the binary has no symbols); the .bss ones are placed by the
 * match-addr lines. Local names were picked for the /Od frame order (tools/vc6_locals.py): plausible, not recovered.
 */
/* BYTES: cast, dead-code, flow, inline, layout, slot-group, view. */
/* BYTES(layout): written '= 0' only to keep definition order in .bss */
/* BYTES(layout): defined 0x2a bytes long so g_pauseViewDistance lands on its byte +0x2a (the original probably had one 0x2c-byte record) */
/* BYTES(layout): placeholder: four unreferenced bytes kept only for the .bss layout */
/* BYTES(inline): Pad::SetActuator (member-macro inline): source-only inline: its expansion gives the original's shape */
/* BYTES(cast, inferred): g_textCursorY macro: read as u16 through a cast: the original loads it unsigned here, although the field is s16 */
/* BYTES(view, inferred): GAME_STATE / OPTION_BITS / RUNTIME_BITS macros: views: g_animDt is the start of the GameState block; optionBits / runtimeFlags are read as bitfields */
/* BYTES(flow): NOISE_TEX_FIX_DESC macro: a macro, not a helper: the original re-reads the global at every test (0x545999, 0x545a16, 0x545b2b) */
/* BYTES(flow): the eight key-binding rows and the four arrow rows (two macros): the original has one copy of this handler per row; they differ only in constants */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
#include "../sdk/dinput.h"
#include "../sdk/win32.h"
#include "../sdk/ddraw.h"
#include "../sdk/crt.h"
#define SDW_MEMBERS_Progress inline void ToggleAutoSave();

#define SDW_MEMBERS_Texture                                                              \
    Texture(D3DApp *app, u32 width, u32 height, u32 format, s32 *result); /* 0x40a140 */ \
    long Surface_LockForWrite(DDSURFACEDESC2 *desc);                      /* 0x40ae3f */
#include "sdw_classes.h"
#define SDW_INLINE_PAD_SETACTUATOR_INT 1
#include "input_inlines.h"
#undef SDW_INLINE_PAD_SETACTUATOR_INT
#include "sdw_empty_call.h"
#include "sdw_global_views.h"

typedef void (*MenuHandler)(u8 msg, MenuPage *self);

/* ---- globals of other objects (named in the tables) ---- */
#include "progress.h"
#include "game_state.h"
#include "scenaric.h"
#include "stream_player.h"
#include "draw2d.h"
#include "text.h"
#include "interface.h"
#include "input.h"
#include "screen.h"
#include "../objects/menu.h"
#include "prompt.h"
#include "sound_mgr.h"
#include "sfx_volume.h"
#include "fade.h"
#include "approach.h"
extern u32 g_gameFlags;        /* 0x6ddf74 */
extern u32 *g_screenLayerBase; /* 0x585044  2D draw layers: layer n is g_screenLayerBase + n */

extern TextPort g_textPort;
/* cast kept: a view macro: this file reads the s16 TextPort.cursorY as a u16 */
#define g_textCursorY (*(u16 *)&g_textPort.cursorY) /* 0x6ddb02 */

extern TextPort g_textPort;
#define g_textWinY (g_textPort.winY) /* 0x6ddafa */
extern const float g_viewDistFar;    /* 0x5770c4  12000.0, the view distance at setting 0 */
extern const float g_viewDistNear;   /* 0x5770c8  4000.0, the view distance at setting 255 */

extern TextPort g_textPort;
#define g_textCursorX (g_textPort.cursorX) /* 0x6ddb00 */

extern TextPort g_textPort;
#define g_textClipOffX (g_textPort.clipOffX) /* 0x6ddb04 */
extern s32 g_dtRawMs;                        /* 0x71b2d8 */
extern s32 g_dt;                             /* 0x71b300 */
extern s32 g_dtMs;                           /* 0x71b2e8 */

/* ==== this object's .bss 0x6de9a0-0x6defa8 (definition order; see the header) ==== */
char *g_onOffNames[2] = {0};          /* 0x6de9a0  UI strings 0x10, 0xf */
char *g_soundModeNames[3] = {0};      /* 0x6de9a8  BSM group 0x10, 0x1000 / 0x2000 / 0x4000 */
u8 g_pauseCfg[7] = {0};               /* 0x6de9b4  "CfgGame" registry image written by PauseMenu_Exit */
MenuPage g_pausedMenuItems[50] = {0}; /* 0x6de9c0  node storage of g_pausedMenu */
char *g_uiString1B = 0;               /* 0x6deb50 */
UiCursor g_menuCursors[4] = {0};      /* 0x6deb58  slider cursors: 0..2 the volumes, 3 the fog */
s16 g_pauseBoxRect[4] = {0};          /* 0x6dec48  {x, y, w, h} of the pause box on the 512 x 240 screen */
s16 g_pauseTextRect[4] = {0};         /* 0x6dec50  the title / footer text window PauseMenu_Update sets */
u8 g_progressPauseCopy[0x2a] = {
    0}; /* 0x6dec58  the saved progress record, snapshotted on open; 0x2c bytes with the next two (see the header) */
u8 g_pauseViewDistance = 0;          /* 0x6dec82  view distance to restore when the display page is left */
s16 g_pauseResumeIndex = 0;          /* 0x6dec84  node index of "resume", where the cursor returns */
u8 g_menuFooterStyle = 0;            /* 0x6dec86  which footer the menu draws */
u8 g_menuFooterStylePrev = 0;        /* 0x6dec87  last frame's g_menuFooterStyle; nothing reads it */
u8 g_pauseRebinding = 0;             /* 0x6dec88  1 while a key-binding item captures input */
u8 g_pauseFlag89 = 0;                /* 0x6dec89 */
u8 g_pauseSlidersShown = 0;          /* 0x6dec8a  sliders are drawn on unselected rows too */
u8 g_pauseFlag8b = 0;                /* 0x6dec8b */
u8 g_pauseExitRequested = 0;         /* 0x6dec8c  "exit" confirmed: leave the level when the menu closes */
u8 g_pauseRestartRequested = 0;      /* 0x6dec8d  "restart level" confirmed */
u8 g_pauseFlag8e = 0;                /* 0x6dec8e */
u8 g_pauseFlag8f = 0;                /* 0x6dec8f */
char g_msgBoxText[0x80] = {0};       /* 0x6dec90  the message box's text (strcpy, unbounded) */
s16 g_msgBoxRect[4] = {0};           /* 0x6ded10  the message box's {x, y, w, h} */
u8 g_msgBoxAlign = 0;                /* 0x6ded18 */
UiFrame g_msgBoxFrame = {0};         /* 0x6ded20  the message box's four frame strips */
s32 g_rebindCooldown = 0;            /* 0x6dede4  Menu_RebindControl waits while > 0; the message box's ms */
u8 g_quitCfg[7] = {0};               /* 0x6dede8  the same image written by PauseMenu_QuitConfirm */
u16 g_uiLangMask = 0;                /* 0x6dedf0  language mask of the BSM strings */
MenuPage g_pauseMenuItems[50] = {0}; /* 0x6dedf8  node storage of g_pauseMenu */
Texture *g_pMenuNoiseTexture = 0;    /* 0x6def88 */
char *g_settingNames[3] = {0};       /* 0x6def8c  UI strings 0xc, 0xd, 0xe */
u32 g_pauseBss_6def98 = 0;           /* 0x6def98  unreferenced (see the header); descriptive name */
s32 g_confirmPending = 0;            /* 0x6def9c */
s32 g_quitPending = 0;               /* 0x6defa0 */

/* ==== this object's .data 0x57c128-0x57c168, then the literals ==== */
Menu g_pauseMenu = {g_pauseMenuItems, 0, 50, 2, 0, 0, 0, 0}; /* 0x57c128 */
s32 g_confirmChoice = 1;                                     /* 0x57c13c  yes/no box of restart / exit: 1 = no */
s32 g_quitChoice = 1;                                        /* 0x57c140  yes/no box of quit */
u8 g_bindUpShown = 1;       /* 0x57c144  the binding items' "show the current binding" flags */
u8 g_bindRightShown = 1;    /* 0x57c145 */
u8 g_bindDownShown = 1;     /* 0x57c146 */
u8 g_bindLeftShown = 1;     /* 0x57c147 */
u8 g_bindActionShown = 1;   /* 0x57c148 */
u8 g_bindJumpShown = 1;     /* 0x57c149 */
u8 g_bindWolfEyeShown = 1;  /* 0x57c14a */
u8 g_bindRunShown = 1;      /* 0x57c14b */
u8 g_bindQuickInvShown = 1; /* 0x57c14c */
u8 g_bindSneakShown = 1;    /* 0x57c14d */
u8 g_bindViewMapShown = 1;  /* 0x57c14e */
u8 g_bindCamLeftShown = 1;  /* 0x57c14f */
u8 g_bindCamRightShown = 1; /* 0x57c150 */
Menu g_pausedMenu = {g_pausedMenuItems, 0, 50, 2, 0, 0, 0, 0}; /* 0x57c154 */

/* ---- functions ---- */
void Menu_Printf(MenuPage *item, u8 align, const char *fmt, ...);             /* 0x5464e3 */
void Menu_PrintfSelected(int blink, u8 align, const char *fmt, ...);          /* 0x5464bd */
s16 Menu_AddItems(MenuPage *page, MenuHandler handler, ...);                  /* 0x54ea70 */
void Menu_DrawBindingText(u8 actionId, MenuPage *item);                       /* 0x545c11 */
void Menu_BuildValidateCancelFooter();                                        /* 0x545d9f */
void Menu_BuildValidateBackFooter();                                          /* 0x545e74 */
void Menu_BuildQuitFooter();                                                  /* 0x545f49 */
void Menu_BuildPauseFooter();                                                 /* 0x545f53 */
void Menu_BuildNavFooter();                                                   /* 0x545fde */
void Menu_BuildAutoSaveFooter();                                              /* 0x546079 */
void Menu_BuildControlsFooter();                                              /* 0x546083 */
void Menu_ShowMessageBox(const char *text, u8 align, s32 durationMs);         /* 0x546158 */
long Menu_CreateNoiseTexture();                                               /* 0x545929 */
void PauseMenu_OnOpen_stub();                                                 /* 0x546674 */
LONG Reg_CreateSubKey(HKEY *out, const char *name);                           /* 0x55f8dc */
void Reg_CloseKey(HKEY *key);                                                 /* 0x55f93a */
u8 Reg_WriteBinary(HKEY key, const char *name, const void *data, DWORD size); /* 0x55fbb2 */

void PauseMenu_ItemAutoSave(u8 msg, MenuPage *self);
void PauseMenu_PageOptions(u8 msg, MenuPage *self);
void PauseMenu_PageSoundOptions(u8 msg, MenuPage *self);
void PauseMenu_ItemResume(u8 msg, MenuPage *self);
void PauseMenu_ItemExit(u8 msg, MenuPage *self);
void PauseMenu_ItemRestartLevel(u8 msg, MenuPage *self);
void PauseMenu_ItemSpeakerMode(u8 msg, MenuPage *self);
void PauseMenu_ItemSfxVolume(u8 msg, MenuPage *item);
void PauseMenu_ItemVoiceVolume(u8 msg, MenuPage *item);
void PauseMenu_ItemMusicVolume(u8 msg, MenuPage *item);
void PauseMenu_ItemSubTitles(u8 msg, MenuPage *item);
void PauseMenu_ItemSoundDone(u8 msg, MenuPage *item);
void PauseMenu_PageControllerSetting(u8 msg, MenuPage *item);
void PauseMenu_PageDisplaySettings(u8 msg, MenuPage *item);
void PauseMenu_PageEditConfig(u8 msg, MenuPage *item);
void PauseMenu_ItemQuit(u8 msg, MenuPage *item);
void PauseMenu_QuitConfirm(u8 msg, MenuPage *item);
void PauseMenu_ItemSoundsEnabled(u8 msg, MenuPage *item);
void PauseMenu_ItemFog(u8 msg, MenuPage *item);
void PauseMenu_ItemDisplayDone(u8 msg, MenuPage *item);
void PauseMenu_ItemControlDevice(u8 msg, MenuPage *item);
void PauseMenu_ItemBindUp(u8 msg, MenuPage *item);
void PauseMenu_ItemBindRight(u8 msg, MenuPage *item);
void PauseMenu_ItemBindDown(u8 msg, MenuPage *item);
void PauseMenu_ItemBindLeft(u8 msg, MenuPage *item);
void PauseMenu_ItemBindAction(u8 msg, MenuPage *item);
void PauseMenu_ItemBindJump(u8 msg, MenuPage *item);
void PauseMenu_ItemBindWolfEyeView(u8 msg, MenuPage *item);
void PauseMenu_ItemBindRun(u8 msg, MenuPage *item);
void PauseMenu_ItemBindQuickInventory(u8 msg, MenuPage *item);
void PauseMenu_ItemBindWalkStealthily(u8 msg, MenuPage *item);
void PauseMenu_ItemBindViewMap(u8 msg, MenuPage *item);
void PauseMenu_ItemBindCamLeft(u8 msg, MenuPage *item);
void PauseMenu_ItemBindCamRight(u8 msg, MenuPage *item);
void PauseMenu_ItemSaveConfig(u8 msg, MenuPage *item);
void Menu_RebindControl(u8 actionId);

/* ---- the menu tail's declarations not already above ---- */
void PauseMenu_Exit();                                                      /* 0x542bcd */
void Text_PrintFmtStyled(u8 style, s32 blink, const char *fmt, va_list ap); /* 0x5348d8 */
u16 Text_CountWrappedLines(const char *s);                                  /* 0x53353f */
void Draw2D_TexRect(float z, float x0, float y0, float x1, float y1, s32 texIndex, float uTL, float vTL, u32 cTL,
                    float uBL, float vBL, u32 cBL, float uTR, float vTR, u32 cTR, float uBR, float vBR,
                    u32 cBR); /* 0x5242df */
void Draw2D_TexRect_Immediate(float z, float x0, float y0, float x1, float y1, u32 renderFlags, Texture *texture,
                              float uTL, float vTL, u32 cTL, float uBL, float vBL, u32 cBL, float uTR, float vTR,
                              u32 cTR, float uBR, float vBR, u32 cBR); /* 0x52607b */

void Menu_Printf(MenuPage *item, u8 align, const char *fmt, ...);
void Menu_DrawMessageBox();
void Menu_BuildValidateBackFooter();
void Menu_BuildBackFooter();
void Menu_BuildNavFooter();
void PauseMenu_OnOpen_stub();
void PauseMenu_DrawNoiseOverlay();
void PausedMenu_ItemPaused(u8 msg, MenuPage *item);

/* cast kept: GameState is a view over the run of globals from g_animDt; one struct global would move them in .bss */
#define GAME_STATE ((GameState *)&g_animDt)
/* cast kept: the saved record is raw bytes; its option byte is at +0x14 */
#define RECORD_OPTIONS(rec) ((ProgressOptionBits *)((rec) + 0x14))

/* source-only inline: clears g_gameFlags bits (as in src/engine/fade.cpp). Inlined with a constant argument, /Od
 * keeps the `~` as code (mov eax,0x4001; not eax at 0x542b4b). */
/* BYTES(inline): source-only inline: the ~ stays code (mov eax,0x4001; not eax at 0x542b4b) */
#define SDW_INLINE_FREE_GAME_CLEARFLAGS_U32 1
#include "game_state_inlines.h"
#undef SDW_INLINE_FREE_GAME_CLEARFLAGS_U32

/* source-only inline: the pause box, centred on the 512 x 240 virtual screen. The constants reach the code unfolded
 * (mov eax,0x200; sub eax,0x1c0 at 0x5428d4). */
/* BYTES(inline): source-only inline: its constants reach the code unfolded (mov eax,0x200; sub eax,0x1c0 at 0x5428d4) */
inline void PauseMenu_SetBox(s32 w, s32 h)
{
    g_pauseBoxRect[0] = (0x200 - w) / 2;
    g_pauseBoxRect[1] = (0xf0 - h) / 2;
    g_pauseBoxRect[2] = w;
    g_pauseBoxRect[3] = h;
}

/* source-only inlines: the saved record (+0x84) and the live option byte (+0x98), each a pointer temp at every use
 * (0x54334c). */
/* BYTES(inline): source-only inline: a pointer temp at every use (0x54334c) */
inline u8 *Progress_Record()
{
    return g_pProgress->timeKeeperBits; /* +0x84, the saved record; its option byte is at +0x14 */
}
inline ProgressOptionBits *Progress_Options()
{
    return &g_pProgress->optionBits;
}

/* 0x5428c0 - per-level UI set-up, called once by Load_DAVnWAR: the pause box, the four slider cursors' skins, the UI
 * strings the option rows print, and the BSM language mask for the player's language. */
void Menus_LoadLevelUi()
{
    u16 i;
    u8 playerLang;

    g_pauseFlag89 = 1;
    g_pauseSlidersShown = 0;
    PauseMenu_SetBox(0x1c0, 0xa0);
    g_pauseBoxRect[3] -= g_pCurFont->lineHeight / 2;
    for (i = 0; i < 4; i++)
        g_menuCursors[i].UiFrame_LoadSkin(0, 0);
    g_settingNames[0] = Text_GetUiString(UISTR_MONO);
    g_settingNames[1] = Text_GetUiString(UISTR_STEREO);
    g_settingNames[2] = Text_GetUiString(UISTR_REVERSE_STEREO);
    g_onOffNames[0] = Text_GetUiString(UISTR_NO);
    g_onOffNames[1] = Text_GetUiString(UISTR_YES);
    g_uiString1B = Text_GetUiString(UISTR_CUSTOMIZE);
    Menu_CreateNoiseTexture();
    playerLang = g_pProgress->language;
    switch (playerLang) {
        case GAME_LANG_FRENCH:
            g_uiLangMask = LANGMASK_FRENCH;
            break;
        case GAME_LANG_ENGLISH:
            g_uiLangMask = LANGMASK_ENGLISH;
            break;
        case GAME_LANG_SPANISH:
            g_uiLangMask = LANGMASK_SPANISH;
            break;
        case GAME_LANG_ITALIAN:
            g_uiLangMask = LANGMASK_ITALIAN;
            break;
        case GAME_LANG_GERMAN:
            g_uiLangMask = LANGMASK_GERMAN;
            break;
        case GAME_LANG_DUTCH:
            g_uiLangMask = LANGMASK_DUTCH;
            break;
        case GAME_LANG_BRAZILIAN:
            g_uiLangMask = LANGMASK_PORTUGUESE;
            break;
        default:
            g_uiLangMask = LANGMASK_ENGLISH;
            break;
    }
    g_soundModeNames[0] = g_textCatalog.LoadString(0x10, g_uiLangMask, 0x1000);
    g_soundModeNames[1] = g_textCatalog.LoadString(0x10, g_uiLangMask, 0x2000);
    g_soundModeNames[2] = g_textCatalog.LoadString(0x10, g_uiLangMask, 0x4000);
}

/* 0x542ac0 - back to the root page with the cursor on "resume". */
void PauseMenu_ReturnToRoot()
{
    Menu_SetCapture(MENU_CAPTURE_NONE, 0);
    Menu_ResetToRoot(&g_pauseMenu, g_pauseResumeIndex);
    g_pauseSlidersShown = 1;
    g_pauseFlag8f = 1;
    g_pauseFlag8e = 0;
}

/* 0x542afa - opens the pause menu: snapshots the saved record, pauses the sounds and the game (g_gameFlags 0x40, clears
 * 0x4001), makes the pause menu current at its root. */
void PauseMenu_Open()
{
    g_pProgress->CopyRecord(g_progressPauseCopy);
    Sound_PauseAll();
    g_pad.SetActuator(0);
    g_pProgress->controls.actuatorEnable = 0;
    g_gameFlags |= GF_PAUSED;
    Menu_SetCurrent(&g_pauseMenu);
    Game_ClearFlags(GF_BIT0 | GF_UPDATE_OBJECTS);
    Menu_ResetToRoot(&g_pauseMenu, -1);
    PauseMenu_OnOpen_stub();
    g_menuFooterStyle = MENU_FOOTER_NONE;
    g_pauseFlag89 = 1;
    g_pauseSlidersShown = 1;
    g_pauseFlag8f = 1;
    g_pauseRebinding = 0;
    g_pauseFlag8b = 0;
    g_pauseExitRequested = 0;
    g_pauseRestartRequested = 0;
    g_pauseFlag8e = 0;
    g_inputMgr.SetActiveFlags(0);
    g_inputMgr.ResetEdges();
}

/* 0x542bcd - closes the pause menu: restarts the level music, resumes the sounds and the game (clears 0x60, sets
 * 0xc000 unconditionally), starts the level exit / restart fade that was confirmed, restores the saved record and
 * writes the options to the registry key "CfgGame". */
void PauseMenu_Exit()
{
    HKEY key;
    DWORD len;

    Menu_Close();
    g_pStreamPlayer->Halt();
    g_pStreamPlayer->RestartLevelMusic();
    g_pStreamPlayer->ApplyVolume();
    Sound_ResumeAll();
    Game_ClearFlags(GF_PAUSE_TOGGLE | GF_PAUSED);
    g_gameFlags |= GF_UPDATE_OBJECTS | GF_RENDER_WORLD;
    if (g_pauseExitRequested) {
        g_levelExitFlags |= LEVEL_EXIT_TO_MENU;
        Fade_StartLevelExit(0x1000);
        g_pauseExitRequested = 0;
    }
    if (g_pauseRestartRequested) {
        g_levelExitFlags |= LEVEL_EXIT_RESTART;
        Fade_StartLevelExit(0x1000);
        g_pauseRestartRequested = 0;
    }
    g_pProgress->LoadRecord(g_progressPauseCopy);
    g_pad.SetActuator(g_progressPauseCopy[0x17]);
    len = 7;
    g_pauseCfg[0] = g_pProgress->streamVolumeA;
    g_pauseCfg[1] = g_pProgress->streamVolumeB;
    g_pauseCfg[2] = g_pProgress->sfxVolume;
    g_pauseCfg[3] = g_pProgress->optionBits.soundMode;
    g_pauseCfg[4] = g_pProgress->optionBits.setting;
    g_pauseCfg[5] = g_pProgress->optionBits.gate;
    g_pauseCfg[6] = g_pProgress->viewDistanceSetting;
    if (Reg_CreateSubKey(&key, "CfgGame") == 0) {
        if (Reg_WriteBinary(key, 0, g_pauseCfg, len) == 1)
            Reg_CloseKey(&key);
    }
    g_inputMgr.SetActiveFlags(1);
    g_inputMgr.ResetEdges();
}

/* 0x542d8b */
u32 Game_IsPaused()
{
    return g_gameFlags & GF_PAUSED;
}

/* 0x542d98 - empties the pause menu's node array. */
void PauseMenu_ClearItems()
{
    memset(g_pauseMenu.items, 0, g_pauseMenu.capacity * sizeof(MenuPage));
    g_pauseMenu.count = 0;
}

/* BYTES(inline): source-only inline: 'this' goes through a temp (0x542e71) */
inline void Progress::ToggleAutoSave()
{
    runtimeBits.autoSaveOn ^= 1;
}

/* source-only inline: Progress.runtimeFlags bit 4, the result a temp (0x542dff) */
/* BYTES(inline): source-only inline: its result goes through a temp (0x542dff) */
inline s32 Progress_AutoSaveAvailable()
{
    return g_pProgress->runtimeBits.saveSlotValid;
}

/* 0x542dc2 - "auto save" on/off (Progress.runtimeFlags bit 5), greyed out unless bit 4 is set. */
void PauseMenu_ItemAutoSave(u8 msg, MenuPage *self)
{
    switch (msg) {
        case MENU_MSG_PREDRAW:
            self->disabled = Progress_AutoSaveAvailable() != 1;
            break;
        case MENU_MSG_DRAW_SELECTED:
            Menu_BuildAutoSaveFooter();
            break;
        case MENU_MSG_DRAW:
            Menu_Printf(self, TEXTALIGN_CENTER, "%s %s", Text_GetUiString(UISTR_AUTOMATIC_SAVE),
                        g_onOffNames[g_pProgress->runtimeBits.autoSaveOn]);
            break;
        case MENU_MSG_VALUE_CHANGED:
            Ui_PlayMoveSound();
            g_pProgress->ToggleAutoSave();
            break;
    }
}

/* 0x542eca - the "options" page. */
void PauseMenu_PageOptions(u8 msg, MenuPage *self)
{
    switch (msg) {
        case MENU_MSG_DRAW_SELECTED:
            Menu_BuildPauseFooter();
            break;
        case MENU_MSG_DRAW:
            Menu_Printf(self, TEXTALIGN_CENTER, Text_GetUiString(UISTR_OPTIONS));
            break;
    }
}

/* 0x542f06 - the "sound options" page; while it has the cursor the volumes are those of the snapshot. */
/* BYTES(view, inferred): the volume bytes are copied as one u32, as the original does */
void PauseMenu_PageSoundOptions(u8 msg, MenuPage *self)
{
    switch (msg) {
        case MENU_MSG_DRAW_SELECTED:
            Menu_BuildValidateBackFooter();
            /* cast kept: the three volumes and the option byte (+0x95..+0x98) are copied as one u32 */
            *(u32 *)&g_pProgress->streamVolumeA = *(u32 *)&g_progressPauseCopy[0x11];
            Sound_SetSfxVolume(g_pProgress->sfxVolume);
            g_pStreamPlayer->ApplyVolume();
            break;
        case MENU_MSG_DRAW:
            Menu_Printf(self, TEXTALIGN_CENTER, Text_GetUiString(UISTR_SOUND_OPTIONS));
            break;
    }
}

/* 0x542f73 - "resume": clears GF_PAUSE_TOGGLE (0x20), which closes the menu. */
void PauseMenu_ItemResume(u8 msg, MenuPage *self)
{
    switch (msg) {
        case MENU_MSG_DRAW_SELECTED:
            Menu_BuildPauseFooter();
            break;
        case MENU_MSG_DRAW:
            Menu_Printf(self, TEXTALIGN_CENTER, Text_GetUiString(UISTR_RESUME));
            break;
        case MENU_MSG_CONFIRM:
            GAME_STATE->Game_SetFlags(GF_PAUSE_TOGGLE, 0);
            break;
    }
}

/* 0x542fc5 - the yes/no box shared by "exit" and "restart level": *outConfirmed is set when "yes" is validated. */
void PauseMenu_HandleConfirm(u8 msg, u8 *outConfirmed)
{
    s32 choice;

    switch (msg) {
        case MENU_MSG_DRAW_SELECTED:
            Menu_BuildPauseFooter();
            break;
        case MENU_MSG_CONFIRM:
            if (g_confirmPending == 0) {
                g_pauseSlidersShown = 1;
                g_pauseFlag8b = 0;
                Menu_SetCapture(MENU_CAPTURE_HIDDEN, 0);
            } else {
                g_pauseSlidersShown = 1;
                g_pauseFlag8b = 0;
                GAME_STATE->Game_SetFlags(GF_PAUSED, g_confirmChoice);
                g_confirmChoice = 1;
            }
            g_confirmPending = 0;
            break;
        case MENU_MSG_CAPTURE_TICK:
            Text_CenterVertically(4);
            Text_Printf(TEXTALIGN_CENTER, Text_GetUiString(UISTR_ARE_YOU_SURE));
            Text_NewLine(2);
            Menu_PrintfSelected(g_confirmChoice, TEXTALIGN_CENTER, Text_GetUiString(UISTR_NO));
            Text_NewLine(1);
            Menu_PrintfSelected(g_confirmChoice == 0, TEXTALIGN_CENTER, Text_GetUiString(UISTR_YES));
            if (Pad_MenuPressed((u16)~PAD_CROSS)) {
                choice = g_confirmChoice; /* the release message resets it */
                g_confirmPending = 0;
                Menu_SetCapture(MENU_CAPTURE_NONE, 1);
                g_confirmChoice = choice;
                Ui_PlayConfirmSound();
                Menu_SetDirty(1);
                *outConfirmed = g_confirmChoice == 0;
                GAME_STATE->Game_SetFlags(GF_PAUSE_TOGGLE, g_confirmChoice);
            } else if (Pad_MenuPressed((u16)~PAD_TRIANGLE)) {
                g_confirmPending = 0;
                Menu_SetCapture(MENU_CAPTURE_NONE, 1);
                Ui_PlayCancelSound();
                Menu_SetDirty(1);
            } else if (Pad_MenuRepeat((u16)~PAD_UP) || Pad_MenuRepeat((u16)~PAD_DOWN)) {
                Ui_PlayMoveSound();
                g_confirmChoice ^= 1;
            }
            break;
        case MENU_MSG_CAPTURE_RELEASE:
            g_pauseSlidersShown = 1;
            g_confirmChoice = 1;
            break;
    }
}

/* 0x5431e5 - "exit". */
void PauseMenu_ItemExit(u8 msg, MenuPage *self)
{
    PauseMenu_HandleConfirm(msg, &g_pauseExitRequested);
    if (msg == MENU_MSG_DRAW)
        Menu_Printf(self, TEXTALIGN_CENTER, Text_GetUiString(UISTR_EXIT));
}

/* 0x54321c - "restart level". */
void PauseMenu_ItemRestartLevel(u8 msg, MenuPage *self)
{
    PauseMenu_HandleConfirm(msg, &g_pauseRestartRequested);
    if (msg == MENU_MSG_DRAW)
        Menu_Printf(self, TEXTALIGN_CENTER, Text_GetUiString(UISTR_RESTART_LEVEL));
}

/* 0x543253 - *value += delta, saturating at 0 and 0xff. */
void PauseMenu_StepSetting(u8 *value, s8 delta)
{
    switch (delta >= 0 ? 1 : -1) {
        case -1:
            if (*value + delta > 0)
                *value = *value + delta;
            else
                *value = 0;
            break;
        case 1:
            if (*value + delta < 0xff)
                *value = *value + delta;
            else
                *value = 0xff;
            break;
    }
}

/* 0x5432d3 - the speaker-mode row (Progress.optionFlags bits 0-1, cycled % 3). */
void PauseMenu_ItemSpeakerMode(u8 msg, MenuPage *self)
{
    switch (msg) {
        case MENU_MSG_DRAW_SELECTED:
            Menu_BuildNavFooter();
            break;
        case MENU_MSG_DRAW:
            Menu_Printf(self, TEXTALIGN_LEFT, Text_GetUiString(UISTR_MODE));
            Menu_Printf(self, TEXTALIGN_RIGHT, g_settingNames[g_pProgress->optionBits.setting]);
            break;
        case MENU_MSG_LEFT:
            Progress_Options()->setting = (RECORD_OPTIONS(Progress_Record())->setting + 2) % 3;
            break;
        case MENU_MSG_RIGHT:
            Progress_Options()->setting = (RECORD_OPTIONS(Progress_Record())->setting + 1) % 3;
            break;
        case MENU_MSG_VALUE_CHANGED:
            Stub_Ret(g_pProgress->optionBits.setting);
            Ui_PlayMoveSound();
            break;
    }
}

/* 0x54341c - the body of the three volume rows: label, slider cursor cursorIdx, LEFT / RIGHT step by 8. */
void PauseMenu_VolumeItem(u8 msg, MenuPage *item, u8 cursorIdx, u8 *pValue, u8 uiStringId)
{
    switch (msg) {
        case MENU_MSG_DRAW_SELECTED:
            g_menuCursors[cursorIdx].Animate();
            Menu_BuildNavFooter();
            break;
        case MENU_MSG_DRAW:
            Menu_Printf(item, TEXTALIGN_LEFT, Text_GetUiString(uiStringId));
            if (item->selected || g_pauseSlidersShown) {
                g_menuCursors[cursorIdx].SetPos(0x15c, (s16)g_textCursorY + g_textWinY);
                g_menuCursors[cursorIdx].SetSlide(*pValue);
                g_menuCursors[cursorIdx].Draw(g_screenLayerBase + 2);
            }
            break;
        case MENU_MSG_LEFT:
            PauseMenu_StepSetting(pValue, -8);
            break;
        case MENU_MSG_RIGHT:
            PauseMenu_StepSetting(pValue, 8);
            break;
        case MENU_MSG_VALUE_CHANGED:
            g_pStreamPlayer->ApplyVolume();
            Ui_PlayMoveSound();
            break;
    }
}

/* 0x54354c - "sfx" volume. */
void PauseMenu_ItemSfxVolume(u8 msg, MenuPage *item)
{
    PauseMenu_VolumeItem(msg, item, 0, &g_pProgress->sfxVolume, UISTR_SFX);
    if (msg == MENU_MSG_VALUE_CHANGED)
        Sound_SetSfxVolume(g_pProgress->sfxVolume);
}

/* 0x54358e - "voice" volume. */
void PauseMenu_ItemVoiceVolume(u8 msg, MenuPage *item)
{
    PauseMenu_VolumeItem(msg, item, 1, &g_pProgress->streamVolumeB, UISTR_VOICE);
}

/* 0x5435b2 - "music" volume. */
void PauseMenu_ItemMusicVolume(u8 msg, MenuPage *item)
{
    PauseMenu_VolumeItem(msg, item, 2, &g_pProgress->streamVolumeA, UISTR_MUSIC);
}

/* 0x5435d6 - "sub-titles" on/off (Progress.optionFlags bit 2). */
void PauseMenu_ItemSubTitles(u8 msg, MenuPage *item)
{
    switch (msg) {
        case MENU_MSG_DRAW_SELECTED:
            Menu_BuildNavFooter();
            break;
        case MENU_MSG_DRAW:
            Menu_Printf(item, TEXTALIGN_LEFT, Text_GetUiString(UISTR_SUB_TITLES));
            Menu_Printf(item, TEXTALIGN_RIGHT, g_onOffNames[g_pProgress->optionBits.gate]);
            break;
        case MENU_MSG_VALUE_CHANGED:
            Progress_Options()->gate ^= 1;
            Ui_PlayMoveSound();
            break;
    }
}

/* 0x543683 - "done" on the sound page: the new volumes become the snapshot's. */
void PauseMenu_ItemSoundDone(u8 msg, MenuPage *item)
{
    switch (msg) {
        case MENU_MSG_DRAW_SELECTED:
            g_menuFooterStyle = MENU_FOOTER_VALIDATE_CANCEL;
            Menu_BuildValidateCancelFooter();
            break;
        case MENU_MSG_DRAW:
            Menu_Printf(item, TEXTALIGN_LEFT, Text_GetUiString(UISTR_DONE));
            break;
        case MENU_MSG_CONFIRM:
            /* cast kept: as in PauseMenu_PageSoundOptions: the four bytes copied as one u32 */
            *(u32 *)&g_progressPauseCopy[0x11] = *(u32 *)&g_pProgress->streamVolumeA;
            Menu_GoBack();
            g_pauseSlidersShown = 1;
            break;
    }
}

/* source-only inline: the root's items, which depend on where the player is (a level 0..31, or -1). */
/* BYTES(inline): source-only inline: its expansion gives the original's shape */
inline s32 Progress_InLevel()
{
    s8 level = g_pProgress->currentLevel;
    return level >= SCENE_LVL_00 && level < SCENE_LEVEL_COUNT;
}
inline s8 Progress_Level()
{
    return g_pProgress->currentLevel;
}

/* 0x5436eb - builds the pause menu tree (called by Game_ReloadLevel). */
void PauseMenu_Build()
{
    MenuPage *root;
    MenuPage *optMenu;
    MenuPage *sounds;
    MenuPage *padPage;
    MenuPage *dispPage;
    MenuPage *bindPage;

    Menu_SetCurrent(&g_pauseMenu);
    root = Menu_AddPage(0, "PAUSE", 0);
    /* cast kept (these four calls): Menu_AddPage takes a label or a handler in one void * parameter */
    optMenu = Menu_AddPage(root, (void *)PauseMenu_PageOptions, 1);
    sounds = Menu_AddPage(optMenu, (void *)PauseMenu_PageSoundOptions, 1);
    padPage = Menu_AddPage(optMenu, (void *)PauseMenu_PageControllerSetting, 1);
    /* cast kept: menu pages are passed as untyped function pointers */
    dispPage = Menu_AddPage(optMenu, (void *)PauseMenu_PageDisplaySettings, 1);
    if (Progress_InLevel())
        g_pauseResumeIndex = Menu_AddItems(root, PauseMenu_ItemResume, PauseMenu_ItemRestartLevel, PauseMenu_ItemExit,
                                           PauseMenu_ItemQuit, 0);
    else if (Progress_Level() == -1)
        g_pauseResumeIndex = Menu_AddItems(root, PauseMenu_ItemResume, PauseMenu_ItemQuit, 0);
    else
        g_pauseResumeIndex = Menu_AddItems(root, PauseMenu_ItemResume, PauseMenu_ItemExit, PauseMenu_ItemQuit, 0);
    Menu_AddItems(optMenu, PauseMenu_ItemAutoSave, 0);
    Menu_AddItems(sounds, PauseMenu_ItemSpeakerMode, PauseMenu_ItemSoundsEnabled, PauseMenu_ItemSfxVolume,
                  PauseMenu_ItemVoiceVolume, PauseMenu_ItemMusicVolume, PauseMenu_ItemSubTitles,
                  PauseMenu_ItemSoundDone, 0);
    Menu_AddItems(dispPage, PauseMenu_ItemFog, PauseMenu_ItemDisplayDone, 0);
    Menu_AddItems(padPage, PauseMenu_ItemControlDevice, 0);
    bindPage = Menu_AddPage(padPage, (void *)PauseMenu_PageEditConfig, 1); /* cast kept: as above */
    Menu_AddItems(bindPage, PauseMenu_ItemBindUp, PauseMenu_ItemBindRight, PauseMenu_ItemBindDown,
                  PauseMenu_ItemBindLeft, PauseMenu_ItemBindAction, PauseMenu_ItemBindJump,
                  PauseMenu_ItemBindWolfEyeView, PauseMenu_ItemBindRun, PauseMenu_ItemBindQuickInventory,
                  PauseMenu_ItemBindWalkStealthily, PauseMenu_ItemBindCamLeft, PauseMenu_ItemBindCamRight,
                  PauseMenu_ItemBindViewMap, PauseMenu_ItemSaveConfig, 0);
}

/* 0x543901 - the "controller setting" page. */
void PauseMenu_PageControllerSetting(u8 msg, MenuPage *item)
{
    switch (msg) {
        case MENU_MSG_DRAW_SELECTED:
            Menu_BuildValidateBackFooter();
            break;
        case MENU_MSG_DRAW:
            Menu_Printf(item, TEXTALIGN_CENTER, Text_GetUiString(UISTR_CONTROLLER_SETTING));
            break;
    }
}

/* 0x54393d - the "Display settings" page; while it has the cursor the view distance is the one saved on entry. */
void PauseMenu_PageDisplaySettings(u8 msg, MenuPage *item)
{
    float t;

    switch (msg) {
        case MENU_MSG_DRAW_SELECTED:
            Menu_BuildValidateBackFooter();
            g_pProgress->viewDistanceSetting = g_pauseViewDistance;
            t = (float)(0xff - g_pProgress->viewDistanceSetting) / 255.0f;
            g_pViewFrustum->SetViewDistance((1.0f - t) * g_viewDistNear + g_viewDistFar * t, 1);
            break;
        case MENU_MSG_DRAW:
            Menu_Printf(item, TEXTALIGN_CENTER, g_textCatalog.LoadString(1, g_uiLangMask, 0x800));
            break;
    }
}

/* 0x5439eb - the "Edit config." page: entering it reloads the saved bindings. */
void PauseMenu_PageEditConfig(u8 msg, MenuPage *item)
{
    u8 device;

    switch (msg) {
        case MENU_MSG_DRAW_SELECTED:
            Menu_BuildValidateBackFooter();
            break;
        case MENU_MSG_DRAW:
            Menu_Printf(item, TEXTALIGN_LEFT, g_textCatalog.LoadString(0x10, g_uiLangMask, 8));
            break;
        case MENU_MSG_FOCUS:
            device = g_inputMgr.GetCurrentDeviceIdx();
            g_inputMgr.LoadConfig(g_dirReference);
            g_inputMgr.SelectDevice(device);
            break;
    }
}

/* 0x543a67 - "Quit...". */
void PauseMenu_ItemQuit(u8 msg, MenuPage *item)
{
    PauseMenu_QuitConfirm(msg, item);
    if (msg == MENU_MSG_DRAW)
        Menu_Printf(item, TEXTALIGN_CENTER, g_textCatalog.LoadString(2, g_uiLangMask, 0x10));
}

/* 0x543aa8 - the quit yes/no box: "yes" writes "CfgGame" and posts WM_QUIT. */
void PauseMenu_QuitConfirm(u8 msg, MenuPage *item)
{
    HKEY key;
    DWORD len;

    switch (msg) {
        case MENU_MSG_DRAW_SELECTED:
            Menu_BuildQuitFooter();
            break;
        case MENU_MSG_CONFIRM:
            if (g_quitPending == 0) {
                g_pauseSlidersShown = 1;
                Menu_SetCapture(MENU_CAPTURE_HIDDEN, 0);
            } else {
                GAME_STATE->Game_SetFlags(GF_PAUSED, g_quitChoice);
                g_quitChoice = 1;
            }
            g_quitPending = 0;
            break;
        case MENU_MSG_CAPTURE_TICK:
            Text_CenterVertically(4);
            Text_Printf(TEXTALIGN_CENTER, Text_GetUiString(UISTR_ARE_YOU_SURE));
            Text_NewLine(2);
            Menu_PrintfSelected(g_quitChoice, TEXTALIGN_CENTER, Text_GetUiString(UISTR_NO));
            Text_NewLine(1);
            Menu_PrintfSelected(g_quitChoice == 0, TEXTALIGN_CENTER, Text_GetUiString(UISTR_YES));
            if (Pad_MenuPressed((u16)~PAD_CROSS)) {
                if (g_quitChoice == 0) {
                    len = 7;
                    g_quitCfg[0] = g_pProgress->streamVolumeA;
                    g_quitCfg[1] = g_pProgress->streamVolumeB;
                    g_quitCfg[2] = g_pProgress->sfxVolume;
                    g_quitCfg[3] = g_pProgress->optionBits.soundMode;
                    g_quitCfg[4] = g_pProgress->optionBits.setting;
                    g_quitCfg[5] = g_pProgress->optionBits.gate;
                    g_quitCfg[6] = g_pProgress->viewDistanceSetting;
                    if (Reg_CreateSubKey(&key, "CfgGame") == 0) {
                        if (Reg_WriteBinary(key, 0, g_quitCfg, len) == 1)
                            Reg_CloseKey(&key);
                    }
                    PostQuitMessage(0);
                } else {
                    g_quitPending = 0;
                    Menu_SetCapture(MENU_CAPTURE_NONE, 1);
                    Ui_PlayConfirmSound();
                    GAME_STATE->Game_SetFlags(GF_PAUSE_TOGGLE, g_quitChoice);
                }
            }
            if (Pad_MenuPressed((u16)~PAD_TRIANGLE)) {
                g_quitPending = 0;
                Menu_SetCapture(MENU_CAPTURE_NONE, 1);
                Menu_SetDirty(1);
                Ui_PlayCancelSound();
            }
            if (Pad_MenuRepeat((u16)~PAD_UP) || Pad_MenuRepeat((u16)~PAD_DOWN)) {
                g_quitChoice ^= 1;
                Ui_PlayMoveSound();
            }
            break;
        case MENU_MSG_CAPTURE_RELEASE:
            g_pauseSlidersShown = 1;
            g_quitChoice = 1;
            break;
    }
}

/* 0x543d6e - "Sounds Enabled" (Progress.optionFlags bits 3-4, cycled % 3). */
void PauseMenu_ItemSoundsEnabled(u8 msg, MenuPage *item)
{
    switch (msg) {
        case MENU_MSG_DRAW_SELECTED:
            Menu_BuildNavFooter();
            break;
        case MENU_MSG_DRAW:
            Menu_Printf(item, TEXTALIGN_LEFT, g_textCatalog.LoadString(1, g_uiLangMask, 0x2000));
            Menu_Printf(item, TEXTALIGN_RIGHT, g_soundModeNames[g_pProgress->optionBits.soundMode]);
            break;
        case MENU_MSG_LEFT:
            Progress_Options()->soundMode = (RECORD_OPTIONS(Progress_Record())->soundMode + 2) % 3;
            break;
        case MENU_MSG_RIGHT:
            Progress_Options()->soundMode = (RECORD_OPTIONS(Progress_Record())->soundMode + 1) % 3;
            break;
        case MENU_MSG_VALUE_CHANGED:
            Ui_PlayMoveSound();
            break;
    }
}

/* 0x543ebc - "Fog": the view-distance slider (cursor 3), applied to the frustum on every change. */
void PauseMenu_ItemFog(u8 msg, MenuPage *item)
{
    float t;

    switch (msg) {
        case MENU_MSG_DRAW_SELECTED:
            g_menuCursors[3].Animate();
            Menu_BuildNavFooter();
            break;
        case MENU_MSG_DRAW:
            Menu_Printf(item, TEXTALIGN_LEFT, g_textCatalog.LoadString(1, g_uiLangMask, 0x1000));
            if (item->selected || g_pauseSlidersShown) {
                g_menuCursors[3].SetPos(0xdd, (s16)g_textCursorY + g_textWinY);
                g_menuCursors[3].SetSlide(g_pProgress->viewDistanceSetting);
                g_menuCursors[3].Draw(g_screenLayerBase + 2);
            }
            break;
        case MENU_MSG_LEFT:
            PauseMenu_StepSetting(&g_pProgress->viewDistanceSetting, -8);
            break;
        case MENU_MSG_RIGHT:
            PauseMenu_StepSetting(&g_pProgress->viewDistanceSetting, 8);
            break;
        case MENU_MSG_VALUE_CHANGED:
            t = (float)(0xff - g_pProgress->viewDistanceSetting) / 255.0f;
            g_pViewFrustum->SetViewDistance((1.0f - t) * g_viewDistNear + g_viewDistFar * t, 1);
            break;
    }
}

/* 0x54402f - "done" on the display page: the new view distance is kept. */
void PauseMenu_ItemDisplayDone(u8 msg, MenuPage *item)
{
    switch (msg) {
        case MENU_MSG_DRAW_SELECTED:
            g_menuFooterStyle = MENU_FOOTER_VALIDATE_CANCEL;
            Menu_BuildValidateCancelFooter();
            break;
        case MENU_MSG_DRAW:
            Menu_Printf(item, TEXTALIGN_LEFT, Text_GetUiString(UISTR_DONE));
            break;
        case MENU_MSG_CONFIRM:
            g_pauseViewDistance = g_pProgress->viewDistanceSetting;
            Menu_GoBack();
            g_pauseSlidersShown = 1;
            break;
    }
}

/* 0x544097 - "Control device": keyboard or joystick; CROSS switches, re-creating the joystick if it was lost. */
void PauseMenu_ItemControlDevice(u8 msg, MenuPage *item)
{
    switch (msg) {
        case MENU_MSG_DRAW_SELECTED:
            Menu_BuildControlsFooter();
            break;
        case MENU_MSG_DRAW:
            Menu_Printf(item, TEXTALIGN_LEFT, g_textCatalog.LoadString(1, g_uiLangMask, 0x400));
            switch (g_inputMgr.GetCurrentDeviceIdx()) {
                case INPUTDEV_MASK_JOYSTICK:
                    Menu_Printf(item, TEXTALIGN_RIGHT, g_textCatalog.LoadString(0x10, g_uiLangMask, 2));
                    break;
                default:
                    Menu_Printf(item, TEXTALIGN_RIGHT, g_textCatalog.LoadString(0x10, g_uiLangMask, 4));
                    break;
            }
            break;
        case MENU_MSG_CONFIRM:
            switch (g_inputMgr.GetCurrentDeviceIdx()) {
                case INPUTDEV_MASK_KEYBOARD:
                    if (g_inputMgr.SetAcquired(INPUTDEV_MASK_JOYSTICK, 1) == E_FAIL) {
                        if (g_inputMgr.RecreateDevice(INPUTDEV_MASK_JOYSTICK) >= 0)
                            g_inputMgr.SelectDevice(INPUTDEV_MASK_JOYSTICK);
                    } else {
                        g_inputMgr.SelectDevice(INPUTDEV_MASK_JOYSTICK);
                    }
                    break;
                default:
                    g_inputMgr.SelectDevice(INPUTDEV_MASK_KEYBOARD);
                    break;
            }
            break;
    }
}

/* source-only inline: the current input device, the result a u8 temp (0x5441f9) */
/* BYTES(inline): source-only inline: its result goes through a u8 temp (0x5441f9) */
inline u8 Input_CurrentDevice()
{
    return g_inputMgr.GetCurrentDeviceIdx();
}

/* The four direction rows of "Edit config.": keyboard-only menu keys (slots 12..15), greyed out on the joystick. */
#define PAUSEMENU_BIND_ARROW(NAME, LABEL_INDEX, SLOT, SHOWN, JOY)                           \
    void NAME(u8 msg, MenuPage *item)                                                       \
    {                                                                                       \
        char caption[256];                                                                  \
        char joyText[128];                                                                  \
        switch (msg) {                                                                      \
            case MENU_MSG_DRAW_SELECTED:                                                    \
                Menu_BuildControlsFooter();                                                 \
                break;                                                                      \
            case MENU_MSG_PREDRAW:                                                          \
                item->disabled = Input_CurrentDevice() != INPUTDEV_MASK_KEYBOARD;           \
                break;                                                                      \
            case MENU_MSG_DRAW:                                                             \
                strcpy(caption, g_textCatalog.LoadString(0x10, g_uiLangMask, LABEL_INDEX)); \
                Menu_Printf(item, TEXTALIGN_LEFT, caption);                                 \
                if (SHOWN) {                                                                \
                    if (g_inputMgr.GetCurrentDeviceIdx() == INPUTDEV_MASK_KEYBOARD) {       \
                        Menu_DrawBindingText(SLOT, item);                                   \
                    } else {                                                                \
                        strcpy(joyText, JOY);                                               \
                        strcat(joyText, caption);                                           \
                        Menu_Printf(item, TEXTALIGN_RIGHT, joyText);                        \
                    }                                                                       \
                }                                                                           \
                break;                                                                      \
            case MENU_MSG_CONFIRM:                                                          \
                SHOWN = 0;                                                                  \
                if (!g_pauseRebinding)                                                      \
                    Menu_SetCapture(MENU_CAPTURE_DRAWN, 0);                                 \
                break;                                                                      \
            case MENU_MSG_CAPTURE_TICK:                                                     \
                g_pauseRebinding = 1;                                                       \
                g_menuFooterStyle = MENU_FOOTER_BIND;                                       \
                g_uiFooterText = g_strUiString1E;                                           \
                Menu_RebindControl(SLOT);                                                   \
                break;                                                                      \
            case MENU_MSG_CAPTURE_RELEASE:                                                  \
                SHOWN = 1;                                                                  \
                g_pauseSlidersShown = 1;                                                    \
                g_pauseRebinding = 0;                                                       \
                break;                                                                      \
        }                                                                                   \
    }

/* 0x5441b8 "Up" slot 0xc, 0x544359 "Right" slot 0xd, 0x5444fa "Down" slot 0xe, 0x54469b "Left" slot 0xf. */
PAUSEMENU_BIND_ARROW(PauseMenu_ItemBindUp, 0x10, INPUT_BIND_MENU_UP, g_bindUpShown, "(JOY)")
PAUSEMENU_BIND_ARROW(PauseMenu_ItemBindRight, 0x20, INPUT_BIND_MENU_RIGHT, g_bindRightShown, "(JOY)")
PAUSEMENU_BIND_ARROW(PauseMenu_ItemBindDown, 0x40, INPUT_BIND_MENU_DOWN, g_bindDownShown, "(JOY)")
PAUSEMENU_BIND_ARROW(PauseMenu_ItemBindLeft, 0x80, INPUT_BIND_MENU_LEFT, g_bindLeftShown, "(JOY)")

/* The action rows: "$B_<button>$ (<name>)", the name a UI string (GET_UI) or a BSM string (GET_BSM). */
#define PAUSEMENU_GET_UI(INDEX) Text_GetUiString(INDEX)
#define PAUSEMENU_GET_BSM(INDEX) g_textCatalog.LoadString(0x10, g_uiLangMask, INDEX)
#define PAUSEMENU_BIND_ACTION(NAME, BUTTON, NAME_STRING, SLOT, SHOWN) \
    void NAME(u8 msg, MenuPage *item)                                 \
    {                                                                 \
        char label[64];                                               \
        switch (msg) {                                                \
            case MENU_MSG_DRAW_SELECTED:                              \
                Menu_BuildControlsFooter();                           \
                break;                                                \
            case MENU_MSG_DRAW:                                       \
                strcpy(label, BUTTON);                                \
                strcat(label, " (");                                  \
                strcat(label, NAME_STRING);                           \
                strcat(label, ")");                                   \
                Menu_Printf(item, TEXTALIGN_LEFT, label);             \
                if (SHOWN)                                            \
                    Menu_DrawBindingText(SLOT, item);                 \
                break;                                                \
            case MENU_MSG_CONFIRM:                                    \
                SHOWN = 0;                                            \
                if (!g_pauseRebinding)                                \
                    Menu_SetCapture(MENU_CAPTURE_DRAWN, 0);           \
                break;                                                \
            case MENU_MSG_CAPTURE_TICK:                               \
                g_pauseRebinding = 1;                                 \
                g_menuFooterStyle = MENU_FOOTER_BIND;                 \
                g_uiFooterText = g_strUiString1E;                     \
                Menu_RebindControl(SLOT);                             \
                break;                                                \
            case MENU_MSG_CAPTURE_RELEASE:                            \
                SHOWN = 1;                                            \
                g_pauseSlidersShown = 1;                              \
                g_pauseRebinding = 0;                                 \
                break;                                                \
        }                                                             \
    }

/* 0x54483f .. 0x5451a9 */
PAUSEMENU_BIND_ACTION(PauseMenu_ItemBindAction, "$B_CROSS$", PAUSEMENU_GET_UI(UISTR_ACTION), INPUT_BIND_CROSS,
                      g_bindActionShown)
PAUSEMENU_BIND_ACTION(PauseMenu_ItemBindJump, "$B_SQUARE$", PAUSEMENU_GET_UI(UISTR_JUMP), INPUT_BIND_SQUARE,
                      g_bindJumpShown)
PAUSEMENU_BIND_ACTION(PauseMenu_ItemBindWolfEyeView, "$B_TRIANGLE$", PAUSEMENU_GET_UI(UISTR_WOLFS_EYE_VIEW),
                      INPUT_BIND_TRIANGLE, g_bindWolfEyeShown)
PAUSEMENU_BIND_ACTION(PauseMenu_ItemBindRun, "$B_CIRCLE$", PAUSEMENU_GET_UI(UISTR_RUN), INPUT_BIND_CIRCLE,
                      g_bindRunShown)
PAUSEMENU_BIND_ACTION(PauseMenu_ItemBindQuickInventory, "$B_L1$", PAUSEMENU_GET_UI(UISTR_QUICK_INVENTORY),
                      INPUT_BIND_L1, g_bindQuickInvShown)
PAUSEMENU_BIND_ACTION(PauseMenu_ItemBindWalkStealthily, "$B_R1$", PAUSEMENU_GET_UI(UISTR_WALK_STEALTHILY),
                      INPUT_BIND_R1, g_bindSneakShown)
PAUSEMENU_BIND_ACTION(PauseMenu_ItemBindViewMap, "$B_SELECT$", PAUSEMENU_GET_BSM(0x400), INPUT_BIND_SELECT,
                      g_bindViewMapShown)
PAUSEMENU_BIND_ACTION(PauseMenu_ItemBindCamLeft, "$B_L2$", PAUSEMENU_GET_BSM(0x200), INPUT_BIND_L2, g_bindCamLeftShown)
PAUSEMENU_BIND_ACTION(PauseMenu_ItemBindCamRight, "$B_R2$", PAUSEMENU_GET_BSM(0x100), INPUT_BIND_R2,
                      g_bindCamRightShown)

/* 0x5452e3 - "Save config": writes the bindings to the registry and says so for two seconds. */
void PauseMenu_ItemSaveConfig(u8 msg, MenuPage *item)
{
    switch (msg) {
        case MENU_MSG_DRAW_SELECTED:
            g_menuFooterStyle = MENU_FOOTER_VALIDATE_CANCEL;
            Menu_BuildValidateCancelFooter();
            break;
        case MENU_MSG_DRAW:
            Menu_Printf(item, TEXTALIGN_LEFT, g_textCatalog.LoadString(0x10, g_uiLangMask, 0x800));
            break;
        case MENU_MSG_CONFIRM:
            g_inputMgr.SaveConfig(0);
            Menu_ShowMessageBox(g_textCatalog.LoadString(8, g_uiLangMask, 0x400), TEXTALIGN_CENTER, 2000);
            Menu_GoBack();
            g_pauseSlidersShown = 1;
            break;
    }
}

/* 0x54537d - 1 if every character of the key name is a letter, a digit or one of " !()+,-./:=?" (the font has them). */
int Menu_IsKeyNameDisplayable(const char *keyName)
{
    char allowed[] = " !()+,-./:=?";
    u32 i;
    u32 j;
    int valid;
    int found;

    if (strlen(keyName) == 0)
        return 0;
    for (i = 0; i < strlen(keyName); i++) {
        valid = keyName[i] >= 'a' && keyName[i] <= 'z';
        valid |= keyName[i] >= 'A' && keyName[i] <= 'Z';
        valid |= keyName[i] >= '0' && keyName[i] <= '9';
        if (!valid) {
            found = 0;
            for (j = 0; j < strlen(allowed); j++) {
                if (keyName[i] == allowed[j]) {
                    found = 1;
                    break;
                }
            }
            if (!found)
                return 0;
        }
    }
    return 1;
}

/* 0x5454e7 - "press a key": binds the first control pressed to actionId, swapping it with the action that had it.
 * Slots 12..15 (the menu arrows) take keyboard keys only. */
/* BYTES(dead-code): unused is never used: it only fills the frame's 12-byte gap at ebp-0x18 */
void Menu_RebindControl(u8 actionId)
{
    long result;
    u8 changed;
    InputBindingIdx newKey;
    char keyText[64];
    char unused[12]; /* never used; it only fills the frame's 12-byte gap at ebp-0x18 */
    int usable;
    u8 clash;
    InputBindingIdx oldBind;

    changed = 0;
    if (g_rebindCooldown <= 0) {
        g_inputMgr.ResetEdges();
        result = g_inputMgr.GetAnyPressed(&newKey);
        if (result == (long)E_FAIL) {
            Menu_SetCapture(MENU_CAPTURE_NONE, 0);
        } else if (result == 0) {
            g_inputMgr.Input_GetKeyName(newKey.code, keyText, 0x40);
            if (!g_inputMgr.IsReservedKey(&newKey)) {
                usable = 1;
                if (newKey.devIdx != INPUTDEV_MASK_JOYSTICK)
                    usable = Menu_IsKeyNameDisplayable(keyText);
                if (usable) {
                    clash = g_inputMgr.FindBindingByIndex(&newKey);
                    if (clash < 0x10) {
                        g_inputMgr.GetBinding(actionId, &oldBind);
                        if ((clash > INPUT_BIND_R3) == 1) {
                            if ((actionId > INPUT_BIND_R3) == 1) {
                                if (oldBind.devIdx == INPUTDEV_MASK_KEYBOARD &&
                                    newKey.devIdx == INPUTDEV_MASK_KEYBOARD) {
                                    g_inputMgr.BindMenuKey(clash, oldBind.code);
                                    g_inputMgr.BindMenuKey(actionId, newKey.code);
                                    changed = 1;
                                }
                            } else if (oldBind.devIdx == INPUTDEV_MASK_KEYBOARD) {
                                g_inputMgr.BindMenuKey(clash, oldBind.code);
                                g_inputMgr.BindAction(actionId, newKey.devIdx, newKey.code);
                                changed = 1;
                            }
                        } else if ((actionId > INPUT_BIND_R3) == 1) {
                            if (newKey.devIdx == INPUTDEV_MASK_KEYBOARD) {
                                g_inputMgr.BindAction(clash, oldBind.devIdx, oldBind.code);
                                g_inputMgr.BindMenuKey(actionId, newKey.code);
                                changed = 1;
                            }
                        } else {
                            g_inputMgr.BindAction(clash, oldBind.devIdx, oldBind.code);
                            g_inputMgr.BindAction(actionId, newKey.devIdx, newKey.code);
                            changed = 1;
                        }
                    } else if ((actionId > INPUT_BIND_R3) == 1) {
                        if (newKey.devIdx == INPUTDEV_MASK_KEYBOARD) {
                            g_inputMgr.BindMenuKey(actionId, newKey.code);
                            changed = 1;
                        }
                    } else {
                        g_inputMgr.BindAction(actionId, newKey.devIdx, newKey.code);
                        changed = 1;
                    }
                } else {
                    Menu_ShowMessageBox(g_textCatalog.LoadString(8, g_uiLangMask, 0x200), TEXTALIGN_CENTER, 2000);
                }
            } else {
                Menu_ShowMessageBox(g_textCatalog.LoadString(8, g_uiLangMask, 0x200), TEXTALIGN_CENTER, 2000);
            }
            if (changed == 1)
                Menu_SetCapture(MENU_CAPTURE_NONE, 0);
        }
    }
}

/* ======================================================================================================================
 * 0x54579d-0x546679: the tail of the menu module: the pause menu's
 * per-frame update and its TV-static backdrop, the binding text of the controls page, the footer-hint builders, the
 * timed message box, the Menu_Printf wrappers, the one-item "PAUSED" menu (PausedMenu_*).
 */

/* source-only inlines (as in src/engine/map.cpp / pause_menu.cpp): the 512 x 240 virtual screen. Their constants reach
 * the code unfolded (mov edx,0x200; sub edx,0x100 at 0x5457f9). */
/* BYTES(inline): source-only inlines: their constants reach the code unfolded (mov edx,0x200; sub edx,0x100 at 0x5457f9) */
#define SDW_INLINE_FREE_SCREENWIDTHS16 1
#include "screen_inlines.h"
#undef SDW_INLINE_FREE_SCREENWIDTHS16
#define SDW_INLINE_FREE_SCREENHEIGHTS16 1
#include "screen_inlines.h"
#undef SDW_INLINE_FREE_SCREENHEIGHTS16

/* the noise texture's surface description gets its dwSize before every DirectDraw call on it: a macro, since the global
 * is re-read at every test (0x545999, 0x545a16, 0x545b2b) */
#define NOISE_TEX_FIX_DESC()                                                           \
    /* cast kept: Texture.desc holds the DDSURFACEDESC2 as raw bytes */                \
    if (g_pMenuNoiseTexture && !((DDSURFACEDESC2 *)g_pMenuNoiseTexture->desc)->dwSize) \
    ((DDSURFACEDESC2 *)g_pMenuNoiseTexture->desc)->dwSize = sizeof(DDSURFACEDESC2)

/* 0x54579d - one frame of the open pause menu: leave it when the pad reports a connection (and the game is not in
 * flag 0x20), else the static backdrop, the "pause" title, the menu itself in the pause box, the footer hint two rows
 * below it and the message box on top. */
/* BYTES(dead-code): result is stored and never read, as in the original */
void PauseMenu_Update()
{
    u32 result;

    if (g_pad.IsConnected() && !(g_gameFlags & GF_PAUSE_TOGGLE)) {
        PauseMenu_Exit();
    } else {
        g_pPolyBin->Flush();
        PauseMenu_DrawNoiseOverlay();
        Text_SetFont(FONT_GAME_SMALL);
        g_pauseTextRect[0] = 0x80;
        g_pauseTextRect[1] = g_pauseBoxRect[1] - 0x18;
        g_pauseTextRect[2] = ScreenWidthS16() - 0x100;
        g_pauseTextRect[3] = g_pCurFont->lineHeight;
        Text_SetWindowRect(g_screenLayerBase, g_pauseTextRect, 1);
        Text_Printf(TEXTALIGN_CENTER, Text_GetUiString(UISTR_PAUSE));
        Hud_EndBox_stub();
        g_pauseFlag89 = 0;
        g_menuFooterStylePrev = g_menuFooterStyle;
        g_menuFooterStyle = MENU_FOOTER_NONE;
        Text_SetWindowRect(g_screenLayerBase + 2, g_pauseBoxRect, 1);
        result = Menu_Update(MENU_LAYOUT_CENTERED, TEXTALIGN_CENTER);
        Hud_EndBox_stub();
        g_pauseTextRect[0] = 0x10;
        g_pauseTextRect[1] = g_pauseBoxRect[1] + 0xa8 - g_pCurFont->lineHeight / 2;
        g_pauseTextRect[2] = ScreenWidthS16() - 0x20;
        g_pauseTextRect[3] = g_pCurFont->lineHeight * 2;
        Text_SetColor(0x4bccff);
        Text_SetWindowRect(g_screenLayerBase + 2, g_pauseTextRect, 1);
        Text_Printf(TEXTALIGN_CENTER, "%s", g_uiFooterText);
        Hud_EndBox_stub();
        Menu_DrawMessageBox();
    }
}

/* 0x545929 - makes (or, after a lost surface, restores) the 256 x 256 noise texture behind the pause menu and fills it
 * with random levels 6..9 in the top nibble (alpha) over 0x00f. Returns 0 in every reachable case: E_FAIL needs
 * "create" and "lost" both set, and "lost" is only computed when the texture already exists. A failed creation
 * (status != 0) keeps the new object, clears "create", skips the fill and still returns 0. */
long Menu_CreateNoiseTexture()
{
    s32 status;
    s32 lost;
    u16 *dst;
    s32 create;

    create = 0;
    lost = 0;
    create = g_pMenuNoiseTexture == 0;
    lost = !create && g_pMenuNoiseTexture->surface && g_pMenuNoiseTexture->surface->IsLost() == DDERR_SURFACELOST;
    if (create) {
        NOISE_TEX_FIX_DESC();
        g_pMenuNoiseTexture = new Texture(g_pD3DAppMain, 0x100, 0x100, TEXFMT_ARGB4444, &status);
        if (status)
            create = 0;
    }
    if (lost) {
        NOISE_TEX_FIX_DESC();
        if (g_pMenuNoiseTexture->surface->Restore())
            lost = 0;
    }
    if (lost ^ create) {
        /* cast kept: Texture.desc holds the DDSURFACEDESC2 as raw bytes */
        if (g_pMenuNoiseTexture->Surface_LockForWrite((DDSURFACEDESC2 *)g_pMenuNoiseTexture->desc) >= 0) {
            srand(time(0));
            /* cast kept: the locked surface is raw memory of 16-bit texels; desc is raw bytes */
            dst = (u16 *)((DDSURFACEDESC2 *)g_pMenuNoiseTexture->desc)->lpSurface;
            if (dst) {
                u16 row;
                u16 column;
                for (row = 0; row < 0x100; row++) {
                    for (column = 0; column < 0x100; column++) {
                        s32 grey = rand() * 4 / 0x7fff + 6;
                        dst[(row << 8) + column] = ((u16)grey << 12) + 0xf;
                    }
                }
            }
            g_pMenuNoiseTexture->Surface_Unlock();
            NOISE_TEX_FIX_DESC();
            return 0;
        }
    }
    if (!create || !lost)
        return 0;
    return E_FAIL;
}

/* 0x545b69 - the pause menu's backdrop: the noise texture stretched over the whole viewport (texture coordinates up to
 * viewport / 256, so it tiles), grey 0xc0c0c0. */
void PauseMenu_DrawNoiseOverlay()
{
    float uMax;
    float v;

    if (Menu_CreateNoiseTexture() == 0) {
        uMax = g_pViewFrustum->viewportWidth / 256.0f;
        v = g_pViewFrustum->viewportHeight / 256.0f;
        Draw2D_TexRect_Immediate(
            g_screen.Draw2D_LayerToZ(g_screenLayerBase + 3), 0, 0, g_pViewFrustum->viewportWidth,
            g_pViewFrustum->viewportHeight, RSF_BLEND_ALPHA | RSF_ZWRITE_OFF | RSF_TEXTURED | RSF_FILTER_LINEAR,
            g_pMenuNoiseTexture, 0, 0, 0xc0c0c0, 0, v, 0xc0c0c0, uMax, 0, 0xc0c0c0, uMax, v, 0xc0c0c0);
    }
}

/* 0x545c11 - the control currently bound to an action, printed on its row of the controls page: "(MOUSE)n" or "(JOY)n"
 * in green, or the key's name in yellow, cut at its first space when it would run past x 0x1c0 of the window. */
void Menu_DrawBindingText(u8 actionId, MenuPage *item)
{
    char *firstSpace;
    s32 n;
    s32 room;
    char name[64];
    char text[64];
    InputBindingIdx b;

    g_inputMgr.GetBinding(actionId, &b);
    switch (b.devIdx) {
        case INPUTDEV_MASK_MOUSE:
            sprintf(text, "(MOUSE)%d", b.code);
            Text_SetColor(0x50ff50);
            break;
        case INPUTDEV_MASK_JOYSTICK:
            sprintf(text, "(JOY)%d", b.code);
            Text_SetColor(0x50ff50);
            break;
        default:
            g_inputMgr.Input_GetKeyName(b.code, name, 0x40);
            room = (g_textClipOffX + 0x1c0 - g_textCursorX) / g_pCurFont->glyphWidth - 1;
            if (room < strlen(name)) {
                firstSpace = strchr(name, ' ');
                if (firstSpace)
                    n = firstSpace - name;
                else
                    n = room;
                strncpy(text, name, n);
                text[n] = 0;
            } else {
                strcpy(text, name);
            }
            Text_SetColor(0xffff50);
            break;
    }
    Menu_Printf(item, TEXTALIGN_RIGHT, text);
    Text_SetColor(0x4bccff);
}

/* 0x545d9f - footer "VALID <Return> validate / CANCEL <Escape> <UI string 0x21>" (style 8). */
void Menu_BuildValidateCancelFooter()
{
    char format[256];
    char keyValid[64];
    char keyCancel[64];

    g_inputMgr.Input_GetKeyName(DIK_RETURN, keyValid, 0x40);
    g_inputMgr.Input_GetKeyName(DIK_ESCAPE, keyCancel, 0x40);
    g_menuFooterStyle = MENU_FOOTER_VALIDATE_CANCEL;
    strcpy(format, "$B_VALID$/$C_CYAN$");
    strcat(format, keyValid);
    strcat(format, "$C_DEFAULT$ %s\n");
    strcat(format, "$B_CANCEL$/$C_CYAN$");
    strcat(format, keyCancel);
    strcat(format, "$C_DEFAULT$ %s");
    g_uiFooterText = Text_Sprintf(g_menuFooterText, format, g_strValidate, g_strUiString21);
}

/* 0x545e74 - footer "VALID <Return> validate / CANCEL <Escape> cancel" (style 1). */
void Menu_BuildValidateBackFooter()
{
    char format[256];
    char keyValid[64];
    char keyCancel[64];

    g_inputMgr.Input_GetKeyName(DIK_RETURN, keyValid, 0x40);
    g_inputMgr.Input_GetKeyName(DIK_ESCAPE, keyCancel, 0x40);
    g_menuFooterStyle = MENU_FOOTER_VALIDATE_BACK;
    strcpy(format, "$B_VALID$/$C_CYAN$");
    strcat(format, keyValid);
    strcat(format, "$C_DEFAULT$ %s\n");
    strcat(format, "$B_CANCEL$/$C_CYAN$");
    strcat(format, keyCancel);
    strcat(format, "$C_DEFAULT$ %s");
    g_uiFooterText = Text_Sprintf(g_menuFooterText, format, g_strValidate, g_strCancel);
}

/* 0x545f49 */
void Menu_BuildQuitFooter()
{
    Menu_BuildValidateBackFooter();
}

/* 0x545f53 */
void Menu_BuildPauseFooter()
{
    Menu_BuildValidateBackFooter();
}

/* 0x545f5d - footer "CANCEL <Escape> cancel" on the second line (style 3). */
void Menu_BuildBackFooter()
{
    char format[256];
    char keyCancel[64];

    g_inputMgr.Input_GetKeyName(DIK_ESCAPE, keyCancel, 0x40);
    g_menuFooterStyle = MENU_FOOTER_BACK;
    strcpy(format, "\n$B_CANCEL$/$C_CYAN$");
    strcat(format, keyCancel);
    strcat(format, "$C_DEFAULT$ %s");
    g_uiFooterText = Text_Sprintf(g_menuFooterText, format, g_strCancel);
}

/* 0x545fde - footer "LEFT/RIGHT <UI string 0x1c> / CANCEL <Escape> cancel" (style 5). */
void Menu_BuildNavFooter()
{
    char format[256];
    char keyCancel[64];

    g_inputMgr.Input_GetKeyName(DIK_ESCAPE, keyCancel, 0x40);
    g_menuFooterStyle = MENU_FOOTER_NAV;
    strcpy(format, "$B_LEFT$$B_RIGHT$ %s\n");
    strcat(format, "$B_CANCEL$/$C_CYAN$");
    strcat(format, keyCancel);
    strcat(format, "$C_DEFAULT$ %s");
    g_uiFooterText = Text_Sprintf(g_menuFooterText, format, g_strUiString1C, g_strCancel);
}

/* 0x546079 */
void Menu_BuildAutoSaveFooter()
{
    Menu_BuildNavFooter();
}

/* 0x546083 - footer "VALID <Return> <UI string 0x1c> / CANCEL <Escape> cancel" (style 6). */
void Menu_BuildControlsFooter()
{
    char format[256];
    char keyValid[64];
    char keyCancel[64];

    g_inputMgr.Input_GetKeyName(DIK_RETURN, keyValid, 0x40);
    g_inputMgr.Input_GetKeyName(DIK_ESCAPE, keyCancel, 0x40);
    g_menuFooterStyle = MENU_FOOTER_CONTROLS;
    strcpy(format, "$B_VALID$/$C_CYAN$");
    strcat(format, keyValid);
    strcat(format, "$C_DEFAULT$ %s\n");
    strcat(format, "$B_CANCEL$/$C_CYAN$");
    strcat(format, keyCancel);
    strcat(format, "$C_DEFAULT$ %s");
    g_uiFooterText = Text_Sprintf(g_menuFooterText, format, g_strUiString1C, g_strCancel);
}

/* 0x546158 - opens the timed message box: sized to the text plus a one-cell margin, centred on the screen, framed with
 * frame bitmap 0x15. The text is strcpy'd into a 0x80-byte buffer with no bound. */
void Menu_ShowMessageBox(const char *text, u8 align, s32 durationMs)
{
    g_msgBoxRect[2] = (Text_MaxLineLength(text) + 2) * g_pCurFont->glyphWidth;
    g_msgBoxRect[3] = (Text_CountWrappedLines(text) + 2) * g_pCurFont->lineHeight;
    g_msgBoxRect[0] = ScreenWidthS16() / 2 - g_msgBoxRect[2] / 2;
    g_msgBoxRect[1] = ScreenHeightS16() / 2 - g_msgBoxRect[3] / 2;
    Ui_BuildFrameQuads(&g_msgBoxFrame, g_msgBoxRect, DAV_IDI_IGLCADP_, 8);
    strcpy(g_msgBoxText, text);
    g_msgBoxAlign = align;
    g_rebindCooldown = durationMs;
}

/* 0x546226 - draws the message box while its time lasts (holding the menu's input meanwhile) and counts the time down
 * in real milliseconds; releases the menu when it runs out. */
/* BYTES(slot-group): the four corners are one struct only to keep the original slot order */
void Menu_DrawMessageBox()
{
    u32 color;
    u16 strip;
    struct {
        float y0, x0, x1, y1;
    } scr; /* one local: the four scaled corners in the original's slot order */

    if (g_rebindCooldown > 0) {
        Menu_SetCapture(MENU_CAPTURE_DRAWN, 0);
        Draw2D_FlatRect_Immediate(g_screen.Draw2D_LayerToZ(g_screenLayerBase + 1), g_screen.ScaleX(g_msgBoxRect[0]),
                                  g_screen.ScaleY(g_msgBoxRect[1]), g_screen.ScaleX(g_msgBoxRect[0] + g_msgBoxRect[2]),
                                  g_screen.ScaleY(g_msgBoxRect[1] + g_msgBoxRect[3]), RSF_BLEND_ALPHA, 0x60404040);
        Text_SetWindow(g_screenLayerBase + 1, g_msgBoxRect[0], g_msgBoxRect[1], g_msgBoxRect[2], g_msgBoxRect[3], 1);
        Text_CenterVertically(Text_CountWrappedLines(g_msgBoxText));
        Text_PrintfStyled(g_msgBoxAlign, 1, "%s", g_msgBoxText);
        color = 0xaa8832;
        for (strip = 0; strip < 4; strip++) {
            scr.x0 = g_screen.ScaleX(g_msgBoxFrame.quads[strip][0]);
            scr.y0 = g_screen.ScaleY(g_msgBoxFrame.quads[strip][1]);
            scr.x1 = g_screen.ScaleX(g_msgBoxFrame.quads[strip][6]);
            scr.y1 = g_screen.ScaleY(g_msgBoxFrame.quads[strip][7]);
            Draw2D_TexRect(g_screen.Draw2D_LayerToZ(g_screenLayerBase + 1), scr.x0, scr.y0, scr.x1, scr.y1,
                           g_msgBoxFrame.texPage, g_msgBoxFrame.uvs[strip][0], g_msgBoxFrame.uvs[strip][1], color,
                           g_msgBoxFrame.uvs[strip][4], g_msgBoxFrame.uvs[strip][5], color, g_msgBoxFrame.uvs[strip][2],
                           g_msgBoxFrame.uvs[strip][3], color, g_msgBoxFrame.uvs[strip][6], g_msgBoxFrame.uvs[strip][7],
                           color);
        }
        g_rebindCooldown -= g_dtRawMs;
        if (g_rebindCooldown <= 0)
            Menu_SetCapture(MENU_CAPTURE_NONE, 0);
    }
}

/* 0x5464bd - Text_PrintFmtStyled with an explicit blink flag. */
void Menu_PrintfSelected(int blink, u8 align, const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    Text_PrintFmtStyled(align, blink, fmt, ap);
}

/* 0x5464e3 - one menu row's text; the row blinks while it is the selected one. */
void Menu_Printf(MenuPage *item, u8 align, const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    Text_PrintFmtStyled(align, item->selected, fmt, ap);
}

/* 0x546515 - opens the one-item "PAUSED" menu (the sibling of PauseMenu_Open 0x542afa): snapshot of the progress
 * record, game paused (0x40), flags 0x4001 cleared. */
void PausedMenu_Open()
{
    Stub_Ret();
    g_pad.SetActuator(0);
    g_pProgress->CopyRecord(g_progressPauseCopy);
    g_gameFlags |= GF_PAUSED;
    Menu_SetCurrent(&g_pausedMenu);
    Game_ClearFlags(GF_BIT0 | GF_UPDATE_OBJECTS);
    Menu_ResetToRoot(&g_pausedMenu, -1);
    PauseMenu_OnOpen_stub();
    g_menuFooterStyle = MENU_FOOTER_NONE;
    g_pauseFlag89 = 1;
    g_pauseSlidersShown = 1;
    g_pauseRebinding = 0;
    g_pauseFlag8b = 0;
    g_pauseExitRequested = 0;
    g_pauseRestartRequested = 0;
    g_inputMgr.SetActiveFlags(0);
}

/* 0x5465c1 - builds g_pausedMenu: a "PAUSED" root with one item. */
void PausedMenu_Build()
{
    MenuPage *page;

    Menu_SetCurrent(&g_pausedMenu);
    page = Menu_AddPage(0, "PAUSED", 0);
    Menu_AddItems(page, PausedMenu_ItemPaused, 0);
}

/* 0x5465fd - empties g_pausedMenu's node array. */
void PausedMenu_ClearItems()
{
    memset(g_pausedMenu.items, 0, g_pausedMenu.capacity * sizeof(MenuPage));
    g_pausedMenu.count = 0;
}

/* 0x546627 - the "PAUSED" menu's only item: BSM string group 0x10, index 1. */
void PausedMenu_ItemPaused(u8 msg, MenuPage *item)
{
    switch (msg) {
        case MENU_MSG_DRAW_SELECTED:
            Menu_BuildBackFooter();
            break;
        case MENU_MSG_DRAW:
            Menu_Printf(item, TEXTALIGN_CENTER, g_textCatalog.LoadString(0x10, g_uiLangMask, 1));
            break;
    }
}

/* 0x54666f */
void PausedMenu_Stub() {}

/* 0x546674 */
void PauseMenu_OnOpen_stub() {}
