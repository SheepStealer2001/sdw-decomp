/* T229 - original object Train.cpp (guessed name), one translation unit.
 * .text 0x4fd5b0-0x50110e plus the COMDAT TrainCarBody::HandleMessage 0x501110-0x501145; .rdata 0x576e58-0x576ea0
 * (the vtables of Train and TrainCarBody); no .data/.bss. */
/* BYTES: slot-group, slot-name. */
/* match-addr: Train_PlaceCars=0x4fd6a6 Train_SetState=0x4fece2 TrainCarBody_HandleMessage=0x501110
 *
 * Train (class 135 "Train", vtable 0x576e58, sizeof 0x4f4) - the train of Level 14 (disc Lvl-16): a locomotive and
 * four wagons that run from station to station (class 136 TrainStation) through the tunnels, stopping where a
 * station's button has told them to wait and leaving at once when its other button is pressed. SheepD3D.exe
 * 0x4fd5b0-0x501145, the whole file: the factory, PlaceCars, SetState, PostLoadInit, Reset, Render, Update,
 * HandleMessage, and the one method of the embedded car class (0x501110, vtable 0x576e7c).
 * The Train, TrainCar and TrainWaypoint layouts are in data/structs.
 *
 * The five cars are sub-objects built inside the Train from the level's exports (ALOCOM1B 0xa8, AWAGON2A 0xb4,
 * AWAGON2B 0xb5, AWAGON1B 0xb3, AWAGON1A 0xb2) and are added to the world in their own right; each carries flag
 * bits saying which dock box (DOCKA / DOCKB) loads it. The train only ever faces one of the four compass
 * quadrants: PlaceCars rounds rot.y to a multiple of 0x400, enables the model box set of that quadrant
 * (0x40 / 0x80 / 0x100 / 0x200) on the loco and on every car, chains the cars back from the loco along the
 * facing using the half-extents of their own boxes, and carries every object standing on a car with them.
 *
 * States (+0x64, TrainState): 0 HIDDEN (out of play until a station starts its route), 1 AT_STATION (stopped at a
 * waypoint; every Lvl-16 route is tunnel - station stop - tunnel), 2 MOVING (running to the waypoint).
 *
 * Devices that only pin the original code generation, not claims about the original source text: the inline
 * helpers below (their names are not recovered - each is here because its expansion gives the original's stack
 * temporaries), and the names and grouping of the locals (under /Od a local's slot follows from a hash of its name;
 * see src/README.md and tools/vc6_locals.py). Two rules used throughout: a struct groups locals into one name, and an
 * inline function SUBSTITUTES a plain local argument but COPIES a struct member into a temporary of its own - so a
 * local that is passed to an inline has to be a local of its own (PlaceCars' `dRot`).
 */
#define SDW_MEMBERS_ScnObject            \
    static void *operator new(u32 size); \
    void SetFacing(s16 f);               \
    /* the spelling PlaceCars uses */    \
    Vec3s &PositionRef()                 \
    {                                    \
        return pos;                      \
    }                                    \
    /* inline, defined below */          \
    void SetUpdateMode(s32 mode);                /* inline, defined below */
#define SDW_MEMBERS_TrailEmitter TrailEmitter(); /* inline (defined below): the pools are the inline buffers */
#define SDW_MEMBERS_ZoneList    \
    void Load(u32 id);          \
    /* inline, defined below */ \
    Box *Overlaps(CollBox *b); /* inline, defined below */
#define SDW_MEMBERS_Train                                          \
    void SetRiderAttached(s32 attach); /* inline, defined below */ \
    u16 Voice()                                                    \
    {                                                              \
        return soundHandle;                                        \
    } /* inline: a u16 temp per read */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
#include "sdw_classes.h"
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#define SDW_INLINE_SCNOBJECT_FACING 1
#define SDW_INLINE_SCNOBJECT_SETFACING_S16 1
#define SDW_INLINE_SCNOBJECT_SETHEADING_S16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_FACING
#undef SDW_INLINE_SCNOBJECT_SETFACING_S16
#undef SDW_INLINE_SCNOBJECT_SETHEADING_S16
#include "../engine/grid_queries.h"
#include "../engine/property_math.h"

extern Wolf *g_pWolf;                  /* 0x6cf310 */
extern s32 g_dt;                       /* 0x71b300 */
extern "C" const s16 g_sinTable4096[]; /* 0x57ece0 */
extern "C" const s16 *g_pCosTable;     /* 0x5814e4 */

#include "../engine/scn_tools.h"
#include "../engine/id_list.h"
#include "../engine/scenaric.h"
#include "../engine/sound_mgr.h"
#include "../engine/interface.h"
extern "C" s32 Coll_BoxGroundQuery(CollBox *box, s32 *outY, ScnObject *self, u8 mode,
                                   ScnObject **outHitObj);                           /* 0x51a9de */
s32 Box_GroundQueryFlatTop(GroundQuery *q, CollBox *box, Vec3s *boxPos, s32 margin); /* 0x515934 */
u16 Sound_Play(u16 soundId, void *owner, u16 volume, u8 flags, s32 rate);            /* 0x5491b8 */
#include "../sdk/crt.h"

