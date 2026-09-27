/* match-init: Sheep_Init_BubbleEmitter Sheep_Init_Splash Sheep_Init_IceBlock */
/* match-addr: g_sheepSplashSynthRec=0x6cef58 g_pFrozenRiverInst=0x6cefd4 g_flockInOrangeBits=0x6ceff0 g_sheepIceCubeBody=0x6cf000 g_beachOrangeZoneList=0x6cf064 g_flockReserveTargets=0x6cf080 g_flockIncomplete=0x6cf094 g_sheepBubbleEmit=0x6cf098   (renamed for the .bss hash order, see below) */
/*
 * T092 - the original object Sheep.cpp (guessed name; "Character_Sheep" in its warning string 0x57ac70), one file.
 * .text 0x476e50-0x47d30e (the three shared-effect initialisers, then Sheep_Move .. Sheep_Create),
 * .rdata 0x575074-0x5750a0 (sheepCarryOffset, then the Sheep vtable COMDAT that Sheep_Create emits),
 * .data 0x57ab38-0x57acd8, .bss 0x6cef58-0x6cf280; static initialisers .CRT$XCU 0x579020-0x579028.
 * The shared effects, movement, availability tests, the flock, targeting, decisions, states, motion, the update, the
 * message handler, the reset and initialisation, in address order.
 *
 * .bss. VC6 emits a file's uninitialised globals (constructed ones included) in the order of a hash of their names
 * (h = (h<<2) + (h>>4) + c over the name, then (h ^ h>>16) & 1023, ascending; last declared first within a bucket),
 * then the explicitly zero-initialised ones in definition order. Here the three constructed shared-effect globals
 * (the splash and ice-block bodies, the bubble emitter) are interleaved with plain ones up to 0x6cf1fc, so those
 * thirteen must hash in address order; with the descriptive names only five do (g_sheepSplash 174,
 * g_flockScentSources 563, g_flockScentSeenTime 663, g_flockScentCount 784, g_sheepIceBlockRecord 888). The others
 * carry names chosen for the order (the descriptive name in brackets):
 *   g_sheepSplashSynthRec (85) [g_sheepSplashRecord]        g_pFrozenRiverInst (536) [g_pFrozenRiver]
 *   g_flockInOrangeBits (600) [g_flockInZoneMask]           g_sheepIceCubeBody (824) [g_sheepIceBlock]
 *   g_beachOrangeZoneList (858) [g_beachOrangeBoxes]        g_flockReserveTargets (954) [g_flockReservedTargets]
 *   g_flockIncomplete (971) [g_flockSheepMissing]           g_sheepBubbleEmit (1017) [g_sheepBubbleEmitter]
 * The globals after the bubble emitter are `= 0` and so keep their definition (= address) order.
 * g_beachOrangeZoneList, g_samOrangeBoxes and g_sheepRepelBoxes are 8-byte zone lists {boxes, count} (ZoneList).
 * .data: the bubble parameters, g_sheepAltModelIds (in .data, so not const), the state and idle tables, then the
 * warning literal of Flock_GetObjNearestAvailableSheep_Vision.
 */
/* BYTES: bss-name, layout, slot-group. */
/* BYTES(bss-name): named for its .bss hash key 85 */
/* BYTES(bss-name): named for its .bss hash key 536 */
/* BYTES(bss-name): named for its .bss hash key 600 */
/* BYTES(bss-name): named for its .bss hash key 824 */
/* BYTES(bss-name): named for its .bss hash key 858 */
/* BYTES(bss-name): named for its .bss hash key 954 */
/* BYTES(bss-name): named for its .bss hash key 971 */
/* BYTES(bss-name): named for its .bss hash key 1017 */
/* BYTES(layout): written '= 0' only to keep definition order in .bss */
/* BYTES(layout): not const: the original has it in .data */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *, Animator *, u16, u32);
#include "animation.h"
#include "../engine/sound_mgr.h"
#include "../engine/fade.h"
#include "../engine/scenaric.h"
#include "../engine/progress.h"
#include "../engine/time.h"
#include "../engine/scn_tools.h"
#include "../engine/approach.h"
#include "../engine/maths.h"
#include "../engine/id_list.h"
u16 Sound_Play(u16, void *, u16, u8, s32);
#define SDW_INLINE_FREE_INSTFLAGS_SET_U16_U16 1
#include "instance_inlines.h"
#undef SDW_INLINE_FREE_INSTFLAGS_SET_U16_U16
#define SDW_INLINE_FREE_INSTFLAGS_CLEAR_U16_U16 1
#include "instance_inlines.h"
#undef SDW_INLINE_FREE_INSTFLAGS_CLEAR_U16_U16
/* Member declarations: the Sheep's SDW_MEMBERS_ lists plus the SDW_EXTRA_ additions. */

#define SDW_EXTRA_ScnObject                                     \
    void AttachTo(ScnObject *, u8, Vec3s *, Vec3s *, u32, u32); \
    Vec3s &Position()                                           \
    {                                                           \
        return pos;                                             \
    }                                                           \
    Vec3s &Rotation()                                           \
    {                                                           \
        return rot;                                             \
    }                                                           \
    s32 IsActiveInWorld()                                       \
    {                                                           \
        return IsVisible() && IsInWorld();                      \
    }                                                           \
    s16 Heading()                                               \
    {                                                           \
        return rot.y;                                           \
    }                                                           \
    void EnableFlag2(s32 value)                                 \
    {                                                           \
        if (value)                                              \
            flags &= (u16)~SCN_OF_MUTE_ANIM_SOUND;              \
        else                                                    \
            flags |= SCN_OF_MUTE_ANIM_SOUND;                    \
    }                                                           \
    void SetUpdateMode(u8);                                     \
    void SetTint(u32 color, s16 amount, s32 on);                \
    static void *operator new(u32);                             \
    void EnableStaticFlag(s32 on)                               \
    {                                                           \
        if (on)                                                 \
            flags |= SCN_OF_NO_DIST_CULL;                       \
        else                                                    \
            flags &= (u16)~SCN_OF_NO_DIST_CULL;                 \
    }                                                           \
    CollBox *FirstModelBox();
#define SDW_EXTRA_ScnBody                                             \
    void PlayAnimSelection(const u16 &id, const s32 &loop, s32 blend) \
    {                                                                 \
        u32 options = 0;                                              \
        if (loop)                                                     \
            options |= ANIM_SET_LOOP;                                 \
        if (blend)                                                    \
            options |= ANIM_SET_BLEND;                                \
        Anim_Start(Inst(), &anim, id, options);                       \
    }

#define SDW_MEMBERS_InlineEmitter16 InlineEmitter16();
#define SDW_MEMBERS_WallAvoid void ClearActive();


#define SDW_MEMBERS_AltModel          \
    s32 Present()                     \
    {                                 \
        return modelResIdx != 0xffff; \
    }

#define SDW_EXTRA_Sheep                                                                         \
    void StopSound(u16 handle)                                                                  \
    {                                                                                           \
        Sound_Stop(handle, this);                                                               \
    }                                                                                           \
    u16 PlayBleat(s32 pitch)                                                                    \
    {                                                                                           \
        return Sound_Play(SND_SMOBLONE, this, 255, SNDF_POSITIONAL | SNDF_NO_RETRIGGER, pitch); \
    }                                                                                           \
    s32 SoundPlaying(u16 handle)                                                                \
    {                                                                                           \
        return Sound_IsPlaying(handle);                                                         \
    }                                                                                           \
    s32 OwnsSharedFx();                                                                         \
    s32 ThinkThisFrame();                                                                       \
    s32 AtHome();                                                                               \
    s32 IsOutsideCandidate();
/* ---- the Sheep's members ---- */
#define SDW_MEMBERS_ScnObject                                           \
    u32 HasInstanceFlag(u16 mask)                                       \
    {                                                                   \
        return inst_flags & mask;                                       \
    }                                                                   \
    ScnObject *Scenaric_FindBestInRadius(Vec3s *, s16, s16, u16, u16 *, \
                                         u32 (*)(ScnObject *, ScnObject *, u32, ScnObject *), s32);


#define SDW_MEMBERS_Sheep                \
    ScnObject *ReservedTarget()          \
    {                                    \
        if (flags & SHF_TARGET_RESERVED) \
            return pTarget;              \
        return 0;                        \
    }                                    \
    s32 RestrictedByMissingSheep();      \
    void ClearFlags(u32 mask)            \
    {                                    \
        flags &= ~mask;                  \
    }
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_ISVISIBLE 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_ISVISIBLE
#define SDW_INLINE_SCNOBJECT_ISINWORLD 1
#define SDW_INLINE_SCNOBJECT_SETHEADING_S16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_ISINWORLD
#undef SDW_INLINE_SCNOBJECT_SETHEADING_S16
#define SDW_INLINE_SCNOBJECT_ENABLEBOXCOLLIDE_S32 1
#define SDW_INLINE_SCNOBJECT_SETTINT_U32_S16_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_ENABLEBOXCOLLIDE_S32
#undef SDW_INLINE_SCNOBJECT_SETTINT_U32_S16_S32
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_SCNBODY_CURRENTANIM 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_CURRENTANIM
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#define SDW_INLINE_SHADOW_SETVISIBLE_S32 1
#include "../engine/shadow_inlines.h"
#undef SDW_INLINE_SHADOW_SETVISIBLE_S32
#define SDW_INLINE_PROGRESS_CURRENTLEVEL 1
#include "../engine/progress_inlines.h"
#undef SDW_INLINE_PROGRESS_CURRENTLEVEL
#define SDW_INLINE_SCNMOBILE_SETSHADOWRADIUS_U8 1
#include "../engine/scn_mobile_inlines.h"
#undef SDW_INLINE_SCNMOBILE_SETSHADOWRADIUS_U8
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#define SDW_INLINE_SCNMOBILE_GROUNDY 1
#include "../engine/scn_mobile_inlines.h"
#undef SDW_INLINE_SCNMOBILE_GROUNDY
FlockScentSource *Flock_FindScentSource(ScnObject *);
s32 Flock_ReserveTarget(ScnObject *);
void Flock_UnreserveTarget(ScnObject *);
s32 Flock_IsTargetReserved(ScnObject *);
Sheep *Flock_GetObjNearestAvailableSheep_Vision(ScnObject *, u32);
Sheep *Flock_GetObjNearestAvailableSheep_Scent(ScnObject *);
u32 Flock_NearestAvailableSheepCB(ScnObject *, ScnObject *, u32, ScnObject *); /* 0x477ac4, defined after its user */

/* ---- other declarations ---- */
/* Resource records, not object layouts. Six-byte states and four-byte idle rows. */
struct SheepStateEntry {
    u8 anim, speedLow, speedHigh, model;
    u16 flags;
};
struct SheepIdleEntry {
    u16 anim;
    u8 low, high;
};
extern Wolf *g_pWolf;
extern u32 g_gameTime;
extern u8 g_sharedScratch[];
extern s32 g_dt, g_dtMs;
extern "C" s16 g_sinTable4096[5122];
extern "C" const s16 *g_pCosTable;
#include "../sdk/crt.h"
extern "C" s16 Math_RadiansToAngle4096(float);
#include "../sdk/win32.h"
s32 Vec3s_DistSqXZ(Vec3s *, Vec3s *);
s32 Rand_Bounded(s32);
s32 Scenaric_FindByClass(u16, ScnObject **, s32);
void Flock_ResetGlobals();
#define ABS_VALUE(x) ((x) >= 0 ? (x) : -(x))
#define WRAP_ANGLE(x) ((s16)((s16)(((x) + 2048) & 4095) - 2048))
#define ADD_VEC(dst, src)   \
    {                       \
        (dst).x += (src).x; \
        (dst).y += (src).y; \
        (dst).z += (src).z; \
    }

