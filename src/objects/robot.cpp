/*
 * T087 - original object Robot.cpp (guessed name): .text 0x45e570-0x461f19, .rdata 0x574fa8-0x574ff8 (Robot's vtable,
 * then the ??_7ScnControllable COMDAT, first emitted here), .data 0x57a870-0x57a988 (8-aligned: the cinematic header's
 * stride-table copy, padding, g_robotStateTable, the three move records, the surface tuning row, the idle tables and
 * the dash-smoke parameters, all defined here in address order).
 * The member lists are wolf.h's plus, through SDW_EXTRA_<Class>, the members only the Robot's code needs; its inline
 * helpers follow wolf.h's.
 *
 * Robot_Update also matches its 27-entry jump table at 0x460b27, and Robot_HandleMessage and
 * Robot_EnterState theirs after the code (the 0x6c-entry byte index of the message switch, the MSG_CARRY_ANIM argument
 * table, and the two SetUpdateMode tables).
 *
 * Robot (CLASSID 101 "Robot", the remote-controlled robot Ralph drives), SheepD3D.exe 0x45e570-0x461f19:
 *  - 0x45e570-0x45f779: the 22 small methods before Robot_Update - the stick-heading override, the camera request, the
 *    put-down point, the fall / jump tests, the context actions, the common mover and the movement steps, the
 *    per-state step drivers, and taking and releasing control;
 *  - 0x45f77a: Robot_Update, the vtable +0x04 override, one function of 5,037 bytes;
 *  - 0x460b93-0x461f19: the message handler (vtable +0x10), Render (+0x08), the idle helpers, the state-entry family
 *    (EnterState, SetState, SetStateAnim, SetStateKeepAnim), the state reset (ClearState), the checkpoint restart
 *    (Reset, +0x14), the first init (PostLoadInit = Robot_Init, +0x00) and the class factory (Robot_Create).
 * g_robotStateTable 0x57a880 holds a RobotStateDesc {u16 animId; u16 flags} per state (0x46191e mov dx,word [ecx];
 * its flags word is read as movzx word [desc+2], 0x45e723, 0x45e7ff, 0x45ebde, 0x45ef45, 0x461664). Robot+0x120
 * smokeEmitter is a TrailEmitter (Robot_Create 0x461eae-0x461edb is its constructor: pools at +0x24 / +0xe4, count 16,
 * Emitter_Reset).
 *
 * The Robot is a ScnControllable like the Wolf, and its code is a smaller copy of the Wolf's: the same scratch block
 * g_sharedScratch 0x6d5468 reached through a pointer local, the same Mobile_Steer / Wolf_SurfaceVelocity /
 * Vec3s_ScaleByDt / Wolf_MoveResolve chain, one walk record, one dash record and one surface tuning row. robotFlags
 * (+0x2dc) are the RobotFlags (ROBOT_F_*) of sdw_enums.h.
 *
 * Robot_Update, once a frame: advance the state and air timers, then either the passive states (ROBOT_ST_DORMANT,
 * ROBOT_ST_IDLE falling / riding, ROBOT_ST_CARRIED; ROBOT_SF_ACTIVE clear) or, for the controllable states, read the
 * pad and run the state's step and transitions. Then carry the held object, the death / water zones, the shadow-zone
 * flag, the dash smoke, the tint and the animation. ROBOT_ST_LAUNCHED has no case. Angles: 4096 per turn. Vertical
 * points down.
 *
 * Match devices, not claims about the source text: local names are chosen for their /Od stack slots
 * (tools/vc6_locals.py); `w` pins Robot_Update's named locals at their stack offsets; the small inline helpers are
 * there for their /Ob1 expansions (each gives a stack temporary the original has - in Robot_Update handed out in
 * source order from EBP-0x34 down to `this` at EBP-0x94, the compiler's own switch / boolean temps following `this`;
 * a constant argument loaded into a register before it is tested; the update-mode setter's four-way jump table on a
 * stack copy of its constant), as in the Wolf's code. Their names are descriptive. SetState's loop flag is a named local set
 * by an if/else (0x46190b), and MSG_PICKUP's five locals (attachRot -8, offset -0x10, here -0x18, parent -0x1c, hook
 * -0x1d) share one block. SetUpdateMode takes a u8: the constant 0 then goes through xor edx,edx
 * (0x4617f8) while 1 is stored directly (0x46174f), as in the original. Robot::ClearFlags complements its mask at run
 * time (mov eax,0x3bde; not eax), where `robotFlags &= ~0x3bde` would fold it.
 */
/* BYTES: dead-code, layout, slot-group, slot-name, slot-scope, temp. */
/* BYTES(layout): the Cine.h header static: every object including the cinematic header carries this copy in its .data, referenced or not */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"

class ScnObject;
class Camera;
#include "../app/app_main.h"
#include "../engine/fade.h"
#include "camera.h"
#include "../engine/input.h"
#include "../engine/scn_tools.h"
void Camera_StartScripted(ScnObject *owner, Camera *cam, u16 rx, u16 ry, u16 rz, Vec3s *eye, u16 focal, u32 flags,
                          s32 time); /* 0x55a70d */

/* Members the Robot's code needs beyond wolf.h's. */
#define SDW_EXTRA_ScnObject                                                           \
    /* inline: the mask in a register (0x4605de, 0x461336) */                         \
    inline void SetTint(u32 color, s16 amount, s32 on);                               \
    void StartCamera(u16 x, u16 y, u16 z, Vec3s *eye, u16 focal, u32 mode, s32 time); \
    void SetUpdateMode(u8 mode);                                                      \
    /* inline, defined below */
#define SDW_EXTRA_ScnMobile \
    Shadow *GetShadow()     \
    {                       \
        return &shadow;     \
    } /* inline: the shadow block at +0x64 (0x4603f9) */

#define SDW_MEMBERS_Robot                                                                                                                                \
    u16 StateSound()                                                                                                                                     \
    {                                                                                                                                                    \
        return stateSoundHandle;                                                                                                                         \
    } /* inline: a stack copy (0x45fa95) */ /* inline: the constant mask is loaded and complemented in a register (mov eax,0x21; not eax at 0x45f741) */ \
    void ClearRobotFlags(u32 mask)                                                                                                                       \
    {                                                                                                                                                    \
        robotFlags &= ~mask;                                                                                                                             \
    } /* inline: the same, as spelled by Update and the state code (0x45f820, 0x4619ff mov eax,0x3bde; not eax) */                                       \
    void ClearFlags(u32 mask)                                                                                                                            \
    {                                                                                                                                                    \
        robotFlags &= ~mask;                                                                                                                             \
    }
#include "../game/wolf.h" /* the Wolf's shared declarations (callees, globals, the ScnControllable members) */
#define SDW_INLINE_SCNOBJECT_INSTFLAGS_U16 1
#define SDW_INLINE_SCNOBJECT_STARTCAMERA_U16_U16_U16_VEC3S_U16_U32_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_INSTFLAGS_U16
#undef SDW_INLINE_SCNOBJECT_STARTCAMERA_U16_U16_U16_VEC3S_U16_U32_S32
#define SDW_INLINE_SCNOBJECT_SETNODISTCULL_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETNODISTCULL_S32
#define SDW_INLINE_SHADOW_REPROJECT 1
#include "../engine/shadow_inlines.h"
#undef SDW_INLINE_SHADOW_REPROJECT

/* ---- .data 0x57a870-0x57a988, in address order ---- */

/* 0x57a870 - the cinematic header's static copy of the 9-byte opcode stride table (src/engine/cine.cpp,
 * g_cineOpStride 0x5816fc). The original header defined it static, so every object including it carries its own
 * unreferenced copy at the head of its .data; defined here in its place (the 120-byte state table below gives the
 * section 8-byte alignment, so it starts at +0x10). */
