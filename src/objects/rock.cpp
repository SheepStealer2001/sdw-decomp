/*
 * T198 - original object Rock.cpp (guessed name), one translation unit.
 *   .text  0x4e25b0-0x4e4803 (Rock_PlayRollSound .. Rock_Create, then LaunchArc_Step 0x4e4720, contiguous with
 *          Rock_Create, so taken as this object's last main-.text function)
 *   .rdata 0x576910-0x576958 (??_7Rock, then the ??_7ScnLogicShadowed COMDAT, first emitted here)
 *   .data  0x57b704-0x57b710 (this TU's copy of the Cine header's 9-byte opcode stride table, + 3 pad)
 */
/* BYTES: layout, slot-group. */
/* BYTES(layout): the Cine.h header static: every object including the cinematic header carries this copy in its .data, referenced or not */
/* PAL PC SheepD3D.exe. */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class CollBox;
class Instance;
struct Animator;
u32 Anim_Start(Instance *inst, Animator *animator, u16 id, u32 opts); /* 0x550196 */
#include "../engine/sound_mgr.h"
#include "../engine/id_list.h"
#include "../engine/scn_tools.h"
#include "../engine/scenaric.h"
#include "world_draw.h"
#include "../engine/shadow.h"
#include "camera.h"
#include "../engine/maths.h"
#include "../engine/cine.h"

#define SDW_MEMBERS_ScnObject            \
    static void *operator new(u32 size); \
    CollBox *GetModelBoxes(u32 *count);  \
    s16 GetTop();                        \
    void SetRotation(Vec3s *rotation);   \
    void SetUpdateMode(u8 mode);

#define SDW_MEMBERS_Shadow   \
    s16 GetHeight()          \
    {                        \
        return groundPos.y;  \
    }                        \
    void SetRadius(u8 value) \
    {                        \
        radius = value;      \
    }

#define SDW_MEMBERS_ZoneList                                    \
    void Load(u32 id);                                          \
    Box *FindOverlap(CollBox *query)                            \
    {                                                           \
        return BoxList_FindOverlappingBox(query, boxes, count); \
    }


#define SDW_MEMBERS_Rock      \
    s32 IsSettled()           \
    {                         \
        return settleMs <= 0; \
    }
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_SETROTATION_VEC3S 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_SETROTATION_VEC3S
#define SDW_INLINE_SCNOBJECT_INSTFLAGS_U16 1
#define SDW_INLINE_SCNOBJECT_ISINWORLD 1
#define SDW_INLINE_SCNOBJECT_STOPSOUND_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_INSTFLAGS_U16
#undef SDW_INLINE_SCNOBJECT_ISINWORLD
#undef SDW_INLINE_SCNOBJECT_STOPSOUND_U16
#define SDW_INLINE_SCNOBJECT_ISSOUNDPLAYING_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_ISSOUNDPLAYING_U16
#define SDW_INLINE_SCNOBJECT_ENABLEBOXCOLLIDE_S32 1
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_ENABLEBOXCOLLIDE_S32
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_SHADOW_SETFLAG4_S32 1
#include "../engine/shadow_inlines.h"
#undef SDW_INLINE_SHADOW_SETFLAG4_S32
#define SDW_INLINE_ZONELIST_LOAD_U32 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_LOAD_U32
#define SDW_INLINE_ZONELIST_CLEAR 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_CLEAR
#define SDW_INLINE_CINE_ISACTIVE 1
#include "../engine/cine_inlines.h"
#undef SDW_INLINE_CINE_ISACTIVE
/* 0x57b704 - this TU's copy of the cinematic header's static opcode stride table (payload bytes per key, by opcode;
   the copies src/engine/cine.cpp and cine2.cpp index are g_cineOpStride 0x5816fc, g_cineOpStride2 and
   g_cineOpStride3). A static table in a header that every cinematic .cpp includes: each includer gets its own
   unreferenced copy in .data, ahead of its own data. Local definition standing in for that header, named
   s_cineOpStride as in the other batches (a static must not reuse the global name g_cineOpStride). */
static u8 s_cineOpStride[9] = {0, 8, 8, 4, 2, 2, 4, 2, 2};