/* ---- data ---- */
/* .rdata 0x575074. Original read-only vector at 0x575074; resolved by its contents. */
static const Vec3s sheepCarryOffset = {0, 7, 0};
/* .data 0x57ab38 - the bubbles the shared emitter blows while a sheep is under water */
EmitterRiseParams g_sheepBubbleParams = {-120, 8192, 1024, 10, 0, 2};
/* 0x57ab4c - the costume model sets (Sheep_Create) */
u16 g_sheepAltModelIds[2] = {82, 228};
/* 0x57ab50 - per state: animation, animation speed range (x/16), model set, flags */
SheepStateEntry g_sheepStateTable[46] = {
    {AMOUTO01_ANIM_STAND1, 12, 20, 0, SHSF_ANIM_ARG},
    {AMOUTO01_ANIM_SMEL1, 12, 20, 1, SHSF_ANIM_ARG},
    {AMOUTO01_ANIM_STAND2, 16, 16, 0,
     SHSF_NO_BOX_COLLIDE | SHSF_ALWAYS_UPDATE | SHSF_ANIM_ARG | SHSF_HIDE_SHADOW | SHSF_BLEAT},
    {AMOUTO01_ANIM_CARRY1B, 16, 16, 0, SHSF_NO_BOX_COLLIDE | SHSF_ALWAYS_UPDATE | SHSF_HIDE_SHADOW | SHSF_BLEAT},
    {AMOUTO01_ANIM_STAND2, 12, 20, 0, SHSF_ALWAYS_UPDATE | SHSF_ANIM_ARG},
    {AMOUTO01_ANIM_WALK1, 12, 20, 0, SHSF_ALWAYS_UPDATE | SHSF_ANIM_ARG | SHSF_BLEAT},
    {AMOUTO01_ANIM_STAND2, 16, 20, 0, SHSF_ALWAYS_UPDATE | SHSF_ANIM_ARG | SHSF_BLEAT},
    {AMOUTO01_ANIM_WALK3, 12, 20, 0, SHSF_KEEPS_RESERVATION | SHSF_ALWAYS_UPDATE | SHSF_ANIM_ARG},
    {AMOUTO01_ANIM_WALK1, 12, 20, 0, SHSF_ALWAYS_UPDATE | SHSF_ANIM_ARG | SHSF_BLEAT},
    {AMOUTO01_ANIM_SMEL1, 16, 20, 0, SHSF_ALWAYS_UPDATE | SHSF_ANIM_ARG},
    {AMOUTO01_ANIM_WALK2, 12, 20, 0, SHSF_KEEPS_RESERVATION | SHSF_ALWAYS_UPDATE | SHSF_ANIM_ARG},
    {AMOUTO01_ANIM_STAND1, 8, 24, 0, SHSF_KEEPS_RESERVATION | SHSF_ALWAYS_UPDATE | SHSF_ANIM_ARG},
    {AMOUTO01_ANIM_STAND2, 16, 20, 0, SHSF_KEEPS_RESERVATION | SHSF_ALWAYS_UPDATE | SHSF_ANIM_ARG},
    {AMOUTO01_ANIM_STAND1, 8, 24, 0, SHSF_KEEPS_RESERVATION | SHSF_ALWAYS_UPDATE | SHSF_ANIM_ARG},
    {AMOUTO01_ANIM_WALK3, 12, 20, 0, SHSF_ALWAYS_UPDATE | SHSF_ANIM_ARG},
    {AMOUTO01_ANIM_WALK1, 8, 24, 0,
     SHSF_KEEPS_RESERVATION | SHSF_ALWAYS_UPDATE | SHSF_ANIM_ARG | SHSF_BLEAT | SHSF_ON_BUTTON},
    {AMOUTO01_ANIM_STAND2, 12, 20, 0, SHSF_KEEPS_RESERVATION | SHSF_ALWAYS_UPDATE | SHSF_ANIM_ARG | SHSF_ON_BUTTON},
    {AMOUTO01_ANIM_WALK1, 8, 24, 0,
     SHSF_KEEPS_RESERVATION | SHSF_ALWAYS_UPDATE | SHSF_ANIM_ARG | SHSF_BLEAT | SHSF_ON_BUTTON},
    {AMOUTO01_ANIM_WALK3, 8, 24, 0, SHSF_ALWAYS_UPDATE | SHSF_ANIM_ARG | SHSF_BLEAT},
    {AMOUTO01_ANIM_WALK3, 12, 20, 0, SHSF_ALWAYS_UPDATE | SHSF_ANIM_ARG},
    {AMOUTO01_ANIM_DEAD2, 12, 20, 0, SHSF_ALWAYS_UPDATE | SHSF_DYING},
    {AMOUTO01_ANIM_SQUASH, 12, 20, 0, SHSF_NO_BOX_COLLIDE | SHSF_ALWAYS_UPDATE | SHSF_HIDE_SHADOW | SHSF_DYING},
    {AMOUTO01_ANIM_STAND2, 12, 20, 0, SHSF_ALWAYS_UPDATE | SHSF_ANIM_ARG | SHSF_DYING},
    {AMOUTO01_ANIM_STAND2, 12, 20, 0,
     SHSF_NO_BOX_COLLIDE | SHSF_ALWAYS_UPDATE | SHSF_ANIM_ARG | SHSF_HIDE_SHADOW | SHSF_DYING},
    {AMOUTO01_ANIM_WALK3, 12, 20, 0, SHSF_ALWAYS_UPDATE | SHSF_ANIM_ARG},
    {AMOUTO01_ANIM_WALK3, 12, 20, 0, SHSF_ALWAYS_UPDATE | SHSF_ANIM_ARG},
    {AMOUTO01_ANIM_STAND1, 12, 20, 0, SHSF_ANIM_ARG},
    {AMOUTO01_ANIM_WALK1, 12, 20, 0, SHSF_ANIM_ARG},
    {AMOUTO01_ANIM_WALK3, 12, 20, 0, SHSF_ALWAYS_UPDATE | SHSF_ANIM_ARG},
    {AMOUTO01_ANIM_WALK1, 16, 16, 0, SHSF_KEEPS_RESERVATION | SHSF_ALWAYS_UPDATE | SHSF_FOLLOWING | SHSF_BLEAT},
    {AMOUTO01_ANIM_STAND2, 12, 20, 0, SHSF_KEEPS_RESERVATION | SHSF_ALWAYS_UPDATE | SHSF_ANIM_ARG | SHSF_FOLLOWING},
    {AMOUTO01_ANIM_WALK2, 12, 20, 0, SHSF_KEEPS_RESERVATION | SHSF_ALWAYS_UPDATE | SHSF_ANIM_ARG | SHSF_FOLLOWING},
    {AMOUTO01_ANIM_STAND1, 12, 20, 0, SHSF_ANIM_ARG | SHSF_BLEAT},
    {AMOUTO01_ANIM_STAND2, 12, 20, 0, SHSF_ALWAYS_UPDATE | SHSF_IN_WATER | SHSF_DYING},
    {AMOUTO01_ANIM_FREEZE, 16, 16, 0, SHSF_ALWAYS_UPDATE | SHSF_IN_WATER | SHSF_FROZEN | SHSF_HIDE_SHADOW},
    {AMOUTO01_ANIM_FREEZE, 16, 16, 0, SHSF_ALWAYS_UPDATE | SHSF_IN_WATER | SHSF_FROZEN | SHSF_HIDE_SHADOW},
    {AMOUTO01_ANIM_FREEZE, 16, 16, 0, SHSF_ALWAYS_UPDATE | SHSF_IN_WATER | SHSF_FROZEN | SHSF_HIDE_SHADOW},
    {AMOUTO01_ANIM_FREEZE, 16, 16, 0, SHSF_ALWAYS_UPDATE | SHSF_IN_WATER | SHSF_FROZEN | SHSF_HIDE_SHADOW},
    {AMOUTO01_ANIM_FREEZE, 16, 16, 0, SHSF_ALWAYS_UPDATE | SHSF_FROZEN | SHSF_HIDE_SHADOW},
    {AMOUTO01_ANIM_COLD, 16, 16, 0, SHSF_ALWAYS_UPDATE | SHSF_ANIM_ARG | SHSF_FROZEN | SHSF_HIDE_SHADOW},
    {AMOUTO01_ANIM_TORNA4, 16, 16, 0, SHSF_ALWAYS_UPDATE | SHSF_HIDE_SHADOW},
    {AMOUTO01_ANIM_TORNA5, 16, 16, 0, SHSF_ALWAYS_UPDATE | SHSF_HIDE_SHADOW},
    {AMOUTO01_ANIM_WHIRL1A, 16, 16, 0,
     SHSF_ALWAYS_UPDATE | SHSF_IN_WATER | SHSF_ANIM_ARG | SHSF_FROZEN | SHSF_HIDE_SHADOW},
    {AMOUTO01_ANIM_PUT, 16, 16, 0,
     SHSF_NO_BOX_COLLIDE | SHSF_ALWAYS_UPDATE | SHSF_HIDE_SHADOW | SHSF_NOT_GRABBABLE | SHSF_BLEAT},
    {AMOUTO01_ANIM_WALK2, 16, 16, 0, SHSF_ALWAYS_UPDATE | SHSF_BLEAT},
    {AMOUTO01_ANIM_WALK3, 12, 20, 0, SHSF_ALWAYS_UPDATE | SHSF_ANIM_ARG}};
/* 0x57ac64 - the idle variants (animation, loop-count range) */
SheepIdleEntry g_sheepIdleVariants[3] = {{AMOUTO01_ANIM_STAND4, 1, 3}, {AMOUTO01_ANIM_BAH, 1, 2}, {0, 0, 0}};

/* .bss, the hashed part (see the header; bucket in brackets) */
ScnRecordSynth g_sheepSplashSynthRec;    /* 0x6cef58 [85]  record synthesised from export 0x5a */
ScnObject *g_pFrozenRiverInst;           /* 0x6cefd4 [536] */
FlockScentSource g_flockScentSources[2]; /* 0x6cefd8 [563] */
u32 g_flockInOrangeBits;                 /* 0x6ceff0 [600] bit per sheep: inside the orange zones */
u32 g_flockScentSeenTime;                /* 0x6ceff4 [663] */
u8 g_flockScentCount;                    /* 0x6ceff8 [784] */
ZoneList g_beachOrangeZoneList;          /* 0x6cf064 [858] BOXSAMORANGE-like beach zones (id list 0xc3) */
ScnRecordSynth g_sheepIceBlockRecord;    /* 0x6cf06c [888] record synthesised from export 0x62 */
ScnObject *g_flockReserveTargets[5];     /* 0x6cf080 [954] */
u8 g_flockIncomplete;                    /* 0x6cf094 [971] set while a sheep is outside the orange zones */
/* .bss, the zero-initialised part, in address order */
ScnObject *g_pFlockSam = 0;       /* 0x6cf1fc */
Sheep *g_sheepTable[20] = {0};    /* 0x6cf200 */
Sheep *g_pCheckpointSheep = 0;    /* 0x6cf250 */
u32 g_sheepMovedMask = 0;         /* 0x6cf254 */
ZoneList g_samOrangeBoxes = {0};  /* 0x6cf258  zone list (id list 0x46) */
u8 g_sheepIceBlockPresent = 0;    /* 0x6cf260 */
Sheep *g_pSheepOutOfZone = 0;     /* 0x6cf264 */
u32 g_sheepMask = 0;              /* 0x6cf268 */
ZoneList g_sheepRepelBoxes = {0}; /* 0x6cf26c  zone list (id list 0x35) */
u8 g_sheepSplashPresent = 0;      /* 0x6cf274 */
Sheep *g_pSheepFxOwner = 0;       /* 0x6cf278 */
u8 g_sheepCount = 0;              /* 0x6cf27c */
u8 g_flockReservedCount = 0;      /* 0x6cf27d */

/* ---- inline helpers (none has a caller before its definition) ---- */
/* used by the shared effects */
inline InlineEmitter16::InlineEmitter16()
{
    base.slotPool = slotBuf;
    base.particles = particleBuf;
    base.count = 16;
    base.Emitter_Reset();
}
/* used by the movement, the update and the message handler */
inline void WallAvoid::ClearActive()
{
    flags.active = 0;
}
/* used by the availability tests and the update */
#define SDW_INLINE_ZONELIST_FINDCONTAINING_VEC3S 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_FINDCONTAINING_VEC3S
/* used by the availability tests, the decision code and the message handler */
inline s32 Sheep::RestrictedByMissingSheep()
{
    return g_flockIncomplete && (g_flockInOrangeBits & (1 << (u8)index)) && !g_beachOrangeZoneList.count;
}
/* used by the decision code */
inline u32 RegistryFlags(u16 classId)
{
    return g_scenaricClassRegistry[classId].classFlags;
}
/* used by the state code, the reset and the update */
inline s32 Sheep::OwnsSharedFx()
{
    return this == g_pSheepFxOwner;
}
/* used by the state code */
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8
/* used by the update */
#define SDW_INLINE_FREE_ZONES_GET_U8 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_FREE_ZONES_GET_U8
inline s32 Sheep::ThinkThisFrame()
{
    return (u8)index == g_framesThisSecond && !(flags & SHF_SCRIPT_HELD);
}
inline s32 Sheep::AtHome()
{
    return !((pos.x - homePos.x) | (pos.z - homePos.z)) && ABS_VALUE(pos.y - homePos.y) <= 60;
}
/* used by the message handler */
inline s32 Sheep::IsOutsideCandidate()
{
    return this == g_pSheepOutOfZone || !g_pSheepOutOfZone;
}
/* used by initialisation */
inline CollBox *ScnObject::FirstModelBox()
{
    ModelBoxList *boxes = inst_model->boxes;
    if (boxes)
        return boxes->boxes;
    return 0;
}
#define SDW_INLINE_FREE_PROPERTYU32_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_PROPERTYU32_VOID_U32

