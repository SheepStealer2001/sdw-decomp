/* PAL PC shared Wolf/Sheep wall-avoidance steering. */
/* BYTES: slot-group. */
#include "sdw_enums.h"
#include "sdw_classes.h"
#include "../sdk/crt.h"
extern "C" s16 Math_RadiansToAngle4096(float);
void WallAvoid::Reset()
{
    flags.active = 0;
    flags.turnSide = 0;
    wallNormal.x = 0;
    wallNormal.y = 0;
    wallNormal.z = 0;
}
/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets */
void WallAvoid::ComputeSteer(const Vec3s *from, u16 fallback, const Vec3s *target, s16 keepDist, u16 *outSteer,
                             u16 *outTarget, s16 *outDistance)
{
    struct Work {
        s16 steer, targetHeading;
        s32 dx, dy, dz;
        s16 unused, difference, distance, wallHeading;
    } w;
    w.dx = target->x - from->x;
    w.dy = target->y - from->y;
    w.dz = target->z - from->z;
    if (!(w.dx | w.dz))
        w.targetHeading = fallback;
    else
        w.targetHeading = (Math_RadiansToAngle4096((float)atan2(w.dx, w.dz)) + 2048) & 4095;
    w.dx *= w.dx;
    w.dy *= w.dy;
    w.dz *= w.dz;
    w.distance = (s16)sqrt((double)w.dx + (double)w.dz);
    if (w.distance < keepDist) {
        w.distance = keepDist - w.distance;
        w.steer = (w.targetHeading + 2048) & 4095;
    } else {
        w.distance -= keepDist;
        w.steer = w.targetHeading;
    }
    if (flags.active) {
        w.wallHeading = Math_RadiansToAngle4096((float)atan2(wallNormal.x, wallNormal.z)) & 4095;
        w.difference = (s16)((w.steer - w.wallHeading + 2048) & 4095) - 2048;
        if (flags.turnSide) {
            if (w.difference < -512)
                flags.turnSide = 0;
        } else if (w.difference > 512)
            flags.turnSide = 1;
        if (flags.turnSide)
            w.steer = (w.wallHeading + 768) & 4095;
        else
            w.steer = (w.wallHeading - 768) & 4095;
    }
    *outSteer = w.steer;
    *outTarget = w.targetHeading;
    *outDistance = w.distance;
}
void WallAvoid::UpdateFromContact(const ContactInfo *contact, u16 result)
{
    if (result & COLL_WALL) {
        wallNormal = contact->wallNormalMean;
        flags.active = 1;
    } else
        flags.active = 0;
}
