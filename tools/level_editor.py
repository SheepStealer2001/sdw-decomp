#!/usr/bin/env python3
"""The level editor: a level in 3D in the browser, its objects moved, copied, brought from other levels, changed or
removed, saved as a mod's level patch (levels/<Level>.txt, the format mods/loader/levelpatch.c applies; MODDING.md).
The game's files are only read.

  python3 tools/level_editor.py                             a start screen: pick the level and the mod
  python3 tools/level_editor.py "Level 3" --mod MyMod      straight to Level 3 (Lvl-03) of MyMod
  python3 tools/level_editor.py --mods DIR                  the mods of another folder
  python3 tools/level_editor.py --app                       as the launchers start it (SDW Level Editor.app / .bat):
                                                          one editor at a time, stopping when its page is closed
  python3 tools/level_editor.py --check                     a test of reading and writing patches

It saves into the installed game's Mods folder, found by itself (a CrossOver or Wine bottle's, or Program Files' on
Windows: the game folder with a Mods folder first), or mods/examples when there is no installed game; --mods names
another. A mod's patch is <Mods>/<Mod>/levels/<Level>.txt.

A level is its folder name (Lvl-03, Scene) or its name as players number the levels ("Level 3", B1, B2, "Planet X").

It serves the editor (tools/editor/level_editor.html) on this computer only (127.0.0.1) and opens it in the browser.
The level: its collision mesh (the ground and walls, tools/war_collision.py) and its objects (position, rotation, model,
class, designer properties named from src/include/scenaric_props.h). A patch the mod already has is read, so the
editor starts from it; lines the editor does not change (import, sound, text, comments) are kept as they are.
"""
import argparse
import json
import os
import re
import struct
import sys
import tempfile
import threading
import time
import webbrowser
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
from urllib.parse import parse_qs, urlparse

sys.path.insert(0, str(Path(__file__).resolve().parent))
import textio  # noqa: F401,E402  (UTF-8, LF text files on Windows)
import build_mods  # noqa: E402
import scenaric_to_c  # noqa: E402
import war_meshes  # noqa: E402
import war_collision  # noqa: E402

ROOT = Path(__file__).resolve().parent.parent
LEVELS = ROOT / "Sheep, Dog 'n' Wolf (PAL Version)" / "Levels"
PAGE = Path(__file__).resolve().parent / "editor" / "level_editor.html"
LEVEL_NAME = re.compile(r"^[A-Za-z0-9_-]{1,32}$")
LEVEL_NAMES = {**{"Lvl-%02d" % n: "Level %d" % n for n in range(0, 5)}, "Lvl-05": "Level B1",
             **{"Lvl-%02d" % (n + 1): "Level %d" % n for n in range(5, 9)}, "Lvl-10": "Level B2",
             **{"Lvl-%02d" % (n + 2): "Level %d" % n for n in range(9, 15)}, "Lvl-17": "Planet X"}


# ---- the level ----

def schema():
    """{class id: {"name", "props": [(offset, name)]}}: the class names of src/include/scenaric_props.h (generated
    from the disc) and the property names the mod loader accepts (build_mods.scenaric_props, which it is built from)."""
    h = ROOT / "src" / "include" / "scenaric_props.h"
    text = h.read_text() if h.exists() else ""
    out = {int(m.group(2)): {"name": m.group(1), "props": []}
           for m in re.finditer(r'// class "([^"]+)" \(id (\d+)\)', text)}
    for cid, off, name in build_mods.scenaric_props():
        out.setdefault(cid, {"name": "class %d" % cid, "props": []})["props"].append((off, name))
    return out


def game_folders():
    """Installed copies of the game (folders holding SheepD3D.exe), the ones with a Mods folder (the mod loader's)
    first: CrossOver and Wine bottles, and Program Files on Windows."""
    home = Path.home()
    roots = [p / "drive_c" for p in (home / "Library" / "Application Support" / "CrossOver" / "Bottles").glob("*")]
    roots += [home / ".wine" / "drive_c", Path("C:/")]
    found = []
    for root in roots:
        for exe in list(root.glob("Program Files*/*/SheepD3D.exe")) + list(root.glob("Program Files*/*/*/SheepD3D.exe")):
            if exe.parent not in found:
                found.append(exe.parent)
    return sorted(found, key=lambda f: not (f / "Mods").is_dir())


def make_names(games):
    """The objects' class and property names come from src/include/scenaric_props.h, which tools/scenaric_to_c.py
    generates from the game's own Scenaric_Classes.h. When it has not been generated, generate it now: from the
    repository's copy of the disc, else from an installed game's Levels/Lvl-03 (the game installs that header too)."""
    if scenaric_to_c.OUT_H.exists():
        return
    for src in [scenaric_to_c.SRC] + [g / "Levels" / "Lvl-03" / "Scenaric_Classes.h" for g in games]:
        if src.is_file() and src.with_name("GameRes.h").is_file():
            print("the objects' names, from %s:" % src)
            scenaric_to_c.main(src)
            return
    print("no Scenaric_Classes.h found: the objects are shown by class number")


