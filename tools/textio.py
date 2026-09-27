"""Text files in this repository are UTF-8 with LF line endings. Python on Windows defaults text-mode files to the locale
code page and writes CRLF, which garbles non-ASCII text and makes regenerated files differ from the committed ones.
Importing this module first makes text-mode open() (and so pathlib's read_text / write_text) use UTF-8 and write LF on
Windows, as on macOS and Linux. It changes nothing elsewhere, and binary modes are untouched."""
import builtins
import io
import sys

if sys.platform == "win32":
    _open = io.open

    def _utf8_open(file, mode="r", buffering=-1, encoding=None, errors=None, newline=None, closefd=True, opener=None):
        if "b" not in mode:
            if encoding in (None, "locale"):   # pathlib passes "locale" for "not given" on Python 3.10+
                encoding = "utf-8"
            if newline is None and any(c in mode for c in "wax+"):
                newline = "\n"
        return _open(file, mode, buffering, encoding, errors, newline, closefd, opener)

    builtins.open = io.open = _utf8_open

    for _stream in (sys.stdout, sys.stderr):     # the console code page cannot print every character in the tables
        if hasattr(_stream, "reconfigure"):
            _stream.reconfigure(encoding="utf-8", errors="replace")
