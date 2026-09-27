/* T076 - original object DancingGhost.cpp (guessed name): .text 0x441310-0x44299b, then the COMDAT
 * DancingGhost::UpdateState 0x4429a0-0x444304; .rdata 0x574e34-0x574e58 (vtable), .data 0x57a80c-0x57a818 (the cinematic
 * header's stride-table copy), .bss 0x6cc87c-0x6cc8ec (+4 padding). StartDanceCamera and the factory sit after
 * UpdateCarry (their original place), and UpdateState is an inline member defined after its caller, so it is emitted
 * as the COMDAT that follows the main .text.
 * .bss ORDER: VC6 lays out a file's UNinitialised globals by a hash of their names - bucket (h ^ h >> 16) & 1023 of
 * h = (h << 2) + (h >> 4) + c, ascending, the last-declared first inside a bucket - not by definition order. The 15
 * are written with zero initializers instead, which keeps them in .bss but in definition order, so the names do not
 * decide the order. g_dgFailedLines and g_dgFailedVoices are the names T133 (the manager) and data/symbols.csv use.
 * match-addr: g_dgFailedLines=0x6cc87c g_dgFailedVoices=0x6cc890
 * PAL PC DancingGhost. Reads the dialogue scroll's flags through the shared TextScroll view (sdw_global_views.h). */
/* BYTES: bss-name, dead-code, layout, slot-group, slot-scope, temp. */
/* BYTES(layout): the Cine.h header static: every object including the cinematic header carries this copy in its .data, referenced or not */
/* BYTES(bss-name): renamed for the .bss hash order; the file now writes its globals '= 0', so the name no longer decides the order */
/* BYTES(bss-name): renamed for the .bss hash order; the file now writes its globals '= 0', so the name no longer decides the order */
/* BYTES(layout): written '= 0' only to keep definition order in .bss */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *, Animator *, u16, u32);
#include "../engine/sound_mgr.h"
#include "ghosthalo.h"
#include "../app/app_main.h"
#include "../engine/scenaric.h"
#include "../engine/scn_tools.h"
#include "camera.h"
#include "fireball.h"
#include "../engine/pause_menu.h"
#include "../engine/map.h"
u16 Sound_Play(u16, void *, u16, u8, s32);
#include "../sdk/crt.h"
#define SDW_INLINE_FREE_INSTFLAGSSET_U16_U16 1
#include "instance_inlines.h"
#undef SDW_INLINE_FREE_INSTFLAGSSET_U16_U16
#define SDW_INLINE_FREE_INSTFLAGSCLEAR_U16_U16 1
#include "instance_inlines.h"
#undef SDW_INLINE_FREE_INSTFLAGSCLEAR_U16_U16

#define SDW_MEMBERS_ScnObject       \
    static void *operator new(u32); \
    void SetFacing(s16 a);          \
    void FacePoint(Vec3s *p)        \
    {                               \
        SetFacing(HeadingTo(p));    \
    }                               \
    void SetUpdateMode(u8 mode);    \
    void SetUpdateMode(s32 mode);


#define SDW_MEMBERS_DancingGhost                                  \
    void SetPathIndex(s16 value)                                  \
    {                                                             \
        traj.pointIndex = value;                                  \
    }                                                             \
    s16 GetPathIndex()                                            \
    {                                                             \
        s16 result = traj.pointIndex;                             \
        return result;                                            \
    }                                                             \
    s32 StepPath(Vec3s *velocity, s16 *heading)                   \
    {                                                             \
        s32 result = TrajFollower_Step(&traj, velocity, heading); \
        return result;                                            \
    }                                                             \
    void UpdateState();                                           \
    void CopyStep(u16, u16);                                      \
    void ClearStep(u16);                                          \
    void InitializeQueueInline();
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_STOPSOUND_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_STOPSOUND_U16
#define SDW_INLINE_SCNOBJECT_PLAYSOUND_U16_U16_U8_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_PLAYSOUND_U16_U16_U8_S32
#define SDW_INLINE_SCNOBJECT_INWORLD 1
#define SDW_INLINE_SCNOBJECT_SETFACING_S16 1
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_INWORLD
#undef SDW_INLINE_SCNOBJECT_SETFACING_S16
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32
#define SDW_INLINE_SCNOBJECT_ENABLEBOXCOLLIDE_S32 1
#define SDW_INLINE_SCNOBJECT_ENABLETINT_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_ENABLEBOXCOLLIDE_S32
#undef SDW_INLINE_SCNOBJECT_ENABLETINT_S32
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#include "sdw_global_views.h"
/* 0x57a80c - the cinematic header's static copy of the 9-byte opcode stride table (src/engine/cine.cpp,
 * g_cineOpStride 0x5816fc). The original header defined it static, so every object including it carries its own
 * unreferenced copy at the head of its .data; defined here in its place (the map calls this slot a guess: DancingGhost
 * does not use g_cinePlayer; the bytes are the same whichever includer holds it). */
