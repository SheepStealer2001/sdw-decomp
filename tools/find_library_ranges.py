#!/usr/bin/env python3
"""Recover TRANSLATION-UNIT boundaries in SheepD3D.exe from the call graph, and measure how a range is entered.

WHAT THIS ACTUALLY DOES (read this before trusting the output):

1. Boundary finding - WORKS, and is the reason to use this tool. MSVC 6.0 emits each .cpp file's functions
   contiguously and in source order, so a file boundary is a place no LOCAL call crosses. Counting *all*
   call edges across a boundary is useless (the median boundary has ~1670 crossings, because the whole game
   calls shared engine helpers across the image); restricting to edges spanning fewer than LOCAL_WINDOW
   functions leaves a clean signal. Validation: with no vtable anchor at all this isolates 0x47d310-0x4935f7
   as exactly the Wolf, and 0x461f20-0x46ec15 as Sam. It is a better partition than tools/partition.py's
   arbitrary 70-function slices, which cut across real file boundaries.

2. Library detection - DOES NOT WORK automatically, so there is deliberately no verdict column.
   Both obvious metrics fail on their own:
     - few external entries: a self-contained GAME class scores identically. Sam and Robot have ZERO
       external entries, because everything reaches them through vtables.
     - many orphans (no direct caller) was meant to mean "vtable-dispatched game code", but libjpeg scores
       54% orphans, since it dispatches through method pointers in its struct exactly like a vtable.
   What does work is the hand test: take the range, list its entry points, and see whether they are a small
   published API. That is how libjpeg 6 is pinned to 0x4222c0-0x42dae0: 151 functions whose external entries
   come from the two image decoders, in libjpeg's documented call order. Use the
   two-argument form to run that check on a candidate.

The "overlaps class module" column exists to tell case 2 apart by hand: a range that covers a known class
is game code, and a large range covering none is unclassified engine code worth a module pass.

Usage:
  python3 tools/find_library_ranges.py                     # partition the game range, largest first
  python3 tools/find_library_ranges.py 0x4222c0 0x42dae0   # entry-point report for one range (the library test)
"""
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
CRT_START = 0x565850          # everything from here is the MSVC CRT (docs/01)
MIN_FUNCS = 20                # a range smaller than this is not worth excluding
MAX_ENTRIES = 14              # a library's API surface is small
LOCAL_WINDOW = 60             # only calls spanning fewer than this many functions count as 'local'


def load():
    g = json.loads((ROOT / "work" / "ghidra_functions.json").read_text())
    funcs = {}
    for a, v in g.items():
        n = int(a, 16)
        if v.get("game") and n < CRT_START:
            funcs[n] = {"name": v.get("name", ""), "size": v.get("size", 0),
                        "callers": [int(c, 16) for c in v.get("callers", [])]}
    return funcs


def measure(funcs, addrs, lo, hi):
    """-> (n, entries, orphans, entry_detail) for the range [lo, hi]."""
    inside = [a for a in addrs if lo <= a <= hi]
    entries, orphans = [], 0
    for a in inside:
        cs = funcs[a]["callers"]
        if not cs:
            orphans += 1
            continue
        ext = [c for c in cs if not (lo <= c <= hi)]
        if ext:
            entries.append((a, ext))
    return len(inside), entries, orphans


def cut_profile(funcs, addrs):
    """cross[i] = number of call edges spanning the boundary just before addrs[i].

    MSVC 6.0 emits each translation unit's functions contiguously and in source order, so a real module
    boundary is a place where very few calls cross. Growing a range never helps here the way an
    external-entry count does (widening simply absorbs the callers), which is why the boundary has to be
    scored directly. Low points in this profile are candidate .cpp boundaries; a library shows up as a
    long run bracketed by two of them.
    """
    idx = {a: i for i, a in enumerate(addrs)}
    n = len(addrs)
    delta = [0] * (n + 2)
    for a in addrs:
        ja = idx[a]
        for c in funcs[a]["callers"]:
            jc = idx.get(c)
            if jc is None or jc == ja:
                continue
            lo, hi = (ja, jc) if ja < jc else (jc, ja)
            # ONLY LOCAL EDGES. Counting every edge is useless here: the median boundary has ~1670
            # crossings because the whole game calls shared engine helpers across the image, which
            # swamps the signal. A translation unit's own calls are short-range, so restricting to
            # edges spanning fewer than LOCAL_WINDOW functions leaves exactly the file structure.
            if hi - lo > LOCAL_WINDOW:
                continue
            delta[lo + 1] += 1          # this edge spans every boundary in (lo, hi]
            delta[hi + 1] -= 1
    cross, run = [0] * (n + 1), 0
    for i in range(n + 1):
        run += delta[i]
        cross[i] = run
    return cross


