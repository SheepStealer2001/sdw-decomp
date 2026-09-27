#!/usr/bin/env python3
"""Evidence for recovering the ORIGINAL translation units (the .obj files the game was linked from).

A byte-identical rebuild needs the source organised as the original compilation units: MS LINK lays out each object's
sections contiguously, in link order, so every original file is one run of .text plus one run in each of .rdata, .data
and .bss, all in the same file order. This tool gathers what the exe says about those runs:

  * .text sections: with the compiled spans (they include switch tables), every gap between game functions is either
    nothing (same section), 0xCC (LINK's padding between sections: an object boundary, or a COMDAT such as ??_G) or 0x90
    (VC6 /O2 pads each function to 16 bytes inside its own section). Each run is a "segment".
  * data items: every .rdata/.data/.bss address the compiled code refers to (tools/link_census.py), with its kind (string,
    float constant, vtable, static, global), size where the listing states it, and the segments referring to it.
  * static initialisers (.CRT$XCU entries) in link order, and linkage hints (a function some caller names with a C
    symbol was probably defined in a .c file).

Output: work/tu/evidence.json and one readable report per region, work/tu/region_NN.txt. Inputs: work/link/census.json
(run tools/link_census.py first), data/function_sizes.json, the exe. Read-only elsewhere.
"""
import collections
import json
import re
import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import match           # noqa: E402
import vc6             # noqa: E402
import listing_sizes   # noqa: E402

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / "work" / "tu"
TEXT_END = 0x5644e0            # first import thunk; the DirectX lib data and thunks and the CRT follow
JPEG = (0x4222c0, 0x42dae0)
SECTIONS = [(".rdata", 0x5742cc, 0x579000), (".data", 0x579000, 0x584c50), (".bss", 0x584c50, 0x71f000)]
XCU = (0x579004, 0x5790d0)
NREGIONS = 10


def section_of(a):
    for n, lo, hi in SECTIONS:
        if lo <= a < hi:
            return n
    return None


