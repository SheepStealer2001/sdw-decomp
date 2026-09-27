/*
 * T250 - original object ScnTools.cpp (guessed name), one translation unit: the scenaric grab-bag after the class
 * registration - an empty stub, the object lookups, Vec3s_ScaleByDt, the box-list queries, the class icons,
 * CollBox::Box_Translate, the designer-property readers and distances, the flat and dome ground queries,
 * Matrix_RollByDisplacement, the voice stream wrappers and five empty stubs.
 * .text 0x5145c0-0x515ef4, .bss 0x6d411c-0x6d4120 (g_voiceOwner); no .rdata/.data (its float constants are owned by
 * earlier objects).
 * The groups, with the neighbouring objects:
 *   class icons and voice lines   Scenaric_Stub_5145c0, Scenaric_GetClassIcons, Scenaric_DrawClassIcon (with its inline
 *                                 helpers), Matrix_RollByDisplacement, g_voiceOwner, Voice_*, the stubs
 *                                 0x515ed9-0x515eef (the class registration is T249, the transition T251)
 *   object lookups                Scenaric_FindByClass / FindByRecord / FindByIdList, Dav_FindResourceIndex
 *                                 (Scenaric_SendToClass 0x511529 is T247)
 *   Vec3s_ScaleByDt 0x5147bd
 *   box lists                     BoxList_FindContainingPoint, the other four box-list queries and Box_GapXZ
 *   CollBox::Box_Translate, Box_GroundQueryFlatTop and Box_GroundQueryDome (the object grid and the polygon clippers
 *                                 belong to later objects)
 *   designer properties           the six Scn_GetProp* readers and the six Vec3s distances
 */
/* BYTES: inline, slot-group, slot-name, temp. */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "../sdk/d3d7.h"
#include "../engine/timer.h"
#include "../sdk/crt.h"
#define SDW_MEMBERS_Mat44 Mat44();           /* 0x4077f0 */
#define SDW_MEMBERS_RenderPoly RenderPoly(); /* 0x41aad0 RenderPoly_Ctor */
#define SDW_MEMBERS_D3DApp                                               \
    void Render_SetStateFlags(u32 flags);   /* 0x4155f0 */               \
    void Render_ClearStateFlags(u32 flags); /* 0x4159b0 */               \
    void DrawPrimitiveInline(u32 type, u32 fvf, void *verts, u32 count); \
    void SetTextureInline(Texture *texture, volatile u32 stage);
#define SDW_MEMBERS_PolyBatcher                       \
    void SubmitPoly(RenderPoly *poly); /* 0x415c80 */ \
    void SubmitPolyInline(RenderPoly *poly);
#include "object_lookup.h"
#include "box_queries.h"
#include "ground_flat.h"
#include "property_math.h"

/* The DAV directory (DavHeader.dir): packed, its pointers sit at +6/+0xa/+0x12, which the struct generator cannot lay
 * out, so it is declared here (as in src/objects/lightspot.cpp and the other DAV readers). */
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

#include "fixed_math.h"
#include "load_warmeshes.h"
#include "scenaric.h"
#include "game_state.h"
#include "screen.h"
#include "draw2d.h"
#include "stream_player.h"
#include "progress.h"
#include "../app/app_main.h"
#include "id_list.h"
void Scenaric_RegisterClass_2(u16 classId, ScnObject *(*factory)(void *), u32 classFlags, u16 iconIdA, u16 iconIdB);

extern u32 g_gameFlags; /* 0x6ddf74 */

/* ---- the object lookups ---- */
/* ---- Vec3s_ScaleByDt ---- */
extern s32 g_dt; /* 0x71b300  frame delta used by gameplay, clamped to 0..0xAA       */
/* ---- the box queries ---- */
extern "C" {
}

/* ---- inline helpers (no bodies of their own in the exe) ---- */
/* the box-list queries */
/* BYTES(inline): source-only inline: the original tests the overlap as one OR of the sign bits (the four copies are one helper under the names of the files they came from) */
#define SDW_INLINE_FREE_BOXOVERLAP4_S32_S32_S32_S32 1
#include "coll_box_inlines.h"
#undef SDW_INLINE_FREE_BOXOVERLAP4_S32_S32_S32_S32

#define SDW_INLINE_FREE_BOXOVERLAP2_S32_S32 1
#include "coll_box_inlines.h"
#undef SDW_INLINE_FREE_BOXOVERLAP2_S32_S32

