/*
 * T094 - the original object Wolf_Move.cpp (guessed name), one file.
 * .text 0x488be0-0x48d3b1 (Wolf_GetStickHeading .. Wolf_MoveWithPlatform: the movement steps and the per-state bodies),
 * .rdata 0x575920-0x575930 (g_wolfClimbDirTable); no .data, no .bss.
 * Wolf_Create and the Wolf's data before it belong to T093 (src/game/wolf.cpp), the code after it (0x48d3c0-) to T095
 * (src/game/wolf_misc.cpp). Uses the shared src/game/wolf.h.
 *
 * Wolf (Ralph), SheepD3D.exe 0x47d310-0x4935f6 by address (tools/find_library_ranges.py; the ScnControllable functions
 * after it, src/game/scn_controllable.cpp, may be its tail).
 *
 * Every step picks its movement record the same way: bank = g_wolfMoveBank0[mode], descriptor = bank->states[state],
 * record = bank->profiles[surface][descriptor->profile], and builds velocity / rotation / displacement in the shared
 * scratch g_sharedScratch 0x6d5468, reached through a pointer local (as a PSX original reaches its scratchpad), before
 * handing them to Wolf_ApplyMove or Wolf_MoveWithPlatform. Each step has its own view of that block
 * (WolfMoveScratch, WolfClimbScratch, WolfSwimScratch, WolfFloatScratch, WolfArcScratch, WolfHoldScratch).
 * The per-state bodies run one movement step, then the state changes in priority order (the context action, falling,
 * sliding, jumping, running, sneaking, pushing), returning 1 when the state carries on and 0 when it changed. padBits
 * (+0xd4) as read here: 1 jump, 4 run (GroundCommon enters the run state 0x22 on it; inferred), 0x10 sneak, 0x40 action.
 *
 * Match devices, not claims about the source text: the `work` structs pin the original stack offsets of locals, and
 * several locals are named for their stack slot (under /Od a local's slot follows from a hash of its name,
 * tools/vc6_locals.py: modeBank, descriptor, facingTarget, fallTime, oldPos, events ...). The inline helpers in wolf.h
 * are there for their expansions. Angles: 4096 per turn. Speeds: units per second. Vertical points down.
 */
/* BYTES: flow, inline, slot-group, slot-name, temp, view. */
/* BYTES(view): the shared scratch viewed as this step's record through a pointer local, as the original reaches it */
#include "sdw_enums.h"
#include "wolf.h"

/* 0x575920 - climb-box direction (flags bits 27-30) -> heading / 0x200 */
extern const u8 g_wolfClimbDirTable[] = {0, 2, 6, 0, 0, 1, 7, 0, 4, 3, 5, 0, 0, 0, 0, 0};

/* 0x488be0 - the vtable +0x24 override. Two flag bits lock the stick heading to one axis: 0x200000 to 90 or 270 degrees
 * (0x400 / 0xc00), otherwise 0x400000 to 0 or 180 (0 / 0x800), each by rounding to the nearer of the two; if both are
 * set, 0x200000 wins. What sets them in game is not traced yet. */
s16 Wolf::GetStickHeading(s16 fallback)
{
    s16 heading = ScnControllable::GetStickHeading(fallback);
    if (flags & (WOLF_FB_AXIS_LOCK_A | WOLF_FB_AXIS_LOCK_B)) {
        if (flags & WOLF_FB_AXIS_LOCK_A) {
            heading = (heading + 2048) & 4095;
            heading = (heading - (heading & 2047) - 1024) & 4095;
        } else {
            heading = (heading + 1024) & 4095;
            heading = heading - (heading & 2047);
        }
    }
    return heading;
}

/* 0x488c86 - keep pos + delta inside box by shortening delta: axes & 1 clamps x and z to both faces, axes & 2 the
 * vertical to box.min, axes & 4 to box.max (the rocket flight in FLYBOX). */
/* BYTES(slot-group): locals grouped in work only to pin the original frame offsets */
void Wolf::ClampDeltaToBox(Box *box, Vec3s *delta, u32 axes)
{
    struct {
        s32 y, x, z;
    } work;
    if (axes & AXIS_X) {
        work.x = delta->x + pos.x;
        if (work.x < box->min[0])
            work.x = box->min[0];
        else if (work.x > box->max[0])
            work.x = box->max[0];
        delta->x = work.x - pos.x;
        work.z = delta->z + pos.z;
        if (work.z < box->min[2])
            work.z = box->min[2];
        else if (work.z > box->max[2])
            work.z = box->max[2];
        delta->z = work.z - pos.z;
    }
    if (axes & (AXIS_Y_MIN | AXIS_Y_MAX)) {
        work.y = delta->y + pos.y;
        if (work.y < box->min[1]) {
            if (axes & AXIS_Y_MIN)
                work.y = box->min[1];
        } else if (work.y > box->max[1]) {
            if (axes & AXIS_Y_MAX)
                work.y = box->max[1];
        }
        delta->y = work.y - pos.y;
    }
}

/* 0x488dbb - full record speed straight along the facing: motion and wanted heading = facing, pitch/roll zeroed. */
void Wolf::SetForwardVelocityMax(Vec3s *outVel, Vec3s *rot, const MoveRecord *rec)
{
    rot->x = 0;
    rot->z = 0;
    moveDir = rot->y;
    wantedDir = rot->y;
    bounceSpeed = 0;
    speed = rec->maxSpeed;
    Vec_FromPolar(outVel, speed, moveDir);
    outVel->y = 0;
    velocity = *outVel;
}

/* 0x488e5a - a forced forward leap at the record's full speed: for the first jumpTicks of the state the vertical speed
 * follows the analytic jump curve (air time held at 0), afterwards gravity at the surface rate up to 2000. */
/* BYTES(slot-group): locals grouped in work only to pin the original frame offsets */
u32 Wolf::LeapStep(s32 jumpHeight, s32 jumpTicks)
{
    struct {
        WolfMoveScratch *m;
        const MoveRecord *rec;
        WolfStateDesc *desc;
        WolfMoveBank *bank;
        s16 *tuning;
    } work;
    work.bank = &g_wolfMoveBank0[mode];
    work.desc = &work.bank->states[state];
    work.tuning = work.bank->surfaceTuning[surface];
    work.rec = &work.bank->profiles[surface][work.desc->profile];
    work.m = (WolfMoveScratch *)g_sharedScratch; /* cast kept: the shared scratch block is raw bytes */
    work.m->rot = rot;
    SetForwardVelocityMax(&work.m->vel, &work.m->rot, work.rec);
    if (stateTime <= jumpTicks) {
        airTime = 0;
        Wolf_JumpVelocity(&work.m->vel, jumpHeight, jumpTicks, stateTime);
    } else
        ApplyGravity(&work.m->vel, work.tuning[8], 2000);
    Vec3s_ScaleByDt(&work.m->vel, &work.m->step);
    return ApplyMove(&work.m->step, &work.m->rot, COLL_WALL, &work.m->vel);
}

/* 0x488f81 - move toward explicit targets instead of the stick: facing turns toward facingTarget, motion toward
 * headingTarget (only while moving), speed approaches targetSpeed; then slope/gravity, integrate and move.
 * Returns Wolf_SurfaceVelocity's result (on a slope, velocity reversed). */
/* BYTES(slot-group): locals grouped in work only to pin the original frame offsets */
s32 Wolf::MoveStepDirected(s16 facingTarget, s16 headingTarget, s32 targetSpeed)
{
    struct {
        s16 unused, facing;
        s32 result;
        WolfMoveScratch *m;
        const MoveRecord *rec;
        WolfStateDesc *desc;
        WolfMoveBank *bank;
    } work;
    work.m = (WolfMoveScratch *)g_sharedScratch; /* cast kept: the shared scratch block is raw bytes */
    work.bank = &g_wolfMoveBank0[mode];
    work.desc = &work.bank->states[state];
    work.rec = &work.bank->profiles[surface][work.desc->profile];
    work.facing = rot.y;
    work.m->rot.y = Math_ApproachAngle(work.facing, facingTarget, &facingAngVel, work.rec->maxFacingTurnRate,
                                       work.rec->facingTurnAcceleration, work.rec->travelTurnAcceleration, 1);
    work.m->rot.x = 0;
    work.m->rot.z = 0;
    wantedDir = headingTarget;
    bounceSpeed = 0;
    if (speed != 0)
        headingTarget = Math_ApproachAngle(moveDir, headingTarget, &moveDirAngVel, work.rec->maxTravelTurnRate,
                                           work.rec->travelTurnAcceleration, work.rec->travelTurnAcceleration, 1);
    speed = (s16)Math_ApproachLinear(speed, targetSpeed, work.rec->maxSpeed, work.rec->acceleration,
                                     work.rec->deceleration);
    Vec_FromPolar(&work.m->vel, speed, headingTarget);
    work.m->vel.y = 0;
    work.result = Wolf_SurfaceVelocity(&work.m->vel, (flags & WOLF_FB_IN_SLIDE_ZONE) || (flags & WOLF_FB_FORCE_SLIDE),
                                       SurfaceTuning());
    Vec3s_ScaleByDt(&work.m->vel, &work.m->step);
    moveDir = headingTarget;
    velocity = work.m->vel;
    ApplyMove(&work.m->step, &work.m->rot, RESOLVE_SLIDE_ALL, &work.m->vel);
    return work.result;
}

/* 0x4891bb - gravity: vertical speed += gravity * airTime >> 12, capped at terminal. Unlike Wolf_SurfaceVelocity there
 * is no minimum step, and the addition goes through a 16-bit truncation of the increment. */
void Wolf::ApplyGravity(Vec3s *vel, s32 gravity, s32 terminal)
{
    gravity = (gravity * airTime) >> 12;
    if (vel->y + gravity > terminal)
        vel->y = terminal;
    else
        vel->y += (s16)gravity;
}

/* 0x48920b - the vertical speed of a jump of the given height lasting `ticks`, at time t (clamped to ticks): the
 * derivative of a parabola, 2 * height * (ticks - t) / ticks^2 in 4.12. y is SUBTRACTED (the vertical axis points
 * down). */
void Wolf_JumpVelocity(Vec3s *vel, s32 height, s32 ticks, s32 t)
{
    if (t > ticks)
        t = ticks;
    vel->y -= (s16)((height * 2 * (ticks - t)) / ((ticks * ticks) >> 12));
}

