/*
 * T210 - original object Seesaw.cpp (guessed name), rebuilt as one translation unit: the seesaw class, then the
 * CamShot utility (the seesaw -> Pad_AnySkipInput transition at 0x4ef717 is unaligned; CamShot is also called by bull,
 * Rock and SmallRock).
 *   .text  0x4eb0b0-0x4efaed (seesaw_Reset .. seesaw_UpdateLaunches in address order, then Pad_AnySkipInput and
 *          CamShot Init / Stop / Update / Start)
 *   .rdata 0x576b44-0x576b68 (??_7seesaw)
 *   .data  0x57b758-0x57b764 (this TU's copy of the Cine header's 9-byte opcode stride table, + 3 pad)
 * The seesaw's functions are in address order, and the CamShot utility follows them.
 */
/* BYTES: cast, dead-code, inline, layout, slot-name, slot-scope. */
/* BYTES(layout): the Cine.h header static: every object including the cinematic header carries this copy in its .data, referenced or not */
#define SDW_MEMBERS_ScnObject            \
    static void *operator new(u32 size); \
    CollBox *GetFirstModelBox();         \
    CollBox *GetModelBoxes(u32 *count);  \
    u16 Weight()                         \
    {                                    \
        return weight;                   \
    }                                    \
    void SetUpdateMode(u8 mode);


#define SDW_MEMBERS_seesaw             \
    s32 FindLaunch(ScnObject *obj);    \
    s32 IsLaunching(ScnObject *obj);   \
    s32 OverlapXZ(const CollBox *box); \
    s32 OverlapXZ(const CollBox *box, const Vec3s *at);
#include "sdw_enums.h"
#include "scenaric_props.h"
#include "sdw_classes.h"
#define SDW_INLINE_SCNOBJECT_FACING 1
#define SDW_INLINE_SCNOBJECT_SETPARTHEIGHT_U32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_FACING
#undef SDW_INLINE_SCNOBJECT_SETPARTHEIGHT_U32
#define SDW_INLINE_SCNOBJECT_SETCOLLIDABLE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETCOLLIDABLE_S32
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_INSTFLAGS_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_INSTFLAGS_U16
/* 0x57b758 - this TU's copy of the cinematic header's static opcode stride table (payload bytes per key, by opcode;
   the copies src/engine/cine.cpp and cine2.cpp index are g_cineOpStride 0x5816fc, g_cineOpStride2 and
   g_cineOpStride3). A static table in a header that every cinematic .cpp includes: each includer gets its own
   unreferenced copy in .data, ahead of its own data. Local definition standing in for that header, named
   s_cineOpStride as in the other batches (a static must not reuse the global name g_cineOpStride). */
static u8 s_cineOpStride[9] = {0, 8, 8, 4, 2, 2, 4, 2, 2};

extern Wolf *g_pWolf;
extern s32 g_dtMs;
#include "camera.h"
#include "../engine/fixed_math.h"
#include "../engine/input.h"
#include "../engine/coll_clip.h"
#include "../engine/obj_grid.h"
#include "../engine/scenaric.h"
#include "../engine/sound_mgr.h"
#include "../engine/id_list.h"
#include "../app/app_main.h"
extern "C" s16 g_sinTable4096[5122];
extern "C" const s16 *g_pCosTable;
u16 Sound_Play(u16 id, void *owner, u16 volume, u8 priority, s32 pitch);
s32 ObjGrid_QueryBoxOverlap(CollBox *box, ScnObject **out);

#define SDW_INLINE_FREE_SCENARIC_CLASSFLAGS_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_FREE_SCENARIC_CLASSFLAGS_U16

#define SDW_INLINE_FREE_SCN_GETPROPU32_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_SCN_GETPROPU32_VOID_U32

#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8

#define SDW_INLINE_SCNOBJECT_GETMODELBOXES_U32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETMODELBOXES_U32

#define SDW_INLINE_FREE_ISHELD_SCNOBJECT_VEC3S 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_FREE_ISHELD_SCNOBJECT_VEC3S

inline s32 seesaw::FindLaunch(ScnObject *obj)
{
    for (u16 i = 0; i < launchCount; i++) {
        if (launches[i].obj == obj)
            return i;
    }
    return -1;
}

inline s32 seesaw::IsLaunching(ScnObject *obj)
{
    for (u16 i = 0; i < launchCount; i++) {
        if (launches[i].obj == obj)
            return 1;
    }
    if (obj->HandleMessage(this, MSG_QUERY_IN_FLIGHT, 0))
        return 1;
    return 0;
}

#define SDW_INLINE_LAUNCHARC_STEP_S32_S16_S16_S16 1
#include "../engine/launch_arc_inlines.h"
#undef SDW_INLINE_LAUNCHARC_STEP_S32_S16_S16_S16

inline CollBox *ScnObject::GetFirstModelBox()
{
    ModelBoxList *list = inst_model->boxes;
    if (list)
        return list->boxes;
    return 0;
}

#define SDW_INLINE_FREE_BOX_CONTAINSPOINT_BOX_VEC3S 1
#include "../engine/coll_box_inlines.h"
#undef SDW_INLINE_FREE_BOX_CONTAINSPOINT_BOX_VEC3S

/* Inline forms of the overlap expressions at 0x4ec355
 * and 0x4ed75a. These retain the four signed box-edge distances. */
/* BYTES(inline): source-only inline: keeps the four signed box-edge distances of the original (0x4ec355, 0x4ed75a) */
inline s32 seesaw::OverlapXZ(const CollBox *box)
{
    return (~((footprint.max[0] - box->min.x) | (box->max.x - footprint.min[0]) | (footprint.max[2] - box->min.z) |
              (box->max.z - footprint.min[2])) &
            0x80000000U) != 0;
}

inline s32 seesaw::OverlapXZ(const CollBox *box, const Vec3s *at)
{
    return (~((footprint.max[0] - (box->min.x + at->x)) | ((box->max.x + at->x) - footprint.min[0]) |
              (footprint.max[2] - (box->min.z + at->z)) | ((box->max.z + at->z) - footprint.min[2])) &
            0x80000000U) != 0;
}

/* 0x4eb0b0 */
void seesaw::Reset()
{
    camShot.Stop(this);
    Camera_ReleaseScripted(this);
    ejectionTimer2 = 0;
    ejectionTimer1 = 0;
    creakSound = 0;
}

