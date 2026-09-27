# Fixes

Bug fixes for the game, as patches against the decompiled source. `src/` itself stays the exact original: it compiles to the byte-identical `SheepD3D.exe`, and every fix here is a change on top of it.

```bash
python3 tools/build_fixed.py --list              # the fixes
python3 tools/build_fixed.py                     # build with all of them: work/link/SheepD3D_fixed.exe
python3 tools/build_fixed.py --only NAME ...     # build with some of them
```

The build applies the patches to a copy of the source in `work/fixed/` and compiles and links that copy (the setup is the same as for the normal build: GETTING_STARTED.md). To play the result, put it next to the game's own `SheepD3D.exe` under its own name and run it from there.

A fixed exe is laid out differently from the original: every function after the first changed one moves, so an address in the original exe does not point at the same thing in a fixed one.

## The fixes

| Fix | What it changes |
|---|---|
| `iris-mask-leak` | The memory growth with every level load: the iris transition makes a new 256 x 256 mask texture on each load and never deletes the previous one (128 KB per load from the third on). The fix keeps and redraws the one it has. |
| `level-load-leaks` | Smaller leaks on every level load: the vertex buffers of every mesh in the level (never released), the Road Runner's shared buffers (re-created each time), the rolling carpet's belt backup texture and the gameplay Timer (neither deleted before being made again). |
| `paused-menu-overflow` | The crash after many level loads and restarts: the one-item "PAUSED" menu gains two nodes on every load and is never emptied, so from the 26th load on each load writes past its 50-node array over the pause menu's data, and the 93rd load reads a texture through an overwritten pointer. The fix empties it when a level is freed, as the pause menu already is. |
| `windowed-mode` | A "Windowed" checkbox in the launcher's configuration: the chosen resolution in a window with a title bar, centred on the main screen, instead of fullscreen. The game's display code already had a windowed path (which WinMain's filter hid); the fix takes it for the chosen mode, sizes the window, and keeps the picture in place when the window moves. The choice is saved with the other launcher settings. |
| `display-mode-overflow` | The game not starting with large monitors attached (the launcher never appears): before the launcher it lists every display mode each screen offers into a 100-entry table on the stack with no limit, and a screen offering more (a scaled 6K monitor offers 114 modes at its three depths) overwrites the stack. The fix keeps only the 16, 24 and 32-bit modes the game can use, and never more than the table holds. |
| `raft-current-high-fps` | Level B1 (disc `Lvl-05`): the river current picks up the raft at any frame rate as it does at 30 FPS. Above about 50 FPS the original current never builds up, and the raft waits for lag frames to start moving. |

## Mods on the fixed exe

A fix moves functions: everything after one that changed size is at another address, so the mod loader, which finds functions by their addresses in the original, cannot use the fixed exe as it is. `build_fixed.py` therefore also writes `SheepD3D_fixed.sdwsym` beside the exe: its SHA-1, then each original address and where the same function or variable is in this build (from the two builds' linker maps). Copy it next to the fixed exe with the same name; the loader reads it when the exe's hash matches and runs every mod as on the original.

## Writing a fix

A fix is one `.patch` file made with `git diff` from an edited `src/`. Put a plain description before the diff: what goes wrong in the game, why, and what the fix changes (`git apply` skips everything before the first `diff --git` line). In the code, mark each change with a comment that starts with `FIX <name>:`. Restore `src/` afterwards (`git checkout -- src`), so that the normal build stays byte-identical, and check with `python3 tools/build_fixed.py --only <name>` that only the functions the fix means to change are reported as differing.

A change to `src/` (even to a comment near a fix) can stop a patch from applying: `build_fixed.py` then names it and stops. Make the fix again on the current `src/` the same way and replace the file's `diff` part, keeping its description. Run `build_fixed.py` after changing `src/`.
