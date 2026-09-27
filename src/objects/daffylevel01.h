#ifndef SDW_OBJECTS_DAFFYLEVEL01_H
#define SDW_OBJECTS_DAFFYLEVEL01_H

/* The functions and globals daffylevel01.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

struct MenuPage;
class ScnObject;

ScnObject *DaffyLevel01_Create(void *record);
void DaffyLevel01_DrawChoice0(u8 msg, MenuPage *self);
void DaffyLevel01_DrawChoice1(u8 msg, MenuPage *self);
void DaffyLevel01_DrawChoice2(u8 msg, MenuPage *self);
void DaffyLevel01_DrawChoice3(u8 msg, MenuPage *self);
void DaffyLevel01_DrawChoice4(u8 msg, MenuPage *self);

#endif
