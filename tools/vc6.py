#!/usr/bin/env python3
"""Compile source with the original toolchain and byte-match every function against SheepD3D.exe.

  python3 tools/vc6.py src/engine/approach.cpp               compile one file, one verdict line per function
  python3 tools/vc6.py --all                                 every src/**/*.c and *.cpp, with a total
  python3 tools/vc6.py FILE --diff Math_ApproachLinear       instruction diff for one function
  python3 tools/vc6.py FILE --flags "/Zp4"                   extra flags for an options experiment (appended)
  python3 tools/vc6.py --all -j 6                            compile six files at once (about 2 minutes instead of 80)

Toolchain:
VC6 SP5 CL 12.00.8804 / C1 12.00.8867 / C1XX 12.00.8964 + Processor Pack backend c2.dll 13.00.9044, MSPDB60 6.00.8168,
in work/vc6/bin/, run under CrossOver in the compiler-only bottle work/vc6/bottles/sdw-vc6 (never the game's SDW bottle).
There is no SDK or CRT header set: the sources use the stand-ins in src/sdk/.

RECIPE is the combined setting that reproduces the game's code. Every choice in it is
backed by at least one function it alone reproduces, except /Ob1 vs /Ob2 and /Zp8 vs /Zp4 outside the Timer layout, which
are not uniquely recovered. A file adds its own switches with a line `match-flags: /TP` in its first 40 lines (a .c
file that needs C++ syntax for a thiscall receiver is compiled /TP, keeping extern "C" names).

Objects and /FAcs listings go to work/match/. The verdicts are tools/match.py's (see there for MATCH / MATCH~ / DIFF).
A file whose globals have constructors lists its static-initialiser roots with `match-init: NAME ...` (tools/match.py).
`--all` first compiles the generated headers alone (their SDW_AT/SDW_SIZE checks are VC6's proof of every recorded field
offset), and also rewrites data/match_results.json: one row per function in src/ with its file, address and verdict, so
other tools can read the proof state without running the compiler. It is only written by a full
--all run with no --flags, so it is always a snapshot of the whole tree under the recipe.
"""
import textio  # noqa: F401  (first: UTF-8, LF text files on Windows)
import argparse, json, os, re, subprocess, sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import match  # noqa: E402
import sys

ROOT = Path(__file__).resolve().parents[1]
# The Microsoft toolchain folder: work/vc6 unless SDW_VC6_DIR names another (bin/, lib/, vc98/, dx8sdk/, native/, bottles/).
VC6 = ROOT / Path(os.environ["SDW_VC6_DIR"]).expanduser() if os.environ.get("SDW_VC6_DIR") else ROOT / "work" / "vc6"
BIN = VC6 / "bin"
BOTTLES = VC6 / "bottles"
BOTTLE = "sdw-vc6"
OUT = ROOT / "work" / "match"
RESULTS = ROOT / "data" / "match_results.json"
# CrossOver's wine; SDW_CROSSOVER_WINE overrides the default install location.
WINE = os.environ.get("SDW_CROSSOVER_WINE") or "/Applications/CrossOver.app/Contents/SharedSupport/CrossOver/bin/wine"
# How CL.EXE is launched. Default: CrossOver on macOS, in the compiler-only bottle above. Set SDW_WINE to a plain Wine
# binary (Linux, or macOS without CrossOver; SDW_WINEPREFIX optionally picks the prefix). On Windows CL.EXE runs natively.
PLAIN_WINE = os.environ.get("SDW_WINE")
NATIVE = sys.platform == "win32"
RECIPE = ["/c", "/MT", "/Od", "/Gd", "/G6", "/Ob1", "/GX-", "/Zp8", "/Op-", "/QIfdiv-"]


def winpath(p):
    if NATIVE:
        return str(Path(p).resolve())
    return "Z:" + str(Path(p).resolve()).replace("/", "\\")


def file_init_roots(src):
    """`match-init: NAME_OR_0xADDR ...` in the first 40 lines: the file's static-initialiser roots, in the order its
    globals with constructors are defined (VC6 emits one .CRT$XCU entry per such global, in that order). Names are
    looked up in the symbol tables, e.g. `match-init: Sound_StaticInit_BankWaves Sound_StaticInit_Channels`."""
    by_name, _ = match.load_symbols()
    out = []
    for line in src.read_text(errors="replace").splitlines()[:40]:
        m = re.search(r"match-init:\s*(.*?)\s*(\*/)?\s*$", line)
        if m:
            for tok in m.group(1).split():
                out.append(int(tok, 16) if tok.startswith("0x") else by_name[tok])
    return out


