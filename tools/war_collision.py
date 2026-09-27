#!/usr/bin/env python3
"""Decode a level's static collision mesh from its .WAR file and classify surfaces the way the movement code does.

Format (the triangle record layout worked out from the Lvl-00 bytes):
  resource type 0x80 = grid: u16 nx, u16 nz, s16 originX, s16 originZ, s16 cellX, s16 cellZ, then for z in 0..nz, x in 0..nx: u32 count + count*u32 triangle indices
  resource type 0x81 = triangles, 0x28 bytes each, 20 x s16:
      [0..2] bbox min (x, y, z)   [3..5] bbox max      world space, vertical points DOWN
      [6..8] normal, 4.12 fixed point, world space: flat floor = (0,-4096,0); n.y < 0 faces up, > 0 faces down (overhang/ceiling)
      [9] pad   [10..18] three vertices stored as (x, -y, -z)   [19] flags
Surface classes (thresholds from the per-surface block at 0x6cf280, bank 0 normal surface; the class-2 rule is inferred):
  floor      n.y <  -2922   (< ~44.5 deg from horizontal)  no slope timer
  slope      -2922 <= n.y < -2048                           slope timer runs (slide), jumping still allowed
  steep      -2048 <= n.y < -1400                           no ground jump (S+4 = 2048)
  wall-back  -1400 <= n.y <  0      "class 2" contact, wall leaning AWAY from the player
  wall       n.y == 0
  overhang   n.y >  0               a ceiling contact
Angles are degrees from horizontal: acos(-n.y / 4096).

Usage:
  python3 tools/war_collision.py Lvl-00                          # census of surface classes
  python3 tools/war_collision.py Lvl-00 --near -8883 25 --radius 700 [--y 156] [--classes wall-back,steep]
       (--near takes x and z, memory order; vertical is optional and only used for sorting/printing the height difference)
"""
import argparse
import math
import struct
from collections import Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
LEVELS = ROOT / "Sheep, Dog 'n' Wolf (PAL Version)" / "Levels"


def classify(nv):
    if nv < -2922:
        return "floor"
    if nv < -2048:
        return "slope"
    if nv < -1400:
        return "steep"
    if nv < 0:
        return "wall-back"
    if nv == 0:
        return "wall"
    return "overhang"


def load(folder):
    war = [f for f in (LEVELS / folder).iterdir() if f.suffix.lower() == ".war"][0]
    d = war.read_bytes()
    count, = struct.unpack_from("<I", d, 0x0C)
    ents = [struct.unpack_from("<I", d, 0x10 + i * 4)[0] for i in range(count)]
    offs = sorted(e & 0xFFFFFF for e in ents) + [len(d)]
    tri_off = next(e & 0xFFFFFF for e in ents if e >> 24 == 0x81)
    grid_off = next(e & 0xFFFFFF for e in ents if e >> 24 == 0x80)
    ntri = (offs[offs.index(tri_off) + 1] - tri_off) // 0x28
    tris = []
    for i in range(ntri):
        r = struct.unpack_from("<20h", d, tri_off + i * 0x28)
        verts = [(r[10 + k * 3], -r[11 + k * 3], -r[12 + k * 3]) for k in range(3)]
        tris.append({"i": i, "min": r[0:3], "max": r[3:6], "n": r[6:9], "verts": verts, "flags": r[19] & 0xFFFF,
                     "cls": classify(r[7]), "angle": math.degrees(math.acos(max(-1.0, min(1.0, -r[7] / 4096.0))))})
    grid = struct.unpack_from("<2H4h", d, grid_off)
    return war.name, grid, tris


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("level")
    ap.add_argument("--near", nargs=2, type=int, metavar=("X", "Y2"))
    ap.add_argument("--y", type=int)
    ap.add_argument("--radius", type=int, default=600)
    ap.add_argument("--classes", default="wall-back")
    a = ap.parse_args()
    name, grid, tris = load(a.level)
    print("%s: %d collision triangles, grid %dx%d origin (%d,%d) cell %dx%d" % ((name, len(tris)) + grid))
    if not a.near:
        c = Counter(t["cls"] for t in tris)
        for k in ("floor", "slope", "steep", "wall-back", "wall", "overhang"):
            print("  %-10s %6d" % (k, c.get(k, 0)))
        wb = [t for t in tris if t["cls"] == "wall-back"]
        hist = Counter(int(t["angle"] // 2) * 2 for t in wb)
        print("  wall-back by angle from horizontal (deg): " + ", ".join("%d-%d: %d" % (k, k + 2, hist[k]) for k in sorted(hist)))
        return
    x, z = a.near
    want = set(a.classes.split(","))
    hits = []
    for t in tris:
        if t["cls"] not in want:
            continue
        dx = max(t["min"][0] - x, 0, x - t["max"][0])
        dz = max(t["min"][2] - z, 0, z - t["max"][2])
        dist = math.hypot(dx, dz)
        if dist <= a.radius:
            hits.append((dist, t))
    hits.sort(key=lambda h: h[0])
    print("%d %s triangle(s) within %d of (x=%d, z=%d)%s" % (len(hits), "/".join(sorted(want)), a.radius, x, z,
                                                                 "" if a.y is None else ", reference vertical %d" % a.y))
    print("%6s %5s %-9s %6s  %-22s %-24s %-24s %s" % ("tri", "dist", "class", "angle", "normal (x,y,z)", "bbox min (x,y,z)", "bbox max", "faces (heading from wall, 4096/turn)"))
    for dist, t in hits[:60]:
        n = t["n"]
        heading = int(round(math.atan2(n[0], n[2]) * 4096 / (2 * math.pi))) & 4095
        print("%6d %5d %-9s %6.1f  %-22s %-24s %-24s %d" % (t["i"], dist, t["cls"], t["angle"], tuple(n), tuple(t["min"]), tuple(t["max"]), heading))


if __name__ == "__main__":
    main()
