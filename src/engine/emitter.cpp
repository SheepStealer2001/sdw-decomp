/* match-init: Sfx_StaticInit */
/*
 * T264 - guessed original name: Emitter.cpp. SheepD3D.exe .text 0x52a860-0x52dd3d, .data 0x57bc38-0x57bc50,
 * .bss 0x6d8078-0x6d82fc.
 * The particle emitters: the static RenderPoly every flat particle is pushed through (its four static-initialiser
 * thunks start the object), the two particle vertex buffers, the spawn, the update variants, render, and the effect
 * sprite sheets: the particle vertex buffers 0x52a860-0x52a9e8, then the emitters 0x52a9e9-0x52dd3c. The stubs,
 * cheats and countdown that follow are src/engine/emitter_stubs.cpp (T265) and src/engine/cheat.cpp (T266).
 *
 * .bss order. VC6 lays out an object's .bss as (1) the globals defined WITHOUT an initialiser (constructed objects
 * included), sorted by a hash of their names, key (h ^ h >> 16) & 0x3ff (tools/vc6_locals.py has the hash), then (2)
 * the globals explicitly initialised to zero, in definition order. g_particlePoly has a constructor, so it is in group
 * (1), and the sprite sheets come before it at 0x6d8078: they must be in group (1) with a smaller key. The name
 * g_spriteSheets (the descriptive name) has key 1006 (> g_particlePoly's 120), so the table is s_spriteSheets here,
 * key 24. Nothing outside this object refers to 0x6d8078-0x6d82b3, so the original could well have had a file-static
 * table next to s_effectSheetRes. The two vertex buffers are `= 0` (group 2), which puts them after g_particlePoly in
 * definition order.
 *
 * Compilation: /Od. The render path: Emitter_RenderFlat expands PolyBatcher::SubmitPoly (0x415c80) twice, and inside
 * the first expansion Render_SetStateFlags (0x4155f0), while every other Set / Clear stays a call. Writing SubmitPoly,
 * Render_SetStateFlags and Render_ClearStateFlags as the engine header's inline functions reproduces exactly those
 * /Ob1 choices; the out-of-line copies VC6 then emits of the two state functions are COMDATs whose selected copies
 * belong to an earlier object (0x4155f0 / 0x4159b0), so LINK discards these. The texture stage is an enum (an
 * enum-typed inline parameter gets a stack temp even for a constant, as in src/engine/interface.cpp). Local names are
 * chosen for their stack slots (tools/vc6_locals.py); the inline helpers (SetTransform, GetDevice, DrawTriangleList,
 * DrawTexTriangleList, SetTexture, CreateVB) have no bodies in the exe, so their names are not recovered. A shape that
 * reproduces the bytes is a representation, not proof that the original source read this way.
 *
 * An emitter (ParticleEmitter, and the InlineEmitterN / TrailEmitter variants that carry their pools inline) is a
 * ring of `count` particles: a 0xC-byte float position per slot (slotPool, memcpy'd straight into the D3DFVF_XYZ
 * vertex buffer when drawn) and an 8-byte Particle of bitfields {age:24, grey:8; angle:12, size:12, frame:7,
 * alive:1}. Each Emitter_Update* variant spawns through Sfx_SpawnBillboard (which interpolates the source position
 * between frames so a fast emitter leaves an even trail), then ages and moves every live particle, and returns
 * the emitter's active bit (any particle alive). Emitter_Render draws them as camera-facing quads through
 * Draw2D_TexRect; Emitter_RenderFlat as quads lying in the ground plane, rotated by the particle angle.
 */
/* BYTES: bss-name, cast, dead-code, inline, layout, slot-name, view. */
/* BYTES(bss-name): named for its .bss hash key 24 so it lands before g_particlePoly */
/* BYTES(layout): written '= 0' only to keep definition order in .bss */
/* BYTES(layout): placeholder: 32 unreferenced bytes kept only for the .bss size */
/* BYTES(view): spelled as the g_screen fields they are (macros), not separate globals */
/* BYTES(cast): TexStage enum parameter (D3DApp::SetTexture): the stage is enum-typed so the original's stack temp for the constant 0 appears (0x52bea0) */
/* BYTES(inline): D3DApp::SetTransform / GetDevice / DrawTriangleList / DrawTexTriangleList / CreateVB (member-macro inlines): source-only inline: the D3DApp pointer lands in a stack temp at each use */
/* BYTES(inline): D3DApp::Render_SetStateFlags / Render_ClearStateFlags (inline in this object): header inline: VC6 expands it where the /Ob1 budget allows; this object's out-of-line COMDAT is discarded for T013's */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
#include "../sdk/d3d7.h"
#include "../sdk/ddraw.h"
#include "../sdk/win32.h"

class Mat44;

#include "../sdk/crt.h"
#define IsEqualGUID(a, b) (!memcmp((a), (b), 16)) /* sizeof(GUID); the type is incomplete here */
struct D3DXyzrhwVertex {                          /* a D3DFVF_XYZRHW vertex, as ProcessVertices writes it */
    float x, y, z, rhw;
};
struct D3DTLVertex { /* a D3DTLVERTEX (FVF 0x1c4), the vertex of a textured RenderPoly */
    float x, y, z, rhw;
    u32 color, specular;
    float u, v;
};
class Texture;
/* A texture stage. An enum-typed inline parameter is copied to a stack temp even when bound to a constant, which
 * is what the original does with the stage (0x52bea0 mov [ebp-0x20c],0); the same device as
 * src/engine/interface.cpp. */
enum TexStage { TEX_STAGE_0 };

