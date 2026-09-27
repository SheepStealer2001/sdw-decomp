/* T132 - original object CrumblyPlat.cpp (guessed name).
 * Ranges: .text 0x4aef30-0x4af49b, .rdata 0x575e30-0x575e54 (vtable), .data 0x57b558-0x57b564 (a copy of the cinematic
 * header's static opcode-stride table; see T131). */
/* BYTES: layout. */
/* BYTES(layout): the Cine.h header static: every object including the cinematic header carries this copy in its .data, referenced or not */
/* PAL PC CrumblyPlat, 0x4aef30-0x4af49b. */
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

/* 0x57b558 - the cinematic header's static copy of the 9-byte opcode stride table (src/engine/cine1.cpp,
 * g_cineOpStride 0x5816fc). The original header defined it static, so every object including it carries its own
 * unreferenced copy at the head of its .data; defined here in its place. */
static u8 s_cineOpStride[9] = {0, 8, 8, 4, 2, 2, 4, 2, 2};

s32 ObjGrid_QueryBoxPoints(const CollBox *, ScnObject **);
#define SDW_INLINE_SCNOBJECT_GETFIRSTMODELBOXINLINE 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETFIRSTMODELBOXINLINE
void CrumblyPlat::PostLoadInit()
{
    worldBox.Box_Translate(GetFirstModelBoxInline(), &pos);
    pushDown.x = 0;
    pushDown.y = 10;
    pushDown.z = 0;
    pushUp.x = 0;
    pushUp.y = -30;
    pushUp.z = 0;
    state = CRUMBLYPLAT_ST_GONE;
    SetState(CRUMBLYPLAT_ST_INTACT);
}
void CrumblyPlat::Update()
{
    switch (state) {
        case CRUMBLYPLAT_ST_INTACT:
            CheckWolfOnTop();
            break;
        case CRUMBLYPLAT_ST_BREAK:
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(CRUMBLYPLAT_ST_GONE);
            break;
    }
    AdvanceAnim();
}
s32 CrumblyPlat::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    if (!sender)
        return 0;
    switch (msgId) {
        case MSG_GROUND_QUERY:
            if (state == CRUMBLYPLAT_ST_INTACT)
                /* cast kept: the message arg is a void *; this one carries a GroundQuery */
                return GroundQuery((::GroundQuery *)arg);
            break;
        case MSG_FREEZE:
            return 1;
    }
    return 0;
}
void CrumblyPlat::Reset()
{
    EnableBoxCollide(1);
    SetState(CRUMBLYPLAT_ST_INTACT);
    SetVisible(1);
}
void CrumblyPlat::SetState(u8 value)
{
    if (state == value)
        return;
    state = value;
    switch (value) {
        case CRUMBLYPLAT_ST_INTACT:
            PlayAnim(APLANC01_ANIM_STAND, 1, 0);
            break;
        case CRUMBLYPLAT_ST_BREAK:
            PlayAnim(APLANC01_ANIM_BREAK, 0, 0);
            break;
        case CRUMBLYPLAT_ST_GONE:
            SetVisible(0);
            break;
    }
}
void CrumblyPlat::CheckWolfOnTop()
{
    s32 index;
    ScnObject *rider;
    hitCount = ObjGrid_QueryBoxPoints(&worldBox, hits);
    if (!hitCount)
        return;
    for (index = 0; index < hitCount; index++) {
        rider = hits[index];
        if (rider->GetClassId() == CLASSID_WOLF) {
            EnableBoxCollide(0);
            SetState(CRUMBLYPLAT_ST_BREAK);
        }
    }
}
void CrumblyPlat::PushRiders()
{
    s32 index;
    ScnObject *rider;
    for (index = 0; index < hitCount; index++) {
        rider = hits[index];
        if (rider->GetClassId() == CLASSID_WOLF)
            rider->Translate(&pushUp);
        else if (rider->GetClassId() != CLASSID_CRUMBLYPLAT && rider->GetClassId() != CLASSID_SHEEP &&
                 rider->GetClassId() != CLASSID_ROBOT)
            rider->Translate(&pushDown);
    }
}
s32 CrumblyPlat::GroundQuery(::GroundQuery *query)
{
    if (worldBox.ContainsXZ(&g_pWolf->pos)) {
        ::GroundQuery *result = query;
        result->pos.y = worldBox.min.y + 5;
        result->normal.y = -4096;
        result->normal.x = 0;
        result->normal.z = 0;
        return 1;
    }
    return 0;
}
ScnObject *CrumblyPlat_Create(void *record)
{
    CrumblyPlat *object = new CrumblyPlat;
    object = (CrumblyPlat *)object->Init(record, 0); /* cast kept: Init returns the object as its base class */
    return object;
}
