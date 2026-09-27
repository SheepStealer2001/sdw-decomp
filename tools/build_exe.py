#!/usr/bin/env python3
"""Build SheepD3D.exe from src/ with the original Microsoft tools, and compare it with the original byte for byte.

  python3 tools/build_exe.py                 compile every src/ file (vc6.py --all -j 6), link, stamp, compare
  python3 tools/build_exe.py --no-compile    link the objects already in work/match/
  python3 tools/build_exe.py --out FILE      where to write the exe (default work/link/SheepD3D.exe)

Where LINK runs (the compile step follows tools/vc6.py's own choice, with the same environment variables):
  * CrossOver (the default on macOS): a link bottle as described below. This path is unchanged.
  * plain Wine: set SDW_WINE to the wine binary. LINK runs in the prefix SDW_LINK_WINEPREFIX, else SDW_WINEPREFIX, else
    WINEPREFIX, else ~/.wine, with WINEDLLOVERRIDES=msvcrt=n,b; Microsoft's MSVCRT.DLL 6.0.8168 must be that prefix's
    32-bit msvcrt.dll (drive_c/windows/syswow64 in a 64-bit prefix, system32 in a 32-bit one). --install-msvcrt copies
    work/vc6/native/MSVCRT.DLL there, keeping Wine's own file as msvcrt.dll.wine-builtin; use a prefix kept for this.
    The loader trace is checked exactly as under CrossOver.
  * Windows: Windows always loads its own, newer msvcrt.dll for the name MSVCRT.DLL (a KnownDLL). So LINK runs from
    work/link/win/, which holds copies of LINK.EXE, MSPDB60.DLL and CVTRES.EXE whose import of MSVCRT.dll is renamed to
    MSVCR6.dll, next to a copy of work/vc6/native/MSVCRT.DLL named MSVCR6.DLL: the name is not a KnownDLL, so the loader
    takes the copy beside LINK. The copies are made on first use; the files in work/vc6/ are never changed.

Steps, each one measured:
  * objects: one per data/tu_map.json entry, in map order, which is the original link order;
  * resources: rebuilt from the player's own SheepD3D.exe by tools/exe_res.py and linked as a .res;
  * libraries: the DirectX 8.0 SDK import libraries and VC6 SP5's, dxguid.lib before WINMM.LIB (the order that gives the
    original Rich-header tuple order), LIBCMT (static CRT) and OLDNAMES, all with /nodefaultlib;
  * LINK 6.00.8447 must run on Microsoft's own MSVCRT 6.0.8168: /OPT:ICF picks the surviving copy of identical
    functions with the loaded runtime's qsort, and Wine's qsort orders equal keys differently. CrossOver treats msvcrt as
    a KnownDLL, so a separate link bottle (work/vc6/bottles/sdw-link, cloned from the compiler bottle on first use) holds
    the native DLL in its system folder; the loader trace is checked every time;
  * the file header's TimeDateStamp is set to the original's 0x3b601fc7 (2001-07-26 13:48:55 UTC).
Microsoft's files stay in work/vc6/ (never committed; SDW_VC6_DIR names another folder, and SDW_CROSSOVER_WINE another
CrossOver wine, exactly as for tools/vc6.py); see src/README.md for where they come from.
"""
import textio  # noqa: F401  (first: UTF-8, LF text files on Windows)
import argparse
import hashlib
import json
import os
import re
import shutil
import struct
import subprocess
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import exe_res  # noqa: E402
import match    # noqa: E402
import vc6      # noqa: E402

ROOT = Path(__file__).resolve().parents[1]
LINKDIR = ROOT / "work" / "link"
VC = vc6.VC6                     # work/vc6, or SDW_VC6_DIR
LINK_BOTTLE = "sdw-link"
NATIVE_MSVCRT = VC / "native" / "MSVCRT.DLL"
TIMESTAMP = 0x3b601fc7
ORIGINAL_SHA1 = "ca39374d53ae0030c5bd8c90dda45465e446dfe3"
OPTIONS = ["/nologo", "/machine:ix86", "/subsystem:windows,4.0", "/base:0x400000", "/fixed", "/incremental:no",
           "/opt:ref", "/opt:icf", "/align:4096", "/filealign:4096", "/stack:0x10000,0x1000", "/heap:0x100000,0x1000",
           "/nodefaultlib"]
