/*
 * T066 - original object CrocodileLevel11.cpp (guessed name): SheepD3D.exe .text 0x4342b0-0x435f24, .rdata
 * 0x574ccc-0x574cf0 (CrocodileLevel11's vtable), .data 0x57a680-0x57a68c (this object's copy of the cinematic
 * header's 9-byte opcode stride table).
 *
 *  - CrocodileLevel11::Render is defined after SetState, at its address (0x435bbd), for the object's .text order;
 *  - the stride table copy s_cineOpStride is DEFINED (static, unreferenced), as in the other objects that include the
 *    cinematic header: CrocodileLevel11 calls Cine::Start (0x43539b), so it includes that header, and the header's
 *    static table lands at the head of its .data. Values from the exe bytes (the same as g_cineOpStride 0x5816fc,
 *    src/engine/cine.cpp).
 * The ScnBody / ScnObject vtables this object also emits are COMDATs owned by T063 (BipbipLevel14), the first emitter.
 *
 * PAL PC CrocodileLevel11, runtime VAs 0x4342b0..0x435f23.
 */
/* BYTES: flow, layout, slot-group, temp. */
/* BYTES(layout): the Cine.h header static: every object including the cinematic header carries this copy in its .data, referenced or not */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"

#define SDW_MEMBERS_ScnObject            \
    static void *operator new(u32 size); \
    void SetFacing(s16 value);


#define SDW_MEMBERS_ZoneList void Load(u16);

#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#define SDW_INLINE_SCNOBJECT_FACING 1
#define SDW_INLINE_SCNOBJECT_SETFACING_S16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_FACING
#undef SDW_INLINE_SCNOBJECT_SETFACING_S16
#define SDW_INLINE_SCNBODY_GETANIMID 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_GETANIMID
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#define SDW_INLINE_CINE_ISFINISHED 1
#include "../engine/cine_inlines.h"
#undef SDW_INLINE_CINE_ISFINISHED
extern u32 g_gameTime;
#include "../app/app_main.h"
#include "../engine/cine.h"
#include "camera.h"
#include "../engine/scn_tools.h"
#include "../engine/id_list.h"
extern Wolf *g_pWolf;
u32 Anim_Start(Instance *, Animator *, u16, u32);
#include "../sdk/crt.h"
extern "C" s16 Math_RadiansToAngle4096(float);
void Camera_StartScripted(ScnObject *, Camera *, u16, u16, u16, Vec3s *, u16, u32, s32);
s32 ObjGrid_QueryBoxPoints(const CollBox *, ScnObject **);
s32 Rand_Bounded(s32);

/* 0x57a680 - the cinematic header's static copy of the 9-byte opcode stride table (src/engine/cine.cpp,
 * g_cineOpStride 0x5816fc). The original header defined it static, so every object including it carries its own
 * unreferenced copy at the head of its .data; defined here in its place. */
static u8 s_cineOpStride[9] = {0, 8, 8, 4, 2, 2, 4, 2, 2};

#define SDW_INLINE_ZONELIST_LOAD_U16 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_LOAD_U16
#define SDW_INLINE_ZONELIST_FINDCONTAINING_VEC3S 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_FINDCONTAINING_VEC3S
/* BYTES(temp, inferred): the offset and the result go through the read struct because the original's expansion keeps both in stack slots */
#define SDW_INLINE_FREE_PROPU32_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_PROPU32_VOID_U32
#define SDW_INLINE_SCNOBJECT_FIRSTBOX 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_FIRSTBOX
#define SDW_INLINE_FREE_STARTCINE_U32_U32_BOX_BOX_VOID 1
#include "../engine/cine_inlines.h"
#undef SDW_INLINE_FREE_STARTCINE_U32_U32_BOX_BOX_VOID
#define ANGLE_DIFF(a, b) ((s16)((s16)(((a) - (b) + 0x800) & 0xfff) - 0x800))
#define ABS_VALUE(a) ((a) >= 0 ? (a) : -(a))
static inline s32 InBox(CollBox *box, Vec3s *point)
{
    return point->x >= box->min.x && point->x <= box->max.x && point->y >= box->min.y && point->y <= box->max.y &&
           point->z >= box->min.z && point->z <= box->max.z;
}
#define SDW_INLINE_FREE_INBOXXZ_BOX_VEC3S 1
#include "../engine/coll_box_inlines.h"
#undef SDW_INLINE_FREE_INBOXXZ_BOX_VEC3S