/* 0x4eb0fd */
/* BYTES(slot-name, inferred): local names chosen for their stack slots (tools/vc6_locals.py), not recovered */
void seesaw::PostLoadInit()
{
    u32 *localList;
    u16 trajectoryPropertiesCurrent[2] = {0x2c, 0x30};
    u32 id;
    u32 iTemp;
    u16 countTemp;
    CollBox *boxLocal;
    u32 cameraIdsLocal[2];
    Vec3s temp_center;
    void *rec_value;
    s16 tempAngle;
    u32 modelCount;
    rec_value = record;
    creakSound = 0;
    pendingLaunch.positive = 0;
    pendingLaunch.negative = 0;
    hopIdx = 0;
    pendingLaunch.rock = 0;
    tempAngle = Facing() & 0x7ff;
    if ((tempAngle - 0x400 >= 0 ? tempAngle - 0x400 : -(tempAngle - 0x400)) < 0x200) {
        axisIsZ = 1;
        rot.y = 0x400;
    } else {
        axisIsZ = 0;
        rot.y = 0;
    }
    seesawFlags = Scn_GetPropU32(rec_value, 0x20);
    maxTiltNeg = ((s32)Scn_GetPropU32(rec_value, 0x24) << 12) / 360;
    maxTiltPos = ((s32)Scn_GetPropU32(rec_value, 0x28) << 12) / 360;
    if (maxTiltPos == 0)
        maxTiltPos = maxTiltNeg;
    raisedBox = 0;
    boxLocal = GetModelBoxes(&modelCount);
    temp_center = pos;
    footprint.flags = 0;
    if (axisIsZ) {
        footprint.min[1] = temp_center.y + boxLocal->min.y;
        footprint.max[1] = temp_center.y + boxLocal->max.y;
        footprint.min[2] = temp_center.z + boxLocal->min.x;
        footprint.max[2] = temp_center.z + boxLocal->max.x;
        footprint.max[0] = temp_center.x - boxLocal->min.z;
        footprint.min[0] = temp_center.x - boxLocal->max.z;
        halfLength = (footprint.max[2] - footprint.min[2]) >> 1;
        halfWidth = (footprint.max[0] - footprint.min[0]) >> 1;
        centreOnAxis = (footprint.min[2] + footprint.max[2]) >> 1;
        footprint.min[2] = temp_center.z - halfLength;
        footprint.max[2] = temp_center.z + halfLength;
        footprint.min[0] = temp_center.x - halfWidth;
        footprint.max[0] = temp_center.x + halfWidth;
        footprintUp500 = footprint;
        footprintUp500.min[1] -= 500;
        raisedBox = &footprintUp500;
    } else {
        footprint.min[0] = boxLocal->min.x + temp_center.x;
        footprint.min[1] = boxLocal->min.y + temp_center.y;
        footprint.min[2] = boxLocal->min.z + temp_center.z;
        footprint.max[0] = boxLocal->max.x + temp_center.x;
        footprint.max[1] = boxLocal->max.y + temp_center.y;
        footprint.max[2] = boxLocal->max.z + temp_center.z;
        halfLength = (footprint.max[0] - footprint.min[0]) >> 1;
        halfWidth = (footprint.max[2] - footprint.min[2]) >> 1;
        centreOnAxis = (footprint.min[0] + footprint.max[0]) >> 1;
        footprint.min[0] = temp_center.x - halfLength;
        footprint.max[0] = temp_center.x + halfLength;
        footprint.min[2] = temp_center.z - halfWidth;
        footprint.max[2] = temp_center.z + halfWidth;
        footprintUp500 = footprint;
        footprintUp500.min[1] -= 500;
        raisedBox = &footprintUp500;
    }
    plankTopOffset = -boxLocal->min.y;
    SetCollidable(0);
    bodyCount = 0;
    launchCount = 0;
    tilt = 0;
    tiltVel = 0;
    pivotY = 0;
    pivotAxisCoord = 0;
    tiltSin = 0;
    tiltCos = 0x1000;
    cameraIdsLocal[0] = Scn_GetPropU32(rec_value, 0);
    cameraIdsLocal[1] = Scn_GetPropU32(rec_value, 12);
    launchCamFlags[0] = (Scn_GetPropU32(rec_value, 4) != 0) + (Scn_GetPropU32(rec_value, 8) ? CAMSCR_BLEND_OUT : 0);
    launchCamFlags[1] = (Scn_GetPropU32(rec_value, 16) != 0) + (Scn_GetPropU32(rec_value, 20) ? CAMSCR_BLEND_OUT : 0);
    for (iTemp = 0; iTemp < 2; iTemp++) {
        localList = Scn_FindIdList((u16)Scn_GetPropU32(rec_value, trajectoryPropertiesCurrent[iTemp]), &countTemp);
        if (countTemp == 1) {
            traj[iTemp] = (Trajectory3 *)*localList; /* cast kept: an id list holds record addresses as u32 */
            if (traj[iTemp]->count != 3)
                traj[iTemp] = 0;
        } else
            traj[iTemp] = 0;
        localList = Scn_FindIdList((u16)cameraIdsLocal[iTemp], &countTemp);
        if (!localList)
            launchCam[iTemp] = 0;
        else
            launchCam[iTemp] = (CamSetup *)*localList; /* cast kept: an id list holds record addresses as u32 */
    }
    camShot.Init(seesawFlags & SEESAW_F_NO_FREEZE);
    id = Scn_GetPropU32(rec_value, 24);
    ejectionBox1 = 0;
    if (id) {
        localList = Scn_FindIdList((u16)id, &countTemp);
        if (countTemp == 1)
            ejectionBox1 = (Box *)*localList; /* cast kept: an id list holds record addresses as u32 */
    }
    id = Scn_GetPropU32(rec_value, 28);
    ejectionBox2 = 0;
    if (id) {
        localList = Scn_FindIdList((u16)id, &countTemp);
        if (countTemp == 1)
            ejectionBox2 = (Box *)*localList; /* cast kept: an id list holds record addresses as u32 */
    }
    ejectionTimer2 = 0;
    ejectionTimer1 = 0;
    if (raisedBox) {
        footprintUp1500 = *raisedBox;
        footprintUp1500.min[1] -= 1000;
        raisedBox = &footprintUp1500;
    }
    SetUpdateMode(SCN_UPD_ALWAYS);
    SetPartHeight(0);
}

/* 0x4ebbb7 */
void seesaw::UpdateEjectionTimers()
{
    if (ejectionBox1) {
        if (Box_ContainsPoint(ejectionBox1, &g_pWolf->pos))
            ejectionTimer1 = 4000;
        else if (ejectionTimer1 > 0) {
            if (g_pWolf->HandleMessage(this, MSG_WOLF_IS_GROUNDED, 0))
                ejectionTimer1 = 100;
            else
                ejectionTimer1 -= (s16)g_dtMs;
        }
    }
    if (ejectionBox2) {
        if (Box_ContainsPoint(ejectionBox2, &g_pWolf->pos))
            ejectionTimer2 = 4000;
        else if (ejectionTimer2 > 0) {
            if (g_pWolf->HandleMessage(this, MSG_WOLF_IS_GROUNDED, 0))
                ejectionTimer2 = 100;
            else
                ejectionTimer2 -= (s16)g_dtMs;
        }
    }
}

