/*
 * T298 - original object guessed as Input.cpp (data/tu_map.json). Ranges: .text 0x55e480-0x55f6a1, .data
 * 0x57eb64-0x57ebd4, .bss 0x719620-0x71af50: the pad / input layer and Pad_MenuHeld 0x55f441 / Pad_MenuPressed 0x55f55f
 * (Crc32 after them is T299, the registry functions T300).
 *
 *  - The .data and .bss of the object are defined here in address order (values from the exe, tools/data_init.py).
 *    .bss uses explicit zero initialisers, which VC6 keeps in definition order (uninitialised globals would be ordered
 *    by a hash of their names). The original spelling is unknown.
 *  - g_padMenuCur / g_padMenuPrev / g_padMenuRepeat (0x71965a / 0x71965c / 0x719668) are fields of g_pad, not objects:
 *    written as macros for the fields, they compile to the same addresses. Other objects read g_padPrevButtons 0x71964c,
 *    g_padCurButtons 0x719654 (g_pad.prev/cur.buttons) and g_padMasks 0x57eb80 (&g_inputMap[4]) the same way; nothing
 *    can define those names as symbols, so their users need the same field / element spelling to link.
 *  - Pad is 0x64 bytes in the generated headers, but g_pad and g_pad2 are 0x70 apart and g_padRawSnapshot follows
 *    g_pad2 at +0x6c: sizeof(Pad) is 0x6c in the original (an object of 64 bytes or more is 8-aligned, so 0x6c gives
 *    exactly these gaps). The 8 bytes data/structs/Pad.csv does not lay out (nothing reads them) are defined as the
 *    placeholders g_padTail_* after each Pad.
 *  - The rumble tables are in .data, so they were not const: their decorated names lose the const (users in other
 *    objects declare them `extern const u8 ...[]`).
 *  - Clock globals as the Time object (T304) defines them: g_gameTimeMs s32 (only copied here); g_dtRawMs s32, read
 *    here through a macro as its low half-word, which is the s16 load (movsx) the original makes.
 */
/* BYTES: cast, dead-code, inline, layout, view. */
/* BYTES(layout): written '= 0' only to keep definition order in .bss */
/* BYTES(layout): placeholder: the last 8 bytes of a 0x6c-byte Pad (the generated Pad is 0x64); drop when Pad.csv gains them */
/* BYTES(layout): placeholder: the object's remaining .bss (inferred: the recorded pad data) */
/* BYTES(layout): placeholder: four initialised globals no code refers to (types and names unknown) */
/* BYTES(layout): not const: the original has these tables in .data */
/* BYTES(view): g_padMenuCur / g_padMenuPrev / g_padMenuRepeat / g_padPrevButtons / g_padCurButtons macros: spelled as the g_pad fields they are (macros), not separate globals */
/* BYTES(cast): g_dtRawMs macro: read as its low half-word (movsx), as the original does */
/* BYTES(inline): Pad::SetActuator, Progress::SetPadIsAnalog (member-macro inlines): source-only inline: its expansion gives the original's shape */
/* BYTES(inline): Swap<T> (template inline): source-only inline: two pointer temps and a value temp per call, as the original */
/* BYTES(layout): the Cine.h header static: every object including the cinematic header carries this copy in its .data, referenced or not */
/*
 * The pad / input layer, SheepD3D.exe 0x55e480-0x55f440: the PS1-style pad buffers (g_pad, g_pad2) filled from the
 * DirectInput manager g_inputMgr, stick dead zones and analog-to-stick conversion, the button remap table, the
 * record / playback modes and their .PAD files, latching and auto-repeat, then (0x55f441 onwards) the menu pad tests.
 *
 * The structs PadFrame, PadRepeat, PadRecHeader, ControlConfig and Pad (raw / prev / cur as PadFrame, menuCur/menuPrev
 * +0x32/+0x34, +0x59/+0x5a/+0x5c/+0x5d) are in data/structs. The names are descriptive, not recovered (the binary has no
 * symbols). Local names were picked for the /Od frame order (tools/vc6_locals.py): they are plausible, not recovered.
 *
 * The pad frames follow the PlayStation's libpad receive buffer: byte 0 status (0 = ok), byte 1 = type << 4 | length,
 * then the active-low button word and four stick bytes. The raw buffer is 34 bytes on the PS1, which is why the
 * previous frame starts at +0x22. The type nibble is read and written as a C bitfield (shr 4 / and 0xf):
 * PadFrame.typeLen is the bitfield struct PadTypeLen {len:4, type:4}.
 */
