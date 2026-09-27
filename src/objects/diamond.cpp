/* T134 - original object Diamond.cpp (guessed name).
 * Ranges: .text 0x4b3270-0x4b3ad3, .rdata 0x575e78-0x575e9c (vtable), .data 0x57b584-0x57b590 (a copy of the cinematic
 * header's static opcode-stride table; per the map it is Diamond's, though it could equally belong to a separate
 * UpdateBeatTrack object - byte-identical either way). */
/* BYTES: cast, flow, layout, slot-name. */
/* BYTES(layout): the Cine.h header static: every object including the cinematic header carries this copy in its .data, referenced or not */
/* BYTES(cast): written v >= 0 ? 1 : -1: only this gives the original setge + lea in one register */
/* BYTES(flow): operand order: VC6 folds the int operand into the imul only when it comes second */
/*
 * Diamond (class 110, vtable 0x575e78, sizeof 0xbc) - the three gems of Level 10 (disc Lvl-12), the Gossamer level.
 * Each gem rides a two-point TRAJ while the player turns the gear that owns it: the gear sends MSG_WHEEL_SPIN (0x42)
 * with the signed amount it moved, and the gem interpolates between the first and the last point of its trajectory in
 * percent of a full 0xc00 of turn. At 0xc00 the gem is seated: it snaps to the end point, answers the gear with
 * MSG_TELEPORTED (0x43), and in that state teleports everything inside its ZONE box sideways by the width of
 * BOXTELEPORT and re-runs every VisibilityManager (CLASSID_VISIBILITYMANAGER) - the swap that lights one of the three
 * lamps of the Gossamer fight.
 * SheepD3D.exe 0x4b3270-0x4b3ad2: PostLoadInit, Update, HandleMessage, Reset, SetState, the zone teleport and the
 * class factory. 0x4b3ae0 is already DoorLevel's file.
 *
 *
 * States (+0xb8): DIAMOND_ST_LOOSE (the gem follows the gear), DIAMOND_ST_SEATED (reached 0xc00 of turn; the teleport
 * has been done).
 * Only backwards turn is refused: a negative amount is dropped by the sign test, so the gem never slides back.
 * Devices that only pin the original code generation: the inline helpers below (their names are descriptive; each one is
 * here because its expansion gives the original's stack temporaries) and the local names (under /Od a local's slot
 * follows from a hash of its name, tools/vc6_locals.py: rec, box in PostLoadInit; n, np, it, found, i, vis, moved,
 * ix in Diamond_TeleportZone).
 * Two shapes here are only about arrangement. The sign test is written `v >= 0 ? 1 : -1`: VC6 turns exactly that
 * into `setge` + `lea` with the result left in the same register, while the equivalent `(v >= 0) * 2 - 1` allocates
 * a second one. And the interpolation is written `percent * delta.x`, not `delta.x * percent`, because VC6 folds an
 * int operand into the imul when it comes second and loads it into a register when the s16 one does.
 * A shape that reproduces the bytes is a representation, not proof that the original source read that way.
 */
#define SDW_MEMBERS_ScnObject            \
    static void *operator new(u32 size); \
    void SetUpdateMode(u32 mode);
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
#include "../engine/sound_mgr.h"
#include "../engine/scn_tools.h"
#include "../engine/scenaric.h"
#include "../app/app_main.h"
#include "sdw_classes.h"
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_STOPSOUND_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_STOPSOUND_U16
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U32

/* ((v) >= 0) * 2 - 1: +1 for a forward turn, -1 for a backward one (0x4b369c-0x4b36a3). */
#define SDW_SIGN(v) ((v) >= 0 ? 1 : -1)

u16 Sound_Play(u16 soundId, void *owner, u16 volume, u8 flags, s32 rate); /* 0x5491b8 */

/* 0x57b584 - the cinematic header's static copy of the 9-byte opcode stride table (src/engine/cine.cpp,
 * g_cineOpStride 0x5816fc). The original header defined it static, so every object including it carries its own
 * unreferenced copy at the head of its .data; defined here in its place. */
static u8 s_cineOpStride[9] = {0, 8, 8, 4, 2, 2, 4, 2, 2};

/* 0x4b3270 - vtable +0x00: reads TRAJ, keeps its first and last point and their difference, reads ZONE and the width
 * of BOXTELEPORT, and parks the gem at its load position with the trajectory's end height. */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py) */
