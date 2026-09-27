/* T089 - the original object Sam_Pirate.cpp (guessed name), one file.
 * .text 0x46ed60-0x47237a (Sam_Pirate_Init .. Sam_Pirate_Create), .rdata 0x57501c-0x575040 (the Sam_Pirate vtable
 * COMDAT), .data 0x57ab24-0x57ab38 (the boat and jail idle-anim tables, then 4 bytes of alignment pad).
 * The state code, the update, the helpers and the factory, in address order. The embedded emitter constructor is
 * represented by the InlineEmitter16 member, not a replacement class.
 */
/* BYTES: slot-group, slot-name. */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *instance, Animator *animator, u16 id, u32 options);
#include "animation.h"
#include "../engine/sound_mgr.h"
#include "../engine/scn_tools.h"
#include "world_draw.h"
#include "../engine/fixed_math.h"
#include "../engine/id_list.h"

#define SDW_MEMBERS_ScnObject            \
    static void *operator new(u32 size); \
    void SetUpdateMode(s32 mode);

#define SDW_EXTRA_ScnObject \
    u16 GetHeadingU16()     \
    {                       \
        return rot.y;       \
    }


#define SDW_EXTRA_ScnBody                         \
    u32 AnimDuration(u16 id)                      \
    {                                             \
        return Anim_GetDurationMs(Inst(), id, 1); \
    }

#define SDW_MEMBERS_ZoneList void Load(u32 id);
#define SDW_MEMBERS_CollBox s32 ContainsXZ(const Vec3s *p);
#define SDW_EXTRA_CollBox                                                                                          \
    s32 ContainsXYZ(const Vec3s *p)                                                                                \
    {                                                                                                              \
        return p->x >= min.x && p->x <= max.x && p->y >= min.y && p->y <= max.y && p->z >= min.z && p->z <= max.z; \
    }
#define SDW_EXTRA_Sam_Pirate                         \
    void SetIdleDuration(u32 duration, s32 extended) \
    {                                                \
        if (extended)                                \
            timerMs = duration * 4;                  \
        else                                         \
            timerMs = duration * 2;                  \
    }
#define SDW_MEMBERS_InlineEmitter16 InlineEmitter16();
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_ISSOUNDPLAYING_U16 1
#define SDW_INLINE_SCNOBJECT_STOPSOUNDHANDLE_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_ISSOUNDPLAYING_U16
#undef SDW_INLINE_SCNOBJECT_STOPSOUNDHANDLE_U16
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_CURRENTANIM 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_CURRENTANIM
#define SDW_INLINE_COLLBOX_CONTAINSXZ_CONST_VEC3S 1
#include "../engine/coll_box_inlines.h"
#undef SDW_INLINE_COLLBOX_CONTAINSXZ_CONST_VEC3S
extern Wolf *g_pWolf;
s32 Vec3s_DistXZ(Vec3s *a, Vec3s *b);
#define ABS_VALUE(x) ((x) >= 0 ? (x) : -(x))
s32 Scenaric_FindByClass(u16 classId, ScnObject **out, s32 capacity);
s32 Rand_Bounded(s32 limit);
u16 Sound_Play(u16 sound, void *owner, u16 volume, u8 category, s32 flags);
u8 Dialogue_Say(const char *, s32, ScnObject *, u32);
void Dialogue_Reset();
extern s32 g_dtMs;

/* 0x57ab24 / 0x57ab2c - the idle animations played on the boat and in the jail (index 0 is the circling walk) */
u16 g_samPirateBoatIdleAnims[4] = {APIRAT01_ANIM_WALK, APIRAT01_ANIM_STAND, APIRAT01_ANIM_STAND0, APIRAT01_ANIM_STAND1};
u16 g_samPirateJailIdleAnims[4] = {APIRAT01_ANIM_WALK, APIRAT01_ANIM_NO, APIRAT01_ANIM_NO1, APIRAT01_ANIM_HIT};

#define SDW_INLINE_FREE_SCN_GETPROPU32_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_SCN_GETPROPU32_VOID_U32
#define SDW_INLINE_ZONELIST_LOAD_U32 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_LOAD_U32
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32

