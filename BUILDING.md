# Building and checking

This is the reference for every platform. For a step-by-step start, follow **[GETTING_STARTED.md](GETTING_STARTED.md)** on Windows (from an empty PC to the byte-identical exe) or **[MACOS.md](MACOS.md)** on macOS, which also holds everything specific to CrossOver.

Everything runs from the repository root. The tools need Python 3.9 or newer and use only its standard library. Nothing from the game and nothing from Microsoft is in this repository: you supply both, and they stay in folders that `.gitignore` keeps out of commits (the game folder and `work/`).

Commands are written for a Unix shell. In a Windows Command Prompt type `python` for `python3` (the python.org installer does not make a `python3`); the tools accept forward slashes in their arguments there too. Leave out the `# …` comments after a command (a Command Prompt passes them to the program), and write `mkdir work` for `mkdir -p work`.

What you need for each level of use:

| You want to... | You need |
|---|---|
| read the source, the tables and the notes | nothing |
| list a level's objects, zones and collision (`tools/war_*.py`) | your copy of the game; run `python3 tools/scenaric_to_c.py` (§3) once, so the objects' classes have names |
| regenerate the headers and index the exe (§2, §3) | your copy of the game, LLVM's `objdump`, optionally a C compiler |
| prove that a function matches (`tools/vc6.py`) | the above, plus Visual C++ 6.0 SP5 with the Processor Pack, and, outside Windows, CrossOver or Wine (§5) |
| link the whole exe (`tools/build_exe.py`) | the above, plus LINK, CVTRES, the SDK and runtime libraries, and Microsoft's MSVCRT.DLL 6.0.8168 |
| load the names and types into Ghidra (§8) | your copy of the game, LLVM's `objdump`, and Ghidra |

## 1. Your copy of the game

Copy everything on the PAL PC disc into a folder at the repository root with exactly this name:

```
Sheep, Dog 'n' Wolf (PAL Version)/SheepD3D.exe
```

Check that it is the build this project targets:

```bash
shasum "Sheep, Dog 'n' Wolf (PAL Version)/SheepD3D.exe"
# ca39374d53ae0030c5bd8c90dda45465e446dfe3
```

On Windows: `certutil -hashfile "Sheep, Dog 'n' Wolf (PAL Version)\SheepD3D.exe" SHA1`.

The level tools also read `Levels/` from that folder, and `tools/scenaric_to_c.py` reads the disc's `Levels/Lvl-03/Scenaric_Classes.h` and `Levels/Lvl-03/GameRes.h`. The level tools name the objects' classes from a file that `scenaric_to_c.py` writes (§3): run it once before using them.

## 2. The disassembly baseline

The matcher and the header generator read a disassembly of the exe and an index built from it, both in `work/`:

```bash
mkdir -p work
objdump -d --x86-asm-syntax=intel --no-show-raw-insn "Sheep, Dog 'n' Wolf (PAL Version)/SheepD3D.exe" > work/SheepD3D.asm
python3 tools/pe_index.py
```

That is LLVM's objdump: `objdump` on macOS (from Xcode's command-line tools), `llvm-objdump` on Windows (LLVM's own installer) and on Linux (the distribution's LLVM package). Use LLVM's: GNU objdump (Linux binutils) prints the same instructions in another format (`DWORD PTR [eax],0x…`), which the header generator does not read. On Windows run it in a Command Prompt: Windows PowerShell's `>` writes UTF-16, which `pe_index.py` cannot read.

## 3. The generated headers

