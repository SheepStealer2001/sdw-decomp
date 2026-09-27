/* T071 - original object DaffyLevel09.cpp (guessed name): .text 0x43a0a0-0x43b2d3, .rdata 0x574d80-0x574da4 (vtable),
 * .data 0x57a6e8-0x57a6f4 (the cinematic header's stride-table copy). */
/* BYTES: layout, slot-group, slot-scope. */
/* BYTES(layout): the Cine.h header static: every object including the cinematic header carries this copy in its .data, referenced or not */
/*
 * DaffyLevel09 (class 104 "DaffyLevel09", vtable 0x574d80, sizeof 0x19c) - Daffy in Level 9 (disc Lvl-11), standing by
 * Elmer's swirling DUCK SEASON / RABBIT SEASON sign. He turns to face Ralph, plays his line when Ralph walks into the
 * ACTIVATIONBOX, and when Ralph walks into one of the BOXDEGUIS boxes wearing model set 4 (the duck disguise) he walks
 * out along the TRAJ path, flips the sign to rabbit season (SwirlSign msg 0x3d) and walks back along TRAJRET.
 * Message 0 (MSG_KILL) drops him into the placard state: a second body, built from an exported resource, is placed
 * 100 units in front of his face and 0x28 above it and shown for two seconds.
 * SheepD3D.exe 0x43a0a0-0x43b2d2: PostLoadInit, Reset, Update, HandleMessage, Render, SetState and the factory
 * (the file starts 16-byte aligned at 0x43a0a0 and ends with int3 padding before DaffyMilitary at 0x43b2e0).
 *
 * States (+0xcc): 7 idle/facing Ralph, 1 the CINE line, 2 the CINEDEGUIS line, 3 walk out (TRAJ), 4 and 5 the two
 * halves of the sign-flipping animation (5 sends the SwirlSign msg 0x3d), 6 walk back (TRAJRET), 0 placard.
 *
 * Devices that only pin the original code generation (the inline helpers have no bodies in the exe, so their names are
 * not recovered): Scn_GetPropU32 with a u32 offset and SDW_PROP, Facing / SetFacing / GetUpdateMode-style
 * SetUpdateMode / EnableBoxCollide / SetTintOverride / Flags16_Set / Flags16_Clear as in src/objects/daffymilitary.cpp,
 * PlayAnim / AnimFlags as in src/game/wolf.h, StartCine (the five Cine_Start argument temps 0x43acfa-0x43ad10),
 * StartCamera (three argument temps 0x43a9d4-0x43a9fb), Box_ContainsPoint (0x43a581), ZoneList::Resolve /
 * FindContainingXZ (0x43a147, 0x43a63f) and Cine_IsActive / Cine_IsFinished.
 * Update's twelve locals are grouped into one `w` struct so that their stack slots are pinned by the struct layout
 * instead of by VC6's name hash (src/README.md; the same device as src/engine/collide.cpp). That is a representation
 * that reproduces the bytes, not evidence that the original source read that way.
 */

#define SDW_MEMBERS_ScnObject                                                    \
    static void *operator new(u32 size);                                         \
    void SetFacing(s16 f); /* inline: its argument is a stack temp (0x43a88a) */ \
    void SetUpdateMode(u8 mode);                                                 \
    void StartCamera(u16 rotX, u16 rotY, Vec3s *eye); /* inline, defined below */


#include "sdw_types.h"
#include "sdw_enums.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *inst, Animator *animator, u16 id, u32 opts); /* 0x550196 */
#include "../engine/id_list.h"
#include "../app/app_main.h"
#include "../engine/cine.h"
#include "../engine/scenaric.h"
#include "../engine/scn_tools.h"
#include "camera.h"
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_FACING 1
#define SDW_INLINE_SCNOBJECT_SETFACING_S16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_FACING
#undef SDW_INLINE_SCNOBJECT_SETFACING_S16
#define SDW_INLINE_SCNOBJECT_ENABLEBOXCOLLIDE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_ENABLEBOXCOLLIDE_S32
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#define SDW_INLINE_ZONELIST_RESOLVE_U32 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_RESOLVE_U32
/* 0x57a6e8 - the cinematic header's static copy of the 9-byte opcode stride table (src/engine/cine.cpp,
 * g_cineOpStride 0x5816fc). The original header defined it static, so every object including it carries its own
 * unreferenced copy at the head of its .data; defined here in its place. */
