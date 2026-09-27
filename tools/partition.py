#!/usr/bin/env python3
"""Carve SheepD3D.exe's game code into modules (one per scenaric class + engine chunks), with the naming coverage of each.

Method: MSVC emits each translation unit's functions contiguously. A class's OWN functions are known exactly: its factory plus
every vtable slot whose target does not already appear in one of its base vtables (i.e. an override or a new virtual). Every other
function is assigned to the class owning the nearest own-function by address; functions farther than GAP bytes from any anchor form
"engine_<addr>" chunks, split so no chunk exceeds MAX_FUNCS functions.

The linker pads BETWEEN original source files with int3 (0xCC), and VC6 emits the functions of one /Od file back to back with
no padding, so a padded gap is evidence of a file boundary. A non-anchor function is therefore assigned to an anchor in its own
padding-delimited block first, and only falls back to "nearest anchor within GAP" when its block holds no anchor at all. Optimised files align their functions the same way, so a block is not always a file:
assignment of non-anchor functions stays a HEURISTIC - reading the code may show otherwise.

Inputs : data/class_map.csv, work/ghidra_functions.json, the exe
Outputs: data/modules.csv (one row per module with its coverage), work/modules.json (full function lists)
"""
import csv
import json
import struct
from bisect import bisect_left
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
EXE = ROOT / "Sheep, Dog 'n' Wolf (PAL Version)" / "SheepD3D.exe"
BASE = 0x400000
TEXT = (0x401000, 0x565850)
GAP = 0x1800
# libjpeg 6 (docs/01; 0x42c2e0 takes a j_decompress_ptr, 0x42da00 is jdiv_round_up,
# 0x42dac0 is jzero_far). Library code, not game code: it must never be pulled into a class module - by address alone
# BipbipLevel14 would get a 31-function range starting 0x42c2e0, although its vtable has nothing below 0x42dae0.
LIBJPEG = (0x4222c0, 0x42dae0)
MAX_FUNCS = 70


