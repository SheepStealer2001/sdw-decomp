/* T125 - original object Catapult.cpp (guessed name).
 * Ranges: .text 0x4a8a40-0x4ab38a, .rdata 0x575d28-0x575d50 (the __real@3c23d70a COMDAT this object emits first, then
 * the vtable), .bss 0x6cf5f4-0x6cf5fc (g_catapultLiftDelta, defined here).
 * The functions are in the original address order. */
/* BYTES: slot-group, slot-name. */
/* BYTES(slot-name, inferred): local names chosen for their stack slots (tools/vc6_locals.py), not recovered */
/* Collide_BoxVsObjBox is declared extern "C", as it is defined (src/engine/collide.cpp), so the decorated names agree at link. */
/* PAL PC Catapult, 0x4a8a40-0x4ab38a. */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
struct GroundQuery;
#include "../engine/sound_mgr.h"
#include "camera.h"
#include "../engine/collide.h"
#include "../app/app_main.h"
#include "../engine/fixed_math.h"
#include "../engine/load_warmeshes.h"
#include "../engine/scenaric.h"
#include "../engine/input.h"
#include "../engine/screen.h"
#include "../engine/interface.h"
#include "../engine/id_list.h"
#include "../engine/draw2d.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *inst, Animator *animator, u16 id, u32 options);

#define SDW_MEMBERS_ScnObject            \
    static void *operator new(u32 size); \
    CollBox *GetFirstModelBox();         \
    CollBox *GetModelBoxes(u32 *count);  \
    void SetUpdateMode(u8 mode);
#define SDW_MEMBERS_Catapult void SetBucketRequested(s32 enabled);
#define SDW_MEMBERS_ScnBody                                                                                    \
    AnimJointPose *GetJointPose(u8 index)                                                                      \
    {                                                                                                          \
        return (AnimJointPose *)anim.bufC + index; /* cast kept: Animator's pose buffers are untyped blocks */ \
    }

#define SDW_MEMBERS_CollBox s32 ContainsXZ(const Vec3s *point);


#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_INSTFLAGS_U16 1
#define SDW_INLINE_SCNOBJECT_STOPSOUND_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_INSTFLAGS_U16
#undef SDW_INLINE_SCNOBJECT_STOPSOUND_U16
#define SDW_INLINE_SCNOBJECT_SETPARTHEIGHT_U32 1
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETPARTHEIGHT_U32
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_COLLBOX_CONTAINSXZ_CONST_VEC3S 1
#include "../engine/coll_box_inlines.h"
#undef SDW_INLINE_COLLBOX_CONTAINSXZ_CONST_VEC3S
#define SDW_INLINE_UIQUAD_SETCOLOR_U32 1
#include "../engine/ui_quad_inlines.h"
#undef SDW_INLINE_UIQUAD_SETCOLOR_U32
inline void Catapult::SetBucketRequested(s32 enabled)
{
    catFlags.bucketRequested = enabled;
}
s32 ObjGrid_QueryBoxOverlap(CollBox *box, ScnObject **out);
u16 Sound_Play(u16 id, void *owner, u16 volume, u8 flags, s32 rate);
extern "C" s16 g_sinTable4096[5122];
extern "C" const s16 *g_pCosTable;
extern "C" s16 Math_RadiansToAngle4096(float angle);
extern "C" double sqrt(double value), atan2(double y, double x);
void Camera_StartScripted(ScnObject *owner, Camera *cam, u16 pitch, u16 yaw, u16 roll, Vec3s *pos, u16 focal, u32 mode,
                          s32 time);
Vec3s g_catapultLiftDelta; /* 0x6cf5f4 .bss, used only by ScanBucket */