def writable(folder):
    """True when the editor can save into `folder`, or, while it does not exist yet, into the folder that will hold it.
    Under Program Files, Windows lets only administrators write."""
    d = Path(folder)
    while not d.exists() and d.parent != d:
        d = d.parent
    try:
        with tempfile.TemporaryFile(dir=d):
            return True
    except OSError:
        return False


def use_levels(folder):
    """Read the levels from `folder` (the installed game's Levels) instead of the repository's copy of the disc."""
    global LEVELS
    LEVELS = war_collision.LEVELS = Path(folder)


def level_folder(name):
    """A level's folder from its folder name or its name as players number the levels, or None."""
    names = {k.lower(): k for k in levels()}
    names.update({v.lower(): k for k, v in LEVEL_NAMES.items()})
    names.update({v.lower().replace("level ", ""): k for k, v in LEVEL_NAMES.items() if v.startswith("Level B")})
    return names.get(name.strip().lower())


def levels():
    return sorted(p.name for p in LEVELS.iterdir() if p.is_dir() and any(f.suffix.lower() == ".war" for f in p.iterdir()))


def war_bytes(level):
    folder = LEVELS / level
    return [f for f in folder.iterdir() if f.suffix.lower() == ".war"][0].read_bytes()


def read_objects(level, sch):
    """Every object of a level (resource type 5, with or without bit 0x40), as the editor shows it."""
    d = war_bytes(level)
    count, = struct.unpack_from("<I", d, 0x0C)
    table = struct.unpack_from("<%dI" % count, d, 0x10)
    out = []
    for i, e in enumerate(table):
        if (e >> 24) & 0xBF != 5:
            continue
        off = e & 0xFFFFFF
        model, = struct.unpack_from("<H", d, off)
        x, y, z = struct.unpack_from("<3h", d, off + 4)
        cls, = struct.unpack_from("<H", d, off + 0x0A)
        rot = list(struct.unpack_from("<3h", d, off + 0x0C))
        c = sch.get(cls, {"name": "class %d" % cls, "props": []})
        props = {"0x%x" % o: struct.unpack_from("<i", d, off + 0x14 + o)[0] for o, _ in c["props"]}
        out.append({"res": i, "class": cls, "model": model, "pos": [x, y, z], "rot": rot, "props": props})
    return out


def read_ground(level):
    """The collision triangles as a flat list x, y, z (game space: y points down) and each one's surface kind."""
    _, _, tris = war_collision.load(level)
    kinds = {"floor": 0, "slope": 1, "steep": 2, "wall-back": 3, "wall": 3, "overhang": 4}
    flat, kind = [], []
    for t in tris:
        for v in t["verts"]:
            flat += list(v)
        kind.append(kinds.get(t["cls"], 3))
    return flat, kind


# ---- geometry: the level's scenery and the objects' models, as the game decodes them (src/engine/bs_file.cpp) ----
# A geometry record: +0 u32 vertices (s16 x, y, z, pad), +4 u32 entries, +8 u16 vertex count, +0xa u16 entry count; an
# animated model (0x44) also has +0x14 u32 joints {u16 parent, s16 x, y, z, u16 vertex count}, +0x18 u16 joint count,
# its vertices each joint's in turn, in that joint's space. Every polygon starts with its u8 vertex indices; a quad
# a b c d is the triangles (a, d, c) and (a, b, d). The texel pairs sit at +4 and the texture id at +12, except in the
# fixed-square quads (kinds 8-0xf: the id at +4, the square's corners implied).

TRI, QUAD = 3, 4
# An animated model (0x44) is 8 times the level's units: its collision boxes are 1/8 of its vertices (the crate, res 4
# of Lvl-03: vertices +-200 and 400 high, box +-26 and 51 high) and Anim_GetRootOffset 0x55086d divides by 8 to get
# back to the level's units. A static one (0x43) is in the level's units (its boxes match its vertices).
ANIMATED_SCALE = 8
# kind: (corners, colour offsets per corner or one, textured: texels at +4 / "square" / None)
POLY = {0x00: (TRI, (4,), None), 0x01: (QUAD, (4,), None), 0x02: (TRI, (4, 8, 12), None),
        0x03: (QUAD, (4, 8, 12, 16), None), 0x04: (TRI, (16,), 4), 0x05: (QUAD, (16,), 4),
        0x06: (TRI, (16, 20, 24), 4), 0x07: (QUAD, (16, 20, 24, 28), 4),
        **{k: (QUAD, (16, 20, 24, 28), "square") for k in range(0x08, 0x0C)},
        **{k: (QUAD, (16,), "square") for k in range(0x0C, 0x10)},
        0x10: (TRI, (4,), None), 0x11: (QUAD, (4,), None), 0x12: (TRI, (4, 8, 12), None),
        0x13: (QUAD, (4, 8, 12, 16), None), 0x14: (TRI, (16,), 4), 0x15: (TRI, (16, 20, 24), 4),
        0x16: (TRI, (4, 8, 12), None)}
