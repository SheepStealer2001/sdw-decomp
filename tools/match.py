#!/usr/bin/env python3
"""Byte-match compiled functions against SheepD3D.exe.

Takes a COFF object produced by the original toolchain (VC6 SP5 + Processor Pack; see tools/vc6.py)
and, for every function in it, compares the compiled bytes with the original ones at that function's address.

  python3 tools/match.py work/match/time_rand_math.obj            one line per function, then a summary
  python3 tools/match.py OBJ --diff Math_ApproachLinear           instruction-level diff for one function
  python3 tools/match.py OBJ --addr RoundPower2=0x54da7d          address for a name the symbol tables lack
  python3 tools/match.py OBJ --json out.json                      machine-readable results

How a function is placed: its COFF name is undecorated (`_Foo`, `_Foo@8`, `?Foo@Class@@...` -> `Class_Foo`, then `Foo`)
and looked up in data/symbols.csv, symbols_modules.csv, symbols_auto.csv (first wins, the project's tier order). A virtual
method, deleting destructor or vtable the tables do not name is found through its class's vtable (work/class_vtables.json,
from tools/cpp_classes.py). The
compiled span runs from the symbol to the next function symbol in its section (or the section end); VC6 /Od emits
functions back to back with no padding.

How relocations are checked (i386 COFF: DIR32 = S+A, DIR32NB = S+A-base, REL32 = S+A-(P+4)); no byte is masked:
  ok        the target symbol has a known address (symbol tables, IAT, or another function in this object) and the
            linked value equals the original's.
  content   the target is data defined in this object (a float constant, a string, a static) with no known address; its
            address is read from the original and the object's bytes for it are found there.
  inferred  the target is an external with no known address; the original's value is accepted, and reported, so it can be
            checked or named. Typical for the CRT helpers the compiler calls on its own (__ftol, __chkesp).
  CONFLICT  a known address disagrees, the same symbol is read at two different addresses, the object's data is not
            what the original holds there, or an inferred call lands on a function the tables name differently.

  renamed   like inferred, but the tables name that address differently: the bytes match, the source should use the
            table's name (or the table is wrong).
  local     a call to another function of this object that the tables do not name; its address is taken from the
            original and that function is then checked in its own right (so is one found through a vtable slot).

A static initialiser (VC6's $E functions) is placed from its file's `match-init:` list (see tools/vc6.py), and the thunks it
calls by reference; a function placed by a reference at an address the tables name counts as placed by name.

Verdicts: MATCH (every byte equal, every relocation ok/content/local, span >= Ghidra's size for the function; longer is
allowed because VC6 puts a switch's jump table after the code, and those bytes are compared too), MATCH~ (the same, but
some target address was taken from the original: inferred, renamed, the function itself placed by a reference, or the
original's size unknown), DIFF
(anything else; first differing offset shown), NOADDR (not in the tables and nothing in the object points at it).
Exit status 1 if any function is DIFF.
A MATCH is whole-function machine-code equality under the recorded flags - not a linked image, and not proof that
the source spelling is the original's.
"""
import textio  # noqa: F401  (first: UTF-8, LF text files on Windows)
import argparse, csv, difflib, json, re, struct, subprocess, sys, tempfile
from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[1]
EXE = ROOT / "Sheep, Dog 'n' Wolf (PAL Version)" / "SheepD3D.exe"
SYMBOL_TIERS = [ROOT / "data" / n for n in ("symbols.csv", "symbols_modules.csv", "symbols_auto.csv")]
FUNCS_JSON = ROOT / "work" / "ghidra_functions.json"
SIZES_JSON = ROOT / "data" / "function_sizes.json"            # committed fallback: {va: {size, game}} from the same export
IAT_JSON = ROOT / "work" / "iat.json"
VTABLES_JSON = ROOT / "work" / "class_vtables.json"          # written by tools/cpp_classes.py
OBJDUMP = "i686-w64-mingw32-objdump"

DIR32, DIR32NB, REL32 = 0x06, 0x07, 0x14
DEBUG_RELOCS = {0x00, 0x0A, 0x0B, 0x0D}           # ABSOLUTE, SECTION, SECREL, SECREL7: debug sections only
SCN_CODE, SCN_UNINIT, SCN_NRELOC_OVFL = 0x20, 0x80, 0x01000000


