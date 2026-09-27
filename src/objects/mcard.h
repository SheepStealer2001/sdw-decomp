#ifndef SDW_OBJECTS_MCARD_H
#define SDW_OBJECTS_MCARD_H

/* The functions and globals mcard.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

extern u8 g_mcardMode;
s32 Card_Access(s32);
s32 Card_AnyBlockUsed();
s32 Card_CommitBlock(s32, s32);
void Card_DrawScreen();
s32 Card_StateMachine(); /* 0x54b459 */
u8 MCard_ChooseOption(char *, char *, char *);
s32 MCard_DelayElapsed();
void MCard_DrawOkRetryFooter();
void MCard_DrawTextColored(s16 *, u8, u32, char *, u32);
void MCard_DrawValidCancelFooter();
void MCard_DrawValidFooter();
u8 MCard_FindSlotWithState(s32, char);
const char *MCard_GetString(u32 id);
void MCard_Init(); /* 0x54a7c3 */
u8 MCard_PromptConfirm(char *, char *);
s8 MCard_PromptYesNo(char *, char *, char *);
void MCard_ResetCursor();
void MCard_SetMode(u8); /* 0x54a968 */
u8 MCard_ShowMessage(char *);
u8 MCard_SlotChooser(char *, char *, char *);
u8 MCard_SlotChooserConfirmSave();
u8 MCard_StepBlockScan();
u8 MCard_StepCursor(s32, char);
void MCard_Stub_54d4f5(s32);
u8 MCard_UpdateScreen();
void Save_InitCardHeader();

#endif
