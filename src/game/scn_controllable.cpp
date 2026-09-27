/*
 * T096 - the original object ScnControllable.cpp (guessed name), one file.
 * .text 0x491e10-0x493e75 (ScnControllable_NextIdleAnim .. Wolf_InitActionPromptStrings), no .rdata,
 * .data 0x57b218-0x57b298, .bss 0x6cf318-0x6cf448.
 * The first function before it, ScnControllable_IsStickTowardSide 0x491d25, is the last function of T095
 * (src/game/wolf_misc.cpp).
 * .data: this object's copy of the cinematic stride table (see the definition), the prompt string-id tables and
 * g_itemPromptValue, then Wolf_DrawActionPrompt's three literals ($SG, in the order the compiler meets them).
 * .bss: VC6 emits uninitialised globals in the order of a hash of their names and explicitly zero-initialised ones
 * after them in definition order; every global here is zero-initialised, so they come out in address order.
 *
 * ScnControllable - steering and the move/collide step shared by the player-controllable actors (Wolf, Robot),
 * SheepD3D.exe 0x49246f-0x4936ab. Which original file these were in is not settled: tools/find_library_ranges.py puts
 * 0x47d310-0x4935f6 in one file (the Wolf's), so they may be its tail.
 *
 * ScanInteractables and StopMotion use the ActionHit, InteractScan and ModelBoxList structs and Model::boxes. The Wolf_
 * functions are named for their caller but live here and touch only ScnControllable fields. The `work` structs only pin
 * the original stack offsets of locals (VC6 /Od orders them its own way); they are not a claim about the source text.
 * Neither are the inline helpers' names (they have no bodies in the exe). Angles: 4096 per turn. Speeds: units per
 * second.
 */
/* BYTES: cast, inline, layout, slot-group, slot-scope, temp, view. */
/* BYTES(layout): the Cine.h header static: every object including the cinematic header carries this copy in its .data, referenced or not */
/* BYTES(layout): written '= 0' only to keep definition order in .bss */
/* BYTES(cast, inferred): g_gameTimeMs / g_frameCount2 macros: read as u32 through a cast: the original compares them unsigned in this object */
#include "sdw_types.h"
struct Box16;
class Instance;
struct Animator;
u32 Anim_Start(Instance *, Animator *, u16, u32);

#define SDW_MEMBERS_ScnObject inline void GetSolidBoxes(u32 &count, CollBox *&box);


#include "sdw_enums.h"
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "../objects/instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_CINE_ISACTIVE 1
#include "../engine/cine_inlines.h"
#undef SDW_INLINE_CINE_ISACTIVE

/* ScnControllable input helpers */
#include "../engine/cine.h"
#include "../engine/input.h"
#include "../engine/maths.h"
#include "../engine/approach.h"
#include "../app/app_main.h"
#include "../engine/scenaric.h"
#include "../engine/scn_tools.h"
#include "../engine/text.h"
#include "../engine/interface.h"

#define g_padMasks (g_inputMap + 4)

s32 Rand_Bounded(s32);
#define ABS_VALUE(x) ((x) >= 0 ? (x) : -(x))
#define DOWN(pad, index) (((pad)->cur.buttons & ~g_padMasks[index]) == 0)
#define PRESSED(pad, index) (DOWN(pad, index) && ((pad)->prev.buttons & ~g_padMasks[index]) != 0)

/* src/game/scn_controllable.cpp */
/* engine/time_rand_math.cpp */
/* engine/math_fixed.c */
extern "C" s16 Math_RadiansToAngle4096(float radians);

#include "../sdk/crt.h"

#define g_camYaw (g_camera.rot.y)       /* 0x584d2a  camera heading */
extern "C" s16 g_sinTable4096[5122];    /* 0x57ece0  4.12 sine, 4096 steps per turn */
extern "C" const s16 *g_pCosTable;      /* 0x5814e4  = g_sinTable4096 + 1024 */
extern u8 g_sharedScratch[];            /* 0x6d5468  shared scratch; the scanner keeps an InteractScan there */
s32 Vec3s_DistSqXZ(Vec3s *a, Vec3s *b); /* 0x51588e  dx*dx + dz*dz */

/* Inline helpers. They have no bodies of their own in the exe (/Ob1 expands them); their names are descriptive, and each one is
 * there because its expansion is the only way found to give the original's temps and instruction shapes:
 * GetClassId + Scenaric_ClassFlags: a u16 temp for the id, then a u32 temp for the flags (0x49267d/0x492694; the same
 * pair at 0x46e274 and 0x491268). FlagsClear: `!` materialised as neg/sbb/inc before the test (0x4926bb). InstFlags:
 * the u16 mask loaded into a register first (mov eax,8 at 0x4926c8; the same shape on anim flags at 0x42fcd9). */
/* BYTES(inline): source-only inline: the only way found to give the original's temps (u16 id, u32 flags), ! as neg / sbb / inc, and the mask loaded into a register first */
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID

#define SDW_INLINE_FREE_SCENARIC_CLASSFLAGS_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_FREE_SCENARIC_CLASSFLAGS_U16

#define SDW_INLINE_SCNOBJECT_FLAGSCLEAR_U32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_FLAGSCLEAR_U32

#define SDW_INLINE_SCNOBJECT_INSTFLAGS_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_INSTFLAGS_U16

/* The model's box list with the leading disabled boxes (flags & 1) skipped: count and box are left at the first
 * enabled one. Inlined; the same expansion (one list temp) is at 0x4926fa here and 0x51101c in 0x510f34. */
