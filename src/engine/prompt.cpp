/* match-init: Ui_InitRectDialogBody_thunk Ui_InitRectChoiceLeft_thunk Ui_InitRectChoiceRight_thunk Ui_InitRectChoiceWide_thunk Ui_InitRectFooter_thunk */
/* T277 - original object "Prompt.cpp" (guessed name): the prompt / dialog part of the HUD layer, 0x53ea50-0x53f423
 * (Prompt_*, Dialog_*, the layout rects and their static initialisers, the memory-card prompts and the three UI blips).
 *   .text  0x53ea50-0x53f423  (its static initialisers sit mid-file, after Dialog_UpdateAnswer, where the rects are
 *                             defined)
 *   .data  0x57c05c-0x57c07c  g_rectDialogBody, g_rectChoiceLeft, g_rectChoiceWide, g_rectFooter (their constant
 *                             members; the rest is written by the static initialisers)
 *   .bss   0x6de330-0x6de368  g_uiSoundHandle, the global prompt's state, then g_rectChoiceRight (all dynamic)
 * .bss order: VC6 puts a file's globals WITHOUT an initialiser (and objects with constructors) first, ordered by a
 * hash of their names, and after them the globals WITH an initialiser (explicit zero, or dynamic like
 * g_rectChoiceRight) in definition order. g_rectChoiceRight is dynamically initialised, so it is in the second group;
 * the prompt globals are therefore written `= 0` here, which puts all eight in definition order.
 * The declarations are shared with T275 (interface.cpp) and T276 (fade.cpp); UiQuad / UiIcon methods are declared by
 * the names T275 defines them under (UiQuad_Draw, UiIcon_Setup ...), and Ui_DrawMemCardBackdrop's calls by the short
 * names (Draw, Setup ...) are mapped to those by #defines around it.
 */
/* BYTES: cast, dead-code, inline, layout, view. */
/* BYTES(layout): written '= 0' only to keep definition order in .bss (g_rectChoiceRight is dynamic, so it sorts with them) */
/* BYTES(inline): Cine::IsActive, UiQuad::SetColor, UiIcon::SetColor (member-macro inlines): source-only inline: see fade.cpp (value / this / &mainQuad in a stack temp) */
/* BYTES(cast): ARGB4444 macro: the redundant & 0xffff is the original's (and ecx,0xffff at 0x53d62c) */
/*
 * The screen-space HUD layer, SheepD3D.exe 0x53d4bb-0x53f42f, in three objects (T275 interface.cpp, T276 fade.cpp, T277
 * prompt.cpp): the cinematic letterbox bars, the fade curtain and the three fade entry points, the telescope and
 * cannon-sight masks, the flat / gradient / outline rectangle helpers, the global yes/no prompt, the NPC question box
 * (DialogBox), the memory-card prompts and the three UI blips.
 *
 * Everything here is drawn in the 512x240 virtual HUD space (the PlayStation's screen) and scaled by Screen::ScaleX/Y.
 * The fade and letterbox curtains are a 4x4 ARGB4444 texture (a PolyBatcher texture page reserved for it, locked and
 * refilled every frame) stretched over the screen.
 * The names are descriptive, not recovered (the binary has no symbols). Local names were picked for the /Od frame order
 * (tools/vc6_locals.py): they are plausible, not recovered.
 */
#include "sdw_enums.h"
#include "../sdk/ddraw.h"
#define SDW_MEMBERS_Texture void Surface_LockForWrite(DDSURFACEDESC2 *desc); /* 0x40ae3f */


#include "sdw_enums.h"
#include "scenaric_props.h"
#include "sdw_classes.h"
#define SDW_INLINE_CINE_ISACTIVE 1
#include "cine_inlines.h"
#undef SDW_INLINE_CINE_ISACTIVE
#define SDW_INLINE_UIQUAD_SETCOLOR_U32 1
#include "ui_quad_inlines.h"
#undef SDW_INLINE_UIQUAD_SETCOLOR_U32
#define SDW_INLINE_UIICON_SETCOLOR_U32 1
#include "ui_icon_inlines.h"
#undef SDW_INLINE_UIICON_SETCOLOR_U32

/* The DAV directory (DavHeader.dir): packed, its pointers sit at +6/+0xa/+0x12, which the struct generator cannot lay
 * out, so it is declared here. */
#pragma pack(push, 1)
struct DavDirectory {
    u16 indexCount;
    u16 bitmapCount;
    u16 unk04;
    u16 *indices;
    DavBitmapRec *bitmaps; /* +0x0a 10-byte records */
    u32 fileSize;
    u32 *idLists;
};
#pragma pack(pop)

