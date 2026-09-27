/* PAL PC RollingCarpet (class 115), 0x4e58e0-0x4e7317: the rolling belt that a rider drives, with its RCarpetMobile targets. */
/* BYTES: slot-group, view. */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
#include "../sdk/ddraw.h"
#include "../sdk/crt.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *inst, Animator *animator, u16 id, u32 opts); /* 0x550196 */
#include "../engine/sound_mgr.h"
#include "../engine/scn_tools.h"
#include "../engine/collide.h"
#include "camera.h"
#include "../engine/id_list.h"
#include "../app/app_main.h"
#include "../engine/game_state.h"
#include "../engine/draw2d.h"

#define SDW_MEMBERS_ScnObject                      \
    static void *operator new(u32 size);           \
    void SetRotation(Vec3s *rotation);             \
    CollBox *GetFirstModelBox();                   \
    void GetModelBoxes(CollBox **out, u32 *count); \
    void StartCamera(u16 x, u16 y, u16 z, Vec3s *position, u16 focal, u32 mode, s32 duration);

#define SDW_MEMBERS_CollBox                                                                      \
    s32 ContainsPointXZ(Vec3s *point) /* inline (source-only) */                                 \
    {                                                                                            \
        return point->x >= min.x && point->x <= max.x && point->z >= min.z && point->z <= max.z; \
    }
#define SDW_MEMBERS_Texture                                               \
    Texture(D3DApp *app, u32 width, u32 height, u32 format, s32 *result); \
    void Surface_LockForRead(DDSURFACEDESC2 *desc);                       \
    long Surface_LockForWrite(DDSURFACEDESC2 *desc);
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_GETFACING 1
#define SDW_INLINE_SCNOBJECT_INSTFLAGS_U16 1
#define SDW_INLINE_SCNOBJECT_SETROTATION_VEC3S 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_GETFACING
#undef SDW_INLINE_SCNOBJECT_INSTFLAGS_U16
#undef SDW_INLINE_SCNOBJECT_SETROTATION_VEC3S
#define SDW_INLINE_SCNOBJECT_ENABLEBOXCOLLIDE_S32 1
#define SDW_INLINE_SCNOBJECT_STOPSOUND_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_ENABLEBOXCOLLIDE_S32
#undef SDW_INLINE_SCNOBJECT_STOPSOUND_U16
#define SDW_INLINE_SCNOBJECT_ISSOUNDPLAYING_U16 1
#define SDW_INLINE_SCNOBJECT_SETSOUNDRATE_U16_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_ISSOUNDPLAYING_U16
#undef SDW_INLINE_SCNOBJECT_SETSOUNDRATE_U16_S32
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32

/* The packed DAV directory, as in src/engine/load_dav.cpp. */
#pragma pack(push, 1)
struct DavDirectory {
    u16 indexCount, bitmapCount, unk04;
    u16 *indices;
    DavBitmapRec *bitmaps;
    u32 fileSize;
    u32 *idLists;
};
#pragma pack(pop)

s32 Box_GroundQueryFlatTop(GroundQuery *query, CollBox *box, Vec3s *position, s32 margin);
s32 ObjGrid_QueryBoxOverlap(CollBox *box, ScnObject **out);
void Camera_StartScripted(ScnObject *owner, Camera *cam, u16 x, u16 y, u16 z, Vec3s *position, u16 focal, u32 mode,
                          s32 duration);
u16 Sound_Play(u16 id, void *owner, u16 volume, u8 flags, s32 rate);
extern s32 g_dtMs;
/* T201 .bss 0x6cfa30..0x6cfa50, recovered field order. */
struct RollingCarpetBeltState {
    Texture *backup;
    TexScroll scroll;
    s32 ready;
};
RollingCarpetBeltState g_rollingCarpetBelt;
#define g_rollingCarpetBeltBackup g_rollingCarpetBelt.backup
#define g_rollingCarpetBeltScroll g_rollingCarpetBelt.scroll
#define g_rollingCarpetBeltReady g_rollingCarpetBelt.ready

