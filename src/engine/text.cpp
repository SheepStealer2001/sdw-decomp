/* match-init: Text_StaticInit RenderPoly_StaticInit_6ddf30 */
/* match-addr: g_hexDigits=0x57bcd0 g_pTextScratch=0x57bce0 g_textScratchBuf=0x6ddb10 g_textDefWinX=0x6ddf24 g_textDefWinY=0x6ddf26 g_textDefWinW=0x6ddf28 g_textDefWinH=0x6ddf2a g_textGlyphPoly=0x6ddf30 div=0x568549 */
/*
 * T270 - original object "Text.cpp" (guessed name): the text engine as ONE translation unit, 0x5321d0-0x5362d0 (the
 * File_* layer before it is T269 file.cpp; the GameState and debug-stub objects after it are T271 game_state.cpp and
 * T272 debug.cpp).
 *   .text  0x5321d0-0x5362d1  the two static initialisers (g_textFormatBuf, g_textGlyphPoly), then the functions in
 *                             address order
 *   .data  0x57bcd0-0x57c008  g_hexDigits, g_pTextScratch, the $B_...$ button tokens (alphabetical), g_pCurFont, the
 *                             two glyph-variant tables, the G_NAME / M_CARD / colour tokens, and an unreferenced
 *                             second copy of the colour tokens
 *   .bss   0x6ddab8-0x6ddf5c  g_fonts .. g_textBlinkPhase
 *
 * Notes on the source:
 *  - The 34 `Str_IsPrefixOf(tok, "...")` token tests of Text_ExpandButtonToken / NameToken / ColorToken /
 *    MemCardToken name the static char arrays s_tok<TOKEN>. The exe shows they were named data: VC6 /Od emits all of a
 *    file's named initialised data before any $SG literal, and the literals in the order the code meets them, but here
 *    the button tokens lie in ALPHABETICAL order and BEFORE g_pCurFont (an initialised pointer), and M_CARD lies before
 *    the colours although its only user comes after theirs. The code bytes are the same either way (push OFFSET ...).
 *  - The glyph-variant tables start at 0x57bda0 / 0x57be80 (224 entries, characters 0x20..0xff); the code indexes them
 *    as g_glyphVariantFwd[ch] with the symbol at 0x57bd80 = table - 0x20, a #define of that address.
 *  - .bss: VC6 orders a file's uninitialised globals by a hash of their NAMES. The window block 0x6ddaf4 and the
 *    scrolling-text block 0x6ddf00 are defined as two structs, g_textPort and g_textScroll (TextPort / TextScroll,
 *    src/include/sdw_global_views.h), the default window as g_textDefRect and the scratch buffer as
 *    g_textScratchBuffer, with the descriptive names (g_textWinX/Y/H, g_textCursorX/Y, g_textClipOffX,
 *    g_scrollTextBegin/Source/Flags ...) #defined as their members; every other object that uses those names declares
 *    them the same way. Details and the hash keys: the note above the TextRect type below.
 *
 * Scrolling text. A dialogue string is split into pages by markers "$<digits>$" (the digits, or "(digits)", give the
 * page's display time in tenths of a second: Str_ParseU16 * 100 ms). Tokens "$B_...$", "$C_...$", "$G_...$" expand to
 * button glyphs, colour codes and names. ScrollText_Run shows one page per call from g_scrollTextPage, counts its time
 * down by g_dtRawMs and moves to the next page when it runs out; once the player takes over with the action button
 * (only while the letterbox is up, GF_LETTERBOX 0x1000) the timers are parked at INT_MAX and the stick / d-pad pages.
 *
 * The text engine proper: font cell geometry, the text window and cursor, the engine's own number formatters and
 * printf (%s %d %u %x %% %$ plus the $TOKEN$ escapes for button glyphs, colours, the player's name and the memory
 * card), the word-wrapping and non-wrapping line emitters, the glyph quad drawer and the font loader.
 * Text lives in the 512x240 virtual HUD screen (Screen_ScaleX/Y convert). Strings may carry the inline colour code
 * {0x01, r, g, b}: the emitters skip its payload and switch g_pCurFont->color to it.
 * The match-addr line pins the data objects the symbol tables do not place (the hex digit table 0x57bcd0, the $TOKEN$
 * scratch pointer 0x57bce0 (initialised to 0x6ddb10; g_textFormatBuf = it + 0x10), the buffers at 0x6ddb10, the default
 * window 0x6ddf24 and the RenderPoly 0x6ddf30); div is the CRT's div() (0x568549).
 * Devices that pin the code generation (not claims about the source text):
 *  - local names are chosen for their stack slots (tools/vc6_locals.py);
 *  - PolyBatcher::Submit is the batcher's add-a-triangle body (as PolyBatcher_SubmitPoly 0x415c80, but calling
 *    Render_SetStateFlags out of line), __forceinline because VC6 declines to expand it as a plain inline here; its
 *    second expansion in Text_DrawString calls the out-of-line Render_SetTexture / Render_DrawPrimitive where the
 *    first expands them, so it is written twice (SubmitCallingTexFlush), and the expanded D3DApp wrappers are inline
 *    twins (DrawPrimitiveInline, SetTextureInline) whose names are not recovered;
 *  - `1.0f / (g_pViewFrustum->nearZ)` keeps its parentheses, `x + (g_textWinX + g_textClipOffX)` its grouping, and
 *    Text_WordWrap / Text_DrawNoWrap their statement shapes, for the original's operand order.
 * A shape that reproduces the bytes is a representation, not proof that the original source read this way.
 */
/* BYTES: bss-name, cast, dead-code, flow, inline, layout, slot-name, view. */
/* BYTES(bss-name): aggregate names chosen to be readable and to hash in the exe's .bss order (keys 29 < 120 < 217 < 235 < 258 < 315 < 687); the old names are member #defines */
/* BYTES(layout): initialised on purpose: they follow the hashed group in definition order */
/* BYTES(view): g_glyphVariantFwd / Rev #defines: a #define of table - 0x20: the code indexes the 224-entry tables with the character itself */
/* BYTES(view): PROGRESS_GATE() / SCROLL_FLAGS macros: bitfield views: the original reads these words with VC6's bitfield code */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "../sdk/d3d7.h"
class Mat44;
#include "../sdk/crt.h"

/* Screen's scale methods under the names T262 (screen.cpp) defines them by (ScaleX / ScaleY); the spelling
 * Screen_ScaleX / Screen_ScaleY used below is mapped to them after the class header. */
#define SDW_MEMBERS_RenderPoly                                              \
    RenderPoly();                            /* 0x41aad0 RenderPoly_Ctor */ \
    /* virtual ~RenderPoly() is generated */ /* 0x41ac04 RenderPoly_Dtor */
#define SDW_MEMBERS_D3DApp                                                               \
    void Render_SetStateFlags(u32 flags);                                 /* 0x4155f0 */ \
    void Render_ClearStateFlags(u32 flags);                               /* 0x4159b0 */ \
    void Render_DrawPrimitive(u32 type, u32 fvf, void *verts, u32 count); /* 0x415c00 */ \
    void Render_SetTexture(Texture *tex, u32 stage);                      /* 0x415c40 */ \
    /* the inline twins of the two above (defined below) */                              \
    inline void DrawPrimitiveInline(u32 type, u32 fvf, void *verts, u32 count);          \
    inline void SetTextureInline(Texture *tex, s32 stage);
#define SDW_MEMBERS_PolyBatcher                                                   \
    __forceinline void Submit(RenderPoly *poly); /* source-only inlines, below */ \
    __forceinline void SubmitCallingTexFlush(RenderPoly *poly);
#include "sdw_classes.h"
#include "sdw_global_views.h"
#define Screen_ScaleX ScaleX
#define Screen_ScaleY ScaleY

/* The DAV directory (DavHeader.dir): packed, its pointers sit at +6/+0xa/+0x12, which the struct generator cannot lay
 * out, so it is declared here as in src/objects/lightspot.cpp. */
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

/* ---- the game's own helpers ---- */
#include "crc32.h"
#include "input.h"
#include "fixed_math.h"
#include "maths.h"
#include "interface.h"
#include "load_dav.h"
#include "load_warmeshes.h"
#include "progress.h"
#include "time.h"
#include "game_state.h"
#include "screen.h"
#include "draw2d.h"
s32 Rand_Bounded(s32 bound);                           /* 0x561219 */
u16 Str_Length(const char *s);                         /* 0x5614fb */
u32 *Res_GetValidatedIdList(u16 resId, u16 *outCount); /* 0x548381 */

u8 Text_ResetWindow();    /* 0x5322b3 */
void Text_ResetMeasure(); /* 0x5322fb */
s32 Text_FindPrevPageMark(char **start, char **cur);
s32 Text_FindNextPageMark(char **start, char **cur);
s32 Text_LineIsBlank(char **p);
char *Text_FindPageMark(char **p);
char *Text_PageStep(s32 mode, char **text, char **next, char **prev);
s32 Text_GetScrollInput(PadFrame *frame);
u16 Text_MeasureLine(const char *s);                               /* 0x533403 */
void Text_ApplyWindow(u32 *layer);                                 /* 0x53378a */
u16 Str_ParseU16(const char *s);                                   /* 0x533d68 */
s32 Text_ExpandButtonToken(const char *tok, char *out, u16 *len);  /* 0x533fbd */
s32 Text_ExpandColorToken(const char *tok, char *out, u16 *len);   /* 0x53447c */
s32 Text_ExpandNameToken(const char *tok, char *out, u16 *len);    /* 0x5343da */
s32 Text_ExpandMemCardToken(const char *tok, char *out, u16 *len); /* 0x534cd0 */
void Text_WordWrap(char *text, u8 mode);                           /* 0x5346ff */
void Text_SetWindow(u32 *layer, s32 x, s32 y, s32 w, s32 h, u32 unused);
void Text_VFormat(char *out, const char *fmt, va_list ap);
void Text_DrawString(u8 c, s32 x, s32 y, float z, u32 rgb);
void Text_EmitLineThunk(const char *text, s32 n, u8 align);
void Text_DrawNoWrap(const char *text, u8 align);
void Text_PutColorCode(char *out, u32 rgb);
void Text_PrintFmtStyled(u8 style, s32 blink, const char *fmt, va_list ap);

/* ---- globals used here, defined elsewhere ---- */

#define g_padCurButtons (g_pad.cur.buttons) /* 0x719654 */

#define g_padPrevButtons (g_pad.prev.buttons) /* 0x71964c */

#define g_padMasks (g_inputMap + 4)           /* 0x57eb80  active-low button masks; [10] = action */
extern u32 g_gameFlags;                       /* 0x6ddf74  0x1000 = GF_LETTERBOX */
extern s32 g_dtRawMs;                         /* 0x71b2d8  read as a dword here */
extern u8 g_sharedScratch[];                  /* 0x6d5468  shared scratch */
extern const char *(*g_pGetUiString)(u32 id); /* 0x6e3898 */

