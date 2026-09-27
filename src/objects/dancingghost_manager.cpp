/* T133 - original object DancingGhostManager.cpp (guessed name).
 * Ranges: .text 0x4af4a0-0x4b275f (main), then its COMDAT DancingGhost::ClearGhostQueue 0x4b2760-0x4b2839;
 * .rdata 0x575e54-0x575e78 (vtable); .data 0x57b564-0x57b584 (the cinematic header's static opcode-stride copy, then
 * ParseSequences' five token literals); .bss 0x6cf604-0x6cf60b (g_dgStepDefaultPos, g_dgTokenInitial).
 * Contents: the three parsing leaves; ParseSequences .. FindTwoNearestGhosts; HandleBeatResult and its inline helpers;
 * PostLoadInit and CollectDancersInline; Reset, AbortRound; Update, AllDancersIdle, DrawClearTextInline;
 * HandleMessage; SetState and its inline helpers; Create; ClearGhostQueue.
 * Not in this object: UpdateBeatTrack 0x4b2840 (src/objects/dancingghost_track.cpp, with .bss g_dgMetronomeLatch
 * 0x6cf60c). The map places it here as a second COMDAT, but that cannot be reproduced: VC6 /Ob1 inlines an inline
 * member wherever its body is visible in the TU (definition order does not matter), within a size budget spent in
 * call order, so Update inlines UpdateBeatTrack (its first large inline call) and then has no budget left for
 * DrawClearTextInline - the
 * opposite of the exe. With UpdateBeatTrack in its own object (the map's own alternative) every function matches and
 * both objects are byte-identical.
 * ClearGhostQueue is an inline member: HandleBeatResult's inline budget is spent before the CancelPair expansions reach
 * it, so those calls stay out of line and the body is emitted as a COMDAT after the main .text, as in the exe (its
 * position in the file does not matter). The ...Inline helpers represent the observed inline expansions:
 * ClearQueueInline is the same body as ClearGhostQueue, standing for the copies of it that the original expanded in
 * place (0x4afccc) while other calls in the same function (0x4afc74/0x4afd74/0x4afe67/0x4b000a) went to the out-of-line
 * copy; the original helper names are unknown.
 * The one explicitly initialized but otherwise unused pointer in CollectDancersInline is the original's; its purpose is
 * unknown.
 * g_sinTable4096 and g_pCosTable are declared extern "C", as they are defined, so the decorated names agree at link. */
/* BYTES: dead-code, inline, layout, slot-name, slot-scope. */
/* BYTES(layout): the Cine.h header static: every object including the cinematic header carries this copy in its .data, referenced or not */
#define SDW_MEMBERS_Box s32 ContainsPointXZ(Vec3s *); /* before sdw_types.h, which defines Box */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
#include "../engine/sound_mgr.h"
#include "../app/app_main.h"
#include "dancingghost.h"
#include "../engine/interface.h"
#include "sheep.h"
#include "../engine/cine.h"
#include "../engine/scn_tools.h"
#include "camera.h"
#include "../engine/map.h"
#include "../engine/pause_menu.h"
#include "../engine/text.h"
class ScnObject;
class Camera;
void Camera_StartScripted(ScnObject *, Camera *, u16, u16, u16, Vec3s *, u16, u32, s32);
#define SDW_MEMBERS_ScnObject                                                           \
    static void *operator new(u32);                                                     \
    s32 SoundPlaying(u16 sound)                                                         \
    {                                                                                   \
        return Sound_IsPlaying(sound);                                                  \
    }                                                                                   \
    void SetUpdateMode(s32 mode);                                                       \
    void StartCamera(u16 x, u16 y, u16 z, Vec3s *point, u16 focal, u32 mode, s32 time); \
    void SetRotation(const Vec3s &value);

#define SDW_MEMBERS_Sprite void DrawAt(u32 *layer, s32 drawX, s32 yPos);
#define SDW_MEMBERS_DancingGhost \
    void ClearGhostQueue();      \
    void ClearQueueInline();     \
    void CancelPair();           \
    void CancelPairInline();     \
    s32 IsIdle()                 \
    {                            \
        if (steps[0].step == 0)  \
            return 1;            \
        return 0;                \
    }
#define SDW_MEMBERS_DancingGhostManager \
    s32 AllDancersIdle();               \
    void CollectDancersInline();        \
    void DrawClearTextInline();
