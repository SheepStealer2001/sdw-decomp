#!/usr/bin/env python3
"""A model of MS LINK 6's section layout, checked byte for byte against SheepD3D.exe.

The byte-identical rebuild needs the rebuilt objects to land exactly where the original objects did. This tool lays out a list
of objects the way LINK does and compares every placed byte with the exe, so one translation unit can be checked
without a full link (which also needs the libraries, resources and CRT). The rules, each verified with test links:
  * per output section (.text, .rdata, .data, then .bss), objects contribute in link order, and within one object its
    sections go in the object's own section-table order (main section and COMDATs interleaved as the table has them);
  * each contribution is aligned to its section's alignment; LINK pads code with 0xCC, data with zeros;
  * a COMDAT (out-of-line inline function, ??_G/??_E, vtable ??_7, float constant __real@, /GF string ??_C) is taken
    from the FIRST object in link order that defines it; later copies are discarded;
  * /OPT:REF drops COMDATs nothing references; /OPT:ICF folds identical COMDAT functions into the first copy.

Modes:
  python3 tools/layout.py                      every src/ object (work/match/*.obj) in the order of its first function's
                                               original address: shows where the file split diverges from the exe
  python3 tools/layout.py --order LIST         objects listed one per line in LIST (paths), in link order
  python3 tools/layout.py --anchored ...       place each object at its own original address (from its functions and
                                               data as the census found them) and check it alone: a per-object verdict
  python3 tools/layout.py --tu T077 OBJ ...    check one object against its entry in the object map
                                               (data/tu_map.json, or --map): its main .text and the COMDATs it
                                               owned in the original, its .rdata/.data/.bss contributions, each at the
                                               recovered address; COMDATs owned by an earlier object are left out, as
                                               LINK discards them; references outside the object resolve to their
                                               original addresses. Exit status 0 only when every byte agrees.

Section bases come from the exe: .text 0x401000, game .rdata 0x5742cc (after the IAT), game .data 0x579100 (after the
.CRT$XC tables), game .bss 0x584c50. Externals (CRT, imports, the DirectX libraries) resolve through the symbol tables
and the IAT; a target the tables cannot place is reported, not guessed. Read-only.
"""
import argparse
import collections
import json
import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import match  # noqa: E402

ROOT = Path(__file__).resolve().parents[1]
BASES = {".text": 0x401000, ".rdata": 0x5742cc, ".data": 0x579100, ".bss": 0x584c50}
FILL = {".text": 0xCC, ".rdata": 0x00, ".data": 0x00, ".bss": 0x00}
SCN_CODE, SCN_INIT, SCN_UNINIT, SCN_COMDAT = 0x20, 0x40, 0x80, 0x1000
DIR32, DIR32NB, REL32 = match.DIR32, match.DIR32NB, match.REL32


def out_section(sec):
    n, f = sec["name"], sec["flags"]
    if n.startswith((".debug", ".drectve")) or (f & 0x800):          # LNK_REMOVE / LNK_INFO
        return None
    if n.startswith(".CRT$"):
        return ".crt"
    if f & SCN_CODE:
        return ".text"
    if f & SCN_UNINIT:
        return ".bss"
    if n == ".rdata" or not (f & 0x80000000):                         # read-only initialised data
        return ".rdata"
    return ".data"


def alignment(flags):
    a = (flags >> 20) & 0xF
    return 1 << (a - 1) if a else 16


