/* match-init: Shadow_StaticInit 0x473b8e 0x473bd1 */
/* match-addr: g_splashVertsRear=0x6ccd38 g_splashFrontCount=0x6cde2c g_splashSpareBlock=0x6cde30 g_splashFrontVertices=0x6cde48   (the globals renamed for the .bss hash order, see below) */
/* The init roots above, in definition order: g_shadowPoly's (Shadow_StaticInit), then the two SplashVertex arrays'
 * (Splash_StaticInit_VertsA / _VertsB, table names for g_splashFrontVertices / g_splashVertsRear). */
/* BYTES: bss-name, inline, layout, slot-group, slot-name, slot-scope, temp, view. */
/* BYTES(bss-name): named for its .bss hash key 17 */
/* BYTES(bss-name): named for its .bss hash key 228 */
/* BYTES(bss-name): named for its .bss hash key 334 */
/* BYTES(bss-name): placeholder: 24 unreferenced bytes, named for its .bss hash key 287 */
/* BYTES(layout): written '= 0' only to keep definition order after the hashed globals */
/* BYTES(view): D3DApp::Render_SetStateFlags(s32) (inline twin overload): s32 overload, inline twin of Render_SetStateFlags 0x4155f0: the constant 0x1000 picks this expansion, u32 flag words pick the call */
/* BYTES(inline): D3DApp::CreateVB / SetTransform / GetDevice / DrawTriangleList / DrawTexturedTriangleList (member-macro inlines): source-only inline: its expansion gives the original's temporaries */
/* BYTES(inline): SplashVertex() {}: empty user-declared constructor: the array initialiser loop at 0x473b98 has no call in its body */
/*
 * T090 - the original object Shadow.cpp (guessed name), one file.
 * .text 0x472380-0x474d8e (ObjGrid_QueryGroundY .. Splash_Render), .rdata 0x575040-0x575050 (four float constants,
 * all of Splash_BuildRing's), no .data, .bss 0x6ccd38-0x6cef58; static initialisers .CRT$XCU 0x579014-0x57901c.
 * The blob shadows (query, re-projection, init, level load, render) and the splash block (0x473b8e-0x474d8d, up to
 * Splash_Render). The Shark class is the next object, T091.
 *
 * .bss: VC6 emits a file's uninitialised globals (constructed ones included) in the order of a hash of their names
 * (h = (h<<2) + (h>>4) + c over the name, then (h ^ h>>16) & 1023, ascending; last declared first within a bucket),
 * then its explicitly zero-initialised globals in definition order. The original order here interleaves the two
 * SplashVertex arrays and g_shadowPoly (all constructed, so all hashed) with plain globals, so every global up to
 * g_shadowPoly must hash in address order: g_splashVertsRear (17), g_splashCountB (34), g_shadowSprite (173),
 * g_splashFrontCount (228), g_splashSpareBlock (287, the unreferenced 24 bytes at 0x6cde30), g_splashFrontVertices
 * (334), g_shadowPoly (387). The front-facing triangle array is the "front" one, the rear-facing one the "rear" / B one
 * (Splash_BuildStrips). The four vertex-buffer pointers after g_shadowPoly are `= 0`, which puts them after the hashed
 * ones in definition order.
 */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
#include "../sdk/win32.h"
#include "../sdk/d3d7.h"
#include "../sdk/ddraw.h"
#include "../sdk/crt.h"
struct D3DXyzrhwVertex { /* a D3DFVF_XYZRHW vertex, as ProcessVertices writes it */
    float x, y, z, rhw;
};
class Mat44;
#define IsEqualGUID(a, b) (!memcmp((a), (b), 16)) /* sizeof(GUID); the type is incomplete here */
/* Shadow_Render's vertex views */
struct FlameVertex {
    float x, y, z, rhw;
    u32 diffuse, specular;
    float u, v;
};
struct FlatVertex {
    float x, y, z, rhw;
    u32 diffuse, specular;
};
struct XformedVertex {
    float x, y, z, rhw;
};
/* Member declarations, the union of the five files'. */

#define SDW_MEMBERS_Shadow                 \
    inline s32 NeedsProject();             \
    s32 NeedsDraw()                        \
    {                                      \
        return !(flags & SHADOW_F_HIDDEN); \
    }
#define SDW_MEMBERS_RenderPoly RenderPoly(); /* virtual ~RenderPoly() comes from the generated class header */
#define SDW_MEMBERS_PolyBatcher          \
    void SubmitPolyInline(RenderPoly *); \
    void SubmitPolyAgain(RenderPoly *);
#define SDW_MEMBERS_Mat44 Mat44(); /* 0x4077f0 */
#define SDW_MEMBERS_Vec3f \
    Vec3f() {}
/* D3DApp: Shadow_Render's batcher calls Render_SetStateFlags 0x4155f0 (u32 flags), while Splash_Render expands the
 * same body inline with flags = 0x1000; the inline twin takes s32 so the two can live in one class (the constant
 * argument picks it, the u32 flag words pick the call). */
