#ifndef SDW_ENGINE_PAUSE_MENU_H
#define SDW_ENGINE_PAUSE_MENU_H

/* The functions and globals pause_menu.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

struct MenuPage;
class Texture;

extern Texture *g_pMenuNoiseTexture; /* 0x6def88 built by Menu_CreateNoiseTexture 0x545929 */
u32 Game_IsPaused();                 /* 0x542d8b returns g_gameFlags & 0x40 (paused) */
void Menu_BuildAutoSaveFooter();     /* 0x546079 */
void Menu_BuildBackFooter();
void Menu_BuildControlsFooter();                        /* 0x546083 */
void Menu_BuildNavFooter();                             /* 0x545fde */
void Menu_BuildPauseFooter();                           /* 0x545f53 */
void Menu_BuildQuitFooter();                            /* 0x545f49 */
void Menu_BuildValidateBackFooter();                    /* 0x545e74 */
void Menu_BuildValidateCancelFooter();                  /* 0x545d9f */
long Menu_CreateNoiseTexture();                         /* 0x545929 */
void Menu_DrawBindingText(u8 actionId, MenuPage *item); /* 0x545c11 */
void Menu_DrawMessageBox();
void Menu_Printf(MenuPage *item, u8 align, const char *fmt, ...);    /* 0x5464e3 */
void Menu_PrintfSelected(int blink, u8 align, const char *fmt, ...); /* 0x5464bd */
void Menu_RebindControl(u8 actionId);
void Menu_ShowMessageBox(const char *text, u8 align, s32 durationMs); /* 0x546158 */
void Menus_LoadLevelUi();    /* 0x5428c0 pause-menu UI setup (frame skins, UI strings) */
void PauseMenu_Build();      /* 0x5436eb */
void PauseMenu_ClearItems(); /* 0x542d98 */
void PauseMenu_DrawNoiseOverlay();
void PauseMenu_Exit(); /* 0x542bcd */
void PauseMenu_ItemAutoSave(u8 msg, MenuPage *self);
void PauseMenu_ItemControlDevice(u8 msg, MenuPage *item);
void PauseMenu_ItemDisplayDone(u8 msg, MenuPage *item);
void PauseMenu_ItemExit(u8 msg, MenuPage *self);
void PauseMenu_ItemFog(u8 msg, MenuPage *item);
void PauseMenu_ItemMusicVolume(u8 msg, MenuPage *item);
void PauseMenu_ItemQuit(u8 msg, MenuPage *item);
void PauseMenu_ItemRestartLevel(u8 msg, MenuPage *self);
void PauseMenu_ItemResume(u8 msg, MenuPage *self);
void PauseMenu_ItemSfxVolume(u8 msg, MenuPage *item);
void PauseMenu_ItemSoundDone(u8 msg, MenuPage *item);
void PauseMenu_ItemSoundsEnabled(u8 msg, MenuPage *item);
void PauseMenu_ItemSpeakerMode(u8 msg, MenuPage *self);
void PauseMenu_ItemSubTitles(u8 msg, MenuPage *item);
void PauseMenu_ItemVoiceVolume(u8 msg, MenuPage *item);
void PauseMenu_OnOpen_stub(); /* 0x546674 */
void PauseMenu_Open();        /* 0x542afa */
void PauseMenu_PageControllerSetting(u8 msg, MenuPage *item);
void PauseMenu_PageDisplaySettings(u8 msg, MenuPage *item);
void PauseMenu_PageEditConfig(u8 msg, MenuPage *item);
void PauseMenu_PageOptions(u8 msg, MenuPage *self);
void PauseMenu_PageSoundOptions(u8 msg, MenuPage *self);
void PauseMenu_QuitConfirm(u8 msg, MenuPage *item);
void PauseMenu_Update(); /* 0x54579d pause-menu update (draw, or PauseMenu_Exit) */
void PausedMenu_Build(); /* 0x5465c1 */
void PausedMenu_ItemPaused(u8 msg, MenuPage *item);
void PausedMenu_Open(); /* 0x546515 the other pause-menu opener (sibling of PauseMenu_Open) */

/* The functions and globals pause_menu.cpp defines, declared once for every file that uses them. */

struct Menu;

extern Menu g_pauseMenu; /* 0x57c128 */

#endif