#define SDW_MEMBERS_Mat44 Mat44(); /* 0x4077f0 */
#define SDW_MEMBERS_D3DApp                                                                                            \
    void CreateVB(D3DVERTEXBUFFERDESC *desc, IDirect3DVertexBuffer7 **out);                                           \
    void SetTransform(u32 state, Mat44 *m);                                                                           \
    IDirect3DDevice7 *GetDevice();                                                                                    \
    void DrawTriangleList(void *verts, s32 count);                                                                    \
    void DrawTexTriangleList(void *verts, s32 count)                                                                  \
    {                                                                                                                 \
        if (count)                                                                                                    \
            pD3DDevice->DrawPrimitive(                                                                                \
                D3DPT_TRIANGLELIST, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1, verts, count, 0); \
    }                                                                                                                 \
    inline void SetTexture(Texture *tex, TexStage stage); /* defined below: it needs Texture */                       \
    /* 0x4155f0 / 0x4159b0, inline in the engine header: VC6 expands them where its budget allows */                  \
    void Render_SetStateFlags(u32 flags)                                                                              \
    {                                                                                                                 \
        if (flags & RSF_ANTIALIAS)                                                                                    \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_ANTIALIAS, D3DANTIALIAS_SORTINDEPENDENT);                       \
        if (flags & RSF_BLEND_ALPHA) {                                                                                \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);                                        \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_INVSRCALPHA);                                \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_SRCALPHA);                                  \
        }                                                                                                             \
        if (flags & RSF_BLEND_ADD) {                                                                                  \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);                                        \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_INVSRCALPHA);                                \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);                                       \
        }                                                                                                             \
        if (flags & RSF_ALPHATEST) {                                                                                  \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, TRUE);                                         \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHAREF, 8);                                                   \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHAFUNC, D3DCMP_LESSEQUAL);                                   \
        }                                                                                                             \
        if (flags & RSF_CLIPPLANE)                                                                                    \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_CLIPPLANEENABLE, TRUE);                                         \
        if (flags & RSF_DITHER)                                                                                       \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_DITHERENABLE, TRUE);                                            \
        if (flags & RSF_LIGHTING)                                                                                     \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_LIGHTING, TRUE);                                                \
        if (flags & RSF_SPECULAR)                                                                                     \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_SPECULARENABLE, TRUE);                                          \
        if (flags & RSF_COLORVERTEX)                                                                                  \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_COLORVERTEX, TRUE);                                             \
        if (flags & RSF_CULL_CW)                                                                                      \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_CW);                                          \
        if (flags & RSF_CULL_CCW)                                                                                     \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_CCW);                                         \
        if (flags & RSF_ZTEST)                                                                                        \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, D3DZB_TRUE);                                           \
        if (flags & RSF_ZWRITE_ON)                                                                                    \
            if (pD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, TRUE))                                        \
                pD3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, TRUE);                                             \
        if (flags & RSF_ZWRITE_OFF)                                                                                   \
            if (pD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, FALSE))                                       \
                pD3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, TRUE);                                             \
        if (flags & RSF_TEXTURED) {                                                                                   \
            pD3DDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);                                             \
            pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);                                     \
            pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);                                     \
            pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);                                     \
        } else {                                                                                                      \
            pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);                                     \
            pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);                                   \
        }                                                                                                             \
        if (flags & RSF_FILTER_LINEAR) {                                                                              \
            pD3DDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_LINEAR);                                     \
            pD3DDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFN_LINEAR);                                     \
        }                                                                                                             \
        if (flags & RSF_FOG)                                                                                          \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_FOGENABLE, TRUE);                                               \
    }                                                                                                                 \
    void Render_ClearStateFlags(u32 flags)                                                                            \
    {                                                                                                                 \
        if (flags & RSF_ANTIALIAS)                                                                                    \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_ANTIALIAS, D3DANTIALIAS_NONE);                                  \
        if ((flags & RSF_BLEND_ALPHA) || (flags & RSF_BLEND_ADD))                                                     \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);                                       \
        if (flags & RSF_ALPHATEST)                                                                                    \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, FALSE);                                        \
        if (flags & RSF_CLIPPLANE)                                                                                    \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_CLIPPLANEENABLE, FALSE);                                        \
        if (flags & RSF_DITHER)                                                                                       \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_DITHERENABLE, FALSE);                                           \
        if (flags & RSF_LIGHTING)                                                                                     \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_LIGHTING, FALSE);                                               \
        if (flags & RSF_SPECULAR)                                                                                     \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_SPECULARENABLE, FALSE);                                         \
        if (flags & RSF_COLORVERTEX)                                                                                  \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_COLORVERTEX, FALSE);                                            \
        if ((flags & RSF_CULL_CW) || (flags & RSF_CULL_CCW))                                                          \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE);                                        \
        if (flags & RSF_ZTEST)                                                                                        \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, D3DZB_FALSE);                                          \
        if (flags & RSF_ZWRITE_ON)                                                                                    \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, FALSE);                                           \
        if (flags & RSF_ZWRITE_OFF)                                                                                   \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, TRUE);                                            \
        if (flags & RSF_TEXTURED)                                                                                     \
            pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);                                      \
        if (flags & RSF_FILTER_LINEAR) {                                                                              \
            pD3DDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_POINT);                                      \
            pD3DDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFN_POINT);                                      \
        }                                                                                                             \
        if (flags & RSF_FOG)                                                                                          \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_FOGENABLE, FALSE);                                              \
    }
#define SDW_MEMBERS_RenderPoly RenderPoly(); /* 0x41aad0 RenderPoly_Ctor */
#define SDW_MEMBERS_PolyBatcher \
    inline void SubmitPoly(RenderPoly *poly); /* 0x415c80, inline in the engine header; defined below */
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

#include "fixed_math.h"
#include "load_warmeshes.h"
#include "draw2d.h"
#include "screen.h"
#include "scenaric.h"
s32 Rand_Bounded(s32 bound); /* 0x561219 */
void Draw2D_TexRect(float z, float x0, float y0, float x1, float y1, s32 texIndex, float uTL, float vTL, u32 cTL,
                    float uBL, float vBL, u32 cBL, float uTR, float vTR, u32 cTR, float uBR, float vBR,
                    u32 cBR);            /* 0x5242df */
void Sfx_CreateVertexBuffers(u32 count); /* 0x52a89f */

