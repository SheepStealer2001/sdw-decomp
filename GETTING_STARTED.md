# Getting started on Windows

This guide takes a Windows PC with nothing installed to a `SheepD3D.exe` built from this repository's source that is byte for byte the game's original. Follow it from top to bottom. It was tested on Windows 10; every command runs in a **Command Prompt** (`cmd`), not PowerShell.

On macOS read [MACOS.md](MACOS.md) instead; [BUILDING.md](BUILDING.md) is the reference behind both guides.

What you need before you start:

- **Your own copy of the game**, the PAL PC release of *Sheep, Dog 'n' Wolf*. Nothing from the game is in this repository.
- **Your own copies of four Microsoft products**: Visual C++ 6.0, its Service Pack 5, the Visual C++ 6.0 Processor Pack and the DirectX 8.0 SDK (step 5 says which file comes from which). One file, `MSVCRT.DLL`, is taken from the Visual C++ 6.0 disc itself, so you need that disc or an image of it, not only an installed copy. Microsoft does not distribute any of these products, and this repository cannot include them.
- About 1 GB of free space on the drive you build on (this guide uses `D:`) for the repository, the game, the Microsoft files and the build, plus the space LLVM takes (step 2). If you build on `C:`, read `C:` wherever this guide says `D:`. If `C:` is nearly full, read the note in step 2 before installing LLVM.
- An internet connection for the downloads in steps 1 to 3.

## 1. Install Python

1. Download the latest **Python 3** "Windows installer (64-bit)" from the official site's Windows page, [python.org/downloads/windows](https://www.python.org/downloads/windows/), which lists it for each release. Any Python 3.9 or newer works.
2. Run it. On the first page, **tick "Add python.exe to PATH"** at the bottom, then click "Install Now".
3. Open a new Command Prompt (Start menu, type `cmd`, press Enter) and check:

```
python --version
```

It prints `Python 3.` followed by the version you installed. If it prints `Python was not found` or opens the Microsoft Store, the PATH box was not ticked: run the installer again, choose "Modify", click "Next" and tick "Add Python to environment variables". The `py` launcher that the installer also puts on the PATH works as well: wherever this guide says `python`, `py` does the same.

The tools use nothing but Python's standard library: there is no `pip install` step.

## 2. Install LLVM (objdump and clang)

The tools read a disassembly of the game made by LLVM's `llvm-objdump`, and one check compiles a file with `clang`.

1. Open LLVM's official releases page, [github.com/llvm/llvm-project/releases](https://github.com/llvm/llvm-project/releases), and under the latest release's "Assets" download **`LLVM-<version>-win64.exe`**.
2. Run it. On the page "Install Options", **choose one of the options that add LLVM to the system PATH** (for all users or for the current user). Any install folder works.
3. Open a new Command Prompt and check both programs:

```
llvm-objdump --version
clang --version
```

The first prints `LLVM version <version>` and a list of targets; the second `clang version <version>` and `Target: x86_64-pc-windows-msvc`.

**If `C:` is nearly full**, the installer fails even when you install LLVM on another drive, because it is first unpacked on `C:`. Use the portable archive from the same page instead: **`clang+llvm-<version>-x86_64-pc-windows-msvc.tar.xz`** (about 900 MB; save it on the drive with the space). Only three things are needed from it: `bin\llvm-objdump.exe`, `bin\clang.exe` and the folder `lib\clang`. The `tar` that comes with Windows 10 cannot unpack `.tar.xz`, but Python can. In the folder where you saved the archive, run this (one line; change the file name to the one you downloaded, and `D:\LLVM` to where you want the files):

```
python -W ignore -c "import sys,tarfile;t=tarfile.open(sys.argv[1]);[(setattr(m,'name',m.name.split('/',1)[1]),t.extract(m,sys.argv[2])) for m in t if m.isfile() and m.name.split('/',1)[1].startswith(('bin/llvm-objdump.exe','bin/clang.exe','lib/clang/'))]" clang+llvm-<version>-x86_64-pc-windows-msvc.tar.xz D:\LLVM
```

