#!/usr/bin/env python3
"""Build a FIXED SheepD3D.exe: the decompiled source with the fixes in fixes/ applied.

  python3 tools/build_fixed.py                  every fix in fixes/
  python3 tools/build_fixed.py --only NAME ...  only these fixes (their file names without .patch)
  python3 tools/build_fixed.py --list           the fixes, with the first line of each
  python3 tools/build_fixed.py --out FILE       where to write the exe (default work/link/SheepD3D_fixed.exe)

Each fix is a patch against the source (fixes/README.md). The patches are applied to a copy of src/, tools/ and data/
in work/fixed/, never to src/ itself, so the byte-identical build is untouched. The copy is compiled and linked with
tools/build_mod.py, which reports every function that no longer matches the original. The copy is taken from the files
as they are on disk, uncommitted edits included. To play the result, put it next to the game's own SheepD3D.exe under
its own name and run it from there.
"""
import textio  # noqa: F401  (first: UTF-8, LF text files on Windows)
import argparse
import re
import hashlib
import os
import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
FIXES = ROOT / "fixes"
COPY = ROOT / "work" / "fixed"
GAME = "Sheep, Dog 'n' Wolf (PAL Version)"
COPIED = ("src", "tools", "data")
PRIVATE = {"match", "link", "fixed"}          # work/ entries the copy keeps to itself


def link_dir(src, dst):
    try:
        os.symlink(src, dst, target_is_directory=True)
    except OSError:
        if sys.platform != "win32":
            raise
        import _winapi                         # a junction needs no special rights
        _winapi.CreateJunction(str(src), str(dst))


def link_file(src, dst):
    try:
        os.link(src, dst)
    except OSError:
        shutil.copy2(src, dst)


def is_link(path):
    return path.is_symlink() or bool(getattr(os.path, "isjunction", None) and os.path.isjunction(path))


def unlink_links(folder):
    """Remove the links in folder (not what they point to), so that the rest can be deleted."""
    if not folder.exists():
        return
    for entry in list(folder.iterdir()):
        if is_link(entry):
            try:
                entry.unlink()
            except OSError:
                os.rmdir(entry)                  # a junction


def first_line(patch):
    for line in patch.read_text().splitlines():
        if line.strip():
            return line.strip()
    return ""


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--only", nargs="+", metavar="NAME")
    ap.add_argument("--list", action="store_true")
    ap.add_argument("--out", type=Path, default=ROOT / "work" / "link" / "SheepD3D_fixed.exe")
    ap.add_argument("-j", "--jobs", type=int, default=6)
    a = ap.parse_args()
    fixes = sorted(FIXES.glob("*.patch"))
    if a.list:
        for p in fixes:
            print("%-32s %s" % (p.stem, first_line(p)))
        return
    if a.only:
        unknown = set(a.only) - {p.stem for p in fixes}
        if unknown:
            raise SystemExit("no such fix: %s (see --list)" % ", ".join(sorted(unknown)))
        fixes = [p for p in fixes if p.stem in a.only]
    if not fixes:
        raise SystemExit("no fixes to apply")

    # a fresh copy: the source, tools and tables copied; the game, the compiler and the indexes linked
    unlink_links(COPY / "work")
    unlink_links(COPY)
    if COPY.exists():
        shutil.rmtree(COPY)
    COPY.mkdir(parents=True)
    for d in COPIED:
        shutil.copytree(ROOT / d, COPY / d, ignore=shutil.ignore_patterns("__pycache__", "._*"))
    if (ROOT / GAME).exists():
        link_dir(ROOT / GAME, COPY / GAME)
    (COPY / "work").mkdir()
    for entry in (ROOT / "work").iterdir():
        if entry.name in PRIVATE or entry.name.startswith("public"):
            continue
        (link_dir if entry.is_dir() else link_file)(entry, COPY / "work" / entry.name)
    (COPY / "work" / "match").mkdir()

    # git must not look for a repository above the copy: inside one, it would skip paths outside the current folder
    env = dict(os.environ, GIT_CEILING_DIRECTORIES=str(COPY.parent))
    for p in fixes:
        r = subprocess.run(["git", "apply", "--whitespace=nowarn", str(p)], cwd=COPY, env=env, capture_output=True,
                           text=True)
        check = subprocess.run(["git", "apply", "--check", "--reverse", str(p)], cwd=COPY, env=env,
                               capture_output=True, text=True)
        if r.returncode or check.returncode:
            raise SystemExit("%s does not apply to this source:\n%s" % (p.name, (r.stderr or check.stderr).strip()))
        print("applied  %-32s %s" % (p.stem, first_line(p)), flush=True)

    a.out.parent.mkdir(parents=True, exist_ok=True)
    cmd = [sys.executable, str(COPY / "tools" / "build_mod.py"), "--out", str(a.out.resolve()), "-j", str(a.jobs)]
    r = subprocess.run(cmd, cwd=COPY).returncode
    if a.out.exists():
        write_symbol_file(a.out.resolve())
    sys.exit(r)


MAP_LINE = re.compile(r"^\s*[0-9a-f]{4}:[0-9a-f]{8}\s+(\S+)\s+([0-9a-f]{8})\s+(?:[fi]\s+)*(\S+)\s*$")


def map_addresses(path):
    """{(name, object): address} of every public and static symbol of a LINK /map file."""
    out = {}
    for line in path.read_text(errors="replace").splitlines():
        m = MAP_LINE.match(line)
        if m:
            out.setdefault((m.group(1), m.group(3)), int(m.group(2), 16))
    return out


def write_symbol_file(exe):
    """<exe>.sdwsym beside the exe, for the mod loader (mods/loader/loader.c, load_translation): this exe's SHA-1,
    then each address of the original (as the byte-identical build's map has it) and where the same symbol is in this
    exe (its own map), sorted. The loader runs mods on this exe with it."""
    original = ROOT / "work" / "link" / "SheepD3D.map"
    if not original.exists():
        print("no work/link/SheepD3D.map (tools/build_exe.py writes it): no symbol file for the mod loader")
        return
    old, new = map_addresses(original), map_addresses(exe.with_suffix(".map"))
    pairs = {}
    for key, addr in old.items():
        if key in new:
            pairs.setdefault(addr, new[key])
    sha = hashlib.sha1(exe.read_bytes()).hexdigest()
    lines = ["SDW symbols, SHA-1 %s (tools/build_fixed.py: original address, address in %s)" % (sha, exe.name)]
    lines += ["%08x %08x" % p for p in sorted(pairs.items())]
    exe.with_suffix(".sdwsym").write_text("\n".join(lines) + "\n")
    moved = sum(1 for a, b in pairs.items() if a != b)
    print("wrote %s: %d addresses, %d of them moved (mods run on this exe with it)" %
          (exe.with_suffix(".sdwsym").name, len(pairs), moved))


if __name__ == "__main__":
    main()