static u8 s_cineOpStride[9] = {0, 8, 8, 4, 2, 2, 4, 2, 2};
/* 0x57a880 - per Robot state {animId, RobotStateFlags} */
RobotStateDesc g_robotStateTable[30] = {
    {AROBOT01_ANIM_STAND, ROBOT_SF_GROUNDED},
    {AROBOT01_ANIM_STAND, ROBOT_SF_ALWAYS_UPDATE | ROBOT_SF_GROUNDED},
    {AROBOT01_ANIM_LINK, ROBOT_SF_CARRIED | ROBOT_SF_ANIM_LOOP},
    {AROBOT01_ANIM_WEAK, ROBOT_SF_ALWAYS_UPDATE | ROBOT_SF_ACTIVE | ROBOT_SF_GROUNDED},
    {AROBOT01_ANIM_STAND1, ROBOT_SF_ALWAYS_UPDATE | ROBOT_SF_ANIM_LOOP | ROBOT_SF_ACTIVE | ROBOT_SF_SCAN_ACTIONS |
                               ROBOT_SF_CAM_LEDGE_PROBE | ROBOT_SF_GROUNDED},
    {AROBOT01_ANIM_RUN, ROBOT_SF_ALWAYS_UPDATE | ROBOT_SF_ANIM_LOOP | ROBOT_SF_ACTIVE | ROBOT_SF_SCAN_ACTIONS |
                            ROBOT_SF_CAM_LEDGE_PROBE | ROBOT_SF_GROUNDED},
    {AROBOT01_ANIM_SPEED, ROBOT_SF_ALWAYS_UPDATE | ROBOT_SF_ANIM_LOOP | ROBOT_SF_ACTIVE | ROBOT_SF_CAM_LEDGE_PROBE |
                              ROBOT_SF_SMOKE | ROBOT_SF_GROUNDED},
    {AROBOT01_ANIM_STAND1,
     ROBOT_SF_ALWAYS_UPDATE | ROBOT_SF_ANIM_LOOP | ROBOT_SF_ACTIVE | ROBOT_SF_SCAN_ACTIONS | ROBOT_SF_CAM_LEDGE_PROBE},
    {AROBOT01_ANIM_JUMP1,
     ROBOT_SF_ALWAYS_UPDATE | ROBOT_SF_ACTIVE | ROBOT_SF_SCAN_ACTIONS | ROBOT_SF_CAM_LEDGE_PROBE | ROBOT_SF_GROUNDED},
    {AROBOT01_ANIM_JUMP2, ROBOT_SF_ALWAYS_UPDATE | ROBOT_SF_ACTIVE | ROBOT_SF_SCAN_ACTIONS},
    {AROBOT01_ANIM_JUMP3, ROBOT_SF_ALWAYS_UPDATE | ROBOT_SF_ACTIVE | ROBOT_SF_SCAN_ACTIONS | ROBOT_SF_CAM_LEDGE_PROBE},
    {AROBOT01_ANIM_TAKE,
     ROBOT_SF_ALWAYS_UPDATE | ROBOT_SF_ACTIVE | ROBOT_SF_SCAN_ACTIONS | ROBOT_SF_CAM_LEDGE_PROBE | ROBOT_SF_GROUNDED},
    {AROBOT01_ANIM_TAKE1, ROBOT_SF_ALWAYS_UPDATE | ROBOT_SF_ACTIVE | ROBOT_SF_CAM_LEDGE_PROBE | ROBOT_SF_GROUNDED},
    {AROBOT01_ANIM_PUT, ROBOT_SF_ALWAYS_UPDATE | ROBOT_SF_ACTIVE | ROBOT_SF_CAM_LEDGE_PROBE | ROBOT_SF_GROUNDED},
    {AROBOT01_ANIM_PUT2, ROBOT_SF_ALWAYS_UPDATE | ROBOT_SF_ACTIVE | ROBOT_SF_CAM_LEDGE_PROBE | ROBOT_SF_GROUNDED},
    {AROBOT01_ANIM_STAND1, ROBOT_SF_ALWAYS_UPDATE | ROBOT_SF_ACTIVE | ROBOT_SF_CAM_LEDGE_PROBE},
    {AROBOT01_ANIM_BLOW, ROBOT_SF_ALWAYS_UPDATE | ROBOT_SF_ACTIVE | ROBOT_SF_CAM_LEDGE_PROBE},
    {AROBOT01_ANIM_KICK, ROBOT_SF_ALWAYS_UPDATE | ROBOT_SF_ACTIVE | ROBOT_SF_CAM_LEDGE_PROBE | ROBOT_SF_EJECTED},
    {AROBOT01_ANIM_KICK2,
     ROBOT_SF_ALWAYS_UPDATE | ROBOT_SF_ANIM_LOOP | ROBOT_SF_ACTIVE | ROBOT_SF_CAM_LEDGE_PROBE | ROBOT_SF_EJECTED},
    {AROBOT01_ANIM_KICK2,
     ROBOT_SF_ALWAYS_UPDATE | ROBOT_SF_ANIM_LOOP | ROBOT_SF_ACTIVE | ROBOT_SF_CAM_LEDGE_PROBE | ROBOT_SF_EJECTED},
    {AROBOT01_ANIM_KICK3,
     ROBOT_SF_ALWAYS_UPDATE | ROBOT_SF_ACTIVE | ROBOT_SF_CAM_LEDGE_PROBE | ROBOT_SF_GROUNDED | ROBOT_SF_EJECTED},
    {AROBOT01_ANIM_STAND1, ROBOT_SF_ALWAYS_UPDATE | ROBOT_SF_ACTIVE | ROBOT_SF_CAM_LEDGE_PROBE | ROBOT_SF_GROUNDED},
    {AROBOT01_ANIM_TAKE,
     ROBOT_SF_ALWAYS_UPDATE | ROBOT_SF_ACTIVE | ROBOT_SF_SCAN_ACTIONS | ROBOT_SF_CAM_LEDGE_PROBE | ROBOT_SF_GROUNDED},
    {AROBOT01_ANIM_TRAP0, ROBOT_SF_ALWAYS_UPDATE | ROBOT_SF_ACTIVE | ROBOT_SF_CAM_LEDGE_PROBE | ROBOT_SF_GROUNDED},
    {AROBOT01_ANIM_TRAP1,
     ROBOT_SF_ALWAYS_UPDATE | ROBOT_SF_ANIM_LOOP | ROBOT_SF_ACTIVE | ROBOT_SF_CAM_LEDGE_PROBE | ROBOT_SF_GROUNDED},
    {AROBOT01_ANIM_TRAP2,
     ROBOT_SF_ALWAYS_UPDATE | ROBOT_SF_ANIM_LOOP | ROBOT_SF_ACTIVE | ROBOT_SF_CAM_LEDGE_PROBE | ROBOT_SF_GROUNDED},
    {AROBOT01_ANIM_TRAP3, ROBOT_SF_ALWAYS_UPDATE | ROBOT_SF_ACTIVE | ROBOT_SF_CAM_LEDGE_PROBE | ROBOT_SF_GROUNDED},
    {AROBOT01_ANIM_TAKE,
     ROBOT_SF_ALWAYS_UPDATE | ROBOT_SF_ACTIVE | ROBOT_SF_SCAN_ACTIONS | ROBOT_SF_CAM_LEDGE_PROBE | ROBOT_SF_GROUNDED},
    {AROBOT01_ANIM_TAKE1, ROBOT_SF_ALWAYS_UPDATE | ROBOT_SF_ACTIVE | ROBOT_SF_CAM_LEDGE_PROBE | ROBOT_SF_GROUNDED},
    {AROBOT01_ANIM_TAKE, ROBOT_SF_ALWAYS_UPDATE | ROBOT_SF_ACTIVE | ROBOT_SF_CAM_LEDGE_PROBE | ROBOT_SF_GROUNDED}};
/* 0x57a8f8 - walk: maxSpeed 300 */
MoveRecord g_robotWalkMoveRec = {300, 1000, 1000, 0, 20480, 20480, 61440, 61440, 2048};
/* 0x57a914 - dash: maxSpeed 600; its maxSpeed also sets the smoke spawn interval (Update) */
MoveRecord g_robotDashMoveRec = {600, 600, 600, 0, 1706, 1706, 20480, 20480, 2048};
/* 0x57a930 - a third record (maxSpeed 1200) no code refers to */
MoveRecord g_robotMoveRec2_unused = {1200, 1200, 1200, 0, 20480, 20480, 20480, 20480, 0};
/* 0x57a94c - [2] jump slope, [3] normal cutoff, [7] max fall, [8] gravity (0x57a95e-0x57a960 is alignment padding) */
s16 g_robotSurfaceTuning[9] = {4096, 4096, 2048, 2900, 50, 0, 600, 1500, 2000};
/* 0x57a960 - the idle variations {animId, fewest loops, most loops} */
IdleAnimEntry g_robotIdleVariants[3] = {
    {AROBOT01_ANIM_STAND3, 3, 5}, {AROBOT01_ANIM_STAND4, 1, 1}, {AROBOT01_ANIM_STAND5, 3, 5}};
/* 0x57a96c - the base idle: STAND1 for 10..20 loops */
IdleAnimEntry g_robotIdleBase = {AROBOT01_ANIM_STAND1, 10, 20};
/* 0x57a970 - the dash smoke; its spawnInterval is rewritten by Update */
EmitterDriftParams g_robotDashSmokeParams = {100, -20, 2608, 163, 20, 60, 0};

/* ---- callees and globals (wolf.h has the rest) ---- */

/* ---- inline helpers of Robot_Update ---- */

/* Set / clear bits of a 16-bit flag word through its address: the address is a stack temp and the constant mask is
 * loaded into a register (0x4609aa); the clear complements it and narrows it to 16 bits first (0x4609ce). */
#define SDW_INLINE_FREE_BITS16_SET_U16_U16 1
#include "instance_inlines.h"
#undef SDW_INLINE_FREE_BITS16_SET_U16_U16

#define SDW_INLINE_FREE_BITS16_CLEAR_U16_U16 1
#include "instance_inlines.h"
#undef SDW_INLINE_FREE_BITS16_CLEAR_U16_U16

/* Colour tint: colour, strength and the instance flag 0x10 that turns the tinted draw on; a temp per arm (0x4609a1,
 * 0x4609c2). */
#define SDW_INLINE_SCNOBJECT_SETTINT_U32_S16_S32 2
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETTINT_U32_S16_S32

/* The blob shadow on or off (flag 1 = not drawn), on the ScnMobile's shadow block (+0x64). */
#define SDW_INLINE_SHADOW_SETVISIBLE_S32 1
#include "../engine/shadow_inlines.h"
#undef SDW_INLINE_SHADOW_SETVISIBLE_S32

