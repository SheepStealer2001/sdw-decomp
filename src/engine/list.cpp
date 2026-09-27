/*
 * T281, guessed original file List.cpp + Loader.cpp: .text 0x547240-0x548423,
 * .data 0x57c3c0-0x57c6ac, .bss 0x6defa8-0x6deff0 (g_listNodeHeap, g_scratch32k).
 * Contents: the Scratch32k_* functions 0x547240-0x5472a0, the list primitives (List_GetAt 0x547434 among them), and the
 * first part of the level loader, GetResourceType 0x547470 .. Res_GetValidatedIdList 0x548381 (the rest of the loader
 * is T282 src/engine/load_dav.cpp and T283 src/engine/jpeg_mlt.cpp).
 * The map records a possible hidden file split at 0x547470 (list half with the .bss, loader half with the .data); the
 * bytes do not decide it, so this is one object as the map has it.
 *
 * - A node is a ListNode {void *data; ListNode *prev; ListNode *next} - 12 bytes, from g_listNodeHeap.
 * - The loader: devices that only pin the original code generation, each noted where used: the local names in
 *   Load_DAVnWAR are chosen for their stack slots (tools/vc6_locals.py); `case WAR_RES_TYPE_8: case WAR_RES_TYPE_9: return;` in
 *   Install_WarResource, `if (0) ;` in Load_FreeLevel, and the source-only inlines Progress_OnFrontEndScreen and
 *   Game_ClearFlags (their names are not recovered).
 */
/* BYTES: dead-code, flow, inline, layout, slot-name. */
/* BYTES(layout): the Cine.h header static: every object including the cinematic header carries this copy in its .data, referenced or not */
#include "sdw_enums.h"
#include "timer.h"

#define SDW_MEMBERS_PolyBatcher \
    PolyBatcher(D3DApp *app, u32 capacity, const char *davPath, s32 *outPageCount); /* 0x416970 PolyBatcher_Construct */
#define SDW_MEMBERS_StreamPlayer StreamPlayer(); /* 0x563640 StreamPlayer_Construct */
#include "scenaric_props.h"
#include "sdw_classes.h"
#define SDW_INLINE_CINE_ISACTIVE 1
#include "cine_inlines.h"
#undef SDW_INLINE_CINE_ISACTIVE

/* ---- this object's .data ---- */

/* 0x57c3c0 - this TU's copy of the cinematic header's static opcode stride table (payload bytes per key, by opcode;
   the cinematic code indexes the copies in src/engine/cine1.cpp and src/engine/cine.cpp). A static table in a header
   that every cinematic .cpp includes: each includer gets its own unreferenced copy in .data, ahead of its own data.
   Local definition standing in for that header (the loader uses g_cinePlayer). */
static u8 g_cineOpStride[9] = {0, 8, 8, 4, 2, 2, 4, 2, 2};

/* 0x57c3cc - "\LEV", referenced by no instruction (a level-directory prefix nothing uses any more). The name is descriptive. */
char g_strLevDir[] = "\\LEV";

/* ---- this object's .bss ---- */
Heap g_listNodeHeap; /* 0x6defa8  the 32 KB list-node heap (every ListNode and list head) */
void *g_scratch32k;  /* 0x6defe8  its arena, malloc'd by Scratch32k_Alloc */

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
#include "game_state.h"
#include "pause_menu.h"
#include "../app/app_main.h"
#include "id_list.h"
#include "obj_grid.h"
#include "cine.h"
#include "draw2d.h"
#include "stream_player.h"
#include "time.h"
#include "progress.h"
#include "maths.h"
#include "text.h"
#include "sound_mgr.h"
#include "jpeg_mlt.h"
#include "load_war.h"
#include "load_warmeshes.h"
#include "scn_register.h"
#include "scenaric.h"
#include "../objects/instance.h"
#include "interface.h"
#include "progress_inventory.h"
#include "emitter.h"
#include "collide.h"
#include "shadow.h"
#include "../objects/camera.h"
#include "tex_scroll.h"
#include "transition.h"
#include "weather.h"
#include "../objects/menu.h"
#include "map.h"
#include "input.h"
#include "../objects/mcard.h"
#include "prompt.h"
#include "file.h"
#include "load_dav.h"
extern u32 g_gameFlags; /* 0x6ddf74  4 = WAR loaded, 8 = DAV loaded */

