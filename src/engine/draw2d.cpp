/* match-init: StaticInit_g_6d6780 StaticInit_g_6d656c StaticInit_g_matrix6d5428 StaticInit_g_matrix6d6ec8 StaticInit_g_draw2dPoly StaticInit_g_draw2dImmVerts StaticInit_g_draw2dImmTexVerts */
/*
 * T256 - guessed original name: Draw2D.cpp (globals + 2D primitives). SheepD3D.exe .text 0x5228f0-0x5265f7,
 * .bss 0x6d5218-0x6d6fd0. No .data / .rdata of its own.
 * The file that owns the application globals (instance and window handles, the data paths, the device / frustum /
 * batcher pointers, the shared collision scratch buffers), the static input manager, the text resource bank, two
 * matrices, the scratch RenderPoly and the two immediate-mode vertex arrays (their static initialisers, atexit
 * registrations and destructors, 0x5228f0-0x522a64), and the Draw2D_* primitives: flat / gouraud / textured triangles
 * and rectangles pushed through the PolyBatcher (0x522a65-0x5247d0) and the immediate-mode ones that draw straight
 * through the device (0x5247d1-0x5265f6). Draw_StaticInit_Empty 0x5228de ends the previous object
 * (src/engine/debug_draw.cpp, T255); the fixed-point converters after the immediate primitives are
 * src/engine/fixed_math.cpp (T257). The map's possible hidden split at 0x5229a0 (globals | Draw2D) is not taken.
 *
 * The static-initialiser thunks are placed from the match-init roots, in the order the constructed globals are defined.
 *
 * The queued primitives fill g_d2dScratchPoly's vertex block and expand PolyBatcher::SubmitPoly (0x415c80) in place;
 * the rectangles then patch one vertex and submit the second triangle with a real call. /Ob1 expands the
 * inline draw helpers differently per caller (the rectangles' textured flush calls Render_DrawPrimitive 0x415c00), so
 * the expansions are written as inline twins (SubmitPolyTri, SubmitPolyRect) that say which helpers stay calls. The
 * immediate primitives fill the static TL vertex arrays, apply the render-state flags (ApplyStateFlags, the inline twin
 * of Render_SetStateFlags 0x4155f0), call IDirect3DDevice7::DrawPrimitive directly and restore the states with the
 * out-of-line Render_ClearStateFlags 0x4159b0. Twin and helper names emit no code of their own and are not recovered. A shape
 * that reproduces the bytes is a representation, not proof that the original source read this way.
 *
 * The .bss order. VC6 lays out an object's .bss as (1) every global defined without an initialiser, constructed
 * objects included, sorted by a hash of its NAME, key (h ^ h >> 16) & 0x3ff with h = (h << 2) + (h >> 4) + c, then (2)
 * the globals explicitly initialised to zero, in definition order. Here the last item, g_d2dScratchPoly at 0x6d6fb0, is
 * a constructed object, which can only be in group (1); so every item of this .bss is in group (1), and the original's
 * names must have hashed in address order. No source shape other than the names moves an item of group (1) (tested:
 * prior extern declarations, extern "C", `= T()` for a constructed object, static). The 27 named globals and the
 * unreferenced block therefore carry names whose keys ascend with their addresses (g_hInstance 79, g_musicsPath 147, ...
 * g_d2dScratchPoly 1000); the names are representations chosen for their keys (tools/layout.py --tu T256 checks the layout).
 */
/* BYTES: bss-name, dead-code, inline, layout, temp, view. */
/* BYTES(bss-name): named for its .bss hash key 147 so the object's .bss keeps address order */
/* BYTES(bss-name): named for its .bss hash key 162 so the object's .bss keeps address order */
/* BYTES(bss-name): named for its .bss hash key 186 so the object's .bss keeps address order */
/* BYTES(bss-name): named for its .bss hash key 226 so the object's .bss keeps address order */
/* BYTES(bss-name): named for its .bss hash key 227 so the object's .bss keeps address order */
/* BYTES(bss-name): named for its .bss hash key 271 so the object's .bss keeps address order */
/* BYTES(bss-name): named for its .bss hash key 350 so the object's .bss keeps address order */
/* BYTES(bss-name): named for its .bss hash key 361 so the object's .bss keeps address order */
/* BYTES(bss-name): named for its .bss hash key 404 so the object's .bss keeps address order */
/* BYTES(bss-name): named for its .bss hash key 575 so the object's .bss keeps address order */
/* BYTES(bss-name): named for its .bss hash key 581 so the object's .bss keeps address order */
/* BYTES(bss-name): named for its .bss hash key 642 so the object's .bss keeps address order */
/* BYTES(bss-name): named for its .bss hash key 723 so the object's .bss keeps address order */
/* BYTES(bss-name): named for its .bss hash key 743 so the object's .bss keeps address order */
/* BYTES(bss-name): named for its .bss hash key 768 so the object's .bss keeps address order */
/* BYTES(bss-name): named for its .bss hash key 784 so the object's .bss keeps address order */
/* BYTES(bss-name): named for its .bss hash key 860 so the object's .bss keeps address order */
/* BYTES(bss-name): named for its .bss hash key 896 so the object's .bss keeps address order */
/* BYTES(bss-name): named for its .bss hash key 897 so the object's .bss keeps address order */
/* BYTES(bss-name): named for its .bss hash key 946 so the object's .bss keeps address order */
/* BYTES(bss-name): named for its .bss hash key 975 so the object's .bss keeps address order */
/* BYTES(bss-name): named for its .bss hash key 1000 so the object's .bss keeps address order */
/* BYTES(bss-name): placeholder: 3072 unreferenced bytes kept only for the layout (uninitialised and not static, so VC6 keeps it in the name-hash group) */
/* BYTES(layout): defined in the order of their static initialisers (0x5228f0-0x522a64), not in address order */
/* BYTES(inline): ImmFlatVertex() {} / ImmTexVertex() {}: empty user-declared constructor: gives the original's empty vector-constructor loops 0x5229e9 / 0x522a2c */
/* BYTES(view): class, not struct: VC6 mangles the class-key of the first declaration and the callers link against PAVTexture@@ */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "../sdk/d3d7.h"
#include "../sdk/win32.h"
class Mat44;
#include "../sdk/crt.h"
struct FlatVertex { /* the vertex of an untextured RenderPoly (FVF 0xc4) */
    float x, y, z, rhw;
    u32 diffuse, specular;
};
struct TexVertex { /* a D3DTLVERTEX (FVF 0x1c4), the vertex of a textured RenderPoly */
    float x, y, z, rhw;
    u32 diffuse, specular;
    float u, v;
};
/* the immediate-mode vertex arrays: the SDK's D3DTLVERTEX-style empty inline constructor gives the (empty)
 * vector-constructor loops 0x5229e9 / 0x522a2c */