/* The scripted camera's owner, or 0 unless the camera runs mode 7, 9 or 10 (0x46014d-0x46019a). */
#define SDW_INLINE_FREE_CAMERA_HASSCRIPTOWNER 1
#include "camera_inlines.h"
#undef SDW_INLINE_FREE_CAMERA_HASSCRIPTOWNER

#define SDW_INLINE_FREE_CAMERA_GETSCRIPTOWNER 1
#include "camera_inlines.h"
#undef SDW_INLINE_FREE_CAMERA_GETSCRIPTOWNER

/* Clear the stick words through their address, a stack temp: stickX, stickY, stickMag (0x46025c), and with padBits
 * the whole input block (0x45f8bc). */
#define SDW_INLINE_FREE_STICK_CLEAR_S32 1
#include "../game/scn_controllable_inlines.h"
#undef SDW_INLINE_FREE_STICK_CLEAR_S32

inline void Input_Clear(s32 *input)
{
    input[0] = 0;
    input[1] = 0;
    input[2] = 0;
    input[3] = 0;
}

/* ---- inline helpers of the message handler and the state code ---- */

/* Stop a sound this object started: the handle argument is a stack temp (0x461640). */
#define SDW_INLINE_SCNOBJECT_STOPSOUND_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_STOPSOUND_U16

/* Whether the collision sweeps test the object's boxes: flag 0x400 clears when on. */
#define SDW_INLINE_SCNOBJECT_SETBOXCOLLIDE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETBOXCOLLIDE_S32

/* The update policy (ScnUpdateMode): by distance, always, never, or also in cinematics. */
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8

/* The instance flags word (+4) through a pointer (0x461c4f). */
#define SDW_INLINE_FREE_INSTFLAGS_SET_U16_U16 1
#include "instance_inlines.h"
#undef SDW_INLINE_FREE_INSTFLAGS_SET_U16_U16

#define SDW_INLINE_FREE_INSTFLAGS_CLEAR_U16_U16 1
#include "instance_inlines.h"
#undef SDW_INLINE_FREE_INSTFLAGS_CLEAR_U16_U16

/* INST_F_TINT (0x10), the tint override. */
#define SDW_INLINE_SCNOBJECT_SETTINTED_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETTINTED_S32

/* The four words of the controller state (stickX, stickY, stickMag, padBits at +0xc8) cleared through a pointer
 * (0x461a1a). */
inline void PadState_Clear(s32 *p)
{
    p[0] = 0;
    p[1] = 0;
    p[2] = 0;
    p[3] = 0;
}

/* 0x45e570 - the vtable +0x24 override: either axis lock snaps the stick heading to one axis, ROBOT_F_AXIS_LOCK_A to
 * 90 / 270 degrees, otherwise (ROBOT_F_AXIS_LOCK_B) to 0 / 180, by rounding to the nearer of the two (the Wolf's
 * 0x488be0 with other bits). */
s16 Robot::GetStickHeading(s16 fallback)
{
    s16 heading = ScnControllable::GetStickHeading(fallback);
    if (robotFlags & (ROBOT_F_AXIS_LOCK_A | ROBOT_F_AXIS_LOCK_B)) {
        if (robotFlags & ROBOT_F_AXIS_LOCK_A) {
            heading = (heading + 2048) & 4095;
            heading = (heading - heading % 2048 - 1024) & 4095;
        } else {
            heading = (heading + 1024) & 4095;
            heading = heading - heading % 2048;
        }
    }
    return heading;
}

/* 0x45e62e - MSG_GRABBER_CAMERA from the frozen Wolf's camera update: a View-button release gives control back (first
 * re-freezing a third party that suspended the robot, if it accepts), and ROBOT_ST_IDLE unless ROBOT_F_DEAD or an
 * ejected state. While the robot is driven the camera request follows it in CAM_ROBOT, which it switches to from the
 * ordinary modes (and from a scripted mode that does not return to CAM_ROBOT); coming back restores the saved orbit. */
s32 Robot::UpdateCamera(Pad *pad)
{
    RobotStateDesc *desc;
    s32 bringBack;
    desc = &g_robotStateTable[state];
    if (!(pad->cur.buttons & ~g_padMasks[8]) && (pad->prev.buttons & ~g_padMasks[8]) && !(g_gameFlags & GF_BIT0) &&
        !Map_IsOpen() && !g_camDebugMode) {
        if (suspender && suspender->HandleMessage(this, MSG_FREEZE, 0)) {
            robotFlags |= ROBOT_F_CONTROLLED;
            suspender = 0;
        }
        if (!suspender) {
            ReleaseControl();
            if (!(robotFlags & ROBOT_F_DEAD) && !(desc->flags & ROBOT_SF_EJECTED))
                SetState(ROBOT_ST_IDLE);
        }
    }
    if (!g_camDebugMode && (robotFlags & ROBOT_F_DRIVEN)) {
        g_camReqTarget = pos;
        g_camReqAimOffset = boundCenter;
        g_camReqVel = velocity;
        g_camReqRot.x = 0;
        g_camReqRot.y = (-Facing() + 0x800) & 0xfff;
        g_camReqRot.z = 0;
        g_camReqDist = 0;
        g_camReqFocal = 0;
        g_camReqSpeedFactor = 0x1000;
        if ((desc->flags & ROBOT_SF_CAM_LEDGE_PROBE) && !(robotFlags & ROBOT_F_ON_OBJECT))
            g_camReqFlags.ledgeProbe = 1;
        else
            g_camReqFlags.ledgeProbe = 0;
        g_camReqFlags.snapYaw = 0;
        g_camReqFlags.usePitch = 0;
        g_camReqFlags.useYaw = 0;
        g_camReqFlags.padRotate = 1;
        g_camReqFlags.restriction = 0;
        if (g_camMode != CAM_ROBOT) {
            bringBack = 0;
            switch (g_camMode) {
                case CAM_FOLLOW:
                case CAM_FOLLOW_RUN:
                case CAM_ROCKET:
                case CAM_FIXED_LOOKAT:
                case CAM_DIRECTED:
                    Camera_SetMode(CAM_ROBOT, 0);
                    bringBack = 1;
                    break;
                default:
                    if (Camera_IsScriptedOrReturning() && g_camScriptReturnMode != CAM_ROBOT) {
                        g_camScriptFlags &= ~CAMSCR_BLEND_OUT;
                        Camera_SetMode(CAM_ROBOT, 0);
                        bringBack = 1;
                    }
            }
            if (bringBack && g_camMode == CAM_ROBOT) {
                g_camReqFlags.useYaw = 1;
                g_camReqFlags.usePitch = 1;
                g_camReqRot = savedCamRot;
                g_camReqDist = savedCamDist;
                g_camReqVel.x = g_camReqVel.y = g_camReqVel.z = 0;
            }
        } else {
            savedCamRot = g_camera.rot;
            savedCamDist = g_camera.dist;
        }
        return 1;
    }
    return 0;
}

/* 0x45e9f1 - where the held object would be put down (pos + dropOffset) and whether it may be: not while touching a
 * movable object, and only onto ground no more than 20 below that point. */
s32 Robot::GetDropPoint(Vec3s *out)
{
    s16 floorY;
    if (!(robotFlags & ROBOT_F_TOUCHING_MOVABLE)) {
        out->x = dropOffset.x + pos.x;
        out->y = dropOffset.y + pos.y;
        out->z = dropOffset.z + pos.z;
        floorY = heldObject->World_GroundYRay(out, 1);
        return floorY >= out->y && floorY <= out->y + 0x14;
    }
    return 0;
}

/* 0x45eab1 - falling: in the air for more than 0x800 ticks, or more than 0x200 with the ground over 200 below. */
s32 Robot::IsFalling()
{
    return airTime > 0x800 || (airTime > 0x200 && GroundY() - pos.y > 200);
}

/* 0x45eb0f - a jump may start: the ground within 10 below or no more than 0x111 ticks in the air, and the ground normal
 * steeper-up than the jump slope (tuning word 2, read unsigned). */
s32 Robot::CanJump()
{
    return (GroundY() - pos.y <= 10 || airTime <= 0x111) && groundNormal.y < -(u16)g_robotSurfaceTuning[2];
}

/* 0x45eb72 - this frame's context action. Empty-handed, in a state with ROBOT_SF_SCAN_ACTIONS, the objects around
 * answer MSG_QUERY_ACTION; holding something, the held object's MSG_QUERY_HELD_ACTION answer is the held action and it
 * is asked for its prompt (MSG_QUERY_NEAREST_TARGET). The prompt id is preset to CLASSID_ROBOT, the "none" value. */
void Robot::ScanContextActions()
{
    RobotStateDesc *desc;
    ctxAction.target = 0;
    ctxAction.action = CTX_NONE;
    heldActionType = HELD_NONE;
    ctxPromptExtra = 1;
    itemPromptId = CLASSID_ROBOT;
    desc = &g_robotStateTable[state];
    if (!heldObject) {
        if (desc->flags & ROBOT_SF_SCAN_ACTIONS)
            ScanInteractables(&ctxAction, 0, 0x1e, 0x1e, 0x3c, 0x400, 0);
    } else {
        heldActionType = heldObject->HandleMessage(this, MSG_QUERY_HELD_ACTION, 0);
        heldObject->HandleMessage(this, MSG_QUERY_NEAREST_TARGET, &itemPromptId);
    }
}