/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets */
void CrocodileLevel11::PostLoadInit()
{
    struct Work {
        Box *nearest;
        u32 distance;
        void *properties;
        Vec3s center;
        u16 unused[2], index;
        u32 best;
    } w;
    w.properties = record;
    zones.Load((u16)PropU32(w.properties, 4));
    daffy = Scn_GetPropObject(w.properties, 32);
    remainDeadAtReset = PropU32(w.properties, 36);
    camera = Scn_GetPropCamera(w.properties, 8);
    cineId = PropU32(w.properties, 12);
    cineBox = Scn_GetPropBox(w.properties, 16);
    /* cast kept: Box and CollBox are two views of one 16-byte record (CollBox has the methods) */
    activationBox = (CollBox *)Scn_GetPropBox(w.properties, 0);
    cineSheepBox = Scn_GetPropBox(w.properties, 24);
    cineTextIndex = (u8)PropU32(w.properties, 28);
    cinePlayed = 0;
    cineFlags = PropU32(w.properties, 20);
    w.nearest = zones.FindContaining(&pos);
    if (!w.nearest) {
        w.best = -1;
        for (w.index = 0; w.index < zones.count; w.index++) {
            w.center.x = zones.boxes[w.index]->max[0] + zones.boxes[w.index]->min[0];
            w.center.y = zones.boxes[w.index]->max[1] + zones.boxes[w.index]->min[1];
            w.center.z = zones.boxes[w.index]->max[2] + zones.boxes[w.index]->min[2];
            w.center.x /= 2;
            w.center.y /= 2;
            w.center.z /= 2;
            w.center.x -= pos.x;
            w.center.y -= pos.y;
            w.center.z -= pos.z;
            w.distance = w.center.x * w.center.x + w.center.z * w.center.z;
            if (w.distance < w.best) {
                w.best = w.distance;
                w.nearest = zones.boxes[w.index];
            }
        }
    }
    w.center.x = w.nearest->max[0] + w.nearest->min[0];
    w.center.y = w.nearest->max[1] + w.nearest->min[1];
    w.center.z = w.nearest->max[2] + w.nearest->min[2];
    w.center.x /= 2;
    w.center.y /= 2;
    w.center.z /= 2;
    w.center.y = pos.y;
    SetPosition(&w.center);
    curBox = w.nearest;
    homeBox = curBox;
    bounceBox = 0;
    homePos = pos;
    homeFacing = Facing();
    wolfFrozen = 0;
    SetState(CROC11_ST_IDLE);
}

void CrocodileLevel11::Reset()
{
    wolfFrozen = 0;
    curBox = homeBox;
    if (state != CROC11_ST_DEAD)
        SetPosition(&homePos);
    SetFacing(homeFacing);
    nextIdleAnimTime = g_gameTime;
    if (!remainDeadAtReset)
        SetState(CROC11_ST_IDLE);
}

