/* match-addr: ??0BsStream@@QAE@PBD@Z=0x41b1a0 */
/*
 * T027 - original object BsStream.cpp (guessed name): SheepD3D.exe .text 0x41b1a0-0x41b3dd plus the COMDAT
 * ??_GBsStream 0x41b3e0-0x41b40e, .rdata 0x5743fc-0x574400 (BsStream's vtable).
 *
 * The BsStream part of the byte-stream code (0x41b1a0-0x41b40e, 7 functions and the scalar deleting destructor);
 * BsFile is src/engine/bs_file.cpp.
 *
 * BsStream 0x14 { vtbl 0x5743fc; u8 ok; u8 *data; u32 size; u32 cursor; }: a byte stream over a whole file image.
 * Its only virtual function is the destructor. The match-addr line places the constructor, whose table name
 * (BsStream_Open) is not the ..._Ctor the matcher derives from ??0.
 */
#include "sdw_types.h"

#define SDW_MEMBERS_BsStream BsStream(const char *path); /* 0x41b1a0 BsStream_Open */
#include "sdw_classes.h"

#include "bs_io.h"

/* ------------------------------------------------------------------------------------------------ BsStream */

/* 0x41b1a0: loads the whole file; ok = 1 when it has at least one byte. */
BsStream::BsStream(const char *path)
{
    s32 len;

    cursor = 0;
    /* cast kept: the loader returns the image as a void * and the size through a u32 *, and len is tested signed */
    data = (u8 *)Bs_LoadFile(path, (u32 *)&len);
    if (len <= 0) {
        ok = 0;
    } else {
        ok = 1;
        size = len;
    }
}

/* 0x41b1fa */
BsStream::~BsStream()
{
    if (data)
        delete data;
}

/* 0x41b22e: bounds-checked absolute seek. */
bool BsStream::Seek(u32 pos)
{
    bool moved;

    if (pos >= 0 && pos < size) {
        cursor = pos;
        moved = 1;
    } else {
        moved = 0;
    }
    return moved;
}

/* 0x41b264: bounds-checked relative seek. */
bool BsStream::Skip(int delta)
{
    bool moved;

    if (cursor + delta >= 0 && cursor + delta < size) {
        cursor += delta;
        moved = 1;
    } else {
        moved = 0;
    }
    return moved;
}

/* 0x41b2ad */
u8 BsStream::ReadU8(u8 advance)
{
    u8 v;

    v = data[cursor];
    if (advance == 1)
        cursor += 1;
    return v;
}

/* 0x41b2e9: little-endian. */
u16 BsStream::ReadU16(u8 advance)
{
    u16 v;

    v = data[cursor];
    v += (u16)(data[cursor + 1] << 8);
    if (advance == 1)
        cursor += 2;
    return v;
}

/* 0x41b34c: little-endian. */
u32 BsStream::ReadU32(u8 advance)
{
    u32 v;

    v = data[cursor];
    v += data[cursor + 1] << 8;
    v += data[cursor + 2] << 16;
    v += data[cursor + 3] << 24;
    if (advance == 1)
        cursor += 4;
    return v;
}

/* 0x41b3e0 BsStream_ScalarDeletingDtor: generated from the virtual destructor. */
