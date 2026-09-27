#!/usr/bin/env python3
"""Generate C headers from the recovered struct and enum tables.

Input : data/structs/<Struct>.csv (offset, ctype, name, ...), data/enums/<Enum>.csv, data/class_map.csv (object sizes)
        --overlay DIR --out DIR: an alternative set of tables - CSVs in DIR replace/add to data/structs, headers go to
        --out (compile against them with tools/vc6.py --include; the shared CSVs and headers stay untouched)
Output: src/include/sdw_types.h     basic typedefs (s8..u32, s64/u64, Vec3s, Box)
        src/include/sdw_enums.h     every recovered enum
        src/include/sdw_structs.h   one struct per CSV, fields at their exact offsets with explicit padding, sized to the class's
                                    object size when known; compiles with `cc -fsyntax-only` and carries _Static_assert size checks

Layout is FLAT per class (each struct repeats the base-class fields it was observed to use at their absolute offsets) - that is what
Ghidra needs to type `this`. Nested base structs could replace it once the base-class sizes are settled.
Overlapping or misaligned claims are reported and the later field is emitted as a comment rather than breaking the layout.
"""
import textio  # noqa: F401  (first: UTF-8, LF text files on Windows)
import argparse
import csv
import re
from pathlib import Path
import os
import sys


def fnptr_typedefs():
    """data/fnptr_typedefs.csv: named function-pointer types, as (name, C typedef lines). Each is 4 bytes wide.
    The generated headers emit them after the forward declarations, so struct parameters can be named."""
    p = ROOT / "data" / "fnptr_typedefs.csv"
    if not p.exists():
        return []
    out = []
    for r in csv.DictReader(p.open()):
        lines = ["struct %s;" % f for f in r["forward"].split()]
        lines.append("typedef %s (SDW_CDECL *%s)(%s);   // %s" % (r["return"], r["name"], r["params"], r["meaning"]))
        out.append((r["name"], lines))
    return out


def write_atomic(path, text):
    """Write a generated file so a concurrent reader sees either the old or the new version, never half of one.

    A compile can run against src/include while it is regenerated; a plain write_text() truncates first, and a
    compile that opened the header in that window would see an empty or cut file. An unchanged file is not rewritten
    at all, so its mtime and contents stay put."""
    path = Path(path)
    if path.exists() and path.read_bytes() == text.encode("utf-8"):
        return
    tmp = path.with_name(path.name + ".tmp%d" % os.getpid())
    tmp.write_text(text)
    os.replace(str(tmp), str(path))

ROOT = Path(__file__).resolve().parent.parent
INC = ROOT / "src" / "include"
OVERLAY = None      # --overlay DIR: CSVs there replace (or add to) data/structs/*.csv - an alternative set of tables


def struct_paths():
    """data/structs/*.csv, with any same-named file in the overlay taking its place and new overlay files added."""
    paths = {q.stem: q for q in (ROOT / "data" / "structs").glob("*.csv")}
    if OVERLAY:
        for q in Path(OVERLAY).glob("*.csv"):
            if q.name != "vtable_slots.csv":
                paths[q.stem] = q
    return [paths[k] for k in sorted(paths)]
SIZES = {"s8": 1, "u8": 1, "char": 1, "bool8": 1, "s16": 2, "u16": 2, "s32": 4, "u32": 4, "int": 4, "float": 4, "fnptr": 4,
         "s64": 8, "u64": 8, "double": 8, "Vec3s": 6, "Box": 16}
SIZES.update({n: 4 for n, _ in fnptr_typedefs()})   # named function-pointer types are 4-byte scalars


def parse_type(ctype):
    """'s16[3][3]' -> ('s16', [3, 3]);  'Wolf *' -> ('Wolf *', [])."""
    t = ctype.strip()
    dims = []
    while True:
        m = re.fullmatch(r"(.+?)\s*\[(\d+|0x[0-9a-fA-F]+)\]", t)
        if not m:
            break
        dims.insert(0, int(m.group(2), 0))
        t = m.group(1).strip()
    return t, dims