static u8 s_cineOpStride[9] = {0, 8, 8, 4, 2, 2, 4, 2, 2};
#define NEW(o) ((o)->newStep)
#define DONE(o) ((o)->stepDone)
#define ELAPSED(o) ((o)->elapsedMs)
#define SOUND(o) ((o)->soundHandle)
#define CHAT(o) ((o)->chatBox)
#define HOME(o) ((o)->homePos)
#define ROT(o) ((o)->homeRot)
#define PREV(o) ((o)->previousStep)
#define PHASE(o) ((o)->carryPhase)
/* .bss 0x6cc87c-0x6cc8ec, in address order; the zero initializers keep that order (see the header). */
const char *g_dgFailedLines[5] = {0}; /* 0x6cc87c */
u32 g_dgFailedVoices[4] = {0};        /* 0x6cc890 */
Vec3s g_dgDefaultPos = {0};           /* 0x6cc8a0 */
Vec3s g_dgCarryPos = {0};             /* 0x6cc8a8 */
Vec3s g_dgCarryA = {0};               /* 0x6cc8b0 */
Vec3s g_dgCarryB = {0};               /* 0x6cc8b8 */
Vec3s g_dgFollowerVelocity = {0};     /* 0x6cc8c0 */
Vec3s g_dgFollowerStep = {0};         /* 0x6cc8c8 */
Vec3s g_dgMonolithePos = {0};         /* 0x6cc8d0 */
s16 g_dgFollowerHeading = 0;          /* 0x6cc8d6 */
u32 g_dgCarryOrbitAngle = 0;          /* 0x6cc8d8 */
u16 g_dgDancerCount = 0;              /* 0x6cc8dc */
s32 g_dgCarryLeg = 0;                 /* 0x6cc8e0 */
s32 g_dgMonolitheFound = 0;           /* 0x6cc8e4 */
Trajectory *g_dgTrajectory = 0;       /* 0x6cc8e8 */
extern "C" s16 g_sinTable4096[5122];
extern "C" const s16 *g_pCosTable;
extern Wolf *g_pWolf;
extern s32 g_dt;
s32 Vec3s_ManhattanDist(Vec3s *, Vec3s *), Vec3s_ManhattanDistXZ(Vec3s *, Vec3s *), Vec3s_DistXZ(Vec3s *, Vec3s *);
extern "C" s16 Math_RadiansToAngle4096(float);
void Camera_StartScripted(ScnObject *, Camera *, u16, u16, u16, Vec3s *, u16, u32, s32);
#define SDW_INLINE_ZONELIST_FINDCONTAINING_VEC3S 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_FINDCONTAINING_VEC3S
inline ZoneList *GetZones(u8 index)
{
    return &g_waterZones[index];
}
/* BYTES(temp, inferred): the offset and the result go through the read struct because the original's expansion keeps both in stack slots */
#define SDW_INLINE_FREE_PROPU32_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_PROPU32_VOID_U32
inline s32 StepSign(s16 value)
{
    return value >= 0 ? 1 : -1;
}
#define COPY_VEC(dst, src) \
    (dst).x = (src).x;     \
    (dst).y = (src).y;     \
    (dst).z = (src).z