/* Box_GroundQueryDome's sign-bit overlap test: nonzero when all four differences are >= 0 (bit 31 clear in
 * every one). */
#define SDW_INLINE_FREE_OVERLAP4_S32_S32_S32_S32 1
#include "coll_box_inlines.h"
#undef SDW_INLINE_FREE_OVERLAP4_S32_S32_S32_S32

/* Box_GroundQueryFlatTop */
/* The sign bit is clear only when all four signed differences are nonnegative. */
static inline u32 GroundOverlapFour(s32 a, s32 b, s32 c, s32 d)
{
    return ~(a | b | c | d) & 0x80000000U;
}

/* This inline boundary and expression preserve VC6's original register order. */
/* Folding the expression into the caller leaves nine operand bytes different. */
/* BYTES(inline): source-only inline: the boundary and the (position + minimum) + margin order keep the original register order (folded in, nine operand bytes differ) */
static inline s32 GroundTranslatedMinimum(s32 margin, s32 position, s32 minimum)
{
    return (position + minimum) + margin;
}

/* 0x5145c0  Empty, unreferenced. */
void Scenaric_Stub_5145c0() {}

/* 0x5145c5, 145 bytes. Capacity is checked only after writing a match. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; pad0, pad1 fill gaps */
s32 Scenaric_FindByClass(u16 classId, ScnObject **out, s32 maximum)
{
    struct {
        u16 pad0;
        u16 objectClass;
        ScnObject **cursor;
        s32 count;
        u16 pad1;
        u16 index;
        ScnObject *object;
    } w;
    w.count = 0;
    w.cursor = g_scnObjects;
    for (w.index = 0; w.index < g_scnObjectCount; w.index++, w.cursor++) {
        w.object = *w.cursor;
        if (w.object) {
            w.objectClass = w.object->classId;
            if (w.objectClass == classId) {
                out[w.count] = w.object;
                w.count++;
                if (w.count == maximum)
                    return w.count;
            }
        }
    }
    return w.count;
}

/* 0x514656, 193 bytes. Search authored entries, then the active base prefix. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; pad fill gaps */
ScnObject *Scenaric_FindByRecord(void *record)
{
    struct {
        void *activeRecord;
        void *authoredRecord;
        ScnObject **cursor;
        u16 pad;
        u16 index;
        ScnObject *object;
    } w;
    w.cursor = g_scnObjects;
    for (w.index = 0; w.index < g_scnObjectCount; w.index++, w.cursor++) {
        w.object = *w.cursor;
        if (w.object) {
            w.authoredRecord = w.object->record;
            if (w.authoredRecord == record)
                return w.object;
        }
    }
    w.cursor = g_scnActive;
    for (w.index = 0; w.index < g_scnActiveBaseCount; w.index++, w.cursor++) {
        w.object = *w.cursor;
        if (w.object) {
            w.activeRecord = w.object->record;
            if (w.activeRecord == record)
                return w.object;
        }
    }
    return 0;
}

/* 0x514717, 56 bytes. A nonzero count leads directly to reading list[0]. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; pad fill gaps */
ScnObject *Scenaric_FindByIdList(u16 listId)
{
    struct {
        u16 pad;
        u16 count;
        u32 *list;
    } w;
    w.list = Scn_FindIdList(listId, &w.count);
    if (w.count == 0)
        return 0;
    /* cast kept: an export id list holds record pointers as u32 words */
    return Scenaric_FindByRecord((void *)w.list[0]);
}

/* 0x51474f, 110 bytes. A miss and resource index zero both return zero. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
u16 Dav_FindResourceIndex(void *resource)
{
    struct {
        u32 *end;
        u32 *cursor;
    } w;
    w.cursor = g_pDav->war.table;
    w.end = w.cursor + g_pDav->war.header->resourceCount;
    for (; w.cursor < w.end; w.cursor++) {
        if (g_pDav->war.blob + (*w.cursor & 0xffffff) == resource)
            return (u16)(w.cursor - g_pDav->war.table);
    }
    return 0;
}

/* 0x5147bd - THE movement integrator (76 callers): displacement = velocity * g_dt / 4096, per axis, signed division
 * truncating toward zero, NO remainder carried to the next frame. This is why speed depends on frame rate and heading. */
