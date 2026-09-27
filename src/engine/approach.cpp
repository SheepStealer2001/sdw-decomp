/*
 * T280, guessed original file Approach.cpp: .text 0x546680-0x547237, no data.
 * Contents: Math_ApproachAngle 0x546680, Math_ApproachLinear 0x546833, the approach / step helpers and the path
 * follower 0x5468ee-0x54723f.
 * (The map notes a possible hidden file split at 0x546bc0, before the path follower; nothing in the bytes decides it,
 * so this is one object as the map has it.)
 *
 * Time unit 1/4096 s (g_dt); angles 4096 per turn. Local names were picked for the /Od frame order
 * (tools/vc6_locals.py): plausible, not recovered.
 */
/* BYTES: slot-group, temp. */
#include "sdw_types.h"
#include "sdw_classes.h"
#include "sdw_enums.h"

#include "../sdk/crt.h"
extern "C" s16 Math_RadiansToAngle4096(float radians); /* 0x5269ce */
#include "lerp.h"

extern s32 g_dt;   /* 0x71b300  frame delta used by gameplay, clamped to 0..0xAA */
extern s32 g_dtMs; /* 0x71b2e8 */

#define SDW_ABS(v) ((v) >= 0 ? (v) : -(v))

/* 0x546680 - signed 16-bit angle inputs/return; 32-bit angular rate. */
/* BYTES(slot-group): desired_rate / rate_step / step grouped in one struct to keep the original three slots */
s16 Math_ApproachAngle(s16 cur, s16 target, s32 *rate, s32 limit, s32 accel, s32 decel, s32 force_min_step)
{
    /* Grouped to preserve the original three stack slots with readable names. */
    struct {
        s32 desired_rate, rate_step, step;
    } work;
    work.desired_rate = ((s16)((s16)((target - cur + 2048) & 4095) - 2048) << 12) / g_dt;
    work.step = SDW_ABS(work.desired_rate);
    if (work.step <= (limit << 1)) {
        if (work.step > 1)
            limit = work.step >> 1;
        else
            limit = work.step;
    }
    if (SDW_ABS(work.desired_rate) >= SDW_ABS(*rate))
        work.rate_step = accel;
    else
        work.rate_step = decel;
    work.rate_step = (work.rate_step * g_dt) >> 12;
    if (SDW_ABS(work.desired_rate - *rate) <= work.rate_step) {
        if (work.desired_rate > limit)
            work.desired_rate = limit;
        else if (work.desired_rate < -limit)
            work.desired_rate = -limit;
    } else {
        if (work.desired_rate > *rate) {
            work.desired_rate = *rate + work.rate_step;
            if (work.desired_rate > limit)
                work.desired_rate = limit;
        } else {
            work.desired_rate = *rate - work.rate_step;
            if (work.desired_rate < -limit)
                work.desired_rate = -limit;
        }
    }
    work.step = (work.desired_rate * g_dt) >> 12;
    if (force_min_step && work.step == 0 && work.desired_rate != 0) {
        if (work.desired_rate >= 0)
            work.step = 1;
        else
            work.step = -1;
    }
    target = (s16)((cur + work.step) & 4095);
    *rate = work.desired_rate;
    return target;
}

/* 0x546833 - step uses accel when target >= cur, decel otherwise; it is not a speed-magnitude comparison.
 * The per-frame step is truncated, so it can be ZERO at high frame rates (accel * g_dt < 4096). */
s32 Math_ApproachLinear(s32 cur, s32 target, s32 limit, s32 accel, s32 decel)
{
    s32 step;
    if (target >= cur)
        step = accel;
    else
        step = decel;
    step = (step * g_dt) >> 12;
    if (SDW_ABS(target - cur) <= step) {
        if (target > limit)
            target = limit;
        else if (target < -limit)
            target = -limit;
    } else {
        if (target > cur) {
            target = cur + step;
            if (target > limit)
                target = limit;
        } else {
            target = cur - step;
            if (target < -limit)
                target = -limit;
        }
    }
    return target;
}

/* ======================================================================================================================
 * 0x5468ee-0x54723f: the approach / step helpers after Math_ApproachAngle and Math_ApproachLinear, and the path follower.
 */

/* 0x5468ee - moves a point towards a target along the straight line, at a speed that accelerates / decelerates
 * (Math_ApproachLinear) and brakes to land on the target: the speed limit becomes half the distance per unit time
 * once that is below it. */