/* BYTES(inline): source-only inline: its expansion has one list temp (0x4926fa) */
inline void ScnObject::GetSolidBoxes(u32 &count, CollBox *&box)
{
    ModelBoxList *list = inst_model->boxes;
    if (!list) {
        count = 0;
        box = 0;
    } else {
        count = list->count;
        box = list->boxes;
    }
    while (count > 0 && (box->flags & COLLBOX_NONSOLID)) {
        count--;
        box++;
    }
}

#define SDW_ABS(v) ((v) >= 0 ? (v) : -(v))

/* the action prompt */
extern s32 g_actionPromptTimer, g_actionPromptTimer2, g_actionPromptWidth, g_dt;

extern s32 g_gameTimeMs;
/* cast kept: time.cpp defines the clock s32; the prompt code reads it as u32 (unsigned arithmetic) */
#define g_gameTimeMs (*(u32 *)&g_gameTimeMs)

extern s32 g_frameCount2;
/* cast kept: time.cpp defines the counter s32; the prompt code reads it as u32 (unsigned arithmetic) */
#define g_frameCount2 (*(u32 *)&g_frameCount2)

extern u32 g_uiTintColor;
extern u16 g_itemPromptValue;
extern char g_emptyActionPrompt[1], g_emptyPromptStrings[1];
extern char g_emptyHeldPrompt[1], g_emptyViewPrompt[1], g_emptySneakPrompt[1];
extern const char *g_wolfCtxPromptText[41], *g_wolfHeldPromptText[25], *g_wolfCtx4PromptText[2],
    *g_wolfSneakPromptText[2];
extern u8 g_wolfCtxPromptStrIds[41], g_wolfHeldPromptStrIds[25], g_wolfCtx4PromptStrIds[2], g_wolfSneakPromptStrIds[2];
extern u32 *g_screenLayerBase;
/* g_pCosTable: declared above as const s16 * */
u16 Str_Length(const char *);
inline s32 PromptScreenHeight()
{
    return 240;
}
#define COS(a) g_pCosTable[(s16)((a) & 4095)]

/* ---- data, in address order ----
 * .data 0x57b218: this object's copy of the cinematic opcode stride table, a static table of a header every cinematic
 * user includes (one copy per object, src/engine/cine2.cpp); nothing here reads it. */
static u8 g_cineOpStrideCopy[9] = {0, 8, 8, 4, 2, 2, 4, 2, 2};
/* 0x57b224 / 0x57b250 / 0x57b26a / 0x57b26c - the UI string ids of the prompts, by context action, held action,
 * first-person view and sneak action (0 = no prompt); Wolf_InitActionPromptStrings resolves them */
u8 g_wolfCtxPromptStrIds[41] = {
    0,                /* CTX_NONE */
    UISTR_USE2,       /* CTX_USE */
    UISTR_CARRY,      /* CTX_LIFT */
    UISTR_ORDER,      /* CTX_ACTIVATE */
    UISTR_TAKE,       /* CTX_PICKUP */
    UISTR_PUSH,       /* CTX_PUSH */
    UISTR_STOP,       /* (no WolfContextAction 6) */
    UISTR_READ,       /* CTX_READ */
    UISTR_TALK,       /* CTX_TALK */
    UISTR_WATCH,      /* CTX_TELESCOPE */
    UISTR_ACTIVATE,   /* CTX_USE_0A */
    UISTR_DEACTIVATE, /* CTX_USE_0B */
    UISTR_DEFUSE,     /* CTX_DEFUSE */
    UISTR_HIDE,       /* CTX_BUSH */
    UISTR_GO_OUT,     /* CTX_BUSH_TAKE_OFF */
    UISTR_CLIMB,      /* CTX_CLIMB */
    UISTR_LET_GO,     /* CTX_CLIMBING */
    UISTR_GRIP,       /* CTX_CLIMBBOXWALK_ENTER */
    UISTR_LET_GO2,    /* CTX_CLIMBBOXWALK_LEAVE */
    UISTR_CLING_TO,   /* CTX_ELASTIC_TIE */
    UISTR_TAKE_UP,    /* CTX_ELASTIC_TAKE_BACK */
    UISTR_TAKE_UP,    /* CTX_ELASTIC_GRAB */
    UISTR_FIRE,       /* CTX_ELASTIC_PULL */
    UISTR_DROP,       /* CTX_ELASTIC_RELEASE */
    UISTR_BLEAT,      /* CTX_BLEAT */
    UISTR_TAKE,       /* CTX_WOLFTRAP */
    UISTR_TURN2,      /* CTX_SWIRLSIGN */
    UISTR_BOO,        /* CTX_BOO */
    UISTR_USE2,       /* CTX_CANNON */
    UISTR_TURN,       /* CTX_PIPE */
    UISTR_USE2,       /* CTX_TIMEKEEPER */
    UISTR_PUSH,       /* CTX_ICECUBE */
    UISTR_SQUIRM,     /* CTX_SQUIRM */
    UISTR_SAVE,       /* CTX_SAVE */
    UISTR_ACCEPT,     /* CTX_LEVELDOOR */
    UISTR_SET,        /* CTX_CATAPULT */
    UISTR_BONUS,      /* CTX_BONUS */
    0,                /* (no WolfContextAction 37) */
    0,                /* (no WolfContextAction 38) */
    0,                /* (no WolfContextAction 39) */
    0                 /* (no WolfContextAction 40) */
};
u8 g_wolfHeldPromptStrIds[25] = {
    0,                /* HELD_NONE */
    UISTR_USE2,       /* HELD_USE */
    UISTR_PUT,        /* HELD_THROWABLE */
    UISTR_ACTIVATE,   /* HELD_ROCKET */
    UISTR_DEACTIVATE, /* HELD_ROCKET_ACTIVE */
    UISTR_ACTIVATE,   /* HELD_FAN */
    UISTR_DEACTIVATE, /* HELD_FAN_ACTIVE */
    UISTR_ACTIVATE,   /* HELD_FLUTE */
    UISTR_DEACTIVATE, /* HELD_FLUTE_ACTIVE */
    UISTR_CLING_TO,   /* HELD_DROP_IN_PLACE */
    UISTR_ACTIVATE,   /* HELD_DROP_ACTIVE */
    UISTR_DEACTIVATE, /* (no WolfHeldAction 11) */
    UISTR_ACTIVATE,   /* HELD_MINEDETECTOR */
    UISTR_DEACTIVATE, /* HELD_DETECTOR_ACTIVE */
    UISTR_ACTIVATE,   /* HELD_UMBRELLA */
    UISTR_DEACTIVATE, /* HELD_UMBRELLA_OPEN */
    UISTR_ACTIVATE,   /* HELD_REMOTE */
    UISTR_DEACTIVATE, /* HELD_REMOTE_ACTIVE */
    UISTR_USE2,       /* HELD_REMOTE_ALT */
    UISTR_USE2,       /* HELD_TIMEMACHINE */
    UISTR_ACTIVATE,   /* HELD_FISHINGROD */
    UISTR_DEACTIVATE, /* HELD_FISHINGROD_ACTIVE */
    UISTR_BLOW_UP,    /* HELD_INFLATABLE_SHEEP */
    UISTR_OPEN,       /* HELD_KEY_USE */
    UISTR_PLANT       /* HELD_SEED_PLANT */
};
u8 g_wolfCtx4PromptStrIds[2] = {0, 56};
u8 g_wolfSneakPromptStrIds[2] = {0, 45};
u16 g_itemPromptValue = 0xffff; /* 0x57b26e */
/* .bss 0x6cf318-0x6cf445 */
const char *g_wolfCtx4PromptText[2] = {0};
const char *g_wolfSneakPromptText[2] = {0};
const char *g_wolfCtxPromptText[41] = {0};
const char *g_wolfHeldPromptText[25] = {0};
s32 g_actionPromptTimer = 0;
s32 g_actionPromptWidth = 0;
s32 g_actionPromptTimer2 = 0;
char g_emptyActionPrompt[1] = {0};
char g_emptyPromptStrings[1] = {0};
char g_emptyHeldPrompt[1] = {0};
char g_emptyViewPrompt[1] = {0};
char g_emptySneakPrompt[1] = {0};

