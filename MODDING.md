# Modding

Mods for Sheep, Dog 'n' Wolf are DLLs, level patches, models and files. A mod loader, a stand-in `dinput8.dll`, loads them into the game at start-up: a mod's code can replace or wrap any of the game's functions by the name the decompiled source gives it, and a mod without code can move, add and change a level's objects, bring in models, and replace files. It works with your own copy of the PAL `SheepD3D.exe`: nothing of the game is rebuilt or changed on disk, and removing the loader removes every mod.

## Installing

The quickest way: download the ready-made loader and the example mods (`sdw-mod-loader-<version>.zip`) from the repository's **Releases** page, unzip it into the game's folder next to `SheepD3D.exe`, and start the game (steps 3 and 4 below, and the notes on CrossOver and Windows, apply to it too). Nothing needs building for that.

To build the loader and the mods yourself, run the commands in this guide in the repository's folder (on Windows, type `python` where they say `python3`).

1. Put your copy of the game at the repository's root as `Sheep, Dog 'n' Wolf (PAL Version)/` (BUILDING.md §1). Then make the disassembly baseline (BUILDING.md §2: the disassembly and `python3 tools/pe_index.py`, which writes `work/iat.json`) and the object classes' header:

   ```bash
   python3 tools/scenaric_to_c.py
   ```

   The level tools read that folder's `Levels`. The header gives the loader the designer properties' names (`prop` lines) and the sounds and pictures of objects brought from other levels, and gives the level editor the classes' and properties' names; `SpeedHoney` includes it. `work/iat.json` gives the loader the game's `mmioOpenA` import, through which the game opens music and voices: without it, a mod's replacements of those files are not used. Build the mods after this step.
2. Build the loader and the example mods (MinGW-w64 for 32-bit Windows is needed, see "Building" below):

   ```bash
   python3 tools/build_mods.py
   ```