#define SDW_MEMBERS_D3DApp                                                                                  \
    void CreateVB(D3DVERTEXBUFFERDESC *desc, IDirect3DVertexBuffer7 **out);                                 \
    void SetTransform(u32 state, Mat44 *m);                                                                 \
    IDirect3DDevice7 *GetDevice();                                                                          \
    void DrawTriangleList(void *verts, s32 count);                                                          \
    void DrawTexturedTriangleList(void *vertices, s32 count);                                               \
    void Render_SetTexture(Texture *texture, volatile u32 stage);                                           \
    void Render_DrawPrimitive(u32, u32, void *, u32);                                                       \
    void Render_SetStateFlags(u32);         /* 0x4155f0 */                                                  \
    void Render_ClearStateFlags(u32 flags); /* 0x4159b0 */                                                  \
    /* inline twin of Render_SetStateFlags 0x4155f0: the whole body is expanded here with flags = 0x1000 */ \
    void Render_SetStateFlags(s32 flags)                                                                    \
    {                                                                                                       \
        if (flags & RSF_ANTIALIAS)                                                                          \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_ANTIALIAS, D3DANTIALIAS_SORTINDEPENDENT);             \
        if (flags & RSF_BLEND_ALPHA) {                                                                      \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);                              \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_INVSRCALPHA);                      \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_SRCALPHA);                        \
        }                                                                                                   \
        if (flags & RSF_BLEND_ADD) {                                                                        \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);                              \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_INVSRCALPHA);                      \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);                             \
        }                                                                                                   \
        if (flags & RSF_ALPHATEST) {                                                                        \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, TRUE);                               \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHAREF, 8);                                         \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHAFUNC, D3DCMP_LESSEQUAL);                         \
        }                                                                                                   \
        if (flags & RSF_CLIPPLANE)                                                                          \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_CLIPPLANEENABLE, TRUE);                               \
        if (flags & RSF_DITHER)                                                                             \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_DITHERENABLE, TRUE);                                  \
        if (flags & RSF_LIGHTING)                                                                           \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_LIGHTING, TRUE);                                      \
        if (flags & RSF_SPECULAR)                                                                           \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_SPECULARENABLE, TRUE);                                \
        if (flags & RSF_COLORVERTEX)                                                                        \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_COLORVERTEX, TRUE);                                   \
        if (flags & RSF_CULL_CW)                                                                            \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_CW);                                \
        if (flags & RSF_CULL_CCW)                                                                           \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_CCW);                               \
        if (flags & RSF_ZTEST)                                                                              \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, D3DZB_TRUE);                                 \
        if (flags & RSF_ZWRITE_ON)                                                                          \
            if (pD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, TRUE))                              \
                pD3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, TRUE);                                   \
        if (flags & RSF_ZWRITE_OFF)                                                                         \
            if (pD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, FALSE))                             \
                pD3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, TRUE);                                   \
        if (flags & RSF_TEXTURED) {                                                                         \
            pD3DDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);                                   \
            pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);                           \
            pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);                           \
            pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);                           \
        } else {                                                                                            \
            pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);                           \
            pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);                         \
        }                                                                                                   \
        if (flags & RSF_FILTER_LINEAR) {                                                                    \
            pD3DDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_LINEAR);                           \
            pD3DDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFN_LINEAR);                           \
        }                                                                                                   \
        if (flags & RSF_FOG)                                                                                \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_FOGENABLE, TRUE);                                     \
    }                                                                                                       \
    void ClearStateFlagsInline(u32 flags);
#include "sdw_classes.h"
#define SDW_INLINE_D3DAPP_CREATEVB_D3DVERTEXBUFFERDESC_IDIRECT3DVERTEXBUFFER7 1
#define SDW_INLINE_D3DAPP_SETTRANSFORM_U32_MAT44 1
#include "../app/d3dapp_inlines.h"
#undef SDW_INLINE_D3DAPP_CREATEVB_D3DVERTEXBUFFERDESC_IDIRECT3DVERTEXBUFFER7
#undef SDW_INLINE_D3DAPP_SETTRANSFORM_U32_MAT44
#define SDW_INLINE_D3DAPP_GETDEVICE 1
#include "../app/d3dapp_inlines.h"
#undef SDW_INLINE_D3DAPP_GETDEVICE
#define SDW_INLINE_D3DAPP_DRAWTRIANGLELIST_VOID_S32 1
#include "../app/d3dapp_inlines.h"
#undef SDW_INLINE_D3DAPP_DRAWTRIANGLELIST_VOID_S32
#define SDW_INLINE_D3DAPP_DRAWTEXTUREDTRIANGLELIST_VOID_S32 1
#include "../app/d3dapp_inlines.h"
#undef SDW_INLINE_D3DAPP_DRAWTEXTUREDTRIANGLELIST_VOID_S32
#define SDW_INLINE_D3DAPP_CLEARSTATEFLAGSINLINE_U32 1
#include "../app/d3dapp_inlines.h"
#undef SDW_INLINE_D3DAPP_CLEARSTATEFLAGSINLINE_U32