s32 Vec3s_DistXZ(Vec3s *a, Vec3s *b);
s32 ObjGrid_QueryBoxOverlap(CollBox *box, ScnObject **out);
u16 Sound_Play(u16 id, void *owner, u16 volume, u8 flags, s32 rate);
void Dialogue_SetBoxActive(s32 active);
#include "../sdk/crt.h"
extern "C" s16 Math_RadiansToAngle4096(float radians);
extern "C" s16 g_sinTable4096[5122];
extern "C" const s16 *g_pCosTable;
extern Wolf *g_pWolf;
extern s32 g_dt, g_dtMs;
extern u32 g_gameTime, g_gameFlags;

#define SDW_INLINE_FREE_SCN_GETPROPU32_U16_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_SCN_GETPROPU32_U16_U32
#define SDW_INLINE_FREE_SCN_GETPROPID_U16_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_SCN_GETPROPID_U16_U32
#define SDW_INLINE_SCNOBJECT_GETMODELBOXES_U32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETMODELBOXES_U32
inline s16 ScnObject::GetTop()
{
    CollBox *box = GetFirstSolidBox();
    if (!box)
        return 0;
    return box->min.y;
}
#define SDW_INLINE_FREE_BOXCONTAINSXZ_BOX_VEC3S 1
#include "../engine/coll_box_inlines.h"
#undef SDW_INLINE_FREE_BOXCONTAINSXZ_BOX_VEC3S
inline void StartCinematic(u32 id)
{
    g_cinePlayer.Start(id, CINE_CAM_CUT_IN | CINE_NO_WOLF_FREEZE, 0, 0, 0, 0);
}
#define SDW_INLINE_FREE_CLEARGAMEFLAGS_U32 1
#include "../engine/game_state_inlines.h"
#undef SDW_INLINE_FREE_CLEARGAMEFLAGS_U32
#define ROCK_ABS(value) ((value) >= 0 ? (value) : -(value))
#define ROCK_SIGN(value) ((value) >= 0 ? 1 : -1)

/* 0x4e25b0 */
void Rock::PlayRollSound()
{
    if (!rollSound || !IsSoundPlaying(rollSound))
        rollSound = Sound_Play(SND_SCOPUSH, this, 0xff, SNDF_LOOP | SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 0x1000);
}

/* 0x4e2611 */
void Rock::StopRollSound()
{
    if (rollSound) {
        StopSound(rollSound);
        rollSound = 0;
    }
}

/* 0x4e2657 */
s32 Rock::CrushObjectsBelow(ScnObject *exclude)
{
    CollBox crushBox;
    ScnObject *candidate;
    ScnObject *found[64];
    s32 index, crushed, count;
    crushed = 0;
    /* cast kept: Box and CollBox are two views of the same 16-byte record */
    crushBox.Box_Translate((CollBox *)collBox, &pos);
    crushBox.min.x += 30;
    crushBox.min.z += 30;
    crushBox.max.x -= 30;
    crushBox.max.z -= 30;
    crushBox.max.y += 50;
    crushBox.min.y = crushBox.max.y - 100;
    count = ObjGrid_QueryBoxOverlap(&crushBox, found);
    for (index = 0; index < count; index++) {
        candidate = found[index];
        /* cast kept: HandleMessage's arg is a void *; this message passes a number in it */
        if (candidate != exclude && this != candidate)
            if (candidate->HandleMessage(this, MSG_KILL, (void *)KILL_CRUSH))
                crushed = 1;
    }
    return crushed;
}

/* 0x4e2783 */
void Rock::ApplyGravity(Vec3s *delta)
{
    s32 speed = fallTime * 4000 >> 12;
    if (speed < 200)
        speed = 200;
    else if (speed > 3000)
        speed = 3000;
    delta->y += (s16)(speed * g_dt / 4096);
}