# ---------------------------------------------------------------- the original executable
class Exe:
    def __init__(self, path):
        self.d = d = path.read_bytes()
        pe = struct.unpack_from("<I", d, 0x3C)[0]
        nsec, = struct.unpack_from("<H", d, pe + 6)
        optsz, = struct.unpack_from("<H", d, pe + 20)
        opt = pe + 24
        self.base, = struct.unpack_from("<I", d, opt + 28)
        self.secs = []
        for i in range(nsec):
            name, vs, va, rs, rp = struct.unpack_from("<8sIIII", d, opt + optsz + i * 40)
            self.secs.append((va, vs, rp, rs))

    def read(self, va, n):
        rva = va - self.base
        for sva, vs, rp, rs in self.secs:
            if sva <= rva < sva + max(vs, rs):
                o = rva - sva
                if o + n > rs:                       # past the raw data (bss-like tail): no file bytes to compare
                    return None
                return self.d[rp + o:rp + o + n]
        return None

    def u32(self, va):
        b = self.read(va, 4)
        return None if b is None else struct.unpack("<I", b)[0]


# ---------------------------------------------------------------- COFF object
class Obj:
    def __init__(self, path):
        d = path.read_bytes()
        machine, nsec, _, symptr, nsym, optsz, _ = struct.unpack_from("<HHIIIHH", d, 0)
        if machine != 0x14C:
            raise SystemExit("%s: not an i386 COFF object (machine 0x%x)" % (path, machine))
        strtab = symptr + nsym * 18
        def name_at(raw):
            if raw[:4] == b"\0\0\0\0":
                o = strtab + struct.unpack_from("<I", raw, 4)[0]
                return d[o:d.index(b"\0", o)].decode("latin-1")
            return raw.rstrip(b"\0").decode("latin-1")
        self.sections = [None]                        # 1-based, like the symbol table's section numbers
        for i in range(nsec):
            h = 20 + optsz + i * 40
            raw = d[h:h + 8]
            _, _, size, ptr, rptr, _, nrel, _, flags = struct.unpack_from("<IIIIIIHHI", d, h + 8)
            nm = raw.rstrip(b"\0").decode("latin-1")
            if nm.startswith("/"):
                nm = name_at(b"\0\0\0\0" + struct.pack("<I", int(nm[1:])))
            data = b"" if flags & SCN_UNINIT else d[ptr:ptr + size]
            relocs = []
            first = 0
            if flags & SCN_NRELOC_OVFL:
                nrel = struct.unpack_from("<I", d, rptr)[0]
                first = 1
            for k in range(first, nrel):
                va, si, ty = struct.unpack_from("<IIH", d, rptr + k * 10)
                relocs.append((va, si, ty))
            self.sections.append({"name": nm, "size": size, "flags": flags, "data": data, "relocs": relocs})
        self.symbols = {}
        i = 0
        while i < nsym:
            o = symptr + i * 18
            raw = d[o:o + 8]
            value, secnum, ty, cls, naux = struct.unpack_from("<IhHBB", d, o + 8)
            self.symbols[i] = {"name": name_at(raw), "value": value, "sec": secnum, "type": ty, "cls": cls,
                               "aux": d[o + 18:o + 18 + 18 * naux]}
            i += 1 + naux

    def resolve(self, s):
        """What the linker would bind a symbol to inside this object: an undefined name that this object also defines
        (VC6 emits both, e.g. for a COMDAT scalar deleting destructor), or a weak external's alternate (a vtable's
        vector-deleting-destructor slot ??_E falls back to ??_G this way)."""
        if not hasattr(self, "_defined"):
            self._defined = {}
            for x in self.symbols.values():
                if x["sec"] > 0 and x["cls"] == 2:
                    self._defined.setdefault(x["name"], x)
        for _ in range(4):
            if s["sec"] > 0:
                return s
            d = self._defined.get(s["name"])
            if d:
                return d
            if s["cls"] == 105 and len(s["aux"]) >= 4:          # IMAGE_SYM_CLASS_WEAK_EXTERNAL: aux TagIndex
                s = self.symbols[struct.unpack_from("<I", s["aux"], 0)[0]]
                continue
            return s
        return s

    def is_section_symbol(self, s):
        return s["cls"] == 3 and s["value"] == 0 and s["sec"] > 0 and s["name"] == self.sections[s["sec"]]["name"]

    def functions(self):
        """(symbol, section, start, span) for every function symbol in a code section."""
        by_sec = {}
        for s in self.symbols.values():
            if s["sec"] > 0 and (s["type"] >> 4) == 2 and s["cls"] in (2, 3):   # DTYPE_FUNCTION, EXTERNAL/STATIC
                sec = self.sections[s["sec"]]
                if sec["flags"] & SCN_CODE:
                    by_sec.setdefault(s["sec"], []).append(s)
        out = []
        for secnum, syms in by_sec.items():
            syms.sort(key=lambda s: s["value"])
            for k, s in enumerate(syms):
                end = syms[k + 1]["value"] if k + 1 < len(syms) else self.sections[secnum]["size"]
                out.append((s, secnum, s["value"], end - s["value"]))
        out.sort(key=lambda f: (f[1], f[2]))
        return out


