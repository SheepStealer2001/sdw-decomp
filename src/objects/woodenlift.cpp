/*
 * WoodenLift (class 36, CLASSID 36 "Wooden Lift", vtable 0x57707c, sizeof 0xa8) - the lift that travels between its
 * start height and -MAXZVALUE. Msg 0x1f switches it one way, 0x20 the other; it carries whatever stands on it (and a
 * stack of FloatingBox / class 0x41 objects on top when it moves up), refuses to move down onto a bodyless world
 * object or when the 30-unit downward probe hits, and runs the level's scripted CAMERA while Ralph (or a driven
 * Robot) stands inside IDCAMERABOX above CAMERACHANGEZVALUE.
 *
 * SheepD3D.exe 0x509fc0-0x50b215: PostLoadInit, UpdateBoxes, IsDriverInBoxXZ, Reset, Update, Move, CarryObjects,
 * HandleMessage, the factory.
 * liftFlags (+0x5d) is three one-bit fields (switchedOn, camReleased, soundOn): every access is a byte bitfield
 * operation (and al,-2 at 0x50a12a; shr dl,2 / and dl,1 at 0x50a6f6).
 */
/* BYTES: slot-name. */
#define SDW_MEMBERS_ScnObject                                                  \
    static void *operator new(u32 size);                                       \
    void StartCamera(u16 rx, u16 ry, u16 rz, Vec3s *eye, u16 focal, u32 mode); \
    CollBox *GetFirstModelBox();                                               \
    void SetUpdateMode(s32 mode);
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
#include "../engine/sound_mgr.h"
#include "../engine/scn_tools.h"
#include "camera.h"
#include "../app/app_main.h"
#include "../engine/scenaric.h"
#include "sdw_classes.h"
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_GETFLAGS 1
#define SDW_INLINE_SCNOBJECT_ISINWORLD 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_GETFLAGS
#undef SDW_INLINE_SCNOBJECT_ISINWORLD
#define SDW_INLINE_SCNOBJECT_INSTFLAGS_U16 1
#define SDW_INLINE_SCNOBJECT_STOPSOUND_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_INSTFLAGS_U16
#undef SDW_INLINE_SCNOBJECT_STOPSOUND_U16
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32

u16 Sound_Play(u16 id, void *owner, u16 volume, u8 flags, s32 rate);                 /* 0x5491b8 */
s32 Scenaric_FindByClass(u16 classId, ScnObject **out, s32 max);                     /* 0x5145c5 */
s32 ObjGrid_QueryBoxOverlap(CollBox *box, ScnObject **out);                          /* 0x510f34 */
s32 Box_GroundQueryFlatTop(GroundQuery *q, CollBox *box, Vec3s *boxPos, s32 margin); /* 0x515934 */
void Camera_StartScripted(ScnObject *owner, Camera *cam, u16 rotX, u16 rotY, u16 rotZ, Vec3s *pos, u16 focal, u32 mode,
                          s32 time); /* 0x55a70d */
extern Wolf *g_pWolf;                /* 0x6cf310 */
extern s32 g_dt;                     /* 0x71b300 */

/* A designer property of the WAR record: the dword at record + 0x14 + offset. Inlined; the offset and the value are
 * stack temps (0x509fd9, 0x509fe9). */
#define SDW_INLINE_FREE_SCN_GETPROPS32_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_SCN_GETPROPS32_VOID_U32

/* Model box 0 (object-local), or NULL when the model has no box list. Inlined: the list and the result are stack
 * temps (0x50a20d, 0x50a21c). */
inline CollBox *ScnObject::GetFirstModelBox()
{
    ModelBoxList *list = inst_model->boxes;
    if (list)
        return (CollBox *)list->boxes; /* cast kept: Box and CollBox are two views of one 16-byte zone record */
    return 0;
}

#define SDW_INLINE_SCNOBJECT_STARTCAMERA_U16_U16_U16_VEC3S_U16_U32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_STARTCAMERA_U16_U16_U16_VEC3S_U16_U32

/* Whether p lies inside box in x and z (the vertical is not tested); p is a stack temp (0x50a596). */
#define SDW_INLINE_FREE_BOX_CONTAINSPOINTXZ_BOX_VEC3S 1
#include "../engine/coll_box_inlines.h"
#undef SDW_INLINE_FREE_BOX_CONTAINSPOINTXZ_BOX_VEC3S

