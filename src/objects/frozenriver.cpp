/*
 * FrozenRiver (class 83 "FrozenRiver", vtable 0x5761ec, sizeof 0xe4) - the current of the frozen river in Level 7
 * (disc Lvl-08): it carries whatever falls into its water boxes - the Wolf, a frozen Sheep, and the level's ice floes
 * (SlidingIceCubes, launched one by one) - along its path (IDWATERTRAJ) to the Crane, and calls the Crane when one
 * reaches the last point. SheepD3D.exe 0x4c2980-0x4c3ea3, the whole file (it starts 16-byte aligned with the factory
 * and ends at 0x4c3ea3, followed by int3 padding). The last two functions, 0x4c3bc7 and 0x4c3c03, are listed under
 * GeyserIn by tools/partition.py, but they are FrozenRiver methods (this = FrozenRiver, called only by
 * FrozenRiver_UpdateCargo and FrozenRiver_Update) and sit before the padding that closes this file.
 *
 * A cargo slot (RiverCargo) moves toward path point `node` at 150 units/s on x/z and keeps its height; within 50 of
 * the point it takes the next one, unless another cargo is moving toward it (then it waits). At the last point it
 * calls the Crane (msg 0x37) unless the crane is busy; the Crane answers with msg 0x3e 1 when its hook takes the
 * cargo, which frees the slot, and 0x3e 0 when it is done. Every cargo also turns by 8/4096 of a turn per frame.
 *
 * Devices that only pin the original code generation (the inline helpers have no bodies, so their names are not
 * recovered):
 *  - Scn_GetPropU32 takes the offset as u32: the int constant then gets a stack temp of its own (0x4c2c62), as in
 *    the original; with an s32 parameter VC6 substitutes the constant.
 *  - ZoneList::Load / Clear / FindContainingXZ, SetVisible, SetCollidable, SetFacing and PlayAnim are inline
 *    methods whose `this` is a stack temp where the original has one (0x4c2c84, 0x4c2caa, 0x4c2ee5, 0x4c2be9 ...).
 *  - Local names are chosen for their stack slots (tools/vc6_locals.py): cubes/rec/boxId/i, best/i/minDist,
 *    toNode/move/velocity.
 */

#define SDW_MEMBERS_ScnObject            \
    static void *operator new(u32 size); \
    void SetFacing(s16 f);               \
    /* inline, defined below */          \
    void SetUpdateMode(s32 mode); /* inline, defined below */

#define SDW_MEMBERS_ZoneList \
    void Load(u32 id);       \
    /* inline, defined below */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *inst, Animator *animator, u16 id, u32 opts); /* 0x550196 (used by the PlayAnim inline) */
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_SETFACING_S16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_SETFACING_S16
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#include "../engine/property_math.h"

extern Wolf *g_pWolf; /* 0x6cf310 */
#include "../app/app_main.h"
#include "../engine/id_list.h"
#include "../engine/scn_tools.h"
#include "../engine/maths.h"
extern s32 g_dtMs; /* 0x71b2e8  g_dt * 1000 >> 12 */

s32 Scenaric_FindByClass(u16 classId, ScnObject **out, s32 max); /* 0x5145c5 */
void Camera_StartScripted(ScnObject *owner, Camera *cam, u16 rotX, u16 rotY, u16 rotZ, Vec3s *pos, u16 focal, u32 mode,
                          s32 time); /* 0x55a70d */

/* ---- inline helpers ---- */

/* A u32 designer property: the record's property block starts at +0x14. */
#define SDW_INLINE_FREE_SCN_GETPROPU32_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_SCN_GETPROPU32_VOID_U32

/* The boxes of an id list (an ID...BOX property). */
#define SDW_INLINE_ZONELIST_LOAD_U32 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_LOAD_U32

#define SDW_INLINE_ZONELIST_CLEAR 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_CLEAR

/* The first box of the list that contains p on x and z, or 0. */
#define SDW_INLINE_ZONELIST_FINDCONTAININGXZ_VEC3S 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_FINDCONTAININGXZ_VEC3S

/* SCN_OF_HIDDEN (0x800) off or on. */
#define SDW_INLINE_SCNOBJECT_SETVISIBLE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETVISIBLE_S32

/* SCN_OF_NO_BOX_COLLIDE (0x400) off or on. */
#define SDW_INLINE_SCNOBJECT_SETCOLLIDABLE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETCOLLIDABLE_S32

/* A CAMERA property {u16 focal; s16 rot[3]; Vec3s eye} as a scripted camera owned by this object. */
#define SDW_INLINE_SCNOBJECT_STARTCAMERABLENDED_U16_U16_U16_VEC3S_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_STARTCAMERABLENDED_U16_U16_U16_VEC3S_U16

