"""Writes crate.png, the Crate's texture (64 x 64: planks, a border and a cross brace), so the mod carries no image
file: tools/build_mods.py runs each models/*_png.py of an example before making its models."""
import struct
import sys
import zlib
from pathlib import Path

W = H = 64


def pixel(x, y):
    if x < 5 or x > 58 or y < 5 or y > 58 or abs(x - y) < 4 or abs(x - (63 - y)) < 4:
        base = (120, 78, 36)                      # the border and the brace
    elif y % 11 == 0:
        base = (110, 72, 34)                      # a gap between planks
    else:
        base = (190, 140, 80) if (y // 11) % 2 else (172, 124, 68)
    grain = ((x * 7 + y * 13) % 9) - 4
    return tuple(max(0, min(255, c + grain * 3)) for c in base)


def main(out):
    raw = b"".join(b"\0" + bytes(c for x in range(W) for c in pixel(x, y)) for y in range(H))

    def chunk(kind, body):
        return struct.pack(">I", len(body)) + kind + body + struct.pack(">I", zlib.crc32(kind + body) & 0xFFFFFFFF)

    Path(out).write_bytes(b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", struct.pack(">IIBBBBB", W, H, 8, 2, 0, 0, 0)) +
                          chunk(b"IDAT", zlib.compress(raw, 9)) + chunk(b"IEND", b""))


if __name__ == "__main__":
    main(sys.argv[1] if len(sys.argv) > 1 else Path(__file__).with_name("crate.png"))