def file_addrs(src):
    """`match-addr: NAME=0xVA ...` in the first 40 lines: addresses for names the symbol tables cannot place, e.g. one
    constructor overload, whose decorated COFF name (`??0D3DApp@@QAE@PAUHWND__@@PAUHINSTANCE__@@@Z`) is unambiguous
    where the undecorated one is not. Same effect as match.py's --addr, but recorded in the file, so --all honours it."""
    out = {k: int(v, 16) for k, v in dir_config(src).get("addr", {}).get(Path(src).name, {}).items()}
    for line in src.read_text(errors="replace").splitlines()[:40]:
        m = re.search(r"match-addr:\s*(.*?)\s*(\*/)?\s*$", line)
        if m:
            for tok in m.group(1).split():
                if "=" not in tok:      # prose after the pins on the same line
                    break
                k, v = tok.split("=", 1)
                out[k] = int(v, 16)
    return out


def dir_config(src):
    """A folder's vc6.json, for sources that must stay unmodified (src/jpeg: IJG's own files): {"flags": [...],
    "system_include": "work/vc6/vc98/INCLUDE", "addr": {"file.c": {"COFF name": "0xVA", ...}}}. A file's own
    match-flags / match-addr lines take precedence."""
    cfg = Path(src).resolve().parent / "vc6.json"
    return json.load(open(cfg)) if cfg.exists() else {}


def file_flags(src):
    for line in src.read_text(errors="replace").splitlines()[:40]:
        m = re.search(r"match-flags:\s*(.*?)\s*(\*/)?\s*$", line)
        if m:
            return m.group(1).split()
    return list(dir_config(src).get("flags", []))


def compile_one(src, extra, include=None, out=print):
    if not (BIN / "CL.EXE").exists():
        raise SystemExit("no compiler in %s (see this file's docstring)" % BIN)
    OUT.mkdir(parents=True, exist_ok=True)
    stem = "_".join(src.resolve().relative_to(ROOT).with_suffix("").parts) if ROOT in src.resolve().parents else src.stem
    obj, asm = OUT / (stem + ".obj"), OUT / (stem + ".asm")
    for f in (obj, asm):
        if f.exists():
            f.unlink()
    flags = RECIPE + file_flags(src) + extra
    incs = ([include] if include else []) + [ROOT / "src" / "include"]
    if dir_config(src).get("system_include"):              # e.g. the real VC6 headers (Microsoft files, never committed)
        inc = dir_config(src)["system_include"]
        incs.append(VC6 / inc[len("work/vc6/"):] if inc.startswith("work/vc6/") else ROOT / inc)   # follows SDW_VC6_DIR
    cl_args = [str(BIN / "CL.EXE"), "/nologo", *flags,
               *["/I" + winpath(i) for i in incs], "/FAcs", "/Fa" + winpath(asm), "/Fo" + winpath(obj), winpath(src)]
    if NATIVE:
        argv, env = cl_args, dict(os.environ)
    elif PLAIN_WINE:
        argv = [PLAIN_WINE, *cl_args]
        env = dict(os.environ, WINEDEBUG="-all")
        if os.environ.get("SDW_WINEPREFIX"):
            env["WINEPREFIX"] = os.environ["SDW_WINEPREFIX"]
    else:
        argv = [WINE, "--bottle", BOTTLE, "--no-update", *cl_args]
        env = dict(os.environ, CX_BOTTLE_PATH=str(BOTTLES), CX_BOTTLE=BOTTLE, WINEDEBUG="-all")
    env.pop("CL", None)                                   # inherited CL/_CL_ would add switches silently
    env.pop("_CL_", None)
    for attempt in (1, 2):                               # a compile that hangs under Wine (seen once under load) is retried once
        try:
            p = subprocess.run(argv, cwd=OUT, env=env, capture_output=True, text=True, timeout=300)
            break
        except subprocess.TimeoutExpired:
            if attempt == 2:
                out("%s: compile timed out twice" % src)
                return None, flags
    text = "\n".join(l for l in (p.stdout + p.stderr).splitlines() if l.strip() and l.strip() != src.name)
    if p.returncode != 0 or not obj.exists():
        out("%s: compile failed (exit %d)\n%s" % (src, p.returncode, text))
        return None, flags
    if text:
        out(text)
    return obj, flags