/* ScnUpdateMode: 0 near the camera only, 1 always, 2 never, 3 also during cinematics. */
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32

/* ---- FrozenRiver ---- */

/* 0x4c2980 - the class factory for CLASSID 83 "FrozenRiver": new FrozenRiver (the ScnObject, ScnLogic and FrozenRiver
 * vtables in turn), then ScnLogic_Init(record) through the vtable. */
ScnObject *FrozenRiver_Create(void *record)
{
    ScnObject *obj = new FrozenRiver;
    obj = ((ScnLogic *)obj)->Init(record); /* cast kept: a downcast: obj was just made as a FrozenRiver, a ScnLogic */
    return obj;
}

/* 0x4c29e5 - vtable +0x00: reads the path, the camera and the water boxes, finds the Crane (none disables the river)
 * and the SlidingIceCubes (hidden and not solid until launched), and always updates. The floe timer is set to 10000
 * and then to 0 (so the first floe comes 10 s after the start); a restart (Reset) leaves it at 10000 instead. */
void FrozenRiver::PostLoadInit()
{
    ScnObject *cubes[3];
    u32 boxId;
    u16 *rec;
    s32 i;

    enabled = 1;
    rec = record;
    path = Scn_GetPropTrajectory(rec, 8); /* PROPERTY_FROZENRIVER_IDWATERTRAJ */
    floeStart = path->pts[0];
    for (i = 0; i < 2; i++) {
        cargo[i].obj = 0;
        cargo[i].moving = 0;
        cargo[i].waitingCrane = 0;
        cargo[i].done = 0;
    }
    for (i = 0; i < 3; i++) {
        floes[i].obj = 0;
        floes[i].moving = 0;
        floes[i].active = 0;
        floes[i].done = 0;
        floes[i].waitingCrane = 0;
    }
    floeOnCrane = 0;
    camera = Scn_GetPropCamera(rec, 0); /* PROPERTY_FROZENRIVER_CAMERA */
    crane = 0;
    Scenaric_FindByClass(CLASSID_CRANE, &crane, 1);
    if (!crane)
        enabled = 0;
    floeCount = Scenaric_FindByClass(CLASSID_SLIDINGICECUBE, cubes, 3);
    for (i = 0; i < floeCount; i++) {
        floes[i].obj = cubes[i];
        if (floes[i].obj) {
            floes[i].obj->SetVisible(0);
            floes[i].obj->SetCollidable(0);
        }
    }
    boxId = Scn_GetPropU32(rec, 4); /* PROPERTY_FROZENRIVER_IDWATERBOX */
    if (boxId)
        waterBoxes.Load(boxId);
    else
        waterBoxes.Clear();
    if (waterBoxes.count > 0)
        floeStart.y = waterBoxes.boxes[0]->min[1] + 110;
    else
        floeStart.y = 110;
    floesActive = 0;
    sheepInRiver = 0;
    wolfInRiver = 0;
    craneBusy = 0;
    SetVisible(0);
    SetCollidable(0);
    SetUpdateMode(SCN_UPD_ALWAYS);
    floeTimer = 10000;
    camStarted = 0;
    floeTimer = 0;
}

/* 0x4c2e8b - vtable +0x04: launch floes and move the cargo, while enabled. */
void FrozenRiver::Update()
{
    switch (enabled) {
        case 1:
            LaunchFloe();
            UpdateCargo();
    }
}

/* 0x4c2ebc - vtable +0x10. Msg 0x36 (sent by whatever lands in the water: the Wolf, a frozen Sheep, a floe dropped by
 * the crane): if the sender stands in one of the water boxes (x/z) it is taken on as cargo (FrozenRiver_AddCargo) and
 * the reply is 1. Msg 0x3e (from the Crane, arg bit 0 = its hook is busy): when set, the first Wolf/Sheep cargo moving
 * at the last point is let go (the crane has it) and every floe at the last point is taken out of the water. */
