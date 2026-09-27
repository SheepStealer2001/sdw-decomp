/*
 * Wolf (Ralph): the declarations shared by the Wolf source files - the members of the Wolf and of the classes its code
 * calls into, the globals and callees it uses, and the small inline helpers whose /Ob1 expansions the matches depend on.
 * The class LAYOUTS come from the generated src/include/sdw_classes.h; this header only adds members through its hooks.
 *
 * Include this INSTEAD of sdw_classes.h. A Wolf source file that needs members not declared here adds them with
 * SDW_EXTRA_<Class> (e.g. SDW_EXTRA_Wolf) defined before the include. Where the Wolf files' declarations differ, the
 * defining file's is used. The inline helpers have no bodies of their own in the exe (except where an out-of-line copy
 * is noted), so their names are not recovered; each is here because its expansion gives the original's stack
 * temporaries.
 */
/* BYTES: inline. */
/* BYTES(inline): ScnObject::Facing (member-macro inline): source-only inline: every read goes through a fresh 2-byte stack temp (0x48f955, 0x48c3f9) */
/* BYTES(inline): ScnObject::SetFacing (member-macro inline): source-only inline: its argument is a stack temp (0x48bf08, 0x48fd3c) */
/* BYTES(inline): ScnObject::GetAttachLink (member-macro inline): source-only inline: a pointer copy (0x490fc9) */
/* BYTES(inline): ScnBody::PlayAnim (member-macro inline): source-only inline: start animation with option bits (out-of-line copies noted, e.g. Dragon_PlayAnim 0x448a40) */
/* BYTES(inline): ScnBody::AnimId (member-macro inline): source-only inline: a stack copy per read (0x48ad11, 0x48cfc6) */
/* BYTES(inline): ScnBody::AnimFlags (member-macro inline): source-only inline: the mask in a register (0x48c3d1) */
/* BYTES(inline): ScnMobile::GroundY (member-macro inline): source-only inline: a fresh 16-bit temp per read (0x48dc93, 0x48cf69) */
/* BYTES(inline): ScnMobile::SetShadowRadius (member-macro inline): source-only inline: a byte temp (0x48db6a) */
/* BYTES(inline): <pad> IsValid (member-macro inline): source-only inline: setne before the test (0x48d8b6) */
/* BYTES(inline): Wolf::ClearFlags / ClearFxFlags (member-macro inline): source-only inline: the constant mask is loaded into a register and complemented there (mov eax,0x10000; not eax at 0x490cdd, 0x483781, 0x48d1db) */
#ifndef SDW_WOLF_H
#define SDW_WOLF_H


#define SDW_MEMBERS_ScnObject                                                                                    \
    static void *operator new(u32 size); /* 0x50d5f4 Scenaric_Alloc: every class factory's `new` (level heap) */ \
    void AttachTo(ScnObject *parent, u8 joint, Vec3s *offset, Vec3s *rot, u32 arg, u32 arg2); /* 0x50ff14 */     \
    CollBox *GetFirstModelBox();                                                                                 \
    void SetFacing(s16 f); /* inline: its argument is a stack temp (0x48bf08, 0x48fd3c) */                       \
    AttachLink *GetAttachLink()                                                                                  \
    {                                                                                                            \
        return attachLink;                                                                                       \
    } /* inline: a pointer copy (0x490fc9) */


#define SDW_MEMBERS_ParticleEmitter                                                                                   \
    ParticleEmitter();                                    /* inline (defined below): Emitter_Init(0), no pools yet */ \
    ParticleEmitter(Vec3f *slots, Particle *parts, u8 n); /* inline (defined below): the pools are given */
#define SDW_MEMBERS_TrailEmitter TrailEmitter();          /* inline (defined below): the pools are the inline buffers */
#define SDW_MEMBERS_WolfLaunchPath s32 Step(WolfArcScratch *s, s32 dt); /* inline, defined below */
#define SDW_MEMBERS_Wolf                                                                                                                                                                                                   \
    const s16 *SurfaceTuning(); /* inline accessor, defined below */                                                                                                                                                       \
    /* inline: clear flag bits; the constant mask is loaded into a register and complemented there */ /* (mov eax,0x10000; not eax at 0x490cdd, 0x483781, 0x48d1db), which a written-out `flags &= ~mask` does not give */ \
    void ClearFlags(u32 mask)                                                                                                                                                                                              \
    {                                                                                                                                                                                                                      \
        flags &= ~mask;                                                                                                                                                                                                    \
    }                                                                                                                                                                                                                      \
    void ClearFxFlags(u32 mask)                                                                                                                                                                                            \
    {                                                                                                                                                                                                                      \
        fxFlags &= ~mask;                                                                                                                                                                                                  \
    }