/* The vtables the inlined constructors store, as data symbols: C++ cannot spell "the vtable of that class". */
extern "C" const void *g_scnObjectVtbl2[]; /* 0x574c40 ScnObject_vtbl_2 */
extern "C" const void *g_scnBodyVtbl2[];   /* 0x574c1c ScnBody_vtbl_2 */
extern "C" const void *g_trainCarVtbl[];   /* 0x576e7c: ScnBody with only HandleMessage overridden */

/* ---- the embedded car ---- */

/* One of the five car sub-objects: a ScnBody whose only own method answers the ground query (0x576e7c). The class
 * TrainCarBody is generated in src/include/sdw_classes.h. */

/* ---- inline helpers ---- */

/* The first box of a model box list, or 0: an inline helper, so the list pointer gets a temp of its own. */
inline CollBox *ModelBoxes(ModelBoxList *l)
{
    if (l)
        return &l->boxes[0];
    return 0;
}

/* |v|, as a macro: the original evaluates its argument once per use (three GetFirstSolidBox calls per site). */
#define ABS(v) ((v) >= 0 ? (v) : -(v))

/* Turn by delta, wrapped to a full turn. Inline with the current facing as a parameter: that is where the
 * original's temporaries are (the facing a stack temp, the delta substituted, the object a third temp). */
inline void TurnBy(s16 facing, ScnObject *o, s16 delta)
{
    o->rot.y = (s16)(facing - delta & 0xfff);
}

/* SCN_OF_HIDDEN (0x800) off or on. */
#define SDW_INLINE_SCNOBJECT_SETVISIBLE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETVISIBLE_S32

/* The instance byte at +0x13 is a height in sixteenths: inline, so the constant is shifted at run time. */
inline u8 PartHeight16(u32 height)
{
    return (u8)(height >> 4);
}

inline u8 &PartHeightByte(ScnObject &object)
{
    return object.partHeight;
}

/* A u32 designer property: the record's property block starts at +0x14. */
#define SDW_INLINE_FREE_SCN_GETPROPU32_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_SCN_GETPROPU32_VOID_U32

/* ScnUpdateMode: 0 near the camera only, 1 always, 2 never, 3 also during cinematics. */
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32

/* The boxes of an ID...BOX property. */
#define SDW_INLINE_ZONELIST_LOAD_U32 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_LOAD_U32

#define SDW_INLINE_ZONELIST_CONTAINS_VEC3S 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_CONTAINS_VEC3S

#define SDW_INLINE_ZONELIST_CONTAINSXZ_VEC3S 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_CONTAINSXZ_VEC3S

inline Box *ZoneList::Overlaps(CollBox *b)
{
    return BoxList_FindOverlappingBox(b, boxes, count);
}

/* p inside box on all three axes, inclusive. */
#define SDW_INLINE_FREE_BOX_CONTAINSPOINT_COLLBOX_VEC3S 1
#include "../engine/coll_box_inlines.h"
#undef SDW_INLINE_FREE_BOX_CONTAINSPOINT_COLLBOX_VEC3S

/* Tell the rider (or, with no rider, the Wolf) to attach to or let go of the train. */
inline void Train::SetRiderAttached(s32 attach)
{
    if (rider) {
        if (attach) {
            if (!riderAttached)
                riderAttached = rider->HandleMessage(this, MSG_FREEZE, 0);
        } else if (riderAttached) {
            riderAttached = rider->HandleMessage(this, MSG_UNFREEZE, 0) == 0;
        }
    } else if (!attach) {
        if (riderAttached)
            riderAttached = g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0) == 0;
    }
}

/* The constructors, inlined into the factory: the five car bodies in turn (the compiler's array constructor over
 * TrainCar cars[5], whose element constructor writes the three vtables of ScnObject, ScnBody and the car class),
 * then the chimney emitter's pools; the Train's own vtable goes in last, after both. */

inline TrailEmitter::TrailEmitter()
{
    base.slotPool = slotBuf;
    base.particles = particleBuf;
    base.count = 16;
    base.Emitter_Reset();
}

/* 0x4fd5b0 */
ScnObject *Train_Create(void *record)
{
    ScnBody *train = new Train;
    train = train->Init(record, 0);
    return train;
}