void DancingGhost::ReleasePartnerWait()
{
    if (steps[0].partner) {
        steps[0].partner->ReleasePartnerWait();
        if (steps[0].partner->steps[0].step == DG_STEP_STAND)
            steps[0].partner->steps[0].step = DG_STEP_WATCH_WOLF;
    }
}
inline void DancingGhost::CopyStep(u16 to, u16 from)
{
    if (to < 6 && from < 6)
        memcpy(&steps[to], &steps[from], 20);
}
inline void DancingGhost::ClearStep(u16 slot)
{
    if (slot < 6) {
        steps[slot].step = DG_STEP_WATCH_WOLF;
        steps[slot].partner = 0;
        steps[slot].argument = 0;
        COPY_VEC(steps[slot].point, g_dgDefaultPos);
        if (queueCount)
            queueCount--;
    }
}
void DancingGhost::PopStep()
{
    u16 i;
    if (queueCount) {
        if (steps[0].step == DG_STEP_STAND)
            return;
        if (steps[0].partner)
            steps[0].partner->PopStep();
        for (i = 0; i < (u16)(queueCount - 1); i++)
            CopyStep(i, i + 1);
        ClearStep(queueCount - 1);
    }
    NEW(this) = 1;
    DONE(this) = 0;
}
s32 DancingGhost::PushStep(u16 step, DancingGhost *partner, Vec3s point, u32 argument)
{
    if (queueCount >= 6)
        return 0;
    if (partner && !partner->PushStep(DG_STEP_STAND, 0, g_dgDefaultPos, 0))
        return 0;
    steps[queueCount].step = step;
    steps[queueCount].partner = partner;
    steps[queueCount].argument = argument;
    COPY_VEC(steps[queueCount].point, point);
    if (!queueCount) {
        NEW(this) = 1;
        DONE(this) = 0;
    }
    queueCount++;
    SetUpdateMode(SCN_UPD_ALWAYS);
    return 1;
}
/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets */
void DancingGhost::PostLoadInit()
{
    struct Work {
        ScnObject **scan;
        ScnObject *object;
        u16 unused, index;
        void *properties;
    } w;
    w.scan = g_scnObjects;
    w.object = 0;
    w.properties = record;
    PREV(this).x = 0;
    PREV(this).y = 0;
    PREV(this).z = 0;
    SOUND(this) = 0;
    HOME(this) = pos;
    ROT(this) = rot;
    if (!danceFlags.tambourine) {
        CHAT(this) = 0;
        CHAT(this) = Scn_GetPropBox(w.properties, 0);
    }
    if (!danceFlags.tambourine) {
        if (!g_dgTrajectory)
            g_dgTrajectory = Scn_GetPropTrajectory(w.properties, 8);
        TrajFollower_Init(&traj, g_dgTrajectory, 500, 0x800, 1, 1, 50);
    }
    if (!g_dgMonolitheFound) {
        for (w.scan = g_scnObjects, w.index = 0; w.index < g_scnObjectCount; w.index++, w.scan++) {
            w.object = *w.scan;
            if (w.object->GetClassId() == CLASSID_MONOLITHE) {
                COPY_VEC(g_dgMonolithePos, w.object->pos);
                g_dgMonolitheFound = 1;
            }
        }
    }
    EnableBoxCollide(0);
    SetUpdateMode(SCN_UPD_ALWAYS);
}
inline void DancingGhost::InitializeQueueInline()
{
    u16 index;
    for (index = 0; index < 6; index++) {
        steps[index].step = DG_STEP_WATCH_WOLF;
        steps[index].partner = 0;
        steps[index].argument = 0;
        COPY_VEC(steps[index].point, g_dgDefaultPos);
    }
    queueCount = 0;
    ELAPSED(this) = 0;
    NEW(this) = 1;
    DONE(this) = 0;
}
/* BYTES(slot-scope, inferred): the nested block(s) only order the frame: their locals are allocated after the enclosing scope's */
void DancingGhost::Reset()
{
    if (!InWorld())
        return;
    {
        StopSound(SOUND(this));
        SOUND(this) = 0;
        PREV(this).x = 0;
        PREV(this).y = 0;
        PREV(this).z = 0;
        rot = ROT(this);
        SetPosition(&HOME(this));
        InitializeQueueInline();
    }
}
void DancingGhost::Update()
{
    if (GetZones(ZONE_SHADOW)->FindContaining(&pos)) {
        tintColor = 0;
        tintAmount = 0x600;
        EnableTint(1);
    } else {
        tintColor = 0;
        tintAmount = 0;
        EnableTint(0);
    }
    UpdateState();
    AdvanceAnim();
}
void DancingGhost::Despawn()
{
    if (SOUND(this)) {
        StopSound(SOUND(this));
        SOUND(this) = 0;
    }
    RemoveFromWorld();
}
s32 DancingGhost::HandleMessage(ScnObject *sender, u32 msg, void *arg)
{
    if (sender) {
        switch (sender->GetClassId()) {
            case CLASSID_WOLF:
                switch (msg) {
                    case MSG_FREEZE:
                        return 1;
                    case MSG_DRAW_PROMPT:
                        return 0;
                    case MSG_GRABBER_CAMERA:
                        if (steps[0].step == DG_STEP_CARRY_WOLF)
                            return 1;
                        return 0;
                }
        }
    }
    return 0;
}
/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets; unused0 fill gaps */
s32 DancingGhost::StepToward(Vec3s *target, u16 speed, s32 arriveDistance, s32 smooth)
{
    struct Work {
        Vec3s delta;
        u16 unused0;
        Vec3s change;
        u16 unused1, unused2, frames;
        s32 distance;
    } w;
    w.delta.x = 0;
    w.delta.y = 0;
    w.delta.z = 0;
    w.frames = 1;
    w.distance = Vec3s_ManhattanDist(target, &pos);
    w.change.x = 0;
    w.change.y = 0;
    w.change.z = 0;
    if (w.distance > arriveDistance) {
        w.frames = ((w.distance << 12) / (speed * g_dt)) | 1;
        w.delta.x = target->x - pos.x;
        w.delta.y = target->y - pos.y;
        w.delta.z = target->z - pos.z;
        w.delta.x /= (s16)w.frames;
        w.delta.y /= (s16)w.frames;
        w.delta.z /= (s16)w.frames;
        if (smooth) {
            w.change.x = w.delta.x - PREV(this).x;
            w.change.y = w.delta.y - PREV(this).y;
            w.change.z = w.delta.z - PREV(this).z;
            w.delta.x -= w.change.x;
            w.delta.x += (s16)StepSign(w.change.x);
            w.delta.y -= w.change.y;
            w.delta.y += (s16)StepSign(w.change.y);
            w.delta.z -= w.change.z;
            w.delta.z += (s16)StepSign(w.change.z);
        }
        Translate(&w.delta);
        FacePoint(target);
        COPY_VEC(PREV(this), w.delta);
        return 0;
    }
    FacePoint(&g_pWolf->pos);
    return 1;
}