`src/include/scenaric_props.h` (the object classes' level properties) is generated from your copy of the game's `Levels/Lvl-03/Scenaric_Classes.h` and is not in the repository. Generate it once, before compiling anything:

```bash
python3 tools/scenaric_to_c.py
```

`src/include/sdw_types.h`, `sdw_enums.h`, `sdw_structs.h` and `sdw_classes.h` are generated from `data/` (and, for the vtables in `sdw_classes.h`, from your exe and its disassembly). They are committed, but run the generator once before the first compile: it also writes `work/class_vtables.json`, which the matcher needs to place virtual methods, and `work/struct_layout.tsv` for the Ghidra scripts. Run it again after changing anything in `data/`:

```bash
python3 tools/structs_to_c.py
cc -fsyntax-only -std=c11 work/layout_check.c     # asserts every field offset and struct size
```

The second line prints nothing when every assertion holds. Any C11 compiler does; on Windows it is `clang` from LLVM.

## 4. The Microsoft toolchain

The game was compiled with **Visual C++ 6.0 Service Pack 5 plus the Processor Pack** and linked with **LINK 6.00.8447**. Copy these files from your own installations into `work/vc6/` (or into another folder and set `SDW_VC6_DIR` to it; every tool that uses the toolchain follows that variable). The hashes are those of the copies the match results were produced with; a different build of any of them is not expected to reproduce the bytes. Microsoft does not distribute these products; GETTING_STARTED.md step 5 says which product each file comes from.

| Put it at | What it is | Version | Bytes | SHA-256 |
|---|---|---|---:|---|
| `work/vc6/bin/CL.EXE` | compiler driver (VC6 SP5) | 12.00.8804 | 51,200 | `1bf99f206271ecdbd13da2829192ea2c02e2a44c740b8d72935d5d9cb753b156` |
| `work/vc6/bin/C1.DLL` | C front end (SP5) | 12.00.8867 | 682,545 | `0792033fc3d2fec262ff95478c6a9e70a06e836ed55f0a9710200fa06c204754` |
| `work/vc6/bin/C1XX.DLL` | C++ front end (SP5) | 12.00.8964 | 1,206,323 | `f014b3bee650224adf6cb44e51f0eeac5abbf5da666fa299d5250b2f3d1937c6` |
| `work/vc6/bin/c2.dll` | back end **from the Processor Pack** (the SP5 one will not match) | 13.00.9044 | 856,113 | `6c8e3988a5ccd69a9f9829b5c7512848d71c018b8d24be3928d7b00b5f79f231` |
| `work/vc6/bin/MSPDB60.DLL` | needed by CL and LINK | 6.00.8168 | 180,276 | `7c90b80d42914562332e28c253415e72a80c6651003d08a9ffdc4ba6e27fc37c` |
| `work/vc6/bin/LINK.EXE` | linker (VC6 SP5) | 6.00.8447 | 462,901 | `9672e578fdfaa43bdb8e9c16071682988665cb90bba2904bf02f6a5576d8ffbc` |
| `work/vc6/bin/CVTRES.EXE` | resource converter, run by LINK | 5.00.1736.1 | 15,632 | `83b602ed8e69e979fc9557f482a4a4c6c9a97b4ad67b879aedeacd2b09e5b20b` |
| `work/vc6/lib/LIBCMT.LIB` | static C runtime (SP5) | SP5 | 936,402 | `28b9f04962378ec4668072f37d7fd2835cd6cacc17b40cf22002c57bd8e76714` |
| `work/vc6/native/MSVCRT.DLL` | the runtime LINK itself must run on (see §7) | 6.0.8168 | 254,005 | `fb6260b07c05bdbcb6928156da514be5b6b69f095b335692ba46c561d85bd5f8` |
| `work/vc6/vc98/INCLUDE/` | VC6's own headers (the whole folder) | VC6 | | needed by the 29 files of `src/jpeg` (IJG's library), which `vc6.py --all` and `build_exe.py` compile |
| `work/vc6/vc98/LIB/` | VC6's import libraries (the whole folder) | VC6 | | the link uses the nine below |
| `work/vc6/dx8sdk/lib/` | the DirectX 8.0 SDK's libraries (the whole folder) | DX 8.0 | | the link uses the eight below |

The libraries the link reads, in link order (paths under `work/vc6/`; on a case-sensitive file system use exactly these spellings):