/* 0x4fd6a6 */
/* BYTES(slot-group): locals grouped in work / v0 only to pin the original frame offsets; pad0, pad1, pad2, pad0, pad1, pad2 fill gaps */
/* BYTES(slot-name): two groups and six singles named so that eight names pin every stack offset (tools/vc6_locals.py) */
void Train::PlaceCars()
{
    /* The locals, grouped into two structs and six singles so that eight names pin every stack offset the original
     * has (src/README.md; under /Od a local's slot follows from a hash of its name, tools/vc6_locals.py). `work` and
     * `v0` are the two groups; `riders` is the object list; `side`, `orient`, `dRot`, `mask` and `ki2` have to be
     * locals of their own to come out where they do - `dRot` in particular, because an inline function substitutes a
     * plain local argument but copies a struct member into a temporary. */
    ScnObject *riders[2][64];
    struct {
        Vec3s pos;
        u16 pad0;
        u16 i;
        u16 m;
        s32 boxMask[4];
        s32 hits[2];
        CollBox box;
        u16 pad1;
        u16 quadrant;
        Vec3s carPos;
        u16 pad2;
    } work;
    u8 side;
    Vec3s orient;
    s16 dRot;
    u16 mask;
    u16 ki2;
    struct {
        Vec3s dockCenter;
        u16 pad0;
        Vec3s dropPos;
        u8 pad1[4];
        s16 swap;
        u16 n;
        u16 q;
        Vec3s carOrigin;
        u16 pad2;
    } v0;
    work.boxMask[0] = COLLBOX_QUADRANT0;
    work.boxMask[1] = COLLBOX_QUADRANT1;
    work.boxMask[2] = COLLBOX_QUADRANT2;
    work.boxMask[3] = COLLBOX_QUADRANT3;
    work.hits[0] = work.hits[1] = 0;
    rider = 0;
    work.quadrant = (u16)((s16)(Facing() + 0x200 & 0xfff) / 0x400);
    rot.y = (s16)(work.quadrant << 10);
    for (work.m = 0; work.m < 4; work.m++)
        DisableBoxes(work.boxMask[work.m]);
    EnableBoxes(work.boxMask[work.quadrant]);
    /* everything the cars are about to run into, gathered per dock side */
    for (work.i = 0; work.i < 5; work.i++) {
        if (cars[work.i].flags & TRAINCAR_F_PRESENT) {
            if (cars[work.i].flags & (TRAINCAR_F_SIDE_LEFT | TRAINCAR_F_SIDE_RIGHT)) {
                if (cars[work.i].flags & TRAINCAR_F_SIDE_LEFT)
                    side = 0;
                else
                    side = 1;
                if (cars[work.i].body.GetFirstSolidBox()) {
                    work.box = *cars[work.i].body.GetFirstSolidBox();
                    work.carPos = cars[work.i].body.pos;
                    work.carPos.y -= 99;
                    work.box.max.x += work.carPos.x;
                    work.box.max.y += work.carPos.y;
                    work.box.max.z += work.carPos.z;
                    work.carPos.y -= 200;
                    work.box.min.x += work.carPos.x;
                    work.box.min.y += work.carPos.y;
                    work.box.min.z += work.carPos.z;
                    work.hits[side] = ObjGrid_QueryBoxPoints(&work.box, riders[side]);
                }
            }
            for (mask = 0; mask < 4; mask++)
                cars[work.i].body.DisableBoxes(work.boxMask[mask]);
            cars[work.i].body.EnableBoxes(work.boxMask[work.quadrant]);
        }
    }
    work.pos = pos;
    orient = rot;
    switch (Facing()) {
        case 0:
            work.pos.z += (s16)ABS(GetFirstSolidBox()->min.z);
            break;
        case 0x400:
            work.pos.x += (s16)ABS(GetFirstSolidBox()->min.x);
            break;
        case 0x800:
            work.pos.z -= (s16)ABS(GetFirstSolidBox()->max.z);
            break;
        case 0xc00:
            work.pos.x -= (s16)ABS(GetFirstSolidBox()->max.x);
            break;
    }
    for (work.m = 0; work.m < 5; work.m++) {
        if (cars[work.m].flags & TRAINCAR_F_PRESENT) {
            if (cars[work.m].flags & (TRAINCAR_F_SIDE_LEFT | TRAINCAR_F_SIDE_RIGHT)) {
                if (cars[work.m].flags & TRAINCAR_F_SIDE_LEFT)
                    side = 0;
                else
                    side = 1;
                dRot = (s16)(cars[work.m].body.Facing() - orient.y & 0xfff);
                for (v0.q = 0; (s32)v0.q < work.hits[side]; v0.q++) {
                    if (riders[side][v0.q]->GetClassId() != CLASSID_TRAIN) {
                        if (riders[side][v0.q]->GetClassId() == CLASSID_WOLF) {
                            if (riders[side][v0.q]->HandleMessage(this, MSG_WOLF_IS_DEAD, 0) == 0)
                                rider = riders[side][v0.q];
                        } else if (riders[side][v0.q]->GetClassId() == CLASSID_SHEEP) {
                            if (Box_ContainsPoint(jumpInSheepBox, &riders[side][v0.q]->PositionRef()))
                                riders[side][v0.q]->HandleMessage(this, MSG_SHEEP_RUN_TO_SAM, 0);
                        }
                        riders[side][v0.q]->SetHeading((s16)(riders[side][v0.q]->Facing() - dRot & 0xfff));
                    }
                }
            }
            cars[work.m].body.rot = orient;
            switch (cars[work.m].body.Facing()) {
                case 0:
                    work.pos.z += (s16)ABS(cars[work.m].body.GetFirstSolidBox()->min.z);
                    break;
                case 0x400:
                    work.pos.x += (s16)ABS(cars[work.m].body.GetFirstSolidBox()->min.x);
                    break;
                case 0x800:
                    work.pos.z -= (s16)ABS(cars[work.m].body.GetFirstSolidBox()->max.z);
                    break;
                case 0xc00:
                    work.pos.x -= (s16)ABS(cars[work.m].body.GetFirstSolidBox()->max.x);
                    break;
            }
            v0.carOrigin = cars[work.m].body.pos;
            cars[work.m].body.SetPosition(&work.pos);
            if (cars[work.m].flags & (TRAINCAR_F_SIDE_LEFT | TRAINCAR_F_SIDE_RIGHT)) {
                for (v0.n = 0; (s32)v0.n < work.hits[side]; v0.n++) {
                    if (riders[side][v0.n]->GetClassId() != CLASSID_TRAIN &&
                        (riders[side][v0.n]->GetClassId() != CLASSID_WOLF ||
                         riders[side][v0.n]->GetClassId() == CLASSID_WOLF)) {
                        work.carPos = riders[side][v0.n]->PositionRef();
                        work.carPos.x -= v0.carOrigin.x;
                        work.carPos.y -= v0.carOrigin.y;
                        work.carPos.z -= v0.carOrigin.z;
                        switch (dRot) {
                            case 0x400:
                                v0.swap = work.carPos.x;
                                work.carPos.x = (s16)-work.carPos.z;
                                work.carPos.z = v0.swap;
                                break;
                            case 0x800:
                                work.carPos.x = (s16)-work.carPos.x;
                                work.carPos.z = (s16)-work.carPos.z;
                                break;
                            case 0xc00:
                                v0.swap = work.carPos.x;
                                work.carPos.x = work.carPos.z;
                                work.carPos.x = (s16)-v0.swap;
                                break;
                        }
                        work.carPos.x += work.pos.x;
                        work.carPos.y += work.pos.y;
                        work.carPos.z += work.pos.z;
                        riders[side][v0.n]->SetPosition(&work.carPos);
                    }
                }
                if (placeRiders) {
                    if (cars[work.m].flags & TRAINCAR_F_SIDE_LEFT)
                        side = 0;
                    else
                        side = 1;
                    v0.dockCenter.x = (s16)(docks[side]->min.x + docks[side]->max.x);
                    v0.dockCenter.y = (s16)(docks[side]->min.y + docks[side]->max.y);
                    v0.dockCenter.z = (s16)(docks[side]->min.z + docks[side]->max.z);
                    v0.dockCenter.x = (s16)(v0.dockCenter.x / 2);
                    v0.dockCenter.y = (s16)(v0.dockCenter.y / 2);
                    v0.dockCenter.z = (s16)(v0.dockCenter.z / 2);
                    work.hits[side] = ObjGrid_QueryPointsInRectXZ(docks[side]->min.x, docks[side]->min.z,
                                                                  docks[side]->max.x, docks[side]->max.z, riders[side]);
                    if (work.hits[side] > 0) {
                        for (ki2 = 0; (s32)ki2 < work.hits[side]; ki2++) {
                            v0.dropPos.x = (s16)(v0.dockCenter.x - riders[side][ki2]->PositionRef().x);
                            v0.dropPos.y = (s16)(v0.dockCenter.y - riders[side][ki2]->PositionRef().y);
                            v0.dropPos.z = (s16)(v0.dockCenter.z - riders[side][ki2]->PositionRef().z);
                            v0.dropPos.x += work.pos.x;
                            v0.dropPos.y += work.pos.y;
                            v0.dropPos.z += work.pos.z;
                            v0.dropPos.y = (s16)(work.pos.y - 0x42);
                            riders[side][ki2]->SetPosition(&v0.dropPos);
                            riders[side][ki2]->SnapToGround(1);
                        }
                    }
                }
            }
            switch (cars[work.m].body.Facing()) {
                case 0:
                    work.pos.z += (s16)ABS(cars[work.m].body.GetFirstSolidBox()->max.z);
                    break;
                case 0x400:
                    work.pos.x += (s16)ABS(cars[work.m].body.GetFirstSolidBox()->max.x);
                    break;
                case 0x800:
                    work.pos.z -= (s16)ABS(cars[work.m].body.GetFirstSolidBox()->min.z);
                    break;
                case 0xc00:
                    work.pos.x -= (s16)ABS(cars[work.m].body.GetFirstSolidBox()->min.x);
                    break;
            }
        }
    }
    smokePos = pos;
    smokePos.y = (s16)(smokePos.y - 500);
    smokePos.x += (s16)(g_sinTable4096[Facing()] * 0x46 >> 12);
    smokePos.z += (s16)(g_pCosTable[Facing()] * 0x46 >> 12);
    placeRiders = 0;
}

