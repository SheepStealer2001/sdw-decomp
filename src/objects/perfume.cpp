/*
 * T189 - original object Perfume.cpp (guessed name), one translation unit.
 *   .text  0x4da360-0x4dac7a (Perfume_Create .. Perfume_HandleMessage)
 *   .rdata 0x576798-0x5767d4 (g_perfumeScentFxParams, then ??_7Perfume)
 * The scent parameters are defined in place.
 */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *inst, Animator *animator, u16 id, u32 opts);

#define SDW_MEMBERS_ScnObject                                                                 \
    static void *operator new(u32 size);                                                      \
    void AttachTo(ScnObject *parent, u8 joint, Vec3s *offset, Vec3s *rot, u32 arg, u32 arg2); \
    void SetPickup(s32 on)                                                                    \
    {                                                                                         \
        if (on)                                                                               \
            flags |= SCN_OF_NO_PLANE_CULL;                                                    \
        else                                                                                  \
            flags &= (u16)~SCN_OF_NO_PLANE_CULL;                                              \
    }


#define SDW_MEMBERS_InlineEmitter32 InlineEmitter32();
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_ISINWORLD 1
#define SDW_INLINE_SCNOBJECT_ISKEPT 1
#define SDW_INLINE_SCNOBJECT_SETKEPT_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_ISINWORLD
#undef SDW_INLINE_SCNOBJECT_ISKEPT
#undef SDW_INLINE_SCNOBJECT_SETKEPT_S32
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_SHADOW_SETVISIBLE_S32 1
#include "../engine/shadow_inlines.h"
#undef SDW_INLINE_SHADOW_SETVISIBLE_S32
inline InlineEmitter32::InlineEmitter32()
{
    base.slotPool = slotBuf;
    base.particles = particleBuf;
    base.count = 32;
    base.Emitter_Reset();
}

#include "sheep.h"
#include "../engine/progress_inventory.h"
/* 0x576798 - the scent particles; const, so main CONST in .rdata ahead of Perfume's vtable. An array of one, so the
   name decays to the pointer Emitter_UpdatePerfume takes. */
const EmitterPerfumeParams g_perfumeScentFxParams[1] = {{450, -15, 45056, 1408, 16, 64, 80, 3}};
extern u32 g_gameTime;
extern s32 g_dt;

/* 0x4da360 Perfume_Create. Generated InlineEmitter32 member construction
 * initializes its buffers before the derived vtable is installed. */
ScnObject *Perfume_Create(void *record)
{
    Perfume *obj = new Perfume;
    obj = (Perfume *)obj->Init(record, 0); /* cast kept: Init returns the object as its ScnBody base */
    return obj;
}

/* 0x4da401 Perfume_Init */
void Perfume::PostLoadInit()
{
    scentFx.base.Emitter_Reset();
    perfumeFlags.blown = 0;
    perfumeFlags.registered = 0;
    if (IsInWorld())
        SnapToGround(1);
    homePos = pos;
    SetPickup(1);
    state = PERFUME_ST_INACTIVE;
    blowTime = 0;
    reportTime = 0;
    lastReport.classId = 0; /* no report held (a report carries the sheep's class id) */
    lastReport.proximity = 0;
    PlayAnim(AFLACO01_ANIM_OBJET, 0, 0);
}

/* 0x4da52b Perfume_Reset */
void Perfume::Reset()
{
    scentFx.base.Emitter_Reset();
    state = PERFUME_ST_INACTIVE;
    perfumeFlags.blown = 0;
    if (perfumeFlags.registered) {
        Flock_RemoveScentSource(this);
        perfumeFlags.registered = 0;
    }
    if (IsInWorld()) {
        if (IsKept()) {
            RemoveFromWorld();
            Inventory_Add(this);
        } else {
            SetPosition(&homePos);
        }
    }
    blowTime = 0;
    reportTime = 0;
    lastReport.classId = 0; /* no report held (a report carries the sheep's class id) */
    lastReport.proximity = 0;
    PlayAnim(AFLACO01_ANIM_OBJET, 0, 0);
}