SQUARE = {0x08: 64, 0x09: 64, 0x0A: 32, 0x0B: 32, 0x0C: 64, 0x0D: 64, 0x0E: 32, 0x0F: 32}
SPLIT_A = (0x08, 0x0A, 0x0C, 0x0E)  # the square's corners: A (0,0) (0,c) (c,0) (c,c); B has the two middle ones swapped


def dav_tables(dav):
    """The texture ids' rectangles {x, w, y, h, page} and the pages (w, h, kind) of a .DAV (tools/war_meshes.py)."""
    hdr, = struct.unpack_from("<I", dav, 0x14)
    n_ent, n_rec, n_pages, ent_at, rec_at, pages_at = struct.unpack_from("<3H3I", dav, hdr)
    entries = struct.unpack_from("<%dH" % n_ent, dav, ent_at)
    rects = [struct.unpack_from("<5H", dav, rec_at + 10 * i) for i in range(n_rec)]
    return entries, rects, pages_at, n_pages


def pose_vertices(war, rec, verts):
    """An animated model's vertices in its default pose, with no animation applied: each joint's vertices (in the joint
    table's order, each joint's after the previous joint's) moved by the joint's offset and its parents' (the model's
    joint table: +0x14 u32 joints {u16 parent, s16 x, y, z, u16 vertex count}, +0x18 u16 joint count)."""
    joints_at, n_joints = struct.unpack_from("<IH", war, rec + 0x14)
    where, i = [], 0
    for j in range(n_joints):
        parent, x, y, z, count = struct.unpack_from("<H3hH", war, joints_at + 10 * j)
        base = where[parent] if 0 < j and parent < j else (0, 0, 0)
        where.append((base[0] + x, base[1] + y, base[2] + z))
        for v in verts[i:i + count]:
            v[0] += where[j][0]
            v[1] += where[j][1]
            v[2] += where[j][2]
        i += count


def geometry(war, dav_t, rec, animated):
    """A geometry record as triangles grouped by texture page (-1: untextured): {page: [x, y, z, u, v, r, g, b] * 3
    per triangle}, and its bounds. The level's units (an animated model divided by ANIMATED_SCALE), y down; colours
    0-255 (a textured one: 128 leaves the texture as is)."""
    entries, rects = dav_t[0], dav_t[1]
    vtx_at, ent_at, n_vtx, n_ent = struct.unpack_from("<IIHH", war, rec)
    verts = [list(struct.unpack_from("<3h", war, vtx_at + 8 * i)) for i in range(n_vtx)]
    if animated:
        pose_vertices(war, rec, verts)
        verts = [[round(c / ANIMATED_SCALE, 2) for c in v] for v in verts]
    out = {}
    o = ent_at
    for _ in range(n_ent):
        kind, count = struct.unpack_from("<HH", war, o)
        stride = war_meshes.STRIDE.get(kind)
        if stride is None:
            break
        shape = POLY.get(kind)
        for n in range(count if shape else 0):
            el = o + 4 + n * stride
            corners, cols, tex = shape
            idx = war[el:el + corners]
            colours = [tuple(war[el + c:el + c + 3]) for c in cols]
            colours = colours * corners if len(colours) == 1 else colours
            page, uv = -1, [(0.0, 0.0)] * corners
            if tex is not None:
                tid, = struct.unpack_from("<I", war, el + (4 if tex == "square" else 12))
                if tid < len(entries) and entries[tid] < len(rects):
                    x0, w, y0, h, page = rects[entries[tid]]
                    if tex == "square":
                        c = SQUARE[kind]
                        sq = [(0, 0), (0, c), (c, 0), (c, c)] if kind in SPLIT_A else [(0, 0), (c, 0), (0, c), (c, c)]
                        uv = [((a * (c - 1) / c + 0.5 + x0) / 256, (b * (c - 1) / c + 0.5 + y0) / 256) for a, b in sq]
                    else:
                        t = war[el + 4:el + 4 + 2 * corners]
                        uv = [((t[2 * k] * (w - 1) / max(w, 1) + 0.5 + x0) / 256,
                               (t[2 * k + 1] * (h - 1) / max(h, 1) + 0.5 + y0) / 256) for k in range(corners)]
            tris = [(0, 1, 2)] if corners == TRI else [(0, 3, 2), (0, 1, 3)]
            dest = out.setdefault(page, [])
            for tri in tris:
                if any(idx[k] >= n_vtx for k in tri):
                    continue
                for k in tri:
                    dest += verts[idx[k]] + [round(uv[k][0], 5), round(uv[k][1], 5)] + list(colours[k])
        o += 4 + count * stride
    lo = [min(v[i] for v in verts) for i in range(3)] if verts else [0, 0, 0]
    hi = [max(v[i] for v in verts) for i in range(3)] if verts else [0, 0, 0]
    return {"groups": {str(k): v for k, v in out.items()}, "lo": lo, "hi": hi}