/* 0x45ec5f - whether the scanned action may fire: held actions HELD_NONE, HELD_USE and HELD_KEY_USE always,
 * HELD_THROWABLE (put down) on the ground; context actions CTX_USE, CTX_ACTIVATE, CTX_PICKUP, CTX_WOLFTRAP and
 * CTX_TIMEKEEPER on the ground. */
s32 Robot::IsContextActionAllowed(s32 onGround)
{
    s32 ok;
    ok = 0;
    switch (ctxAction.action) {
        case CTX_NONE:
            switch (heldActionType) {
                case HELD_NONE:
                case HELD_USE:
                case HELD_KEY_USE:
                    ok = 1;
                    break;
                case HELD_THROWABLE:
                    if (onGround)
                        ok = 1;
            }
            break;
        case CTX_USE:
        case CTX_ACTIVATE:
        case CTX_PICKUP:
        case CTX_WOLFTRAP:
        case CTX_TIMEKEEPER:
            if (onGround)
                ok = 1;
    }
    return ok;
}

/* 0x45ed32 - run the action on an Action press; 1 when the state changed. HELD_THROWABLE: put down
 * (MSG_HELD_STATE_BEGIN, ROBOT_ST_PUTDOWN); HELD_USE / HELD_KEY_USE: MSG_HELD_STATE_BEGIN only. CTX_PICKUP picks up
 * (ROBOT_ST_PICKUP_REACH), CTX_WOLFTRAP springs the wolf trap (ROBOT_ST_TRAP_REACH), CTX_USE sends MSG_USE,
 * CTX_ACTIVATE activates (ROBOT_ST_ACTIVATE), CTX_TIMEKEEPER sends MSG_USE and enters ROBOT_ST_TIMEKEEPER. */
s32 Robot::DoContextAction()
{
    switch (ctxAction.action) {
        case CTX_NONE:
            switch (heldActionType) {
                case HELD_THROWABLE:
                    heldObject->HandleMessage(this, MSG_HELD_STATE_BEGIN, 0);
                    SetState(ROBOT_ST_PUTDOWN);
                    return 1;
                case HELD_USE:
                case HELD_KEY_USE:
                    heldObject->HandleMessage(this, MSG_HELD_STATE_BEGIN, 0);
            }
            break;
        case CTX_PICKUP:
            SetState(ROBOT_ST_PICKUP_REACH);
            return 1;
        case CTX_WOLFTRAP:
            SetState(ROBOT_ST_TRAP_REACH);
            return 1;
        case CTX_USE:
            ctxAction.target->HandleMessage(this, MSG_USE, 0);
            break;
        case CTX_ACTIVATE:
            SetState(ROBOT_ST_ACTIVATE);
            return 1;
        case CTX_TIMEKEEPER:
            ctxAction.target->HandleMessage(this, MSG_USE, 0);
            SetState(ROBOT_ST_TIMEKEEPER);
            return 1;
    }
    return 0;
}

/* 0x45ee9e - drop an action that may not fire, then run it on an Action press (WOLF_ACT_ACTION_EDGE). */
s32 Robot::TryContextAction(s32 onGround)
{
    if (!IsContextActionAllowed(onGround)) {
        ctxAction.target = 0;
        ctxAction.action = CTX_NONE;
        heldActionType = HELD_NONE;
    }
    if (padBits & WOLF_ACT_ACTION_EDGE)
        return DoContextAction();
    return 0;
}

/* 0x45eefe - the common mover: take the new rotation, let riders modify the move (grounded states), then collide and
 * slide; a movable contact sets ROBOT_F_TOUCHING_MOVABLE, a floor object ROBOT_F_ON_OBJECT. */
/* BYTES(slot-name): names chosen for their stack slots: desc -4, info -0x20, collResult -0x24 */
u32 Robot::Move(Vec3s *delta, Vec3s *vel, const Vec3s *newRot, u16 mask)
{
    RobotStateDesc *desc; /* the names order the frame: desc -4, info -0x20, collResult -0x24 */
    ContactInfo info;
    u32 collResult;
    desc = &g_robotStateTable[state];
    rot = *newRot;
    if (riderCount > 0)
        ApplyMoveModifiers(delta, vel, desc->flags & ROBOT_SF_GROUNDED, 0, 1);
    collResult = Wolf_MoveResolve(delta, &info, g_robotSurfaceTuning[3], mask, 0x19, 0x32, 10);
    if (info.movableObj)
        robotFlags |= ROBOT_F_TOUCHING_MOVABLE;
    if (info.floorObj)
        robotFlags |= ROBOT_F_ON_OBJECT;
    return collResult;
}

/* 0x45efc5 - walk: steer by the stick with the walk record, slope and gravity, move (colliding with the world and the
 * objects while anything moves). */
u32 Robot::WalkStep()
{
    u16 mask;
    WolfMoveScratch *m;
    Vec3s step;
    m = (WolfMoveScratch *)g_sharedScratch; /* cast kept: the shared scratch block is raw bytes */
    m->rot = rot;
    Mobile_Steer(&m->vel, &m->rot, &g_robotWalkMoveRec, 0);
    Wolf_SurfaceVelocity(&m->vel, 0, g_robotSurfaceTuning);
    Vec3s_ScaleByDt(&m->vel, &step);
    if (!(stickX | stickY | step.x | step.z))
        mask = 0;
    else
        mask = RESOLVE_SLIDE_ALL;
    return Move(&step, &m->vel, &m->rot, mask);
}

/* 0x45f07a - dash: full dash-record speed along the stick (or the facing), slope and gravity, move. */
/* BYTES(dead-code): unused is set and never read (mov byte [ebp-1],0 at 0x45f083) */
u32 Robot::DashStep()
{
    u8 unused; /* set and never read (0x45f083 mov byte [ebp-1],0) */
    WolfMoveScratch *m;
    Vec3s step;
    unused = 0;
    m = (WolfMoveScratch *)g_sharedScratch; /* cast kept: the shared scratch block is raw bytes */
    m->rot = rot;
    SteerRun(&m->vel, &m->rot, &g_robotDashMoveRec, 1);
    Wolf_SurfaceVelocity(&m->vel, 0, g_robotSurfaceTuning);
    Vec3s_ScaleByDt(&m->vel, &step);
    return Move(&step, &m->vel, &m->rot, RESOLVE_SLIDE_ALL);
}

/* 0x45f100 - turn toward a heading while slowing to a stop. */
u32 Robot::TurnToHeadingStep(s16 heading)
{
    WolfMoveScratch *m;
    Vec3s step;
    m = (WolfMoveScratch *)g_sharedScratch; /* cast kept: the shared scratch block is raw bytes */
    m->rot = rot;
    SteerToHeading(&m->vel, &m->rot, &g_robotWalkMoveRec, heading);
    Wolf_SurfaceVelocity(&m->vel, 0, g_robotSurfaceTuning);
    Vec3s_ScaleByDt(&m->vel, &step);
    return Move(&step, &m->vel, &m->rot, 0);
}

/* 0x45f187 - fall: steer, then gravity (tuning[8] per 4096 ticks of air time) capped at tuning[7]. */
u32 Robot::FallStep(u16 mask)
{
    s32 fall;
    WolfMoveScratch *m;
    Vec3s step;
    m = (WolfMoveScratch *)g_sharedScratch; /* cast kept: the shared scratch block is raw bytes */
    m->rot = rot;
    Mobile_Steer(&m->vel, &m->rot, &g_robotWalkMoveRec, 0);
    fall = g_robotSurfaceTuning[8] * airTime >> 12;
    if (m->vel.y + fall > g_robotSurfaceTuning[7])
        m->vel.y = g_robotSurfaceTuning[7];
    else
        m->vel.y += (s16)fall;
    Vec3s_ScaleByDt(&m->vel, &step);
    return Move(&step, &m->vel, &m->rot, mask);
}

/* 0x45f24a - the rise of a jump (ROBOT_ST_JUMP_RISE): air time held at 0, the upward speed falls linearly from about 1000 u/s to
 * 0 over the first 0x4cc ticks of the state. */
u32 Robot::JumpRiseStep()
{
    s32 t;
    WolfMoveScratch *m;
    Vec3s step;
    m = (WolfMoveScratch *)g_sharedScratch; /* cast kept: the shared scratch block is raw bytes */
    m->rot = rot;
    airTime = 0;
    Mobile_Steer(&m->vel, &m->rot, &g_robotWalkMoveRec, 0);
    t = stateTime;
    if (t > 0x4cc)
        t = 0x4cc;
    m->vel.y -= (s16)((0x4cc - t) * 300 / 0x170);
    Vec3s_ScaleByDt(&m->vel, &step);
    return Move(&step, &m->vel, &m->rot, COLL_WALL);
}