/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets; padCenter, padDelta, padAnims fill gaps */
void CrocodileLevel11::Update()
{
    struct Work {
        s32 deadIndex;
        Vec3s center;
        u16 padCenter;
        Vec3s delta;
        u16 padDelta;
        Box *chosen;
        u16 idleAnims[3];
        u16 padAnims;
        s32 index;
        u32 distance;
        u8 unused[3], next;
        ScnObject *target;
    } w;
    w.idleAnims[0] = ACROCO01_ANIM_STAND0;
    w.idleAnims[1] = ACROCO01_ANIM_STAND1;
    w.idleAnims[2] = ACROCO01_ANIM_STAND2;
    switch (state) {
        case CROC11_ST_IDLE:
            if (g_gameTime > nextIdleAnimTime) {
                PlayAnim(w.idleAnims[Rand_Bounded(3)], 1, 1);
                nextIdleAnimTime = g_gameTime + Rand_Bounded(0x5000);
            }
            if (!cinePlayed && InBox(activationBox, &g_pWolf->pos)) {
                SetState(CROC11_ST_CINE);
                break;
            }
            /* cast kept: Box and CollBox are two views of one 16-byte record (CollBox has the methods) */
            boxObjectCount = ObjGrid_QueryBoxPoints((CollBox *)curBox, boxObjects);
            if (boxObjectCount > 1)
                SetState(CROC11_ST_SCAN);
            break;
        case CROC11_ST_CINE:
            if (g_cinePlayer.IsFinished()) {
                SetState(CROC11_ST_IDLE);
                cinePlayed = 1;
            }
            break;
        case CROC11_ST_SCAN:
            w.next = CROC11_ST_IDLE;
            prevBoxObjectCount = boxObjectCount;
            for (w.index = 0; w.index < boxObjectCount; w.index++) {
                switch (boxObjects[w.index]->GetClassId()) {
                    case CLASSID_WOLF:
                        w.next = CROC11_ST_THREATEN_WOLF;
                        targetIndex = w.index;
                        break;
                    case CLASSID_CANNONBALL:
                        w.next = CROC11_ST_WATCH_BALL;
                        targetIndex = w.index;
                        break;
                }
            }
            SetState(w.next);
            break;
        case CROC11_ST_THREATEN_WOLF:
            w.target = boxObjects[targetIndex];
            if (InBox(&biteBox, &w.target->pos)) {
                if (!w.target->HandleMessage(this, MSG_WOLF_IS_DEAD, 0)) {
                    PlayAnim(ACROCO01_ANIM_EAT1, 0, 1);
                    /* cast kept: HandleMessage's arg is a void *: this message passes a number in it */
                    w.target->HandleMessage(this, MSG_KILL, (void *)KILL_CROCODILE);
                    break;
                }
                SetState(CROC11_ST_IDLE);
            }
            if (!InBoxXZ(curBox, &w.target->pos))
                SetState(CROC11_ST_IDLE);
            if (GetAnimId() == ACROCO01_ANIM_BRAKE && AnimFlags(ANIM_F_FINISHED)) {
                SetFacing(HeadingToPoint(w.target->pos));
                PlayAnim(ACROCO01_ANIM_STAND1, 1, 0);
            }
            break;
        case CROC11_ST_WATCH_BALL:
            if (!InBoxXZ(curBox, &ball->pos)) {
            /* BYTES(flow): the retry is a goto loop: written as do / while, Update compiles differently (test-built) */
            choose_other_box:
                w.chosen = zones.boxes[Rand_Bounded(zones.count)];
                if (curBox == w.chosen)
                    goto choose_other_box;
                curBox = w.chosen;
                SetState(CROC11_ST_RELOCATE);
            }
            UpdateCamera();
            break;
        case CROC11_ST_RELOCATE:
            switch (relocatePhase) {
                case CROC11_RELOC_TURN_TO_BOX:
                    if (GetAnimId() != turnAnim) {
                        PlayAnim(turnAnim, 1, 1);
                        turnOffset = 0;
                    } else {
                        turnOffset += (s16)(ANGLE_DIFF(HeadingToBoxCenter(curBox), HeadingToObject(daffy)) / 10);
                        SetFacing((turnOffset + HeadingToObject(daffy)) & 0xfff);
                        if (ABS_VALUE(ANGLE_DIFF(HeadingToBoxCenter(curBox), Facing())) < 10) {
                            PlayAnim(ACROCO01_ANIM_RUN, 1, 1);
                            relocatePhase = CROC11_RELOC_WALK;
                        }
                    }
                    UpdateCamera();
                    break;
                case CROC11_RELOC_WALK:
                    w.center.x = curBox->max[0] + curBox->min[0];
                    w.center.y = curBox->max[1] + curBox->min[1];
                    w.center.z = curBox->max[2] + curBox->min[2];
                    w.center.x /= 2;
                    w.center.y /= 2;
                    w.center.z /= 2;
                    w.center.y = pos.y;
                    w.delta = pos;
                    w.delta.x = w.center.x - w.delta.x;
                    w.delta.y = w.center.y - w.delta.y;
                    w.delta.z = w.center.z - w.delta.z;
                    w.distance = (s32)sqrt((double)w.delta.x * w.delta.x + w.delta.z * w.delta.z);
                    w.delta.x /= (s16)w.distance;
                    w.delta.y /= (s16)w.distance;
                    w.delta.z /= (s16)w.distance;
                    w.delta.x *= 430;
                    w.delta.y *= 430;
                    w.delta.z *= 430;
                    Vec3s_ScaleByDt(&w.delta, &w.delta);
                    Collide_ResolveMove(&w.delta, 0, 0x578, RESOLVE_SLIDE_ALL, 0, 0, 10, 0, 0);
                    if (w.distance < 50) {
                        if (turnAnim == ACROCO01_ANIM_TURN)
                            turnAnim = ACROCO01_ANIM_TURN1;
                        else
                            turnAnim = ACROCO01_ANIM_TURN;
                        PlayAnim(turnAnim, 1, 1);
                        relocatePhase = CROC11_RELOC_TURN_TO_DAFFY;
                        turnOffset = 0;
                    } else
                        Translate(&w.delta);
                    UpdateCamera();
                    break;
                case CROC11_RELOC_TURN_TO_DAFFY:
                    turnOffset += (s16)(ANGLE_DIFF(HeadingToObject(daffy), HeadingToBoxCenter(curBox)) / 10);
                    SetFacing((HeadingToBoxCenter(curBox) + turnOffset) & 0xfff);
                    if (ABS_VALUE(ANGLE_DIFF(HeadingToObject(daffy), Facing())) < 10) {
                        SetState(CROC11_ST_IDLE);
                        wolfFrozen = !g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0);
                        Camera_ReleaseScripted(this);
                    } else
                        UpdateCamera();
                    break;
            }
            break;
        case CROC11_ST_DEAD:
            if (GetAnimId() == ACROCO01_ANIM_DEAD && AnimFlags(ANIM_F_FINISHED))
                Camera_ReleaseScripted(this);
            if (!bouncer) {
                boxObjectCount = ObjGrid_QueryBoxPoints(bounceBox, boxObjects);
                if (boxObjectCount > prevBoxObjectCount) {
                    for (w.deadIndex = 0; w.deadIndex < boxObjectCount; w.deadIndex++) {
                        if (boxObjects[w.deadIndex]->GetClassId() == CLASSID_WOLF &&
                            !boxObjects[w.deadIndex]->HandleMessage(this, MSG_WOLF_IS_DEAD, 0)) {
                            bouncer = boxObjects[w.deadIndex];
                            bounceEntryVert = bouncer->pos.y;
                        }
                    }
                }
                prevBoxObjectCount = boxObjectCount;
            } else {
                if (bouncer->pos.y > bounceEntryVert) {
                    PlayAnim(ACROCO01_ANIM_SQUASH, 0, 1);
                    bouncer->HandleMessage(this, MSG_BOUNCE, 0);
                }
                bouncer = 0;
                bounceEntryVert = -32768;
            }
            break;
    }
    AdvanceAnim();
}

