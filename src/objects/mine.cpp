/*
 * T184 - original object Mine.cpp (guessed name), one translation unit: the Mine base class and its two subclasses
 * GroundMine and DefusableMine, one file in the original (the Mine->GroundMine 0x4d525d and GroundMine->DefusableMine
 * 0x4d66ec transitions are unaligned).
 *   .text  0x4d4ce0-0x4d7cf6 (Mine_FindTrigger .. DefusableMine_Create)
 *   .rdata 0x576648-0x576708: g_defuseButtonTable (main CONST), then the vtable COMDATs of GroundMine 0x576690,
 *          Mine 0x5766b8 and DefusableMine 0x5766e0
 *   .data  0x57b6dc-0x57b704: g_groundMineFuseFxParams, g_defusableMineRingFxParams
 *   .bss   0x6cf688-0x6cf708: the four defuse-panel sprites and g_defusePadSprites[4]
 * The three classes in that order, each with its own notes in place. The data is defined in place;
 * g_defuseButtonTable is defined ahead of the functions: main CONST has to precede the vtable COMDATs in .rdata.
 */
/* BYTES: layout. */
/* BYTES(layout): written '= 0' only to keep definition order in .bss */
/* BYTES(layout): defined before the functions so main CONST precedes the vtable COMDATs in .rdata */
/* --- Mine: */
/* PAL PC 0x4d4ce0-0x4d525d. Uses the SetState(u8) declaration; the radius callback takes four arguments and returns
 * a u32 score. */
/* --- GroundMine: */
/* PAL PC GroundMine. Uses Mine::SetState(u8) and the generated
 * InlineEmitter3/4 member layouts. */
/* --- DefusableMine: */
/* PAL PC 0x4d66ec-0x4d7cf6. Uses the Mine/DefusableMine SetState(u8).
 * The Sprite Load/Thunk/Frame functions are declared locally. */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *inst, Animator *animator, u16 id, u32 opts);
#include "../engine/sound_mgr.h"
#include "camera.h"
#include "../app/app_main.h"
#include "../engine/scn_tools.h"
#include "../engine/id_list.h"
#include "sheep.h"
#include "../engine/input.h"

#define SDW_MEMBERS_ScnObject                                                                                   \
    static void *operator new(u32 size);                                                                        \
    ScnObject *Scenaric_FindBestInRadius(Vec3s *center, s16 bottom, s16 top, u16 radius, u16 *distance,         \
                                         u32 (*score)(ScnObject *, ScnObject *, u32, ScnObject *), s32 hidden); \
    s32 ForceDraw()                                                                                             \
    {                                                                                                           \
        return (flags & SCN_OF_NO_DIST_CULL) != 0;                                                              \
    }                                                                                                           \
    void SetUpdateMode(s32 mode);                                                                               \
    void AttachTo(ScnObject *parent, u8 joint, Vec3s *offset, Vec3s *rot, u32 arg, u32 arg2);


#define SDW_MEMBERS_ZoneList void Load(u32 id);

#define SDW_MEMBERS_InlineEmitter3 \
    InlineEmitter3();              \
    void RenderFlat(Camera *view, s32 forward);
#define SDW_MEMBERS_InlineEmitter4 InlineEmitter4();

#define SDW_MEMBERS_InlineEmitter1 \
    InlineEmitter1();              \
    void RenderFlat(Camera *view, s32 forward);
#define SDW_MEMBERS_Sprite                                                       \
    void DrawAt(u32 *layer, s32 x, s32 y, u32 color, u32 flip);                  \
    void DrawFrameAt(u32 *layer, s32 x, s32 y, u32 color, u32 frame, u32 flip)   \
    {                                                                            \
        DrawFrame(layer, x, y, x + widthMinus1, y + height, color, frame, flip); \
    }
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_ISINWORLD 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_ISINWORLD
#define SDW_INLINE_SCNOBJECT_INSTFLAGS_U16 1
#define SDW_INLINE_SCNOBJECT_SETVISIBLE_S32 1
#define SDW_INLINE_SCNOBJECT_STOPSOUND_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_INSTFLAGS_U16
#undef SDW_INLINE_SCNOBJECT_SETVISIBLE_S32
#undef SDW_INLINE_SCNOBJECT_STOPSOUND_U16
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_GETANIMID 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_GETANIMID
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_SHADOW_SETVISIBLE_S32 1
#include "../engine/shadow_inlines.h"
#undef SDW_INLINE_SHADOW_SETVISIBLE_S32
#define SDW_INLINE_SPRITE_DRAWAT_U32_S32_S32_U32_U32 1
#define SDW_INLINE_SPRITE_DRAWTHUNKAT_U32_S32_S32_U32_U32 1
#include "../engine/sprite_inlines.h"
#undef SDW_INLINE_SPRITE_DRAWAT_U32_S32_S32_U32_U32
#undef SDW_INLINE_SPRITE_DRAWTHUNKAT_U32_S32_S32_U32_U32
inline InlineEmitter3::InlineEmitter3()
{
    base.slotPool = slotBuf;
    base.particles = particleBuf;
    base.count = 3;
    base.Emitter_Reset();
}
inline InlineEmitter4::InlineEmitter4()
{
    base.slotPool = slotBuf;
    base.particles = particleBuf;
    base.count = 4;
    base.Emitter_Reset();
}
inline InlineEmitter1::InlineEmitter1()
{
    base.slotPool = slotBuf;
    base.particles = particleBuf;
    base.count = 1;
    base.Emitter_Reset();
}
inline void InlineEmitter3::RenderFlat(Camera *view, s32 forward)
{
    if (forward)
        base.Emitter_RenderFlat_Fwd(view);
    else
        base.Emitter_RenderFlat(view);
}
#define SDW_INLINE_INLINEEMITTER4_RENDERFLAT_CAMERA_S32 1
#include "../engine/emitter4_inlines.h"
#undef SDW_INLINE_INLINEEMITTER4_RENDERFLAT_CAMERA_S32
inline void InlineEmitter1::RenderFlat(Camera *view, s32 forward)
{
    if (forward)
        base.Emitter_RenderFlat_Fwd(view);
    else
        base.Emitter_RenderFlat(view);
}