/* BYTES(temp): the squares go through the Vec3i sq so each is converted on its own (fild / fiadd / fiadd at 0x546949) */
void Vec3s_ApproachPoint(Vec3s *cur, const Vec3s *target, s32 *speed, s32 maxSpeed, s32 accel, s32 decel)
{
    s32 newSpeed;
    s32 dist;
    Vec3i sq; /* the squared components, each converted on its own (fild / fiadd / fiadd at 0x546949) */
    Vec3i d;

    d.x = target->x - cur->x;
    d.y = target->y - cur->y;
    d.z = target->z - cur->z;
    sq.x = d.x * d.x;
    sq.y = d.y * d.y;
    sq.z = d.z * d.z;
    dist = ((s32)sqrt((double)sq.x + sq.y + sq.z) << 12) / g_dt;
    if (dist <= maxSpeed << 1) {
        if (dist > 1)
            maxSpeed = dist >> 1;
        else
            maxSpeed = dist;
    }
    newSpeed = Math_ApproachLinear(*speed, dist, maxSpeed, accel, decel);
    if (dist != 0 && newSpeed != dist) {
        d.x = d.x * newSpeed / dist;
        d.y = d.y * newSpeed / dist;
        d.z = d.z * newSpeed / dist;
    }
    cur->x += (s16)d.x;
    cur->y += (s16)d.y;
    cur->z += (s16)d.z;
    *speed = newSpeed;
}

/* 0x546a31 - the one-dimensional Vec3s_ApproachPoint: returns cur moved towards target by this frame's step of *vel. */
s32 Math_ApproachValue(s32 cur, s32 target, s32 *vel, s32 maxRate, s32 accel, s32 decel)
{
    s32 rate;
    s32 dist;

    rate = ((target - cur) << 12) / g_dt;
    dist = rate >= 0 ? rate : -rate;
    if (dist <= maxRate << 1) {
        if (dist > 1)
            maxRate = dist >> 1;
        else
            maxRate = dist;
    }
    rate = Math_ApproachLinear(*vel, rate, maxRate, accel, decel);
    target = cur + (rate * g_dt >> 12);
    *vel = rate;
    return target;
}

/* 0x546acb - steps cur towards target by at most ratePerSec * g_dt / 4096 (truncated: zero below 4096 / g_dt). */
s32 Math_StepTowards(s32 cur, s32 target, s32 ratePerSec)
{
    s32 delta;
    s32 step;

    delta = target - cur;
    if (delta != 0) {
        step = ratePerSec * g_dt >> 12;
        if ((delta >= 0 ? delta : -delta) > step) {
            if (delta > 0)
                delta = step;
            else
                delta = -step;
        }
        target = cur + delta;
    }
    return target;
}

/* 0x546b34 - Math_StepTowards on a 12-bit angle: the difference is wrapped into -0x800..0x7ff first. */
s16 Math_StepAngleTowards(s16 cur, s16 target, s32 ratePerSec)
{
    s32 delta;
    s32 step;

    delta = (s16)((s16)((target - cur + 0x800) & 0xfff) - 0x800);
    if (delta != 0) {
        step = ratePerSec * g_dt >> 12;
        if ((delta >= 0 ? delta : -delta) > step) {
            if (delta > 0)
                delta = step;
            else
                delta = -step;
        }
        target = (cur + delta) & 0xfff;
    }
    return target;
}

/* the path's points */
#define PF_POINT(i) (path->pts[i])

/* 0x546bc0 - the length of the current segment (points[fromIdx] to points[toIdx]). */
/* BYTES(slot-group): d / sq are Vec4i (PSX VECTOR, pad unused) because the frame has a 16-byte slot for each (d -0x10, sq -0x20) */
s32 PathFollower::GetSegmentLength()
{
    Vec4i sq; /* 16-byte vectors: the frame has a 4-byte hole after each (0x546bc0: d at -0x10, sq at -0x20) */
    Vec4i d;

    d.x = PF_POINT(fromIdx).x - PF_POINT(toIdx).x;
    d.y = PF_POINT(fromIdx).y - PF_POINT(toIdx).y;
    d.z = PF_POINT(fromIdx).z - PF_POINT(toIdx).z;
    sq.x = d.x * d.x;
    sq.y = d.y * d.y;
    sq.z = d.z * d.z;
    return (s32)sqrt((double)sq.x + sq.y + sq.z);
}

/* 0x546c8e - sets up the segment from segStart: its end point (with smooth set and a point after it, the halfway
 * point towards that next point, so corners are cut), the heading, and the rate for speed units per second. */
