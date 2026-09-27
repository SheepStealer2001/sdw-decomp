/* T278 - original object "Map.cpp" (guessed name): the Map / select-menu screen 0x53f430-0x5428bf.
 *   .text  0x53f430-0x5428bf  (37 functions)
 *   .data  0x57c07c-0x57c128  g_strNoHelpAvailable, then the five $SG literals of Map_UpdateState / Map_Draw
 *   .bss   0x6de368-0x6de9a0  g_pMapLocation, s_mapFooterRect, 516 unreferenced bytes, g_map, g_mapFooterDefault
 *  - The .bss globals are defined here and written `= 0` / `= { 0 }`: VC6 puts a file's globals without an initialiser
 *    first, in the order of a hash of their names, and the initialised ones after them in definition order; the four
 *    names hash out of the exe's order, so all are initialised, and they come out in definition (= address) order.
 *  - Map::GetHelpText returns g_strNoHelpAvailable (0x57c07c), a named array rather than the literal "no help
 *    available": in the exe that string comes FIRST in the object's .data, ahead of the literals of the earlier
 *    functions, and VC6 /Od emits a file's named initialised data before all of its $SG literals (the literals in the
 *    order the code meets them). The code bytes are the same either way (mov eax, OFFSET ...).
 */
/* BYTES: dead-code, inline, layout, slot-name, view. */
/* BYTES(layout): written '= 0' only to keep definition order in .bss */
/* BYTES(layout): placeholder: 516 unreferenced bytes kept only for the layout (a word, then 512 bytes: one item would be 8-aligned) */
/* BYTES(view): the #defines map the old spellings to the defined names; no body changed */
/* BYTES(inline): Map::ItemSlot (inline): source-only inline: its expansion gives the original's shape */
/* BYTES(inline): Map::ActionSlot (inline): source-only inline: its expansion gives the original's shape */
/* BYTES(inline): Map::RowToSlot (inline): source-only inline: its expansion gives the original's shape */
/* BYTES(inline): SetShown (inline): source-only inline: its expansion gives the original's shape */
/* PAL PC 0x53f430-0x5428bf: the Map / inventory screen (the single Map object g_map 0x6de578) and the select-menu
 * wipe that opens and closes it (37 functions).
 * Layout facts this file relies on (data/structs): UiIcon is 0x44 bytes (the Map's two UiIcon arrays have a 0x44
 * stride: imul 0x44 at 0x53fe64, 0x5410f3, 0x541bed; itemSlots = actionSlots + 4 * 0x44 = 0x304), with u8 enabled +0 /
 * highlighted +1; Map +0x4 is a ScrollText scrollText, +0x5b the s8 scrollDir, and pulsePhaseFast/Slow (+0x34/+0x36) are
 * s16.
 * Shapes that reproduce the bytes (representations, not proof of the original spelling): the one-line inline
 * helpers below (ItemSlot, ActionSlot, RowToSlot, SetShown, GameFlags_Clear/Set, Wolf_GetHeldObject, PropU32) and
 * the local names chosen for their stack slots, noted per function.
 */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
#include "../sdk/ddraw.h"


#define SDW_MEMBERS_UiIcon \
    void SetShown(s32 on)  \
    {                      \
        highlighted = on;  \
        enabled = on;      \
    }

/* ScrollText's methods under the names T275 (interface.cpp) defines them by, Init(char *, ...) and Draw; the calls
 * below spell them ScrollText_Init / ScrollText_Draw through the two #defines after the class header. */
#define SDW_MEMBERS_Texture \
    long Surface_LockForWrite(DDSURFACEDESC2 *desc); /* returns long, as T009 (texture.cpp) defines it */
/* Screen's scale methods under the names T262 (screen.cpp) defines them by (ScaleX / ScaleY); the spelling
 * Screen_ScaleX / Screen_ScaleY used below is mapped to them after the class header. */
#define SDW_MEMBERS_Map                     \
    s32 InWindow(s16 rel)                   \
    {                                       \
        if (rel >= 0 && rel < visibleCount) \
            return 1;                       \
        return 0;                           \
    }                                       \
    UiIcon *ItemSlot(s16 slot)              \
    {                                       \
        return &itemSlots[slot];            \
    }                                       \
    UiIcon *ActionSlot(s16 index)           \
    {                                       \
        return &actionSlots[index];         \
    }                                       \
    s16 RowToSlot(s16 row)                  \
    {                                       \
        return row - scrollTop;             \
    }
#include "sdw_classes.h"
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_GETFLAGS 1
#include "scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_GETFLAGS
#define SDW_INLINE_UIQUAD_SETCOLOR_U32 1
#define SDW_INLINE_UIQUAD_SETFADELEVEL_U8 1
#include "ui_quad_inlines.h"
#undef SDW_INLINE_UIQUAD_SETCOLOR_U32
#undef SDW_INLINE_UIQUAD_SETFADELEVEL_U8
#define SDW_INLINE_UIICON_SETENABLED_S32 1
#define SDW_INLINE_UIICON_SETHIGHLIGHTED_S32 1
#include "ui_icon_inlines.h"
#undef SDW_INLINE_UIICON_SETENABLED_S32
#undef SDW_INLINE_UIICON_SETHIGHLIGHTED_S32
#define SDW_INLINE_UIICON_SETCOLOR_U32 1
#include "ui_icon_inlines.h"
#undef SDW_INLINE_UIICON_SETCOLOR_U32
#define ScrollText_Init Init
#define ScrollText_Draw Draw
#define Screen_ScaleX ScaleX
#define Screen_ScaleY ScaleY

/* ---- globals ---- */
extern "C" const s16 *g_pCosTable; /* 0x5814e4 (C: T305 defines _g_pCosTable) */
extern u32 g_gameFlags;            /* 0x6ddf74 */
extern s32 g_dtRawMs;              /* 0x71b2d8 */
#include "../app/app_main.h"
#include "scenaric.h"
#include "text.h"
#include "screen.h"
#include "draw2d.h"
#include "game_state.h"
#include "interface.h"
#include "fixed_math.h"
#include "lerp.h"
#include "id_list.h"
#include "sound_mgr.h"
#include "input.h"
#include "prompt.h"
#include "scn_tools.h"
#include "progress_inventory.h"
extern Wolf *g_pWolf;          /* 0x6cf310 */
extern u32 *g_screenLayerBase; /* 0x585044 */

/* ---- this object's data, in address order ---- */
/* 0x57c07c - Map::GetHelpText's fallback (named data: see the header). */
char g_strNoHelpAvailable[] = "no help available";
/* .bss, all initialised to zero so that they keep this (definition) order: see the header. */
ScnObject *g_pMapLocation = 0; /* 0x6de368 */
/* The rectangle of the footer hint line (x 0x60, y 0xd9, w 0x170, h 0xf), filled in by Map_InitObject and passed to
 * Text_SetWindowRect by Map_Draw. 0x6de36c, right after g_pMapLocation; the tables do not name it. */
