#ifndef SDW_ENGINE_ID_LIST_H
#define SDW_ENGINE_ID_LIST_H

/* The functions and globals id_list.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

extern u32 *g_idListBlob;                                    /* 0x71af5c */
extern u32 g_idListCount;                                    /* 0x71af60 */
extern u32 g_warExportCount;                                 /* 0x71af58 */
extern u32 *g_warExportTable;                                /* 0x71af54 WAR type 0x82 */
extern u32 g_warRelocCount;                                  /* 0x71af68 */
extern u32 *g_warRelocTable;                                 /* 0x71af64 WAR type 0x85 */
u32 *IdList_FindWithCount(u16 id, u16 *countOut);            /* 0x5608d1 */
u32 *IdList_GetBlob(u32 *countOut);                          /* 0x56095c */
u16 IdList_GetId(u16 *rec);                                  /* 0x56099c */
u32 *IdList_Next(u32 *rec);                                  /* 0x56097e */
void Res_RelocateIdLists(u32 *blob, int nRecords, int base); /* 0x5606e0 */
u32 *Scn_FindIdList(u16 id, u16 *countOut);                  /* 0x560846 */
u32 *WarReloc_Find(u16 id, u16 *countOut, s32 *claim);       /* 0x560797 */

#endif
