/* Example mod: a new item, with its model, its behaviour and its place in a level. The honey pot of Level 10 is imported
 * into Level 3 (levels\Lvl-03.txt: `import`, then a new object of class 198 showing it, ahead of where Ralph starts).
 * Class 198 is built as the game builds static scenery (class 12, the generic object for its model) and turns
 * slowly; when Ralph reaches it, it is taken out of the world and Ralph runs half again as fast for
 * ten seconds: the maximum speeds of his movement profiles (g_wolfMoveProfilesNormal, 18 of 0x1c bytes, s16 maxSpeed
 * first) are raised, and put back when the time is up or the level ends. Taking it plays a sound of the mod's own: a
 * short chirp made in memory at start-up, played by Windows (winmm's PlaySound), so the mod carries no sound file. */
#include "sdw_mod.h"
#include "sdw_enums.h"
#include "scenaric_props.h" /* the class ids: generated from your disc (BUILDING.md) */
#include "sdw_classes.h"

#define CLASS_SPEED_HONEY 198
#define MAX_ITEMS 16
#define REACH_XZ 150            /* how close Ralph must come (units; y points down) */
#define REACH_Y 300
#define BOOST_TIME (10 * 4096)  /* ten seconds, in g_dt units (1/4096 s) */
#define PROFILES 18
#define PROFILE_SIZE 0x1c
#define TURN_PER_SECOND 1024

typedef void(SDW_THIS_CC *RenderFn)(ScnObject *self, SDW_THIS_EDX, Camera *view);
typedef void(SDW_THIS_CC *RemoveFn)(ScnObject *self, SDW_THIS_EDX);
static const SdwModApi *s_api;
static void *s_vtable[32];
static RenderFn s_render;
static RemoveFn s_remove;       /* ScnObject_RemoveFromWorld */
static s32 *s_dt;               /* g_dt */
static ScnObject **s_wolf;      /* g_pWolf */
static unsigned char *s_profiles; /* g_wolfMoveProfilesNormal */
static s16 s_savedSpeed[PROFILES];
static s32 s_boostLeft;
static ScnObject *s_items[MAX_ITEMS];
static int s_nItems;

/* the few Windows functions used (sdw_classes.h and windows.h do not mix: MODDING.md) */
extern "C" __declspec(dllimport) void *__stdcall LoadLibraryA(const char *name);
extern "C" __declspec(dllimport) void *__stdcall GetProcAddress(void *module, const char *name);
typedef int(__stdcall *PlaySoundFn)(const void *sound, void *module, unsigned long flags);
#define SND_ASYNC 0x0001
#define SND_MEMORY 0x0004
#define RATE 22050
#define CHIRP (RATE / 5) /* a fifth of a second */
static unsigned char s_wav[44 + CHIRP];
static PlaySoundFn s_playSound;

static void put32(unsigned char *p, unsigned v)
{
    p[0] = (unsigned char)v;
    p[1] = (unsigned char)(v >> 8);
    p[2] = (unsigned char)(v >> 16);
    p[3] = (unsigned char)(v >> 24);
}

/* a .wav image: 8-bit mono PCM, a square wave rising from 600 to 1400 Hz and fading out */
static void MakeChirp(void)
{
    static const char hdr[] = "RIFF....WAVEfmt \x10\0\0\0\x01\0\x01\0........\x01\0\x08\0data";
    unsigned phase = 0, i;
    for (i = 0; i < 40; i++)
        s_wav[i] = (unsigned char)hdr[i];
    put32(s_wav + 4, 36 + CHIRP);
    put32(s_wav + 24, RATE);
    put32(s_wav + 28, RATE);
    put32(s_wav + 40, CHIRP);
    for (i = 0; i < CHIRP; i++) {
        unsigned freq = 600 + 800 * i / CHIRP, amp = 48 * (CHIRP - i) / CHIRP;
        phase += freq * 65536 / RATE;
        s_wav[44 + i] = (unsigned char)(phase & 0x8000 ? 128 + amp : 128 - amp);
    }
}

static void SDW_THIS_CC Render(ScnObject *self, SDW_THIS_EDX, Camera *view)
{
    self->rot.y = (s16)((self->rot.y + (*s_dt * TURN_PER_SECOND >> 12)) & 0xFFF);
    s_render(self, edx_unused, view);
}

static void *__cdecl Create(void *record)
{
    ScnObject *obj = (ScnObject *)s_api->create_object(CLASSID_MISCSTATIC, record);
    if (!obj)
        return 0;
    if (!s_render) {
        void **vt = *(void ***)obj;
        int i;
        for (i = 0; i < 32; i++)
            s_vtable[i] = vt[i];
        s_render = (RenderFn)vt[2];
        s_vtable[2] = (void *)Render;
    }
    *(void ***)obj = s_vtable;
    if (s_nItems < MAX_ITEMS)
        s_items[s_nItems++] = obj;
    return obj;
}

static void SetBoost(int on)
{
    int i;
    for (i = 0; i < PROFILES; i++) {
        s16 *maxSpeed = (s16 *)(s_profiles + i * PROFILE_SIZE);
        if (on && !s_boostLeft)
            s_savedSpeed[i] = *maxSpeed;
        *maxSpeed = on ? (s16)(s_savedSpeed[i] * 3 / 2) : s_savedSpeed[i];
    }
}

static int Near(const ScnObject *a, const ScnObject *b)
{
    int dx = a->pos.x - b->pos.x, dy = a->pos.y - b->pos.y, dz = a->pos.z - b->pos.z;
    return dx > -REACH_XZ && dx < REACH_XZ && dz > -REACH_XZ && dz < REACH_XZ && dy > -REACH_Y && dy < REACH_Y;
}

static void __cdecl OnFrame(void)
{
    int i;
    if (s_boostLeft > 0 && (s_boostLeft -= *s_dt) <= 0) {
        SetBoost(0);
        s_boostLeft = 0;
    }
    for (i = 0; i < s_nItems; i++) {
        if (!*s_wolf || !Near(*s_wolf, s_items[i]))
            continue;
        s_remove(s_items[i], 0);
        s_items[i--] = s_items[--s_nItems];
        SetBoost(1);
        s_boostLeft = BOOST_TIME;
        s_api->log("SpeedHoney: Ralph took the honey");
        if (s_playSound)
            s_playSound(s_wav, 0, SND_MEMORY | SND_ASYNC);
    }
}

static void __cdecl OnLevelFree(void)
{
    if (s_boostLeft > 0)
        SetBoost(0);
    s_boostLeft = 0;
    s_nItems = 0; /* the level's objects are freed with it */
}

SDW_MOD_EXPORT int SdwModInit(const SdwModApi *api)
{
    s_api = api;
    if (api->version < 2)
        return 1;
    s_dt = (s32 *)api->find("g_dt");
    s_wolf = (ScnObject **)api->find("g_pWolf");
    s_profiles = (unsigned char *)api->find("g_wolfMoveProfilesNormal");
    s_remove = (RemoveFn)api->find("ScnObject_RemoveFromWorld");
    if (!s_dt || !s_wolf || !s_profiles || !s_remove)
        return 1;
    MakeChirp();
    if (void *winmm = LoadLibraryA("winmm.dll"))
        s_playSound = (PlaySoundFn)GetProcAddress(winmm, "PlaySoundA");
    api->on_frame(OnFrame);
    api->on_level_free(OnLevelFree);
    return api->register_class(CLASS_SPEED_HONEY, Create, CLASSID_MISCSTATIC);
}
