#!/usr/bin/env python3
"""Print a C initializer for data in SheepD3D.exe, typed by the generated headers: the values a source file
needs to define a global in place.

  python3 tools/data_init.py 0x57618c EmitterDriftParams          one struct
  python3 tools/data_init.py 0x5758a0 IdleAnimEntry 5             an array of 5
  python3 tools/data_init.py 0x57b5c8 "char *" 33                 33 pointers (strings come out as literals)
  python3 tools/data_init.py 0x57ece0 s16 4097                    scalars

Struct layouts come from src/include/sdw_structs.h (every byte is a member there, padding included, so an initializer
built from it reproduces the bytes exactly). Pointers are written symbolically: a string as its literal, a function by
its table name, data as &name / &name[i] / ((u8 *)&name + off); anything unrecognised as a hex constant flagged
/* ?ptr */ for the writer to resolve. Floats print as the shortest literal that reads back to the same bits.
A C++ class with a vtable cannot be aggregate-initialised; define those objects with their constructors instead.
Read-only.
"""
import argparse
import json
import re
import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import match  # noqa: E402

ROOT = Path(__file__).resolve().parents[1]
HEADER = ROOT / "src" / "include" / "sdw_structs.h"
PRIM = {"u8": (1, "B"), "s8": (1, "b"), "char": (1, "b"), "bool8": (1, "B"), "u16": (2, "H"), "s16": (2, "h"),
        "u32": (4, "I"), "s32": (4, "i"), "int": (4, "i"), "BOOL": (4, "i"), "DWORD": (4, "I"), "LONG": (4, "i"),
        "float": (4, "f"), "f32": (4, "f"), "double": (8, "d"), "f64": (8, "d")}
FIELD = re.compile(r"^\s+(.+?)\s*(\b\w+)\s*((?:\[[^\]]+\])*)\s*(?::\s*(\d+))?\s*;\s*(?://\s*\+0x([0-9a-f]+))?")


def parse_structs():
    structs, cur = {}, None
    for line in open(HEADER, errors="replace"):
        m = re.match(r"^struct (\w+) \{", line)
        if m:
            cur = structs.setdefault(m.group(1), [])
            continue
        if cur is not None and line.startswith("};"):
            cur = None
            continue
        if cur is not None:
            f = FIELD.match(line)
            if f and not line.strip().startswith("//"):
                ctype, name, dims, bits, off = f.groups()
                dims = [int(d, 0) for d in re.findall(r"\[([^\]]+)\]", dims)]
                cur.append({"type": ctype.strip(), "name": name, "dims": dims, "bits": int(bits) if bits else None,
                            "off": int(off, 16) if off else None})
    return structs