| File | Bytes | SHA-256 |
|---|---:|---|
| `dx8sdk/lib/dsetup.lib` | 5,510 | `b47d75cdae2163333182ee2229170837d024fef9a0b743c40f09a31a4014eb29` |
| `dx8sdk/lib/ddraw.lib` | 4,540 | `5d09dfd37c308e2b78a213e8ddaf547d1e785d9a56b6843e49cbfc015a8c9149` |
| `dx8sdk/lib/dinput.lib` | 19,104 | `be0e10078acc1bee14a37344755f474442e290c3f1aab8e02eb0c8162e1c93ad` |
| `dx8sdk/lib/dinput8.lib` | 20,354 | `3ae2ee8fbc392beb1a7581eb485311bb7a53bc1b728cd52692c6a6e0f19af014` |
| `dx8sdk/lib/dsound.lib` | 7,226 | `f8b660968270801ecf4b0cf0ff2d00f0687cf1fdbc3936c8e6d82ad09bec085c` |
| `dx8sdk/lib/dxguid.lib` | 104,532 | `70a74a5e9c195f73db8d6a9bd082611825d07605b67fc7bceb10171d2c194cce` |
| `vc98/LIB/WINMM.LIB` | 43,982 | `bb098679a802afdbc6fdaac03dfe1ea81453a36f6d4768b1c1cd092b1dbe9342` |
| `vc98/LIB/KERNEL32.LIB` | 175,226 | `cd4a546f71481319aeee455aa0fb51698a094f9d3e6d8339acf445489a263c68` |
| `vc98/LIB/USER32.LIB` | 135,444 | `0a3c34c4daec09c1b9fd163d5d0b2ee9a6cbe0b3e863ae81a1fae3b9f936fd19` |
| `vc98/LIB/GDI32.LIB` | 81,080 | `dc99cf3da3e3e6b2efeee7724a6cb400a615f8e2dde9ebf6df6b3bf3eb3d7c1b` |
| `vc98/LIB/ADVAPI32.LIB` | 117,982 | `88e77c6590be2a9139c0015c1f07965d7c261cc552de4209d6e8e8d86c4c11c4` |
| `dx8sdk/lib/amstrmid.lib` | 187,146 | `23f7dca5e15055409bbe7caa04a5f21f221bd8f5bfae0f565405387f55ad93c6` |
| `dx8sdk/lib/strmiids.lib` | 187,146 | `23f7dca5e15055409bbe7caa04a5f21f221bd8f5bfae0f565405387f55ad93c6` |
| `vc98/LIB/SHELL32.LIB` | 97,768 | `53150052405bbce4f264226a7bd86d3496bafe8c4bc2252835415826d996bd53` |
| `vc98/LIB/OLE32.LIB` | 68,152 | `f31f06ecaf4d8c5e9990cd65f752e9cc7466bf3694555c3914219cbfaf2026a9` |
| `vc98/LIB/UUID.LIB` | 1,103,672 | `deddbb6c1b5859a221a71d74ea3311053980186753b11bac4401a34c61eb5656` |
| `lib/LIBCMT.LIB` | 936,402 | (above) |
| `vc98/LIB/OLDNAMES.LIB` | 65,372 | `348a220e94f90bd9f530514ae59ff9b044b76e2056f52b45df406c12d2156afb` |

(`amstrmid.lib` and `strmiids.lib` are the same file in the DirectX 8.0 SDK.) No SDK or runtime headers are needed to compile the game's own files: the Windows, DirectX and C runtime declarations they use are in the stand-ins in `src/sdk/` (`windef.h`, `win32.h`, `mmsystem.h`, `ddraw.h`, `d3d7.h`, `dinput.h`, `dsound.h`, `mmstream.h`, `crt.h`); only `src/jpeg` includes the real VC6 headers, from `work/vc6/vc98/INCLUDE/`.

## 5. Running the compiler

`tools/vc6.py` runs `CL.EXE` in one of three ways:

- **Windows.** `CL.EXE` runs directly (Python for Windows).
- **macOS with CrossOver (the default everywhere but Windows).** In a compiler-only bottle, `work/vc6/bottles/sdw-vc6`: MACOS.md says how to make it. `SDW_CROSSOVER_WINE` names CrossOver's `wine` if it is not at `/Applications/CrossOver.app/Contents/SharedSupport/CrossOver/bin/wine`.
- **Linux, or macOS with plain Wine.** Set `SDW_WINE` to your `wine` binary (without it the tool looks for CrossOver), and optionally `SDW_WINEPREFIX` to the prefix to run it in (a 64-bit prefix is fine: CL is a 32-bit program).

Check one file, then the whole tree:

```bash
python3 tools/vc6.py src/objects/anvil.cpp        # 5 functions, each "MATCH"
python3 tools/vc6.py --all -j 6                   # every file; rewrites data/match_results.json
```

`--all -j 6` compiles six files at a time. It first compiles the generated headers on their own (every `SDW_AT` / `SDW_SIZE` check in them is a layout proof by the original compiler) and prints two `== headers (…): compile, every layout check holds` lines, then one verdict line per function and a total. The whole tree takes about 80 seconds under CrossOver on a recent Mac; on Windows 10 on an eight-core PC, `tools/build_exe.py` compiles and links it in 35 seconds.

`python3 tools/vc6.py FILE --diff NAME` shows an instruction diff for one function. It needs GNU objdump for 32-bit Windows, `i686-w64-mingw32-objdump`, on the PATH: `brew install mingw-w64` on macOS, the package `binutils-mingw-w64-i686` on Debian or Ubuntu. It is not tested on Windows.

## 6. Per-object layout

Every source file stands for one original object file. `tools/tu_status.py` places each compiled object at its original addresses the way LINK 6 would and compares every byte of its code, data and COMDATs with the exe:

```bash
python3 tools/vc6.py --all -j 6        # objects in work/match/
python3 tools/link_census.py           # where every function and reference lands: work/link/census.json
python3 tools/tu_status.py             # one verdict per object: IDENTICAL / DIFFERS / NO OBJECT
```

`tu_status.py` lists only the objects that are not IDENTICAL and ends with `== N objects: …` (every one IDENTICAL when all is well). Skip the first line if you have just compiled the tree.

## 7. Linking the exe