def source_files(source, mod_dir):
    """The .WAR and .DAV bytes of a level (Lvl-03) or of a mod's model file (mod:models\\crate)."""
    if source.startswith("mod:"):
        name = source[4:].replace("\\", "/")
        if ".." in name or not re.match(r"^[A-Za-z0-9_/-]+$", name) or mod_dir is None:
            raise ValueError("a model file is mod:<file>")
        files = {}
        # the mod's folder, then its build (tools/build_mods.py makes the .WAR and .DAV of a mod's .obj models there)
        for base in (mod_dir, ROOT / "work" / "mods" / "Mods" / mod_dir.name):
            folder, stem = (base / name).parent, Path(name).name.lower()
            if folder.is_dir():
                files = {f.suffix.lower(): f for f in folder.iterdir() if f.stem.lower() == stem}
                if ".war" in files and ".dav" in files:
                    break
    else:
        if source not in levels():
            raise ValueError("no such level")
        files = {f.suffix.lower(): f for f in (LEVELS / source).iterdir()}
    if ".war" not in files or ".dav" not in files:
        raise ValueError("%s: no .WAR and .DAV" % source)
    return files[".war"].read_bytes(), files[".dav"].read_bytes()


def model_list(war):
    """The object models of a .WAR (types 0x43 and 0x44): {res: type}."""
    count, = struct.unpack_from("<I", war, 0x0C)
    return {i: e >> 24 for i, e in enumerate(struct.unpack_from("<%dI" % count, war, 0x10)) if e >> 24 in (0x43, 0x44)}


def pages_binary(dav):
    """The texture pages for the page: per page u16 width, height and format (PolyBatcher::LoadTexturePages: bits 0x1c
    the kind, 4 opaque, 8 blended, 0x10 added; bits 3 the pixels, 0 RGB565, 1 ARGB1555, 2 ARGB4444), then its
    pixels."""
    _, _, at, n = dav_tables(dav)
    out = bytearray(struct.pack("<H", n))
    for _ in range(n):
        x0, x1, y0, y1, fmt = struct.unpack_from("<5H", dav, at)
        w, h = abs(x0 - x1), abs(y0 - y1)
        out += struct.pack("<3H", w, h, fmt) + dav[at + 10:at + 10 + 2 * w * h]
        at += 10 + 2 * w * h
    return bytes(out)


def scenery(level):
    """The level's own geometry (its placed records: types 3, 0xb and 0x26 without bit 0x40), in world space: {"main",
    "sky"} (0x26 is kept apart, what it is is not established)."""
    war, dav = source_files(level, None)
    dav_t = dav_tables(dav)
    count, = struct.unpack_from("<I", war, 0x0C)
    out = {"main": {}, "sky": {}}
    for e in struct.unpack_from("<%dI" % count, war, 0x10):
        t = e >> 24
        if t not in (3, 0x0B, 0x26):
            continue
        g = geometry(war, dav_t, e & 0xFFFFFF, False)
        layer = out["sky" if t == 0x26 else "main"]
        for page, tris in g["groups"].items():
            layer.setdefault(page, []).extend(tris)
    return out


# ---- patches: the editor's state from a patch, and a patch from the editor's state ----
# An object of the state: {"id", "res" (its resource here, or None for a new one), "source" (None, {"res"} or
# {"level", "res"}), "name" (a new one's), "class", "model", "pos", "rot", "props" {"0x<offset>": value},
# "removed"}. "base" is the record the object starts from as the loader applies the patch: the level's own, or for a
# new one the untouched record it is copied from. write_patch writes the new objects before any change to the
# level's own, so each copy is made from the untouched record, and then writes what differs from "base".

def number(s):
    return int(s, 0)


def object_from(level_objects, res):
    for o in level_objects:
        if o["res"] == res:
            return o
    return None


def values(o):
    return {"class": o["class"], "model": o["model"], "pos": list(o["pos"]), "rot": list(o["rot"]),
            "props": dict(o["props"])}


def fresh(now, base, **extra):
    s = values(now)
    s.update(extra)
    s["base"] = values(base)
    return s


def parse_patch(text, level, objects_of, sch):
    """The editor's state for `level` with patch `text` applied, and the lines it keeps as they are. objects_of(level)
    gives a level's objects (for `new ... from <Level> <res>`)."""
    mine = {o["res"]: o for o in objects_of(level)}
    state = [fresh(o, o, id="r%d" % o["res"], res=o["res"], source=None, name=None, removed=False)
             for o in mine.values()]
    by_res = {s["res"]: s for s in state}
    names, kept = {}, []
    for raw in text.splitlines():
        line = raw.split("#")[0].strip()
        w = line.split()
        if not w:
            if raw.strip():
                kept.append(raw)
            continue
        try:
            if w[0] == "new" and len(w) in (4, 5) and w[2] == "from" and w[1] not in names:
                if len(w) == 5:  # from another level: its untouched record
                    now = base = object_from(objects_of(w[3]), number(w[4]))
                    source = {"level": w[3], "res": number(w[4])}
                elif w[3] in names:  # a copy of a new one: what it is now, from the same record
                    now, source = names[w[3]], names[w[3]]["source"]
                    base = names[w[3]]["base"]
                else:  # a copy of one of this level's: what it is now (the loader copies it as it is)
                    now, source = by_res[number(w[3])], {"res": number(w[3])}
                    base = mine[number(w[3])]
                    if now["removed"]:
                        raise ValueError
                if now is None:
                    raise ValueError
                s = fresh(now, base, id="n%d" % len(names), res=None, source=source, name=w[1], removed=False)
                names[w[1]] = s
                state.append(s)
                continue
            target = names.get(w[0]) or by_res.get(number(w[0]) if re.match(r"^-?(0x)?[0-9a-fA-F]+$", w[0]) else -1)
            if target is None or len(w) < 2:
                raise ValueError
            op, args = w[1], w[2:]
            if op in ("pos", "move", "rot") and len(args) == 3:
                v = [number(a) for a in args]
                if op == "move":
                    v = [a + b for a, b in zip(target["pos"], v)]
                target["rot" if op == "rot" else "pos"] = v
            elif op == "model" and len(args) == 1:
                target["model"] = number(args[0]) if re.match(r"^(0x)?[0-9a-fA-F]+$", args[0]) else args[0]
            elif op == "class" and len(args) == 1:
                target["class"] = number(args[0])
            elif op == "prop" and len(args) == 2:
                off = prop_offset(sch, target["class"], args[0])
                if off is None:
                    raise ValueError
                target["props"]["0x%x" % off] = number(args[1])
            elif op == "remove" and not args:
                target["removed"] = True
            else:
                raise ValueError
        except (ValueError, KeyError, IndexError):
            kept.append(raw)
    return state, kept