/* 0x4ebde2 */
/* BYTES(slot-name, inferred): local names chosen for their stack slots (tools/vc6_locals.py), not recovered */
s32 seesaw::IsBodyObstructed()
{
    CollBox query_local;
    s32 jValue;
    s32 tempCount;
    s32 iValue;
    ScnObject *objectsLocal[64];
    ScnObject *obj;
    for (iValue = 0; iValue < bodyCount; iValue++) {
        ScnObject *tempOther;
        Vec3s size_value;
        CollBox *box;
        obj = bodies[iValue].obj;
        if (obj->InstFlags(INST_F_ATTACHED))
            continue;
        box = obj->GetFirstModelBox();
        if (box) {
            query_local.min.x = box->min.x + obj->pos.x;
            query_local.min.y = box->min.y + obj->pos.y;
            query_local.min.z = box->min.z + obj->pos.z;
            query_local.max.x = box->max.x + obj->pos.x;
            query_local.max.y = box->max.y + obj->pos.y;
            query_local.max.z = box->max.z + obj->pos.z;
            query_local.min.y -= 50;
        } else {
            size_value.x = 10;
            size_value.y = 10;
            size_value.z = 10;
            query_local.max.x = obj->pos.x + size_value.x;
            query_local.max.y = obj->pos.y + size_value.y;
            query_local.max.z = obj->pos.z + size_value.z;
            query_local.min.x = obj->pos.x - size_value.x;
            query_local.min.y = obj->pos.y - size_value.y;
            query_local.min.z = obj->pos.z - size_value.z;
            query_local.min.y -= 50;
        }
        tempCount = ObjGrid_QueryBoxOverlap(&query_local, objectsLocal);
        for (jValue = 0; jValue < tempCount; jValue++) {
            tempOther = objectsLocal[jValue];
            if (tempOther->GetClassId() != CLASSID_SEESAW && tempOther->GetClassId() != obj->GetClassId() &&
                !tempOther->InstFlags(INST_F_ATTACHED))
                return 1;
        }
    }
    return 0;
}

/* 0x4ec0d0 */
/* BYTES(slot-scope, inferred): the nested block(s) only order the frame: their locals are allocated after the enclosing scope's */
/* BYTES(slot-name, inferred): local names chosen for their stack slots (tools/vc6_locals.py), not recovered */
void seesaw::Update()
{
    s16 local_feet;
    s32 step;
    ScnObject *candidate;
    s16 height;
    u16 localI;
    s16 local_distance;
    Vec3s delta;
    s32 errorCurrent;
    s16 tempOldTilt;
    Vec3s *tempOrigin;
    s16 localTorque = 0;
    delta.x = 0;
    delta.y = 0;
    delta.z = 0;
    tempOrigin = &pos;
    tempOldTilt = tilt;
    if (creakSound && !Sound_IsPlaying(creakSound))
        creakSound = 0;
    UpdateEjectionTimers();
    tiltSin = g_sinTable4096[(s16)(tilt & 0xfff)];
    tiltCos = g_pCosTable[(s16)(tilt & 0xfff)];
    if (axisIsZ)
        pivotAxisCoord = tempOrigin->z;
    else
        pivotAxisCoord = tempOrigin->x;
    pivotY = tempOrigin->y - ((plankTopOffset * tiltCos) >> 12);
    {
        CollBox *boxLocal;
        Vec3s *at;
        s32 overlap_value;
        ListNode **head_value;
        ListNode *node_local;
        ListNode ***tempCell;
        s16 tempColumns;
        s16 localRows;
        s16 cellX;
        s16 tempCellZ;
        ListNode ***row;
        ObjGrid_CellFromXZ(footprint.min[0] - 500, footprint.min[2] - 500, &cellX, &tempCellZ);
        ObjGrid_CellFromXZ(footprint.max[0] + 500, footprint.max[2] + 500, &tempColumns, &localRows);
        localRows -= tempCellZ;
        tempColumns -= cellX;
        row = &g_objGridCells[cellX + tempCellZ * g_objGridDimX];
        while (localRows >= 0) {
            tempCell = row;
            cellX = tempColumns;
            while (cellX >= 0) {
                head_value = *tempCell;
                node_local = *head_value;
                while (node_local) {
                    candidate = (ScnObject *)node_local->data; /* cast kept: a grid cell's list node holds a void * */
                    if (candidate != this) {
                        at = &candidate->pos;
                        boxLocal = candidate->GetFirstModelBox();
                        if (boxLocal) {
                            overlap_value = OverlapXZ(boxLocal, at);
                            local_feet = at->y + boxLocal->max.y;
                        } else {
                            overlap_value = at->x >= footprint.min[0] && at->x <= footprint.max[0] &&
                                            at->z >= footprint.min[2] && at->z <= footprint.max[2];
                            local_feet = at->y;
                        }
                        if (overlap_value) {
                            if (axisIsZ)
                                local_distance = at->z - pivotAxisCoord;
                            else
                                local_distance = at->x - pivotAxisCoord;
                            height = (local_distance * tiltSin) / tiltCos;
                            delta.y = pivotY + height - local_feet;
                            if (delta.y <= 50 && delta.y >= -20)
                                AddBody(candidate, 512);
                        }
                    }
                    node_local = node_local->next;
                }
                tempCell++;
                cellX--;
            }
            row += g_objGridDimX;
            localRows--;
        }
    }
    if (!IsBodyObstructed()) {
        for (localI = 0; localI < bodyCount; localI++) {
            s16 axisCoord;
            if (axisIsZ)
                axisCoord = bodies[localI].obj->pos.z;
            else
                axisCoord = bodies[localI].obj->pos.x;
            if (bodies[localI].landingSpeed > 0)
                localTorque +=
                    (s16)(((axisCoord - centreOnAxis) * bodies[localI].obj->Weight() * bodies[localI].landingSpeed) >>
                          16);
        }
    } else
        localTorque = tilt;
    if (localTorque < -maxTiltNeg)
        localTorque = -maxTiltNeg;
    else if (localTorque > maxTiltPos)
        localTorque = maxTiltPos;
    errorCurrent = localTorque - tilt;
    if ((errorCurrent >= 0 ? errorCurrent : -errorCurrent) > 0xcd)
        tiltVel = errorCurrent * 3;
    else
        tiltVel = (errorCurrent + tiltVel * 3) / 4;
    step = (errorCurrent + tiltVel) >> 5;
    if (step < -62)
        step = -62;
    if (step > 62)
        step = 62;
    tilt = step + tilt;
    if (tilt < -maxTiltNeg)
        tilt = -maxTiltNeg;
    else if (tilt > maxTiltPos)
        tilt = maxTiltPos;
    tiltSin = g_sinTable4096[(s16)(tilt & 0xfff)];
    tiltCos = g_pCosTable[(s16)(tilt & 0xfff)];
    pivotY = tempOrigin->y - ((plankTopOffset * tiltCos) >> 12);
    if (!creakSound) {
        s32 oldTilted;
        s32 newTilted;
        oldTilted = (tempOldTilt >= 0 ? tempOldTilt : -tempOldTilt) > 0x71;
        newTilted = (tilt >= 0 ? tilt : -tilt) > 0x71;
        if (oldTilted ^ newTilted)
            creakSound = Sound_Play(SND_SBSTIMOV, this, 0xff, SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 0x1000);
    }
    for (localI = 0; localI < bodyCount; localI++) {
        s32 localLaunchable;
        s32 negative_current;
        CollBox *box_temp;
        s32 local_handled;
        Vec3s *at_value;
        ScnObject *obj;
        s32 offset;
        s32 positive;
        obj = bodies[localI].obj;
        box_temp = obj->GetFirstModelBox();
        at_value = &obj->pos;
        if (box_temp) {
            s16 otherHeight;
            if (axisIsZ) {
                local_distance = at_value->z + box_temp->min.z - pivotAxisCoord;
                height = (local_distance * tiltSin) / tiltCos;
                local_distance = at_value->z + box_temp->max.z - pivotAxisCoord;
                otherHeight = (local_distance * tiltSin) / tiltCos;
            } else {
                local_distance = at_value->x + box_temp->min.x - pivotAxisCoord;
                height = (local_distance * tiltSin) / tiltCos;
                local_distance = at_value->x + box_temp->max.x - pivotAxisCoord;
                otherHeight = (local_distance * tiltSin) / tiltCos;
            }
            if (otherHeight < height)
                height = otherHeight;
            delta.y = pivotY + height - (at_value->y + box_temp->max.y);
            if (delta.y < 0) {
                if (delta.y < -8)
                    delta.y += 2;
                else
                    delta.y = 0;
            }
        } else {
            if (axisIsZ)
                local_distance = at_value->z - pivotAxisCoord;
            else
                local_distance = at_value->x - pivotAxisCoord;
            height = (local_distance * tiltSin) / tiltCos;
            delta.y = pivotY + height - at_value->y;
        }
        if ((delta.y >= 0 ? delta.y : -delta.y) > 50)
            continue;
        if ((seesawFlags & SEESAW_F_SLIDE) && (tilt >= 0 ? tilt : -tilt) > 56) {
            Vec3s drift;
            s16 speed_current;
            speed_current = (g_sinTable4096[(s16)(tilt & 0xfff)] * 100) >> 12;
            if (axisIsZ) {
                drift.z = speed_current;
                drift.y = drift.x = 0;
            } else {
                drift.x = speed_current;
                drift.y = drift.z = 0;
            }
            obj->HandleMessage(this, MSG_SEESAW_DRIFT, &drift);
        }
        /* cast kept: the height step travels in the void * argument */
        local_handled = obj->HandleMessage(this, MSG_SEESAW_TOUCH, (void *)(s32)delta.y);
        if (axisIsZ)
            offset = obj->pos.z - centreOnAxis;
        else
            offset = obj->pos.x - centreOnAxis;
        negative_current = offset < -30;
        positive = offset > 30;
        if (Scenaric_ClassFlags(obj->GetClassId()) & SCN_CF_SHEEP_ANCHORABLE) {
            WolfSpotArg tempAnchor;
            tempAnchor.pos.y = obj->pos.y;
            tempAnchor.radius = 15;
            if (axisIsZ) {
                tempAnchor.pos.z = offset < 0 ? pos.z - 300 : pos.z + 300;
                tempAnchor.pos.x = pos.x;
            } else {
                tempAnchor.pos.z = pos.z;
                tempAnchor.pos.x = offset < 0 ? pos.x - 300 : pos.x + 300;
            }
            obj->HandleMessage(this, MSG_SET_ANCHOR, &tempAnchor);
        }
        if ((delta.y < 0 || (Scenaric_ClassFlags(obj->GetClassId()) & SCN_CF_SNAP_TO_SUPPORT)) &&
            (Scenaric_ClassFlags(obj->GetClassId()) & SCN_CF_SEESAW_SNAP) && !local_handled)
            obj->Translate(&delta);
        localLaunchable = (pendingLaunch.negative || pendingLaunch.positive) && !launchCount;
        if (localLaunchable) {
            hopTraj[hopIdx].count = 3;
            hopTraj[hopIdx].pts[0] = obj->pos;
            hopTraj[hopIdx].pts[1].x = obj->pos.x;
            hopTraj[hopIdx].pts[1].y = obj->pos.y - 300;
            hopTraj[hopIdx].pts[1].z = obj->pos.z;
            hopTraj[hopIdx].pts[2].x = obj->pos.x;
            hopTraj[hopIdx].pts[2].y = pos.y;
            hopTraj[hopIdx].pts[2].z = obj->pos.z;
            if (negative_current && traj[0] && pendingLaunch.positive) {
                if (obj->GetClassId() == CLASSID_SHEEP && !pendingLaunch.rock)
                    LaunchObject(obj, &hopTraj[hopIdx++], 0, 0, 1);
                else
                    LaunchObject(obj, traj[0], launchCam[0], launchCamFlags[0], 0);
            } else if (positive && traj[1] && pendingLaunch.negative) {
                if (obj->GetClassId() == CLASSID_SHEEP && !pendingLaunch.rock)
                    LaunchObject(obj, &hopTraj[hopIdx++], 0, 0, 1);
                else
                    LaunchObject(obj, traj[1], launchCam[1], launchCamFlags[1], 0);
            }
            hopIdx %= 4;
        }
    }
    pendingLaunch.positive = 0;
    pendingLaunch.negative = 0;
    pendingLaunch.rock = 0;
    if (axisIsZ) {
        Vec3s rotation;
        rotation.x = 0;
        rotation.y = 0xc00;
        rotation.z = tilt;
        rot = rotation;
    } else {
        Vec3s rotation;
        rotation.x = 0;
        rotation.y = 0;
        rotation.z = tilt;
        rot = rotation;
    }
    UpdateLaunches();
    AdvanceAnim();
    bodyCount = 0;
}

