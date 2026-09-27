/*
 * T185 - original object MineDetector.cpp (guessed name), one translation unit.
 *   .text  0x4d7d00-0x4d8a9d (MineDetector_ConeScoreCB .. MineDetector_Create, in address order)
 *   .rdata 0x576708-0x57672c (??_7MineDetector)
 *   .bss   0x6cf708-0x6cf728 (g_mineDetectorGaugeSprite, g_mineDetectorIconSprite)
 * The functions are in address order; the two sprites are defined here.
 */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *inst, Animator *animator, u16 id, u32 opts);
#include "../engine/sound_mgr.h"
#include "../engine/interface.h"

#define SDW_MEMBERS_ScnObject                                                                           \
    static void *operator new(u32 size);                                                                \
    void AttachTo(ScnObject *parent, u8 joint, Vec3s *offset, Vec3s *rot, u32 arg, u32 arg2);           \
    ScnObject *Scenaric_FindBestInRadius(Vec3s *center, s16 bottom, s16 top, u16 radius, u16 *distance, \
                                         u32 (*score)(ScnObject *, ScnObject *, u32, ScnObject *), s32 hidden);

#define SDW_MEMBERS_ScnBody                  \
    s32 AnimFinished()                       \
    {                                        \
        return anim.flags & ANIM_F_FINISHED; \
    }


#define SDW_MEMBERS_Sprite void DrawAt(u32 *layer, s32 x, s32 y, u32 color, u32 flip);
#define SDW_MEMBERS_AnimSprite                                            \
    void DrawAt(u32 *layer, s32 x, s32 y, u32 color, u32 frame, u32 flip) \
    {                                                                     \
        Draw(layer, x, y, x + width, y + height, color, frame, flip);     \
    }
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_INSTFLAGS_U16 1
#define SDW_INLINE_SCNOBJECT_ISINWORLD 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_INSTFLAGS_U16
#undef SDW_INLINE_SCNOBJECT_ISINWORLD
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_STOPSOUND_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_STOPSOUND_U16
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_SHADOW_SETVISIBLE_S32 1
#include "../engine/shadow_inlines.h"
#undef SDW_INLINE_SHADOW_SETVISIBLE_S32
#define SDW_INLINE_SPRITE_DRAWAT_U32_S32_S32_U32_U32 1
#include "../engine/sprite_inlines.h"
#undef SDW_INLINE_SPRITE_DRAWAT_U32_S32_S32_U32_U32

#define DET_ABS(a) ((a) >= 0 ? (a) : -(a))
#define DET_ANGLE_DELTA(a, b) ((s16)((s16)(((a) - (b) + 0x800) & 0xfff) - 0x800))

#include "../sdk/crt.h"
extern "C" s16 Math_RadiansToAngle4096(float radians); /* 0x5269ce */
u16 Sound_Play(u16 id, void *owner, u16 volume, u8 flags, s32 rate);
/* .bss 0x6cf708-0x6cf728; uninitialised globals come out in the order of VC6's name hash, which is the exe's here */
Sprite g_mineDetectorGaugeSprite; /* 0x6cf708 */
Sprite g_mineDetectorIconSprite;  /* 0x6cf718 */
extern u32 *g_screenLayerBase;

#define SDW_INLINE_SCNOBJECT_GETPARENT 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETPARENT

/* 0x4d7d00 MineDetector_ConeScoreCB */
u32 MineDetector_ConeScoreCB(ScnObject *self, ScnObject *candidate, u32 distance, ScnObject *selected)
{
    s16 p;
    s16 q;
    Vec3s r;
    r.x = candidate->pos.x - self->pos.x;
    r.y = candidate->pos.y - self->pos.y;
    r.z = candidate->pos.z - self->pos.z;
    q = (Math_RadiansToAngle4096((float)atan2((double)r.x, (double)r.z)) + 0x800) & 0xfff;
    p = self->rot.y;
    if (DET_ABS(DET_ANGLE_DELTA(p, q)) <= 0x2aa && candidate->HandleMessage(self, MSG_DETECTOR_PING, 0))
        return distance;
    return 0xffffffff;
}

