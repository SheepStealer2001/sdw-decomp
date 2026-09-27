#!/usr/bin/env python3
"""Sizes of the data items in a VC6 assembly listing (/FAcs), by label: `?g_camera@@3UCamera@@A DB 0e0H DUP (?)` is 224
bytes. COFF symbols carry no size; the listing states every data definition, so this is where tools/tu_evidence.py gets
the size of each data item. Alignment padding (`ORG $+n`) is not
counted. Usage as a module: sizes(path) -> {label: size}."""
import re
import sys

UNIT = {"DB": 1, "DW": 2, "DD": 4, "DQ": 8, "DT": 10}
DIRECTIVE = re.compile(r"^(?:(\S+)\s+)?(DB|DW|DD|DQ|DT)\b\s*(.*)$")


def split_operands(s):
    out, cur, depth, q = [], "", 0, False
    for ch in s:
        if q:
            cur += ch
            if ch == "'":
                q = False
            continue
        if ch == ";":                      # comment
            break
        if ch == "'":
            q = True
        elif ch == "(":
            depth += 1
        elif ch == ")":
            depth -= 1
        elif ch == "," and depth == 0:
            out.append(cur.strip())
            cur = ""
            continue
        cur += ch
    if cur.strip():
        out.append(cur.strip())
    return out


def number(tok):
    tok = tok.strip()
    if tok.upper().endswith("H"):
        return int(tok[:-1], 16)
    return int(tok)


def operand_size(op, unit):
    m = re.match(r"^(\w+)\s+DUP\s*\((.*)\)$", op)
    if m:
        inner = split_operands(m.group(2))
        return number(m.group(1)) * sum(operand_size(x, unit) for x in inner)
    if op.startswith("'"):
        # a quoted string; '' inside is one quote character
        body = op[1:-1] if op.endswith("'") else op[1:]
        return len(body.replace("''", "'")) * unit
    return unit


def sizes(path):
    out, label = {}, None
    in_data = False
    for line in open(path, errors="replace"):
        line = line.rstrip("\n")
        m = re.match(r"^(\S+)\s+SEGMENT\b", line)
        if m:
            in_data = m.group(1) in ("_DATA", "CONST", "_BSS") or m.group(1).startswith("CRT$")
            label = None
            continue
        if re.match(r"^\S+\s+ENDS\b", line):
            in_data, label = False, None
            continue
        if not in_data:
            continue
        m = DIRECTIVE.match(line.strip() if not line[:1].strip() else line)
        if not m:
            if line.strip().startswith("ORG"):
                label = None                 # padding ends the item before it
            continue
        lab, d, ops = m.groups()
        if lab and not line[:1].isspace():
            label = lab
            out[label] = 0
        if label is None:
            continue
        out[label] += sum(operand_size(o, UNIT[d]) for o in split_operands(ops))
    return out


if __name__ == "__main__":
    for k, v in sizes(sys.argv[1]).items():
        print(v, k)
