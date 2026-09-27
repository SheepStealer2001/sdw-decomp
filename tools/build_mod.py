#!/usr/bin/env python3
"""Build a MODDED SheepD3D.exe from src/: the same compile and link as tools/build_exe.py, but a function that no longer
matches the original is reported, not fatal. Only a file that fails to compile stops the build.

  python3 tools/build_mod.py                 compile every source (vc6.py --all -j 6), then link, stamp and compare
  python3 tools/build_mod.py --out FILE      where to write the exe (default work/link/SheepD3D.exe)

The comparison at the end shows how far the build is from the original (for an unmodified tree: BYTE-IDENTICAL).
A new source file has to be added to data/tu_map.json at its place in link order to be linked. To play a build, copy it
next to the game's own SheepD3D.exe under another name and run it from there.
"""
import argparse
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--out", type=Path)
    ap.add_argument("-j", "--jobs", type=int, default=6)
    a = ap.parse_args()
    p = subprocess.run([sys.executable, str(ROOT / "tools" / "vc6.py"), "--all", "-j", str(a.jobs)], cwd=ROOT,
                       capture_output=True, text=True)
    out = p.stdout + p.stderr
    diffs = [l.strip() for l in out.splitlines() if re.match(r"\s+DIFF\s", l)]
    failed = [l.strip() for l in out.splitlines() if "compile failed" in l or "timed out" in l]
    total = next((l for l in out.splitlines() if l.startswith("== total over")), "")
    print(total or out[-2000:])
    if failed:
        print("\n".join(failed))
        raise SystemExit("stopped: %d file(s) failed to compile" % len(failed))
    if diffs:
        print("%d function(s) differ from the original (expected for a mod):" % len(diffs))
        for d in diffs[:40]:
            print("   " + d)
        if len(diffs) > 40:
            print("   ... and %d more" % (len(diffs) - 40))
    cmd = [sys.executable, str(ROOT / "tools" / "build_exe.py"), "--no-compile"] + (["--out", str(a.out)] if a.out else [])
    sys.exit(subprocess.run(cmd, cwd=ROOT).returncode)


if __name__ == "__main__":
    main()