void Sam_Pirate::PostLoadInit()
{
    void *properties = record;
    u8 index;
    jailBox = Scn_GetPropBox(properties, 12);
    nearJailBox = Scn_GetPropBox(properties, 4);
    nearJailPos.x = (nearJailBox->max[0] + nearJailBox->min[0]) / 2;
    nearJailPos.y = -200;
    nearJailPos.z = (nearJailBox->max[2] + nearJailBox->min[2]) / 2;
    nearJailPos.y = QueryGroundY(&nearJailPos, 1);
    bridgeBox = Scn_GetPropBox(properties, 8);
    forbiddenBoxes.Load(Scn_GetPropU32(properties, 0));
    textInBoat1 = Text_GetClassString((u8)Scn_GetPropU32(properties, 20));
    textInBoat2 = Text_GetClassString((u8)Scn_GetPropU32(properties, 24));
    textInJail = Text_GetClassString((u8)Scn_GetPropU32(properties, 28));
    trajBoat = Scn_GetPropTrajectory(properties, 32);
    TrajFollower_Init(&trajFollower, trajBoat, 250, 2048, 1, 0, 50);
    trajPointCount = trajBoat->count;
    for (index = 0; index < trajPointCount; ++index) {
        trajPoints[index].x = 0;
        trajPoints[index].y = 0;
        trajPoints[index].z = 0;
    }
    for (index = 0; index < trajPointCount; ++index)
        trajPoints[index] = trajBoat->pts[index];
    trajExclude.x = 0;
    trajExclude.y = 0;
    trajExclude.z = 0;
    guardPost = PickTrajPoint(trajPoints, trajPointCount, trajExclude, 1);
    bridgeEndPos = PickTrajPoint(trajPoints, trajPointCount, trajExclude, 0);
    nbCoins = Scn_GetPropU32(properties, 16);
    for (index = 0; index < nbCoins; ++index)
        coins[index] = 0;
    coinCount = Scenaric_FindByClass(CLASSID_GOLDENCOINS, coins, nbCoins);
    Scenaric_FindByClass(CLASSID_FALLINGGATE2, &jailGate, 1);
    dustParams.vSpeed = -20;
    dustParams.life = 4096;
    dustParams.spawnInterval = dustParams.life / 6;
    dustParams.sizeStart = 10;
    dustParams.hSpeed = 100;
    dustParams.sizeEnd = 40;
    dustParams.sheetIndex = 0;
    keepSearchOrigin = 0;
    coinPickedBefore = 0;
    jailWarningGiven = 0;
    jailedOnce = 0;
    nearestCoinDist = 600;
    SnapToGround(1);
    coinSearchOrigin.x = 0;
    coinSearchOrigin.y = 0;
    coinSearchOrigin.z = 0;
    spawnPos = pos;
    SetUpdateMode(SCN_UPD_ALWAYS);
    shadow.radius = 40;
    shotSoundHandle = 0;
    talkedFlag = 0;
    boatTextToggle = 0;
    SetState(SAMP_STT_GOTO_POST);
}