struct ImmFlatVertex {
    float x, y, z, rhw;
    u32 diffuse, specular;
    ImmFlatVertex() {}
};
struct ImmTexVertex {
    float x, y, z, rhw;
    u32 diffuse, specular;
    float u, v;
    ImmTexVertex() {}
};
/* `class`, as sdw_classes.h has it: VC6 mangles the class-key of the first declaration it sees, and
 * Draw2D_TexTri/TexRect_Immediate's callers link against ...PAVTexture@@. */
class Texture;

#define SDW_MEMBERS_InputMgr                      \
    InputMgr(); /* 0x527a00 InputMgr_Construct */ \
    /* ~InputMgr 0x527abc is the virtual destructor sdw_classes.h declares */
#define SDW_MEMBERS_TextResBank TextResBank(); /* 0x54f8f0 */
#define SDW_MEMBERS_Mat44 Mat44();             /* 0x4077f0 */
#define SDW_MEMBERS_RenderPoly                                              \
    RenderPoly();                            /* 0x41aad0 RenderPoly_Ctor */ \
    /* virtual ~RenderPoly() is generated */ /* 0x41ac04 RenderPoly_Dtor */
#define SDW_MEMBERS_PolyBatcher                       \
    void SubmitPoly(RenderPoly *poly); /* 0x415c80 */ \
    void SubmitPolyTri(RenderPoly *poly);             \
    void SubmitPolyRect(RenderPoly *poly);            \
    void InvalidateTexture()                          \
    {                                                 \
        textureDirty = 1;                             \
    } /* inline */
#define SDW_MEMBERS_D3DApp                                                             \
    void Render_SetStateFlags(u32 flags);   /* 0x4155f0 */                             \
    void Render_ClearStateFlags(u32 flags); /* 0x4159b0 */                             \
    void Render_DrawPrimitive(u32 type, u32 fvf, void *verts, u32 count);              \
    void DrawPrimitiveInline(u32 type, u32 fvf, void *verts, u32 count);               \
    void SetTextureInline(Texture *tex, volatile u32 stage);                           \
    void SetStateFlagsInline(u32 flags);                                               \
    /* the immediate primitives' helpers */                                            \
    void BindTexture(Texture *tex, s32 stage);             /* inline, defined below */ \
    void DrawTLTriangles(u32 fvf, void *verts, u32 count); /* inline, defined below */ \
    void ApplyStateFlags(u32 flags);                       /* inline, defined below */
#include "sdw_classes.h"
#define SDW_INLINE_D3DAPP_DRAWPRIMITIVEINLINE_U32_U32_VOID_U32 1
#include "../app/d3dapp_inlines.h"
#undef SDW_INLINE_D3DAPP_DRAWPRIMITIVEINLINE_U32_U32_VOID_U32

/* BYTES(temp): volatile by-value parameter: the original copies the stage to a stack temp (see engine/mesh.cpp) */
#define SDW_INLINE_D3DAPP_SETTEXTUREINLINE_TEXTURE_VOLATILE_U32 1
#include "../app/d3dapp_inlines.h"
#undef SDW_INLINE_D3DAPP_SETTEXTUREINLINE_TEXTURE_VOLATILE_U32

/* 0x4155f0 Render_SetStateFlags as the immediate primitives expand it: each RenderStateFlags bit sets its device
 * states. */
/* BYTES(inline): __forceinline twin of Render_SetStateFlags 0x4155f0, as the immediate primitives expand it */
#define SDW_INLINE_D3DAPP_SETSTATEFLAGSINLINE_U32 1
#include "../app/d3dapp_inlines.h"
#undef SDW_INLINE_D3DAPP_SETSTATEFLAGSINLINE_U32

/* ---- this object's .bss, 0x6d5218-0x6d6fd0, in address order (see the header: the compiler reorders them). The
 * constructed globals are defined further down, in the order of their static initialisers. ---- */
HINSTANCE__ *g_hInstance;    /* 0x6d5218 */
char g_musicsPath[256];      /* 0x6d5220 */
SoundDevice *g_pSoundSystem; /* 0x6d5320 */
char g_dirBonusGame[256];    /* 0x6d5328  exeDir + ".\Bonus\" */
/* g_matUnk6d5428                              0x6d5428  (constructed, below) */
u8 g_sharedScratch[512]; /* 0x6d5468  shared scratch (collision, camera, shadow, text...) */
/* 0x6d5668: the second 512-byte scratch: the ping-pong vertex buffer of the Poly_Clip* chain and the second merge
 * buffer of the segment tests. C name, as its users declare it. Its last 18 bytes (0x6d5856) are the game-space
 * vertices of the triangle under test, collide.cpp's g_collSegTriVerts: not a global of its own (a separate 18-byte global
 * would be 4-aligned; 0x6d5856 is only 2-aligned), see src/engine/collide.cpp. (`extern "C" { }`: the one-line form
 * `extern "C" u8 x[N];` would only declare it.) */
extern "C" {
u8 g_clipTriBuffer[0x200];
}
/* 0x6d5868-0x6d6468: 3072 bytes no instruction refers to (tools/tu_sheet.py). Genuinely opaque: nothing reads or writes
 * them, so neither their type nor their name can be recovered; kept as bytes. Uninitialised, like its neighbours (it
 * lies between items of the name-sorted group), and not static, because VC6 drops an unreferenced uninitialised
 * static (tested). */
