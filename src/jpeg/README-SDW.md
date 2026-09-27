# libjpeg in Sheep, Dog 'n' Wolf

SheepD3D.exe links the Independent JPEG Group's JPEG library, **release 6 of 2-Aug-95**, at .text 0x4222c0-0x42dae0, between the game's own objects. The `.c` and `.h` files in this folder are IJG's release 6 files **unmodified**, and IJG's `README` is included unaltered as their licence requires. This software is based in part on the work of the Independent JPEG Group.

## Where the files come from

`jpegsrc.v6.tar.gz`, 531,703 bytes, SHA-256 `6311cbbb24b57bc7fb68bd788a9876bb63673302fded219070b39147fb124de7`, from FUNET's archive (`ftp.funet.fi/pub/graphics/packages/jpeg/`); GWDG's copy (`ftp.gwdg.de/pub/misc/ghostscript/3rdparty/`) is byte-identical. IJG's own site keeps only 6a and later. Release 6, not 6a: the game calls `jpeg_create_decompress` with one argument (6a turned it into a macro over `jpeg_CreateDecompress(cinfo, version, size)`).

## What the game linked

The 26 files here, the decompression half of the library, in **alphabetical order** (jcomapi.c first, jutils.c last), each its own object. 159 of their functions are in the exe and byte-match (`python3 tools/vc6.py src/jpeg/*.c`); the other 8 are API functions the game never calls, which `/OPT:REF` removed (jpeg_abort_decompress, jpeg_set_marker_processor, jpeg_input_complete, jpeg_has_multiple_scans, jpeg_read_raw_data, jpeg_start_output, jpeg_finish_output, jpeg_new_colormap).

## Additions (not IJG files)

- `jconfig.h`: release 6 ships no configuration for Visual C++, so the game's developers wrote their own. This one is the configuration that reproduces every function; it leaves `HAVE_STDDEF_H` undefined (see the comment inside), which changes one register tie-break in `start_pass_1_quant`.
- `vc6.json`: build settings for `tools/vc6.py`, kept here so the IJG files stay unmodified: the switches **`/O2 /Oy- /Ob2`** (on top of the project recipe, with `/G6`), the path to the **real Visual C++ 6 headers** (the files include `<stdio.h>` and friends; supply `VC98\INCLUDE` from your own VC6 installation at `work/vc6/vc98/INCLUDE`, never commit it), and per-file address pins, because several static functions share names across files (`start_pass`, `process_restart`, ...). `tools/jpeg_pins.py` regenerates the pins.
- `README-SDW.md`: this file.

## Linker facts it shows

- `/OPT:ICF` folds identical functions only within the same linkage: libjpeg's static empty methods (term_source, start_pass_dcolor, finish_pass_1_quant, finish_pass2) share one body at 0x422c30, while the public `jpeg_mem_term` 0x42bc80 is the single `ret` that the game's own empty calls also land on, and the public `jpeg_mem_init` was folded into the game's `Video_NullFrameProc` 0x4222b0. Which copy survives is not simply the first in link order (the malloc wrappers kept the later one, the free wrappers the earlier one); a relink with the original objects reproduces it.
- `exit` is declared `noreturn` by the VC6 headers, which `error_exit` depends on.

## Stand-ins: jcapimin.c, jcmarker.c, jdtrans.c (not linked into the exe)

RECONSTRUCTION - stand-ins for missing original objects; their code is discarded by the linker. These three files are IJG release 6's own `jcapimin.c`, `jcmarker.c` and `jdtrans.c`, unmodified (so their labels live here, not in the files). None of their functions is in SheepD3D.exe: every one is unreferenced and `/OPT:REF` removes it. They are linked because the original link evidently had three more C inputs than the 26 recovered here (the Rich header counts 29 C inputs of this compiler), and because their code COMDATs, although removed, take part in `/OPT:ICF`'s sort of identical-function candidates. They reproduce: the C input count, and (with the C++ stand-ins in `src/standin/`) the original survivors of the fold groups above. Link positions: `jcapimin.c`, `jcmarker.c` before `jcomapi.c` (T037), in that order; `jdtrans.c` after `jdsample.c` (T052) - their alphabetical places in the JPEG run, and all three are in IJG's `makefile.mc6` library list. That the original linked these particular files is a hypothesis, not recovered from the exe.