/* ---- this object's .bss aggregates (defined below, with the .bss) ---- */
/* .bss order. VC6 lays out a file's .bss as (1) the globals defined WITHOUT an initialiser (constructed objects and
 * scalars with a dynamic initialiser included), sorted by a hash of their NAMES - key (h ^ h >> 16) & 0x3ff with
 * h = h * 4 + (h >> 4) + c over the characters (tools/vc6_locals.py's h), ties last-declared first - then (2) the globals
 * explicitly initialised (to zero, or dynamically with an aggregate initialiser), in definition order. g_textGlyphPoly
 * has a constructor, so it is in group (1), and everything the exe puts before it must be in group (1) too, in
 * ascending key order. The descriptive names cannot do that: the ones other objects use (g_textWinX 310,
 * g_textWinY 311, g_textWinH 294, g_textCursorX 93, g_textCursorY 92, g_textClipOffX 357, g_scrollTextBegin 707,
 * g_scrollTextSource 521, g_scrollTextFlags 168) are themselves out of order, so no renaming of this file's private
 * globals alone can work. The original evidently had fewer, aggregated objects - Text_Reset clears the 0x18-byte
 * window block 0x6ddaf4 with one memset. They are written that way here: the window block and the scrolling-text block
 * are structs (TextPort, TextScroll, src/include/sdw_global_views.h), the default window a struct, and the
 * descriptive names are #defines of their members. The aggregate names are descriptive, chosen to hash in the exe's order:
 *     g_fonts 29 < g_textPort 120 < g_textScratchBuffer 217 < g_textScroll 235 < g_textDefRect 258
 *       < g_textFormatBuf 315 < g_textGlyphPoly 687        then, initialised (definition order): the three tail words.
 * Every other object that uses the window or scroll-text names declares the aggregates the same way. */

/* 0x6ddaf4, 0x18 bytes: the text window (Text_Reset clears it with one memset). */
/* 0x6ddf00, 0x24 bytes: the scrolling-text state. */

/* 0x6ddf24: the default text window Text_ApplyWindow restores. */
struct TextRect {
    s16 x, y, w, h;
};

extern Font g_fonts[3];                 /* 0x6ddab8 */
extern TextPort g_textPort;             /* 0x6ddaf4 */
extern char g_textScratchBuffer[0x3f0]; /* 0x6ddb10 */
extern TextScroll g_textScroll;         /* 0x6ddf00 */
extern TextRect g_textDefRect;          /* 0x6ddf24 */

/* the descriptive names, as used by the bodies below */
#define g_textScratchBuf g_textScratchBuffer
#define g_textLayer g_textPort.layer
#define g_textWinX g_textPort.winX
#define g_textWinY g_textPort.winY
#define g_textWinW g_textPort.winW
#define g_textWinH g_textPort.winH
#define g_textCursorX g_textPort.cursorX
#define g_textCursorY g_textPort.cursorY
#define g_textClipOffX g_textPort.clipOffX
#define g_textClipOffY g_textPort.clipOffY
#define g_textNoClip g_textPort.noClip
#define g_scrollTextBegin g_textScroll.begin
#define g_scrollTextSource g_textScroll.source
#define g_scrollTextPage g_textScroll.page
#define g_scrollTextNextPage g_textScroll.nextPage
#define g_scrollTextFlags g_textScroll.flags
#define g_scrollTextStartMs g_textScroll.startMs
#define g_scrollTextPageMs g_textScroll.pageMs
#define g_scrollTextRemainMs g_textScroll.remainMs
#define g_scrollTextInputTime g_textScroll.inputTime
#define g_textDefWinX g_textDefRect.x
#define g_textDefWinY g_textDefRect.y
#define g_textDefWinW g_textDefRect.w
#define g_textDefWinH g_textDefRect.h

