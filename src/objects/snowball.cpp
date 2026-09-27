/*
 * Snowball (class 75, vtable 0x576cb4, sizeof 0x94) - the big snowball of Level 6 (disc Lvl-07). Ralph pushes it
 * (msg 0xA, the push step's displacement) and, while it rolls inside its ZONE boxes, it grows: the target scale goes
 * from 0x100 to 0x333 (0.25 to 0.8) over 20 s of rolling, and each push that finds room for the bigger ball (ground
 * under it within reach, still in the zone) applies it, resizing the model's collision box. It has a dome-shaped top
 * (msg 0xD answers the ground query with Box_GroundQueryDome), so Ralph can stand on it. SheepD3D.exe 0x4f5770-0x4f615e:
 * PostLoadInit, Update, HandleMessage, Render, Reset (empty), the resize / move / gravity steps and the class factory.
 *
 * States (+0x62): 0 resting, 1 being pushed (rolling sound 0x7b), 2 falling (after a frame without a push).
 * Devices that only pin the original code generation: the inline helpers below (their names are not recovered; each
 * one is here because its expansion gives the original's stack temporaries), SDW_PROP (offsetof: as a u32 argument it
 * is not a plain constant, so the property inline gets a temp for it) and the local names (under /Od a local's slot
 * follows from a hash of its name, tools/vc6_locals.py).
 */
/* BYTES: cast. */
/* BYTES(cast): SDW_PROP (offsetof) argument: offsetof as a u32 argument: not a plain constant, so the inline gets the original's temp */

#define SDW_MEMBERS_ScnObject                                                                                                                                \
    static void *operator new(u32 size); /* 0x50d5f4 Scenaric_Alloc: every class factory's `new` (level heap) */                                             \
    CollBox *GetFirstModelBox();                                                                                                                             \
    /* inline: the test materialised (neg/sbb/neg, 0x4f5c9f) */                                                                                              \
    s32 NoDistCull()                                                                                                                                         \
    {                                                                                                                                                        \
        return (flags & SCN_OF_NO_DIST_CULL) != 0;                                                                                                           \
    } /* inline: likewise (0x4f5c0c) */ /* inline: beyond 3000 units from the camera (dist2 9000000) unless exempt; the && goes through a temp (0x4f5c25) */ \
    s32 IsFar()                                                                                                                                              \
    {                                                                                                                                                        \
        return !NoDistCull() && camDist2 > 0x895440;                                                                                                         \
    }


#include "sdw_types.h"
#include "sdw_enums.h"
#include "../engine/id_list.h"
#include "../engine/fixed_math.h"
#include "world_draw.h"
#include "../engine/scn_tools.h"
#include "../engine/shadow.h"
#include "../engine/sound_mgr.h"
#include "../engine/maths.h"
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#define SDW_INLINE_SCNOBJECT_ENABLEBOXCOLLIDE_S32 1
#define SDW_INLINE_SCNOBJECT_INSTFLAGS_U16 1
#define SDW_INLINE_SCNOBJECT_ISINWORLD 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_ENABLEBOXCOLLIDE_S32
#undef SDW_INLINE_SCNOBJECT_INSTFLAGS_U16
#undef SDW_INLINE_SCNOBJECT_ISINWORLD
#define SDW_INLINE_SHADOW_SETFLAG4_S32 1
#include "../engine/shadow_inlines.h"
#undef SDW_INLINE_SHADOW_SETFLAG4_S32
#define SDW_INLINE_ZONELIST_RESOLVE_U32 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_RESOLVE_U32
#include "scenaric_props.h"

/* cast kept: offsetof written out (no CRT header); as a u32 argument it is not a plain constant, so the inline gets a
 * temp */
#define SDW_PROP(T, f) ((u32) & ((T *)0)->f)
#define SDW_ABS(v) ((v) >= 0 ? (v) : -(v))

#include "../sdk/crt.h"
extern "C" s32 Coll_BoxGroundQuery(CollBox *box, s32 *outY, ScnObject *self, u8 mode,
                                   ScnObject **outHitObj);                /* 0x51a9de */
u16 Sound_Play(u16 soundId, void *owner, u16 volume, u8 flags, s32 rate); /* 0x5491b8 */

extern Wolf *g_pWolf; /* 0x6cf310 */
extern s32 g_dt;      /* 0x71b300 */

/* A designer property of the WAR record: the dword at record + 0x14 + offset (the offset is a stack temp, 0x4f57ad). */
#define SDW_INLINE_FREE_SCN_GETPROPU32_U16_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_SCN_GETPROPU32_U16_U32

/* The model's first collision box, or NULL: the list pointer is a temp (0x4f57eb); ScnObject_GetFirstModelBox 0x4c1ec0
 * is an out-of-line copy (src/game/wolf.h). */
