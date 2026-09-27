/*
 * T303 - original object guessed as TexScroll.cpp (tu_map). Ranges: .text 0x5609c0-0x560f7b, .bss 0x71af70-0x71b2c8
 * (g_texScrollCount, g_texScrolls, g_texScrollBackup). The six TexScroll_* functions 0x5609c0-0x560f7a (the id-list
 * part before them is T302).
 *
 * The texture scrolls: up to four id-lists named by the WAR type-0x83 header (g_texScrollListIds, reversal bits in
 * g_texScrollReverseMask) list DAV bitmap rects; each becomes a TexScroll entry plus a Texture holding a copy of the
 * rect's texels, and TexScroll_Update rotates the rect one row per call by copying from that copy. Game_Frame calls it
 * on every entry once per frame while GF_UPDATE_OBJECTS is set (tail jump at 0x56052f into 0x560f41).
 * Local names: VC6 /Od places locals by a hash of their NAMES, not their declaration order (tools/vc6_locals.py); where
 * a function below says its names were chosen for their slots, other names give the same code at other offsets.
 * .bss: defined with explicit "= 0" initialisers, which VC6 places in definition order (uninitialised globals would be
 * ordered by a hash of their names instead). The original spelling is unknown. Texture::Surface_LockForWrite is
 * declared returning long, as src/engine/texture.cpp defines it (same code).
 */
/* BYTES: dead-code, layout, slot-name, view. */
/* BYTES(layout): written '= 0' only to keep definition order in .bss */
/* BYTES(layout): placeholder: unreferenced bytes kept only for the .bss layout */
/* BYTES(view): struct TexScrollReverseBits (g_texScrollReverseMask): a u16 bitfield, not a mask: TexScroll_Init's 16-bit load / shift / and (0x560e61-0x560ec5) is VC6's bitfield code */
#include "../sdk/ddraw.h"
#include "../sdk/crt.h"
#define SDW_MEMBERS_Texture                                                                                        \
    Texture(D3DApp *app, u32 width, u32 height, u32 format, s32 *result); /* 0x40a140 */                           \
    void Surface_LockForRead(DDSURFACEDESC2 *desc);  /* Lock(NULL, desc, 0x811 = WAIT|READONLY|NOSYSLOCK, NULL) */ \
    long Surface_LockForWrite(DDSURFACEDESC2 *desc); /* 0x40ae3f (returns long, as its definition declares it) */
#include "sdw_classes.h"

/* ---- globals (addresses in SheepD3D.exe) ---- */
/* This object's .bss, defined in address order with explicit "= 0" initialisers (VC6 then keeps definition order; see
 * the header). Two runs of it are referred to by no code: 0x71af72-0x71af80 and 0x71b280-0x71b288. g_texScrolls must be
 * 8-aligned (VC6 aligns an object of 64 bytes or more to 8), so the 8-byte placeholder at 0x71af74 reproduces the first
 * gap, and a 4-byte one at 0x71b280 the second (g_texScrolls then holds exactly 32 entries of 0x18). What the original
 * kept there is unknown; nothing in the exe reads or writes those bytes. */
u16 g_texScrollCount = 0; /* 0x71af70  number of TexScroll entries in use (really the texture-scroll count) */
u32 g_texScrollUnref_71af74[2] = {0}; /* 0x71af74  unreferenced (placeholder, see above)                    */
TexScroll g_texScrolls[32] = {0};     /* 0x71af80  stride 0x18                                                      */
u32 g_texScrollUnref_71b280 = 0;      /* 0x71b280  unreferenced (placeholder, see above)                            */
Texture *g_texScrollBackup[16] = {0}; /* 0x71b288  per entry: a copy of the rect's texels                          */
#include "game_state.h"
#include "draw2d.h"
#include "id_list.h"
/* 0x6ddf68, type-0x83 header bytes 8..9: bit i reverses list i. A u16 BITFIELD, not a mask: TexScroll_Init reads each bit
 * as a 16-bit load, shift and `and` (0x560e61-0x560ec5), which is VC6's bitfield code; `(mask >> i) & 1` on a u16 is
 * promoted and compiles to movzx/sar instead. */
