/* T273 - original object "HoleFX.cpp" (guessed name); the two empty functions after it (0x538df0 / 0x538df5) are T274,
 * src/fx/hole_fx_stub.cpp.
 *   .text  0x5363c0-0x538db4  HoleFX ctor .. Color_GreyFromScalar (13), then COMDAT ??_GHoleFX 0x538dc0
 *   .rdata 0x577150-0x57716c  ??_7HoleFX, 4 bytes of alignment, then the float constants it is first to use
 *                             (__real@4010000000000000, __real@42480000, __real@c0000000, __real@c0400000)
 *   .data  0x57c008-0x57c038  the two $SG strings of HoleFX_Init's error box
 */
/* BYTES: cast, dead-code, inline, slot-name, slot-scope. */
/* BYTES(inline): HoleVertex() {}: empty user-declared constructor: gives the original's empty constructor loop for the unused array (0x53779c) */
/* BYTES(inline): Vec3f() {} (member macro): empty user-declared constructor: gives the original's loop over rayDirs */
/* BYTES(inline): Vec3f::Length (member-macro inline): source-only inline: its float result goes through a temp (fst, 0x536904 / 0x536b88) */
/* BYTES(inline): D3DApp::CreateVBWithResult (member-macro inline): source-only inline: its expansion gives the original's temporaries */
/* BYTES(inline): D3DApp::GetDevice (member-macro inline): source-only inline: its return value goes through two stack temps (0x536e7e), as the original */
/* BYTES(inline): D3DApp::Render_SetStateFlags / ClearStateFlagsInline (inline twins): source-only inline twin of 0x4155f0 / 0x4159b0: expanded with the constant flags materialised per test */
/* BYTES(cast): D3DApp::DrawTexTriangleList (inline): source-only inline: the unsigned count decides which argument gets a temporary (0x537473) */
/* BYTES(slot-scope): HoleFX::CubicControlPoint (member-macro inline): source-only inline: its locals are inline temporaries allocated after the Length temp (0x5368b8), which nested-block locals would not be */
/*
 * HoleFX, SheepD3D.exe 0x5363c0-0x538df9: the "iris" level-transition effect, the circle that closes on Ralph at the
 * end of a level and opens on the next one (g_holeFX 0x6d4138, driven by the Transition_* functions at 0x515f9c).
 * Two modes, chosen once in HoleFX_Init by whether an off-screen copy of the frame can be created:
 *  - blocking (hasCaptureSurface == HOLEFX_MESH): the frame is frozen into captureSurface and drawn as a 65-vertex
 *    radial mesh (8 rays x 8 Bezier samples around the hole centre, 120 textured triangles). The mesh is bent by
 *    quadratic Bezier curves (cubic ones when useCubicBezier is set), and each vertex's grey level comes from its
 *    curve's tangent, so the frozen picture looks sucked into the hole.
 *  - non-blocking (HOLEFX_MASK): the game keeps running and a 256x256 ARGB4444 circle mask (maskTexture) is drawn as
 *    four mirrored quads around the hole, with black flat rectangles covering the rest of the screen.
 * The file also holds the Bezier curve evaluators and the tangent-to-grey colour ramp, which are HoleFX methods (ecx
 * is loaded at every call and the colour ramp reads shadeScale / shadeBias).
 *
 * RenderPoly's +8..+0x17 is the embedded PolyTri whose three vertex indices HoleFX_BuildMesh writes through &poly
 * (0x538564). The constructor settles /Ob1 and /GX-; the destructor 0x538dc0 is placed
 * by the vtable slot. VC6 also emits the ??_H / ??_I vector iterators, which nothing here calls (the loops are
 * inlined) and the original's linker dropped.
 *
 * The inline helpers have no bodies in the exe, so their names are not recovered; each is there because its expansion
 * gives the original's shapes, as in src/objects/shark.cpp:
 *  - D3DApp::Render_SetStateFlags / ClearStateFlagsInline: the inline twins of 0x4155f0 / 0x4159b0, expanded with the
 *    constant flags materialised per test (the non-blocking branch of HoleFX_Draw calls the out-of-line 0x4159b0).
 *  - D3DApp::GetDevice: an inline whose return value goes through two stack temporaries, the first copied from an
 *    uninitialised one (0x536e7e; the same shape as Splash_Render 0x474d38 in src/engine/shadow.cpp).
 *  - D3DApp::BindTexture (texture first, signed stage) and DrawTexTriangleList (unsigned count): the parameter types
 *    and order are what decide which arguments get temporaries (0x537844, 0x537473).
 *  - D3DApp::CreateVBWithResult: the DirectX SDK samples' "system memory unless TnL HAL" idiom (as in src/objects/bipbip.cpp),
 *    here returning the HRESULT.
 *  - Vec3f::Length: its float result is a temporary (fst, 0x536904 / 0x536b88) unless assigned straight to a local.
 *  - HoleFX::CubicControlPoint: the cubic branch of UpdateVertices; as an inline its two locals are allocated after
 *    the Length temporary (0x5369ca), which named locals of a nested block are not.
 * Local names are chosen for their stack slots (tools/vc6_locals.py). A shape that reproduces the bytes is a
 * representation, not proof that the original source read this way.
 */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "../sdk/d3d7.h"
