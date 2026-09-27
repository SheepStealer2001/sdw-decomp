/* match-addr: g_wolfPropModelTable=0x5750ac   (g_wolfPropBaseModelId + g_wolfPropModelIds as one array, see below) */
/*
 * T093 - the original object Wolf.cpp (guessed name; the Wolf's main file), one file.
 * .text 0x47d310-0x488bda (Wolf_StateMachine .. Wolf_Create), .rdata 0x5750a0-0x575920 (the main CONST 0x5750a0-0x5758f8,
 * then the Wolf vtable COMDAT that Wolf_Create emits), .data 0x57acd8-0x57b218, .bss 0x6cf280-0x6cf318.
 * The rest of the Wolf's code is T094 (src/game/wolf_move.cpp) and T095 (src/game/wolf_misc.cpp); all three use the
 * shared src/game/wolf.h, and this file's SDW_EXTRA_ members are below.
 *
 * Data. .rdata holds the model-id lists, the two state-descriptor tables, the idle / dance tables and the put-down
 * offsets; since they are in .rdata they are const here, and two spellings stand in for plain declarations:
 *  - g_wolfStateDescriptors0 / g_wolfStateTable1 are const (their decorated names gain the const); Wolf_InitMoveConfig
 *    stores them in WolfMoveBank.states (a WolfStateDesc *), so a macro of the same name casts the const away there.
 *  - g_wolfPropBaseModelId 0x5750ac and g_wolfPropModelIds 0x5750ae cannot be two objects: VC6 4-aligns an array in
 *    .rdata, so an array after a u16 would start at 0x5750b0. They are one array g_wolfPropModelTable {base, 8 props}
 *    (like the Wolf's own record model + g_wolfAltModelIds), and the two names are macros for its elements.
 * .data: this object's copy of the cinematic stride table, the ten trail parameter blocks Wolf_UpdateTrailFx (T095) uses,
 * the two movement-profile tables. .bss: all zero-initialised, so VC6 keeps them in definition order (uninitialised
 * globals would be ordered by a hash of their names).
 */
/* BYTES: cast, dead-code, flow, inline, layout, slot-group, view. */
/* BYTES(layout): the Cine.h header static: every object including the cinematic header carries this copy in its .data, referenced or not */
/* BYTES(view): const because the original has them in .rdata; the macro casts the const away for WolfMoveBank.states */
/* BYTES(layout): one array: VC6 would 4-align a separate array after the u16 base id; the two old names are element macros */
/* BYTES(layout): one table of two rows: row 1 is addressed as symbol + offset (mov eax,0x57afb0 at 0x48811f), as the original does */
/* BYTES(layout): written '= 0' only to keep definition order in .bss */
/* BYTES(inline): shared ScnObject::SetPartHeight: the constant is loaded into a register and shifted unsigned (0x47e54c) */
/* BYTES(inline): ScnObject::GetPos (member-macro inline): source-only inline: its value goes to a stack temp taken before the rest of the expression is evaluated (0x482379) */
/* BYTES(inline): shared ScnObject::InstFlags: the mask in a register (0x48764d, 0x484da7) */
/* BYTES(inline): shared ScnObject::IsInWorld: flag 0x100 materialised with neg/sbb/neg before the test (0x48617b) */
/* BYTES(inline): ScnObject::SetAnimSound (member-macro inline): source-only inline: SCN_OF_MUTE_ANIM_SOUND cleared when on (0x487ec3, 0x488503) */
/* BYTES(inline): shared ScnObject::SetVisible: SCN_OF_HIDDEN cleared when on (0x4884d3) */
/* BYTES(inline): shared SetNoDistCull / local SetNoPlaneCull: the flag set or cleared (0x4885de, 0x48860e) */
/* BYTES(inline): shared Shadow::Reproject / SetFlag4: the shadow's address (their this) is kept in a stack temp (0x47e062, 0x4883d7) */
/* BYTES(inline): shared ZoneList::Clear expansion at 0x4888aa */
/* BYTES(inline): shared Progress::CurrentLevel: a byte temp (0x48840d) */
/* BYTES(inline): shared Cine::IsActive: the value goes through a stack temp before the test (0x485d06, 0x4866ff, 0x487af5) */
/* BYTES(inline): ScnMobile::GetShadow (member-macro inline): source-only inline: the shadow block at +0x64 */
/* BYTES(inline): ScnBody::LoopSfx (member-macro inline): source-only inline: a stack copy (0x47e4f1) */
/* BYTES(flow, inferred): PAD_PRESSED macro: a press: down now (active low), up in the previous frame */
#define SDW_EXTRA_ScnObject                                                                                      \
    /* inline: its value goes to a stack temp taken before the rest of the expression is evaluated (0x482379) */ \
    Vec3s *GetPos()                                                                                              \
    {                                                                                                            \
        return &pos;                                                                                             \
    }                                                                                                            \
    /* inline, defined below */                                                                                  \
    inline void SetTint(u32 color, s16 amount, s32 on);                                                          \
    /* inline: SCN_OF_MUTE_ANIM_SOUND cleared when on (0x487ec3 Wolf_Reset, 0x488503 Wolf_Init) */               \
    void SetAnimSound(s32 on)                                                                                    \
    {                                                                                                            \
        if (on)                                                                                                  \
            flags &= (u16)~SCN_OF_MUTE_ANIM_SOUND;                                                               \
        else                                                                                                     \
            flags |= SCN_OF_MUTE_ANIM_SOUND;                                                                     \
    }                                                                                                            \
    void SetNoPlaneCull(s32 on)                                                                                  \
    {                                                                                                            \
        if (on)                                                                                                  \
            flags |= SCN_OF_NO_PLANE_CULL;                                                                       \
        else                                                                                                     \
            flags &= (u16)~SCN_OF_NO_PLANE_CULL;                                                                 \
    }                                                                                                            \
    inline void SetUpdateMode(u32 mode);                                                                         \
    /* defined below */
#define SDW_EXTRA_Shadow

#define SDW_EXTRA_ParticleEmitter inline void RenderGround(Camera *view, s32 fwd); /* inline, defined below */
#define SDW_EXTRA_TrailEmitter inline void RenderGround(Camera *view, s32 fwd);    /* inline, defined below */
#define SDW_EXTRA_ZoneList \
    void Load(u32 id);     \
    /* inline (0x4888aa) */


#define SDW_EXTRA_Wolf                        \
    Shadow *GetShadow()                       \
    {                                         \
        return &shadow;                       \
    } /* inline: the shadow block at +0x64 */ \
    u16 LoopSfx()                             \
    {                                         \
        return loopSfxHandle;                 \
    } /* inline: a stack copy (0x47e4f1) */

#include "scenaric_props.h"
#include "sdw_enums.h"
#include "wolf.h"
#define SDW_INLINE_SCNOBJECT_SETPARTHEIGHT_U32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETPARTHEIGHT_U32
#define SDW_INLINE_SCNOBJECT_INSTFLAGS_U16 1
#define SDW_INLINE_SCNOBJECT_ISINWORLD 1
#define SDW_INLINE_SCNOBJECT_SETVISIBLE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_INSTFLAGS_U16
#undef SDW_INLINE_SCNOBJECT_ISINWORLD
#undef SDW_INLINE_SCNOBJECT_SETVISIBLE_S32
#define SDW_INLINE_SCNOBJECT_SETNODISTCULL_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETNODISTCULL_S32
#define SDW_INLINE_SHADOW_REPROJECT 1
#define SDW_INLINE_SHADOW_SETFLAG4_S32 1
#include "../engine/shadow_inlines.h"
#undef SDW_INLINE_SHADOW_REPROJECT
#undef SDW_INLINE_SHADOW_SETFLAG4_S32
#define SDW_INLINE_ZONELIST_CLEAR 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_CLEAR
#define SDW_INLINE_PROGRESS_CURRENTLEVEL 1
#include "../engine/progress_inlines.h"
#undef SDW_INLINE_PROGRESS_CURRENTLEVEL
#define SDW_INLINE_CINE_ISACTIVE 1
#include "../engine/cine_inlines.h"
#undef SDW_INLINE_CINE_ISACTIVE

/* ---- callees not declared in wolf.h ---- */
#include "../engine/progress_inventory.h"
#include "../objects/animation.h"
#include "../engine/approach.h"
#include "../objects/camera.h"
#include "../objects/timemachine.h"
#include "../engine/fade.h"
#include "../engine/cine.h"
#include "../engine/id_list.h"
#include "../engine/scn_tools.h"
#include "../engine/scenaric.h"
#include "../engine/progress.h"
#include "../engine/input.h"
s32 Vec3s_DistSqXZ(Vec3s *a, Vec3s *b); /* 0x51588e  dx*dx + dz*dz */
class Instance;
struct Animator;
/* 0x5468ee - move cur toward target: *speed approaches the speed that would arrive this frame (halved near the end,
 * capped at maxSpeed) and cur moves that far along the line to target. */

/* Sign-bit overlap test, as in src/engine/scn_tools.cpp: nonzero when all four differences are >= 0. As an inline
 * its arguments are evaluated last to first and all four are live at once (esi/edi, 0x47e1e7). */
/* BYTES(inline): source-only inline: its arguments are evaluated last to first and all four are live at once (esi / edi, 0x47e1e7) */
#define SDW_INLINE_FREE_OVERLAP4_S32_S32_S32_S32 1
#include "../engine/coll_box_inlines.h"
#undef SDW_INLINE_FREE_OVERLAP4_S32_S32_S32_S32

extern const WolfDanceMove g_wolfDanceMoves[4]; /* 0x5758c8  state 0xc1: button -> dance animation */

/* A button press on this frame: down now (active low), up in the previous frame. */
#define PAD_PRESSED(pad, mask) (!((pad)->cur.buttons & ~(mask)) && ((pad)->prev.buttons & ~(mask)))

/* The scripted camera's owner, or 0 unless the camera runs mode 7, 9 or 10: an inlined getter (0x481d2b; the same at
 * 0x460190, 0x4a0829, 0x4a67db, 0x55a3a7). The mode test is materialised in a temp; the result goes straight to the
 * variable assigned. */
/* BYTES(inline): source-only inline: the mode test is materialised in a temp; the result goes straight to the variable (0x481d2b) */
#define SDW_INLINE_FREE_CAMERA_HASSCRIPTOWNER 1
#include "../objects/camera_inlines.h"
#undef SDW_INLINE_FREE_CAMERA_HASSCRIPTOWNER

#define SDW_INLINE_FREE_CAMERA_GETSCRIPTOWNER 1
#include "../objects/camera_inlines.h"
#undef SDW_INLINE_FREE_CAMERA_GETSCRIPTOWNER

/* Clear the three stick words (stickX, stickY, stickMag): the address is a stack temp (0x47e001), as for an inline
 * taking &stickX. */
/* BYTES(inline): source-only inline: the address is a stack temp (0x47e001), as for an inline taking &stickX */
#define SDW_INLINE_FREE_STICK_CLEAR_S32 1
#include "scn_controllable_inlines.h"
#undef SDW_INLINE_FREE_STICK_CLEAR_S32

/* ---- more callees and globals (the update) ---- */

/* ---- inline helpers ---- */

/* Set / clear bits of a 16-bit flag word through its address: the address is a stack temp and the constant mask is
 * loaded into a register (0x48410b-0x484127); the clear complements it and narrows it to 16 bits first (0x484138). */
/* BYTES(inline): source-only inline: the address is a stack temp and the constant mask is loaded into a register; the clear complements and narrows it first (0x48410b-0x484138) */
#define SDW_INLINE_FREE_BITS16_SET_U16_U16 1
#include "../objects/instance_inlines.h"
#undef SDW_INLINE_FREE_BITS16_SET_U16_U16

#define SDW_INLINE_FREE_BITS16_CLEAR_U16_U16 1
#include "../objects/instance_inlines.h"
#undef SDW_INLINE_FREE_BITS16_CLEAR_U16_U16

/* The object's colour tint: colour, strength and the instance flag 0x10 that turns the tinted draw on. Each arm of the
 * flag test takes the flags' address again (a temp per arm, 0x484114 / 0x484135); a computed amount gets its own temp
 * (0x484213). */
/* BYTES(inline): source-only inline: each arm of the flag test takes the flags' address again; a computed amount gets its own temp (0x484114, 0x484135, 0x484213) */
#define SDW_INLINE_SCNOBJECT_SETTINT_U32_S16_S32 2
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETTINT_U32_S16_S32

/* Whether the collision sweeps test the object's boxes: flag 0x400 clears when on (0x4872aa). */
/* BYTES(inline): source-only inline: flag 0x400 cleared when on (0x4872aa) */
#define SDW_INLINE_SCNOBJECT_SETBOXCOLLIDE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETBOXCOLLIDE_S32

/* The blob shadow on or off (flag 1 = not drawn). Called on the ScnMobile's shadow block (+0x64), whose address is a
 * stack temp (0x48722d). */
/* BYTES(inline): source-only inline: called on the shadow block (+0x64), whose address is a stack temp (0x48722d) */
#define SDW_INLINE_SHADOW_SETVISIBLE_S32 1
#include "../engine/shadow_inlines.h"
#undef SDW_INLINE_SHADOW_SETVISIBLE_S32

/* An emitter's ground-plane draw, through the forwarder or directly (0x486ea9, 0x486ee8). */
/* BYTES(inline): source-only inline: expansion temporaries */
inline void ParticleEmitter::RenderGround(Camera *view, s32 fwd)
{
    if (fwd)
        Emitter_RenderFlat_Fwd(view);
    else
        Emitter_RenderFlat(view);
}

inline void TrailEmitter::RenderGround(Camera *view, s32 fwd)
{
    if (fwd)
        base.Emitter_RenderFlat_Fwd(view);
    else
        base.Emitter_RenderFlat(view);
}

/* ---- more callees and globals (the message handler) ---- */
s32 Scenaric_FindByClass(u16 classId, ScnObject **out, s32 max); /* 0x5145c5 */
s32 Vec3s_DistSqXZ(Vec3s *a, Vec3s *b);                          /* 0x51588e */

/* ---- more callees and globals (the init code) ---- */
s32 Scenaric_FindByClass(u16 classId, ScnObject **out, s32 max); /* 0x5145c5 */

/* The profile tables are two objects of two rows each (normal ground, ice): a row other than the first is stored
 * through a register (mov eax,0x57afb0 at 0x48811f, mov edx,0x57b1e0 at 0x488257), the form VC6 gives an address
 * with an offset from its symbol, while the first rows are stored as immediates. So g_wolfMoveProfilesIce 0x57afb0 is
 * row 1 of g_wolfMoveProfilesNormal, and bank 1 has its own table at 0x57b1a8. */

/* A dword property of a scenaric record (record + 0x14 + offset: the designer properties, scenaric_props.h). The
 * offset parameter is unsigned: a constant int argument then needs a conversion, and VC6 gives it a stack temporary
 * (mov [ebp-0x28],8 at 0x48886a); an int parameter is folded into the displacement. */
/* BYTES(inline): source-only inline: the offset parameter is unsigned, so a constant int argument gets a stack temporary (mov [ebp-0x28],8 at 0x48886a) */
#define SDW_INLINE_FREE_SCN_GETPROPU32_U16_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_SCN_GETPROPU32_U16_U32

/* The boxes of an id list (a *BOXES property). */
#define SDW_INLINE_ZONELIST_LOAD_U32 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_LOAD_U32

/* 0x50d89e-style update policy (ScnUpdateMode): by distance, always, never, or also in cinematics. */
/* BYTES(inline): source-only inline: expansion temporaries */
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U32

/* The instance flags word (+4, the embedded render instance's first field) through a pointer (0x488085). */
#define SDW_INLINE_FREE_INSTFLAGS_SET_U16_U16 1
#include "../objects/instance_inlines.h"
#undef SDW_INLINE_FREE_INSTFLAGS_SET_U16_U16

#define SDW_INLINE_FREE_INSTFLAGS_CLEAR_U16_U16 1
#include "../objects/instance_inlines.h"
#undef SDW_INLINE_FREE_INSTFLAGS_CLEAR_U16_U16

/* INST_F_TINT, the tint override (0x48807f). */
#define SDW_INLINE_SCNOBJECT_SETTINTED_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETTINTED_S32

/* ---- data, in address order ---- */
/* .rdata 0x5750a0 - the five costume / prop model sets of the Wolf's own model (Wolf_Create) */
extern const u16 g_wolfAltModelIds[] = {WAR_IDO_AEXPLOS2, WAR_IDO_ABCOYO01, WAR_IDO_AMCOYO01, WAR_IDO_ACOLAP01,
                                        WAR_IDO_ACOGHO01};
/* 0x5750ac - the prop body's record model, then its eight prop models (Wolf_Init): one array, see the header */
extern const u16 g_wolfPropModelTable[9] = {WAR_IDO_AETOIL01, WAR_IDO_ASABLE01, WAR_IDO_ASABLE02,
                                            WAR_IDO_AGLACON1, WAR_IDO_AGLACON3, WAR_IDO_AFEUIL01,
                                            WAR_IDO_ACENSURE, WAR_IDO_ACBOUE01, WAR_IDO_AECLAI01};