# ---------------------------------------------------------------- names
def load_symbols():
    by_name, by_addr = {}, {}
    for path in SYMBOL_TIERS:
        if not path.exists():
            continue
        with open(path, newline="") as f:
            for row in csv.reader(f):
                if len(row) < 2 or not row[0].startswith("0x"):
                    continue
                a, n = int(row[0], 16), row[1].strip()
                by_name.setdefault(n, a)
                by_addr.setdefault(a, n)
    return by_name, by_addr


def load_iat():
    """'__imp__Name@N' / '__imp__Name' -> IAT slot VA."""
    out = {}
    if IAT_JSON.exists():
        for va, full in json.load(open(IAT_JSON)).items():
            fn = full.split("!", 1)[-1]
            out[fn] = int(va, 16)
    return out


def candidates(coff_name):
    """Names to try in the symbol tables for a decorated COFF name, best first."""
    n = coff_name
    if n.startswith("?"):
        if n in ("??2@YAPAXI@Z", "??3@YAXPAX@Z"):              # the global operator new / delete
            return ["operator_new" if n[2] == "2" else "operator_delete"]
        m = re.match(r"\?\?([0-9])([A-Za-z_]\w*)@", n)          # ??0Class@@ ctor, ??1Class@@ dtor
        if m:
            kinds = {"0": ("Ctor", "Construct"), "1": ("Destruct", "Dtor")}.get(m.group(1), ())
            return [m.group(2) + "_" + k for k in kinds]
        m = re.match(r"\?\?_([GE])([A-Za-z_]\w*)@@", n)          # compiler-generated deleting destructors
        if m:
            return [m.group(2) + ("_ScalarDeletingDtor" if m.group(1) == "G" else "_VectorDeletingDtor")]
        m = re.match(r"\?\?_7([A-Za-z_]\w*)@@", n)               # vftable
        if m:
            return [m.group(1) + "_vtbl"]
        m = re.match(r"\?([A-Za-z_]\w*)@([A-Za-z_]\w*)@@", n)    # ?Method@Class@@...
        if m:
            meth, cls = m.group(1), m.group(2)
            return [cls + "_" + meth, meth] if not meth.startswith(cls + "_") else [meth]
        m = re.match(r"\?([A-Za-z_]\w*)@@", n)                   # ?Free@@... (C++ free function or global)
        return [m.group(1)] if m else []
    if n.startswith("@"):                                        # fastcall @Name@N
        return [n[1:].split("@")[0]]
    if n.startswith("_"):                                        # cdecl _Name, stdcall _Name@N
        return [n[1:].split("@")[0]]
    return [n]