/* 0x415c80 PolyBatcher_SubmitPoly, as the engine header's inline: untextured triangles and the immediate texture
 * pages are batched (and flushed when a batch fills), everything else goes to the sorted list. */
#define SDW_INLINE_D3DAPP_SETTEXTURE_TEXTURE_TEXSTAGE 1
#include "../app/d3dapp_inlines.h"
#undef SDW_INLINE_D3DAPP_SETTEXTURE_TEXTURE_TEXSTAGE

/* BYTES(inline): header inline: Emitter_RenderFlat expands it twice, as the original */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py) */
inline void PolyBatcher::SubmitPoly(RenderPoly *poly)
{
    /* local names chosen for their stack slots (tools/vc6_locals.py) */
    D3DTLVertex *tri;
    u32 idx6;
    D3DTLVertex *batch;
    u32 *num;
    u32 *pflags;

    switch (poly->type) {
        case RPOLY_OPAQUE:
            if (flatBatchCount <= batchCapacity) {
                /* cast kept: the flat batch is raw vertex memory, 3 vertices of 0x18 bytes (FVF 0xc4) per triangle */
                memcpy((u8 *)flatBatchVerts + flatBatchCount * 3 * 0x18, poly->verts, 0x48);
                flatBatchCount++;
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
            /* cast kept: RenderPoly.verts is a float array; this poly's vertices are D3DTLVertex records */
            tri = (D3DTLVertex *)poly->verts;
            idx6 = poly->type - RPOLY_TEXTURED_BASE & ~RPOLY_F_8000;
            if (idx6 < immediateTexCount) {
                /* cast kept: a texture batch is untyped vertex memory of D3DTLVertex records */
                batch = (D3DTLVertex *)texBatchVerts[idx6];
                num = &texBatchCounts[idx6];
                pflags = &texStateFlags[idx6];
                if (*num <= batchCapacity) {
                    memcpy(&batch[*num * 3], tri, 0x60);
                    (*num)++;
                }
                if (*num >= batchCapacity) {
                    if (idx6 != lastTextureIndex || textureDirty == 1) {
                        renderer->SetTexture(textures[idx6], TEX_STAGE_0);
                        lastTextureIndex = idx6;
                        textureDirty = 0;
                    }
                    renderer->Render_SetStateFlags(*pflags);
                    renderer->DrawTexTriangleList(batch, batchCapacity * 3);
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

/* 0x6d6ff4 / 0x6d6ff6: g_screenW / g_screenH are not globals of their own but g_screen's viewportWidth /
 * viewportHeight (+0x14 / +0x16; g_screen is 0x6d6fe0, src/engine/screen.cpp, T262), so they are written as those
 * fields: a declaration-level alias that leaves the bodies untouched (the same device as g_collSegTriVerts in
 * src/engine/collide.cpp). */
#define g_screenW (g_screen.viewportWidth)
#define g_screenH (g_screen.viewportHeight)
extern s32 g_dt;   /* 0x71b300 */
extern s32 g_dtMs; /* 0x71b2e8 */

/* 0x6d8078 - the 11 effect sprite sheets (stride 0x34), loaded by Sfx_InitSpriteSheets from s_effectSheetRes. Today's
 * name elsewhere: g_spriteSheets (renamed for the .bss order, see the header). */
static AnimSprite s_spriteSheets[11];

/* ---- the particle vertex buffers ---- */
RenderPoly g_particlePoly;                   /* 0x6d82b4  the one polygon every flat particle is pushed through */
IDirect3DVertexBuffer7 *g_pVertexBufSrc = 0; /* 0x6d82d4  D3DFVF_XYZ */
IDirect3DVertexBuffer7 *g_pVertexBufXf = 0;  /* 0x6d82d8  D3DFVF_XYZRHW */
/* 0x6d82dc-0x6d82fc: 32 bytes of this object's .bss that no instruction refers to (tools/tu_sheet.py). Genuinely opaque:
 * nothing reads or writes them, so neither their type nor their name can be recovered; kept as bytes so that the
 * object's .bss has its original size. */
static u8 s_unref_6d82dc[32] = {0};

/* 0x52a89f - creates the two particle vertex buffers of `count` vertices, each only once. */
void Sfx_CreateVertexBuffers(u32 count)
{
    D3DVERTEXBUFFERDESC desc;
    if (g_pVertexBufSrc == 0) {
        memset(&desc, 0, sizeof(desc));
        desc.dwSize = sizeof(desc);
        desc.dwCaps = D3DVBCAPS_DONOTCLIP;
        desc.dwFVF = D3DFVF_XYZ;
        desc.dwNumVertices = count;
        g_pD3DAppMain->CreateVB(&desc, &g_pVertexBufSrc);
    }
    if (g_pVertexBufXf == 0) {
        memset(&desc, 0, sizeof(desc));
        desc.dwSize = sizeof(desc);
        desc.dwCaps = D3DVBCAPS_DONOTCLIP;
        desc.dwFVF = D3DFVF_XYZRHW;
        desc.dwNumVertices = count;
        g_pD3DAppMain->CreateVB(&desc, &g_pVertexBufXf);
    }
}

/* 0x52a9e9 - draws every live particle as a camera-facing quad: the positions are transformed by ProcessVertices
 * (world = identity, view = the camera's) and each one on screen becomes a Draw2D_TexRect of the particle's size
 * scaled by the projection and the depth. A frame index past the sheet's frame count draws a double-width quad from
 * two frames (frame / count, then frame % count). altSheet swaps sheet 0 for sheet 10. */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py) */
void ParticleEmitter::Emitter_Render(Camera *view, s32 altSheet)
{
    /* local names chosen for their stack slots (tools/vc6_locals.py) */
    AnimSprite *sprite = &s_spriteSheets[sheetIndex];
    Mat44 ident;
    void *data;
    u32 len;
    D3DXyzrhwVertex *vtx8;
    Particle *p;
    u8 i3;
    s32 wide7;
    float half3;
    float hh;
    float x;
    float y;
    SpriteFrame *fr6;
    u32 color1;

    if (!flags.active)
        return;
    if (!sprite->frameCount)
        return;
    if (altSheet && sheetIndex == 0)
        sprite = &s_spriteSheets[10];
    g_pVertexBufSrc->Lock(DDLOCK_WAIT, &data, &len);
    memcpy(data, slotPool, count * 12);
    g_pVertexBufSrc->Unlock();
    ident.SetIdentity();
    g_pD3DAppMain->SetTransform(D3DTRANSFORMSTATE_WORLD, &ident);
    g_pD3DAppMain->SetTransform(D3DTRANSFORMSTATE_VIEW, &view->viewMat);
    g_pVertexBufXf->ProcessVertices(D3DVOP_TRANSFORM, 0, count, g_pVertexBufSrc, 0, g_pD3DAppMain->GetDevice(),
                                    D3DPV_DONOTCOPYDATA);
    g_pVertexBufXf->Lock(DDLOCK_WAIT, &data, &len);
    /* cast kept: the locked vertex buffer is untyped; its format (D3DFVF_XYZRHW) makes it D3DXyzrhwVertex */
    vtx8 = (D3DXyzrhwVertex *)data;
    p = particles;
    for (i3 = 0; i3 < count; i3++) {
        if (p->alive && vtx8->z >= 0.0f && vtx8->z < 1.0f) {
            if (p->frame >= sprite->frameCount)
                wide7 = 1;
            else
                wide7 = 0;
            half3 = (float)p->size * g_projMatrix.m[0][0] * vtx8->rhw * g_screenW * 0.5f;
            if (wide7)
                half3 *= 2;
            x = vtx8->x - half3 / 2.0f;
            if (x + half3 >= 0.0f && g_screenW > x && g_screenW / 2 > half3) {
                y = vtx8->y - (hh = -(float)p->size * g_projMatrix.m[1][1] * vtx8->rhw * g_screenH * 0.5f) / 2.0f;
                if (y + hh >= 0.0f && g_screenH > y && g_screenH / 2 > hh) {
                    if (wide7) {
                        fr6 = &sprite->frames[p->frame / sprite->frameCount];
                        half3 /= 2.0f;
                    } else
                        fr6 = &sprite->frames[p->frame];
                    while (wide7 >= 0) {
                        color1 = p->grey | p->grey << 8 | p->grey << 16;
                        Draw2D_TexRect(
                            vtx8->z, x, y, x + half3, y + hh, fr6->texPage, Tex_CornerUV(0, sprite->width, fr6->u),
                            Tex_CornerUV(0, sprite->height, fr6->v), color1, Tex_CornerUV(0, sprite->width, fr6->u),
                            Tex_CornerUV(sprite->height, sprite->height, fr6->v), color1,
                            Tex_CornerUV(sprite->width, sprite->width, fr6->u), Tex_CornerUV(0, sprite->height, fr6->v),
                            color1, Tex_CornerUV(sprite->width, sprite->width, fr6->u),
                            Tex_CornerUV(sprite->height, sprite->height, fr6->v), color1);
                        wide7--;
                        if (wide7 >= 0) {
                            x += half3;
                            fr6 = &sprite->frames[p->frame % sprite->frameCount];
                        }
                    }
                }
            }
        }
        vtx8++;
        p++;
    }
    g_pVertexBufXf->Unlock();
}

/* 0x52afa8 */
void ParticleEmitter::Emitter_RenderFlat_Fwd(Camera *view)
{
    Emitter_RenderFlat(view);
}

/* 0x52afc1 - draws every live particle as a quad lying in the ground plane: the four corners are the slot position
 * +/- the half size rotated by the particle angle, transformed by ProcessVertices and pushed as two textured
 * triangles through g_particlePoly into the polygon batcher. The UVs of every frame are computed once. */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py) */
void ParticleEmitter::Emitter_RenderFlat(Camera *view)
{
    /* local names chosen for their stack slots (tools/vc6_locals.py) */
    AnimSprite *sprite = &s_spriteSheets[sheetIndex];
    u32 rgb;
    float rn;
    u8 m;
    float uvs[10][8];
    u32 len;
    D3DXyzrhwVertex *xv2;
    Vec3f *dst0;
    float z;
    SpriteFrame *frame;
    float x;
    Mat44 worldZ;
    float h;
    void *data;
    Vec3f *srcn;
    D3DTLVertex *tl;
    Particle *q8;
    float b;
    float a;
    u32 i7;

    if (!flags.active)
        return;
    if (!sprite->frameCount)
        return;
    frame = sprite->frames;
    for (i7 = 0; i7 < sprite->frameCount; i7++) {
        uvs[i7][0] = Tex_CornerUV(0, sprite->width, frame->u);
        uvs[i7][1] = Tex_CornerUV(0, sprite->height, frame->v);
        uvs[i7][2] = Tex_CornerUV(sprite->width, sprite->width, frame->u);
        uvs[i7][3] = Tex_CornerUV(0, sprite->height, frame->v);
        uvs[i7][4] = Tex_CornerUV(sprite->width, sprite->width, frame->u);
        uvs[i7][5] = Tex_CornerUV(sprite->height, sprite->height, frame->v);
        uvs[i7][6] = Tex_CornerUV(0, sprite->width, frame->u);
        uvs[i7][7] = Tex_CornerUV(sprite->height, sprite->height, frame->v);
        frame++;
    }
    g_pVertexBufSrc->Lock(DDLOCK_WAIT, &data, &len);
    q8 = particles;
    srcn = slotPool;
    dst0 = (Vec3f *)data; /* cast kept: the locked vertex buffer is untyped; its format (D3DFVF_XYZ) makes it Vec3f */
    for (m = 0; m < count; m++) {
        if (q8->alive) {
            h = (float)q8->size / 2.0f;
            x = -((float)sin(rn = Math_Angle4096ToRadians_2(q8->angle)) * h);
            z = -((float)cos(rn) * h);
            a = x + z;
            b = x - z;
            x = srcn->x;
            z = srcn->z;
            dst0[0].x = x + b;
            dst0[0].y = srcn->y;
            dst0[0].z = z + a;
            dst0[1].x = x + a;
            dst0[1].y = srcn->y;
            dst0[1].z = z - b;
            dst0[2].x = x - b;
            dst0[2].y = srcn->y;
            dst0[2].z = z - a;
            dst0[3].x = x - a;
            dst0[3].y = srcn->y;
            dst0[3].z = z + b;
        }
        q8++;
        srcn++;
        dst0 += 4;
    }
    g_pVertexBufSrc->Unlock();
    worldZ.SetIdentity();
    g_pD3DAppMain->SetTransform(D3DTRANSFORMSTATE_WORLD, &worldZ);
    g_pD3DAppMain->SetTransform(D3DTRANSFORMSTATE_VIEW, &view->viewMat);
    g_pVertexBufXf->ProcessVertices(D3DVOP_TRANSFORM, 0, count * 4, g_pVertexBufSrc, 0, g_pD3DAppMain->GetDevice(),
                                    D3DPV_DONOTCOPYDATA);
    g_pVertexBufXf->Lock(DDLOCK_WAIT, &data, &len);
    /* cast kept: the locked vertex buffer is untyped; its format (D3DFVF_XYZRHW) makes it D3DXyzrhwVertex */
    xv2 = (D3DXyzrhwVertex *)data;
    q8 = particles;
    /* cast kept: RenderPoly.verts is a float array; this poly's vertices are D3DTLVertex records */
    tl = (D3DTLVertex *)g_particlePoly.verts;
    for (m = 0; m < count; m++) {
        if (q8->alive && 1) {
            frame = &sprite->frames[q8->frame];
            rgb = q8->grey | q8->grey << 8 | q8->grey << 16;
            g_particlePoly.type = frame->texPage + RPOLY_TEXTURED_BASE;
            tl[2].color = rgb;
            tl[1].color = rgb;
            tl[0].color = rgb;
            tl[2].specular = 0xff000000;
            tl[1].specular = 0xff000000;
            tl[0].specular = 0xff000000;
            memcpy(&tl[0], &xv2[0], 16);
            memcpy(&tl[1], &xv2[1], 16);
            memcpy(&tl[2], &xv2[2], 16);
            tl[0].u = uvs[q8->frame][0];
            tl[0].v = uvs[q8->frame][1];
            tl[1].u = uvs[q8->frame][2];
            tl[1].v = uvs[q8->frame][3];
            tl[2].u = uvs[q8->frame][4];
            tl[2].v = uvs[q8->frame][5];
            g_pPolyBin->SubmitPoly(&g_particlePoly);
            memcpy(&tl[0], &xv2[2], 16);
            memcpy(&tl[1], &xv2[3], 16);
            memcpy(&tl[2], &xv2[0], 16);
            tl[0].u = uvs[q8->frame][4];
            tl[0].v = uvs[q8->frame][5];
            tl[1].u = uvs[q8->frame][6];
            tl[1].v = uvs[q8->frame][7];
            tl[2].u = uvs[q8->frame][0];
            tl[2].v = uvs[q8->frame][1];
            g_pPolyBin->SubmitPoly(&g_particlePoly);
        }
        xv2 += 4;
        q8++;
    }
    g_pVertexBufXf->Unlock();
}

/* 0x52c636 - writes the particle at the ring index and advances the ring. */
void ParticleEmitter::Emitter_AddParticle(const Vec3f *pos, s16 angle, s16 size, u8 grey, u8 frame)
{
    Particle *p;
    s32 slot = ring;
    if (slot < count) {
        p = &particles[slot];
        p->age = 0;
        p->angle = (s16)(angle & 0xfff);
        p->size = size;
        p->alive = 1;
        p->grey = grey;
        p->frame = frame;
        slotPool[slot].x = pos->x;
        slotPool[slot].y = pos->y;
        slotPool[slot].z = pos->z;
        ring = (slot + 1) % count;
    }
}

/* 0x52c763 - spawns the particles due between the last call and `now`: one every `interval` ticks, each at the
 * source position interpolated back along the move since the last call. frameSel -1 picks a random frame, -2
 * the ring index modulo the frame count. spawn == 0 stops the stream (and forgets the last position). */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py) */
void ParticleEmitter::Sfx_SpawnBillboard(const Vec3s *src, s16 angle, s16 size, u8 grey, u32 interval, u32 now,
                                         s32 spawn, u8 frameSel)
{
    Vec3f pos; /* names chosen for their stack slots (tools/vc6_locals.py) */
    u32 nFrames;
    u8 frame;
    float f;
    Vec3f d;
    u32 t;

    nFrames = s_spriteSheets[sheetIndex].frameCount;
    if (nFrames == 0)
        nFrames = 1;
    if (spawn) {
        if (lastSpawnTime == 0) {
            if (now != lastSpawnTime) {
                switch (frameSel) {
                    case EMITTER_FRAME_RANDOM:
                        frame = Rand_Bounded(nFrames);
                        break;
                    case EMITTER_FRAME_CYCLE:
                        frame = ring % (s32)nFrames;
                        break;
                    default:
                        frame = frameSel;
                }
                pos.x = src->x;
                pos.y = src->y;
                pos.z = src->z;
                Emitter_AddParticle(&pos, angle, size, grey, frame);
            }
            spawnCursor = now;
        } else if (now != lastSpawnTime) {
            d.x = src->x - lastSrc[0];
            d.y = src->y - lastSrc[1];
            d.z = src->z - lastSrc[2];
            while (spawnCursor + interval <= now) {
                t = spawnCursor + interval;
                f = (float)(now - t) / (now - lastSpawnTime);
                pos.x = src->x - d.x * f;
                pos.y = src->y - d.y * f;
                pos.z = src->z - d.z * f;
                switch (frameSel) {
                    case EMITTER_FRAME_RANDOM:
                        frame = Rand_Bounded(nFrames);
                        break;
                    case EMITTER_FRAME_CYCLE:
                        frame = ring % (s32)nFrames;
                        break;
                    default:
                        frame = frameSel;
                }
                Emitter_AddParticle(&pos, angle, size, grey, frame);
                spawnCursor = t;
            }
        }
        lastSrc[0] = src->x;
        lastSrc[1] = src->y;
        lastSrc[2] = src->z;
        lastSpawnTime = now;
    } else {
        lastSpawnTime = 0;
        spawnCursor = 0;
    }
}

/* 0x52c9ec - dust / smoke: particles drift along -sin/-cos(angle) at p->hSpeed and vertically at p->vSpeed per
 * second, grow from sizeStart to sizeEnd, fade in over the first quarter of their life and out over the rest. */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py) */
s32 ParticleEmitter::Emitter_UpdateDrift(const EmitterDriftParams *p, Vec3s *src, s16 angle, s32 spawn)
{
    /* local names chosen for their stack slots (tools/vc6_locals.py) */
    s32 invLife = 0x1000000 / p->life;
    s32 live = 0;
    u32 idx = 0;
    Particle *q = particles;
    Vec3f *xyz8 = slotPool;
    AnimSprite *sprite;
    float hMove;
    float dv;
    s32 age7;
    float rn;
    s32 grey;

    sheetIndex = p->sheetIndex;
    sprite = &s_spriteSheets[sheetIndex];
    Sfx_SpawnBillboard(src, angle, p->sizeStart, 0x80, p->spawnInterval, lastSpawnTime + g_dt, spawn,
                       EMITTER_FRAME_RANDOM);
    hMove = (float)(p->hSpeed * g_dt) / 4096.0f;
    dv = (float)(p->vSpeed * g_dt) / 4096.0f;
    while (idx < count) {
        if (q->alive) {
            age7 = q->age;
            if (age7 < p->life) {
                q->age = age7 + g_dt;
                age7 = age7 * invLife >> 12;
                xyz8->x -= (float)sin(rn = Math_Angle4096ToRadians_2(q->angle)) * hMove;
                xyz8->z -= (float)cos(rn) * hMove;
                xyz8->y += dv;
                q->size = p->sizeStart + ((p->sizeEnd - p->sizeStart) * age7 >> 12);
                grey = age7 << 7 >> 12;
                if (grey < 0x20)
                    grey <<= 2;
                else
                    grey = 0x80 - (grey * 4 - 0x80) / 3;
                q->grey = (u8)grey;
                live++;
            } else
                q->alive = 0;
        }
        idx++;
        q++;
        xyz8++;
    }
    flags.active = live != 0;
    return flags.active;
}

/* 0x52cc89 - bubbles: particles rise at p->riseSpeed per second and die when their life ends or once they are above
 * the cap height (vertical points down, so above = smaller). The inverse life is computed and never used. */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py) */
