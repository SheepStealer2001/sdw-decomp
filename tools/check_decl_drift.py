#!/usr/bin/env python3
"""Find functions the source declares more than once with DIFFERENT signatures.

Each source file declares the externals it uses, and those declarations can drift apart - for example a query routine
taking `const CollBox *` in one file, `Box *` in another and `const Box *` in a third.  The machine-level pointer is the
same in every case, so every file still byte-matches on its own.  It stops being cosmetic once one canonical DEFINITION
of such a function is written: C++ mangles the parameter types into the symbol, so a single definition cannot satisfy
imports that spell the same parameter three different ways.

A disagreement found here concerns the interfaces between the reconstructed source files, not the original game.

Read-only.  Usage:
  python3 tools/check_decl_drift.py             # functions whose declarations disagree
  python3 tools/check_decl_drift.py --all       # every multiply-declared function, agreeing ones included
Exit status is 1 if any drift is found.
"""
import re
import sys
from collections import defaultdict
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from check_symbol_prototypes import is_member, table   # a declaration inside a class body is a METHOD

ROOT = Path(__file__).resolve().parent.parent
SRC = ROOT / "src"
SKIP_DIRS = {"include"}          # generated headers are produced from the tables, not hand-declared

# The type and the name need NOT be separated by whitespace: `void *Scn_GetPropCamera(...)` puts the space
# BEFORE the star. Requiring \s+ here silently skipped every pointer-returning declaration written that way, which is
# the most common spelling in this tree.
DECL = re.compile(r"(?:^|\n)[ \t]*((?:static\s+|inline\s+|extern\s+|__forceinline\s+|virtual\s+)*"
                  r"[A-Za-z_][\w:<>, ]*?(?:\w\s+|[\*&]\s*))(\w+)\s*\(([^;{)]*)\)\s*(?:const\s*)?[;{]")


# `return Foo(a, b);` and `else Foo(x);` match the declaration shape with the KEYWORD as the return type. Reading
# those as declarations invented whole signatures out of call sites.
STMT = {"return", "else", "if", "while", "for", "switch", "do", "case", "goto", "new", "delete", "throw"}

# u32 IS unsigned int here: sdw_types.h typedefs them, C++ mangles the underlying type, and two files spelling the
# same parameter differently cannot collide at link time. Only a different UNDERLYING type is real drift.
TYPEDEFS = {"s8": "signed char", "u8": "unsigned char", "bool8": "unsigned char", "char": "char",
            "s16": "short", "u16": "unsigned short", "s32": "int", "u32": "unsigned int",
            "s64": "__int64", "u64": "unsigned __int64", "int": "int",
            "float": "float", "double": "double", "void": "void", "fnptr": "void *",
            # the Win32 spellings: LONG and DWORD are typedefs of long / unsigned long and mangle the same
            "LONG": "long", "DWORD": "unsigned long"}


def canon_types(text):
    """Rewrite the project's typedef spellings to the underlying C type, so u32 and unsigned int compare equal.

    `unsigned` must NOT be mapped as a lone word: that would turn `unsigned short` into `unsigned int short` and
    invent a difference in every file that spells the type out. Only a BARE `unsigned` means `unsigned int`.
    """
    # `::GroundQuery` names the same type as `GroundQuery`; a file writes the leading :: where a method of the same
    # name would otherwise hide the struct (rcarpetmobile.cpp), which is scoping, not a different type.
    text = re.sub(r"(?<![\w:])::(?=\w)", "", text)
    text = re.sub(r"\bunsigned\b(?!\s+(?:int|short|char|long|__int64))", "unsigned int", text)
    return re.sub(r"\b(\w+)\b", lambda m: TYPEDEFS.get(m.group(1), m.group(1)), text)


