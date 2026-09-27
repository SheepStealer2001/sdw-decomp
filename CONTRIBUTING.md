# Contributing

Every game function already compiles to the original's bytes, every source file already reproduces its original object file, and the linked exe is the original, byte for byte. A contribution keeps it that way: **every function must stay MATCH, every object IDENTICAL, and the linked exe BYTE-IDENTICAL.** What remains is making the source read better (names, types, comments) without changing a byte.

Set up first as in [GETTING_STARTED.md](GETTING_STARTED.md) (Windows) or [MACOS.md](MACOS.md), through the byte-identical build. Bug fixes are not changes to `src/`: they are patches in `fixes/` ([fixes/README.md](fixes/README.md)).

## The verdicts

- **MATCH:** every byte equals the original, and every address the code refers to resolves to the same place, using the symbol tables.
- **MATCH~:** every byte equals, but some target could only be read from the original, usually because a name is missing from the tables or spelled differently there. Add or fix the name and it becomes MATCH.
- **DIFF:** a byte or a reference differs. `python3 tools/vc6.py <file> --diff <FunctionName>` shows an instruction diff.
- **NOADDR:** a function the compiler produced that has no counterpart address, typically a compiler helper.

`python3 tools/tu_status.py` (after `tools/vc6.py --all` and `tools/link_census.py`) gives the per-object verdict: IDENTICAL means the object's code, data and COMDATs land byte for byte where the original's did.

## Where things go

- A function lives in the source file of the original object that contained it (`data/tu_map.json` lists each object's address ranges; `src/README.md` has the table). Functions stay in their original order within the file.
- Keep each function's original address in a comment above it.
- Use the names from the symbol tables: the matcher finds each function by name. A free function uses its table name; a method is `Class::Method` (the table name without its `Class_` prefix).
- Every game source file is C++ and includes the generated `sdw_classes.h` (functions with C names are declared `extern "C"`); never declare your own copy of a generated class. A class's non-virtual methods are declared in its generated body, from `data/class_methods.csv`; a file defines `SDW_MEMBERS_<Class>` before the include only for what stays per file, such as constructors.
- Inline helpers that several files use have one body, in the owner's `<file>_inlines.h`: a file defines the `SDW_INLINE_<HELPER>` selectors of the helpers it uses, includes that header and undefines them again, so each file keeps its own selection and definition order (VC6 expands an inline where it was defined). The few helpers with more than one byte-proven spelling take the variant's number as the selector's value.
- A game function or global that other files use is declared once, in the header of the file that defines it (`src/<dir>/<file>.h`, or `<file>_api.h` where `<file>.h` already holds that file's member hooks). Include that header instead of writing a local prototype.
- Windows, DirectX and C runtime declarations come from the stand-ins in `src/sdk/` (`windef.h`, `win32.h`, `mmsystem.h`, `ddraw.h`, `d3d7.h`, `dinput.h`, `dsound.h`, `mmstream.h`, `crt.h`), spelled as the SDK spells them and holding only what `src/` uses. Add to them instead of declaring SDK names in a file.
- No inline assembly (outside the stand-ins in `src/standin/`, whose code the link discards) and no byte patches: the source has to compile to the bytes.

## The compiler's rules

`src/README.md` lists the measured rules of VC6 that the source relies on; read it before touching a function:

- under `/Od`, local variables get their stack slots from a **hash of their names**, not their order (`python3 tools/vc6_locals.py` predicts the layout and finds names for a wanted one). That is why some locals carry a number, such as `object_7` or `lockSize_13`: the number was chosen so the name lands in the slot the original uses. To rename one, pick a name in the same bucket (`python3 tools/vc6_locals.py order <current> <candidate>` shows both), or the frame moves;
- uninitialised globals are ordered in `.bss` by a hash of their names as well; `= 0` globals follow in definition order;
- `/Ob1` expands `inline` functions within a per-caller budget;
- when every instruction is right but their arrangement is not, the lever is the shape of the source (a variable's lifetime, a `__forceinline` split, a deferred block), not a compiler switch.

A file sets extra switches with a `match-flags:` line, pins names the tables cannot place with `match-addr:`, and names the static-initialiser roots of its globals that have constructors in a `match-init:` line, all in its first 40 lines. A shape that reproduces the bytes is a representation, not proof that the original source read that way; say so in the file.

## Changing the tables

- **A name:** add a row to `data/symbols.csv` (`address,name,kind,comment`) and say what it rests on: the instructions, the callers, a string. That file outranks `data/symbols_modules.csv` and `data/symbols_auto.csv` (see docs/00-conventions.md), so it is also how a name in those is corrected. Rename the function in `src/` too (its definition, its declaration in the owner's header, every caller) and recompile those files; they must stay MATCH. If `tools/build_mods.py` then adds rows to `data/mod_symbols_published.csv`, commit them; never remove a row from it.
- **A field:** edit the class's CSV in `data/structs/` with the offset, the type and the instruction that proves it; then `python3 tools/structs_to_c.py` and `cc -fsyntax-only -std=c11 work/layout_check.c`.
- **A signature other files call:** update every caller in the same change and recompile them; they must stay MATCH.
- Mark every claim *verified* or *inferred*.

## Before you open a pull request

```bash
python3 tools/structs_to_c.py                  # if you changed anything in data/; commit the regenerated src/include/ headers
cc -fsyntax-only -std=c11 work/layout_check.c  # prints nothing (clang on Windows)
python3 tools/check_symbol_prototypes.py       # "0 disagreed"
python3 tools/check_decl_drift.py              # only the two known cases, Box_ContainsPointXZ and Stub_Ret (src/README.md)
python3 tools/check_array_strides.py           # "0 disagree"
python3 tools/vc6.py --all -j 6                # no DIFF; rewrites data/match_results.json: commit it with the change
python3 tools/link_census.py && python3 tools/tu_status.py   # every object IDENTICAL
python3 tools/build_exe.py --no-compile        # ends "BYTE-IDENTICAL"
```

A change is merged when every line gives the result in its comment. The last line is the only check that sees which copy of identical functions the linker keeps, which any change to a code COMDAT can move.

## Never commit

- Anything from the game or derived from its bytes: the exe, level files, disassembly listings, memory dumps, and `src/include/scenaric_props.h`, which is generated from your disc.
- Anything from Microsoft: compilers, linkers, libraries, headers, DLLs.

`work/`, the game folder and `src/include/scenaric_props.h` are gitignored for this reason.
