#ifndef SDW_ENGINE_PROMPT_H
#define SDW_ENGINE_PROMPT_H

/* The functions and globals prompt.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

struct DialogBox;
class ScnObject;

extern s32 g_promptActive;                                        /* 0x6de334 */
extern u32 g_promptLineCount;                                     /* 0x6de354 */
extern u32 g_promptMaxLineWidth;                                  /* 0x6de350 */
extern s32 g_promptResult;                                        /* 0x6de358 */
extern ScnObject *g_promptSender;                                 /* 0x6de35c */
extern u16 g_uiSoundHandle;                                       /* 0x6de330 */
u8 Dialog_Update(DialogBox *dlg);                                 /* 0x53eee8 */
u32 Dialog_UpdateAnswer(DialogBox *dlg, const u32 *answerVoices); /* 0x53efaa */
void Prompt_End();                                                /* 0x53eb4c */
void Ui_DrawMemCardBackdrop(u8);
void Ui_DrawScrollArrow(s16, s16, u16);
void Ui_DrawTextInRect(s16 *, u8, u32, char *);
void Ui_PlayCancelSound();  /* 0x53f3dd */
void Ui_PlayConfirmSound(); /* 0x53f400 */
void Ui_PlayMoveSound();    /* 0x53f3ba */
u8 Ui_PromptConfirm(char *, char *);
s8 Ui_PromptYesNo(char *, char *, char *, u8 *);

/* The functions and globals prompt.cpp defines, declared once for every file that uses them. */

struct TextBox;

extern TextBox g_promptBox; /* 0x6de338 */

#endif