# ---------------------------------------------------------------- the comparison
class Matcher:
    def __init__(self, obj, exe, overrides, init_roots=()):
        self.obj, self.exe = obj, exe
        self.by_name, self.by_addr = load_symbols()
        self.by_name.update(overrides)
        for n, a in overrides.items():
            self.by_addr.setdefault(a, n)
        self.iat = load_iat()
        self.vtables = json.load(open(VTABLES_JSON)) if VTABLES_JSON.exists() else {}
        self.via_vtable = set()                                  # names placed through a vtable, for the report
        self.orig_size = {}
        # Original function sizes: Ghidra's export when present, else the committed data/function_sizes.json (the
        # same boundaries, extracted from it, so a checkout without Ghidra still gets strict verdicts).
        # Ghidra's export wins where both know a function; the committed file also covers functions Ghidra never
        # defined (libjpeg's folded one- and three-byte bodies, added by hand with their evidence).
        for sizes in (SIZES_JSON, FUNCS_JSON):
            if sizes.exists():
                for k, v in json.load(open(sizes)).items():
                    if v.get("size") is not None:
                        self.orig_size[int(k, 16)] = v.get("size")
        self.func_at = {}                                        # (sec, value) -> VA, for calls between this object's functions
        self.placed = []
        for s, sec, start, span in obj.functions():
            va, nm = self.lookup(s["name"])
            self.placed.append((s, sec, start, span, va, nm))
            if va is not None:
                self.func_at[(sec, start)] = va
        # Static initialisers: the object's .CRT$XCU entries point at its root initialisers ($E symbols), in the order
        # the globals are defined; nothing else refers to them. init_roots gives their addresses in that order (vc6.py
        # reads them from a `match-init:` line); the ctor / atexit / dtor thunks they call are then placed by reference.
        xcu = [sec for sec in obj.sections[1:] if sec["name"].startswith(".CRT$XC")]
        roots = sorted((off, si) for sec in xcu for off, si, ty in sec["relocs"] if ty == DIR32)
        for (off, si), va in zip(roots, init_roots):
            t = obj.resolve(obj.symbols[si])
            self.func_at[(t["sec"], t["value"])] = va
            self.placed = [(s, sec, start, span, va if (sec, start) == (t["sec"], t["value"]) else pva,
                            self.by_addr.get(va, nm) if (sec, start) == (t["sec"], t["value"]) else nm)
                           for s, sec, start, span, pva, nm in self.placed]
        self.local_funcs = {(sec, start) for _, sec, start, _, _, _ in self.placed}
        self.derived = {}                                        # (sec, value) -> VA read from a call or a vtable
        self.seen = {}                                           # symbol key -> address read from the original

    def lookup(self, coff_name):
        if coff_name in self.by_name:        # a decorated name pinned by --addr / match-addr: picks one overload of
            return self.by_name[coff_name], coff_name   # a set candidates() cannot tell apart (two ??0D3DApp ctors)
        for c in candidates(coff_name):
            if c in self.by_name:
                return self.by_name[c], c
        c = candidates(coff_name)
        va = self.vtable_lookup(coff_name)
        if va is not None:
            self.via_vtable.add(coff_name)
            return va, self.by_addr.get(va, c[0] if c else coff_name)
        return None, (c[0] if c else coff_name)

    def vtable_lookup(self, n):
        """A virtual method, deleting destructor or vtable of a class in work/class_vtables.json, found through the class's vtable
        in the exe when the tables do not know the name (a slot named PostLoadInit whose Wolf implementation is called Wolf_Init)."""
        m = re.match(r"\?\?_7([A-Za-z_]\w*)@@6B@$", n)
        if m and m.group(1) in self.vtables:
            return int(self.vtables[m.group(1)]["vtable"], 16)
        m = re.match(r"\?\?_[GE]([A-Za-z_]\w*)@@", n)
        cls, meth = (m.group(1), "~" + m.group(1)) if m else (None, None)
        if not m:
            m = re.match(r"\?([A-Za-z_]\w*)@([A-Za-z_]\w*)@@", n)
            if not m:
                return None
            meth, cls = m.group(1), m.group(2)
        v = self.vtables.get(cls)
        if not v or meth not in v["slots"]:
            return None
        va = int(v["targets"][v["slots"].index(meth)], 16)
        if n.startswith("??_G"):
            # The slot holds the VECTOR deleting destructor (??_E) when the original linker kept that one: its body tests
            # flags & 2 first (mov eax,[ebp+8]; and eax,2). A scalar ??_G must not be placed there.
            body = self.exe.read(va, 0x40) or b""
            if b"\x8b\x45\x08\x83\xe0\x02" in body[:0x10] or re.search(r"Vec(tor)?DeletingDtor", self.by_addr.get(va, "")):
                return None
        return va

    def known_address(self, sym):
        """Address of a relocation target if it is known independently of the original's bytes, else None."""
        if sym["sec"] > 0:
            if (sym["sec"], sym["value"]) in self.func_at:
                return self.func_at[(sym["sec"], sym["value"])]
            if not (self.obj.sections[sym["sec"]]["flags"] & SCN_CODE) and not self.obj.is_section_symbol(sym):
                va, _ = self.lookup(sym["name"])            # a global this object defines under a table name
                if va is not None:
                    return va
            if self.obj.sections[sym["sec"]]["flags"] & SCN_CODE and (sym["type"] >> 4) != 2:
                # a label inside one of this object's placed functions: a switch case or the jump table itself
                for _, sec, start, span, va, _ in self.placed:
                    if sec == sym["sec"] and start <= sym["value"] < start + span:
                        va = va if va is not None else self.derived.get((sec, start))
                        return None if va is None else va + sym["value"] - start
            return None                                          # else: defined here, read it and check it
        n = sym["name"]
        if n.startswith("__imp_"):
            base = n[6:]
            for k in (base.lstrip("_").split("@")[0], base):
                if k in self.iat:
                    return self.iat[k]
            return None
        va, _ = self.lookup(n)
        return va

    def derive(self, sym, va):
        """A function of this object with no table address, found at va through a call or a data pointer. It is then
        checked as a function in its own right; a second, different address is a conflict."""
        key = (sym["sec"], sym["value"])
        prev = self.derived.setdefault(key, va)
        return prev == va

    def content_check(self, sym, addend, S):
        """For data defined in this object (S read from the original): are the object's bytes for it there?
        A section symbol stands for the section base, so the object referenced is at S + addend, section offset addend;
        a named symbol is at S, section offset sym.value. Pointer fields inside the data (relocations: vtable slots,
        string tables) are resolved like code relocations: known targets must agree, this object's functions get their
        address from them, unknown externals are skipped."""
        sec = self.obj.sections[sym["sec"]]
        if sec["flags"] & SCN_UNINIT or not sec["data"]:
            return True, "uninitialised, not compared"
        if self.obj.is_section_symbol(sym):
            start, target = addend, S + addend
        else:
            start, target = sym["value"], S
        marks = sorted({s["value"] for s in self.obj.symbols.values() if s["sec"] == sym["sec"]} |
                       self.section_refs(sym["sec"]) | {len(sec["data"])})
        end = min(next((m for m in marks if m > start), len(sec["data"])), start + 256)
        want = bytearray(sec["data"][start:end])
        have = self.exe.read(target, len(want))
        if have is None:
            return False, "no file bytes at 0x%x" % target
        skipped = 0
        for off, si, ty in sec["relocs"]:
            if not (start <= off < end) or ty != DIR32:
                continue
            fo = off - start
            raw = self.obj.symbols[si]
            tsym = self.obj.resolve(raw)
            # A slot that is a WEAK EXTERNAL (class 105, the ??_E) falling back only to this object's local ??_G: the exe's
            # slot address is the ??_E's, so it says nothing about the ??_G. Leave it unresolved.
            if raw["cls"] == 105 and tsym is not raw and tsym["name"].startswith("??_G"):
                tsym = raw
            a = struct.unpack_from("<I", want, fo)[0]
            got = (struct.unpack_from("<I", have, fo)[0] - a) & 0xFFFFFFFF
            k = self.known_address(tsym)
            if k is not None:
                if k != got:
                    return False, "pointer at +0x%x is 0x%x, tables say %s = 0x%x" % (fo, got, tsym["name"], k)
            elif tsym["sec"] > 0 and (tsym["sec"], tsym["value"]) in self.local_funcs:
                if not self.derive(tsym, got):
                    return False, "pointer at +0x%x to %s disagrees with an earlier reference" % (fo, tsym["name"])
            else:
                skipped += 1
            struct.pack_into("<I", want, fo, (got + a) & 0xFFFFFFFF)
        note = "%d bytes at 0x%x" % (len(want), target) + (", %d unresolved pointer(s) not compared" % skipped if skipped else "")
        return bytes(want) == have, note

    def section_refs(self, secnum):
        out = set()
        for sec in self.obj.sections[1:]:
            for va, si, ty in sec["relocs"]:
                s = self.obj.symbols.get(si)
                if s and s["sec"] == secnum and self.obj.is_section_symbol(s) and va + 4 <= len(sec["data"]):
                    out.add(struct.unpack_from("<I", sec["data"], va)[0])
        return out

    def check(self, s, sec, start, span, va):
        code = self.obj.sections[sec]
        compiled = bytearray(code["data"][start:start + span])
        original = self.exe.read(va, span)
        relocs, linked = [], bytearray(compiled)
        for off, si, ty in code["relocs"]:
            if not (start <= off < start + span) or ty in DEBUG_RELOCS:
                continue
            fo = off - start
            sym = self.obj.resolve(self.obj.symbols[si])
            addend = struct.unpack_from("<I", compiled, fo)[0]
            p = va + fo
            orig = struct.unpack_from("<I", original, fo)[0] if original and fo + 4 <= len(original) else None
            if ty == DIR32:
                read = None if orig is None else (orig - addend) & 0xFFFFFFFF
                value = lambda S: (S + addend) & 0xFFFFFFFF
            elif ty == DIR32NB:
                read = None if orig is None else (orig + self.exe.base - addend) & 0xFFFFFFFF
                value = lambda S: (S + addend - self.exe.base) & 0xFFFFFFFF
            elif ty == REL32:
                read = None if orig is None else (orig + p + 4 - addend) & 0xFFFFFFFF
                value = lambda S: (S + addend - p - 4) & 0xFFFFFFFF
            else:
                relocs.append({"offset": fo, "symbol": sym["name"], "type": ty, "status": "CONFLICT",
                               "note": "unsupported relocation type 0x%x" % ty})
                continue
            known = self.known_address(sym)
            r = {"offset": fo, "symbol": sym["name"], "type": ty, "target": None if read is None else "0x%x" % read}
            S = known if known is not None else read
            if read is None:
                r["status"], r["note"] = "CONFLICT", "no original bytes"
            elif known is not None:
                r["status"] = "ok" if read == known else "CONFLICT"
                if read != known:
                    r["note"] = "tables say 0x%x, original has 0x%x" % (known, read)
                elif sym["sec"] > 0 and not (self.obj.sections[sym["sec"]]["flags"] & SCN_CODE):
                    ok, what = self.content_check(sym, addend, read)    # defined data: its initial bytes must be there too
                    if not ok:
                        r["status"], r["note"] = "CONFLICT", "object data differs from the original's (%s)" % what
            elif sym["sec"] > 0 and (sym["sec"], sym["value"]) in self.local_funcs:
                ok = self.derive(sym, read)
                r["status"] = "local" if ok else "CONFLICT"
                r["note"] = ("0x%x, checked as its own function" % read) if ok else "a second address for this function"
            else:
                key = (sym["sec"], sym["value"], addend) if self.obj.is_section_symbol(sym) else sym["name"]
                prev = self.seen.setdefault(key, read)
                named = self.by_addr.get(read)
                if prev != read:
                    r["status"], r["note"] = "CONFLICT", "read at 0x%x here but 0x%x elsewhere" % (read, prev)
                elif sym["sec"] > 0:
                    ok, what = self.content_check(sym, addend, read)
                    r["status"] = "content" if ok else "CONFLICT"
                    r["note"] = ("object data found there (%s)" % what) if ok else "object data differs from the original's (%s)" % what
                elif named and named not in candidates(sym["name"]):
                    r["status"], r["note"] = "renamed", "0x%x is %s in the tables" % (read, named)
                else:
                    r["status"] = "inferred"
                    r["note"] = "0x%x" % read + (" = %s" % named if named else "")
            if S is not None:
                struct.pack_into("<I", linked, fo, value(S))
            relocs.append(r)
        return compiled, bytes(linked), original, relocs

    def judge(self, s, sec, start, span, va, nm, how=None):
        res = {"name": nm, "symbol": s["name"], "span": span, "va": "0x%x" % va}
        if how:
            res["address_from"] = how
        osz = self.orig_size.get(va)
        res["original_size"] = osz
        compiled, linked, original, relocs = self.check(s, sec, start, span, va)
        res["relocations"] = relocs
        diffs = [i for i in range(span) if original is None or i >= len(original) or linked[i] != original[i]]
        bad = [r for r in relocs if r["status"] == "CONFLICT"]
        soft = [r for r in relocs if r["status"] in ("inferred", "renamed")]
        res["bytes_differing"] = len(diffs)
        if diffs:
            res["first_diff"] = diffs[0]
        named_there = how and va in self.by_addr         # placed by a reference, at an address the tables name
        if named_there:
            res["name"] = self.by_addr[va]
        if not diffs and not bad and (osz is None or span >= osz):
            res["verdict"] = "MATCH~" if soft or (how and not named_there) or osz is None else "MATCH"
            if osz is not None and span > osz:
                res["note"] = "%d bytes past Ghidra's function end also match (a jump table?)" % (span - osz)
            elif osz is None:
                res["note"] = "original size unknown: the compiled bytes could be a prefix of a longer function"
        else:
            res["verdict"] = "DIFF"
        if how and va in self.by_addr and self.by_addr[va] not in candidates(s["name"]):
            res["table_name"] = self.by_addr[va]
        if s["name"] in self.via_vtable:
            res["note"] = ((res.get("note") + "; ") if res.get("note") else "") + "placed by its vtable slot"
        res["_linked"], res["_original"] = linked, original
        return res

    def run(self, only=None):
        results, pending = [], []
        for item in self.placed:
            s, sec, start, span, va, nm = item
            if va is None:
                pending.append(item)
            else:
                results.append(self.judge(*item))
        # second pass: this object's functions the tables do not name, placed by a call or a vtable slot seen above
        for s, sec, start, span, va, nm in pending:
            d = self.derived.get((sec, start))
            if d is None:
                results.append({"name": nm, "symbol": s["name"], "span": span, "verdict": "NOADDR"})
            else:
                results.append(self.judge(s, sec, start, span, d, nm, how="a reference from this object"))
        if only:
            results = [r for r in results if only in (r["name"], r["symbol"])]
        order = {(s["name"]): i for i, (s, *_rest) in enumerate(self.placed)}
        results.sort(key=lambda r: order.get(r["symbol"], 0))
        return results