/* ---- callees ---- */
typedef void (*MenuHandler)(u8 msg, MenuPage *self);
#include "fixed_math.h"
#include "interface.h"
#include "draw2d.h"
#include "scn_tools.h"
#include "scenaric_loop.h"
#include "text.h"
#include "input.h"
#include "game_state.h"
#include "screen.h"
#include "cine.h"
#include "scenaric.h"
#include "time.h"
u32 Rgb24_Lerp(u32 a, u32 b, s16 t); /* 0x52795c */
void Draw2D_TexRect(float z, float x0, float y0, float x1, float y1, s32 texIndex, float uTL, float vTL, u32 cTL,
                    float uBL, float vBL, u32 cBL, float uTR, float vTR, u32 cTR, float uBR, float vBR,
                    u32 cBR);                                                            /* 0x5242df */
void Dialogue_SetBoxActive(s32 active);                                                  /* 0x539479 */
u8 Dialogue_Say(const char *text, s32 voiceId, ScnObject *speaker, u32 arg);             /* 0x539893 */
s32 Rand_Bounded(s32 bound);                                                             /* 0x561219 */
u16 Text_CountWrappedLines(const char *s);                                               /* 0x53353f */
void Menu_BuildList(MenuPage *pages, Menu *menu, s8 count, const MenuHandler *handlers); /* 0x539199 */
void Ui_DrawTextBox(TextBox *box, u16 lineCount);                                        /* 0x53d243 */
void Ui_DrawMenuBox(MenuBox *box);                                                       /* 0x53d385 */
u32 *Res_GetValidatedIdList(u16 resId, u16 *outCount);                                   /* 0x548381 */
u16 Sound_Play(u16 soundId, void *owner, u16 volume, u8 flags, s32 rate);                /* 0x5491b8 */

/* ---- globals ---- */
extern u32 *g_screenLayerBase;       /* 0x585044 */
extern Wolf *g_pWolf;                /* 0x6cf310 */
extern u32 g_gameFlags;              /* 0x6ddf74 */
extern s32 g_dt;                     /* 0x71b300 */
extern s16 g_dtRawMs;                /* 0x71b2d8 */
extern s32 g_gameTime;               /* 0x71b2d0 */
extern s32 g_dialogueCurText;        /* 0x6ddfb0 */
extern u16 *g_resTelescopeMaskOuter; /* 0x6de0e0 */
extern u16 *g_resTelescopeMaskInner; /* 0x6de0e4 */
extern u16 *g_resCannonMask;         /* 0x6de144 */

/* inline: the constant mask is substituted but its `~` is left to run time (mov edx,0x10; not edx at 0x53d80e). */
/* BYTES(inline): source-only inline: the constant is substituted but its ~ stays code (mov edx,0x10; not edx) */
#define SDW_INLINE_FREE_GAME_CLEARFLAGS_U32 1
#include "game_state_inlines.h"
#undef SDW_INLINE_FREE_GAME_CLEARFLAGS_U32
/* inline: the virtual screen size (Screen::virtWidth / virtHeight are u16). As inlines they are not constant
 * expressions to VC6: the rects below get run-time initialisers (0x53f04f), and `ScreenWidthU16() - x` loads the
 * constant first (0x53eb0e), which the literal 512 does not. */
/* BYTES(inline): source-only inlines, not literals: run-time rect initialisers (0x53f04f) and the constant loaded first (0x53eb0e) */
#define SDW_INLINE_FREE_SCREENWIDTHU16 1
#include "screen_inlines.h"
#undef SDW_INLINE_FREE_SCREENWIDTHU16
#define SDW_INLINE_FREE_SCREENHEIGHTU16 1
#include "screen_inlines.h"
#undef SDW_INLINE_FREE_SCREENHEIGHTU16

/* An ARGB8888 colour as ARGB4444. The last term's redundant 16-bit mask is the original's (and ecx,0xffff at 0x53d62c). */
#define ARGB4444(c) ((((c) >> 4) & 0xf) | (((c) >> 8) & 0xf0) | (((c) >> 12) & 0xf00) | (((c) >> 16) & 0xf000 & 0xffff))

/* ---- globals defined here (.bss, all `= 0`: see the header) ---- */
u16 g_uiSoundHandle = 0;       /* 0x6de330  the last UI blip's voice */
s32 g_promptActive = 0;        /* 0x6de334 */
TextBox g_promptBox = {0};     /* 0x6de338 */
u32 g_promptMaxLineWidth = 0;  /* 0x6de350 */
u32 g_promptLineCount = 0;     /* 0x6de354 */
s32 g_promptResult = 0;        /* 0x6de358 */
ScnObject *g_promptSender = 0; /* 0x6de35c */

