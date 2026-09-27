/*
 * T171 - original object InflatableSheep.cpp (guessed name), one translation unit.
 *   .text  0x4cc5b0-0x4cdcf3 (InflatableSheep_Respawn .. InflatableSheep_Create, in address order: Update and
 *          HandleMessage come before Render, as in the exe)
 *   .rdata 0x576474-0x576498 (??_7InflatableSheep)
 *   .data  0x57b678-0x57b6c4 (g_inflatableSheepStateTable, g_inflatableSheepRippleFxParams, defined in place)
 */
/* BYTES: layout, slot-group, view. */
/* BYTES(layout): not const: the original has it in .data */
/* BYTES(view): view: inline storage begins with the particleemitter header (the access widths are the original's) */
/* PAL PC InflatableSheep. Helper names and local work records are reconstructed from the listed originals. */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *instance, Animator *animator, u16 id, u32 options);
#include "../engine/scn_tools.h"
#include "../engine/scenaric.h"
#include "../engine/progress.h"
#include "sheep.h"
#include "sam_api.h"
#include "../engine/progress_inventory.h"
#include "../engine/approach.h"
#include "../engine/maths.h"
#include "instance.h"
#define SDW_INLINE_FREE_INSTFLAGS_SET_U16_U16 1
#include "instance_inlines.h"
#undef SDW_INLINE_FREE_INSTFLAGS_SET_U16_U16

#define SDW_INLINE_FREE_INSTFLAGS_CLEAR_U16_U16 1
#include "instance_inlines.h"
#undef SDW_INLINE_FREE_INSTFLAGS_CLEAR_U16_U16

#define SDW_MEMBERS_ScnObject                                                            \
    static void *operator new(u32 size);                                                 \
    void AttachTo(ScnObject *parent, u8 joint, Vec3s *offset, Vec3s *rot, u32 a, u32 b); \
    s32 IsBoxCollideEnabled()                                                            \
    {                                                                                    \
        return (flags & SCN_OF_NO_BOX_COLLIDE) == 0;                                     \
    }                                                                                    \
    void SetTint(u32 color, s16 amount, s32 on);


#define SDW_MEMBERS_CollBox s32 ContainsXZ(const Vec3s *point);

#define INFLATABLE_EMITTER_METHODS             \
    void RenderFlat(Camera *view, s32 forward) \
    {                                          \
        if (forward)                           \
            Emitter_RenderFlat_Fwd(view);      \
        else                                   \
            Emitter_RenderFlat(view);          \
    }
#define SDW_MEMBERS_ParticleEmitter INFLATABLE_EMITTER_METHODS
#define SDW_MEMBERS_InlineEmitter4 InlineEmitter4();

#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_GETHEADING 1
#define SDW_INLINE_SCNOBJECT_SETHEADING_S16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_GETHEADING
#undef SDW_INLINE_SCNOBJECT_SETHEADING_S16
#define SDW_INLINE_SCNOBJECT_ISINWORLD 1
#define SDW_INLINE_SCNOBJECT_ISKEPT 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_ISINWORLD
#undef SDW_INLINE_SCNOBJECT_ISKEPT
#define SDW_INLINE_SCNOBJECT_INSTFLAGS_U16 1
#define SDW_INLINE_SCNOBJECT_SETKEPT_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_INSTFLAGS_U16
#undef SDW_INLINE_SCNOBJECT_SETKEPT_S32
#define SDW_INLINE_SCNOBJECT_ENABLEBOXCOLLIDE_S32 1
#define SDW_INLINE_SCNOBJECT_SETVISIBLE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_ENABLEBOXCOLLIDE_S32
#undef SDW_INLINE_SCNOBJECT_SETVISIBLE_S32
#define SDW_INLINE_SCNOBJECT_SETTINT_U32_S16_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETTINT_U32_S16_S32
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_SCNMOBILE_SETSHADOWRADIUS_U8 1
#include "../engine/scn_mobile_inlines.h"
#undef SDW_INLINE_SCNMOBILE_SETSHADOWRADIUS_U8
#define SDW_INLINE_SHADOW_SETVISIBLE_S32 1
#include "../engine/shadow_inlines.h"
#undef SDW_INLINE_SHADOW_SETVISIBLE_S32
#define SDW_INLINE_SHADOW_REPROJECT 1
#include "../engine/shadow_inlines.h"
#undef SDW_INLINE_SHADOW_REPROJECT
#define SDW_INLINE_COLLBOX_CONTAINSXZ_CONST_VEC3S 1
#include "../engine/coll_box_inlines.h"
#undef SDW_INLINE_COLLBOX_CONTAINSXZ_CONST_VEC3S
#define SDW_INLINE_ZONELIST_FINDCONTAINING_VEC3S 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_FINDCONTAINING_VEC3S
#define SDW_INLINE_PROGRESS_GETLEVEL 1
#include "../engine/progress_inlines.h"
#undef SDW_INLINE_PROGRESS_GETLEVEL
inline InlineEmitter4::InlineEmitter4()
{
    base.slotPool = slotBuf;
    base.particles = particleBuf;
    base.count = 4;
    base.Emitter_Reset();
}
/* Inline storage begins with the ParticleEmitter header. */

