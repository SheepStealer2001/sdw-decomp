/*
 * T254 - guessed original name: Collide.cpp. SheepD3D.exe .text 0x5196c0-0x51e1eb, .data 0x57b998-0x57ba04 (the three
 * French error strings of Coll_BuildCellGrid, $SG), .bss 0x6d5140-0x6d520c.
 * The static-collision core: level init / shutdown and the static cell grid, the triangle-list merge, box vs object
 * box, the swept-box detector Collide_SweepBox and the placement ground query, the triangle tests and ray casts, the
 * segment cell walk and segment tests, Sam's own swept-box detector, and the XZ segment-vs-box-list slab test.
 * The box / triangle clipping these tests call is src/engine/coll_clip.cpp (T253).
 *
 * Linkage: the sweep functions (Collide_MergeTriLists .. Coll_BoxGroundQuery, Collide_PointInPolyXZ, Collide_GroundYRay)
 * are extern "C", the others C++. The `w` / `top` structs only pin the original stack offsets of locals (VC6 /Od
 * orders locals by a hash of their names, src/README.md); they are not a claim about the source text, and neither are
 * the inline helpers (CollisionOverlapFour/Two, FlagsClear, TriEdgeCross, CollCellLookup, RayPlaneDistance,
 * SegReachesPlane, SegInsideTri), which have no bodies of their own in the exe.
 *
 * .bss (0x6d5140-0x6d520c): the collision state keeps its C names (extern "C") and is written `= 0`: VC6 emits
 * explicitly zero-initialised globals in DEFINITION order (uninitialised ones would be ordered by a hash of their
 * names, which is not the address order here). 0x6d5140-0x6d51cc (140 bytes) and 0x6d5208-0x6d520c (4 bytes) are
 * referred to by no instruction anywhere; they are kept as opaque bytes in place, and the 140-byte one (>= 64 bytes)
 * is what makes this object's .bss 8-aligned.
 * Declarations: ObjGrid_CellFromXZ, g_objGridCells and g_objGridDimX are declared with C++ linkage here, as
 * src/engine/obj_grid.cpp (T252) defines them, and so is g_sharedScratch, as src/engine/draw2d.cpp (T256) defines it and
 * most of its users declare it.
 * g_collSegTriVerts (0x6d5856) is written as the address it is, g_clipTriBuffer + 0x1ee: it is the tail of that 512-byte
 * scratch, not a global of its own (see the comment at its #define).
 *
 * Coordinates: game space is (x, y pointing DOWN, z); CollTri.verts is stored in "detector space"
 * (x, -y, -z) /16. Fractions are 4.12 (0x1000 = the whole move).
 */
/* BYTES: dead-code, flow, inline, layout, slot-group, slot-name, view. */
/* BYTES(layout): written '= 0' only to keep definition order in .bss */
/* BYTES(layout): placeholder: unreferenced bytes kept only for the .bss layout (the 140-byte block also makes it 8-aligned) */
/* BYTES(view): g_collSegTriVerts macro: spelled as the address it is (the tail of g_clipTriBuffer): a separate global would be 4-aligned */

#include "sdw_classes.h"
#include "sdw_enums.h"
#include "scenaric_props.h"

#include "../sdk/crt.h"
void Debug_Printf(const char *fmt, ...); /* 0x5363a5, an empty stub in the retail build */
#include "id_list.h"
#include "obj_grid.h"
#include "../app/app_main.h"
#include "maths.h"
#include "scn_tools.h"
#include "coll_clip.h"
#include "fixed_math.h"

/* src/engine/obj_grid.cpp (T252) */

/* 0x6d5468, src/engine/draw2d.cpp (T256): shared scratch. C++ linkage, as its definition and most users declare it. */
extern u8 g_sharedScratch[];

/* C++ linkage, as their definitions (src/app/app_main.cpp, src/engine/maths.cpp) and every other user declare them. */

/* C++ linkage: this is the provider in src/engine/scn_tools.cpp (T250) 0x514823, which is NOT extern "C". */

extern "C" {

/* src/engine/fixed_math.cpp */
s16 Math_RadiansToAngle4096(float radians);

/* src/engine/coll_clip.cpp (T253) */
u32 Collide_ClipTriToBox(Box6i *box, Vec3i *vertices, u32 vertexCount, s32 *height);

/* defined below, called before their definitions */
s32 Collide_MergeTriLists(CollTri **out, CollTri **left, s32 leftCount, CollTri **right, s32 rightCount);
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
s32 Collide_BoxVsObjBox(ScnObject *owner, CollBox *moverBox, Vec3s *delta, CollBox *otherBox, Vec3s *otherPos,
                        s32 *fraction, s32 *height, CollContact *contacts, s32 *count);
s32 Collide_PointInPolyXZ(Vec3s *point, Vec3s *vertices, u8 count);

extern const s16 g_sinTable4096[]; /* 0x57ece0  4.12 sine, 4096 steps per turn */
extern const s16 *g_pCosTable;     /* 0x5814e4  = g_sinTable4096 + 1024 */
extern Vec3i g_clipTriBuffer[];    /* 0x6d5668  ping-pong vertex buffer of the Poly_Clip* chain */
/* 0x6d5856: the game-space vertices of the triangle under test are NOT a global of their own: they are the last 18
 * bytes of the 512-byte g_clipTriBuffer (0x6d5668 + 0x1ee, just past the 123 merge entries a scratch buffer may hold).
 * A separate 18-byte global would be 4-aligned by VC6 (tested); 0x6d5856 is only 2-aligned. The symbol
 * g_collSegTriVerts is therefore written as that address. */
/* cast kept: the vertices are the tail of the Vec3i buffer, at a byte offset that is not a whole Vec3i */
#define g_collSegTriVerts ((Vec3s *)((u8 *)g_clipTriBuffer + 0x1ee))

/* ---- .bss 0x6d5140-0x6d520c, in address order ---- */
/* 0x6d5140-0x6d51cc: 140 bytes no instruction refers to (tools/tu_sheet.py). Genuinely opaque: nothing reads or writes
 * them, so neither their type nor their name can be recovered; kept as bytes so that the .bss has its original
 * layout (and its 8-byte alignment). */
static u8 s_unref_6d5140[140] = {0};
CollTri **g_collMergeDst = 0;      /* 0x6d51cc  segment-gather merge output */
s32 g_collGridOriginZ = 0;         /* 0x6d51d0 */
s32 g_collGridOriginX = 0;         /* 0x6d51d4 */
u32 g_collMergeCount = 0;          /* 0x6d51d8  entries in the accumulated list */
s32 g_collMergeOverflowed = 0;     /* 0x6d51dc  output moved to the caller's buffer */
CollBox **g_collNoStaticZones = 0; /* 0x6d51e0  id-list 7: inside one of these the static world is not tested */
u16 g_collNoStaticZoneCount = 0;   /* 0x6d51e4 */
CollBox **g_collStaticBoxes = 0;   /* 0x6d51e8  id-list 6: world-space static boxes */
u16 g_collStaticBoxCount = 0;      /* 0x6d51ec */
s32 g_collCellSizeX = 0;           /* 0x6d51f0 */
s32 g_collCellSizeZ = 0;           /* 0x6d51f4 */
CollTri **g_collMergeResult = 0;   /* 0x6d51f8  the buffer holding the latest union */
CollTri **g_collMergeSrc = 0;      /* 0x6d51fc  the accumulated list (merge input) */
u16 g_collGridNX = 0;              /* 0x6d5200 */
u16 g_collGridNZ = 0;              /* 0x6d5202 */
CollCell ***g_collGrid = 0;        /* 0x6d5204  [cellX][cellZ] -> {u32 count; CollTri *tris[count]} */
/* 0x6d5208-0x6d520c: 4 bytes no instruction refers to (see above). */
static u8 s_unref_6d5208[4] = {0};

} /* extern "C" */

/* Inline helpers (no bodies in the exe; /Ob1 expands them). CollisionOverlapFour/Two: "all of these differences are
 * >= 0" computed as one OR of the sign bits, the shape the original uses for every AABB test here (or/or/or, not,
 * and 0x80000000). FlagsClear: `!` materialised as neg/sbb/inc before the test (0x51a84f), as in scn_controllable.cpp. */
/* BYTES(inline): source-only inline: one OR of the sign bits (or / or / or, not, and 0x80000000) */
#define SDW_INLINE_FREE_COLLISIONOVERLAPFOUR_S32_S32_S32_S32 1
#define SDW_INLINE_FREE_COLLISIONOVERLAPTWO_S32_S32 1
#include "coll_box_inlines.h"
#undef SDW_INLINE_FREE_COLLISIONOVERLAPFOUR_S32_S32_S32_S32
#undef SDW_INLINE_FREE_COLLISIONOVERLAPTWO_S32_S32

/* BYTES(inline): source-only inline: ! materialised as neg / sbb / inc before the test (0x51a84f) */
#define SDW_INLINE_SCNOBJECT_FLAGSCLEAR_U32 1
#include "scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_FLAGSCLEAR_U32

void Coll_BuildCellGrid();
void Coll_FreeCellGrid();

/* 0x5196c0: level load: the static cell grid, then the id-list 7 zones (no static collision inside) and the id-list 6
 * static boxes. */
void Collide_InitLevel()
{
    Coll_BuildCellGrid();
    /* cast kept: an export id list holds record pointers of any kind; these two lists hold boxes */
    g_collNoStaticZones = (CollBox **)Scn_FindIdList(WAR_IDO_CAMNOCOLLBOX, &g_collNoStaticZoneCount);
    g_collStaticBoxes = (CollBox **)Scn_FindIdList(WAR_IDO_CAMCOLLBOX, &g_collStaticBoxCount); /* cast kept: as above */
}

/* 0x5196f2 */
void Collide_ShutdownLevel()
{
    Coll_FreeCellGrid();
}

