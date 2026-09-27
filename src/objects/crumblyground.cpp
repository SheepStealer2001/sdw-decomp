/* T131 - original object CrumblyGround.cpp (guessed name).
 * Ranges: .text 0x4ae760-0x4aef2c, .rdata 0x575e0c-0x575e30 (vtable), .data 0x57b54c-0x57b558 (a copy of the cinematic
 * header's static opcode-stride table; the map gives the tables 0x57b54c/0x57b558 to 2 of {CreditsManager,
 * CrumblyGround, CrumblyPlat}, any choice byte-identical). */
/* BYTES: layout. */
/* BYTES(layout): the Cine.h header static: every object including the cinematic header carries this copy in its .data, referenced or not */
/* PAL PC CrumblyGround, 0x4ae760..0x4aef2c. */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *, Animator *, u16, u32);

#define SDW_MEMBERS_ScnObject static void *operator new(u32);


#define SDW_MEMBERS_CollBox s32 ContainsXZ(Vec3s *point);
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#define SDW_INLINE_SCNOBJECT_ENABLEBOXCOLLIDE_S32 1
#define SDW_INLINE_SCNOBJECT_SETVISIBLE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_ENABLEBOXCOLLIDE_S32
#undef SDW_INLINE_SCNOBJECT_SETVISIBLE_S32
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_COLLBOX_CONTAINSXZ_VEC3S 1
#include "../engine/coll_box_inlines.h"
#undef SDW_INLINE_COLLBOX_CONTAINSXZ_VEC3S
extern Wolf *g_pWolf;

/* 0x57b54c - the cinematic header's static copy of the 9-byte opcode stride table (src/engine/cine.cpp,
 * g_cineOpStride 0x5816fc). The original header defined it static, so every object including it carries its own
 * unreferenced copy at the head of its .data; defined here in its place. */
static u8 s_cineOpStride[9] = {0, 8, 8, 4, 2, 2, 4, 2, 2};

s32 ObjGrid_QueryBoxPoints(const CollBox *, ScnObject **);
#define SDW_INLINE_SCNOBJECT_GETFIRSTMODELBOXINLINE 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETFIRSTMODELBOXINLINE
void CrumblyGround::PostLoadInit()
{
    Vec3s offset;
    victim = 0;
    captureActive = 0;
    offset.x = 0;
    offset.y = -16;
    offset.z = 0;
    triggerBox.Box_Translate(GetFirstModelBoxInline(), &pos);
    triggerBox.min.x += offset.x;
    triggerBox.min.y += offset.y;
    triggerBox.min.z += offset.z;
    triggerBox.max.x += offset.x;
    triggerBox.max.y += offset.y;
    triggerBox.max.z += offset.z;
    pushDelta.x = 0;
    pushDelta.y = 10;
    pushDelta.z = 0;
    unusedOffset.x = 0;
    unusedOffset.y = -30;
    unusedOffset.z = 0;
    SetState(CRUMBLYGROUND_ST_INTACT);
}
void CrumblyGround::Update()
{
    switch (state) {
        case CRUMBLYGROUND_ST_INTACT:
            CountAndCapture();
            break;
        case CRUMBLYGROUND_ST_CRACK:
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(CRUMBLYGROUND_ST_COLLAPSE);
            break;
        case CRUMBLYGROUND_ST_COLLAPSE:
            PushObjectsDown();
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(CRUMBLYGROUND_ST_GONE);
            break;
    }
    AdvanceAnim();
}
s32 CrumblyGround::HandleMessage(ScnObject *sender, u32 msg, void *arg)
{
    if (!sender)
        return 0;
    if (sender->GetClassId() == CLASSID_WOLF && state == CRUMBLYGROUND_ST_INTACT) {
        switch (msg) {
            case MSG_GROUND_QUERY:
                return GetFloor((GroundQuery *)arg); /* cast kept: MSG_GROUND_QUERY's arg is a GroundQuery */
        }
    }
    return 0;
}
void CrumblyGround::Reset()
{
    victim = 0;
    captureActive = 0;
    SetState(CRUMBLYGROUND_ST_INTACT);
    EnableBoxCollide(1);
    SetVisible(1);
}
void CrumblyGround::SetState(u8 value)
{
    if (state == value)
        return;
    state = value;
    switch (value) {
        case CRUMBLYGROUND_ST_INTACT:
            PlayAnim(A11BLOC1_ANIM_STAND, 1, 0);
            break;
        case CRUMBLYGROUND_ST_CRACK:
            PlayAnim(A11BLOC1_ANIM_BRAKE1, 0, 0);
            break;
        case CRUMBLYGROUND_ST_COLLAPSE:
            if (captureActive == 1) {
                if (victim) {
                    victim->HandleMessage(this, MSG_UNFREEZE, 0);
                    /* cast kept: MSG_KILL's arg is the WolfKillType, a number in the void * */
                    victim->HandleMessage(this, MSG_KILL, (void *)KILL_FALL_PIT);
                }
                captureActive = 0;
            }
            PlayAnim(A11BLOC1_ANIM_BRAKE, 0, 0);
            EnableBoxCollide(0);
            break;
        case CRUMBLYGROUND_ST_GONE:
            SetVisible(0);
            break;
    }
}
void CrumblyGround::CountAndCapture()
{
    u16 count = 0;
    ScnObject *rider;
    s32 index;
    overlapCount = ObjGrid_QueryBoxPoints(&triggerBox, overlappingObjects);
    if (overlapCount < 2)
        return;
    for (index = 0; index < overlapCount; index++) {
        rider = overlappingObjects[index];
        if (rider->GetClassId() == CLASSID_WOLF || rider->GetClassId() == CLASSID_SHEEP ||
            rider->GetClassId() == CLASSID_ROBOT)
            count++;
        if (count > 1 && (rider->GetClassId() == CLASSID_WOLF || rider->GetClassId() == CLASSID_ROBOT)) {
            victim = rider;
            victim->HandleMessage(this, MSG_FREEZE, 0);
            captureActive = 1;
        }
    }
    if (count > 1)
        SetState(CRUMBLYGROUND_ST_CRACK);
}
void CrumblyGround::PushObjectsDown()
{
    ScnObject *rider;
    s32 index;
    for (index = 0; index < overlapCount; index++) {
        rider = overlappingObjects[index];
        if (rider->GetClassId() != CLASSID_CRUMBLYGROUND && rider->GetClassId() != CLASSID_SHEEP &&
            rider->GetClassId() != CLASSID_ROBOT && rider->GetClassId() != CLASSID_WOLF)
            rider->Translate(&pushDelta);
    }
}
s32 CrumblyGround::GetFloor(GroundQuery *query)
{
    if (triggerBox.ContainsXZ(&g_pWolf->pos)) {
        GroundQuery *result = query;
        result->pos.y = triggerBox.min.y + 5;
        result->normal.y = -4096;
        result->normal.x = 0;
        result->normal.z = 0;
        return 1;
    }
    return 0;
}
ScnObject *CrumblyGround_Create(void *record)
{
    ScnBody *object = new CrumblyGround;
    object = object->Init(record, 0);
    return object;
}