/* ---- the shared effects (their initialisers open the object's code, 0x476e50-0x476eca) ---- */
InlineEmitter16 g_sheepBubbleEmit; /* 0x6cf098 [1017] */
ScnBody g_sheepSplash;             /* 0x6cef70 [174] */
ScnBody g_sheepIceCubeBody;        /* 0x6cf000 [824] */

/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets; pad0, pad1 fill gaps */
void Sheep::Move(Vec3s *delta, Vec3s *velocity, s32 grounded)
{
    struct Work {
        MoveModifyArg arg;
        u16 pad0;
        Vec3s future;
        u16 pad1;
    } w;
    if (pPlatform) {
        w.arg.delta = *delta;
        if (grounded)
            w.arg.flag0 = 1;
        else
            w.arg.flag0 = 0;
        w.arg.noPull = 0;
        w.arg.flag2 = 1;
        w.arg.velocity = *velocity;
        pPlatform->HandleMessage(this, MSG_MODIFY_MOVE, &w.arg);
        *delta = w.arg.delta;
    }
    if (pPlatform && pPlatform->GetClassId() == CLASSID_GOSSAMERONDE) {
        w.future.x = pos.x;
        w.future.y = pos.y;
        w.future.z = pos.z;
        w.future.x += delta->x;
        w.future.y += delta->y;
        w.future.z += delta->z;
        if (!TestBodyAt(&w.future, CQ_OBJECTS)) {
            Translate(delta);
            UpdateShadow();
            g_sheepMovedMask |= 1 << (u8)index;
        }
    } else {
        Translate(delta);
        UpdateShadow();
        g_sheepMovedMask |= 1 << (u8)index;
    }
}

s32 Sheep::IsAvailable()
{
    if (!(flags & (SHF_SLEEPY | SHF_SCRIPT_HELD | SHF_PUT_TO_SLEEP)) && !ReservedTarget() &&
        !HasInstanceFlag(INST_F_ATTACHED) && !RestrictedByMissingSheep())
        return 1;
    return 0;
}

void Sheep::AttachIceBlock()
{
    if (g_sheepIceBlockPresent) {
        g_sheepIceCubeBody.Position() = pos;
        g_sheepIceCubeBody.PlayAnim(AGLACON1_ANIM_STAND2, 1, 0);
        g_sheepIceCubeBody.AttachTo(this, 0, 0, 0, 0, 0);
        flags |= SHF_ICE_ATTACHED;
    }
}

void Sheep::DetachIceBlock()
{
    g_sheepIceCubeBody.Detach();
    ClearFlags(SHF_ICE_ATTACHED);
}

void Sheep::StartSplashFx()
{
    Vec3s at;
    if (g_sheepSplashPresent && pWaterZone) {
        at = pos;
        at.y = pWaterZone->min[1];
        g_sheepSplash.Position() = at;
        g_sheepSplash.PlayAnim(APLOUF01_ANIM_SPLASH, 1, 0);
        flags |= SHF_SPLASH_ACTIVE;
    }
}

void Sheep::StopSplashFx()
{
    ClearFlags(SHF_SPLASH_ACTIVE);
}

void Sheep::UpdateZoneMask()
{
    u32 mask = 1 << (u8)index;
    if (!(flags & SHF_IN_GEYSER) &&
        (BoxList_FindContainingPoint(&pos, g_samOrangeBoxes.boxes, g_samOrangeBoxes.count) ||
         BoxList_FindContainingPoint(&pos, g_beachOrangeZoneList.boxes, g_beachOrangeZoneList.count))) {
        g_flockInOrangeBits |= mask;
        if (this == g_pSheepOutOfZone)
            g_pSheepOutOfZone = 0;
        if (this == g_pCheckpointSheep && !(flags & SHF_KEEP_CHECKPOINT))
            g_pCheckpointSheep = 0;
    } else {
        g_flockInOrangeBits &= ~mask;
        if (this != g_pSheepOutOfZone)
            g_pSheepOutOfZone = this;
    }
    if (g_flockInOrangeBits == g_sheepMask)
        g_flockIncomplete = 0;
    else
        g_flockIncomplete = 1;
}

u32 Sheep::TryReserveTarget(s32 force, u32 visionRange)
{
    if (Flock_IsTargetReserved(pTarget) && !(flags & SHF_TARGET_RESERVED) && pTarget->GetClassId() == CLASSID_SALAD)
        return 0;
    if (g_pSheepOutOfZone && this != g_pSheepOutOfZone && !g_samOrangeBoxes.FindContaining(&pTarget->pos) &&
        !g_beachOrangeZoneList.FindContaining(&pTarget->pos))
        return 0;
    if ((force || (!Flock_IsTargetReserved(pTarget) &&
                   this == Flock_GetObjNearestAvailableSheep_Vision(pTarget, visionRange))) &&
        Flock_ReserveTarget(pTarget))
        flags |= SHF_TARGET_RESERVED;
    return flags & SHF_TARGET_RESERVED;
}

u32 Sheep::TryReserveScentSource()
{
    if (!Flock_IsTargetReserved(pTarget) && Flock_GetObjNearestAvailableSheep_Scent(pTarget) == this &&
        Flock_ReserveTarget(pTarget))
        flags |= SHF_TARGET_RESERVED;
    return flags & SHF_TARGET_RESERVED;
}

void Sheep::ReleaseTarget()
{
    Flock_UnreserveTarget(pTarget);
    ClearFlags(SHF_TARGET_RESERVED);
}

FlockScentSource *Flock_FindScentSource(ScnObject *object)
{
    s32 index = 0;
    while (index < g_flockScentCount && g_flockScentSources[index].obj != object)
        ++index;
    if (index == g_flockScentCount)
        return 0;
    return &g_flockScentSources[index];
}

void Flock_AddScentSource(ScnObject *object, s32 heading, s32 range)
{
    FlockScentSource *source = Flock_FindScentSource(object);
    if (!source) {
        if (g_flockScentCount >= 2)
            return;
        source = &g_flockScentSources[g_flockScentCount];
        ++g_flockScentCount;
    }
    range += 90;
    source->obj = object;
    source->heading = heading;
    source->rangeSq = range * range;
}

void Flock_RemoveScentSource(ScnObject *object)
{
    s32 index = 0;
    while (index < g_flockScentCount && g_flockScentSources[index].obj != object)
        ++index;
    if (index < g_flockScentCount) {
        --g_flockScentCount;
        while (index < g_flockScentCount) {
            g_flockScentSources[index] = g_flockScentSources[index + 1];
            ++index;
        }
    }
    if (!g_flockScentCount)
        g_flockScentSeenTime = 0;
}

/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets */
ScnObject *Sheep::FindScentSourceInRange()
{
    struct Work {
        FlockScentSource *scent;
        s32 index;
    } w;
    w.index = 0;
    while (w.index < g_flockScentCount) {
        w.scent = &g_flockScentSources[w.index];
        if (!Flock_IsTargetReserved(w.scent->obj) && TestScentSource(w.scent))
            return w.scent->obj;
        ++w.index;
    }
    return 0;
}

/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets */
s32 Sheep::TestScentSource(FlockScentSource *source)
{
    struct Work {
        u16 response[2];
        u32 distSq;
        s32 distance, dx, dy, dz, angle;
    } w;
    w.dx = source->obj->pos.x - pos.x;
    w.dy = 0;
    w.dz = source->obj->pos.z - pos.z;
    w.angle = Math_RadiansToAngle4096((float)atan2(w.dx, w.dz));
    w.dx *= w.dx;
    w.dy *= w.dy;
    w.dz *= w.dz;
    w.distSq = w.dx + w.dz;
    if (w.distSq <= source->rangeSq) {
        w.angle = ABS_VALUE(WRAP_ANGLE(source->heading - w.angle));
        if (w.angle <= 1024) {
            w.distance = (s32)sqrt((double)(s32)w.distSq);
            if ((w.distance * g_sinTable4096[w.angle] >> 12) <= 200) {
                w.response[0] = 11;
                w.response[1] = (u16)(w.distance * 65535 / (s32)sqrt((double)(s32)source->rangeSq));
                source->obj->HandleMessage(this, MSG_ATTRACT_REPORT, &w.response);
                return 1;
            }
        }
    }
    return 0;
}

s32 Sheep::CanSmellObject(ScnObject *object)
{
    FlockScentSource *scent = Flock_FindScentSource(object);
    if (!scent)
        return 0;
    return TestScentSource(scent);
}

s32 Flock_ReserveTarget(ScnObject *target)
{
    if (g_flockReservedCount < 5) {
        g_flockReserveTargets[g_flockReservedCount] = target;
        ++g_flockReservedCount;
        return 1;
    }
    return 0;
}

void Flock_UnreserveTarget(ScnObject *target)
{
    s32 index = 0;
    while (index < g_flockReservedCount && g_flockReserveTargets[index] != target)
        ++index;
    /* Original decrements even when the target is absent. */
    --g_flockReservedCount;
    while (index < g_flockReservedCount) {
        g_flockReserveTargets[index] = g_flockReserveTargets[index + 1];
        ++index;
    }
}

s32 Flock_IsTargetReserved(ScnObject *target)
{
    s32 index;
    for (index = 0; index < g_flockReservedCount; ++index) {
        if (g_flockReserveTargets[index] == target)
            return 1;
    }
    return 0;
}

Sheep *Flock_GetObjNearestAvailableSheep_Vision(ScnObject *target, u32 range)
{
    u16 radius = range;
    if (radius > 600) {
        radius = 600;
        OutputDebugStringA(
            "WARNING - Character_Sheep: Flock_GetObjNearestAvailableSheep_Vision() - parameter range out of limit.");
    }
    /* cast kept: a downcast (the callback accepts only sheep) */
    return (Sheep *)target->Scenaric_FindBestInRadius(&target->pos, target->pos.y - 150, target->pos.y + 150, radius, 0,
                                                      Flock_NearestAvailableSheepCB, 0);
}

u32 Flock_NearestAvailableSheepCB(ScnObject *, ScnObject *candidate, u32 score, ScnObject *)
{
    if (candidate->GetClassId() == CLASSID_SHEEP) {
        Sheep *sheep = (Sheep *)candidate; /* cast kept: a downcast (its class id was just tested) */
        if (sheep->IsAvailable())
            return score;
    }
    return 0xffffffff;
}

/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets */
Sheep *Flock_GetObjNearestAvailableSheep_Scent(ScnObject *source)
{
    struct Work {
        Sheep *candidate;
        FlockScentSource *scent;
        s32 bestDistance;
        Sheep *best;
        s32 distance, index;
    } w;
    w.best = 0;
    w.scent = Flock_FindScentSource(source);
    if (w.scent) {
        w.bestDistance = 0x7fffffff;
        for (w.index = 0; w.index < g_sheepCount; ++w.index) {
            w.candidate = g_sheepTable[w.index];
            if (w.candidate->IsAvailable() && w.candidate->TestScentSource(w.scent) &&
                w.candidate->pos.y >= source->pos.y - 150 && w.candidate->pos.y <= source->pos.y + 150) {
                w.distance = Vec3s_DistSqXZ(&w.candidate->pos, &source->pos);
                if (w.distance < w.bestDistance) {
                    w.best = w.candidate;
                    w.bestDistance = w.distance;
                }
            }
        }
    }
    return w.best;
}

s32 Sheep::ShouldIgnoreLures()
{
    if (RestrictedByMissingSheep() && (g_samOrangeBoxes.FindContaining(&g_pWolf->pos) ||
                                       (g_flockScentSeenTime > 0 && g_gameTime - g_flockScentSeenTime <= 0x5000)))
        return 1;
    return 0;
}

