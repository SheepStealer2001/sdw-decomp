/*
 * Wheel (class 111 "Wheel", vtable 0x577010, sizeof 0x1c8) - the big paddle wheel of Level 10 (disc Lvl-12), with four
 * platforms (its LIFT0..LIFT3 properties, WheelDummy objects) hanging from it at quarter turns. Standing on a platform
 * makes the wheel turn: Wheel_ComputeSpinStep 0x508aff sums a torque over the four arms, weighting an occupied arm 0x5a
 * and an arm below the hub 0x1e, and divides by 600 (empty) or 0x118 (loaded). A SuperButton's MSG_SWITCH_ON drives the
 * whole wheel along its rail by TRANSLATION over RAISINGTIME ms; past half the travel it glows red and reports the
 * rider to the Wolf and to the Gossamer_Boss. A sheep caught by a platform is killed (MSG_KILL).
 * SheepD3D.exe 0x5078f0-0x509678: PostLoadInit, Reset, Update, HandleMessage, SetState, ScanLift, UpdateSpin,
 * ComputeSpinStep, MoveAlongRail, PlaceLifts and the factory.
 *
 *
 * States (+0x42): WHEEL_ST_AT_START stopped at railStart, WHEEL_ST_ROLL_OUT driven along the rail, WHEEL_ST_AT_END
 * free to turn at the far end, WHEEL_ST_STOPPED held by MSG_TELEPORTED, WHEEL_ST_ROLL_BACK driven back. The turning
 * itself happens only in WHEEL_ST_AT_END.
 *
 * Devices that only pin the original code generation, not claims about the source text:
 *  - the inline helpers below (their names are descriptive; each is here because its expansion gives the original's stack
 *    temporaries: GetFirstModelBox its list pointer and its result, SetGlowTint a `this` temp for each lift,
 *    Sound_IsPlayingHandle and StopSound a u16 temp for the handle, Scn_GetPropU32 a temp for the offset,
 *    SetUpdateMode a 4-way jump table on a constant, Flags16_Set / Flags16_Clear the mask in a register);
 *  - the `w` structs, which group a function's locals into one local so their slots follow the struct layout instead
 *    of VC6's name hash (src/README.md). Measured here: a struct local's size is rounded up to a multiple of 8, so a
 *    group whose base is not 8-aligned cannot be written as one struct - hence the three loose block locals in
 *    MoveAlongRail, whose NAMES were chosen for their slots (tools/vc6_locals.py), and the two leading fields of
 *    ComputeSpinStep's struct, which hold what the original keeps below the rest of the frame.
 * A shape that reproduces the bytes is a representation, not evidence that the original source read that way.
 * The lifts are WheelDummy objects (class 112, another file): the code writes their two phase angles (+0x50 / +0x52)
 * and their world box (+0x44..+0x4f).
 */
/* BYTES: dead-code, slot-group, slot-name, slot-scope, view. */
/* BYTES(view): written through casts: the lifts' class (WheelDummy) belongs to another file */
#define SDW_MEMBERS_ScnObject                                \
    static void *operator new(u32 size);                     \
    /* inline: the result is a stack temp (0x508421) */      \
    CollBox *GetFirstModelBox(); /* inline, defined below */ \
    void SetUpdateMode(u8 mode);                             \
    /* inline, defined below */                              \
    void SetGlowTint(s16 amount);                            \
    /* inline: the handle is a stack temp (0x5086cc) */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
#include "../engine/sound_mgr.h"
#include "../app/app_main.h"
#include "../engine/scn_tools.h"
#include "sdw_classes.h"
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#define SDW_INLINE_SCNOBJECT_FACING 1
#define SDW_INLINE_SCNOBJECT_STOPSOUND_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_FACING
#undef SDW_INLINE_SCNOBJECT_STOPSOUND_U16
#include "../engine/grid_queries.h"
#include "../engine/property_math.h"

extern Wolf *g_pWolf;                /* 0x6cf310 */
extern s32 g_dt;                     /* 0x71b300 */
extern s32 g_dtMs;                   /* 0x71b2e8 */
extern "C" s16 g_sinTable4096[5122]; /* 0x57ece0 */
extern "C" const s16 *g_pCosTable;   /* 0x5814e4 */

