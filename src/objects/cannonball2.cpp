/* PAL PC CannonBall2 0x4a4bc0-0x4a5710. The flag updates are the original read-modify-writes. */
/* BYTES: slot-scope. */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
#define SDW_MEMBERS_ScnObject        \
    static void *operator new(u32);  \
    void SetInstanceScale(u32 scale) \
    {                                \
        partHeight = scale >> 4;     \
    }                                \
    void AttachTo(ScnObject *, u8, Vec3s *, Vec3s *, u32, u32);


#include "sdw_classes.h"
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_INSTFLAGS_U16 1
#define SDW_INLINE_SCNOBJECT_SETCONTACTENABLED_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_INSTFLAGS_U16
#undef SDW_INLINE_SCNOBJECT_SETCONTACTENABLED_S32
#define SDW_INLINE_SHADOW_SETVISIBLE_S32 1
#include "../engine/shadow_inlines.h"
#undef SDW_INLINE_SHADOW_SETVISIBLE_S32
struct BallMoveMessage {
    Vec3s delta;
    u16 bit0 : 1;
    u16 bit1 : 1;
    u16 bit2 : 1;
    Vec3s velocity;
};
extern s32 g_dt;
extern u8 g_sharedScratch[];
#include "../engine/scenaric.h"
#include "../engine/scn_tools.h"
u16 Sound_Play(u16, void *, u16, u8, s32);
#define SDW_INLINE_FREE_ZONES_GET_U8 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_FREE_ZONES_GET_U8
#define SDW_INLINE_ZONELIST_CONTAINS_VEC3S 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_CONTAINS_VEC3S
#define BALL_ABS(v) ((v) >= 0 ? (v) : -(v))
/* BYTES(slot-scope, inferred): the nested block(s) only order the frame: their locals are allocated after the enclosing scope's */
u32 CannonBall2::Move(Vec3s *delta, ContactInfo *info, u16 slide, Vec3s *aux)
{
    u32 result;
    {
        BallMoveMessage modify;
        {
            Vec3s half;
            if (moveModifier) {
                modify.delta = *delta;
                modify.velocity = *aux;
                modify.bit0 = 0;
                modify.bit1 = 0;
                modify.bit2 = 0;
                moveModifier->HandleMessage(this, MSG_MODIFY_MOVE, &modify);
                *delta = modify.delta;
            }
            if (BALL_ABS(delta->x) <= 50 && BALL_ABS(delta->z) <= 50 && BALL_ABS(delta->y) <= 50)
                result = Collide_ResolveMove(delta, info, 0xb54, slide, 0, 0, 10, 0, 0);
            else {
                half.x = delta->x >> 1;
                half.y = delta->y >> 1;
                half.z = delta->z >> 1;
                delta->x -= half.x;
                delta->y -= half.y;
                delta->z -= half.z;
                result = Collide_ResolveMove(delta, info, 0xb54, slide, 0, 0, 10, 0, 0);
                delta->x += pos.x;
                delta->y += pos.y;
                delta->z += pos.z;
                result |= Collide_ResolveMove(&half, info, 0xb54, slide, delta, 0, 10, 0, 0);
                delta->x = delta->x + half.x - pos.x;
                delta->y = delta->y + half.y - pos.y;
                delta->z = delta->z + half.z - pos.z;
            }
        }
    }
    Translate(delta);
    return result;
}
/* BYTES(slot-scope, inferred): the nested block(s) only order the frame: their locals are allocated after the enclosing scope's */
u32 CannonBall2::StepFall(u16 slide)
{
    ContactInfo info;
    {
        s32 speed;
        {
            /* cast kept: g_sharedScratch is one scratch buffer that each user lays out its own way */
            Vec3s *velocity = (Vec3s *)g_sharedScratch;
            {
                u32 result;
                {
                    Vec3s delta;
                    velocity->x = velocity->z = 0;
                    speed = (timer * 2000) >> 12;
                    if (speed > 1000)
                        velocity->y = 1000;
                    else if (speed < 120)
                        velocity->y = 120;
                    else
                        velocity->y = (s16)speed;
                    Vec3s_ScaleByDt(velocity, &delta);
                    result = Move(&delta, &info, slide, velocity);
                    if (info.movableObj)
                        flags.movable = 1;
                    else
                        flags.movable = 0;
                    return result;
                }
            }
        }
    }
}
/* BYTES(slot-scope, inferred): the nested block(s) only order the frame: their locals are allocated after the enclosing scope's */
void CannonBall2::Update()
{
    Vec3s delta;
    {
        u16 slide;
        switch (state) {
            case CB2_ST_REST:
                timer = 0;
                if (moveModifier)
                    state = CB2_ST_SLIDE;
                break;
            case CB2_ST_SLIDE:
                if (flags.magnet) {
                    if (!flags.sound) {
                        Sound_Play(SND_BALL_MAGNET, this, 0xff, SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 0x1000);
                        flags.sound = 1;
                    }
                    flags.magnet = 0;
                } else
                    flags.sound = 0;
                timer += g_dt;
                if (timer > 0x1e000)
                    timer = 0x1e000;
                if (moveModifier)
                    slide = RESOLVE_SLIDE_ALL;
                else
                    slide = COLL_WALL;
                if ((StepFall(slide) & COLL_FLOOR) && !moveModifier) {
                    if (Zones_Get(ZONE_DEATH)->Contains(&pos)) {
                        state = CB2_ST_SINK;
                        flags.magnet = 0;
                        flags.sound = 0;
                        timer = 0;
                    } else if (!flags.movable) {
                        state = CB2_ST_REST;
                        flags.magnet = 0;
                        flags.sound = 0;
                    }
                }
                break;
            case CB2_ST_SINK:
                delta.x = delta.z = 0;
                delta.y = (g_dt * 100) >> 12;
                Translate(&delta);
                timer += g_dt;
                if (timer >= 0x1000)
                    Reset();
                break;
        }
    }
    AdvanceAnim();
}
/* BYTES(slot-scope, inferred): the nested block(s) only order the frame: their locals are allocated after the enclosing scope's */
s32 CannonBall2::HandleMessage(ScnObject *sender, u32 msg, void *arg)
{
    Vec3s offset;
    {
        Vec3s old;
        {
            ScnObject *parent;
            {
                u8 joint;
                {
                    BallMoveMessage *drop;
                    switch (msg) {
                        case MSG_QUERY_ACTION:
                            if (sender->GetClassId() == CLASSID_WOLF && pos.y <= sender->pos.y + 30 &&
                                pos.y >= sender->pos.y - 80)
                                return CTX_LIFT;
                            break;
                        case MSG_PICKUP:
                            parent = sender;
                            joint = (u8)(u32)arg; /* cast kept: arg carries the joint number */
                            if (sender->GetClassId() == CLASSID_MAGNET) {
                                old = pos;
                                offset.x = 15;
                                offset.z = 20;
                                offset.y = 75;
                                AttachTo(parent, joint, &offset, 0, 1, 0);
                                SetPosition(&old);
                            } else {
                                offset.x = offset.z = 0;
                                offset.y = 10;
                                AttachTo(parent, joint, &offset, 0, 1, 0);
                                SetContactEnabled(0);
                                shadow.SetVisible(0);
                            }
                            return 1;
                        case MSG_DROP:
                            drop = (BallMoveMessage *)arg; /* cast kept: MSG_DROP's arg is the drop record */
                            Detach();
                            if (!drop->bit1)
                                SetPosition(&drop->delta);
                            SetContactEnabled(1);
                            shadow.SetVisible(1);
                            state = CB2_ST_SLIDE;
                            return 1;
                        case MSG_QUERY_HELD_ACTION:
                            return HELD_THROWABLE;
                        case MSG_RIDER_ADD:
                            if (!moveModifier)
                                moveModifier = sender;
                            return 1;
                        case MSG_RIDER_REMOVE:
                            if (moveModifier == sender)
                                moveModifier = 0;
                            return 1;
                        case MSG_MAGNET_QUERY:
                            if (state != CB2_ST_SINK)
                                return 1;
                            break;
                        case MSG_SEESAW_TOUCH:
                        case MSG_LANDED:
                            if (!InstFlags(INST_F_ATTACHED))
                                state = CB2_ST_SLIDE;
                            break;
                        case MSG_MAGNET_CONTACT:
                            flags.magnet = 1;
                            break;
                    }
                }
            }
        }
    }
    return 0;
}
void CannonBall2::Render(Camera *view)
{
    RenderFacingCamera(view, 0, 0, 0);
}
void CannonBall2::Reset()
{
    SetPosition(&spawnPos);
    SetContactEnabled(1);
    shadow.SetVisible(1);
    state = CB2_ST_REST;
    timer = 0;
    flags.movable = 0;
    flags.magnet = 0;
    flags.sound = 0;
}
void CannonBall2::PostLoadInit()
{
    shadow.radius = 28;
    SetInstanceScale(0x120);
    SnapToGround(1);
    spawnPos = pos;
    state = CB2_ST_REST;
    timer = 0;
    moveModifier = 0;
    flags.movable = 0;
    flags.magnet = 0;
    flags.sound = 0;
}
ScnObject *CannonBall2_Create(void *record)
{
    ScnBody *object = new CannonBall2;
    object = object->Init(record, 0);
    return object;
}