#define g_wolfPropBaseModelId (g_wolfPropModelTable[0])
#define g_wolfPropModelIds (&g_wolfPropModelTable[1])
/* 0x5750c0 / 0x5757b8 - the state descriptors of bank 0 (on foot, 0xDF states) and bank 1 (carrying, 0x1D) */
extern const WolfStateDesc g_wolfStateDescriptors0[0xDF] = {
    /* 0x00 WOLF_ST_IDLE */
    {ACOYOT01_ANIM_STAND0, 0, 0, WMS_RALPH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_STATIONARY | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_ITEM_WHEEL_OK | WSF_CONTEXT_ACTIONS |
         WSF_LOOK_MODE_OK},
    /* 0x01 WOLF_ST_WALK */
    {ACOYOT01_ANIM_RUN1, 0, WOLF_CUE_WALK, WMS_RALPH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_ITEM_WHEEL_OK | WSF_CONTEXT_ACTIONS |
         WSF_LOOK_MODE_OK},
    /* 0x02 WOLF_ST_SKID */
    {ACOYOT01_ANIM_SLIDE3, 0, WOLF_CUE_WALK, WMS_RALPH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_ITEM_WHEEL_OK | WSF_CONTEXT_ACTIONS |
         WSF_LOOK_MODE_OK},
    /* 0x03 WOLF_ST_JUMP_START */
    {ACOYOT01_ANIM_JUMP0, 0, WOLF_CUE_JUMP, WMS_RALPH, CAM_FOLLOW, WSF_ITEM_WHEEL_OK | WSF_CONTEXT_ACTIONS},
    /* 0x04 WOLF_ST_JUMP_ASCEND */
    {ACOYOT01_ANIM_JUMP1, 0, WOLF_CUE_JUMP, WMS_RALPH, CAM_FOLLOW, WSF_ITEM_WHEEL_OK | WSF_CONTEXT_ACTIONS},
    /* 0x05 WOLF_ST_JUMP_APEX */
    {ACOYOT01_ANIM_JUMP2, 0, WOLF_CUE_JUMP, WMS_RALPH, CAM_FOLLOW,
     WSF_BUSY_OR_QUIET | WSF_CAM_LEDGE_PROBE | WSF_ITEM_WHEEL_OK | WSF_CONTEXT_ACTIONS | WSF_AIRBORNE},
    /* 0x06 WOLF_ST_FALL */
    {ACOYOT01_ANIM_FALL1, 0, WOLF_CUE_GLIDE, WMS_RALPH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_BUSY_OR_QUIET | WSF_CAM_LEDGE_PROBE | WSF_ITEM_WHEEL_OK | WSF_CONTEXT_ACTIONS | WSF_AIRBORNE},
    /* 0x07 WOLF_ST_LAND_IDLE */
    {ACOYOT01_ANIM_RECEPT2, 0, WOLF_CUE_WALK, WMS_RALPH, CAM_FOLLOW,
     WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_ITEM_WHEEL_OK | WSF_CONTEXT_ACTIONS | WSF_LOOK_MODE_OK},
    /* 0x08 WOLF_ST_LAND_MOVE */
    {ACOYOT01_ANIM_RECEPT1, 0, WOLF_CUE_WALK, WMS_RALPH, CAM_FOLLOW,
     WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_ITEM_WHEEL_OK | WSF_CONTEXT_ACTIONS | WSF_LOOK_MODE_OK},
    /* 0x09 WOLF_ST_SLOPE_SLIDE */
    {ACOYOT01_ANIM_SLIDE3, 0, WOLF_CUE_WALK, WMS_RALPH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_SLIDING | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_ITEM_WHEEL_OK},
    /* 0x0a WOLF_ST_SNEAK_IDLE */
    {ACOYOT01_ANIM_STAND11, 1, 0, WMS_RALPH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_STATIONARY | WSF_BUSY_OR_QUIET | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_ITEM_WHEEL_OK |
         WSF_CONTEXT_ACTIONS | WSF_LOOK_MODE_OK},
    /* 0x0b WOLF_ST_SNEAK_MOVE */
    {ACOYOT01_ANIM_TIPTOE1, 1, WOLF_CUE_RUN, WMS_RALPH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_BUSY_OR_QUIET | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_ITEM_WHEEL_OK |
         WSF_CONTEXT_ACTIONS | WSF_LOOK_MODE_OK},
    /* 0x0c WOLF_ST_DIE_FALL_PIT */
    {ACOYOT01_ANIM_STRIKE2, 0, 0, WMS_RALPH, CAM_REQ_DIR_HOLD, WSF_BODY_FLAG0 | WSF_AIRBORNE},
    /* 0x0d WOLF_ST_FALL_TO_DEATH */
    {ACOYOT01_ANIM_DEAD1, 0, WOLF_CUE_GLIDE, WMS_RALPH, CAM_FOLLOW, WSF_BODY_FLAG0},
    /* 0x0e WOLF_ST_DIE_FELL */
    {AEXPLOS2_ANIM_BOOM, 0, WOLF_CUE_GLIDE, WMS_EXPLOSION, CAM_FOLLOW, WSF_BODY_FLAG0},
    /* 0x0f WOLF_ST_DIE_KNOCKED */
    {ACOYOT01_ANIM_EXPLOS1, 0, 0, WMS_RALPH, CAM_REQ_DIR_HOLD,
     WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_IGNORE_INPUT_FREEZE | WSF_KILL_WHEN_DEAD},
    /* 0x10 WOLF_ST_DIE_CRUSHED */
    {ACOYOT01_ANIM_SQUASH3, 0, 0, WMS_RALPH, CAM_FOLLOW,
     WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_IGNORE_INPUT_FREEZE | WSF_BODY_FLAG0 | WSF_SKIP_BOX_TEST},
    /* 0x11 WOLF_ST_LIFT_END_M0 */
    {ACOYOT01_ANIM_TAKE2B, 0, WOLF_CUE_5, WMS_RALPH, CAM_FOLLOW,
     WSF_BUSY_OR_QUIET | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE},
    /* 0x12 WOLF_ST_PUTDOWN_M0 */
    {ACOYOT01_ANIM_PUT2A, 0, WOLF_CUE_6, WMS_RALPH, CAM_FOLLOW,
     WSF_BUSY_OR_QUIET | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_INTERACTING},
    /* 0x13 WOLF_ST_SCRIPT_WALK */
    {ACOYOT01_ANIM_RUN1, 0, WOLF_CUE_WALK, WMS_RALPH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_UNTOUCHABLE},
    /* 0x14 WOLF_ST_SCRIPT_TURN */
    {ACOYOT01_ANIM_STAND0, 0, 0, WMS_RALPH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_UNTOUCHABLE},
    /* 0x15 WOLF_ST_SCRIPT_PUTDOWN */
    {ACOYOT01_ANIM_PUT2A, 0, WOLF_CUE_6, WMS_RALPH, CAM_FOLLOW,
     WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_UNTOUCHABLE},
    /* 0x16 WOLF_ST_BOUNCED_UP */
    {ACOYOT01_ANIM_FALL1, 0, WOLF_CUE_JUMP, WMS_RALPH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_CAM_LEDGE_PROBE | WSF_ITEM_WHEEL_OK | WSF_CONTEXT_ACTIONS},
    /* 0x17 WOLF_ST_SKID_ICE */
    {ACOYOT01_ANIM_SLIDE5, 0, WOLF_CUE_WALK, WMS_RALPH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_ITEM_WHEEL_OK | WSF_CONTEXT_ACTIONS |
         WSF_LOOK_MODE_OK},
    /* 0x18 WOLF_ST_TRAJECTORY */
    {ACOYOT01_ANIM_SEESAW, 0, WOLF_CUE_GLIDE, WMS_RALPH, CAM_FOLLOW, WSF_LOOP_ANIM | WSF_CAM_LEDGE_PROBE},
    /* 0x19 WOLF_ST_GEYSER_SUCKED */
    {ACOYOT01_ANIM_TORNA1, 0, WOLF_CUE_SPIN, WMS_RALPH, CAM_FOLLOW,
     WSF_CAM_LEDGE_PROBE | WSF_BODY_FLAG0 | WSF_KEEP_COVER | WSF_UNTOUCHABLE},
    /* 0x1a WOLF_ST_GEYSER_EJECT */
    {ACOYOT01_ANIM_TORNA2, 0, WOLF_CUE_SPIN_END, WMS_RALPH, CAM_FOLLOW,
     WSF_CAM_LEDGE_PROBE | WSF_BODY_FLAG0 | WSF_KEEP_COVER | WSF_UNTOUCHABLE},
    /* 0x1b WOLF_ST_DIE_HOOVERED */
    {ACOYOT01_ANIM_TORNA1, 0, WOLF_CUE_SPIN, WMS_RALPH, CAM_FOLLOW, WSF_CAM_LEDGE_PROBE | WSF_KEEP_COVER},
    /* 0x1c WOLF_ST_KNOCKBACK */
    {ACOYOT01_ANIM_SEESAW, 0, 0, WMS_RALPH, CAM_FOLLOW, WSF_LOOP_ANIM | WSF_CAM_LEDGE_PROBE | WSF_SMOKE_TRAIL},
    /* 0x1d WOLF_ST_LIFT_START */
    {ACOYOT01_ANIM_TAKE2A, 0, WOLF_CUE_5, WMS_RALPH, CAM_FOLLOW,
     WSF_BUSY_OR_QUIET | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_CONTEXT_ACTIONS | WSF_INTERACTING},
    /* 0x1e WOLF_ST_PUTDOWN_RECOVER */
    {ACOYOT01_ANIM_PUT2B, 0, WOLF_CUE_6, WMS_RALPH, CAM_FOLLOW,
     WSF_BUSY_OR_QUIET | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE},
    /* 0x1f WOLF_ST_SCRIPT_PUTDOWN_RECOVER */
    {ACOYOT01_ANIM_PUT2B, 0, WOLF_CUE_6, WMS_RALPH, CAM_FOLLOW,
     WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_UNTOUCHABLE},
    /* 0x20 WOLF_ST_DOUBLEJUMP_ASCEND */
    {ACOYOT01_ANIM_JUMP3, 0, WOLF_CUE_JUMP, WMS_RALPH, CAM_FOLLOW, WSF_ITEM_WHEEL_OK | WSF_CONTEXT_ACTIONS},
    /* 0x21 WOLF_ST_DOUBLEJUMP_FALL */
    {ACOYOT01_ANIM_JUMP2, 0, WOLF_CUE_JUMP, WMS_RALPH, CAM_FOLLOW,
     WSF_BUSY_OR_QUIET | WSF_CAM_LEDGE_PROBE | WSF_ITEM_WHEEL_OK | WSF_CONTEXT_ACTIONS | WSF_AIRBORNE},
    /* 0x22 WOLF_ST_RUN_START */
    {ACOYOT01_ANIM_RUNF1, 2, WOLF_CUE_WALK, WMS_RALPH, CAM_FOLLOW_RUN, WSF_RUN_COLLISION | WSF_GROUND_STATE},
    /* 0x23 WOLF_ST_RUN */
    {ACOYOT01_ANIM_RUNF2, 2, WOLF_CUE_WALK, WMS_RALPH, CAM_FOLLOW_RUN,
     WSF_LOOP_ANIM | WSF_RUN_COLLISION | WSF_GROUND_STATE},
    /* 0x24 WOLF_ST_RUN_STOP */
    {ACOYOT01_ANIM_SLIDE1, 2, WOLF_CUE_WALK, WMS_RALPH, CAM_FOLLOW, WSF_RUN_COLLISION | WSF_GROUND_STATE},
    /* 0x25 WOLF_ST_RUN_CRASH */
    {ACOYOT01_ANIM_STRIKE1, 0, WOLF_CUE_WALK, WMS_RALPH, CAM_FOLLOW, WSF_GROUND_STATE},
    /* 0x26 WOLF_ST_RUN_AIR */
    {ACOYOT01_ANIM_RUNF2, 2, WOLF_CUE_WALK, WMS_RALPH, CAM_FOLLOW_RUN,
     WSF_LOOP_ANIM | WSF_RUN_COLLISION | WSF_GROUND_STATE},
    /* 0x27 WOLF_ST_PICKUP_ITEM */
    {ACOYOT01_ANIM_TAKE1A, 0, WOLF_CUE_5, WMS_RALPH, CAM_FOLLOW,
     WSF_BUSY_OR_QUIET | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_CONTEXT_ACTIONS | WSF_INTERACTING},
    /* 0x28 WOLF_ST_PICKUP_END */
    {ACOYOT01_ANIM_TAKE1B, 0, WOLF_CUE_5, WMS_RALPH, CAM_FOLLOW,
     WSF_BUSY_OR_QUIET | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE},
    /* 0x29 WOLF_ST_THROW_ITEM */
    {ACOYOT01_ANIM_PUT1A, 0, WOLF_CUE_6, WMS_RALPH, CAM_FOLLOW,
     WSF_BUSY_OR_QUIET | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_INTERACTING},
    /* 0x2a WOLF_ST_THROW_END */
    {ACOYOT01_ANIM_PUT1B, 0, WOLF_CUE_6, WMS_RALPH, CAM_FOLLOW,
     WSF_BUSY_OR_QUIET | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE},
    /* 0x2b WOLF_ST_ITEM_TAKE_OUT */
    {ACOYOT01_ANIM_TAKE4, 0, WOLF_CUE_5, WMS_RALPH, CAM_FOLLOW,
     WSF_BUSY_OR_QUIET | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_ITEM_WHEEL_OK},
    /* 0x2c WOLF_ST_ITEM_STOW */
    {ACOYOT01_ANIM_PUT4, 0, WOLF_CUE_6, WMS_RALPH, CAM_FOLLOW,
     WSF_BUSY_OR_QUIET | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_ITEM_WHEEL_OK},
    /* 0x2d WOLF_ST_RUN_RECOVER */
    {ACOYOT01_ANIM_STAND7, 0, 0, WMS_RALPH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_STATIONARY | WSF_BUSY_OR_QUIET | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_ITEM_WHEEL_OK |
         WSF_CONTEXT_ACTIONS | WSF_LOOK_MODE_OK},
    /* 0x2e WOLF_ST_PUSH_START */
    {ACOYOT01_ANIM_PUSH1A, 0, WOLF_CUE_WALK, WMS_RALPH, CAM_FOLLOW,
     WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_CONTEXT_ACTIONS},
    /* 0x2f WOLF_ST_PUSH */
    {ACOYOT01_ANIM_PUSH1B, 3, WOLF_CUE_WALK, WMS_RALPH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_CONTEXT_ACTIONS},
    /* 0x30 WOLF_ST_PUSH_END */
    {ACOYOT01_ANIM_PUSH1C, 0, WOLF_CUE_WALK, WMS_RALPH, CAM_FOLLOW,
     WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_CONTEXT_ACTIONS},
    /* 0x31 WOLF_ST_CAUGHT_BY_SAM */
    {ACOYOT01_ANIM_PUNCH7, 0, 0, WMS_RALPH, CAM_REQ_DIR_HOLD2, WSF_IGNORE_INPUT_FREEZE | WSF_SKIP_BOX_TEST},
    /* 0x32 WOLF_ST_DIE_SAM_PUNCH */
    {ACOYOT01_ANIM_DIEPNCH2, 0, 0, WMS_RALPH, CAM_REQ_DIR_HOLD3, WSF_IGNORE_INPUT_FREEZE | WSF_SKIP_BOX_TEST},
    /* 0x33 WOLF_ST_DIE_SAM_PUNCH_WINDUP */
    {ACOYOT01_ANIM_PUNCH3, 0, 0, WMS_RALPH, CAM_REQ_DIR_HOLD2, WSF_IGNORE_INPUT_FREEZE | WSF_SKIP_BOX_TEST},
    /* 0x34 WOLF_ST_DIE_SAM_PUNCH_HIT */
    {ACOYOT01_ANIM_PUNCH3, 0, 0, WMS_RALPH, CAM_REQ_DIR_HOLD3, WSF_IGNORE_INPUT_FREEZE | WSF_SKIP_BOX_TEST},
    /* 0x35 WOLF_ST_DIE_FALL_TO_SENDER */
    {ACOYOT01_ANIM_PUNCH6, 0, 0, WMS_RALPH, CAM_FOLLOW, WSF_IGNORE_INPUT_FREEZE},
    /* 0x36 WOLF_ST_SHRUG */
    {ACOYOT01_ANIM_STANDNO, 0, 0, WMS_RALPH, CAM_FOLLOW,
     WSF_STATIONARY | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_ITEM_WHEEL_OK},
    /* 0x37 WOLF_ST_DIE_ZONE */
    {ACOYOT01_ANIM_SAND1, 0, 0, WMS_RALPH, CAM_FOLLOW,
     WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_IGNORE_INPUT_FREEZE | WSF_BODY_FLAG0},
    /* 0x38 WOLF_ST_TALK_TURN */
    {ACOYOT01_ANIM_STAND0, 0, 0, WMS_RALPH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_STATIONARY | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_CONTEXT_ACTIONS},
    /* 0x39 WOLF_ST_UPDRAFT */
    {ACOYOT01_ANIM_GRAVIT1, 4, WOLF_CUE_GLIDE, WMS_RALPH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_CAM_LEDGE_PROBE | WSF_ITEM_WHEEL_OK | WSF_CONTEXT_ACTIONS | WSF_FLYING},
    /* 0x3a WOLF_ST_UPDRAFT_FAST */
    {ACOYOT01_ANIM_GRAVIT2, 5, WOLF_CUE_GLIDE, WMS_RALPH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_CAM_LEDGE_PROBE | WSF_ITEM_WHEEL_OK | WSF_FLYING},
    /* 0x3b WOLF_ST_DIE_BLOWN_AWAY */
    {ACOYOT01_ANIM_GRAVIT2, 4, WOLF_CUE_GLIDE, WMS_RALPH, CAM_FIXED_LOOKAT,
     WSF_LOOP_ANIM | WSF_IGNORE_INPUT_FREEZE | WSF_FLYING},
    /* 0x3c WOLF_ST_DIE_BLACKHOLE */
    {ACOYOT01_ANIM_TORNA1, 4, WOLF_CUE_GLIDE, WMS_RALPH, CAM_FOLLOW,
     WSF_CAM_LEDGE_PROBE | WSF_BODY_FLAG0 | WSF_KEEP_COVER | WSF_UNTOUCHABLE | WSF_FLYING},
    /* 0x3d WOLF_ST_ACTIVATE */
    {ACOYOT01_ANIM_LETTER1A, 0, 0, WMS_RALPH, CAM_FOLLOW,
     WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_CONTEXT_ACTIONS | WSF_INTERACTING},
    /* 0x3e WOLF_ST_ACTIVATE_END */
    {ACOYOT01_ANIM_LETTER1B, 0, 0, WMS_RALPH, CAM_FOLLOW, WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE},
    /* 0x3f WOLF_ST_CLIMB_ZONE */
    {ACOYOT01_ANIM_CLIMB1, 6, 0, WMS_RALPH, CAM_REQ_DIR_CLIMB, WSF_LOOP_ANIM | WSF_INTERACTING},
    /* 0x40 WOLF_ST_CLIMB_OBJECT */
    {ACOYOT01_ANIM_CLIMB1, 6, 0, WMS_RALPH, CAM_REQ_DIR_HIGH, WSF_LOOP_ANIM | WSF_CONTEXT_ACTIONS | WSF_INTERACTING},
    /* 0x41 WOLF_ST_CLIMBBOXWALK_IDLE */
    {ACOYOT01_ANIM_ELASTAND, 7, 0, WMS_RALPH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_STATIONARY | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_CONTEXT_ACTIONS | WSF_LOOK_MODE_OK |
         WSF_INTERACTING},
    /* 0x42 WOLF_ST_CLIMBBOXWALK_MOVE */
    {ACOYOT01_ANIM_ELASWALK, 7, 0, WMS_RALPH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_CONTEXT_ACTIONS | WSF_LOOK_MODE_OK | WSF_INTERACTING},
    /* 0x43 WOLF_ST_WADE */
    {ACOYOT01_ANIM_SWIM1, 8, WOLF_CUE_WALK, WMS_RALPH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_ITEM_WHEEL_OK | WSF_CONTEXT_ACTIONS | WSF_IN_WATER | WSF_WATER_COLLIDE},
    /* 0x44 WOLF_ST_SWIM */
    {ACOYOT01_ANIM_SWIM2, 8, WOLF_CUE_WALK, WMS_RALPH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_ITEM_WHEEL_OK | WSF_CONTEXT_ACTIONS | WSF_IN_WATER | WSF_WATER_COLLIDE |
         WSF_CAM_USE_WOLF_ANGLE},
    /* 0x45 WOLF_ST_WATER_JUMP */
    {ACOYOT01_ANIM_JUMP1, 8, WOLF_CUE_JUMP, WMS_RALPH, CAM_FOLLOW,
     WSF_ITEM_WHEEL_OK | WSF_CONTEXT_ACTIONS | WSF_IN_WATER},
    /* 0x46 WOLF_ST_WATER_JUMP_FALL */
    {ACOYOT01_ANIM_JUMP2, 8, WOLF_CUE_JUMP, WMS_RALPH, CAM_FOLLOW,
     WSF_BUSY_OR_QUIET | WSF_ITEM_WHEEL_OK | WSF_CONTEXT_ACTIONS | WSF_IN_WATER | WSF_AIRBORNE},
    /* 0x47 WOLF_ST_WATER_DOUBLEJUMP */
    {ACOYOT01_ANIM_JUMP3, 8, WOLF_CUE_JUMP, WMS_RALPH, CAM_FOLLOW,
     WSF_ITEM_WHEEL_OK | WSF_CONTEXT_ACTIONS | WSF_IN_WATER},
    /* 0x48 WOLF_ST_WATER_DOUBLEJUMP_FALL */
    {ACOYOT01_ANIM_JUMP2, 8, WOLF_CUE_JUMP, WMS_RALPH, CAM_FOLLOW,
     WSF_BUSY_OR_QUIET | WSF_ITEM_WHEEL_OK | WSF_CONTEXT_ACTIONS | WSF_IN_WATER | WSF_AIRBORNE},
    /* 0x49 WOLF_ST_WATER_PLUNGE */
    {ACOYOT01_ANIM_FALL1, 8, WOLF_CUE_GLIDE, WMS_RALPH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_BUSY_OR_QUIET | WSF_CAM_LEDGE_PROBE | WSF_IN_WATER | WSF_AIRBORNE},
    /* 0x4a WOLF_ST_FROZEN_FLOAT */
    {ACOYOT01_ANIM_FREEZE1, 8, 0, WMS_RALPH, CAM_FOLLOW, WSF_BODY_FLAG0 | WSF_IN_WATER | WSF_ICE_BLOCK},
    /* 0x4b WOLF_ST_FROZEN_FALL */
    {ACOYOT01_ANIM_FREEZE1, 0, 0, WMS_RALPH, CAM_FOLLOW,
     WSF_BUSY_OR_QUIET | WSF_CAM_LEDGE_PROBE | WSF_BODY_FLAG0 | WSF_IN_WATER | WSF_AIRBORNE | WSF_ICE_BLOCK},
    /* 0x4c WOLF_ST_DIE_FROZEN */
    {ACOYOT01_ANIM_SWIM1, 0, 0, WMS_RALPH, CAM_FOLLOW, WSF_IGNORE_INPUT_FREEZE | WSF_IN_WATER | WSF_WATER_COLLIDE},
    /* 0x4d WOLF_ST_FROZEN_GRABBED */
    {ACOYOT01_ANIM_FREEZE1, 8, 0, WMS_RALPH, CAM_FOLLOW, WSF_BODY_FLAG0 | WSF_IN_WATER | WSF_ICE_BLOCK},
    /* 0x4e WOLF_ST_FROZEN_GROUND */
    {ACOYOT01_ANIM_FREEZE1, 8, 0, WMS_RALPH, CAM_FOLLOW,
     WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_BODY_FLAG0 | WSF_ICE_BLOCK},
    /* 0x4f WOLF_ST_FROZEN_SHAKE */
    {ACOYOT01_ANIM_FREEZE4, 8, 0, WMS_RALPH, CAM_FOLLOW,
     WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_BODY_FLAG0 | WSF_ICE_BLOCK},
    /* 0x50 WOLF_ST_FROZEN_BREAK */
    {ACOYOT01_ANIM_FREEZE6, 8, 0, WMS_RALPH, CAM_FOLLOW,
     WSF_STATIONARY | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_ICE_BLOCK},
    /* 0x51 WOLF_ST_SHIVER */
    {ACOYOT01_ANIM_FREEZE3, 0, 0, WMS_RALPH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_STATIONARY | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE},
    /* 0x52 WOLF_ST_ROCKET_MOUNT */
    {ACOYOT01_ANIM_ROCKET0, 0, 0, WMS_RALPH, CAM_FIXED_LOOKAT, WSF_IGNORE_INPUT_FREEZE | WSF_ITEM_IN_USE | WSF_FLYING},
    /* 0x53 WOLF_ST_ROCKET_IGNITE */
    {ACOYOT01_ANIM_ROCKET2, 9, WOLF_CUE_JUMP, WMS_RALPH, CAM_FIXED_LOOKAT,
     WSF_IGNORE_INPUT_FREEZE | WSF_ITEM_IN_USE | WSF_FLYING},
    /* 0x54 WOLF_ST_ROCKET_FLY */
    {ACOYOT01_ANIM_ROCKET3, 9, WOLF_CUE_WALK, WMS_RALPH, CAM_ROCKET,
     WSF_LOOP_ANIM | WSF_NO_DISTANCE_COUNT | WSF_IGNORE_INPUT_FREEZE | WSF_ITEM_IN_USE | WSF_FLYING},
    /* 0x55 WOLF_ST_ROCKET_CRASH */
    {ACOYOT01_ANIM_STRIKE4, 0, 0, WMS_RALPH, CAM_ROCKET, WSF_IGNORE_INPUT_FREEZE | WSF_FLYING},
    /* 0x56 WOLF_ST_DAZED */
    {ACOYOT01_ANIM_ANVIL1, 0, 0, WMS_RALPH, CAM_FOLLOW, WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE},
    /* 0x57 WOLF_ST_DIE_SHARK */
    {ACOYOT01_ANIM_EAT, 0, WOLF_CUE_WALK, WMS_RALPH, CAM_FOLLOW,
     WSF_IGNORE_INPUT_FREEZE | WSF_SKIP_BOX_TEST | WSF_IN_WATER},
    /* 0x58 WOLF_ST_DIE_CROCODILE */
    {ACOYOT01_ANIM_EAT1, 0, WOLF_CUE_WALK, WMS_RALPH, CAM_FOLLOW,
     WSF_IGNORE_INPUT_FREEZE | WSF_SKIP_BOX_TEST | WSF_IN_WATER},
    /* 0x59 WOLF_ST_DIE_PIRANHAS */
    {ACOYOT01_ANIM_PIRANA1, 0, WOLF_CUE_WALK, WMS_RALPH, CAM_FOLLOW,
     WSF_IGNORE_INPUT_FREEZE | WSF_SKIP_BOX_TEST | WSF_IN_WATER},
    /* 0x5a WOLF_ST_FAN_IDLE */
    {ACOYOT01_ANIM_STANDV1, 10, 0, WMS_RALPH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_ITEM_IN_USE},
    /* 0x5b WOLF_ST_FAN_TURN */
    {ACOYOT01_ANIM_WALKV1, 10, WOLF_CUE_WALK, WMS_RALPH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_ITEM_IN_USE},
    /* 0x5c WOLF_ST_COVER_APPROACH */
    {ACOYOT01_ANIM_HIDE1, 0, 0, WMS_RALPH, CAM_REQ_DIR_COVER,
     WSF_BUSY_OR_QUIET | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_ITEM_WHEEL_OK | WSF_CONTEXT_ACTIONS},
    /* 0x5d WOLF_ST_COVER_ENTER */
    {ACOYOT01_ANIM_HIDE1, 0, 0, WMS_RALPH, CAM_REQ_DIR_COVER,
     WSF_BUSY_OR_QUIET | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_ITEM_WHEEL_OK | WSF_CONTEXT_ACTIONS |
         WSF_KEEP_COVER},
    /* 0x5e WOLF_ST_COVER_HIDDEN */
    {ACOYOT01_ANIM_HIDE2, 0, 0, WMS_RALPH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_BUSY_OR_QUIET | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_ITEM_WHEEL_OK |
         WSF_CONTEXT_ACTIONS | WSF_KEEP_COVER},
    /* 0x5f WOLF_ST_BUSH_PUT_ON */
    {ACOYOT01_ANIM_JUMP6, 0, 0, WMS_RALPH, CAM_FOLLOW, WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_CONTEXT_ACTIONS},
    /* 0x60 WOLF_ST_BUSH_RISE */
    {ABCOYO01_ANIM_HEAD1, 0, 0, WMS_BUSH, CAM_FOLLOW,
     WSF_STATIONARY | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_LOOK_MODE_OK},
    /* 0x61 WOLF_ST_BUSH_IDLE */
    {ABCOYO01_ANIM_STAND3, 0, 0, WMS_BUSH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_STATIONARY | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_LOOK_MODE_OK},
    /* 0x62 WOLF_ST_BUSH_WALK */
    {ABCOYO01_ANIM_WALK2, 0, WOLF_CUE_WALK, WMS_BUSH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_LOOK_MODE_OK},
    /* 0x63 WOLF_ST_BUSH_CROUCH */
    {ABCOYO01_ANIM_STAND4, 1, 0, WMS_BUSH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_STATIONARY | WSF_BUSY_OR_QUIET | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_KEEP_COVER |
         WSF_LOOK_MODE_OK},
    /* 0x64 WOLF_ST_BUSH_HIDDEN */
    {ABCOYO01_ANIM_STAND1, 1, 0, WMS_BUSH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_STATIONARY | WSF_BUSY_OR_QUIET | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_KEEP_COVER |
         WSF_SHEEP_NIBBLE | WSF_LOOK_MODE_OK},
    /* 0x65 WOLF_ST_BUSH_UNCROUCH */
    {ABCOYO01_ANIM_STAND2, 0, 0, WMS_BUSH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_STATIONARY | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_LOOK_MODE_OK},
    /* 0x66 WOLF_ST_BUSH_SNEAK */
    {ABCOYO01_ANIM_WALK1, 1, WOLF_CUE_RUN, WMS_BUSH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_BUSY_OR_QUIET | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_LOOK_MODE_OK},
    /* 0x67 WOLF_ST_BUSH_SLIDE */
    {ABCOYO01_ANIM_SLIDE1, 0, 0, WMS_BUSH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_SLIDING | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE},
    /* 0x68 WOLF_ST_BUSH_FALL */
    {ABCOYO01_ANIM_FALL1, 0, WOLF_CUE_GLIDE, WMS_BUSH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_BUSY_OR_QUIET | WSF_CAM_LEDGE_PROBE | WSF_AIRBORNE},
    /* 0x69 WOLF_ST_BUSH_JUMP_START */
    {ABCOYO01_ANIM_JUMP0, 0, WOLF_CUE_JUMP, WMS_BUSH, CAM_FOLLOW, 0},
    /* 0x6a WOLF_ST_BUSH_JUMP */
    {ABCOYO01_ANIM_JUMP1, 0, WOLF_CUE_JUMP, WMS_BUSH, CAM_FOLLOW, 0},
    /* 0x6b WOLF_ST_BUSH_JUMP_FALL */
    {ABCOYO01_ANIM_JUMP2, 0, WOLF_CUE_GLIDE, WMS_BUSH, CAM_FOLLOW,
     WSF_BUSY_OR_QUIET | WSF_CAM_LEDGE_PROBE | WSF_AIRBORNE},
    /* 0x6c WOLF_ST_BUSH_LAND */
    {ABCOYO01_ANIM_JUMP3, 0, 0, WMS_BUSH, CAM_FOLLOW, WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_LOOK_MODE_OK},
    /* 0x6d WOLF_ST_BUSH_TAKE_OFF */
    {ACOYOT01_ANIM_OUT6, 0, 0, WMS_RALPH, CAM_FOLLOW, WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE},
    /* 0x6e WOLF_ST_FLUTE_IDLE */
    {ACOYOT01_ANIM_FLUTE1, 11, 0, WMS_RALPH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_STATIONARY | WSF_BUSY_OR_QUIET | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_ITEM_IN_USE |
         WSF_LOOK_MODE_OK},
    /* 0x6f WOLF_ST_FLUTE_WALK */
    {ACOYOT01_ANIM_WALKF1, 11, 0, WMS_RALPH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_BUSY_OR_QUIET | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_ITEM_IN_USE | WSF_LOOK_MODE_OK},
    /* 0x70 WOLF_ST_ELASTIC_TAKE_BACK */
    {ACOYOT01_ANIM_TAKE1A, 0, WOLF_CUE_5, WMS_RALPH, CAM_FOLLOW,
     WSF_BUSY_OR_QUIET | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_CONTEXT_ACTIONS},
    /* 0x71 WOLF_ST_ELASTIC_GRAB */
    {ACOYOT01_ANIM_TAKE1A, 0, WOLF_CUE_5, WMS_RALPH, CAM_FOLLOW,
     WSF_BUSY_OR_QUIET | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_CONTEXT_ACTIONS},
    /* 0x72 WOLF_ST_ELASTIC_PULL_IDLE */
    {ACOYOT01_ANIM_DRAU1, 0, 0, WMS_RALPH, CAM_REQ_DIR_BEHIND,
     WSF_LOOP_ANIM | WSF_STATIONARY | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_KEEP_ELASTIC | WSF_LOOK_MODE_OK},
    /* 0x73 WOLF_ST_ELASTIC_PULL_MOVE */
    {ACOYOT01_ANIM_DRAU2, 0, WOLF_CUE_WALK, WMS_RALPH, CAM_REQ_DIR_BEHIND,
     WSF_LOOP_ANIM | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_KEEP_ELASTIC | WSF_LOOK_MODE_OK},
    /* 0x74 WOLF_ST_DETECTOR_START */
    {ACOYOT01_ANIM_DETEC3, 12, 0, WMS_RALPH, CAM_FOLLOW, WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_ITEM_IN_USE},
    /* 0x75 WOLF_ST_DETECTOR_IDLE */
    {ACOYOT01_ANIM_DETEC1, 12, 0, WMS_RALPH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_STATIONARY | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_ITEM_IN_USE | WSF_LOOK_MODE_OK},
    /* 0x76 WOLF_ST_DETECTOR_WALK */
    {ACOYOT01_ANIM_DETEC2, 12, WOLF_CUE_WALK, WMS_RALPH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_ITEM_IN_USE | WSF_LOOK_MODE_OK},
    /* 0x77 WOLF_ST_DETECTOR_END */
    {ACOYOT01_ANIM_DETEC4, 0, 0, WMS_RALPH, CAM_FOLLOW, WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE},
    /* 0x78 WOLF_ST_UMBRELLA_OPEN */
    {ACOYOT01_ANIM_UMBREL2, 0, 0, WMS_RALPH, CAM_FOLLOW, WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_ITEM_IN_USE},
    /* 0x79 WOLF_ST_UMBRELLA_IDLE */
    {ACOYOT01_ANIM_UMBREL3, 0, 0, WMS_RALPH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_STATIONARY | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_ITEM_IN_USE | WSF_LOOK_MODE_OK},
    /* 0x7a WOLF_ST_UMBRELLA_WALK */
    {ACOYOT01_ANIM_UMBREL9, 0, WOLF_CUE_WALK, WMS_RALPH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_ITEM_IN_USE | WSF_LOOK_MODE_OK},
    /* 0x7b WOLF_ST_UMBRELLA_CLOSE */
    {ACOYOT01_ANIM_UMBREL4, 0, 0, WMS_RALPH, CAM_FOLLOW, WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE},
    /* 0x7c WOLF_ST_UMBRELLA_GLIDE */
    {ACOYOT01_ANIM_UMBREL5, 0, WOLF_CUE_GLIDE, WMS_RALPH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_BUSY_OR_QUIET | WSF_CAM_FIXED_PITCH | WSF_ITEM_IN_USE | WSF_AIRBORNE},
    /* 0x7d WOLF_ST_UMBRELLA_JUMP_START */
    {ACOYOT01_ANIM_UMBREL10, 0, WOLF_CUE_JUMP_START, WMS_RALPH, CAM_FOLLOW, WSF_CAM_LEDGE_PROBE | WSF_ITEM_IN_USE},
    /* 0x7e WOLF_ST_UMBRELLA_JUMP */
    {ACOYOT01_ANIM_UMBREL11, 0, WOLF_CUE_JUMP, WMS_RALPH, CAM_FOLLOW, WSF_ITEM_IN_USE},
    /* 0x7f WOLF_ST_UMBRELLA_AIR_HOP */
    {ACOYOT01_ANIM_UMBREL8, 0, WOLF_CUE_AIR_HOP, WMS_RALPH, CAM_FOLLOW, WSF_CAM_FIXED_PITCH | WSF_ITEM_IN_USE},
    /* 0x80 WOLF_ST_UMBRELLA_HOP_FALL */
    {ACOYOT01_ANIM_UMBREL1, 0, WOLF_CUE_HOP_FALL, WMS_RALPH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_BUSY_OR_QUIET | WSF_CAM_FIXED_PITCH | WSF_ITEM_IN_USE | WSF_AIRBORNE},
    /* 0x81 WOLF_ST_UMBRELLA_OPEN_AIR */
    {ACOYOT01_ANIM_UMBREL6, 0, WOLF_CUE_GLIDE, WMS_RALPH, CAM_FOLLOW,
     WSF_BUSY_OR_QUIET | WSF_CAM_FIXED_PITCH | WSF_ITEM_IN_USE | WSF_AIRBORNE},
    /* 0x82 WOLF_ST_UMBRELLA_CLOSE_AIR */
    {ACOYOT01_ANIM_UMBREL7, 0, WOLF_CUE_GLIDE, WMS_RALPH, CAM_FOLLOW,
     WSF_BUSY_OR_QUIET | WSF_CAM_FIXED_PITCH | WSF_AIRBORNE},
    /* 0x83 WOLF_ST_SHEEPCOSTUME_PUT_ON */
    {ACOYOT01_ANIM_STAND0, 0, 0, WMS_RALPH, CAM_FOLLOW, WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE},
    /* 0x84 WOLF_ST_SHEEPCOSTUME_APPEAR */
    {AMCOYO01_ANIM_STAND1, 13, 0, WMS_SHEEPCOSTUME, CAM_FOLLOW, WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE},
    /* 0x85 WOLF_ST_SHEEPCOSTUME_IDLE */
    {AMCOYO01_ANIM_STAND1, 13, 0, WMS_SHEEPCOSTUME, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_STATIONARY | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_ITEM_WHEEL_OK | WSF_LOOK_MODE_OK},
    /* 0x86 WOLF_ST_SHEEPCOSTUME_WALK */
    {AMCOYO01_ANIM_WALK1, 13, 0, WMS_SHEEPCOSTUME, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_LOOK_MODE_OK},
    /* 0x87 WOLF_ST_SHEEPCOSTUME_JUMP_START */
    {AMCOYO01_ANIM_JUMP0, 13, 0, WMS_SHEEPCOSTUME, CAM_FOLLOW, 0},
    /* 0x88 WOLF_ST_SHEEPCOSTUME_JUMP */
    {AMCOYO01_ANIM_JUMP1, 13, 0, WMS_SHEEPCOSTUME, CAM_FOLLOW, 0},
    /* 0x89 WOLF_ST_SHEEPCOSTUME_JUMP_FALL */
    {AMCOYO01_ANIM_JUMP2, 13, 0, WMS_SHEEPCOSTUME, CAM_FOLLOW, WSF_BUSY_OR_QUIET | WSF_CAM_LEDGE_PROBE | WSF_AIRBORNE},
    /* 0x8a WOLF_ST_SHEEPCOSTUME_FALL */
    {AMCOYO01_ANIM_FALL1, 13, 0, WMS_SHEEPCOSTUME, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_BUSY_OR_QUIET | WSF_CAM_LEDGE_PROBE | WSF_AIRBORNE},
    /* 0x8b WOLF_ST_SHEEPCOSTUME_SLIDE */
    {AMCOYO01_ANIM_SLIDE1, 13, 0, WMS_SHEEPCOSTUME, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_SLIDING | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE},
    /* 0x8c WOLF_ST_SHEEPCOSTUME_BLEAT */
    {AMCOYO01_ANIM_TALK1, 13, 0, WMS_SHEEPCOSTUME, CAM_FOLLOW, WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE},
    /* 0x8d WOLF_ST_SHEEPCOSTUME_GRABBED */
    {AMCOYO01_ANIM_CARRY1B, 13, 0, WMS_SHEEPCOSTUME, CAM_FOLLOW, WSF_BODY_FLAG0 | WSF_SKIP_BOX_TEST},
    /* 0x8e WOLF_ST_SHEEPCOSTUME_DIE_THROWN */
    {AMCOYO01_ANIM_KICK, 13, 0, WMS_SHEEPCOSTUME, CAM_REQ_DIR_HOLD, WSF_IGNORE_INPUT_FREEZE | WSF_SKIP_BOX_TEST},
    /* 0x8f WOLF_ST_SHEEPCOSTUME_DIE_LANDED */
    {AMCOYO01_ANIM_KICK, 13, 0, WMS_SHEEPCOSTUME, CAM_REQ_DIR_HOLD, WSF_IGNORE_INPUT_FREEZE | WSF_SKIP_BOX_TEST},
    /* 0x90 WOLF_ST_SHEEPCOSTUME_TAKE_OFF */
    {AMCOYO01_ANIM_STAND1, 13, 0, WMS_SHEEPCOSTUME, CAM_FOLLOW, WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE},
    /* 0x91 WOLF_ST_COSTUME_OFF_END */
    {ACOYOT01_ANIM_STAND0, 0, 0, WMS_RALPH, CAM_FOLLOW, WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE},
    /* 0x92 WOLF_ST_WOLFTRAP_ENTER */
    {ACOYOT01_ANIM_TAKE1A, 0, WOLF_CUE_5, WMS_RALPH, CAM_FOLLOW,
     WSF_BUSY_OR_QUIET | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_CONTEXT_ACTIONS},
    /* 0x93 WOLF_ST_WOLFTRAP_START */
    {ACOYOT01_ANIM_TRAP0, 0, 0, WMS_RALPH, CAM_FOLLOW, WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE},
    /* 0x94 WOLF_ST_WOLFTRAP_LOOP */
    {ACOYOT01_ANIM_TRAP1, 0, 0, WMS_RALPH, CAM_FOLLOW, WSF_LOOP_ANIM | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE},
    /* 0x95 WOLF_ST_WOLFTRAP_REACT */
    {ACOYOT01_ANIM_TRAP2, 0, 0, WMS_RALPH, CAM_FOLLOW, WSF_LOOP_ANIM | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE},
    /* 0x96 WOLF_ST_WOLFTRAP_EXIT */
    {ACOYOT01_ANIM_TRAP3, 0, 0, WMS_RALPH, CAM_FOLLOW, WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE},
    /* 0x97 WOLF_ST_GOSSAMER_GRAB_0 */
    {ACOYOT01_ANIM_CATCH, 0, 0, WMS_RALPH, CAM_FOLLOW, WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE},
    /* 0x98 WOLF_ST_GOSSAMER_GRAB_1 */
    {ACOYOT01_ANIM_CATCH1, 0, 0, WMS_RALPH, CAM_FOLLOW, WSF_LOOP_ANIM | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE},
    /* 0x99 WOLF_ST_GOSSAMER_GRAB_2 */
    {ACOYOT01_ANIM_CATCH2, 0, 0, WMS_RALPH, CAM_FOLLOW, WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE},
    /* 0x9a WOLF_ST_GOSSAMER_GRAB_3 */
    {ACOYOT01_ANIM_CATCH3, 0, 0, WMS_RALPH, CAM_FOLLOW, WSF_LOOP_ANIM | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE},
    /* 0x9b WOLF_ST_GOSSAMER_RELEASED */
    {ACOYOT01_ANIM_CATCH4, 0, 0, WMS_RALPH, CAM_FOLLOW, WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE},
    /* 0x9c WOLF_ST_TIMEMACHINE_USE */
    {ACOYOT01_ANIM_CHRONO1, 0, 9, WMS_RALPH, CAM_FOLLOW,
     WSF_BUSY_OR_QUIET | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_KEEP_COVER | WSF_UNTOUCHABLE},
    /* 0x9d WOLF_ST_TIMEMACHINE_DEPART */
    {ACOYOT01_ANIM_CHRONO2, 0, 0, WMS_RALPH, CAM_FOLLOW,
     WSF_BUSY_OR_QUIET | WSF_GROUND_STATE | WSF_BODY_FLAG0 | WSF_SKIP_BOX_TEST | WSF_KEEP_COVER | WSF_UNTOUCHABLE},
    /* 0x9e WOLF_ST_TIMEMACHINE_ARRIVE */
    {ACOYOT01_ANIM_CHRONO3, 0, 0, WMS_RALPH, CAM_FOLLOW,
     WSF_BUSY_OR_QUIET | WSF_GROUND_STATE | WSF_KEEP_COVER | WSF_UNTOUCHABLE},
    /* 0x9f WOLF_ST_BURNT */
    {ACOYOT01_ANIM_CRAME1, 0, 0, WMS_RALPH, CAM_FOLLOW, WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE},
    /* 0xa0 WOLF_ST_RABBITCOSTUME_PUT_ON */
    {ACOYOT01_ANIM_STAND0, 0, 0, WMS_RALPH, CAM_FOLLOW, WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE},
    /* 0xa1 WOLF_ST_RABBITCOSTUME_ON_END */
    {ACOYOT01_ANIM_STAND0, 0, 0, WMS_RALPH, CAM_FOLLOW, WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE},
    /* 0xa2 WOLF_ST_RABBITCOSTUME_TAKE_OFF */
    {ACOYOT01_ANIM_STAND0, 0, 0, WMS_RALPH, CAM_FOLLOW, WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE},
    /* 0xa3 WOLF_ST_RABBITCOSTUME_OFF_END */
    {ACOYOT01_ANIM_STAND0, 0, 0, WMS_RALPH, CAM_FOLLOW, WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE},
    /* 0xa4 WOLF_ST_REMOTE_PRESS */
    {ACOYOT01_ANIM_BUTTON1, 0, 0, WMS_RALPH, CAM_FOLLOW, WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE},
    /* 0xa5 WOLF_ST_REMOTE_START */
    {ACOYOT01_ANIM_BUTTON1, 0, 0, WMS_RALPH, CAM_FOLLOW, WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE},
    /* 0xa6 WOLF_ST_REMOTE_DRIVE */
    {ACOYOT01_ANIM_STAND10, 0, 0, WMS_RALPH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_ITEM_IN_USE},
    /* 0xa7 WOLF_ST_REMOTE_END */
    {ACOYOT01_ANIM_BUTTON1, 0, 0, WMS_RALPH, CAM_FOLLOW, WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_ITEM_IN_USE},
    /* 0xa8 WOLF_ST_DIE_ZAPPED_REMOTE */
    {ACOYOT01_ANIM_ELECTR1, 0, 0, WMS_RALPH, CAM_REQ_DIR_HOLD, WSF_LOOP_ANIM | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE},
    /* 0xa9 WOLF_ST_DIE_CANNONBALL */
    {ACOYOT01_ANIM_ANVIL1, 0, 0, WMS_RALPH, CAM_FOLLOW,
     WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_IGNORE_INPUT_FREEZE | WSF_KILL_WHEN_DEAD},
    /* 0xaa WOLF_ST_USE_CTX1A */
    {ACOYOT01_ANIM_PANNEAU1, 0, 0, WMS_RALPH, CAM_FOLLOW, WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_CONTEXT_ACTIONS},
    /* 0xab WOLF_ST_USE_CTX1A_END */
    {ACOYOT01_ANIM_PANNEAU2, 0, 0, WMS_RALPH, CAM_FOLLOW, WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE},
    /* 0xac WOLF_ST_FISHING_CAST */
    {ACOYOT01_ANIM_FISH1, 0, 0, WMS_RALPH, CAM_REQ_DIR_FRONT, WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_ITEM_IN_USE},
    /* 0xad WOLF_ST_FISHING_IDLE */
    {ACOYOT01_ANIM_FISH2, 14, 0, WMS_RALPH, CAM_REQ_DIR_FRONT,
     WSF_LOOP_ANIM | WSF_STATIONARY | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_ITEM_IN_USE | WSF_LOOK_MODE_OK},
    /* 0xae WOLF_ST_FISHING_STEP_A */
    {ACOYOT01_ANIM_FISH3, 14, WOLF_CUE_FISH_A, WMS_RALPH, CAM_REQ_DIR_FRONT,
     WSF_LOOP_ANIM | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_ITEM_IN_USE | WSF_LOOK_MODE_OK},
    /* 0xaf WOLF_ST_FISHING_STEP_B */
    {ACOYOT01_ANIM_FISH4, 14, WOLF_CUE_FISH_B, WMS_RALPH, CAM_REQ_DIR_FRONT,
     WSF_LOOP_ANIM | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_ITEM_IN_USE | WSF_LOOK_MODE_OK},
    /* 0xb0 WOLF_ST_FISHING_REEL */
    {ACOYOT01_ANIM_FISH5, 14, WOLF_CUE_FISH_C, WMS_RALPH, CAM_REQ_DIR_FRONT,
     WSF_LOOP_ANIM | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_ITEM_IN_USE | WSF_LOOK_MODE_OK},
    /* 0xb1 WOLF_ST_FISHING_END */
    {ACOYOT01_ANIM_FISH6, 0, 0, WMS_RALPH, CAM_FOLLOW, WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE},
    /* 0xb2 WOLF_ST_GHOSTCOSTUME_PUT_ON */
    {ACOYOT01_ANIM_STAND0, 0, 0, WMS_RALPH, CAM_FOLLOW, WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE},
    /* 0xb3 WOLF_ST_GHOSTCOSTUME_APPEAR */
    {ACOGHO01_ANIM_STAND1, 15, 0, WMS_GHOSTCOSTUME, CAM_FOLLOW, WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE},
    /* 0xb4 WOLF_ST_GHOSTCOSTUME_IDLE */
    {ACOGHO01_ANIM_STAND1, 15, 0, WMS_GHOSTCOSTUME, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_STATIONARY | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_ITEM_WHEEL_OK | WSF_LOOK_MODE_OK},
    /* 0xb5 WOLF_ST_GHOSTCOSTUME_WALK */
    {ACOGHO01_ANIM_RUN, 15, 0, WMS_GHOSTCOSTUME, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_LOOK_MODE_OK},
    /* 0xb6 WOLF_ST_GHOSTCOSTUME_JUMP_START */
    {ACOGHO01_ANIM_JUMP0, 15, WOLF_CUE_JUMP, WMS_GHOSTCOSTUME, CAM_FOLLOW, 0},
    /* 0xb7 WOLF_ST_GHOSTCOSTUME_JUMP */
    {ACOGHO01_ANIM_JUMP1, 15, WOLF_CUE_JUMP, WMS_GHOSTCOSTUME, CAM_FOLLOW, 0},
    /* 0xb8 WOLF_ST_GHOSTCOSTUME_JUMP_FALL */
    {ACOGHO01_ANIM_JUMP2, 15, WOLF_CUE_GLIDE, WMS_GHOSTCOSTUME, CAM_FOLLOW,
     WSF_BUSY_OR_QUIET | WSF_CAM_LEDGE_PROBE | WSF_AIRBORNE},
    /* 0xb9 WOLF_ST_GHOSTCOSTUME_LAND */
    {ACOGHO01_ANIM_JUMP3, 15, 0, WMS_GHOSTCOSTUME, CAM_FOLLOW,
     WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_LOOK_MODE_OK},
    /* 0xba WOLF_ST_GHOSTCOSTUME_FALL */
    {ACOGHO01_ANIM_FALL, 15, 0, WMS_GHOSTCOSTUME, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_BUSY_OR_QUIET | WSF_CAM_LEDGE_PROBE | WSF_AIRBORNE},
    /* 0xbb WOLF_ST_GHOSTCOSTUME_SLIDE */
    {ACOGHO01_ANIM_SLIDE, 15, 0, WMS_GHOSTCOSTUME, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_SLIDING | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE},
    /* 0xbc WOLF_ST_GHOSTCOSTUME_SNEAK */
    {ACOGHO01_ANIM_TIPTOE, 1, 0, WMS_GHOSTCOSTUME, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_BUSY_OR_QUIET | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_LOOK_MODE_OK},
    /* 0xbd WOLF_ST_GHOSTCOSTUME_BOO */
    {ACOGHO01_ANIM_HOO, 15, 0, WMS_GHOSTCOSTUME, CAM_FOLLOW, WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE},
    /* 0xbe WOLF_ST_GHOSTCOSTUME_LIFTED */
    {ACOGHO01_ANIM_CATCH1, 15, 0, WMS_GHOSTCOSTUME, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_BUSY_OR_QUIET | WSF_CAM_LEDGE_PROBE | WSF_AIRBORNE},
    /* 0xbf WOLF_ST_GHOSTCOSTUME_CAPTURED */
    {ACOGHO01_ANIM_FEAR, 15, 0, WMS_GHOSTCOSTUME, CAM_FOLLOW, WSF_LOOP_ANIM | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE},
    /* 0xc0 WOLF_ST_DANCE_GOTO */
    {ACOGHO01_ANIM_RUN, 15, 0, WMS_GHOSTCOSTUME, CAM_FOLLOW, WSF_LOOP_ANIM | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE},
    /* 0xc1 WOLF_ST_DANCE */
    {ACOGHO01_ANIM_DANCE5, 15, 0, WMS_GHOSTCOSTUME, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_STATIONARY | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE},
    /* 0xc2 WOLF_ST_GHOSTCOSTUME_TAKE_OFF */
    {ACOGHO01_ANIM_STAND1, 15, 0, WMS_GHOSTCOSTUME, CAM_FOLLOW, WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE},
    /* 0xc3 WOLF_ST_GHOSTCOSTUME_OFF_END */
    {ACOYOT01_ANIM_STAND0, 0, 0, WMS_RALPH, CAM_FOLLOW, WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE},
    /* 0xc4 WOLF_ST_GHOST_LIFTED */
    {ACOYOT01_ANIM_LEVIT1, 0, 0, WMS_RALPH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_BUSY_OR_QUIET | WSF_CAM_LEDGE_PROBE | WSF_AIRBORNE},
    /* 0xc5 WOLF_ST_INFLATE_START */
    {ACOYOT01_ANIM_INFLAT0, 0, 0, WMS_RALPH, CAM_FOLLOW, WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_ITEM_IN_USE},
    /* 0xc6 WOLF_ST_INFLATE */
    {ACOYOT01_ANIM_INFLAT1, 0, 0, WMS_RALPH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_ITEM_IN_USE},
    /* 0xc7 WOLF_ST_HIT_BY_BIPBIP */
    {ACOYOT01_ANIM_SPIN1, 0, 0, WMS_RALPH, CAM_FOLLOW, WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE},
    /* 0xc8 WOLF_ST_MAGNET_DRAG_IDLE */
    {ACOYOT01_ANIM_MAGNET1, 0, 0, WMS_RALPH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_STATIONARY | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_ITEM_WHEEL_OK | WSF_LOOK_MODE_OK |
         WSF_MAGNET_DRAG},
    /* 0xc9 WOLF_ST_MAGNET_DRAG_MOVE */
    {ACOYOT01_ANIM_MAGNET2, 0, 0, WMS_RALPH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_ITEM_WHEEL_OK | WSF_MAGNET_DRAG},
    /* 0xca WOLF_ST_HOOVER_HOLD */
    {ACOYOT01_ANIM_STANDV1, 10, 0, WMS_RALPH, CAM_FOLLOW, WSF_LOOP_ANIM | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE},
    /* 0xcb WOLF_ST_DIE_RAFT */
    {ACOYOT01_ANIM_SINK1, 0, 0, WMS_RALPH, CAM_FOLLOW, WSF_IGNORE_INPUT_FREEZE | WSF_SKIP_BOX_TEST},
    /* 0xcc WOLF_ST_DIE_PIRATE */
    {ACOYOT01_ANIM_SHOOT1, 0, 0, WMS_RALPH, CAM_FOLLOW,
     WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_IGNORE_INPUT_FREEZE},
    /* 0xcd WOLF_ST_PANIC_RUN_BEES */
    {ACOYOT01_ANIM_BEERUN1, 16, 0, WMS_RALPH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_CAM_LEDGE_PROBE | WSF_RUN_COLLISION | WSF_GROUND_STATE | WSF_IGNORE_INPUT_FREEZE},
    /* 0xce WOLF_ST_DIE_MARTIAN */
    {ACOYOT01_ANIM_ASSDEAD1, 0, 0, WMS_RALPH, CAM_FOLLOW,
     WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_IGNORE_INPUT_FREEZE | WSF_BODY_FLAG0 | WSF_SKIP_BOX_TEST},
    /* 0xcf WOLF_ST_PANIC_RUN_BURNT */
    {ACOYOT01_ANIM_BEERUN1, 16, 0, WMS_RALPH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_CAM_LEDGE_PROBE | WSF_RUN_COLLISION | WSF_GROUND_STATE | WSF_IGNORE_INPUT_FREEZE |
         WSF_SMOKE_TRAIL},
    /* 0xd0 WOLF_ST_DEFUSE_MINE */
    {ACOYOT01_ANIM_MINE, 0, WOLF_CUE_5, WMS_RALPH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_BUSY_OR_QUIET | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_INTERACTING},
    /* 0xd1 WOLF_ST_TIMEKEEPER_USE */
    {ACOYOT01_ANIM_CLOCK2, 0, 0, WMS_RALPH, CAM_FOLLOW, WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_INTERACTING},
    /* 0xd2 WOLF_ST_PUSH_ICECUBE */
    {ACOYOT01_ANIM_ICEPUSH1, 0, 0, WMS_RALPH, CAM_FOLLOW,
     WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_CONTEXT_ACTIONS | WSF_INTERACTING},
    /* 0xd3 WOLF_ST_KICK_ICECUBE_END */
    {ACOYOT01_ANIM_ICEPUSH2, 0, 0, WMS_RALPH, CAM_FOLLOW, WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE},
    /* 0xd4 WOLF_ST_FLATTENED_START */
    {ACOYOT01_ANIM_SQUAPNCH, 17, 0, WMS_RALPH, CAM_FOLLOW, WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_FLATTENED},
    /* 0xd5 WOLF_ST_FLATTENED_IDLE */
    {ACOYOT01_ANIM_SQUASTD, 17, 0, WMS_RALPH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_STATIONARY | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_FLATTENED},
    /* 0xd6 WOLF_ST_FLATTENED_WALK */
    {ACOYOT01_ANIM_SQUARUN, 17, 0, WMS_RALPH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_FLATTENED},
    /* 0xd7 WOLF_ST_FLATTENED_FALL */
    {ACOYOT01_ANIM_SQUAFALL, 17, 0, WMS_RALPH, CAM_FOLLOW, WSF_LOOP_ANIM | WSF_CAM_LEDGE_PROBE | WSF_FLATTENED},
    /* 0xd8 WOLF_ST_FLATTENED_KNOCKBACK */
    {ACOYOT01_ANIM_SQUAKICK, 2, 0, WMS_RALPH, CAM_FOLLOW, WSF_CAM_LEDGE_PROBE | WSF_FLATTENED},
    /* 0xd9 WOLF_ST_FLATTENED_RECOVER */
    {ACOYOT01_ANIM_SQUAJUMP, 17, 0, WMS_RALPH, CAM_FOLLOW, WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE},
    /* 0xda WOLF_ST_GHOST_CAPTURED */
    {ACOYOT01_ANIM_FEAR, 0, 0, WMS_RALPH, CAM_FOLLOW, WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_GHOST_CAPTURED},
    /* 0xdb WOLF_ST_FROZEN_GEYSER */
    {ACOYOT01_ANIM_WHIRL1, 0, 0, WMS_RALPH, CAM_FOLLOW, WSF_LOOP_ANIM | WSF_BODY_FLAG0 | WSF_IN_WATER | WSF_ICE_BLOCK},
    /* 0xdc WOLF_ST_ENTER_LEVEL_DOOR */
    {ACOYOT01_ANIM_OPEN, 0, 0, WMS_RALPH, CAM_FOLLOW, WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_INTERACTING},
    /* 0xdd WOLF_ST_DIE_ZAPPED */
    {ACOYOT01_ANIM_TOAST, 0, 0, WMS_RALPH, CAM_REQ_DIR_HOLD,
     WSF_LOOP_ANIM | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_IGNORE_INPUT_FREEZE},
    /* 0xde WOLF_ST_CATAPULT_OPERATE */
    {ACOYOT01_ANIM_STAND0, 0, 0, WMS_RALPH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_BUSY_OR_QUIET | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_INTERACTING},
};
extern const WolfStateDesc g_wolfStateTable1[0x1D] = {
    /* 0x00 WOLF_ST_IDLE */
    {ACOYOT01_ANIM_STAND8, 0, 0, WMS_RALPH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_STATIONARY | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_CONTEXT_ACTIONS | WSF_LOOK_MODE_OK},
    /* 0x01 WOLF_ST_WALK */
    {ACOYOT01_ANIM_RUN2, 0, WOLF_CUE_WALK, WMS_RALPH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_CONTEXT_ACTIONS | WSF_LOOK_MODE_OK},
    /* 0x02 WOLF_ST_SKID */
    {ACOYOT01_ANIM_SLIDE4, 0, WOLF_CUE_WALK, WMS_RALPH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_CONTEXT_ACTIONS | WSF_LOOK_MODE_OK},
    /* 0x03 WOLF_ST_JUMP_START */
    {ACOYOT01_ANIM_JUMP0B, 0, WOLF_CUE_JUMP, WMS_RALPH, CAM_FOLLOW, 0},
    /* 0x04 WOLF_ST_JUMP_ASCEND */
    {ACOYOT01_ANIM_JUMP4, 0, WOLF_CUE_JUMP, WMS_RALPH, CAM_FOLLOW, 0},
    /* 0x05 WOLF_ST_JUMP_APEX */
    {ACOYOT01_ANIM_JUMP5, 0, WOLF_CUE_JUMP, WMS_RALPH, CAM_FOLLOW,
     WSF_BUSY_OR_QUIET | WSF_CAM_LEDGE_PROBE | WSF_AIRBORNE},
    /* 0x06 WOLF_ST_FALL */
    {ACOYOT01_ANIM_FALL2, 0, WOLF_CUE_GLIDE, WMS_RALPH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_BUSY_OR_QUIET | WSF_CAM_LEDGE_PROBE | WSF_AIRBORNE},
    /* 0x07 WOLF_ST_LAND_IDLE */
    {ACOYOT01_ANIM_RECEPT4, 0, WOLF_CUE_WALK, WMS_RALPH, CAM_FOLLOW,
     WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_CONTEXT_ACTIONS | WSF_LOOK_MODE_OK},
    /* 0x08 WOLF_ST_LAND_MOVE */
    {ACOYOT01_ANIM_RECEPT3, 0, WOLF_CUE_WALK, WMS_RALPH, CAM_FOLLOW,
     WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_CONTEXT_ACTIONS | WSF_LOOK_MODE_OK},
    /* 0x09 WOLF_ST_SLOPE_SLIDE */
    {ACOYOT01_ANIM_SLIDE4, 0, WOLF_CUE_WALK, WMS_RALPH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_SLIDING | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE},
    /* 0x0a WOLF_ST_SNEAK_IDLE */
    {ACOYOT01_ANIM_STAND8, 1, 0, WMS_RALPH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_STATIONARY | WSF_BUSY_OR_QUIET | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_CONTEXT_ACTIONS |
         WSF_LOOK_MODE_OK},
    /* 0x0b WOLF_ST_SNEAK_MOVE */
    {ACOYOT01_ANIM_SNEAK2, 1, WOLF_CUE_RUN, WMS_RALPH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_BUSY_OR_QUIET | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_CONTEXT_ACTIONS |
         WSF_LOOK_MODE_OK},
    /* 0x0c WOLF_ST_DIE_FALL_PIT */
    {ACOYOT01_ANIM_STRIKE3, 0, 0, WMS_RALPH, CAM_REQ_DIR_HOLD, WSF_BODY_FLAG0 | WSF_AIRBORNE},
    /* 0x0d WOLF_ST_FALL_TO_DEATH */
    {ACOYOT01_ANIM_DEAD2, 0, 0, WMS_RALPH, CAM_FOLLOW, WSF_BODY_FLAG0},
    /* 0x0e WOLF_ST_DIE_FELL */
    {AEXPLOS2_ANIM_BOOM, 0, WOLF_CUE_GLIDE, WMS_EXPLOSION, CAM_FOLLOW, WSF_BODY_FLAG0},
    /* 0x0f WOLF_ST_DIE_KNOCKED */
    {ACOYOT01_ANIM_EXPLOSM1, 0, 0, WMS_RALPH, CAM_REQ_DIR_HOLD,
     WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_IGNORE_INPUT_FREEZE | WSF_KILL_WHEN_DEAD},
    /* 0x10 WOLF_ST_DIE_CRUSHED */
    {ACOYOT01_ANIM_SQUASH3, 0, 0, WMS_RALPH, CAM_FOLLOW,
     WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_IGNORE_INPUT_FREEZE | WSF_BODY_FLAG0 | WSF_SKIP_BOX_TEST},
    /* 0x11 WOLF_ST_LIFT_END_M0 */
    {ACOYOT01_ANIM_TAKE2B, 0, WOLF_CUE_5, WMS_RALPH, CAM_FOLLOW,
     WSF_BUSY_OR_QUIET | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE},
    /* 0x12 WOLF_ST_PUTDOWN_M0 */
    {ACOYOT01_ANIM_PUT2A, 0, WOLF_CUE_6, WMS_RALPH, CAM_FOLLOW,
     WSF_BUSY_OR_QUIET | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_INTERACTING},
    /* 0x13 WOLF_ST_SCRIPT_WALK */
    {ACOYOT01_ANIM_RUN2, 0, WOLF_CUE_WALK, WMS_RALPH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_UNTOUCHABLE},
    /* 0x14 WOLF_ST_SCRIPT_TURN */
    {ACOYOT01_ANIM_STAND8, 0, 0, WMS_RALPH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_UNTOUCHABLE},
    /* 0x15 WOLF_ST_SCRIPT_PUTDOWN */
    {ACOYOT01_ANIM_PUT2A, 0, WOLF_CUE_6, WMS_RALPH, CAM_FOLLOW,
     WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_UNTOUCHABLE},
    /* 0x16 WOLF_ST_BOUNCED_UP */
    {ACOYOT01_ANIM_FALL2, 0, WOLF_CUE_JUMP, WMS_RALPH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_CAM_LEDGE_PROBE | WSF_ITEM_WHEEL_OK | WSF_CONTEXT_ACTIONS},
    /* 0x17 WOLF_ST_SKID_ICE */
    {ACOYOT01_ANIM_SLIDE6, 0, WOLF_CUE_WALK, WMS_RALPH, CAM_FOLLOW,
     WSF_LOOP_ANIM | WSF_CAM_LEDGE_PROBE | WSF_GROUND_STATE | WSF_CONTEXT_ACTIONS | WSF_LOOK_MODE_OK},
    /* 0x18 WOLF_ST_TRAJECTORY */
    {ACOYOT01_ANIM_FALL2, 0, WOLF_CUE_GLIDE, WMS_RALPH, CAM_FOLLOW, WSF_LOOP_ANIM | WSF_CAM_LEDGE_PROBE},
    /* 0x19 WOLF_ST_GEYSER_SUCKED */
    {ACOYOT01_ANIM_TORNA4, 0, WOLF_CUE_SPIN, WMS_RALPH, CAM_FOLLOW,
     WSF_CAM_LEDGE_PROBE | WSF_BODY_FLAG0 | WSF_KEEP_COVER | WSF_UNTOUCHABLE},
    /* 0x1a WOLF_ST_GEYSER_EJECT */
    {ACOYOT01_ANIM_TORNA5, 0, WOLF_CUE_SPIN_END, WMS_RALPH, CAM_FOLLOW,
     WSF_CAM_LEDGE_PROBE | WSF_BODY_FLAG0 | WSF_KEEP_COVER | WSF_UNTOUCHABLE},
    /* 0x1b WOLF_ST_DIE_HOOVERED */
    {ACOYOT01_ANIM_TORNA4, 0, WOLF_CUE_SPIN, WMS_RALPH, CAM_FOLLOW, WSF_CAM_LEDGE_PROBE},
    /* 0x1c WOLF_ST_KNOCKBACK */
    {ACOYOT01_ANIM_FALL2, 0, 0, WMS_RALPH, CAM_FOLLOW, WSF_LOOP_ANIM | WSF_CAM_LEDGE_PROBE | WSF_SMOKE_TRAIL},
};
/* const in .rdata; WolfMoveBank.states is a WolfStateDesc * (see the header) */
/* cast kept (both macros): the tables are const (.rdata) but WolfMoveBank.states is a plain WolfStateDesc * */
#define g_wolfStateDescriptors0 ((WolfStateDesc *)g_wolfStateDescriptors0)
#define g_wolfStateTable1 ((WolfStateDesc *)g_wolfStateTable1)
/* 0x5758a0 - idle animations: base, then variants (on foot; carrying; ghost costume) */
extern const IdleAnimEntry g_wolfIdleAnims[5] = {{ACOYOT01_ANIM_STAND0, 3, 5},
                                                 {ACOYOT01_ANIM_STAND1, 1, 2},
                                                 {ACOYOT01_ANIM_STAND2, 5, 6},
                                                 {ACOYOT01_ANIM_STAND3, 5, 6},
                                                 {ACOYOT01_ANIM_STAND5, 1, 1}};
