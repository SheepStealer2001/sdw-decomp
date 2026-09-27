/* T112 - original object Bullet.cpp (guessed name).
 * Ranges: .text 0x49e020-0x49e9c8, .rdata 0x575b54-0x575b78 (Bullet's vtable).
 *
 * Bullet (class 92, vtable 0x575b54, sizeof 0x90) - the shot Elmer Fudd fires in Level 9 (disc Lvl-11). There is one
 * Bullet object in the level and Elmer re-arms it: Elmer_SetState(3) aims it (msg 0x3203, the muzzle position) and
 * fires it (msg 0x3200), and asks it every frame whether it is still in flight (msg 0x3201).
 * SheepD3D.exe 0x49e020-0x49e9c7: PostLoadInit, Reset, Update, the flight step, HandleMessage, SetState and the
 * class factory.
 *
 * It homes: state 1 re-aims at Ralph's chest (his position lifted by 0x78) every frame and flies at 1000 units/s, so
 * it cannot be outrun, only outlived - Ralph dying (Wolf msg 0x40d) makes it vanish. Inside 0x6e units it switches to
 * state 2, which pins it to Ralph at 0x5a units on the bearing it struck from and keeps that bearing while he turns;
 * state 3 plays the last animation and tells Ralph MSG_KILL with cause 7, then tells Elmer msg 0x6f. If Ralph survives
 * that message the bullet goes dormant instead (state 5), which is also what Reset leaves it in.
 * Devices that only pin the original code generation: the inline helpers below (their names are not recovered; each
 * one is here because its expansion gives the original's stack temporaries) and the local names - under /Od a local's
 * slot follows from a hash of its name (tools/vc6_locals.py), which is why the two identical position locals of Update
 * are called restPos and stuckPos and the flight step's are move / velocity / dir.
 *
 * PostLoadInit: the original spends one more scratch register in the first statement than a bare discarded call does,
 * without emitting a byte for it (the second lookup loads `this` into edx, not ecx). A one-pass loop does: the dead
 * `while(1)` back edge is register-allocated and then dropped, exactly like the unreachable pre-case statement in
 * InstantHoover::HandleMessage. The original spelling is unknown (a do/break/while(1) macro is one plausible source);
 * only the machine code is proven.
 */
/* BYTES: slot-name. */
/* g_sinTable4096, g_pCosTable and Math_RadiansToAngle4096 are declared extern "C", as they are defined, so the decorated names agree at link. */

#define SDW_MEMBERS_ScnObject            \
    static void *operator new(u32 size); \
    void SetFacing(s16 f);               \
    void SetUpdateMode(s32 mode);

#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *inst, Animator *animator, u16 id, u32 opts); /* 0x550196 */
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
#define SDW_INLINE_SCNOBJECT_SETVISIBLE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_ENABLEBOXCOLLIDE_S32
#undef SDW_INLINE_SCNOBJECT_SETVISIBLE_S32
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16

#include "../sdk/crt.h"
extern "C" s16
Math_RadiansToAngle4096(float radians); /* 0x5269ce; C linkage like its definition (src/engine/fixed_math.cpp) */
s32 Scenaric_FindByClass(u16 classId, ScnObject **out, s32 max); /* 0x5145c5 */
s32 Vec3s_DistXZ(Vec3s *a, Vec3s *b);                            /* 0x5157bd */
#include "../engine/scn_tools.h"

extern Wolf *g_pWolf;                  /* 0x6cf310 */
extern "C" const s16 g_sinTable4096[]; /* 0x57ece0  4.12 sine, 4096 steps per turn; C linkage like its definition */
extern "C" const s16 *g_pCosTable;     /* 0x5814e4  = g_sinTable4096 + 1024 */

/* 0x49e020 - vtable +0x00: find Elmer and the SwirlSign, no box collision, always updated, dormant (state 5). */
void Bullet::PostLoadInit()
{
    do {
        Scenaric_FindByClass(CLASSID_ELMER, &elmer, 1);
        break;
    } while (1);
    Scenaric_FindByClass(CLASSID_SWIRLSIGN, &swirlSign, 1);
    EnableBoxCollide(0);
    SetUpdateMode(SCN_UPD_ALWAYS);
    SetState(BULLET_ST_GONE);
}

/* 0x49e146 - vtable +0x14: back to dormant. */
void Bullet::Reset()
{
    SetState(BULLET_ST_GONE);
}