u8 g_unref6d5868[0xc00];
char g_pathScene[256]; /* 0x6d6468 */
D3DApp *g_pD3DAppMain; /* 0x6d6568 */
/* g_textCatalog                               0x6d656c  (constructed, below) */
char g_introDir[256];        /* 0x6d6578 */
u32 g_maxImmediateTriangles; /* 0x6d6678 */
char g_pathWheelDir[256];    /* 0x6d6680 */
/* g_inputMgr                                  0x6d6780  (constructed, below) */
char g_voiceDir[256];     /* 0x6d68b8 */
char g_pathEnding[256];   /* 0x6d69b8 */
char g_pathDemoDir[256];  /* 0x6d6ab8 */
char g_levelPathFmt[256]; /* 0x6d6bb8  exeDir + ".\Levels\Lvl-%02d\Lvl-%02d" */
char g_dirReference[256]; /* 0x6d6cb8 */
char g_pathFendDir[256];  /* 0x6d6db8 */
HWND__ *g_hGameWindow;    /* 0x6d6eb8 */
Frustrum *g_pViewFrustum; /* 0x6d6ebc */
PolyBatcher *g_pPolyBin;  /* 0x6d6ec0 */
/* g_projMatrix                              0x6d6ec8  (constructed, below; g_projDepthBias 0x6d6ef0 and
 *                                             g_projDepthScale 0x6d6f00 are two of its elements) */
/* g_draw2dImmVerts, g_draw2dImmTexCorners, g_d2dScratchPoly   0x6d6f08, 0x6d6f50, 0x6d6fb0  (constructed, below) */

/* ---- the constructed globals, in the order of their static initialisers (0x5228f0-0x522a64) ---- */
InputMgr g_inputMgr;                   /* 0x6d6780 */
TextResBank g_textCatalog;             /* 0x6d656c */
Mat44 g_matUnk6d5428;                  /* 0x6d5428 */
Mat44 g_projMatrix;                    /* 0x6d6ec8  the projection matrix (Screen_SetProjection) */
RenderPoly g_d2dScratchPoly;           /* 0x6d6fb0  the scratch poly of the queued primitives */
ImmFlatVertex g_draw2dImmVerts[3];     /* 0x6d6f08 */
ImmTexVertex g_draw2dImmTexCorners[3]; /* 0x6d6f50 */

/* 0x415c80 PolyBatcher_SubmitPoly as the triangle primitives expand it: the state set / clear stay calls, both
 * draws are expanded. */
/* BYTES(inline): source-only inline twin of SubmitPoly 0x415c80 for the triangle primitives: state set / clear stay calls, both draws expanded */
inline void PolyBatcher::SubmitPolyTri(RenderPoly *poly)
{
    TexVertex *tri;
    u32 idx6;
    TexVertex *batch;
    u32 *num;
    u32 *pflags;

    switch (poly->type) {
        case RPOLY_OPAQUE:
            if (flatBatchCount <= batchCapacity) {
                /* cast kept: the flat batch is an untyped buffer (PolyBatcher.flatBatchVerts is a void *) */
                memcpy((u8 *)flatBatchVerts + flatBatchCount * 3 * 0x18, poly->verts, 0x48);
                flatBatchCount++;
            }
            if (flatBatchCount >= batchCapacity) {
                renderer->Render_SetStateFlags(stateFlags);
                renderer->DrawPrimitiveInline(D3DPT_TRIANGLELIST, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR,
                                              flatBatchVerts, batchCapacity * 3);
                renderer->Render_ClearStateFlags(stateFlags);
                flatBatchCount = 0;
                textureDirty = 1;
            }
            break;
        case RPOLY_BLEND:
            if (computeSortZ == 1)
                poly->sortZ = poly->verts[2] + poly->verts[8] + poly->verts[14];
            sortedPolys[sortedCount].Assign(poly);
            sortedList[sortedCount] = &sortedPolys[sortedCount];
            sortedCount++;
            break;
        case RPOLY_ADD:
            if (computeSortZ == 1)
                poly->sortZ = poly->verts[2] + poly->verts[8] + poly->verts[14];
            sortedPolys[sortedCount].Assign(poly);
            sortedList[sortedCount] = &sortedPolys[sortedCount];
            sortedCount++;
            break;
        default:
            /* cast kept: RenderPoly.verts is a raw float block; a textured poly holds three TL vertices */
            tri = (TexVertex *)poly->verts;
            idx6 = poly->type - RPOLY_TEXTURED_BASE & ~RPOLY_F_8000;
            if (idx6 < immediateTexCount) {
                /* cast kept: a page batch is an untyped buffer of TL vertices (texBatchVerts holds void *) */
                batch = (TexVertex *)texBatchVerts[idx6];
                num = &texBatchCounts[idx6];
                pflags = &texStateFlags[idx6];
                if (*num <= batchCapacity) {
                    memcpy(&batch[*num * 3], tri, 0x60);
                    (*num)++;
                }
                if (*num >= batchCapacity) {
                    if (idx6 != lastTextureIndex || textureDirty == 1) {
                        renderer->SetTextureInline(textures[idx6], 0);
                        lastTextureIndex = idx6;
                        textureDirty = 0;
                    }
                    renderer->Render_SetStateFlags(*pflags);
                    renderer->DrawPrimitiveInline(D3DPT_TRIANGLELIST,
                                                  D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1, batch,
                                                  batchCapacity * 3);
                    renderer->Render_ClearStateFlags(*pflags);
                    *num = 0;
                }
            } else {
                if (computeSortZ == 1)
                    poly->sortZ = tri[0].z + tri[1].z + tri[2].z;
                sortedPolys[sortedCount].Assign(poly);
                sortedList[sortedCount] = &sortedPolys[sortedCount];
                sortedCount++;
            }
    }
}

/* 0x415c80 as the rectangle primitives expand it: as SubmitPolyTri, but the textured flush draws through the
 * out-of-line Render_DrawPrimitive. */