#include "sdw_classes.h"
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_ISINWORLD 1
#define SDW_INLINE_SCNOBJECT_STOPSOUND_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_ISINWORLD
#undef SDW_INLINE_SCNOBJECT_STOPSOUND_U16
#define SDW_INLINE_SCNOBJECT_SETCOLLISION_S32 1
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32 1
#define SDW_INLINE_SCNOBJECT_STARTCAMERA_U16_U16_U16_VEC3S_U16_U32_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETCOLLISION_S32
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32
#undef SDW_INLINE_SCNOBJECT_STARTCAMERA_U16_U16_U16_VEC3S_U16_U32_S32
#define SDW_INLINE_SCNOBJECT_SETROTATION_CONST_VEC3S 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETROTATION_CONST_VEC3S
#define SDW_INLINE_CINE_ISACTIVE 1
#include "../engine/cine_inlines.h"
#undef SDW_INLINE_CINE_ISACTIVE
#define SDW_INLINE_SPRITE_DRAWAT_U32_S32_S32 1
#include "../engine/sprite_inlines.h"
#undef SDW_INLINE_SPRITE_DRAWAT_U32_S32_S32
extern Wolf *g_pWolf;
extern u16 g_scnObjectCount, g_dgDancerCount;
extern s32 g_gameTimeMs, g_dt;
extern u32 *g_screenLayerBase, g_uiTintColor;
extern u32 g_gameFlags;
extern "C" const s16 *g_pCosTable;
extern "C" s16 g_sinTable4096[5122];

/* 0x57b564 - the cinematic header's static copy of the 9-byte opcode stride table (src/engine/cine.cpp,
 * g_cineOpStride 0x5816fc). The original header defined it static, so every object including it carries its own
 * unreferenced copy at the head of its .data; defined here in its place. */
static u8 s_cineOpStride[9] = {0, 8, 8, 4, 2, 2, 4, 2, 2};

/* .bss 0x6cf604-0x6cf60b (g_dgMetronomeLatch 0x6cf60c belongs to UpdateBeatTrack's object, T133b) */
Vec3s g_dgStepDefaultPos; /* 0x6cf604 */
u8 g_dgTokenInitial;      /* 0x6cf60a */

s32 Scenaric_FindByClass(u16, ScnObject **, s32);
#define SDW_INLINE_FREE_GETPROP_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_GETPROP_VOID_U32

/* ---- the parsing leaves */
s16 DancingGhostManager_CountTextChars(const char *text)
{
    const char *scan = text;
    s16 count = 0;
    if (!text)
        return 0;
    if (*scan == '$') {
        do {
            scan++;
        } while (*scan != '$' && *scan);
        if (*scan)
            scan++;
    }
    while (*scan) {
        scan++;
        count++;
    }
    return count;
}

s32 DancingGhostManager_TokenEquals(const u8 *text, const u8 *pattern)
{
    const u8 *token = text, *literal = pattern;
    while (*literal) {
        if (*token != *literal || !*token)
            return 1;
        token++;
        literal++;
    }
    return 0;
}

s32 DancingGhostManager_NextToken(u8 *out, const u8 **cursor)
{
    u16 count = 0;
    u8 *scan = out;
    *scan = 0;
    if (**cursor == '$')
        (*cursor)++;
    while (**cursor && **cursor != ',' && **cursor != '$' && count < 2) {
        *scan = **cursor;
        scan++;
        (*cursor)++;
        count++;
    }
    *scan = 0;
    if ((*cursor)[1] && (**cursor == '$' || **cursor == ','))
        (*cursor)++;
    if (!**cursor)
        return 0;
    return 1;
}

void DancingGhostManager::ParseSequences(void *record)
{
    u16 sequence, first;
    const u8 *cursor = 0;
    u8 token[3] = {g_dgTokenInitial};
    for (first = 0; first < 4; first++)
        sequenceText[first] = Text_GetClassString(GetProp(record, 0x58) + first);
    for (sequence = 0; sequence < 4; sequence++) {
        /* cast kept (the text and the five token literals below): the token parser works on unsigned bytes */
        cursor = (const u8 *)sequenceText[sequence];
        sequenceLengths[sequence] = 0;
        while (DancingGhostManager_NextToken(token, &cursor) == 1 && sequenceLengths[sequence] < 20) {
            /* cast kept: string literals compared as bytes */
            if (!DancingGhostManager_TokenEquals(token, (const u8 *)"SQ")) {
                sequenceButtons[sequence][sequenceLengths[sequence]] = (u16)~PAD_SQUARE;
                sequenceLengths[sequence]++;
            } else if (!DancingGhostManager_TokenEquals(token, (const u8 *)"TR")) {
                sequenceButtons[sequence][sequenceLengths[sequence]] = (u16)~PAD_TRIANGLE;
                sequenceLengths[sequence]++;
            } else if (!DancingGhostManager_TokenEquals(token, (const u8 *)"CI")) {
                sequenceButtons[sequence][sequenceLengths[sequence]] = (u16)~PAD_CIRCLE;
                sequenceLengths[sequence]++;
            } else if (!DancingGhostManager_TokenEquals(token, (const u8 *)"CR")) {
                sequenceButtons[sequence][sequenceLengths[sequence]] = (u16)~PAD_CROSS;
                sequenceLengths[sequence]++;
            } else if (!DancingGhostManager_TokenEquals(token, (const u8 *)"  ")) {
                sequenceButtons[sequence][sequenceLengths[sequence]] = PAD_ALL_RELEASED;
                sequenceLengths[sequence]++;
            }
        }
    }
}

void DancingGhostManager::SetGhostSpot(const Box *box)
{
    ghostSpot.x = (box->min[0] + box->max[0]) / 2;
    ghostSpot.y = g_pWolf->pos.y - 150;
    ghostSpot.z = (box->min[2] + box->max[2]) / 2;
}