static s16 s_mapFooterRect[4] = {0};
/* 0x6de374-0x6de578: 516 bytes of this object's .bss that no instruction refers to (tools/tu_sheet.py). Genuinely
 * opaque: nothing reads or writes them, so neither their type nor their name can be recovered; kept as bytes so that
 * g_map lands at its address and the object's .bss has its original size. VC6 8-aligns every item of 64 bytes or more,
 * and 0x6de374 is not 8-aligned, so the block cannot be one item: it is written as a word and a 512-byte array
 * (0x6de378, 8-aligned); how the original divided it is not known. */
u32 g_mapUnref_6de374 = 0;
u8 g_mapUnref_6de378[0x200] = {0};
Map g_map = {0}; /* 0x6de578 */
/* 0x6de98c-0x6de99c: 16 more bytes nothing refers to. Either Map is really 0x424 bytes (the exe has g_mapFooterDefault
 * 0x424 after g_map, the generated struct is 0x414) or another unreferenced item sat here; opaque, kept as bytes. */
u8 g_mapUnref_6de98c[16] = {0};
char g_mapFooterDefault[4] = {0}; /* 0x6de99c  the footer text shown by default: "" */

/* ---- callees ---- */
void SelectMenu_SetState(u8 state);
u8 SelectMenu_GetState();
s32 Scenaric_FindByClass(u16 classId, ScnObject **out, s32 max);
u32 *Res_GetValidatedIdList(u16 resId, u16 *outCount);
extern "C" u32 Rgb24_Lerp(u32 a, u32 b, s16 t); /* 0x52795c (C: T258 defines _Rgb24_Lerp) */
void Draw2D_TexRect(float z, float x0, float y0, float x1, float y1, s32 texIndex, float uTL, float vTL, u32 cTL,
                    float uBL, float vBL, u32 cBL, float uTR, float vTR, u32 cTR, float uBR, float vBR, u32 cBR);
u16 Map_ResolvePropExportId(void *levelRecord, u16 propOffset);
void Map_DrawWipeBar(u16 y, s16 height);

/* BYTES(inline): source-only inline: its expansion gives the original's shape */
#define SDW_INLINE_FREE_GAMEFLAGS_CLEAR_U32 1
#include "game_state_inlines.h"
#undef SDW_INLINE_FREE_GAMEFLAGS_CLEAR_U32
/* BYTES(inline): source-only inline: its expansion gives the original's shape */
inline void GameFlags_Set(u32 mask)
{
    g_gameFlags |= mask;
}
/* BYTES(inline): source-only inline: its expansion gives the original's shape */
#define SDW_INLINE_FREE_SCREENWIDTHS16 1
#include "screen_inlines.h"
#undef SDW_INLINE_FREE_SCREENWIDTHS16
#define SDW_INLINE_FREE_SCREENHEIGHTS16 1
#include "screen_inlines.h"
#undef SDW_INLINE_FREE_SCREENHEIGHTS16
/* BYTES(inline): source-only inline: its expansion gives the original's shape */
#define SDW_INLINE_FREE_PROPU32_VOID_U32 2
#include "scn_tools_inlines.h"
#undef SDW_INLINE_FREE_PROPU32_VOID_U32
/* BYTES(inline): source-only inline: its expansion gives the original's shape */
#define SDW_INLINE_FREE_SCENARIC_CLASSFLAGS_U16 1
#include "scenaric_inlines.h"
#undef SDW_INLINE_FREE_SCENARIC_CLASSFLAGS_U16
/* BYTES(inline): source-only inline: its expansion gives the original's shape */
inline ScnObject *Wolf_GetHeldObject(Wolf *wolf)
{
    if (wolf->mode != WOLF_MODE_CARRY)
        return wolf->heldObject;
    return 0;
}

/* 0x53f430 */
void Map_Init()
{
    g_map.InitObject();
}

/* 0x53f43f - builds the map screen for this level: the inventory rows (every class flagged 0x400 and not 0x200), the
 * resource lists, the Mailbox markers, the action labels, the five backdrop quads, the list arrows and the four
 * action buttons. */