/* cast kept (both macros): Box and CollBox are two views of one 16-byte record (CollBox has the tests) */
#define IN_XZ(box, point) (((CollBox *)(box))->ContainsXZ(&(point)))
#define IN_XYZ(box, point) (((CollBox *)(box))->ContainsXYZ(&(point)))
/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets */
void Sam_Pirate::Update()
{
    struct Work {
        s32 coinWolfDistance, jailOutsideDistance, jailInsideDistance, jailReturnDistance, jailApproachDistance,
            homeDistance, talkApproachDistance;
        Vec3s talkPoint;
        u16 talkPointPad;
        Vec3s coinPoint;
        u16 coinPointPad;
        s32 targetDistance, postDistance, coinDistance, samCoinDistance, wolfCoinDistance;
        u8 padZone[2], coinIndex, zoneIndex;
        s32 animDuration, guardDistance, spawnDistance, guardMoveDistance;
        s32 voice;
        char *text;
        s32 candidateDistance, hearDistance;
        u8 padIndex[3], index;
    } w;
    if (IN_XZ(bridgeBox, pos) || ABS_VALUE(pos.y) > ABS_VALUE(guardPost.y) - 10)
        onBoat = 1;
    else
        onBoat = 0;
    if (IN_XZ(jailBox, pos) && !jailedOnce && !jailGate->HandleMessage(this, MSG_GATE_QUERY, 0))
        SetState(SAMP_STT_GO_TO_JAIL);
    if (!coinHuntActive) {
        for (w.index = 0; w.index < nbCoins; ++w.index) {
            if (coins[w.index]->HandleMessage(this, MSG_COIN_IS_HELD, 0) &&
                (w.hearDistance = Vec3s_DistXZ(&g_pWolf->pos, &pos)) < 600 &&
                ABS_VALUE(coins[w.index]->pos.y) <= ABS_VALUE(pos.y))
                /* cast kept: MSG_COIN_SET_HINT_DIST's arg is the distance, a number in the void * */
                coins[w.index]->HandleMessage(this, MSG_COIN_SET_HINT_DIST, (void *)w.hearDistance);
            if (!coins[w.index]->HandleMessage(this, MSG_COIN_IS_TAKEN, 0)) {
                if (coinSearchOrigin.x == 0 && coinSearchOrigin.y == 0 && coinSearchOrigin.z == 0)
                    coinSearchOrigin = pos;
                w.candidateDistance = Vec3s_DistXZ(&coins[w.index]->pos, &coinSearchOrigin);
                if (w.candidateDistance <= 600 && ABS_VALUE(coins[w.index]->pos.y) <= ABS_VALUE(pos.y) + 20) {
                    SetState(SAMP_STT_LEAVE_FOR_COIN);
                    break;
                }
            }
        }
    }
    dustEmitter.base.Emitter_UpdateDrift(&dustParams, &pos, GetHeadingU16(), 0);
    switch (state) {
        case SAMP_STT_TALK:
            FaceToward(g_pWolf->pos);
            if (onBoat) {
                if (!boatTextToggle) {
                    w.text = textInBoat1;
                    w.voice = VOICE_CINSA_LVL1501;
                } else {
                    w.text = textInBoat2;
                    w.voice = VOICE_CINSA_LVL1502;
                }
            } else {
                w.text = textInJail;
                w.voice = VOICE_CINSA_LVL1503;
                jailWarningGiven = 1;
            }
            if (!Dialogue_Say(w.text, w.voice, this, 1)) {
                Dialogue_Reset();
                g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0);
                talkedFlag = 1;
                if (onBoat) {
                    if (!boatTextToggle)
                        boatTextToggle = 1;
                    else
                        boatTextToggle = 0;
                    SetState(SAMP_STT_GUARD);
                } else
                    SetState(SAMP_STT_GO_TO_JAIL);
            }
            break;
        case SAMP_STT_GOTO_POST:
            w.guardMoveDistance = MoveToward(guardPost, 250, 150, guardPost, bridgeEndPos, onBoat);
            if (w.guardMoveDistance < 30)
                SetState(SAMP_STT_GUARD);
            break;
        case SAMP_STT_LEAVE_FOR_COIN:
            coinHuntActive = 1;
            w.spawnDistance = MoveToward(spawnPos, 250, 150, guardPost, bridgeEndPos, onBoat);
            if (w.spawnDistance < 30)
                SetState(SAMP_STT_SEEK_COIN);
            break;
        case SAMP_STT_GUARD:
            coinHuntActive = 0;
            coinSearchOrigin.x = coinSearchOrigin.y = coinSearchOrigin.z = 0;
            wolfDist = Vec3s_DistXZ(&g_pWolf->pos, &pos);
            if (wolfDist <= 400)
                SetState(SAMP_STT_DRAW);
            switch (CurrentAnim()) {
                case APIRAT01_ANIM_WALK:
                    circleAngle = CircleStep(circleOffset, circlePivot, circleAngle, -30);
                    if (ABS_VALUE(circleAngle) > 0x2ffd) {
                        circleAngle = 0;
                        SetState(SAMP_STT_GUARD);
                    }
                    break;
                case APIRAT01_ANIM_STAND:
                case APIRAT01_ANIM_STAND0:
                case APIRAT01_ANIM_STAND1:
                    FaceToward(bridgeEndPos);
                    timerMs -= g_dtMs;
                    if (timerMs <= 0)
                        SetState(SAMP_STT_GUARD);
                    break;
            }
            w.guardDistance = Vec3s_DistXZ(&g_pWolf->pos, &guardPost);
            if (w.guardDistance <= 600 && !talkedFlag && IN_XYZ(bridgeBox, g_pWolf->pos) &&
                g_pWolf->HandleMessage(this, MSG_FREEZE, 0)) {
                g_pWolf->HandleMessage(this, MSG_WOLF_STRIP, 0);
                SetState(SAMP_STT_WALK_TO_TALK);
            }
            if (w.guardDistance > 600)
                talkedFlag = 0;
            break;
        case SAMP_STT_DRAW:
            FaceToward(g_pWolf->pos);
            if (AnimFlags(ANIM_F_FINISHED)) {
                w.animDuration = Anim_GetDurationMs(Inst(), APIRAT01_ANIM_SHOOT, 1);
                timerMs = w.animDuration * 2;
                SetState(SAMP_STT_SHOOT);
            }
            break;
        case SAMP_STT_SHOOT:
            FaceToward(g_pWolf->pos);
            timerMs -= g_dtMs;
            if (timerMs <= 0)
                SetState(SAMP_STT_HOLSTER);
            break;
        case SAMP_STT_HOLSTER:
            FaceToward(g_pWolf->pos);
            if (AnimFlags(ANIM_F_FINISHED) && IsSoundPlaying(shotSoundHandle))
                StopSoundHandle(shotSoundHandle);
            break;
        case SAMP_STT_SEEK_COIN:
            dustEmitter.base.Emitter_UpdateDrift(&dustParams, &pos, GetHeadingU16(), 1);
            coinHuntActive = 1;
            coinFound = 0;
            for (w.coinIndex = 0; w.coinIndex < nbCoins; ++w.coinIndex) {
                if (!coins[w.coinIndex]->HandleMessage(this, MSG_COIN_IS_TAKEN, 0)) {
                    if (!keepSearchOrigin)
                        coinSearchOrigin = pos;
                    w.coinDistance = Vec3s_DistXZ(&coins[w.coinIndex]->pos, &coinSearchOrigin);
                    w.postDistance = Vec3s_DistXZ(&guardPost, &pos);
                    if ((onBoat && IN_XZ(bridgeBox, coins[w.coinIndex]->pos)) ||
                        ABS_VALUE(pos.y) < ABS_VALUE(guardPost.y) + 100) {
                        if (w.coinDistance <= 600 && ABS_VALUE(coins[w.coinIndex]->pos.y) <= ABS_VALUE(pos.y) + 20) {
                            coinFound = 1;
                            if (w.coinDistance <= nearestCoinDist) {
                                nearestCoinDist = w.coinDistance;
                                targetCoin = coins[w.coinIndex];
                            }
                        }
                    }
                }
            }
            keepSearchOrigin = 0;
            if (!coinFound) {
                SetState(SAMP_STT_NO_COIN);
                break;
            } else {
                w.targetDistance = MoveToward(targetCoin->pos, 450, 150, guardPost, bridgeEndPos, onBoat);
                if (w.targetDistance < 50) {
                    timerMs = 700;
                    pocketStep = 0;
                    coinScale = 1024;
                    if (ABS_VALUE(pos.y) - ABS_VALUE(targetCoin->pos.y) > 40)
                        SetState(SAMP_STT_SNAP_TO_COIN);
                    else {
                        if (!coinPickedBefore)
                            SetState(SAMP_STT_PICK_COIN);
                        else
                            SetState(SAMP_STT_POCKET_COIN);
                    }
                    break;
                } else {
                    if (IN_XZ(jailBox, targetCoin->pos) && !jailGate->HandleMessage(this, MSG_GATE_QUERY, 0))
                        SetState(SAMP_STT_RETURN_TO_POST);
                    for (w.zoneIndex = 0; w.zoneIndex < forbiddenBoxes.count; ++w.zoneIndex)
                        if (IN_XZ(forbiddenBoxes.boxes[w.zoneIndex], targetCoin->pos) &&
                            forbiddenBoxes.boxes[w.zoneIndex])
                            SetState(SAMP_STT_RETURN_TO_POST);
                    w.samCoinDistance = Vec3s_DistXZ(&targetCoin->pos, &pos);
                    w.wolfCoinDistance = Vec3s_DistXZ(&targetCoin->pos, &g_pWolf->pos);
                    if (w.samCoinDistance < 250 && w.wolfCoinDistance < 30) {
                        timerMs = 1000;
                        SetState(SAMP_STT_STARE_COIN_THIEF);
                    }
                }
            }
            break;
        case SAMP_STT_POCKET_COIN:
            keepSearchOrigin = 1;
            timerMs -= g_dtMs;
            if (timerMs <= 0) {
                if (pocketStep == 0) {
                    coinScale = 0x2aa;
                    timerMs = 100;
                    w.coinPoint = targetCoin->pos;
                    w.coinPoint.y -= 50;
                    targetCoin->SetPosition(&w.coinPoint);
                }
                if (pocketStep == 1)
                    coinScale = 0x155;
                ++pocketStep;
            }
            targetCoin->HandleMessage(this, MSG_COIN_CLAIM, 0);
            /* cast kept: MSG_COIN_SET_SCALE's arg is the scale, a number in the void * */
            targetCoin->HandleMessage(this, MSG_COIN_SET_SCALE, (void *)coinScale);
            if (AnimFlags(ANIM_F_FINISHED)) {
                nearestCoinDist = 600;
                coinSearchOrigin = targetCoin->pos;
                /* cast kept: MSG_KILL's arg is the WolfKillType, a number in the void * */
                targetCoin->HandleMessage(this, MSG_KILL, (void *)KILL_GENERIC);
                SetState(SAMP_STT_SEEK_COIN);
            }
            if (IN_XZ(jailBox, pos) && state != SAMP_STT_GO_TO_JAIL &&
                !jailGate->HandleMessage(this, MSG_GATE_QUERY, 0)) {
                /* cast kept: MSG_KILL's arg is the WolfKillType, a number in the void * */
                targetCoin->HandleMessage(this, MSG_KILL, (void *)KILL_GENERIC);
                SetState(SAMP_STT_GO_TO_JAIL);
            }
            break;
        case SAMP_STT_NO_COIN:
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(SAMP_STT_RETURN_TO_POST);
            break;
        case SAMP_STT_PICK_COIN:
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(SAMP_STT_POCKET_COIN);
            break;
        case SAMP_STT_SNAP_TO_COIN:
            SetPosition(&targetCoin->pos);
            SetState(SAMP_STT_SEEK_COIN);
            break;
        case SAMP_STT_WALK_TO_TALK:
            if (onBoat) {
                w.talkPoint.x = guardPost.x;
                w.talkPoint.y = guardPost.y;
                w.talkPoint.z = guardPost.z;
            } else {
                w.talkPoint.x = nearJailPos.x;
                w.talkPoint.y = nearJailPos.y;
                w.talkPoint.z = nearJailPos.z;
            }
            w.talkApproachDistance = MoveToward(w.talkPoint, 250, 150, guardPost, bridgeEndPos, onBoat);
            if (w.talkApproachDistance < 20)
                SetState(SAMP_STT_TALK);
            break;
        case SAMP_STT_RETURN_TO_POST:
            dustEmitter.base.Emitter_UpdateDrift(&dustParams, &pos, GetHeadingU16(), 1);
            if (IN_XZ(jailBox, pos) && !jailGate->HandleMessage(this, MSG_GATE_QUERY, 0))
                SetState(SAMP_STT_GO_TO_JAIL);
            w.homeDistance = MoveToward(guardPost, 450, 150, guardPost, bridgeEndPos, onBoat);
            if (w.homeDistance < 20)
                SetState(SAMP_STT_GUARD);
            break;
        case SAMP_STT_GO_TO_JAIL:
            w.jailApproachDistance = MoveToward(nearJailPos, 450, 150, guardPost, bridgeEndPos, onBoat);
            if (w.jailApproachDistance < 20) {
                jailSpot = pos;
                SetState(SAMP_STT_JAILED);
            }
            break;
        case SAMP_STT_LEAVE_JAIL:
            w.jailReturnDistance = MoveToward(jailSpot, 250, 150, guardPost, bridgeEndPos, onBoat);
            if (w.jailReturnDistance < 20)
                SetState(SAMP_STT_RETURN_TO_POST);
            break;
        case SAMP_STT_JAILED:
            if (jailGate->HandleMessage(this, MSG_GATE_QUERY, 0))
                SetState(SAMP_STT_LEAVE_JAIL);
            switch (CurrentAnim()) {
                case APIRAT01_ANIM_WALK:
                    circleAngle = CircleStep(circleOffset, circlePivot, circleAngle, -30);
                    if (ABS_VALUE(circleAngle) > 0x1ffe) {
                        circleAngle = 0;
                        SetState(SAMP_STT_JAILED);
                    }
                    break;
                case APIRAT01_ANIM_NO:
                case APIRAT01_ANIM_NO1:
                case APIRAT01_ANIM_HIT:
                    FaceToward(bridgeEndPos);
                    timerMs -= g_dtMs;
                    if (timerMs <= 0)
                        SetState(SAMP_STT_JAILED);
                    break;
            }
            if (IN_XYZ(jailBox, g_pWolf->pos)) {
                if (ABS_VALUE(g_pWolf->pos.y) < ABS_VALUE(pos.y) + 40) {
                    w.jailInsideDistance = Vec3s_DistXZ(&g_pWolf->pos, &nearJailPos);
                    if (w.jailInsideDistance <= 500) {
                        if (!jailWarningGiven) {
                            if (g_pWolf->HandleMessage(this, MSG_FREEZE, 0))
                                SetState(SAMP_STT_TALK);
                        } else
                            SetState(SAMP_STT_DRAW);
                    }
                    if (jailWarningGiven)
                        SetState(SAMP_STT_DRAW);
                }
            } else {
                w.jailOutsideDistance = Vec3s_DistXZ(&g_pWolf->pos, &nearJailPos);
                if (w.jailOutsideDistance <= 400 && !talkedFlag && g_pWolf->HandleMessage(this, MSG_FREEZE, 0)) {
                    g_pWolf->HandleMessage(this, MSG_WOLF_STRIP, 0);
                    SetState(SAMP_STT_WALK_TO_TALK);
                }
                if (w.jailOutsideDistance > 400)
                    talkedFlag = 0;
            }
            break;
        case SAMP_STT_STARE_COIN_THIEF:
            FaceToward(g_pWolf->pos);
            timerMs -= g_dtMs;
            if (timerMs <= 0) {
                w.coinWolfDistance = Vec3s_DistXZ(&targetCoin->pos, &g_pWolf->pos);
                if (w.coinWolfDistance < 30)
                    SetState(SAMP_STT_DRAW);
                else
                    SetState(SAMP_STT_SEEK_COIN);
            }
            break;
    }
    AdvanceAnim();
}