/* Data record at 0x576648, 12-byte stride shown by 0x4d670d / 0x4d70d1. */
struct DefuseButtonData {
    u16 mask;
    s16 x;
    s16 y;
    u16 pad;
    u32 color;
};
/* 0x576648 - the defuse panel's buttons: pad mask, offset from the panel centre, colour. const: it is in .rdata */
const DefuseButtonData g_defuseButtonTable[] = {{(u16)~PAD_TRIANGLE, 0, -18, 0, 0x004000},
                                                {(u16)~PAD_CIRCLE, 21, -7, 0, 0x000030},
                                                {(u16)~PAD_SQUARE, -21, -7, 0, 0x400040},
                                                {(u16)~PAD_CROSS, 0, 4, 0, 0xff0000},
                                                {0x0000, 11, -44, 0, 0x1e2ed1},
                                                {0x0000, -11, -44, 0, 0x3db92e}};

/* ======== Mine ======== */
s32 Vec3s_Dist(Vec3s *a, Vec3s *b); /* 0x515813 */
extern u32 g_gameTime;

#define g_camPos (g_camera.pos)

u32 Mine_TriggerScoreCB(ScnObject *self, ScnObject *candidate, u32 distance, ScnObject *selected);

/* 0x4d4ce0 Mine_FindTrigger */
ScnObject *Mine::FindTrigger(u16 radius)
{
    return Scenaric_FindBestInRadius(&pos, pos.y - (radius >> 1), pos.y + (radius >> 1), radius, 0, Mine_TriggerScoreCB,
                                     0);
}

/* 0x4d4d2a Mine_TriggerScoreCB */
u32 Mine_TriggerScoreCB(ScnObject *self, ScnObject *candidate, u32 distance, ScnObject *selected)
{
    u16 p = candidate->GetClassId();
    if (candidate->GetFirstSolidBox() && p != CLASSID_DEFUSABLEMINE && p != CLASSID_GROUNDMINE &&
        p != CLASSID_ICEGROUND && p != CLASSID_ELASTICTREE && p != CLASSID_MISCSTATIC && p != CLASSID_SWIRLSIGN)
        return distance;
    return 0xffffffff;
}

/* 0x4d4d87 Mine_InitModels */
ScnObject *Mine::InitModels(void *record)
{
    u16 p = 3;
    return InitWithAltModels(record, &normalModel, 1, &p, &explodedModel);
}

/* 0x4d4dbe Mine_SetState */
void Mine::SetState(u8 newState)
{
    s16 p; /* lower broadcast bound, -2 */
    s32 q; /* camera distance, -8 */
    s16 r; /* upper broadcast bound, -a */
    state = newState;
    switch (newState) {
        case MINE_ST_SAFE:
            shadow.SetVisible(1);
            break;
        case MINE_ST_REARMING:
            deadline = g_gameTime + 0x5000;
            shadow.SetVisible(1);
            break;
        case MINE_ST_ARMED:
            shadow.SetVisible(1);
            break;
        case MINE_ST_COUNTDOWN:
            deadline = g_gameTime + 0x2aa;
            shadow.SetVisible(1);
            break;
        case MINE_ST_EXPLODING:
            shadow.SetVisible(0);
            SwapModel(&explodedModel);
            mineFlags.exploded = 1;
            PlayAnim(MINE_ANIM_0_IDLE, 0, 0);
            p = pos.y - 300;
            r = pos.y + 300;
            Scenaric_BroadcastInRadius(CLASSID_NONE, p, r, 300, MSG_KILL, 0, 0);
            Scenaric_BroadcastInRadius(CLASSID_ROCKS, p, r, 0x226, MSG_KILL, 0, 0);
            q = Vec3s_Dist(&pos, &g_camPos);
            if (q <= 5000)
                Camera_StartShake((5000 - q) * 60 / 5000, 0x2000);
            break;
    }
}

/* 0x4d505c Mine_HandleMessage */
s32 Mine::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    switch (msgId) {
        case MSG_DETECTOR_PING:
            return 1;
        case MSG_KILL:
            if (!arg && state == MINE_ST_ARMED) {
                SetState(MINE_ST_COUNTDOWN);
                return 1;
            }
            break;
        case MSG_CINE_END:
            if (IsInWorld())
                savedPos = pos;
            return 1;
    }
    return 0;
}

