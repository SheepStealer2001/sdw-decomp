#ifndef SDW_ENGINE_INTERFACE_H
#define SDW_ENGINE_INTERFACE_H

/* The functions and globals interface.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

class ScnObject;
struct UiFrame;

extern s16 g_cannonOriginX;                             /* 0x6de100 */
extern s16 g_cannonOriginY;                             /* 0x6de102 */
extern s16 g_cannonTileH;                               /* 0x6de128 */
extern s16 g_cannonTileW;                               /* 0x6de12a */
extern s16 g_letterboxTimer;                            /* 0x6de32c ms, 0..1500 */
extern char *g_strCancel;                               /* 0x6de12c */
extern char *g_strUiString1C;                           /* 0x6de148 */
extern char *g_strUiString1E;                           /* 0x6de178 */
extern char *g_strUiString21;                           /* 0x6de138 */
extern char *g_strValidate;                             /* 0x6de0dc */
extern s16 g_telescopeOriginX;                          /* 0x6de140 */
extern u16 g_telescopeOriginY;                          /* 0x6de142 */
extern s16 g_telescopeTileH;                            /* 0x6de13e */
extern s16 g_telescopeTileW;                            /* 0x6de13c */
extern char *g_uiFooterText;                            /* 0x6de0d8 */
extern ScnObject *g_voiceOwner_2;                       /* 0x6de324 */
extern u32 g_voicePending;                              /* 0x6de31c */
extern u32 g_voicePlaying;                              /* 0x6de320 */
u8 Dialogue_Show(void *text, s32 arg);                  /* 0x5395c7 */
void Dialogue_StopVoice();                              /* 0x5394d7 */
void Fade_DrawOverlay(bool white, u8 level, s16 *rect); /* 0x53d8b5, T275 */
void Hud_DrawCannonMask();
void Hud_DrawTelescopeMask(bool);
void Interface_Init();                                                       /* 0x53c38b */
void Letterbox_Update();                                                     /* 0x53d4bb */
void Menu_BuildConfirmMenu();                                                /* 0x5390e9 */
void **Res_FindBitmapGroup(void *bmpRecord, u16 *outCount);                  /* 0x53b47b */
void StringBank_RandomiseGlyphs();                                           /* 0x539273 */
char *Text_GetUiString(u8 index);                                            /* 0x5393f4 */
void Ui_BuildFrameQuads(UiFrame *out, s16 *rect, u16 frameResId, u16 inset); /* 0x53c782 */
void Ui_DrawFlatRect(u32 *, s32, s32, s32, s32, u32);
void Ui_DrawFrameQuads(UiFrame *frame, s32 unusedArg);                                               /* 0x53d0a8 */
void Ui_DrawGouraudRect(u32 *layer, s32 x0, s32 y0, s32 x1, s32 y1, u32 c0, u32 c1, u32 c2, u32 c3); /* 0x53e375 */
void Ui_DrawPanelFill(u32 colorRGB, s16 *rect);                                                      /* 0x53c537 */
void Ui_DrawRectOutline(s16 *rect, u32 colorRGB);
void Ui_DrawSubtitleBox(const char *text, const s16 *rect, u16 frameStyle);

/* The functions and globals interface.cpp defines, declared once for every file that uses them. */

class AnimSprite;
struct DialogueShownFlags;
class Sprite;
class UiQuad;

extern AnimSprite g_animSpriteCrayon1;          /* 0x6de220 */
extern UiQuad g_cannonMaskQuads[4];             /* 0x6de180 */
extern DialogueShownFlags g_dialogueShownFlags; /* 0x6ddfb5 bit 0: Dialogue_Show printed the text this frame */
extern char g_menuFooterText[];                 /* 0x6de058 */
extern Sprite g_spriteCrayon2;                  /* 0x6de0f0 */
extern UiQuad g_telescopeMaskQuads[8];          /* 0x6ddfb8 */

#endif