extern const IdleAnimEntry g_wolfIdleAnimsCarry[2] = {{ACOYOT01_ANIM_STAND8, 3, 5}, {ACOYOT01_ANIM_STAND12, 1, 2}};
extern const IdleAnimEntry g_wolfGhostIdleAnims[3] = {
    {ACOGHO01_ANIM_STAND1, 2, 3}, {ACOGHO01_ANIM_STAND2, 4, 6}, {ACOGHO01_ANIM_STAND3, 4, 6}};
/* 0x5758c8 - state 0xc1: button -> dance animation */
extern const WolfDanceMove g_wolfDanceMoves[4] = {{(u16)~PAD_TRIANGLE, ACOGHO01_ANIM_DANCE},
                                                  {(u16)~PAD_CIRCLE, ACOGHO01_ANIM_DANCE2},
                                                  {(u16)~PAD_CROSS, ACOGHO01_ANIM_DANCE4},
                                                  {(u16)~PAD_SQUARE, ACOGHO01_ANIM_DANCE3}};
/* 0x5758d8 - the rabbit costume's idle animations */
extern const IdleAnimEntry g_wolfIdleAnimsCostume4[2] = {{ACOYOT01_ANIM_STANDBU2, 3, 5},
                                                         {ACOYOT01_ANIM_STANDBU0, 10, 20}};
/* 0x5758e0 / 0x5758e8 / 0x5758f0 - held-object offsets (put down at the feet / in front; the equip preview) */
extern const Vec3s g_wolfPutDownOffsetNear = {0, -2, 0};
extern const Vec3s g_wolfPutDownOffsetFront = {0, -70, -92};
extern const Vec3s g_wolfEquipPreviewOffset = {0, -100, 0};

/* .data 0x57acd8: this object's copy of the cinematic opcode stride table, a static table of a header every cinematic
 * user includes (one copy per object, src/engine/cine2.cpp); nothing here reads it. */
static u8 g_cineOpStrideCopy[9] = {0, 8, 8, 4, 2, 2, 4, 2, 2};
/* The trail parameter blocks of Wolf_UpdateTrailFx, 0x57ace4-0x57adb3, in their .data order; nothing else refers to
 * them. Defined here with the exe's initial bytes (the matcher checks them by content); the spawn intervals, the bubble
 * size/cap are rewritten every frame. The table names are Ghidra's. */
EmitterDriftParams g_wolfDustParams = {100, -20, 2608, 163, 40, 120, 0}; /* run dust, and states with flag 0x40000000 */
EmitterDriftParams g_wolfTurnDustParams = {100, -80, 2048, 409, 20, 40, 0}; /* sharp turn (fxFlags 0x1000) */
EmitterDriftParams g_wolfRocketFxParams = {200, 160, 3264, 204, 20, 60, 0}; /* rocket exhaust (state flag 0x20) */
EmitterDriftParams g_wolfLandDustParams = {0, -80, 1024, 64, 20, 60, 0};    /* landing dust (landDustTimer) */
EmitterDriftParams g_wolfSkidDustParams = {10, -20, 2048, 409, 30, 50, 0};  /* skid (fxFlags 0x800) */
EmitterDriftParams g_wolfBreathParams = {40, 10, 4096, 512, 20, 50, 0};     /* cold breath (snow) */
EmitterRiseParams g_wolfBubbleParams = {-120, 0x2000, 0x400, 10, 0, 2};     /* underwater bubbles */
EmitterFadeParams g_wolfWakeParams = {0x2000, 0, 0x200, 50, 200, 1};        /* wading wake rings */
EmitterTrailParams g_wolfSnowPrintParams = {0x7fffffff, 150, 30, 6};        /* footprints in snow (permanent) */
EmitterTrailParams g_wolfWetPrintParams = {0x1e000, 150, 30, 9};            /* wet footprints */
/* 0x57adb8 / 0x57b1a8 - the movement profiles of bank 0 ([surface][profile]; row 1 is the ice row) and bank 1 */
MoveRecord g_wolfMoveProfilesNormal[2][18] = {
    {
        {600, 4000, 3000, 0, 81920, 81920, 245760, 245760, 1024},
        {250, 1000, 1000, 0, 20480, 20480, 61440, 61440, 2048},
        {1200, 1200, 1200, 0, 3413, 3413, 40960, 40960, 0},
        {150, 800, 1000, 0, 1024, 1024, 2048, 2048, 2048},
        {150, 200, 200, 300, 2048, 2048, 4096, 4096, 0},
        {300, 300, 300, 300, 2048, 2048, 4096, 4096, 0},
        {100, 1000, 1000, 0, 20480, 20480, 61440, 61440, 2048},
        {300, 2000, 1000, 0, 20480, 20480, 61440, 61440, 2048},
        {300, 1000, 1000, 300, 4096, 4096, 12288, 12288, 0},
        {1200, 2400, 1200, 1200, 1024, 1024, 2048, 2048, 0},
        {0, 4000, 3000, 0, 1024, 1024, 245760, 245760, 2048},
        {200, 1000, 1000, 0, 20480, 20480, 61440, 61440, 2048},
        {250, 1000, 1000, 0, 81920, 163840, 245760, 491520, 2048},
        {250, 1000, 1000, 0, 20480, 20480, 61440, 61440, 2048},
        {250, 1000, 1000, 0, 81920, 163840, 245760, 491520, 2048},
        {500, 3000, 2000, 0, 61440, 61440, 204800, 204800, 1024},
        {1200, 1200, 1200, 0, 2048, 2048, 8192, 8192, 0},
        {200, 1000, 1000, 0, 4096, 4096, 12288, 12288, 2048},
    },
    {
        {600, 2000, 1000, 0, 2048, 81920, 4096, 245760, 1024},
        {250, 500, 400, 0, 2048, 20480, 4096, 61440, 1024},
        {1200, 600, 400, 0, 3072, 3413, 4096, 40960, 0},
        {150, 500, 400, 0, 1024, 1024, 2048, 2048, 1024},
        {150, 200, 200, 300, 2048, 2048, 4096, 4096, 0},
        {300, 300, 300, 300, 2048, 2048, 4096, 4096, 0},
        {100, 500, 500, 0, 2048, 81920, 4096, 245760, 2048},
        {300, 1000, 500, 0, 2048, 81920, 4096, 245760, 2048},
        {300, 1000, 1000, 300, 4096, 4096, 12288, 12288, 0},
        {1200, 2400, 1200, 1200, 1024, 1024, 2048, 2048, 0},
        {0, 2000, 800, 0, 1024, 1024, 245760, 245760, 1024},
        {200, 500, 500, 0, 2048, 20480, 4096, 61440, 1024},
        {250, 500, 500, 0, 2048, 163840, 4096, 491520, 1024},
        {250, 500, 500, 0, 2048, 20480, 4096, 61440, 2048},
        {250, 500, 500, 0, 2048, 163840, 4096, 491520, 1024},
        {500, 1500, 700, 0, 2048, 61440, 4096, 204800, 1024},
        {1200, 600, 400, 0, 2048, 2048, 4096, 4096, 0},
        {200, 1000, 1000, 0, 4096, 4096, 12288, 12288, 2048},
    },
};
MoveRecord g_wolfMoveProfilesCarry[2][2] = {
    {
        {450, 2000, 1500, 0, 20480, 20480, 61440, 61440, 1024},
        {200, 800, 400, 0, 20480, 20480, 61440, 61440, 2048},
    },
    {
        {450, 1000, 500, 0, 20480, 20480, 61440, 61440, 1024},
        {200, 800, 400, 0, 20480, 20480, 61440, 61440, 2048},
    },
};