class Emitter:
    def __init__(self):
        self.exe = match.Exe(match.EXE)
        self.structs = parse_structs()
        self.by_name, self.by_addr = match.load_symbols()
        sizes = json.load(open(ROOT / "data" / "function_sizes.json"))
        self.funcs = {int(k, 16) for k in sizes}
        # data symbols with a known extent, for interior pointers: address -> name (from the symbol tables)
        self.data_syms = sorted((a, n) for a, n in self.by_addr.items() if a >= 0x574000)

    def size_align(self, ctype):
        t = ctype.replace("struct ", "").strip()
        if t.endswith("*"):
            return 4, 4
        if t in PRIM:
            return PRIM[t][0], PRIM[t][0]
        if t in self.structs:
            size, align = 0, 1
            for f in self.structs[t]:
                if f["bits"] is not None:
                    fs, fa = self.size_align(f["type"])
                    off = f["off"] if f["off"] is not None else size
                    size = max(size, off + fs)
                    align = max(align, fa)
                    continue
                fs, fa = self.size_align(f["type"])
                n = 1
                for d in f["dims"]:
                    n *= d
                off = f["off"] if f["off"] is not None else (size + fa - 1) // fa * fa
                size = max(size, off + fs * n)
                align = max(align, fa)
            return (size + align - 1) // align * align, align
        raise SystemExit("unknown type %r (not a primitive or a struct in %s)" % (ctype, HEADER.name))

    # ---- values
    def c_float(self, v, double=False):
        bits = struct.pack("<d" if double else "<f", v)
        for prec in range(1, 18):
            s = "%.*g" % (prec, v)
            if struct.pack("<d" if double else "<f", float(s)) == bits:
                break
        if "e" not in s and "." not in s and "inf" not in s and "nan" not in s:
            s += ".0"
        return s + ("" if double else "f")

    def c_string(self, a):
        b = self.exe.read(a, 512) or b""
        n = b.find(b"\0")
        if n < 0:
            return None
        s = b[:n]
        if not all(32 <= c < 127 or c in (9, 10, 13) or c >= 0xa0 for c in s):
            return None
        out, after_hex = "", False
        for c in s:
            ch = chr(c)
            if after_hex and ch in "0123456789abcdefABCDEF":
                out += '" "'            # a \xNN escape swallows every following hex digit: close and reopen the literal
            esc = {"\\": "\\\\", '"': '\\"', "\n": "\\n", "\r": "\\r", "\t": "\\t"}.get(ch)
            out += esc or (ch if 32 <= c < 127 else "\\x%02x" % c)
            after_hex = not esc and not 32 <= c < 127
        return '"%s"' % out

    def c_pointer(self, v):
        if v == 0:
            return "0"
        if v in self.funcs or (0x401000 <= v < 0x574000 and v in self.by_addr):
            return "%s /* fn 0x%x */" % (self.by_addr.get(v, "FUN_%06x" % v), v)
        if v in self.by_addr and v >= 0x574000:
            return "&%s" % self.by_addr[v]
        s = self.c_string(v) if 0x574000 <= v < 0x585000 else None
        if s is not None:
            return s
        prev = None
        for a, n in self.data_syms:
            if a > v:
                break
            prev = (a, n)
        if prev and v - prev[0] < 0x10000:
            return "((u8 *)&%s + 0x%x)" % (prev[1], v - prev[0])
        return "0x%x /* ?ptr */" % v

    def value(self, ctype, a, indent):
        t = ctype.replace("struct ", "").strip()
        if t.endswith("*"):
            return self.c_pointer(self.exe.u32(a))
        if t in PRIM:
            size, fmt = PRIM[t]
            v = struct.unpack("<" + fmt, self.exe.read(a, size))[0]
            if fmt in "fd":
                return self.c_float(v, fmt == "d")
            return str(v) if fmt.islower() or v < 0x10000 else "0x%x" % v
        return self.aggregate(t, a, indent)

    def aggregate(self, t, a, indent):
        parts = []
        pad = "    " * (indent + 1)
        bitacc = None
        for f in self.structs[t]:
            off = a + (f["off"] or 0)
            if f["bits"] is not None:
                # consecutive bitfields in one unit: emit each field's value in declaration order
                fs, _ = self.size_align(f["type"])
                unit = int.from_bytes(self.exe.read(a + f["off"], fs), "little")
                if bitacc is None or bitacc[0] != f["off"]:
                    bitacc = [f["off"], 0]
                v = (unit >> bitacc[1]) & ((1 << f["bits"]) - 1)
                bitacc[1] += f["bits"]
                parts.append("%s%d%s" % (pad, v, "  /* %s:%d */" % (f["name"], f["bits"])))
                continue
            bitacc = None
            parts.append(pad + self.array(f["type"], off, f["dims"], indent + 1) + "  /* %s */" % f["name"])
        return "{\n" + ",\n".join(parts) + "\n" + "    " * indent + "}"

    def array(self, ctype, a, dims, indent):
        if not dims:
            return self.value(ctype, a, indent)
        size, _ = self.size_align(ctype)
        stride = size
        for d in dims[1:]:
            stride *= d
        t = ctype.replace("struct ", "").strip()
        if t in ("u8", "s8") and len(dims) == 1:
            b = self.exe.read(a, dims[0])
            return "{" + ", ".join(str(x if t == "u8" else struct.unpack("b", bytes([x]))[0]) for x in b) + "}"
        items = [self.array(ctype, a + i * stride, dims[1:], indent + 1) for i in range(dims[0])]
        if all("\n" not in x for x in items) and sum(len(x) for x in items) < 100:
            return "{" + ", ".join(items) + "}"
        pad = "    " * (indent + 1)
        return "{\n" + ",\n".join(pad + x for x in items) + "\n" + "    " * indent + "}"


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("addr")
    ap.add_argument("ctype")
    ap.add_argument("count", nargs="?", type=int)
    a = ap.parse_args()
    e = Emitter()
    addr = int(a.addr, 16)
    if a.count is None:
        print(e.value(a.ctype, addr, 0))
    else:
        print(e.array(a.ctype, addr, [a.count], 0))
    size, _ = e.size_align(a.ctype)
    print("/* %d bytes at 0x%x..0x%x */" % (size * (a.count or 1), addr, addr + size * (a.count or 1)))


if __name__ == "__main__":
    main()