/* 0x4fece2 - enter a state: 0 hides the train and its cars, 1 stops it at the station (and hoots when it stops in
 * an OFFTRAINBOX after a run), 2 starts the run. */
/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets */
void Train::SetState(u8 newState)
{
    struct {
        u16 j;
        u16 i;
    } w;
    switch (newState) {
        case TRAIN_ST_HIDDEN:
            Sound_Stop(Voice(), this);
            SetVisible(0);
            for (w.i = 0; w.i < 5; w.i++) {
                if (cars[w.i].flags & TRAINCAR_F_PRESENT)
                    cars[w.i].body.SetVisible(0);
            }
            break;
        case TRAIN_ST_AT_STATION:
            if (state == TRAIN_ST_MOVING && offTrainZones.Contains(&pos)) {
                Sound_Stop(Voice(), this);
                soundHandle = Sound_Play(SND_TRAIN_WHISTLE, this, 0x3ff, SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 0x1000);
            }
            SetVisible(1);
            SetRiderAttached(0);
            for (w.j = 0; w.j < 5; w.j++) {
                if (cars[w.j].flags & TRAINCAR_F_PRESENT)
                    cars[w.j].body.SetVisible(1);
            }
            break;
        case TRAIN_ST_MOVING:
            Sound_Stop(Voice(), this);
            runTicks = 0;
            break;
    }
    state = newState;
}

