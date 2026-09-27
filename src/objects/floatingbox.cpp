/*
 * T150 - original object FloatingBox.cpp (guessed name), one translation unit.
 *   .text  0x4bf6b0-0x4c1eb1 (FloatingBox_Render .. FloatingBox_BuildRecord, in address order)
 *          + COMDAT 0x4c1ec0-0x4c1ee9 ScnObject::GetFirstModelBox (the out-of-line copy of an inline accessor)
 *   .rdata 0x576168-0x57618c (??_7FloatingBox)
 * INLINE BUDGET (see src/objects/dragon.cpp for the rules): in the exe GetFirstModelBox is inlined at every use (Update,
 * SetState, HandleMessage case 0xd) except five of the six uses in HandleMessage case 0x14, which call the COMDAT
 * (0x4c1031-0x4c1089) after one inlined copy (0x4c0ffc). Direct call sites of a caller are decided first against a
 * large budget; call sites exposed inside an expanded inline are decided afterwards against what is left. So case 0x14
 * reads the box through the inline helper FloatingBox::LoadWorldBox (expanded, no frame slot of its own), whose six
 * nested GetFirstModelBox sites are second-round sites: with the accessor's estimate raised, only the first still fits.
 * Every other use is a direct site and is expanded. The devices below change no generated instruction (every function
 * byte-matches); they only change the inliner's estimates. They were found by search and are representations, not the
 * original text:
 *   - ScnObject::GetFirstModelBox is `inline` (so it is a COMDAT) and its body is wrapped in 24 empty blocks
 *     (22 and up all match, tested to 64; with 8..21 blocks two nested copies inline, with none three; without
 *     the helper every direct site inlines whatever the block count, tested to 20);
 *   - FloatingBox::LoadWorldBox, an inline helper holding case 0x14's six worldBox assignments (verbatim);
 *   - every use calls GetFirstModelBox, as the exe's single COMDAT implies.
 */
/* BYTES: dead-code, inline, slot-group, slot-scope. */
/* PAL PC FloatingBox, 0x4bf6b0-0x4c1eb1, plus GetFirstModelBox 0x4c1ec0.
 * The fields are in data/structs/FloatingBox.csv; each names only accesses visible in the original instructions. */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
#include "../engine/scn_tools.h"
#include "../engine/scenaric.h"
#include "../engine/maths.h"
#include "../engine/id_list.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *instance, Animator *animator, u16 id, u32 options);

#define SDW_MEMBERS_ScnObject                                                            \
    static void *operator new(u32 size);                                                 \
    void AttachTo(ScnObject *parent, u8 joint, Vec3s *offset, Vec3s *rot, u32 a, u32 b); \
    CollBox *GetFirstModelBox();                                                         \
    void SetTint(u32 value)                                                              \
    {                                                                                    \
        partHeight = (u8)(value >> 4);                                                   \
    }                                                                                    \
    void SetUpdateMode(s32 mode);


#define SDW_MEMBERS_InlineEmitter4 InlineEmitter4();

#define SDW_MEMBERS_FloatingBox void LoadWorldBox();
#define SDW_MEMBERS_CollBox s32 ContainsXZ(const Vec3s *point);

#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_GETHEADING 1
#define SDW_INLINE_SCNOBJECT_ISINWORLD 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_GETHEADING
#undef SDW_INLINE_SCNOBJECT_ISINWORLD
#define SDW_INLINE_SCNOBJECT_ENABLEBOXCOLLIDE_S32 1
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_ENABLEBOXCOLLIDE_S32
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_GETANIMID 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_GETANIMID
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_COLLBOX_CONTAINSXZ_CONST_VEC3S 1
#include "../engine/coll_box_inlines.h"
#undef SDW_INLINE_COLLBOX_CONTAINSXZ_CONST_VEC3S
#define SDW_INLINE_ZONELIST_FINDCONTAINING_VEC3S 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_FINDCONTAINING_VEC3S
inline InlineEmitter4::InlineEmitter4()
{
    base.slotPool = slotBuf;
    base.particles = particleBuf;
    base.count = 4;
    base.Emitter_Reset();
}
#define SDW_INLINE_INLINEEMITTER4_RENDERFLAT_CAMERA_S32 1
#include "../engine/emitter4_inlines.h"
#undef SDW_INLINE_INLINEEMITTER4_RENDERFLAT_CAMERA_S32
#define SDW_INLINE_SCNOBJECT_BROADCASTAROUND_S32_S32_U16_U32_VOID 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_BROADCASTAROUND_S32_S32_U16_U32_VOID
/* Supported object offsets: state handler 4c17c1, update 4bfb44,
   post-load 4bf872 and reset 4bfaa3. Names are not recovered. */