u16 Sound_Play(u16 soundId, void *owner, u16 volume, u8 flags, s32 rate); /* 0x5491b8 */

#define SDW_ABS(v) ((v) >= 0 ? (v) : -(v))

/* ---- inline helpers ---- */

/* A designer property of the WAR record: the dword at record + 0x14 + offset (the offset is a stack temp, 0x507a22). */
#define SDW_INLINE_FREE_SCN_GETPROPU32_U16_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_SCN_GETPROPU32_U16_U32

/* The model's first collision box, or NULL: the list pointer is a temp (0x507a4a); same body as src/objects/snowball.cpp. */
inline CollBox *ScnObject::GetFirstModelBox()
{
    ModelBoxList *list = inst_model->boxes;
    if (list)
        return list->boxes;
    return 0;
}

/* Set / clear bits of a 16-bit flag word: the word's address is a stack temp and the mask is loaded into a register
 * (0x508210-0x50824e). */
#define SDW_INLINE_FREE_FLAGS16_SET_U16_U16 1
#include "instance_inlines.h"
#undef SDW_INLINE_FREE_FLAGS16_SET_U16_U16

#define SDW_INLINE_FREE_FLAGS16_CLEAR_U16_U16 1
#include "instance_inlines.h"
#undef SDW_INLINE_FREE_FLAGS16_CLEAR_U16_U16

#define SDW_INLINE_SCNOBJECT_SETTINTOVERRIDE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETTINTOVERRIDE_S32

/* The red glow the wheel and its lifts wear while the rail move is past half way: `this` is a stack temp when it is
 * one of the lifts (0x50825a), and none when it is the wheel itself. */
inline void ScnObject::SetGlowTint(s16 amount)
{
    tintColor = 0xff0000ff;
    tintAmount = amount;
    SetTintOverride(1);
}

/* inline: the update mode, a switch on its argument (the jump table at 0x507db7; ScnUpdateMode). */
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8

/* The sound-channel test: its argument is a stack temp (0x5086b0). */
inline s32 Sound_IsPlayingHandle(u16 handle)
{
    return Sound_IsPlaying(handle);
}

/* ---- Wheel ---- */

/* 0x5078f0 - vtable +0x00: the eight designer properties, the home position / rotation, the arm radius from the
 * wheel's own model box, the level's Gossamer_Boss, the four lifts at quarter turns, then WHEEL_ST_AT_START and
 * always-update. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; pad0, pad8, pad18 fill gaps */