/* 0x5196fc: read the collision map header (nx, nz, originX, originZ, cell sizes), point g_collGrid[x][z] at each
 * cell record, and rewrite every cell's triangle INDICES into CollTri pointers. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
void Coll_BuildCellGrid()
{
    struct {
        u32 index;
        u32 triCount;
        CollTri **table;
        u32 k;
        s32 z, x;
        CollCell **block;
        u32 *cell; /* a CollCell: count, then the triangle indices / pointers */
        CollTri *tri;
    } w;
    if (g_pWarCollMap == 0) {
        Debug_Printf("erreur : carte introuvable\n");
        exit(0);
    }
    if (g_pWarCollTris == 0) {
        Debug_Printf("erreur : polygones de collision introuvables\n");
        exit(0);
    }
    if (g_collGrid != 0) {
        Debug_Printf("erreur : il y a d\xe9j\xe0 une map\n");
        exit(0);
    }
    g_collGridNX = g_pWarCollMap[0];
    g_collGridNZ = g_pWarCollMap[1];
    g_collGridOriginX = (s16)g_pWarCollMap[2];
    g_collGridOriginZ = (s16)g_pWarCollMap[3];
    g_collCellSizeX = (s16)g_pWarCollMap[4];
    g_collCellSizeZ = (s16)g_pWarCollMap[5];
    g_pWarCollMap += 6;
    g_collGrid = (CollCell ***)malloc(g_collGridNX * 4);            /* cast kept: malloc returns void * */
    w.block = (CollCell **)malloc(g_collGridNZ * 4 * g_collGridNX); /* cast kept: malloc returns void * */
    for (w.x = 0; w.x < g_collGridNX; ++w.x) {
        g_collGrid[w.x] = w.block;
        w.block += g_collGridNZ;
    }
    w.triCount = 0;
    w.cell = (u32 *)g_pWarCollMap; /* cast kept: the cell records are read as words (the header before them as u16) */
    for (w.z = 0; w.z < g_collGridNZ; ++w.z) {
        for (w.x = 0; w.x < g_collGridNX; ++w.x) {
            for (w.k = 0; w.k < w.cell[0]; ++w.k) {
                if (w.cell[w.k + 1] + 1 > w.triCount)
                    w.triCount = w.cell[w.k + 1] + 1;
            }
            w.cell += w.cell[0] + 1;
        }
    }
    w.table = (CollTri **)malloc(w.triCount * 4); /* cast kept: malloc returns void * */
    w.tri = g_pWarCollTris;
    for (w.k = 0; w.k < w.triCount; ++w.k) {
        w.table[w.k] = w.tri;
        ++w.tri;
    }
    w.cell = (u32 *)g_pWarCollMap; /* cast kept: as above */
    for (w.z = 0; w.z < g_collGridNZ; ++w.z) {
        for (w.x = 0; w.x < g_collGridNX; ++w.x) {
            g_collGrid[w.x][w.z] = (CollCell *)w.cell; /* cast kept: w.cell walks the cell records word by word */
            for (w.k = 0; w.k < w.cell[0]; ++w.k) {
                w.index = w.cell[w.k + 1];
                /* cast kept: each triangle index word is rewritten in place as the triangle's address */
                w.cell[w.k + 1] = (u32)w.table[w.cell[w.k + 1]];
            }
            w.cell += w.cell[0] + 1;
        }
    }
    if (w.table != 0) {
        free(w.table);
        w.table = 0;
    }
}

/* 0x5199ff */
void Coll_FreeCellGrid()
{
    if (g_collGrid != 0) {
        if (g_collGrid[0] != 0) {
            free(g_collGrid[0]);
            g_collGrid[0] = 0;
        }
        if (g_collGrid != 0) {
            free(g_collGrid);
            g_collGrid = 0;
        }
    }
}