s32 CrocodileLevel11::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    switch (msgId) {
        case MSG_FREEZE:
            wolfFrozen = 0;
            return 1;
        case MSG_KILL:
            if (sender->GetClassId() == CLASSID_CANNONBALL && state != CROC11_ST_DEAD) {
                SetState(CROC11_ST_DEAD);
                return 1;
            }
            break;
    }
    return 0;
}

/* Keep each narrow multiply before divide; folding into a ratio changes overflow. */
#define BUILD_BITE_BOX()    \
    biteBox = *FirstBox();  \
    biteBox.min.x *= 200;   \
    biteBox.min.y *= 200;   \
    biteBox.min.z *= 200;   \
    biteBox.max.x *= 200;   \
    biteBox.max.y *= 200;   \
    biteBox.max.z *= 200;   \
    biteBox.min.x /= 80;    \
    biteBox.min.y /= 80;    \
    biteBox.min.z /= 80;    \
    biteBox.max.x /= 80;    \
    biteBox.max.y /= 80;    \
    biteBox.max.z /= 80;    \
    biteBox.min.x += pos.x; \
    biteBox.min.y += pos.y; \
    biteBox.min.z += pos.z; \
    biteBox.max.x += pos.x; \
    biteBox.max.y += pos.y; \
    biteBox.max.z += pos.z;