void DancingGhostManager::DespawnDanceSet()
{
    u16 i, j;
    for (i = 0; i < dancerCount; i++)
        dancers[i]->Despawn();
    for (j = 0; j < tambourineCount; j++)
        tambourineGhosts[j]->Despawn();
    RemoveFromWorld();
}

void DancingGhostManager::FindTwoNearestGhosts()
{
    u16 pass;
    DancingGhost *ghost;
    s32 range;
    u16 i;
    DancingGhost *selected[2] = {0, 0};
    for (pass = 0; pass < 2; pass++) {
        nearestDistance = 65535;
        for (i = 0; i < dancerCount; i++) {
            ghost = dancers[i];
            range = Vec3s_ManhattanDistXZ(&g_pWolf->pos, &ghost->pos);
            if (range < nearestDistance) {
                if (selected[0] == ghost || selected[1] == ghost)
                    continue;
                nearestDistance = range;
                selected[pass] = ghost;
            }
        }
    }
    nearestGhost = selected[0];
    secondNearestGhost = selected[1];
}

/* ---- HandleBeatResult and its inline helpers */
inline void DancingGhost::ClearQueueInline()
{
    u16 i;
    for (i = 0; i < 6; i++) {
        steps[i].step = 0;
        steps[i].partner = 0;
        steps[i].argument = 0;
        steps[i].point.x = g_dgDefaultPos.x;
        steps[i].point.y = g_dgDefaultPos.y;
        steps[i].point.z = g_dgDefaultPos.z;
    }
    queueCount = 0;
    elapsedMs = 0;
    newStep = 1;
    stepDone = 0;
}
inline void DancingGhost::CancelPair()
{
    if (steps[0].partner)
        steps[0].partner->ClearQueueInline();
    ClearGhostQueue();
}
inline void DancingGhost::CancelPairInline()
{
    if (steps[0].partner)
        steps[0].partner->ClearQueueInline();
    ClearQueueInline();
}
inline s32 DancingGhostManager::AllDancersIdle()
{
    u16 i;
    for (i = 0; i < dancerCount; i++)
        if (!dancers[i]->IsIdle())
            return 0;
    return 1;
}
void DancingGhostManager::HandleBeatResult()
{
    u16 m, l, k, j, i;
    if (!reactionQueued) {
        FindTwoNearestGhosts();
        if (eventCode <= DANCE_EV_3 || danceCompleted) {
            for (i = 0; i < dancerCount; i++)
                dancers[i]->CancelPair();
        } else {
            nearestGhost->CancelPair();
            secondNearestGhost->CancelPair();
        }
        if (eventCode > DANCE_EV_3)
            reactionQueued = nearestGhost->PushStep(DG_STEP_ESCORT, secondNearestGhost, g_dgDefaultPos, 0);
        else
            reactionQueued = 1;
        if (reactionQueued == 1) {
            g_pWolf->HandleMessage(this, MSG_SCARE, 0);
            for (j = 0; j < tambourineCount; j++)
                tambourineGhosts[j]->CancelPair();
            switch (eventCode) {
                case DANCE_EV_INTRUDER:
                    nearestGhost->PushStep(DG_STEP_KILL_WOLF, 0, g_dgDefaultPos, 0);
                    break;
                case DANCE_EV_LEFT_RING:
                    nearestGhost->PushStep(DG_STEP_NOP, 0, g_dgDefaultPos, 0);
                    break;
                case DANCE_EV_2:
                    nearestGhost->PushStep(DG_STEP_FAIL_LINE_1, 0, g_dgDefaultPos, 0);
                    break;
                case DANCE_EV_1:
                    nearestGhost->PushStep(DG_STEP_FAIL_LINE_2, 0, g_dgDefaultPos, 0);
                    break;
                case DANCE_EV_3:
                    nearestGhost->PushStep(DG_STEP_FAIL_LINE_3, 0, g_dgDefaultPos, 0);
                    break;
            }
            if (eventCode > DANCE_EV_3 && eventCode != DANCE_EV_INTRUDER && eventCode != DANCE_EV_1) {
                if (stepIndex < 0)
                    stepIndex = 0;
                SetGhostSpot(stepBoxes[stepIndex]);
                nearestGhost->PushStep(DG_STEP_CARRY_WOLF, secondNearestGhost, ghostSpot, stepIndex);
            }
            if (!externalFreeze)
                wolfFrozen = g_pWolf->HandleMessage(this, MSG_FREEZE, 0);
        }
    } else if (AllDancersIdle()) {
        Camera_ReleaseAny();
        if (!externalFreeze && wolfFrozen)
            wolfFrozen = ~g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0) & 1;
        if (!danceCompleted) {
            if (eventCode == DANCE_EV_INTRUDER) {
                for (k = 0; k < dancerCount; k++)
                    dancers[k]->Reset();
                for (l = 0; l < tambourineCount; l++)
                    tambourineGhosts[l]->Reset();
                Reset();
            } else {
                for (m = 0; m < dancerCount; m++)
                    dancers[m]->CancelPairInline();
                AbortRound();
            }
        }
    }
}