#include "../sdk/ddraw.h"
#include "../sdk/win32.h"
class Mat44;
#include "../sdk/crt.h"
#define IsEqualGUID(a, b) (!memcmp((a), (b), 16)) /* sizeof(GUID); the type is incomplete here */

/* The two vertex formats: the mesh's own vertices in the vertex buffer (XYZRHW | DIFFUSE | SPECULAR, 0x18 bytes), and
 * the textured ones in each RenderPoly's vertex block (plus TEX1, 0x20 bytes). The empty constructor is what makes the
 * unused array in HoleFX_Draw's non-blocking branch an (empty) constructor loop (0x53779c). */
struct HoleVertex {
    HoleVertex() {}
    float x, y, z, rhw;
    u32 diffuse, specular;
};
struct HoleTlVertex {
    float x, y, z, rhw;
    u32 diffuse, specular;
    float u, v;
};

#define SDW_MEMBERS_Vec3f                                                                       \
    Vec3f() {} /* empty but user-declared: this is what makes the original loop over rayDirs */ \
    float Length()                                                                              \
    {                                                                                           \
        return sqrt(x * x + y * y + z * z);                                                     \
    } /* inline: its float result is a temp (fst) */
#define SDW_MEMBERS_RenderPoly                              \
    RenderPoly();                            /* 0x41aad0 */ \
    /* virtual ~RenderPoly() is generated */ /* 0x41ac04 */
#define SDW_MEMBERS_Texture                                                              \
    Texture(D3DApp *app, u32 width, u32 height, u32 format, s32 *result); /* 0x40a140 */ \
    long Surface_LockForWrite(DDSURFACEDESC2 *desc); /* 0x40ae3f; returns long, as T009 defines it */
#define SDW_MEMBERS_Mat44 Mat44();                   /* 0x4077f0 */
#define SDW_MEMBERS_D3DApp                                                                      \
    long CreateVBWithResult(D3DVERTEXBUFFERDESC *desc, IDirect3DVertexBuffer7 **out);           \
    void BindTexture(Texture *tex, s32 stage);                                                  \
    void DrawTexTriangleList(void *verts, u32 count);                                           \
    void Render_ClearStateFlags(u32 flags);                                                     \
    IDirect3DDevice7 *GetDevice();                                                              \
    /* inline twin of Render_SetStateFlags 0x4155f0 (as src/objects/shark.cpp) */               \
    void Render_SetStateFlags(u32 flags)                                                        \
    {                                                                                           \
        if (flags & RSF_ANTIALIAS)                                                              \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_ANTIALIAS, D3DANTIALIAS_SORTINDEPENDENT); \
        if (flags & RSF_BLEND_ALPHA) {                                                          \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);                  \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_INVSRCALPHA);          \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_SRCALPHA);            \
        }                                                                                       \
        if (flags & RSF_BLEND_ADD) {                                                            \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);                  \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_INVSRCALPHA);          \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);                 \
        }                                                                                       \
        if (flags & RSF_ALPHATEST) {                                                            \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, TRUE);                   \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHAREF, 8);                             \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHAFUNC, D3DCMP_LESSEQUAL);             \
        }                                                                                       \
        if (flags & RSF_CLIPPLANE)                                                              \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_CLIPPLANEENABLE, TRUE);                   \
        if (flags & RSF_DITHER)                                                                 \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_DITHERENABLE, TRUE);                      \
        if (flags & RSF_LIGHTING)                                                               \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_LIGHTING, TRUE);                          \
        if (flags & RSF_SPECULAR)                                                               \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_SPECULARENABLE, TRUE);                    \
        if (flags & RSF_COLORVERTEX)                                                            \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_COLORVERTEX, TRUE);                       \
        if (flags & RSF_CULL_CW)                                                                \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_CW);                    \
        if (flags & RSF_CULL_CCW)                                                               \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_CCW);                   \
        if (flags & RSF_ZTEST)                                                                  \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, D3DZB_TRUE);                     \
        if (flags & RSF_ZWRITE_ON)                                                              \
            if (pD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, TRUE))                  \
                pD3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, TRUE);                       \
        if (flags & RSF_ZWRITE_OFF)                                                             \
            if (pD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, FALSE))                 \
                pD3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, TRUE);                       \
        if (flags & RSF_TEXTURED) {                                                             \
            pD3DDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);                       \
            pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);               \
            pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);               \
            pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);               \
        } else {                                                                                \
            pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);               \
            pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);             \
        }                                                                                       \
        if (flags & RSF_FILTER_LINEAR) {                                                        \
            pD3DDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_LINEAR);               \
            pD3DDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFN_LINEAR);               \
        }                                                                                       \
        if (flags & RSF_FOG)                                                                    \
            pD3DDevice->SetRenderState(D3DRENDERSTATE_FOGENABLE, TRUE);                         \
    }                                                                                           \
    void ClearStateFlagsInline(u32 flags);