void Vec3s_ScaleByDt(const Vec3s *vel, Vec3s *out)
{
    out->x = (s16)((vel->x * g_dt) / 4096);
    out->y = (s16)((vel->y * g_dt) / 4096);
    out->z = (s16)((vel->z * g_dt) / 4096);
}

/* 0x514823 - the zone test: the first box of the list that contains the point, all three axes inclusive, else 0.
 * X and Z go through the sign-bit test on s32 copies of the point; the vertical axis is compared directly. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
Box *BoxList_FindContainingPoint(Vec3s *point, Box **boxes, u16 count)
{
    struct {
        s32 wideZ, wideX;
        Box *box;
        u32 remaining;
        s16 unused0C, z, y, x;
        Box **cursor;
    } w;
    w.x = point->x;
    w.y = point->y;
    w.z = point->z;
    if (count) {
        for (w.remaining = count, w.cursor = boxes; w.remaining; --w.remaining, ++w.cursor) {
            w.box = *w.cursor;
            w.wideZ = w.z;
            w.wideX = w.x;
            if (Overlap4(w.box->max[0] - w.wideX, w.wideX - w.box->min[0], w.box->max[2] - w.wideZ,
                         w.wideZ - w.box->min[2]) &&
                w.y >= w.box->min[1] && w.y <= w.box->max[1])
                return w.box;
        }
    }
    return 0;
}

/* 0x5148ff, 205 bytes. Inclusive X/Z containment and point.y >= top;
 * there is deliberately no comparison with the box bottom. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
Box *BoxList_FindContainingPointBelowTop(Vec3s *point, Box **boxes, u16 count)
{
    struct {
        s32 wideZ, wideX;
        Box *box;
        u32 remaining;
        s16 unused0C, z, y, x;
        Box **cursor;
    } w;
    w.x = point->x;
    w.y = point->y;
    w.z = point->z;
    if (count) {
        for (w.remaining = count, w.cursor = boxes; w.remaining; --w.remaining, ++w.cursor) {
            w.box = *w.cursor;
            w.wideZ = w.z;
            w.wideX = w.x;
            if (BoxOverlap4(w.box->max[0] - w.wideX, w.wideX - w.box->min[0], w.box->max[2] - w.wideZ,
                            w.wideZ - w.box->min[2]) &&
                w.y >= w.box->min[1])
                return w.box;
        }
    }
    return 0;
}

/* 0x5149cc, 217 bytes. First inclusive overlap across all three axes. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
Box *BoxList_FindOverlappingBox(CollBox *query, Box **boxes, u16 count)
{
    struct {
        Box *box;
        u32 remaining;
        Box **cursor;
    } w;
    if (count) {
        for (w.remaining = count, w.cursor = boxes; w.remaining; --w.remaining, ++w.cursor) {
            w.box = *w.cursor;
            if (BoxOverlap4(w.box->max[0] - query->min.x, query->max.x - w.box->min[0], w.box->max[2] - query->min.z,
                            query->max.z - w.box->min[2]) &&
                BoxOverlap2(w.box->max[1] - query->min.y, query->max.y - w.box->min[1]))
                return w.box;
        }
    }
    return 0;
}

/* 0x514aa5, 177 bytes. First inclusive X/Z containment; height is ignored. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
Box *BoxList_FindContainingPointXZ(Vec3s *point, Box **boxes, u16 count)
{
    struct {
        s32 wideZ, wideX;
        Box *box;
        u32 remaining;
        s16 z, x;
        Box **cursor;
    } w;
    w.x = point->x;
    w.z = point->z;
    if (count) {
        for (w.remaining = count, w.cursor = boxes; w.remaining; --w.remaining, ++w.cursor) {
            w.box = *w.cursor;
            w.wideZ = w.z;
            w.wideX = w.x;
            if (BoxOverlap4(w.box->max[0] - w.wideX, w.wideX - w.box->min[0], w.box->max[2] - w.wideZ,
                            w.wideZ - w.box->min[2]))
                return w.box;
        }
    }
    return 0;
}

/* 0x514b56, 164 bytes. First inclusive X/Z overlap; height is ignored. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
Box *BoxList_FindOverlappingBoxXZ(Box *query, Box **boxes, u16 count)
{
    struct {
        Box *box;
        u32 remaining;
        Box **cursor;
    } w;
    if (count) {
        for (w.remaining = count, w.cursor = boxes; w.remaining; --w.remaining, ++w.cursor) {
            w.box = *w.cursor;
            if (BoxOverlap4(w.box->max[0] - query->min[0], query->max[0] - w.box->min[0], w.box->max[2] - query->min[2],
                            query->max[2] - w.box->min[2]))
                return w.box;
        }
    }
    return 0;
}

/* 0x514bfa, 196 bytes. Maximum of the two axis gaps, zero for touch/overlap. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
s32 Box_GapXZ(CollBox *a, CollBox *b)
{
    struct {
        s32 z, x;
    } w;
    if (b->min.x >= a->max.x)
        w.x = b->min.x - a->max.x;
    else if (a->min.x >= b->max.x)
        w.x = a->min.x - b->max.x;
    else
        w.x = 0;
    if (b->min.z >= a->max.z)
        w.z = b->min.z - a->max.z;
    else if (a->min.z >= b->max.z)
        w.z = a->min.z - b->max.z;
    else
        w.z = 0;
    if (w.z > w.x)
        return w.z;
    else
        return w.x;
}

/* 0x514cbe  The two inventory icon resources a class registered (Scenaric_RegisterClass keeps the first entry of each
 * icon id list). */