#define g_camPos (g_camera.pos) /* 0x584d20 */

/* ---- functions ---- */
void Debug_Printf(const char *fmt, ...); /* 0x5363a5, a no-op stub */
void Dialogue_Reset();                   /* 0x539507 */

/* ======================================================================================================================
 * 0x547240-0x5472a0: the 32 KB arena of the list-node heap (g_listNodeHeap), malloc'd once.
 */

/* 0x547240 */
void Scratch32k_Alloc()
{
    g_scratch32k = malloc(0x8000);
}

/* 0x547257 - tears the list-node heap down and frees its arena. */
void Scratch32k_Free()
{
    g_listNodeHeap.Term();
    if (g_scratch32k) {
        free(g_scratch32k);
        g_scratch32k = 0;
    }
}

/* 0x547287 - (re)initialises the list-node heap over the arena; old nodes are simply forgotten. */
void Scratch32k_Install()
{
    /* cast kept: g_scratch32k is the malloc'd arena (void *) that the heap takes as bytes */
    g_listNodeHeap.Init((u8 *)g_scratch32k, 0x8000);
}

/* ======================================================================================================================
 * 0x5472a1-0x547470: the doubly-linked list primitives over the list-node heap.
 */

/* 0x5472a1 - the number of nodes in a list (walks the next chain). */
u32 List_Count(ListNode **list)
{
    u32 count = 0;
    ListNode *node;

    for (node = *list; node; node = node->next)
        count++;
    return count;
}

/* 0x5472d9 - allocates the head cell of an empty list (one pointer, zeroed). */
void List_Create(ListNode ***listOut)
{
    *listOut = (ListNode **)g_listNodeHeap.Alloc(4); /* cast kept: the heap hands out untyped memory, like malloc */
    **listOut = 0;
}

/* 0x5472fa - allocates one 12-byte node; the caller fills its data pointer. */
void List_AllocateNode(ListNode **nodeOut)
{
    *nodeOut = (ListNode *)g_listNodeHeap.Alloc(12); /* cast kept: the heap hands out untyped memory, like malloc */
}

/* 0x547310 - pushes a node in front of the list's first node. */
void List_PushFront(ListNode **list, ListNode *node)
{
    node->next = *list;
    node->prev = 0;
    if (*list)
        (*list)->prev = node;
    *list = node;
}

/* 0x547345 - the node whose next is 0, or 0 for an empty list. Nothing in the exe calls this. */
ListNode *List_GetLast(ListNode **list)
{
    ListNode *node;

    if (!*list)
        return 0;
    node = *list;
    while (node->next)
        node = node->next;
    return node;
}

/* 0x547378 - unlinks a node; the head advances when the node is the first one. Does not free it. */
void List_Remove(ListNode **list, ListNode *node)
{
    ListNode *n = node;

    if (n == *list)
        *list = n->next;
    else
        n->prev->next = n->next;
    if (n->next)
        n->next->prev = n->prev;
}

/* 0x5473c4 - returns a node to the list-node heap. */
void List_FreeNode(void *node)
{
    g_listNodeHeap.Free(node);
}

/* 0x5473d7 - frees every node of a list (next is read before each free) and empties the head cell. */
void List_Clear(ListNode **list)
{
    ListNode *node = *list;

    while (node) {
        ListNode *cur = node;
        node = node->next;
        List_FreeNode(cur);
    }
    *list = 0;
}

/* 0x547415 - List_Clear, then frees the head cell itself: the counterpart of List_Create. Nothing in the exe calls it. */
void List_Destroy(ListNode **list)
{
    List_Clear(list);
    g_listNodeHeap.Free(list);
}

/* 0x547434 - the node at position index, or 0 past the end. */
ListNode *List_GetAt(ListNode **head, u16 index)
{
    ListNode *node = *head;
    while (node) {
        if (!index)
            return node;
        --index;
        node = node->next;
    }
    return 0;
}

/* ======================================================================================================================
 * 0x547470-0x548423: the level loader, first part.
 */

/* source-only inline (no out-of-line copy): the current screen is one of the two non-level ones (-6, -7). Load_DAVnWAR
 * evaluates it and drops the value (0x548130-0x548166). */
/* BYTES(inline): source-only inline: Load_DAVnWAR evaluates it and drops the value (0x548130-0x548166) */
inline s32 Progress_OnFrontEndScreen()
{
    s8 level = g_pProgress->currentLevel;
    return level == SCENE_DEMO_A || level == SCENE_DEMO_B;
}