#define SDW_MEMBERS_HoleFX                                                                                \
    HoleFX(); /* 0x5363c0 */                                                                              \
    /* inline: the cubic curve's extra control point, the curve's start moved toward the hole centre by \
     * viewportHeight / 50 (its locals are inline temporaries, after the Length temp at 0x5368b8) */ \
    void CubicControlPoint(Vec3f *ctrl, Vec3f *from, Vec3f *d)                                            \
    {                                                                                                     \
        float dlen;                                                                                       \
        float kk;                                                                                         \
        d->x = pos[0] - from->x;                                                                          \
        d->y = pos[1] - from->y;                                                                          \
        d->z = pos[2] - from->z;                                                                          \
        dlen = d->Length();                                                                               \
        kk = viewport->viewportHeight / (50.0f * dlen);                                                   \
        d->x = d->x * kk;                                                                                 \
        d->y = d->y * kk;                                                                                 \
        d->z = d->z * kk;                                                                                 \
        ctrl->x = from->x + d->x;                                                                         \
        ctrl->y = from->y + d->y;                                                                         \
        ctrl->z = from->z + d->z;                                                                         \
    }
#include "sdw_classes.h"
#define SDW_INLINE_D3DAPP_CREATEVBWITHRESULT_D3DVERTEXBUFFERDESC_IDIRECT3DVERTEXBUFFER7 2
#define SDW_INLINE_D3DAPP_DRAWTEXTRIANGLELIST_VOID_U32 1
#define SDW_INLINE_D3DAPP_GETDEVICE 1
#include "../app/d3dapp_inlines.h"
#undef SDW_INLINE_D3DAPP_CREATEVBWITHRESULT_D3DVERTEXBUFFERDESC_IDIRECT3DVERTEXBUFFER7
#undef SDW_INLINE_D3DAPP_DRAWTEXTRIANGLELIST_VOID_U32
#undef SDW_INLINE_D3DAPP_GETDEVICE
#define SDW_INLINE_D3DAPP_CLEARSTATEFLAGSINLINE_U32 1
#include "../app/d3dapp_inlines.h"
#undef SDW_INLINE_D3DAPP_CLEARSTATEFLAGSINLINE_U32

/* inline: the texture's surface as stage `stage` (Texture is not complete inside the D3DApp member list). The
 * argument order (texture first) and the signed stage are what give the original's temporaries (0x537844). */
/* BYTES(cast): source-only inline: texture first and a signed stage, which give the original's temporaries (0x537844) */
#define SDW_INLINE_D3DAPP_BINDTEXTURE_TEXTURE_S32 1
#include "../app/d3dapp_inlines.h"
#undef SDW_INLINE_D3DAPP_BINDTEXTURE_TEXTURE_S32

#include "../engine/draw2d.h"
#include "../engine/screen.h"
extern u32 *g_screenLayerBase; /* 0x585044 */
void Draw2D_TexRect_Immediate(float z, float x0, float y0, float x1, float y1, u32 renderFlags, Texture *texture,
                              float uTL, float vTL, u32 cTL, float uBL, float vBL, u32 cBL, float uTR, float vTR,
                              u32 cTR, float uBR, float vBR, u32 cBR); /* 0x52607b */

/* 0x5363c0 */
HoleFX::HoleFX()
{
    hasCaptureSurface = HOLEFX_MASK;
    app = 0;
    viewport = 0;
    vertexBuffer = 0;
    maskTexture = 0;
}

/* 0x53647a - the virtual destructor: deletes the circle mask; the compiler then destroys the 120 RenderPolys. */
HoleFX::~HoleFX()
{
    if (maskTexture)
        delete maskTexture;
}

/* 0x536517 - picks the mode: a capture surface the size of the viewport means blocking mode (frozen frame, radial
 * mesh); failing that, non-blocking mode with the circle mask and the radius that covers the whole screen. */