/* 0x45f30b - the scripted flight after Sam's kick (ROBOT_ST_EJECT_FLIGHT), without collision: over 2 s (0x2000 ticks) from
 * ejectStartPos to the landing point ejectTraj pts[1], x / z linear in t, the height a parabola up to pts[0].y
 * (reached at 1 s) and down again; the body faces along the flight. Returns 1 once the time is up. */
/* BYTES(dead-code): desc is computed and never read, as in the original */
/* BYTES(slot-name): names chosen for their stack slots: dz -4, desc -8, t -0xc, newPos -0x14, f -0x18, to -0x1c */
s32 Robot::EjectFlightStep()
{
    s32 apex; /* the names order the frame: dz -4, desc -8, t -0xc, newPos -0x14, f -0x18, to -0x1c, */
    s32 dx;   /* facing -0x1e, dx -0x24, apex -0x28 */
    s16 facing;
    Vec3s *to;
    s32 f;
    Vec3s newPos;
    s32 t;
    RobotStateDesc *desc;
    s32 dz;
    desc = &g_robotStateTable[state];
    if (stateTime > 0x2000)
        stateTime = 0x2000;
    to = &ejectTraj->pts[1];
    dx = ejectStartPos.x - to->x;
    dz = ejectStartPos.z - to->z;
    facing = Math_RadiansToAngle4096((float)atan2(dx, dz)) & 0xfff;
    if (stateTime <= 0x1000) {
        apex = ejectTraj->pts[0].y - ejectStartPos.y;
        t = stateTime;
        newPos.y = ejectStartPos.y;
        f = t * 0x1000 / 0x2000;
        airTime = 0;
    } else {
        apex = ejectTraj->pts[0].y - to->y;
        t = 0x2000 - stateTime;
        newPos.y = to->y;
        f = 0x1000 - t * t / 0x2000;
    }
    /* apex first: its operand is loaded into ecx for the imul (0x45f489), with apex second it is a memory operand */
    newPos.y += (s16)(apex * (-(t * t) / 0x1000 + t * 2) / 0x1000);
    newPos.x = ejectStartPos.x - (dx * f >> 12);
    newPos.z = ejectStartPos.z - (dz * f >> 12);
    SetPosition(&newPos);
    rot.y = facing;
    return stateTime >= 0x2000;
}

/* 0x45f50e - the ground states ROBOT_ST_CTRL_IDLE and ROBOT_ST_WALK: walk, then the context action, falling
 * (ROBOT_ST_FALL), jump (ROBOT_ST_JUMP_START, WOLF_ACT_JUMP_EDGE) or dash (ROBOT_ST_DASH, WOLF_ACT_RUN_HELD); 1 while
 * the state carries on. */
s32 Robot::GroundControlStep()
{
    WalkStep();
    if (TryContextAction(1))
        return 0;
    if (IsFalling()) {
        SetState(ROBOT_ST_FALL);
        return 0;
    }
    if (padBits) {
        if ((padBits & WOLF_ACT_JUMP_EDGE) && CanJump()) {
            SetState(ROBOT_ST_JUMP_START);
            return 0;
        }
        if (padBits & WOLF_ACT_RUN_HELD) {
            SetState(ROBOT_ST_DASH);
            return 0;
        }
    }
    return 1;
}

/* 0x45f5a6 - stand: stick released, walk (decelerate, gravity), ROBOT_ST_FALL when falling. */
/* BYTES(temp): the stick words are cleared through a pointer local, as the original (add eax,0xc8 at 0x45f5b2) */
s32 Robot::StandStep()
{
    s32 *stick; /* the three stick words are cleared through a pointer local (0x45f5b2 add eax,0xc8) */
    stick = &stickX;
    stick[0] = 0;
    stick[1] = 0;
    stick[2] = 0;
    WalkStep();
    if (IsFalling()) {
        SetState(ROBOT_ST_FALL);
        return 0;
    }
    return 1;
}

/* 0x45f602 - turn toward the context target (or keep the facing), ROBOT_ST_FALL when falling. */
s32 Robot::FaceTargetStep()
{
    s16 heading;
    if (ctxAction.target)
        heading = HeadingTo(&ctxAction.target->pos);
    else
        heading = rot.y;
    TurnToHeadingStep(heading);
    if (IsFalling()) {
        SetState(ROBOT_ST_FALL);
        return 0;
    }
    return 1;
}

/* 0x45f66d - whether the pad drives the robot: controlled, game not paused (GF_BIT0), map closed, no debug camera. */
s32 Robot::IsControllable()
{
    return (robotFlags & ROBOT_F_CONTROLLED) && !(g_gameFlags & GF_BIT0) && !Map_IsOpen() && !g_camDebugMode;
}

/* 0x45f6be - freeze the Wolf with the robot as the freezer (MSG_FREEZE); if he accepts, the robot is controlled and
 * driven (ROBOT_F_CONTROLLED | ROBOT_F_DRIVEN) and stops. */
s32 Robot::TakeControl()
{
    if (g_pWolf->HandleMessage(this, MSG_FREEZE, 0)) {
        robotFlags |= ROBOT_F_CONTROLLED;
        robotFlags |= ROBOT_F_DRIVEN;
        StopMotion();
        return 1;
    }
    return 0;
}

/* 0x45f721 - unfreeze the Wolf (MSG_UNFREEZE) and let go: ROBOT_F_CONTROLLED | ROBOT_F_DRIVEN, the suspender and the
 * remote cleared. */
void Robot::ReleaseControl()
{
    g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0);
    ClearRobotFlags(ROBOT_F_CONTROLLED | ROBOT_F_DRIVEN);
    suspender = 0;
    remote = 0;
}

