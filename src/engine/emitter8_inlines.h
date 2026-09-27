/* Shared inline definitions. Select helpers before inclusion, then undefine their selectors.
 * BYTES(inline): repeated inclusion preserves each caller's dependency visibility and definition order.
 * Per-helper guards prevent redefinition when a later include selects more helpers. */

#if defined(SDW_INLINE_INLINEEMITTER8_RENDERFLAT_CAMERA_S32) && \
    !defined(SDW_INLINE_INLINEEMITTER8_RENDERFLAT_CAMERA_S32_DEFINED)
#define SDW_INLINE_INLINEEMITTER8_RENDERFLAT_CAMERA_S32_DEFINED
inline void InlineEmitter8::RenderFlat(Camera *view, s32 fwd)
{
    if (fwd)
        base.Emitter_RenderFlat_Fwd(view);
    else
        base.Emitter_RenderFlat(view);
}
#endif
