/* PAL PC box, 0x49ce30-0x49d301 (a ScnMobile). */
/* BYTES: dead-code. */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *, Animator *, u16, u32);

#define SDW_MEMBERS_ScnObject       \
    static void *operator new(u32); \
    void SetUpdateMode(s32 mode);


#include "sdw_classes.h"
#include "../engine/scn_tools.h"
#include "../engine/id_list.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_SCNOBJECT_BROADCASTAROUND_S32_S32_U16_U32_VOID 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_BROADCASTAROUND_S32_S32_U16_U32_VOID
#define SDW_INLINE_FREE_SETPROP_VOID_U32_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_SETPROP_VOID_U32_U32
#define SDW_INLINE_FREE_GETPROP_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_GETPROP_VOID_U32
void box::PostLoadInit()
{
    state = BOX_ST_IDLE;
    SetUpdateMode(SCN_UPD_ALWAYS);
    PlayAnim(ACAISS01_ANIM_STAND0, 0, 0);
    canRelease = 1;
}
void box::Reset()
{
    canRelease = 1;
}
void box::Update()
{
    Vec3s step, velocity;
    s32 landed;
    u16 contactFlags;
    ContactInfo contact;
    switch (state) {
        case BOX_ST_FALL:
            velocity.x = velocity.z = 0;
            velocity.y = 800;
            Vec3s_ScaleByDt(&velocity, &step);
            contactFlags = Collide_ResolveMove(&step, &contact, 0xb54, COLL_WALL, 0, 0, 10, 0, 0);
            Translate(&step);
            landed = ((contactFlags & COLL_FLOOR) && contact.floorObj == 0) ||
                     (contactFlags && contact.movableObj && !(flags & SCN_OF_IN_CINE_BOX) &&
                      contact.movableObj->GetClassId() != CLASSID_SNOWYGROUND);
            if (landed && canRelease) {
                PlayAnim(ACAISS01_ANIM_OPEN1, 0, 0);
                contents->AddToWorld(0);
                step = pos;
                step.y -= 2;
                contents->SetPosition(&step);
                if (contact.movableObj)
                    contents->SnapToGround(0);
                /* cast kept: HandleMessage's arg is a void *; this message passes a number in it */
                contents->HandleMessage(this, MSG_CONTAINER_STATE, (void *)CONTAINER_RELEASED);
                unk80 = 0;
                state = BOX_ST_OPENED;
                /* cast kept: HandleMessage's arg is a void *; this message passes the sender's class id in it */
                BroadcastAround(1000, 1000, 1000, MSG_LOUD_NOISE, (void *)(u32)GetClassId());
            }
            break;
        case BOX_ST_OPENED:
            if (AnimFlags(ANIM_F_FINISHED)) {
                RemoveFromWorld();
                state = BOX_ST_IDLE;
            }
            break;
    }
    AdvanceAnim();
}
s32 box::HandleMessage(ScnObject *, u32 msg, void *)
{
    switch (msg) {
        case MSG_GEYSER_IN:
            canRelease = 0;
            break;
        case MSG_GEYSER_OUT:
            canRelease = 1;
            break;
        case MSG_USE:
            state = BOX_ST_FALL;
            break;
    }
    return 0;
}
/* BYTES(dead-code): contentProp is read from the record and never used, as in the original */
ScnObject *box_Create(void *record)
{
    u32 contentProp;
    box *object = new box;
    object = (box *)object->Init(record, 0); /* cast kept: Init returns this as a ScnBody * */
    contentProp = GetProp(record, 0);
    object->contents = 0;
    return object;
}
void box_BuildRecord(ScnRecordSynth *out, const Vec3s *pos, u32 contentProp)
{
    u16 count;
    u32 model;
    out->pos = *pos;
    out->rot.x = out->rot.y = out->rot.z = 0;
    out->classId = CLASSID_BOX;
    model = *Scn_FindIdList(WAR_IDO_ACAISS01, &count);
    /* cast kept: an id list holds resource pointers as u32 words */
    out->modelResIndex = Dav_FindResourceIndex((void *)model);
    out->secondaryRes = 0xffff;
    SetProp(out, 0, contentProp);
}