static u8 s_cineOpStride[9] = {0, 8, 8, 4, 2, 2, 4, 2, 2};
#include "../engine/property_math.h"
#include "scenaric_props.h"

/* cast kept: offsetof written out (no CRT header); as a u32 argument it is not a constant, so the inline gets a temp */
#define SDW_PROP(T, f) ((u32) & ((T *)0)->f)

#include "../sdk/crt.h"

extern Wolf *g_pWolf;                /* 0x6cf310 */
extern s32 g_dtMs;                   /* 0x71b2e8 */
extern "C" s16 g_sinTable4096[5122]; /* 0x57ece0 */
extern "C" const s16 *g_pCosTable;   /* 0x5814e4 */

s32 Scenaric_FindByClass(u16 classId, ScnObject **out, s32 max);          /* 0x5145c5 */
extern "C" s16 Math_RadiansToAngle4096(float radians);                    /* 0x5269ce */
u16 Sound_Play(u16 soundId, void *owner, u16 volume, u8 flags, s32 rate); /* 0x5491b8 */
void Camera_StartScripted(ScnObject *owner, Camera *cam, u16 rotX, u16 rotY, u16 rotZ, Vec3s *pos, u16 focal, u32 mode,
                          s32 time); /* 0x55a70d */

/* ---- inline helpers ---- */

/* A designer property of the WAR record: the dword at record + 0x14 + offset (the offset is a stack temp, 0x43a0c8). */
#define SDW_INLINE_FREE_SCN_GETPROPU32_U16_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_SCN_GETPROPU32_U16_U32

/* Set / clear bits of a 16-bit flag word: the word's address is a stack temp and the mask is loaded into a register
 * (0x43abb7-0x43abf8). */
#define SDW_INLINE_FREE_FLAGS16_SET_U16_U16 1
#include "instance_inlines.h"
#undef SDW_INLINE_FREE_FLAGS16_SET_U16_U16

#define SDW_INLINE_FREE_FLAGS16_CLEAR_U16_U16 1
#include "instance_inlines.h"
#undef SDW_INLINE_FREE_FLAGS16_CLEAR_U16_U16

#define SDW_INLINE_SCNOBJECT_SETTINTOVERRIDE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETTINTOVERRIDE_S32

/* inline: the update mode, a switch on its argument (the jump tables at 0x43a3d0 and 0x43aaff; ScnUpdateMode). */
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8

/* inline: the fixed scripted shot of state 3 - only the two angles and the eye point vary. */
inline void ScnObject::StartCamera(u16 rotX, u16 rotY, Vec3s *eye)
{
    Camera_StartScripted(this, &g_camera, rotX, rotY, 0, eye, 700, 0, 0x1000);
}

#define SDW_INLINE_FREE_STARTCINE_U32_U32_BOX_BOX_VOID 1
#include "../engine/cine_inlines.h"
#undef SDW_INLINE_FREE_STARTCINE_U32_U32_BOX_BOX_VOID

/* The cinematic player's two flags, each through a local of the inline (0x43a747, 0x43a792). */
inline s32 Cine_IsActive()
{
    s32 active = g_cinePlayer.active;
    return active;
}

#define SDW_INLINE_FREE_CINE_ISFINISHED 1
#include "../engine/cine_inlines.h"
#undef SDW_INLINE_FREE_CINE_ISFINISHED

#define SDW_INLINE_ZONELIST_FINDCONTAININGXZ_VEC3S 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_FINDCONTAININGXZ_VEC3S

/* Whether p lies inside box (both faces inclusive): its two parameters and its value are stack temps (0x43a581). */
#define SDW_INLINE_FREE_BOX_CONTAINSPOINT_BOX_VEC3S 1
#include "../engine/coll_box_inlines.h"
#undef SDW_INLINE_FREE_BOX_CONTAINSPOINT_BOX_VEC3S

/* ---- DaffyLevel09 ---- */

/* 0x43a0a0 - vtable +0x00: the thirteen designer properties, the SwirlSign and the Elmer of the level, the placard
 * body built from exported resource 2, then always-update and state 7. */
