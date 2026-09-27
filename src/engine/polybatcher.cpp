/*
 * Object T014 (data/tu_map.json), guessed original file PolyBatcher.cpp.
 *   .text 0x416640-0x418a9f, then the COMDAT ??_GPolyBatcher 0x418aa0-0x418ace
 *   .rdata 0x5743a0-0x5743a8 (??_7PolyBatcher + pad)   .data 0x579440-0x579448 ('VDX7')   .bss 0x5cc648-0x5cc6c8
 * The engine header's inline helpers this file also uses (Render_DrawPrimitive 0x415c00, Render_SetTexture 0x415c40,
 * the empty constructor 0x415c70 and PolyBatcher::SubmitPoly 0x415c80) are COMDATs the original linker took from the
 * Mesh object T013, so they are written there (src/engine/mesh.cpp) and only declared here. This file defines
 * g_vdx7Magic (.data) and g_davTexturePath (.bss).
 */
/* BYTES: cast, inline, slot-name, view. */
/* BYTES(inline, inferred): TlVertexFlat() {} / TlVertexTex() {}: empty user-declared constructor: new T[n] runs the original's empty construction loop */
/* BYTES(slot-name): PolyBatcher::PolyBatcher (no texture file): idx is named for its stack slot: the new[] count temporary goes after it */
/* BYTES(view): Cpp_NoopCtor: an empty constructor's out-of-line copy, written as a method returning this so it has the table's name */
/* match-addr: ??0PolyBatcher@@QAE@PAVD3DApp@@II@Z=0x416640 ??0BsStream@@QAE@PBD@Z=0x41b1a0 ??0Texture@@QAE@PBDPAVD3DApp@@IIIPAH@Z=0x40a8b9 abs=0x566d35 qsort=0x566944 */
/*
 * The polygon batcher, SheepD3D.exe 0x415c00-0x418acf: the D3D draw helpers the engine header defines inline
 * (Render_DrawPrimitive, Render_SetTexture; Render_SetStateFlags 0x4155f0 and Render_ClearStateFlags 0x4159b0 are the
 * same kind), the empty constructor 0x415c70, PolyBatcher::SubmitPoly (all in T013, see above), and then this file: the
 * two constructors, the destructor, the state-flag accessors, the VDX7 texture-page loader, lost-surface recovery, the
 * frame begin / end / present and the end-of-frame flush with its sort comparator.
 *
 * The batcher (g_pPolyBin 0x6d6ec0) is where every RenderPoly ends up. Type 1 (untextured) triangles are appended
 * to one batch; a texture page below immediateTexCount has its own batch; everything else (blend types 2 / 3 and the
 * textures after the immediate prefix) is copied into the sorted pool and drawn back to front by Flush. A batch that
 * fills (batchCapacity triangles) is drawn at once.
 *
 * RenderPoly has a virtual destructor (slot 0 of 0x5743f8, through which ~PolyBatcher deletes the pool; it is in
 * data/vtable_slots.csv), and data/structs has the BsStream struct. Because RenderPoly is polymorphic, VC6 also emits
 * RenderPoly's vector deleting destructor and the vector ctor / dtor iterators here (NOADDR: nothing in this file
 * refers to them); the first matches 0x415540 byte for byte when placed there, so that COMDAT most likely came from
 * this file. Render_RestoreLostSurfaces 0x41785d reads the device through an inline accessor that returns a value (see
 * there).
 *
 * Shapes that are representations, not the original's text:
 *  - Render_SetStateFlags / _ClearStateFlags / Render_DrawPrimitive / Render_SetTexture are inline functions of the
 *    engine header; which call sites VC6 expanded and which it called is not reproduced by writing them all inline
 *    (as in src/engine/weather.cpp), so each site says which one it uses: the *Inline twins (and BindTexture) are
 *    expanded, the Render_* names are calls. The twins have no bodies of their own in the exe, so their names are not
 *    recovered.
 *  - Cpp_NoopCtor 0x415c70 is an empty constructor's out-of-line copy (its only caller is the static initialiser
 *    0x40f8e8, which passes it to the vector-constructor iterator for 0x18-byte elements); it is written as a method
 *    returning `this`, under the table's name.
 *  - The first constructor 0x416640 has no caller; the tables cannot tell the two constructors apart, so both are
 *    pinned by their decorated names above, as are the second BsStream / Texture constructors this file calls and the
 *    CRT's abs / qsort (no table rows).
 */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "../sdk/d3d7.h"