# ---------------------------------------------------------------- reporting
def disasm(blob, va):
    """[(address, instruction text)]; branch targets inside the blob become <+off> so an inserted instruction does not
    make every later line differ."""
    with tempfile.NamedTemporaryFile(suffix=".bin") as f:
        f.write(blob)
        f.flush()
        out = subprocess.run([OBJDUMP, "-D", "-b", "binary", "-m", "i386", "-M", "intel", "--no-show-raw-insn",
                              "--adjust-vma=0x%x" % va, f.name], capture_output=True, text=True).stdout
    def rel(m):
        t = int(m.group(0), 16)
        return "<+0x%x>" % (t - va) if va <= t < va + len(blob) else m.group(0)
    lines = []
    for line in out.splitlines():
        m = re.match(r"\s*([0-9a-f]+):\s+(.*)", line)
        if m:
            text = re.sub(r"\s+", " ", m.group(2)).strip()
            if re.match(r"(j\w+|call|loop\w*) 0x[0-9a-f]+$", text):
                text = re.sub(r"0x[0-9a-f]+", rel, text)
            lines.append((int(m.group(1), 16) - va, text))
    return lines


def show_diff(res, exe):
    """Original (its whole function, by the Ghidra size) against the compiled bytes linked at the same address.
    Lines are compared by instruction text; the offsets are each side's own."""
    va = int(res["va"], 16)
    osz = res.get("original_size") or res["span"]
    a = disasm(exe.read(va, osz) or b"", va)
    b = disasm(res["_linked"], va)
    print("--- original %s  %s (%d bytes)" % (res["va"], res["name"], osz))
    print("+++ compiled, linked at the same address (%d bytes)" % res["span"])
    sm = difflib.SequenceMatcher(None, [t for _, t in a], [t for _, t in b], autojunk=False)
    for tag, i1, i2, j1, j2 in sm.get_opcodes():
        if tag == "equal":
            span = list(range(i1, i2))
            keep = span if len(span) <= 6 else span[:3] + [None] + span[-3:]
            for i in keep:
                print("       ..." if i is None else "  +%04x  %s" % (a[i][0], a[i][1]))
            continue
        for i in range(i1, i2):
            print("- +%04x  %s" % (a[i][0], a[i][1]))
        for j in range(j1, j2):
            print("+ +%04x  %s" % (b[j][0], b[j][1]))


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("obj", type=Path)
    ap.add_argument("--diff", metavar="NAME", help="instruction diff for one function")
    ap.add_argument("--addr", action="append", default=[], metavar="NAME=0xVA", help="address for a name the tables lack")
    ap.add_argument("--json", type=Path, help="write the full results here")
    ap.add_argument("-v", "--verbose", action="store_true", help="list every relocation")
    a = ap.parse_args()
    overrides = {}
    for kv in a.addr:
        n, v = kv.split("=", 1)
        overrides[n] = int(v, 16)
    exe = Exe(EXE)
    results = Matcher(Obj(a.obj), exe, overrides).run(a.diff)
    if a.diff and not results:
        raise SystemExit("no function %r in %s" % (a.diff, a.obj))
    report(results, a.verbose)
    if a.diff and results[0].get("va"):
        show_diff(results[0], exe)
    if a.json:
        a.json.write_text(json.dumps(clean(results), indent=1) + "\n")
    sys.exit(1 if any(r["verdict"] == "DIFF" for r in results) else 0)