```bash
python3 tools/build_exe.py             # compile everything, link, stamp the timestamp, compare
python3 tools/build_exe.py --no-compile   # link the objects already in work/match/
```

It links the objects in the original order (`data/tu_map.json`) with the libraries above and a `.res` rebuilt from the `.rsrc` section of your own exe (`tools/exe_res.py`), sets the original header timestamp, writes `work/link/SheepD3D.exe` (`--out FILE` writes it elsewhere) and compares it with the original byte by byte. When all is well the last line is `SHA-1 ca39374d53ae0030c5bd8c90dda45465e446dfe3  == the original: BYTE-IDENTICAL`; otherwise it prints how many bytes differ in each section. `work/link/compare.json` holds the same comparison. `-j N` sets how many files the compile step compiles at once (6 by default).

**Why LINK needs Microsoft's MSVCRT.DLL 6.0.8168.** `/OPT:ICF` folds identical functions, and which copy survives is decided by sorting the candidates with the C runtime's `qsort` - the runtime LINK itself is running on. Wine's own `msvcrt.dll`, and the newer one that Windows ships, order equal keys differently, so the link must load Microsoft's 6.0.8168. How that is arranged:

- **Windows.** Windows always loads its own, newer `msvcrt.dll` for that name (a KnownDLL that cannot be overridden per program). So the tool makes copies of LINK.EXE, MSPDB60.DLL and CVTRES.EXE in `work/link/win/` whose import of `MSVCRT.dll` is renamed to `MSVCR6.dll`, and puts `work/vc6/native/MSVCRT.DLL` beside them under that name, which is not a KnownDLL. Your files in `work/vc6/` are not changed, and nothing is installed into Windows. Tested on Windows 10: the build is byte-identical.
- **CrossOver (the default everywhere but Windows).** A second bottle holds the native DLL; the tool makes it once and checks the loader trace on every link. MACOS.md has the details.
- **Plain Wine.** Set `SDW_WINE` (as for the compiler). LINK runs in the prefix named by `SDW_LINK_WINEPREFIX`, else `SDW_WINEPREFIX`, else `WINEPREFIX`, else `~/.wine`, with `WINEDLLOVERRIDES=msvcrt=n,b`. That prefix's 32-bit `msvcrt.dll` (in `drive_c/windows/syswow64` for a 64-bit prefix, `system32` for a 32-bit one) must be Microsoft's 6.0.8168: copy `work/vc6/native/MSVCRT.DLL` there yourself, or pass `--install-msvcrt`, which does it and keeps Wine's file as `msvcrt.dll.wine-builtin`. Use a prefix you keep for this. The tool checks the loader trace as under CrossOver. (This mode is untested; Windows and CrossOver are the tested paths.)

## 8. Ghidra (optional)

The names and types in `data/` can be loaded into your own Ghidra project, so the decompiler shows `this->speed` instead of raw offsets. Import `SheepD3D.exe` once with Ghidra's auto-analysis (`ghidra/` is kept out of commits; `analyzeHeadless` is in Ghidra's `support/` folder):

```bash
mkdir -p ghidra
analyzeHeadless "$PWD/ghidra" SDW -import "Sheep, Dog 'n' Wolf (PAL Version)/SheepD3D.exe"
```

Then run the scripts in `tools/ghidra/` headless, with the same project folder and name (they take their inputs as arguments and do not prompt for them). Close the project in the Ghidra window first: only one process may hold it.

```bash
analyzeHeadless <project dir> <project name> -process SheepD3D.exe -noanalysis -scriptPath tools/ghidra -postScript <Script>.java <args>
```

1. `ImportScenaricTypes.java src/include/scenaric_props.h` - the designer-property structs and the class-id enum.
2. `ApplySymbols.java data/symbols_auto.csv data/symbols_modules.csv data/symbols.csv` - every name, later files win.
3. `ApplyClassTypes.java src/include work/struct_layout.tsv` - every struct and class, with methods moved into their class so `this` is typed (run `tools/structs_to_c.py` first; it writes the TSV).
4. `ExportDecomp.java <repo>/work` - Ghidra's pseudo-C for every function into `work/decomp/`, and `work/ghidra_functions.json` (callers, callees, sizes; `tools/partition.py` and `find_library_ranges.py` read it). Pass the parent `work`, not `work/decomp`. The matcher then prefers its function sizes to the committed `data/function_sizes.json`: if `vc6.py` reports DIFFs after an export that it did not report before, delete `work/ghidra_functions.json`.

Two more for repairs: `SeedFunctions.java work/seed_functions.txt` (as a `-preScript`, with analysis) creates the functions only reachable through pointers that `tools/find_seeds.py` lists, and `FixFunctionSplit.java CHUNK:OWNER ...` undoes a function wrongly split in two.