/* BYTES(dead-code): invZ is computed and never used, as in the original */
s32 ParticleEmitter::Emitter_UpdateRiseToCap(const EmitterRiseParams *p, Vec3s *src, s32 spawn)
{
    /* local names chosen for their stack slots (tools/vc6_locals.py) */
    s32 invZ = 0x1000000 / p->life;
    s32 live = 0;
    u32 idx = 0;
    Particle *q = particles;
    Vec3f *v = slotPool;
    AnimSprite *sprite;
    float vStep;
    s32 age;

    sheetIndex = p->sheetIndex;
    sprite = &s_spriteSheets[sheetIndex];
    Sfx_SpawnBillboard(src, 0, p->size, 0x80, p->spawnInterval, lastSpawnTime + g_dt, spawn, EMITTER_FRAME_RANDOM);
    vStep = (float)(p->riseSpeed * g_dt) / 4096.0f;
    while (idx < count) {
        if (q->alive) {
            age = q->age;
            if (age < p->life) {
                q->age = age + g_dt;
                v->y += vStep;
                if (p->cap >= v->y)
                    q->alive = 0;
                else
                    live++;
            } else
                q->alive = 0;
        }
        idx++;
        q++;
        v++;
    }
    flags.active = live != 0;
    return flags.active;
}

