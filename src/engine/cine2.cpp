/* match-flags: /O2 /Oy- */
/*
 * T308 - original object guessed as Cine2.cpp (data/tu_map.json). Ranges: .data 0x581708-0x581714 (this object's copy of
 * the opcode-stride table); code: COMDATs only (/O2), 0x561be0-0x5626d0 (Cine_AdvanceScriptCursor .. Cine_DetachEntry),
 * 21 functions (Cine_Construct .. CaptureCamera and ResetIterators are T307).
 * The map lists this range as a PLACEHOLDER for T308..T311: four objects (table copies #2..#5 at 0x581708/14/20/2c)
 * sit between T307 and Cine_Update, and their code, if any, is in 0x561be0-0x5626d0 with boundaries unknown (the only
 * codegen constraint is that ResolveTracks 0x561dc0 is not in ResetIterators' object). Following the map, all of it is
 * here and T309..T311 hold only their table copies.
 *
 * The stride table is a header static: not const (it is in .data), internal linkage, one copy per including object;
 * this object's copy is not referenced by its code.
 * The cinematic player was built OPTIMISED, /O2 with frame pointers kept (/Oy-); the devices that pin its code
 * generation (locals such as `vis`, `o`, `e`/`blob`, `obj`, `now`, and the (bool) condition in ResolveTracks) are noted
 * where used.
 */
/* BYTES: cast, flow, layout, switches, temp. */
/* BYTES(layout): the Cine.h header static: every object including the cinematic header carries this copy in its .data, referenced or not */
/* BYTES(switches): built with /O2 /Oy-, not the project recipe; the file header says why */
#define SDW_MEMBERS_Cine Cine(); /* 0x5617c0 Cine_Construct */
#define SDW_MEMBERS_ScnObject \
    void AttachTo(ScnObject *parent, u8 joint, Vec3s *offset, Vec3s *rot, u32 arg, u32 arg2); /* 0x50ff14 */
#include "sdw_enums.h"
#include "sdw_classes.h"

/* 0x581708 - this object's copy of the cinematic header's opcode-stride table (a header static, see T307's
 * g_cineOpStride 0x5816fc); unreferenced here. */
static u8 g_cineOpStride_581708[9] = {0, 8, 8, 4, 2, 2, 4, 2, 2};
#include "../app/app_main.h"
#include "game_state.h"
#include "scenaric.h"
#include "id_list.h"
#include "scn_tools.h"
#include "../objects/camera.h"
#include "../objects/animation.h"
extern Vec3s g_camPos;       /* 0x584d20  = g_camera.pos */
extern u8 g_sharedScratch[]; /* 0x6d5468  shared scratch buffer */
extern Wolf *g_pWolf;        /* 0x6cf310 */

void Camera_StartScripted(ScnObject *owner, Camera *cam, u16 rotX, u16 rotY, u16 rotZ, Vec3s *pos, u16 focal, u32 mode,
                          s32 time);    /* 0x55a70d */
void Dialogue_SetBoxActive(s32 active); /* 0x539479 */

/* 0x561be0 - to the next record, and to the next track when this one's records are used up; 0 at the end */
s32 Cine::AdvanceScriptCursor()
{
    s32 pad = 0;
    if ((scriptCursor->opcode == CINE_OP_FLICKER_PHASE_A || scriptCursor->opcode == CINE_OP_FLICKER_PHASE_B ||
         scriptCursor->opcode == CINE_OP_ATTACH_A || scriptCursor->opcode == CINE_OP_ATTACH_B) &&
        (scriptCursor->count & 1))
        pad = 2;
    /* cast kept: a script is a byte stream of records whose size depends on the opcode (and the 2-byte pad) */
    scriptCursor = (CineRecord *)((u8 *)RecordSize() + pad);
    if (--scriptRemaining == 0) {
        blocksRemaining--;
        /* cast kept: a track header follows the last record of the previous track in the byte stream */
        targetDesc = (CineTrack *)scriptCursor;
        /* cast kept: a track's records follow its header in the byte stream */
        scriptCursor = (CineRecord *)(targetDesc + 1);
        scriptRemaining = targetDesc->recordCount;
    }
    return blocksRemaining != 0;
}