/* BYTES(slot-name): names chosen for their stack slots: ptr -4, mail -0x18, slot -0x1a, count -0x1c, mapResId -0x1e, bmps -0x24, mapBitmap -0x28, h2 -0x2a, u0 -0x2b, baseV -0x2c, resIds -0x48, props -0x4c */
void Map::InitObject()
{
    /* names chosen for their stack slots (src/README.md): the original's frame is ptr -4, mail -0x18, slot -0x1a,
     * count -0x1c, mapResId -0x1e, bmps -0x24, mapBitmap -0x28, h2 -0x2a, u0 -0x2b, baseV -0x2c, resIds -0x48,
     * props -0x4c */
    void *props;
    u16 resIds[13];
    u8 baseV;
    u8 u0;
    s16 h2;
    u16 *mapBitmap;
    u8 *bmps;
    u16 mapResId;
    u16 count;
    u16 slot;
    ScnObject *mail[5];
    u16 **ptr;
    resIds[0] = DAV_IDI_IGLHELPB;
    resIds[1] = DAV_IDI_IGLHELPA;
    resIds[2] = DAV_IDI_IGLUSEDB;
    resIds[3] = DAV_IDI_IGLUSEDA;
    resIds[4] = DAV_IDI_IGLCOMBB;
    resIds[5] = DAV_IDI_IGLCOMBA;
    resIds[6] = DAV_IDI_IGLEXITB;
    resIds[7] = DAV_IDI_IGLEXITA;
    resIds[8] = DAV_IDI_IGLCADG_;
    resIds[9] = DAV_IDI_IGLFLEC_;
    resIds[10] = DAV_IDI_IGLCADH_;
    resIds[11] = DAV_IDI_IGLCERC_;
    resIds[12] = DAV_IDI_IGLTRIA_;
    mapResId = DAV_IDI_MAP;
    SelectMenu_SetState(SEL_SUPPRESSED);
    locationCount = 0;
    itemCount = 0;
    slot = 0;
    for (itemCount = 0; slot < g_scnObjectCount; slot++) {
        inventoryClassIds[itemCount] = CLASSID_NONE;
        if ((Scenaric_ClassFlags(g_scnObjects[slot]->GetClassId()) & SCN_CF_INVENTORY_ITEM) &&
            !(Scenaric_ClassFlags(g_scnObjects[slot]->GetClassId()) & SCN_CF_COMPOSITE_ITEM))
            FindOrAddRow(g_scnObjects[slot]->GetClassId());
    }
    for (slot = 0; slot < 13; slot++) {
        ptr = (u16 **)Res_GetValidatedIdList(resIds[slot], &count); /* cast kept: an id list of bitmap records */
        if (ptr)
            resIcons[slot] = *ptr;
        else
            resIcons[slot] = 0;
    }
    ptr = (u16 **)Res_GetValidatedIdList(mapResId, &count); /* cast kept: an id list of bitmap records */
    if (!ptr)
        return;
    mapBitmap = *ptr;
    locationCount = Scenaric_FindByClass(CLASSID_MAILBOX, mail, 5);
    if (locationCount) {
        for (slot = 0; slot < locationCount; slot++) {
            props = mail[slot]->record;
            markers[slot].mapX = PropU32(props, 0x1c) << 1;
            markers[slot].mapY = PropU32(props, 0x20);
            markers[slot].acceptedClassIds[0] = Map_ResolvePropExportId(props, 0x24);
            markers[slot].acceptedClassIds[1] = Map_ResolvePropExportId(props, 0x28);
            markers[slot].acceptedClassIds[2] = Map_ResolvePropExportId(props, 0x2c);
            markers[slot].acceptedClassIds[3] = Map_ResolvePropExportId(props, 0x30);
        }
        for (slot = 0; slot < locationCount; slot++) {
            markers[slot].sprite.UiQuad_SetFromBitmap(resIcons[11], markers[slot].mapX, markers[slot].mapY, 1, 1, 0x400,
                                                      0x400);
            markers[slot].sprite.SetColor(0x808080);
        }
    }
    actionText[1] = Text_GetUiString(UISTR_USE);
    actionText[0] = Text_GetUiString(UISTR_HELP);
    actionText[2] = Text_GetUiString(UISTR_COMBINE);
    actionText[3] = Text_GetUiString(UISTR_EXIT);
    actionTextSplit = Text_GetUiString(UISTR_TAKE_APART);
    visibleCount = 4;
    if (visibleCount > itemCount)
        visibleCount = itemCount;
    ResetSelection();
    SelectMenu_SetState(SEL_CLOSED);
    g_pMapLocation = 0;
    Scenaric_FindByClass(CLASSID_MAPLOCATION, &g_pMapLocation, 1);
    s_mapFooterRect[0] = 0x60;
    s_mapFooterRect[1] = 0xd9;
    s_mapFooterRect[2] = ScreenWidthS16() - 0x90;
    s_mapFooterRect[3] = 0xf;
    /* cast kept: g_pDav->header->dir->bitmaps read as bytes (DavDirectory is packed and declared in load_dav.cpp), and
     * the low bytes of a 10-byte record's u and v taken below */
    bmps = *(u8 **)((u8 *)*(void **)((u8 *)*(void **)g_pDav + 0x14) + 10);
    u0 = bmps[*mapBitmap * 10];
    baseV = bmps[*mapBitmap * 10 + 4];
    backdrop[0].UiQuad_SetFromBitmap(mapBitmap, 0, 0, 0, 0, 0x800, 0x400);
    backdrop[0].h = 0x30;
    backdrop[1].UiQuad_SetFromBitmap(mapBitmap, 0, 0x30, 0, 0, 0x800, 0x400);
    backdrop[1].w = 0x60;
    backdrop[1].h = 0xa1;
    backdrop[2].UiQuad_SetFromBitmap(mapBitmap, (s16)(ScreenWidthS16() - 0x30), 0x30, 0, 0, 0x800, 0x400);
    backdrop[2].w = 0x30;
    backdrop[2].h = 0xa1;
    backdrop[4].UiQuad_SetFromBitmap(mapBitmap, 0, 0xd1, 0, 0, 0x800, 0x400);
    backdrop[4].h = 0x1f;
    backdrop[3].UiQuad_SetFromBitmap(mapBitmap, 0x60, 0x30, 0, 0, 0x800, 0x400);
    backdrop[3].w = ScreenWidthS16() - 0x90;
    backdrop[3].h = 0xa1;
    backdrop[0].u = u0;
    backdrop[0].v = baseV;
    backdrop[0].wMinus1 = 0xff;
    backdrop[0].hMinus1 = 0x30;
    backdrop[1].u = u0;
    backdrop[1].v = baseV + 0x30;
    backdrop[1].wMinus1 = 0x30;
    backdrop[1].hMinus1 = 0xa1;
    backdrop[2].u = 0xe7;
    backdrop[2].v = baseV + 0x30;
    backdrop[2].wMinus1 = 0x18;
    backdrop[2].hMinus1 = 0xa1;
    backdrop[4].u = u0;
    backdrop[4].v = baseV + 0xd1;
    backdrop[4].wMinus1 = 0xff;
    backdrop[4].hMinus1 = 0x1f;
    backdrop[3].u = u0 + 0x30;
    backdrop[3].v = baseV + 0x30;
    backdrop[3].wMinus1 = 0xb7;
    backdrop[3].hMinus1 = 0xa1;
    backdrop[0].SetColor(0x808080);
    backdrop[1].SetColor(0x808080);
    backdrop[2].SetColor(0x808080);
    backdrop[3].SetColor(0x808080);
    backdrop[4].SetColor(0x808080);
    listBaseY = 0x80;
    h2 = visibleCount * 0x24 / 2;
    scrollArrow[0].UiQuad_SetFromBitmap(resIcons[9], 0x18, (s16)(listBaseY - h2 - 2), 0, 0, 0x400, 0x400);
    scrollArrow[0].y -= scrollArrow[0].h;
    scrollArrow[0].SetColor(0x808080);
    scrollArrow[1].UiQuad_SetFromBitmap(resIcons[9], 0x18, (s16)(listBaseY + h2), 0, 0, 0x400, 0x400);
    scrollArrow[1].UiQuad_Mirror(1, 1);
    scrollArrow[1].SetColor(0x808080);
    listBaseY -= h2;
    for (slot = 0; slot < 4; slot++)
        actionSlots[slot].UiIcon_Setup(resIcons[10], resIcons[slot * 2], resIcons[slot * 2 + 1], slot * 100 + 0x60,
                                       0x10, 0x400, 0x400, 1);
}

/* 0x53fe7f */
void Map_Update()
{
    g_map.UpdateState();
}

/* 0x53fe8e - one full-width curtain bar of the open/close wipe */
void Map_DrawWipeBar(u16 y, s16 height)
{
    if (height > 0)
        Draw2D_FlatRect(g_screen.Draw2D_LayerToZ(g_screenLayerBase + 3), g_screen.Screen_ScaleX(0),
                        g_screen.Screen_ScaleY(y), g_screen.Screen_ScaleX(0x200),
                        g_screen.Screen_ScaleY(y + (u16)height), Color_RgbToBgrHalved(0xa05050), 0);
}

/* 0x53ff1a - the translucent description panel: a 4x4 ARGB4444 texel block of 0xB05050 at alpha 4, stretched over rect,
 * then a one-pixel 0xB0B0B0 outline */
/* BYTES(dead-code): stride is never used: it only takes the slot at ebp-0x10 */
void Map_DrawPanel(s16 *rect)
{
    s32 k;
    Texture *tex;
    s32 y;
    s32 stride; /* declared and never used: it only takes the slot at ebp-0x10 */
    DDSURFACEDESC2 *desc;
    u32 argb;
    u8 *texels;
    tex = g_pPolyBin->textures[g_pPolyBin->texturePageCount - 4];
    desc = new DDSURFACEDESC2;
    tex->Surface_LockForWrite(desc);
    texels = (u8 *)desc->lpSurface; /* cast kept: a locked surface is raw memory in the texture's pixel format */
    argb = Color_RgbToBgrHalved(0xb05050) + 0x40000000;
    for (y = 0; y < 4; y++)
        for (k = 0; k < 4; k++)
            /* cast kept: row y of the locked ARGB4444 surface, lPitch bytes apart */
            ((u16 *)(y * desc->lPitch + (u32)texels))[k] =
                ((argb >> 4) & 0xf) | ((argb >> 8) & 0xf0) | ((argb >> 12) & 0xf00) | ((argb >> 16) & 0xf000 & 0xffff);
    tex->Surface_Unlock();
    delete desc;
    Draw2D_TexRect(g_screen.Draw2D_LayerToZ(g_screenLayerBase + 3), g_screen.Screen_ScaleX(rect[0]),
                   g_screen.Screen_ScaleY(rect[1]), g_screen.Screen_ScaleX(rect[0] + rect[2]),
                   g_screen.Screen_ScaleY(rect[1] + rect[3]), g_pPolyBin->texturePageCount - 4, 1.0f, 1.0f, 0x888, 1.0f,
                   1.0f, 0x888, 1.0f, 1.0f, 0x888, 1.0f, 1.0f, 0x888);
    Ui_DrawRectOutline(rect, 0xb0b0b0);
}

