/*
 * T253 - guessed original name: CollClip.cpp. SheepD3D.exe .text 0x5169c0-0x5196b9. No data of its own (it uses the
 * shared scratch g_clipTriBuffer 0x6d5668 and g_sharedScratch, defined in src/engine/draw2d.cpp, T256).
 * The six axis-plane polygon clippers (Poly_Clip*) and the box sweeps built on them: box vs triangle (twice: the
 * general one and Sam's min/max variant), the X/Z polygon clip, the clip of a triangle to a box, box vs box, and the
 * shaped targets (cylinder, cone, ellipsoid). In address order: Poly_ClipMinX .. Poly_ClipMaxZ (0x5169c0-0x5175d9);
 * Collide_BoxTriangleSweep, Collide_ClipTriToBox, Collide_SweepBoxVsBox / VsCylinder / VsCone / VsEllipsoid and
 * Collide_BoxTriangleSweep_MinMaxY (0x5175da-0x5196b8, with Collide_BoxPolyClipXZ_MaxY at 0x517c4b among them).
 *
 * Linkage: Poly_Clip* and Collide_BoxPolyClipXZ_MaxY are C++, the sweep functions extern "C". The `w` / `top` structs
 * only pin the original stack offsets of locals (VC6 /Od orders locals by a hash of their names, src/README.md); they are
 * not a claim about the source text, and neither are the names of the inline helpers, which have no bodies of their own.
 *
 * Coordinates: game space is (x, y pointing DOWN, z). The static triangles and the triangle sweeps work in
 * "detector space": x16 and with the vertical and z negated (CollTri.verts is stored that way /16), hence the
 * `<< 4` and the sign flips when a CollBox is turned into a Box6i. Fractions are 4.12 (0x1000 = the whole move).
 * Every test here is against the END box (box + delta): a surface is found only if the box overlaps it where the
 * move ends, and the fraction is then worked back from the start position.
 */
/* BYTES: dead-code, inline, slot-group. */
/* BYTES(slot-group): TRI_AXIS_FRACTION macro (Collide_BoxTriangleSweep, Collide_BoxTriangleSweep_MinMaxY): each axis pass uses its own w members because each has its own slots in the original */
#include "sdw_classes.h"
#include "sdw_enums.h"

#include "../sdk/crt.h"

#include "fixed_math.h"
extern "C" {

/* src/engine/fixed_math.cpp */
s16 Math_RadiansToAngle4096(float radians);

extern const s16 g_sinTable4096[]; /* 0x57ece0  4.12 sine, 4096 steps per turn */
extern const s16 *g_pCosTable;     /* 0x5814e4  = g_sinTable4096 + 1024 */
extern Vec3i g_clipTriBuffer[];    /* 0x6d5668  ping-pong vertex buffer of the Poly_Clip* chain */
}

/* Inline helpers (no bodies in the exe; /Ob1 expands them). CollisionOverlapFour/Two: "all of these differences are
 * >= 0" computed as one OR of the sign bits, the shape the original uses for every AABB test here (or/or/or, not,
 * and 0x80000000). */
/* BYTES(inline): source-only inline: the original tests the AABB overlap as one OR of the sign bits (or / or / or, not, and 0x80000000) */
#define SDW_INLINE_FREE_COLLISIONOVERLAPFOUR_S32_S32_S32_S32 1
#define SDW_INLINE_FREE_COLLISIONOVERLAPTWO_S32_S32 1
#include "coll_box_inlines.h"
#undef SDW_INLINE_FREE_COLLISIONOVERLAPFOUR_S32_S32_S32_S32
#undef SDW_INLINE_FREE_COLLISIONOVERLAPTWO_S32_S32

/* ---- 0x5169c0-0x5175d9: Sutherland-Hodgman clip of a closed polygon (s32 vertices) against one axis plane ----
 * Each keeps the side of the plane named by the function (inclusive), inserts the crossing point on every edge that
 * changes side (t = -d0 * 4096 / (d1 - d0), the other two coordinates interpolated with >> 12), and returns the
 * number of vertices written to out. The six are identical but for the axis and the sign; only the X pair also
 * advances `out` after the closing edge, and only Poly_ClipMaxZ also tests the loop counter against 0xffffffff before
 * copying a vertex (a condition that can never fail inside the loop; it is in the original's bytes, 0x5174f4).
 * The box/triangle sweeps call them as MinX, MaxX, MinZ, MaxZ, MinY, MaxY, ping-ponging between the triangle's own
 * vertex buffer and a scratch buffer. */
struct ClipWork { /* one local, to pin the original's slots */
    s32 tClose, t;
    u32 written, i;
    Vec3i *next, *prev;
    Vec3i diff;
    s32 dist, step, nextDist;
};

/* 0x5169c0 - keeps the part of the polygon with x >= plane. */
/* BYTES(slot-group): locals grouped in ClipWork only to pin the original frame offsets */
u32 Poly_ClipMinX(s32 plane, Vec3i *in, u32 count, Vec3i *out)
{
    ClipWork w;
    w.written = 0;
    if (!count)
        return w.written;
    w.prev = in;
    w.dist = w.prev->x - plane;
    if (w.dist >= 0) {
        *out = *w.prev;
        ++out;
        ++w.written;
    }
    w.next = in + 1;
    for (w.i = 1; w.i != count; ++w.i) {
        w.diff.x = w.next->x - w.prev->x;
        w.diff.y = w.next->y - w.prev->y;
        w.diff.z = w.next->z - w.prev->z;
        w.step = w.diff.x;
        w.nextDist = w.dist + w.step;
        if ((w.dist ^ w.nextDist) < 0) { /* the edge crosses the plane */
            w.t = (-w.dist << 12) / w.step;
            out->x = plane;
            out->y = ((w.diff.y * w.t) >> 12) + w.prev->y;
            out->z = ((w.diff.z * w.t) >> 12) + w.prev->z;
            ++out;
            ++w.written;
        }
        if (w.nextDist >= 0) {
            *out = *w.next;
            ++out;
            ++w.written;
        }
        w.dist = w.nextDist;
        w.prev = w.next;
        ++w.next;
    }
    w.next = in; /* the closing edge, last vertex -> first */
    w.diff.x = w.next->x - w.prev->x;
    w.diff.y = w.next->y - w.prev->y;
    w.diff.z = w.next->z - w.prev->z;
    w.step = w.diff.x;
    w.nextDist = w.dist + w.step;
    if ((w.dist ^ w.nextDist) < 0) {
        w.tClose = (-w.dist << 12) / w.step;
        out->x = plane;
        out->y = ((w.diff.y * w.tClose) >> 12) + w.prev->y;
        out->z = ((w.diff.z * w.tClose) >> 12) + w.prev->z;
        ++out;
        ++w.written;
    }
    return w.written;
}

