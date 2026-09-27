# The scenaric object system

Everything here is verified in the code unless marked *(inferred)*.

## Classes

Every gameplay object ("scenaric object") is an instance of one of the classes listed in the disc header `Levels/Lvl-03/Scenaric_Classes.h` (197 class ids for the whole game). At startup the engine calls

```
Scenaric_RegisterClass(classId, factoryFn, a, b, c)      // 0x50d630; all 182 calls are in one function, 0x513560
```

once per class. 181 of the 197 header ids are registered; the 16 that are not (8 robin, 9 Porky, 12 MiscStatic, 17 Tree_02, 34, 37, 44, 46, 49, 51, 63, 153, 154, 168, 180, 185) are *(inferred)* cut or editor-only classes. One registration call has non-constant arguments and is not parsed by `tools/class_map.py`.

Each factory is `p = Scenaric_Alloc(size)` followed by an **inlined constructor chain that stores one vtable per inheritance level, base first**. That makes the object size, the vtable and the full ancestry mechanically recoverable: `tools/class_map.py` writes them to `data/class_map.csv`.

The hierarchy (the base-class names are chosen from what derives from them):

```
ScnObject        0x574c40   root of all 181 classes
├─ ScnBody       0x574c1c   88 direct: static world props - Mailbox, bridge, seesaw, FallingRock...
│  └─ ScnMobile  0x574c84   52 direct: Sam, Sheep, Dynamite, Rocket, box, Salad, Fan...
│     ├─ ScnControllable 0x574fd0   Wolf (0x71c bytes), Robot - the player-controllable actors
│     ├─ Mine 0x5766b8               DefusableMine, GroundMine
│     └─ FishingRod 0x5760b8 → 0x576110 → MagnetRod, SaladRod
└─ ScnLogic      0x57595c   33 direct: Goal, CameraManager, CheckpointManager, CinematicsManager, SignPost...
   └─ ScnLogicShadowed 0x576934   Rock, Snowball
```

There is no RTTI; the class name for a vtable comes only from this registration table.

## Heap allocation layout

`Heap_Alloc` 0x54de2a is a best-fit free-list allocator. An allocated block is

```
+0  u32 blockSize | flags(low 2 bits)     // = requested size + 8, rounded per heap alignment
+4  u32 0x98765432                        // allocation magic (the slot holds the free-list link when the block is free)
+8  user data  →  C++ object, vtable pointer first
```

`data/class_map.csv` gives each class's object size and vtable.

## Common object layout

Positions are three signed 16-bit integers: x (+0x0C), vertical (+0x0E, pointing down) and z (+0x10). Angles are 12-bit: 4096 = one full turn. The full layouts are in `data/structs/` (`ScnObject.csv`, `ScnBody.csv`, `ScnMobile.csv`, `ScnControllable.csv` and one file per class). Selected fields of `ScnObject`, which every object has, and of `ScnControllable`, the base of Ralph (`Wolf`) and the `Robot`:

| Offset | Type | Field | Class |
|---|---|---|---|
| +0x00 | ptr | vtable | `ScnObject` |
| +0x0C / +0x0E / +0x10 | s16 ×3 | position: x, y, z | `ScnObject` |
| +0x16 | s16 | facing direction (`rot.y`) | `ScnObject` |
| +0xD0 | s32 | stick magnitude | `ScnControllable` |
| +0xDE | s16 | move direction, written by `Mobile_Steer` 0x492b4b at 0x492d77 | `ScnControllable` |
| +0xE6 | s16 | wanted direction, written at 0x492be1 | `ScnControllable` |
| +0xF0 | s16 | speed, written at 0x492d33 | `ScnControllable` |

`Mobile_Steer` 0x492b4b is the mover that writes the speed and both move directions.

## Instance properties

Each instance's designer properties live in a separate block described by `src/include/scenaric_props.h` (generated from the disc header: 132 structs, all 4-byte slots; `tools/ghidra/ImportScenaricTypes.java` loads them into Ghidra under `/SDW/ScenaricProps`). Object `+0x20` points at the serialised scenaric record, whose properties start at record `+0x14` (`tools/war_objects.py --props` prints them by name).

## How a level is won - `Goal_Update` 0x4c74a0

Every frame, until its done bit is set, the Goal (class 23) requires **all** of:

1. Ralph's position inside the goal's model box moved to the goal position - x, z **and** vertical (the standard box is about ±200 × ±200, vertical −25…+24);
2. Ralph's horizontal distance to the goal centre ≤ `winRadius` = `modelBox.max.x × 90 / 128` = **138 units** for the standard goal;
3. `Scenaric_FindNearestOfClass(goalPos, SHEEP, boxMinVert, boxMaxVert, winRadius)` finds a sheep - **a sheep within 138 units of the goal and inside the box's height range** - or goal flag bit 1 ("no sheep needed") is set, although the next test still reads the sheep it looked for, which is null when none was found (`src/objects/goal.cpp`);
4. if that sheep is attached to something, the carrier must not be Sam (class 1).

It then sends the Wolf message 0x401 (level won), or, if goal flag bit 0 is set, starts a cinematic instead. The test is on the sheep's position alone: it is met by a sheep Ralph carries and equally by one standing, thrown or pushed within 138 units of the goal.

## Save format: the PlayStation memory-card block, kept in the PC port

`Save_InitCardHeader` 0x54a6fb builds, in BSS at `g_saveCardHeader` 0x6e3698, a literal **PS1 memory-card block header**. Every field matches the PS1 layout:

| offset | size | contents | source in the exe |
|---|---|---|---|
| +0x00 | 2 | `"SC"` magic | 0x57e5c8 |
| +0x02 | 1 | icon display flag `0x11` - one icon frame | immediate |
| +0x03 | 1 | block count `0x01` | immediate |
| +0x04 | 0x22 | title, **Shift-JIS**, decoding to the full-width `ＳＨＥＥＰ　ＤＯＧ´ｎ　ＷＯＬＦ` | 0x577198 |
| +0x60 | 0x20 | 16-entry 15-bit BGR CLUT, entry 0 transparent | 0x57c838 |
| +0x80 | 0x80 | 16×16 icon at 4bpp - 128 bytes is exactly 16·16/2 | 0x57c858 |

Alongside it are **four card blocks**: `g_cardBlockState` 0x6e37a0 is `char[4]` holding `'E'` (0x45, empty) or `'F'` (0x46, full), and `g_cardBlocks` 0x6e37a4 holds the matching records at a 0x2c stride. `Save_InitCardHeader` marks all four `'E'`; `Card_CommitBlock` 0x54aaaa marks one `'F'` after a write and stamps `g_rawTime`; `Card_AnyBlockUsed` 0x54b04e scans for the first `'F'`. The status word at 0x6e379c is reset to 0x3f2, one of a family of codes (0x321, 0x322, 0x323, 0x388, 0x389, 0x3ea, 0x3f2) that look like the PS1 card status values; they are not individually decoded.

This is console code compiled into the PC build.

**Where it is stored: the Windows registry.** The card is backed by a registry binary value, not a file. `Card_OpenRegistryRoot` 0x52a2d6 opens the application's root with key `"Progress"` / value `"SdwSaves"`; `Card_ReadBlocks` 0x52a3a6 reads `"SdwSaves"` into a buffer rounded **up to a multiple of 0x80 - one PS1 frame** - returning status 0x323 on a full read and 0x321 otherwise; `Card_WriteBlocks` 0x52a42c writes it back with a length of `blocks << 13`, **8192 bytes per block, exactly one PS1 memory-card block**, returning 0x389 or 0x388. The port keeps the console's block and frame geometry and swaps only the storage.

The driver above it is the 5 KB state machine at 0x54b459, reached from the `MCardManager` class (vtable 0x576624); 0x54b176 is the card **UI** (it calls `Draw2D_TexRect` and `Text_SetWindow`), not the write path.

### What a save contains: a 0x2c-byte progress record

`Progress_CopyRecord` 0x50be91 is `memcpy(dest, this + 0x84, 0x2c)`. Everything that saves goes through it: `PauseMenu_Open` 0x542afa snapshots the record into `g_progressPauseCopy` 0x6dec58, and `Card_CommitBlock` copies it into `g_cardBlocks[blockIndex]`. So **the whole persistent state is 44 bytes** at `+0x84` of the progress object `g_pProgress` 0x57b80c (`data/structs/Progress.csv`).

`Progress_CopyRecord` is `__thiscall`: the progress object is the implicit receiver (the source, in ECX) and the one pushed argument is the destination, as in `Card_CommitBlock`'s call at 0x54aae6 (`src/engine/card.cpp`).

## Lifts, platforms and scene flow

- **Wooden Lift:** carries riders with plain `ScnObject_Translate` (no collision, no ceiling check); the stacked-object pass runs only while moving up and only for the first stacked item. It descends only if no bodyless, in-world object has its origin within 50 units under it, so a pickup under a lift blocks it indefinitely. **`WoodenLift_Reset` restores neither position nor direction: after a checkpoint restart the lift stays where it was.** Its camera box test is XZ only.
- **WoodenPlatForm** moves by editing its ACTIVATIONZONE box in place and translating **every object overlapping that zone**, not just what stands on it.
- **Scene flow:** success in level 0x10 (`Lvl-16`) goes Fend (-4) → `Lvl-17` (0x11, Planet X) → Ending (-5) → the credits FMV list 0x71b318 ({1, 'Credits.BVS'}) → title (-1); every transition to -1 calls `Progress_Reset`. `Progress_AwardTimeKeeper` adds a bonus point with no already-awarded check (the TimeKeeper's own state prevents doubles).

## Implementation limitations

- **Loading:** `Progress_BuildScenePath` uses the install path as a printf format, so a '%' in the path corrupts every level path. `Load_DAV`'s version test is inverted: it warns on a match and never rejects. `Res_GetValidatedIdList` checks only entry 0. `Load_SND` does not bound the sample count against its 256-entry arrays. `Load_FreeWAR` can walk a stale count after a failed load.
- **Sound:** the positional falloff truncates the deltas to s16 before squaring, so two points more than 32767 apart on an axis count as near.
- **Text:** the copies `TextResBank_LoadString` makes every frame are never freed (23 callers, the pause menu among them).
- **Drawing:** `Instance_DrawAnimParts` writes up to jointCount matrices into the 42-entry `g_animPartMatrices` with no clamp; a model with more than 42 joints would overwrite the camera state after it.
- **Cinematics:** `Cine_FindFirstPosKey` / `Cine_FindFirstRotKey` restore the iterator only on failure, and `Cine_NotifyActors` resumes from wherever they left it *(inferred: a possible hang)*. Cinematic opcodes 7 and 8 are inert (the attach table is never populated).
