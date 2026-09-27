/* T197 - original object Resizer.cpp (guessed name): PAL PC Resizer, .text 0x4e1820-0x4e25a4, .rdata 0x5768ec-0x576910
 * (vtable), .bss 0x6cf8f8-0x6cf9fc (g_resizerState). */
/* BYTES: slot-group, view. */
/* BYTES(view): view: typed views of unnamed generated storage, proved by the original loads/stores (the access widths are the original's) */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *inst, Animator *animator, u16 id, u32 opts); /* 0x550196 */

#define SDW_MEMBERS_ScnObject                                                         \
    static void *operator new(u32 size);                                              \
    CollBox *GetFirstModelBox();                                                      \
    void StartCamera(u16 x, u16 y, u16 z, Vec3s *eye, u16 focal, u32 mode, s32 time); \
    void SetUpdateMode(u8 mode);

#include "sdw_classes.h"
#include "../engine/scn_tools.h"
#include "../engine/id_list.h"
#include "camera.h"
#include "../app/app_main.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
s32 ObjGrid_QueryBoxOverlap(CollBox *box, ScnObject **out);
void Camera_StartScripted(ScnObject *owner, Camera *camera, u16 x, u16 y, u16 z, Vec3s *eye, u16 focal, u32 mode,
                          s32 time);
extern Wolf *g_pWolf;
extern s32 g_gameTimeMs;
extern "C" s16 g_sinTable4096[5122];
extern "C" const s16 *g_pCosTable;
/* T197 .bss: candidates 0x6cf8f8, count 0x6cf9f8. */
struct ResizerCandidates {
    ScnObject *objects[64];
    s32 count;
};
ResizerCandidates g_resizerState;
#define g_resizerCandidates g_resizerState.objects
#define g_resizerCandidateCount g_resizerState.count

#define SDW_INLINE_FREE_SCN_GETPROPU32_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_SCN_GETPROPU32_VOID_U32
inline CollBox *ScnObject::GetFirstModelBox()
{
    ModelBoxList *list = inst_model->boxes;
    if (list)
        return list->boxes;
    return 0;
}
#define SDW_INLINE_SCNOBJECT_STARTCAMERA_U16_U16_U16_VEC3S_U16_U32_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_STARTCAMERA_U16_U16_U16_VEC3S_U16_U32_S32

/* 0x4e1820 */
void Resizer::PlaceAtBoxCentre(ScnObject *object, Box *box)
{
    Vec3s position;
    position.x = (box->max[0] + box->min[0]) / 2;
    position.y = height;
    position.z = (box->max[2] + box->min[2]) / 2;
    object->SetPosition(&position);
}

/* 0x4e187e */
void Resizer::PostLoadInit()
{
    u16 *props = record;
    Vec3s velocity;
    flags = 0;
    if (Scn_GetPropU32(props, 4)) {
        flags |= RESIZER_F_GROW;
        SwapModel(&altMaximize);
    } else if (Scn_GetPropU32(props, 0)) {
        flags |= RESIZER_F_EXIT_ONLY;
        SwapModel(&altExit);
    }
    if (!(flags & RESIZER_F_EXIT_ONLY))
        exitObject = Scn_GetPropObject(props, 12);
    if (!(flags & RESIZER_F_EXIT_ONLY)) {
        activationBox = 0;
        activationBox = Scn_GetPropBox(props, 8);
    }
    modelBox = GetFirstModelBox();
    height = modelBox->min.y - 5;
    if (!(flags & RESIZER_F_EXIT_ONLY)) {
        camRot.x = 0x100;
        camRot.y = -rot.y & 0xfff;
        camRot.z = 0;
        camFocal = 0x180;
        camEye.x = pos.x;
        camEye.y = pos.y;
        camEye.z = pos.z;
        velocity.x = -(g_sinTable4096[rot.y] * 300) / 4096;
        velocity.y = -250;
        velocity.z = -(g_pCosTable[rot.y] * 300) / 4096;
        camEye.x += velocity.x;
        camEye.y += velocity.y;
        camEye.z += velocity.z;
    }
    SetUpdateMode(SCN_UPD_NORMAL);
    SetState(RESIZER_ST_IDLE);
}