/* .bss 0x6cf280-0x6cf318 */
WolfMoveBank g_wolfMoveBank0[2] = {0}; /* 0x6cf280  [0] Ralph on foot, [1] carrying (g_wolfMoveBank1 0x6cf2c8) */
Wolf *g_pWolf = 0;                     /* 0x6cf310 */
u8 g_wolfInstanceCount = 0;            /* 0x6cf314  counted up by Wolf_Init -> playerIndex */

/* ---- the state machine ---- */
/* 0x47d310 - one frame of Ralph's state machine (see the file header). */
/* BYTES(slot-group): locals grouped in w with per-member offsets; pulledTo / pullRate named so the frame reads w, pullRate, pulledTo; unused8c is never touched */
void Wolf::StateMachine(Pad *pad)
{
    Vec3s pulledTo; /* -0xa4 (the names order the frame: w, pullRate, pulledTo) */
    s32 pullRate;   /* -0x9c */
    struct {
        Vec3s minePoint;     /* -0x98  state 0xd0: the point 40 units in front of the mine */
        s32 mineDist;        /* -0x90  |squared XZ distance to the mine - 0x640| */
        u16 unused8c;        /* -0x8c  never touched */
        s16 mineHeading;     /* -0x8a  heading from Ralph to the mine */
        s32 canStep;         /* -0x88  state 0xc1: a new dance move may start */
        s32 move;            /* -0x84  state 0xc1: index into g_wolfDanceMoves */
        ScnObject *camOwner; /* -0x80  state 0xbd: the scripted camera's owner */
        s32 notReeling;      /* -0x7c  state 0xb0: the stick is not held up or down */
        s32 reel;            /* -0x78  state 0xb0: reel amount this frame */
        Vec3s coverSpot;     /* -0x74  state 0x5c: where to stand against the cover */
        s32 reached;         /* -0x6c  state 0x5c: MoveTowardPoint arrived */
        Vec3s upright;       /* -0x68  state 0x54: the rotation Ralph gets back after the rocket */
        s32 fuelOk;          /* -0x60  state 0x54: the rocket's reply to msg 0x900 (burn fuel) */
        s32 intact;          /* -0x5c  state 0x54: the rocket's reply to msg 0x901 (impact) */
        s32 impact;          /* -0x58  state 0x54: Wolf_RocketFlyStep's result */
        Vec3s snap;          /* -0x54  state 0x10: Ralph moved onto the crusher's face */
        CollBox myBox;       /* -0x4c  state 0x10: Ralph's first box, in the world */
        CollBox killerBox;   /* -0x3c  state 0x10: the crusher's first solid box, in the world */
        CollBox *solidBox;   /* -0x2c */
        Box *climbBox;       /* -0x28  states 0x3f/0x40 */
        s32 drop;            /* -0x24  state 0x26: ground height below Ralph's feet */
        s32 result;          /* -0x20  a step's return value */
        u32 moveFlags;       /* -0x1c  a step's collision / landing flags */
        DropMsgArg dropArg;  /* -0x18  message 5 (put down) argument */
        MoveRecord *rec;     /* -0x10 */
        WolfStateDesc *desc; /* -0xc */
        WolfMoveBank *bank;  /* -0x8   g_wolfMoveBank0[mode] */
        u16 *tuning;         /* -0x4   bank->surfaceTuningU[surface], read unsigned */
    } w;

    w.bank = &g_wolfMoveBank0[mode];
    w.tuning = w.bank->surfaceTuningU[surface];
    FilterInput(pad, (u16)w.bank->params[0]);
    switch (state) {
        case WOLF_ST_IDLE:
            if (GroundCommon()) {
                if (stickMag != 0)
                    SetState(WOLF_ST_WALK);
                else if (flags & WOLF_FB_ON_MOVING_PLATFORM)
                    SetState(WOLF_ST_SKID);
                else if (AnimFlags(ANIM_F_FINISHED))
                    IdleAnimStep();
            }
            break;
        case WOLF_ST_WALK:
            if (flags & WOLF_FB_SPECIAL_OBJECT)
                UpdateMoveLoopSound();
            if (GroundCommon()) {
                FootstepSound();
                if (stickMag == 0) {
                    w.desc = &w.bank->states[state];
                    w.rec = &w.bank->profiles[surface][w.desc->profile];
                    if (surface == WOLF_SURFACE_ICE && speed >= w.rec->deceleration >> 2)
                        SetState(WOLF_ST_SKID_ICE);
                    else if (stateTime * 2 >= animDurationTicks)
                        SetIdleState();
                }
            }
            break;
        case WOLF_ST_SKID:
        case WOLF_ST_SKID_ICE:
            if (state == WOLF_ST_SKID_ICE)
                PlayLoopSound(SND_WOLF_SKID_ICE);
            else
                PlayLoopSound(SND_SCOGLISS);
            if (GroundCommon()) {
                w.desc = &w.bank->states[state];
                w.rec = &w.bank->profiles[surface][w.desc->profile];
                if (stickMag != 0 && speed >= w.rec->maxSpeed >> 1)
                    SetState(WOLF_ST_WALK);
                else if (speed <= w.rec->deceleration >> 4 && !(flags & WOLF_FB_ON_MOVING_PLATFORM))
                    SetIdleState();
            }
            break;
        case WOLF_ST_JUMP_START:
            MoveStep(0);
            if (!CheckActionButton(0) && AnimFlags(ANIM_F_FINISHED))
                SetState(WOLF_ST_JUMP_ASCEND);
            break;
        case WOLF_ST_JUMP_ASCEND:
            if (JumpAscendStateStep(w.bank->params[1], w.bank->params[2])) {
                if ((padBits & WOLF_ACT_DOUBLE_JUMP) && stateTime >= w.bank->params[2] >> 2)
                    SetState(WOLF_ST_DOUBLEJUMP_ASCEND);
                else if (stateTime >= w.bank->params[2])
                    SetState(WOLF_ST_JUMP_APEX);
            }
            break;
        case WOLF_ST_JUMP_APEX:
            if (FallStateStep(32000)) {
                if (padBits & WOLF_ACT_DOUBLE_JUMP)
                    SetState(WOLF_ST_DOUBLEJUMP_ASCEND);
                else if (AnimFlags(ANIM_F_FINISHED))
                    SetState(WOLF_ST_FALL);
            }
            break;
        case WOLF_ST_FALL:
            if (FallStateStep(32000))
                UpdateFallAnim();
            break;
        case WOLF_ST_LAND_IDLE:
            if (GroundCommon() && AnimFlags(ANIM_F_FINISHED)) {
                SetIdleState();
                Sound_Play(SND_SCORECPT, this, 0xff, SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 0x1000);
            }
            break;
        case WOLF_ST_LAND_MOVE:
            if (GroundCommon() && AnimFlags(ANIM_F_FINISHED)) {
                SetState(WOLF_ST_WALK);
                Sound_Play(SND_SCORECPT, this, 0xff, SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 0x1000);
            }
            break;
        case WOLF_ST_DOUBLEJUMP_ASCEND:
            if (JumpAscendStateStep(w.bank->params[3], w.bank->params[4]) && stateTime >= w.bank->params[4])
                SetState(WOLF_ST_DOUBLEJUMP_FALL);
            break;
        case WOLF_ST_DOUBLEJUMP_FALL:
            if (FallStateStep(32000) && AnimFlags(ANIM_F_FINISHED))
                SetState(WOLF_ST_FALL);
            break;
        case WOLF_ST_RUN_START:
            RunTapCheck(1);
            if (RunStep(pad) && AnimFlags(ANIM_F_FINISHED)) {
                if (flags & WOLF_FB_RUN)
                    SetStateKeepSound(WOLF_ST_RUN);
                else
                    SetState(WOLF_ST_RUN_STOP);
            }
            break;
        case WOLF_ST_RUN:
            RunTapCheck(0);
            if (RunStep(pad) && !(flags & WOLF_FB_RUN) && speed < w.bank->profiles[surface]->maxSpeed)
                SetState(WOLF_ST_RUN_STOP);
            break;
        case WOLF_ST_RUN_STOP:
            ClearFlags(WOLF_FB_RUN);
            if (RunStep(pad) && AnimFlags(ANIM_F_FINISHED))
                SetState(WOLF_ST_RUN_RECOVER);
            break;
        case WOLF_ST_RUN_RECOVER:
            if (GroundCommon()) {
                if (stickMag != 0)
                    SetState(WOLF_ST_WALK);
                else if (stateTime >= 0x5000)
                    SetIdleState();
            }
            break;
        case WOLF_ST_RUN_AIR:
            RunTapCheck(0);
            if (stateTime > w.bank->params[8]) {
                ClearFlags(WOLF_FB_RUN);
                runTapTimer = 0;
            }
            RunMove(0, 0, &w.moveFlags);
            airTime = 0;
            w.drop = GroundY() - pos.y;
            if (w.moveFlags != 0 || w.drop <= 200) {
                if (w.moveFlags & COLL_WALL)
                    SetState(WOLF_ST_FALL);
                else
                    SetStateKeepAnim(WOLF_ST_RUN);
            } else if (speed < w.bank->profiles[surface]->maxSpeed) {
                if (w.drop > 200) {
                    if (w.drop >= w.bank->params[7] && !(flags & WOLF_FB_FORCE_FALL) && riderCount == 0)
                        Die(WOLF_ST_DIE_FALL_PIT, WOLF_DROP_STOP, 0x2000);
                    else
                        SetState(WOLF_ST_FALL);
                } else
                    SetStateKeepAnim(WOLF_ST_RUN);
            }
            break;
        case WOLF_ST_SLOPE_SLIDE:
            fxFlags |= WOLF_FA_REQ_TURN;
            w.result = MoveStep(1);
            if (IsFalling())
                SetState(WOLF_ST_FALL);
            else if ((padBits & WOLF_ACT_JUMP_EDGE) && CanJump())
                SetState(WOLF_ST_JUMP_START);
            else if (!(flags & WOLF_FB_IN_SLIDE_ZONE) && !(flags & WOLF_FB_FORCE_SLIDE) &&
                     groundNormal.y <= -w.tuning[1])
                SetIdleState();
            break;
        case WOLF_ST_SNEAK_IDLE:
            if (SneakStateStep()) {
                if (stickMag != 0)
                    SetState(WOLF_ST_SNEAK_MOVE);
                else if (!(padBits & WOLF_ACT_SNEAK_HELD))
                    SetIdleState();
            }
            break;
        case WOLF_ST_SNEAK_MOVE:
            if (SneakStateStep()) {
                SneakStepSound();
                if (stateTime * 2 >= animDurationTicks) {
                    if (stickMag == 0)
                        SetState(WOLF_ST_SNEAK_IDLE);
                    else if (!(padBits & WOLF_ACT_SNEAK_HELD))
                        SetState(WOLF_ST_WALK);
                }
            }
            break;
        case WOLF_ST_PICKUP_ITEM:
            if (FaceTargetActionStep(CTX_PICKUP)) {
                ctxAction.target->HandleMessage(this, MSG_INVENTORY_STORED, 0);
                ctxAction.target->RemoveFromWorld();
                Inventory_Add(ctxAction.target);
                ItemFly_Start(ctxAction.target, ITEMFLY_TO_INVENTORY, &ctxAction.target->pos);
                SetState(WOLF_ST_PICKUP_END);
            }
            break;
        case WOLF_ST_THROW_ITEM:
            if (State2B_Step() && AnimFlags(ANIM_F_FINISHED)) {
                if (CanPutDownHeldObj(1, &w.dropArg.pos)) {
                    w.dropArg.placed = 1;
                    w.dropArg.flag1 = 0;
                    Inventory_Remove(heldObject);
                    heldObject->HandleMessage(this, MSG_DROP, &w.dropArg);
                    heldObject = 0;
                    SetState(WOLF_ST_THROW_END);
                } else
                    SetState(WOLF_ST_SHRUG);
            }
            break;
        case WOLF_ST_THROW_END:
            if (State2B_Step() && AnimFlags(ANIM_F_FINISHED) && !SwapHeldItem(1, 0))
                SetIdleState();
            break;
        case WOLF_ST_LIFT_START:
            if (FaceTargetActionStep(CTX_LIFT)) {
                /* cast kept: MSG_PICKUP's arg is the joint to attach to, a number in the void * */
                if (ctxAction.target->HandleMessage(this, MSG_PICKUP, (void *)0x12)) {
                    heldObject = ctxAction.target;
                    SetMode(WOLF_MODE_CARRY, WOLFC_ST_LIFT_END);
                } else
                    SetIdleState();
            }
            break;
        case WOLF_ST_PUTDOWN_M0:
            if (State2B_Step() && AnimFlags(ANIM_F_FINISHED)) {
                if (CanPutDownHeldObj(0, &w.dropArg.pos)) {
                    w.dropArg.placed = 1;
                    w.dropArg.flag1 = 0;
                    heldObject->HandleMessage(this, MSG_DROP, &w.dropArg);
                    heldObject = 0;
                    SetMode(WOLF_MODE_NORMAL, WOLF_ST_PUTDOWN_RECOVER);
                } else
                    SetIdleState();
            }
            break;
        case WOLF_ST_LIFT_END_M0:
        case WOLF_ST_PUTDOWN_RECOVER:
        case WOLF_ST_RUN_CRASH:
        case WOLF_ST_PICKUP_END:
        case WOLF_ST_PUSH_END:
        case WOLF_ST_ACTIVATE_END:
        case WOLF_ST_BUSH_TAKE_OFF:
        case WOLF_ST_DETECTOR_END:
        case WOLF_ST_UMBRELLA_CLOSE:
        case WOLF_ST_WOLFTRAP_EXIT:
        case WOLF_ST_GOSSAMER_RELEASED:
        case WOLF_ST_BURNT:
        case WOLF_ST_REMOTE_END:
        case WOLF_ST_USE_CTX1A_END:
        case WOLF_ST_FISHING_END:
        case WOLF_ST_HIT_BY_BIPBIP:
        case WOLF_ST_KICK_ICECUBE_END:
            if (State2B_Step() && AnimFlags(ANIM_F_FINISHED))
                SetIdleState();
            break;
        case WOLF_ST_DIE_KNOCKED:
        case WOLF_ST_DIE_FALL_TO_SENDER:
        case WOLF_ST_SHEEPCOSTUME_DIE_LANDED:
        case WOLF_ST_DIE_ZAPPED_REMOTE:
        case WOLF_ST_DIE_PIRATE:
        case WOLF_ST_DIE_ZAPPED:
            Stick_Clear(&stickX);
            FallStep(fallLimitY, 0);
            break;
        case WOLF_ST_DIE_FALL_PIT:
        case WOLF_ST_DIE_FELL:
        case WOLF_ST_GEYSER_SUCKED:
        case WOLF_ST_DIE_HOOVERED:
        case WOLF_ST_CAUGHT_BY_SAM:
        case WOLF_ST_DIE_SAM_PUNCH:
        case WOLF_ST_DIE_SAM_PUNCH_HIT:
        case WOLF_ST_DIE_ZONE:
        case WOLF_ST_DIE_SHARK:
        case WOLF_ST_DIE_CROCODILE:
        case WOLF_ST_GOSSAMER_GRAB_0:
        case WOLF_ST_TIMEMACHINE_DEPART:
        case WOLF_ST_GHOSTCOSTUME_LIFTED:
        case WOLF_ST_GHOST_LIFTED:
        case WOLF_ST_DIE_RAFT:
        case WOLF_ST_DIE_MARTIAN:
            GetShadow()->Reproject();
            StopMotion();
            break;
        case WOLF_ST_DIE_PIRANHAS:
            GetShadow()->Reproject();
            StopMotion();
            if (stateTime >= 0x2400)
                GetShadow()->SetVisible(0);
            break;
        case WOLF_ST_DIE_CRUSHED:
            GetShadow()->Reproject();
            StopMotion();
            if (killer && (w.solidBox = killer->GetFirstSolidBox()) != 0) {
                w.myBox.Box_Translate(GetFirstModelBox(), &pos);
                w.killerBox.Box_Translate(w.solidBox, &killer->pos);
                if (Overlap4(w.killerBox.max.x - w.myBox.min.x, w.myBox.max.x - w.killerBox.min.x,
                             w.killerBox.max.z - w.myBox.min.z, w.myBox.max.z - w.killerBox.min.z) &&
                    w.killerBox.max.y > pos.y) {
                    w.snap.x = pos.x;
                    w.snap.z = pos.z;
                    w.snap.y = w.killerBox.max.y;
                    SetPosition(&w.snap);
                }
            }
            break;
        case WOLF_ST_ITEM_TAKE_OUT:
            if (State2B_Step() && AnimFlags(ANIM_F_FINISHED) && !(flags & WOLF_FB_EQUIP_PENDING))
                SetIdleState();
            break;
        case WOLF_ST_PUSH_START:
            if (FaceTargetStep()) {
                if (ctxAction.action != CTX_PUSH)
                    SetState(WOLF_ST_PUSH_END);
                else if (AnimFlags(ANIM_F_FINISHED))
                    SetState(WOLF_ST_PUSH);
            }
            break;
        case WOLF_ST_PUSH:
            fxFlags |= WOLF_FA_REQ_SKID;
            if (ctxAction.action != CTX_PUSH)
                w.result = MoveStep(0);
            else
                w.result = PushObjectStep(ctxAction.target);
            if (IsFalling())
                SetState(WOLF_ST_FALL);
            else if (w.result)
                SetState(WOLF_ST_SLOPE_SLIDE);
            else if (!(padBits & WOLF_ACT_ACTION_HELD) || ctxAction.action != CTX_PUSH)
                SetState(WOLF_ST_PUSH_END);
            break;
        case WOLF_ST_DIE_SAM_PUNCH_WINDUP:
            GetShadow()->Reproject();
            StopMotion();
            if (stateTime >= 0x2000) {
                Camera_StartShake(0x14, 0x1000);
                SetStateKeepAnimStopSound(WOLF_ST_DIE_SAM_PUNCH_HIT);
            }
            break;
        case WOLF_ST_FALL_TO_DEATH:
            if (loopSfxHandle == 0)
                loopSfxHandle = Sound_Play(SND_SCOFALL, this, 0x7f, SNDF_NO_RETRIGGER, 0x1000);
            Stick_Clear(&stickX);
            if (FallStateStep(fallLimitY) &&
                ((pos.y >= fallLimitY && !Sound_IsPlaying(LoopSfx())) || stateTime >= 0x5000)) {
                Sound_Play(SND_SCOCHOLE, this, 0x7f, SNDF_NO_RETRIGGER, 0x1000);
                Die(WOLF_ST_DIE_FELL, WOLF_DROP_RELEASE, 0x1000);
                SetPartHeight(0xfff);
            }
            break;
        case WOLF_ST_SCRIPT_WALK:
            if (MoveTowardPoint(&scriptWalkTarget, 0, 0))
                SetState(WOLF_ST_SCRIPT_TURN);
            else {
                FootstepSound();
                if (stateTime >= 0xa000) {
                    SetPosition(&scriptWalkTarget);
                    SetState(WOLF_ST_SCRIPT_TURN);
                }
            }
            break;
        case WOLF_ST_SCRIPT_TURN:
            ForcedHeadingGroundStep(scriptWalkHeading);
            if (SDW_ABS(SDW_ANGLE_DIFF(scriptWalkHeading, Facing())) <= 11) {
                SetFacing(scriptWalkHeading);
                switch (mode) {
                    case WOLF_MODE_NORMAL:
                        SetIdleState();
                        flags |= WOLF_FB_CINE_READY;
                        ClearEffects();
                        break;
                    case WOLF_MODE_CARRY:
                        SetState(WOLF_ST_SCRIPT_PUTDOWN);
                        break;
                }
            }
            break;
        case WOLF_ST_SCRIPT_PUTDOWN:
            Stick_Clear(&stickX);
            MoveStep(0);
            if (AnimFlags(ANIM_F_FINISHED) && CanPutDownHeldObj(0, &w.dropArg.pos)) {
                w.dropArg.placed = 1;
                w.dropArg.flag1 = 0;
                heldObject->HandleMessage(this, MSG_DROP, &w.dropArg);
                if (fxFlags & WOLF_FA_CINE_SHEEP_DROP)
                    heldObject->HandleMessage(this, MSG_SHEEP_WALK_TO, &carryDropPos);
                else {
                    flags |= WOLF_FB_CINE_READY;
                    ClearEffects();
                }
                heldObject = 0;
                SetMode(WOLF_MODE_NORMAL, WOLF_ST_SCRIPT_PUTDOWN_RECOVER);
            }
            break;
        case WOLF_ST_SCRIPT_PUTDOWN_RECOVER:
        case WOLF_ST_TIMEMACHINE_ARRIVE:
            Stick_Clear(&stickX);
            MoveStep(0);
            if (AnimFlags(ANIM_F_FINISHED))
                SetIdleState();
            break;
        case WOLF_ST_BOUNCED_UP:
            w.moveFlags = JumpAscendStep(600, 0xc00);
            if (w.moveFlags & COLL_FLOOR)
                SetState(WOLF_ST_FALL);
            else if (stateTime >= 0xc00)
                SetState(WOLF_ST_FALL);
            break;
        case WOLF_ST_ITEM_STOW:
        case WOLF_ST_SHRUG:
            if (GroundCommon()) {
                if (stickMag != 0)
                    SetState(WOLF_ST_WALK);
                else if (AnimFlags(ANIM_F_FINISHED))
                    SetIdleState();
            }
            break;
        case WOLF_ST_TALK_TURN:
            if (ctxAction.target)
                interactHeading = HeadingTo(&ctxAction.target->pos);
            else
                interactHeading = Facing();
            if (FaceTargetStep() && SDW_ABS(SDW_ANGLE_DIFF(Facing(), interactHeading)) <= 0x100) {
                if (ctxAction.action == CTX_TALK)
                    ctxAction.target->HandleMessage(this, MSG_USE, 0);
                SetIdleState();
            }
            break;
        case WOLF_ST_UPDRAFT:
            UpdraftStep();
            if (!CheckActionButton(0)) {
                if (updraftZone == 0)
                    SetState(WOLF_ST_FALL);
                else if (padBits & WOLF_ACT_RUN_HELD)
                    SetState(WOLF_ST_UPDRAFT_FAST);
            }
            break;
        case WOLF_ST_UPDRAFT_FAST:
            UpdraftStep();
            if (updraftZone == 0)
                SetState(WOLF_ST_FALL);
            else if (!(padBits & WOLF_ACT_RUN_HELD))
                SetState(WOLF_ST_UPDRAFT);
            break;
        case WOLF_ST_DIE_BLOWN_AWAY:
            DeathRiseStep();
            break;
        case WOLF_ST_ACTIVATE:
            if (FaceTargetActionStep(CTX_ACTIVATE)) {
                ctxAction.target->HandleMessage(this, MSG_USE, 0);
                SetState(WOLF_ST_ACTIVATE_END);
            }
            break;
        case WOLF_ST_CLIMB_ZONE:
            w.climbBox = climbZones.FindContaining(&pos);
            ClimbStep(w.climbBox, 1);
            ctxAction.action = CTX_CLIMBING;
            if (w.climbBox == 0 || (padBits & WOLF_ACT_JUMP_EDGE))
                SetState(WOLF_ST_JUMP_ASCEND);
            else if (padBits & WOLF_ACT_ACTION_EDGE)
                SetState(WOLF_ST_FALL);
            break;
        case WOLF_ST_CLIMB_OBJECT:
            /* cast kept: Box and CollBox are two views of one 16-byte record */
            w.climbBox = (Box *)FindClimbBoxOnTarget();
            ClimbStep(w.climbBox, 0);
            ctxAction.action = CTX_CLIMBING;
            if (w.climbBox == 0 || (padBits & WOLF_ACT_JUMP_EDGE))
                SetState(WOLF_ST_JUMP_ASCEND);
            else if (padBits & WOLF_ACT_ACTION_EDGE)
                SetState(WOLF_ST_FALL);
            break;
        case WOLF_ST_CLIMBBOXWALK_IDLE:
            if (ClimbBoxWalkStep() && stickMag != 0)
                SetState(WOLF_ST_CLIMBBOXWALK_MOVE);
            break;
        case WOLF_ST_CLIMBBOXWALK_MOVE:
            if (flags & WOLF_FB_SPECIAL_OBJECT)
                UpdateMoveLoopSound();
            if (ClimbBoxWalkStep() && stickMag == 0)
                SetState(WOLF_ST_CLIMBBOXWALK_IDLE);
            break;
        case WOLF_ST_WADE:
            if (SwimStateStep()) {
                if (waterZone && pos.y > waterZone->min[1] + waterDepthOffset + 50)
                    SetState(WOLF_ST_SWIM);
                else if (stickMag == 0 && rot.x == 0) {
                    if (AnimId() != ACOYOT01_ANIM_SWIM4)
                        PlayAnim(ACOYOT01_ANIM_SWIM4, 1, 1);
                } else if (AnimId() != ACOYOT01_ANIM_SWIM1)
                    PlayAnim(ACOYOT01_ANIM_SWIM1, 1, 1);
            }
            break;
        case WOLF_ST_SWIM:
            if (SwimStateStep()) {
                if (waterZone && pos.y < waterZone->min[1] + waterDepthOffset + 20)
                    SetState(WOLF_ST_WADE);
                else if (stickMag == 0 && !(padBits & (WOLF_ACT_JUMP_HELD | WOLF_ACT_RUN_HELD))) {
                    if (AnimId() != ACOYOT01_ANIM_SWIM2)
                        PlayAnim(ACOYOT01_ANIM_SWIM2, 1, 1);
                    StopLoopSound();
                } else {
                    if (AnimId() != ACOYOT01_ANIM_SWIM3)
                        PlayAnim(ACOYOT01_ANIM_SWIM3, 1, 1);
                    PlayLoopSound(SND_WOLF_SWIM_LOOP);
                }
            }
            break;
        case WOLF_ST_WATER_JUMP:
            if (JumpAscendStateStep(w.bank->params[1], w.bank->params[2])) {
                if ((padBits & WOLF_ACT_DOUBLE_JUMP) && stateTime >= w.bank->params[2] >> 2)
                    SetState(WOLF_ST_WATER_DOUBLEJUMP);
                else if (stateTime >= w.bank->params[2])
                    SetState(WOLF_ST_WATER_JUMP_FALL);
            }
            break;
        case WOLF_ST_WATER_JUMP_FALL:
            if (FallStateStep(32000)) {
                if (padBits & WOLF_ACT_DOUBLE_JUMP)
                    SetState(WOLF_ST_WATER_DOUBLEJUMP);
                else if (stateTime >= w.bank->params[2]) {
                    if (!waterZone)
                        SetState(WOLF_ST_FALL);
                    else
                        SetState(WOLF_ST_WATER_PLUNGE);
                }
            }
            break;
        case WOLF_ST_WATER_DOUBLEJUMP:
            if (JumpAscendStateStep(w.bank->params[3], w.bank->params[4]) && stateTime >= w.bank->params[4])
                SetState(WOLF_ST_WATER_DOUBLEJUMP_FALL);
            break;
        case WOLF_ST_WATER_DOUBLEJUMP_FALL:
            if (FallStateStep(32000) && stateTime >= w.bank->params[4]) {
                if (!waterZone)
                    SetState(WOLF_ST_FALL);
                else
                    SetState(WOLF_ST_WATER_PLUNGE);
            }
            break;
        case WOLF_ST_WATER_PLUNGE:
            if (WaterSinkStep())
                UpdateFallAnim();
            break;
        case WOLF_ST_FROZEN_FLOAT:
            if (!(flags & WOLF_FB_RIVER_REGISTERED)) {
                WaterFloatUpStep();
                if (stateTime >= 0x14000)
                    RestartNoAnim(0x1000);
            } else
                StopMotion();
            if (!waterZone || (waterZone->flags & ZONE_WATER_NO_SURFACE))
                SetState(WOLF_ST_FROZEN_FALL);
            break;
        case WOLF_ST_FROZEN_FALL:
            Stick_Clear(&stickX);
            w.moveFlags = FallStep(32000, 0);
            if (waterZone && !(waterZone->flags & ZONE_WATER_NO_SURFACE))
                SetState(WOLF_ST_FROZEN_FLOAT);
            else if (w.moveFlags & COLL_FLOOR) {
                Sound_Play(SND_ICE_HIT, this, 0xff, SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 0x1000);
                if (fxFlags & WOLF_FA_PROP_ICE) {
                    Fx3or4_Stop();
                    Fx3or4_Start(1);
                    SetState(WOLF_ST_FROZEN_GROUND);
                } else
                    SetIdleState();
            }
            break;
        case WOLF_ST_DIE_FROZEN:
            WaterFloatUpStep();
            break;
        case WOLF_ST_FROZEN_GRABBED:
            StopMotion();
            break;
        case WOLF_ST_FROZEN_GROUND:
            ctxAction.action = CTX_SQUIRM;
            State2B_Step();
            if (padBits & WOLF_ACT_ACTION_EDGE) {
                SetState(WOLF_ST_FROZEN_SHAKE);
                runTapTimer = 0;
                mashCount = 0;
            }
            break;
        case WOLF_ST_FROZEN_SHAKE:
            ctxAction.action = CTX_SQUIRM;
            MashCounterStep();
            State2B_Step();
            if (AnimFlags(ANIM_F_FINISHED)) {
                if (AnimId() == ACOYOT01_ANIM_FREEZE4) {
                    PlayAnim(ACOYOT01_ANIM_FREEZE5, 1, 1);
                    if (fxFlags & WOLF_FA_PROP4)
                        propBody.PlayAnim(AGLACON3_ANIM_FREEZE5, 1, 1);
                } else if (mashCount == 0) {
                    SetState(WOLF_ST_FROZEN_GROUND);
                    if (fxFlags & WOLF_FA_PROP4)
                        propBody.PlayAnim(AGLACON3_ANIM_CLOSED, 1, 1);
                } else if (mashCount >= 10) {
                    SetState(WOLF_ST_FROZEN_BREAK);
                    if (fxFlags & WOLF_FA_PROP4)
                        propBody.PlayAnim(AGLACON3_ANIM_FREEZE6, 1, 1);
                }
            }
            break;
        case WOLF_ST_FROZEN_BREAK:
            PlayLoopSound(SND_ICE_SLIDE);
            State2B_Step();
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(WOLF_ST_SHIVER);
            break;
        case WOLF_ST_SHIVER:
            PlayLoopSound(SND_SCOCOLD);
            State2B_Step();
            if (stateTime >= 0x4000)
                SetIdleState();
            break;
        case WOLF_ST_ROCKET_MOUNT:
            Stick_Clear(&stickX);
            MoveStep(0);
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(WOLF_ST_ROCKET_IGNITE);
            break;
        case WOLF_ST_ROCKET_IGNITE:
            RocketFlyStep(padBits & WOLF_ACT_RUN_HELD, 1);
            heldObject->HandleMessage(this, MSG_USE, 0);
            heldObject->HandleMessage(this, MSG_USE, 0);
            if (stateTime >= 0xd1e)
                SetState(WOLF_ST_ROCKET_FLY);
            break;
        case WOLF_ST_ROCKET_FLY:
            heldActionType = HELD_ROCKET_ACTIVE;
            w.fuelOk = 1;
            w.intact = 1;
            w.impact = RocketFlyStep(padBits & WOLF_ACT_RUN_HELD, 0);
            if (w.impact > 0)
                /* cast kept: MSG_ROCKET_DAMAGE's arg is the impact, a number in the void * */
                w.intact = heldObject->HandleMessage(this, MSG_ROCKET_DAMAGE, (void *)w.impact);
            if (padBits & WOLF_ACT_RUN_HELD)
                /* cast kept: MSG_ROCKET_BURN_FUEL's arg is the frame time, a number in the void * */
                w.fuelOk &= heldObject->HandleMessage(this, MSG_ROCKET_BURN_FUEL, (void *)(g_dt * 2));
            else
                /* cast kept: MSG_ROCKET_BURN_FUEL's arg is the frame time, a number in the void * */
                w.fuelOk &= heldObject->HandleMessage(this, MSG_ROCKET_BURN_FUEL, (void *)g_dt);
            if (!w.fuelOk || (padBits & WOLF_ACT_ACTION_EDGE) || !w.intact) {
                HeldObj_SendMsg16();
                Inventory_Remove(heldObject);
                w.dropArg.pos = pos;
                w.dropArg.placed = 1;
                w.dropArg.flag1 = 0;
                w.fuelOk = heldObject->HandleMessage(this, MSG_DROP, &w.dropArg);
                heldObject = 0;
                if (w.intact)
                    SetState(WOLF_ST_FALL);
                else {
                    w.upright.x = w.upright.z = 0;
                    w.upright.y = Facing();
                    rot = w.upright;
                    SetState(WOLF_ST_ROCKET_CRASH);
                }
            }
            break;
        case WOLF_ST_ROCKET_CRASH:
            if (AnimFlags(ANIM_F_FINISHED)) {
                StopMotion();
                SetState(WOLF_ST_FALL);
            }
            break;
        case WOLF_ST_DAZED:
            GetShadow()->Reproject();
            StopMotion();
            if (AnimFlags(ANIM_F_FINISHED))
                SetIdleState();
            else if (stateTime >= 0x1000 && !(fxFlags & WOLF_FA_PROP0))
                Fx0_Start();
            break;
        case WOLF_ST_TRAJECTORY:
            LaunchArcStep();
            break;
        case WOLF_ST_FAN_IDLE:
            heldActionType = HELD_FAN_ACTIVE;
            if (FaceStickStep()) {
                if (padBits & WOLF_ACT_ACTION_EDGE)
                    SetIdleState();
                else if (stickMag != 0)
                    SetState(WOLF_ST_FAN_TURN);
            }
            break;
        case WOLF_ST_FAN_TURN:
            heldActionType = HELD_FAN_ACTIVE;
            if (FaceStickStep()) {
                if (padBits & WOLF_ACT_ACTION_EDGE)
                    SetIdleState();
                else if (stickMag == 0)
                    SetState(WOLF_ST_FAN_IDLE);
            }
            break;
        case WOLF_ST_COVER_APPROACH:
            if (coverAction.action == CTX_USE) {
                w.coverSpot = pos;
                switch (GetSideQuadrant(coverAction.target)) {
                    case 0:
                    case 3:
                        w.coverSpot.z = coverAction.target->pos.z;
                        break;
                    case 1:
                    case 2:
                        w.coverSpot.x = coverAction.target->pos.x;
                        break;
                }
                w.reached = MoveTowardPoint(&w.coverSpot, GetSideHeading(coverAction.target), 1);
            } else
                w.reached = MoveTowardPoint(&pos, 0, 0);
            if (!CheckActionButton(1)) {
                if (coverAction.action != CTX_USE || AnimFlags(ANIM_F_FINISHED) ||
                    !IsStickTowardSide(coverAction.target))
                    SetIdleState();
                else if (w.reached) {
                    coverObject = coverAction.target;
                    SetStateKeepAnimStopSound(WOLF_ST_COVER_ENTER);
                }
            }
            break;
        case WOLF_ST_COVER_ENTER:
            if (!CheckActionButton(1) && ForcedHeadingGroundStepChecked(GetSideHeading(coverObject))) {
                if (AnimFlags(ANIM_F_FINISHED))
                    SetState(WOLF_ST_COVER_HIDDEN);
                else if (coverAction.action != CTX_USE || !IsStickTowardSide(coverObject))
                    SetIdleState();
            }
            break;
        case WOLF_ST_COVER_HIDDEN:
            if (!CheckActionButton(1) && ForcedHeadingGroundStepChecked(GetSideHeading(coverObject)) &&
                (coverAction.action != CTX_USE || !IsStickTowardSide(coverObject)))
                SetIdleState();
            break;
        case WOLF_ST_BUSH_PUT_ON:
            wallAvoid.Reset();
            if (ctxAction.action == CTX_BUSH)
                MoveTowardPoint(&ctxAction.target->pos, ctxAction.target->Facing(), 1);
            else
                MoveTowardPoint(&pos, 0, 0);
            if (ctxAction.action != CTX_BUSH)
                SetIdleState();
            else if (AnimFlags(ANIM_F_FINISHED)) {
                bush = ctxAction.target;
                bush->HandleMessage(this, MSG_BUSH_WORN, 0);
                SetState(WOLF_ST_BUSH_RISE);
            }
            break;
        case WOLF_ST_BUSH_RISE:
        case WOLF_ST_BUSH_UNCROUCH:
            if (BushCommon()) {
                if (stickMag != 0)
                    SetState(WOLF_ST_BUSH_WALK);
                else if (padBits & WOLF_ACT_SNEAK_HELD)
                    SetState(WOLF_ST_BUSH_HIDDEN);
                else if (AnimFlags(ANIM_F_FINISHED))
                    SetState(WOLF_ST_BUSH_IDLE);
            }
            break;
        case WOLF_ST_BUSH_IDLE:
            if (BushCommon()) {
                if (stickMag != 0)
                    SetState(WOLF_ST_BUSH_WALK);
                else if (padBits & WOLF_ACT_SNEAK_HELD)
                    SetState(WOLF_ST_BUSH_CROUCH);
            }
            break;
        case WOLF_ST_BUSH_WALK:
            if (BushCommon()) {
                FootstepSound();
                if (stickMag == 0)
                    SetState(WOLF_ST_BUSH_IDLE);
                else if (padBits & WOLF_ACT_SNEAK_HELD)
                    SetState(WOLF_ST_BUSH_SNEAK);
            }
            break;
        case WOLF_ST_BUSH_CROUCH:
            if (BushCommon()) {
                if (stickMag != 0)
                    SetState(WOLF_ST_BUSH_SNEAK);
                else if (!(padBits & WOLF_ACT_SNEAK_HELD))
                    SetState(WOLF_ST_BUSH_IDLE);
                else if (AnimFlags(ANIM_F_FINISHED))
                    SetState(WOLF_ST_BUSH_HIDDEN);
            }
            break;
        case WOLF_ST_BUSH_HIDDEN:
            if (BushCommon()) {
                if (stickMag != 0)
                    SetState(WOLF_ST_BUSH_SNEAK);
                else if (!(padBits & WOLF_ACT_SNEAK_HELD))
                    SetState(WOLF_ST_BUSH_UNCROUCH);
            }
            break;
        case WOLF_ST_BUSH_SNEAK:
            if (BushCommon()) {
                SneakStepSound();
                if (stateTime * 2 >= animDurationTicks) {
                    if (stickMag == 0)
                        SetState(WOLF_ST_BUSH_CROUCH);
                    else if (!(padBits & WOLF_ACT_SNEAK_HELD))
                        SetState(WOLF_ST_BUSH_WALK);
                }
            }
            break;
        case WOLF_ST_BUSH_SLIDE:
            fxFlags |= WOLF_FA_REQ_TURN;
            PlayLoopSound(SND_SCOGLISS);
            w.result = MoveStep(1);
            if (IsFalling())
                SetState(WOLF_ST_BUSH_FALL);
            else if (!(flags & WOLF_FB_IN_SLIDE_ZONE) && !(flags & WOLF_FB_FORCE_SLIDE) &&
                     groundNormal.y <= -w.tuning[1])
                SetState(WOLF_ST_BUSH_IDLE);
            break;
        case WOLF_ST_BUSH_FALL:
            FallStepLandTo(0x6c);
            break;
        case WOLF_ST_BUSH_JUMP_START:
            MoveStep(0);
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(WOLF_ST_BUSH_JUMP);
            break;
        case WOLF_ST_BUSH_JUMP:
            w.moveFlags = JumpAscendStep(0x82, 0x400);
            if (w.moveFlags & COLL_FLOOR)
                SetState(WOLF_ST_BUSH_FALL);
            else if (stateTime >= 0x400)
                SetState(WOLF_ST_BUSH_JUMP_FALL);
            break;
        case WOLF_ST_BUSH_JUMP_FALL:
            if (FallStepLandTo(0x6c) && AnimFlags(ANIM_F_FINISHED))
                SetState(WOLF_ST_BUSH_FALL);
            break;
        case WOLF_ST_BUSH_LAND:
            if (BushCommon() && AnimFlags(ANIM_F_FINISHED))
                SetState(WOLF_ST_BUSH_IDLE);
            break;
        case WOLF_ST_FLUTE_IDLE:
            heldActionType = HELD_FLUTE_ACTIVE;
            if (MoveStepGrounded()) {
                if (padBits & WOLF_ACT_ACTION_EDGE)
                    SetIdleState();
                else if (stickMag != 0)
                    SetState(WOLF_ST_FLUTE_WALK);
            }
            break;
        case WOLF_ST_FLUTE_WALK:
            heldActionType = HELD_FLUTE_ACTIVE;
            if (MoveStepGrounded()) {
                if (padBits & WOLF_ACT_ACTION_EDGE)
                    SetIdleState();
                else if (stickMag == 0)
                    SetState(WOLF_ST_FLUTE_IDLE);
            }
            break;
        case WOLF_ST_ELASTIC_TAKE_BACK:
            if (FaceTargetActionStep(CTX_ELASTIC_TAKE_BACK)) {
                ClearActiveItem();
                ctxAction.target->HandleMessage(this, MSG_INVENTORY_STORED, 0);
                ctxAction.target->RemoveFromWorld();
                Inventory_Add(ctxAction.target);
                ItemFly_Start(ctxAction.target, ITEMFLY_TO_INVENTORY, &ctxAction.target->pos);
                SetState(WOLF_ST_PICKUP_END);
            }
            break;
        case WOLF_ST_ELASTIC_GRAB:
            if (FaceTargetActionStep(CTX_ELASTIC_GRAB)) {
                SetActiveItem(ctxAction.target);
                ctxAction.target->HandleMessage(this, MSG_USE, 0);
                SetIdleState();
            }
            break;
        case WOLF_ST_ELASTIC_PULL_IDLE:
            if (StrafeFixedFacingStep(lineHeading) && stickMag != 0)
                SetState(WOLF_ST_ELASTIC_PULL_MOVE);
            break;
        case WOLF_ST_ELASTIC_PULL_MOVE:
            if (StrafeFixedFacingStep(lineHeading)) {
                UpdateMoveLoopSound();
                FootstepSound();
                if (stickMag == 0)
                    SetState(WOLF_ST_ELASTIC_PULL_IDLE);
            }
            break;
        case WOLF_ST_DETECTOR_START:
            if (State2B_Step() && AnimFlags(ANIM_F_FINISHED))
                SetState(WOLF_ST_DETECTOR_IDLE);
            break;
        case WOLF_ST_DETECTOR_IDLE:
            heldActionType = HELD_DETECTOR_ACTIVE;
            if (MoveStepGrounded()) {
                if (padBits & WOLF_ACT_ACTION_EDGE)
                    SetState(WOLF_ST_DETECTOR_END);
                else if (stickMag != 0)
                    SetState(WOLF_ST_DETECTOR_WALK);
            }
            break;
        case WOLF_ST_DETECTOR_WALK:
            heldActionType = HELD_DETECTOR_ACTIVE;
            if (MoveStepGrounded()) {
                FootstepSound();
                if (padBits & WOLF_ACT_ACTION_EDGE)
                    SetState(WOLF_ST_DETECTOR_END);
                else if (stickMag == 0)
                    SetState(WOLF_ST_DETECTOR_IDLE);
            }
            break;
        case WOLF_ST_UMBRELLA_OPEN:
            if (State2B_Step() && AnimFlags(ANIM_F_FINISHED))
                SetState(WOLF_ST_UMBRELLA_IDLE);
            break;
        case WOLF_ST_UMBRELLA_IDLE:
            if (SlowFallItemGroundStep() && stickMag != 0)
                SetState(WOLF_ST_UMBRELLA_WALK);
            break;
        case WOLF_ST_UMBRELLA_WALK:
            if (SlowFallItemGroundStep()) {
                FootstepSound();
                if (stickMag == 0)
                    SetState(WOLF_ST_UMBRELLA_IDLE);
            }
            break;
        case WOLF_ST_UMBRELLA_GLIDE:
            SlowFallItemFallStep();
            break;
        case WOLF_ST_UMBRELLA_JUMP_START:
            MoveStep(0);
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(WOLF_ST_UMBRELLA_JUMP);
            break;
        case WOLF_ST_UMBRELLA_JUMP:
            SlowFallItemAscendStep(200, 0x4fd, 0);
            break;
        case WOLF_ST_UMBRELLA_AIR_HOP:
            SlowFallItemAscendStep(100, 0x599, 1);
            break;
        case WOLF_ST_UMBRELLA_HOP_FALL:
            heldActionType = HELD_UMBRELLA_OPEN;
            w.moveFlags = FallStep(32000, 0);
            if (padBits & WOLF_ACT_ACTION_EDGE)
                SetState(WOLF_ST_UMBRELLA_CLOSE_AIR);
            else if ((w.moveFlags & COLL_FLOOR) || stateTime >= 0x666)
                SetState(WOLF_ST_UMBRELLA_GLIDE);
            break;
        case WOLF_ST_UMBRELLA_OPEN_AIR:
            heldActionType = HELD_UMBRELLA_OPEN;
            FallStep(32000, 0);
            if (padBits & WOLF_ACT_ACTION_EDGE)
                SetState(WOLF_ST_UMBRELLA_CLOSE_AIR);
            else if (AnimFlags(ANIM_F_FINISHED))
                SetState(WOLF_ST_UMBRELLA_GLIDE);
            break;
        case WOLF_ST_UMBRELLA_CLOSE_AIR:
            FallStep(32000, 0);
            if (padBits & WOLF_ACT_ACTION_EDGE)
                SetState(WOLF_ST_UMBRELLA_OPEN_AIR);
            else if (AnimFlags(ANIM_F_FINISHED))
                SetState(WOLF_ST_FALL);
            break;
        case WOLF_ST_GEYSER_EJECT:
            GetShadow()->Reproject();
            StopMotion();
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(WOLF_ST_FALL);
            break;
        case WOLF_ST_SHEEPCOSTUME_PUT_ON:
            if (State2B_Step() && (!(fxFlags & WOLF_FA_PROP6_BILLBOARD) || propBody.AnimFlags(ANIM_F_FINISHED))) {
                SetState(WOLF_ST_SHEEPCOSTUME_APPEAR);
                Fx6_Start(0);
            }
            break;
        case WOLF_ST_SHEEPCOSTUME_APPEAR:
            if (State2B_Step() && (!(fxFlags & WOLF_FA_PROP6_BILLBOARD) || propBody.AnimFlags(ANIM_F_FINISHED)))
                SetState(WOLF_ST_SHEEPCOSTUME_IDLE);
            break;
        case WOLF_ST_SHEEPCOSTUME_IDLE:
            if (ModelSet3GroundStep() && stickMag != 0)
                SetState(WOLF_ST_SHEEPCOSTUME_WALK);
            break;
        case WOLF_ST_SHEEPCOSTUME_WALK:
            if (ModelSet3GroundStep() && stickMag == 0)
                SetState(WOLF_ST_SHEEPCOSTUME_IDLE);
            break;
        case WOLF_ST_SHEEPCOSTUME_JUMP_START:
            MoveStep(0);
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(WOLF_ST_SHEEPCOSTUME_JUMP);
            break;
        case WOLF_ST_SHEEPCOSTUME_JUMP:
            w.moveFlags = JumpAscendStep(0x78, 0x4cc);
            if (w.moveFlags & COLL_FLOOR)
                SetState(WOLF_ST_SHEEPCOSTUME_FALL);
            else if (stateTime >= 0x4cc)
                SetState(WOLF_ST_SHEEPCOSTUME_JUMP_FALL);
            break;
        case WOLF_ST_SHEEPCOSTUME_JUMP_FALL:
            if (FallStepLandTo(0x85) && AnimFlags(ANIM_F_FINISHED))
                SetState(WOLF_ST_SHEEPCOSTUME_FALL);
            break;
        case WOLF_ST_SHEEPCOSTUME_FALL:
            FallStepLandTo(0x85);
            break;
        case WOLF_ST_SHEEPCOSTUME_SLIDE:
            fxFlags |= WOLF_FA_REQ_TURN;
            PlayLoopSound(SND_SCOGLISS);
            w.result = MoveStep(1);
            if (IsFalling())
                SetState(WOLF_ST_SHEEPCOSTUME_FALL);
            else if (!(flags & WOLF_FB_IN_SLIDE_ZONE) && !(flags & WOLF_FB_FORCE_SLIDE) &&
                     groundNormal.y <= -w.tuning[1])
                SetState(WOLF_ST_SHEEPCOSTUME_IDLE);
            break;
        case WOLF_ST_SHEEPCOSTUME_BLEAT:
            Stick_Clear(&stickX);
            w.result = MoveStep(0);
            if (IsFalling())
                SetState(WOLF_ST_SHEEPCOSTUME_FALL);
            else if (w.result)
                SetState(WOLF_ST_SHEEPCOSTUME_SLIDE);
            else if (AnimFlags(ANIM_F_FINISHED)) {
                /* cast kept: MSG_SHEEP_CALL's arg is a flag in the void * */
                Scenaric_BroadcastInRadius(CLASSID_SHEEP, pos.y - 150, pos.y + 150, 800, MSG_SHEEP_CALL, (void *)1, 0);
                if (sheepCostumeListener)
                    /* cast kept: MSG_SHEEP_CALL's arg is a flag in the void * */
                    sheepCostumeListener->HandleMessage(this, MSG_SHEEP_CALL, (void *)1);
                SetState(WOLF_ST_SHEEPCOSTUME_IDLE);
            }
            break;
        case WOLF_ST_SHEEPCOSTUME_GRABBED:
            StopMotion();
            if (AnimFlags(ANIM_F_FINISHED)) {
                switch (AnimId()) {
                    case ACOYOT01_ANIM_JUMP4:
                        PlayAnim(ACOYOT01_ANIM_JUMP3, 1, 1);
                        break;
                }
            }
            break;
        case WOLF_ST_SHEEPCOSTUME_DIE_THROWN:
            Stick_Clear(&stickX);
            FallStep(fallLimitY, 0);
            if (stateTime >= 0x1333) {
                Camera_StartShake(0x14, 0x1000);
                SetStateKeepAnimStopSound(WOLF_ST_SHEEPCOSTUME_DIE_LANDED);
            }
            break;
        case WOLF_ST_SHEEPCOSTUME_TAKE_OFF:
            if (State2B_Step() && (!(fxFlags & WOLF_FA_PROP6_BILLBOARD) || propBody.AnimFlags(ANIM_F_FINISHED))) {
                SetState(WOLF_ST_COSTUME_OFF_END);
                Fx6_Start(0);
            }
            break;
        case WOLF_ST_COSTUME_OFF_END:
            if (State2B_Step() && (!(fxFlags & WOLF_FA_PROP6_BILLBOARD) || propBody.AnimFlags(ANIM_F_FINISHED)))
                SetIdleState();
            break;
        case WOLF_ST_WOLFTRAP_ENTER:
            if (FaceTargetActionStep(CTX_WOLFTRAP)) {
                ctxAction.target->HandleMessage(this, MSG_USE, 0);
                SetState(WOLF_ST_WOLFTRAP_START);
            }
            break;
        case WOLF_ST_WOLFTRAP_START:
        case WOLF_ST_WOLFTRAP_LOOP:
        case WOLF_ST_WOLFTRAP_REACT:
            ctxAction.action = CTX_SQUIRM;
            flags |= WOLF_FB_FROZEN_BUT_PROMPT;
            GetShadow()->Reproject();
            StopMotion();
            if (!(flags & WOLF_FB_IN_WOLFTRAP))
                SetState(WOLF_ST_WOLFTRAP_EXIT);
            else {
                switch (state) {
                    case WOLF_ST_WOLFTRAP_START:
                        if (AnimFlags(ANIM_F_FINISHED))
                            SetState(WOLF_ST_WOLFTRAP_LOOP);
                        break;
                    case WOLF_ST_WOLFTRAP_LOOP:
                        if (flags & WOLF_FB_WOLFTRAP_REACT)
                            SetState(WOLF_ST_WOLFTRAP_REACT);
                        break;
                    case WOLF_ST_WOLFTRAP_REACT:
                        if (AnimFlags(ANIM_F_FINISHED)) {
                            SetState(WOLF_ST_WOLFTRAP_LOOP);
                            ClearFlags(WOLF_FB_WOLFTRAP_REACT);
                        }
                        break;
                }
            }
            break;
        case WOLF_ST_GOSSAMER_GRAB_2:
            GetShadow()->Reproject();
            StopMotion();
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(WOLF_ST_GOSSAMER_GRAB_3);
            break;
        case WOLF_ST_GOSSAMER_GRAB_1:
        case WOLF_ST_GOSSAMER_GRAB_3:
            GetShadow()->Reproject();
            StopMotion();
            ctxAction.action = CTX_SQUIRM;
            flags |= WOLF_FB_FROZEN_BUT_PROMPT;
            break;
        case WOLF_ST_TIMEMACHINE_USE:
            if (State2B_Step() && AnimFlags(ANIM_F_FINISHED)) {
                SetIdleState();
                heldObject->HandleMessage(this, MSG_HELD_STATE_BEGIN, 0);
            }
            break;
        case WOLF_ST_RABBITCOSTUME_PUT_ON:
            if (State2B_Step() && (!(fxFlags & WOLF_FA_PROP6_BILLBOARD) || propBody.AnimFlags(ANIM_F_FINISHED))) {
                costume = WMS_RABBITCOSTUME;
                SetState(WOLF_ST_RABBITCOSTUME_ON_END);
                Fx6_Start(0);
            }
            break;
        case WOLF_ST_RABBITCOSTUME_ON_END:
            if (State2B_Step() && (!(fxFlags & WOLF_FA_PROP6_BILLBOARD) || propBody.AnimFlags(ANIM_F_FINISHED)))
                SetIdleState();
            break;
        case WOLF_ST_RABBITCOSTUME_TAKE_OFF:
            if (State2B_Step() && (!(fxFlags & WOLF_FA_PROP6_BILLBOARD) || propBody.AnimFlags(ANIM_F_FINISHED))) {
                costume = WMS_RALPH;
                SetState(WOLF_ST_RABBITCOSTUME_OFF_END);
                Fx6_Start(0);
            }
            break;
        case WOLF_ST_RABBITCOSTUME_OFF_END:
            if (State2B_Step() && (!(fxFlags & WOLF_FA_PROP6_BILLBOARD) || propBody.AnimFlags(ANIM_F_FINISHED)))
                SetIdleState();
            break;
        case WOLF_ST_REMOTE_PRESS:
            if (State2B_Step()) {
                if (AnimFlags(ANIM_F_FINISHED)) {
                    heldObject->HandleMessage(this, MSG_HELD_STATE_BEGIN, 0);
                    SetIdleState();
                }
            } else
                heldObject->HandleMessage(this, MSG_HELD_STATE_BEGIN, 0);
            break;
        case WOLF_ST_REMOTE_START:
            if (State2B_Step() && AnimFlags(ANIM_F_FINISHED))
                SetState(WOLF_ST_REMOTE_DRIVE);
            break;
        case WOLF_ST_REMOTE_DRIVE:
            heldActionType = HELD_REMOTE_ACTIVE;
            if (State2B_Step() && (padBits & WOLF_ACT_ACTION_EDGE))
                SetState(WOLF_ST_REMOTE_END);
            break;
        case WOLF_ST_DIE_CANNONBALL:
            Stick_Clear(&stickX);
            FallStep(fallLimitY, 0);
            if (stateTime >= 0x1000 && !(fxFlags & WOLF_FA_PROP0))
                Fx0_Start();
            break;
        case WOLF_ST_USE_CTX1A:
            if (FaceTargetActionStep(CTX_SWIRLSIGN)) {
                ctxAction.target->HandleMessage(this, MSG_USE, 0);
                SetState(WOLF_ST_USE_CTX1A_END);
            }
            break;
        case WOLF_ST_FISHING_CAST:
            if (ForcedHeadingGroundStepChecked(lineHeading)) {
                if (heldObject->HandleMessage(this, MSG_ROD_TIP_BLOCKED, &pos))
                    SetIdleState();
                else if (AnimFlags(ANIM_F_FINISHED))
                    SetState(WOLF_ST_FISHING_IDLE);
            }
            break;
        case WOLF_ST_FISHING_IDLE:
            if (SidestepHeldStep(lineHeading) && stickX != 0)
                SetState(WOLF_ST_FISHING_STEP_A);
            break;
        case WOLF_ST_FISHING_STEP_A:
            if (SidestepHeldStep(lineHeading)) {
                FootstepSound();
                if (stickX <= 0) {
                    if (stickX == 0)
                        SetState(WOLF_ST_FISHING_IDLE);
                    else
                        SetState(WOLF_ST_FISHING_STEP_B);
                }
            }
            break;
        case WOLF_ST_FISHING_STEP_B:
            if (SidestepHeldStep(lineHeading)) {
                FootstepSound();
                if (stickX >= 0) {
                    if (stickX == 0)
                        SetState(WOLF_ST_FISHING_IDLE);
                    else
                        SetState(WOLF_ST_FISHING_STEP_A);
                }
            }
            break;
        case WOLF_ST_FISHING_REEL:
            w.reel = stickY;
            w.notReeling = SDW_ABS(stickX) > SDW_ABS(stickY) || SDW_ABS(stickY) < 0x55;
            heldActionType = HELD_FISHINGROD_ACTIVE;
            if (State2B_Step()) {
                if (w.notReeling)
                    SetState(WOLF_ST_FISHING_IDLE);
                else {
                    w.reel = (w.reel * 300) >> 8;
                    w.reel = (w.reel * g_dt) >> 12;
                    if (w.reel > 0)
                        /* cast kept: the reel amount is a number in the void * */
                        heldObject->HandleMessage(this, MSG_ROD_REEL_IN, (void *)w.reel);
                    else
                        /* cast kept: the reel amount is a number in the void * */
                        heldObject->HandleMessage(this, MSG_ROD_REEL_OUT, (void *)-w.reel);
                }
            }
            break;
        case WOLF_ST_GHOSTCOSTUME_PUT_ON:
            if (State2B_Step() && (!(fxFlags & WOLF_FA_PROP6_BILLBOARD) || propBody.AnimFlags(ANIM_F_FINISHED))) {
                SetState(WOLF_ST_GHOSTCOSTUME_APPEAR);
                Fx6_Start(0);
            }
            break;
        case WOLF_ST_GHOSTCOSTUME_APPEAR:
            if (State2B_Step() && (!(fxFlags & WOLF_FA_PROP6_BILLBOARD) || propBody.AnimFlags(ANIM_F_FINISHED)))
                GhostCostumeSetIdle();
            break;
        case WOLF_ST_GHOSTCOSTUME_IDLE:
            if (GhostCostumeGroundStep()) {
                if (stickMag != 0) {
                    if (padBits & WOLF_ACT_SNEAK_HELD)
                        SetState(WOLF_ST_GHOSTCOSTUME_SNEAK);
                    else
                        SetState(WOLF_ST_GHOSTCOSTUME_WALK);
                } else if (AnimFlags(ANIM_F_FINISHED))
                    NextIdleAnim(g_wolfGhostIdleAnims, g_wolfGhostIdleAnims + 1,
                                 2); /* + one ENTRY (4 bytes), as the u8 view had it */
            }
            break;
        case WOLF_ST_GHOSTCOSTUME_WALK:
            if (GhostCostumeGroundStep()) {
                FootstepSound();
                if (stickMag == 0)
                    GhostCostumeSetIdle();
                else if (padBits & WOLF_ACT_SNEAK_HELD)
                    SetState(WOLF_ST_GHOSTCOSTUME_SNEAK);
            }
            break;
        case WOLF_ST_GHOSTCOSTUME_JUMP_START:
            MoveStep(0);
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(WOLF_ST_GHOSTCOSTUME_JUMP);
            break;
        case WOLF_ST_GHOSTCOSTUME_JUMP:
            w.moveFlags = JumpAscendStep(0x82, 0x400);
            if (w.moveFlags & COLL_FLOOR)
                SetState(WOLF_ST_GHOSTCOSTUME_FALL);
            else if (stateTime >= 0x400)
                SetState(WOLF_ST_GHOSTCOSTUME_JUMP_FALL);
            break;
        case WOLF_ST_GHOSTCOSTUME_JUMP_FALL:
            if (FallStepLandTo(0xb9) && AnimFlags(ANIM_F_FINISHED))
                SetState(WOLF_ST_GHOSTCOSTUME_FALL);
            break;
        case WOLF_ST_GHOSTCOSTUME_LAND:
            if (GhostCostumeGroundStep() && AnimFlags(ANIM_F_FINISHED))
                GhostCostumeSetIdle();
            break;
        case WOLF_ST_GHOSTCOSTUME_FALL:
            FallStepLandTo(0xb9);
            break;
        case WOLF_ST_GHOSTCOSTUME_SLIDE:
            fxFlags |= WOLF_FA_REQ_TURN;
            PlayLoopSound(SND_SCOGLISS);
            w.result = MoveStep(1);
            if (IsFalling())
                SetState(WOLF_ST_GHOSTCOSTUME_FALL);
            else if (!(flags & WOLF_FB_IN_SLIDE_ZONE) && !(flags & WOLF_FB_FORCE_SLIDE) &&
                     groundNormal.y <= -w.tuning[1])
                GhostCostumeSetIdle();
            break;
        case WOLF_ST_GHOSTCOSTUME_SNEAK:
            if (GhostCostumeGroundStep()) {
                SneakStepSound();
                if (stateTime * 2 >= animDurationTicks) {
                    if (stickMag == 0)
                        GhostCostumeSetIdle();
                    else if (!(padBits & WOLF_ACT_SNEAK_HELD))
                        SetState(WOLF_ST_GHOSTCOSTUME_WALK);
                }
            }
            break;
        case WOLF_ST_GHOSTCOSTUME_BOO:
            w.camOwner = Camera_GetScriptOwner();
            if (w.camOwner && w.camOwner->GetClassId() == CLASSID_GHOST)
                w.result = ForcedHeadingGroundStep(HeadingTo(&w.camOwner->pos));
            else
                w.result = ForcedHeadingGroundStep(Facing());
            if (IsFalling())
                SetState(WOLF_ST_GHOSTCOSTUME_FALL);
            else if (w.result)
                SetState(WOLF_ST_GHOSTCOSTUME_SLIDE);
            else if (AnimFlags(ANIM_F_FINISHED))
                GhostCostumeSetIdle();
            break;
        case WOLF_ST_GHOSTCOSTUME_CAPTURED:
            PlayLoopSound(SND_SCOCOLD);
            ForcedHeadingGroundStep(interactHeading);
            break;
        case WOLF_ST_DANCE_GOTO:
            if (MoveTowardPoint(&interactPos, 0, 0)) {
                interactHeading = Facing();
                danceMove = -1;
                SetState(WOLF_ST_DANCE);
            }
            break;
        case WOLF_ST_DANCE:
            ForcedHeadingGroundStep(interactHeading);
            if (danceMove == 0xff) {
                if (g_gameTime >> 11 != (g_gameTime - g_dt) >> 11)
                    PlayAnim(ACOYOT01_ANIM_RUN1, 1, 1);
                w.canStep = 1;
            } else {
                w.canStep = runTapTimer >= animDurationTicks >> 1;
                if (AnimFlags(ANIM_F_FINISHED)) {
                    PlayAnim(ACOYOT01_ANIM_RUN1, 1, 1);
                    danceMove = 0xff;
                }
            }
            if (w.canStep) {
                for (w.move = 0; w.move < 4; w.move++) {
                    if (PAD_PRESSED(pad, g_wolfDanceMoves[w.move].padMask)) {
                        PlayAnim(g_wolfDanceMoves[w.move].animId, 0, 1);
                        danceMove = (u8)w.move;
                        runTapTimer = g_dt;
                        animDurationTicks = (Anim_GetDurationMs(Inst(), AnimId(), 1) << 12) / 1000;
                    }
                }
            }
            break;
        case WOLF_ST_GHOSTCOSTUME_TAKE_OFF:
            if (State2B_Step() && (!(fxFlags & WOLF_FA_PROP6_BILLBOARD) || propBody.AnimFlags(ANIM_F_FINISHED))) {
                SetState(WOLF_ST_GHOSTCOSTUME_OFF_END);
                Fx6_Start(0);
            }
            break;
        case WOLF_ST_GHOSTCOSTUME_OFF_END:
            if (State2B_Step() && (!(fxFlags & WOLF_FA_PROP6_BILLBOARD) || propBody.AnimFlags(ANIM_F_FINISHED)))
                SetIdleState();
            break;
        case WOLF_ST_INFLATE_START:
            if (State2B_Step() && AnimFlags(ANIM_F_FINISHED)) {
                heldObject->HandleMessage(this, MSG_INFSHEEP_INFLATE, 0);
                SetState(WOLF_ST_INFLATE);
            }
            break;
        case WOLF_ST_INFLATE:
            PlayLoopSound(SND_WOLF_INFLATE);
            if (State2B_Step() && stateTime >= 0x3000) {
                Inventory_Remove(heldObject);
                SetMode(WOLF_MODE_CARRY, WOLFC_ST_IDLE);
            }
            break;
        case WOLF_ST_MAGNET_DRAG_IDLE:
            if (StrafeAroundFocusStep() && stickMag != 0)
                SetState(WOLF_ST_MAGNET_DRAG_MOVE);
            break;
        case WOLF_ST_MAGNET_DRAG_MOVE:
            if (StrafeAroundFocusStep()) {
                FootstepSound();
                if (stickMag == 0)
                    SetState(WOLF_ST_MAGNET_DRAG_IDLE);
                else if (SDW_ABS(SDW_ANGLE_DIFF(GetStickHeading(moveDir), HeadingTo(magnetTarget->GetPos()))) < 0x200) {
                    if (AnimId() != ACOYOT01_ANIM_MAGNET3)
                        PlayAnim(ACOYOT01_ANIM_MAGNET3, 1, 1);
                } else if (AnimId() != ACOYOT01_ANIM_MAGNET2)
                    PlayAnim(ACOYOT01_ANIM_MAGNET2, 1, 1);
            }
            break;
        case WOLF_ST_HOOVER_HOLD:
            FaceStickStep();
            break;
        case WOLF_ST_PANIC_RUN_BEES:
            flags |= WOLF_FB_RUN;
            RunMove(1, 1, &w.moveFlags);
            break;
        case WOLF_ST_PANIC_RUN_BURNT:
            flags |= WOLF_FB_RUN;
            RunMove(1, 1, &w.moveFlags);
            if (stateTime >= 0xa000)
                SetIdleState();
            break;
        case WOLF_ST_DEFUSE_MINE:
            w.mineHeading = HeadingTo(&interactPos);
            w.mineDist = Vec3s_DistSqXZ(&pos, &interactPos) - 0x640;
            w.mineDist = SDW_ABS(w.mineDist);
            if (w.mineDist >= 4 && stateTime <= 0x1000) {
                w.minePoint.x = interactPos.x + ((g_sinTable4096[w.mineHeading] * 40) >> 12);
                w.minePoint.z = interactPos.z + ((g_pCosTable[w.mineHeading] * 40) >> 12);
                w.minePoint.y = interactPos.y;
                wallAvoid.Reset();
                MoveTowardPoint(&w.minePoint, w.mineHeading, 1);
            } else
                ForcedHeadingGroundStep(w.mineHeading);
            if (CheckFallOrSlide(0)) {
                if (CanControl())
                    SetIdleState();
                else if (AnimId() == ACOYOT01_ANIM_MINE) {
                    if (PAD_PRESSED(pad, (u16)~PAD_TRIANGLE) || PAD_PRESSED(pad, (u16)~PAD_CIRCLE) ||
                        PAD_PRESSED(pad, (u16)~PAD_SQUARE) || PAD_PRESSED(pad, (u16)~PAD_CROSS))
                        PlayAnim(ACOYOT01_ANIM_MINE2, 0, 1);
                } else if (AnimFlags(ANIM_F_FINISHED))
                    PlayAnim(ACOYOT01_ANIM_MINE, 1, 1);
            }
            break;
        case WOLF_ST_TIMEKEEPER_USE:
            if (ForcedHeadingGroundStepChecked(interactHeading) && AnimFlags(ANIM_F_FINISHED))
                SetIdleState();
            break;
        case WOLF_ST_ENTER_LEVEL_DOOR:
            ForcedHeadingGroundStepChecked(interactHeading);
            break;
        case WOLF_ST_PUSH_ICECUBE:
            if (FaceTargetActionStep(CTX_ICECUBE)) {
                ctxAction.target->HandleMessage(this, MSG_USE, 0);
                SetState(WOLF_ST_KICK_ICECUBE_END);
            }
            break;
        case WOLF_ST_FLATTENED_START:
            if (State2B_Step() && AnimFlags(ANIM_F_FINISHED))
                SetState(WOLF_ST_FLATTENED_IDLE);
            break;
        case WOLF_ST_FLATTENED_IDLE:
            if (FlattenedGroundStep() && stickMag != 0)
                SetState(WOLF_ST_FLATTENED_WALK);
            break;
        case WOLF_ST_FLATTENED_WALK:
            if (FlattenedGroundStep() && stickMag == 0)
                SetState(WOLF_ST_FLATTENED_IDLE);
            break;
        case WOLF_ST_FLATTENED_FALL:
            w.moveFlags = FallStep(32000, 0);
            if (w.moveFlags & COLL_FLOOR)
                SetState(WOLF_ST_FLATTENED_IDLE);
            break;
        case WOLF_ST_FLATTENED_KNOCKBACK:
            w.moveFlags = LeapStep(0x12c, 0x2000);
            if (w.moveFlags)
                SetState(WOLF_ST_FLATTENED_FALL);
            else if (AnimId() == ACOYOT01_ANIM_SQUAKICK && AnimFlags(ANIM_F_FINISHED))
                PlayAnim(ACOYOT01_ANIM_SQUAFALL, 1, 1);
            break;
        case WOLF_ST_FLATTENED_RECOVER:
            MoveStep(0);
            if (AnimFlags(ANIM_F_FINISHED))
                SetIdleState();
            break;
        case WOLF_ST_GHOST_CAPTURED:
            ForcedHeadingGroundStep(interactHeading);
            if (AnimId() == ACOYOT01_ANIM_FEAR && AnimFlags(ANIM_F_FINISHED)) {
                PlayAnim(ACOYOT01_ANIM_FEAR2, 1, 1);
                PlayLoopSound(SND_SCOCOLD);
            }
            break;
        case WOLF_ST_KNOCKBACK:
            w.moveFlags = LeapStep(0x1c2, 0x800);
            if (w.moveFlags)
                SetState(WOLF_ST_FALL);
            break;
        case WOLF_ST_FROZEN_GEYSER:
            GetShadow()->Reproject();
            StopMotion();
            if (stateTime >= 0x2000 && AnimId() != ACOYOT01_ANIM_WHIRL2) {
                PlayAnim(ACOYOT01_ANIM_WHIRL2, 0, 1);
                if (fxFlags & WOLF_FA_PROP_ICE)
                    propBody.PlayAnim(AGLACON1_ANIM_WHIRL2, 0, 1);
            }
            break;
        case WOLF_ST_CATAPULT_OPERATE:
            if (State2B_Step()) {
                if (CanControl())
                    SetIdleState();
                else {
                    w.result = Pad_MenuHeld((u16)~PAD_LEFT) || Pad_MenuHeld((u16)~PAD_RIGHT);
                    if (AnimId() == ACOYOT01_ANIM_STAND0) {
                        if (w.result)
                            PlayAnim(ACOYOT01_ANIM_TURN, 1, 1);
                    } else if (AnimFlags(ANIM_F_FINISHED) && !w.result)
                        PlayAnim(ACOYOT01_ANIM_STAND0, 1, 1);
                }
            }
            break;
        case WOLF_ST_DIE_BLACKHOLE:
            pulledTo = pos;
            pullRate = 100;
            Vec3s_ApproachPoint(&pulledTo, &killer->pos, &pullRate, 100, 100, 100);
            SetPosition(&pulledTo);
            StopMotion();
    }
}