class Program:
    def __init__(self, objs):
        self.objs = objs                                  # [(label, match.Obj)]
        self.by_name, self.by_addr = match.load_symbols()
        self.iat = match.load_iat()
        # every section: key (obj index, section number)
        self.secs = {}
        self.comdat_key = {}                              # (oi, sn) -> COMDAT symbol name
        self.defs = {}                                    # external name -> (oi, sn, value) first definition (COMDAT: selected)
        for oi, (label, o) in enumerate(objs):
            for sn in range(1, len(o.sections)):
                s = o.sections[sn]
                kind = out_section(s)
                if kind is None:
                    continue
                self.secs[(oi, sn)] = {"obj": oi, "sn": sn, "kind": kind, "sec": s}
            # COMDAT symbol of each COMDAT section = first external symbol defined in it that is not the section symbol
            for idx in sorted(o.symbols):
                x = o.symbols[idx]
                if x["sec"] > 0 and (oi, x["sec"]) in self.secs:
                    s = o.sections[x["sec"]]
                    if s["flags"] & SCN_COMDAT and (oi, x["sec"]) not in self.comdat_key and not o.is_section_symbol(x):
                        self.comdat_key[(oi, x["sec"])] = x["name"]
                    if x["cls"] == 2 and x["name"] not in self.defs:
                        self.defs[x["name"]] = (oi, x["sec"], x["value"])
        # COMDAT selection: first object wins; later copies discarded
        self.selected = {}
        for key, name in self.comdat_key.items():
            self.selected.setdefault(name, key)
        self.discarded = {k for k, n in self.comdat_key.items() if self.selected[n] != k}

    # ---- symbols
    def target(self, oi, symidx):
        """(kind, value): ('sec', (oi, sn, off)) for something placed in the layout, ('abs', va) for an external of known address, or
        ('unknown', name)."""
        o = self.objs[oi][1]
        raw = o.symbols[symidx]
        s = o.resolve(raw)
        self.last_name = s["name"]
        if s["sec"] > 0:
            key = (oi, s["sec"])
            if key in self.discarded:                      # a discarded COMDAT copy: the selected one
                sel = self.selected[self.comdat_key[key]]
                return "sec", (sel[0], sel[1], s["value"])
            return "sec", (oi, s["sec"], s["value"])
        n = s["name"]
        if n in self.defs:
            d = self.defs[n]
            key = (d[0], d[1])
            if key in self.discarded:
                key = self.selected[self.comdat_key[key]]
            return "sec", (key[0], key[1], d[2])
        if n.startswith("__imp_"):
            base = n[6:]
            for k in (base.lstrip("_").split("@")[0], base):
                if k in self.iat:
                    return "abs", self.iat[k]
            return "unknown", n
        if n in self.by_name:
            return "abs", self.by_name[n]
        for c in match.candidates(n):
            if c in self.by_name:
                return "abs", self.by_name[c]
        va = self.vtable_slot(n)          # a base-class virtual no table names in this form: read it from the vtable
        if va is not None:
            return "abs", va
        return "unknown", n

    def vtable_slot(self, n):
        """?Method@Class@@... -> the address in Class's vtable slot for Method (work/class_vtables.json, read from the
        exe by tools/cpp_classes.py), the way match.py places virtuals the tables do not name."""
        if not hasattr(self, "_vt"):
            p = ROOT / "work" / "class_vtables.json"
            self._vt = json.load(open(p)) if p.exists() else {}
        import re as _re
        m = _re.match(r"\?([A-Za-z_]\w*)@([A-Za-z_]\w*)@@", n)
        if not m:
            return None
        v = self._vt.get(m.group(2))
        if not v or m.group(1) not in v["slots"]:
            return None
        return int(v["targets"][v["slots"].index(m.group(1))], 16)

    # ---- /OPT:REF
    def reachable(self):
        keep = {k for k, v in self.secs.items() if not (v["sec"]["flags"] & SCN_COMDAT)}
        work = list(keep)
        while work:
            oi, sn = work.pop()
            sec = self.objs[oi][1].sections[sn]
            for off, si, ty in sec["relocs"]:
                kind, v = self.target(oi, si)
                if kind == "sec":
                    k = (v[0], v[1])
                    if k in self.secs and k not in keep:
                        keep.add(k)
                        work.append(k)
        return keep

    # ---- /OPT:ICF (identical COMDAT functions fold into the first copy)
    def fold(self, keep):
        alias = {}
        for _ in range(8):
            groups = collections.OrderedDict()
            for k in sorted(keep):
                v = self.secs[k]
                if v["kind"] != ".text" or not (v["sec"]["flags"] & SCN_COMDAT) or k in self.discarded:
                    continue
                sec = v["sec"]
                rel = []
                for off, si, ty in sec["relocs"]:
                    kind, t = self.target(k[0], si)
                    if kind == "sec":
                        tk = alias.get((t[0], t[1]), (t[0], t[1]))
                        t = (tk[0], tk[1], t[2])
                    rel.append((off, ty, kind, t))
                groups.setdefault((sec["data"], tuple(rel)), []).append(k)
            changed = False
            for members in groups.values():
                for m in members[1:]:
                    if alias.get(m) != members[0]:
                        alias[m] = members[0]
                        changed = True
            if not changed:
                break
        return alias

    # ---- placement
    def place(self, keep, alias, anchors=None):
        """Section address for every kept, selected, unfolded section. anchors: {(oi, kind): address} to start an
        object's contribution to one output section at a fixed address (per-object mode)."""
        addr = {}
        cursor = dict(BASES)
        for kind in (".text", ".rdata", ".data", ".bss"):
            for oi in range(len(self.objs)):
                if anchors is not None and (oi, kind) in anchors:
                    cursor[kind] = anchors[(oi, kind)]
                for sn in range(1, len(self.objs[oi][1].sections)):
                    k = (oi, sn)
                    v = self.secs.get(k)
                    if v is None or v["kind"] != kind or k not in keep or k in self.discarded or k in alias:
                        continue
                    a = alignment(v["sec"]["flags"])
                    cursor[kind] = (cursor[kind] + a - 1) // a * a
                    addr[k] = cursor[kind]
                    cursor[kind] += v["sec"]["size"]
        for k, to in alias.items():
            if to in addr:
                addr[k] = addr[to]
        return addr

    def section_address(self, addr, oi, sn):
        k = (oi, sn)
        if k in self.discarded:
            k = self.selected[self.comdat_key[k]]
        return addr.get(k)