It takes a minute or two, prints nothing, and leaves about 180 MB in `D:\LLVM` (`bin` and `lib`); the archive can be deleted afterwards. Then add `D:\LLVM\bin` to your PATH: Start menu, type "environment", open **"Edit environment variables for your account"**, select `Path`, click "Edit", "New", type `D:\LLVM\bin`, and click "OK" twice. Open a new Command Prompt and run the two checks above. (To try it in one Command Prompt without changing any setting, `set PATH=D:\LLVM\bin;%PATH%` lasts until that window is closed.)

## 3. Get the repository

Either download it as a ZIP from the repository's page ("Code", then "Download ZIP"), unpack it and rename the folder it holds to `SDW`, or, if you use Git, clone it. Put it on the drive with the free space, in a short path; this guide uses `D:\SDW`:

```
git clone <the repository's URL> D:\SDW
```

From now on every command runs **in the repository's folder**. Open a Command Prompt and go there first:

```
cd /d D:\SDW
```

`dir` there lists `BUILDING.md`, `GETTING_STARTED.md`, `README.md`, `data`, `docs`, `src`, `tools` and a few more.

## 4. Add your copy of the game

Copy the game's files into the repository's folder, into a new folder named exactly

```
Sheep, Dog 'n' Wolf (PAL Version)
```

so that the game's program is at `D:\SDW\Sheep, Dog 'n' Wolf (PAL Version)\SheepD3D.exe`. The tools need `SheepD3D.exe` and the `Levels` folder beside it (including the disc's `Levels\Lvl-03\Scenaric_Classes.h` and `GameRes.h`), so the simplest is to copy everything on the game's PAL disc (about 550 MB) into that folder (a disc image works too: double-click it to mount it as a drive), with Explorer or, if the disc is drive `E:`:

```
robocopy E:\ "Sheep, Dog 'n' Wolf (PAL Version)" /E
```

Its summary table should show `0` in the `FAILED` column. Then check that it is the exact build the project reproduces:

```
certutil -hashfile "Sheep, Dog 'n' Wolf (PAL Version)\SheepD3D.exe" SHA1
```

```
SHA1 hash of Sheep, Dog 'n' Wolf (PAL Version)\SheepD3D.exe:
ca39374d53ae0030c5bd8c90dda45465e446dfe3
CertUtil: -hashfile command completed successfully.
```

