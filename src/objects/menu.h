#ifndef SDW_OBJECTS_MENU_H
#define SDW_OBJECTS_MENU_H

/* The functions and globals menu.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

struct Menu;
struct MenuPage;

MenuPage *Menu_AddPage(MenuPage *parent, void *labelOrHandler, u8 hasHandler); /* 0x54eb76 */
void Menu_Close();                                                             /* 0x54f837 */
void Menu_GoBack();                                                            /* 0x54efc0 */
void Menu_Init(Menu *menu);                                                    /* 0x54ec74 */
void Menu_ResetToRoot(Menu *menu, s16 cursor);                                 /* 0x54ed27 */
void Menu_SetCapture(u8 captureMode, int notifyOnRelease);                     /* 0x54ee9a */
void Menu_SetCurrent(Menu *menu);                                              /* 0x54ed8d */
void Menu_SetDirty(u32 on);                                                    /* 0x54e810 */
u32 Menu_Update(s32 layout, u8 align);                                         /* 0x54f027 */

#endif