/* source-only inline: clears g_gameFlags bits. Inlined with a constant argument, /Od keeps the `~` as code (mov eax,4;
 * not eax at 0x548228), which a plain `g_gameFlags &= ~4` folds. */
/* BYTES(inline): source-only inline: the ~ stays code (mov eax,4; not eax at 0x548228) */
#define SDW_INLINE_FREE_GAME_CLEARFLAGS_U32 1
#include "game_state_inlines.h"
#undef SDW_INLINE_FREE_GAME_CLEARFLAGS_U32

/* 0x547470 - which pass of Load_DAVnWAR takes WAR resource resIndex: 0 world object (mesh), 1 scenaric object,
 * 2 cinematic object, 3 installed by Install_WarResource, 4 none. The type is the table entry's top byte. */
/* BYTES(dead-code): blob is loaded and never used, as in the original */
u8 GetResourceType(int resIndex)
{
    u8 *blob = g_pDav->war.blob;
    u8 result = RESCLASS_IGNORE;
    switch ((g_pDav->war.table[resIndex] >> 24) & 0xff) {
        case WAR_RES_IGNORED_1:
        case WAR_RES_IGNORED_2:
            break;
        case WAR_RES_MESH:
        case WAR_RES_MODEL:
        case WAR_RES_MESH_B:
        case WAR_RES_SKY:
            result = RESCLASS_MESH;
            break;
        case WAR_RES_SCENARIC:
            result = RESCLASS_SCENARIC;
            break;
        case WAR_RES_CINEMATIC:
            result = RESCLASS_CINEMATIC;
            break;
        case WAR_RES_TYPE_8:
        case WAR_RES_TYPE_9:
        case WAR_RES_TYPE_27:
        case WAR_RES_COLL_GRID:
        case WAR_RES_COLL_TRIS:
        case WAR_RES_EXPORTS:
        case WAR_RES_HEADER3:
        case WAR_RES_OBJ_GRID:
        case WAR_RES_PAIRS:
        case WAR_RES_ANIM_NAMES:
            result = RESCLASS_DATA;
            break;
        default:
            if (((g_pDav->war.table[resIndex] >> 24) & 0xff) & WAR_RES_UNCOUNTED)
                result = RESCLASS_DATA;
            else
                Debug_Printf("Error in GetResourceType(): Unknown Object Type %x!\n",
                             (g_pDav->war.table[resIndex] >> 24) & 0xff);
            break;
    }
    return result;
}

/* 0x5475cb - publishes one class-3 WAR resource: the type (top byte) picks the global, the low 24 bits are its offset
 * in the WAR blob. Types 8 and 9 (and those with bit 0x40) have nothing to install. */
/* BYTES(flow): return, not break: with break VC6 folds cases 8 / 9 into the default and drops them from the jump table */
void Install_WarResource(int resIndex)
{
    u32 i;
    u8 *blob = g_pDav->war.blob;
    switch ((g_pDav->war.table[resIndex] >> 24) & 0xff) {
        case WAR_RES_COLL_GRID:
            /* cast kept: a WAR resource is raw bytes at its table offset */
            g_pWarCollMap = (u16 *)(blob + (g_pDav->war.table[resIndex] & 0xffffff));
            break;
        case WAR_RES_COLL_TRIS:
            /* cast kept: a WAR resource is raw bytes at its table offset */
            g_pWarCollTris = (CollTri *)(blob + (g_pDav->war.table[resIndex] & 0xffffff));
            break;
        case WAR_RES_EXPORTS:
            /* cast kept: a WAR resource is raw bytes at its table offset */
            g_warExportTable = (u32 *)(blob + (g_pDav->war.table[resIndex] & 0xffffff));
            g_warExportCount = *g_warExportTable;
            g_warExportTable++;
            /* cast kept: Res_RelocateIdLists takes the relocation base address as an int */
            Res_RelocateIdLists(g_warExportTable, g_warExportCount, (int)blob);
            break;
        case WAR_RES_PAIRS:
            /* cast kept: a WAR resource is raw bytes at its table offset */
            g_warRelocTable = (u32 *)(blob + (g_pDav->war.table[resIndex] & 0xffffff));
            g_warRelocCount = *g_warRelocTable;
            g_warRelocTable++;
            for (i = 0; i < g_warRelocCount; i++)
                /* cast kept: relocation: a blob offset becomes an address */
                g_warRelocTable[i * 2 + 1] = g_warRelocTable[i * 2 + 1] + (u32)blob;
            break;
        case WAR_RES_ANIM_NAMES:
            g_animNameTable = blob + (g_pDav->war.table[resIndex] & 0xffffff);
            break;
        case WAR_RES_HEADER3:
            /* cast kept: the 12-byte 0x83 header is copied as one block over g_texScrollListIds and the words after it */
            *(WarLevelHeader *)g_texScrollListIds =
                *(WarLevelHeader *)(blob + (g_pDav->war.table[resIndex] & 0xffffff));
            break;
        case WAR_RES_OBJ_GRID:
            /* cast kept: a WAR resource is raw bytes at its table offset */
            g_pWarObjGrid = (u16 *)(blob + (g_pDav->war.table[resIndex] & 0xffffff));
            break;
        case WAR_RES_TYPE_8:
        case WAR_RES_TYPE_9:
            return; /* `return`, not `break`: with break VC6 folds these cases into the default and drops them from the table */
    }
}

