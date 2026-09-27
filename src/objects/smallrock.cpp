/*
 * T217 - original object SmallRock.cpp (guessed name), one translation unit.
 *   .text  0x4f3be0-0x4f5767 (SmallRock_ApplyGravity .. SmallRock_CustomCollide)
 *   .rdata 0x576c88-0x576cb4 (g_smallRockCarryOffset + 2 pad, then ??_7SmallRock)
 *   .data  0x57b764-0x57b770 (this TU's copy of the Cine header's 9-byte opcode stride table, + 3 pad)
 * The carry offset is const (it is in .rdata); an inline const overload of ScnObject::AttachTo passes it on.
 */
/* BYTES: dead-code, layout, slot-name, view. */
/* BYTES(layout): the Cine.h header static: every object including the cinematic header carries this copy in its .data, referenced or not */
/* BYTES(view): ScnObject::AttachTo const overload (inline): source-only const overload: the offset is const (.rdata) and this forwards to the one decorated AttachTo */

#define SDW_MEMBERS_ScnObject                                                                       \
    static void *operator new(u32 size);                                                            \
    void AttachTo(ScnObject *parent, u8 joint, Vec3s *offset, Vec3s *rotation, u32 a, u32 b);       \
    void AttachTo(ScnObject *parent, u8 joint, const Vec3s *offset, Vec3s *rotation, u32 a, u32 b); \
    CollBox *GetFirstModelBox();                                                                    \
    CollBox *GetModelBoxes(u32 *count);                                                             \
    void SetFacing(s16 angle);                                                                      \
    void SetUpdateMode(u8 mode);


#define SDW_MEMBERS_ZoneList void Load(u32 id);

#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *inst, Animator *animator, u16 id, u32 opts);
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_ATTACHTO_SCNOBJECT_U8_CONST_VEC3S_VEC3S_U32_U32 1
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_ATTACHTO_SCNOBJECT_U8_CONST_VEC3S_VEC3S_U32_U32
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#define SDW_INLINE_SCNOBJECT_FACING 1
#define SDW_INLINE_SCNOBJECT_SETFACING_S16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_FACING
#undef SDW_INLINE_SCNOBJECT_SETFACING_S16
#define SDW_INLINE_SCNOBJECT_ISCOLLIDABLE 1
#define SDW_INLINE_SCNOBJECT_SETCOLLIDABLE_S32 1
#define SDW_INLINE_SCNOBJECT_SETNOCULL_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_ISCOLLIDABLE
#undef SDW_INLINE_SCNOBJECT_SETCOLLIDABLE_S32
#undef SDW_INLINE_SCNOBJECT_SETNOCULL_S32
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_SCNMOBILE_SETSHADOWRADIUS_U8 1
#include "../engine/scn_mobile_inlines.h"
#undef SDW_INLINE_SCNMOBILE_SETSHADOWRADIUS_U8
#define SDW_INLINE_SHADOW_SETVISIBLE_S32 1
#include "../engine/shadow_inlines.h"
#undef SDW_INLINE_SHADOW_SETVISIBLE_S32
#define SDW_INLINE_SHADOW_SETFLAG4_S32 1
#include "../engine/shadow_inlines.h"
#undef SDW_INLINE_SHADOW_SETFLAG4_S32
/* 0x57b764 - this TU's copy of the cinematic header's static opcode stride table (payload bytes per key, by opcode;
   the copies src/engine/cine.cpp and cine2.cpp index are g_cineOpStride 0x5816fc, g_cineOpStride2 and
   g_cineOpStride3). A static table in a header that every cinematic .cpp includes: each includer gets its own
   unreferenced copy in .data, ahead of its own data. Local definition standing in for that header, named
   s_cineOpStride as in the other batches (a static must not reuse the global name g_cineOpStride). */
static u8 s_cineOpStride[9] = {0, 8, 8, 4, 2, 2, 4, 2, 2};

extern s32 g_dt;
extern s32 g_dtMs;
#include "camera.h"
#include "../engine/scenaric.h"
#include "../engine/id_list.h"
#include "../engine/scn_tools.h"
#include "../engine/collide.h"
/* 0x576c88 - where a carried small rock sits on its carrier; const, main CONST ahead of the vtable */
const Vec3s g_smallRockCarryOffset = {10, 0, 0};
s32 ObjGrid_QueryBoxOverlap(CollBox *box, ScnObject **out);