/* 0x4da660 Perfume_Render */
void Perfume::Render(Camera *view)
{
    ScnMobile::Render(view);
    if (scentFx.base.flags.active && IsInWorld())
        scentFx.base.Emitter_Render(view, 0);
}

/* 0x4da6b5 Perfume_Update */
void Perfume::Update()
{
    emitPos = pos;
    emitPos.y -= 40;
    switch (state) {
        case PERFUME_ST_EMITTING:
            scentFx.base.Emitter_UpdatePerfume(g_perfumeScentFxParams, &emitPos, blowHeading, perfumeFlags.blown,
                                               emitPos.y - 90, 1);
            if (perfumeFlags.blown) {
                blowTime += g_dt;
                if (blowTime > 0x471c)
                    blowTime = 0x471c;
            } else {
                blowTime -= g_dt;
                if (blowTime < 0)
                    blowTime = 0;
            }
            Flock_AddScentSource(this, blowHeading, (u16)(blowTime * 450 >> 12));
            perfumeFlags.registered = 1;
            perfumeFlags.blown = 0;
            break;
        case PERFUME_ST_INACTIVE:
            if (perfumeFlags.registered) {
                Flock_RemoveScentSource(this);
                perfumeFlags.registered = 0;
            }
            break;
    }
    AdvanceAnim();
}

/* 0x4da86e Perfume_HandleMessage */
s32 Perfume::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    ScnObject *p;
    u8 q;
    DropMsgArg *r;
    PerfumeScentReport *s;
    switch (msgId) {
        case MSG_LANDED:
            SetKept(0);
            Reset();
            return 1;
        case MSG_QUERY_ACTION:
            if (sender->GetClassId() == CLASSID_WOLF || sender->GetClassId() == CLASSID_ROBOT)
                return CTX_PICKUP;
            break;
        case MSG_QUERY_HELD_ACTION:
            return HELD_THROWABLE;
        case MSG_FAN_BLOW:
            blowHeading = (s16)(u32)arg; /* cast kept: the Fan passes its heading in the void * argument */
            perfumeFlags.blown = 1;
            if (lastReport.classId && g_gameTime - reportTime <= 0x800)
                sender->HandleMessage(this, MSG_ATTRACT_REPORT, &lastReport.classId);
            return 1;
        case MSG_INVENTORY_STORED:
            if (perfumeFlags.registered) {
                Flock_RemoveScentSource(this);
                perfumeFlags.registered = 0;
            }
            return 1;
        case MSG_PICKUP:
            p = sender;
            q = (u8)(u32)arg; /* cast kept: MSG_PICKUP passes the joint index in the void * argument */
            AttachTo(p, q, 0, 0, 0, 0);
            shadow.SetVisible(0);
            scentFx.base.Emitter_Reset();
            state = PERFUME_ST_INACTIVE;
            PlayAnim(AFLACO01_ANIM_LINK, 0, 0);
            return 1;
        case MSG_DROP:
            r = (DropMsgArg *)arg; /* cast kept: MSG_DROP passes a DropMsgArg * in the void * argument */
            Detach();
            SetPosition(&r->pos);
            shadow.SetVisible(1);
            if (r->placed) {
                scentFx.base.Emitter_Reset();
                state = PERFUME_ST_EMITTING;
            }
            PlayAnim(AFLACO01_ANIM_OBJET, 0, 0);
            return 1;
        case MSG_CONTAINER_STATE:
            switch ((u32)arg) { /* cast kept: the container state travels in the void * argument */
                case CONTAINER_RELEASED:
                    homePos = pos;
            }
            return 1;
        case MSG_ATTRACT_REPORT:
            s = (PerfumeScentReport *)
                arg; /* cast kept: MSG_ATTRACT_REPORT passes a PerfumeScentReport * in the void * argument */
            if (!lastReport.classId || s->proximity <= lastReport.proximity || g_gameTime - reportTime >= 0x800) {
                reportTime = g_gameTime;
                lastReport = *s;
            }
            return 1;
        case MSG_CHECKPOINT_ROLLBACK:
            Reset();
            return 1;
    }
    return 0;
}
