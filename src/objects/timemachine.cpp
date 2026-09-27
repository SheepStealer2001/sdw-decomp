/* T225 - original object TimeMachine.cpp (guessed name), one translation unit.
 * .text 0x4f9fe0-0x4fc075, .rdata 0x576dc8-0x576dec (TimeMachineSphere's vtable), .data 0x57b770-0x57b800,
 * .bss 0x6cfb50-0x6cfb68.
 * The TimeMachineSphere methods, with TimeMachine_IsInPresent 0x4fba8e between HandleMessage and SwapToPast;
 * functions in address order.
 * The object-private .bss: VC6 orders .bss by a 1024-bucket hash of the names, later definition first in a bucket, so
 * g_timeMachineZonePresent, g_timeMachinePastOffset and g_timeMachinePastEraZone are named for that order, and
 * data/symbols.csv gives the same names (the descriptive ones are g_timeMachineBoxPresent, g_timeMachineOffsetToPast
 * and g_timeMachineBoxPast); g_timeMachineOffsetToPresent (also read by Seed and Tree) keeps its table name. */
/* BYTES: bss-name, dead-code, layout, slot-group, slot-scope. */
/* BYTES(layout): the Cine.h header static: every object including the cinematic header carries this copy in its .data, referenced or not */
/* BYTES(bss-name): named for its .bss hash key 961 */
/* BYTES(bss-name): named for its .bss hash key 964 */
/* BYTES(bss-name): named for its .bss hash key 1011 */
/* BYTES(layout): not const: the original has it in .data */
/* PAL PC TimeMachineSphere. */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *, Animator *, u16, u32);
struct SamScreenGeometry {
    u16 width, height, x, y, aspect;
};

#define SDW_MEMBERS_ScnObject       \
    static void *operator new(u32); \
    void StartCamera(u16, u16, u16, Vec3s *, u16, u32);

#define SDW_MEMBERS_Screen SamScreenGeometry *GetGeometry(SamScreenGeometry *);

#define SDW_MEMBERS_CollBox                                                                                        \
    s32 Contains(Vec3s *p)                                                                                         \
    {                                                                                                              \
        return p->x >= min.x && p->x <= max.x && p->y >= min.y && p->y <= max.y && p->z >= min.z && p->z <= max.z; \
    }
#include "sdw_classes.h"
#include "../engine/scenaric.h"
#include "../engine/scn_tools.h"
#include "../engine/transition.h"
#include "../engine/sound_mgr.h"
#include "camera.h"
#include "../app/app_main.h"
#include "../engine/screen.h"
#include "../engine/cine.h"
#include "sheep.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_ENABLEBOXCOLLIDE_S32 1
#define SDW_INLINE_SCNOBJECT_SETVISIBLE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_ENABLEBOXCOLLIDE_S32
#undef SDW_INLINE_SCNOBJECT_SETVISIBLE_S32
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_GETANIMID 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_GETANIMID
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_CINE_ISACTIVE 1
#include "../engine/cine_inlines.h"
#undef SDW_INLINE_CINE_ISACTIVE
s32 Scenaric_FindByClass(u16, ScnObject **, s32);
s32 TimeMachine_IsInPresent(ScnObject *);
u16 Sound_Play(u16, void *, u16, u8, s32);
void Camera_StartScripted(ScnObject *, Camera *, u16, u16, u16, Vec3s *, u16, u32, s32);
#define g_camPos (g_camera.pos)

#define g_projFocalScale (g_screen.projDist)

extern s32 g_dtMs;
extern u32 g_gameFlags;
extern Wolf *g_pWolf;
#define SDW_INLINE_SCNOBJECT_STOPSOUNDHANDLE_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_STOPSOUNDHANDLE_U16
#define SDW_INLINE_SCNOBJECT_ISSOUNDPLAYING_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_ISSOUNDPLAYING_U16
inline s32 ProjectionScale()
{
    s32 scale = g_projFocalScale;
    return scale;
}
#define SDW_INLINE_SCNOBJECT_STARTCAMERA_U16_U16_U16_VEC3S_U16_U32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_STARTCAMERA_U16_U16_U16_VEC3S_U16_U32

