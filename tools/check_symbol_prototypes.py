#!/usr/bin/env python3
"""Cross-check the prototype prose in the symbol tables against the declarations in byte-matched source.

A row in data/symbols.csv - the highest tier, which outranks everything - can contradict source that already matches
the original byte for byte, and when it does the table is wrong and the source is right: for example a row giving
Box_GroundQueryFlatTop 0x515934 three arguments where src/objects/train.cpp declares four.  A matched declaration is the
strongest evidence there is - the function it describes compiles to the original's exact bytes - so a table row that
disagrees with one should never survive unnoticed.

What it compares: every function declaration or definition in src/ whose name is in the tables, against the argument
count of the prototype written in that row's comment.  Comments are prose, so a row without a parseable prototype is
reported as UNCHECKED rather than guessed at.  This is a lint, not a proof: it flags disagreements for a human to settle
by reading the bytes.

Read-only.  Usage:
  python3 tools/check_symbol_prototypes.py            # report disagreements
  python3 tools/check_symbol_prototypes.py --all      # also list what could not be checked
Exit status is 1 if any disagreement is found, so it can gate a commit.
"""
import csv
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
TIERS = ["data/symbols.csv", "data/symbols_modules.csv", "data/symbols_auto.csv"]
SRC = ROOT / "src"
SKIP_DIRS = {"include"}          # generated headers: their declarations come FROM the tables

# A C declaration or definition: <type> Name(args) followed by ; or {   (Class::Name too)
DECL = re.compile(r"(?:^|\n)[ \t]*(?:static\s+|inline\s+|extern\s+|__forceinline\s+|virtual\s+)*"
                  r"[A-Za-z_][\w ]*[\w\*&\s]*?\b(?:(\w+)::)?(\w+)\s*\(([^;{)]*)\)\s*(?:const\s*)?[;{]")
PROTO = re.compile(r"\b(\w+)\s*\(([^)]*)\)")
# `else\n    Foo(&a, b, c);` matches DECL with the keyword as the "return type" and reads the call's arguments as a
# parameter list (it invented a 1-argument Bezier2_EvalWithTangent from holefx.cpp:469). A declaration never starts
# with one of these words; check_decl_drift.py filters the same set.
STMT = {"return", "else", "if", "while", "for", "switch", "do", "case", "goto", "new", "delete", "throw"}


def arg_count(text):
    """Number of arguments in a parameter list, or None when it cannot be told.

    Rows often annotate a parameter inline - `s32 *box32 /* min[3],max[3] */` - and those commas are not separators.
    Stripping the comments first is not cosmetic: counting them turned a correct 4-argument row into a phantom 6.
    """
    text = re.sub(r"/\*.*?\*/", " ", text, flags=re.S).strip()
    if text in ("", "void"):
        return 0
    if "..." in text:
        return None                                   # varargs: the count is not fixed
    depth, n = 0, 1
    for ch in text:
        if ch in "(<[":
            depth += 1
        elif ch in ")>]":
            depth -= 1
        elif ch == "," and depth == 0:
            n += 1
    return n


def table():
    """{name: (address, tier, comment)} with the first (highest) tier winning, as the pipeline resolves it."""
    out = {}
    for tier in TIERS:
        path = ROOT / tier
        if not path.exists():
            continue
        for row in csv.DictReader(path.open()):
            name, kind = (row.get("name") or "").strip(), (row.get("kind") or "").strip()
            if name and kind == "func" and name not in out:
                out[name] = (row.get("address", ""), Path(tier).stem, row.get("comment") or "")
    return out


def proto_args(name, comment):
    """(count, takes_this) from the prototype in a row's comment, or (None, False) if it has none for this name.

    takes_this is set when the row writes the receiver out explicitly - either `__thiscall` or a leading `T *this`.
    A member declaration in the sources leaves the receiver implicit, so such a row legitimately counts one more.
    """
    for m in PROTO.finditer(comment):
        if m.group(1) != name:
            continue
        args = m.group(2)
        first = args.split(",")[0].strip()
        takes_this = "__thiscall" in comment[:m.start()] or bool(re.match(r"[\w ]+\*\s*(this|_?this)\b", first))
        return arg_count(args), takes_this
    # Many rows write the signature WITHOUT repeating the name - `s32 (GroundQuery *q, Box *box, Vec3s *boxPos) -- ...`.
    # Skipping those would miss exactly the rows this check is for.
    # Only the prototype region counts: everything before the ` -- ` that separates the signature from the prose.
    # Only when the text before the '(' is a bare RETURN TYPE - one token plus decorations. Prose is full of
    # parenthesised asides ("(flag 2 = blend when the model is unchanged)"); accepting those invented 130 phantom
    # disagreements, every one of them a row that has no prototype at all.
    head = comment.split(" -- ")[0]
    m = re.search(r"\(([^()]*)\)", head)
    # A parameter list never contains ';' or '='. The CRT rows are spelled `CRT (static LIBCMT, SP5; ...)`, whose head
    # token "CRT" passes the type test but whose parentheses are a citation, not a signature.
    if m and (";" in m.group(1) or "=" in m.group(1)):
        return None, False
    lead = re.match(r"^\s*(?:(?:static|inline|extern|virtual|__cdecl|__thiscall|__stdcall|__fastcall)\s+)*"
                    r"([A-Za-z_]\w*)\s*(\**)\s*$", head[:m.start()]) if m else None
    if lead and (lead.group(2) or lead.group(1) in RETURN_TYPES):
        first = m.group(1).split(",")[0].strip()
        return arg_count(m.group(1)), ("__thiscall" in head or bool(re.match(r"[\w ]+\*\s*(this|_?this)\b", first)))
    return None, False