#include "../sdk/ddraw.h"
#include "../sdk/win32.h"

class Mat44;
#include "../sdk/crt.h"

/* The batcher's vertex formats: FVF 0xc4 (XYZRHW | DIFFUSE | SPECULAR) and 0x1c4 (the same plus one UV pair). Their
 * empty constructors are what make each `new` below run an (empty) element loop. */
struct TlVertexFlat {
    float x, y, z, rhw;
    u32 diffuse, specular;
    TlVertexFlat() {}
};
struct TlVertexTex {
    float x, y, z, rhw;
    u32 diffuse, specular;
    float u, v;
    TlVertexTex() {}
};

#define SDW_MEMBERS_D3DApp                                                                                        \
    void Render_SetStateFlags(u32 flags);                                 /* 0x4155f0, compiled in emitter.cpp */ \
    void Render_ClearStateFlags(u32 flags);                               /* 0x4159b0, compiled in emitter.cpp */ \
    void Render_DrawPrimitive(u32 type, u32 fvf, void *verts, u32 count); /* 0x415c00 */                          \
    void Render_SetTexture(Texture *tex, u32 stage);                      /* 0x415c40 */                          \
    void SetStateFlagsInline(u32 flags);                                  /* inline twins, defined below */       \
    void ClearStateFlagsInline(u32 flags);                                                                        \
    void DrawPrimitiveInline(u32 type, u32 fvf, void *verts, u32 count);                                          \
    void BindTexture(Texture *tex, s32 stage);                                                                    \
    void ClearTarget(u32 color)                                                                                   \
    {                                                                                                             \
        pD3DDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, color, 1.0f, 0);                              \
    }                                                                                                             \
    void BeginScene()                                                                                             \
    {                                                                                                             \
        pD3DDevice->BeginScene();                                                                                 \
    }                                                                                                             \
    void EndScene()                                                                                               \
    {                                                                                                             \
        pD3DDevice->EndScene();                                                                                   \
    }                                                                                                             \
    IDirectDraw7 *GetDirectDraw()                                                                                 \
    {                                                                                                             \
        return pDD;                                                                                               \
    } /* inline accessor, see Render_RestoreLostSurfaces */
#define SDW_MEMBERS_PolyBatcher                                                                    \
    PolyBatcher(D3DApp *app, u32 capacity, u32 pageCount);                          /* 0x416640 */ \
    PolyBatcher(D3DApp *app, u32 capacity, const char *davPath, s32 *outPageCount); /* 0x416970 */ \
    void SubmitPoly(RenderPoly *poly);                                              /* 0x415c80 */
#define SDW_MEMBERS_RenderPoly RenderPoly();                                        /* 0x41aad0 RenderPoly_Ctor */
#define SDW_MEMBERS_BsStream BsStream(const char *path);                            /* 0x41b1a0 BsStream_Open */
#define SDW_MEMBERS_Texture                                                                                    \
    Texture(D3DApp *app, u32 width, u32 height, u32 format, s32 *result);                       /* 0x40a140 */ \
    Texture(const char *fileName, D3DApp *app, u32 width, u32 height, u32 format, s32 *result); /* 0x40a8b9 */ \
    void Surface_LockReadWrite(DDSURFACEDESC2 *desc);                                           /* 0x40ae74 */
#include "sdw_classes.h"

#include "draw2d.h"
char g_davTexturePath[0x80]; /* 0x5cc648  the .dav path of the loaded texture pages (size inferred: the gap
                                           up to the next object's .bss) */
char g_vdx7Magic[] = "VDX7"; /* 0x579440  the texture-page file signature */

int PolyBatcher_CompareSortZ(RenderPoly **a, RenderPoly **b);