/* 0x5400f2 - the map is open or mid-transition (select-menu states 2..5) */
s32 Map_IsOpen()
{
    return SelectMenu_GetState() >= SEL_OPEN_HOLD && SelectMenu_GetState() <= SEL_CLOSE_WIPE;
}

/* 0x540128 - a Mailbox O1..O4 property: export id -> the class id of the object it names, 0xffff when unresolved */
u16 Map_ResolvePropExportId(void *levelRecord, u16 propOffset)
{
    u32 *found;
    u16 num;
    u16 exportId;
    exportId = PropU32(levelRecord, propOffset);
    found = Scn_FindIdList(exportId, &num);
    if (found == 0 || *found == 0)
        return CLASSID_NONE;
    return *(u16 *)(*found + 10); /* cast kept: the entry is the object's level record address; +0xa is its class id */
}

/* 0x540179 - entered from the wipe once the curtain covers the screen: state 2, world update and render off */
void Map::Open()
{
    ScnObject *held;
    if (state != SEL_SUPPRESSED) {
        ResetSelection();
        state = SEL_OPEN_HOLD;
        GameFlags_Clear(GF_BIT0 | GF_UPDATE_OBJECTS | GF_RENDER_WORLD);
        Text_SetFont(FONT_GAME);
        Text_ResetWindow();
        SortRowsByOwned();
        Sound_PauseAll();
        held = Wolf_GetHeldObject(g_pWolf);
        if (held && (Scenaric_ClassFlags(held->GetClassId()) & SCN_CF_INVENTORY_ITEM))
            cursorIndex = FindRow(held->GetClassId());
        prevActionIndex = 0;
        actionIndex = MAP_ACT_HELP;
        prevCursorIndex = 0;
        combinePartnerIndex = 0;
        cursorIndex = 0;
        scrollTop = 0;
        selectedClassId = inventoryClassIds[cursorIndex];
        helpText = GetHelpText();
    }
}

/* 0x5402a9 - world update and render back on, font fade bits cleared, audio resumed */
void Map::SelectMenu_RestoreFlags()
{
    if (state != SEL_SUPPRESSED) {
        g_gameFlags |= GF_UPDATE_OBJECTS | GF_RENDER_WORLD;
        Text_SetFont(FONT_GAME);
        g_pCurFont->reserved &= (u16)~UIQUAD_FADE_MASK;
        Text_ResetWindow();
        Sound_ResumeAll();
    }
}

/* 0x5402ff - the map screen's state machine and input: the opening hold (2), the interactive state (3) with its four
 * modes (1 item list, 2 action row, 3 combine pick, 4 help scroll; bit 0x80 = the panel is sliding to the next mode),
 * the closing hold (4) and the closing wipe (5); then the draw. */
