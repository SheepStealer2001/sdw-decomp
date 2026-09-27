#ifndef SDW_ENGINE_CRC32_H
#define SDW_ENGINE_CRC32_H

/* The functions and globals crc32.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

u32 Crc32(const u8 *data, int len); /* 0x55f6b0 */

/* The functions and globals crc32.cpp defines, declared once for every file that uses them. */

extern const u32 g_crc32Table[256];

#endif
