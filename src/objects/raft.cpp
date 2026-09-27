/*
 * T194 - original object Raft.cpp (guessed name), one translation unit.
 *   .text  0x4df190-0x4dfe7c (Raft_Create .. Raft_HandleMessage)
 *   .rdata 0x576864-0x5768a4 (g_raftWakeCornerSigns, g_raftWakeFxParams, then ??_7Raft)
 * The two tables are defined in place.
 */
/* BYTES: slot-name, view. */
/* BYTES(view): view: +0x86 is the flags byte of the embedded inlineemitter8 at +0x64 (the access widths are the original's) */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *inst, Animator *animator, u16 id, u32 opts); /* 0x550196 */
#include "../engine/scn_tools.h"
#include "../engine/approach.h"
#include "../engine/maths.h"
#include "../engine/scenaric.h"

#define SDW_MEMBERS_ScnObject static void *operator new(u32 size);


#define SDW_MEMBERS_ZoneList Box *Find(Vec3s *point);
#define SDW_MEMBERS_InlineEmitter8 InlineEmitter8();

#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_INSTFLAGS_U16 1
#define SDW_INLINE_SCNOBJECT_SETDRAWMODE_U32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_INSTFLAGS_U16
#undef SDW_INLINE_SCNOBJECT_SETDRAWMODE_U32
#define SDW_INLINE_SCNOBJECT_ENABLEBOXCOLLIDE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_ENABLEBOXCOLLIDE_S32
#define SDW_INLINE_SCNBODY_GETANIMID 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_GETANIMID
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_ZONELIST_FIND_VEC3S 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_FIND_VEC3S
inline InlineEmitter8::InlineEmitter8()
{
    base.slotPool = slotBuf;
    base.particles = particleBuf;
    base.count = 8;
    base.Emitter_Reset();
}
#define SDW_INLINE_INLINEEMITTER8_RENDERFLAT_CAMERA_S32 1
#include "../engine/emitter8_inlines.h"
#undef SDW_INLINE_INLINEEMITTER8_RENDERFLAT_CAMERA_S32
s32 Box_GroundQueryFlatTop(GroundQuery *query, CollBox *box, Vec3s *position, s32 margin);
s32 ObjGrid_QueryBoxOverlap(CollBox *box, ScnObject **out);
/* Signature from the call at 0x4df99a (three cdecl arguments). */
extern "C" s16 g_sinTable4096[5122];
extern "C" const s16 *g_pCosTable;
/* 0x576864 / 0x57686c - main CONST, ahead of Raft's vtable. Defined here with extern const (external) linkage. */
extern const s8 g_raftWakeCornerSigns[] = {-1, -1, -1, 1, 1, 1, 1, -1}; /* x/z sign per corner */
extern const EmitterFadeParams g_raftWakeFxParams = {8192, 0, 1024, 70, 140, 1};

#define SDW_INLINE_FREE_SCN_GETPROPU32_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_SCN_GETPROPU32_VOID_U32
#define SDW_INLINE_FREE_ZONE_GETLIST_U8 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_FREE_ZONE_GETLIST_U8
#define SDW_INLINE_FREE_SCENARIC_CLASSFLAGS_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_FREE_SCENARIC_CLASSFLAGS_U16

/* 0x4df190 */
ScnObject *Raft_Create(void *record)
{
    Raft *obj = new Raft;
    obj = (Raft *)obj->Init(record, 0); /* cast kept: Init returns the ScnBody * base of this object */
    return obj;
}

/* 0x4df229 */
void Raft::PostLoadInit()
{
    u16 *props = record;
    raftFlags.reset = Scn_GetPropU32(props, 0);
    wakeEmitter.base.Emitter_Reset();
    startPos = pos;
    fanSpeed = 0;
    flowSpeed = 0;
    flowHeading = 0;
    fanHeading = 0;
    SetDrawMode(0x80);
    PlayAnim(ARADEA01_ANIM_STAND1, 1, 0);
    riderCount = 0;
    raftFlags.fan = 0;
    raftFlags.afloat = 1;
    raftFlags.hit = 0;
}

/* 0x4df36d */
void Raft::Reset()
{
    if (raftFlags.reset) {
        SetPosition(&startPos);
        EnableBoxCollide(1);
    }
    raftFlags.fan = 0;
    raftFlags.hit = 0;
    raftFlags.afloat = 1;
    fanSpeed = 0;
    flowSpeed = 0;
    flowHeading = 0;
    fanHeading = 0;
    PlayAnim(ARADEA01_ANIM_STAND1, 1, 0);
}

/* 0x4df48c */
void Raft::Render(Camera *view)
{
    ScnBody::Render(view);
    /* +0x86 is the flags byte of the embedded InlineEmitter8 at +0x64. */
    if (wakeEmitter.base.flags.active)
        wakeEmitter.RenderFlat(view, 1);
}