void Scenaric_GetClassIcons(u16 classId, void **iconAOut, void **iconBOut)
{
    ScnClassRegEntry *entry;

    entry = &g_scenaricClassRegistry[classId];
    *iconAOut = entry->iconA;
    *iconBOut = entry->iconB;
}

/* The textured screen-space vertex of a RenderPoly (XYZRHW | DIFFUSE | SPECULAR | TEX1, 0x20 bytes). */
struct TlVertex {
    float x, y, z, rhw;
    u32 diffuse, specular;
    float u, v;
};
struct FlatVertex {
    float x, y, z, rhw;
    u32 diffuse, specular;
};

#define SDW_INLINE_D3DAPP_DRAWPRIMITIVEINLINE_U32_U32_VOID_U32 1
#include "../app/d3dapp_inlines.h"
#undef SDW_INLINE_D3DAPP_DRAWPRIMITIVEINLINE_U32_U32_VOID_U32
/* BYTES(temp): volatile by-value parameter: the original copies the stage to a stack temp (see engine/mesh.cpp) */
#define SDW_INLINE_D3DAPP_SETTEXTUREINLINE_TEXTURE_VOLATILE_U32 1
#include "../app/d3dapp_inlines.h"
#undef SDW_INLINE_D3DAPP_SETTEXTUREINLINE_TEXTURE_VOLATILE_U32

/* PolyBatcher::SubmitPoly 0x415c80 as Scenaric_DrawClassIcon expands it for its first triangle (the second one is a
 * call): the header's inline copy, written here as a twin (the name is descriptive; it has no body of its own in the exe). */
/* BYTES(inline): __forceinline twin of PolyBatcher::SubmitPoly 0x415c80: the original expands it for the first triangle only; the nested blocks place batch / count after flags */
#define SDW_INLINE_POLYBATCHER_SUBMITPOLYINLINE_RENDERPOLY 2
#include "polybatcher_inlines.h"
#undef SDW_INLINE_POLYBATCHER_SUBMITPOLYINLINE_RENDERPOLY

/* 0x514ceb  Draws a class's inventory icon (iconA, or iconB when useIconB) as a screen quad centred on (x, y) of the
 * 512x240 HUD space, tinted colorRGB, on `layer` (a pointer into the screen layer table): two textured triangles
 * through a heap RenderPoly, the first submitted through the batcher's inline path, the second through the call. */