/* 0x52ce36 - the perfume scent: like Emitter_UpdateDrift, but the horizontal drift (only when `drift` is set) is
 * scaled by how close the particle is to referenceHeight: (range - |ref - y|) / range, none beyond range. */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py) */
s32 ParticleEmitter::Emitter_UpdatePerfume(const EmitterPerfumeParams *params, Vec3s *source, s16 direction, s32 drift,
                                           s16 referenceHeight, s32 spawn)
{
    /* local names chosen for their stack slots (tools/vc6_locals.py) */
    s32 inv = 0x1000000 / params->life;
    s32 live = 0;
    u32 idx = 0;
    Particle *q = particles;
    Vec3f *xyz7 = slotPool;
    AnimSprite *sprite;
    float hStep9;
    float dv;
    s32 age6;
    float dy;
    float k;
    float hs3;
    float rn;

    sheetIndex = params->sheetIndex;
    sprite = &s_spriteSheets[sheetIndex];
    Sfx_SpawnBillboard(source, 0, params->sizeStart, 0x80, params->spawnInterval, lastSpawnTime + g_dt, spawn,
                       EMITTER_FRAME_RANDOM);
    direction &= 0xfff;
    hStep9 = (float)(params->hSpeed * g_dt) / 4096.0f;
    dv = (float)(params->vSpeed * g_dt) / 4096.0f;
    while (idx < count) {
        if (q->alive) {
            age6 = q->age;
            if (age6 < params->life) {
                q->age = age6 + g_dt;
                age6 = age6 * inv >> 12;
                k = params->range - ((dy = referenceHeight - xyz7->y) >= 0.0f ? dy : -dy);
                if (drift && k > 0.0f) {
                    hs3 = hStep9 * k / params->range;
                    xyz7->x -= (float)sin(rn = Math_Angle4096ToRadians_2(direction)) * hs3;
                    xyz7->z -= (float)cos(rn) * hs3;
                }
                xyz7->y += dv;
                q->size = params->sizeStart + ((params->sizeEnd - params->sizeStart) * age6 >> 12);
                live++;
            } else
                q->alive = 0;
        }
        idx++;
        q++;
        xyz7++;
    }
    flags.active = live != 0;
    return flags.active;
}