# ---- signs' text: a sign shows string INDEXTEXT (TEXTNUM on SignPost) of its class's list in the level's .MLT ----

TEXT_LINE = re.compile(r'^\s*text\s+(\d+)\s+(\d+)\s+"((?:[^"\\]|\\.)*)"\s*(#.*)?$')


def sign_props(sch):
    """{class id: the offset of the property that picks its string}: the classes with an INDEXTEXT or TEXTNUM."""
    return {cid: off for cid, v in sch.items() for off, name in v["props"] if name in ("INDEXTEXT", "TEXTNUM")}


def read_mlt(level):
    """The level's text (.MLT: "v1.2", u16 languages at +8, u16 lists at +10, from +16 per language per list a u8
    count and that many zero-terminated Latin-1 strings) in English: its lists of strings."""
    files = [f for f in (LEVELS / level).iterdir() if f.suffix.lower() == ".mlt"]
    if not files:
        return []
    d = files[0].read_bytes()
    if d[4:8] != b"v1.2":
        return []
    blocks, nlists = struct.unpack_from("<HH", d, 8)
    o, langs = 16, []
    for _ in range(blocks):
        lists = []
        for _ in range(nlists):
            n, o = d[o], o + 1
            strings = []
            for _ in range(n):
                z = d.index(b"\0", o)
                strings.append(d[o:z].decode("latin-1"))
                o = z + 1
            lists.append(strings)
        langs.append(lists)
    english = [b for b in langs if len(b[0]) > 1 and b[0][1].lower() == "resume"]
    return english[0] if english else langs[min(1, len(langs) - 1)] if langs else []


def sign_texts(level, sch):
    """{class id: its strings} for the sign classes, in English."""
    lists = read_mlt(level)
    return {cid: (lists[cid + 1] if cid + 1 < len(lists) else []) for cid in sign_props(sch)}


def split_texts(kept):
    """The `text <class> <n> "..."` lines out of the kept ones: ({class: {n: string}}, the other kept lines)."""
    texts, rest = {}, []
    for line in kept:
        m = TEXT_LINE.match(line)
        if not m:
            rest.append(line)
            continue
        s = re.sub(r"\\(.)", lambda e: "\n" if e.group(1) == "n" else e.group(1), m.group(3))
        texts.setdefault(int(m.group(1)), {})[int(m.group(2))] = s
    return texts, rest


def text_lines(texts):
    out = []
    for cls in sorted(texts, key=int):
        for n in sorted(texts[cls], key=int):
            s = texts[cls][n].replace("\\", "\\\\").replace('"', '\\"').replace("\n", "\\n")
            out.append('text %d %d "%s"' % (int(cls), int(n), s))
    return out


def prop_offset(sch, cls, name):
    if re.match(r"^(0x)?[0-9a-fA-F]+$", name) and name[:1].isdigit():
        return int(name, 0)
    for off, n in sch.get(cls, {}).get("props", []):
        if n == name:
            return off
    return None


def prop_name(sch, cls, off):
    for o, n in sch.get(cls, {}).get("props", []):
        if o == off:
            return n
    return "0x%x" % off


