/* PAL PC InstantHoover, 0x4cdd00-0x4cf0cc. */
/* BYTES: flow, slot-group, temp. */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *instance, Animator *animator, u16 id, u32 options);
class ScnObject;
#include "../engine/sound_mgr.h"
#include "../engine/interface.h"
#include "../engine/scn_tools.h"
#include "../engine/progress_inventory.h"

#define SDW_MEMBERS_ScnObject            \
    static void *operator new(u32 size); \
    void AttachTo(ScnObject *parent, u8 joint, Vec3s *offset, Vec3s *rot, u32 a, u32 b);


#define SDW_MEMBERS_CollBox s32 ContainsXZ(const Vec3s *p);
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_ISINWORLD 1
#define SDW_INLINE_SCNOBJECT_ISKEPT 1
#define SDW_INLINE_SCNOBJECT_STOPSOUND_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_ISINWORLD
#undef SDW_INLINE_SCNOBJECT_ISKEPT
#undef SDW_INLINE_SCNOBJECT_STOPSOUND_U16
#define SDW_INLINE_SCNOBJECT_ENABLEBOXCOLLIDE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_ENABLEBOXCOLLIDE_S32
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_COLLBOX_CONTAINSXZ_CONST_VEC3S 1
#include "../engine/coll_box_inlines.h"
#undef SDW_INLINE_COLLBOX_CONTAINSXZ_CONST_VEC3S
extern Wolf *g_pWolf;
extern u32 g_uiTintColor;
extern u32 *g_screenLayerBase0, *g_screenLayerBase;
extern s32 g_dtMs;
#include "../sdk/crt.h"
s32 Vec3s_Dist(Vec3s *a, Vec3s *b);
s32 Scenaric_FindByClass(u16 id, ScnObject **out, s32 maximum);
/* BYTES(temp, inferred): the offset and the result go through the read struct because the original's expansion keeps both in stack slots */
#define SDW_INLINE_FREE_PROPU32_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_PROPU32_VOID_U32
#define ABS_VALUE(a) ((a) >= 0 ? (a) : -(a))
#define SQUARE(a) ((a) * (a))
#define SDW_INLINE_FREE_SCREENHEIGHTS32 1
#include "../engine/screen_inlines.h"
#undef SDW_INLINE_FREE_SCREENHEIGHTS32