/* 0x4e1ba2 */
void Resizer::Reset()
{
    flags &= ~RESIZER_F_RUNTIME_MASK;
    SetState(RESIZER_ST_IDLE);
}

/* 0x4e1bc9 */
/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets */
void Resizer::Update()
{
    struct {
        CollBox box;
        s32 index;
    } w;
    if (g_pWolf->HandleMessage(this, MSG_WOLF_IS_DEAD, 0))
        flags |= RESIZER_F_WOLF_DEAD;
    if (flags & RESIZER_F_WOLF_DEAD) {
        AdvanceAnim();
    } else {
        switch (state) {
            case RESIZER_ST_IDLE:
                if (!(flags & RESIZER_F_EXIT_ONLY)) {
                    w.box = *(CollBox *)
                                activationBox; /* cast kept: Box and CollBox are two views of one 16-byte zone record */
                    g_resizerCandidateCount = ObjGrid_QueryBoxOverlap(&w.box, g_resizerCandidates);
                    for (w.index = 0; w.index < g_resizerCandidateCount; w.index++) {
                        targetClass = g_resizerCandidates[w.index]->GetClassId();
                        switch (targetClass) {
                            case CLASSID_WOLF:
                            case CLASSID_INSTANTMARTIAN:
                                if (flags & RESIZER_F_GROW) {
                                    if (g_resizerCandidates[w.index]->HandleMessage(this, MSG_QUERY_SIZE, 0) !=
                                        WOLF_SIZE_SMALL)
                                        continue;
                                } else {
                                    if (g_resizerCandidates[w.index]->HandleMessage(this, MSG_QUERY_SIZE, 0) ==
                                        WOLF_SIZE_SMALL)
                                        continue;
                                }
                                if (targetClass == CLASSID_WOLF)
                                    StartCamera(camRot.x, camRot.y, camRot.z, &camEye, camFocal, CAMSCR_BLEND_OUT,
                                                0x1000);
                                target = g_resizerCandidates[w.index];
                                SetState(RESIZER_ST_CAPTURE);
                        }
                        if (target)
                            break;
                    }
                }
                break;
            case RESIZER_ST_CAPTURE:
                PlaceAtBoxCentre(target, activationBox);
                if (!(flags & RESIZER_F_WOLF_FROZEN)) {
                    switch (targetClass) {
                        case CLASSID_WOLF:
                            if (!(flags & RESIZER_F_WOLF_FROZEN) && !(flags & RESIZER_F_WOLF_DEAD_HOLD)) {
                                if (!g_pWolf->HandleMessage(this, MSG_FREEZE, 0))
                                    break;
                                flags |= RESIZER_F_WOLF_FROZEN;
                            }
                            break;
                        case CLASSID_INSTANTMARTIAN:
                            flags |= RESIZER_F_WOLF_FROZEN;
                            break;
                    }
                    if (flags & RESIZER_F_WOLF_FROZEN) {
                        startMs = g_gameTimeMs;
                        if (flags & RESIZER_F_GROW)
                            PlayAnim(AREDUC01_ANIM_ACTION1, 0, 0);
                        else
                            PlayAnim(AREDUC01_ANIM_ACTION1, 0, 0);
                    }
                }
                if (flags & RESIZER_F_WOLF_FROZEN) {
                    elapsedMs = g_gameTimeMs - startMs;
                    if (elapsedMs >= 500 || AnimFlags(ANIM_F_FINISHED)) {
                        if (targetClass == CLASSID_WOLF)
                            g_pWolf->HandleMessage(this, MSG_WOLF_SET_INVISIBLE, 0);
                        if (flags & RESIZER_F_GROW) {
                            if (target->HandleMessage(this, MSG_SET_SIZE, 0))
                                flags |= RESIZER_F_TARGET_RESIZED;
                        } else {
                            /* cast kept: HandleMessage's arg is a void *; MSG_SET_SIZE passes 1 (shrink) or 0 (restore) */
                            if (target->HandleMessage(this, MSG_SET_SIZE, (void *)1))
                                flags |= RESIZER_F_TARGET_RESIZED;
                        }
                    }
                }
                if (flags & RESIZER_F_TARGET_RESIZED) {
                    flags &= ~RESIZER_F_TARGET_RESIZED;
                    exitObject->HandleMessage(this, MSG_RESIZER_RECEIVE, target);
                    SetState(RESIZER_ST_RESIZE);
                }
                break;
            case RESIZER_ST_RESIZE:
                switch (targetClass) {
                    case CLASSID_WOLF:
                        if (AnimFlags(ANIM_F_FINISHED) && !(flags & RESIZER_F_WOLF_DEAD_HOLD) &&
                            (flags & RESIZER_F_WOLF_FROZEN) && g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0)) {
                            flags &= ~RESIZER_F_WOLF_FROZEN;
                            SetState(RESIZER_ST_IDLE);
                            Camera_ReleaseScripted(this);
                        }
                        break;
                    case CLASSID_INSTANTMARTIAN:
                        flags &= ~RESIZER_F_WOLF_FROZEN;
                        SetState(RESIZER_ST_IDLE);
                        break;
                }
                break;
            case RESIZER_ST_EJECT:
                if (AnimFlags(ANIM_F_FINISHED))
                    SetState(RESIZER_ST_IDLE);
                break;
        }
        AdvanceAnim();
    }
}