/* 0x53ea50 - open the global prompt box (freezing the Wolf with message 0x0e), sized to its text and centred. */
void Prompt_Begin(char *text, ScnObject *sender)
{
    g_promptSender = sender;
    if (g_pWolf)
        g_pWolf->HandleMessage(sender, MSG_FREEZE, 0);
    g_promptBox.text = text;
    Text_SetFont(FONT_GAME);
    g_promptMaxLineWidth = Text_MaxLineLength(g_promptBox.text);
    g_promptLineCount = Text_CountWrappedLines(g_promptBox.text);
    g_promptActive = 1;
    g_promptResult = PROMPT_PENDING;
    g_promptBox.rect[2] = (g_promptMaxLineWidth + 1) * g_pCurFont->glyphWidth;
    g_promptBox.rect[3] = (g_promptLineCount + 3) * g_pCurFont->lineHeight + 4;
    g_promptBox.rect[0] = (ScreenWidthU16() - g_promptBox.rect[2]) / 2;
    g_promptBox.rect[1] = (ScreenHeightU16() - g_promptBox.rect[3]) / 2;
    g_promptBox.bgColor = 0;
}

/* 0x53eb4c - close the global prompt and unfreeze the Wolf (message 0x0f). */
void Prompt_End()
{
    if (g_promptActive) {
        if (g_pWolf)
            g_pWolf->HandleMessage(g_promptSender, MSG_UNFREEZE, 0);
        g_promptActive = 0;
    }
}

/* 0x53eb88 - draw the global prompt; CROSS with the bars fully out answers 1 or 2. -1 while pending. */
s32 Prompt_Update()
{
    if (!g_promptActive)
        return g_promptResult;
    Ui_DrawTextBox(&g_promptBox, (u16)g_promptLineCount);
    if (g_promptBox.fits && Pad_MenuPressed((u16)~PAD_CROSS) && g_letterboxState == LETTERBOX_OPEN) {
        if (g_promptBox.confirmChoice == PROMPT_CHOICE_1)
            g_promptResult = PROMPT_CHOICE_1;
        else
            g_promptResult = PROMPT_CHOICE_2;
    }
    return g_promptResult;
}

/* 0x53ec02 - open an NPC question box: the question is class string firstStringId, answer i and its reply are strings
 * firstStringId + 1 + 2i and + 2 + 2i; the box is sized to the widest line and centred. Freezes the Wolf. */
void Dialog_Begin(DialogBox *dlg, u32 firstStringId, s8 answerCount, const MenuHandler *handlers, ScnObject *sender)
{
    s32 curI;
    s32 theBase;
    u16 maxLen;
    dlg->active = 1;
    if (g_pWolf)
        g_pWolf->HandleMessage(sender, MSG_FREEZE, 0);
    dlg->inputLatch = 1;
    dlg->sender = sender;
    dlg->questionText = sender->Text_GetClassString((u8)firstStringId);
    dlg->drawBox.text = dlg->questionText;
    theBase = firstStringId + 1;
    for (curI = 0; curI < answerCount; curI++) {
        dlg->answerText[curI] = sender->Text_GetClassString((u8)(theBase + curI * 2));
        dlg->replyText[curI] = sender->Text_GetClassString((u8)(theBase + curI * 2 + 1));
    }
    Menu_BuildList(dlg->listA, &dlg->listB, answerCount, handlers);
    Text_SetWindow(g_screenLayerBase + 6, 0, 0, 512, 240, 0);
    Text_SetFont(FONT_GAME);
    maxLen = Text_MaxLineLength(dlg->drawBox.text);
    for (curI = 0; curI < answerCount; curI++)
        maxLen =
            maxLen < Text_MaxLineLength(dlg->answerText[curI]) ? Text_MaxLineLength(dlg->answerText[curI]) : maxLen;
    dlg->drawBox.rect[2] = maxLen * g_pCurFont->glyphWidth > 512 ? 512 : maxLen * g_pCurFont->glyphWidth;
    dlg->drawBox.rect[3] = 0;
    dlg->drawBox.rect[1] = 0;
    dlg->drawBox.rect[0] = 0;
    Text_SetWindowRect(g_screenLayerBase + 6, dlg->drawBox.rect, 0);
    dlg->drawBox.rect[3] =
        (Text_CountWrappedLines(dlg->drawBox.text) + dlg->listB.count + 2) * g_pCurFont->lineHeight > 240
            ? 240
            : (Text_CountWrappedLines(dlg->drawBox.text) + dlg->listB.count + 2) * g_pCurFont->lineHeight;
    dlg->drawBox.rect[0] = (ScreenWidthU16() - dlg->drawBox.rect[2]) / 2;
    dlg->drawBox.rect[1] = (ScreenHeightU16() - dlg->drawBox.rect[3]) / 2;
    dlg->drawBox.bgColor = 0;
    dlg->drawBox.pages = dlg->listA;
    dlg->drawBox.list = &dlg->listB;
}

