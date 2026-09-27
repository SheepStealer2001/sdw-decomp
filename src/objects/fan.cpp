/* T145 - original object Fan.cpp (guessed name).
 * Ranges: .text 0x4bb550-0x4bc142, .rdata 0x576010-0x576044 (g_fanStateAnims in main CONST, then the vtable).
 * g_fanStateAnims is defined here as const, so Fan::SetState reads it through a `const FanAnimEntry *entry` (same
 * code). */
/* BYTES: slot-scope. */
/* PAL PC Fan, 0x4bb550-0x4bc139. */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *, Animator *, u16, u32);

#define SDW_MEMBERS_ScnObject                                   \
    static void *operator new(u32);                             \
    void AttachTo(ScnObject *, u8, Vec3s *, Vec3s *, u32, u32); \
    CollBox *GetFirstModelBox();


#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETFACING 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETFACING
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_ISINWORLD 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_ISINWORLD
#define SDW_INLINE_SCNOBJECT_ISACTIVE 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_ISACTIVE
#define SDW_INLINE_SCNOBJECT_INSTFLAGS_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_INSTFLAGS_U16
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_ANIMID 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_ANIMID
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_SHADOW_SETVISIBLE_S32 1
#include "../engine/shadow_inlines.h"
#undef SDW_INLINE_SHADOW_SETVISIBLE_S32
struct FanAnimEntry {
    u16 animId;
    u16 flags;
};
/* 0x576010 .rdata (main CONST, ahead of the vtable): per FanState {animation id, flags: 1 hide shadow, 2 loop,
 * 4 blowing}. const, so it lands in .rdata; SetState's local pointer to it became a pointer to const accordingly. */
const FanAnimEntry g_fanStateAnims[4] = {{AVENTI01_ANIM_OBJET, 0},
                                         {AVENTI01_ANIM_LINK, FAN_AF_HIDE_SHADOW},
                                         {AVENTI01_ANIM_STANDV1B, FAN_AF_HIDE_SHADOW | FAN_AF_BLOWING},
                                         {AVENTI01_ANIM_STANDV1, FAN_AF_HIDE_SHADOW | FAN_AF_LOOP | FAN_AF_BLOWING}};
