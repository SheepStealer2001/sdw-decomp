/* PAL PC Flute, 0x4c1ef0-0x4c2752, using the InlineEmitter16 member. */
/* BYTES: slot-group. */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *instance, Animator *animator, u16 id, u32 options);
class ScnObject;
#include "../engine/sound_mgr.h"
#include "../engine/maths.h"
#include "../engine/scn_tools.h"

#define SDW_MEMBERS_ScnObject            \
    static void *operator new(u32 size); \
    void AttachTo(ScnObject *parent, u8 joint, Vec3s *offset, Vec3s *rot, u32 a, u32 b);


#define SDW_MEMBERS_InlineEmitter16 InlineEmitter16();
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_STOPSOUND_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_STOPSOUND_U16
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_GETFACING 1
#define SDW_INLINE_SCNOBJECT_ISINWORLD 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_GETFACING
#undef SDW_INLINE_SCNOBJECT_ISINWORLD
#define SDW_INLINE_SCNOBJECT_INSTFLAGS_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_INSTFLAGS_U16
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_SHADOW_SETVISIBLE_S32 1
#include "../engine/shadow_inlines.h"
#undef SDW_INLINE_SHADOW_SETVISIBLE_S32
inline InlineEmitter16::InlineEmitter16()
{
    base.slotPool = slotBuf;
    base.particles = particleBuf;
    base.count = 16;
    base.Emitter_Reset();
}
#define SDW_INLINE_SCNOBJECT_GETPARENT 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETPARENT
#define SDW_INLINE_SCNOBJECT_BROADCASTAROUND_S32_S32_U16_U32_VOID 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_BROADCASTAROUND_S32_S32_U16_U32_VOID
u16 Sound_Play(u16 id, void *owner, u16 volume, u8 flags, s32 pitch);
s32 Vec3s_Dist(Vec3s *a, Vec3s *b);
s32 Scenaric_FindByClass(u16 id, ScnObject **out, s32 capacity);
/* 0x57618c - the drift of the music-note particles; const, so it sits in .rdata ahead of Flute's vtable */
const EmitterDriftParams g_fluteNoteFxParams = {100, -75, 16384, 2730, 6, 40, 8};
extern "C" const s16 g_sinTable4096[];
extern "C" const s16 *g_pCosTable;