extern s32 g_dtMs;
extern Wolf *g_pWolf;
extern "C" const s16 g_sinTable4096[];
extern "C" const s16 *g_pCosTable;
#include "../sdk/crt.h"
#define SDW_INLINE_FREE_ZONES_GET_U8 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_FREE_ZONES_GET_U8
s32 ObjGrid_QueryBoxOverlap(CollBox *box, ScnObject **out);
void Box_Translate(CollBox *box, const Vec3s *point);
#define ABS_VALUE(a) ((a) >= 0 ? (a) : -(a))
#define SDW_INLINE_FREE_SETPROP_VOID_U32_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_SETPROP_VOID_U32_U32

#define SDW_INLINE_FREE_PROPU32_VOID_U32 2
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_PROPU32_VOID_U32

/* Case 0x14's six worldBox assignments, moved here unchanged: a device (see the INLINE BUDGET note above). */
/* BYTES(inline): source-only inline: moving the six assignments here makes their GetFirstModelBox calls second-round sites, as in the original */
inline void FloatingBox::LoadWorldBox()
{
    worldBox.min.x = GetFirstModelBox()->min.x;
    worldBox.min.y = GetFirstModelBox()->min.y;
    worldBox.min.z = GetFirstModelBox()->min.z;
    worldBox.max.x = GetFirstModelBox()->max.x;
    worldBox.max.y = GetFirstModelBox()->max.y;
    worldBox.max.z = GetFirstModelBox()->max.z;
}

void FloatingBox::Render(Camera *view)
{
    ScnBody::Render(view);
    switch (state) {
        case FBOX_ST_PUSHED:
            if (splashEmitter.base.flags.active)
                splashEmitter.RenderFlat(view, 1);
            break;
        case FBOX_ST_FLOATING:
            if (splashEmitter.base.flags.active) {
                splashTime += g_dtMs;
                if (splashTime > 1000) {
                    splashTime = 0;
                    splashEmitter.base.Emitter_Reset();
                    boxFlags &= ~FBOX_F_SPLASH_STARTED;
                } else
                    splashEmitter.base.Emitter_Render(view, 0);
            }
            break;
    }
}

void FloatingBox::ReleaseContents()
{
    Vec3s point;
    content->AddToWorld(0);
    point = pos;
    point.y -= 30;
    content->SetPosition(&point);
    /* cast kept (both): HandleMessage's arg is a void *; these messages pass a number in it */
    content->HandleMessage(this, MSG_CONTAINER_STATE, (void *)CONTAINER_RELEASED);
    BroadcastAround(2000, 2000, 2000, MSG_LOUD_NOISE, (void *)GetClassId());
}

void FloatingBox::PostLoadInit()
{
    waterBox = 0;
    floatOffset.x = 0;
    floatOffset.y = 30;
    floatOffset.z = 0;
    splashEmitter.base.Emitter_Reset();
    splashParams.life = 0x1000;
    splashParams.fadeStart = 0;
    splashParams.spawnInterval = splashParams.life / 4;
    splashParams.sizeStart = 100;
    splashParams.sheetIndex = 1;
    splashParams.sizeEnd = splashParams.sizeStart << 1;
    splashTime = 0;
    pushSpeed = 0;
    pushAngle = 0;
    boxFlags = 0;
    SetTint(0);
    restoreUnset = 1;
    restoreState = FBOX_ST_OPENED;
    restorePos.x = 0;
    restorePos.y = 0;
    restorePos.z = 0;
    SwapModel(&mainModel);
    PlayAnim(ACAISS02_ANIM_ACTION, 1, 0);
    SetUpdateMode(SCN_UPD_ALWAYS);
}