3. Copy `work/mods/dinput8.dll` and the `work/mods/Mods` folder next to the `SheepD3D.exe` you play. Keep only the mods you want in `Mods` (the Mods menu can also switch one off): each is a folder `Mods\<Name>\`, holding `<Name>.dll` when it has code.
4. Start the game as usual. `Mods\mods.log` says which exe was found and which mods loaded, and the launcher's first screen has a **Mods** button that lists them.

The game imports one function from `dinput8.dll`; the loader passes it on to Windows' own copy, so the game's input works as before. Without a `Mods` folder the loader does nothing at all.

**CrossOver or Wine:** in CrossOver the game's folder is inside the bottle (`~/Library/Application Support/CrossOver/Bottles/<bottle>/drive_c/Program Files/...`; the bottle's Open C: Drive shows it). Wine uses its own built-in `dinput8.dll` unless told to prefer the one next to the game. In CrossOver, open the bottle's Wine configuration, add `dinput8` on the Libraries tab and set it to "Native then Builtin"; with plain Wine, `WINEDLLOVERRIDES="dinput8=n,b" wine SheepD3D.exe`.

**Windows:** if the game is under `Program Files`, Windows keeps the loader's log in its VirtualStore folder, and only administrators may save there: start the level editor by right-clicking `SDW Level Editor.bat` and choosing Run as administrator (its start screen says so when it cannot save), or install the game elsewhere. For a game outside `Program Files`, start the level editor with `--mods` ("The level editor" below).

### When something does not work

- **No `Mods\mods.log` after the game starts:** the loader did not run. `dinput8.dll` and the `Mods` folder must sit beside the `SheepD3D.exe` you start; under CrossOver or Wine, `dinput8` must be set to Native then Builtin.
- **Reading the log:** it is written again each time the game starts. It has `[<Mod>] loaded` for each mod, `<n> mod(s) loaded`, and, when a level loads, `[<Mod>] Lvl-03: <n> change(s) applied`, with a `line <n>: ...` for each line of a patch that could not be applied and why (the other lines still apply).
- **`... is not the PAL SheepD3D.exe this loader knows`:** mods load only into the PAL release (`ca39374d53ae0030c5bd8c90dda45465e446dfe3`), whose functions are at the addresses in `data/symbols*.csv`, or into a widescreen-patched build of it (`bc2b4a7ffa970b60d4601997979aa7d0b62d44c4`, the same addresses). With any other exe the loader only passes the input on. An exe built from the source with changes (the fixed build, `fixes/README.md`) has its functions elsewhere: it comes with a symbol file named like the exe with `.sdwsym` in place of `.exe` (`SheepD3D_fixed.sdwsym`), which the loader uses when it sits beside the exe and matches it.
- **`SdwModInit returned <n>: unloaded`** or **`hook <name>: no such name in the symbol tables`:** see "Writing a mod".
- **The Mods menu:** a mod that did not load says so there, and points to `Mods\mods.log`.

## A first mod

With the loader installed:

1. Double-click `SDW Level Editor.app` (Mac) or `SDW Level Editor.bat` (Windows) in the repository's folder. On its start screen pick Level 3, choose "A new mod..." as the mod, name it `MyFirstMod` and click Open.
2. Pick an object, drag it somewhere else and save (Ctrl+S): the editor writes `Mods\MyFirstMod\levels\Lvl-03.txt` in the game's folder.
3. Start the game and play Level 3. The object is where you put it, and `Mods\mods.log` has a line like `[MyFirstMod] Lvl-03: 1 change(s) applied`.

## The example mods

| Mod | What it does | How |
|---|---|---|
| `InfiniteRocket` | Rockets never run out of fuel and never break | Wraps `Rocket_HandleMessage`: after every fuel burn it fills the tank again, and it answers the damage message (`MSG_ROCKET_DAMAGE`, sent on every impact in flight) itself, so the rocket stays intact |
| `SpeedHoney` | A new item in Level 3: a honey pot that makes Ralph run half again as fast for ten seconds | A model, a class and a level patch: the model imported from Level 10, class 198 (the pot turns, and on each frame the mod checks whether Ralph has reached it: then it leaves the world, a sound of the mod's own plays and his movement profiles' top speeds are raised), and its place in `levels\Lvl-03.txt` |
| `Crate` | A crate of the mod's own in Level 3 | No code: `models\crate.obj` (its texture `crate.png`, which `crate_png.py` draws at build time, its colours in `crate.mtl`, and `crate.rig`: one part, held still) is made into the game's model files when the mod is built, and `levels\Lvl-03.txt` imports it with `mod:` and places it as a Case, the game's own crate class (solid; shadows and items put down land on its top) |
| `Windmill` | A windmill of the mod's own in Level 3, its sails turning | No code: `models\windmill.obj` has two parts (groups `post` and `sails`), `windmill.rig` their pivots and the turning animation; built and placed like `Crate` |

Their source is in `mods/examples/`.

## A mod's folder

`Mods\<Name>\` holds any of:

- `<Name>.dll`: code ("Writing a mod").
- `mod.txt`: its name, version, description and load order ("Managing mods").
- `settings.txt`: settings its code reads ("Managing mods").
- `levels\<Level>.txt`: changes to a level's objects ("Level patches").
- `models\...`: models of the mod's own, as `tools/make_model.py` makes them ("Models of a mod's own").
- `files\...`: files that replace the game's, at the same path under the game folder: `files\Levels\Lvl-03\Lvl-03.DAV` is used instead of `Levels\Lvl-03\Lvl-03.DAV`. When two mods have the same file, the one loaded later wins (in load order, "Managing mods"). Level data, models and textures (`.DAV`), sounds, text, music and voices are all read through the functions this covers; the videos are not.
- `files\...\<file>.patch`: changes a few bytes of a game file without shipping it ("File patches").
- `sounds\...`: sounds of the mod's own, added to the levels' sound banks ("Writing a mod", sounds).

A mod without code (only `levels`, `models`, `sounds` or `files`) is fine.

## File patches

`files\Levels\Lvl-03\Lvl-03.DAV.patch` changes bytes of `Levels\Lvl-03\Lvl-03.DAV`. Each line is a file offset (write hex with `0x`: a plain number is decimal) and the bytes to write there, two hex digits each; `#` starts a comment. The log names each line it cannot apply:

```
0x1f24 1b 00       # two bytes at 0x1f24
0x2000 deadbeef    # or the bytes run together
```

The patches of every mod apply in load order, on top of a mod's replacement of that file if there is one. The loader writes the result to `Mods\cache\` once each time the game starts and the game reads that copy. The `.WAR` files carry a CRC-32 of their contents in their first 4 bytes, which the game checks; a patched copy gets its CRC recomputed. A patch holds only the changed bytes, so it contains no part of the game and can be shared, and several mods can patch the same file.

## Level patches

