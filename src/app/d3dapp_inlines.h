/* Shared inline definitions. Select helpers before inclusion, then undefine their selectors.
 * BYTES(inline): repeated inclusion preserves each caller's dependency visibility and definition order.
 * Per-helper guards prevent redefinition when a later include selects more helpers. */

#if defined(SDW_INLINE_D3DAPP_BINDTEXTURE_TEXTURE_S32) && !defined(SDW_INLINE_D3DAPP_BINDTEXTURE_TEXTURE_S32_DEFINED)
#define SDW_INLINE_D3DAPP_BINDTEXTURE_TEXTURE_S32_DEFINED
/* BYTES(cast): the signed stage gives constant 0 its own expansion temporary
 * (Text, 0x53540c; HoleFX, 0x537844). */
inline void D3DApp::BindTexture(Texture *tex, s32 stage)
{
    pD3DDevice->SetTexture(stage, tex->surface);
}
#endif

#if defined(SDW_INLINE_D3DAPP_CLEARSTATEFLAGSINLINE_U32) && \
    !defined(SDW_INLINE_D3DAPP_CLEARSTATEFLAGSINLINE_U32_DEFINED)
#define SDW_INLINE_D3DAPP_CLEARSTATEFLAGSINLINE_U32_DEFINED
/* BYTES(inline): twin of Render_ClearStateFlags (0x4159b0) for expanded sites;
 * preserve the per-bit tests and device reloads separately from sites retaining
 * a Render_ClearStateFlags call. */
inline void D3DApp::ClearStateFlagsInline(u32 flags)
{
    if (flags & RSF_ANTIALIAS)
        pD3DDevice->SetRenderState(D3DRENDERSTATE_ANTIALIAS, D3DANTIALIAS_NONE);
    if ((flags & RSF_BLEND_ALPHA) || (flags & RSF_BLEND_ADD))
        pD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);
    if (flags & RSF_ALPHATEST)
        pD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, FALSE);
    if (flags & RSF_CLIPPLANE)
        pD3DDevice->SetRenderState(D3DRENDERSTATE_CLIPPLANEENABLE, FALSE);
    if (flags & RSF_DITHER)
        pD3DDevice->SetRenderState(D3DRENDERSTATE_DITHERENABLE, FALSE);
    if (flags & RSF_LIGHTING)
        pD3DDevice->SetRenderState(D3DRENDERSTATE_LIGHTING, FALSE);
    if (flags & RSF_SPECULAR)
        pD3DDevice->SetRenderState(D3DRENDERSTATE_SPECULARENABLE, FALSE);
    if (flags & RSF_COLORVERTEX)
        pD3DDevice->SetRenderState(D3DRENDERSTATE_COLORVERTEX, FALSE);
    if ((flags & RSF_CULL_CW) || (flags & RSF_CULL_CCW))
        pD3DDevice->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE);
    if (flags & RSF_ZTEST)
        pD3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, D3DZB_FALSE);
    if (flags & RSF_ZWRITE_ON)
        pD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, FALSE);
    if (flags & RSF_ZWRITE_OFF)
        pD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, TRUE);
    if (flags & RSF_TEXTURED)
        pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
    if (flags & RSF_FILTER_LINEAR) {
        pD3DDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_POINT);
        pD3DDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFN_POINT);
    }
    if (flags & RSF_FOG)
        pD3DDevice->SetRenderState(D3DRENDERSTATE_FOGENABLE, FALSE);
}
#endif

#if defined(SDW_INLINE_D3DAPP_CREATEVB_D3DVERTEXBUFFERDESC_IDIRECT3DVERTEXBUFFER7) && \
    !defined(SDW_INLINE_D3DAPP_CREATEVB_D3DVERTEXBUFFERDESC_IDIRECT3DVERTEXBUFFER7_DEFINED)
#define SDW_INLINE_D3DAPP_CREATEVB_D3DVERTEXBUFFERDESC_IDIRECT3DVERTEXBUFFER7_DEFINED
#if SDW_INLINE_D3DAPP_CREATEVB_D3DVERTEXBUFFERDESC_IDIRECT3DVERTEXBUFFER7 == 1
/* BYTES(inline): the GUID local precedes the receiver temporary (DebugDraw).
 * This body uses the caller's GUID comparison and descriptor field declarations. */