struct TexScrollReverseBits {
    u16 list0 : 1;
    u16 list1 : 1;
    u16 list2 : 1;
    u16 list3 : 1;
};
extern TexScrollReverseBits g_texScrollReverseMask;

void TexScroll_AddRect(const DavBitmapRec *rec, s8 step);

/* 0x5609c0 - adds every rect of id-list listId as a scroll, stepping +1 row per update, or -1 when reverse is set. */
void TexScroll_AddList(u16 listId, s8 reverse)
{
    u32 *entries;
    u16 count;
    const DavBitmapRec *rec;
    u16 i;
    if (listId != 0) {
        if (reverse == 0)
            reverse = 1;
        else
            reverse = -1;
        entries = IdList_FindWithCount(listId, &count);
        for (i = 0; i < count; i++) {
            rec = (const DavBitmapRec *)*entries; /* cast kept: an id list's entries are record addresses */
            entries++;
            TexScroll_AddRect(rec, reverse);
        }
    }
}

/* 0x560a3b - fills the next TexScroll entry from a DAV bitmap rect and blits the rect's texels from its page texture into a
 * new backup Texture of the rect's size. Neither the constructor's result code nor the 16 backup slots are checked.
 * Names chosen for their slots: entry -0x1c, res -0x18, source -0x14, rect -0x10, as in the original. */
/* BYTES(slot-name): names chosen for their stack slots: entry -0x1c, res -0x18, source -0x14, rect -0x10 */
void TexScroll_AddRect(const DavBitmapRec *rec, s8 step)
{
    TexScroll *entry;
    Texture *source;
    s32 res;
    RECT rect;
    entry = &g_texScrolls[g_texScrollCount];
    entry->y = rec->v;
    entry->h = rec->height;
    entry->x = rec->u;
    entry->w = rec->width;
    entry->texPage = rec->page;
    entry->srcY = 0;
    entry->srcH = rec->height;
    entry->srcX = 0;
    entry->srcW = rec->width;
    entry->offset = 0;
    entry->step = step;
    source = g_pPolyBin->textures[entry->texPage];
    g_texScrollBackup[g_texScrollCount] = new Texture(g_pD3DAppMain, entry->w, entry->h, source->GetFormat(), &res);
    rect.top = rec->v;
    rect.bottom = rec->v + rec->height;
    rect.left = rec->u;
    rect.right = rec->u + rec->width;
    g_texScrollBackup[g_texScrollCount]->GetSurface()->BltFast(0, 0, source->GetSurface(), &rect, 0);
    g_texScrollCount++;
}

/* 0x560bcc - scrolls entry index by one step: offset = (offset + step + H) % H, then rewrites the rect on its texture page
 * from the backup: backup rows startRow..rowEnd-offset go to page rows y+offset.., and (when offset != 0) the last offset
 * backup rows go to the top of the rect. Pitch = Texture width in texels, 16-bit texels.
 * Names and declaration order chosen for their slots (see the file header): texels -4, wpos -8, bakDdsd -0xc,
 * row -0x10, texDdsd -0x14, bak -0x18, bakMem -0x1c, rowEnd -0x20, cw -0x24, startRow -0x28, startCol -0x2c, tex -0x30,
 * cur -0x34, topRow -0x38, as in the original. cw and startCol are copied and never read.
 * The offset is an s8 but runs over 0..H-1 of the backup's height H: see the note after the function. */