s32 Sheep::ShouldEnterFall()
{
    return fallTime > 4096 || (fallTime > 2048 && GroundY() - pos.y > 200);
}

void Sheep::EnterIdleState()
{
    if (flags & SHF_SCRIPT_HELD)
        SetStateId(SHEEP_ST_SCRIPT_HELD);
    else {
        if (g_flockInOrangeBits & (1 << (u8)index))
            SetState(SHEEP_ST_IDLE, 2, 1, 1);
        else
            SetState(SHEEP_ST_IDLE, 25, 1, 1);
        idleLoops = (s8)Rand_Range(5, 10);
    }
}

/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets */
void Sheep::NextIdleAnim()
{
    struct Work {
        SheepStateEntry *row;
        u16 base, anim;
    } w;
    if (!(flags & SHF_SCRIPT_HELD)) {
        if ((u8)idleLoops == 0) {
            w.anim = anim.animId;
            if (g_flockInOrangeBits & (1 << (u8)index))
                w.base = 2;
            else
                w.base = 25;
            if (w.anim == w.base) {
                w.anim = (u16)Rand_Bounded(2);
                idleLoops = (s8)(Rand_Range(g_sheepIdleVariants[w.anim].low, g_sheepIdleVariants[w.anim].high) - 1);
                w.anim = g_sheepIdleVariants[w.anim].anim;
            } else {
                w.anim = w.base;
                idleLoops = (s8)Rand_Range(3, 6);
            }
            w.row = &g_sheepStateTable[state];
            anim.speed = (u16)((Rand_Range(w.row->speedLow, w.row->speedHigh) * 4096) / 16);
            PlayAnim(w.anim, 1, 1);
        } else
            --idleLoops;
    }
}

/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets */
void Sheep::SetState(u8 next, u16 anim, s32 loop, s32 bleat)
{
    struct Work {
        u8 unused[3], model;
        SheepStateEntry *row;
        s32 blend;
    } w;
    w.row = &g_sheepStateTable[next];
    w.blend = 1;
    w.model = w.row->model;
    if (!w.model && (flags & SHF_ALT_MODEL))
        w.model = 2;
    if (w.model != modelIdx) {
        SwapModel(&models[w.model]);
        modelIdx = w.model;
        w.blend = 0;
    }
    if (w.row->flags & SHSF_NO_BOX_COLLIDE)
        EnableBoxCollide(0);
    else
        EnableBoxCollide(1);
    if (w.row->flags & SHSF_HIDE_SHADOW)
        shadow.SetVisible(0);
    else
        shadow.SetVisible(1);
    if (w.row->flags & SHSF_ALWAYS_UPDATE)
        SetUpdateMode(SCN_UPD_ALWAYS);
    else
        SetUpdateMode(SCN_UPD_NORMAL);
    EnableFlag2((w.row->flags & SHSF_ALWAYS_UPDATE) || !(g_flockInOrangeBits & (1 << (u8)index)));
    if ((flags & SHF_TARGET_RESERVED) && !(w.row->flags & SHSF_KEEPS_RESERVATION))
        ReleaseTarget();
    if (sfxHandle) {
        StopSound(sfxHandle);
        sfxHandle = 0;
    }
    if (bleat && (w.row->flags & SHSF_BLEAT))
        sfxHandle = PlayBleat((Rand_Range(12, 20) << 12) >> 4);
    if (w.row->flags & SHSF_FROZEN) {
        if (!(flags & SHF_ICE_ATTACHED)) {
            if (!g_pSheepFxOwner) {
                g_pSheepFxOwner = this;
                g_sheepBubbleEmit.base.Emitter_Reset();
            }
            if (OwnsSharedFx())
                AttachIceBlock();
        }
    } else if (flags & SHF_ICE_ATTACHED)
        DetachIceBlock();
    if ((flags & SHF_RIVER_CARGO) && (!pWaterZone || !(w.row->flags & SHSF_FROZEN)))
        ClearFlags(SHF_RIVER_CARGO);
    state = next;
    ClearFlags(SHF_ATE);
    stateTime = 0;
    this->anim.speed = (u16)((Rand_Range(w.row->speedLow, w.row->speedHigh) * 4096) / 16);
    PlayAnim(anim, loop, w.blend);
}

void Sheep::SetStateQuiet(u8 next)
{
    SheepStateEntry *row = &g_sheepStateTable[next];
    SetState(next, row->anim, row->flags & SHSF_ANIM_ARG, 0);
}

void Sheep::SetStateId(u8 next)
{
    SheepStateEntry *row = &g_sheepStateTable[next];
    SetState(next, row->anim, row->flags & SHSF_ANIM_ARG, 1);
}

u32 Sheep_AttractorScoreCB(ScnObject *self, ScnObject *candidate, u32 score, ScnObject *)
{
    s32 response;
    if (RegistryFlags(candidate->GetClassId()) & SCN_CF_SHEEP_ATTRACTOR) {
        response = candidate->HandleMessage(self, MSG_QUERY_ACTION, 0);
        switch (response) {
            case SHEEP_ATTR_SALAD:
                if (!Flock_IsTargetReserved(candidate)) {
                    if (candidate->HasInstanceFlag(INST_F_ATTACHED))
                        return score + 360000;
                    return score;
                }
                break;
            case SHEEP_ATTR_BUSH:
                return score;
            case SHEEP_ATTR_BUTTON:
                if (!Flock_IsTargetReserved(candidate))
                    return score + 720000;
                break;
        }
    }
    return 0xffffffff;
}

/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets */
ScnObject *Sheep::FindNearestAttractor(s32 fromFood)
{
    struct Work {
        Vec3s *origin;
        ScnObject *result;
    } w;
    if (fromFood)
        w.origin = &lastFoodPos;
    else
        w.origin = &pos;
    if (!(flags & (SHF_ATTRACTION_OFF | SHF_WENT_TO_BUSH)))
        w.result = Scenaric_FindBestInRadius(w.origin, w.origin->y - 150, w.origin->y + 150, 600, 0,
                                             Sheep_AttractorScoreCB, 0);
    else
        w.result = 0;
    return w.result;
}

/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets */
s32 Sheep::Think_FindAttraction()
{
    struct Work {
        ScnObject *candidate;
        s32 response;
    } w;
    if (!(flags & (SHF_SLEEPY | SHF_PUT_TO_SLEEP | SHF_WENT_TO_BUSH))) {
        if (!RestrictedByMissingSheep()) {
            w.candidate = FindNearestAttractor(0);
            if (ReservedTarget() && w.candidate) {
                w.response = w.candidate->HandleMessage(this, MSG_QUERY_ACTION, 0);
                if (w.response == 0x27)
                    w.candidate = 0;
            }
            if (w.candidate) {
                if (ReservedTarget())
                    ReleaseTarget();
                pTarget = w.candidate;
                if (TryReserveTarget(0, 600)) {
                    SetStateId(SHEEP_ST_ATTR_TURN);
                    return 1;
                }
                return 0;
            }
            w.candidate = FindScentSourceInRange();
            if (w.candidate) {
                if (ReservedTarget())
                    ReleaseTarget();
                pTarget = w.candidate;
                SetStateId(SHEEP_ST_SCENT_TURN);
                return 1;
            }
        } else {
            if (FindScentSourceInRange())
                g_flockScentSeenTime = g_gameTime;
        }
    }
    return 0;
}

/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets */
s32 Sheep::IsTargetValidWithin(ScnObject *target, s32 maxDistSq)
{
    struct Work {
        s32 high, low;
    } w;
    if (!(flags & SHF_ATTRACTION_OFF) && target->IsActiveInWorld()) {
        w.low = pos.y - 150;
        w.high = pos.y + 150;
        if (target->pos.y >= w.low && target->pos.y <= w.high && Vec3s_DistSqXZ(&target->pos, &pos) <= maxDistSq)
            return 1;
    }
    return 0;
}

s32 Sheep::IsTargetWithinRange(ScnObject *target, s32 range)
{
    if (!(flags & SHF_ATTRACTION_OFF) && target->IsActiveInWorld() &&
        Vec3s_DistSqXZ(&target->pos, &pos) <= (range + 6) * (range + 6))
        return 1;
    return 0;
}

s32 Sheep::IsTargetSameLevel(ScnObject *target)
{
    return ABS_VALUE(target->pos.y - pos.y) <= 60;
}

s32 Sheep::IsAtTarget(ScnObject *target, s32 range)
{
    return IsTargetWithinRange(target, range) && IsTargetSameLevel(target);
}

s32 Sheep::TurnToHeading(s32 heading)
{
    if (Heading() != heading) {
        SetHeading(Math_StepAngleTowards(Heading(), (s16)heading, 4096));
        return 0;
    }
    return 1;
}

void Sheep::TurnToTarget()
{
    TurnToHeading(HeadingTo(&pTarget->pos));
}

/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets; pad fill gaps */
void Sheep::FrozenFloatStep()
{
    struct Work {
        Vec3s delta;
        u16 pad;
        WolfMoveScratch *scratch;
        SheepStateEntry *row;
    } w;
    w.scratch = (WolfMoveScratch *)g_sharedScratch; /* cast kept: each user lays out the shared scratch its own way */
    w.row = &g_sheepStateTable[state];
    fallTime = 0;
    w.scratch->vel.z = 0;
    w.scratch->vel.x = 0;
    w.scratch->vel.y = -200;
    if (pWaterZone) {
        Zone_GetFlowVelocity(pWaterZone, &w.scratch->rot);
        ADD_VEC(w.scratch->vel, w.scratch->rot);
    }
    Vec3s_ScaleByDt(&w.scratch->vel, &w.delta);
    if (w.delta.y < 0 && pWaterZone && pos.y + w.delta.y < pWaterZone->min[1] + 105) {
        w.delta.y = pWaterZone->min[1] + 105 - pos.y;
        if (g_pFrozenRiverInst && !(flags & SHF_RIVER_CARGO) &&
            g_pFrozenRiverInst->HandleMessage(this, MSG_RIVER_ADD_CARGO, 0))
            flags |= SHF_RIVER_CARGO;
    }
    Collide_ResolveMove(&w.delta, 0, slopeLimit, RESOLVE_SLIDE_ALL, 0, 0, 20, 0, 0);
    Move(&w.delta, &w.scratch->vel, 0);
}

/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets; pad fill gaps */
s32 Sheep::FallStep(s32 sweep, s32 ramp, s32 limitY)
{
    struct Work {
        Vec3s delta;
        u16 pad;
        WolfMoveScratch *scratch;
        s32 speed, result;
        ContactInfo contact;
        SheepStateEntry *row;
    } w;
    w.result = 0;
    w.scratch = (WolfMoveScratch *)g_sharedScratch; /* cast kept: each user lays out the shared scratch its own way */
    w.row = &g_sheepStateTable[state];
    fallTime += g_dt;
    w.scratch->vel.z = 0;
    w.scratch->vel.x = 0;
    w.speed = fallTime * 1500 >> 12;
    if (w.speed > 1400)
        w.speed = 1400;
    else if (!ramp && w.speed < 1400)
        w.speed = 1400;
    w.scratch->vel.y = (s16)w.speed;
    if (pWaterZone) {
        Zone_GetFlowVelocity(pWaterZone, &w.scratch->rot);
        ADD_VEC(w.scratch->vel, w.scratch->rot);
    }
    Vec3s_ScaleByDt(&w.scratch->vel, &w.delta);
    if (pos.y + w.delta.y > limitY)
        w.delta.y = (s16)(limitY - pos.y);
    if (sweep) {
        if (Collide_ResolveMove(&w.delta, &w.contact, slopeLimit, COLL_WALL, 0, 0, 20, 0, 0) & COLL_FLOOR) {
            fallTime = 0;
            if (!w.contact.movableObj) {
                if (ramp && !(w.row->flags & SHSF_IN_WATER))
                    EnterIdleState();
                w.result = 1;
            }
        }
        if (slopeOverrideMs > 0) {
            slopeOverrideMs -= (s16)g_dtMs;
            if (slopeOverrideMs < 0)
                slopeLimit = 0xb54;
        }
    }
    w.scratch->step.z = 0;
    w.scratch->step.x = 0;
    w.scratch->step.y = Heading();
    rot = w.scratch->step;
    Move(&w.delta, &w.scratch->vel, 0);
    if (ramp)
        gravityTimer = 0x5000;
    return w.result;
}

