/* T268 - original object "WeatherFx.cpp" (guessed name): the Weather class methods 0x52e1e0-0x531eca (the static
 * initialiser, g_weather and the Weather_* level hooks 0x52df80-0x52e1d3 are T267, src/engine/weather.cpp).
 *   .text  0x52e1e0-0x531eca  Weather ctor/dtor, SetVolume, SetTexture(2), SfxParticles_InitRandom, Advance, DrawRain,
 *                             DrawSnow, CreateVertexBuffers, WrapToVolume (11), then COMDAT ??_GWeather 0x531ed0
 *   .rdata 0x577148-0x577150  ??_7Weather (emitted here: the out-of-line ctor is here) and __real@46fffe00
 * T267's static initialiser calls the out-of-line ctor 0x52e1e0, so the vtable and ??_G are first emitted by this object.
 */
/* BYTES: cast, dead-code, inline, slot-name. */
/* BYTES(inline): Vec3f() {} (member macro): empty user-declared constructor: gives the original's loop over the particles in the Weather constructor */
/* BYTES(inline): Camera::ViewDir, Progress::CurrentLevel (member-macro inlines): source-only inline (unused in this object; kept with T267's declarations) */
/* BYTES(inline): ZoneList::FindBelowTop (member-macro inline): source-only inline: the list's address goes through a stack temp (0x52eb13) */
/* BYTES(inline): D3DApp::CreateVBWithResult / SetTransform / GetDevice (member-macro inlines): source-only inline: its expansion gives the original's temporaries */
/* BYTES(inline): SUBMIT_BODY macro; PolyBatcher::AddPolyA / AddPolyB / AddPolyC / AddPolyD (inline twins): source-only inline twins of SubmitPoly 0x415c80: each expansion names which render helpers the original expands and which it calls (/Ob1 budget) */
/* BYTES(slot-name): SUBMIT_BODY macro: names chosen for the expansions' stack slots */
/* match-addr: srand=0x567b72 time=0x567ba1   (both are symbol rows too; kept as a pin) */
/*
 * The notes below describe the whole rain and snow feature, of which this object is the Weather class part (no
 * g_weather, no static initialiser, no Weather_* hooks here). Declarations only T267 needs (the camera view direction,
 * Progress::CurrentLevel, HALF_WIDTH_AT) are kept: unused inline declarations emit no code.
 *
 * Rain and snow, SheepD3D.exe 0x52df80-0x531eff (T267 and this object): the one weather object g_weather (0x6d8318, class
 * Weather, vtable 0x577148, sizeof 0x5698) and the hooks the level loader and Game_Frame call on it. The level's WAR
 * header says which weather it has (g_weatherType 0x6ddf6a: 1 rain, 2 snow); the loader then calls Weather_InitRain or
 * Weather_InitSnow, and Game_Frame calls Weather_RenderRain / Weather_RenderSnow after the scene.
 *
 * The particles live in a sphere of camera space (centre 0, 0, sqrt(r^2 - halfWidth^2) + near plane) that is carried
 * along with the camera: every frame each particle moves by vel * g_dtMs / 1000 (Weather_Advance), and a particle that
 * has left the sphere is mirrored through its centre (Weather_WrapToVolume), so the field never runs out. A particle
 * whose position lies inside a water box (g_waterZones, list 0) is cut at the surface and fades out with depth.
 * Rain draws each drop as a streak quad (two triangles) along the fall direction, snow a camera-facing quad; both go
 * through one XYZ vertex buffer, ProcessVertices into an XYZRHW buffer, and g_weather.poly into the PolyBatcher.
 *
 * VC6 also emits an unreferenced ??_H vector-constructor-iterator thunk for the particle array (Vec3f has an empty
 * inline constructor), which the original's linker dropped (as in shark.cpp). Local names are chosen for their stack
 * slots (tools/vc6_locals.py); the inline helpers have no bodies of their own in the exe, so their names are not
 * recovered.
 *
 * Two shapes here are representations, not the original's text:
 *  - The draw routines expand PolyBatcher_SubmitPoly 0x415c80 twice each, and in every expansion VC6 expanded a
 *    different subset of the render-device helpers it calls (Render_SetStateFlags 0x4155f0, Render_ClearStateFlags
 *    0x4159b0, Render_DrawPrimitive 0x415c00, Render_SetTexture 0x415c40: all four are the out-of-line copies of
 *    inline functions, and compiled from the inline twins below they match those addresses byte for byte, as does
 *    PolyBatcher_SubmitPoly 0x415c80 itself compiled out of line from SUBMIT_BODY with the flat-batch helpers
 *    expanded; checked with a separate compile, not part of this file). Written as one inline body with every helper
 *    inline, VC6 expands all of them (7,458 bytes instead of 6,901), so its budget decisions are not reproduced; the
 *    four AddPoly* expansions spell out which helper each site expands and which it calls, through one macro body
 *    (SUBMIT_BODY).
 *  - Weather_InitRain / _InitSnow: see HALF_WIDTH_AT.
 */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "../sdk/d3d7.h"