inline void Scn_GetPropU32(void *props, u32 offset, u32 *out)
{
    /* cast kept: designer properties are 4-byte slots at byte offsets of the raw WAR record */
    *out = *(u32 *)((u8 *)props + offset + 0x14);
}
#define SDW_INLINE_FREE_SCN_GETPROPS32_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_SCN_GETPROPS32_VOID_U32
#define SDW_INLINE_SCNOBJECT_GETMODELBOXES_COLLBOX_U32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETMODELBOXES_COLLBOX_U32
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

/* 0x4e58e0 */
s32 RollingCarpet::CustomCollide(ScnObject *querier, CollBox *mover, Vec3s *disp, s32 *outFrac, s32 *outY,
                                 CollContact *contacts, s32 *nContacts, u32 mode)
{
    return Collide_BoxVsObjBox(this, mover, disp, collBox, &pos, outFrac, outY, contacts, nContacts);
}

/* 0x4e5922 */
/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets */
/* BYTES(view, inferred): a CollBox entry read as the Box it starts with */
void RollingCarpet::PostLoadInit()
{
    struct {
        u32 mode;
        u16 pad, heading;
        CollBox *boxes;
        u32 count, index;
        u16 *props;
    } w;
    w.props = record;
    camera = Scn_GetPropCamera(w.props, 0);
    targets[0] = Scn_GetPropObject(w.props, 0xc);
    targets[1] = Scn_GetPropObject(w.props, 0x10);
    targets[2] = Scn_GetPropObject(w.props, 0x14);
    targets[3] = Scn_GetPropObject(w.props, 0x18);
    targets[4] = Scn_GetPropObject(w.props, 0x1c);
    Scn_GetPropU32(w.props, 4, &w.mode);
    /* cast kept (the five sends below): MSG_CARPET_SET_MODE's arg is the RCarpetMobileMode, a number in the void * */
    if (targets[0])
        targets[0]->HandleMessage(this, MSG_CARPET_SET_MODE, (void *)w.mode);
    if (targets[1])
        /* cast kept: message arguments travel as void * */
        targets[1]->HandleMessage(this, MSG_CARPET_SET_MODE, (void *)w.mode);
    if (targets[2])
        targets[2]->HandleMessage(this, MSG_CARPET_SET_MODE, (void *)w.mode);
    if (targets[3])
        targets[3]->HandleMessage(this, MSG_CARPET_SET_MODE, (void *)w.mode);
    if (targets[4])
        targets[4]->HandleMessage(this, MSG_CARPET_SET_MODE, (void *)w.mode);
    ratio = Scn_GetPropS32(w.props, 8);
    ratio <<= 12;
    ratio /= 10;
    GetModelBoxes(&w.boxes, &w.count);
    topBox.Box_Translate(GetFirstModelBox(), &pos);
    topBox.max.y = topBox.min.y;
    topBox.min.y -= 50;
    topBox.max.z -= 150;
    topBox.min.z += 150;
    topBox.max.x -= 50;
    topBox.min.x += 50;
    center.x = (topBox.min.x + topBox.max.x) >> 1;
    center.z = (topBox.min.z + topBox.max.z) >> 1;
    rollRot.x = rot.x;
    rollRot.y = rot.y;
    rollRot.z = rot.z;
    w.heading = rollRot.y & 0xfff;
    if (w.heading < 0x200)
        axis = RC_AXIS_Z;
    else if (w.heading < 0x600)
        axis = RC_AXIS_X;
    else if (w.heading < 0xa00)
        axis = RC_AXIS_Z;
    else if (w.heading < 0xe00)
        axis = RC_AXIS_X;
    else
        axis = RC_AXIS_Z;
    collBox = 0;
    for (w.index = 0; w.index < w.count; w.index++) {
        if (axis == RC_AXIS_Z) {
            if (!(w.boxes[w.index].flags & COLLBOX_DOOR_SET))
                collBox = &w.boxes[w.index];
        } else if (w.boxes[w.index].flags & COLLBOX_DOOR_SET) {
            collBox = &w.boxes[w.index];
        }
    }
    EnableBoxCollide(0);
    if (!g_rollingCarpetBeltReady) {
        InitBeltScroll(10);
        g_rollingCarpetBeltReady = 1;
    }
    rcFlags.camera = 0;
    rcFlags.registered = 0;
    SetState(RC_ST_IDLE);
}