/* 0x4d7e34 MineDetector_Update */
void MineDetector::Update()
{
    Vec3s p;
    u16 q;
    s32 r;
    u16 s;
    s32 t;
    s32 u;
    ScnObject *v;
    p.x = 0;
    p.y = 0;
    p.z = 0;
    switch (state) {
        case MD_ST_FALLING:
            p.y = 5;
            q = Collide_ResolveMove(&p, &contact, 0xb54, RESOLVE_SLIDE_ALL, 0, &collBox, 0, 0, 0);
            Translate(&p);
            if (q && !contact.floorObj && !contact.movableObj)
                SetState(MD_ST_IDLE);
            break;
        case MD_ST_RAISING:
            if (AnimFlags(ANIM_F_FINISHED)) {
                SetState(MD_ST_SCANNING);
                PlayAnim(ADETEC01_ANIM_DETEC1, 1, 1);
                scanSound =
                    Sound_Play(SND_DETECTOR_SCAN, this, 0xff, SNDF_LOOP | SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 0x1000);
                scanning = 1;
                reading = 0x7fffffff;
            }
            break;
        case MD_ST_SCANNING:
            rot = GetParent()->rot;
            v = Scenaric_FindBestInRadius(&pos, pos.y - 100, pos.y + 100, 200, &s, MineDetector_ConeScoreCB, 1);
            if (v) {
                t = s;
                if (t < 0)
                    t = 0;
            } else {
                t = 200;
            }
            for (r = 0; r < regionCount; r++) {
                u = regions[r]->HandleMessage(this, MSG_DETECTOR_RAY, 0);
                if (u < t) {
                    v = regions[r];
                    t = u;
                }
            }
            if (v)
                reading = t;
            else
                reading = 0x7fffffff;
            break;
        case MD_ST_LOWERING:
            if (AnimFlags(ANIM_F_FINISHED)) {
                SetState(MD_ST_IDLE);
                PlayAnim(ADETEC01_ANIM_LINK, 0, 1);
            }
            break;
    }
    AdvanceAnim();
}

/* 0x4d813f MineDetector_HandleMessage */
s32 MineDetector::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    ScnObject *p;
    u8 q;
    Vec3s *r;
    switch (msgId) {
        case MSG_QUERY_ACTION:
            if (sender->GetClassId() == CLASSID_WOLF)
                return CTX_PICKUP;
            break;
        case MSG_QUERY_HELD_ACTION:
            return HELD_MINEDETECTOR;
        case MSG_HELD_STATE_BEGIN:
            SetState(MD_ST_RAISING);
            PlayAnim(ADETEC01_ANIM_DETEC3, 0, 1);
            return 1;
        case MSG_HELD_STATE_END:
            SetState(MD_ST_LOWERING);
            PlayAnim(ADETEC01_ANIM_DETEC4, 0, 1);
            scanning = 0;
            return 1;
        case MSG_PICKUP:
            p = sender;
            q = (u8)(u32)arg; /* cast kept: arg carries the joint number */
            AttachTo(p, q, 0, 0, 0, 0);
            PlayAnim(ADETEC01_ANIM_LINK, 0, 0);
            SetState(MD_ST_IDLE);
            shadow.SetVisible(0);
            return 1;
        case MSG_DROP:
            r = (Vec3s *)arg; /* cast kept: MSG_DROP's arg is the drop position */
            Detach();
            SetPosition(r);
            PlayAnim(ADETEC01_ANIM_OBJET, 0, 0);
            SetState(MD_ST_IDLE);
            shadow.SetVisible(1);
            return 1;
        case MSG_CHECKPOINT_ROLLBACK:
            Reset();
            return 1;
        case MSG_CARRY_ANIM:
            if (scanning) {
                switch ((u32)arg) { /* cast kept: arg carries a number */
                    case WOLF_CUE_IDLE:
                        PlayAnim(ADETEC01_ANIM_DETEC1, 1, 1);
                        break;
                    case WOLF_CUE_WALK:
                        PlayAnim(ADETEC01_ANIM_DETEC2, 1, 1);
                        break;
                }
            }
            return 1;
        case MSG_CONTAINER_STATE:
            switch ((u32)arg) { /* cast kept: arg carries a number */
                case CONTAINER_RELEASED:
                    homePos = pos;
                    SetState(MD_ST_FALLING);
                    break;
            }
            return 1;
        case MSG_MINEDETECTOR_REGISTER:
            if (regionCount < 4) {
                regions[regionCount] = sender;
                regionCount++;
            }
            return 1;
    }
    return 0;
}