void FloatingBox::Reset()
{
    boxFlags &= ~FBOX_F_RUNTIME_MASK;
    boxFlags &= ~FBOX_F_MOTION_MASK;
    if (!restoreUnset && IsInWorld() && !content->IsInWorld()) {
        rot.y = 0;
        SetPosition(&restorePos);
        SetState(restoreState, 1);
    }
}

/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; unused4a, unused42, unused36, unused2e, unused2c, unused1e, unused16, unused0a, unused02 fill gaps */
void FloatingBox::Update()
{
    /* Stack layout follows the original EBP-58 through EBP-1 region. Padding
       bytes are unaccessed in the binary and remain uninitialized here. */
    struct UpdateWork {
        s32 index, carried;
        Vec3s direction;
        u16 unused4a;
        Vec3s push;
        u16 unused42;
        s32 length;
        Vec3s correction1;
        u16 unused36;
        Vec3s correction0;
        u16 unused2e;
        u16 unused2c;
        s16 randomZ, randomX;
        u16 angle;
        Vec3s probe1;
        u16 unused1e;
        Vec3s probe3;
        u16 unused16;
        Box *zone;
        Vec3s flow;
        u16 unused0a;
        Vec3s next;
        u16 unused02;
    } w;
    w.zone = 0;
    w.next.x = 0;
    w.next.y = 0;
    w.next.z = 0;
    w.flow.x = 0;
    w.flow.y = 0;
    w.flow.z = 0;
    if (g_pWolf->HandleMessage(this, MSG_WOLF_IS_DEAD, 0))
        boxFlags |= FBOX_F_WOLF_DEAD;
    switch (state) {
        case FBOX_ST_SINKING:
            if (boxFlags & FBOX_F_WOLF_DEAD)
                break;
            w.probe3.x = pos.x;
            w.probe3.y = pos.y;
            w.probe3.z = pos.z;
            w.probe3.y -= 90;
            w.zone = Zones_Get(ZONE_WATER)->FindContaining(&w.probe3);
            if (w.zone && !g_pWolf->HandleMessage(this, MSG_WOLF_IS_GROUNDED, 0))
                g_pWolf->HandleMessage(this, MSG_WOLF_STRIP, 0);
            break;
        case FBOX_ST_ON_GROUND:
            if (boxFlags & FBOX_F_BOUNCE) {
                bounceTime += g_dtMs;
                w.angle = (u16)((u32)(bounceTime << 11) / 800u);
                bounceVelocity.y = (g_sinTable4096[w.angle] * -400) / 4096;
                velocity.x = bounceVelocity.x;
                velocity.y = bounceVelocity.y;
                velocity.z = bounceVelocity.z;
                if (w.angle >= 2048) {
                    boxFlags &= ~FBOX_F_BOUNCE;
                    rot = savedRotation;
                } else if (w.angle > 1024 && hitClass != CLASSID_WOLF) {
                    bounceRotation.x += (s16)((bounceVelocity.x >= 0 ? 1 : -1) * (s16)(((g_dtMs << 10) / 400) & 0xfff));
                    bounceRotation.z += (s16)((bounceVelocity.z >= 0 ? 1 : -1) * (s16)(((g_dtMs << 10) / 400) & 0xfff));
                    rot = bounceRotation;
                }
            } else {
                velocity.x = 0;
                velocity.y = 800;
                velocity.z = 0;
                Vec3s_ScaleByDt(&velocity, &frameMove);
            }
            Vec3s_ScaleByDt(&velocity, &frameMove);
            lastMoveResult = Collide_ResolveMove(&frameMove, &contact, 0xb54, RESOLVE_SLIDE_ALL, 0, 0, 10, 0, 0);
            Translate(&frameMove);
            if (lastMoveResult && contact.floorObj) {
                hitClass = contact.floorObj->GetClassId();
                switch (hitClass) {
                    case CLASSID_WOLF:
                        contact.floorObj->HandleMessage(this, MSG_WOLF_DAZE, 0);
                    case CLASSID_CROCODILELEVEL09:
                        boxFlags |= FBOX_F_BOUNCE;
                        bounceTime = 0;
                        w.randomX = Rand_Range(-1, 1);
                        if (!w.randomX)
                            w.randomX = 1;
                        w.randomZ = Rand_Range(-1, 1);
                        if (!w.randomZ)
                            w.randomZ = 1;
                        bounceVelocity.x = w.randomX * 200;
                        bounceVelocity.y = 0;
                        bounceVelocity.z = w.randomZ * 200;
                        break;
                    default:
                        SetState(FBOX_ST_FLOATING, 1);
                        break;
                }
            } else if (lastMoveResult & 1)
                SetState(FBOX_ST_FLOATING, 1);
            else {
                w.zone = Zones_Get(ZONE_WATER)->FindContaining(&pos);
                if (w.zone && pos.y >= w.zone->min[1] && ABS_VALUE(w.zone->min[1] - pos.y) >= floatOffset.y) {
                    waterBox = w.zone;
                    w.correction0.x = 0;
                    w.correction0.y = w.zone->min[1] - pos.y;
                    w.correction0.z = 0;
                    w.correction0.x += floatOffset.x;
                    w.correction0.y += floatOffset.y;
                    w.correction0.z += floatOffset.z;
                    Translate(&w.correction0);
                    SetState(FBOX_ST_PUSHED, 1);
                }
            }
            break;
        case FBOX_ST_FLOATING:
            w.probe1.x = pos.x;
            w.probe1.y = pos.y - 50;
            w.probe1.z = pos.z;
            w.zone = Zones_Get(ZONE_WATER)->FindContaining(&w.probe1);
            if (w.zone) {
                w.correction1.x = 0;
                w.correction1.y = w.zone->min[1] - pos.y;
                w.correction1.z = 0;
                w.correction1.x += floatOffset.x;
                w.correction1.y += floatOffset.y;
                w.correction1.z += floatOffset.z;
                Translate(&w.correction1);
                SetState(FBOX_ST_PUSHED, 1);
            } else if (boxFlags & FBOX_F_THAW) {
                ReleaseContents();
                PlayAnim(ACAISS02_ANIM_STAND, 1, 0);
                SetState(FBOX_ST_OPENING, 1);
                break;
            } else {
                frameMove.x = 0;
                frameMove.y = 5;
                frameMove.z = 0;
                lastMoveResult = Collide_ResolveMove(&frameMove, &contact, 0xb54, RESOLVE_SLIDE_ALL, 0, 0, 10, 0, 0);
                if (!lastMoveResult)
                    SetState(FBOX_ST_ON_GROUND, 0);
                if (boxFlags & FBOX_F_SPLASH_STARTED) {
                    splashEmitter.base.Emitter_UpdateFade(&splashParams, &pos, 0, boxFlags & FBOX_F_SPLASH);
                    boxFlags &= ~FBOX_F_SPLASH;
                }
            }
            break;
        case FBOX_ST_PUSHED:
            emitPos.x = pos.x;
            emitPos.y = pos.y;
            emitPos.z = pos.z;
            emitPos.x -= floatOffset.x;
            emitPos.y -= floatOffset.y;
            emitPos.z -= floatOffset.z;
            splashEmitter.base.Emitter_UpdateFade(&splashParams, &emitPos, (pushAngle + 0x800) & 0xfff, pushSpeed);
            velocity.x = 0;
            velocity.y = 0;
            velocity.z = 0;
            w.next.x = pos.x + frameMove.x;
            w.next.y = pos.y + frameMove.y;
            w.next.z = pos.z + frameMove.z;
            w.zone = Zones_Get(ZONE_WATER)->FindContaining(&w.next);
            if (w.zone) {
                waterBox = w.zone;
                Zone_GetFlowVelocity(w.zone, &w.flow);
                velocity.x += w.flow.x;
                velocity.y += w.flow.y;
                velocity.z += w.flow.z;
            } else
                SetState(FBOX_ST_FLOATING, 0);
            if (state != FBOX_ST_PUSHED) {
                SetState(FBOX_ST_FLOATING, 0);
                break;
            }
            if (boxFlags & (FBOX_F_OPENED | FBOX_F_WOLF_DEAD)) {
                if (GetAnimId() != ACAISS02_ANIM_STAND)
                    PlayAnim(ACAISS02_ANIM_STAND, 0, 0);
                break;
            }
            if (pushSpeed) {
                if (AnimFlags(ANIM_F_FINISHED))
                    PlayAnim(ACAISS02_ANIM_ACTION, 0, 0);
                if (!(boxFlags & FBOX_F_PUSHED) && pushSpeed) {
                    pushSpeed -= 35;
                    if (pushSpeed < 0)
                        pushSpeed = 0;
                }
                w.direction.x = g_sinTable4096[pushAngle];
                w.direction.y = 0;
                w.direction.z = g_pCosTable[pushAngle];
                w.length = (s32)sqrt((double)w.direction.x * (double)w.direction.x + w.direction.z * w.direction.z);
                w.push.x = (w.direction.x * pushSpeed) / (s16)w.length;
                w.push.y = 0;
                w.push.z = (w.direction.z * pushSpeed) / (s16)w.length;
                velocity.x += w.push.x;
                velocity.y += w.push.y;
                velocity.z += w.push.z;
                Vec3s_ScaleByDt(&velocity, &frameMove);
                if (!g_pWolf->Collide_ResolveMove(&frameMove, &contact, 0x578, RESOLVE_SLIDE_ALL, 0, 0, 10, 0, 0))
                    Collide_ResolveMove(&frameMove, 0, 0xb54, COLL_WALL | RESOLVE_KEEP_Y, 0, 0, 10, 0, 0);
                frameMove.y = 0;
                worldBox.min.x = GetFirstModelBox()->min.x;
                worldBox.min.y = GetFirstModelBox()->min.y;
                worldBox.min.z = GetFirstModelBox()->min.z;
                worldBox.max.x = GetFirstModelBox()->max.x;
                worldBox.max.y = GetFirstModelBox()->max.y;
                worldBox.max.z = GetFirstModelBox()->max.z;
                worldBox.Box_Translate(&worldBox, &pos);
                worldBox.min.y -= 20;
                overlapCount = ObjGrid_QueryBoxOverlap(&worldBox, overlapList);
                if (overlapCount) {
                    w.carried = 0;
                    for (w.index = 0; w.index < overlapCount; w.index++) {
                        if (overlapList[w.index]->GetClassId() == CLASSID_WOLF) {
                            overlapList[w.index]->Translate(&frameMove);
                            w.carried = 1;
                        }
                    }
                    if (!w.carried && (w.flow.x | w.flow.y | w.flow.z) == 0) {
                        pushSpeed = 0;
                        frameMove.x = 0;
                        frameMove.y = 0;
                        frameMove.z = 0;
                    }
                }
                Translate(&frameMove);
            } else if (AnimFlags(ANIM_F_FINISHED))
                PlayAnim(ACAISS02_ANIM_STAND, 0, 0);
            break;
        case FBOX_ST_OPENING:
            if (AnimFlags(ANIM_F_FINISHED)) {
                /* cast kept: HandleMessage's arg is a void *; this message passes a number in it */
                content->HandleMessage(this, MSG_CONTAINER_STATE, (void *)CONTAINER_RELEASED);
                SetState(FBOX_ST_OPENED, 1);
            }
            break;
    }
    boxFlags &= ~FBOX_F_PUSHED;
    AdvanceAnim();
}

