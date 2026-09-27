/*
 * T181 - original object Mailbox.cpp (guessed name), one translation unit.
 *   .text  0x4d2ce0-0x4d3e37 (Mailbox_Init .. Mailbox_Create)
 *   .rdata 0x5765dc-0x576600 (??_7Mailbox)
 *   .data  0x57b6c4-0x57b6d0 (this TU's copy of the Cine header's 9-byte opcode stride table, + 3 pad; the map's
 *          choice - 0x57b6d0 is the alternative, the two copies are byte-identical)
 * Init builds the containers through box_BuildRecord / FloatingBox_BuildRecord and box_Create / FloatingBox_Create;
 * the match of this file's relocations fixes those four declarations to their addresses.
 */
/* BYTES: layout. */
/* BYTES(layout): the Cine.h header static: every object including the cinematic header carries this copy in its .data, referenced or not */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class ScnObject;
class Camera;
class Instance;
struct Animator;
u32 Anim_Start(Instance *inst, Animator *animator, u16 id, u32 opts);
void Camera_StartScripted(ScnObject *owner, Camera *cam, u16 rx, u16 ry, u16 rz, Vec3s *eye, u16 focal, u32 mode,
                          s32 time);
#include "../app/app_main.h"
#include "camera.h"
#include "../engine/cine.h"
#include "../engine/scn_tools.h"
#include "../engine/id_list.h"
#include "../engine/scenaric.h"
#include "box.h"
#include "floatingbox.h"
#define SDW_INLINE_FREE_INSTFLAGS_SET_U16_U16 1
#include "instance_inlines.h"
#undef SDW_INLINE_FREE_INSTFLAGS_SET_U16_U16
#define SDW_INLINE_FREE_INSTFLAGS_CLEAR_U16_U16 1
#include "instance_inlines.h"
#undef SDW_INLINE_FREE_INSTFLAGS_CLEAR_U16_U16

#define SDW_MEMBERS_ScnObject                    \
    static void *operator new(u32 size);         \
    void SetTint(u32 color, s16 amount, s32 on); \
    void SetUpdateMode(u8 mode);                 \
    void StartCamera(u16 rx, u16 ry, u16 rz, Vec3s *eye, u16 focal, u32 mode);


#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_ISINWORLD 1
#define SDW_INLINE_SCNOBJECT_SETVISIBLE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_ISINWORLD
#undef SDW_INLINE_SCNOBJECT_SETVISIBLE_S32
#define SDW_INLINE_SCNOBJECT_SETTINT_U32_S16_S32 1
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8 1
#define SDW_INLINE_SCNOBJECT_STARTCAMERA_U16_U16_U16_VEC3S_U16_U32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETTINT_U32_S16_S32
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8
#undef SDW_INLINE_SCNOBJECT_STARTCAMERA_U16_U16_U16_VEC3S_U16_U32
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_CINE_ISACTIVE 1
#include "../engine/cine_inlines.h"
#undef SDW_INLINE_CINE_ISACTIVE
#define SDW_INLINE_SHADOW_SETVISIBLE_S32 1
#include "../engine/shadow_inlines.h"
#undef SDW_INLINE_SHADOW_SETVISIBLE_S32
/* 0x57b6c4 - this TU's copy of the cinematic header's static opcode stride table (payload bytes per key, by opcode;
   the copies src/engine/cine.cpp and cine2.cpp index are g_cineOpStride 0x5816fc, g_cineOpStride2 and
   g_cineOpStride3). A static table in a header that every cinematic .cpp includes: each includer gets its own
   unreferenced copy in .data, ahead of its own data. Local definition standing in for that header, named
   s_cineOpStride as in the other batches (a static must not reuse the global name g_cineOpStride). */
static u8 s_cineOpStride[9] = {0, 8, 8, 4, 2, 2, 4, 2, 2};

u16 Sound_Play(u16 soundId, void *owner, u16 volume, u8 flags, s32 rate); /* 0x5491b8 */
extern s32 g_dtMs;
#define SDW_INLINE_FREE_PROPU32_VOID_U32 2
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_PROPU32_VOID_U32
#define SDW_INLINE_FREE_ZONES_GET_U8 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_FREE_ZONES_GET_U8
#define SDW_INLINE_ZONELIST_CONTAINS_VEC3S 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_CONTAINS_VEC3S

