/*
 * T300 - original object guessed as Registry.cpp (data/tu_map.json). Ranges: .text 0x55f700-0x55fbf6, .data 0x57ebd4-0x57ecd4
 * (the registry key/value string literals, /Od $SG in the order the compiler meets them), .bss 0x71af50-0x71af54 (the
 * four "" lpClass literals: VC6 puts a "" literal in _BSS). The ten Reg_* functions 0x55f700-0x55fbf5; the pad
 * helpers before them (Pad_MenuHeld/Pressed) are T298 and Crc32 is T299.
 * VC6 /Od hashes local names into frame slots; the local names were picked to reproduce the frames (src/README.md,
 * tools/vc6_locals.py).
 * The registry lives under HKEY_LOCAL_MACHINE\SOFTWARE\Infogrames\Sheep, Dog'n Wolf DX7 (docs/01).
 */
#include "sdw_classes.h"
#include "../sdk/win32.h"

#include "../sdk/crt.h"
#define HKEY_LOCAL_MACHINE ((HKEY)0x80000002)
#define KEY_ALL_ACCESS 0x2001f
#define REG_BINARY 3

LONG Reg_OpenAppRoot(HKEY *key);
void Reg_CloseKey2(HKEY *key);

/* 0x55f700 - creates/opens HKLM\SOFTWARE\Infogrames\Sheep, Dog'n Wolf DX7 into *key; on failure *key is closed and 0. */
LONG Reg_OpenAppRoot(HKEY *key)
{
    LONG result;
    HKEY root = HKEY_LOCAL_MACHINE;
    DWORD dwDisposition;
    result = RegCreateKeyExA(root, "SOFTWARE", 0, "", REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, 0, key, &dwDisposition);
    if (result == ERROR_SUCCESS) {
        result =
            RegCreateKeyExA(*key, "Infogrames", 0, "", REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, 0, key, &dwDisposition);
        if (result == ERROR_SUCCESS)
            result = RegCreateKeyExA(*key, "Sheep, Dog'n Wolf DX7", 0, "", REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, 0,
                                     key, &dwDisposition);
    }
    if (result != ERROR_SUCCESS) {
        if (*key)
            RegCloseKey(*key);
        *key = 0;
    }
    return result;
}

/* 0x55f7c5 - closes and zeroes *key (the same body as Reg_CloseKey 0x55f93a). */
void Reg_CloseKey2(HKEY *key)
{
    if (*key) {
        RegCloseKey(*key);
        *key = 0;
    }
}

/* 0x55f7e7 - deletes the Progress, Config, Setup and CfgGame subkeys and then the app key itself; 1 if the last delete
 * succeeded. */
u8 Reg_DeleteAppKeys()
{
    HKEY key = 0;
    HKEY hive = HKEY_LOCAL_MACHINE;
    u8 ok = 0;
    if (RegOpenKeyExA(hive, "SOFTWARE", 0, KEY_ALL_ACCESS, &key) == ERROR_SUCCESS) {
        if (RegOpenKeyExA(key, "Infogrames", 0, KEY_ALL_ACCESS, &key) == ERROR_SUCCESS) {
            HKEY app = 0;
            if (RegOpenKeyExA(key, "Sheep, Dog'n Wolf DX7", 0, KEY_ALL_ACCESS, &app) == ERROR_SUCCESS) {
                RegDeleteKeyA(app, "Progress");
                RegDeleteKeyA(app, "Config");
                RegDeleteKeyA(app, "Setup");
                RegDeleteKeyA(app, "CfgGame");
                RegCloseKey(app);
                if (RegDeleteKeyA(key, "Sheep, Dog'n Wolf DX7") == ERROR_SUCCESS)
                    ok = 1;
            }
        }
    }
    if (key)
        RegCloseKey(key);
    return ok;
}

/* 0x55f8dc - creates/opens the subkey `name` of the app key into *out. */
LONG Reg_CreateSubKey(HKEY *out, const char *name)
{
    LONG result;
    HKEY root = 0;
    DWORD dwDisposition;
    result = Reg_OpenAppRoot(&root);
    if (result == ERROR_SUCCESS)
        result = RegCreateKeyExA(root, name, 0, "", REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, 0, out, &dwDisposition);
    Reg_CloseKey2(&root);
    return result;
}

/* 0x55f93a - closes and zeroes *key. */
void Reg_CloseKey(HKEY *key)
{
    if (*key) {
        RegCloseKey(*key);
        *key = 0;
    }
}

