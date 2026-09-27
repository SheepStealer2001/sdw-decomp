#ifndef SDW_ENGINE_REGISTRY_H
#define SDW_ENGINE_REGISTRY_H

/* The functions and globals registry.cpp defines, declared once for every file that uses them. */
#include "sdw_types.h"

u8 Reg_DeleteAppKeys();
u8 Reg_HasBinaryValue(const char *subkey, const char *valueName); /* 0x55fa50 */

#endif
