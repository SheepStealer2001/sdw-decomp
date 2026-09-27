#!/usr/bin/env python3
"""List a level's models (the mesh resources of its .WAR) and the texture pages of its .DAV each one draws with.

Read-only. What it reads, as the game reads it:
- The .WAR is also a Black Sheep "V2.6" file (BsFile 0x41b7f0): the resource table at file offset 0x10 (u32 per entry,
  type in the top byte with bit 0x40 ignored, offset in the low 24 bits) is the BsFile's frame directory, and
  Load_WarMeshes 0x54a326 builds one Mesh per resource of type 3, 4, 0xb or 0x26, in table order.
- A geometry record: +0 u32 vertex table offset, +4 u32 entry table offset, +8 u16 vertex count, +0xa u16 entry
  count (BsFile_ReadVertices, BsFile_FindEntryValue); a model (type 4) goes on with +0xc boxes, +0x10 anim table,
  +0x14 joints, +0x18 u16 joint count. Entries are {u16 kind; u16 count; payload[count * stride]}, the strides of
  BsFile_SkipEntry 0x41fc89 (data/enums/BsEntryKind.csv).
- The textured kinds carry a u32 texture id: at element offset 12 for FT3/FT4/GT3/GT4/BFT3/BGT3, at 4 for the
  fixed-square kinds 8-0xf (the decoders in src/engine/bs_file.cpp). The id indexes the .DAV's entry table, whose
  value indexes its rectangle records {u16 x, w, y, h, page} (Vdx7 0x41b410); `page` is the index of the texture page
  in the PolyBatcher, which loads the .DAV's pages in file order (PolyBatcher_LoadTexturePages 0x416fe2) and then adds
  four blank ones that the interface and the map draw into (texturePageCount - 1 .. - 4).

Usage:
  python3 tools/war_meshes.py Lvl-03            # every model of the level, with the pages each uses
  python3 tools/war_meshes.py Lvl-03 25         # one resource: its entries, texture ids and rectangles
  python3 tools/war_meshes.py Lvl-03 --pages    # the .DAV's texture pages
  python3 tools/war_meshes.py all --check       # consistency over every level (ids in range, records inside the file)
"""
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
LEVELS = ROOT / "Sheep, Dog 'n' Wolf (PAL Version)" / "Levels"

STRIDE = {0x00: 8, 0x01: 8, 0x02: 16, 0x03: 20, 0x04: 24, 0x05: 24, 0x06: 40, 0x07: 48, 0x08: 48, 0x09: 48,
          0x0a: 48, 0x0b: 48, 0x0c: 24, 0x0d: 24, 0x0e: 24, 0x0f: 24, 0x10: 12, 0x11: 12, 0x12: 20, 0x13: 24,
          0x14: 24, 0x15: 40, 0x16: 20}
KIND = ["F3", "F4", "G3", "G4", "FT3", "FT4", "GT3", "GT4", "GT4_SQ64_A", "GT4_SQ64_B", "GT4_SQ32_A", "GT4_SQ32_B",
        "FT4_SQ64_A", "FT4_SQ64_B", "FT4_SQ32_A", "FT4_SQ32_B", "BF3", "BF4", "BG3", "BG4", "BFT3", "BGT3", "BG3_ALT"]
TEX_ID_AT = {k: 12 for k in (0x04, 0x05, 0x06, 0x07, 0x14, 0x15)}
TEX_ID_AT.update({k: 4 for k in range(0x08, 0x10)})
MESH_TYPES = {3: "mesh", 4: "model", 0x0b: "mesh_b", 0x26: "sky"}
TEXFMT_KIND = {4: "opaque", 8: "blend", 0x10: "add"}  # format & 0x1c (PolyBatcher_LoadTexturePages)


def level_files(level):
    folder = LEVELS / level
    files = {f.suffix.lower(): f for f in folder.iterdir()}
    return files[".war"], files[".dav"]


def read_dav(path):
    """The Vdx7 tables (entries, rectangles) and the page headers."""
    d = path.read_bytes()
    if d[:4] != b"VDX7":
        raise ValueError("%s: not a VDX7 file" % path.name)
    hdr, = struct.unpack_from("<I", d, 0x14)
    n_entries, n_rec, n_pages, ent_at, rec_at, pages_at = struct.unpack_from("<3H3I", d, hdr)
    entries = struct.unpack_from("<%dH" % n_entries, d, ent_at)
    rects = [struct.unpack_from("<5H", d, rec_at + 10 * i) for i in range(n_rec)]  # x, w, y, h, page
    pages, o = [], pages_at
    for i in range(n_pages):
        x0, x1, y0, y1, fmt = struct.unpack_from("<5H", d, o)
        w, h = abs(x0 - x1), abs(y0 - y1)
        pages.append({"index": i, "w": w, "h": h, "format": fmt, "kind": TEXFMT_KIND.get(fmt & 0x1c, "?%x" % fmt),
                      "offset": o, "bytes": 10 + 2 * w * h})
        o += 10 + 2 * w * h
    return {"entries": entries, "rects": rects, "pages": pages, "end": o, "size": len(d), "trailer": d[o:]}