void DaffyLevel09::PostLoadInit()
{
    u16 *props;

    props = record;
    Scenaric_FindByClass(CLASSID_ELMER, &elmer, 1);
    cine = Scn_GetPropU32(props, SDW_PROP(DaffyLevel09Props, CINE));
    cineDeguis = Scn_GetPropU32(props, SDW_PROP(DaffyLevel09Props, CINEDEGUIS));
    cineBox = Scn_GetPropBox(props, 0x10) /* PROPERTY_DAFFYLEVEL09_CINEBOX */;
    activationBox = Scn_GetPropBox(props, 0) /* PROPERTY_DAFFYLEVEL09_ACTIVATIONBOX */;
    deguisBoxes.Resolve(Scn_GetPropU32(props, SDW_PROP(DaffyLevel09Props, BOXDEGUIS)));
    cineSheepBox = Scn_GetPropBox(props, 0x20) /* PROPERTY_DAFFYLEVEL09_CINESHEEPBOX */;
    camera = Scn_GetPropCamera(props, 8) /* PROPERTY_DAFFYLEVEL09_CAMERATRAJ */;
    cineText = (u8)Scn_GetPropU32(props, SDW_PROP(DaffyLevel09Props, CINETEXT));
    cineTextDeguis = (u8)Scn_GetPropU32(props, SDW_PROP(DaffyLevel09Props, CINETEXTDEGUIS));
    Scenaric_FindByClass(CLASSID_SWIRLSIGN, &swirlSign, 1);
    traj = Scn_GetPropTrajectory(props, 0x2c) /* PROPERTY_DAFFYLEVEL09_TRAJ */;
    trajRet = Scn_GetPropTrajectory(props, 0x30) /* PROPERTY_DAFFYLEVEL09_TRAJRET */;
    cineFlag = Scn_GetPropU32(props, SDW_PROP(DaffyLevel09Props, CINEFLAG));
    cineDeguisFlag = Scn_GetPropU32(props, SDW_PROP(DaffyLevel09Props, CINEDEGUISFLAG));
    deguisCinePlayed = 0;
    cinePlayed = 0;
    SnapToGround(1);
    homePos = pos;
    if (Scn_BuildRecordFromExport(WAR_IDO_AETOIL01, placardRecord, 0, 0)) {
        placard.Init(placardRecord, 0);
        placardValid = 1;
    } else
        placardValid = 0;
    soundHandle = 0;
    wolfFrozen = 0;
    unke0 = 0;
    SetUpdateMode(SCN_UPD_ALWAYS);
    SetState(DAFFY09_ST_IDLE);
}

/* 0x43a3e0 - vtable +0x14 (level restart): back to the placed spot and to state 7 unless the placard is up. */
void DaffyLevel09::Reset()
{
    wolfFrozen = 0;
    if (state) {
        SetPosition(&homePos);
        SetState(DAFFY09_ST_IDLE);
    }
    soundHandle = 0;
}