/* 0x57b770 - the cinematic header's static copy of the 9-byte opcode stride table (src/engine/cine.cpp,
 * g_cineOpStride 0x5816fc): unreferenced here, every object including the header carries one at the head of its .data. */
static u8 s_cineOpStride[9] = {0, 8, 8, 4, 2, 2, 4, 2, 2};
/* 0x57b780 - {past, present} designer-property offsets of the sixteen OBJECTnnPAST/PRESENT pairs. Not const: it sits
 * in .data (8-aligned after the stride table). */
u32 g_timeMachinePairPropOffsets[16][2] = {{8, 12},    {16, 20},   {24, 28},   {32, 36},  {40, 44}, {48, 52},
                                           {56, 60},   {64, 68},   {72, 76},   {80, 84},  {88, 92}, {96, 100},
                                           {104, 108}, {112, 116}, {120, 124}, {128, 132}};
/* .bss 0x6cfb50-0x6cfb68, in hash-bucket order (the .bss rule in src/README.md) */
Vec3s g_timeMachineOffsetToPresent; /* 0x6cfb50  bucket 901: BoxPresent.min - BoxPast.min */
Box *g_timeMachineZonePresent;      /* 0x6cfb58  bucket 961: BOXPRESENT (sphere prop +4) */
Vec3s g_timeMachinePastOffset;      /* 0x6cfb5c  bucket 964: BoxPast.min - BoxPresent.min */
Box *g_timeMachinePastEraZone;      /* 0x6cfb64  bucket 1011: BOXPAST (sphere prop +0) */
/* TimeMachine_IsInPresent's era-box test: x and z only */
#define SDW_INLINE_FREE_CONTAINSXZ_BOX_VEC3S 1
#include "../engine/coll_box_inlines.h"
#undef SDW_INLINE_FREE_CONTAINSXZ_BOX_VEC3S