#include "../sdk/ddraw.h"
#include "../sdk/win32.h"
#include "../sdk/crt.h"
class Mat44;
#define IsEqualGUID(a, b) (!memcmp((a), (b), 16)) /* sizeof(GUID); the type is incomplete here */
#include "scn_tools.h"
#include "load_warmeshes.h"
#include "game_state.h"
#include "draw2d.h"
#include "progress.h"
#include "../app/app_main.h"
#include "scenaric.h"
#include "screen.h"
u32 *Res_GetValidatedIdList(u16 resId, u16 *outCount); /* 0x548381 */

#define SDW_MEMBERS_Vec3f \
    Vec3f() {} /* empty but user-declared: the ctor's loop over the particles */
#define SDW_MEMBERS_Mat44   \
    Mat44(); /* 0x4077f0 */ \
    /* under the names T007 (mat44.cpp) defines them by; the bodies' Mat44_ spelling is mapped after the class header */
#define SDW_MEMBERS_RenderPoly                                              \
    RenderPoly();                            /* 0x41aad0 RenderPoly_Ctor */ \
    /* virtual ~RenderPoly() is generated */ /* 0x41ac04 RenderPoly_Dtor */


#define SDW_MEMBERS_ZoneList                                         \
    /* inline: the list's address in a stack temp (0x52eb13) */      \
    Box *FindBelowTop(Vec3s *p)                                      \
    {                                                                \
        return BoxList_FindContainingPointBelowTop(p, boxes, count); \
    }
#define SDW_MEMBERS_D3DApp                                                                            \
    long CreateVBWithResult(D3DVERTEXBUFFERDESC *desc, IDirect3DVertexBuffer7 **out);                 \
    void SetTransform(u32 state, Mat44 *m);                                                           \
    IDirect3DDevice7 *GetDevice();                                        /* inline */                \
    void Render_SetStateFlags(u32 flags);                                 /* 0x4155f0 */              \
    void Render_ClearStateFlags(u32 flags);                               /* 0x4159b0 */              \
    void SetStateFlagsInline(u32 flags);                                  /* inline, defined below */ \
    void ClearStateFlagsInline(u32 flags);                                /* inline, defined below */ \
    void Render_DrawPrimitive(u32 type, u32 fvf, void *verts, u32 count); /* 0x415c00 */              \
    void Render_SetTexture(Texture *tex, u32 stage);                      /* 0x415c40 */              \
    void DrawPrimitiveInline(u32 type, u32 fvf, void *verts, u32 count);  /* inline, defined below */ \
    void BindTexture(Texture *tex, s32 stage); /* inline, defined below: a signed stage, so the 0 gets a temp */
#define SDW_MEMBERS_PolyBatcher                                    \
    void AddPolyA(RenderPoly *poly);   /* inline, defined below */ \
    void AddPolyB(RenderPoly *poly);   /* inline, defined below */ \
    void AddPolyC(RenderPoly *poly);   /* inline, defined below */ \
    void AddPolyD(RenderPoly *poly);   /* inline, defined below */
#define SDW_MEMBERS_Weather Weather(); /* 0x52e1e0 */
#include "sdw_classes.h"
#define SDW_INLINE_CAMERA_VIEWDIR 1
#include "../objects/camera_inlines.h"
#undef SDW_INLINE_CAMERA_VIEWDIR
#define SDW_INLINE_PROGRESS_CURRENTLEVEL 1
#include "progress_inlines.h"
#undef SDW_INLINE_PROGRESS_CURRENTLEVEL
#define SDW_INLINE_D3DAPP_CREATEVBWITHRESULT_D3DVERTEXBUFFERDESC_IDIRECT3DVERTEXBUFFER7 2
#define SDW_INLINE_D3DAPP_SETTRANSFORM_U32_MAT44 1
#include "../app/d3dapp_inlines.h"
#undef SDW_INLINE_D3DAPP_CREATEVBWITHRESULT_D3DVERTEXBUFFERDESC_IDIRECT3DVERTEXBUFFER7
#undef SDW_INLINE_D3DAPP_SETTRANSFORM_U32_MAT44
#define SDW_INLINE_D3DAPP_GETDEVICE 1
#include "../app/d3dapp_inlines.h"
#undef SDW_INLINE_D3DAPP_GETDEVICE
#define Mat44_SetIdentity SetIdentity
#define Mat44_SetRotZYX SetRotZYX

/* The DAV directory (DavHeader.dir): packed, its pointers sit at +6/+0xa/+0x12 (Load_DAV, src/engine/load_dav.cpp),
 * which the struct generator cannot lay out, so it is declared here as in lightspot.cpp. */
#pragma pack(push, 1)
struct DavDirectory {
    u16 indexCount;
    u16 bitmapCount;
    u16 unk04;
    u16 *indices;
    DavBitmapRec *bitmaps; /* +0x0a 10-byte records */
    u32 fileSize;
    u32 *idLists;
};
#pragma pack(pop)

extern s32 g_dtMs; /* 0x71b2e8  g_dt * 1000 >> 12 */

#define g_screenW (g_screen.viewportWidth) /* 0x6d6ff4  viewport width in pixels */

#define g_screenH (g_screen.viewportHeight) /* 0x6d6ff6  viewport height in pixels */