void Wheel::PostLoadInit()
{
    /* one struct, so the eight locals keep the original's slots (see the file header) */
    struct {
        u8 pad0[3];
        u8 k;         /* [ebp-0x1d] */
        CollBox *box; /* [ebp-0x1c] */
        u8 pad8[2];
        s16 step;       /* [ebp-0x16] */
        ScnObject **p;  /* [ebp-0x14] */
        s32 unused;     /* [ebp-0x10] */
        ScnObject *obj; /* [ebp-0x0c] */
        u8 pad18[2];
        u16 i;      /* [ebp-0x06] */
        u16 *props; /* [ebp-0x04] */
    } w;

    w.props = record;
    soundHandle = 0;
    switchSender = 0;
    homePos.x = pos.x;
    homePos.y = pos.y;
    homePos.z = pos.z;
    railPos.x = pos.x;
    railPos.y = pos.y;
    railPos.z = pos.z;
    railStart.x = pos.x;
    railStart.y = pos.y;
    railStart.z = pos.z;
    railStartSet = 0;
    railTravel = 0;
    glow = 0;
    homeRot.x = rot.x;
    homeRot.y = rot.y;
    homeRot.z = rot.z;
    spin.x = rot.x;
    spin.y = rot.y;
    spin.z = rot.z;
    liftLoaded = 0;
    wolfAtHub = 0;
    raisingTime = Scn_GetPropU32(w.props, 0x14); /* PROPERTY_WHEEL_RAISINGTIME */
    w.box = GetFirstModelBox();
    armRadius = (u16)((w.box->min.y - w.box->max.y) >> 1);
    armRadius += 30;
    boss = 0;
    w.p = g_scnObjects;
    w.obj = 0;
    w.unused = 0;
    w.i = 0;
    for (w.p = g_scnObjects, w.i = 0; w.i < g_scnObjectCount; w.i++, w.p++) {
        w.obj = *w.p;
        if (w.obj && w.obj->GetClassId() == CLASSID_GOSSAMER_BOSS) {
            boss = w.obj;
            break;
        }
    }
    mobil = 0;
    mobil = Scn_GetPropObject(w.props, 0x10);    /* PROPERTY_WHEEL_MOBIL */
    translation = Scn_GetPropU32(w.props, 0x1c); /* PROPERTY_WHEEL_TRANSLATION */
    switchAngle = 0;
    switchAngle = (s16)Scn_GetPropU32(w.props, 0x18); /* PROPERTY_WHEEL_SWITCHANGLE */
    switchAngle += (s16)((switchAngle >= 0) ? 1 : -1);
    for (w.k = 0; w.k < 4; w.k++)
        lifts[w.k] = 0;
    /* cast kept (the four lifts): Scn_GetPropObject returns the object as its ScnObject base */
    lifts[0] = (WheelDummy *)Scn_GetPropObject(w.props, 0); /* PROPERTY_WHEEL_LIFT0 */
    lifts[1] = (WheelDummy *)Scn_GetPropObject(w.props, 4);
    lifts[2] = (WheelDummy *)Scn_GetPropObject(w.props, 8);
    /* cast kept: the property object is known to be a WheelDummy */
    lifts[3] = (WheelDummy *)Scn_GetPropObject(w.props, 0xc);
    for (w.k = 0; w.k < 4; w.k++)
        ;
    w.box = lifts[0]->GetFirstModelBox();
    liftBoxSize.x = 0;
    liftBoxSize.y = (s16)(w.box->min.y - w.box->max.y);
    liftBoxSize.z = 0;
    w.step = 0x3ff;
    for (w.k = 0; w.k < 4; w.k++) {
        lifts[w.k]->railPhase = 0;
        lifts[w.k]->spinPhase = (u16)(w.k * w.step);
    }
    PlaceLifts(&spin.x);
    SetState(WHEEL_ST_AT_START);
    SetUpdateMode(SCN_UPD_ALWAYS);
}

/* 0x507dc7 - vtable +0x14 (level restart): back to the placed position and rotation, stopped. */
void Wheel::Reset()
{
    if (soundHandle && Sound_IsPlayingHandle(soundHandle)) {
        StopSound(soundHandle);
        soundHandle = 0;
    }
    railStart.x = homePos.x;
    railStart.y = homePos.y;
    railStart.z = homePos.z;
    spin.x = homeRot.x;
    spin.y = homeRot.y;
    spin.z = homeRot.z;
    liftLoaded = 0;
    wolfAtHub = 0;
    SetPosition(&homePos);
    PlaceLifts(&homeRot.x);
    railStartSet = 0;
    railTravel = 0;
    glow = 0;
    SetState(WHEEL_ST_AT_START);
}

/* 0x507ed7 - vtable +0x04: place the lifts, turn, ramp the red glow with the rail travel, tell the Wolf it is on the
 * wheel, and advance the rail move of WHEEL_ST_ROLL_OUT and WHEEL_ST_ROLL_BACK. */