/* ---- PostLoadInit */
/* BYTES(dead-code): unusedNext is zeroed and never read, as in the original */
inline void DancingGhostManager::CollectDancersInline()
{
    ScnObject **iterator;
    ScnObject *unusedNext;
    DancingGhost *ghost;
    u16 count;
    iterator = g_scnObjects;
    ghost = 0;
    unusedNext = 0;
    count = 0;
    dancerCount = 0;
    tambourineCount = 0;
    for (iterator = g_scnObjects, count = 0; count < g_scnObjectCount; count++, iterator++) {
        /* cast kept: a downcast: only objects of class CLASSID_DANCINGGHOST are kept */
        ghost = (DancingGhost *)*iterator;
        if (!ghost)
            break;
        if (ghost->GetClassId() == CLASSID_DANCINGGHOST) {
            if (dancerCount <= 8 && !ghost->danceFlags.tambourine) {
                dancers[dancerCount] = ghost;
                dancerCount++;
            } else if (tambourineCount <= 8) {
                tambourineGhosts[tambourineCount] = ghost;
                tambourineCount++;
            }
        }
    }
}
/* BYTES(slot-name, inferred): local names chosen for their stack slots (tools/vc6_locals.py), not recovered */
void DancingGhostManager::PostLoadInit()
{
    u16 time;
    void *recordData = record;
    spriteSquare.LoadFromRes(DAV_IDI_IMNPADS_);
    spriteTriangle.LoadFromRes(DAV_IDI_IMNPADT_);
    spriteCircle.LoadFromRes(DAV_IDI_IMNPADC_);
    spriteCross.LoadFromRes(DAV_IDI_IMNPADX_);
    spriteNote.LoadFromRes(DAV_IDI_IMDANSE4);
    spriteBarNote.LoadFromRes(DAV_IDI_IMDANSE3);
    spriteFrame.LoadFromRes(DAV_IDI_IMNINT2_);
    spriteTrack.LoadFromRes(DAV_IDI_IMDANSE2);
    spriteFrameDark.LoadFromRes(DAV_IDI_IMDANSE1);
    danceCams[0] = danceCams[1] = danceCams[2] = danceCams[3] = 0;
    danceCams[0] = Scn_GetPropCamera(recordData, 0x38);
    danceCams[1] = Scn_GetPropCamera(recordData, 0x3c);
    danceCams[2] = Scn_GetPropCamera(recordData, 0x40);
    danceCams[3] = Scn_GetPropCamera(recordData, 0x44);
    for (time = 0; time < 4; time++) {}
    cineId = GetProp(recordData, 0x24);
    cineBox = Scn_GetPropBox(recordData, 0x28);
    cineFlags = GetProp(recordData, 0x2c);
    cineText = 0;
    if (cineFlags & CINE_HAS_TEXT)
        cineText = Text_GetClassString(GetProp(recordData, 0x30));
    g_dgFailedVoices[0] = VOICE_CIN_LVL1402_JA;
    g_dgFailedVoices[1] = VOICE_CIN_LVL1403_GBB;
    g_dgFailedVoices[2] = VOICE_CIN_LVL1404_JA;
    g_dgFailedVoices[3] = VOICE_CIN_LVL1401_GB;
    g_dgFailedLines[0] = Text_GetClassString(GetProp(recordData, 0x50));
    g_dgFailedLines[1] = Text_GetClassString(GetProp(recordData, 0x48));
    g_dgFailedLines[2] = Text_GetClassString(GetProp(recordData, 0x54));
    g_dgFailedLines[3] = Text_GetClassString(GetProp(recordData, 0x4c));
    g_dgFailedLines[4] = Text_GetClassString(GetProp(recordData, 0x34));
    for (time = 0; time < 5; time++)
        if (!*g_dgFailedLines[time])
            g_dgFailedLines[time] = 0;
    stepClearedText = Text_GetClassString(GetProp(recordData, 0x5c));
    if (!*stepClearedText)
        stepClearedText = 0;
    screenWidth = 512;
    screenHeight = 240;
    cellW = spriteCross.widthMinus1 * 2 + 2;
    cellH = spriteCross.height + 1;
    trackY = cellH + (cellH >> 1);
    trackWidth = cellW * 4 + cellW * 2;
    trackHeight = cellH * 2;
    trackX = (screenWidth - trackWidth) >> 1;
    stepClearedLen = DancingGhostManager_CountTextChars(stepClearedText);
    clearLineHeight = cellH + (cellH >> 1);
    clearBoxY = clearLineHeight * 3;
    clearBoxHeight = clearLineHeight * 2;
    clearBoxWidth = cellW * stepClearedLen + cellW * 2;
    clearBoxX = (screenWidth - clearBoxWidth) >> 1;
    clearTextY = clearBoxY + (clearLineHeight >> 1);
    noteLeft = trackX + cellW;
    noteRight = noteLeft + trackWidth - cellW;
    noteTop = trackY;
    noteBottom = trackHeight + noteTop;
    beatPeriod = 2048;
    ParseSequences(recordData);
    /* cast kept: Scenaric_FindByClass fills a ScnObject * array; the class filter makes them PrayingGhosts */
    prayingCount = Scenaric_FindByClass(CLASSID_PRAYINGGHOST, (ScnObject **)prayingGhosts, 4);
    CollectDancersInline();
    g_dgDancerCount = dancerCount;
    stepBoxes[0] = stepBoxes[1] = stepBoxes[2] = stepBoxes[3] = ringBox = holeBox = safeBox = danceBox = 0;
    stepBoxes[0] = Scn_GetPropBox(recordData, 0x14);
    stepBoxes[1] = Scn_GetPropBox(recordData, 0x18);
    stepBoxes[2] = Scn_GetPropBox(recordData, 0x1c);
    stepBoxes[3] = Scn_GetPropBox(recordData, 0x20);
    ringBox = Scn_GetPropBox(recordData, 4);
    holeBox = Scn_GetPropBox(recordData, 8);
    safeBox = Scn_GetPropBox(recordData, 0x10);
    danceBox = Scn_GetPropBox(recordData, 0);
    boxRemove = Scn_GetPropBox(recordData, 0xc);
    SetCollision(0);
    cineTriggered = 0;
    cineDone = 0;
    Reset();
    SetUpdateMode(SCN_UPD_ALWAYS);
}