/* 0x516bc8 - keeps the part of the polygon with x <= plane. */
/* BYTES(slot-group): locals grouped in ClipWork only to pin the original frame offsets */
u32 Poly_ClipMaxX(s32 plane, Vec3i *in, u32 count, Vec3i *out)
{
    ClipWork w;
    w.written = 0;
    if (!count)
        return w.written;
    w.prev = in;
    w.dist = plane - w.prev->x;
    if (w.dist >= 0) {
        *out = *w.prev;
        ++out;
        ++w.written;
    }
    w.next = in + 1;
    for (w.i = 1; w.i != count; ++w.i) {
        w.diff.x = w.next->x - w.prev->x;
        w.diff.y = w.next->y - w.prev->y;
        w.diff.z = w.next->z - w.prev->z;
        w.step = -w.diff.x;
        w.nextDist = w.dist + w.step;
        if ((w.dist ^ w.nextDist) < 0) { /* the edge crosses the plane */
            w.t = (-w.dist << 12) / w.step;
            out->x = plane;
            out->y = ((w.diff.y * w.t) >> 12) + w.prev->y;
            out->z = ((w.diff.z * w.t) >> 12) + w.prev->z;
            ++out;
            ++w.written;
        }
        if (w.nextDist >= 0) {
            *out = *w.next;
            ++out;
            ++w.written;
        }
        w.dist = w.nextDist;
        w.prev = w.next;
        ++w.next;
    }
    w.next = in; /* the closing edge, last vertex -> first */
    w.diff.x = w.next->x - w.prev->x;
    w.diff.y = w.next->y - w.prev->y;
    w.diff.z = w.next->z - w.prev->z;
    w.step = -w.diff.x;
    w.nextDist = w.dist + w.step;
    if ((w.dist ^ w.nextDist) < 0) {
        w.tClose = (-w.dist << 12) / w.step;
        out->x = plane;
        out->y = ((w.diff.y * w.tClose) >> 12) + w.prev->y;
        out->z = ((w.diff.z * w.tClose) >> 12) + w.prev->z;
        ++out;
        ++w.written;
    }
    return w.written;
}

/* 0x516dd4 - keeps the part of the polygon with y >= plane. */
/* BYTES(slot-group): locals grouped in ClipWork only to pin the original frame offsets */
u32 Poly_ClipMinY(s32 plane, Vec3i *in, u32 count, Vec3i *out)
{
    ClipWork w;
    w.written = 0;
    if (!count)
        return w.written;
    w.prev = in;
    w.dist = w.prev->y - plane;
    if (w.dist >= 0) {
        *out = *w.prev;
        ++out;
        ++w.written;
    }
    w.next = in + 1;
    for (w.i = 1; w.i != count; ++w.i) {
        w.diff.x = w.next->x - w.prev->x;
        w.diff.y = w.next->y - w.prev->y;
        w.diff.z = w.next->z - w.prev->z;
        w.step = w.diff.y;
        w.nextDist = w.dist + w.step;
        if ((w.dist ^ w.nextDist) < 0) { /* the edge crosses the plane */
            w.t = (-w.dist << 12) / w.step;
            out->x = ((w.diff.x * w.t) >> 12) + w.prev->x;
            out->y = plane;
            out->z = ((w.diff.z * w.t) >> 12) + w.prev->z;
            ++out;
            ++w.written;
        }
        if (w.nextDist >= 0) {
            *out = *w.next;
            ++out;
            ++w.written;
        }
        w.dist = w.nextDist;
        w.prev = w.next;
        ++w.next;
    }
    w.next = in; /* the closing edge, last vertex -> first */
    w.diff.x = w.next->x - w.prev->x;
    w.diff.y = w.next->y - w.prev->y;
    w.diff.z = w.next->z - w.prev->z;
    w.step = w.diff.y;
    w.nextDist = w.dist + w.step;
    if ((w.dist ^ w.nextDist) < 0) {
        w.tClose = (-w.dist << 12) / w.step;
        out->x = ((w.diff.x * w.tClose) >> 12) + w.prev->x;
        out->y = plane;
        out->z = ((w.diff.z * w.tClose) >> 12) + w.prev->z;
        ++w.written;
    }
    return w.written;
}

/* 0x516fd2 - keeps the part of the polygon with y <= plane. */
/* BYTES(slot-group): locals grouped in ClipWork only to pin the original frame offsets */
u32 Poly_ClipMaxY(s32 plane, Vec3i *in, u32 count, Vec3i *out)
{
    ClipWork w;
    w.written = 0;
    if (!count)
        return w.written;
    w.prev = in;
    w.dist = plane - w.prev->y;
    if (w.dist >= 0) {
        *out = *w.prev;
        ++out;
        ++w.written;
    }
    w.next = in + 1;
    for (w.i = 1; w.i != count; ++w.i) {
        w.diff.x = w.next->x - w.prev->x;
        w.diff.y = w.next->y - w.prev->y;
        w.diff.z = w.next->z - w.prev->z;
        w.step = -w.diff.y;
        w.nextDist = w.dist + w.step;
        if ((w.dist ^ w.nextDist) < 0) { /* the edge crosses the plane */
            w.t = (-w.dist << 12) / w.step;
            out->x = ((w.diff.x * w.t) >> 12) + w.prev->x;
            out->y = plane;
            out->z = ((w.diff.z * w.t) >> 12) + w.prev->z;
            ++out;
            ++w.written;
        }
        if (w.nextDist >= 0) {
            *out = *w.next;
            ++out;
            ++w.written;
        }
        w.dist = w.nextDist;
        w.prev = w.next;
        ++w.next;
    }
    w.next = in; /* the closing edge, last vertex -> first */
    w.diff.x = w.next->x - w.prev->x;
    w.diff.y = w.next->y - w.prev->y;
    w.diff.z = w.next->z - w.prev->z;
    w.step = -w.diff.y;
    w.nextDist = w.dist + w.step;
    if ((w.dist ^ w.nextDist) < 0) {
        w.tClose = (-w.dist << 12) / w.step;
        out->x = ((w.diff.x * w.tClose) >> 12) + w.prev->x;
        out->y = plane;
        out->z = ((w.diff.z * w.tClose) >> 12) + w.prev->z;
        ++w.written;
    }
    return w.written;
}