SDK, VCLIB = VC / "dx8sdk" / "lib", VC / "vc98" / "LIB"
LIBRARIES = [SDK / "dsetup.lib", SDK / "ddraw.lib", SDK / "dinput.lib", SDK / "dinput8.lib", SDK / "dsound.lib",
             SDK / "dxguid.lib", VCLIB / "WINMM.LIB", VCLIB / "KERNEL32.LIB", VCLIB / "USER32.LIB", VCLIB / "GDI32.LIB",
             VCLIB / "ADVAPI32.LIB", SDK / "amstrmid.lib", SDK / "strmiids.lib", VCLIB / "SHELL32.LIB",
             VCLIB / "OLE32.LIB", VCLIB / "UUID.LIB", VC / "lib" / "LIBCMT.LIB", VCLIB / "OLDNAMES.LIB"]


def missing_inputs():
    """Every Microsoft file the link needs, that is not in work/vc6/ (see BUILDING.md for each one)."""
    need = [vc6.BIN / "LINK.EXE", vc6.BIN / "CVTRES.EXE", vc6.BIN / "MSPDB60.DLL", NATIVE_MSVCRT] + LIBRARIES
    return [p for p in need if not p.exists()]


def wine_prefix_msvcrt(install):
    """Plain Wine: the prefix LINK runs in, after checking (or, with --install-msvcrt, placing) Microsoft's MSVCRT.DLL
    as its 32-bit msvcrt.dll."""
    prefix = Path(os.environ.get("SDW_LINK_WINEPREFIX") or os.environ.get("SDW_WINEPREFIX") or os.environ.get("WINEPREFIX")
                  or Path.home() / ".wine")
    windir = prefix / "drive_c" / "windows"
    slot = windir / ("syswow64" if (windir / "syswow64").is_dir() else "system32") / "msvcrt.dll"
    want = hashlib.sha256(NATIVE_MSVCRT.read_bytes()).hexdigest()
    have = hashlib.sha256(slot.read_bytes()).hexdigest() if slot.exists() else None
    if have != want:
        if not install:
            raise SystemExit("%s is not Microsoft's MSVCRT 6.0.8168, which LINK needs for /OPT:ICF to match. Copy %s there "
                             "(or pass --install-msvcrt; use a prefix kept for this)" % (slot, NATIVE_MSVCRT))
        if slot.exists() and not slot.with_name("msvcrt.dll.wine-builtin").exists():
            shutil.copy2(slot, slot.with_name("msvcrt.dll.wine-builtin"))
        shutil.copy2(NATIVE_MSVCRT, slot)
        print("installed %s as %s" % (NATIVE_MSVCRT, slot))
    return prefix


def link_bottle():
    """The link bottle: a clone of the compiler bottle whose 32-bit msvcrt.dll is Microsoft's 6.0.8168."""
    dst = vc6.BOTTLES / LINK_BOTTLE
    if not dst.exists():
        print("creating the link bottle %s (one-time clone of %s)" % (dst, vc6.BOTTLE))
        shutil.copytree(vc6.BOTTLES / vc6.BOTTLE, dst, symlinks=True)
        slot = dst / "drive_c" / "windows" / "syswow64" / "msvcrt.dll"
        slot.unlink()
        shutil.copy2(NATIVE_MSVCRT, slot)
    return LINK_BOTTLE


