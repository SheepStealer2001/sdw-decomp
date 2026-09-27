#!/usr/bin/env python3
"""Recover the scenaric class table from the registration calls in SheepD3D.exe.

The engine registers every scenaric class at startup with
    RegisterScenaricClass(classId, factoryFn, a, b, c)        // 0x50d630, cdecl, 5 args
Each factory does `p = Alloc(size)` (0x50d5f4) followed by an inlined constructor chain that writes one
vtable pointer per inheritance level, base first. Joining classId with the disc's Scenaric_Classes.h gives
name -> factory -> object size -> vtable -> base-class chain.

Live-memory note: objects sit in the engine heap behind an 8-byte header {u32 blockSize, u32 0x98765432},
so an instance can be located by scanning for  <size+8 as u32> 32 54 76 98 <vtable as u32>.

Inputs : work/SheepD3D.asm (objdump, for the registration call sites), work/decomp/ (Ghidra ExportDecomp, for the
         factories - objdump's linear sweep is misaligned at some of them), work/scenaric_classes.json
Outputs: data/class_map.csv, data/symbols_auto.csv (for tools/ghidra/ApplySymbols.java), work/class_map.json
"""
import csv
import json
import re
import struct
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
ASM = ROOT / "work" / "SheepD3D.asm"
REGISTER = 0x50D630
RDATA = (0x574000, 0x579000)
HEAP_MAGIC = 0x98765432

# Shared base classes, identified by vtable. Names are PROVISIONAL: inferred from which classes derive from them.
BASE_NAMES = {
    0x574C40: ("ScnObject", "root of every scenaric class (181/181)"),
    0x574C1C: ("ScnBody", "has a world presence; direct base of static props (Mailbox, bridge, seesaw...)"),
    0x574C84: ("ScnMobile", "movable/physical: Sam, Sheep, Dynamite, Rocket, box..."),
    0x574FD0: ("ScnControllable", "only Wolf and Robot derive from it - the player-controllable actors"),
    0x57595C: ("ScnLogic", "body-less managers/triggers: Goal, CameraManager, CheckpointManager..."),
}

LINE = re.compile(r"^\s*([0-9a-f]+):\s+(\S+)\s*(.*)$")


def load_insns():
    insns = []
    for line in ASM.open():
        m = LINE.match(line)
        if m:
            ops = re.sub(r"\s*<[^>]*>", "", m.group(3)).strip()
            insns.append((int(m.group(1), 16), m.group(2), ops))
    return insns


def imm(ops):
    m = re.fullmatch(r"(-?)0x([0-9a-f]+)|(-?\d+)", ops)
    if not m:
        return None
    if m.group(2):
        return int(m.group(2), 16) * (-1 if m.group(1) else 1)
    return int(m.group(3))


def registrations(insns):
    out = []
    for i, (addr, mn, ops) in enumerate(insns):
        if mn == "call" and imm(ops) == REGISTER:
            args = []
            j = i - 1
            while j >= 0 and len(args) < 5 and insns[j][1] == "push":
                args.append(imm(insns[j][2]))
                j -= 1
            if len(args) == 5 and None not in args:
                out.append({"site": addr, "id": args[0], "factory": args[1], "args": args[2:]})
            else:
                out.append({"site": addr, "id": None, "factory": None, "args": args})
    return out


ALLOC_RE = re.compile(r"(\w+) = \([^)]*\)FUN_0050d5f4\((0x[0-9a-f]+|\d+)\)")
VT_RE = re.compile(r"^\s*\*(\w+) = (?:\([^)]*\))?&\w*?_(005[0-9a-f]{5});", re.M)
MEMBER_RE = re.compile(r"^\s*(\w+)\[(0x[0-9a-f]+|\d+)\] = (?:\([^)]*\))?&\w*?_(005[0-9a-f]{5});", re.M)


def decomp_of(addr):
    hits = list((ROOT / "work" / "decomp").glob("%08x_*.c" % addr))
    return hits[0].read_text() if hits else None


def analyse_factory(addr):
    """Read Ghidra's decompilation of a factory: `p = Alloc(size)` then one `*p = &vtable` per inheritance
    level, base first. Writes to p[n] are embedded member objects, reported separately."""
    src = decomp_of(addr)
    if src is None:
        return None, [], []
    m = ALLOC_RE.search(src)
    if not m:
        return None, [], []
    var, size = m.group(1), int(m.group(2), 0)
    in_rdata = lambda v: RDATA[0] <= v < RDATA[1]
    chain = [int(v, 16) for name, v in VT_RE.findall(src) if name == var and in_rdata(int(v, 16))]
    members = [(int(off, 0) * 4, int(v, 16)) for name, off, v in MEMBER_RE.findall(src)
               if name == var and in_rdata(int(v, 16))]
    return size, chain, members