/* ================= this object's .data 0x57bcd0-0x57c008, in address order ================= */
/* 0x57bcd0 - the hex digits, no terminator (0x57bce0 follows) */
char g_hexDigits[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
char *g_pTextScratch = g_textScratchBuf; /* 0x57bce0  scratch buffer for $token$ text */
/* 0x57bce4-0x57bd9c - the $B_...$ button tokens, in ALPHABETICAL order: named data (see the header) */
static char s_tokACTION[] = "ACTION";
static char s_tokCAM_LEFT[] = "CAM_LEFT";
static char s_tokCAM_RIGHT[] = "CAM_RIGHT";
static char s_tokCANCEL[] = "CANCEL";
static char s_tokCIRCLE[] = "CIRCLE";
static char s_tokCROSS[] = "CROSS";
static char s_tokDOWN[] = "DOWN";
static char s_tokINTVIEW[] = "INTVIEW";
static char s_tokJUMP[] = "JUMP";
static char s_tokL1[] = "L1";
static char s_tokL2[] = "L2";
static char s_tokLEFT[] = "LEFT";
static char s_tokQUICKINV[] = "QUICKINV";
static char s_tokR1[] = "R1";
static char s_tokR2[] = "R2";
static char s_tokRIGHT[] = "RIGHT";
static char s_tokRUN[] = "RUN";
static char s_tokSELECT[] = "SELECT";
static char s_tokSNEAK[] = "SNEAK";
static char s_tokSQUARE[] = "SQUARE";
static char s_tokSTART[] = "START";
static char s_tokTRIANGLE[] = "TRIANGLE";
static char s_tokUP[] = "UP";
static char s_tokVALID[] = "VALID";
Font *g_pCurFont = &g_fonts[0]; /* 0x57bd9c */
/* 0x57bda0 - the forward glyph variant of characters 0x20..0xff (code reads it as g_glyphVariantFwd[ch],
 * 0x57bd80 = this - 0x20: see the #define below) */
static u8 s_glyphVariantFwdTable[224] = {
    0x20, 0x21, 0x22, 0xc3, 0x24, 0xd2, 0xd4, 0x27, 0x28, 0x29, 0xd5, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0xa2, 0xa3, 0xa4,
    0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0x3a, 0xd6, 0xcd, 0x3d, 0xcc, 0x3f, 0xda, 0x61, 0x62, 0x63, 0x64, 0x65,
    0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
    0x79, 0x7a, 0xc9, 0x7c, 0x7d, 0x7e, 0x7f, 0xd9, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b,
    0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0xc8, 0x9c, 0x9d, 0x9e,
    0x9f, 0xdb, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50, 0x51,
    0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0xca, 0x5c, 0x5d, 0x5e, 0x5f, 0xdc, 0xa1, 0xb2, 0xb3, 0xb4,
    0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xac, 0xad, 0xae, 0xaf, 0xc7, 0xd1, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35,
    0x36, 0x37, 0x38, 0x39, 0xc0, 0xc1, 0xc2, 0xbf, 0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xc6, 0xe7, 0xe8, 0xe9, 0xea,
    0xcb, 0xec, 0xed, 0xdf, 0xef, 0xf0, 0xf1, 0xd2, 0xf3, 0xf4, 0xf5, 0xf6, 0xd8, 0xdd, 0xf9, 0xfa, 0xfb, 0xfc, 0xd7,
    0xde, 0xee, 0xbc, 0xbd, 0xbe, 0x23, 0xff, 0xf7, 0xf8, 0xb0, 0x7b, 0x5b, 0x9b, 0xe6, 0x3e, 0x3c, 0xce, 0xfd, 0xfe,
    0xb1, 0x25, 0xd3, 0x26, 0x2a, 0x3b, 0xc5, 0xeb, 0x60, 0x40, 0x80, 0xa0, 0xcf, 0xd0, 0xc4};
/* 0x57be80 - the reverse glyph variant of characters 0x20..0xff */
static u8 s_glyphVariantRevTable[224] = {
    0x20, 0x21, 0x22, 0xe3, 0x24, 0xf2, 0xf4, 0x27, 0x28, 0x29, 0xf5, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0xb2, 0xb3, 0xb4,
    0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0x3a, 0xf6, 0xed, 0x3d, 0xec, 0x3f, 0xfa, 0x81, 0x82, 0x83, 0x84, 0x85,
    0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98,
    0x99, 0x9a, 0xe9, 0x9c, 0x9d, 0x9e, 0x9f, 0xf9, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b,
    0x4c, 0x4d, 0x4e, 0x4f, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0xe8, 0x5c, 0x5d, 0x5e,
    0x5f, 0xfb, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70, 0x71,
    0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0xea, 0x7c, 0x7d, 0x7e, 0x7f, 0xfc, 0xa1, 0x30, 0x31, 0x32,
    0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0xac, 0xad, 0xae, 0xaf, 0xe7, 0xf1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
    0xa8, 0xa9, 0xaa, 0xab, 0xe0, 0xe1, 0xe2, 0xbf, 0xbc, 0xbd, 0xbe, 0x23, 0xff, 0xf7, 0xc6, 0xb0, 0x7b, 0x5b, 0x9b,
    0xcb, 0x3e, 0x3c, 0xee, 0xfd, 0xfe, 0xb1, 0xd2, 0xd3, 0x26, 0x2a, 0x3b, 0xdd, 0xd7, 0x60, 0x40, 0x80, 0xa0, 0xd8,
    0xde, 0xce, 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xeb, 0xc7, 0xc8, 0xc9, 0xca, 0xf8, 0xcc, 0xcd, 0xdf, 0xcf, 0xd0,
    0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xe5, 0xe6, 0xd9, 0xda, 0xdb, 0xdc, 0xef, 0xf0, 0xe4};
#define g_glyphVariantFwd (&s_glyphVariantFwdTable[-0x20]) /* 0x57bd80, as the code indexes it */
#define g_glyphVariantRev (&s_glyphVariantRevTable[-0x20]) /* 0x57be60 */
/* 0x57bf60 - the other $...$ tokens, in the order the exe has them (M_CARD before the colours) */
static char s_tokG_NAME[] = "G_NAME";
static char s_tokM_CARD[] = "M_CARD";
static char s_tokC_DEFAULT[] = "C_DEFAULT";
static char s_tokC_WHITE[] = "C_WHITE";
static char s_tokC_RED[] = "C_RED";
static char s_tokC_GREEN[] = "C_GREEN";
static char s_tokC_BLUE[] = "C_BLUE";
static char s_tokC_YELLOW[] = "C_YELLOW";
static char s_tokC_CYAN[] = "C_CYAN";
static char s_tokC_PURPLE[] = "C_PURPLE";
/* 0x57bfbc-0x57c008 - a second copy of the eight colour tokens that no instruction refers to (tools/tu_sheet.py;
 * attributed to this object by the map). Kept in place as the strings they are. */
static char s_tokC_DEFAULT_unref[] = "C_DEFAULT";
static char s_tokC_WHITE_unref[] = "C_WHITE";
static char s_tokC_RED_unref[] = "C_RED";
static char s_tokC_GREEN_unref[] = "C_GREEN";
static char s_tokC_BLUE_unref[] = "C_BLUE";
static char s_tokC_YELLOW_unref[] = "C_YELLOW";
static char s_tokC_CYAN_unref[] = "C_CYAN";
static char s_tokC_PURPLE_unref[] = "C_PURPLE";

/* ================= this object's .bss 0x6ddab8-0x6ddf5c, in address order (see the note above the .bss aggregates' types) ================= */
Font g_fonts[3];                 /* 0x6ddab8                                   key  29 */
TextPort g_textPort;             /* 0x6ddaf4                                   key 120 */
char g_textScratchBuffer[0x3f0]; /* 0x6ddb10  $TOKEN$ scratch (16 bytes), then the format buffer; key 217 */
TextScroll g_textScroll;         /* 0x6ddf00                                   key 235 */
TextRect g_textDefRect;          /* 0x6ddf24                                   key 258 */
char *g_textFormatBuf = g_pTextScratch + 0x10; /* 0x6ddf2c  Text_StaticInit / Text_InitFormatBuf;  key 315 */
RenderPoly g_textGlyphPoly;                    /* 0x6ddf30  the quad the glyph emitters fill;   key 687 */
u32 g_textEnabled = 0;                         /* 0x6ddf50  (initialised: definition order) */
u32 g_textBlinkLastMs = 0;                     /* 0x6ddf54 */
u32 g_textBlinkPhase = 0;                      /* 0x6ddf58 */

/* ================= 0x5321d0-0x5335f2: scrolling text and the first part of the text engine ================= */

/* 0x5321d0 Text_StaticInit / 0x5321da Text_InitFormatBuf: the definition of g_textFormatBuf above.
 * 0x5321ec-0x53222a: the static initialiser of g_textGlyphPoly (constructor, atexit, destructor), from its definition. */

/* 0x53222b - switches text off and clears the font table, the text window block and the scrolling-text state. */
void Text_Reset()
{
    u32 i;

    g_textEnabled = 0;
    g_pCurFont = 0;
    memset(&g_textLayer, 0, 0x18);
    for (i = 0; i < 3; i++)
        memset(&g_fonts[i], 0, 0x14);
    g_scrollTextSource = 0;
    g_scrollTextBegin = 0;
    g_scrollTextPage = 0;
    g_scrollTextNextPage = 0;
}

/* 0x5322b3 - the default text window is the whole 512x240 virtual screen. */
u8 Text_ResetWindow()
{
    g_textDefWinX = 0;
    g_textDefWinY = 0;
    g_textDefWinW = 0x200;
    g_textDefWinH = 0xf0;
    Text_ApplyWindow(g_textLayer);
    return 0;
}

/* 0x5322ec */
void Text_Disable()
{
    g_textEnabled = 0;
}

/* 0x5322fb - parks the page timers: the player pages by hand from now on. */
void Text_ResetMeasure()
{
    g_scrollTextRemainMs = 0x7fffffff;
    g_scrollTextPageMs = 0x7fffffff;
}

/* 0x532314 - scans back from *cur for the "$<digits>$" marker of the previous page (a '$' preceded by a printable
 * character); on success *cur = just past it and 1. */
s32 Text_FindPrevPageMark(char **start, char **cur)
{
    /* cast kept (the (u8 *) / (char *) views here): the text is char *; the scan reads unsigned bytes (movzx) */
    u8 *cur0 = (u8 *)*cur;
    u8 *scan = cur0;
    u8 *mark = 0;
    u8 num = 0;

    if (*start == 0 || *cur == 0 || *cur == *start)
        return 0;
    /* cast kept: text is walked as bytes */
    while (scan > (u8 *)*start && *scan != 0) {
        if (*scan == '$' && scan[-1] != '$' && scan[-1] > '\n') {
            mark = scan + 1;
            scan--;
            /* cast kept: text is walked as bytes */
            while (scan > (u8 *)*start && *scan != '$') {
                if (!num && *scan >= '0' && *scan <= '9')
                    num = 1;
                scan--;
            }
            if (*scan == '$') {
                if (num && mark != cur0 && *mark != '\n') {
                    /* cast kept: text is walked as bytes */
                    *cur = (char *)mark;
                    return 1;
                }
                num = 0;
            }
        }
        scan--;
    }
    return 0;
}

/* 0x532442 - forward twin of Text_FindPrevPageMark: *cur = just past the next "$<digits>$" marker that is not
 * followed by a newline. */
/* BYTES(flow, inferred): the same guarded step appears twice because the original has both */
s32 Text_FindNextPageMark(char **start, char **cur)
{
    /* cast kept (the (u8 *) / (char *) views here): the text is char *; the scan reads unsigned bytes (movzx) */
    u8 *p = (u8 *)*cur;
    u8 num = 0;

    if (*start == 0 || *cur == 0)
        return 0;
    while (*p != 0) {
        if (*p == '$') {
            p++;
            while (*p >= ' ') {
                if (!num && *p >= '0' && *p <= '9') {
                    num = 1;
                } else if (num && *p == '$') {
                    p++;
                    if (*p == '\n') {
                        num = 0;
                        break;
                    }
                    if (*p >= ' ') {
                        /* cast kept: text is walked as bytes */
                        *cur = (char *)p;
                        return 1;
                    }
                }
                if (*p != 0)
                    p++;
            }
        }
        if (*p != 0)
            p++;
    }
    return 0;
}

/* 0x532541 - 1 if nothing printable but $tokens$ is left before the next control character. */
s32 Text_LineIsBlank(char **pp)
{
    /* cast kept (the (u8 *) / (char *) views here): the text is char *; the scan reads unsigned bytes (movzx) */
    u8 *p = (u8 *)*pp;

    if (p != 0) {
        while (*p > '\n') {
            if (*p == '$') {
                p++;
                while (*p > '\n' && *p != '$')
                    p++;
                if (*p > '\n' && *p != '$') /* never true after that loop: the closing '$' is not skipped */
                    p++;
            }
            if (*p > ' ' && *p != '$')
                return 0;
            if (*p > '\n' && *p != '$')
                p++;
        }
    }
    return 1;
}

/* 0x532601 - the first "$<digits>$" marker of the string, or the string itself when there is none. */
char *Text_FindPageMark(char **pp)
{
    /* cast kept (the (u8 *) / (char *) views here): the text is char *; the scan reads unsigned bytes (movzx) */
    u8 *p = (u8 *)*pp;
    u8 *mark = (u8 *)*pp;
    u8 num = 0;

    while (*p != 0 && !num) {
        if (*p == '$') {
            mark = p;
            p++;
            while (*p >= ' ') {
                if (!num && *p >= '0' && *p <= '9') {
                    num = 1;
                } else if (num && *p == '$') {
                    /* cast kept: text is walked as bytes */
                    return (char *)mark;
                } else if (!num && *p == '$') {
                    break;
                }
                if (*p != 0)
                    p++;
            }
            num = 0;
        }
        if (*p != 0)
            p++;
    }
    return *pp;
}

/* 0x5326e9 - page stepping: mode 2 next page, 1 previous page, 0 skip a blank page. Sets *next / *prev to the
 * neighbouring page starts and the has-previous / has-next flags; returns the page now on screen. */
char *Text_PageStep(s32 mode, char **text, char **next, char **prev)
{
    char *page = g_scrollTextPage;

    if (*text == 0)
        return 0;
    if (page < *text) {
        page = *text;
        g_scrollTextPage = *text;
    }
    g_textScroll.flagBits.hasPrev = 0;
    g_textScroll.flagBits.hasNext = 0;
    *next = page;
    *prev = page;
    g_textScroll.flagBits.hasPrev = Text_FindPrevPageMark(&g_scrollTextBegin, prev);
    g_textScroll.flagBits.hasNext = Text_FindNextPageMark(&g_scrollTextBegin, next);
    if (mode == TEXTPAGE_NEXT && g_textScroll.flagBits.hasNext) {
        *prev = page;
        g_scrollTextPage = *next;
    } else if (mode == TEXTPAGE_PREV && g_textScroll.flagBits.hasPrev) {
        *next = page;
        g_scrollTextPage = *prev;
    } else if (mode == TEXTPAGE_SKIP_BLANK && Text_LineIsBlank(&g_scrollTextPage) && g_textScroll.flagBits.hasNext) {
        *prev = page;
        g_scrollTextPage = *next;
    }
    *next = g_scrollTextPage;
    *prev = g_scrollTextPage;
    g_textScroll.flagBits.hasPrev = Text_FindPrevPageMark(&g_scrollTextBegin, prev);
    g_textScroll.flagBits.hasNext = Text_FindNextPageMark(&g_scrollTextBegin, next);
    return g_scrollTextPage;
}

/* 0x5328b3 - the scrolling multi-page text driver (dialogue boxes, caller 0x5395c7): shows the page of `text` at
 * g_scrollTextPage through Text_WordWrap(mode), counts its display time down and steps to the next page when it runs
 * out; while the letterbox is up (GF_LETTERBOX) the action button takes over the paging (stick / d-pad, with an
 * auto-repeat after one second) and closes the text. 1 while there is text to show, 0 once it is finished or closed.
 * The local names are chosen for their stack slots (src/README.md), which is why their styles differ. */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py), hence the mixed styles */