/* 0x547855 - loads a level: levelPath is the path without extension; .DAV (textures, id lists), .SND, .MLT (strings)
 * and .WAR (everything else), then builds the level's objects and brings up every level subsystem. 1 on success. */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py) */
/* BYTES(dead-code): dummy2 / unused1 are zeroed and never read: the original zeroes these two slots */
u8 Load_DAVnWAR(const char *levelPath, Dav *dav)
{
    /* names chosen for their stack slots (file comment); the four extensions are copied from .data (0x57c40c..) */
    char davStr[5] = ".DAV";
    char extWar[5] = ".WAR";
    char szMlt[5] = ".MLT";
    char szSnd[5] = ".SND";
    u16 dummy2 = 0;
    u16 unused1 = 0;
    s32 texPages;
    char base[128];
    u32 rgb;
    char fullPath[128];
    u16 i;
    u16 j;
    u16 w;
    u16 iCine;
    u16 scnI;

    strcpy(base, levelPath);
    g_pDav = dav;
    g_cinePlayer.Reset();
    Dialogue_Reset();
    Rand_Reset();
    strcpy(fullPath, base);
    Debug_Printf("{Loading DAV...\n");
    strcat(fullPath, davStr);
    g_pPolyBin = new PolyBatcher(g_pD3DAppMain, g_maxImmediateTriangles, fullPath, &texPages);
    if (texPages == 0)
        goto error;
    if (Load_DAV(fullPath, g_pDav) < 0) {
        Debug_Printf("Load_DAVWAR():  File %s not found\n", fullPath);
        goto error;
    }
    g_gameFlags |= GF_LEVEL_LOADED_B;
    if (Font_LoadFromRes(FONT_DEBUG, DAV_IDI_IGLFONTE, FONT_KIND_DEBUG) < 0)
        Debug_Printf("Load_DAVnWAR(): Debug Fonte Loading\n");
    if (Font_LoadFromRes(FONT_GAME, DAV_IDI_IGLTYPO_, FONT_KIND_GAME) < 0)
        Debug_Printf("Load_DAVnWAR(): Game Fonte Loading\n");
    Font_CloneResized(FONT_GAME, FONT_GAME_SMALL, 10, 12);
    Debug_Printf("{Font loaded\n");
    Text_SetFont(FONT_DEBUG);

    Debug_Printf("{Loading SND...\n");
    strcpy(fullPath, base);
    strcat(fullPath, szSnd);
    if (Load_SND(fullPath) < 0)
        Debug_Printf("Load_DAVWAR(): File %s not found\n", fullPath);

    Debug_Printf("{Loading MLT...\n");
    strcpy(fullPath, base);
    strcat(fullPath, szMlt);
    if (Load_MLT(fullPath, &g_pDav->strings) < 0) {
        Debug_Printf("Load_DAVWAR(): File %s not found\n", fullPath);
        g_pDav->strings.listCount = 0;
        g_pDav->strings.lists = 0;
    }

    Debug_Printf("{Loading WAR...\n");
    strcpy(fullPath, base);
    strcat(fullPath, extWar);
    if (Load_WAR(fullPath, &g_pDav->war) < 0) {
        Debug_Printf("Load_DAVWAR():  File %s not found\n", fullPath);
        goto error;
    }
    Load_WarMeshes(fullPath, &g_pDav->war);
    g_gameFlags |= GF_LEVEL_LOADED_A;

    Debug_Printf("{Allocating Objects...\n");
    Time_Init();
    Scenaric_RegisterAllClasses();
    g_worldObjCount = 0;
    g_scnObjectCount = 0;
    g_cineObjectCount = 0;
    g_worldObjs = 0;
    g_scnActive = 0;
    g_scnObjects = 0;
    g_cineObjects = 0;
    i = 0;
    while (i < g_pDav->war.header->resourceCount) {
        switch (GetResourceType(i)) {
            case RESCLASS_MESH:
                g_worldObjCount++;
                break;
            case RESCLASS_SCENARIC:
                g_scnObjectCount++;
                break;
            case RESCLASS_CINEMATIC:
                g_cineObjectCount++;
                break;
        }
        i++;
    }
    if (g_worldObjCount != 0)
        g_worldObjs = (WorldObj **)malloc(g_worldObjCount << 2); /* cast kept: malloc returns untyped memory */
    i = 0;
    while (i < g_pDav->war.header->resourceCount) {
        switch (GetResourceType(i)) {
            case RESCLASS_DATA:
                Install_WarResource(i);
                break;
        }
        i++;
    }
    g_scnActiveCapacity = g_scnObjectCount + 10;
    Scenaric_InitLevelState();
    g_scnActive = (ScnObject **)malloc(g_scnObjectCount * 4 + 0x28); /* cast kept: malloc returns untyped memory */
    if (g_scnObjectCount != 0)
        g_scnObjects = (ScnObject **)malloc(g_scnObjectCount << 2); /* cast kept: malloc returns untyped memory */
    if (g_cineObjectCount != 0)
        g_cineObjects = (ScnObject **)malloc(g_cineObjectCount << 2); /* cast kept: malloc returns untyped memory */
    memset(g_scnActive, 0, g_scnObjectCount * 4 + 0x28);
    memset(g_scnObjects, 0, g_scnObjectCount << 2);
    w = 0;
    scnI = 0;
    iCine = 0;
    i = 0;
    while (i < g_pDav->war.header->resourceCount) {
        switch (GetResourceType(i)) {
            case RESCLASS_MESH:
                /* cast kept: WorldObj_CreateFromResource (instance.cpp) returns void * */
                g_worldObjs[w] = (WorldObj *)WorldObj_CreateFromResource(i);
                w++;
                break;
            case RESCLASS_SCENARIC:
                g_scnObjects[scnI] = Install_ScenaricResource(i);
                scnI++;
                break;
            case RESCLASS_CINEMATIC:
                g_cineObjects[iCine] = Install_CinematicResource(i);
                iCine++;
                break;
        }
        i++;
    }
    Debug_Printf("{Initialising links...\n");
    AttachLink_InitPool();
    Debug_Printf("{Initialising interface...\n");
    Interface_Init();
    Debug_Printf("{Initialising inventory...\n");
    Inventory_Init();
    Debug_Printf("{Initialising billboard sfx...\n");
    Sfx_InitSpriteSheets();
    Debug_Printf("{Initialising Collisions...\n");
    Collide_InitLevel();
    Debug_Printf("{Initialising Shadows...\n");
    Shadow_LoadLevel();
    Debug_Printf("{Initialising Camera...\n");
    Camera_InitSettings();
    Debug_Printf("{Initialising Clusters...\n");
    ObjGrid_Init(g_pWarObjGrid);
    TexScroll_Init();
    Transition_Init();
    Weather_LevelInit();
    if (g_weatherType == WEATHER_RAIN)
        Weather_InitRain(&g_camPos);
    else if (g_weatherType == WEATHER_SNOW)
        Weather_InitSnow(&g_camPos);
    rgb = ((g_pDav->war.header->clearR >> 1) << 16) + ((g_pDav->war.header->clearG >> 1) << 8) +
          (g_pDav->war.header->clearB >> 1);
    g_pPolyBin->SetClearColor(rgb);
    g_pViewFrustum->SetFogColor(rgb);
    g_scnActiveBaseCount = g_scnObjectCount;
    g_scnActiveHigh = 0;
    for (j = 0; j < g_scnObjectCount; j++) {
        if (g_scnObjects[j] != 0)
            g_scnObjects[j]->AddToWorld(0);
    }
    for (j = 0; j < g_scnObjectCount; j++) {
        if (g_scnObjects[j] != 0)
            g_scnObjects[j]->PostLoadInit();
    }
    Debug_Printf("{Initialising Menus...\n");
    Menu_Init(&g_pauseMenu);
    Debug_Printf("{Initialising Map...\n");
    Map_Init();
    Menus_LoadLevelUi();
    g_pStreamPlayer = new StreamPlayer();
    g_pStreamPlayer->Load_MusicVoiceBank();
    g_pStreamPlayer->LoadLevelMusic();
    Input_Init();
    MCard_Init();
    g_pTimer->Start();
    Text_ResetWindow();
    Progress_OnFrontEndScreen();
    return 1;

error:
    Debug_Printf("LoadDAVnWAR(): Error\n");
    return 0;
}