s32 Sam_Pirate::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    switch (msgId) {
        default:
            break;
    }
    return 0;
}

/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets; unused fill gaps */
void Sam_Pirate::SetState(u8 next)
{
    struct Work {
        u32 jailDuration, boatDuration;
        u8 unused[3];
        u8 index;
    } w;
    state = next;
    switch (next) {
        case SAMP_STT_TALK:
            PlayAnim(APIRAT01_ANIM_STAND, 1, 1);
            break;
        case SAMP_STT_GOTO_POST:
            PlayAnim(APIRAT01_ANIM_WALK, 1, 1);
            break;
        case SAMP_STT_GUARD:
            coinPickedBefore = 0;
            do {
                idleAnimPick = (u8)Rand_Bounded(4);
            } while (idleAnimIdx == idleAnimPick);
            idleAnimIdx = idleAnimPick;
            switch (idleAnimIdx) {
                case 0:
                    SetPosition(&spawnPos);
                    circleAngle = -30;
                    circlePivot.x = spawnPos.x;
                    circlePivot.y = spawnPos.y;
                    circlePivot.z = spawnPos.z;
                    circlePivot.x -= 150;
                    circleOffset.x = pos.x - circlePivot.x;
                    circleOffset.y = pos.y - circlePivot.y;
                    circleOffset.z = pos.z - circlePivot.z;
                    PlayAnim(APIRAT01_ANIM_WALK, 1, 1);
                    break;
                case 1:
                case 2:
                case 3:
                    w.boatDuration = AnimDuration(g_samPirateBoatIdleAnims[idleAnimIdx]);
                    timerMs = w.boatDuration * 2;
                    PlayAnim(g_samPirateBoatIdleAnims[idleAnimIdx], 1, 1);
                    break;
            }
            break;
        case SAMP_STT_DRAW:
            PlayAnim(APIRAT01_ANIM_SHOOT, 0, 1);
            break;
        case SAMP_STT_SHOOT:
            /* cast kept: MSG_KILL's arg is the WolfKillType, a number in the void * */
            g_pWolf->HandleMessage(this, MSG_KILL, (void *)KILL_PIRATE);
            shotSoundHandle = Sound_Play(SND_PIRATE_SHOT, this, 255, SNDF_POSITIONAL, 4096);
            PlayAnim(APIRAT01_ANIM_SHOOT1, 1, 1);
            break;
        case SAMP_STT_HOLSTER:
            PlayAnim(APIRAT01_ANIM_SHOOT2, 0, 1);
            break;
        case SAMP_STT_LEAVE_FOR_COIN:
        case SAMP_STT_LEAVE_JAIL:
        case SAMP_STT_SEEK_COIN:
        case SAMP_STT_RETURN_TO_POST:
        case SAMP_STT_WALK_TO_TALK:
            PlayAnim(APIRAT01_ANIM_RUN, 1, 1);
            break;
        case SAMP_STT_POCKET_COIN:
            PlayAnim(APIRAT01_ANIM_TAKE, 0, 1);
            break;
        case SAMP_STT_NO_COIN:
            PlayAnim(APIRAT01_ANIM_STAND3, 0, 1);
            break;
        case SAMP_STT_PICK_COIN:
            coinPickedBefore = 1;
            PlayAnim(APIRAT01_ANIM_HAPPY, 0, 1);
            break;
        case SAMP_STT_GO_TO_JAIL:
            nearestCoinDist = 600;
            for (w.index = 0; w.index < nbCoins; ++w.index)
                /* cast kept: MSG_KILL's arg is the WolfKillType, a number in the void * */
                coins[w.index]->HandleMessage(this, MSG_KILL, (void *)KILL_GENERIC);
            PlayAnim(APIRAT01_ANIM_RUN, 1, 1);
            break;
        case SAMP_STT_JAILED:
            do {
                idleAnimPick = (u8)Rand_Bounded(4);
            } while (idleAnimIdx == idleAnimPick);
            idleAnimIdx = idleAnimPick;
            if (!jailedOnce) {
                idleAnimIdx = 3;
                jailedOnce = 1;
            }
            switch (idleAnimIdx) {
                case 0:
                    SetPosition(&jailSpot);
                    circleAngle = -30;
                    circlePivot.x = jailSpot.x;
                    circlePivot.y = jailSpot.y;
                    circlePivot.z = jailSpot.z;
                    circlePivot.x += 150;
                    circleOffset.x = pos.x - circlePivot.x;
                    circleOffset.y = pos.y - circlePivot.y;
                    circleOffset.z = pos.z - circlePivot.z;
                    PlayAnim(APIRAT01_ANIM_WALK, 1, 1);
                    break;
                case 1:
                case 2:
                case 3:
                    w.jailDuration = AnimDuration(g_samPirateJailIdleAnims[idleAnimIdx]);
                    SetIdleDuration(w.jailDuration, 3);
                    PlayAnim(g_samPirateJailIdleAnims[idleAnimIdx], 1, 1);
                    break;
            }
            break;
        case SAMP_STT_STARE_COIN_THIEF:
            PlayAnim(APIRAT01_ANIM_NO, 1, 1);
            break;
    }
}

