#ifndef SDW_OBJECTS_DAFFYLEVEL02_H
#define SDW_OBJECTS_DAFFYLEVEL02_H

/* The functions and globals daffylevel02.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

struct MenuPage;
class ScnObject;

ScnObject *DaffyLevel02_Create(void *record);
void DaffyLevel02_DrawAnswer0(u8 msg, MenuPage *self);
void DaffyLevel02_DrawAnswer1(u8 msg, MenuPage *self);
void DaffyLevel02_DrawAnswer2(u8 msg, MenuPage *self);

#endif
