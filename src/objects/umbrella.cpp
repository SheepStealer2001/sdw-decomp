/* PAL PC Umbrella. The open flag is a bitfield view. */
/* BYTES: view. */
/* BYTES(view): view: the open flag is a bitfield view (the access widths are the original's) */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *, Animator *, u16, u32);

#define SDW_MEMBERS_ScnObject                                   \
    static void *operator new(u32);                             \
    void AttachTo(ScnObject *, u8, Vec3s *, Vec3s *, u32, u32); \
    void SetRotation(Vec3s *value);


#include "sdw_classes.h"
#include "../engine/maths.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_ISINWORLD 1
#define SDW_INLINE_SCNOBJECT_SETROTATION_VEC3S 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_ISINWORLD
#undef SDW_INLINE_SCNOBJECT_SETROTATION_VEC3S
#define SDW_INLINE_SCNBODY_GETANIMID 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_GETANIMID
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_SHADOW_SETVISIBLE_S32 1
#include "../engine/shadow_inlines.h"
#undef SDW_INLINE_SHADOW_SETVISIBLE_S32
void Umbrella::Update()
{
    switch (state) {
        case UMBRELLA_ST_OPENING_GROUND:
            if (AnimFlags(ANIM_F_FINISHED)) {
                state = UMBRELLA_ST_OPEN;
                PlayAnim(APARAP01_ANIM_UMBREL3, 1, 1);
                umbrellaFlags.open = 1;
            }
            break;
        case UMBRELLA_ST_OPENING_AIR:
            if (AnimFlags(ANIM_F_FINISHED)) {
                state = UMBRELLA_ST_OPEN;
                PlayAnim(APARAP01_ANIM_UMBREL5, 1, 1);
                umbrellaFlags.open = 1;
            }
            break;
        case UMBRELLA_ST_CLOSING:
            if (AnimFlags(ANIM_F_FINISHED)) {
                state = UMBRELLA_ST_CLOSED;
                PlayAnim(APARAP01_ANIM_LINK, 0, 1);
            }
            break;
    }
    AdvanceAnim();
}
s32 Umbrella::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    Vec3s *newPosition;
    u8 partIndex;
    ScnObject *parent;
    u16 animId;
    switch (msgId) {
        case MSG_QUERY_ACTION:
            if (sender->GetClassId() == CLASSID_WOLF)
                return CTX_PICKUP;
            break;
        case MSG_QUERY_HELD_ACTION:
            return HELD_UMBRELLA;
        case MSG_HELD_STATE_BEGIN:
            if (arg == (void *)3) { /* cast kept: the void * arg carries the Wolf's held-state number, 3 in the air */
                state = UMBRELLA_ST_OPENING_AIR;
                PlayAnim(APARAP01_ANIM_UMBREL6, 0, 1);
            } else {
                state = UMBRELLA_ST_OPENING_GROUND;
                PlayAnim(APARAP01_ANIM_UMBREL2, 0, 1);
            }
            return 1;
        case MSG_HELD_STATE_END:
            state = UMBRELLA_ST_CLOSING;
            umbrellaFlags.open = 0;
            if (arg == (void *)3) /* cast kept: the void * arg carries the Wolf's held-state number, 3 in the air */
                PlayAnim(APARAP01_ANIM_UMBREL7, 0, 1);
            else
                PlayAnim(APARAP01_ANIM_UMBREL4, 0, 1);
            return 1;
        case MSG_PICKUP:
            parent = sender;
            partIndex = (u8)(u32)arg; /* cast kept: this message's void * arg carries the joint number */
            AttachTo(parent, partIndex, 0, 0, 0, 0);
            PlayAnim(APARAP01_ANIM_LINK, 0, 0);
            state = UMBRELLA_ST_CLOSED;
            shadow.SetVisible(0);
            return 1;
        case MSG_DROP:
            newPosition = (Vec3s *)arg; /* cast kept: this message's void * arg is the drop position */
            Detach();
            SetPosition(newPosition);
            PlayAnim(APARAP01_ANIM_OBJET, 0, 0);
            state = UMBRELLA_ST_CLOSED;
            shadow.SetVisible(1);
            return 1;
        case MSG_CONTAINER_STATE:
            switch ((s32)arg) { /* cast kept: this message's void * arg carries the container state */
                case CONTAINER_RELEASED:
                    homePos = pos;
                    break;
            }
            return 1;
        case MSG_CHECKPOINT_ROLLBACK:
            Reset();
            return 1;
        case MSG_CARRY_ANIM:
            if (umbrellaFlags.open) {
                switch ((s32)arg) { /* cast kept: this message's void * arg carries the Wolf's animation cue */
                    case WOLF_CUE_GLIDE:
                        animId = APARAP01_ANIM_UMBREL5;
                        break;
                    case WOLF_CUE_JUMP_START:
                        animId = APARAP01_ANIM_UMBREL10;
                        break;
                    case WOLF_CUE_JUMP:
                        animId = APARAP01_ANIM_UMBREL11;
                        break;
                    case WOLF_CUE_AIR_HOP:
                        animId = APARAP01_ANIM_UMBREL8;
                        break;
                    case WOLF_CUE_HOP_FALL:
                        animId = APARAP01_ANIM_UMBREL1;
                        break;
                    case WOLF_CUE_WALK:
                        animId = APARAP01_ANIM_UMBREL9;
                        break;
                    default:
                        animId = APARAP01_ANIM_UMBREL3;
                        break;
                }
                if (GetAnimId() != animId)
                    PlayAnim(animId, 1, 1);
            }
            return 1;
    }
    return 0;
}
void Umbrella::Reset()
{
    state = UMBRELLA_ST_CLOSED;
    PlayAnim(APARAP01_ANIM_OBJET, 0, 0);
    shadow.SetVisible(1);
    SetRotation(g_pZeroVec3s);
    if (IsInWorld())
        SetPosition(&homePos);
    umbrellaFlags.open = 0;
}
void Umbrella::PostLoadInit()
{
    if (IsInWorld())
        SnapToGround(1);
    homePos = pos;
    state = UMBRELLA_ST_CLOSED;
    PlayAnim(APARAP01_ANIM_OBJET, 0, 0);
    umbrellaFlags.open = 0;
}
ScnObject *Umbrella_Create(void *record)
{
    Umbrella *object = new Umbrella;
    object = (Umbrella *)object->Init(record, 0); /* cast kept: Init returns the base class */
    return object;
}