/* 0x53eeda */
void Dialog_Close(DialogBox *dlg)
{
    dlg->active = 0;
}

/* 0x53eee8 - one frame of an open question box: TRIANGLE closes it (1); CROSS picks the highlighted answer (2, once per
 * press); else 0. */
u8 Dialog_Update(DialogBox *dlg)
{
    Dialogue_SetBoxActive(1);
    Ui_DrawMenuBox(&dlg->drawBox);
    if (dlg->drawBox.fits) {
        if (Pad_MenuPressed((u16)~PAD_TRIANGLE)) {
            Dialogue_SetBoxActive(0);
            if (g_pWolf)
                g_pWolf->HandleMessage(dlg->sender, MSG_UNFREEZE, 0);
            Dialog_Close(dlg);
            return DIALOG_CLOSED;
        }
        if (!dlg->inputLatch && Pad_MenuPressed((u16)~PAD_CROSS)) {
            dlg->selected = dlg->drawBox.cursorRow;
            dlg->inputLatch = 1;
            return DIALOG_PICKED;
        }
    }
    dlg->inputLatch = 0;
    return DIALOG_NONE;
}

/* 0x53efaa - play the chosen answer's reply (with its voice from answerVoices, if given); when it has finished, close
 * the box and unfreeze the Wolf (0). 1 while it is still being said. */
u32 Dialog_UpdateAnswer(DialogBox *dlg, const u32 *answerVoices)
{
    s32 voice;
    voice = 0;
    if (answerVoices)
        voice = answerVoices[dlg->selected];
    if (!Dialogue_Say(dlg->replyText[dlg->selected], voice, 0, 1)) {
        Dialogue_SetBoxActive(0);
        if (g_pWolf)
            g_pWolf->HandleMessage(dlg->sender, MSG_UNFREEZE, 0);
        Dialog_Close(dlg);
        return 0;
    }
    dlg->inputLatch = 0;
    return 1;
}

/* 0x53f042..0x53f12b - the prompt layout rects {x, y, w, h}. Their widths are written from the screen width by static
 * initialisers (ScreenWidthU16() is not a compile-time constant to VC6), so every member after the first computed one is
 * set at run time as well. */
s16 g_rectDialogBody[4] = {0x30, 0x10, ScreenWidthU16() - 0x60, 0x8c};       /* 0x57c05c */
s16 g_rectChoiceLeft[4] = {0x20, 0xa0, (ScreenWidthU16() - 0x40) / 2, 0x28}; /* 0x57c064 */
s16 g_rectChoiceRight[4] = {(s16)((ScreenWidthU16() - 0x40) / 2) + 0x20, 0xa0, (ScreenWidthU16() - 0x40) / 2,
                            0x28};                                     /* 0x6de360 */
s16 g_rectChoiceWide[4] = {0x20, 0xa0, ScreenWidthU16() - 0x60, 0x28}; /* 0x57c06c */
s16 g_rectFooter[4] = {0x30, 0xcc, ScreenWidthU16() - 0x60, 0x14};     /* 0x57c074 */

/* Ui_DrawMemCardBackdrop calls UiIcon's methods by their short names (Setup, SetFadeLevel, Draw); they are the
 * functions T275 (interface.cpp) defines as UiIcon_Setup / UiIcon_SetFadeLevel / UiIcon_Draw, so the calls are mapped
 * to those names around this one function. */
#define Setup UiIcon_Setup
#define SetFadeLevel UiIcon_SetFadeLevel
#define Draw UiIcon_Draw
/* 0x53f12c - the memory-card screens' backdrop bitmap, full width at 2x, faded to level. The frame holds a dword the
 * code never touches between the id-list count and the icon (0x53f12f sub esp,0x5c); `spare` stands for it. */