/* 0x4d2ce0 Mailbox_Init */
void Mailbox::PostLoadInit()
{
    s32 p;
    ScnObject *q;
    u32 r;
    s32 s;
    u16 t;
    u16 u;
    u16 v[4];
    void *w;
    u32 *x;
    ScnMobile *i;
    ScnRecordSynth *j;
    w = record;
    SnapToGround(1);
    PlayAnim(ALETTR01_ANIM_STAND1, 0, 0);
    t = PropU32(w, 0x18);
    hasRock = t != 0;
    if (hasRock)
        rock = Scenaric_FindByIdList(t);
    floatingBoxes = (u16)PropU32(w, 0x10);
    for (p = 0; p < 4; p++) {
        items[p] = 0;
        boxes[p] = 0;
    }
    v[0] = PropU32(w, 0);
    v[1] = PropU32(w, 4);
    v[2] = PropU32(w, 8);
    v[3] = PropU32(w, 0xc);
    mbFlags.generated = 1;
    for (p = 0; p < 4; p++) {
        if (v[p]) {
            mbFlags.generated = 0;
            boxes[p] = Scenaric_FindByIdList(v[p]);
            /* cast kept: the designer BOX objects are ScnMobile classes (box, FloatingBox) */
            i = (ScnMobile *)boxes[p];
            i->SetVisible(0);
            if (hasRock) {
                i->SetUpdateMode(SCN_UPD_CINE);
                i->shadow.SetVisible(0);
                i->weight = 60;
            }
        }
    }
    itemIds[0] = PropU32(w, 0x24);
    itemIds[1] = PropU32(w, 0x28);
    itemIds[2] = PropU32(w, 0x2c);
    itemIds[3] = PropU32(w, 0x30);
    for (s = 0; s < 4; s++) {
        if (itemIds[s]) {
            q = Scenaric_FindByIdList(itemIds[s]);
            items[s] = q;
            /* cast kept: MSG_CONTAINER_STATE's arg is the ContainerState, a number in the void * */
            q->HandleMessage(this, MSG_CONTAINER_STATE, (void *)CONTAINER_STORED);
            q->RemoveFromWorld();
            /* cast kept: the item's WAR record has the ScnRecordSynth layout (its position at +4) */
            j = (ScnRecordSynth *)q->record;
            itemSpawnPos[s] = j->pos;
        }
    }
    if (mbFlags.generated) {
        for (s = 0; s < 4; s++) {
            if (itemIds[s]) {
                if (floatingBoxes) {
                    /* cast kept: a synthesised record is raw bytes: the ScnRecordSynth header, then one property */
                    FloatingBox_BuildRecord((ScnRecordSynth *)floatingBoxRecords[s], &itemSpawnPos[s], itemIds[s]);
                    boxes[s] = FloatingBox_Create(floatingBoxRecords[s]);
                } else {
                    /* cast kept: a synthesised record is raw bytes: the ScnRecordSynth header, then one property */
                    box_BuildRecord((ScnRecordSynth *)boxRecords[s], &itemSpawnPos[s], itemIds[s]);
                    boxes[s] = box_Create(boxRecords[s]);
                }
            }
        }
    }
    r = PropU32(w, 0x14);
    x = Scn_FindIdList((u16)r, &u);
    /* cast kept: an id list holds record pointers as u32 words; this one lists cameras */
    camera = x ? (CamSetup *)*x : 0;
    cameraTimeMs = (u16)PropU32(w, 0x34);
    if (cameraTimeMs < 0)
        cameraTimeMs = 0;
    opener = 0;
    mbFlags.opened = 0;
    mbFlags.committed = 0;
    cameraTimer = 0;
    idle = 1;
    deliveryPending = 0;
    if (Zones_Get(ZONE_SHADOW)->Contains(&pos))
        SetTint(0, 0x600, 1);
}

/* 0x4d3475 Mailbox_Reset */
void Mailbox::Reset()
{
    idle = 1;
    deliveryPending = 0;
    cameraTimer = 0;
    SetUpdateMode(SCN_UPD_NORMAL);
    PlayAnim(ALETTR01_ANIM_STAND1, 0, 0);
    if (opener) {
        opener->HandleMessage(this, MSG_UNFREEZE, 0);
        opener = 0;
    }
}

/* 0x4d35c8 Mailbox_Rollback */
void Mailbox::Rollback()
{
    s32 p;
    ScnObject *q;
    ScnObject *r;
    for (p = 0; p < 4; p++) {
        if (itemIds[p]) {
            q = items[p];
            /* cast kept: MSG_CONTAINER_STATE's arg is the ContainerState, a number in the void * */
            q->HandleMessage(this, MSG_CONTAINER_STATE, (void *)CONTAINER_STORED);
            if (q->IsInWorld())
                q->RemoveFromWorld();
            r = boxes[p];
            if (r->IsInWorld())
                r->RemoveFromWorld();
        }
    }
    mbFlags.opened = 0;
    deliveryPending = 0;
}