def read_mesh(d, off):
    """A geometry record: its vertex count and entries, with the texture ids of the textured ones."""
    vtx_at, ent_at, n_vtx, n_ent = struct.unpack_from("<IIHH", d, off)
    o, entries = ent_at, []
    for _ in range(n_ent):
        kind, count = struct.unpack_from("<HH", d, o)
        if kind not in STRIDE:
            raise ValueError("entry kind 0x%x at 0x%x" % (kind, o))
        ids = []
        if kind in TEX_ID_AT:
            ids = [struct.unpack_from("<I", d, o + 4 + i * STRIDE[kind] + TEX_ID_AT[kind])[0] for i in range(count)]
        entries.append({"kind": kind, "count": count, "at": o, "ids": ids})
        o += 4 + count * STRIDE[kind]
    return {"vertices": n_vtx, "vertex_at": vtx_at, "entries": entries, "end": o}


def meshes(level):
    war, davp = level_files(level)
    d = war.read_bytes()
    count, = struct.unpack_from("<I", d, 0x0C)
    table = struct.unpack_from("<%dI" % count, d, 0x10)
    dav = read_dav(davp)
    out = []
    for i, e in enumerate(table):
        t = (e >> 24) & 0xBF
        if t not in MESH_TYPES:
            continue
        m = read_mesh(d, e & 0xFFFFFF)
        ids = sorted({x for en in m["entries"] for x in en["ids"]})
        pages = sorted({dav["rects"][dav["entries"][x]][4] for x in ids if x < len(dav["entries"])})
        out.append(dict(m, res=i, type=t, offset=e & 0xFFFFFF, ids=ids, pages=pages))
    return d, dav, out


def show_level(level):
    d, dav, ms = meshes(level)
    print("%s: %d models, %d texture pages (+4 blank), %d texture ids" % (level, len(ms), len(dav["pages"]),
                                                                           len(dav["entries"])))
    for m in ms:
        polys = sum(en["count"] for en in m["entries"])
        print("  res %4d  %-6s at 0x%06x  %4d vertices %4d prims  pages %s" % (
            m["res"], MESH_TYPES[m["type"]], m["offset"], m["vertices"], polys,
            " ".join(str(p) for p in m["pages"]) or "-"))


def show_one(level, res):
    d, dav, ms = meshes(level)
    m = next((x for x in ms if x["res"] == res), None)
    if m is None:
        sys.exit("%s: resource %d is not a model" % (level, res))
    print("%s res %d: %s at 0x%06x, %d vertices at 0x%x, entries end at 0x%x" % (
        level, res, MESH_TYPES[m["type"]], m["offset"], m["vertices"], m["vertex_at"], m["end"]))
    for en in m["entries"]:
        print("  %-10s x%-4d at 0x%06x" % (KIND[en["kind"]], en["count"], en["at"]))
    for x in m["ids"]:
        r = dav["entries"][x]
        print("  texture id %4d -> rect %4d: x %3d w %3d y %3d h %3d page %d" % ((x, r) + dav["rects"][r]))


def show_pages(level):
    dav = read_dav(level_files(level)[1])
    for p in dav["pages"]:
        print("  page %3d  %3dx%-3d format 0x%04x %-6s at 0x%07x" % (p["index"], p["w"], p["h"], p["format"],
                                                                    p["kind"], p["offset"]))


def check(level):
    """Everything the importer relies on: records and entries inside the file, ids and rectangles in range, the
    opaque pages first (PolyBatcher_LoadTexturePages counts them until the first other kind), the pages followed only
    by the file's "END " tag."""
    d, dav, ms = meshes(level)
    problems = []
    for m in ms:
        if m["end"] > len(d):
            problems.append("res %d runs past the file" % m["res"])
        for x in m["ids"]:
            if x >= len(dav["entries"]):
                problems.append("res %d: texture id %d out of range" % (m["res"], x))
            elif dav["entries"][x] >= len(dav["rects"]):
                problems.append("res %d: id %d -> rect out of range" % (m["res"], x))
            elif dav["rects"][dav["entries"][x]][4] >= len(dav["pages"]):
                problems.append("res %d: id %d -> page out of range" % (m["res"], x))
    kinds = [p["kind"] for p in dav["pages"]]
    first_other = next((i for i, k in enumerate(kinds) if k != "opaque"), len(kinds))
    if any(k == "opaque" for k in kinds[first_other:]):
        problems.append("an opaque page after the first blended one")
    if dav["trailer"] != b"END ":
        problems.append("the pages end at 0x%x, then %r" % (dav["end"], dav["trailer"][:8]))
    print("%s: %d models, %d pages (%d opaque), %s" % (level, len(ms), len(kinds), first_other,
                                                       "; ".join(problems) or "ok"))
    return not problems


def main(argv):
    if not argv:
        sys.exit(__doc__)
    levels = sorted(p.name for p in LEVELS.iterdir() if p.is_dir() and any(
        f.suffix.lower() == ".war" for f in p.iterdir())) if argv[0] == "all" else [argv[0]]
    rest = argv[1:]
    ok = True
    for level in levels:
        if "--check" in rest:
            ok &= check(level)
        elif "--pages" in rest:
            show_pages(level)
        elif rest:
            show_one(level, int(rest[0], 0))
        else:
            show_level(level)
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