/* 0x48924c - commit a step: take the new rotation, then move by step (with the carrier's motion) and collide. */
/* BYTES(slot-group): locals grouped in work only to pin the original frame offsets */
u32 Wolf::ApplyMove(Vec3s *step, Vec3s *newRot, s16 collFlags, Vec3s *vel)
{
    struct {
        u32 result;
        ContactInfo contact;
    } work;
    rot = *newRot;
    work.result = MoveWithPlatform(step, &work.contact, collFlags, vel);
    return work.result;
}

/* 0x48928f - the ordinary stick-driven step. Mobile_Steer from the state's record (a sharp turn sets fxFlags 0x1000);
 * slope and gravity; with faceMotion the body turns toward the resulting velocity instead of the stick and the looping
 * sound 0xec plays. Collision flags 7 while anything moves or the stick is held, else 0. Returns
 * Wolf_SurfaceVelocity's result. */
/* BYTES(slot-group): locals grouped in work only to pin the original frame offsets */
s32 Wolf::MoveStep(s32 faceMotion)
{
    struct {
        s32 result;
        WolfMoveScratch *m;
        const MoveRecord *rec;
        s16 unused1, heading;
        WolfStateDesc *desc;
        s16 unused2, collFlags;
        WolfMoveBank *bank;
    } work;
    work.bank = &g_wolfMoveBank0[mode];
    work.desc = &work.bank->states[state];
    work.rec = &work.bank->profiles[surface][work.desc->profile];
    work.m = (WolfMoveScratch *)g_sharedScratch; /* cast kept: the shared scratch block is raw bytes */
    work.m->rot = rot;
    if (Mobile_Steer(&work.m->vel, &work.m->rot, work.rec, faceMotion))
        fxFlags |= WOLF_FA_REQ_TURN;
    work.result = Wolf_SurfaceVelocity(&work.m->vel, (flags & WOLF_FB_IN_SLIDE_ZONE) || (flags & WOLF_FB_FORCE_SLIDE),
                                       SurfaceTuning());
    if (faceMotion) {
        work.heading =
            (Math_RadiansToAngle4096((float)atan2((double)work.m->vel.x, (double)work.m->vel.z)) + 2048) & 4095;
        work.m->rot.y = Math_ApproachAngle(work.m->rot.y, work.heading, &facingAngVel, work.rec->maxFacingTurnRate,
                                           work.rec->facingTurnAcceleration, work.rec->travelTurnAcceleration, 1);
        PlayLoopSound(SND_SCOGLISS);
    }
    Vec3s_ScaleByDt(&work.m->vel, &work.m->step);
    if ((stickX | stickY | work.m->step.x | work.m->step.z) == 0)
        work.collFlags = 0;
    else
        work.collFlags = RESOLVE_SLIDE_ALL;
    ApplyMove(&work.m->step, &work.m->rot, work.collFlags, &work.m->vel);
    return work.result;
}

/* 0x4894b4 - jump ascent: the stick steers as on the ground (air time held at 0), the vertical speed follows the jump
 * curve for the state's time. */
/* BYTES(slot-group): locals grouped in work only to pin the original frame offsets */
u32 Wolf::JumpAscendStep(s32 height, s32 ticks)
{
    struct {
        WolfMoveScratch *m;
        const MoveRecord *rec;
        WolfStateDesc *desc;
        WolfMoveBank *bank;
    } work;
    work.bank = &g_wolfMoveBank0[mode];
    work.desc = &work.bank->states[state];
    work.rec = &work.bank->profiles[surface][work.desc->profile];
    work.m = (WolfMoveScratch *)g_sharedScratch; /* cast kept: the shared scratch block is raw bytes */
    work.m->rot = rot;
    airTime = 0;
    Mobile_Steer(&work.m->vel, &work.m->rot, work.rec, 0);
    Wolf_JumpVelocity(&work.m->vel, height, ticks, stateTime);
    Vec3s_ScaleByDt(&work.m->vel, &work.m->step);
    return ApplyMove(&work.m->step, &work.m->rot, COLL_WALL, &work.m->vel);
}

/* 0x48959f - falling: the stick steers, gravity at the surface rate up to 2000 (slowFall: a quarter of it, up to 500);
 * the displacement is cut so the vertical never goes below yLimit. Returns the collision flags (bit 0 landed). */
/* BYTES(slot-group): locals grouped in work only to pin the original frame offsets */
u32 Wolf::FallStep(s16 yLimit, s32 slowFall)
{
    struct {
        WolfMoveScratch *m;
        const MoveRecord *rec;
        WolfStateDesc *desc;
        WolfMoveBank *bank;
        s16 *tuning;
    } work;
    work.bank = &g_wolfMoveBank0[mode];
    work.tuning = work.bank->surfaceTuning[surface];
    work.desc = &work.bank->states[state];
    work.rec = &work.bank->profiles[surface][work.desc->profile];
    work.m = (WolfMoveScratch *)g_sharedScratch; /* cast kept: the shared scratch block is raw bytes */
    work.m->rot = rot;
    Mobile_Steer(&work.m->vel, &work.m->rot, work.rec, 0);
    if (slowFall)
        ApplyGravity(&work.m->vel, work.tuning[8] / 4, 500);
    else
        ApplyGravity(&work.m->vel, work.tuning[8], 2000);
    Vec3s_ScaleByDt(&work.m->vel, &work.m->step);
    if (pos.y + work.m->step.y > yLimit)
        work.m->step.y = yLimit - pos.y;
    return ApplyMove(&work.m->step, &work.m->rot, COLL_WALL, &work.m->vel);
}

/* 0x4896e3 - walk to a point. WallAvoid_ComputeSteer gives the heading to the target (turned 0x300 off a wall it is
 * sliding along) and the x / z distance to it; the body turns toward the given facing (useFacing) or that heading.
 * Once one frame at the record's full speed (maxSpeed * g_dt / 4096) would be longer than the distance, the step is the
 * exact x / z remainder to the target (plus the vertical part of Wolf_SurfaceVelocity), motion stops, and it returns
 * 1. The step still goes through collision (mask 7). */
/* BYTES(slot-group): locals grouped in work only to pin the original frame offsets */
s32 Wolf::MoveTowardPoint(Vec3s *target, s16 facing, s32 useFacing)
{
    struct {
        s16 frameStep, targetHeading;
        s32 maxSpeed;
        u32 moveResult;
        WolfMoveScratch *s;
        const MoveRecord *rec;
        s32 arrived;
        s16 distError, steerHeading;
        ContactInfo contact;
        WolfStateDesc *desc;
        WolfMoveBank *bank;
    } work;
    work.s = (WolfMoveScratch *)g_sharedScratch; /* cast kept: the shared scratch block is raw bytes */
    work.bank = &g_wolfMoveBank0[mode];
    work.desc = &work.bank->states[state];
    work.rec = &work.bank->profiles[surface][work.desc->profile];
    /* cast kept (both calls): ComputeSteer writes its headings through u16 *; this step reads them as s16 (movsx) */
    if (useFacing)
        wallAvoid.ComputeSteer(&pos, moveDir, target, 0, (u16 *)&work.steerHeading, (u16 *)&work.targetHeading,
                               &work.distError);
    else
        /* cast kept: ComputeSteer writes u16 headings into these s16 fields */
        wallAvoid.ComputeSteer(&pos, Facing(), target, 0, (u16 *)&work.steerHeading, (u16 *)&work.targetHeading,
                               &work.distError);
    if (useFacing)
        work.s->rot.y = Math_ApproachAngle(Facing(), facing, &facingAngVel, work.rec->maxFacingTurnRate,
                                           work.rec->facingTurnAcceleration, work.rec->travelTurnAcceleration, 1);
    else
        work.s->rot.y = Math_ApproachAngle(Facing(), work.steerHeading, &facingAngVel, work.rec->maxFacingTurnRate,
                                           work.rec->facingTurnAcceleration, work.rec->travelTurnAcceleration, 1);
    work.s->rot.x = 0;
    work.s->rot.z = 0;
    wantedDir = work.steerHeading;
    bounceSpeed = 0;
    work.maxSpeed = work.rec->maxSpeed;
    work.frameStep = (s16)((work.maxSpeed * g_dt) / 4096);
    if (work.frameStep > work.distError) {
        work.s->vel.x = work.s->vel.y = work.s->vel.z = 0;
        Wolf_SurfaceVelocity(&work.s->vel, (flags & WOLF_FB_IN_SLIDE_ZONE) || (flags & WOLF_FB_FORCE_SLIDE),
                             g_wolfMoveBank0[mode].surfaceTuning[surface]);
        Vec3s_ScaleByDt(&work.s->vel, &work.s->step);
        StopMotion();
        work.s->step.x = target->x - pos.x;
        work.s->step.z = target->z - pos.z;
        work.arrived = 1;
    } else {
        if (speed == 0)
            moveDirAngVel = 0;
        else
            work.steerHeading =
                Math_ApproachAngle(moveDir, work.steerHeading, &moveDirAngVel, work.rec->maxTravelTurnRate,
                                   work.rec->travelTurnAcceleration, work.rec->travelTurnAcceleration, 1);
        speed = (s16)Math_ApproachLinear(speed, work.maxSpeed, work.rec->maxSpeed, work.rec->acceleration,
                                         work.rec->deceleration);
        Vec_FromPolar(&work.s->vel, speed, work.steerHeading);
        work.s->vel.y = 0;
        Wolf_SurfaceVelocity(&work.s->vel, (flags & WOLF_FB_IN_SLIDE_ZONE) || (flags & WOLF_FB_FORCE_SLIDE),
                             g_wolfMoveBank0[mode].surfaceTuning[surface]);
        moveDir = work.steerHeading;
        velocity = work.s->vel;
        Vec3s_ScaleByDt(&work.s->vel, &work.s->step);
        work.arrived = 0;
    }
    rot = work.s->rot;
    work.moveResult = MoveWithPlatform(&work.s->step, &work.contact, RESOLVE_SLIDE_ALL, &work.s->vel);
    wallAvoid.UpdateFromContact(&work.contact, (u16)work.moveResult);
    return work.arrived;
}