/* BYTES(inline): source-only inline twin of SubmitPoly 0x415c80 for the rectangles: the textured flush calls Render_DrawPrimitive 0x415c00 */
inline void PolyBatcher::SubmitPolyRect(RenderPoly *poly)
{
    TexVertex *tri;
    u32 idx6;
    TexVertex *batch;
    u32 *num;
    u32 *pflags;

    switch (poly->type) {
        case RPOLY_OPAQUE:
            if (flatBatchCount <= batchCapacity) {
                /* cast kept: the flat batch is an untyped buffer (PolyBatcher.flatBatchVerts is a void *) */
                memcpy((u8 *)flatBatchVerts + flatBatchCount * 3 * 0x18, poly->verts, 0x48);
                flatBatchCount++;
            }
            if (flatBatchCount >= batchCapacity) {
                renderer->Render_SetStateFlags(stateFlags);
                renderer->DrawPrimitiveInline(D3DPT_TRIANGLELIST, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR,
                                              flatBatchVerts, batchCapacity * 3);
                renderer->Render_ClearStateFlags(stateFlags);
                flatBatchCount = 0;
                textureDirty = 1;
            }
            break;
        case RPOLY_BLEND:
            if (computeSortZ == 1)
                poly->sortZ = poly->verts[2] + poly->verts[8] + poly->verts[14];
            sortedPolys[sortedCount].Assign(poly);
            sortedList[sortedCount] = &sortedPolys[sortedCount];
            sortedCount++;
            break;
        case RPOLY_ADD:
            if (computeSortZ == 1)
                poly->sortZ = poly->verts[2] + poly->verts[8] + poly->verts[14];
            sortedPolys[sortedCount].Assign(poly);
            sortedList[sortedCount] = &sortedPolys[sortedCount];
            sortedCount++;
            break;
        default:
            /* cast kept: RenderPoly.verts is a raw float block; a textured poly holds three TL vertices */
            tri = (TexVertex *)poly->verts;
            idx6 = poly->type - RPOLY_TEXTURED_BASE & ~RPOLY_F_8000;
            if (idx6 < immediateTexCount) {
                /* cast kept: a page batch is an untyped buffer of TL vertices (texBatchVerts holds void *) */
                batch = (TexVertex *)texBatchVerts[idx6];
                num = &texBatchCounts[idx6];
                pflags = &texStateFlags[idx6];
                if (*num <= batchCapacity) {
                    memcpy(&batch[*num * 3], tri, 0x60);
                    (*num)++;
                }
                if (*num >= batchCapacity) {
                    if (idx6 != lastTextureIndex || textureDirty == 1) {
                        renderer->SetTextureInline(textures[idx6], 0);
                        lastTextureIndex = idx6;
                        textureDirty = 0;
                    }
                    renderer->Render_SetStateFlags(*pflags);
                    renderer->Render_DrawPrimitive(D3DPT_TRIANGLELIST,
                                                   D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1,
                                                   batch, batchCapacity * 3);
                    renderer->Render_ClearStateFlags(*pflags);
                    *num = 0;
                }
            } else {
                if (computeSortZ == 1)
                    poly->sortZ = tri[0].z + tri[1].z + tri[2].z;
                sortedPolys[sortedCount].Assign(poly);
                sortedList[sortedCount] = &sortedPolys[sortedCount];
                sortedCount++;
            }
    }
}

/* cast kept: RenderPoly.verts is a raw float block; an untextured poly holds three flat vertices */
#define DRAW2D_FLAT ((FlatVertex *)g_d2dScratchPoly.verts)

/* 0x522a65 - one untextured triangle; blendMode 0 = opaque (type 1), otherwise the poly type (2/3 sorted blends) with
 * the sort depth z. rhw is 1 / the near plane, specular opaque black. */
void Draw2D_FlatTri(float z, float x0, float y0, float x1, float y1, float x2, float y2, u32 color, u8 blendMode)
{
    float rhw;

    if (blendMode == 0) {
        g_d2dScratchPoly.type = RPOLY_OPAQUE;
    } else {
        g_d2dScratchPoly.type = blendMode;
        g_d2dScratchPoly.sortZ = z;
    }
    rhw = 1.0f / (g_pViewFrustum->nearZ);
    DRAW2D_FLAT[0].x = x0;
    DRAW2D_FLAT[0].y = y0;
    DRAW2D_FLAT[0].z = z;
    DRAW2D_FLAT[0].rhw = rhw;
    DRAW2D_FLAT[0].diffuse = color;
    DRAW2D_FLAT[0].specular = 0xff000000;
    DRAW2D_FLAT[1].x = x1;
    DRAW2D_FLAT[1].y = y1;
    DRAW2D_FLAT[1].z = z;
    DRAW2D_FLAT[1].rhw = rhw;
    DRAW2D_FLAT[1].diffuse = color;
    DRAW2D_FLAT[1].specular = 0xff000000;
    DRAW2D_FLAT[2].x = x2;
    DRAW2D_FLAT[2].y = y2;
    DRAW2D_FLAT[2].z = z;
    DRAW2D_FLAT[2].rhw = rhw;
    DRAW2D_FLAT[2].diffuse = color;
    DRAW2D_FLAT[2].specular = 0xff000000;
    g_pPolyBin->SubmitPolyTri(&g_d2dScratchPoly);
}

/* 0x522f45 - an untextured axis-aligned rectangle as two triangles: (x0,y0) (x1,y0) (x1,y1), then the middle vertex
 * moved to (x0,y1). */
