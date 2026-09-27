#!/usr/bin/env python3
"""Compare the array strides the CODE uses against the struct sizes the tables generate.

A struct that is too SHORT still compiles, still passes the offset asserts in work/layout_check.c (every field it does
know about is in the right place), and still lets every matched source file build; tools/check_symbol_prototypes.py
does not catch it either - that one compares argument counts.  For example, a NavNode generated as 0x68 bytes would
pass all of those, while every access in SamNav_BuildGraph indexes the array with `imul reg, reg, 0x70`.

The binary states the true stride out loud.  MSVC indexes an array of N-byte elements as `imul reg, reg, N` followed by
a memory operand based on that register, so for any global the tables type as `T[n]` the multiplier used to reach it is
sizeof(T) as the ORIGINAL compiler computed it.  If that disagrees with the size the CSVs produce, the CSV is wrong -
usually incomplete at the tail, which is the one error a layout check cannot see.

Field accesses count too: an operand like `[ecx + 0x6d0ce8]` reaches g_samNavNodes + 0x50, so any address inside the
array's extent is attributed to that array.

Read-only.  Usage:
  python3 tools/check_array_strides.py          # report disagreements
  python3 tools/check_array_strides.py --all    # also list arrays where no stride was found in the code
Exit status is 1 if any disagreement is found.
"""
import csv
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
ASM = ROOT / "work" / "SheepD3D.asm"
LAYOUT = ROOT / "work" / "layout_check.c"
TIERS = ["data/symbols.csv", "data/symbols_modules.csv", "data/symbols_auto.csv"]

SIZE_CHECK = re.compile(r"SDW_SIZE_CHECK\s*\(\s*(\w+)\s*,\s*(0x[0-9a-fA-F]+|\d+)\s*\)")
# `Type[123]` in a row's comment, with an optional leading const / pointer noise.
ARRAY = re.compile(r"\b([A-Z]\w*)\s*\[\s*(\d+)\s*\]")
IMUL = re.compile(r"^\s+([0-9a-f]+):\s+imul\s+(\w+),\s*\w+,\s*(0x[0-9a-f]+|\d+)")
# A power-of-two element is indexed with a SHIFT, never a multiply: `shl eax, 0x4` is stride 16. Without this rule
# every 2/4/8/16-byte array is missed, and a nearby imul for a different array can be attributed to it through the
# same register (g_defusePadSprites and the 0xc stride of g_defuseButtonTable are such a pair).
SHL = re.compile(r"^\s+([0-9a-f]+):\s+shl\s+(\w+),\s*(0x[0-9a-f]+|\d+)")
MEMOP = re.compile(r"\[(\w+) \+ (0x[0-9a-f]{6,8})\]")
LOOKBACK = 8            # instructions between the multiply and the access; MSVC /Od keeps them adjacent


def struct_sizes():
    """{type name: size in bytes}, from the generated asserts AND from the struct CSVs themselves.

    The asserts only cover the ~167 scenaric classes, which have a recorded total size.  Plain structs - NavNode,
    MenuHandler, the descriptor tables - have NO size assert at all, which is exactly why an incomplete one can sit
    there unnoticed.  For those, the size implied by the CSV is `max(offset + width)` rounded up to the widest field's
    alignment; if the code's stride is bigger than that, the CSV is missing tail fields, which is the finding.
    """
    out = {}
    if LAYOUT.exists():
        out.update({m.group(1): int(m.group(2), 0) for m in SIZE_CHECK.finditer(LAYOUT.read_text())})
    sys.path.insert(0, str(ROOT / "tools"))
    import structs_to_c as S                                   # its SIZES / parse_type, not its output

    def width(ctype):
        bf = re.fullmatch(r"(\w+)\s*:\s*\d+", ctype.strip())
        if bf:
            return S.SIZES.get(bf.group(1))
        base, dims = S.parse_type(ctype)
        size = 4 if base.endswith("*") else S.SIZES.get(base, out.get(base))
        if size is None:
            return None
        for d in dims:
            size *= d
        return size

    csvs = sorted((ROOT / "data" / "structs").glob("*.csv"))
    for _ in range(3):                                         # a few passes so nested structs resolve
        for path in csvs:
            name = path.stem
            if name in out:
                continue
            end, align = 0, 1
            complete = True
            for row in csv.DictReader(path.open()):
                off, ctype = row.get("offset", ""), row.get("ctype", "")
                if not off.startswith("0x"):
                    continue
                w = width(ctype)
                if w is None:
                    complete = False
                    continue
                end = max(end, int(off, 16) + w)
                align = max(align, min(4, w if w in (1, 2, 4) else 4))
            if complete and end:
                out[name] = (end + align - 1) // align * align
    return out


