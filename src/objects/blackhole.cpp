/* PAL PC BlackHole, 0x49ad50-0x49b620. MoveModifyArg and
 * BlackHoleFlagBits preserve the observed byte/word bitfield accesses. */
/* BYTES: view. */
/* BYTES(view): view: movemodifyarg and blackholeflagbits preserve the observed byte/word bitfield accesses (the access widths are the original's) */
#include "sdw_types.h"
#include "sdw_enums.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *, Animator *, u16, u32);

#define SDW_MEMBERS_ScnObject       \
    static void *operator new(u32); \
    s16 BoxBottom();                \
    s16 BoxTop();                   \
    void SetUpdateMode(u8 mode);

#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8
#define SDW_INLINE_SCNBODY_GETANIMID 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_GETANIMID
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
extern Wolf *g_pWolf;
extern s32 g_dt;
#include "../engine/scn_tools.h"
#include "../engine/fixed_math.h"
s32 Vec3s_Dist(Vec3s *, Vec3s *);
#include "../sdk/crt.h"
inline s16 ScnObject::BoxBottom()
{
    CollBox *box = GetFirstSolidBox();
    if (!box)
        return 0;
    return box->max.y;
}
inline s16 ScnObject::BoxTop()
{
    CollBox *box = GetFirstSolidBox();
    if (!box)
        return 0;
    return box->min.y;
}
void BlackHole::Update()
{
    Vec3s center;
    s32 distance;
    switch (state) {
        case BLACKHOLE_ST_ACTIVE:
            center = g_pWolf->pos;
            center.y += (s16)((g_pWolf->BoxBottom() + g_pWolf->BoxTop()) >> 1);
            distance = Vec3s_DistSq(&pos, &center);
            if (distance <= 1000000) {
                if (!flags.registered && g_pWolf->HandleMessage(this, MSG_RIDER_ADD, 0))
                    flags.registered = 1;
                if (distance <= 2500) {
                    /* cast kept: arg carries a number */
                    g_pWolf->HandleMessage(this, MSG_KILL, (void *)KILL_BLACKHOLE);
                    PlayAnim(ATROUN01_ANIM_HOLE, 0, 1);
                    state = BLACKHOLE_ST_SPENT;
                }
                /* cast kept: arg carries a number */
                g_pWolf->HandleMessage(this, MSG_WOLF_GLOW_DIST, (void *)(s32)sqrt((double)distance));
                SetUpdateMode(SCN_UPD_ALWAYS);
            } else {
                if (flags.registered && g_pWolf->HandleMessage(this, MSG_RIDER_REMOVE, 0))
                    flags.registered = 0;
                SetUpdateMode(SCN_UPD_NORMAL);
            }
            break;
        case BLACKHOLE_ST_SPENT:
            if (GetAnimId() == ATROUN01_ANIM_HOLE && AnimFlags(ANIM_F_FINISHED))
                PlayAnim(ATROUN01_ANIM_STAND, 1, 1);
            g_pWolf->HandleMessage(this, MSG_WOLF_GLOW_DIST, 0);
            break;
    }
    AdvanceAnim();
}
void BlackHole::ApplyPull(MoveModifyArg *move, ScnObject *object)
{
    Vec3s sourcePos;
    s32 strength, range, closeness;
    Vec3s direction, nextPos, pullVelocity;
    sourcePos = object->pos;
    sourcePos.y += (s16)((object->BoxBottom() + object->BoxTop()) >> 1);
    nextPos.x = sourcePos.x + move->delta.x;
    nextPos.y = sourcePos.y + move->delta.y;
    nextPos.z = sourcePos.z + move->delta.z;
    range = Vec3s_Dist(&nextPos, &pos);
    direction.x = pos.x - nextPos.x;
    direction.y = pos.y - nextPos.y;
    direction.z = pos.z - nextPos.z;
    Vec3s_Normalize(&direction, &direction);
    if (range > 1000)
        strength = 0;
    else {
        if (range < 100)
            closeness = 900;
        else
            closeness = 900 - (range - 100);
        strength = closeness * closeness * 250 / 810000;
    }
    pullVelocity.x = strength * direction.x / 4096;
    pullVelocity.y = strength * direction.y / 4096;
    pullVelocity.z = strength * direction.z / 4096;
    move->velocity.x += pullVelocity.x;
    move->velocity.y += pullVelocity.y;
    move->velocity.z += pullVelocity.z;
    closeness = strength * g_dt / 4096;
    if (closeness >= range) {
        move->delta.x = pos.x - sourcePos.x;
        move->delta.y = pos.y - sourcePos.y;
        move->delta.z = pos.z - sourcePos.z;
    } else
        Vec3s_ScaleByDt(&move->velocity, &move->delta);
}
s32 BlackHole::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    if (msgId == MSG_MODIFY_MOVE) {
        MoveModifyArg *move = (MoveModifyArg *)arg; /* cast kept: MSG_MODIFY_MOVE's arg is a MoveModifyArg */
        if (!move->noPull)
            ApplyPull(move, sender);
        return 1;
    }
    return 0;
}
void BlackHole::Render(Camera *view)
{
    RenderFacingCamera(view, 0, 0, 0);
}
void BlackHole::Reset()
{
    if (flags.registered && g_pWolf->HandleMessage(this, MSG_RIDER_REMOVE, 0))
        flags.registered = 0;
    PlayAnim(ATROUN01_ANIM_STAND, 1, 0);
    state = BLACKHOLE_ST_ACTIVE;
    SetUpdateMode(SCN_UPD_NORMAL);
}
void BlackHole::PostLoadInit()
{
    flags.registered = 0;
    PlayAnim(ATROUN01_ANIM_STAND, 1, 0);
    state = BLACKHOLE_ST_ACTIVE;
}
ScnObject *BlackHole_Create(void *record)
{
    ScnBody *object = new BlackHole;
    object = object->Init(record, 0);
    return object;
}