extern "C" {
void Vec3i_Cross(const Vec3i *a, const Vec3i *b, Vec3i *out); /* 0x526c46 (src/engine/fixed_math.cpp) */
}
#include "maths.h"
#include "obj_grid.h"
#include "scenaric.h"
#include "../app/app_main.h"
#include "draw2d.h"
#include "load_warmeshes.h"
extern "C" s16 Collide_GroundYRay(Vec3s *pos, Vec3s *outNormal, s16 minY); /* 0x51b5f3  static-mesh ground below pos */

extern u8 g_sharedScratch[]; /* 0x6d5468  shared collision scratch; Shadow_Update keeps a ShadowScratch there */

/* Inline helpers (no bodies in the exe; the same three as in game/scn_controllable.cpp). FlagsClear: `!` materialised as
 * neg/sbb/inc before the test (0x4724ad). GetClassId + Scenaric_ClassFlags: a u16 temp for the id, then a u32 temp for
 * the flags, both below the named locals (0x47247b, 0x47248c). */
/* BYTES(inline): source-only inline: ! materialised as neg / sbb / inc (0x4724ad); the id and the flags go through temps below the named locals (0x47247b, 0x47248c) */
#define SDW_INLINE_SCNOBJECT_FLAGSCLEAR_U32 1
#include "scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_FLAGSCLEAR_U32

#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#include "scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID

#define SDW_INLINE_FREE_SCENARIC_CLASSFLAGS_U16 1
#include "scenaric_inlines.h"
#undef SDW_INLINE_FREE_SCENARIC_CLASSFLAGS_U16

/* Flag 2 as a 0/1 value (neg/sbb/neg at 0x47256f), tested before Shadow_Update does anything. */
inline s32 Shadow::NeedsProject()
{
    return (flags & SHADOW_F_REPROJECT) != 0;
}
/* BYTES(temp): volatile by-value parameter: the original copies the stage to a stack temp (see engine/mesh.cpp) */
inline void D3DApp::Render_SetTexture(Texture *texture, volatile u32 stage)
{
    pD3DDevice->SetTexture(stage, texture->surface);
}
/* BYTES(inline, inferred): source-only inline twins of SubmitPoly 0x415c80: /Ob1 expands the helpers differently at each site; the nested blocks place batch / count after flags */
#define SDW_INLINE_POLYBATCHER_SUBMITPOLYINLINE_RENDERPOLY 1
#include "polybatcher_inlines.h"
#undef SDW_INLINE_POLYBATCHER_SUBMITPOLYINLINE_RENDERPOLY

