/*
 * T285, guessed original file LoadWar.cpp: .text 0x549d20-0x54a222, .data 0x57c7cc-0x57c838
 * (the four Load_WAR strings; the object's .data is followed by padding to the 8-aligned .data of T287).
 * Load_WAR, the empty per-resource hook it calls, and Load_FreeWAR 0x54a1ba; the angle/scale helpers and Load_WarMeshes
 * that follow are the next object, T286 src/engine/load_warmeshes.cpp.
 *
 * The .WAR loader, first part: Load_WAR and the empty per-resource hook it calls (SheepD3D.exe 0x549d20-0x54a11b, then
 * Load_WAR's switch tables to 0x54a1b9), then Load_FreeWAR. The object starts after the int3 padding that ends the sound
 * object.
 *
 * Load_WAR reads the whole .WAR into one malloc'd blob (file bytes 4.. to blob+4, so blob offsets are file offsets; the
 * CRC slot blob[0..3] is never filled), checks the "V2.6" version (only a Debug_Printf on mismatch), and relocates every
 * resource in place: the table at blob+0x10 holds one u32 per resource, type << 24 | byte offset, with bit 0x40 of the
 * type marking a resource that is not counted. Mesh records (types 3, 0xb, 0x26) and type-4 records have their pointer
 * words turned from blob offsets into addresses. Returns the number of counted resources (types 3/4/5/0xb/0x26
 * without bit 0x40), or -1: file missing, read or CRC error, or a type-1/2/unknown resource. Every failure path closes
 * the file again and frees the WAR - including the ones taken after the file was already closed.
 *
 * The names are descriptive, not recovered; local names were chosen for their stack slots (src/README.md).
 */
/* BYTES: dead-code, slot-name. */
#include "sdw_enums.h"
#include "sdw_classes.h"

#include "../sdk/crt.h"

/* ---- the game's own helpers ---- */
#include "file.h"
#include "text.h"
#include "maths.h"
#include "game_state.h"
#include "../app/app_main.h"
#include "../objects/instance.h"
void Debug_Printf(const char *fmt, ...); /* 0x5363a5, a no-op stub */
void Load_FreeWAR(WarFile *war);         /* 0x54a1ba */

/* ---- globals (addresses in SheepD3D.exe) ---- */

#define WAR_RES_TYPE(e) ((e) >> 24 & 0xff)
#define WAR_RES_OFFSET(e) ((e) & 0xffffff)

/* 0x549d20 - called with every relocated mesh-type record; compiled empty. */
void Load_WAR_ResourceHook_Stub(void *res) {}