/* 0x4d50f1 Mine_Init */
void Mine::PostLoadInit()
{
    shadow.radius = 20;
    SnapToGround(1);
    savedPos = pos;
    mineFlags.exploded = 0;
    SetState(MINE_ST_ARMED);
}

/* 0x4d5149 Mine_Reset */
void Mine::Reset()
{
    if (mineFlags.exploded) {
        SwapModel(&normalModel);
        mineFlags.exploded = 0;
    }
    if (IsInWorld())
        SetPosition(&savedPos);
    SetState(MINE_ST_ARMED);
}

/* 0x4d51c4 Mine_Update */
void Mine::Update()
{
    ScnObject *p;
    switch (state) {
        case MINE_ST_ARMED:
            p = FindTrigger(200);
            if (p)
                SetState(MINE_ST_COUNTDOWN);
            break;
        case MINE_ST_COUNTDOWN:
            if (g_gameTime >= deadline)
                SetState(MINE_ST_EXPLODING);
            break;
        case MINE_ST_EXPLODING:
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(MINE_ST_EXPLODED);
            break;
    }
    AdvanceAnim();
}

/* ======== GroundMine ======== */
#undef mineFlags

extern u8 g_sharedScratch[]; /* 0x6d5468 */
u16 Sound_Play(u16 id, void *owner, u16 volume, u8 flags, s32 rate);
extern s32 g_dt;
extern u32 g_gameTime;
extern Wolf *g_pWolf;
extern "C" s16 g_sinTable4096[5122];
extern "C" const s16 *g_pCosTable;
/* 0x57b6dc - an EmitterColumnParams block (riseSpeed -200, life 2048, period 2048, sizeStart 16 | sizeEnd 64 << 16,
   sheetIndex 7 + 3 pad). Typed s32[]: a body indexes it as [2] (read at 0x4d63eb). */
EmitterColumnParams g_groundMineFuseFxParams[1] = {{-200, 2048, 2048, 16, 64, 7}};

struct GroundMineMoveArg {
    Vec3s delta;
    u16 flag0 : 1;
    u16 flag1 : 1;
    u16 flag2 : 1;
    Vec3s velocity;
};
/* Byte field at emitter+22, e.g. 4d5d04, is read without zero-extension. */
#define GM_ABS(a) ((a) >= 0 ? (a) : -(a))

#define SDW_INLINE_FREE_PROPU32_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_PROPU32_VOID_U32
#define SDW_INLINE_ZONELIST_LOAD_U32 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_LOAD_U32
#define SDW_INLINE_ZONELIST_CONTAINS_VEC3S 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_CONTAINS_VEC3S
#define SDW_INLINE_FREE_SOUNDISPLAYING_U16 1
#include "../engine/sound_mgr_inlines.h"
#undef SDW_INLINE_FREE_SOUNDISPLAYING_U16
inline void FacingDirection(Vec3s *direction, s16 angle)
{
    s16 masked = angle & 0xfff;
    direction->y = 0;
    direction->x = -g_sinTable4096[masked];
    direction->z = -g_pCosTable[masked];
}

/* 0x4d525d GroundMine_RayBoxEdgeXZ */
s32 GroundMine_RayBoxEdgeXZ(const Vec3s *origin, const Vec3s *direction, const Box *box)
{
    s32 p;
    s32 q;
    s32 r;
    r = q = 0x7fffffff;
    if (origin->x >= box->min[0] && origin->x <= box->max[0] && origin->z >= box->min[2] && origin->z <= box->max[2]
            ? 1
            : 0) {
        if (direction->x) {
            if (direction->x > 0)
                p = box->max[0];
            else
                p = box->min[0];
            r = ((p - origin->x) << 12) / direction->x;
        }
        if (direction->z) {
            if (direction->z > 0)
                p = box->max[2];
            else
                p = box->min[2];
            q = ((p - origin->z) << 12) / direction->z;
        }
    } else {
        if (direction->x) {
            if (direction->x > 0)
                p = box->min[0];
            else
                p = box->max[0];
            r = ((p - origin->x) << 12) / direction->x;
            if (r < 0)
                r = 0x7fffffff;
            else {
                p = origin->z + (r * direction->z >> 12);
                if (p < box->min[2] || p > box->max[2])
                    r = 0x7fffffff;
            }
        }
        if (direction->z) {
            if (direction->z > 0)
                p = box->min[2];
            else
                p = box->max[2];
            q = ((p - origin->z) << 12) / direction->z;
            if (q < 0)
                q = 0x7fffffff;
            else {
                p = origin->x + (q * direction->x >> 12);
                if (p < box->min[0] || p > box->max[0])
                    q = 0x7fffffff;
            }
        }
    }
    if (r < q)
        return r;
    return q;
}