#define SDW_INLINE_FREE_SCENARIC_CLASSFLAGS_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_FREE_SCENARIC_CLASSFLAGS_U16

#define ABS(x) ((x) >= 0 ? (x) : -(x))

/* 0x509fc0 - vtable +0x00: read the properties, find the Robot, and orient the travel so that the start end is
 * where the lift is now and the other end is -MAXZVALUE (dirOn points toward it). */
void WoodenLift::PostLoadInit()
{
    u16 *props = record;
    frozen = 0;
    topY = (s16)-Scn_GetPropS32(props, 0xc);
    speed = (u16)(Scn_GetPropS32(props, 0x10) * 30);
    cameraChangeY = (s16)-Scn_GetPropS32(props, 4);
    cameraBox = Scn_GetPropBox(props, 8);
    camera = Scn_GetPropCamera(props, 0);
    if (!Scenaric_FindByClass(CLASSID_ROBOT, &robot, 1))
        robot = 0;
    SetUpdateMode(SCN_UPD_ALWAYS);
    switchedOn = 0;
    if (pos.y < topY) {
        bottomY = topY;
        topY = pos.y;
        dir = 1;
        dirOn = 1;
        dirOff = -1;
    } else {
        bottomY = pos.y;
        dir = -1;
        dirOn = -1;
        dirOff = 1;
    }
    camReleased = 1;
    soundHandle = 0;
    soundOn = 0;
    UpdateBoxes();
}

/* 0x50a1e9 - rebuild the world-space ride box (model box 0 + pos, top raised 5) and under box (bottom lowered 50). */
void WoodenLift::UpdateBoxes()
{
    Vec3s grow;
    grow.x = 0;
    grow.y = -5;
    grow.z = 0;
    rideBox.min.x = GetFirstModelBox()->min.x + pos.x;
    rideBox.min.y = GetFirstModelBox()->min.y + pos.y;
    rideBox.min.z = GetFirstModelBox()->min.z + pos.z;
    rideBox.max.x = GetFirstModelBox()->max.x + pos.x;
    rideBox.max.y = GetFirstModelBox()->max.y + pos.y;
    rideBox.max.z = GetFirstModelBox()->max.z + pos.z;
    rideBox.min.x += grow.x;
    rideBox.min.y += grow.y;
    rideBox.min.z += grow.z;
    grow.y = 0x32;
    underBox.min.x = GetFirstModelBox()->min.x + pos.x;
    underBox.min.y = GetFirstModelBox()->min.y + pos.y;
    underBox.min.z = GetFirstModelBox()->min.z + pos.z;
    underBox.max.x = GetFirstModelBox()->max.x + pos.x;
    underBox.max.y = GetFirstModelBox()->max.y + pos.y;
    underBox.max.z = GetFirstModelBox()->max.z + pos.z;
    underBox.max.x += grow.x;
    underBox.max.y += grow.y;
    underBox.max.z += grow.z;
}

/* 0x50a54a - whoever drives: the Robot when it answers msg 0x3681, else Ralph; is he inside box in x/z? */
s32 WoodenLift::IsDriverInBoxXZ(Box *box)
{
    if (!box)
        return 0;
    if (robot && robot->HandleMessage(this, MSG_ROBOT_IS_DRIVEN, 0)) {
        if (Box_ContainsPointXZ(box, &robot->pos))
            return 1;
        return 0;
    }
    if (Box_ContainsPointXZ(box, &g_pWolf->pos))
        return 1;
    return 0;
}

/* 0x50a676 - vtable +0x14: stop the motor sound and rebuild the boxes (the position is NOT restored). */
void WoodenLift::Reset()
{
    soundOn = 0;
    StopSound(soundHandle);
    soundHandle = 0;
    UpdateBoxes();
}