/* 0x561c50 - to the next track; 0 at the end */
s32 Cine::SkipRecords()
{
    s32 i;
    for (i = 0; i < targetDesc->recordCount; i++)
        if (!AdvanceScriptCursor())
            return 0;
    return 1;
}

/* 0x561c90 - lets a search scan ahead and come back (the FindFirst* functions) */
void Cine::SaveIterator()
{
    savedTargetDesc = targetDesc;
    savedBlocksRemaining = blocksRemaining;
    savedScriptCursor = scriptCursor;
    savedScriptRemaining = scriptRemaining;
}

/* 0x561cb0 */
void Cine::RestoreIterator()
{
    targetDesc = savedTargetDesc;
    blocksRemaining = savedBlocksRemaining;
    scriptCursor = savedScriptCursor;
    scriptRemaining = savedScriptRemaining;
}

/* 0x561cd0 - {speaker, idle|talk anims, camera setup}, or none */
void Cine::SetDialogueParams(const CineDialogue *p)
{
    if (p != 0) {
        dialogue = *p;
        return;
    }
    dialogue.speaker = 0;
    dialogue.camSetup = 0;
}

/* 0x561d10 - binds the cinematic cineId: finds it in the WAR, resolves and freezes its actors, notes a camera track */
void Cine::Load()
{
    u16 n;
    claimFlag = 0;
    runFlags &= ~CINE_RUN_VOICE_STARTED;
    resource = WarReloc_Find((u16)cineId, &n, &claimFlag);
    dialogueMode = 0;
    /* cast kept: the cinematic resource is its voice stream id (one u32) followed by the track stream */
    tracks = (CineTrack *)(resource + 1);
    trackCount = n;
    cameraCaptured = 0;
    ResolveTracks();
    TagObjectsInBox();
    NotifyActors();
    hasCameraTrack = 0;
    ResetIterators();
    do {
        if (targetDesc->selector == CINE_TRACK_CAMERA)
            hasCameraTrack = 1;
    } while (SkipRecords() && !hasCameraTrack);
}

/* 0x561dc0 - finds each track's object: 1/2 by its WAR record (flagged 0x80 and, with flags & 0x40, saved), 3/4 in the
 * cinematic-only table. No bound on the slots: actorSlots holds 46 (0x7e8 / 0x2c).
 * The (bool) makes VC6 lay the 1/2 block out first, as the original does (a plain || puts the else chain first);
 * e and blob fix the order of the three loads. */
/* BYTES(cast): the (bool) makes VC6 lay the 1/2 block out first (a plain \ */
/* BYTES(temp): e and blob fix the order of the three loads */
void Cine::ResolveTracks()
{
    CineActorSlot *slot = actorSlots;
    ResetIterators();
    do {
        if ((bool)(targetDesc->selector == CINE_TRACK_OBJECT || targetDesc->selector == CINE_TRACK_OBJECT_SPEAKER)) {
            u32 *e = &g_pDav->war.table[targetDesc->index];
            u8 *blob = g_pDav->war.blob;
            slot->obj = Scenaric_FindByRecord(blob + (*e & 0xffffff));
            targetDesc->obj = slot->obj;
            targetDesc->obj->flags |= SCN_OF_IN_CINE_BOX;
            if (flags & CINE_RESTORE_ACTORS)
                SaveActorState(slot->obj, &slot->save);
            slot++;
        } else if (targetDesc->selector == CINE_TRACK_STATIC || targetDesc->selector == CINE_TRACK_STATIC_B) {
            targetDesc->obj = g_cineObjects[targetDesc->index];
        }
    } while (SkipRecords());
}

/* 0x561e60 - applies the visibility of the flicker opcodes' first key and sends every flickered actor msg 0x12;
 * the Wolf gets his first position/rotation keys and the sheep box, or with flags & 0x18 no message at all
 * (wolfReady is set at once). FindFirstPosKey/FindFirstRotKey restore the iterator only when they find nothing, so
 * after the Wolf the walk resumes from his first rotation (else position) record, wherever that is in the script. */