#include "sdw_types.h"
#include "sdw_enums.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *inst, Animator *animator, u16 id, u32 opts); /* 0x550196 (used by the PlayAnim inline) */
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "../objects/instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_FACING 1
#define SDW_INLINE_SCNOBJECT_SETFACING_S16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_FACING
#undef SDW_INLINE_SCNOBJECT_SETFACING_S16
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_SCNBODY_ANIMID 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMID
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#define SDW_INLINE_SCNMOBILE_GROUNDY 1
#define SDW_INLINE_SCNMOBILE_SETSHADOWRADIUS_U8 1
#include "../engine/scn_mobile_inlines.h"
#undef SDW_INLINE_SCNMOBILE_GROUNDY
#undef SDW_INLINE_SCNMOBILE_SETSHADOWRADIUS_U8
#define SDW_INLINE_ALTMODEL_ISVALID 1
#include "../engine/alt_model_inlines.h"
#undef SDW_INLINE_ALTMODEL_ISVALID
#include "sdw_global_views.h"
#include "scenaric_props.h"

/* ---- callees ---- */
/* engine/scn_tools.cpp, engine/approach.cpp, engine/maths.cpp */
#include "../engine/scn_tools.h"
#include "../engine/approach.h"
#include "../engine/maths.h"
#include "scn_controllable.h"
#include "../engine/fixed_math.h"
#include "../objects/world_draw.h"
#include "../engine/scenaric.h"
#include "../engine/sound_mgr.h"
#include "../objects/camera.h"
#include "../engine/progress_inventory.h"
#include "../engine/fade.h"
#include "../engine/map.h"
#include "../engine/input.h"
#include "wolf_move.h"
#include "wolf_api.h"
#include "../app/app_main.h"
#include "../engine/game_state.h"
s32 Rand_Bounded(s32 bound); /* 0x561219 */
/* game/scn_controllable.cpp */
/* engine/fixed_math.cpp, then the CRT */
extern "C" s16 Math_RadiansToAngle4096(float radians);
#include "../sdk/crt.h"
u16 Sound_Play(u16 soundId, void *owner, u16 volume, u8 flags, s32 rate); /* 0x5491b8 */

/* ---- globals ---- */
extern Wolf *g_pWolf;                 /* 0x6cf310 */
extern u8 g_sharedScratch[];          /* 0x6d5468  shared scratch; the move steps keep their vectors there */
extern const u16 g_wolfAltModelIds[]; /* 0x5750a0  {4, 64, 99, 124, 154} */
extern const IdleAnimEntry g_wolfGhostIdleAnims[]; /* 0x5758bc  ghost-costume idle anims (the tail of the table at
                                                     * 0x5758a0): animId, then the idle count's range */
extern const Vec3s g_wolfPutDownOffsetNear;        /* 0x5758e0  (0,-2,0) */
extern const Vec3s g_wolfPutDownOffsetFront;       /* 0x5758e8  (0,-70,-92) */
extern const Vec3s g_wolfEquipPreviewOffset;       /* 0x5758f0  (0,-100,0) */
extern const u8 g_wolfClimbDirTable[]; /* 0x575920  climb-box direction (flags bits 27-30) -> heading / 0x200 */
extern "C" s16 g_sinTable4096[5122];   /* 0x57ece0  4.12 sine, 4096 steps per turn */

#define g_padMasks (g_inputMap + 4) /* 0x57eb80  active-low button masks: [6] inventory 0xfbff, [8] view 0xefff */
extern "C" const s16 *g_pCosTable;  /* 0x5814e4  = g_sinTable4096 + 1024 */
extern u32 g_gameFlags;             /* 0x6ddf74 */
extern u32 g_gameTime;              /* 0x71b2d0  (u32 here: the Wolf code compares it unsigned) */
extern s32 g_dtMs;                  /* 0x71b2e8  g_dt * 1000 >> 12 */
extern s32 g_dt;                    /* 0x71b300  frame delta, 4.12 seconds */