#define SDW_INLINE_FREE_ZONES_GET_U8 1
#include "scenaric_inlines.h"
#undef SDW_INLINE_FREE_ZONES_GET_U8
/* half the width of the view at distance d (tan(fov/2) / aspect * d): the half-width of the particle sphere's slice */
#define HALF_WIDTH_AT(d) (((void)0, g_pViewFrustum->tanHalfFov) / g_pViewFrustum->aspect * (d))
/* squared length as this file's code evaluates it: z first, then y and x (0x52e7f7, 0x531d90) */
#define VEC3F_LENSQ(v) ((v).z * (v).z + ((v).y * (v).y + (v).x * (v).x))

/* The vertex formats: ProcessVertices' XYZRHW output, and the batcher's textured triangle vertex (FVF 0x1c4). */
struct D3DXyzrhwVertex {
    float x, y, z, rhw;
};
struct TlVertex {
    float x, y, z, rhw;
    u32 diffuse, specular;
    float u, v;
};

/* ---- inline helpers: the engine's render-device and batcher code as this file's expansions show it. They have
 * no bodies of their own in the exe; their names are descriptive. ---- */

/* The inline twin of Render_DrawPrimitive 0x415c00: nothing is drawn for an empty batch (count = vertices). */
/* BYTES(inline): source-only inline twin of Render_DrawPrimitive 0x415c00: used where the original expands it */
#define SDW_INLINE_D3DAPP_DRAWPRIMITIVEINLINE_U32_U32_VOID_U32 1
#include "../app/d3dapp_inlines.h"
#undef SDW_INLINE_D3DAPP_DRAWPRIMITIVEINLINE_U32_U32_VOID_U32
/* The inline twin of Render_SetTexture 0x415c40 (a signed stage: the constant 0 gets a stack temp). */
/* BYTES(cast): source-only inline twin of Render_SetTexture 0x415c40; the signed stage makes the constant 0 get a stack temp */
#define SDW_INLINE_D3DAPP_BINDTEXTURE_TEXTURE_S32 1
#include "../app/d3dapp_inlines.h"
#undef SDW_INLINE_D3DAPP_BINDTEXTURE_TEXTURE_S32
/* The inline twin of Render_SetStateFlags 0x4155f0. */
/* BYTES(inline): source-only inline twin of Render_SetStateFlags 0x4155f0: used where the original expands it */
#define SDW_INLINE_D3DAPP_SETSTATEFLAGSINLINE_U32 1
#include "../app/d3dapp_inlines.h"
#undef SDW_INLINE_D3DAPP_SETSTATEFLAGSINLINE_U32
/* The inline twin of Render_ClearStateFlags 0x4159b0. */
/* BYTES(inline): source-only inline twin of Render_ClearStateFlags 0x4159b0: used where the original expands it */
#define SDW_INLINE_D3DAPP_CLEARSTATEFLAGSINLINE_U32 1
#include "../app/d3dapp_inlines.h"
#undef SDW_INLINE_D3DAPP_CLEARSTATEFLAGSINLINE_U32

/* PolyBatcher_SubmitPoly 0x415c80 expanded in place, as the two draw routines have it: type 1 into the untextured
 * batch, 2 / 3 into the sorted list, >= 4 into its texture page's batch (an immediate page) or the sorted list; a full
 * batch is drawn at once. SUBMIT_BODY is the one body; the parameters name, for one expansion, which render helper
 * is expanded there (the *Inline twins, BindTexture) and which is called (the Render_* functions). */