/* 0x55f95c - deletes the subkey `name` of the app key; 1 on success. No callers. */
u8 Reg_DeleteSubKey(const char *name)
{
    HKEY root = 0;
    u8 ok = 0;
    if (Reg_OpenAppRoot(&root) == ERROR_SUCCESS && RegDeleteKeyA(root, name) == ERROR_SUCCESS)
        ok = 1;
    Reg_CloseKey2(&root);
    return ok;
}

/* 0x55f9a6 - 1 if HKLM\SOFTWARE\Infogrames\Sheep, Dog'n Wolf DX7\<name> can be opened. */
u8 Reg_SubKeyExists(const char *name)
{
    HKEY key = 0;
    HKEY hive = HKEY_LOCAL_MACHINE;
    u8 exists = 0;
    if (RegOpenKeyExA(hive, "SOFTWARE", 0, KEY_ALL_ACCESS, &key) == ERROR_SUCCESS &&
        RegOpenKeyExA(key, "Infogrames", 0, KEY_ALL_ACCESS, &key) == ERROR_SUCCESS &&
        RegOpenKeyExA(key, "Sheep, Dog'n Wolf DX7", 0, KEY_ALL_ACCESS, &key) == ERROR_SUCCESS &&
        RegOpenKeyExA(key, name, 0, KEY_ALL_ACCESS, &key) == ERROR_SUCCESS)
        exists = 1;
    if (key)
        RegCloseKey(key);
    return exists;
}

/* 0x55fa50 - 1 if the app subkey `subkey` holds a non-empty REG_BINARY value `valueName` (default "0"). Despite the
 * table name it opens nothing for the caller: the key is closed again before returning. Its one caller,
 * Card_OpenRegistryRoot 0x52a2d6, asks for ("Progress", "SdwSaves"): does a save exist. */
u8 Reg_HasBinaryValue(const char *subkey, const char *valueName)
{
    HKEY key = 0;
    HKEY hive = HKEY_LOCAL_MACHINE;
    DWORD len = 0;
    u8 exists = 0;
    DWORD valueType;
    LONG error;
    if (!valueName)
        valueName = "0";
    if (RegOpenKeyExA(hive, "SOFTWARE", 0, KEY_ALL_ACCESS, &key) == ERROR_SUCCESS &&
        RegOpenKeyExA(key, "Infogrames", 0, KEY_ALL_ACCESS, &key) == ERROR_SUCCESS &&
        RegOpenKeyExA(key, "Sheep, Dog'n Wolf DX7", 0, KEY_ALL_ACCESS, &key) == ERROR_SUCCESS &&
        RegOpenKeyExA(key, subkey, 0, KEY_ALL_ACCESS, &key) == ERROR_SUCCESS) {
        error = RegQueryValueExA(key, valueName, 0, &valueType, 0, &len);
        if (error == ERROR_SUCCESS && valueType == REG_BINARY && len != 0)
            exists = 1;
    }
    if (key)
        RegCloseKey(key);
    return exists;
}

/* 0x55fb41 - reads the REG_BINARY value `name` (default "0") of key into buf; returns the byte count, or zero-fills buf
 * and returns 0 if the value is missing, not binary or larger than bufSize (ERROR_MORE_DATA). A shorter value is read
 * as it is and the rest of buf is left untouched. */
DWORD Reg_ReadBinary(HKEY key, const char *name, void *buf, DWORD bufSize)
{
    DWORD size = 0;
    LONG rc;
    DWORD type;
    if (!name)
        name = "0";
    if (key) {
        size = bufSize;
        /* cast kept: the SDK types the value buffer as BYTE *; the caller hands over any buffer */
        rc = RegQueryValueExA(key, name, 0, &type, (u8 *)buf, &size);
        if (rc != ERROR_SUCCESS || type != REG_BINARY) {
            memset(buf, 0, bufSize);
            size = 0;
        }
    }
    return size;
}

/* 0x55fbb2 - writes data as the REG_BINARY value `name` (default "0") of key; 1 on success. */
u8 Reg_WriteBinary(HKEY key, const char *name, const void *data, DWORD size)
{
    u8 ok = 0;
    if (!name)
        name = "0";
    if (key) {
        /* cast kept: the SDK types the value data as const BYTE *; the caller hands over any data */
        if (RegSetValueExA(key, name, 0, REG_BINARY, (const u8 *)data, size) == ERROR_SUCCESS)
            ok = 1;
    }
    return ok;
}