def rename_crt_import(src, dst):
    """Copy a PE file, renaming its import of MSVCRT.dll to MSVCR6.dll (same length) and dropping the bound-import table,
    which would name the old DLL."""
    b = bytearray(src.read_bytes())
    pe = struct.unpack_from("<I", b, 0x3c)[0]
    opt = pe + 24
    dd = opt + (96 if struct.unpack_from("<H", b, opt)[0] == 0x10b else 112)
    secs = [struct.unpack_from("<IIII", b, opt + struct.unpack_from("<H", b, pe + 20)[0] + 40 * i + 8)
            for i in range(struct.unpack_from("<H", b, pe + 6)[0])]

    def off(rva):
        for vsize, va, rsize, raw in secs:
            if va <= rva < va + max(vsize, rsize):
                return rva - va + raw
        raise SystemExit("%s: RVA 0x%x is in no section" % (src, rva))

    p, n = off(struct.unpack_from("<I", b, dd + 8)[0]), 0
    while struct.unpack_from("<I", b, p + 12)[0]:
        o = off(struct.unpack_from("<I", b, p + 12)[0])
        e = b.index(0, o)
        if bytes(b[o:e]).lower() == b"msvcrt.dll":
            b[o:e] = b"MSVCR6.dll"
            n += 1
        p += 20
    if n != 1:
        raise SystemExit("%s: expected one import of MSVCRT.dll, found %d" % (src, n))
    struct.pack_into("<II", b, dd + 11 * 8, 0, 0)
    dst.write_bytes(bytes(b))


def windows_link_dir():
    """Windows: LINK, MSPDB60 and CVTRES importing MSVCR6.dll, beside Microsoft's MSVCRT 6.0.8168 under that name."""
    d = LINKDIR / "win"
    d.mkdir(parents=True, exist_ok=True)
    for name in ("LINK.EXE", "MSPDB60.DLL", "CVTRES.EXE"):
        src, dst = vc6.BIN / name, d / name
        if not dst.exists() or dst.stat().st_mtime < src.stat().st_mtime:
            rename_crt_import(src, dst)
    crt = d / "MSVCR6.DLL"
    if not crt.exists() or crt.read_bytes() != NATIVE_MSVCRT.read_bytes():
        shutil.copy2(NATIVE_MSVCRT, crt)
    return d


def objects():
    tu_map = json.load(open(ROOT / "data" / "tu_map.json"))["objects"]
    out = []
    for o in tu_map:
        obj = vc6.OUT / ("_".join(Path(o["file"]).with_suffix("").parts) + ".obj")
        if not obj.exists():
            raise SystemExit("no object for %s (%s): compile first" % (o["id"], o["file"]))
        out.append(obj)
    return out


def sections(raw):
    pe = struct.unpack_from("<I", raw, 0x3c)[0]
    nsec, opt = struct.unpack_from("<H", raw, pe + 6)[0], struct.unpack_from("<H", raw, pe + 20)[0]
    out = []
    for i in range(nsec):
        o = pe + 24 + opt + 40 * i
        vsize, rva, rsize, roff = struct.unpack_from("<IIII", raw, o + 8)
        out.append((raw[o:o + 8].rstrip(b"\0").decode(), rva, vsize, roff, rsize))
    return pe, out