/* 0x489b24 - move with the stick while the body keeps the given facing. */
/* BYTES(slot-group): locals grouped in work only to pin the original frame offsets */
s32 Wolf::MoveStepFixedFacing(s16 facing)
{
    struct {
        const MoveRecord *rec;
        WolfStateDesc *desc;
        WolfMoveBank *bank;
    } work;
    work.bank = &g_wolfMoveBank0[mode];
    work.desc = &work.bank->states[state];
    work.rec = &work.bank->profiles[surface][work.desc->profile];
    return MoveStepDirected(facing, GetStickHeading(moveDir), (work.rec->maxSpeed * stickMag) >> 8);
}

/* 0x489bb5 - strafe: stickX > 0 moves at facing - 0x400, stickX < 0 at facing + 0x400, at maxSpeed * |stickX| >> 8;
 * the body keeps the given facing. While still moving in another direction it brakes to 0 along the current motion
 * heading first. With the stick centred the heading is facing - 0x400 unless moveDir is exactly facing - 0x400: the
 * test compares |moveDir - (facing + 0x400)| with |(facing - 0x400) - (facing + 0x400)|, which is always 2048
 * (probably meant to pick the side nearer the current motion). */
/* BYTES(slot-group): locals grouped in work only to pin the original frame offsets */
s32 Wolf::SidestepStep(s16 facing)
{
    struct {
        s32 targetSpeed;
        const MoveRecord *rec;
        s16 unused, heading;
        WolfStateDesc *desc;
        WolfMoveBank *bank;
    } work;
    work.bank = &g_wolfMoveBank0[mode];
    work.desc = &work.bank->states[state];
    work.rec = &work.bank->profiles[surface][work.desc->profile];
    if (stickX == 0) {
        work.targetSpeed = 0;
        work.heading = (facing + 1024) & 4095;
        if (SDW_ABS(SDW_ANGLE_DIFF(moveDir, work.heading)) < SDW_ABS(SDW_ANGLE_DIFF(facing - 1024, work.heading)))
            work.heading = (facing - 1024) & 4095;
    } else if (stickX > 0) {
        work.targetSpeed = (work.rec->maxSpeed * stickX) >> 8;
        work.heading = (facing - 1024) & 4095;
    } else {
        work.targetSpeed = (-work.rec->maxSpeed * stickX) >> 8;
        work.heading = (facing + 1024) & 4095;
    }
    if (speed != 0 && moveDir != work.heading) {
        work.heading = moveDir;
        work.targetSpeed = 0;
    }
    return MoveStepDirected(facing, work.heading, work.targetSpeed);
}

/* 0x489e09 - state 0x2F, pushing an object: the heading is from Ralph toward the object's centre, snapped by the
 * object (Wolf_SnapHeadingToObject, e.g. to 90-degree steps); speed approaches the record's maximum along it, and the
 * facing turns toward it at the travel-turn rates. The object gets msg 10 with this frame's x / z step (when non-zero).
 * Ralph's own x / z step is then replaced by the one that puts him at (horizontal distance - 1, at least 10) from the
 * object's centre, behind it along the new facing (so he faces it). The vertical distance is squared but not used.
 * Returns Wolf_SurfaceVelocity's result. */
/* BYTES(slot-group): locals grouped in work only to pin the original frame offsets; unused12 fills a gap */
s32 Wolf::PushObjectStep(ScnObject *target)
{
    struct {
        Vec3s push;
        s32 msgResult;
        Vec3s newRot;
        s16 unused12;
        Vec3s vel;
        s32 sliding;
        s16 unused20, heading;
        Vec3s step;
        s32 maxSpeed;
        s32 *d;
        const MoveRecord *rec;
        s32 radius;
        s16 unused3c, newFacing;
        WolfStateDesc *desc;
        WolfMoveBank *bank;
    } work;
    work.bank = &g_wolfMoveBank0[mode];
    work.desc = &work.bank->states[state];
    work.rec = &work.bank->profiles[surface][work.desc->profile];
    work.d = (s32 *)g_sharedScratch; /* cast kept: the shared scratch block is raw bytes */
    work.newRot = rot;
    work.d[0] = pos.x - target->pos.x;
    work.d[1] = pos.y - target->pos.y;
    work.d[2] = pos.z - target->pos.z;
    work.heading = Math_RadiansToAngle4096((float)atan2((double)work.d[0], (double)work.d[2]));
    work.heading = SnapHeadingToObject(target, work.heading);
    work.maxSpeed = work.rec->maxSpeed;
    work.d[0] = work.d[0] * work.d[0];
    work.d[1] = work.d[1] * work.d[1];
    work.d[2] = work.d[2] * work.d[2];
    work.radius = (s32)sqrt((double)work.d[0] + work.d[2]) - 1;
    if (work.radius < 10)
        work.radius = 10;
    wantedDir = work.heading;
    bounceSpeed = 0;
    speed = (s16)Math_ApproachLinear(speed, work.maxSpeed, work.rec->maxSpeed, work.rec->acceleration,
                                     work.rec->deceleration);
    work.newFacing = Math_ApproachAngle(work.newRot.y, work.heading, &moveDirAngVel, work.rec->maxTravelTurnRate,
                                        work.rec->travelTurnAcceleration, work.rec->travelTurnAcceleration, 1);
    Vec_FromPolar(&work.vel, speed, work.heading);
    work.vel.y = 0;
    moveDir = work.heading;
    velocity = work.vel;
    facingAngVel = moveDirAngVel;
    work.newRot.y = work.newFacing;
    work.newRot.x = 0;
    work.newRot.z = 0;
    work.sliding = Wolf_SurfaceVelocity(&work.vel, (flags & WOLF_FB_IN_SLIDE_ZONE) || (flags & WOLF_FB_FORCE_SLIDE),
                                        SurfaceTuning());
    Vec3s_ScaleByDt(&work.vel, &work.step);
    if ((work.step.x | work.step.z) != 0) {
        work.push.x = work.step.x;
        work.push.z = work.step.z;
        work.push.y = 0;
        work.msgResult = target->HandleMessage(this, MSG_PUSH, &work.push);
    }
    work.step.x = ((work.radius * g_sinTable4096[work.newFacing]) >> 12) + target->pos.x - pos.x;
    work.step.z = ((work.radius * g_pCosTable[work.newFacing]) >> 12) + target->pos.z - pos.z;
    ApplyMove(&work.step, &work.newRot, RESOLVE_SLIDE_ALL, &work.vel);
    return work.sliding;
}

/* 0x48a18f - rocket flight (states 0x53/0x54). Any stick X aims the heading a quarter turn off the current
 * facing (the turn itself is rate-limited); stick Y drives the vertical speed toward stickY * maxSpeed >> 8. During the
 * launch (launching and stateTime < 0xd1e ticks) the vertical speed is instead a ramp from -2440 (up) to 0, at full
 * speed. Otherwise full record speed with boost, half without. Pitch follows the climb angle, roll half the turn rate.
 * airTime is held at 0. A decaying bounce (bounceSpeed along bounceNormal) is added to the velocity; a wall/floor hit
 * sets a new one from the contact normal(s): -1.5 x the velocity along the normal, clamped to 0..1.5 x maxSpeed.
 * Returns that bounce as a fraction of the cap (4096 = full), or 0 when nothing was hit. */
/* BYTES(slot-group): locals grouped in work only to pin the original frame offsets */
s32 Wolf::RocketFlyStep(s32 boost, s32 launching)
{
    struct {
        s32 result, targetSpeed;
        u32 hit;
        WolfMoveScratch *s;
        MoveRecord *rec;
        s32 climb;
        s16 unused, heading;
        ContactInfo contact;
        WolfStateDesc *desc;
        WolfMoveBank *bank;
    } work;

    work.result = 0;
    work.bank = &g_wolfMoveBank0[mode];
    work.desc = &work.bank->states[state];
    work.rec = &work.bank->profiles[surface][work.desc->profile];
    work.s = (WolfMoveScratch *)g_sharedScratch; /* cast kept: the shared scratch block is raw bytes */
    work.s->rot = rot;
    airTime = 0;
    if (stickX != 0) {
        if (stickX >= 0)
            work.heading = (work.s->rot.y + 0x400) & 4095;
        else
            work.heading = (work.s->rot.y - 0x400) & 4095;
    } else {
        work.heading = work.s->rot.y;
    }
    if (launching && stateTime < 0xd1e) {
        work.s->vel.y = (s16)((0xd1e - stateTime) * -2000 / 0xac0);
        work.targetSpeed = work.rec->maxSpeed;
    } else {
        work.climb = (s16)((stickY * work.rec->maxSpeed) >> 8);
        work.s->vel.y = (s16)Math_ApproachLinear(velocity.y, work.climb, work.rec->maxSpeed, work.rec->verticalRate,
                                                 work.rec->verticalRate);
        if (boost)
            work.targetSpeed = work.rec->maxSpeed;
        else
            work.targetSpeed = work.rec->maxSpeed >> 1;
    }
    wantedDir = work.heading;
    speed = (s16)Math_ApproachLinear(speed, work.targetSpeed, work.rec->maxSpeed, work.rec->acceleration,
                                     work.rec->deceleration);
    work.heading = Math_ApproachAngle(work.s->rot.y, work.heading, &moveDirAngVel, work.rec->maxTravelTurnRate,
                                      work.rec->travelTurnAcceleration, work.rec->travelTurnAcceleration, 1);
    Vec_FromPolar(&work.s->vel, speed, work.heading);
    moveDir = work.heading;
    velocity = work.s->vel;
    facingAngVel = moveDirAngVel;
    if (launching)
        work.s->rot.x = 0;
    else
        work.s->rot.x = Math_RadiansToAngle4096((float)atan2((double)work.s->vel.y, (double)speed)) & 4095;
    work.s->rot.y = work.heading;
    work.s->rot.z = (-moveDirAngVel >> 1) & 4095;
    rot = work.s->rot;
    if (bounceSpeed != 0) {
        bounceSpeed =
            (s16)Math_ApproachLinear(bounceSpeed, 0, bounceSpeed, work.rec->acceleration, work.rec->deceleration);
        work.s->vel.x += (s16)((bounceSpeed * bounceNormal.x) >> 12);
        work.s->vel.y += (s16)((bounceSpeed * bounceNormal.y) >> 12);
        work.s->vel.z += (s16)((bounceSpeed * bounceNormal.z) >> 12);
    }
    Vec3s_ScaleByDt(&work.s->vel, &work.s->step);
    if (flyBox)
        ClampDeltaToBox(flyBox, &work.s->step, 7);
    work.hit = MoveWithPlatform(&work.s->step, &work.contact, RESOLVE_SLIDE_ALL, &work.s->vel);
    if (work.hit & (COLL_FLOOR | COLL_WALL)) {
        if (work.hit & COLL_WALL) {
            bounceNormal = work.contact.wallNormalMean;
            if (work.hit & COLL_FLOOR) { /* wall and floor: the mean of the two normals */
                bounceNormal.x += work.contact.floorNormal.x;
                bounceNormal.y += work.contact.floorNormal.y;
                bounceNormal.z += work.contact.floorNormal.z;
                bounceNormal.x >>= 1;
                bounceNormal.y >>= 1;
                bounceNormal.z >>= 1;
            }
        } else {
            bounceNormal = work.contact.floorNormal;
        }
        bounceSpeed =
            (s16)((-(velocity.x * bounceNormal.x + velocity.y * bounceNormal.y + velocity.z * bounceNormal.z) * 3) >>
                  13);
        if (bounceSpeed > (work.rec->maxSpeed * 3) >> 1)
            bounceSpeed = (s16)((work.rec->maxSpeed * 3) >> 1);
        else if (bounceSpeed < 0)
            bounceSpeed = 0;
        work.result = (bounceSpeed << 12) / ((work.rec->maxSpeed * 3) >> 1);
    }
    return work.result;
}