/* BYTES(dead-code): nUnused2 (-0x44) and unused (-0x10) are written and never read, as in the original */
/* BYTES(cast, inferred): cond ? 1 : 0: the original materialises the value with mov 0 / mov 1 */
/* BYTES(flow, inferred): nI++ is written in both arms because the original has it in both */
s32 ScrollText_Run(u8 mode, char *text, s32 allowInput)
{
    u8 *pucText;     /* -0x4c the page on screen */
    char *pNext;     /* -0x48 */
    s32 nUnused2;    /* -0x44 */
    char *pPrevPage; /* -0x40 */
    s32 stickDown;   /* -0x3c stick / d-pad held down */
    s32 bNewUp;      /* -0x38 up pressed this frame */
    u16 dstLen;      /* -0x32 length of pLine */
    s32 fHasDigit;   /* -0x30 */
    u16 nI;          /* -0x2a read position in the page */
    char *pLine;     /* -0x28 the page's text, expanded, in g_textFormatBuf */
    s32 nLastY;      /* -0x24 scroll input last frame */
    s32 iStickY;     /* -0x20 scroll input now */
    s32 fNewDown;    /* -0x1c down pressed this frame */
    s32 numLen;      /* -0x18 length of the token in g_pTextScratch */
    s32 openedNow;   /* -0x14 the action button opened the paging this call */
    s32 unused;      /* -0x10 */
    s32 bUpHeld;     /* -0x0c stick / d-pad held up */
    s32 bDone;       /* -0x08 the page ends here (a marker or an unknown $token$) */
    s32 iParenLen;   /* -0x04 digits collected inside "(...)" of a page marker */

    pNext = 0;
    pPrevPage = 0;
    nI = 0;
    dstLen = 0;
    fHasDigit = 0;
    openedNow = 0;
    nUnused2 = 0;
    unused = 0;
    stickDown = 0;
    bUpHeld = 0;
    bNewUp = 0;
    fNewDown = 0;
    nLastY = Text_GetScrollInput(&g_pad.prev);
    iStickY = Text_GetScrollInput(&g_pad.cur);
    stickDown = iStickY > 0x80;
    bUpHeld = iStickY < -0x80;
    fNewDown = (iStickY > 0x80 && nLastY <= 0) ? 1 : 0;
    bNewUp = (iStickY < -0x80 && nLastY >= 0) ? 1 : 0;
    g_textScroll.flagBits.pageTurned = 0;
    if (!g_textEnabled)
        return 0;
    if (g_scrollTextSource != text || !(g_gameFlags & GF_LETTERBOX)) {
        g_scrollTextSource = text;
        g_scrollTextBegin = g_scrollTextPage = Text_FindPageMark(&text);
        g_scrollTextStartMs = g_rawTimeMs;
        g_scrollTextPageMs = g_scrollTextRemainMs = 0;
        g_textScroll.flagBits.closed = 0;
        g_textScroll.flagBits.open = 0;
        g_textScroll.flagBits.hasPrev = 0;
        g_textScroll.flagBits.hasNext = 0;
        g_scrollTextInputTime = g_rawTime;
    }
    if (!g_textScroll.flagBits.open && (g_gameFlags & GF_LETTERBOX) && allowInput &&
        (g_padCurButtons & ~g_padMasks[10]) == 0 && (g_padPrevButtons & ~g_padMasks[10]) != 0 &&
        Text_PageStep(TEXTPAGE_SKIP_BLANK, &text, &pNext, &pPrevPage)) {
        Text_ResetMeasure();
        g_scrollTextNextPage = pNext;
        g_scrollTextInputTime = g_rawTime;
        g_textScroll.flagBits.open = 1;
        g_textScroll.flagBits.closed = 0;
        openedNow = 1;
    }
    if (g_textScroll.flagBits.open) {
        s32 held;   /* -0x54 */
        s32 repeat; /* -0x50 */

        if ((g_padCurButtons & ~g_padMasks[10]) == 0 && (g_padPrevButtons & ~g_padMasks[10]) != 0 && !openedNow) {
            g_scrollTextSource = 0;
            g_scrollTextBegin = 0;
            g_textScroll.flagBits.closed = 1;
            return 0;
        }
        if (!stickDown && !bUpHeld)
            g_scrollTextInputTime = g_rawTime;
        held = g_rawTime - g_scrollTextInputTime - 0x1000;
        if (held > 0) {
            repeat = 1;
            g_scrollTextInputTime = g_rawTime - 0xccc;
        } else {
            repeat = 0;
        }
        Text_ResetMeasure();
        if (fNewDown || bNewUp)
            g_scrollTextInputTime = g_rawTime;
        /* cast kept (both (char) casts): the page-turned bit is the low bit of the page pointer Text_PageStep returns,
         * truncated through a char (mov [tmp],eax / movsx ecx,byte [tmp] at 0x532c0c): set for a page that starts at
         * an odd address, whether or not the page changed. */
        if (fNewDown || (repeat && stickDown))
            g_textScroll.flagBits.pageTurned = (char)Text_PageStep(TEXTPAGE_NEXT, &text, &pNext, &pPrevPage);
        else if (bNewUp || (repeat && bUpHeld))
            g_textScroll.flagBits.pageTurned = (char)Text_PageStep(TEXTPAGE_PREV, &text, &pNext, &pPrevPage);
    }
    /* cast kept (pucText and the three page stores below): the page is char *; the parser reads unsigned bytes */
    pucText = (u8 *)g_scrollTextPage;
    pLine = g_textFormatBuf;
    bDone = 0;
    if (g_scrollTextPageMs == 0) {
        while (pucText[nI] == '\n')
            nI++;
        while (pucText[nI] >= ' ' && pucText[nI] != '$')
            nI++;
        if (pucText[nI] == '$') {
            nI++;
            numLen = 0;
            iParenLen = 0;
            while (numLen < 5 && pucText[nI] >= ' ' && pucText[nI] != '$') {
                if (pucText[nI] >= '0' && pucText[nI] <= '9') {
                    g_pTextScratch[numLen] = pucText[nI];
                    numLen++;
                    nI++;
                } else if (pucText[nI] == '(') {
                    while (iParenLen < 5 && pucText[nI] >= ' ' && pucText[nI] != ')') {
                        if (pucText[nI] >= '0' && pucText[nI] <= '9') {
                            g_pTextScratch[iParenLen] = pucText[nI];
                            iParenLen++;
                            nI++;
                        } else {
                            nI++;
                        }
                    }
                    if (iParenLen != 0)
                        numLen = iParenLen;
                } else {
                    nI++;
                }
            }
            g_pTextScratch[numLen] = 0;
            if (pucText[nI] == '$')
                nI++;
            g_scrollTextPageMs = Str_ParseU16(g_pTextScratch) * 100;
            g_scrollTextRemainMs = g_scrollTextPageMs;
            /* cast kept: text is walked as bytes */
            g_scrollTextPage = (char *)pucText + nI;
        } else if (pucText[nI] == 0) {
            g_scrollTextSource = 0;
            g_scrollTextBegin = 0;
            g_scrollTextPageMs = 0;
            g_scrollTextRemainMs = 0;
            return 0;
        }
    }
    while (!bDone && pucText[nI] != 0) {
        if (pucText[nI] == '\n' && pucText[nI + 1] == '$')
            break;
        if (pucText[nI] == '\n') {
            /* a line break: copy the rest of the text up to the next page marker, or break the line here */
            u16 j;       /* -0x60 */
            u16 from;    /* -0x5e */
            u8 hasNum;   /* -0x5b */
            u16 newline; /* -0x5a */
            u16 mark;    /* -0x58 */
            u16 cut;     /* -0x56 */

            newline = nI;
            from = nI;
            cut = 0;
            mark = 0;
            hasNum = 0;
            while (pucText[nI] != 0 && pucText[nI] != '$') {
                if (pucText[nI] == '\n')
                    newline = nI;
                nI++;
            }
            if (pucText[nI] == '$') {
                mark = nI;
                nI++;
                while (pucText[nI] != '$') {
                    if (pucText[nI] >= '0' && pucText[nI] <= '9')
                        hasNum = 1;
                    nI++;
                }
            }
            if (hasNum && pucText[nI] == '$') {
                cut = mark;
            } else if (pucText[nI] == 0) {
                cut = nI;
                mark = nI + 1;
            }
            if (cut != 0) {
                if (pucText[mark - 1] <= '\n') {
                    j = from;
                    while (j < cut) {
                        pLine[dstLen] = pucText[j];
                        dstLen++;
                        j++;
                    }
                }
                nI = cut + 1;
                numLen = 0;
                bDone = 1;
                break;
            }
            pLine[dstLen] = pucText[newline];
            dstLen++;
            nI = newline + 1;
            continue;
        } else if (pucText[nI] == '$') {
            nI++;
            numLen = 0;
            while (pucText[nI] >= ' ' && pucText[nI] != '$') {
                while (pucText[nI] == ' ')
                    nI++;
                if (pucText[nI] > ' ') {
                    g_pTextScratch[numLen] = pucText[nI];
                    numLen++;
                    nI++;
                }
            }
            g_pTextScratch[numLen] = 0;
            if (pucText[nI] == '$')
                nI++;
            switch (*g_pTextScratch) {
                case 'B':
                    Text_ExpandButtonToken(g_pTextScratch, pLine, &dstLen);
                    break;
                case 'C':
                    Text_ExpandColorToken(g_pTextScratch, pLine, &dstLen);
                    break;
                case 'G':
                    Text_ExpandNameToken(g_pTextScratch, pLine, &dstLen);
                    break;
                default: {
                    /* an unknown token ends the page unless it holds a digit (a page marker) */
                    s32 m = 0; /* -0x64 */

                    fHasDigit = 0;
                    while (m < numLen && !fHasDigit) {
                        if (g_pTextScratch[m++] >= '0' && g_pTextScratch[m++] <= '9')
                            fHasDigit = 1;
                    }
                    if (!fHasDigit)
                        bDone = 1;
                }
            }
        } else {
            pLine[dstLen] = pucText[nI];
            dstLen++;
            nI++;
        }
    }
    pLine[dstLen] = 0;
    if (bDone)
        /* cast kept: text is walked as bytes */
        g_scrollTextNextPage = (char *)pucText + (nI - numLen) - 1;
    else
        g_scrollTextNextPage = (char *)pucText + nI;
    if (g_pProgress->optionBits.gate) /* Progress.optionFlags bit 2: the text is only drawn when it is set */
        Text_WordWrap(pLine, mode);
    g_scrollTextRemainMs -= g_dtRawMs;
    if (g_scrollTextRemainMs <= 0) {
        g_scrollTextPageMs = 0;
        g_scrollTextPage = g_scrollTextNextPage;
        if (*g_scrollTextPage == 0) {
            g_scrollTextSource = 0;
            g_scrollTextBegin = 0;
            return 0;
        }
    }
    return 1;
}

/* 0x53337f - vertical scroll input of a pad frame: the dead-zoned left stick Y on an analog pad (the type is always
 * read from g_pad.cur, whichever frame is passed), else +0x100 for d-pad down and -0x100 for up (active-low). */
s32 Text_GetScrollInput(PadFrame *frame)
{
    int y;
    int dx;

    dx = y = 0;
    if (g_pad.cur.typeLen.type == PADTYPE_ANALOG)
        Pad_StickToDeadzonedAxes(frame->leftX, frame->leftY, &dx, &y);
    if (y == 0) {
        if ((frame->buttons & ~(u16)~PAD_DOWN) == 0)
            y = 0x100;
        else if ((frame->buttons & ~(u16)~PAD_UP) == 0)
            y = -0x100;
    }
    return y;
}

/* 0x533403 - length of the first line of s that fits the current font's column count (word-wrapped at a space;
 * $tokens$ count as one glyph, $C colour tokens as none); 0 at the end of the string. */
/* BYTES(slot-name): names chosen for their stack slots: str -4, count -6, brk -0xc, k -0xe, start -0x10, code -0x11 */
u16 Text_MeasureLine(const char *s)
{
    /* names chosen for their stack slots: str -4, count -6, brk -0xc, k -0xe, start -0x10, code -0x11 */
    u8 code;
    u8 *str = (u8 *)s; /* cast kept: the text is const char *; the count reads it as unsigned bytes */
    u16 k = 0;
    u16 start = 0;
    u16 count = 0;
    int brk = 0;

    if (*str == 0)
        return 0;
    do {
        start = k;
        do {
            code = str[k++];
            if (code == '$') {
                if (str[k] == 'C')
                    count--;
                while ((code = str[k++]) != '$')
                    ;
            }
            count++;
            if (count <= g_pCurFont->cols)
                brk = k - 1;
        } while (code > ' ');
    } while (count - 1 <= g_pCurFont->cols && code >= ' ');
    if (count - 1 <= g_pCurFont->cols) {
        if (code == 0)
            return k - 1;
        return k;
    }
    if (start == 0)
        return brk + 1;
    return start;
}

/* 0x53353f - number of wrapped lines in s. */
u16 Text_CountWrappedLines(const char *s)
{
    u16 len = 0;
    u16 count = 0;

    while ((len = Text_MeasureLine(s)) != 0) {
        count++;
        s += len;
    }
    return count;
}

/* 0x533589 - the longest wrapped line of s, its newline not counted. */
u16 Text_MaxLineLength(const char *s)
{
    u16 len = 0;
    u16 longest = 0;

    while ((len = Text_MeasureLine(s)) != 0) {
        s += len;
        if (s[-1] == '\n')
            len--;
        if (len > longest)
            longest = len;
    }
    return longest;
}