#define SDW_MEMBERS_Pad                            \
    /* source-only inline (Input_Init 0x55e63b) */ \
    inline s32 IsAnalog(); /* source-only inline, defined below */
#define SDW_MEMBERS_Progress           \
    void SetPadIsAnalog(u8 analog)     \
    {                                  \
        controls.padIsAnalog = analog; \
    } /* source-only inline (Input_Poll 0x55e98b) */
#include "sdw_enums.h"
#include "sdw_classes.h"
#define SDW_INLINE_PAD_SETACTUATOR_INT 1
#include "input_inlines.h"
#undef SDW_INLINE_PAD_SETACTUATOR_INT

/* source-only inline: the current frame is from an analog pad (type 7). Its int result, narrowed to the u8 argument,
 * is what gives Input_Poll's sub/neg/sbb/inc at 0x55e983 (a bool expression passed straight on compiles to sete). */
/* BYTES(cast): source-only inline returning int: narrowed to the u8 argument it gives the original's sub / neg / sbb / inc (0x55e983) */
inline s32 Pad::IsAnalog()
{
    return cur.typeLen.type == PADTYPE_ANALOG;
}

#include "../sdk/crt.h"

/* ---- globals ---- */
#include "draw2d.h"
#include "progress.h"
#include "maths.h"
#include "file.h"
extern s32 g_gameTimeMs;               /* 0x71b2d4 (Time, T304) */
extern s32 g_dtRawMs;                  /* 0x71b2d8 (Time, T304, defines it s32) */
#define g_dtRawMs (*(s16 *)&g_dtRawMs) /* cast kept: read here as its low half-word, as the original does (movsx) */

/* ---- this object's .data, in address order ---- */
/* 0x57eb64 - the cinematic header's opcode-stride table (g_cineOpStride 0x5816fc): a header static that every object
 * including that header carries in its .data, referenced or not (unreferenced here). */
static u8 g_cineOpStride_57eb64[9] = {0, 8, 8, 4, 2, 2, 4, 2, 2};
/* 0x57eb6e-0x57eb78 - four initialised globals no code refers to (types and names unknown). The zero half-word at
 * 0x57eb76 is alignment padding: g_inputMap, an array of 3 bytes or more, is 4-aligned (a u16 here and a u32 0xffff
 * give the same bytes; the half-word reading needs no invented zero). */
u16 g_inputUnref_57eb6e = 0x1000;
u16 g_inputUnref_57eb70 = 0x0100;
s16 g_inputUnref_57eb72 = -1;
u16 g_inputUnref_57eb74 = 0xffff;
/* 0x57eb78 - button mapping (read with movzx: unsigned). Entries 4.. are the active-low pad masks that other objects
 * read as g_padMasks (0x57eb80 = &g_inputMap[4]; not a separate object, so no symbol of that name is defined here). */
u16 g_inputMap[16] = {(u16)~PAD_SELECT,   (u16)~PAD_L3,     (u16)~PAD_R3,    (u16)~PAD_START,
                      (u16)~PAD_UP,       (u16)~PAD_RIGHT,  (u16)~PAD_DOWN,  (u16)~PAD_LEFT,
                      (u16)~PAD_L2,       (u16)~PAD_R2,     (u16)~PAD_L1,    (u16)~PAD_R1,
                      (u16)~PAD_TRIANGLE, (u16)~PAD_CIRCLE, (u16)~PAD_CROSS, (u16)~PAD_SQUARE};
u8 g_inputMapAux[16] = {0,   0,   0,   0,   0,  0,  0,  0,
                        198, 203, 197, 235, 93, 94, 95, 92}; /* 0x57eb98  swapped in lockstep with g_inputMap */
/* the rumble patterns passed to Pad::Rumble_stub; in .data, so not const in the original */
u8 g_rumbleSeqKill[5] = {0xc8, 0x40, 0x80, 0x20, 0xff};         /* 0x57eba8 */
u8 g_rumbleSeqImpact[6] = {0x20, 0x80, 0x40, 0x60, 0x20, 0xff}; /* 0x57ebb0 */
u8 g_rumbleSeqFallingRock[4] = {0x20, 0xc8, 0x20, 0xff};        /* 0x57ebb8 */
/* (0x57ebbc-0x57ebd4: the literals of PadRec_Load / PadRec_Save) */