extern u32 *g_screenLayerBase, *g_screenLayerBase0;
extern u32 g_uiTintColor;
extern Wolf *g_pWolf;
extern s32 g_dtMs;
s32 Scenaric_FindByClass(u16 id, ScnObject **out, s32 maximum);
#define SDW_INLINE_FREE_SCENARIC_CLASSFLAGS_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_FREE_SCENARIC_CLASSFLAGS_U16
inline CollBox *ScnObject::GetFirstModelBox()
{
    ModelBoxList *list = inst_model->boxes;
    if (list)
        return (CollBox *)list->boxes; /* cast kept: Box and CollBox are two views of one 16-byte zone record */
    return 0;
}
#define SDW_INLINE_SCNOBJECT_GETMODELBOXES_U32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETMODELBOXES_U32
#define SDW_INLINE_FREE_READPROPERTY_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_READPROPERTY_VOID_U32
#define SDW_INLINE_LAUNCHARC_INITCOEFFICIENTS_S32_S32_S32_S32_S32_S32_S32_S32_S32 1
#define SDW_INLINE_LAUNCHARC_STEP_S32_S16_S16_S16 1
#include "../engine/launch_arc_inlines.h"
#undef SDW_INLINE_LAUNCHARC_INITCOEFFICIENTS_S32_S32_S32_S32_S32_S32_S32_S32_S32
#undef SDW_INLINE_LAUNCHARC_STEP_S32_S16_S16_S16
#define ABS_VALUE(v) ((v) >= 0 ? (v) : -(v))
void Catapult::PostLoadInit()
{
    s32 differenceZ, deltaX, span;
    u32 *ids, boxIndex, propertyId;
    u16 number;
    CollBox *modelBoxes;
    void *recordData;
    recordData = record;
    SetState(CAT_ST_IDLE);
    armAngle = 0;
    power = 0;
    bucketCount = 0;
    catFlags.reserved0 = 0;
    arc.obj = 0;
    camHoldMs = 0;
    catFlags.sideways = (ABS_VALUE(rot.y - 0xc00) < 100 || ABS_VALUE(rot.y - 0x400) < 100);
    catFlags.reversed = (rot.y > 0x39c && rot.y < 0x864);
    catFlags.bucketRequested = 1;
    catFlags.bucketArmed = 1;
    powerSound = 0;
    if (catFlags.sideways)
        PlayAnim(ACATAP01_ANIM_STAND1, 1, 0);
    else
        PlayAnim(ACATAP01_ANIM_STAND1, 1, 0);
    SetPartHeight(0xec);
    propertyId = ReadProperty(recordData, 0x18);
    ids = Scn_FindIdList((u16)propertyId, &number);
    /* cast kept (the four id-list reads below): an export id list holds record pointers of any kind */
    if (number == 1)
        mobileCamera = (CamSetup *)*ids;
    else
        mobileCamera = 0;
    offset0 = (s16)ReadProperty(recordData, 0x20);
    h0 = (s16)ReadProperty(recordData, 0x10);
    armLength1 = (s16)ReadProperty(recordData, 4);
    armLength2 = (s16)ReadProperty(recordData, 8);
    armLength3 = (s16)ReadProperty(recordData, 12);
    propertyId = ReadProperty(recordData, 0);
    ids = Scn_FindIdList((u16)propertyId, &number);
    activationBox = 0;
    if (number == 1)
        /* cast kept: the id list holds this box */
        activationBox = (Box *)*ids;
    maxDist = (s16)ReadProperty(recordData, 0x14);
    propertyId = ReadProperty(recordData, 0x24);
    ids = Scn_FindIdList((u16)propertyId, &number);
    if (number == 1) {
        /* cast kept: an export id list holds its records' pointers as u32 words */
        trajectory = (Trajectory *)*ids;
        if (trajectory->count != 3)
            trajectory = 0;
        else {
            deltaX = trajectory->pts[2].x - trajectory->pts[0].x;
            differenceZ = trajectory->pts[2].z - trajectory->pts[0].z;
            span = (s32)sqrt((double)deltaX * deltaX + (double)(differenceZ * differenceZ));
            maxThrowOffset.x = deltaX * maxDist / span;
            maxThrowOffset.z = differenceZ * maxDist / span;
            maxThrowOffset.y = 0;
        }
    } else
        trajectory = 0;
    modelBoxes = GetModelBoxes(&boxIndex);
    while (boxIndex) {
        --boxIndex;
        if (modelBoxes[boxIndex].flags & COLLBOX_NONSOLID) {
            bucketExtent.x = modelBoxes[boxIndex].max.x - modelBoxes[boxIndex].min.x;
            bucketExtent.y = modelBoxes[boxIndex].max.y - modelBoxes[boxIndex].min.y;
            bucketExtent.z = modelBoxes[boxIndex].max.z - modelBoxes[boxIndex].min.z;
            if (catFlags.sideways) {
                bucketExtent.x /= 2;
                bucketExtent.y /= 2;
            } else {
                bucketExtent.y /= 2;
                bucketExtent.z /= 2;
            }
            ++boxIndex;
            break;
        }
    }
    ids = IdList_FindWithCount(DAV_IDI_ICTJAUG1, &number);
    if (ids) {
        /* cast kept: an export id list entry points at this resource */
        powerGauge.UiQuad_SetFromBitmap((u16 *)*ids, 8, 16, 0, 0, 1024, 1024);
        powerGauge.SetColor(0x808080);
    }
    UpdateArmAngle();
    UpdateBucketBox();
    bucketBox.flags = 0;
    wolfFrozen = 0;
}
void Catapult::UpdateBucketArmed()
{
    ScnObject *objects[64];
    s32 count, index;
    if (catFlags.bucketRequested && !catFlags.bucketArmed) {
        count = ObjGrid_QueryBoxOverlap(&bucketBox, objects);
        for (index = 0; index < count; ++index)
            if (objects[index] != this)
                return;
    }
    catFlags.bucketArmed = catFlags.bucketRequested;
}
void Catapult::UpdateArmAngle()
{
    armAngle = 0xe60 + (power * 0x140 >> 10);
}
void Catapult::SetArmPose(s16 angle)
{
    AnimJointPose *joint;
    joint = GetJointPose(7);
    joint->rot[2] = Math_Angle4096ToRadians_2(-(0x122 - (angle >> 3)));
    joint->pos[2] = 0;
    joint->pos[1] = 0;
    joint->pos[0] = 0;
    joint->rot[0] = 0;
    joint->rot[1] = 0;
    joint->scale[2] = Math_U16ToUnitFloat(1024);
    joint->scale[1] = joint->scale[2];
    joint->scale[0] = joint->scale[1];
    joint->channels = 0x100;
    joint = GetJointPose(8);
    joint->rot[2] = Math_Angle4096ToRadians_2(-(0x1f5 - ((angle >> 3) * 8) / 10));
    joint->pos[2] = 0;
    joint->pos[1] = 0;
    joint->pos[0] = 0;
    joint->rot[0] = 0;
    joint->rot[1] = 0;
    joint->scale[2] = Math_U16ToUnitFloat(1024);
    joint->scale[1] = joint->scale[2];
    joint->scale[0] = joint->scale[1];
    joint->channels = 0x100;
    joint = GetJointPose(9);
    joint->rot[2] = Math_Angle4096ToRadians_2(-(0x1c4 - ((angle >> 3) * 64) / 100));
    joint->pos[2] = 0;
    joint->pos[1] = 0;
    joint->pos[0] = 0;
    joint->rot[0] = 0;
    joint->rot[1] = 0;
    joint->scale[2] = Math_U16ToUnitFloat(1024);
    joint->scale[1] = joint->scale[2];
    joint->scale[0] = joint->scale[1];
    joint->channels = 0x100;
    joint = GetJointPose(6);
    joint->rot[2] = Math_Angle4096ToRadians_2((angle << 4) & 0xfff);
    joint->pos[2] = 0;
    joint->pos[1] = 0;
    joint->pos[0] = 0;
    joint->rot[0] = 0;
    joint->rot[1] = 0;
    joint->scale[2] = Math_U16ToUnitFloat(1024);
    joint->scale[1] = joint->scale[2];
    joint->scale[0] = joint->scale[1];
    joint->channels = 0x100;
}
void Catapult::SetState(u8 value)
{
    state = value;
    stateJustEntered = 1;
    switch (value) {
        case CAT_ST_THROW:
            Sound_Play(SND_CATAPULT_FIRE, this, 255, SNDF_NO_RETRIGGER, 4096);
        case CAT_ST_ARM_RETURN:
        case CAT_ST_LAUNCH_PENDING:
            SetUpdateMode(SCN_UPD_ALWAYS);
            break;
        default:
            SetUpdateMode(SCN_UPD_NORMAL);
            break;
    }
}
/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets */
void Catapult::UpdateBucketBox()
{
    struct Work {
        Vec3s point;
        s16 unused, a3, a2, a1, horizontal;
    } w;
    w.a1 = (0x400 - (armAngle >> 3)) & 0xfff;
    w.a2 = (0x400 - (armAngle >> 3) * 18 / 10) & 0xfff;
    w.a3 = (0x400 - (armAngle >> 3) * 244 / 100) & 0xfff;
    w.point.x = pos.x;
    w.point.y =
        pos.y - h0 -
        ((armLength1 * g_sinTable4096[w.a1] + armLength2 * g_sinTable4096[w.a2] + armLength3 * g_sinTable4096[w.a3]) >>
         12);
    w.point.z = pos.z;
    w.horizontal =
        offset0 +
        ((armLength1 * g_pCosTable[w.a1] + armLength2 * g_pCosTable[w.a2] + armLength3 * g_pCosTable[w.a3]) >> 12);
    if (!catFlags.reversed) {
        bucketBox.min.y = w.point.y - bucketExtent.y;
        if (catFlags.sideways) {
            w.point.z += w.horizontal;
            bucketBox.min.z = w.point.z;
            bucketBox.min.x = w.point.x - bucketExtent.x;
        } else {
            w.point.x += w.horizontal;
            bucketBox.min.x = w.point.x;
            bucketBox.min.z = w.point.z - bucketExtent.z;
        }
        bucketBox.max.x = w.point.x + bucketExtent.x;
        bucketBox.max.y = w.point.y + bucketExtent.y;
        bucketBox.max.z = w.point.z + bucketExtent.z;
    } else {
        bucketBox.max.y = w.point.y - bucketExtent.y;
        if (catFlags.sideways) {
            w.point.z -= w.horizontal;
            bucketBox.max.z = w.point.z;
            bucketBox.max.x = w.point.x + bucketExtent.x;
        } else {
            w.point.x -= w.horizontal;
            bucketBox.max.x = w.point.x;
            bucketBox.max.z = w.point.z + bucketExtent.z;
        }
        bucketBox.min.x = w.point.x - bucketExtent.x;
        bucketBox.min.y = w.point.y - bucketExtent.y;
        bucketBox.min.z = w.point.z - bucketExtent.z;
    }
}
/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets; unused fill gaps */
s32 Catapult::FindFreeLandingSpot(CollBox *box, Vec3s *target, s16 maxShift)
{
    struct Work {
        ScnObject *objects[64];
        u32 unused;
        CollBox expanded;
        s32 blocked;
        CollBox query;
    } w;
    s32 index;
    if (!box)
        return 1;
    w.expanded.min.x = box->min.x - 10;
    w.expanded.min.y = box->min.y - 10;
    w.expanded.min.z = box->min.z - 10;
    w.expanded.max.x = box->max.x + 10;
    w.expanded.max.y = box->max.y + 10;
    w.expanded.max.z = box->max.z + 10;
    do {
        w.query.min.x = w.expanded.min.x + target->x;
        w.query.min.y = w.expanded.min.y + target->y;
        w.query.min.z = w.expanded.min.z + target->z;
        w.query.max.x = w.expanded.max.x + target->x;
        w.query.max.y = w.expanded.max.y + target->y;
        w.query.max.z = w.expanded.max.z + target->z;
        index = ObjGrid_QueryBoxesInRectXZ(w.query.min.x, w.query.min.z, w.query.max.x, w.query.max.z, w.objects);
        if (!index)
            return 1;
        w.blocked = 0;
        while (index--) {
            if (w.objects[index]->GetClassId() == CLASSID_CROCODILELEVEL09)
                return 1;
            if (w.objects[index]->GetClassId() == CLASSID_MISCSTATIC)
                continue;
            if (w.objects[index]->GetFirstSolidBox()) {
                w.blocked = 1;
                break;
            }
        }
        if (!w.blocked)
            return 1;
        if (catFlags.sideways)
            target->x += 50;
        else
            target->z += 50;
        maxShift -= 50;
    } while (maxShift >= 0);
    return 0;
}
/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets */
void Catapult::ScanBucket(s32 seat)
{
    CollBox *collisionBox;
    ScnObject *item;
    struct Work {
        Vec3s center;
        u16 angle;
        s32 count, bottom;
        CollBox query;
        ScnObject *objects[64];
    } w;
    w.query.max = bucketBox.max;
    w.query.min.x = bucketBox.min.x;
    w.query.min.y = bucketBox.min.y - 30;
    w.query.min.z = bucketBox.min.z;
    bucketCount = 0;
    w.count = ObjGrid_QueryBoxOverlap(&w.query, w.objects);
    while (w.count) {
        --w.count;
        item = w.objects[w.count];
        collisionBox = item->GetFirstModelBox();
        if (item->InstFlags(INST_F_ATTACHED))
            continue;
        if (collisionBox) {
            w.bottom = collisionBox->max.y + item->pos.y;
            if (w.bottom > w.query.max.y || w.bottom < w.query.min.y)
                continue;
            if (item == this)
                continue;
        } else
            w.bottom = item->pos.y;
        bucketObjs[bucketCount] = item;
        ++bucketCount;
        if (seat) {
            g_catapultLiftDelta.y = bucketBox.min.y - w.bottom - 1;
            if (g_catapultLiftDelta.y < 0 || (Scenaric_ClassFlags(item->GetClassId()) & SCN_CF_SNAP_TO_SUPPORT))
                item->Translate(&g_catapultLiftDelta);
            if (Scenaric_ClassFlags(item->GetClassId()) & SCN_CF_SHEEP_ANCHORABLE) {
                w.center.x = (w.query.min.x + w.query.max.x) >> 1;
                w.center.y = item->pos.y;
                w.center.z = (w.query.min.z + w.query.max.z) >> 1;
                w.angle = 15;
                item->HandleMessage(this, MSG_SET_ANCHOR, &w.center);
            }
        }
    }
}
s32 Catapult::CustomCollide(ScnObject *querier, CollBox *mover, Vec3s *disp, s32 *outFrac, s32 *outY,
                            CollContact *contacts, s32 *nContacts, u32 mode)
{
    Vec3s zero;
    zero.x = 0;
    zero.y = 0;
    zero.z = 0;
    if (!catFlags.bucketArmed)
        return 0;
    return Collide_BoxVsObjBox(this, mover, disp, &bucketBox, &zero, outFrac, outY, contacts, nContacts);
}
/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets; unused2 fill gaps */
void Catapult::TrackCamera(Vec3s *target, CamSetup *setup, u32 mode)
{
    struct Work {
        u16 unused, focal;
        s32 xx, yy, zz;
        Vec3s angles;
        u16 unused2;
        s32 x, y, z;
    } w;
    w.x = target->x - (&setup->eye)->x;
    w.y = target->y - (&setup->eye)->y;
    w.z = target->z - (&setup->eye)->z;
    w.xx = w.x * w.x;
    w.yy = w.y * w.y;
    w.zz = w.z * w.z;
    w.angles.x =
        Math_RadiansToAngle4096((float)atan2((double)w.y, (double)(s32)sqrt((double)w.xx + (double)w.zz))) & 0xfff;
    w.angles.y = Math_RadiansToAngle4096((float)atan2((double)-w.x, (double)w.z)) & 0xfff;
    w.angles.z = 0;
    w.focal = setup->focal;
    Camera_StartScripted(this, &g_camera, w.angles.x, w.angles.y, w.angles.z, &setup->eye, w.focal, mode, 4096);
}
/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets; unused, unused2 fill gaps */
s32 Catapult::SweepThrown(ScnObject *object, Vec3s *delta)
{
    struct Work {
        s32 index, count;
        Vec3s parts[3];
        u16 unused;
        Vec3s position;
        u16 unused2;
        ContactInfo contact;
    } w;
    w.count = 0;
    w.position = object->pos;
    w.parts[0].x = delta->x / 3;
    w.parts[0].y = delta->y / 3;
    w.parts[0].z = delta->z / 3;
    w.parts[1] = w.parts[0];
    w.parts[2].x = delta->x - (w.parts[0].x + w.parts[1].x);
    w.parts[2].y = delta->y - (w.parts[0].y + w.parts[1].y);
    w.parts[2].z = delta->z - (w.parts[0].z + w.parts[1].z);
    for (w.index = 0; w.index < 3; ++w.index) {
        w.count += object->Collide_ResolveMove(&w.parts[w.index], &w.contact, 0xb54, RESOLVE_SLIDE_ALL | RESOLVE_KEEP_Y,
                                               &w.position, 0, 10, 0, 0);
        w.position.x += w.parts[w.index].x;
        w.position.y += w.parts[w.index].y;
        w.position.z += w.parts[w.index].z;
    }
    delta->x = w.parts[0].x + w.parts[1].x + w.parts[2].x;
    delta->y = w.parts[0].y + w.parts[1].y + w.parts[2].y;
    delta->z = w.parts[0].z + w.parts[1].z + w.parts[2].z;
    return w.count;
}
void Catapult::Update()
{
    s16 step;
    Vec3s delta;
    s16 z, y, x, angle;
    ScnObject *sam;
    if (camHoldMs > 0) {
        camHoldMs -= (s16)g_dtMs;
        if (camHoldMs <= 0)
            Camera_ReleaseScripted(this);
    }
    UpdateBucketArmed();
    switch (state) {
        case CAT_ST_IDLE:
            ScanBucket(1);
            AdvanceAnim();
            SetArmPose(armAngle);
            stateJustEntered = 0;
            break;
        case CAT_ST_OPERATED:
            UpdatePowerInput();
            DrawPowerGauge();
            UpdateBucketBox();
            ScanBucket(1);
            if (!(g_pad.cur.buttons & ~g_inputMap[INPUT_SLOT_CROSS]) &&
                (g_pad.prev.buttons & ~g_inputMap[INPUT_SLOT_CROSS]) && !stateJustEntered) {
                if (wolfFrozen) {
                    g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0);
                    wolfFrozen = 0;
                }
                if (powerSound) {
                    StopSound(powerSound);
                    powerSound = 0;
                }
                SetState(CAT_ST_IDLE);
                break;
            }
            stateJustEntered = 0;
            AdvanceAnim();
            SetArmPose(armAngle);
            break;
        case CAT_ST_LAUNCH_PENDING:
            if (mobileCamera && arc.obj)
                TrackCamera(&arc.obj->pos, mobileCamera, 0);
            launchDelayMs -= (s16)g_dtMs;
            if (launchDelayMs <= 0) {
                ScanBucket(0);
                if (Launch()) {
                    if (Scenaric_FindByClass(CLASSID_SAM, &sam, 1))
                        sam->HandleMessage(this, MSG_SAM_FETCH_NOTIFY, arc.obj);
                    SetState(CAT_ST_THROW);
                    SetBucketRequested(0);
                } else {
                    Camera_ReleaseScripted(this);
                    SetState(CAT_ST_IDLE);
                    if (wolfFrozen) {
                        g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0);
                        wolfFrozen = 0;
                    }
                }
            }
            break;
        case CAT_ST_THROW:
            stateJustEntered = 0;
            armSwingT += (s16)(g_dtMs >> 3);
            if (armSwingT > 256)
                armSwingT = 256;
            angle = (g_pCosTable[(s16)(armSwingT * 0x44 & 0xfff)] * armAngle) / (armSwingT * 128 + 4096);
            AdvanceAnim();
            SetArmPose(angle);
            if (arc.obj) {
                if (arc.obj->InstFlags(INST_F_ATTACHED)) {
                    Camera_ReleaseScripted(this);
                    arc.obj = 0;
                    SetState(CAT_ST_ARM_RETURN);
                    break;
                }
                if (mobileCamera)
                    TrackCamera(&arc.obj->pos, mobileCamera, 0);
                step = g_dtMs >> 3;
                if (arc.t < 128)
                    step = (arc.t * step + (128 - arc.t) * (step * 4)) >> 7;
                if (arc.Step(step, &x, &y, &z)) {
                    delta.x = x - thrownLastPos.x;
                    delta.y = y - thrownLastPos.y;
                    delta.z = z - thrownLastPos.z;
                    if ((arc.t < 128 && arc.t > 0) || !SweepThrown(arc.obj, &delta))
                        arc.obj->Translate(&delta);
                    else {
                        /* cast kept: the arg is a number (1: hit something) */
                        arc.obj->HandleMessage(this, MSG_LANDED, (void *)1);
                        camHoldMs = 2000;
                        arc.obj = 0;
                        break;
                    }
                } else {
                    arc.obj->HandleMessage(this, MSG_LANDED, 0);
                    camHoldMs = 2000;
                    arc.obj = 0;
                    break;
                }
                thrownLastPos = arc.obj->pos;
            } else if (armSwingT == 256) {
                SetState(CAT_ST_ARM_RETURN);
                camHoldMs = 2000;
                armSwingT = 0;
            }
            break;
        case CAT_ST_ARM_RETURN:
            stateJustEntered = 0;
            armSwingT += (s16)(g_dtMs >> 2);
            if (armSwingT > 256)
                armSwingT = 256;
            AdvanceAnim();
            SetArmPose(armAngle * armSwingT >> 8);
            if (armSwingT == 256) {
                if (wolfFrozen) {
                    g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0);
                    wolfFrozen = 0;
                }
                SetState(CAT_ST_IDLE);
                SetBucketRequested(1);
            }
            break;
    }
}
s32 Catapult::HandleMessage(ScnObject *sender, u32 message, void *arg)
{
    switch (message) {
        case MSG_QUERY_ACTION:
            if (sender->GetClassId() == CLASSID_WOLF && activationBox)
                /* cast kept: Box and CollBox are two views of one 16-byte zone record */
                return state == CAT_ST_IDLE && ((CollBox *)activationBox)->ContainsXZ(&g_pWolf->pos) ? CTX_CATAPULT : 0;
            break;
        case MSG_USE:
            if (sender->GetClassId() == CLASSID_WOLF) {
                /* cast kept: as in MSG_QUERY_ACTION */
                if (((CollBox *)activationBox)->ContainsXZ(&g_pWolf->pos) && state == CAT_ST_IDLE) {
                    if (!wolfFrozen) {
                        g_pWolf->HandleMessage(this, MSG_FREEZE, 0);
                        wolfFrozen = 1;
                    }
                    SetState(CAT_ST_OPERATED);
                }
            }
            return 1;
        case MSG_FREEZE:
            wolfFrozen = 0;
            return 1;
        case MSG_SWITCH_OFF:
            catFlags.reserved0 = 0;
            break;
        case MSG_GROUND_QUERY:
            /* cast kept: the message arg is a void *; this one carries a GroundQuery */
            return GroundQuery((::GroundQuery *)arg);
        case MSG_SWITCH_ON:
            if (!catFlags.reserved0 && state == CAT_ST_IDLE)
                Launch();
            catFlags.reserved0 = 1;
        case MSG_QUERY_IDLE:
            return state == CAT_ST_IDLE;
    }
    return 0;
}
/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets */
s32 Catapult::Launch()
{
    struct Work {
        s16 unused, dz, dx, deltaPower;
    } w;
    arc.obj = 0;
    if (bucketCount > 1)
        return 0;
    SetState(CAT_ST_LAUNCH_PENDING);
    launchDelayMs = 1;
    AdvanceAnim();
    SetArmPose(armAngle);
    armSwingT = 0;
    if (bucketCount) {
        if (!wolfFrozen) {
            g_pWolf->HandleMessage(this, MSG_FREEZE, 0);
            wolfFrozen = 1;
        }
        w.deltaPower = power - 512;
        w.dx = maxThrowOffset.x * w.deltaPower >> 9;
        w.dz = maxThrowOffset.z * w.deltaPower >> 9;
        landingTarget = trajectory->pts[2];
        landingTarget.x += w.dx;
        landingTarget.z += w.dz;
        FindFreeLandingSpot(bucketObjs[0]->GetFirstModelBox(), &landingTarget, 1000);
        arc.obj = bucketObjs[0];
        arc.t = 0;
        arc.camera = 0;
        arc.camParam = 0;
        arc.flags.noFreeze = 0;
        arc.InitCoefficients(arc.obj->pos.x, trajectory->pts[1].x + w.dx, landingTarget.x, arc.obj->pos.y,
                             trajectory->pts[1].y, trajectory->pts[2].y, arc.obj->pos.z, trajectory->pts[1].z + w.dz,
                             landingTarget.z);
        thrownLastPos = arc.obj->pos;
        SetUpdateMode(SCN_UPD_ALWAYS);
    }
    return 1;
}
void Catapult::Reset()
{
    camHoldMs = 0;
    powerSound = 0;
    Camera_ReleaseScripted(this);
    if (state == CAT_ST_OPERATED)
        SetState(CAT_ST_IDLE);
    SetBucketRequested(1);
}
/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets */
void Catapult::UpdatePowerInput()
{
    struct Work {
        s32 x, y;
    } w;
    w.x = w.y = 0;
    if (g_pad.cur.typeLen.type == PADTYPE_ANALOG)
        Pad_StickToDeadzonedAxes(g_pad.cur.leftX, g_pad.cur.leftY, &w.x, &w.y);
    if (!(w.x | w.y)) {
        if (!(g_pad.cur.buttons & ~(u16)~PAD_RIGHT))
            w.x = 256;
        else if (!(g_pad.cur.buttons & ~(u16)~PAD_LEFT))
            w.x = -256;
    }
    if (w.x) {
        if (!powerSound)
            powerSound = Sound_Play(SND_CATAPULT_POWER, this, 255, SNDF_LOOP | SNDF_NO_RETRIGGER, 4096);
    } else if (powerSound) {
        StopSound(powerSound);
        powerSound = 0;
    }
    power += (s16)(w.x * 6 / 256);
    if (power < 0)
        power = 0;
    else if (power > 1024)
        power = 1024;
    UpdateArmAngle();
}
void Catapult::DrawPowerGauge()
{
    s16 x, y;
    x = (power * 84 >> 10) + 24;
    y = 39;
    Draw2D_FlatTri(g_screen.Draw2D_LayerToZ(g_screenLayerBase + 7) - 0.01f, g_screen.ScaleX(x), g_screen.ScaleY(y),
                   g_screen.ScaleX(x + 4), g_screen.ScaleY(y + 10), g_screen.ScaleX(x - 4), g_screen.ScaleY(y + 10), 0,
                   0);
    powerGauge.UiQuad_Draw(11);
    g_spriteCrayon2.Draw(g_screenLayerBase0 + 1, 8, 16, (s16)(powerGauge.w + 16), (s16)(powerGauge.h + 16),
                         g_uiTintColor, 0);
}
s32 Catapult::GroundQuery(::GroundQuery *query)
{
    CollBox *box = &bucketBox;
    if (box->ContainsXZ(&query->pos) && query->pos.y <= bucketBox.min.y) {
        query->pos.y = bucketBox.min.y;
        query->normal.y = -4096;
        query->normal.x = 0;
        query->normal.z = 0;
        return 1;
    }
    return 0;
}
ScnObject *Catapult_Create(u16 *record)
{
    Catapult *object = new Catapult;
    object = (Catapult *)object->Init(record, 0); /* cast kept: Init returns the object as its base class */
    object->flags |= SCN_OF_CUSTOM_COLLIDE;
    return object;
}