/* BYTES(slot-name, inferred): local names chosen for their stack slots (tools/vc6_locals.py), not recovered */
void Scenaric_DrawClassIcon(u32 *layer, u16 classId, int x, int y, u32 unused, int useIconB, u32 colorRGB)
{
    DavDirectory *table;
    float rhw;
    TlVertex *v;
    u32 wm1;
    DavBitmapRec *pic;
    u16 *icon;
    float x1;
    float yHi;
    RenderPoly *poly;
    s32 tu;
    float x_0;
    float top;
    u32 color2;
    u32 hm1;
    s32 v0;
    float z0;

    /* cast kept (both): the registry keeps an icon as an untyped resource pointer; the resource starts with its u16
     * image index */
    if (useIconB)
        icon = (u16 *)g_scenaricClassRegistry[classId].iconB;
    else
        /* cast kept: the class registry stores its icon bitmaps untyped */
        icon = (u16 *)g_scenaricClassRegistry[classId].iconA;
    if (icon) {
        table = g_pDav->header->dir;
        pic = &table->bitmaps[*icon];
        tu = pic->u;
        v0 = pic->v;
        wm1 = pic->width - 1;
        hm1 = pic->height - 1;
        x_0 = g_screen.ScaleX(x - (wm1 >> 1));
        top = g_screen.ScaleY(y - (hm1 >> 1));
        x1 = g_screen.ScaleX(x + (wm1 >> 1));
        yHi = g_screen.ScaleY(y + (hm1 >> 1));
        z0 = g_screen.Draw2D_LayerToZ(layer) - 0.01f;
        rhw = 1.0f / (g_pViewFrustum->nearZ); /* the parentheses load the divisor first (fld / fdivr) */
        color2 = ((colorRGB >> 16) & 0xff) + ((colorRGB << 16) & 0xff0000) + (colorRGB & 0xff00);
        poly = new RenderPoly;
        poly->type = pic->page + RPOLY_TEXTURED_BASE;
        poly->sortZ = z0;
        v = (TlVertex *)poly->verts; /* cast kept: vertex memory is untyped; this textured poly's are TlVertex */
        v[0].x = x_0;
        v[0].y = top;
        v[0].z = z0;
        v[0].rhw = rhw;
        v[0].diffuse = color2;
        v[0].specular = 0xff000000;
        v[1].x = x1;
        v[1].y = top;
        v[1].z = z0;
        v[1].rhw = rhw;
        v[1].diffuse = color2;
        v[1].specular = 0xff000000;
        v[2].x = x1;
        v[2].y = yHi;
        v[2].z = z0;
        v[2].rhw = rhw;
        v[2].diffuse = color2;
        v[2].specular = 0xff000000;
        v[0].u = Tex_CornerUV(0, wm1, tu);
        v[0].v = Tex_CornerUV(0, hm1, v0);
        v[1].u = Tex_CornerUV(wm1, wm1, tu);
        v[1].v = Tex_CornerUV(0, hm1, v0);
        v[2].u = Tex_CornerUV(wm1, wm1, tu);
        v[2].v = Tex_CornerUV(hm1, hm1, v0);
        g_pPolyBin->SubmitPolyInline(poly);
        v[1].x = x_0;
        v[1].y = yHi;
        v[1].u = Tex_CornerUV(0, wm1, tu);
        v[1].v = Tex_CornerUV(hm1, hm1, v0);
        g_pPolyBin->SubmitPoly(poly);
        delete poly;
    }
}

/* 0x5154ce - this = src moved by offset (16-bit adds, so a coordinate past +-32767 wraps), flags copied. */
void CollBox::Box_Translate(const CollBox *src, const Vec3s *offset)
{
    min.x = src->min.x + offset->x;
    min.y = src->min.y + offset->y;
    min.z = src->min.z + offset->z;
    max.x = src->max.x + offset->x;
    max.y = src->max.y + offset->y;
    max.z = src->max.z + offset->z;
    flags = src->flags;
}

/* 0x51556d, 72 bytes. Writes the terminator before copying eight bytes. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
void Scn_GetPropString8(void *props, s32 propOffset, char *out9)
{
    struct {
        char *source;
        s32 i;
    } w;
    out9[8] = 0;
    /* cast kept: designer properties are slots at byte offsets of the raw WAR record */
    w.source = (char *)props + propOffset + 0x14;
    for (w.i = 0; w.i < 8; w.i++)
        out9[w.i] = w.source[w.i];
}

/* 0x5155b5, 68 bytes. Does not inspect the returned element count. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; padding fill gaps */
Box *Scn_GetPropBox(void *props, s32 propOffset)
{
    struct {
        u32 *list;
        u16 padding;
        u16 count;
        u32 id;
    } w;
    /* cast kept: designer properties are 4-byte slots at byte offsets of the raw WAR record */
    w.id = *(u32 *)((u8 *)props + propOffset + 0x14);
    w.list = Scn_FindIdList((u16)w.id, &w.count);
    /* cast kept: an export id list holds record pointers as u32 words; the caller knows the kind it asked for */
    return w.list ? (Box *)w.list[0] : 0;
}