#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8

inline CollBox *ScnObject::GetFirstModelBox()
{
    ModelBoxList *list = inst_model->boxes;
    if (list)
        return list->boxes;
    return 0;
}

#define SDW_INLINE_SCNOBJECT_GETMODELBOXES_U32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETMODELBOXES_U32

#define SDW_INLINE_FREE_SCN_GETPROPID_U16_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_SCN_GETPROPID_U16_U32

#define SDW_INLINE_FREE_SCN_GETPROPU32_U16_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_SCN_GETPROPU32_U16_U32

#define SDW_INLINE_ZONELIST_LOAD_U32 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_LOAD_U32

#define SDW_INLINE_ZONELIST_FINDCONTAINING_VEC3S 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_FINDCONTAINING_VEC3S

#define SDW_INLINE_FREE_ZONES_GET_U8 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_FREE_ZONES_GET_U8

#define SDW_INLINE_SCNOBJECT_BROADCASTAROUND_S32_S32_U16_U32_VOID 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_BROADCASTAROUND_S32_S32_U16_U32_VOID

#define SDW_INLINE_FREE_BOX_CONTAINSPOINTXZ_BOX_VEC3S 1
#include "../engine/coll_box_inlines.h"
#undef SDW_INLINE_FREE_BOX_CONTAINSPOINTXZ_BOX_VEC3S

#define SDW_INLINE_LAUNCHARC_STEP_S32_S16_S16_S16 1
#include "../engine/launch_arc_inlines.h"
#undef SDW_INLINE_LAUNCHARC_STEP_S32_S16_S16_S16

/* 0x4f3be0 */
void SmallRock::ApplyGravity(Vec3s *velocity)
{
    s32 speed = (fallTime * 1500) >> 12;
    if (speed > 1000)
        speed = 1000;
    if (sliding)
        velocity->z -= (s16)((g_dt * 400) / 4096);
    velocity->y += (s16)((speed * g_dt) / 4096);
}

/* 0x4f3c70 */
/* BYTES(dead-code): target is stored and never read, as in the original */
u16 SmallRock::Move(Vec3s *delta, u16 resolveFlags)
{
    ContactInfo contact;
    u16 hitFlags = Collide_ResolveMove(delta, &contact, 0xb54, resolveFlags, 0, 0, 10, 0, 0);
    if (hitFlags & COLL_FLOOR)
        fallTime = 0;
    if (!contact.movableObj && !sliding)
        moveFlags.movableContact = 0;
    else
        moveFlags.movableContact = 1;
    ScnObject *target = contact.floorObj;
    if (contact.wallObj)
        target = contact.wallObj;
    Translate(delta);
    return hitFlags;
}