/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets; unused1 fill gaps */
s32 Sheep::MoveToward(Vec3s *target, s32 stopDistance)
{
    ContactInfo contact;
    struct Work {
        Vec3s velocity;
        u16 pad0, unused0;
        s16 steer;
        s32 step;
        u16 unused1;
        s16 targetHeading;
        Vec3s delta;
        u16 pad1, unused2, result;
        s32 speed;
        s16 difference, distance;
    } w;
    fallTime += g_dt;
    /* cast kept: WallAvoid writes the headings as u16; here they stay s16 (signed arithmetic, movsx table indexing) */
    wallAvoid.ComputeSteer(&pos, Heading(), target, (s16)stopDistance, (u16 *)&w.steer, (u16 *)&w.targetHeading,
                           &w.distance);
    w.difference = (s16)((Heading() - w.steer + 2048) & 4095) - 2048;
    SetHeading(Math_StepAngleTowards(Heading(), w.steer, 4096));
    w.speed = fallTime * 1500 >> 12;
    if (w.speed < 120)
        w.speed = 120;
    else if (w.speed > 1400)
        w.speed = 1400;
    w.velocity.x = (s16)(-250 * g_sinTable4096[w.steer] / 4096);
    w.velocity.z = (s16)(-250 * g_pCosTable[w.steer] / 4096);
    w.velocity.y = (s16)w.speed;
    w.delta.y = (s16)(w.speed * g_dt >> 12);
    if (!stopDistance && w.distance <= 5) {
        w.delta.x = target->x - pos.x;
        w.delta.z = target->z - pos.z;
    } else {
        if (ABS_VALUE(w.difference) < 8192) {
            w.step = g_dt * 250 >> 12;
            if (w.step > w.distance && w.steer == w.targetHeading) {
                w.delta.x = (s16)(-w.distance * g_sinTable4096[w.steer] / 4096);
                w.delta.z = (s16)(-w.distance * g_pCosTable[w.steer] / 4096);
            } else {
                w.delta.x = (s16)(w.velocity.x * g_dt / 4096);
                w.delta.z = (s16)(w.velocity.z * g_dt / 4096);
            }
        } else {
            w.velocity.x = w.velocity.z = 0;
            w.delta.x = w.delta.z = 0;
        }
    }
    w.result = Collide_ResolveMove(&w.delta, &contact, 0xb54, RESOLVE_SLIDE_ALL, 0, 0, 20, 0, 0);
    if (w.result & (COLL_FLOOR | COLL_FLOOR_EDGE))
        fallTime = 0;
    wallAvoid.UpdateFromContact(&contact, w.result);
    Move(&w.delta, &w.velocity, 1);
    gravityTimer = 0x5000;
    return (Heading() == w.targetHeading || stopDistance == 0) && w.distance <= 5;
}

void Sheep::StartApproach(s32 kind)
{
    switch (kind) {
        case SHEEP_ATTR_SALAD:
            if (TryReserveTarget(0, 600)) {
                wallAvoid.ClearActive();
                SetStateId(SHEEP_ST_ATTR_WALK);
            } else
                EnterIdleState();
            break;
        case SHEEP_ATTR_BUTTON:
            if (TryReserveTarget(0, 600)) {
                if (Vec3s_DistSqXZ(&pos, &pTarget->pos) <= 10000)
                    SetStateId(SHEEP_ST_BUTTON_WALK);
                else {
                    wallAvoid.ClearActive();
                    SetStateId(SHEEP_ST_ATTR_WALK);
                }
            } else
                EnterIdleState();
            break;
        case SHEEP_ATTR_BUSH:
            if (TryReserveTarget(1, 600)) {
                wallAvoid.ClearActive();
                SetStateId(SHEEP_ST_ATTR_WALK);
            } else
                EnterIdleState();
            break;
        default:
            EnterIdleState();
            break;
    }
}