/* BYTES(slot-name): names chosen for their stack slots: label -4, k -6, box -0x10, ok -0x14 */
void Map::UpdateState()
{
    /* names chosen for their stack slots (src/README.md): label -4, k -6, box -0x10, ok -0x14 */
    s32 ok;
    s16 box[4];
    u16 k;
    char *label;
    switch (state) {
        case SEL_OPEN_HOLD:
            if (phaseCounter > 0)
                phaseCounter--;
            else
                state = SEL_INTERACTIVE;
            break;
        case SEL_CLOSE_HOLD:
            phaseCounter++;
            if (phaseCounter == 0xf) {
                state = SEL_CLOSE_WIPE;
                mode = MAP_MODE_ITEM_LIST;
                closeCounter = 0;
                Map_DrawWipeBar(0, 0xf0);
            }
            break;
        case SEL_CLOSE_WIPE:
            closeCounter++;
            if (closeCounter == 5)
                SelectMenu_RestoreFlags();
            Map_DrawWipeBar(0, 0xf0);
            if (closeCounter == 10) {
                closeCounter = 0;
                state = SEL_CLOSE_TAIL;
            }
            break;
    }
    if (state == SEL_INTERACTIVE && visibleCount) {
        if (Pad_MenuPressed((u16)~PAD_TRIANGLE)) {
            Ui_PlayCancelSound();
            switch (mode) {
                case MAP_MODE_ITEM_LIST:
                    state = SEL_CLOSE_HOLD;
                    break;
                case MAP_MODE_ACTION_ROW:
                    SetPanelRect(panelFrom, &actionSlots[actionIndex].mainQuad, 0, 0);
                    SetPanelRect(panelTo, &ItemSlot(cursorIndex - scrollTop)->mainQuad, 0, 0);
                    BeginModeTransition(MAP_MODE_ITEM_LIST);
                    prevActionIndex = actionIndex;
                    break;
                case MAP_MODE_HELP_SCROLL:
                    prevActionIndex = actionIndex;
                    SetPanelRect(panelFrom, &backdrop[3], 0, 0);
                    SetPanelRect(panelTo, &actionSlots[actionIndex].mainQuad, 0, 0);
                    BeginModeTransition(MAP_MODE_ACTION_ROW);
                    break;
                case MAP_MODE_COMBINE_PICK:
                    swapPending = 0;
                    prevActionIndex = actionIndex;
                    mode = MAP_MODE_ACTION_ROW;
                    break;
            }
        }
        if (Pad_MenuPressed((u16)~PAD_CROSS)) {
            Ui_PlayConfirmSound();
            switch (mode) {
                case MAP_MODE_ITEM_LIST:
                    if (swapPending)
                        CombineItems();
                    else
                        actionIndex = MAP_ACT_HELP;
                    SetPanelRect(panelFrom, &ItemSlot(cursorIndex - scrollTop)->mainQuad, 0, 0);
                    SetPanelRect(panelTo, &actionSlots[actionIndex].mainQuad, 0, 0);
                    BeginModeTransition(MAP_MODE_ACTION_ROW);
                    break;
                case MAP_MODE_ACTION_ROW:
                    switch (actionIndex) {
                        case MAP_ACT_USE:
                            if (ItemSlot(cursorIndex - scrollTop)->enabled &&
                                ItemSlot(cursorIndex - scrollTop)->highlighted) {
                                Inventory_SelectClass(selectedClassId);
                                g_pWolf->SwapHeldItem(0, 1);
                                state = SEL_CLOSE_HOLD;
                                mode = MAP_MODE_ITEM_LIST;
                            }
                            break;
                        case MAP_ACT_HELP:
                            helpText = GetHelpText();
                            box[0] = g_map.backdrop[3].x + 4;
                            box[1] = g_map.backdrop[3].y + 4;
                            box[2] = g_map.backdrop[3].w - 8;
                            box[3] = g_map.backdrop[3].h - 8;
                            scrollText.ScrollText_Init(helpText, box);
                            SetPanelRect(panelFrom, &actionSlots[actionIndex].mainQuad, 0, 0);
                            SetPanelRect(panelTo, &backdrop[3], 0, 0);
                            BeginModeTransition(MAP_MODE_HELP_SCROLL);
                            break;
                        case MAP_ACT_COMBINE:
                            if (ItemSlot(cursorIndex - scrollTop)->enabled &&
                                ItemSlot(cursorIndex - scrollTop)->highlighted) {
                                if (IsSelectedItemAssembled()) {
                                    SplitItem();
                                    mode = MAP_MODE_ACTION_ROW;
                                } else {
                                    combinePartnerIndex = cursorIndex;
                                    mode = MAP_MODE_COMBINE_PICK;
                                }
                            }
                            break;
                        case MAP_ACT_EXIT:
                            state = SEL_CLOSE_HOLD;
                            break;
                    }
                    break;
            }
        }
        if (mode & MAP_MODE_F_TRANSIENT) {
            autoRepeatMs += (u16)g_dtRawMs;
            if (Pad_MenuPressed((u16)~PAD_UP) || Pad_MenuPressed((u16)~PAD_DOWN) || Pad_MenuPressed((u16)~PAD_LEFT) ||
                Pad_MenuPressed((u16)~PAD_RIGHT))
                autoRepeatMs = 0x96;
            if (autoRepeatMs >= 0x96)
                mode = pendingMode;
        }
        switch (mode) {
            case MAP_MODE_ITEM_LIST:
                if (Pad_MenuRepeat((u16)~PAD_UP)) {
                    CursorPrev();
                    if (cursorIndex != prevCursorIndex) {
                        Ui_PlayMoveSound();
                        SetPanelRect(panelFrom, &ItemSlot(prevCursorIndex - scrollTop)->mainQuad, 0, 0);
                        SetPanelRect(panelTo, &ItemSlot(cursorIndex - scrollTop)->mainQuad, 0, 0);
                        BeginModeTransition(MAP_MODE_NONE);
                    }
                } else if (Pad_MenuRepeat((u16)~PAD_DOWN)) {
                    CursorNext();
                    if (cursorIndex != prevCursorIndex) {
                        Ui_PlayMoveSound();
                        SetPanelRect(panelFrom, &ItemSlot(prevCursorIndex - scrollTop)->mainQuad, 0, 0);
                        SetPanelRect(panelTo, &ItemSlot(cursorIndex - scrollTop)->mainQuad, 0, 0);
                        BeginModeTransition(MAP_MODE_NONE);
                    }
                }
                if (IsRowVisible(cursorIndex)) {
                    ok = 0;
                    if (g_pWolf->CanSwapItem(1) && ItemSlot(cursorIndex - scrollTop)->enabled)
                        ok = 1;
                    actionSlots[1].enabled = ok;
                    actionSlots[2].enabled = ok;
                }
                g_uiFooterText =
                    Text_Sprintf(g_menuFooterText, "$B_VALID$ %s, $B_CANCEL$ %s", g_strValidate, g_strCancel);
                break;
            case MAP_MODE_ACTION_ROW:
                if (Pad_MenuRepeat((u16)~PAD_LEFT)) {
                    ActionPrev();
                    if (actionIndex != prevActionIndex) {
                        Ui_PlayMoveSound();
                        SetPanelRect(panelFrom, &actionSlots[prevActionIndex].mainQuad, 0, 0);
                        SetPanelRect(panelTo, &actionSlots[actionIndex].mainQuad, 0, 0);
                        BeginModeTransition(MAP_MODE_NONE);
                    }
                } else if (Pad_MenuRepeat((u16)~PAD_RIGHT)) {
                    ActionNext();
                    if (actionIndex != prevActionIndex) {
                        Ui_PlayMoveSound();
                        SetPanelRect(panelFrom, &actionSlots[prevActionIndex].mainQuad, 0, 0);
                        SetPanelRect(panelTo, &actionSlots[actionIndex].mainQuad, 0, 0);
                        BeginModeTransition(MAP_MODE_NONE);
                    }
                }
                label = actionText[actionIndex];
                if (actionIndex == MAP_ACT_COMBINE && IsSelectedItemAssembled())
                    label = actionTextSplit;
                g_uiFooterText = Text_Sprintf(g_menuFooterText, "$B_VALID$ %s, $B_CANCEL$ %s", label, g_strCancel);
                break;
            case MAP_MODE_HELP_SCROLL:
                scrollDir = 0;
                if (Pad_MenuHeld((u16)~PAD_UP))
                    scrollDir = -1;
                else if (Pad_MenuHeld((u16)~PAD_DOWN))
                    scrollDir = 1;
                scrollText.ScrollList_Update(scrollDir);
                g_uiFooterText = Text_Sprintf(g_menuFooterText, "$B_UP$$B_DOWN$, $B_CANCEL$ %s", g_strCancel);
                break;
            case MAP_MODE_COMBINE_PICK:
                swapPending = 1;
                mode = MAP_MODE_ITEM_LIST;
                break;
        }
        ActionSlot(prevActionIndex)->SetColor(0x808080);
        ActionSlot(prevActionIndex)->SetHighlighted(0);
        for (k = 0; k < visibleCount; k++)
            itemSlots[k].mainQuad.SetColor(0x808080);
        if (swapPending) {
            ActionSlot(actionIndex)->SetColor(0x40c040);
            ActionSlot(actionIndex)->SetHighlighted(1);
            if (IsRowVisible(combinePartnerIndex))
                ItemSlot(combinePartnerIndex - scrollTop)->SetColor(0x40c040);
        }
        if (!(mode & MAP_MODE_F_TRANSIENT)) {
            if (mode == MAP_MODE_ACTION_ROW)
                ItemSlot(cursorIndex - scrollTop)->SetColor(0x40c040);
            else
                ItemSlot(cursorIndex - scrollTop)->SetColor(pulseColorFast);
        }
        switch (mode) {
            case MAP_MODE_ACTION_ROW:
                ActionSlot(actionIndex)->SetColor(pulseColorFast);
                ActionSlot(actionIndex)->SetHighlighted(1);
                break;
            case MAP_MODE_COMBINE_PICK:
            case MAP_MODE_HELP_SCROLL:
                ActionSlot(actionIndex)->SetColor(0x40c040);
                ActionSlot(actionIndex)->SetHighlighted(1);
                break;
        }
    }
    if (state >= SEL_OPEN_HOLD && state <= SEL_CLOSE_HOLD)
        Draw();
}