/* 0x5171d4 - keeps the part of the polygon with z >= plane. */
/* BYTES(slot-group): locals grouped in ClipWork only to pin the original frame offsets */
u32 Poly_ClipMinZ(s32 plane, Vec3i *in, u32 count, Vec3i *out)
{
    ClipWork w;
    w.written = 0;
    if (!count)
        return w.written;
    w.prev = in;
    w.dist = w.prev->z - plane;
    if (w.dist >= 0) {
        *out = *w.prev;
        ++out;
        ++w.written;
    }
    w.next = in + 1;
    for (w.i = 1; w.i != count; ++w.i) {
        w.diff.x = w.next->x - w.prev->x;
        w.diff.y = w.next->y - w.prev->y;
        w.diff.z = w.next->z - w.prev->z;
        w.step = w.diff.z;
        w.nextDist = w.dist + w.step;
        if ((w.dist ^ w.nextDist) < 0) { /* the edge crosses the plane */
            w.t = (-w.dist << 12) / w.step;
            out->x = ((w.diff.x * w.t) >> 12) + w.prev->x;
            out->y = ((w.diff.y * w.t) >> 12) + w.prev->y;
            out->z = plane;
            ++out;
            ++w.written;
        }
        if (w.nextDist >= 0) {
            *out = *w.next;
            ++out;
            ++w.written;
        }
        w.dist = w.nextDist;
        w.prev = w.next;
        ++w.next;
    }
    w.next = in; /* the closing edge, last vertex -> first */
    w.diff.x = w.next->x - w.prev->x;
    w.diff.y = w.next->y - w.prev->y;
    w.diff.z = w.next->z - w.prev->z;
    w.step = w.diff.z;
    w.nextDist = w.dist + w.step;
    if ((w.dist ^ w.nextDist) < 0) {
        w.tClose = (-w.dist << 12) / w.step;
        out->x = ((w.diff.x * w.tClose) >> 12) + w.prev->x;
        out->y = ((w.diff.y * w.tClose) >> 12) + w.prev->y;
        out->z = plane;
        ++w.written;
    }
    return w.written;
}

/* 0x5173d2 - keeps the part of the polygon with z <= plane. */
/* BYTES(slot-group): locals grouped in ClipWork only to pin the original frame offsets */
/* BYTES(dead-code): the counter test against 0xffffffff can never fail; it is in the original (0x5174f4) */
u32 Poly_ClipMaxZ(s32 plane, Vec3i *in, u32 count, Vec3i *out)
{
    ClipWork w;
    w.written = 0;
    if (!count)
        return w.written;
    w.prev = in;
    w.dist = plane - w.prev->z;
    if (w.dist >= 0) {
        *out = *w.prev;
        ++out;
        ++w.written;
    }
    w.next = in + 1;
    for (w.i = 1; w.i != count; ++w.i) {
        w.diff.x = w.next->x - w.prev->x;
        w.diff.y = w.next->y - w.prev->y;
        w.diff.z = w.next->z - w.prev->z;
        w.step = -w.diff.z;
        w.nextDist = w.dist + w.step;
        if ((w.dist ^ w.nextDist) < 0) { /* the edge crosses the plane */
            w.t = (-w.dist << 12) / w.step;
            out->x = ((w.diff.x * w.t) >> 12) + w.prev->x;
            out->y = ((w.diff.y * w.t) >> 12) + w.prev->y;
            out->z = plane;
            ++out;
            ++w.written;
        }
        if (w.nextDist >= 0 && w.i != 0xffffffff) {
            *out = *w.next;
            ++out;
            ++w.written;
        }
        w.dist = w.nextDist;
        w.prev = w.next;
        ++w.next;
    }
    w.next = in; /* the closing edge, last vertex -> first */
    w.diff.x = w.next->x - w.prev->x;
    w.diff.y = w.next->y - w.prev->y;
    w.diff.z = w.next->z - w.prev->z;
    w.step = -w.diff.z;
    w.nextDist = w.dist + w.step;
    if ((w.dist ^ w.nextDist) < 0) {
        w.tClose = (-w.dist << 12) / w.step;
        out->x = ((w.diff.x * w.tClose) >> 12) + w.prev->x;
        out->y = ((w.diff.y * w.tClose) >> 12) + w.prev->y;
        out->z = plane;
        ++w.written;
    }
    return w.written;
}