/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets; pad0, pad1, pad3, pad4, pad5 fill gaps */
void Sheep::Update()
{
    struct Work {
        Vec3s fxAt;
        u16 pad0;
        SheepStateEntry *fxRow, *waterRow;
        void *oldWater;
        Vec3s waterAt;
        u16 pad1;
        s32 landed;
        u16 pad2, contactFlags;
        ContactInfo contact;
        Vec3s specialDelta;
        u16 pad3;
        u32 zoneFlags;
        s32 heading;
        Vec3s velocity;
        u16 pad4;
        s32 moved, stopDistance;
        Vec3s delta;
        u16 pad5;
        s32 reply;
        Box *death;
    } w;
    g_sheepMovedMask &= ~(1 << (u8)index);
    stateTime += g_dt;
    switch (state) {
        case SHEEP_ST_IDLE:
            if (AnimFlags(ANIM_F_FINISHED))
                NextIdleAnim();
            if (g_pProgress->CurrentLevel() == SCENE_LVL_12 && g_pSheepOutOfZone && this == g_pSheepOutOfZone) {
                w.specialDelta.x = 0;
                w.specialDelta.y = 0;
                w.specialDelta.z = 0;
                w.specialDelta.y = (s16)(g_dtMs * 1300 / 1000);
                if (w.specialDelta.y > 40)
                    w.specialDelta.y = 40;
                else if (w.specialDelta.y <= 0)
                    w.specialDelta.y = 2;
                w.contactFlags = Collide_ResolveMove(&w.specialDelta, &w.contact, 0xb54, COLL_FLOOR, 0, 0, 10, 0, 0);
                if (!w.contactFlags) {
                    Translate(&w.specialDelta);
                    SetStateId(SHEEP_ST_FALL);
                    break;
                }
            }
            if (ThinkThisFrame()) {
                if (ShouldIgnoreLures()) {
                    if (!Rand_Bounded(2))
                        SetStateId(SHEEP_ST_LOOK_AT_WOLF);
                } else if (!Think_FindAttraction()) {
                    if (!g_beachOrangeZoneList.count && (g_flockInOrangeBits & (1 << (u8)index)) && !AtHome()) {
                        if (stateTime >= 0x2000)
                            SetStateId(SHEEP_ST_RETURN_HOME);
                    } else {
                        ClearFlags(SHF_WENT_TO_BUSH);
                        pRepelZone =
                            BoxList_FindContainingPoint(&pos, g_sheepRepelBoxes.boxes, g_sheepRepelBoxes.count);
                        if (stateTime >= 0x5000 && pRepelZone) {
                            wallAvoid.ClearActive();
                            SetStateId(SHEEP_ST_LEAVE_REPEL_BOX);
                        } else if ((flags & SHF_HAS_ANCHOR) && Vec3s_DistSqXZ(&pos, &anchorPos) > anchorRadiusSq)
                            SetStateId(SHEEP_ST_RETURN_TO_ANCHOR);
                        else if (flags & (SHF_SLEEPY | SHF_PUT_TO_SLEEP))
                            SetStateId(SHEEP_ST_SLEEP);
                    }
                }
            }
            break;
        case SHEEP_ST_SLEEP:
            if (ThinkThisFrame()) {
                if (!sfxHandle) {
                    if (!Rand_Bounded(16))
                        sfxHandle =
                            Sound_Play(SND_SMOSLEEP, this, 127, SNDF_LOOP | SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 4096);
                } else if (!Rand_Bounded(2)) {
                    StopSound(sfxHandle);
                    sfxHandle = 0;
                }
                if (ShouldIgnoreLures()) {
                    if (!Rand_Bounded(4))
                        SetStateId(SHEEP_ST_LOOK_AT_WOLF);
                } else {
                    pRepelZone = BoxList_FindContainingPoint(&pos, g_sheepRepelBoxes.boxes, g_sheepRepelBoxes.count);
                    if (stateTime >= 0x5000 && pRepelZone) {
                        wallAvoid.ClearActive();
                        SetStateId(SHEEP_ST_LEAVE_REPEL_BOX);
                    } else if ((flags & SHF_HAS_ANCHOR) && Vec3s_DistSqXZ(&pos, &anchorPos) > anchorRadiusSq)
                        SetStateId(SHEEP_ST_RETURN_TO_ANCHOR);
                    else if (!(flags & (SHF_SLEEPY | SHF_PUT_TO_SLEEP)))
                        EnterIdleState();
                }
            }
            break;
        case SHEEP_ST_CARRIED:
            g_sheepMovedMask |= 1 << (u8)index;
            break;
        case SHEEP_ST_CARRIED_BY_SAM:
            g_sheepMovedMask |= 1 << (u8)index;
            if (CurrentAnim() == AMOUTO01_ANIM_CARRY1B && AnimFlags(ANIM_F_FINISHED))
                PlayAnim(AMOUTO01_ANIM_WALK4, 1, 1);
            break;
        case SHEEP_ST_FALL:
            FallStep(1, 1, 32000);
            break;
        case SHEEP_ST_ATTR_TURN:
            TurnToTarget();
            if (AnimFlags(ANIM_F_FINISHED))
                SetStateQuiet(SHEEP_ST_ATTR_LOOK);
            break;
        case SHEEP_ST_ATTR_LOOK:
            if (ThinkThisFrame())
                pTarget = FindNearestAttractor(0);
            if (AnimFlags(ANIM_F_FINISHED)) {
                if (pTarget)
                    w.reply = pTarget->HandleMessage(this, MSG_QUERY_ACTION, 0);
                else
                    w.reply = SHEEP_ATTR_NONE;
                StartApproach(w.reply);
            }
            break;
        case SHEEP_ST_ATTR_WALK:
            w.reply = pTarget->HandleMessage(this, MSG_QUERY_ACTION, 0);
            switch (w.reply) {
                case SHEEP_ATTR_BUSH:
                    w.stopDistance = 120;
                    flags |= SHF_WENT_TO_BUSH;
                    break;
                case SHEEP_ATTR_BUTTON:
                    w.stopDistance = 100;
                    break;
                default:
                    w.stopDistance = 80;
                    break;
            }
            w.moved = MoveToward(&pTarget->pos, w.stopDistance);
            if (ShouldEnterFall())
                SetStateId(SHEEP_ST_FALL);
            else if (!IsTargetValidWithin(pTarget, 360000) || !w.reply)
                SetStateId(SHEEP_ST_ATTR_LOOK);
            else if (w.moved) {
                switch (w.reply) {
                    case SHEEP_ATTR_SALAD:
                        SetStateId(SHEEP_ST_EAT_ALIGN);
                        break;
                    case SHEEP_ATTR_BUSH:
                        SetStateId(SHEEP_ST_EAT_BUSH);
                        break;
                    case SHEEP_ATTR_BUTTON:
                        if (TryReserveTarget(0, 600))
                            SetStateId(SHEEP_ST_BUTTON_WALK);
                        break;
                    default:
                        SetStateId(SHEEP_ST_ATTR_LOOK);
                        break;
                }
            } else if (w.reply == SHEEP_ATTR_BUTTON && ThinkThisFrame())
                Think_FindAttraction();
            break;
        case SHEEP_ST_SCENT_TURN:
            TurnToTarget();
            CanSmellObject(pTarget);
            if (AnimFlags(ANIM_F_FINISHED))
                SetStateId(SHEEP_ST_SCENT_SNIFF);
            break;
        case SHEEP_ST_SCENT_SNIFF:
            if (ThinkThisFrame())
                pTarget = FindScentSourceInRange();
            if (pTarget)
                CanSmellObject(pTarget);
            if (AnimFlags(ANIM_F_FINISHED)) {
                if (pTarget && TryReserveScentSource()) {
                    wallAvoid.ClearActive();
                    SetStateId(SHEEP_ST_SCENT_WALK);
                } else
                    EnterIdleState();
            }
            break;
        case SHEEP_ST_SCENT_WALK:
            w.moved = MoveToward(&pTarget->pos, 80);
            if (ShouldEnterFall())
                SetStateId(SHEEP_ST_FALL);
            else if (!CanSmellObject(pTarget))
                SetStateId(SHEEP_ST_SCENT_SNIFF);
            else if (w.moved)
                SetStateId(SHEEP_ST_SCENT_SNIFF);
            break;
        case SHEEP_ST_EAT_ALIGN:
            w.heading = HeadingTo(&pTarget->pos);
            w.stopDistance = WRAP_ANGLE(w.heading - Heading());
            if (ABS_VALUE(w.stopDistance) <= 341) {
                if (CurrentAnim() != AMOUTO01_ANIM_STAND2 && AnimFlags(ANIM_F_FINISHED))
                    PlayAnim(AMOUTO01_ANIM_STAND2, 1, 1);
            } else {
                if (CurrentAnim() != AMOUTO01_ANIM_WALK1)
                    PlayAnim(AMOUTO01_ANIM_WALK1, 1, 1);
                TurnToHeading(w.heading);
            }
            if (!IsTargetValidWithin(pTarget, 360000))
                SetStateId(SHEEP_ST_ATTR_LOOK);
            else if (!IsTargetWithinRange(pTarget, 80))
                SetStateId(SHEEP_ST_ATTR_WALK);
            else if (!pTarget->HasInstanceFlag(INST_F_ATTACHED) && IsTargetSameLevel(pTarget))
                SetStateId(SHEEP_ST_EAT);
            break;
        case SHEEP_ST_EAT:
            if (!sfxHandle)
                sfxHandle = Sound_Play(SND_SMOEATIN, this, 255, SNDF_LOOP | SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 4096);
            w.moved = 1;
            if (IsAtTarget(pTarget, 80)) {
                lastFoodPos = pTarget->pos;
                if (pTarget->HandleMessage(this, MSG_USE, 0)) {
                    flags |= SHF_ATE;
                    w.moved = 0;
                }
            }
            if (w.moved) {
                ReleaseTarget();
                if (!(flags & SHF_ATE))
                    SetStateId(SHEEP_ST_ATTR_LOOK);
                else {
                    pTarget = FindNearestAttractor(1);
                    if (!pTarget)
                        SetStateId(SHEEP_ST_WALK_TO_LAST_FOOD);
                    else if (!IsTargetValidWithin(pTarget, 360000))
                        SetStateId(SHEEP_ST_ATTR_WALK_FAR);
                    else if (TryReserveTarget(0, 600))
                        SetStateId(SHEEP_ST_ATTR_WALK);
                    else
                        SetStateId(SHEEP_ST_ATTR_LOOK);
                }
            }
            break;
        case SHEEP_ST_EAT_BUSH:
            if (!sfxHandle)
                sfxHandle = Sound_Play(SND_SMOEATIN, this, 255, SNDF_LOOP | SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 4096);
            w.moved = 1;
            if (IsAtTarget(pTarget, 120) && pTarget->HandleMessage(this, MSG_USE, 0))
                w.moved = 0;
            if (w.moved)
                SetStateId(SHEEP_ST_ATTR_LOOK);
            break;
        case SHEEP_ST_WALK_TO_LAST_FOOD:
            w.moved = MoveToward(&lastFoodPos, 0);
            if (ShouldEnterFall())
                SetStateId(SHEEP_ST_FALL);
            else if (w.moved || stateTime >= 0x5000)
                SetStateId(SHEEP_ST_ATTR_LOOK);
            break;
        case SHEEP_ST_BUTTON_WALK:
            w.moved = MoveToward(&pTarget->pos, 0);
            if (ShouldEnterFall())
                SetStateId(SHEEP_ST_FALL);
            else if (!IsAtTarget(pTarget, 100))
                SetStateId(SHEEP_ST_ATTR_LOOK);
            else if (w.moved)
                SetStateId(SHEEP_ST_BUTTON_STAND);
            else if (ThinkThisFrame())
                Think_FindAttraction();
            break;
        case SHEEP_ST_BUTTON_STAND:
            if (ThinkThisFrame()) {
                w.reply = pTarget->HandleMessage(this, MSG_QUERY_ACTION, 0);
                if (w.reply != SHEEP_ATTR_BUTTON || !IsAtTarget(pTarget, 100))
                    SetStateId(SHEEP_ST_ATTR_LOOK);
                else if (!Think_FindAttraction() && stateTime >= 0x5000 && !Rand_Bounded(4))
                    SetStateId(SHEEP_ST_BUTTON_FIDGET);
            }
            break;
        case SHEEP_ST_BUTTON_FIDGET:
            if (AnimFlags(ANIM_F_FINISHED))
                SetStateId(SHEEP_ST_BUTTON_STAND);
            break;
        case SHEEP_ST_ATTR_WALK_FAR:
            MoveToward(&pTarget->pos, 80);
            if (ShouldEnterFall())
                SetStateId(SHEEP_ST_FALL);
            else if (!pTarget->IsActiveInWorld())
                SetStateId(SHEEP_ST_ATTR_LOOK);
            else if (IsTargetValidWithin(pTarget, 360000)) {
                if (TryReserveTarget(0, 600))
                    SetStateId(SHEEP_ST_ATTR_WALK);
                else
                    SetStateId(SHEEP_ST_ATTR_LOOK);
            }
            break;
        case SHEEP_ST_RETURN_HOME:
            w.moved = MoveToward(&homePos, 0);
            if (ShouldEnterFall())
                SetStateId(SHEEP_ST_FALL);
            else if (w.moved)
                EnterIdleState();
            else if (ThinkThisFrame())
                Think_FindAttraction();
            break;
        case SHEEP_ST_DIE_FALL:
            FallStep(1, 1, fallLimitY);
            gravityTimer = 0x5000;
            break;
        case SHEEP_ST_DIE_FADE:
            flags |= SHF_FADED_OUT;
            SetTint(0, 4096, 1);
            break;
        case SHEEP_ST_LEAVE_REPEL_BOX:
            pRepelZone = BoxList_FindContainingPoint(&pos, g_sheepRepelBoxes.boxes, g_sheepRepelBoxes.count);
            if (!pRepelZone)
                EnterIdleState();
            else {
                w.delta = pos;
                w.zoneFlags = pRepelZone->flags;
                if (w.zoneFlags & FLOW_POS_Z)
                    w.delta.z += 500;
                else if (w.zoneFlags & FLOW_NEG_Z)
                    w.delta.z -= 500;
                if (w.zoneFlags & FLOW_POS_X)
                    w.delta.x += 500;
                else if (w.zoneFlags & FLOW_NEG_X)
                    w.delta.x -= 500;
                MoveToward(&w.delta, 0);
                if (ShouldEnterFall())
                    SetStateId(SHEEP_ST_FALL);
                else if (ThinkThisFrame())
                    Think_FindAttraction();
            }
            break;
        case SHEEP_ST_DIE_FALL_NOCOLLIDE:
            FallStep(0, 1, 32000);
            gravityTimer = 0x5000;
            break;
        case SHEEP_ST_WALK_TO_POINT:
            if (MoveToward(&walkDest, 0)) {
                g_pWolf->HandleMessage(this, MSG_WOLF_SET_CINE_READY, 0);
                EnterIdleState();
            } else if (stateTime >= 0xa000) {
                SetPosition(&walkDest);
                g_pWolf->HandleMessage(this, MSG_WOLF_SET_CINE_READY, 0);
                EnterIdleState();
            }
            break;
        case SHEEP_ST_LOOK_AT_WOLF:
            SetHeading(Math_StepAngleTowards(Heading(), HeadingTo(&g_pWolf->pos), 4096));
            if (ThinkThisFrame() && !ShouldIgnoreLures() && !Rand_Bounded(2))
                EnterIdleState();
            break;
        case SHEEP_ST_RETURN_TO_ANCHOR:
            w.moved = MoveToward(&anchorPos, 0);
            if (ShouldEnterFall())
                SetStateId(SHEEP_ST_FALL);
            else if (!(flags & SHF_HAS_ANCHOR) || Vec3s_DistSqXZ(&pos, &anchorPos) <= anchorRadiusSq || w.moved)
                EnterIdleState();
            break;
        case SHEEP_ST_CALL_TURN:
            TurnToTarget();
            if (AnimFlags(ANIM_F_FINISHED))
                SetStateId(SHEEP_ST_CALL_WAIT);
            break;
        case SHEEP_ST_CALL_WAIT:
            w.reply = pTarget->HandleMessage(this, MSG_QUERY_ACTION, 0);
            if (!IsTargetValidWithin(pTarget, 640000) || w.reply != SHEEP_ATTR_LEADER)
                SetStateId(SHEEP_ST_ATTR_LOOK);
            else if (Vec3s_DistSqXZ(&pos, &pTarget->pos) >= 40000)
                SetStateId(SHEEP_ST_CALL_FOLLOW);
            break;
        case SHEEP_ST_CALL_FOLLOW:
            MoveToward(&pTarget->pos, 0);
            if (ShouldEnterFall())
                SetStateId(SHEEP_ST_FALL);
            else {
                w.reply = pTarget->HandleMessage(this, MSG_QUERY_ACTION, 0);
                if (!IsTargetValidWithin(pTarget, 640000) || w.reply != SHEEP_ATTR_LEADER)
                    SetStateId(SHEEP_ST_ATTR_LOOK);
                else if (Vec3s_DistSqXZ(&pos, &pTarget->pos) <= 10000)
                    SetStateId(SHEEP_ST_CALL_WAIT);
            }
            break;
        case SHEEP_ST_DROWN:
            FallStep(1, 1, 32000);
            gravityTimer = 0x5000;
            break;
        case SHEEP_ST_FROZEN_FLOAT:
            FrozenFloatStep();
            if (!pWaterZone || (pWaterZone->flags & ZONE_WATER_NO_SURFACE))
                SetStateId(SHEEP_ST_FROZEN_FALL);
            else if (!(flags & SHF_RIVER_CARGO) && stateTime >= 0x14000)
                Fade_StartRestart(0);
            break;
        case SHEEP_ST_FROZEN_FALL:
            w.landed = FallStep(1, 0, 32000);
            if (pWaterZone && !(pWaterZone->flags & ZONE_WATER_NO_SURFACE))
                SetStateId(SHEEP_ST_FROZEN_FLOAT);
            else if (w.landed) {
                SetStateId(SHEEP_ST_FROZEN_REST);
                Sound_Play(SND_ICE_HIT, this, 255, SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 4096);
            }
            break;
        case SHEEP_ST_FROZEN_CARRIED:
            g_sheepMovedMask |= 1 << (u8)index;
            break;
        case SHEEP_ST_THAW:
            FallStep(1, 0, 32000);
            if (stateTime >= 0x2000)
                SetStateId(SHEEP_ST_SHIVER);
            break;
        case SHEEP_ST_SHIVER:
            if (!sfxHandle || !SoundPlaying(sfxHandle))
                sfxHandle = Sound_Play(SND_SCOCOLD, this, 255, SNDF_LOOP | SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 4096);
            FallStep(1, 0, 32000);
            if (g_sheepIceCubeBody.AnimFlags(ANIM_F_FINISHED) && stateTime >= 0x4000)
                SetStateId(SHEEP_ST_FALL);
            break;
        case SHEEP_ST_LAUNCHED:
            gravityTimer = 0;
            break;
        case SHEEP_ST_GEYSER_OUT:
            if (AnimFlags(ANIM_F_FINISHED)) {
                gravityTimer = 0x5000;
                SetStateId(SHEEP_ST_FALL);
            }
            break;
        case SHEEP_ST_GEYSER_IN:
            fallTime = 0;
            gravityTimer = 0;
            break;
        case SHEEP_ST_FROZEN_GEYSER_IN:
            fallTime = 0;
            gravityTimer = 0;
            if (stateTime >= 0x2000 && CurrentAnim() != AMOUTO01_ANIM_WHIRL2A) {
                PlayAnim(AMOUTO01_ANIM_WHIRL2A, 0, 1);
                if (flags & SHF_ICE_ATTACHED)
                    g_sheepIceCubeBody.PlayAnim(AGLACON1_ANIM_WHIRL2, 0, 1);
            }
            break;
        case SHEEP_ST_IN_CANNON:
            fallTime = 0;
            gravityTimer = 0;
            break;
        case SHEEP_ST_RUN_TO_SAM:
            w.heading = HeadingTo(&g_pFlockSam->pos);
            SetHeading(Math_StepAngleTowards(Heading(), (s16)w.heading, 4096));
            w.velocity.x = (s16)(g_sinTable4096[w.heading] * -500 / 4096);
            w.velocity.z = (s16)(g_pCosTable[w.heading] * -500 / 4096);
            w.velocity.y = 0;
            Vec3s_ScaleByDt(&w.velocity, &w.delta);
            Translate(&w.delta);
            if (AnimFlags(ANIM_F_FINISHED))
                SetStateId(SHEEP_ST_WALK_TO_SAM);
            break;
        case SHEEP_ST_WALK_TO_SAM:
            w.moved = MoveToward(&g_pFlockSam->pos, 100);
            if (w.moved)
                EnterIdleState();
            break;
    }
    if ((gravityTimer > 0 || pPlatform) && !(g_sheepMovedMask & (1 << (u8)index))) {
        gravityTimer -= g_dt;
        if (gravityTimer < 0)
            gravityTimer = 0;
        FallStep(1, 0, 32000);
        if (ShouldEnterFall())
            SetStateId(SHEEP_ST_FALL);
    }
    if ((g_sheepMovedMask & (1 << (u8)index)) && !(flags & SHF_DEAD)) {
        UpdateZoneMask();
        w.death = Zones_Get(ZONE_DEATH)->FindContaining(&pos);
        if (w.death) {
            flags |= SHF_DEAD;
            if (w.death->flags & FLOW_NEG_Z) {
                Fade_StartRestart(0x3000);
                SetStateId(SHEEP_ST_DIE_FADE);
            } else if (!HasInstanceFlag(INST_F_ATTACHED)) {
                if (w.death->flags & FLOW_POS_Z) {
                    fallLimitY = w.death->max[1];
                    SetStateId(SHEEP_ST_DIE_FALL);
                }
                Fade_StartRestart(0x1000);
            }
        }
        if (Zones_Get(ZONE_SHADOW)->FindContaining(&pos))
            flags |= SHF_IN_FADE_ZONE;
        else
            ClearFlags(SHF_IN_FADE_ZONE);
        if (flags & SHF_FADED_OUT)
            SetTint(0, 4096, 1);
        else if (flags & SHF_IN_FADE_ZONE)
            SetTint(0, 3072, 1);
        else
            SetTint(0, 0, 0);
        if (!HasInstanceFlag(INST_F_ATTACHED)) {
            w.oldWater = pWaterZone;
            w.waterAt = pos;
            w.waterAt.y -= 60;
            pWaterZone = Zones_Get(ZONE_WATER)->FindContaining(&w.waterAt);
            if (pWaterZone) {
                w.waterRow = &g_sheepStateTable[state];
                if (!(w.waterRow->flags & SHSF_IN_WATER)) {
                    if (pWaterZone->flags & ZONE_WATER_FREEZING)
                        SetStateId(SHEEP_ST_FROZEN_FLOAT);
                    else {
                        flags |= SHF_DEAD;
                        Fade_StartRestart(0x3000);
                        SetStateId(SHEEP_ST_DROWN);
                    }
                }
                if (!w.oldWater) {
                    if (!g_pSheepFxOwner) {
                        g_pSheepFxOwner = this;
                        g_sheepBubbleEmit.base.Emitter_Reset();
                    }
                    if (OwnsSharedFx())
                        StartSplashFx();
                }
            }
        }
    }
    ClearFlags(SHF_HAS_ANCHOR | SHF_PUT_TO_SLEEP);
    AdvanceAnim();
    if (OwnsSharedFx()) {
        w.fxRow = &g_sheepStateTable[state];
        if ((w.fxRow->flags & SHSF_IN_WATER) || g_sheepBubbleEmit.base.flags.active) {
            w.fxAt = pos;
            w.fxAt.y += (s16)Rand_Range(-50, -25);
            w.fxAt.x += (s16)Rand_Range(-25, 25);
            w.fxAt.z += (s16)Rand_Range(-25, 25);
            if (pWaterZone)
                g_sheepBubbleParams.cap = pWaterZone->min[1];
            g_sheepBubbleParams.size = (s16)Rand_Range(10, 20);
            g_sheepBubbleEmit.base.Emitter_UpdateRiseToCap(&g_sheepBubbleParams, &w.fxAt,
                                                           w.fxRow->flags & SHSF_IN_WATER);
        }
        if (flags & SHF_SPLASH_ACTIVE) {
            g_sheepSplash.AdvanceAnim();
            if (g_sheepSplash.AnimFlags(ANIM_F_FINISHED))
                StopSplashFx();
        }
        if (flags & SHF_ICE_ATTACHED) {
            if (pWaterZone && ABS_VALUE(pos.y - pWaterZone->min[1] - 105) <= 5) {
                if (g_sheepIceCubeBody.CurrentAnim() == AMOUTO01_ANIM_STAND1)
                    g_sheepIceCubeBody.PlayAnim(AGLACON1_ANIM_STAND1, 1, 0);
            } else if (g_sheepIceCubeBody.CurrentAnim() == AMOUTO01_ANIM_SMEL2)
                g_sheepIceCubeBody.PlayAnim(AGLACON1_ANIM_STAND2, 1, 1);
            g_sheepIceCubeBody.Position() = pos;
            g_sheepIceCubeBody.AdvanceAnim();
        }
        if (!(flags & SHF_SPLASH_ACTIVE) && !(flags & SHF_ICE_ATTACHED) && !g_sheepBubbleEmit.base.flags.active) {
            g_pSheepFxOwner = 0;
            ClearFlags(SHF_SPLASH_ACTIVE);
        }
    }
}