/* ---- this object's .bss, in address order (explicit zero initialisers keep definition order, see the header) ---- */
u8 g_inputMode = 0;                   /* 0x719620  0 live, 1 record, 2 playback */
s16 g_padRepeatDelay = 0;             /* 0x719622  auto-repeat threshold, ms */
Pad g_pad = {0};                      /* 0x719628 */
u32 g_padTail_71968c[2] = {0};        /* 0x71968c  unreferenced: the last 8 bytes of a 0x6c-byte Pad (see the header) */
Pad g_pad2 = {0};                     /* 0x719698 */
u32 g_padTail_7196fc[2] = {0};        /* 0x7196fc  likewise for g_pad2 */
PadFrame g_padRawSnapshot = {0};      /* 0x719704 */
u32 g_inputUnref_71970c[7] = {0};     /* 0x71970c  28 bytes no code refers to (unknown) */
PadRecHeader g_padRec = {0};          /* 0x719728  .remap = g_savedRemap 0x71972a */
u8 g_padRecData_719750[0x1800] = {0}; /* 0x719750  no code names these bytes (the rest of the object's .bss). Inferred:
                                          * the recorded pad data after the 0x28-byte header, reached through &g_padRec
                                          * (PadRec_Save writes 0x28 + dataLen bytes from it, PadRec_Load copies a file over it) */

/* Fields of g_pad that other code reads under their own names (0x71964c.. are inside g_pad, not objects of their own) */
#define g_padMenuCur (g_pad.menuCur)              /* 0x71965a */
#define g_padMenuPrev (g_pad.menuPrev)            /* 0x71965c */
#define g_padMenuRepeat (g_pad.menuRepeat.output) /* 0x719668 */

/* ---- functions ---- */
void Pad_DetectType(Pad *pad);
void Pad_ReadRaw(Pad *pad);
void Pad_InitPair(Pad *p1, Pad *p2);
void PadFrame_Clear(PadFrame *frame);
void Pad_ClearFrames(Pad *pad);
void Input_SetMode(u8 mode);
u16 Input_GetMapping(u8 slot);
void Input_SwapMapping(u8 slot, u16 value);
void Input_ApplyRemap(u16 slotC, u16 slotD, u16 slotE, u16 slotF, u16 slotB, u16 slotA);

/* source-only inline: the swap in Input_SwapMapping (two pointer temps and a value temp per call) */
template <class T> inline void Swap(T &a, T &b)
{
    T t = a;
    a = b;
    b = t;
}

/* 0x55e480 - device type from the input manager: 4 for a digital device (keyboard), else 7 (analog) and the state
 * is cleared. */
void Pad_DetectType(Pad *pad)
{
    if (g_inputMgr.IsDeviceDigital(0) == 1) {
        pad->padType = PADTYPE_DIGITAL;
        pad->padTypeReq = PADTYPE_DIGITAL;
    } else {
        pad->state59 = 0;
        pad->state5a = 0;
        pad->state44 = 0;
        pad->state48 = 0;
        pad->state4c = 0;
        pad->state60 = 0;
        pad->actuatorEnable = 0;
        pad->state54 = 0;
        pad->padType = PADTYPE_ANALOG;
        pad->padTypeReq = PADTYPE_ANALOG;
    }
}

/* 0x55e4fb - polls the input manager into pad->raw. On failure only the status is set (the buttons stay stale). The
 * type always comes from g_pad, whatever pad is being filled. */
void Pad_ReadRaw(Pad *pad)
{
    pad->raw.rightY = 0x80;
    pad->raw.rightX = 0x80;
    if (g_inputMgr.Poll() < 0) {
        pad->raw.status = 1;
    } else {
        Pad_DetectType(&g_pad);
        pad->raw.status = 0;
        pad->raw.typeLen.type = g_pad.padType;
        pad->raw.leftX = g_inputMgr.axisX;
        pad->raw.leftY = g_inputMgr.axisY;
        pad->raw.buttons = g_inputMgr.padBits;
    }
}

