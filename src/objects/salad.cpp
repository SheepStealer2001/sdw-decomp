/* PAL PC Salad, 0x4e7db0-0x4e8ac9. */
/* BYTES: slot-group. */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *inst, Animator *animator, u16 id, u32 opts); /* 0x550196 */

#define SDW_MEMBERS_ScnObject                                                                          \
    static void *operator new(u32 size);                                                               \
    void AttachTo(ScnObject *parent, u8 joint, Vec3s *offset, Vec3s *rotation, u32 arg, u32 arg2);     \
    ScnObject *Scenaric_FindBestInRadius(Vec3s *center, s16 minY, s16 maxY, u16 radius, u16 *distance, \
                                         u32 (*score)(ScnObject *, ScnObject *, u32, ScnObject *),     \
                                         s32 includeFlagged);                                          \
    void SetRotation(Vec3s *rotation);


#include "sdw_classes.h"
#include "../engine/maths.h"
#include "../engine/progress_inventory.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_ISINWORLD 1
#define SDW_INLINE_SCNOBJECT_ISKEPT 1
#define SDW_INLINE_SCNOBJECT_ISVISIBLE 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_ISINWORLD
#undef SDW_INLINE_SCNOBJECT_ISKEPT
#undef SDW_INLINE_SCNOBJECT_ISVISIBLE
#define SDW_INLINE_SCNOBJECT_INSTFLAGS_U16 1
#define SDW_INLINE_SCNOBJECT_SETVISIBLE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_INSTFLAGS_U16
#undef SDW_INLINE_SCNOBJECT_SETVISIBLE_S32
#define SDW_INLINE_SCNOBJECT_SETKEPT_S32 1
#define SDW_INLINE_SCNOBJECT_SETROTATION_VEC3S 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETKEPT_S32
#undef SDW_INLINE_SCNOBJECT_SETROTATION_VEC3S
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_SHADOW_SETENABLED_S32 1
#include "../engine/shadow_inlines.h"
#undef SDW_INLINE_SHADOW_SETENABLED_S32
#define SDW_INLINE_ALTMODEL_ISVALID 1
#include "../engine/alt_model_inlines.h"
#undef SDW_INLINE_ALTMODEL_ISVALID
extern u32 g_gameTime;
extern s32 g_dt;
extern s32 g_dtMs;
/* Radius-search score callback: four cdecl arguments. */
u32 Salad_ScoreAttractTargetCB(ScnObject *self, ScnObject *candidate, u32 distanceSquared, ScnObject *selectedSoFar);

/* 0x4e7db0 */
void Salad::SetState(u8 newState)
{
    if (newState == SALAD_ST_SQUASH) {
        if (!squashed && squashedModel.IsValid()) {
            SwapModel(&squashedModel);
            squashed = 1;
        }
    } else if (squashed) {
        SwapModel(&mainModel);
        squashed = 0;
    }
    state = newState;
}

/* 0x4e7e61 */
void Salad::Consume()
{
    liftBlocked = 0;
    liftBlockMs = 0;
    SetVisible(0);
    SetKept(0);
    SetPosition(&homePos);
    SetRotation(g_pZeroVec3s);
    stateTime = g_gameTime;
    eatTime = 0;
    SetState(SALAD_ST_GONE);
    PlayAnim(ALAITU01_ANIM_OBJET, 0, 0);
}

/* 0x4e7f7c */
void Salad::Update()
{
    if (liftBlocked) {
        liftBlockMs += g_dtMs;
        if (liftBlockMs > 1000)
            liftBlocked = 0;
    }
    switch (state) {
        case SALAD_ST_GONE:
            if (g_gameTime - stateTime >= 0xa000) {
                SetVisible(1);
                stateTime = g_gameTime;
                SetState(SALAD_ST_RESPAWN);
            }
            break;
        case SALAD_ST_RESPAWN:
            if (IsRespawnWobbleDone(g_gameTime - stateTime))
                SetState(SALAD_ST_IDLE);
            break;
        case SALAD_ST_SQUASH:
            if (AnimFlags(ANIM_F_FINISHED))
                Consume();
            break;
    }
    AdvanceAnim();
}