void HoleFX::Init(D3DApp *app, void *viewport, float radius, u8 useCubicBezier)
{
    DDSURFACEDESC2 desc;
    s32 result;

    this->app = app;
    this->viewport = (Frustrum *)viewport; /* cast kept: Init takes void *, as transition.cpp declares it too */
    this->useCubicBezier = useCubicBezier;
    this->radius = radius;
    alpha = 1.0f;
    shadeScale = 1.0f;
    shadeBias = 0.0f;
    pos[0] = this->viewport->viewportWidth / 2.0f;
    pos[1] = this->viewport->viewportHeight / 2.0f;
    pos[2] = this->viewport->nearZ;
    captured = 0;
    /* cast kept: a pointer or handle used as a number here */
    if (this->app->CreateTextureSurface(&captureSurface, (s32)this->viewport->viewportWidth,
                                        (s32)this->viewport->viewportHeight) < 0) {
        hasCaptureSurface = HOLEFX_MASK;
        maskTexture = new Texture(this->app, 0x100, 0x100, TEXFMT_ARGB4444, &result);
        BuildCircleMask();
        screenRadius = sqrt(this->viewport->viewportWidth * this->viewport->viewportWidth / 4.0 +
                            this->viewport->viewportHeight * this->viewport->viewportHeight / 4.0);
    } else {
        hasCaptureSurface = HOLEFX_MESH;
        desc.dwSize = 0x7c;
        captureSurface->GetSurfaceDesc(&desc);
        captureWidth = desc.dwWidth;
        captureHeight = desc.dwHeight;
        if (CreateVertexBuffer() != 1) {
            MessageBoxA(0, "Error : cannot create HoleFX VB", "HoleFX ERROR", MB_OK);
            exit(0);
        }
        BuildMesh();
    }
}

/* 0x53678f */
void HoleFX::SetShading(float scale, float bias)
{
    shadeScale = scale;
    shadeBias = bias;
}

/* 0x5367b4 - blocking mode only: rebuilds the 65 mesh vertices. Vertex 0 is the hole centre; along each of the 8 rays
 * 8 samples of a Bezier curve run from the ray's end (pulled in to `radius`, the curve's start) through, for the cubic
 * curve, a control point pulled toward the hole, to the undistorted screen point at the viewport edge; alpha scales
 * how far along the curve the samples go. Each sample's grey comes from its tangent, then every vertex is projected
 * (z, rhw) and the centre takes the average colour of the eight innermost samples. */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py) */
void HoleFX::UpdateVertices(int unusedBatcherArg)
{
    /* local names chosen for their stack slots (tools/vc6_locals.py) */
    Mat44 mat; /* the projection */
    u16 sumR;
    u16 sumB;
    u16 gSum;
    void *data;
    Vec3f d;      /* cubic: from the curve's start toward the hole */
    Vec3f center; /* the undistorted screen centre, at the near plane */
    HoleVertex *vtx;
    u32 nbytes;
    u32 k;
    u32 ray;
    Vec3f tg;     /* the curve's tangent at the sample */
    float at;     /* the curve parameter */
    float rscale; /* radius / |rayDirs[ray]| */
    Vec3f edgePt; /* the curve's start: the ray pulled in to the hole's radius */
    Vec3f outer;  /* the curve's end: the undistorted point at the viewport edge */
    u32 step;
    Vec3f ctrl; /* cubic: the extra control point pulled toward the hole */

    if (hasCaptureSurface == HOLEFX_MESH) {
        viewport->BuildProjectionMatrix(&mat);
        center.x = viewport->viewportWidth / 2.0f;
        center.y = viewport->viewportHeight / 2.0f;
        center.z = viewport->nearZ;
        vertexBuffer->Lock(DDLOCK_WAIT, &data, &nbytes);
        vtx = (HoleVertex *)data; /* cast kept: the locked vertex buffer is untyped; its FVF makes it HoleVertex */
        vtx->x = pos[0];
        vtx->y = pos[1];
        vtx->z = pos[2];
        vtx++;
        for (ray = 0; ray < 8; ray++) {
            rscale = radius / rayDirs[ray].Length();
            edgePt.x = rscale * rayDirs[ray].x + center.x;
            edgePt.y = rscale * rayDirs[ray].y + center.y;
            edgePt.z = center.z;
            if (useCubicBezier == 1)
                CubicControlPoint(&ctrl, &edgePt, &d);
            outer.x = center.x + rayDirs[ray].x;
            outer.y = center.y + rayDirs[ray].y;
            outer.z = center.z;
            for (step = 0; step < 8; step++) {
                at = (step + 1) / 8.0f * alpha;
                if (useCubicBezier == 1)
                    Bezier3_Eval(&vtx->x, &tg.x, pos, &edgePt.x, &ctrl.x, &outer.x, at);
                else
                    Bezier2_EvalWithTangent(&vtx->x, &tg.x, pos, &edgePt.x, &outer.x, at);
                vtx->diffuse = Color_GreyFromScalar(tg.z / tg.Length());
                vtx++;
            }
        }
        vtx = (HoleVertex *)data; /* cast kept: as above */
        for (k = 0; k < 0x41; k++) {
            vtx->rhw = vtx->z;
            vtx->z = (mat.m[2][2] * vtx->z + mat.m[3][2]) / vtx->rhw;
            vtx++;
        }
        vtx = (HoleVertex *)data; /* cast kept: as above */
        sumR = gSum = sumB = 0;
        for (ray = 0; ray < 8; ray++) {
            sumR += (u16)((vtx[ray * 8 + 1].diffuse >> 16) & 0xff);
            gSum += (u16)((vtx[ray * 8 + 1].diffuse >> 8) & 0xff);
            sumB += (u16)(vtx[ray * 8 + 1].diffuse & 0xff);
        }
        vtx->diffuse = (sumR / 8 << 16) + (gSum / 8 << 8) + sumB / 8;
        vertexBuffer->Unlock();
    }
}