def write_patch(state, kept, level, sch, texts=None):
    """The patch text for the editor's state: the kept lines first, the signs' text, then per object only what differs
    from its base."""
    lines = ["# %s (%s): made with tools/level_editor.py" % (level, LEVEL_NAMES.get(level, level))]
    lines += [k for k in kept if not k.startswith("# %s (" % level)]
    lines += text_lines(texts or {})
    used = set()
    changes = []
    for s in [s for s in state if s.get("res") is None] + [s for s in state if s.get("res") is not None]:
        new = s.get("res") is None
        if s.get("removed") and new:
            continue
        if new:
            base_name = re.sub(r"[^A-Za-z0-9_]", "_", s.get("name") or "obj")[:24] or "obj"
            if base_name[0].isdigit():
                base_name = "o" + base_name
            name, k = base_name, 2
            while name in used:
                name, k = "%s%d" % (base_name, k), k + 1
            used.add(name)
            s["name"] = name
            src = s["source"]
            lines.append("new %s from %s" % (name, ("%s %d" % (src["level"], src["res"])) if "level" in src else src["res"]))
            who, out = name, lines
        else:
            who, out = str(s["res"]), changes
            if s.get("removed"):
                out.append("%s remove" % who)
                continue
        b = s["base"]
        if s["pos"] != b["pos"]:
            out.append("%s pos %d %d %d" % (who, *s["pos"]))
        if s["rot"] != b["rot"]:
            out.append("%s rot %d %d %d" % (who, *s["rot"]))
        if s["model"] != b["model"]:
            out.append("%s model %s" % (who, s["model"]))
        if s["class"] != b["class"]:
            out.append("%s class %d" % (who, s["class"]))
        for off, v in sorted(s["props"].items(), key=lambda kv: int(kv[0], 16)):
            if b["props"].get(off) != v:
                out.append("%s prop %s %d" % (who, prop_name(sch, s["class"], int(off, 16)), v))
    lines += changes
    return "\n".join(lines) + "\n"


# ---- the server ----

SETTINGS = {"left": ("rotate", "pan"), "middle": ("rotate", "pan"), "right": ("rotate", "pan"), "step": (1, 100000)}


def settings_path():
    """The editor's own settings (mouse buttons, wheel step), kept on this computer: Application Support on a Mac,
    %APPDATA% on Windows, ~/.config elsewhere."""
    if sys.platform == "darwin":
        return Path.home() / "Library" / "Application Support" / "SDW Level Editor" / "settings.json"
    if sys.platform == "win32" and os.environ.get("APPDATA"):
        return Path(os.environ["APPDATA"]) / "SDW Level Editor" / "settings.json"
    return Path.home() / ".config" / "sdw-level-editor" / "settings.json"


def clean_settings(values):
    """The settings in `values` that are known and valid."""
    out = {}
    for key, allowed in SETTINGS.items():
        v = values.get(key) if isinstance(values, dict) else None
        if isinstance(allowed[0], str) and v in allowed:
            out[key] = v
        elif isinstance(allowed[0], int) and isinstance(v, int) and allowed[0] <= v <= allowed[1]:
            out[key] = v
    return out

NO_WRITE = ("The editor may not write in %s, so it cannot save there. On Windows a game under Program Files needs "
            "administrator rights: close this page, then right-click SDW Level Editor.bat and choose Run as "
            "administrator. Or install the game outside Program Files.")


class Editor:
    def __init__(self, mods_dir, level=None, mod=None):
        self.mods_dir, self.sch = Path(mods_dir), schema()
        self.level = self.patch_path = self.mod_dir = None
        self.cache, self.files = {}, {}
        if level:
            self.open(level, mod)

    def open(self, level, mod):
        """Edit `level` (its folder name) of the mod folder `mod` (made when the patch is first saved)."""
        if level not in levels():
            raise ValueError("no level %s" % level)
        if not LEVEL_NAME.match(mod or "") or mod.lower() == "cache":
            raise ValueError("a mod's name is letters, digits, - and _ (and not cache)")
        self.level, self.mod_dir = level, self.mods_dir / mod
        self.patch_path = self.mod_dir / "levels" / (level + ".txt")
        self.files = {k: v for k, v in self.files.items() if not k.startswith("mod:")}

    def choices(self):
        mods = sorted(d.name for d in self.mods_dir.iterdir() if d.is_dir() and d.name.lower() != "cache"
                      and LEVEL_NAME.match(d.name)) if self.mods_dir.is_dir() else []
        return {"levels": [[l, LEVEL_NAMES.get(l, l)] for l in levels()], "mods": mods, "modsDir": str(self.mods_dir),
                "writable": writable(self.mods_dir), "level": self.level, "mod": self.mod_dir.name if self.mod_dir else None}

    def objects_of(self, level):
        if level not in self.cache:
            self.cache[level] = read_objects(level, self.sch)
        return self.cache[level]

    def source(self, name):
        if name not in self.files:
            self.files[name] = source_files(name, self.mod_dir)
        return self.files[name]

    def classes(self):
        return {str(k): {"name": v["name"], "props": [["0x%x" % o, n] for o, n in v["props"]]}
                for k, v in self.sch.items()}

    def model(self, name, res):
        war, dav = self.source(name)
        kinds = model_list(war)
        if res not in kinds:
            raise ValueError("%s %d is not an object model" % (name, res))
        e, = struct.unpack_from("<I", war, 0x10 + 4 * res)
        return geometry(war, dav_tables(dav), e & 0xFFFFFF, kinds[res] == 0x44)

    def start(self):
        if not self.level:
            return {"choose": True}
        text = self.patch_path.read_text() if self.patch_path.exists() else ""
        state, kept = parse_patch(text, self.level, self.objects_of, self.sch)
        texts, kept = split_texts(kept)
        flat, kind = read_ground(self.level)
        users = {}
        for o in self.objects_of(self.level):
            users.setdefault(o["model"], set()).add(self.sch.get(o["class"], {}).get("name", "?"))
        models = {str(r): {"animated": k == 0x44, "users": sorted(users.get(r, ()))}
                  for r, k in model_list(self.source(self.level)[0]).items()}
        return {"level": self.level, "title": LEVEL_NAMES.get(self.level, self.level), "patch": str(self.patch_path),
                "mod": self.mod_dir.name, "levels": [[l, LEVEL_NAMES.get(l, l)] for l in levels()],
                "classes": self.classes(), "models": models, "state": state, "kept": kept, "ground": flat,
                "texts": texts, "signs": {str(c): "0x%x" % o for c, o in sign_props(self.sch).items()},
                "signText": {str(c): v for c, v in sign_texts(self.level, self.sch).items()},
                "kind": kind}