/* 0x50a6bf - vtable +0x04: one movement step with the motor sound, then the scripted camera. */
void WoodenLift::Update()
{
    switch (frozen) {
        case 0:
            if (Move(dir)) {
                if (!soundOn) {
                    StopSound(soundHandle);
                    soundHandle = 0;
                    soundHandle = Sound_Play(SND_SPFASCEN, this, 0xff, SNDF_LOOP | SNDF_POSITIONAL, 0x1000);
                    soundOn = 1;
                }
            } else {
                StopSound(soundHandle);
                soundHandle = 0;
                soundOn = 0;
            }
            if (IsDriverInBoxXZ(cameraBox)) {
                if (switchedOn) {
                    if (pos.y < cameraChangeY) {
                        if (camera) {
                            if (camReleased)
                                StartCamera(camera->rot[0], camera->rot[1], camera->rot[2], &camera->eye, camera->focal,
                                            (CAMSCR_BLEND_IN | CAMSCR_BLEND_OUT));
                            camReleased = 0;
                        }
                    } else {
                        Camera_ReleaseScripted(this);
                        switchedOn = 0;
                    }
                } else if (!camReleased) {
                    Camera_ReleaseScripted(this);
                    camReleased = 1;
                }
            } else if (!camReleased) {
                Camera_ReleaseScripted(this);
                camReleased = 1;
            }
            break;
    }
}

/* 0x50a8f0 - one step toward the end dir points at; returns 1 when the lift moved. Going down (dir 1) it first
 * refuses if a bodyless world object is in the under box (a class 3 / 0x30 one is told msg 0x79), if the
 * 30-unit probe below hits something not attached, or if its own body would collide one step lower. */
s32 WoodenLift::Move(s16 dir)
{
    s32 blocked;
    s32 n;
    ScnObject *cur;
    s32 step;
    ScnObject *found[64];
    Vec3s delta;
    Vec3s probe;
    s16 end;
    Vec3s origin;
    s32 hits;

    blocked = 0;
    hits = ObjGrid_QueryBoxOverlap(&underBox, found);
    origin.x = pos.x;
    origin.y = pos.y;
    origin.z = pos.z;
    delta.x = 0;
    delta.z = 0;
    step = speed * g_dt / 4096;
    delta.y = (s16)(step * dir);
    if (dir == 1) {
        for (n = 0; n < hits; n++) {
            cur = found[n];
            if (!cur->GetFirstSolidBox() && !cur->InstFlags(INST_F_ATTACHED) && cur->IsInWorld() &&
                !(cur->GetFlags() & SCN_OF_HIDDEN)) {
                if (cur->GetClassId() == CLASSID_SALAD || cur->GetClassId() == CLASSID_BUSH)
                    cur->HandleMessage(this, MSG_LIFT_CRUSH, 0);
                blocked = 1;
            }
        }
        if (blocked)
            return 0;
    }
    if (dir == 1) {
        probeDelta.y = 0x1e;
        probeDelta.x = 0;
        probeDelta.z = 0;
        downProbeResult =
            Collide_ResolveMove(&probeDelta, &probeContact, 0xb54,
                                RESOLVE_SLIDE_ALL | RESOLVE_ASK_MOVER | RESOLVE_NO_STATIC, 0, 0, 10, 0, 0);
        if (probeContact.floorObj && probeContact.floorObj->InstFlags(INST_F_ATTACHED))
            downProbeResult = 0;
        if (downProbeResult)
            return 0;
        probe.x = origin.x + delta.x;
        probe.y = origin.y + delta.y;
        probe.z = origin.z + delta.z;
        if (TestBodyAt(&probe, CQ_OBJECTS))
            return 0;
        end = bottomY;
    } else {
        end = topY;
    }
    if (origin.y < topY || origin.y > bottomY) {
        delta.y = end - origin.y;
        if (CarryObjects(delta)) {
            Translate(&delta);
            UpdateBoxes();
        } else {
            return 0;
        }
    } else if (ABS(origin.y - end) > step) {
        if (CarryObjects(delta)) {
            Translate(&delta);
            UpdateBoxes();
        } else {
            return 0;
        }
    } else if (ABS(origin.y - end) != 0) {
        delta.y = (s16)(ABS(origin.y - end) * dir);
        if (CarryObjects(delta)) {
            Translate(&delta);
            UpdateBoxes();
        } else {
            return 0;
        }
    } else {
        StopSound(soundHandle);
        return 0;
    }
    return 1;
}