/* 0x4ed435 */
s32 seesaw::GroundQuery(::GroundQuery *query)
{
    s16 distance;
    s16 height;
    if (flags & SCN_OF_IN_CINE_BOX)
        return 0;
    if (query->pos.x >= footprint.min[0] && query->pos.x <= footprint.max[0] && query->pos.z >= footprint.min[2] &&
        query->pos.z <= footprint.max[2]) {
        if (axisIsZ)
            distance = query->pos.z - pivotAxisCoord;
        else
            distance = query->pos.x - pivotAxisCoord;
        height = (distance * tiltSin) / tiltCos;
        query->pos.y = pivotY + height;
        query->normal.y = -tiltCos;
        if (axisIsZ) {
            query->normal.z = tiltSin;
            query->normal.x = 0;
        } else {
            query->normal.x = tiltSin;
            query->normal.z = 0;
        }
        return 1;
    }
    return 0;
}

/* 0x4ed584 */
s32 seesaw::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    switch (msgId) {
        case MSG_GROUND_QUERY:
            return GroundQuery((::GroundQuery *)arg); /* cast kept: the query travels in the void * argument */
        case MSG_FREEZE:
            camShot.Stop(this);
            return 1;
    }
    return 0;
}

/* 0x4ed5d0 */
ScnObject *seesaw_Create(void *record)
{
    seesaw *obj = new seesaw;
    obj = (seesaw *)obj->Init(record, 0); /* cast kept: Init returns the object as its ScnBody base */
    obj->flags |= SCN_OF_CUSTOM_COLLIDE;
    obj->raisedBox = 0;
    obj->tiltSin = 0;
    obj->tiltCos = 0x1000;
    return obj;
}

/* 0x4ed66d. The two vertex buffers retain the original 0x300-byte stack
 * extents, although this routine writes only four vertices in each.
 * Contact triVerts and moved-box flags are intentionally never initialized. */
