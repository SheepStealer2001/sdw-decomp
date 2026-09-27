#!/usr/bin/env python3
"""Locate every compiled libjpeg function (src/jpeg, IJG release 6) in SheepD3D.exe and record where it is.

libjpeg's files stay exactly as IJG shipped them, so they cannot carry match-addr lines, and many of their static
functions share names across files (start_pass, process_restart, ...). This finds each function of each compiled
object (work/match/src_jpeg_<file>.obj, from `python3 tools/vc6.py src/jpeg/*.c`) in the exe's libjpeg range by its
bytes with relocation fields masked, and writes the addresses into src/jpeg/vc6.json as per-file pins (COFF name ->
VA), which vc6.py hands to the matcher. Prints the public (external) ones as rows for data/symbols.csv.

Not found are (a) functions the game never calls, which /OPT:REF removed, and (b) bodies identical to another one
that /OPT:ICF folded: those are pinned by hand in FOLDED below, from the exe (see README-SDW.md).
Usage: python3 tools/jpeg_pins.py [--write]
"""
import json
import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import match  # noqa: E402

ROOT = Path(__file__).resolve().parents[1]
JPEG = ROOT / "src" / "jpeg"
LO, HI = 0x4222c0, 0x42dae0
# Folded by /OPT:ICF into another identical body (read from the exe: the surviving copy's address and its references).
FOLDED = {
    "jdatasrc.c": {"_term_source": 0x422c30},              # the surviving static "ret"; three more fold into it
    "jdcolor.c": {"_start_pass_dcolor": 0x422c30},
    "jquant1.c": {"_finish_pass_1_quant": 0x422c30},
    "jquant2.c": {"_finish_pass2": 0x422c30},
    "jdcoefct.c": {"_dummy_consume_data": 0x4231a0},       # static "xor eax,eax; ret", kept on its own
    "jmemnobs.c": {"_jpeg_mem_init": 0x4222b0,             # public "xor eax,eax; ret": folded into Video_NullFrameProc
                   "_jpeg_mem_term": 0x42bc80,             # public "ret": the survivor the game's empty calls also use
                   # the two malloc and the two free wrappers are identical in pairs; one copy of each survives, and
                   # not the same way round: free_small (earlier) 0x42bc10 calls free 0x566c4c, get_large (later)
                   # 0x42bc30 calls malloc 0x566b12
                   "_jpeg_free_small": 0x42bc10, "_jpeg_free_large": 0x42bc10,
                   "_jpeg_get_small": 0x42bc30, "_jpeg_get_large": 0x42bc30},
}
# Removed by /OPT:REF (the game never calls them) but byte-identical, relocations masked, to a function that stayed:
# never pin them at that function's address.
REMOVED = {"jdapimin.c": {"_jpeg_abort_decompress"}}


def locate(obj, region):
    out = {}
    for s, sec, start, span in obj.functions():
        code = obj.sections[sec]
        body = code["data"][start:start + span]
        masked = set()
        for off, si, ty in code["relocs"]:
            if start <= off < start + span:
                masked.update(range(off - start, off - start + 4))
        end = span
        while end > 0 and body[end - 1] == 0x90 and (end - 1) not in masked:
            end -= 1
        if end < 6:
            continue
        pat = b"".join(b"." if i in masked else re.escape(bytes([body[i]])) for i in range(end))
        hits = [m.start() + LO for m in re.finditer(pat, region, re.S)]
        if len(hits) == 1:
            out[s["name"]] = (hits[0], s["cls"])
    return out


def main():
    exe = match.Exe(match.EXE)
    region = exe.read(LO, HI - LO)
    cfg = json.load(open(JPEG / "vc6.json"))
    pins, public = {}, []
    for src in sorted(JPEG.glob("*.c")):
        obj = match.Obj(ROOT / "work" / "match" / ("src_jpeg_" + src.stem + ".obj"))
        found = {n: v for n, v in locate(obj, region).items() if n not in REMOVED.get(src.name, set())}
        found.update({n: (a, 2 if not n.startswith(("_term", "_start_pass", "_finish", "_dummy")) else 3)
                      for n, a in FOLDED.get(src.name, {}).items()})
        pins[src.name] = {n: "0x%x" % a for n, (a, cls) in sorted(found.items(), key=lambda x: x[1][0])}
        for n, (a, cls) in found.items():
            if cls == 2:
                public.append((a, n.lstrip("_"), src.name))
    cfg["addr"] = pins
    print("pinned %d functions in %d files" % (sum(len(v) for v in pins.values()), len(pins)))
    for a, n, f in sorted(public):
        print("0x%x,%s,func,IJG libjpeg release 6 (%s); located by tools/jpeg_pins.py" % (a, n, f))
    if "--write" in sys.argv:
        (JPEG / "vc6.json").write_text(json.dumps(cfg, indent=1) + "\n")
        print("wrote", (JPEG / "vc6.json").relative_to(ROOT))


if __name__ == "__main__":
    main()
