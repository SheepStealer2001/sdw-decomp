#ifndef SDW_ENGINE_TEXT_H
#define SDW_ENGINE_TEXT_H

/* The functions and globals text.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

struct Font;
struct PadFrame;

extern Font g_fonts[3];                                                               /* 0x6ddab8 */
extern Font *g_pCurFont;                                                              /* 0x57bd9c */
extern char g_textScratchBuffer[0x3f0];                                               /* 0x6ddb10 */
void Font_CloneResized(u8 srcFontId, u8 dstFontId, u8 advancePlus1, u8 heightMinus1); /* 0x535c31 */
s8 Font_LoadFromRes(u8 fontId, u16 resType, u8 cellGeometry);                         /* 0x535c83 */
u32 Font_SetCellSize(u8, u8);
void Hud_EndBox_stub();                                  /* 0x535eef */
s32 ScrollText_Run(u8 mode, char *text, s32 allowInput); /* 0x5328b3 (src/engine/text.cpp) */
u16 Str_ParseU16(const char *s);                         /* 0x533d68 */
void Text_ApplyWindow(u32 *layer);                       /* 0x53378a */
void Text_CenterVertically(s32 lines);                   /* 0x533684 */
void Text_Disable();                                     /* 0x5322ec */
void Text_DrawNoWrap(const char *text, u8 align);
void Text_DrawString(u8 c, s32 x, s32 y, float z, u32 rgb);
void Text_EmitLineThunk(const char *text, s32 n, u8 align);
s32 Text_ExpandButtonToken(const char *tok, char *out, u16 *len);  /* 0x533fbd */
s32 Text_ExpandColorToken(const char *tok, char *out, u16 *len);   /* 0x53447c */
s32 Text_ExpandMemCardToken(const char *tok, char *out, u16 *len); /* 0x534cd0 */
s32 Text_ExpandNameToken(const char *tok, char *out, u16 *len);    /* 0x5343da */
s32 Text_FindNextPageMark(char **start, char **cur);
char *Text_FindPageMark(char **p);
s32 Text_FindPrevPageMark(char **start, char **cur);
s32 Text_GetScrollInput(PadFrame *frame);
s32 Text_LineIsBlank(char **p);
u16 Text_MaxLineLength(const char *s); /* 0x533589 */
u16 Text_MeasureLine(const char *s);   /* 0x533403 */
void Text_NewLine(s32 lines);          /* 0x533659 */
char *Text_PageStep(s32 mode, char **text, char **next, char **prev);
void Text_PrintFmt(const char *fmt, ...);                          /* 0x534655 */
void Text_Printf(u8 align, const char *fmt, ...);                  /* 0x534699 */
void Text_PrintfStyled(u8 align, s32 blink, const char *fmt, ...); /* 0x5348aa */
void Text_PutColorCode(char *out, u32 rgb);
void Text_ResetMeasure();          /* 0x5322fb */
u8 Text_ResetWindow();             /* 0x5322b3 */
void Text_ScrambleGlyphs(char *s); /* 0x533dda */
void Text_SetColor(u32 rgb);       /* 0x5336b8 */
void Text_SetCursor(s16 x, s16 y); /* 0x53363f */
void Text_SetFont(u8 fontId);      /* 0x5336dc */
void Text_SetNoClipOnce();
void Text_SetWindow(u32 *layer, s32 x, s32 y, s32 w, s32 h, u32 unused); /* 0x535dde */
void Text_SetWindowRect(u32 *layer, const s16 *rect, u32 noClip);        /* 0x533836 */
char *Text_Sprintf(char *out, const char *fmt, ...);                     /* 0x53492c */
void Text_WordWrap(char *text, u8 mode);                                 /* 0x5346ff */

#endif
