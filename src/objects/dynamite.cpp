/* T138 - original object Dynamite.cpp (guessed name).
 * Ranges: .text 0x4b4e70-0x4b594f, .rdata 0x575f08-0x575f2c (vtable), .data 0x57b61c-0x57b64c (g_dynamiteStates,
 * g_dynamiteFuseFxParams). The two tables are in .data, so they are not const. */
/* PAL PC Dynamite. Generated fields, including its InlineEmitter3. */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *, Animator *, u16, u32);
#include "../engine/sound_mgr.h"
#include "../app/app_main.h"
#include "camera.h"

#define SDW_MEMBERS_ScnObject                                   \
    static void *operator new(u32 size);                        \
    void AttachTo(ScnObject *, u8, Vec3s *, Vec3s *, u32, u32); \
    void SetUpdateMode(u8 mode);


#define SDW_MEMBERS_InlineEmitter3 InlineEmitter3();
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_ISINWORLD 1
#define SDW_INLINE_SCNOBJECT_STOPSOUND_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_ISINWORLD
#undef SDW_INLINE_SCNOBJECT_STOPSOUND_U16
#define SDW_INLINE_SCNOBJECT_SETVISIBLE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETVISIBLE_S32
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_SHADOW_SETVISIBLE_S32 1
#include "../engine/shadow_inlines.h"
#undef SDW_INLINE_SHADOW_SETVISIBLE_S32
inline InlineEmitter3::InlineEmitter3()
{
    base.slotPool = slotBuf;
    base.particles = particleBuf;
    base.count = 3;
    base.Emitter_Reset();
}
extern u32 g_gameTime;
extern s32 g_dt;

#define g_camPos (g_camera.pos)

s32 Vec3s_Dist(Vec3s *, Vec3s *);
u16 Sound_Play(u16, void *, u16, u8, s32);
struct DynamiteStateEntry {
    u16 anim;
    u8 flags;
    u8 reserved;
};
/* 0x57b61c .data (non-const in the original: both tables sit in .data, not ahead of the vtable in .rdata) */
static DynamiteStateEntry g_dynamiteStates[] = {
    {ADYNAM01_ANIM_OBJET, DYN_SF_PICKUP | DYN_SF_SHADOW, 0}, /* DYNAMITE_ST_ON_GROUND */
    {ADYNAM01_ANIM_LINK, 0, 0},                              /* DYNAMITE_ST_HELD_BY_WOLF */
    {ADYNAM01_ANIM_LINK2, 0, 0},                             /* DYNAMITE_ST_HELD */
    {ADYNAM01_ANIM_STAND1, DYN_SF_SHADOW | DYN_SF_LOOP, 0},  /* DYNAMITE_ST_LIT */
    {ADYNAM01_ANIM_OBJET, 0, 0},                             /* DYNAMITE_ST_EXPLODING */
    {ADYNAM01_ANIM_OBJET, 0, 0},                             /* DYNAMITE_ST_RESPAWN_WAIT */
    {ADYNAM01_ANIM_OBJET, DYN_SF_SHADOW, 0}};                /* DYNAMITE_ST_RESPAWN_WOBBLE */