#define SUBMIT_BODY(SETFLAGS, FLATDRAW, CLEARFLAGS, BIND, TEXDRAW)                                                     \
    switch (poly->type) {                                                                                              \
        case RPOLY_OPAQUE:                                                                                             \
            if (flatBatchCount <= batchCapacity) {                                                                     \
                /* cast kept: the flat batch is raw vertex memory, 3 vertices of 0x18 bytes (FVF 0xc4) per triangle */ \
                memcpy((u8 *)flatBatchVerts + flatBatchCount * 3 * 0x18, poly->verts, 0x48);                           \
                flatBatchCount++;                                                                                      \
            }                                                                                                          \
            if (flatBatchCount >= batchCapacity) {                                                                     \
                renderer->SETFLAGS(stateFlags);                                                                        \
                renderer->FLATDRAW(D3DPT_TRIANGLELIST, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR,               \
                                   flatBatchVerts, batchCapacity * 3);                                                 \
                renderer->CLEARFLAGS(stateFlags);                                                                      \
                flatBatchCount = 0;                                                                                    \
                textureDirty = 1;                                                                                      \
            }                                                                                                          \
            break;                                                                                                     \
        case RPOLY_BLEND:                                                                                              \
            if (computeSortZ == 1)                                                                                     \
                poly->sortZ = poly->verts[2] + poly->verts[8] + poly->verts[14];                                       \
            sortedPolys[sortedCount].Assign(poly);                                                                     \
            sortedList[sortedCount] = &sortedPolys[sortedCount];                                                       \
            sortedCount++;                                                                                             \
            break;                                                                                                     \
        case RPOLY_ADD:                                                                                                \
            if (computeSortZ == 1)                                                                                     \
                poly->sortZ = poly->verts[2] + poly->verts[8] + poly->verts[14];                                       \
            sortedPolys[sortedCount].Assign(poly);                                                                     \
            sortedList[sortedCount] = &sortedPolys[sortedCount];                                                       \
            sortedCount++;                                                                                             \
            break;                                                                                                     \
        default: {                                                                                                     \
            u32 idx; /* (the names give the expansions' stack slots) */                                                \
            float *aV;                                                                                                 \
            aV = poly->verts;                                                                                          \
            idx = (poly->type - RPOLY_TEXTURED_BASE) & ~RPOLY_F_8000;                                                  \
            if (idx < immediateTexCount) {                                                                             \
                /* cast kept: vertex memory is untyped: its vertex format (FVF) decides the vertex struct */           \
                TlVertex *buf = (TlVertex *)texBatchVerts[idx];                                                        \
                u32 *pCount = &texBatchCounts[idx];                                                                    \
                u32 *flags = &texStateFlags[idx];                                                                      \
                if (*pCount <= batchCapacity) {                                                                        \
                    memcpy(buf + *pCount * 3, aV, 0x60);                                                               \
                    (*pCount)++;                                                                                       \
                }                                                                                                      \
                if (*pCount >= batchCapacity) {                                                                        \
                    if (idx != lastTextureIndex || textureDirty == 1) {                                                \
                        renderer->BIND(textures[idx], 0);                                                              \
                        lastTextureIndex = idx;                                                                        \
                        textureDirty = 0;                                                                              \
                    }                                                                                                  \
                    renderer->Render_SetStateFlags(*flags);                                                            \
                    renderer->TEXDRAW(D3DPT_TRIANGLELIST,                                                              \
                                      D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1, buf,             \
                                      batchCapacity * 3);                                                              \
                    renderer->Render_ClearStateFlags(*flags);                                                          \
                    *pCount = 0;                                                                                       \
                }                                                                                                      \
            } else {                                                                                                   \
                if (computeSortZ == 1)                                                                                 \
                    poly->sortZ = aV[2] + aV[10] + aV[18];                                                             \
                sortedPolys[sortedCount].Assign(poly);                                                                 \
                sortedList[sortedCount] = &sortedPolys[sortedCount];                                                   \
                sortedCount++;                                                                                         \
            }                                                                                                          \
            break;                                                                                                     \
        }                                                                                                              \
    }
inline void PolyBatcher::AddPolyA(RenderPoly *poly) /* Weather_DrawRain, first triangle */
{
    SUBMIT_BODY(SetStateFlagsInline, DrawPrimitiveInline, Render_ClearStateFlags, BindTexture, DrawPrimitiveInline)
}
inline void PolyBatcher::AddPolyB(RenderPoly *poly) /* Weather_DrawRain, second triangle */
{
    SUBMIT_BODY(Render_SetStateFlags, DrawPrimitiveInline, ClearStateFlagsInline, BindTexture, DrawPrimitiveInline)
}
inline void PolyBatcher::AddPolyC(RenderPoly *poly) /* Weather_DrawSnow, first triangle */
{
    SUBMIT_BODY(SetStateFlagsInline, DrawPrimitiveInline, Render_ClearStateFlags, Render_SetTexture,
                Render_DrawPrimitive)
}
inline void PolyBatcher::AddPolyD(RenderPoly *poly) /* Weather_DrawSnow, second triangle */
    {SUBMIT_BODY(Render_SetStateFlags, DrawPrimitiveInline, Render_ClearStateFlags, Render_SetTexture,
                 Render_DrawPrimitive)}

/* g_weather 0x6d8318, the one weather object, and its four static-initialiser thunks (0x52df80-0x52dfb0) are T267's
 * (src/engine/weather.cpp). */

/* 0x52e1e0 */
Weather::Weather()
{
    hasTex2 = 0;
    vbXf = 0;
    vb = 0;
}

/* 0x52e259 */
Weather::~Weather() {}

/* 0x52e27b - the sphere the particles live in, and (re)creates the vertex buffers for count quads (at most 500). */
void Weather::SetVolume(float halfWidth, float radius_, u32 count_)
{
    radius = radius_;
    radiusSq = radius * radius;
    centre.y = 0.0f;
    centre.x = 0.0f;
    centre.z = (float)sqrt(radiusSq - halfWidth * halfWidth) + g_pViewFrustum->nearZ;
    count = count_;
    if (count > 500)
        count = 500;
    CreateVertexBuffers(count * 4);
}

/* 0x52e322 - the first particle texture: the bitmap of id list resId, its four corner UVs, the quad size and colour.
 * Returns 0 when the level has no such bitmap. */
u8 Weather::SetTexture(u16 resId, float width, float height, u32 color)
{
    u32 kV_;
    void **kList;
    u32 aU;
    u16 *kId;
    u16 aCounts;
    u32 pH;
    DavBitmapRec *aRec;
    u32 w;

    /* cast kept: an id list holds untyped record pointers; the caller knows the kind it asked for */
    kList = (void **)Res_GetValidatedIdList(resId, &aCounts);
    if (kList && aCounts) {
        /* cast kept: an id list holds untyped record pointers; the caller knows the kind it asked for */
        kId = (u16 *)*kList;
        aRec = &g_pDav->header->dir->bitmaps[*kId];
        w = aRec->width;
        pH = aRec->height;
        aU = aRec->u;
        kV_ = aRec->v;
        tex.page = aRec->page;
        tex.uv[0] = Tex_CornerUV(0, w, aU);
        tex.uv[1] = Tex_CornerUV(0, pH, kV_);
        tex.uv[2] = Tex_CornerUV(w, w, aU);
        tex.uv[3] = Tex_CornerUV(0, pH, kV_);
        tex.uv[4] = Tex_CornerUV(0, w, aU);
        tex.uv[5] = Tex_CornerUV(pH, pH, kV_);
        tex.uv[6] = Tex_CornerUV(w, w, aU);
        tex.uv[7] = Tex_CornerUV(pH, pH, kV_);
        tex.width = width;
        tex.height = height;
        tex.color = color & 0xffffff;
        return 1;
    }
    return 0;
}