/* 0x4d549f GroundMine_RayToHazard. The final box local may remain uninitialized, as in the original. */
s32 GroundMine::RayToHazard(const Vec3s *origin, const Vec3s *direction, s32 limit)
{
    Box **p;
    s32 q;
    s32 r;
    s32 s;
    Box *t;
    s32 u;
    u32 v;
    Vec3s w;
    r = 0;
    w = *origin;
    u = 6;
    do {
        if (!hazardBoxes.Contains(&w)) {
            s = 0x7fffffff;
            for (v = hazardBoxes.count, p = hazardBoxes.boxes; v; v--, p++) {
                t = *p;
                q = GroundMine_RayBoxEdgeXZ(&w, direction, t);
                if (q < s)
                    s = q;
            }
            if (s == 0x7fffffff)
                r = 0x7fffffff;
            else {
                r += s;
                w.x = origin->x + (r * direction->x >> 12);
                w.z = origin->z + (r * direction->z >> 12);
            }
        }
        if (r != 0x7fffffff) {
            t = safeBoxes.Contains(&w);
            if (t) {
                r += 4 + GroundMine_RayBoxEdgeXZ(&w, direction, t);
                w.x = origin->x + (r * direction->x >> 12);
                w.z = origin->z + (r * direction->z >> 12);
            }
        }
        u--;
    } while (t && r < limit && u > 0);
    return r;
}

/* 0x4d566b GroundMine_Move. Only the first three flag bits are initialized. */
u32 GroundMine::Move(Vec3s *delta, ContactInfo *contact, u16 flags, const Vec3s *velocity)
{
    u32 p;
    GroundMineMoveArg q;
    Vec3s r;
    if (mover) {
        q.delta = *delta;
        q.velocity = *velocity;
        q.flag0 = 0;
        q.flag1 = 0;
        q.flag2 = 0;
        mover->HandleMessage(this, MSG_MODIFY_MOVE, &q);
        *delta = q.delta;
    }
    if (GM_ABS(delta->x) <= 50 && GM_ABS(delta->z) <= 50 && GM_ABS(delta->y) <= 50)
        p = Collide_ResolveMove(delta, contact, 0xb54, flags, 0, 0, 10, 0, 0);
    else {
        r.x = delta->x >> 1;
        r.y = delta->y >> 1;
        r.z = delta->z >> 1;
        delta->x -= r.x;
        delta->y -= r.y;
        delta->z -= r.z;
        p = Collide_ResolveMove(delta, contact, 0xb54, flags, 0, 0, 10, 0, 0);
        delta->x += pos.x;
        delta->y += pos.y;
        delta->z += pos.z;
        p |= Collide_ResolveMove(&r, contact, 0xb54, flags, delta, 0, 10, 0, 0);
        delta->x = delta->x + r.x - pos.x;
        delta->y = delta->y + r.y - pos.y;
        delta->z = delta->z + r.z - pos.z;
    }
    Translate(delta);
    return p;
}

/* 0x4d590e GroundMine_FallStep */
u32 GroundMine::FallStep(u16 flags)
{
    ContactInfo p;
    s32 q;
    Vec3s *r;
    u32 s;
    Vec3s t;
    r = (Vec3s *)g_sharedScratch; /* cast kept: one scratch buffer, laid out by each user */
    r->x = r->z = 0;
    q = fallTime * 2000 >> 12;
    if (q > 1000)
        r->y = 1000;
    else if (q < 120)
        r->y = 120;
    else
        r->y = q;
    Vec3s_ScaleByDt(r, &t);
    s = Move(&t, &p, flags, r);
    if (p.movableObj)
        gmFlags.onMover = 1;
    else
        gmFlags.onMover = 0;
    return s;
}

/* 0x4d59e1 GroundMine_Update */
void GroundMine::Update()
{
    ScnObject *p;
    ScnObject *q;
    u16 r;
    Vec3s s;
    Vec3s t;
    if (!hazardBoxes.count) {
        if (state != MINE_ST_EXPLODING) {
            if (gmFlags.falling) {
                fallTime += g_dt;
                if (fallTime > 0x1e000)
                    fallTime = 0x1e000;
                if (mover)
                    r = RESOLVE_SLIDE_ALL;
                else
                    r = COLL_WALL;
                if ((FallStep(r) & COLL_FLOOR) && !gmFlags.onMover && !mover)
                    gmFlags.falling = 0;
            } else {
                fallTime = 0;
                if (mover)
                    gmFlags.falling = 1;
            }
        }
        if (state == MINE_ST_COUNTDOWN && gmFlags.countingDown) {
            if (!gmFlags.triggered && !FindTrigger(radius)) {
                gmFlags.countingDown = 0;
                SetState(MINE_ST_ARMED);
            } else {
                Mine::Update();
            }
        } else if (state != MINE_ST_ARMED) {
            Mine::Update();
        } else {
            if (gmFlags.triggered || FindTrigger(radius)) {
                if (fuseDelay) {
                    SetState(MINE_ST_COUNTDOWN);
                    deadline = g_gameTime + fuseDelay;
                    fuseFx.base.Emitter_Reset();
                    gmFlags.countingDown = 1;
                } else {
                    SetState(MINE_ST_EXPLODING);
                }
            }
            AdvanceAnim();
        }
        if (state == MINE_ST_ARMED || state == MINE_ST_COUNTDOWN) {
            t = pos;
            t.y -= 25;
            ringFx.base.Emitter_UpdateFade(&ringParams, &t, 0, 1);
            if (!beepSound || !::SoundIsPlaying(beepSound))
                beepSound = Sound_Play(SND_MINE_BEEP, this, 0x7f, SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 0x1000);
        } else {
            if (ringFx.base.flags.active)
                ringFx.base.Emitter_Reset();
            if (beepSound) {
                StopSound(beepSound);
                beepSound = 0;
            }
        }
        s = pos;
        s.y -= 33;
        if (state != MINE_ST_COUNTDOWN)
            gmFlags.countingDown = 0;
        if (gmFlags.countingDown) {
            fuseFx.base.Emitter_UpdateColumn(g_groundMineFuseFxParams, &s, deadline - g_gameTime, 1);
        } else {
            if (fuseFx.base.flags.active)
                fuseFx.base.Emitter_UpdateColumn(g_groundMineFuseFxParams, &s, 0, 0);
        }
        gmFlags.triggered = 0;
    } else if (state == MINE_ST_ARMED) {
        p = 0;
        if (hazardBoxes.Contains(&g_pWolf->pos) && !safeBoxes.Contains(&g_pWolf->pos)) {
            p = g_pWolf;
        } else {
            q = g_pSheepOutOfZone;
            if (q && hazardBoxes.Contains(&q->pos) && !safeBoxes.Contains(&q->pos))
                p = q;
        }
        if (p) {
            SetPosition(&p->pos);
            SetVisible(1);
            SetState(MINE_ST_EXPLODING);
        }
    } else {
        Mine::Update();
    }
}