/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; gap, pad fill gaps */
Vec3s Sam_Pirate::PickTrajPoint(Vec3s *points, u16 count, Vec3s exclude, s32 nearest)
{
    /* Local work records describe observed stack placement, not game layouts. */
    struct Work {
        s32 distance;
        u8 gap[3];
        u8 index;
        s32 bestDistance;
        Vec3s result;
        u16 pad;
    } w;
    w.bestDistance = 0;
    for (w.index = 0; w.index < count; ++w.index) {
        w.distance = Vec3s_DistXZ(&points[w.index], &pos);
        if (nearest) {
            if ((w.distance < w.bestDistance || w.bestDistance == 0) && !Vec3sEqual(exclude, points[w.index])) {
                w.bestDistance = w.distance;
                w.result = points[w.index];
            }
        } else {
            if ((w.distance > w.bestDistance || w.bestDistance == 0) && !Vec3sEqual(exclude, points[w.index])) {
                w.bestDistance = w.distance;
                w.result = points[w.index];
            }
        }
    }
    return w.result;
}

/* BYTES(slot-name): names chosen for their stack slots; the Vec4s pad word is unused */
s32 Sam_Pirate::MoveToward(Vec3s target, s32 speed, s16 fallSpeed, Vec3s post, Vec3s bridgeEnd, s32 aboard)
{
    /* Named slots reproduce the original /Od frame; Vec4s padding is unused. */
    s32 samDistances[2];
    u8 localIndex;
    Vec3s routeTargets[2];
    s32 currentDistBridge;
    ContactInfo contactInfo;
    s32 currentOriginalDistance;
    Vec3s localVelocity;
    u16 currentHit;
    s32 routeRange;
    Vec4s dir;
    Vec3s delta;
    currentOriginalDistance = Vec3s_DistXZ(&target, &pos);
    routeRange = currentOriginalDistance;
    if (target.x == post.x && target.z == post.z && !aboard) {
        currentDistBridge = Vec3s_DistXZ(&bridgeEnd, &pos);
        if (currentDistBridge > 30) {
            target.x = bridgeEnd.x;
            target.y = bridgeEnd.y;
            target.z = bridgeEnd.z;
            routeRange = currentDistBridge;
        }
    }
    if (ABS_VALUE(target.y) > ABS_VALUE(pos.y) + 20 && !aboard) {
        routeTargets[0].x = bridgeEnd.x;
        routeTargets[0].y = bridgeEnd.y;
        routeTargets[0].z = bridgeEnd.z;
        routeTargets[1].x = bridgeEnd.x;
        routeTargets[1].y = bridgeEnd.y;
        routeTargets[1].z = bridgeEnd.z;
        routeTargets[0].z += 50;
        routeTargets[1].z -= 50;
        for (localIndex = 0; localIndex < 2; ++localIndex)
            samDistances[localIndex] = Vec3s_DistXZ(&routeTargets[localIndex], &pos);
        if (samDistances[0] < samDistances[1]) {
            target.x = routeTargets[0].x;
            target.y = routeTargets[0].y;
            target.z = routeTargets[0].z;
            routeRange = samDistances[0];
        } else {
            target.x = routeTargets[1].x;
            target.y = routeTargets[1].y;
            target.z = routeTargets[1].z;
            routeRange = samDistances[1];
        }
    }
    FaceToward(target);
    if (currentOriginalDistance == 0) {
        localVelocity.x = 0;
        localVelocity.y = 0;
        localVelocity.z = 0;
    } else {
        dir.x = target.x - pos.x;
        dir.y = 0;
        dir.z = target.z - pos.z;
        localVelocity.x = (dir.x * speed) / routeRange;
        if (aboard)
            localVelocity.y = fallSpeed;
        else
            localVelocity.y = fallSpeed / 2;
        localVelocity.z = (dir.z * speed) / routeRange;
        Vec3s_ScaleByDt(&localVelocity, &delta);
        currentHit = Collide_ResolveMove(&delta, &contactInfo, 1400, RESOLVE_SLIDE_ALL, 0, 0, 10, 0, 0);
        if (currentHit && contactInfo.movableObj && contactInfo.movableObj->GetClassId() == CLASSID_WOLF &&
            g_pWolf->HandleMessage(this, MSG_WOLF_IS_DEAD, 0) == 0)
            SetState(SAMP_STT_DRAW);
        Translate(&delta);
    }
    return currentOriginalDistance;
}

