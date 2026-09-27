/* PAL PC Pipe, 0x4dac80-0x4dc4d4. */
/* BYTES: flow, view. */
/* BYTES(view): view: fields in the class padding (data/structs/pipe.csv), with the instructions that prove them (the access widths are the original's) */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *inst, Animator *animator, u16 id, u32 opts);
#include "../engine/sound_mgr.h"
#include "../engine/scn_tools.h"
#include "../engine/id_list.h"

#define SDW_MEMBERS_ScnObject static void *operator new(u32 size);


#define SDW_MEMBERS_ZoneList void Load(u32 id);

#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_SETBOXCOLLIDE_S32 1
#define SDW_INLINE_SCNOBJECT_STOPSOUND_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_SETBOXCOLLIDE_S32
#undef SDW_INLINE_SCNOBJECT_STOPSOUND_U16
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32

/* Pipe's fields (data/structs/Pipe.csv), with the instructions that prove them:
 * +64 Box*[2] (4daca3,4dacb7); +6c ZoneList (4dad37..4dad57);
 * +94 Box* model bounds (4dad8f); +98 ScnObject* ball (4daf5b);
 * +9c/+a2/+a8/+ae Vec3s entry/direction/exit/center (4daf82..4db268);
 * +b4/+b8 s32 forbidden push axes (4dacd6,4dad0b); +c0..c5 u8 state data;
 * +cc/+d0 u32 delay/start time (4dbd5c,4dc24c). */
#define PIPE_ABS(a) ((a) >= 0 ? (a) : -(a))

s32 ObjGrid_QueryBoxOverlap(CollBox *query, ScnObject **out);             /* 0x510f34 */
s32 Vec3s_DistXZ(Vec3s *a, Vec3s *b);                                     /* 0x5157bd */
u16 Sound_Play(u16 soundId, void *owner, u16 volume, u8 flags, s32 rate); /* 0x5491b8 */
extern u32 g_gameTime;

#define SDW_INLINE_FREE_PROPU32_VOID_U32 2
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_PROPU32_VOID_U32
#define SDW_INLINE_ZONELIST_LOAD_U32 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_LOAD_U32
#define SDW_INLINE_ZONELIST_CONTAINS_VEC3S 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_CONTAINS_VEC3S
#define SDW_INLINE_SCNOBJECT_GETMODELBOX 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETMODELBOX
#define SDW_INLINE_FREE_SOUNDISPLAYING_U16 1
#include "../engine/sound_mgr_inlines.h"
#undef SDW_INLINE_FREE_SOUNDISPLAYING_U16

/* 0x4dac80 Pipe_PostLoadInit */
void Pipe::PostLoadInit()
{
    void *p;
    u32 q;
    p = record;
    /* cast kept (both): Box and CollBox are two views of one 16-byte record (CollBox has the methods) */
    mouths[0] = (CollBox *)Scn_GetPropBox(p, 0);
    mouths[1] = (CollBox *)Scn_GetPropBox(p, 4);
    q = PropU32(p, 0xc);
    if (q)
        forbidX = 1;
    else
        forbidX = 0;
    q = PropU32(p, 0x10);
    if (q)
        forbidZ = 1;
    else
        forbidZ = 0;
    movementZones.Load(PropU32(p, 8));
    soundHandle = 0;
    modelBox = GetModelBox();
    occupied = 0;
    armed = 0;
    pushedThisFrame = 0;
}