/* 0x55e578 */
void Pad_InitPair(Pad *p1, Pad *p2)
{
    p1->actuatorEnable = 0;
    p2->actuatorEnable = 0;
    g_padRepeatDelay = 500;
    p1->ResetState();
    p2->ResetState();
}

/* 0x55e5a4 - idle frame: no button down (active low), both sticks centred. */
/* BYTES(view, inferred): the two stick bytes are stored as one u16, as the original does */
void PadFrame_Clear(PadFrame *frame)
{
    frame->buttons = 0xffff;
    /* cast kept: one 16-bit store centres both bytes of each stick (X and Y), as the original does */
    *(u16 *)&frame->leftX = 0x8080;
    *(u16 *)&frame->rightX = 0x8080;
}

/* 0x55e5c4 */
void Pad_ClearFrames(Pad *pad)
{
    PadFrame_Clear(&pad->cur);
    PadFrame_Clear(&pad->prev);
    PadFrame_Clear(&pad->raw);
}

/* 0x55e5f3 - input bring-up (from Load_DAVnWAR): opens both pads, enables the actuators of an analog pad, clears the
 * frames, goes live and restores the saved button layout. */
void Input_Init()
{
    Pad_InitPair(&g_pad, &g_pad2);
    g_inputMgr.SetAcquiredAll(1);
    g_pad.Open(PAD_PORT_1);
    g_pad2.Open(PAD_PORT_2);
    if (g_pad.padTypeReq == PADTYPE_ANALOG)
        g_pad.SetActuator(1);
    if (g_pad2.padTypeReq == PADTYPE_ANALOG)
        g_pad2.SetActuator(1);
    Pad_ClearFrames(&g_pad);
    Pad_ClearFrames(&g_pad2);
    Input_SetMode(INPUT_MODE_LIVE);
    Input_ApplyRemap(g_pProgress->controls.remap[2], g_pProgress->controls.remap[3], g_pProgress->controls.remap[0],
                     g_pProgress->controls.remap[1], g_pProgress->controls.remap[4], g_pProgress->controls.remap[5]);
}

/* 0x55e6e8 */
void Input_Reacquire()
{
    g_inputMgr.SetAcquiredAll(1);
    Input_SetMode(INPUT_MODE_LIVE);
}

/* 0x55e703 */
void Input_Unacquire()
{
    g_inputMgr.SetAcquiredAll(0);
}

/* 0x55e714 - 0 live, 1 record (snapshots the button map into the recording header), 2 playback (latches both pads).
 * Entering record while playing back, or playback while recording, hangs on purpose (an assert of the PS1 code). */
void Input_SetMode(u8 mode)
{
    switch (mode) {
        case INPUT_MODE_RECORD:
            if (g_inputMode == INPUT_MODE_PLAYBACK)
                while (1)
                    ;
            g_padRec.remap[0] = Input_GetMapping(INPUT_SLOT_CROSS);
            g_padRec.remap[1] = Input_GetMapping(INPUT_SLOT_SQUARE);
            g_padRec.remap[2] = Input_GetMapping(INPUT_SLOT_TRIANGLE);
            g_padRec.remap[3] = Input_GetMapping(INPUT_SLOT_CIRCLE);
            g_padRec.remap[4] = Input_GetMapping(INPUT_SLOT_R1);
            g_padRec.remap[5] = Input_GetMapping(INPUT_SLOT_L1);
            break;
        case INPUT_MODE_PLAYBACK:
            g_pad.Latch();
            g_pad2.Latch();
            if (g_inputMode == INPUT_MODE_RECORD)
                while (1)
                    ;
            break;
    }
    g_inputMode = mode;
}

/* 0x55e7e0 */
u8 Input_GetMode()
{
    return g_inputMode;
}

/* 0x55e7ea - loads <name>.PAD into the recording buffer (copying as many bytes as the allocation holds, with no check
 * against the buffer), applies its button layout and starts playback. No caller: record/playback is dead in this
 * build. path[64] sits at EBP-0x48 (8-aligned); a 65..68-byte array would move it to EBP-0x50. */
void PadRec_Load(const char *name)
{
    void *data;
    char path[0x40];
    unsigned int size;
    Str_Concat2(path, name, ".PAD");
    data = File_LoadWhole(path);
    if (data != 0) {
        size = _msize(data);
        memcpy(&g_padRec, data, size);
        if (data != 0) {
            free(data);
            data = 0;
        }
        Input_ApplyRemap(g_padRec.remap[2], g_padRec.remap[3], g_padRec.remap[0], g_padRec.remap[1], g_padRec.remap[4],
                         g_padRec.remap[5]);
        Input_SetMode(INPUT_MODE_PLAYBACK);
    }
}

