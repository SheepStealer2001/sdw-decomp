/*
 * T297 - original object guessed as PackJpeg.cpp (tu_map). Ranges: .text 0x55dab0-0x55e480, .data 0x57eb08-0x57eb64 (the
 * string literals: ".SDW", "rb", then the two MessageBoxA strings of BeginImage), .bss 0x7195d8-0x719620 (g_packJpeg,
 * g_packJpegTextures, g_packJpegPixels, g_packJpegOpen): the pack helpers 0x55dab0-0x55dd11, then the image code
 * 0x55dd12-0x55e47f.
 *
 *  - packedJpegErrorTitle: in the exe the title "SDW Error" is a .data literal placed between this object's other
 *    literals (0x57eb14, before the 65-byte message: /Od emits the literals of a call in push order, right to left), so
 *    the original passed a literal. It is a macro for that literal.
 *  - Texture::Surface_LockForWrite is declared returning long, as src/engine/texture.cpp defines it (the return is
 *    ignored, same code; declared void it decorates to a name nothing defines).
 *  - .bss: defined here in address order with explicit zero initialisers; VC6 keeps a zero-initialised global in
 *    definition order (uninitialised ones would be ordered by a hash of their names, src/README.md).
 *  - SelectByName/SelectAndFetch/Open return a full EAX; the original buffer lifetime, inclusive first copy loop and
 *    fixed 256-pixel pitch are preserved in DecodeToTextures.
 */
/* BYTES: layout, slot-group. */
/* BYTES(layout): packedJpegErrorTitle macro: a literal, not a static const array: the original passes a literal (.data, 0x57eb14) */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "../sdk/crt.h"
#include "../sdk/win32.h"
#include "../sdk/ddraw.h"

#define SDW_MEMBERS_Texture                  \
    Texture(D3DApp *, u32, u32, u32, s32 *); \
    long Surface_LockForWrite(DDSURFACEDESC2 *);
#include "sdw_classes.h"
#include "../engine/maths.h"
#include "../engine/jpeg_mlt.h"
#include "../engine/draw2d.h"
#include "../engine/text.h"
#include "../engine/screen.h"
#define SDW_INLINE_PACKJPEG_GETCOUNT 1
#include "pack_jpeg_inlines.h"
#undef SDW_INLINE_PACKJPEG_GETCOUNT
extern u32 *g_screenLayerBase;
void Draw2D_TexRect_Immediate(float, float, float, float, float, u32, Texture *, float, float, u32, float, float, u32,
                              float, float, u32, float, float, u32);
#define packedJpegErrorTitle "SDW Error" /* a literal in the original (see the header) */

/* ---- this object's .bss, in address order ---- */
PackJpeg g_packJpeg = {0};            /* 0x7195d8  the open pack (Bonus gallery) */
Texture *g_packJpegTextures[4] = {0}; /* 0x7195f8  the four 256x256 quarter textures */
u16 *g_packJpegPixels[4] = {0};       /* 0x719608  their locked texels while decoding */
s32 g_packJpegOpen = 0;               /* 0x719618  1 while a pack is open */

s32 PackJpeg::Prev()
{
    if (index > 0)
        --index;
    SetIndex(index);
    return index == 0;
}
s32 PackJpeg::Next()
{
    if (index < GetCount() - 1)
        ++index;
    SetIndex(index);
    return index == GetCount() - 1;
}
void PackJpeg::Close()
{
    g_packJpegOpen = 0;
    if (entries) {
        free(entries);
        entries = 0;
    }
    fclose(file);
}
s32 PackJpeg::SelectByName(const char *name)
{
    u32 i = 0;
    while (i < GetCount() && !Str_IsPrefixOf(entries[i].name, name))
        ++i;
    if (i == GetCount())
        return 0;
    SetIndex(i);
    return 1;
}
s32 PackJpeg::SelectAndFetch(const char *name, void *out)
{
    if (SelectByName(name)) {
        memcpy(out, &opaqueValue, 4);
        return 1;
    }
    return 0;
}
s32 PackJpeg::Open(const char *pathNoExt)
{
    char path[64];
    Str_Concat2(path, pathNoExt, ".SDW");
    file = fopen(path, "rb");
    if (file) {
        fread(this, 1, 12, file);
        /* cast kept: malloc returns untyped memory */
        entries = (PackJpegEntry *)malloc(count * sizeof(PackJpegEntry));
        fread(entries, 1, count * sizeof(PackJpegEntry), file);
        SetIndex(0);
        g_packJpegOpen = 1;
        return 1;
    }
    return 0;
}
s32 PackJpeg::SetIndex(u32 value)
{
    index = value;
    return 0;
}