/* 0x45f77a - one frame of the robot (see the file header). */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; pad14 fill gaps */
void Robot::Update()
{
    struct {
        Vec3s smokePos;       /* -0x30  dash smoke source: pos lifted by half the particle size */
        s32 div;              /* -0x28  max(speed, 25) */
        Vec3s waterProbe;     /* -0x24  pos, 30 units up */
        Box *killBox;         /* -0x1c  death zone the robot is in */
        s32 controllable;     /* -0x18  Robot_IsControllable() */
        u16 pad14;            /* -0x14 */
        u16 fallMask;         /* -0x12  state 1: FallStep mask, 7 with riders, else 2 */
        DropMsgArg dropArg;   /* -0x10  message 5 (put down) argument */
        s32 accepted;         /* -0x8   state 0xb: the target's reply to msg 4 (pick up) */
        RobotStateDesc *desc; /* -0x4   g_robotStateTable[state] */
    } w;

    w.desc = &g_robotStateTable[state];
    stateTime += g_dt;
    if (stateTime > 0x1e000)
        stateTime = 0x1e000;
    airTime += g_dt;
    if (airTime > 0x1e000)
        airTime = 0x1e000;
    ClearFlags(ROBOT_F_TOUCHING_MOVABLE | ROBOT_F_ON_OBJECT);
    if (!(w.desc->flags & ROBOT_SF_ACTIVE)) {
        switch (state) {
            case ROBOT_ST_DORMANT:
                airTime = 0;
                if (riderCount)
                    SetState(ROBOT_ST_IDLE);
                break;
            case ROBOT_ST_IDLE:
                Input_Clear(&stickX);
                if (riderCount)
                    w.fallMask = RESOLVE_SLIDE_ALL;
                else
                    w.fallMask = COLL_WALL;
                if ((FallStep(w.fallMask) & 1) && !(robotFlags & ROBOT_F_TOUCHING_MOVABLE) && !riderCount &&
                    !smokeEmitter.base.flags.active)
                    SetState(ROBOT_ST_DORMANT);
                break;
            case ROBOT_ST_CARRIED:
                airTime = 0;
                break;
        }
    } else {
        w.controllable = IsControllable();
        ScanContextActions();
        ReadPad(&g_pad, g_camMode != CAM_LOOK && w.controllable, 0);
        switch (state) {
            case ROBOT_ST_POWER_ON:
                if (StandStep() && AnimFlags(ANIM_F_FINISHED))
                    EnterIdle();
                break;
            case ROBOT_ST_CTRL_IDLE:
                if (AnimId() == AROBOT01_ANIM_STAND1 || AnimId() == AROBOT01_ANIM_STAND2) {
                    if (!stateSoundHandle || !Sound_IsPlaying(StateSound()))
                        stateSoundHandle = Sound_Play(SND_ROBOT_IDLE, this, 0xff,
                                                      SNDF_LOOP | SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 0x1000);
                } else if (stateSoundHandle) {
                    Sound_Stop(StateSound(), this);
                    stateSoundHandle = 0;
                }
                if (GroundControlStep()) {
                    if (stickMag != 0)
                        SetState(ROBOT_ST_WALK);
                    else if (AnimFlags(ANIM_F_FINISHED))
                        NextIdleAnim();
                }
                break;
            case ROBOT_ST_WALK:
                if (!stateSoundHandle || !Sound_IsPlaying(StateSound()))
                    stateSoundHandle =
                        Sound_Play(SND_ROBOT_WALK, this, 0xff, SNDF_LOOP | SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 0x1000);
                if (GroundControlStep() && stickMag == 0)
                    EnterIdle();
                break;
            case ROBOT_ST_DASH:
                if (!stateSoundHandle || !Sound_IsPlaying(StateSound()))
                    stateSoundHandle =
                        Sound_Play(SND_ROBOT_DASH, this, 0xff, SNDF_LOOP | SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 0x1000);
                DashStep();
                if (IsFalling())
                    SetState(ROBOT_ST_FALL);
                else if (stateTime >= 0x400 && !(padBits & WOLF_ACT_RUN_HELD))
                    SetState(ROBOT_ST_WALK);
                break;
            case ROBOT_ST_FALL:
                if (FallStep(COLL_WALL) & 1) {
                    EnterIdle();
                    Sound_Play(SND_ROBOT_JUMP, this, 0xff, SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 0x1000);
                } else
                    TryContextAction(0);
                break;
            case ROBOT_ST_JUMP_START:
                WalkStep();
                if (!TryContextAction(0) && AnimFlags(ANIM_F_FINISHED))
                    SetState(ROBOT_ST_JUMP_RISE);
                break;
            case ROBOT_ST_JUMP_RISE:
                if (JumpRiseStep() & 1) {
                    SetState(ROBOT_ST_FALL);
                    Sound_Play(SND_ROBOT_JUMP, this, 0xff, SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 0x1000);
                } else if (!TryContextAction(0) && stateTime >= 0x4cc)
                    SetState(ROBOT_ST_JUMP_APEX);
                break;
            case ROBOT_ST_JUMP_APEX:
                if (FallStep(COLL_WALL) & 1) {
                    EnterIdle();
                    Sound_Play(SND_ROBOT_JUMP, this, 0xff, SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 0x1000);
                } else if (!TryContextAction(0) && AnimFlags(ANIM_F_FINISHED))
                    SetState(ROBOT_ST_FALL);
                break;
            case ROBOT_ST_PICKUP_REACH:
                if (FaceTargetStep()) {
                    if (ctxAction.action != CTX_PICKUP)
                        EnterIdle();
                    else if (AnimFlags(ANIM_F_FINISHED)) {
                        /* cast kept: the joint travels in the void * argument */
                        w.accepted = ctxAction.target->HandleMessage(this, MSG_PICKUP, (void *)0x13);
                        if (w.accepted)
                            heldObject = ctxAction.target;
                        SetState(ROBOT_ST_PICKUP_END);
                    }
                }
                break;
            case ROBOT_ST_PUTDOWN:
                if (StandStep() && AnimFlags(ANIM_F_FINISHED)) {
                    if (GetDropPoint(&w.dropArg.pos)) {
                        w.dropArg.placed = 1;
                        w.dropArg.flag1 = 0;
                        heldObject->HandleMessage(this, MSG_DROP, &w.dropArg);
                        heldObject = 0;
                    }
                    SetState(ROBOT_ST_PUTDOWN_END);
                }
                break;
            case ROBOT_ST_PICKUP_END:
            case ROBOT_ST_PUTDOWN_END:
            case ROBOT_ST_TRAP_FREED:
            case ROBOT_ST_ACTIVATE_END:
                if (StandStep() && AnimFlags(ANIM_F_FINISHED))
                    EnterIdle();
                break;
            case ROBOT_ST_DIE:
            case ROBOT_ST_DIE_BURN_DROWN:
                if (robotFlags & ROBOT_F_DEATH_FALLS) {
                    Input_Clear(&stickX);
                    FallStep(COLL_WALL);
                } else
                    StopMotion();
                if (AnimFlags(ANIM_F_FINISHED)) {
                    if (remote)
                        remote->HandleMessage(this, MSG_KILL,
                                              (void *)KILL_ZAP); /* cast kept: the cause is a void * argument */
                    else
                        Fade_StartRestart(0x3000);
                    if (robotFlags & ROBOT_F_CONTROLLED)
                        ReleaseControl();
                    if (robotFlags & ROBOT_F_DEATH_FALLS)
                        SetState(ROBOT_ST_IDLE);
                }
                break;
            case ROBOT_ST_KICKED:
                TurnToHeadingStep(ejectFacing);
                if (AnimFlags(ANIM_F_FINISHED))
                    SetState(ROBOT_ST_EJECT_FLIGHT);
                break;
            case ROBOT_ST_EJECT_FLIGHT:
                if (EjectFlightStep()) {
                    Camera_ReleaseScripted(this);
                    SetState(ROBOT_ST_EJECT_FALL);
                } else if ((robotFlags & ROBOT_F_DRIVEN) && this != Camera_GetScriptOwner() && ejectCam)
                    StartCamera(ejectCam->rot[0], ejectCam->rot[1], ejectCam->rot[2], &ejectCam->eye, ejectCam->focal,
                                CAMSCR_BLEND_OUT, 0x1000);
                break;
            case ROBOT_ST_EJECT_FALL:
                Stick_Clear(&stickX);
                if (FallStep(COLL_WALL) & 1)
                    SetStateKeepAnim(ROBOT_ST_EJECT_LAND);
                break;
            case ROBOT_ST_EJECT_LAND:
                Stick_Clear(&stickX);
                WalkStep();
                if (AnimFlags(ANIM_F_FINISHED)) {
                    if (w.controllable)
                        SetState(ROBOT_ST_FALL);
                    else
                        SetState(ROBOT_ST_IDLE);
                }
                break;
            case ROBOT_ST_TRAP_REACH:
                if (FaceTargetStep()) {
                    if (ctxAction.action != CTX_WOLFTRAP)
                        EnterIdle();
                    else if (AnimFlags(ANIM_F_FINISHED)) {
                        trap = ctxAction.target;
                        robotFlags |= ROBOT_F_TRAPPED;
                        ClearFlags(ROBOT_F_STRUGGLE);
                        trap->HandleMessage(this, MSG_USE, 0);
                        SetState(ROBOT_ST_TRAPPED_START);
                    }
                }
                break;
            case ROBOT_ST_TRAPPED_START:
            case ROBOT_ST_TRAPPED:
            case ROBOT_ST_TRAPPED_STRUGGLE:
                GetShadow()->Reproject();
                if (!(robotFlags & ROBOT_F_TRAPPED))
                    SetState(ROBOT_ST_TRAP_FREED);
                else {
                    switch (state) {
                        case ROBOT_ST_TRAPPED_START:
                            if (AnimFlags(ANIM_F_FINISHED))
                                SetState(ROBOT_ST_TRAPPED);
                            break;
                        case ROBOT_ST_TRAPPED:
                            if (robotFlags & ROBOT_F_STRUGGLE)
                                SetState(ROBOT_ST_TRAPPED_STRUGGLE);
                            break;
                        case ROBOT_ST_TRAPPED_STRUGGLE:
                            if (AnimFlags(ANIM_F_FINISHED)) {
                                SetState(ROBOT_ST_TRAPPED);
                                ClearFlags(ROBOT_F_STRUGGLE);
                            }
                            break;
                    }
                }
                break;
            case ROBOT_ST_ACTIVATE:
                if (FaceTargetStep()) {
                    if (ctxAction.action != CTX_ACTIVATE)
                        EnterIdle();
                    else if (AnimFlags(ANIM_F_FINISHED)) {
                        ctxAction.target->HandleMessage(this, MSG_USE, 0);
                        SetState(ROBOT_ST_ACTIVATE_END);
                    }
                }
                break;
            case ROBOT_ST_TIMEKEEPER:
                if (AnimFlags(ANIM_F_FINISHED))
                    EnterIdle();
                break;
        }
    }
    if (heldObject)
        heldObject->SetPosition(&pos);
    if (!(robotFlags & ROBOT_F_DEAD) && !InstFlags(INST_F_ATTACHED)) {
        w.waterProbe = pos;
        w.waterProbe.y -= 0x1e;
        w.killBox = Zones_Get(ZONE_DEATH)->FindContaining(&pos);
        if (w.killBox) {
            SetState(ROBOT_ST_DIE_BURN_DROWN);
            switch (w.killBox->flags) {
                case DZ_37_PROP2:
                case DZ_37_PROP1:
                case DZ_37_BURN:
                    robotFlags |= ROBOT_F_DEAD;
                    GetShadow()->SetVisible(0);
                    break;
                default:
                    robotFlags |= ROBOT_F_DEAD | ROBOT_F_DEATH_FALLS;
            }
        } else if (Zones_Get(ZONE_WATER)->FindContaining(&w.waterProbe)) {
            robotFlags |= ROBOT_F_DEAD | ROBOT_F_DEATH_FALLS;
            SetState(ROBOT_ST_DIE_BURN_DROWN);
        }
    }
    if (Zones_Get(ZONE_SHADOW)->FindContaining(&pos))
        robotFlags |= ROBOT_F_IN_SHADOW;
    else
        ClearFlags(ROBOT_F_IN_SHADOW);
    if ((w.desc->flags & ROBOT_SF_SMOKE) || smokeEmitter.base.flags.active) {
        w.smokePos = pos;
        w.smokePos.y -= (s16)(g_robotDashSmokeParams.sizeStart >> 1);
        if (speed < 0x19)
            w.div = 0x19;
        else
            w.div = speed;
        g_robotDashSmokeParams.spawnInterval = (g_robotDashMoveRec.maxSpeed * 0xa3) / w.div;
        smokeFlickerTimer -= g_dt;
        if (smokeFlickerTimer < 0) {
            robotFlags ^= ROBOT_F_SMOKE_PHASE;
            smokeFlickerTimer = Rand_Range(0x100, 0x400);
            if (!(robotFlags & ROBOT_F_SMOKE_PHASE))
                smokeFlickerTimer >>= 1;
        }
        smokeEmitter.base.Emitter_UpdateDrift(&g_robotDashSmokeParams, &w.smokePos, Facing(),
                                              (w.desc->flags & ROBOT_SF_SMOKE) && (robotFlags & ROBOT_F_SMOKE_PHASE));
    }
    if (robotFlags & ROBOT_F_BURNT)
        SetTint(0, 0x1000, 1);
    else if (robotFlags & ROBOT_F_IN_SHADOW)
        SetTint(0, 0xc00, 1);
    else
        SetTint(0, 0, 0);
    ClearFlags(ROBOT_F_AXIS_LOCK_A | ROBOT_F_AXIS_LOCK_B);
    AdvanceAnim();
}