/* ---- the per-frame update ---- */
/* 0x4833c0 - the vtable +4 override, once per frame: classify the zones Ralph is in (ice, slide, updraft), advance the
 * timers, the quick inventory and the deferred equip, run the context scan and the state machine, then the shadow /
 * footprint / death / water zones, the held object's position, the prop and splash bodies, the tint, the camera, the
 * trails, the recent-contact flags, the animation and the HUD prompts. */
void Wolf::Update()
{
    ScnObject *selItem;
    Vec3s flyFrom;
    Box *prevWater;
    const u16 *surfaceTuning;
    Pad *pad;
    s32 falling;
    s32 inControl;
    Box *killZone;
    WolfStateDesc *descriptor;
    WolfMoveBank *modeBank;

    if (Zones_Get(ZONE_ICE)->FindContaining(&pos)) /* ICEBOX */
        surface = WOLF_SURFACE_ICE;
    else
        surface = WOLF_SURFACE_NORMAL;
    if (Zones_Get(ZONE_SLIDE)->FindContaining(&pos)) /* SLIDEBOX */
        flags |= WOLF_FB_IN_SLIDE_ZONE;
    else
        ClearFlags(WOLF_FB_IN_SLIDE_ZONE);
    updraftZone = 0;
    if (!(flags & WOLF_FB_DEAD)) {
        updraftZone = Zones_Get(ZONE_GRAVITY)->FindContaining(&pos);
        if (updraftZone && state != WOLF_ST_UPDRAFT && state != WOLF_ST_UPDRAFT_FAST) {
            if (mode == WOLF_MODE_CARRY)
                DropHeld(WOLF_DROP_RELEASE);
            else
                DropHeld(0);
            SetMode(WOLF_MODE_NORMAL, WOLF_ST_UPDRAFT);
        }
    }
    modeBank = &g_wolfMoveBank0[mode];
    descriptor = &modeBank->states[state];
    surfaceTuning = modeBank->surfaceTuningU[surface];
    if (playerIndex == 1)
        pad = &g_pad2;
    else
        pad = &g_pad;
    stateTime += g_dt;
    runTapTimer += g_dt;
    footstepTimer += g_dt;
    if (footstepTimer > 0x1e000)
        footstepTimer = 0x1e000;
    airTime += g_dt;
    if (airTime > 0x1e000)
        airTime = 0x1e000;
    if ((descriptor->flags & WSF_GROUND_STATE) &&
        ((flags & WOLF_FB_IN_SLIDE_ZONE) || (flags & WOLF_FB_FORCE_SLIDE) || groundNormal.y >= -surfaceTuning[0] ||
         (descriptor->flags & WSF_SLIDING)))
        slopeTime += g_dt;
    else {
        slopeTime = 0;
        ClearFlags(WOLF_FB_FORCE_SLIDE);
    }
    if (slopeTime > 0x1e000)
        slopeTime = 0x1e000;
    ClearFxFlags(WOLF_FA_REQ_SKID | WOLF_FA_REQ_TURN);
    ClearFlags(WOLF_FB_MOVED);
    falling = IsFalling();
    if (QuickInventory(pad))
        SwapHeldItem(1, 1);
    if (flags & WOLF_FB_EQUIP_PENDING) {
        selItem = Inventory_GetSelectedObject();
        if (heldObject || !selItem || mode != WOLF_MODE_NORMAL || !(descriptor->flags & WSF_ITEM_WHEEL_OK))
            ClearFlags(WOLF_FB_EQUIP_PENDING);
        if (flags & WOLF_FB_EQUIP_PENDING) {
            if (g_gameFlags & GF_ITEM_FLY) {
                flyFrom.x = g_wolfEquipPreviewOffset.x + pos.x;
                flyFrom.y = g_wolfEquipPreviewOffset.y + pos.y;
                flyFrom.z = g_wolfEquipPreviewOffset.z + pos.z;
                ItemFly_SetFromPos(&flyFrom);
            } else {
                EquipItem(selItem);
                ClearFlags(WOLF_FB_EQUIP_PENDING);
            }
        } else if (g_gameFlags & GF_ITEM_FLY)
            ItemFly_Stop();
    }
    ScanContextActions();
    StateMachine(pad);
    if (Zones_Get(ZONE_SHADOW)->FindContaining(&pos)) /* shadow zones */
        flags |= WOLF_FB_IN_SHADOW_ZONE;
    else
        ClearFlags(WOLF_FB_IN_SHADOW_ZONE);
    if (Zones_Get(ZONE_PRINTS)->FindContaining(&pos)) /* PRINTSBOX */
        flags |= WOLF_FB_IN_FOOTPRINT_ZONE;
    else
        ClearFlags(WOLF_FB_IN_FOOTPRINT_ZONE);
    if (!(flags & WOLF_FB_DEAD) || (descriptor->flags & WSF_KILL_WHEN_DEAD)) {
        killZone = Zones_Get(ZONE_DEATH)->FindContaining(&pos);
        if (killZone) {
            flags |= WOLF_FB_DEAD;
            switch (killZone->flags) {
                case DZ_37_BURN:
                    Die(WOLF_ST_DIE_ZONE, WOLF_DROP_RELEASE | WOLF_DROP_STOP, 0x2000);
                    fxFlags |= WOLF_FA_BURNT;
                    break;
                case DZ_37_PROP1:
                    Die(WOLF_ST_DIE_ZONE, WOLF_DROP_RELEASE | WOLF_DROP_STOP, 0x2000);
                    FxGeneric_Start(WOLF_PROP_SABLE1, ASABLE01_ANIM_SAND1);
                    break;
                case DZ_37_PROP2:
                    Die(WOLF_ST_DIE_ZONE, WOLF_DROP_RELEASE | WOLF_DROP_STOP, 0x2000);
                    FxGeneric_Start(WOLF_PROP_SABLE2, ASABLE02_ANIM_SAND1);
                    break;
                case DZ_FALL:
                    NotifyAttached_6A();
                    DropHeld(0);
                    fallLimitY = killZone->max[1];
                    SetState(WOLF_ST_FALL_TO_DEATH);
                    break;
                case DZ_BLOWN_AWAY:
                    Die(WOLF_ST_DIE_BLOWN_AWAY, 0, 0x4000);
                    break;
                case DZ_RESTART:
                    RestartNoAnim(0x1000);
                    break;
                default:
                    Sound_Play(SND_SCOFALL, this, 0x7f, SNDF_NO_RETRIGGER, 0x1000);
                    RestartNoAnim(0x1000);
            }
        }
    }
    prevWater = waterZone;
    waterZone = Zones_Get(ZONE_WATER)->FindContaining(&pos);
    if (waterZone) {
        if (!prevWater && falling && pos.y <= waterZone->min[1] + waterDepthOffset * 2)
            Fx2Body_StartAtSurface();
        timeOutOfWater = 0;
    } else {
        timeOutOfWater += g_dt;
        if (timeOutOfWater > 0x3c000)
            timeOutOfWater = 0x3c000;
    }
    ClearFlags(WOLF_FB_IN_WATER);
    descriptor = &modeBank->states[state];
    if (waterZone && !(descriptor->flags & WSF_IN_WATER)) {
        flags |= WOLF_FB_IN_WATER;
        if (!(flags & WOLF_FB_DEAD) && pos.y > waterZone->min[1] + waterDepthOffset) {
            if (mode == WOLF_MODE_CARRY)
                DropHeld(WOLF_DROP_RELEASE);
            else
                DropHeld(0);
            if (falling || (waterZone->flags & (ZONE_WATER_FREEZING | ZONE_WATER_NO_SURFACE)))
                SetMode(WOLF_MODE_NORMAL, WOLF_ST_WATER_PLUNGE);
            else
                SetMode(WOLF_MODE_NORMAL, WOLF_ST_WADE);
        }
    }
    if (heldObject)
        heldObject->SetPosition(&pos);
    if (fxFlags & (WOLF_FA_PROP0 | WOLF_FA_PROP6_BILLBOARD | WOLF_FA_DEATHZONE_PROP | WOLF_FA_PROP7 | WOLF_FA_PROP4 |
                   WOLF_FA_PROP8))
        propBody.AdvanceAnim();
    else if (fxFlags & WOLF_FA_PROP5) {
        if (propBody.AnimFlags(ANIM_F_FINISHED))
            Fx5_Stop();
        else
            propBody.AdvanceAnim();
    } else if (fxFlags & WOLF_FA_PROP_ICE) {
        if (waterZone && SDW_ABS(pos.y - waterZone->min[1] - 0x69) <= 5) {
            if (propBody.AnimId() == ACOYOT01_ANIM_EAR1)
                propBody.PlayAnim(AGLACON1_ANIM_STAND1, 1, 0);
        } else {
            if (propBody.AnimId() == ACOYOT01_ANIM_DIVE1)
                propBody.PlayAnim(AGLACON1_ANIM_STAND2, 1, 1);
        }
        propBody.AdvanceAnim();
    }
    if (fxFlags & WOLF_FA_SPLASH_ACTIVE) {
        splashBody.AdvanceAnim();
        if (splashBody.AnimFlags(ANIM_F_FINISHED))
            Fx2Body_Stop();
    }
    magnetTarget = 0;
    if (fxFlags & WOLF_FA_BURNT)
        SetTint(0, 0x1000, 1);
    else if (flags & WOLF_FB_IN_SHADOW_ZONE)
        SetTint(0, 0xc00, 1);
    else if (glowDist < 0x200) {
        SetTint(0x600060, 0x1000 - (glowDist << 12) / 512, 1);
        glowDist = 0x7fffffff;
    } else
        SetTint(0, 0, 0);
    UpdateCamera(pad);
    UpdateTrailFx();
    ClearFlags(WOLF_FB_FORCE_SLIDE | WOLF_FB_AXIS_LOCK_A | WOLF_FB_AXIS_LOCK_B | WOLF_FB_FORCE_FALL |
               WOLF_FB_JUST_UNFROZEN | WOLF_FB_NO_LOOK);
    if (g_gameTime - lastObjectContactTime <= 0x400)
        flags |= WOLF_FB_RECENT_OBJECT_CONTACT;
    else
        ClearFlags(WOLF_FB_RECENT_OBJECT_CONTACT);
    if (g_gameTime - lastContact2Time <= 0x400)
        flags |= WOLF_FB_RECENT_CONTACT2;
    else
        ClearFlags(WOLF_FB_RECENT_CONTACT2);
    AdvanceAnim();
    if (!grabber || !grabber->HandleMessage(this, MSG_DRAW_PROMPT, 0)) {
        inControl = (!(flags & WOLF_FB_FROZEN) || (flags & WOLF_FB_FROZEN_BUT_PROMPT)) &&
                    !(flags & (WOLF_FB_DEAD | WOLF_FB_WON));
        DrawActionPrompt(&ctxAction, &coverAction,
                         inControl && !(g_gameFlags & GF_ITEM_WHEEL_OPEN) && !(flags & WOLF_FB_PROMPT_DISABLED));
        Hud_DrawItemPrompt(&itemPromptClassId, inControl && itemPromptClassId);
    }
}