void DancingGhostManager::Reset()
{
    if (!IsInWorld())
        return;
    if (beatSound && SoundPlaying(beatSound))
        StopSound(beatSound);
    beatSound = 0;
    cineTriggered = 0;
    cineDone = 0;
    stepIndex = -1;
    if (danceCompleted)
        SetState(DGMGR_ST_COMPLETE, 1);
    else
        SetState(DGMGR_ST_WAIT_CINE, 1);
    roundArmed = 0;
    eventCode = DANCE_EV_NONE;
    reactionQueued = 0;
    wolfFrozen = 0;
    externalFreeze = 0;
    nearestGhost = 0;
    secondNearestGhost = 0;
    nearestDistance = 65535;
    padMask = PAD_ALL_RELEASED;
    pressedMask = PAD_ALL_RELEASED;
}

void DancingGhostManager::AbortRound()
{
    roundArmed = 0;
    eventCode = DANCE_EV_NONE;
    reactionQueued = 0;
    wolfFrozen = 0;
    externalFreeze = 0;
    nearestGhost = 0;
    secondNearestGhost = 0;
    nearestDistance = 65535;
    padMask = PAD_ALL_RELEASED;
    pressedMask = PAD_ALL_RELEASED;
    SetState(DGMGR_ST_PLACE_WOLF, 0);
}