def norm(text):
    """Collapse whitespace and drop parameter NAMES, so only the types are compared."""
    text = re.sub(r"/\*.*?\*/", " ", text, flags=re.S)
    parts = []
    for arg in re.split(r",(?![^<(]*[>)])", text):
        arg = " ".join(arg.split())
        # `const CollBox *boxes` -> `const CollBox *`. A bare type must survive: stripping the last word from
        # `double` leaves nothing and turned sqrt(double) into sqrt(), inventing drift that does not exist.
        stripped = re.sub(r"\b\w+\s*(\[\s*\d*\s*\])?$", r"\1", arg).strip()
        if stripped and not re.fullmatch(r"(const\s+)?(unsigned\s+|signed\s+)?[\w:]+", arg.strip()):
            arg = stripped
        parts.append(" ".join(arg.split()))
    # `f()` and `f(void)` are the same declaration in C++ and mangle identically; only C distinguishes them.
    out = canon_types(", ".join(p for p in parts if p not in ("",)))
    # `Vec3s*` and `Vec3s *` are the same type; the tree mixes both spellings.
    out = re.sub(r"\s*([*&])\s*", r" \1", out).replace(" ,", ",").strip()
    return "" if out.strip() == "void" else out


def main():
    show_all = "--all" in sys.argv
    # Only names the tables resolve to ONE original function can collide: SetState, QueryInterface, GetCaps and the
    # rest of the COM/interface method names are genuinely different functions that happen to share a spelling, and
    # reporting them would bury the real cases in noise.
    known = set(table())
    decls = defaultdict(list)
    for path in sorted(SRC.rglob("*")):
        if path.suffix not in (".c", ".cpp", ".h") or path.parent.name in SKIP_DIRS:
            continue
        text = path.read_text(encoding="utf-8", errors="replace")
        for m in DECL.finditer(text):
            # `extern`/`static`/`inline` and an explicit `__cdecl` (the /Gd default everywhere here) are not part of
            # the type: leaving them in reported `double __cdecl sqrt(double)` as drifting from `double sqrt(double)`.
            ret = " ".join(re.sub(r"\b(extern|static|inline|__forceinline|__cdecl)\b", " ", m.group(1)).split())
            if ret.split()[0] in STMT if ret.split() else True:
                continue
            ret = re.sub(r"\s*([*&])\s*", r" \1", canon_types(ret)).strip()
            name, args = m.group(2), norm(m.group(3))
            if name in ("if", "for", "while", "switch", "return", "sizeof", "do"):
                continue
            # Methods of different classes legitimately share a name - AnimFlags is declared in 20 class bodies and
            # is not drift. Only FILE-SCOPE declarations of the same original function can collide at link time,
            # which is the case this checks for.
            if is_member(text, m.start()) or name not in known:
                continue
            line = text[:m.start()].count("\n") + 2
            decls[name].append((ret, args, "%s:%d" % (path.relative_to(ROOT), line)))

    drift, agree = [], 0
    for name, rows in sorted(decls.items()):
        shapes = {(r, a) for r, a, _w in rows}
        if len(rows) < 2:
            continue
        if len(shapes) == 1:
            agree += 1
        else:
            drift.append((name, rows))

    for name, rows in drift:
        print("DRIFT  %s  (%d declarations, %d distinct)"
              % (name, len(rows), len({(r, a) for r, a, _ in rows})))
        by_shape = defaultdict(list)
        for ret, args, where in rows:
            by_shape[(ret, args)].append(where)
        for (ret, args), wheres in sorted(by_shape.items()):
            print("       %s (%s)" % (ret, args))
            for w in wheres:
                print("           %s" % w)
    if show_all:
        print("\n%d multiply-declared function(s) agree everywhere" % agree)
    print("\n%d function(s) drift, %d declared consistently in 2+ files" % (len(drift), agree))
    if not drift and not agree:
        print("WARNING: nothing was compared at all - treat this as a FAILED run, not a pass")
        return 1
    return 1 if drift else 0


if __name__ == "__main__":
    sys.exit(main())