/* 0x460b93 - vtable +0x10. Returns 1 for most handled messages, 0 for ignored ones, or the answer of a query. */
/* BYTES(slot-scope): msg 4's five locals share one block: attachRot -8, offset -0x10, here -0x18, parent -0x1c, hook -0x1d */
s32 Robot::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    if (msgId < MSG_WOLF_CAUGHT) {
        switch (msgId) {
            case MSG_KILL: /* Sam's kick (CLASSID_SAM) ejects the robot along EJECTTRAJ; anything else knocks it over */
                if (!(robotFlags & ROBOT_F_DEAD)) {
                    if (sender->GetClassId() == CLASSID_SAM && ejectTraj) {
                        if (suspender) {
                            suspender->HandleMessage(this, MSG_FREEZE, 0);
                            robotFlags |= ROBOT_F_CONTROLLED;
                            suspender = 0;
                        }
                        if (robotFlags & ROBOT_F_TRAPPED) {
                            trap->HandleMessage(this, MSG_WOLFTRAP_RESET, 0);
                            ClearFlags(ROBOT_F_TRAPPED | ROBOT_F_STRUGGLE);
                            trap = 0;
                        }
                        StopMotion();
                        ejectStartPos = pos;
                        SetState(ROBOT_ST_KICKED);
                        ejectFacing = HeadingTo(&sender->pos);
                    } else {
                        switch ((s32)arg) { /* cast kept: the kill cause travels in the void * argument */
                            case KILL_GENERIC:
                                if (sender->GetClassId() == CLASSID_FIREBALL)
                                    SetState(ROBOT_ST_DIE_BURN_DROWN);
                                else
                                    SetState(ROBOT_ST_DIE);
                                robotFlags |= ROBOT_F_DEAD | ROBOT_F_BURNT | ROBOT_F_DEATH_FALLS;
                        }
                    }
                }
                return 1;
            case MSG_QUERY_ACTION: /* the Wolf asks whether it may stand on the robot: 2 when he is within -80..+50 of its height */
                if (sender->GetClassId() == CLASSID_WOLF && pos.y <= sender->pos.y + 0x32 &&
                    pos.y >= sender->pos.y - 0x50)
                    return CTX_LIFT;
                break;
            case MSG_PICKUP: { /* attach to a joint (arg) of the sender: hung 0x4b below a Magnet, else turned round */
                ScnObject *parent;
                u8 hook;
                Vec3s here;
                Vec3s offset;
                Vec3s attachRot;
                parent = sender;
                hook = (u8)arg; /* cast kept: MSG_PICKUP passes the joint in the void * argument */
                if (sender->GetClassId() == CLASSID_MAGNET) {
                    here = pos;
                    offset.x = offset.z = 0;
                    offset.y = 0x4b;
                    AttachTo(parent, hook, &offset, 0, 1, 0);
                    SetPosition(&here);
                } else {
                    SetState(ROBOT_ST_CARRIED);
                    attachRot.x = attachRot.z = 0;
                    attachRot.y = 0x800;
                    AttachTo(parent, hook, 0, &attachRot, 1, 0);
                }
                return 1;
            }
            case MSG_DROP: { /* detach, facing away from the sender; put down at arg->pos unless arg->flag1 */
                DropMsgArg *drop;
                drop = (DropMsgArg *)arg; /* cast kept: MSG_DROP passes a DropMsgArg * in the void * */
                Detach();
                rot.y = (sender->Facing() + 0x800) & 0xfff;
                if (!drop->flag1)
                    SetPosition(&drop->pos);
                SetState(ROBOT_ST_IDLE);
                return 1;
            }
            case MSG_CARRY_ANIM:    /* the held-object animation: WOLF_CUE_WALK = RUN2COY, the other cues LINK */
                switch ((s32)arg) { /* cast kept: the carrier's animation cue travels in the void * argument */
                    case WOLF_CUE_IDLE:
                    case WOLF_CUE_JUMP:
                    case WOLF_CUE_GLIDE:
                    case WOLF_CUE_RUN:
                    case WOLF_CUE_5:
                    case WOLF_CUE_6:
                        if (AnimId() != AROBOT01_ANIM_LINK)
                            PlayAnim(AROBOT01_ANIM_LINK, 1, 1);
                        break;
                    case WOLF_CUE_WALK:
                        if (AnimId())
                            PlayAnim(AROBOT01_ANIM_RUN2COY, 1, 1);
                }
                return 1;
            case MSG_QUERY_HELD_ACTION:
                return HELD_THROWABLE;
            case MSG_REMOTE_SWITCH: { /* the remote control: arg 1 switches the robot on and takes control, arg 0 lets go */
                RobotStateDesc *desc;
                desc = &g_robotStateTable[state];
                switch ((s32)arg) { /* cast kept: on / off travels in the void * argument */
                    case 1:
                        if (!(robotFlags & ROBOT_F_DEAD)) {
                            remote = sender;
                            TakeControl();
                            if (robotFlags & ROBOT_F_TRAPPED)
                                SetState(ROBOT_ST_TRAPPED);
                            else if (!(desc->flags & ROBOT_SF_EJECTED)) {
                                if (robotFlags & ROBOT_F_POWERED_ON)
                                    EnterIdle();
                                else {
                                    robotFlags |= ROBOT_F_POWERED_ON;
                                    SetState(ROBOT_ST_POWER_ON);
                                }
                            }
                        }
                        break;
                    case 0:
                        if (robotFlags & ROBOT_F_CONTROLLED)
                            ReleaseControl();
                        if (!(robotFlags & ROBOT_F_DEAD) && !(desc->flags & ROBOT_SF_EJECTED))
                            SetState(ROBOT_ST_IDLE);
                }
                return 1;
            }
            case MSG_FREEZE: /* freeze: suspends control (the Wolf's own freeze also ends DRIVEN); from the Wolf while not
                   * controlled, it releases the suspender instead */
                if (robotFlags & ROBOT_F_CONTROLLED) {
                    ClearFlags(ROBOT_F_CONTROLLED);
                    if (sender->GetClassId() == CLASSID_WOLF) {
                        suspender = 0;
                        ClearFlags(ROBOT_F_DRIVEN);
                    } else
                        suspender = sender;
                    return 1;
                }
                if (sender->GetClassId() == CLASSID_WOLF && suspender &&
                    suspender->HandleMessage(this, MSG_FREEZE, 0)) {
                    robotFlags |= ROBOT_F_CONTROLLED;
                    suspender = 0;
                    return 1;
                }
                break;
            case MSG_UNFREEZE:
                robotFlags |= ROBOT_F_CONTROLLED;
                suspender = 0;
                return 1;
            case MSG_RIDER_ADD:
                return AddMoveModifier(sender);
            case MSG_RIDER_REMOVE:
                RemoveMoveModifier(sender);
                return 1;
            case MSG_TRAP_STATE:
                if (!arg) {
                    ClearFlags(ROBOT_F_TRAPPED | ROBOT_F_STRUGGLE);
                    trap = 0;
                }
                return 1;
            case MSG_STRUGGLE:
                robotFlags |= ROBOT_F_STRUGGLE;
                return 1;
            case MSG_GRABBER_CAMERA:             /* the Wolf's camera, handed to the robot while he drives it */
                return UpdateCamera((Pad *)arg); /* cast kept: the pad travels in the void * argument */
            case MSG_DRAW_PROMPT: /* the HUD prompts, drawn by the Wolf's code with the robot's context block */
                if (robotFlags & ROBOT_F_DRIVEN) {
                    /* cast kept (both): the prompt code is the Wolf's; it reads only the ScnControllable part */
                    ((Wolf *)this)->DrawActionPrompt(&ctxAction, 0, 1);
                    ((Wolf *)this)->Hud_DrawItemPrompt(&itemPromptId, itemPromptId != CLASSID_ROBOT);
                    return 1;
                }
                break;
            case MSG_AXIS_LOCK: /* lock the stick heading to one axis this frame */
                if (!arg)
                    robotFlags |= ROBOT_F_AXIS_LOCK_A;
                else if ((s32)arg == 1) /* cast kept: the axis travels in the void * argument */
                    robotFlags |= ROBOT_F_AXIS_LOCK_B;
                return 1;
            case MSG_QUERY_CONTROLLED:
                if (robotFlags & ROBOT_F_CONTROLLED)
                    return 1;
                break;
            case MSG_MAGNET_QUERY:
                return 1;
            case MSG_LAUNCH:
                SetState(ROBOT_ST_LAUNCHED);
                break;
            case MSG_SEESAW_TOUCH:
            case MSG_LANDED:
                if (!InstFlags(INST_F_ATTACHED) && !(robotFlags & ROBOT_F_DEAD)) {
                    if (robotFlags & ROBOT_F_CONTROLLED) {
                        if (state == ROBOT_ST_LAUNCHED)
                            SetState(ROBOT_ST_FALL);
                    } else if (state != ROBOT_ST_IDLE)
                        SetState(ROBOT_ST_IDLE);
                }
                break;
            case MSG_QUERY_NEAREST_TARGET:
                if (heldObject)
                    return heldObject->HandleMessage(this, msgId, arg);
        }
    } else {
        switch (msgId) {
            case MSG_ROBOT_IS_ACTIVE: { /* whether the robot is in a controllable state */
                RobotStateDesc *desc;
                desc = &g_robotStateTable[state];
                if (desc->flags & ROBOT_SF_ACTIVE)
                    return 1;
                break;
            }
            case MSG_ROBOT_IS_DRIVEN: /* whether it is driven */
                if (robotFlags & ROBOT_F_DRIVEN)
                    return 1;
                break;
            case MSG_ROBOT_IS_EJECTED: /* whether it is being ejected */
                if (state == ROBOT_ST_KICKED || state == ROBOT_ST_EJECT_FLIGHT || state == ROBOT_ST_EJECT_FALL ||
                    state == ROBOT_ST_EJECT_LAND)
                    return 1;
        }
    }
    return 0;
}

