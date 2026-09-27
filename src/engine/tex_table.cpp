/*
 * T263 - guessed original name: TexTable.cpp. SheepD3D.exe .text 0x52a810-0x52a851, .bss 0x6d7070-0x6d8078.
 * The level's mesh cache keyed by model resource: g_texCount, g_texObjects[512], g_texKeys[512] (filled by elastic and
 * the WAR mesh loader, freed by Load_FreeWarMeshes), their definitions, and the lookup Texture_FindByResource 0x52a810.
 */
#include "sdw_classes.h"

/* ---- .bss (0x6d7070-0x6d8078). Uninitialised: VC6 orders them by a hash of their names, which here is already the
 * address order (count, objects, keys). ---- */
u16 g_texCount;          /* 0x6d7070 */
Mesh *g_texObjects[512]; /* 0x6d7078 */
u32 g_texKeys[512];      /* 0x6d7878 */

/* 0x52a810 - the cached mesh built for a model resource, or 0. */
void *Texture_FindByResource(Model *model)
{
    u32 i = 0;
    do {
        /* cast kept: elastic.cpp and load_warmeshes.cpp store the keys as u32 addresses */
        if (g_texKeys[i] == (u32)model)
            return g_texObjects[i];
        i++;
    } while (i < g_texCount);
    return 0;
}