void Diamond::PostLoadInit()
{
    u16 *rec;
    Box *box;
    rec = record;
    zone = 0;
    traj = Scn_GetPropTrajectory(rec, 4);
    startPos.x = traj->pts[0].x;
    startPos.y = traj->pts[0].y;
    startPos.z = traj->pts[0].z;
    endPos.x = traj->pts[traj->count - 1].x;
    endPos.y = traj->pts[traj->count - 1].y;
    endPos.z = traj->pts[traj->count - 1].z;
    delta.x = endPos.x - startPos.x;
    delta.y = endPos.y - startPos.y;
    delta.z = endPos.z - startPos.z;
    progress = 0;
    percent = 0;
    sliding = 0;
    zone = Scn_GetPropBox(rec, 8);
    box = Scn_GetPropBox(rec, 0);
    teleportDx = box->max[0] - box->min[0];
    homePos = pos;
    homePos.y = endPos.y;
    SetPosition(&homePos);
    teleported = 0;
    SetUpdateMode(SCN_UPD_ALWAYS);
    SetUpdateMode(SCN_UPD_CINE);
    soundHandle = 0;
    SetState(DIAMOND_ST_LOOSE);
}

/* 0x4b3609 - vtable +0x04: the sliding loop runs only while turns keep arriving; the first frame without one stops it. */
void Diamond::Update()
{
    if (!sliding && soundHandle) {
        StopSound(soundHandle);
        soundHandle = 0;
        SetState(DIAMOND_ST_LOOSE);
    }
}

/* 0x4b3664 - vtable +0x10: MSG_WHEEL_SPIN, the gear's signed turn amount. Backward turns and a seated gem are refused. */
s32 Diamond::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    Vec3s p;
    if (sender) {
        switch (msgId) {
            case MSG_WHEEL_SPIN:
                /* cast kept (both (s32)arg): the gear passes its signed turn in the void * argument */
                if (state == DIAMOND_ST_SEATED || SDW_SIGN((s16)(s32)arg) != 1)
                    return 0;
                if (arg) {
                    sliding = 1;
                    soundHandle = Sound_Play(SND_DIAMOND_SLIDE, this, 0xff, SNDF_NO_RETRIGGER, 0x1000);
                } else {
                    sliding = 0;
                }
                progress += (s16)(s32)arg;
                if (progress < 0xc00) {
                    percent = progress * 100 / 0xc00;
                    p.x = startPos.x + percent * delta.x / 100;
                    p.y = endPos.y;
                    p.z = startPos.z + percent * delta.z / 100;
                    SetPosition(&p);
                } else {
                    SetPosition(&endPos);
                    sender->HandleMessage(this, MSG_TELEPORTED,
                                          (void *)1); /* cast kept: a message argument is a void * */
                    SetState(DIAMOND_ST_SEATED);
                }
                return 1;
        }
    }
    return 0;
}

/* 0x4b3811 - vtable +0x14: back to the load position with no turn banked. */
void Diamond::Reset()
{
    soundHandle = 0;
    SetPosition(&homePos);
    progress = 0;
    percent = 0;
    sliding = 0;
    SetState(DIAMOND_ST_LOOSE);
}

/* 0x4b3872 - state change; only entering DIAMOND_ST_SEATED does anything. */
void Diamond::SetState(u8 newState)
{
    if (state == newState)
        return;
    state = newState;
    switch (newState) {
        case DIAMOND_ST_SEATED:
            TeleportZone();
            soundHandle = Sound_Play(SND_DIAMOND_SEAT, this, 0xff, SNDF_NO_RETRIGGER, 0x1000);
            break;
    }
}

/* 0x4b38db - the swap the seated gem performs: every object whose grid point lies in ZONE is moved along x by the
 * width of BOXTELEPORT and told so with MSG_TELEPORTED, then every VisibilityManager is updated once. */
void Diamond::TeleportZone()
{
    s32 n;
    Vec3s np;
    ScnObject **it;
    ScnObject *found[64];
    s32 i;
    ScnObject *vis;
    ScnObject *moved;
    u16 ix;

    it = g_scnObjects;
    vis = 0;
    n = ObjGrid_QueryPointsInRectXZ(zone->min[0], zone->min[2], zone->max[0], zone->max[2], found);
    for (i = 0; i < n; i++) {
        moved = found[i];
        np = moved->pos;
        np.x += teleportDx;
        moved->SetPosition(&np);
        moved->HandleMessage(this, MSG_TELEPORTED,
                             (void *)teleportDx); /* cast kept: the dx travels in the void * argument */
    }
    teleported = 1;
    it = g_scnObjects;
    for (ix = 0; ix < g_scnObjectCount; ix++, it++) {
        vis = *it;
        if (vis && vis->GetClassId() == CLASSID_VISIBILITYMANAGER)
            vis->Update();
    }
}

/* 0x4b3a6c - the class factory for CLASSID_DIAMOND. */
ScnObject *Diamond_Create(void *record)
{
    ScnBody *obj = new Diamond;
    obj = obj->Init(record, 0);
    return obj;
}
