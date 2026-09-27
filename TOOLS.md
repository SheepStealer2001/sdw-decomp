# Tools

Every tool runs from the repository root; the docstring at the top of each file says how to use it. "The game" means your copy in `Sheep, Dog 'n' Wolf (PAL Version)/`; "the baseline" means `work/SheepD3D.asm` and `work/iat.json` (BUILDING.md §2); "the generated files" means `src/include/scenaric_props.h` and `work/class_vtables.json`, which `tools/scenaric_to_c.py` and `tools/structs_to_c.py` write (BUILDING.md §3); "the toolchain" means the Microsoft files in `work/vc6/` (BUILDING.md §4). Outputs go to `work/` unless a row says otherwise.

## Build and prove

| Tool | What it does | Needs |
|---|---|---|
| `vc6.py` | Compiles source with the original compiler and byte-matches every function against the exe (MATCH / MATCH~ / DIFF / NOADDR); `--diff NAME` for an instruction diff; `--all -j 6` for the tree, which rewrites `data/match_results.json` | the game, the baseline, the generated files, the toolchain |
| `match.py` | The matcher behind `vc6.py`: compares a compiled COFF object with the exe, resolving every relocation through the symbol tables | the game, the baseline, `work/class_vtables.json` (uses `data/function_sizes.json` when there is no Ghidra export) |
| `build_exe.py` | Links all objects in the original order with LINK 6 into `work/link/SheepD3D.exe` and compares it with the original byte by byte | the game, the baseline, the generated files, the full toolchain incl. MSVCRT 6.0.8168 |
| `build_mod.py` | The same compile and link as `build_exe.py` for a modified tree: a function that does not match is reported, not fatal, and the result is compared with the original. Its compile step (`vc6.py --all`) rewrites `data/match_results.json`, and the exe goes to `work/link/SheepD3D.exe` | as `build_exe.py` |
| `build_fixed.py` | Builds `work/link/SheepD3D_fixed.exe`: the patches in `fixes/` applied to a copy of the source, compiled and linked like `build_mod.py`; `--list`, `--only NAME` | as `build_exe.py`, plus git |
| `standin.py` | Adds, checks and links the stand-in objects of `data/tu_map.json` (reconstructions of original objects whose code the linker discarded: `src/standin/`, and three unmodified IJG files in `src/jpeg/`) | as `build_exe.py` |
| `exe_res.py` | Rebuilds the game's `.res` from the `.rsrc` section of your exe (the link uses it) | the game |
| `link_census.py` | Where every compiled function lands and what every relocation points at: `work/link/census.json` | compiled objects (`vc6.py --all`) |
| `tu_status.py` | Checks every original object's layout (code, data, COMDATs) byte for byte at its original addresses | compiled objects and the census |
| `layout.py` | The model of LINK 6's section layout that `tu_status.py` uses; also checks a single object or a list | compiled objects and the census |
| `jpeg_pins.py` | Finds each compiled libjpeg function in the exe and prints its address; with `--write` it records the addresses in `src/jpeg/vc6.json` | compiled `src/jpeg` objects |
| `data_init.py` | Prints a typed C initialiser for data at an address in the exe (to define a global in place) | the game, the generated headers |
| `listing_sizes.py` | Sizes of the data items in a VC6 `/FAcs` listing (used by `tu_evidence.py`) | compiled objects |

## Consistency checks (run before a pull request)

| Tool | What it does | Needs |
|---|---|---|
| `check_symbol_prototypes.py` | Every declaration in `src/` against the prototype written in that function's symbol row; exits 1 on a disagreement | nothing |
| `check_decl_drift.py` | Functions declared with different signatures in different source files | nothing |
| `check_array_strides.py` | The array strides the code uses (`imul reg, reg, N`) against the struct sizes the tables generate | the baseline, `work/layout_check.c` |

## Names, types and headers