/* 0x55e89b - writes the recording (0x28-byte header + dataLen) to ..\Data<name>.pad (no separator) and goes live.
 * No caller. */
void PadRec_Save(const char *name)
{
    char path[0x40];
    u32 size = g_padRec.dataLen + 0x28;
    g_padRec.gameTimeMs = g_gameTimeMs;
    g_padRec.version = 3;
    g_padRec.padType = PADTYPE_ANALOG;
    Str_Concat2(path, "..\\Data", name);
    Str_Concat2(path, path, ".pad");
    File_SaveWhole(path, &g_padRec, size);
    Input_SetMode(INPUT_MODE_LIVE);
}

/* 0x55e913 - once per frame (Main_Loop): reads g_pad; when live, latches both pads and follows a change of pad type
 * into the saved Progress+0x9c. Only g_pad is ever read: g_pad2 is latched from the frame Pad_ClearFrames left.
 * `buttons` is set and never read (0x55e93c). */
/* BYTES(dead-code): buttons is set and never read, as in the original (0x55e93c) */
void Input_Poll()
{
    Pad_ReadRaw(&g_pad);
    g_padRawSnapshot = g_pad.raw;
    u16 buttons = 0xffff;
    switch (g_inputMode) {
        case INPUT_MODE_LIVE:
            g_pad.Latch();
            g_pad2.Latch();
            if (g_pad.TypeChanged())
                g_pProgress->SetPadIsAnalog(g_pad.IsAnalog());
            if (g_pad.JustConnected())
                g_pad.OnConnected_stub();
            break;
    }
}

/* 0x55e9bf - raw stick bytes (centre 0x80) to axes in -256..256: a SQUARE dead zone (0,0 only when BOTH |x| and |y|
 * are under 56; otherwise the small axis is kept as it is), then each axis clamped to +-120 and scaled by 256/120, so
 * a full diagonal gives (256, 256). */
void Pad_StickToDeadzonedAxes(u8 rawX, u8 rawY, int *outX, int *outY)
{
    s16 xVal = rawX - 0x80;
    s16 yVal = rawY - 0x80;
    s16 xAbs = xVal >= 0 ? xVal : -xVal;
    s16 yAbs = yVal >= 0 ? yVal : -yVal;
    if (xAbs < 0x38 && yAbs < 0x38) {
        *outX = 0;
        *outY = 0;
    } else {
        if (xVal > 0) {
            if (xVal > 0x78)
                xVal = 0x78;
        } else if (xVal < -0x78)
            xVal = -0x78;
        if (yVal > 0) {
            if (yVal > 0x78)
                yVal = 0x78;
        } else if (yVal < -0x78)
            yVal = -0x78;
        *outX = xVal * 256 / 0x78;
        *outY = yVal * 256 / 0x78;
    }
}

/* 0x55eac6 - raw stick bytes to a direction vector: a ROUND dead zone of radius 56, then the length clamped to 120 and
 * mapped 56..120 -> 0..256 along the stick's direction. Returns that strength (0..256). */
int Pad_AnalogToStick(u8 rawX, u8 rawY, int *outX, int *outY)
{
    s16 x = rawX - 0x80;
    s16 y = rawY - 0x80;
    int dist = x * x + y * y; /* squared, then the length, then the 20.12 scale factor */
    if (dist <= 0xc40) {
        *outX = 0;
        *outY = 0;
        return 0;
    }
    dist = (int)sqrt((double)dist);
    int strength = dist;
    if (strength > 0x78)
        strength = 0x78;
    dist = ((strength - 0x38) << 20) / (dist << 6);
    *outX = x * dist / 4096;
    *outY = y * dist / 4096;
    return (strength - 0x38) * 256 / 64;
}

/* 0x55ebad */
void Input_EmptyStub() {}

/* 0x55ebb2 */
u16 Input_GetMapping(u8 slot)
{
    return g_inputMap[slot];
}

/* 0x55ebc3 - puts `value` at map slot `slot` by swapping it with wherever it is now, so the map stays a permutation
 * (value not found: the search stops at index 16, one past the table, and that entry is swapped). */