/* 0x536d24 - blocking mode: freezes the current frame (the front buffer, or the back buffer when useSecondBuffer) into
 * the capture surface, once; force == 1 captures again. */
HRESULT HoleFX::CaptureScreen(int useSecondBuffer, u8 force)
{
    RECT dst;
    HRESULT hr;

    if ((captured == 0 || force == 1) && hasCaptureSurface == HOLEFX_MESH) {
        g_pPolyBin->Flush();
        dst.left = dst.top = 0;
        dst.right = (s32)viewport->viewportWidth;
        dst.bottom = (s32)viewport->viewportHeight;
        switch (useSecondBuffer) {
            case 0: {
                IDirectDrawSurface7 *front = app->pPrimary;
                hr = captureSurface->Blt(&dst, front, 0, DDBLT_WAIT, 0);
                break;
            }
            default: {
                IDirectDrawSurface7 *back = app->pBackBuffer;
                hr = captureSurface->Blt(&dst, back, 0, DDBLT_WAIT, 0);
            }
        }
        captured = 1;
    } else {
        hr = 0;
    }
    return hr;
}

/* 0x536e34 - blocking mode: the 120 mesh triangles, textured with the frozen frame, one DrawPrimitive each.
 * Non-blocking mode: the circle mask as four mirrored quads around the hole (its radius from the depth pos[2] between
 * the frustum's planes, times alpha and screenRadius) and black rectangles over the rest of the screen. The batcher
 * argument is unused; the transition functions all pass g_pPolyBin and the body flushes the global. */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py) */