/* ---- Update (AllDancersIdle is the copy above) */
inline void DancingGhostManager::DrawClearTextInline()
{
    s16 boxY;
    u32 oldFont;
    s16 textY;
    u32 index;
    char letterText[2];
    s32 letterInterval;
    letterInterval = 500 / stepClearedLen;
    boxY = clearBoxY;
    letterText[0] = 0;
    letterText[1] = 0;
    Text_SetWindow(g_screenLayerBase + 9, 1, 1, 500, 235, 1);
    Text_SetFont(FONT_GAME);
    oldFont = Font_SetCellSize((u8)cellW, (u8)clearLineHeight);
    if (beatElapsedMs <= 2500) {
        for (index = 0; index < stepClearedLen; index++) {
            letterText[0] = stepClearedText[index + 4];
            textY = (s32)(clearTextY * (beatElapsedMs + index * letterInterval)) / 500;
            if (textY > clearTextY)
                textY = clearTextY;
            Text_SetCursor(clearBoxX + cellW * (index + 1), textY);
            Text_WordWrap(letterText, TEXTALIGN_CONTINUE);
        }
    } else {
        textY = clearTextY - clearTextY * (beatElapsedMs - 2500) / 500;
        if (textY < -(s32)cellH)
            textY = -clearLineHeight;
        boxY = textY - (clearLineHeight >> 1);
        Text_SetCursor(clearBoxX + cellW, textY);
        Text_PrintFmt(stepClearedText);
    }
    g_spriteCrayon2.Draw(g_screenLayerBase + 10, clearBoxX, boxY, clearBoxX + clearBoxWidth, boxY + clearBoxHeight,
                         g_uiTintColor, 0);
    Hud_EndBox_stub();
    Font_SetCellSize((u8)(oldFont & 255), (u8)(oldFont >> 16));
}
/* BYTES(slot-scope, inferred): the nested block(s) only order the frame: their locals are allocated after the enclosing scope's */
void DancingGhostManager::Update()
{
    u16 i;
    WolfSpotArg spot;
    if (g_pWolf->HandleMessage(this, MSG_WOLF_IS_DEAD, 0))
        return;
    if (!eventCode) {
        if (cineBox->ContainsPointXZ(&g_pWolf->pos))
            cineTriggered = 1;
        if (safeBox->ContainsPointXZ(&g_pWolf->pos))
            insideSafe = 1;
        else
            insideSafe = 0;
        if (danceBox->ContainsPointXZ(&g_pWolf->pos))
            insideDanceZone = 1;
        else
            insideDanceZone = 0;
        if (ringBox->ContainsPointXZ(&g_pWolf->pos))
            insideRing = 1;
        else
            insideRing = 0;
        if (holeBox->ContainsPointXZ(&g_pWolf->pos))
            insideHole = 1;
        else
            insideHole = 0;
        if (!insideRing || insideHole)
            outsideRing = 1;
        else
            outsideRing = 0;
        if (stepBoxes[0]->ContainsPointXZ(&g_pWolf->pos))
            roundArmed = 1;
        if (insideSafe && danceCompleted && (escapedSheep = g_pSheepOutOfZone, escapedSheep) &&
            boxRemove->ContainsPointXZ(&escapedSheep->pos)) {
            for (i = 0; i < prayingCount; i++)
                if (prayingGhosts[i])
                    /* cast kept: HandleMessage's arg is a void *: this message passes a number in it */
                    prayingGhosts[i]->HandleMessage(this, MSG_PGHOST_REGISTER, (void *)(u32)i);
            DespawnDanceSet();
        } else {
            if (roundArmed && !danceCompleted && outsideRing && !eventCode)
                eventCode = DANCE_EV_LEFT_RING;
            if (!insideSafe && (insideDanceZone || cineTriggered) &&
                !g_pWolf->HandleMessage(this, MSG_WOLF_IS_GHOSTCOSTUME, 0)) {
                if (!eventCode)
                    eventCode = DANCE_EV_INTRUDER;
                cineTriggered = 0;
            }
        }
    }
    if (eventCode)
        HandleBeatResult();
    switch (state) {
        case DGMGR_ST_WAIT_CINE:
            if (cineTriggered)
                SetState(DGMGR_ST_CINEMATIC, 1);
            break;
        case DGMGR_ST_CINEMATIC:
            if (!g_cinePlayer.IsActive()) {
                cineDone = 1;
                SetState(DGMGR_ST_PLACE_WOLF, 1);
            }
            break;
        case DGMGR_ST_DANCE:
            if (eventCode || Game_IsPaused() || Map_IsOpen())
                break;
            {
                danceClock += g_dt;
                if (UpdateBeatTrack()) {
                    SetState(DGMGR_ST_ROUND_END, 1);
                    Camera_ReleaseAny();
                    break;
                } else
                    StartCamera(danceCams[stepIndex]->rot[0], danceCams[stepIndex]->rot[1],
                                danceCams[stepIndex]->rot[2], &danceCams[stepIndex]->eye, danceCams[stepIndex]->focal,
                                CAMSCR_BLEND_OUT, 4096);
            }
            break;
        case DGMGR_ST_PLACE_WOLF:
            if (Vec3s_ManhattanDistXZ(&stepCenter, &g_pWolf->pos) <= 150) {
                spot.pos.x = stepCenter.x;
                spot.pos.y = stepCenter.y;
                spot.pos.z = stepCenter.z;
                spot.radius = 30;
                g_pWolf->HandleMessage(this, MSG_SET_ANCHOR, &spot);
                if (AllDancersIdle() && Vec3s_ManhattanDistXZ(&stepCenter, &g_pWolf->pos) <= spot.radius)
                    SetState(DGMGR_ST_DANCE, 1);
            } else if (AllDancersIdle() && Vec3s_ManhattanDistXZ(&stepCenter, &g_pWolf->pos) > 150 && !eventCode)
                eventCode = DANCE_EV_WANDERED;
            break;
        case DGMGR_ST_ROUND_END:
            beatElapsedMs = g_gameTimeMs - beatTimeMs;
            if (beatElapsedMs <= 3000) {
                if (stepIndex + 1 < 4)
                    DrawClearTextInline();
            } else if (stepIndex + 1 < 4)
                SetState(DGMGR_ST_PLACE_WOLF, 1);
            else
                SetState(DGMGR_ST_COMPLETE, 1);
            break;
    }
}

s32 DancingGhostManager::HandleMessage(ScnObject *sender, u32 msg, void *)
{
    if (sender) {
        switch (msg) {
            case MSG_FREEZE:
                if (sender->GetClassId() == CLASSID_WOLF) {
                    externalFreeze = 1;
                    return 1;
                }
                break;
        }
    }
    return 0;
}