def scan(funcs, addrs):
    """Ranges bracketed by two low-crossing boundaries: candidate translation units / libraries."""
    cross = cut_profile(funcs, addrs)
    n = len(addrs)
    # a boundary is a "cut" if it is a local minimum and cheap in absolute terms
    cuts = [i for i in range(1, n) if cross[i] == 0]
    cuts = [0] + cuts + [n]
    out = []
    for k in range(len(cuts) - 1):
        i, j = cuts[k], cuts[k + 1] - 1
        if j - i + 1 < MIN_FUNCS:
            continue
        lo, hi = addrs[i], addrs[j]
        cnt, entries, orphans = measure(funcs, addrs, lo, hi)
        out.append((cnt, lo, hi, entries, orphans))
    return out


def report_range(funcs, addrs, lo, hi):
    cnt, entries, orphans = measure(funcs, addrs, lo, hi)
    size = sum(funcs[a]["size"] for a in addrs if lo <= a <= hi)
    print("range 0x%x-0x%x : %d functions, %d bytes" % (lo, hi, cnt, size))
    print("  entered from outside : %d   (a library's API surface)" % len(entries))
    print("  no caller at all     : %d   (high => vtable-dispatched game code, NOT a library)" % orphans)
    print("  entry points:")
    for a, ext in sorted(entries):
        callers = ", ".join("0x%06x %s" % (c, funcs.get(c, {}).get("name", "?")) for c in sorted(ext)[:4])
        print("    0x%06x %-28s <- %s" % (a, funcs[a]["name"] or "-", callers))


def main():
    funcs = load()
    addrs = sorted(funcs)
    if len(sys.argv) >= 3:
        report_range(funcs, addrs, int(sys.argv[1], 16), int(sys.argv[2], 16))
        return
    import csv
    # Known class modules, so a range can be identified instead of guessed at. This matters: a
    # self-contained GAME class (Sam, Sheep, Dragon, Robot) scores exactly like a library on entry
    # count, and libjpeg scores like game code on orphan count because its method-pointer dispatch
    # looks the same as a vtable. Neither metric discriminates on its own - the overlap does.
    classes = []
    mods = ROOT / "data" / "modules.csv"
    if mods.exists():
        for r in csv.DictReader(mods.open()):
            if not r["module"].startswith(("engine_", "base_")):   # base_* are vtable-anchored pseudo-modules spanning the whole hierarchy
                classes.append((int(r["first"], 16), int(r["last"], 16), r["module"]))

    def overlap(lo, hi):
        hits = [nm for a, b, nm in classes if a <= hi and b >= lo]
        return ", ".join(hits[:3]) + (" +%d" % (len(hits) - 3) if len(hits) > 3 else "")

    print("%d game functions below 0x%x\n" % (len(addrs), CRT_START))
    print("Ranges bracketed by boundaries that no LOCAL call crosses - candidate translation units.")
    print("Few entry points is NECESSARY but not sufficient for library code: read the entry points.\n")
    print("%-19s %6s %8s %7s %7s  %s" % ("range", "funcs", "bytes", "entries", "orphan%", "overlaps class module"))
    for cnt, lo, hi, entries, orphans in sorted(scan(funcs, addrs), key=lambda r: -r[0]):
        size = sum(funcs[a]["size"] for a in addrs if lo <= a <= hi)
        print("0x%06x-0x%06x %6d %8d %7d %6d%%  %s"
              % (lo, hi, cnt, size, len(entries), 100.0 * orphans / cnt, overlap(lo, hi) or "-"))


if __name__ == "__main__":
    main()