/* 0x52e4a0 - as SetTexture, for the second texture, and marks it present. */
u8 Weather::SetTexture2(u16 resId, float width, float height, u32 color)
{
    u32 kV_;
    void **kList;
    u32 aU;
    u16 *kId;
    u16 aCounts;
    u32 pH;
    DavBitmapRec *aRec;
    u32 w;

    /* cast kept: an id list holds untyped record pointers; the caller knows the kind it asked for */
    kList = (void **)Res_GetValidatedIdList(resId, &aCounts);
    if (kList && aCounts) {
        /* cast kept: an id list holds untyped record pointers; the caller knows the kind it asked for */
        kId = (u16 *)*kList;
        aRec = &g_pDav->header->dir->bitmaps[*kId];
        w = aRec->width;
        pH = aRec->height;
        aU = aRec->u;
        kV_ = aRec->v;
        tex2.page = aRec->page;
        tex2.uv[0] = Tex_CornerUV(0, w, aU);
        tex2.uv[1] = Tex_CornerUV(0, pH, kV_);
        tex2.uv[2] = Tex_CornerUV(w, w, aU);
        tex2.uv[3] = Tex_CornerUV(0, pH, kV_);
        tex2.uv[4] = Tex_CornerUV(0, w, aU);
        tex2.uv[5] = Tex_CornerUV(pH, pH, kV_);
        tex2.uv[6] = Tex_CornerUV(w, w, aU);
        tex2.uv[7] = Tex_CornerUV(pH, pH, kV_);
        tex2.width = width;
        tex2.height = height;
        tex2.color = color & 0xffffff;
        hasTex2 = 1;
        return 1;
    }
    return 0;
}

/* 0x52e625 - scatter the particles uniformly in angle over the sphere (radius uniform, so denser near the centre) and
 * give each a velocity: dir turned by up to +-spread about two axes, scaled to a speed in [speedMin, speedMax]. Seeds
 * the CRT generator from the clock, so the pattern differs on every level load. */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py) */
void Weather::SfxParticles_InitRandom(Vec3f *dir, float spread, float speedMin, float speedMax)
{
    u32 i;

    srand(time(0));
    for (i = 0; i < count; i++) {
        float pitch; /* the names give the original's stack slots (tools/vc6_locals.py) */
        float aC;
        Vec3f vec;
        float aScale;
        Mat44 aRot;
        float r;
        float a;

        a = (float)rand() / 32767.0f * 3.14159265f * 2.0f;
        pitch = (float)rand() / 32767.0f * 3.14159265f * 2.0f;
        r = (float)rand() / 32767.0f * radius;
        aRot.Mat44_SetRotZYX(a, pitch, 0.0f);
        particles[i].pos.x = r * aRot.m[2][0];
        particles[i].pos.y = r * aRot.m[2][1];
        particles[i].pos.z = r * aRot.m[2][2];
        particles[i].pos.z += centre.z;
        particles[i].viewPos.x = particles[i].pos.x;
        particles[i].viewPos.y = particles[i].pos.y;
        particles[i].viewPos.z = particles[i].pos.z;
        a = (float)rand() / 32767.0f * spread * 2.0f;
        aC = (float)rand() / 32767.0f * spread * 2.0f;
        r = (float)rand() / 32767.0f * (speedMax - speedMin) + speedMin;
        aRot.Mat44_SetRotZYX(spread - a, 0.0f, spread - aC);
        aRot.TransformPoint(dir, &vec);
        aScale = r / (float)sqrt(VEC3F_LENSQ(vec));
        particles[i].vel.x = vec.x * aScale;
        particles[i].vel.y = vec.y * aScale;
        particles[i].vel.z = vec.z * aScale;
        particles[i].inAir = 1;
        particles[i].alpha = 1.0f;
    }
}

/* 0x52e8bf - move every particle by its velocity over this frame, then carry the field along with the camera. */
void Weather::Advance(Mat44 *view)
{
    u32 n;
    float dt;

    dt = (float)g_dtMs / 1000.0f;
    for (n = 0; n < count; n++) {
        particles[n].pos.x += dt * particles[n].vel.x;
        particles[n].pos.y += dt * particles[n].vel.y;
        particles[n].pos.z += dt * particles[n].vel.z;
    }
    WrapToVolume(view);
}