/* ================= 0x5335f3-0x5362d0: the text engine, second part ================= */

/* The inline twins of Render_DrawPrimitive 0x415c00 and Render_SetTexture 0x415c40. The stage is a signed int: its
 * conversion to the device's DWORD is what gives the constant 0 its own stack temp in the expansion (0x53540c). */
/* BYTES(cast): source-only inline twins of 0x415c00 / 0x415c40; the signed stage gives the constant 0 its own stack temp (0x53540c) */
#define SDW_INLINE_D3DAPP_DRAWPRIMITIVEINLINE_U32_U32_VOID_U32 1
#include "../app/d3dapp_inlines.h"
#undef SDW_INLINE_D3DAPP_DRAWPRIMITIVEINLINE_U32_U32_VOID_U32
inline void D3DApp::SetTextureInline(Texture *tex, s32 stage)
{
    pD3DDevice->SetTexture(stage, tex->surface);
}

/* ---- source-only inline: PolyBatcher's submit, expanded in Text_DrawString (twice) ---- */
struct FlatVertex {
    float x, y, z, rhw;
    u32 diffuse, specular;
}; /* 0x18, FVF 0xc4 */
struct TlVertex {
    float x, y, z, rhw;
    u32 diffuse, specular;
    float u, v;
}; /* 0x20, FVF 0x1c4 */

/* BYTES(inline): __forceinline twin of SubmitPoly (Render_SetStateFlags out of line): VC6 declines to expand it as a plain inline here */
/* BYTES(slot-name): declared before batch for its stack slot */
__forceinline void PolyBatcher::Submit(RenderPoly *poly)
{
    switch (poly->type) {
        case RPOLY_OPAQUE:
            if (flatBatchCount <= batchCapacity) {
                /* cast kept: the batch buffer is untyped vertex memory; this batch holds FlatVertex triangles */
                memcpy((FlatVertex *)flatBatchVerts + flatBatchCount * 3, poly->verts, 0x48);
                flatBatchCount++;
            }
            if (flatBatchCount >= batchCapacity) {
                renderer->Render_SetStateFlags(stateFlags);
                renderer->DrawPrimitiveInline(D3DPT_TRIANGLELIST, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR,
                                              flatBatchVerts, batchCapacity * 3);
                renderer->Render_ClearStateFlags(stateFlags);
                flatBatchCount = 0;
                textureDirty = 1;
            }
            break;
        case RPOLY_BLEND:
            if (computeSortZ == 1)
                poly->sortZ = poly->verts[2] + poly->verts[8] + poly->verts[14];
            sortedPolys[sortedCount].Assign(poly);
            sortedList[sortedCount] = &sortedPolys[sortedCount];
            sortedCount++;
            break;
        case RPOLY_ADD:
            if (computeSortZ == 1)
                poly->sortZ = poly->verts[2] + poly->verts[8] + poly->verts[14];
            sortedPolys[sortedCount].Assign(poly);
            sortedList[sortedCount] = &sortedPolys[sortedCount];
            sortedCount++;
            break;
        default: {
            float *tri = poly->verts;
            u32 page = (poly->type - 4) & ~RPOLY_F_8000;
            if (page < immediateTexCount) {
                u32 *count; /* declared before batch for its stack slot */
                /* cast kept: the batch buffer is untyped vertex memory; a texture batch holds TlVertex triangles */
                TlVertex *batch = (TlVertex *)texBatchVerts[page];
                u32 *flags;
                count = &texBatchCounts[page];
                flags = &texStateFlags[page];
                if (*count <= batchCapacity) {
                    memcpy(batch + *count * 3, tri, 0x60);
                    (*count)++;
                }
                if (*count >= batchCapacity) {
                    if (page != lastTextureIndex || textureDirty == 1) {
                        renderer->SetTextureInline(textures[page], 0);
                        lastTextureIndex = page;
                        textureDirty = 0;
                    }
                    renderer->Render_SetStateFlags(*flags);
                    renderer->DrawPrimitiveInline(D3DPT_TRIANGLELIST,
                                                  D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1, batch,
                                                  batchCapacity * 3);
                    renderer->Render_ClearStateFlags(*flags);
                    *count = 0;
                }
            } else {
                if (computeSortZ == 1)
                    poly->sortZ = tri[2] + tri[10] + tri[18];
                sortedPolys[sortedCount].Assign(poly);
                sortedList[sortedCount] = &sortedPolys[sortedCount];
                sortedCount++;
            }
            break;
        }
    }
}

/* The same, as the SECOND expansion in Text_DrawString has it: there the texture-batch flush calls the out-of-line
 * Render_SetTexture / Render_DrawPrimitive while the untextured flush is still expanded (presumably VC6's inline budget
 * for the function runs out at that point; not reproduced, hence the second body). */
/* BYTES(inline): __forceinline copy for the second expansion: there the original calls Render_SetTexture / Render_DrawPrimitive out of line */
__forceinline void PolyBatcher::SubmitCallingTexFlush(RenderPoly *poly)
{
    switch (poly->type) {
        case RPOLY_OPAQUE:
            if (flatBatchCount <= batchCapacity) {
                /* cast kept: the batch buffer is untyped vertex memory; this batch holds FlatVertex triangles */
                memcpy((FlatVertex *)flatBatchVerts + flatBatchCount * 3, poly->verts, 0x48);
                flatBatchCount++;
            }
            if (flatBatchCount >= batchCapacity) {
                renderer->Render_SetStateFlags(stateFlags);
                renderer->DrawPrimitiveInline(D3DPT_TRIANGLELIST, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR,
                                              flatBatchVerts, batchCapacity * 3);
                renderer->Render_ClearStateFlags(stateFlags);
                flatBatchCount = 0;
                textureDirty = 1;
            }
            break;
        case RPOLY_BLEND:
            if (computeSortZ == 1)
                poly->sortZ = poly->verts[2] + poly->verts[8] + poly->verts[14];
            sortedPolys[sortedCount].Assign(poly);
            sortedList[sortedCount] = &sortedPolys[sortedCount];
            sortedCount++;
            break;
        case RPOLY_ADD:
            if (computeSortZ == 1)
                poly->sortZ = poly->verts[2] + poly->verts[8] + poly->verts[14];
            sortedPolys[sortedCount].Assign(poly);
            sortedList[sortedCount] = &sortedPolys[sortedCount];
            sortedCount++;
            break;
        default: {
            float *tri = poly->verts;
            u32 page = (poly->type - 4) & ~RPOLY_F_8000;
            if (page < immediateTexCount) {
                u32 *count; /* declared before batch for its stack slot */
                /* cast kept: the batch buffer is untyped vertex memory; a texture batch holds TlVertex triangles */
                TlVertex *batch = (TlVertex *)texBatchVerts[page];
                u32 *flags;
                count = &texBatchCounts[page];
                flags = &texStateFlags[page];
                if (*count <= batchCapacity) {
                    memcpy(batch + *count * 3, tri, 0x60);
                    (*count)++;
                }
                if (*count >= batchCapacity) {
                    if (page != lastTextureIndex || textureDirty == 1) {
                        renderer->Render_SetTexture(textures[page], 0);
                        lastTextureIndex = page;
                        textureDirty = 0;
                    }
                    renderer->Render_SetStateFlags(*flags);
                    renderer->Render_DrawPrimitive(D3DPT_TRIANGLELIST,
                                                   D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1,
                                                   batch, batchCapacity * 3);
                    renderer->Render_ClearStateFlags(*flags);
                    *count = 0;
                }
            } else {
                if (computeSortZ == 1)
                    poly->sortZ = tri[2] + tri[10] + tri[18];
                sortedPolys[sortedCount].Assign(poly);
                sortedList[sortedCount] = &sortedPolys[sortedCount];
                sortedCount++;
            }
            break;
        }
    }
}

/* 0x5335f3 - sets the current font's glyph size (line pitch = height + 2); returns the old size as h << 16 + w. */
u32 Font_SetCellSize(u8 w, u8 h)
{
    u32 old = (g_pCurFont->glyphHeight << 16) + g_pCurFont->glyphWidth;
    g_pCurFont->glyphWidth = w;
    g_pCurFont->glyphHeight = h;
    g_pCurFont->lineHeight = h + 2;
    return old;
}

/* 0x53363f */
void Text_SetCursor(s16 x, s16 y)
{
    g_textCursorX = x;
    g_textCursorY = y;
}

/* 0x533659 - carriage return and `lines` line feeds. */
void Text_NewLine(s32 lines)
{
    g_textCursorX = 0;
    g_textCursorY = g_textCursorY + g_pCurFont->lineHeight * lines;
}

/* 0x533684 - puts the cursor so that `lines` lines are centred vertically in the window. */
void Text_CenterVertically(s32 lines)
{
    if (!g_textEnabled)
        return;
    g_textCursorY = (g_textWinH - lines * g_pCurFont->lineHeight) / 2;
}

/* 0x5336b8 */
void Text_SetColor(u32 rgb)
{
    g_pCurFont->color = rgb | 0x05000000;
}

/* 0x5336cd - no callers. */
void Font_SetHeader(u32 v)
{
    g_pCurFont->color = v;
}

/* 0x5336dc - selects g_fonts[fontId]; a change refits the window in its cells. The colour is reset either way. */
void Text_SetFont(u8 fontId)
{
    if (g_pCurFont != &g_fonts[fontId]) {
        g_pCurFont = &g_fonts[fontId];
        g_pCurFont->cols = g_textWinW / g_pCurFont->glyphWidth;
        g_pCurFont->rows = g_textWinH / g_pCurFont->lineHeight;
    }
    Text_SetColor(0x4bccff);
}

/* 0x53374f */
void Text_GetWindow(s16 *out)
{
    out[0] = g_textWinX;
    out[1] = g_textWinY;
    out[2] = g_textWinW;
    out[3] = g_textWinH;
}

/* 0x53378a - back to the default window. The clip offsets are set to the window origin (not to 0). */
void Text_ApplyWindow(u32 *layer)
{
    g_textWinX = g_textDefWinX;
    g_textWinY = g_textDefWinY;
    g_textWinW = g_textDefWinW;
    g_textWinH = g_textDefWinH;
    g_pCurFont->cols = g_textWinW / g_pCurFont->glyphWidth;
    g_pCurFont->rows = g_textWinH / g_pCurFont->lineHeight;
    g_textClipOffX = g_textWinX;
    g_textClipOffY = g_textWinY;
    g_textCursorX = g_textCursorY = 0;
    g_textLayer = layer;
}

/* 0x533836 */
void Text_SetWindowRect(u32 *layer, const s16 *rect, u32 noClip)
{
    Text_SetWindow(layer, rect[0], rect[1], rect[2], rect[3], noClip);
}

/* 0x53386a - the window size in character cells. */
void Text_SetWindowCells(u32 *layer, s32 x, s32 y, s32 cols, s32 rows, u32 noClip)
{
    Text_SetWindow(layer, x, y, cols * g_pCurFont->glyphWidth, rows * g_pCurFont->lineHeight, noClip);
}