/* BYTES(dead-code): localQuerierPos is stored and never read, as in the original */
/* BYTES(slot-name, inferred): local names chosen for their stack slots (tools/vc6_locals.py), not recovered */
s32 seesaw::CustomCollide(ScnObject *querier, CollBox *mover, Vec3s *disp, s32 *outFrac, s32 *outY,
                          CollContact *contacts, s32 *nContacts, u32 mode)
{
    s32 localBottomY;
    Vec3s temp_detectorDelta;
    Box6i detectorBox_current;
    Vec3s normal;
    s16 maxZ;
    s16 maxY;
    s16 maxX;
    s16 minZ;
    s16 minY;
    s16 currentMinX;
    s32 temp_hits;
    CollContact *value_contact;
    Vec3i temp_vertices[64];
    s32 tempFraction;
    Vec3s *localQuerierPos;
    CollBox local_moved;
    s32 local_result;
    s16 cosine_temp;
    s32 currentHeight;
    Vec3s *origin_current;
    s32 overlap_local;
    s16 sine_local;
    Vec3i tempSavedVertices[64];
    s32 slopeLocal;

    origin_current = &pos;
    localQuerierPos = &querier->pos;
    temp_hits = 0;
    *outFrac = 0x1000;
    tilt = rot.z;
    if (*nContacts >= 16)
        return 0;
    local_moved.min.x = mover->min.x + disp->x;
    local_moved.min.y = mover->min.y + disp->y;
    local_moved.min.z = mover->min.z + disp->z;
    local_moved.max.x = mover->max.x + disp->x;
    local_moved.max.y = mover->max.y + disp->y;
    local_moved.max.z = mover->max.z + disp->z;
    overlap_local = OverlapXZ(&local_moved);
    value_contact = &contacts[*nContacts];
    if (axisIsZ) {
        value_contact->normal.x = 0;
        sine_local = g_sinTable4096[(u16)tilt & 0xfff];
        value_contact->normal.z = sine_local;
        value_contact->point.x = origin_current->x;
        value_contact->point.z = origin_current->z + (((u16)plankTopOffset * value_contact->normal.z) >> 12);
    } else {
        sine_local = g_sinTable4096[(u16)tilt & 0xfff];
        value_contact->normal.x = sine_local;
        value_contact->normal.z = 0;
        value_contact->point.x = origin_current->x + (((u16)plankTopOffset * value_contact->normal.x) >> 12);
        value_contact->point.z = origin_current->z;
    }
    cosine_temp = g_pCosTable[(u16)tilt & 0xfff];
    value_contact->normal.y = -cosine_temp;
    value_contact->point.y = origin_current->y + (((u16)plankTopOffset * value_contact->normal.y) >> 12);
    value_contact->obj = this;
    slopeLocal = (halfLength * sine_local) / cosine_temp;
    if ((u8)mode & CQ_FROM_GROUND_QUERY) {
        if (overlap_local) {
            /* The first and third clamp locals are distances along the
             * plank axis; the remaining four are vertical bounds. */
            if (axisIsZ) {
                if (local_moved.min.z > footprint.min[2])
                    currentMinX = local_moved.min.z;
                else
                    currentMinX = footprint.min[2];
                if (local_moved.max.z < footprint.max[2])
                    minZ = local_moved.max.z;
                else
                    minZ = footprint.max[2];
            } else {
                if (local_moved.min.x > footprint.min[0])
                    currentMinX = local_moved.min.x;
                else
                    currentMinX = footprint.min[0];
                if (local_moved.max.x < footprint.max[0])
                    minZ = local_moved.max.x;
                else
                    minZ = footprint.max[0];
            }
            currentMinX -= pivotAxisCoord;
            minZ -= pivotAxisCoord;
            maxZ = (currentMinX * sine_local) / cosine_temp;
            maxY = (minZ * sine_local) / cosine_temp;
            if (maxY < maxZ)
                maxZ = maxY;
            maxZ += pivotY;
            if (raisedBox->max[1] < local_moved.max.y)
                maxX = raisedBox->max[1];
            else
                maxX = local_moved.max.y;
            if (raisedBox->min[1] > maxZ)
                minY = raisedBox->min[1];
            else
                minY = maxZ;
            if (minY <= maxX) {
                temp_hits++;
                *outY = minY - 1;
                if (*nContacts < 16) {
                    value_contact->normal.x = 0;
                    value_contact->normal.y = 0;
                    value_contact->normal.z = 0x1000;
                    value_contact->point.x = raisedBox->max[0];
                    value_contact->point.y = raisedBox->max[1];
                    value_contact->point.z = raisedBox->max[2];
                    value_contact->obj = this;
                    (*nContacts)++;
                    value_contact++;
                }
                return temp_hits;
            }
        }
        return 0;
    }
    if (overlap_local) {
        if (axisIsZ) {
            temp_vertices[0].x = (value_contact->point.x - halfWidth) << 4;
            temp_vertices[0].y = -(value_contact->point.y + slopeLocal) << 4;
            temp_vertices[0].z = -raisedBox->max[2] << 4;
            temp_vertices[1].x = (value_contact->point.x + halfWidth) << 4;
            temp_vertices[1].y = temp_vertices[0].y;
            temp_vertices[1].z = temp_vertices[0].z;
            temp_vertices[2].x = temp_vertices[1].x;
            temp_vertices[2].y = -(value_contact->point.y - slopeLocal) << 4;
            temp_vertices[2].z = -raisedBox->min[2] << 4;
            temp_vertices[3].x = temp_vertices[0].x;
            temp_vertices[3].y = temp_vertices[2].y;
            temp_vertices[3].z = temp_vertices[2].z;
            normal.x = value_contact->normal.x;
            normal.y = -value_contact->normal.y;
            normal.z = -value_contact->normal.z;
        } else {
            temp_vertices[2].x = raisedBox->max[0] << 4;
            temp_vertices[2].y = -(value_contact->point.y + slopeLocal) << 4;
            temp_vertices[2].z = -(value_contact->point.z - halfWidth) << 4;
            temp_vertices[0].x = raisedBox->min[0] << 4;
            temp_vertices[0].y = -(value_contact->point.y - slopeLocal) << 4;
            temp_vertices[0].z = -(value_contact->point.z + halfWidth) << 4;
            temp_vertices[1].x = temp_vertices[2].x;
            temp_vertices[1].y = temp_vertices[2].y;
            temp_vertices[1].z = -(value_contact->point.z + halfWidth) << 4;
            temp_vertices[3].x = temp_vertices[0].x;
            temp_vertices[3].y = temp_vertices[0].y;
            temp_vertices[3].z = -(value_contact->point.z - halfWidth) << 4;
            normal.x = value_contact->normal.x;
            normal.y = -value_contact->normal.y;
            normal.z = -value_contact->normal.z;
        }
        temp_detectorDelta.x = disp->x << 4;
        temp_detectorDelta.y = -disp->y << 4;
        temp_detectorDelta.z = -disp->z << 4;
        detectorBox_current.min[0] = mover->min.x << 4;
        detectorBox_current.max[0] = mover->max.x << 4;
        detectorBox_current.min[1] = -mover->max.y << 4;
        detectorBox_current.max[1] = -mover->min.y << 4;
        detectorBox_current.min[2] = -mover->max.z << 4;
        detectorBox_current.max[2] = -mover->min.z << 4;
        tempSavedVertices[0].x = temp_vertices[0].x;
        tempSavedVertices[0].y = temp_vertices[0].y;
        tempSavedVertices[0].z = temp_vertices[0].z;
        tempSavedVertices[1].x = temp_vertices[1].x;
        tempSavedVertices[1].y = temp_vertices[1].y;
        tempSavedVertices[1].z = temp_vertices[1].z;
        tempSavedVertices[2].x = temp_vertices[2].x;
        tempSavedVertices[2].y = temp_vertices[2].y;
        tempSavedVertices[2].z = temp_vertices[2].z;
        tempSavedVertices[3].x = temp_vertices[3].x;
        tempSavedVertices[3].y = temp_vertices[3].y;
        tempSavedVertices[3].z = temp_vertices[3].z;
        currentHeight = 0x80000000;
        if (Vec3s_Dot(&value_contact->normal, disp) <= 0) {
            local_result = Collide_BoxTriangleSweep(&detectorBox_current, &temp_detectorDelta, temp_vertices, 4,
                                                    outFrac, &currentHeight, &normal);
            if (local_result) {
                temp_hits++;
                currentHeight = (-currentHeight >> 4) - 1;
                *outY = currentHeight;
                AddBody(querier, (disp->y << 10) / g_dtMs);
                if (*nContacts < 16) {
                    (*nContacts)++;
                    value_contact++;
                }
            }
        }
        normal.x = 0;
        normal.y = 0;
        normal.z = -0x1000;
        if (normal.z * disp->z >= 0) {
            temp_vertices[0].x = tempSavedVertices[1].x;
            temp_vertices[0].y = -raisedBox->max[1] << 4;
            temp_vertices[0].z = tempSavedVertices[1].z;
            temp_vertices[1].x = tempSavedVertices[1].x;
            temp_vertices[1].y = tempSavedVertices[1].y;
            temp_vertices[1].z = tempSavedVertices[1].z;
            temp_vertices[2].x = tempSavedVertices[0].x;
            temp_vertices[2].y = tempSavedVertices[0].y;
            temp_vertices[2].z = tempSavedVertices[0].z;
            temp_vertices[3].x = temp_vertices[2].x;
            temp_vertices[3].y = temp_vertices[0].y;
            temp_vertices[3].z = tempSavedVertices[0].z;
            local_result = Collide_BoxTriangleSweep(&detectorBox_current, &temp_detectorDelta, temp_vertices, 4,
                                                    &tempFraction, &currentHeight, &normal);
            if (local_result) {
                temp_hits++;
                if (tempFraction < *outFrac)
                    *outFrac = tempFraction;
                currentHeight = (-currentHeight >> 4) - 1;
                if (currentHeight < *outY)
                    *outY = currentHeight;
                if (*nContacts < 16) {
                    value_contact->normal.x = 0;
                    value_contact->normal.y = 0;
                    value_contact->normal.z = 0x1000;
                    value_contact->point.x = raisedBox->max[0];
                    value_contact->point.y = raisedBox->max[1];
                    value_contact->point.z = raisedBox->max[2];
                    value_contact->obj = this;
                    (*nContacts)++;
                    value_contact++;
                }
            }
        }
        normal.x = 0;
        normal.y = 0;
        normal.z = 0x1000;
        if (normal.z * disp->z >= 0) {
            temp_vertices[0].x = tempSavedVertices[2].x;
            temp_vertices[0].y = -raisedBox->max[1] << 4;
            temp_vertices[0].z = tempSavedVertices[2].z;
            temp_vertices[1].x = tempSavedVertices[2].x;
            temp_vertices[1].y = tempSavedVertices[2].y;
            temp_vertices[1].z = tempSavedVertices[2].z;
            temp_vertices[2].x = tempSavedVertices[3].x;
            temp_vertices[2].y = tempSavedVertices[3].y;
            temp_vertices[2].z = tempSavedVertices[3].z;
            temp_vertices[3].x = temp_vertices[2].x;
            temp_vertices[3].y = temp_vertices[0].y;
            temp_vertices[3].z = tempSavedVertices[3].z;
            local_result = Collide_BoxTriangleSweep(&detectorBox_current, &temp_detectorDelta, temp_vertices, 4,
                                                    &tempFraction, &currentHeight, &normal);
            if (local_result) {
                temp_hits++;
                if (tempFraction < *outFrac)
                    *outFrac = tempFraction;
                currentHeight = (-currentHeight >> 4) - 1;
                if (currentHeight < *outY)
                    *outY = currentHeight;
                if (*nContacts < 16) {
                    value_contact->normal.x = 0;
                    value_contact->normal.y = 0;
                    value_contact->normal.z = -0x1000;
                    value_contact->point.x = raisedBox->min[0];
                    value_contact->point.y = raisedBox->min[1];
                    value_contact->point.z = raisedBox->min[2];
                    value_contact->obj = this;
                    (*nContacts)++;
                    value_contact++;
                }
            }
        }
        normal.x = -0x1000;
        normal.y = 0;
        normal.z = 0;
        if (Vec3s_Dot(&normal, disp) <= 0) {
            temp_vertices[0].x = tempSavedVertices[0].x;
            temp_vertices[0].y = -raisedBox->max[1] << 4;
            temp_vertices[0].z = tempSavedVertices[0].z;
            temp_vertices[1].x = tempSavedVertices[0].x;
            temp_vertices[1].y = tempSavedVertices[0].y;
            temp_vertices[1].z = tempSavedVertices[0].z;
            temp_vertices[2].x = tempSavedVertices[3].x;
            temp_vertices[2].y = tempSavedVertices[3].y;
            temp_vertices[2].z = tempSavedVertices[3].z;
            temp_vertices[3].x = temp_vertices[2].x;
            temp_vertices[3].y = temp_vertices[0].y;
            temp_vertices[3].z = tempSavedVertices[3].z;
            local_result = Collide_BoxTriangleSweep(&detectorBox_current, &temp_detectorDelta, temp_vertices, 4,
                                                    &tempFraction, &currentHeight, &normal);
            if (local_result) {
                temp_hits++;
                if (tempFraction < *outFrac)
                    *outFrac = tempFraction;
                currentHeight = (-currentHeight >> 4) - 1;
                if (currentHeight < *outY)
                    *outY = currentHeight;
                if (*nContacts < 16) {
                    value_contact->normal.x = -0x1000;
                    value_contact->normal.y = 0;
                    value_contact->normal.z = 0;
                    value_contact->point.x = raisedBox->min[0];
                    value_contact->point.y = raisedBox->min[1];
                    value_contact->point.z = raisedBox->min[2];
                    value_contact->obj = this;
                    (*nContacts)++;
                    value_contact++;
                }
            }
        }
        normal.x = 0x1000;
        normal.y = 0;
        normal.z = 0;
        if (Vec3s_Dot(&normal, disp) <= 0) {
            temp_vertices[0].x = tempSavedVertices[1].x;
            temp_vertices[0].y = -raisedBox->max[1] << 4;
            temp_vertices[0].z = tempSavedVertices[1].z;
            temp_vertices[1].x = tempSavedVertices[1].x;
            temp_vertices[1].y = tempSavedVertices[1].y;
            temp_vertices[1].z = tempSavedVertices[1].z;
            temp_vertices[2].x = tempSavedVertices[2].x;
            temp_vertices[2].y = tempSavedVertices[2].y;
            temp_vertices[2].z = tempSavedVertices[2].z;
            temp_vertices[3].x = temp_vertices[2].x;
            temp_vertices[3].y = temp_vertices[0].y;
            temp_vertices[3].z = tempSavedVertices[2].z;
            local_result = Collide_BoxTriangleSweep(&detectorBox_current, &temp_detectorDelta, temp_vertices, 4,
                                                    &tempFraction, &currentHeight, &normal);
            if (local_result) {
                temp_hits++;
                if (tempFraction < *outFrac)
                    *outFrac = tempFraction;
                currentHeight = (-currentHeight >> 4) - 1;
                if (currentHeight < *outY)
                    *outY = currentHeight;
                if (*nContacts < 16) {
                    value_contact->normal.x = 0x1000;
                    value_contact->normal.y = 0;
                    value_contact->normal.z = 0;
                    value_contact->point.x = raisedBox->max[0];
                    value_contact->point.y = raisedBox->max[1];
                    value_contact->point.z = raisedBox->max[2];
                    value_contact->obj = this;
                    (*nContacts)++;
                    value_contact++;
                }
            }
        }
        normal.x = 0;
        normal.y = -0x1000;
        normal.z = 0;
        if (normal.y * disp->y >= 0) {
            localBottomY = -raisedBox->max[1] << 4;
            temp_vertices[0].x = tempSavedVertices[0].x;
            temp_vertices[0].y = localBottomY;
            temp_vertices[0].z = tempSavedVertices[0].z;
            temp_vertices[1].x = tempSavedVertices[1].x;
            temp_vertices[1].y = localBottomY;
            temp_vertices[1].z = tempSavedVertices[1].z;
            temp_vertices[2].x = tempSavedVertices[2].x;
            temp_vertices[2].y = localBottomY;
            temp_vertices[2].z = tempSavedVertices[2].z;
            temp_vertices[3].x = tempSavedVertices[3].x;
            temp_vertices[3].y = localBottomY;
            temp_vertices[3].z = tempSavedVertices[3].z;
            local_result = Collide_BoxTriangleSweep(&detectorBox_current, &temp_detectorDelta, temp_vertices, 4,
                                                    &tempFraction, &currentHeight, &normal);
            if (local_result) {
                temp_hits++;
                if (tempFraction < *outFrac)
                    *outFrac = tempFraction;
                currentHeight = (-currentHeight >> 4) - 1;
                if (currentHeight < *outY)
                    *outY = currentHeight;
                if (*nContacts < 16) {
                    value_contact->normal.x = 0;
                    value_contact->normal.y = 0x1000;
                    value_contact->normal.z = 0;
                    value_contact->point.x = raisedBox->max[0];
                    value_contact->point.y = raisedBox->max[1];
                    value_contact->point.z = raisedBox->max[2];
                    value_contact->obj = this;
                    (*nContacts)++;
                    value_contact++;
                }
            }
        }
    }
    return temp_hits;
}

