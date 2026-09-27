#ifndef SDW_ENGINE_LIST_H
#define SDW_ENGINE_LIST_H

/* The functions and globals list.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

struct Dav;
struct ListNode;

void List_AllocateNode(ListNode **nodeOut);           /* 0x5472fa */
u32 List_Count(ListNode **list);                      /* 0x5472a1 */
void List_Create(ListNode ***listOut);                /* 0x5472d9 */
void List_FreeNode(void *node);                       /* 0x5473c4 */
void List_PushFront(ListNode **list, ListNode *node); /* 0x547310 */
void List_Remove(ListNode **list, ListNode *node);    /* 0x547378 */
u8 Load_DAVnWAR(const char *levelPath, Dav *dav);     /* 0x547855 */
void Load_FreeLevel();                                /* 0x54818f */
void Scratch32k_Alloc();                              /* 0x547240 */
void Scratch32k_Free();                               /* 0x547257 */
void Scratch32k_Install();                            /* 0x547287 */

#endif