void Input_SwapMapping(u8 slot, u16 value)
{
    u8 i = 0;
    while (g_inputMap[i] != value && i < 0x10)
        i++;
    Swap(g_inputMap[i], g_inputMap[slot]);
    Swap(g_inputMapAux[i], g_inputMapAux[slot]);
}

/* 0x55ec69 - a saved layout: the six remappable slots in the order c, d, e, f, b, a. */
void Input_ApplyRemap(u16 slotC, u16 slotD, u16 slotE, u16 slotF, u16 slotB, u16 slotA)
{
    Input_SwapMapping(INPUT_SLOT_TRIANGLE, slotC);
    Input_SwapMapping(INPUT_SLOT_CIRCLE, slotD);
    Input_SwapMapping(INPUT_SLOT_CROSS, slotE);
    Input_SwapMapping(INPUT_SLOT_SQUARE, slotF);
    Input_SwapMapping(INPUT_SLOT_R1, slotB);
    Input_SwapMapping(INPUT_SLOT_L1, slotA);
}

/* 0x55ecc8 - the options screen's control preset (Progress+0x9a): three fixed layouts or the custom one. */
void Input_ApplyControlConfig(ControlConfig *cfg)
{
    switch (cfg->preset) {
        case CTRL_PRESET_A:
            Input_ApplyRemap((u16)~PAD_TRIANGLE, (u16)~PAD_CIRCLE, (u16)~PAD_CROSS, (u16)~PAD_SQUARE, (u16)~PAD_R1,
                             (u16)~PAD_L1);
            break;
        case CTRL_PRESET_B:
            Input_ApplyRemap((u16)~PAD_SQUARE, (u16)~PAD_TRIANGLE, (u16)~PAD_CIRCLE, (u16)~PAD_CROSS, (u16)~PAD_R1,
                             (u16)~PAD_L1);
            break;
        case CTRL_PRESET_C:
            Input_ApplyRemap((u16)~PAD_CROSS, (u16)~PAD_SQUARE, (u16)~PAD_TRIANGLE, (u16)~PAD_CIRCLE, (u16)~PAD_R1,
                             (u16)~PAD_L1);
            break;
        case CTRL_PRESET_CUSTOM:
            Input_ApplyRemap(cfg->remap[2], cfg->remap[3], cfg->remap[0], cfg->remap[1], cfg->remap[4], cfg->remap[5]);
            break;
    }
}

/* 0x55edb0 - the live map into the custom layout. */
void Input_StoreControlConfig(ControlConfig *cfg)
{
    cfg->remap[2] = Input_GetMapping(INPUT_SLOT_TRIANGLE);
    cfg->remap[3] = Input_GetMapping(INPUT_SLOT_CIRCLE);
    cfg->remap[0] = Input_GetMapping(INPUT_SLOT_CROSS);
    cfg->remap[1] = Input_GetMapping(INPUT_SLOT_SQUARE);
    cfg->remap[4] = Input_GetMapping(INPUT_SLOT_R1);
    cfg->remap[5] = Input_GetMapping(INPUT_SLOT_L1);
}

/* 0x55ee1b */
void Pad::ResetState()
{
    padType = PADTYPE_NONE;
    padTypeReq = PADTYPE_NONE;
    state59 = 0;
    state5a = 0;
    state5c = 0;
    state5d = 0;
    state44 = 0;
    state48 = 0;
    state4c = 0;
    state60 = 0;
    state50 = 0x1000;
    state54 = 0;
}

/* 0x55ee88 - auto-repeat (`this` unused): the value passes through on the frame it changes and again every
 * g_padRepeatDelay ms while it is held (500 ms first, then 125 ms; the delay is ONE global shared by every channel of
 * both pads); in between the output is 0xffff. A repeat pulse ORs in 0xff06: in the active-low word that releases the
 * whole upper byte (L2 R2 L1 R1 Triangle Circle Cross Square) and L3/R3, so only the d-pad, Select and Start repeat. */