/* 0x4df4e7 */
/* BYTES(slot-name, inferred): local names chosen for their stack slots (tools/vc6_locals.py), not recovered */
void Raft::Update()
{
    ScnObject *object;
    struct {
        Vec3s position;
        u16 radius;
    } tempNotify;
    Box *waterZone_value;
    Vec3s splashPos;
    s32 cornerLocal;
    s32 localValid;
    s32 localIndex;
    Vec3s velocity;
    ScnObject *found[64];
    s32 wantedFan_value;
    s16 waterHeading;
    Vec3s delta_temp;
    u16 foundClass_temp;
    s32 wantedFlowLocal;
    CollBox worldValue;
    s32 localCount;
    s32 localWolfOn;
    if (raftFlags.afloat) {
        worldValue.Box_Translate(GetFirstSolidBox(), &pos);
        worldValue.min.y -= 10;
        localCount = ObjGrid_QueryBoxOverlap(&worldValue, found);
        localValid = 0;
        localWolfOn = 0;
        for (localIndex = 0; localIndex < localCount; localIndex++) {
            foundClass_temp = found[localIndex]->GetClassId();
            if (!found[localIndex]->InstFlags(INST_F_ATTACHED) && foundClass_temp != CLASSID_CANONDUMMY &&
                foundClass_temp != CLASSID_CANNONBALL && foundClass_temp != CLASSID_FALLINGGATE &&
                foundClass_temp != CLASSID_FALLINGGATE2) {
                localValid++;
                if (foundClass_temp == CLASSID_WOLF)
                    localWolfOn = 1;
            } else {
                found[localIndex] = 0;
            }
        }
        if (riderCount != localValid) {
            PlayAnim(ARADEA01_ANIM_ACTION1, 0, 1);
            riderCount = localValid;
        } else if (GetAnimId() == ARADEA01_ANIM_ACTION1 && AnimFlags(ANIM_F_FINISHED)) {
            PlayAnim(ARADEA01_ANIM_STAND1, 1, 1);
        }
        if (raftFlags.fan && localWolfOn)
            wantedFan_value = 350;
        else
            wantedFan_value = 0;
        fanSpeed = Math_ApproachLinear(fanSpeed, wantedFan_value, 350, 200, 200);
        velocity.y = 0;
        if (fanSpeed) {
            velocity.x = fanSpeed * g_sinTable4096[fanHeading] / 4096;
            velocity.z = fanSpeed * g_pCosTable[fanHeading] / 4096;
            cornerLocal = (s16)((fanHeading + Rand_Range(-1024, 1024)) & 0xfff) / 1024;
            splashPos.y = pos.y;
            if (g_raftWakeCornerSigns[cornerLocal * 2] > 0)
                splashPos.x = worldValue.max.x;
            else
                splashPos.x = worldValue.min.x;
            if (g_raftWakeCornerSigns[cornerLocal * 2 + 1] > 0)
                splashPos.z = worldValue.max.z;
            else
                splashPos.z = worldValue.min.z;
            wakeEmitter.base.Emitter_UpdateFade(&g_raftWakeFxParams, &splashPos, (fanHeading + 0x800) & 0xfff, 1);
        } else {
            velocity.z = 0;
            velocity.x = velocity.z;
            if (wakeEmitter.base.flags.active)
                wakeEmitter.base.Emitter_UpdateFade(&g_raftWakeFxParams, &pos, 0, 0);
        }
        waterZone_value = Zone_GetList(ZONE_WATER)->Find(&pos);
        if (waterZone_value) {
            Zone_GetFlowHeading(waterZone_value, &waterHeading, &wantedFlowLocal);
            if (!flowSpeed)
                flowHeading = waterHeading;
            else
                flowHeading = Math_StepAngleTowards(flowHeading, waterHeading, 0x2800);
        } else {
            wantedFlowLocal = 0;
        }
        flowSpeed = Math_ApproachLinear(flowSpeed, wantedFlowLocal, 2000, 50, 100);
        if (flowSpeed) {
            velocity.x += (s16)((-flowSpeed * g_sinTable4096[flowHeading]) >> 12);
            velocity.z += (s16)((-flowSpeed * g_pCosTable[flowHeading]) >> 12);
        }
        Vec3s_ScaleByDt(&velocity, &delta_temp);
        Collide_ResolveMove(&delta_temp, 0, 0xb54, COLL_WALL | RESOLVE_KEEP_Y, 0, 0, 10, 0, 0);
        tempNotify.position.x = pos.x + delta_temp.x;
        tempNotify.position.y = pos.y + delta_temp.y;
        tempNotify.position.z = pos.z + delta_temp.z;
        tempNotify.radius = 50;
        if (raftFlags.hit && localWolfOn)
            raftFlags.afloat = 0;
        for (localIndex = 0; localIndex < localCount; localIndex++) {
            object = found[localIndex];
            if (object) {
                object->Translate(&delta_temp);
                if (Scenaric_ClassFlags(object->GetClassId()) & SCN_CF_SHEEP_ANCHORABLE)
                    object->HandleMessage(this, MSG_SET_ANCHOR, &tempNotify);
                if (!raftFlags.afloat)
                    /* cast kept: HandleMessage's arg is a void *: this message passes a number in it */
                    object->HandleMessage(this, MSG_KILL, (void *)KILL_RAFT);
            }
        }
        if (!raftFlags.afloat && raftFlags.reset)
            EnableBoxCollide(0);
        raftFlags.fan = 0;
        raftFlags.hit = 0;
    } else {
        if (wakeEmitter.base.flags.active)
            wakeEmitter.base.Emitter_UpdateFade(&g_raftWakeFxParams, &pos, 0, 0);
        if (GetAnimId() != ARADEA01_ANIM_DEAD1)
            PlayAnim(ARADEA01_ANIM_DEAD1, 0, 1);
    }
    AdvanceAnim();
}

/* 0x4dfdbf */
s32 Raft::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    switch (msgId) {
        case MSG_GROUND_QUERY:
            /* cast kept: the message arg is a void *: what it carries depends on the message id */
            return Box_GroundQueryFlatTop((GroundQuery *)arg, GetFirstSolidBox(), &pos, 0);
        case MSG_FAN_BLOW:
            /* cast kept: the message arg is a void *: what it carries depends on the message id */
            fanHeading = (s16)(s32)arg;
            raftFlags.fan = 1;
            return 1;
        case MSG_KILL:
            if (sender->GetClassId() == CLASSID_CANNONBALL || sender->GetClassId() == CLASSID_WATERMINE) {
                raftFlags.hit = 1;
                return 1;
            }
            break;
    }
    return 0;
}