/* The two camera flag bytes are bitfields (CamFlagBits, CamRequestBits in data/structs; bits = the CamFlags /
 * CamRequestFlags enums of sdw_enums.h). */

extern CamScriptState g_camScriptState;
#define g_camScriptReturnMode (g_camScriptState.returnMode) /* 0x6e43ac */
/* the camera request the Wolf fills every frame (read by Camera_Update) */

/* EmitterFlagBits (an emitter's flags byte) and DropMsgArg (the message-5 argument) are bitfield structs in data/structs. */

#define SDW_ABS(v) ((v) >= 0 ? (v) : -(v))
/* signed difference a - b of two headings, in -2048..2047 */
#define SDW_ANGLE_DIFF(a, b) ((s16)((s16)(((a) - (b) + 2048) & 4095) - 2048))

/* ---- inline helpers ---- */

/* The tuning row of the current bank and surface. It has to be an inline: the callers that pass it straight to
 * Wolf_SurfaceVelocity compute surface * 0x12 BEFORE mode * 0x48 and form lea [mode + surface + bank] (0x489128,
 * 0x48936d, 0x48a098-0x48a0b2, 0x48a82a, 0x48bd1a), which VC6 only does for an inlined call: the same expression
 * written out in the call evaluates mode's term first, as Wolf_MoveTowardPoint's two calls do (0x4898ed, 0x489a5f)
 * and Wolf_CanJump 0x491b61. About 60 spellings of the direct expression were tried; none reverses the order. */
/* BYTES(inline): source-only inline: callers passing it to Wolf_SurfaceVelocity compute surface * 0x12 before mode * 0x48 and form lea [mode + surface + bank], which VC6 only does for an inlined call (about 60 spellings of the direct expression tried) */
inline const s16 *Wolf::SurfaceTuning()
{
    return g_wolfMoveBank0[mode].surfaceTuning[surface];
}

/* Whether the level has any zone of a type (a ZoneType, index into the seven lists; ZONE_PRINTS = footprints): the
 * constant type is loaded into a register and scaled in the address (0x488ba8; a narrow parameter type gives the scaled
 * form, int gives shl), and the comparison is materialised with setg before the test, which is what an inlined
 * `return count > 0` compiles to (also 0x4cc666 ZONE_DEATH, 0x441ad6 ZONE_SHADOW). */
/* BYTES(inline): source-only inline: the constant type is loaded into a register and scaled in the address (a narrow parameter gives the scaled form, int gives shl) and the compare is materialised with setg (0x488ba8) */
inline s32 Zones_Exist(u8 type)
{
    return g_waterZones[type].count > 0;
}

/* One of the seven global zone lists by ZoneType (ZONE_WATER first); the constant type is loaded into a register first
 * (0x48dcb0 xor edx,edx), as in Zones_Exist. */
/* BYTES(inline): source-only inline: the constant type is loaded into a register first (0x48dcb0 xor edx,edx) */
#define SDW_INLINE_FREE_ZONES_GET_U8 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_FREE_ZONES_GET_U8

/* The first box of a zone list that contains p, or 0: the list's address is kept in a stack temp and the count and
 * boxes are read through it (0x48af87-0x48afa7 on the Wolf's climbZones +0x120; 0x490437, 0x48dcb9, 0x48e105; the
 * same expansion on the global lists at 0x4cc66b). */
/* BYTES(inline): source-only inline: the list's address is kept in a stack temp and count / boxes are read through it (0x48af87-0x48afa7) */
#define SDW_INLINE_ZONELIST_FINDCONTAINING_VEC3S 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_FINDCONTAINING_VEC3S