/* 0x541019 */
void Map::ResetSelection()
{
    u16 i;
    swapPending = 0;
    prevActionIndex = 0;
    actionIndex = MAP_ACT_HELP;
    prevCursorIndex = 0;
    combinePartnerIndex = 0;
    cursorIndex = 0;
    scrollTop = 0;
    helpTimerMs = 3000;
    markerTimerMs = 6000;
    pulsePhaseFast = 0;
    pulsePhaseSlow = 0;
    wipeHeight = 0;
    selectedClassId = inventoryClassIds[cursorIndex];
    helpText = GetHelpText();
    phaseCounter = 0xf;
    mode = MAP_MODE_ITEM_LIST;
    g_uiFooterText = g_mapFooterDefault;
    for (i = 0; i < 4; i++) {
        actionSlots[i].SetEnabled(1);
        actionSlots[i].SetHighlighted(0);
        actionSlots[i].SetColor(0x808080);
    }
    for (i = 0; i < visibleCount; i++) {
        itemSlots[i].SetEnabled(1);
        itemSlots[i].SetHighlighted(0);
        itemSlots[i].SetColor(0x808080);
    }
}

/* 0x5411d1 - per-frame map render: the visible item rows (icon, count, fade), the fade level on every quad, the
 * tooltip, the Wolf and Mailbox markers, the description panel, the footer hint and the list, then the pulse colours */
/* BYTES(slot-name): names chosen for their stack slots: count -4, dummy -6, i -8, iconA -0xc, iconB -0x10, box -0x18, lvl -0x19, id -0x1c, step -0x1e, hi -0x24, posA -0x28 */
/* BYTES(dead-code): dummy is set to 0 and never read, as in the original */
void Map::Draw()
{
    /* names chosen for their stack slots (src/README.md): count -4, dummy -6, i -8, iconA -0xc, iconB -0x10,
     * box -0x18, lvl -0x19, id -0x1c, step -0x1e, hi -0x24, posA -0x28 */
    Vec2s posA;
    Vec2s hi;
    s16 step;
    u16 id;
    u8 lvl;
    s16 box[4];
    void *iconB;
    void *iconA;
    u16 i;
    u16 dummy; /* set to 0 and never read */
    u32 count;
    lvl = phaseCounter >> 1;
    if (!visibleCount || !itemCount) {
        Text_ApplyWindow(g_screenLayerBase + 2);
        Text_NewLine(4);
        Text_WordWrap("{ Warning: no map bitmaps {", TEXTALIGN_CENTER);
        if (!visibleCount) {
            Text_NewLine(4);
            Text_WordWrap("{ Warning: no inventory objects {", TEXTALIGN_CENTER);
        }
        Hud_EndBox_stub();
        return;
    }
    dummy = 0;
    if (mode & MAP_MODE_F_TRANSIENT) {
        step = (autoRepeatMs << 12) / 0x96;
        if (step > 0x1000)
            step = 0x1000;
        /* cast kept: each rect {x, y, w, h} is lerped as two Vec2s pairs; the lerp target pointer is typed Vec3s */
        Lerp_SetVecTarget((Vec3s *)&panelTo[0]);
        Vec2s_LerpToTarget(&posA, (Vec2s *)&panelFrom[0], step);
        Lerp_SetVecTarget((Vec3s *)&panelTo[2]);
        /* cast kept: two s16 of the array read as one Vec2s */
        Vec2s_LerpToTarget(&hi, (Vec2s *)&panelFrom[2], step);
        box[0] = posA.x;
        box[1] = posA.y;
        box[2] = hi.x;
        box[3] = hi.y;
    }
    for (i = 0; i < visibleCount; i++) {
        id = inventoryClassIds[i + scrollTop];
        Scenaric_GetClassIcons(id, &iconA, &iconB);
        /* cast kept: Scenaric_GetClassIcons returns the class's icon bitmaps through void * out-parameters */
        itemSlots[i].UiIcon_Setup(resIcons[8], (u16 *)iconA, (u16 *)iconB, 0x10, listBaseY + i * 0x24, 0x400, 0x400, 0);
        count = Inventory_CountClass(id);
        itemSlots[i].SetShown(count > 0);
        itemSlots[i].quantity = count;
        if (lvl < 0xf)
            itemSlots[i].UiIcon_SetFadeLevel(lvl);
    }
    if (lvl < 0xf) {
        backdrop[0].SetFadeLevel(lvl);
        backdrop[1].SetFadeLevel(lvl);
        backdrop[2].SetFadeLevel(lvl);
        backdrop[3].SetFadeLevel(lvl);
        backdrop[4].SetFadeLevel(lvl);
        for (i = 0; i < 4; i++)
            actionSlots[i].UiIcon_SetFadeLevel(lvl);
        for (i = 0; i < locationCount; i++)
            markers[i].sprite.SetFadeLevel(lvl);
        for (i = 0; i < 2; i++)
            scrollArrow[i].SetFadeLevel(lvl);
        g_pCurFont->reserved = (g_pCurFont->reserved & (u16)~UIQUAD_FADE_MASK) | (lvl << 6);
    }
    if (mode == MAP_MODE_ITEM_LIST)
        DrawHelpTooltip();
    if ((mode & ~MAP_MODE_F_TRANSIENT) != MAP_MODE_HELP_SCROLL) {
        /* cast kept: a downcast */
        if (g_pMapLocation)
            ((MapLocation *)g_pMapLocation)->DrawWolfMarker(lvl, pulseColorSlow);
        HighlightLocationsForZone(selectedClassId);
    }
    if ((mode & ~MAP_MODE_F_TRANSIENT) == MAP_MODE_HELP_SCROLL && pendingMode == MAP_MODE_HELP_SCROLL) {
        backdrop[3].SetFadeLevel(6);
        scrollText.ScrollText_Draw(2, TEXTALIGN_LEFT);
        box[0] = g_map.backdrop[3].x;
        box[1] = g_map.backdrop[3].y;
        box[2] = g_map.backdrop[3].w;
        box[3] = g_map.backdrop[3].h;
    }
    Map_DrawPanel(box);
    Text_SetWindowRect(g_screenLayerBase + 2, s_mapFooterRect, 1);
    Text_SetColor(0x4bccff);
    Text_Printf(TEXTALIGN_CENTER, g_uiFooterText);
    Hud_EndBox_stub();
    Text_SetWindow(g_screenLayerBase + 2, 0x10, 0x10, (u16)(ScreenWidthS16() - 0x20), (u16)(ScreenHeightS16() - 0x20),
                   1);
    for (i = 0; i < visibleCount; i++)
        itemSlots[i].UiIcon_Draw(0xb);
    for (i = 0; i < 4; i++)
        actionSlots[i].UiIcon_Draw(0xb);
    scrollArrow[0].UiQuad_Draw(0xb);
    scrollArrow[1].UiQuad_Draw(0xb);
    Hud_EndBox_stub();
    g_map.backdrop[0].UiQuad_Draw(0xb);
    g_map.backdrop[1].UiQuad_Draw(0xb);
    g_map.backdrop[2].UiQuad_Draw(0xb);
    g_map.backdrop[3].UiQuad_Draw(0xb);
    g_map.backdrop[4].UiQuad_Draw(0xb);
    pulsePhaseFast = (pulsePhaseFast + g_dtRawMs * 12) & 0xfff;
    pulsePhaseSlow = (pulsePhaseSlow + g_dtRawMs * 4) & 0xfff;
    pulseColorFast = Rgb24_Lerp(0x808080, 0x3040e0, g_pCosTable[pulsePhaseFast] / 2 + 0x800);
    pulseColorSlow = Rgb24_Lerp(0x808080, 0x3040e0, g_pCosTable[pulsePhaseSlow] / 2 + 0x800);
}