/* 0x48a767 - one ground step toward a forced heading, braking to a stop (ScnControllable::SteerToHeading), with slope
 * and gravity (Wolf_SurfaceVelocity, double slope rate while flags & 0x3000). Returns Wolf_SurfaceVelocity's result.
 * Unlike the ordinary fall step it does not clamp the height. */
/* BYTES(slot-group): locals grouped in work only to pin the original frame offsets */
s32 Wolf::ForcedHeadingGroundStep(s16 heading)
{
    struct {
        s32 result;
        WolfMoveScratch *s;
        MoveRecord *rec;
        WolfStateDesc *desc;
        WolfMoveBank *bank;
    } work;

    work.bank = &g_wolfMoveBank0[mode];
    work.desc = &work.bank->states[state];
    work.rec = &work.bank->profiles[surface][work.desc->profile];
    work.s = (WolfMoveScratch *)g_sharedScratch; /* cast kept: the shared scratch block is raw bytes */
    work.s->rot = rot;
    SteerToHeading(&work.s->vel, &work.s->rot, work.rec, heading);
    work.result = Wolf_SurfaceVelocity(&work.s->vel, (flags & WOLF_FB_IN_SLIDE_ZONE) || (flags & WOLF_FB_FORCE_SLIDE),
                                       SurfaceTuning());
    Vec3s_ScaleByDt(&work.s->vel, &work.s->step);
    ApplyMove(&work.s->step, &work.s->rot, 0, &work.s->vel);
    return work.result;
}

/* 0x48a897 - climbing (state 0x3f: box = a CLIMBBOXES box, isClimbZone = 1; state 0x40: box = the box of the context
 * object +0x644, isClimbZone = 0). The facing turns to the wall heading: the climb box's direction bits (flags bits 27-30
 * through g_wolfClimbDirTable, x 0x200), or toward the top centre of the context box. The stick's own angle (no camera)
 * moves him in the wall plane, up for stick Y > 0, at a speed approaching maxSpeed * stickMag >> 8; he is pushed into
 * the wall at maxSpeed while moving and during the first 0x1000 ticks. In a climb box the step is clamped to its sides
 * and bottom (axes 5), and to its top as well only with box flag 0x2000000 (axes 7); on a context object his box top is
 * kept from rising above the target. Loop sound 0x28 and animation 0x5f while still, else 0x5b/0x5c (up/down) or
 * 0x5e/0x5d (stick X > 0 / <= 0), whichever stick axis is larger. */
/* BYTES(slot-group): locals grouped in work only to pin the original frame offsets */
void Wolf::ClimbStep(Box *box, s32 isClimbZone)
{
    struct {
        s32 top, wallPush, axes, targetSpeed;
        WolfClimbScratch *s;
        MoveRecord *rec;
        s16 stickDir, heading;
        s32 across;
        WolfStateDesc *desc;
        WolfMoveBank *bank;
    } work;

    work.bank = &g_wolfMoveBank0[mode];
    work.desc = &work.bank->states[state];
    work.rec = &work.bank->profiles[surface][work.desc->profile];
    work.s = (WolfClimbScratch *)g_sharedScratch; /* cast kept: the shared scratch block is raw bytes */
    airTime = 0;
    work.s->rot = rot;
    work.heading = rot.y;
    if (box) {
        if (isClimbZone) {
            work.heading = g_wolfClimbDirTable[(box->flags & CLIMB_DIR_MASK) >> 27];
            work.heading <<= 9;
        } else {
            work.s->target.y = box->min[1] + ctxAction.target->pos.y;
            work.s->target.x = ((box->min[0] + box->max[0]) >> 1) + ctxAction.target->pos.x;
            work.s->target.z = ((box->min[2] + box->max[2]) >> 1) + ctxAction.target->pos.z;
            work.heading = HeadingTo(&work.s->target);
        }
    }
    if (stickMag != 0)
        work.stickDir = Math_RadiansToAngle4096((float)atan2((double)stickX, (double)stickY)) & 4095;
    else
        work.stickDir = 0;
    work.targetSpeed = (work.rec->maxSpeed * stickMag) >> 8;
    wantedDir = 0;
    bounceSpeed = 0;
    speed = (s16)Math_ApproachLinear(speed, work.targetSpeed, work.rec->maxSpeed, work.rec->acceleration,
                                     work.rec->deceleration);
    if (stateTime >= 0x1000 && speed == 0)
        work.wallPush = 0;
    else
        work.wallPush = work.rec->maxSpeed;
    work.across = (speed * g_sinTable4096[work.stickDir]) / 4096;
    /* sideways along the wall plus the push into it (Vec_FromPolar's forward is (-sin, -cos)) */
    work.s->vel.x =
        (s16)((-work.across * g_pCosTable[work.heading] - work.wallPush * g_sinTable4096[work.heading]) / 4096);
    work.s->vel.z =
        (s16)((work.across * g_sinTable4096[work.heading] - work.wallPush * g_pCosTable[work.heading]) / 4096);
    work.s->vel.y = (s16)((-speed * g_pCosTable[work.stickDir]) / 4096);
    work.s->rot.y = Math_ApproachAngle(work.s->rot.y, work.heading, &facingAngVel, work.rec->maxFacingTurnRate,
                                       work.rec->facingTurnAcceleration, work.rec->travelTurnAcceleration, 1);
    work.s->rot.x = 0;
    work.s->rot.z = 0;
    moveDir = 0;
    velocity = work.s->vel;
    Vec3s_ScaleByDt(&work.s->vel, &work.s->step);
    if (box) {
        if (isClimbZone) {
            if (box->flags & CLIMB_F_ALL_AXES)
                work.axes = AXIS_X | AXIS_Y_MIN | AXIS_Y_MAX;
            else
                work.axes = AXIS_X | AXIS_Y_MAX;
            ClampDeltaToBox(box, &work.s->step, work.axes);
        } else {
            work.top = pos.y + GetFirstModelBox()->min.y;
            if (work.s->step.y + work.top < work.s->target.y)
                work.s->step.y = work.s->target.y - work.top;
        }
    }
    ApplyMove(&work.s->step, &work.s->rot, RESOLVE_SLIDE_ALL, &work.s->vel);
    if (speed == 0 || (work.s->step.x | work.s->step.y | work.s->step.z) == 0) {
        PlayLoopSound(SND_SCOCOLD);
        if (AnimId() != ACOYOT01_ANIM_CLIMB5)
            PlayAnim(ACOYOT01_ANIM_CLIMB5, 1, 1);
    } else {
        StopLoopSound();
        if (SDW_ABS(stickX) > SDW_ABS(stickY)) {
            if (stickX <= 0) {
                if (AnimId() != ACOYOT01_ANIM_CLIMB3)
                    PlayAnim(ACOYOT01_ANIM_CLIMB3, 1, 1);
            } else {
                if (AnimId() != ACOYOT01_ANIM_CLIMB4)
                    PlayAnim(ACOYOT01_ANIM_CLIMB4, 1, 1);
            }
        } else {
            if (stickY >= 0) {
                if (AnimId() != ACOYOT01_ANIM_CLIMB1)
                    PlayAnim(ACOYOT01_ANIM_CLIMB1, 1, 1);
            } else {
                if (AnimId() != ACOYOT01_ANIM_CLIMB2)
                    PlayAnim(ACOYOT01_ANIM_CLIMB2, 1, 1);
            }
        }
    }
}

/* 0x48af65 - states 0x41/0x42, walking on top of a climb box (context action 0x11, which becomes 0x12 once inside):
 * the ordinary step, then the action button (unless already 0x12). Back to idle when no CLIMBBOXES box with flag
 * 0x4000000 contains him any more, or on the action edge with context 0x12; a jump when the jump button is held and
 * Wolf_CanJump allows it. Returns 1 while the state goes on. */
s32 Wolf::ClimbBoxWalkStep()
{
    Box *box;
    if (ctxAction.action == CTX_CLIMBBOXWALK_ENTER)
        ctxAction.action = CTX_CLIMBBOXWALK_LEAVE;
    box = climbZones.FindContaining(&pos);
    MoveStep(0);
    if (ctxAction.action != CTX_CLIMBBOXWALK_LEAVE && CheckActionButton(1))
        return 0;
    if (!box || !(box->flags & CLIMB_F_WALKABLE) ||
        ((padBits & WOLF_ACT_ACTION_EDGE) && ctxAction.action == CTX_CLIMBBOXWALK_LEAVE)) {
        SetIdleState();
        return 0;
    }
    if ((padBits & WOLF_ACT_JUMP_EDGE) && CanJump()) {
        SetState(WOLF_ST_JUMP_START);
        return 0;
    }
    return 1;
}

/* 0x48b04b - swimming: the stick steers horizontally (Mobile_Steer, air time held at 0); the vertical speed approaches
 * -maxSpeed while padBits 2 is held (up; not when exactly at the surface, waterZone top + waterDepthOffset) plus
 * +maxSpeed while padBits 8 is held (down), at the record's verticalRate. The water current of the zone is added. An
 * upward step that would cross the surface from within 200 units below the zone top is cut to end at the surface.
 * The pitch turns toward 0xe00 (up) / 0x200 (down) while moving vertically, else back to 0, at 0x800 per second. */