/* BYTES(dead-code): spare is never used: it stands for the dword the original frame never touches (sub esp,0x5c at 0x53f12f) */
/* BYTES(view): the #defines map the calls to the names T275 defines; the body is unchanged */
void Ui_DrawMemCardBackdrop(u8 level)
{
    UiIcon icon;
    s32 spare;
    u16 count;
    void **list;
    u16 *kBmp;
    /* cast kept (both): an export id list holds record pointers of any kind; this one lists bitmaps */
    list = (void **)Res_GetValidatedIdList(DAV_IDI_IMEMCMAP, &count);
    if (list) {
        kBmp = (u16 *)*list;
        icon.Setup(kBmp, 0, 0, 0, 0, 0x800, 0x400, 0);
        icon.mainQuad.h = 0xf0;
        icon.SetColor(0x808080);
        icon.SetFadeLevel(level);
        icon.Draw(3);
    }
}
#undef Setup
#undef SetFadeLevel
#undef Draw

/* 0x53f1ab - the scroll arrow, drawn at (x, y) at double height; flipMode turns it into the down arrow. */
void Ui_DrawScrollArrow(s16 x, s16 y, u16 flipMode)
{
    Sprite arrow;
    if (arrow.LoadFromRes(DAV_IDI_IGLFLEC_))
        arrow.Draw(g_screenLayerBase + 2, x, y, x + arrow.widthMinus1, y + arrow.height * 2, 0x808080, flipMode);
}

/* 0x53f200 - text clipped to rect, centred vertically on its wrapped line count. */
void Ui_DrawTextInRect(s16 *rect, u8 align, u32 style, char *text)
{
    Text_SetWindowRect(g_screenLayerBase + 2, rect, 1);
    Text_CenterVertically(Text_CountWrappedLines(text));
    Text_PrintfStyled(align, style, text);
    Hud_EndBox_stub();
}

void Ui_PlayMoveSound();
void Ui_PlayCancelSound();
void Ui_PlayConfirmSound();

/* 0x53f24e - title and one highlighted line; CROSS answers 0x10, TRIANGLE 0x80, else 0. */
u8 Ui_PromptConfirm(char *title, char *prompt)
{
    Text_SetColor(0x4bccff);
    Ui_DrawTextInRect(g_rectDialogBody, TEXTALIGN_CENTER, 0, title);
    Ui_DrawTextInRect(g_rectChoiceWide, TEXTALIGN_CENTER, 1, prompt);
    if (Pad_MenuPressed((u16)~PAD_CROSS)) {
        Ui_PlayConfirmSound();
        return MCARD_CUR_LEFT;
    }
    if (Pad_MenuPressed((u16)~PAD_TRIANGLE)) {
        Ui_PlayCancelSound();
        return MCARD_IN_CANCEL;
    }
    return MCARD_CUR_NONE;
}

/* 0x53f2c7 - title and two options side by side; LEFT / RIGHT toggles *choice between 0x10 and 0x20, CROSS answers
 * *choice, TRIANGLE 0x80, else 0. */
s8 Ui_PromptYesNo(char *title, char *optLeft, char *optRight, u8 *choice)
{
    Text_SetColor(0x4bccff);
    Ui_DrawTextInRect(g_rectDialogBody, TEXTALIGN_CENTER, 0, title);
    Ui_DrawTextInRect(g_rectChoiceLeft, TEXTALIGN_CENTER, *choice == MCARD_CUR_LEFT, optLeft);
    Ui_DrawTextInRect(g_rectChoiceRight, TEXTALIGN_CENTER, *choice == MCARD_CUR_RIGHT, optRight);
    if (Pad_MenuRepeat((u16)~PAD_LEFT) || Pad_MenuRepeat((u16)~PAD_RIGHT)) {
        if (*choice == MCARD_CUR_LEFT)
            *choice = MCARD_CUR_RIGHT;
        else
            *choice = MCARD_CUR_LEFT;
        Ui_PlayMoveSound();
    }
    if (Pad_MenuPressed((u16)~PAD_CROSS)) {
        Ui_PlayConfirmSound();
        return *choice;
    }
    if (Pad_MenuPressed((u16)~PAD_TRIANGLE)) {
        Ui_PlayCancelSound();
        return MCARD_IN_CANCEL;
    }
    return MCARD_CUR_NONE;
}

/* 0x53f3ba - the cursor-move blip. */
void Ui_PlayMoveSound()
{
    g_uiSoundHandle = Sound_Play(SND_SGLCHGLI, 0, 0xff, 0, 0x1000);
}

/* 0x53f3dd - the cancel / back sound. */
void Ui_PlayCancelSound()
{
    g_uiSoundHandle = Sound_Play(SND_SMOJPMOV, 0, 0xff, 0, 0x1000);
}

/* 0x53f400 - the confirm sound. */
void Ui_PlayConfirmSound()
{
    g_uiSoundHandle = Sound_Play(SND_SBONMBTN, 0, 0xff, 0, 0x1000);
}