/* 0x43a430 - vtable +0x04. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; pad06, pad0e, pad26, pad2e, pad3c fill gaps */
/* BYTES(slot-scope): the nested block(s) only order the frame: their locals are allocated after the enclosing scope's */
void DaffyLevel09::Update()
{
    /* one struct, so the twelve locals keep the original's slots (see the file header) */
    struct {
        Vec3s velBack; /* [ebp-0x40] */
        u8 pad06[2];
        Vec3s deltaBack; /* [ebp-0x38] */
        u8 pad0e[4];
        s16 headingBack; /* [ebp-0x2e] */
        s32 dxSq;        /* [ebp-0x2c] */
        s32 dvertSq;     /* [ebp-0x28] */
        s32 dzSq;        /* [ebp-0x24] */
        Vec3s vel;       /* [ebp-0x20] */
        u8 pad26[2];
        Vec3s delta; /* [ebp-0x18] */
        u8 pad2e[2];
        s32 dx;    /* [ebp-0x10] */
        s32 dvert; /* [ebp-0x0c] */
        s32 dz;    /* [ebp-0x08] */
        u8 pad3c[2];
        s16 heading; /* [ebp-0x02] */
    } w;

    switch (state) {
        case DAFFY09_ST_PLACARD:
            if (placardTimeMs > 0) {
                placardTimeMs -= g_dtMs;
                if (placardTimeMs <= 0) {
                    if (wolfFrozen)
                        wolfFrozen = (g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0) == 0);
                    Camera_ReleaseScripted(this);
                }
            }
            placard.AdvanceAnim();
            SetUpdateMode(SCN_UPD_NORMAL);
            break;
        case DAFFY09_ST_IDLE:
            if (Box_ContainsPoint(activationBox, &g_pWolf->pos) && cinePlayed == 0) {
                SetState(DAFFY09_ST_CINE_INTRO);
                break;
            }
            if (deguisBoxes.FindContainingXZ(&g_pWolf->pos) &&
                g_pWolf->HandleMessage(this, MSG_WOLF_IS_RABBITCOSTUME, 0)) {
                if (swirlSign->HandleMessage(this, MSG_SWIRLSIGN_IS_A, 0) &&
                    !g_pWolf->HandleMessage(this, MSG_WOLF_IS_DEAD, 0)) {
                    if (deguisCinePlayed == 0)
                        SetState(DAFFY09_ST_CINE_DISGUISE);
                    else
                        SetState(DAFFY09_ST_WALK_OUT);
                } else if (deguisCinePlayed == 0) {
                    cinePlayed = 1;
                    deguisCinePlayed = 1;
                }
            }
            SetFacing(HeadingTo(&g_pWolf->pos));
            break;
        case DAFFY09_ST_CINE_INTRO:
            if (Cine_IsFinished()) {
                SetState(DAFFY09_ST_IDLE);
                cinePlayed = 1;
            }
            break;
        case DAFFY09_ST_CINE_DISGUISE:
            if (g_pWolf->HandleMessage(this, MSG_WOLF_IS_DEAD, 0)) {
                if (Cine_IsActive()) {
                    g_cinePlayer.CaptureCamera();
                    g_cinePlayer.Stop();
                }
                SetState(DAFFY09_ST_IDLE);
                break;
            }
            if (Cine_IsFinished()) {
                cinePlayed = 1;
                deguisCinePlayed = 1;
                if (swirlSign->HandleMessage(this, MSG_SWIRLSIGN_IS_A, 0))
                    SetState(DAFFY09_ST_WALK_OUT);
                else
                    SetState(DAFFY09_ST_IDLE);
            }
            break;
        case DAFFY09_ST_WALK_OUT:
            if (g_pWolf->HandleMessage(this, MSG_WOLF_IS_DEAD, 0)) {
                SetState(DAFFY09_ST_IDLE);
                break;
            }
            if (TrajFollower_Step(&walkOut, &w.vel, &w.heading)) {
                SetState(DAFFY09_ST_TURN0);
                break;
            }
            {
                SetFacing(w.heading);
                Vec3s_ScaleByDt(&w.vel, &w.delta);
                Translate(&w.delta);
                SnapToGround(0);
                w.dx = pos.x - camera->eye.x;
                w.dvert = pos.y - camera->eye.y;
                w.dz = pos.z - camera->eye.z;
                w.dxSq = w.dx * w.dx;
                w.dvertSq = w.dvert * w.dvert;
                w.dzSq = w.dz * w.dz;
                camRotX = Math_RadiansToAngle4096((float)atan2(w.dvert, (s32)sqrt((double)w.dxSq + w.dzSq))) & 0xfff;
                camRotY = Math_RadiansToAngle4096((float)atan2(-w.dx, w.dz)) & 0xfff;
                StartCamera(camRotX, camRotY, &camera->eye);
            }
            break;
        case DAFFY09_ST_TURN0:
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(DAFFY09_ST_TURN1);
            break;
        case DAFFY09_ST_TURN1:
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(DAFFY09_ST_WALK_BACK);
            break;
        case DAFFY09_ST_WALK_BACK:
            if (TrajFollower_Step(&walkBack, &w.velBack, &w.headingBack)) {
                SetState(DAFFY09_ST_IDLE);
                break;
            }
            {
                SetFacing(w.headingBack);
                Vec3s_ScaleByDt(&w.velBack, &w.deltaBack);
                Translate(&w.deltaBack);
                SnapToGround(0);
            }
            break;
    }
    AdvanceAnim();
}

/* 0x43ab0f - vtable +0x10. 0x3801 asks whether the placard is up; 0 (MSG_KILL) puts it up for two seconds, with the
 * model tinted black and the running cinematic cut short; 0xe releases the Wolf freeze. */
s32 DaffyLevel09::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    switch (msgId) {
        case MSG_DAFFY09_IS_PLACARD:
            if (state == DAFFY09_ST_PLACARD)
                return 1;
            break;
        case MSG_KILL:
            if (state) {
                if (Cine_IsActive()) {
                    g_cinePlayer.CaptureCamera();
                    g_cinePlayer.Stop();
                }
                placardTimeMs = 2000;
                tintColor = 0;
                tintAmount = 0x1000;
                SetTintOverride(1);
                SetState(DAFFY09_ST_PLACARD);
                return 1;
            }
            break;
        case MSG_FREEZE:
            wolfFrozen = 0;
            Camera_ReleaseScripted(this);
            return 1;
    }
    return 0;
}