extern "C" {

/* The three axis passes of the triangle sweeps: the fraction of the move at which the box's leading face reaches the
 * nearest clipped vertex on that axis, clamped to [0, 0x1000]. Each pass has its own original local slots. */
#define TRI_AXIS_FRACTION(axis, k, result, low, lowTemp, high, highTemp) \
    w.result = 4096;                                                     \
    if (delta->axis > 0) {                                               \
        w.cursor = vertices;                                             \
        w.low = w.cursor->axis;                                          \
        for (w.i = 1; w.i < w.count; ++w.i) {                            \
            ++w.cursor;                                                  \
            w.lowTemp = w.cursor->axis;                                  \
            if (w.low > w.lowTemp)                                       \
                w.low = w.lowTemp;                                       \
        }                                                                \
        w.result = ((w.low - box->max[k]) << 12) / delta->axis;          \
        if (w.result < 0)                                                \
            w.result = 0;                                                \
        else if (w.result > 4096)                                        \
            w.result = 4096;                                             \
    }                                                                    \
    if (delta->axis < 0) {                                               \
        w.cursor = vertices;                                             \
        w.high = w.cursor->axis;                                         \
        for (w.i = 1; w.i < w.count; ++w.i) {                            \
            ++w.cursor;                                                  \
            highTemp = w.cursor->axis;                                   \
            if (w.high < highTemp)                                       \
                w.high = highTemp;                                       \
        }                                                                \
        w.result = ((box->min[k] - w.high) << 12) / -delta->axis;        \
        if (w.result < 0)                                                \
            w.result = 0;                                                \
        else if (w.result > 4096)                                        \
            w.result = 4096;                                             \
    }

/* 0x5175da: sweep a detector-space box by delta against one polygon (in place in `vertices`, which it clips to the
 * END box with the six Poly_Clip* passes, ping-ponging through g_clipTriBuffer). Returns the clipped vertex count
 * (0 = no contact); *height = the highest clipped vertical, *fraction = when the box meets the polygon: along the
 * normal when the start corner is more than 0x10000 behind the plane, else the smallest of the per-axis fractions. */
/* BYTES(slot-group): locals grouped in w / top only to pin the original frame offsets */
u32 Collide_BoxTriangleSweep(Box6i *box, Vec3s *delta, Vec3i *vertices, u32 vertexCount, s32 *fraction, s32 *height,
                             Vec3s *normal)
{
    struct {
        s32 zMax, zMinTemp, zMin;
        s32 vMaxTemp, vMax, vMinTemp, vMin;
        s32 xMaxTemp, xMax, xMinTemp, xMin, heightTemp;
        Vec3i *scratch;
        u32 count;
        s32 fractionZ;
        u32 i;
        s32 fractionV, fractionX;
        Vec3i *cursor;
        Box6i destination;
        Vec3i corner;
    } w;
    struct {
        s32 zMaxTemp;
    } top;
    w.scratch = g_clipTriBuffer;
    w.destination = *box;
    w.destination.min[0] += delta->x;
    w.destination.min[1] += delta->y;
    w.destination.min[2] += delta->z;
    w.destination.max[0] += delta->x;
    w.destination.max[1] += delta->y;
    w.destination.max[2] += delta->z;
    *height = -0x100000;
    *fraction = 4096;
    w.count = Poly_ClipMinX(w.destination.min[0], vertices, vertexCount, w.scratch);
    w.count = Poly_ClipMaxX(w.destination.max[0], w.scratch, w.count, vertices);
    w.count = Poly_ClipMinZ(w.destination.min[2], vertices, w.count, w.scratch);
    w.count = Poly_ClipMaxZ(w.destination.max[2], w.scratch, w.count, vertices);
    w.count = Poly_ClipMinY(w.destination.min[1], vertices, w.count, w.scratch);
    w.count = Poly_ClipMaxY(w.destination.max[1], w.scratch, w.count, vertices);
    if (!w.count)
        return 0;
    w.cursor = vertices;
    for (w.i = 0; w.i < w.count; ++w.i, ++w.cursor) {
        w.heightTemp = w.cursor->y;
        if (*height < w.heightTemp)
            *height = w.heightTemp;
    }
    /* the start box's corner that leads along the normal, back in game space */
    if (normal->x > 0)
        w.corner.x = box->min[0];
    else
        w.corner.x = box->max[0];
    if (normal->y > 0)
        w.corner.y = -box->max[1];
    else
        w.corner.y = -box->min[1];
    if (normal->z > 0)
        w.corner.z = -box->max[2];
    else
        w.corner.z = -box->min[2];
    w.corner.x = vertices->x - w.corner.x;
    w.corner.y = -vertices->y - w.corner.y;
    w.corner.z = -vertices->z - w.corner.z;
    w.fractionX = w.corner.x * normal->x + w.corner.y * normal->y + w.corner.z * normal->z;
    if (w.fractionX < -65536) {
        w.fractionV = (normal->x * delta->x - normal->y * delta->y - normal->z * delta->z) >> 4;
        if (w.fractionV) {
            w.fractionX = (w.fractionX << 8) / w.fractionV;
            if (w.fractionX < 0)
                w.fractionX = 0;
            else if (w.fractionX > 4096)
                w.fractionX = 4096;
            *fraction = w.fractionX;
        } else
            *fraction = 0;
    } else {
        TRI_AXIS_FRACTION(x, 0, fractionX, xMin, xMinTemp, xMax, w.xMaxTemp)
        TRI_AXIS_FRACTION(y, 1, fractionV, vMin, vMinTemp, vMax, w.vMaxTemp)
        TRI_AXIS_FRACTION(z, 2, fractionZ, zMin, zMinTemp, zMax, top.zMaxTemp)
        if (w.fractionX > w.fractionV)
            w.fractionX = w.fractionV;
        if (w.fractionX > w.fractionZ)
            w.fractionX = w.fractionZ;
        *fraction = w.fractionX;
    }
    return w.count;
}

} /* extern "C" */

/* 0x517c4b: clip a polygon to the box on X and Z only; returns the vertex count left and the largest vertical of the
 * result in *maxY (-0x100000 when nothing is left). No callers. */
/* BYTES(slot-group): locals grouped in w / top only to pin the original frame offsets */
u32 Collide_BoxPolyClipXZ_MaxY(Box6i *box, Vec3i *vertices, u32 vertexCount, s32 *maxY)
{
    struct {
        Vec3i *scratch;
        u32 count, i;
        Vec3i *cursor;
    } w;
    struct {
        s32 heightTemp;
    } top;
    w.scratch = g_clipTriBuffer;
    *maxY = -0x100000;
    w.count = Poly_ClipMinX(box->min[0], vertices, vertexCount, w.scratch);
    w.count = Poly_ClipMaxX(box->max[0], w.scratch, w.count, vertices);
    w.count = Poly_ClipMinZ(box->min[2], vertices, w.count, w.scratch);
    w.count = Poly_ClipMaxZ(box->max[2], w.scratch, w.count, vertices);
    if (w.count == 0)
        return 0;
    w.cursor = vertices;
    for (w.i = 0; w.i < w.count; ++w.i, ++w.cursor) {
        top.heightTemp = w.cursor->y;
        if (*maxY < top.heightTemp)
            *maxY = top.heightTemp;
    }
    return w.count;
}