| Tool | What it does | Needs |
|---|---|---|
| `structs_to_c.py` | Generates `src/include/sdw_types.h`, `sdw_enums.h`, `sdw_structs.h` (and, through `cpp_classes.py`, `sdw_classes.h`) from `data/`, plus `work/layout_check.c`, `work/struct_layout.tsv` and `work/class_vtables.json` | the game, the baseline |
| `cpp_classes.py` | Generates `sdw_classes.h`: every class as C++ with its base, its vtable slots read from the exe and its fields, each offset checked by VC6 at compile time; and `work/class_vtables.json`, with which the matcher places virtual methods | the game, the baseline |
| `scenaric_to_c.py` | Converts the disc's `Scenaric_Classes.h` into `src/include/scenaric_props.h` (designer-property structs) and `work/scenaric_classes.json` | the game |
| `class_map.py` | Recovers the scenaric class table (id, factory, size, vtable, base chain) from the registration calls: `data/class_map.csv`; it also rewrites `data/symbols_auto.csv` | the baseline, a Ghidra export (`work/decomp/`), `work/scenaric_classes.json` |
| `vc6_locals.py` | Predicts where VC6 `/Od` puts local variables (it hashes their names) and finds names that give a wanted stack layout | nothing |
| `pe_index.py` | Builds the baseline index: import table, per-function callers/strings/APIs, strings | the game, `work/SheepD3D.asm` |

## Finding your way in the exe

| Tool | What it does | Needs |
|---|---|---|
| `partition.py` | Splits the game code into modules (one per class plus engine chunks) with naming coverage: `data/modules.csv` | the game, a Ghidra export |
| `find_library_ranges.py` | Recovers translation-unit boundaries from the call graph and lists how a range is entered | a Ghidra export |
| `find_seeds.py` | Function entry points reachable only through pointers, for `ghidra/SeedFunctions.java` | the game |
| `tu_evidence.py` | Evidence for the original object boundaries (padding between sections, data items, static initialisers) | the census, the game |
| `tu_sheet.py` | Work sheet for one original object of `data/tu_map.json`: its functions, data, COMDATs and notes | `tu_evidence.py`'s output, the census |

## Levels (read-only on the game's files)

| Tool | What it does | Needs |
|---|---|---|
| `war_objects.py` | Lists a level's scenaric objects with positions and, with `--props`, their designer properties | the game, and `work/scenaric_classes.json` (`scenaric_to_c.py`) for class names, the class filter and `--props` |
| `war_exports.py` | Lists a level's exported resources by their original names: zones, trigger boxes, trajectories | the game |
| `war_collision.py` | Decodes a level's static collision mesh and classifies surfaces as the movement code does | the game |
| `war_meshes.py` | Lists a level's models (mesh resources) with the texture pages each draws with, a model's entries and texture rectangles, or the `.DAV`'s pages; `all --check` checks every level | the game |

## Mods and the level editor

| Tool | What it does | Needs |
|---|---|---|
| `level_editor.py` | The level editor: a level in 3D in the browser, its objects moved, copied, brought from other levels and changed, saved as a mod's level patch in the installed game's `Mods` (MODDING.md); started by `SDW Level Editor.app` / `.bat` with a start screen, or with a level and `--mod`; `--check` tests its reading and writing of patches | the game; it generates `src/include/scenaric_props.h` (the class and property names) from the installed game's `Scenaric_Classes.h` when the repository has none |
| `build_mods.py` | Builds the mod loader (a stand-in `dinput8.dll`) and the mods in `mods/examples/` into `work/mods/`, and appends every name it has not listed yet to `data/mod_symbols_published.csv`; `--check-hooks` checks that every game function can be hooked (MODDING.md); `--class-sounds` lists the sounds each object class's code names | MinGW-w64 (32-bit); `src/include/scenaric_props.h` (`scenaric_to_c.py`: the `SpeedHoney` example includes it, and the loader takes the designer-property names from it); the baseline's `work/iat.json`, without which mods' replacements of the music and voices are not used; `--check-hooks`: the game and the baseline |
| `make_model.py` | Makes a model file a mod can import (a `.WAR` and a `.DAV` holding its textures, empty without any) from a Wavefront `.obj`: coloured or textured triangles (a `.png`), an optional collision box, and with `--rig FILE` parts that animate | the `.obj` |

`textio.py` is a module the other tools import (UTF-8, LF text files on every platform); `editor/level_editor.html` is the level editor's page.

## Ghidra scripts (`tools/ghidra/`, see BUILDING.md §8)

| Script | What it does |
|---|---|
| `ImportScenaricTypes.java` | Loads `scenaric_props.h` (property structs, class-id enum) |
| `ApplySymbols.java` | Applies the three symbol tables (later files win) |
| `ApplyClassTypes.java` | Builds every struct from `work/struct_layout.tsv` and moves methods into their class so `this` is typed |
| `ExportDecomp.java` | Writes pseudo-C for every function to `work/decomp/` and `work/ghidra_functions.json` |
| `SeedFunctions.java` | Creates functions only reachable through pointers (input from `find_seeds.py`) |
| `FixFunctionSplit.java` | Undoes a function wrongly split in two |