/* 0x5155f9, 55 bytes. Only a zero full dword bypasses the lookup. */
void **Scn_GetPropIdList(void *props, u32 propOffset, u16 *countOut)
{
    /* cast kept: designer properties are 4-byte slots at byte offsets of the raw WAR record */
    u32 id = *(u32 *)((u8 *)props + propOffset + 0x14);
    if (id)
        /* cast kept: an export id list holds record pointers as u32 words; the caller knows the kind it asked for */
        return (void **)Scn_FindIdList((u16)id, countOut);
    else {
        *countOut = 0;
        return 0;
    }
}

/* 0x515630, 56 bytes. Resolves the referenced exported record to a live object. */
ScnObject *Scn_GetPropObject(void *props, s32 propOffset)
{
    /* cast kept: designer properties are 4-byte slots at byte offsets of the raw WAR record */
    u32 id = *(u32 *)((u8 *)props + propOffset + 0x14);
    return id ? Scenaric_FindByIdList((u16)id) : 0;
}

/* 0x515668, 68 bytes. Same body as Box/Camera apart from its call displacement. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; padding fill gaps */
Trajectory *Scn_GetPropTrajectory(void *props, s32 propOffset)
{
    struct {
        u32 *list;
        u16 padding;
        u16 count;
        u32 id;
    } w;
    /* cast kept: designer properties are 4-byte slots at byte offsets of the raw WAR record */
    w.id = *(u32 *)((u8 *)props + propOffset + 0x14);
    w.list = Scn_FindIdList((u16)w.id, &w.count);
    /* cast kept: an export id list holds record pointers as u32 words; the caller knows the kind it asked for */
    return w.list ? (Trajectory *)w.list[0] : 0;
}

/* 0x5156ac, 68 bytes. A CAMERA property: the camera setup record {u16 focal; s16 rot[3]; Vec3s eye}. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; padding fill gaps */
CamSetup *Scn_GetPropCamera(void *props, s32 propOffset)
{
    struct {
        u32 *list;
        u16 padding;
        u16 count;
        u32 id;
    } w;
    /* cast kept: designer properties are 4-byte slots at byte offsets of the raw WAR record */
    w.id = *(u32 *)((u8 *)props + propOffset + 0x14);
    w.list = Scn_FindIdList((u16)w.id, &w.count);
    /* cast kept: an export id list holds record pointers as u32 words; the caller knows the kind it asked for */
    return w.list ? (CamSetup *)w.list[0] : 0;
}

/* 0x5156f0, 80 bytes. x/z are the horizontal coordinates. */
s32 Vec3s_ManhattanDistXZ(Vec3s *a, Vec3s *b)
{
    s32 total = a->x - b->x;
    if (total < 0)
        total = -total;
    s32 delta = a->z - b->z;
    if (delta < 0)
        return total - delta;
    else
        return total + delta;
}

/* 0x515740, 125 bytes. Accumulates X, Z, then vertical Y. */
s32 Vec3s_ManhattanDist(Vec3s *a, Vec3s *b)
{
    s32 total = a->x - b->x;
    if (total < 0)
        total = -total;
    s32 delta = a->z - b->z;
    if (delta < 0)
        total -= delta;
    else
        total += delta;
    delta = a->y - b->y;
    if (delta < 0)
        return total - delta;
    else
        return total + delta;
}