void Flute::SetState(u8 value)
{
    state = value;
    if (playSound) {
        StopSound(playSound);
        playSound = 0;
    }
}
/* BYTES(slot-group): locals grouped in origin only to pin the original frame offsets; unused fill gaps */
void Flute::Update()
{
    s32 distance;
    /* Origin vector is at EBP-0x10; heading at -6, with four untouched
       bytes between them. Preserve the observed frame, not guessed names. */
    struct NoteOrigin {
        Vec3s point;
        u8 unused[4];
        s16 heading;
    } origin;
    ScnObject *holder;
    switch (state) {
        case FLUTE_ST_HELD:
            if (noteFx.base.flags.active)
                noteFx.base.Emitter_UpdateDriftLift(&g_fluteNoteFxParams, &pos, 0, 0);
            break;
        case FLUTE_ST_PLAYING:
            holder = GetParent();
            origin.heading = holder->rot.y;
            if (sam) {
                distance = Vec3s_DistSq(&pos, &sam->pos);
                if (distance <= 90000)
                    sam->HandleMessage(this, MSG_SAM_INVESTIGATE, 0);
                if (distance <= 4000000)
                    origin.heading = HeadingTo(&sam->pos);
                /* cast kept: HandleMessage's arg is a void *; this message passes the holder's class id in it */
                BroadcastAround(200, 200, 600, MSG_LOUD_NOISE, (void *)holder->GetClassId());
            }
            origin.point.x =
                (s16)(Rand_Range(-10, 10) + holder->pos.x - (g_sinTable4096[holder->GetFacing()] * 60 >> 12));
            origin.point.y = (s16)(Rand_Range(-10, 10) + holder->pos.y - 110);
            origin.point.z = (s16)(Rand_Range(-10, 10) + holder->pos.z - (g_pCosTable[holder->GetFacing()] * 60 >> 12));
            noteFx.base.Emitter_UpdateDriftLift(&g_fluteNoteFxParams, &origin.point, origin.heading, 1);
            break;
    }
    AdvanceAnim();
}
/* BYTES(slot-group, inferred): locals grouped in attach only to pin the original frame offsets; unused fill gaps */
s32 Flute::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    s32 distance;
    switch (msgId) {
        case MSG_QUERY_ACTION:
            if (sender->GetClassId() == CLASSID_WOLF)
                return CTX_PICKUP;
            break;
        case MSG_QUERY_HELD_ACTION:
            return HELD_FLUTE;
        case MSG_HELD_STATE_BEGIN:
            if (sender->GetClassId() == CLASSID_WOLF) {
                SetState(FLUTE_ST_PLAYING);
                playSound =
                    Sound_Play(SND_FLUTE_TUNE, this, 0xff, SNDF_LOOP | SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 0x1000);
            }
            return 1;
        case MSG_HELD_STATE_END:
            SetState(FLUTE_ST_HELD);
            return 1;
        case MSG_QUERY_NEAREST_TARGET:
            if (state == FLUTE_ST_PLAYING && sam) {
                distance = Vec3s_Dist(&pos, &sam->pos);
                if (distance <= 300) {
                    /* cast kept: HandleMessage's arg is a void *; this message passes the reply record */
                    FanReport *report = (FanReport *)arg;
                    report->classId = CLASSID_SAM;
                    report->proximity = (u16)(distance * 65535 / 300);
                    return 1;
                }
            }
            break;
        case MSG_PICKUP: {
            struct AttachArgs {
                u8 unused[3];
                u8 bone;
                ScnObject *owner;
            } attach;
            attach.owner = sender;
            /* cast kept: HandleMessage's arg is a void *; MSG_PICKUP passes the joint number in it */
            attach.bone = (u8)(u32)arg;
            AttachTo(attach.owner, attach.bone, 0, 0, 0, 0);
            PlayAnim(AFLUTE01_ANIM_LINK, 0, 0);
            SetState(FLUTE_ST_HELD);
            shadow.SetVisible(0);
            noteFx.base.Emitter_Reset();
            return 1;
        }
        case MSG_DROP: {
            /* cast kept: HandleMessage's arg is a void *; MSG_DROP passes the drop record */
            DropMsgArg *drop = (DropMsgArg *)arg;
            Detach();
            SetPosition(&drop->pos);
            PlayAnim(AFLUTE01_ANIM_OBJET, 0, 0);
            SetState(FLUTE_ST_WORLD);
            shadow.SetVisible(1);
            if (drop->placed)
                noteFx.base.Emitter_Reset();
            return 1;
        }
        case MSG_CONTAINER_STATE:
            /* cast kept: HandleMessage's arg is a void *; this message passes the container state in it */
            switch ((s32)arg) {
                case CONTAINER_RELEASED:
                    homePos = pos;
                    break;
            }
            return 1;
        case MSG_CHECKPOINT_ROLLBACK:
            Reset();
            return 1;
    }
    return 0;
}
void Flute::Render(Camera *view)
{
    ScnMobile::Render(view);
    if (noteFx.base.flags.active && IsInWorld())
        noteFx.base.Emitter_Render(view, 0);
}
void Flute::Reset()
{
    PlayAnim(AFLUTE01_ANIM_OBJET, 0, 0);
    SetState(FLUTE_ST_WORLD);
    shadow.SetVisible(1);
    if (IsInWorld())
        SetPosition(&homePos);
    noteFx.base.Emitter_Reset();
}
void Flute::PostLoadInit()
{
    if (IsInWorld())
        SnapToGround(1);
    homePos = pos;
    playSound = 0;
    PlayAnim(AFLUTE01_ANIM_OBJET, 0, 0);
    SetState(FLUTE_ST_WORLD);
    sam = 0;
    Scenaric_FindByClass(CLASSID_SAM, &sam, 1);
    noteFx.base.Emitter_Reset();
}
ScnObject *Flute_Create(void *record)
{
    Flute *object = new Flute;
    object = (Flute *)object->Init(record, 0); /* cast kept: Init returns this as a ScnBody * */
    return object;
}