struct InflatableStateEntry {
    u16 animId;
    u16 flags;
};

/* 0x57b678 - anim id and flags per state (14 states) */
InflatableStateEntry g_inflatableSheepStateTable[] = {
    {AMOUGO01_ANIM_OBJET, INFSHEEP_SF_NOT_SOLID},
    {AMOUGO01_ANIM_LINK, INFSHEEP_SF_NOT_SOLID | INFSHEEP_SF_NO_SHADOW},
    {AMOUGO01_ANIM_OBJET, INFSHEEP_SF_NOT_SOLID},
    {AMOUGO01_ANIM_INFLAT2, INFSHEEP_SF_NOT_SOLID | INFSHEEP_SF_NO_SHADOW},
    {AMOUGO01_ANIM_INFLAT5, INFSHEEP_SF_NOT_SOLID | INFSHEEP_SF_NO_SHADOW},
    {AMOUGO01_ANIM_LINK2, INFSHEEP_SF_NOT_SOLID | INFSHEEP_SF_NO_SHADOW},
    {AMOUGO01_ANIM_INFLAT5, 0},
    {AMOUGO01_ANIM_INFLAT5, 0},
    {AMOUGO01_ANIM_INFLAT5, 0},
    {AMOUGO01_ANIM_INFLAT4, INFSHEEP_SF_NOT_SOLID},
    {AMOUGO01_ANIM_INFLAT4, INFSHEEP_SF_NOT_SOLID},
    {AMOUGO01_ANIM_OBJET, INFSHEEP_SF_NOT_SOLID | INFSHEEP_SF_HIDDEN},
    {AMOUGO01_ANIM_OBJET, INFSHEEP_SF_NOT_SOLID},
    {AMOUGO01_ANIM_INFLAT5, 0}};
extern s32 g_dt;
extern u32 g_gameTime;
/* 0x57b6b0 - the water ripples; not const: Update rewrites spawnInterval every frame */
EmitterFadeParams g_inflatableSheepRippleFxParams = {8192, 0, 16384, 80, 250, 1};
extern "C" const s16 g_sinTable4096[];
extern "C" const s16 *g_pCosTable;
s32 ObjGrid_QueryBoxPoints(const CollBox *box, ScnObject **out);
#include "instance.h"
#define SDW_INLINE_SCNOBJECT_SETATTACHMENT_U8_CONST_VEC3S_CONST_VEC3S_S32_CONST_VEC3S 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETATTACHMENT_U8_CONST_VEC3S_CONST_VEC3S_S32_CONST_VEC3S

#define SDW_INLINE_FREE_ZONES_GET_U8 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_FREE_ZONES_GET_U8

#define SDW_INLINE_SCNOBJECT_GETFIRSTMODELBOXINLINE 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETFIRSTMODELBOXINLINE

void InflatableSheep::Respawn()
{
    SetKept(0);
    SetPosition(&homePos);
    stateTime = g_gameTime;
    SetState(INFSHEEP_ST_RESPAWN_HIDDEN, 0);
    UpdateTint();
}

void InflatableSheep::Pop()
{
    if (isFlags.inflated) {
        SetDeflated();
        SetState(INFSHEEP_ST_POPPING, 1);
    } else
        Respawn();
}