/* BYTES(dead-code): unused is never used: only its empty constructor loop is in the original (0x53779c) */
void HoleFX::Draw(PolyBatcher *unusedBatcher)
{
    /* local names chosen for their stack slots (tools/vc6_locals.py) */
    HoleVertex *a;
    HoleVertex *vb;
    u32 tri;
    u32 size;
    void *locked;
    HoleVertex *c;
    HoleTlVertex *out;
    float uNear; /* texel-centre texture coordinates of the mask's inner and outer edges */
    float rad;
    Vec3f irisLR; /* the hole's square: bottom-right and top-left corners */
    float uFar;
    Vec3f irisUL;

    if (hasCaptureSurface == HOLEFX_MESH) {
        vertexBuffer->Lock(DDLOCK_WAIT, &locked, &size);
        app->GetDevice()->SetTexture(0, captureSurface);
        app->Render_SetStateFlags(RSF_DITHER | RSF_TEXTURED);
        for (tri = 0; tri < 120; tri++) {
            /* cast kept: the locked vertex buffer is untyped; its FVF makes it HoleVertex */
            a = (HoleVertex *)locked + tris[tri].poly.idx[0];
            vb = (HoleVertex *)locked + tris[tri].poly.idx[1]; /* cast kept: as above */
            c = (HoleVertex *)locked + tris[tri].poly.idx[2];  /* cast kept: as above */
            out = (HoleTlVertex *)tris[tri].verts; /* cast kept: a RenderPoly's vertex block is untyped floats */
            memcpy(out, a, 0x10);
            memcpy(out + 1, vb, 0x10);
            memcpy(out + 2, c, 0x10);
            out[2].specular = 0xff000000;
            out[1].specular = 0xff000000;
            out[0].specular = 0xff000000;
            out[0].diffuse = a->diffuse;
            out[1].diffuse = vb->diffuse;
            out[2].diffuse = c->diffuse;
            app->DrawTexTriangleList(out, 3);
        }
        app->ClearStateFlagsInline(RSF_DITHER | RSF_TEXTURED);
        vertexBuffer->Unlock();
    } else {
        HoleVertex unused[3]; /* never used; its (empty) constructor loop is at 0x53779c */
        g_pPolyBin->Flush();
        rad = (viewport->farZ - pos[2]) / (viewport->farZ - viewport->nearZ) * alpha * screenRadius;
        app->BindTexture(maskTexture, 0);
        app->Render_SetStateFlags(RSF_BLEND_ALPHA | RSF_ZTEST | RSF_TEXTURED | RSF_FILTER_LINEAR);
        irisUL.x = pos[0] - rad;
        irisUL.y = pos[1] - rad;
        irisLR.x = rad + pos[0];
        irisLR.y = rad + pos[1];
        uNear = 0.5f / maskTexture->width + 0.0f;
        uFar = 1.0f - 0.5f / maskTexture->width;
        Draw2D_TexRect_Immediate(g_screen.Draw2D_LayerToZ(g_screenLayerBase + 1), irisUL.x, irisUL.y, pos[0], pos[1],
                                 RSF_BLEND_ALPHA | RSF_TEXTURED | RSF_FILTER_LINEAR, maskTexture, uFar, uFar, 0xffffff,
                                 uFar, uNear, 0xffffff, uNear, uFar, 0xffffff, uNear, uNear, 0xffffff);
        Draw2D_TexRect_Immediate(g_screen.Draw2D_LayerToZ(g_screenLayerBase + 1), pos[0], irisUL.y, irisLR.x, pos[1],
                                 RSF_BLEND_ALPHA | RSF_TEXTURED | RSF_FILTER_LINEAR, maskTexture, uNear, uFar, 0xffffff,
                                 uNear, uNear, 0xffffff, uFar, uFar, 0xffffff, uFar, uNear, 0xffffff);
        Draw2D_TexRect_Immediate(g_screen.Draw2D_LayerToZ(g_screenLayerBase + 1), irisUL.x, pos[1], pos[0], irisLR.y,
                                 RSF_BLEND_ALPHA | RSF_TEXTURED | RSF_FILTER_LINEAR, maskTexture, uFar, uNear, 0xffffff,
                                 uFar, uFar, 0xffffff, uNear, uNear, 0xffffff, uNear, uFar, 0xffffff);
        Draw2D_TexRect_Immediate(g_screen.Draw2D_LayerToZ(g_screenLayerBase + 1), pos[0], pos[1], irisLR.x, irisLR.y,
                                 RSF_BLEND_ALPHA | RSF_TEXTURED | RSF_FILTER_LINEAR, maskTexture, uNear, uNear,
                                 0xffffff, uNear, uFar, 0xffffff, uFar, uNear, 0xffffff, uFar, uFar, 0xffffff);
        if (irisUL.y >= 0.0f)
            Draw2D_FlatRect_Immediate(g_screen.Draw2D_LayerToZ(g_screenLayerBase + 1), irisUL.x, 0.0f, irisLR.x,
                                      irisUL.y, RSF_DITHER | RSF_COLORVERTEX, 0);
        if (irisUL.x >= 0.0f)
            Draw2D_FlatRect_Immediate(g_screen.Draw2D_LayerToZ(g_screenLayerBase + 1), 0.0f, 0.0f, irisUL.x,
                                      viewport->viewportHeight, RSF_DITHER | RSF_COLORVERTEX, 0);
        if (irisLR.x < viewport->viewportWidth)
            Draw2D_FlatRect_Immediate(g_screen.Draw2D_LayerToZ(g_screenLayerBase + 1), irisLR.x, 0.0f,
                                      viewport->viewportWidth, viewport->viewportHeight, RSF_DITHER | RSF_COLORVERTEX,
                                      0);
        if (irisLR.y < viewport->viewportHeight)
            Draw2D_FlatRect_Immediate(g_screen.Draw2D_LayerToZ(g_screenLayerBase + 1), irisUL.x, irisLR.y, irisLR.x,
                                      viewport->viewportHeight, RSF_DITHER | RSF_COLORVERTEX, 0);
        app->Render_ClearStateFlags(RSF_BLEND_ALPHA | RSF_ZTEST | RSF_TEXTURED | RSF_FILTER_LINEAR);
    }
}

/* 0x538130 - the mesh's vertex buffer, 65 XYZRHW|DIFFUSE|SPECULAR vertices, created once. */
u8 HoleFX::CreateVertexBuffer()
{
    u32 count;
    D3DVERTEXBUFFERDESC desc;

    count = 0x41;
    if (vertexBuffer == 0) {
        memset(&desc, 0, sizeof desc);
        desc.dwSize = sizeof desc;
        desc.dwCaps = D3DVBCAPS_DONOTCLIP;
        desc.dwFVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR;
        desc.dwNumVertices = count;
        if (g_pD3DAppMain->CreateVBWithResult(&desc, &vertexBuffer) < 0)
            return 0;
    }
    return 1;
}

/* 0x5381f4 - blocking mode, once: the 8 ray directions are the compass points of the viewport half-extent (N, NE, E,
 * SE, S, SW, W, NW), the vertices are built once so that each one's UV is its undistorted screen position over the
 * capture size, and the 120 triangles are laid out: per ray, the fan triangle (centre, ray i, ray i+1) and then two
 * triangles per step between the two rays' sample strips. */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py) */