/* BYTES(slot-scope, inferred): the nested block(s) only order the frame: their locals are allocated after the enclosing scope's */
void TimeMachineSphere::PostLoadInit()
{
    void *props = record;
    {
        u8 i;
        {
            ScnObject *present;
            {
                ScnObject *past;
                /* cast kept: the record builder writes the synthetic record through a u16 * */
                if (Scn_BuildRecordFromExport(WAR_IDO_ABTIME02, (u16 *)&billboardRecord, 0, 0))
                    billboard.Init(&billboardRecord, 0);
                billboard.PlayAnim(ABTIME02_ANIM_STAND2, 1, 0);
                g_timeMachinePastEraZone = Scn_GetPropBox(props, 0);
                g_timeMachineZonePresent = Scn_GetPropBox(props, 4);
                g_timeMachineOffsetToPresent.x = g_timeMachineZonePresent->min[0] - g_timeMachinePastEraZone->min[0];
                g_timeMachineOffsetToPresent.y = g_timeMachineZonePresent->min[1] - g_timeMachinePastEraZone->min[1];
                g_timeMachineOffsetToPresent.z = g_timeMachineZonePresent->min[2] - g_timeMachinePastEraZone->min[2];
                g_timeMachinePastOffset.x = g_timeMachinePastEraZone->min[0] - g_timeMachineZonePresent->min[0];
                g_timeMachinePastOffset.y = g_timeMachinePastEraZone->min[1] - g_timeMachineZonePresent->min[1];
                g_timeMachinePastOffset.z = g_timeMachinePastEraZone->min[2] - g_timeMachineZonePresent->min[2];
                for (i = 0; i < 16; i++) {
                    present = Scn_GetPropObject(props, g_timeMachinePairPropOffsets[i][1]);
                    past = Scn_GetPropObject(props, g_timeMachinePairPropOffsets[i][0]);
                    if (present) {
                        pairs[i].present = present;
                        if (past)
                            pairs[i].past = past;
                        else
                            pairs[i].past = 0;
                    } else
                        pairs[i].present = 0;
                }
                EnableBoxCollide(0);
                Scenaric_FindByClass(CLASSID_TIMEMACHINECHRONO, &chrono, 1);
                for (i = 0; i < 5; i++)
                    seeds[i] = 0;
                seedCount = (u8)Scenaric_FindByClass(CLASSID_SEED, seeds, 5);
                if (!Scenaric_FindByClass(CLASSID_DRAGON, &dragon, 1))
                    dragon = 0;
                if (!Scenaric_FindByClass(CLASSID_GOSSAMER_LEV08, &gossamer, 1))
                    gossamer = 0;
                if (!Scenaric_FindByClass(CLASSID_BULL, &bull, 1))
                    bull = 0;
                releaseCamNextFrame = 0;
                humSound = 0;
                openSound = 0;
                SetState(TMS_IDLE);
            }
        }
    }
}
/* BYTES(slot-group): locals grouped in geometry only to pin the original frame offsets */
/* BYTES(slot-scope): the nested block(s) only order the frame: their locals are allocated after the enclosing scope's */
void TimeMachineSphere::Update()
{
    /* Scopes retain the original stack slots for separate transition cases. */
    u16 billboardAnim;
    {
        Vec3s wolfPast;
        {
            Vec3s camPast;
            {
                ScnObject *fogPast;
                {
                    Vec3s spherePast;
                    {
                        CollBox returnBox;
                        {
                            Vec3s followerTarget;
                            {
                                CollBox followerBox;
                                {
                                    Vec3s wolfPresent;
                                    {
                                        Vec3s camPresent;
                                        {
                                            u8 seedIndex;
                                            {
                                                ScnObject *fogPresent;
                                                {
                                                    Vec3s spherePresent;
                                                    {
                                                        Vec3s gossamerPresent;
                                                        {
                                                            u8 gossamerSeed;
                                                            {
                                                                Vec3s dragonPresent;
                                                                {
                                                                    struct {
                                                                        SamScreenGeometry present, past;
                                                                    } geometry;
                                                                    if (releaseCamNextFrame) {
                                                                        Camera_ReleaseScripted(this);
                                                                        releaseCamNextFrame = 0;
                                                                    }
                                                                    switch (state) {
                                                                        case TMS_DEPART_TO_PAST:
                                                                            if (AnimFlags(ANIM_F_FINISHED)) {
                                                                                SwapToPast();
                                                                                if (g_gameFlags & GF_UPDATE_OBJECTS) {
                                                                                    wolfInsideSinceArrival = 1;
                                                                                    wolfPast = g_pWolf->pos;
                                                                                    wolfPast.x +=
                                                                                        g_timeMachinePastOffset.x;
                                                                                    wolfPast.y +=
                                                                                        g_timeMachinePastOffset.y;
                                                                                    wolfPast.z +=
                                                                                        g_timeMachinePastOffset.z;
                                                                                    g_pWolf->HandleMessage(
                                                                                        this, MSG_TIMEMACHINE_ARRIVE,
                                                                                        &wolfPast);
                                                                                    if (wolfFrozen &&
                                                                                        g_pWolf->HandleMessage(
                                                                                            this, MSG_UNFREEZE, 0))
                                                                                        wolfFrozen = 0;
                                                                                    camPast = g_camPos;
                                                                                    camPast.x +=
                                                                                        g_timeMachinePastOffset.x;
                                                                                    camPast.y +=
                                                                                        g_timeMachinePastOffset.y;
                                                                                    camPast.z +=
                                                                                        g_timeMachinePastOffset.z;
                                                                                    spherePast = pos;
                                                                                    spherePast.x +=
                                                                                        g_timeMachinePastOffset.x;
                                                                                    spherePast.y +=
                                                                                        g_timeMachinePastOffset.y;
                                                                                    spherePast.z +=
                                                                                        g_timeMachinePastOffset.z;
                                                                                    SetPosition(&spherePast);
                                                                                    StartCamera(
                                                                                        g_camera.rot.z, g_camera.rot.y,
                                                                                        g_camera.rot.z, &camPast,
                                                                                        (u16)((ProjectionScale()
                                                                                               << 10) /
                                                                                              g_screen
                                                                                                  .GetGeometry(
                                                                                                      &geometry.past)
                                                                                                  ->width),
                                                                                        0);
                                                                                    if (Scenaric_FindByClass(
                                                                                            CLASSID_VISIBILITYMANAGER,
                                                                                            &fogPast, 1))
                                                                                        fogPast->Update();
                                                                                    releaseCamNextFrame = 1;
                                                                                    SetState(TMS_OPEN);
                                                                                }
                                                                            }
                                                                            break;
                                                                        case TMS_OPEN:
                                                                            if (AnimFlags(ANIM_F_FINISHED))
                                                                                SetState(TMS_OPENED);
                                                                            break;
                                                                        case TMS_OPENED:
                                                                            if (AnimFlags(ANIM_F_FINISHED)) {
                                                                                if (wolfInsideSinceArrival)
                                                                                    SetState(TMS_SETTLE);
                                                                                else
                                                                                    SetState(TMS_CLOSE);
                                                                            }
                                                                            break;
                                                                        case TMS_CLOSE:
                                                                            if (AnimFlags(ANIM_F_FINISHED)) {
                                                                                if (IsSoundPlaying(openSound))
                                                                                    StopSoundHandle(openSound);
                                                                                SetState(TMS_IDLE);
                                                                            }
                                                                            break;
                                                                        case TMS_SETTLE:
                                                                            if (AnimFlags(ANIM_F_FINISHED)) {
                                                                                if (IsSoundPlaying(openSound))
                                                                                    StopSoundHandle(openSound);
                                                                                SetState(TMS_WAIT_RETURN);
                                                                            }
                                                                            break;
                                                                        case TMS_WAIT_RETURN:
                                                                            if (!g_cinePlayer.IsActive()) {
                                                                                returnBox.min.x = pos.x - 185;
                                                                                returnBox.min.y = pos.y - 185;
                                                                                returnBox.min.z = pos.z - 185;
                                                                                returnBox.max.x = pos.x + 185;
                                                                                returnBox.max.y = pos.y + 185;
                                                                                returnBox.max.z = pos.z + 185;
                                                                                if (wolfInsideSinceArrival &&
                                                                                    returnBox.Contains(&g_pWolf->pos))
                                                                                    wolfInsideSinceArrival = 1;
                                                                                else
                                                                                    wolfInsideSinceArrival = 0;
                                                                                if (!wolfInsideSinceArrival &&
                                                                                    returnBox.Contains(&g_pWolf->pos)) {
                                                                                    followerTarget = pos;
                                                                                    if (dragon)
                                                                                        dragonFollows =
                                                                                            dragon->HandleMessage(
                                                                                                this,
                                                                                                MSG_TIMEMACHINE_CALL,
                                                                                                &followerTarget);
                                                                                    else
                                                                                        dragonFollows = 0;
                                                                                    if (gossamer)
                                                                                        gossamerFollows =
                                                                                            gossamer->HandleMessage(
                                                                                                this,
                                                                                                MSG_TIMEMACHINE_CALL,
                                                                                                &followerTarget);
                                                                                    else
                                                                                        gossamerFollows = 0;
                                                                                    if (IsSoundPlaying(humSound))
                                                                                        StopSoundHandle(humSound);
                                                                                    SetState(TMS_GATHER_FOLLOWERS);
                                                                                }
                                                                            }
                                                                            break;
                                                                        case TMS_GATHER_FOLLOWERS:
                                                                            followerBox.min.x = pos.x - 185;
                                                                            followerBox.min.y = pos.y - 185;
                                                                            followerBox.min.z = pos.z - 185;
                                                                            followerBox.max.x = pos.x + 185;
                                                                            followerBox.max.y = pos.y + 185;
                                                                            followerBox.max.z = pos.z + 185;
                                                                            if (dragon &&
                                                                                followerBox.Contains(&dragon->pos) &&
                                                                                dragonFollows) {
                                                                                dragon->HandleMessage(
                                                                                    this, MSG_TIMEMACHINE_LOCK, 0);
                                                                                SetState(TMS_DEPART_TO_PRESENT);
                                                                            } else if (gossamer &&
                                                                                       followerBox.Contains(
                                                                                           &gossamer->pos) &&
                                                                                       gossamerFollows) {
                                                                                gossamer->HandleMessage(
                                                                                    this, MSG_TIMEMACHINE_LOCK, 0);
                                                                                SetState(TMS_DEPART_TO_PRESENT);
                                                                            } else if (!gossamerFollows &&
                                                                                       !dragonFollows)
                                                                                SetState(TMS_DEPART_TO_PRESENT);
                                                                            if (gossamerFollows || dragonFollows) {
                                                                                followerWaitMs -= g_dtMs;
                                                                                if (followerWaitMs <= 0) {
                                                                                    if (gossamerFollows)
                                                                                        gossamer->HandleMessage(
                                                                                            this,
                                                                                            MSG_TIMEMACHINE_RELEASE, 0);
                                                                                    if (dragonFollows)
                                                                                        dragon->HandleMessage(
                                                                                            this,
                                                                                            MSG_TIMEMACHINE_RELEASE, 0);
                                                                                    SetState(TMS_DEPART_TO_PRESENT);
                                                                                }
                                                                            }
                                                                            break;
                                                                        case TMS_DEPART_TO_PRESENT:
                                                                            if (AnimFlags(ANIM_F_FINISHED)) {
                                                                                SwapToPresent();
                                                                                if (g_gameFlags & GF_UPDATE_OBJECTS) {
                                                                                    wolfPresent = g_pWolf->pos;
                                                                                    wolfPresent.x +=
                                                                                        g_timeMachineOffsetToPresent.x;
                                                                                    wolfPresent.y +=
                                                                                        g_timeMachineOffsetToPresent.y;
                                                                                    wolfPresent.z +=
                                                                                        g_timeMachineOffsetToPresent.z;
                                                                                    if (gossamerFollows) {
                                                                                        gossamerPresent = gossamer->pos;
                                                                                        gossamerArrivalQuery.obj =
                                                                                            gossamer;
                                                                                        useDefaultArrival = 1;
                                                                                        for (gossamerSeed = 0;
                                                                                             gossamerSeed < seedCount;
                                                                                             gossamerSeed++) {
                                                                                            if (seeds[gossamerSeed]->HandleMessage(
                                                                                                    this,
                                                                                                    MSG_SEED_QUERY_ARRIVAL,
                                                                                                    &gossamerArrivalQuery)) {
                                                                                                gossamerArrivalQuery.pos
                                                                                                    .x +=
                                                                                                    g_timeMachineOffsetToPresent
                                                                                                        .x;
                                                                                                gossamerArrivalQuery.pos
                                                                                                    .y +=
                                                                                                    g_timeMachineOffsetToPresent
                                                                                                        .y;
                                                                                                gossamerArrivalQuery.pos
                                                                                                    .z +=
                                                                                                    g_timeMachineOffsetToPresent
                                                                                                        .z;
                                                                                                gossamerPresent.x =
                                                                                                    gossamerArrivalQuery
                                                                                                        .pos.x;
                                                                                                gossamerPresent.y =
                                                                                                    gossamerArrivalQuery
                                                                                                        .pos.y;
                                                                                                gossamerPresent.z =
                                                                                                    gossamerArrivalQuery
                                                                                                        .pos.z;
                                                                                                useDefaultArrival = 0;
                                                                                                break;
                                                                                            }
                                                                                        }
                                                                                        if (useDefaultArrival) {
                                                                                            gossamerPresent.x +=
                                                                                                g_timeMachineOffsetToPresent
                                                                                                    .x;
                                                                                            gossamerPresent.y +=
                                                                                                g_timeMachineOffsetToPresent
                                                                                                    .y;
                                                                                            gossamerPresent.z +=
                                                                                                g_timeMachineOffsetToPresent
                                                                                                    .z;
                                                                                        }
                                                                                        gossamer->HandleMessage(
                                                                                            this,
                                                                                            MSG_TIMEMACHINE_ARRIVE,
                                                                                            &gossamerPresent);
                                                                                    }
                                                                                    if (dragonFollows) {
                                                                                        dragonPresent = dragon->pos;
                                                                                        dragonPresent.x +=
                                                                                            g_timeMachineOffsetToPresent
                                                                                                .x;
                                                                                        dragonPresent.y +=
                                                                                            g_timeMachineOffsetToPresent
                                                                                                .y;
                                                                                        dragonPresent.z +=
                                                                                            g_timeMachineOffsetToPresent
                                                                                                .z;
                                                                                        dragon->HandleMessage(
                                                                                            this,
                                                                                            MSG_TIMEMACHINE_ARRIVE,
                                                                                            &dragonPresent);
                                                                                    }
                                                                                    wolfArrivalQuery.obj = g_pWolf;
                                                                                    for (seedIndex = 0;
                                                                                         seedIndex < seedCount;
                                                                                         seedIndex++) {
                                                                                        if (seeds[seedIndex]
                                                                                                ->HandleMessage(
                                                                                                    this,
                                                                                                    MSG_SEED_QUERY_ARRIVAL,
                                                                                                    &wolfArrivalQuery)) {
                                                                                            wolfArrivalQuery.pos.x +=
                                                                                                g_timeMachineOffsetToPresent
                                                                                                    .x;
                                                                                            wolfArrivalQuery.pos.y +=
                                                                                                g_timeMachineOffsetToPresent
                                                                                                    .y;
                                                                                            wolfArrivalQuery.pos.z +=
                                                                                                g_timeMachineOffsetToPresent
                                                                                                    .z;
                                                                                            wolfPresent.x =
                                                                                                wolfArrivalQuery.pos.x;
                                                                                            wolfPresent.y =
                                                                                                wolfArrivalQuery.pos.y;
                                                                                            wolfPresent.z =
                                                                                                wolfArrivalQuery.pos.z;
                                                                                            break;
                                                                                        }
                                                                                    }
                                                                                    g_pWolf->HandleMessage(
                                                                                        this, MSG_TIMEMACHINE_ARRIVE,
                                                                                        &wolfPresent);
                                                                                    if (wolfFrozen &&
                                                                                        g_pWolf->HandleMessage(
                                                                                            this, MSG_UNFREEZE, 0))
                                                                                        wolfFrozen = 0;
                                                                                    camPresent = g_camPos;
                                                                                    camPresent.x +=
                                                                                        g_timeMachineOffsetToPresent.x;
                                                                                    camPresent.y +=
                                                                                        g_timeMachineOffsetToPresent.y;
                                                                                    camPresent.z +=
                                                                                        g_timeMachineOffsetToPresent.z;
                                                                                    spherePresent = pos;
                                                                                    spherePresent.x +=
                                                                                        g_timeMachineOffsetToPresent.x;
                                                                                    spherePresent.y +=
                                                                                        g_timeMachineOffsetToPresent.y;
                                                                                    spherePresent.z +=
                                                                                        g_timeMachineOffsetToPresent.z;
                                                                                    SetPosition(&spherePresent);
                                                                                    StartCamera(
                                                                                        g_camera.rot.z, g_camera.rot.y,
                                                                                        g_camera.rot.z, &camPresent,
                                                                                        (u16)((ProjectionScale()
                                                                                               << 10) /
                                                                                              g_screen
                                                                                                  .GetGeometry(
                                                                                                      &geometry.present)
                                                                                                  ->width),
                                                                                        0);
                                                                                    if (Scenaric_FindByClass(
                                                                                            CLASSID_VISIBILITYMANAGER,
                                                                                            &fogPresent, 1))
                                                                                        fogPresent->Update();
                                                                                    releaseCamNextFrame = 1;
                                                                                    SetState(TMS_OPEN);
                                                                                }
                                                                            }
                                                                            break;
                                                                    }
                                                                    AdvanceAnim();
                                                                    switch (GetAnimId()) {
                                                                        case ABTIME01_ANIM_INSIDE:
                                                                            billboardAnim = ABTIME02_ANIM_INSIDE;
                                                                            break;
                                                                        case ABTIME01_ANIM_APPEAR:
                                                                            billboardAnim = ABTIME02_ANIM_APPEAR;
                                                                            break;
                                                                        case ABTIME01_ANIM_CHRONO:
                                                                            billboardAnim = ABTIME02_ANIM_CHRONO;
                                                                            break;
                                                                        case ABTIME01_ANIM_STAND:
                                                                            billboardAnim = ABTIME02_ANIM_STAND;
                                                                            break;
                                                                        case ABTIME01_ANIM_STAND2:
                                                                            billboardAnim = ABTIME02_ANIM_STAND2;
                                                                            break;
                                                                        case ABTIME01_ANIM_OUT:
                                                                            billboardAnim = ABTIME02_ANIM_OUT;
                                                                            break;
                                                                        case ABTIME01_ANIM_OUT1:
                                                                            billboardAnim = ABTIME02_ANIM_OUT1;
                                                                            break;
                                                                        case ABTIME01_ANIM_OUT2:
                                                                            billboardAnim = ABTIME02_ANIM_OUT2;
                                                                            break;
                                                                    }
                                                                    if (billboardAnim != billboard.GetAnimId()) {
                                                                        billboard.pos = pos;
                                                                        billboard.PlayAnim(billboardAnim,
                                                                                           AnimFlags(ANIM_F_LOOP), 1);
                                                                    }
                                                                    billboard.AdvanceAnim();
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
void TimeMachineSphere::SetState(u8 next)
{
    state = next;
    switch (next) {
        case TMS_IDLE:
            PlayAnim(ABTIME01_ANIM_STAND2, 1, 0);
            SetVisible(0);
            break;
        case TMS_DEPART_TO_PAST:
            wolfFrozen = 0;
            SetVisible(1);
            if (bull)
                bull->HandleMessage(this, MSG_TIMEMACHINE_CALL, 0);
            if (!g_pWolf->HandleMessage(this, MSG_WOLF_IS_DEAD, 0)) {
                if (!wolfFrozen && g_pWolf->HandleMessage(this, MSG_FREEZE, 0))
                    wolfFrozen = 1;
                else
                    wolfFrozen = 0;
            }
            SetPosition(&g_pWolf->pos);
            g_pWolf->HandleMessage(this, MSG_TIMEMACHINE_LOCK, 0);
            PlayAnim(ABTIME01_ANIM_CHRONO, 0, 1);
            break;
        case TMS_OPEN:
            openSound = Sound_Play(SND_TIMEMACHINE_OPEN, this, 0xff, SNDF_POSITIONAL, 0x1000);
            PlayAnim(ABTIME01_ANIM_APPEAR, 0, 1);
            break;
        case TMS_OPENED:
            PlayAnim(ABTIME01_ANIM_OUT, 0, 1);
            break;
        case TMS_CLOSE:
            PlayAnim(ABTIME01_ANIM_OUT1, 0, 1);
            break;
        case TMS_SETTLE:
            PlayAnim(ABTIME01_ANIM_OUT2, 0, 0);
            break;
        case TMS_WAIT_RETURN:
            humSound = Sound_Play(SND_TIMEMACHINE_HUM, this, 0xff, SNDF_LOOP | SNDF_POSITIONAL, 0x1000);
            PlayAnim(ABTIME01_ANIM_STAND, 1, 1);
            break;
        case TMS_GATHER_FOLLOWERS:
            wolfFrozen = 0;
            followerWaitMs = 3000;
            if (!g_pWolf->HandleMessage(this, MSG_WOLF_IS_DEAD, 0)) {
                if (!wolfFrozen && g_pWolf->HandleMessage(this, MSG_FREEZE, 0))
                    wolfFrozen = 1;
                else
                    wolfFrozen = 0;
            }
            g_pWolf->HandleMessage(this, MSG_TIMEMACHINE_LOCK, 0);
            break;
        case TMS_DEPART_TO_PRESENT:
            PlayAnim(ABTIME01_ANIM_INSIDE, 0, 1);
            break;
    }
}
s32 TimeMachineSphere::HandleMessage(ScnObject *, u32 msg, void *)
{
    switch (msg) {
        case MSG_TIMEMACHINE_START:
            SetState(TMS_DEPART_TO_PAST);
            break;
        case MSG_FREEZE:
            wolfFrozen = 0;
            return 1;
    }
    return 0;
}
/* 0x4fba8e. Era query ignores height; outside both era boxes is present. */
s32 TimeMachine_IsInPresent(ScnObject *object)
{
    if (ContainsXZ(g_timeMachineZonePresent, &object->pos))
        return 1;
    if (ContainsXZ(g_timeMachinePastEraZone, &object->pos))
        return 0;
    return 1;
}
void TimeMachineSphere::SwapToPast()
{
    TimeTravelArg args;
    u8 i;
    Transition_Start();
    for (i = 0; i < 16; i++)
        if (pairs[i].present && pairs[i].past) {
            args.obj = pairs[i].present;
            args.pos = pairs[i].present->pos;
            args.pos.x += g_timeMachinePastOffset.x;
            args.pos.y += g_timeMachinePastOffset.y;
            args.pos.z += g_timeMachinePastOffset.z;
            pairs[i].past->HandleMessage(this, MSG_TIMEMACHINE_SWAP, &args);
            args.obj = pairs[i].past;
            args.pos = pairs[i].past->pos;
            args.pos.x += g_timeMachineOffsetToPresent.x;
            args.pos.y += g_timeMachineOffsetToPresent.y;
            args.pos.z += g_timeMachineOffsetToPresent.z;
            pairs[i].present->HandleMessage(this, MSG_TIMEMACHINE_SWAP_OUT, &args);
        }
}
/* BYTES(slot-scope, inferred): the nested block(s) only order the frame: their locals are allocated after the enclosing scope's */
/* BYTES(dead-code): moved is stored and never read, as in the original */
void TimeMachineSphere::SwapToPresent()
{
    Sheep **sheep;
    {
        u8 count;
        {
            TimeTravelArg args;
            {
                u8 i;
                {
                    u32 moved;
                    Transition_Start();
                    for (i = 0; i < 16; i++)
                        if (pairs[i].present && pairs[i].past) {
                            args.obj = pairs[i].past;
                            args.pos = pairs[i].past->pos;
                            args.pos.x += g_timeMachineOffsetToPresent.x;
                            args.pos.y += g_timeMachineOffsetToPresent.y;
                            args.pos.z += g_timeMachineOffsetToPresent.z;
                            pairs[i].present->HandleMessage(this, MSG_TIMEMACHINE_SWAP, &args);
                            args.obj = pairs[i].present;
                            args.pos = pairs[i].present->pos;
                            args.pos.x += g_timeMachinePastOffset.x;
                            args.pos.y += g_timeMachinePastOffset.y;
                            args.pos.z += g_timeMachinePastOffset.z;
                            pairs[i].past->HandleMessage(this, MSG_TIMEMACHINE_SWAP_OUT, &args);
                        }
                    count = g_sheepCount;
                    moved = g_sheepMovedMask;
                    sheep = g_sheepTable;
                    for (i = 0; i < count; i++)
                        sheep[i]->HandleMessage(this, MSG_TIMEMACHINE_SWAP, 0);
                }
            }
        }
    }
}
void TimeMachineSphere::Render(Camera *view)
{
    ScnBody::Render(view);
    billboard.RenderFacingCamera(view, 0, 0, 0);
}
void TimeMachineSphere::Reset()
{
    TimeTravelArg args;
    u8 i;
    StopSoundHandle(humSound);
    StopSoundHandle(openSound);
    if (!TimeMachine_IsInPresent(this))
        for (i = 0; i < 16; i++)
            if (pairs[i].past && pairs[i].present) {
                args.obj = pairs[i].past;
                args.pos = pairs[i].past->pos;
                args.pos.x += g_timeMachineOffsetToPresent.x;
                args.pos.y += g_timeMachineOffsetToPresent.y;
                args.pos.z += g_timeMachineOffsetToPresent.z;
                pairs[i].present->HandleMessage(this, MSG_TIMEMACHINE_SWAP, &args);
            }
    SetState(TMS_IDLE);
}
ScnObject *TimeMachineSphere_Create(void *record)
{
    TimeMachineSphere *object = new TimeMachineSphere;
    object = (TimeMachineSphere *)object->Init(record, 0); /* cast kept: Init returns the object as its ScnBody base */
    return object;
}
