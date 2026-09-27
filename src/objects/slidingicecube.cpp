/* PAL PC SlidingIceCube.
 * match-init: SlidingIceCube_StaticInit
 */

#define SDW_MEMBERS_ScnObject                                                                 \
    static void *operator new(u32 size);                                                      \
    void AttachTo(ScnObject *parent, u8 joint, Vec3s *offset, Vec3s *rotation, u32 a, u32 b); \
    void SetFacing(s16 angle);                                                                \
    void SetPos(const Vec3s *at);                                                             \
    void SetUpdateMode(u8 mode);


#define SDW_MEMBERS_ZoneList void Load(u32 id);

#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *inst, Animator *animator, u16 id, u32 opts);
#include "../engine/sound_mgr.h"
#include "../engine/scenaric.h"
#include "../engine/id_list.h"
#include "../engine/scn_tools.h"
#include "../engine/approach.h"
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_ISCOLLIDABLE 1
#define SDW_INLINE_SCNOBJECT_SETCOLLIDABLE_S32 1
#define SDW_INLINE_SCNOBJECT_SETFACING_S16 1
#define SDW_INLINE_SCNOBJECT_SETPOS_CONST_VEC3S 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_ISCOLLIDABLE
#undef SDW_INLINE_SCNOBJECT_SETCOLLIDABLE_S32
#undef SDW_INLINE_SCNOBJECT_SETFACING_S16
#undef SDW_INLINE_SCNOBJECT_SETPOS_CONST_VEC3S
#define SDW_INLINE_SCNOBJECT_SETNOCULL_S32 1
#define SDW_INLINE_SCNOBJECT_STOPSOUND_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETNOCULL_S32
#undef SDW_INLINE_SCNOBJECT_STOPSOUND_U16
#define SDW_INLINE_SCNOBJECT_SOUNDISPLAYING_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SOUNDISPLAYING_U16
#define SDW_INLINE_SCNBODY_ANIMID 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMID
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_ZONELIST_CLEAR 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_CLEAR

extern s32 g_dt;
extern "C" s16 g_sinTable4096[5122];
extern "C" const s16 *g_pCosTable;
s32 ObjGrid_QueryBoxOverlap(CollBox *box, ScnObject **out);
u16 Sound_Play(u16 id, void *owner, u16 volume, u8 flags, s32 rate);
s32 Scenaric_FindByClass(u16 id, ScnObject **out, s32 max);

/* Local message 0x2e payload: position followed by the radius word. */
struct IceCubeRiderMessage {
    Vec3s pos;
    u16 radius;
};

/* The global ScnBody constructor supplies 0x4f2920 and 0x4f292a. */
u16 g_iceCubeSplashRecord[12];
ScnBody g_iceCubeSplashBody;

#define SDW_INLINE_FREE_SCN_GETPROPU32_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_SCN_GETPROPU32_VOID_U32

#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8

#define SDW_INLINE_ZONELIST_LOAD_U32 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_LOAD_U32

#define SDW_INLINE_ZONELIST_FINDCONTAINING_VEC3S 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_FINDCONTAINING_VEC3S

#define SDW_INLINE_FREE_ZONES_GET_U8 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_FREE_ZONES_GET_U8

#define SDW_INLINE_FREE_SCENARIC_CLASSFLAGS_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_FREE_SCENARIC_CLASSFLAGS_U16

/* 0x4f2943 */
void SlidingIceCube::ShowSplash(Box *water)
{
    Vec3s point;
    if (splashReady) {
        point = pos;
        point.y = water->min[1];
        g_iceCubeSplashBody.SetPos(&point);
        g_iceCubeSplashBody.PlayAnim(APLOUF01_ANIM_SPLASH, 0, 0);
        splashVisible = 1;
    }
}

/* 0x4f29d7 */
void SlidingIceCube::HideSplash()
{
    splashVisible = 0;
}

/* 0x4f29ec */
void SlidingIceCube::ApplyGravity(Vec3s *velocity)
{
    if (fallTime > 0xf000)
        fallTime = 0xf000;
    velocity->y = (fallTime * 4000) >> 12;
    if (velocity->y < 200)
        velocity->y = 200;
    else if (velocity->y > 1500)
        velocity->y = 1500;
}

/* 0x4f2a55 */
s32 SlidingIceCube::QueryRiders(ScnObject **out)
{
    CollBox box;
    box.Box_Translate(GetFirstSolidBox(), &pos);
    box.max.y = box.min.y;
    box.min.y -= 10;
    return ObjGrid_QueryBoxOverlap(&box, out);
}

/* 0x4f2a9f. The rider pass also carries the cube when it appears in the
 * query; the original has no separate Translate call for this. */