/* 0x4d5f98 GroundMine_HandleMessage */
s32 GroundMine::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    Vec3s p;
    Vec3s q;
    switch (msgId) {
        case MSG_DETECTOR_PING:
            if (!hazardBoxes.count)
                return 1;
            break;
        case MSG_RIDER_ADD:
            if (!mover)
                mover = sender;
            return 1;
        case MSG_RIDER_REMOVE:
            if (mover == sender)
                mover = 0;
            return 1;
        case MSG_MAGNET_QUERY:
            if (!hazardBoxes.count)
                return 1;
            break;
        case MSG_MAGNET_PULL:
            gmFlags.triggered = 1;
            break;
        case MSG_SEESAW_TOUCH:
        case MSG_LANDED:
            if (!InstFlags(INST_F_ATTACHED))
                gmFlags.falling = 1;
            break;
        case MSG_DETECTOR_RAY:
            q = sender->pos;
            FacingDirection(&p, sender->rot.y);
            return RayToHazard(&q, &p, 0x7fffffff);
        default:
            return Mine::HandleMessage(sender, msgId, arg);
    }
    return 0;
}

/* 0x4d6195 GroundMine_Render */
void GroundMine::Render(Camera *view)
{
    ScnMobile::Render(view);
    if (InstFlags(INST_F_DRAWN)) {
        if (!(!ForceDraw() && camDist2 > 9000000 ? 1 : 0)) {
            if (ringFx.base.flags.active)
                ringFx.RenderFlat(view, 1);
            if (fuseFx.base.flags.active)
                fuseFx.base.Emitter_Render(view, 0);
        }
    }
}

/* 0x4d6266 GroundMine_Reset. The live beep handle is intentionally cleared without stopping it. */
void GroundMine::Reset()
{
    gmFlags.onMover = 0;
    gmFlags.falling = 0;
    gmFlags.triggered = 0;
    fallTime = 0;
    beepSound = 0;
    Mine::Reset();
    if (hazardBoxes.count)
        SetVisible(0);
}

/* 0x4d6319 GroundMine_Init */
void GroundMine::PostLoadInit()
{
    void *p = record;
    hazardBoxes.Load(PropU32(p, 0));
    safeBoxes.Load(PropU32(p, 0xc));
    radius = PropU32(p, 4);
    gmFlags.countingDown = 0;
    fuseDelay = PropU32(p, 8) * g_groundMineFuseFxParams[0].period;
    if (fuseDelay > 0)
        fuseDelay--;
    fuseFx.base.Emitter_Reset();
    ringFx.base.Emitter_Reset();
    ringParams.sheetIndex = 5;
    ringParams.life = 0x4000;
    ringParams.fadeStart = 0;
    ringParams.spawnInterval = 0x1000;
    ringParams.sizeStart = 0;
    ringParams.sizeEnd = radius << 1;
    if (hazardBoxes.count) {
        SetUpdateMode(SCN_UPD_ALWAYS);
        SetVisible(0);
        Scenaric_SendToClass(CLASSID_MINEDETECTOR, MSG_MINEDETECTOR_REGISTER, 0);
    }
    gmFlags.onMover = 0;
    gmFlags.falling = 0;
    gmFlags.triggered = 0;
    mover = 0;
    fallTime = 0;
    beepSound = 0;
    Mine::PostLoadInit();
}

/* 0x4d6610 GroundMine_Create. Generated emitter members initialize in construction. */
ScnObject *GroundMine_Create(void *record)
{
    GroundMine *obj = new GroundMine;
    obj = (GroundMine *)obj->InitModels(record); /* cast kept: InitModels returns the object as a ScnObject * */
    return obj;
}

/* ======== DefusableMine ======== */

/* Byte instructions at 4d6c7f and 4d7133 identify the +a7 (Mine) and +f8 (DefusableMine) bitfields. */
/* (struct DefuseButtonData and g_defuseButtonTable 0x576648 are defined at the top of the file) */
/* .bss 0x6cf688-0x6cf708. VC6 orders uninitialised globals by a hash of their names, and globals explicitly initialised
   to zero after them in definition order (src/README.md); `= {0}` keeps the exe's
   order without renaming them. */