void CrocodileLevel11::SetState(u8 next)
{
    Vec3s center;
    switch (next) {
        case CROC11_ST_IDLE:
            if (state != CROC11_ST_SCAN) {
                nextIdleAnimTime = g_gameTime;
                PlayAnim(ACROCO01_ANIM_STAND0, 1, 0);
            }
            break;
        case CROC11_ST_CINE:
            StartCine(cineId, cineFlags, cineBox, cineSheepBox, Text_GetClassString(cineTextIndex));
            break;
        case CROC11_ST_THREATEN_WOLF:
            if (ABS_VALUE(ANGLE_DIFF(Facing(), HeadingToPoint(boxObjects[targetIndex]->pos))) > 0x400)
                PlayAnim(ACROCO01_ANIM_BRAKE, 0, 1);
            else
                PlayAnim(ACROCO01_ANIM_STAND1, 1, 1);
            BUILD_BITE_BOX();
            break;
        case CROC11_ST_WATCH_BALL:
            ball = boxObjects[targetIndex];
            ball->HandleMessage(this, MSG_CB_SET_OWNS_CAMERA, 0);
            BUILD_BITE_BOX();
            bounceBox = &biteBox;
            PlayAnim(ACROCO01_ANIM_STAND2, 1, 1);
            break;
        case CROC11_ST_RELOCATE:
            center.x = curBox->min[0] + curBox->max[0];
            center.y = curBox->min[1] + curBox->max[1];
            center.z = curBox->min[2] + curBox->max[2];
            center.x /= 2;
            center.y /= 2;
            center.z /= 2;
            if (ANGLE_DIFF(HeadingToPoint(center), Facing()) < 0)
                turnAnim = ACROCO01_ANIM_TURN;
            else
                turnAnim = ACROCO01_ANIM_TURN1;
            relocatePhase = CROC11_RELOC_TURN_TO_BOX;
            wolfFrozen = g_pWolf->HandleMessage(this, MSG_FREEZE, 0);
            break;
        case CROC11_ST_DEAD:
            bouncer = 0;
            bounceEntryVert = -32768;
            PlayAnim(ACROCO01_ANIM_DEAD, 0, 1);
            break;
    }
    state = next;
}

/* BYTES(layout): defined here, after SetState, for the object's .text order */
void CrocodileLevel11::Render(Camera *view)
{
    ScnBody::Render(view);
}

void CrocodileLevel11::PlayAnim(u16 id, s32 loop, s32 blend)
{
    u32 options = 0;
    if (loop)
        options |= ANIM_SET_LOOP;
    if (blend)
        options |= ANIM_SET_BLEND;
    Anim_Start(Inst(), &anim, id, options);
}

/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; unused fill gaps */
void CrocodileLevel11::UpdateCamera()
{
    /* Explicit work record retains the observed independent coordinate arrays. */
    struct Work {
        Vec3s *eye;
        Vec3i square;
        Vec3s angles;
        u16 unused;
        Vec3i delta;
    } w;
    w.delta.x = pos.x - camera->eye.x;
    w.delta.y = pos.y - camera->eye.y;
    w.delta.z = pos.z - camera->eye.z;
    w.square.x = w.delta.x * w.delta.x;
    w.square.y = w.delta.y * w.delta.y;
    w.square.z = w.delta.z * w.delta.z;
    w.angles.x =
        Math_RadiansToAngle4096((float)atan2(w.delta.y, (s32)sqrt((double)w.square.x + (double)w.square.z))) & 0xfff;
    w.angles.y = Math_RadiansToAngle4096((float)atan2(-w.delta.x, w.delta.z)) & 0xfff;
    w.eye = &camera->eye;
    Camera_StartScripted(this, &g_camera, w.angles.x, w.angles.y, 0, w.eye, 400, 0, 4096);
}

/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets; unused fill gaps */
s16 CrocodileLevel11::HeadingToPoint(Vec3s point)
{
    struct Work {
        Vec3s delta;
        u16 unused[2];
        s16 heading;
    } w;
    w.delta.x = point.x - pos.x;
    w.delta.y = point.y - pos.y;
    w.delta.z = point.z - pos.z;
    w.heading = (Math_RadiansToAngle4096((float)atan2(w.delta.x, w.delta.z)) + 0x800) & 0xfff;
    return w.heading;
}

s16 CrocodileLevel11::HeadingToObject(ScnObject *object)
{
    if (object)
        return HeadingToPoint(object->pos);
    return 0;
}

s16 CrocodileLevel11::HeadingToBoxCenter(Box *box)
{
    Vec3s center;
    center.x = box->min[0] + box->max[0];
    center.y = box->min[1] + box->max[1];
    center.z = box->min[2] + box->max[2];
    center.x /= 2;
    center.y /= 2;
    center.z /= 2;
    return HeadingToPoint(center);
}

ScnObject *CrocodileLevel11_Create(void *record)
{
    ScnBody *object = new CrocodileLevel11;
    object = object->Init(record, 0);
    return object;
}