/* 0x4eeb9c */
/* BYTES(slot-name, inferred): local names chosen for their stack slots (tools/vc6_locals.py), not recovered */
void seesaw::AddBody(ScnObject *obj, s32 landingSpeed)
{
    u16 iValue;
    s32 valueCanLaunch;
    s32 forceLaunch;
    valueCanLaunch = 1;
    if (obj->InstFlags(INST_F_ATTACHED))
        return;
    if (bodyCount >= 32)
        return;
    if (IsLaunching(obj))
        return;
    if (landingSpeed <= 0)
        landingSpeed = 0;
    for (iValue = 0; iValue < bodyCount; iValue++) {
        if (bodies[iValue].obj == obj) {
            if (landingSpeed > bodies[iValue].landingSpeed && bodies[iValue].canLaunch)
                bodies[iValue].landingSpeed = landingSpeed;
            return;
        }
    }
    bodies[bodyCount].obj = obj;
    if (landingSpeed && landingSpeed < 512)
        landingSpeed = 512;
    if (obj->GetClassId() == CLASSID_WOLF && landingSpeed > 0) {
        s32 local_distance;
        s32 localNegative;
        s32 positive;
        if (!axisIsZ)
            local_distance = obj->pos.x - centreOnAxis;
        else
            local_distance = obj->pos.z - centreOnAxis;
        localNegative = local_distance < -30;
        positive = local_distance > 30;
        if ((ejectionBox1 && localNegative) || (ejectionBox2 && positive)) {
            valueCanLaunch = 0;
            if ((ejectionBox1 && localNegative && ejectionTimer1 > 0) ||
                (ejectionBox2 && positive && ejectionTimer2 > 0)) {
                if (localNegative)
                    pendingLaunch.negative = 1;
                if (positive)
                    pendingLaunch.positive = 1;
                landingSpeed = 48000;
            } else
                landingSpeed = 512;
            ejectionTimer2 = 0;
            ejectionTimer1 = 0;
        } else {
            ejectionTimer2 = 0;
            ejectionTimer1 = 0;
        }
    } else if (obj->GetClassId() != CLASSID_WOLF) {
        s32 lastDistance_local;
        s32 index;
        s32 hop = 0;
        forceLaunch = 0;
        if (obj->GetClassId() == CLASSID_SHEEP) {
            index = FindLaunch(obj);
            if (index != -1)
                hop = launches[index].flags.seesawBit3;
        }
        if (!hop && (forceLaunch || obj->HandleMessage(this, MSG_QUERY_MOVING, 0) == 1)) {
            if (!axisIsZ)
                lastDistance_local = obj->pos.x - centreOnAxis;
            else
                lastDistance_local = obj->pos.z - centreOnAxis;
            pendingLaunch.negative |= lastDistance_local < -30;
            pendingLaunch.positive |= lastDistance_local > 30;
            pendingLaunch.rock = 1;
        }
    }
    bodies[bodyCount].canLaunch = valueCanLaunch;
    bodies[bodyCount].landingSpeed = landingSpeed;
    bodyCount++;
}