Sprite g_defuseGlyphSprite = {0};                                                /* 0x6cf688 */
Sprite g_defuseBeatLightSprite = {0};                                            /* 0x6cf698 */
Sprite g_defusePanelSprite = {0};                                                /* 0x6cf6a8 */
Sprite g_defusePanelTopSprite = {0};                                             /* 0x6cf6b8 */
Sprite g_defusePadSprites[4] = {0};                                              /* 0x6cf6c8 */
EmitterFadeParams g_defusableMineRingFxParams = {7168, 6144, 12288, 50, 320, 5}; /* 0x57b6f0 */
extern u32 *g_screenLayerBase;
extern u32 g_gameTime;

#define g_padCurButtons (g_pad.cur.buttons)

#define g_padPrevButtons (g_pad.prev.buttons)

extern Wolf *g_pWolf;
u16 Sound_Play(u16 id, void *owner, u16 volume, u8 flags, s32 rate);
inline s32 PropS32(void *record, u32 off)
{
    /* cast kept: a designer property is a 4-byte slot at a byte offset of the raw WAR record */
    return *(s32 *)((u8 *)record + off + 0x14);
}

/* 0x4d66ec DefusableMine_DrawGlyph */
void DefusableMine::DrawGlyph(s32 glyph)
{
    s32 p =
        g_defusePanelSprite.widthMinus1 + 16 - (g_defuseGlyphSprite.widthMinus1 >> 1) + g_defuseButtonTable[glyph].x;
    s32 q = g_defusePanelSprite.height + 32 - (g_defuseGlyphSprite.height >> 1) + g_defuseButtonTable[glyph].y;
    g_defuseGlyphSprite.DrawFrameAt(g_screenLayerBase + 7, p, q, g_defuseButtonTable[glyph].color, 1, 0);
}

/* 0x4d679b DefusableMine_DrawPanel. Lamp Y intentionally uses the glyph width. */
void DefusableMine::DrawPanel()
{
    s32 p = g_defusePanelSprite.widthMinus1 + 16;
    s32 q;
    s32 r = g_defusePanelSprite.height + 32;
    g_defuseGlyphSprite.DrawAt(g_screenLayerBase + 8, p + 11 - (g_defuseGlyphSprite.widthMinus1 >> 1),
                               r - 44 - (g_defuseGlyphSprite.widthMinus1 >> 1), 0xf1769, 0);
    g_defuseGlyphSprite.DrawAt(g_screenLayerBase + 8, p - 11 - (g_defuseGlyphSprite.widthMinus1 >> 1),
                               r - 44 - (g_defuseGlyphSprite.widthMinus1 >> 1), 0x1e5c17, 0);
    for (q = 0; q < 4; q++) {
        g_defusePadSprites[q].DrawAt(g_screenLayerBase + 8,
                                     p + g_defuseButtonTable[q].x - (g_defusePadSprites[q].widthMinus1 >> 1),
                                     r + g_defuseButtonTable[q].y - (g_defusePadSprites[q].height >> 1), 0x808080, 0);
    }
    g_defusePanelSprite.DrawThunkAt(g_screenLayerBase + 9, 16, 32, 0x808080, 0);
    g_defusePanelSprite.DrawThunkAt(g_screenLayerBase + 9, p, 32, 0x808080, 4);
    g_defusePanelSprite.DrawThunkAt(g_screenLayerBase + 9, p, r, 0x808080, 0xc);
    g_defusePanelSprite.DrawThunkAt(g_screenLayerBase + 9, 16, r, 0x808080, 8);
    r = 32 - g_defusePanelTopSprite.height;
    g_defusePanelTopSprite.DrawThunk(g_screenLayerBase + 9, p - g_defusePanelTopSprite.widthMinus1, r, p, 34, 0x808080,
                                     0);
    g_defusePanelTopSprite.DrawThunk(g_screenLayerBase + 9, p, r, p + g_defusePanelTopSprite.widthMinus1, 34, 0x808080,
                                     4);
}

/* 0x4d6ac1 DefusableMine_DrawBeatLights */
void DefusableMine::DrawBeatLights()
{
    s32 p = g_defusePanelSprite.widthMinus1 - 7 - g_defuseBeatLightSprite.widthMinus1 / 2;
    s32 q;
    s32 r = g_defusePanelSprite.height + 55 - g_defuseBeatLightSprite.height / 2;
    s32 s;
    u8 t;
    t = 0;
    s = g_gameTime < stepStart + 0x1400;
    for (q = 0; q < 4; q++) {
        if (s && (q << 12) / 3 + stepStart < g_gameTime) {
            g_defuseBeatLightSprite.DrawFrameAt(g_screenLayerBase + 8, p + q * 46 / 3, r, 0xffff, 0, 0);
            t++;
        } else {
            g_defuseBeatLightSprite.DrawFrameAt(g_screenLayerBase + 8, p + q * 46 / 3, r, 0x604040, 0, 0);
        }
    }
    if (s && t != lightsShown && t != 4) {
        Sound_Play(SND_MINE_BEAT_LIGHT, this, 0xff, SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 0x1000);
        lightsShown = t;
    }
}

