#ifndef SDW_ENGINE_FILE_H
#define SDW_ENGINE_FILE_H

/* The functions and globals file.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

struct FileHandle;

s32 File_Close(FileHandle *h);                                 /* 0x5320f8 */
void *File_LoadWhole(const char *name);                        /* 0x53210d */
s32 File_Open(const char *name, FileHandle *fh);               /* 0x531f00 */
s32 File_Read(FileHandle *h, void *buf, s32 len);              /* 0x53208b */
s32 File_ReadChecked(FileHandle *fh, void *buf, s32 len);      /* 0x532020 */
u8 File_SaveWhole(const char *path, const void *buf, s32 len); /* 0x532181 */
void File_Seek(FileHandle *h, s32 pos);                        /* 0x532007 */

#endif