/* One step along the launch trajectory (msg 0xC): while t < 0x100 advance t by dt (capped at 0x100) and put the point
 * of the per-axis quadratic p0 + (v * t >> 8) + (a * t * t >> 15) in s->target; returns whether it was still on the
 * path. Inlined into Wolf_LaunchArcStep: the path's address (the inline's this), dt (g_dtMs >> 2) and t * t are stack
 * temps (0x48bfe6-0x48c037), but the target is written through the caller's scratch pointer itself ([ebp-0x18] + 8,
 * 0x48c060). VC6 /Ob1 substitutes an argument that is a plain variable and gives any other argument a temp: passed as
 * &s->target (pointer or reference) the target got a temp of its own. So the inline takes the scratch pointer. */
/* BYTES(inline): source-only inline: the path's address, dt and t*t are stack temps but the target is written through the caller's scratch pointer, so the inline takes the scratch pointer (0x48bfe6-0x48c060) */
inline s32 WolfLaunchPath::Step(WolfArcScratch *s, s32 dt)
{
    s32 t2;
    if (t < 0x100) {
        t = dt + t;
        if (t > 0x100)
            t = 0x100;
        t2 = t * t;
        s->target.x = x.p0 + ((t * x.v) >> 8) + ((t2 * x.a) >> 15);
        s->target.y = y.p0 + ((t * y.v) >> 8) + ((t2 * y.a) >> 15);
        s->target.z = z.p0 + ((t * z.v) >> 8) + ((t2 * z.a) >> 15);
        return 1;
    }
    return 0;
}

/* GetClassId + Scenaric_ClassFlags: a u16 temp for the id, then a u32 temp for the flags (0x48be7d/0x48be8e, 0x48f461,
 * 0x491268; the same pair in ScnControllable_ScanInteractables 0x49267d/0x492694, where these two were first found). */
/* BYTES(inline): source-only inline: expansion temporaries */
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID

#define SDW_INLINE_FREE_SCENARIC_CLASSFLAGS_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_FREE_SCENARIC_CLASSFLAGS_U16

/* The first box of the model's box list, or 0 without a list (the count is not checked).
 * ScnObject_GetFirstModelBox 0x4c1ec0 is an out-of-line copy. */
/* BYTES(inline): source-only inline: inline with an out-of-line copy at 0x4c1ec0 */
inline CollBox *ScnObject::GetFirstModelBox()
{
    ModelBoxList *list = inst_model->boxes;
    if (list)
        return list->boxes;
    return 0;
}

/* Whether p lies inside box (both faces inclusive): its two parameters are stack temps (0x4905d6). */
/* BYTES(inline): source-only inline: its two parameters are stack temps (0x4905d6) */
#define SDW_INLINE_FREE_BOX_CONTAINSPOINT_BOX_VEC3S 1
#include "../engine/coll_box_inlines.h"
#undef SDW_INLINE_FREE_BOX_CONTAINSPOINT_BOX_VEC3S

/* Whether the camera runs a scripted mode (CAM_SCRIPTED .. CAM_SCRIPT_TO_SCRIPT): its value is materialised in a
 * temporary before the test
 * (0x48fb6e-0x48fb7e). */
/* BYTES(inline): source-only inline: its value is materialised in a temporary before the test (0x48fb6e-0x48fb7e) */
inline s32 Camera_IsScriptedOrReturning()
{
    return g_camMode == CAM_SCRIPT_BLEND_IN || g_camMode == CAM_SCRIPT_TO_SCRIPT || g_camMode == CAM_SCRIPTED ||
           g_camMode == CAM_SCRIPT_RETURN;
}

/* The emitter constructors, inlined into the factories. A plain ParticleEmitter starts with no pools (Emitter_Init(0));
 * the trail kind is a ParticleEmitter `base` followed by inline buffers for 16 particles, and constructs the base with
 * them (pools, count, Emitter_Reset - no Emitter_Init). DefusableMine_Create 0x4d7c90 has the same inline shape for 1
 * particle, so the original was probably one template over the slot count. */
/* BYTES(inline): source-only inline: the emitter constructors inlined into the factories (probably one template over the slot count in the original) */
inline ParticleEmitter::ParticleEmitter()
{
    Emitter_Init(0);
}

inline ParticleEmitter::ParticleEmitter(Vec3f *slots, Particle *parts, u8 n)
{
    slotPool = slots;
    particles = parts;
    count = n;
    Emitter_Reset();
}

inline TrailEmitter::TrailEmitter() : base(slotBuf, particleBuf, 16) {}

#endif