/* 0x4d6c73 DefusableMine_Update */
void DefusableMine::Update()
{
    u32 p;
    u32 q;
    Vec3s r;
    s32 s;
    u32 t;
    ScnObject *u;
    if (!mineFlags.exploded && !InstFlags(INST_F_ATTACHED) && AnimFlags(ANIM_F_FINISHED)) {
        if (!GetAnimId())
            PlayAnim(AMINED01_ANIM_GREEN, 0, 0);
        else
            PlayAnim(AMINED01_ANIM_RED, 0, 0);
    }
    p = (g_gameTime + phaseOffset) % 0x3000;
    if (p >= 0x1800) {
        if (dormant) {
            dormant = 0;
            Sound_Play(SND_MINE_BEAT, this, 0x7f, SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 0x1000);
        }
    } else {
        dormant = 1;
    }
    r = pos;
    r.y -= 25;
    ringFx.base.Emitter_UpdateFade(&g_defusableMineRingFxParams, &r, 0, !dormant && state == MINE_ST_ARMED ? 1 : 0);
    switch (state) {
        case MINE_ST_ARMED:
            if (!dormant) {
                u = FindTrigger((p - 0x1800) * 200 / 0x1800);
                if (u)
                    SetState(MINE_ST_COUNTDOWN);
            }
            break;
        case DM_ST_DEFUSE_BEGIN:
            SetState(DM_ST_PLAYBACK);
            break;
        case DM_ST_PLAYBACK:
            if (g_gameTime % 0x400 > 0x200)
                DrawGlyph(5);
            t = (code >> (seqIndex << 1)) & 3;
            q = stepStart + 0x1000;
            DrawBeatLights();
            if (g_gameTime >= q) {
                if (g_gameTime < q + 0x800) {
                    if (!dmFlags.played) {
                        Sound_Play(SND_MINE_DEFUSE_STEP, this, 0xff, SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 0x1000);
                        dmFlags.played = 1;
                    }
                    DrawGlyph(t);
                } else if (seqIndex == codeLength - 1 && g_gameTime < q + 0x2800) {
                    if (!finalSound)
                        finalSound = Sound_Play(SND_MINE_DEFUSE_FINAL, this, 0xff,
                                                SNDF_LOOP | SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 0x1000);
                    if (g_gameTime % 0x400 < 0x200)
                        DrawGlyph(4);
                } else {
                    seqIndex++;
                    stepStart = g_gameTime;
                    lightsShown = 0xff;
                    dmFlags.played = 0;
                    if (seqIndex >= codeLength)
                        SetState(DM_ST_INPUT);
                }
            }
            DrawPanel();
            break;
        case DM_ST_INPUT:
            if (g_gameTime % 0x400 < 0x200)
                DrawGlyph(4);
            t = (code >> (seqIndex << 1)) & 3;
            q = stepStart + 0x1000;
            for (s = 0; s < 4; s++) {
                if (!(g_padCurButtons & ~g_defuseButtonTable[s].mask) &&
                    (g_padPrevButtons & ~g_defuseButtonTable[s].mask)) {
                    if (g_gameTime >= q - 0x400 && g_gameTime <= q + 0x400 && s == t &&
                        !(dmFlags.failed | dmFlags.hit)) {
                        dmFlags.hit = 1;
                        Sound_Play(SND_MINE_DEFUSE_STEP, this, 0xff, SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 0x1000);
                    } else {
                        dmFlags.failed = 1;
                        dmFlags.hit = 0;
                        Sound_Play(SND_MINE_DEFUSE_FAIL, this, 0xff, SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 0x1000);
                    }
                }
            }
            if (dmFlags.hit)
                DrawGlyph(t);
            DrawBeatLights();
            if (g_gameTime >= q + 0x800) {
                if (!dmFlags.hit || dmFlags.failed) {
                    SetState(MINE_ST_EXPLODING);
                } else {
                    seqIndex++;
                    stepStart = g_gameTime;
                    dmFlags.hit = 0;
                    lightsShown = 0xff;
                    if (seqIndex >= codeLength)
                        SetState(MINE_ST_SAFE);
                }
            }
            DrawPanel();
            break;
        case MINE_ST_EXPLODED:
            if (g_gameTime >= deadline) {
                Reset();
                SetState(MINE_ST_REARMING);
            }
            break;
        case MINE_ST_REARMING:
            if (dormant == 1 && g_gameTime >= deadline)
                SetState(MINE_ST_ARMED);
            break;
        default:
            Mine::Update();
            return;
    }
    AdvanceAnim();
}