/* 0x4e8091 */
/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets */
s32 Salad::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    struct {
        u16 *out;
        DropMsgArg *drop;
        u8 pad0[3], joint;
        ScnObject *parent;
        u16 pad1, distance;
        ScnObject *found;
    } w;
    switch (msgId) {
        case MSG_LIFT_CRUSH:
            if (IsInWorld()) {
                liftBlocked = 1;
                liftBlockMs = 0;
            }
            break;
        case MSG_QUERY_ACTION:
            if (IsInWorld()) {
                if ((sender->GetClassId() == CLASSID_WOLF || sender->GetClassId() == CLASSID_ROBOT) &&
                    state == SALAD_ST_IDLE)
                    return CTX_PICKUP;
                if (sender->GetClassId() == CLASSID_SHEEP) {
                    if (liftBlocked)
                        return CTX_NONE;
                    return SHEEP_ATTR_SALAD;
                }
            }
            break;
        case MSG_PICKUP:
            w.parent = sender;
            w.joint = (u8)(u32)arg; /* cast kept: this message passes the joint number in its void * */
            AttachTo(w.parent, w.joint, 0, 0, 0, 0);
            SetState(SALAD_ST_IDLE);
            if (w.parent->GetClassId() == CLASSID_SALADROD) {
                shadow.SetEnabled(1);
                PlayAnim(ALAITU01_ANIM_CANLINK, 0, 0);
            } else {
                shadow.SetEnabled(0);
                if (w.parent->GetClassId() == CLASSID_WOLF)
                    PlayAnim(ALAITU01_ANIM_LINK, 0, 0);
                else
                    PlayAnim(ALAITU01_ANIM_OBJET, 0, 0);
            }
            return 1;
        case MSG_DROP:
            w.drop = (DropMsgArg *)arg; /* cast kept: the message arg is a void *; MSG_DROP passes a DropMsgArg */
            Detach();
            if (!w.drop->flag1)
                SetPosition(&w.drop->pos);
            shadow.SetEnabled(1);
            SetState(SALAD_ST_IDLE);
            PlayAnim(ALAITU01_ANIM_OBJET, 0, 0);
            return 1;
        case MSG_QUERY_NEAREST_TARGET:
            w.out = (u16 *)arg; /* cast kept: the message arg is a void *; this query passes a u16[2] reply */
            w.found = Scenaric_FindBestInRadius(&pos, pos.y - 150, pos.y + 150, 600, &w.distance,
                                                Salad_ScoreAttractTargetCB, 0);
            if (w.found) {
                w.out[0] = w.found->GetClassId();
                w.out[1] = (s32)(w.distance * 65535) / 600;
                return 1;
            } else {
                return 0;
            }
        case MSG_USE:
            if (sender->GetClassId() == CLASSID_SHEEP && state == SALAD_ST_IDLE && !InstFlags(INST_F_ATTACHED) &&
                IsInWorld() && IsVisible()) {
                eatTime += g_dt;
                if (eatTime >= 0x3000)
                    Consume();
                return 1;
            } else {
                return 0;
            }
        case MSG_CONTAINER_STATE:
            switch ((u32)arg) { /* cast kept: this message passes a number in its void * argument */
                case CONTAINER_RELEASED:
                    homePos = pos;
                    break;
            }
            return 1;
        case MSG_CHECKPOINT_ROLLBACK:
            Reset();
            return 1;
        case MSG_QUERY_HELD_ACTION:
            return HELD_THROWABLE;
        case MSG_KILL:
            switch ((u32)arg) { /* cast kept: this message passes a number in its void * argument */
                case KILL_CRUSH:
                case KILL_RAFT:
                    SetState(SALAD_ST_SQUASH);
                    PlayAnim(ALAITU01_ANIM_APPEAR, 0, 0);
                    return 1;
            }
            break;
        case MSG_LANDED:
            if (!InstFlags(INST_F_ATTACHED)) {
                SetState(SALAD_ST_SQUASH);
                PlayAnim(ALAITU01_ANIM_APPEAR, 0, 0);
                return 1;
            }
            break;
        case MSG_GEYSER_OUT:
            if (state != SALAD_ST_SQUASH && state != SALAD_ST_GONE) {
                SetState(SALAD_ST_SQUASH);
                PlayAnim(ALAITU01_ANIM_APPEAR, 0, 0);
                SetRotation(&sender->rot);
            }
            return 0;
    }
    return 0;
}

/* 0x4e8762 */
u32 Salad_ScoreAttractTargetCB(ScnObject *self, ScnObject *candidate, u32 distanceSquared, ScnObject *selectedSoFar)
{
    if (candidate->GetClassId() == CLASSID_SHEEP) {
        if (candidate->HandleMessage(self, MSG_SHEEP_QUERY_AVAILABLE, 0))
            return distanceSquared;
    } else if (candidate->GetClassId() == CLASSID_SALAD) {
        return distanceSquared + 360000;
    }
    return -1;
}

/* 0x4e87c0 */
void Salad::Render(Camera *view)
{
    if (state == SALAD_ST_RESPAWN)
        RenderRespawnWobble(view, g_gameTime - stateTime);
    else
        ScnMobile::Render(view);
}

/* 0x4e8802 */
void Salad::Reset()
{
    liftBlocked = 0;
    liftBlockMs = 0;
    SetVisible(1);
    shadow.SetEnabled(1);
    SetState(SALAD_ST_IDLE);
    PlayAnim(ALAITU01_ANIM_OBJET, 0, 0);
    eatTime = 0;
    SetRotation(g_pZeroVec3s);
    if (IsInWorld()) {
        if (IsKept()) {
            RemoveFromWorld();
            Inventory_Add(this);
        } else {
            SetPosition(&homePos);
        }
    }
}

/* 0x4e895e */
void Salad::PostLoadInit()
{
    if (IsInWorld())
        SnapToGround(1);
    liftBlocked = 0;
    liftBlockMs = 0;
    eatTime = 0;
    stateTime = 0;
    homePos = pos;
    squashed = 0;
    shadow.radius = 20;
    SetState(SALAD_ST_IDLE);
    PlayAnim(ALAITU01_ANIM_OBJET, 0, 0);
}

/* 0x4e8a3f */
/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets */
ScnObject *Salad_Create(void *record)
{
    struct {
        u16 pad, alternate;
        Salad *obj;
    } w;
    w.obj = new Salad;
    w.alternate = WAR_IDO_ALAITU02;
    /* cast kept: InitWithAltModels returns the ScnObject base of this salad */
    w.obj = (Salad *)w.obj->InitWithAltModels(record, &w.obj->mainModel, 1, &w.alternate, &w.obj->squashedModel);
    return w.obj;
}