/* ---- SetState and its inline helpers */
#define SDW_INLINE_FREE_SETGAMEFLAGS_U32 1
#include "../engine/game_state_inlines.h"
#undef SDW_INLINE_FREE_SETGAMEFLAGS_U32
#define SDW_INLINE_FREE_CLEARGAMEFLAGS_U32 1
#include "../engine/game_state_inlines.h"
#undef SDW_INLINE_FREE_CLEARGAMEFLAGS_U32
inline void StartSceneInline(u32 id, u32 flags, const char *text)
{
    /* cast kept: Cine::Start takes the dialogue text as a void * */
    g_cinePlayer.Start(id, flags, 0, 0, (void *)text, 0);
}
inline void RotateAroundInline(Vec3s &point, Vec3s center, s16 angle)
{
    s16 x, resultZ;
    angle &= 4095;
    x = ((point.x - center.x) * g_pCosTable[angle] - (point.z - center.z) * g_sinTable4096[angle]) / 4096 + center.x;
    resultZ =
        ((point.x - center.x) * g_sinTable4096[angle] + (point.z - center.z) * g_pCosTable[angle]) / 4096 + center.z;
    point.x = x;
    point.z = resultZ;
}
void DancingGhostManager::SetState(u8 value, s32 advanceStep)
{
    Vec3s points[8], delta, targetHome, facing;
    u16 split, tambSlot, danceIndex;
    danceIndex = 0;
    tambSlot = 0;
    state = value;
    switch (value) {
        case DGMGR_ST_CINEMATIC:
            StartSceneInline((u16)cineId, cineFlags, cineText);
            break;
        case DGMGR_ST_DANCE:
            beatTimeMs = g_gameTimeMs;
            pressTimeMs = 0;
            pressLateMs = 0;
            pressLateMinus250Ms = 0;
            beatElapsedMs = 0;
            beatIndex = 0;
            roundArmed = 1;
            pressHandled = 0;
            facing.x = 0;
            facing.y = 0;
            facing.z = 0;
            currentStep = 0xffff;
            previousStep = 0xffff;
            pressedMask = PAD_ALL_RELEASED;
            switch (stepIndex) {
                case 0:
                    facing.y = -1024;
                    break;
                case 1:
                    facing.y = 0;
                    break;
                case 2:
                    facing.y = 1024;
                    break;
                case 3:
                    facing.y = 2048;
                    break;
            }
            for (danceIndex = 0; danceIndex < dancerCount; danceIndex++) {
                dancers[danceIndex]->SetRotation(facing);
                dancers[danceIndex]->PushStep(DG_STEP_DANCE, 0, g_dgStepDefaultPos, 13);
            }
            for (tambSlot = 0; tambSlot < tambourineCount; tambSlot++)
                tambourineGhosts[tambSlot]->PushStep(DG_STEP_POSE_KILL, 0, g_dgDefaultPos, 0);
            if (!eventCode) {
                /* cast kept: HandleMessage's arg is a void *: this message passes a number in it */
                g_pWolf->HandleMessage(this, MSG_WOLF_SET5_TURN, (void *)((s32)facing.y | 0x80000000));
                g_pWolf->HandleMessage(this, MSG_WOLF_DISABLE_PROMPT, 0);
            }
            break;
        case DGMGR_ST_PLACE_WOLF: {
            s32 stepArgument;
            if (advanceStep)
                stepIndex++;
            SetGameFlags(GF_TRANSITION_IDLE);
            if (!externalFreeze && wolfFrozen)
                wolfFrozen = ~g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0) & 1;
            beatTimeMs = g_gameTimeMs;
            split = dancerCount >> 1;
            stepCenter.x = (stepBoxes[stepIndex]->min[0] + stepBoxes[stepIndex]->max[0]) / 2;
            stepCenter.y = stepBoxes[stepIndex]->max[1];
            stepCenter.z = (stepBoxes[stepIndex]->min[2] + stepBoxes[stepIndex]->max[2]) / 2;
            for (danceIndex = 0; danceIndex < dancerCount; danceIndex++) {
                if (stepIndex & 1) {
                    delta.y = 0;
                    delta.z = 0;
                    if (danceIndex < split)
                        delta.x = (danceIndex + 1) * 150;
                    else
                        delta.x = -(danceIndex - split + 1) * 150;
                } else {
                    delta.x = 0;
                    delta.y = 0;
                    if (danceIndex < split)
                        delta.z = (danceIndex + 1) * 150;
                    else
                        delta.z = -(danceIndex - split + 1) * 150;
                }
                points[danceIndex].x = stepCenter.x;
                points[danceIndex].y = stepCenter.y;
                points[danceIndex].z = stepCenter.z;
                points[danceIndex].x += delta.x;
                points[danceIndex].y += delta.y;
                points[danceIndex].z += delta.z;
            }
            stepArgument = 0;
            for (tambSlot = 0; tambSlot < tambourineCount; tambSlot++)
                tambourineGhosts[tambSlot]->PushStep(DG_STEP_POSE_STANDV2, 0, g_dgDefaultPos, 0);
            for (danceIndex = 0; danceIndex < dancerCount; danceIndex++) {
                if (advanceStep && stepIndex) {
                    stepArgument = stepIndex * 2 - 1;
                    dancers[danceIndex]->PushStep(DG_STEP_FOLLOW_PATH, 0, g_dgStepDefaultPos, stepArgument);
                }
                dancers[danceIndex]->PushStep(DG_STEP_WALK_TO, 0, points[danceIndex], 30);
            }
            g_pWolf->HandleMessage(this, MSG_WOLF_SET5_TURN, 0);
            /* cast kept: HandleMessage's arg is a void *: this message passes a number in it */
            g_pWolf->HandleMessage(this, MSG_WOLF_DISABLE_PROMPT, (void *)1);
            break;
        }
        case DGMGR_ST_ROUND_END:
            ClearGameFlags(GF_TRANSITION_IDLE);
            beatTimeMs = g_gameTimeMs;
            for (tambSlot = 0; tambSlot < tambourineCount; tambSlot++)
                tambourineGhosts[tambSlot]->PushStep(DG_STEP_POSE_STANDV2, 0, g_dgDefaultPos, 0);
            if (stepIndex + 1 >= 4) {
                Vec3s circlePoint;
                circlePoint.x = 0;
                circlePoint.y = 0;
                circlePoint.z = 0;
                for (danceIndex = 0; danceIndex < dancerCount; danceIndex++) {
                    circlePoint.x = g_pWolf->pos.x;
                    circlePoint.y = g_pWolf->pos.y;
                    circlePoint.z = g_pWolf->pos.z;
                    circlePoint.x += 150;
                    circlePoint.z += 150;
                    RotateAroundInline(circlePoint, g_pWolf->pos, (danceIndex << 12) / dancerCount);
                    dancers[danceIndex]->PushStep(DG_STEP_WALK_TO, 0, circlePoint, 50);
                }
            }
            for (danceIndex = 0; danceIndex < dancerCount; danceIndex++)
                dancers[danceIndex]->PushStep(DG_STEP_APPLAUD, 0, g_dgDefaultPos, 0);
            if (!externalFreeze && stepIndex + 1 >= 4)
                dancers[0]->PushStep(DG_STEP_FINAL_LINE, 0, g_dgDefaultPos, 0);
            if (!externalFreeze && stepIndex + 1 < 4)
                wolfFrozen = g_pWolf->HandleMessage(this, MSG_FREEZE, 0);
            break;
        case DGMGR_ST_COMPLETE:
            danceCompleted = 1;
            SetGameFlags(GF_TRANSITION_IDLE);
            for (tambSlot = 0; tambSlot < tambourineCount; tambSlot++)
                tambourineGhosts[tambSlot]->PushStep(DG_STEP_POSE_STANDV2, 0, g_dgDefaultPos, 0);
            for (danceIndex = 0; danceIndex < dancerCount; danceIndex++) {
                targetHome.x = (dancers[danceIndex]->chatBox->min[0] + dancers[danceIndex]->chatBox->max[0]) / 2;
                targetHome.y = dancers[danceIndex]->chatBox->max[1];
                targetHome.z = (dancers[danceIndex]->chatBox->min[2] + dancers[danceIndex]->chatBox->max[2]) / 2;
                dancers[danceIndex]->PushStep(DG_STEP_WALK_TO, 0, targetHome, 50);
                /* cast kept: PushStep's argument is a u32: this step passes the dancer array's address in it */
                dancers[danceIndex]->PushStep(DG_STEP_PAIR_DANCE, 0, g_dgStepDefaultPos, (u32)dancers);
            }
            g_pWolf->HandleMessage(this, MSG_WOLF_SET5_TURN, 0);
            /* cast kept: HandleMessage's arg is a void *: this message passes a number in it */
            g_pWolf->HandleMessage(this, MSG_WOLF_DISABLE_PROMPT, (void *)1);
            break;
    }
}