u16 SlidingIceCube::SlideMove(Vec3s *delta, u16 resolveFlags)
{
    s32 size;
    IceCubeRiderMessage args;
    ScnObject *list[64];
    u16 classId;
    ScnObject *current;
    u8 saved[64];
    size = QueryRiders(list);
    for (s32 index = 0; index < size; index++) {
        current = list[index];
        saved[index] = current->IsCollidable();
        if (saved[index] && this != current)
            current->SetCollidable(0);
    }
    classId = Collide_ResolveMove(delta, 0, 0xb54, resolveFlags, 0, 0, 4, 0, 0);
    for (index = 0; index < size; index++) {
        current = list[index];
        if (saved[index])
            current->SetCollidable(1);
    }
    if (classId & COLL_FLOOR)
        fallTime = 0;
    args.pos.x = pos.x + delta->x;
    args.pos.y = pos.y + delta->y;
    args.pos.z = pos.z + delta->z;
    args.radius = 20;
    if (pushBoxes.count && !pushBoxes.FindContaining(&args.pos)) {
        delta->x = 0;
        delta->y = 0;
        delta->z = 0;
        args.pos = pos;
        slideSpeed = 0;
    }
    for (index = 0; index < size; index++) {
        current = list[index];
        if (current->GetClassId() != CLASSID_FALLINGGATE)
            current->Translate(delta);
        if (Scenaric_ClassFlags(current->GetClassId()) & SCN_CF_SHEEP_ANCHORABLE)
            current->HandleMessage(this, MSG_SET_ANCHOR, &args);
    }
    return classId;
}

/* 0x4f2dae */
u16 SlidingIceCube::CrushMove(Vec3s *delta, u16 resolveFlags)
{
    ContactInfo contact;
    u16 res;
    ScnObject *target;
    res = Collide_ResolveMove(delta, &contact, 0xb54, resolveFlags, 0, 0, 4, 0, 0);
    if (contact.floorObj)
        target = contact.floorObj;
    else if (contact.wallObj)
        target = contact.wallObj;
    else
        target = 0;
    if (target) {
        /* cast kept: MSG_KILL's arg is the WolfKillType, a number in the void * */
        target->HandleMessage(this, MSG_KILL, (void *)KILL_CRUSH);
        res = 0;
    } else {
        Translate(delta);
    }
    return res;
}

/* 0x4f2e39 */
void SlidingIceCube::PlaySlideSound()
{
    if (!slideSound || !SoundIsPlaying(slideSound))
        slideSound = Sound_Play(SND_ICE_SLIDE, this, 0xff, SNDF_LOOP | SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 0x1000);
}

/* 0x4f2e97 */
void SlidingIceCube::Update()
{
    s32 size;
    IceCubeRiderMessage args;
    ScnObject *list[64];
    u16 classId;
    ScnObject *current;
    s32 j;
    Vec3s delta;
    Vec3s velocity;
    Box *flowZone;
    switch (state) {
        case SIC_ST_IDLE:
            size = QueryRiders(list);
            args.pos = pos;
            args.radius = 20;
            for (j = 0; j < size; j++) {
                current = list[j];
                if (Scenaric_ClassFlags(current->GetClassId()) & SCN_CF_SHEEP_ANCHORABLE)
                    current->HandleMessage(this, MSG_SET_ANCHOR, &args);
            }
            break;
        case SIC_ST_SLIDING:
            fallTime += g_dt;
            if (cubeFlags.bounced)
                slideSpeed = Math_ApproachLinear(slideSpeed, 0, 800, 400, 4000);
            velocity.x = (-slideSpeed * g_sinTable4096[slideDir]) / 4096;
            velocity.z = (-slideSpeed * g_pCosTable[slideDir]) / 4096;
            ApplyGravity(&velocity);
            Vec3s_ScaleByDt(&velocity, &delta);
            classId = SlideMove(&delta, COLL_FLOOR | COLL_FLOOR_EDGE);
            if ((classId & COLL_WALL) && !cubeFlags.bounced) {
                slideDir = (slideDir + 0x800) & 0xfff;
                cubeFlags.bounced = 1;
                Sound_Play(SND_ICE_HIT, this, 0xff, SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 0x1000);
            }
            PlaySlideSound();
            if (slideSpeed == 0)
                SetState(SIC_ST_IDLE);
            break;
        case SIC_ST_FALLING:
            flowZone = Zones_Get(ZONE_WATER)->FindContaining(&pos);
            fallTime += g_dt;
            if (flowZone && fallTime > 0x100)
                fallTime = 0x100;
            velocity.x = velocity.z = 0;
            ApplyGravity(&velocity);
            Vec3s_ScaleByDt(&velocity, &delta);
            classId = CrushMove(&delta, COLL_WALL);
            if (flowZone) {
                if (!splashVisible)
                    ShowSplash(flowZone);
                if (AnimId() != AGLACON1_ANIM_FOND1)
                    PlayAnim(AGLACON1_ANIM_FOND1, 0, 0);
                if (pos.y >= flowZone->min[1] + 600) {
                    if (splashVisible)
                        HideSplash();
                    SetState(SIC_ST_IDLE);
                    frozenRiver->HandleMessage(this, MSG_RIVER_ADD_CARGO, 0);
                } else if (splashVisible) {
                    g_iceCubeSplashBody.AdvanceAnim();
                }
            } else if (classId & COLL_FLOOR) {
                Sound_Play(SND_ICE_HIT, this, 0xff, SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 0x1000);
                SetState(SIC_ST_RIVER_SLIDE);
                slideSpeed = 0;
            }
            break;
        case SIC_ST_RIVER_SLIDE:
            fallTime += g_dt;
            slideSpeed = Math_ApproachLinear(slideSpeed, 800, 800, 400, 4000);
            velocity.x = 0;
            velocity.z = -slideSpeed;
            ApplyGravity(&velocity);
            Vec3s_ScaleByDt(&velocity, &delta);
            classId = CrushMove(&delta, RESOLVE_SLIDE_ALL);
            if (classId & COLL_FLOOR)
                fallTime = 0;
            PlaySlideSound();
            if (fallTime >= 0x200)
                SetState(SIC_ST_FALLING);
            break;
    }
    AdvanceAnim();
}

