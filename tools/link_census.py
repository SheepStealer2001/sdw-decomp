#!/usr/bin/env python3
"""Census for linking: where every compiled function sits in SheepD3D.exe and what every relocation in it points at.

For each object of src/ (compiled by tools/vc6.py into work/match/) this re-runs the matcher's placement (so a function
lands at its original address) and then reads, at each relocation site, the address the ORIGINAL code refers to.
Because every function byte-matches, that address is exactly the original target of the reference, including data
defined inside the object (its string literals, constants, vtables) and externals no table names. tools/layout.py,
tools/tu_evidence.py and tools/tu_sheet.py read it. Output: work/link/census.json. Read-only on everything else.

Usage: python3 tools/link_census.py
"""
import json
import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import match  # noqa: E402
import vc6    # noqa: E402

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / "work" / "link" / "census.json"


def sources():
    return sorted(p for p in (ROOT / "src").rglob("*") if p.suffix in (".c", ".cpp"))


def obj_path(src):
    return vc6.OUT / ("_".join(src.resolve().relative_to(ROOT).with_suffix("").parts) + ".obj")


def census(src, exe):
    obj = match.Obj(obj_path(src))
    m = match.Matcher(obj, exe, dict(vc6.file_addrs(src)), vc6.file_init_roots(src))
    m.run()
    funcs, refs = [], []
    for s, sec, start, span, va, nm in m.placed:
        va = va if va is not None else m.derived.get((sec, start))
        funcs.append({"symbol": s["name"], "sec": sec, "start": start, "span": span, "va": va})
        if va is None:
            continue
        code = obj.sections[sec]
        orig = exe.read(va, span)
        for off, si, ty in code["relocs"]:
            if not (start <= off < start + span) or ty in match.DEBUG_RELOCS:
                continue
            fo = off - start
            raw = obj.symbols[si]
            sym = obj.resolve(raw)
            addend = struct.unpack_from("<I", code["data"], off)[0]
            o = struct.unpack_from("<I", orig, fo)[0]
            if ty == match.DIR32:
                S = (o - addend) & 0xFFFFFFFF
            elif ty == match.REL32:
                S = (o + va + fo + 4 - addend) & 0xFFFFFFFF
            elif ty == match.DIR32NB:
                S = (o + exe.base - addend) & 0xFFFFFFFF
            else:
                continue
            section = obj.is_section_symbol(sym)
            refs.append({"symbol": sym["name"], "raw": raw["name"], "defined": sym["sec"] > 0, "section_symbol": section,
                         "local": sym["cls"] != 2, "sec": sym["sec"], "value": sym["value"],
                         "addend": addend if section else 0,
                         # the original address of the referenced item itself (a section symbol + addend is the item)
                         "target": (S + addend) & 0xFFFFFFFF if section else S,
                         "type": ty, "site": va + fo})
    # the object's own data: every symbol and section, with sizes, so an item found above can be sized
    data = []
    for i, sec in enumerate(obj.sections[1:], 1):
        if sec["flags"] & match.SCN_CODE or sec["name"].startswith((".debug", ".drectve")):
            continue
        data.append({"sec": i, "name": sec["name"], "size": sec["size"], "flags": sec["flags"],
                     "symbols": sorted({(s["value"], s["name"], s["cls"]) for s in obj.symbols.values() if s["sec"] == i})})
    return {"file": str(src.relative_to(ROOT)), "obj": str(obj_path(src).relative_to(ROOT)), "functions": funcs,
            "refs": refs, "data": data}


def main():
    exe = match.Exe(match.EXE)
    out = []
    for src in sources():
        out.append(census(src, exe))
    OUT.parent.mkdir(parents=True, exist_ok=True)
    OUT.write_text(json.dumps(out))
    nf = sum(len(o["functions"]) for o in out)
    nr = sum(len(o["refs"]) for o in out)
    print("%d objects, %d functions, %d relocations -> %s" % (len(out), nf, nr, OUT.relative_to(ROOT)))


if __name__ == "__main__":
    main()
