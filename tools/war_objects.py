#!/usr/bin/env python3
"""List the scenaric objects stored in a level's .WAR file.

WAR layout used here (consistent with the loader near 0x50df00):
    +0x04 'V2.6'   +0x0C u32 resource count   +0x10 u32[count] entries = (type << 24) | fileOffset
    type 5 = scenaric record: u16, u16, s16 pos[3] (+4: x, y, z), u16 classId (+0x0A), s16 rot[3] (+0x0C), u16, properties (+0x14)
Positions are printed in MEMORY order: x (+4 / object +0x0C), y (+6 / +0x0E, points DOWN), z (+8 / +0x10). Other sources
name the axes differently: "X, Y, Z" in this memory order, or "Z" for the vertical axis (X=+0x0C, Z=+0x0E, Y=+0x10).

Usage:
    python3 tools/war_objects.py Lvl-06          # list every object in one level
    python3 tools/war_objects.py Lvl-06 seesaw   # ... filtered by class name substring
    python3 tools/war_objects.py Lvl-06 Sam --props   # ... with the designer properties, named from the disc's Scenaric_Classes.h
    python3 tools/war_objects.py all seesaw           # ... across every level (the level's in-game name shown alongside)

Properties are raw 4-byte slots (types not yet known): printed as signed ints. Values that look like small ids are usually references to
other WAR resources (boxes, trajectories, objects) - e.g. Sam's GREENZONEBOX = the export id of his green-zone box list.
"""
import json
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
LEVELS = ROOT / "Sheep, Dog 'n' Wolf (PAL Version)" / "Levels"

# In-game level name -> disc folder. B1 sits after level 4 and B2 after level 8 in the game's own numbering (the level
# labels 0x57b590).
LEVEL_TO_DISC = {**{"Level %d" % n: "Lvl-%02d" % n for n in range(0, 5)}, "Level B1": "Lvl-05",
                 **{"Level %d" % n: "Lvl-%02d" % (n + 1) for n in range(5, 9)}, "Level B2": "Lvl-10",
                 **{"Level %d" % n: "Lvl-%02d" % (n + 2) for n in range(9, 15)}}


def class_schema():
    p = ROOT / "work" / "scenaric_classes.json"
    return {c["id"]: c for c in json.loads(p.read_text())} if p.exists() else {}


def class_names():
    return {k: v["name"] for k, v in class_schema().items()}


def war_path(folder):
    d = LEVELS / folder
    hits = [f for f in d.iterdir() if f.suffix.lower() == ".war"]
    return hits[0]


def objects(folder):
    d = war_path(folder).read_bytes()
    assert d[4:8] == b"V2.6", "unexpected WAR version"
    count, = struct.unpack_from("<I", d, 0x0C)
    out = []
    for i in range(count):
        e, = struct.unpack_from("<I", d, 0x10 + i * 4)
        if e >> 24 != 5:
            continue
        off = e & 0xFFFFFF
        x, v, z = struct.unpack_from("<3h", d, off + 4)
        cid, = struct.unpack_from("<H", d, off + 0x0A)
        rot = struct.unpack_from("<3h", d, off + 0x0C)
        out.append({"res": i, "offset": off, "class": cid, "x": x, "y": v, "z": z, "rot": rot})
    return out


def props_of(folder, o, schema):
    c = schema.get(o["class"])
    if not c or not c["size"]:
        return []
    d = war_path(folder).read_bytes()
    return [(p["name"], struct.unpack_from("<i", d, o["offset"] + 0x14 + p["offset"])[0]) for p in c["props"]]


def main():
    names = class_names()
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    show_props = "--props" in sys.argv
    if not args:
        raise SystemExit(__doc__)
    schema = class_schema()
    comm = {v: k for k, v in LEVEL_TO_DISC.items()}
    folders = sorted(f.name for f in LEVELS.iterdir() if f.name.startswith("Lvl-")) if args[0] == "all" else [args[0]]
    flt = args[1].lower() if len(args) > 1 else ""
    for folder in folders:
        rows = [o for o in objects(folder) if flt in names.get(o["class"], "?").lower()]
        if not rows:
            continue
        print("%s  (%s)  = %s" % (folder, war_path(folder).name, comm.get(folder, "Planet X" if folder == "Lvl-17" else "level ?")))
        print("%5s %8s %-22s %7s %7s %7s   rot" % ("res", "offset", "class", "x", "y", "z"))
        for o in rows:
            print("%5d %8x %3d %-18s %7d %7d %7d   %s" % (o["res"], o["offset"], o["class"], names.get(o["class"], "?"), o["x"], o["y"], o["z"], o["rot"]))
            if show_props:
                pr = props_of(folder, o, schema)
                for i in range(0, len(pr), 4):
                    print("          " + "   ".join("%s=%d" % kv for kv in pr[i:i + 4]))


if __name__ == "__main__":
    main()