/* 0x4f3477 */
s32 SlidingIceCube::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    switch (msgId) {
        case MSG_QUERY_ACTION:
            if (!frozenRiver && sender->GetClassId() == CLASSID_WOLF && sender->pos.y >= pos.y - 100)
                return CTX_ICECUBE;
            break;
        case MSG_USE:
        case MSG_BUMP:
            if (!frozenRiver) {
                s32 angle = (s16)((HeadingTo(&sender->pos) + 0xa00) & 0xfff);
                angle -= angle % 0x400;
                s32 parity = angle % 0x800;
                if ((cubeFlags.allowX && parity != 0) || (cubeFlags.allowY && parity == 0)) {
                    slideDir = angle;
                    slideSpeed = 800;
                    cubeFlags.bounced = 0;
                    if (state != SIC_ST_SLIDING)
                        SetState(SIC_ST_SLIDING);
                    if (msgId == MSG_BUMP)
                        Sound_Play(SND_ICE_HIT, this, 0xff, SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 0x1000);
                }
                return 1;
            }
            break;
        case MSG_PICKUP: {
            ScnObject *parent = sender;
            u8 j = (u8)(u32)arg; /* cast kept: MSG_PICKUP's arg carries the joint number */
            AttachTo(parent, j, 0, 0, 0, 0);
            SetFacing(0x155);
            SetState(SIC_ST_IDLE);
            return 1;
        }
        case MSG_DROP: {
            DropMsgArg *drop = (DropMsgArg *)arg; /* cast kept: MSG_DROP's arg is a DropMsgArg */
            Detach();
            if (!drop->flag1)
                SetPosition(&drop->pos);
            SetState(SIC_ST_FALLING);
            return 1;
        }
        case MSG_RIVER_CARGO_END:
            return 1;
    }
    return 0;
}

/* 0x4f36d3 */
void SlidingIceCube::Render(Camera *view)
{
    ScnBody::Render(view);
    if (splashVisible)
        g_iceCubeSplashBody.ScnBody::Render(view);
}

/* 0x4f3708 */
void SlidingIceCube::SetState(u8 value)
{
    if (value == SIC_ST_IDLE)
        SetUpdateMode(SCN_UPD_NORMAL);
    else
        SetUpdateMode(SCN_UPD_ALWAYS);
    PlayAnim(AGLACON1_ANIM_STAND2, 0, 0);
    if (slideSound) {
        StopSound(slideSound);
        slideSound = 0;
    }
    cubeFlags.bounced = 0;
    fallTime = 0;
    state = value;
}

/* 0x4f393f */
void SlidingIceCube::Reset()
{
    fallTime = 0;
    cubeFlags.bounced = 0;
    slideSpeed = 0;
    slideDir = 0;
    splashVisible = 0;
    SetPosition(&homePos);
    SetState(SIC_ST_IDLE);
}

/* 0x4f39a2. +0x64/+0x68 is the current generated ZoneList storage. */
void SlidingIceCube::PostLoadInit()
{
    cubeFlags.bounced = 0;
    slideSpeed = 0;
    slideDir = 0;
    slideSound = 0;
    void *rec = record;
    u32 index = Scn_GetPropU32(rec, 0);
    if (index)
        pushBoxes.Load(index);
    else
        pushBoxes.Clear();
    cubeFlags.allowX = Scn_GetPropU32(rec, 4);
    cubeFlags.allowY = Scn_GetPropU32(rec, 8);
    SnapToGround(1);
    homePos = pos;
    SetState(SIC_ST_IDLE);
    fallTime = 0;
    splashVisible = 0;
    splashReady = 0;
    frozenRiver = 0;
    Scenaric_FindByClass(CLASSID_FROZENRIVER, &frozenRiver, 1);
    if (Scn_BuildRecordFromExport(WAR_IDO_APLOUF01, g_iceCubeSplashRecord, 0, 0)) {
        g_iceCubeSplashBody.ScnBody::Init(g_iceCubeSplashRecord, 0);
        g_iceCubeSplashBody.SetNoCull(1);
        splashReady = 1;
    }
}

/* 0x4f3b78 */
ScnObject *SlidingIceCube_Create(void *record)
{
    ScnBody *obj = new SlidingIceCube;
    obj = obj->Init(record, 0);
    return obj;
}