/* 0x5418c1 - the first line of the help text as a drop-shadowed tooltip beside the cursor row, while helpTimerMs runs */
void Map::DrawHelpTooltip()
{
    s16 *pos;
    u16 len;
    s16 box[4];
    char saved;
    if (helpTimerMs > 0) {
        pos = &itemSlots[g_map.RowToSlot(cursorIndex)].mainQuad.x;
        box[0] = pos[0] + 0x50;
        box[1] = pos[1] + 8;
        box[2] = (ScreenWidthS16() - 0x10 - (pos[0] + 0x50)) / g_pCurFont->glyphWidth * g_pCurFont->glyphWidth + 4;
        box[3] = 0x24;
        len = 0;
        while (helpText[len] != 0 && helpText[len] != '\n')
            len++;
        saved = helpText[len];
        helpText[len] = 0;
        Text_SetWindowRect(g_screenLayerBase + 2, box, 1);
        Text_SetColor(0x301818);
        Text_SetCursor(2, 2);
        Text_WordWrap(helpText, TEXTALIGN_LEFT);
        Hud_EndBox_stub();
        Text_SetWindowRect(g_screenLayerBase + 1, box, 1);
        Text_SetColor(0x4bccff);
        Text_SetCursor(0, 0);
        Text_WordWrap(helpText, TEXTALIGN_LEFT);
        Hud_EndBox_stub();
        helpText[len] = saved;
        helpTimerMs -= (s16)g_dtRawMs;
    }
}

/* 0x541a5a - the class string 0 of an instance of the selected class, or "no help available" */
/* BYTES(layout): a named array, not a literal: the exe has this string first in the object's .data, where VC6 puts named data before the $SG literals */
char *Map::GetHelpText()
{
    char *text;
    u16 count;
    ScnObject *inst;
    count = Scenaric_FindByClass(selectedClassId, &inst, 1);
    if (count) {
        text = inst->Text_GetClassString(0);
        if (*text)
            return text;
    }
    return g_strNoHelpAvailable;
}

/* 0x541aaa - draws the Mailbox markers that accept classId, pulsing while markerTimerMs runs */
void Map::HighlightLocationsForZone(u16 classId)
{
    u16 i;
    u16 j;
    for (i = 0; i < locationCount; i++)
        for (j = 0; j < 4; j++)
            if (markers[i].acceptedClassIds[j] == classId) {
                if (markerTimerMs > 0) {
                    markers[i].sprite.SetColor(pulseColorFast);
                    markerTimerMs -= (s16)g_dtRawMs;
                } else
                    markers[i].sprite.SetColor(0x808080);
                markers[i].sprite.UiQuad_Draw(0xb);
            }
}

/* 0x541bc3 */
void Map::CursorPrev()
{
    prevCursorIndex = cursorIndex;
    ItemSlot(prevCursorIndex - scrollTop)->SetColor(0x808080);
    if (cursorIndex > 0) {
        do
            cursorIndex--;
        while (cursorIndex == combinePartnerIndex && swapPending);
        if (cursorIndex < 0)
            cursorIndex = prevCursorIndex;
        else {
            helpTimerMs = 3000;
            markerTimerMs = 6000;
        }
        if (cursorIndex < scrollTop)
            scrollTop = cursorIndex;
    }
    selectedClassId = inventoryClassIds[cursorIndex];
    helpText = GetHelpText();
}

/* 0x541ccc */
void Map::CursorNext()
{
    prevCursorIndex = cursorIndex;
    ItemSlot(prevCursorIndex - scrollTop)->SetColor(0x808080);
    if (cursorIndex < itemCount - 1) {
        do
            cursorIndex++;
        while (cursorIndex == combinePartnerIndex && swapPending);
        if (cursorIndex > itemCount - 1)
            cursorIndex = prevCursorIndex;
        else {
            helpTimerMs = 3000;
            markerTimerMs = 6000;
        }
        if (cursorIndex > scrollTop + visibleCount - 1)
            scrollTop = cursorIndex - (visibleCount - 1);
    }
    selectedClassId = inventoryClassIds[cursorIndex];
    helpText = GetHelpText();
}

/* 0x541e04 */
void Map::ActionPrev()
{
    prevActionIndex = actionIndex;
    actionSlots[prevActionIndex].SetColor(0x808080);
    if (actionIndex > MAP_ACT_HELP) {
        do
            actionIndex--;
        while (actionIndex >= MAP_ACT_HELP && !actionSlots[actionIndex].enabled);
        if (actionIndex < MAP_ACT_HELP)
            actionIndex = prevActionIndex;
    }
}

/* 0x541ead - mirror of Map_ActionPrev, except that it greys itemSlots[prevActionIndex] where ActionPrev greys
 * actionSlots[prevActionIndex] (0x541ed1 lea +0x304). Harmless: Map_UpdateState greys the old action button and
 * repaints every item row each frame anyway. */
void Map::ActionNext()
{
    prevActionIndex = actionIndex;
    itemSlots[prevActionIndex].SetColor(0x808080);
    if (actionIndex < MAP_ACT_EXIT) {
        do
            actionIndex++;
        while (actionIndex < MAP_ACT_EXIT && !actionSlots[actionIndex].enabled);
        if (actionIndex > MAP_ACT_EXIT)
            actionIndex = prevActionIndex;
    }
}

/* 0x541f59 */
ScnObject *Map::GetSelectedObject()
{
    Inventory_SelectClass(selectedClassId);
    return Inventory_GetSelectedObject();
}

/* 0x541f79 - combines the item under the cursor with the one latched at combinePartnerIndex */
void Map::CombineItems()
{
    ScnObject *target;
    ScnObject *partner;
    ScnObject *result;
    u16 partnerClass;
    target = GetSelectedObject();
    if (target) {
        partnerClass = inventoryClassIds[combinePartnerIndex];
        Inventory_SelectClass(partnerClass);
        partner = Inventory_GetSelectedObject();
        result = FindCombineTarget(selectedClassId, partnerClass);
        if (result) {
            result->HandleMessage(0, MSG_ITEM_COMBINE, &target);
            ReleaseItem(target);
            ReleaseItem(partner);
            AcquireItem(result);
            Inventory_SelectClass(result->GetClassId());
            scrollTop = 0;
            if (cursorIndex > scrollTop + visibleCount - 1)
                scrollTop = cursorIndex - (visibleCount - 1);
        }
    }
    selectedClassId = inventoryClassIds[cursorIndex];
    helpText = GetHelpText();
    swapPending = 0;
}

/* 0x5420a4 - splits a composite item (class flag 0x200) into its two parts */
void Map::SplitItem()
{
    ScnObject *parts[2];
    ScnObject *obj;
    Inventory_SelectClass(selectedClassId);
    obj = Inventory_GetSelectedObject();
    if (Scenaric_ClassFlags(obj->GetClassId()) & SCN_CF_COMPOSITE_ITEM) {
        if (obj->HandleMessage(0, MSG_ITEM_SPLIT_QUERY, parts)) {
            AcquireItem(parts[0]);
            AcquireItem(parts[1]);
            obj->HandleMessage(0, MSG_ITEM_CONSUMED, 0);
            ReleaseItem(obj);
        }
        scrollTop = 0;
        if (cursorIndex > scrollTop + visibleCount - 1)
            scrollTop = cursorIndex - (visibleCount - 1);
    }
    selectedClassId = inventoryClassIds[cursorIndex];
    helpText = GetHelpText();
    g_map.swapPending = 0;
}