/* BYTES(temp): obj: one load for both branches, as the original */
void Cine::NotifyActors()
{
    CineActorMsg msg;
    Vec3s wolfPos;
    ResetIterators();
    do {
        if ((targetDesc->selector == CINE_TRACK_OBJECT || targetDesc->selector == CINE_TRACK_OBJECT_SPEAKER ||
             targetDesc->selector == CINE_TRACK_STATIC || targetDesc->selector == CINE_TRACK_STATIC_B) &&
            (scriptCursor->opcode == CINE_OP_FLICKER_PHASE_A || scriptCursor->opcode == CINE_OP_FLICKER_PHASE_B)) {
            ScnObject *obj = targetDesc->obj; /* one load for both branches, as the original */
            if (scriptCursor->opcode == CINE_OP_FLICKER_PHASE_A)
                obj->flags &= ~SCN_OF_HIDDEN;
            else
                obj->flags |= SCN_OF_HIDDEN;
            if (targetDesc->obj == g_pWolf) {
                msg.pos = FindFirstPosKey();
                msg.rot = FindFirstRotKey();
                msg.box = 0;
                if (!(flags & CINE_WOLF_NO_HANDSHAKE_MASK)) {
                    msg.box = sheepBox;
                    targetDesc->obj->HandleMessage(0, MSG_CINE_PLACE, &msg);
                } else {
                    wolfPos.x = g_pWolf->pos.x;
                    wolfPos.y = g_pWolf->pos.y;
                    wolfPos.z = g_pWolf->pos.z;
                    msg.pos = &wolfPos;
                    wolfReady = 1;
                }
            } else {
                targetDesc->obj->HandleMessage(0, MSG_CINE_PLACE, 0);
            }
        }
    } while (AdvanceScriptCursor());
}

/* 0x561f50 - freezes (0x80) and hides every active object inside cineBox that is not an actor, not the Wolf and not
 * what he carries, remembering its visibility; at most 16. Only x and z are tested: the box has no vertical extent. */
/* BYTES(temp): vis gives the original's [this + count] operand order */
void Cine::TagObjectsInBox()
{
    Box *box = cineBox;
    ScnObject *carried;
    ScnObject **pp;
    u16 i;
    u8 *vis = taggedVisible; /* gives the original's [this + count] operand order */
    taggedCount = 0;
    if (box == 0)
        return;
    pp = g_scnActive;
    carried = 0;
    if (g_pWolf != 0) {
        if (g_pWolf->mode == WOLF_MODE_CARRY)
            carried = g_pWolf->heldObject;
        else
            carried = 0;
    }
    for (i = 0; i < g_scnActiveHigh; i++) {
        ScnObject *obj = *pp++;
        if (obj != 0 && !ScriptTargetsObject(obj) && obj != carried && obj != g_pWolf && obj->pos.x >= box->min[0] &&
            obj->pos.x <= box->max[0] && obj->pos.z >= box->min[2] && obj->pos.z <= box->max[2] && taggedCount < 16) {
            obj->flags |= SCN_OF_IN_CINE_BOX;
            if ((obj->flags & (SCN_OF_HIDDEN2 | SCN_OF_HIDDEN)) == 0)
                vis[taggedCount] = 1;
            else
                vis[taggedCount] = 0;
            obj->flags |= SCN_OF_HIDDEN;
            taggedObjs[taggedCount] = obj;
            taggedCount++;
        }
    }
}

/* 0x562070 - called by Load_DAVnWAR: marks the player idle */
void Cine::Reset()
{
    if (flags & CINE_LETTERBOX)
        Dialogue_SetBoxActive(0);
    active = 0;
    finished = 1;
    wolfReady = 0;
}

/* 0x5620a0 - called from Cine_Stop: claims the resource, plays the end state if the camera was captured, restores and
 * releases the actors (msg 0x13), restores the tagged objects, stops the voice, and with flags & 0x1000 requests the
 * level exit - whatever stopped the cinematic. */
