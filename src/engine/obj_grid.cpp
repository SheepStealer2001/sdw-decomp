/*
 * T252 - guessed original name: ObjGrid.cpp. SheepD3D.exe .text 0x516740-0x5169be, .bss 0x6d5124-0x6d5140.
 * The object grid: its geometry and cell array (this object's .bss, plus the WAR type-0x84 header pointer the loader
 * sets), init / free, the cell of a point (clamped or not) and the list of a cell: ObjGrid_Init .. ObjGrid_GetCellList
 * (0x516740-0x5169bd), and the definitions of the grid globals.
 *
 * The `w` struct only pins the original stack offsets of locals (VC6 /Od hands out slots by a hash of the local names,
 * src/README.md); it is not a claim about the source text.
 *
 * .bss: the globals are written `= 0`. VC6 emits explicitly zero-initialised globals to .bss in DEFINITION order,
 * while uninitialised ones are ordered by a hash of their names ((h ^ h >> 16) & 0x3ff), which with these names would
 * start with g_objGridDimX. (In C, which the linkage of some callers suggests, an uninitialised global would be a COMMON
 * placed after the CRT's .bss, so a C original needed the `= 0` too.) The 4 bytes after g_pWarObjGrid are the alignment
 * of the next object's 8-aligned .bss.
 * Linkage: the definitions have C++ names, as their users declare them.
 */
/* BYTES: layout, slot-group. */
/* BYTES(layout): written '= 0' only to keep definition order in .bss */
#include "sdw_classes.h"

#include "../sdk/crt.h"
#include "list.h"

/* ---- .bss 0x6d5124-0x6d513c ---- */
s16 g_objGridOriginX = 0;             /* 0x6d5124  object grid origin X */
s16 g_objGridOriginZ = 0;             /* 0x6d5126  object grid origin Z (z) */
u16 g_objGridShift = 0;               /* 0x6d5128  log2 of the cell size */
u16 g_objGridDimX = 0;                /* 0x6d512a  cells along X */
u16 g_objGridDimZ = 0;                /* 0x6d512c  cells along Z */
ListNode ***g_objGridCells = 0;       /* 0x6d5130  dimX*dimZ list heads, one per cell */
ListNode **g_objGridOverflowList = 0; /* 0x6d5134  the list every object outside the grid shares */
u16 *g_pWarObjGrid = 0;               /* 0x6d5138  WAR type 0x84 (set by the loader, 0x547855) */

/* 0x516740 - builds the object grid from the level's type-0x84 resource header {dimX, dimZ, originX, originZ, shift} and
 * gives every cell an empty list, plus the shared overflow list for anything outside the grid. The cell array comes from
 * the CRT allocator, not the level heap. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
void ObjGrid_Init(u16 *header)
{
    struct {
        s32 z, x;
        ListNode ***cell;
    } w;

    g_objGridDimX = *header++;
    g_objGridDimZ = *header++;
    g_objGridOriginX = *header++;
    g_objGridOriginZ = *header++;
    g_objGridShift = *header++;
    /* cast kept: malloc returns void * */
    g_objGridCells = (ListNode ***)malloc((u32)g_objGridDimX * g_objGridDimZ * 4);
    w.cell = g_objGridCells;
    for (w.z = 0; w.z < g_objGridDimZ; ++w.z) {
        for (w.x = 0; w.x < g_objGridDimX; ++w.x) {
            List_Create(w.cell);
            ++w.cell;
        }
    }
    List_Create(&g_objGridOverflowList);
}

/* 0x516845 - frees the cell array on level teardown. The lists themselves live in the level heap and go with it. */
void ObjGrid_Free()
{
    if (g_objGridCells) {
        free(g_objGridCells);
        g_objGridCells = 0;
    }
}

/* 0x51686b - the cell a point falls in, WITHOUT clamping: the caller gets negative or out-of-range cells and
 * ObjGrid_GetCellList turns those into the overflow list. */
void ObjGrid_CellFromXZ_Unclamped(s16 x, s16 z, s16 *cellX, s16 *cellZ)
{
    *cellX = (x - g_objGridOriginX) >> g_objGridShift;
    *cellZ = (z - g_objGridOriginZ) >> g_objGridShift;
}

/* 0x5168a8 - object-grid cell of a point, clamped to the grid (ObjGrid_CellFromXZ_Unclamped 0x51686b is the same
 * without the clamp). */
void ObjGrid_CellFromXZ(s16 x, s16 z, s16 *cellX, s16 *cellZ)
{
    *cellX = (x - g_objGridOriginX) >> g_objGridShift;
    *cellZ = (z - g_objGridOriginZ) >> g_objGridShift;
    if (*cellX < 0)
        *cellX = 0;
    else if (*cellX >= g_objGridDimX)
        *cellX = g_objGridDimX - 1;
    if (*cellZ < 0)
        *cellZ = 0;
    else if (*cellZ >= g_objGridDimZ)
        *cellZ = g_objGridDimZ - 1;
}

/* 0x51694f - the list head of a cell, or the shared overflow list when the cell is outside the grid. The four range
 * tests are OR-ed as values, not short-circuited, which is what the original emits. */
ListNode **ObjGrid_GetCellList(s16 cellX, s16 cellZ)
{
    if ((cellX < 0) | (cellZ < 0) | (cellX >= g_objGridDimX) | (cellZ >= g_objGridDimZ))
        return g_objGridOverflowList;
    else
        return g_objGridCells[cellX + cellZ * g_objGridDimX];
}