/* 0x50adec - move everything on the ride box by delta. Moving up, the first free FloatingBox (0x8f) or class 0x41
 * object on it first lifts whatever stands on IT (its box raised 5); then every unattached object except other
 * lifts is translated, and sheep-like classes (class flag 0x800) are told msg 0x2e {lift pos, radius 20}. */
/* BYTES(slot-name): names place the frame (tools/vc6_locals.py): hits, zone, jdx, args, crateBox, cargo, hitObj, count2, found2, i from EBP-4 down */
s32 WoodenLift::CarryObjects(Vec3s delta)
{
    /* The names place the frame (tools/vc6_locals.py): hits, zone, jdx, args, crateBox, cargo, hitObj, count2,
     * found2, i from EBP-4 down (the pointer arrays land 8-aligned). */
    s32 hits;
    s32 jdx;
    CollBox zone;
    WolfSpotArg args;
    ScnObject *crateBox;
    ScnObject *found2[64];
    s32 count2;
    ScnObject *hitObj;
    ScnObject *cargo[64];
    s32 i;

    hits = ObjGrid_QueryBoxOverlap(&rideBox, cargo);
    jdx = 0;
    count2 = 0;
    crateBox = 0;
    for (i = 0; i < hits && delta.y < 0; i++) {
        hitObj = cargo[i];
        if (!hitObj->InstFlags(INST_F_ATTACHED) &&
            (hitObj->GetClassId() == CLASSID_FLOATINGBOX || hitObj->GetClassId() == CLASSID_SMALLROCK)) {
            crateBox = hitObj;
            zone.min.x = crateBox->GetFirstSolidBox()->min.x + crateBox->pos.x;
            zone.min.y = crateBox->GetFirstSolidBox()->min.y + crateBox->pos.y;
            zone.min.z = crateBox->GetFirstSolidBox()->min.z + crateBox->pos.z;
            zone.max.x = crateBox->GetFirstSolidBox()->max.x + crateBox->pos.x;
            zone.max.y = crateBox->GetFirstSolidBox()->max.y + crateBox->pos.y;
            zone.max.z = crateBox->GetFirstSolidBox()->max.z + crateBox->pos.z;
            zone.min.y = zone.min.y - 5;
            count2 = ObjGrid_QueryBoxOverlap(&zone, found2);
            for (jdx = 0; jdx < count2; jdx++) {
                if (found2[jdx] != crateBox && found2[jdx] != this)
                    found2[jdx]->Translate(&delta);
            }
            break;
        }
    }
    for (i = 0; i < hits; i++) {
        hitObj = cargo[i];
        if (!hitObj->InstFlags(INST_F_ATTACHED) && hitObj->GetClassId() != CLASSID_WOODEN_LIFT) {
            hitObj->Translate(&delta);
            if (Scenaric_ClassFlags(hitObj->GetClassId()) & SCN_CF_SHEEP_ANCHORABLE) {
                args.pos.x = pos.x;
                args.pos.y = pos.y;
                args.pos.z = pos.z;
                args.radius = 0x14;
                hitObj->HandleMessage(this, MSG_SET_ANCHOR, &args);
            }
        }
    }
    return 1;
}

/* 0x50b109 - vtable +0x10: 0xd ground query on the flat top of model box 0 (margin 5); 0x1f switch on (dirOn);
 * 0x20 switch off (dirOff). */
s32 WoodenLift::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    switch (msgId) {
        case MSG_SWITCH_ON:
            switchedOn = 1;
            dir = dirOn;
            break;
        case MSG_SWITCH_OFF:
            switchedOn = 0;
            dir = dirOff;
            break;
        case MSG_GROUND_QUERY:
            /* cast kept: the message arg is a void *; this one carries a GroundQuery */
            return Box_GroundQueryFlatTop((GroundQuery *)arg, GetFirstModelBox(), &pos, 5);
    }
    return 0;
}

/* 0x50b1b1 - the class factory for CLASSID 36 "Wooden Lift": new WoodenLift (the base vtables in turn, then
 * WoodenLift's), then ScnLogic::Init(record) through the vtable. */
ScnObject *WoodenLift_Create(void *record)
{
    WoodenLift *obj = new WoodenLift;
    obj = (WoodenLift *)obj->Init(record); /* cast kept: Init returns the object as its base class */
    return obj;
}