/* 0x57b638 .data */
static EmitterColumnParams g_dynamiteFuseFxParams[1] = {{-200, 0x800, 0x800, 0x10, 0x40, 7}};
struct DynamiteDrop {
    Vec3s point;
    u16 lit : 1;
};
void Dynamite::SetState(u8 nextState)
{
    const DynamiteStateEntry *entry = &g_dynamiteStates[nextState];
    PlayAnim(entry->anim, entry->flags & DYN_SF_LOOP, 0);
    shadow.SetVisible(entry->flags & DYN_SF_SHADOW);
    if (soundHandle) {
        StopSound(soundHandle);
        soundHandle = 0;
    }
    state = nextState;
}
void Dynamite::Update()
{
    s32 a;
    Vec3s b;
    switch (state) {
        case DYNAMITE_ST_LIT:
            fuse -= g_dt;
            if (fuse <= 0) {
                u16 c = GetClassId();
                s16 e = pos.y + 3000;
                s16 d = pos.y - 3000;
                /* cast kept: MSG_LOUD_NOISE's arg is the sender's class id, a number in the void * */
                Scenaric_BroadcastInRadius(CLASSID_NONE, d, e, 3000, MSG_LOUD_NOISE, (void *)(u32)c, 0);
                s16 g = pos.y + 200;
                s16 f = pos.y - 200;
                Scenaric_BroadcastInRadius(CLASSID_NONE, f, g, 300, MSG_KILL, 0, 0);
                a = Vec3s_Dist(&pos, &g_camPos);
                if (a < 5000)
                    Camera_StartShake((5000 - a) * 60 / 5000, 0x2000);
                emitter.base.Emitter_Reset();
                SwapModel(&explodedModel);
                dynFlags.exploded = 1;
                SetState(DYNAMITE_ST_EXPLODING);
            } else {
                b = pos;
                b.y -= 50;
                emitter.base.Emitter_UpdateColumn(g_dynamiteFuseFxParams, &b, fuse, 1);
            }
            break;
        case DYNAMITE_ST_EXPLODING:
            if (AnimFlags(ANIM_F_FINISHED)) {
                SwapModel(&normalModel);
                dynFlags.exploded = 0;
                SetVisible(0);
                SetPosition(&homePos);
                stateTimestamp = g_gameTime;
                SetState(DYNAMITE_ST_RESPAWN_WAIT);
                SetUpdateMode(SCN_UPD_NORMAL);
            }
            break;
        case DYNAMITE_ST_RESPAWN_WAIT:
            if ((u32)(g_gameTime - stateTimestamp) >= 0x3000) {
                SetVisible(1);
                stateTimestamp = g_gameTime;
                SetState(DYNAMITE_ST_RESPAWN_WOBBLE);
            }
            break;
        case DYNAMITE_ST_RESPAWN_WOBBLE:
            if (IsRespawnWobbleDone(g_gameTime - stateTimestamp))
                SetState(DYNAMITE_ST_ON_GROUND);
            break;
    }
    AdvanceAnim();
}
s32 Dynamite::HandleMessage(ScnObject *sender, u32 message, void *arg)
{
    ScnObject *a;
    u8 b;
    DynamiteDrop *c;
    switch (message) {
        case MSG_QUERY_ACTION:
            switch (sender->GetClassId()) {
                case CLASSID_WOLF:
                case CLASSID_ROBOT:
                    if (g_dynamiteStates[state].flags & DYN_SF_PICKUP)
                        return CTX_PICKUP;
                    break;
            }
            break;
        case MSG_PICKUP:
            a = sender;
            b = (u8)(u32)arg; /* cast kept: MSG_PICKUP's arg carries the joint number */
            AttachTo(a, b, 0, 0, 0, 0);
            if (sender->GetClassId() == CLASSID_WOLF)
                SetState(DYNAMITE_ST_HELD_BY_WOLF);
            else
                SetState(DYNAMITE_ST_HELD);
            return 1;
        case MSG_DROP:
            c = (DynamiteDrop *)arg; /* cast kept: MSG_DROP's arg is a DynamiteDrop */
            Detach();
            SetPosition(&c->point);
            if (c->lit) {
                SetState(DYNAMITE_ST_LIT);
                soundHandle =
                    Sound_Play(SND_SDYMECHE, this, 0xff, SNDF_LOOP | SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 0x1000);
                fuse = fuseTotal;
                SetUpdateMode(SCN_UPD_ALWAYS);
            } else
                SetState(DYNAMITE_ST_ON_GROUND);
            return 1;
        case MSG_QUERY_NEAREST_TARGET:
            return 0;
        case MSG_CONTAINER_STATE:
            switch ((u32)arg) { /* cast kept: MSG_CONTAINER_STATE's arg carries the container state */
                case CONTAINER_RELEASED:
                    homePos = pos;
                    break;
            }
            return 1;
        case MSG_QUERY_HELD_ACTION:
            return HELD_THROWABLE;
        case MSG_CHECKPOINT_ROLLBACK:
            Reset();
            return 1;
    }
    return 0;
}
void Dynamite::Render(Camera *view)
{
    if (state == DYNAMITE_ST_RESPAWN_WOBBLE)
        RenderRespawnWobble(view, g_gameTime - stateTimestamp);
    else
        ScnMobile::Render(view);
    if (state == DYNAMITE_ST_LIT && emitter.base.flags.active)
        emitter.base.Emitter_Render(view, 0);
}
void Dynamite::Reset()
{
    if (dynFlags.exploded) {
        SwapModel(&normalModel);
        dynFlags.exploded = 0;
    }
    SetState(DYNAMITE_ST_ON_GROUND);
    SetUpdateMode(SCN_UPD_NORMAL);
    SetVisible(1);
    emitter.base.Emitter_Reset();
    if (IsInWorld())
        SetPosition(&homePos);
}
void Dynamite::PostLoadInit()
{
    void *a = record;
    u32 b;
    s32 c;
    fuse = 0;
    stateTimestamp = 0;
    homePos = pos;
    b = 0;
    /* cast kept: designer properties are 4-byte slots at byte offsets of the raw WAR record */
    c = *(s32 *)((u8 *)a + b + 0x14);
    fuseTotal = (c << 12) / 1000;
    dynFlags.exploded = 0;
    soundHandle = 0;
    SetState(DYNAMITE_ST_ON_GROUND);
    shadow.radius = 25;
    emitter.base.Emitter_Reset();
}
ScnObject *Dynamite_Create(void *record)
{
    Dynamite *a = new Dynamite;
    u16 b = 3;
    /* cast kept: InitWithAltModels returns the object as its ScnObject base */
    a = (Dynamite *)a->InitWithAltModels(record, &a->normalModel, 1, &b, &a->explodedModel);
    return a;
}