def compare(built, original):
    a, b = original.read_bytes(), built.read_bytes()
    report = {"sha1": hashlib.sha1(b).hexdigest(), "identical": a == b, "size": [len(a), len(b)]}
    diff = [i for i in range(min(len(a), len(b))) if a[i] != b[i]]
    report["differing_bytes"] = len(diff) + abs(len(a) - len(b))
    report["first_difference"] = hex(diff[0]) if diff else None
    _, sa = sections(a)
    rows = []
    for name, rva, vsize, roff, rsize in sa:
        d = [i for i in range(roff, roff + rsize) if i < len(b) and a[i] != b[i]]
        rows.append({"section": name, "differing_bytes": len(d), "first": ("0x%x" % (0x400000 + rva + d[0] - roff)) if d else None})
    hdr = [i for i in range(0, sa[0][3]) if i < len(b) and a[i] != b[i]]
    rows.insert(0, {"section": "headers", "differing_bytes": len(hdr), "first": hex(hdr[0]) if hdr else None})
    report["sections"] = rows
    return report


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--no-compile", action="store_true")
    ap.add_argument("--out", type=Path, default=LINKDIR / "SheepD3D.exe")
    ap.add_argument("-j", "--jobs", type=int, default=6)
    ap.add_argument("--install-msvcrt", action="store_true", help="plain Wine only: put Microsoft's MSVCRT.DLL into the prefix")
    a = ap.parse_args()
    missing = missing_inputs()
    if missing:
        raise SystemExit("missing Microsoft files (see BUILDING.md):\n  " + "\n  ".join(str(m) for m in missing))
    LINKDIR.mkdir(parents=True, exist_ok=True)
    if not a.no_compile:
        r = subprocess.run([sys.executable, str(ROOT / "tools" / "vc6.py"), "--all", "-j", str(a.jobs)], cwd=ROOT)
        if r.returncode:
            raise SystemExit("compile step failed")
    res = LINKDIR / "SheepD3D.res"
    res.write_bytes(exe_res.extract()[0])
    out = a.out.resolve()
    rsp = LINKDIR / "link.rsp"
    lines = OPTIONS + ["/verbose", "/map:" + vc6.winpath(out.with_suffix(".map")), "/out:" + vc6.winpath(out)]
    lines += [vc6.winpath(o) for o in objects()] + [vc6.winpath(res)] + [vc6.winpath(l) for l in LIBRARIES]
    rsp.write_text("\r\n".join('"%s"' % l for l in lines) + "\r\n")
    if out.exists():
        out.unlink()
    link = [str(vc6.BIN / "LINK.EXE"), "@" + vc6.winpath(rsp)]
    if vc6.NATIVE:                                   # Windows: the copies that load MSVCRT 6.0.8168 as MSVCR6.DLL
        argv, env = [str(windows_link_dir() / "LINK.EXE")] + link[1:], dict(os.environ)
    elif vc6.PLAIN_WINE:                             # plain Wine: the native DLL in the prefix, msvcrt=n,b
        env = dict(os.environ, WINEPREFIX=str(wine_prefix_msvcrt(a.install_msvcrt)), WINEDLLOVERRIDES="msvcrt=n,b",
                   WINEDEBUG="+loaddll")
        argv = [vc6.PLAIN_WINE] + link
    else:                                            # CrossOver (default): the link bottle
        bottle = link_bottle()
        env = dict(os.environ, CX_BOTTLE_PATH=str(vc6.BOTTLES), CX_BOTTLE=bottle, WINEDEBUG="+loaddll")
        argv = [vc6.WINE, "--bottle", bottle, "--no-update", "--dll", "msvcrt=n,b", "--debugmsg", "+loaddll"] + link
    for k in ("CL", "_CL_", "LINK", "_LINK_", "LIB", "INCLUDE"):
        env.pop(k, None)
    p = subprocess.run(argv, cwd=LINKDIR, env=env, capture_output=True, text=True, errors="replace", timeout=900)
    log = p.stdout + p.stderr
    (LINKDIR / "link.log").write_text(log)
    native = re.search(r'Loaded L"[^"]*MSVCRT\.dll" at [0-9a-f]+: native', log, re.I)
    errors = [l for l in log.splitlines() if re.search(r"\b(LNK\d{4}|fatal error)\b", l)]
    if p.returncode or not out.exists():
        raise SystemExit("LINK failed (exit %d):\n%s" % (p.returncode, "\n".join(errors[:30])))
    if not vc6.NATIVE and not native:
        raise SystemExit("LINK did not run on the native MSVCRT (see %s): /OPT:ICF would not match" % (LINKDIR / "link.log"))
    raw = bytearray(out.read_bytes())
    pe = struct.unpack_from("<I", raw, 0x3c)[0]
    struct.pack_into("<I", raw, pe + 8, TIMESTAMP)
    out.write_bytes(bytes(raw))
    rep = compare(out, match.EXE)
    (LINKDIR / "compare.json").write_text(json.dumps(rep, indent=1) + "\n")
    print("linked %s (%s; %d warnings)" % (out, "MSVCRT 6.0.8168 as MSVCR6.DLL" if vc6.NATIVE else "native MSVCRT confirmed",
                                           sum(1 for l in errors if "warning" in l)))
    print("SHA-1 %s  %s" % (rep["sha1"], "== the original: BYTE-IDENTICAL" if rep["sha1"] == ORIGINAL_SHA1 else "(original %s)" % ORIGINAL_SHA1))
    if not rep["identical"]:
        print("%d bytes differ, first at file offset %s" % (rep["differing_bytes"], rep["first_difference"]))
        for r in rep["sections"]:
            print("  %-8s %9d differing bytes%s" % (r["section"], r["differing_bytes"], ("   first at %s" % r["first"]) if r["first"] else ""))


if __name__ == "__main__":
    main()