s32 InflatableSheep::CheckDeathZone()
{
    if (Zones_Get(ZONE_DEATH)->FindContaining(&pos)) {
        Pop();
        return 1;
    }
    return 0;
}

s32 InflatableSheep::CheckSquashed(s32 popAfter)
{
    /* The array has 64 slots, with VC6 alignment padding above it. */
    s32 count;
    ScnObject *other;
    ScnObject *nearObjectsList[64];
    s32 i;
    CollBox box;
    s32 found;
    box.Box_Translate(GetFirstModelBoxInline(), &pos);
    box.min.y -= 40;
    count = ObjGrid_QueryBoxPoints(&box, nearObjectsList);
    found = 0;
    i = 0;
    while (i < count && !found) {
        other = nearObjectsList[i];
        if (this != other && other->GetClassId() != CLASSID_WOODEN_LIFT && other->GetClassId() != CLASSID_SAM &&
            other->IsBoxCollideEnabled() && other->GetFirstSolidBox() && other->GetClassId() != CLASSID_MONOLITHE)
            found = 1;
        ++i;
    }
    if (found) {
        SetDeflated();
        if (popAfter)
            SetState(INFSHEEP_ST_POPPING, 1);
        else
            SetState(INFSHEEP_ST_DEFLATING, 1);
    }
    return found;
}

void InflatableSheep::ApplyFall(Vec3s *delta)
{
    s32 speed = fallTime * 1000 >> 12;
    if (speed > 1200)
        speed = 1200;
    delta->y += (s16)((speed * g_dt) / 4096);
}

void InflatableSheep::SetDeflated()
{
    isFlags.inflated = 0;
    shadow.radius = 20;
    shadow.Reproject();
}

void InflatableSheep::SetInflated()
{
    isFlags.inflated = 1;
    SetShadowRadius((u8)GetFirstModelBoxInline()->max.x);
    shadow.Reproject();
}

void InflatableSheep::SetState(u8 value, s32 blend)
{
    InflatableStateEntry *entry = &g_inflatableSheepStateTable[value];
    if (entry->flags & INFSHEEP_SF_NOT_SOLID)
        EnableBoxCollide(0);
    else
        EnableBoxCollide(1);
    if (entry->flags & INFSHEEP_SF_NO_SHADOW)
        shadow.SetVisible(0);
    else
        shadow.SetVisible(1);
    if (entry->flags & INFSHEEP_SF_HIDDEN)
        SetVisible(0);
    else
        SetVisible(1);
    state = value;
    PlayAnim(entry->animId, entry->flags & INFSHEEP_SF_LOOP, blend);
}

void InflatableSheep::UpdateTint()
{
    if (isFlags.burnt)
        SetTint(0, 0x1000, 1);
    else if (Zones_Get(ZONE_SHADOW)->FindContaining(&pos))
        SetTint(0, 0xc00, 1);
    else
        SetTint(0, 0, 0);
}

