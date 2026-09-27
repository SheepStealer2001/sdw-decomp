#!/usr/bin/env python3
"""Predict where VC6 /Od puts a function's local variables, from their NAMES - and choose names that reproduce a frame.

Measured on the compiler (C1XX 12.00.8964, project recipe) with about 1,600 test names without an exception:

    h = 0;  for each character c of the name:  h = (h << 2) + (h >> 4) + c      (32-bit)
    bucket = (h ^ (h >> 16)) & 15

The compiler keeps a function's locals in a 16-bucket hash table and hands out stack slots walking buckets 0..15, from
EBP-4 downward, each at its natural alignment. Inside one bucket the LAST declared local comes first (the higher address).
A nested block's locals come after all of the enclosing scope's; `this` and inline-expansion temporaries come after the
named locals. So the declaration order barely matters and the names decide the frame: to move a local, rename it. (The
older trick of grouping locals in a struct, as in Timer and Mobile_Steer, also works, because a struct is one local.)

  python3 tools/vc6_locals.py order NAME...              buckets, and the slot order for that declaration order
  python3 tools/vc6_locals.py frame TYPE:NAME...         predicted EBP offsets (types: u8 s16 u16 s32 u32 float double
                                                         s64 ptr, or a byte size such as 12)
  python3 tools/vc6_locals.py pick 'a,b,c' 'd,e' ...     one comma list of candidate names per slot, EBP-4 downward:
                                                         the first choice whose buckets reproduce that order, and the
                                                         declaration order that gives it
"""
import sys

SIZES = {"u8": 1, "s8": 1, "char": 1, "s16": 2, "u16": 2, "s32": 4, "u32": 4, "int": 4, "float": 4, "ptr": 4,
         "double": 8, "s64": 8, "u64": 8}


def bucket(name):
    h = 0
    for ch in name:
        h = (((h << 2) & 0xFFFFFFFF) + (h >> 4) + ord(ch)) & 0xFFFFFFFF
    return (h ^ (h >> 16)) & 15


def order(decl):
    """Names in declaration order -> names in slot order from EBP-4 downward."""
    pos = {n: i for i, n in enumerate(decl)}
    return sorted(decl, key=lambda n: (bucket(n), -pos[n]))


def frame(typed):
    """[(type, name)] in declaration order -> [(name, ebp offset)]."""
    size = {}
    for t, n in typed:
        size[n] = SIZES[t] if t in SIZES else int(t, 0)
    out, cur = [], 0
    for n in order([n for _, n in typed]):
        s = size[n]
        a = min(s, 8) if s in (1, 2, 4, 8) else 4
        cur = -(((-cur) + s + a - 1) // a * a)
        out.append((n, cur))
    return out


def pick(slots):
    def dfs(i, lo, acc):
        if i == len(slots):
            return acc
        for n in slots[i]:
            if bucket(n) >= lo and n not in acc:
                r = dfs(i + 1, bucket(n), acc + [n])
                if r:
                    return r
        return None
    r = dfs(0, 0, [])
    if not r:
        return None, None
    decl = list(reversed(r))                  # later-declared first inside a bucket: reverse frame order always works
    assert order(decl) == r
    return r, decl


def main():
    if len(sys.argv) < 3 or sys.argv[1] not in ("order", "frame", "pick"):
        raise SystemExit(__doc__)
    cmd, args = sys.argv[1], sys.argv[2:]
    if cmd == "order":
        for n in args:
            print("%-24s bucket %2d" % (n, bucket(n)))
        print("slot order from EBP-4 down:", " ".join(order(args)))
    elif cmd == "frame":
        typed = [tuple(a.split(":", 1)) for a in args]
        for n, off in frame(typed):
            print("%-24s bucket %2d  [ebp-0x%x]" % (n, bucket(n), -off))
    else:
        r, decl = pick([a.split(",") for a in args])
        if not r:
            for a in args:
                print([(n, bucket(n)) for n in a.split(",")])
            raise SystemExit("no choice keeps the buckets nondecreasing; add candidate names")
        print("frame:", " ".join("%s(%d)" % (n, bucket(n)) for n in r))
        print("declare in this order:", ", ".join(decl))


if __name__ == "__main__":
    main()