/* 0x4e27ef */
/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets */
u16 Rock::Move(Vec3s *delta, u16 resolveFlags, s32 pushing)
{
    struct {
        ScnObject *crush;
        s32 horizontal;
        u16 pad0, result;
        ContactInfo wolfContact;
        ScnObject *exclude;
        s32 distance, movedWolf;
        ContactInfo contact;
        u16 pad1, wolfResult;
        Vec3s saved;
        u16 pad2, pad3;
        s16 savedY;
    } w;
    w.movedWolf = 0;
    w.distance = Vec3s_DistXZ(&pos, &g_pWolf->pos);
    w.savedY = delta->y;
    w.horizontal = delta->x | delta->z;
    w.saved.x = delta->x;
    w.saved.y = delta->y;
    w.saved.z = delta->z;
    w.result = Collide_ResolveMove(delta, &w.contact, 0xb54, resolveFlags, 0, 0, 10, 0, 0);
    if (state == ROCK_ST_LAUNCHED) {
        delta->x = w.saved.x;
        delta->z = w.saved.z;
        if (g_pWolf && w.saved.y < 0 && !g_pWolf->HandleMessage(this, MSG_WOLF_IS_GROUNDED, 0) &&
            ((w.contact.movableObj && w.contact.movableObj->GetClassId() == CLASSID_WOLF) ||
             (w.distance < 150 && g_pWolf->pos.y <= pos.y + 50))) {
            w.exclude = this;
            w.saved.x = g_dtMs * 300 * ROCK_SIGN(g_pWolf->pos.x - pos.x) / 1000;
            w.saved.z = g_dtMs * 300 * ROCK_SIGN(g_pWolf->pos.z - pos.z) / 1000;
            w.saved.y -= (s16)(g_dtMs * 800 / 1000);
            if (g_dtMs > 100) {
                if (ROCK_ABS(w.saved.x) > 30)
                    w.saved.x = ROCK_SIGN(w.saved.x) * 30;
                if (ROCK_ABS(w.saved.z) > 30)
                    w.saved.z = ROCK_SIGN(w.saved.z) * 30;
                if (ROCK_ABS(w.saved.y) > 80)
                    w.saved.y = ROCK_SIGN(w.saved.y) * 80;
            }
            w.wolfResult = g_pWolf->Collide_ResolveMove(&w.saved, &w.wolfContact, 0xb54, RESOLVE_SLIDE_ALL, 0, 0, 10,
                                                        &w.exclude, 1);
            g_pWolf->HandleMessage(this, MSG_WOLF_FORCE_FALL, 0);
            g_pWolf->Translate(&w.saved);
            w.movedWolf = 1;
        }
    }
    if (!w.contact.movableObj) {
        if (settleMs > 0)
            settleMs -= (s16)g_dtMs;
    } else
        settleMs = 2000;
    w.crush = w.contact.floorObj;
    if (w.contact.wallObj && pusher != w.contact.wallObj) {
        if ((pushing && w.horizontal) || w.contact.wallObj->pos.y + w.contact.wallObj->GetTop() >= pos.y)
            w.crush = w.contact.wallObj;
    }
    if (w.movedWolf) {
        if (w.crush && w.crush->GetClassId() == CLASSID_WOLF)
            w.crush = 0;
        if (w.saved.y > w.savedY)
            delta->y = w.saved.y;
        else
            delta->y = w.savedY;
    }
    /* cast kept: HandleMessage's arg is a void *; this message passes a number in it */
    if (w.crush && state != ROCK_ST_ON_SEESAW)
        w.crush->HandleMessage(this, MSG_KILL, (void *)KILL_CRUSH);
    Translate(delta);
    if (delta->y > 0 && state != ROCK_ST_ON_SEESAW)
        CrushObjectsBelow(w.crush);
    if (w.result & COLL_FLOOR) {
        if (fallTime > 0x800)
            Sound_Play(SND_SR2ROFA2, this, 0xff, SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 0x1000);
        fallTime = 0;
    }
    return w.result;
}

/* 0x4e2c5c */
void Rock::StartLvl03Cinematic()
{
    g_gameFlags |= GF_CINE_SURVIVES_RESTART;
    if (lvl03BoxDelivered) {
        lvl03BoxDelivered = 0;
        StartCinematic(lvl03CineWithBox);
    } else
        StartCinematic(lvl03CineWithoutBox);
    SetState(ROCK_ST_LVL03_CINE);
    pusher = 0;
}