s32 FrozenRiver::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    s32 i;

    switch (msgId) {
        case MSG_RIVER_ADD_CARGO:
            if (waterBoxes.FindContainingXZ(&sender->pos)) {
                AddCargo(sender);
                return 1;
            }
            return 0;
        case MSG_RIVER_CRANE_BUSY:
            craneBusy = (u8)(u32)arg; /* cast kept: the message arg is a void *; the Crane passes its busy flag in it */
            if (craneBusy) {
                for (i = 0; i < 2; i++) {
                    camStarted = 0;
                    if (cargo[i].obj && cargo[i].node == path->count - 1 && cargo[i].moving) {
                        if (cargo[i].obj->GetClassId() == CLASSID_WOLF)
                            wolfInRiver = 0;
                        else
                            sheepInRiver = 0;
                        cargo[i].moving = 0;
                        cargo[i].done = 1;
                        cargo[i].waitingCrane = 0;
                        break;
                    }
                }
                for (i = 0; i < floeCount; i++) {
                    if (floes[i].obj && floes[i].node == path->count - 1) {
                        floesActive--;
                        floes[i].active = 0;
                        floes[i].moving = 0;
                        floeOnCrane = 1;
                        floes[i].done = 1;
                        /* cast kept: a downcast: the floes are SlidingIceCubes, ScnBody objects */
                        ((ScnBody *)floes[i].obj)->PlayAnim(AGLACON1_ANIM_STAND2, 1, 0);
                        floeTimer = 0;
                    }
                }
            }
            break;
    }
    return 0;
}

/* 0x4c3187 - vtable +0x14 (level restart): empties every slot, finds and hides the floes again, clears every flag and
 * sets the floe timer to 10000 (the first floe after a restart comes on the next frame). */
void FrozenRiver::Reset()
{
    ScnObject *cubes[3];
    s32 i;

    for (i = 0; i < 2; i++) {
        cargo[i].obj = 0;
        cargo[i].moving = 0;
        cargo[i].active = 0;
        cargo[i].done = 0;
        cargo[i].waitingCrane = 0;
    }
    wolfInRiver = 0;
    sheepInRiver = 0;
    for (i = 0; i < floeCount; i++) {
        floes[i].obj = 0;
        floes[i].moving = 0;
        floes[i].active = 0;
        floes[i].waitingCrane = 0;
        floes[i].done = 0;
    }
    floeOnCrane = 0;
    floeCount = Scenaric_FindByClass(CLASSID_SLIDINGICECUBE, cubes, 3);
    for (i = 0; i < floeCount; i++) {
        floes[i].obj = cubes[i];
        if (floes[i].obj) {
            floes[i].obj->SetVisible(0);
            floes[i].obj->SetCollidable(0);
        }
    }
    craneBusy = 0;
    sheepInRiver = 0;
    wolfInRiver = 0;
    floesActive = 0;
    floeTimer = 10000;
}

/* 0x4c340c - the path point after the one nearest to p (x/z), or the last point if that is the nearest. */
u16 FrozenRiver::FindNextNode(Vec3s *p)
{
    u16 minDist;
    u16 i;
    u16 best;

    best = 0;
    minDist = Vec3s_DistXZ(p, &path->pts[0]);
    for (i = 1; i < path->count; i++) {
        u16 dist = Vec3s_DistXZ(p, &path->pts[i]);
        if (dist < minDist) {
            minDist = dist;
            best = i;
        }
    }
    if (best == path->count - 1)
        return best;
    return ++best;
}

/* 0x4c34ca - take on a cargo: the Wolf into slot 0 and a Sheep into slot 1 (one of each at a time), heading for the
 * point after the nearest one; returns 1 if taken. A floe (dropped back by the crane) only clears floeOnCrane. */
s32 FrozenRiver::AddCargo(ScnObject *obj)
{
    switch (obj->GetClassId()) {
        case CLASSID_WOLF:
            if (!wolfInRiver) {
                cargo[0].obj = g_pWolf;
                cargo[0].node = FindNextNode(&cargo[0].obj->pos);
                if (IsNodeTaken(cargo[0].node, &cargo[0]))
                    cargo[0].moving = 0;
                else
                    cargo[0].moving = 1;
                wolfInRiver = 1;
                cargo[0].done = 0;
                return 1;
            }
            break;
        case CLASSID_SHEEP:
            if (!sheepInRiver) {
                cargo[1].obj = obj;
                cargo[1].node = FindNextNode(&cargo[1].obj->pos);
                if (IsNodeTaken(cargo[1].node, &cargo[1]))
                    cargo[1].moving = 0;
                else
                    cargo[1].moving = 1;
                sheepInRiver = 1;
                cargo[1].done = 0;
                return 1;
            }
            break;
        case CLASSID_SLIDINGICECUBE:
            floeOnCrane = 0;
            break;
    }
    return 0;
}

/* 0x4c364b - whether a cargo other than self is moving toward path point node. The floes are checked up to floeCount,
 * active or not. */
s32 FrozenRiver::IsNodeTaken(u16 node, RiverCargo *self)
{
    s32 i;

    for (i = 0; i < 2; i++)
        if (cargo[i].obj && &cargo[i] != self && cargo[i].node == node && cargo[i].moving)
            return 1;
    for (i = 0; i < floeCount; i++)
        if (floes[i].obj && &floes[i] != self && floes[i].node == node && floes[i].moving)
            return 1;
    return 0;
}