/* 0x49e15b - vtable +0x04: the three live states. */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py) */
void Bullet::Update()
{
    Vec3s stuckPos;
    Vec3s restPos;
    Vec3s wolfPos;
    if (state == BULLET_ST_GONE)
        return;
    wolfPos = g_pWolf->pos;
    switch (state) {
        case BULLET_ST_FLY:
            if (g_pWolf->HandleMessage(this, MSG_WOLF_IS_DEAD, 0)) {
                SetState(BULLET_ST_GONE);
                break;
            }
            dist = Vec3s_DistXZ(&wolfPos, &pos);
            if (dist <= 0x6e)
                SetState(BULLET_ST_HIT);
            else {
                wolfPos.y -= 0x78;
                AimAt(&wolfPos);
            }
            break;
        case BULLET_ST_HIT:
            if (g_pWolf->HandleMessage(this, MSG_WOLF_IS_DEAD, 0)) {
                SetState(BULLET_ST_GONE);
                break;
            }
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(BULLET_ST_ORBIT_KILL);
            else {
                stuckPos.x = wolfPos.x;
                stuckPos.y = wolfPos.y;
                stuckPos.z = wolfPos.z;
                stuckPos.y -= 0x78;
                stuckPos.x += (s16)(g_sinTable4096[(s16)((g_pWolf->Facing() + yawOffset) & 0xfff)] * 0x5a >> 12);
                stuckPos.z += (s16)(g_pCosTable[(s16)((g_pWolf->Facing() + yawOffset) & 0xfff)] * 0x5a >> 12);
                SetPosition(&stuckPos);
                stuckPos.x = wolfPos.x - pos.x;
                stuckPos.y = wolfPos.y - pos.y;
                stuckPos.z = wolfPos.z - pos.z;
                SetFacing((s16)(Math_RadiansToAngle4096((float)atan2((double)stuckPos.x, (double)stuckPos.z)) & 0xfff) +
                          0x800);
            }
            break;
        case BULLET_ST_ORBIT_KILL:
            restPos.x = wolfPos.x;
            restPos.y = wolfPos.y;
            restPos.z = wolfPos.z;
            restPos.y -= 0x78;
            restPos.x += (s16)(g_sinTable4096[(s16)((g_pWolf->Facing() + yawOffset) & 0xfff)] * 0x5a >> 12);
            restPos.z += (s16)(g_pCosTable[(s16)((g_pWolf->Facing() + yawOffset) & 0xfff)] * 0x5a >> 12);
            SetPosition(&restPos);
            restPos.x = wolfPos.x - pos.x;
            restPos.y = wolfPos.y - pos.y;
            restPos.z = wolfPos.z - pos.z;
            SetFacing((s16)(Math_RadiansToAngle4096((float)atan2((double)restPos.x, (double)restPos.z)) & 0xfff) +
                      0x800);
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(BULLET_ST_GONE);
            break;
    }
    AdvanceAnim();
}

/* 0x49e52e - one flight step toward target: 1000 units/s along the straight line, then face it. */
void Bullet::AimAt(Vec3s *target)
{
    Vec3s dir;
    Vec3s velocity;
    Vec3s move;
    dir.x = target->x - pos.x;
    dir.y = target->y - pos.y;
    dir.z = target->z - pos.z;
    velocity.x = (s16)(dir.x * 1000 / dist);
    velocity.y = (s16)(dir.y * 1000 / dist);
    velocity.z = (s16)(dir.z * 1000 / dist);
    Vec3s_ScaleByDt(&velocity, &move);
    Translate(&move);
    SetFacing((s16)(Math_RadiansToAngle4096((float)atan2((double)dir.x, (double)dir.z)) & 0xfff) + 0x800);
}

/* 0x49e634 - vtable +0x10: Elmer's four messages. 0x3200 fire, 0x3201 "are you busy?", 0x3202 stop, 0x3203 place. */
s32 Bullet::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    switch (msgId) {
        case MSG_BULLET_SET_POS:
            /* cast kept: HandleMessage's arg is a void *; this message carries the position */
            SetPosition((Vec3s *)arg);
            break;
        case MSG_BULLET_FIRE:
            SetState(BULLET_ST_FLY);
            break;
        case MSG_BULLET_IS_BUSY:
            return state != BULLET_ST_GONE;
        case MSG_BULLET_RESET:
            SetState(BULLET_ST_GONE);
            break;
        case MSG_BULLET_DONE:
            SetState(BULLET_ST_GONE);
            break;
    }
    return 0;
}

/* 0x49e6c6 - enter a state: 1 flying (anim 2, visible), 2 hit (anim 4, the bearing from Ralph recorded), 3 the last
 * animation plus MSG_KILL cause 7 to Ralph and msg 0x6f to Elmer, 5 dormant (hidden). */
void Bullet::SetState(u8 newState)
{
    Vec3s rel;
    s16 ang;
    state = newState;
    switch (newState) {
        case BULLET_ST_FLY:
            PlayAnim(ABALLE01_ANIM_RUN, 0, 0);
            SetVisible(1);
            break;
        case BULLET_ST_HIT:
            PlayAnim(ABALLE01_ANIM_ACTION, 0, 0);
            rel.x = pos.x - g_pWolf->pos.x;
            rel.y = pos.y - g_pWolf->pos.y;
            rel.z = pos.z - g_pWolf->pos.z;
            ang = Math_RadiansToAngle4096((float)atan2((double)rel.x, (double)rel.z)) & 0xfff;
            yawOffset = (s16)((ang - g_pWolf->Facing() + 0x800) & 0xfff) - 0x800;
            break;
        case BULLET_ST_ORBIT_KILL:
            PlayAnim(ABALLE01_ANIM_ACTION1, 0, 0);
            /* cast kept: HandleMessage's arg is a void *; MSG_KILL passes the kill type in it */
            if (!g_pWolf->HandleMessage(this, MSG_KILL, (void *)KILL_CANNONBALL))
                SetState(BULLET_ST_GONE);
            if (elmer)
                elmer->HandleMessage(this, MSG_BULLET_DONE, 0);
            break;
        case BULLET_ST_GONE:
            SetVisible(0);
            break;
    }
}

/* 0x49e958 - the class factory for CLASSID 92 "Bullet": new Bullet (the base vtables in turn, then Bullet's), then
 * ScnMobile_Init(record, 0) through vtable slot +0x20. */
ScnObject *Bullet_Create(void *record)
{
    Bullet *obj = new Bullet;
    obj = (Bullet *)obj->Init(record, 0); /* cast kept: Init returns this as a ScnBody * */
    return obj;
}