/* 0x5338a9 - moves / resizes the window; a top above the screen is cut off and remembered in g_textClipOffY. */
void Text_AdjustWindow(s16 dx, s16 dy, s16 dw, s16 dh)
{
    g_textWinX += dx;
    g_textWinY += dy;
    g_textWinW += dw;
    g_textWinH += dh;
    if (g_textWinY < 0) {
        g_textWinH += g_textWinY;
        g_textClipOffY = g_textWinY;
        g_textWinY = 0;
    }
    g_pCurFont->cols = g_textWinW / g_pCurFont->glyphWidth;
    g_pCurFont->rows = g_textWinH / g_pCurFont->lineHeight;
}

/* 0x533970 - '$' and v in hex without leading zeros; returns the length. */
u16 Text_FormatHex(char *out, u32 v)
{
    s8 shift = 28;
    u16 len = 0;
    while (!((v >> shift) & 0xf) && shift > 0)
        shift -= 4;
    out[len++] = '$';
    do {
        out[len] = g_hexDigits[(v >> shift) & 0xf];
        shift -= 4;
        len++;
    } while (shift != -4);
    return len;
}

/* 0x5339fc - '&' and all eight hex digits. No callers. */
u16 Text_FormatHex8(char *out, u32 v)
{
    s8 shift = 28;
    u16 len = 0;
    out[len++] = '&';
    do {
        out[len] = g_hexDigits[(v >> shift) & 0xf];
        shift -= 4;
        len++;
    } while (shift != -4);
    return len;
}

/* 0x533a64 - decimal through packed BCD; more than 8 digits clamps to '>' 99999999. */
u16 Text_FormatUInt(char *out, u32 v)
{
    s8 shift = 28;
    u16 len = 0;
    if (v > 99999999) {
        v = 99999999;
        out[len++] = '>';
    }
    v = Int_ToBcd(v);
    while (!((v >> shift) & 0xf) && shift > 0)
        shift -= 4;
    do {
        out[len] = g_hexDigits[(v >> shift) & 0xf];
        shift -= 4;
        len++;
    } while (shift != -4);
    return len;
}

/* 0x533b0f - as Text_FormatUInt, zero-padded to minDigits. No callers. */
u16 Text_FormatUIntPadded(char *out, u32 v, u8 minDigits)
{
    s8 shift = 28;
    u16 len = 0;
    if (v > 99999999) {
        v = 99999999;
        out[len++] = '>';
    }
    v = Int_ToBcd(v);
    while (!((v >> shift) & 0xf) && shift > 0) {
        if (shift < minDigits * 4)
            out[len++] = '0';
        shift -= 4;
    }
    do {
        out[len] = g_hexDigits[(v >> shift) & 0xf];
        shift -= 4;
        len++;
    } while (shift != -4);
    return len;
}

/* 0x533be0 - signed decimal; |v| over 99999999 clamps to '>' +-99999999 (the '>' comes before the '-'). */
u16 Text_FormatInt(char *out, s32 v)
{
    s8 shift = 28;
    u16 len = 0;
    if ((v >= 0 ? v : -v) > 99999999) {
        v = (v >= 0 ? 1 : -1) * 99999999;
        out[len++] = '>';
    }
    if (v >= 0) {
        v = Int_ToBcd(v);
    } else {
        v = Int_ToBcd(-v);
        out[len++] = '-';
    }
    while (!((v >> shift) & 0xf) && shift > 0)
        shift -= 4;
    do {
        out[len] = g_hexDigits[(v >> shift) & 0xf];
        shift -= 4;
        len++;
    } while (shift != -4);
    return len;
}

/* 0x533ce2 - '%' and v in binary without leading zeros. No callers. */
u16 Text_FormatBinary(char *out, u32 v)
{
    s8 shift = 31;
    u16 len = 0;
    while (!((v >> shift) & 1) && shift > 0)
        shift--;
    out[len++] = '%';
    do {
        out[len] = ((v >> shift) & 1) + '0';
        shift--;
        len++;
    } while (shift != -1);
    return len;
}

/* 0x533d68 - decimal digits to a number, from the last digit back (no sign, no validation). */
u16 Str_ParseU16(const char *s)
{
    u16 result = 0;
    u16 mul = 1;
    const char *p = s + Str_Length(s) - 1;
    while (p >= s) {
        result += (u16)((*p - '0') * mul);
        mul *= 10;
        p--;
    }
    return result;
}

/* 0x533dda - the load-time glyph shuffle: every printable character draws Rand_Bounded(0x7e) and becomes its forward
 * variant (roll < 0x2a), its reverse variant (roll < 0x54, or always for an apostrophe) or stays; $...$ tokens and the
 * character after '%' are skipped, 0xce / 0xee become 0xde. One RNG call per character, skipped ones included. */
void Text_ScrambleGlyphs(char *s)
{
    u8 rnd;
    u8 ch;
    while ((ch = *s) != 0) {
        rnd = Rand_Bounded(0x7e) & 0x7f;
        if (ch == '$') {
            do {
                s++;
                ch = *s;
            } while (ch != '$');
        } else if (ch == '%') {
            s++;
        } else if (ch >= 0x20) {
            if (ch == 0x92)
                ch = '\'';
            if (ch == 0xce || ch == 0xee)
                *s = (char)0xde;
            else if (rnd < 0x2a)
                *s = g_glyphVariantFwd[ch];
            else if (rnd < 0x54 || ch == '\'')
                *s = g_glyphVariantRev[ch];
        }
        s++;
    }
}

/* 0x533ec3 - the button-glyph blink: a 0..0xba ms phase from g_rawTimeMs; the first third turns every printable
 * character into its forward variant, the second into its reverse one (colour codes are skipped). */
/* BYTES(dead-code): unused is zeroed and never read: the original zeroes this slot */
void Text_BlinkButtonGlyphs(char *s)
{
    u8 ch;
    u32 unused = 0;
    if (g_textBlinkLastMs != g_rawTimeMs) {
        g_textBlinkPhase += g_rawTimeMs - g_textBlinkLastMs;
        if (g_textBlinkPhase > 0xba)
            g_textBlinkPhase = 0;
        g_textBlinkLastMs = g_rawTimeMs;
    }
    if (g_textBlinkPhase < 0x3e) {
        while ((ch = *s) != 0) {
            if (ch == 1)
                s += 3;
            else if (ch >= 0x20)
                *s = g_glyphVariantFwd[ch];
            s++;
        }
    } else if (g_textBlinkPhase < 0x7c) {
        while ((ch = *s) != 0) {
            if (ch == 1)
                s += 3;
            else if (ch >= 0x20)
                *s = g_glyphVariantRev[ch];
            s++;
        }
    }
}

/* 0x533fbd - $B_<button>$: the remappable actions take their glyph from the current mapping (g_inputMapAux[8..15]),
 * the fixed pad buttons have fixed glyphs; the four face buttons are drawn in grey (0x808080) between two colour codes,
 * SELECT and START are two glyphs wide. Returns 0 for an unknown token. */
/* BYTES(layout): a named static array, not a literal: the exe has the tokens as named data (alphabetical, before g_pCurFont) */
s32 Text_ExpandButtonToken(const char *tok, char *out, u16 *len)
{
    u8 glyph = 0;
    u32 savedColor;
    if (tok[0] != 'B' || tok[1] != '_')
        return 0;
    tok += 2;
    if (Str_IsPrefixOf(tok, s_tokACTION))
        glyph = g_inputMapAux[14];
    else if (Str_IsPrefixOf(tok, s_tokRUN))
        glyph = g_inputMapAux[13];
    else if (Str_IsPrefixOf(tok, s_tokJUMP))
        glyph = g_inputMapAux[15];
    else if (Str_IsPrefixOf(tok, s_tokINTVIEW))
        glyph = g_inputMapAux[12];
    else if (Str_IsPrefixOf(tok, s_tokQUICKINV))
        glyph = g_inputMapAux[10];
    else if (Str_IsPrefixOf(tok, s_tokSNEAK))
        glyph = g_inputMapAux[11];
    else if (Str_IsPrefixOf(tok, s_tokCAM_LEFT))
        glyph = g_inputMapAux[8];
    else if (Str_IsPrefixOf(tok, s_tokCAM_RIGHT))
        glyph = g_inputMapAux[9];
    else if (Str_IsPrefixOf(tok, s_tokCROSS))
        glyph = 0x5f;
    else if (Str_IsPrefixOf(tok, s_tokTRIANGLE))
        glyph = 0x5d;
    else if (Str_IsPrefixOf(tok, s_tokSQUARE))
        glyph = 0x5c;
    else if (Str_IsPrefixOf(tok, s_tokCIRCLE))
        glyph = 0x5e;
    else if (Str_IsPrefixOf(tok, s_tokVALID))
        glyph = 0x5f;
    else if (Str_IsPrefixOf(tok, s_tokCANCEL))
        glyph = 0x5d;
    else if (Str_IsPrefixOf(tok, s_tokL1))
        glyph = 0xc5;
    else if (Str_IsPrefixOf(tok, s_tokR1))
        glyph = 0xeb;
    else if (Str_IsPrefixOf(tok, s_tokL2))
        glyph = 0xc6;
    else if (Str_IsPrefixOf(tok, s_tokR2))
        glyph = 0xcb;
    else if (Str_IsPrefixOf(tok, s_tokLEFT))
        glyph = 0xd0;
    else if (Str_IsPrefixOf(tok, s_tokRIGHT))
        glyph = 0xcf;
    else if (Str_IsPrefixOf(tok, s_tokUP))
        glyph = 0xd7;
    else if (Str_IsPrefixOf(tok, s_tokDOWN))
        glyph = 0xce;
    else if (Str_IsPrefixOf(tok, s_tokSELECT)) {
        out[*len] = (char)0xac;
        (*len)++;
        glyph = 0xad;
    } else if (Str_IsPrefixOf(tok, s_tokSTART)) {
        out[*len] = (char)0xae;
        (*len)++;
        glyph = 0xaf;
    }
    if (!glyph)
        return 0;
    if (glyph >= 0x5c && glyph <= 0x5f) {
        savedColor = g_pCurFont->color;
        Text_PutColorCode(out + *len, 0x808080);
        *len += 4;
        out[*len] = glyph;
        (*len)++;
        Text_PutColorCode(out + *len, savedColor);
        *len += 4;
    } else {
        out[*len] = glyph;
        (*len)++;
    }
    return 1;
}

/* 0x5343da - $G_NAME$: UI string 0x4d. */
s32 Text_ExpandNameToken(const char *tok, char *out, u16 *len)
{
    if (Str_IsPrefixOf(tok, s_tokG_NAME)) {
        *len += (u16)((u16)Str_Copy(out + *len, Text_GetUiString(UISTR_GAME_TITLE)) - 1);
        return 1;
    }
    return 0;
}

/* 0x534437 - the inline colour code {1, r, g, b}. */
void Text_PutColorCode(char *out, u32 rgb)
{
    if (!out)
        return;
    out[0] = 1;
    out[1] = rgb & 0xff;
    out[2] = (rgb >> 8) & 0xff;
    out[3] = (rgb >> 16) & 0xff;
}

