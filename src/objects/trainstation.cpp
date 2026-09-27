/* PAL PC TrainStation, 0x501150-0x501faf. */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
#define SDW_MEMBERS_ScnObject       \
    static void *operator new(u32); \
    void SetUpdateMode(s32 mode);
#include "sdw_classes.h"
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_SETVISIBLE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_SETVISIBLE_S32
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32
extern s32 g_dt;
#include "../app/app_main.h"
#include "../engine/scn_tools.h"
#include "camera.h"
s32 Vec3s_DistXZ(Vec3s *, Vec3s *);
#include "../sdk/crt.h"
extern "C" s16 Math_RadiansToAngle4096(float);
void Camera_StartScripted(ScnObject *, Camera *, u16, u16, u16, Vec3s *, u16, u32, s32);
/* cast kept (both): designer properties are 4-byte slots at byte offsets of the raw WAR record */
inline s32 PropertyS32(void *record, u32 offset)
{
    return *(s32 *)((u8 *)record + offset + 0x14);
}
#define SDW_INLINE_FREE_PROPERTYU32_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_PROPERTYU32_VOID_U32
#define ABS_VALUE(v) ((v) >= 0 ? (v) : -(v))
ScnObject *TrainStation_Create(void *record)
{
    TrainStation *object = new TrainStation;
    object = (TrainStation *)object->Init(record); /* cast kept: Init returns the ScnObject * base of this object */
    return object;
}
void TrainStation::SetState(u8 value)
{
    state = value;
    switch (value) {
        case STATION_IDLE:
            train = 0;
            break;
        case STATION_ROUTE_ACTIVE:
            timerTicks = 0;
            commandPending = 1;
            camActive = 0;
            break;
    }
}
void TrainStation::PostLoadInit()
{
    u16 index;
    u32 routeSpeed;
    s32 distance;
    void *properties;
    SetUpdateMode(SCN_UPD_ALWAYS);
    properties = record;
    box = Scn_GetPropBox(properties, 0);
    traj = Scn_GetPropTrajectory(properties, 16);
    next = Scn_GetPropObject(properties, 8);
    waitTicks = PropertyS32(properties, 20) * 4096 / 1000;
    camera = Scn_GetPropCamera(properties, 4);
    speedPercent = PropertyU32(properties, 12);
    if (next && next->GetClassId() != CLASSID_TRAINSTATION)
        next = 0;
    if (traj && traj->count < 3)
        traj = 0;
    if (traj) {
        for (index = 0; index < traj->count; index++) {
            if (ABS_VALUE(traj->pts[traj->count - 1].x - traj->pts[0].x) <
                ABS_VALUE(traj->pts[traj->count - 1].z - traj->pts[0].z))
                traj->pts[index].x = traj->pts[0].x;
            else
                traj->pts[index].z = traj->pts[0].z;
            traj->pts[index].y = traj->pts[0].y;
        }
    }
    routeSpeed = 1500;
    routeSpeed = routeSpeed * speedPercent / 100;
    distance = Vec3s_DistXZ(&traj->pts[traj->count - 2], &traj->pts[traj->count - 1]);
    lastSegTimeMs = distance * 1000;
    lastSegTimeMs /= routeSpeed;
    distance = Vec3s_DistXZ(&traj->pts[traj->count - 3], &traj->pts[traj->count - 2]);
    prevSegTimeMs = distance * 1000;
    prevSegTimeMs /= routeSpeed;
    trafficJams = 0;
    SetVisible(0);
    SetState(STATION_IDLE);
    skipWait = 0;
    passenger = 0;
}
void TrainStation::Reset() {}
void TrainStation::Update()
{
    u16 focalScale;
    Vec3s *cameraPosition;
    u16 camRoll;
    u32 *waitOut;
    u32 waypointDelay, jamWaitTime;
    Vec3s cameraAngles;
    s16 length;
    Vec3s difference;
    timerTicks += g_dt;
    switch (state) {
        case STATION_ROUTE_ACTIVE:
            /* cast kept: HandleMessage returns an s32; MSG_TRAIN_GET_PASSENGER answers with the passenger object
             * in it */
            passenger = (ScnObject *)train->HandleMessage(this, MSG_TRAIN_GET_PASSENGER, 0);
            if (camera) {
                if (passenger) {
                    difference.x = passenger->pos.x - (s16)camera->eye.x;
                    difference.y = passenger->pos.y - (s16)camera->eye.y;
                    difference.z = passenger->pos.z - (s16)camera->eye.z;
                    length = (s16)sqrt((double)difference.x * difference.x + difference.y * difference.y +
                                       difference.z * difference.z);
                    cameraAngles.x = Math_RadiansToAngle4096((float)atan2(difference.y, length)) & 4095;
                    cameraAngles.y = (-Math_RadiansToAngle4096((float)atan2(difference.x, difference.z))) & 4095;
                    focalScale = camera->focal;
                    cameraPosition = &camera->eye;
                    camRoll = camera->rot[2];
                    Camera_StartScripted(this, &g_camera, cameraAngles.x, cameraAngles.y, camRoll, cameraPosition,
                                         focalScale, 0, 4096);
                    camActive = 1;
                } else if (camActive) {
                    Camera_ReleaseScripted(this);
                    camActive = 0;
                }
            }
            if (commandPending) {
                jamWaitTime = 0;
                if (trafficJams) {
                    waitOut = &jamWaitTime;
                    if (skipWait)
                        waitOut = 0;
                    jammed = trafficJams->HandleMessage(this, MSG_BULL_TARGET_QUERY, waitOut);
                    jamWaitTime = (jamWaitTime << 12) / 1000;
                    if (waypointIndex == 0) {
                        if (train->HandleMessage(this, MSG_TRAIN_GET_PASSENGER, 0))
                            waypointDelay = 0;
                        else {
                            waypointDelay = (jamsArrivingTimeMs << 12) / 1000;
                            waypointDelay -= (prevSegTimeMs << 12) / 1000;
                        }
                    } else if (waypointIndex == traj->count - 1) {
                        if (jammed)
                            waypointDelay = jamWaitTime;
                        else
                            waypointDelay = 0;
                    } else
                        waypointDelay = 0;
                } else
                    waypointDelay = 0;
                if (timerTicks >= waypointDelay) {
                    waypoint = traj->pts[waypointIndex];
                    if (waypointIndex == traj->count - 1) {
                        waypointHeading = Math_RadiansToAngle4096(
                            (float)atan2(traj->pts[waypointIndex].x - traj->pts[waypointIndex - 1].x,
                                         traj->pts[waypointIndex].z - traj->pts[waypointIndex - 1].z));
                        train->HandleMessage(this, MSG_TRAIN_FINAL_WAYPOINT, &waypoint);
                    } else {
                        waypointHeading = (Math_RadiansToAngle4096((float)atan2(
                                               traj->pts[waypointIndex + 1].x - traj->pts[waypointIndex].x,
                                               traj->pts[waypointIndex + 1].z - traj->pts[waypointIndex].z)) +
                                           2048) &
                                          4095;
                        if (waypointIndex == 0)
                            train->HandleMessage(this, MSG_TRAIN_START_AT, &waypoint);
                        else
                            train->HandleMessage(this, MSG_TRAIN_WAYPOINT, &waypoint);
                    }
                    commandPending = 0;
                    skipWait = 0;
                }
            }
            break;
    }
}
s32 TrainStation::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    if (sender->GetClassId() == CLASSID_TRAIN || sender->GetClassId() == CLASSID_BIPBIPLEVEL14) {
        switch (msgId) {
            case MSG_STATION_QUERY_TRAIN_ZONE:
                if (state == STATION_ROUTE_ACTIVE)
                    return train->HandleMessage(this, MSG_TRAIN_IN_ZONE, 0);
                break;
            case MSG_STATION_GET_SPEEDPERCENT:
                return speedPercent;
            case MSG_STATION_BEGIN_ROUTE:
                if (sender->GetClassId() == CLASSID_TRAIN && !state && traj) {
                    waypointIndex = 0;
                    train = sender;
                    SetState(STATION_ROUTE_ACTIVE);
                    return 1;
                }
                break;
            case MSG_TRAIN_WAYPOINT:
                train = sender;
                if (!commandPending) {
                    waypointIndex++;
                    if (waypointIndex == traj->count - 1 &&
                        !trafficJams->HandleMessage(this, MSG_BULL_TARGET_QUERY, 0)) {
                        waypoint = traj->pts[waypointIndex];
                        waypointHeading = Math_RadiansToAngle4096(
                            (float)atan2(traj->pts[waypointIndex].x - traj->pts[waypointIndex - 1].x,
                                         traj->pts[waypointIndex].z - traj->pts[waypointIndex - 1].z));
                        train->HandleMessage(this, MSG_TRAIN_WAYPOINT, &waypoint);
                    }
                    if (waypointIndex < traj->count) {
                        timerTicks = 0;
                        commandPending = 1;
                    } else {
                        if (arg)
                            /* cast kept: the message arg is a void *; here it is where to write the next station */
                            *(ScnObject **)arg = next;
                        SetState(STATION_IDLE);
                    }
                } else
                    skipWait = 1;
                return 1;
        }
    }
    if (sender->GetClassId() == CLASSID_TRAFFICJAMS) {
        switch (msgId) {
            case MSG_JAMS_STATION_ACTIVE:
                trafficJams = sender;
                /* cast kept: the message arg is a void *; this message passes the arrival time (ms) in it */
                jamsArrivingTimeMs = (u32)arg;
                if (state == STATION_IDLE)
                    return 0;
                return 1;
            case MSG_STATION_GET_LASTSEG_MS:
                return lastSegTimeMs;
        }
    }
    return 0;
}
