/*
 * GeyserOut (class 73 "GeyserOut", vtable 0x576258, sizeof 0xd4) - the geyser that spits back out whatever a vortex
 * (GeyserIn) swallowed, in Level 7 (disc Lvl-08), where it throws Ralph back into the level.
 * SheepD3D.exe 0x4c5c90-0x4c7068, the whole file (it ends with the factory, followed by int3 padding).
 *
 * What it does: the vortex hands the object over with msg 0x2886 and starts the eruption with msg 0x2880. In state 1
 * everything inside SPITBOX is pushed along a trajectory - the Wolf along TRAJECTORYWOLF, everything else along
 * TRAJECTORY - at a speed that falls off with the distance from the mouth (4000 for the Wolf, 2500 for the rest,
 * capped at 2000), the Wolf's with a small sine wobble on top. The jet sound 0x12f loops while the camera is close.
 * Unless ALWAYSSPIT is set the eruption stops after spitDelay ms (msg 0x2883).
 *
 * Devices that only pin the original code generation (the inline helpers have no bodies in the exe, so their names are
 * not recovered): Scn_GetPropU32 with a u32 offset gives the offset its own stack temp (0x4c5dc2), as in
 * frozenriver.cpp; Flag / FlagClear / SetFlag / ClearFlag are MACROS, not inline methods: an inline with a mask
 * parameter gives the mask a stack temp of its own, which the original has not;
 * IsHeld is a free inline (its ScnObject* argument gets the temp at ebp-0x84 and its 0/1 result the one at ebp-0x88,
 * while the holder position it writes is the caller's last named local at ebp-0x7c);
 * StopSound / IsSoundPlaying give the u16 handle temps; GetClassId, AnimFlags, InstFlags, PlayAnim and SetUpdateMode
 * are the shared ones (SetUpdateMode takes a u8, so the argument 0 is materialised by xor as in src/objects/bridge.cpp).
 * The local names were chosen for their stack slots (tools/vc6_locals.py): speed / head / where / vec / info / shake /
 * touched / i / nfound / angl / delta / holderPos in Update, rec / slant / trajectory in PostLoadInit. The `continue`s
 * of Update's object loop are written as nested ifs, because the original jumps to one shared end-of-body.
 *
 * A defect worth recording: at the end of state 1, when the box has emptied and the camera flag is set, the branch
 * taken when `passenger` is NULL still reads passenger->classId (0x4c66b0-0x4c66c0). Every path that sets the camera
 * flag from a Salad or a generic object leaves passenger untouched, so a Salad riding the jet with no msg 0x2886
 * behind it dereferences a null pointer when it leaves the box.
 */
/* BYTES: flow, slot-name. */

#define SDW_MEMBERS_ScnObject            \
    static void *operator new(u32 size); \
    /* inline, defined below */          \
    void SetUpdateMode(u8 mode); /* inline, defined below; a u8, so 0 is materialised by xor (0x4c69d9) */

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
#define SDW_INLINE_SCNOBJECT_INSTFLAGS_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_INSTFLAGS_U16
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#include "../engine/grid_queries.h"
#include "../engine/property_math.h"

#define SDW_ABS(v) ((v) >= 0 ? (v) : -(v))

/* Every read of the flag word is masked and the result tested; every write truncates it to a byte. Macros, not inline
 * methods: an inline with a mask parameter would give the mask a stack temp of its own, which the original has not. */
#define Flag(m) (flags & (m))
#define FlagClear(m) (~flags & (m))
#define SetFlag(m) (flags = (u8)(flags | (m)))
#define ClearFlag(m) (flags = (u8)(flags & ~(m)))

extern Wolf *g_pWolf; /* 0x6cf310 */

#include "../app/app_main.h"
#include "../engine/scn_tools.h"
#include "../engine/sound_mgr.h"
#include "camera.h"
#define g_camPos (g_camera.pos)    /* 0x584d20 */
extern s32 g_dtMs;                 /* 0x71b2e8  g_dt * 1000 >> 12 */
extern "C" const s16 *g_pCosTable; /* 0x5814e4  g_sinTable4096 + 1024 */