void Pad::AutoRepeat(PadRepeat *rep, u16 value)
{
    rep->output = value;
    if (rep->output == rep->lastValue) {
        rep->timerMs += g_dtRawMs;
        if (rep->timerMs < g_padRepeatDelay) {
            rep->output = 0xffff;
        } else {
            g_padRepeatDelay = 0x7d;
            rep->timerMs = 0;
            rep->output |= PAD_L3 | PAD_R3 | PAD_L2 | PAD_R2 | PAD_L1 | PAD_R1 | PAD_TRIANGLE | PAD_CIRCLE | PAD_CROSS |
                           PAD_SQUARE;
        }
    } else {
        g_padRepeatDelay = 500;
        rep->lastValue = rep->output;
        rep->timerMs = 0;
    }
}

/* 0x55ef29 - an exact copy of Pad::Latch (no caller found). */
void Pad::Latch_Dup()
{
    raw.buttons |= ~((u16)~PAD_L3 & (u16)~PAD_R3);
    if (!IsConnected())
        raw.buttons = PAD_ALL_RELEASED;
    AutoRepeat(&btnRepeat, cur.buttons);
    AnalogToDpadBits();
    prev = cur;
    cur = raw;
}

/* 0x55ef9f - the frame step: L3/R3 forced released (bits 1 and 2; the constant is 0xffff0006, i.e. ~0xfff9), all
 * released when disconnected, the button auto-repeat and the stick-as-d-pad word updated, then prev = cur and
 * cur = raw. "Disconnected" is the status of the frame latched LAST time, so a failed poll is seen one frame late. */
void Pad::Latch()
{
    raw.buttons |= ~((u16)~PAD_L3 & (u16)~PAD_R3);
    if (!IsConnected())
        raw.buttons = PAD_ALL_RELEASED;
    AutoRepeat(&btnRepeat, cur.buttons);
    AnalogToDpadBits();
    prev = cur;
    cur = raw;
}

/* 0x55f015 - the button word on a frame it changed while something outside ignoreMask is down, else 0xffff. */
u16 Pad::GetChangedPress(u16 ignoreMask)
{
    if (cur.status == 0 && cur.buttons != prev.buttons && (cur.buttons | ignoreMask) != PAD_ALL_RELEASED)
        return cur.buttons;
    return PAD_ALL_RELEASED;
}

/* 0x55f061 */
s32 Pad::IsConnected()
{
    return !cur.status;
}

/* 0x55f078 - empty (a PS1 libpad call stubbed out) */
void Pad::Stub_55f078(u32 arg) {}

/* 0x55f085 - empty: no rumble on PC */
void Pad::Rumble_stub(s32 durationMs, const u8 *pattern, s16 strength) {}

/* 0x55f092 - empty */
void Pad::OnConnected_stub() {}

/* 0x55f09d - empty */
void Pad::Stub_55f09d() {}

/* 0x55f0a8 - retries the first poll up to 60 times; then the same type detection as Pad_DetectType (on this pad).
 * `port` (0, 0x10: the PS1 port numbers) is unused; both pads poll the one input manager. */
void Pad::Open(int port)
{
    int tries = 0;
    while (g_inputMgr.Poll() < 0 && tries < 0x3c)
        tries++;
    if (tries == 0x3c)
        return;
    if (g_inputMgr.IsDeviceDigital(0) == 1) {
        padType = PADTYPE_DIGITAL;
        padTypeReq = PADTYPE_DIGITAL;
    } else {
        state59 = 0;
        state5a = 0;
        state44 = 0;
        state48 = 0;
        state4c = 0;
        state60 = 0;
        actuatorEnable = 0;
        state54 = 0;
        padType = PADTYPE_ANALOG;
        padTypeReq = PADTYPE_ANALOG;
    }
}

/* 0x55f15c - both frames have a type and it differs */
s32 Pad::TypeChanged()
{
    return cur.typeLen.type != PADTYPE_NONE && prev.typeLen.type != PADTYPE_NONE &&
           cur.typeLen.type != prev.typeLen.type;
}

/* 0x55f1c8 - a type now, none in the previous frame */
s32 Pad::JustConnected()
{
    return cur.typeLen.type != PADTYPE_NONE && prev.typeLen.type == PADTYPE_NONE;
}

/* 0x55f210 - menuCur = raw buttons with the left stick folded in as d-pad bits (past a dead zone of 56 per axis:
 * right 0x20, left 0x80, down 0x40, up 0x10), then its auto-repeat (g_padMenuRepeat). */
