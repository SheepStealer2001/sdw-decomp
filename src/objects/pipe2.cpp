/* PAL PC 0x4dc4e0-0x4de039. */
/* BYTES: view. */
/* BYTES(view): view: fields in the class padding (data/structs/pipe2.csv), with the instructions that prove them (the access widths are the original's) */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *inst, Animator *animator, u16 id, u32 opts);

#define SDW_MEMBERS_ScnObject            \
    static void *operator new(u32 size); \
    void SetUpdateMode(s32 mode);

#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_SETBOXCOLLIDE_S32 1
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_SETBOXCOLLIDE_S32
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32

/* Fields in the class padding (data/structs/Pipe2.csv), with the instructions
 * that prove them: +64 Box*[2] mouths;
 * +7c Box saved mouth (4dc7ef); +8c Box* model bounds (4dc51c);
 * +b0/+b4 Box* original mouths (4dc533,4dc54a); +b8 ScnObject* ball (4dc894);
 * +bc/+c2/+c8/+ce Vec3s entry/direction/exit/center (4dc8bb..4dcb6d);
 * +d4 Vec3s home position (4dc5a7); +e6/+ec Vec3s mouth centers (4ddc74,4ddce3);
 * +f2/+f8 Vec3s turn offsets (4ddd68,4dddc8); +fe Vec3s pivot (4ddd3d);
 * +104 u8 occupied; +10c s32 freeze reply; +110..113 u8 indexes/state/axis;
 * +118 s32 exit span; +11c/+120 u32 launch delay/start. */
#define PIPE_ABS(a) ((a) >= 0 ? (a) : -(a))

#include "../engine/scn_tools.h"
#include "world_draw.h"
#include "../engine/fixed_math.h"
s32 ObjGrid_QueryBoxOverlap(CollBox *query, ScnObject **out);             /* 0x510f34 */
s32 Vec3s_DistXZ(Vec3s *a, Vec3s *b);                                     /* 0x5157bd */
u16 Sound_Play(u16 soundId, void *owner, u16 volume, u8 flags, s32 rate); /* 0x5491b8 */
extern u32 g_gameTime;
extern Wolf *g_pWolf;

#define SDW_INLINE_SCNOBJECT_GETMODELBOX 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETMODELBOX

/* 0x4dc4e0 Pipe2_PostLoadInit */
void Pipe2::PostLoadInit()
{
    void *p = record;
    modelBox = GetModelBox();
    /* cast kept: Box and CollBox are two views of the same 16-byte record */
    originalMouths[0] = (CollBox *)Scn_GetPropBox(p, 0);
    originalMouths[1] = (CollBox *)Scn_GetPropBox(p, 4);
    mouths[0] = originalMouths[0];
    mouths[1] = originalMouths[1];
    modelBox = GetModelBox();
    homePos = pos;
    SetUpdateMode(SCN_UPD_ALWAYS);
    occupied = 0;
    armed = 0;
    wolfFrozen = 0;
}

