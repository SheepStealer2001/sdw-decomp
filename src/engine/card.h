#ifndef SDW_ENGINE_CARD_H
#define SDW_ENGINE_CARD_H

/* The functions and globals card.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

s32 CardStub_FreeBlocks();
s32 CardStub_Status();
s32 CardStub_Status2();
s32 CardStub_Status3(const char *);
s32 Card_ReadBlocks(const char *, u32, void *);
s32 Card_SaveExists(const char *);
s32 Card_WriteFrames(const char *, u32, void *);
void Reg_CloseProgressKey();
s32 Reg_OpenProgressKey();

#endif
