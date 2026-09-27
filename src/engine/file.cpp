/* T269 - original object "File.cpp" (guessed name; language not provable, kept C++ for the symbol names): the File_*
 * layer 0x531f00-0x5321cf.
 *   .text  0x531f00-0x5321d0  File_Open .. File_SaveWhole (10)
 *   .data  0x57bc7c-0x57bcd0  g_fileLastSize, g_fileBytesRead, then the two $SG strings of File_LoadWhole
 *                             (0x57bccc-0x57bcd0 is the 8-alignment padding before T270's .data)
 *   .bss   0x6dd9b0-0x6ddab8  g_filePath, g_fileBaseDir
 */
/* BYTES: dead-code, layout, slot-name. */
/* BYTES(layout): written '= { 0 }' only to keep definition order in .bss */
/* match-addr: _open=0x567d55 _lseek=0x567c7d _read=0x56803b _write=0x568279 _close=0x568469   the CRT's low-level
 * I/O under the names this file calls: data/symbols.csv has them as open / lseek / read / write / close (identified by
 * their call shapes; 0x567d55 is the thunk that calls _sopen with _SH_DENYNO 0x40). */
/*
 * File I/O helpers: a FileHandle {fd, size, remaining} over the CRT's low-level _open/_lseek/_read/_write/_close, the
 * whole-file load / save used by the pad recorder (src/engine/input.cpp).
 * Names are descriptive (the binary has no symbols); local names are chosen for their stack slots (src/README.md).
 */
#include "sdw_classes.h"
#include "../sdk/crt.h"

/* ---- the game's own helpers ---- */
void Debug_Printf(const char *fmt, ...); /* 0x5363a5, an empty stub in the retail build */
#include "crc32.h"

s32 File_Read(FileHandle *h, void *buf, s32 len);
s32 File_Close(FileHandle *h);

/* ---- globals defined here ---- */
s32 g_fileLastSize = 1;  /* 0x57bc7c  size of the last file File_Open opened */
s32 g_fileBytesRead = 1; /* 0x57bc80  bytes File_Read has read since File_Open (starts at 1) */
/* .bss: VC6 puts a file's globals WITHOUT an initialiser first, ordered by a hash of their names (g_fileBaseDir's key
 * 460 < g_filePath's 630 would put the base directory first), and the initialised ones after them in definition
 * order. The exe has g_filePath first, so both are written `= { 0 }`. */
char g_filePath[0x104] = {0}; /* 0x6dd9b0  base directory + name, built by File_Open */
char g_fileBaseDir[4] = {0};  /* 0x6ddab4  prefix File_Open puts before every name; never written: "" */

/* 0x531f00 - opens g_fileBaseDir + name read-only (O_BINARY), fills h and returns the size, or -1. */
s32 File_Open(const char *name, FileHandle *h)
{
    strcpy(g_filePath, g_fileBaseDir);
    strcat(g_filePath, name);
    h->fd = _open(g_filePath, _O_BINARY);
    if (h->fd == -1)
        return -1;
    h->size = _lseek(h->fd, 0, SEEK_END);
    h->remaining = h->size;
    _lseek(h->fd, 0, SEEK_SET);
    g_fileLastSize = h->size;
    g_fileBytesRead = 1;
    return h->size;
}

/* 0x531f9d - creates / opens path for writing (O_RDWR | O_CREAT, S_IREAD | S_IWRITE); 0, or -1 on failure. Signed:
 * File_SaveWhole tests it with movsx (0x532197). */
s8 File_Create(const char *path, FileHandle *h)
{
    h->fd = _open(path, _O_RDWR | _O_CREAT, _S_IWRITE | _S_IREAD);
    if (h->fd == -1)
        return -1;
    return 0;
}

/* 0x531fcb - reads len bytes at offset; len or -1. */
s32 File_ReadAt(FileHandle *h, void *buf, s32 offset, s32 len)
{
    _lseek(h->fd, offset, SEEK_SET);
    if (_read(h->fd, buf, len) != len)
        return -1;
    return len;
}

/* 0x532007 */
void File_Seek(FileHandle *h, s32 pos)
{
    _lseek(h->fd, pos, SEEK_SET);
}

/* 0x532020 - reads a u32 CRC, then len bytes that must hash (Crc32) to it; len or -1. */
/* BYTES(slot-name): names chosen for their stack slots: result -4, crc -8 */
s32 File_ReadChecked(FileHandle *h, void *buf, s32 len)
{
    u32 crc; /* names chosen for their stack slots: result -4, crc -8 */
    s32 result;

    result = File_Read(h, &crc, 4);
    if (result != 4)
        return -1;
    result = File_Read(h, buf, len);
    if (result != len)
        return -1;
    if (crc != Crc32((u8 *)buf, len)) /* cast kept: Crc32 hashes bytes; the caller's buffer is untyped */
        return -1;
    return result;
}

/* 0x53208b */
s32 File_Read(FileHandle *h, void *buf, s32 len)
{
    if (_read(h->fd, buf, len) != len)
        return -1;
    h->remaining -= len;
    g_fileBytesRead += len;
    return len;
}

/* 0x5320d1 - 0 on a full write, else -1. */
s32 File_Write(FileHandle *h, const void *buf, s32 len)
{
    if (_write(h->fd, buf, len) != len)
        return -1;
    return 0;
}

/* 0x5320f8 */
s32 File_Close(FileHandle *h)
{
    _close(h->fd);
    return 0;
}

/* 0x53210d - the whole file in a malloc'd block (no CRC), or 0. */
/* BYTES(slot-name): names chosen for their stack slots: data -4, ret -8, file -0x18 */
/* BYTES(dead-code): ret is never used: it fills the dead word the original frame has at -8 */
void *File_LoadWhole(const char *name)
{
    /* names chosen for their stack slots: data -4, ret -8 (never used), file -0x18 */
    FileHandle file;
    s32 ret;
    void *data;

    Debug_Printf("***********LOADFILE CALLED (no chksum)*********\n");
    if (File_Open(name, &file) <= 0)
        goto error;
    data = malloc(file.size);
    File_Read(&file, data, file.size);
    File_Close(&file);
    return data;
error:
    Debug_Printf("Error in _LoadFile\n");
    return 0;
}

/* 0x532181 - writes len bytes to path; 0, or -1 (0xff) on failure. */
/* BYTES(dead-code): status is never used: it fills the dead word the original frame has at ebp-4 */
u8 File_SaveWhole(const char *path, const void *buf, s32 len)
{
    FileHandle file;
    s32 status; /* never used: the original frame has a dead word at ebp-4 */

    if (File_Create(path, &file))
        return -1;
    if (File_Write(&file, buf, len))
        return -1;
    File_Close(&file);
    return 0;
}