inline void PolyBatcher::SubmitPolyAgain(RenderPoly *poly)
{
    /* cast kept (the vertex casts here): vertex memory is untyped; the poly's type decides which vertex struct it holds */
    switch (poly->type) {
        case RPOLY_OPAQUE:
            if (flatBatchCount <= batchCapacity) {
                /* cast kept: a locked vertex buffer is untyped; its vertex format gives this type */
                memcpy((FlatVertex *)flatBatchVerts + flatBatchCount * 3, poly->verts, 0x48);
                ++flatBatchCount;
            }
            if (flatBatchCount >= batchCapacity) {
                renderer->Render_SetStateFlags(stateFlags);
                renderer->DrawTriangleList(flatBatchVerts, batchCapacity * 3);
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
            ++sortedCount;
            break;
        case RPOLY_ADD:
            if (computeSortZ == 1)
                poly->sortZ = poly->verts[2] + poly->verts[8] + poly->verts[14];
            sortedPolys[sortedCount].Assign(poly);
            sortedList[sortedCount] = &sortedPolys[sortedCount];
            ++sortedCount;
            break;
        default:
            /* cast kept: a locked vertex buffer is untyped; its vertex format gives this type */
            FlameVertex *vertices = (FlameVertex *)poly->verts;
            u32 texture = (poly->type - RPOLY_TEXTURED_BASE) & ~RPOLY_F_8000;
            if (texture < immediateTexCount) {
                u32 *flags;
                {
                    /* cast kept: a locked vertex buffer is untyped; its vertex format gives this type */
                    FlameVertex *batch = (FlameVertex *)texBatchVerts[texture];
                    {
                        u32 *count = &texBatchCounts[texture];
                        flags = &texStateFlags[texture];
                        if (*count <= batchCapacity) {
                            memcpy(batch + *count * 3, vertices, 0x60);
                            ++*count;
                        }
                        if (*count >= batchCapacity) {
                            if (texture != lastTextureIndex || textureDirty == 1) {
                                renderer->Render_SetTexture(textures[texture], 0);
                                lastTextureIndex = texture;
                                textureDirty = 0;
                            }
                            renderer->Render_SetStateFlags(*flags);
                            renderer->Render_DrawPrimitive(
                                D3DPT_TRIANGLELIST, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1,
                                batch, batchCapacity * 3);
                            renderer->Render_ClearStateFlags(*flags);
                            *count = 0;
                        }
                    }
                }
            } else {
                if (computeSortZ == 1)
                    poly->sortZ = vertices[0].z + vertices[1].z + vertices[2].z;
                sortedPolys[sortedCount].Assign(poly);
                sortedList[sortedCount] = &sortedPolys[sortedCount];
                ++sortedCount;
            }
    }
}

/* ---- .bss, the plain globals (see the header note on their order) ---- */
s32 g_splashCountB;     /* 0x6cde18  vertices written to g_splashVertsRear */
Sprite g_shadowSprite;  /* 0x6cde1c  the blob sprite, loaded from res type 0x21 */
s32 g_splashFrontCount; /* 0x6cde2c  vertices written to g_splashFrontVertices */
/* 0x6cde30 - 24 bytes that nothing in the image refers to (no code reference, no pointer): opaque, kept as bytes */
u8 g_splashSpareBlock[24];
IDirect3DVertexBuffer7 *g_shadowVB = 0;   /* 0x6cef48  4-vertex XYZ buffer */
IDirect3DVertexBuffer7 *g_shadowVBTL = 0; /* 0x6cef4c  4-vertex XYZRHW buffer (ProcessVertices target) */
IDirect3DVertexBuffer7 *g_splashVb = 0;   /* 0x6cef50  D3DFVF_XYZ, 32 vertices */
IDirect3DVertexBuffer7 *g_splashVbXf = 0; /* 0x6cef54  D3DFVF_XYZRHW, 32 vertices */

/*
 * Blob shadows: the object-top ground query and the shadow re-projection (SheepD3D.exe 0x472380 and 0x47255f), then
 * Shadow_Render 0x472904 and its neighbours. By address this is its own small file between the Sam/Daffy classes and
 * the Wolf.
 *
 * The `w` structs only pin the original stack offsets of locals (VC6 /Od hands out slots by a hash of the local names,
 * src/README.md); they are not a claim about the source text. Neither are the inline helpers' names.
 * Coordinates: x, y (points DOWN, so a smaller y is higher), z. Normals: 4.12, up = (0, -4096, 0).
 */
/* 0x472380 - the highest object top below minY (y >= minY is below) near pos: every object registered in the grid
 * cells covering pos +-500 in X and Z whose class has flag 0x40, that is not `exclude`, has neither object flag
 * 0x800 nor 4 and stands at or below minY, is asked message 0xd (ground query, GroundQuery argument); the smallest
 * answered height > minY wins and its normal goes to outNormal. 32000 = nothing found (outNormal untouched). */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; unused04 fills a gap */
s16 ObjGrid_QueryGroundY(Vec3s *pos, Vec3s *outNormal, s16 minY, ScnObject *exclude)
{
    struct {
        ListNode **list;
        ListNode *node;
        ListNode ***cell;
        s16 cellsX, cellsZ, cellX, cellZ;
        ListNode ***row;
        ScnObject *object;
        GroundQuery query;
        s16 unused04, best;
    } w;
    w.query.pos = *pos;
    w.best = 32000;
    ObjGrid_CellFromXZ(pos->x - 500, pos->z - 500, &w.cellX, &w.cellZ);
    ObjGrid_CellFromXZ(pos->x + 500, pos->z + 500, &w.cellsX, &w.cellsZ);
    w.cellsZ -= w.cellZ;
    w.cellsX -= w.cellX;
    w.row = &g_objGridCells[w.cellX + w.cellZ * g_objGridDimX];
    while (w.cellsZ >= 0) {
        w.cell = w.row;
        w.cellX = w.cellsX;
        while (w.cellX >= 0) {
            w.list = *w.cell;
            w.node = *w.list;
            while (w.node) {
                w.object = (ScnObject *)w.node->data; /* cast kept: a list node carries its payload as void * */
                if ((Scenaric_ClassFlags(w.object->GetClassId()) & SCN_CF_GROUND_PROVIDER) && w.object != exclude &&
                    w.object->FlagsClear(SCN_OF_HIDDEN2 | SCN_OF_HIDDEN) && w.object->pos.y >= minY) {
                    if (w.object->HandleMessage(exclude, MSG_GROUND_QUERY, &w.query) && w.best > w.query.pos.y &&
                        w.query.pos.y > minY) {
                        *outNormal = w.query.normal;
                        w.best = w.query.pos.y;
                    }
                }
                w.node = w.node->next;
            }
            ++w.cell;
            --w.cellX;
        }
        w.row += g_objGridDimX;
        --w.cellsZ;
    }
    return w.best;
}

/* 0x47255f - re-projects a blob shadow onto the ground under pos, only when its flag 2 is set (and clears it): ground =
 * the higher of the static mesh (Collide_GroundYRay) and the object tops (ObjGrid_QueryGroundY, owner excluded), both
 * searched from 10 units above pos. The quad is spanned by tangent = (n.y, -n.x, 0) scaled to the shadow radius and
 * bitangent = n x tangent; the blob fades with the drop, alpha = 128 - min(drop / 4, 96). */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; the unusedNN members fill gaps */
void Shadow_Update(Shadow *shadow, Vec3s *pos, ScnObject *owner)
{
    struct {
        ShadowScratch *scratch;
        s32 fade;
        Vec3s *corners;
        Vec3s objectNormal;
        s16 unused16, unused14, objectHeight;
        Vec3s point;
        s16 unused0A;
        Vec3s normal;
        s16 unused02;
    } w;
    if (!shadow->NeedsProject())
        return;
    shadow->flags &= (u8)~SHADOW_F_REPROJECT;
    w.point.x = pos->x;
    w.point.z = pos->z;
    w.point.y = Collide_GroundYRay(&w.point, &w.normal, pos->y - 10);
    w.objectHeight = ObjGrid_QueryGroundY(pos, &w.objectNormal, pos->y - 10, owner);
    if (w.objectHeight < w.point.y) {
        w.point.y = w.objectHeight;
        w.normal = w.objectNormal;
    }
    /* cast kept: each user lays out the shared scratch buffer its own way */
    w.scratch = (ShadowScratch *)g_sharedScratch;
    w.scratch->tangent.x = w.normal.y;
    w.scratch->tangent.y = -w.normal.x;
    w.scratch->tangent.z = 0;
    w.scratch->normal.x = w.normal.x;
    w.scratch->normal.y = w.normal.y;
    w.scratch->normal.z = w.normal.z;
    Vec3i_SetLength(&w.scratch->tangent.x, shadow->radius);
    Vec3i_Cross(&w.scratch->normal, &w.scratch->tangent, &w.scratch->bitangent);
    shadow->groundPos.x = w.point.x;
    shadow->groundPos.y = w.point.y;
    shadow->groundPos.z = w.point.z;
    w.corners = shadow->corners;
    w.corners[0].x = w.scratch->tangent.x + w.scratch->bitangent.x;
    w.corners[0].y = w.scratch->tangent.y + w.scratch->bitangent.y;
    w.corners[0].z = w.scratch->tangent.z + w.scratch->bitangent.z;
    w.corners[1].x = w.scratch->tangent.x - w.scratch->bitangent.x;
    w.corners[1].y = w.scratch->tangent.y - w.scratch->bitangent.y;
    w.corners[1].z = w.scratch->tangent.z - w.scratch->bitangent.z;
    w.fade = (w.point.y - pos->y) >> 2;
    if (w.fade > 96)
        w.fade = 96;
    shadow->alpha = 128 - w.fade;
}

/* PAL PC blob-shadow initialization, 0x472745-0x4727ae. */
void Shadow_Init(Shadow *shadow, u8 radius)
{
    shadow->radius = radius;
    shadow->flags = 0;
    shadow->flags |= SHADOW_F_REPROJECT;
}
void Shadow_FreeLevel() {}
/* 0x6cef28 - its initialisers 0x47276f-0x4727ae follow Shadow_FreeLevel */
RenderPoly g_shadowPoly;

/* PAL PC shadow resources, 0x4727ae. The DirectX interface declarations are those of the particle vertex buffers
 * (src/engine/emitter.cpp); the game layouts come from the generated headers. */
void Shadow_LoadLevel()
{
    D3DVERTEXBUFFERDESC desc;
    g_shadowSprite.LoadFromRes(DAV_IDI_IGLOMBR1);
    if (!g_shadowVB) {
        memset(&desc, 0, sizeof(desc));
        desc.dwSize = sizeof(desc);
        desc.dwCaps = D3DVBCAPS_DONOTCLIP;
        desc.dwFVF = D3DFVF_XYZ;
        desc.dwNumVertices = 4;
        g_pD3DAppMain->CreateVB(&desc, &g_shadowVB);
    }
    if (!g_shadowVBTL) {
        memset(&desc, 0, sizeof(desc));
        desc.dwSize = sizeof(desc);
        desc.dwCaps = D3DVBCAPS_DONOTCLIP;
        desc.dwFVF = D3DFVF_XYZRHW;
        desc.dwNumVertices = 4;
        g_pD3DAppMain->CreateVB(&desc, &g_shadowVBTL);
    }
}

/* 0x472904 - draws a blob shadow: the quad around groundPos is transformed by D3D (ProcessVertices) and submitted as two
 * triangles textured with the blob sprite, faded by the shadow's alpha. */
/* BYTES(slot-scope): the nested blocks only order the frame: each block's local is allocated after the enclosing ones */
void Shadow_Render(Shadow *shadow)
{
    FlameVertex *poly;
    {
        void *locked;
        {
            Mat44 world;
            {
                Vec3f *points;
                {
                    XformedVertex *transformed;
                    {
                        u32 color;
                        {
                            Camera *camera = &g_camera;
                            {
                                u32 lockSize;
                                if (!shadow->NeedsDraw())
                                    return;
                                /* cast kept (the three vertex views below): vertex memory is untyped; the buffers hold
                                 * D3DFVF_XYZ and D3DFVF_XYZRHW vertices, the poly FVF 0x1c4 ones */
                                g_shadowVB->Lock(DDLOCK_WAIT, &locked, &lockSize);
                                points = (Vec3f *)locked;
                                points[0].x = (float)(shadow->groundPos.x - shadow->corners[0].x);
                                points[0].y = (float)(shadow->groundPos.y - shadow->corners[0].y);
                                points[0].z = (float)(shadow->groundPos.z - shadow->corners[0].z);
                                points[1].x = (float)(shadow->groundPos.x + shadow->corners[0].x);
                                points[1].y = (float)(shadow->groundPos.y + shadow->corners[0].y);
                                points[1].z = (float)(shadow->groundPos.z + shadow->corners[0].z);
                                points[2].x = (float)(shadow->groundPos.x + shadow->corners[1].x);
                                points[2].y = (float)(shadow->groundPos.y + shadow->corners[1].y);
                                points[2].z = (float)(shadow->groundPos.z + shadow->corners[1].z);
                                points[3].x = (float)(shadow->groundPos.x - shadow->corners[1].x);
                                points[3].y = (float)(shadow->groundPos.y - shadow->corners[1].y);
                                points[3].z = (float)(shadow->groundPos.z - shadow->corners[1].z);

                                g_shadowVB->Unlock();
                                world.SetIdentity();
                                g_pD3DAppMain->SetTransform(D3DTRANSFORMSTATE_WORLD, &world);
                                g_pD3DAppMain->SetTransform(D3DTRANSFORMSTATE_VIEW, &camera->viewMat);
                                g_shadowVBTL->ProcessVertices(D3DVOP_TRANSFORM, 0, 4, g_shadowVB, 0,
                                                              g_pD3DAppMain->GetDevice(), D3DPV_DONOTCOPYDATA);
                                g_shadowVBTL->Lock(DDLOCK_WAIT, &locked, &lockSize);
                                /* cast kept: a locked vertex buffer is untyped; its vertex format gives this type */
                                transformed = (XformedVertex *)locked;
                                poly = (FlameVertex *)g_shadowPoly.verts;
                                g_shadowPoly.type = g_shadowSprite.texPage + RPOLY_TEXTURED_BASE;
                                color = shadow->alpha | (shadow->alpha << 8) | (shadow->alpha << 16);
                                poly[0].diffuse = poly[1].diffuse = poly[2].diffuse = color;
                                poly[0].specular = poly[1].specular = poly[2].specular = 0xff000000;
                                memcpy(poly, transformed, 16);
                                memcpy(poly + 1, transformed + 2, 16);
                                memcpy(poly + 2, transformed + 1, 16);
                                poly[0].z -= (1.0f - poly[0].z) * 1.0f * (1.0f - poly[0].z);
                                poly[1].z -= (1.0f - poly[1].z) * 1.0f * (1.0f - poly[1].z);
                                poly[2].z -= (1.0f - poly[2].z) * 1.0f * (1.0f - poly[2].z);
                                poly[0].u = Tex_CornerUV(0, g_shadowSprite.widthMinus1, g_shadowSprite.u);
                                poly[0].v =
                                    Tex_CornerUV(g_shadowSprite.height, g_shadowSprite.height, g_shadowSprite.v);
                                poly[1].u = Tex_CornerUV(0, g_shadowSprite.widthMinus1, g_shadowSprite.u);
                                poly[1].v = Tex_CornerUV(0, g_shadowSprite.height, g_shadowSprite.v);
                                poly[2].u = Tex_CornerUV(g_shadowSprite.widthMinus1, g_shadowSprite.widthMinus1,
                                                         g_shadowSprite.u);
                                poly[2].v = Tex_CornerUV(0, g_shadowSprite.height, g_shadowSprite.v);
                                g_pPolyBin->SubmitPolyInline(&g_shadowPoly);
                                memcpy(poly + 1, transformed + 1, 16);
                                memcpy(poly + 2, transformed + 3, 16);
                                poly[0].z -= (1.0f - poly[0].z) * 1.0f * (1.0f - poly[0].z);
                                poly[1].z -= (1.0f - poly[1].z) * 1.0f * (1.0f - poly[1].z);
                                poly[2].z -= (1.0f - poly[2].z) * 1.0f * (1.0f - poly[2].z);
                                poly[1].u = Tex_CornerUV(g_shadowSprite.widthMinus1, g_shadowSprite.widthMinus1,
                                                         g_shadowSprite.u);
                                poly[1].v = Tex_CornerUV(0, g_shadowSprite.height, g_shadowSprite.v);
                                poly[2].u = Tex_CornerUV(g_shadowSprite.widthMinus1, g_shadowSprite.widthMinus1,
                                                         g_shadowSprite.u);
                                poly[2].v =
                                    Tex_CornerUV(g_shadowSprite.height, g_shadowSprite.height, g_shadowSprite.v);
                                g_pPolyBin->SubmitPolyAgain(&g_shadowPoly);
                                g_shadowVBTL->Unlock();
                            }
                        }
                    }
                }
            }
        }
    }
}

/* ---- the splash block, 0x473b8e-0x474d8d ----
 * Nothing in the exe calls Splash_CreateVertexBuffers or Splash_Render, and the only callers of the two builders are
 * Splash_Render itself: a cut effect the linker kept because it is in this object. It builds a ring of 4 x seg
 * points around a world position and draws it as two triangle strips out of two 180-vertex system-memory arrays;
 * seg is a camera-distance LOD, 8 within 500 units and 2 beyond 2000. The object map puts it in this object, not the
 * Shark's: its SplashVertex arrays are interleaved with the shadow globals in this object's .bss, and its initialisers
 * follow g_shadowPoly's in .CRT$XCU (0x473b8e joins Shadow_Render unaligned; 0x474d8e-0x474d8f is the int3 pad before
 * the Shark's object).
 */
/* The caller's record: a world position and the ring radius (only these two fields are read). */
struct SplashSource {
    Vec3s pos;
    u8 pad6;
    u8 radius;
};
struct SplashVertex {
    SplashVertex() {} /* empty: the array initialiser loop at 0x473b98 has no call in its body */
    float x, y, z, rhw;
    u32 color, specular;
};
SplashVertex g_splashFrontVertices[180]; /* 0x6cde48  the front-facing triangles (A) */
SplashVertex g_splashVertsRear[180];     /* 0x6ccd38  the rear-facing triangles (B) */

/* 0x473c14 - the two vertex buffers, made once. No caller. */
void Splash_CreateVertexBuffers()
{
    D3DVERTEXBUFFERDESC desc;
    u32 i;

    if (!g_splashVb) {
        memset(&desc, 0, sizeof(desc));
        desc.dwSize = sizeof(desc);
        desc.dwCaps = D3DVBCAPS_DONOTCLIP;
        desc.dwFVF = D3DFVF_XYZ;
        desc.dwNumVertices = 32;
        g_pD3DAppMain->CreateVB(&desc, &g_splashVb);
    }
    if (!g_splashVbXf) {
        memset(&desc, 0, sizeof(desc));
        desc.dwSize = sizeof(desc);
        desc.dwCaps = D3DVBCAPS_DONOTCLIP;
        desc.dwFVF = D3DFVF_XYZRHW;
        desc.dwNumVertices = 32;
        g_pD3DAppMain->CreateVB(&desc, &g_splashVbXf);
    }
    for (i = 0; i < 180; i++) {
        g_splashFrontVertices[i].color = 0xffffffff;
        g_splashFrontVertices[i].specular = 0xff000000;
        g_splashVertsRear[i].color = 0x80000000;
        g_splashVertsRear[i].specular = 0xff000000;
    }
}

/* 0x473dbb - fills the source vertex buffer with 4 x seg ring points of the given radius around p (two half-circles
 * 20 units below it, then the same pair 200 units lower) and returns seg. */
u8 Splash_BuildRing(const Vec3s *p, float radius, Camera *cam)
{
    void *locked;
    Mat44 mat;
    float dist2;
    Vec3s off;
    u8 i;
    Vec3f *vtx;
    u8 seg;
    u32 bytes;

    g_splashVb->Lock(DDLOCK_WAIT, &locked, &bytes);
    vtx = (Vec3f *)locked; /* cast kept: vertex memory is untyped; this buffer holds D3DFVF_XYZ vertices */
    off.x = cam->pos.x - p->x;
    off.y = cam->pos.y - p->y;
    off.z = cam->pos.z - p->z;
    dist2 = (float)(off.x * off.x + off.y * off.y + off.z * off.z);
    if (dist2 >= 4000000.0f)
        seg = 2;
    else if (dist2 <= 250000.0f)
        seg = 8;
    else
        seg = (u8)((u8)(s32)((dist2 - 250000.0f) * -1.6e-6f) + 8);
    mat.SetRotZXY(0.0f, 3.1415927f / seg, 0.0f);
    vtx[0].x = radius;
    vtx[0].y = -20.0f;
    vtx[0].z = 0.0f;
    vtx[seg].x = -radius;
    vtx[seg].y = -20.0f;
    vtx[seg].z = 0.0f;
    for (i = 1; i < seg; i++) {
        mat.TransformPoint(&vtx[i - 1], &vtx[i]);
        mat.TransformPoint(&vtx[i + seg - 1], &vtx[i + seg]);
    }
    memcpy(&vtx[seg * 2], vtx, seg * 12 * 2);
    for (i = seg * 2; i < seg * 4; i++)
        vtx[i].y += 200.0f;
    g_splashVb->Unlock();
    return seg;
}

/* 0x47400e - turns the 4 x seg ring points, already transformed to screen space, into two triangle strips and sorts
 * each triangle into the front array (A) or the back one (B) by a parity of four tests: three "z > 1" (behind the far
 * plane) and the sign of the 2-D cross product (the winding). g_splashFrontCount / _CountB count the vertices written.
 * The local names are chosen for their stack slots: anyB / aUsed are the "strip B / strip A has been started" latches,
 * dstB / ptrA the two write cursors and cur the current one. */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py) */