def clean(results):
    return [{k: v for k, v in r.items() if not k.startswith("_")} for r in results]


def report(results, verbose=False, indent=""):
    counts = {}
    for r in results:
        counts[r["verdict"]] = counts.get(r["verdict"], 0) + 1
        size = "%d/%s" % (r["span"], r.get("original_size") if r.get("original_size") is not None else "?")
        extra = ""
        if r["verdict"] == "DIFF":
            bits = []
            if r.get("bytes_differing"):
                bits.append("first diff at +0x%x, %d bytes differ" % (r["first_diff"], r["bytes_differing"]))
            if r.get("original_size") is not None and r["span"] < r["original_size"]:
                bits.append("shorter than the original")
            # once the code diverges the later relocation fields no longer line up, so only report those before it
            conf = [x for x in r["relocations"] if x["status"] == "CONFLICT" and x["offset"] <= r.get("first_diff", 1 << 30)]
            bits += ["%s @+0x%x: %s" % (x["symbol"], x["offset"], x.get("note", "")) for x in conf[:3]]
            extra = "  " + "; ".join(bits)
        elif r["verdict"] in ("MATCH~", "MATCH"):
            bits = ["%s %s -> %s" % (x["status"], x["symbol"], x["note"]) for x in r["relocations"] if x["status"] in ("inferred", "renamed")]
            if r.get("address_from"):
                bits.insert(0, "address from " + r["address_from"] + (" (tables: %s)" % r["table_name"] if r.get("table_name") else ""))
            if r.get("note"):
                bits.append(r["note"])
            extra = ("  " + "; ".join(bits)) if bits else ""
        elif r["verdict"] == "NOADDR":
            extra = "  (%s: not in the symbol tables and nothing in this object refers to it; --addr to place it)" % r["symbol"]
        print("%s%-7s %-10s %9s  %s%s" % (indent, r["verdict"], r.get("va", "-"), size, r["name"], extra))
        if verbose:
            for x in r.get("relocations", []):
                print("%s          +0x%-4x %-8s %-40s %s %s" % (indent, x["offset"], x["status"], x["symbol"], x.get("target") or "", x.get("note", "")))
    print("%s%d functions: %s" % (indent, len(results), ", ".join("%d %s" % (v, k) for k, v in sorted(counts.items()))))
    return counts


if __name__ == "__main__":
    main()
