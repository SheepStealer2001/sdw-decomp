/* PAL PC WaterMine. The detonation flag is a generated bitfield. */
/* BYTES: view. */
/* BYTES(view): view: the detonation flag is a generated bitfield (the access widths are the original's) */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *, Animator *, u16, u32);
#include "../engine/scn_tools.h"
#include "../engine/scenaric.h"
#include "../app/app_main.h"
#include "camera.h"

#define SDW_MEMBERS_ScnObject static void *operator new(u32);


#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_ISVISIBLE 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_ISVISIBLE
#define SDW_INLINE_SCNOBJECT_ENABLEBOXCOLLIDE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_ENABLEBOXCOLLIDE_S32
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_SHADOW_SETVISIBLE_S32 1
#include "../engine/shadow_inlines.h"
#undef SDW_INLINE_SHADOW_SETVISIBLE_S32
#define SDW_INLINE_ZONELIST_FINDCONTAINING_VEC3S 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_FINDCONTAINING_VEC3S
#define g_camPos (g_camera.pos)

#define SDW_INLINE_FREE_ZONES_GET_U8 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_FREE_ZONES_GET_U8
s32 ObjGrid_QueryBoxOverlap(CollBox *, ScnObject **);
s32 Vec3s_Dist(Vec3s *, Vec3s *);
s32 WaterMine::KillOverlapping()
{
    s32 objectIndex;
    ScnObject *raftHit, *candidate, *overlaps[64];
    s32 didKill, count;
    didKill = 0;
    raftHit = 0;
    count = ObjGrid_QueryBoxOverlap(&triggerBox, overlaps);
    for (objectIndex = 0; objectIndex < count; objectIndex++) {
        candidate = overlaps[objectIndex];
        if (candidate->GetClassId() == CLASSID_RAFT)
            raftHit = candidate;
    }
    if (raftHit) {
        /* cast kept: HandleMessage's arg is a void *; MSG_KILL passes the kill type in it */
        raftHit->HandleMessage(this, MSG_KILL, (void *)KILL_GENERIC);
        didKill = 1;
    } else
        for (objectIndex = 0; objectIndex < count; objectIndex++) {
            candidate = overlaps[objectIndex];
            if (candidate->GetClassId() != CLASSID_WATERMINE && candidate->IsVisible()) {
                /* cast kept: HandleMessage's arg is a void *; MSG_KILL passes the kill type in it */
                candidate->HandleMessage(this, MSG_KILL, (void *)KILL_GENERIC);
                didKill = 1;
            }
        }
    return didKill;
}
void WaterMine::Update()
{
    s32 distance;
    Vec3s point;
    switch (state) {
        case MINE_ST_SAFE:
            if (KillOverlapping()) {
                distance = Vec3s_Dist(&pos, &g_camPos);
                if (distance < 5000)
                    Camera_StartShake((s16)((5000 - distance) * 60 / 5000), 8192);
                if (waterBox && homePos.y >= waterBox->min[1] + 200) {
                    SwapModel(&altModels[2]);
                    PlayAnim(WATERMINE_ANIM_2, 0, 0);
                } else {
                    SwapModel(&altModels[0]);
                    PlayAnim(WATERMINE_ANIM_3, 0, 0);
                }
                flags.detonated = 1;
                state = MINE_ST_REARMING;
                shadow.SetVisible(0);
                EnableBoxCollide(0);
            }
            break;
        case MINE_ST_REARMING:
            if (AnimFlags(ANIM_F_FINISHED)) {
                SwapModel(&altModels[1]);
                PlayAnim(WATERMINE_ANIM_1, 0, 0);
                if (waterBox) {
                    point = pos;
                    point.y = waterBox->min[1];
                    SetPosition(&point);
                }
                state = MINE_ST_ARMED;
            }
            break;
    }
    AdvanceAnim();
}
void WaterMine::SetIdleModel()
{
    if (waterBox && homePos.y >= waterBox->min[1] + 20) {
        SwapModel(&modelIdle);
        PlayAnim(WATERMINE_ANIM_IDLE, 0, 0);
    } else {
        SwapModel(&altModels[3]);
        PlayAnim(WATERMINE_ANIM_IDLE, 0, 0);
    }
    flags.detonated = 0;
}
void WaterMine::Reset()
{
    if (flags.detonated)
        SetIdleModel();
    SetPosition(&homePos);
    state = MINE_ST_SAFE;
    PlayAnim(WATERMINE_ANIM_IDLE, 0, 0);
    shadow.SetVisible(1);
    EnableBoxCollide(1);
}
void WaterMine::PostLoadInit()
{
    homePos = pos;
    waterBox = Zones_Get(ZONE_WATER)->FindContaining(&pos);
    triggerBox.max = pos;
    triggerBox.min = triggerBox.max;
    triggerBox.min.x -= 150;
    triggerBox.min.y -= 150;
    triggerBox.min.z -= 150;
    triggerBox.max.x += 150;
    triggerBox.max.y += 150;
    triggerBox.max.z += 150;
    SetIdleModel();
    state = MINE_ST_SAFE;
}
ScnObject *WaterMine_Create(void *record)
{
    static const u16 modelIds[] = {WAR_IDO_AEXPLOS1, WAR_IDO_APLOUF01, WAR_IDO_AEXPLOS3, WAR_IDO_AMINEM02};
    WaterMine *object = new WaterMine;
    /* cast kept: InitWithAltModels returns this as a ScnObject * */
    object = (WaterMine *)object->InitWithAltModels(record, &object->modelIdle, 4, modelIds, object->altModels);
    return object;
}
