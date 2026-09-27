#ifndef SDW_ENGINE_OBJ_GRID_H
#define SDW_ENGINE_OBJ_GRID_H

/* The functions and globals obj_grid.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

struct ListNode;

extern ListNode ***g_objGridCells; /* 0x6d5130 dimX*dimZ list handles (each points at the cell's head node) */
extern u16 g_objGridDimX;          /* 0x6d512a */
extern u16 *g_pWarObjGrid;         /* 0x6d5138 WAR type 0x84 */
void ObjGrid_CellFromXZ(s16 x, s16 z, s16 *cellXOut, s16 *cellZOut);           /* 0x5168a8, clamped to the grid */
void ObjGrid_CellFromXZ_Unclamped(s16 x, s16 z, s16 *cellXOut, s16 *cellZOut); /* 0x51686b */
void ObjGrid_Free();                                                           /* 0x516845 */
ListNode **ObjGrid_GetCellList(s16 cellX, s16 cellZ);                          /* 0x51694f the cell's list head */
void ObjGrid_Init(u16 *gridHeader);                                            /* 0x516740 */

#endif
