# Sheep, Dog 'n' Wolf - a byte-matching decompilation

A decompilation of the 2001 PC game *Sheep, Dog 'n' Wolf* (released in North America as *Sheep Raider*), PAL version, `SheepD3D.exe` (SHA-1 `ca39374d53ae0030c5bd8c90dda45465e446dfe3`). Matching and building need that exact file: any other exe, a patched one included, fails the checks.

The source here compiles, with the game's original compiler, to the game's original machine code. A function counts as done only when it compiles to the original's bytes with every address it refers to resolving to the same place; that check is automated (`tools/vc6.py`) and its result is in `data/match_results.json`.

**How it was made:** Almost all of the legwork was done with Claude and Codex coordinating via an MCP mailbox server: reading the machine code, naming, writing the source and getting it to match, etc. In-game testing and technical knowledge about the game's inner workings came from the Sheep Raider community. Nothing is taken on trust: every function and the whole exe are checked byte for byte by the tools here to make sure everything holds up.

## Status

- **Every game function matches: 3,305 of 3,305.** Each one compiles, with Visual C++ 6.0 SP5 and the Processor Pack, to exactly the original's bytes, with every relocation resolved against the symbol tables. The bundled JPEG library (IJG release 6, unmodified, in `src/jpeg/`) matches too: all 159 of its functions in the exe (at 154 distinct addresses, because the linker folded identical ones). `data/match_results.json` holds 3,729 MATCH rows, 0 DIFF, and 78 NOADDR (compiler-generated helpers that have no address of their own); an inline helper that several files compile has a row per file. Not counted: Microsoft's static C runtime, which the link takes from the real LIBCMT.LIB.
- **The source is organised as the original object files.** There are 328 source files, one per `.obj` the game was linked from, in the original link order (`data/tu_map.json`); 12 of them (3 in `src/jpeg/`, 9 in `src/standin/`) are stand-ins for objects whose code the linker discarded ("What is decompiled, and what is not" below). **All 328 objects are byte-identical** to the originals, each checked on its own at its original addresses, code, data and COMDATs included.
- **The whole exe is byte-identical.** Linking all objects with the original LINK 6.00.8447 (`tools/build_exe.py`) gives SHA-1 `ca39374d53ae0030c5bd8c90dda45465e446dfe3`, the original's.
- **Matching is complete; readability is not.** Names and types are recovered from the code and the disc's headers, and fields that are not proven are marked *inferred*. [CONTRIBUTING.md](CONTRIBUTING.md) says how to improve them without changing a byte.

## What is decompiled, and what is not

- **The game's code:** all 3,305 functions are C++ written for this project, each compiling to the original's bytes, with no byte patches. One of them uses inline assembly: `Timer::ReadTSC` (the `cpuid` and `rdtsc` instructions, which Visual C++ 6.0 cannot express in C), as the original must have too.
- **12 stand-in objects:** the original link included object files whose code the linker then threw away. They cannot be recovered from the exe, but they still steered the link: which copy of two identical functions was kept, the order the C runtime's functions were pulled in, and a count in the exe's header. 3 are unmodified JPEG library files. The 9 in `src/standin/` were made for this project only to reproduce those effects, 4 of them in inline assembly. None adds code or data of its own to the exe.
- **Names, types and comments:** recovered, not the developers'. Object classes and their settings use the names from the developers' headers on the disc; everything else is named after what the code does. The source reproduces the machine code, not necessarily the way it was originally written.

## What is here

| Path | What it holds |
|---|---|
| `src/` | The decompiled source, one file per original object; every function carries its original address. `src/README.md` has the file table and the compiler rules the matching relies on |
| `src/include/` | The headers the sources share. `sdw_types.h`, `sdw_enums.h`, `sdw_structs.h` and `sdw_classes.h` are **generated** from `data/` by `tools/structs_to_c.py`: do not edit them by hand. `scenaric_props.h` is generated from your copy of the game (`tools/scenaric_to_c.py`) |
| `src/jpeg/` | The Independent JPEG Group's library, release 6, unmodified, under IJG's own licence |
| `data/symbols*.csv` | A name, kind and note for every function and global, by address, in three tables |
| `data/structs/`, `data/enums/` | Every recovered struct, class and enum, one CSV each, fields at exact byte offsets |
| `data/tu_map.json` | The original object files in link order, with the address ranges each one owns |
| `data/match_results.json` | The current match result: one row per compiled function with its verdict |
| `data/class_map.csv`, `data/vtable_slots.csv` | The game's object classes (id, factory, size, vtable, base chain) and virtual slot signatures |
| `data/class_bases.csv` | The plain (non-virtual) base of a struct or class, where it has one |
| `data/class_methods.csv` | The non-virtual member functions each class declares in its body |
| `data/fnptr_typedefs.csv` | Named function-pointer types for the generated headers |
| `data/function_sizes.json` | Every function's original size, for the matcher |
| `data/mod_symbols_published.csv` | Every name the mod loader has published, kept as aliases so that mods keep loading |
| `data/modules.csv` | The game code split into modules, one per class plus engine chunks (`tools/partition.py`) |
| `tools/` | The matcher, the linker driver, the mod builder, the header generators, the consistency checks, level-file readers, the level editor and Ghidra scripts. See [TOOLS.md](TOOLS.md) |
| `SDW Level Editor.app`, `SDW Level Editor.bat` | Double-click to open the level editor (Mac, Windows): a level in 3D in your browser, its objects moved, copied, brought from other levels and changed, signs' text included, saved as a mod. See [MODDING.md](MODDING.md) |
| `fixes/` | Bug fixes as patches on top of the original source, built into a separate exe by `tools/build_fixed.py`. See [fixes/README.md](fixes/README.md) |
| `mods/` | The mod loader (a stand-in `dinput8.dll` that loads mod DLLs into your own `SheepD3D.exe`), the mod API and four example mods. See [MODDING.md](MODDING.md) |
| `docs/` | Notes on the engine: conventions and key facts ([docs/00-conventions.md](docs/00-conventions.md)), the executable and the game's files, the frame loop and the object system |