/* BYTES(slot-scope, inferred): index lives in a nested block so it is allocated after the function's other locals */
void ScnControllable::NextIdleAnim(const IdleAnimEntry *base, const IdleAnimEntry *variants, u32 count)
{
    if (!idleLoopCount) {
        u16 anim = this->anim.animId;
        {
            s32 index;
            if (anim == base->animId && !g_cinePlayer.IsActive()) {
                index = Rand_Bounded(count);
                anim = variants[index].animId;
                idleLoopCount = (u8)(Rand_Range(variants[index].loopsMin, variants[index].loopsMax) - 1);
            } else {
                anim = base->animId;
                idleLoopCount = (u8)(Rand_Range(base->loopsMin, base->loopsMax) - 1);
            }
            PlayAnim(anim, 1, 1);
        }
    } else
        --idleLoopCount;
}
s32 ScnControllable::AddMoveModifier(ScnObject *object)
{
    s32 index = 0;
    while (index < riderCount && riders[index] != object)
        ++index;
    if (index == riderCount) {
        if (riderCount < 8)
            ++riderCount;
        else
            return 0;
    }
    riders[index] = object;
    return 1;
}
void ScnControllable::RemoveMoveModifier(ScnObject *object)
{
    s32 index = 0;
    while (index < riderCount && riders[index] != object)
        ++index;
    if (index < riderCount) {
        --riderCount;
        while (index < riderCount) {
            riders[index] = riders[index + 1];
            ++index;
        }
    }
}
/* BYTES(slot-group): locals grouped in Work only to pin the original frame offsets; pad fills a gap */
/* BYTES(view): three bitfield stores: the original changes only these three bits of the uninitialised stack word */
s32 ScnControllable::ApplyMoveModifiers(Vec3s *delta, Vec3s *velocityIn, s32 f1, s32 f2, s32 f4)
{
    struct Work {
        s32 index;
        MoveModifyArg arg;
        u16 pad;
    } w;
    w.arg.delta = *delta;
    /* The flags are split into bitfields. The original changes only
     * three bits of the uninitialized stack word; preserve all others. */
    if (f1)
        w.arg.flag0 = 1;
    else
        w.arg.flag0 = 0;
    if (f2)
        w.arg.noPull = 1;
    else
        w.arg.noPull = 0;
    if (f4)
        w.arg.flag2 = 1;
    else
        w.arg.flag2 = 0;
    w.arg.velocity = *velocityIn;
    w.index = 0;
    while (w.index < riderCount) {
        riders[w.index]->HandleMessage(this, MSG_MODIFY_MOVE, &w.arg);
        ++w.index;
    }
    *delta = w.arg.delta;
    return speed == 0 && ABS_VALUE(delta->x) + ABS_VALUE(delta->z) >= 2;
}
/* BYTES(slot-group): locals grouped in Work only to pin the original frame offsets */
void ScnControllable::ReadPad(Pad *pad, s32 enable, s32 sneak)
{
    struct Work {
        u32 bits;
        s32 y, magnitude, x;
    } w;
    w.x = w.y = 0;
    w.bits = 0;
    w.magnitude = 0;
    if (enable) {
        if (pad->cur.typeLen.type == PADTYPE_ANALOG) {
            w.magnitude = Pad_AnalogToStick(pad->cur.leftX, pad->cur.leftY, &w.x, &w.y);
            w.y = -w.y;
        }
        if (!w.magnitude) {
            if (DOWN(pad, 1))
                w.x = 256;
            else if (DOWN(pad, 3))
                w.x = -256;
            if (DOWN(pad, 0))
                w.y = 256;
            else if (DOWN(pad, 2))
                w.y = -256;
            w.magnitude = ABS_VALUE(w.x) + ABS_VALUE(w.y);
            if (w.magnitude > 256) {
                w.magnitude = 256;
                if (w.x > 0)
                    w.x = 181;
                else
                    w.x = -181;
                if (w.y > 0)
                    w.y = 181;
                else
                    w.y = -181;
            }
        }
        if (PRESSED(pad, 11))
            w.bits |= WOLF_ACT_JUMP_EDGE | WOLF_ACT_DOUBLE_JUMP;
        if (DOWN(pad, 11))
            w.bits |= WOLF_ACT_JUMP_HELD;
        if (PRESSED(pad, 9))
            w.bits |= WOLF_ACT_RUN;
        if (DOWN(pad, 9))
            w.bits |= WOLF_ACT_RUN_HELD;
        if (PRESSED(pad, 10))
            w.bits |= WOLF_ACT_ACTION_EDGE;
        if (DOWN(pad, 10))
            w.bits |= WOLF_ACT_ACTION_HELD;
    }
    if (sneak && DOWN(pad, 7))
        w.bits |= WOLF_ACT_SNEAK_HELD;
    padBits = w.bits;
    stickX = w.x;
    stickY = w.y;
    stickMag = w.magnitude;
}

