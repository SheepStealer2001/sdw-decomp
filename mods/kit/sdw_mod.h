/* The Sheep, Dog 'n' Wolf mod API: what a mod DLL gets from the mod loader (mods/loader, a stand-in dinput8.dll next to
 * SheepD3D.exe). A mod is a 32-bit DLL in Mods\<Name>\<Name>.dll that exports SdwModInit:
 *
 *     #include "sdw_mod.h"
 *     SDW_MOD_EXPORT int SdwModInit(const SdwModApi *api) { ...; return 0; }
 *
 * SdwModInit runs once, when the game starts (before its window exists), and returns 0 to stay loaded. Install every
 * hook there: hooks installed later can race the game's own threads.
 *
 * Game functions are found by the names in data/symbols*.csv (for example "Rocket_HandleMessage"), the same names the
 * decompiled source uses, so a mod does not depend on addresses. The game's classes and their fields are in
 * src/include/sdw_classes.h (C++; its layout checks also hold under MinGW). Call game functions through pointers from
 * find(), not through the class declarations: the game's code is not linked into a mod. A member function (thiscall:
 * `this` in ECX) is hooked or called portably as __fastcall with an unused second parameter (EDX), see SDW_THIS.
 *
 * Version 2 added the fields after on_object_created, version 3 the settings; a mod that uses them checks
 * api->version. Each mod gets its own api (its mod_dir and settings). A mod folder may also hold mod.txt, which the
 * loader reads for the Mods menu and the load order (MODDING.md). */
#ifndef SDW_MOD_H
#define SDW_MOD_H

#define SDW_MOD_API_VERSION 3

#ifdef __cplusplus
#define SDW_MOD_EXPORT extern "C" __declspec(dllexport)
extern "C" {
#else
#define SDW_MOD_EXPORT __declspec(dllexport)
#endif

/* A member function's hook or pointer: `r (SDW_THIS_CC *)(Class *self, SDW_THIS_EDX, args...)` */
#define SDW_THIS_CC __fastcall
#define SDW_THIS_EDX void *edx_unused

typedef void (*SdwEventFn)(void);
typedef void (*SdwObjectFn)(void *object);           /* a ScnObject * */
typedef void *(__cdecl *SdwFactoryFn)(void *record); /* returns a ScnObject * */

typedef struct SdwModApi {
    unsigned version;    /* SDW_MOD_API_VERSION of the loader */
    const char *game;    /* which exe the loader found: "SheepD3D PAL" (or "SheepD3D PAL, widescreen build") */
    const char *mod_dir; /* this mod's folder, with a trailing backslash (for its own files) */

    /* The address of a game function or global by its table name, or 0 if the tables have no such name. */
    void *(*find)(const char *name);

    /* Replaces the game function `name` with `replacement`, which must have the same calling convention and
     * parameters. *original receives a pointer that runs the game's own function (or the previous mod's hook). Two
     * mods may hook the same function: the later one runs first. Returns 0, or -1 (unknown name) / -2 (the function's
     * first instructions cannot be moved; the log says which). */
    int (*hook)(const char *name, void *replacement, void **original);
    int (*hook_address)(void *target, void *replacement, void **original);

    /* A line in Mods\mods.log (prefixed with the mod's name while its SdwModInit runs). printf-style. */
    void (*log)(const char *fmt, ...);

    /* Events. Each runs every function registered for it, in the order the mods were loaded. */
    void (*on_frame)(SdwEventFn fn);           /* once per frame, after the game's clock (Time_Update) */
    void (*on_level_load)(SdwEventFn fn);      /* after a level has loaded (Game_ReloadLevel) */
    void (*on_level_free)(SdwEventFn fn);      /* before a level is freed (Game_FreeLevel) */
    void (*on_object_created)(SdwObjectFn fn); /* each scenaric object a level load creates */

    /* ---- version 2: new object classes ---- */

    /* Registers `factory` for class id `classId`, with the class flags and inventory icons of `likeClassId`; the
     * game then calls it for every object of that class a level has (a level patch gives an object a class with
     * `<res> class <id>`). The game uses ids 0-196 of its 200: a mod class takes 197, 198 or 199, or an id the game
     * leaves unregistered. 0, or -1 if the id is out of range. The factory returns the new object (a ScnObject *). */
    int (*register_class)(unsigned short classId, SdwFactoryFn factory, unsigned short likeClassId);
    /* Creates an object of an existing class from a level record, as the game does (Scenaric_CreateObject: the
     * class's factory, or for a class without one, such as static scenery (12), the generic object for the record's
     * model): how a mod class starts from a game class before changing its behaviour. */
    void *(*create_object)(unsigned short classId, void *record);

    /* ---- version 3: settings ---- */

    /* A setting of this mod: `key = value` lines of settings.txt in its folder (`#` starts a comment), or `fallback`
     * when it has no such line. Pass the api the mod was given: setting(api, "width", "1024"). */
    const char *(*setting)(const struct SdwModApi *self, const char *key, const char *fallback);
    int (*setting_int)(const struct SdwModApi *self, const char *key, int fallback);
} SdwModApi;

typedef int (*SdwModInitFn)(const SdwModApi *api);

#ifdef __cplusplus
}
#endif
#endif