def main():
    exe = match.Exe(match.EXE)
    by_name, by_addr = match.load_symbols()
    census = json.load(open(ROOT / "work" / "link" / "census.json"))

    # ---- functions: the compiled span where there is one, else Ghidra's size
    fn = {}
    for o in census:
        src = ROOT / o["file"]
        flags = " ".join(vc6.file_flags(src))
        obj = match.Obj(ROOT / o["obj"])
        for f in o["functions"]:
            if f["va"] is None:
                continue
            comdat = bool(obj.sections[f["sec"]]["flags"] & 0x1000)
            prev = fn.get(f["va"])
            if prev is None or (prev["comdat"] and not comdat):
                fn[f["va"]] = {"va": f["va"], "span": f["span"], "comdat": comdat, "file": o["file"], "flags": flags,
                               "symbol": f["symbol"], "name": by_addr.get(f["va"], f["symbol"])}
    for k, v in json.load(open(ROOT / "data" / "function_sizes.json")).items():
        a = int(k, 16)
        if a < TEXT_END and a not in fn:
            fn[a] = {"va": a, "span": v["size"], "comdat": None, "file": None, "flags": None, "symbol": None,
                     "name": by_addr.get(a), "size_from": "ghidra"}
    fn.pop(0x450080, None)          # an internal switch arm of Gossamer_Boss_Update that Ghidra split off

    # C-linkage hints: callers naming a function with an undecorated C symbol
    c_named = collections.defaultdict(set)
    for o in census:
        for r in o["refs"]:
            n = r["raw"]
            if not n.startswith(("?", "$", "__imp_", ".", "__real")) and r["target"] in fn:
                c_named[r["target"]].add(o["file"])
    for a, files in c_named.items():
        fn[a]["c_named_by"] = sorted(files)

    # ---- segments
    addrs = sorted(a for a in fn if a < TEXT_END)
    segs, cur = [], None
    for i, a in enumerate(addrs):
        f = fn[a]
        if cur is None:
            cur = {"start": a, "funcs": [a]}
        end = a + f["span"]
        nxt = addrs[i + 1] if i + 1 < len(addrs) else TEXT_END
        gap = nxt - end
        pad = exe.read(end, gap) if gap > 0 else b""
        kind = "none" if gap <= 0 else "cc" if set(pad) == {0xCC} else "90" if set(pad) == {0x90} else "other"
        f["gap_after"] = {"len": gap, "fill": kind}
        if kind == "none" and nxt < TEXT_END:
            cur["funcs"].append(nxt)
            continue
        cur["end"] = end
        cur["pad_after"] = {"len": max(gap, 0), "fill": kind}
        segs.append(cur)
        cur = None
    for i, s in enumerate(segs):
        f0 = fn[s["start"]]
        s["id"] = i
        s["kind"] = ("jpeg" if JPEG[0] <= s["start"] < JPEG[1] else
                     "o2" if f0.get("flags") and "/O2" in f0["flags"] else
                     "comdat" if f0["comdat"] else "plain" if f0["comdat"] is False else "unknown")
        s["files"] = sorted({fn[a]["file"] for a in s["funcs"] if fn[a]["file"]})
        for a in s["funcs"]:
            fn[a]["seg"] = i

    # ---- data items referred to by the compiled code
    items = {}
    sizes_cache = {}
    p = ROOT / "work" / "link" / "obj_items.json"      # optional: data item sizes, used when present
    obj_items = json.load(open(p)) if p.exists() else []
    size_at = {}
    for it in obj_items:
        size_at.setdefault(it["addr"], it["size"])
    for o in census:
        fva = sorted((f["va"], f["span"]) for f in o["functions"] if f["va"])
        for r in o["refs"]:
            t = r["target"]
            sec = section_of(t)
            if sec is None:
                continue
            site_fn = next((va for va, sp in fva if va <= r["site"] < va + sp), None)
            s = r["symbol"]
            kind = ("string" if s.startswith(("$SG", "??_C")) else "float" if s.startswith("__real") else
                    "vtable" if s.startswith("??_7") else "static" if (r["local"] or r["section_symbol"]) and r["defined"]
                    else "global_defined_here" if r["defined"] else "extern")
            it = items.setdefault(t, {"addr": t, "section": sec, "kinds": set(), "names": set(), "segs": set(),
                                      "funcs": set(), "size": size_at.get(t)})
            it["kinds"].add(kind)
            if not r["section_symbol"]:
                it["names"].add(s)
            if site_fn is not None:
                it["funcs"].add(site_fn)
                if site_fn in fn and "seg" in fn[site_fn]:
                    it["segs"].add(fn[site_fn]["seg"])
            tn = by_addr.get(t)
            if tn:
                it["names"].add(tn)
    item_list = []
    for t in sorted(items):
        it = items[t]
        item_list.append({"addr": t, "section": it["section"], "kinds": sorted(it["kinds"]), "names": sorted(it["names"]),
                          "segs": sorted(it["segs"]), "funcs": sorted(it["funcs"]), "size": it["size"]})

    # ---- order check: items used by exactly one segment should follow segment order within each section
    inversions = []
    for sec, _, _ in SECTIONS:
        seq = [(it["addr"], it["segs"][0]) for it in item_list if it["section"] == sec and len(it["segs"]) == 1]
        for (a, sa), (b, sb) in zip(seq, seq[1:]):
            if sb < sa:
                inversions.append({"section": sec, "a": a, "seg_a": sa, "b": b, "seg_b": sb})

    # ---- static initialisers, in link order
    xcu = []
    for a in range(*XCU, 4):
        v = exe.u32(a)
        xcu.append({"slot": a, "func": v, "name": by_addr.get(v), "seg": fn.get(v, {}).get("seg")})

    ev = {"about": __doc__.split("\n")[0], "functions": [fn[a] for a in sorted(fn)], "segments": segs,
          "data_items": item_list, "inversions": inversions, "xcu": xcu,
          "rich": {"cpp_objects_pp": 299, "c_objects_pp": 29, "note": "Utc12_2_CPP / Utc12_2_C build 9044 counts"}}
    OUT.mkdir(parents=True, exist_ok=True)
    (OUT / "evidence.json").write_text(json.dumps(ev, indent=0, default=list))

    # ---- readable region reports: consecutive segments, cut near equal counts but never inside an /O2 or jpeg run
    weight = [0 if s["kind"] == "jpeg" else 1 for s in segs]     # libjpeg's many small sections count as one block
    per, acc, cuts = sum(weight) / NREGIONS, 0, [0]
    for i in range(1, len(segs)):
        acc += weight[i - 1]
        if len(cuts) < NREGIONS and acc >= per * len(cuts) and segs[i]["kind"] == "plain" and segs[i - 1]["kind"] != "jpeg":
            cuts.append(i)
    cuts.append(len(segs))
    regions = []
    for r, (lo, hi) in enumerate(zip(cuts, cuts[1:])):
        rs = segs[lo:hi]
        segset = set(range(lo, hi))
        lines = ["REGION %02d: segments %d-%d, .text 0x%x-0x%x" % (r, lo, hi - 1, rs[0]["start"], rs[-1]["end"]), ""]
        lines.append("== CODE SEGMENTS (a segment = one run of packed functions; pad_after = bytes before the next one)")
        for s in rs:
            lines.append("SEG %d  %s  0x%x-0x%x  pad_after %d x %s  files: %s" % (
                s["id"], s["kind"], s["start"], s["end"], s["pad_after"]["len"], s["pad_after"]["fill"], ", ".join(s["files"])))
            for a in s["funcs"]:
                f = fn[a]
                extra = ""
                if f.get("c_named_by"):
                    extra = "  [C-named by %s]" % ", ".join(x.split("/")[-1] for x in f["c_named_by"])
                lines.append("    0x%06x +%-5d %s%s %s%s" % (a, f["span"], "COMDAT " if f["comdat"] else "",
                                                          f["name"], ("(" + f["file"].split("/")[-1] + ")") if f["file"] else "", extra))
        for sec, _, _ in SECTIONS:
            lines.append("")
            lines.append("== %s items referred to by this region's code (segs = referring segments; * = also used outside the region)" % sec)
            for it in item_list:
                if it["section"] != sec or not (set(it["segs"]) & segset):
                    continue
                outside = set(it["segs"]) - segset
                lines.append("    0x%06x size %-6s %-28s segs %s%s  %s" % (
                    it["addr"], it["size"] if it["size"] is not None else "?", "/".join(it["kinds"]),
                    ",".join(map(str, it["segs"][:8])) + ("..." if len(it["segs"]) > 8 else ""), " *" if outside else "",
                    " ".join(sorted(it["names"]))[:120]))
        lines.append("")
        lines.append("== static initialisers (.CRT$XCU, link order) in this region")
        for x in xcu:
            if x["seg"] in segset:
                lines.append("    slot 0x%x -> 0x%x %s (seg %s)" % (x["slot"], x["func"], x["name"], x["seg"]))
        lines.append("")
        lines.append("== data-order inversions touching this region")
        for inv in inversions:
            if inv["seg_a"] in segset or inv["seg_b"] in segset:
                lines.append("    %s 0x%x (seg %d) then 0x%x (seg %d)" % (inv["section"], inv["a"], inv["seg_a"], inv["b"], inv["seg_b"]))
        (OUT / ("region_%02d.txt" % r)).write_text("\n".join(lines) + "\n")
        regions.append({"region": r, "segs": [lo, hi - 1], "text": [rs[0]["start"], rs[-1]["end"]]})
    (OUT / "regions.json").write_text(json.dumps(regions, indent=1))
    kinds = collections.Counter(s["kind"] for s in segs)
    print("%d functions, %d segments %s, %d data items, %d order inversions, %d regions -> %s" % (
        len(fn), len(segs), dict(kinds), len(item_list), len(inversions), len(regions), OUT.relative_to(ROOT)))


if __name__ == "__main__":
    main()