/* 0x4f3d1c */
/* BYTES(slot-name, inferred): local names chosen for their stack slots (tools/vc6_locals.py), not recovered */
void SmallRock::Update()
{
    ScnObject *objTemp;
    Box *current_water;
    Vec3s current_probe;
    Vec3s localDelta;
    Vec3s point;
    if (slidingBox)
        sliding = Box_ContainsPointXZ(slidingBox, &pos);
    switch (state) {
        case SROCK_ST_SHATTERED:
            if (AnimFlags(ANIM_F_FINISHED))
                ReturnHome();
            break;
        case SROCK_ST_FALL:
            fallTime += g_dt;
            localDelta.x = 0;
            localDelta.y = 0;
            localDelta.z = 0;
            ApplyGravity(&localDelta);
            if (localDelta.y < 8)
                localDelta.y = 8;
            if ((Move(&localDelta, COLL_WALL) & COLL_FLOOR) && !moveFlags.movableContact) {
            } else
                moveFlags.movableContact = 1;
            /* deathBoxes is the ZoneList at +0xdc. */
            if (deathBoxes.FindContaining(&pos) && rockFlags.thrown) {
                ReturnHome();
                break;
            }
            current_probe.x = pos.x;
            current_probe.y = pos.y - 80;
            current_probe.z = pos.z;
            current_water = Zones_Get(ZONE_WATER)->FindContaining(&current_probe);
            if (current_water) {
                rockFlags.inWater = 1;
                SetUpdateMode(SCN_UPD_ALWAYS);
                if (!(current_water->flags & ZONE_WATER_NO_SURFACE)) {
                    if (rockFlags.hasIce) {
                        SetState(SROCK_ST_FLOATING);
                        break;
                    }
                    ReturnHome();
                    break;
                }
            }
            if (rockFlags.inWater && fallTime > 0x4000) {
                ReturnHome();
                break;
            }
            break;
        case SROCK_ST_FLOATING: {
            Box *zoneCurrent;
            Vec3s atLocal;
            s32 tempCount;
            s32 iValue;
            Vec3s displacementTemp;
            CollBox boxLocal;
            Vec3s velocity;
            ScnObject *objects[64];
            atLocal.x = pos.x;
            atLocal.y = pos.y - 80;
            atLocal.z = pos.z;
            zoneCurrent = Zones_Get(ZONE_WATER)->FindContaining(&atLocal);
            if (!zoneCurrent || (zoneCurrent->flags & ZONE_WATER_NO_SURFACE)) {
                StartFall();
                break;
            }
            Zone_GetFlowVelocity(zoneCurrent, &velocity);
            Vec3s_ScaleByDt(&velocity, &displacementTemp);
            displacementTemp.y = zoneCurrent->min[1] + 80 - pos.y;
            Move(&displacementTemp, COLL_WALL);
            boxLocal.max.x = iceBody.GetFirstModelBox()->max.x + pos.x;
            boxLocal.max.y = iceBody.GetFirstModelBox()->max.y + pos.y;
            boxLocal.max.z = iceBody.GetFirstModelBox()->max.z + pos.z;
            boxLocal.min.x = iceBody.GetFirstModelBox()->min.x + pos.x;
            boxLocal.min.y = iceBody.GetFirstModelBox()->min.y + pos.y;
            boxLocal.min.z = iceBody.GetFirstModelBox()->min.z + pos.z;
            boxLocal.max.y = boxLocal.min.y - 1;
            boxLocal.min.y = boxLocal.max.y - 30;
            tempCount = ObjGrid_QueryBoxOverlap(&boxLocal, objects);
            for (iValue = 0; iValue < tempCount; iValue++) {
                objTemp = objects[iValue];
                if (displacementTemp.y > 0)
                    displacementTemp.y = 0;
                objTemp->Collide_ResolveMove(&displacementTemp, 0, 0xb54, COLL_WALL, 0, 0, 10, 0, 0);
                objTemp->Translate(&displacementTemp);
            }
        } break;
        case SROCK_ST_ON_SEESAW:
            fallTime += g_dt;
            localDelta.x = 0;
            localDelta.y = 0;
            localDelta.z = 0;
            ApplyGravity(&localDelta);
            Move(&localDelta, COLL_WALL);
            moveFlags.movableContact = 1;
            break;
        case SROCK_ST_LAUNCHED:
            if (launch.Step(g_dtMs >> 2, &point.x, &point.y, &point.z)) {
                localDelta.x = point.x - pos.x;
                localDelta.y = point.y - pos.y;
                localDelta.z = point.z - pos.z;
                if (Move(&localDelta, COLL_WALL) && !moveFlags.movableContact && launch.t > 0x80)
                    StartFall();
            } else
                StartFall();
            break;
        case SROCK_ST_PUSHED:
            if (!moveFlags.pushed)
                StartFall();
            moveFlags.pushed = 0;
            break;
    }
    camShot.Update(this);
    AdvanceAnim();
}

/* 0x4f47c4 */
void SmallRock::StartFall()
{
    SetState(SROCK_ST_FALL);
    fallTime = 1;
}