# A fallback prototype is only believed when the token before '(' is an actual return type. Prose passes a
# "single word" test too: `CRT (static LIBCMT, SP5): strncpy` reads as a 2-argument signature otherwise.
RETURN_TYPES = {"void", "int", "char", "short", "long", "float", "double", "bool", "signed", "unsigned",
                "s8", "s16", "s32", "s64", "u8", "u16", "u32", "u64", "BOOL", "DWORD", "HRESULT", "LRESULT", "size_t"}


MEMBER_BLOCK = re.compile(r"#define\s+SDW_(?:MEMBERS|EXTRA)_\w+|\bclass\s+\w+[^;]*\{|\bstruct\s+\w+\s*:[^;]*\{")


def is_member(text, pos):
    """True when the declaration at pos sits inside a class body or an SDW_MEMBERS_/SDW_EXTRA_ macro block.

    Both leave the receiver implicit, so the table row for the same function legitimately carries one argument more.
    The macro blocks are line-continued, so the block runs until a line that does not end in a backslash.
    """
    starts = [m.end() for m in MEMBER_BLOCK.finditer(text, 0, pos)]
    if not starts:
        return False
    chunk = text[starts[-1]:pos]
    if "\\\n" in text[starts[-1]:starts[-1] + 200] or text[starts[-1]:].lstrip().startswith("\\"):
        return all(line.rstrip().endswith("\\") for line in chunk.splitlines()[:-1] if line.strip())
    return chunk.count("{") >= chunk.count("}")


def sources():
    for path in sorted(SRC.rglob("*")):
        if path.suffix in (".c", ".cpp", ".h") and not any(p in SKIP_DIRS for p in path.relative_to(SRC).parts[:-1]):
            if path.parent.name not in SKIP_DIRS:
                yield path


def main():
    show_all = "--all" in sys.argv
    tbl = table()
    bad, unchecked, checked = [], [], 0
    for path in sources():
        text = path.read_text(encoding="utf-8", errors="replace")
        for m in DECL.finditer(text):
            name = m.group(2)
            first = m.group(0).split(None, 1)
            if first and re.match(r"\w+", first[0]) and re.match(r"\w+", first[0]).group(0) in STMT:
                continue
            if name not in tbl:
                continue
            src_n = arg_count(m.group(3))
            if src_n is None:
                continue
            addr, tier, comment = tbl[name]
            tbl_n, takes_this = proto_args(name, comment)
            line = text[:m.start()].count("\n") + 2
            where = "%s:%d" % (path.relative_to(ROOT), line)
            member = bool(m.group(1)) or is_member(text, m.start())
            allowed = {src_n} | ({src_n + 1} if member or takes_this else set())
            if tbl_n is None:
                unchecked.append((name, addr, tier, where))
            elif tbl_n not in allowed:
                bad.append((name, addr, tier, where, src_n, tbl_n, member))
            else:
                checked += 1

    seen = set()
    for name, addr, tier, where, src_n, tbl_n, member in bad:
        if (name, where) in seen:
            continue
        seen.add((name, where))
        print("DISAGREES  %-40s %s  %s" % (name, addr, tier))
        print("           matched source says %d argument(s)%s: %s"
              % (src_n, " plus an implicit this" if member else "", where))
        print("           table row says %d — read the bytes and settle it" % tbl_n)
    if show_all:
        for name, addr, tier, where in sorted(set(unchecked)):
            print("UNCHECKED  %-40s %s  %s  (row has no prototype)" % (name, addr, tier))
    print("\n%d declaration(s) agreed, %d disagreed, %d had no prototype to check against"
          % (checked, len(seen), len(set(unchecked))))
    return 1 if seen else 0


if __name__ == "__main__":
    sys.exit(main())