/* 0x5421ad - into the inventory; the cursor goes to its row */
void Map::AcquireItem(ScnObject *obj)
{
    Inventory_Add(obj);
    obj->HandleMessage(0, MSG_INVENTORY_STORED, 0);
    cursorIndex = FindOrAddRow(obj->GetClassId());
    selectedClassId = inventoryClassIds[cursorIndex];
}

/* 0x542211 - out of the inventory (message 8); an in-world item also empties the Wolf's hands */
void Map::ReleaseItem(ScnObject *obj)
{
    obj->HandleMessage(0, MSG_INVENTORY_TAKE_OUT, 0);
    Inventory_Remove(obj);
    if (obj->GetFlags() & SCN_OF_IN_WORLD) {
        Inventory_ClearSelection();
        g_pWolf->SwapHeldItem(0, 1);
    }
    if (Scenaric_ClassFlags(obj->GetClassId()) & SCN_CF_COMPOSITE_ITEM)
        RemoveRow(obj->GetClassId());
}

/* 0x5422a9 */
s32 Map::IsRowVisible(s16 row)
{
    s16 rel;
    rel = RowToSlot(row);
    if (rel >= 0 && rel < visibleCount)
        return 1;
    return 0;
}

/* 0x5422e9 */
void Map::BeginModeTransition(u8 nextMode)
{
    autoRepeatMs = 0;
    if (nextMode)
        pendingMode = nextMode;
    else
        pendingMode = mode;
    mode |= MAP_MODE_F_TRANSIENT;
}

/* 0x542331 - owned classes first, then the rest, each group in its previous order */
void Map::SortRowsByOwned()
{
    u16 id;
    u16 n;
    u16 saved[16];
    u16 i;
    n = 0;
    for (i = 0; i < itemCount; i++)
        saved[i] = inventoryClassIds[i];
    for (i = 0; i < itemCount; i++) {
        id = saved[i];
        if (Inventory_CountClass(id)) {
            inventoryClassIds[n] = id;
            n++;
        }
    }
    for (i = 0; i < itemCount; i++) {
        id = saved[i];
        if (!Inventory_CountClass(id)) {
            inventoryClassIds[n] = id;
            n++;
        }
    }
}

/* 0x542445 */
u16 Map::FindOrAddRow(u16 classId)
{
    u16 i;
    i = 0;
    while (inventoryClassIds[i] != classId && i < itemCount)
        i++;
    if (i == itemCount) {
        inventoryClassIds[itemCount] = classId;
        itemCount++;
    }
    return i;
}

/* 0x5424c6 - drops the class's row once none of it is carried */
void Map::RemoveRow(u16 classId)
{
    u16 i;
    i = 0;
    while (inventoryClassIds[i] != classId && i < itemCount)
        i++;
    Inventory_SelectClass(classId);
    if (!Inventory_GetSelectedObject()) {
        for (; i < 15; i++)
            inventoryClassIds[i] = inventoryClassIds[i + 1];
        inventoryClassIds[i] = CLASSID_NONE;
    }
    itemCount--;
}

/* 0x54257d */
u16 Map::FindRow(u16 classId)
{
    u16 i;
    i = 0;
    while (inventoryClassIds[i] != classId && i < itemCount)
        i++;
    return i;
}

/* 0x5425ca - outRect = the quad's x, y with w / h, or the quad's own size where they are 0 */
void Map::SetPanelRect(s16 *outRect, UiQuad *src, u16 h, u16 w)
{
    if (!w)
        w = src->w;
    if (!h)
        h = src->h;
    outRect[0] = src->x;
    outRect[1] = src->y;
    outRect[2] = w;
    outRect[3] = h;
}

/* 0x54262e - the selected item is a composite that answers message 0x6e with a non-zero value */
s32 Map::IsSelectedItemAssembled()
{
    ScnObject *assembled[2];
    ScnObject *obj;
    obj = GetSelectedObject();
    if (obj && (Scenaric_ClassFlags(obj->GetClassId()) & SCN_CF_COMPOSITE_ITEM) &&
        obj->HandleMessage(0, MSG_COMPOSITE_PARTS_QUERY, assembled) && assembled[0])
        return 1;
    return 0;
}

/* 0x54269a - the composite object that is made from classA and classB (message 0x28 with the pair, reply 0 = free) */
/* BYTES(dead-code): base is stored and never read, as in the original */
ScnObject *Map::FindCombineTarget(u16 classA, u16 classB)
{
    ScnObject **base;
    struct {
        s32 inUse; /* reply: 0 = this composite is free to be built from the pair */
        u16 classA;
        u16 classB;
    } msg;
    s32 found;
    ScnObject *obj;
    s32 idx;
    idx = 0;
    found = 0;
    msg.classA = classA;
    msg.classB = classB;
    base = g_scnObjects;
    do {
        obj = g_scnObjects[idx];
        idx++;
        if (obj && (Scenaric_ClassFlags(obj->GetClassId()) & SCN_CF_COMPOSITE_ITEM) &&
            obj->HandleMessage(0, SCN_MSG_ITEM_MATCH_QUERY, &msg) && !msg.inUse)
            found = 1;
    } while (idx < g_scnObjectCount && !found);
    if (found)
        return obj;
    return 0;
}

/* 0x542759 - the curtain wipe: state 1 closes it over the world (then Map_Open), state 6 opens it again */
void SelectMenu_UpdateWipe()
{
    u16 gap;
    if (SelectMenu_GetState() == SEL_OPEN_WIPE) {
        GameFlags_Clear(GF_UPDATE_OBJECTS);
        g_map.wipeHeight += 7;
        Map_DrawWipeBar(0, g_map.wipeHeight);
        Map_DrawWipeBar(ScreenHeightS16() - g_map.wipeHeight, g_map.wipeHeight);
        if (g_map.wipeHeight > ScreenHeightS16() / 2 + 7) {
            g_map.Open();
            g_map.wipeHeight = 0;
        }
    } else if (SelectMenu_GetState() == SEL_CLOSE_TAIL) {
        gap = ScreenHeightS16() / 2 - g_map.wipeHeight;
        if (g_map.wipeHeight > ScreenHeightS16() / 2) {
            GameFlags_Set(GF_UPDATE_OBJECTS);
            SelectMenu_SetState(SEL_CLOSED);
            g_map.wipeHeight = 0;
        }
        g_map.wipeHeight += 7;
        Map_DrawWipeBar(0, gap);
        Map_DrawWipeBar(ScreenHeightS16() - gap, gap);
    }
}

/* 0x542893 */
void SelectMenu_SetState(u8 state)
{
    g_map.state = state;
}

/* 0x5428a0 */
u8 SelectMenu_GetState()
{
    return g_map.state;
}

/* 0x5428aa */
s32 SelectMenu_HasItems()
{
    return g_map.itemCount > 0;
}