/* 0x53447c - $C_<colour>$. Returns 0 for an unknown colour. */
s32 Text_ExpandColorToken(const char *tok, char *out, u16 *len)
{
    out += *len;
    if (Str_IsPrefixOf(tok, s_tokC_DEFAULT))
        Text_PutColorCode(out, 0x4bccff);
    else if (Str_IsPrefixOf(tok, s_tokC_WHITE))
        Text_PutColorCode(out, 0xffffff);
    else if (Str_IsPrefixOf(tok, s_tokC_RED))
        Text_PutColorCode(out, 0x5050ff);
    else if (Str_IsPrefixOf(tok, s_tokC_GREEN))
        Text_PutColorCode(out, 0x50ff50);
    else if (Str_IsPrefixOf(tok, s_tokC_BLUE))
        Text_PutColorCode(out, 0xff7070);
    else if (Str_IsPrefixOf(tok, s_tokC_YELLOW))
        Text_PutColorCode(out, 0x50ffff);
    else if (Str_IsPrefixOf(tok, s_tokC_CYAN))
        Text_PutColorCode(out, 0xffff50);
    else if (Str_IsPrefixOf(tok, s_tokC_PURPLE))
        Text_PutColorCode(out, 0xff50ff);
    else
        return 0;
    *len += 4;
    return 1;
}

/* 0x5345f3 - formats into g_textScratchBuf, or (alt) into the collision scratch. No callers. */
void Text_SelectFormatBuffer(s32 alt)
{
    if (alt) {
        /* cast kept: g_sharedScratch is one scratch buffer that each user lays out its own way */
        g_pTextScratch = (char *)g_sharedScratch;
        g_textFormatBuf = g_pTextScratch + 0x10;
    } else {
        g_pTextScratch = g_textScratchBuf;
        g_textFormatBuf = g_pTextScratch + 0x10;
    }
}

/* 0x534630 */
void Text_PrintWrapped(char *s)
{
    if (s == 0 || *s == 0)
        return;
    Text_WordWrap(s, TEXTALIGN_CONTINUE);
}

/* 0x534655 */
void Text_PrintFmt(const char *fmt, ...)
{
    va_list ap;
    if (!fmt)
        return;
    va_start(ap, fmt);
    if (g_textEnabled) {
        Text_VFormat(g_textFormatBuf, fmt, ap);
        Text_PrintWrapped(g_textFormatBuf);
    }
}

/* 0x534699 - the main text printf: wrapped, or (after Text_SetNoClipOnce) split on spaces only. */
void Text_Printf(u8 align, const char *fmt, ...)
{
    va_list ap;
    if (!fmt)
        return;
    va_start(ap, fmt);
    if (g_textEnabled) {
        Text_VFormat(g_textFormatBuf, fmt, ap);
        if (!g_textNoClip)
            Text_WordWrap(g_textFormatBuf, align);
        else
            Text_DrawNoWrap(g_textFormatBuf, align);
    }
}

/* 0x5346ff - the word wrap: breaks lines at bytes <= 0x20 so that a line holds at most `cols` visible characters (colour
 * codes do not count), cutting a word that is longer than a whole line; each line goes to the emitter. */
/* BYTES(flow, inferred): statement shapes kept for the original's operand order */
void Text_WordWrap(char *text, u8 mode)
{
    /* cast kept (the (u8 *) / (char *) views here): the text is char *; the scan reads unsigned bytes (movzx) */
    u8 skip;
    u8 hard;
    u8 *end;
    u8 *line;
    u8 *p;
    /* cast kept: text is walked as bytes */
    p = line = end = (u8 *)text;
    skip = 0;
    hard = 0;
    if (text != 0) {
        while (*p != 0) {
            p = line = end;
            skip = 0;
            hard = 0;
            while (p - line <= g_pCurFont->cols + skip) {
                if (*p <= 0x20) {
                    if (*p == 1) {
                        end = p - skip;
                        p += 4;
                        skip += 4;
                        continue;
                    }
                    if (*p == '\n') {
                        p++;
                        end = p - skip;
                        hard = 1;
                        break;
                    }
                    if (*p == 0) {
                        end = p - skip;
                        hard = 1;
                        break;
                    }
                    p++;
                    end = p - skip;
                } else {
                    p++;
                }
            }
            if (end == line && !hard)
                end = line + g_pCurFont->cols;
            /* cast kept: text is walked as bytes */
            Text_EmitLineThunk((char *)line, end - line, mode);
            end += skip;
            if (!hard) {
                g_textCursorX = 0;
                g_textCursorY += g_pCurFont->lineHeight;
                while (*end == ' ')
                    end++;
                if (*end)
                    p = end;
            }
        }
    }
}

/* 0x5348aa */
void Text_PrintfStyled(u8 align, s32 blink, const char *fmt, ...)
{
    va_list ap;
    if (!fmt)
        return;
    va_start(ap, fmt);
    Text_PrintFmtStyled(align, blink, fmt, ap);
}

/* 0x5348d8 */
void Text_PrintFmtStyled(u8 style, s32 blink, const char *fmt, va_list ap)
{
    if (!fmt)
        return;
    if (g_textEnabled) {
        Text_VFormat(g_textFormatBuf, fmt, ap);
        if (blink)
            Text_BlinkButtonGlyphs(g_textFormatBuf);
        Text_WordWrap(g_textFormatBuf, style);
    }
}

/* 0x53492c - sprintf through the engine's formatter; returns out. */
char *Text_Sprintf(char *out, const char *fmt, ...)
{
    va_list ap;
    if (fmt) {
        va_start(ap, fmt);
        Text_VFormat(out, fmt, ap);
    } else {
        *out = 0;
    }
    return out;
}

/* 0x53495f - the formatter: %s %d %u %x %% %$ and the $TOKEN$ escapes (B_ button glyph, C_ colour, G_NAME, M_CARD); an
 * unknown conversion or token produces nothing. The token is gathered in g_pTextScratch with no length check. */
/* BYTES(dead-code): unused is zeroed and never read: the original zeroes this slot */
void Text_VFormat(char *out, const char *fmt, va_list ap)
{
    s32 unused = 0;
    u16 i = 0;
    u16 len = 0;
    const u8 *fp = (const u8 *)fmt; /* cast kept: the format is const char *; it is read as unsigned bytes */
    char *dst = out;
    u8 code;
    do {
        if (fp[i] == '%') {
            i++;
            code = fp[i];
            switch (code) {
                case 's': {
                    const char *str = va_arg(ap, const char *);
                    while (*str) {
                        dst[len] = *str;
                        len++;
                        str++;
                    }
                    break;
                }
                case '$':
                    dst[len] = '$';
                    len++;
                    break;
                case '%':
                    dst[len] = '%';
                    len++;
                    break;
                case 'd':
                    len += Text_FormatInt(dst + len, va_arg(ap, s32));
                    break;
                case 'u':
                    len += Text_FormatUInt(dst + len, va_arg(ap, u32));
                    break;
                case 'x':
                    len += Text_FormatHex(dst + len, va_arg(ap, u32));
                    break;
            }
        } else if (fp[i] == '$') {
            u8 k = 0;
            while (fp[++i] != '$') {
                g_pTextScratch[k] = fp[i];
                k++;
            }
            g_pTextScratch[k] = 0;
            switch (g_pTextScratch[0]) {
                case 'B':
                    Text_ExpandButtonToken(g_pTextScratch, dst, &len);
                    break;
                case 'C':
                    Text_ExpandColorToken(g_pTextScratch, dst, &len);
                    break;
                case 'G':
                    Text_ExpandNameToken(g_pTextScratch, dst, &len);
                    break;
                case 'M':
                    Text_ExpandMemCardToken(g_pTextScratch, dst, &len);
                    break;
            }
        } else {
            dst[len] = fp[i];
            len++;
        }
    } while (fp[i++] != 0);
    dst[len] = 0;
    va_end(ap);
}

/* 0x534cd0 - $M_CARD$: UI string 0x32. */
s32 Text_ExpandMemCardToken(const char *tok, char *out, u16 *len)
{
    if (Str_IsPrefixOf(tok, s_tokM_CARD)) {
        *len += (u16)((u16)Str_Copy(out + *len, g_pGetUiString(MCSTR_SAVE_FILE)) - 1);
        return 1;
    }
    return 0;
}

/* 0x534d2e - draws one glyph (despite the table name): a textured quad of the current font's cell for character c at
 * window position x, y (HUD units), depth z, colour rgb, clipped to the text window by shrinking the quad and its UVs.
 * The quad goes to the PolyBatcher as two triangles through g_textGlyphPoly (Submit, expanded twice).
 * Locals are named for their stack slots (see the file comment): glyphW / ch are the cell's texel size, texU / v0 its
 * texel origin, trimL / topCut / cutRight / cutBottom how much of the cell the window hides on each side (the right
 * and bottom ones negative), scrX / screenY / edgeR / edgeB the cell's HUD rectangle, fx0 / Y0 / X1 / yBottom the
 * same rectangle in screen pixels. */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py) */