/* 0x4ff003 */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py) */
void Train::PostLoadInit()
{
    /* Three locals named for their stack slots (src/README.md, tools/vc6_locals.py). */
    u16 i;
    void *props;
    Vec3s at;
    SetUpdateMode(SCN_UPD_ALWAYS);
    props = record;
    firstStation = Scn_GetPropObject(props, 8);
    /* cast kept (these three lines): Box and CollBox are two views of one 16-byte record */
    docks[0] = (CollBox *)Scn_GetPropBox(props, 0);
    docks[1] = (CollBox *)Scn_GetPropBox(props, 4);
    jumpInSheepBox = (CollBox *)Scn_GetPropBox(props, 0xc);
    offTrainZones.Load(Scn_GetPropU32(props, 0x10));
    safeRecallZones.Load(Scn_GetPropU32(props, 0x14));
    if (firstStation && firstStation->GetClassId() != CLASSID_TRAINSTATION)
        firstStation = 0;
    station = firstStation;
    partHeight = PartHeight16(0x80);
    cars[0].exportId = WAR_IDO_ALOCOM1B;
    cars[0].flags = 0;
    cars[1].exportId = WAR_IDO_AWAGON2A;
    cars[1].flags = TRAINCAR_F_BIT1 | TRAINCAR_F_SIDE_LEFT;
    cars[2].exportId = WAR_IDO_AWAGON2B;
    cars[2].flags = TRAINCAR_F_BIT1 | TRAINCAR_F_SIDE_RIGHT;
    cars[3].exportId = WAR_IDO_AWAGON1B;
    cars[3].flags = TRAINCAR_F_BIT4;
    cars[4].exportId = WAR_IDO_AWAGON1A;
    cars[4].flags = 0;
    at = pos;
    for (i = 0; i < 5; i++) {
        /* cast kept: Scn_BuildRecordFromExport writes the ScnRecordSynth through a u16 *, as it is declared */
        if (Scn_BuildRecordFromExport(cars[i].exportId, (u16 *)&cars[i].record, GetClassId(), &at)) {
            cars[i].flags |= TRAINCAR_F_PRESENT;
            cars[i].body.Init(&cars[i].record, 0);
            PartHeightByte(cars[i].body) = PartHeight16(0x80);
            cars[i].body.AddToWorld(0);
        } else {
            cars[i].flags &= ~TRAINCAR_F_PRESENT;
        }
    }
    placeRiders = 1;
    riderAttached = 0;
    blackout = 0;
    rider = 0;
    command = -1;
    smoke.base.Emitter_Reset();
    smokeParams.hSpeed = 60;
    smokeParams.vSpeed = -100;
    smokeParams.life = 0x2000;
    smokeParams.spawnInterval = smokeParams.life / 0x18;
    smokeParams.sizeStart = 0x50;
    smokeParams.sizeEnd = 0xb4;
    smokeParams.sheetIndex = 0;
    soundHandle = 0;
    SetState(TRAIN_ST_HIDDEN);
}

/* 0x4ff46a - vtable +0x14. */
void Train::Reset()
{
    placeRiders = 1;
    blackout = 0;
    soundHandle = 0;
}

/* 0x4ff498 - vtable +0x08: the loco, then the chimney smoke. */
void Train::Render(Camera *view)
{
    ScnBody::Render(view);
    smoke.base.Emitter_Render(view, 3);
}