/* BYTES(slot-group): locals grouped in work only to pin the original frame offsets */
void Wolf::SwimMove()
{
    struct {
        s16 unused1, pitchTarget;
        WolfSwimScratch *s;
        s16 unused2, pitch;
        const MoveRecord *rec;
        s32 targetVert;
        s16 unused3, vertSpeed;
        ContactInfo contact;
        WolfStateDesc *desc;
        WolfMoveBank *bank;
    } work;
    work.bank = &g_wolfMoveBank0[mode];
    work.desc = &work.bank->states[state];
    work.rec = &work.bank->profiles[surface][work.desc->profile];
    work.s = (WolfSwimScratch *)g_sharedScratch; /* cast kept: the shared scratch block is raw bytes */
    work.s->rot = rot;
    airTime = 0;
    work.vertSpeed = velocity.y;
    work.pitch = work.s->rot.x;
    Mobile_Steer(&work.s->vel, &work.s->rot, work.rec, 0);
    work.targetVert = 0;
    if (padBits & WOLF_ACT_JUMP_HELD)
        work.targetVert -= work.rec->maxSpeed;
    if (waterZone && pos.y == waterZone->min[1] + waterDepthOffset)
        work.targetVert = 0;
    if (padBits & WOLF_ACT_RUN_HELD)
        work.targetVert += work.rec->maxSpeed;
    work.s->vel.y = (s16)Math_ApproachLinear(work.vertSpeed, work.targetVert, work.rec->maxSpeed,
                                             work.rec->verticalRate, work.rec->verticalRate);
    velocity.y = work.s->vel.y;
    if (waterZone) {
        Zone_GetFlowVelocity(waterZone, &work.s->flow);
        work.s->vel.x += work.s->flow.x;
        work.s->vel.y += work.s->flow.y;
        work.s->vel.z += work.s->flow.z;
    }
    Vec3s_ScaleByDt(&work.s->vel, &work.s->step);
    if (work.s->step.y < 0 && waterZone && pos.y + work.s->step.y < waterZone->min[1] + waterDepthOffset &&
        pos.y >= waterZone->min[1] - 200)
        work.s->step.y = waterZone->min[1] + waterDepthOffset - pos.y;
    work.pitchTarget = 0;
    if (work.s->vel.y != 0) {
        if (padBits & WOLF_ACT_JUMP_HELD)
            work.pitchTarget = 0xe00;
        else if (padBits & WOLF_ACT_RUN_HELD)
            work.pitchTarget = 0x200;
    }
    work.s->rot.x = Math_StepAngleTowards(work.pitch, work.pitchTarget, 0x800);
    rot = work.s->rot;
    MoveWithPlatform(&work.s->step, &work.contact, RESOLVE_SLIDE_ALL, &work.s->vel);
}

/* 0x48b352 - states 0x4a (frozen in an ice block) and 0x4c: float up at 200 u/s plus the water current, steering
 * zeroed, speed = the horizontal length of that velocity. The rise stops at the zone top + 0x69 (from within 200
 * units below the top); on reaching it the frozen river (+0x6a4) gets msg 0x36 once (flags 0x8000 set when it
 * answers). The facing is kept, pitch and roll zeroed. */
/* BYTES(slot-group): locals grouped in work only to pin the original frame offsets */
void Wolf::WaterFloatUpStep()
{
    struct {
        WolfFloatScratch *s;
        const MoveRecord *rec;
        ContactInfo contact;
        WolfStateDesc *desc;
        WolfMoveBank *bank;
    } work;
    work.bank = &g_wolfMoveBank0[mode];
    work.desc = &work.bank->states[state];
    work.rec = &work.bank->profiles[surface][work.desc->profile];
    work.s = (WolfFloatScratch *)g_sharedScratch; /* cast kept: the shared scratch block is raw bytes */
    airTime = 0;
    work.s->vel.z = 0;
    work.s->vel.x = 0;
    work.s->vel.y = -200;
    if (waterZone) {
        Zone_GetFlowVelocity(waterZone, &work.s->flow);
        work.s->vel.x += work.s->flow.x;
        work.s->vel.y += work.s->flow.y;
        work.s->vel.z += work.s->flow.z;
    }
    work.s->sq[0] = work.s->vel.x;
    work.s->sq[1] = work.s->vel.y;
    work.s->sq[2] = work.s->vel.z;
    work.s->sq[0] = work.s->sq[0] * work.s->sq[0];
    work.s->sq[1] = work.s->sq[1] * work.s->sq[1];
    work.s->sq[2] = work.s->sq[2] * work.s->sq[2];
    velocity = work.s->vel;
    speed = (s16)sqrt((double)work.s->sq[0] + work.s->sq[2]);
    wantedDir = 0;
    facingAngVel = 0;
    moveDirAngVel = 0;
    bounceSpeed = 0;
    Vec3s_ScaleByDt(&work.s->vel, &work.s->step);
    if (work.s->step.y < 0 && waterZone && pos.y + work.s->step.y < waterZone->min[1] + 0x69 &&
        pos.y >= waterZone->min[1] - 200) {
        work.s->step.y = waterZone->min[1] + 0x69 - pos.y;
        if (frozenRiver && !(flags & WOLF_FB_RIVER_REGISTERED) &&
            frozenRiver->HandleMessage(this, MSG_RIVER_ADD_CARGO, 0))
            flags |= WOLF_FB_RIVER_REGISTERED;
    }
    MoveWithPlatform(&work.s->step, &work.contact, RESOLVE_SLIDE_ALL, &work.s->vel);
    work.s->rot.y = Facing();
    work.s->rot.z = 0;
    work.s->rot.x = 0;
    rot = work.s->rot;
}

/* 0x48b664 - state 0x49, sinking: the stick steers, the water current is added, and gravity at the surface rate pulls
 * him down to at most 2000 u/s (at the cap airTime is set back to the value that gives exactly 2000). On foot and in a
 * zone without flag 0x2000000 (sink-through) airTime first falls by 3 x g_dt, clamped to 0..0x3000, so the sinking
 * slows to a stop. Leaves the water -> state 6 (fall); landed or airTime down to 0 -> state 0x43 (swim), or, in a
 * freezing zone (0x1000000), drop what he holds and state 0x4a (frozen). Carrying (mode 1) keeps sinking. */
/* BYTES(slot-group): locals grouped in work only to pin the original frame offsets */
s32 Wolf::WaterSinkStep()
{
    struct {
        u32 moveResult;
        s32 sinkThrough;
        WolfSwimScratch *s;
        const MoveRecord *rec;
        s32 gravity;
        WolfStateDesc *desc;
        WolfMoveBank *bank;
        const s16 *tuning;
    } work;
    work.bank = &g_wolfMoveBank0[mode];
    work.tuning = work.bank->surfaceTuning[surface];
    work.desc = &work.bank->states[state];
    work.rec = &work.bank->profiles[surface][work.desc->profile];
    work.s = (WolfSwimScratch *)g_sharedScratch; /* cast kept: the shared scratch block is raw bytes */
    work.s->rot = rot;
    Mobile_Steer(&work.s->vel, &work.s->rot, work.rec, 0);
    work.sinkThrough = 0;
    if (waterZone) {
        if (waterZone->flags & ZONE_WATER_NO_SURFACE)
            work.sinkThrough = 1;
        Zone_GetFlowVelocity(waterZone, &work.s->flow);
        work.s->vel.x += work.s->flow.x;
        work.s->vel.y += work.s->flow.y;
        work.s->vel.z += work.s->flow.z;
    }
    if (mode != WOLF_MODE_CARRY && !work.sinkThrough) {
        airTime -= g_dt * 3;
        if (airTime < 0)
            airTime = 0;
        else if (airTime > 0x3000)
            airTime = 0x3000;
    }
    work.gravity = (work.tuning[8] * airTime) >> 12;
    if (work.s->vel.y + work.gravity > 2000) {
        work.s->vel.y = 2000;
        airTime = (2000 << 12) / work.tuning[8];
    } else
        work.s->vel.y += (s16)work.gravity;
    Vec3s_ScaleByDt(&work.s->vel, &work.s->step);
    work.moveResult = ApplyMove(&work.s->step, &work.s->rot, COLL_WALL, &work.s->vel);
    if (!waterZone) {
        SetState(WOLF_ST_FALL);
        return 0;
    }
    if (mode != WOLF_MODE_CARRY && ((work.moveResult & COLL_FLOOR) || airTime == 0)) {
        if (waterZone->flags & ZONE_WATER_FREEZING) {
            DropHeld(WOLF_DROP_RELEASE);
            SetState(WOLF_ST_FROZEN_FLOAT);
        } else
            SetState(WOLF_ST_WADE);
        return 0;
    }
    return 1;
}

/* 0x48b90b - states 0x43/0x44, swimming: Wolf_SwimMove, then the action button. Out of the water -> state 6; a
 * sink-through zone (0x2000000) -> 0x49; jump held within 20 units below the surface -> 0x45 (jump out); above the
 * surface -> state 1. Returns 1 while swimming goes on. */
s32 Wolf::SwimStateStep()
{
    SwimMove();
    if (CheckActionButton(0))
        return 0;
    if (!waterZone) {
        SetState(WOLF_ST_FALL);
        return 0;
    }
    if (waterZone->flags & ZONE_WATER_NO_SURFACE) {
        SetState(WOLF_ST_WATER_PLUNGE);
        return 0;
    }
    if ((padBits & WOLF_ACT_JUMP_EDGE) && pos.y >= waterZone->min[1] + waterDepthOffset &&
        pos.y <= waterZone->min[1] + waterDepthOffset + 20) {
        SetState(WOLF_ST_WATER_JUMP);
        return 0;
    }
    if (pos.y < waterZone->min[1] + waterDepthOffset) {
        SetState(WOLF_ST_WALK);
        return 0;
    }
    return 1;
}

/* 0x48ba14 - states 0x39/0x3a, in an updraft zone (+0x6cc): the stick steers (air time held at 0); a zone with flag
 * 0x40000000 lifts him at 120 u/s, 0x20000000 doubles the vertical speed. The rise is cut at the zone top (from within
 * 200 units below it). */