/* 0x52e9a5 - rain. Each drop is a streak quad: its two long sides run along the fall direction, offset sideways by a
 * vector perpendicular to the view direction (cullNormal) and to the vertical, half the first texture's width long,
 * and it is the first texture's height tall. A drop inside a water box (not a sink-through box, flag 0x2000000) is
 * cut at the surface instead: it is drawn flat on the surface with the second texture, as a splash that grows from
 * the drop's size to twice the second texture's size and fades out as the drop sinks for the time it takes to fall
 * half a unit per unit of speed below it. The quads are written to the XYZ buffer, transformed by ProcessVertices,
 * and each is pushed through g_weather.poly as two triangles. */
/* a colour word whose channel bytes are also read and written one by one */
union ColorBytes {
    u32 value;
    u8 c[4];
};

/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py) */
void Weather::DrawRain(Mat44 *view, float *cullNormal)
{
    WeatherTex *tx; /* the names give the original's stack slots (tools/vc6_locals.py) */
    ColorBytes rgb;
    Vec3f aDrop;
    float pT;
    Vec3f tSpread;
    Vec3s pt;
    Box *box;
    float k;
    u32 i;
    D3DXyzrhwVertex *proj;
    Vec3f splash;
    u32 aSize;
    Vec3f *out;
    Vec3f width;
    Mat44 mat;
    void *locked;
    TlVertex *tri;

    width.x = cullNormal[2];
    width.y = 0.0f;
    width.z = -cullNormal[0];
    k = tex.width / ((float)sqrt(VEC3F_LENSQ(width)) * 2.0f);
    width.x *= k;
    width.z *= k;
    width.y = tex.height / 2.0f;
    splash.x = tex2.width / 2.0f;
    splash.z = tex2.height / 2.0f;
    splash.y = 0.0f;
    vb->Lock(DDLOCK_WAIT, &locked, &aSize);
    out = (Vec3f *)locked; /* cast kept: vertex memory is untyped: its vertex format (FVF) decides the vertex struct */
    for (i = 0; i < count; i++) {
        box = 0;
        pt.x = (s16)particles[i].pos.x;
        pt.y = (s16)particles[i].pos.y;
        pt.z = (s16)particles[i].pos.z;
        box = Zones_Get(ZONE_WATER)->FindBelowTop(&pt);
        if (!box || (box->flags & ZONE_WATER_NO_SURFACE)) {
            particles[i].inAir = 1;
            particles[i].alpha = 1.0f;
            out[0].x = out[2].x = particles[i].pos.x - width.x;
            out[0].y = out[1].y = particles[i].pos.y - width.y;
            out[0].z = out[2].z = particles[i].pos.z - width.z;
            out[1].x = out[3].x = width.x + particles[i].pos.x;
            out[2].y = out[3].y = width.y + particles[i].pos.y;
            out[1].z = out[3].z = width.z + particles[i].pos.z;
        } else {
            particles[i].inAir = 0;
            pT = (particles[i].pos.y - box->min[1]) / particles[i].vel.y;
            aDrop.x = pT * particles[i].vel.x;
            aDrop.z = pT * particles[i].vel.z;
            if (pT >= 0.5f) {
                tSpread.x = tSpread.z = 0.0f;
            } else {
                tSpread.x = (2.0f * pT + 1.0f) * splash.x;
                tSpread.z = (2.0f * pT + 1.0f) * splash.z;
                particles[i].alpha = 1.0f - 2.0f * pT;
            }
            out[0].x = out[2].x = particles[i].pos.x - aDrop.x - tSpread.x;
            out[0].z = out[1].z = particles[i].pos.z - aDrop.z - tSpread.z;
            out[1].x = out[3].x = particles[i].pos.x - aDrop.x + tSpread.x;
            out[2].z = out[3].z = particles[i].pos.z - aDrop.z + tSpread.z;
            out[0].y = out[1].y = out[2].y = out[3].y = box->min[1];
        }
        out += 4;
    }
    vb->Unlock();
    mat.Mat44_SetIdentity();
    g_pD3DAppMain->SetTransform(D3DTRANSFORMSTATE_WORLD, &mat);
    g_pD3DAppMain->SetTransform(D3DTRANSFORMSTATE_VIEW, view);
    vbXf->ProcessVertices(D3DVOP_TRANSFORM, 0, count * 4, vb, 0, g_pD3DAppMain->GetDevice(), D3DPV_DONOTCOPYDATA);
    vbXf->Lock(DDLOCK_WAIT, &locked, &aSize);
    /* cast kept: vertex memory is untyped: its vertex format (FVF) decides the vertex struct */
    proj = (D3DXyzrhwVertex *)locked;
    /* cast kept: RenderPoly.verts is a float array; this poly's vertices are TlVertex records (FVF 0x1c4) */
    tri = (TlVertex *)poly.verts;
    for (i = 0; i < count; i++) {
        if (particles[i].inAir == 1)
            tx = &tex;
        else
            tx = &tex2;
        if (tx == &tex || hasTex2 == 1) {
            rgb.c[0] = (u8)(tx->colorBytes[0] * particles[i].alpha);
            rgb.c[1] = (u8)(tx->colorBytes[1] * particles[i].alpha);
            rgb.c[2] = (u8)(tx->colorBytes[2] * particles[i].alpha);
            poly.sortZ = proj[0].z * 3.0f;
            poly.type = tx->page + RPOLY_TEXTURED_BASE;
            tri[2].specular = 0xff000000;
            tri[1].specular = 0xff000000;
            tri[0].specular = 0xff000000;
            tri[2].diffuse = rgb.value;
            tri[1].diffuse = rgb.value;
            tri[0].diffuse = rgb.value;
            memcpy(&tri[0], &proj[0], sizeof(D3DXyzrhwVertex));
            memcpy(&tri[1], &proj[2], sizeof(D3DXyzrhwVertex));
            memcpy(&tri[2], &proj[1], sizeof(D3DXyzrhwVertex));
            tri[0].u = tx->uv[0];
            tri[0].v = tx->uv[1];
            tri[1].u = tx->uv[4];
            tri[1].v = tx->uv[5];
            tri[2].u = tx->uv[2];
            tri[2].v = tx->uv[3];
            g_pPolyBin->AddPolyA(&poly);
            memcpy(&tri[0], &proj[1], sizeof(D3DXyzrhwVertex));
            memcpy(&tri[2], &proj[3], sizeof(D3DXyzrhwVertex));
            tri[0].u = tx->uv[2];
            tri[0].v = tx->uv[3];
            tri[2].u = tx->uv[6];
            tri[2].v = tx->uv[7];
            g_pPolyBin->AddPolyB(&poly);
        }
        proj += 4;
    }
    vbXf->Unlock();
}

