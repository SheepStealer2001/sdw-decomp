/*
 * LazerRobot (class 149, CLASSID_LAZERROBOT, vtable 0x576570, sizeof 0xc0), SheepD3D.exe 0x4d11e0-0x4d1af5: the
 * flying saucer of Planet X. It sits parked on its placed spot with a low hum (sound 0xdc, animation 0) until a
 * switch sends it MSG_SWITCH_ON; then it swaps to the loud hum (0xdb) and animation 1 and flies its TRAJ waypoint
 * path at SPEED for ever, turning toward each waypoint at most 50 units of facing (0x32 of 4096) per step. While it
 * flies it rebuilds a kill volume around itself every frame - 240 units either side on both horizontal axes, 400
 * units above its own position and, underneath, the ground height below it - and sends the Wolf MSG_KILL when his
 * position falls inside it. The box reaches down to the floor, so the saucer only has to pass overhead. A second
 * MSG_SWITCH_ON parks it again.
 * Designer properties (lazerRobotProps): SPEED +0, TRAJ +4.
 *
 * The inline helpers have no bodies of their own in the exe, so their names are not recovered; each is there because
 * its expansion gives the original's shapes: SetUpdateMode a 4-way jump table on a constant (ScnUpdateMode; table at
 * 0x4d13ad), PlayAnim an option word in a stack slot, PropU32 its offset argument in a temp, Facing / SetFacing /
 * Voice / PointIndex a 2-byte temp per use, Box_ContainsPoint its two arguments and its value in temps (0x4d1612).
 * A shape that reproduces the bytes is a representation, not proof that the original source read this way.
 */

#define SDW_MEMBERS_ScnObject            \
    static void *operator new(u32 size); \
    void SetUpdateMode(u8 mode);         \
    void SetFacing(s16 f); /* inline: its argument is a 2-byte temp */

#define SDW_MEMBERS_LazerRobot             \
    u16 Voice()                            \
    {                                      \
        return voice;                      \
    } /* inline: a 2-byte temp per read */ \
    s16 PointIndex()                       \
    {                                      \
        return follower.pointIndex;        \
    } /* inline: a 2-byte temp per read */
#include "sdw_types.h"
#include "sdw_enums.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *inst, Animator *animator, u16 id, u32 opts); /* 0x550196 (used by the PlayAnim inline) */
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8
#define SDW_INLINE_SCNOBJECT_FACING 1
#define SDW_INLINE_SCNOBJECT_SETFACING_S16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_FACING
#undef SDW_INLINE_SCNOBJECT_SETFACING_S16
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#include "../engine/property_math.h"

#include "../engine/scn_tools.h"
#include "../engine/sound_mgr.h"
u16 Sound_Play(u16 soundId, void *owner, u16 volume, u8 flags, s32 rate); /* 0x5491b8 */
#include "../sdk/crt.h"
extern "C" s16 Math_RadiansToAngle4096(float radians); /* 0x5269ce */

extern Wolf *g_pWolf; /* 0x6cf310 */

/* A designer property: the dword at record + 0x14 + off (lazerRobotProps). Inline: the offset is a stack temp. */
#define SDW_INLINE_FREE_PROPU32_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_PROPU32_VOID_U32

/* Whether p lies inside box (both faces inclusive): its two parameters and its value are stack temps. */
#define SDW_INLINE_FREE_BOX_CONTAINSPOINT_COLLBOX_VEC3S 1
#include "../engine/coll_box_inlines.h"
#undef SDW_INLINE_FREE_BOX_CONTAINSPOINT_COLLBOX_VEC3S

/* 0x4d11e0 - vtable +0x00: the two properties into the trajectory follower, parked state, the quiet hum, the placed
 * position kept for Reset, and update mode 1 (always update - the saucer keeps flying off camera). */
void LazerRobot::PostLoadInit()
{
    u16 *rec;
    rec = record;
    speed = (s16)PropU32(rec, 0);
    traj = Scn_GetPropTrajectory(rec, 4);
    TrajFollower_Init(&follower, traj, speed, 0x800, 1, 0, 0x32);
    active = 0;
    activator = 0;
    PlayAnim(AGOLGO01_ANIM_STAND, 1, 0);
    homePos = pos;
    voice = 0;
    voice = Sound_Play(SND_LAZERROBOT_VOICE_A, this, 0xff, SNDF_LOOP | SNDF_POSITIONAL, 0x1000);
    pulseLatch = 0;
    SetUpdateMode(SCN_UPD_ALWAYS);
}

/* 0x4d13bd - vtable +0x04: while active, fly the path and test the kill volume; the animation always advances. */
void LazerRobot::Update()
{
    switch (active) {
        case 1:
            StepPath();
            CheckWolfKill();
            break;
    }
    AdvanceAnim();
}

/* 0x4d13f3 - vtable +0x10. MSG_SWITCH_ON toggles the saucer between parked and flying, but only once per switch
 * press: pulseLatch blocks every further ON until MSG_SWITCH_OFF clears it. The sender is remembered so that Reset
 * can tell it (MSG_BUTTON_LOCK, 0x4d) that the robot has gone home. */