/* 0x52d0ee - like Emitter_UpdateDrift, but the vertical step grows with age and the size grows over the first
 * quarter of the life only. The age is rescaled to 0..4096 before it is compared with life / 4 (ticks) and scaled
 * again, so for a life above 16384 ticks the size reaches its end value later than a quarter of the way. */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py) */
s32 ParticleEmitter::Emitter_UpdateDriftLift(const EmitterDriftParams *p, Vec3s *src, s16 angle, s32 spawn)
{
    /* local names chosen for their stack slots (tools/vc6_locals.py) */
    s32 invLife = 0x1000000 / p->life;
    s32 live = 0;
    u32 idx = 0;
    Particle *q = particles;
    Vec3f *xyz8 = slotPool;
    AnimSprite *sprite;
    float hMove;
    float dv;
    s32 age7;
    float rn;

    sheetIndex = p->sheetIndex;
    sprite = &s_spriteSheets[sheetIndex];
    Sfx_SpawnBillboard(src, angle & 0xfff, p->sizeStart, 0x80, p->spawnInterval, lastSpawnTime + g_dt, spawn,
                       EMITTER_FRAME_RANDOM);
    hMove = (float)(p->hSpeed * g_dt) / 4096.0f;
    dv = (float)(p->vSpeed * g_dt) / 4096.0f;
    while (idx < count) {
        if (q->alive) {
            age7 = q->age;
            if (age7 < p->life) {
                q->age = age7 + g_dt;
                age7 = age7 * invLife >> 12;
                xyz8->x -= (float)sin(rn = Math_Angle4096ToRadians_2(q->angle)) * hMove;
                xyz8->z -= (float)cos(rn) * hMove;
                xyz8->y += age7 * dv / p->life;
                if (age7 < p->life >> 2) {
                    age7 = age7 * invLife >> 10;
                    q->size = p->sizeStart + ((p->sizeEnd - p->sizeStart) * age7 >> 12);
                } else
                    q->size = p->sizeEnd;
                live++;
            } else
                q->alive = 0;
        }
        idx++;
        q++;
        xyz8++;
    }
    flags.active = live != 0;
    return flags.active;
}