class TrueAddresses(dict):
    """Original address of every function and named data item the rebuilt objects define (link census): per-object checks
    resolve references to OTHER objects through these, so one misplaced object does not fail its neighbours. Static
    functions of different files can share a name (libjpeg's start_input_pass), so every address of a name is kept:
    self[name] is the first, self.all[name] the list, and within(name, lo, hi) prefers one inside a range."""
    def within(self, name, lo, hi):
        for va in self.all.get(name, []):
            if lo <= va < hi:
                return va
        return self.get(name)


def true_addresses():
    out = TrueAddresses()
    out.all = collections.defaultdict(list)
    out.objs = collections.defaultdict(set)              # name -> the census objects that define it
    for o in json.load(open(ROOT / "work" / "link" / "census.json")):
        for f in o["functions"]:
            out.objs[f["symbol"]].add(o["obj"])
            if f["va"] is not None:
                out.setdefault(f["symbol"], f["va"])
                if f["va"] not in out.all[f["symbol"]]:
                    out.all[f["symbol"]].append(f["va"])
        for r in o["refs"]:
            if r["defined"] and not r["section_symbol"] and not r["local"]:
                out.setdefault(r["symbol"], r["target"])
                if r["target"] not in out.all[r["symbol"]]:
                    out.all[r["symbol"]].append(r["target"])
    # names the objects only refer to (CRT and SDK symbols, globals no source file defines): the census read their
    # original addresses at byte-matched call sites; used only after every definition above
    for o in json.load(open(ROOT / "work" / "link" / "census.json")):
        for r in o["refs"]:
            if not r["defined"] and not r["section_symbol"] and r["symbol"] not in out:
                out[r["symbol"]] = r["target"]
                out.all[r["symbol"]].append(r["target"])
    return out