/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; unusedTop fill gaps */
s32 FloatingBox::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    /* One observed frame region; these names do not imply an original struct. */
    u8 joint;
    struct MessageWork {
        ScnObject *owner;
        s32 index;
        GroundQuery *ground;
        DropMsgArg *drop;
        Vec3s delta;
        u16 unusedTop;
    } w;
    if (sender)
        switch (msgId) {
            case MSG_FREEZE:
                if (sender->GetClassId() == CLASSID_WOLF &&
                    ((state == FBOX_ST_SINKING && g_pWolf->HandleMessage(this, MSG_WOLF_STRIP, 0)) ||
                     state != FBOX_ST_SINKING)) {
                    boxFlags |= FBOX_F_OPENED;
                    return 1;
                }
                return 0;
            case MSG_QUERY_ACTION:
                if (sender->GetClassId() == CLASSID_WOLF) {
                    switch (state) {
                        case FBOX_ST_FLOATING:
                            return CTX_LIFT;
                        case FBOX_ST_PUSHED:
                            if (!waterBox)
                                return CTX_NONE;
                            if (ABS_VALUE(waterBox->min[1] - g_pWolf->pos.y) < 90)
                                return CTX_LIFT;
                            return CTX_NONE;
                    }
                    return CTX_NONE;
                }
                return CTX_NONE;
            case MSG_QUERY_HELD_ACTION:
                return HELD_THROWABLE;
            case MSG_USE:
                SetState(FBOX_ST_ON_GROUND, 1);
                break;
            case MSG_FAN_BLOW:
                if (state == FBOX_ST_PUSHED) {
                    LoadWorldBox();
                    worldBox.Box_Translate(&worldBox, &pos);
                    worldBox.min.y -= 20;
                    overlapCount = ObjGrid_QueryBoxOverlap(&worldBox, overlapList);
                    for (w.index = 0; w.index < overlapCount; w.index++) {
                        if (overlapList[w.index] == g_pWolf) {
                            pushSpeed += 35;
                            if (pushSpeed > 600)
                                pushSpeed = 600;
                            pushAngle = (s16)(u32)arg; /* cast kept: this message passes the angle in its void * */
                            boxFlags |= FBOX_F_PUSHED;
                            return 1;
                        }
                    }
                    return 0;
                }
                break;
            case MSG_KILL:
                switch (sender->GetClassId()) {
                    case CLASSID_DYNAMITE:
                        if (state == FBOX_ST_FLOATING)
                            boxFlags |= FBOX_F_THAW;
                        break;
                    case CLASSID_SMALLROCK:
                        if (state == FBOX_ST_FLOATING)
                            boxFlags |= FBOX_F_THAW;
                        break;
                    case CLASSID_GROUNDMINE:
                    case CLASSID_CANNONBALL:
                        if (state == FBOX_ST_SINKING)
                            g_pWolf->HandleMessage(this, MSG_WOLF_STRIP, 0);
                        break;
                }
                break;
            case MSG_PICKUP:
                if ((sender->GetClassId() == CLASSID_WOLF && state == FBOX_ST_FLOATING) ||
                    ABS_VALUE(waterBox->min[1] - g_pWolf->pos.y) <= 90) {
                    if (state != FBOX_ST_SINKING)
                        SetState(FBOX_ST_SINKING, 1);
                    SwapModel(&mainModel);
                    PlayAnim(ACAISS02_ANIM_ACTION, 0, 0);
                    EnableBoxCollide(0);
                    w.owner = sender;
                    joint = (u8)(u32)arg; /* cast kept: this message passes the joint number in its void * */
                    AttachTo(w.owner, joint, 0, 0, 1, 0);
                    return 1;
                }
                break;
            case MSG_DROP:
                w.drop = (DropMsgArg *)arg; /* cast kept: the message arg is a void *; MSG_DROP passes a DropMsgArg */
                Detach();
                EnableBoxCollide(1);
                rot.y = sender->GetHeading() & 0xfff;
                if (!w.drop->flag1) {
                    w.delta.x = w.drop->pos.x - pos.x;
                    w.delta.y = w.drop->pos.y - pos.y;
                    w.delta.z = w.drop->pos.z - pos.z;
                    Collide_ResolveMove(&w.delta, &contact, 0xb54, RESOLVE_SLIDE_ALL, 0, 0, 10, 0, 0);
                    Translate(&w.delta);
                }
                SetState(FBOX_ST_ON_GROUND, 1);
                return 1;
            case MSG_GROUND_QUERY:
                w.ground = (GroundQuery *)arg; /* cast kept: the message arg is a void *; this one is a GroundQuery */
                worldBox.min.x = GetFirstModelBox()->min.x;
                worldBox.min.y = GetFirstModelBox()->min.y;
                worldBox.min.z = GetFirstModelBox()->min.z;
                worldBox.max.x = GetFirstModelBox()->max.x;
                worldBox.max.y = GetFirstModelBox()->max.y;
                worldBox.max.z = GetFirstModelBox()->max.z;
                worldBox.Box_Translate(&worldBox, &pos);
                if (worldBox.ContainsXZ(&w.ground->pos)) {
                    w.ground->pos.y = pos.y - 1 + GetFirstModelBox()->min.y;
                    w.ground->normal.y = -0x1000;
                    w.ground->normal.x = 0;
                    w.ground->normal.z = 0;
                    return 1;
                }
                return 0;
            case MSG_LANDED:
                boxFlags |= FBOX_F_THAW;
                SetState(FBOX_ST_ON_GROUND, 1);
                break;
        }
    return 0;
}

