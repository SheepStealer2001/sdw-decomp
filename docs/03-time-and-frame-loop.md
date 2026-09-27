# Time and the frame loop

Everything here is verified in the code unless marked *(inferred)*.

## Frame order - `Main_Loop` 0x401614

Each iteration, after pumping Windows messages:

1. `Time_Update` 0x56108c - compute this frame's delta and advance every clock.
2. `Render_BeginFrame` 0x417a38 - clear and `BeginScene`.
3. `Game_Frame_2` 0x560542, only while `g_gameFlags` has bits 4 and 8: the game update and draw - the object update loop `Scenaric_UpdateAll` 0x55fcfa and the object render loop `Scenaric_RenderAll` 0x560182.
4. `Input_Poll` 0x55e913 - refresh `g_pad` and latch previous/current (`Pad_Latch` 0x55ef9f, called twice).
5. `Cheat_Poll` 0x52de00; if `Cheat_IsLevelSkipRequested` 0x52de40, `Fade_StartLevelExit(0x1000)` 0x53e700.
6. `Level_ExitUpdate` 0x510675 (while `DAT_0057b838` is set): the level-exit and progression state machine.
7. `Render_EndFrame` 0x417e59 (flush and `EndScene`), then `Render_Present` 0x417ecf (present, then the frame limiter).

`Input_Poll` (step 4) refreshes the controller state after the game update and latches it into `g_pad`; the latch writes are at 0x55f00b.

## The frame limiter

`Render_Present(g_maxFps)` is called unconditionally each iteration, so the limiter runs even on frames where the device was not ready and nothing was drawn. It ends in `Frame_LimitFps` 0x405daf, which **busy-waits - there is no `Sleep`** - on a second `Timer` object, `g_limiterTimer` 0x585068, distinct from the clock `Time_Update` reads (`g_pTimer` 0x71b2e4):

```c
do {  g_limiterElapsedSec = Timer_GetElapsed(&g_limiterTimer, 1);   // unit 1 = seconds
      fps = (u32)__ftol(1.0 / g_limiterElapsedSec);
      if (fps <= maxFps) break;
} while (g_limiterElapsedSec != 0.0);
Timer_Stop(&g_limiterTimer); Timer_Start(&g_limiterTimer);
```

The exit condition is `trunc(1/elapsed) <= maxFps`. `g_maxFps` is the u32 **60 at 0x579110** (its only reader is `Main_Loop`), so the loop exits once `elapsed > 1/61` s: **the cap is about 61 FPS, not a 60.0 Hz period.**

- Fullscreen presents with `Flip(NULL, DDFLIP_WAIT)`, so it is vsync-bound as well as capped. Windowed mode uses `Blt(..., DDBLT_WAIT)`: no vsync, the cap only.
- The stock game therefore never exceeds about 61 FPS (`g_dtRaw` = 67 at the cap, 68 at 60 Hz vsync).
- The two `Sleep` references in the exe are DirectSound buffer-restore retries (`Sleep(10)` while `DSERR_BUFFERLOST`), unrelated to pacing.
- `g_maxImmediateTris` 0x6d6678 = **1000**, written by the first instruction of `App_InitGameSystems` 0x401871 and read once, in `Load_DAVnWAR`, which hands it to the `PolyBatcher` constructor. The address is in BSS, so that write is the only place the value appears.

## The time model - `Time_Update`

The unit of time everywhere is **1/4096 second**, as an integer.

```
g_dtRaw = trunc(Timer_GetDelta(seconds) * 4096.0)          // 0x71b2cc; the fraction is thrown away every frame
g_dt    = (g_gameFlags & 0x40) ? 0 : (g_dtRaw * g_timeScale) >> 12
if (g_dt > 0xAA || g_dt < 0) g_dt = 0xAA                   // 0x71b300; clamp = 170/4096 s = 41.5 ms = 24.1 FPS
g_animDt      = g_dt * 1000 >> 2                           // 0x6ddf5c
g_dtRawMs     = g_dtRaw * 1000 >> 12                       // truncated a second time
g_dtMs        = g_dt    * 1000 >> 12
g_gameTime   += g_dt;    g_gameTimeMs += g_dtMs
g_rawTime    += g_dtRaw; g_rawTimeMs  += g_dtRawMs         // 0x71b304: unscaled and never paused
g_frameCount += 1                                          // 0x71b2c8
```

- The clock source is RDTSC (`Timer_ReadTSC` 0x40b4e4), converted with `g_cpuHz`, which `Timer_Construct` 0x40b060 measures once against `QueryPerformanceCounter` over 100 samples. Anything that changes the TSC rate after that calibration (frequency scaling on old CPUs, virtual machines, core migration on hardware without an invariant TSC) changes the game's speed.
- `Time_Update` keeps an alternative that sets `g_dtRaw = 0x88` (136, about 4096/30: a fixed 30 FPS step) instead of the measured delta. The branch is still in the exe, but a local flag that is always 1 selects the measured delta, so it is never taken (`src/engine/time.cpp`).
- `g_timeScale` (4.12 fixed point, 0x1000 = 1.0) is written only by `Time_Init` 0x560f80.

