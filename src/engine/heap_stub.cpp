/*
 * T288, guessed original file: an unnamed stub file. .text 0x54d500-0x54d507 (one function),
 * .data 0x57e6cc-0x57e6d0 (one pointer to it).
 *
 * 0x54d500 sits alone between the memory-card object (T287) and the heap object (T289) with int3 padding on both sides.
 * No instruction calls it; the only reference is the dword 0x57e6cc in .data, which sits between T287's strings and
 * T289's nibble tables. That dword is placed here as this object's only .data item (the map puts it in T288; it could
 * equally be the first .data item of T289 - both give the same bytes). Its name is descriptive
 * (nothing reads it by address, so the tables have no row for it).
 */
#include "sdw_types.h"

/* 0x54d500 - returns 0. No call to it; the only reference is the pointer at 0x57e6cc below. */
s32 Stub_Ret0_54d500(void)
{
    return 0;
}

/* 0x57e6cc - the address of the stub above; no instruction reads it. The name is descriptive. */
s32 (*g_pStubRet0_54d500)(void) = Stub_Ret0_54d500;