s32 LazerRobot::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    switch (msgId) {
        case MSG_SWITCH_ON:
            if (!pulseLatch) {
                if (active != 1) {
                    active = 1;
                    activator = sender;
                    PlayAnim(AGOLGO01_ANIM_STANDLZR, 1, 0);
                    Sound_Stop(Voice(), this);
                    voice = Sound_Play(SND_LAZERROBOT_VOICE_B, this, 0xff, SNDF_LOOP | SNDF_POSITIONAL, 0x1000);
                } else {
                    active = 0;
                    activator = sender;
                    PlayAnim(AGOLGO01_ANIM_STAND, 1, 0);
                    Sound_Stop(Voice(), this);
                    voice = Sound_Play(SND_LAZERROBOT_VOICE_A, this, 0xff, SNDF_LOOP | SNDF_POSITIONAL, 0x1000);
                }
                pulseLatch = 1;
            }
            break;
        case MSG_SWITCH_OFF:
            if (pulseLatch)
                pulseLatch = 0;
            break;
    }
    return 0;
}

/* 0x4d158e LazerRobot_CheckWolfKill. The kill volume: 240 units either side on x and z, 400 above the
 * saucer, and down to the ground height under it (the vertical axis points DOWN, so max.y is the floor). The Wolf
 * is killed on his position alone, not his box - and the box is only rebuilt while the saucer is active, so a
 * parked saucer keeps the last one it built. */
void LazerRobot::CheckWolfKill()
{
    killBox.min.x = pos.x - 240;
    killBox.min.y = pos.y - 400;
    killBox.min.z = pos.z - 240;
    killBox.max.x = pos.x + 240;
    killBox.max.y = QueryGroundY(&pos, 1);
    killBox.max.z = pos.z + 240;
    if (Box_ContainsPoint(&killBox, &g_pWolf->pos))
        /* cast kept: MSG_KILL's arg is the WolfKillType, a number in the void * */
        g_pWolf->HandleMessage(this, MSG_KILL, (void *)KILL_GENERIC);
}

/* 0x4d16c4 LazerRobot_StepPath: one follower step, scaled by the frame time, turned toward the waypoint
 * it is steering to, and applied. The waypoint is read straight out of the trajectory resource (a u16 count then
 * the Vec3s points). */
void LazerRobot::StepPath()
{
    TrajFollower_Step(&follower, &vel, &pathHeading);
    Vec3s_ScaleByDt(&vel, &step);
    TurnToward(&traj->pts[PointIndex()]);
    Translate(&step);
}

/* 0x4d174b LazerRobot_TurnToward: face the target, at most 0x32 of 4096 per step. The wanted heading is
 * the horizontal atan2 plus half a turn; if the saucer is more than a quarter turn away (0x800) it turns through
 * the 0/4096 wrap, otherwise it closes the gap directly. */
void LazerRobot::TurnToward(Vec3s *target)
{
    heading = (s16)((Math_RadiansToAngle4096(
                         (float)atan2((double)target->x - (double)pos.x, (double)target->z - (double)pos.z)) +
                     0x800) &
                    0xfff);
    if ((Facing() - heading >= 0 ? Facing() - heading : -(Facing() - heading)) > 0x800) {
        if (Facing() > heading)
            heading = (s16)(0x1000 - Facing() + heading);
        else
            heading = (s16)(0x1000 - heading + Facing());
        if (heading > 0x32)
            heading = 0x32;
        SetFacing(heading);
    } else {
        if (Facing() > heading) {
            if (Facing() - heading > 0x32)
                heading = (s16)(Facing() - 0x32);
        } else {
            if (heading - Facing() > 0x32)
                heading = (s16)(Facing() + 0x32);
        }
        SetFacing(heading);
    }
}

/* 0x4d1999 - vtable +0x14: back to the placed spot, parked, the follower restarted at waypoint 0, the switch told
 * (MSG_BUTTON_LOCK, 0x4d), the hum stopped and both latches cleared. */
void LazerRobot::Reset()
{
    SetPosition(&homePos);
    active = 0;
    PlayAnim(AGOLGO01_ANIM_STAND, 1, 0);
    TrajFollower_Init(&follower, traj, speed, 0x800, 1, 0, 0x32);
    if (activator)
        activator->HandleMessage(this, MSG_BUTTON_LOCK, 0);
    Sound_Stop(Voice(), this);
    pulseLatch = 0;
    voice = 0;
}

/* 0x4d1a8f - the class factory for CLASSID 149 "LazerRobot": new LazerRobot (the base vtables in turn, then
 * LazerRobot's), then ScnBody_Init(record, 0) through vtable slot +0x20. */
ScnObject *LazerRobot_Create(void *record)
{
    LazerRobot *obj = new LazerRobot;
    obj = (LazerRobot *)obj->Init(record, 0); /* cast kept: Init returns the object as a ScnBody * */
    return obj;
}