/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets; unused0, unused1, unused2, unused4 fill gaps */
s32 DancingGhost::CarryStep(DancingGhost *other, Vec3s *target, Vec3s *extra, u16 speed, s32 arriveDistance)
{
    struct Work {
        Vec3s delta;
        u16 unused0;
        Vec3s change;
        u16 unused1;
        s32 angle, arrived;
        Vec3s orbit;
        u16 unused2;
        s32 distance;
        u16 unused3, frames;
        Vec3s unusedVector;
        u16 unused4;
    } w;
    w.unusedVector.x = 0;
    w.unusedVector.y = 0;
    w.unusedVector.z = 0;
    w.delta.x = 0;
    w.delta.y = 0;
    w.delta.z = 0;
    w.orbit.x = 0;
    w.orbit.y = 0;
    w.orbit.z = 0;
    w.change.x = 0;
    w.change.y = 0;
    w.change.z = 0;
    w.frames = 1;
    if (extra->x || extra->z) {
        g_dgCarryPos.x += extra->x;
        g_dgCarryPos.y += extra->y;
        g_dgCarryPos.z += extra->z;
        Translate(extra);
        other->Translate(extra);
        g_pWolf->Translate(extra);
        if (g_dgCarryOrbitAngle > 0xfff) {
            w.angle = (s16)(Math_RadiansToAngle4096((float)atan2((double)g_dgCarryPos.x - g_dgMonolithePos.x,
                                                                 (double)g_dgCarryPos.z - g_dgMonolithePos.z)) &
                            0xfff);
            g_dgCarryOrbitAngle = (s16)((w.angle + 0x400) & 0xfff);
        }
        w.orbit.x = (g_sinTable4096[g_dgCarryOrbitAngle] * 40) / 4096;
        w.orbit.z = (g_pCosTable[g_dgCarryOrbitAngle] * 40) / 4096;
        if (w.orbit.x * w.delta.x + w.orbit.y * w.delta.y + w.orbit.z * w.delta.z < 0) {
            w.orbit.x = -w.orbit.x;
            w.orbit.z = -w.orbit.z;
        }
    }
    w.distance = Vec3s_ManhattanDist(target, &g_dgCarryPos);
    if (w.distance > arriveDistance)
        w.arrived = 0;
    else
        w.arrived = 1;
    w.frames = ((w.distance << 12) / (speed * g_dt)) | 1;
    w.delta.x = target->x - g_dgCarryPos.x;
    w.delta.y = target->y - g_dgCarryPos.y;
    w.delta.z = target->z - g_dgCarryPos.z;
    w.delta.x /= (s16)w.frames;
    w.delta.y /= (s16)w.frames;
    w.delta.z /= (s16)w.frames;
    w.delta.x += w.orbit.x;
    w.delta.y += w.orbit.y;
    w.delta.z += w.orbit.z;
    w.change.x = w.delta.x - PREV(this).x;
    w.change.y = w.delta.y - PREV(this).y;
    w.change.z = w.delta.z - PREV(this).z;
    w.delta.x -= w.change.x;
    w.delta.y -= w.change.y;
    w.delta.z -= w.change.z;
    if (w.change.x)
        w.delta.x = PREV(this).x + StepSign(w.change.x);
    if (w.change.y)
        w.delta.y = PREV(this).y + StepSign(w.change.y);
    if (w.change.z)
        w.delta.z = PREV(this).z + StepSign(w.change.z);
    if (g_dgCarryPos.y > QueryGroundY(&g_dgCarryPos, 1) && w.delta.y > 0)
        w.delta.y = 0;
    g_dgCarryPos.x += w.delta.x;
    g_dgCarryPos.y += w.delta.y;
    g_dgCarryPos.z += w.delta.z;
    Translate(&w.delta);
    other->Translate(&w.delta);
    COPY_VEC(PREV(this), w.delta);
    COPY_VEC(PREV(other), w.delta);
    g_pWolf->SetPosition(&g_dgCarryPos);
    g_pGhostHalo->HandleMessage(this, MSG_HALO_DRAW_AT, &g_dgCarryPos);
    return w.arrived;
}