/* 0x4e5ddf */
void RollingCarpet::Reset()
{
    if (rcFlags.registered) {
        SetState(RC_ST_RELEASE);
        timerMs = 0;
    } else
        SetState(RC_ST_IDLE);
    motorSound = 0;
}

/* 0x4e5e2c */
/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets */
ScnObject *RollingCarpet::FindRider()
{
    struct {
        u16 pad, index;
        ScnObject *selected;
        ScnObject *found[65];
        s32 count;
    } w;
    w.selected = 0;
    w.count = ObjGrid_QueryBoxOverlap(&topBox, w.found);
    if (w.count != 0 && w.count < 3) {
        for (w.index = 0; w.index < w.count; w.index++) {
            if (!w.found[w.index]->InstFlags(INST_F_ATTACHED) && w.found[w.index] != this)
                w.selected = w.found[w.index];
        }
    }
    return w.selected;
}

/* 0x4e5ee9 */
/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets; pad fill gaps */
void RollingCarpet::FaceRider()
{
    struct {
        s32 unused, forward;
        u16 pad;
        s16 heading;
    } w;
    w.unused = 0;
    w.heading = rider->GetFacing();
    switch (axis) {
        case RC_AXIS_Z:
            if (w.heading < 0x400) {
                rollRot.y = 0x800;
                w.forward = 0;
            } else if (w.heading < 0xc00) {
                rollRot.y = 0;
                w.forward = 1;
            } else {
                rollRot.y = 0x800;
                w.forward = 0;
            }
            break;
        case RC_AXIS_X:
            if (w.heading < 0x800) {
                rollRot.y = 0xc00;
                w.forward = 0;
            } else {
                rollRot.y = 0x400;
                w.forward = 1;
            }
            break;
    }
    if (rot.y != rollRot.y)
        SetRotation(&rollRot);
}

