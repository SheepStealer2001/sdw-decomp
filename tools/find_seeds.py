#!/usr/bin/env python3
"""List function entry points that are only reachable through pointers, for tools/ghidra/SeedFunctions.java.

Works on raw bytes so it does not depend on any disassembler's alignment:
  * `68 imm32` (push imm32) anywhere in .text
  * every 4-byte-aligned dword in .rdata/.data (vtables, static callback tables)
where imm32 lands in the pre-CRT code range on a `55 8B EC` frame prologue.
Writes work/seed_functions.txt and reports how many Ghidra does not know yet.
"""
import json
import struct
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
EXE = ROOT / "Sheep, Dog 'n' Wolf (PAL Version)" / "SheepD3D.exe"
BASE = 0x400000
TEXT = (0x401000, 0x565850)          # game/engine code, CRT excluded
TEXT_RAW = (0x1000, 0x174000)        # raw == rva for .text/.rdata in this PE
DATA_RAW = (0x174000, 0x185000)      # .rdata + initialised .data


def main():
    d = EXE.read_bytes()

    def is_entry(va):
        o = va - BASE
        return TEXT[0] <= va < TEXT[1] and d[o:o + 3] == b"\x55\x8b\xec"

    seeds = {}
    i = TEXT_RAW[0]
    while True:
        i = d.find(b"\x68", i, TEXT_RAW[1] - 4)
        if i < 0:
            break
        va, = struct.unpack_from("<I", d, i + 1)
        if is_entry(va):
            seeds.setdefault(va, "push@%x" % (i + BASE))
        i += 1
    for o in range(DATA_RAW[0], DATA_RAW[1] - 3, 4):
        va, = struct.unpack_from("<I", d, o)
        if is_entry(va):
            seeds.setdefault(va, "data@%x" % (o + BASE))

    out = ROOT / "work" / "seed_functions.txt"
    out.write_text("".join("%x\n" % a for a in sorted(seeds)))
    known = set()
    gf = ROOT / "work" / "ghidra_functions.json"
    if gf.exists():
        known = {int(a, 16) for a in json.loads(gf.read_text())}
    new = [a for a in seeds if a not in known]
    print("pointer-referenced entry points: %d (push: %d, data: %d); unknown to Ghidra: %d" % (
        len(seeds), sum(v.startswith("push") for v in seeds.values()),
        sum(v.startswith("data") for v in seeds.values()), len(new)))


if __name__ == "__main__":
    main()
