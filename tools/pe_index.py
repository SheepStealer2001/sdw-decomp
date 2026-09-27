#!/usr/bin/env python3
"""Baseline index of SheepD3D.exe built from the raw PE + objdump listing.

Outputs (into work/):
  iat.json        IAT slot VA -> "DLL!Function"
  functions.json  function start VA -> {callers: n, strings: [...], apis: [...]}
  strings.json    string VA -> text (rdata/data only)

Stdlib only, so it runs on the stock macOS python3.
Usage: python3 tools/pe_index.py
"""
import textio  # noqa: F401  (first: UTF-8, LF text files on Windows)
import json
import re
import struct
from bisect import bisect_right
from collections import defaultdict
from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parent.parent
EXE = ROOT / "Sheep, Dog 'n' Wolf (PAL Version)" / "SheepD3D.exe"
ASM = ROOT / "work" / "SheepD3D.asm"
OUT = ROOT / "work"

# DLLs imported by ordinal only; names from the DirectX 8 SDK import libs.
ORDINALS = {
    ("DSOUND.dll", 1): "DirectSoundCreate",
    ("DSOUND.dll", 2): "DirectSoundEnumerateA",
    ("DSETUP.dll", 11): "DirectXSetupGetVersion",
}


class PE:
    def __init__(self, path):
        self.d = d = path.read_bytes()
        pe = struct.unpack_from("<I", d, 0x3C)[0]
        nsec, = struct.unpack_from("<H", d, pe + 6)
        optsz, = struct.unpack_from("<H", d, pe + 20)
        opt = pe + 24
        self.base, = struct.unpack_from("<I", d, opt + 28)
        self.dirs = [struct.unpack_from("<II", d, opt + 96 + i * 8) for i in range(16)]
        self.secs = []
        for i in range(nsec):
            name, vs, va, rs, rp = struct.unpack_from("<8sIIII", d, opt + optsz + i * 40)
            self.secs.append((name.rstrip(b"\0").decode(), va, vs, rp, rs))

    def off(self, rva):
        for _, va, vs, rp, rs in self.secs:
            if va <= rva < va + max(vs, rs):
                return rva - va + rp
        raise ValueError(hex(rva))

    def cstr(self, o):
        return self.d[o:self.d.index(b"\0", o)].decode("latin-1")

    def imports(self):
        iat = {}
        o = self.off(self.dirs[1][0])
        while True:
            oft, _, _, nm, ft = struct.unpack_from("<IIIII", self.d, o)
            if not nm:
                break
            dll = self.cstr(self.off(nm))
            t = self.off(oft or ft)
            slot = ft
            while True:
                v, = struct.unpack_from("<I", self.d, t)
                if not v:
                    break
                if v & 0x80000000:
                    fn = ORDINALS.get((dll, v & 0xFFFF), "#%d" % (v & 0xFFFF))
                else:
                    fn = self.cstr(self.off(v) + 2)
                iat[self.base + slot] = "%s!%s" % (dll, fn)
                t += 4
                slot += 4
            o += 20
        return iat

    def strings(self, minlen=5):
        out = {}
        for name, va, vs, rp, rs in self.secs:
            if name not in (".rdata", ".data"):
                continue
            blob = self.d[rp:rp + rs]
            for m in re.finditer(rb"[\x09\x0a\x0d\x20-\x7e\x80-\xff]{%d,}\x00" % minlen, blob):
                out[self.base + va + m.start()] = m.group()[:-1].decode("latin-1")
        return out


def main():
    pe = PE(EXE)
    iat = pe.imports()
    strings = pe.strings()

    line_re = re.compile(r"^\s*([0-9a-f]+):\s+(\S+)\s*(.*)$")
    imm_re = re.compile(r"0x([0-9a-f]{6,8})")
    insns = []
    for line in ASM.open():
        m = line_re.match(line)
        if m:
            insns.append((int(m.group(1), 16), m.group(2), m.group(3)))

    text_lo = pe.base + pe.secs[0][1]
    text_hi = text_lo + pe.secs[0][2]

    starts = defaultdict(int)
    for _, mn, ops in insns:
        if mn == "call":
            # direct calls print as "0x401093 <.text+0x93>"; indirect ones start with a register/ptr
            m = re.match(r"0x([0-9a-f]+)\b", ops.strip())
            if m:
                t = int(m.group(1), 16)
                if text_lo <= t < text_hi:
                    starts[t] += 1
    order = sorted(starts)

    funcs = {a: {"callers": starts[a], "strings": [], "apis": []} for a in order}
    for addr, mn, ops in insns:
        i = bisect_right(order, addr) - 1
        if i < 0:
            continue
        f = funcs[order[i]]
        for m in imm_re.finditer(ops):
            v = int(m.group(1), 16)
            if v in iat and iat[v] not in f["apis"]:
                f["apis"].append(iat[v])
            elif v in strings and strings[v] not in f["strings"]:
                f["strings"].append(strings[v])

    OUT.mkdir(exist_ok=True)
    (OUT / "iat.json").write_text(json.dumps({"%08x" % k: v for k, v in sorted(iat.items())}, indent=1))
    (OUT / "strings.json").write_text(json.dumps({"%08x" % k: v for k, v in sorted(strings.items())}, indent=1))
    (OUT / "functions.json").write_text(json.dumps({"%08x" % k: v for k, v in funcs.items()}, indent=1))
    named = sum(1 for f in funcs.values() if f["strings"] or f["apis"])
    print("instructions: %d" % len(insns))
    print("IAT slots:    %d" % len(iat))
    print("strings:      %d" % len(strings))
    print("functions (direct-call targets): %d, with string/API anchors: %d" % (len(funcs), named))


if __name__ == "__main__":
    main()