/* BYTES(slot-group, inferred): d / sq are Vec4i (PSX VECTOR, pad unused) for their 16-byte frame slots, as in GetSegmentLength */
void PathFollower::BeginSegment()
{
    s32 len;
    Vec4i sq;
    Vec4i d;
    s32 time;

    if (smooth == 1 && toIdx < path->count - 1) {
        Lerp_SetVecTarget(&PF_POINT(toIdx + 1));
        Vec3s_LerpToTarget(&segEnd, &PF_POINT(toIdx), 0x800);
    } else {
        segEnd.x = PF_POINT(toIdx).x;
        segEnd.y = PF_POINT(toIdx).y;
        segEnd.z = PF_POINT(toIdx).z;
    }
    if (smooth == 1 && toIdx < path->count - 1) {
        d.x = segStart.x - segEnd.x;
        d.y = segStart.y - segEnd.y;
        d.z = segStart.z - segEnd.z;
    } else {
        d.x = segStart.x - PF_POINT(toIdx).x;
        d.y = segStart.y - PF_POINT(toIdx).y;
        d.z = segStart.z - PF_POINT(toIdx).z;
    }
    heading = Math_RadiansToAngle4096((float)atan2((double)d.x, (double)d.z)) & 0xfff;
    sq.x = d.x * d.x;
    sq.y = d.y * d.y;
    sq.z = d.z * d.z;
    len = (s32)sqrt((double)sq.x + sq.y + sq.z);
    time = len * 1000 / speed;
    rate = 0x400000 / time;
    t = 0;
}

/* 0x546eb5 - starts the path at its first point, at speed units per second. */
void PathFollower::Start(s32 speed)
{
    fromIdx = 0;
    toIdx = 1;
    this->speed = speed;
    smooth = 0;
    segStart.x = PF_POINT(0).x;
    segStart.y = PF_POINT(0).y;
    segStart.z = PF_POINT(0).z;
    BeginSegment();
}

/* 0x546f1e - one frame along a straight segment. Returns PATHSTEP_ON_SEGMENT while on it, PATHSTEP_NEXT_SEGMENT when
 * it moved on to the next segment, PATHSTEP_END at the end of the path (rate is then 0). The position lags a frame:
 * it is lerped before t advances. */
u8 PathFollower::Step()
{
    Lerp_SetVecTarget(&PF_POINT(toIdx));
    Vec3s_LerpToTarget(&pos, &PF_POINT(fromIdx), t);
    t = t + rate * g_dtMs / 1024;
    if (t >= 0x1000) {
        if (toIdx == path->count - 1) {
            rate = 0;
            return PATHSTEP_END;
        }
        fromIdx++;
        toIdx++;
        smooth = 0;
        segStart.x = PF_POINT(fromIdx).x;
        segStart.y = PF_POINT(fromIdx).y;
        segStart.z = PF_POINT(fromIdx).z;
        BeginSegment();
        return PATHSTEP_NEXT_SEGMENT;
    }
    return PATHSTEP_ON_SEGMENT;
}

/* 0x54705f - Step through the rounded corner: a quadratic curve over (segStart, points[toIdx], segEnd), heading along
 * its tangent. The next segment starts at this one's end with smooth set. */
u8 PathFollower::StepSmooth()
{
    Vec3s a;
    Vec3s b;

    Lerp_SetVecTarget(&segEnd);
    Vec3s_LerpToTarget(&a, &PF_POINT(toIdx), t);
    Lerp_SetVecTarget(&PF_POINT(toIdx));
    Vec3s_LerpToTarget(&b, &segStart, t);
    Lerp_SetVecTarget(&a);
    Vec3s_LerpToTarget(&pos, &b, t);
    b.x -= a.x;
    b.y -= a.y;
    b.z -= a.z;
    heading = Math_RadiansToAngle4096((float)atan2((double)b.x, (double)b.z)) & 0xfff;
    t = t + rate * g_dtMs / 1024;
    if (t >= 0x1000) {
        if (toIdx == path->count - 1) {
            rate = 0;
            return PATHSTEP_END;
        }
        fromIdx++;
        toIdx++;
        smooth = 1;
        segStart.x = segEnd.x;
        segStart.y = segEnd.y;
        segStart.z = segEnd.z;
        BeginSegment();
        return PATHSTEP_NEXT_SEGMENT;
    }
    return PATHSTEP_ON_SEGMENT;
}