/* 0x5157bd, 86 bytes. X is promoted before multiplication; Z is squared in s32. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
s32 Vec3s_DistXZ(Vec3s *a, Vec3s *b)
{
    struct {
        s32 dx, dz;
    } w;
    w.dx = b->x - a->x;
    w.dz = b->z - a->z;
    return (s32)sqrt((double)w.dx * w.dx + w.dz * w.dz);
}

/* 0x515813, 123 bytes. Each square is s32; only the sum is floating point. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
s32 Vec3s_Dist(Vec3s *a, Vec3s *b)
{
    struct {
        s32 dx, dy, dz;
    } w;
    w.dx = b->x - a->x;
    w.dy = b->y - a->y;
    w.dz = b->z - a->z;
    w.dx *= w.dx;
    w.dy *= w.dy;
    w.dz *= w.dz;
    return (s32)sqrt((double)w.dx + w.dy + w.dz);
}

/* 0x51588e, 62 bytes. Squared distance retains the original s32 arithmetic. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
s32 Vec3s_DistSqXZ(Vec3s *a, Vec3s *b)
{
    struct {
        s32 dx, dz;
    } w;
    w.dx = b->x - a->x;
    w.dz = b->z - a->z;
    return w.dx * w.dx + w.dz * w.dz;
}

/* 0x5158cc, 104 bytes. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
s32 Vec3s_DistSq(Vec3s *a, Vec3s *b)
{
    struct {
        s32 dx, dy, dz;
    } w;
    w.dx = b->x - a->x;
    w.dy = b->y - a->y;
    w.dz = b->z - a->z;
    w.dx *= w.dx;
    w.dy *= w.dy;
    w.dz *= w.dz;
    return w.dx + w.dy + w.dz;
}

/* 0x515934, 204 bytes. Incoming height and box flags are not read. Failed queries leave outputs alone. */
/* BYTES(slot-group): locals grouped in w only to pin the original offsets: z at EBP-8, x at EBP-4 */
s32 Box_GroundQueryFlatTop(GroundQuery *query, CollBox *box, Vec3s *boxPos, s32 margin)
{
    /* Reconstructed stack work record: z at EBP-8, x at EBP-4. */
    struct {
        s32 z, x;
    } w;
    w.z = query->pos.z;
    w.x = query->pos.x;
    if (GroundOverlapFour(
            boxPos->x + box->max.x - margin - w.x, w.x - GroundTranslatedMinimum(margin, boxPos->x, box->min.x),
            boxPos->z + box->max.z - margin - w.z, w.z - GroundTranslatedMinimum(margin, boxPos->z, box->min.z))) {
        query->pos.y = boxPos->y + box->min.y;
        query->normal.z = 0;
        query->normal.x = 0;
        query->normal.y = -4096;
        return 1;
    } else {
        return 0;
    }
}

/* 0x515a00 - ground query against a box treated as a dome (Rock, SmallRock: message 0xd): a sphere of radius
 * r = (max.x - min.x) / 2 about the box centre. Inside the square |dx|, |dz| <= r - 1 the surface is
 * centre.y - sqrt(r*r - dx*dx - dz*dz) and the normal is normalize(dx, -h, dz). Returns 1 on a hit. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
s32 Box_GroundQueryDome(GroundQuery *query, CollBox *box, Vec3s *boxPos)
{
    struct {
        s32 z, x, height;
        s16 cx, cz;
        s32 innerRadius, dx, radius, dz;
    } w;
    w.cx = ((box->max.x + box->min.x) >> 1) + boxPos->x;
    w.cz = ((box->max.z + box->min.z) >> 1) + boxPos->z;
    w.innerRadius = ((box->max.x - box->min.x) >> 1) - 1;
    w.z = query->pos.z;
    w.x = query->pos.x;
    if (Overlap4(w.cx + w.innerRadius - w.x, w.x - (w.cx - w.innerRadius), w.cz + w.innerRadius - w.z,
                 w.z - (w.cz - w.innerRadius))) {
        w.dx = query->pos.x - w.cx;
        w.dz = query->pos.z - w.cz;
        w.radius = w.innerRadius + 1;
        query->normal.x = w.dx;
        query->normal.z = w.dz;
        w.dx *= w.dx;
        w.radius *= w.radius;
        w.dz *= w.dz;
        /* The square's corners lie outside the circle, where this argument is negative: sqrt returns NaN, __ftol (a
         * 64-bit fistp) keeps the low word of the integer indefinite, 0, and the query still succeeds, with the surface
         * at the box's vertical centre and a horizontal normal (dx, 0, dz). */
        w.height = (s32)sqrt((double)w.radius - (double)w.dx - (double)w.dz);
        query->pos.y = ((box->max.y + box->min.y) >> 1) + boxPos->y - w.height;
        query->normal.y = -w.height;
        Vec3s_Normalize(&query->normal, &query->normal);
        return 1;
    } else {
        return 0;
    }
}

/* 0x515b81  Rolls a 4.12 matrix by a ground displacement without slipping: an x move turns it about z, a z move about
 * x, by the arc angle displacement * 4096 / (2 pi radius) (31416 = pi * 10000); then renormalises the columns and writes
 * the rotation and translation back. The translation goes through (s16), so a coordinate beyond +-32767 wraps. */