void Wheel::Update()
{
    u16 i;
    s16 tint;

    wolfAtHub = 0;
    PlaceLifts(&spin.x);
    UpdateSpin();
    if (railTravel <= (translation >> 1) && glow < 1000) {
        glow = g_dtMs * 1000 / (s32)(raisingTime >> 1) + glow;
        if (glow > 1000)
            glow = 1000;
    } else if (glow != 0) {
        glow -= g_dtMs * 1000 / (s32)(raisingTime >> 1);
        if (glow < 0)
            glow = 0;
    }
    tint = (s16)((glow << 12) / 1000);
    if (glow >= 500 && liftLoaded == 0) {
        hubBox.min.x = (s16)(GetFirstModelBox()->min.x + pos.x);
        hubBox.min.y = (s16)(GetFirstModelBox()->min.y + pos.y);
        hubBox.min.z = (s16)(GetFirstModelBox()->min.z + pos.z);
        hubBox.max.x = (s16)(GetFirstModelBox()->max.x + pos.x);
        hubBox.max.y = (s16)(GetFirstModelBox()->max.y + pos.y);
        hubBox.max.z = (s16)(GetFirstModelBox()->max.z + pos.z);
        hubBox.min.y -= 10;
        overlapCount = ObjGrid_QueryBoxOverlap(&hubBox, overlap);
        for (i = 0; i < overlapCount; i++) {
            if (overlap[i]->GetClassId() == CLASSID_WOLF) {
                wolfAtHub = 1;
                break;
            }
        }
    }
    SetGlowTint(tint);
    lifts[0]->SetGlowTint(tint);
    lifts[1]->SetGlowTint(tint);
    lifts[2]->SetGlowTint(tint);
    lifts[3]->SetGlowTint(tint);
    if (glow >= 500 && (liftLoaded || wolfAtHub))
        /* cast kept: the throw heading travels in the void * argument */
        g_pWolf->HandleMessage(this, MSG_WOLF_THROW, (void *)(s16)((Facing() + 0x400) & 0xfff));
    switch (state) {
        case WHEEL_ST_ROLL_OUT:
            if (MoveAlongRail(translation, 1))
                SetState(WHEEL_ST_AT_END);
            break;
        case WHEEL_ST_ROLL_BACK:
            if (MoveAlongRail(0, -1))
                SetState(WHEEL_ST_AT_START);
            break;
    }
}

/* 0x5084aa - vtable +0x10: the switch messages that drive the rail move, and MSG_TELEPORTED (hold / re-home). */
s32 Wheel::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    if (sender) {
        switch (msgId) {
            case MSG_FREEZE:
                if (sender->GetClassId() == CLASSID_WOLF)
                    return 1;
                break;
            case MSG_SWITCH_ON:
                if (switchSender == 0)
                    switchSender = sender;
                if (state == WHEEL_ST_AT_START || state == WHEEL_ST_ROLL_BACK)
                    SetState(WHEEL_ST_ROLL_OUT);
                break;
            case MSG_SWITCH_OFF:
                if (switchSender == 0)
                    switchSender = sender;
                if (state == WHEEL_ST_ROLL_OUT || state == WHEEL_ST_AT_END)
                    SetState(WHEEL_ST_ROLL_BACK);
                break;
            case MSG_TELEPORTED:
                switch ((s32)arg) { /* cast kept: MSG_TELEPORTED's value travels in the void * argument */
                    case 1:
                        SetState(WHEEL_ST_STOPPED);
                        break;
                    default:
                        railPos = pos;
                        railStart = pos;
                        PlaceLifts(&spin.x);
                }
        }
    }
    return 0;
}

/* 0x50862e - the state entry: the two sounds, the rail-start latch and the two reports to the Gossamer_Boss. */
void Wheel::SetState(u8 newState)
{
    state = newState;
    switch (newState) {
        case WHEEL_ST_AT_START:
            if (railStartSet == 0) {
                railStart.x = pos.x;
                railStart.y = pos.y;
                railStart.z = pos.z;
                railStartSet = 1;
            }
            if (soundHandle && Sound_IsPlayingHandle(soundHandle)) {
                StopSound(soundHandle);
                soundHandle = 0;
            }
            break;
        case WHEEL_ST_ROLL_OUT:
            soundHandle = Sound_Play(SND_WHEEL_ROLL, this, 0xff, SNDF_POSITIONAL, 0x1000);
            break;
        case WHEEL_ST_AT_END:
            bossMsgSender = switchSender;
            bossMsgOn = 1;
            if (boss)
                boss->HandleMessage(this, MSG_WHEEL_SPIN, &bossMsgSender);
            if (soundHandle && Sound_IsPlayingHandle(soundHandle)) {
                StopSound(soundHandle);
                soundHandle = 0;
            }
            break;
        case WHEEL_ST_STOPPED:
            if (soundHandle && Sound_IsPlayingHandle(soundHandle)) {
                StopSound(soundHandle);
                soundHandle = 0;
            }
            break;
        case WHEEL_ST_ROLL_BACK:
            bossMsgSender = switchSender;
            bossMsgOn = 0;
            if (boss)
                boss->HandleMessage(this, MSG_WHEEL_SPIN, &bossMsgSender);
            soundHandle = Sound_Play(SND_WHEEL_ROLL, this, 0xff, SNDF_POSITIONAL, 0x1000);
            break;
    }
}