def check(prog, addr, exe, true_addr=None):
    """Apply relocations and compare each placed section with the exe. Returns per-section results. With true_addr
    (per-object mode), a target defined in another object resolves to its original address."""
    results = []
    for k, a in sorted(addr.items(), key=lambda x: x[1]):
        v = prog.secs[k]
        if k in prog.discarded:
            continue
        sec = v["sec"]
        size = sec["size"]
        if v["kind"] == ".bss":
            results.append({"key": k, "kind": ".bss", "addr": a, "size": size, "same": size, "diff": [], "unknown": []})
            continue
        data = bytearray(sec["data"])
        unknown = []
        for off, si, ty in sec["relocs"]:
            if ty in match.DEBUG_RELOCS:
                continue
            kind, t = prog.target(k[0], si)
            name = prog.last_name
            raw = prog.objs[k[0]][1].symbols[si]
            if raw["cls"] == 105 and true_addr is not None and raw["name"] in true_addr:
                # a weak external (a vtable's ??_E slot): LINK binds it to a strong definition anywhere in the program
                # first, and only falls back to this object's alternate (its ??_G) when there is none
                kind, t, name = "abs", true_addr[raw["name"]], raw["name"]
            elif (kind == "abs" and true_addr is not None and raw["sec"] == 0 and raw["cls"] == 2
                  and raw["name"] in true_addr and true_addr[raw["name"]] != t):
                # an external whose exact decorated name the census placed: it beats the symbol tables' undecorated
                # lookup, which cannot tell overloads apart (T014's call to the 3-argument Texture constructor)
                kind, t, name = "abs", true_addr[raw["name"]], raw["name"]
            elif kind == "sec" and true_addr is not None and t[0] != k[0] and name in true_addr:
                kind, t = "abs", true_addr[name]
            elif kind == "unknown" and true_addr is not None and t in true_addr:
                kind, t = "abs", true_addr[t]       # a name no object defines: its original address from the census
            if kind == "sec":
                base = prog.section_address(addr, t[0], t[1])
                S = None if base is None else base + t[2]
                if S is None and true_addr is not None and name in true_addr:
                    S = true_addr[name]          # this copy is discarded (a COMDAT another object owns): the original's
                if S is None:
                    S = prog.vtable_slot(name)   # a virtual folded into another class's identical copy (ICF): the
                                                 # class's own vtable in the exe says where its slot points
            elif kind == "abs":
                S = t
            else:
                S = None
            if S is None:
                unknown.append((off, t if kind == "unknown" else "unplaced"))
                continue
            A = struct.unpack_from("<I", data, off)[0]
            P = a + off
            val = {DIR32: S + A, DIR32NB: S + A - exe.base, REL32: S + A - P - 4}.get(ty)
            if val is None:
                unknown.append((off, "reloc type 0x%x" % ty))
                continue
            struct.pack_into("<I", data, off, val & 0xFFFFFFFF)
        orig = exe.read(a, size) or b""
        masked = {o + i for o, _ in unknown for i in range(4)}
        diff = [i for i in range(size) if i not in masked and (i >= len(orig) or data[i] != orig[i])]
        results.append({"key": k, "kind": v["kind"], "addr": a, "size": size, "same": size - len(diff) - len(masked),
                        "diff": diff, "unknown": unknown})
    return results


def gaps(prog, addr, exe):
    """Padding between consecutive placed sections: LINK fills it with 0xCC (code) or zeros (data)."""
    bad = []
    for kind in (".text", ".rdata", ".data"):
        spans = sorted((a, a + prog.secs[k]["sec"]["size"]) for k, a in addr.items()
                       if prog.secs[k]["kind"] == kind and k not in prog.discarded)
        for (a0, e0), (a1, _) in zip(spans, spans[1:]):
            if a1 > e0:
                orig = exe.read(e0, a1 - e0) or b""
                if any(b != FILL[kind] for b in orig):
                    bad.append((kind, e0, a1))
    return bad