/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; unused fill gaps */
void FloatingBox::SetState(u8 value, s32 effect)
{
    state = value;
    if (restoreUnset) {
        switch (value) {
            case FBOX_ST_FLOATING:
            case FBOX_ST_PUSHED:
                restoreUnset = 0;
                restoreState = value;
                restorePos.x = pos.x;
                restorePos.y = pos.y;
                restorePos.z = pos.z;
                break;
        }
    }
    switch (value) {
        case FBOX_ST_ON_GROUND:
            if (effect)
                boxFlags |= FBOX_F_SPLASH;
            else
                boxFlags &= ~FBOX_F_SPLASH;
            savedRotation.x = rot.x;
            savedRotation.y = rot.y;
            savedRotation.z = rot.z;
            bounceRotation.x = rot.x;
            bounceRotation.y = rot.y;
            bounceRotation.z = rot.z;
            break;
        case FBOX_ST_FLOATING:
            boxFlags &= ~FBOX_F_BOUNCE;
            waterBox = 0;
            if (effect) {
                if (boxFlags & FBOX_F_SPLASH)
                    boxFlags |= FBOX_F_SPLASH_STARTED;
                splashParams.life = 0x1000;
                splashParams.fadeStart = 0;
                splashParams.spawnInterval = 0;
                splashParams.sizeStart = 100;
                splashParams.sheetIndex = 0;
                splashParams.sizeEnd = splashParams.sizeStart << 1;
                pushSpeed = 0;
                splashTime = 0;
            }
            SwapModel(&mainModel);
            PlayAnim(ACAISS02_ANIM_ACTION, 0, 0);
            break;
        case FBOX_ST_PUSHED: {
            /* Observed frame layout, EBP-24 through EBP-1; unused bytes unknown. */
            struct StateWork {
                s32 index;
                CollBox *solid;
                ScnObject *object;
                Vec3s point;
                u16 unused;
                CollBox box;
            } w;
            boxFlags &= ~(FBOX_F_THAW | FBOX_F_BOUNCE);
            SwapModel(&floatModel);
            PlayAnim(ACAISS02_ANIM_FALL, 1, 0);
            splashParams.life = 0x1000;
            splashParams.fadeStart = splashParams.life >> 1;
            splashParams.spawnInterval = 500;
            splashParams.sizeStart = 100;
            splashParams.sizeEnd = splashParams.sizeStart << 1;
            splashParams.sheetIndex = 1;
            w.box.min.x = GetFirstModelBox()->min.x + pos.x;
            w.box.min.y = GetFirstModelBox()->min.y + pos.y;
            w.box.min.z = GetFirstModelBox()->min.z + pos.z;
            w.box.max.x = GetFirstModelBox()->max.x + pos.x;
            w.box.max.y = GetFirstModelBox()->max.y + pos.y;
            w.box.max.z = GetFirstModelBox()->max.z + pos.z;
            overlapCount = ObjGrid_QueryBoxOverlap(&w.box, overlapList);
            if (overlapCount) {
                for (w.index = 0; w.index < overlapCount; w.index++) {
                    w.object = overlapList[w.index];
                    if (w.object != this) {
                        w.solid = GetFirstSolidBox();
                        if (w.solid) {
                            w.point.x = w.object->pos.x;
                            w.point.y = w.object->pos.y;
                            w.point.z = w.object->pos.z;
                            w.point.y -= (s16)((w.box.min.y - w.solid->max.y) + 100);
                            w.object->SetPosition(&w.point);
                        }
                    }
                }
            }
            break;
        }
        case FBOX_ST_OPENING:
            EnableBoxCollide(0);
            break;
        case FBOX_ST_OPENED:
            RemoveFromWorld();
            break;
    }
}