void Splash_BuildStrips(const D3DXyzrhwVertex *v, u8 seg)
{
    u8 aUsed;
    u8 anyB;
    u8 i;
    u8 span;
    SplashVertex *ptrA;
    u8 base;
    SplashVertex *dstB;
    SplashVertex *cur;

    anyB = 0;
    aUsed = 0;
    i = 0;
    base = 0;
    span = seg * 2 - 1;
    g_splashCountB = seg * 2 - 2;
    g_splashFrontCount = g_splashCountB;
    if (((v[base].z > 1.0f) ^ (v[base + 1].z > 1.0f) ^ (v[span].z > 1.0f) ^
         ((v[base + 1].x - v[base].x) * (v[span].y - v[base].y) -
              (v[base + 1].y - v[base].y) * (v[span].x - v[base].x) <=
          0.0f)) == 1)
        cur = g_splashFrontVertices;
    else
        cur = g_splashVertsRear;
    while (!(anyB && aUsed)) {
        if (cur == g_splashVertsRear)
            anyB = 1;
        else
            aUsed = 1;
        for (i = 1; i < seg - 1; i++) {
            memcpy(cur, &v[base + i], 16);
            memcpy(&cur[1], &v[base + i + 1], 16);
            memcpy(&cur[2], &v[base + (span - (i - 1))], 16);
            cur += 3;
            memcpy(cur, &v[base + i + 1], 16);
            memcpy(&cur[1], &v[base + (span - i)], 16);
            memcpy(&cur[2], &v[base + (span - (i - 1))], 16);
            cur += 3;
        }
        memcpy(cur, &v[base], 16);
        memcpy(&cur[1], &v[base + 1], 16);
        memcpy(&cur[2], &v[base + span], 16);
        cur += 3;
        memcpy(cur, &v[base + seg - 1], 16);
        memcpy(&cur[1], &v[base + seg], 16);
        memcpy(&cur[2], &v[base + seg + 1], 16);
        cur += 3;
        if (anyB == 1)
            cur = g_splashFrontVertices;
        else
            cur = g_splashVertsRear;
        base = seg * 2;
    }
    base = seg * 2 - 2;
    span = seg * 2;
    ptrA = &g_splashFrontVertices[base * 3];
    dstB = &g_splashVertsRear[base * 3];
    for (i = 0; i < seg * 2; i++) {
        if (((v[i].z > 1.0f) ^ (v[i + span].z > 1.0f) ^ (v[(i + 1) % (seg * 2)].z > 1.0f) ^
             ((v[i + span].x - v[i].x) * (v[(i + 1) % (seg * 2)].y - v[i].y) -
                  (v[i + span].y - v[i].y) * (v[(i + 1) % (seg * 2)].x - v[i].x) <=
              0.0f)) == 1) {
            cur = ptrA;
            ptrA += 6;
            g_splashFrontCount += 2;
        } else {
            cur = dstB;
            dstB += 6;
            g_splashCountB += 2;
        }
        memcpy(cur, &v[i], 16);
        memcpy(&cur[1], &v[i + span], 16);
        memcpy(&cur[2], &v[(i + 1) % (seg * 2)], 16);
        cur += 3;
        memcpy(cur, &v[i + span], 16);
        memcpy(&cur[1], &v[(i + 1) % (seg * 2)], 16);
        memcpy(&cur[2], &v[(i + 1) % (seg * 2) + span], 16);
    }
}