/* BYTES(slot-name): names and declaration order chosen for their stack slots: texels -4, wpos -8, bakDdsd -0xc, row -0x10, texDdsd -0x14, bak -0x18, bakMem -0x1c, rowEnd -0x20, cw -0x24, startRow -0x28, startCol -0x2c, tex -0x30, cur -0x34, topRow -0x38 */
/* BYTES(dead-code): cw and startCol are copied and never read, as in the original */
void TexScroll_Update(u16 index)
{
    DDSURFACEDESC2 *bakDdsd;
    s32 wpos;
    u16 *texels;
    s32 row;
    Texture *bak;
    DDSURFACEDESC2 *texDdsd;
    u16 *bakMem;
    s32 startRow, cw, rowEnd;
    s32 startCol;
    Texture *tex;
    s32 topRow;
    TexScroll *cur;
    cur = &g_texScrolls[index];
    bak = g_texScrollBackup[index];
    tex = g_pPolyBin->textures[cur->texPage];
    cur->offset = (s8)((cur->offset + cur->step + bak->GetHeight()) % bak->GetHeight());
    startRow = cur->srcY;
    rowEnd = cur->srcH;
    startCol = cur->srcX;
    cw = cur->srcW;
    texDdsd = new DDSURFACEDESC2;
    bakDdsd = new DDSURFACEDESC2;
    tex->Surface_LockForWrite(texDdsd);
    bak->Surface_LockForRead(bakDdsd);
    texels = (u16 *)texDdsd->lpSurface; /* cast kept: a locked surface is raw memory; these pages are 16-bit texels */
    bakMem = (u16 *)bakDdsd->lpSurface; /* cast kept: as above */
    rowEnd -= cur->offset;
    wpos = (cur->y + cur->offset) * tex->GetWidth() + cur->x;
    for (row = startRow; row < rowEnd; row++) {
        memcpy(texels + wpos, bakMem + row * bak->GetWidth(), cur->srcW * 2);
        wpos += tex->GetWidth();
    }
    if (cur->offset != 0) {
        startRow = rowEnd;
        rowEnd = cur->srcH;
        wpos = cur->y * tex->GetWidth() + cur->x;
        for (topRow = startRow; topRow < rowEnd; topRow++) {
            memcpy(texels + wpos, bakMem + topRow * bak->GetWidth(), cur->srcW * 2);
            wpos += tex->GetWidth();
        }
    }
    tex->Surface_Unlock();
    bak->Surface_Unlock();
    delete texDdsd;
    delete bakDdsd;
}
/* Note (read, not observed in game): H is the backup Texture's height, which the constructor rounds up to a power of two
 * (and to a square) when the device reports D3DPTEXTURECAPS_POW2 / SQUAREONLY, while the copy loops use the rect height h
 * (srcH). With H > h the offset reaches h..H-1, rowEnd goes negative and the second loop reads backup rows before the
 * locked memory and writes up to H rows from the rect's top, past its bottom. Separately, H > 128 makes the s8 offset wrap
 * to negative values, so rowEnd -= offset grows past h and the first loop reads below the backup and writes above the rect.
 * Neither has been checked against the shipped data or hardware; on a device without the power-of-two cap and with rects
 * of at most 128 rows neither happens. */

/* 0x560e12 - at level load (Load_DAVnWAR 0x547f37): builds the scroll list from the four id-lists of the type-0x83 header.
 * `total` and `list` are named for their stack slots (-8 and -2, as in the original). */
/* BYTES(slot-name): total and list are named for their stack slots (-8, -2) */
void TexScroll_Init(void)
{
    s32 total;
    u16 list;
    total = 0;
    g_texScrollCount = 0;
    for (list = 0; list < 4; list++)
        total += g_texScrollListIds[list];
    if (total != 0) {
        TexScroll_AddList(g_texScrollListIds[0], g_texScrollReverseMask.list0);
        TexScroll_AddList(g_texScrollListIds[1], g_texScrollReverseMask.list1);
        TexScroll_AddList(g_texScrollListIds[2], g_texScrollReverseMask.list2);
        TexScroll_AddList(g_texScrollListIds[3], g_texScrollReverseMask.list3);
    }
}

/* 0x560edd - at level unload (Load_FreeLevel 0x54818f): deletes every backup Texture. The slots are not cleared. */
void TexScroll_FreeAll(void)
{
    u32 i;
    for (i = 0; i < g_texScrollCount; i++)
        delete g_texScrollBackup[i];
    g_texScrollCount = 0;
}

/* 0x560f41 - scrolls every entry by one step (TexScroll_Update on each); reached once per frame from Game_Frame 0x56052f. */
void TexScroll_UpdateAll(void)
{
    u16 i;
    for (i = 0; i < g_texScrollCount; i++)
        TexScroll_Update(i);
}