void Draw2D_FlatRect(float z, float x0, float y0, float x1, float y1, u32 color, u8 blendMode)
{
    float rhw;

    if (blendMode == 0) {
        g_d2dScratchPoly.type = RPOLY_OPAQUE;
    } else {
        g_d2dScratchPoly.type = blendMode;
        g_d2dScratchPoly.sortZ = z;
    }
    rhw = 1.0f / (g_pViewFrustum->nearZ);
    DRAW2D_FLAT[0].x = x0;
    DRAW2D_FLAT[0].y = y0;
    DRAW2D_FLAT[0].z = z;
    DRAW2D_FLAT[0].rhw = rhw;
    DRAW2D_FLAT[0].diffuse = color;
    DRAW2D_FLAT[0].specular = 0xff000000;
    DRAW2D_FLAT[1].x = x1;
    DRAW2D_FLAT[1].y = y0;
    DRAW2D_FLAT[1].z = z;
    DRAW2D_FLAT[1].rhw = rhw;
    DRAW2D_FLAT[1].diffuse = color;
    DRAW2D_FLAT[1].specular = 0xff000000;
    DRAW2D_FLAT[2].x = x1;
    DRAW2D_FLAT[2].y = y1;
    DRAW2D_FLAT[2].z = z;
    DRAW2D_FLAT[2].rhw = rhw;
    DRAW2D_FLAT[2].diffuse = color;
    DRAW2D_FLAT[2].specular = 0xff000000;
    g_pPolyBin->SubmitPolyRect(&g_d2dScratchPoly);
    DRAW2D_FLAT[1].x = x0;
    DRAW2D_FLAT[1].y = y1;
    g_pPolyBin->SubmitPoly(&g_d2dScratchPoly);
}

/* 0x52342e - Draw2D_FlatTri with a colour per vertex. No callers. */
void Draw2D_GouraudTri(float z, float x0, float y0, u32 c0, float x1, float y1, u32 c1, float x2, float y2, u32 c2,
                       u8 blendMode)
{
    float rhw;

    if (blendMode == 0) {
        g_d2dScratchPoly.type = RPOLY_OPAQUE;
    } else {
        g_d2dScratchPoly.type = blendMode;
        g_d2dScratchPoly.sortZ = z;
    }
    rhw = 1.0f / (g_pViewFrustum->nearZ);
    DRAW2D_FLAT[0].x = x0;
    DRAW2D_FLAT[0].y = y0;
    DRAW2D_FLAT[0].z = z;
    DRAW2D_FLAT[0].rhw = rhw;
    DRAW2D_FLAT[0].diffuse = c0;
    DRAW2D_FLAT[0].specular = 0xff000000;
    DRAW2D_FLAT[1].x = x1;
    DRAW2D_FLAT[1].y = y1;
    DRAW2D_FLAT[1].z = z;
    DRAW2D_FLAT[1].rhw = rhw;
    DRAW2D_FLAT[1].diffuse = c1;
    DRAW2D_FLAT[1].specular = 0xff000000;
    DRAW2D_FLAT[2].x = x2;
    DRAW2D_FLAT[2].y = y2;
    DRAW2D_FLAT[2].z = z;
    DRAW2D_FLAT[2].rhw = rhw;
    DRAW2D_FLAT[2].diffuse = c2;
    DRAW2D_FLAT[2].specular = 0xff000000;
    g_pPolyBin->SubmitPolyTri(&g_d2dScratchPoly);
}

/* 0x52390e - Draw2D_FlatRect with a colour per corner: (x0,y0) cTL, (x1,y0) cTR, (x1,y1) cBR, then the middle vertex
 * moved to (x0,y1) cBL. */
void Draw2D_GouraudRect(float z, float x0, float y0, float x1, float y1, u32 cTL, u32 cBL, u32 cTR, u32 cBR,
                        u8 blendMode)
{
    float rhw;

    if (blendMode == 0) {
        g_d2dScratchPoly.type = RPOLY_OPAQUE;
    } else {
        g_d2dScratchPoly.type = blendMode;
        g_d2dScratchPoly.sortZ = z;
    }
    rhw = 1.0f / (g_pViewFrustum->nearZ);
    DRAW2D_FLAT[0].x = x0;
    DRAW2D_FLAT[0].y = y0;
    DRAW2D_FLAT[0].z = z;
    DRAW2D_FLAT[0].rhw = rhw;
    DRAW2D_FLAT[0].diffuse = cTL;
    DRAW2D_FLAT[0].specular = 0xff000000;
    DRAW2D_FLAT[1].x = x1;
    DRAW2D_FLAT[1].y = y0;
    DRAW2D_FLAT[1].z = z;
    DRAW2D_FLAT[1].rhw = rhw;
    DRAW2D_FLAT[1].diffuse = cTR;
    DRAW2D_FLAT[1].specular = 0xff000000;
    DRAW2D_FLAT[2].x = x1;
    DRAW2D_FLAT[2].y = y1;
    DRAW2D_FLAT[2].z = z;
    DRAW2D_FLAT[2].rhw = rhw;
    DRAW2D_FLAT[2].diffuse = cBR;
    DRAW2D_FLAT[2].specular = 0xff000000;
    g_pPolyBin->SubmitPolyRect(&g_d2dScratchPoly);
    DRAW2D_FLAT[1].x = x0;
    DRAW2D_FLAT[1].y = y1;
    DRAW2D_FLAT[1].diffuse = cBL;
    g_pPolyBin->SubmitPoly(&g_d2dScratchPoly);
}

/* 0x523e03 - a textured triangle: poly type = texture page + 4, sort depth z, per-vertex uv and colour. */
void Draw2D_TexTri(float z, float x0, float y0, float x1, float y1, float x2, float y2, s32 texIndex, float u0,
                   float v0, u32 c0, float u1, float v1, u32 c1, float u2, float v2, u32 c2)
{
    float rhw;
    TexVertex *v;

    g_d2dScratchPoly.type = texIndex + RPOLY_TEXTURED_BASE;
    g_d2dScratchPoly.sortZ = z;
    rhw = 1.0f / (g_pViewFrustum->nearZ);
    v = (TexVertex *)g_d2dScratchPoly.verts; /* cast kept: the raw float block holds three TL vertices */
    v[0].x = x0;
    v[0].y = y0;
    v[0].z = z;
    v[0].rhw = rhw;
    v[0].diffuse = c0;
    v[0].specular = 0xff000000;
    v[1].x = x1;
    v[1].y = y1;
    v[1].z = z;
    v[1].rhw = rhw;
    v[1].diffuse = c1;
    v[1].specular = 0xff000000;
    v[2].x = x2;
    v[2].y = y2;
    v[2].z = z;
    v[2].rhw = rhw;
    v[2].diffuse = c2;
    v[2].specular = 0xff000000;
    v[0].u = u0;
    v[0].v = v0;
    v[1].u = u1;
    v[1].v = v1;
    v[2].u = u2;
    v[2].v = v2;
    g_pPolyBin->SubmitPolyTri(&g_d2dScratchPoly);
}