u16 Sound_Play(u16 soundId, void *owner, u16 volume, u8 flags, s32 rate); /* 0x5491b8 */

/* the shared eruption phase: one angle for every geyser in the level, advanced once per carried Wolf */
s32 g_geyserWobblePhase; /* 0x6cf660 */

/* ---- inline helpers ---- */

/* A u32 designer property: the record's property block starts at +0x14. */
#define SDW_INLINE_FREE_SCN_GETPROPU32_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_SCN_GETPROPU32_VOID_U32

#define SDW_INLINE_SCNOBJECT_STOPSOUND_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_STOPSOUND_U16

#define SDW_INLINE_SCNOBJECT_ISSOUNDPLAYING_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_ISSOUNDPLAYING_U16

/* ScnUpdateMode: 0 near the camera only, 1 always, 2 never, 3 also during cinematics. */
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8

/* INST_ATTACHED (8): the object is riding on another one; then it is not the geyser's to push. The holder's position
 * is written out but the caller does not use it. */
#define SDW_INLINE_FREE_ISHELD_SCNOBJECT_VEC3S 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_FREE_ISHELD_SCNOBJECT_VEC3S

/* ---- GeyserOut ---- */

/* 0x4c5c90 - vtable +0x00: both followers, the mouth point and the speed falloff, the spit box and the camera, the
 * three flag properties, and the tilt test (a mouth whose vertical rotation is small but whose other two axes add up
 * to more than 25 is marked 0x20). */
void GeyserOut::PostLoadInit()
{
    u16 *rec;
    Vec3s slant;
    Trajectory *trajectory;

    rec = record;
    flags = 0;
    soundHandle = 0;
    trajectory = Scn_GetPropTrajectory(rec, 0x14); /* PROPERTY_GEYSEROUT_TRAJECTORY */
    TrajFollower_Init(&objPath, trajectory, 0, 0x800, 1, 1, 0x32);
    trajectory = Scn_GetPropTrajectory(rec, 0x18); /* PROPERTY_GEYSEROUT_TRAJECTORYWOLF */
    TrajFollower_Init(&wolfPath, trajectory, 0, 0x800, 1, 1, 0x32);
    startPoint.x = trajectory->pts[0].x;
    startPoint.y = trajectory->pts[0].y;
    startPoint.z = trajectory->pts[0].z;
    wolfSpeedPerUnit = 4000 / Vec3s_Dist(&trajectory->pts[0], &trajectory->pts[1]);
    objSpeedPerUnit = 4000 / Vec3s_Dist(&trajectory->pts[0], &trajectory->pts[1]);
    /* cast kept: Box and CollBox are two views of one 16-byte zone record */
    spitBox = (CollBox *)Scn_GetPropBox(rec, 0x10); /* PROPERTY_GEYSEROUT_SPITBOX */
    camera = Scn_GetPropCamera(rec, 4);             /* PROPERTY_GEYSEROUT_CAMERA */
    if (Scn_GetPropU32(rec, 8))                     /* PROPERTY_GEYSEROUT_CAMERAFORSALAD */
        SetFlag(GEYSEROUT_F_CAM_OBJ);
    if (Scn_GetPropU32(rec, 0xc)) /* PROPERTY_GEYSEROUT_CAMINTERPOLATE */
        SetFlag(GEYSEROUT_F_CAM_WOLF);
    passenger = 0;
    if (Scn_GetPropU32(rec, 0)) { /* PROPERTY_GEYSEROUT_ALWAYSSPIT */
        SetFlag(GEYSEROUT_F_BUSY);
        SetState(GEYSEROUT_ST_CHARGE);
    } else {
        SetState(GEYSEROUT_ST_IDLE);
    }
    slant.x = rot.x;
    slant.y = rot.y;
    slant.z = rot.z;
    if (slant.y < 0x19) {
        if (SDW_ABS(slant.x) + SDW_ABS(slant.z) > 0x19)
            SetFlag(GEYSEROUT_F_SLANTED);
    }
}