/* 0x4f47e3 */
/* BYTES(slot-name, inferred): local names chosen for their stack slots (tools/vc6_locals.py), not recovered */
s32 SmallRock::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    switch (msgId) {
        case MSG_QUERY_ACTION:
            if (sender->GetClassId() == CLASSID_WOLF && !rockFlags.floating) {
                s16 height;
                s16 localCenter;
                CollBox *box = sender->GetFirstModelBox();
                localCenter = sender->pos.y + ((box->max.y + box->min.y) >> 1);
                height = (collBox->max.y + collBox->min.y) >> 1;
                if (height + pos.y <= localCenter + 90 && height + pos.y >= localCenter - 90)
                    return CTX_LIFT;
                return CTX_NONE;
            }
            break;
        case MSG_PUSH: {
            fallTime += g_dt;
            /* cast kept: the message arg is a void *: what it carries depends on the message id */
            Vec3s *localRequested = (Vec3s *)arg;
            Vec3s localDelta = *localRequested;
            Vec3s rotation;
            s32 tempAngle;
            ApplyGravity(&localDelta);
            Move(&localDelta, COLL_FLOOR | COLL_FLOOR_EDGE);
            localRequested->x = localDelta.x;
            localRequested->z = localDelta.z;
            rotation = rot;
            if (localRequested->x) {
                tempAngle = ((localDelta.x << 11) * 10000) / 31416;
                rotation.z = (rotation.z + (tempAngle << 1) / (collBox->max.x - collBox->min.x)) & 0xfff;
                rotation.x = 0;
            }
            if (localRequested->z) {
                tempAngle = ((-localDelta.z << 11) * 10000) / 31416;
                rotation.x = (rotation.x + (tempAngle << 1) / (collBox->max.z - collBox->min.z)) & 0xfff;
                rotation.z = 0;
            }
            rot = rotation;
            SetState(SROCK_ST_PUSHED);
            moveFlags.pushed = 1;
            return 1;
        }
        case MSG_SEESAW_TOUCH:
            if (state != SROCK_ST_ON_SEESAW && state != SROCK_ST_LAUNCHED) {
                SetState(SROCK_ST_ON_SEESAW);
                fallTime = 1;
            }
            break;
        case MSG_LAUNCH:
            moveFlags.movableContact = 0;
            SetState(SROCK_ST_LAUNCHED);
            /* cast kept: the message arg is a void *: what it carries depends on the message id */
            launch = *(LaunchArc *)arg;
            fallTime = 1;
            rockFlags.thrown = 1;
            if (launch.camera && g_camMode != CAM_SCRIPTED) {
                camShot.Start(launch.camera, this, launch.camParam);
                launch.camera = 0;
            }
            return 1;
        case MSG_FREEZE:
            camShot.Stop(this);
            return 1;
        case MSG_CINE_END:
            StartFall();
            return 1;
        case MSG_PICKUP: {
            ScnObject *local_parent = sender;
            /* cast kept: the message arg is a void *: what it carries depends on the message id */
            u8 parentJoint = (u8)(u32)arg;
            Vec3s rotation;
            carryYaw = (Facing() - local_parent->Facing()) & 0xfff;
            rotation.x = 0;
            rotation.z = 0;
            rotation.y = carryYaw;
            AttachTo(local_parent, parentJoint, &g_smallRockCarryOffset, &rotation, 1, 0);
            SetCollidable(0);
            shadow.SetVisible(0);
            SetState(SROCK_ST_REST);
            return 1;
        }
        case MSG_QUERY_HELD_ACTION:
            return HELD_THROWABLE;
        case MSG_KILL:
            if (rockFlags.thrown)
                ReturnHome();
            break;
        case MSG_LANDED:
            SetUpdateMode(SCN_UPD_ALWAYS);
            if (arg && rockFlags.thrown)
                SetState(SROCK_ST_SHATTERED);
            else
                StartFall();
            break;
        case MSG_DROP: {
            /* cast kept: the message arg is a void *: what it carries depends on the message id */
            DropMsgArg *drop = (DropMsgArg *)arg;
            Detach();
            SetCollidable(1);
            shadow.SetVisible(1);
            SetFacing((sender->Facing() + carryYaw) & 0xfff);
            if (!drop->flag1)
                SetPosition(&drop->pos);
            if (drop->placed) {
                fallTime = 0;
                rockFlags.thrown = 1;
                SetState(SROCK_ST_FALL);
            } else
                SetState(SROCK_ST_REST);
            return 1;
        }
    }
    return 0;
}

/* 0x4f4ec9 */
void SmallRock::Reset()
{
    camShot.Init(0);
    if (rockFlags.initAtReset) {
        ReturnHome();
        SnapToGround(1);
    }
}

/* 0x4f4f09 */
void SmallRock::Render(Camera *view)
{
    ScnMobile::Render(view);
    if (rockFlags.floating) {
        iceBody.pos = pos;
        iceBody.Render(view);
    }
}