/* 0x5242df - the general textured rectangle (texture page texIndex, uv and colour per corner) as two triangles:
 * TL, TR, BR, then the middle vertex moved to BL. cBL is never read: the BL corner keeps cTR's colour. */
void Draw2D_TexRect(float z, float x0, float y0, float x1, float y1, s32 texIndex, float uTL, float vTL, u32 cTL,
                    float uBL, float vBL, u32 cBL, float uTR, float vTR, u32 cTR, float uBR, float vBR, u32 cBR)
{
    float rhw;
    TexVertex *v;

    g_d2dScratchPoly.type = texIndex + RPOLY_TEXTURED_BASE;
    g_d2dScratchPoly.sortZ = z;
    rhw = 1.0f / (g_pViewFrustum->nearZ);
    v = (TexVertex *)g_d2dScratchPoly.verts; /* cast kept: the raw float block holds three TL vertices */
    v[0].x = x0;
    v[0].y = y0;
    v[0].z = z;
    v[0].rhw = rhw;
    v[0].diffuse = cTL;
    v[0].specular = 0xff000000;
    v[1].x = x1;
    v[1].y = y0;
    v[1].z = z;
    v[1].rhw = rhw;
    v[1].diffuse = cTR;
    v[1].specular = 0xff000000;
    v[2].x = x1;
    v[2].y = y1;
    v[2].z = z;
    v[2].rhw = rhw;
    v[2].diffuse = cBR;
    v[2].specular = 0xff000000;
    v[0].u = uTL;
    v[0].v = vTL;
    v[1].u = uTR;
    v[1].v = vTR;
    v[2].u = uBR;
    v[2].v = vBR;
    g_pPolyBin->SubmitPolyRect(&g_d2dScratchPoly);
    v[1].x = x0;
    v[1].y = y1;
    v[1].u = uBL;
    v[1].v = vBL;
    g_pPolyBin->SubmitPoly(&g_d2dScratchPoly);
}

/* 0x5247d1 - an untextured triangle drawn at once through the device (not queued) with the given RenderStateFlags,
 * from the static vertex array g_draw2dImmVerts. */
void Draw2D_FlatTri_Immediate(float z, float x0, float y0, float x1, float y1, float x2, float y2, u32 renderFlags,
                              u32 color)
{
    float rhw;

    rhw = 1.0f / (g_pViewFrustum->nearZ);
    g_draw2dImmVerts[0].x = x0;
    g_draw2dImmVerts[0].y = y0;
    g_draw2dImmVerts[0].z = z;
    g_draw2dImmVerts[0].rhw = rhw;
    g_draw2dImmVerts[0].diffuse = color;
    g_draw2dImmVerts[0].specular = 0xff000000;
    g_draw2dImmVerts[1].x = x1;
    g_draw2dImmVerts[1].y = y1;
    g_draw2dImmVerts[1].z = z;
    g_draw2dImmVerts[1].rhw = rhw;
    g_draw2dImmVerts[1].diffuse = color;
    g_draw2dImmVerts[1].specular = 0xff000000;
    g_draw2dImmVerts[2].x = x2;
    g_draw2dImmVerts[2].y = y2;
    g_draw2dImmVerts[2].z = z;
    g_draw2dImmVerts[2].rhw = rhw;
    g_draw2dImmVerts[2].diffuse = color;
    g_draw2dImmVerts[2].specular = 0xff000000;
    g_pD3DAppMain->SetStateFlagsInline(renderFlags);
    g_pD3DAppMain->DrawPrimitiveInline(D3DPT_TRIANGLELIST, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR,
                                       g_draw2dImmVerts, 3);
    g_pD3DAppMain->Render_ClearStateFlags(renderFlags);
}

/* 0x524c82 - an untextured rectangle drawn at once: (x0,y0) (x1,y0) (x1,y1), then the middle vertex moved to (x0,y1).
 * The local vertex array is never used (only its empty constructor loop remains). */
/* BYTES(dead-code): verts is never used: only its empty constructor loop is in the original */
void Draw2D_FlatRect_Immediate(float z, float x0, float y0, float x1, float y1, u32 renderFlags, u32 color)
{
    ImmFlatVertex verts[3];
    float rhw;

    rhw = 1.0f / (g_pViewFrustum->nearZ);
    g_draw2dImmVerts[0].x = x0;
    g_draw2dImmVerts[0].y = y0;
    g_draw2dImmVerts[0].z = z;
    g_draw2dImmVerts[0].rhw = rhw;
    g_draw2dImmVerts[0].diffuse = color;
    g_draw2dImmVerts[0].specular = 0xff000000;
    g_draw2dImmVerts[1].x = x1;
    g_draw2dImmVerts[1].y = y0;
    g_draw2dImmVerts[1].z = z;
    g_draw2dImmVerts[1].rhw = rhw;
    g_draw2dImmVerts[1].diffuse = color;
    g_draw2dImmVerts[1].specular = 0xff000000;
    g_draw2dImmVerts[2].x = x1;
    g_draw2dImmVerts[2].y = y1;
    g_draw2dImmVerts[2].z = z;
    g_draw2dImmVerts[2].rhw = rhw;
    g_draw2dImmVerts[2].diffuse = color;
    g_draw2dImmVerts[2].specular = 0xff000000;
    g_pD3DAppMain->SetStateFlagsInline(renderFlags);
    g_pD3DAppMain->DrawPrimitiveInline(D3DPT_TRIANGLELIST, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR,
                                       g_draw2dImmVerts, 3);
    g_draw2dImmVerts[1].x = x0;
    g_draw2dImmVerts[1].y = y1;
    g_pD3DAppMain->DrawPrimitiveInline(D3DPT_TRIANGLELIST, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR,
                                       g_draw2dImmVerts, 3);
    g_pD3DAppMain->Render_ClearStateFlags(renderFlags);
}

/* ---- the immediate primitives, continued (0x5251a6-0x5265f6) ---- */