# The base classes that are not registered scenaric classes, by vtable, and their chains (base first). Shared with cpp_classes.py.
VT_STRUCT = {"0x574c40": "ScnObject", "0x574c1c": "ScnBody", "0x574c84": "ScnMobile", "0x574fd0": "ScnControllable", "0x57595c": "ScnLogic",
             "0x576934": "ScnLogicShadowed", "0x5766b8": "Mine", "0x5760b8": "FishingRod", "0x576110": "CompositeRod",
             "0x57441c": "Sound", "0x576e7c": "TrainCarBody",
             # Mesh is AnimMesh's base; PolyTri is the base of the six Bs polygon classes
             "0x574370": "Mesh", "0x5743f4": "PolyTri",
             "0x5743e4": "BsPolyFlat", "0x5743ec": "BsPolyGouraud", "0x5743e8": "BsPolyTexFlat", "0x5743f0": "BsPolyTexGouraud",
             "0x5743dc": "BsPolyBlendFlat", "0x5743e0": "BsPolyBlendGouraud",
             "0x5742d0": "InputDevice", "0x574308": "Joystick", "0x574314": "Keyboard", "0x574334": "Mouse"}
BASE_CHAIN = {"ScnObject": [], "ScnBody": ["ScnObject"], "ScnLogic": ["ScnObject"], "ScnMobile": ["ScnObject", "ScnBody"],
              "ScnControllable": ["ScnObject", "ScnBody", "ScnMobile"], "ScnLogicShadowed": ["ScnObject", "ScnLogic"],
              "Mine": ["ScnObject", "ScnBody", "ScnMobile"], "FishingRod": ["ScnObject", "ScnBody", "ScnMobile"],
              "CompositeRod": ["ScnObject", "ScnBody", "ScnMobile", "FishingRod"],
              "Sound": [], "StaticSound": ["Sound"], "StreamSound": ["Sound"],
              "InputDevice": [], "Joystick": ["InputDevice"], "Keyboard": ["InputDevice"], "Mouse": ["InputDevice"],
              "TrainCarBody": ["ScnObject", "ScnBody"],
              "Mesh": [], "AnimMesh": ["Mesh"], "PolyTri": [],
              "BsPolyFlat": ["PolyTri"], "BsPolyGouraud": ["PolyTri"], "BsPolyTexFlat": ["PolyTri"], "BsPolyTexGouraud": ["PolyTri"],
              "BsPolyBlendFlat": ["PolyTri"], "BsPolyBlendGouraud": ["PolyTri"]}   # Train's car sub-object: ScnBody + HandleMessage 0x501110

# Sizes of the non-class structs (e.g. Shadow 0x18), computed from their CSVs in main() before anything is emitted, so a field typed
# as one of them gets its real size instead of the gap to the next field (a struct's LAST field has no next field to measure).
STRUCT_SIZE = {}
# A bitfield row: ctype "u8:1", "u16:3" (the base type and the bit width). Consecutive rows at the SAME offset share one
# storage unit of the base type, first row = lowest bits (VC6's order); the offset checks skip them (no address).
BITF = re.compile(r"(\w+)\s*:\s*(\d+)")


def type_size(ctype):
    """Size in bytes, or None when the element type is unknown (then the field is emitted as opaque bytes)."""
    bf = BITF.fullmatch(ctype.strip())
    if bf:
        return SIZES.get(bf.group(1))
    base, dims = parse_type(ctype)
    size = 4 if base.endswith("*") else SIZES.get(base, STRUCT_SIZE.get(base))
    if size is None:
        return None
    for d in dims:
        size *= d
    return size


def field_layout(ctype, name, gap):
    """ONE source of truth for a field: returns (C declaration, size in bytes) - they can never disagree.
    Unknown element types (e.g. 'AltModel[9]') become opaque bytes spanning the gap to the next field, so the layout cannot shift."""
    bf = BITF.fullmatch(ctype.strip())
    if bf and bf.group(1) in SIZES:
        return "%s %s : %s" % (bf.group(1), name, bf.group(2)), SIZES[bf.group(1)], False
    base, dims = parse_type(ctype)
    size = type_size(ctype)
    if size is None:
        return "u8 %s[0x%x]" % (name, gap), gap, True
    if base in STRUCT_SIZE and base not in SIZES:  # a table struct of known size: opaque bytes of its real size (no declaration-order issue)
        return "u8 %s[0x%x]" % (name, size), size, True
    return "%s %s%s" % (norm_type(base), name, "".join("[%d]" % d for d in dims)), size, False


def c_decl(ctype, name):
    return field_layout(ctype, name, 4)[0]


def norm_type(t):
    t = t.strip()
    if t.endswith("*"):
        base = t[:-1].strip()
        return ("struct %s *" % base) if re.fullmatch(r"[A-Z]\w*", base) and base not in ("Vec3s", "Box") else (base + " *" if base in SIZES or base == "void" else "void *")
    return t if t in SIZES else "u32"


def ident(s):
    s = re.sub(r"\W", "_", s.strip())
    return ("_" + s) if not s or s[0].isdigit() else s