inline void D3DApp::CreateVB(D3DVERTEXBUFFERDESC *desc, IDirect3DVertexBuffer7 **out)
{
    GUID *guid = devices[deviceIndex].pDeviceGUID;
    if (IsEqualGUID(guid, &IID_IDirect3DTnLHalDevice) == 0)
        desc->dwCaps |= D3DVBCAPS_SYSTEMMEMORY;
    pD3D->CreateVertexBuffer(desc, out, 0);
}
#elif SDW_INLINE_D3DAPP_CREATEVB_D3DVERTEXBUFFERDESC_IDIRECT3DVERTEXBUFFER7 == 2
/* BYTES(inline): direct GUID comparison with the dwCaps descriptor view. */
inline void D3DApp::CreateVB(D3DVERTEXBUFFERDESC *desc, IDirect3DVertexBuffer7 **out)
{
    GUID *guid = devices[deviceIndex].pDeviceGUID;
    if ((!memcmp(guid, &IID_IDirect3DTnLHalDevice, 16)) == 0)
        desc->dwCaps |= D3DVBCAPS_SYSTEMMEMORY;
    pD3D->CreateVertexBuffer(desc, out, 0);
}
#elif SDW_INLINE_D3DAPP_CREATEVB_D3DVERTEXBUFFERDESC_IDIRECT3DVERTEXBUFFER7 == 3
/* BYTES(inline): direct GUID comparison with the caps descriptor view. */
inline void D3DApp::CreateVB(D3DVERTEXBUFFERDESC *desc, IDirect3DVertexBuffer7 **out)
{
    GUID *guid = devices[deviceIndex].pDeviceGUID;
    if ((!memcmp(guid, &IID_IDirect3DTnLHalDevice, 16)) == 0)
        desc->dwCaps |= D3DVBCAPS_SYSTEMMEMORY;
    pD3D->CreateVertexBuffer(desc, out, 0);
}
#endif
#endif

#if defined(SDW_INLINE_D3DAPP_CREATEVBWITHRESULT_D3DVERTEXBUFFERDESC_IDIRECT3DVERTEXBUFFER7) && \
    !defined(SDW_INLINE_D3DAPP_CREATEVBWITHRESULT_D3DVERTEXBUFFERDESC_IDIRECT3DVERTEXBUFFER7_DEFINED)
#define SDW_INLINE_D3DAPP_CREATEVBWITHRESULT_D3DVERTEXBUFFERDESC_IDIRECT3DVERTEXBUFFER7_DEFINED
#if SDW_INLINE_D3DAPP_CREATEVBWITHRESULT_D3DVERTEXBUFFERDESC_IDIRECT3DVERTEXBUFFER7 == 1
/* BYTES(inline): the result-returning twin preserves AnimMesh/Mesh's expansion
 * temporaries; the descriptor uses caps and the direct GUID comparison. */
inline long D3DApp::CreateVBWithResult(D3DVERTEXBUFFERDESC *desc, IDirect3DVertexBuffer7 **out)
{
    GUID *guid = devices[deviceIndex].pDeviceGUID;
    if ((!memcmp(guid, &IID_IDirect3DTnLHalDevice, 16)) == 0)
        desc->dwCaps |= D3DVBCAPS_SYSTEMMEMORY;
    return pD3D->CreateVertexBuffer(desc, out, 0);
}
#elif SDW_INLINE_D3DAPP_CREATEVBWITHRESULT_D3DVERTEXBUFFERDESC_IDIRECT3DVERTEXBUFFER7 == 2
/* BYTES(inline): result-returning twin with the caller's GUID comparison and dwCaps view. */
inline long D3DApp::CreateVBWithResult(D3DVERTEXBUFFERDESC *desc, IDirect3DVertexBuffer7 **out)
{
    GUID *guid = devices[deviceIndex].pDeviceGUID;
    if (IsEqualGUID(guid, &IID_IDirect3DTnLHalDevice) == 0)
        desc->dwCaps |= D3DVBCAPS_SYSTEMMEMORY;
    return pD3D->CreateVertexBuffer(desc, out, 0);
}
#endif
#endif

#if defined(SDW_INLINE_D3DAPP_DRAWPRIMITIVEINLINE_U32_U32_VOID_U32) && \
    !defined(SDW_INLINE_D3DAPP_DRAWPRIMITIVEINLINE_U32_U32_VOID_U32_DEFINED)
#define SDW_INLINE_D3DAPP_DRAWPRIMITIVEINLINE_U32_U32_VOID_U32_DEFINED
/* BYTES(inline): twin of Render_DrawPrimitive (0x415c00) at expanded sites;
 * the separate Render_* spelling preserves sites that call it out of line. */
inline void D3DApp::DrawPrimitiveInline(u32 type, u32 fvf, void *verts, u32 count)
{
    if (count)
        pD3DDevice->DrawPrimitive(type, fvf, verts, count, 0);
}
#endif

#if defined(SDW_INLINE_D3DAPP_DRAWTEXTRIANGLELIST_VOID_U32) && \
    !defined(SDW_INLINE_D3DAPP_DRAWTEXTRIANGLELIST_VOID_U32_DEFINED)
#define SDW_INLINE_D3DAPP_DRAWTEXTRIANGLELIST_VOID_U32_DEFINED
/* BYTES(cast): the unsigned count determines which argument gets an expansion
 * temporary in HoleFX (0x537473). */
inline void D3DApp::DrawTexTriangleList(void *verts, u32 count)
{
    if (count)
        pD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1,
                                  verts, count, 0);
}
#endif

#if defined(SDW_INLINE_D3DAPP_DRAWTEXTUREDTRIANGLELIST_VOID_S32) && \
    !defined(SDW_INLINE_D3DAPP_DRAWTEXTUREDTRIANGLELIST_VOID_S32_DEFINED)