/* ---- the inline twins: the engine header's render-device helpers as the expanded sites show them ---- */

/* BYTES(inline): source-only inline twin (twin of Render_DrawPrimitive 0x415c00): used where the original expands the helper; the Render_* name stays a call */
#define SDW_INLINE_D3DAPP_DRAWPRIMITIVEINLINE_U32_U32_VOID_U32 1
#include "../app/d3dapp_inlines.h"
#undef SDW_INLINE_D3DAPP_DRAWPRIMITIVEINLINE_U32_U32_VOID_U32
/* a signed stage: the constant 0 gets a stack temp at every expansion */
/* BYTES(cast): source-only inline twin of Render_SetTexture; the stage is signed so the constant 0 gets a stack temp at every expansion */
#define SDW_INLINE_D3DAPP_BINDTEXTURE_TEXTURE_S32 1
#include "../app/d3dapp_inlines.h"
#undef SDW_INLINE_D3DAPP_BINDTEXTURE_TEXTURE_S32
/* BYTES(inline): source-only inline twin (twin of Render_SetStateFlags 0x4155f0): used where the original expands the helper; the Render_* name stays a call */
#define SDW_INLINE_D3DAPP_SETSTATEFLAGSINLINE_U32 1
#include "../app/d3dapp_inlines.h"
#undef SDW_INLINE_D3DAPP_SETSTATEFLAGSINLINE_U32
/* BYTES(inline): source-only inline twin (twin of Render_ClearStateFlags 0x4159b0): used where the original expands the helper; the Render_* name stays a call */
#define SDW_INLINE_D3DAPP_CLEARSTATEFLAGSINLINE_U32 1
#include "../app/d3dapp_inlines.h"
#undef SDW_INLINE_D3DAPP_CLEARSTATEFLAGSINLINE_U32
/* 0x416640 - the constructor without a texture file: pageCount pages, each an immediate page with its own batch
 * (0xc000 state flags). No caller; the loading constructor below is the one Load_DAVnWAR uses. */
PolyBatcher::PolyBatcher(D3DApp *app, u32 capacity, u32 pageCount)
{
    u32 idx; /* named for its stack slot: the new[] count temporary goes after it */

    renderer = app;
    batchCapacity = capacity;
    defaultStateFlags = RSF_DITHER | RSF_COLORVERTEX;
    clearColor = 0;
    lineBatchVerts = new TlVertexFlat[4000];
    lineBatchCount = 0;
    lineStateFlags = RSF_ANTIALIAS | RSF_DITHER | RSF_COLORVERTEX;
    flatBatchVerts = new TlVertexFlat[batchCapacity * 3];
    flatBatchCount = 0;
    stateFlags = RSF_DITHER | RSF_COLORVERTEX;
    texturePageCount = pageCount;
    textures = (Texture **)malloc(texturePageCount * 4); /* cast kept: malloc returns untyped memory */
    texStateFlags = new u32[texturePageCount];
    for (idx = 0; idx < texturePageCount; idx++)
        texStateFlags[idx] = RSF_TEXTURED | RSF_FILTER_LINEAR;
    texBatchVerts = (void **)malloc(texturePageCount * 4); /* cast kept: malloc returns untyped memory */
    for (idx = 0; idx < texturePageCount; idx++)
        texBatchVerts[idx] = new TlVertexTex[batchCapacity * 3];
    texBatchCounts = new u32[texturePageCount];
    memset(texBatchCounts, 0, texturePageCount * 4);
    textureDirty = 1;
    sortedPolys = new RenderPoly[6000];
    sortedList = (RenderPoly **)malloc(6000 * 4); /* cast kept: malloc returns untyped memory */
    computeSortZ = 1;
    sortedCount = 0;
    blendStateFlags = RSF_BLEND_ALPHA;
    addStateFlags = RSF_BLEND_ADD;
}

/* 0x416970 - the constructor: the line and untextured batches, then the texture pages from the VDX7 file (its path
 * kept in g_davTexturePath for Render_RestoreLostSurfaces), the sorted pool of 6000 RenderPolys and its pointer list.
 * *outPageCount receives LoadTexturePages' page count (0 on failure). */