ScnObject *DancingGhostManager_Create(void *record)
{
    u16 k, j, dancerIndex;
    DancingGhostManager *result = new DancingGhostManager;
    /* cast kept: Init returns the ScnObject * base of this object */
    result = (DancingGhostManager *)result->Init(record);
    for (j = 0; j < 4; j++)
        result->prayingGhosts[j] = 0;
    for (dancerIndex = 0; dancerIndex < 8; dancerIndex++)
        result->dancers[dancerIndex] = 0;
    for (k = 0; k < 8; k++)
        result->tambourineGhosts[k] = 0;
    result->stepIndex = -1;
    result->padMask = PAD_ALL_RELEASED;
    result->pressedMask = PAD_ALL_RELEASED;
    result->cineDone = 0;
    result->cineTriggered = 0;
    result->nearestGhost = 0;
    result->secondNearestGhost = 0;
    result->nearestDistance = 65535;
    result->beatElapsedMs = 0;
    result->eventCode = DANCE_EV_NONE;
    result->externalFreeze = 0;
    result->wolfFrozen = 0;
    result->unk128 = 0;
    result->reactionQueued = 0;
    result->roundArmed = 0;
    result->danceCompleted = 0;
    return result;
}

/* ---- COMDAT 0x4b2760: not inlined in HandleBeatResult (budget), so emitted out of line */
/* BYTES(inline): inline member: HandleBeatResult's /Ob1 budget runs out before these calls, so it is emitted as the COMDAT at 0x4b2760 */
inline void DancingGhost::ClearGhostQueue()
{
    u16 i;
    for (i = 0; i < 6; i++) {
        steps[i].step = 0;
        steps[i].partner = 0;
        steps[i].argument = 0;
        steps[i].point.x = g_dgDefaultPos.x;
        steps[i].point.y = g_dgDefaultPos.y;
        steps[i].point.z = g_dgDefaultPos.z;
    }
    queueCount = 0;
    elapsedMs = 0;
    newStep = 1;
    stepDone = 0;
}