/* 0x4e2ceb */
/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets; pad0, pad1, pad2 fill gaps */
void Rock::Update()
{
    struct {
        s32 dt;
        LaunchArc *arc;
        u16 pad0;
        s16 unused;
        s32 arcResult;
        Vec3s target;
        u16 pad1;
        Vec3s delta;
        u16 pad2;
        Vec3s velocity;
        u16 pad3, pad4, result;
    } w;
    switch (state) {
        case ROCK_ST_FALL:
            fallTime += g_dt;
            w.delta.x = 0;
            w.delta.y = 0;
            w.delta.z = 0;
            ApplyGravity(&w.delta);
            if (Move(&w.delta, COLL_WALL, 1) & COLL_FLOOR) {
                if (IsSettled()) {
                    SetState(ROCK_ST_REST);
                    pusher = 0;
                }
            } else
                settleMs = 2000;
            break;
        case ROCK_ST_ON_SEESAW:
            if (fallTime < 0x800)
                fallTime = 0x800;
            w.delta.x = 0;
            w.delta.y = 0;
            w.delta.z = 0;
            ApplyGravity(&w.delta);
            Move(&w.delta, 0, 0);
            settleMs = 2000;
            break;
        case ROCK_ST_LAUNCHED:
            w.dt = g_dtMs >> 2;
            w.arc = &launch;
            if (w.arc->flags.full3D)
                w.arcResult = w.arc->Step(w.dt, &w.target.x, &w.target.y, &w.target.z);
            else if (w.arc->flags.planeYZ) {
                w.arcResult = w.arc->Step(w.dt, &w.target.z, &w.target.y, &w.unused);
                w.target.x = w.arc->obj->pos.x;
            } else {
                w.arcResult = w.arc->Step(w.dt, &w.target.x, &w.target.y, &w.unused);
                w.target.z = w.arc->obj->pos.z;
            }
            if (w.arcResult) {
                w.delta.x = w.target.x - pos.x;
                w.delta.y = w.target.y - pos.y;
                w.delta.z = w.target.z - pos.z;
                if (Move(&w.delta, COLL_WALL, 1) && launch.t > 128)
                    StartFall();
            } else
                StartFall();
            break;
        case ROCK_ST_PUSHED:
            if (rockFlags.moving)
                PlayRollSound();
            else
                StopRollSound();
            if (lvl03Enabled && BoxContainsXZ(lvl03Box, &pos))
                StartLvl03Cinematic();
            else {
                if (!rockFlags.pushed) {
                    Shadow_Update(&shadow, &pos, this);
                    if (shadow.GetHeight() - (pos.y + collBox->max[1]) >= 50)
                        SetState(ROCK_ST_ROLL_OFF);
                    else {
                        settleMs = 2000;
                        SetState(ROCK_ST_FALL);
                    }
                }
                rockFlags.pushed = 0;
            }
            break;
        case ROCK_ST_ROLL_OFF:
            PlayRollSound();
            fallTime += g_dt;
            w.velocity.x = g_sinTable4096[pushHeading] * 200 >> 12;
            w.velocity.z = g_pCosTable[pushHeading] * 200 >> 12;
            w.velocity.y = 0;
            Vec3s_ScaleByDt(&w.velocity, &w.delta);
            ApplyGravity(&w.delta);
            w.result = Move(&w.delta, RESOLVE_SLIDE_ALL, 1);
            Matrix_RollByDisplacement(&rollMatrix, &w.delta, (collBox->max[0] - collBox->min[0]) >> 1);
            if (lvl03Enabled && BoxContainsXZ(lvl03Box, &pos))
                StartLvl03Cinematic();
            else if (!(w.result & COLL_FLOOR) || g_gameTime - stateTime >= 0x2000) {
                settleMs = 2000;
                SetState(ROCK_ST_FALL);
            }
            break;
        case ROCK_ST_LVL03_CINE:
            if (g_gameTime - stateTime <= 0x3000)
                PlayRollSound();
            else
                StopRollSound();
            CrushObjectsBelow(0);
            if (!g_cinePlayer.IsActive())
                SetState(ROCK_ST_FALL);
            else if (!BoxContainsXZ(lvl03CameraBox, &g_pWolf->pos) && g_cinePlayer.cameraHeld) {
                g_cinePlayer.cameraHeld = 0;
                Camera_ReleaseAny();
                Dialogue_SetBoxActive(0);
            }
            break;
        case ROCK_ST_SMASHED:
            if (rockFlags.debrisValid)
                debrisBody.AdvanceAnim();
            if (g_gameTime - smashTime >= 0x5000) {
                rockFlags.fallsAtInit = 1;
                Reset();
                if (!rockFlags.reset)
                    ReturnHome();
            }
            break;
    }
    camShot.Update(this);
}