def serve(editor, port, open_browser=True, app=False):
    """Serve the editor on 127.0.0.1. With `app`, the server stops once its page has been closed for 40 s (the page
    pings every 5 s), or after 2 minutes if no page ever opens."""
    seen = {"first": None, "last": time.time()}

    class Handler(BaseHTTPRequestHandler):
        def log_message(self, *a):
            pass

        def reply(self, body, kind="application/json", code=200):
            data = body if isinstance(body, bytes) else body.encode()
            self.send_response(code)
            self.send_header("Content-Type", kind)
            self.send_header("Content-Length", str(len(data)))
            self.send_header("Cache-Control", "no-store")
            self.end_headers()
            self.wfile.write(data)

        def do_GET(self):
            u = urlparse(self.path)
            q = {k: v[0] for k, v in parse_qs(u.query).items()}
            try:
                if u.path == "/":
                    return self.reply(PAGE.read_bytes(), "text/html; charset=utf-8")
                if u.path == "/api/ping":
                    seen["last"] = time.time()
                    seen["first"] = seen["first"] or seen["last"]
                    return self.reply("{}")
                if u.path == "/api/settings":
                    try:
                        return self.reply(json.dumps(clean_settings(json.loads(settings_path().read_text()))))
                    except (OSError, ValueError):
                        return self.reply("{}")
                if u.path == "/api/choices":
                    return self.reply(json.dumps(editor.choices()))
                if u.path == "/api/start":
                    return self.reply(json.dumps(editor.start()))
                if u.path == "/api/scenery":
                    return self.reply(json.dumps(scenery(editor.level)))
                if u.path == "/api/pages":
                    return self.reply(pages_binary(editor.source(q.get("source", ""))[1]), "application/octet-stream")
                if u.path == "/api/model":
                    return self.reply(json.dumps(editor.model(q.get("source", ""), int(q.get("res", "-1")))))
                if u.path == "/api/signtext":
                    level = q.get("level", "")
                    if level not in levels():
                        return self.reply('{"error": "no such level"}', code=404)
                    return self.reply(json.dumps({str(c): v for c, v in sign_texts(level, editor.sch).items()}))
                if u.path == "/api/objects":
                    level = q.get("level", "")
                    if not LEVEL_NAME.match(level) or level not in levels():
                        return self.reply('{"error": "no such level"}', code=404)
                    return self.reply(json.dumps(editor.objects_of(level)))
            except (ValueError, OSError, struct.error) as e:
                return self.reply(json.dumps({"error": str(e)}), code=400)
            self.reply('{"error": "not found"}', code=404)

        def do_POST(self):
            path = urlparse(self.path).path
            if path not in ("/api/save", "/api/open", "/api/settings"):
                return self.reply('{"error": "not found"}', code=404)
            try:
                body = json.loads(self.rfile.read(int(self.headers.get("Content-Length", "0"))))
                if path == "/api/settings":
                    settings_path().parent.mkdir(parents=True, exist_ok=True)
                    settings_path().write_text(json.dumps(clean_settings(body), indent=1) + "\n")
                    return self.reply("{}")
                if path == "/api/open":
                    editor.open(body.get("level", ""), body.get("mod", ""))
                    return self.reply(json.dumps({"patch": str(editor.patch_path)}))
                if not editor.level:
                    raise ValueError("no level is open")
                text = write_patch(body["state"], body.get("kept", []), editor.level, editor.sch, body.get("texts"))
                editor.patch_path.parent.mkdir(parents=True, exist_ok=True)
                editor.patch_path.write_text(text)
                self.reply(json.dumps({"saved": str(editor.patch_path), "text": text}))
            except PermissionError:
                self.reply(json.dumps({"error": NO_WRITE % editor.mods_dir}), code=500)
            except Exception as e:  # noqa: BLE001
                self.reply(json.dumps({"error": str(e)}), code=500)

    try:
        server = ThreadingHTTPServer(("127.0.0.1", port), Handler)
    except OSError:
        if not app:
            raise
        webbrowser.open("http://127.0.0.1:%d/" % port)  # already running: show it again
        return
    url = "http://127.0.0.1:%d/" % server.server_address[1]
    print("the level editor: %s   (Ctrl+C to stop)" % url)
    print("it saves into %s" % editor.mods_dir)
    if open_browser:
        threading.Timer(0.5, lambda: webbrowser.open(url)).start()
    if app:
        def watch():
            while True:
                time.sleep(5)
                now = time.time()
                if (seen["first"] and now - seen["last"] > 40) or (not seen["first"] and now - seen["last"] > 120):
                    server.shutdown()
                    return
        threading.Thread(target=watch, daemon=True).start()
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        pass