/* 0x4dadba Pipe_Update. Preserve separate quadrant cases and the state-1-only push reset. */
/* BYTES(flow): the quadrant cases stay separate and the push reset stays state-1-only, as the original */
void Pipe::Update()
{
    ScnObject *p[64];
    s32 q;
    Vec3s r;
    if (armed) {
        switch (pipeState) {
            case PIPE_ST_IDLE:
                for (mouthIndex = 0; mouthIndex < 2; mouthIndex++) {
                    workBox = *mouths[mouthIndex];
                    q = ObjGrid_QueryBoxOverlap(&workBox, p);
                    if (q > 0 && occupied != 1) {
                        for (objectIndex = 0; objectIndex < q; objectIndex++) {
                            if (p[objectIndex]->GetClassId() == CLASSID_CANNONBALL) {
                                ball = p[objectIndex];
                                entryPos.x = (workBox.max.x + workBox.min.x) / 2;
                                entryPos.y = (workBox.max.y + workBox.min.y) / 2;
                                entryPos.z = (workBox.max.z + workBox.min.z) / 2;
                                workBox.min.x = pos.x + modelBox->min.x;
                                workBox.min.y = pos.y + modelBox->min.y;
                                workBox.min.z = pos.z + modelBox->min.z;
                                workBox.max.x = pos.x + modelBox->max.x;
                                workBox.max.y = pos.y + modelBox->max.y;
                                workBox.max.z = pos.z + modelBox->max.z;
                                centerPos.x = (workBox.max.x + workBox.min.x) / 2;
                                centerPos.y = (workBox.max.y + workBox.min.y) / 2;
                                centerPos.z = (workBox.max.z + workBox.min.z) / 2;
                                if (mouths[mouthIndex]->min.x == mouths[0]->min.x &&
                                    mouths[mouthIndex]->max.x == mouths[0]->max.x)
                                    workBox = *mouths[1];
                                else
                                    workBox = *mouths[0];
                                exitPos.x = (workBox.max.x + workBox.min.x) / 2;
                                exitPos.y = (workBox.max.y + workBox.min.y) / 2;
                                exitPos.z = (workBox.max.z + workBox.min.z) / 2;
                                r.x = centerPos.x - entryPos.x;
                                r.y = centerPos.y - entryPos.y;
                                r.z = centerPos.z - entryPos.z;
                                exitBoxSpan = Vec3s_DistXZ(&workBox.min, &workBox.max);
                                if (PIPE_ABS(r.x) < PIPE_ABS(r.z)) {
                                    if (r.x > 0) {
                                        if (exitPos.x > centerPos.x && entryPos.z > centerPos.z)
                                            entryPos.z -= (s16)(exitBoxSpan / 8);
                                        else
                                            entryPos.z += (s16)(exitBoxSpan / 8);
                                    } else {
                                        if (exitPos.x < centerPos.x && entryPos.z > centerPos.z)
                                            entryPos.z -= (s16)(exitBoxSpan / 8);
                                        else
                                            entryPos.z += (s16)(exitBoxSpan / 8);
                                    }
                                } else {
                                    if (r.z > 0) {
                                        if (exitPos.x > centerPos.x)
                                            entryPos.x += (s16)(exitBoxSpan / 8);
                                        else
                                            entryPos.x -= (s16)(exitBoxSpan / 8);
                                    } else {
                                        if (exitPos.x > centerPos.x)
                                            entryPos.x += (s16)(exitBoxSpan / 8);
                                        else
                                            entryPos.x -= (s16)(exitBoxSpan / 8);
                                    }
                                }
                                if (!p[objectIndex]->HandleMessage(this, MSG_CB_ENTER_PIPE, &entryPos))
                                    occupied = 1;
                            }
                        }
                    }
                }
                break;
            case PIPE_ST_EJECT:
                Sound_Play(SND_PIPE_EJECT, this, 0xff, SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 0x1000);
                launchDir.x = 0;
                launchDir.y = 0;
                launchDir.z = 0;
                if (centerPos.x > entryPos.x && centerPos.z > entryPos.z && centerPos.x < exitPos.x &&
                    centerPos.z < exitPos.z) {
                    if (PIPE_ABS(centerPos.x - entryPos.x) > PIPE_ABS(centerPos.z - entryPos.z)) {
                        launchDir.z = 0xfff;
                        exitAxis = 2;
                    } else {
                        launchDir.x = 0xfff;
                        exitAxis = 1;
                    }
                }
                if (centerPos.x < entryPos.x && centerPos.z < entryPos.z && centerPos.x > exitPos.x &&
                    centerPos.z > exitPos.z) {
                    if (PIPE_ABS(centerPos.x - entryPos.x) < PIPE_ABS(centerPos.z - entryPos.z)) {
                        launchDir.x = -0xfff;
                        exitAxis = 1;
                    } else {
                        launchDir.z = -0xfff;
                        exitAxis = 2;
                    }
                }
                if (centerPos.x > entryPos.x && centerPos.z < entryPos.z && centerPos.x < exitPos.x &&
                    centerPos.z > exitPos.z) {
                    if (PIPE_ABS(centerPos.x - entryPos.x) < PIPE_ABS(centerPos.z - entryPos.z)) {
                        launchDir.x = 0xfff;
                        exitAxis = 1;
                    } else {
                        launchDir.z = -0xfff;
                        exitAxis = 2;
                    }
                }
                if (centerPos.x < entryPos.x && centerPos.z > entryPos.z && centerPos.x > exitPos.x &&
                    centerPos.z < exitPos.z) {
                    if (PIPE_ABS(centerPos.x - entryPos.x) > PIPE_ABS(centerPos.z - entryPos.z)) {
                        launchDir.z = 0xfff;
                        exitAxis = 2;
                    } else {
                        launchDir.x = -0xfff;
                        exitAxis = 1;
                    }
                }
                ball->HandleMessage(this, MSG_CB_PLACE, &exitPos);
                launchDelay = 0x999;
                /* cast kept: HandleMessage's arg is a void *; this message passes the launch delay (ms) in it */
                ball->HandleMessage(this, MSG_CB_PIPE_TRANSIT, (void *)launchDelay);
                ball->HandleMessage(this, MSG_CB_PIPE_CAMERA, &launchDir);
                SetState(PIPE_ST_LAUNCH_WAIT);
                break;
            case PIPE_ST_LAUNCH_WAIT:
                if (g_gameTime - launchStart > launchDelay) {
                    SetBoxCollide(0);
                    ball->HandleMessage(this, MSG_CB_LAUNCH, &launchDir);
                    SetState(PIPE_ST_IDLE);
                } else
                    SetState(PIPE_ST_LAUNCH_WAIT);
                break;
        }
    }
    if (pipeState == PIPE_ST_TRANSIT) {
        if (!::SoundIsPlaying(soundHandle))
            soundHandle = Sound_Play(SND_SCOPUSH, this, 0xff, SNDF_POSITIONAL, 0x1000);
        if (!pushedThisFrame) {
            if (::SoundIsPlaying(soundHandle))
                StopSound(soundHandle);
            SetState(PIPE_ST_IDLE);
        }
        pushedThisFrame = 0;
    }
    AdvanceAnim();
}

