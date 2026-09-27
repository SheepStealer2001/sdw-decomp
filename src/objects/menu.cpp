/* PAL PC Menu, 0x54e800-0x54f837. A flat MenuPage tree. */
/* BYTES: dead-code. */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "sdw_classes.h"
#include "sdw_global_views.h"
typedef void (*MenuHandler)(u8, MenuPage *);
/* T290 .bss 0x6e38b0..0x6e38d0 (data/structs/MenuState.csv). */
MenuState g_menuState;
#define g_curMenu g_menuState.current
#define g_menuItemCount g_menuState.itemCount
#define g_menuCapacity g_menuState.capacity
#define g_menuCursor g_menuState.cursor
#define g_menuLastCursor g_menuState.lastCursor
#define g_menuCurPage g_menuState.currentPage
#define g_menuPrevPage g_menuState.previousPage
#define g_menuCapture g_menuState.capture
#define g_menuCapturePrev g_menuState.previousCapture
#define g_menuCursorRow g_menuState.cursorRow
#define g_menuPages g_menuState.pages
#include "../engine/game_state.h"
#include "../engine/draw2d.h"
#include "../engine/text.h"
#include "../engine/input.h"
#include "../engine/prompt.h"
extern u32 g_gameFlags;
#define g_padMenuCur (g_pad.menuCur)

#define g_padMenuPrev (g_pad.menuPrev)

extern TextPort g_textPort;
#define g_textWinH (g_textPort.winH)