/* 0x53049a - snow. Each flake is a camera-facing quad of the first texture's size at its projected position (its
 * screen size scaled by the projection and rhw, so it shrinks with distance), drawn as two triangles. A flake inside a
 * water box is held at the surface and fades out over the time it takes to sink half a unit per unit of speed; the
 * fade also shrinks the quad (the size is scaled by the alpha). */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py) */
/* BYTES(dead-code): pUnused is never used: it fills a 12-byte hole in the original frame at ebp-0x70 */
void Weather::DrawSnow(Mat44 *view)
{
    float hw; /* the names give the original's stack slots (tools/vc6_locals.py) */
    WeatherTex *aTx;
    float aHh;
    Vec3f aDrop;
    float pDepth;
    Vec3f pUnused; /* never used: a 12-byte hole in the original's frame at ebp-0x70 */
    Vec3s pt;
    Box *box;
    u32 i;
    D3DXyzrhwVertex *proj;
    u32 aSize;
    Vec3f *out;
    Mat44 mat;
    void *locked;
    TlVertex *tri;

    vb->Lock(DDLOCK_WAIT, &locked, &aSize);
    out = (Vec3f *)locked; /* cast kept: vertex memory is untyped: its vertex format (FVF) decides the vertex struct */
    for (i = 0; i < count; i++) {
        box = 0;
        pt.x = (s16)particles[i].pos.x;
        pt.y = (s16)particles[i].pos.y;
        pt.z = (s16)particles[i].pos.z;
        box = Zones_Get(ZONE_WATER)->FindBelowTop(&pt);
        if (!box) {
            particles[i].inAir = 1;
            particles[i].alpha = 1.0f;
            memcpy(out, &particles[i].pos, sizeof(Vec3f));
        } else {
            particles[i].inAir = 0;
            pDepth = (particles[i].pos.y - box->min[1]) / particles[i].vel.y;
            aDrop.x = pDepth * particles[i].vel.x;
            aDrop.z = pDepth * particles[i].vel.z;
            if (pDepth >= 0.5f)
                particles[i].alpha = 0.0f;
            else
                particles[i].alpha = 1.0f - 2.0f * pDepth;
            out->x = particles[i].pos.x - aDrop.x;
            out->z = particles[i].pos.z - aDrop.z;
            out->y = box->min[1];
        }
        out++;
    }
    vb->Unlock();
    mat.Mat44_SetIdentity();
    g_pD3DAppMain->SetTransform(D3DTRANSFORMSTATE_WORLD, &mat);
    g_pD3DAppMain->SetTransform(D3DTRANSFORMSTATE_VIEW, view);
    vbXf->ProcessVertices(D3DVOP_TRANSFORM, 0, count, vb, 0, g_pD3DAppMain->GetDevice(), D3DPV_DONOTCOPYDATA);
    vbXf->Lock(DDLOCK_WAIT, &locked, &aSize);
    /* cast kept: vertex memory is untyped: its vertex format (FVF) decides the vertex struct */
    proj = (D3DXyzrhwVertex *)locked;
    /* cast kept: RenderPoly.verts is a float array; this poly's vertices are TlVertex records (FVF 0x1c4) */
    tri = (TlVertex *)poly.verts;
    for (i = 0; i < count; i++) {
        if (particles[i].inAir == 1)
            aTx = &tex;
        else
            aTx = &tex2;
        if (aTx == &tex || hasTex2 == 1) {
            poly.sortZ = proj->z * 3.0f;
            poly.type = aTx->page + RPOLY_TEXTURED_BASE;
            tri[2].specular = 0xff000000;
            tri[1].specular = 0xff000000;
            tri[0].specular = 0xff000000;
            tri[0].diffuse = tri[1].diffuse = tri[2].diffuse = aTx->color;
            hw = g_projMatrix.m[0][0] * aTx->width * proj->rhw * g_screenW / 4.0f;
            aHh = -aTx->height * g_projMatrix.m[1][1] * proj->rhw * g_screenH / 4.0f;
            hw *= particles[i].alpha;
            aHh *= particles[i].alpha;
            tri[0].z = tri[1].z = tri[2].z = proj->z;
            tri[0].rhw = tri[1].rhw = tri[2].rhw = proj->rhw;
            tri[0].x = proj->x - hw;
            tri[0].y = proj->y - aHh;
            tri[1].x = proj->x - hw;
            tri[1].y = aHh + proj->y;
            tri[2].x = hw + proj->x;
            tri[2].y = proj->y - aHh;
            tri[0].u = aTx->uv[0];
            tri[0].v = aTx->uv[1];
            tri[1].u = aTx->uv[4];
            tri[1].v = aTx->uv[5];
            tri[2].u = aTx->uv[2];
            tri[2].v = aTx->uv[3];
            g_pPolyBin->AddPolyC(&poly);
            tri[0].x = hw + proj->x;
            tri[0].y = proj->y - aHh;
            tri[2].x = hw + proj->x;
            tri[2].y = aHh + proj->y;
            tri[0].u = aTx->uv[2];
            tri[0].v = aTx->uv[3];
            tri[2].u = aTx->uv[6];
            tri[2].v = aTx->uv[7];
            g_pPolyBin->AddPolyD(&poly);
        }
        proj++;
    }
    vbXf->Unlock();
}