/* 0x4ef04e */
void seesaw::RemoveLaunch(s32 index)
{
    launches[index].obj->HandleMessage(this, MSG_LANDED, (void *)1); /* cast kept: an argument is a void * */
    for (u16 i = index + 1; i < launchCount; i++) {
        launches[i - 1] = launches[i];
        launchPos[i - 1] = launchPos[i];
    }
    launchCount--;
}

/* 0x4ef12f */
void seesaw::ArcCoeffsFrom3Points(s32 p0, s32 pMid, s32 pEnd, s32 *outP0, s32 *outV, s32 *outA)
{
    *outP0 = p0;
    *outV = -3 * p0 - pEnd + 4 * pMid;
    *outA = p0 + pEnd - 2 * pMid;
}

/* 0x4ef16a. The holder position is intentionally read and unused. The
 * one-bit freeze assignment retains the original (flags & 2) truncation. */
/* BYTES(dead-code): the holder position is read and unused, as in the original */
/* BYTES(cast): the one-bit store keeps the (flags & 2) truncation, as the original */
void seesaw::LaunchObject(ScnObject *obj, const Trajectory3 *trajectory, CamSetup *camera, u32 camParam, u32 hop)
{
    Vec3s args;
    LaunchArc *current;
    if (launchCount >= 32)
        return;
    if (IsHeld(obj, &args))
        return;
    if (FindLaunch(obj) != -1)
        return;
    if (obj->GetClassId() == CLASSID_WOLF) {
        ejectionTimer2 = 0;
        ejectionTimer1 = 0;
    }
    current = &launches[launchCount];
    current->obj = obj;
    current->t = 0;
    current->camera = camera;
    current->camParam = camParam;
    current->flags.noFreeze = seesawFlags & SEESAW_F_NO_FREEZE;
    current->flags.planeYZ = 0;
    current->flags.full3D = 1;
    current->flags.seesawBit3 = hop;
    launchPos[launchCount] = obj->pos;
    ArcCoeffsFrom3Points(obj->pos.x, trajectory->pts[1].x, trajectory->pts[2].x, &current->coeffs[0],
                         &current->coeffs[1], &current->coeffs[2]);
    ArcCoeffsFrom3Points(obj->pos.y, trajectory->pts[1].y, trajectory->pts[2].y, &current->coeffs[3],
                         &current->coeffs[4], &current->coeffs[5]);
    ArcCoeffsFrom3Points(obj->pos.z, trajectory->pts[1].z, trajectory->pts[2].z, &current->coeffs[6],
                         &current->coeffs[7], &current->coeffs[8]);
    if (!obj->HandleMessage(this, MSG_LAUNCH, current))
        launchCount++;
}

