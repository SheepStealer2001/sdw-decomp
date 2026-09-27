#!/usr/bin/env python3
"""Object layout status: every object of the map checked with tools/layout.py's per-object mode.

Each map object is checked through the compiled object of its own src/ file (the map entry's "file"; every src/ file
is exactly one object). Reports IDENTICAL / DIFFERS / NO OBJECT per object, with the first difference per section.
Objects are compiled beforehand (tools/vc6.py); this only reads work/match/*.obj.

  python3 tools/tu_status.py                 summary + the objects still to do
  python3 tools/tu_status.py --json FILE     also write per-object results
  python3 tools/tu_status.py T077 T150       just these objects, with details
"""
import argparse
import collections
import json
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import layout  # noqa: E402
import match   # noqa: E402

ROOT = Path(__file__).resolve().parents[1]
MAP = ROOT / "data" / "tu_map.json"


def obj_for(path):
    p = (ROOT / path).resolve()
    return ROOT / "work" / "match" / ("_".join(p.relative_to(ROOT).with_suffix("").parts) + ".obj")


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("ids", nargs="*")
    ap.add_argument("--map", type=Path, default=MAP)
    ap.add_argument("--json", type=Path)
    a = ap.parse_args()
    tu_map = json.load(open(a.map))
    tu_map = tu_map["objects"] if isinstance(tu_map, dict) else tu_map
    exe = match.Exe(match.EXE)
    rows = []
    for o in tu_map:
        if a.ids and o["id"] not in a.ids:
            continue
        cand, how = None, None
        if o.get("file") and obj_for(o["file"]).exists() and (ROOT / o["file"]).exists():
            cand, how = obj_for(o["file"]), "own file"
        if cand is None or not cand.exists():
            rows.append({"id": o["id"], "status": "NO OBJECT",
                         "how": "not compiled (python3 tools/vc6.py %s)" % (o.get("file") or "<its file>")})
            continue
        ok, lines = layout.tu_check(o["id"], cand, tu_map, exe, verbose=bool(a.ids))
        rows.append({"id": o["id"], "status": "IDENTICAL" if ok else "DIFFERS", "how": how, "obj": str(cand.relative_to(ROOT)),
                     "lines": lines})
    c = collections.Counter(r["status"] for r in rows)
    for r in rows:
        if a.ids or r["status"] != "IDENTICAL":
            print("%-5s %-10s %s" % (r["id"], r["status"], r.get("how")))
            if a.ids:
                for l in r.get("lines", []):
                    print("      " + l)
    print("== %d objects: %s" % (len(rows), ", ".join("%d %s" % (v, k) for k, v in sorted(c.items()))))
    if a.json:
        a.json.write_text(json.dumps(rows, indent=1))


if __name__ == "__main__":
    main()