void HoleFX::BuildMesh()
{
    float uv[0x41 * 2]; /* u, v per vertex; local names chosen for their stack slots (tools/vc6_locals.py) */
    RenderPoly *tri;
    HoleVertex *vb;
    u32 q;
    void *locked;
    u32 i;
    PolyTri *pt;
    u32 len;
    float *tv;
    u32 cur;
    u32 next;
    u32 step;

    rayDirs[0].x = 0.0f;
    rayDirs[0].y = -viewport->viewportHeight / 2.0f;
    rayDirs[0].z = 0.0f;
    rayDirs[1].x = viewport->viewportWidth / 2.0f;
    rayDirs[1].y = -viewport->viewportHeight / 2.0f;
    rayDirs[1].z = 0.0f;
    rayDirs[2].x = viewport->viewportWidth / 2.0f;
    rayDirs[2].y = 0.0f;
    rayDirs[2].z = 0.0f;
    rayDirs[3].x = viewport->viewportWidth / 2.0f;
    rayDirs[3].y = viewport->viewportHeight / 2.0f;
    rayDirs[3].z = 0.0f;
    rayDirs[4].x = 0.0f;
    rayDirs[4].y = viewport->viewportHeight / 2.0f;
    rayDirs[4].z = 0.0f;
    rayDirs[5].x = -viewport->viewportWidth / 2.0f;
    rayDirs[5].y = viewport->viewportHeight / 2.0f;
    rayDirs[5].z = 0.0f;
    rayDirs[6].x = -viewport->viewportWidth / 2.0f;
    rayDirs[6].y = 0.0f;
    rayDirs[6].z = 0.0f;
    rayDirs[7].x = -viewport->viewportWidth / 2.0f;
    rayDirs[7].y = -viewport->viewportHeight / 2.0f;
    rayDirs[7].z = 0.0f;
    UpdateVertices(0);
    vertexBuffer->Lock(DDLOCK_WAIT, &locked, &len);
    vb = (HoleVertex *)locked; /* cast kept: the locked vertex buffer is untyped; its FVF makes it HoleVertex */
    for (q = 0; q < 0x41; q++) {
        uv[q * 2] = vb[q].x / captureWidth;
        uv[q * 2 + 1] = vb[q].y / captureHeight;
    }
    vertexBuffer->Unlock();
    tri = tris;
    for (i = 0; i < 8; i++) {
        cur = i * 8 + 1;
        next = (i + 1) % 8 * 8 + 1;
        tv = tri->verts;
        pt = &tri->poly;
        pt->idx[0] = 0;
        pt->idx[1] = cur;
        pt->idx[2] = next;
        tv[6] = uv[pt->idx[0] * 2];
        tv[7] = uv[pt->idx[0] * 2 + 1];
        tv[14] = uv[pt->idx[1] * 2];
        tv[15] = uv[pt->idx[1] * 2 + 1];
        tv[22] = uv[pt->idx[2] * 2];
        tv[23] = uv[pt->idx[2] * 2 + 1];
        tri++;
        for (step = 1; step < 8; step++) {
            tv = tri->verts;
            pt = &tri->poly;
            pt->idx[0] = cur;
            pt->idx[1] = cur + 1;
            pt->idx[2] = next;
            tv[6] = uv[pt->idx[0] * 2];
            tv[7] = uv[pt->idx[0] * 2 + 1];
            tv[14] = uv[pt->idx[1] * 2];
            tv[15] = uv[pt->idx[1] * 2 + 1];
            tv[22] = uv[pt->idx[2] * 2];
            tv[23] = uv[pt->idx[2] * 2 + 1];
            tv = tri[1].verts;
            pt = &tri[1].poly;
            pt->idx[0] = cur + 1;
            pt->idx[1] = next + 1;
            pt->idx[2] = next;
            tv[6] = uv[pt->idx[0] * 2];
            tv[7] = uv[pt->idx[0] * 2 + 1];
            tv[14] = uv[pt->idx[1] * 2];
            tv[15] = uv[pt->idx[1] * 2 + 1];
            tv[22] = uv[pt->idx[2] * 2];
            tv[23] = uv[pt->idx[2] * 2 + 1];
            tri += 2;
            cur++;
            next++;
        }
    }
}

/* 0x53886e - non-blocking mode: fills the mask texture with a quarter disc of opaque black (ARGB4444 0xf000) of
 * radius height - 1 in its top-left corner; HoleFX_Draw mirrors it into the four quadrants. Always returns 0. */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py) */