extern "C" {

/* 0x519a55: merge two pointer-sorted triangle lists into a sorted union, a triangle present in both emitted once
 * (the static cells overlap). Returns the merged count; no capacity check (the callers' buffers hold 512). */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
s32 Collide_MergeTriLists(CollTri **out, CollTri **left, s32 leftCount, CollTri **right, s32 rightCount)
{
    struct {
        s32 remainingRight;
        CollTri **rightCursor;
        s32 remainingLeft;
        CollTri **leftCursor, **output;
        s32 count;
    } w;
    w.remainingRight = rightCount;
    w.rightCursor = right;
    w.remainingLeft = leftCount;
    w.leftCursor = left;
    w.output = out;
    w.count = 0;
    while (w.remainingLeft) {
        if (w.remainingRight) {
            while (w.remainingLeft && *w.leftCursor < *w.rightCursor) {
                *w.output = *w.leftCursor;
                ++w.output;
                ++w.leftCursor;
                --w.remainingLeft;
                ++w.count;
            }
            if (w.remainingLeft && *w.leftCursor == *w.rightCursor) {
                *w.output = *w.rightCursor;
                ++w.output;
                ++w.rightCursor;
                --w.remainingRight;
                ++w.leftCursor;
                --w.remainingLeft;
                ++w.count;
            }
            while (w.remainingRight && w.remainingLeft && *w.leftCursor > *w.rightCursor) {
                *w.output = *w.rightCursor;
                ++w.output;
                ++w.rightCursor;
                ++w.count;
                --w.remainingRight;
            }
        } else {
            while (w.remainingLeft) {
                *w.output = *w.leftCursor;
                ++w.output;
                ++w.leftCursor;
                ++w.count;
                --w.remainingLeft;
            }
        }
    }
    while (w.remainingRight) {
        *w.output = *w.rightCursor;
        ++w.output;
        ++w.rightCursor;
        ++w.count;
        --w.remainingRight;
    }
    return w.count;
}

/* The contact written for the face Collide_SweepBoxVsBox reports: an axis normal and a corner of the target. */
#define BOX_CONTACT(nx, nv, nz, px, pv, pz) \
    w.contact->normal.x = nx;               \
    w.contact->normal.y = nv;               \
    w.contact->normal.z = nz;               \
    w.contact->point.x = w.target.px.x;     \
    w.contact->point.y = w.target.pv.y;     \
    w.contact->point.z = w.target.pz.z;

/* 0x519bea: sweep the mover's box against one box of an object (otherBox is object-local, placed at otherPos).
 * Nothing when 16 contacts exist or the box is COLLBOX_NONSOLID; a shaped mover box (COLLBOX_SHAPED_MASK) honours
 * cone / ellipsoid / cylinder targets; otherwise the plain box sweep, which records one contact (normal and corner
 * of the face hit) and reports a height 2 units higher. Returns 1 on contact. */
s32 Collide_BoxVsObjBox(ScnObject *owner, CollBox *moverBox, Vec3s *delta, CollBox *otherBox, Vec3s *otherPos,
                        s32 *fraction, s32 *height, CollContact *contacts, s32 *count)
{
    struct {
        u32 mask;
        CollContact *contact;
        CollBox target;
    } w;
    if (*count >= 16)
        return 0;
    if (otherBox->flags & COLLBOX_NONSOLID)
        return 0;
    w.target.flags = otherBox->flags;
    w.target.min.x = otherBox->min.x + otherPos->x;
    w.target.min.y = otherBox->min.y + otherPos->y;
    w.target.min.z = otherBox->min.z + otherPos->z;
    w.target.max.x = otherBox->max.x + otherPos->x;
    w.target.max.y = otherBox->max.y + otherPos->y;
    w.target.max.z = otherBox->max.z + otherPos->z;
    if (moverBox->flags & COLLBOX_SHAPED_MASK) {
        if (otherBox->flags & COLLBOX_CONE)
            return Collide_SweepBoxVsCone(owner, moverBox, delta, &w.target, fraction, height, contacts, count);
        if (otherBox->flags & COLLBOX_ELLIPSOID)
            return Collide_SweepBoxVsEllipsoid(owner, moverBox, delta, &w.target, fraction, height, contacts, count);
        if (otherBox->flags & COLLBOX_CYLINDER)
            return Collide_SweepBoxVsCylinder(owner, moverBox, delta, &w.target, fraction, height, contacts, count);
    }
    w.mask = Collide_SweepBoxVsBox(moverBox, delta, &w.target, fraction, height);
    if (w.mask) {
        *height -= 2;
        w.contact = &contacts[*count];
        ++*count;
        w.contact->obj = owner;
        if (w.mask & CBF_HIT_MINZ) {
            BOX_CONTACT(0, 0, -4096, min, min, min)
        } else if (w.mask & CBF_HIT_MAXZ) {
            BOX_CONTACT(0, 0, 4096, min, min, max)
        } else if (w.mask & CBF_HIT_MINX) {
            BOX_CONTACT(-4096, 0, 0, min, min, min)
        } else if (w.mask & CBF_HIT_MAXX) {
            BOX_CONTACT(4096, 0, 0, max, min, min)
        } else if (w.mask & CBF_HIT_MAXY) {
            BOX_CONTACT(0, 4096, 0, min, max, min)
        } else if (w.mask & CBF_HIT_MINY) {
            BOX_CONTACT(0, -4096, 0, min, min, min)
        }
        return 1;
    }
    return 0;
}
#undef BOX_CONTACT

/* 0x519f6d: the swept-box detector behind Collide_ResolveMove. Sweeps `box` by `delta` and returns the number of
 * contacts written to contacts[16]; *fraction = the earliest contact (4.12; 0x2000 = none), *height = the highest
 * surface met in game space (a static triangle's top - 1, or an object's result), *auxHeight = the static part alone.
 * flags (CollQueryFlags): CQ_STATIC_EXT first tests the no-static zones (when the END box's centre is inside one,
 * neither the static boxes nor the triangles are tested) and then the static boxes; CQ_STATIC or CQ_STATIC_EXT the
 * static triangles of every grid cell the END box touches (merged into one sorted list; only those whose normal does
 * not face along the move); CQ_OBJECTS the objects of the object-grid cells within 500 units, less excludeSelf, the
 * excluded list and (CQ_ASK_MOVER) those the mover vetoes: their boxes unless SCN_OF_NO_BOX_COLLIDE (overridden by
 * CQ_IGNORE_SKIPBOX), and their CustomCollide when SCN_OF_CUSTOM_COLLIDE. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; unused1086, unused02 are fillers */
s32 Collide_SweepBox(ScnObject *owner, CollBox *box, Vec3s *delta, s32 *fraction, s32 *height, CollContact *contacts,
                     ScnObject *excludeSelf, u8 flags, s32 *auxHeight, ScnObject **excludedObjects, s32 excludedCount)
{
    struct {
        u16 unused10D0, objectFlags;
        ModelBoxList *boxList;
        u32 *cellWords;
        s32 excluded;
        ListNode **list;
        ListNode *node;
        ListNode ***cell;
        s16 cellsX, cellsZ, cellX, cellZ;
        ListNode ***row;
        CollBox *otherBox;
        ScnObject *object;
        s32 boxCount;
        Vec3s *vertex;
        CollTri *triangle;
        CollTri **swap;
        s32 maxZ;
        Vec3s scaledDelta;
        s16 unused1086;
        s32 exclusionIndex, maxCellZ, maxV, triangleHeight;
        Box6i scaledBox;
        s32 maxCellX, maxX, triangleFraction;
        CollTri *bufferB[512], *bufferA[512];
        CollTri **candidates;
        CollContact *contact;
        s32 z;
        Vec3i *vertices;
        s32 objectFraction, i, candidateCount;
        CollTri **otherCandidates;
        s32 skipTriangles, cellCount;
        CollTri **cellTriangles;
        s32 minCellZ, minZ, minCellX, minV, objectHeight, minX, count;
        Vec3s center;
        s16 unused02;
    } w;
    w.vertices = (Vec3i *)g_sharedScratch; /* cast kept: g_sharedScratch is raw bytes, laid out by each user */
    w.candidates = w.bufferA;
    w.otherCandidates = w.bufferB;
    w.candidateCount = 0;
    /* the move and the start box in detector space */
    w.scaledDelta.x = delta->x << 4;
    w.scaledDelta.y = -delta->y << 4;
    w.scaledDelta.z = -delta->z << 4;
    w.scaledBox.min[0] = box->min.x << 4;
    w.scaledBox.max[0] = box->max.x << 4;
    w.scaledBox.min[1] = -box->max.y << 4;
    w.scaledBox.max[1] = -box->min.y << 4;
    w.scaledBox.min[2] = -box->max.z << 4;
    w.scaledBox.max[2] = -box->min.z << 4;
    /* the END box in game space */
    w.minX = box->min.x + delta->x;
    w.maxX = box->max.x + delta->x;
    w.minV = box->min.y + delta->y;
    w.maxV = box->max.y + delta->y;
    w.minZ = box->min.z + delta->z;
    w.maxZ = box->max.z + delta->z;
    *height = -0x40000000;
    *fraction = 8192;
    w.count = 0;
    w.skipTriangles = 0;
    if (flags & CQ_STATIC_EXT) {
        w.center.x = ((box->min.x + box->max.x) >> 1) + delta->x;
        w.center.y = ((box->min.y + box->max.y) >> 1) + delta->y;
        w.center.z = ((box->min.z + box->max.z) >> 1) + delta->z;
        /* cast kept: Box is the plain C view of the same 16-byte record as CollBox */
        if (BoxList_FindContainingPoint(&w.center, (Box **)g_collNoStaticZones, g_collNoStaticZoneCount))
            w.skipTriangles = 1;
        else {
            for (w.i = 0; w.i < g_collStaticBoxCount; ++w.i) {
                if (Collide_BoxVsObjBox(0, box, delta, g_collStaticBoxes[w.i], g_pZeroVec3s, &w.objectFraction,
                                        &w.objectHeight, contacts, &w.count)) {
                    /* kept in detector space like the triangle heights; the conversion below turns it back */
                    *height = -w.objectHeight << 4;
                    *fraction = w.objectFraction;
                }
            }
        }
    }
    if (!w.skipTriangles && (flags & (CQ_STATIC | CQ_STATIC_EXT))) {
        w.minCellX = (w.minX - g_collGridOriginX) / g_collCellSizeX;
        w.maxCellX = (w.maxX - g_collGridOriginX) / g_collCellSizeX;
        w.minCellZ = (w.minZ - g_collGridOriginZ) / g_collCellSizeZ;
        w.maxCellZ = (w.maxZ - g_collGridOriginZ) / g_collCellSizeZ;
        for (w.i = w.minCellX; w.i <= w.maxCellX; ++w.i) {
            for (w.z = w.minCellZ; w.z <= w.maxCellZ; ++w.z) {
                if (w.i < 0 || w.z < 0 || w.i >= g_collGridNX || w.z >= g_collGridNZ) {
                    w.cellCount = 0;
                    w.cellTriangles = 0;
                } else {
                    /* cast kept: a CollCell is walked as words: its count, then the triangle pointers */
                    w.cellWords = (u32 *)g_collGrid[w.i][w.z];
                    w.cellCount = *w.cellWords;
                    ++w.cellWords;
                    w.cellTriangles = (CollTri **)w.cellWords; /* cast kept: as above */
                }
                w.candidateCount = Collide_MergeTriLists(w.otherCandidates, w.candidates, w.candidateCount,
                                                         w.cellTriangles, w.cellCount);
                w.swap = w.candidates;
                w.candidates = w.otherCandidates;
                w.otherCandidates = w.swap;
            }
        }
        for (w.i = w.candidateCount; w.i; --w.i, ++w.candidates) {
            w.triangle = *w.candidates;
            if (CollisionOverlapFour(w.maxX - w.triangle->bbMin.x, w.triangle->bbMax.x - w.minX,
                                     w.maxZ - w.triangle->bbMin.z, w.triangle->bbMax.z - w.minZ) &&
                CollisionOverlapTwo(w.maxV - w.triangle->bbMin.y, w.triangle->bbMax.y - w.minV)) {
                if (Vec3s_Dot(&w.triangle->normal, delta) <= 0) {
                    w.vertex = w.triangle->verts;
                    w.vertices[0].x = w.vertex->x << 4;
                    w.vertices[0].y = w.vertex->y << 4;
                    w.vertices[0].z = w.vertex->z << 4;
                    ++w.vertex;
                    w.vertices[1].x = w.vertex->x << 4;
                    w.vertices[1].y = w.vertex->y << 4;
                    w.vertices[1].z = w.vertex->z << 4;
                    ++w.vertex;
                    w.vertices[2].x = w.vertex->x << 4;
                    w.vertices[2].y = w.vertex->y << 4;
                    w.vertices[2].z = w.vertex->z << 4;
                    if (Collide_BoxTriangleSweep(&w.scaledBox, &w.scaledDelta, w.vertices, 3, &w.triangleFraction,
                                                 &w.triangleHeight, &w.triangle->normal)) {
                        if (w.triangleFraction < *fraction)
                            *fraction = w.triangleFraction;
                        if (w.count < 16) {
                            w.vertex = w.triangle->verts;
                            w.contact = &contacts[w.count];
                            w.contact->obj = 0;
                            w.contact->normal.x = w.triangle->normal.x;
                            w.contact->normal.y = w.triangle->normal.y;
                            w.contact->normal.z = w.triangle->normal.z;
                            w.contact->point.x = w.vertex->x;
                            w.contact->point.y = -w.vertex->y;
                            w.contact->point.z = -w.vertex->z;
                            w.contact->triVerts = &w.vertex->x;
                            ++w.count;
                        }
                        if (w.triangleHeight > *height)
                            *height = w.triangleHeight;
                    }
                }
            }
        }
    }
    *height = (-*height >> 4) - 1;
    *auxHeight = *height;
    if (flags & CQ_OBJECTS) {
        ObjGrid_CellFromXZ(box->min.x - 500, box->min.z - 500, &w.cellX, &w.cellZ);
        ObjGrid_CellFromXZ(box->max.x + 500, box->max.z + 500, &w.cellsX, &w.cellsZ);
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
                    w.object = (ScnObject *)w.node->data; /* cast kept: a list node's payload is untyped */
                    w.excluded = 0;
                    if (excludedObjects) {
                        for (w.exclusionIndex = 0; w.exclusionIndex < excludedCount; ++w.exclusionIndex) {
                            if (w.object == excludedObjects[w.exclusionIndex]) {
                                w.excluded = 1;
                                break;
                            }
                        }
                    }
                    if (!w.excluded && (flags & CQ_ASK_MOVER))
                        w.excluded = owner->HandleMessage(0, MSG_COLLIDE_IGNORE_QUERY, w.object);
                    if (w.object != excludeSelf && !w.excluded) {
                        w.boxList = w.object->inst_model->boxes;
                        if (!w.boxList) {
                            w.boxCount = 0;
                            w.otherBox = 0;
                        } else {
                            w.boxCount = w.boxList->count;
                            w.otherBox = w.boxList->boxes;
                        }
                        if (w.object->FlagsClear(SCN_OF_NO_BOX_COLLIDE) || (flags & CQ_IGNORE_SKIPBOX)) {
                            for (; w.boxCount; --w.boxCount, ++w.otherBox) {
                                if (Collide_BoxVsObjBox(w.object, box, delta, w.otherBox, &w.object->pos,
                                                        &w.objectFraction, &w.objectHeight, contacts, &w.count)) {
                                    if (w.objectHeight < *height)
                                        *height = w.objectHeight;
                                    if (w.objectFraction < *fraction)
                                        *fraction = w.objectFraction;
                                }
                            }
                        }
                        w.objectFlags = w.object->flags;
                        if (w.objectFlags & SCN_OF_CUSTOM_COLLIDE) {
                            if (w.object->CustomCollide(owner, box, delta, &w.objectFraction, &w.objectHeight, contacts,
                                                        &w.count, 0)) {
                                if (w.objectHeight < *height)
                                    *height = w.objectHeight;
                                if (w.objectFraction < *fraction)
                                    *fraction = w.objectFraction;
                            }
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
    }
    return w.count;
}

/* 0x51a9de: the placement / ground query: which surfaces does a box (no displacement) overlap, and where is the
 * highest of them? Returns the number of hits; *height = the highest surface's top - 1 in game space (the ground
 * queries stretch the box's bottom to 32000 to find the ground below). mode & (CQ_STATIC|CQ_STATIC_EXT): static
 * triangles clipped to the box (Collide_ClipTriToBox); mode & CQ_OBJECTS: the solid boxes of the objects within
 * 500 units (skipping `self` and SCN_OF_NO_BOX_COLLIDE ones) and their custom colliders, which get a copy of the box
 * raised by 2 and swept 2 down, with mode | CQ_FROM_GROUND_QUERY. *outObject (optional) = the object that gave the
 * final height. */
/* BYTES(slot-group): locals grouped in w / top only to pin the original frame offsets; unused11FA, unused10A0, unused1076, unused106E, unused1066, unused30 are fillers */
s32 Coll_BoxGroundQuery(CollBox *box, s32 *height, ScnObject *self, u8 mode, ScnObject **outObject)
{
    struct {
        ModelBoxList *boxList;
        u32 *cellWords;
        Vec3s delta;
        s16 unused11FA;
        CollBox shiftedBox;
        s32 fraction, callbackCount;
        CollContact contacts[16];
        s32 unused10A0;
        ListNode **list;
        ListNode *node;
        ListNode ***cell;
        s16 cellsX, cellsZ, cellX, cellZ;
        ListNode ***row;
        CollBox *otherBox;
        ScnObject *object;
        Vec3s position;
        s16 unused1076;
        Vec3s targetMin;
        s16 unused106E;
        Vec3s targetMax;
        s16 unused1066;
        s32 boxCount;
        Vec3s *vertex;
        CollTri *triangle;
        CollTri **swap;
        s32 maxCellZ, candidateHeight;
        Box6i scaledBox;
        s32 maxCellX;
        CollTri *bufferB[512], *bufferA[512];
        s32 unused30;
        CollTri **candidates;
        s32 z;
        Vec3i *vertices;
        s32 i, candidateCount;
        CollTri **otherCandidates;
        s32 cellCount;
        CollTri **cellTriangles;
        s32 minCellZ, minCellX, hits;
    } w;
    struct {
        u16 unused120C, objectFlags;
    } top;
    w.vertices = (Vec3i *)g_sharedScratch; /* cast kept: g_sharedScratch is raw bytes, laid out by each user */
    w.candidates = w.bufferA;
    w.otherCandidates = w.bufferB;
    w.candidateCount = 0;
    *height = -0x40000000;
    w.hits = 0;
    if (outObject)
        *outObject = 0;
    if (mode & (CQ_STATIC | CQ_STATIC_EXT)) {
        w.scaledBox.min[0] = box->min.x << 4;
        w.scaledBox.max[0] = box->max.x << 4;
        w.scaledBox.min[1] = -box->max.y << 4;
        w.scaledBox.max[1] = -box->min.y << 4;
        w.scaledBox.min[2] = -box->max.z << 4;
        w.scaledBox.max[2] = -box->min.z << 4;
        w.minCellX = (box->min.x - g_collGridOriginX) / g_collCellSizeX;
        w.maxCellX = (box->max.x - g_collGridOriginX) / g_collCellSizeX;
        w.minCellZ = (box->min.z - g_collGridOriginZ) / g_collCellSizeZ;
        w.maxCellZ = (box->max.z - g_collGridOriginZ) / g_collCellSizeZ;
        for (w.i = w.minCellX; w.i <= w.maxCellX; ++w.i) {
            for (w.z = w.minCellZ; w.z <= w.maxCellZ; ++w.z) {
                if (w.i < 0 || w.z < 0 || w.i >= g_collGridNX || w.z >= g_collGridNZ) {
                    w.cellCount = 0;
                    w.cellTriangles = 0;
                } else {
                    /* cast kept: a CollCell is walked as words: its count, then the triangle pointers */
                    w.cellWords = (u32 *)g_collGrid[w.i][w.z];
                    w.cellCount = *w.cellWords;
                    ++w.cellWords;
                    w.cellTriangles = (CollTri **)w.cellWords; /* cast kept: as above */
                }
                w.candidateCount = Collide_MergeTriLists(w.otherCandidates, w.candidates, w.candidateCount,
                                                         w.cellTriangles, w.cellCount);
                w.swap = w.candidates;
                w.candidates = w.otherCandidates;
                w.otherCandidates = w.swap;
            }
        }
        for (w.i = w.candidateCount; w.i; --w.i, ++w.candidates) {
            w.triangle = *w.candidates;
            if (CollisionOverlapFour(box->max.x - w.triangle->bbMin.x, w.triangle->bbMax.x - box->min.x,
                                     box->max.z - w.triangle->bbMin.z, w.triangle->bbMax.z - box->min.z) &&
                CollisionOverlapTwo(box->max.y - w.triangle->bbMin.y, w.triangle->bbMax.y - box->min.y)) {
                w.vertex = w.triangle->verts;
                w.vertices[0].x = w.vertex->x << 4;
                w.vertices[0].y = w.vertex->y << 4;
                w.vertices[0].z = w.vertex->z << 4;
                ++w.vertex;
                w.vertices[1].x = w.vertex->x << 4;
                w.vertices[1].y = w.vertex->y << 4;
                w.vertices[1].z = w.vertex->z << 4;
                ++w.vertex;
                w.vertices[2].x = w.vertex->x << 4;
                w.vertices[2].y = w.vertex->y << 4;
                w.vertices[2].z = w.vertex->z << 4;
                if (Collide_ClipTriToBox(&w.scaledBox, w.vertices, 3, &w.candidateHeight)) {
                    ++w.hits;
                    if (w.candidateHeight > *height)
                        *height = w.candidateHeight;
                }
            }
        }
    }
    *height = (-*height >> 4) - 1;
    if (mode & CQ_OBJECTS) {
        ObjGrid_CellFromXZ(box->min.x - 500, box->min.z - 500, &w.cellX, &w.cellZ);
        ObjGrid_CellFromXZ(box->max.x + 500, box->max.z + 500, &w.cellsX, &w.cellsZ);
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
                    w.object = (ScnObject *)w.node->data; /* cast kept: a list node's payload is untyped */
                    if (w.object != self) {
                        if (w.object->FlagsClear(SCN_OF_NO_BOX_COLLIDE)) {
                            w.boxList = w.object->inst_model->boxes;
                            if (!w.boxList) {
                                w.boxCount = 0;
                                w.otherBox = 0;
                            } else {
                                w.boxCount = w.boxList->count;
                                w.otherBox = w.boxList->boxes;
                            }
                            w.position = w.object->pos;
                            while (w.boxCount) {
                                if (!(w.otherBox->flags & COLLBOX_NONSOLID)) {
                                    w.targetMin.x = w.otherBox->min.x + w.position.x;
                                    w.targetMin.y = w.otherBox->min.y + w.position.y;
                                    w.targetMin.z = w.otherBox->min.z + w.position.z;
                                    w.targetMax.x = w.otherBox->max.x + w.position.x;
                                    w.targetMax.y = w.otherBox->max.y + w.position.y;
                                    w.targetMax.z = w.otherBox->max.z + w.position.z;
                                    if (CollisionOverlapFour(w.targetMax.x - box->min.x, box->max.x - w.targetMin.x,
                                                             w.targetMax.z - box->min.z, box->max.z - w.targetMin.z) &&
                                        CollisionOverlapTwo(w.targetMax.y - box->min.y, box->max.y - w.targetMin.y)) {
                                        ++w.hits;
                                        if (w.targetMin.y < w.targetMax.y)
                                            w.candidateHeight = w.targetMin.y - 1;
                                        else
                                            w.candidateHeight = box->min.y;
                                        if (w.candidateHeight < *height) {
                                            *height = w.candidateHeight;
                                            if (outObject)
                                                *outObject = w.object;
                                        }
                                    }
                                }
                                --w.boxCount;
                                ++w.otherBox;
                            }
                        }
                        top.objectFlags = w.object->flags;
                        if (top.objectFlags & SCN_OF_CUSTOM_COLLIDE) {
                            w.delta.x = w.delta.z = 0;
                            w.delta.y = 2;
                            w.shiftedBox = *box;
                            w.shiftedBox.min.y -= 2;
                            w.shiftedBox.max.y -= 2;
                            w.callbackCount = 0;
                            if (w.object->CustomCollide(self, &w.shiftedBox, &w.delta, &w.fraction, &w.candidateHeight,
                                                        w.contacts, &w.callbackCount, mode | CQ_FROM_GROUND_QUERY)) {
                                ++w.hits;
                                if (w.candidateHeight < *height) {
                                    *height = w.candidateHeight;
                                    if (outObject)
                                        *outObject = w.object;
                                }
                            }
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
    }
    return w.hits;
}

} /* extern "C" */

/* 0x51b301: the unnormalised normal (v1 - v0) x (v2 - v0) of a game-space triangle, in the order e2 x e1. No callers. */
/* BYTES(slot-group): e1 / e2 are Vec4i (PSX VECTOR, pad unused) for their 16-byte slots (-0x10, -0x20) */
void Collide_TriNormalRaw(Vec3s *verts, Vec3i *normal)
{
    Vec4i e1, e2; /* 16-byte slots (-0x10, -0x20): Vec4i, the pad word unused */
    e1.x = verts[1].x - verts[0].x;
    e1.y = verts[1].y - verts[0].y;
    e1.z = verts[1].z - verts[0].z;
    e2.x = verts[2].x - verts[0].x;
    e2.y = verts[2].y - verts[0].y;
    e2.z = verts[2].z - verts[0].z;
    normal->x = e2.y * e1.z - e2.z * e1.y;
    normal->y = e2.z * e1.x - e2.x * e1.z;
    normal->z = e2.x * e1.y - e2.y * e1.x;
}

/* An inline helper, expanded in place by /Ob1 in Collide_RayInsideTri and three times in Collide_SegmentVsStaticTris
 * (the name and the parameter order are not the original's; it has no body in the exe): *out = (cur - o) x (prev - o). The frame
 * shows it: its two difference vectors, then its pointer parameters in their own slots, after the caller's locals. */
/* BYTES(inline): source-only inline: its two difference vectors, then its pointer parameters in their own slots after the caller's locals */
inline void TriEdgeCross(Vec3s *o, Vec3s *prev, Vec3s *cur, Vec4i *out)
{
    Vec4i e1, e0; /* 16-byte slots: the pad word is never used */
    e0.x = prev->x - o->x;
    e0.y = prev->y - o->y;
    e0.z = prev->z - o->z;
    e1.x = cur->x - o->x;
    e1.y = cur->y - o->y;
    e1.z = cur->z - o->z;
    out->x = e1.y * e0.z - e1.z * e0.y;
    out->y = e1.z * e0.x - e1.x * e0.z;
    out->z = e1.x * e0.y - e1.y * e0.x;
}

/* 0x51b3bc: is the ray's line inside the game-space triangle? For every edge (prev, cur) the scalar triple product
 * dir . ((cur - origin) x (prev - origin)), the cross product /16 first; 0 as soon as one is negative, else 1.
 * One-sided: a triangle wound the other way never passes. */
/* BYTES(slot-group): cross is a Vec4i (PSX VECTOR, pad unused) for its 16-byte slot */
s32 Collide_RayInsideTri(CollRay *ray, Vec3s *verts)
{
    Vec4i cross; /* 16-byte slot */
    s32 j, i;
    j = 2;
    for (i = 0; i < 3; ++i) {
        TriEdgeCross(&ray->origin, &verts[j], &verts[i], &cross);
        cross.x /= 16;
        cross.y /= 16;
        cross.z /= 16;
        if (cross.x * ray->dir.x + cross.y * ray->dir.y + cross.z * ray->dir.z < 0)
            return 0;
        j = i;
    }
    return 1;
}

extern "C" {

/* 0x51b525: is point inside the convex polygon in the x/z plane? Edge-function test against every edge including
 * the closing one; 0 on the first negative edge, else 1 (on an edge counts as inside). */
s32 Collide_PointInPolyXZ(Vec3s *point, Vec3s *vertices, u8 count)
{
    Vec3s *cursor = vertices;
    while (count > 1) {
        if (-(point->x - cursor->x) * (cursor[1].z - cursor->z) + (point->z - cursor->z) * (cursor[1].x - cursor->x) <
            0)
            return 0;
        --count;
        ++cursor;
    }
    if (-(point->x - cursor->x) * (vertices->z - cursor->z) + (point->z - cursor->z) * (vertices->x - cursor->x) < 0)
        return 0;
    return 1;
}

/* 0x51b5f3: the static ground below point: a vertical ray through the ONE static cell containing it, over the
 * upward-facing triangles (normal.y < 0) whose box spans point in x/z and reaches minV; the height comes from
 * the plane equation. Returns the highest such surface at or below minV (32000 if none) and its normal in *normal
 * (0, -0x1000, 0 if none). */
/* BYTES(slot-group): locals grouped in w / top only to pin the original frame offsets */
/* BYTES(flow, inferred): w.count is zeroed twice because the original stores it twice */
s16 Collide_GroundYRay(Vec3s *point, Vec3s *normal, s16 minV)
{
    struct {
        Vec3s *vertex;
        CollTri *triangle;
        s32 x, cellZ, cellX, height;
        u32 count;
        Vec3s *scratch;
        s16 unused10, best;
        CollTri **list;
        s32 z;
        u32 i;
    } w;
    struct {
        u32 *cellWords;
    } top;
    w.cellX = (point->x - g_collGridOriginX) / g_collCellSizeX;
    w.cellZ = (point->z - g_collGridOriginZ) / g_collCellSizeZ;
    w.x = point->x;
    w.z = point->z;
    w.best = 32000;
    normal->x = 0;
    normal->z = 0;
    normal->y = -4096;
    w.count = 0;
    if (w.cellX < 0 || w.cellZ < 0 || w.cellX >= g_collGridNX || w.cellZ >= g_collGridNZ) {
        w.count = 0;
        w.list = 0;
    } else {
        /* cast kept: a CollCell is walked as words: its count, then the triangle pointers */
        top.cellWords = (u32 *)g_collGrid[w.cellX][w.cellZ];
        w.count = *top.cellWords;
        ++top.cellWords;
        w.list = (CollTri **)top.cellWords; /* cast kept: as above */
    }
    w.scratch = (Vec3s *)g_sharedScratch; /* cast kept: g_sharedScratch is raw bytes, laid out by each user */
    for (w.i = 0; w.i < w.count; ++w.i, ++w.list) {
        w.triangle = *w.list;
        w.vertex = w.triangle->verts;
        if (w.triangle->normal.y < 0 &&
            CollisionOverlapFour(w.triangle->bbMax.x - w.x, w.x - w.triangle->bbMin.x, w.triangle->bbMax.z - w.z,
                                 w.z - w.triangle->bbMin.z) &&
            CollisionOverlapTwo(w.best - w.triangle->bbMin.y, w.triangle->bbMax.y - minV)) {
            /* the vertices back in game space */
            w.scratch[0].x = w.vertex[0].x;
            w.scratch[0].y = -w.vertex[0].y;
            w.scratch[0].z = -w.vertex[0].z;
            w.scratch[1].x = w.vertex[1].x;
            w.scratch[1].y = -w.vertex[1].y;
            w.scratch[1].z = -w.vertex[1].z;
            w.scratch[2].x = w.vertex[2].x;
            w.scratch[2].y = -w.vertex[2].y;
            w.scratch[2].z = -w.vertex[2].z;
            if (Collide_PointInPolyXZ(point, w.scratch, 3)) {
                w.height = w.scratch[0].y + (w.triangle->normal.x * (w.scratch[0].x - point->x) +
                                             w.triangle->normal.z * (w.scratch[0].z - point->z)) /
                                                w.triangle->normal.y;
                if (w.height < w.best && w.height >= minV) {
                    *normal = w.triangle->normal;
                    w.best = (s16)w.height;
                }
            }
        }
    }
    return w.best;
}

} /* extern "C" */

/* Inline helpers of Collide_RayCastStatic (descriptive names, no bodies in the exe; each expansion has its own slots).
 * CollCellLookup: the triangle list of static cell (x, z), empty outside the grid. */
/* BYTES(inline): source-only inline: each expansion has its own slots */
inline void CollCellLookup(s32 x, s32 z, u32 *count, CollTri ***list)
{
    u32 *cellWords;
    if (x < 0 || z < 0 || x >= g_collGridNX || z >= g_collGridNZ) {
        *count = 0;
        *list = 0;
    } else {
        /* cast kept: a CollCell is walked as words: its count, then the triangle pointers */
        cellWords = (u32 *)g_collGrid[x][z];
        *count = *cellWords;
        ++cellWords;
        *list = (CollTri **)cellWords; /* cast kept: as above */
    }
}

/* The distance along the ray to the plane of a triangle facing it (normal . dir < 0), 0x7fffffff otherwise. */
/* BYTES(inline): source-only inline: each expansion has its own slots */
inline s32 RayPlaneDistance(Vec3s *normal, CollRay *ray, Vec3s *v0)
{
    Vec3s offset;
    s32 t, denom;
    t = 0x7fffffff;
    denom = (normal->x * ray->dir.x + normal->y * ray->dir.y + normal->z * ray->dir.z) >> 12;
    if (denom < 0) {
        offset.x = v0->x - ray->origin.x;
        offset.y = v0->y - ray->origin.y;
        offset.z = v0->z - ray->origin.z;
        t = (normal->x * offset.x + normal->y * offset.y + normal->z * offset.z) / denom;
    }
    return t;
}

/* Union cell (x, z) into the accumulated list: merge into dst, then swap the two buffers. */
#define RAY_ADD_CELL(x, z)                                                           \
    CollCellLookup(x, z, &w.cellCount, &w.cellList);                                 \
    w.count = Collide_MergeTriLists(w.dst, w.src, w.count, w.cellList, w.cellCount); \
    w.swap = w.src;                                                                  \
    w.src = w.dst;                                                                   \
    w.dst = w.swap

/* 0x51b8a2: cast the ray against the static triangles. Walks the cells under origin -> end in x/z along the major
 * axis (both neighbours when the minor cell changes), unions their triangle lists, then for every triangle whose
 * vertical span meets the ray's and which faces it, the plane distance; the nearest one within maxDist whose
 * triangle contains the line wins. Returns that distance (0x7fffffff if none) and, when outHit is given, the hit
 * triangle's normal and game-space vertices. */
/* BYTES(slot-group): locals grouped in w only to pin the original slots (-0x1058..-4); cellCount / cellList / swap are unused members that fill slots */
s32 Collide_RayCastStatic(CollRay *ray, CollRayHit *outHit)
{
    /* w pins the stack slots (-0x1058..-4, a multiple of 8 as VC6 wants for large locals); endCellZ is -0x105c. */
    struct {
        s32 prev, dx, maxV, endCellX;
        CollTri *bufB[512];
        CollTri *bufA[512];
        CollTri **src;
        s32 k, dist, cellZ, cellX;
        u32 count;
        CollTri **dst;
        u32 cellCount;
        CollTri **cellList;
        Vec3s *vertex;
        CollTri **swap;
        s32 step, minV;
        Vec3s *verts;
        CollTri *triangle;
        s32 best, dz;
        u32 i;
    } w;
    s32 endCellZ;
    w.src = w.bufA;
    w.dst = w.bufB;
    w.count = 0;
    w.cellX = (ray->origin.x - g_collGridOriginX) / g_collCellSizeX;
    w.cellZ = (ray->origin.z - g_collGridOriginZ) / g_collCellSizeZ;
    w.dx = ray->end.x - ray->origin.x;
    w.dz = ray->end.z - ray->origin.z;
    if ((w.dx >= 0 ? w.dx : -w.dx) >= (w.dz >= 0 ? w.dz : -w.dz)) {
        if (w.dx == 0) {
            RAY_ADD_CELL(w.cellX, w.cellZ);
        } else {
            w.endCellX = (ray->end.x - g_collGridOriginX) / g_collCellSizeX;
            if (w.endCellX > w.cellX)
                w.step = 1;
            else if (w.endCellX < w.cellX)
                w.step = -1;
            else
                w.step = 0;
            do {
                RAY_ADD_CELL(w.cellX, w.cellZ);
                w.cellX += w.step;
                w.prev = w.cellZ;
                w.cellZ = w.cellX * g_collCellSizeX + g_collGridOriginX; /* the column's edge in x */
                w.cellZ = ray->origin.z + w.dz * (w.cellZ - ray->origin.x) / w.dx;
                w.cellZ = (w.cellZ - g_collGridOriginZ) / g_collCellSizeZ;
                if (w.cellZ != w.prev) {
                    RAY_ADD_CELL(w.cellX - w.step, w.cellZ);
                    RAY_ADD_CELL(w.cellX, w.prev);
                }
            } while (w.cellX != w.endCellX + w.step);
        }
    } else {
        endCellZ = (ray->end.z - g_collGridOriginZ) / g_collCellSizeZ;
        if (endCellZ > w.cellZ)
            w.step = 1;
        else if (endCellZ < w.cellZ)
            w.step = -1;
        else
            w.step = 0;
        do {
            RAY_ADD_CELL(w.cellX, w.cellZ);
            w.cellZ += w.step;
            w.prev = w.cellX;
            w.cellX = w.cellZ * g_collCellSizeZ + g_collGridOriginZ; /* the row's edge in z */
            w.cellX = ray->origin.x + w.dx * (w.cellX - ray->origin.z) / w.dz;
            w.cellX = (w.cellX - g_collGridOriginX) / g_collCellSizeX;
            if (w.cellX != w.prev) {
                RAY_ADD_CELL(w.cellX, w.cellZ - w.step);
                RAY_ADD_CELL(w.prev, w.cellZ);
            }
        } while (w.cellZ != endCellZ + w.step);
    }
    w.verts = (Vec3s *)g_sharedScratch; /* cast kept: g_sharedScratch is raw bytes, laid out by each user */
    if (ray->origin.y >= ray->end.y) {
        w.minV = ray->end.y;
        w.maxV = ray->origin.y;
    } else {
        w.minV = ray->origin.y;
        w.maxV = ray->end.y;
    }
    w.best = ray->maxDist;
    for (w.i = 0; w.i < w.count; ++w.i, ++w.src) {
        w.triangle = *w.src;
        if (CollisionOverlapTwo(w.maxV - w.triangle->bbMin.y, w.triangle->bbMax.y - w.minV)) {
            /* the vertices back in game space */
            w.vertex = w.triangle->verts;
            w.verts[0].x = w.vertex[0].x;
            w.verts[0].y = -w.vertex[0].y;
            w.verts[0].z = -w.vertex[0].z;
            w.dist = RayPlaneDistance(&w.triangle->normal, ray, w.verts);
            if (w.dist >= 0 && w.dist < w.best) {
                w.verts[1].x = w.vertex[1].x;
                w.verts[1].y = -w.vertex[1].y;
                w.verts[1].z = -w.vertex[1].z;
                w.verts[2].x = w.vertex[2].x;
                w.verts[2].y = -w.vertex[2].y;
                w.verts[2].z = -w.vertex[2].z;
                if (Collide_RayInsideTri(ray, w.verts)) {
                    w.best = w.dist;
                    if (outHit) {
                        outHit->normal = w.triangle->normal;
                        for (w.k = 0; w.k < 3; ++w.k)
                            outHit->verts[w.k] = w.verts[w.k];
                    }
                }
            }
        }
    }
    if (w.best >= ray->maxDist)
        w.best = 0x7fffffff;
    return w.best;
}

/* 0x51c297: union the triangles of static cell (cellX, cellZ) into the segment's accumulated list, ping-ponging
 * between g_collMergeSrc and g_collMergeDst (first the two 512-byte shared scratch buffers). Once the union could
 * reach 123 entries (0x7b; the scratch buffers hold 128) the output moves to the caller's bufB for good, and the
 * pair becomes bufB / bufA. Returns the buffer the union was written to (g_collMergeResult). */
/* BYTES(slot-group): Collide_MergeTriLists 0x519a55 written out in place: its locals in a nested block (struct m) come after the function's own (-0x30..-0x1c) */
CollTri **Collide_SegCells_AddCell(s32 cellX, s32 cellZ, CollTri **bufA, CollTri **bufB)
{
    struct {
        u32 *cellWords;
        CollTri **swap;
        s32 switched;
        u32 count;
        CollTri **list;
        u32 total;
    } w;
    w.switched = 0;
    w.total = g_collMergeCount;
    if (cellX < 0 || cellZ < 0 || cellX >= g_collGridNX || cellZ >= g_collGridNZ) {
        w.count = 0;
        w.list = 0;
    } else {
        /* cast kept: a CollCell is walked as words: its count, then the triangle pointers */
        w.cellWords = (u32 *)g_collGrid[cellX][cellZ];
        w.count = *w.cellWords;
        ++w.cellWords;
        w.list = (CollTri **)w.cellWords; /* cast kept: as above */
    }
    if (w.count + w.total >= 0x7b && !g_collMergeOverflowed) {
        g_collMergeDst = bufB;
        g_collMergeOverflowed = 1;
        w.switched = 1;
    }
    g_collMergeResult = g_collMergeDst;
    { /* Collide_MergeTriLists (0x519a55) written out in place: the same body, its locals in a nested block after
         * the function's own (-0x30..-0x1c). */
        struct {
            s32 remainingRight;
            CollTri **rightCursor;
            s32 remainingLeft;
            CollTri **leftCursor, **output;
            s32 count;
        } m;
        m.remainingRight = w.count;
        m.rightCursor = w.list;
        m.remainingLeft = g_collMergeCount;
        m.leftCursor = g_collMergeSrc;
        m.output = g_collMergeDst;
        m.count = 0;
        while (m.remainingLeft) {
            if (m.remainingRight) {
                while (m.remainingLeft && *m.leftCursor < *m.rightCursor) {
                    *m.output = *m.leftCursor;
                    ++m.output;
                    ++m.leftCursor;
                    --m.remainingLeft;
                    ++m.count;
                }
                if (m.remainingLeft && *m.leftCursor == *m.rightCursor) {
                    *m.output = *m.rightCursor;
                    ++m.output;
                    ++m.rightCursor;
                    --m.remainingRight;
                    ++m.leftCursor;
                    --m.remainingLeft;
                    ++m.count;
                }
                while (m.remainingRight && m.remainingLeft && *m.leftCursor > *m.rightCursor) {
                    *m.output = *m.rightCursor;
                    ++m.output;
                    ++m.rightCursor;
                    ++m.count;
                    --m.remainingRight;
                }
            } else {
                while (m.remainingLeft) {
                    *m.output = *m.leftCursor;
                    ++m.output;
                    ++m.leftCursor;
                    ++m.count;
                    --m.remainingLeft;
                }
            }
        }
        while (m.remainingRight) {
            *m.output = *m.rightCursor;
            ++m.output;
            ++m.rightCursor;
            ++m.count;
            --m.remainingRight;
        }
        g_collMergeCount = m.count;
    }
    if (!w.switched) {
        w.swap = g_collMergeDst;
        g_collMergeDst = g_collMergeSrc;
        g_collMergeSrc = w.swap;
    } else {
        g_collMergeSrc = bufB;
        g_collMergeDst = bufA;
    }
    return g_collMergeResult;
}

/* 0x51c51e: reset the merge state and add every static cell the x/z segment (x0, z0) + (dx, dz) crosses, column by
 * column: the z range of each column comes from the line equation at the column's edges (floor division throughout);
 * a segment with dx == 0 is one column. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
void Collide_SegCells_Gather(s32 x0, s32 z0, s32 dx, s32 dz, CollTri **bufA, CollTri **bufB)
{
    struct {
        s32 zHi, remaining, x0Snap, stepX, zLo, zCell, stepCell, z1Snap, colEdge, z0Snap, cellX, lineC, x1Snap;
    } w;
    x0 -= g_collGridOriginX;
    z0 -= g_collGridOriginZ;
    g_collMergeCount = 0;
    g_collMergeSrc = (CollTri **)g_sharedScratch; /* cast kept: g_sharedScratch is raw bytes, laid out by each user */
    g_collMergeDst = (CollTri **)g_clipTriBuffer; /* cast kept: the clipper's vertex buffer, reused as a merge buffer */
    g_collMergeOverflowed = 0;
    w.lineC = -dz * x0 + dx * z0;
    if (x0 >= 0)
        w.x0Snap = x0 / g_collCellSizeX * g_collCellSizeX;
    else
        w.x0Snap = (x0 - (g_collCellSizeX - 1)) / g_collCellSizeX * g_collCellSizeX;
    if (z0 >= 0)
        w.z0Snap = z0 / g_collCellSizeZ * g_collCellSizeZ;
    else
        w.z0Snap = (z0 - (g_collCellSizeZ - 1)) / g_collCellSizeZ * g_collCellSizeZ;
    if (x0 + dx >= 0)
        w.x1Snap = (x0 + dx) / g_collCellSizeX * g_collCellSizeX;
    else
        w.x1Snap = (x0 + dx - (g_collCellSizeX - 1)) / g_collCellSizeX * g_collCellSizeX;
    if (z0 + dz >= 0)
        w.z1Snap = (z0 + dz) / g_collCellSizeZ * g_collCellSizeZ;
    else
        w.z1Snap = (z0 + dz - (g_collCellSizeZ - 1)) / g_collCellSizeZ * g_collCellSizeZ;
    w.zLo = w.zHi = w.z0Snap / g_collCellSizeZ;
    if (dx > 0) {
        w.stepX = g_collCellSizeX;
        w.stepCell = 1;
        w.cellX = w.x0Snap;
        if (w.cellX < 0)
            w.cellX -= g_collCellSizeX - 1;
        w.cellX /= g_collCellSizeX;
        w.colEdge = w.x0Snap + w.stepX;
        w.remaining = x0 + dx - w.colEdge;
    } else if (dx != 0) {
        w.stepX = -g_collCellSizeX;
        w.stepCell = -1;
        w.colEdge = w.x0Snap;
        w.cellX = w.x0Snap;
        if (w.cellX < 0)
            w.cellX -= g_collCellSizeX - 1;
        w.cellX /= g_collCellSizeX;
        w.remaining = w.colEdge - (x0 + dx);
    } else {
        w.cellX = w.x0Snap;
        if (w.cellX < 0)
            w.cellX -= g_collCellSizeX - 1;
        w.cellX /= g_collCellSizeX;
        if (dz < 0)
            w.zLo = w.z1Snap / g_collCellSizeZ;
        else
            w.zHi = w.z1Snap / g_collCellSizeZ;
        for (w.zCell = w.zLo; w.zCell <= w.zHi; ++w.zCell)
            Collide_SegCells_AddCell(w.cellX, w.zCell, bufA, bufB);
        return;
    }
    while (w.remaining >= 0) {
        if (dz < 0) {
            w.zHi = w.zLo;
            w.zLo = (dz * w.colEdge + w.lineC) / dx;
            if (w.zLo < 0)
                w.zLo -= g_collCellSizeZ - 1;
            w.zLo /= g_collCellSizeZ;
        } else {
            w.zLo = w.zHi;
            w.zHi = (dz * w.colEdge + w.lineC) / dx;
            if (w.zHi < 0)
                w.zHi -= g_collCellSizeZ - 1;
            w.zHi /= g_collCellSizeZ;
        }
        for (w.zCell = w.zLo; w.zCell <= w.zHi; ++w.zCell)
            Collide_SegCells_AddCell(w.cellX, w.zCell, bufA, bufB);
        w.cellX += w.stepCell;
        w.colEdge += w.stepX;
        w.remaining -= g_collCellSizeX;
    }
    w.remaining += g_collCellSizeX;
    if (w.remaining != 0) {
        if (dz < 0) {
            w.zHi = w.zLo;
            w.zLo = (dz * (x0 + dx) + w.lineC) / dx;
            if (w.zLo < 0)
                w.zLo -= g_collCellSizeZ - 1;
            w.zLo /= g_collCellSizeZ;
        } else {
            w.zLo = w.zHi;
            w.zHi = (dz * (x0 + dx) + w.lineC) / dx;
            if (w.zHi < 0)
                w.zHi -= g_collCellSizeZ - 1;
            w.zHi /= g_collCellSizeZ;
        }
        w.cellX = w.x1Snap;
        if (w.cellX < 0)
            w.cellX -= g_collCellSizeX - 1;
        w.cellX /= g_collCellSizeX;
        for (w.zCell = w.zLo; w.zCell <= w.zHi; ++w.zCell)
            Collide_SegCells_AddCell(w.cellX, w.zCell, bufA, bufB);
    }
}

/* Does the segment origin -> origin + delta reach the plane of the triangle (normal, through v0)? With
 * d1 = normal . delta and vDot = normal . (v0 - origin): 0 <= vDot <= d1 (or the mirror for d1 < 0); a segment
 * parallel to the plane passes only if it lies in it. */
/* BYTES(inline): source-only inline: its expansion gives the original's slots */
inline s32 SegReachesPlane(Vec3s *origin, Vec3s *normal, Vec3s *delta, Vec3s *v0)
{
    s32 vDot;
    Vec3s offset;
    s32 d1;
    d1 = normal->x * delta->x + normal->y * delta->y + normal->z * delta->z;
    offset.x = v0->x - origin->x;
    offset.y = v0->y - origin->y;
    offset.z = v0->z - origin->z;
    vDot = normal->x * offset.x + normal->y * offset.y + normal->z * offset.z;
    if (d1 > 0)
        return vDot >= 0 && vDot <= d1;
    else if (d1 == 0)
        return vDot == 0;
    else
        return vDot <= 0 && vDot >= d1;
}

/* Is the ray's line inside the triangle, either winding? The three edge triple products dir . ((cur - o) x (prev - o))
 * must share the sign of the first; a zero one counts as inside AT ONCE, the remaining edges unchecked. */
/* BYTES(inline): source-only inline: its expansion gives the original's slots */
/* BYTES(slot-group): cross is a Vec4i (PSX VECTOR, pad unused) for its 16-byte slot */
inline s32 SegInsideTri(CollRay *ray, Vec3s *v)
{
    s32 d0, dot;
    Vec4i cross; /* 16-byte slot */
    TriEdgeCross(&ray->origin, &v[2], v, &cross);
    d0 = cross.x * ray->dir.x + cross.y * ray->dir.y + cross.z * ray->dir.z;
    if (d0 == 0)
        return 1;
    TriEdgeCross(&ray->origin, v, &v[1], &cross);
    dot = cross.x * ray->dir.x + cross.y * ray->dir.y + cross.z * ray->dir.z;
    if (dot == 0)
        return 1;
    if ((dot ^ d0) < 0)
        return 0;
    TriEdgeCross(&ray->origin, &v[1], &v[2], &cross);
    dot = cross.x * ray->dir.x + cross.y * ray->dir.y + cross.z * ray->dir.z;
    if (dot == 0)
        return 1;
    return (dot ^ d0) >= 0;
}

/* 0x51c99e: is the segment ray->origin -> ray->end clear of the static triangles? Gathers the cells under it, culls
 * each triangle by its box against the segment's box, then needs the plane reached within the segment and the line
 * inside the triangle: 0 on the first such triangle, else 0x7fffffff. The copy of the hit triangle into hitVerts
 * follows the `break` and is unreachable (the original has it too: its loop body without its initialisation). */
/* BYTES(slot-name): names place the locals: idx -4, result -8, verts -0xc, segMinX -0x10 (same bucket as verts, declared first), w -0x1040, vertex -0x1044; verts stays a plain local for SegInsideTri's expansions */
/* BYTES(slot-group): locals grouped in w (size a multiple of 8) to pin the original frame; unused30 / unused1a fill gaps */
/* BYTES(flow): result = result emits nothing but advances /Od's register rotation, as the original's dead loop shows (it starts at edx) */
/* BYTES(dead-code): unreachable copy after the break, as in the original */
s32 Collide_SegmentVsStaticTris(CollRay *ray, Vec3s *hitVerts)
{
    /* The names place the locals: idx -4, result -8, verts -0xc, segMinX -0x10 (same bucket as verts, declared
     * first), w -0x1040 (its size a multiple of 8, or VC6 pads the slot), vertex -0x1044. verts must be a plain local: SegInsideTri's inline expansions use it in place (a member of w
     * would be copied into a temporary first). */
    Vec3s *vertex;
    struct {
        CollTri *triangle;
        s32 maxZ, maxV, maxX;
        CollTri *bufB[512];
        CollTri *bufA[512];
        s32 unused30;
        CollTri **list;
        s32 k;
        u32 count;
        Vec3s delta;
        s16 unused1a;
        s32 minZ, minV;
    } w;
    s32 segMinX;
    Vec3s *verts;
    s32 result;
    u32 idx;
    w.delta.x = ray->end.x - ray->origin.x;
    w.delta.y = ray->end.y - ray->origin.y;
    w.delta.z = ray->end.z - ray->origin.z;
    if (ray->origin.y >= ray->end.y) {
        w.minV = ray->end.y;
        w.maxV = ray->origin.y;
    } else {
        w.minV = ray->origin.y;
        w.maxV = ray->end.y;
    }
    if (ray->origin.x >= ray->end.x) {
        segMinX = ray->end.x;
        w.maxX = ray->origin.x;
    } else {
        segMinX = ray->origin.x;
        w.maxX = ray->end.x;
    }
    if (ray->origin.z >= ray->end.z) {
        w.minZ = ray->end.z;
        w.maxZ = ray->origin.z;
    } else {
        w.minZ = ray->origin.z;
        w.maxZ = ray->end.z;
    }
    result = 0x7fffffff;
    Collide_SegCells_Gather(ray->origin.x, ray->origin.z, ray->end.x - ray->origin.x, ray->end.z - ray->origin.z,
                            w.bufA, w.bufB);
    w.count = g_collMergeCount;
    w.list = g_collMergeResult;
    verts = g_collSegTriVerts;
    for (idx = 0; idx < w.count; ++idx, ++w.list) {
        w.triangle = *w.list;
        if (CollisionOverlapTwo(w.maxV - w.triangle->bbMin.y, w.triangle->bbMax.y - w.minV) &&
            CollisionOverlapFour(w.maxX - w.triangle->bbMin.x, w.triangle->bbMax.x - segMinX,
                                 w.maxZ - w.triangle->bbMin.z, w.triangle->bbMax.z - w.minZ)) {
            /* the vertices back in game space */
            vertex = w.triangle->verts;
            verts[0].x = vertex[0].x;
            verts[0].y = -vertex[0].y;
            verts[0].z = -vertex[0].z;
            if (SegReachesPlane(&ray->origin, &w.triangle->normal, &w.delta, verts)) {
                verts[1].x = vertex[1].x;
                verts[1].y = -vertex[1].y;
                verts[1].z = -vertex[1].z;
                verts[2].x = vertex[2].x;
                verts[2].y = -vertex[2].y;
                verts[2].z = -vertex[2].z;
                if (SegInsideTri(ray, verts)) {
                    result = 0;
                    break;
                    /* unreachable: the statement below emits nothing but takes one register of /Od's eax/ecx/edx
                     * rotation, which the original's dead loop shows (it starts at edx; src/README.md) */
                    result = result;
                    for (w.k = 0; w.k < 3; ++w.k)
                        hitVerts[w.k] = verts[w.k];
                }
            }
        }
    }
    return result;
}

/* 0x51d2eb: is the segment from -> to (s32 game-space points) clear of the static triangles? Builds a CollRay (length
 * through sqrt, direction normalised to 4.12 then >>2, the points truncated to s16): 0x7fffffff when clear, 0 when
 * blocked. hitVerts (void * as the callers declare it) is passed through as the Vec3s[3] of the hit triangle; every
 * caller passes 0. */
/* BYTES(slot-name): names place the locals: sq -0x10, ray -0x28, dir -0x38 (Vec4i for their 16-byte slots) */
/* BYTES(view, inferred): dir is a Vec4i (for its slot) passed as the Vec3i it holds */
s32 Collide_SegmentClearStatic(s32 *from, s32 *to, void *hitVerts)
{
    /* The names place the locals: sq -0x10, ray -0x28, dir -0x38 (Vec4i: 16-byte slots, the pad word unused). */
    Vec4i dir;
    CollRay ray;
    Vec4i sq;
    ray.origin.x = (s16)from[0];
    ray.origin.y = (s16)from[1];
    ray.origin.z = (s16)from[2];
    ray.end.x = (s16)to[0];
    ray.end.y = (s16)to[1];
    ray.end.z = (s16)to[2];
    dir.x = to[0] - from[0];
    dir.y = to[1] - from[1];
    dir.z = to[2] - from[2];
    sq.x = dir.x * dir.x;
    sq.y = dir.y * dir.y;
    sq.z = dir.z * dir.z;
    ray.maxDist = (s32)sqrt((double)sq.x + sq.y + sq.z);
    Vec3i_Normalize((Vec3i *)&dir, (Vec3i *)&dir); /* cast kept: dir is a Vec4i only for its 16-byte stack slot */
    dir.x >>= 2;
    dir.y >>= 2;
    dir.z >>= 2;
    ray.dir.x = (s16)dir.x;
    ray.dir.y = (s16)dir.y;
    ray.dir.z = (s16)dir.z;
    return Collide_SegmentVsStaticTris(&ray, (Vec3s *)hitVerts); /* cast kept: the callers declare hitVerts void * */
}

/* 0x51d3f0 */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; unused109E, unused68 are fillers */
/* BYTES(flow): the original tests with test/jl */
u32 Collide_SweepBox_Sam(ScnObject *mover, CollBox *box, Vec3s *disp, s32 *outFrac, s32 *outMaxY, s32 *outMinY,
                         CollContact *contacts, ScnObject *exclude, u8 queryFlags, s32 *outStaticY)
{
    struct {
        u16 unused10E8, objectFlags;
        ModelBoxList *boxList;
        s32 *cellWords;
        s32 excluded;
        ListNode **list;
        ListNode *node;
        ListNode ***cell;
        s16 cellsX, cellsZ, cellX, cellZ;
        ListNode ***row;
        CollBox *otherBox;
        ScnObject *object;
        s32 boxCount;
        Vec3s *vertex;
        CollTri *triangle;
        CollTri **swap;
        s32 maxZ;
        Vec3s scaledDelta;
        s16 unused109E;
        s32 specialWall, maxCellZ, maxV, triangleHeight;
        Box6i scaledBox;
        s32 maxCellX, maxX, triangleFraction;
        CollTri *bufferB[512], *bufferA[512];
        s32 unused68;
        CollTri **candidates;
        CollContact *contact;
        s32 z;
        Vec3i *vertices;
        s32 objectFraction, i, candidateCount;
        CollTri **otherCandidates;
        s32 skipTriangles, cellCount;
        CollTri **cellTriangles;
        s32 minCellZ, minZ, triangleMinHeight, minCellX, minV;
        Box6i wallBox;
        s32 objectHeight, minX, count;
    } w;

    w.vertices = (Vec3i *)g_sharedScratch; /* cast kept: g_sharedScratch is raw bytes, laid out by each user */
    w.candidates = w.bufferA;
    w.otherCandidates = w.bufferB;
    w.candidateCount = 0;
    w.specialWall = 0;
    w.scaledDelta.x = disp->x << 4;
    w.scaledDelta.y = -disp->y << 4;
    w.scaledDelta.z = -disp->z << 4;
    w.scaledBox.min[0] = box->min.x << 4;
    w.scaledBox.max[0] = box->max.x << 4;
    w.scaledBox.min[1] = -box->max.y << 4;
    w.scaledBox.max[1] = -box->min.y << 4;
    w.scaledBox.min[2] = -box->max.z << 4;
    w.scaledBox.max[2] = -box->min.z << 4;
    w.wallBox.min[0] = w.scaledBox.min[0];
    w.wallBox.min[1] = w.scaledBox.min[1] - 0x640;
    w.wallBox.min[2] = w.scaledBox.min[2];
    w.wallBox.max[0] = w.scaledBox.max[0];
    w.wallBox.max[1] = w.scaledBox.max[1];
    w.wallBox.max[2] = w.scaledBox.max[2];
    w.minX = box->min.x + disp->x;
    w.maxX = box->max.x + disp->x;
    w.minV = box->min.y + disp->y;
    w.maxV = box->max.y + disp->y;
    w.minZ = box->min.z + disp->z;
    w.maxZ = box->max.z + disp->z;
    *outMaxY = -0x40000000;
    *outMinY = 0x40000000;
    *outFrac = 8192;
    w.count = 0;
    w.skipTriangles = 0;
    if (!w.skipTriangles && (queryFlags & (CQ_STATIC | CQ_STATIC_EXT))) {
        w.minCellX = (w.minX - g_collGridOriginX) / g_collCellSizeX;
        w.maxCellX = (w.maxX - g_collGridOriginX) / g_collCellSizeX;
        w.minCellZ = (w.minZ - g_collGridOriginZ) / g_collCellSizeZ;
        w.maxCellZ = (w.maxZ - g_collGridOriginZ) / g_collCellSizeZ;
        for (w.i = w.minCellX; w.i <= w.maxCellX; ++w.i) {
            for (w.z = w.minCellZ; w.z <= w.maxCellZ; ++w.z) {
                if (w.i < 0 || w.z < 0 || w.i >= g_collGridNX || w.z >= g_collGridNZ) {
                    w.cellCount = 0;
                    w.cellTriangles = 0;
                } else {
                    /* cast kept: a CollCell is walked as words: its count, then the triangle pointers */
                    w.cellWords = (s32 *)g_collGrid[w.i][w.z];
                    w.cellCount = *w.cellWords;
                    ++w.cellWords;
                    w.cellTriangles = (CollTri **)w.cellWords; /* cast kept: as above */
                }
                w.candidateCount = Collide_MergeTriLists(w.otherCandidates, w.candidates, w.candidateCount,
                                                         w.cellTriangles, w.cellCount);
                w.swap = w.candidates;
                w.candidates = w.otherCandidates;
                w.otherCandidates = w.swap;
            }
        }
        for (w.i = w.candidateCount; w.i; --w.i, ++w.candidates) {
            w.triangle = *w.candidates;
            if (CollisionOverlapFour(w.maxX - w.triangle->bbMin.x, w.triangle->bbMax.x - w.minX,
                                     w.maxZ - w.triangle->bbMin.z, w.triangle->bbMax.z - w.minZ)) {
                /* |normal.y|, written so the negation is the TAKEN branch, as the original has it (test/jl) */
                if ((w.triangle->normal.y >= 0 ? w.triangle->normal.y : -w.triangle->normal.y) < 2000 &&
                    (queryFlags & CQ_SAM_WALL_OVERLAP)) {
                    *outFrac = 0;
                    w.vertex = w.triangle->verts;
                    w.vertices[0].x = w.vertex->x << 4;
                    w.vertices[0].y = w.vertex->y << 4;
                    w.vertices[0].z = w.vertex->z << 4;
                    ++w.vertex;
                    w.vertices[1].x = w.vertex->x << 4;
                    w.vertices[1].y = w.vertex->y << 4;
                    w.vertices[1].z = w.vertex->z << 4;
                    ++w.vertex;
                    w.vertices[2].x = w.vertex->x << 4;
                    w.vertices[2].y = w.vertex->y << 4;
                    w.vertices[2].z = w.vertex->z << 4;
                    if (Collide_BoxTriangleSweep_MinMaxY(&w.wallBox, &w.scaledDelta, w.vertices, 3, &w.triangleFraction,
                                                         &w.triangleHeight, &w.triangleMinHeight,
                                                         &w.triangle->normal)) {
                        if (w.count < 16) {
                            w.contact = &contacts[w.count];
                            w.contact->obj = 0;
                            w.contact->normal.x = w.triangle->normal.x;
                            w.contact->normal.y = w.triangle->normal.y;
                            w.contact->normal.z = w.triangle->normal.z;
                            w.contact->point.x = w.vertex->x;
                            w.contact->point.y = -w.vertex->y;
                            w.contact->point.z = -w.vertex->z;
                            ++w.count;
                        }
                        w.triangleHeight = w.vertices[0].y;
                        if (w.triangleHeight < w.vertices[1].y)
                            w.triangleHeight = w.vertices[1].y;
                        if (w.triangleHeight < w.vertices[2].y)
                            w.triangleHeight = w.vertices[2].y;
                        w.triangleMinHeight = w.vertices[0].y;
                        if (w.triangleMinHeight > w.vertices[1].y)
                            w.triangleMinHeight = w.vertices[1].y;
                        if (w.triangleMinHeight > w.vertices[2].y)
                            w.triangleMinHeight = w.vertices[2].y;
                        if (w.triangleHeight > *outMaxY)
                            *outMaxY = w.triangleHeight;
                        if (w.triangleMinHeight < *outMinY)
                            *outMinY = w.triangleMinHeight;
                        w.specialWall = 1;
                    }
                } else if (CollisionOverlapTwo(w.maxV - w.triangle->bbMin.y, w.triangle->bbMax.y - w.minV)) {
                    if (Vec3s_Dot(&w.triangle->normal, disp) <= 0) {
                        w.vertex = w.triangle->verts;
                        w.vertices[0].x = w.vertex->x << 4;
                        w.vertices[0].y = w.vertex->y << 4;
                        w.vertices[0].z = w.vertex->z << 4;
                        ++w.vertex;
                        w.vertices[1].x = w.vertex->x << 4;
                        w.vertices[1].y = w.vertex->y << 4;
                        w.vertices[1].z = w.vertex->z << 4;
                        ++w.vertex;
                        w.vertices[2].x = w.vertex->x << 4;
                        w.vertices[2].y = w.vertex->y << 4;
                        w.vertices[2].z = w.vertex->z << 4;
                        if (Collide_BoxTriangleSweep_MinMaxY(&w.scaledBox, &w.scaledDelta, w.vertices, 3,
                                                             &w.triangleFraction, &w.triangleHeight,
                                                             &w.triangleMinHeight, &w.triangle->normal)) {
                            if (w.triangleFraction < *outFrac)
                                *outFrac = w.triangleFraction;
                            if (w.count < 16) {
                                w.vertex = w.triangle->verts;
                                w.contact = &contacts[w.count];
                                w.contact->obj = 0;
                                w.contact->normal.x = w.triangle->normal.x;
                                w.contact->normal.y = w.triangle->normal.y;
                                w.contact->normal.z = w.triangle->normal.z;
                                w.contact->point.x = w.vertex->x;
                                w.contact->point.y = -w.vertex->y;
                                w.contact->point.z = -w.vertex->z;
                                ++w.count;
                            }
                            if (w.triangleHeight > *outMaxY)
                                *outMaxY = w.triangleHeight;
                            if (w.triangleMinHeight < *outMinY)
                                *outMinY = w.triangleMinHeight;
                        }
                    }
                }
            }
        }
    }
    *outMaxY = (-*outMaxY >> 4) - 1;
    *outMinY = (-*outMinY >> 4) - 1;
    *outStaticY = *outMaxY;
    if (queryFlags & CQ_OBJECTS) {
        ObjGrid_CellFromXZ(box->min.x - 500, box->min.z - 500, &w.cellX, &w.cellZ);
        ObjGrid_CellFromXZ(box->max.x + 500, box->max.z + 500, &w.cellsX, &w.cellsZ);
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
                    w.object = (ScnObject *)w.node->data; /* cast kept: a list node's payload is untyped */
                    w.excluded = 0;
                    if (queryFlags & CQ_ASK_MOVER)
                        w.excluded = mover->HandleMessage(0, MSG_COLLIDE_IGNORE_QUERY, w.object);
                    if (w.object != exclude && !w.excluded) {
                        w.boxList = w.object->inst_model->boxes;
                        if (!w.boxList) {
                            w.boxCount = 0;
                            w.otherBox = 0;
                        } else {
                            w.boxCount = w.boxList->count;
                            w.otherBox = w.boxList->boxes;
                        }
                        if (w.object->FlagsClear(SCN_OF_NO_BOX_COLLIDE)) {
                            for (; w.boxCount; --w.boxCount, ++w.otherBox) {
                                if (Collide_BoxVsObjBox(w.object, box, disp, w.otherBox, &w.object->pos,
                                                        &w.objectFraction, &w.objectHeight, contacts, &w.count)) {
                                    if (w.objectHeight < *outMaxY)
                                        *outMaxY = w.objectHeight;
                                    if (w.objectFraction < *outFrac)
                                        *outFrac = w.objectFraction;
                                }
                            }
                        }
                        w.objectFlags = w.object->flags;
                        if (w.objectFlags & SCN_OF_CUSTOM_COLLIDE) {
                            if (w.object->CustomCollide(mover, box, disp, &w.objectFraction, &w.objectHeight, contacts,
                                                        &w.count, 0)) {
                                if (w.objectHeight < *outMaxY)
                                    *outMaxY = w.objectHeight;
                                if (w.objectFraction < *outFrac)
                                    *outFrac = w.objectFraction;
                            }
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
    }
    return w.specialWall ? (w.count | SWEEP_SPECIAL_WALL) : w.count;
}

/* 0x51e033: does the segment origin -> origin + delta cross any of the boxes in the x/z plane? Slab test with t in
 * 10-bit fixed point (0..0x400); a zero delta component is replaced by 1. Only caller Sam_IsWolfBehindHidingBox. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
s32 Collide_SegmentVsBoxListXZ(Vec3s *origin, Vec3s *delta, CollBox **boxes, s32 count)
{
    struct {
        CollBox *box;
        s16 unused1c, dx;
        s32 tMaxZ, tMaxX, i, tMinZ, tMinX;
        s16 unused4, dz;
    } w;
    w.dx = delta->x;
    w.dz = delta->z;
    if (w.dx == 0)
        ++w.dx;
    if (w.dz == 0)
        ++w.dz;
    for (w.i = 0; w.i < count; ++w.i) {
        w.box = boxes[w.i];
        if (w.dx > 0) {
            w.tMinX = ((w.box->min.x - origin->x) << 10) / w.dx;
            w.tMaxX = ((w.box->max.x - origin->x) << 10) / w.dx;
        } else {
            w.tMaxX = ((w.box->min.x - origin->x) << 10) / w.dx;
            w.tMinX = ((w.box->max.x - origin->x) << 10) / w.dx;
        }
        if (w.dz > 0) {
            w.tMinZ = ((w.box->min.z - origin->z) << 10) / w.dz;
            w.tMaxZ = ((w.box->max.z - origin->z) << 10) / w.dz;
        } else {
            w.tMaxZ = ((w.box->min.z - origin->z) << 10) / w.dz;
            w.tMinZ = ((w.box->max.z - origin->z) << 10) / w.dz;
        }
        if (w.tMinX < w.tMinZ)
            w.tMinX = w.tMinZ;
        if (w.tMaxX > w.tMaxZ)
            w.tMaxX = w.tMaxZ;
        if (w.tMinX < 0)
            w.tMinX = 0;
        if (w.tMaxX > 0x400)
            w.tMaxX = 0x400;
        if (w.tMinX < w.tMaxX)
            return 1;
    }
    return 0;
}
