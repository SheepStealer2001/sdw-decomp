#ifndef SDW_ENGINE_BS_IO_H
#define SDW_ENGINE_BS_IO_H

/* The functions and globals bs_io.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

void *Bs_LoadFile(const char *path, u32 *sizeOut); /* 0x41b72d */


/* The functions and globals bs_io.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

struct FILE;

u32 Bs_FileSize(FILE *f); /* 0x41b6e0 */

#endif