/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
/* BYTES(slot-group): workspace grouped in one struct, members in reverse physical order of the original slots */
void PackJpeg::DecodeToTextures(s32 unused)
{
    /* Stack-only workspace, reverse physical order of the original local slots. */
    struct Work {
        u8 pad[3], slot;
        s32 row, sourceOffset, targetOffset;
        DDSURFACEDESC2 surface;
        s32 unusedSurfacePad, rowBytes;
        u16 *decoded;
    } w;
    w.decoded = new u16[image->byteSize / 2];
    fseek(file, entries[index].fileOffset, SEEK_SET);
    Jpeg_DecodeToRgb555Flipped(file, w.decoded);
    for (w.slot = 0; w.slot < 4; ++w.slot) {
        g_packJpegTextures[w.slot]->Surface_LockForWrite(&w.surface);
        /* cast kept: a locked surface is raw memory; these textures are 16-bit ARGB1555 */
        g_packJpegPixels[w.slot] = (u16 *)w.surface.lpSurface;
    }
    w.rowBytes = (image->width / 2) * 2;
    for (w.row = image->height / 2; w.row >= 0; --w.row) {
        w.sourceOffset = w.row * image->width;
        w.targetOffset = (image->height / 2 - w.row) * 256;
        memcpy(g_packJpegPixels[2] + w.targetOffset, w.decoded + w.sourceOffset, w.rowBytes);
        memcpy(g_packJpegPixels[3] + w.targetOffset, w.decoded + (w.sourceOffset + image->width / 2), w.rowBytes);
    }
    for (w.row = image->height / 2; w.row < image->height; ++w.row) {
        w.sourceOffset = w.row * image->width;
        w.targetOffset = (image->height - (w.row + 1)) * 256;
        memcpy(g_packJpegPixels[0] + w.targetOffset, w.decoded + w.sourceOffset, w.rowBytes);
        memcpy(g_packJpegPixels[1] + w.targetOffset, w.decoded + (w.sourceOffset + image->width / 2), w.rowBytes);
    }
    for (w.slot = 0; w.slot < 4; ++w.slot)
        g_packJpegTextures[w.slot]->Surface_Unlock();
}

/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets */
void PackJpeg::BeginImage()
{
    struct Work {
        s32 result;
        u8 pad[3], slot;
        u32 width, height;
    } w;
    image = (PackJpegImage *)malloc(sizeof(PackJpegImage)); /* cast kept: malloc returns untyped memory */
    fseek(file, entries[index].fileOffset, SEEK_SET);
    if (Jpeg_GetDimensions(file, &w.width, &w.height) == 1) {
        image->width = (s16)w.width;
        image->height = (s16)w.height;
        image->byteSize = w.width * w.height * 2;
        for (w.slot = 0; w.slot < 4; ++w.slot)
            if (!g_packJpegTextures[w.slot])
                g_packJpegTextures[w.slot] = new Texture(g_pD3DAppMain, 256, 256, TEXFMT_ARGB1555, &w.result);
    } else
        MessageBoxA(0, "SDW Packed JPEG file invalid : a non-JPEG file has been detected", packedJpegErrorTitle, MB_OK);
}

void PackJpeg::ReleaseImage()
{
    u8 i;
    if (image)
        free(image);
    for (i = 0; i < 4; ++i) {
        if (g_packJpegTextures[i]) {
            delete g_packJpegTextures[i];
            g_packJpegTextures[i] = 0;
        }
    }
}

/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets */
void PackJpeg::DrawImage()
{
    struct Work {
        float scale, u;
        u32 color;
        float bottom, v, right, midY, midX, top, left;
    } w;
    w.color = 0xffffff;
    w.top = 0;
    w.bottom = g_pViewFrustum->viewportHeight - g_screen.ScaleY(g_pCurFont->glyphHeight) * 2;
    w.left = (1.0f - (float)image->width / (float)image->height) * g_pViewFrustum->viewportWidth * 0.5f;
    w.right = (1.0f + (float)image->width / (float)image->height) * g_pViewFrustum->viewportWidth * 0.5f;
    if (w.left < 0.0f) {
        w.scale = g_pViewFrustum->viewportWidth / (w.right - w.left);
        w.bottom *= w.scale;
        w.left = 0;
        w.right = g_pViewFrustum->viewportWidth - 1.0f;
    }
    w.midY = (w.bottom - w.top) / 2.0f;
    w.midX = (w.right - w.left) / 2.0f;
    w.u = (float)(image->width / 2) / 256.0f;
    w.v = (float)(image->height / 2) / 256.0f;
#define QUAD(tile, x0, y0, x1, y1)                                                                                \
    Draw2D_TexRect_Immediate(g_screen.Draw2D_LayerToZ(g_screenLayerBase + 1), x0, y0, x1, y1, RSF_TEXTURED,       \
                             g_packJpegTextures[tile], 0, 0, w.color, 0, w.v, w.color, w.u, 0, w.color, w.u, w.v, \
                             w.color)
    QUAD(0, w.left, w.top, w.midX, w.midY);
    QUAD(1, w.midX, w.top, w.right, w.midY);
    QUAD(2, w.left, w.midY, w.midX, w.bottom);
    QUAD(3, w.midX, w.midY, w.right, w.bottom);
#undef QUAD
}