/* 0x4e3366 */
void Rock::StartFall()
{
    SetState(ROCK_ST_FALL);
    fallTime = 0x1000;
    settleMs = 2000;
}

/* 0x4e3391 */
/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets; pad0, pad1 fill gaps */
s32 Rock::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    struct {
        Vec3s *push;
        Vec3s delta;
        u16 pad0;
        CollBox world;
        Vec3s newPos;
        u16 pad1;
        s32 pushY;
    } w;
    if (state == ROCK_ST_SMASHED)
        return 0;
    switch (msgId) {
        case MSG_QUERY_IN_FLIGHT:
            return state == ROCK_ST_LAUNCHED;
        case MSG_QUERY_MOVING:
            if (state != ROCK_ST_LVL03_CINE &&
                (state == ROCK_ST_FALL || state == ROCK_ST_LAUNCHED || state == ROCK_ST_ROLL_OFF))
                return 1;
            return 2;
        case MSG_QUERY_ACTION:
            if (state != ROCK_ST_LVL03_CINE && sender->GetClassId() == CLASSID_WOLF && state != ROCK_ST_ON_SEESAW &&
                (rockFlags.allowZ | rockFlags.allowX)) {
                w.pushY = sender->pos.y - 100;
                if (w.pushY <= collBox->max[1] + pos.y && w.pushY >= collBox->min[1] + pos.y)
                    return CTX_PUSH;
            }
            break;
        case MSG_PUSH:
            if (state != ROCK_ST_LVL03_CINE && state != ROCK_ST_ROLL_OFF) {
                fallTime += g_dt;
                /* cast kept: the message arg is a void *; what it carries depends on the message */
                w.push = (Vec3s *)arg;
                w.delta.x = w.push->x;
                w.delta.y = 0;
                w.delta.z = w.push->z;
                if (sender->GetClassId() == CLASSID_WOLF) {
                    if (!rockFlags.allowX)
                        w.delta.x = 0;
                    if (!rockFlags.allowZ)
                        w.delta.z = 0;
                }
                w.newPos.x = pos.x + w.delta.x;
                w.newPos.y = pos.y + w.delta.y;
                w.newPos.z = pos.z + w.delta.z;
                /* cast kept: Box and CollBox are two views of the same 16-byte record */
                w.world.Box_Translate((CollBox *)collBox, &w.newPos);
                /* +0x160/+0x164 are the generated pointer/count pair of a ZoneList. */
                if (pushBoxes.count > 0 && !pushBoxes.FindOverlap(&w.world)) {
                    w.delta.z = 0;
                    w.delta.x = w.delta.z;
                }
                ApplyGravity(&w.delta);
                pushHeading = Math_RadiansToAngle4096((float)atan2(w.delta.x, w.delta.z)) & 0xfff;
                Move(&w.delta, RESOLVE_SLIDE_ALL, 1);
                w.push->x = w.delta.x;
                w.push->z = w.delta.z;
                Matrix_RollByDisplacement(&rollMatrix, &w.delta, (collBox->max[0] - collBox->min[0]) >> 1);
                if (state != ROCK_ST_PUSHED)
                    SetState(ROCK_ST_PUSHED);
                rockFlags.pushed = 1;
                rockFlags.moving = (w.delta.x | w.delta.z) != 0;
                pusher = sender;
                return 1;
            }
            break;
        case MSG_SEESAW_TOUCH:
            if (state != ROCK_ST_LVL03_CINE) {
                settleMs = 2000;
                if (state != ROCK_ST_ON_SEESAW && state != ROCK_ST_LAUNCHED) {
                    seesawRestPos = pos;
                    /* Preserve the original comparisons of absolute coordinates. */
                    if (ROCK_ABS(seesawRestPos.x) <= ROCK_ABS(launchStartPos.x) + 50 &&
                        ROCK_ABS(seesawRestPos.x) >= ROCK_ABS(launchStartPos.x) - 50 &&
                        ROCK_ABS(seesawRestPos.z) <= ROCK_ABS(launchStartPos.z) + 50 &&
                        ROCK_ABS(seesawRestPos.z) >= ROCK_ABS(launchStartPos.z) - 50) {
                        SetPosition(&launchStartPos);
                        launchStartPos.x = 0;
                        launchStartPos.y = 0;
                        launchStartPos.z = 0;
                    }
                    SetState(ROCK_ST_ON_SEESAW);
                    if (fallTime < 0x800)
                        fallTime = 0x800;
                    pusher = 0;
                }
            }
            break;
        case MSG_LAUNCH:
            if (state != ROCK_ST_LVL03_CINE) {
                if (settleMs > 0)
                    settleMs -= (s16)g_dtMs;
                SetState(ROCK_ST_LAUNCHED);
                /* cast kept: the message arg is a void *; what it carries depends on the message */
                launch = *(LaunchArc *)arg;
                fallTime = 1;
                if (launch.camera && g_camMode != CAM_SCRIPTED) {
                    camShot.Init(launch.flags.noFreeze);
                    camShot.Start(launch.camera, this, launch.camParam);
                    launch.camera = 0;
                }
                launchStartPos = pos;
                return 1;
            }
            break;
        case MSG_GROUND_QUERY:
            /* cast kept: the message arg is a void *; collBox is a Box, the same record as a CollBox */
            if (collBox)
                return Box_GroundQueryDome((GroundQuery *)arg, (CollBox *)collBox, &pos);
            break;
        case MSG_FREEZE:
            camShot.Stop(this);
            return 1;
        case MSG_CINE_END:
            if (state == ROCK_ST_LVL03_CINE)
                ClearGameFlags(GF_CINE_SURVIVES_RESTART);
            StartFall();
            return 1;
        case MSG_MAILBOX_DELIVERED:
            lvl03BoxDelivered = 1;
            break;
        case MSG_QUERY_PUSH_AXIS:
            if (!rockFlags.allowX)
                return PUSH_AXIS_X_LOCKED;
            if (!rockFlags.allowZ)
                return PUSH_AXIS_Z_LOCKED;
            break;
        case MSG_KILL:
            if (sender->GetClassId() == CLASSID_TRAIN && state != ROCK_ST_SMASHED) {
                if (rockFlags.debrisValid) {
                    debrisBody.pos = pos;
                    debrisBody.PlayAnim(AROCHE03_ANIM_EXPLOD2, 0, 0);
                }
                EnableBoxCollide(0);
                SetState(ROCK_ST_SMASHED);
                smashTime = g_gameTime;
            }
            break;
    }
    return 0;
}