/* 0x52d381 - a column held on the anchor: every particle is put back on the anchor's x / z each frame and rises
 * at riseSpeed; the frame index is the caller's age / period (a countdown shown by the frame). */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py) */
s32 ParticleEmitter::Emitter_UpdateColumn(const EmitterColumnParams *params, const Vec3s *anchor, s32 age, s32 spawn)
{
    /* local names chosen for their stack slots (tools/vc6_locals.py) */
    s32 invLife = 0x1000000 / params->life;
    s32 live = 0;
    u32 idx = 0;
    Particle *q = particles;
    Vec3f *xyz8 = slotPool;
    AnimSprite *sprite;
    u8 fr7;
    float vStep;
    s32 age7;

    sheetIndex = params->sheetIndex;
    sprite = &s_spriteSheets[sheetIndex];
    fr7 = age / params->period;
    Sfx_SpawnBillboard(anchor, 0, params->sizeStart, 0x80, params->period, lastSpawnTime + g_dt, spawn, fr7);
    vStep = (float)(params->riseSpeed * g_dt) / 4096.0f;
    while (idx < count) {
        if (q->alive) {
            age7 = q->age;
            if (age7 < params->life) {
                q->age = age7 + g_dt;
                age7 = age7 * invLife >> 12;
                xyz8->x = anchor->x;
                xyz8->z = anchor->z;
                xyz8->y += vStep;
                q->size = params->sizeStart + ((params->sizeEnd - params->sizeStart) * age7 >> 12);
                live++;
            } else
                q->alive = 0;
        }
        idx++;
        q++;
        xyz8++;
    }
    flags.active = live != 0;
    return flags.active;
}

/* 0x52d57c - motionless particles (rings, wakes): full grey until fadeStart, then fading to 0 at the end of the
 * life, the size growing from sizeStart to sizeEnd over the whole life. */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py) */
s32 ParticleEmitter::Emitter_UpdateFade(const EmitterFadeParams *p, Vec3s *src, s16 angle, s32 spawn)
{
    /* local names chosen for their stack slots (tools/vc6_locals.py) */
    s32 invLife = 0x1000000 / p->life;
    s32 invOut = 0x1000000 / (p->life - p->fadeStart);
    s32 live = 0;
    u32 idx = 0;
    Particle *q = particles;
    Vec3f *xyz8 = slotPool;
    AnimSprite *sprite;
    s32 age7;

    sheetIndex = p->sheetIndex;
    sprite = &s_spriteSheets[sheetIndex];
    Sfx_SpawnBillboard(src, angle, p->sizeStart, 0x80, p->spawnInterval, lastSpawnTime + g_dt, spawn,
                       EMITTER_FRAME_RANDOM);
    while (idx < count) {
        if (q->alive) {
            age7 = q->age;
            if (age7 < p->life) {
                q->age = age7 + g_dt;
                if (age7 <= p->fadeStart)
                    q->grey = 0x80;
                else
                    q->grey = (u8)(0x80 - ((age7 - p->fadeStart << 7) * invOut >> 24));
                age7 = age7 * invLife >> 12;
                q->size = p->sizeStart + ((p->sizeEnd - p->sizeStart) * age7 >> 12);
                live++;
            } else
                q->alive = 0;
        }
        idx++;
        q++;
        xyz8++;
    }
    flags.active = live != 0;
    return flags.active;
}