extern TextPort g_textPort;
#define g_textCursorY (g_textPort.cursorY)
/* cast kept: MenuPage.handler is one void * slot for a MenuHandler or a char * label (hasHandler says which) */
#define DISPATCH(index, message) ((MenuHandler)g_menuPages[index].handler)(message, &g_menuPages[index])
#define SDW_INLINE_FREE_GAME_CLEARFLAGS_U32 1
#include "../engine/game_state_inlines.h"
#undef SDW_INLINE_FREE_GAME_CLEARFLAGS_U32
s32 Menu_IsDirty()
{
    return g_menuState.dirty;
}
void Menu_SetDirty(u32 on)
{
    g_menuState.dirty = on;
}
s16 Menu_GetItemIndex(MenuPage *item)
{
    if (!item)
        return -1;
    return item - g_menuPages;
}
s16 Menu_FindPrevSibling(s16 index)
{
    MenuPage *a = &g_menuPages[index];
    s16 b = index - 1;
    while ((g_menuPages[b].parent != a->parent && b > 0) || g_menuPages[b].disabled)
        b--;
    if (g_menuPages[b].parent != a->parent)
        return index;
    return b;
}
s16 Menu_FindNextSibling(s16 index)
{
    MenuPage *a = &g_menuPages[index];
    s16 b = index + 1;
    if (b == g_menuItemCount)
        return index;
    while ((g_menuPages[b].parent != a->parent && b < g_menuItemCount - 1) || g_menuPages[b].disabled)
        b++;
    if (g_menuPages[b].parent != a->parent)
        return index;
    return b;
}
s16 Menu_FindFirstChild(s16 page)
{
    s16 a = 1;
    while ((g_menuPages[a].parent != page && a < g_menuItemCount - 1) || g_menuPages[a].disabled)
        a++;
    if (g_menuPages[a].parent != page)
        return 1;
    return a;
}
s16 Menu_FindLastChild(s16 page)
{
    s16 a = g_menuItemCount - 1;
    while ((g_menuPages[a].parent != page && a > 0) || g_menuPages[a].disabled)
        a--;
    return a;
}
s16 Menu_AddItems(MenuPage *page, MenuHandler handler, ...)
{
    char *a;
    s16 b;
    s16 c;
    MenuHandler d;
    MenuPage *e;
    /* The original vararg walk starts with the named first handler. */
    /* cast kept (a and both reads through it): the variable argument list is walked by hand from the named handler */
    a = (char *)&handler;
    c = Menu_GetItemIndex(page);
    d = *(MenuHandler *)((a += 4) - 4);
    b = g_menuItemCount;
    g_menuState.dirty = 1;
    do {
        e = &g_menuPages[g_menuItemCount];
        e->kind = MENU_KIND_ITEM;
        e->hasHandler = 1;
        e->disabled = 0;
        e->selected = 0;
        e->parent = c;
        e->handler = (void *)d; /* cast kept: the handler slot is a void * shared with the char * label */
        page->childCount++;
        g_menuItemCount++;
        g_curMenu->count++;
        d = *(MenuHandler *)((a += 4) - 4);
    } while (d);
    a = 0;
    return b;
}
MenuPage *Menu_AddPage(MenuPage *parent, void *labelOrHandler, u8 hasHandler)
{
    MenuPage *a = &g_menuPages[g_menuItemCount];
    a->kind = MENU_KIND_PAGE;
    a->hasHandler = hasHandler;
    a->disabled = 0;
    a->selected = 0;
    a->handler = labelOrHandler;
    a->parent = Menu_GetItemIndex(parent);
    a->childCount = 0;
    if (parent)
        parent->childCount++;
    g_menuItemCount++;
    g_curMenu->count++;
    return a;
}
/* BYTES(dead-code): b is computed and never used, as in the original */
void Menu_SetItemColor(s16 index)
{
    u32 a;
    MenuPage *b = &g_menuPages[index];
    a = 0x808080;
    if (index == g_menuCursor)
        a = 0x3030c0;
    Text_SetColor(a);
}
void Menu_Init(Menu *menu)
{
    u16 a;
    MenuPage *b;
    g_menuItemCount = 0;
    g_menuCapacity = 0;
    g_menuCursor = 1;
    g_menuLastCursor = 0;
    g_menuCurPage = 0;
    g_menuPrevPage = 0;
    g_menuCapture = 0;
    g_menuCapturePrev = 0;
    g_menuCursorRow = 0;
    g_curMenu = menu;
    for (a = 0; a < menu->count; a++) {
        b = &menu->items[a];
        /* cast kept: the slot holds a handler or a label */
        if (b->hasHandler)
            ((MenuHandler)b->handler)(MENU_MSG_INIT, b);
    }
}
void Menu_ResetToRoot(Menu *menu, s16 cursor)
{
    menu->curPage = 0;
    g_menuCurPage = 0;
    g_menuState.dirty = 1;
    if (cursor != -1)
        g_menuCursor = cursor;
    else {
        menu->cursor = Menu_FindFirstChild(menu->curPage);
        g_menuCursor = menu->cursor;
    }
    g_menuCursorRow = 0;
}
void Menu_SetCurrent(Menu *menu)
{
    g_menuState.dirty = 1;
    if (g_curMenu) {
        g_curMenu->cursor = g_menuCursor;
        g_curMenu->lastCursor = g_menuLastCursor;
        g_curMenu->curPage = g_menuCurPage;
        g_curMenu->prevPage = g_menuPrevPage;
    }
    g_curMenu = menu;
    g_menuPages = menu->items;
    g_menuItemCount = menu->count;
    g_menuCapacity = menu->capacity;
    g_menuCursor = menu->cursor;
    g_menuLastCursor = menu->lastCursor;
    g_menuCurPage = menu->curPage;
    g_menuPrevPage = menu->prevPage;
}
void Menu_PrintPath(MenuPage *page)
{
    if (page->parent != -1)
        Menu_PrintPath(&g_menuPages[page->parent]);
    Text_SetColor(0x803030);
    Text_PrintFmt("%s", page->handler);
}
void Menu_SetCapture(u8 mode, s32 notify)
{
    g_padMenuPrev = g_padMenuCur;
    g_inputMgr.ResetEdges();
    g_menuCapturePrev = g_menuCapture;
    g_menuCapture = mode;
    if (!g_menuCapture && g_menuCapturePrev && notify && g_menuPages[g_menuCursor].hasHandler)
        DISPATCH(g_menuCursor, MENU_MSG_CAPTURE_RELEASE);
}
void Menu_NotifyPageChange()
{
    g_menuState.dirty = 1;
    if (g_menuPages[g_menuPrevPage].hasHandler)
        DISPATCH(g_menuPrevPage, MENU_MSG_UNFOCUS);
    if (g_menuPages[g_menuCurPage].hasHandler)
        DISPATCH(g_menuCurPage, MENU_MSG_FOCUS);
}
void Menu_GoBack()
{
    if (g_menuPages[g_menuCurPage].parent >= 0) {
        g_menuPrevPage = g_menuCurPage;
        g_menuCurPage = g_menuPages[g_menuCurPage].parent;
        g_menuCursor = g_menuPrevPage;
    } else
        Game_ClearFlags(GF_BIT0 | GF_PAUSE_TOGGLE);
    Menu_NotifyPageChange();
}
u32 Menu_Update(s32 layout, u8 align)
{
    u32 a = 0;
    s16 b = 0;
    u8 c;
    u8 d;
    u8 e = 0;
    MenuPage *f;
    s32 g;
    s16 h;
    s16 i;
    d = 0;
    if (g_gameFlags & GF_PAUSED)
        Text_SetFont(FONT_GAME_SMALL);
    else if (layout == 0)
        Text_SetFont(FONT_DEBUG);
    else
        Text_SetFont(FONT_GAME);
    f = &g_menuPages[g_menuCurPage];
    if (!g_menuCapture && g_menuCapturePrev) {
        DISPATCH(g_menuCursor, MENU_MSG_CAPTURE_RELEASE);
        g_menuCapturePrev = 0;
        g_menuState.dirty = 1;
    } else {
        g = g_menuCapture;
        if (g)
            DISPATCH(g_menuCursor, MENU_MSG_CAPTURE_TICK);
        if (g != 2) {
            if (layout == 0) {
                Menu_PrintPath(f);
                Text_NewLine(1);
            } else
                Text_SetColor(0x4bccff);
            c = (g_textWinH - g_textCursorY) / g_pCurFont->lineHeight - 1;
            if (g_menuCursorRow >= c)
                d = g_menuCursorRow - c;
            g_menuPages[g_menuLastCursor].selected = 0;
            g_menuPages[g_menuCursor].selected = 1;
            if (layout == 1 && g_menuPages[g_menuCurPage].childCount < c)
                Text_CenterVertically(g_menuPages[g_menuCurPage].childCount);
            do {
                if (g_menuPages[b].parent == g_menuCurPage) {
                    if (e >= d) {
                        if (g_menuPages[b].hasHandler)
                            DISPATCH(b, MENU_MSG_PREDRAW);
                        if (!g_menuPages[b].disabled) {
                            if (layout == 0)
                                Menu_SetItemColor(b);
                            else
                                Text_SetColor(0x4bccff);
                        } else
                            Text_SetColor(0xa0a0a0);
                        if (g_menuPages[b].hasHandler) {
                            DISPATCH(b, MENU_MSG_DRAW);
                            if (g_menuCursor == b)
                                DISPATCH(g_menuCursor, MENU_MSG_DRAW_SELECTED);
                        } else
                            Text_PrintfStyled(align, g_menuCursor == b && layout != 0, "%s", g_menuPages[b].handler);
                        Text_NewLine(1);
                    }
                    if (g_menuCursor == b)
                        g_menuCursorRow = e;
                    e++;
                }
                b++;
            } while (e < f->childCount);
            Menu_SetDirty(0);
            if (g_menuCursor != g_menuLastCursor) {
                if (g_menuPages[g_menuLastCursor].hasHandler)
                    DISPATCH(g_menuLastCursor, MENU_MSG_UNFOCUS);
                if (g_menuPages[g_menuCursor].hasHandler)
                    DISPATCH(g_menuCursor, MENU_MSG_FOCUS);
                g_menuLastCursor = g_menuCursor;
            }
            if (!g) {
                if (Pad_MenuRepeat((u16)~PAD_UP)) {
                    Ui_PlayMoveSound();
                    h = Menu_FindPrevSibling(g_menuCursor);
                    if (h != g_menuCursor)
                        g_menuCursor = h;
                    else
                        g_menuCursor = Menu_FindLastChild(g_menuPages[g_menuCursor].parent);
                } else if (Pad_MenuRepeat((u16)~PAD_DOWN)) {
                    Ui_PlayMoveSound();
                    h = Menu_FindNextSibling(g_menuCursor);
                    if (h != g_menuCursor)
                        g_menuCursor = h;
                    else
                        g_menuCursor = Menu_FindFirstChild(g_menuPages[g_menuCursor].parent);
                }
                switch (g_menuPages[g_menuCursor].kind) {
                    case MENU_KIND_PAGE:
                        if (Pad_MenuPressed((u16)~PAD_CROSS)) {
                            Ui_PlayConfirmSound();
                            if (!g_menuPages[g_menuCursor].disabled) {
                                if (g_menuPages[g_menuCursor].hasHandler)
                                    DISPATCH(g_menuCursor, MENU_MSG_CONFIRM);
                                g_menuPrevPage = g_menuCurPage;
                                g_menuCurPage = g_menuCursor;
                                g_menuCursor = Menu_FindFirstChild(g_menuCurPage);
                                Menu_NotifyPageChange();
                                a = 1;
                            }
                        }
                        break;
                    case MENU_KIND_ITEM:
                        if (Pad_MenuRepeat((u16)~PAD_LEFT))
                            DISPATCH(g_menuCursor, MENU_MSG_LEFT);
                        else if (Pad_MenuRepeat((u16)~PAD_RIGHT))
                            DISPATCH(g_menuCursor, MENU_MSG_RIGHT);
                        else if (Pad_MenuPressed((u16)~PAD_CROSS) && g_letterboxState != LETTERBOX_OPENING) {
                            Ui_PlayConfirmSound();
                            i = g_menuCursor;
                            DISPATCH(i, MENU_MSG_CONFIRM);
                        }
                        if (Pad_MenuRepeat((u16)~PAD_LEFT) || Pad_MenuRepeat((u16)~PAD_RIGHT))
                            DISPATCH(g_menuCursor, MENU_MSG_VALUE_CHANGED);
                        break;
                }
                if (Pad_MenuPressed((u16)~PAD_TRIANGLE)) {
                    Ui_PlayCancelSound();
                    if (g_menuPages[g_menuCursor].hasHandler)
                        DISPATCH(g_menuCursor, MENU_MSG_CANCEL);
                    Menu_GoBack();
                    a = 1;
                }
            }
        }
    }
    if (g_menuPages[g_menuCursor].disabled) {
        g_menuPages[g_menuCursor].selected = 0;
        g_menuCursor = Menu_FindFirstChild(g_menuCurPage);
    }
    Text_SetColor(0x4bccff);
    return a;
}
void Menu_Close()
{
    if (g_menuCapture) {
        DISPATCH(g_menuCursor, MENU_MSG_CAPTURE_RELEASE);
        g_menuPages[g_menuCursor].selected = 0;
        g_menuCursor = 0;
        g_menuCapture = 0;
    }
    if (g_menuPages[g_menuCursor].hasHandler)
        DISPATCH(g_menuCursor, MENU_MSG_CANCEL);
}