extern u32 g_gameTime;
u16 Sound_Play(u16, void *, u16, u8, s32);
#include "../engine/sound_mgr.h"
s32 ObjGrid_QueryBoxPoints(const CollBox *, ScnObject **);
#include "../sdk/crt.h"
extern "C" s16 Math_RadiansToAngle4096(float);
#define SDW_INLINE_SCNOBJECT_GETPARENT 1
#define SDW_INLINE_SCNOBJECT_ISSOUNDPLAYING_U16 1
#define SDW_INLINE_SCNOBJECT_STOPSOUNDHANDLE_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETPARENT
#undef SDW_INLINE_SCNOBJECT_ISSOUNDPLAYING_U16
#undef SDW_INLINE_SCNOBJECT_STOPSOUNDHANDLE_U16
inline CollBox *ScnObject::GetFirstModelBox()
{
    ModelBoxList *list = inst_model->boxes;
    if (list)
        return (CollBox *)list->boxes; /* cast kept: Box and CollBox are two views of the same 16-byte record */
    return 0;
}
#define AngleDifference(a, b) ((s16)((s16)(((a) - (b) + 0x800) & 0xfff) - 0x800))
#define FAN_ABS(x) ((x) >= 0 ? (x) : -(x))
#define SDW_INLINE_FREE_BOXOVERLAP4_S32_S32_S32_S32 1
#include "../engine/coll_box_inlines.h"
#undef SDW_INLINE_FREE_BOXOVERLAP4_S32_S32_S32_S32
void Fan::UpdateSound()
{
    if (soundStarted) {
        if (!IsSoundPlaying(sound))
            sound = Sound_Play(SND_FAN_BLOW_LOOP, this, 0xff, SNDF_LOOP | SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 0x1000);
    } else {
        soundStarted = 1;
        sound = Sound_Play(SND_FAN_BLOW, this, 0xff, SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 0x1000);
    }
}
void Fan::StopSound()
{
    if (sound != 0) {
        StopSoundHandle(sound);
        sound = 0;
    }
    soundStarted = 0;
}
/* BYTES(slot-scope, inferred): the nested block(s) only order the frame: their locals are allocated after the enclosing scope's */
void Fan::Blow(s16 heading)
{
    s32 count;
    {
        s16 bearing;
        {
            ScnObject *objects[64];
            {
                Vec3i delta;
                {
                    CollBox region;
                    region.min.x = pos.x - 400;
                    region.min.y = pos.y - 300;
                    region.min.z = pos.z - 400;
                    region.max.x = pos.x + 400;
                    region.max.y = pos.y + 100;
                    region.max.z = pos.z + 400;
                    count = ObjGrid_QueryBoxPoints(&region, objects);
                    {
                        for (s32 i = 0; i < count; ++i) {
                            ScnObject *object = objects[i];
                            if (object != this && object->IsActive() && !object->InstFlags(INST_F_ATTACHED)) {
                                CollBox *box = object->GetFirstModelBox();
                                {
                                    s32 hit = 0;
                                    if (box) {
                                        CollBox world, holder;
                                        world.Box_Translate(box, &object->pos);
                                        holder.Box_Translate(GetParent()->GetFirstModelBox(), &GetParent()->pos);
                                        if (BoxOverlap4(holder.max.x - world.min.x, world.max.x - holder.min.x,
                                                        holder.max.z - world.min.z, world.max.z - holder.min.z))
                                            hit = 1;
                                    }
                                    if (!hit) {
                                        delta.x = pos.x - object->pos.x;
                                        delta.y = pos.y - object->pos.y;
                                        delta.z = pos.z - object->pos.z;
                                        bearing = Math_RadiansToAngle4096((float)atan2(delta.x, delta.z));
                                        if (FAN_ABS(AngleDifference(heading, bearing)) <= 0x199) {
                                            delta.x *= delta.x;
                                            delta.y *= delta.y;
                                            delta.z *= delta.z;
                                            if (delta.x + delta.z <= 160000)
                                                hit = 1;
                                        }
                                    }
                                    /* cast kept: the message arg is a void *; it carries the heading */
                                    if (hit)
                                        object->HandleMessage(this, MSG_FAN_BLOW, (void *)(s32)heading);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
void Fan::SetState(u8 next, s32 stopSound)
{
    const FanAnimEntry *entry = &g_fanStateAnims[next];
    PlayAnim(entry->animId, entry->flags & FAN_AF_LOOP, 0);
    if (entry->flags & FAN_AF_HIDE_SHADOW)
        shadow.SetVisible(0);
    else
        shadow.SetVisible(1);
    if (stopSound)
        StopSound();
    state = next;
}
void Fan::Update()
{
    switch (state) {
        case FAN_ST_SPIN_UP:
            Blow(GetParent()->GetFacing());
            UpdateSound();
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(FAN_ST_BLOWING, 0);
            break;
        case FAN_ST_BLOWING:
            UpdateSound();
            Blow(GetParent()->GetFacing());
            break;
    }
    AdvanceAnim();
}
/* BYTES(slot-scope, inferred): the nested block(s) only order the frame: their locals are allocated after the enclosing scope's */
s32 Fan::HandleMessage(ScnObject *sender, u32 msg, void *arg)
{
    switch (msg) {
        case MSG_QUERY_ACTION:
            if (sender->GetClassId() == CLASSID_WOLF)
                return CTX_PICKUP;
            break;
        case MSG_PICKUP: {
            ScnObject *parent = sender;
            {
                /* cast kept: the message arg is a void *; what it carries depends on the message */
                u8 joint = (u8)(u32)arg;
                AttachTo(parent, joint, 0, 0, 0, 0);
            }
            SetState(FAN_ST_HELD, 1);
            return 1;
        }
        case MSG_DROP: {
            /* cast kept: the message arg is a void *; what it carries depends on the message */
            Vec3s *drop = (Vec3s *)arg;
            Detach();
            SetPosition(drop);
            SetState(FAN_ST_WORLD, 1);
            return 1;
        }
        case MSG_CONTAINER_STATE:
            switch ((u32)arg) { /* cast kept: the message arg is a void *; what it carries depends on the message */
                case CONTAINER_RELEASED:
                    homePos = pos;
            }
            return 1;
        case MSG_HELD_STATE_BEGIN:
            SetState(FAN_ST_SPIN_UP, 1);
            return 1;
        case MSG_HELD_STATE_END:
            SetState(FAN_ST_HELD, 1);
            return 1;
        case MSG_CARRY_ANIM:
            if (g_fanStateAnims[state].flags & FAN_AF_BLOWING) {
                /* cast kept: the message arg is a void * carrying the Wolf's carry-pose number */
                if (arg == (void *)1) {
                    if (AnimId() != AVENTI01_ANIM_WALKV1)
                        PlayAnim(AVENTI01_ANIM_WALKV1, 1, 1);
                } else if (AnimId() != AVENTI01_ANIM_STANDV1)
                    PlayAnim(AVENTI01_ANIM_STANDV1, 1, 1);
            }
            return 1;
        case MSG_QUERY_HELD_ACTION:
            return HELD_FAN;
        case MSG_ATTRACT_REPORT: {
            /* cast kept: the message arg is a void *; what it carries depends on the message */
            FanReport *report = (FanReport *)arg;
            if (lastReport.classId == 0 || report->proximity <= lastReport.proximity ||
                g_gameTime - reportTime >= 0x800) {
                reportTime = g_gameTime;
                lastReport = *report;
            }
            return 1;
        }
        case MSG_QUERY_NEAREST_TARGET:
            if (g_fanStateAnims[state].flags & FAN_AF_BLOWING) {
                /* cast kept: the message arg is a void *; what it carries depends on the message */
                FanReport *report = (FanReport *)arg;
                if (lastReport.classId && g_gameTime - reportTime <= 0x800) {
                    *report = lastReport;
                    return 1;
                }
            }
            break;
        case MSG_CHECKPOINT_ROLLBACK:
            Reset();
            return 1;
    }
    return 0;
}
void Fan::Reset()
{
    SetState(FAN_ST_WORLD, 1);
    if (IsInWorld())
        SetPosition(&homePos);
    soundStarted = 0;
    reportTime = 0;
    lastReport.classId = 0;
    lastReport.proximity = 0;
}
void Fan::PostLoadInit()
{
    if (IsInWorld())
        SnapToGround(1);
    homePos = pos;
    soundStarted = 0;
    sound = 0;
    SetState(FAN_ST_WORLD, 1);
    reportTime = 0;
    lastReport.classId = 0;
    lastReport.proximity = 0;
}
ScnObject *Fan_Create(void *record)
{
    Fan *object = new Fan;
    object = (Fan *)object->Init(record, 0); /* cast kept: Init returns the object as a ScnObject * */
    return object;
}