void Matrix_RollByDisplacement(Mat34s *m, Vec3s *delta, s32 radius)
{
    Mat44 mat;
    Mat44 turn;
    Vec3s angle;
    s32 len;

    mat.SetIdentity();
    mat.m[0][0] = Math_Fixed12ToFloat_s16(m->rot[0]);
    mat.m[0][1] = Math_Fixed12ToFloat_s16(m->rot[3]);
    mat.m[0][2] = Math_Fixed12ToFloat_s16(m->rot[6]);
    mat.m[1][0] = Math_Fixed12ToFloat_s16(m->rot[1]);
    mat.m[1][1] = Math_Fixed12ToFloat_s16(m->rot[4]);
    mat.m[1][2] = Math_Fixed12ToFloat_s16(m->rot[7]);
    mat.m[2][0] = Math_Fixed12ToFloat_s16(m->rot[2]);
    mat.m[2][1] = Math_Fixed12ToFloat_s16(m->rot[5]);
    mat.m[2][2] = Math_Fixed12ToFloat_s16(m->rot[8]);
    mat.m[3][0] = (float)m->trans[0];
    mat.m[3][1] = (float)m->trans[1];
    mat.m[3][2] = (float)m->trans[2];
    if (delta->x | delta->z) {
        angle.y = 0;
        if (delta->x) {
            len = (delta->x << 11) * 10000 / 31416;
            angle.x = 0;
            angle.z = (len / radius) & 0xfff;
            turn.SetRotXZY(0.0f, 0.0f, Math_Angle4096ToRadians_2(angle.z));
            mat.MulInPlace(turn);
        }
        if (delta->z) {
            len = (-delta->z << 11) * 10000 / 31416;
            angle.z = 0;
            angle.x = (len / radius) & 0xfff;
            turn.SetRotXZY(Math_Angle4096ToRadians_2(angle.x), 0.0f, 0.0f);
            mat.MulInPlace(turn);
        }
        mat.NormalizeColumnsInPlace();
        m->rot[0] = Math_FloatToFixed12_s16(mat.m[0][0]);
        m->rot[1] = Math_FloatToFixed12_s16(mat.m[1][0]);
        m->rot[2] = Math_FloatToFixed12_s16(mat.m[2][0]);
        m->rot[3] = Math_FloatToFixed12_s16(mat.m[0][1]);
        m->rot[4] = Math_FloatToFixed12_s16(mat.m[1][1]);
        m->rot[5] = Math_FloatToFixed12_s16(mat.m[2][1]);
        m->rot[6] = Math_FloatToFixed12_s16(mat.m[0][2]);
        m->rot[7] = Math_FloatToFixed12_s16(mat.m[1][2]);
        m->rot[8] = Math_FloatToFixed12_s16(mat.m[2][2]);
        m->trans[0] = (s16)mat.m[3][0];
        m->trans[1] = (s16)mat.m[3][1];
        m->trans[2] = (s16)mat.m[3][2];
    }
}

void *g_voiceOwner; /* 0x6d411c  the owner token of the voice line now streaming */

/* 0x515e50  Starts voice clip voiceId in the menu language, unless the stream player is busy with something other than
 * music; remembers owner so only it can stop the line. Returns 1 when the line started. */
s32 Voice_PlayStream(u32 voiceId, void *owner)
{
    u8 voiceLang;
    s32 state;

    if (!voiceId)
        return 0;
    state = g_pStreamPlayer->IsMusic();
    if (state != 0 && state != 1)
        return 0;
    voiceLang = g_pProgress->language;
    g_pStreamPlayer->PlayVoice(voiceId, voiceLang, 0, 1);
    g_voiceOwner = owner;
    return 1;
}

/* 0x515eb5  Stops the voice line, but only for the object that started it. */
void Voice_StopStream(void *owner)
{
    if (g_voiceOwner == owner) {
        g_pStreamPlayer->StopVoice();
        g_voiceOwner = 0;
    }
}

/* 0x515ed9  Returns 0; unreferenced. */
s32 Scenaric_Stub_Return0_515ed9()
{
    return 0;
}

/* 0x515ee0 .. 0x515eef  Empty, unreferenced. */
void Scenaric_Stub_515ee0() {}

void Scenaric_Stub_515ee5() {}

void Scenaric_Stub_515eea() {}

void Scenaric_Stub_515eef() {}
