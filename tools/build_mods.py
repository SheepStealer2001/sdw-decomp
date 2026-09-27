#!/usr/bin/env python3
"""Build the mod loader (a stand-in dinput8.dll) and the example mods with MinGW-w64 (32-bit).

  python3 tools/build_mods.py                 work/mods/dinput8.dll and work/mods/Mods/<Name>/<Name>.dll
  python3 tools/build_mods.py --cc PREFIX     the compiler prefix (default i686-w64-mingw32-, or SDW_MINGW)
  python3 tools/build_mods.py --check-hooks   check the loader's instruction-length decoder against the disassembly
                                              (work/SheepD3D.asm) at the start of every game function

To install: copy work/mods/dinput8.dll and the work/mods/Mods folder next to SheepD3D.exe (MODDING.md). The loader's
symbol table (name -> address, from data/symbols*.csv with the same precedence as the matcher: symbols.csv over
symbols_modules.csv over symbols_auto.csv) is generated into work/mods/build/symbols.c.
"""
import textio  # noqa: F401  (first: UTF-8, LF text files on Windows)
import make_model
import argparse
import csv
import os
import re
import shutil
import subprocess
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
MODS = ROOT / "mods"
OUT = ROOT / "work" / "mods"
TIERS = ["data/symbols_auto.csv", "data/symbols_modules.csv", "data/symbols.csv"]   # later tiers win
CNAME = re.compile(r"^[A-Za-z_][A-Za-z0-9_@?$]*$")


def symbols():
    by_name, by_addr = {}, {}
    for tier in TIERS:
        for r in csv.DictReader(open(ROOT / tier)):
            try:
                addr = int(r["address"], 16)
            except (ValueError, KeyError):
                continue
            name = r["name"].strip()
            if not CNAME.match(name):
                continue
            by_addr[addr] = name                  # a name a later tier replaces stays as well (an alias)
            by_name[name] = addr
    return published_names(by_name)


PUBLISHED = ROOT / "data" / "mod_symbols_published.csv"


def published_names(current):
    """Every name a loader has had stays, so that a mod keeps finding what it hooks when the decompilation renames a
    function: data/mod_symbols_published.csv (committed) lists each name and its address as first published; a name
    the tables no longer have is added back as an alias for that address, and new names are appended to the file."""
    old = {}
    if PUBLISHED.exists():
        for r in csv.DictReader(open(PUBLISHED)):
            old[r["name"]] = int(r["address"], 16)
    table = dict(current)
    for name, addr in old.items():
        if name not in table:
            table[name] = addr
        elif table[name] != addr:
            print("note: %s was 0x%x when first published, now 0x%x (the tables' correction wins)" %
                  (name, addr, table[name]))
    if set(table) - set(old):
        rows = sorted({**old, **{n: a for n, a in table.items() if n not in old}}.items())
        with open(PUBLISHED, "w", newline="") as f:
            f.write("name,address\n")
            f.writelines("%s,0x%x\n" % r for r in rows)
    return table


def scenaric_props():
    """(class id, offset, name) of every designer property, from src/include/scenaric_props.h (which
    tools/scenaric_to_c.py generates from the player's own disc); empty when it has not been generated."""
    h = ROOT / "src" / "include" / "scenaric_props.h"
    if not h.exists():
        return []
    text = h.read_text()
    ids = {m.group(1): int(m.group(2)) for m in re.finditer(r"CLASSID_(\w+)\s*=\s*(\d+)", text)}
    out = []
    for m in re.finditer(r"typedef struct (\w+)Props \{(.*?)\}", text, re.S):
        cid = ids.get(m.group(1).upper())
        if cid is None:
            continue
        for f in re.finditer(r"\b(\w+);\s*// \+0x([0-9a-fA-F]+)", m.group(2)):
            out.append((cid, int(f.group(2), 16), f.group(1)))
    return out


def class_pictures():
    """(class id, picture id) of every .DAV picture an object class's own code names (its DAV_IDI_ constants: the
    rocket's fuel gauge and its two icons), found as class_sounds finds sounds. An object brought to another level
    brings the ones that level lacks."""
    props = (ROOT / "src" / "include" / "scenaric_props.h").read_text()
    classes = {m.group(1): int(m.group(2), 0) for m in re.finditer(r"\b(CLASSID_\w+) = (\w+)", props)}
    pics = {m.group(1): int(m.group(2)) for m in re.finditer(r"\b(DAV_IDI_\w+) = (\d+)", props)}
    factories = {}
    for f in sorted((ROOT / "src" / "objects").glob("*.cpp")):
        for m in re.finditer(r"^\s*ScnObject \*(\w+_Create)\(void \*", f.read_text(errors="replace"), re.M):
            factories[m.group(1)] = f
    out = []
    reg = (ROOT / "src" / "engine" / "scn_register.cpp").read_text()
    for cname, factory in re.findall(r"Scenaric_RegisterClass_2\(\s*(CLASSID_\w+)\s*,\s*(\w+)", reg):
        f = factories.get(factory)
        if f and cname in classes:
            ids = {pics[s] for s in re.findall(r"\bDAV_IDI_\w+\b", f.read_text(errors="replace")) if s in pics}
            out += [(classes[cname], i) for i in sorted(ids)]
    return out