/* 0x50889b - what stands on lift `index`: a sheep is killed, the Wolf makes the lift count as loaded. */
s32 Wheel::ScanLift(u16 index)
{
    s32 found;
    u16 i;

    found = 0;
    i = 0;
    overlapCount = ObjGrid_QueryBoxOverlap(&lifts[index]->box, overlap);
    for (i = 0; i < overlapCount; i++) {
        if (overlap[i]->GetClassId() == CLASSID_SHEEP)
            overlap[i]->HandleMessage(this, MSG_KILL,
                                      (void *)KILL_GENERIC); /* cast kept: the cause is a void * argument */
        if (overlap[i]->GetClassId() == CLASSID_WOLF)
            found = 1;
    }
    return found;
}

/* 0x50897d - find the loaded lift, then, in WHEEL_ST_AT_END only, turn the wheel by this frame's step and re-place the
 * lifts. */
/* BYTES(dead-code): lift is stored and never read, as in the original */
void Wheel::UpdateSpin()
{
    u16 i;
    ScnObject *lift;

    liftLoaded = 0;
    for (i = 0; i < 4; i++) {
        lift = lifts[i];
        if (ScanLift(i)) {
            liftLoaded = 1;
            loadedLift = i;
            break;
        }
    }
    if (state == WHEEL_ST_AT_END) {
        spinStep = ComputeSpinStep();
        if (spinStep) {
            if ((soundHandle && Sound_IsPlayingHandle(soundHandle) == 0) || soundHandle == 0)
                soundHandle = Sound_Play(SND_WHEEL_SPIN, this, 0xff, SNDF_LOOP | SNDF_POSITIONAL, 0x1000);
        } else if (soundHandle)
            StopSound(soundHandle);
        spin.z += spinStep;
        spin.z &= 0xfff;
        PlaceLifts(&spin.x);
        if (liftLoaded)
            mobil->HandleMessage(this, MSG_WHEEL_SPIN,
                                 (void *)(s32)spinStep); /* cast kept: the step is a void * argument */
    }
}

/* 0x508aff - the torque on the wheel: for each arm, the cross product of the arm vector with a downward weight, dotted
 * with the unit vector of the current spin. An arm below the hub weighs 0x1e, the arm the Wolf stands on 0x5a. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; pad0a, pad12, pad1a, pad26, pad2e, pad36, pad3e, pad4a fill gaps */
