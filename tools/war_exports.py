#!/usr/bin/env python3
"""List a level's exported WAR resources by their original developer names - zones, trigger boxes, trajectories, objects.

The export table is WAR resource type 0x82: tag "Exp ----", u32 entry count, then entries {u16 id, u16 n, u32 fileOffset[n]}
(the table location checked on Lvl-03, where id 616 -> box at 0xDAA14).
Ids are global across levels and are named in the disc header Levels/Lvl-03/GameRes.h (WAR_IDO_*), e.g. 54 WATERBOX, 69 BOXSAMGREEN,
70 BOXSAMORANGE, 92 ICEBOX, 104 SLIDEBOX. Scenaric properties such as Sam's GREENZONEBOX hold these ids.

A target is printed as a box when it parses as one: u32 flags, s16 min[3], s16 max[3] with min <= max on every axis
(memory order x / y / z; vertical points DOWN, so "min y" is the TOP of the box). The flags word carries zone-type bits
(water boxes: 0x01000000 freezing, 0x02000000 and others).

Usage:
  python3 tools/war_exports.py Lvl-02                 # every export
  python3 tools/war_exports.py Lvl-02 SAM             # name filter (case-insensitive substring), or a numeric id
  python3 tools/war_exports.py all WATERBOX           # across all levels
"""
import re
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
LEVELS = ROOT / "Sheep, Dog 'n' Wolf (PAL Version)" / "Levels"
DISC_TO_LEVEL = {**{"Lvl-%02d" % n: "Level %d" % n for n in range(0, 5)}, "Lvl-05": "Level B1",
                 **{"Lvl-%02d" % (n + 1): "Level %d" % n for n in range(5, 9)}, "Lvl-10": "Level B2",
                 **{"Lvl-%02d" % (n + 2): "Level %d" % n for n in range(9, 15)}}


def names():
    out = {}
    h = (LEVELS / "Lvl-03" / "GameRes.h").read_text(encoding="latin-1")
    for m in re.finditer(r"#define\s+((?:WAR|DAV)_ID\w_\w+)\s+(\d+)", h):
        out.setdefault(int(m.group(2)), m.group(1))
    return out


def exports(folder):
    war = [f for f in (LEVELS / folder).iterdir() if f.suffix.lower() == ".war"][0]
    d = war.read_bytes()
    count, = struct.unpack_from("<I", d, 0x0C)
    off = next((e & 0xFFFFFF for e in struct.unpack_from("<%dI" % count, d, 0x10) if e >> 24 == 0x82), None)
    if off is None:
        return d, []
    n, = struct.unpack_from("<I", d, off)
    o, out = off + 4, []
    for _ in range(n):
        rid, cnt = struct.unpack_from("<2H", d, o)
        out.append((rid, struct.unpack_from("<%dI" % cnt, d, o + 4)))
        o += 4 + 4 * cnt
    return d, out


def as_box(d, off):
    if off + 16 > len(d):
        return None
    flags, = struct.unpack_from("<I", d, off)
    mn, mx = struct.unpack_from("<3h", d, off + 4), struct.unpack_from("<3h", d, off + 10)
    return (flags, mn, mx) if all(a <= b for a, b in zip(mn, mx)) else None


def main():
    if len(sys.argv) < 2:
        sys.exit(__doc__)
    nm = names()
    folders = sorted(f.name for f in LEVELS.iterdir() if f.name.startswith("Lvl-")) if sys.argv[1] == "all" else [sys.argv[1]]
    flt = sys.argv[2].lower() if len(sys.argv) > 2 else ""
    for folder in folders:
        d, ex = exports(folder)
        rows = [(rid, offs) for rid, offs in ex
                if not flt or (flt.isdigit() and int(flt) == rid) or flt in nm.get(rid, "").lower()]
        if not rows:
            continue
        print("%s = %s  (%d exports)" % (folder, DISC_TO_LEVEL.get(folder, "level ?"), len(ex)))
        for rid, offs in rows:
            print("  %4d %-28s %d target(s)" % (rid, nm.get(rid, "?"), len(offs)))
            boxy = "BOX" in nm.get(rid, "BOX")  # only *BOX exports (and unnamed ids) are box lists; SAMTRAJ etc. are other structures
            if not boxy:
                print("         (not a box list - %s; first target @%x)" % ("trajectory/node data" if "TRAJ" in nm.get(rid, "") else "other resource", offs[0]))
                continue
            for off in offs:
                b = as_box(d, off)
                if b:
                    print("         @%-7x box flags %08x   x %6d..%-6d y %6d..%-6d z %6d..%-6d" % (
                        off, b[0], b[1][0], b[2][0], b[1][1], b[2][1], b[1][2], b[2][2]))
                else:
                    print("         @%-7x (not a box)" % off)


if __name__ == "__main__":
    main()