/* 0x549d25 - loads and relocates the level's .WAR into war (= g_pDav->war). The counted-resource total, or -1. */
/* BYTES(slot-name): declared in this order for the original slots: result -4, p -8, color -9, version -0x14, i -0x18, len -0x1c, k -0x1e, counter -0x20, maxSize -0x24, file -0x34; loop body scnRec -0x38, mesh4 -0x3c, meshRec -0x40 */
/* BYTES(dead-code): result, maxSize and scnRec are set and never read, as in the original */
int Load_WAR(const char *path, WarFile *war)
{
    /* declared in this order for the original slots: result -4 (set, never read), p -8, color -9, version -0x14, i -0x18,
     * len -0x1c, k -0x1e, counter -0x20, maxSize -0x24 (set, never read), file -0x34; then, in the loop body, scnRec
     * -0x38 (set, never read), mesh4 -0x3c, meshRec -0x40 */
    u32 *p;
    s32 result;
    u8 color;
    char version[8];
    u32 i;
    u16 k;
    u32 len;
    u16 counter;
    u32 maxSize;
    FileHandle file;

    result = 0;
    counter = 0;
    war->table = 0;
    war->blob = 0;
    memset(&g_texScrollListIds, 0, 12);
    g_animNameTable = 0;
    if (File_Open(path, &file) <= 0) {
        Debug_Printf("Load_WAR: WAR File not found\n");
        goto fail;
    }
    maxSize = 0xcaa30;
    war->blob = (u8 *)malloc(file.size); /* cast kept: malloc returns untyped memory */
    if (File_ReadChecked(&file, war->blob + 4, file.size - 4) < 0) {
        Debug_Printf("Load_WAR: Read Error\n");
        goto fail;
    }
    File_Close(&file);
    war->header = (WarHeader *)war->blob;   /* cast kept: the file's first bytes are its header */
    war->table = (u32 *)(war->blob + 0x10); /* cast kept: the resource table follows the header in the file bytes */
    color = war->header->clearR;
    war->header->clearR = 0; /* NUL-terminates the 4-byte version string at +4 for the compare */
    Text_Sprintf(version, "V%u.%u", 2, 6);
    if (!Str_IsPrefixOf(version, war->header->version))
        Debug_Printf(" Bad version: Viewer:%s, Dav&War:%s\n", version, war->header->version);
    war->header->clearR = color;
    for (i = 0; i < war->header->resourceCount; i++) {
        u32 *meshRec;
        u32 *mesh4;
        u8 *scnRec;

        switch (WAR_RES_TYPE(war->table[i]) & WAR_RES_TYPE_MASK) {
            case WAR_RES_IGNORED_1:
            case WAR_RES_IGNORED_2:
                goto fail;
            case WAR_RES_MESH:
            case WAR_RES_MESH_B:
            case WAR_RES_SKY:
                /* a mesh: words 0, 1 and (when set) 3 are blob offsets */
                /* cast kept: a resource is raw bytes at its table offset */
                meshRec = (u32 *)(WAR_RES_OFFSET(war->table[i]) + war->blob);
                meshRec[0] += (u32)war->blob; /* cast kept: relocation: a blob offset becomes an address */
                meshRec[1] += (u32)war->blob; /* cast kept: relocation: a blob offset becomes an address */
                if (meshRec[3] != 0)
                    meshRec[3] += (u32)war->blob; /* cast kept: relocation: a blob offset becomes an address */
                Load_WAR_ResourceHook_Stub(meshRec);
                if (!(WAR_RES_TYPE(war->table[i]) & WAR_RES_UNCOUNTED))
                    counter++;
                break;
            case WAR_RES_MODEL:
                /* words 0, 1, 4, 5 and (when set) 3 are blob offsets; word 4 then points at two counted lists of offsets
             * {u32 n; u32 off[n]; u32 m; u32 off[m]}, 0 = none */
                /* cast kept: a resource is raw bytes at its table offset */
                mesh4 = (u32 *)(WAR_RES_OFFSET(war->table[i]) + war->blob);
                mesh4[0] += (u32)war->blob; /* cast kept: relocation: a blob offset becomes an address */
                mesh4[1] += (u32)war->blob; /* cast kept: relocation: a blob offset becomes an address */
                mesh4[4] += (u32)war->blob; /* cast kept: relocation: a blob offset becomes an address */
                mesh4[5] += (u32)war->blob; /* cast kept: relocation: a blob offset becomes an address */
                if (mesh4[3] != 0)
                    mesh4[3] += (u32)war->blob; /* cast kept: relocation: a blob offset becomes an address */
                len = *(u32 *)mesh4[4];         /* cast kept: word 4 now holds the address of the offset lists */
                p = (u32 *)mesh4[4] + 1;        /* cast kept: word 4 now holds the address of the offset lists */
                for (k = 0; k < len; k++) {
                    if (*p != 0)
                        *p += (u32)war->blob; /* cast kept: relocation: a blob offset becomes an address */
                    p++;
                }
                len = *p;
                p++;
                for (k = 0; k < len; k++, p++) {
                    if (*p != 0)
                        *p += (u32)war->blob; /* cast kept: relocation: a blob offset becomes an address */
                }
                Load_WAR_ResourceHook_Stub(mesh4);
                if (!(WAR_RES_TYPE(war->table[i]) & WAR_RES_UNCOUNTED))
                    counter++;
                break;
            case WAR_RES_SCENARIC:
                /* a scenaric object record: relocated later, by its class's loader; counted even with bit 0x40 */
                scnRec = WAR_RES_OFFSET(war->table[i]) + war->blob;
                counter++;
                break;
            case WAR_RES_TYPE_8:
            case WAR_RES_TYPE_9:
            case WAR_RES_CINEMATIC:
            case WAR_RES_TYPE_27:
            case WAR_RES_COLL_GRID:
            case WAR_RES_COLL_TRIS:
            case WAR_RES_EXPORTS:
            case WAR_RES_HEADER3:
            case WAR_RES_OBJ_GRID:
            case WAR_RES_PAIRS:
            case WAR_RES_ANIM_NAMES:
                break;
            default:
                goto fail;
        }
    }
    return counter;
fail:
    File_Close(&file);
    Load_FreeWAR(war);
    return -1;
}

/* ---- 0x54a1ba: Load_FreeWAR ---- */

void Load_FreeWAR(WarFile *war)
{
    s32 index;
    for (index = 0; index < g_worldObjCount; ++index)
        WorldObj_Free(g_worldObjs[index]);
    if (war->blob) {
        free(war->blob);
        war->blob = 0;
    }
    g_animNameTable = 0;
}