/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; unused48, unused3e, unused36, unused2a, unused02 fill gaps */
void InflatableSheep::Update()
{
    /* Original 0x4c-byte area: water -4c, ground -46, delta -44,
       velocity -3c, contact flags -34, flow heading -32, ripple -30,
       flow speed -28, collision details -24, rotation -8. */
    struct Work {
        u16 unused48;
        s16 ground;
        Vec3s delta;
        u16 unused3e;
        Vec3s velocity;
        u16 unused36;
        u16 hit;
        s16 flowHeading;
        Vec3s ripple;
        u16 unused2a;
        s32 flowSpeed;
        ContactInfo contact;
        Vec3s rotation;
        u16 unused02;
    } w;
    Box *waterBox = 0;
    switch (state) {
        case INFSHEEP_ST_CARRIED_FLAT:
            UpdateTint();
            break;
        case INFSHEEP_ST_FALL_FLAT:
            fallTime += g_dt;
            w.delta.x = 0;
            w.delta.y = 0;
            w.delta.z = 0;
            ApplyFall(&w.delta);
            w.ground = World_GroundYRay(&pos, 0);
            if (pos.y + w.delta.y >= w.ground) {
                w.delta.y = w.ground - pos.y;
                SetState(INFSHEEP_ST_FLAT, 1);
            }
            Translate(&w.delta);
            UpdateTint();
            break;
        case INFSHEEP_ST_INFLATING:
            if (AnimFlags(ANIM_F_FINISHED)) {
                SetInflated();
                w.rotation.x = w.rotation.z = 0;
                w.rotation.y = carryYaw;
                SetAttachment(carryJoint, 0, &w.rotation, 1, 0);
                SetState(INFSHEEP_ST_CARRIED_INFLATED, 0);
            }
            UpdateTint();
            break;
        case INFSHEEP_ST_CARRIED_INFLATED:
        case INFSHEEP_ST_CARRIED_BY_SAM:
            UpdateTint();
            break;
        case INFSHEEP_ST_STANDING:
            if (!CheckSquashed(0)) {
                w.delta.x = 0;
                w.delta.y = 10;
                w.delta.z = 0;
                if (!Collide_ResolveMove(&w.delta, &w.contact, 0xb54, COLL_FLOOR | COLL_FLOOR_EDGE, 0, 0, 10, 0, 0))
                    SetState(INFSHEEP_ST_FALLING, 0);
                else {
                    waterBox = Zones_Get(ZONE_WATER)->FindContaining(&pos);
                    if (waterBox && pos.y >= waterBox->min[1] + 20) {
                        SetState(INFSHEEP_ST_FLOATING, 1);
                        driftSpeed = 0;
                        driftHeading = 0;
                    }
                }
            }
            break;
        case INFSHEEP_ST_FALLING:
            fallTime += g_dt;
            w.delta.x = 0;
            w.delta.y = 0;
            w.delta.z = 0;
            ApplyFall(&w.delta);
            w.hit = Collide_ResolveMove(&w.delta, &w.contact, 0xb54, COLL_WALL, 0, 0, 10, 0, 0);
            Translate(&w.delta);
            if (!CheckDeathZone() && (w.hit & COLL_FLOOR) && !w.contact.movableObj) {
                fallTime = 0;
                SetState(INFSHEEP_ST_STANDING, 1);
            } else {
                waterBox = Zones_Get(ZONE_WATER)->FindContaining(&pos);
                if (waterBox && pos.y >= waterBox->min[1] + 20) {
                    SetState(INFSHEEP_ST_FLOATING, 1);
                    driftSpeed = 0;
                    driftHeading = 0;
                }
            }
            UpdateTint();
            break;
        case INFSHEEP_ST_FLOATING:
            fallTime = 0;
            waterBox = Zones_Get(ZONE_WATER)->FindContaining(&pos);
            if (waterBox) {
                Zone_GetFlowHeading(waterBox, &w.flowHeading, &w.flowSpeed);
                if (!driftSpeed)
                    driftHeading = w.flowHeading;
                else
                    driftHeading = Math_StepAngleTowards(driftHeading, w.flowHeading, 0x2800);
            } else
                w.flowSpeed = 0;
            driftSpeed = Math_ApproachLinear(driftSpeed, w.flowSpeed, 2000, 50, 100);
            if (driftSpeed) {
                w.velocity.x = (s16)(-driftSpeed * g_sinTable4096[driftHeading] >> 12);
                w.velocity.z = (s16)(-driftSpeed * g_pCosTable[driftHeading] >> 12);
                w.velocity.y = 0;
                Vec3s_ScaleByDt(&w.velocity, &w.delta);
                Collide_ResolveMove(&w.delta, 0, 0xb54, COLL_WALL | RESOLVE_KEEP_Y, 0, 0, 10, 0, 0);
                Translate(&w.delta);
            }
            if (!CheckDeathZone() && !CheckSquashed(0) && (!waterBox || pos.y < waterBox->min[1] + 20))
                SetState(INFSHEEP_ST_FALLING, 1);
            UpdateTint();
            break;
        case INFSHEEP_ST_DEFLATING:
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(INFSHEEP_ST_FLAT, 1);
            break;
        case INFSHEEP_ST_POPPING:
            if (AnimFlags(ANIM_F_FINISHED))
                Respawn();
            break;
        case INFSHEEP_ST_RESPAWN_HIDDEN:
            if (g_gameTime - stateTime >= 0x3000) {
                stateTime = g_gameTime;
                SetState(INFSHEEP_ST_RESPAWN_WOBBLE, 0);
            }
            break;
        case INFSHEEP_ST_RESPAWN_WOBBLE:
            if (IsRespawnWobbleDone(g_gameTime - stateTime))
                SetState(INFSHEEP_ST_FLAT, 1);
            break;
    }
    if (emitter.base.flags.active || waterBox) {
        if (waterBox) {
            w.ripple = pos;
            w.ripple.y = waterBox->min[1];
            g_inflatableSheepRippleFxParams.spawnInterval = Rand_Range(0x2000, 0x6000);
            emitter.base.Emitter_UpdateFade(&g_inflatableSheepRippleFxParams, &w.ripple, GetHeading(), 1);
        } else
            emitter.base.Emitter_UpdateFade(&g_inflatableSheepRippleFxParams, &pos, GetHeading(), 0);
    }
    AdvanceAnim();
}