/* BYTES(slot-group): locals grouped in work only to pin the original frame offsets */
void Wolf::UpdraftStep()
{
    struct {
        WolfMoveScratch *s;
        const MoveRecord *rec;
        WolfStateDesc *desc;
        WolfMoveBank *bank;
    } work;
    work.bank = &g_wolfMoveBank0[mode];
    work.desc = &work.bank->states[state];
    work.rec = &work.bank->profiles[surface][work.desc->profile];
    work.s = (WolfMoveScratch *)g_sharedScratch; /* cast kept: the shared scratch block is raw bytes */
    airTime = 0;
    work.s->rot = rot;
    Mobile_Steer(&work.s->vel, &work.s->rot, work.rec, 0);
    if (updraftZone) {
        if (updraftZone->flags & FLOW_POS_Z)
            work.s->vel.y = -120;
        if (updraftZone->flags & FLOW_NEG_Z)
            work.s->vel.y *= 2;
    }
    Vec3s_ScaleByDt(&work.s->vel, &work.s->step);
    if (updraftZone && pos.y + work.s->step.y < updraftZone->min[1] && pos.y >= updraftZone->min[1] - 200)
        work.s->step.y = updraftZone->min[1] - pos.y;
    ApplyMove(&work.s->step, &work.s->rot, RESOLVE_SLIDE_ALL, &work.s->vel);
}

/* 0x48bb95 - state 0x3b (the death of zone type 0x4000000): straight up at 240 u/s, air time held at 0. */
/* BYTES(slot-group): locals grouped in work only to pin the original frame offsets */
void Wolf::DeathRiseStep()
{
    struct {
        WolfMoveScratch *s;
        WolfMoveBank *bank;
    } work;
    work.bank = &g_wolfMoveBank0[mode];
    work.s = (WolfMoveScratch *)g_sharedScratch; /* cast kept: the shared scratch block is raw bytes */
    airTime = 0;
    work.s->rot = rot;
    work.s->vel.z = 0;
    work.s->vel.x = 0;
    work.s->vel.y = -240;
    Vec3s_ScaleByDt(&work.s->vel, &work.s->step);
    ApplyMove(&work.s->step, &work.s->rot, RESOLVE_SLIDE_ALL, &work.s->vel);
}

/* 0x48bc2d - the run step: ScnControllable::SteerRun (flags 4 = running), slope and gravity when applySurface
 * (result bit 4 when sliding), move. A wall hit (collision flag 2) head-on - the wall normal within 0x2aa (60 degrees)
 * of the motion heading - low enough on the body (contact height at most pos.y - 70 * scale / 1024) at at least
 * half the record's speed is a crash: result bit 1, and a wall object without class flag 8 gets msg 0x11. With
 * reflectOnCrash the motion heading and the facing become moveDir + 2 * delta, delta being the signed angle between
 * the motion and the into-wall direction (not a mirror bounce: a dead-on hit leaves the heading unchanged). The
 * collision flags go to *outCollFlags. The facing is set through an inlined setter (its argument is the temp at
 * ebp-0x42, 0x48bf08). */
/* BYTES(slot-group): locals grouped in work only to pin the original frame offsets */
/* BYTES(inline): the facing goes through the inlined SetFacing: its argument is the temp at ebp-0x42 (0x48bf08) */
u8 Wolf::RunMove(s32 applySurface, s32 reflectOnCrash, u32 *outCollFlags)
{
    struct {
        u32 moveResult;
        WolfMoveScratch *s;
        const MoveRecord *rec;
        s16 unused1, heading;
        ContactInfo contact;
        WolfStateDesc *desc;
        u8 unused2[3], result;
        WolfMoveBank *bank;
    } work;
    work.result = 0;
    work.bank = &g_wolfMoveBank0[mode];
    work.desc = &work.bank->states[state];
    work.rec = &work.bank->profiles[surface][work.desc->profile];
    work.s = (WolfMoveScratch *)g_sharedScratch; /* cast kept: the shared scratch block is raw bytes */
    work.s->rot = rot;
    SteerRun(&work.s->vel, &work.s->rot, work.rec, flags & WOLF_FB_RUN);
    rot = work.s->rot;
    if (applySurface &&
        Wolf_SurfaceVelocity(&work.s->vel, (flags & WOLF_FB_IN_SLIDE_ZONE) || (flags & WOLF_FB_FORCE_SLIDE),
                             SurfaceTuning()))
        work.result |= WOLF_RUN_SLIDE;
    Vec3s_ScaleByDt(&work.s->vel, &work.s->step);
    work.moveResult = MoveWithPlatform(&work.s->step, &work.contact, RESOLVE_SLIDE_ALL, &work.s->vel);
    if (work.moveResult & COLL_WALL) {
        work.heading = Math_RadiansToAngle4096(
            (float)atan2((double)work.contact.wallNormalMean.x, (double)work.contact.wallNormalMean.z));
        work.heading = SDW_ANGLE_DIFF(moveDir, work.heading);
        if (SDW_ABS(work.heading) <= 0x2aa && work.contact.minContactY <= pos.y - ((scale * 70) >> 10) &&
            speed >= work.rec->maxSpeed / 2) {
            if (!work.contact.wallObj)
                work.result |= 1;
            else if (!(Scenaric_ClassFlags(work.contact.wallObj->GetClassId()) & SCN_CF_08)) {
                work.result |= 1;
                work.contact.wallObj->HandleMessage(this, MSG_BUMP, 0);
            }
            if (reflectOnCrash) {
                moveDir = (work.heading + SDW_ANGLE_DIFF(work.heading, -moveDir)) & 4095;
                SetFacing(moveDir);
            }
        }
    }
    *outCollFlags = work.moveResult;
    return work.result;
}

/* 0x48bf28 - state 0x18, launched (msg 0xC): follows the launch parabola (Wolf+0x66c) until its parameter reaches
 * 0x100, turning toward the direction of travel; the displacement is exactly the way to this frame's point. Rising
 * holds air time at 0. Landing (collision bit 0) -> state 7 with the landing dust timer from the air time at entry
 * (>> 3, at most 0x400); the end of the path without landing -> state 6 (fall). */
/* BYTES(slot-group): locals grouped in work only to pin the original frame offsets */
s32 Wolf::LaunchArcStep()
{
    struct {
        s32 onPath;
        u32 moveResult;
        WolfArcScratch *s;
        const MoveRecord *rec;
        s16 unused, heading;
        s32 airTimeAtStart;
        WolfStateDesc *desc;
        WolfMoveBank *bank;
    } work;
    work.bank = &g_wolfMoveBank0[mode];
    work.desc = &work.bank->states[state];
    work.rec = &work.bank->profiles[surface][work.desc->profile];
    work.s = (WolfArcScratch *)g_sharedScratch; /* cast kept: the shared scratch block is raw bytes */
    work.airTimeAtStart = airTime;
    work.s->rot.x = 0;
    work.s->rot.y = Facing();
    work.s->rot.z = 0;
    work.s->step.x = 0;
    work.s->step.y = 0;
    work.s->step.z = 0;
    work.s->target.z = pos.z;
    work.onPath = launchPath.Step(work.s, g_dtMs >> 2);
    if (work.onPath) {
        work.s->step.x = work.s->target.x - pos.x;
        work.s->step.y = work.s->target.y - pos.y;
        work.s->step.z = work.s->target.z - pos.z;
        if (work.s->step.y < 0)
            airTime = 0;
        if ((work.s->step.x | work.s->step.z) != 0) {
            work.heading =
                (Math_RadiansToAngle4096((float)atan2((double)work.s->step.x, (double)work.s->step.z)) + 2048) & 4095;
            work.s->rot.y = Math_ApproachAngle(work.s->rot.y, work.heading, &facingAngVel, work.rec->maxFacingTurnRate,
                                               work.rec->facingTurnAcceleration, work.rec->travelTurnAcceleration, 1);
        }
    }
    work.s->vel.x = 0;
    work.s->vel.y = 0;
    work.s->vel.z = 0;
    work.moveResult = ApplyMove(&work.s->step, &work.s->rot, COLL_WALL, &work.s->vel);
    if (work.moveResult & COLL_FLOOR) {
        SetState(WOLF_ST_LAND_IDLE);
        landDustTimer = work.airTimeAtStart >> 3;
        if (landDustTimer > 0x400)
            landDustTimer = 0x400;
        return 0;
    }
    if (!work.onPath) {
        SetState(WOLF_ST_FALL);
        return 0;
    }
    return 1;
}

/* 0x48c279 - after a ground step: falling -> state 6 (fall), on a steep slope -> 9 (slide); 1 if neither. */
s32 Wolf::CheckFallOrSlide(s32 onSlope)
{
    if (IsFalling()) {
        SetState(WOLF_ST_FALL);
        return 0;
    }
    if (onSlope) {
        SetState(WOLF_ST_SLOPE_SLIDE);
        return 0;
    }
    return 1;
}

/* 0x48c2b9 - state 0x2B (taking the wheel out): the stick is ignored (stickX, stickY, stickMag zeroed), then an
 * ordinary ground step. No jump test. */
/* BYTES(temp, inferred): the stick words are cleared through a pointer local, as the original (compare Robot::StandStep) */
s32 Wolf::State2B_Step()
{
    s32 result;
    s32 *stick;
    stick = &stickX;
    stick[0] = 0;
    stick[1] = 0;
    stick[2] = 0;
    result = MoveStep(0);
    return CheckFallOrSlide(result);
}

/* 0x48c307 - a ground step with only the fall / slide checks (states 0x6e/0x6f/0x75/0x76). */
s32 Wolf::MoveStepGrounded()
{
    s32 sliding;
    sliding = MoveStep(0);
    return CheckFallOrSlide(sliding);
}

/* 0x48c32d - turn toward a forced heading while braking, then the fall / slide checks. */
s32 Wolf::ForcedHeadingGroundStepChecked(s16 heading)
{
    s32 sliding;
    sliding = ForcedHeadingGroundStep(heading);
    return CheckFallOrSlide(sliding);
}

/* 0x48c358 - turn toward the context-action object (or keep the facing without one). */
s32 Wolf::FaceTargetStep()
{
    s16 heading;
    if (ctxAction.target)
        heading = HeadingTo(&ctxAction.target->pos);
    else
        heading = rot.y;
    return ForcedHeadingGroundStepChecked(heading);
}