/* 0x4f4f69 */
void SmallRock::SetState(u8 value)
{
    if (state == SROCK_ST_SHATTERED)
        SwapModel(&baseModel);
    state = value;
    switch (state) {
        case SROCK_ST_SHATTERED:
            SwapModel(&brokenModel);
            PlayAnim(AROCHE5B_ANIM_BREAK, 0, 0);
            /* cast kept: HandleMessage's arg is a void *: this message passes a number in it */
            BroadcastAround(200, 200, 100, MSG_KILL, (void *)KILL_CRUSH);
            break;
        default:
            PlayAnim(AROCHE5B_ANIM_STAND, 1, 0);
            break;
    }
    if (state == SROCK_ST_FLOATING && rockFlags.hasIce) {
        rockFlags.floating = 1;
        iceBody.PlayAnim(AGLACON1_ANIM_STAND2, 0, 0);
        iceBody.pos = pos;
    }
}

/* 0x4f5138 SmallRock_Init. The two AltModel providers are ScnBody methods,
 * as recorded in data/symbols_modules.csv at 0x51033a and 0x5101d5. */
/* BYTES(slot-name, inferred): local names chosen for their stack slots (tools/vc6_locals.py), not recovered */
void SmallRock::PostLoadInit()
{
    Box **localList;
    u16 recordTemp[10];
    u32 id;
    u16 countTemp;
    u16 *rec_temp = record;
    u32 modelCount;
    state = SROCK_ST_REST;
    AltModel_SaveBody(&baseModel);
    deathBoxes.Load(WAR_IDO_DEATHBOX);
    rockFlags.thrown = 0;
    rockFlags.floating = 0;
    rockFlags.inWater = 0;
    sliding = 0;
    id = Scn_GetPropId(rec_temp, 8);
    slidingBox = 0;
    if (id) {
        /* cast kept: an id list holds untyped record pointers; the caller knows the kind it asked for */
        localList = (Box **)Scn_FindIdList((u16)id, &countTemp);
        if (countTemp == 1)
            slidingBox = *localList;
    }
    rockFlags.initAtReset = Scn_GetPropU32(rec_temp, 4) != 0;
    if (Scn_GetPropU32(rec_temp, 0)) {
        homePos = pos;
        StartFall();
        SnapToGround(1);
    } else {
        SnapToGround(1);
        SetState(SROCK_ST_REST);
        homePos = pos;
    }
    collBox = GetModelBoxes(&modelCount);
    if (!(collBox->flags & COLLBOX_ELLIPSOID) && modelCount > 1)
        collBox++;
    fallTime = 0;
    moveFlags.pushed = 0;
    moveFlags.movableContact = 0;
    weight = 120;
    SetShadowRadius((u8)collBox->max.x);
    shadow.SetFlag4(1);
    camShot.Init(0);
    AltModel_InitBody(&brokenModel, WAR_IDO_AROCHE5B);
    if (Scn_BuildRecordFromExport(WAR_IDO_AGLACON1, recordTemp, 0, 0)) {
        iceBody.Init(recordTemp, 0);
        iceBody.SetNoCull(1);
        rockFlags.hasIce = 1;
    } else
        rockFlags.hasIce = 0;
}

/* 0x4f5491 */
void SmallRock::ReturnHome()
{
    SetPosition(&homePos);
    SetUpdateMode(SCN_UPD_NORMAL);
    StartFall();
    rockFlags.thrown = 0;
    rockFlags.floating = 0;
    rockFlags.inWater = 0;
}

/* 0x4f55ab */
ScnObject *SmallRock_Create(void *record)
{
    ScnBody *obj = new SmallRock;
    obj = obj->Init(record, 0);
    obj->flags |= SCN_OF_CUSTOM_COLLIDE;
    return obj;
}

/* 0x4f564c */
s32 SmallRock::CustomCollide(ScnObject *querier, CollBox *mover, Vec3s *disp, s32 *outFrac, s32 *outY,
                             CollContact *contacts, s32 *nContacts, u32 mode)
{
    if (!IsCollidable())
        return 0;
    if (rockFlags.floating)
        return Collide_BoxVsObjBox(this, mover, disp, iceBody.GetFirstModelBox(), &pos, outFrac, outY, contacts,
                                   nContacts);
    else {
        CollBox *box = GetFirstModelBox();
        if (box)
            return Collide_BoxVsObjBox(this, mover, disp, GetFirstModelBox(), &pos, outFrac, outY, contacts, nContacts);
        else
            return 0;
    }
}