s16 Wheel::ComputeSpinStep()
{
    /* one struct, so the locals keep the original's slots (see the file header); its first two fields are the
     * rotation's results, which sit below the rest in the original frame. */
    struct {
        s16 rx;       /* [ebp-0x50] */
        s16 rz;       /* [ebp-0x4e] */
        Vec3s center; /* [ebp-0x4c] */
        u8 pad0a[2];
        Vec3s arm; /* [ebp-0x44] */
        u8 pad12[2];
        Vec3s torque; /* [ebp-0x3c] */
        u8 pad1a[2];
        s32 onThisLift; /* [ebp-0x34] */
        Vec3s d;        /* [ebp-0x30] */
        u8 pad26[2];
        Vec3s p; /* [ebp-0x28] */
        u8 pad2e[2];
        Vec3s origin; /* [ebp-0x20] */
        u8 pad36[2];
        Vec3s moment; /* [ebp-0x18] */
        u8 pad3e[4];
        s16 total;  /* [ebp-0x0e] */
        s16 wx;     /* [ebp-0x0c] */
        s16 weight; /* [ebp-0x0a] */
        s16 wz;     /* [ebp-0x08] */
        u8 pad4a[4];
        u16 i; /* [ebp-0x02] */
    } w;
    s16 spinAngle; /* [ebp-0x52] */

    w.total = 0;
    w.onThisLift = 0;
    w.center = pos;
    w.center.y = -w.center.y;
    spin.x &= 0xfff;
    spin.y &= 0xfff;
    spin.z &= 0xfff;
    w.arm.x = -10;
    w.arm.y = 0;
    w.arm.z = 0;
    w.origin.x = 0;
    w.origin.y = 0;
    w.origin.z = 0;
    spinAngle = spin.y;
    spinAngle &= 0xfff;
    w.rx =
        (s16)(((w.arm.x - w.origin.x) * g_pCosTable[spinAngle] - (w.arm.z - w.origin.z) * g_sinTable4096[spinAngle]) /
                  0xfff +
              w.origin.x);
    w.rz =
        (s16)(((w.arm.x - w.origin.x) * g_sinTable4096[spinAngle] + (w.arm.z - w.origin.z) * g_pCosTable[spinAngle]) /
                  0xfff +
              w.origin.z);
    w.arm.x = w.rx;
    w.arm.z = w.rz;
    w.arm.x /= 10;
    w.arm.z /= 10;
    for (w.i = 0; w.i < 4; w.i++) {
        w.onThisLift = (liftLoaded && loadedLift == w.i);
        if (w.onThisLift == 0 || g_pWolf->pos.y > pos.y)
            w.p = lifts[w.i]->pos;
        else
            w.p = g_pWolf->pos;
        w.p.y = -w.p.y;
        w.d.x = (s16)(w.center.x - w.p.x);
        w.d.y = (s16)(w.center.y - w.p.y);
        w.d.z = (s16)(w.center.z - w.p.z);
        w.wx = 0;
        w.wz = 0;
        w.weight = 0;
        if (liftLoaded == 0 && w.p.y - w.center.y > 1)
            w.weight = 0x1e;
        else if (w.onThisLift)
            w.weight = 0x5a;
        w.torque.x = (s16)(-w.d.z * w.weight);
        w.torque.y = 0;
        w.torque.z = (s16)(w.d.x * w.weight);
        w.moment.x = (s16)(w.torque.x * w.arm.x);
        w.moment.y = 0;
        w.moment.z = (s16)(w.torque.z * w.arm.z);
        w.total += (s16)(w.moment.x + w.moment.z);
    }
    if (liftLoaded == 0)
        w.total /= 600;
    else
        w.total /= 0x118;
    if (w.total == 0 && liftLoaded && g_pWolf->pos.y < pos.y)
        w.total = 4;
    return w.total;
}

/* 0x508e41 - one step of the rail move: the wheel, its four lifts (with their world boxes) and, when a lift carries
 * him, the Wolf are all translated along the spin direction. Returns 1 when the move is over. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; pad06, pad16, pad1e, pad24 fill gaps */
/* BYTES(slot-scope): the nested block(s) only order the frame: their locals are allocated after the enclosing scope's */
/* BYTES(slot-name): names chosen for their slots: rz [ebp-0x2c], outX [ebp-0x2e], spinAngle [ebp-0x30] */
s32 Wheel::MoveAlongRail(s32 limit, s8 dir)
{
    /* one struct, so the locals keep the original's slots (see the file header) */
    struct {
        Vec3s arm; /* [ebp-0x28] */
        u8 pad06[4];
        s16 step;          /* [ebp-0x1e] */
        WheelDummy *lift2; /* [ebp-0x1c] */
        Vec3s delta;       /* [ebp-0x18] */
        u8 pad16[2];
        Vec3s origin; /* [ebp-0x10] */
        u8 pad1e[2];
        WheelDummy *lift; /* [ebp-0x08] */
        u8 pad24[2];
        s16 speed; /* [ebp-0x02] */
    } w;
    u8 k;

    railTravel = Vec3s_DistXZ(&railStart, &pos);
    w.speed = (s16)(translation * 1000 / (s32)raisingTime);
    w.step = 0;
    w.step = (s16)(w.speed * g_dt / 4096);
    if ((limit != 0 && railTravel >= limit) || (limit == 0 && railTravel <= SDW_ABS(w.step))) {
        if (limit == 0)
            SetPosition(&railStart);
        return 1;
    }
    w.arm.x = -10;
    w.arm.y = 0;
    w.arm.z = 0;
    w.origin.x = 0;
    w.origin.y = 0;
    w.origin.z = 0;
    {
        /* names chosen for the slots they get (src/README.md): rz [ebp-0x2c], outX [ebp-0x2e], spinAngle [ebp-0x30] */
        s16 rz;
        s16 outX;
        s16 spinAngle;

        spinAngle = spin.y;
        spinAngle &= 0xfff;
        outX = (s16)(((w.arm.x - w.origin.x) * g_pCosTable[spinAngle] -
                      (w.arm.z - w.origin.z) * g_sinTable4096[spinAngle]) /
                         0xfff +
                     w.origin.x);
        rz = (s16)(((w.arm.x - w.origin.x) * g_sinTable4096[spinAngle] +
                    (w.arm.z - w.origin.z) * g_pCosTable[spinAngle]) /
                       0xfff +
                   w.origin.z);
        w.arm.x = outX;
        w.arm.z = rz;
        w.arm.x /= 10;
        w.arm.z /= 10;
    }
    w.delta.x = (s16)(w.arm.x * w.step * dir);
    w.delta.y = (s16)(w.arm.y * w.step * dir);
    w.delta.z = (s16)(w.arm.z * w.step * dir);
    Translate(&w.delta);
    railPos = pos;
    for (k = 0; k < 4; k++) {
        w.lift = lifts[k];
        w.lift2 = w.lift;
        w.lift->Translate(&w.delta);
        w.lift2->box.min.x += w.delta.x;
        w.lift2->box.min.y += w.delta.y;
        w.lift2->box.min.z += w.delta.z;
        w.lift2->box.max.x += w.delta.x;
        w.lift2->box.max.y += w.delta.y;
        w.lift2->box.max.z += w.delta.z;
    }
    if (liftLoaded)
        g_pWolf->Translate(&w.delta);
    return 0;
}