/* 0x48c3a4 - an action animation facing its object: back to idle once the context action is no longer ctxType;
 * otherwise returns the animation's finished flag (anim flag 8). 0 when the step changed state. */
s32 Wolf::FaceTargetActionStep(s32 ctxType)
{
    if (FaceTargetStep()) {
        if (ctxAction.action != ctxType) {
            SetIdleState();
            return 0;
        }
        return AnimFlags(ANIM_F_FINISHED);
    }
    return 0;
}

/* 0x48c3e9 - turn in place toward the stick (the facing when centred). */
s32 Wolf::FaceStickStep()
{
    s16 heading;
    s32 sliding;
    heading = GetStickHeading(Facing());
    sliding = ForcedHeadingGroundStep(heading);
    return CheckFallOrSlide(sliding);
}

/* 0x48c431 - the shared ground-state body: action, fall (6), slide (9), jump (3), run (0x22, with run flag 4 and
 * the run tap timer reset), sneak (0xa standing / 0xb moving), then the push test. */
s32 Wolf::GroundCommon()
{
    s32 sliding;
    sliding = MoveStep(0);
    if (CheckActionButton(1))
        return 0;
    if (IsFalling()) {
        SetState(WOLF_ST_FALL);
        return 0;
    }
    if (sliding) {
        SetState(WOLF_ST_SLOPE_SLIDE);
        return 0;
    }
    if ((padBits & WOLF_ACT_JUMP_EDGE) && CanJump()) {
        SetState(WOLF_ST_JUMP_START);
        return 0;
    }
    if (padBits & WOLF_ACT_RUN) {
        flags |= WOLF_FB_RUN;
        runTapTimer = 0;
        SetState(WOLF_ST_RUN_START);
        return 0;
    }
    if (padBits & WOLF_ACT_SNEAK_HELD) {
        if (stickMag == 0)
            SetState(WOLF_ST_SNEAK_IDLE);
        else
            SetState(WOLF_ST_SNEAK_MOVE);
        return 0;
    }
    return !TryStartPush(1);
}

/* 0x48c546 - sneaking (states 0xa/0xb): as the ground body without the jump, run and sneak tests. */
s32 Wolf::SneakStateStep()
{
    s32 sliding;
    sliding = MoveStep(0);
    if (CheckActionButton(1))
        return 0;
    if (IsFalling()) {
        SetState(WOLF_ST_FALL);
        return 0;
    }
    if (sliding) {
        SetState(WOLF_ST_SLOPE_SLIDE);
        return 0;
    }
    return !TryStartPush(1);
}

/* 0x48c5af - states 0x72/0x73: move with the stick keeping a fixed facing (context action 0x17); fall, slide,
 * jump; the action button ends it. */
s32 Wolf::StrafeFixedFacingStep(s16 facing)
{
    s32 sliding;
    ctxAction.action = CTX_ELASTIC_RELEASE;
    sliding = MoveStepFixedFacing(facing);
    if (IsFalling()) {
        SetState(WOLF_ST_FALL);
        return 0;
    }
    if (sliding) {
        SetState(WOLF_ST_SLOPE_SLIDE);
        return 0;
    }
    if ((padBits & WOLF_ACT_JUMP_EDGE) && CanJump()) {
        SetState(WOLF_ST_JUMP_START);
        return 0;
    }
    if (padBits & WOLF_ACT_ACTION_EDGE) {
        SetIdleState();
        return 0;
    }
    return 1;
}

/* 0x48c654 - states 0xad-0xaf: sidestep holding an object (held action 0x15). The held object gets msg 0x63 with
 * Ralph's new position; a non-zero answer puts him back where he was and stops him. Fall, slide; action -> 0xb1;
 * the stick pushed further forward/back than sideways (and |stickY| >= 0x55) -> 0xb0. */
/* BYTES(slot-name): named for its stack slot (tools/vc6_locals.py): oldPos */
s32 Wolf::SidestepHeldStep(s16 facing)
{
    s32 sliding;
    Vec3s oldPos;
    heldActionType = HELD_FISHINGROD_ACTIVE;
    oldPos = pos;
    sliding = SidestepStep(facing);
    if (heldObject->HandleMessage(this, MSG_ROD_TIP_BLOCKED, &pos)) {
        SetPosition(&oldPos);
        StopMotion();
    }
    if (IsFalling()) {
        SetState(WOLF_ST_FALL);
        return 0;
    }
    if (sliding) {
        SetState(WOLF_ST_SLOPE_SLIDE);
        return 0;
    }
    if (padBits & WOLF_ACT_ACTION_EDGE) {
        SetState(WOLF_ST_FISHING_END);
        return 0;
    }
    if (SDW_ABS(stickX) < SDW_ABS(stickY) && SDW_ABS(stickY) >= 0x55) {
        SetState(WOLF_ST_FISHING_REEL);
        return 0;
    }
    return 1;
}

/* 0x48c7c6 - wearing the Bush (context action 0xe = take it off): fall 0x68, slide 0x67, jump 0x69; the action
 * button (unless flag 0x10) stops him, tells the Bush MSG_BUSH_TAKEN_OFF (0x1c02, arg 1), forgets it and enters
 * 0x6d. */
s32 Wolf::BushCommon()
{
    s32 sliding;
    sliding = MoveStep(0);
    ctxAction.action = CTX_BUSH_TAKE_OFF;
    if (IsFalling()) {
        SetState(WOLF_ST_BUSH_FALL);
        return 0;
    }
    if (sliding) {
        SetState(WOLF_ST_BUSH_SLIDE);
        return 0;
    }
    if ((padBits & WOLF_ACT_JUMP_EDGE) && CanJump()) {
        SetState(WOLF_ST_BUSH_JUMP_START);
        return 0;
    }
    if ((padBits & WOLF_ACT_ACTION_EDGE) && !(flags & WOLF_FB_RECENT_CONTACT2)) {
        StopMotion();
        bush->HandleMessage(this, MSG_BUSH_TAKEN_OFF, (void *)1); /* cast kept: the arg is a number (1: at the Wolf) */
        bush = 0;
        SetState(WOLF_ST_BUSH_TAKE_OFF);
        return 0;
    }
    return 1;
}

/* 0x48c8b5 - model set 3 on the ground (states 0x85/0x86, context action 0x18): fall 0x8a, slide 0x8b, jump 0x87,
 * action 0x8c. */
s32 Wolf::ModelSet3GroundStep()
{
    s32 sliding;
    sliding = MoveStep(0);
    ctxAction.action = CTX_BLEAT;
    if (IsFalling()) {
        SetState(WOLF_ST_SHEEPCOSTUME_FALL);
        return 0;
    }
    if (sliding) {
        SetState(WOLF_ST_SHEEPCOSTUME_SLIDE);
        return 0;
    }
    if ((padBits & WOLF_ACT_JUMP_EDGE) && CanJump()) {
        SetState(WOLF_ST_SHEEPCOSTUME_JUMP_START);
        return 0;
    }
    if (padBits & WOLF_ACT_ACTION_EDGE) {
        SetState(WOLF_ST_SHEEPCOSTUME_BLEAT);
        return 0;
    }
    return 1;
}

/* 0x48c963 - the ghost costume's idle (state 0xb4), with a random count of idle loops. */
void Wolf::GhostCostumeSetIdle()
{
    SetState(WOLF_ST_GHOSTCOSTUME_IDLE);
    idleLoopCount = Rand_Range(g_wolfGhostIdleAnims[0].loopsMin, g_wolfGhostIdleAnims[0].loopsMax);
}

/* 0x48c99c - the ghost costume on the ground (states 0xb4/0xb5/0xb9/0xbc, context action 0x1b): fall 0xba, slide
 * 0xbb, jump 0xb6; the action button sends msg 0x70 to every Ghost (class 0x78) within 500 and +-200 vertically,
 * enters 0xbd and plays sound 0x154. */
s32 Wolf::GhostCostumeGroundStep()
{
    s32 sliding;
    sliding = MoveStep(0);
    ctxAction.action = CTX_BOO;
    if (IsFalling()) {
        SetState(WOLF_ST_GHOSTCOSTUME_FALL);
        return 0;
    }
    if (sliding) {
        SetState(WOLF_ST_GHOSTCOSTUME_SLIDE);
        return 0;
    }
    if ((padBits & WOLF_ACT_JUMP_EDGE) && CanJump()) {
        SetState(WOLF_ST_GHOSTCOSTUME_JUMP_START);
        return 0;
    }
    if (padBits & WOLF_ACT_ACTION_EDGE) {
        Scenaric_BroadcastInRadius(CLASSID_GHOST, pos.y - 200, pos.y + 200, 500, MSG_GHOST_BOO, 0, 0);
        SetState(WOLF_ST_GHOSTCOSTUME_BOO);
        Sound_Play(SND_WOLF_GHOST_BOO, this, 0xff, SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 0x1000);
        return 0;
    }
    return 1;
}

/* 0x48ca9d - states 0xc8/0xc9: move with the stick while facing the focus object (+0x63c; the facing is kept
 * without one); fall, slide; back to idle once the focus object is gone. */
/* BYTES(slot-name): named for its stack slot (tools/vc6_locals.py): facingTarget, descriptor, modeBank */
s32 Wolf::StrafeAroundFocusStep()
{
    s32 sliding;
    const MoveRecord *profile;
    s16 facingTarget;
    WolfStateDesc *descriptor;
    WolfMoveBank *modeBank;
    modeBank = &g_wolfMoveBank0[mode];
    descriptor = &modeBank->states[state];
    profile = &modeBank->profiles[surface][descriptor->profile];
    if (magnetTarget)
        facingTarget = HeadingTo(&magnetTarget->pos);
    else
        facingTarget = rot.y;
    sliding = MoveStepDirected(facingTarget, GetStickHeading(moveDir), (profile->maxSpeed * stickMag) >> 8);
    if (IsFalling()) {
        SetState(WOLF_ST_FALL);
        return 0;
    }
    if (sliding) {
        SetState(WOLF_ST_SLOPE_SLIDE);
        return 0;
    }
    if (!magnetTarget) {
        SetIdleState();
        return 0;
    }
    return 1;
}