/* 0x4e21a1 */
s32 Resizer::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    Vec3s position;
    if (sender) {
        switch (msgId) {
            case MSG_FREEZE:
                if (sender->GetClassId() == CLASSID_WOLF) {
                    flags |= RESIZER_F_WOLF_DEAD_HOLD;
                    return 1;
                }
                return 0;
            case MSG_RESIZER_RECEIVE:
                if (arg) {
                    target = (ScnObject *)arg; /* cast kept: the message arg is a void *; this one carries the target */
                    targetClass = target->GetClassId();
                    if ((flags & RESIZER_F_EXIT_ONLY) && sender->GetClassId() == CLASSID_RESIZER) {
                        position.x = pos.x;
                        position.y = height;
                        position.z = pos.z;
                        target->SetPosition(&position);
                        SetState(RESIZER_ST_EJECT);
                    }
                }
                break;
        }
    }
    return 0;
}

/* 0x4e22ac */
void Resizer::SetState(u8 newState)
{
    state = newState;
    switch (newState) {
        case RESIZER_ST_IDLE:
            target = 0;
            if (!(flags & RESIZER_F_EXIT_ONLY))
                modelBox->flags |= COLLBOX_NONSOLID;
            if (flags & RESIZER_F_GROW)
                PlayAnim(AREDUC01_ANIM_STAND, 1, 0);
            else if (flags & RESIZER_F_EXIT_ONLY)
                PlayAnim(AREDUC01_ANIM_STAND, 1, 0);
            else
                PlayAnim(AREDUC01_ANIM_STAND, 1, 0);
            break;
        case RESIZER_ST_CAPTURE:
            if (!(flags & RESIZER_F_EXIT_ONLY))
                modelBox->flags &= ~COLLBOX_NONSOLID;
            break;
        case RESIZER_ST_EJECT:
            PlayAnim(AREDUC01_ANIM_ACTION1, 1, 0);
            if (targetClass == CLASSID_WOLF)
                /* cast kept: the arg is a number (0 hide, else show) */
                g_pWolf->HandleMessage(this, MSG_WOLF_SET_INVISIBLE, (void *)1);
            break;
    }
}

/* 0x4e24ae */
ScnObject *Resizer_Create(void *record)
{
    Resizer *result = new Resizer;
    u16 alternatives[2] = {WAR_IDO_AREDUC02, WAR_IDO_AREDUC03};
    /* cast kept: InitWithAltModels returns the object as its base class */
    result = (Resizer *)result->InitWithAltModels(record, &result->model, 2, alternatives, &result->altMaximize);
    return result;
}

/* 0x4e2530 */
void Resizer_BuildSpawnRecord(ScnRecordSynth *out, Vec3s *position)
{
    void *resource;
    u16 count;
    out->pos = *position;
    out->rot.z = 0;
    out->rot.y = 0;
    out->rot.x = 0;
    out->classId = CLASSID_RESIZER;
    /* cast kept: an id list's entries are record addresses */
    resource = (void *)*Scn_FindIdList(WAR_IDO_AREDUC01, &count);
    out->modelResIndex = Dav_FindResourceIndex(resource);
    out->secondaryRes = 0xffff;
}