/* 0x4c5f01 - vtable +0x14: a restart forgets the jet sound and the passenger. */
void GeyserOut::Reset()
{
    soundHandle = 0;
    passenger = 0;
}

/* 0x4c5f25 - vtable +0x04: the jet sound by camera distance, then the state machine. */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py) */
/* BYTES(flow): nested ifs instead of continue: the original jumps to one shared end of the loop body */
void GeyserOut::Update()
{
    s32 speed;
    s16 head;
    Vec3s vec;
    Vec3s where;
    s32 nfound;
    u16 i;
    ScnObject *touched[10];
    s32 shake;
    ContactInfo info;
    Vec3s holderPos;
    Vec3s delta;
    s32 angl;

    if (IsSoundPlaying(soundHandle)) {
        if (Vec3s_DistSq(&pos, &g_camPos) > 0x736504)
            StopSound(soundHandle);
    } else {
        if (Vec3s_DistSq(&pos, &g_camPos) < 0x5f5e10 && animState != GEYSEROUT_AS_IDLE)
            soundHandle = Sound_Play(SND_GEYSER, this, 0xff,
                                     SNDF_LOOP | SNDF_POSITIONAL | SNDF_DIST_HORIZONTAL | SNDF_NO_RETRIGGER, 0x1000);
    }
    switch (state) {
        case GEYSEROUT_ST_IDLE:
            ClearFlag(GEYSEROUT_F_WOLF_FROZEN);
            break;
        case GEYSEROUT_ST_RESET:
            ClearFlag(GEYSEROUT_F_WOLF_FROZEN);
            break;
        case GEYSEROUT_ST_CHARGE:
            timer -= g_dtMs;
            if (g_pWolf->HandleMessage(this, MSG_WOLF_IS_DEAD, 0))
                break;
            if (FlagClear(GEYSEROUT_F_BUSY) && timer <= 0)
                SetState(GEYSEROUT_ST_SPIT);
            nfound = ObjGrid_QueryBoxOverlap(spitBox, touched);
            for (i = 0; i < nfound; i++) {
                if (touched[i] != this) {
                    if (!IsHeld(touched[i], &holderPos)) {
                        if (touched[i] != passenger)
                            touched[i]->HandleMessage(this, MSG_GEYSER_OUT, 0);
                        where = touched[i]->pos;
                        switch (touched[i]->GetClassId()) {
                            case CLASSID_WOLF:
                                speed = 4000 - wolfSpeedPerUnit * Vec3s_Dist(&where, &startPoint);
                                if (speed > 2000)
                                    speed = 2000;
                                g_geyserWobblePhase += 0x14d;
                                angl = g_geyserWobblePhase & 0xfff;
                                shake = (g_pCosTable[angl] * 500 + 250) >> 12;
                                speed += shake;
                                wolfPath.speed = (s16)speed;
                                TrajFollower_Step(&wolfPath, &vec, &head);
                                Vec3s_ScaleByDt(&vec, &delta);
                                Collide_ResolveMove(&delta, &info, 0x578, RESOLVE_SLIDE_ALL, 0, 0, 0xa, 0, 0);
                                touched[i]->Translate(&delta);
                                if (Flag(GEYSEROUT_F_FROZE_WOLF) && g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0))
                                    ClearFlag(GEYSEROUT_F_FROZE_WOLF);
                                g_pWolf->HandleMessage(this, MSG_WOLF_FORCE_FALL, 0);
                                if (Flag(GEYSEROUT_F_WOLF_FROZEN)) {
                                    Camera_ReleaseAny();
                                    ClearFlag(GEYSEROUT_F_WOLF_FROZEN);
                                }
                                break;
                            case CLASSID_SALAD:
                                if (Flag(GEYSEROUT_F_CAM_OBJ))
                                    SetFlag(GEYSEROUT_F_WOLF_FROZEN);
                                /* cast kept: the message's void * arg carries a number */
                                touched[i]->HandleMessage(this, MSG_GEYSER_OUT, (void *)1);
                                break;
                            case CLASSID_SAM:
                                break;
                            default:
                                if (Flag(GEYSEROUT_F_CAM_OBJ))
                                    SetFlag(GEYSEROUT_F_WOLF_FROZEN);
                                speed = 2500 - objSpeedPerUnit * Vec3s_Dist(&where, &startPoint);
                                if (speed > 2000)
                                    speed = 2000;
                                objPath.speed = (s16)speed;
                                TrajFollower_Step(&objPath, &vec, &head);
                                Vec3s_ScaleByDt(&vec, &delta);
                                Collide_ResolveMove(&delta, &info, 0x578, RESOLVE_SLIDE_ALL, 0, 0, 0xa, 0, 0);
                                touched[i]->Translate(&delta);
                                break;
                        }
                    }
                }
            }
            if (Flag(GEYSEROUT_F_FROZE_WOLF) && nfound <= 1) {
                g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0);
                ClearFlag(GEYSEROUT_F_FROZE_WOLF);
            }
            if (Flag(GEYSEROUT_F_WOLF_FROZEN) && nfound <= 1) {
                if (Flag(GEYSEROUT_F_FROZE_WOLF)) {
                    g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0);
                    ClearFlag(GEYSEROUT_F_FROZE_WOLF);
                }
                if (passenger != 0) {
                    if ((passenger->GetClassId() == CLASSID_SHEEP || passenger->GetClassId() == CLASSID_SALAD) &&
                        Flag(GEYSEROUT_F_CAM_OBJ))
                        Camera_ReleaseAny();
                    else if (passenger->GetClassId() == CLASSID_WOLF || passenger->GetClassId() == CLASSID_BOX)
                        Camera_SetMode(CAM_FOLLOW, 0);
                } else {
                    if (passenger->GetClassId() == CLASSID_WOLF || passenger->GetClassId() == CLASSID_BOX)
                        Camera_SetMode(CAM_FOLLOW, 0);
                }
                ClearFlag(GEYSEROUT_F_WOLF_FROZEN);
            }
            if (AnimFlags(ANIM_F_FINISHED))
                SetAnimState(5);
            break;
        case GEYSEROUT_ST_SPIT:
            if (AnimFlags(ANIM_F_FINISHED)) {
                SetAnimState(6);
                SetState(GEYSEROUT_ST_IDLE);
            }
            break;
    }
    AdvanceAnim();
}