#define SDW_INLINE_D3DAPP_DRAWTEXTUREDTRIANGLELIST_VOID_S32_DEFINED
inline void D3DApp::DrawTexturedTriangleList(void *vertices, s32 count)
{
    if (count)
        pD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1,
                                  vertices, count, 0);
}
#endif

#if defined(SDW_INLINE_D3DAPP_DRAWTRIANGLELIST_VOID_S32) && \
    !defined(SDW_INLINE_D3DAPP_DRAWTRIANGLELIST_VOID_S32_DEFINED)
#define SDW_INLINE_D3DAPP_DRAWTRIANGLELIST_VOID_S32_DEFINED
inline void D3DApp::DrawTriangleList(void *verts, s32 count)
{
    if (count)
        pD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR, verts, count,
                                  0);
}
#endif

#if defined(SDW_INLINE_D3DAPP_DRAWTRIANGLELIST_VOID_U32) && \
    !defined(SDW_INLINE_D3DAPP_DRAWTRIANGLELIST_VOID_U32_DEFINED)
#define SDW_INLINE_D3DAPP_DRAWTRIANGLELIST_VOID_U32_DEFINED
inline void D3DApp::DrawTriangleList(void *verts, u32 count)
{
    if (count)
        pD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR, verts, count,
                                  0);
}
#endif

#if defined(SDW_INLINE_D3DAPP_GETDEVICE) && !defined(SDW_INLINE_D3DAPP_GETDEVICE_DEFINED)
#define SDW_INLINE_D3DAPP_GETDEVICE_DEFINED
/* BYTES(inline): the return travels through expansion temporaries, including
 * two stack copies in HoleFX (0x536e7e) and Splash_Render (0x474d38). */
inline IDirect3DDevice7 *D3DApp::GetDevice()
{
    return pD3DDevice;
}
#endif

#if defined(SDW_INLINE_D3DAPP_SETFLAGSINLINE_U32) && !defined(SDW_INLINE_D3DAPP_SETFLAGSINLINE_U32_DEFINED)
#define SDW_INLINE_D3DAPP_SETFLAGSINLINE_U32_DEFINED
/* BYTES(inline): inferred inline state-setting twin used by LightSpot's beam
 * submissions; preserve which surrounding Render_* helpers remain calls. */
inline void D3DApp::SetFlagsInline(u32 flags)
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
            pD3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, TRUE);
    if (flags & RSF_ZWRITE_OFF)
        if (pD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, FALSE))
            pD3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, TRUE);
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
#endif

#if defined(SDW_INLINE_D3DAPP_SETSTATEFLAGSINLINE_U32) && !defined(SDW_INLINE_D3DAPP_SETSTATEFLAGSINLINE_U32_DEFINED)
#define SDW_INLINE_D3DAPP_SETSTATEFLAGSINLINE_U32_DEFINED
/* BYTES(inline): twin of Render_SetStateFlags (0x4155f0) for expanded sites;
 * constant flag arguments remain materialized in the individual tests. */
inline void D3DApp::SetStateFlagsInline(u32 flags)
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
            pD3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, TRUE);
    if (flags & RSF_ZWRITE_OFF)
        if (pD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, FALSE))
            pD3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, TRUE);
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
#endif

#if defined(SDW_INLINE_D3DAPP_SETTEXTURE_TEXTURE_TEXSTAGE) && \
    !defined(SDW_INLINE_D3DAPP_SETTEXTURE_TEXTURE_TEXSTAGE_DEFINED)
#define SDW_INLINE_D3DAPP_SETTEXTURE_TEXTURE_TEXSTAGE_DEFINED
inline void D3DApp::SetTexture(Texture *tex, TexStage stage)
{
    pD3DDevice->SetTexture(stage, tex->surface);
}
#endif

#if defined(SDW_INLINE_D3DAPP_SETTEXTUREINLINE_TEXTURE_VOLATILE_U32) && \
    !defined(SDW_INLINE_D3DAPP_SETTEXTUREINLINE_TEXTURE_VOLATILE_U32_DEFINED)
#define SDW_INLINE_D3DAPP_SETTEXTUREINLINE_TEXTURE_VOLATILE_U32_DEFINED
inline void D3DApp::SetTextureInline(Texture *tex, volatile u32 stage)
{
    pD3DDevice->SetTexture(stage, tex->surface);
}
#endif

#if defined(SDW_INLINE_D3DAPP_SETTRANSFORM_U32_MAT44) && !defined(SDW_INLINE_D3DAPP_SETTRANSFORM_U32_MAT44_DEFINED)
#define SDW_INLINE_D3DAPP_SETTRANSFORM_U32_MAT44_DEFINED
/* BYTES(inline): the receiver/device reload goes through an expansion temporary
 * in the Screen, AnimMesh, and emitter callers. */
inline void D3DApp::SetTransform(u32 state, Mat44 *m)
{
    pD3DDevice->SetTransform(state, m);
}
#endif