/* 0x4e5fe6 */
void RollingCarpet::Update()
{
    s32 speed;
    switch (state) {
        case RC_ST_IDLE:
            rider = FindRider();
            if (rider)
                SetState(RC_ST_REGISTER);
            riderDelta = 0;
            riderSpeed = 0;
            rcFlags.camera = 0;
            break;
        case RC_ST_REGISTER:
            SetState(RC_ST_RIDDEN);
            break;
        case RC_ST_RELEASE:
            timerMs -= g_dtMs;
            if (timerMs <= 0)
                SetState(RC_ST_IDLE);
            break;
        case RC_ST_RIDDEN:
            /* cast kept: MSG_AXIS_LOCK's arg is the RollingCarpetAxis, a number in the void * */
            rider->HandleMessage(this, MSG_AXIS_LOCK, (void *)(u32)axis);
            if (rider->HandleMessage(this, MSG_QUERY_CONTROLLED, 0)) {
                if (!rcFlags.camera && camera) {
                    rcFlags.camera = 1;
                    StartCamera(camera->rot[0], camera->rot[1], camera->rot[2], &camera->eye, camera->focal,
                                CAMSCR_BLEND_IN | CAMSCR_BLEND_OUT, 0x1000);
                }
            } else if (rcFlags.camera) {
                rcFlags.camera = 0;
                Camera_ReleaseScripted(this);
            }
            if (rider->GetClassId() == CLASSID_WOLF) {
                if ((riderSpeed >= 0 ? riderSpeed : -riderSpeed) >= 250)
                    SetState(RC_ST_ROLLING);
            } else if ((riderSpeed >= 0 ? riderSpeed : -riderSpeed) >= 150)
                SetState(RC_ST_ROLLING);
            if (!topBox.ContainsPointXZ(&rider->pos) || rider->InstFlags(INST_F_ATTACHED))
                SetState(RC_ST_RELEASE);
            break;
        case RC_ST_ROLLING:
            if (rider->GetClassId() == CLASSID_WOLF) {
                if ((riderSpeed >= 0 ? riderSpeed : -riderSpeed) < 250)
                    SetState(RC_ST_RIDDEN);
            } else if ((riderSpeed >= 0 ? riderSpeed : -riderSpeed) < 150)
                SetState(RC_ST_RIDDEN);
            if ((riderSpeed >= 0 ? riderSpeed : -riderSpeed) >= 700)
                SetState(RC_ST_ROLLING_FAST);
            /* cast kept: MSG_AXIS_LOCK's arg is the RollingCarpetAxis, a number in the void * */
            rider->HandleMessage(this, MSG_AXIS_LOCK, (void *)(u32)axis);
            if (rider->InstFlags(INST_F_ATTACHED))
                SetState(RC_ST_RELEASE);
            if (riderDelta >= 0)
                speed = riderSpeed >= 0 ? riderSpeed : -riderSpeed;
            else
                speed = -(riderSpeed >= 0 ? riderSpeed : -riderSpeed);
            if (targets[0])
                targets[0]->HandleMessage(this, MSG_CARPET_DRIVE, &speed);
            if (targets[1])
                targets[1]->HandleMessage(this, MSG_CARPET_DRIVE, &speed);
            if (targets[2])
                targets[2]->HandleMessage(this, MSG_CARPET_DRIVE, &speed);
            if (targets[3])
                targets[3]->HandleMessage(this, MSG_CARPET_DRIVE, &speed);
            if (targets[4])
                targets[4]->HandleMessage(this, MSG_CARPET_DRIVE, &speed);
            if (rider->GetClassId() == CLASSID_WOLF)
                ScrollBelt(riderDelta / 3 >= 0 ? riderDelta / 3 : -(riderDelta / 3));
            else
                ScrollBelt(riderDelta >= 0 ? riderDelta : -riderDelta);
            break;
        case RC_ST_ROLLING_FAST:
            if ((riderSpeed >= 0 ? riderSpeed : -riderSpeed) < 700)
                SetState(RC_ST_ROLLING);
            timerMs -= g_dtMs;
            if (timerMs <= 0)
                SetState(RC_ST_RELEASE);
            /* cast kept: MSG_AXIS_LOCK's arg is the RollingCarpetAxis, a number in the void * */
            rider->HandleMessage(this, MSG_AXIS_LOCK, (void *)(u32)axis);
            if (riderDelta >= 0)
                speed = riderSpeed >= 0 ? riderSpeed : -riderSpeed;
            else
                speed = -(riderSpeed >= 0 ? riderSpeed : -riderSpeed);
            if (targets[0])
                targets[0]->HandleMessage(this, MSG_CARPET_DRIVE, &speed);
            if (targets[1])
                targets[1]->HandleMessage(this, MSG_CARPET_DRIVE, &speed);
            if (targets[2])
                targets[2]->HandleMessage(this, MSG_CARPET_DRIVE, &speed);
            if (targets[3])
                targets[3]->HandleMessage(this, MSG_CARPET_DRIVE, &speed);
            if (targets[4])
                targets[4]->HandleMessage(this, MSG_CARPET_DRIVE, &speed);
            if (rider->GetClassId() == CLASSID_WOLF)
                ScrollBelt(riderDelta / 3 >= 0 ? riderDelta / 3 : -(riderDelta / 3));
            else
                ScrollBelt(riderDelta >= 0 ? riderDelta : -riderDelta);
            break;
    }
    AdvanceAnim();
}

/* 0x4e68af */
/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets; pad fill gaps */
s32 RollingCarpet::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    struct {
        MoveModifyArg *move;
        Vec3s delta;
        u16 pad;
    } w;
    if (sender) {
        switch (msgId) {
            case MSG_MODIFY_MOVE:
                w.move = (MoveModifyArg *)arg; /* cast kept: MSG_MODIFY_MOVE's arg is a MoveModifyArg */
                switch (axis) {
                    case RC_AXIS_X:
                        riderDelta = w.move->delta.x;
                        if (riderDelta)
                            riderSpeed = w.move->velocity.x;
                        else
                            riderSpeed = 0;
                        break;
                    case RC_AXIS_Z:
                        riderDelta = w.move->delta.z;
                        if (riderDelta)
                            riderSpeed = w.move->velocity.z;
                        else
                            riderSpeed = 0;
                        break;
                }
                FaceRider();
                w.delta.x = center.x;
                w.delta.y = center.y;
                w.delta.z = center.z;
                w.delta.x -= rider->pos.x;
                w.delta.y -= rider->pos.y;
                w.delta.z -= rider->pos.z;
                w.move->delta.x = (w.delta.x >= 0 ? w.delta.x : -w.delta.x) <= 4 ? w.delta.x : (s16)(w.delta.x >> 2);
                w.move->delta.z = (w.delta.z >= 0 ? w.delta.z : -w.delta.z) <= 4 ? w.delta.z : (s16)(w.delta.z >> 2);
                switch (state) {
                    case RC_ST_RIDDEN:
                    case RC_ST_ROLLING:
                    case RC_ST_ROLLING_FAST:
                        if (w.move->delta.y < -10)
                            SetState(RC_ST_RELEASE);
                        break;
                }
                break;
            case MSG_GROUND_QUERY:
                if (collBox)
                    /* cast kept: MSG_GROUND_QUERY's arg is a GroundQuery */
                    return Box_GroundQueryFlatTop((GroundQuery *)arg, collBox, &pos, 10);
                break;
        }
    }
    return 0;
}