The second line must be exactly **`ca39374d53ae0030c5bd8c90dda45465e446dfe3`**. Any other value means another build (another region's release, or an exe that has been patched), and nothing below will match it.

The game folder is listed in `.gitignore`: it can never be committed by accident.

## 5. Add the Microsoft tools

The game was compiled with Visual C++ 6.0 and Service Pack 5, with the compiler back end of the Processor Pack, and linked against the DirectX 8.0 SDK. Copy the files below from your own copies of those products into a folder `work\vc6` in the repository (`work` is also in `.gitignore`). Create the folders first:

```
mkdir work\vc6\bin work\vc6\lib work\vc6\native work\vc6\vc98 work\vc6\dx8sdk
```

| Microsoft product | Files | Put them in |
|---|---|---|
| **Visual C++ 6.0** (also part of Visual Studio 6.0) | `MSPDB60.DLL` version 6.00.8168 | `work\vc6\bin\` |
| | its `INCLUDE` and `LIB` folders, whole | `work\vc6\vc98\INCLUDE\` and `work\vc6\vc98\LIB\` |
| | `MSVCRT.DLL` version 6.0.8168, from the `OS\SYSTEM` folder of the Visual C++ 6.0 disc | `work\vc6\native\` |
| **Visual Studio 6.0 Service Pack 5** (for Visual C++ 6.0) | `CL.EXE` 12.00.8804, `C1.DLL` 12.00.8867, `C1XX.DLL` 12.00.8964, `LINK.EXE` 6.00.8447, `CVTRES.EXE` 5.00.1736.1 | `work\vc6\bin\` |
| | `LIBCMT.LIB` (the static C runtime) | `work\vc6\lib\` |
| **Visual C++ 6.0 Processor Pack** | `c2.dll` 13.00.9044, the compiler's back end: it replaces Service Pack 5's `C2.DLL` | `work\vc6\bin\` |
| **DirectX 8.0 SDK** (8.0, not 8.1) | its `lib` folder, whole | `work\vc6\dx8sdk\lib\` |

Nothing has to be installed on this PC: the build runs the programs from `work\vc6`. The simplest way to get the files is to install Visual C++ 6.0, then Service Pack 5, then the Processor Pack, in that order, on this PC or on any other (a virtual machine works too), and copy them out of that installation. Inside its `Microsoft Visual Studio` folder, the programs and `c2.dll` are in `VC98\Bin`, `MSPDB60.DLL` in `Common\MSDev98\Bin`, `LIBCMT.LIB` in `VC98\LIB`, and the folders to copy whole are `VC98\INCLUDE` and `VC98\LIB`. These commands copy all of them; change the first line to your `Microsoft Visual Studio` folder (the one shown is an example):

```
set VS=C:\Program Files (x86)\Microsoft Visual Studio
copy "%VS%\VC98\Bin\CL.EXE" work\vc6\bin
copy "%VS%\VC98\Bin\C1.DLL" work\vc6\bin
copy "%VS%\VC98\Bin\C1XX.DLL" work\vc6\bin
copy "%VS%\VC98\Bin\C2.DLL" work\vc6\bin
copy "%VS%\VC98\Bin\LINK.EXE" work\vc6\bin
copy "%VS%\VC98\Bin\CVTRES.EXE" work\vc6\bin
copy "%VS%\Common\MSDev98\Bin\MSPDB60.DLL" work\vc6\bin
copy "%VS%\VC98\LIB\LIBCMT.LIB" work\vc6\lib
robocopy "%VS%\VC98\INCLUDE" work\vc6\vc98\INCLUDE /E
robocopy "%VS%\VC98\LIB" work\vc6\vc98\LIB /E
```

Copy the DirectX 8.0 SDK's `lib` folder the same way, to `work\vc6\dx8sdk\lib`. `MSVCRT.DLL` comes from the Visual C++ 6.0 disc, not from the installation (`copy E:\OS\SYSTEM\MSVCRT.DLL work\vc6\native` if the disc is drive `E:`). Do not use the `msvcrt.dll` in `C:\Windows`, which is Windows' own, much newer runtime, and do not copy `MSVCRT.DLL` into `C:\Windows` or install it anywhere: `build_exe.py` makes copies of `LINK.EXE`, `MSPDB60.DLL` and `CVTRES.EXE` in `work\link\win\` that load it from `work\vc6\native\` under the name `MSVCR6.DLL`, and leaves the files in `work\vc6` unchanged. BUILDING.md §7 explains why the linker needs that runtime.

When you are done, `work\vc6` looks like this (the `vc98` and `dx8sdk` folders hold many more files):

```
work\vc6\bin\C1.DLL  C1XX.DLL  c2.dll  CL.EXE  CVTRES.EXE  LINK.EXE  MSPDB60.DLL
work\vc6\lib\LIBCMT.LIB
work\vc6\native\MSVCRT.DLL
work\vc6\vc98\INCLUDE\...
work\vc6\vc98\LIB\...        (KERNEL32.LIB, USER32.LIB, ... OLDNAMES.LIB)
work\vc6\dx8sdk\lib\...      (ddraw.lib, dinput8.lib, dsound.lib, dxguid.lib, ...)
```

**Check the files against [BUILDING.md](BUILDING.md) §4**, which lists the size and SHA-256 of every program and library the build reads. A different build of any of them is not expected to reproduce the original's bytes. This compares all of them with that table, one line each:

```
python -c "import re,hashlib,os;t=open('BUILDING.md',encoding='utf-8').read();[print(('OK       ' if hashlib.sha256(open(p,'rb').read()).hexdigest()==h else 'DIFFERS  ')+p if os.path.isfile(p) else 'MISSING  '+p) for f,h in re.findall(r'\| \x60([^\x60]+)\x60 \|[^\n]*\x60([0-9a-f]{64})\x60',t) for p in [f if f.startswith('work/') else 'work/vc6/'+f]]"
```

```
OK       work/vc6/bin/CL.EXE
OK       work/vc6/bin/C1.DLL
OK       work/vc6/bin/C1XX.DLL
...
OK       work/vc6/vc98/LIB/OLDNAMES.LIB
```

Every line must start with `OK`. `MISSING` means the file is not where the table above puts it; `DIFFERS` means it is another version of that file.

## 6. Build

Run these in order in the repository's folder. Each one says what it prints when all is well.

**The disassembly of the game.** Prints nothing and takes a few seconds; it writes `work\SheepD3D.asm` (about 18 MB).

```
llvm-objdump -d --x86-asm-syntax=intel --no-show-raw-insn "Sheep, Dog 'n' Wolf (PAL Version)\SheepD3D.exe" > work\SheepD3D.asm
```

**The index of the exe** built from it:

```
python tools\pe_index.py
```

```
instructions: 442465
IAT slots:    168
strings:      932
functions (direct-call targets): 2352, with string/API anchors: 220
```

(LLVM 21 and 23 give these counts.) An error, or **`instructions: 0`**, means the disassembly is unreadable or empty: see "Troubleshooting".

**The headers.** The first command makes one from the game disc's `Scenaric_Classes.h` and `GameRes.h`; the second writes the others and the tables the matcher reads, from `data\`. It takes a few seconds.

```
python tools\scenaric_to_c.py
python tools\structs_to_c.py
```

```
197 classes, 1189 properties, 1 classes with padding gaps -> src\include\scenaric_props.h
<n> structs (<n> fields), <n> enums -> src/include/   (verify: cc -fsyntax-only -std=c11 work/layout_check.c)
D:\SDW\src\include\sdw_classes.h: <n> classes/structs (<n> with vtables)
```

A `layout problem:` or `class problem:` line means the tables in `data\` disagree about a struct or class (two fields that overlap, fields past the object's size); the line names the struct and the field.

**The layout check** (optional): clang compiles a file that asserts the offset of every field and the size of every struct. It prints nothing when every assertion holds.

```
clang -fsyntax-only -std=c11 work\layout_check.c
```

**One source file**, compiled with the original compiler and compared with the game, function by function:

```
python tools\vc6.py src\objects\anvil.cpp
```

```
== src\objects\anvil.cpp   (/c /MT /Od /Gd /G6 /Ob1 /GX- /Zp8 /Op- /QIfdiv-)
  MATCH   0x494df0     162/162  Anvil_PostLoadInit
  MATCH   0x494e92     383/383  Anvil_Update
  MATCH   0x495011     221/221  Anvil_HandleMessage
  MATCH   0x4950ee     100/100  Anvil_Create
  MATCH   0x4d42c0       11/11  ScnObject_Nop  placed by its vtable slot
  5 functions: 5 MATCH
```

If this works, the compiler is set up correctly.

**The whole exe.** This compiles every source file (the verdict of every function scrolls past), links them in the original order with the original linker, and compares the result with your `SheepD3D.exe`:

```
python tools\build_exe.py
```

It took 35 seconds on an eight-core PC. Along the way, lines `Command line warning D4025 : overriding '/Od' with '/O2'` (files the original compiled with other switches), `NOADDR` (helpers the compiler generates, which have no address of their own) and `0 functions:` (files that define no function of their own) are expected. The last lines are:

```
== total over 328 files: 3729 MATCH, 78 NOADDR
wrote data\match_results.json
linked D:\SDW\work\link\SheepD3D.exe (MSVCRT 6.0.8168 as MSVCR6.DLL; 1 warnings)
SHA-1 ca39374d53ae0030c5bd8c90dda45465e446dfe3  == the original: BYTE-IDENTICAL
```

**`BYTE-IDENTICAL`** means that `work\link\SheepD3D.exe`, built from the source in `src\`, is the original program, byte for byte. The one warning is LINK's `LNK4108` about `/ALIGN`, one of the original's link options; it is expected.

## What next

- [README.md](README.md) says what is in the repository; `docs/00-conventions.md` and `src/README.md` are where to start reading the source.
- [BUILDING.md](BUILDING.md) is the reference: the per-object layout check and the Ghidra scripts.
- [TOOLS.md](TOOLS.md) lists every tool; [CONTRIBUTING.md](CONTRIBUTING.md) says how to change the source without changing a byte. `python tools\build_mod.py` builds an exe from source you have edited.

The other documents write commands the way a Unix shell does (`python3 tools/vc6.py`). In a Command Prompt type `python` for `python3`; forward slashes in the tools' arguments work as they are.

## Troubleshooting

- **`'python' is not recognized...`, `'llvm-objdump' is not recognized...` or `'clang' is not recognized...`**: the program is not on the PATH. Open a new Command Prompt after installing (an open one keeps the old PATH), and see the checks in steps 1 and 2.
- **`C:` is full or nearly full.** Installers unpack themselves on `C:` whatever folder you choose, so they can fail there; use LLVM's portable archive (step 2). Keep the repository, the game and the Microsoft files on another drive: the build itself writes nothing to `C:` apart from small temporary files.
- **`pe_index.py` stops with `UnicodeDecodeError: 'utf-8' codec can't decode byte 0xff in position 0`.** The disassembly was written by Windows PowerShell, whose `>` writes UTF-16 text (the file is about twice the size it should be). Run the `llvm-objdump` line again in a Command Prompt, then `pe_index.py` again.
- **`pe_index.py` prints `instructions: 0`.** `work\SheepD3D.asm` is empty or incomplete. Run the `llvm-objdump` line again and check that it prints nothing.
- **A Python traceback ending in `FileNotFoundError`** names the file a tool could not find: most often the game folder's name (step 4), or a file of the disc missing from it (`Levels\Lvl-03\Scenaric_Classes.h`, `Levels\Lvl-03\GameRes.h`).
- **Files copied from a Mac.** macOS can leave a hidden file whose name starts with `._` next to every file it copies (it keeps extended attributes in them). The tools take them for source files and tables and fail on them: `structs_to_c.py` stops with a `UnicodeDecodeError` or `ValueError`, and `vc6.py` reports `src\objects\._anvil.cpp: compile failed` with `error C2018: unknown character`. Delete them all; this lists each file as it deletes it:

  ```
  python -c "import pathlib;[(print(p),p.unlink()) for p in pathlib.Path('.').rglob('._*') if p.is_file()]"
  ```

  A file `._.` may remain in the top folder. It is harmless; `del "\\?\%CD%\._."` deletes it (a name ending in a dot needs that form).
- **`missing Microsoft files (see BUILDING.md)`** from `build_exe.py`: the lines after it name each file that is not where step 5 puts it.
- **`no compiler in D:\SDW\work\vc6\bin`**: `CL.EXE` is not there (step 5).
- **`compile failed` for every file**, or `== headers (sdw_classes.h): FAILED`: `CL.EXE` starts but cannot load one of its files. The lines under the first `compile failed` are the compiler's own message, which usually names the file. Run the check in step 5.
- **`compile step failed`** from `build_exe.py`: at least one file failed to compile or one function says `DIFF`, and the link was not run. Scroll up to the first `compile failed` or `DIFF` line.
- **Functions say `DIFF`, or the SHA-1 differs**, although you changed nothing: check the Microsoft files' hashes (step 5) and the game's SHA-1 (step 4). Only the exact versions reproduce the original.
- **Git lists changed files after a build.** The tools write their text files as UTF-8 with LF line endings on every platform (`tools/textio.py`), so a build should leave `src\include\` and `data\match_results.json` unchanged. If git still lists them and you have not changed anything in `data\`, restore them before you commit with `git checkout -- src/include data/match_results.json`.
