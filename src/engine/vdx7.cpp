/* match-addr: ??0Vdx7@@QAE@PBD@Z=0x41b410 */
/*
 * T028 - original object Vdx7.cpp (guessed name): SheepD3D.exe .text 0x41b410-0x41b6a2 plus the COMDAT ??_GVdx7
 * 0x41b6b0-0x41b6de, .rdata 0x574400-0x574404 (Vdx7's vtable), .data 0x5794b0-0x5794b8 (its "VDX7" magic literal):
 * the constructor, the destructor and the scalar deleting destructor.
 *
 * Vdx7 0x10 { vtbl 0x574400; u8 ok; u32 *entries; Vdx7Record *records (10 bytes each); }: the .DAV companion table of a
 * Black Sheep file. It reads the file through a local BsStream (declared here). The match-addr line places the
 * constructor, whose table name (Vdx7_Open) is not the ..._Ctor the matcher derives. Locals are named for their stack
 * slots (tools/vc6_locals.py).
 */
/* BYTES: slot-name. */
/* BYTES(slot-name): Vdx7::Vdx7: names chosen for their stack slots (tools/vc6_locals.py): bs, head, back, e, nEntries, nrec, ri, addr from EBP-0x14 down */
#include "sdw_types.h"

#define SDW_MEMBERS_BsStream BsStream(const char *path); /* 0x41b1a0 BsStream_Open */
#define SDW_MEMBERS_Vdx7 Vdx7(const char *path);         /* 0x41b410 Vdx7_Open */
#include "sdw_classes.h"

#include "../sdk/crt.h"

/* ------------------------------------------------------------------------------------------------ Vdx7 */

/* 0x41b410: header "VDX7", four skipped u32s, then the u32 at +0x14 is where the counts live: u16 nEntries, u16 nRecords,
 * u16 (unused), u32 offset of the entry table (u16 each), u32 offset of the record table (five u16s each). */
Vdx7::Vdx7(const char *path)
{
    /* names chosen for the stack slots (tools/vc6_locals.py): bs, head, back, e, nEntries, nrec, ri, addr from EBP-0x14 down */
    BsStream bs(path);
    u32 back;
    char head[4];
    u32 e;
    u32 nrec;
    u32 nEntries;
    u32 ri;
    u32 addr;

    ok = 0;
    if (bs.ok == 1) {
        head[0] = bs.ReadU8(1);
        head[1] = bs.ReadU8(1);
        head[2] = bs.ReadU8(1);
        head[3] = bs.ReadU8(1);
        if (strncmp(head, "VDX7", 4) == 0) {
            bs.ReadU32(1);
            bs.ReadU32(1);
            bs.ReadU32(1);
            bs.ReadU32(1);
            bs.Seek(bs.ReadU32(1));
            nEntries = bs.ReadU16(1);
            entries = (u32 *)malloc(nEntries * 4); /* cast kept: malloc returns untyped memory */
            nrec = bs.ReadU16(1);
            records = (Vdx7Record *)malloc(nrec * 10); /* cast kept: malloc returns untyped memory */
            bs.ReadU16(1);
            addr = bs.ReadU32(1);
            back = bs.cursor;
            bs.Seek(addr);
            for (e = 0; e < nEntries; e++)
                entries[e] = bs.ReadU16(1);
            bs.Seek(back);
            addr = bs.ReadU32(1);
            bs.Seek(addr);
            for (ri = 0; ri < nrec; ri++) {
                records[ri].x = bs.ReadU16(1);
                records[ri].w = bs.ReadU16(1);
                records[ri].y = bs.ReadU16(1);
                records[ri].h = bs.ReadU16(1);
                records[ri].page = bs.ReadU16(1);
            }
            ok = 1;
        }
    }
}

/* 0x41b65e */
Vdx7::~Vdx7()
{
    if (entries)
        free(entries);
    if (records)
        free(records);
}

/* 0x41b6b0 Vdx7_ScalarDeletingDtor: generated from the virtual destructor. */