/* BYTES(flow): the grouping (g_textWinX + g_textClipOffX) keeps the original operand order */
/* BYTES(flow): the parentheses matter: without them VC6 loads 1.0f first (fld / fdiv) */
void Text_DrawString(u8 c, s32 x, s32 y, float z, u32 rgb)
{
    div_t glyph;
    float wRecip;
    TlVertex *quad;
    s32 glyphW;
    s32 edgeR;
    s32 edgeB;
    s32 scrX;
    s32 screenY;
    float X1;
    float yBottom;
    s32 cutRight;
    u32 colour;
    s32 topCut;
    s32 trimL;
    s32 cutBottom;
    float fx0;
    float Y0;
    s32 texU;
    s32 ch;
    s32 v0;

    trimL = topCut = cutRight = cutBottom = 0;
    if (x + g_pCurFont->glyphWidth < 0)
        return;
    if (x < 0)
        trimL = -x;
    if (x >= g_textWinW)
        return;
    if (x + g_pCurFont->glyphWidth >= g_textWinW)
        cutRight = g_textWinW - (x + g_pCurFont->glyphWidth + 1);
    if (y + g_pCurFont->glyphHeight < 0)
        return;
    if (y < 0)
        topCut = -y;
    if (y >= g_textWinH)
        return;
    if (y + g_pCurFont->glyphHeight >= g_textWinH)
        cutBottom = g_textWinH - (y + g_pCurFont->glyphHeight + 1);
    scrX = x + (g_textWinX + g_textClipOffX);
    screenY = y + (g_textWinY + g_textClipOffY);
    edgeR = scrX + g_pCurFont->glyphWidth;
    edgeB = screenY + g_pCurFont->glyphHeight;
    fx0 = g_screen.Screen_ScaleX(scrX + trimL);
    Y0 = g_screen.Screen_ScaleY(screenY + topCut);
    X1 = g_screen.Screen_ScaleX(edgeR + cutRight);
    yBottom = g_screen.Screen_ScaleY(edgeB + cutBottom);
    glyph = div(c - 0x20, 16);
    texU = glyph.rem * g_pCurFont->cellW + g_pCurFont->sheetU;
    v0 = glyph.quot * g_pCurFont->cellH + g_pCurFont->sheetV;
    glyphW = g_pCurFont->cellW;
    ch = g_pCurFont->cellH;
    g_textGlyphPoly.type = g_pCurFont->texPage + 4;
    g_textGlyphPoly.sortZ = z;
    wRecip = 1.0f / (g_pViewFrustum->nearZ); /* the parentheses matter: without them VC6 loads 1.0f first (fld/fdiv) */
    colour = Color_RgbToBgr(rgb);
    /* cast kept: vertex memory is untyped floats; the polygon's vertex format decides the struct it holds */
    quad = (TlVertex *)g_textGlyphPoly.verts;
    quad[0].x = fx0;
    quad[0].y = Y0;
    quad[0].z = z;
    quad[0].rhw = wRecip;
    quad[0].diffuse = colour;
    quad[0].specular = 0xff000000;
    quad[1].x = X1;
    quad[1].y = Y0;
    quad[1].z = z;
    quad[1].rhw = wRecip;
    quad[1].diffuse = colour;
    quad[1].specular = 0xff000000;
    quad[2].x = X1;
    quad[2].y = yBottom;
    quad[2].z = z;
    quad[2].rhw = wRecip;
    quad[2].diffuse = colour;
    quad[2].specular = 0xff000000;
    quad[0].u = Tex_CornerUV(trimL, glyphW, texU);
    quad[0].v = Tex_CornerUV(topCut, ch, v0);
    quad[1].u = Tex_CornerUV(cutRight + glyphW, glyphW, texU);
    quad[1].v = Tex_CornerUV(topCut, ch, v0);
    quad[2].u = Tex_CornerUV(cutRight + glyphW, glyphW, texU);
    quad[2].v = Tex_CornerUV(cutBottom + ch, ch, v0);
    g_pPolyBin->Submit(&g_textGlyphPoly);
    quad[1].x = fx0;
    quad[1].y = yBottom;
    quad[1].u = Tex_CornerUV(trimL, glyphW, texU);
    quad[1].v = Tex_CornerUV(cutBottom + ch, ch, v0);
    g_pPolyBin->SubmitCallingTexFlush(&g_textGlyphPoly);
}

/* 0x535a88 - one string, not wrapped and not clipped to the window width: aligned by Text_MeasureLine, '\n' starts a
 * new line, colour codes switch the font colour, drawing stops at x = 512. */
void Text_DrawLineRaw(const char *text, u8 align)
{
    float z;
    s32 i;
    z = g_screen.Draw2D_LayerToZ(g_textLayer);
    switch (align) {
        case TEXTALIGN_LEFT:
            g_textCursorX = 0;
            break;
        case TEXTALIGN_RIGHT:
            g_textCursorX = g_textWinW - Text_MeasureLine(text) * g_pCurFont->glyphWidth;
            break;
        case TEXTALIGN_CENTER:
            g_textCursorX = (g_textWinW - Text_MeasureLine(text) * g_pCurFont->glyphWidth) >> 1;
            break;
    }
    i = 0;
    while (text[i] != 0 && g_textCursorX < 0x200) {
        if (text[i] == 1) {
            g_pCurFont->color = text[++i] + (text[++i] << 8) + (text[++i] << 16);
        } else if (text[i] == '\n') {
            g_textCursorX = 0;
            g_textCursorY += g_pCurFont->lineHeight;
        } else {
            Text_DrawString(text[i], g_textCursorX, g_textCursorY, z, g_pCurFont->color);
            g_textCursorX += g_pCurFont->glyphWidth;
        }
        i++;
    }
}

/* 0x535c31 - g_fonts[dst] = g_fonts[src] with another advance width and glyph height. */
void Font_CloneResized(u8 srcFontId, u8 dstFontId, u8 advancePlus1, u8 heightMinus1)
{
    g_fonts[dstFontId] = g_fonts[srcFontId];
    g_fonts[dstFontId].glyphWidth = advancePlus1 - 1;
    g_fonts[dstFontId].glyphHeight = heightMinus1 + 1;
}

/* 0x535c83 - makes g_fonts[fontId] the current font and loads it from the DAV bitmap of resource list resType, which
 * must hold exactly one entry; cellGeometry 1 is the 8x16-cell debug font, 2 the 16x16-cell game font. Enables text.
 * The list is dereferenced before its count is checked (a missing list would crash, not return -1). */
s8 Font_LoadFromRes(u8 fontId, u16 resType, u8 cellGeometry)
{
    DavBitmapRec *rec;
    u16 count;
    u16 *list;
    /* cast kept: an export id list holds record pointers of any kind; a font's is a bitmap index list */
    list = (u16 *)*Res_GetValidatedIdList(resType, &count);
    if (count != 1)
        return -1;
    rec = &g_pDav->header->dir->bitmaps[*list];
    g_pCurFont = &g_fonts[fontId];
    switch (cellGeometry) {
        case 1:
            g_pCurFont->glyphWidth = 7;
            g_pCurFont->glyphHeight = 0xf;
            g_pCurFont->cellW = 8;
            g_pCurFont->cellH = 0x10;
            g_pCurFont->cellWShift = 3;
            g_pCurFont->cellHShift = 4;
            g_pCurFont->lineHeight = 0x11;
            break;
        case 2:
            g_pCurFont->glyphHeight = 0xf;
            g_pCurFont->glyphWidth = 0xf;
            g_pCurFont->cellH = 0x10;
            g_pCurFont->cellW = 0x10;
            g_pCurFont->cellHShift = 4;
            g_pCurFont->cellWShift = 4;
            g_pCurFont->lineHeight = 0x11;
            break;
    }
    g_pCurFont->sheetU = rec->u;
    g_pCurFont->sheetV = rec->v;
    Text_ResetWindow();
    g_pCurFont->texPage = TexAtlas_GetPage(rec);
    g_pCurFont->reserved = 0;
    g_pCurFont->color = 0x4bccff;
    g_textEnabled = 1;
    return 0;
}

/* 0x535dd9 - empty, no callers. */
void Text_Stub_535dd9() {}

/* 0x535dde - sets the window (clamped to the 512x240 screen unless Text_SetNoClipOnce was called; a negative origin is
 * remembered in the clip offsets), refits the font's cells, homes the cursor. The last argument is not used. */
void Text_SetWindow(u32 *layer, s32 x, s32 y, s32 w, s32 h, u32 unused)
{
    g_textCursorX = g_textCursorY = 0;
    g_textClipOffX = 0;
    g_textClipOffY = 0;
    if (!g_textNoClip) {
        if (x < 0) {
            g_textClipOffX = x;
            x = 0;
        }
        if (y < 0) {
            g_textClipOffY = y;
            y = 0;
        }
        if (x + w > 0x200)
            w = (s16)(0x200 - x);
        if (y + h > 0xf0)
            h = (s16)(0xf0 - y);
    }
    g_textWinX = x;
    g_textWinY = y;
    g_textWinW = w;
    g_textWinH = h;
    g_pCurFont->cols = g_textWinW / g_pCurFont->glyphWidth;
    g_pCurFont->rows = g_textWinH / g_pCurFont->lineHeight;
    g_textLayer = layer;
    g_textNoClip = 0;
}

/* 0x535eef - empty. */
void Hud_EndBox_stub() {}

/* 0x535ef4 - the next Text_SetWindow skips the screen clamp and Text_Printf does not wrap. */
void Text_SetNoClipOnce()
{
    g_textNoClip = 1;
}

/* 0x535f03 - draws n characters of text as one line at the cursor row: align 0 = at the cursor, 1 = left, 2 = centred,
 * 3 = right (by the count of printable characters). Stops at the window's bottom; leaves the cursor after the text. */
void Text_EmitLine(const char *text, s32 n, u8 align)
{
    const u8 *p;
    u32 col;
    s32 idx;
    s32 printable = 0;
    s32 x;
    s32 y;
    float z;
    u8 ch;
    if (*text == 0 || n == 0)
        return;
    p = (const u8 *)text; /* cast kept: the text is const char *; it is read as unsigned bytes */
    for (x = 0; x < n; x++) {
        if (*p >= 0x20)
            printable++;
        if (*p == 1) {
            p += 3;
            x--;
        }
        p++;
    }
    switch (align) {
        case TEXTALIGN_CONTINUE:
            x = g_textCursorX;
            break;
        case TEXTALIGN_LEFT:
            x = 0;
            break;
        case TEXTALIGN_CENTER:
            x = (g_textWinW - printable * g_pCurFont->glyphWidth) / 2;
            break;
        case TEXTALIGN_RIGHT:
            x = g_textWinW - printable * g_pCurFont->glyphWidth;
            break;
    }
    y = g_textCursorY;
    if (y >= g_textWinH)
        return;
    col = g_pCurFont->color;
    z = g_screen.Draw2D_LayerToZ(g_textLayer);
    for (idx = 0; idx < n; idx++) {
        ch = *text++;
        if (ch == '\n') {
            y += g_pCurFont->lineHeight;
            x = 0;
        } else if (ch == ' ') {
            x += g_pCurFont->glyphWidth;
        } else if (ch == 1) {
            col = 0;
            ch = *text++;
            col = ch;
            ch = *text++;
            col |= ch << 8;
            ch = *text++;
            col |= ch << 16;
            idx--;
        } else {
            Text_DrawString(ch, x, y, z, col);
            x += g_pCurFont->glyphWidth;
        }
    }
    g_pCurFont->color = col;
    g_textCursorX = x;
    g_textCursorY = y;
}

/* 0x536173 */
void Text_EmitLineThunk(const char *text, s32 n, u8 align)
{
    Text_EmitLine(text, n, align);
}

/* 0x53618c - the no-wrap path of Text_Printf: emits the text word run by word run, split only at control bytes below
 * 0x20 (newlines) and the end, and starts a new line after a run once the column count reaches the font's cols. */
/* BYTES(dead-code): d and savedPos are stored and never read, as in the original */
void Text_DrawNoWrap(const char *text, u8 align)
{
    const char *line = text;
    s32 emit;
    s32 d;
    s32 cnt;
    u8 ch;
    s32 len;
    s32 savedPos;
    s32 pos;
    savedPos = 0;
    pos = 0;
    len = 0;
    d = 0;
    cnt = 0;
    emit = 0;
    if (*text == 0)
        return;
    do {
        line += len;
        do {
            d = cnt;
            savedPos = pos;
            do {
                ch = line[pos];
                pos++;
                cnt++;
                if (ch == 1) {
                    pos += 3;
                    ch = 0x21;
                    cnt--;
                }
            } while (ch > 0x20);
        } while (ch >= 0x20);
        emit = cnt;
        len = pos;
        if (ch == 0) {
            emit--;
            len--;
        }
        Text_EmitLineThunk(line, emit, align);
        if (line[len - 1] != '\n' && cnt >= g_pCurFont->cols) {
            g_textCursorX = 0;
            g_textCursorY += g_pCurFont->lineHeight;
        }
        cnt = 0;
        pos = 0;
    } while (line[len] != 0);
}
