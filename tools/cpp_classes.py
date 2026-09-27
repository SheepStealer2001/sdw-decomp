#!/usr/bin/env python3
"""Generate src/include/sdw_classes.h: C++ declarations of every recovered struct and class, for sources compiled with VC6.

Run by tools/structs_to_c.py (after it has written the C headers); can also be run alone. Inputs: data/structs/*.csv (fields,
absolute offsets), data/class_map.csv (scenaric classes: vtable, base-vtable chain, object size), the symbol tiers (vtable names
of the other polymorphic classes, names of slot implementations), data/vtable_slots.csv (slot names/signatures that cannot be
read off an implementation's name), and SheepD3D.exe (the vtables themselves). Outputs: the header, and work/class_vtables.json,
which tools/match.py uses to place a virtual method by its vtable slot when its name alone does not find it.

What a class gets:
  - its base class (the vtable chain; scenaric classes from class_map, the rest are roots),
  - the virtual slots it introduces and the ones it overrides (a slot whose function differs from the base's), with the slot's
    name and signature; a deleting-destructor slot becomes `virtual ~Class()`; unknown signatures are `void ()` and say so,
  - its own fields - the CSV rows at or above the base's size (derived CSVs repeat base fields; those belong to the base) -
    placed by simulating VC6's /Zp8 layout and inserting explicit padding, then PROVEN: one compile-time check per field and one
    for sizeof when the object size is known. A row the layout cannot honour (overlap, misalignment) is emitted as a comment and
    listed in the output,
  - the non-virtual methods from data/class_methods.csv (class, declaration, address comment), after the virtual slots,
  - a member hook: a source file defines SDW_MEMBERS_<Class> before including this header for what stays per file: the
    constructors, the inline helpers whose expansion is tuned per file, and members spelled differently in different files. A second hook, SDW_EXTRA_<Class>, lets a file add members
    on top of a shared member header (src/game/wolf.h defines SDW_MEMBERS_Wolf; a new Wolf file adds SDW_EXTRA_Wolf).
Fields typed as another recovered struct are embedded as that type (declared first); unknown types become opaque bytes.
"""
import textio  # noqa: F401  (first: UTF-8, LF text files on Windows)
import csv, json, re, struct, sys

# A class whose name differs from its CSV only by case cannot have its own file on a case-insensitive
# filesystem (macOS): data/structs/box.csv IS data/structs/Box.csv, the 16-byte geometric box. The
# scenaric class `box` (id 14, the crate) keeps its fields in BoxCrate.csv instead.
CSV_ALIAS = {"box": "BoxCrate"}
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import structs_to_c as S  # noqa: E402
import match  # noqa: E402

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / "src" / "include" / "sdw_classes.h"
VT_JSON = ROOT / "work" / "class_vtables.json"
ASM = ROOT / "work" / "SheepD3D.asm"
TEXT = (0x401000, 0x573976)
RDATA = (0x574000, 0x578f12)
KEYWORDS = set("""and asm auto bool break case catch char class const const_cast continue default delete do double dynamic_cast else enum
explicit export extern false float for friend goto if inline int long mutable namespace new operator or private protected public
register reinterpret_cast return short signed sizeof static static_cast struct switch template this throw true try typedef typeid
typename union unsigned using virtual void volatile wchar_t while""".split())
ALIGN = {"s8": 1, "u8": 1, "char": 1, "bool8": 1, "s16": 2, "u16": 2, "s32": 4, "u32": 4, "int": 4, "float": 4, "fnptr": 4,
         "s64": 8, "u64": 8, "double": 8, "Vec3s": 2, "Box": 4}
ALIGN.update({n: 4 for n, _ in S.fnptr_typedefs()})   # named function-pointer types (data/fnptr_typedefs.csv)
DTOR = re.compile(r"_(Scalar|Vector)?DeletingDtor$")


# SDK types whose definition is the SDK stand-in's (src/sdk/windef.h): their CSVs keep the evidence and feed the C layout
# check through sdw_structs.h, and the class header includes the stand-in instead of defining them a second time.
SDK_DEFINED = {"GUID", "RECT"}