/* 0x531a08 - release and recreate the two vertex buffers: the XYZ source and the XYZRHW ProcessVertices target. */
u8 Weather::CreateVertexBuffers(u32 n)
{
    D3DVERTEXBUFFERDESC desc;

    if (vb) {
        vb->Release();
        vb = 0;
    }
    if (vbXf) {
        vbXf->Release();
        vbXf = 0;
    }
    if (!vb) {
        memset(&desc, 0, sizeof(desc));
        desc.dwSize = sizeof(desc);
        desc.dwCaps = D3DVBCAPS_DONOTCLIP;
        desc.dwFVF = D3DFVF_XYZ;
        desc.dwNumVertices = n;
        if (g_pD3DAppMain->CreateVBWithResult(&desc, &vb) < 0)
            return 0;
    }
    if (!vbXf) {
        memset(&desc, 0, sizeof(desc));
        desc.dwSize = sizeof(desc);
        desc.dwCaps = D3DVBCAPS_DONOTCLIP;
        desc.dwFVF = D3DFVF_XYZRHW;
        desc.dwNumVertices = n;
        if (g_pD3DAppMain->CreateVBWithResult(&desc, &vbXf) < 0)
            return 0;
    }
    return 1;
}

/* 0x531bdd - move each particle into camera space; one that has left the sphere is mirrored through the centre from
 * its last camera-space position and put back into world space (the inverse of the view rotation, then the camera
 * translation); one that is still inside just records its camera-space position. */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py) */
void Weather::WrapToVolume(Mat44 *view)
{
    u32 n; /* the names give the original's stack slots (tools/vc6_locals.py) */
    Vec3f aWrap;
    Vec3f local;
    Mat44 inv;
    Vec3f d;
    Vec3f eye;
    float r;

    view->Transpose3x3(&inv);
    inv.m[3][0] = inv.m[3][1] = inv.m[3][2] = 0.0f;
    eye.x = -view->m[3][0];
    eye.y = -view->m[3][1];
    eye.z = -view->m[3][2];
    for (n = 0; n < count; n++) {
        local.x = view->m[0][0] * particles[n].pos.x + view->m[1][0] * particles[n].pos.y +
                  view->m[2][0] * particles[n].pos.z + view->m[3][0];
        local.y = view->m[0][1] * particles[n].pos.x + view->m[1][1] * particles[n].pos.y +
                  view->m[2][1] * particles[n].pos.z + view->m[3][1];
        local.z = view->m[0][2] * particles[n].pos.x + view->m[1][2] * particles[n].pos.y +
                  view->m[2][2] * particles[n].pos.z + view->m[3][2];
        d.x = centre.x - local.x;
        d.y = centre.y - local.y;
        d.z = centre.z - local.z;
        r = (float)sqrt(VEC3F_LENSQ(d));
        if (r > radius) {
            aWrap.x = 2.0f * centre.x - particles[n].viewPos.x;
            aWrap.y = 2.0f * centre.y - particles[n].viewPos.y;
            aWrap.z = 2.0f * centre.z - particles[n].viewPos.z;
            aWrap.x += eye.x;
            aWrap.y += eye.y;
            aWrap.z += eye.z;
            particles[n].pos.x = inv.m[0][0] * aWrap.x + inv.m[1][0] * aWrap.y + inv.m[2][0] * aWrap.z + inv.m[3][0];
            particles[n].pos.y = inv.m[0][1] * aWrap.x + inv.m[1][1] * aWrap.y + inv.m[2][1] * aWrap.z + inv.m[3][1];
            particles[n].pos.z = inv.m[0][2] * aWrap.x + inv.m[1][2] * aWrap.y + inv.m[2][2] * aWrap.z + inv.m[3][2];
        } else {
            particles[n].viewPos.x = local.x;
            particles[n].viewPos.y = local.y;
            particles[n].viewPos.z = local.z;
        }
    }
}