/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets; unused2 fill gaps */
s32 Sheep::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    struct Work {
        Vec3s *destination;
        WolfRespawnArg *checkpoint;
        WolfSpotArg *anchor;
        SheepStateEntry *dropRow;
        DropMsgArg *drop;
        u8 unused0[3], joint;
        ScnObject *carrier;
        s32 canPickup;
        SheepStateEntry *pickupRow, *touchRow;
        u32 result;
        u16 unused1, anim;
        s32 loop;
        Vec3s rotation;
        u16 unused2;
    } w;
    w.result = 0;
    if (msgId < MSG_WOLF_CAUGHT) {
        switch (msgId) {
            case MSG_QUERY_MOVING:
                if (state == SHEEP_ST_LAUNCHED)
                    w.result = 1;
                else
                    w.result = 2;
                break;
            case MSG_KILL:
                switch ((u32)arg) { /* cast kept: this message's void * arg carries the kill kind */
                    case KILL_FALL_PIT:
                        flags |= SHF_DEAD;
                        Fade_StartRestart(0x7000);
                        if (!HasInstanceFlag(INST_F_ATTACHED))
                            SetStateId(SHEEP_ST_DIE_FALL_NOCOLLIDE);
                        break;
                    case KILL_GENERIC:
                        if (state != SHEEP_ST_DIE_FADE) {
                            flags |= SHF_DEAD;
                            Fade_StartRestart(0x3000);
                            SetStateId(SHEEP_ST_DIE_FADE);
                        }
                        break;
                    case KILL_CRUSH:
                        if (state != SHEEP_ST_DIE_CRUSH) {
                            flags |= SHF_DEAD;
                            Fade_StartRestart(0x3000);
                            SetStateId(SHEEP_ST_DIE_CRUSH);
                        }
                        break;
                }
                w.result = 1;
                break;
            case MSG_QUERY_ACTION:
                w.touchRow = &g_sheepStateTable[state];
                if ((sender->GetClassId() == CLASSID_WOLF || sender->GetClassId() == CLASSID_SAM) &&
                    !(w.touchRow->flags & (SHSF_FROZEN | SHSF_DYING | SHSF_NOT_GRABBABLE))) {
                    if (pos.y <= sender->pos.y + 50 && pos.y >= sender->pos.y - 80)
                        w.result = 2;
                    else
                        w.result = 0;
                }
                break;
            case MSG_LANDED:
                if (!(flags & SHF_DEAD))
                    SetStateId(SHEEP_ST_FALL);
                w.result = 1;
                break;
            case MSG_SEESAW_TOUCH:
                gravityTimer = 0x5000;
                w.result = 0;
                break;
            case MSG_GEYSER_IN:
                if (IsOutsideCandidate()) {
                    if (g_sheepStateTable[state].flags & SHSF_FROZEN) {
                        SetStateId(SHEEP_ST_FROZEN_GEYSER_IN);
                        if (flags & SHF_ICE_ATTACHED)
                            g_sheepIceCubeBody.PlayAnim(AGLACON1_ANIM_WHIRL1, 1, 1);
                        w.result = 2000;
                    } else {
                        SetStateId(SHEEP_ST_GEYSER_IN);
                        w.result = Anim_GetDurationMs(Inst(), CurrentAnim(), 1);
                    }
                    flags |= SHF_IN_GEYSER;
                    UpdateZoneMask();
                }
                break;
            case MSG_GEYSER_OUT:
                fallTime = 0x800;
                gravityTimer = 0x5000;
                slopeLimit = 0xb54;
                slopeOverrideMs = 0;
                if (arg) {
                    SetStateId(SHEEP_ST_GEYSER_OUT);
                    rot = sender->rot;
                    ClearFlags(SHF_IN_GEYSER);
                    UpdateZoneMask();
                    w.result = Anim_GetDurationMs(Inst(), CurrentAnim(), 1);
                }
                break;
            case MSG_SEESAW_DRIFT:
                if (state == SHEEP_ST_IDLE) {
                    fallTime = 0x800;
                    SetStateId(SHEEP_ST_FALL);
                    slopeLimit = 0x1000;
                    slopeOverrideMs = 500;
                    w.result = 1;
                }
                break;
            case MSG_PICKUP:
                w.pickupRow = &g_sheepStateTable[state];
                w.canPickup = 0;
                if (!(flags & SHF_SCRIPT_HELD) || sender == pScriptHolder) {
                    switch (sender->GetClassId()) {
                        case CLASSID_WOLF:
                            if (RestrictedByMissingSheep()) {
                                if (state != SHEEP_ST_LOOK_AT_WOLF)
                                    SetStateId(SHEEP_ST_LOOK_AT_WOLF);
                            } else
                                w.canPickup = !(w.pickupRow->flags & (SHSF_FROZEN | SHSF_DYING | SHSF_NOT_GRABBABLE));
                            break;
                        case CLASSID_SAM:
                            w.canPickup = !(w.pickupRow->flags & (SHSF_FROZEN | SHSF_DYING | SHSF_NOT_GRABBABLE));
                            break;
                        case CLASSID_CRANE:
                            w.canPickup = 1;
                            break;
                    }
                }
                if (w.canPickup) {
                    w.carrier = sender;
                    w.joint = (u8)(u32)arg; /* cast kept: this message's void * arg carries the joint number */
                    if (sender->GetClassId() == CLASSID_SAM) {
                        SetStateId(SHEEP_ST_CARRIED_BY_SAM);
                        carryHeadingOffset = 0;
                        AttachTo(w.carrier, w.joint, 0, 0, 0, 0);
                    } else {
                        if (w.pickupRow->flags & SHSF_FROZEN) {
                            SetStateId(SHEEP_ST_FROZEN_CARRIED);
                            w.rotation.y = (Heading() + 512) & 4095;
                            SetHeading(w.rotation.y - w.rotation.y % 1024);
                        } else
                            SetStateId(SHEEP_ST_CARRIED);
                        carryHeadingOffset = (Heading() - w.carrier->Heading()) & 4095;
                        w.rotation.x = w.rotation.z = 0;
                        w.rotation.y = carryHeadingOffset;
                        /* cast kept: drops the const of the .rdata offset (AttachTo takes a Vec3s *) */
                        AttachTo(w.carrier, w.joint, (Vec3s *)&sheepCarryOffset, &w.rotation, 1, 0);
                    }
                    w.result = 1;
                }
                break;
            case MSG_DROP:
                w.dropRow = &g_sheepStateTable[state];
                w.drop = (DropMsgArg *)arg; /* cast kept: this message's void * arg is a DropMsgArg */
                Detach();
                SetHeading((sender->Heading() + carryHeadingOffset) & 4095);
                if (!w.drop->flag1)
                    SetPosition(&w.drop->pos);
                if (w.dropRow->flags & SHSF_FROZEN)
                    SetStateId(SHEEP_ST_FROZEN_FALL);
                else
                    SetStateId(SHEEP_ST_FALL);
                ClearFlags(SHF_IN_GEYSER);
                UpdateZoneMask();
                w.result = 1;
                break;
            case MSG_CARRY_ANIM:
                w.anim = anim.animId;
                w.loop = 1;
                switch ((u32)arg) { /* cast kept: this message's void * arg carries the Wolf's animation cue */
                    case WOLF_CUE_IDLE:
                    case WOLF_CUE_JUMP:
                    case WOLF_CUE_GLIDE:
                        w.anim = AMOUTO01_ANIM_STAND8;
                        w.loop = 1;
                        break;
                    case WOLF_CUE_WALK:
                    case WOLF_CUE_RUN:
                        w.anim = AMOUTO01_ANIM_RUN2;
                        w.loop = 1;
                        break;
                    case WOLF_CUE_5:
                    case WOLF_CUE_6:
                        w.anim = AMOUTO01_ANIM_STAND2;
                        w.loop = 1;
                        break;
                    case WOLF_CUE_SPIN:
                        w.anim = AMOUTO01_ANIM_TORNA4;
                        w.loop = 0;
                        break;
                    case WOLF_CUE_SPIN_END:
                        w.anim = AMOUTO01_ANIM_TORNA5;
                        w.loop = 0;
                        break;
                }
                if (CurrentAnim() != w.anim)
                    PlayAnimSelection(w.anim, w.loop, 1);
                w.result = 1;
                break;
            case MSG_CINE_PLACE:
                flags |= SHF_ATTRACTION_OFF;
                break;
            case MSG_CINE_END:
                ClearFlags(SHF_ATTRACTION_OFF);
                break;
            case MSG_QUERY_HELD_ACTION:
                w.result = 2;
                break;
            case MSG_QUERY_MOVED:
                if (g_sheepMovedMask & (1 << (u8)index))
                    w.result = 1;
                break;
            case MSG_SCRIPT_HOLD:
                flags |= SHF_SCRIPT_HELD;
                SetStateId(SHEEP_ST_SCRIPT_HELD);
                pScriptHolder = sender;
                w.result = 1;
                break;
            case MSG_SCRIPT_RELEASE:
                ClearFlags(SHF_SCRIPT_HELD);
                EnterIdleState();
                pScriptHolder = 0;
                w.result = 1;
                break;
            case MSG_SET_ANCHOR:
                w.anchor = (WolfSpotArg *)arg; /* cast kept: this message's void * arg is a WolfSpotArg */
                flags |= SHF_HAS_ANCHOR;
                anchorPos = w.anchor->pos;
                anchorRadiusSq = w.anchor->radius * w.anchor->radius;
                w.result = 1;
                break;
            case MSG_TIMEMACHINE_SWAP:
                if (g_flockInOrangeBits & (1 << (u8)index))
                    SetPosition(&homePos);
                SnapToGround(1);
                EnterIdleState();
                w.result = 1;
                break;
            case MSG_THAW:
                if (state == SHEEP_ST_FROZEN_REST) {
                    if (flags & SHF_ICE_ATTACHED) {
                        g_sheepIceCubeBody.PlayAnim(AGLACON1_ANIM_FOND1, 0, 0);
                        SetStateId(SHEEP_ST_THAW);
                    } else
                        SetStateId(SHEEP_ST_FALL);
                }
                w.result = 1;
                break;
            case MSG_RIDER_ADD:
                if (!pPlatform || pPlatform == sender) {
                    pPlatform = sender;
                    w.result = 1;
                }
                break;
            case MSG_RIDER_REMOVE:
                if (pPlatform == sender) {
                    pPlatform = 0;
                    w.result = 1;
                }
                break;
            case MSG_LAUNCH:
                if (!(flags & SHF_DEAD))
                    SetStateId(SHEEP_ST_LAUNCHED);
                break;
            case MSG_SHEEP_CALL:
                if (g_sheepStateTable[state].flags & SHSF_FOLLOWING) {
                    if (sender == pTarget) {
                        SetStateId(SHEEP_ST_ATTR_LOOK);
                        w.result = 1;
                    }
                } else {
                    if (g_sheepStateTable[state].flags & SHSF_ON_BUTTON) {
                        if (ReservedTarget())
                            ReleaseTarget();
                        pTarget = 0;
                        EnterIdleState();
                    }
                    if (IsAvailable() && arg) {
                        pTarget = sender;
                        if (TryReserveTarget(0, 800)) {
                            wallAvoid.ClearActive();
                            SetStateId(SHEEP_ST_CALL_TURN);
                            w.result = 1;
                        } else
                            pTarget = 0;
                    }
                }
                break;
        }
    } else {
        switch (msgId) {
            case MSG_SHEEP_SAVE_RESPAWN:
                w.checkpoint = (WolfRespawnArg *)arg; /* cast kept: this message's void * arg is a WolfRespawnArg */
                if (!(g_sheepStateTable[state].flags & SHSF_FROZEN)) {
                    respawnPos = w.checkpoint->pos;
                    respawnHeading = w.checkpoint->facing;
                    g_pCheckpointSheep = this;
                }
                w.result = 1;
                break;
            case MSG_SHEEP_KEEP_CHECKPOINT:
                flags |= SHF_KEEP_CHECKPOINT;
                w.result = 1;
                break;
            case MSG_SHEEP_WALK_TO:
                SetStateId(SHEEP_ST_WALK_TO_POINT);
                w.destination = (Vec3s *)arg; /* cast kept: this message's void * arg is the destination */
                walkDest = *w.destination;
                walkDest.y = QueryGroundY(&walkDest, 0);
                w.result = 1;
                break;
            case MSG_SHEEP_QUERY_AVAILABLE:
                if (IsAvailable() || ReservedTarget() == sender)
                    w.result = 1;
                break;
            case MSG_SHEEP_SLEEP:
                flags |= SHF_PUT_TO_SLEEP;
                w.result = 1;
                break;
            case MSG_SHEEP_QUERY_IN_WATER:
                if (pWaterZone)
                    w.result = 1;
                break;
            case MSG_SHEEP_ENTER_CANNON:
                rot = sender->rot;
                SetStateId(SHEEP_ST_IN_CANNON);
                PlayAnim(g_sheepStateTable[state].anim, 0, 0);
                w.result = 1;
                break;
            case MSG_SHEEP_MARK_IN_GEYSER:
                if (IsOutsideCandidate()) {
                    flags |= SHF_IN_GEYSER;
                    UpdateZoneMask();
                    w.result = 1;
                }
                break;
            case MSG_SHEEP_RUN_TO_SAM:
                if (!HasInstanceFlag(INST_F_ATTACHED) && state != SHEEP_ST_RUN_TO_SAM)
                    SetStateId(SHEEP_ST_RUN_TO_SAM);
                w.result = 1;
                break;
        }
    }
    return w.result;
}