/* 0x48cbac - states 0x79/0x7a, holding the slow-fall item (held action 0xf): fall 0x7c, action 0x7b, the jump
 * button 0x7d (no CanJump test). The step's slope result is ignored. */
s32 Wolf::SlowFallItemGroundStep()
{
    heldActionType = HELD_UMBRELLA_OPEN;
    MoveStep(0);
    if (IsFalling()) {
        SetState(WOLF_ST_UMBRELLA_GLIDE);
        return 0;
    }
    if (padBits & WOLF_ACT_ACTION_EDGE) {
        SetState(WOLF_ST_UMBRELLA_CLOSE);
        return 0;
    }
    if (padBits & WOLF_ACT_JUMP_EDGE) {
        SetState(WOLF_ST_UMBRELLA_JUMP_START);
        return 0;
    }
    return 1;
}

/* 0x48cc29 - state 0x7c, gliding down with the slow-fall item (quarter gravity, terminal 500): landing 0x79,
 * action 0x82, the jump button restarts the air time and boosts (0x7f). */
s32 Wolf::SlowFallItemFallStep()
{
    u32 coll;
    heldActionType = HELD_UMBRELLA_OPEN;
    coll = FallStep(32000, 1);
    if (coll & COLL_FLOOR) {
        SetState(WOLF_ST_UMBRELLA_IDLE);
        return 0;
    }
    if (padBits & WOLF_ACT_ACTION_EDGE) {
        SetState(WOLF_ST_UMBRELLA_CLOSE_AIR);
        return 0;
    }
    if (padBits & WOLF_ACT_JUMP_EDGE) {
        airTime = 0;
        SetState(WOLF_ST_UMBRELLA_AIR_HOP);
        return 0;
    }
    return 1;
}

/* 0x48ccbe - states 0x7e/0x7f, rising with the slow-fall item: a hit or the end of the rise -> 0x7c (0x80 for the
 * boost); action 0x82. */
s32 Wolf::SlowFallItemAscendStep(s32 height, s32 duration, s32 isBoost)
{
    u32 coll;
    heldActionType = HELD_UMBRELLA_OPEN;
    coll = JumpAscendStep(height, duration);
    if ((coll & COLL_FLOOR) || stateTime >= duration) {
        if (isBoost)
            SetState(WOLF_ST_UMBRELLA_HOP_FALL);
        else
            SetState(WOLF_ST_UMBRELLA_GLIDE);
        return 0;
    }
    if (padBits & WOLF_ACT_ACTION_EDGE) {
        SetState(WOLF_ST_UMBRELLA_CLOSE_AIR);
        return 0;
    }
    return 1;
}

/* 0x48cd4e - states 0xd5/0xd6, flattened: fall 0xd7; after flattenDuration ticks (runTapTimer counts) -> 0xd9. */
s32 Wolf::FlattenedGroundStep()
{
    MoveStep(0);
    if (IsFalling()) {
        SetState(WOLF_ST_FLATTENED_FALL);
        return 0;
    }
    if (runTapTimer >= flattenDuration) {
        SetState(WOLF_ST_FLATTENED_RECOVER);
        return 0;
    }
    return 1;
}

/* 0x48cdaa - the jump ascent states (4/0x20/0x45/0x47): action; a hit -> fall (6); then the push test. */
s32 Wolf::JumpAscendStateStep(s16 height, s32 duration)
{
    u32 coll;
    coll = JumpAscendStep(height, duration);
    if (CheckActionButton(0))
        return 0;
    if (coll & COLL_FLOOR) {
        SetState(WOLF_ST_FALL);
        return 0;
    }
    return !TryStartPush(0);
}

/* 0x48ce06 - a fall that lands in landState, with the landing dust timer min(airTime >> 3, 0x400) (the air time
 * from before this step). */
/* BYTES(slot-name): named for its stack slot (tools/vc6_locals.py): fallTime */
s32 Wolf::FallStepLandTo(u8 landState)
{
    s32 fallTime;
    u32 coll;
    fallTime = airTime;
    coll = FallStep(32000, 0);
    if (coll & COLL_FLOOR) {
        SetState(landState);
        landDustTimer = fallTime >> 3;
        if (landDustTimer > 0x400)
            landDustTimer = 0x400;
        return 0;
    }
    return 1;
}

/* 0x48ce7d - the fall states (5/6/0xd/0x21/0x46/0x48): action (with flag 0x40000 as its argument); landing -> 7
 * (stick centred) or 8, with the landing dust timer; then the push test. */
/* BYTES(slot-name): named for its stack slot (tools/vc6_locals.py): fallTime */
s32 Wolf::FallStateStep(s16 yLimit)
{
    s32 fallTime;
    u32 coll;
    fallTime = airTime;
    coll = FallStep(yLimit, 0);
    if (CheckActionButton(flags & WOLF_FB_SPECIAL_OBJECT))
        return 0;
    if (coll & COLL_FLOOR) {
        if (stickMag == 0)
            SetState(WOLF_ST_LAND_IDLE);
        else
            SetState(WOLF_ST_LAND_MOVE);
        landDustTimer = fallTime >> 3;
        if (landDustTimer > 0x400)
            landDustTimer = 0x400;
        return 0;
    }
    return !TryStartPush(0);
}

/* 0x48cf34 - the run states' body. Running (flag 4) off a drop higher than bank param 7 at full record speed ->
 * 0x26 (keeping the animation if it already is 0x26's); falling or flag 0x2000000 -> 6; RunMove's events: 4 slope
 * -> 9, 1 head-on crash -> stop, 0x25, rumble, camera shake (20 units for 1 s) and fx 0; 2 (outside state 0x24)
 * -> 0x24 keeping the sound. */
/* BYTES(slot-name): named for its stack slot (tools/vc6_locals.py): modeBank, events */
s32 Wolf::RunStep(Pad *pad)
{
    WolfMoveBank *modeBank;
    u8 events;
    u32 collFlags;
    s32 height;
    modeBank = &g_wolfMoveBank0[mode];
    events = RunMove(1, 0, &collFlags);
    height = GroundY() - pos.y;
    if ((flags & WOLF_FB_RUN) && height > modeBank->params[7] && speed >= modeBank->profiles[surface]->maxSpeed) {
        if (AnimId() == modeBank->states[0x26].anim)
            SetStateKeepAnim(WOLF_ST_RUN_AIR);
        else
            SetStateKeepSound(WOLF_ST_RUN_AIR);
        return 0;
    }
    if (IsFalling() || (flags & WOLF_FB_FORCE_FALL)) {
        SetState(WOLF_ST_FALL);
        return 0;
    }
    if (events & WOLF_RUN_SLIDE) {
        SetState(WOLF_ST_SLOPE_SLIDE);
        return 0;
    }
    if (events & WOLF_RUN_CRASH) {
        StopMotion();
        SetState(WOLF_ST_RUN_CRASH);
        pad->Rumble_stub(1000, g_rumbleSeqImpact, 0x1000);
        Camera_StartShake(20, 0x1000);
        Fx0_Start();
        return 0;
    }
    if (state != WOLF_ST_RUN_STOP && (events & WOLF_RUN_BRAKE)) {
        SetStateKeepSound(WOLF_ST_RUN_STOP);
        return 0;
    }
    return 1;
}

/* 0x48d0c7 - move by delta with everything that carries or pushes the mover: both vectors are scaled to Ralph's size,
 * the riders' modifiers (ScnControllable::ApplyMoveModifiers, only while something rides him) may change the delta and
 * set flags 0x10000000, then the collision mover with limits scaled like the body. Records the times of an object
 * contact (+0x614) and of a movable contact (+0x618); a floor hit without an object clears flags 0x400. Any movement
 * sets flags 0x100 and, unless the state has flag 0x20, adds the XZ length to distanceTravelled (saturating). */
/* BYTES(slot-group): locals grouped in work only to pin the original frame offsets */
/* BYTES(flow, inferred): ClearFlags is written in both arms because the original has both calls */
u32 Wolf::MoveWithPlatform(Vec3s *delta, ContactInfo *contact, u16 mask, Vec3s *vel)
{
    struct {
        Vec3s vel;
        s32 modFlag4; /* ApplyMoveModifiers bit 4: state flag 0x40 or model set 3 */
        u32 result;
        Vec3s delta;
        WolfStateDesc *desc;
        s32 modified;
        WolfMoveBank *bank;
        s16 *tuning;
    } work;
    work.bank = &g_wolfMoveBank0[mode];
    work.tuning = work.bank->surfaceTuning[surface];
    work.desc = &work.bank->states[state];
    ScaleVec(delta);
    ScaleVec(vel);
    if (work.desc->flags & WSF_RUN_COLLISION)
        work.modFlag4 = 1;
    else
        work.modFlag4 = 0;
    if (modelSet == WMS_SHEEPCOSTUME)
        work.modFlag4 = 1;
    work.delta = *delta;
    if (riderCount > 0) {
        work.vel = *vel;
        work.modified = ApplyMoveModifiers(&work.delta, &work.vel, work.desc->flags & WSF_GROUND_STATE,
                                           work.desc->flags & WSF_INTERACTING, work.modFlag4);
        if (work.modified)
            flags |= WOLF_FB_ON_MOVING_PLATFORM;
        else
            ClearFlags(WOLF_FB_ON_MOVING_PLATFORM);
    } else
        ClearFlags(WOLF_FB_ON_MOVING_PLATFORM);
    if (mask == 0 && (work.delta.x | work.delta.z) != 0)
        mask = RESOLVE_SLIDE_ALL;
    work.result = Wolf_MoveResolve(&work.delta, contact, work.tuning[3], mask, (scale * 35) >> 10, (scale * 100) >> 10,
                                   (scale * 10) / 1024);
    if (contact->movableObj)
        lastContact2Time = g_gameTime;
    if (contact->floorObj)
        lastObjectContactTime = g_gameTime;
    else if (work.result & COLL_FLOOR)
        ClearFlags(WOLF_FB_SEESAW_CAM);
    *delta = work.delta;
    if (delta->x | delta->y | delta->z) {
        flags |= WOLF_FB_MOVED;
        if (!(work.desc->flags & WSF_NO_DISTANCE_COUNT)) {
            distanceTravelled += (s32)sqrt((double)delta->x * delta->x + delta->z * delta->z);
            if (distanceTravelled > 0x7fffffff)
                distanceTravelled = 0x7fffffff;
        }
    }
    return work.result;
}