/* 0x4d8585 MineDetector_DrawGauge */
void MineDetector::DrawGauge()
{
    u32 p;
    s32 q;
    s32 r;
    s32 s;
    u32 t;
    s32 u;
    u32 v;
    s32 w;
    s32 x;
    v = 0x8000;
    w = 140;
    q = 40;
    r = 55;
    u = w - 11;
    g_mineDetectorGaugeSprite.DrawThunk(g_screenLayerBase + 8, q - 1 - g_mineDetectorGaugeSprite.widthMinus1 / 2, r - 6,
                                        q + g_mineDetectorGaugeSprite.widthMinus1 / 2 - 1, r + w - 6, 0x808080, 0);
    if (reading <= 200) {
        s = 200 - reading;
        x = s * u / 200;
        p = 0xff00;
        t = s * 255 / 200;
        t = t | ((255 - t) << 8);
        Ui_DrawGouraudRect(g_screenLayerBase + 9, q - 4, r + u - x, q + 4, r + u, t, t, p, p);
        v = 0x80;
    }
    g_mineDetectorIconSprite.DrawAt(g_screenLayerBase + 8, 40 - (g_mineDetectorIconSprite.widthMinus1 >> 1),
                                    210 - (g_mineDetectorIconSprite.height >> 1), 0x808080, 0);
    g_animSpriteCrayon1.DrawAt(g_screenLayerBase + 9, 40 - (g_animSpriteCrayon1.width >> 1),
                               210 - (g_animSpriteCrayon1.height >> 1), v, 0, 0);
}

/* 0x4d878b MineDetector_Render */
void MineDetector::Render(Camera *view)
{
    ScnMobile::Render(view);
    if (state == MD_ST_SCANNING)
        DrawGauge();
}

/* 0x4d87bb MineDetector_InitCollBox. The flags dword is intentionally untouched. */
void MineDetector::InitCollBox()
{
    collBox.min.x = -10;
    collBox.min.y = -20;
    collBox.min.z = -10;
    collBox.max.x = 10;
    collBox.max.y = 0;
    collBox.max.z = 10;
}

/* 0x4d880e MineDetector_SetState */
void MineDetector::SetState(u8 newState)
{
    state = newState;
    if (scanSound) {
        StopSound(scanSound);
        scanSound = 0;
    }
}

/* 0x4d8862 MineDetector_Reset */
void MineDetector::Reset()
{
    SetState(MD_ST_IDLE);
    PlayAnim(ADETEC01_ANIM_OBJET, 0, 0);
    scanning = 0;
    reading = 0x7fffffff;
    shadow.SetVisible(1);
    if (IsInWorld())
        SetPosition(&homePos);
}

/* 0x4d893e MineDetector_Init */
void MineDetector::PostLoadInit()
{
    scanSound = 0;
    if (IsInWorld())
        SnapToGround(1);
    homePos = pos;
    reading = 0x7fffffff;
    SetState(MD_ST_IDLE);
    PlayAnim(ADETEC01_ANIM_OBJET, 0, 0);
    scanning = 0;
    g_mineDetectorGaugeSprite.LoadFromRes(DAV_IDI_IDTJAUG_);
    g_mineDetectorIconSprite.LoadFromRes(DAV_IDI_IDTICONC);
    InitCollBox();
}

/* 0x4d8a23 MineDetector_Create */
ScnObject *MineDetector_Create(void *record)
{
    MineDetector *obj = new MineDetector;
    obj = (MineDetector *)obj->Init(record, 0); /* cast kept: Init returns the object as a ScnBody * */
    obj->regionCount = 0;
    return obj;
}