/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets; unused3, unused4, unused5 fill gaps */
s32 DancingGhost::UpdateCarry()
{
    struct Work {
        u16 unused0, radius;
        Vec3s cameraOffset;
        u16 unused1, unused2;
        s16 angle;
        s32 monolitheDistance, distanceA;
        Vec3s eye;
        u16 unused3;
        s32 distanceB, done;
        Vec3s target;
        u16 unused4;
        Vec3s extra;
        u16 unused5;
    } w;
    w.distanceA = Vec3s_ManhattanDistXZ(&g_dgCarryPos, &g_dgCarryA);
    w.distanceB = Vec3s_ManhattanDistXZ(&g_dgCarryPos, &g_dgCarryB);
    w.monolitheDistance = -1;
    w.done = 0;
    w.cameraOffset.x = 400;
    w.cameraOffset.y = 600;
    w.cameraOffset.z = 400;
    w.extra.x = 0;
    w.extra.y = 0;
    w.extra.z = 0;
    w.radius = 700;
    w.angle = 0;
    switch (PHASE(this)) {
        case 0:
            COPY_VEC(w.target, g_dgCarryB);
            if (w.distanceA > 500 && steps[0].argument)
                w.target.y -= 400;
            else
                w.target.y -= 300;
            if (CarryStep(steps[0].partner, &w.target, &w.extra, 900, 60)) {
                g_dgCarryOrbitAngle = -1;
                PHASE(this) = 1;
            }
            break;
        case 1:
            if (g_dgMonolitheFound) {
                w.monolitheDistance = Vec3s_DistXZ(&g_dgCarryPos, &g_dgMonolithePos);
                if (w.monolitheDistance < w.radius) {
                    w.angle = Math_RadiansToAngle4096((float)atan2((double)g_dgCarryPos.x - g_dgMonolithePos.x,
                                                                   (double)g_dgCarryPos.z - g_dgMonolithePos.z)) &
                              0xfff;
                    w.extra.x = ((w.radius - w.monolitheDistance) * g_sinTable4096[w.angle]) / 4096;
                    w.extra.z = ((w.radius - w.monolitheDistance) * g_pCosTable[w.angle]) / 4096;
                }
            }
            COPY_VEC(w.target, g_dgCarryA);
            if (w.distanceA > 500)
                w.target.y -= 300;
            if (CarryStep(steps[0].partner, &w.target, &w.extra, 900, 30))
                w.done = 1;
    }
    if (!g_dgCarryLeg && (w.distanceA < 2000 || w.distanceA < w.distanceB))
        g_dgCarryLeg = 1;
    if (!g_dgCarryLeg) {
        COPY_VEC(w.eye, g_dgCarryB);
    } else {
        COPY_VEC(w.eye, g_dgCarryA);
    }
    if (steps[0].argument > 2) {
        w.cameraOffset.x = -w.cameraOffset.x;
        w.cameraOffset.z = -w.cameraOffset.z;
    }
    w.eye.x -= w.cameraOffset.x;
    w.eye.y -= w.cameraOffset.y;
    w.eye.z -= w.cameraOffset.z;
    StartDanceCamera(&w.eye, &pos);
    return w.done;
}

/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets; unused fill gaps */
void DancingGhost::StartDanceCamera(Vec3s *eye, Vec3s *target)
{
    struct Work {
        s32 xSq, ySq, zSq;
        Vec3s angles;
        u16 unused;
        s32 dx, dy, dz;
    } w;
    w.dx = target->x - eye->x;
    w.dy = target->y - eye->y;
    w.dz = target->z - eye->z;
    w.xSq = w.dx * w.dx;
    w.ySq = w.dy * w.dy;
    w.zSq = w.dz * w.dz;
    w.angles.x = Math_RadiansToAngle4096((float)atan2(w.dy, (s32)sqrt((double)w.xSq + w.zSq))) & 0xfff;
    w.angles.y = Math_RadiansToAngle4096((float)atan2(-w.dx, w.dz)) & 0xfff;
    w.angles.z = 0;
    Camera_StartScripted(this, &g_camera, w.angles.x, w.angles.y, w.angles.z, eye, 800, CAMSCR_BLEND_OUT, 4096);
}
/* BYTES(dead-code): unused is zeroed and never read, as in the original */
ScnObject *DancingGhost_Create(void *record)
{
    DancingGhost *object = 0;
    Vec3s unused = {0, 0, 0};
    object = new DancingGhost;
    u16 alt = 0xc0;
    /* cast kept: InitWithAltModels returns the object it was called on, as a ScnObject * */
    object = (DancingGhost *)object->InitWithAltModels(record, &object->model, 1, &alt, &object->altTambourine);
    object->InitializeQueueInline();
    g_dgTrajectory = 0;
    object->danceFlags.tambourine = PropU32(record, 4);
    if (object->danceFlags.tambourine)
        object->SwapModel(&object->altTambourine);
    else
        object->SwapModel(&object->model);
    return object;
}

