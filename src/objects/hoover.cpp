/* PAL PC Hoover 0x4cb130-0x4cbb11. Original directional/sound behavior retained. */
/* BYTES: slot-scope. */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *, Animator *, u16, u32);
class ScnObject;
u16 Sound_Play(u16, void *, u16, u8, s32);
#include "../engine/sound_mgr.h"
#include "../engine/scn_tools.h"

#define SDW_MEMBERS_ScnObject       \
    static void *operator new(u32); \
    void SetFacing(s16 angle);      \
    void AttachTo(ScnObject *, u8, Vec3s *, Vec3s *, u32, u32);


#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_GETFACING 1
#define SDW_INLINE_SCNOBJECT_SETFACING_S16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_GETFACING
#undef SDW_INLINE_SCNOBJECT_SETFACING_S16
#define SDW_INLINE_SCNOBJECT_ISINWORLD 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_ISINWORLD
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_SHADOW_SETVISIBLE_S32 1
#include "../engine/shadow_inlines.h"
#undef SDW_INLINE_SHADOW_SETVISIBLE_S32
extern Wolf *g_pWolf;
extern "C" const s16 g_sinTable4096[];
extern "C" const s16 *g_pCosTable;
s32 Scenaric_FindByClass(u16, ScnObject **, s32);
s32 Vec3s_DistSqXZ(Vec3s *, Vec3s *);
#include "../sdk/crt.h"
extern "C" s16 Math_RadiansToAngle4096(float);
#define SDW_INLINE_SCNOBJECT_ISSOUNDPLAYING_U16 1
#define SDW_INLINE_SCNOBJECT_STOPSOUNDHANDLE_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_ISSOUNDPLAYING_U16
#undef SDW_INLINE_SCNOBJECT_STOPSOUNDHANDLE_U16
void Hoover::PostLoadInit()
{
    void *props = record;
    activationBox = Scn_GetPropBox(props, 0);
    ghostCount = (u16)Scenaric_FindByClass(CLASSID_GHOST, ghosts, 15);
    wolfFrozen = 0;
    inHand = 0;
    charged = 0;
    SetState(HOOVER_ST_OFF);
    suckSoundHandle = 0;
    if (IsInWorld())
        homePos = pos;
}
void Hoover::Reset()
{
    suckSoundHandle = 0;
    wolfFrozen = 0;
    inHand = 0;
    charged = 0;
    SetState(HOOVER_ST_OFF);
    if (IsInWorld()) {
        SetPosition(&homePos);
        SnapToGround(1);
    }
}
/* BYTES(slot-scope, inferred): the nested block(s) only order the frame: their locals are allocated after the enclosing scope's */
void Hoover::Update()
{
    s32 i;
    {
        Vec3s delta;
        {
            s16 bearing;
            switch (state) {
                case HOOVER_ST_ARMED:
                    if (!g_pWolf->HandleMessage(this, MSG_WOLF_IS_RUNNING, 0) &&
                        !g_pWolf->HandleMessage(this, MSG_WOLF_IS_DEAD, 0)) {
                        for (i = 0; i < (s32)ghostCount; i++) {
                            if (Vec3s_DistSqXZ(&g_pWolf->pos, &ghosts[i]->pos) < 0xe100) {
                                delta.x = ghosts[i]->pos.x - g_pWolf->pos.x;
                                delta.y = ghosts[i]->pos.y - g_pWolf->pos.y;
                                delta.z = ghosts[i]->pos.z - g_pWolf->pos.z;
                                bearing = (Math_RadiansToAngle4096((float)atan2(delta.x, delta.z)) + 0x800) & 0xfff;
                                if ((s16)((s16)((GetFacing() - bearing + 0x800) & 0xfff) - 0x800) < 0x640) {
                                    /* cast kept: HandleMessage's arg is a void *: this message passes a number in it */
                                    if (ghosts[i]->HandleMessage(this, MSG_KILL, (void *)KILL_GENERIC)) {
                                        delta = g_pWolf->pos;
                                        delta.x -= (s16)((g_sinTable4096[bearing] * 240) >> 12);
                                        delta.z -= (s16)((g_pCosTable[bearing] * 240) >> 12);
                                        ghosts[i]->SetPosition(&delta);
                                        g_pWolf->SetFacing(bearing - 50);
                                        if (!wolfFrozen)
                                            wolfFrozen = g_pWolf->HandleMessage(this, MSG_FREEZE, 0);
                                        /* cast kept: HandleMessage's arg is a void *: this message passes a number in
                                         * it */
                                        g_pWolf->HandleMessage(this, MSG_WOLF_HOOVER_HOLD, (void *)1);
                                        SetState(HOOVER_ST_SUCKING);
                                    }
                                    break;
                                }
                            }
                        }
                    }
                    break;
                case HOOVER_ST_SUCKING:
                    if (AnimFlags(ANIM_F_FINISHED))
                        SetState(HOOVER_ST_ARMED);
                    break;
            }
        }
    }
    AdvanceAnim();
}
s32 Hoover::HandleMessage(ScnObject *sender, u32 msg, void *arg)
{
    Vec3s *drop;
    switch (msg) {
        case MSG_QUERY_ACTION:
            if (sender->GetClassId() == CLASSID_WOLF)
                return CTX_PICKUP;
            break;
        case MSG_PICKUP:
            /* cast kept: the message arg is a void *: what it carries depends on the message id */
            AttachTo(sender, (u8)(u32)arg, 0, 0, 0, 0);
            PlayAnim(AASPIR01_ANIM_LINK, 0, 0);
            shadow.SetVisible(0);
            return 1;
        case MSG_DROP:
            drop = (Vec3s *)arg; /* cast kept: the message arg is a void *: what it carries depends on the message id */
            Detach();
            SetPosition(drop);
            SnapToGround(1);
            PlayAnim(AASPIR01_ANIM_OBJET, 0, 0);
            shadow.SetVisible(1);
            return 1;
        case MSG_QUERY_HELD_ACTION:
            inHand = 1;
            return HELD_NONE;
        case MSG_INVENTORY_STORED:
            inHand = 0;
            if (state == HOOVER_ST_ARMED)
                SetState(HOOVER_ST_OFF);
            return 1;
        case MSG_INVENTORY_TAKE_OUT:
            if (charged)
                SetState(HOOVER_ST_ARMED);
            return 1;
        case MSG_CONTAINER_STATE:
            switch ((u32)arg) { /* cast kept: the message arg is a void *: what it carries depends on the message id */
                case CONTAINER_RELEASED:
                    homePos = pos;
            }
            return 1;
        case MSG_CHECKPOINT_ROLLBACK:
            Reset();
            return 1;
        case MSG_HOOVER_CHARGED:
            battery = sender;
            charged = 1;
            if (inHand)
                SetState(HOOVER_ST_ARMED);
            break;
        case MSG_HOOVER_DISCHARGED:
            charged = 0;
            SetState(HOOVER_ST_OFF);
            break;
        case MSG_HOOVER_GHOST_CAUGHT:
            if (wolfFrozen)
                wolfFrozen = !g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0);
            g_pWolf->HandleMessage(this, MSG_WOLF_HOOVER_HOLD, 0);
            battery->HandleMessage(this, MSG_BATTERY_DRAIN, 0);
            break;
    }
    return 0;
}
void Hoover::SetState(u8 next)
{
    s32 i;
    state = next;
    switch (next) {
        case HOOVER_ST_SUCKING:
            if (IsSoundPlaying(suckSoundHandle))
                StopSoundHandle(suckSoundHandle);
            suckSoundHandle = Sound_Play(SND_FAN_BLOW, this, 0x33,
                                         SNDF_POSITIONAL | SNDF_DIST_HORIZONTAL | SNDF_NO_RETRIGGER, 0x1000);
            /* cast kept: HandleMessage's arg is a void *: this message passes a number in it */
            battery->HandleMessage(this, MSG_BATTERY_DRAIN, (void *)1);
            PlayAnim(AASPIR01_ANIM_SUCK1, 0, 0);
            break;
        case HOOVER_ST_ARMED:
            for (i = 0; i < (s32)ghostCount; i++)
                /* cast kept: HandleMessage's arg is a void *: this message passes a number in it */
                ghosts[i]->HandleMessage(this, MSG_GHOST_SET_VULNERABLE, (void *)1);
            break;
        case HOOVER_ST_OFF:
            if (IsSoundPlaying(suckSoundHandle))
                StopSoundHandle(suckSoundHandle);
            for (i = 0; i < (s32)ghostCount; i++)
                ghosts[i]->HandleMessage(this, MSG_GHOST_SET_VULNERABLE, 0);
            break;
    }
}
ScnObject *Hoover_Create(void *record)
{
    Hoover *object = new Hoover;
    object = (Hoover *)object->Init(record, 0); /* cast kept: Init returns the ScnBody * base of this object */
    return object;
}