/* 0x4c6792 - the state setter. */
void GeyserOut::SetState(u8 newState)
{
    switch (newState) {
        case GEYSEROUT_ST_IDLE:
            SetAnimState(3);
            break;
        case GEYSEROUT_ST_CHARGE:
            timer = spitDelay;
            SetAnimState(4);
            /* cast kept: the message's void * arg carries a number */
            if (passenger != 0)
                passenger->HandleMessage(this, MSG_GEYSER_OUT, (void *)1);
            break;
        case GEYSEROUT_ST_SPIT:
            passenger = 0;
            SetAnimState(6);
            break;
        case GEYSEROUT_ST_RESET:
            SetAnimState(6);
            break;
    }
    state = newState;
}

/* 0x4c6846 - the animation setter: any change silences the jet sound first. */
void GeyserOut::SetAnimState(u8 newAnim)
{
    if (animState != newAnim)
        StopSound(soundHandle);
    switch (newAnim) {
        case GEYSEROUT_AS_IDLE:
            PlayAnim(ATORNA01_ANIM_STAND0, 0, 0);
            break;
        case GEYSEROUT_AS_CHARGE:
            PlayAnim(ATORNA01_ANIM_TORNA3, 0, 0);
            break;
        case GEYSEROUT_AS_TOSTAND:
            if (newAnim != animState)
                PlayAnim(ATORNA01_ANIM_TOSTAND2, 1, 0);
            break;
        case GEYSEROUT_AS_SPIT:
            if (newAnim != animState) {
                PlayAnim(ATORNA01_ANIM_TORNA4, 0, 0);
                SetUpdateMode(SCN_UPD_NORMAL);
            }
            break;
    }
    animState = newAnim;
}