def tu_check(tu_id, obj_path, tu_map, exe, verbose=False):
    """One object against its map entry. Returns (ok, lines)."""
    entry = next((o for o in tu_map if o["id"] == tu_id), None)
    if entry is None:
        return False, ["no object %s in the map" % tu_id]
    o = match.Obj(Path(obj_path))
    prog = Program([(str(obj_path), o)])
    true_addr = true_addresses()
    h = lambda x: int(x, 16)
    comdat_ranges = [tuple(h(v) for v in c.split()[0].split("-")) for c in entry.get("text_comdats") or []]
    text_lo = min([h(entry["text_main"][0])] if entry["text_main"] else [] + [a for a, _ in comdat_ranges] or [0])
    if not entry["text_main"] and comdat_ranges:
        text_lo = min(a for a, _ in comdat_ranges)
    text_hi = max([h(entry["text_main"][1])] if entry["text_main"] else [] + [b for _, b in comdat_ranges] or [0])
    if comdat_ranges:
        text_hi = max([text_hi] + [b for _, b in comdat_ranges])
    ranges = {".text": (text_lo, text_hi)}
    for kind, key in ((".rdata", "rdata"), (".data", "data"), (".bss", "bss")):
        if entry.get(key):
            ranges[kind] = (h(entry[key][0]), h(entry[key][1]))
    # ownership: a COMDAT is this object's when its symbol's original address lies inside its range for that section kind;
    # one whose original address is unknown is kept (a new COMDAT has to be checked, not silently dropped)
    keep, dropped, removed, static_comdats = set(), [], [], set()
    own_obj = str(Path(obj_path).resolve().relative_to(ROOT)) if Path(obj_path).resolve().is_relative_to(ROOT) else str(obj_path)
    for k, v in prog.secs.items():
        if v["kind"] not in ranges or v["kind"] == ".crt":
            continue
        if not (v["sec"]["flags"] & SCN_COMDAT):
            keep.add(k)
            continue
        name = prog.comdat_key.get(k)
        if (any(sy["sec"] == k[1] and sy["name"] == name and sy["cls"] == 3 for sy in o.symbols.values())
                and own_obj not in true_addr.objs.get(name, ())):
            keep.add(k)                           # a STATIC COMDAT symbol (/O2 /Gy's initialiser _$E2) that the census
            static_comdats.add(k)                 # knows only from OTHER objects: LINK never binds a static across
            continue                              # objects, so it is this object's (ICF folding of its statics still applies)
        lo, hi = ranges[v["kind"]]
        va = true_addr.within(name, lo, hi)
        if va is None and v["kind"] == ".text":
            removed.append(name)                  # a COMDAT function the exe does not contain: /OPT:REF removed it
        elif va is None or lo <= va < hi:
            keep.add(k)
        else:
            dropped.append((name, va))
    # /OPT:ICF inside this object: identical COMDATs that share one original address survive as a single copy. Of such
    # a group, the member whose turn in section order lands on that address is the survivor; the others are folded.
    def own_va(k):
        if k in static_comdats:
            return None
        lo, hi = ranges[prog.secs[k]["kind"]]
        return true_addr.within(prog.comdat_key.get(k), lo, hi)
    shared = collections.Counter(own_va(k) for k in keep if prog.secs[k]["sec"]["flags"] & SCN_COMDAT and own_va(k))
    cursor = {kind: lo for kind, (lo, hi) in ranges.items()}
    addr, placed_at = {}, set()
    for kind in (".text", ".rdata", ".data", ".bss"):
        for sn in range(1, len(o.sections)):
            k = (0, sn)
            v = prog.secs.get(k)
            if v is None or v["kind"] != kind or k not in keep:
                continue
            a = alignment(v["sec"]["flags"])
            at = (cursor[kind] + a - 1) // a * a
            va = own_va(k) if v["sec"]["flags"] & SCN_COMDAT else None
            if va is not None and shared[va] > 1 and (va != at or va in placed_at):
                dropped.append((prog.comdat_key.get(k), va))          # folded into the copy that lands at va
                continue
            if va is not None:
                placed_at.add(va)
            cursor[kind] = at
            addr[k] = cursor[kind]
            cursor[kind] += v["sec"]["size"]
    # a COMDAT left out because its surviving copy is elsewhere (another object's, or an identical one folded by ICF)
    # is still referenced here, e.g. by this object's vtable: those references go to the survivor's address
    for name, va in dropped:
        if va is not None:
            true_addr[name] = va
    res = check(prog, addr, exe, true_addr)
    lines, ok = [], True
    for kind in (".text", ".rdata", ".data", ".bss"):
        rs = [r for r in res if r["kind"] == kind]
        size = sum(r["size"] for r in rs)
        same = sum(r["same"] for r in rs)
        unk = sum(len(r["unknown"]) for r in rs)
        want = ranges.get(kind)
        end = cursor.get(kind)
        span_note = ""
        if want:
            if end is None or end > want[1]:
                ok = False
                span_note = "  OVERRUNS the range end 0x%x" % want[1]
            else:
                gap = want[1] - end
                pad = exe.read(end, gap) if kind != ".bss" and gap else b""
                fill = FILL[kind]
                # what is left before the next object must be alignment padding: fill bytes, and rounding this end up to
                # 4, 8 or 16 lands exactly on the range end. Checking the fill alone would let a missing all-zero
                # global (or a missing .bss, which has no bytes at all) pass as padding.
                aligned = gap == 0 or any((end + a - 1) // a * a == want[1] for a in (4, 8, 16))
                if (pad and any(b != fill for b in pad)) or not aligned:
                    ok = False
                    span_note = "  %d bytes short of the range end 0x%x (%s)" % (
                        gap, want[1], "not padding" if pad and any(b != fill for b in pad) else "more than alignment")
        elif size:
            ok = False
            span_note = "  but the map gives this object no %s range" % kind
        first = next((r for r in rs if r["diff"]), None)
        if size != same or first or unk:
            ok = False
        lines.append("%-7s %7d bytes, %7d identical%s%s%s" % (
            kind, size, same, (", %d unresolved" % unk) if unk else "",
            ("; first difference at 0x%x" % (first["addr"] + first["diff"][0])) if first else "", span_note))
        if verbose:
            for r in rs:
                sec = o.sections[r["key"][1]]
                lines.append("    %-8s sec %-3d 0x%06x +%-5d %s%s" % (sec["name"], r["key"][1], r["addr"], r["size"],
                             prog.comdat_key.get(r["key"], ""), "" if not r["diff"] else "  DIFF at +0x%x" % r["diff"][0]))
                for off, what in r["unknown"]:
                    lines.append("        unresolved +0x%x %s" % (off, what))
    for name in removed:
        lines.append("  (COMDAT %s left out: not in the exe, removed by /OPT:REF)" % name)
    for name, va in dropped:
        lines.append("  (COMDAT %s left out: its original copy is at 0x%x%s)" % (
            name, va, "" if not (lo_hi := ranges.get(".text")) or not (lo_hi[0] <= va < lo_hi[1]) else ", folded into another copy here"))
    return ok, lines


def default_objects():
    """The rebuilt objects, ordered by the lowest original address of their functions (from the link census)."""
    census = json.load(open(ROOT / "work" / "link" / "census.json"))
    order = []
    for o in census:
        vas = [f["va"] for f in o["functions"] if f["va"]]
        order.append((min(vas) if vas else 1 << 32, o["obj"]))
    return [ROOT / p for _, p in sorted(order)]


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--order", type=Path, help="file listing object paths in link order")
    ap.add_argument("--from-map", action="store_true", help="link order and objects from data/tu_map.json: each object's "
                    "src/ file (the whole-program check)")
    ap.add_argument("--anchored", action="store_true", help="place each object at its own original address")
    ap.add_argument("--json", type=Path, help="write per-section results here")
    ap.add_argument("--tu", nargs=2, action="append", metavar=("ID", "OBJ"), help="check OBJ as map object ID (repeatable)")
    ap.add_argument("--map", type=Path, default=ROOT / "data" / "tu_map.json")
    ap.add_argument("--exe", type=Path, help="compare against this image instead of SheepD3D.exe (test links)")
    ap.add_argument("--bases", help="section bases, e.g. .text=0x401000,.rdata=0x402000,... (test links)")
    ap.add_argument("-v", "--verbose", action="store_true")
    a = ap.parse_args()
    if a.bases:
        for kv in a.bases.split(","):
            k, v = kv.split("=")
            BASES[k] = int(v, 16)
    if a.tu:
        tu_map = json.load(open(a.map))
        tu_map = tu_map["objects"] if isinstance(tu_map, dict) else tu_map
        exe = match.Exe(a.exe or match.EXE)
        all_ok = True
        for tu_id, obj in a.tu:
            ok, lines = tu_check(tu_id, obj, tu_map, exe, a.verbose)
            all_ok &= ok
            print("== %s  %s  %s" % (tu_id, obj, "IDENTICAL" if ok else "DIFFERS"))
            for l in lines:
                print("   " + l)
        sys.exit(0 if all_ok else 1)
    if a.from_map:
        tu_map = json.load(open(a.map))
        tu_map = tu_map["objects"] if isinstance(tu_map, dict) else tu_map
        paths, absent = [], []
        for o in tu_map:
            src = ROOT / o["file"]
            obj = ROOT / "work" / "match" / ("_".join(src.relative_to(ROOT).with_suffix("").parts) + ".obj")
            if src.exists() and obj.exists():
                paths.append(obj)
            else:
                absent.append(o["id"])
        if absent:
            print("objects without a compiled file (left out): %s" % " ".join(absent))
    else:
        paths = [Path(l.strip()) for l in open(a.order) if l.strip()] if a.order else default_objects()
    objs = [(str(p.relative_to(ROOT)) if ROOT in p.resolve().parents else str(p), match.Obj(p)) for p in paths]
    exe = match.Exe(a.exe or match.EXE)
    prog = Program(objs)
    keep = prog.reachable()
    alias = prog.fold(keep)
    anchors = anchor_map(prog) if a.anchored else None
    addr = prog.place(keep, alias, anchors)
    res = check(prog, addr, exe, true_addresses() if a.anchored else None)
    report(prog, res, gaps(prog, addr, exe), a.verbose)
    if a.json:
        a.json.write_text(json.dumps([{"obj": prog.objs[r["key"][0]][0], "section": r["key"][1], "kind": r["kind"],
                                       "addr": "0x%x" % r["addr"], "size": r["size"], "same": r["same"],
                                       "first_diff": ("0x%x" % (r["addr"] + r["diff"][0])) if r["diff"] else None,
                                       "unknown": [str(u[1]) for u in r["unknown"]]} for r in res], indent=0))


def anchor_map(prog):
    """Per-object mode: start each object's contribution to each output section where the census says its first
    placed item is in the original (its first function, or the original address of a data item it defines)."""
    census = {o["obj"]: o for o in json.load(open(ROOT / "work" / "link" / "census.json"))}
    anchors = {}
    for oi, (label, o) in enumerate(prog.objs):
        c = census.get(label)
        if not c:
            continue
        # section number -> original address of its start, from functions and from defined-data references
        start = {}
        for f in c["functions"]:
            if f["va"] is not None:
                start.setdefault(f["sec"], f["va"] - f["start"])
        for r in c["refs"]:
            if r["defined"]:
                off = r["addend"] if r["section_symbol"] else r["value"]
                start.setdefault(r["sec"], r["target"] - off)
        for sn, base in sorted(start.items(), key=lambda x: x[1]):
            v = prog.secs.get((oi, sn))
            if v and (oi, sn) not in prog.discarded and (oi, v["kind"]) not in anchors:
                anchors[(oi, v["kind"])] = base
    return anchors


def report(prog, res, bad_gaps, verbose):
    tot = collections.defaultdict(lambda: [0, 0, 0])
    per_obj = collections.defaultdict(lambda: [0, 0])
    first = {}
    for r in res:
        t = tot[r["kind"]]
        t[0] += r["size"]
        t[1] += r["same"]
        t[2] += len(r["unknown"])
        per_obj[r["key"][0]][0] += r["size"]
        per_obj[r["key"][0]][1] += r["same"]
        if r["diff"] and r["kind"] not in first:
            first[r["kind"]] = r
    for kind in (".text", ".rdata", ".data", ".bss"):
        size, same, unk = tot[kind]
        print("%-7s %9d bytes placed, %9d identical (%.2f%%), %d unresolved targets" % (
            kind, size, same, 100.0 * same / size if size else 0, unk))
        r = first.get(kind)
        if r:
            label = prog.objs[r["key"][0]][0]
            print("        first difference at 0x%x (%s section %d, +0x%x)" % (r["addr"] + r["diff"][0], label, r["key"][1], r["diff"][0]))
    print("padding gaps that are not LINK fill: %d" % len(bad_gaps))
    whole = sum(1 for oi, (n, s) in per_obj.items() if n == s)
    print("objects placed identically in every section: %d of %d" % (whole, len(per_obj)))
    if verbose:
        for oi, (n, s) in sorted(per_obj.items()):
            if n != s:
                print("   %-50s %d/%d" % (prog.objs[oi][0], s, n))


if __name__ == "__main__":
    main()