def up(x, a):
    return (x + a - 1) // a * a


def cname(s):
    s = S.ident(s)
    return s + "_" if s in KEYWORDS else s


class Gen:
    def __init__(self, paths=None, out=OUT, vt_json=VT_JSON, slots_path=None):
        self.out, self.vt_json = Path(out), vt_json
        self.exe = match.Exe(match.EXE)
        self.by_name, self.by_addr = match.load_symbols()
        paths = paths if paths is not None else sorted((ROOT / "data" / "structs").glob("*.csv"))
        self.tables = {S.ident(p.stem): list(csv.DictReader(p.open())) for p in paths if S.ident(p.stem) not in ("Vec3s", "Box")}
        self.cmap = {}
        for r in csv.DictReader((ROOT / "data" / "class_map.csv").open()):
            self.cmap[S.ident(r["name"].replace(" ", ""))] = r
        self.slot_rows, self.slot_bodies = {}, {}
        # data/class_methods.csv {class, declaration, comment}: the non-virtual members every file sees in the class body
        # a type any source file adds members to (SDW_MEMBERS_/SDW_EXTRA_ hooks): with member functions it is a class
        self.hooked = set()
        for f in (ROOT / "src").rglob("*"):
            if f.suffix in (".cpp", ".h") and "include" not in f.parts and "jpeg" not in f.parts:
                self.hooked |= set(re.findall(r"#define SDW_(?:MEMBERS|EXTRA)_(\w+)", f.read_text(errors="replace")))
        self.methods = {}
        mpath = ROOT / "data" / "class_methods.csv"
        if mpath.exists():
            for r in csv.DictReader(mpath.open()):
                self.methods.setdefault(r["class"], []).append((r["declaration"], r["comment"]))
        for r in csv.reader(l for l in Path(slots_path or ROOT / "data" / "vtable_slots.csv").open() if not l.startswith("#")):
            if r and r[0] != "class":
                sig = r[3]
                m = re.search(r"\s*(\{.*\})\s*$", sig)        # `void () {}`: the class defines this slot inline in its body
                if m:
                    self.slot_bodies[(r[0], int(r[1], 16))] = m.group(1)
                    sig = sig[:m.start()]
                self.slot_rows[(r[0], int(r[1], 16))] = (r[2], sig)
        # vtable address -> class, and each class's base
        self.vt_of, self.base_of = {}, {}
        for vt, cls in S.VT_STRUCT.items():
            self.vt_of[cls] = int(vt, 16)
        for cls, chain in S.BASE_CHAIN.items():
            self.base_of[cls] = chain[-1] if chain else None
        for cls, r in self.cmap.items():
            self.vt_of[cls] = int(r["vtable"], 16)
            chain = [S.VT_STRUCT.get(b) for b in r["base_vtables(base->derived)"].split()]
            known = [c for c in chain if c]
            self.base_of[cls] = known[-1] if known else None
        for cls, rows in self.tables.items():
            if cls in self.vt_of:
                continue
            a = self.by_name.get(cls + "_vtbl") or self.by_name.get("g_vtbl" + cls)
            if a is not None:
                self.vt_of[cls] = a
            else:
                vrow = next((r for r in rows if int(r["offset"], 16) == 0 and re.search(r"vt|vptr|vfptr", r["name"], re.I)), None)
                if vrow is not None:                              # polymorphic; the vtable row's comment usually names the vtable
                    m = re.search(r"0x57[0-9a-fA-F]{4}", vrow.get("meaning", ""))
                    v = int(m.group(0), 16) if m else None
                    self.vt_of[cls] = v if v and RDATA[0] <= v < RDATA[1] else None
        # plain (non-virtual) bases: data/class_bases.csv {class, base}. A polymorphic class puts its vfptr first and the
        # base after it (VC6's layout); the class's own rows inside the base's span belong to the base.
        self.plain_base = {}
        bases_csv = ROOT / "data" / "class_bases.csv"
        if bases_csv.exists():
            for r in csv.DictReader(bases_csv.open()):
                self.plain_base[r["class"]] = r["base"]
        self.classes = sorted(set(self.tables) | set(self.cmap) | set(self.vt_of))
        self.boundaries = self.vtable_starts()
        self.problems = []
        self.layout = {}                                          # class -> (size, align)
        self.slots = {}                                           # class -> [(name, signature, target VA)]

    # ------------------------------------------------------------ vtables
    def vtable_starts(self):
        """Every address stored as a vptr ([reg] = imm32 into .rdata), plus the known vtables: the boundaries between vtables."""
        starts = {a for a in self.vt_of.values() if a}
        pat = re.compile(r"mov\s+dword ptr \[e[a-z]{2}\], (0x57[0-9a-f]{4})$")
        for line in ASM.open():
            m = pat.search(line)
            if m:
                v = int(m.group(1), 16)
                if RDATA[0] <= v < RDATA[1]:
                    starts.add(v)
        return sorted(starts)

    def vtable_entries(self, vt):
        nxt = next((b for b in self.boundaries if b > vt), RDATA[1])
        out, a = [], vt
        while a < nxt:
            v = self.exe.u32(a)
            if v is None or not (TEXT[0] <= v < TEXT[1]):
                break
            out.append(v)
            a += 4
        return out

    def slot_name(self, cls, idx, target):
        off = idx * 4
        if (cls, off) in self.slot_rows:
            return self.slot_rows[(cls, off)]
        n = self.by_addr.get(target, "")
        if DTOR.search(n):
            return "~", None
        for pre in (cls + "_",):
            if n.startswith(pre):
                n = n[len(pre):]
                break
        else:
            n = ""
        n = re.sub(r"_default$", "", n)
        return (cname(n) if n else "vfunc_%02x" % off), None

    def build_slots(self, cls, done):
        if cls in done:
            return self.slots.get(cls, [])
        done.add(cls)
        base = self.base_of.get(cls)
        inherited = self.build_slots(base, done) if base else []
        vt = self.vt_of.get(cls)
        entries = self.vtable_entries(vt) if vt else []
        slots, decls = list(inherited), []
        for i, target in enumerate(entries):
            if i < len(inherited):
                name, sig, btarget = inherited[i]
                slots[i] = (name, sig, target)
                if target != btarget:
                    decls.append((i, name, sig, target, "override"))
            else:
                name, sig = self.slot_name(cls, i, target)
                slots.append((name, sig, target))
                decls.append((i, name, sig, target, "new"))
        if vt is not None and len(entries) < len(inherited):
            self.problems.append("%s: vtable 0x%x has %d slots, fewer than its base's %d" % (cls, vt, len(entries), len(inherited)))
        self.slots[cls] = slots
        self.slot_decls = getattr(self, "slot_decls", {})
        self.slot_decls[cls] = decls
        return slots

    # ------------------------------------------------------------ fields
    def type_info(self, ctype):
        """(C++ element type, dims, size, align) or None for an unknown element type."""
        bf = S.BITF.fullmatch(ctype.strip())
        if bf and bf.group(1) in ALIGN:
            return bf.group(1), [], S.SIZES[bf.group(1)], ALIGN[bf.group(1)]
        base, dims = S.parse_type(ctype)
        n = 1
        for d in dims:
            n *= d
        if base.endswith("*"):
            inner = base[:-1].strip()
            inner_base = inner.rstrip("*").strip()
            if inner_base in self.classes or inner_base in ("Vec3s", "Box") or inner_base in ALIGN or inner_base == "void":
                t = base
            elif re.fullmatch(r"[A-Z]\w*", inner_base):
                self.forward.add(inner_base)
                t = base
            else:
                t = "void *"
            t = t.replace("fnptr *", "fnptr *")
            return t, dims, 4 * n, 4
        if base in ALIGN:
            return base, dims, S.SIZES[base] * n, ALIGN[base]
        if base in self.layout:
            size, align = self.layout[base]
            return base, dims, size * n, align
        return None

    def emit_class(self, cls, out):
        base = self.base_of.get(cls)
        plain = self.plain_base.get(cls) if not base else None
        poly = cls in self.vt_of
        bsize, balign = self.layout.get(base, (0, 1)) if base else (0, 1)
        psize, palign = self.layout.get(plain, (0, 1)) if plain else (0, 1)
        rows = sorted(self.tables.get(CSV_ALIAS.get(cls, cls), []), key=lambda r: int(r["offset"], 16))
        known_size = int(self.cmap[cls]["obj_size"], 16) if cls in self.cmap and self.cmap[cls]["obj_size"] else None
        # first pass: the class's alignment (members + base + vptr), needed to place the first member after a new vptr
        fields = []
        for r in rows:
            off = int(r["offset"], 16)
            if off < bsize or (off == 0 and poly and re.search(r"vt|vptr|vfptr", r["name"], re.I)):
                continue
            fields.append((off, r))
        infos = [self.type_info(r["ctype"]) for _, r in fields]
        align = max([balign, palign, 4 if poly else 1] + [i[3] for i in infos if i])
        introduces_vptr = poly and not (base and base in self.vt_of)
        cur = bsize
        if introduces_vptr:
            cur = up(4, min(align, 8))                            # VC6 pads the vfptr to the class alignment (Timer: +8)
        vptr_end = cur
        if plain:
            plain_off = up(cur, palign)
            cur = plain_off + psize
            fields = [(o, r) for o, r in fields if not (plain_off <= o < cur)]
            infos = [self.type_info(r["ctype"]) for _, r in fields]
        parent = base or plain
        is_class = poly or base or plain or cls in self.methods or cls in self.hooked
        head = ("class %s%s {" if is_class else "struct %s%s {") % (cls, (" : public %s" % parent) if parent else "")
        out.append(head)
        out.append("public:")
        out.append("#ifdef SDW_MEMBERS_%s" % cls)
        out.append("    SDW_MEMBERS_%s" % cls)
        out.append("#endif")
        out.append("#ifdef SDW_EXTRA_%s" % cls)
        out.append("    SDW_EXTRA_%s" % cls)
        out.append("#endif")
        if introduces_vptr and not self.slot_decls.get(cls):
            out.append("    virtual void vfunc_00();                       // vtable not known: declared only so the class has its vptr")
        for i, name, sig, target, kind in self.slot_decls.get(cls, []):
            tn = self.by_addr.get(target, "0x%x" % target)
            if name == "~":
                out.append("    virtual ~%s();%s// +0x%02x %s" % (cls, " " * max(1, 38 - len(cls)), i * 4, tn))
                continue
            # `s32 (float v) = 0` in vtable_slots.csv: the slot is _purecall in the class that INTRODUCES it (Sound). An
            # override inherits the signature but not the marker, or every derived class would be abstract too.
            pure = bool(sig) and sig.rstrip().endswith("= 0")
            if pure:
                sig = sig.rstrip()[:-3].rstrip()
                pure = kind == "new"
            ret, params = (sig.split(" ", 1) if sig else ("void", "()"))
            if not sig:
                note = " (signature not modelled)"
            else:
                note = ""
            decl = "virtual %s %s%s%s;" % (ret, name, params, " SDW_PURE" if pure else "")
            if (cls, i * 4) in self.slot_bodies:
                decl = decl[:-1] + " " + self.slot_bodies[(cls, i * 4)]
            out.append("    %-46s // +0x%02x %s%s%s" % (decl, i * 4, tn, "" if kind == "new" else " (override)", note))
        for decl, comment in self.methods.get(cls, []):
            out.append("    %-60s %s" % (decl, comment) if comment else "    " + decl)
        if introduces_vptr and not plain and vptr_end > 4 and fields and fields[0][0] == vptr_end and \
                (infos[0] is None or infos[0][3] < vptr_end):
            # MSVC pads a new vtable pointer to the class's alignment; GCC (a mod built with MinGW) does not
            out.append("#ifdef __GNUC__")
            out.append("    u8 _vptrPad[0x%x];" % (vptr_end - 4))
            out.append("#endif")
        checks, used, bit_off, last = [], set(), None, None     # last: the previous plain field, for unions
        for (off, r), info in zip(fields, infos):
            fname = cname(r["name"].replace(".", "_"))
            while fname in used:
                fname += "_"
            meaning = re.sub(r"\s+", " ", r.get("meaning", ""))[:100]
            bf = S.BITF.fullmatch(r["ctype"].strip())
            if bf and info is not None:               # bitfields: rows at one offset share a storage unit; no offset check
                decl = "%s %s : %s;" % (bf.group(1), fname, bf.group(2))
                if bit_off == off:
                    out.append("    %-46s // +0x%03x (bits) %s" % (decl, off, meaning))
                    used.add(fname)
                    continue
                if off < cur:
                    self.problems.append("%s+0x%x %s: overlaps the previous field or the base (next free 0x%x)" % (cls, off, r["name"], cur))
                    out.append("    // +0x%03x %s %s: OVERLAP, not declared - %s" % (off, r["ctype"], fname, meaning))
                    continue
                if off > up(cur, info[3]):
                    out.append("    u8 _pad%03x[0x%x];" % (cur, off - cur))
                out.append("    %-46s // +0x%03x (bits) %s" % (decl, off, meaning))
                used.add(fname)
                bit_off, cur = off, off + info[2]
                continue
            bit_off = None
            if last is not None and off == last["off"] and info is not None and not off % info[3]:
                t, dims, size, a = info                            # another member of a union at this offset
                decl = ("%s %s%s;" % (t, fname, "".join("[%d]" % d for d in dims))).replace("* ", "*").replace(" *", " *")
                line = "        %-42s // +0x%03x %s" % (decl, off, meaning)
                if last["close"] is None:
                    out[last["idx"]] = "    " + out[last["idx"]]
                    out.insert(last["idx"], "    union {")
                    out.append(line)
                    out.append("    };")
                    last["close"] = len(out) - 1
                else:
                    out.insert(last["close"], line)
                    last["close"] += 1
                cur = max(cur, off + size)
                used.add(fname)
                checks.append((fname, off))
                continue
            if off < cur:
                self.problems.append("%s+0x%x %s: overlaps the previous field or the base (next free 0x%x)" % (cls, off, r["name"], cur))
                out.append("    // +0x%03x %s %s: OVERLAP, not declared - %s" % (off, r["ctype"], fname, meaning))
                continue
            if info is None:                                      # unknown element type: opaque, up to the next field
                nxt = next((o for o, _ in fields if o > off), known_size or off + 4)
                n = max(nxt - off, 1)
                if off > cur:
                    out.append("    u8 _pad%03x[0x%x];" % (cur, off - cur))
                out.append("    %-46s // +0x%03x (%s) %s" % ("u8 %s[0x%x];" % (fname, n), off, r["ctype"].strip(), meaning))
                cur = off + n
            else:
                t, dims, size, a = info
                if off % a:
                    self.problems.append("%s+0x%x %s: %s is not %d-aligned there; declared as opaque bytes" % (cls, off, r["name"], r["ctype"], a))
                    if off > cur:
                        out.append("    u8 _pad%03x[0x%x];" % (cur, off - cur))
                    out.append("    %-46s // +0x%03x (%s, misaligned) %s" % ("u8 %s[0x%x];" % (fname, size), off, r["ctype"].strip(), meaning))
                    cur = off + size
                else:
                    if off > up(cur, a):
                        out.append("    u8 _pad%03x[0x%x];" % (cur, off - cur))
                    decl = "%s %s%s;" % (t, fname, "".join("[%d]" % d for d in dims))
                    decl = decl.replace("* ", "*").replace(" *", " *")
                    out.append("    %-46s // +0x%03x %s" % (decl, off, meaning))
                    cur = off + size
                    last = {"off": off, "idx": len(out) - 1, "close": None}
            used.add(fname)
            checks.append((fname, off))
        size = up(cur, align)
        if known_size is not None:
            if known_size < cur:
                self.problems.append("%s: fields end at 0x%x, past the object size 0x%x" % (cls, cur, known_size))
            elif up(cur, align) != known_size:
                out.append("    u8 _pad%03x[0x%x];" % (cur, known_size - cur))
                size = up(known_size, align)
                if size != known_size:
                    self.problems.append("%s: object size 0x%x is not a multiple of its alignment %d" % (cls, known_size, align))
        out.append("};")
        for fname, off in checks:
            out.append("SDW_AT(%s, %s, 0x%x);" % (cls, fname, off))
        if known_size is not None and size == known_size:
            out.append("SDW_SIZE(%s, 0x%x);" % (cls, known_size))
        out.append("")
        self.layout[cls] = (size, align)

    def order(self):
        """Bases before derived classes, embedded types before their containers."""
        deps = {}
        for c in self.classes:
            d = set()
            if self.base_of.get(c):
                d.add(self.base_of[c])
            if self.plain_base.get(c):
                d.add(self.plain_base[c])
            for r in self.tables.get(CSV_ALIAS.get(c, c), []):
                b, _ = S.parse_type(r["ctype"])
                if not b.endswith("*") and b in self.classes and b != c:
                    d.add(b)
            deps[c] = d
        done, out = set(), []

        def visit(c, stack=()):
            if c in done or c in stack:
                return
            for d in sorted(deps.get(c, ())):
                if d in deps:
                    visit(d, stack + (c,))
            done.add(c)
            out.append(c)
        for c in self.classes:
            visit(c)
        return out

    def run(self):
        self.forward = set()
        done = set()
        for c in self.classes:
            if c in self.vt_of or self.base_of.get(c):
                self.build_slots(c, done)
        body = []
        order = self.order()
        for c in order:
            self.emit_class(c, [] if c in SDK_DEFINED else body)     # an SDK type: its layout only
        head = ["// GENERATED by tools/cpp_classes.py (run by tools/structs_to_c.py) - do not edit; see that file's docstring.",
                "// C++ only, for the VC6 match sources. Declare a class's constructors and non-virtual methods by defining",
                "// SDW_MEMBERS_<Class> (or, on top of a shared member header, SDW_EXTRA_<Class>) before including this file.",
                "#ifndef SDW_CLASSES_H", "#define SDW_CLASSES_H", '#include "sdw_types.h"',
                '#include "../sdk/windef.h" /* %s */' % ", ".join(sorted(SDK_DEFINED)), "",
                "#define SDW_OFF(T, m) ((unsigned)&((T *)0)->m)",
                "#define SDW_AT(T, m, off) typedef char T##_##m##_at[(SDW_OFF(T, m) == (off)) ? 1 : -1]",
                "#define SDW_SIZE(T, n) typedef char T##_size_is[(sizeof(T) == (n)) ? 1 : -1]",
                "#ifdef __GNUC__ /* a mod built with MinGW: the game's pure slots are declared, not pure (arrays of them compile) */",
                "#define SDW_PURE", "#else", "#define SDW_PURE = 0", "#endif", ""]
        fwd = sorted((set(order) | self.forward) - {"Vec3s", "Box"})
        for c in fwd:
            kind = "class" if (c in self.vt_of or self.base_of.get(c) or self.plain_base.get(c) or c in self.methods or c in self.hooked) else "struct"
            head.append("%s %s;" % (kind, c))
        head.append("")
        for _name, lines in S.fnptr_typedefs():     # named function-pointer types (data/fnptr_typedefs.csv)
            head.extend(lines)
        head.append("")
        S.write_atomic(self.out, "\n".join(head + body + ["#endif"]) + "\n")
        vt = {}
        for c, slots in self.slots.items():
            if self.vt_of.get(c):
                vt[c] = {"vtable": "0x%x" % self.vt_of[c], "slots": [n if n != "~" else "~" + c for n, _, _ in slots],
                         "targets": ["0x%x" % t for _, _, t in slots]}
        if self.vt_json:
            Path(self.vt_json).write_text(json.dumps(vt, indent=1, sort_keys=True) + "\n")
        print("%s: %d classes/structs (%d with vtables)%s" % (self.out, len(order), len(vt),
              ", %d layout problems" % len(self.problems) if self.problems else ""))
        for p in self.problems[:25]:
            print("  class problem:", p)
        if len(self.problems) > 25:
            print("  ... %d more" % (len(self.problems) - 25))


def main():
    Gen().run()


if __name__ == "__main__":
    main()
