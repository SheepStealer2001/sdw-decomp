#!/usr/bin/env python3
"""Work sheet for one original object of data/tu_map.json: everything needed to rebuild it as one source file.

  python3 tools/tu_sheet.py T077

Prints the map entry (file to write, switches, language, ranges, the COMDATs it owns, its static initialisers, the
recovery notes), then every function in its .text ranges in address order with the current src/ file that holds it,
then every data item the compiled code refers to inside its .rdata/.data/.bss ranges (address, size where known, kind, names)
and the unreferenced gaps between them (bytes that still have to be defined, usually tables or strings reached only
through pointers). Read-only.
"""
import json
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import match  # noqa: E402

ROOT = Path(__file__).resolve().parents[1]


def main():
    if len(sys.argv) < 2:
        raise SystemExit(__doc__)
    m = json.load(open(ROOT / "data" / "tu_map.json"))["objects"]
    o = next((x for x in m if x["id"] == sys.argv[1]), None)
    if o is None:
        raise SystemExit("no object %s" % sys.argv[1])
    ev = json.load(open(ROOT / "work" / "tu" / "evidence.json"))
    census = json.load(open(ROOT / "work" / "link" / "census.json"))
    by_name, by_addr = match.load_symbols()
    h = lambda x: int(x, 16)
    print("%s  ->  %s   (%s, %s)" % (o["id"], o["file"], o.get("language"), o.get("switches")))
    print("guessed original name: %s" % o.get("name_guess"))
    for k in ("text_main", "rdata", "data", "bss"):
        if o.get(k):
            print("  %-9s %s - %s" % (k, o[k][0], o[k][1]))
    for c in o.get("text_comdats") or []:
        print("  COMDAT    %s" % c)
    if o.get("xcu"):
        print("  static initialisers (.CRT$XCU order): %s" % ", ".join(o["xcu"]))
    print("  current files: %s" % ", ".join(o.get("current_files") or []))
    for k in ("start_evidence", "notes"):
        if o.get(k):
            print("  %s: %s" % (k, o[k]))
    # functions
    rngs = []
    if o["text_main"]:
        rngs.append((h(o["text_main"][0]), h(o["text_main"][1]), "main"))
    for c in o.get("text_comdats") or []:
        a, b = c.split()[0].split("-")
        rngs.append((h(a), h(b), "COMDAT"))
    file_of = {}
    for c in census:
        for f in c["functions"]:
            if f["va"] is not None:
                file_of.setdefault(f["va"], (c["file"], f["symbol"]))
    print("\nFUNCTIONS (address order):")
    for f in sorted(ev["functions"], key=lambda f: f["va"]):
        where = next((kind for lo, hi, kind in rngs if lo <= f["va"] < hi), None)
        if where:
            src, sym = file_of.get(f["va"], (f.get("file") or "?", "?"))
            print("  0x%06x +%-5d %-6s %-44s %s  [%s]" % (f["va"], f["span"], where, f.get("name") or "?", src, sym))
    # data
    print("\nDATA referred to by code, inside this object's ranges:")
    for kind, key in ((".rdata", "rdata"), (".data", "data"), (".bss", "bss")):
        if not o.get(key):
            continue
        lo, hi = h(o[key][0]), h(o[key][1])
        items = [it for it in ev["data_items"] if lo <= it["addr"] < hi]
        print("  %s %s-%s:" % (kind, o[key][0], o[key][1]))
        cur = lo
        for it in items:
            if it["addr"] > cur:
                print("      (0x%06x-0x%06x: %d bytes not referred to by code)" % (cur, it["addr"], it["addr"] - cur))
            names = " ".join(n for n in it["names"] if not n.startswith("$"))[:110]
            print("    0x%06x size %-6s %-24s %s" % (it["addr"], it["size"] if it["size"] is not None else "?",
                                                    "/".join(it["kinds"]), names or by_addr.get(it["addr"], "")))
            cur = max(cur, it["addr"] + (it["size"] or 1))
        if cur < hi:
            print("      (0x%06x-0x%06x: %d bytes not referred to by code)" % (cur, hi, hi - cur))


if __name__ == "__main__":
    main()