## Where to start

- **To read:** [docs/00-conventions.md](docs/00-conventions.md) first, then [docs/01-executable-and-files.md](docs/01-executable-and-files.md) (the exe and the game's files), [docs/03-time-and-frame-loop.md](docs/03-time-and-frame-loop.md) and [docs/04-object-system.md](docs/04-object-system.md). [src/README.md](src/README.md) maps the source files to the original objects and lists the compiler rules the matching relies on.
- **To mod the game:** [MODDING.md](MODDING.md). A mod is a DLL that hooks the game's functions by name, a level patch that places and changes objects, replacement files, or any mix of them. Building the mod loader and the mods needs your own copy of the game, the baseline and headers of [BUILDING.md](BUILDING.md) §1-§3, and MinGW-w64 for 32-bit Windows. It does not need the Microsoft tools.
- **To change a level's objects in 3D:** double-click **SDW Level Editor**. It needs Python 3 and your installed copy of the game, from which it also takes the objects' names. It saves your changes as a mod, which the game applies once the mod loader is installed ([MODDING.md](MODDING.md)).
- **To use the level tools** (object lists, zones, collision): you need only your own copy of the game ([BUILDING.md](BUILDING.md) §1). Run `python3 tools/scenaric_to_c.py` once so they name the object classes (§3).
- **To build the exe yourself on Windows, start with [GETTING_STARTED.md](GETTING_STARTED.md)**: it goes step by step from an empty PC (installing Python and LLVM, adding your game and the Microsoft tools) to an exe that is byte for byte the original. On macOS, read [MACOS.md](MACOS.md); on Linux, [BUILDING.md](BUILDING.md) (the Microsoft tools run under Wine). [BUILDING.md](BUILDING.md) is the reference behind all of them.
- **To build a modified exe** from edited source: `python3 tools/build_mod.py` (the same setup as building the exe). It writes `work/link/SheepD3D.exe`; to play it, copy it next to the game's own `SheepD3D.exe` under another name.
- **To build the game with bug fixes:** `python3 tools/build_fixed.py` (the same setup, plus git, which applies the patches). The fixes and how to write one: [fixes/README.md](fixes/README.md).
- **To check a function or contribute:** you need your own copy of the game and of the Microsoft tools (neither is included, and neither can be). Set up with the guides above, then read [CONTRIBUTING.md](CONTRIBUTING.md).

## Conventions used in the notes

- *verified* means read in the machine code or the data bytes; *inferred* means not, and is marked.
- Level numbers are the disc's `Lvl-NN` folders unless a note says otherwise; [docs/00-conventions.md](docs/00-conventions.md) maps them to the level numbers players use.

## Credits

- **uhwot**, for [sdw_re_stuff](https://github.com/uhwot/sdw_re_stuff) (MIT): the research into the game's asset formats (WAR, DAV, SND and others), with an exporter and ImHex patterns.
- **The whole Sheep Raider Discord community**, for helping crack this game open.

## Licence

The project's own work - tools, documentation, the recovered tables and the source written for this project - is under the MIT licence (`LICENSE`; [NOTICE.md](NOTICE.md) says what it covers and what it does not). `src/jpeg/` is under the IJG's terms (`src/jpeg/README`); this software is based in part on the work of the Independent JPEG Group. *Sheep, Dog 'n' Wolf* itself belongs to its rights holders: the source reproduces the game's program and contains its data tables and strings as initialisers where the original object files defined them. `src/include/scenaric_props.h` is not included: the build generates it from a header on your copy of the game's disc. Nothing here grants any right to the game. The tools that read the game need your own copy of it, and building the exe also needs your own copies of the Microsoft tools.
