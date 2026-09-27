/* Shared inline definitions. Select helpers before inclusion, then undefine their selectors.
 * BYTES(inline): repeated inclusion preserves each caller's dependency visibility and definition order.
 * Per-helper guards prevent redefinition when a later include selects more helpers. */

#if defined(SDW_INLINE_POLYBATCHER_SUBMITPOLYINLINE_RENDERPOLY) && \
    !defined(SDW_INLINE_POLYBATCHER_SUBMITPOLYINLINE_RENDERPOLY_DEFINED)
#define SDW_INLINE_POLYBATCHER_SUBMITPOLYINLINE_RENDERPOLY_DEFINED
#if SDW_INLINE_POLYBATCHER_SUBMITPOLYINLINE_RENDERPOLY == 1
/* BYTES(inline): inferred Shadow/FireBall twin of SubmitPoly (0x415c80):
 * draw helpers and the flat clear expand; other Render_* helpers remain calls.
 * Nested blocks place batch/count after flags in the expansion's stack slots. */
inline void PolyBatcher::SubmitPolyInline(RenderPoly *poly)
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
                renderer->ClearStateFlagsInline(stateFlags);
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
                            renderer->DrawTexturedTriangleList(batch, batchCapacity * 3);
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
#elif SDW_INLINE_POLYBATCHER_SUBMITPOLYINLINE_RENDERPOLY == 2
/* BYTES(inline): ScnTools' force-inline twin for its first triangle: draws and
 * texture binding expand, state set/clear remain calls. Keep the nested slot order. */
__forceinline void PolyBatcher::SubmitPolyInline(RenderPoly *poly)
{
    switch (poly->type) {
        case RPOLY_OPAQUE:
            if (flatBatchCount <= batchCapacity) {
                /* cast kept: the flat batch is untyped vertex memory, 3 FlatVertex (FVF 0xc4) per triangle */
                memcpy((FlatVertex *)flatBatchVerts + flatBatchCount * 3, poly->verts, 0x48);
                ++flatBatchCount;
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
            /* cast kept: vertex memory is untyped; a textured poly's are TlVertex */
            TlVertex *vertices = (TlVertex *)poly->verts;
            u32 texture = (poly->type - RPOLY_TEXTURED_BASE) & ~RPOLY_F_8000;
            if (texture < immediateTexCount) {
                u32 *flags;
                {
                    /* cast kept: a texture batch is untyped vertex memory holding TlVertex */
                    TlVertex *batch = (TlVertex *)texBatchVerts[texture];
                    {
                        u32 *count = &texBatchCounts[texture];
                        flags = &texStateFlags[texture];
                        if (*count <= batchCapacity) {
                            memcpy(batch + *count * 3, vertices, 0x60);
                            ++*count;
                        }
                        if (*count >= batchCapacity) {
                            if (texture != lastTextureIndex || textureDirty == 1) {
                                renderer->SetTextureInline(textures[texture], 0);
                                lastTextureIndex = texture;
                                textureDirty = 0;
                            }
                            renderer->Render_SetStateFlags(*flags);
                            renderer->DrawPrimitiveInline(
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
#elif SDW_INLINE_POLYBATCHER_SUBMITPOLYINLINE_RENDERPOLY == 3
/* BYTES(inline): FishingRod's first submission expands; the textured flush keeps
 * Render_DrawPrimitive out of line. Nested blocks retain batch/count after flags. */
inline void PolyBatcher::SubmitPolyInline(RenderPoly *poly)
{
    switch (poly->type) {
        case RPOLY_OPAQUE:
            if (flatBatchCount <= batchCapacity) {
                /* cast kept: vertex memory is untyped; an opaque polygon's are FlatVertex */
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
            /* cast kept: vertex memory is untyped; a textured polygon's are FlameVertex */
            FlameVertex *vertices = (FlameVertex *)poly->verts;
            u32 texture = (poly->type - 4) & ~RPOLY_F_8000;
            if (texture < immediateTexCount) {
                u32 *flags;
                {
                    /* cast kept: vertex memory is untyped; a texture batch holds FlameVertex */
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
#endif
#endif