PolyBatcher::PolyBatcher(D3DApp *app, u32 capacity, const char *davPath, s32 *outPageCount)
{
    renderer = app;
    batchCapacity = capacity;
    defaultStateFlags = RSF_DITHER | RSF_COLORVERTEX;
    clearColor = 0;
    lineBatchVerts = new TlVertexFlat[4000];
    lineBatchCount = 0;
    lineStateFlags = RSF_ANTIALIAS | RSF_DITHER | RSF_COLORVERTEX;
    flatBatchVerts = new TlVertexFlat[batchCapacity * 3];
    flatBatchCount = 0;
    stateFlags = RSF_DITHER | RSF_COLORVERTEX;
    strcpy(g_davTexturePath, davPath);
    *outPageCount = LoadTexturePages(davPath);
    textureDirty = 1;
    sortedPolys = new RenderPoly[6000];
    sortedList = (RenderPoly **)malloc(6000 * 4); /* cast kept: malloc returns untyped memory */
    computeSortZ = 1;
    sortedCount = 0;
    blendStateFlags = RSF_BLEND_ALPHA;
    addStateFlags = RSF_BLEND_ADD;
}

/* 0x416b72 - frees the batches, the texture pages and the sorted pool. */
PolyBatcher::~PolyBatcher()
{
    u32 i;

    if (lineBatchVerts)
        delete lineBatchVerts;
    for (i = 0; i < texturePageCount; i++) {
        if (textures[i])
            delete textures[i];
    }
    free(textures);
    delete texStateFlags;
    if (flatBatchVerts)
        delete flatBatchVerts;
    for (i = 0; i < immediateTexCount; i++) {
        if (texBatchVerts[i])
            delete texBatchVerts[i];
    }
    free(texBatchVerts);
    delete texBatchCounts;
    if (sortedPolys)
        delete[] sortedPolys;
    if (sortedList)
        free(sortedList);
}

/* 0x416d04 - the state-flag word of a RenderPoly type: 0 lines, 1 untextured, 2 alpha blend, 3 additive, >= 4 its
 * texture page's. */
u32 PolyBatcher::GetTypeStateFlags(s32 polyType)
{
    u32 page;

    switch (polyType) {
        case RPOLY_LINE:
            return lineStateFlags;
        case RPOLY_OPAQUE:
            return stateFlags;
        case RPOLY_BLEND:
            return blendStateFlags;
        case RPOLY_ADD:
            return addStateFlags;
        default:
            page = polyType - RPOLY_TEXTURED_BASE & ~RPOLY_F_8000;
            return texStateFlags[page];
    }
}

/* 0x416d73 - sets the state-flag word of a RenderPoly type (the mapping of 0x416d04). */
void PolyBatcher::SetTypeStateFlags(s32 polyType, u32 flags)
{
    u32 page;

    switch (polyType) {
        case RPOLY_LINE:
            lineStateFlags = flags;
            break;
        case RPOLY_OPAQUE:
            stateFlags = flags;
            break;
        case RPOLY_BLEND:
            blendStateFlags = flags;
            break;
        case RPOLY_ADD:
            addStateFlags = flags;
            break;
        default:
            page = polyType - RPOLY_TEXTURED_BASE & ~RPOLY_F_8000;
            texStateFlags[page] = flags;
    }
}

/* 0x416df2 - clears bits in the state-flag word of a RenderPoly type. */
void PolyBatcher::ClearTypeStateFlags(s32 polyType, u32 flags)
{
    u32 page;

    switch (polyType) {
        case RPOLY_LINE:
            lineStateFlags &= ~flags;
            break;
        case RPOLY_OPAQUE:
            stateFlags &= ~flags;
            break;
        case RPOLY_BLEND:
            blendStateFlags &= ~flags;
            break;
        case RPOLY_ADD:
            addStateFlags &= ~flags;
            break;
        default:
            page = polyType - RPOLY_TEXTURED_BASE & ~RPOLY_F_8000;
            texStateFlags[page] &= ~flags;
    }
}

