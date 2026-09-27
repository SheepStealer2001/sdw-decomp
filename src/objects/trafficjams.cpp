/* PAL PC TrafficJams, 0x4fc840-0x4fd5ac. */
/* BYTES: slot-group. */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *inst, Animator *animator, u16 id, u32 options);

#define SDW_MEMBERS_ScnObject            \
    static void *operator new(u32 size); \
    void SetUpdateMode(u8 mode);

#define SDW_MEMBERS_CollBox s32 ContainsXZ(const Vec3s *p);
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_COLLBOX_CONTAINSXZ_CONST_VEC3S 1
#include "../engine/coll_box_inlines.h"
#undef SDW_INLINE_COLLBOX_CONTAINSXZ_CONST_VEC3S
extern Wolf *g_pWolf;
extern s32 g_dtMs;
s32 Scenaric_FindByClass(u16 id, ScnObject **out, s32 maximum);
#include "../engine/scn_tools.h"
#define SDW_INLINE_FREE_READPROPERTY_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_READPROPERTY_VOID_U32
void TrafficJams::PostLoadInit()
{
    u8 index;
    void *props;
    for (index = 0; index < 4; ++index)
        ring[index] = 0;
    props = record;
    Scenaric_FindByClass(CLASSID_CHRONOMETER, &chronometer, 1);
    Scenaric_FindByClass(CLASSID_TRAIN, &train, 1);
    breakTimeMs = ReadProperty(props, 8);
    arrivingTimeMs = ReadProperty(props, 0);
    station = Scn_GetPropObject(props, 0x10);
    stationBox = Scn_GetPropBox(props, 4);
    ring[0] = this;
    ring[1] = Scn_GetPropObject(props, 0xc);
    ring[1]->HandleMessage(this, MSG_JAMS_SET_PREV, 0);
    /* cast kept: the message arg is a void *; this message passes a number in it */
    station->HandleMessage(this, MSG_JAMS_STATION_ACTIVE, (void *)arrivingTimeMs);
    lastSegTimeMs = 0;
    switchLatch = 0;
    trainAtStation = 0;
    wolfInBox = 0;
    SetUpdateMode(SCN_UPD_ALWAYS);
    SetPhase(JAMS_PHASE_TRAIN_AWAY);
    SetState(JAMS_CLEAR);
}
void TrafficJams::Reset()
{
    switchLatch = 0;
    wolfInBox = 0;
    breakRemainingMs = 0;
}
/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets */
void TrafficJams::Update()
{
    struct Work {
        u16 sumIndex, scanIndex;
        s32 activeStation;
    } w;
    if (lastSegTimeMs == 0) {
        lastSegTimeMs = station->HandleMessage(this, MSG_STATION_GET_LASTSEG_MS, 0);
        lastSegRemainingMs = lastSegTimeMs;
    }
    if (ring[3])
        ring[1]->HandleMessage(this, MSG_JAMS_SET_PREV, ring[3]);
    /* cast kept (the three stationBox casts below): Box and CollBox are two views of one 16-byte record */
    if (stationBox) {
        if (((CollBox *)stationBox)->ContainsXZ(&g_pWolf->pos) == 1 && wolfInBox == 0) {
            wolfInBox = 1;
            if (!train->HandleMessage(this, MSG_TRAIN_GET_PASSENGER, 0))
                /* cast kept: the message arg is a void *; this message passes a number in it */
                chronometer->HandleMessage(this, MSG_CHRONO_VISIBLE, (void *)1);
        } else if (!((CollBox *)stationBox)->ContainsXZ(&g_pWolf->pos) && wolfInBox == 1) {
            wolfInBox = 0;
            chronometer->HandleMessage(this, MSG_CHRONO_VISIBLE, 0);
        }
        /* cast kept: the id list holds this box */
        if (((CollBox *)stationBox)->ContainsXZ(&g_pWolf->pos)) {
            etaMs = 0;
            w.activeStation = 0;
            if (train->HandleMessage(this, MSG_TRAIN_GET_PASSENGER, 0))
                arrivingRemainingMs = 0;
            for (w.scanIndex = 0; w.scanIndex < 4; ++w.scanIndex)
                if (ring[w.scanIndex] && ring[w.scanIndex]->HandleMessage(this, MSG_JAMS_STATION_ACTIVE, 0))
                    w.activeStation = w.scanIndex;
            if (w.activeStation) {
                for (w.sumIndex = 0; w.sumIndex < 4; ++w.sumIndex) {
                    if (w.sumIndex >= w.activeStation && ring[w.sumIndex]) {
                        etaMs += ring[w.sumIndex]->HandleMessage(this, MSG_JAMS_GET_BREAK_LEFT, 0);
                        etaMs += ring[w.sumIndex]->HandleMessage(this, MSG_JAMS_GET_ARRIVING_LEFT, 0);
                        etaMs += ring[w.sumIndex]->HandleMessage(this, MSG_JAMS_GET_LASTSEG_LEFT, 0);
                    }
                }
                etaMs += arrivingRemainingMs;
            } else
                etaMs = arrivingRemainingMs;
            if (state == JAMS_JAMMED && train->HandleMessage(this, MSG_TRAIN_IS_AT_STATION, 0) &&
                phase == JAMS_PHASE_TRAIN_HERE)
                /* cast kept: the message arg is a void *; this message passes a number in it */
                chronometer->HandleMessage(this, MSG_CHRONO_SHOW_DEPARTURE, (void *)((breakRemainingMs << 12) / 1000));
            else
                /* cast kept: the message arg is a void *; this message passes a number in it */
                chronometer->HandleMessage(this, MSG_CHRONO_SHOW_ARRIVAL, (void *)((etaMs << 12) / 1000));
        }
    }
    switch (phase) {
        case JAMS_PHASE_TRAIN_HERE:
            /* cast kept: the message arg is a void *; this message passes a number in it */
            if (!station->HandleMessage(this, MSG_JAMS_STATION_ACTIVE, (void *)arrivingTimeMs)) {
                SetPhase(JAMS_PHASE_TRAIN_AWAY);
                break;
            }
            if (arrivingRemainingMs > 0) {
                arrivingRemainingMs -= g_dtMs;
                if (arrivingRemainingMs < 0)
                    arrivingRemainingMs = 0;
            }
            if (trainAtStation != train->HandleMessage(this, MSG_TRAIN_IS_AT_STATION, 0)) {
                trainAtStation = train->HandleMessage(this, MSG_TRAIN_IS_AT_STATION, 0);
                if (!trainAtStation) {
                    SetState(JAMS_CLEAR);
                    breakRemainingMs = 0;
                }
            }
            trainAtStation = train->HandleMessage(this, MSG_TRAIN_IS_AT_STATION, 0);
            if (trainAtStation && state == JAMS_JAMMED) {
                breakRemainingMs -= g_dtMs;
                if (breakRemainingMs <= 0) {
                    SetState(JAMS_CLEAR);
                    breakRemainingMs = 0;
                }
            }
            if (arrivingRemainingMs == 0 && (breakRemainingMs == 0 || state != JAMS_JAMMED)) {
                lastSegRemainingMs -= g_dtMs;
                if (lastSegRemainingMs <= 0)
                    lastSegRemainingMs = 0;
            }
            break;
        case JAMS_PHASE_TRAIN_AWAY:
            /* cast kept: the message arg is a void *; this message passes a number in it */
            if (station->HandleMessage(this, MSG_JAMS_STATION_ACTIVE, (void *)arrivingTimeMs))
                SetPhase(JAMS_PHASE_TRAIN_HERE);
            break;
    }
    AdvanceAnim();
}
s32 TrafficJams::HandleMessage(ScnObject *sender, u32 message, void *arg)
{
    switch (message) {
        case MSG_BULL_TARGET_QUERY:
            if (state == JAMS_JAMMED) {
                if (arg)
                    *(s32 *)arg = breakTimeMs; /* cast kept: MSG_BULL_TARGET_QUERY's arg points at the s32 reply */
                return 1;
            }
            if (arg)
                *(s32 *)arg = arrivingTimeMs; /* cast kept: MSG_BULL_TARGET_QUERY's arg points at the s32 reply */
            return 0;
        case MSG_SWITCH_ON:
            if (!switchLatch) {
                if (state == JAMS_JAMMED)
                    SetState(JAMS_CLEAR);
                else
                    SetState(JAMS_JAMMED);
                switchLatch = 1;
            }
            break;
        case MSG_SWITCH_OFF:
            if (switchLatch)
                switchLatch = 0;
            break;
        case MSG_JAMS_SET_PREV:
            ring[3] = sender;
            ring[2] = (ScnObject *)arg; /* cast kept: MSG_JAMS_SET_PREV's arg is the ScnObject before the sender */
            break;
        case MSG_JAMS_GET_BREAK_LEFT:
            if (state == JAMS_JAMMED) {
                if (breakRemainingMs < 0)
                    breakRemainingMs = 0;
                return breakRemainingMs;
            }
            break;
        case MSG_JAMS_GET_ARRIVING_LEFT:
            if (arrivingRemainingMs < 0)
                arrivingRemainingMs = 0;
            return arrivingRemainingMs;
        case MSG_JAMS_GET_LASTSEG_LEFT:
            if (sender != this) {
                if (lastSegRemainingMs < 0)
                    lastSegRemainingMs = 0;
                return lastSegRemainingMs;
            }
            break;
        case MSG_JAMS_STATION_ACTIVE:
            /* cast kept: the message arg is a void *; this message passes a number in it */
            return station->HandleMessage(this, MSG_JAMS_STATION_ACTIVE, (void *)arrivingTimeMs);
    }
    return 0;
}
void TrafficJams::SetPhase(u8 value)
{
    phase = value;
    breakRemainingMs = breakTimeMs;
    arrivingRemainingMs = arrivingTimeMs;
    lastSegRemainingMs = lastSegTimeMs;
    switch (phase) {
        case JAMS_PHASE_TRAIN_AWAY:
            trainAtStation = 0;
            break;
    }
}
void TrafficJams::SetState(u8 value)
{
    state = value;
    switch (state) {
        case JAMS_JAMMED:
            PlayAnim(A20FEU01_ANIM_RED, 1, 0);
            break;
        case JAMS_CLEAR:
            PlayAnim(A20FEU01_ANIM_GREEN, 1, 0);
            if (train->HandleMessage(this, MSG_TRAIN_IS_AT_STATION, 0))
                breakRemainingMs = 0;
            break;
    }
}
ScnObject *TrafficJams_Create(u16 *record)
{
    TrafficJams *object = new TrafficJams;
    object = (TrafficJams *)object->Init(record, 0); /* cast kept: Init returns the object as a ScnBody * */
    return object;
}
