#!/usr/bin/env python3
"""Stand-in objects: reconstructions of original object files whose content is lost because the linker discarded all of
their code. The original link still felt them: they pulled library members and import thunks in a particular order,
their dead COMDATs took part in /OPT:ICF's choice of survivors, and they count in the Rich header. A stand-in reproduces
those side effects and must leave no byte in the exe: its code is removed by /OPT:REF or folded by /OPT:ICF, and it
defines no ordinary data or bss.

A stand-in is an entry of data/tu_map.json with "standin": true, placed in link order like any other object:
  {"id": "S01", "standin": true, "file": "src/standin/<name>.cpp", "language": "cpp", "reproduces": "<side effect>",
   "text_main": null, "rdata": null, "data": null, "bss": null, "text_comdats": []}
Its switches come from the file's match-flags line (or the folder's vc6.json), as for every source.

  python3 tools/standin.py add --after T036 --file src/standin/x.cpp --reproduces "..." [--id S01] [--map MAP]
  python3 tools/standin.py check [--map MAP]          compile every stand-in; fail on any retained (non-COMDAT) content
  python3 tools/standin.py build [--map MAP] [--no-compile] [--out EXE]
                                                      tools/build_exe.py's link with the objects of MAP (default the map)
"""
import argparse
import json
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import match  # noqa: E402
import vc6    # noqa: E402

ROOT = Path(__file__).resolve().parents[1]
MAP = ROOT / "data" / "tu_map.json"
KEEP_EMPTY = (".drectve", ".debug$S", ".debug$T", ".debug$F")


def load(path):
    return json.load(open(path))


def save(path, m):
    Path(path).write_text(json.dumps(m, indent=1) + "\n")


def cmd_add(a):
    m = load(a.map)
    objs = m["objects"]
    i = next((k for k, o in enumerate(objs) if o["id"] == a.after), None)
    if i is None:
        raise SystemExit("no object %s" % a.after)
    sid = a.id or "S%02d" % (1 + sum(1 for o in objs if o.get("standin")))
    if any(o["id"] == sid for o in objs):
        raise SystemExit("id %s exists" % sid)
    lang = "c" if a.file.endswith(".c") else "cpp"
    objs.insert(i + 1, {"id": sid, "standin": True, "file": a.file, "language": lang, "reproduces": a.reproduces,
                        "name_guess": None, "text_main": None, "rdata": None, "data": None, "bss": None,
                        "text_comdats": [], "xcu": [], "current_files": []})
    save(a.map, m)
    print("added %s after %s: %s" % (sid, a.after, a.file))


def retained(obj_path):
    """(problems, comdats, externals) of a compiled stand-in: ordinary sections with content are problems."""
    o = match.Obj(Path(obj_path))
    problems, comdats = [], []
    for sn in range(1, len(o.sections)):
        s = o.sections[sn]
        if s["name"].startswith(KEEP_EMPTY):
            continue
        if s["flags"] & 0x1000:
            comdats.append(s["name"])
            continue
        if s["size"]:
            problems.append("%s: %d bytes of ordinary %s content" % (Path(obj_path).name, s["size"], s["name"]))
    externals = sorted({s["name"] for s in o.symbols.values() if s["sec"] == 0 and s["cls"] == 2})
    return problems, comdats, externals


def cmd_check(a):
    m = load(a.map)["objects"]
    bad = 0
    for o in m:
        if not o.get("standin"):
            continue
        src = ROOT / o["file"]
        obj, flags = vc6.compile_one(src, [])
        if obj is None:
            print("%s %s: compile failed" % (o["id"], o["file"]))
            bad += 1
            continue
        problems, comdats, ext = retained(obj)
        bad += bool(problems)
        print("%s %-36s %s  COMDATs %d, externals %d%s" % (o["id"], o["file"], "OK" if not problems else "RETAINS CONTENT",
                                                            len(comdats), len(ext), "" if not problems else ": " + "; ".join(problems)))
    sys.exit(1 if bad else 0)


def cmd_build(a):
    import build_exe
    m = load(a.map)["objects"]
    def objects():
        out = []
        for o in m:
            obj = vc6.OUT / ("_".join(Path(o["file"]).with_suffix("").parts) + ".obj")
            if not obj.exists():
                raise SystemExit("no object for %s (%s): compile first" % (o["id"], o["file"]))
            out.append(obj)
        return out
    build_exe.objects = objects
    argv = ["build_exe.py"] + (["--no-compile"] if a.no_compile else []) + (["--out", str(a.out)] if a.out else [])
    sys.argv = argv
    build_exe.main()


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    sub = ap.add_subparsers(dest="cmd", required=True)
    p = sub.add_parser("add")
    p.add_argument("--after", required=True)
    p.add_argument("--file", required=True)
    p.add_argument("--reproduces", required=True)
    p.add_argument("--id")
    p.add_argument("--map", type=Path, default=MAP)
    p = sub.add_parser("check")
    p.add_argument("--map", type=Path, default=MAP)
    p = sub.add_parser("build")
    p.add_argument("--map", type=Path, default=MAP)
    p.add_argument("--no-compile", action="store_true")
    p.add_argument("--out", type=Path)
    a = ap.parse_args()
    {"add": cmd_add, "check": cmd_check, "build": cmd_build}[a.cmd](a)


if __name__ == "__main__":
    main()