/* 0x416ea8 */
u32 PolyBatcher::GetDefaultStateFlags()
{
    return defaultStateFlags;
}

/* 0x416eb9 */
void PolyBatcher::SetDefaultStateFlags(u32 flags)
{
    defaultStateFlags = flags;
}

/* 0x416ecf */
void PolyBatcher::ClearDefaultStateFlags(u32 flags)
{
    defaultStateFlags &= ~flags;
}

/* 0x416eef */
u32 PolyBatcher::GetClearColor()
{
    return clearColor;
}

/* 0x416f00 */
void PolyBatcher::SetClearColor(u32 argb)
{
    clearColor = argb;
}

/* 0x416f16 */
void PolyBatcher::SetComputeSortZ(u8 enable)
{
    computeSortZ = enable;
}

/* 0x416f2c - replaces texture page `index` with a texture read from a bitmap file; 4 when the index is out of range,
 * else the constructor's result (on failure the new page is deleted but its pointer is left in the table). */
s32 PolyBatcher::CreateTexture(const char *fileName, u32 index, u32 width, u32 height, u32 format)
{
    s32 result;

    if (index >= texturePageCount) {
        result = TEXRES_OUT_OF_MEMORY;
    } else {
        textures[index] = new Texture(fileName, renderer, width, height, format, &result);
        if (result)
            delete textures[index];
    }
    return result;
}

/* 0x416fe2 - reads the VDX7 texture-page file: the immediate pages first (format bits 0x1c == 4), each with its own
 * batch buffer, then the sorted pages (0x08 alpha blend, 0x10 additive), then four blank 4x4 pages. Returns the
 * number of pages read, 0 when the file is missing, not VDX7, or a texture cannot be created. */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py); each of the three page loops has its own set */