void Sheep::Render(Camera *view)
{
    ScnMobile::Render(view);
    if (OwnsSharedFx()) {
        if (flags & SHF_SPLASH_ACTIVE)
            g_sheepSplash.ScnBody::Render(view);
        if (flags & SHF_ICE_ATTACHED) {
            g_sheepIceCubeBody.Position() = pos;
            g_sheepIceCubeBody.Rotation() = rot;
            g_sheepIceCubeBody.ScnBody::Render(view);
        }
        if (g_sheepBubbleEmit.base.flags.active)
            g_sheepBubbleEmit.base.Emitter_Render(view, 0);
    }
}

void Sheep::ResetVars()
{
    fallTime = 0;
    stateTime = 0;
    gravityTimer = 0;
    fallLimitY = 32000;
    idleLoops = 0;
    slopeLimit = 0xb54;
    slopeOverrideMs = 0;
    pRepelZone = 0;
    pScriptHolder = 0;
    pWaterZone = 0;
    ClearFlags(SHF_DEAD | SHF_ATTRACTION_OFF | SHF_SCRIPT_HELD | SHF_HAS_ANCHOR | SHF_SPLASH_ACTIVE | SHF_RIVER_CARGO |
               SHF_PUT_TO_SLEEP | SHF_IN_FADE_ZONE | SHF_FADED_OUT | SHF_WENT_TO_BUSH | SHF_IN_GEYSER);
    lastFoodPos.x = 0;
    lastFoodPos.y = 0;
    lastFoodPos.z = 0;
    walkDest.x = 0;
    walkDest.y = 0;
    walkDest.z = 0;
    wallAvoid.Reset();
    g_sheepMovedMask &= ~(1 << (u8)index);
}

/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets; pad0, pad1 fill gaps */
void Sheep::Reset()
{
    struct Work {
        Vec3s at;
        u16 pad0;
        Vec3s rotation;
        u16 pad1;
        SheepStateEntry *row;
    } w;
    w.row = &g_sheepStateTable[state];
    SetTint(0, 0, 0);
    w.rotation.x = 0;
    w.rotation.z = 0;
    if (this != g_pCheckpointSheep) {
        w.at = homePos;
        w.rotation.y = homeHeading;
    } else {
        w.at = respawnPos;
        w.at.y = QueryGroundY(&w.at, 0);
        w.rotation.y = respawnHeading;
    }
    SetPosition(&w.at);
    rot = w.rotation;
    ResetVars();
    EnterIdleState();
    PlayAnim(CurrentAnim(), 1, 0);
    UpdateZoneMask();
}

/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets */
void Sheep::PostLoadInit()
{
    struct Work {
        void *properties;
        u32 flags;
    } w;
    index = g_sheepCount;
    ++g_sheepCount;
    g_sheepTable[(u8)index] = this;
    if ((u8)index == 0) {
        if (models[2].Present())
            ShareAnimTable(&models[2].modelResIdx);
        /* cast kept (the three lists): an export id list, here of zone boxes */
        g_sheepRepelBoxes.boxes = (Box **)Scn_FindIdList(WAR_IDO_SHEEPREPELBOX, &g_sheepRepelBoxes.count);
        g_samOrangeBoxes.boxes = (Box **)Scn_FindIdList(WAR_IDO_BOXSAMORANGE, &g_samOrangeBoxes.count);
        g_beachOrangeZoneList.boxes = (Box **)Scn_FindIdList(WAR_IDO_BEACHORANGEZONE2, &g_beachOrangeZoneList.count);
        g_pFlockSam = 0;
        Scenaric_FindByClass(CLASSID_SAM, &g_pFlockSam, 1);
        /* cast kept: the builder fills the record as raw u16 words */
        if (Scn_BuildRecordFromExport(WAR_IDO_APLOUF01, (u16 *)&g_sheepSplashSynthRec, 0, 0)) {
            g_sheepSplash.ScnBody::Init(&g_sheepSplashSynthRec, 0);
            g_sheepSplash.EnableStaticFlag(1);
            g_sheepSplashPresent = 1;
        } else
            g_sheepSplashPresent = 0;
        /* cast kept: the builder fills the record as raw u16 words */
        if (Scn_BuildRecordFromExport(WAR_IDO_AGLACON1, (u16 *)&g_sheepIceBlockRecord, 0, 0)) {
            g_sheepIceCubeBody.ScnBody::Init(&g_sheepIceBlockRecord, 0);
            g_sheepIceCubeBody.EnableStaticFlag(1);
            g_sheepIceBlockPresent = 1;
        } else
            g_sheepIceBlockPresent = 0;
        g_sheepBubbleEmit.base.Emitter_Reset();
        g_pSheepFxOwner = 0;
        g_pFrozenRiverInst = 0;
        Scenaric_FindByClass(CLASSID_FROZENRIVER, &g_pFrozenRiverInst, 1);
    }
    g_sheepMask = 0xffffffffU >> (32 - g_sheepCount);
    UpdateZoneMask();
    weight = 40;
    SetShadowRadius((u8)FirstModelBox()->max.x);
    flags = 0;
    pTarget = 0;
    ResetVars();
    pPlatform = 0;
    modelIdx = 0;
    w.properties = record;
    w.flags = PropertyU32(w.properties, 0);
    if (w.flags & SHEEP_PROP_F_1)
        flags |= SHF_SLEEPY;
    if ((w.flags & SHEEP_PROP_F_ALT_MODEL) && models[2].Present())
        flags |= SHF_ALT_MODEL;
    SnapToGround(1);
    homePos = pos;
    homeHeading = Heading();
    respawnPos = homePos;
    respawnHeading = homeHeading;
    sfxHandle = 0;
    EnterIdleState();
    PlayAnim(CurrentAnim(), 1, 0);
}

void Flock_ResetGlobals()
{
    g_sheepCount = 0;
    g_sheepMovedMask = 0;
    g_flockInOrangeBits = 0;
    g_flockReservedCount = 0;
    g_flockScentCount = 0;
    g_flockScentSeenTime = 0;
    g_pCheckpointSheep = 0;
    g_pSheepOutOfZone = 0;
    g_samOrangeBoxes.boxes = 0;
    g_samOrangeBoxes.count = 0;
    g_beachOrangeZoneList.boxes = 0;
    g_beachOrangeZoneList.count = 0;
    g_flockIncomplete = 0;
}

ScnObject *Sheep_Create(void *record)
{
    Sheep *object = new Sheep;
    /* cast kept: InitWithAltModels returns the base class */
    object = (Sheep *)object->InitWithAltModels(record, &object->models[0], 2, g_sheepAltModelIds, &object->models[1]);
    Flock_ResetGlobals();
    return object;
}