void Pad::AnalogToDpadBits()
{
    s16 x = raw.leftX - 0x80;
    s16 y = raw.leftY - 0x80;
    menuPrev = menuCur;
    menuCur = raw.buttons;
    if ((x >= 0 ? x : -x) < 0x38)
        x = 0;
    else if (x > 0)
        menuCur &= (u16)~PAD_RIGHT;
    else
        menuCur &= (u16)~PAD_LEFT;
    if ((y >= 0 ? y : -y) < 0x38)
        y = 0;
    else if (y > 0)
        menuCur &= (u16)~PAD_DOWN;
    else
        menuCur &= (u16)~PAD_UP;
    AutoRepeat(&menuRepeat, menuCur);
}

/* 0x55f323 - Pad_MenuHeld on the auto-repeat word: true on the press frame and on each repeat, with the same keyboard
 * fallbacks (Cross = Enter, Triangle = Esc, the d-pad masks = the arrow-key nav word). */
bool Pad_MenuRepeat(int activeLowMask)
{
    bool pulse = (g_padMenuRepeat & ~activeLowMask) == 0;
    switch (activeLowMask) {
        case (u16)~PAD_CROSS:
            pulse |= g_inputMgr.enterPressed == 1;
            break;
        case (u16)~PAD_TRIANGLE:
            pulse |= g_inputMgr.escPressed == 1;
            break;
        case (u16)~PAD_LEFT:
            pulse |= g_inputMgr.menuNav == (u16)~PAD_LEFT;
            break;
        case (u16)~PAD_RIGHT:
            pulse |= g_inputMgr.menuNav == (u16)~PAD_RIGHT;
            break;
        case (u16)~PAD_UP:
            pulse |= g_inputMgr.menuNav == (u16)~PAD_UP;
            break;
        case (u16)~PAD_DOWN:
            pulse |= g_inputMgr.menuNav == (u16)~PAD_DOWN;
            break;
    }
    return pulse;
}

/* 0x55f441 - is the pad button whose bit is CLEAR in activeLowMask held? The menu masks also accept the keyboard:
 * Cross 0xBFFF = Enter, Triangle 0xEFFF = Esc, and the four d-pad masks = the arrow-key nav word. Those keyboard inputs
 * are edges/auto-repeat pulses, so on the keyboard "held" is only true on the press and repeat frames. */
bool Pad_MenuHeld(int activeLowMask)
{
    bool held = (g_padMenuCur & ~activeLowMask) == 0;
    switch (activeLowMask) {
        case (u16)~PAD_CROSS:
            held |= g_inputMgr.enterPressed == 1;
            break;
        case (u16)~PAD_TRIANGLE:
            held |= g_inputMgr.escPressed == 1;
            break;
        case (u16)~PAD_LEFT:
            held |= g_inputMgr.menuNav == (u16)~PAD_LEFT;
            break;
        case (u16)~PAD_RIGHT:
            held |= g_inputMgr.menuNav == (u16)~PAD_RIGHT;
            break;
        case (u16)~PAD_UP:
            held |= g_inputMgr.menuNav == (u16)~PAD_UP;
            break;
        case (u16)~PAD_DOWN:
            held |= g_inputMgr.menuNav == (u16)~PAD_DOWN;
            break;
    }
    return held;
}

/* 0x55f55f - as Pad_MenuHeld, but only on the frame the button goes down (down now, up in the previous frame). */
bool Pad_MenuPressed(int activeLowMask)
{
    bool pressed = (g_padMenuCur & ~activeLowMask) == 0 && (g_padMenuPrev & ~activeLowMask) != 0;
    switch (activeLowMask) {
        case (u16)~PAD_CROSS:
            pressed |= g_inputMgr.enterPressed == 1;
            break;
        case (u16)~PAD_TRIANGLE:
            pressed |= g_inputMgr.escPressed == 1;
            break;
        case (u16)~PAD_LEFT:
            pressed |= g_inputMgr.menuNav == (u16)~PAD_LEFT;
            break;
        case (u16)~PAD_RIGHT:
            pressed |= g_inputMgr.menuNav == (u16)~PAD_RIGHT;
            break;
        case (u16)~PAD_UP:
            pressed |= g_inputMgr.menuNav == (u16)~PAD_UP;
            break;
        case (u16)~PAD_DOWN:
            pressed |= g_inputMgr.menuNav == (u16)~PAD_DOWN;
            break;
    }
    return pressed;
}
