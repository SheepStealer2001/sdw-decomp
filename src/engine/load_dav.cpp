/*
 * T282, guessed original file LoadDav.cpp: .text 0x548430-0x5486a7, .data 0x57c6ac-0x57c72c
 * (the five Load_DAV strings): TexAtlas_GetPage 0x548430, Load_DAV 0x54843c and Dav_Free 0x54867e (the other loader
 * objects are T281 src/engine/list.cpp and T283 src/engine/jpeg_mlt.cpp).
 *
 * The local names in Load_DAV are chosen for their stack slots (VC6 /Od orders locals by a hash of their names,
 * tools/vc6_locals.py); other names give the same code at other offsets. Its `deadWord` local is a device that
 * reproduces the frame, not recovered source (see there).
 */
/* BYTES: dead-code, slot-name. */
#include "sdw_classes.h"

#include "../sdk/crt.h"

/* The DAV directory (DavHeader.dir): packed, its pointers sit at +6/+0xa/+0x12 (Load_DAV 0x5485a5, 0x5485c1, 0x5485dd), which
 * the struct generator cannot lay out, so it is declared here. */
#pragma pack(push, 1)
struct DavDirectory {
    u16 indexCount;  /* +0x00 entries of `indices`; Res_GetValidatedIdList bounds an entry's index by it */
    u16 bitmapCount; /* +0x02 records in `bitmaps`; Res_GetValidatedIdList bounds an entry's value by it */
    u16 unk04;
    u16 *indices;          /* +0x06 relocated by Load_DAV; the id-list entries point into it */
    DavBitmapRec *bitmaps; /* +0x0a relocated by Load_DAV; 10-byte records (TexAtlas_GetPage) */
    u32 fileSize;          /* +0x0e size of the whole .DAV file */
    u32 *idLists;          /* +0x12 relocated by Load_DAV: {u32 count; id-list records} -> g_idListBlob */
};
#pragma pack(pop)

/* ---- globals (addresses in SheepD3D.exe) ---- */
#include "id_list.h"
#include "file.h"

/* ---- functions ---- */
void Debug_Printf(const char *fmt, ...); /* 0x5363a5, a no-op stub */
s32 Dav_Free(Dav *dav);                  /* 0x54867e, below */

/* 0x548430 - the texture page of a DAV bitmap record (returned widened: movzx eax, word ptr [eax+8]). */
u32 TexAtlas_GetPage(DavBitmapRec *rec)
{
    return rec->page;
}

/* 0x54843c - reads the .DAV file at path into dav: a 0x44-byte probe read finds the directory and the file size, then
 * the whole file is read into dav->blob and its internal offsets become pointers. 0, or -1 (dav freed) on failure.
 * The version check is inverted: "Bad version" is printed when the magic DOES match (Debug_Printf is a no-op anyway).
 * When the file does not open, the failure path frees `probe` before anything was assigned to it (0x54864a tests the
 * uninitialised ebp-8; the zeroed local is `unused` at ebp-4). */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py) */
/* BYTES(dead-code): deadWord is never used: it fills the unused 4-byte slot the original frame has at ebp-0x1c */
/* BYTES(dead-code): unused is zeroed and never read: the original stores 0 at ebp-4 */
s32 Load_DAV(const char *path, Dav *dav)
{
    /* names chosen for their stack slots (file comment) */
    FileHandle file;
    s32 deadWord; /* match device, not recovered source: the original frame has an unused 4-byte slot at ebp-0x1c */
    u32 headerSize;
    u32 davSize;
    char ver[5];
    DavHeader *probe;
    s32 unused = 0;

    if (File_Open(path, &file) <= 0) {
        Debug_Printf("Load_DAV: DAV File not found\n");
        goto fail;
    }
    headerSize = 0x44;
    probe = (DavHeader *)malloc(headerSize); /* cast kept: malloc returns void * */
    if (File_Read(&file, probe, headerSize) < 0) {
        Debug_Printf("Load_DAV: Read Error\n");
        goto fail;
    }
    /* cast kept: the file stores the directory as an offset from its start; adding the base makes it a pointer */
    probe->dir = (DavDirectory *)((u32)probe->dir + (u32)probe);
    davSize = probe->dir->fileSize;
    if (probe != 0) {
        free(probe);
        probe = 0;
    }
    dav->blob = (u8 *)malloc(davSize); /* cast kept: malloc returns void * */
    File_Seek(&file, 0);
    if (File_Read(&file, dav->blob, davSize) < 0) {
        Debug_Printf("Load_DAV: Read Error\n");
        goto fail;
    }
    File_Close(&file);
    dav->header = (DavHeader *)dav->blob; /* cast kept: the blob is the file, and the file starts with its header */
    sprintf(ver, "VDX7");
    if (strncmp(dav->header->magic, ver, 4) == 0)
        Debug_Printf(" Bad version: Viewer:%s, Dav&War:%s\n", ver, dav->header);
    /* cast kept (these four relocations): the file stores the pointers as offsets from its start */
    dav->header->dir = (DavDirectory *)((u32)dav->header->dir + (u32)dav->blob);
    dav->header->dir->indices = (u16 *)((u32)dav->header->dir->indices + (u32)dav->blob);
    dav->header->dir->bitmaps = (DavBitmapRec *)((u32)dav->header->dir->bitmaps + (u32)dav->blob);
    /* cast kept: file data relocated by its load address */
    dav->header->dir->idLists = (u32 *)((u32)dav->header->dir->idLists + (u32)dav->blob);
    g_idListBlob = dav->header->dir->idLists;
    g_idListCount = *g_idListBlob;
    g_idListBlob++;
    /* cast kept: Res_RelocateIdLists takes the base address as an int */
    Res_RelocateIdLists(g_idListBlob, g_idListCount, (int)dav->blob);
    return 0;

fail:
    File_Close(&file);
    if (probe != 0) {
        free(probe);
        probe = 0;
    }
    Dav_Free(dav);
    return -1;
}

/* 0x54867e - frees the DAV image. Always 0. */
s32 Dav_Free(Dav *dav)
{
    if (dav->blob != 0) {
        free(dav->blob);
        dav->blob = 0;
    }
    return 0;
}
