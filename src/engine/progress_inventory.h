#ifndef SDW_ENGINE_PROGRESS_INVENTORY_H
#define SDW_ENGINE_PROGRESS_INVENTORY_H

/* The functions and globals progress_inventory.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

class ScnObject;

void InvWheel_Draw(s16 angle, u8 crayonFrame, s32 slideY);
void InvWheel_DrawSlot(u16 classId, s32 x, s32 y, u32 color, u8 crayonFrame);
s32 InvWheel_IsRotating();           /* 0x50c90d */
void InvWheel_Render();              /* 0x50ccf1 */
void Inventory_Add(ScnObject *obj);  /* 0x50c4ac */
void Inventory_ClearSelection();     /* 0x50c811 */
void Inventory_CommitAtCheckpoint(); /* 0x50d37f */
u32 Inventory_CountClass(u16 classId);
void Inventory_DropUncommitted();         /* 0x50d180 */
ScnObject *Inventory_GetSelectedObject(); /* 0x50c635 */
void Inventory_Init();                    /* 0x50d10e */
u16 Inventory_NextClass(u16 classId);
u16 Inventory_PrevClass(u16 classId);
void Inventory_Remove(ScnObject *obj);                             /* 0x50c59e */
void Inventory_SelectClass(u16 classId);                           /* 0x50c66f */
ScnObject *Inventory_SelectNext();                                 /* 0x50c6a1 */
ScnObject *Inventory_SelectPrev();                                 /* 0x50c75c */
void Inventory_SetWheelOpen(s32 open);                             /* 0x50c941 */
void Inventory_UpdateFx();                                         /* 0x50ce50 */
ScnObject *ItemFly_GetObject();                                    /* 0x50c925 */
void ItemFly_SetFromPos(const Vec3s *pos);                         /* 0x50ce02 */
void ItemFly_Start(ScnObject *obj, u8 mode, const Vec3s *fromPos); /* 0x50cd29 */
void ItemFly_Stop();                                               /* 0x50ce2f */

#endif
