#!/usr/bin/env python3
"""Make a model file a mod can import: a Wavefront .obj becomes <name>.WAR and <name>.DAV in the game's formats, holding
one static model (resource 0, type 0x43) with flat-coloured triangles. A level patch imports it with
`import <name> from mod:<path> 0` (MODDING.md).

What it writes, as the game reads it (tools/war_meshes.py reads the same):
- <name>.WAR: a Black Sheep "V2.6" file: u32 CRC-32 of the rest (the game's: reflected, no final XOR), "V2.6", u32 0,
  u32 resource count (1), the table (u32: type 0x43 << 24 | offset), then the geometry record {u32 vertices, u32
  entries, u16 vertex count, u16 entry count, u32 boxes or 0, "GEOM"}, the vertices (s16 x, y, z, 0: y points down),
  the box list {u32 1; u32 flags 0 (solid); s16 min[3]; s16 max[3]} when --box is given, and one G3 entry (the
  vertex-coloured triangles most of the game's models use: 51126 in all levels, against 647 F3) {u16 2; u16 count;
  {u8 a, b, c, 0; {u8 r, g, b, 0x50}[3]}[count]}, each corner the face's colour (BsFile_ReadVertices 0x41c03f,
  BsDecode_Kind2_3 0x41c461; the 0x50 after each colour is what the game's files carry).
  Faces with texture coordinates, when their material has a texture (map_Kd, an 8-bit RGB/RGBA .png of at most
  256 x 256), are an FT3 entry instead: {u16 4; u16 count; {u8 a, b, c, 0; u8 u0, v0, u1, v1, u2, v2; u16 0;
  u32 texture id; {u8 0x80, 0x80, 0x80, 0x50}[2]}[count]} (BsDecode_Stride24_Pass1/2; the colour 0x80 leaves the
  texture as it is; the texels are within the texture's rectangle).
- <name>.DAV: the VDX7 texture file BsFile opens with it: the textures packed in rows on 256 x 256 opaque pages
  (ARGB1555), texture id i = rectangle i (Vdx7 0x41b410, PolyBatcher_LoadTexturePages 0x416fe2); empty without any.

Limits: at most 256 vertices (the entries index them by byte); faces with more than three corners are split into a fan.
Colours come from the .mtl's Kd (0-1, 1 = 255) of each face's material, white without one; a material with a
map_Kd texture draws it on the faces that have texture coordinates. The .obj's y points up and
the game's down, so y is negated. The game draws a triangle from one side only: a face wound counter-clockwise seen
from outside (the .obj convention) shows from outside with its corners kept in the .obj's order (verified in game:
reversing them hid a crate's top and showed its sides only from the wrong side). --flip reverses every triangle. A closed
model whose faces point inwards is reported (its signed volume is negative).

Usage:
  python3 tools/make_model.py crate.obj Mods/Crate/models/crate [--scale 100] [--box] [--flip]
"""
import argparse
import struct
import sys
import zlib
from pathlib import Path

TYPE_OBJECT_MESH = 0x43
TYPE_OBJECT_MODEL = 0x44
# An animated model is 8 times the level's units, a static one is not: the game's animated models carry collision
# boxes 1/8 of their vertices (Lvl-03's crate: vertices +-200, box +-26) and Anim_GetRootOffset 0x55086d divides by 8.
# So --scale means the level's units for both, and an animated model's vertices, joint offsets and moves are written
# 8 times larger, its box not.
ANIMATED_SCALE = 8
ENTRY_G3 = 2
ENTRY_FT3 = 4
PAGE = 256             # a texture page is 256 x 256 (Bs_TexelToUV divides by 256)
PAGE_OPAQUE_1555 = 5   # the page format of the game's opaque pages: kind 4, ARGB1555 (alpha bit 0, as theirs)
NEUTRAL = 0x80         # the colour of a textured triangle that leaves its texture as it is


def crc32_game(data):
    """The game's CRC-32 (Crc32): the reflected polynomial, 0xFFFFFFFF in, no final XOR."""
    table = []
    for k in range(256):
        v = k
        for _ in range(8):
            v = 0xEDB88320 ^ (v >> 1) if v & 1 else v >> 1
        table.append(v)
    c = 0xFFFFFFFF
    for b in data:
        c = table[(c ^ b) & 0xFF] ^ (c >> 8)
    return c