#define SDW_INLINE_D3DAPP_BINDTEXTURE_TEXTURE_S32 1
#include "../app/d3dapp_inlines.h"
#undef SDW_INLINE_D3DAPP_BINDTEXTURE_TEXTURE_S32

inline void D3DApp::DrawTLTriangles(u32 fvf, void *verts, u32 count)
{
    if (count)
        pD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, fvf, verts, count, 0);
}

/* inline twin of Render_SetStateFlags 0x4155f0 (as src/fx/holefx.cpp) */
/* BYTES(inline): source-only inline twin of Render_SetStateFlags 0x4155f0 */
inline void D3DApp::ApplyStateFlags(u32 flags)
{
    if (flags & RSF_ANTIALIAS)
        pD3DDevice->SetRenderState(D3DRENDERSTATE_ANTIALIAS, D3DANTIALIAS_SORTINDEPENDENT);
    if (flags & RSF_BLEND_ALPHA) {
        pD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);
        pD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_INVSRCALPHA);
        pD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_SRCALPHA);
    }
    if (flags & RSF_BLEND_ADD) {
        pD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);
        pD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_INVSRCALPHA);
        pD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
    }
    if (flags & RSF_ALPHATEST) {
        pD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, TRUE);
        pD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHAREF, 8);
        pD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHAFUNC, D3DCMP_LESSEQUAL);
    }
    if (flags & RSF_CLIPPLANE)
        pD3DDevice->SetRenderState(D3DRENDERSTATE_CLIPPLANEENABLE, TRUE);
    if (flags & RSF_DITHER)
        pD3DDevice->SetRenderState(D3DRENDERSTATE_DITHERENABLE, TRUE);
    if (flags & RSF_LIGHTING)
        pD3DDevice->SetRenderState(D3DRENDERSTATE_LIGHTING, TRUE);
    if (flags & RSF_SPECULAR)
        pD3DDevice->SetRenderState(D3DRENDERSTATE_SPECULARENABLE, TRUE);
    if (flags & RSF_COLORVERTEX)
        pD3DDevice->SetRenderState(D3DRENDERSTATE_COLORVERTEX, TRUE);
    if (flags & RSF_CULL_CW)
        pD3DDevice->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_CW);
    if (flags & RSF_CULL_CCW)
        pD3DDevice->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_CCW);
    if (flags & RSF_ZTEST)
        pD3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, D3DZB_TRUE);
    if (flags & RSF_ZWRITE_ON)
        if (pD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, TRUE))
            pD3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, D3DZB_TRUE);
    if (flags & RSF_ZWRITE_OFF)
        if (pD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, FALSE))
            pD3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, D3DZB_TRUE);
    if (flags & RSF_TEXTURED) {
        pD3DDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
        pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
        pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
        pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    } else {
        pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
        pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    }
    if (flags & RSF_FILTER_LINEAR) {
        pD3DDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_LINEAR);
        pD3DDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFN_LINEAR);
    }
    if (flags & RSF_FOG)
        pD3DDevice->SetRenderState(D3DRENDERSTATE_FOGENABLE, TRUE);
}

/* 0x5251a6 - one Gouraud-shaded untextured triangle, drawn at once with the given render-state flags. */
void Draw2D_GouraudTri_Immediate(float z, float x0, float y0, u32 c0, float x1, float y1, u32 c1, float x2, float y2,
                                 u32 c2, u32 renderFlags)
{
    float rhw = 1.0f / (g_pViewFrustum->nearZ); /* the parentheses load the divisor first (fld / fdivr) */
    g_draw2dImmVerts[0].x = x0;
    g_draw2dImmVerts[0].y = y0;
    g_draw2dImmVerts[0].z = z;
    g_draw2dImmVerts[0].rhw = rhw;
    g_draw2dImmVerts[0].diffuse = c0;
    g_draw2dImmVerts[0].specular = 0xff000000;
    g_draw2dImmVerts[1].x = x1;
    g_draw2dImmVerts[1].y = y1;
    g_draw2dImmVerts[1].z = z;
    g_draw2dImmVerts[1].rhw = rhw;
    g_draw2dImmVerts[1].diffuse = c1;
    g_draw2dImmVerts[1].specular = 0xff000000;
    g_draw2dImmVerts[2].x = x2;
    g_draw2dImmVerts[2].y = y2;
    g_draw2dImmVerts[2].z = z;
    g_draw2dImmVerts[2].rhw = rhw;
    g_draw2dImmVerts[2].diffuse = c2;
    g_draw2dImmVerts[2].specular = 0xff000000;
    g_pD3DAppMain->ApplyStateFlags(renderFlags);
    g_pD3DAppMain->DrawTLTriangles(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR, g_draw2dImmVerts, 3);
    g_pD3DAppMain->Render_ClearStateFlags(renderFlags);
}

/* 0x525657 - a Gouraud-shaded untextured rectangle: (TL, TR, BR), then vertex 1 becomes the bottom-left corner. */
void Draw2D_GouraudRect_Immediate(float z, float x0, float y0, float x1, float y1, u32 cTL, u32 cBL, u32 cTR, u32 cBR,
                                  u32 renderFlags)
{
    float rhw = 1.0f / (g_pViewFrustum->nearZ); /* the parentheses load the divisor first (fld / fdivr) */
    g_draw2dImmVerts[0].x = x0;
    g_draw2dImmVerts[0].y = y0;
    g_draw2dImmVerts[0].z = z;
    g_draw2dImmVerts[0].rhw = rhw;
    g_draw2dImmVerts[0].diffuse = cTL;
    g_draw2dImmVerts[0].specular = 0xff000000;
    g_draw2dImmVerts[1].x = x1;
    g_draw2dImmVerts[1].y = y0;
    g_draw2dImmVerts[1].z = z;
    g_draw2dImmVerts[1].rhw = rhw;
    g_draw2dImmVerts[1].diffuse = cTR;
    g_draw2dImmVerts[1].specular = 0xff000000;
    g_draw2dImmVerts[2].x = x1;
    g_draw2dImmVerts[2].y = y1;
    g_draw2dImmVerts[2].z = z;
    g_draw2dImmVerts[2].rhw = rhw;
    g_draw2dImmVerts[2].diffuse = cBR;
    g_draw2dImmVerts[2].specular = 0xff000000;
    g_pD3DAppMain->ApplyStateFlags(renderFlags);
    g_pD3DAppMain->DrawTLTriangles(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR, g_draw2dImmVerts, 3);
    g_draw2dImmVerts[1].x = x0;
    g_draw2dImmVerts[1].y = y1;
    g_draw2dImmVerts[1].diffuse = cBL;
    g_pD3DAppMain->DrawTLTriangles(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR, g_draw2dImmVerts, 3);
    g_pD3DAppMain->Render_ClearStateFlags(renderFlags);
}