/* 0x49246f - stick direction as a world heading: atan2 of the stick, minus the camera's heading, turned half a circle.
 * With the stick at rest it returns the fallback. */
s16 ScnControllable::GetStickHeading(s16 fallback)
{
    if (stickMag != 0)
        return (Math_RadiansToAngle4096((float)atan2((double)stickX, (double)stickY)) - g_camYaw + 2048) & 4095;
    return fallback;
}

/* 0x4924db - find what the actor can interact with. Collects up to 64 objects whose origins lie in the actor's world box
 * grown by max(boxDist + 500, pointRadius) in x/z, and keeps those whose class has registration flag 1, with object flags
 * 0x804 clear and instance flag 8 clear. A candidate with model boxes (and object flag 0x400 clear) is tested box by box:
 * vertical overlap, x/z box gap <= the primary gap threshold, and the heading to it within maxAngle of the actor's
 * facing (class flag 0x80 waives the angle). One without boxes is tested as a point: origin within yRange of the actor
 * box's bottom, squared x/z distance <= the primary point threshold, no angle test. Each accepted test asks the object
 * for its action with message 2 (primary, unless skipPrimary; skipped for boxes once a point has won) and message 0x26
 * (secondary, when given); a nonzero answer is stored with the object and tightens that output's threshold. Returns
 * primary->action != 0. `work` only pins the original stack offsets of the locals (VC6 /Od orders them its own way). */