/* ---- the message handler ---- */
/* 0x484550 - vtable +0x10. Returns 1 for most handled messages, 0 for ignored ones, or the answer of a query. */
/* BYTES(cast): 0xfffd written out: the original has the 16-bit constant (and ecx,0xfffd at 0x4869d3) */
s32 Wolf::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    WolfStateDesc *desc = &g_wolfMoveBank0[mode].states[state];
    s32 rv = 0;
    Vec3s v;

    if (msgId < MSG_WOLF_CAUGHT) {
        switch (msgId) {
            case MSG_KILL: /* arg = WolfKillType: ignored while dead or in a state with flag 0x2000000 (unless it
                   * also has 0x80000000), and while attached to something in a costume */
                if (((!(flags & WOLF_FB_DEAD) && !(desc->flags & WSF_UNTOUCHABLE)) ||
                     (desc->flags & WSF_KILL_WHEN_DEAD)) &&
                    (modelSet == WMS_RALPH || !InstFlags(INST_F_ATTACHED))) {
                    Pad *pad;
                    if (playerIndex == 1)
                        pad = &g_pad2;
                    else
                        pad = &g_pad;
                    /* cast kept: MSG_KILL's arg is the WolfKillType, a number in the void * */
                    switch ((s32)arg) {
                        case KILL_FALL_PIT:
                            Die(WOLF_ST_DIE_FALL_PIT, WOLF_DROP_STOP, 0x2000);
                            break;
                        case KILL_GENERIC:
                            if (state != WOLF_ST_DIE_CANNONBALL) {
                                if (desc->flags & WSF_IN_WATER)
                                    Die(WOLF_ST_DIE_FROZEN, WOLF_DROP_RELEASE | WOLF_DROP_STOP, 0x4000);
                                else {
                                    fallLimitY = 32000;
                                    if (mode == WOLF_MODE_CARRY)
                                        Die(WOLF_ST_DIE_KNOCKED, WOLF_DROP_STOP, 0x2000);
                                    else
                                        Die(WOLF_ST_DIE_KNOCKED, WOLF_DROP_RELEASE | WOLF_DROP_STOP, 0x2000);
                                }
                                fxFlags |= WOLF_FA_BURNT;
                                pad->Rumble_stub(1500, g_rumbleSeqKill, 0x1000);
                            }
                            break;
                        case KILL_CRUSH:
                            Die(WOLF_ST_DIE_CRUSHED, WOLF_DROP_STOP, 0x2000);
                            killer = sender;
                            pad->Rumble_stub(1500, g_rumbleSeqImpact, 0x1000);
                            break;
                        case KILL_RESTART_ONLY:
                            camOverride = CAM_REQ_DIR_HOLD;
                            RestartNoAnim(0x3000);
                            break;
                        case KILL_FALL_TO_SENDER:
                            fallLimitY = sender->pos.y + 50;
                            Die(WOLF_ST_DIE_FALL_TO_SENDER, WOLF_DROP_RELEASE | WOLF_DROP_STOP, 0x2000);
                            break;
                        case KILL_SHARK:
                            Die(WOLF_ST_DIE_SHARK, WOLF_DROP_RELEASE | WOLF_DROP_STOP, 0x800);
                            rot = sender->rot;
                            SetPosition(&sender->pos);
                            break;
                        case KILL_TYPE6:
                            Die(WOLF_ST_WADE, WOLF_DROP_RELEASE | WOLF_DROP_STOP, 0x1000);
                            break;
                        case KILL_CANNONBALL:
                            fallLimitY = 32000;
                            Die(WOLF_ST_DIE_CANNONBALL, WOLF_DROP_RELEASE | WOLF_DROP_STOP, 0x1000);
                            break;
                        case KILL_ZAP:
                            fallLimitY = 32000;
                            if (sender->GetClassId() != CLASSID_REMOTECONTROL) {
                                Die(WOLF_ST_DIE_ZAPPED, WOLF_DROP_RELEASE | WOLF_DROP_STOP, 0x2000);
                                Fx8_Start();
                                fxFlags |= WOLF_FA_BURNT;
                            } else {
                                Die(WOLF_ST_DIE_ZAPPED_REMOTE, WOLF_DROP_STOP, 0x2000);
                                Sound_Play(SND_SRBELECT, this, 0xff, SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 0x1000);
                            }
                            pad->Rumble_stub(1500, g_rumbleSeqKill, 0x1000);
                            break;
                        case KILL_PIRANHAS:
                            Die(WOLF_ST_DIE_PIRANHAS, WOLF_DROP_RELEASE | WOLF_DROP_STOP, 0x400);
                            break;
                        case KILL_CROCODILE:
                            Die(WOLF_ST_DIE_CROCODILE, WOLF_DROP_RELEASE | WOLF_DROP_STOP, 0x800);
                            rot = sender->rot;
                            SetPosition(&sender->pos);
                            break;
                        case KILL_RAFT:
                            Die(WOLF_ST_DIE_RAFT, WOLF_DROP_RELEASE | WOLF_DROP_STOP, 0);
                            ClearFxFlags(WOLF_FA_BURNT);
                            rot = sender->rot;
                            SetPosition(&sender->pos);
                            break;
                        case KILL_PIRATE:
                            SetFacing(HeadingTo(&sender->pos));
                            fallLimitY = 32000;
                            Die(WOLF_ST_DIE_PIRATE, WOLF_DROP_RELEASE | WOLF_DROP_STOP, 0x1000);
                            break;
                        case KILL_MARTIAN:
                            Die(WOLF_ST_DIE_MARTIAN, WOLF_DROP_RELEASE | WOLF_DROP_STOP, 0x5800);
                            PlayAnim(g_wolfMoveBank0[mode].states[state].anim, 1, 0);
                            break;
                        case KILL_BURN_RUN: /* not lethal: the burnt run (state 0xcf) */
                            if (state != WOLF_ST_PANIC_RUN_BURNT) {
                                DropHeld(WOLF_DROP_RELEASE | WOLF_DROP_STOP);
                                SetMode(WOLF_MODE_NORMAL, WOLF_ST_PANIC_RUN_BURNT);
                            } else
                                SetStateKeepAnimStopSound(WOLF_ST_PANIC_RUN_BURNT);
                            Sound_Play(SND_WOLF_HURT, this, 0xff, SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 0x1000);
                            break;
                        case KILL_BLACKHOLE:
                            Die(WOLF_ST_DIE_BLACKHOLE, WOLF_DROP_RELEASE | WOLF_DROP_STOP, 0x2000);
                            killer = sender;
                            break;
                    }
                }
                rv = 1;
                break;
            case MSG_USE: /* from a sheep while Ralph is in a state with flag 0x10000: after 0x14000 ticks of it, state 0xa */
                if (sender->GetClassId() == CLASSID_SHEEP && (desc->flags & WSF_SHEEP_NIBBLE) &&
                    !(flags & WOLF_FB_RECENT_OBJECT_CONTACT)) {
                    bushNibbleTimer += g_dt;
                    if (bushNibbleTimer >= 0x14000) {
                        DropHeld(0);
                        SetState(WOLF_ST_SNEAK_IDLE);
                        bushNibbleTimer = 0;
                        if (fxFlags & WOLF_FA_PROP5)
                            Fx5_Stop();
                        Fx5_Start(1);
                    } else if (!(fxFlags & WOLF_FA_PROP5))
                        Fx5_Start(0);
                    rv = 1;
                }
                break;
            case MSG_QUERY_ACTION: /* a sheep asks which reaction to play */
                if (sender->GetClassId() == CLASSID_SHEEP) {
                    if ((desc->flags & WSF_SHEEP_NIBBLE) && !(flags & WOLF_FB_RECENT_OBJECT_CONTACT) &&
                        stateTime > 0x3000)
                        rv = SHEEP_ATTR_BUSH;
                    else if (modelSet == WMS_SHEEPCOSTUME)
                        rv = SHEEP_ATTR_LEADER;
                }
                break;
            case MSG_PICKUP: /* attach to a joint (arg) of the sender: Sam (1), InstantMartian (0x91), Crane (0x52) */
                if (sender->GetClassId() == CLASSID_SAM || sender->GetClassId() == CLASSID_INSTANTMARTIAN ||
                    sender->GetClassId() == CLASSID_CRANE) {
                    ScnObject *host;
                    u8 joint;
                    host = sender;
                    joint = (u8)arg; /* cast kept: MSG_PICKUP's arg is the joint number, a number in the void * */
                    AttachTo(host, joint, 0, 0, 0, 0);
                    if (desc->flags & WSF_ICE_BLOCK) {
                        SetState(WOLF_ST_FROZEN_GRABBED);
                        rot.y = 0x900;
                    } else if (modelSet == WMS_SHEEPCOSTUME) {
                        SetState(WOLF_ST_SHEEPCOSTUME_GRABBED);
                        Scenaric_SendToClass(CLASSID_SHEEP, MSG_SHEEP_CALL, 0);
                    }
                    rv = 1;
                }
                break;
            case MSG_DROP: /* detach; put down at arg->pos unless arg->flag1 */
                if (InstFlags(INST_F_ATTACHED)) {
                    DropMsgArg *drop;
                    drop = (DropMsgArg *)arg; /* cast kept: MSG_DROP's arg is a DropMsgArg */
                    Detach();
                    if (!drop->flag1)
                        SetPosition(&drop->pos);
                    if (desc->flags & WSF_ICE_BLOCK)
                        SetState(WOLF_ST_FROZEN_FALL);
                    else if (modelSet == WMS_SHEEPCOSTUME) {
                        rot = sender->rot;
                        SetState(WOLF_ST_SHEEPCOSTUME_IDLE);
                    }
                }
                rv = 1;
                break;
            case MSG_LAUNCH: /* launch along the trajectory in arg */
                /* cast kept: MSG_LAUNCH's arg is a WolfLaunchPath */
                launchPath = *(WolfLaunchPath *)arg;
                DropHeld(0);
                SetState(WOLF_ST_TRAJECTORY);
                rv = 1;
                break;
            case MSG_SEESAW_TOUCH:
                flags |= WOLF_FB_SEESAW_CAM;
                break;
            case MSG_FREEZE: /* freeze (held by the sender); refused in states with flag 0x200 */
                if (!(desc->flags & WSF_IGNORE_INPUT_FREEZE)) {
                    if ((flags & WOLF_FB_FROZEN) && grabber && grabber->HandleMessage(this, MSG_FREEZE, 0))
                        Unfreeze();
                    if (!(flags & WOLF_FB_FROZEN)) {
                        SetFrozenBy(sender);
                        if (speed >= g_wolfMoveBank0[WOLF_MODE_NORMAL].profiles[WOLF_SURFACE_NORMAL]->maxSpeed) {
                            speed = g_wolfMoveBank0[WOLF_MODE_NORMAL].profiles[WOLF_SURFACE_NORMAL]->maxSpeed - 1;
                            ClearFlags(WOLF_FB_RUN);
                        }
                        rv = 1;
                    }
                }
                break;
            case MSG_UNFREEZE:
                Unfreeze();
                rv = 1;
                break;
            case MSG_CINE_PLACE: { /* scripted walk to arg->target; arg 0 stops and reports ready (flag 0x80) */
                /* cast kept: MSG_CINE_PLACE's arg is a WolfWalkToArg, or 0 */
                WolfWalkToArg *walk = (WolfWalkToArg *)arg;
                if (walk) {
                    scriptWalkTarget = *walk->target;
                    if (mode == WOLF_MODE_CARRY) {
                        DropHeld(WOLF_DROP_STOP);
                        if (heldObject->GetClassId() == CLASSID_SHEEP && walk->dropZone) {
                            fxFlags |= WOLF_FA_CINE_SHEEP_DROP;
                            carryDropPos.x = (walk->dropZone->min[0] + walk->dropZone->max[0]) >> 1;
                            carryDropPos.y = (walk->dropZone->min[1] + walk->dropZone->max[1]) >> 1;
                            carryDropPos.z = (walk->dropZone->min[2] + walk->dropZone->max[2]) >> 1;
                        } else
                            ClearFxFlags(WOLF_FA_CINE_SHEEP_DROP);
                    } else
                        DropHeld(WOLF_DROP_RELEASE | WOLF_DROP_STOP);
                    if (walk->rot)
                        scriptWalkHeading = walk->rot->y;
                    else
                        scriptWalkHeading = 0;
                    SetState(WOLF_ST_SCRIPT_WALK);
                    ClearFlags(WOLF_FB_CINE_READY);
                } else {
                    DropHeld(WOLF_DROP_RELEASE | WOLF_DROP_STOP);
                    SetMode(WOLF_MODE_NORMAL, WOLF_ST_IDLE);
                    flags |= WOLF_FB_CINE_READY;
                    ClearEffects();
                }
                rv = 1;
                break;
            }
            case MSG_CINE_END: /* end of a cinematic */
                if (desc->flags & WSF_ICE_BLOCK)
                    SetState(WOLF_ST_FROZEN_FALL);
                else {
                    DropHeld(WOLF_DROP_RELEASE | WOLF_DROP_STOP);
                    ResetState();
                    SetMode(WOLF_MODE_NORMAL, WOLF_ST_IDLE);
                    SnapToGround(1);
                }
                rv = 1;
                break;
            case MSG_RIDER_ADD:
                rv = AddMoveModifier(sender);
                break;
            case MSG_RIDER_REMOVE:
                RemoveMoveModifier(sender);
                rv = 1;
                break;
            case MSG_QUERY_MOVED: /* moved this frame? */
                if (flags & WOLF_FB_MOVED)
                    rv = 1;
                break;
            case MSG_SEESAW_DRIFT:
                flags |= WOLF_FB_FORCE_SLIDE;
                rv = 1;
                break;
            case MSG_SET_ACTION_HEADING: { /* the heading along the segment from -> to (elastic, fishing rod), on Ralph's side of it */
                /* cast kept: MSG_SET_ACTION_HEADING's arg is a WolfLineArg */
                WolfLineArg *line = (WolfLineArg *)arg;
                Vec3s offset, dir;
                dir.x = -line->to->z + line->from->z;
                dir.z = line->to->x - line->from->x;
                dir.y = 0;
                offset.x = pos.x - line->from->x;
                offset.z = pos.z - line->from->z;
                offset.y = 0;
                lineHeading = Math_RadiansToAngle4096((float)atan2(-dir.x, -dir.z)) & 0xfff;
                if (offset.x * dir.x + offset.y * dir.y + offset.z * dir.z >= 0)
                    lineHeading = (lineHeading + 0x800) & 0xfff;
                if (line->reverse)
                    lineHeading = (lineHeading + 0x800) & 0xfff;
                rv = 1;
                break;
            }
            case MSG_TRAP_STATE:
                if (!arg) {
                    ClearFlags(WOLF_FB_IN_WOLFTRAP);
                    ClearFlags(WOLF_FB_WOLFTRAP_REACT);
                }
                rv = 1;
                break;
            case MSG_STRUGGLE:
                flags |= WOLF_FB_WOLFTRAP_REACT;
                rv = 1;
                break;
            case MSG_TIMEMACHINE_ARRIVE: { /* time machine: arrive at arg, on the ground if it is within 50 */
                /* cast kept: MSG_TIMEMACHINE_ARRIVE's arg is the Vec3s to arrive at */
                Vec3s *to = (Vec3s *)arg;
                SetPosition(to);
                if (TestBodyAt(&pos, CQ_OBJECTS))
                    SnapToGround(1);
                else {
                    v.x = pos.x;
                    v.y = pos.y - 50;
                    v.z = pos.z;
                    v.y = QueryGroundY(&v, 0);
                    if (SDW_ABS(v.y - pos.y) <= 50)
                        SetPosition(&v);
                }
                DropHeld(WOLF_DROP_RELEASE | WOLF_DROP_STOP);
                SetMode(WOLF_MODE_NORMAL, WOLF_ST_TIMEMACHINE_ARRIVE);
                rv = 1;
                break;
            }
            case MSG_TIMEMACHINE_LOCK: /* time machine: leave */
                if (TimeMachine_IsInPresent(this)) {
                    DropHeld(WOLF_DROP_STOP);
                    SetStateAnim(WOLF_ST_TIMEMACHINE_DEPART, ACOYOT01_ANIM_CHRONO2, 0, 1);
                } else {
                    SetFacing(HeadingTo(&sender->pos));
                    DropHeld(WOLF_DROP_RELEASE | WOLF_DROP_STOP);
                    SetStateAnim(WOLF_ST_TIMEMACHINE_DEPART, ACOYOT01_ANIM_CHRONO4, 0, 1);
                }
                rv = 1;
                break;
            case MSG_GEYSER_IN: /* returns the animation's length in ms */
                if (desc->flags & WSF_ICE_BLOCK) {
                    DropHeld(WOLF_DROP_RELEASE | WOLF_DROP_STOP);
                    SetMode(WOLF_MODE_NORMAL, WOLF_ST_FROZEN_GEYSER);
                    if (fxFlags & WOLF_FA_PROP_ICE)
                        propBody.PlayAnim(AGLACON1_ANIM_WHIRL1, 1, 1);
                    rv = 2000;
                } else {
                    if (!heldObject || heldObject->GetClassId() != CLASSID_SHEEP ||
                        heldObject->HandleMessage(this, MSG_SHEEP_MARK_IN_GEYSER, 0)) {
                        DropHeld(WOLF_DROP_STOP);
                        SetState(WOLF_ST_GEYSER_SUCKED);
                    } else {
                        DropHeld(WOLF_DROP_RELEASE | WOLF_DROP_STOP);
                        SetMode(WOLF_MODE_NORMAL, WOLF_ST_GEYSER_SUCKED);
                    }
                    rv = Anim_GetDurationMs(Inst(), AnimId(), 1);
                }
                break;
            case MSG_GEYSER_OUT: /* returns the animation's length in ms */
                if (arg) {
                    DropHeld(WOLF_DROP_STOP);
                    SetState(WOLF_ST_GEYSER_EJECT);
                    if (sender->rot.x != 0 || sender->rot.z != 0)
                        rot = sender->rot;
                    rv = Anim_GetDurationMs(Inst(), AnimId(), 1);
                }
                break;
            case MSG_DRAGON_BURN: /* the dragon's fire */
                if (!arg && state != WOLF_ST_BURNT && !(desc->flags & (WSF_UNTOUCHABLE | WSF_GHOST_CAPTURED))) {
                    DropHeld(WOLF_DROP_RELEASE);
                    SetMode(WOLF_MODE_NORMAL, WOLF_ST_BURNT);
                }
                rv = 1;
                break;
            case MSG_AXIS_LOCK: /* lock the stick heading to one axis this frame */
                if (!arg)
                    flags |= WOLF_FB_AXIS_LOCK_A;
                else if ((s32)arg ==
                         RC_AXIS_Z) /* cast kept: MSG_AXIS_LOCK's arg is the RollingCarpetAxis in the void * */
                    flags |= WOLF_FB_AXIS_LOCK_B;
                rv = 1;
                break;
            case MSG_BEES_STING:
                if (state != WOLF_ST_PANIC_RUN_BEES && !(desc->flags & (WSF_UNTOUCHABLE | WSF_GHOST_CAPTURED)) &&
                    !(flags & WOLF_FB_DEAD)) {
                    DropHeld(WOLF_DROP_RELEASE | WOLF_DROP_STOP);
                    SetMode(WOLF_MODE_NORMAL, WOLF_ST_PANIC_RUN_BEES);
                    Sound_Play(SND_WOLF_HURT, this, 0xff, SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 0x1000);
                    rv = 1;
                }
                break;
            case MSG_BOUNCE:
                if (!(flags & WOLF_FB_DEAD) && !(desc->flags & (WSF_UNTOUCHABLE | WSF_GHOST_CAPTURED))) {
                    DropHeld(0);
                    SetState(WOLF_ST_BOUNCED_UP);
                    rv = 1;
                }
                break;
            case MSG_QUERY_CONTROLLED: /* not frozen? */
                rv = !(flags & WOLF_FB_FROZEN);
                break;
            case MSG_MAGNET_PULL: { /* MSG_MAGNET_PULL: arg {caught object, reply} */
                /* cast kept: MSG_MAGNET_PULL's arg is {caught object, reply} */
                ScnObject **pull = (ScnObject **)arg;
                magnetTarget = *pull;
                if (!(desc->flags &
                      (WSF_IN_WATER | WSF_AIRBORNE | WSF_UNTOUCHABLE | WSF_MAGNET_DRAG | WSF_GHOST_CAPTURED)) &&
                    sender == heldObject) {
                    DropHeld(0);
                    SetState(WOLF_ST_MAGNET_DRAG_IDLE);
                }
                rv = 1;
                break;
            }
            case MSG_WOLF_HOOVER_HOLD:
                if (arg) {
                    if (mode == WOLF_MODE_NORMAL) {
                        DropHeld(0);
                        SetState(WOLF_ST_HOOVER_HOLD);
                    }
                } else if (state == WOLF_ST_HOOVER_HOLD) {
                    DropHeld(0);
                    SetIdleState();
                }
                rv = 1;
                break;
            case MSG_WOLF_DIE_HOOVERED:
                if (state != WOLF_ST_DIE_HOOVERED && !(flags & WOLF_FB_DEAD)) {
                    Die(WOLF_ST_DIE_HOOVERED, WOLF_DROP_STOP, 0x1000);
                    rv = 1;
                }
                break;
            case MSG_SET_SIZE: /* shrink (arg != 0) or restore the size; 1 if it changed */
                DropHeld(WOLF_DROP_RELEASE | WOLF_DROP_STOP);
                SetMode(WOLF_MODE_NORMAL, WOLF_ST_IDLE);
                if (arg) {
                    if (scale != 0x155) {
                        SetScale(0x155);
                        rv = 1;
                    }
                } else if (scale != 0x400) {
                    SetScale(0x400);
                    rv = 1;
                }
                break;
            case MSG_QUERY_SIZE: /* the size: 0 normal, 1 small, 2 big */
                if (scale == 0x400)
                    rv = WOLF_SIZE_NORMAL;
                else if (scale < 0x400)
                    rv = WOLF_SIZE_SMALL;
                else
                    rv = WOLF_SIZE_BIG;
                break;
            case MSG_SCARE: /* scared away from the sender */
                if (!(flags & WOLF_FB_DEAD) && !(desc->flags & (WSF_UNTOUCHABLE | WSF_GHOST_CAPTURED))) {
                    interactHeading = HeadingTo(&sender->pos);
                    fallLimitY = 32000;
                    DropHeld(WOLF_DROP_RELEASE | WOLF_DROP_STOP);
                    if (modelSet == WMS_GHOSTCOSTUME)
                        SetMode(WOLF_MODE_NORMAL, WOLF_ST_GHOSTCOSTUME_CAPTURED);
                    else
                        SetMode(WOLF_MODE_NORMAL, WOLF_ST_GHOST_CAPTURED);
                    rv = 1;
                }
                break;
            case MSG_SET_ANCHOR: { /* ghost costume: go to the spot in arg unless already within its radius */
                /* cast kept: MSG_SET_ANCHOR's arg is a WolfSpotArg */
                WolfSpotArg *spot = (WolfSpotArg *)arg;
                if (modelSet == WMS_GHOSTCOSTUME && state != WOLF_ST_DANCE_GOTO && !IsFalling() &&
                    Vec3s_DistSqXZ(&spot->pos, &pos) >= spot->radius * spot->radius) {
                    interactPos = spot->pos;
                    SetState(WOLF_ST_DANCE_GOTO);
                }
                break;
            }
            case MSG_MIRROR_RENDER:
                Msg74_Draw((DrawMsgArgs *)arg); /* cast kept: MSG_MIRROR_RENDER's arg is a DrawMsgArgs */
                rv = 1;
                break;
        }
    } else {
        switch (msgId) {
            case MSG_WOLF_CAUGHT: /* caught: arg 0 grabbed, 2 hit, 1 killed, 3 released */
                if (!(desc->flags & WSF_UNTOUCHABLE) && !g_cinePlayer.IsActive()) {
                    /* cast kept: MSG_WOLF_CAUGHT's arg is the WolfCaughtArg, a number in the void * */
                    switch ((s32)arg) {
                        case CAUGHT_GRAB:
                        case CAUGHT_HIT:
                            if (!(flags & WOLF_FB_DEAD)) {
                                if (modelSet == WMS_SHEEPCOSTUME) {
                                    fallLimitY = 32000;
                                    Die(WOLF_ST_SHEEPCOSTUME_DIE_THROWN, WOLF_DROP_STOP, 0x2000);
                                } else {
                                    v.y = HeadingTo(&sender->pos);
                                    v.x = v.z = 0;
                                    rot = v;
                                    SetPosition(&sender->pos);
                                    if (!arg) {
                                        NotifyAttached_6A();
                                        flags |= WOLF_FB_DEAD;
                                        DropHeld(WOLF_DROP_RELEASE | WOLF_DROP_STOP);
                                        SetMode(WOLF_MODE_NORMAL, WOLF_ST_CAUGHT_BY_SAM);
                                    } else
                                        Die(WOLF_ST_DIE_SAM_PUNCH_WINDUP, WOLF_DROP_RELEASE | WOLF_DROP_STOP, 0x2000);
                                }
                            }
                            break;
                        case CAUGHT_END:
                            Die(WOLF_ST_DIE_SAM_PUNCH, WOLF_DROP_RELEASE | WOLF_DROP_STOP, 0x2000);
                            break;
                        case CAUGHT_RELEASE:
                            ClearFlags(WOLF_FB_DEAD);
                            DropHeld(WOLF_DROP_RELEASE | WOLF_DROP_STOP);
                            SetMode(WOLF_MODE_NORMAL, WOLF_ST_IDLE);
                            break;
                    }
                    rv = 1;
                }
                break;
            case MSG_WOLF_WIN: /* level won */
                if (!(flags & (WOLF_FB_DEAD | WOLF_FB_WON))) {
                    flags |= WOLF_FB_WON;
                    camOverride = CAM_REQ_DIR_HOLD;
                    DropHeld(WOLF_DROP_STOP);
                    if (mode == WOLF_MODE_CARRY)
                        SetState(WOLF_ST_PUTDOWN_M0);
                    else
                        SetMode(WOLF_MODE_NORMAL, WOLF_ST_IDLE);
                    Fade_StartLevelExit(0x3000);
                }
                rv = 1;
                break;
            case MSG_WOLF_SAVE_RESPAWN: { /* checkpoint: save the respawn point */
                /* cast kept: MSG_WOLF_SAVE_RESPAWN's arg is a WolfRespawnArg */
                WolfRespawnArg *spawn = (WolfRespawnArg *)arg;
                savedRespawnPos = spawn->pos;
                savedRespawnFacing = spawn->facing;
                Inventory_CommitAtCheckpoint();
                rv = 1;
                break;
            }
            case MSG_WOLF_IS_NOISY: /* quiet? */
                rv = !(desc->flags & WSF_BUSY_OR_QUIET) && !(desc->flags & WSF_STATIONARY);
                break;
            case MSG_WOLF_IS_VISIBLE: /* visible to the sender? */
                if (!(flags & WOLF_FB_IN_SHADOW_ZONE)) {
                    if (desc->flags & WSF_KEEP_COVER) {
                        if (modelSet == WMS_BUSH) {
                        } else if (coverObject)
                            rv = IsOnSameSideAs(sender, coverObject);
                    } else
                        rv = 1;
                }
                break;
            case MSG_WOLF_DAZE:
                if (!(flags & WOLF_FB_DEAD) && !(desc->flags & (WSF_UNTOUCHABLE | WSF_GHOST_CAPTURED))) {
                    DropHeld(WOLF_DROP_RELEASE);
                    SetMode(WOLF_MODE_NORMAL, WOLF_ST_DAZED);
                    rv = 1;
                }
                break;
            case MSG_WOLF_SET_CINE_READY:
                flags |= WOLF_FB_CINE_READY;
                ClearEffects();
                rv = 1;
                break;
            case MSG_WOLF_IS_CINE_READY:
                if (flags & WOLF_FB_CINE_READY)
                    rv = 1;
                break;
            case MSG_WOLF_TAKE_ITEM_CLASS: { /* take away the item of class arg, from the hand, the inventory or the level; 1 if found */
                ScnObject *item = 0;
                /* cast kept: MSG_WOLF_TAKE_ITEM_CLASS's arg is the class id, a number in the void * */
                if (heldObject && heldObject->GetClassId() == (u32)arg) {
                    item = heldObject;
                    DropHeld(WOLF_DROP_RELEASE);
                    SetMode(WOLF_MODE_NORMAL, WOLF_ST_IDLE);
                    Inventory_Remove(item);
                } else {
                    Inventory_SelectClass((u16)arg); /* cast kept: the class id is a number in the void * */
                    item = Inventory_GetSelectedObject();
                    if (item)
                        Inventory_Remove(item);
                    else {
                        /* cast kept: the class id is a number in the void * */
                        Scenaric_FindByClass((u16)arg, &item, 1);
                        if (item) {
                            if (item->IsInWorld())
                                item->RemoveFromWorld();
                            else
                                item = 0;
                        }
                    }
                }
                rv = item != 0;
                break;
            }
            case MSG_WOLF_IS_IN_BUSH:
                rv = modelSet == WMS_BUSH;
                break;
            case MSG_WOLF_ALLOW_LOOK:
                if (arg)
                    flags |= WOLF_FB_LOOK_ALLOWED;
                else
                    ClearFlags(WOLF_FB_LOOK_ALLOWED);
                rv = 1;
                break;
            case MSG_WOLF_IS_SHEEPCOSTUME:
                rv = modelSet == WMS_SHEEPCOSTUME;
                break;
            case MSG_WOLF_IS_RUNNING:
                if (desc->flags & WSF_RUN_COLLISION)
                    rv = 1;
                break;
            case MSG_WOLF_IS_DEAD:
                if (flags & WOLF_FB_DEAD)
                    rv = 1;
                break;
            case MSG_WOLF_IS_GROUNDED:
                rv = (desc->flags & WSF_GROUND_STATE) && !(flags & WOLF_FB_RECENT_OBJECT_CONTACT);
                break;
            case MSG_WOLF_IS_GROUNDED_OR_CONTACT:
                rv = (desc->flags & WSF_GROUND_STATE) || (flags & WOLF_FB_RECENT_OBJECT_CONTACT);
                break;
            case MSG_WOLF_GOSSAMER_SCRIPT:
                /* cast kept: MSG_WOLF_GOSSAMER_SCRIPT's arg is the WolfGossamerScript, a number in the void * */
                switch ((s32)arg) {
                    case GOSSAMER_SCRIPT_GRAB_0:
                        DropHeld(WOLF_DROP_RELEASE | WOLF_DROP_STOP);
                        v.y = HeadingTo(&sender->pos);
                        v.x = v.z = 0;
                        rot = v;
                        SetMode(WOLF_MODE_NORMAL, WOLF_ST_GOSSAMER_GRAB_0);
                        break;
                    case GOSSAMER_SCRIPT_GRAB_1:
                        SetState(WOLF_ST_GOSSAMER_GRAB_1);
                        break;
                    case GOSSAMER_SCRIPT_GRAB_2:
                        SetState(WOLF_ST_GOSSAMER_GRAB_2);
                        break;
                    case GOSSAMER_SCRIPT_RELEASED:
                        SetState(WOLF_ST_GOSSAMER_RELEASED);
                        break;
                    case GOSSAMER_SCRIPT_IDLE:
                        SetIdleState();
                        break;
                }
                rv = 1;
                break;
            case MSG_WOLF_IS_RABBITCOSTUME:
                rv = modelSet == WMS_RABBITCOSTUME;
                break;
            case MSG_WOLF_SET_INVISIBLE:
                if (arg)
                    ClearFlags(WOLF_FB_INVISIBLE);
                else
                    flags |= WOLF_FB_INVISIBLE;
                rv = 1;
                break;
            case MSG_WOLF_IS_GHOSTCOSTUME:
                rv = modelSet == WMS_GHOSTCOSTUME;
                break;
            case MSG_WOLF_IS_CARRYING_SHEEP_M1:
                rv = mode == WOLF_MODE_CARRY && heldObject->GetClassId() == CLASSID_SHEEP;
                break;
            case MSG_WOLF_DISABLE_PROMPT:
                if (arg)
                    ClearFlags(WOLF_FB_PROMPT_DISABLED);
                else
                    flags |= WOLF_FB_PROMPT_DISABLED;
                rv = 1;
                break;
            case MSG_WOLF_STRIP:
                if (heldObject || modelSet) {
                    costume = WMS_RALPH;
                    DropHeld(WOLF_DROP_RELEASE);
                    SetMode(WOLF_MODE_NORMAL, WOLF_ST_IDLE);
                }
                rv = 1;
                break;
            case MSG_WOLF_FORCE_STOW:
                if (heldObject || modelSet) {
                    /* cast kept (these three lines): MSG_WOLF_FORCE_STOW's arg is the Vec3s drop offset */
                    dropOffset.x = ((Vec3s *)arg)->x;
                    dropOffset.y = ((Vec3s *)arg)->y;
                    dropOffset.z = ((Vec3s *)arg)->z;
                    costume = WMS_RALPH;
                    DropHeld(WOLF_DROP_RELEASE | WOLF_DROP_USE_OFFSET);
                    SetMode(WOLF_MODE_NORMAL, WOLF_ST_IDLE);
                }
                rv = 1;
                break;
            case MSG_WOLF_GHOST_CARRY:
                if (modelSet == WMS_GHOSTCOSTUME) {
                    if (arg)
                        SetState(WOLF_ST_GHOSTCOSTUME_LIFTED);
                    else
                        SetState(WOLF_ST_GHOSTCOSTUME_FALL);
                } else if (mode == WOLF_MODE_NORMAL) {
                    if (arg)
                        SetState(WOLF_ST_GHOST_LIFTED);
                    else
                        SetState(WOLF_ST_FALL);
                }
                rv = 1;
                break;
            case MSG_WOLF_HIT_BY_BIPBIP:
                if (state != WOLF_ST_HIT_BY_BIPBIP && !(flags & WOLF_FB_DEAD) &&
                    !(desc->flags & (WSF_UNTOUCHABLE | WSF_GHOST_CAPTURED))) {
                    DropHeld(WOLF_DROP_RELEASE);
                    SetMode(WOLF_MODE_NORMAL, WOLF_ST_HIT_BY_BIPBIP);
                }
                rv = 1;
                break;
            case MSG_WOLF_FORCE_FALL:
                flags |= WOLF_FB_FORCE_FALL;
                rv = 1;
                break;
            case MSG_WOLF_IS_CATCHABLE: /* catchable? */
                if (!(desc->flags & WSF_UNTOUCHABLE) && !g_cinePlayer.IsActive())
                    rv = 1;
                break;
            case MSG_WOLF_GET_DISTANCE:
                rv = distanceTravelled;
                break;
            case MSG_WOLF_SQUASH: /* flattened for arg ticks */
                if (!(flags & WOLF_FB_DEAD) &&
                    !(desc->flags & (WSF_UNTOUCHABLE | WSF_FLATTENED | WSF_GHOST_CAPTURED))) {
                    if (arg)
                        /* cast kept: MSG_WOLF_SQUASH's arg is the duration, a number in the void * */
                        flattenDuration = (s32)arg;
                    DropHeld(WOLF_DROP_RELEASE | WOLF_DROP_STOP);
                    SetMode(WOLF_MODE_NORMAL, WOLF_ST_FLATTENED_START);
                    rv = 1;
                }
                break;
            case MSG_WOLF_IS_SQUASHED:
                if (desc->flags & WSF_FLATTENED)
                    rv = 1;
                break;
            case MSG_WOLF_SQUASH_LEAP:
                if ((desc->flags & WSF_FLATTENED) && state != WOLF_ST_FLATTENED_KNOCKBACK) {
                    SetFacing((HeadingTo(&sender->pos) + 0x800) & 0xfff);
                    SetState(WOLF_ST_FLATTENED_KNOCKBACK);
                    rv = 1;
                }
                break;
            case MSG_WOLF_IS_FLYING:
                if (desc->flags & WSF_FLYING)
                    rv = 1;
                break;
            case MSG_WOLF_IS_IN_WATER_STATE:
                if (desc->flags & WSF_IN_WATER)
                    rv = 1;
                break;
            case MSG_WOLF_IS_SCARED:
                if (desc->flags & WSF_GHOST_CAPTURED)
                    rv = 1;
                break;
            case MSG_WOLF_THROW:
                if (!(flags & WOLF_FB_DEAD) && !(desc->flags & (WSF_UNTOUCHABLE | WSF_GHOST_CAPTURED))) {
                    DropHeld(WOLF_DROP_STOP);
                    SetState(WOLF_ST_KNOCKBACK);
                    rot.y = (s16)arg; /* cast kept: MSG_WOLF_THROW's arg is the heading, a number in the void * */
                    Sound_Play(SND_WOLF_HURT, this, 0xff, SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 0x1000);
                    rv = 1;
                }
                break;
            case MSG_WOLF_SET5_TURN:
                if (arg) {
                    /* cast kept: MSG_WOLF_SET5_TURN's arg is the heading, a number in the void * */
                    interactHeading = (s16)((s32)arg & 0xffff);
                    danceMove = -1;
                    SetState(WOLF_ST_DANCE);
                } else if (state == WOLF_ST_DANCE)
                    GhostCostumeSetIdle();
                rv = 1;
                break;
            case MSG_WOLF_IS_USING_SPECIAL_OBJECT:
                if (flags & WOLF_FB_SPECIAL_OBJECT)
                    rv = 1;
                break;
            case MSG_WOLF_GLOW_DIST: /* the nearest light this frame */
                /* cast kept (these two lines): MSG_WOLF_GLOW_DIST's arg is the distance, a number in the void * */
                if ((s32)arg < glowDist)
                    glowDist = (s32)arg;
                rv = 1;
                break;
            case MSG_WOLF_NO_LOOK:
                flags |= WOLF_FB_NO_LOOK;
                rv = 1;
                break;
            case MSG_WOLF_SET_OBJFLAG2:
                if (arg)
                    ScnObject::flags &=
                        (u16)~SCN_OF_MUTE_ANIM_SOUND; /* the 16-bit mask as a constant: and ecx,0xfffd (0x4869d3) */
                else
                    ScnObject::flags |= SCN_OF_MUTE_ANIM_SOUND;
                rv = 1;
                break;
        }
    }
    return rv;
}