/* 0x4ef3fe */
/* BYTES(slot-name, inferred): local names chosen for their stack slots (tools/vc6_locals.py), not recovered */
void seesaw::UpdateLaunches()
{
    Vec3s *currentPrevious;
    ContactInfo contactTemp;
    s32 localDone;
    Vec3s localDelta;
    LaunchArc *arcCurrent;
    u16 localI;
    Vec3s localPoint;
    arcCurrent = launches;
    for (localI = 0; localI < launchCount; localI++, arcCurrent++) {
        localDone = 0;
        if (arcCurrent->Step(g_dtMs >> 2, &localPoint.x, &localPoint.y, &localPoint.z)) {
            if (arcCurrent->obj->GetFirstModelBox()) {
                currentPrevious = &launchPos[localI];
                localDelta.x = localPoint.x - arcCurrent->obj->pos.x;
                localDelta.y = localPoint.y - arcCurrent->obj->pos.y;
                localDelta.z = localPoint.z - arcCurrent->obj->pos.z;
                if (arcCurrent->obj->Collide_ResolveMove(&localDelta, &contactTemp, 0xb54, RESOLVE_SLIDE_ALL, 0, 0, 10,
                                                         0, 0)) {
                    if (contactTemp.movableObj) { /* cast kept (both): the kill cause is a void * argument */
                        contactTemp.movableObj->HandleMessage(this, MSG_KILL, (void *)KILL_CRUSH);
                        arcCurrent->obj->HandleMessage(this, MSG_KILL, (void *)KILL_CRUSH);
                    }
                    localDone = 1;
                }
                arcCurrent->obj->Translate(&localDelta);
                *currentPrevious = localPoint;
            } else {
                arcCurrent->obj->SetPosition(&localPoint);
                launchPos[localI] = localPoint;
            }
            if (arcCurrent->camera && g_camMode != CAM_SCRIPTED) {
                camShot.Init(seesawFlags & SEESAW_F_NO_FREEZE);
                camShot.Start(arcCurrent->camera, this, arcCurrent->camParam);
                arcCurrent->camera = 0;
            }
        } else
            localDone = 1;
        if (localDone) {
            RemoveLaunch(localI);
            localI--;
            arcCurrent--;
        }
    }
    camShot.Update(this);
}

/* ---- CamShot: the launch camera shots and their skip input (its SDW_MEMBERS_CamShot and SDW_MEMBERS_Pad are at the
   top of the file) ---- */
extern Wolf *g_pWolf;
extern s32 g_dtMs;
void Camera_StartScripted(ScnObject *owner, Camera *cam, u16 rotX, u16 rotY, u16 rotZ, Vec3s *pos, u16 focal, u32 mode,
                          s32 time);

#define SDW_INLINE_FREE_CAMERA_ISSCRIPTCONTROLLED 1
#include "camera_inlines.h"
#undef SDW_INLINE_FREE_CAMERA_ISSCRIPTCONTROLLED

/* 0x4ef717 */
s32 Pad_AnySkipInput(Pad *pad)
{
    s32 h = 0;
    s32 v = 0;
    if (pad->cur.typeLen.type == PADTYPE_ANALOG) {
        Pad_AnalogToStick(pad->cur.leftX, pad->cur.leftY, &h, &v);
        v = -v;
        if (h || v)
            return 1;
    }
    if (g_pad.GetChangedPress((u16) ~(PAD_UP | PAD_RIGHT | PAD_DOWN | PAD_LEFT)) != PAD_ALL_RELEASED)
        return 1;
    if ((!(pad->cur.buttons & ~(u16)~PAD_CROSS) && (pad->prev.buttons & ~(u16)~PAD_CROSS)) ||
        (!(pad->cur.buttons & ~(u16)~PAD_CIRCLE) && (pad->prev.buttons & ~(u16)~PAD_CIRCLE)) ||
        (!(pad->cur.buttons & ~(u16)~PAD_SQUARE) && (pad->prev.buttons & ~(u16)~PAD_SQUARE)) ||
        (!(pad->cur.buttons & ~(u16)~PAD_TRIANGLE) && (pad->prev.buttons & ~(u16)~PAD_TRIANGLE)) ||
        (!(pad->cur.buttons & ~(u16)~PAD_L1) && (pad->prev.buttons & ~(u16)~PAD_L1)) ||
        (!(pad->cur.buttons & ~(u16)~PAD_R1) && (pad->prev.buttons & ~(u16)~PAD_R1)) ||
        (!(pad->cur.buttons & ~(u16)~PAD_L2) && (pad->prev.buttons & ~(u16)~PAD_L2)) ||
        (!(pad->cur.buttons & ~(u16)~PAD_R2) && (pad->prev.buttons & ~(u16)~PAD_R2)))
        return 1;
    return 0;
}

/* 0x4ef8c9 */
void CamShot::Init(u32 flags)
{
    cam = 0;
    elapsedMs = 0;
    wolfFrozen = 0;
    noFreeze = flags;
}

/* 0x4ef8f9 */
void CamShot::Stop(ScnObject *owner)
{
    if (cam) {
        if (wolfFrozen)
            g_pWolf->HandleMessage(owner, MSG_UNFREEZE, 0);
        if (Camera_IsScriptControlled())
            Camera_ReleaseAny();
        Init(noFreeze);
    }
}

/* 0x4ef982 */
void CamShot::Update(ScnObject *owner)
{
    if (cam) {
        elapsedMs += g_dtMs;
        if (elapsedMs >= 1000 && Pad_AnySkipInput(&g_pad)) {
            if (wolfFrozen)
                g_pWolf->HandleMessage(owner, MSG_UNFREEZE, 0);
            if (Camera_IsScriptControlled())
                Camera_ReleaseAny();
            Init(noFreeze);
            return;
        }
        Camera_StartScripted(0, &g_camera, cam->rot[0], cam->rot[1], cam->rot[2], &cam->eye, cam->focal, param, 0x1000);
    }
}

/* 0x4efa8f */
void CamShot::Start(CamSetup *setup, ScnObject *owner, u32 flags)
{
    cam = setup;
    elapsedMs = 0;
    param = flags;
    if (!noFreeze && g_pWolf->HandleMessage(owner, MSG_FREEZE, 0))
        wolfFrozen = 1;
    else
        wolfFrozen = 0;
}