/* 0x4e3d20 */
void Rock::Render(Camera *view)
{
    if (state != ROCK_ST_SMASHED) {
        Instance_DrawRigid(Inst(), view, 0, &rollMatrix);
        if (InstFlags(INST_F_DRAWN) && IsInWorld()) {
            Shadow_Update(&shadow, &pos, this);
            Shadow_Render(&shadow);
        }
    } else if (rockFlags.debrisValid)
        debrisBody.Render(view);
}

/* 0x4e3dda */
void Rock::SetState(u8 newState)
{
    StopRollSound();
    if (newState == ROCK_ST_FALL)
        SetUpdateMode(SCN_UPD_ALWAYS);
    else
        SetUpdateMode(SCN_UPD_NORMAL);
    state = newState;
    stateTime = g_gameTime;
}

/* 0x4e3f8a */
void Rock::ReturnHome()
{
    SetPosition(&homePos);
    SetRotation(g_pZeroVec3s);
    Mat34s_FromEulerScaled(&rot, &rollMatrix, 0);
    fallTime = 0;
    rockFlags.pushed = 0;
    rockFlags.moving = 0;
    settleMs = 0;
    pushHeading = 0;
    if (settleMs > 0)
        settleMs -= (s16)g_dtMs;
    if (rockFlags.fallsAtInit)
        SetState(ROCK_ST_FALL);
    else
        SetState(ROCK_ST_REST);
}

/* 0x4e4082 */
void Rock::Reset()
{
    camShot.Init(0);
    EnableBoxCollide(1);
    pusher = 0;
    if (rockFlags.reset)
        ReturnHome();
}