## Cheat codes, polled from `Main_Loop`

`Cheat_Poll` 0x52de00 is called from `Main_Loop` every frame - including while paused and in menus - and runs **two** pad sequences through one matcher, `Cheat_CheckSequence` 0x52dd70.

| | progress counter | code table | length | result |
|---|---|---|---|---|
| A - "mystery" | `0x6d8300` | `0x57bc50` | 6 | `g_cheatMysteryFlag` 0x6d8304 `= 1` |
| B - level skip | `0x6d8308` | `0x57bc5c` | 7 | `g_cheatLevelSkip` 0x6d830c |

The tables are **one's-complement pad words** (active-low, the PlayStation convention the port kept, as in the save format), so `~entry` gives the bit that must be pressed. Decoded with `data/enums/PadButton.csv`:

- **A (mystery):** Square, Square, Triangle, Triangle, Circle, Circle
- **B (level skip):** Square, Square, Triangle, Square, Triangle, Circle, Circle

The first three presses are identical, so entering either code walks the other's counter to 3 before they diverge on the fourth press. Both share the single timeout at `g_cheatSeqTimer` 0x6d82fc, so progress on one resets the window for both.

**The window between presses is half its nominal value.** The matcher accumulates `g_dtMs` until 0x5dd (1501 ms), but `Cheat_Poll` calls it twice per frame - once per sequence - so the real window is about 750 ms. It is independent of the frame rate (it is driven by `g_dtMs`).

**The "mystery" cheat is dead code.** `0x6d8304` has exactly one reference in the whole image: the write `mov dword ptr [0x6d8304], 0x1` at 0x52de18. There is no read anywhere, so whatever consumed the flag was removed and only the detector remains. It uses the same three buttons in a simpler pattern and the same matcher in the same function, which suggests *(inferred)* an early form of the level-skip input.

The level-skip result, by contrast, is read by `Cheat_IsLevelSkipRequested` 0x52de40, whose only caller is `Main_Loop`.

### A cut timed mode in the same module

`Countdown_Format` 0x52de50 renders `g_countdownMs` 0x6d8310 either as a game-over label or as `(ms/1000) % 60`, and `Countdown_DrawHud` 0x52dea3 draws it in a text box at (0x50, 0x28), tinting it toward red once the value drops below 30000 ms. **`Countdown_DrawHud` has no callers**: both functions and the global are unreachable, a timed mode that did not ship.

## Other engine details in the frame path

- **Save blocks are CRC-32'd.** `Crc32` 0x55f6b0 is the standard table-driven algorithm over the table at 0x5771d0 (the reflected polynomial 0xEDB88320). It returns the running value without the conventional final XOR with 0xFFFFFFFF. Its callers are `Card_Access` and `Card_CommitBlock`, the counterpart of the loader's diagnostic for a load without a checksum (its string is at 0x57bc84).
- **A deliberate hang in the input code.** `Input_SetMode` 0x55e714, called with mode 1 while the state byte at 0x719620 is 2, runs `mov edx,1 / test edx,edx / je / jmp` at 0x55e73b-0x55e744: the exit branch can never be taken, so it is `while (TRUE);`, most likely a debug assert. Whether the condition is reachable in normal play is not established.
- **`Str_IsPrefixOf` 0x5614b0 is used as an equality test by six call sites** in the text and menu code, but it returns true whenever its first argument is a *prefix* of the second, so an empty first argument matches anything.
- The engine's own `printf` renders decimals through `Int_ToBcd` 0x5612c4, a shift-and-add-6-per-decade binary-to-BCD conversion.
- **Stream stalls are hidden from every clock.** When a music or voice clip is not ready, `StreamPlayer_UpdateVoice` 0x563bd0 busy-waits on the loader thread, but it first calls `Time_Pause` (saving the unconsumed timer delta) and afterwards `Time_Resume` pushes the timer base back by that delta. A disc stall therefore never advances `g_gameTime`.
- **Memory-card autosave cost:** `MCardManager` freezes Ralph and runs `Card_StateMachine` in mode 3: a 16-frame fade in, real-time waits of 2000 ms (detect), 1000 ms (open), 1500 ms (before write) and 2000 ms (the completion message), then a 16-frame fade out - about **6.5 s of wall clock plus about 32 frames** per autosave. The PC card is always present (`CardStub_Status` = 0x67), so the no-card, format and damaged-card screens are unreachable.