extern TextScroll g_textScroll;

u8 Dialogue_Say(const char *, s32, ScnObject *, u32), Dialogue_Show(void *, s32);
void Dialogue_Reset(), Camera_ReleaseScripted(ScnObject *), Vec3s_ScaleByDt(const Vec3s *, Vec3s *);
s32 Rand_Bounded(s32), Rand_Range(s32, s32);
#define APHASE(o) ((o)->animPhase)
/* 0x4429a0 - a COMDAT after the object's main .text in the original: an inline member defined after its only caller
 * (Update 0x441acd), which /Ob1 cannot expand there, so it is emitted out of line. */
/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets; unused0 fill gaps */
/* BYTES(layout): inline member defined after its caller so it is emitted as the COMDAT after the main .text (0x4429a0) */
inline void DancingGhost::UpdateState()
{
    u16 partnerAnim1, selfAnim9;
    s32 nearestDistance6;
    struct Work {
        DancingGhost **array, *candidate;
        s32 distance;
        Vec3s eye;
        u16 unused0;
        Vec3s selfTarget, partnerTarget, leftOffset, rightOffset;
        u16 unused1, index;
        s32 selfArrived, partnerArrived;
    } w;
    w.leftOffset.x = 150;
    w.leftOffset.y = 0;
    w.leftOffset.z = 0;
    w.rightOffset.x = -150;
    w.rightOffset.y = 0;
    w.rightOffset.z = 0;
    w.selfArrived = 0;
    w.partnerArrived = 0;
    w.index = 0;
    if (NEW(this))
        ELAPSED(this) = 0;
    else if (!Game_IsPaused() && !Map_IsOpen())
        ELAPSED(this) = (g_dt * 1000 >> 12) + ELAPSED(this);
    switch (steps[0].step) {
        case DG_STEP_WATCH_WOLF:
            if (NEW(this)) {
                if (danceFlags.tambourine)
                    PlayAnim(AFANTO02_ANIM_STAND, 1, 1);
                else
                    PlayAnim(AFANTO02_ANIM_STAND, 1, 1);
                SetUpdateMode((u8)SCN_UPD_NORMAL);
            }
            FacePoint(&g_pWolf->pos);
            break;
        case DG_STEP_STAND:
            if (NEW(this))
                PlayAnim(AFANTO02_ANIM_STAND, 1, 1);
            break;
        case DG_STEP_ESCORT:
            if (NEW(this)) {
                PlayAnim(AFANTO02_ANIM_WALK, 1, 1);
                steps[0].partner->PlayAnim(AFANTO02_ANIM_WALK, 1, 1);
            }
            w.selfTarget.x = g_pWolf->pos.x + w.leftOffset.x;
            w.selfTarget.y = g_pWolf->pos.y + w.leftOffset.y;
            w.selfTarget.z = g_pWolf->pos.z + w.leftOffset.z;
            w.partnerTarget.x = g_pWolf->pos.x + w.rightOffset.x;
            w.partnerTarget.y = g_pWolf->pos.y + w.rightOffset.y;
            w.partnerTarget.z = g_pWolf->pos.z + w.rightOffset.z;
            w.selfArrived = StepToward(&w.selfTarget, 1200, 30, 0);
            w.partnerArrived = steps[0].partner->StepToward(&w.partnerTarget, 1200, 30, 0);
            if (Vec3s_ManhattanDistXZ(&g_pWolf->pos, &pos) > 500) {
                COPY_VEC(w.eye, pos);
                w.eye.y -= 250;
                StartDanceCamera(&w.eye, &g_pWolf->pos);
            } else if (g_camMode)
                Camera_ReleaseScripted(this);
            if ((w.selfArrived && w.partnerArrived) || ELAPSED(this) > 20000) {
                COPY_VEC(g_dgCarryPos, g_pWolf->pos);
                COPY_VEC(g_dgCarryB, g_pWolf->pos);
                PREV(this).x = PREV(this).x >> 2;
                PREV(this).y = 0;
                PREV(this).z = PREV(this).z >> 2;
                ReleasePartnerWait();
                DONE(this) = 1;
            }
            break;
        case DG_STEP_NOP:
            DONE(this) = 1;
            break;
        case DG_STEP_FAIL_LINE_1:
            if (NEW(this)) {
                FacePoint(&g_pWolf->pos);
                PlayAnim(AFANTO02_ANIM_TALK2, 1, 1);
            }
            if (!Dialogue_Say(g_dgFailedLines[1], g_dgFailedVoices[0], this, 1) ||
                (ELAPSED(this) > 20000 && !g_textScroll.flagBits.open))
                DONE(this) = 1;
            break;
        case DG_STEP_FAIL_LINE_2:
            if (NEW(this)) {
                FacePoint(&g_pWolf->pos);
                PlayAnim(AFANTO02_ANIM_TALK2, 1, 1);
            }
            if (!Dialogue_Say(g_dgFailedLines[2], g_dgFailedVoices[1], this, 1) ||
                (ELAPSED(this) > 20000 && !g_textScroll.flagBits.open))
                DONE(this) = 1;
            break;
        case DG_STEP_FAIL_LINE_3:
            if (NEW(this)) {
                FacePoint(&g_pWolf->pos);
                PlayAnim(AFANTO02_ANIM_TALK2, 1, 1);
            }
            if (!Dialogue_Say(g_dgFailedLines[3], g_dgFailedVoices[2], this, 1) ||
                (ELAPSED(this) > 20000 && !g_textScroll.flagBits.open))
                DONE(this) = 1;
            break;
        case DG_STEP_KILL_WOLF:
            if (AnimFlags(ANIM_F_FINISHED) && g_pWolf->HandleMessage(this, MSG_WOLF_IS_DEAD, 0))
                DONE(this) = 1;
            else {
                if (NEW(this))
                    PlayAnim(AFANTO02_ANIM_STAND1, 0, 1);
                if (AnimFlags(ANIM_F_FINISHED))
                    PlayAnim(AFANTO02_ANIM_KILL, 0, 1);
                if (ELAPSED(this) > 20000) {
                    g_pWolf->HandleMessage(this, MSG_KILL, (void *)KILL_ZAP); /* cast kept: arg carries a number */
                    DONE(this) = 1;
                } else if (ELAPSED(this) > 4300)
                    g_pFireBall->HandleMessage(this, MSG_FIREBALL_STRIKE, g_pWolf);
            }
            break;
        case DG_STEP_CARRY_WOLF:
            if (NEW(this)) {
                PlayAnim(AFANTO02_ANIM_HOO, 1, 1);
                steps[0].partner->PlayAnim(AFANTO02_ANIM_HOO, 1, 1);
                COPY_VEC(g_dgCarryA, steps[0].point);
                w.leftOffset.x = g_dgCarryA.x - g_dgCarryB.x;
                w.leftOffset.y = g_dgCarryA.y - g_dgCarryB.y;
                w.leftOffset.z = g_dgCarryA.z - g_dgCarryB.z;
                PHASE(this) = 0;
                g_dgCarryLeg = 0;
                g_pWolf->HandleMessage(this, MSG_WOLF_GHOST_CARRY, (void *)1); /* cast kept: arg carries a number */
            }
            DONE(this) = UpdateCarry();
            DONE(steps[0].partner) = DONE(this);
            if (ELAPSED(this) > 20000) {
                g_pWolf->SetPosition(&g_dgCarryA);
                DONE(this) = 1;
            }
            if (DONE(this)) {
                g_pWolf->HandleMessage(this, MSG_WOLF_GHOST_CARRY, 0);
                ReleasePartnerWait();
            }
            break;
        case DG_STEP_FOLLOW_PATH:
            if (NEW(this)) {
                PlayAnim(AFANTO02_ANIM_WALK, 1, 1);
                SetPathIndex((s16)steps[0].argument);
            }
            if (GetPathIndex() == (s16)steps[0].argument) {
                StepPath(&g_dgFollowerVelocity, &g_dgFollowerHeading);
                SetFacing(g_dgFollowerHeading);
                Vec3s_ScaleByDt(&g_dgFollowerVelocity, &g_dgFollowerStep);
                Translate(&g_dgFollowerStep);
            } else
                DONE(this) = 1;
            if (ELAPSED(this) > 20000)
                DONE(this) = 1;
            break;
        case DG_STEP_WALK_TO:
            if (NEW(this))
                PlayAnim(AFANTO02_ANIM_WALK, 1, 1);
            if (StepToward(&steps[0].point, 500, steps[0].argument, 0) || ELAPSED(this) > 20000) {
                SetPosition(&steps[0].point);
                DONE(this) = 1;
            }
            break;
        case DG_STEP_POSE_STANDV2:
            if (NEW(this))
                PlayAnim(AFANTO02_ANIM_STANDV2, 1, 1);
            if (queueCount > 1)
                DONE(this) = 1;
            break;
        case DG_STEP_POSE_KILL:
            if (NEW(this))
                PlayAnim(AFANTO02_ANIM_KILL, 1, 1);
            if (queueCount > 1)
                DONE(this) = 1;
            break;
        case DG_STEP_DANCE:
            if (NEW(this)) {
                APHASE(this) = 0;
                PlayAnim((u16)steps[0].argument, 0, 1);
            }
            if (AnimFlags(ANIM_F_FINISHED)) {
                switch (APHASE(this)) {
                    case 0:
                        APHASE(this)++;
                        PlayAnim(AFANTO02_ANIM_DANCE5, 1, 1);
                        break;
                }
            }
            if (queueCount > 1 || ELAPSED(this) > 20000)
                DONE(this) = 1;
            break;
        case DG_STEP_APPLAUD:
            if (NEW(this))
                PlayAnim(AFANTO02_ANIM_APPLAUSE, 1, 0);
            FacePoint(&g_pWolf->pos);
            if (ELAPSED(this) > 2000)
                DONE(this) = 1;
            break;
        case DG_STEP_FINAL_LINE:
            if (NEW(this)) {
                PlayAnim(AFANTO02_ANIM_STAND, 1, 1);
                g_pWolf->HandleMessage(this, MSG_FREEZE, 0);
            }
            if (!Dialogue_Show((void *)g_dgFailedLines[4], 1)) { /* cast kept: Dialogue_Show takes the text as void * */
                g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0);
                Dialogue_Reset();
                DONE(this) = 1;
            }
            break;
        case DG_STEP_PAIR_DANCE:
            if (NEW(this)) {
                danceFlags.paired = 0;
                PlayAnim(AFANTO02_ANIM_TALK, 1, 1);
                SetUpdateMode((u8)SCN_UPD_NORMAL);
            }
            if (!danceFlags.paired) {
                w.array = (DancingGhost **)steps[0].argument; /* cast kept: this step's argument word is an address */
                w.candidate = 0;
                nearestDistance6 = 0x7fffffff;
                w.distance = 0x7fffffff;
                for (w.index = 0; w.index < g_dgDancerCount; w.index++) {
                    w.candidate = w.array[w.index];
                    if (w.candidate == this)
                        continue;
                    w.distance = Vec3s_ManhattanDistXZ(&pos, &w.candidate->pos);
                    if (w.distance < nearestDistance6) {
                        nearestDistance6 = w.distance;
                        steps[0].partner = w.candidate;
                    }
                }
                if (steps[0].partner && steps[0].partner->steps[0].step == DG_STEP_PAIR_DANCE) {
                    danceFlags.paired = 1;
                    steps[0].partner->danceFlags.paired = 1;
                    danceFlags.leader = 1;
                    steps[0].partner->danceFlags.leader = 0;
                    steps[0].partner->steps[0].partner = this;
                }
            }
            if (steps[0].partner) {
                if (danceFlags.paired && danceFlags.leader && AnimFlags(ANIM_F_FINISHED)) {
                    partnerAnim1 = Rand_Bounded(29) / 10 + AFANTO02_ANIM_TALK; /* TALK, TALK2 or TALK3 */
                    selfAnim9 = (u16)Rand_Range(0, 1);
                    if (selfAnim9)
                        selfAnim9 = AFANTO02_ANIM_STAND;
                    else
                        selfAnim9 = AFANTO02_ANIM_TALK4;
                    danceFlags.leader = 0;
                    steps[0].partner->danceFlags.leader = 1;
                    PlayAnim(selfAnim9, 1, 1);
                    steps[0].partner->PlayAnim(partnerAnim1, 1, 1);
                    StopSound(SOUND(this));
                    SOUND(this) = 0;
                    SOUND(steps[0].partner) =
                        steps[0].partner->PlaySound(SND_DANCE_STEP, 255, SNDF_LOOP | SNDF_POSITIONAL, 4096);
                }
                FacePoint(&steps[0].partner->pos);
            }
            if (queueCount > 1) {
                if (danceFlags.leader) {
                    StopSound(SOUND(this));
                    SOUND(this) = 0;
                    DONE(this) = 1;
                } else {
                    steps[0].partner->steps[0].partner = 0;
                    steps[0].partner->PopStep();
                }
            }
    }
    NEW(this) = 0;
    if (DONE(this))
        PopStep();
}