/* 0x4c373a - one frame of every cargo not yet taken by the crane: turn it, and move it, or start moving again once
 * its point is free. The floes are walked up to floesActive, not floeCount. */
void FrozenRiver::UpdateCargo()
{
    s32 i;

    for (i = 0; i < 2; i++) {
        if (cargo[i].obj && !cargo[i].done) {
            SpinCargo(&cargo[i]);
            if (cargo[i].moving)
                MoveCargo(&cargo[i]);
            else if (!IsNodeTaken(cargo[i].node, &cargo[i]))
                cargo[i].moving = 1;
        }
    }
    for (i = 0; i < floesActive; i++) {
        if (floes[i].obj && !floes[i].done && floes[i].active) {
            SpinCargo(&floes[i]);
            if (floes[i].moving)
                MoveCargo(&floes[i]);
            else if (!IsNodeTaken(floes[i].node, &floes[i]))
                floes[i].moving = 1;
        }
    }
}

/* 0x4c38e4 - move a cargo toward its path point at 150 units/s (x/z, scaled by g_dt; its height is kept) through its
 * SetPosition. Within 50 of the point: the next point if it is free; at the last point the river camera for the Wolf
 * (once) and the call to the Crane (msg 0x37, unless the crane is busy), after which the cargo waits. A floe does
 * nothing at a point while another floe is on the crane. */
void FrozenRiver::MoveCargo(RiverCargo *c)
{
    Vec3s velocity;
    Vec3s move;
    Vec3s toNode;

    toNode.x = path->pts[c->node].x - c->obj->pos.x;
    toNode.z = path->pts[c->node].z - c->obj->pos.z;
    toNode.y = floeStart.y;
    nodeDist = Vec3s_DistXZ(&path->pts[c->node], &c->obj->pos);
    if (c->waitingCrane)
        return;
    if (nodeDist < 50) {
        if (!floeOnCrane || c->obj->GetClassId() != CLASSID_SLIDINGICECUBE) {
            if (c->node != path->count - 1) {
                if (!IsNodeTaken(c->node + 1, c)) {
                    c->moving = 1;
                    c->node++;
                }
            } else {
                if (!camStarted && camera && c->obj == g_pWolf) {
                    StartCameraBlended(camera->rot[0], camera->rot[1], camera->rot[2], &camera->eye, camera->focal);
                    camStarted = 1;
                }
                if (!c->waitingCrane && !craneBusy) {
                    crane->HandleMessage(this, MSG_RIVER_CARGO_END, c->obj);
                    c->waitingCrane = 1;
                }
            }
        }
    } else {
        velocity.x = toNode.x * 150 / (s32)nodeDist;
        velocity.z = toNode.z * 150 / (s32)nodeDist;
        Vec3s_ScaleByDt(&velocity, &move);
        move.y = c->obj->pos.y;
        move.x += c->obj->pos.x;
        move.z += c->obj->pos.z;
        c->obj->SetPosition(&move);
    }
}

/* 0x4c3bc7 - turn a cargo by 8/4096 of a turn: every frame, whatever the frame rate. */
void FrozenRiver::SpinCargo(RiverCargo *c)
{
    s16 facing;

    facing = c->obj->rot.y;
    facing += 8;
    c->obj->SetFacing(facing);
}

/* 0x4c3c03 - once the floe timer passes 10000 ms, no floe is on the crane and not every floe is in the water: put the
 * first inactive floe in at floeStart with a random heading, shown and solid, playing animation 1. The timer counts
 * g_dtMs every frame. */
void FrozenRiver::LaunchFloe()
{
    s16 heading;
    s32 i;

    if (floeTimer > 10000 && !floeOnCrane && floesActive < floeCount) {
        floeTimer = 0;
        floesActive++;
        i = 0;
        while (floes[i].active)
            i++;
        floes[i].done = 0;
        floes[i].obj->SetPosition(&floeStart);
        floes[i].node = FindNextNode(&floes[i].obj->pos);
        if (IsNodeTaken(floes[i].node, &floes[i]))
            floes[i].moving = 0;
        else
            floes[i].moving = 1;
        floes[i].active = 1;
        floes[i].waitingCrane = 0;
        heading = Rand_Range(0, 0xfff);
        floes[i].obj->SetFacing(heading);
        floes[i].obj->SetVisible(1);
        floes[i].obj->SetCollidable(1);
        /* cast kept: a downcast: the floes are SlidingIceCubes, ScnBody objects */
        ((ScnBody *)floes[i].obj)->PlayAnim(AGLACON1_ANIM_STAND1, 1, 0);
    }
    floeTimer += g_dtMs;
}