u8 HoleFX::BuildCircleMask()
{
    DDSURFACEDESC2 surfDesc; /* local names chosen for their stack slots (tools/vc6_locals.py) */
    u16 *pix;
    float rad;
    u32 row;
    u8 result;

    result = 0;
    if (maskTexture) {
        maskTexture->Surface_LockForWrite(&surfDesc);
        pix = (u16 *)surfDesc.lpSurface; /* cast kept: a locked surface is raw memory; ARGB4444 is 16-bit */
        memset(pix, 0, maskTexture->width * 2 * maskTexture->height);
        rad = (float)(maskTexture->height - 1);
        for (row = 0; row < (u32)(s32)rad; row++) {
            u32 span = (s32)sqrt(rad * rad - row * row);
            u32 x;
            for (x = 0; x < span; x++)
                pix[row * maskTexture->width + x] = 0xf000;
        }
        maskTexture->Surface_Unlock();
    }
    return result;
}

/* 0x5389d6 - quadratic Bezier point and derivative: pos = u^2 p0 + 2tu p1 + t^2 p2, tan = -2u p0 + 2(1-2t) p1 + 2t p2. */
void HoleFX::Bezier2_EvalWithTangent(float *outPos, float *outTangent, const float *p0, const float *p1,
                                     const float *p2, float t)
{
    float tt;
    float invT;
    float uu;

    tt = t * t;
    invT = 1.0f - t;
    uu = invT * invT;
    outPos[0] = uu * p0[0] + p1[0] * 2.0f * t * invT + tt * p2[0];
    outPos[1] = uu * p0[1] + p1[1] * 2.0f * t * invT + tt * p2[1];
    outPos[2] = uu * p0[2] + p1[2] * 2.0f * t * invT + tt * p2[2];
    outTangent[0] = p0[0] * -2.0f * invT + p1[0] * 2.0f * (1.0f - 2.0f * t) + p2[0] * 2.0f * t;
    outTangent[1] = p0[1] * -2.0f * invT + p1[1] * 2.0f * (1.0f - 2.0f * t) + p2[1] * 2.0f * t;
    outTangent[2] = p0[2] * -2.0f * invT + p1[2] * 2.0f * (1.0f - 2.0f * t) + p2[2] * 2.0f * t;
}

/* 0x538b2d - cubic Bezier point, pos = u^3 p0 + 3tu^2 p1 + 3t^2u p2 + t^3 p3, and a second vector
 * -3u^2 p0 + (1+t)(3t+1) p1 + (2-3t) p2 + 3t^2 p3, which is NOT the curve's derivative (that would be
 * -3u^2 p0 + 3u(1-3t) p1 + 3t(2-3t) p2 + 3t^2 p3). The caller only shades with it (third component over length),
 * and this is the path the game uses (Transition_Init 0x51651b passes useCubicBezier = 1). */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py) */
void HoleFX::Bezier3_Eval(float *outPos, float *outTangent, const float *p0, const float *p1, const float *p2,
                          const float *p3, float t)
{
    float u; /* local names chosen for their stack slots (tools/vc6_locals.py) */
    float w1;
    float c2;
    float ttt;
    float omt3;

    ttt = t * t * t;
    u = 1.0f - t;
    omt3 = u * u * u;
    outPos[0] = omt3 * p0[0] + p1[0] * 3.0f * t * u * u + p2[0] * 3.0f * t * t * u + ttt * p3[0];
    outPos[1] = omt3 * p0[1] + p1[1] * 3.0f * t * u * u + p2[1] * 3.0f * t * t * u + ttt * p3[1];
    outPos[2] = omt3 * p0[2] + p1[2] * 3.0f * t * u * u + p2[2] * 3.0f * t * t * u + ttt * p3[2];
    w1 = (1.0f + t) * (3.0f * t + 1.0f);
    c2 = 2.0f - 3.0f * t;
    outTangent[0] = p0[0] * -3.0f * u * u + p3[0] * 3.0f * t * t + w1 * p1[0] + c2 * p2[0];
    outTangent[1] = p0[1] * -3.0f * u * u + p3[1] * 3.0f * t * t + w1 * p1[1] + c2 * p2[1];
    outTangent[2] = p0[2] * -3.0f * u * u + p3[2] * 3.0f * t * t + w1 * p1[2] + c2 * p2[2];
}

/* 0x538d20 - grey 0xRRGGBB for a curve's tangent: 1 - (|v| * shadeScale + shadeBias), clamped to [0, 1]. */
u32 HoleFX::Color_GreyFromScalar(float v)
{
    float f;
    u32 ci;
    u32 grey;

    f = 1.0f - ((float)fabs(v) * shadeScale + shadeBias);
    if (f < 0.0f)
        f = 0.0f;
    if (f > 1.0f)
        f = 1.0f;
    ci = (s32)(f * 255.0f);
    grey = ci << 16 | ci << 8 | ci;
    return grey;
}