inline CollBox *ScnObject::GetFirstModelBox()
{
    ModelBoxList *list = inst_model->boxes;
    if (list)
        return list->boxes;
    return 0;
}

#define SDW_INLINE_ZONELIST_FINDCONTAININGXZ_VEC3S 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_FINDCONTAININGXZ_VEC3S

/* The stop of a sound this object owns: the handle is a stack temp (0x4f5964). */
inline void Sound_StopOwned(u16 handle, ScnObject *owner)
{
    Sound_Stop(handle, owner);
}

/* 0x4f5770 - vtable +0x00: the roll matrix from the placed rotation, the ZONE list, the radius from model box 0 at
 * scale 1.0, weight 120; then TryResize shrinks it to its starting scale 0x100 in place, and it snaps to the
 * ground. */
void Snowball::PostLoadInit()
{
    CollBox *box;
    u32 zoneId;
    u16 *props;
    props = record;
    state = SNOWBALL_ST_REST;
    rollSound = 0;
    Mat34s_FromEulerScaled(&rot, &rollMatrix, 0);
    zoneId = Scn_GetPropU32(props, SDW_PROP(SnowballProps, ZONE));
    zone.Resolve(zoneId);
    box = GetFirstModelBox();
    baseRadius = (box->max.x - box->min.x) >> 1;
    radius = baseRadius;
    scale = 0x400;
    fallTime = 0;
    rollTime = 0;
    pushed = 0;
    weight = 0x78;
    shadow.SetFlag4(1);
    TryResize(0x100, g_pZeroVec3s);
    SnapToGround(1);
}

/* 0x4f58bb - vtable +0x04. Falling: gravity until the move reports floor contact, then resting. Pushed: a frame
 * without a push stops the rolling sound and lets it fall; otherwise the rolling sound keeps playing. */
void Snowball::Update()
{
    Vec3s vel;
    switch (state) {
        case SNOWBALL_ST_FALL:
            fallTime += g_dt;
            vel.x = 0;
            vel.y = 0;
            vel.z = 0;
            ApplyGravity(&vel);
            if (Move(&vel, 0) & COLL_FLOOR)
                state = SNOWBALL_ST_REST;
            break;
        case SNOWBALL_ST_ROLL:
            if (!pushed) {
                state = SNOWBALL_ST_FALL;
                if (rollSound) {
                    Sound_StopOwned(rollSound, this);
                    rollSound = 0;
                }
            } else {
                if (!rollSound)
                    rollSound = Sound_Play(SND_SNOWBALL_ROLL, this, 0xff,
                                           SNDF_LOOP | SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 0x1000);
                pushed = 0;
            }
            break;
    }
}

/* 0x4f59cc - vtable +0x10. Msg 2 (context query): CTX_PUSH (5) to Ralph while his feet are below the ball's centre.
 * Msg 0xA (push, arg = the pusher's displacement): the ball takes the horizontal part plus gravity; inside the zone it
 * tries to grow to the scale its roll time allows (else it just moves) and rolls its matrix, outside it only falls.
 * Msg 0xD (ground query): the dome of model box 0. */
s32 Snowball::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    s16 goal;
    Vec3s d;
    Vec3s *push;
    Vec3s at;
    switch (msgId) {
        case MSG_QUERY_ACTION:
            if (sender->GetClassId() == CLASSID_WOLF && sender->pos.y > pos.y)
                return CTX_PUSH;
            break;
        case MSG_PUSH:
            push = (Vec3s *)arg; /* cast kept: the message arg is a void *: what it carries depends on the message id */
            fallTime += g_dt;
            d.x = push->x;
            d.y = 0;
            d.z = push->z;
            ApplyGravity(&d);
            at.x = pos.x + d.x;
            at.y = pos.y + d.y;
            at.z = pos.z + d.z;
            if (!zone.count || zone.FindContainingXZ(&at)) {
                if (rollTime < 0x14000)
                    goal = rollTime * 0x233 / 0x14000 + 0x100;
                else
                    goal = 0x333;
                if (!TryResize(goal, &d))
                    Move(&d, COLL_FLOOR | COLL_FLOOR_EDGE);
                Matrix_RollByDisplacement(&rollMatrix, &d, radius);
            } else {
                d.x = d.z = 0;
                Move(&d, 0);
            }
            pushed = 1;
            state = SNOWBALL_ST_ROLL;
            return 1;
        case MSG_GROUND_QUERY:
            /* cast kept: the message arg is a void *: what it carries depends on the message id */
            return Box_GroundQueryDome((GroundQuery *)arg, GetFirstModelBox(), &pos);
    }
    return 0;
}

/* 0x4f5bbc - vtable +0x08: the roll matrix scaled by the current scale; far away (dist2 > 9000000) only the secondary
 * draw, an empty stub, so the ball is not drawn at all; otherwise the rigid draw, then the blob shadow when drawn and
 * in the world. */