/* 0x4e40f8 */
void Rock::PostLoadInit()
{
    Vec3s target;
    u32 id;
    u16 *props;
    u32 count;
    props = record;
    rollSound = 0;
    lvl03BoxDelivered = 0;
    lvl03CineWithBox = (u16)Scn_GetPropU32(props, 0x24);
    lvl03CineWithoutBox = (u16)Scn_GetPropU32(props, 0x28);
    lvl03Enabled = lvl03CineWithBox != 0 || lvl03CineWithoutBox != 0;
    lvl03Box = Scn_GetPropBox(props, 0x10);
    lvl03CameraBox = Scn_GetPropBox(props, 0x1c);
    stateTime = 0;
    if (Scn_GetPropU32(props, 4))
        rockFlags.allowX = 1;
    else
        rockFlags.allowX = 0;
    if (Scn_GetPropU32(props, 8))
        rockFlags.allowZ = 1;
    else
        rockFlags.allowZ = 0;
    if (Scn_GetPropU32(props, 0x2c))
        rockFlags.reset = 1;
    else
        rockFlags.reset = 0;
    if (Scn_GetPropU32(props, 0xc))
        rockFlags.fallsAtInit = 1;
    else
        rockFlags.fallsAtInit = 0;
    id = Scn_GetPropId(props, 0);
    if (id)
        pushBoxes.Load(id);
    else
        pushBoxes.Clear();
    if (rockFlags.fallsAtInit)
        SetState(ROCK_ST_FALL);
    else {
        target = pos;
        target.y = QueryGroundY(&target, 1);
        SetPosition(&target);
        SetState(ROCK_ST_REST);
    }
    homePos = pos;
    collBox = (Box *)GetModelBoxes(&count); /* cast kept: Box and CollBox are two views of the same 16-byte record */
    if (count > 1 && !(collBox->flags & COLLBOX_ELLIPSOID))
        collBox++;
    pusher = 0;
    fallTime = 0;
    rockFlags.pushed = 0;
    rockFlags.moving = 0;
    settleMs = 0;
    pushHeading = 0;
    Mat34s_FromEulerScaled(&rot, &rollMatrix, 0);
    if (settleMs > 0)
        settleMs -= (s16)g_dtMs;
    weight = 120;
    shadow.SetRadius((u8)collBox->max[0]);
    shadow.SetFlag4(1);
    camShot.Init(0);
    rockFlags.debrisValid = 0;
    smashTime = 0;
    /* cast kept: the record builder takes the synthesised record as raw u16 words */
    if (Scn_BuildRecordFromExport(WAR_IDO_AROCHE03, (u16 *)&debrisRecord, 0, 0)) {
        debrisBody.Init(&debrisRecord, 0);
        rockFlags.debrisValid = 1;
    }
    SetUpdateMode(SCN_UPD_CINE);
    seesawRestPos.x = 0;
    seesawRestPos.y = 0;
    seesawRestPos.z = 0;
    launchStartPos.x = 0;
    launchStartPos.y = 0;
    launchStartPos.z = 0;
}

/* 0x4e4688 */
ScnObject *Rock_Create(void *record)
{
    Rock *obj = new Rock;
    obj->collBox = 0;
    obj = (Rock *)obj->Init(record); /* cast kept: Init returns the object as a ScnObject * */
    return obj;
}

/* ---- LaunchArc::Step (SDW_MEMBERS_LaunchArc is above) ---- */
/* 0x4e4720 */
s32 LaunchArc::Step(s32 dt, s16 *outA, s16 *outB, s16 *outC)
{
    s32 squared;
    if (t < 0x100) {
        t = dt + t;
        if (t > 0x100)
            t = 0x100;
        squared = t * t;
        *outA = coeffs[0] + ((t * coeffs[1]) >> 8) + ((squared * coeffs[2]) >> 15);
        *outB = coeffs[3] + ((t * coeffs[4]) >> 8) + ((squared * coeffs[5]) >> 15);
        *outC = coeffs[6] + ((t * coeffs[7]) >> 8) + ((squared * coeffs[8]) >> 15);
        return 1;
    }
    return 0;
}