/* 0x43ac33 - vtable +0x08: the body, plus the placard while it is up. */
void DaffyLevel09::Render(Camera *view)
{
    ScnMobile::Render(view);
    if (placardValid && state == DAFFY09_ST_PLACARD)
        placard.Render(view);
}

/* 0x43ac7f - the state entry: each state starts its animation, and the two talking states their cinematic. */
void DaffyLevel09::SetState(u8 newState)
{
    Vec3s p;

    state = newState;
    switch (state) {
        case DAFFY09_ST_CINE_INTRO:
            StartCine(cine, cineFlag, cineBox, cineSheepBox, Text_GetClassString(cineText));
            PlayAnim(AROBIN01_ANIM_TALK1, 1, 1);
            break;
        case DAFFY09_ST_CINE_DISGUISE:
            StartCine(cineDeguis, cineDeguisFlag, cineBox, cineSheepBox, Text_GetClassString(cineTextDeguis));
            PlayAnim(AROBIN01_ANIM_TALK1, 1, 1);
            break;
        case DAFFY09_ST_IDLE:
            if (wolfFrozen)
                /* cast kept: HandleMessage's arg is a void *; this message passes a number in it */
                wolfFrozen = (g_pWolf->HandleMessage(this, MSG_UNFREEZE, (void *)1) == 0);
            Camera_ReleaseScripted(this);
            PlayAnim(AROBIN01_ANIM_STAND1, 1, 1);
            break;
        case DAFFY09_ST_WALK_OUT:
            TrajFollower_Init(&walkOut, traj, 0x145, 0x800, 0, 0, 0x32);
            if (!wolfFrozen)
                /* cast kept: HandleMessage's arg is a void *; this message passes a number in it */
                wolfFrozen = g_pWolf->HandleMessage(this, MSG_FREEZE, (void *)1);
            PlayAnim(AROBIN01_ANIM_WALK1, 1, 1);
            break;
        case DAFFY09_ST_WALK_BACK:
            if (wolfFrozen)
                /* cast kept: HandleMessage's arg is a void *; this message passes a number in it */
                wolfFrozen = (g_pWolf->HandleMessage(this, MSG_UNFREEZE, (void *)1) == 0);
            Camera_ReleaseScripted(this);
            TrajFollower_Init(&walkBack, trajRet, 0x145, 0x800, 0, 0, 0x32);
            PlayAnim(AROBIN01_ANIM_WALK1, 1, 1);
            break;
        case DAFFY09_ST_TURN0:
            PlayAnim(AROBIN01_ANIM_TURN0, 0, 1);
            break;
        case DAFFY09_ST_TURN1:
            /* cast kept: HandleMessage's arg is a void *; this message passes a number in it */
            swirlSign->HandleMessage(this, MSG_SWIRLSIGN_TURN, (void *)1);
            PlayAnim(AROBIN01_ANIM_TURN1, 0, 1);
            break;
        case DAFFY09_ST_PLACARD:
            p = pos;
            p.x += (s16)((g_sinTable4096[Facing()] * 100) >> 12);
            p.y -= 0x28;
            p.z += (s16)((g_pCosTable[Facing()] * 100) >> 12);
            placard.pos = p;
            placard.PlayAnim(AETOIL01_ANIM_TURN1, 1, 1);
            EnableBoxCollide(0);
            soundHandle = Sound_Play(SND_DAFFY09_PLACARD, this, 0xff, SNDF_POSITIONAL, 0x1000);
            PlayAnim(AROBIN01_ANIM_BURN2, 0, 1);
            break;
    }
}

/* 0x43b246 - the class factory for CLASSID 104 "DaffyLevel09": new DaffyLevel09 (the ScnObject, ScnBody and ScnMobile
 * vtables, then the placard sub-object's own two, then DaffyLevel09's), then ScnMobile_Init(record, 0). */
ScnObject *DaffyLevel09_Create(void *record)
{
    DaffyLevel09 *obj = new DaffyLevel09;
    obj = (DaffyLevel09 *)obj->Init(record, 0); /* cast kept: Init returns the ScnObject base */
    return obj;
}