def main():
    exe = EXE.read_bytes()
    funcs = {int(a, 16): v for a, v in json.loads((ROOT / "work" / "ghidra_functions.json").read_text()).items() if v["game"]}
    addrs = sorted(funcs)
    classes = list(csv.DictReader((ROOT / "data" / "class_map.csv").open()))

    vts = set()
    for c in classes:
        if c["vtable"]:
            vts.add(int(c["vtable"], 16))
            vts.update(int(b, 16) for b in c["base_vtables(base->derived)"].split())
    vt_sorted = sorted(vts)

    def slots(vt):
        nxt = next((v for v in vt_sorted if v > vt), vt + 0x400)
        out = []
        for off in range(0, min(nxt - vt, 0x400), 4):
            p, = struct.unpack_from("<I", exe, vt - BASE + off)
            if not (TEXT[0] <= p < TEXT[1]):
                break
            out.append(p)
        return out

    base_targets = {vt: set(slots(vt)) for vt in vts}
    anchors = {}  # func addr -> class name
    info = {}
    for c in classes:
        if not c["vtable"]:
            continue
        vt = int(c["vtable"], 16)
        inherited = set()
        for b in c["base_vtables(base->derived)"].split():
            inherited |= base_targets[int(b, 16)]
        own = [p for p in slots(vt) if p not in inherited]
        own.append(int(c["factory"], 16))
        name = c["name"].replace(" ", "")
        info[name] = {"class_id": int(c["class_id"]), "vtable": c["vtable"], "obj_size": c["obj_size"], "own": sorted(set(own))}
        for p in own:
            anchors.setdefault(p, name)
    # shared base-class implementations anchor their own modules
    for vt, label in ((0x574C40, "ScnObject"), (0x574C1C, "ScnBody"), (0x574C84, "ScnMobile"), (0x574FD0, "ScnControllable"), (0x57595C, "ScnLogic")):
        for p in slots(vt):
            anchors.setdefault(p, "base_" + label)

    # a function that already carries a verified "<Class>_..." name belongs to that class, wherever the linker put it
    by_prefix = {n.lower(): n for n in info}
    by_prefix.update({"scnobject": "base_ScnObject", "scnbody": "base_ScnBody", "scnmobile": "base_ScnMobile",
                      "scncontrollable": "base_ScnControllable", "scnlogic": "base_ScnLogic", "mobile": "base_ScnMobile"})

    a_sorted = sorted(anchors)

    # Padding-delimited blocks: the bytes between one function's end and the next function's start are int3 / nop filler
    # when the linker started a new source file (or, in an optimised file, aligned the next function). One block per run.
    block_of, blocks = {}, {}
    b = 0
    for i, f in enumerate(addrs):
        if i:
            prev = addrs[i - 1]
            gap = exe[prev + funcs[prev]["size"] - BASE:f - BASE]
            if gap and all(c in (0xCC, 0x90) for c in gap):
                b += 1
        block_of[f] = b
        blocks.setdefault(b, []).append(f)
    anchors_in_block = {}
    for a in a_sorted:
        if a in block_of:
            anchors_in_block.setdefault(block_of[a], []).append(a)

    modules, guessed = {}, set()             # guessed: assigned across a padding boundary, so the class is not evidence
    for f in addrs:
        if LIBJPEG[0] <= f < LIBJPEG[1]:     # library code keeps its own module and never joins a class
            modules.setdefault("libjpeg", []).append(f)
            continue
        pre = funcs[f]["name"].split("_", 1)[0].lower()
        if "_" in funcs[f]["name"] and pre in by_prefix and not funcs[f]["name"].startswith("FUN_"):
            modules.setdefault(by_prefix[pre], []).append(f)
            continue
        same = anchors_in_block.get(block_of[f])
        if same:                                   # an anchor in this function's own block wins, at any distance
            owner = anchors[min(same, key=lambda a: abs(a - f))]
        else:                                      # no anchor in this block: fall back to distance, and flag the guess
            i = bisect_left(a_sorted, f)
            cands = [a_sorted[j] for j in (i - 1, i) if 0 <= j < len(a_sorted)]
            near = min(cands, key=lambda a: abs(a - f))
            owner = anchors[near] if abs(near - f) <= GAP else None
            if owner:
                guessed.add(f)
        modules.setdefault(owner, []).append(f)

    engine = modules.pop(None, [])
    chunk, chunks = [], []
    for f in engine:
        if chunk and (f - chunk[-1] > GAP or len(chunk) >= MAX_FUNCS):
            chunks.append(chunk)
            chunk = []
        chunk.append(f)
    if chunk:
        chunks.append(chunk)
    for ch in chunks:
        modules["engine_%06x" % ch[0]] = ch

    rows, full = [], {}
    for name, fl in modules.items():
        named = [f for f in fl if not funcs[f]["name"].startswith(("FUN_", "thunk_"))]
        size = sum(funcs[f]["size"] for f in fl)
        ci = info.get(name, {})
        rows.append([name, ci.get("class_id", ""), "0x%x" % fl[0], "0x%x" % fl[-1], len(fl), size, len(named),
                     "%.0f" % (100.0 * len(named) / len(fl)), ci.get("vtable", ""), ci.get("obj_size", "")])
        full[name] = {"functions": ["%08x" % f for f in fl], "class": ci,
                      "guessed": ["%08x" % f for f in fl if f in guessed]}
    rows.sort(key=lambda r: int(r[2], 16))
    with (ROOT / "data" / "modules.csv").open("w", newline="") as fh:
        w = csv.writer(fh)
        w.writerow(["module", "class_id", "first", "last", "functions", "code_bytes", "named", "named_pct", "vtable", "obj_size"])
        w.writerows(rows)
    (ROOT / "work" / "modules.json").write_text(json.dumps(full, indent=1))

    tot = sum(r[4] for r in rows)
    print("%d of them assigned across a padding boundary (guesses; listed per module in work/modules.json)" % len(guessed))
    print("%d modules (%d class, %d engine chunks) covering %d functions" % (
        len(rows), sum(1 for r in rows if not r[0].startswith("engine_")), sum(1 for r in rows if r[0].startswith("engine_")), tot))
    print("largest modules by code size:")
    for r in sorted(rows, key=lambda r: -r[5])[:22]:
        print("  %-22s %s..%s  %4d funcs %7d bytes  named %3d%%" % (r[0], r[2], r[3], r[4], r[5], int(r[7])))


if __name__ == "__main__":
    main()