/* 0x4d7347 DefusableMine_HandleMessage */
s32 DefusableMine::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    s32 p;
    ScnObject *q;
    u8 r;
    DropMsgArg *s;
    p = Mine::HandleMessage(sender, msgId, arg);
    if (p)
        return p;
    switch (msgId) {
        case MSG_FREEZE:
            if (sender->GetClassId() == CLASSID_WOLF) {
                if (state == DM_ST_PLAYBACK || state == DM_ST_INPUT)
                    SetState(MINE_ST_ARMED);
                dmFlags.wolfFrozen = 0;
                return 1;
            }
            break;
        case MSG_LAUNCH:
            SetState(MINE_ST_EXPLODING);
            return 1;
        case MSG_QUERY_ACTION:
            switch (state) {
                case MINE_ST_ARMED:
                    switch (sender->GetClassId()) {
                        case CLASSID_WOLF:
                            if (dormant == 1)
                                return CTX_DEFUSE;
                    }
                    break;
                case MINE_ST_SAFE:
                    if (sender->GetClassId() == CLASSID_WOLF || sender->GetClassId() == CLASSID_ROBOT)
                        return CTX_PICKUP;
                    break;
            }
            break;
        case MSG_QUERY_HELD_ACTION:
            return HELD_THROWABLE;
        case MSG_USE:
            SetState(DM_ST_DEFUSE_BEGIN);
            return 1;
        case MSG_PICKUP:
            q = sender;
            r = (u8)(u32)arg; /* cast kept: the message arg is a void *; what it carries depends on the message */
            AttachTo(q, r, 0, 0, 0, 0);
            ringFx.base.Emitter_Reset();
            SetState(MINE_ST_SAFE);
            PlayAnim(AMINED01_ANIM_LINK, 0, 0);
            shadow.SetVisible(0);
            return 1;
        case MSG_DROP:
            s = (DropMsgArg *)arg; /* cast kept: the message arg is a void *; what it carries depends on the message */
            Detach();
            SetPosition(&s->pos);
            shadow.SetVisible(1);
            if (s->placed) {
                ringFx.base.Emitter_Reset();
                SetState(MINE_ST_REARMING);
            }
            return 1;
        case MSG_CHECKPOINT_ROLLBACK:
            Reset();
            return 1;
    }
    return 0;
}

/* 0x4d7699 DefusableMine_SetState */
void DefusableMine::SetState(u8 newState)
{
    s32 p;
    if (finalSound) {
        StopSound(finalSound);
        finalSound = 0;
    }
    p = 0;
    switch (newState) {
        case MINE_ST_SAFE:
            PlayAnim(AMINED01_ANIM_GREEN, 1, 0);
            break;
        case MINE_ST_ARMED:
            PlayAnim(AMINED01_ANIM_RED, 1, 0);
            break;
        case MINE_ST_COUNTDOWN:
            PlayAnim(AMINED01_ANIM_GREEN, 0, 0);
            break;
        case DM_ST_PLAYBACK:
        case DM_ST_INPUT:
            p = 1;
            seqIndex = 0;
            stepStart = g_gameTime;
            dmFlags.hit = 0;
            dmFlags.failed = 0;
            lightsShown = 0xff;
            dmFlags.played = 0;
            PlayAnim(AMINED01_ANIM_RED, 0, 0);
            shadow.SetVisible(1);
            break;
        case MINE_ST_EXPLODED:
            shadow.SetVisible(0);
            deadline = g_gameTime + 0x14000;
            break;
        default:
            PlayAnim(AMINED01_ANIM_RED, 0, 0);
            break;
    }
    if (p) {
        if (!dmFlags.wolfFrozen && g_pWolf->HandleMessage(this, MSG_FREEZE, 0))
            dmFlags.wolfFrozen = 1;
    } else if (dmFlags.wolfFrozen) {
        g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0);
        dmFlags.wolfFrozen = 0;
    }
    Mine::SetState(newState);
}

/* 0x4d7a14 DefusableMine_Render */
void DefusableMine::Render(Camera *view)
{
    ScnMobile::Render(view);
    if (InstFlags(INST_F_DRAWN)) {
        if (!(!ForceDraw() && camDist2 > 9000000 ? 1 : 0)) {
            if (ringFx.base.flags.active)
                ringFx.RenderFlat(view, 1);
        }
    }
}

/* 0x4d7aba DefusableMine_Reset. Clears the live sound handle before SetState can stop it. */
void DefusableMine::Reset()
{
    finalSound = 0;
    dormant = 1;
    Mine::Reset();
    SetState(MINE_ST_REARMING);
    deadline = g_gameTime;
}

/* 0x4d7aff DefusableMine_Init */
void DefusableMine::PostLoadInit()
{
    void *p = record;
    finalSound = 0;
    dormant = 1;
    phaseOffset = (PropS32(p, 4) << 12) / 100;
    phaseOffset %= 0x3000;
    phaseOffset = 0x3000 - phaseOffset;
    ringFx.base.Emitter_Reset();
    codeLength = PropS32(p, 8);
    code = PropS32(p, 0);
    dmFlags.wolfFrozen = 0;
    g_defusePanelSprite.LoadFromRes(DAV_IDI_IMNINT1_);
    g_defusePanelTopSprite.LoadFromRes(DAV_IDI_IMNINT3_);
    g_defuseGlyphSprite.LoadFromRes(DAV_IDI_IMNINT2_);
    g_defuseBeatLightSprite.LoadFromRes(DAV_IDI_IMNCHEK_);
    g_defusePadSprites[0].LoadFromRes(DAV_IDI_IMNPADT_);
    g_defusePadSprites[1].LoadFromRes(DAV_IDI_IMNPADC_);
    g_defusePadSprites[2].LoadFromRes(DAV_IDI_IMNPADS_);
    g_defusePadSprites[3].LoadFromRes(DAV_IDI_IMNPADX_);
    Mine::PostLoadInit();
}

/* 0x4d7c50 DefusableMine_Create. Generated emitter member initializes in construction. */
ScnObject *DefusableMine_Create(void *record)
{
    DefusableMine *obj = new DefusableMine;
    obj = (DefusableMine *)obj->InitModels(record); /* cast kept: InitModels returns the object as a ScnObject * */
    return obj;
}