def class_sounds():
    """(class id, sound id) of every sound an object class's own code names: the SND_ constants in the file that
    defines its factory (the class registered with it in src/engine/scn_register.cpp). A superset of what it plays
    (a constant can sit in a table the class never reads), which is what bringing an object to another level wants:
    its sounds are added to that level's bank."""
    classes = {m.group(1): int(m.group(2), 0) for m in
               re.finditer(r"\b(CLASSID_\w+) = (\w+)", (ROOT / "src" / "include" / "scenaric_props.h").read_text())}
    snd = {m.group(1): int(m.group(2), 0) for m in
           re.finditer(r"\b(SND_\w+) = (0x[0-9a-fA-F]+|\d+)", (ROOT / "src" / "include" / "sdw_enums.h").read_text())}
    factories = {}
    for f in sorted((ROOT / "src" / "objects").glob("*.cpp")):
        for m in re.finditer(r"^\s*ScnObject \*(\w+_Create)\(void \*", f.read_text(errors="replace"), re.M):
            factories[m.group(1)] = f
    out = []
    reg = (ROOT / "src" / "engine" / "scn_register.cpp").read_text()
    for cname, factory in re.findall(r"Scenaric_RegisterClass_2\(\s*(CLASSID_\w+)\s*,\s*(\w+)", reg):
        f = factories.get(factory)
        if f and cname in classes:
            ids = {snd[s] for s in re.findall(r"\bSND_\w+\b", f.read_text(errors="replace")) if s in snd}
            out += [(classes[cname], i) for i in sorted(ids)]
    return out


def iat_symbols():
    """__imp_<name> -> the import-table slot of each imported function, from work/iat.json (tools/pe_index.py)."""
    p = ROOT / "work" / "iat.json"
    if not p.exists():
        return {}
    import json
    return {"__imp_" + v.split("!")[1]: int(k, 16) for k, v in json.load(open(p)).items() if "!" in v}


def write_symbols(path):
    table = symbols()
    table.update(iat_symbols())
    lines = ["/* GENERATED by tools/build_mods.py from data/symbols*.csv - do not edit. Sorted by name (strcmp). */",
             "typedef struct {", "    const char *name;", "    unsigned addr;", "} SdwSymbol;",
             "const SdwSymbol g_sdwSymbols[] = {"]
    for name in sorted(table, key=lambda n: n.encode()):
        lines.append('    {"%s", 0x%x},' % (name, table[name]))
    lines += ["};", "const int g_sdwSymbolCount = %d;" % len(table), ""]
    props = scenaric_props()
    lines += ["typedef struct {", "    unsigned short classId, offset;", "    const char *name;", "} LevelPatchProp;",
              "const LevelPatchProp g_sdwProps[] = {"]
    lines += ['    {%d, 0x%x, "%s"},' % p for p in props] or ['    {0, 0, ""},']
    lines += ["};", "const int g_sdwPropCount = %d;" % len(props), ""]
    sounds = class_sounds() if (ROOT / "src" / "include" / "scenaric_props.h").exists() else []
    lines += ["/* the sounds each object class's code names (class_sounds in tools/build_mods.py) */",
              "const unsigned short g_sdwClassSounds[][2] = {"]
    lines += ["    {%d, %d}," % s for s in sounds] or ["    {0xFFFF, 0},"]
    lines += ["};", "const int g_sdwClassSoundCount = %d;" % len(sounds), ""]
    pictures = class_pictures() if (ROOT / "src" / "include" / "scenaric_props.h").exists() else []
    lines += ["/* the .DAV pictures each object class's code names (class_pictures in tools/build_mods.py) */",
              "const unsigned short g_sdwClassPictures[][2] = {"]
    lines += ["    {%d, %d}," % s for s in pictures] or ["    {0xFFFF, 0},"]
    lines += ["};", "const int g_sdwClassPictureCount = %d;" % len(pictures), ""]
    path.write_text("\n".join(lines))
    return len(table)


def run(cmd):
    r = subprocess.run(cmd, capture_output=True, text=True)
    if r.returncode:
        sys.stdout.write(r.stdout + r.stderr)
        raise SystemExit("failed: " + " ".join(str(c) for c in cmd))