/* BYTES(temp): o keeps 0xff7f and 0x800 in registers, as the original */
void Cine::Finish()
{
    u16 n;
    CineActorSlot *slot;
    u16 i;
    claimFlag = 1;
    resource = WarReloc_Find((u16)cineId, &n, &claimFlag);
    /* cast kept: the cinematic resource is its voice stream id (one u32) followed by the track stream */
    tracks = (CineTrack *)(resource + 1);
    if (cameraCaptured == 1)
        RunOpcodes();
    slot = actorSlots;
    ResetIterators();
    do {
        if (targetDesc->selector == CINE_TRACK_OBJECT || targetDesc->selector == CINE_TRACK_OBJECT_SPEAKER) {
            if (flags & CINE_RESTORE_ACTORS)
                RestoreActorState(slot->obj, &slot->save);
            slot->obj->flags &= ~SCN_OF_IN_CINE_BOX;
            targetDesc->obj->HandleMessage(0, MSG_CINE_END, 0);
            slot++;
        }
    } while (SkipRecords());
    for (i = 0; i < taggedCount; i++) {
        ScnObject *o = taggedObjs[i]; /* keeps 0xff7f and 0x800 in registers, as the original */
        if (taggedVisible[i])
            o->flags &= ~SCN_OF_HIDDEN;
        else
            o->flags |= SCN_OF_HIDDEN;
        taggedObjs[i]->flags &= ~SCN_OF_IN_CINE_BOX;
    }
    Voice_StopStream(0);
    if (flags & CINE_LEVEL_EXIT)
        g_levelExitFlags |= LEVEL_EXIT_NEXT;
}

/* 0x5621b0 - evaluates every record at the current time, then drives the scripted camera */
void Cine::RunOpcodes()
{
    InitRecordCursors();
    ResetIterators();
    do {
        if (scriptCursor->count != 0) {
            keyNext = keyCur = scriptCursor->keyCursor;
            switch (scriptCursor->opcode) {
                case CINE_OP_KEY_POSITION:
                    OpKeyPosition();
                    break;
                case CINE_OP_KEY_ROTATION:
                    OpKeyRotation();
                    break;
                case CINE_OP_PLAY_ANIM:
                    OpStartAnim();
                    OpAdvanceAnim();
                    break;
                case CINE_OP_FLICKER_PHASE_A:
                case CINE_OP_FLICKER_PHASE_B:
                    OpFlickerVisibility();
                    break;
                case CINE_OP_KEY_CAMSCALAR:
                    OpKeyCamScalar();
                    break;
                case CINE_OP_ATTACH_A:
                case CINE_OP_ATTACH_B:
                    OpToggleActorEntry();
                    break;
            }
        }
    } while (AdvanceScriptCursor());
    if (hasCameraTrack && cameraHeld)
        Camera_StartScripted(0, &g_camera, camRot.x, camRot.y, camRot.z, &camPos, camFocalScale, (~flags >> 1) & 3,
                             0x1000);
}

/* 0x5622b0 - position, rotation, visibility and (for an animated object) the animator state, at the start */
void Cine::SaveActorState(ScnObject *obj, CineActorSave *s)
{
    s->pos = obj->pos;
    s->rot = obj->rot;
    s->visible = (obj->flags & (SCN_OF_HIDDEN2 | SCN_OF_HIDDEN)) == 0;
    if (obj->inst_flags & INST_F_ANIMATED) {
        Animator *a = &((ScnBody *)obj)->anim; /* cast kept: an object with INST_F_ANIMATED is a ScnBody */
        s->anim40 = a->cur;
        s->anim50 = a->timeAcc;
        s->anim54 = a->pendingSoundId;
        s->anim5a[0] = a->animId;
        s->anim5a[1] = a->frame;
        s->anim5a[2] = a->frameDuration;
        s->anim5a[3] = a->speed;
        s->anim5a[4] = a->flags;
    }
}

/* 0x562340 - the inverse, through SetPosition; the animator pose is rebuilt from the restored frame */
void Cine::RestoreActorState(ScnObject *obj, CineActorSave *s)
{
    obj->rot = s->rot;
    obj->SetPosition(&s->pos);
    if (s->visible)
        obj->flags &= ~SCN_OF_HIDDEN;
    else
        obj->flags |= SCN_OF_HIDDEN;
    if (obj->inst_flags & INST_F_ANIMATED) {
        Animator *a = &((ScnBody *)obj)->anim; /* cast kept: an object with INST_F_ANIMATED is a ScnBody */
        a->cur = s->anim40;
        a->timeAcc = s->anim50;
        a->pendingSoundId = s->anim54;
        a->animId = s->anim5a[0];
        a->frame = s->anim5a[1];
        a->frameDuration = s->anim5a[2];
        a->speed = s->anim5a[3];
        a->flags = s->anim5a[4];
        Anim_RestorePose(a);
    }
}

