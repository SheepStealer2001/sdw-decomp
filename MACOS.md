# Building on macOS

The tools are Python and run on macOS as they are. The Microsoft compiler and linker are 32-bit Windows programs: on macOS they run under **CrossOver**, which is the tested path and the default, or under plain Wine (BUILDING.md §5 and §7). This page holds only what is specific to macOS; [BUILDING.md](BUILDING.md) is the reference for every step, and [GETTING_STARTED.md](GETTING_STARTED.md) is the step-by-step guide for Windows.

## What to install

- **Xcode's command-line tools** (`xcode-select --install`): they provide `python3`, LLVM's `objdump` and `cc`. Any Python 3.9 or newer works, for example one from python.org.
- **CrossOver**, to run `CL.EXE` and `LINK.EXE`. The tools expect its `wine` at `/Applications/CrossOver.app/Contents/SharedSupport/CrossOver/bin/wine`; set `SDW_CROSSOVER_WINE` if yours is elsewhere.
- Your copy of the game and your copies of the Microsoft files, placed as BUILDING.md §1 and §4 say. The Microsoft files are copied out of an installation of those products (GETTING_STARTED.md step 5 names the product and folder of each); the Mac needs only the files. Check them against BUILDING.md §4 with `shasum -a 256 work/vc6/bin/* work/vc6/lib/* work/vc6/native/*`.

## The compiler bottle

`tools/vc6.py` runs `CL.EXE` in a CrossOver bottle named `sdw-vc6` inside `work/vc6/bottles/`. Make it once, from the repository root. It must be a 64-bit bottle (the link step puts Microsoft's MSVCRT.DLL in its `syswow64` folder); a plain Windows 7 one works:

```bash
CX_BOTTLE_PATH="$PWD/work/vc6/bottles" /Applications/CrossOver.app/Contents/SharedSupport/CrossOver/bin/cxbottle \
    --bottle sdw-vc6 --create --template win7_64 --description "VC6 SP5 + Processor Pack"
```

With `SDW_VC6_DIR` set, the bottles live in its `bottles/` folder: use that for `CX_BOTTLE_PATH`.

Keep it separate from any bottle you play the game in. Running CrossOver's `wine` from a shell can make macOS ask whether it may modify apps: dismiss the prompt; the build does not need that permission.

## The link bottle

LINK must run on Microsoft's MSVCRT.DLL 6.0.8168 (BUILDING.md §7 says why). CrossOver treats `msvcrt` as a KnownDLL, so a DLL placed next to LINK is ignored. `tools/build_exe.py` therefore clones the compiler bottle into a second bottle, `work/vc6/bottles/sdw-link`, the first time it links (it prints `creating the link bottle ...`), puts `work/vc6/native/MSVCRT.DLL` into that bottle's `syswow64` as its `msvcrt.dll`, and runs LINK there with `msvcrt=n,b`. Every link checks the loader trace to be sure the native DLL was loaded, and stops if it was not.

## Building

From the repository root, in Terminal:

```bash
shasum "Sheep, Dog 'n' Wolf (PAL Version)/SheepD3D.exe"      # ca39374d53ae0030c5bd8c90dda45465e446dfe3
mkdir -p work
objdump -d --x86-asm-syntax=intel --no-show-raw-insn "Sheep, Dog 'n' Wolf (PAL Version)/SheepD3D.exe" > work/SheepD3D.asm
python3 tools/pe_index.py
python3 tools/scenaric_to_c.py
python3 tools/structs_to_c.py
cc -fsyntax-only -std=c11 work/layout_check.c                   # prints nothing when every layout assertion holds
python3 tools/vc6.py src/objects/anvil.cpp                      # 5 functions, each "MATCH"
python3 tools/build_exe.py                                      # ends "== the original: BYTE-IDENTICAL"
```

`python3 tools/vc6.py --all -j 6` compiles the whole tree in about 80 seconds under CrossOver on a recent Mac; `build_exe.py` runs that and then links. Its summary line reads `native MSVCRT confirmed` where the Windows one reads `MSVCRT 6.0.8168 as MSVCR6.DLL`.

The build commands print what GETTING_STARTED.md step 6 shows for them, with `/` in paths; the lines described there as expected (`D4025`, `NOADDR`, `0 functions:`, LINK's `LNK4108`) appear here too.

## When something fails

- **`compile failed` for every file**: the compiler bottle is missing or not at `work/vc6/bottles/sdw-vc6` (see "The compiler bottle").
- **`LINK did not run on the native MSVCRT`**: the link bottle does not hold Microsoft's DLL. Delete `work/vc6/bottles/sdw-link`; the next `build_exe.py` makes it again.

## Copying the repository to another machine

macOS keeps extended attributes in hidden `._` files when it copies to a disk or an archive that cannot store them, and the tools on the other side read those as source files and tables. Make archives with `COPYFILE_DISABLE=1 tar ...`, or delete the `._` files afterwards (GETTING_STARTED.md, "Troubleshooting").

Keep the repository on an APFS or Mac OS Extended disk: on an exFAT or FAT disk macOS can write these files beside the files there, and the tools read them on the Mac as well.