/* 0x509140 - put the four lifts where the wheel's rotation says: hub, out along the arm, then the two rotations that
 * carry the arm around; each lift's world box follows, and the Wolf rides the lift he stands on. */
/* BYTES(slot-group): locals grouped in w / t / u only to pin the original frame offsets; pad04, pad0e, pad16, pad22, pad2a, pad32, pad3a, pad0, pad14, pad0, pad14 fill gaps */
/* BYTES(slot-scope): the nested block(s) only order the frame: their locals are allocated after the enclosing scope's */
/* BYTES(slot-scope): GetFirstModelBox written out: the list pointer is a block local ([ebp-0x64]) because the original stores straight into box */
void Wheel::PlaceLifts(s16 *rot)
{
    /* one struct, so the locals keep the original's slots (see the file header) */
    struct {
        CollBox *box; /* [ebp-0x40] */
        u8 pad04[3];
        u8 k;            /* [ebp-0x39] */
        Vec3s wolfDelta; /* [ebp-0x38] */
        u8 pad0e[2];
        Vec3s liftRot; /* [ebp-0x30] */
        u8 pad16[2];
        WheelDummy *lift2; /* [ebp-0x28] */
        Vec3s center;      /* [ebp-0x24] */
        u8 pad22[2];
        Vec3s wheelRot; /* [ebp-0x1c] */
        u8 pad2a[2];
        Vec3s ridePos; /* [ebp-0x14] */
        u8 pad32[2];
        Vec3s armOffset; /* [ebp-0x0c] */
        u8 pad3a[2];
        WheelDummy *lift; /* [ebp-0x04] */
    } w;

    w.center = pos;
    w.armOffset.x = armRadius;
    w.armOffset.y = 0;
    w.armOffset.z = -120;
    rot[0] &= 0xfff;
    rot[1] &= 0xfff;
    rot[2] &= 0xfff;
    w.wheelRot.z = (s16)(-rot[0] & 0xfff);
    w.wheelRot.x = (s16)(rot[2] & 0xfff);
    w.wheelRot.y = (s16)(-rot[1] & 0xfff);
    this->rot = w.wheelRot;
    w.liftRot.x = 0;
    w.liftRot.y = (s16)(w.wheelRot.y & 0xfff);
    w.liftRot.z = 0;
    for (w.k = 0; w.k < 4; w.k++) {
        w.lift = lifts[w.k];
        w.lift2 = w.lift;
        if (liftLoaded && loadedLift == w.k)
            w.ridePos = w.lift->pos;
        w.lift->SetPosition(&w.center);
        w.lift->Translate(&w.armOffset);
        w.lift->rot = w.liftRot;
        {
            struct {
                u8 pad0[2];
                s16 a;   /* [ebp-0x4e] */
                s16 nz;  /* [ebp-0x4c] */
                s16 nx;  /* [ebp-0x4a] */
                Vec3s p; /* [ebp-0x48] */
                u8 pad14[2];
            } t;

            t.a = (s16)((rot[2] + w.lift2->spinPhase) & 0xfff);
            t.p = w.lift->pos;
            t.a &= 0xfff;
            t.nx = (s16)(((t.p.x - w.center.x) * g_pCosTable[t.a] - (t.p.y - w.center.y) * g_sinTable4096[t.a]) / 4096 +
                         w.center.x);
            t.nz = (s16)(((t.p.x - w.center.x) * g_sinTable4096[t.a] + (t.p.y - w.center.y) * g_pCosTable[t.a]) / 4096 +
                         w.center.y);
            t.p.x = t.nx;
            t.p.y = t.nz;
            w.lift->SetPosition(&t.p);
        }
        {
            struct {
                u8 pad0[2];
                s16 b;   /* [ebp-0x5e] */
                s16 qx;  /* [ebp-0x5c] */
                s16 qz;  /* [ebp-0x5a] */
                Vec3s q; /* [ebp-0x58] */
                u8 pad14[2];
            } u;

            u.b = (s16)((rot[1] + w.lift2->railPhase - 0x400) & 0xfff);
            u.q = w.lift->pos;
            u.b &= 0xfff;
            u.qx =
                (s16)(((u.q.x - w.center.x) * g_pCosTable[u.b] - (u.q.z - w.center.z) * g_sinTable4096[u.b]) / 0xfff +
                      w.center.x);
            u.qz =
                (s16)(((u.q.x - w.center.x) * g_sinTable4096[u.b] + (u.q.z - w.center.z) * g_pCosTable[u.b]) / 0xfff +
                      w.center.z);
            u.q.x = u.qx;
            u.q.z = u.qz;
            w.lift->SetPosition(&u.q);
        }
        {
            /* GetFirstModelBox written out: the original stores the result straight into `box`, so the list pointer
             * is a block local of its own ([ebp-0x64]) */
            ModelBoxList *list;

            list = w.lift->inst_model->boxes;
            if (list)
                w.box = list->boxes;
            else
                w.box = 0;
        }
        w.lift2->box.min.x = (s16)(w.box->min.x + liftBoxSize.x);
        w.lift2->box.min.y = (s16)(w.box->min.y + liftBoxSize.y);
        w.lift2->box.min.z = (s16)(w.box->min.z + liftBoxSize.z);
        w.lift2->box.max.x = (s16)(w.box->max.x + liftBoxSize.x);
        w.lift2->box.max.y = (s16)(w.box->max.y + liftBoxSize.y);
        w.lift2->box.max.z = (s16)(w.box->max.z + liftBoxSize.z);
        w.lift2->box.min.x += w.lift->pos.x;
        w.lift2->box.min.y += w.lift->pos.y;
        w.lift2->box.min.z += w.lift->pos.z;
        w.lift2->box.max.x += w.lift->pos.x;
        w.lift2->box.max.y += w.lift->pos.y;
        w.lift2->box.max.z += w.lift->pos.z;
        if (liftLoaded && loadedLift == w.k) {
            w.wolfDelta.x = (s16)(w.lift->pos.x - w.ridePos.x);
            w.wolfDelta.y = (s16)(w.lift->pos.y - w.ridePos.y);
            w.wolfDelta.z = (s16)(w.lift->pos.z - w.ridePos.z);
            g_pWolf->Translate(&w.wolfDelta);
        }
    }
}

/* 0x509614 - the class factory for CLASSID 111 "Wheel": new Wheel (the ScnObject, ScnLogic and Wheel vtables in
 * turn), then ScnLogic_Init(record) through vtable slot +0x20. */
ScnObject *Wheel_Create(void *record)
{
    Wheel *obj = new Wheel;
    obj = (Wheel *)obj->Init(record); /* cast kept: Init returns the object as its ScnObject base */
    return obj;
}