/* 0x4ff4c5 - vtable +0x04. */
/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets; pad0, pad1, pad2, pad3, pad4, pad5, pad6 fill gaps */
void Train::Update()
{
    struct {
        s32 groundY;
        Vec3s center;
        u16 pad0;
        CollBox box;
        Vec3s move;
        u16 pad1;
        ContactInfo contact;
        s32 count;
        s32 n;
        ScnObject *found[64];
        u8 pad2[4];
        Vec3s testPos;
        u16 pad3;
        u32 frac;
        Vec3s step;
        u16 pad4;
        u32 dist;
        Vec3s delta;
        u16 pad5;
        CollBox hull;
        u8 pad6[3];
        u8 s;
    } w;
    switch (state) {
        case TRAIN_ST_HIDDEN:
            smoke.base.Emitter_UpdateDrift(&smokeParams, &smokePos, (s16)(Facing() + 0x800 & 0xfff), 0);
            if (rider)
                Fade_DrawOverlay(0, 0x1f, 0);
            if (station)
                station->HandleMessage(this, MSG_STATION_BEGIN_ROUTE, 0);
            break;
        case TRAIN_ST_AT_STATION:
            smoke.base.Emitter_UpdateDrift(&smokeParams, &smokePos, (s16)(Facing() + 0x800 & 0xfff), 1);
            if (rider && (command == MSG_TRAIN_START_AT || command == MSG_TRAIN_FINAL_WAYPOINT))
                Fade_DrawOverlay(0, 0x1f, 0);
            if (!Box_ContainsPoint(&wolfBox, &g_pWolf->pos))
                g_pWolf->HandleMessage(this, MSG_RIDER_REMOVE, 0);
            break;
        case TRAIN_ST_MOVING:
            runTicks += g_dt;
            smoke.base.Emitter_UpdateDrift(&smokeParams, &smokePos, (s16)(Facing() + 0x800 & 0xfff), 1);
            w.delta.x = (s16)(pos.x - waypoint.pos.x);
            w.delta.y = (s16)(pos.y - waypoint.pos.y);
            w.delta.z = (s16)(pos.z - waypoint.pos.z);
            w.dist = (u32)sqrt((double)w.delta.x * w.delta.x + (double)(w.delta.z * w.delta.z));
            if (!Sound_IsPlaying(Voice())) {
                w.hull.Box_Translate(GetFirstSolidBox(), &pos);
                if (offTrainZones.Overlaps(&w.hull))
                    soundHandle =
                        Sound_Play(SND_TRAIN_RUN, this, 0xff, SNDF_LOOP | SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 0x1000);
            }
            if (rider) {
                if (command == MSG_TRAIN_START_AT) {
                    Fade_DrawOverlay(0, 0x1f, 0);
                    blackout = 1;
                } else if (command == MSG_TRAIN_WAYPOINT && (u32)runTicks < 0x1000 && riderAttached == 1) {
                    Fade_DrawOverlay(0, (u8)(0x1f - ((u32)runTicks * 0x1f >> 12)), 0);
                    blackout = 0;
                } else {
                    w.frac = (w.dist << 12) / (u32)ABS(speed);
                    if (command == MSG_TRAIN_FINAL_WAYPOINT && w.frac < 0x1000)
                        Fade_DrawOverlay(0, (u8)(0x1f - (w.frac * 0x1f >> 12)), 0);
                }
            }
            if (blackout)
                Fade_DrawOverlay(0, 0x1f, 0);
            if (w.dist > 0x50) {
                w.step.x = (s16)(w.delta.x * speed / (s32)w.dist);
                w.step.y = 0;
                w.step.z = (s16)(w.delta.z * speed / (s32)w.dist);
                Vec3s_ScaleByDt(&w.step, &vel);
                sweepBox = *GetFirstSolidBox();
                w.testPos = pos;
                w.testPos.x += vel.x;
                w.testPos.y += vel.y;
                w.testPos.z += vel.z;
                sweepBox.min.x += w.testPos.x;
                sweepBox.min.y += w.testPos.y;
                sweepBox.min.z += w.testPos.z;
                sweepBox.max.x += w.testPos.x;
                sweepBox.max.y += w.testPos.y;
                sweepBox.max.z += w.testPos.z;
                w.count = ObjGrid_QueryBoxOverlap(&sweepBox, w.found);
                for (w.n = 0; w.n < w.count; w.n++) {
                    switch (w.found[w.n]->GetClassId()) {
                        case CLASSID_WOLF:
                            if (w.found[w.n]->HandleMessage(this, MSG_WOLF_IS_DEAD, 0) == 0) {
                                /* cast kept: MSG_KILL's arg is the WolfKillType, a number in the void * */
                                w.found[w.n]->HandleMessage(this, MSG_KILL, (void *)KILL_CRUSH);
                                w.found[w.n]->SnapToGround(0);
                            }
                            break;
                        case CLASSID_SAM:
                            w.move.x = (s16)-vel.x;
                            w.move.y = (s16)-vel.y;
                            w.move.z = (s16)-vel.z;
                            if (w.move.x) {
                                if (w.found[w.n]->pos.z < pos.z)
                                    w.move.z = (s16)0xff60;
                                else
                                    w.move.z = 0xa0;
                                w.move.x = 0;
                            } else {
                                if (w.found[w.n]->pos.x < pos.x)
                                    w.move.x = (s16)0xff60;
                                else
                                    w.move.x = 0xa0;
                                w.move.z = 0;
                            }
                            w.move.y = 0;
                            w.found[w.n]->Collide_ResolveMove(&w.move, &w.contact, 0xb54, RESOLVE_SLIDE_ALL, 0, 0, 10,
                                                              0, 0);
                            w.found[w.n]->Translate(&w.move);
                            if (!safeRecallZones.ContainsXZ(&w.found[w.n]->pos)) {
                                w.box.flags = ModelBoxes(w.found[w.n]->inst_model->boxes)->flags;
                                for (w.s = 0; w.s < safeRecallZones.count; w.s++) {
                                    w.center.x =
                                        (s16)(safeRecallZones.boxes[w.s]->min[0] + safeRecallZones.boxes[w.s]->max[0]);
                                    w.center.y =
                                        (s16)(safeRecallZones.boxes[w.s]->min[1] + safeRecallZones.boxes[w.s]->max[1]);
                                    w.center.z =
                                        (s16)(safeRecallZones.boxes[w.s]->min[2] + safeRecallZones.boxes[w.s]->max[2]);
                                    w.center.x = (s16)(w.center.x / 2);
                                    w.center.y = (s16)(w.center.y / 2);
                                    w.center.z = (s16)(w.center.z / 2);
                                    w.box.Box_Translate(ModelBoxes(w.found[w.n]->inst_model->boxes), &w.center);
                                    if (Coll_BoxGroundQuery(&w.box, &w.groundY, this,
                                                            CQ_STATIC | CQ_OBJECTS | CQ_SAM_WALL_OVERLAP, 0) == 0) {
                                        w.found[w.n]->SetPosition(&w.center);
                                        w.found[w.n]->SnapToGround(1);
                                        break;
                                    }
                                }
                            }
                            break;
                        case CLASSID_TRAIN:
                            break;
                        default:
                            /* cast kept: MSG_KILL's arg is the WolfKillType, a number in the void * */
                            w.found[w.n]->HandleMessage(this, MSG_KILL, (void *)KILL_CRUSH);
                            break;
                    }
                }
                switch (Facing()) {
                    case 0:
                    case 0x400:
                        hullBox.min.x = cars[0].body.GetFirstSolidBox()->min.x;
                        hullBox.min.y = cars[0].body.GetFirstSolidBox()->min.y;
                        hullBox.min.z = cars[0].body.GetFirstSolidBox()->min.z;
                        hullBox.min.x += cars[0].body.pos.x;
                        hullBox.min.y += cars[0].body.pos.y;
                        hullBox.min.z += cars[0].body.pos.z;
                        hullBox.max.x = cars[4].body.GetFirstSolidBox()->max.x;
                        hullBox.max.y = cars[4].body.GetFirstSolidBox()->max.y;
                        hullBox.max.z = cars[4].body.GetFirstSolidBox()->max.z;
                        hullBox.max.x += cars[4].body.pos.x;
                        hullBox.max.y += cars[4].body.pos.y;
                        hullBox.max.z += cars[4].body.pos.z;
                        break;
                    case 0x800:
                    case 0xc00:
                        hullBox.min.x = cars[4].body.GetFirstSolidBox()->min.x;
                        hullBox.min.y = cars[4].body.GetFirstSolidBox()->min.y;
                        hullBox.min.z = cars[4].body.GetFirstSolidBox()->min.z;
                        hullBox.min.x += cars[4].body.pos.x;
                        hullBox.min.y += cars[4].body.pos.y;
                        hullBox.min.z += cars[4].body.pos.z;
                        hullBox.max.x = cars[0].body.GetFirstSolidBox()->max.x;
                        hullBox.max.y = cars[0].body.GetFirstSolidBox()->max.y;
                        hullBox.max.z = cars[0].body.GetFirstSolidBox()->max.z;
                        hullBox.max.x += cars[0].body.pos.x;
                        hullBox.max.y += cars[0].body.pos.y;
                        hullBox.max.z += cars[0].body.pos.z;
                        break;
                }
                hullBox.min.x = (s16)(hullBox.min.x - 0x32);
                hullBox.min.y = (s16)(hullBox.min.y - 0x32);
                hullBox.min.z = (s16)(hullBox.min.z - 0x32);
                hullBox.max.x += 0x32;
                hullBox.max.y += 0x32;
                hullBox.max.z += 0x32;
                wolfBox.max.x = hullBox.max.x;
                wolfBox.max.y = hullBox.max.y;
                wolfBox.max.z = hullBox.max.z;
                wolfBox.min.x = hullBox.min.x;
                wolfBox.min.y = hullBox.min.y;
                wolfBox.min.z = hullBox.min.z;
                wolfBox.min.x = (s16)(wolfBox.min.x - 0x15e);
                wolfBox.min.y = (s16)(wolfBox.min.y - 0x160);
                wolfBox.min.z = (s16)(wolfBox.min.z - 0x15e);
                wolfBox.max.x += 0x15e;
                wolfBox.max.y += 0x15e;
                wolfBox.max.z += 0x15e;
                if (Box_ContainsPoint(&hullBox, &g_pWolf->pos) && !rider && offTrainZones.Contains(&g_pWolf->pos)) {
                    g_pWolf->HandleMessage(this, MSG_RIDER_ADD, 0);
                } else if (!Box_ContainsPoint(&wolfBox, &g_pWolf->pos)) {
                    g_pWolf->HandleMessage(this, MSG_RIDER_REMOVE, 0);
                }
                Translate(&vel);
                w.delta.x = (s16)(pos.x - waypoint.pos.x);
                w.delta.y = (s16)(pos.y - waypoint.pos.y);
                w.delta.z = (s16)(pos.z - waypoint.pos.z);
                w.dist = (u32)sqrt((double)w.delta.x * w.delta.x + (double)(w.delta.z * w.delta.z));
                if (w.dist < 0x50)
                    SetPosition(&waypoint.pos);
                PlaceCars();
            } else {
                if (command == MSG_TRAIN_FINAL_WAYPOINT)
                    SetState(TRAIN_ST_HIDDEN);
                else
                    SetState(TRAIN_ST_AT_STATION);
                station->HandleMessage(this, MSG_TRAIN_WAYPOINT, &station);
            }
            break;
    }
    if (!offTrainZones.Contains(&g_pWolf->pos)) {
        g_pWolf->HandleMessage(this, MSG_RIDER_REMOVE, 0);
        if (state != TRAIN_ST_AT_STATION)
            SetRiderAttached(1);
    } else {
        SetRiderAttached(0);
    }
    AdvanceAnim();
}