/* BYTES(slot-group): locals grouped in work only to pin the original frame offsets */
/* BYTES(slot-scope): GetFirstModelBox 0x4c1ec0 written out in a block: its list local lands on the original's temp at ebp-0x13c (an inline would add a result temp) */
s32 ScnControllable::ScanInteractables(ActionHit *primary, ActionHit *secondary, s16 yRange, s16 boxDist,
                                       s16 pointRadius, s16 maxAngle, s32 skipPrimary)
{
    struct {
        s16 unused0, angle;
        ScnObject *obj;
        s16 unused1, yMax;
        s32 i;
        ScnObject *list[64];
        CollBox *box;
        s16 unused2, facing;
        InteractScan *scratch;
        s32 reply;
        s16 unused3, yMin;
        s32 dist;
        s32 pointHit;
        CollBox *ownBox;
        u32 boxCount;
        s32 n;
    } work;
    /* cast kept: g_sharedScratch is one scratch buffer that each user lays out its own way */
    work.scratch = (InteractScan *)g_sharedScratch;
    primary->target = 0;
    primary->action = 0;
    { /* the body of ScnObject_GetFirstModelBox 0x4c1ec0, written out: as an inline returning a value it adds a result
         * temp that the original does not have; a block-scope list lands on the original's temp at ebp-0x13c */
        ModelBoxList *list = inst_model->boxes;
        if (list)
            work.ownBox = list->boxes;
        else
            work.ownBox = 0;
    }
    if (work.ownBox == 0)
        return 0;
    work.scratch->selfPos = pos;
    work.scratch->selfBox.Box_Translate(work.ownBox, &work.scratch->selfPos);
    work.dist = boxDist + 500;
    if (pointRadius > work.dist)
        work.dist = pointRadius;
    work.scratch->primaryBoxGap = boxDist;
    work.scratch->primaryPointDist2 = pointRadius * pointRadius;
    work.scratch->secondaryBoxGap = work.scratch->primaryBoxGap;
    work.scratch->secondaryPointDist2 = work.scratch->primaryPointDist2;
    work.yMin = work.scratch->selfBox.max.y - yRange;
    work.yMax = work.scratch->selfBox.max.y + yRange;
    work.facing = rot.y;
    work.pointHit = 0;
    work.n = ObjGrid_QueryPointsInRectXZ(
        work.scratch->selfBox.min.x - work.dist, work.scratch->selfBox.min.z - work.dist,
        work.scratch->selfBox.max.x + work.dist, work.scratch->selfBox.max.z + work.dist, work.list);
    for (work.i = 0; work.i < work.n; work.i++) {
        work.obj = work.list[work.i];
        if ((Scenaric_ClassFlags(work.obj->GetClassId()) & SCN_CF_INTERACTABLE) &&
            work.obj->FlagsClear(SCN_OF_HIDDEN | SCN_OF_HIDDEN2) && !work.obj->InstFlags(INST_F_ATTACHED)) {
            if (work.obj->FlagsClear(SCN_OF_NO_BOX_COLLIDE)) {
                work.obj->GetSolidBoxes(work.boxCount, work.box);
            } else {
                work.box = 0;
                work.boxCount = 0;
            }
            work.scratch->candPos = work.obj->pos;
            if (work.boxCount > 0) {
                while (work.boxCount > 0) {
                    if (!(work.box->flags & COLLBOX_NONSOLID)) {
                        work.scratch->candBox.Box_Translate(work.box, &work.scratch->candPos);
                        if (work.scratch->selfBox.min.y <= work.scratch->candBox.max.y &&
                            work.scratch->selfBox.max.y >= work.scratch->candBox.min.y) {
                            work.dist = Box_GapXZ(&work.scratch->selfBox, &work.scratch->candBox);
                            if (work.dist <= work.scratch->primaryBoxGap) {
                                work.angle = Math_RadiansToAngle4096(
                                    (float)atan2((double)-(work.scratch->candPos.x - work.scratch->selfPos.x),
                                                 (double)-(work.scratch->candPos.z - work.scratch->selfPos.z)));
                                if (SDW_ABS((s16)((s16)((work.facing - work.angle + 2048) & 4095) - 2048)) <=
                                        maxAngle ||
                                    (Scenaric_ClassFlags(work.obj->GetClassId()) & SCN_CF_INTERACT_ANY_FACING)) {
                                    if (!work.pointHit && !skipPrimary) {
                                        work.reply = work.obj->HandleMessage(this, MSG_QUERY_ACTION, 0);
                                        if (work.reply) {
                                            primary->target = work.obj;
                                            primary->action = work.reply;
                                            work.scratch->primaryBoxGap = work.dist;
                                        }
                                    }
                                    if (secondary) {
                                        work.reply = work.obj->HandleMessage(this, MSG_QUERY_COVER, 0);
                                        if (work.reply) {
                                            secondary->target = work.obj;
                                            secondary->action = work.reply;
                                            work.scratch->secondaryBoxGap = work.dist;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    work.boxCount--;
                    work.box++;
                }
            } else if (work.scratch->candPos.y >= work.yMin && work.scratch->candPos.y <= work.yMax) {
                work.dist = Vec3s_DistSqXZ(&work.scratch->candPos, &work.scratch->selfPos);
                if (work.dist <= work.scratch->primaryPointDist2) {
                    if (!skipPrimary) {
                        work.reply = work.obj->HandleMessage(this, MSG_QUERY_ACTION, 0);
                        if (work.reply) {
                            primary->target = work.obj;
                            primary->action = work.reply;
                            work.scratch->primaryPointDist2 = work.dist;
                            work.pointHit = 1;
                        }
                    }
                    if (secondary) {
                        work.reply = work.obj->HandleMessage(this, MSG_QUERY_COVER, 0);
                        if (work.reply) {
                            secondary->target = work.obj;
                            secondary->action = work.reply;
                            work.scratch->secondaryPointDist2 = work.dist;
                        }
                    }
                }
            }
        }
    }
    return primary->action != 0;
}

/* 0x492af7 - planar velocity from speed and heading. The vertical component is NOT written. */
void Vec_FromPolar(Vec3s *out, s32 speed, s16 heading)
{
    out->x = (s16)((-speed * g_sinTable4096[heading]) / 4096);
    out->z = (s16)((-speed * g_pCosTable[heading]) / 4096);
}

/* 0x492b4b - the stick drives the actor: target speed = maxSpeed * stick deflection >> 8, scaled by the cosine of the
 * angle between the stick and the current motion (so turning sharply slows him); a stick more than
 * returnAngleThreshold away from the motion makes the function return 1. Speed is APPROACHED at the record's rates,
 * never clamped to the new maximum. Facing turns toward the stick unless suppressFacing. */
/* BYTES(slot-group): locals grouped in work only to pin the original frame offsets */
s32 ScnControllable::Mobile_Steer(Vec3s *outVel, Vec3s *rot, const MoveRecord *rec, s32 suppressFacing)
{
    struct {
        s32 targetSpeed, flagged;
        s16 unused, braking, delta, desired;
    } work;
    work.flagged = 0;
    work.targetSpeed = (rec->maxSpeed * stickMag) >> 8;
    work.desired = GetStickHeading(moveDir);
    if (!suppressFacing && stickMag != 0)
        rot->y = Math_ApproachAngle(rot->y, work.desired, &facingAngVel, rec->maxFacingTurnRate,
                                    rec->facingTurnAcceleration, rec->travelTurnAcceleration, 1);
    wantedDir = work.desired;
    bounceSpeed = 0;
    work.braking = rec->deceleration;
    if (speed == 0) {
        moveDirAngVel = 0;
    } else {
        work.delta = (s16)((work.desired - moveDir + 2048) & 4095) - 2048;
        if (SDW_ABS(work.delta) > rec->returnAngleThreshold)
            work.flagged = 1;
        work.targetSpeed = (g_pCosTable[(s16)(work.delta & 4095)] * work.targetSpeed) >> 12;
        if (work.targetSpeed < 0) { /* stick pointing backwards: brake along the current heading */
            work.targetSpeed = 0;
            work.desired = moveDir;
            work.braking = (s16)((rec->acceleration + rec->deceleration) / 2);
        }
        work.desired = Math_ApproachAngle(moveDir, work.desired, &moveDirAngVel, rec->maxTravelTurnRate,
                                          rec->travelTurnAcceleration, rec->travelTurnAcceleration, 1);
    }
    speed = (s16)Math_ApproachLinear(speed, work.targetSpeed, rec->maxSpeed, rec->acceleration, work.braking);
    Vec_FromPolar(outVel, speed, work.desired);
    outVel->y = 0;
    rot->x = 0;
    rot->z = 0;
    moveDir = work.desired;
    velocity = *outVel;
    return work.flagged;
}

/* 0x492d9f - run steering: full record speed while runFlag, else brake to 0; facing and motion turn together. */
/* BYTES(slot-group): locals grouped in work only to pin the original frame offsets */
void ScnControllable::SteerRun(Vec3s *outVel, Vec3s *rot, const MoveRecord *rec, s32 runFlag)
{
    struct {
        s32 targetSpeed;
        s16 unused, heading;
    } work;
    if (runFlag)
        work.targetSpeed = rec->maxSpeed;
    else
        work.targetSpeed = 0;
    work.heading = GetStickHeading(rot->y);
    wantedDir = work.heading;
    bounceSpeed = 0;
    speed = (s16)Math_ApproachLinear(speed, work.targetSpeed, rec->maxSpeed, rec->acceleration, rec->deceleration);
    work.heading = Math_ApproachAngle(rot->y, work.heading, &moveDirAngVel, rec->maxTravelTurnRate,
                                      rec->travelTurnAcceleration, rec->travelTurnAcceleration, 1);
    Vec_FromPolar(outVel, speed, work.heading);
    outVel->y = 0;
    moveDir = work.heading;
    velocity = *outVel;
    facingAngVel = moveDirAngVel;
    rot->y = work.heading;
    rot->x = 0;
    rot->z = 0;
}

/* 0x492ee2 - turn toward a given heading while braking to a stop. */
void ScnControllable::SteerToHeading(Vec3s *outVel, Vec3s *rot, const MoveRecord *rec, s16 heading)
{
    s32 targetSpeed = 0;
    rot->y = Math_ApproachAngle(rot->y, heading, &facingAngVel, rec->maxFacingTurnRate, rec->facingTurnAcceleration,
                                rec->travelTurnAcceleration, 1);
    rot->x = 0;
    rot->z = 0;
    if (speed == 0)
        moveDirAngVel = 0;
    else
        heading = Math_ApproachAngle(moveDir, heading, &moveDirAngVel, rec->maxTravelTurnRate,
                                     rec->travelTurnAcceleration, rec->travelTurnAcceleration, 1);
    wantedDir = heading;
    bounceSpeed = 0;
    speed = (s16)Math_ApproachLinear(speed, targetSpeed, rec->maxSpeed, rec->acceleration, rec->deceleration);
    Vec_FromPolar(outVel, speed, heading);
    outVel->y = 0;
    moveDir = heading;
    velocity = *outVel;
}

/* 0x49303c - gravity and slopes. While moving or on a slope: add slope pull (tuning[4] + tuning[5] * slopeTime, doubled
 * on request, capped at 8000), project the velocity onto the ground plane, clamp x and z to tuning[6]. Then gravity:
 * tuning[8] * airTime >> 12, but never less than 120 per call, with the vertical speed capped at tuning[7].
 * Returns 1 when on a slope and the new velocity points against the old one. */
/* BYTES(slot-group): locals grouped in work only to pin the original frame offsets; unused fills a gap */
s32 ScnControllable::Wolf_SurfaceVelocity(Vec3s *vel, s32 doubleSlopeRate, const s16 *surfaceTuning)
{
    struct {
        s32 projection, added;
        Vec3s old;
        s16 unused;
        s32 vertical, rate;
    } work;
    work.old = *vel;
    if ((vel->x | vel->z) != 0 || slopeTime > 0) {
        if (doubleSlopeRate)
            work.rate = surfaceTuning[5] * 2;
        else
            work.rate = surfaceTuning[5];
        work.added = surfaceTuning[4] + ((work.rate * slopeTime) >> 12);
        if (work.added > 8000)
            work.added = 8000;
        vel->y += (s16)work.added;
        work.projection = -(groundNormal.x * vel->x + groundNormal.y * vel->y + groundNormal.z * vel->z) >> 6;
        vel->x += (s16)((work.projection * groundNormal.x) >> 18);
        vel->y += (s16)((work.projection * groundNormal.y) >> 18);
        vel->z += (s16)((work.projection * groundNormal.z) >> 18);
        if (SDW_ABS(vel->x) > surfaceTuning[6]) {
            if (vel->x > 0)
                vel->x = surfaceTuning[6];
            else
                vel->x = -surfaceTuning[6];
        }
        if (SDW_ABS(vel->z) > surfaceTuning[6]) {
            if (vel->z > 0)
                vel->z = surfaceTuning[6];
            else
                vel->z = -surfaceTuning[6];
        }
    }
    work.vertical = (surfaceTuning[8] * airTime) >> 12;
    if (work.vertical < 120)
        work.vertical = 120;
    if (vel->y + work.vertical > surfaceTuning[7])
        vel->y = surfaceTuning[7];
    else
        vel->y += (s16)work.vertical;
    return slopeTime > 0 && vel->x * work.old.x + vel->y * work.old.y + vel->z * work.old.z <= 0;
}

/* 0x4932ee - move by delta with collision. A delta within the limits is resolved in one call; a bigger one in two halves
 * (the second starting where the first ended). Any floor contact (flags & 5) resets airTime and takes the floor normal;
 * the contact effects of the first half stay, but only the second half's flags are returned. Finally: flags & 2 with
 * zero resolved displacement also resets a positive airTime. */
/* BYTES(slot-group): locals grouped in work only to pin the original frame offsets; unused fills a gap */
u32 ScnControllable::Wolf_MoveResolve(Vec3s *delta, ContactInfo *info, u16 normalCutoff, u16 mask, s32 horizLimit,
                                      s32 vertLimit, s16 stepAllowance)
{
    struct {
        Vec3s second;
        s16 unused;
        u32 flags;
    } work;
    if (SDW_ABS(delta->x) <= horizLimit && SDW_ABS(delta->z) <= horizLimit && SDW_ABS(delta->y) <= vertLimit) {
        work.flags = Collide_ResolveMove(delta, info, normalCutoff, mask, 0, 0, stepAllowance, 0, 0);
        if (work.flags & (COLL_FLOOR | COLL_FLOOR_EDGE)) {
            airTime = 0;
            groundNormal = info->floorNormal;
        }
    } else {
        work.second.x = delta->x >> 1;
        work.second.y = delta->y >> 1;
        work.second.z = delta->z >> 1;
        delta->x -= work.second.x;
        delta->y -= work.second.y;
        delta->z -= work.second.z;
        work.flags = Collide_ResolveMove(delta, info, normalCutoff, mask, 0, 0, stepAllowance, 0, 0);
        if (work.flags & (COLL_FLOOR | COLL_FLOOR_EDGE)) {
            airTime = 0;
            groundNormal = info->floorNormal;
        }
        delta->x += pos.x; /* delta becomes the first half's end point... */
        delta->y += pos.y;
        delta->z += pos.z;
        work.flags = Collide_ResolveMove(&work.second, info, normalCutoff, mask, delta, 0, stepAllowance, 0, 0);
        if (work.flags & (COLL_FLOOR | COLL_FLOOR_EDGE)) {
            airTime = 0;
            groundNormal = info->floorNormal;
        }
        delta->x = delta->x + work.second.x - pos.x; /* ...and then the total displacement again */
        delta->y = delta->y + work.second.y - pos.y;
        delta->z = delta->z + work.second.z - pos.z;
    }
    Translate(delta);
    UpdateShadow();
    if ((work.flags & COLL_WALL) && airTime > 0 && (delta->x | delta->y | delta->z) == 0)
        airTime = 0;
    return work.flags;
}

/* 0x4935f7 - stop dead: speed, both turn rates, the bounce and the velocity are zeroed, and both the motion heading and the
 * wanted heading are set to the facing. The facing goes through a local (ebp-2) in the original; it could as well be an
 * inlined setter's argument. */
/* BYTES(temp): the facing goes through heading because the original has it in a local (ebp-2) */
void ScnControllable::StopMotion()
{
    s16 heading;
    speed = 0;
    moveDirAngVel = 0;
    facingAngVel = 0;
    heading = rot.y;
    moveDir = heading;
    wantedDir = moveDir;
    bounceSpeed = 0;
    velocity.x = 0;
    velocity.y = 0;
    velocity.z = 0;
    bounceNormal.x = 0;
    bounceNormal.y = 0;
    bounceNormal.z = 0;
}

/* PAL PC action-prompt text, icon and panel animation. */

/* BYTES(slot-group): locals grouped in Work only to pin the original frame offsets */
void Wolf::DrawActionPrompt(ActionHit *primary, ActionHit *secondary, s32 show)
{
    struct Work {
        char text[64];
        s32 height, phase, width, view;
        const char *label;
        s32 dy, dx, sneak, draw, remaining;
    } w;
    w.view = 0;
    w.sneak = 0;
    w.label = 0;
    if (show) {
        if (primary->action)
            w.label = g_wolfCtxPromptText[primary->action];
        /* cast kept (the four (s32 *) reads): the caller passes its four-word context block through ActionHit *;
         * words two and three are the held-action and first-person-view indices. */
        else if (((s32 *)primary)[2])
            w.label = g_wolfHeldPromptText[((s32 *)primary)[2]];
        /* cast kept: reads the prompt index word of the primary action record */
        else if (((s32 *)primary)[3]) {
            w.label = g_wolfCtx4PromptText[((s32 *)primary)[3]];
            w.view = 1;
        } else if (secondary && secondary->action) {
            w.label = g_wolfSneakPromptText[secondary->action];
            w.sneak = 1;
        }
    }
    w.draw = 1;
    if (w.label) {
        if (w.view)
            Str_Concat2(w.text, "$B_INTVIEW$ ", w.label);
        else if (w.sneak)
            Str_Concat2(w.text, "$B_SNEAK$ ", w.label);
        else
            Str_Concat2(w.text, "$B_ACTION$ ", w.label);
        Text_SetFont(FONT_GAME);
        if (!actionPromptText[0] || g_gameTimeMs > actionPromptTimeMs + 500) {
            Str_Copy(actionPromptText, w.text);
            actionPromptTimeMs = g_gameTimeMs;
            g_actionPromptWidth = (Str_Length(w.label) + 2) * g_pCurFont->glyphWidth;
            if (g_actionPromptWidth > 450)
                g_actionPromptWidth = 450;
        }
        Text_SetWindow(g_screenLayerBase + 8, 32, 24, g_actionPromptWidth + 20, 40, 1);
        Text_PrintFmt(actionPromptText);
        Hud_EndBox_stub();
        g_actionPromptTimer += g_dt;
    } else {
        if (g_actionPromptTimer > 0) {
            if (g_actionPromptTimer > 2048)
                g_actionPromptTimer = 2048;
            g_actionPromptTimer -= g_dt;
        } else {
            g_actionPromptTimer = 0;
            w.draw = 0;
        }
        if (!actionPromptText[0] || g_gameTimeMs > actionPromptTimeMs + 500) {
            Str_Copy(actionPromptText, g_emptyActionPrompt);
            actionPromptTimeMs = g_gameTimeMs;
        }
        if (actionPromptText[0]) {
            Text_SetFont(FONT_GAME);
            Text_SetWindow(g_screenLayerBase + 8, 32, 24, g_actionPromptWidth + 20, 40, 1);
            Text_PrintFmt(actionPromptText);
            Hud_EndBox_stub();
        }
    }
    if (w.draw) {
        w.width = g_actionPromptWidth + 32;
        w.height = 31;
        if (g_actionPromptTimer < 2048) {
            w.phase = g_actionPromptTimer >> 6;
            w.phase *= w.phase;
            w.remaining = 2048 - g_actionPromptTimer;
            w.dx = (-w.remaining * COS((((w.phase << 12) * 22 * 2) / 7) >> 12) * 24) / 8388608;
            w.dy = (-w.remaining * COS(((w.phase << 12) * 2 * 2) >> 12) * 12) / 8388608;
        } else
            w.dx = w.dy = 0;
        g_spriteCrayon2.Draw(g_screenLayerBase + 9, 16 - w.dx, 16 - w.dy, w.width + w.dx + 16, w.height + w.dy + 16,
                             g_uiTintColor, 0);
    }
}
/* BYTES(slot-group): locals grouped in Work only to pin the original frame offsets */
void Wolf::Hud_DrawItemPrompt(u16 *prompt, s32 show)
{
    struct Work {
        s32 phase, bottom, top, right, draw, dy, left, dx;
        u32 color;
        s32 remaining;
    } w;
    w.draw = 0;
    if (show) {
        w.draw = 1;
        g_itemPromptValue = prompt[1];
        g_actionPromptTimer2 += g_dt;
    } else {
        if (g_actionPromptTimer2 > 0) {
            if (g_actionPromptTimer2 > 2048)
                g_actionPromptTimer2 = 2048;
            w.draw = 1;
            g_actionPromptTimer2 -= g_dt;
        } else
            g_actionPromptTimer2 = 0;
    }
    if (w.draw) {
        Scenaric_DrawClassIcon(g_screenLayerBase + 8, prompt[0], g_animSpriteCrayon1.width / 2 + 18,
                               PromptScreenHeight() - 14 - g_animSpriteCrayon1.height + g_animSpriteCrayon1.height / 2,
                               0, 0, 0x808080);
        if (g_actionPromptTimer2 < 2048) {
            w.phase = g_actionPromptTimer2 >> 6;
            w.phase *= w.phase;
            w.remaining = 2048 - g_actionPromptTimer2;
            w.dx = (-w.remaining * COS((((w.phase << 12) * 22 * 2) / 7) >> 12) * 24) / 8388608;
            w.dy = (-w.remaining * COS(((w.phase << 12) * 2 * 2) >> 12) * 12) / 8388608;
        } else
            w.dx = w.dy = 0;
        w.left = 18 - w.dx;
        w.top = PromptScreenHeight() - 14 - g_animSpriteCrayon1.height - w.dy;
        w.right = g_animSpriteCrayon1.width + w.dx + 18;
        w.bottom = (PromptScreenHeight() - 14 - g_animSpriteCrayon1.height) + (w.dy + g_animSpriteCrayon1.height);
        if (g_itemPromptValue >= 0xe665)
            w.color = 0x101050;
        else
            w.color = 0x502828;
        g_animSpriteCrayon1.Draw(g_screenLayerBase + 9, w.left, w.top, w.right, w.bottom, w.color,
                                 (g_frameCount2 >> 2) & 255, 0);
    }
}
void ScnControllable::Hud_ResetActionPrompt()
{
    g_actionPromptTimer = 0;
    g_actionPromptWidth = 0;
    g_actionPromptTimer2 = 0;
    g_itemPromptValue = 0xffff;
    actionPromptText[0] = 0;
    actionPromptTimeMs = g_gameTimeMs;
}
void Wolf::InitActionPromptStrings()
{
    s32 index;
    for (index = 0; index < 41; ++index) {
        if (g_wolfCtxPromptStrIds[index])
            g_wolfCtxPromptText[index] = Text_GetUiString(g_wolfCtxPromptStrIds[index]);
        else
            g_wolfCtxPromptText[index] = g_emptyPromptStrings;
    }
    for (index = 0; index < 25; ++index) {
        if (g_wolfHeldPromptStrIds[index])
            g_wolfHeldPromptText[index] = Text_GetUiString(g_wolfHeldPromptStrIds[index]);
        else
            g_wolfHeldPromptText[index] = g_emptyHeldPrompt;
    }
    for (index = 0; index < 2; ++index) {
        if (g_wolfCtx4PromptStrIds[index])
            g_wolfCtx4PromptText[index] = Text_GetUiString(g_wolfCtx4PromptStrIds[index]);
        else
            g_wolfCtx4PromptText[index] = g_emptyViewPrompt;
    }
    for (index = 0; index < 2; ++index) {
        if (g_wolfSneakPromptStrIds[index])
            g_wolfSneakPromptText[index] = Text_GetUiString(g_wolfSneakPromptStrIds[index]);
        else
            g_wolfSneakPromptText[index] = g_emptySneakPrompt;
    }
}
