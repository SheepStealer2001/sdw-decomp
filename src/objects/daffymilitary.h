#ifndef SDW_OBJECTS_DAFFYMILITARY_H
#define SDW_OBJECTS_DAFFYMILITARY_H

/* The functions and globals daffymilitary.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

struct MenuPage;
class ScnObject;

ScnObject *DaffyMilitary_Create(void *record);
void DaffyMilitary_DrawAnswer0(u8 msg, MenuPage *self);
void DaffyMilitary_DrawAnswer1(u8 msg, MenuPage *self);
void DaffyMilitary_DrawAnswer2(u8 msg, MenuPage *self);

#endif