/* 0x4e6acb */
void RollingCarpet::SetState(u8 newState)
{
    switch (newState) {
        case RC_ST_REGISTER:
            rider->HandleMessage(this, MSG_RIDER_ADD, 0);
            rcFlags.registered = 1;
            break;
        case RC_ST_RELEASE:
            rider->HandleMessage(this, MSG_RIDER_REMOVE, 0);
            rcFlags.registered = 0;
            if (rcFlags.camera) {
                rcFlags.camera = 0;
                Camera_ReleaseScripted(this);
            }
            timerMs = 0x400;
            if (rider && rider->InstFlags(INST_F_ATTACHED))
                riderSpeed = 0;
            break;
        case RC_ST_IDLE:
            PlayAnim(ATAPIS01_ANIM_STAND, 0, 0);
            StopSound(motorSound);
            motorSound = 0;
            riderSpeed = 0;
            break;
        case RC_ST_RIDDEN:
            PlayAnim(ATAPIS01_ANIM_STAND, 0, 0);
            StopSound(motorSound);
            motorSound = 0;
            riderSpeed = 0;
            break;
        case RC_ST_ROLLING:
            if (rider->GetClassId() == CLASSID_WOLF)
                PlayAnim(ATAPIS01_ANIM_RUN, 1, 0);
            if (rider->GetClassId() == CLASSID_SHEEP)
                PlayAnim(ATAPIS01_ANIM_WALK, 1, 0);
            if (!IsSoundPlaying(motorSound))
                motorSound = Sound_Play(SND_CARPET_MOTOR, this, 0xff, SNDF_LOOP | SNDF_POSITIONAL, 0x1000);
            SetSoundRate(motorSound, 0x1000);
            break;
        case RC_ST_ROLLING_FAST:
            if (rider->GetClassId() == CLASSID_WOLF)
                PlayAnim(ATAPIS01_ANIM_RUNF, 1, 0);
            timerMs = 0x800;
            SetSoundRate(motorSound, 0x2000);
            break;
    }
    state = newState;
}

/* 0x4e6ea3 */
ScnObject *RollingCarpet_Create(void *record)
{
    RollingCarpet *obj = new RollingCarpet;
    obj = (RollingCarpet *)obj->Init(record, 0); /* cast kept: Init returns the object as a ScnBody * */
    obj->flags |= SCN_OF_CUSTOM_COLLIDE;
    obj->collBox = 0;
    g_rollingCarpetBeltReady = 0;
    return obj;
}

