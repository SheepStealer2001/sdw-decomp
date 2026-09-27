/* T084 - original object Patrol.cpp (guessed name; possibly two files, a Box helper and the patrol helpers - the split
 * at 0x459710 cannot be tested): .text 0x4593a0-0x459a8c, no data in any section.
 * Box_SegmentCrossesXZ 0x4593a0 (a free helper using the CollBox signature; Box and CollBox share these offsets), then
 * Vec3s_NormalizeInPlace 0x459710, TrajPatrol_Init 0x459789 and TrajPatrol_Step 0x4597f5. The language is not decided
 * by the map; compiled as C++. */
/* BYTES: slot-group, slot-name. */
#include "sdw_classes.h"
inline s32 PointInsideXZ(CollBox *box, Vec3s *p)
{
    return p->x >= box->min.x && p->x <= box->max.x && p->z >= box->min.z && p->z <= box->max.z;
}
#define COPY_POINT(dst, src) \
    dst.x = (src)->x;        \
    dst.y = (src)->y;        \
    dst.z = (src)->z
/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets; unused0, unused1 fill gaps */
s32 Box_SegmentCrossesXZ(CollBox *box, Vec3s *a, Vec3s *b)
{
    struct Work {
        s32 fraction, coordinate;
        Vec3s low;
        u16 unused0;
        Vec3s high;
        u16 unused1;
    } w;
    if (PointInsideXZ(box, a))
        return 1;
    if (PointInsideXZ(box, b))
        return 1;
    if (a->z > b->z) {
        COPY_POINT(w.high, a);
        COPY_POINT(w.low, b);
    } else {
        COPY_POINT(w.high, b);
        COPY_POINT(w.low, a);
    }
    w.fraction = (((w.high.x - w.low.x) * (w.high.z - box->max.z)) >> 12) / (w.high.z - w.low.z);
    w.coordinate = w.high.x - (w.fraction << 12);
    if (w.coordinate > box->min.x && w.coordinate < box->max.x)
        return 1;
    w.fraction = (((w.high.x - w.low.x) * (w.high.z - box->min.z)) >> 12) / (w.high.z - w.low.z);
    w.coordinate = w.high.x - (w.fraction << 12);
    if (w.coordinate > box->min.x && w.coordinate < box->max.x)
        return 1;
    if (a->x > b->x) {
        COPY_POINT(w.high, a);
        COPY_POINT(w.low, b);
    } else {
        COPY_POINT(w.high, b);
        COPY_POINT(w.low, a);
    }
    w.fraction = (((w.high.z - w.low.z) * (w.high.x - box->max.x)) >> 12) / (w.high.x - w.low.x);
    w.coordinate = w.high.z - (w.fraction << 12);
    if (w.coordinate > box->min.z && w.coordinate < box->max.z)
        return 1;
    w.fraction = (((w.high.z - w.low.z) * (w.high.x - box->min.x)) >> 12) / (w.high.x - w.low.x);
    w.coordinate = w.high.z - (w.fraction << 12);
    if (w.coordinate > box->min.z && w.coordinate < box->max.z)
        return 1;
    return 0;
}

#include "../engine/fixed_math.h"
#include "../sdk/crt.h"
extern "C" s16 Math_RadiansToAngle4096(float);

void Vec3s_NormalizeInPlace(Vec3s *v)
{
    s16 length = (s16)sqrt((double)Vec3s_LengthSq(v));
    v->x = (v->x << 12) / length;
    v->y = (v->y << 12) / length;
    v->z = (v->z << 12) / length;
}

void TrajPatrol_Init(TrajPatrol *f, Trajectory *traj, s16 speed, s16 headingBias, u32 continuousHeading,
                     u32 use3dDistance, s16 radius)
{
    f->traj = traj;
    f->pointIndex = 0;
    f->arriveRadiusSq = radius * radius;
    f->speed = speed;
    f->headingBias = headingBias;
    f->heading = 0;
    f->continuousHeading = continuousHeading;
    f->use3dDistance = use3dDistance;
    f->moving = 0;
    f->forward = 1;
}

/* BYTES(slot-name): names share VC6 bucket 2; declared in reverse to keep the original slots */
s32 TrajPatrol_Step(Vec3s *pos, TrajPatrol *f, Vec3s *velocity, s16 *heading)
{
    /* These names share VC6's local bucket2; reverse declarations retain the original slots. */
    s16 retries_9;
    Vec3s *target_14;
    s32 advance_9, dz_6, dy_18, dx_19, distance_27, wrapped_1;
    wrapped_1 = 0;
    f->advanced = 0;
    f->moving = 1;
    retries_9 = 200;
    do {
        target_14 = &f->traj->pts[f->pointIndex];
        dx_19 = target_14->x - pos->x;
        dz_6 = target_14->z - pos->z;
        if (f->use3dDistance)
            dy_18 = target_14->y - pos->y;
        else
            dy_18 = 0;
        distance_27 = dx_19 * dx_19 + dy_18 * dy_18 + dz_6 * dz_6;
        advance_9 = distance_27 < f->arriveRadiusSq;
        if (f->forceAdvance) {
            advance_9 = 1;
            f->forceAdvance = 0;
        }
        if (advance_9) {
            if (f->forward)
                f->pointIndex++;
            else
                f->pointIndex--;
            f->advanced = 1;
            if (f->pointIndex >= f->traj->count - 1 || f->pointIndex < 0) {
                if (f->pointIndex >= f->traj->count) {
                    f->pointIndex = 0;
                    if (f->forward)
                        wrapped_1 = 1;
                }
                if (f->pointIndex < 0) {
                    f->pointIndex = f->traj->count - 1;
                    if (!f->forward)
                        wrapped_1 = 1;
                }
            }
        }
        if (retries_9-- <= 0)
            break;
    } while (advance_9);
    distance_27 = (s32)sqrt((double)distance_27);
    velocity->x = (dx_19 * f->speed) / distance_27;
    velocity->z = (dz_6 * f->speed) / distance_27;
    if (f->use3dDistance)
        velocity->y = (dy_18 * f->speed) / distance_27;
    else
        velocity->y = 0;
    if (f->advanced || f->continuousHeading) {
        f->heading = (f->headingBias + Math_RadiansToAngle4096((float)atan2(dx_19, dz_6))) & 0xfff;
        *heading = f->heading;
    } else
        *heading = f->heading;
    return wrapped_1;
}
