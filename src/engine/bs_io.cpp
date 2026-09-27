/* match-addr: ftell=0x566f6d fseek=0x566eb4 */
/*
 * T029 - original object BsIO.cpp (guessed name; Bs_FileSize / Bs_LoadFile / Bs_WriteFile): SheepD3D.exe .text
 * 0x41b6e0-0x41b7eb, .data 0x5794b8-0x5794c0 (the "rb" and "w" mode literals). No .rdata.
 *
 * The engine's file helpers (3 functions); the BsFile reader that uses them is T030 (src/engine/bs_file.cpp).
 *
 * ftell / fseek are CRT functions the symbol tables do not name; the match-addr line places them (0x566f6d is the
 * LIBCMT ftell, 0x566eb4 fseek; both called only from Bs_FileSize).
 */
#include "sdw_types.h"
#include "../sdk/crt.h"

/* ------------------------------------------------------------------------------------------------ file helpers */

/* 0x41b6e0: the file's length, leaving its position where it was. */
u32 Bs_FileSize(FILE *f)
{
    long n;
    long pos;

    pos = ftell(f);
    fseek(f, 0, SEEK_END);
    n = ftell(f);
    fseek(f, pos, SEEK_SET);
    return n;
}

/* 0x41b72d: reads a whole file into a new[]'d buffer; NULL with *sizeOut = 0 when it cannot be opened. */
void *Bs_LoadFile(const char *path, u32 *sizeOut)
{
    void *buf;
    FILE *f;

    f = fopen(path, "rb");
    if (f == 0) {
        *sizeOut = 0;
        return 0;
    }
    *sizeOut = Bs_FileSize(f);
    buf = new char[*sizeOut];
    fread(buf, 1, *sizeOut, f);
    fclose(f);
    return buf;
}

/* 0x41b7ad: writes a buffer to a file (mode "w"). No callers. */
void Bs_WriteFile(const char *path, const void *buf, u32 size)
{
    FILE *f;

    f = fopen(path, "w");
    fwrite(buf, 1, size, f);
    fclose(f);
}