/* 0x4e6f35 */
/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets */
void RollingCarpet::InitBeltScroll(s8 step)
{
    struct {
        s32 result;
        Texture *source;
        RECT rect;
        u32 *entries;
        u16 pad, count;
        DavBitmapRec *rects, *rec;
    } w;
    w.rects = g_pDav->header->dir->bitmaps;
    w.entries = IdList_FindWithCount(DAV_IDI_ITPDESS_, &w.count);
    /* cast kept: an id list holds resource pointers as u32 words; this entry points at the u16 bitmap index */
    w.rec = &w.rects[*(u16 *)w.entries[0]];
    g_rollingCarpetBeltScroll.y = w.rec->v;
    g_rollingCarpetBeltScroll.h = w.rec->height;
    g_rollingCarpetBeltScroll.x = w.rec->u;
    g_rollingCarpetBeltScroll.w = w.rec->width;
    g_rollingCarpetBeltScroll.texPage = w.rec->page;
    g_rollingCarpetBeltScroll.srcY = 0;
    g_rollingCarpetBeltScroll.srcH = w.rec->height;
    g_rollingCarpetBeltScroll.srcX = 0;
    g_rollingCarpetBeltScroll.srcW = w.rec->width;
    g_rollingCarpetBeltScroll.offset = 0;
    g_rollingCarpetBeltScroll.step = step;
    w.source = g_pPolyBin->textures[g_rollingCarpetBeltScroll.texPage];
    g_rollingCarpetBeltBackup = new Texture(g_pD3DAppMain, g_rollingCarpetBeltScroll.w, g_rollingCarpetBeltScroll.h,
                                            w.source->GetFormat(), &w.result);
    w.rect.top = w.rec->v;
    w.rect.bottom = w.rec->v + w.rec->height;
    w.rect.left = w.rec->u;
    w.rect.right = w.rec->u + w.rec->width;
    g_rollingCarpetBeltBackup->GetSurface()->BltFast(0, 0, w.source->GetSurface(), &w.rect, 0);
}

/* 0x4e70ce. The callers pass a short; only its low signed byte is stored. */
/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets */
void RollingCarpet::ScrollBelt(s16 rows)
{
    struct {
        s32 topRow;
        Texture *tex;
        s32 startCol, startRow, width, rowEnd;
        u16 *backupPixels;
        DDSURFACEDESC2 *texDesc;
        s32 row;
        DDSURFACEDESC2 *backupDesc;
        s32 position;
        u16 *pixels;
    } w;
    w.tex = g_pPolyBin->textures[g_rollingCarpetBeltScroll.texPage];
    g_rollingCarpetBeltScroll.step = (s8)rows;
    g_rollingCarpetBeltScroll.offset = (s8)((g_rollingCarpetBeltScroll.offset + g_rollingCarpetBeltScroll.step +
                                             g_rollingCarpetBeltBackup->GetHeight()) %
                                            g_rollingCarpetBeltBackup->GetHeight());
    w.startRow = g_rollingCarpetBeltScroll.srcY;
    w.rowEnd = g_rollingCarpetBeltScroll.srcH;
    w.startCol = g_rollingCarpetBeltScroll.srcX;
    w.width = g_rollingCarpetBeltScroll.srcW;
    w.texDesc = new DDSURFACEDESC2;
    w.backupDesc = new DDSURFACEDESC2;
    w.tex->Surface_LockForWrite(w.texDesc);
    g_rollingCarpetBeltBackup->Surface_LockForRead(w.backupDesc);
    /* cast kept (these two lines): a locked surface is raw memory; these textures hold 16-bit pixels */
    w.pixels = (u16 *)w.texDesc->lpSurface;
    w.backupPixels = (u16 *)w.backupDesc->lpSurface;
    w.rowEnd -= g_rollingCarpetBeltScroll.offset;
    w.position = (g_rollingCarpetBeltScroll.y + g_rollingCarpetBeltScroll.offset) * w.tex->GetWidth() +
                 g_rollingCarpetBeltScroll.x;
    for (w.row = w.startRow; w.row < w.rowEnd; w.row++) {
        memcpy(w.pixels + w.position, w.backupPixels + w.row * g_rollingCarpetBeltBackup->GetWidth(),
               g_rollingCarpetBeltScroll.srcW * 2);
        w.position += w.tex->GetWidth();
    }
    if (g_rollingCarpetBeltScroll.offset != 0) {
        w.startRow = w.rowEnd;
        w.rowEnd = g_rollingCarpetBeltScroll.srcH;
        w.position = g_rollingCarpetBeltScroll.y * w.tex->GetWidth() + g_rollingCarpetBeltScroll.x;
        for (w.topRow = w.startRow; w.topRow < w.rowEnd; w.topRow++) {
            memcpy(w.pixels + w.position, w.backupPixels + w.topRow * g_rollingCarpetBeltBackup->GetWidth(),
                   g_rollingCarpetBeltScroll.srcW * 2);
            w.position += w.tex->GetWidth();
        }
    }
    w.tex->Surface_Unlock();
    g_rollingCarpetBeltBackup->Surface_Unlock();
    delete w.texDesc;
    delete w.backupDesc;
}
