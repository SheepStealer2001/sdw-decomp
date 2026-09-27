/*
 * T261 - guessed original name: Card.cpp. SheepD3D.exe .text 0x52a2a0-0x52a4b4, .data 0x57bbe4-0x57bc38,
 * .bss 0x6d6fd8-0x6d6fe0.
 * The registry-backed "memory card": the PC port keeps the PS1 card API and answers it from
 * HKLM\...\Progress\SdwSaves; the card status stubs.
 *
 * Reg_OpenProgressKey and Card_SaveExists return a full int (mov eax,1 / xor eax,eax; bool or u8 gives neg/sbb or a byte
 * return), and Card_*'s first parameter is the PS1 card file name the callers push (g_mcardFileName, 0x54aa05) and
 * these PC versions ignore. The 7 string literals are the object's .data ($SG, in the order the compiler meets them);
 * g_regProgressKey is its .bss.
 */
/* BYTES: cast, view. */
/* BYTES(cast): Reg_OpenProgressKey, Card_SaveExists: returns a full int: bool or u8 would give neg / sbb or a byte return, the original has mov eax,1 / xor eax,eax */
#include "sdw_types.h"
#include "sdw_enums.h"

#include "../sdk/windef.h"
#include "../sdk/crt.h"

/* ---- the game's own functions ---- */
LONG Reg_CreateSubKey(HKEY *out, const char *name); /* 0x55f8dc */
void Reg_CloseKey(HKEY *key);                       /* 0x55f93a */
#include "registry.h"
DWORD Reg_ReadBinary(HKEY key, const char *name, void *buf, DWORD bufSize);   /* 0x55fb41 */
u8 Reg_WriteBinary(HKEY key, const char *name, const void *data, DWORD size); /* 0x55fbb2 */

/* ---- the registry "memory card" ---- */
HKEY g_regProgressKey; /* 0x6d6fd8  the open Progress subkey (no table name yet) */

/* 0x52a2a0 - opens (creates) the Progress subkey; 1 on success. */
s32 Reg_OpenProgressKey()
{
    if (Reg_CreateSubKey(&g_regProgressKey, "Progress") == 0)
        return 1;
    return 0;
}

/* 0x52a2c4 */
void Reg_CloseProgressKey()
{
    Reg_CloseKey(&g_regProgressKey);
}

/* 0x52a2d6 - 1 when Progress\SdwSaves holds a non-empty binary value. */
/* BYTES(view): the unused file-name parameter stays because every caller pushes it (0x54b763, 0x54c4e2) */
s32 Card_SaveExists(
    const char *
        fileName) /* the file name is passed and ignored: Card_StateMachine pushes it (0x54b763-0x54b76e, 0x54c4e2-0x54c4ee) */
{
    if (Reg_HasBinaryValue("Progress", "SdwSaves") == 1)
        return 1;
    return 0;
}

/* 0x52a2fe - the PS1 card status query's answer on PC. */
s32 CardStub_Status()
{
    return CARD_PRESENT;
}

/* 0x52a308 - free blocks on a PS1 card. */
s32 CardStub_FreeBlocks()
{
    return 0xf;
}

/* 0x52a312 */
s32 CardStub_Status2()
{
    return CARD_FORMAT_OK;
}

/* 0x52a31c */
s32 CardStub_Status3(
    const char *fileName) /* the file name is passed and ignored: Card_CommitBlock pushes it (0x54ab27-0x54ab32) */
{
    return CARD_PREWRITE_OK;
}

/* 0x52a326 - reads nBlocks 8 KB card blocks of SdwSaves into dest; CARD_READ_OK when the whole size was read, else
 * CARD_READ_SHORT and dest untouched. fileName (the PS1 card file) is not used. No callers. */
s32 Card_ReadWholeBlocks(const char *fileName, u32 nBlocks, void *dest)
{
    s32 result = CARD_READ_SHORT;
    u32 size = nBlocks << 13;
    u8 *buf = new u8[size];
    if (Reg_ReadBinary(g_regProgressKey, "SdwSaves", buf, size) == size) {
        memcpy(dest, buf, size);
        result = CARD_READ_OK;
    }
    delete buf;
    return result;
}

/* 0x52a3a6 - as Card_ReadWholeBlocks, for nBytes rounded up to 128-byte PS1 card frames. */
s32 Card_ReadBlocks(const char *fileName, u32 nBytes, void *dest)
{
    s32 result = CARD_READ_SHORT;
    u32 size = ((nBytes + 0x7f) >> 7) << 7;
    u8 *buf = new u8[size];
    if (Reg_ReadBinary(g_regProgressKey, "SdwSaves", buf, size) == size) {
        memcpy(dest, buf, size);
        result = CARD_READ_OK;
    }
    delete buf;
    return result;
}

/* 0x52a42c - writes nBlocks 8 KB blocks from src to SdwSaves: CARD_WRITE_OK or CARD_WRITE_FAILED. */
s32 Card_WriteBlocks(const char *fileName, u32 nBlocks, void *src)
{
    u32 size = nBlocks << 13;
    if (Reg_WriteBinary(g_regProgressKey, "SdwSaves", src, size) == 1)
        return CARD_WRITE_OK;
    return CARD_WRITE_FAILED;
}

/* 0x52a46d - as Card_WriteBlocks, for nBytes rounded up to 128-byte frames. */
s32 Card_WriteFrames(const char *fileName, u32 nBytes, void *src)
{
    u32 size = ((nBytes + 0x7f) >> 7) << 7;
    if (Reg_WriteBinary(g_regProgressKey, "SdwSaves", src, size) == 1)
        return CARD_WRITE_OK;
    return CARD_WRITE_FAILED;
}