def check_inputs():
    """Stop before anything is written when an input is missing: data/class_map.csv and data/symbols_auto.csv are
    committed, and a run without the Ghidra export would otherwise replace them with wrong rows."""
    missing = []
    if not ASM.exists():
        missing.append("%s (objdump of the exe; see BUILDING.md section 2)" % ASM.relative_to(ROOT))
    if not any((ROOT / "work" / "decomp").glob("*.c")):
        missing.append("work/decomp/*.c (a Ghidra export: tools/ghidra/ExportDecomp.java)")
    if not (ROOT / "work" / "scenaric_classes.json").exists():
        missing.append("work/scenaric_classes.json (python3 tools/scenaric_to_c.py)")
    if missing:
        raise SystemExit("class_map.py: missing input(s), nothing written:\n  " + "\n  ".join(missing))


def main():
    check_inputs()
    insns = load_insns()
    names = {c["id"]: c for c in json.loads((ROOT / "work" / "scenaric_classes.json").read_text())}

    regs = registrations(insns)
    bad = [r for r in regs if r["id"] is None]
    rows = []
    for r in regs:
        if r["id"] is None:
            continue
        size, vts, members = analyse_factory(r["factory"])
        c = names.get(r["id"], {})
        rows.append({
            "class_id": r["id"], "name": c.get("name", "?"), "propsize": c.get("size"),
            "factory": r["factory"], "obj_size": size, "vtable": vts[-1] if vts else None,
            "bases": vts[:-1], "reg_args": r["args"], "reg_site": r["site"], "members": members,
        })
    rows.sort(key=lambda x: x["class_id"])
    unread = [x for x in rows if x["obj_size"] is None or x["vtable"] is None]
    if unread:
        raise SystemExit("class_map.py: %d factories could not be read from work/decomp/ (e.g. %s), nothing written: "
                         "re-run tools/ghidra/ExportDecomp.java" % (len(unread), ", ".join("0x%x" % x["factory"] for x in unread[:5])))

    hx = lambda v: "" if v is None else "0x%x" % v
    (ROOT / "data").mkdir(exist_ok=True)
    with (ROOT / "data" / "class_map.csv").open("w", newline="") as fh:
        w = csv.writer(fh)
        w.writerow(["class_id", "name", "factory", "obj_size", "vtable", "base_vtables(base->derived)",
                    "reg_arg3", "reg_arg4", "reg_arg5", "live_memory_aob"])
        for x in rows:
            aob = ""
            if x["obj_size"] is not None and x["vtable"]:
                aob = " ".join("%02X" % b for b in struct.pack("<III", x["obj_size"] + 8, HEAP_MAGIC, x["vtable"]))
            w.writerow([x["class_id"], x["name"], hx(x["factory"]), hx(x["obj_size"]), hx(x["vtable"]),
                        " ".join(hx(b) for b in x["bases"])] + [hx(a) for a in x["reg_args"]] + [aob])
    (ROOT / "work" / "class_map.json").write_text(json.dumps(rows, indent=1))

    ident = lambda s: re.sub(r"\W", "", s)
    with (ROOT / "data" / "symbols_auto.csv").open("w", newline="") as fh:
        w = csv.writer(fh)
        w.writerow(["address", "name", "kind", "comment"])
        w.writerow([hx(REGISTER), "Scenaric_RegisterClass", "func", "(classId, factory, a, b, c) - called once per class at startup"])
        for vt, (name, why) in sorted(BASE_NAMES.items()):
            w.writerow([hx(vt), name + "_vtbl", "vtable", "PROVISIONAL name: " + why])
        for x in rows:
            n = ident(x["name"])
            w.writerow([hx(x["factory"]), n + "_Create", "func",
                        "factory for CLASSID %d \"%s\": new(0x%x) + inlined ctor chain" % (x["class_id"], x["name"], x["obj_size"])])
            if x["vtable"] not in BASE_NAMES:
                w.writerow([hx(x["vtable"]), n + "_vtbl", "vtable", "sizeof(%s) = 0x%x" % (n, x["obj_size"])])

    ids = {x["class_id"] for x in rows}
    print("registration calls: %d (unparsed: %d)" % (len(regs), len(bad)))
    print("distinct class ids registered: %d / %d in header; missing: %s" % (
        len(ids & set(names)), len(names), sorted(set(names) - ids)[:20]))
    print("ids not in header: %s" % sorted(ids - set(names)))
    print("with size+vtable recovered: %d" % sum(1 for x in rows if x["obj_size"] and x["vtable"]))


if __name__ == "__main__":
    main()