/* 0x525b56 - one textured, Gouraud-shaded triangle: binds tex to stage 0 and leaves the batcher's texture binding
 * marked stale, since the direct SetTexture bypassed it. */
void Draw2D_TexTri_Immediate(float z, float x0, float y0, float x1, float y1, float x2, float y2, u32 renderFlags,
                             Texture *tex, float u0, float v0, u32 c0, float u1, float v1, u32 c1, float u2, float v2,
                             u32 c2)
{
    float rhw = 1.0f / (g_pViewFrustum->nearZ); /* the parentheses load the divisor first (fld / fdivr) */
    g_draw2dImmTexCorners[0].x = x0;
    g_draw2dImmTexCorners[0].y = y0;
    g_draw2dImmTexCorners[0].z = z;
    g_draw2dImmTexCorners[0].rhw = rhw;
    g_draw2dImmTexCorners[0].diffuse = c0;
    g_draw2dImmTexCorners[0].specular = 0xff000000;
    g_draw2dImmTexCorners[1].x = x1;
    g_draw2dImmTexCorners[1].y = y1;
    g_draw2dImmTexCorners[1].z = z;
    g_draw2dImmTexCorners[1].rhw = rhw;
    g_draw2dImmTexCorners[1].diffuse = c1;
    g_draw2dImmTexCorners[1].specular = 0xff000000;
    g_draw2dImmTexCorners[2].x = x2;
    g_draw2dImmTexCorners[2].y = y2;
    g_draw2dImmTexCorners[2].z = z;
    g_draw2dImmTexCorners[2].rhw = rhw;
    g_draw2dImmTexCorners[2].diffuse = c2;
    g_draw2dImmTexCorners[2].specular = 0xff000000;
    g_draw2dImmTexCorners[0].u = u0;
    g_draw2dImmTexCorners[0].v = v0;
    g_draw2dImmTexCorners[1].u = u1;
    g_draw2dImmTexCorners[1].v = v1;
    g_draw2dImmTexCorners[2].u = u2;
    g_draw2dImmTexCorners[2].v = v2;
    g_pD3DAppMain->BindTexture(tex, 0);
    g_pD3DAppMain->ApplyStateFlags(renderFlags);
    g_pD3DAppMain->DrawTLTriangles(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1,
                                   g_draw2dImmTexCorners, 3);
    g_pD3DAppMain->Render_ClearStateFlags(renderFlags);
    g_pPolyBin->InvalidateTexture();
}

/* 0x52607b - a textured rectangle: (TL, TR, BR), then vertex 1 becomes the bottom-left corner. Only its position and
 * uv are rewritten: cBL is never read, so the bottom-left corner keeps the top-right colour. */
void Draw2D_TexRect_Immediate(float z, float x0, float y0, float x1, float y1, u32 renderFlags, Texture *texture,
                              float uTL, float vTL, u32 cTL, float uBL, float vBL, u32 cBL, float uTR, float vTR,
                              u32 cTR, float uBR, float vBR, u32 cBR)
{
    float rhw = 1.0f / (g_pViewFrustum->nearZ); /* the parentheses load the divisor first (fld / fdivr) */
    g_draw2dImmTexCorners[0].x = x0;
    g_draw2dImmTexCorners[0].y = y0;
    g_draw2dImmTexCorners[0].z = z;
    g_draw2dImmTexCorners[0].rhw = rhw;
    g_draw2dImmTexCorners[0].diffuse = cTL;
    g_draw2dImmTexCorners[0].specular = 0xff000000;
    g_draw2dImmTexCorners[1].x = x1;
    g_draw2dImmTexCorners[1].y = y0;
    g_draw2dImmTexCorners[1].z = z;
    g_draw2dImmTexCorners[1].rhw = rhw;
    g_draw2dImmTexCorners[1].diffuse = cTR;
    g_draw2dImmTexCorners[1].specular = 0xff000000;
    g_draw2dImmTexCorners[2].x = x1;
    g_draw2dImmTexCorners[2].y = y1;
    g_draw2dImmTexCorners[2].z = z;
    g_draw2dImmTexCorners[2].rhw = rhw;
    g_draw2dImmTexCorners[2].diffuse = cBR;
    g_draw2dImmTexCorners[2].specular = 0xff000000;
    g_draw2dImmTexCorners[0].u = uTL;
    g_draw2dImmTexCorners[0].v = vTL;
    g_draw2dImmTexCorners[1].u = uTR;
    g_draw2dImmTexCorners[1].v = vTR;
    g_draw2dImmTexCorners[2].u = uBR;
    g_draw2dImmTexCorners[2].v = vBR;
    g_pD3DAppMain->BindTexture(texture, 0);
    g_pD3DAppMain->ApplyStateFlags(renderFlags);
    g_pD3DAppMain->DrawTLTriangles(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1,
                                   g_draw2dImmTexCorners, 3);
    g_draw2dImmTexCorners[1].x = x0;
    g_draw2dImmTexCorners[1].y = y1;
    g_draw2dImmTexCorners[1].u = uBL;
    g_draw2dImmTexCorners[1].v = vBL;
    g_pD3DAppMain->DrawTLTriangles(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1,
                                   g_draw2dImmTexCorners, 3);
    g_pD3DAppMain->Render_ClearStateFlags(renderFlags);
    g_pPolyBin->InvalidateTexture();
}