/* 0x4dbf90 Pipe_HandleMessage */
s32 Pipe::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    Vec3s p;
    Vec3s q;
    switch (msgId) {
        case MSG_QUERY_ACTION:
            if (sender->GetClassId() == CLASSID_WOLF)
                return CTX_PUSH;
            break;
        case MSG_PUSH:
            p = *(Vec3s *)arg; /* cast kept: the message arg is a void *; MSG_PUSH passes the push delta */
            if (p.x && forbidX)
                return 2;
            if (p.z && forbidZ)
                return 2;
            q.x = p.x + pos.x;
            q.y = p.y + pos.y;
            q.z = p.z + pos.z;
            if (movementZones.Contains(&q)) {
                Collide_ResolveMove(&p, 0, 0xb54, RESOLVE_SLIDE_ALL, 0, 0, 10, 0, 0);
                if (p.y < 0)
                    p.y = 0;
                Translate(&p);
                mouths[0]->min.x += p.x;
                mouths[0]->min.y += p.y;
                mouths[0]->min.z += p.z;
                mouths[0]->max.x += p.x;
                mouths[0]->max.y += p.y;
                mouths[0]->max.z += p.z;
                mouths[1]->min.x += p.x;
                mouths[1]->min.y += p.y;
                mouths[1]->min.z += p.z;
                mouths[1]->max.x += p.x;
                mouths[1]->max.y += p.y;
                mouths[1]->max.z += p.z;
                pushedThisFrame = 1;
                if (pipeState != PIPE_ST_TRANSIT)
                    SetState(PIPE_ST_TRANSIT);
                return 1;
            }
            return 2;
        case MSG_PIPE_EJECT:
            SetState(PIPE_ST_EJECT);
            launchStart = g_gameTime;
            break;
        case MSG_BALL_RETURNED:
            SetBoxCollide(1);
            SetState(PIPE_ST_IDLE);
            occupied = 0;
            armed = 0;
            break;
        case MSG_PIPE_ARM:
            SetState(PIPE_ST_IDLE);
            armed = 1;
            break;
    }
    return 0;
}

/* 0x4dc33e Pipe_SetState */
void Pipe::SetState(u8 newState)
{
    pipeState = newState;
    switch (newState) {
        case PIPE_ST_EJECT:
            if (exitAxis == 1)
                PlayAnim(ATUYAU01_ANIM_THROW, 0, 1);
            else
                PlayAnim(ATUYAU01_ANIM_THROW2, 0, 1);
            break;
    }
}

/* 0x4dc403 Pipe_Reset. Preserve clearing the handle before stopping sound zero. */
void Pipe::Reset()
{
    soundHandle = 0;
    StopSound(soundHandle);
    SetBoxCollide(1);
}

/* 0x4dc46e Pipe_Create */
ScnObject *Pipe_Create(void *record)
{
    Pipe *obj = new Pipe;
    obj = (Pipe *)obj->Init(record, 0); /* cast kept: Init returns the ScnBody * base of this object */
    return obj;
}