def arrays(sizes):
    """[(address, name, element type, count, element size)] for every global the tables type as an array."""
    out, seen = [], set()
    for tier in TIERS:
        path = ROOT / tier
        if not path.exists():
            continue
        for row in csv.DictReader(path.open()):
            addr, name, kind = row.get("address", ""), row.get("name", ""), row.get("kind", "")
            if kind != "data" or not addr.startswith("0x") or name in seen:
                continue
            m = ARRAY.search(row.get("comment") or "")
            if not m or m.group(1) not in sizes:
                continue
            seen.add(name)
            out.append((int(addr, 16), name, m.group(1), int(m.group(2)), sizes[m.group(1)]))
    return sorted(out)


def strides(targets):
    """{array name: {stride: first VA that used it}} by reading the disassembly."""
    if not ASM.exists():
        sys.exit("work/SheepD3D.asm is missing - regenerate it (see BUILDING.md)")
    spans = [(base, base + count * size, name) for base, name, _t, count, size in targets]
    found, recent = {}, []
    with ASM.open() as fh:
        for line in fh:
            m = IMUL.match(line)
            if m:
                recent.append((m.group(2), int(m.group(3), 0), m.group(1)))
                del recent[:-LOOKBACK]
                continue
            m = SHL.match(line)
            if m:
                recent.append((m.group(2), 1 << int(m.group(3), 0), m.group(1)))
                del recent[:-LOOKBACK]
                continue
            for reg, addr in MEMOP.findall(line):
                a = int(addr, 16)
                for lo, hi, name in spans:
                    if lo <= a < hi:
                        for i in range(len(recent) - 1, -1, -1):
                            rreg, k, va = recent[i]
                            if rreg == reg:
                                found.setdefault(name, {}).setdefault(k, va)
                                del recent[i]        # consumed: a scale feeds ONE access, so it must not be
                                break                # re-used for a later access through the same register
                        break
            if len(recent) > LOOKBACK:
                del recent[:-LOOKBACK]
    return found


def main():
    show_all = "--all" in sys.argv
    sizes = struct_sizes()
    targets = arrays(sizes)
    found = strides(targets)

    bad, ok, quiet = [], 0, []
    for base, name, etype, count, esize in targets:
        seen = found.get(name)
        if not seen:
            quiet.append((name, etype, esize))
            continue
        # The element stride is the multiplier actually used; several may appear if the code also indexes a sub-array.
        if esize in seen:
            ok += 1
        else:
            bad.append((name, hex(base), etype, esize, seen))

    for name, base, etype, esize, seen in bad:
        print("DISAGREES  %-34s %s" % (name, base))
        print("           the tables make %s %d (0x%x) bytes" % (etype, esize, esize))
        print("           the code indexes it with: %s"
              % ", ".join("%d (0x%x) at %s" % (k, k, va) for k, va in sorted(seen.items())))
        print("           read the bytes: a stride LARGER than the struct means the CSV is missing tail fields")
    if show_all:
        for name, etype, esize in quiet:
            print("NO STRIDE  %-34s %s is %d bytes; no imul-based access found" % (name, etype, esize))
    print("\n%d array(s) agree with the code, %d disagree, %d had no indexed access to check"
          % (ok, len(bad), len(quiet)))
    if not ok and not bad:
        print("WARNING: nothing was checked at all - treat this as a FAILED run, not a pass")
        return 1
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())