/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; unused fill gaps */
s32 InflatableSheep::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    /* Declared locals are the 16-byte area before inline getter temporaries. */
    struct Work {
        DropMsgArg *drop;
        Vec3s rotation;
        u16 unused;
        ScnObject *holder;
    } w;
    if (msgId < MSG_WOLF_CAUGHT) {
        switch (msgId) {
            case MSG_QUERY_ACTION:
                if (state == INFSHEEP_ST_GRABBED_BY_SAM)
                    return CTX_NONE;
                switch (sender->GetClassId()) {
                    case CLASSID_WOLF:
                        if (isFlags.inflated)
                            return CTX_LIFT;
                        if (state == INFSHEEP_ST_FLAT)
                            return CTX_PICKUP;
                        break;
                    case CLASSID_ROBOT:
                        if (state == INFSHEEP_ST_FLAT)
                            return CTX_PICKUP;
                        break;
                }
                break;
            case MSG_SCRIPT_HOLD:
                SetState(INFSHEEP_ST_GRABBED_BY_SAM, 1);
                break;
            case MSG_SCRIPT_RELEASE:
                SetState(INFSHEEP_ST_FALLING, 1);
                break;
            case MSG_PICKUP:
                w.holder = sender;
                /* cast kept: HandleMessage's arg is a void *; MSG_PICKUP passes the joint number in it */
                carryJoint = (u8)(u32)arg;
                if (isFlags.inflated) {
                    if (sender->GetClassId() == CLASSID_SAM) {
                        SetState(INFSHEEP_ST_CARRIED_BY_SAM, 0);
                        carryYaw = 0;
                        AttachTo(w.holder, carryJoint, 0, 0, 0, 0);
                    } else {
                        SetState(INFSHEEP_ST_CARRIED_INFLATED, 0);
                        carryYaw = (GetHeading() - w.holder->GetHeading()) & 0xfff;
                        w.rotation.x = w.rotation.z = 0;
                        w.rotation.y = carryYaw;
                        AttachTo(w.holder, carryJoint, 0, &w.rotation, 1, 0);
                    }
                } else {
                    SetState(INFSHEEP_ST_CARRIED_FLAT, 0);
                    carryYaw = 0;
                    AttachTo(w.holder, carryJoint, 0, 0, 0, 0);
                }
                return 1;
            case MSG_DROP:
                /* cast kept: HandleMessage's arg is a void *; MSG_DROP passes the drop record */
                w.drop = (DropMsgArg *)arg;
                Detach();
                if (!w.drop->flag1)
                    SetPosition(&w.drop->pos);
                SetHeading((sender->GetHeading() + carryYaw) & 0xfff);
                if (!CheckDeathZone()) {
                    if (isFlags.inflated) {
                        fallTime = 0;
                        SetState(INFSHEEP_ST_FALLING, 1);
                    } else
                        SetState(INFSHEEP_ST_FLAT, 0);
                }
                return 1;
            case MSG_QUERY_HELD_ACTION:
                if (sender->GetClassId() == CLASSID_ROBOT || isFlags.inflated)
                    return HELD_THROWABLE;
                return HELD_INFLATABLE_SHEEP;
            case MSG_CONTAINER_STATE:
                switch ((u32)arg) { /* cast kept: HandleMessage's arg is a void *; this message passes a number in it */
                    case CONTAINER_RELEASED:
                        homePos = pos;
                        break;
                }
                return 1;
            case MSG_CHECKPOINT_ROLLBACK:
                Reset();
                return 1;
            case MSG_KILL:
                switch ((u32)arg) { /* cast kept: HandleMessage's arg is a void *; this message passes a number in it */
                    case KILL_CRUSH:
                    case KILL_PIRANHAS:
                        if (!InstFlags(INST_F_ATTACHED) && state != INFSHEEP_ST_POPPING)
                            Pop();
                        return 1;
                    case KILL_GENERIC:
                        if (InstFlags(INST_F_ATTACHED))
                            isFlags.burnt = 1;
                        break;
                }
                break;
            case MSG_LANDED:
                Pop();
                break;
        }
    } else {
        switch (msgId) {
            case MSG_INFSHEEP_INFLATE:
                SetState(INFSHEEP_ST_INFLATING, 1);
                return 1;
            case MSG_INFLATABLE_QUERY:
                if (isFlags.inflated)
                    return 1;
                break;
            case MSG_INFSHEEP_DEFLATE_RESET:
                if (isFlags.inflated) {
                    SetDeflated();
                    SetState(INFSHEEP_ST_DEFLATING, 1);
                }
                Reset();
                return 1;
        }
    }
    return 0;
}

