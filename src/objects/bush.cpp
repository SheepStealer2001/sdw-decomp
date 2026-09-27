/* PAL PC Bush, 0x49e9d0-0x49f1f7. */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *, Animator *, u16, u32);

#define SDW_MEMBERS_ScnObject       \
    static void *operator new(u32); \
    void SetRotation(Vec3s *value);

#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_ISINWORLD 1
#define SDW_INLINE_SCNOBJECT_SETVISIBLE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_ISINWORLD
#undef SDW_INLINE_SCNOBJECT_SETVISIBLE_S32
#define SDW_INLINE_SCNOBJECT_SETROTATION_VEC3S 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETROTATION_VEC3S
#define SDW_INLINE_SCNBODY_ANIMID 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMID
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
extern Wolf *g_pWolf;
#include "../engine/maths.h"
#include "../engine/scn_tools.h"
extern u32 g_gameTime;
extern s32 g_dt, g_dtMs;
void Bush::Update()
{
    s32 isNear;
    if (bushFlags.liftLock) {
        liftLockMs += g_dtMs;
        if (liftLockMs > 1000)
            bushFlags.liftLock = 0;
    }
    switch (state) {
        case BUSH_ST_IDLE:
            isNear = Vec3s_DistSq(&pos, &g_pWolf->pos) <= 2500;
            if (bushFlags.fanBlown || isNear != bushFlags.wolfNear) {
                if (AnimId() != ABCOYO02_ANIM_OUT6) {
                    PlayAnim(ABCOYO02_ANIM_OUT6, 1, 1);
                    anim.speed = 0x6000;
                }
            } else if (AnimId() != ABCOYO02_ANIM_STAND1 && AnimFlags(ANIM_F_FINISHED)) {
                PlayAnim(ABCOYO02_ANIM_STAND1, 0, 1);
                anim.speed = 0x1000;
            }
            bushFlags.wolfNear = isNear;
            break;
        case BUSH_ST_RESPAWN_HIDDEN:
            if (g_gameTime - stateTime >= 0x3000) {
                SetVisible(1);
                state = BUSH_ST_RESPAWN_WOBBLE;
                stateTime = g_gameTime;
            }
            break;
        case BUSH_ST_RESPAWN_WOBBLE:
            if (IsRespawnWobbleDone(g_gameTime - stateTime)) {
                state = BUSH_ST_IDLE;
                stateTime = g_gameTime;
            }
            break;
    }
    bushFlags.fanBlown = 0;
    AdvanceAnim();
}
void Bush::StartRespawn()
{
    SetVisible(0);
    SetPosition(&homePos);
    SetRotation(g_pZeroVec3s);
    state = BUSH_ST_RESPAWN_HIDDEN;
    stateTime = g_gameTime;
    eatenTime = 0;
}
s32 Bush::HandleMessage(ScnObject *sender, u32 message, void *arg)
{
    s32 restore;
    switch (message) {
        case MSG_LIFT_CRUSH:
            bushFlags.liftLock = 1;
            liftLockMs = 0;
            break;
        case MSG_QUERY_ACTION:
            if (sender->GetClassId() == CLASSID_WOLF)
                return CTX_BUSH;
            if (sender->GetClassId() == CLASSID_SHEEP && state == BUSH_ST_IDLE && g_gameTime - stateTime > 0x3000) {
                if (bushFlags.liftLock)
                    return CTX_NONE;
                return SHEEP_ATTR_BUSH;
            }
            return CTX_NONE;
        case MSG_USE:
            if (sender->GetClassId() == CLASSID_SHEEP && state == BUSH_ST_IDLE) {
                eatenTime += g_dt;
                if (eatenTime >= 0x14000)
                    StartRespawn();
                return 1;
            }
            return 0;
        case MSG_LANDED:
            StartRespawn();
            return 1;
        case MSG_KILL:
            switch ((s32)arg) { /* cast kept: MSG_KILL's arg is the WolfKillType, a number in the void * */
                case KILL_GENERIC:
                case KILL_CRUSH:
                    StartRespawn();
                    return 1;
            }
            break;
        case MSG_BUSH_ENTER_BEGIN:
            PlayAnim(ABCOYO02_ANIM_JUMP6, 0, 1);
            anim.speed = 0x1000;
            return 1;
        case MSG_BUSH_WORN:
            RemoveFromWorld();
            state = BUSH_ST_WORN;
            stateTime = g_gameTime;
            return 1;
        case MSG_BUSH_TAKEN_OFF:
            /* cast kept: MSG_BUSH_TAKEN_OFF's arg is a flag (0: respawn at home), a number in the void * */
            restore = (s32)arg;
            AddToWorld(0);
            if (restore == 0)
                StartRespawn();
            else {
                SetPosition(&sender->pos);
                rot = sender->rot;
                state = BUSH_ST_IDLE;
                stateTime = g_gameTime;
            }
            PlayAnim(ABCOYO02_ANIM_OUT6, 0, 1);
            anim.speed = 0x1000;
            return 1;
        case MSG_FAN_BLOW:
            bushFlags.fanBlown = 1;
            return 1;
    }
    return 0;
}
void Bush::Render(Camera *view)
{
    if (state == BUSH_ST_RESPAWN_WOBBLE)
        RenderRespawnWobble(view, g_gameTime - stateTime);
    else
        ScnBody::Render(view);
}
void Bush::Reset()
{
    PlayAnim(ABCOYO02_ANIM_STAND1, 0, 1);
    anim.speed = 0x1000;
    state = BUSH_ST_IDLE;
    stateTime = g_gameTime;
    eatenTime = 0;
    bushFlags.fanBlown = 0;
    bushFlags.liftLock = 0;
    bushFlags.wolfNear = 0;
    SetVisible(1);
    SetRotation(g_pZeroVec3s);
    if (IsInWorld())
        SetPosition(&homePos);
}
void Bush::PostLoadInit()
{
    homePos = pos;
    homePos.y = QueryGroundY(&homePos, 1);
    SetPosition(&homePos);
    PlayAnim(ABCOYO02_ANIM_STAND1, 0, 1);
    state = BUSH_ST_IDLE;
    stateTime = g_gameTime;
    eatenTime = 0;
    bushFlags.fanBlown = 0;
    bushFlags.liftLock = 0;
    bushFlags.wolfNear = 0;
}
ScnObject *Bush_Create(void *record)
{
    ScnBody *object = new Bush;
    object = object->Init(record, 0);
    return object;
}