s32 PolyBatcher::LoadTexturePages(const char *path)
{
    /* local names chosen for their stack slots (tools/vc6_locals.py); the three page loops each have their own */
    s32 total;
    u32 iPage;
    u32 j;
    char sig[4];
    bool immediate;
    s32 k;

    total = 0;
    iPage = 0;
    BsStream stream(path);
    if (stream.ok) {
        sig[0] = stream.ReadU8(1);
        sig[1] = stream.ReadU8(1);
        sig[2] = stream.ReadU8(1);
        sig[3] = stream.ReadU8(1);
        if (strncmp(sig, g_vdx7Magic, 4) == 0) {
            stream.ReadU32(1);
            stream.ReadU32(1);
            stream.ReadU32(1);
            stream.ReadU32(1);
            stream.Seek(stream.ReadU32(1));
            stream.ReadU16(1);
            stream.ReadU16(1);
            texturePageCount = stream.ReadU16(1);
            textures = (Texture **)malloc((texturePageCount + 4) * 4); /* cast kept: malloc returns untyped memory */
            texStateFlags = new u32[texturePageCount + 4];
            stream.ReadU32(1);
            stream.ReadU32(1);
            stream.Seek(stream.ReadU32(1));
            iPage = 0;
            /* the immediate pages (format bits 0x1c == 4) come first */
            do {
                u32 numTexels;
                u32 n;
                DDSURFACEDESC2 *surfDesc;
                u16 *pix;
                u32 wid;
                u32 format;
                u32 h;
                s32 err;

                total++;
                wid = abs(stream.ReadU16(1) - stream.ReadU16(1));
                h = abs(stream.ReadU16(1) - stream.ReadU16(1));
                format = stream.ReadU16(1);
                immediate = (format & TEXFMT_KIND_MASK) == TEXFMT_KIND_IMMEDIATE;
                if (immediate == 1) {
                    textures[iPage] = new Texture(renderer, wid, h, format & TEXFMT_PIXEL_MASK, &err);
                    if (err)
                        return 0;
                    surfDesc = new DDSURFACEDESC2;
                    textures[iPage]->Surface_LockReadWrite(surfDesc);
                    /* cast kept: a locked surface is raw memory; these pages hold 16-bit texels */
                    pix = (u16 *)surfDesc->lpSurface;
                    numTexels = wid * h;
                    for (n = 0; n < numTexels; n++) {
                        *pix = stream.ReadU16(1);
                        pix++;
                    }
                    textures[iPage]->Surface_Unlock();
                    delete surfDesc;
                    texStateFlags[iPage] = RSF_DITHER | RSF_TEXTURED | RSF_FILTER_LINEAR;
                    iPage++;
                }
            } while (iPage < texturePageCount && immediate == 1);
            immediateTexCount = iPage;
            texBatchCounts = new u32[immediateTexCount];
            memset(texBatchCounts, 0, immediateTexCount * 4);
            texBatchVerts = (void **)malloc(immediateTexCount * 4); /* cast kept: malloc returns untyped memory */
            for (j = 0; j < immediateTexCount; j++)
                texBatchVerts[j] = new TlVertexTex[batchCapacity * 3];
            /* back to the first sorted page's header (five u16s) */
            stream.Skip(-10);
            do {
                u32 numTexels;
                u32 n;
                DDSURFACEDESC2 *surfDesc;
                u16 *pix;
                u32 wid;
                u32 format;
                u32 h;
                s32 err;

                total++;
                wid = abs(stream.ReadU16(1) - stream.ReadU16(1));
                h = abs(stream.ReadU16(1) - stream.ReadU16(1));
                format = stream.ReadU16(1);
                textures[iPage] = new Texture(renderer, wid, h, format & TEXFMT_PIXEL_MASK, &err);
                if (err)
                    return 0;
                surfDesc = new DDSURFACEDESC2;
                textures[iPage]->Surface_LockReadWrite(surfDesc);
                /* cast kept: a locked surface is raw memory; these pages hold 16-bit texels */
                pix = (u16 *)surfDesc->lpSurface;
                numTexels = wid * h;
                for (n = 0; n < numTexels; n++) {
                    *pix = stream.ReadU16(1);
                    pix++;
                }
                textures[iPage]->Surface_Unlock();
                delete surfDesc;
                switch (format & TEXFMT_KIND_MASK) {
                    case TEXFMT_KIND_BLEND:
                        texStateFlags[iPage] = RSF_BLEND_ALPHA | RSF_TEXTURED | RSF_FILTER_LINEAR;
                        break;
                    case TEXFMT_KIND_ADD:
                        texStateFlags[iPage] = RSF_BLEND_ADD | RSF_TEXTURED | RSF_FILTER_LINEAR;
                        break;
                    default:
                        total = 0;
                }
                iPage++;
            } while (iPage < texturePageCount);
            /* four blank 4x4 pages after the file's */
            for (k = 0; k < 4; k++) {
                s32 xx;
                s32 y;
                DDSURFACEDESC2 *ddsd;
                u8 *pixels;
                s32 err;

                total++;
                textures[iPage] = new Texture(renderer, 4, 4, TEXFMT_ARGB4444, &err);
                if (err)
                    return 0;
                ddsd = new DDSURFACEDESC2;
                textures[iPage]->Surface_LockReadWrite(ddsd);
                /* cast kept: a locked surface is raw memory, addressed here by byte with lPitch */
                pixels = (u8 *)ddsd->lpSurface;
                for (y = 0; y < 4; y++)
                    for (xx = 0; xx < 4; xx++)
                        /* cast kept: the row address is a number (base + y * pitch), read as 16-bit texels */
                        ((u16 *)(y * ddsd->lPitch + (u32)pixels))[xx] = 0;
                textures[iPage]->Surface_Unlock();
                delete ddsd;
                texStateFlags[iPage] = RSF_BLEND_ALPHA | RSF_TEXTURED | RSF_FILTER_LINEAR;
                iPage++;
                texturePageCount++;
            }
        }
    }
    return total;
}