/* 0x4dc69a Pipe2_Update */
void Pipe2::Update()
{
    ScnObject *p[64];
    s32 q;
    Vec3s r;
    Vec3s s;
    if (armed) {
        switch (pipeState) {
            case PIPE2_ST_IDLE:
                for (mouthIndex = 0; mouthIndex < 2; mouthIndex++) {
                    if (mouths[mouthIndex]->flags & COLLBOX_NONSOLID) {
                        queryBox = *mouths[mouthIndex];
                        q = ObjGrid_QueryBoxOverlap(&queryBox, p);
                        if (q > 0 && occupied != 1) {
                            savedMouth = *mouths[mouthIndex];
                            for (objectIndex = 0; objectIndex < q; objectIndex++) {
                                if (p[objectIndex]->GetClassId() == CLASSID_CANNONBALL) {
                                    ball = p[objectIndex];
                                    entryPos.x = (queryBox.max.x + queryBox.min.x) / 2;
                                    entryPos.y = (queryBox.max.y + queryBox.min.y) / 2;
                                    entryPos.z = (queryBox.max.z + queryBox.min.z) / 2;
                                    queryBox.min.x = pos.x + modelBox->min.x;
                                    queryBox.min.y = pos.y + modelBox->min.y;
                                    queryBox.min.z = pos.z + modelBox->min.z;
                                    queryBox.max.x = pos.x + modelBox->max.x;
                                    queryBox.max.y = pos.y + modelBox->max.y;
                                    queryBox.max.z = pos.z + modelBox->max.z;
                                    centerPos.x = (queryBox.max.x + queryBox.min.x) / 2;
                                    centerPos.y = (queryBox.max.y + queryBox.min.y) / 2;
                                    centerPos.z = (queryBox.max.z + queryBox.min.z) / 2;
                                    if (savedMouth.min.x == mouths[0]->min.x && savedMouth.max.x == mouths[0]->max.x)
                                        queryBox = *mouths[1];
                                    else
                                        queryBox = *mouths[0];
                                    exitPos.x = (queryBox.max.x + queryBox.min.x) / 2;
                                    exitPos.y = (queryBox.max.y + queryBox.min.y) / 2;
                                    exitPos.z = (queryBox.max.z + queryBox.min.z) / 2;
                                    r.x = centerPos.x - entryPos.x;
                                    r.y = centerPos.y - entryPos.y;
                                    r.z = centerPos.z - entryPos.z;
                                    exitBoxSpan = Vec3s_DistXZ(&queryBox.min, &queryBox.max);
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
                }
                break;
            case PIPE2_ST_EJECT:
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
                /* cast kept: HandleMessage's arg is a void *; this message passes a number in it */
                ball->HandleMessage(this, MSG_CB_PIPE_TRANSIT, (void *)launchDelay);
                ball->HandleMessage(this, MSG_CB_PIPE_CAMERA, &launchDir);
                SetState(PIPE2_ST_LAUNCH_WAIT);
                break;
            case PIPE2_ST_LAUNCH_WAIT:
                if (g_gameTime - launchStart > launchDelay) {
                    SetBoxCollide(0);
                    ball->HandleMessage(this, MSG_CB_LAUNCH, &launchDir);
                    SetState(PIPE2_ST_IDLE);
                } else
                    SetState(PIPE2_ST_LAUNCH_WAIT);
                break;
        }
    }
    switch (pipeState) {
        case PIPE2_ST_SPIN_RESET:
            s = rot;
            s.y = s.y + 0x20;
            spinAngle = spinAngle + 0x20;
            if (spinAngle == 0x400) {
                if (wolfFrozen) {
                    g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0);
                    wolfFrozen = 0;
                }
                SetState(PIPE2_ST_IDLE);
            }
            rot = s;
            break;
        case PIPE2_ST_INIT_MOUTHS:
            MoveBoxWithSpin(mouths[0], turnOffset1, turnPivot, 0x400);
            MoveBoxWithSpin(mouths[1], turnOffset2, turnPivot, 0x400);
            SetState(PIPE2_ST_SPIN_RESET);
            break;
    }
    AdvanceAnim();
}

/* 0x4dd954 Pipe2_HandleMessage */
s32 Pipe2::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    switch (msgId) {
        case MSG_QUERY_ACTION:
            if (sender->GetClassId() == CLASSID_WOLF && pipeState != PIPE2_ST_SPIN_RESET)
                return CTX_PIPE;
            break;
        case MSG_USE:
            SetState(PIPE2_ST_INIT_MOUTHS);
            break;
        case MSG_PIPE_EJECT:
            SetState(PIPE2_ST_EJECT);
            launchStart = g_gameTime;
            break;
        case MSG_BALL_RETURNED:
            SetBoxCollide(1);
            SetState(PIPE2_ST_IDLE);
            homePos = pos;
            occupied = 0;
            armed = 0;
            break;
        case MSG_PIPE_ARM:
            SetState(PIPE2_ST_IDLE);
            armed = 1;
            break;
        case MSG_PIPE_RELEASE:
            SetBoxCollide(1);
            occupied = 0;
            break;
        case MSG_FREEZE:
            if (sender->GetClassId() == CLASSID_WOLF) {
                wolfFrozen = 0;
                return 1;
            }
            break;
    }
    return 0;
}

/* 0x4ddb61 Pipe2_SetState */
void Pipe2::SetState(u8 newState)
{
    pipeState = newState;
    switch (newState) {
        case PIPE2_ST_EJECT:
            if (exitAxis == 1)
                PlayAnim(ATUYAU02_ANIM_THROW, 0, 1);
            else
                PlayAnim(ATUYAU02_ANIM_THROW2, 0, 1);
            break;
        case PIPE2_ST_SPIN_RESET:
            spinAngle = 0;
            unk116 = 0;
            break;
        case PIPE2_ST_INIT_MOUTHS:
            mouthCenter1.x = (mouths[0]->max.x + mouths[0]->min.x) / 2;
            mouthCenter1.y = (mouths[0]->max.y + mouths[0]->min.y) / 2;
            mouthCenter1.z = (mouths[0]->max.z + mouths[0]->min.z) / 2;
            mouthCenter2.x = (mouths[1]->max.x + mouths[1]->min.x) / 2;
            mouthCenter2.y = (mouths[1]->max.y + mouths[1]->min.y) / 2;
            mouthCenter2.z = (mouths[1]->max.z + mouths[1]->min.z) / 2;
            turnPivot = pos;
            turnOffset1.x = mouthCenter1.x - turnPivot.x;
            turnOffset1.y = mouthCenter1.y - turnPivot.y;
            turnOffset1.z = mouthCenter1.z - turnPivot.z;
            turnOffset2.x = mouthCenter2.x - turnPivot.x;
            turnOffset2.y = mouthCenter2.y - turnPivot.y;
            turnOffset2.z = mouthCenter2.z - turnPivot.z;
            wolfFrozen = g_pWolf->HandleMessage(this, MSG_FREEZE, 0);
            break;
    }
}

/* 0x4dde37 Pipe2_MoveBoxWithSpin. Vec3s arguments are passed by value (ret 0x18). */
void Pipe2::MoveBoxWithSpin(CollBox *box, Vec3s offset, Vec3s pivot, s16 angle)
{
    Vec3s p;
    Vec3s q;
    Vec3s r;
    Vec3s s;
    Vec4i t;
    Mat34s u;
    p.x = 0;
    p.y = 0;
    p.z = 0;
    p.y = angle;
    t.x = offset.x;
    t.y = offset.y;
    t.z = offset.z;
    s.x = (box->max.x + box->min.x) / 2;
    s.y = (box->max.y + box->min.y) / 2;
    s.z = (box->max.z + box->min.z) / 2;
    Mat34s_FromEulerScaled(&p, &u, 0);
    /* cast kept: an in-place transform: x, y, z of the Vec4i are read, the Vec4i is written */
    Mat34s_TransformVec3i(&u, (Vec3i *)&t, &t);
    q.x = pivot.x + (s16)t.x;
    q.y = pivot.y + (s16)t.y;
    q.z = pivot.z + (s16)t.z;
    r.x = q.x - s.x;
    r.y = q.y - s.y;
    r.z = q.z - s.z;
    box->min.x += r.x;
    box->min.z += r.z;
    box->max.x += r.x;
    box->max.z += r.z;
}

/* 0x4ddf8a Pipe2_Reset */
void Pipe2::Reset()
{
    wolfFrozen = 0;
    SetBoxCollide(1);
}

/* 0x4ddfd3 Pipe2_Create */
ScnObject *Pipe2_Create(void *record)
{
    Pipe2 *obj = new Pipe2;
    obj = (Pipe2 *)obj->Init(record, 0); /* cast kept: Init returns the object as a ScnObject * */
    return obj;
}