/* 0x500c58 - vtable +0x10. */
/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets; pad0 fill gaps */
s32 Train::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    struct {
        Vec3s push;
        MoveModifyArg *out;
        u16 pad0;
        u16 i;
    } w;
    if (sender->GetClassId() == CLASSID_WOLF) {
        switch (msgId) {
            case MSG_MODIFY_MOVE:
                if (!rider && sender->HandleMessage(this, MSG_WOLF_IS_DEAD, 0) == 0) {
                    w.out = (MoveModifyArg *)arg; /* cast kept: MSG_MODIFY_MOVE's arg is a MoveModifyArg */
                    w.push.x = (s16)-vel.x;
                    w.push.y = (s16)-vel.y;
                    w.push.z = (s16)-vel.z;
                    if (w.push.x) {
                        if (sender->pos.z < pos.z)
                            w.push.z = (s16)(g_dt * -0x44c / 0x1000);
                        else
                            w.push.z = (s16)(g_dt * 0x44c / 0x1000);
                    } else {
                        if (sender->pos.x < pos.x)
                            w.push.x = (s16)(g_dt * -0x44c / 0x1000);
                        else
                            w.push.x = (s16)(g_dt * 0x44c / 0x1000);
                    }
                    w.push.y = 0;
                    w.out->delta.x = w.push.x;
                    w.out->delta.y = w.push.y;
                    w.out->delta.z = w.push.z;
                }
                break;
            case MSG_FREEZE:
                riderAttached = 0;
                return 1;
        }
    }
    switch (msgId) {
        case MSG_TRAIN_IN_ZONE:
            if (offTrainZones.Contains(&pos))
                return 1;
            for (w.i = 0; w.i < 5; w.i++) {
                if (offTrainZones.Contains(&cars[w.i].body.pos))
                    return 1;
            }
            break;
        case MSG_TRAIN_WAYPOINT:
        case MSG_TRAIN_START_AT:
        case MSG_TRAIN_FINAL_WAYPOINT:
            command = msgId;
            if (sender->GetClassId() == CLASSID_TRAINSTATION && arg) {
                waypoint = *(TrainWaypoint *)arg; /* cast kept: the station messages' arg is a TrainWaypoint */
                if (msgId == MSG_TRAIN_START_AT) {
                    SetFacing(waypoint.heading);
                    SetPosition(&waypoint.pos);
                    PlaceCars();
                    speed = sender->HandleMessage(this, MSG_STATION_GET_SPEEDPERCENT, 0) * -1500 / 100;
                }
                SetState(TRAIN_ST_MOVING);
                return 1;
            }
            break;
        case MSG_TRAIN_GET_PASSENGER:
            if (sender->GetClassId() == CLASSID_TRAINSTATION) {
                if (!offTrainZones.Contains(&g_pWolf->pos) && state == TRAIN_ST_AT_STATION)
                    return 0;
                /* cast kept (both returns): HandleMessage answers an s32; this message returns the rider's address */
                return (s32)rider;
            }
            return (s32)rider;
        case MSG_TRAIN_IS_AT_STATION:
            if (state == TRAIN_ST_AT_STATION && command == MSG_TRAIN_WAYPOINT)
                return 1;
            break;
        case MSG_SWITCH_OFF:
            hornLatched = 0;
            break;
        case MSG_SWITCH_ON:
            if (state != TRAIN_ST_MOVING && hornLatched == 0)
                station->HandleMessage(this, MSG_TRAIN_WAYPOINT, 0);
            hornLatched = 1;
            break;
        case MSG_GROUND_QUERY:
            /* cast kept: MSG_GROUND_QUERY's arg is a GroundQuery */
            return Box_GroundQueryFlatTop((GroundQuery *)arg, GetFirstSolidBox(), &pos, 0);
    }
    return 0;
}

/* 0x501110 - vtable 0x576e7c +0x10: a car answers only the ground query, with the flat top of its own box.
 * Inline: in the original it is a COMDAT placed after the main .text (16-aligned at 0x501110), emitted with
 * TrainCarBody's vtable. */
inline s32 TrainCarBody::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    if (msgId == MSG_GROUND_QUERY)
        /* cast kept: MSG_GROUND_QUERY's arg is a GroundQuery */
        return Box_GroundQueryFlatTop((GroundQuery *)arg, GetFirstSolidBox(), &pos, 0);
    return 0;
}