void Sam_Pirate::FaceToward(Vec3s target)
{
    s16 angle = HeadingTo(&target);
    rot.y = angle;
}

void Sam_Pirate::Render(Camera *view)
{
    dustEmitter.base.Emitter_Render(view, 0);
    ScnMobile::Render(view);
}

/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets; padNew, padRot fill gaps */
s16 Sam_Pirate::CircleStep(Vec3s offset, Vec3s pivot, s16 angle, s16 step)
{
    struct Work {
        Mat34s matrix;
        Vec4i rotated;
        Vec3s newPos;
        u16 padNew;
        Vec3s rotation;
        u16 padRot;
    } w;
    w.rotation.x = 0;
    w.rotation.y = 0;
    w.rotation.z = 0;
    w.rotation.y = angle;
    angle += step;
    w.rotated.x = offset.x;
    w.rotated.y = offset.y;
    w.rotated.z = offset.z;
    Mat34s_FromEulerScaled(&w.rotation, &w.matrix, 0);
    /* cast kept: the transform reads x, y, z of this Vec4i as a Vec3i and writes the Vec4i back */
    Mat34s_TransformVec3i(&w.matrix, (Vec3i *)&w.rotated, &w.rotated);
    w.newPos.x = pivot.x + (s16)w.rotated.x;
    w.newPos.y = pos.y;
    w.newPos.z = pivot.z + (s16)w.rotated.z;
    FaceToward(w.newPos);
    SetPosition(&w.newPos);
    return angle;
}

s32 Sam_Pirate::Vec3sEqual(Vec3s a, Vec3s b)
{
    if (a.x == b.x && a.y == b.y && a.z == b.z)
        return 1;
    else
        return 0;
}

void Sam_Pirate::Reset()
{
    talkedFlag = 0;
    boatTextToggle = 0;
    nearestCoinDist = 600;
    dustEmitter.base.Emitter_Reset();
    if (IN_XZ(jailBox, pos))
        SetState(SAMP_STT_GO_TO_JAIL);
    else {
        coinHuntActive = 0;
        SnapToGround(1);
        SetPosition(&spawnPos);
        SetState(SAMP_STT_GOTO_POST);
    }
}

inline InlineEmitter16::InlineEmitter16()
{
    base.slotPool = slotBuf;
    base.particles = particleBuf;
    base.count = 16;
    base.Emitter_Reset();
}
ScnObject *Sam_Pirate_Create(void *record)
{
    Sam_Pirate *object = new Sam_Pirate;
    object = (Sam_Pirate *)object->Init(record, 0); /* cast kept: Init returns the object as a ScnBody * */
    return object;
}