def main(argv=None):
    global INC, OVERLAY
    ap = argparse.ArgumentParser(description="generate the headers in src/include from the data CSVs")
    ap.add_argument("--overlay", help="directory of struct CSVs (and vtable_slots.csv) that replace or add to the shared ones")
    ap.add_argument("--out", help="write the headers here instead of src/include (and skip the shared work/ outputs)")
    a = ap.parse_args(argv)
    OVERLAY = a.overlay
    private = a.out is not None
    if private:
        INC = Path(a.out).resolve()
    INC.mkdir(parents=True, exist_ok=True)
    sizes = {}
    for r in csv.DictReader((ROOT / "data" / "class_map.csv").open()):
        if r["obj_size"]:
            sizes[ident(r["name"].replace(" ", ""))] = int(r["obj_size"], 16)

    write_atomic(INC / "sdw_types.h", """// GENERATED by tools/structs_to_c.py - basic types of the decompilation.
#ifndef SDW_TYPES_H
#define SDW_TYPES_H
typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;
#ifdef _MSC_VER                                                    /* VC6 has no long long */
typedef __int64 s64;
typedef unsigned __int64 u64;
#else
typedef long long s64;
typedef unsigned long long u64;
#endif
typedef void (*fnptr)(void);
#ifdef _MSC_VER
#define SDW_CDECL __cdecl
#else
#define SDW_CDECL
#endif
typedef struct Vec3s { s16 x, y, z; } Vec3s;                 // vertical axis points DOWN
typedef struct Box { u32 flags; s16 min[3]; s16 max[3];          // zone / trigger / model box as stored in WAR files
#if defined(__cplusplus) && defined(SDW_MEMBERS_Box)
    SDW_MEMBERS_Box                                                 // member functions only (Box_ContainsPointXZ 0x4486c0 is a thiscall)
#endif
} Box;
#endif
""")

    out = ["// GENERATED by tools/structs_to_c.py from data/enums/*.csv - do not edit by hand.", "#ifndef SDW_ENUMS_H", "#define SDW_ENUMS_H", ""]
    nenum = 0
    all_names = {}  # C has ONE namespace for enumerators: a name may be defined once across all enums
    # ScnMsg first: a message spelling shared by several <Class>Msg enums is then owned by the generic enum, and the
    # per-class rows print as its documentation ("already defined in enum ScnMsg")
    for path in sorted((ROOT / "data" / "enums").glob("*.csv"), key=lambda q: (q.stem != "ScnMsg", q.name)):
        rows = list(csv.reader(path.open()))[1:]
        if not rows:
            continue
        name = ident(path.stem)
        desc = rows[0][2] if rows[0][0] == "#" else ""
        out.append("// %s" % desc)
        out.append("enum %s {" % name)
        seen = set()
        for val, vname, meaning in (r[:3] for r in rows if r[0] != "#"):
            vn = ident(vname)
            if vn in seen or not re.fullmatch(r"-?(0x[0-9a-fA-F]+|\d+)", val.strip()):
                out.append("    // %s = %s  %s" % (vn, val, meaning))
                continue
            if vn in all_names:
                prev_enum, prev_val = all_names[vn]
                if int(prev_val, 0) == int(val.strip(), 0):
                    out.append("    // %s = %s  (already defined in enum %s)  %s" % (vn, val.strip(), prev_enum, meaning))
                    continue
                vn = "%s_%s" % (name, vn)  # same name, different value: qualify it
            seen.add(vn)
            all_names[vn] = (name, val.strip())
            out.append("    %s = %s,%s" % (vn, val.strip(), ("  // " + meaning) if meaning else ""))
        out += ["};", ""]
        nenum += 1
    out.append("#endif")
    write_atomic(INC / "sdw_enums.h", "\n".join(out) + "\n")

    out = ["// GENERATED by tools/structs_to_c.py from data/structs/*.csv - do not edit by hand.",
           "// Flat per-class layouts: every field sits at its verified absolute offset; _padNN are bytes not yet understood.",
           "#ifndef SDW_STRUCTS_H", "#define SDW_STRUCTS_H", '#include "sdw_types.h"',
           "// Pointers are 4 bytes in the game; the size checks only make sense on a 32-bit target (MSVC6 / -m32).",
           "#if defined(_M_IX86) || defined(__i386__)", "#define SDW_SIZE_CHECK(S, N) typedef char S##_size_check[(sizeof(struct S) == (N)) ? 1 : -1]",
           "#else", "#define SDW_SIZE_CHECK(S, N)", "#endif",
           "#pragma pack(push, 1)  // every field is at an explicit byte offset; never let the compiler realign", ""]
    # Vec3s and Box are defined in sdw_types.h; a CSV of the same name documents them but must not redefine them
    paths = [q for q in struct_paths() if ident(q.stem) not in ("Vec3s", "Box")]
    for path in paths:
        out.append("struct %s;" % ident(path.stem))
    out.append("")
    for _name, lines in fnptr_typedefs():          # named function-pointer types (data/fnptr_typedefs.csv)
        out.extend(lines)
    out.append("")
    # Flatten inheritance: a class struct also gets the fields recovered for its base classes, wherever it has none of its own.
    chain = {k: list(v) for k, v in BASE_CHAIN.items()}
    for r in csv.DictReader((ROOT / "data" / "class_map.csv").open()):
        chain[ident(r["name"].replace(" ", ""))] = [VT_STRUCT[b] for b in r["base_vtables(base->derived)"].split() if b in VT_STRUCT]
    tables = {ident(q.stem): list(csv.DictReader(q.open())) for q in paths}

    def struct_size(sname, seen=()):
        """A class's object size from class_map, else the end of the struct's last field; None if that last field's size is unknown."""
        if sname in sizes:
            return sizes[sname]
        rows = tables.get(sname)
        if not rows or sname in seen:
            return None
        last = max(rows, key=lambda r: int(r["offset"], 16))
        bf = BITF.fullmatch(last["ctype"].strip())
        if bf:
            return int(last["offset"], 16) + SIZES.get(bf.group(1), 0) if bf.group(1) in SIZES else None
        base, dims = parse_type(last["ctype"])
        if base.endswith("*"):
            one = 4
        elif base in SIZES:
            one = SIZES[base]
        else:
            one = struct_size(base, seen + (sname,))
        if one is None:
            return None
        for d in dims:
            one *= d
        return int(last["offset"], 16) + one
    for s in tables:
        if s not in sizes:
            n = struct_size(s)
            if n:
                STRUCT_SIZE[s] = n

    def flattened(sname):
        rows = [dict(r) for r in tables[sname]]
        spans = [(int(r["offset"], 16), int(r["offset"], 16) + (type_size(r["ctype"]) or 4)) for r in rows]  # unknown types: >= 4
        for base in reversed(chain.get(sname, [])):  # nearest base first
            for r in tables.get(base, []):
                lo = int(r["offset"], 16)
                hi = lo + (type_size(r["ctype"]) or 4)
                if all(hi <= a or lo >= b for a, b in spans):
                    r = dict(r)
                    r["meaning"] = "[%s] %s" % (base, r["meaning"])
                    rows.append(r)
                    spans.append((lo, hi))
        return sorted(rows, key=lambda r: int(r["offset"], 16))

    nstruct, nfield, problems, views = 0, 0, [], []
    layout = ["# struct\tsize\toffset\tlength\tctype\tname\tcomment   (GENERATED for tools/ghidra/ApplyClassTypes.java)"]
    for path in paths:
        sname = ident(path.stem)
        rows = flattened(sname)
        total = sizes.get(sname)
        out.append("struct %s {%s" % (sname, ("  // object size 0x%x" % total) if total else ""))
        cur, used, bit_off, last = 0, set(), None, None   # last: the previous plain field, for unions
        offs = [int(r["offset"], 16) for r in rows]
        for idx, r in enumerate(rows):
            off = int(r["offset"], 16)
            bf = BITF.fullmatch(r["ctype"].strip())
            if bf and bit_off == off:                 # another bitfield in the current storage unit
                fname = ident(r["name"])
                while fname in used:
                    fname += "_"
                out.append("    %-34s // +0x%03x (bits) %s" % (field_layout(r["ctype"], fname, 1)[0] + ";", off,
                                                              re.sub(r"\s+", " ", r["meaning"])[:100]))
                used.add(fname)
                nfield += 1
                continue
            bit_off = off if bf else None
            nxt = min([o for o in offs[idx + 1:] if o > off] + ([total] if total else []) or [off + 4])
            fname = ident(r["name"])
            while fname in used:
                fname += "_"
            decl, sz, opaque = field_layout(r["ctype"], fname, max(nxt - off, 1))
            note = "%s%s%s" % ("(%s) " % r["ctype"].strip() if opaque else "", re.sub(r"\s+", " ", r["meaning"])[:110],
                               "  [inferred]" if r.get("confidence", "").strip() == "inferred" else "")
            if last is not None and off == last["off"] and not bf and (not opaque or type_size(r["ctype"]) is not None):
                line = "        %-30s // +0x%03x %s" % (decl + ";", off, note)          # another member of a union (a struct view: its bytes)
                if last["close"] is None:
                    out[last["idx"]] = "    " + out[last["idx"]]
                    out.insert(last["idx"], "    union {")
                    out.append(line)
                    out.append("    };")
                    last["close"] = len(out) - 1
                else:
                    out.insert(last["close"], line)
                    last["close"] += 1
                used.add(fname)
                cur = max(cur, off + sz)
                nfield += 1
                if opaque:      # a struct view stays out of the Ghidra table (its later row would replace the plain field there)
                    views.append((sname, fname, off))
                else:
                    layout.append("\t".join([sname, "0x%x" % (total or 0), "0x%x" % off, "0x%x" % sz, r["ctype"].strip(), fname,
                                             note.replace("\t", " ")]))
                continue
            if off < cur or (total and off + sz > total):
                problems.append("%s+0x%x %s overlaps/overflows (cur 0x%x)" % (sname, off, r["name"], cur))
                out.append("    // +0x%03x %s;  // OVERLAP - %s" % (off, decl, note))
                continue
            if off > cur:
                out.append("    u8 _pad%03x[0x%x];" % (cur, off - cur))
            out.append("    %-34s // +0x%03x %s" % (decl + ";", off, note))
            last = None if bf else {"off": off, "idx": len(out) - 1, "close": None}
            used.add(fname)
            cur = off + sz
            nfield += 1
            layout.append("\t".join([sname, "0x%x" % (total or 0), "0x%x" % off, "0x%x" % sz,
                                     ("u8[0x%x]" % sz) if opaque else (bf.group(1) if bf else r["ctype"].strip()),
                                     ("_bits_%x" % off) if bf else fname, note.replace("\t", " ")]))
        if total and cur < total:
            out.append("    u8 _pad%03x[0x%x];" % (cur, total - cur))
        out.append("};")
        if total:
            out.append("SDW_SIZE_CHECK(%s, 0x%x);" % (sname, total))
        out.append("")
        nstruct += 1
    out.append("#pragma pack(pop)")
    out.append("#endif")
    write_atomic(INC / "sdw_structs.h", "\n".join(out) + "\n")
    work = INC if private else ROOT / "work"
    work.mkdir(exist_ok=True)
    write_atomic(work / "struct_layout.tsv", "\n".join(layout) + "\n")

    # Independent self-check: re-emit the header with every pointer turned
    # into a 4-byte integer so it has the game's layout on ANY host, then assert offsetof() of every field and sizeof() of every struct.
    hdr = "\n".join(out)
    hdr = re.sub(r"(?m)^(\s*)(?:struct )?\w+ \*+\s*(\w+)", r"\1u32 \2", hdr)  # ANY pointer declarator -> 4 bytes on the host
    hdr = hdr.replace('#include "sdw_types.h"', (INC / "sdw_types.h").read_text().replace("typedef void (*fnptr)(void);", "typedef u32 fnptr;"))
    hdr = re.sub(r"(?m)^typedef [^(\n]+ \(SDW_CDECL \*(\w+)\)\(.*$", r"typedef u32 \1;", hdr)  # named fn-pointer types -> 4 bytes too
    chk = [hdr, "#include <stddef.h>"]
    for line in layout[1:]:
        sname, total, off, sz, _ctype, fname, _ = line.split("\t", 6)
        if fname.startswith("_bits_"):              # a bitfield storage unit: no member of that name, no address
            continue
        chk.append("_Static_assert(offsetof(struct %s, %s) == %s, \"%s.%s offset\");" % (sname, fname, off, sname, fname))
    for sname, fname, off in views:
        chk.append("_Static_assert(offsetof(struct %s, %s) == 0x%x, \"%s.%s offset\");" % (sname, fname, off, sname, fname))
    for sname, total in sorted({(l.split("\t")[0], l.split("\t")[1]) for l in layout[1:]}):
        if int(total, 16):
            chk.append("_Static_assert(sizeof(struct %s) == %s, \"%s size\");" % (sname, total, sname))
    write_atomic(work / "layout_check.c", "\n".join(chk) + "\n")
    print("%d structs (%d fields), %d enums -> src/include/   (verify: cc -fsyntax-only -std=c11 work/layout_check.c)" % (nstruct, nfield, nenum))
    for p in problems[:20]:
        print("  layout problem:", p)
    import cpp_classes                     # the C++ class header for the VC6 match sources, from the same CSVs
    cpp_classes.Gen(struct_paths(), INC / "sdw_classes.h", None if private else cpp_classes.VT_JSON,
                    Path(OVERLAY) / "vtable_slots.csv" if OVERLAY and (Path(OVERLAY) / "vtable_slots.csv").exists() else None).run()


if __name__ == "__main__":
    main()