/* 0x5623d0 - position key: kind 1 steps to 'from', kind 2 interpolates */
void Cine::KeyframePos(Vec3s *dest, Vec3s *from, Vec3s *to, s32 t0, s32 t1, u16 kind)
{
    switch (kind) {
        case CINE_KEY_STEP:
            dest->x = from->x;
            dest->y = from->y;
            dest->z = from->z;
            break;
        case CINE_KEY_LERP:
            LerpVec3s(dest, from, to, t0, t1);
            break;
    }
}

/* 0x562420 - out = from + (to - from) * (time - t0) / (t1 - t0) per component; 'to' once time reaches t1 */
void Cine::LerpVec3s(Vec3s *out, Vec3s *from, Vec3s *to, s32 t0, s32 t1)
{
    s32 num = time - t0;
    s32 den = t1 - t0;
    if (t0 != t1 && time < t1) {
        out->x = (to->x - from->x) * num / den + from->x;
        out->y = (to->y - from->y) * num / den + from->y;
        out->z = (to->z - from->z) * num / den + from->z;
        return;
    }
    *out = *to;
}

/* 0x5624c0 - rotation key: kind 1 steps to 'from', kind 2 interpolates each angle the short way round */
void Cine::KeyframeRot(Vec3s *dest, u16 *from, u16 *to, s32 t0, s32 t1, u16 kind)
{
    switch (kind) {
        case CINE_KEY_STEP:
            dest->x = from[0];
            dest->y = from[1];
            dest->z = from[2];
            break;
        case CINE_KEY_LERP:
            dest->x = LerpAngle(from[0], to[0], t0, t1);
            dest->y = LerpAngle(from[1], to[1], t0, t1);
            dest->z = LerpAngle(from[2], to[2], t0, t1);
            break;
    }
}

/* 0x562570 - a 12-bit angle from 'from' towards 'to' over [t0, t1], through the shorter arc (more than half a turn,
 * 0x800, goes the other way); 'to' when they are equal, t0 == t1 or time has reached t1. The result goes through `to`
 * and one masked return: four separate returns allocate the registers differently. */
/* BYTES(flow): the result goes through 'to' and one masked return: four separate returns allocate the registers differently */
u16 Cine::LerpAngle(s16 from, s16 to, s32 t0, s32 t1)
{
    s32 den = t1 - t0;
    s32 num = time - t0;
    s16 d;
    if (from != to && den != 0 && time < t1) {
        if (from < to) {
            d = to - from;
            if (d > 0x800) {
                d = 0x1000 - d;
                to = from - d * num / den;
            } else
                to = from + d * num / den;
        } else {
            d = from - to;
            if (d > 0x800) {
                d = 0x1000 - d;
                to = from + d * num / den;
            } else
                to = from - d * num / den;
        }
        return to & 0xfff;
    }
    return to;
}

/* 0x5625f0 - scalar key: kind 1 steps to 'from', kind 2 interpolates linearly */
/* BYTES(temp, inferred): now is a copy of time because the original reads it through a register of its own */
void Cine::KeyframeScalar(u16 *dest, u16 from, u16 to, s32 t0, s32 t1, u16 kind)
{
    s32 now;
    switch (kind) {
        case CINE_KEY_STEP:
            *dest = from;
            break;
        case CINE_KEY_LERP:
            now = time;
            if (t0 != t1 && now < t1) {
                *dest = (to - from) * (now - t0) / (t1 - t0) + from;
                return;
            }
            *dest = to;
            break;
    }
}

/* 0x562660 - attaches attachTable[i].child to its parent's joint, unless it is attached already */
void Cine::AttachEntry(s32 i)
{
    if (!(attachTable[i].child->inst_flags & INST_F_ATTACHED))
        attachTable[i].child->AttachTo(attachTable[i].parent, attachTable[i].joint, &attachTable[i].offset,
                                       &attachTable[i].rot, attachTable[i].arg, 0);
}

/* 0x5626b0 - detaches attachTable[i].child if it is attached */
void Cine::DetachEntry(s32 i)
{
    if (attachTable[i].child->inst_flags & INST_F_ATTACHED)
        attachTable[i].child->Detach();
}