void InflatableSheep::Render(Camera *view)
{
    if (state == INFSHEEP_ST_RESPAWN_WOBBLE)
        RenderRespawnWobble(view, g_gameTime - stateTime);
    else
        ScnMobile::Render(view);
    if (emitter.base.flags.active)
        emitter.base.RenderFlat(view, 1);
}

/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; unused fill gaps */
void InflatableSheep::Reset()
{
    /* Storage/order observed at 4cd88c, not a claimed original struct. */
    struct Work {
        s32 special;
        s32 doDefault;
        ScnObject *sheep;
        u16 unused;
        u16 index;
        CollBox *box;
    } w;
    w.index = 0;
    w.box = 0;
    w.special = 0;
    w.sheep = 0;
    fallTime = 0;
    stateTime = g_gameTime;
    driftSpeed = 0;
    driftHeading = 0;
    emitter.base.Emitter_Reset();
    w.doDefault = 1;
    if (isFlags.inflated) {
        w.special = 0;
        if (g_pProgress->GetLevel() == SCENE_LVL_15 && IsInWorld()) {
            w.sheep = g_pSheepOutOfZone;
            if (w.sheep)
                w.sheep->Reset();
            w.sheep = g_pSheepOutOfZone;
            if (!w.sheep) {
                for (w.index = 0; w.index < g_samWolfCanBeHitZoneCount; ++w.index) {
                    w.box = g_samWolfCanBeHitZoneBoxes[w.index];
                    if (w.box && w.box->ContainsXZ(&pos)) {
                        SetDeflated();
                        PlayAnim(AMOUGO01_ANIM_OBJET, 0, 0);
                        w.doDefault = 0;
                        w.special = 1;
                        break;
                    }
                }
            }
        }
        if (IsKept() && w.special) {
            SetDeflated();
            RemoveFromWorld();
            Inventory_Add(this);
            w.doDefault = 0;
        } else {
            if (state != INFSHEEP_ST_FLOATING)
                SnapToGround(1);
            SetState(INFSHEEP_ST_FALLING, 0);
            if (pos.y == 32000 || isFlags.burnt)
                SetDeflated();
            else
                w.doDefault = 0;
        }
    }
    if (w.doDefault) {
        if (IsInWorld()) {
            if (IsKept()) {
                RemoveFromWorld();
                Inventory_Add(this);
            } else
                SetPosition(&homePos);
        }
        SetState(INFSHEEP_ST_FLAT, 0);
    }
    isFlags.burnt = 0;
    UpdateTint();
}

void InflatableSheep::PostLoadInit()
{
    if (IsInWorld())
        SnapToGround(1);
    homePos = pos;
    fallTime = 0;
    stateTime = g_gameTime;
    driftSpeed = 0;
    driftHeading = 0;
    emitter.base.Emitter_Reset();
    isFlags.burnt = 0;
    SetDeflated();
    UpdateTint();
    SetState(INFSHEEP_ST_FLAT, 0);
}

ScnObject *InflatableSheep_Create(void *record)
{
    InflatableSheep *object = new InflatableSheep;
    object = (InflatableSheep *)object->Init(record, 0); /* cast kept: Init returns this as a ScnBody * */
    return object;
}
