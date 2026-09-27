#!/usr/bin/env python3
"""Rebuild the game's compiled resource file (.res) from the .rsrc section of the player's own SheepD3D.exe.

The build links resources as a .res (LINK runs CVTRES itself). The resources are the game's icons, dialogs and strings,
so they are taken from the exe at build time instead of being committed. CVTRES sorts the resource directory by
type/name/language but lays the payloads out in input order, so the records are written in the original payload order
(by data address). A PE keeps no .res memory flags or DataVersion; the conventional 0x1030 is used, which does not
reach the output.

  python3 tools/exe_res.py [OUT.res]        default work/link/SheepD3D.res
"""
import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import match  # noqa: E402

ROOT = Path(__file__).resolve().parents[1]


def extract(exe_path=match.EXE):
    raw = Path(exe_path).read_bytes()
    pe = struct.unpack_from("<I", raw, 0x3c)[0]
    nsec, opt_size = struct.unpack_from("<H", raw, pe + 6)[0], struct.unpack_from("<H", raw, pe + 20)[0]
    secs = []
    for i in range(nsec):
        o = pe + 24 + opt_size + 40 * i
        name = raw[o:o + 8].rstrip(b"\0").decode()
        vsize, rva, rsize, roff = struct.unpack_from("<IIII", raw, o + 8)
        secs.append((name, rva, vsize, roff, rsize))
    def at(rva, n):
        for name, srva, vsize, roff, rsize in secs:
            if srva <= rva < srva + max(vsize, rsize):
                return raw[roff + rva - srva:roff + rva - srva + n]
        raise ValueError("RVA 0x%x in no section" % rva)
    base = next(s[1] for s in secs if s[0] == ".rsrc")
    leaves = []
    def key(v):
        if not v & 0x80000000:
            return v
        q = base + (v & 0x7fffffff)
        n = struct.unpack("<H", at(q, 2))[0]
        return at(q + 2, 2 * n).decode("utf-16-le")
    def walk(off, path):
        named, ids = struct.unpack("<HH", at(base + off + 12, 4))
        for i in range(named + ids):
            k, dst = struct.unpack("<II", at(base + off + 16 + 8 * i, 8))
            if dst & 0x80000000:
                walk(dst & 0x7fffffff, path + [key(k)])
            else:
                rva, size = struct.unpack("<II", at(base + dst, 8))
                leaves.append((rva, path + [key(k)], at(rva, size)))
    walk(0, [])
    def ident(v):
        return struct.pack("<HH", 0xffff, v) if isinstance(v, int) else v.encode("utf-16-le") + b"\0\0"
    def record(typ, name, lang, data):
        h = ident(typ) + ident(name)
        h += b"\0" * (-len(h) % 4)
        h += struct.pack("<IHHII", 0, 0x1030, lang, 0, 0)
        out = struct.pack("<II", len(data), 8 + len(h)) + h + data
        return out + b"\0" * (-len(out) % 4)
    res = record(0, 0, 0, b"")
    for rva, (typ, name, lang), data in sorted(leaves, key=lambda x: x[0]):
        res += record(typ, name, lang, data)
    return res, len(leaves)


def main():
    out = Path(sys.argv[1]) if len(sys.argv) > 1 else ROOT / "work" / "link" / "SheepD3D.res"
    out.parent.mkdir(parents=True, exist_ok=True)
    res, n = extract()
    out.write_bytes(res)
    print("%s: %d resources, %d bytes" % (out, n, len(res)))


if __name__ == "__main__":
    main()