/* 0x4c6ab0 - vtable +0x10. 0x2880 erupt, 0x2881 hold off, 0x2882 release, 0x2883 set the eruption length,
 * 0x2884..0x2888 the property queries, 0xe the UI freeze acknowledgement, 0x5d the update-mode switch. */
s32 GeyserOut::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    switch (msgId) {
        case MSG_GEYSEROUT_WANTS_WOLF_CAM:
            return Flag(GEYSEROUT_F_CAM_WOLF);
        case MSG_GEYSEROUT_WANTS_OBJ_CAM:
            return Flag(GEYSEROUT_F_CAM_OBJ);
        case MSG_GEYSEROUT_SET_DELAY:
            spitDelay = (s32)arg; /* cast kept: the message's void * arg carries a number */
            break;
        case MSG_GEYSEROUT_GET_CAMERA:
            return (s32)camera; /* cast kept: the s32 reply carries the camera's address */
        case MSG_GEYSEROUT_SET_PASSENGER:
            passenger = (ScnObject *)arg; /* cast kept: this message's void * arg is the passenger */
            if (Flag(GEYSEROUT_F_CAM_OBJ)) {
                /* cast kept: the message's void * arg carries a number */
                if (g_pWolf->HandleMessage(this, MSG_FREEZE, (void *)1))
                    SetFlag(GEYSEROUT_F_FROZE_WOLF);
                SetFlag(GEYSEROUT_F_WOLF_FROZEN);
            }
            if (passenger->GetClassId() == CLASSID_BOX && FlagClear(GEYSEROUT_F_FROZE_WOLF)) {
                /* cast kept: the message's void * arg carries a number */
                if (g_pWolf->HandleMessage(this, MSG_FREEZE, (void *)1))
                    SetFlag(GEYSEROUT_F_FROZE_WOLF);
            }
            if (passenger->GetClassId() == CLASSID_WOLF) {
                SetFlag(GEYSEROUT_F_WOLF_FROZEN);
                if (FlagClear(GEYSEROUT_F_FROZE_WOLF)) {
                    /* cast kept: the message's void * arg carries a number */
                    if (passenger->HandleMessage(this, MSG_FREEZE, (void *)1))
                        SetFlag(GEYSEROUT_F_FROZE_WOLF);
                }
            }
            break;
        case MSG_GEYSEROUT_IS_BUSY:
            return Flag(GEYSEROUT_F_BUSY);
        case MSG_GEYSEROUT_START:
            SetUpdateMode(SCN_UPD_ALWAYS);
            SetState(GEYSEROUT_ST_CHARGE);
            break;
        case MSG_GEYSEROUT_CORK:
            SetFlag(GEYSEROUT_F_CORKED);
            SetState(GEYSEROUT_ST_RESET);
            break;
        case MSG_FREEZE:
            ClearFlag(GEYSEROUT_F_FROZE_WOLF);
            ClearFlag(GEYSEROUT_F_WOLF_FROZEN);
            Camera_SetMode(CAM_FOLLOW, 0);
            return 1;
        case MSG_GEYSEROUT_UNCORK:
            ClearFlag(GEYSEROUT_F_CORKED);
            if (Flag(GEYSEROUT_F_BUSY))
                SetState(GEYSEROUT_ST_CHARGE);
            else
                SetState(GEYSEROUT_ST_IDLE);
            break;
        case MSG_GEYSER_ZONE_ACTIVE:
            if (arg)
                SetUpdateMode(SCN_UPD_ALWAYS);
            else
                SetUpdateMode(SCN_UPD_NORMAL);
            break;
    }
    return 0;
}

/* 0x4c7002 - the class factory for CLASSID 73 "GeyserOut". */
ScnObject *GeyserOut_Create(void *record)
{
    ScnObject *obj = new GeyserOut;
    obj = ((ScnBody *)obj)->Init((u16 *)record, 0); /* cast kept: a downcast, and the level record as raw words */
    return obj;
}