/* 0x4746c3 - the whole effect, and the only caller of the two builders: it places the ring at src->pos, transforms
 * it with the camera's view matrix through the hardware (ProcessVertices), sorts the triangles, and draws the two
 * arrays with different blend factors (A additive-ish 9/5, B 1/5). No caller in the exe. */
void Splash_Render(SplashSource *src)
{
    void *locked;
    Mat44 mat;
    Camera *cam;
    u8 n;
    u32 bytes;

    cam = &g_camera;
    n = Splash_BuildRing(&src->pos, (float)src->radius, cam);
    mat.SetTranslation((float)src->pos.x, (float)src->pos.y, (float)src->pos.z);
    g_pD3DAppMain->SetTransform(D3DTRANSFORMSTATE_WORLD, &mat);
    g_pD3DAppMain->SetTransform(D3DTRANSFORMSTATE_VIEW, &cam->viewMatCopy);
    g_splashVbXf->ProcessVertices(D3DVOP_TRANSFORM, 0, n * 4, g_splashVb, 0, g_pD3DAppMain->GetDevice(),
                                  D3DPV_DONOTCOPYDATA);
    g_splashVbXf->Lock(DDLOCK_WAIT, &locked, &bytes);
    Splash_BuildStrips((D3DXyzrhwVertex *)locked, n); /* cast kept: vertex memory is untyped; these are D3DFVF_XYZRHW */
    g_pD3DAppMain->Render_SetStateFlags(RSF_ZWRITE_OFF);
    g_pD3DAppMain->GetDevice()->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);
    g_pD3DAppMain->GetDevice()->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_DESTCOLOR);
    g_pD3DAppMain->GetDevice()->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_SRCALPHA);
    g_pD3DAppMain->DrawTriangleList(g_splashFrontVertices, g_splashFrontCount * 3);
    g_pD3DAppMain->GetDevice()->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ZERO);
    g_pD3DAppMain->GetDevice()->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_SRCALPHA);
    g_pD3DAppMain->DrawTriangleList(g_splashVertsRear, g_splashCountB * 3);
    g_pD3DAppMain->GetDevice()->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);
    g_pD3DAppMain->Render_ClearStateFlags(RSF_ZWRITE_OFF);
    g_splashVbXf->Unlock();
}
