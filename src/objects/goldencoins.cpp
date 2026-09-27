/* PAL PC GoldenCoins, original VA 0x4c7930-0x4c8101. */
/* BYTES: dead-code, slot-scope. */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *, Animator *, u16, u32);
class ScnObject;
/* The radius-search score callback, as used by Salad and Mine. */
typedef u32 (*CoinScore)(ScnObject *, ScnObject *, u32, ScnObject *);

#define SDW_MEMBERS_ScnObject                                   \
    static void *operator new(u32 size);                        \
    void AttachTo(ScnObject *, u8, Vec3s *, Vec3s *, u32, u32); \
    ScnObject *Scenaric_FindBestInRadius(Vec3s *, s16, s16, u16, u16 *, CoinScore, s32);

#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_ISINWORLD 1
#define SDW_INLINE_SCNOBJECT_SETVISIBLE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_ISINWORLD
#undef SDW_INLINE_SCNOBJECT_SETVISIBLE_S32
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
struct CoinReport {
    u16 classId, proximity;
};
u32 GoldenCoins_NearestCoinScoreCB(ScnObject *, ScnObject *, u32, ScnObject *);

void GoldenCoins::PostLoadInit()
{
    homePos.x = pos.x;
    homePos.y = pos.y;
    homePos.z = pos.z;
    homePos.y -= 200;
    homePos.y = QueryGroundY(&homePos, 1);
    SetPosition(&homePos);
    inInventory = 0;
    hintPending = 0;
    revealed = 0;
    renderScale = 0;
    SetState(COIN_BURIED);
}

void GoldenCoins::Update()
{
    switch (state) {
        case COIN_POCKETED:
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(COIN_BURIED);
            break;
        case COIN_APPEARING:
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(COIN_ON_GROUND);
            break;
    }
    AdvanceAnim();
}

void GoldenCoins::SetState(u8 next)
{
    switch (next) {
        case COIN_BURIED:
            SetVisible(0);
            renderScale = 0;
            claimedBySam = 0;
            SetPosition(&homePos);
            break;
        case COIN_POCKETED:
            SetVisible(0);
            break;
        case COIN_APPEARING:
            appearTimer = 0x3c00;
            SetVisible(1);
            PlayAnim(APIECE01_ANIM_APPEAR, 0, 0);
            break;
        case COIN_ON_GROUND:
            claimedBySam = 0;
            SetVisible(1);
            PlayAnim(APIECE01_ANIM_OBJET, 0, 0);
            break;
        case COIN_HELD:
            PlayAnim(APIECE01_ANIM_LINK, 0, 0);
            break;
    }
    state = next;
}

void GoldenCoins::Render(Camera *view)
{
    Vec3s scale;
    if (renderScale) {
        scale.x = (s16)renderScale;
        scale.y = (s16)renderScale;
        scale.z = (s16)renderScale;
        RenderScaled(view, &scale);
    } else {
        scale.x = 0x400;
        scale.y = 0x400;
        scale.z = 0x400;
        RenderScaled(view, &scale);
    }
}

/* BYTES(slot-scope, inferred): the nested block(s) only order the frame: their locals are allocated after the enclosing scope's */
s32 GoldenCoins::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    ScnObject *found;
    switch (msgId) {
        case MSG_DETECTOR_PING:
            if (state == COIN_BURIED) {
                revealed = 1;
                SetState(COIN_APPEARING);
            }
            return 1;
        case MSG_QUERY_ACTION:
            if (!claimedBySam && sender->GetClassId() == CLASSID_WOLF)
                return CTX_PICKUP;
            break;
        case MSG_PICKUP: {
            ScnObject *parent = sender;
            {
                u8 joint = (u8)(u32)arg; /* cast kept: this message's arg carries the joint number */
                AttachTo(parent, joint, 0, 0, 0, 0);
            }
            SetState(COIN_HELD);
            return 1;
        }
        case MSG_DROP: {
            Vec3s *drop = (Vec3s *)arg; /* cast kept: this message's arg is the drop position */
            Detach();
            SetPosition(drop);
            SetState(COIN_ON_GROUND);
            return 1;
        }
        case MSG_INVENTORY_STORED:
            inInventory = 1;
            return 1;
        case MSG_INVENTORY_TAKE_OUT:
            inInventory = 0;
            return 1;
        case MSG_QUERY_HELD_ACTION:
            return HELD_THROWABLE;
        case MSG_KILL:
            SetState(COIN_POCKETED);
            break;
        case MSG_COIN_IS_HELD:
            if (state == COIN_HELD)
                return 1;
            return 0;
        case MSG_COIN_IS_TAKEN:
            if (state == COIN_ON_GROUND && !inInventory)
                return 0;
            return 1;
        case MSG_COIN_SET_SCALE:
            renderScale = (s32)arg; /* cast kept: this message's arg carries the scale */
            break;
        case MSG_COIN_CLAIM:
            claimedBySam = 1;
            break;
        case MSG_QUERY_NEAREST_TARGET: {
            CoinReport *report = (CoinReport *)arg; /* cast kept: this message's arg is the reply record */
            if (hintPending) {
                report->classId = CLASSID_GOLDENCOINS;
                report->proximity = (hintDist * 0xffff) / 600;
                hintPending = 0;
                return 1;
            }
            found = Scenaric_FindBestInRadius(&pos, pos.y - 150, pos.y + 150, 600, &hintDist,
                                              GoldenCoins_NearestCoinScoreCB, 0);
            if (found) {
                report->classId = found->GetClassId();
                report->proximity = (hintDist * 0xffff) / 600;
                return 1;
            }
            return 0;
        }
        case MSG_COIN_SET_HINT_DIST:
            hintDist = (u16)(u32)arg; /* cast kept: this message's arg carries the distance */
            hintPending = 1;
            break;
        case MSG_CHECKPOINT_ROLLBACK:
            Reset();
            return 1;
    }
    return 0;
}

u32 GoldenCoins_NearestCoinScoreCB(ScnObject *self, ScnObject *candidate, u32 distanceSquared, ScnObject *best)
{
    if (candidate->GetClassId() == CLASSID_GOLDENCOINS)
        return distanceSquared + 360000;
    return -1;
}

void GoldenCoins::Reset()
{
    if (revealed)
        SetState(COIN_ON_GROUND);
    else
        SetState(COIN_BURIED);
    if (IsInWorld())
        SetPosition(&homePos);
    renderScale = 0;
}

/* BYTES(slot-scope, inferred): the nested block(s) only order the frame: their locals are allocated after the enclosing scope's */
/* BYTES(dead-code): unusedClass is stored and never read, as in the original */
ScnObject *GoldenCoins_Create(void *record)
{
    /* The original stores 0x77 to a word local without subsequently reading it. */
    u16 unusedClass;
    {
        ScnBody *obj = new GoldenCoins;
        unusedClass = 0x77;
        obj = obj->Init(record, 0);
        return obj;
    }
}
