# The executable and the game's files

Everything here is verified against the files unless marked *(inferred)*.

## The executable

| | |
|---|---|
| File | `SheepD3D.exe`, 2,461,696 bytes |
| SHA-1 | `ca39374d53ae0030c5bd8c90dda45465e446dfe3` |
| MD5 | `0ca4acee40e3468e80b4d97b390842e3` |
| Built | Thu 26 Jul 2001 (PE timestamp 0x3b601fc7), Microsoft Visual C++ 6.0 |
| Image base / entry | `0x400000` / `0x568753` (the standard MSVC CRT `WinMainCRTStartup`) |
| Protection | None: no packer, no copy-protection sections, a normal import table |
| Relocations / exports / debug directory | None |

Sections: `.text` 0x401000 (1.45 MB), `.rdata` 0x574000, `.data` 0x579000 (1.6 MB virtual, mostly BSS), `.rsrc` 0x71f000.

Imports: DDRAW (`DirectDrawCreateEx`, Direct3D 7 immediate mode), DINPUT8, DSOUND, WINMM (mmio only, for WAV files), DSETUP, and ordinary Win32. There is no `timeGetTime` or `GetTickCount`: the clock is RDTSC calibrated against `QueryPerformanceCounter` ([the frame loop](03-time-and-frame-loop.md)).

## Code layout

| Range | Contents | Code generation |
|---|---|---|
| `0x401000-0x565850` | The game and its engine ("Black Sheep"), with libjpeg 6 at `0x4222c0-0x42dae0` and DirectX SDK utility code interleaved | Mostly `/Od`: EBP-framed, every local on the stack, `this` spilled to `[ebp-x]` and reloaded. The sound and video layer, the cinematic and stream player and libjpeg were built `/O2 /Oy-` ([00-conventions.md](00-conventions.md) lists the files) |
| `0x565850-0x574000` | Microsoft's static C runtime (SP5 LIBCMT) | Optimised, frame-pointer omission |

- Unoptimised MSVC 6 output maps almost statement for statement back to C++, which is what makes a byte-matching source practical.
- The code is C++ (constructors store vtable pointers, `ecx` = `this`) built **without RTTI**, so class names come from strings, the disc headers and behaviour, not from the binary's type information.
- Functions reached only through pointers (state-machine tables, vtables) are not direct-call targets; `tools/find_seeds.py` lists them for Ghidra.

## Naming evidence on the disc and in the exe

1. **Developer headers left on the disc** in `Levels/Lvl-03/`:
   - `Scenaric_Classes.h` (2,382 lines): every scenaric object class with its `CLASSID_*`, per-property byte offsets (`PROPERTY_SAM_GREENZONEBOX 44`, ...) and `PROPSIZE_*`. It is the schema of the object instances in the level files and the source of the canonical class names (Wolf, Sam, Sheep, Dynamite, Rocket, Mailbox...). `tools/scenaric_to_c.py` turns it into `src/include/scenaric_props.h`.
   - `GameRes.h`: the resource ids exported from the DAV and WAR files (`WAR_IDO_*` objects, `DAV_IDI_*` images).
   - `Cin_Lvl-03.h`: cinematic ids. `Lvl-03.SoundIndex` / `.TextIndex`: sound and text ids; the index files also keep the original asset paths.
2. **Strings in the exe** (about 930): state names (`SAM_STT_FOLLOWNODES`, `EnterReachGoalState`, `EnterFollowTrajectoryState`, `EnterSearchNearestNodeState`), function names in diagnostics (`Flock_GetObjNearestAvailableSheep_Vision`, `Install_ScenaricResource`, `Load_DAVnWAR`, `GetResourceType`), the loader's progress messages, which give the initialisation order (collisions, shadows, camera, clusters, menus, map), camera and AI debug messages, and Hungarian-style variable names.
3. **PlayStation heritage**: button tokens (`$B_CROSS$`, `$B_L1$`), memory-card message tables with placeholder entries for the PC version, and the PlayStation memory-card save format ([the object system](04-object-system.md)). The PC build is a port of the PlayStation code base.

## Data formats

All uncompressed, little-endian and magic-tagged. [uhwot/sdw_re_stuff](https://github.com/uhwot/sdw_re_stuff) (MIT) documents the asset formats of this build with a Go exporter, ImHex patterns and a WAR CRC32 tool.

| Extension | Magic | Contents |
|---|---|---|
| `.WAR` | CRC32 + `V2.6` | A level's world: objects, scenaric instances and their property blocks, collision, trajectories and nodes. The resource table is at 0x10 ([00-conventions.md, "WAR level files"](00-conventions.md#war-level-files)) |
| `.DAV` | `VDX7` + `CHEK`×4 | Graphics: named sections (`_SRA`, `_SDA`, "----Section material----"), textures, models |
| `.SND` | hash + `vdx7` | A bundle of RIFF/WAVE blobs |
| `.MLT` | hash + `v1.2` | Localised strings (NUL-separated, per language) |
| `.BSC` | `BlackSheep Controller ConfigV2.0` | Key and pad mapping (`REMAP`, `REMAP_DIR`) |
| `.BSV` | `BSV_FILEV2.5` | The music and voice bank index |
| `.BSM` | `GREETINGV1.0` | Launcher text |
| `.BVS` | `RIFF…AVI ` | Plain AVI (Indeo 5) |
| `.sdw` | - | The bonus gallery: a JPEG archive with a name/offset/size table |

Saves are kept in the Windows registry, in the layout of a PlayStation memory card ([the object system](04-object-system.md)). `_LoadFile` also has a "no chksum" path for plain files.

Levels: `Lvl-00` ... `Lvl-17`, plus `Scene`, `Wheel`, `Intro`, `Fend`, `Ending` and `Demos`. [00-conventions.md](00-conventions.md#levels-two-numberings) maps the `Lvl-NN` folders to the level numbers players use.