/* ---- drawing, dropping what is held, the state changes ---- */
/* 0x486c1b - the vtable +8 override. Ralph's body is skipped in the first-person look (camera mode 1 at distance <= 20,
 * outside the debug camera) and while flags & 0x100000; otherwise drawn scaled by scale / 0x400, about the pivot
 * (0, -90, 0) in states with flag 0x200000. Then the prop body (turned to face the camera with fxFlags 0x4000) and the
 * splash body at the same scale, and the three emitters. */
void Wolf::Render(Camera *view)
{
    Vec3s sc;
    Vec3s facing;
    WolfStateDesc *curDesc;
    Vec3s pivot;

    sc.x = sc.y = sc.z = scale;
    if ((g_camDebugMode || g_camMode != CAM_LOOK || g_camera.dist > 0x14) && !(flags & WOLF_FB_INVISIBLE)) {
        curDesc = &g_wolfMoveBank0[mode].states[state];
        pivot.x = 0;
        pivot.y = -90;
        pivot.z = 0;
        if (scale == 0x400) {
            if (curDesc->flags & WSF_WATER_COLLIDE)
                RenderPivoted(view, &pivot, 0);
            else
                ScnMobile::Render(view);
        } else {
            if (curDesc->flags & WSF_WATER_COLLIDE)
                RenderPivoted(view, &pivot, &sc);
            else
                RenderScaled(view, &sc);
        }
    }
    if (fxFlags & WOLF_FA_PROP6_BILLBOARD) {
        propBody.CalcFacingCameraRot(&facing, view, 0, 0);
        propBody.rot = facing;
    }
    if (scale == 0x400) {
        if (fxFlags & (WOLF_FA_PROP_ICE | WOLF_FA_PROP0 | WOLF_FA_PROP5 | WOLF_FA_PROP6_BILLBOARD |
                       WOLF_FA_DEATHZONE_PROP | WOLF_FA_PROP7 | WOLF_FA_PROP4 | WOLF_FA_PROP8))
            propBody.Render(view);
        if (fxFlags & WOLF_FA_SPLASH_ACTIVE)
            splashBody.Render(view);
    } else {
        if (fxFlags & (WOLF_FA_PROP_ICE | WOLF_FA_PROP0 | WOLF_FA_PROP5 | WOLF_FA_PROP6_BILLBOARD |
                       WOLF_FA_DEATHZONE_PROP | WOLF_FA_PROP7 | WOLF_FA_PROP4 | WOLF_FA_PROP8))
            propBody.RenderScaled(view, &sc);
        if (fxFlags & WOLF_FA_SPLASH_ACTIVE)
            splashBody.RenderScaled(view, &sc);
    }
    if (trailA.base.flags.active) {
        if (fxFlags & WOLF_FA_STATE_DUST)
            trailA.base.Emitter_Render(view, 3);
        else
            trailA.base.Emitter_Render(view, 0);
    }
    if (particles.flags.active)
        particles.RenderGround(view, 0);
    if (trailB.base.flags.active)
        trailB.RenderGround(view, 1);
}