void Snowball::Render(Camera *view)
{
    Mat34s mat;
    Vec3s scl;
    memcpy(&mat, &rollMatrix, sizeof mat);
    scl.x = scl.y = scl.z = scale;
    Mat34s_ApplyScale(&mat, &scl);
    if (IsFar()) {
        if (InstFlags(INST_F_HAS_SECONDARY))
            Instance_DrawSecondary_Stub(Inst(), view, 0, &mat);
    } else {
        Instance_DrawRigid(Inst(), view, 0, &mat);
        if (InstFlags(INST_F_DRAWN) && IsInWorld()) {
            Shadow_Update(&shadow, &pos, this);
            Shadow_Render(&shadow);
        }
    }
}

/* 0x4f5cd8 - vtable +0x14 (level restart): nothing. The ball keeps its place, its size and its roll time. */
void Snowball::Reset() {}

/* 0x4f5ce3 - try the ball at newScale after the displacement delta: the new radius r = baseRadius*newScale >> 10, the
 * centre moved by delta plus the change of radius on each moving axis; a box of radius r there, stretched down to
 * 32000, looks for the ground (with Ralph's box collision off). If the ground is within |r - radius| + 10 of the new
 * bottom and the new centre is in the zone, it applies it: shadow radius, model box 0 = +-r, the move, scale, radius,
 * fallTime = 0 and rollTime += g_dt. Returns whether the ball now has newScale. */
s32 Snowball::TryResize(s16 newScale, Vec3s *delta)
{
    s32 r;
    CollBox query;
    Vec3s d;
    Vec3s newPos;
    s32 ground;
    CollBox *modelBox;
    r = baseRadius * newScale >> 10;
    newPos = pos;
    if (delta->x)
        newPos.x += (s16)(delta->x + r - radius);
    if (delta->z)
        newPos.z += (s16)(delta->z + r - radius);
    query.flags = GetFirstModelBox()->flags;
    query.min.x = newPos.x - r;
    query.min.y = newPos.y - r;
    query.min.z = newPos.z - r;
    query.max.x = newPos.x + r;
    query.max.y = 0x7d00;
    query.max.z = newPos.z + r;
    g_pWolf->EnableBoxCollide(0);
    if (Coll_BoxGroundQuery(&query, &ground, this, CQ_STATIC | CQ_OBJECTS, 0)) {
        ground = ground - r - newPos.y;
        if (SDW_ABS(ground) <= SDW_ABS(r - radius) + 10) {
            d.x = delta->x;
            d.z = delta->z;
            d.y = ground;
            newPos.x = pos.x + d.x;
            newPos.y = pos.y + d.y;
            newPos.z = pos.z + d.z;
            if (!zone.count || zone.FindContainingXZ(&newPos)) {
                if (r <= 0xff)
                    shadow.radius = r;
                else
                    shadow.radius = 0xff;
                modelBox = GetFirstModelBox();
                modelBox->min.x = -r;
                modelBox->min.y = -r;
                modelBox->min.z = -r;
                modelBox->max.x = r;
                modelBox->max.y = r;
                modelBox->max.z = r;
                Translate(&d);
                scale = newScale;
                radius = r;
                fallTime = 0;
                rollTime += g_dt;
            }
        }
    }
    g_pWolf->EnableBoxCollide(1);
    return scale == newScale;
}

/* 0x4f6025 - collide-and-slide by delta (floor cutoff 0xb54, step 10), then commit it; floor contact (bit 0) ends the
 * fall. Returns the resolver's result bits. */
u16 Snowball::Move(Vec3s *delta, u16 resolveFlags)
{
    u16 result;
    ContactInfo ci;
    result = Collide_ResolveMove(delta, &ci, 0xb54, resolveFlags, 0, 0, 0xa, 0, 0);
    if (result & COLL_FLOOR)
        fallTime = 0;
    Translate(delta);
    return result;
}

/* 0x4f6081 - gravity: fall speed fallTime*4000 >> 12 (fallTime capped at 0x5000), at most 3000 units/s, added to
 * vel->y for this frame (speed * g_dt / 4096). */
void Snowball::ApplyGravity(Vec3s *vel)
{
    s32 speed;
    if (fallTime > 0x5000)
        fallTime = 0x5000;
    speed = fallTime * 4000 >> 12;
    if (speed > 3000)
        speed = 3000;
    vel->y += (s16)(speed * g_dt / 4096);
}

/* 0x4f60f1 - the class factory for CLASSID 75 "Snowball": new Snowball (the base vtables in turn, then Snowball's),
 * then ScnLogicShadowed_Init(record) through vtable slot +0x20. */
ScnObject *Snowball_Create(void *record)
{
    Snowball *obj = new Snowball;
    obj = (Snowball *)obj->Init(record); /* cast kept: Init returns the ScnObject * base of this object */
    return obj;
}