Box *InstantHoover::FindDockBox()
{
    u16 index;
    for (index = 0; index < dockBoxCount; ++index) {
        /* cast kept: Box and CollBox are two views of one 16-byte record */
        if (((CollBox *)dockBoxes[index])->ContainsXZ(&pos)) {
            dockRadius = ABS_VALUE(dockBoxes[index]->min[0] - dockBoxes[index]->max[0]) >> 1;
            if (dockRadius > (ABS_VALUE(dockBoxes[index]->min[2] - dockBoxes[index]->max[2]) >> 1))
                dockRadius = ABS_VALUE(dockBoxes[index]->min[2] - dockBoxes[index]->max[2]) >> 1;
            return dockBoxes[index];
        }
    }
    return 0;
}
void InstantHoover::PostLoadInit()
{
    void *properties = record;
    s32 index;
    suckRange = PropU32(properties, 8);
    vertRange = PropU32(properties, 16);
    openTimeMs = PropU32(properties, 12);
    /* cast kept: an id list holds record pointers of any kind; this one lists boxes */
    dockBoxes = (Box **)Scn_GetPropIdList(properties, 0, &dockBoxCount);
    martianCount = (u8)Scenaric_FindByClass(CLASSID_INSTANTMARTIAN, martians, 10);
    socketCount = (u8)Scenaric_FindByClass(CLASSID_INSTANTSOCKET, sockets, 15);
    Scenaric_FindByClass(CLASSID_MARVIN, &marvin, 1);
    if (!martianCount)
        martians[0] = 0;
    for (index = 0; index < martianCount; ++index)
        martianCaptured[index] = 0;
    suckRange = 300;
    capturedCount = 0;
    openTimerMs = 0;
    hudIcon.LoadFromRes(DAV_IDI_IGHICONC);
    hudDigits.InitFromRes(DAV_IDI_ITICPTR_);
    martianInRange = 0;
    homePos = pos;
    state = IHOOVER_ST_IDLE;
    docked = 0;
    PlayAnim(APIEGH01_ANIM_CLOSED, 0, 0);
    EnableBoxCollide(0);
    soundHandle = 0;
}
void InstantHoover::Update()
{
    Vec3s target;
    switch (state) {
        case IHOOVER_ST_IDLE:
            if (docked) {
                martianInRange = 0;
                if (CountMartiansInRange() > 0) {
                    martianInRange = 1;
                    if (hudTint == g_uiTintColor)
                        hudTint = 0xaa0000;
                    else
                        hudTint = g_uiTintColor;
                } else
                    hudTint = g_uiTintColor;
                DrawHud();
            }
            break;
        case IHOOVER_ST_OPEN:
            DrawHud();
            if (openTimerMs > openTimeMs) {
                state = IHOOVER_ST_IDLE;
                openTimerMs = 0;
                PlayAnim(APIEGH01_ANIM_CLOSED1, 0, 0);
                state = IHOOVER_ST_ANIM;
                nextAction = IHOOVER_NEXT_AFTER_CLOSE;
                break;
            }
            openTimerMs += g_dtMs;
            if (docked)
                SuckMartians();
            openTimerMs += g_dtMs;
            break;
        case IHOOVER_ST_DROPPED:
            dockBox = FindDockBox();
            if (dockBox) {
                target.x = (dockBox->min[0] + dockBox->max[0]) >> 1;
                target.z = (dockBox->min[2] + dockBox->max[2]) >> 1;
                target.y = g_pWolf->pos.y;
                docked = 0;
                if ((s32)sqrt((double)SQUARE(target.x - pos.x) + (double)SQUARE(target.z - pos.z)) < dockRadius) {
                    SetPosition(&target);
                    docked = 1;
                    dockedSocket = FindNearestSocket();
                    if (dockedSocket)
                        FindNearestSocket()->HandleMessage(this, MSG_SOCKET_DOCKED, 0);
                }
            }
            state = IHOOVER_ST_IDLE;
            break;
        case IHOOVER_ST_ANIM:
            if (docked) {
                DrawHud();
                SuckMartians();
            }
            if (AnimFlags(ANIM_F_FINISHED)) {
                switch (nextAction) {
                    case IHOOVER_NEXT_IDLE_LOOP:
                        state = IHOOVER_ST_IDLE;
                        PlayAnim(APIEGH01_ANIM_CLOSED, 1, 0);
                        break;
                    case IHOOVER_NEXT_AFTER_CATCH:
                    case IHOOVER_NEXT_AFTER_CLOSE:
                        PlayAnim(APIEGH01_ANIM_CLOSED, 0, 0);
                        state = IHOOVER_ST_IDLE;
                        if (capturedCount == martianCount && marvin)
                            /* cast kept: arg carries a number */
                            marvin->HandleMessage(this, MSG_MARVIN_SOLVED, (void *)1);
                        break;
                    case IHOOVER_NEXT_OPEN:
                        state = IHOOVER_ST_OPEN;
                        PlayAnim(APIEGH01_ANIM_OPEN, 0, 0);
                        break;
                }
            }
            break;
    }
    AdvanceAnim();
}
void InstantHoover::DrawHud()
{
    s16 top = ScreenHeightS32() - 50;
    g_spriteCrayon2.Draw(g_screenLayerBase0 + 9, 10, top, 110, top + 40, hudTint, 0);
    if (martianCount - capturedCount > 9) {
        hudDigits.Draw(g_screenLayerBase + 9, 55, top + 10, 75, top + 30, 0xffffff, 1, 0);
        hudDigits.Draw(g_screenLayerBase + 9, 75, top + 10, 95, top + 30, 0xffffff, martianCount - capturedCount - 10,
                       0);
    } else {
        hudDigits.Draw(g_screenLayerBase + 9, 55, top + 10, 75, top + 30, 0xffffff, 0, 0);
        hudDigits.Draw(g_screenLayerBase + 9, 75, top + 10, 95, top + 30, 0xffffff, martianCount - capturedCount, 0);
    }
    hudIcon.Draw(g_screenLayerBase + 9, 15, top + 10, 47, top + 29, 0xffffff, 0);
}
/* BYTES(flow): the unreachable assignment emits nothing but advances /Od's register choice, as the original shows */
s32 InstantHoover::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    ScnObject *source;
    u8 joint;
    Vec3s *dropArg;
    switch ((s32)msgId) {
        /* Unreachable compiler-shaping setup: VC6 allocates for this assignment
       but emits no instructions. This reproduces the original register
       sequence; the historical source spelling is unknown. Case 4 retains
       its live setup. */
        source = sender;
        case MSG_REMOTE_SWITCH: {
            if (state == IHOOVER_ST_IDLE && arg == (void *)1 && docked) { /* cast kept: arg carries a number */
                hudTint = g_uiTintColor;
                state = IHOOVER_ST_ANIM;
                nextAction = IHOOVER_NEXT_OPEN;
                PlayAnim(APIEGH01_ANIM_OPEN1, 0, 0);
            }
            break;
        }
        case MSG_REMOTE_QUERY_UNAVAILABLE:
            if (docked)
                return 0;
            else
                return 1;
        case MSG_QUERY_ACTION:
            if (sender->GetClassId() == CLASSID_WOLF)
                return CTX_PICKUP;
            break;
        case MSG_INVENTORY_STORED:
            if (dockedSocket)
                FindNearestSocket()->HandleMessage(this, MSG_SOCKET_UNDOCKED, 0);
            return 1;
        case MSG_QUERY_HELD_ACTION:
            return HELD_THROWABLE;
        case MSG_PICKUP:
            docked = 0;
            source = sender;
            joint = (u8)(u32)arg; /* cast kept: arg carries the joint number */
            PlayAnim(APIEGH01_ANIM_LINK, 0, 0);
            AttachTo(source, joint, 0, 0, 0, 0);
            return 1;
        case MSG_DROP:
            docked = 0;
            dropArg = (Vec3s *)arg; /* cast kept: MSG_DROP's arg is the drop position */
            Detach();
            SetPosition(dropArg);
            PlayAnim(APIEGH01_ANIM_CLOSED, 0, 0);
            state = IHOOVER_ST_DROPPED;
            return 1;
        case MSG_CHECKPOINT_ROLLBACK:
            Reset();
            return 1;
    }
    return 0;
}
u8 InstantHoover::CountMartiansInRange()
{
    u8 count = 0;
    s32 index;
    if (martians) {
        for (index = 0; index < martianCount; ++index) {
            if (!martianCaptured[index]) {
                if ((s32)sqrt((double)SQUARE(martians[index]->pos.x - pos.x) +
                              (double)SQUARE(martians[index]->pos.z - pos.z)) < suckRange)
                    ++count;
            }
        }
    }
    return count;
}
void InstantHoover::SuckMartians()
{
    s32 index;
    if (martians) {
        for (index = 0; index < martianCount; ++index) {
            if (!martianCaptured[index] && Vec3s_Dist(&pos, &martians[index]->pos) < suckRange) {
                state = IHOOVER_ST_ANIM;
                nextAction = IHOOVER_NEXT_AFTER_CATCH;
                PlayAnim(APIEGH01_ANIM_CATCH, 0, 0);
                martians[index]->HandleMessage(this, MSG_HOOVER_SUCK, 0);
                martianCaptured[index] = 1;
                ++capturedCount;
            }
        }
    }
    if (Vec3s_Dist(&pos, &g_pWolf->pos) < 50) {
        state = IHOOVER_ST_ANIM;
        nextAction = IHOOVER_NEXT_AFTER_CATCH;
        PlayAnim(APIEGH01_ANIM_CATCH, 0, 0);
        g_pWolf->HandleMessage(this, MSG_WOLF_DIE_HOOVERED, 0);
    }
}
void InstantHoover::Reset()
{
    s32 index;
    if (IsInWorld()) {
        if (IsKept()) {
            RemoveFromWorld();
            Inventory_Add(this);
        } else
            SetPosition(&homePos);
    }
    capturedCount = 0;
    for (index = 0; index < 10; ++index) {
        if (martianCaptured[index] == 1) {
            if (martians[index]->HandleMessage(this, MSG_MARTIAN_IS_CAUGHT, 0) == 1)
                ++capturedCount;
            else
                martianCaptured[index] = 0;
        }
    }
    if (marvin) {
        if (capturedCount == martianCount)
            /* cast kept: arg carries a number */
            marvin->HandleMessage(this, MSG_MARVIN_SOLVED, (void *)1);
        else
            marvin->HandleMessage(this, MSG_MARVIN_SOLVED, 0);
    }
    PlayAnim(APIEGH01_ANIM_CLOSED, 0, 0);
    state = IHOOVER_ST_IDLE;
    StopSound(soundHandle);
    soundHandle = 0;
    docked = 0;
}
/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets; unused fill gaps */
ScnObject *InstantHoover::FindNearestSocket()
{
    /* Locals at -9 (index), -8 (best), -4 (distance), -2 (best distance). */
    struct Work {
        u8 unused[3];
        u8 index;
        ScnObject *best;
        u16 distance;
        u16 minimum;
    } w;
    w.best = 0;
    w.minimum = 10000;
    for (w.index = 0; w.index < socketCount; ++w.index) {
        w.distance = (u16)sqrt((double)SQUARE(sockets[w.index]->pos.x - pos.x) +
                               (double)SQUARE(sockets[w.index]->pos.z - pos.z));
        if (w.distance < w.minimum) {
            w.best = sockets[w.index];
            w.minimum = w.distance;
        }
    }
    if (w.minimum == 10000)
        return 0;
    return w.best;
}
ScnObject *InstantHoover_Create(void *record)
{
    ScnBody *object = new InstantHoover;
    object = object->Init(record, 0);
    return object;
}