/* 0x41785d - after DDERR_SURFACELOST: restores every DirectDraw surface, frees the texture pages and their batches
 * and reloads them from g_davTexturePath; E_FAIL when the page count changed (or the restore failed).
 *
 * The first statement goes through D3DApp::GetDirectDraw, an inline accessor (the DirectX 7 SDK framework's
 * CD3DFramework7 has the same one-line `return m_pDD;`; the name is that header's, the method has no body of its own in
 * the exe). An inline that RETURNS a value is what the original's odd prologue shows (0x417866-0x417881): VC6 /Od
 * copies the inline's uninitialised return variable ([ebp-0x24]) into a return temporary allocated after `this`
 * ([ebp-0x2c]), then stores pDD there and makes the virtual call through that temporary. A plain free inline
 * `return g_pD3DAppMain->pDD;` matches the same way. Earlier attempts (an inline parameter or local, ~30 shapes) put the
 * pointer before `this`. The local names were chosen for their stack slots (hr -4, oldCount -8, n -0xc). */
/* BYTES(inline): the value-returning inline GetDirectDraw reproduces the return temporary after 'this' ([ebp-0x2c]) of the original prologue (0x417866-0x417881) */
/* BYTES(slot-name): names chosen for their stack slots: hr -4, oldCount -8, n -0xc */
long PolyBatcher::Render_RestoreLostSurfaces()
{
    long hr;
    u32 n;
    u32 oldCount;

    hr = g_pD3DAppMain->GetDirectDraw()->RestoreAllSurfaces();
    if (hr >= 0) {
        oldCount = texturePageCount;
        for (n = 0; n < texturePageCount; n++) {
            if (textures[n]) {
                delete textures[n];
                textures[n] = 0;
            }
        }
        if (textures) {
            free(textures);
            textures = 0;
        }
        for (n = 0; n < immediateTexCount; n++) {
            if (texBatchVerts[n]) {
                delete texBatchVerts[n];
                texBatchVerts[n] = 0;
            }
        }
        if (texBatchVerts) {
            free(texBatchVerts);
            texBatchVerts = 0;
        }
        if (texStateFlags) {
            delete texStateFlags;
            texStateFlags = 0;
        }
        if (texBatchCounts) {
            delete texBatchCounts;
            texBatchCounts = 0;
        }
        LoadTexturePages(g_davTexturePath);
        if (texturePageCount != oldCount)
            hr = E_FAIL;
    }
    return hr;
}

/* 0x417a1e - whether a texture page has its own immediate batch. */
bool PolyBatcher::IsImmediateTexture(u32 textureIndex)
{
    return textureIndex < immediateTexCount;
}

/* 0x417a38 - clears target and z (clearColor, z = 1.0), begins the scene and applies the default state flags. */
void PolyBatcher::Render_BeginFrame()
{
    renderer->ClearTarget(clearColor);
    renderer->BeginScene();
    textureDirty = 1;
    renderer->SetStateFlagsInline(defaultStateFlags);
}

/* 0x417e59 - flush == 1 draws what is queued; anything else drops it. Ends the scene either way. */
void PolyBatcher::Render_EndFrame(u8 flush)
{
    if (flush == 1) {
        Flush();
    } else {
        lineBatchCount = 0;
        flatBatchCount = 0;
        sortedCount = 0;
        memset(texBatchCounts, 0, immediateTexCount * 4);
    }
    renderer->EndScene();
}

/* 0x417ecf - presents the frame (Blt into the window's client rectangle when the mode is the desktop-compatible
 * windowed one, Flip otherwise), recovers lost surfaces, then waits out the frame limiter. */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py) */
void PolyBatcher::Render_Present(u32 maxFps)
{
    D3DApp *d3dApp; /* names chosen for their stack slots */
    long hr;

    d3dApp = renderer;
    if (d3dApp->pPrimary == 0) {
        hr = E_FAIL;
    } else if (d3dApp->devices[d3dApp->deviceIndex].bDesktopCompatible &&
               d3dApp->devices[d3dApp->deviceIndex].modeIndex == 0) {
        hr = d3dApp->pPrimary->Blt(&d3dApp->clientRect, d3dApp->pBackBuffer, 0, DDBLT_WAIT, 0);
    } else {
        hr = d3dApp->pPrimary->Flip(0, DDFLIP_WAIT);
    }
    if (hr == DDERR_SURFACELOST)
        Render_RestoreLostSurfaces();
    renderer->Frame_LimitFps(maxFps);
}