# ---- its own test ----

def check():
    """Reading and writing patches: every example's patch read, written and read again gives the same objects."""
    sch = schema()
    cache = {}

    def objects_of(level):
        if level not in cache:
            cache[level] = read_objects(level, sch)
        return cache[level]

    fails = 0
    for p in sorted((ROOT / "mods" / "examples").glob("*/levels/*.txt")):
        level = p.stem
        text = p.read_text()
        s1, kept = parse_patch(text, level, objects_of, sch)
        out = write_patch(s1, kept, level, sch)
        s2, kept2 = parse_patch(out, level, objects_of, sch)
        strip = lambda st: [{k: v for k, v in s.items() if k not in ("base", "id", "name")} for s in st]  # noqa: E731
        ok = strip(s1) == strip(s2)
        print("%s  %s: read, written and read again (%d kept line(s))" % ("PASS" if ok else "FAIL",
                                                                          p.relative_to(ROOT), len(kept)))
        fails += not ok
    s, _ = parse_patch("60 move 0 -100 0\nnew a from 114\na pos 1 2 3\n114 remove\n", "Lvl-03", objects_of, sch)
    out = write_patch(s, [], "Lvl-03", sch)
    want = ["60 pos", "114 remove", "new a from 114", "a pos 1 2 3"]
    ok = all(any(l.startswith(w) for l in out.splitlines()) for w in want)
    print("%s  a patch's changes written back: %s" % ("PASS" if ok else "FAIL", " | ".join(out.splitlines()[1:])))
    fails += not ok
    # a copy made after its source was moved: the copy is written first, from the untouched record, with the move
    s, _ = parse_patch("114 pos 5 6 7\nnew b from 114\n", "Lvl-03", objects_of, sch)
    out = write_patch(s, [], "Lvl-03", sch).splitlines()[1:]
    ok = out == ["new b from 114", "b pos 5 6 7", "114 pos 5 6 7"]
    print("%s  a copy of a changed object: %s" % ("PASS" if ok else "FAIL", " | ".join(out)))
    fails += not ok
    # signs' text: read, written and read again
    line = 'text 78 5 "Hé \\"you\\"\\nline two"'
    texts, rest = split_texts(["# a comment", line])
    ok = texts == {78: {5: 'Hé "you"\nline two'}} and rest == ["# a comment"] and text_lines(texts) == [line]
    print("%s  a sign's text line read and written again: %s" % ("PASS" if ok else "FAIL", text_lines(texts)))
    fails += not ok
    ok = sign_texts("Lvl-03", sch).get(78, ["", ""])[1].startswith("Get the elastic")
    print("%s  Level 3's signs in English: %r" % ("PASS" if ok else "FAIL", sign_texts("Lvl-03", sch).get(78, [])[1:2]))
    fails += not ok
    print("all passed" if not fails else "%d failed" % fails)
    return fails == 0


def main(argv):
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ap.add_argument("level", nargs="?", help='the level: its folder (Lvl-03) or its number as players count ("Level 3", B1)')
    ap.add_argument("--mod", help="the mod whose level patch it edits (a folder name)")
    ap.add_argument("--mods", help="the folder of mods (default: the installed game's Mods, else mods/examples)")
    ap.add_argument("--port", type=int, default=0, help="the local port (default: any free one; 8766 with --app)")
    ap.add_argument("--app", action="store_true", help="as the launchers start it: port 8766, one at a time, and it "
                                                      "stops when its page has been closed")
    ap.add_argument("--no-browser", action="store_true", help="do not open the browser")
    ap.add_argument("--check", action="store_true", help="test reading and writing patches, then stop")
    a = ap.parse_args(argv)
    if a.check:
        return 0 if check() else 1
    if a.app:  # started from a launcher, with no terminal: what it prints goes to work/level_editor.log
        (ROOT / "work").mkdir(exist_ok=True)
        sys.stdout = sys.stderr = open(ROOT / "work" / "level_editor.log", "w", buffering=1)
    games = game_folders()
    if not LEVELS.is_dir() and games and (games[0] / "Levels").is_dir():
        use_levels(games[0] / "Levels")
    make_names(games)
    mods = Path(a.mods) if a.mods else games[0] / "Mods" if games else ROOT / "mods" / "examples"
    level = level_folder(a.level) if a.level else None
    if a.level and level is None:
        ap.error("no level %s under %s" % (a.level, LEVELS))
    if level and not a.mod:
        ap.error("with a level, give --mod <Name>")
    try:
        editor = Editor(mods, level, a.mod)
    except ValueError as e:
        ap.error(str(e))
    serve(editor, a.port or (8766 if a.app else 0), not a.no_browser, a.app)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