/* 0x54818f - frees what Load_DAVnWAR built, in roughly the reverse order. The PolyBatcher and the StreamPlayer are
 * deleted without clearing their pointers, and g_pStreamPlayer is used without a NULL test. */
/* BYTES(flow): if (0) ; emits nothing but advances /Od's round-robin register choice (edx at 0x5481e6); probably a compiled-out debug check */
void Load_FreeLevel()
{
    if (g_cinePlayer.IsActive())
        g_cinePlayer.Stop();
    Prompt_End();
    Input_Unacquire();
    ObjGrid_Free();
    Camera_FreeLevel();
    Shadow_FreeLevel();
    Collide_ShutdownLevel();
    TexScroll_FreeAll();
    Menu_Init(&g_pauseMenu);
    /* Device, not a claim about the source text: an `if` with a constant condition and an empty body emits nothing
     * but advances VC6 /Od's round-robin register choice by one, which the original shows from here on (edx at
     * 0x5481e6 rather than ecx). Most likely a compiled-out debug check. */
    if (0)
        ;
    if (g_pDav != 0)
        Load_FreeMLT(&g_pDav->strings);
    if (g_gameFlags & GF_LEVEL_LOADED_A) {
        Load_FreeWarMeshes(&g_pDav->war);
        Load_FreeWAR(&g_pDav->war);
        Game_ClearFlags(GF_LEVEL_LOADED_A);
    }
    if (g_gameFlags & GF_LEVEL_LOADED_B) {
        Dav_Free(g_pDav);
        delete g_pPolyBin;
        Game_ClearFlags(GF_LEVEL_LOADED_B);
    }
    Sound_ShutdownChannels();
    if (g_worldObjs != 0) {
        free(g_worldObjs);
        g_worldObjs = 0;
    }
    if (g_scnActive != 0) {
        free(g_scnActive);
        g_scnActive = 0;
    }
    if (g_scnObjects != 0) {
        free(g_scnObjects);
        g_scnObjects = 0;
    }
    if (g_cineObjects != 0) {
        free(g_cineObjects);
        g_cineObjects = 0;
    }
    Text_Disable();
    g_pStreamPlayer->Halt();
    delete g_pStreamPlayer;
}

/* 0x548366 - no callers. Meant to hand out the DAV bitmap-record table, but it assigns to its own by-value argument. */
void Dav_GetBitmapTable_Dead(DavBitmapRec *out)
{
    if (out != 0)
        out = g_pDav->header->dir->bitmaps;
}

/* 0x548381 - the DAV id list resId, or NULL (count 0) when it is missing or an entry is out of range. The range check
 * reads the list's FIRST entry on every pass (list[0], never list[i]), so only that entry is really validated. */
u32 *Res_GetValidatedIdList(u16 resId, u16 *outCount)
{
    u16 i;
    u32 *list;
    DavDirectory *davDir = g_pDav->header->dir;
    s32 valid = 1;
    list = IdList_FindWithCount(resId, outCount);
    if (list != 0) {
        for (i = 0; i < *outCount; i++) {
            /* cast kept: an id-list entry is a u32 holding a pointer into the DAV index table */
            if (*(u16 *)*list >= davDir->bitmapCount || (u32)((u16 *)*list - davDir->indices) >= davDir->indexCount)
                valid = 0;
        }
    }
    if (valid)
        return list;
    *outCount = 0;
    return 0;
}