`levels\Lvl-03.txt` (or `levels\Scene.txt` for the studio, and so on: the level's file name) changes that level's objects when it loads, in memory: the game's files stay as they are. One change per line, `#` starts a comment, and `<res>` is an object's resource index as `python3 tools/war_objects.py Lvl-03` lists it. `<Level>` names a level by its folder on the disc, not by its number as players count. The two differ after Level 4: Level 0-4 are `Lvl-00`-`Lvl-04`, B1 is `Lvl-05`, Level 5-8 are `Lvl-06`-`Lvl-09`, B2 is `Lvl-10`, Level 9-14 are `Lvl-11`-`Lvl-16` and Planet X is `Lvl-17`. The level editor's start screen shows both.

```
114 pos 379 14 -1765          # place object 114 at x, y (pointing down), z
60 move 0 -100 0              # move object 60 by that much (here 100 up)
114 rot 0 1024 0              # set its rotation about x, y, z (4096 = one turn)
114 model 25                  # show another model resource of the level
52 prop CAMERA1 600           # a designer property, by its name in Scenaric_Classes.h (or its byte offset)
53 remove                     # the object is not created
new spinner from 114          # a new object: a copy of object 114...
spinner class 197             # ...which later lines change by its name (197: a mod's class, "New kinds of objects")
spinner pos -2003 -2 -5973
new fan from Lvl-02 82        # an object of another level (Level 2's fan), with all it needs (below)
sound 107 from Lvl-02         # a sound of another level's bank added to this level's
text 198 "Honey:\nFast."      # a class's name and help on the map screen
text 78 5 "Hello!"            # string 5 of class 78's text (a sign's), changed or added
import sign from Lvl-11 30    # a model of another level (below)
spinner model sign
```

Numbers are decimal or `0x` hex; names given by `new` and `import` hold within their file. A property's byte offset counts from the first property and is a multiple of 4; `python3 tools/war_objects.py Lvl-03 --props` shows each object's properties by name.

Every applied patch is listed in `Mods\mods.log`, with each line that could not be applied and why. Removing an object that others refer to (a trajectory, a switch's target) can crash the game. Several mods can patch the same level; they apply in load order.

### The level editor

Double-click `SDW Level Editor.app` (Mac) or `SDW Level Editor.bat` (Windows, with Python 3 installed) in the repository's folder. The editor opens in your browser (a page served on this computer only; nothing is downloaded) with a start screen: pick a level, by its folder or its number as players count, and a mod, one of your `Mods` folder's or a new name.

It saves into the installed game's `Mods` folder. It looks for one in `Program Files` and in CrossOver and Wine bottles (a game folder with a `Mods` folder first), and saves into `mods/examples` when it finds none; the start screen shows which ("Mods folder: ..."), and warns when it may not write there. It takes the objects' names from the installed game's `Levels/Lvl-03/Scenaric_Classes.h` when `tools/scenaric_to_c.py` has not been run. A saved change is in the game the next time that level loads. "Change level" goes back to the start screen. Started by double-click, it stops about 40 seconds after its page is closed.

On a Mac the launcher runs `/usr/bin/python3`: if macOS offers to install its command-line tools, install them and open the editor again. A copy of the repository downloaded as a zip may be stopped the first time it opens; Open Anyway in System Settings, Privacy & Security, lets it run.

It shows the level with its textures, every object with its model (animated ones in their default pose; the few the game turns to face the camera, like the cannonballs, drawn that way), the sky on a switch, and, on another, the collision surfaces (floors green, slopes yellow, walls grey). You can:
- turn the view and pan it by dragging (left turns, middle and right pan; Settings in the panel chooses each button, and the editor keeps the choice), move forward and back (wheel, a steady step you set), look at a point (double-click), move the view (W A S D), centre it on the picked object (F);
- pick an object in the view or the list (clicking the same place again picks the one behind it) and drag it along the ground, or with Shift up and down; turn it with Q and E (with Shift, in finer steps), nudge it with the arrows (with Shift, further), drop it to the ground with G;
- change its position, rotation, model, class and designer properties in the panel;
- copy it (Ctrl+D), remove it (Delete or Backspace; again brings it back), and bring an object from another level;
- undo (Ctrl+Z) and redo (Ctrl+Shift+Z or Ctrl+Y), and save (Ctrl+S).

A patch the mod already has is loaded, so the editor carries on from it: lines it does not change (`import`, `sound`, a class's `text <class> "..."`, comments) are kept as written, signs' `text <class> <n>` lines are in the signs' panels, and an `import` line's name becomes a model to choose.

From a terminal: `python3 tools/level_editor.py` (the start screen), `python3 tools/level_editor.py "Level 3" --mod MyMod` (straight to that level of that mod; a level is named by its folder, `Lvl-03`, or its number as players count, `"Level 3"`, `B1`, `"Planet X"`), and `--mods DIR` for the mods of another folder, such as `mods/examples`, or `"<game folder>/Mods"` for a game the editor does not find. Started from a terminal, it runs until Ctrl+C.

### Objects from other levels

`new <name> from <Level> <res>` adds object `<res>` of another level (`python3 tools/war_objects.py Lvl-02` lists them), and everything it needs that this level lacks comes with it:

- **its model**, with its textures and animations (as `import` brings one);
- **the characters' animations**: Ralph, Sam, the sheep and the others the two levels share (the same skeleton) get the animations the other level has and this one lacks: Ralph's fan animations, for the fan;
- **its sounds**: those its class's code plays (a table generated from the source: `python3 tools/build_mods.py --class-sounds` lists it) and those the animations it brings name, taken from the other level's sound bank when this level's lacks them;
- **its pictures**: its inventory icon and the pictures its class's code draws (the rocket's fuel gauge and icons), from the other level's `.DAV` with the texture pages they are on;
- **its name and help text** (the map screen), from the other level's `.MLT`, in the language the game is in.

All of it is read from your own copy of the game when the level loads. Other resources an object's properties name (a trajectory, a switch's target) are not brought across: change them with later lines. `sound <id> from <Level>` adds any other sound of another level's bank.

### Text for a class

`text <class> "<string>" ...` gives a class the name and help the map screen shows, for a class of the mod's own or to replace the game's: `text 198 "Honey:\nMakes Ralph run faster for ten seconds."` (`\n` starts a new line; the game's own entries are one string: the name, a colon, then a line per use). Write a class's name and help in plain ASCII: letters with accents are converted only in a sign's `text <class> <n>` lines.

### Signs' text

A sign shows the string of its class's text list whose number is in its `INDEXTEXT` property (`TEXTNUM` on the regular signs and the credits; `INDEXTEXT` on simple, animated and tip signs). `text <class> <n> "<string>"` changes string `<n>`, or adds it after the list's last, in whichever language the game runs in; a new sign points at its own with `prop INDEXTEXT <n>`. Letters with accents are written as they are. The level editor does this for you: a sign's panel has its text (shown in English), a copied sign gets a string of its own, and a sign brought from another level brings its text. `$B_JUMP$` and the like name the player's buttons, as the game's own signs do.

### Models from other levels

`import <name> from <Level> <res>` adds model `<res>` of another level to this one, with the texture pages it uses, and `<object> model <name>` shows it. `python3 tools/war_meshes.py Lvl-11` lists a level's models with their resource type and texture pages. The models objects use, type `0x43` (static) and `0x44` (animated, with its joints and animations), can be imported. The other level's `.WAR` stays in memory while this level is loaded (the model's data is read there), about 1 to 2 MB per level imported from; a texture page is 256 x 256 and each one imported adds 128 KB of video memory. If an import fails, the log says why and the objects that would show it are not created. `<Level>` can also be `mod:<file>` for a model file in the mod's own folder: `import crate from mod:models\crate 0` reads `models\crate.WAR` and `models\crate.DAV`.

An imported animated model's animations may name sounds its level's bank has and this one's lacks; `import` does not bring them (`new ... from <Level>` does): add them with `sound <id> from <Level>`.

### Models of a mod's own

`python3 tools/make_model.py crate.obj models/crate --scale 100 --box` makes those two files from a Wavefront `.obj` (the output path without its extension: put them in the mod's `models` folder, and name them with letters, digits, `-` and `_`): one static model of at most 256 vertices, `--scale` game units per `.obj` unit, `--box` for a solid collision box around it. Each face takes its material's `Kd` colour from the `.mtl`; a material with a texture (`map_Kd`, an 8-bit RGB or RGBA `.png` of at most 256 x 256; its alpha is ignored, as the pages are opaque) draws it on the faces that have texture coordinates. A model's textures are packed onto 256 x 256 pages, each one adding 128 KB of video memory while the level is loaded. The build does this for every `models\*.obj` of an example (options in a `.args` file beside it; a `<name>_png.py` beside it, if there is one, writes `<name>.png` first). The Crate example adds `--rig crate.rig`: one part held still, as the game's own crates are made, for the Case class it is placed as.

With `--rig <file>.rig` (a path from the `.obj`'s folder) the model is animated: its parts are the `.obj`'s groups (`o` or `g`), every face in a group the `.rig` names, and the `.rig` gives each part's joint and the animations. `--scale` means the level's units either way (the game stores an animated model 8 times larger than a static one, and `make_model.py` writes it so). The Windmill's `.rig`:

```
joint post - 0 0 0          # a part: its group, its parent (- for the root), its pivot in .obj units
joint sails post 0 1.5 -0.15
anim 0 turn                 # an animation: the game's id for it (static scenery plays 0) and a name
key 500                     # a key lasting 500 ms...
sails rot 0 0 1024          # ...with this joint turned a quarter about z (rot in 1/4096 turn, pos in game units,
                            # scale 1024 = 1, all in the game's space: y points down); unlisted joints are at rest
```

Keys loop, and turn the short way between two angles. The game draws each triangle from one side only: wind every face counter-clockwise as seen from outside (the usual `.obj` convention, and what modelling programs export). `make_model.py` warns when a closed model's faces point inwards; `--flip` turns every triangle around.

## Managing mods

- **`mod.txt`** in a mod's folder (optional) describes it: `name`, `version`, `author`, `description`, `requires` (the folder names of mods it needs, separated by commas or spaces) and `after` (mods it must load after), one `key: value` per line:

  ```
  name: Speed Honey
  version: 1.0
  description: A honey pot that makes Ralph run faster for ten seconds.
  requires: AnotherMod
  ```

- **Load order:** the folders' names, except that a mod loads after the mods it `requires` or names in `after`. A mod whose requirement is not installed or is switched off is not loaded (the log says why).
- **The Mods menu:** the launcher's first screen has a **Mods** button: a list of the installed mods with their names, versions and descriptions. Untick one to switch it off (the menu writes `Mods\disabled.txt`); a change applies the next time the game starts.
- **Settings:** `settings.txt` in a mod's folder, `key = value` per line (`#` starts a comment), which the mod reads with `api->setting(api, "key", "default")` or `api->setting_int(...)`.
- **Conflicts** are written to `Mods\mods.log`: two mods hooking the same function (both run: the later one first, calling through to the earlier), two replacing the same file (the later one's is used), two claiming the same class.

## Writing a mod

A mod is a 32-bit DLL that exports `SdwModInit`. The API is `mods/kit/sdw_mod.h`. A mod is written in C++ (a `.cpp` file), as the game's classes in `sdw_classes.h` are:

```cpp
#include "sdw_mod.h"
#include "sdw_enums.h"
#include "sdw_classes.h"

typedef s32(SDW_THIS_CC *HandleMessageFn)(Rocket *self, SDW_THIS_EDX, ScnObject *sender, u32 msgId, void *arg);
static HandleMessageFn s_original;

static s32 SDW_THIS_CC HandleMessage(Rocket *self, SDW_THIS_EDX, ScnObject *sender, u32 msgId, void *arg)
{
    s32 result = s_original(self, edx_unused, sender, msgId, arg); /* the game's own function */
    if (msgId == MSG_ROCKET_BURN_FUEL)
        self->fuel = self->fuelMax;
    return result;
}

SDW_MOD_EXPORT int SdwModInit(const SdwModApi *api)
{
    return api->hook("Rocket_HandleMessage", (void *)HandleMessage, (void **)&s_original);
}
```

- **Finding a function:** every function has a name in `data/symbols*.csv` and a definition in `src/`; `data/match_results.json` gives each name's file. A member function is named `Class_Method` in the tables and defined as `Class::Method` in `src/`: `Rocket_HandleMessage` is `Rocket::HandleMessage` in `src/objects/rocket.cpp`. `api->hook(name, replacement, &original)` makes the game call `replacement` instead; `original` runs the game's function (or the previous mod's replacement when two mods wrap the same one: the later mod runs first). `api->find(name)` gives the address of a function or global, to call it or read it.
- **Names stay:** when the decompilation gives a function a better name, the name it had keeps working: every name a loader has had is listed in `data/mod_symbols_published.csv`, and the loader keeps each of them as an alias of its function. The mod API only grows (new functions at its end), so a mod keeps loading with later loaders, on the original exe and on a build from the source with its symbol file.
- **Calling conventions:** a replacement must take the same parameters in the same convention. Free functions are `__cdecl` unless their declaration in `src/` says otherwise. Member functions pass the object in ECX (`__thiscall`); write them as `SDW_THIS_CC` with an unused `SDW_THIS_EDX` second parameter, as above, which works with any compiler.
- **Game data:** `src/include/sdw_classes.h` declares every class with its fields at their exact offsets (its layout checks hold under MinGW too), so a mod reads and writes game objects directly. Its member functions are declarations only: the game's code is not linked into a mod, so call them through `api->find`, and do not call virtual functions through the class (a GCC vtable is laid out differently): read the pointer from the object's vtable instead.
- **Events:** `api->on_frame`, `on_level_load`, `on_level_free` and `on_object_created` run a function of the mod at those points.
- **When:** `SdwModInit` runs once, when the game starts, before its window exists. Install every hook there and return 0: any other value unloads the mod (the log says so). `api->hook` returns 0, or -1 for a name the tables do not have and -2 for a function that cannot be hooked.
- **Also in the API:** `api->log(format, ...)` writes a line to `Mods\mods.log`, `api->mod_dir` is the mod's folder (with a trailing backslash), and `api->version` is the loader's API version: check it is at least 2 before `register_class` or `create_object`, and at least 3 before `setting`.
- **Sounds of a mod's own:** `sounds\<Level>\<id>.wav` (or `sounds\<id>.wav` for every level; `<id>` decimal or with `0x`, as in `sounds\Lvl-03\0x400.wav`) adds a sound to that level's sound bank. The game's `Sound_Play` (`u16 Sound_Play(u16 id, void *object, u16 volume, u8 flags, s32 rate)`, `__cdecl`, from `api->find`) plays it: volume 0-255, rate 4096 for the sound's own pitch, positioned at the object with the flags `SNDF_*` (`sdw_enums.h`) as the game's own objects do. The game's ids go up to 0x157; take ids from 0x400 to 0xFFFF for new sounds, or a game sound's id to replace it. A bank holds 256 sounds, and the largest has 84. Windows can also play a mod's sounds alongside the game's: `PlaySoundA` (winmm) with a `.wav`, or one made in memory as `SpeedHoney` does.
- **Headers:** do not include `windows.h` together with `sdw_classes.h`: the source's SDK stand-ins (`src/sdk/`) and the real SDK headers define the same types. Declare the few Windows functions a mod needs, as `SpeedHoney` does with `LoadLibraryA` and `GetProcAddress`.
- **Limits:** one game function, `Res_RelocateIdLists`, starts with a short jump and cannot be hooked; every other one can (`python3 tools/build_mods.py --check-hooks` checks them all; it reads the disassembly, BUILDING.md §2).

## New kinds of objects

`api->register_class(id, factory, likeClassId)` gives a class id a factory in the mod, with the class flags and inventory icons of game class `likeClassId`: the game calls it for each object of that class a level has, and a level patch gives an object that class (`<res> class <id>`). The game uses class ids 0-196 of its 200, so a mod class takes 197, 198 or 199 (or an id the game leaves unregistered). A factory usually starts from a game object, `api->create_object(gameClassId, record)`, and gives it a copy of its vtable with some methods replaced: `SpeedHoney` does this with static scenery and its `Render`. The object vtable's first slots are `PostLoadInit`, `Update`, `Render`, `CustomCollide`, `HandleMessage`, `Reset`, `RenderScaled` and `SetPosition` (`sdw_classes.h`, class `ScnObject`).

## Building

The loader and the examples are built with MinGW-w64 for 32-bit Windows (`i686-w64-mingw32-gcc`): on Windows from MSYS2 (the `mingw-w64-i686-gcc` package; add its `mingw32\bin`, for example `C:\msys64\mingw32\bin`, to the PATH), on macOS from Homebrew (`mingw-w64`), on Linux from the distribution. Set `SDW_MINGW` (or pass `--cc`) to the compiler's prefix if it is not `i686-w64-mingw32-`.

```bash
python3 tools/build_mods.py              # the loader and every mod in mods/examples/
python3 tools/build_mods.py --check-hooks
```

A new mod is built like the examples: put it in its own folder under `mods/examples/` (the folder's name is the mod's name) with its `.cpp` or `.c` files. The build makes `work/mods/Mods/<Name>/<Name>.dll`, copies `mod.txt`, `settings.txt`, `levels` and `files`, and makes `models\*.obj` into model files; copy a `sounds` folder yourself. It stops at the first mod that does not compile. Or compile it yourself:

```bash
i686-w64-mingw32-g++ -O2 -shared -Imods/kit -Isrc/include -o MyMod.dll MyMod.cpp -static -static-libgcc -static-libstdc++
```

A mod contains no part of the game, so it can be shared freely.