def check_hooks():
    """Every game function's first instructions, decoded by a Python copy of mods/loader/hook.c's decoder, against the
    instruction boundaries in the disassembly."""
    sys.path.insert(0, str(MODS / "loader"))
    from hook_decoder import insn
    asm = ROOT / "work" / "SheepD3D.asm"
    exe = next((ROOT).glob("Sheep*/SheepD3D.exe"))
    data = exe.read_bytes()
    import struct
    pe = struct.unpack_from("<I", data, 0x3C)[0]
    nsec = struct.unpack_from("<H", data, pe + 6)[0]
    opt = struct.unpack_from("<H", data, pe + 20)[0]
    secs = []
    for i in range(nsec):
        vs, va, rs, rp = struct.unpack_from("<IIII", data, pe + 24 + opt + 40 * i + 8)
        secs.append((0x400000 + va, max(vs, rs), rp))
    off = lambda v: next(rp + v - va for va, n, rp in secs if va <= v < va + n)
    starts = set(int(m.group(1), 16) for m in re.finditer(r"^\s+([0-9a-f]+):", asm.read_text(), re.M))
    funcs = {}
    for tier in TIERS:
        for r in csv.DictReader(open(ROOT / tier)):
            try:
                a = int(r["address"], 16)
            except ValueError:
                continue
            if r.get("kind") == "func" and 0x401000 <= a < 0x565850:
                funcs[a] = r["name"]
    ok = unmovable = wrong = unknown = 0
    for a in sorted(funcs):
        pos, fine = 0, True
        while pos < 5:
            n, kind = insn(data, off(a) + pos)
            if kind in ("rel8", "bad"):
                unmovable += 1
                print("not hookable: %s 0x%x (%s at +%d)" % (funcs[a], a, kind, pos))
                fine = False
                break
            nxt = a + pos + n
            if a + pos in starts and nxt not in starts:
                wrong += 1
                print("DECODER DISAGREES: %s 0x%x +%d" % (funcs[a], a, pos))
                fine = False
                break
            if a + pos not in starts:
                unknown += 1                     # objdump's linear sweep lost sync there: no reference to compare
            pos += n
        ok += fine
    print("%d functions: %d hookable, %d not hookable, %d decoder disagreements (%d instructions without a reference)"
          % (len(funcs), ok, unmovable, wrong, unknown))
    return wrong == 0


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--cc", default=os.environ.get("SDW_MINGW", "i686-w64-mingw32-"))
    ap.add_argument("--check-hooks", action="store_true")
    ap.add_argument("--class-sounds", action="store_true", help="list the sounds each object class's code names")
    a = ap.parse_args()
    if a.check_hooks:
        sys.exit(0 if check_hooks() else 1)
    if a.class_sounds:
        names = {int(m.group(2)): m.group(1) for m in re.finditer(
            r'// class "([^"]+)" \(id (\d+)\)', (ROOT / "src" / "include" / "scenaric_props.h").read_text())}
        by = {}
        for c, s in class_sounds():
            by.setdefault(c, []).append(s)
        for c in sorted(by):
            print("%3d %-22s %s" % (c, names.get(c, "?"), " ".join(str(s) for s in by[c])))
        sys.exit(0)
    gcc, gxx = a.cc + "gcc", a.cc + "g++"
    if not shutil.which(gcc):
        raise SystemExit("%s not found: install MinGW-w64 for 32-bit Windows (MODDING.md), or pass --cc" % gcc)
    build = OUT / "build"
    build.mkdir(parents=True, exist_ok=True)
    n = write_symbols(build / "symbols.c")
    loader = MODS / "loader"
    run([gcc, "-O2", "-s", "-shared", "-Wall", "-o", str(OUT / "dinput8.dll"), str(loader / "loader.c"),
         str(loader / "hook.c"), str(loader / "levelpatch.c"), str(build / "symbols.c"), str(loader / "dinput8.def"), "-ladvapi32", "-lgdi32",
         "-static-libgcc", "-Wl,--enable-stdcall-fixup"])
    print("built work/mods/dinput8.dll (the loader, %d symbols)" % n)
    for d in sorted((MODS / "examples").iterdir()):
        if not d.is_dir():
            continue
        dest = OUT / "Mods" / d.name
        if dest.exists():
            shutil.rmtree(dest)
        dest.mkdir(parents=True)
        for f in sorted(d.glob("*")):             # its settings files (settings.txt)
            if f.is_file() and f.suffix.lower() in (".txt", ".ini", ".cfg"):
                shutil.copy(f, dest / f.name)
        for sub in ("levels", "files"):          # a mod's content: level patches and replacement files
            if (d / sub).is_dir():
                shutil.copytree(d / sub, dest / sub)
                print("copied work/mods/Mods/%s/%s" % (d.name, sub))
        for gen in sorted((d / "models").glob("*_png.py")):  # textures made by a script (the repository holds no images)
            subprocess.run([sys.executable, str(gen), str(gen.with_name(gen.stem[:-4] + ".png"))], check=True)
        for obj in sorted((d / "models").glob("*.obj")):  # its own models: the game's files, by tools/make_model.py
            args = obj.with_suffix(".args")
            opts = args.read_text().split() if args.is_file() else []
            make_model.main([str(obj), str(dest / "models" / obj.stem)] + opts)
        srcs = sorted(d.glob("*.cpp")) + sorted(d.glob("*.c"))
        if not srcs:
            continue
        run([gxx, "-O2", "-s", "-shared", "-Wall", "-Wno-unused-function", "-I", str(MODS / "kit"), "-I",
             str(ROOT / "src" / "include"), "-o", str(dest / (d.name + ".dll"))] + [str(s) for s in srcs] +
            ["-static", "-static-libgcc", "-static-libstdc++"])
        print("built work/mods/Mods/%s/%s.dll" % (d.name, d.name))


if __name__ == "__main__":
    main()