/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; unused fill gaps */
/* BYTES(dead-code): unusedContent is read and never used, as in the original */
ScnObject *FloatingBox_Create(void *record)
{
    struct Work {
        u16 unused;
        u16 model;
        FloatingBox *object;
    } w;
    u32 unusedContent;
    w.object = new FloatingBox;
    w.model = WAR_IDO_ACAISS02;
    /* cast kept: InitWithAltModels returns the ScnObject base of this box */
    w.object =
        (FloatingBox *)w.object->InitWithAltModels(record, &w.object->mainModel, 1, &w.model, &w.object->floatModel);
    /* The shipped factory reads property 0, but never uses the value. */
    unusedContent = PropU32(record, 0);
    w.object->content = 0;
    w.object->waterBox = 0;
    w.object->restoreUnset = 1;
    return w.object;
}

void FloatingBox_BuildRecord(ScnRecordSynth *out, const Vec3s *pos, u32 contentProp)
{
    u16 count;
    u32 model;
    out->pos = *pos;
    out->rot.x = out->rot.y = out->rot.z = 0;
    out->classId = CLASSID_FLOATINGBOX;
    model = *Scn_FindIdList(WAR_IDO_ACAISS03, &count);
    /* cast kept: an export id list holds its record pointers as u32 words */
    out->modelResIndex = Dav_FindResourceIndex((void *)model);
    out->secondaryRes = 0xffff;
    SetProp(out, 0, contentProp);
}

/* 24 empty blocks: a device that raises the inliner's estimate (see the INLINE BUDGET note above). */
/* BYTES(slot-scope, inferred): the nested block(s) only order the frame: their locals are allocated after the enclosing scope's */
/* BYTES(inline): the 24 empty blocks only raise /Ob1's estimate (22 and up match); they emit nothing */
inline CollBox *ScnObject::GetFirstModelBox()
{
    {
        {
            {
                {
                    {
                        {
                            {
                                {
                                    {
                                        {
                                            {
                                                {
                                                    {
                                                        {
                                                            {
                                                                {
                                                                    {
                                                                        {
                                                                            {
                                                                                {
                                                                                    {
                                                                                        {
                                                                                            {
                                                                                                {
                                                                                                    ModelBoxList
                                                                                                        *boxes =
                                                                                                            inst_model
                                                                                                                ->boxes;
                                                                                                    if (boxes)
                                                                                                        return boxes
                                                                                                            ->boxes;
                                                                                                    return 0;
                                                                                                }
                                                                                            }
                                                                                        }
                                                                                    }
                                                                                }
                                                                            }
                                                                        }
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
