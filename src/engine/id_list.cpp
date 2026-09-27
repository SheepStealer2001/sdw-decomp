/*
 * T302 - original object guessed as IdList.cpp (data/tu_map.json). Ranges: .text 0x5606e0-0x5609b2, .bss
 * 0x71af54-0x71af70 (g_warExportTable/Count, g_idListBlob/Count, g_warRelocTable/Count): the eight id-list functions
 * 0x5606e0-0x5609b1 (the TexScroll code after them is T303, src/engine/tex_scroll.cpp).
 *
 * Record formats: an id-list record is a u32 header (the id in the low word, the entry count in the high word)
 * followed by that many u32 entries, which Res_RelocateIdLists turns from file offsets into addresses. The WAR
 * type-0x85 table is {u32 idAndCount; u32 ptr} pairs: the id in the low word, a 15-bit count above it and a claimed
 * bit on top.
 * .bss: the six globals are defined here with explicit "= 0" initialisers. VC6 puts a zero-initialised global in .bss
 * in DEFINITION order, after any uninitialised ones, which it orders by a hash of their names (key = (h ^ h>>16) & 0x3ff,
 * h = h*4 + h>>4 + c; src/README.md): the explicit form reproduces the original's address order without renaming
 * anything. Whether the original spelled them this way is not known.
 */
/* BYTES: layout, slot-name. */
/* BYTES(layout): written '= 0' only to keep definition order in .bss */
#include "sdw_classes.h"

/* ---- globals (addresses in SheepD3D.exe), defined here in address order ---- */
u32 *g_warExportTable = 0; /* 0x71af54  WAR type 0x82: id-list records                                   */
u32 g_warExportCount = 0;  /* 0x71af58  record count                                                     */
u32 *g_idListBlob = 0;     /* 0x71af5c  DAV id-list records                                              */
u32 g_idListCount = 0;     /* 0x71af60  record count                                                     */
u32 *g_warRelocTable = 0;  /* 0x71af64  WAR type 0x85: {u32 idAndCount; u32 ptr} pairs                   */
u32 g_warRelocCount = 0;   /* 0x71af68  pair count                                                       */

/* 0x5606e0 - walks nRecords id-list records and adds base to every entry (file offsets -> pointers). */
void Res_RelocateIdLists(u32 *blob, int nRecords, int base)
{
    u16 count;
    for (; nRecords != 0; nRecords--) {
        count = (u16)(*blob >> 16);
        blob++;
        for (; count != 0; count--) {
            *blob = *blob + base;
            blob++;
        }
    }
}

/* 0x56073e - the id (low word) of the type-0x85 pair at index, clamped to the last pair; 0xffff when the table is empty. No callers. */
u16 WarReloc_GetIdAt(u16 index)
{
    u32 count = g_warRelocCount;
    u32 *p = g_warRelocTable;
    if (index >= count)
        index = (u16)(count - 1);
    if (index == 0xffff)
        return 0xffff;
    p += index * 2;
    return (u16)(*p & 0xffff);
}

/* 0x560797 - finds the type-0x85 pair for id. Writes its 15-bit count through countOut; when *claim is non-zero on entry
 * marks the pair claimed; either way *claim receives the pair's previous claimed bit. Returns the pair's pointer word. */
u32 *WarReloc_Find(u16 id, u16 *countOut, int *claim)
{
    int wasClaimed;
    u32 count = g_warRelocCount;
    u32 *p = g_warRelocTable;
    while (count != 0 && (*p & 0xffff) != id) {
        p += 2;
        count--;
    }
    if (count == 0) {
        *claim = 0;
        *countOut = 0;
        return 0;
    }
    wasClaimed = (u16)(*p >> 16) >> 15;
    *countOut = (u16)((*p >> 16) & 0x7fff);
    if (*claim != 0)
        *p = *p | 0x80000000;
    *claim = wasClaimed;
    return (u32 *)p[1]; /* cast kept: the pair's second word is a relocated address stored as u32 */
}

/* 0x560846 - the entries of the WAR export-table record for id, and their count; NULL and 0 when there is none.
 * `result` is named for its slot (count -0xc, p -8, result -4, as in the original); `rec`, `found`, `entry` land elsewhere. */
/* BYTES(slot-name): result is named for its slot (count -0xc, p -8, result -4) and the found pointer goes through it as in the original */
u32 *Scn_FindIdList(u16 id, u16 *countOut)
{
    u32 count = g_warExportCount;
    u32 *p = g_warExportTable;
    u32 *result;
    while (count != 0 && (*p & 0xffff) != id) {
        p = p + (*p >> 16) + 1;
        count--;
    }
    if (count == 0)
        result = 0;
    else
        result = p;
    if (result == 0) {
        *countOut = 0;
        return 0;
    }
    *countOut = (u16)(*result >> 16);
    return result + 1;
}

/* 0x5608d1 - the same search over the DAV id-list blob. Returns the ENTRIES (record + 1), not the record header. */
u32 *IdList_FindWithCount(u16 id, u16 *countOut)
{
    u32 count = g_idListCount;
    u32 *p = g_idListBlob;
    u32 *result;
    while (count != 0 && (*p & 0xffff) != id) {
        p = p + (*p >> 16) + 1;
        count--;
    }
    if (count == 0)
        result = 0;
    else
        result = p;
    if (result == 0) {
        *countOut = 0;
        return 0;
    }
    *countOut = (u16)(*result >> 16);
    return result + 1;
}

/* 0x56095c - the DAV id-list blob and its record count; NULL (count untouched) when no level is loaded. */
u32 *IdList_GetBlob(u32 *countOut)
{
    if (g_idListBlob == 0)
        return 0;
    *countOut = g_idListCount;
    return g_idListBlob;
}

/* 0x56097e - the record after rec (header + count entries); NULL stays NULL. */
u32 *IdList_Next(u32 *rec)
{
    if (rec == 0)
        return 0;
    return rec + (*rec >> 16) + 1;
}

/* 0x56099c - a record's id, 0 for NULL. */
u16 IdList_GetId(u16 *rec)
{
    if (rec == 0)
        return 0;
    return *rec;
}