/* 0x417faf - draws everything still queued: the untextured batch, each immediate page's batch, the lines, then the
 * sorted list back to front (qsort by sortZ, descending), and empties all of them. */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py) */
void PolyBatcher::Flush()
{
    u32 page; /* names chosen for their stack slots */
    u32 tex;
    u32 k;

    if (flatBatchCount > 0) {
        renderer->SetStateFlagsInline(stateFlags);
        renderer->DrawPrimitiveInline(D3DPT_TRIANGLELIST, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR,
                                      flatBatchVerts, flatBatchCount * 3);
        renderer->ClearStateFlagsInline(stateFlags);
        textureDirty = 1;
    }
    for (tex = 0; tex < immediateTexCount; tex++) {
        if (texBatchCounts[tex] > 0) {
            if (tex != lastTextureIndex || textureDirty == 1) {
                renderer->BindTexture(textures[tex], 0);
                lastTextureIndex = tex;
                textureDirty = 0;
            }
            renderer->Render_SetStateFlags(texStateFlags[tex]);
            renderer->DrawPrimitiveInline(D3DPT_TRIANGLELIST,
                                          D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1,
                                          texBatchVerts[tex], texBatchCounts[tex] * 3);
            renderer->Render_ClearStateFlags(texStateFlags[tex]);
        }
    }
    if (lineBatchCount > 0) {
        renderer->Render_SetStateFlags(lineStateFlags);
        renderer->DrawPrimitiveInline(D3DPT_LINELIST, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR, lineBatchVerts,
                                      lineBatchCount * 2);
        renderer->Render_ClearStateFlags(lineStateFlags);
        textureDirty = 1;
    }
    renderer->Render_SetStateFlags(RSF_ZWRITE_OFF);
    /* cast kept: qsort takes an untyped comparator; this one compares RenderPoly pointers */
    qsort(sortedList, sortedCount, 4, (int (*)(const void *, const void *))PolyBatcher_CompareSortZ);
    for (k = 0; k < sortedCount; k++) {
        switch (sortedList[k]->type) {
            case RPOLY_BLEND:
                renderer->Render_SetStateFlags(blendStateFlags);
                renderer->DrawPrimitiveInline(D3DPT_TRIANGLELIST, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR,
                                              sortedList[k]->verts, 3);
                renderer->Render_ClearStateFlags(blendStateFlags);
                textureDirty = 1;
                break;
            case RPOLY_ADD:
                renderer->Render_SetStateFlags(addStateFlags);
                renderer->DrawPrimitiveInline(D3DPT_TRIANGLELIST, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR,
                                              sortedList[k]->verts, 3);
                renderer->Render_ClearStateFlags(addStateFlags);
                textureDirty = 1;
                break;
            default:
                page = sortedList[k]->type - RPOLY_TEXTURED_BASE & ~RPOLY_F_8000;
                if (page != lastTextureIndex || textureDirty == 1) {
                    renderer->BindTexture(textures[page], 0);
                    lastTextureIndex = page;
                    textureDirty = 0;
                }
                renderer->Render_SetStateFlags(texStateFlags[page]);
                renderer->DrawPrimitiveInline(D3DPT_TRIANGLELIST,
                                              D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1,
                                              sortedList[k]->verts, 3);
                renderer->Render_ClearStateFlags(texStateFlags[page]);
        }
    }
    renderer->Render_ClearStateFlags(RSF_ZWRITE_OFF | RSF_TEXTURED);
    lineBatchCount = 0;
    flatBatchCount = 0;
    sortedCount = 0;
    memset(texBatchCounts, 0, immediateTexCount * 4);
}

/* 0x418a5c - qsort comparator: larger sortZ first (back to front). */
int PolyBatcher_CompareSortZ(RenderPoly **a, RenderPoly **b)
{
    if ((*a)->sortZ == (*b)->sortZ)
        return 0;
    else if ((*a)->sortZ > (*b)->sortZ)
        return -1;
    else
        return 1;
}