extern "C" {

/* 0x517d2f: clip a polygon (in place) to a detector-space box; returns the clipped vertex count, *height = the
 * highest clipped vertical (-0x100000 if nothing is left). The static half of Coll_BoxGroundQuery. */
/* BYTES(slot-group): locals grouped in w / top only to pin the original frame offsets */
u32 Collide_ClipTriToBox(Box6i *box, Vec3i *vertices, u32 vertexCount, s32 *height)
{
    struct {
        Vec3i *scratch;
        u32 count, i;
        Vec3i *cursor;
    } w;
    struct {
        s32 heightTemp;
    } top;
    w.scratch = g_clipTriBuffer;
    *height = -0x100000;
    w.count = Poly_ClipMinX(box->min[0], vertices, vertexCount, w.scratch);
    w.count = Poly_ClipMaxX(box->max[0], w.scratch, w.count, vertices);
    w.count = Poly_ClipMinZ(box->min[2], vertices, w.count, w.scratch);
    w.count = Poly_ClipMaxZ(box->max[2], w.scratch, w.count, vertices);
    w.count = Poly_ClipMinY(box->min[1], vertices, w.count, w.scratch);
    w.count = Poly_ClipMaxY(box->max[1], w.scratch, w.count, vertices);
    if (w.count > 0) {
        w.cursor = vertices;
        for (w.i = 0; w.i < w.count; ++w.i, ++w.cursor) {
            top.heightTemp = w.cursor->y;
            if (*height < top.heightTemp)
                *height = top.heightTemp;
        }
    }
    return w.count;
}

/* One axis of Collide_SweepBoxVsBox: if the start box was clear of the target on the side it moves from, the
 * fraction at which it reaches it; the earliest one wins and records its face (CollBoxFace). */
#define BOX_AXIS_FRACTION(axis, positive, negative, assignMask) \
    if (delta->axis > 0) {                                      \
        w.gap = target->min.axis - mover->max.axis;             \
        if (w.gap >= 0) {                                       \
            w.gap = (w.gap << 12) / delta->axis;                \
            if (w.gap <= *fraction) {                           \
                *fraction = w.gap;                              \
                w.mask assignMask positive;                     \
            }                                                   \
        }                                                       \
    } else if (delta->axis != 0) {                              \
        w.gap = target->max.axis - mover->min.axis;             \
        if (w.gap <= 0) {                                       \
            w.gap = (w.gap << 12) / delta->axis;                \
            if (w.gap <= *fraction) {                           \
                *fraction = w.gap;                              \
                w.mask assignMask negative;                     \
            }                                                   \
        }                                                       \
    }

/* 0x517e4b: sweep a game-space box against another. Contact only if the END box overlaps the target strictly on all
 * three axes; returns the CollBoxFace bits of the face hit first (CBF_ALREADY_INSIDE with fraction 0 when the start
 * box already overlapped), *fraction = that time minus one unit, *height = the lower of the two tops. */
/* BYTES(slot-group): locals grouped in w / top only to pin the original frame offsets; 'gap' is a filler member */
u32 Collide_SweepBoxVsBox(CollBox *mover, Vec3s *delta, CollBox *target, s32 *fraction, s32 *height)
{
    struct {
        u32 mask;
        s32 gap;
        CollBox destination;
    } w;
    w.mask = 0;
    w.destination.min.x = mover->min.x + delta->x;
    w.destination.min.y = mover->min.y + delta->y;
    w.destination.min.z = mover->min.z + delta->z;
    w.destination.max.x = mover->max.x + delta->x;
    w.destination.max.y = mover->max.y + delta->y;
    w.destination.max.z = mover->max.z + delta->z;
    if (!CollisionOverlapFour(target->max.x - w.destination.min.x, w.destination.max.x - target->min.x,
                              target->max.z - w.destination.min.z, w.destination.max.z - target->min.z) ||
        !CollisionOverlapTwo(target->max.y - w.destination.min.y, w.destination.max.y - target->min.y))
        return 0;
    *fraction = 4096;
    BOX_AXIS_FRACTION(x, CBF_HIT_MINX, CBF_HIT_MAXX, =)
    BOX_AXIS_FRACTION(y, CBF_HIT_MINY, CBF_HIT_MAXY, |=)
    BOX_AXIS_FRACTION(z, CBF_HIT_MINZ, CBF_HIT_MAXZ, |=)
    if (w.destination.min.y > target->min.y)
        *height = w.destination.min.y;
    else
        *height = target->min.y;
    if (!w.mask) {
        *fraction = 0;
        w.mask = CBF_ALREADY_INSIDE;
    } else {
        --*fraction;
        if (*fraction < 0)
            *fraction = 0;
    }
    return w.mask;
}
#undef BOX_AXIS_FRACTION

/* 0x51819d: COLLBOX_CYLINDER target - both boxes as vertical cylinders (radius = half the summed X widths). If the
 * END circles overlap: a horizontal contact from the ray/circle solve (normal from atan2 of the start offset) and a
 * vertical one from the top/bottom gap, each only while fewer than 16 contacts are recorded. */
/* BYTES(slot-group): locals grouped in w / top only to pin the original frame offsets */
s32 Collide_SweepBoxVsCylinder(ScnObject *owner, CollBox *mover, Vec3s *delta, CollBox *target, s32 *fraction,
                               s32 *height, CollContact *contacts, s32 *count)
{
    struct {
        s32 wideX, sqX, sqR, sqZ, t, dx2, dot2, dz2;
        CollContact *contact;
        s32 hit;
        s16 cx, cz, unused1C, radius, tx, tz;
        Vec3s relative;
        s16 unused0E, unused0C, angle;
        s32 best, dot;
    } w;
    struct {
        s32 wideZ;
    } top;
    w.hit = 0;
    if (*count >= 16)
        return 0;
    w.cx = ((mover->max.x + mover->min.x) >> 1) + delta->x;
    w.cz = ((mover->max.z + mover->min.z) >> 1) + delta->z;
    w.tx = (target->max.x + target->min.x) >> 1;
    w.tz = (target->max.z + target->min.z) >> 1;
    w.radius = (mover->max.x - mover->min.x + target->max.x - target->min.x) >> 1;
    top.wideZ = w.cz;
    w.wideX = w.cx;
    if (CollisionOverlapFour(w.tx + w.radius - w.wideX, w.wideX - (w.tx - w.radius), w.tz + w.radius - top.wideZ,
                             top.wideZ - (w.tz - w.radius)) &&
        CollisionOverlapTwo(target->max.y - (mover->min.y + delta->y), mover->max.y + delta->y - target->min.y)) {
        w.sqX = w.cx - w.tx;
        w.sqZ = w.cz - w.tz;
        w.sqR = w.radius;
        w.sqX *= w.sqX;
        w.sqR *= w.sqR;
        w.sqZ *= w.sqZ;
        if (w.sqX + w.sqZ <= w.sqR) {
            w.best = 4096;
            if (delta->x | delta->z) {
                w.sqX = w.cx - delta->x - w.tx;
                w.sqZ = w.cz - delta->z - w.tz;
                w.sqR = (s16)(w.radius + 1);
                w.relative.x = w.sqX;
                w.relative.y = 0;
                w.relative.z = w.sqZ;
                w.sqX *= w.sqX;
                w.sqR *= w.sqR;
                w.sqZ *= w.sqZ;
                w.dot = w.relative.x * delta->x + w.relative.y * delta->y + w.relative.z * delta->z;
                w.dx2 = delta->x;
                w.dz2 = delta->z;
                w.dot2 = w.dot;
                w.dx2 *= w.dx2;
                w.dot2 *= w.dot2;
                w.dz2 *= w.dz2;
                w.t = w.dot2 - (w.dx2 + w.dz2) * (w.sqX + w.sqZ - w.sqR);
                if (w.t >= 0) {
                    w.t = ((-w.dot - (s32)sqrt((double)w.t)) << 12) / (w.dx2 + w.dz2);
                    if (w.t <= w.best) {
                        w.best = w.t;
                        w.contact = &contacts[*count];
                        w.angle =
                            Math_RadiansToAngle4096((float)atan2((double)w.relative.x, (double)w.relative.z)) & 0xfff;
                        w.contact->normal.x = g_sinTable4096[w.angle];
                        w.contact->normal.y = 0;
                        w.contact->normal.z = g_pCosTable[w.angle];
                        w.contact->point.x = w.tx;
                        w.contact->point.y = target->max.y;
                        w.contact->point.z = w.tz;
                        w.contact->obj = owner;
                        ++*count;
                        w.hit = 1;
                    }
                }
            }
            if (*count < 16) {
                if (delta->y > 0) {
                    w.t = target->min.y - mover->max.y;
                    if (w.t >= 0) {
                        w.t = (w.t << 12) / delta->y;
                        if (w.t < w.best)
                            w.best = w.t;
                        w.contact = &contacts[*count];
                        w.contact->normal.x = 0;
                        w.contact->normal.y = -4096;
                        w.contact->normal.z = 0;
                        w.contact->point.x = w.tx;
                        w.contact->point.y = target->max.y;
                        w.contact->point.z = w.tz;
                        w.contact->obj = owner;
                        ++*count;
                        w.hit = 1;
                    }
                } else if (delta->y != 0) {
                    w.t = target->max.y - mover->min.y;
                    if (w.t <= 0) {
                        /* sic: the fraction is stored straight into best and the raw gap (<= 0) is then compared
                         * with it (0x518658-0x518666), so an upward contact always ends with fraction 0 */
                        w.best = (w.t << 12) / delta->y;
                        if (w.t < w.best)
                            w.best = w.t;
                        w.contact = &contacts[*count];
                        w.contact->normal.x = 0;
                        w.contact->normal.y = 4096;
                        w.contact->normal.z = 0;
                        w.contact->point.x = w.tx;
                        w.contact->point.y = target->max.y;
                        w.contact->point.z = w.tz;
                        w.contact->obj = owner;
                        ++*count;
                        w.hit = 1;
                    }
                }
            }
            if (w.hit) {
                if (mover->min.y + delta->y > target->min.y)
                    *height = mover->min.y + delta->y;
                else
                    *height = target->min.y - 1;
                --w.best;
                if (w.best < 0)
                    w.best = 0;
                *fraction = w.best;
            }
        }
    }
    return w.hit;
}

/* 0x518742: COLLBOX_CONE target: a cone standing on the target box's bottom, as high as the combined radius (half
 * the summed X widths) times 0xf09/0x578. A mover whose bottom is at or below the target's bottom is handled as a
 * cylinder; otherwise contact, with fraction 0, when the END centre is inside the cone's circle at the height of the
 * mover's END bottom, with a slanted normal (0xf09 horizontal, -0x578 vertical). */
/* BYTES(slot-group): locals grouped in w / top only to pin the original frame offsets */
s32 Collide_SweepBoxVsCone(ScnObject *owner, CollBox *mover, Vec3s *delta, CollBox *target, s32 *fraction, s32 *height,
                           CollContact *contacts, s32 *count)
{
    struct {
        s32 wideX, sqX, sqR, sqZ, coneHeight;
        CollContact *contact;
        s32 depth, hit;
        s16 cx, cz, unused0C, radius, tx, tz, unused04, angle;
    } w;
    struct {
        s32 wideZ;
    } top;
    w.hit = 0;
    if (mover->max.y >= target->max.y)
        return Collide_SweepBoxVsCylinder(owner, mover, delta, target, fraction, height, contacts, count);
    if (*count >= 16)
        return w.hit;
    w.cx = ((mover->max.x + mover->min.x) >> 1) + delta->x;
    w.cz = ((mover->max.z + mover->min.z) >> 1) + delta->z;
    w.tx = (target->max.x + target->min.x) >> 1;
    w.tz = (target->max.z + target->min.z) >> 1;
    w.radius = (mover->max.x - mover->min.x + target->max.x - target->min.x) >> 1;
    top.wideZ = w.cz;
    w.wideX = w.cx;
    if (CollisionOverlapFour(w.tx + w.radius - w.wideX, w.wideX - (w.tx - w.radius), w.tz + w.radius - top.wideZ,
                             top.wideZ - (w.tz - w.radius))) {
        w.coneHeight = (w.radius * 0xf09) / 0x578;
        w.depth = w.coneHeight - (target->max.y - (mover->max.y + delta->y));
        if (w.depth >= 0) {
            if (w.depth > w.coneHeight)
                w.depth = w.coneHeight;
            w.sqX = w.cx - w.tx;
            w.sqZ = w.cz - w.tz;
            w.sqR = w.radius * w.depth / w.coneHeight;
            w.sqX *= w.sqX;
            w.sqR *= w.sqR;
            w.sqZ *= w.sqZ;
            if (w.sqX + w.sqZ <= w.sqR) {
                *fraction = 0;
                w.contact = &contacts[*count];
                w.contact->normal.y = -0x578;
                w.angle = Math_RadiansToAngle4096((float)atan2((double)w.cx - (double)delta->x - (double)w.tx,
                                                               (double)w.cz - (double)delta->z - (double)w.tz)) &
                          0xfff;
                w.contact->normal.x = (g_sinTable4096[w.angle] * 0xf09) >> 12;
                w.contact->normal.z = (g_pCosTable[w.angle] * 0xf09) >> 12;
                w.contact->point.x = w.tx;
                w.contact->point.y = target->max.y;
                w.contact->point.z = w.tz;
                w.contact->obj = owner;
                ++*count;
                if (mover->min.y + delta->y > target->min.y)
                    *height = mover->min.y + delta->y;
                else
                    *height = target->min.y;
                w.hit = 1;
            }
        }
    }
    return w.hit;
}

/* 0x518a8d: COLLBOX_ELLIPSOID target (XZ radius half its X width, vertical radius half its height; the mover adds
 * half its own X width). The vertical offset is rescaled so the ellipsoid becomes a sphere; if the END box touches
 * it, the fraction comes from the ray/sphere solve and the normal points from the centre to the start position. */
/* BYTES(slot-group): locals grouped in w / top only to pin the original frame offsets; 'unused22' is a filler member */
s32 Collide_SweepBoxVsEllipsoid(ScnObject *owner, CollBox *mover, Vec3s *delta, CollBox *target, s32 *fraction,
                                s32 *height, CollContact *contacts, s32 *count)
{
    struct {
        s32 sqV, sqZ, dx2, dv2, dz2, scaledV;
        CollContact *contact;
        s32 t;
        s16 unused38, radiusV;
        s32 hit;
        s16 cx, cz, unused2C, radius;
        Vec3s center;
        s16 unused22;
        Vec3i relative;
        s32 distance;
        s16 unused10, radiusX;
        s32 dot;
        s16 unused08, moverRadius;
        s32 magnitude;
    } w;
    struct {
        s32 sqX;
    } top;
    w.hit = 0;
    if (*count >= 16)
        return w.hit;
    w.cx = ((mover->max.x + mover->min.x) >> 1) + delta->x;
    w.cz = ((mover->max.z + mover->min.z) >> 1) + delta->z;
    w.center.x = (target->max.x + target->min.x) >> 1;
    w.center.y = (target->max.y + target->min.y) >> 1;
    w.center.z = (target->max.z + target->min.z) >> 1;
    w.moverRadius = (mover->max.x - mover->min.x) >> 1;
    w.radiusX = (target->max.x - target->min.x) >> 1;
    w.radiusV = (target->max.y - target->min.y) >> 1;
    if (CollisionOverlapFour(
            w.center.x + w.radiusX - (w.cx - w.moverRadius), w.cx + w.moverRadius - (w.center.x - w.radiusX),
            w.center.z + w.radiusX - (w.cz - w.moverRadius), w.cz + w.moverRadius - (w.center.z - w.radiusX)) &&
        CollisionOverlapTwo(w.center.y + w.radiusV - (mover->min.y + delta->y),
                            mover->max.y + delta->y - (w.center.y - w.radiusV))) {
        w.radius = w.moverRadius + w.radiusX;
        top.sqX = w.cx - w.center.x;
        w.sqZ = w.cz - w.center.z;
        if (mover->max.y + delta->y <= w.center.y)
            w.sqV = (mover->max.y + delta->y - w.center.y) * w.radius / w.radiusV;
        else if (mover->min.y + delta->y >= w.center.y)
            w.sqV = (mover->min.y + delta->y - w.center.y) * w.radius / w.radiusV;
        else
            w.sqV = 0;
        top.sqX *= top.sqX;
        w.sqV *= w.sqV;
        w.sqZ *= w.sqZ;
        if (top.sqX + w.sqV + w.sqZ <= w.radius * w.radius) {
            w.radius = w.moverRadius + w.radiusX + 1;
            w.scaledV = delta->y * w.radius / w.radiusV;
            w.dx2 = delta->x;
            w.dv2 = w.scaledV;
            w.dz2 = delta->z;
            w.dx2 *= w.dx2;
            w.dv2 *= w.dv2;
            w.dz2 *= w.dz2;
            w.relative.x = w.cx - delta->x - w.center.x;
            w.relative.z = w.cz - delta->z - w.center.z;
            if (mover->max.y <= w.center.y)
                w.relative.y = (mover->max.y - w.center.y) * w.radius / w.radiusV;
            else if (mover->min.y >= w.center.y)
                w.relative.y = (mover->min.y - w.center.y) * w.radius / w.radiusV;
            else
                w.relative.y = 0;
            top.sqX = w.relative.x * w.relative.x;
            w.sqV = w.relative.y * w.relative.y;
            w.sqZ = w.relative.z * w.relative.z;
            w.magnitude = w.dx2 + w.dz2 + w.dv2;
            if (w.magnitude != 0) {
                w.dot = delta->x * w.relative.x + delta->z * w.relative.z + w.scaledV * w.relative.y;
                w.distance = top.sqX + w.sqZ + w.sqV - w.radius * w.radius;
                w.t = w.dot * w.dot - w.magnitude * w.distance;
                w.t = ((-w.dot - (s32)sqrt((double)w.t)) << 12) / w.magnitude;
                if (w.t < 0)
                    w.t = 0;
                if (w.t > 4096)
                    w.t = 4096;
                *fraction = w.t;
            } else
                *fraction = 0;
            w.contact = &contacts[*count];
            w.contact->normal.x = w.cx - delta->x - w.center.x;
            w.contact->normal.z = w.cz - delta->z - w.center.z;
            if (mover->max.y <= w.center.y)
                w.contact->normal.y = (mover->max.y - w.center.y) * w.radiusX / w.radiusV - 50;
            else if (mover->min.y >= w.center.y)
                w.contact->normal.y = (mover->min.y - w.center.y) * w.radiusX / w.radiusV + 50;
            else
                w.contact->normal.y = 0;
            Vec3s_Normalize(&w.contact->normal, &w.contact->normal);
            w.contact->point.x = w.center.x;
            w.contact->point.y = w.center.y;
            w.contact->point.z = w.center.z;
            w.contact->obj = owner;
            ++*count;
            if (mover->min.y + delta->y > target->min.y)
                *height = mover->min.y + delta->y;
            else
                *height = target->min.y;
            w.hit = 1;
        }
    }
    return w.hit;
}

/* 0x51902f: Collide_BoxTriangleSweep that also reports the LOWEST clipped vertical in *minHeight (Sam's sweep). */
/* BYTES(slot-group): locals grouped in w / top only to pin the original frame offsets */
u32 Collide_BoxTriangleSweep_MinMaxY(Box6i *box, Vec3s *delta, Vec3i *vertices, u32 vertexCount, s32 *fraction,
                                     s32 *height, s32 *minHeight, Vec3s *normal)
{
    struct {
        s32 zMax, zMinTemp, zMin;
        s32 vMaxTemp, vMax, vMinTemp, vMin;
        s32 xMaxTemp, xMax, xMinTemp, xMin, heightTemp;
        Vec3i *scratch;
        u32 count;
        s32 fractionZ;
        u32 i;
        s32 fractionV, fractionX;
        Vec3i *cursor;
        Box6i destination;
        Vec3i corner;
    } w;
    struct {
        s32 zMaxTemp;
    } top;
    w.scratch = g_clipTriBuffer;
    w.destination = *box;
    w.destination.min[0] += delta->x;
    w.destination.min[1] += delta->y;
    w.destination.min[2] += delta->z;
    w.destination.max[0] += delta->x;
    w.destination.max[1] += delta->y;
    w.destination.max[2] += delta->z;
    *height = -0x100000;
    *minHeight = 0x100000;
    *fraction = 4096;
    w.count = Poly_ClipMinX(w.destination.min[0], vertices, vertexCount, w.scratch);
    w.count = Poly_ClipMaxX(w.destination.max[0], w.scratch, w.count, vertices);
    w.count = Poly_ClipMinZ(w.destination.min[2], vertices, w.count, w.scratch);
    w.count = Poly_ClipMaxZ(w.destination.max[2], w.scratch, w.count, vertices);
    w.count = Poly_ClipMinY(w.destination.min[1], vertices, w.count, w.scratch);
    w.count = Poly_ClipMaxY(w.destination.max[1], w.scratch, w.count, vertices);
    if (!w.count)
        return 0;
    w.cursor = vertices;
    for (w.i = 0; w.i < w.count; ++w.i, ++w.cursor) {
        w.heightTemp = w.cursor->y;
        if (*height < w.heightTemp)
            *height = w.heightTemp;
        if (*minHeight > w.heightTemp)
            *minHeight = w.heightTemp;
    }
    if (normal->x > 0)
        w.corner.x = box->min[0];
    else
        w.corner.x = box->max[0];
    if (normal->y > 0)
        w.corner.y = -box->max[1];
    else
        w.corner.y = -box->min[1];
    if (normal->z > 0)
        w.corner.z = -box->max[2];
    else
        w.corner.z = -box->min[2];
    w.corner.x = vertices->x - w.corner.x;
    w.corner.y = -vertices->y - w.corner.y;
    w.corner.z = -vertices->z - w.corner.z;
    w.fractionX = w.corner.x * normal->x + w.corner.y * normal->y + w.corner.z * normal->z;
    if (w.fractionX < -65536) {
        w.fractionV = (normal->x * delta->x - normal->y * delta->y - normal->z * delta->z) >> 4;
        if (w.fractionV) {
            w.fractionX = (w.fractionX << 8) / w.fractionV;
            if (w.fractionX < 0)
                w.fractionX = 0;
            else if (w.fractionX > 4096)
                w.fractionX = 4096;
            *fraction = w.fractionX;
        } else
            *fraction = 0;
    } else {
        TRI_AXIS_FRACTION(x, 0, fractionX, xMin, xMinTemp, xMax, w.xMaxTemp)
        TRI_AXIS_FRACTION(y, 1, fractionV, vMin, vMinTemp, vMax, w.vMaxTemp)
        TRI_AXIS_FRACTION(z, 2, fractionZ, zMin, zMinTemp, zMax, top.zMaxTemp)
        if (w.fractionX > w.fractionV)
            w.fractionX = w.fractionV;
        if (w.fractionX > w.fractionZ)
            w.fractionX = w.fractionZ;
        *fraction = w.fractionX;
    }
    return w.count;
}
#undef TRI_AXIS_FRACTION

} /* extern "C" */