def read_png(path):
    """An 8-bit RGB or RGBA, non-interlaced .png: (width, height, rows of (r, g, b) tuples)."""
    d = Path(path).read_bytes()
    if d[:8] != b"\x89PNG\r\n\x1a\n":
        raise ValueError("%s: not a .png" % path)
    o, idat, w = 8, b"", 0
    while o < len(d):
        n, kind = struct.unpack_from(">I4s", d, o)
        body = d[o + 8:o + 8 + n]
        if kind == b"IHDR":
            w, h, depth, ctype, _, _, interlace = struct.unpack(">IIBBBBB", body)
            if depth != 8 or ctype not in (2, 6) or interlace:
                raise ValueError("%s: only 8-bit RGB / RGBA, not interlaced" % path)
            bpp = 3 if ctype == 2 else 4
        elif kind == b"IDAT":
            idat += body
        o += 12 + n
    raw, stride, prev, rows = zlib.decompress(idat), w * bpp, bytearray(w * bpp), []
    for y in range(h):
        f, line = raw[y * (stride + 1)], bytearray(raw[y * (stride + 1) + 1:(y + 1) * (stride + 1)])
        for i in range(stride):  # the five PNG row filters
            a = line[i - bpp] if i >= bpp else 0
            b, c = prev[i], prev[i - bpp] if i >= bpp else 0
            if f == 1:
                line[i] = (line[i] + a) & 255
            elif f == 2:
                line[i] = (line[i] + b) & 255
            elif f == 3:
                line[i] = (line[i] + (a + b) // 2) & 255
            elif f == 4:
                pa, pb, pc = abs(b - c), abs(a - c), abs(a + b - 2 * c)
                line[i] = (line[i] + (a if pa <= pb and pa <= pc else b if pb <= pc else c)) & 255
        rows.append([tuple(line[x * bpp:x * bpp + 3]) for x in range(w)])
        prev = line
    return w, h, rows


def read_obj(path):
    """Vertices, the textures (the .mtl's map_Kd files, in first-use order), and triangles as
    (corners, (r, g, b), texture index or None, group): corners are (vertex, (u, v) or None); the group is the last
    `o` or `g` name before the face (a part of an animated model)."""
    verts, uvs, tris, colours, maps, textures = [], [], [], {}, {}, []
    current, use_map, group = (255, 255, 255), None, None
    folder = Path(path).parent
    for line in Path(path).read_text().splitlines():
        w = line.split()
        if not w or w[0].startswith("#"):
            continue
        if w[0] == "mtllib":
            name = None
            for m in (folder / " ".join(w[1:])).read_text().splitlines():
                m = m.split()
                if m and m[0] == "newmtl":
                    name = " ".join(m[1:])
                elif m and m[0] == "Kd" and name:
                    colours[name] = tuple(max(0, min(255, round(float(x) * 255))) for x in m[1:4])
                elif m and m[0] == "map_Kd" and name:
                    maps[name] = folder / " ".join(m[1:])
        elif w[0] == "usemtl":
            name = " ".join(w[1:])
            current, use_map = colours.get(name, (255, 255, 255)), maps.get(name)
            if use_map and use_map not in textures:
                textures.append(use_map)
        elif w[0] in ("o", "g"):
            group = " ".join(w[1:])
        elif w[0] == "v":
            verts.append(tuple(float(x) for x in w[1:4]))
        elif w[0] == "vt":
            uvs.append((float(w[1]), float(w[2]) if len(w) > 2 else 0.0))
        elif w[0] == "f":
            corners = []
            for x in w[1:]:
                parts = x.split("/")
                i = int(parts[0])
                uv = int(parts[1]) if len(parts) > 1 and parts[1] else None
                corners.append((i - 1 if i > 0 else len(verts) + i,
                                None if uv is None or not use_map else uvs[uv - 1 if uv > 0 else len(uvs) + uv]))
            tex = textures.index(use_map) if use_map and all(c[1] is not None for c in corners) else None
            for k in range(1, len(corners) - 1):
                tris.append(((corners[0], corners[k], corners[k + 1]), current, tex, group))
    return verts, tris, textures


def pack(sizes):
    """Places textures of these (w, h) on 256 x 256 pages, in rows (tallest first): (x, y, page) for each."""
    order = sorted(range(len(sizes)), key=lambda i: -sizes[i][1])
    at, page, x, y, row = [None] * len(sizes), 0, 0, 0, 0
    for i in order:
        w, h = sizes[i]
        if w > PAGE or h > PAGE:
            raise ValueError("a texture is %dx%d: at most %dx%d" % (w, h, PAGE, PAGE))
        if x + w > PAGE:
            x, y, row = 0, y + row, 0
        if y + h > PAGE:
            page, x, y, row = page + 1, 0, 0, 0
        at[i] = (x, y, page)
        x, row = x + w, max(row, h)
    return at


def read_rig(path):
    """An animated model's parts and animations (a .rig text file, '#' starts a comment):
        joint <name> <parent name, or - for the root> <pivot x y z, in .obj units>
        anim <id> <name>          the game's animation id (static scenery plays the lowest it finds, so 0), a name
        key <ms>                  a key of that animation, lasting <ms>
        <joint> rot|pos|scale <x> <y> <z>   in the game's space (y points down): rot in 1/4096 turn (Euler, applied
                                  y, x, z), pos in the level's units (written ANIMATED_SCALE times larger, as
                                  the model), scale 1024 = 1; a joint not given in a key is at rest
    Each joint's faces are the .obj's group (`o` or `g`) of that name. The root is added before them: joints
    are numbered from 1 in the order given."""
    joints, anims = [("root", None, (0.0, 0.0, 0.0))], []
    for n, line in enumerate(Path(path).read_text().splitlines(), 1):
        w = line.split("#")[0].split()
        if not w:
            continue
        names = [j[0] for j in joints]
        if w[0] == "joint" and len(w) == 6:
            parent = 0 if w[2] == "-" else names.index(w[2]) if w[2] in names else None
            if parent is None:
                raise ValueError("%s line %d: joint %s is not defined yet" % (path, n, w[2]))
            joints.append((w[1], parent, tuple(float(x) for x in w[3:6])))
        elif w[0] == "anim" and len(w) == 3:
            anims.append({"id": int(w[1], 0), "name": w[2], "keys": []})
        elif w[0] == "key" and len(w) == 2 and anims:
            anims[-1]["keys"].append({"ms": int(w[1]), "joints": {}})
        elif len(w) == 5 and w[1] in ("rot", "pos", "scale") and anims and anims[-1]["keys"] and w[0] in names:
            anims[-1]["keys"][-1]["joints"].setdefault(names.index(w[0]), {})[w[1]] = tuple(int(x, 0) for x in w[2:5])
        else:
            raise ValueError("%s line %d: %s" % (path, n, line.strip()))
    if len(joints) < 2 or not anims or any(not a["keys"] for a in anims):
        raise ValueError("%s: a rig needs a joint, and each animation a key" % path)
    return joints, anims


def rig_layout(verts, tris, joints):
    """Vertices in joint order (each part's, in its own space: less its pivot), and the triangles renumbered."""
    names = [j[0] for j in joints]
    order, index, newverts = [], {}, []
    for tr in tris:
        if tr[3] not in names[1:]:
            raise ValueError("a face in group %s, which is no joint of the rig" % repr(tr[3]))
    for j in range(1, len(joints)):
        pivot = joints[j][2]
        count = 0
        for tr in tris:
            if names.index(tr[3]) != j:
                continue
            for v, _ in tr[0]:
                if (j, v) not in index:
                    index[(j, v)] = len(newverts)
                    newverts.append(tuple(a - b for a, b in zip(verts[v], pivot)))
                    count += 1
        order.append(count)
    newtris = [(tuple((index[(names.index(tr[3]), v)], uv) for v, uv in tr[0]),) + tuple(tr[1:]) for tr in tris]
    return newverts, newtris, order


def rig_blocks(joints, anims, counts, scale, at):
    """The joint table, the two animation lists and the animations, from file offset `at`: (bytes, lists offset,
    joints offset)."""
    def s16(v):
        if not -32768 <= v <= 32767:
            raise ValueError("a value is outside the 16-bit range")
        return v
    out = bytearray()
    joints_at = at
    for j, (name, parent, pivot) in enumerate(joints):
        base = joints[parent][2] if parent is not None else (0.0, 0.0, 0.0)
        off = [round((a - b) * scale) for a, b in zip(pivot, base)]
        off[1] = -off[1]  # y points down in the game
        out += struct.pack("<H3hH", parent or 0, *map(s16, off), counts[j - 1] if j else 0)
    while (at + len(out)) % 4:
        out += b"\0"
    starts = []
    for a in anims:
        starts.append(at + len(out))
        body = bytearray()
        for k in a["keys"]:
            words = []
            for j in range(len(joints)):  # every joint in every key: one not listed would keep an unset pose
                ch = k["joints"].get(j, {})
                control, vals = j, []
                for bit, key in ((0x40, "rot"), (0x200, "pos"), (0x1000, "scale")):
                    if key in ch or (key == "rot" and not ch):
                        control |= bit * 7  # the x, y and z bits of that channel
                        xyz = ch.get(key, (0, 0, 0))
                        if key == "scale" and not all(0 <= v <= 0xFFFF for v in xyz):
                            raise ValueError("a scale is outside 0-65535")
                        vals += (list(xyz) if key == "scale" else [s16(v) for v in xyz] if key == "rot"
                                 else [s16(v * ANIMATED_SCALE) for v in xyz])
                words += [control] + vals
            payload = struct.pack("<%dH" % len(words), *[w & 0xFFFF for w in words])
            body += struct.pack("<4H", k["ms"], len(joints), len(payload) // 2, 0) + payload
        out += a["name"].encode()[:8].ljust(8, b"\0") + struct.pack("<H", len(a["keys"])) + body
        while (at + len(out)) % 4:
            out += b"\0"
    lists_at = at + len(out)
    mapped = [0] * (max(a["id"] for a in anims) + 1)
    for a, s in zip(anims, starts):
        mapped[a["id"]] = s
    out += struct.pack("<I%dI" % len(starts), len(starts), *starts) + struct.pack("<I%dI" % len(mapped), len(mapped), *mapped)
    return bytes(out), lists_at, joints_at


def build(verts, tris, scale, box, flip, sizes=(), rig=None):
    """The .WAR. `sizes` are the textures' (w, h); a textured triangle's texture id is its texture's index."""
    if not tris:
        raise ValueError("no faces")
    if len(verts) > 256:
        raise ValueError("%d vertices: a model holds at most 256" % len(verts))
    pts, level = [], []
    unit = ANIMATED_SCALE if rig else 1
    for x, y, z in verts:
        level.append((round(x * scale), round(-y * scale), round(z * scale)))
        p = (round(x * scale * unit), round(-y * scale * unit), round(z * scale * unit))
        if any(not -32768 <= c <= 32767 for c in p):
            raise ValueError("a vertex is outside the 16-bit range at this scale")
        pts.append(p)
    flat = [tr for tr in tris if tr[2] is None]
    tex = [tr for tr in tris if tr[2] is not None]
    rec = 0x14
    vtx = rec + (0x20 if rig else 0x14)
    boxes = vtx + 8 * len(pts) if box else 0
    ent = (boxes + 20) if box else vtx + 8 * len(pts)
    nent = bool(flat) + bool(tex)
    out = bytearray(b"\0" * 4 + b"V2.6" + struct.pack("<II", 0, 1) +
                    struct.pack("<I", (TYPE_OBJECT_MODEL if rig else TYPE_OBJECT_MESH) << 24 | rec))
    if rig:  # {..., u32 animation lists, u32 joints, u16 joint count, u16 0, "GEOM"}: filled in at the end
        out += struct.pack("<IIHHIIIHH4s", vtx, ent, len(pts), nent, boxes, 0, 0, len(rig[0]), 0, b"GEOM")
    else:
        out += struct.pack("<IIHHI4s", vtx, ent, len(pts), nent, boxes, b"GEOM")
    for p in pts:
        out += struct.pack("<4h", *p, 0)
    if box:  # in the level's units
        lo = [min(p[i] for p in level) for i in range(3)]
        hi = [max(p[i] for p in level) for i in range(3)]
        out += struct.pack("<II3h3h", 1, 0, *lo, *hi)

    def corners(tr):
        a, b, c = tr[0]
        return (a, c, b) if flip else (a, b, c)  # the .obj's order (see the file's comment)

    if flat:
        out += struct.pack("<HH", ENTRY_G3, len(flat))
        for tr in flat:
            (r, g, bl) = tr[1]
            out += struct.pack("<4B", *(c[0] for c in corners(tr)), 0) + struct.pack("<4B", r, g, bl, 0x50) * 3
    if tex:
        out += struct.pack("<HH", ENTRY_FT3, len(tex))
        for tr in tex:
            w, h = sizes[tr[2]]
            cs = corners(tr)
            texels = []
            for _, (u, v) in cs:  # texels in the texture's rectangle; the .obj's v points up, the image's rows down
                texels += [max(0, min(255, round(u * (w - 1)))), max(0, min(255, round((1 - v) * (h - 1))))]
            colour = struct.pack("<4B", NEUTRAL, NEUTRAL, NEUTRAL, 0x50)
            out += struct.pack("<4B6BHI", *(c[0] for c in cs), 0, *texels, 0, tr[2]) + colour * 2
    if rig:
        while len(out) % 4:
            out += b"\0"
        blocks, lists_at, joints_at = rig_blocks(rig[0], rig[1], rig[2], scale * ANIMATED_SCALE, len(out))
        out += blocks
        struct.pack_into("<II", out, rec + 0x10, lists_at, joints_at)
    struct.pack_into("<I", out, 0, crc32_game(out[4:]))
    return bytes(out)


def dav(textures=()):
    """The .DAV: VDX7, four CHEK words, the header's offset (0x24), "_SRA" + where the pages start, "_SDA", then the
    header {u16 entries, rectangles, pages; u32 their offsets}, the entry table (u16 per texture id: its rectangle),
    the rectangles {u16 x, w, y, h, page}, the pages {u16 x0, x1, y0, y1, format; u16 pixels[256 * 256]}, "END ".
    Texture id i is rectangle i, the place pack() gives it on an opaque page (ARGB1555, alpha bit 0 as the game's)."""
    head = 0x24
    at = pack([(w, h) for w, h, _ in textures])
    npages = max((p for _, _, p in at), default=-1) + 1
    ent_at = head + 18
    rec_at = ent_at + 2 * len(textures)
    pages_at = rec_at + 10 * len(textures)
    pages = [bytearray(2 * PAGE * PAGE) for _ in range(npages)]
    rects = b""
    for (w, h, rows), (x0, y0, p) in zip(textures, at):
        for y in range(h):
            for x in range(w):
                r, g, b = rows[y][x]
                struct.pack_into("<H", pages[p], 2 * ((y0 + y) * PAGE + x0 + x), (r >> 3) << 10 | (g >> 3) << 5 | b >> 3)
        rects += struct.pack("<5H", x0, w, y0, h, p)
    return (b"VDX7" + b"CHEK" * 4 + struct.pack("<I", head) + b"_SRA" + struct.pack("<I", pages_at) + b"_SDA" +
            struct.pack("<3H3I", len(textures), len(textures), npages, ent_at, rec_at, pages_at) +
            b"".join(struct.pack("<H", i) for i in range(len(textures))) + rects +
            b"".join(struct.pack("<5H", 0, PAGE, 0, PAGE, PAGE_OPAQUE_1555) + bytes(pg) for pg in pages) + b"END ")


def dot(a, b):
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2]


def cross(a, b):
    return (a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0])


