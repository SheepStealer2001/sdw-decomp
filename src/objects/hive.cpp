/* PAL PC Hive 0x4c9d40-0x4ca657. */
/* BYTES: dead-code, slot-scope. */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *, Animator *, u16, u32);

#define SDW_MEMBERS_ScnObject                           \
    static void *operator new(u32);                     \
    void StartCamera(u16, u16, u16, Vec3s *, u16, u32); \
    void SetUpdateMode(s32 mode);

#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
extern Wolf *g_pWolf;
extern s32 g_dtMs;
#include "../app/app_main.h"
#include "../engine/scn_tools.h"
#include "camera.h"
s32 Scenaric_FindByClass(u16, ScnObject **, s32);
void Camera_StartScripted(ScnObject *, Camera *, u16, u16, u16, Vec3s *, u16, u32, s32);
#include "../sdk/crt.h"
extern "C" s16 Math_RadiansToAngle4096(float);
#define SDW_INLINE_SCNOBJECT_STARTCAMERA_U16_U16_U16_VEC3S_U16_U32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_STARTCAMERA_U16_U16_U16_VEC3S_U16_U32
#define SDW_INLINE_FREE_READINTPROPERTY_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_READINTPROPERTY_VOID_U32
void Hive::PostLoadInit()
{
    void *props = record;
    camera = Scn_GetPropCamera(props, 0);
    samWatchTime = ReadIntProperty(props, 8);
    isMother = ReadIntProperty(props, 4);
    beeCount = Scenaric_FindByClass(CLASSID_BEES, bees, 4);
    Scenaric_FindByClass(CLASSID_SAM, &sam, 1);
    uiFrozen = 0;
    swarmPresent = 0;
    samInHoney = 0;
    swarm = 0;
    SetUpdateMode(SCN_UPD_ALWAYS);
    SetState(HIVE_ST_IDLE);
}
void Hive::Reset()
{
    samInHoney = 0;
    uiFrozen = 0;
}
void Hive::Update()
{
    switch (state) {
        case HIVE_ST_HIT:
            if (AnimFlags(ANIM_F_FINISHED)) {
                if (swarmPresent) {
                    if (isMother)
                        SetState(HIVE_ST_RELEASE_BEES);
                    else {
                        swarmPresent = 0;
                        SetState(HIVE_ST_TRACK_SWARM);
                    }
                } else {
                    Camera_ReleaseAny();
                    SetState(HIVE_ST_IDLE);
                }
            }
            if (trackSwarm)
                AimCameraAt(swarm->pos);
            break;
        case HIVE_ST_TRACK_SWARM:
            AimCameraAt(swarm->pos);
            break;
        case HIVE_ST_RELEASE_BEES:
            beeReleaseTimer -= g_dtMs;
            if (beeReleaseTimer <= 0) {
                /* cast kept: the bee's index travels in the void * argument */
                bees[beeIndex]->HandleMessage(this, MSG_BEES_RELEASE, (void *)(u32)beeIndex);
                beeIndex++;
                beeReleaseTimer = 150;
            }
            if (beeIndex >= beeCount) {
                swarmPresent = 0;
                if (!samInHoney)
                    SetState(HIVE_ST_TRACK_SWARM);
                else
                    SetState(HIVE_ST_WATCH_SAM);
            }
            break;
        case HIVE_ST_WATCH_SAM:
            AimCameraAt(sam->pos);
            samWatchTimer -= g_dtMs;
            if (samWatchTimer <= 0)
                SetState(HIVE_ST_IDLE);
            break;
    }
    AdvanceAnim();
}
/* BYTES(slot-scope, inferred): the nested block(s) only order the frame: their locals are allocated after the enclosing scope's */
s32 Hive::HandleMessage(ScnObject *sender, u32 msg, void *arg)
{
    ScnObject *object;
    {
        Vec3s delta;
        switch (msg) {
            case MSG_HIVE_RESET:
                SetState(HIVE_ST_IDLE);
                break;
            case MSG_HIVE_TRACK_SWARM:
                trackSwarm = 1;
                break;
            case MSG_HIVE_HIT:
                uiFrozen = g_pWolf->HandleMessage(this, MSG_FREEZE, 0);
                delta.x = pos.x - sender->pos.x;
                delta.y = pos.y - sender->pos.y;
                delta.z = pos.z - sender->pos.z;
                {
                    s16 angle = (Math_RadiansToAngle4096((float)atan2(delta.x, delta.z)) + 0xc00) & 0xfff;
                    rot.y = angle;
                }
                SetState(HIVE_ST_HIT);
                break;
            case MSG_HIVE_SWARM_HOME:
                swarmPresent = 1;
                swarm = sender;
                if (state == HIVE_ST_TRACK_SWARM)
                    SetState(HIVE_ST_IDLE);
                break;
            case MSG_HONEY_VICTIM:
                object = (ScnObject *)arg; /* cast kept: MSG_HONEY_VICTIM passes the victim in the void * argument */
                if (object->GetClassId() == CLASSID_SAM && state != HIVE_ST_WATCH_SAM) {
                    samInHoney = 1;
                    samWatchTimer = samWatchTime;
                }
                break;
            case MSG_FREEZE:
                uiFrozen = 0;
                Camera_ReleaseScripted(this);
                return 1;
        }
    }
    return 0;
}
void Hive::SetState(u8 next)
{
    state = next;
    switch (next) {
        case HIVE_ST_IDLE:
            PlayAnim(ANIDAB01_ANIM_DEFAULT, 1, 1);
            Camera_ReleaseScripted(this);
            if (uiFrozen)
                uiFrozen = !g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0);
            break;
        case HIVE_ST_RELEASE_BEES:
            beeReleaseTimer = 0;
            beeIndex = 0;
            break;
        case HIVE_ST_HIT:
            PlayAnim(ANIDAB01_ANIM_MOVE, 0, 0);
            trackSwarm = 0;
            if (swarmPresent && !isMother) {
                for (beeIndex = 0; beeIndex < beeCount; beeIndex++)
                    bees[beeIndex]->HandleMessage(this, MSG_BEES_HIVE_HIT, 0);
            }
            break;
    }
}
/* BYTES(slot-scope, inferred): the nested block(s) only order the frame: their locals are allocated after the enclosing scope's */
/* BYTES(dead-code): f is computed and never read, as in the original */
void Hive::AimCameraAt(Vec3s target)
{
    s32 c = target.x - (&camera->eye)->x;
    s32 b = target.y - (&camera->eye)->y;
    s32 a = target.z - (&camera->eye)->z;
    {
        s32 g = c * c;
        s32 f = b * b;
        s32 e = a * a;
        {
            Vec3s d;
            d.x = Math_RadiansToAngle4096((float)atan2(b, (s32)sqrt((double)g + e))) & 0xfff;
            d.y = Math_RadiansToAngle4096((float)atan2(-c, a)) & 0xfff;
            StartCamera(d.x, d.y, 0, &camera->eye, camera->focal, 0);
        }
    }
}
ScnObject *Hive_Create(void *record)
{
    Hive *object = new Hive;
    object = (Hive *)object->Init(record, 0); /* cast kept: Init returns the object as its ScnBody base */
    return object;
}