def check_headers():
    """Compile the generated headers on their own: every SDW_AT / SDW_SIZE check in them is a VC6 layout proof."""
    OUT.mkdir(parents=True, exist_ok=True)
    bad = 0
    for name, text in (("_headers_check.cpp", '#include "sdw_classes.h"\n'),
                       ("_headers_check.c", '#include "sdw_structs.h"\n#include "sdw_enums.h"\n')):
        src = OUT / name
        src.write_text(text)
        obj, _ = compile_one(src, [])
        print("== headers (%s): %s" % ("sdw_classes.h" if name.endswith(".cpp") else "sdw_structs.h + sdw_enums.h",
                                       "compile, every layout check holds" if obj else "FAILED"))
        bad += obj is None
    return bad


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("files", nargs="*", type=Path)
    ap.add_argument("--all", action="store_true", help="every .c/.cpp under src/")
    ap.add_argument("--flags", default="", help="extra compiler switches, appended after the recipe")
    ap.add_argument("--diff", metavar="NAME", help="instruction diff for one function")
    ap.add_argument("--addr", action="append", default=[], metavar="NAME=0xVA")
    ap.add_argument("--include", metavar="DIR", help="search this include directory before src/include (for example "
                    "headers from `structs_to_c.py --overlay ... --out DIR`)")
    ap.add_argument("-v", "--verbose", action="store_true")
    ap.add_argument("-j", "--jobs", type=int, default=1, help="compile this many files at once (matching stays in order)")
    a = ap.parse_args()
    files = a.files
    if not (ROOT / "src" / "include" / "scenaric_props.h").exists():
        raise SystemExit("src/include/scenaric_props.h is missing: generate it from your copy of the game with "
                         "python3 tools/scenaric_to_c.py (BUILDING.md section 3)")
    if a.all:
        files = sorted(p for p in (ROOT / "src").rglob("*") if p.suffix in (".c", ".cpp"))
    if not files:
        ap.error("no source files")
    overrides = {kv.split("=", 1)[0]: int(kv.split("=", 1)[1], 16) for kv in a.addr}
    exe = match.Exe(match.EXE)
    total, failed = {}, 0
    if a.all:
        failed += check_headers()
    rows, file_rows = [], []
    compiled = {}
    if a.jobs > 1:                                      # compile in parallel up front; output and matching stay in file order
        import concurrent.futures
        def quiet(src):
            lines = []
            r = compile_one(src, a.flags.split(), a.include, out=lines.append)
            return r, "".join(l + "\n" for l in lines)
        with concurrent.futures.ThreadPoolExecutor(a.jobs) as ex:
            compiled = dict(zip(files, ex.map(quiet, files)))
    for src in files:
        if src in compiled:
            (obj, flags), out = compiled[src]
            if out:
                print(out, end="")
        else:
            obj, flags = compile_one(src, a.flags.split(), a.include)
        rel = src.resolve().relative_to(ROOT) if ROOT in src.resolve().parents else src
        print("== %s   (%s)" % (rel, " ".join(flags)))
        file_rows.append({"file": rel.as_posix() if hasattr(rel, "as_posix") else str(rel), "flags": flags, "compiled": obj is not None})
        if obj is None:
            failed += 1
            continue
        pinned = dict(file_addrs(src))                   # the file's own `match-addr:` pins, then the command line's
        pinned.update(overrides)
        results = match.Matcher(match.Obj(obj), exe, pinned, file_init_roots(src)).run(a.diff)
        for r in results:
            rows.append({"va": r.get("va"), "name": r["name"], "symbol": r["symbol"], "file": rel.as_posix() if hasattr(rel, "as_posix") else str(rel), "verdict": r["verdict"],
                         "bytes": r["span"], "original_size": r.get("original_size"),
                         "unresolved": sorted({x["symbol"] for x in r.get("relocations", []) if x["status"] in ("inferred", "renamed")})})
        counts = match.report(results, a.verbose, indent="  ")
        for k, v in counts.items():
            total[k] = total.get(k, 0) + v
        if a.diff and results and results[0].get("va"):
            match.show_diff(results[0], exe)
    if len(files) > 1:
        print("== total over %d files: %s%s" % (len(files), ", ".join("%d %s" % (v, k) for k, v in sorted(total.items())),
                                               ", %d failed to compile" % failed if failed else ""))
    if a.all and not a.flags and not a.diff and not overrides and not a.include:
        rows.sort(key=lambda r: (r["va"] is None, int(r["va"], 16) if r["va"] else 0, r["name"]))
        RESULTS.write_text(json.dumps({
            "about": "Written by `python3 tools/vc6.py --all`. One row per function compiled from src/. verdict: MATCH = every byte and "
                     "every relocation agree with SheepD3D.exe; MATCH~ = every byte agrees but some target address was only readable from "
                     "the exe (listed in unresolved: CRT helpers, or a name the symbol tables spell differently); DIFF = compiled but not "
                     "yet equal; NOADDR = a compiler-generated helper nothing refers to (no address, not a game function to count).",
            "recipe": RECIPE,
            "files": file_rows,
            "totals": dict(sorted(total.items())),
            "functions": rows}, indent=1) + "\n")
        print("wrote %s" % RESULTS.relative_to(ROOT))
    sys.exit(1 if failed or total.get("DIFF") else 0)


if __name__ == "__main__":
    main()