def main(argv):
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ap.add_argument("obj")
    ap.add_argument("out", help="the output path without extension: writes <out>.WAR and <out>.DAV")
    ap.add_argument("--scale", type=float, default=100.0, help="game units per .obj unit (default 100)")
    ap.add_argument("--box", action="store_true", help="give the model a solid collision box around it")
    ap.add_argument("--flip", action="store_true", help="reverse every triangle's corners")
    ap.add_argument("--rig", help="a .rig file: the model is animated (its parts, pivots and animations)")
    a = ap.parse_args(argv)
    verts, tris, textures = read_obj(a.obj)
    volume = sum(dot(verts[tr[0][0][0]], cross(verts[tr[0][1][0]], verts[tr[0][2][0]])) for tr in tris) / 6
    if volume < 0:
        print("warning: %s: its faces point inwards (signed volume %.3g): wind them counter-clockwise seen from "
              "outside, or give --flip" % (a.obj, volume))
    images = [read_png(tx) for tx in textures]
    rig = None
    if a.rig:
        joints, anims = read_rig(Path(a.obj).parent / a.rig if not Path(a.rig).is_absolute() else a.rig)
        verts, tris, counts = rig_layout(verts, tris, joints)
        rig = (joints, anims, counts)
    war = build(verts, tris, a.scale, a.box, a.flip, [im[:2] for im in images], rig)
    out = Path(a.out)
    out.parent.mkdir(parents=True, exist_ok=True)
    out.with_suffix(".WAR").write_bytes(war)
    out.with_suffix(".DAV").write_bytes(dav(images))
    print("wrote %s.WAR (%d vertices, %d triangles%s%s) and %s.DAV" % (
        out, len(verts), len(tris), ", %d textured from %d texture(s)" % (
            sum(1 for tr in tris if tr[2] is not None), len(images)) if images else "",
        ", a collision box" if a.box else "", out))
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