/* 0x52d791 - footprints: spawns on the caller's clock (lastSpawnTime + spawnTime) with the frame chosen by the
 * ring index (frameSel 0xfe); with keep == 0 the emitter's spawn clock is put back afterwards. Particles do not
 * move; life 0x7fffffff means permanent (the emitter is just marked active, nothing ages), otherwise they fade
 * over the last quarter of their life. */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py) */
s32 ParticleEmitter::Emitter_UpdateTrail(const EmitterTrailParams *p, Vec3s *src, s32 spawnTime, s16 angle, s32 keep)
{
    /* local names chosen for their stack slots (tools/vc6_locals.py) */
    AnimSprite *sprite;
    u32 savedLast;
    u32 cur0;
    s32 live7;
    s32 start;
    s32 invOut;
    u32 n;
    Particle *q4;
    s32 age7;

    sheetIndex = p->sheetIndex;
    sprite = &s_spriteSheets[sheetIndex];
    savedLast = lastSpawnTime;
    cur0 = spawnCursor;
    Sfx_SpawnBillboard(src, angle, p->size, 0x80, p->param4, lastSpawnTime + spawnTime, keep, EMITTER_FRAME_CYCLE);
    if (keep == 0) {
        lastSpawnTime = savedLast;
        spawnCursor = cur0;
    }
    if (p->life == 0x7fffffff)
        flags.active = 1;
    else {
        live7 = 0;
        start = p->life * 3 / 4;
        invOut = 0x1000000 / (p->life - start);
        n = 0;
        q4 = particles;
        while (n < count) {
            if (q4->alive) {
                age7 = q4->age;
                if (age7 < p->life) {
                    q4->age = age7 + g_dt;
                    if (age7 > start)
                        q4->grey = (u8)(0x80 - ((age7 - start << 7) * invOut >> 24));
                    live7++;
                } else
                    q4->alive = 0;
            }
            n++;
            q4++;
        }
        flags.active = live7 != 0;
    }
    return flags.active;
}

/* 0x52d971 - Emitter_UpdateDrift without the fade, with the frame animated through the sheet over the life. */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py) */
s32 ParticleEmitter::Emitter_UpdateDriftAnimated(const EmitterDriftParams *p, Vec3s *src, s16 angle, s32 spawn)
{
    /* local names chosen for their stack slots (tools/vc6_locals.py) */
    s32 invLife = 0x1000000 / p->life;
    s32 live = 0;
    u32 idx = 0;
    Particle *q = particles;
    Vec3f *xyz8 = slotPool;
    AnimSprite *sprite;
    float hMove;
    float dv;
    s32 age7;
    float rn;

    sheetIndex = p->sheetIndex;
    sprite = &s_spriteSheets[sheetIndex];
    Sfx_SpawnBillboard(src, angle, p->sizeStart, 0x80, p->spawnInterval, lastSpawnTime + g_dt, spawn,
                       EMITTER_FRAME_RANDOM);
    hMove = (float)(p->hSpeed * g_dt) / 4096.0f;
    dv = (float)(p->vSpeed * g_dt) / 4096.0f;
    while (idx < count) {
        if (q->alive) {
            age7 = q->age;
            if (age7 < p->life) {
                q->age = age7 + g_dt;
                age7 = age7 * invLife >> 12;
                xyz8->x -= (float)sin(rn = Math_Angle4096ToRadians_2(q->angle)) * hMove;
                xyz8->z -= (float)cos(rn) * hMove;
                xyz8->y += dv;
                q->size = p->sizeStart + ((p->sizeEnd - p->sizeStart) * age7 >> 12);
                q->frame = (u8)(sprite->frameCount * age7 >> 12);
                live++;
            } else
                q->alive = 0;
        }
        idx++;
        q++;
        xyz8++;
    }
    flags.active = live != 0;
    return flags.active;
}

/* 0x52dbe3 - kills every particle and forgets the spawn state. */
/* BYTES(slot-name): names chosen for their stack slots: i at ebp-4, cur at ebp-8 */
void ParticleEmitter::Emitter_Reset()
{
    s32 i = 0; /* slot order by name: i at ebp-4, cur at ebp-8 */
    Particle *cur = particles;
    while (i < count) {
        cur->alive = 0;
        i++;
        cur++;
    }
    lastSpawnTime = 0;
    spawnCursor = 0;
    ring = 0;
    sheetIndex = EMITTER_SHEET_NONE;
    flags.active = 0;
    lastSrc[2] = 0;
    lastSrc[1] = 0;
    lastSrc[0] = 0;
}

/* 0x52dc84 - allocates the two pools of n slots from the level heap (or none), then resets. */
void ParticleEmitter::Emitter_Init(s32 n)
{
    if (n == 0) {
        slotPool = 0;
        particles = 0;
        count = 0;
    } else {
        /* cast kept (both pools): Heap::Alloc returns untyped memory */
        slotPool = (Vec3f *)g_scenaricLevelHeap.Alloc(n * 12);
        particles = (Particle *)g_scenaricLevelHeap.Alloc(n * 8);
        count = n;
    }
    Emitter_Reset();
}

/* 0x57bc38 - the DAV bitmap groups of the 11 effect sheets, in sheet-index order */
static u16 s_effectSheetRes[11] = {DAV_IDI_IGLSMOK_, DAV_IDI_IGLTIDE_, DAV_IDI_IGLBULL_, DAV_IDI_IFLFUMEE,
                                   DAV_IDI_IGLEAVE1, DAV_IDI_IMNCERC_, DAV_IDI_IPASNE01, DAV_IDI_ITICPTR_,
                                   DAV_IDI_IFTNOTE_, DAV_IDI_IPASNE02, DAV_IDI_IGLSMO2_};

/* 0x52dcf1 - loads the 11 effect sprite sheets and creates the particle vertex buffers (0x200 vertices). */
void Sfx_InitSpriteSheets()
{
    s32 i;
    for (i = 0; i < 11; i++)
        s_spriteSheets[i].InitFromRes(s_effectSheetRes[i]);
    Sfx_CreateVertexBuffers(0x200);
}