/* 0x46155f - vtable +0x08: the animated draw, then the dash smoke while its emitter is active. */
void Robot::Render(Camera *view)
{
    ScnMobile::Render(view);
    if (smokeEmitter.base.flags.active)
        smokeEmitter.base.Emitter_Render(view, 0);
}

/* 0x4615a0 - ROBOT_ST_CTRL_IDLE: its own animation empty-handed, the holding idle STAND2 otherwise, and a random number
 * of loops before the first idle variant. */
void Robot::EnterIdle()
{
    if (!heldObject)
        SetState(ROBOT_ST_CTRL_IDLE);
    else
        SetStateAnim(ROBOT_ST_CTRL_IDLE, AROBOT01_ANIM_STAND2, 1);
    idleLoopCount = Rand_Range(10, 20);
}

/* 0x4615e6 - the idle variation of ROBOT_ST_CTRL_IDLE, empty-handed only. */
void Robot::NextIdleAnim()
{
    if (!heldObject)
        ScnControllable::NextIdleAnim(&g_robotIdleBase, g_robotIdleVariants, 3);
}

/* 0x461611 - the only writer of state: enter a state without touching the animation. Stops the state's loop sound,
 * applies the descriptor's box-collision and shadow bit (ROBOT_SF_CARRIED) and update policy (ROBOT_SF_ALWAYS_UPDATE),
 * and resets the timer. */
void Robot::EnterState(u8 newState)
{
    RobotStateDesc *desc;

    desc = &g_robotStateTable[newState];
    if (stateSoundHandle) {
        StopSound(stateSoundHandle);
        stateSoundHandle = 0;
    }
    if (desc->flags & ROBOT_SF_CARRIED) {
        SetBoxCollide(0);
        shadow.SetVisible(0);
    } else {
        SetBoxCollide(1);
        shadow.SetVisible(1);
    }
    if (desc->flags & ROBOT_SF_ALWAYS_UPDATE)
        SetUpdateMode(SCN_UPD_ALWAYS);
    else
        SetUpdateMode(SCN_UPD_NORMAL);
    state = newState;
    stateTime = 0;
}

/* 0x4618df - enter a state with its own animation (looped per ROBOT_SF_ANIM_LOOP), blended. */
/* BYTES(temp): loop is a named local set by if / else, as the original stores it (0x46190b) */
void Robot::SetState(u8 newState)
{
    s32 loop;
    RobotStateDesc *desc;
    s32 doBlend;

    desc = &g_robotStateTable[newState];
    doBlend = 1;
    if (desc->flags & ROBOT_SF_ANIM_LOOP)
        loop = 1;
    else
        loop = 0;
    PlayAnim(desc->animId, loop, doBlend);
    EnterState(newState);
}

/* 0x46197b - enter a state with a given animation, blended. */
void Robot::SetStateAnim(u8 newState, u16 animId, s32 loop)
{
    PlayAnim(animId, loop, 1);
    EnterState(newState);
}

/* 0x4619dd - enter a state keeping the current animation. */
void Robot::SetStateKeepAnim(u8 newState)
{
    EnterState(newState);
}

/* 0x4619f6 - the state reset of Init and Reset: every flag but ROBOT_F_CONTROLLED, ROBOT_F_DRIVEN and ROBOT_F_POWERED_ON
 * cleared, stick and pad cleared, motion stopped, the timers zeroed, the ground flat, no eject, no context action or
 * trap, the smoke emptied, and the robot camera back to its defaults behind the robot. */
void Robot::ClearState()
{
    ClearFlags(ROBOT_F_TOUCHING_MOVABLE | ROBOT_F_ON_OBJECT | ROBOT_F_TRAPPED | ROBOT_F_STRUGGLE | ROBOT_F_DEAD |
               ROBOT_F_SMOKE_PHASE | ROBOT_F_AXIS_LOCK_A | ROBOT_F_AXIS_LOCK_B | ROBOT_F_IN_SHADOW | ROBOT_F_BURNT |
               ROBOT_F_DEATH_FALLS);
    PadState_Clear(&stickX);
    StopMotion();
    airTime = 0;
    slopeTime = 0;
    stateTime = 0;
    groundNormal.x = 0;
    groundNormal.y = -0x1000;
    groundNormal.z = 0;
    ejectStartPos.x = 0;
    ejectStartPos.y = 0;
    ejectStartPos.z = 0;
    ctxAction.target = 0;
    ctxAction.action = CTX_NONE;
    heldActionType = HELD_NONE;
    ctxPromptExtra = 0;
    itemPromptId = CLASSID_ROBOT;
    itemPromptArg = 0;
    idleLoopCount = 0;
    ejectFacing = 0;
    smokeEmitter.base.Emitter_Reset();
    smokeFlickerTimer = 0;
    trap = 0;
    savedCamRot.x = g_camMinPitch;
    savedCamRot.y = -Facing() & 0xfff;
    savedCamRot.z = 0;
    savedCamDist = g_camModeParams[CAM_ROBOT].dist;
}

/* 0x461ba0 - vtable +0x14, the checkpoint restart: drop what is held where it is (MSG_DROP with flag1, which keeps
 * the position: the pos words are never written), let go of Ralph's control state, no tint, back to the spawn point
 * facing the spawn way, the state reset, and ROBOT_ST_IDLE. */
void Robot::Reset()
{
    DropMsgArg drop;
    Vec3s facing;

    if (heldObject) {
        drop.placed = 1;
        drop.flag1 = 1;
        heldObject->HandleMessage(this, MSG_DROP, &drop);
        heldObject = 0;
    }
    ClearFlags(ROBOT_F_CONTROLLED | ROBOT_F_DRIVEN | ROBOT_F_POWERED_ON);
    suspender = 0;
    remote = 0;
    tintColor = 0;
    tintAmount = 0;
    SetTinted(0);
    SetPosition(&spawnPos);
    facing.x = 0;
    facing.y = spawnFacing;
    facing.z = 0;
    rot = facing;
    ClearState();
    SetState(ROBOT_ST_IDLE);
}

/* 0x461ce1 - vtable +0x00 (Robot_Init): the EJECTCAM and EJECTTRAJ properties, weight 40, the state reset, the put-down
 * offset (0,-2,0), nothing held, no suspender or remote, no riders, never distance-culled, on the ground, that spot and
 * facing saved for Reset, ROBOT_ST_IDLE, and the current animation restarted without loop or blend. */
void Robot::PostLoadInit()
{
    u16 *rec;

    rec = record;
    ejectCam = Scn_GetPropCamera(rec, 0);
    ejectTraj = Scn_GetPropTrajectory(rec, 4);
    weight = 0x28;
    robotFlags = 0;
    ClearState();
    dropOffset.x = 0;
    dropOffset.y = -2;
    dropOffset.z = 0;
    heldObject = 0;
    suspender = 0;
    remote = 0;
    stateSoundHandle = 0;
    riderCount = 0;
    SetNoDistCull(1);
    SnapToGround(1);
    spawnPos = pos;
    spawnFacing = Facing();
    SetState(ROBOT_ST_IDLE);
    PlayAnim(AnimId(), 0, 0);
}

/* 0x461e6b - the class factory for CLASSID 101 "Robot": new Robot (the base vtables in turn, the smoke emitter's
 * constructor, the Robot vtable), then ScnMobile's vtable +0x20 Init. */
ScnObject *Robot_Create(void *record)
{
    Robot *object = new Robot;
    object = (Robot *)object->Init(record, 0); /* cast kept: Init returns the object as its ScnBody base */
    return object;
}