/* 0x486f11 - let go of what Ralph holds. dropFlags & 1: on foot, the held item is put down where he stands (message 5,
 * then 7) and removed from the world; carrying (mode 1), the carried object gets message 5 with `placed` set at his
 * position - plus the drop offset with & 8; & 2 sets flag1 and keeps the height; otherwise flag1 is clear and the
 * height is the ground under the CARRIED object's own box minus 20 (20 higher again if its body would not fit there).
 * Then the active item goes back to the inventory unless the state keeps it (flag 0x20000). dropFlags & 4 also stops
 * his motion. The carried branch does not check heldObject for 0. */
/* BYTES(dead-code): result is stored and never read, as in the original */
void Wolf::DropHeld(u32 dropFlags)
{
    WolfStateDesc *stateDesc;
    DropMsgArg drop;
    s32 result;

    if (flags & WOLF_FB_HELD_IN_USE)
        HeldObj_SendMsg16();
    if (dropFlags & WOLF_DROP_RELEASE) {
        switch (mode) {
            case WOLF_MODE_NORMAL:
                if (heldObject) {
                    drop.pos = pos;
                    drop.placed = 0;
                    drop.flag1 = 0;
                    result = heldObject->HandleMessage(this, MSG_DROP, &drop);
                    heldObject->HandleMessage(this, MSG_INVENTORY_STORED, 0);
                    heldObject->RemoveFromWorld();
                    heldObject = 0;
                }
                break;
            case WOLF_MODE_CARRY:
                drop.pos = pos;
                drop.placed = 1;
                if (dropFlags & WOLF_DROP_USE_OFFSET) {
                    drop.pos.x += dropOffset.x;
                    drop.pos.y += dropOffset.y;
                    drop.pos.z += dropOffset.z;
                }
                if (dropFlags & WOLF_DROP_NO_SNAP)
                    drop.flag1 = 1;
                else if (!(dropFlags & WOLF_DROP_USE_OFFSET)) {
                    drop.flag1 = 0;
                    drop.pos.y = heldObject->QueryGroundY(&drop.pos, 1) - 20;
                    if (heldObject->TestBodyAt(&drop.pos, CQ_STATIC))
                        drop.pos.y += 20;
                }
                result = heldObject->HandleMessage(this, MSG_DROP, &drop);
                heldObject = 0;
                break;
        }
        if (flags & WOLF_FB_SPECIAL_OBJECT) {
            stateDesc = &g_wolfMoveBank0[mode].states[state];
            if (!(stateDesc->flags & WSF_KEEP_ELASTIC))
                StoreActiveItemInInventory();
        }
    }
    if (dropFlags & WOLF_DROP_STOP)
        StopMotion();
}

/* 0x48716f - the only writer of state: enter a state without touching the animation. Resets the state timers, lets go
 * of a grabber the state releases (flag 0x200), applies the descriptor's shadow (0x400 hides it) and box-collision
 * (0x800 skips it) bits, stops the dizzy effect, keeps the ice block only in 0x800000 states, drops the cover object
 * and the hiding flag, takes off the bush and the elastic, sends the held object its message for the state, and
 * measures the current animation's length in ticks. */
/* BYTES(dead-code): reply is stored and never read, as in the original */
void Wolf::EnterState(u8 newState)
{
    WolfStateDesc *descriptor;
    s32 reply;

    descriptor = &g_wolfMoveBank0[mode].states[newState];
    state = newState;
    stateTime = 0;
    footstepTimer = 0;
    stepSoundCount = 0;
    if ((descriptor->flags & WSF_IGNORE_INPUT_FREEZE) && (flags & WOLF_FB_FROZEN) && grabber) {
        reply = grabber->HandleMessage(this, MSG_FREEZE, 0);
        Unfreeze();
    }
    if (descriptor->flags & WSF_BODY_FLAG0)
        shadow.SetVisible(0);
    else
        shadow.SetVisible(1);
    if (descriptor->flags & WSF_SKIP_BOX_TEST)
        SetBoxCollide(0);
    else
        SetBoxCollide(1);
    if (fxFlags & WOLF_FA_PROP0)
        Fx0_Stop();
    if (descriptor->flags & WSF_ICE_BLOCK) {
        if (!(fxFlags & (WOLF_FA_PROP_ICE | WOLF_FA_PROP4)))
            Fx3or4_Start(0);
    } else if (fxFlags & (WOLF_FA_PROP_ICE | WOLF_FA_PROP4))
        Fx3or4_Stop();
    if (!(descriptor->flags & WSF_KEEP_COVER))
        coverObject = 0;
    if (fxFlags & WOLF_FA_PROP6_BILLBOARD)
        Fx6_Stop();
    if (flags & WOLF_FB_RIVER_REGISTERED) {
        if (!waterZone || !(descriptor->flags & WSF_ICE_BLOCK))
            ClearFlags(WOLF_FB_RIVER_REGISTERED);
    }
    if (bush && modelSet != WMS_BUSH) {
        bush->HandleMessage(this, MSG_BUSH_TAKEN_OFF, 0);
        bush = 0;
        if (!(fxFlags & WOLF_FA_PROP5) && !(flags & WOLF_FB_RESETTING))
            Fx5_Start(1);
    }
    if ((flags & WOLF_FB_ITEM_ACTION) && !(descriptor->flags & WSF_KEEP_ELASTIC)) {
        ClearFlags(WOLF_FB_ITEM_ACTION);
        elastic->HandleMessage(this, MSG_USE, 0);
        ClearActiveItem();
    }
    if (descriptor->flags & WSF_ITEM_IN_USE) {
        if (flags & WOLF_FB_HELD_IN_USE)
            /* cast kept: MSG_CARRY_ANIM's arg is the state's carry pose, a number in the void * */
            heldObject->HandleMessage(this, MSG_CARRY_ANIM, (void *)descriptor->heldMsgArg);
        else
            HeldObj_SendMsg15();
    } else if (flags & WOLF_FB_HELD_IN_USE)
        HeldObj_SendMsg16();
    else if (heldObject)
        /* cast kept: MSG_CARRY_ANIM's arg is the state's carry pose, a number in the void * */
        heldObject->HandleMessage(this, MSG_CARRY_ANIM, (void *)descriptor->heldMsgArg);
    animDurationTicks = (Anim_GetDurationMs(Inst(), AnimId(), 1) << 12) / 1000;
}

/* 0x4875b9 - enter a state with its animation: switch to the state's model set (0 = the costume's), dropping the
 * dizzy / ice-block effects and any parent attachment when the model changes; the rabbit costume (4) swaps the idle
 * animation 0x12 for 0xbd. The animation blends only when the model stayed the same. */
void Wolf::SetStateAnim(u8 newState, u16 animId, s32 loop, s32 stopSound)
{
    s32 doBlend;
    WolfStateDesc *descriptor;
    u8 set;

    descriptor = &g_wolfMoveBank0[mode].states[newState];
    doBlend = 1;
    set = descriptor->modelSet;
    if (set == 0)
        set = costume;
    if (set != modelSet) {
        if (fxFlags & WOLF_FA_PROP0)
            Fx0_Stop();
        if (fxFlags & (WOLF_FA_PROP_ICE | WOLF_FA_PROP4))
            Fx3or4_Stop();
        if (InstFlags(INST_F_ATTACHED))
            Detach();
        SwapModel(&modelSets[set]);
        modelSet = set;
        doBlend = 0;
    }
    if (costume == WMS_RABBITCOSTUME && animId == ACOYOT01_ANIM_RUN1)
        animId = ACOYOT01_ANIM_BWALK;
    PlayAnim(animId, loop, doBlend);
    if (stopSound)
        StopLoopSound();
    EnterState(newState);
}

/* 0x487716 - enter a state with its own animation (looped per descriptor flag 1), stopping the loop sound. */
void Wolf::SetState(u8 newState)
{
    WolfStateDesc *descriptor;

    descriptor = &g_wolfMoveBank0[mode].states[newState];
    SetStateAnim(newState, descriptor->anim, descriptor->flags & WSF_LOOP_ANIM, 1);
}

/* 0x487762 - enter a state keeping the animation, stopping the loop sound. */
void Wolf::SetStateKeepAnimStopSound(u8 newState)
{
    StopLoopSound();
    EnterState(newState);
}

/* 0x487783 - as SetState, but the loop sound carries on (the run states). */
void Wolf::SetStateKeepSound(u8 newState)
{
    WolfStateDesc *descriptor;

    descriptor = &g_wolfMoveBank0[mode].states[newState];
    SetStateAnim(newState, descriptor->anim, descriptor->flags & WSF_LOOP_ANIM, 0);
}

/* 0x4877cf - enter a state keeping the animation and the sound. */
void Wolf::SetStateKeepAnim(u8 newState)
{
    EnterState(newState);
}

/* 0x4877e8 - the only writer of mode (0 on foot, 1 carrying): resets the run tap timer, sets flag 4, flattens the
 * ground normal, stops, and enters newState (0 = the idle state for the mode). */
void Wolf::SetMode(u8 newMode, u8 newState)
{
    mode = newMode;
    runTapTimer = 0;
    flags |= WOLF_FB_RUN;
    groundNormal.x = 0;
    groundNormal.y = -0x1000;
    groundNormal.z = 0;
    StopMotion();
    if (newState == 0)
        SetIdleState();
    else
        SetState(newState);
}

/* ---- idle, reset and init ---- */
/* 0x48786d - enter the idle state 0 with its animation. On foot, empty-handed: the idle table's base animation (the
 * rabbit costume's table in costume 4) and a random loop count before the first variant. Holding something: animation
 * 0xF for the rocket, else 0x31 and a count of Rand_Bounded(5). Carrying (mode 1): the state's own animation. */
void Wolf::SetIdleState()
{
    const IdleAnimEntry *table;
    switch (mode) {
        case WOLF_MODE_NORMAL:
            if (!heldObject) {
                if (costume == WMS_RABBITCOSTUME)
                    table = g_wolfIdleAnimsCostume4;
                else
                    table = g_wolfIdleAnims;
                SetStateAnim(WOLF_ST_IDLE, table->animId, 1, 1);
                idleLoopCount = Rand_Range(table->loopsMin, table->loopsMax);
            } else {
                switch (IsRocket(heldObject)) {
                    case 1:
                        SetStateAnim(WOLF_ST_IDLE, ACOYOT01_ANIM_ROCKET1, 1, 1);
                        break;
                    default:
                        SetStateAnim(WOLF_ST_IDLE, ACOYOT01_ANIM_STAND10, 1, 1);
                        idleLoopCount = Rand_Bounded(5);
                }
            }
            break;
        case WOLF_MODE_CARRY:
            SetState(WOLF_ST_IDLE);
            idleLoopCount = Rand_Range(g_wolfIdleAnimsCarry[0].loopsMin, g_wolfIdleAnimsCarry[0].loopsMax);
    }
}

/* 0x48797d - the idle variation of state 0 (at the end of each idle loop). Empty-handed: the next idle animation from
 * the table; in the rabbit costume animation 0xBA is followed by 0xBB, which plays sound 0x4F. Holding something other
 * than the rocket: after idleLoopCount loops of animation 0x31, one fidget with the item (0x45 the flute, 0x47 the red
 * scarf, else 0x30) - not during a cinematic - then 0x31 again for Rand_Bounded(5) loops. */
void Wolf::IdleAnimStep()
{
    u16 anim;
    switch (mode) {
        case WOLF_MODE_NORMAL:
            if (!heldObject) {
                if (costume == WMS_RABBITCOSTUME) {
                    if (AnimId() == ACOYOT01_ANIM_STANDBU0 && idleLoopCount)
                        PlayAnim(ACOYOT01_ANIM_STANDBU1, 1, 1);
                    NextIdleAnim(&g_wolfIdleAnimsCostume4[0], &g_wolfIdleAnimsCostume4[1], 1);
                    if (AnimId() == ACOYOT01_ANIM_STANDBU1)
                        loopSfxHandle = Sound_Play(SND_WOLF_IDLE_BB, this, 0xff,
                                                   SNDF_LOOP | SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 0x1000);
                    else
                        StopLoopSound();
                } else
                    NextIdleAnim(&g_wolfIdleAnims[0], &g_wolfIdleAnims[1], 4);
            } else {
                switch (IsRocket(heldObject)) {
                    case 1:
                        break;
                    default:
                        if (!idleLoopCount) {
                            anim = AnimId();
                            if (anim == ACOYOT01_ANIM_STAND10 && !g_cinePlayer.IsActive()) {
                                switch (heldObject->GetClassId()) {
                                    case CLASSID_FLUTE:
                                        anim = ACOYOT01_ANIM_STANDF1;
                                        break;
                                    case CLASSID_REDSCARF:
                                        anim = ACOYOT01_ANIM_BUFIGHT1;
                                        break;
                                    default:
                                        anim = ACOYOT01_ANIM_STAND9;
                                }
                                idleLoopCount = 0;
                            } else {
                                anim = ACOYOT01_ANIM_STAND10;
                                idleLoopCount = Rand_Bounded(5);
                            }
                            PlayAnim(anim, 1, 1);
                        } else
                            idleLoopCount--;
                }
            }
            break;
        case WOLF_MODE_CARRY:
            NextIdleAnim(&g_wolfIdleAnimsCarry[0], &g_wolfIdleAnimsCarry[1], 1);
    }
}

/* 0x487bdf - the state reset of level start and respawn: state 0 on normal ground, out of water, no cover / killer /
 * elastic / magnet / context action / updraft, most Wolf flags cleared (0x80808ac6 kept) and 0x80 set, the contact
 * times stamped now, the timers and scripted targets back to their defaults (the fall limit 32000, flatten 0xA000,
 * time out of water 0x258000 = 600 s, no light), motion stopped and the particle trails emptied. */
void Wolf::ResetState()
{
    state = WOLF_ST_IDLE;
    surface = WOLF_SURFACE_NORMAL;
    waterZone = 0;
    coverObject = 0;
    killer = 0;
    ClearFlags(WOLF_FB_DEAD | WOLF_FB_IN_WATER | WOLF_FB_RECENT_CONTACT2 | WOLF_FB_RECENT_OBJECT_CONTACT |
               WOLF_FB_MOVED | WOLF_FB_SEESAW_CAM | WOLF_FB_IN_SLIDE_ZONE | WOLF_FB_FORCE_SLIDE |
               WOLF_FB_EQUIP_PENDING | WOLF_FB_WOLFTRAP_REACT | WOLF_FB_IN_WOLFTRAP | WOLF_FB_SPECIAL_OBJECT |
               WOLF_FB_ITEM_ACTION | WOLF_FB_INVISIBLE | WOLF_FB_AXIS_LOCK_A | WOLF_FB_AXIS_LOCK_B |
               WOLF_FB_PROMPT_DISABLED | WOLF_FB_FORCE_FALL | WOLF_FB_IN_SHADOW_ZONE | WOLF_FB_IN_FOOTPRINT_ZONE |
               WOLF_FB_ON_MOVING_PLATFORM | WOLF_FB_JUST_UNFROZEN | WOLF_FB_NO_LOOK);
    flags |= WOLF_FB_CINE_READY;
    ClearFxFlags(WOLF_FA_REQ_SKID | WOLF_FA_CINE_SHEEP_DROP);
    lastObjectContactTime = g_gameTime;
    lastContact2Time = g_gameTime;
    elastic = 0;
    magnetTarget = 0;
    bushNibbleTimer = 0;
    interactPos.x = 0;
    interactPos.y = 0;
    interactPos.z = 0;
    fallLimitY = 32000;
    interactHeading = 0;
    camOverride = CAM_REQ_NONE;
    idleLoopCount = 0;
    ctxAction.target = 0;
    ctxAction.action = CTX_NONE;
    heldActionType = HELD_NONE;
    ctx4PromptIndex = 0;
    coverAction.target = 0;
    coverAction.action = CTX_NONE;
    coverWord2 = 0;
    coverWord3 = 0;
    itemPromptClassId = 0;
    itemPromptLevel = 0;
    updraftZone = 0;
    lineHeading = 0;
    flattenDuration = 0xa000;
    mashCount = 0;
    danceMove = -1;
    timeOutOfWater = 0x258000;
    glowDist = 0x7fffffff;
    scriptWalkTarget.x = 0;
    scriptWalkTarget.y = 0;
    scriptWalkTarget.z = 0;
    carryDropPos.x = 0;
    carryDropPos.y = 0;
    carryDropPos.z = 0;
    scriptWalkHeading = 0;
    wallAvoid.Reset();
    StopMotion();
    airTime = 0;
    slopeTime = 0;
    trailA.base.Emitter_Reset();
    trailB.base.Emitter_Reset();
    particles.Emitter_Reset();
    ClearEffects();
    SetPartHeight(0x100);
}

/* 0x487ea2 - the checkpoint restart (vtable +0x14): let a grabber go (message 0xE), no costume, full size, back on
 * the ground under the saved respawn point facing the saved way, nothing in hand (Wolf_DropHeld(7)), the state reset,
 * on foot, the camera reset behind him, the fade-in, the splash stopped, no tint, and the uncommitted inventory
 * items dropped. `reply` (the grabber's answer) is never read. */
/* BYTES(dead-code): reply is stored and never read, as in the original */
void Wolf::Reset()
{
    s32 reply;
    Vec3s rotNew;
    Vec3s at;
    flags |= WOLF_FB_RESETTING;
    SetAnimSound(1);
    if (flags & WOLF_FB_FROZEN) {
        if (grabber)
            reply = grabber->HandleMessage(this, MSG_FREEZE, 0);
        Unfreeze();
    }
    costume = WMS_RALPH;
    SetScale(0x400);
    at = savedRespawnPos;
    at.y = QueryGroundY(&at, 1);
    SetPosition(&at);
    DropHeld(WOLF_DROP_RELEASE | WOLF_DROP_NO_SNAP | WOLF_DROP_STOP);
    rotNew.x = 0;
    rotNew.y = savedRespawnFacing;
    rotNew.z = 0;
    rot = rotNew;
    ResetState();
    SetMode(WOLF_MODE_NORMAL, WOLF_ST_IDLE);
    PlayAnim(AnimId(), 1, 0);
    Camera_Reset();
    g_camera.rot.y = -Facing() & 0xfff;
    Fade_StartOut(0x1000);
    if (fxFlags & WOLF_FA_SPLASH_ACTIVE)
        Fx2Body_Stop();
    tintColor = 0;
    tintAmount = 0;
    SetTinted(0);
    ClearFlags(WOLF_FB_LOOK_ALLOWED | WOLF_FB_RESETTING);
    Inventory_DropUncommitted();
    Hud_ResetActionPrompt();
}

/* 0x4880f2 - the two movement banks: bank 0 (on foot) with the 0xDF state descriptors and the 18 normal / ice
 * profiles, bank 1 (carrying) with 0x1D states and 2 + 2 profiles, each with its nine parameters and the two
 * per-surface tuning rows (normal, ice). */
void Wolf::InitMoveConfig()
{
    s16 *tuningRow;
    WolfMoveBank *bank;
    bank = &g_wolfMoveBank0[WOLF_MODE_NORMAL];
    bank->states = g_wolfStateDescriptors0;
    bank->stateCount = 0xdf;
    bank->profiles[WOLF_SURFACE_NORMAL] = g_wolfMoveProfilesNormal[WOLF_SURFACE_NORMAL];
    bank->profiles[WOLF_SURFACE_ICE] = g_wolfMoveProfilesNormal[WOLF_SURFACE_ICE];
    bank->profileCount = 0x12;
    bank->params[0] = 0xff;
    bank->params[1] = 0xc8;
    bank->params[2] = 0x4fd;
    bank->params[3] = 0x96;
    bank->params[4] = 0x4cc;
    bank->params[5] = 0x199;
    bank->params[6] = 0xc00;
    bank->params[7] = 0xbb8;
    bank->params[8] = 0xc00;
    tuningRow = bank->surfaceTuning[WOLF_SURFACE_NORMAL];
    tuningRow[0] = 0xb6a;
    tuningRow[1] = 0xdde;
    tuningRow[2] = 0x800;
    tuningRow[3] = 0x578;
    tuningRow[4] = 0x64;
    tuningRow[5] = 0x2bc;
    tuningRow[6] = 0x4b0;
    tuningRow[7] = 0x7d0;
    tuningRow[8] = 0xb54;
    tuningRow = bank->surfaceTuning[WOLF_SURFACE_ICE];
    tuningRow[0] = 0xe8e;
    tuningRow[1] = 0xf7f;
    tuningRow[2] = 0xb6a;
    tuningRow[3] = 0x578;
    tuningRow[4] = 0x64;
    tuningRow[5] = 0x2bc;
    tuningRow[6] = 0x4b0;
    tuningRow[7] = 0x7d0;
    tuningRow[8] = 0xb54;
    bank = &g_wolfMoveBank0[WOLF_MODE_CARRY];
    bank->states = g_wolfStateTable1;
    bank->stateCount = 0x1d;
    bank->profiles[WOLF_SURFACE_NORMAL] = g_wolfMoveProfilesCarry[WOLF_SURFACE_NORMAL];
    bank->profiles[WOLF_SURFACE_ICE] = g_wolfMoveProfilesCarry[WOLF_SURFACE_ICE];
    bank->profileCount = 2;
    bank->params[0] = 0xd3;
    bank->params[1] = 0x82;
    bank->params[2] = 0x48b;
    bank->params[3] = 0;
    bank->params[4] = 0;
    bank->params[5] = 0;
    bank->params[6] = 0;
    bank->params[7] = 0;
    bank->params[8] = 0;
    tuningRow = bank->surfaceTuning[WOLF_SURFACE_NORMAL];
    tuningRow[0] = 0xb6a;
    tuningRow[1] = 0xdde;
    tuningRow[2] = 0x800;
    tuningRow[3] = 0x578;
    tuningRow[4] = 0xc8;
    tuningRow[5] = 0x3e8;
    tuningRow[6] = 0x4b0;
    tuningRow[7] = 0x7d0;
    tuningRow[8] = 0xfa0;
    tuningRow = bank->surfaceTuning[WOLF_SURFACE_ICE];
    tuningRow[0] = 0xe8e;
    tuningRow[1] = 0xf7f;
    tuningRow[2] = 0xb6a;
    tuningRow[3] = 0x578;
    tuningRow[4] = 0xc8;
    tuningRow[5] = 0x3e8;
    tuningRow[6] = 0x4b0;
    tuningRow[7] = 0x7d0;
    tuningRow[8] = 0xfa0;
}

/* 0x48836f - the first init after loading (vtable +0): the costume's animation table shared, the movement banks and
 * the HUD prompt strings, the update policy (a level numbered -1 gets a hidden, silent, never-updated Ralph), the
 * frozen river and Sam looked up, every field to its level-start value, the saved collision box, the state reset,
 * standing on the ground, that spot saved as the respawn point with the camera behind him, the FLYBOX,
 * RESTRICTIONBOX and CLIMBBOXES properties, the prop body with its eight models and the splash body, and the player
 * index counted up (0, then 1). */
void Wolf::PostLoadInit()
{
    u16 *rec;
    u32 id;
    ClearFlags(WOLF_FB_RESETTING);
    if (modelSets[WMS_RABBITCOSTUME].IsValid())
        ShareAnimTable(&modelSets[WMS_RABBITCOSTUME].modelResIdx);
    InitMoveConfig();
    InitActionPromptStrings();
    weight = 0x3c;
    shadow.SetFlag4(1);
    if (g_pProgress->CurrentLevel() == SCENE_WHEEL) {
        SetUpdateMode(SCN_UPD_NEVER);
        SetVisible(0);
        SetAnimSound(0);
    } else {
        SetUpdateMode(SCN_UPD_ALWAYS);
        SetNoDistCull(1);
        SetNoPlaneCull(1);
    }
    bush = 0;
    frozenRiver = 0;
    Scenaric_FindByClass(CLASSID_FROZENRIVER, &frozenRiver, 1);
    sheepCostumeListener = 0;
    Scenaric_FindByClass(CLASSID_SAM, &sheepCostumeListener, 1);
    flags = 0;
    fxFlags = 0;
    savedCamRot.x = 0;
    savedCamRot.y = 0;
    savedCamRot.z = 0;
    savedCamDist = 0x262;
    loopSfxHandle = 0;
    distanceTravelled = 0;
    scale = 0x400;
    waterDepthOffset = 0x5a;
    savedBox = *GetFirstModelBox();
    riderCount = 0;
    grabber = 0;
    heldObject = 0;
    modelSet = WMS_RALPH;
    costume = WMS_RALPH;
    ResetState();
    SnapToGround(1);
    savedRespawnPos = pos;
    savedRespawnFacing = Facing();
    g_camera.rot.y = -Facing() & 0xfff;
    SetMode(WOLF_MODE_NORMAL, WOLF_ST_IDLE);
    PlayAnim(AnimId(), 1, 0);
    rec = record;
    flyBox = Scn_GetPropBox(rec, 4);
    id = Scn_GetPropU32(rec, 8);
    if (id)
        restrictions.Load(id);
    else
        restrictions.Clear();
    id = Scn_GetPropU32(rec, 0);
    if (id)
        climbZones.Load(id);
    else
        climbZones.Clear();
    Scn_BuildRecordFromExport(g_wolfPropBaseModelId, propRecord, 0, 0);
    propBody.InitWithAltModels(propRecord, &propModels[0], 8, g_wolfPropModelIds, &propModels[1]);
    if (Scn_BuildRecordFromExport(WAR_IDO_APLOUF01, splashRecord, 0, 0)) {
        (&splashBody)->Init(splashRecord, 0);
        fxFlags |= WOLF_FA_HAS_SPLASH;
    } else
        ClearFxFlags(WOLF_FA_HAS_SPLASH);
    Hud_ResetActionPrompt();
    playerIndex = g_wolfInstanceCount++;
    if (playerIndex >= 2)
        playerIndex = 1;
    Fade_StartOut(0x1000);
}

/* ---- the class factory ---- */
/* 0x488a51 - the class factory for CLASSID 0 "Wolf": new Wolf (every constructor on the way inlined: the base classes'
 * vtables in turn, the two 16-particle trails, the particle emitter with no pool, the prop and splash bodies, then the
 * Wolf vtable), then the model sets: the record's model plus the five costume/prop sets of g_wolfAltModelIds. A level
 * with PRINTSBOX zones gets a 128-particle pool for footprints. */
ScnObject *Wolf_Create(void *record)
{
    Wolf *wolf = new Wolf;
    /* cast kept: InitWithAltModels returns the object as a ScnBody * */
    wolf = (Wolf *)wolf->InitWithAltModels(record, &wolf->modelSets[0], 5, g_wolfAltModelIds, &wolf->modelSets[1]);
    g_pWolf = wolf;
    g_wolfInstanceCount = 0;
    if (Zones_Exist(ZONE_PRINTS))
        wolf->particles.Emitter_Init(0x80);
    return wolf;
}