/* 0x4d368b Mailbox_Deliver */
void Mailbox::Deliver()
{
    s32 p;
    ScnObject *q;
    FloatingBox *r;
    box *s;
    if (mbFlags.generated) {
        for (p = 0; p < 4; p++) {
            if (itemIds[p]) {
                q = items[p];
                if (floatingBoxes) {
                    /* cast kept: the generated containers are FloatingBox objects when floatingBoxes is set */
                    r = (FloatingBox *)boxes[p];
                    r->PostLoadInit();
                    r->content = q;
                    r->AddToWorld(&itemSpawnPos[p]);
                    r->HandleMessage(this, MSG_USE, 0);
                } else {
                    s = (box *)boxes[p]; /* cast kept: the containers are box objects here */
                    s->PostLoadInit();
                    s->contents = q;
                    s->AddToWorld(&itemSpawnPos[p]);
                    s->HandleMessage(this, MSG_USE, 0);
                }
            }
        }
    } else {
        if (hasRock)
            rock->HandleMessage(this, MSG_MAILBOX_DELIVERED, 0);
        for (p = 0; p < 4; p++) {
            if (itemIds[p]) {
                q = items[p];
                s = (box *)boxes[p]; /* cast kept: the containers are box objects here */
                s->flags |= SCN_OF_CINE_UPDATE;
                s->contents = q;
                s->SetPosition(&q->pos);
                s->SetVisible(1);
                boxes[p]->HandleMessage(this, MSG_USE, 0);
            }
        }
    }
}

/* 0x4d389a Mailbox_Update */
void Mailbox::Update()
{
    if (deliveryPending && !g_cinePlayer.IsActive()) {
        Deliver();
        deliveryPending = 0;
    }
    switch (idle) {
        case 0:
            if (cameraTimer > cameraTimeMs) {
                if (opener) {
                    opener->HandleMessage(this, MSG_UNFREEZE, 0);
                    opener = 0;
                }
                idle = 1;
                cameraTimer = 0;
                Camera_ReleaseScripted(this);
                SetUpdateMode(SCN_UPD_NORMAL);
            } else if (!g_cinePlayer.IsActive())
                cameraTimer += g_dtMs;
            break;
    }
    AdvanceAnim();
}

/* 0x4d3a3c Mailbox_HandleMessage */
s32 Mailbox::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    switch (msgId) {
        case MSG_USE:
            if ((sender->GetClassId() == CLASSID_WOLF || sender->GetClassId() == CLASSID_ROBOT) && !mbFlags.opened) {
                if (camera) {
                    if (!(g_camMode == CAM_SCRIPT_BLEND_IN || g_camMode == CAM_SCRIPT_TO_SCRIPT ||
                                  g_camMode == CAM_SCRIPTED
                              ? 1
                              : 0)) {
                        opener = sender;
                        opener->HandleMessage(this, MSG_FREEZE, 0);
                        StartCamera(camera->rot[0], camera->rot[1], camera->rot[2], &camera->eye, camera->focal,
                                    CAMSCR_BLEND_IN | CAMSCR_BLEND_OUT);
                    }
                }
                PlayAnim(ALETTR01_ANIM_LETTER1, 0, 1);
                idle = 0;
                mbFlags.opened = 1;
                Sound_Play(SND_SGLPLANE, this, 0x7f, SNDF_POSITIONAL | SNDF_NO_ATTENUATION, 0xc00);
                SetUpdateMode(SCN_UPD_ALWAYS);
                deliveryPending = 1;
            }
            break;
        case MSG_QUERY_ACTION:
            if ((sender->GetClassId() == CLASSID_WOLF || sender->GetClassId() == CLASSID_ROBOT) && !mbFlags.opened)
                return CTX_ACTIVATE;
            break;
        case MSG_FREEZE:
            opener = 0;
            return 1;
        case MSG_CHECKPOINT_COMMIT:
            if (mbFlags.opened)
                mbFlags.committed = 1;
            return 1;
        case MSG_CHECKPOINT_ROLLBACK:
            if (mbFlags.opened && !mbFlags.committed && mbFlags.generated)
                Rollback();
            return 1;
    }
    return 0;
}

/* 0x4d3dd0 Mailbox_Create */
ScnObject *Mailbox_Create(void *record)
{
    Mailbox *obj = new Mailbox;
    obj = (Mailbox *)obj->Init(record, 0); /* cast kept: Init returns the object as a ScnBody * */
    return obj;
}
