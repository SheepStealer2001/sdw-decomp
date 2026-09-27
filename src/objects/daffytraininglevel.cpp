/* T074 - original object DaffyTrainingLevel.cpp (guessed name): .text 0x43d5c0-0x440f30, .rdata 0x574dec-0x574e10
 * (vtable), .data 0x57a728-0x57a800 (8-aligned: the cinematic header's stride-table copy, padding, g_trainingHelpProps),
 * .bss 0x6cc878-0x6cc87c (g_trainingEmptyText). The class factory DaffyTrainingLevel_Create comes first, where the
 * original has it (0x43d5c0 is the object's first code).
 * PAL PC DaffyTrainingLevel. Canonical fields and shared data symbols. */
/* BYTES: dead-code, layout, temp. */
/* BYTES(layout): the Cine.h header static: every object including the cinematic header carries this copy in its .data, referenced or not */
/* BYTES(layout): its 200-byte size gives the section 8-byte alignment, as in the exe */

#define SDW_MEMBERS_Box                                                                                  \
    s32 Contains(Vec3s *p)                                                                               \
    {                                                                                                    \
        return p->x >= min[0] && p->x <= max[0] && p->y >= min[1] && p->y <= max[1] && p->z >= min[2] && \
               p->z <= max[2];                                                                           \
    }                                                                                                    \
    s32 ContainsXZ(Vec3s *p)                                                                             \
    {                                                                                                    \
        return p->x >= min[0] && p->x <= max[0] && p->z >= min[2] && p->z <= max[2];                     \
    }
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
#include "../engine/scn_tools.h"
#include "rocket.h"
#include "../engine/cine.h"
#include "../app/app_main.h"
#include "camera.h"
#include "../engine/id_list.h"
#include "animation.h"
#include "../engine/draw2d.h"
#include "../engine/text.h"
#include "../engine/input.h"
#include "../engine/interface.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *, Animator *, u16, u32);
#define SDW_MEMBERS_ScnObject                              \
    static void *operator new(u32);                        \
    void PlayVoice(u32 id)                                 \
    {                                                      \
        Voice_PlayStream(id, this);                        \
    }                                                      \
    void SetFacing(s16 value);                             \
    s32 IsLive()                                           \
    {                                                      \
        return IsInWorld() && !InstFlags(INST_F_ATTACHED); \
    }                                                      \
    void SetUpdateMode(s32 mode);


#define SDW_MEMBERS_Cine            \
    s32 IsCaptured()                \
    {                               \
        return cameraCaptured == 1; \
    }
#define SDW_MEMBERS_DaffyTrainingLevel \
    Shadow *GetShadow()                \
    {                                  \
        return &shadow;                \
    }                                  \
    void StartCamera(u16 rx, u16 ry, u16 rz, Vec3s *at, u16 focal);
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_RECORD 1
#define SDW_INLINE_SCNOBJECT_SETFACING_S16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_RECORD
#undef SDW_INLINE_SCNOBJECT_SETFACING_S16
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_GETFACING 1
#define SDW_INLINE_SCNOBJECT_INSTFLAGS_U16 1
#define SDW_INLINE_SCNOBJECT_ISINWORLD 1
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_GETFACING
#undef SDW_INLINE_SCNOBJECT_INSTFLAGS_U16
#undef SDW_INLINE_SCNOBJECT_ISINWORLD
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32
#define SDW_INLINE_SCNBODY_GETANIMID 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_GETANIMID
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_SHADOW_SETVISIBLE_S32 1
#include "../engine/shadow_inlines.h"
#undef SDW_INLINE_SHADOW_SETVISIBLE_S32
#define SDW_INLINE_SHADOW_REPROJECT 1
#include "../engine/shadow_inlines.h"
#undef SDW_INLINE_SHADOW_REPROJECT
#define SDW_INLINE_CINE_ISACTIVE 1
#include "../engine/cine_inlines.h"
#undef SDW_INLINE_CINE_ISACTIVE
/* 0x57a728 - the cinematic header's static copy of the 9-byte opcode stride table (src/engine/cine.cpp,
 * g_cineOpStride 0x5816fc). The original header defined it static, so every object including it carries its own
 * unreferenced copy at the head of its .data; defined here in its place. */
static u8 s_cineOpStride[9] = {0, 8, 8, 4, 2, 2, 4, 2, 2};
/* 0x57a738 - per help slot, the property offsets of: text id, box id list, path id list, type, voice. The 200-byte
 * array makes VC6 give the .data section 8-byte alignment, which puts it at +0x10 after the 12-byte copy.
 * The slots are the designer's TEXT_3, TEXT_2, TEXT_1, TEXT00..TEXT16 (with BOX / TRAJECTORY / TYPE / VOICE of the
 * same suffix). TRAINING_PROP is offsetof written out (there is no CRT header); in a static initialiser VC6 folds
 * it to the constant, so the .data bytes are the plain offsets 144, 8, 224, 304, 384, ... */
/* cast kept: offsetof: a property's byte offset in the props block */
#define TRAINING_PROP(field) ((u16)(u32) & ((DaffyTrainingLevelProps *)0)->field)
#define TRAINING_HELP_SLOT(n)                                                                             \
    {TRAINING_PROP(TEXT##n), TRAINING_PROP(BOX##n), TRAINING_PROP(TRAJECTORY##n), TRAINING_PROP(TYPE##n), \
     TRAINING_PROP(VOICE##n)}
u16 g_trainingHelpProps[20][5] = {
    TRAINING_HELP_SLOT(_3), TRAINING_HELP_SLOT(_2), TRAINING_HELP_SLOT(_1), TRAINING_HELP_SLOT(00),
    TRAINING_HELP_SLOT(01), TRAINING_HELP_SLOT(02), TRAINING_HELP_SLOT(03), TRAINING_HELP_SLOT(04),
    TRAINING_HELP_SLOT(05), TRAINING_HELP_SLOT(06), TRAINING_HELP_SLOT(07), TRAINING_HELP_SLOT(08),
    TRAINING_HELP_SLOT(09), TRAINING_HELP_SLOT(10), TRAINING_HELP_SLOT(11), TRAINING_HELP_SLOT(12),
    TRAINING_HELP_SLOT(13), TRAINING_HELP_SLOT(14), TRAINING_HELP_SLOT(15), TRAINING_HELP_SLOT(16)};
/* 0x6cc878 - the empty line shown for an out-of-range help slot */
char g_trainingEmptyText[4];
extern Wolf *g_pWolf;

extern u32 g_gameTime;

extern s32 g_gameTimeMs;

extern "C" s16 g_sinTable4096[5122];
#include "../sdk/crt.h"
extern "C" s16 Math_RadiansToAngle4096(float);
void Camera_StartScripted(ScnObject *, Camera *, u16, u16, u16, Vec3s *, u16, u32, s32);
s32 Scenaric_FindByClass(u16, ScnObject **, s32);
extern u32 *g_screenLayerBase;

#define g_padMasks (g_inputMap + 4)

#define g_padCurButtons (g_pad.cur.buttons)

#define g_padPrevButtons (g_pad.prev.buttons)

u16 Str_Length(const char *), Text_CountWrappedLines(const char *);
void Ui_DrawTextBox(TextBox *, u16);
void Dialogue_Reset(), Scenaric_Stub_515eef();
s32 ObjGrid_QueryBoxPoints(const CollBox *, ScnObject **);
/* The dialogue may run when no cinematic plays, or when the playing one has captured the camera. */
#define CINE_TALK(first) ((first) || (g_cinePlayer.IsActive() && g_cinePlayer.IsCaptured()))
#define ANGLE_WRAP(a) ((s16)((s16)(((a) + 0x800) & 0xfff) - 0x800))
#define ABS(x) ((x) >= 0 ? (x) : -(x))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
/* Same dimensions-as-inline-values idiom as engine/interface.cpp and daffylevel01.cpp. */
#define SDW_INLINE_FREE_SCREENWIDTHU16 1
#include "../engine/screen_inlines.h"
#undef SDW_INLINE_FREE_SCREENWIDTHU16
#define SDW_INLINE_FREE_SCREENHEIGHTU16 1
#include "../engine/screen_inlines.h"
#undef SDW_INLINE_FREE_SCREENHEIGHTU16
#define SDW_INLINE_FREE_INDEXEDPROP_VOID_U16 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_INDEXEDPROP_VOID_U16
/* BYTES(temp, inferred): the offset and the result go through the read struct because the original's expansion keeps both in stack slots */
#define SDW_INLINE_FREE_PROPU32_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_PROPU32_VOID_U32
inline void DaffyTrainingLevel::StartCamera(u16 rx, u16 ry, u16 rz, Vec3s *at, u16 focal)
{
    Camera_StartScripted(this, &g_camera, rx, ry, rz, at, focal, CAMSCR_BLEND_IN | CAMSCR_BLEND_OUT, 0x1000);
}

ScnObject *DaffyTrainingLevel_Create(void *record)
{
    ScnBody *object = new DaffyTrainingLevel;
    object = object->Init(record, 0);
    return object;
}
void DaffyTrainingLevel::PlayPendingVoice()
{
    if (pendingVoice && currentText)
        PlayVoice(pendingVoice);
    pendingVoice = 0;
}
void DaffyTrainingLevel::SetHelpLine(u32 slot)
{
    void *properties = Record();
    if (slot >= 20) {
        currentText = g_trainingEmptyText;
        pendingVoice = 0;
    } else {
        currentText = Text_GetClassString((u8)helpTextIds[slot]);
        pendingVoice = IndexedProp(properties, g_trainingHelpProps[slot][4]);
    }
}
void DaffyTrainingLevel::SetLine(u8 text, u32 voice)
{
    currentText = Text_GetClassString(text);
    pendingVoice = voice;
}
s32 DaffyTrainingLevel::EatTextTag(u32 tag)
{
    if (currentText[0] == '$' && currentText[5] == '$' && currentText[1] == ((tag >> 24) & 255) &&
        currentText[2] == ((tag >> 16) & 255) && currentText[3] == ((tag >> 8) & 255) &&
        currentText[4] == (tag & 255)) {
        currentText += 6;
        return 1;
    }
    return 0;
}
void DaffyTrainingLevel::ParseTextTags()
{
    lockTag = EatTextTag(DTL_TAG_LOCK);
    viewTag = EatTextTag(DTL_TAG_VIEW);
    if (EatTextTag(DTL_TAG_STOP))
        stopTag = 1;
    else
        stopTag = 0;
    if (EatTextTag(DTL_TAG_CONF))
        confirmTag = DTL_CONFIRM_AFTER_LINE;
    else
        confirmTag = DTL_CONFIRM_NONE;
}

/* 0x43d872: walk / hop / jump toward dst; returns the remaining distance. */
u32 DaffyTrainingLevel::MoveTo(Vec3s dst, s32 speed, u8 gait)
{
    s32 overrun;
    u32 range;
    Vec3s frac;
    u32 clock;
    s32 drop;
    s32 toZ;
    Vec3s travel;
    Vec3s cursor;
    s32 gapX;
    u16 sinIndex;
    currentTime = g_gameTime * 1000 >> 12;
    gapX = dst.x - pos.x;
    drop = dst.y - pos.y;
    toZ = dst.z - pos.z;
    if ((gapX || toZ) && (gait != DTL_GAIT_JUMP || helpIndex >= 3))
        SetFacing((Math_RadiansToAngle4096((float)atan2(gapX, toZ)) + 0x800) & 0xfff);
    range = (u32)sqrt((double)gapX * gapX + toZ * toZ);
    if (gait != this->gait || dst.x != target.x || dst.z != target.z) {
        this->gait = gait;
        lastTargetTime = currentTime;
        clock = 0;
        jumpDuration = 400;
        target.x = dst.x;
        target.z = dst.z;
        if (gait != DTL_GAIT_WARP_A && gait != DTL_GAIT_WARP_B) {
            if (trajNodeIndex)
                SetPosition(&helpSlot[helpIndex].trajectory->pts[trajNodeIndex - 1]);
            else
                SetPosition(&helpSlot[helpIndex].trajectory->pts[0]);
            SnapToGround(0);
        }
        moveStart = pos;
        switch (gait) {
            case DTL_GAIT_WALK:
                if (GetAnimId() != AROBIN01_ANIM_WALK1)
                    PlayAnim(AROBIN01_ANIM_WALK1, 1, 0);
                break;
            case DTL_GAIT_WALK2:
                if (GetAnimId() != AROBIN01_ANIM_WALK2)
                    PlayAnim(AROBIN01_ANIM_WALK2, 1, 0);
                break;
            case DTL_GAIT_JUMP:
                if (range < 50 && initFlag == DTL_JUMP_TAKEOFF) {
                    SetPosition(&dst);
                    SnapToGround(0);
                    movementDone = 1;
                    return range = 0;
                }
                movementDone = 0;
                if (helpIndex < 3) {
                    jumpDuration = Anim_GetDurationMs(Inst(), AROBIN01_ANIM_HIDE2, 0);
                    SetFacing((HeadingTo(&dst) - 0x400) & 0xfff);
                    PlayAnim(AROBIN01_ANIM_HIDE2, 0, 0);
                } else
                    PlayAnim(AROBIN01_ANIM_JUMP3, 0, 1);
                break;
            default:
                PlayAnim(AROBIN01_ANIM_STAND1, 1, 0);
        }
    } else {
        clock = currentTime - lastTargetTime;
        if (clock > 10000) {
            SetPosition(&dst);
            SnapToGround(0);
        }
    }
    switch (gait) {
        case DTL_GAIT_WALK:
        case DTL_GAIT_WALK2:
            if (!range)
                range = 1;
            frac.x = (s16)(gapX * speed / (s32)range);
            frac.y = (s16)(drop * speed / (s32)range);
            frac.z = (s16)(toZ * speed / (s32)range);
            Vec3s_ScaleByDt(&frac, &travel);
            Translate(&travel);
            break;
        case DTL_GAIT_JUMP:
            if (helpIndex < 3) {
                if (AnimFlags(ANIM_F_FINISHED)) {
                    SetPosition(&dst);
                    SnapToGround(0);
                    movementDone = 1;
                } else {
                    drop = dst.y - moveStart.y;
                    frac.x = 0;
                    frac.z = 0;
                    frac.y = (s16)(drop * (s32)clock / (s32)jumpDuration);
                    cursor.x = moveStart.x + frac.x;
                    cursor.y = moveStart.y + frac.y;
                    cursor.z = moveStart.z + frac.z;
                    SetPosition(&cursor);
                }
            } else {
                overrun = clock - jumpDuration;
                if (initFlag != DTL_JUMP_TAKEOFF && initFlag != DTL_JUMP_LAND) {
                    gapX = dst.x - moveStart.x;
                    drop = dst.y - moveStart.y;
                    toZ = dst.z - moveStart.z;
                    frac.x = (s16)(gapX * (s32)clock / (s32)jumpDuration);
                    frac.y = (s16)(drop * (s32)clock / (s32)jumpDuration);
                    frac.z = (s16)(toZ * (s32)clock / (s32)jumpDuration);
                    sinIndex = (u16)((clock << 11) / jumpDuration & 0xfff);
                    frac.y -= (s16)((u32)(g_sinTable4096[sinIndex] * 100) >> 12);
                    cursor.x = moveStart.x + frac.x;
                    cursor.y = moveStart.y + frac.y;
                    cursor.z = moveStart.z + frac.z;
                    SetPosition(&cursor);
                }
                switch (initFlag) {
                    case DTL_JUMP_TAKEOFF:
                        if (GetAnimId() != AROBIN01_ANIM_JUMP3)
                            PlayAnim(AROBIN01_ANIM_JUMP3, 0, 1);
                        if (AnimFlags(ANIM_F_FINISHED)) {
                            lastTargetTime = currentTime;
                            initFlag++;
                        }
                        break;
                    case DTL_JUMP_RISE:
                        if (GetAnimId() != AROBIN01_ANIM_JUMP4)
                            PlayAnim(AROBIN01_ANIM_JUMP4, 0, 1);
                        if (sinIndex > 0x400)
                            initFlag++;
                        break;
                    case DTL_JUMP_FALL:
                        if (GetAnimId() != AROBIN01_ANIM_JUMP5)
                            PlayAnim(AROBIN01_ANIM_JUMP5, 0, 1);
                        if (range <= 50 || overrun > 0) {
                            SetPosition(&dst);
                            SnapToGround(0);
                            if (trajNodeIndex != helpSlot[helpIndex].trajectory->count - 1)
                                movementDone = 1;
                            else
                                initFlag++;
                        }
                        break;
                    default:
                        if (GetAnimId() != AROBIN01_ANIM_JUMP6)
                            PlayAnim(AROBIN01_ANIM_JUMP6, 0, 1);
                        if (AnimFlags(ANIM_F_FINISHED))
                            movementDone = 1;
                }
            }
            if (movementDone) {
                initFlag = DTL_JUMP_TAKEOFF;
                SnapToGround(0);
            }
            break;
        case DTL_GAIT_WARP_A:
        case DTL_GAIT_WARP_B:
            SetPosition(&dst);
            range = 0;
            break;
        default:
            range = 0;
    }
    return range;
}

/* 0x43e282 */
void DaffyTrainingLevel::PostLoadInit()
{
    void *properties;
    s32 j;
    s32 numResolved;
    u32 i;
    u16 count;
    u32 *boxIds;
    u32 *path;
    properties = record;
    initFlag = DTL_JUMP_TAKEOFF;
    Scenaric_FindByClass(CLASSID_WATCH, &watch, 1);
    SetMode(DTL_MODE_IDLE, DTL_WATCH_OFF);
    pendingVoice = 0;
    numResolved = 0;
    helpCount = 0;
    i = 0;
    fromPreviousMask = 0;
    wolfMoved = 0;
    while (i < 20) {
        fromPreviousMask = PropU32(properties, 0x5c);
        helpTextIds[helpCount] = IndexedProp(properties, g_trainingHelpProps[i][0]);
        boxIds = Scn_FindIdList((u16)IndexedProp(properties, g_trainingHelpProps[i][1]), &count);
        helpSlot[helpCount].type = IndexedProp(properties, g_trainingHelpProps[i][3]);
        path = Scn_FindIdList((u16)IndexedProp(properties, g_trainingHelpProps[i][2]), &count);
        if (path) {
            if (!boxIds)
                helpBox[helpCount] = 0;
            else
                helpBox[helpCount] = (Box *)*boxIds; /* cast kept: an id list holds record addresses as u32 */
            /* cast kept: an id list holds record addresses as u32 */
            helpSlot[helpCount].trajectory = (Trajectory *)*path;
            helpCount++;
            numResolved++;
        }
        i++;
    }
    startBox = Scn_GetPropBox(properties, 0x7c);
    finishBox = Scn_GetPropBox(properties, 0x80);
    challengeDuration = PropU32(properties, 0x84) * 100;
    cameras = Scn_GetPropObject(properties, 0x50);
    if (cameras) {
        j = 0;
        while (j < 2) {
            cameraFlags[j] = 0;
            j++;
        }
        initNotify = 1;
    } else
        initNotify = 0;
    cinematics = Scn_GetPropObject(properties, 0x54);
    helpIndex = 0;
    trajNodeIndex = 0;
    MoveTo(helpSlot[0].trajectory->pts[0], 0, DTL_GAIT_WARP_A);
    wolfFrozen = 0;
    SetUpdateMode(SCN_UPD_ALWAYS);
    SetUpdateMode(SCN_UPD_CINE);
    lineFlags.helpPending = 0;
    confirmTag = DTL_CONFIRM_NONE;
    replayingHelp = 0;
    currentText = 0;
    retryPhase = 0;
    useLatched = 0;
    stopTag = 0;
    helpVoice = PropU32(properties, 0x6c);
    jokeVoice1 = PropU32(properties, 0x74);
    jokeVoice2 = PropU32(properties, 0x78);
    jokeEnabled = PropU32(properties, 0x70);
    viewTag = 0;
    viewStart = g_gameTime;
    lockTag = 0;
    dialogueRestart = 0;
}

/* 0x43e7f6 */
void DaffyTrainingLevel::Reset()
{
    u32 voice;
    Voice_StopStream(this);
    replayingHelp = 0;
    lineFlags.helpPending = 1;
    animPhase = 0;
    if (helpIndex >= 0 && helpIndex < helpCount)
        stopTag = !((fromPreviousMask >> helpIndex) & 1);
    else
        stopTag = 1;
    if ((helpIndex == 13 || helpIndex == 14) && (watchState == DTL_WATCH_DONE || watchState == DTL_WATCH_OFF))
        wolfMoved = 1;
    else
        wolfMoved = 0;
    if (helpSlot[helpIndex].type > 10 || helpIndex == 10) {
        retryPhase++;
        if (retryPhase > 1)
            retryPhase = 1;
    } else
        retryPhase = 0;
    if (!stopTag) {
        trajNodeIndex = 0;
        if (helpIndex > 0) {
            if (helpIndex == 13)
                helpIndex -= 2;
            else
                helpIndex -= 1;
        } else
            helpIndex = helpCount - 1;
        MoveTo(helpSlot[helpIndex].trajectory->pts[trajNodeIndex], 0, DTL_GAIT_WARP_A);
        SnapToGround(0);
    }
    if (helpIndex >= 18 || (helpIndex == 11 && !lineFlags.jokeDone))
        SetHelpLine(helpIndex);
    else if (retryPhase <= 1) {
        voice = 0;
        if (jokeEnabled & (1 << helpIndex)) {
            if (retryPhase != 0 && helpIndex == 9)
                voice = jokeVoice1;
            else if (retryPhase != 0 || helpIndex == 11)
                voice = jokeVoice2;
            else
                voice = jokeVoice1;
        }
        if (helpIndex != 13)
            SetLine(helpTextIds[helpIndex] + 1, voice);
        else
            SetHelpLine(helpIndex);
    } else
        SetLine(helpTextIds[helpIndex] + 2, 0);
    SetMode(DTL_MODE_IDLE, DTL_WATCH_OFF);
    dialogueRestart = 0;
    cameraNotify = 1;
}

/* 0x43eb56: mode 0 idle, 1 talking, 2 walking to the next help point; 0xff leaves a field alone. */
void DaffyTrainingLevel::SetMode(u8 newMode, u8 newWatchState)
{
    if (newMode != DTL_MODE_KEEP)
        mode = newMode;
    if (newWatchState != DTL_WATCH_KEEP)
        watchState = newWatchState;
    if (newMode != DTL_MODE_KEEP) {
        switch (mode) {
            case DTL_MODE_IDLE:
                GetShadow()->SetVisible(1);
                initFlag = DTL_JUMP_TAKEOFF;
                forcedFreeze = 0;
                Camera_ReleaseScripted(this);
                if (wolfFrozen && !lockTag) {
                    g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0);
                    g_pWolf->HandleMessage(this, MSG_WOLF_ALLOW_LOOK, 0);
                    wolfFrozen = 0;
                }
                if (!g_cinePlayer.IsActive()) {
                    PlayAnim(AROBIN01_ANIM_STAND1, 1, 1);
                    Voice_StopStream(this);
                    robotSequence = 0;
                    robot = 0;
                }
                break;
            case DTL_MODE_TALK:
                savedWolfPos.x = g_pWolf->pos.x;
                savedWolfPos.y = g_pWolf->pos.y;
                savedWolfPos.z = g_pWolf->pos.z;
                if (helpIndex > 3) {
                    GetShadow()->SetVisible(1);
                    UpdateShadow();
                }
                forcedFreeze = 0;
                lastSpeechMs = g_gameTimeMs;
                if (!wolfFrozen && g_pWolf->HandleMessage(this, MSG_FREEZE, 0))
                    wolfFrozen = 1;
                break;
            case DTL_MODE_WALK:
                animPhase = 0;
                if (helpIndex <= 3) {
                    GetShadow()->SetVisible(0);
                    StartCamera(g_camera.rot.x, g_camera.rot.y, g_camera.rot.z, &g_camera.pos, 0x276);
                }
                if (!forcedFreeze && wolfFrozen == 1 && !lockTag) {
                    g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0);
                    g_pWolf->HandleMessage(this, MSG_WOLF_ALLOW_LOOK, 0);
                    wolfFrozen = 0;
                }
                retryPhase = 0;
                replayingHelp = 0;
                Voice_StopStream(this);
                break;
        }
    }
}

/* 0x43ef00 */
/* BYTES(dead-code): prevPos is never used and restAnim is set and never read: slots / stores the original has */
void DaffyTrainingLevel::Update()
{
    u32 next;
    Vec3s at;
    u16 rotY;
    s32 m;
    ScnObject *candidates[64];
    s32 found;
    Vec3s prevPos;
    s32 noticed;
    Vec3s bearing;
    s16 dist2d;
    Vec3s fromCam;
    u16 restAnim;
    restAnim = AROBIN01_ANIM_STAND1;
    if (initNotify) {
        if (cameras)
            cameras->HandleMessage(this, MSG_CAMMGR_ENABLE_ONLY_BOX, 0);
        initNotify = 0;
    }
    if (wolfMoved) {
        g_pWolf->SetPosition(&savedWolfPos);
        g_pWolf->SnapToGround(0);
        wolfMoved = 0;
    }
    switch (mode) {
        case DTL_MODE_IDLE:
            if (!g_cinePlayer.IsActive())
                SetFacing(HeadingTo(&g_pWolf->pos));
            else if (helpIndex == 13) {
                if (!lineFlags.helpPending) {
                    SetHelpLine(15);
                    SetMode(DTL_MODE_TALK, DTL_WATCH_KEEP);
                    ParseTextTags();
                } else
                    SetHelpLine(13);
            }
            lineFlags.helpPending = 0;
            if (helpIndex == 17 && g_pRocket && !g_pRocket->IsLive()) {
                SetMode(DTL_MODE_WALK, DTL_WATCH_KEEP);
                break;
            }
            if (!currentText)
                SetHelpLine(helpIndex);
            if (helpCount > 0 && helpIndex != -1) {
                if (!helpBox[helpIndex])
                    SetMode(DTL_MODE_WALK, DTL_WATCH_KEEP);
                else {
                    if (viewTag) {
                        fromCam.x = pos.x - g_camera.pos.x;
                        fromCam.y = pos.y - g_camera.pos.y;
                        fromCam.z = pos.z - g_camera.pos.z;
                        dist2d = (s16)sqrt((double)fromCam.x * fromCam.x + fromCam.z * fromCam.z);
                        bearing.x = Math_RadiansToAngle4096((float)atan2(fromCam.y, dist2d)) & 0xfff;
                        bearing.y = -Math_RadiansToAngle4096((float)atan2(fromCam.x, fromCam.z)) & 0xfff;
                        bearing.z = 0;
                        noticed = 0;
                        if (ABS(ANGLE_WRAP(g_camera.rot.x - bearing.x)) < 250 &&
                            ABS(ANGLE_WRAP(g_camera.rot.y - bearing.y)) < 375) {
                            if (g_gameTime - viewStart >= 0x1000)
                                noticed = 1;
                        } else
                            viewStart = g_gameTime;
                    }
                    if (helpBox[helpIndex]->Contains(&g_pWolf->pos) || (viewTag && noticed)) {
                        if (viewTag) {
                            viewTag = 0;
                            /* cast kept: arg carries a number */
                            g_pWolf->HandleMessage(this, MSG_WOLF_ALLOW_LOOK, (void *)1);
                        }
                        if (*currentText) {
                            if (!g_pWolf->HandleMessage(this, MSG_WOLF_IS_FLYING, 0)) {
                                SetMode(DTL_MODE_TALK, DTL_WATCH_KEEP);
                                ParseTextTags();
                            }
                        } else
                            SetMode(DTL_MODE_WALK, DTL_WATCH_KEEP);
                    }
                }
            }
            if (cameras && cameraNotify)
                cameras->HandleMessage(this, MSG_CAMMGR_ENABLE_ONLY_BOX, helpBox[helpIndex]);
            else
                cameraNotify = 0;
            break;
        case DTL_MODE_TALK:
            SetFacing(HeadingTo(&g_pWolf->pos));
            if (!wolfFrozen && *currentText && g_pWolf->HandleMessage(this, MSG_FREEZE, 0) == 1)
                wolfFrozen = 1;
            if (helpIndex > 10)
                lineFlags.jokeDone = 1;
            if (watchState && watchState != DTL_WATCH_DONE) {
                watch->HandleMessage(this, MSG_WATCH_TICK, 0);
                watchState = DTL_WATCH_OFF;
            }
            if (confirmTag != DTL_CONFIRM_ASKING) {
                if (g_gameTimeMs - lastSpeechMs >= 0x5dc && !g_cinePlayer.IsActive()) {
                    if (retryPhase == 1) {
                        switch (animPhase) {
                            case 0:
                                PlayAnim(AROBIN01_ANIM_JOKE, 1, 1);
                                animPhase++;
                                break;
                            default:
                                if (AnimFlags(ANIM_F_FINISHED))
                                    PlayAnim(AROBIN01_ANIM_TALK1, 1, 1);
                        }
                    } else {
                        if (g_pSoundSystem && g_pSoundSystem->GetVoiceAmplitude(1)) {
                            if (GetAnimId() != AROBIN01_ANIM_TALK1)
                                PlayAnim(AROBIN01_ANIM_TALK1, 1, 1);
                        } else if (GetAnimId() != AROBIN01_ANIM_STAND1A)
                            PlayAnim(AROBIN01_ANIM_STAND1A, 1, 1);
                        Scenaric_Stub_515eef();
                    }
                }
                if (CINE_TALK(!g_cinePlayer.IsActive()) || helpIndex == 13 || helpIndex == 14 || helpIndex == 16) {
                    if (currentText && *currentText == '$') {
                        PlayPendingVoice();
                        if (!Dialogue_Show(currentText, 1) || dialogueRestart) {
                            dialogueRestart = 0;
                            SetHelpLine(helpIndex);
                            if (*currentText && wolfFrozen == 1) {
                                if (confirmTag == DTL_CONFIRM_AFTER_LINE)
                                    confirmTag = DTL_CONFIRM_ASKING;
                                else {
                                    SetMode(DTL_MODE_WALK, DTL_WATCH_KEEP);
                                    Voice_StopStream(this);
                                }
                            } else {
                                if (replayingHelp)
                                    SetMode(DTL_MODE_IDLE, DTL_WATCH_KEEP);
                                else
                                    SetMode(DTL_MODE_WALK, DTL_WATCH_KEEP);
                                Voice_StopStream(this);
                            }
                        } else if (CINE_TALK(0)) {
                            robotSequence = 1;
                            if (CINE_TALK(!g_cinePlayer.IsActive())) {
                                Dialogue_Reset();
                                dialogueRestart = 1;
                            } else
                                dialogueRestart = 0;
                        }
                    } else {
                        if (confirmTag == DTL_CONFIRM_AFTER_LINE)
                            confirmTag = DTL_CONFIRM_ASKING;
                        else {
                            if (replayingHelp)
                                SetMode(DTL_MODE_IDLE, DTL_WATCH_KEEP);
                            else
                                SetMode(DTL_MODE_WALK, DTL_WATCH_KEEP);
                            Voice_StopStream(this);
                        }
                    }
                }
            } else {
                confirmBox.text = Text_GetClassString(0);
                Text_SetFont(FONT_GAME);
                confirmBox.rect[2] = MIN((s16)(Str_Length(confirmBox.text) + 1) * g_pCurFont->glyphWidth,
                                         (s16)(ScreenWidthU16() / g_pCurFont->glyphWidth - 4) * g_pCurFont->glyphWidth);
                confirmBox.rect[0] = 0;
                confirmBox.rect[1] = 0;
                confirmBox.rect[3] = 0;
                Text_SetWindowRect(g_screenLayerBase + 6, confirmBox.rect, 0);
                confirmBox.rect[3] =
                    MIN((s16)(Text_CountWrappedLines(confirmBox.text) + 3) * g_pCurFont->lineHeight,
                        (s16)(ScreenHeightU16() / g_pCurFont->lineHeight - 4) * g_pCurFont->lineHeight);
                confirmBox.rect[0] = (ScreenWidthU16() - confirmBox.rect[2]) / 2;
                confirmBox.rect[1] = (ScreenHeightU16() - confirmBox.rect[3]) / 2;
                confirmBox.bgColor = 0;
                Ui_DrawTextBox(&confirmBox, 0xffff);
                if (confirmBox.fits == 1 && !(g_padCurButtons & ~g_padMasks[10]) &&
                    (g_padPrevButtons & ~g_padMasks[10])) {
                    if (confirmBox.confirmChoice == 1) {
                        useLatched = 0;
                        if (replayingHelp)
                            SetMode(DTL_MODE_IDLE, DTL_WATCH_KEEP);
                        else
                            SetMode(DTL_MODE_WALK, DTL_WATCH_KEEP);
                        Voice_StopStream(this);
                        confirmTag = DTL_CONFIRM_NONE;
                    } else {
                        useLatched++;
                        if (cinematics)
                            cinematics->HandleMessage(this, MSG_CINE_REARM, helpBox[helpIndex]);
                        currentText = 0;
                        SetMode(DTL_MODE_IDLE, DTL_WATCH_KEEP);
                        Voice_StopStream(this);
                        confirmTag = DTL_CONFIRM_NONE;
                    }
                }
            }
            break;
        case DTL_MODE_WALK:
            if (robotSequence) {
                if (!robot) {
                    /* cast kept: Box and CollBox are two views of one 16-byte record */
                    found = ObjGrid_QueryBoxPoints((CollBox *)helpBox[helpIndex], candidates);
                    for (m = 0; m < found; m++) {
                        if (candidates[m] && candidates[m]->GetClassId() == CLASSID_SIGNPOSTANIMATED &&
                            !candidates[m]->InstFlags(INST_F_DRAWN))
                            robot = candidates[m];
                    }
                    if (!robot)
                        robotSequence = 0;
                    else {
                        PlayAnim(AROBIN01_ANIM_HAMMER, 1, 0);
                        rotY = GetFacing() & 0xfff;
                        robot->SetFacing(rotY);
                        at = pos;
                        at.x -= 0x3e;
                        at.z -= 0x28;
                        robot->SetPosition(&at);
                        robot->HandleMessage(this, MSG_SIGN_REVEAL, 0);
                    }
                } else if (GetAnimId() == AROBIN01_ANIM_HAMMER && AnimFlags(ANIM_F_FINISHED))
                    robotSequence = 0;
                else
                    AdvanceAnim();
            } else {
                if (cameraNotify) {
                    cameras->HandleMessage(this, MSG_CAMMGR_ENABLE_ONLY_BOX, 0);
                    cameraNotify = 0;
                }
                if (helpIndex < 18 && !forcedFreeze) {
                    next = helpIndex + 1;
                    if (next < helpCount && helpBox[next]) {
                        if (helpBox[next]->Contains(&g_pWolf->pos) && !wolfFrozen &&
                            g_pWolf->HandleMessage(this, MSG_FREEZE, 0)) {
                            wolfFrozen = 1;
                            forcedFreeze = 1;
                        }
                    }
                }
                if (GetAnimId() == AROBIN01_ANIM_JOKE && !AnimFlags(ANIM_F_FINISHED))
                    PlayAnim(AROBIN01_ANIM_STAND1, 1, 0);
                if (helpSlot[helpIndex].type == DTL_GAIT_WARP_A || helpSlot[helpIndex].type == DTL_GAIT_WARP_B)
                    trajNodeIndex = helpSlot[helpIndex].trajectory->count - 1;
                prevPos = pos;
                if ((u8)(helpSlot[helpIndex].type % 10) == DTL_GAIT_JUMP && helpIndex < 3 && !movementDone)
                    GetShadow()->Reproject();
                if (MoveTo(helpSlot[helpIndex].trajectory->pts[trajNodeIndex], 600, helpSlot[helpIndex].type % 10) <
                        50 &&
                    ((helpSlot[helpIndex].type == DTL_GAIT_JUMP && movementDone) ||
                     helpSlot[helpIndex].type != DTL_GAIT_JUMP)) {
                    SnapToGround(0);
                    trajNodeIndex++;
                    if (trajNodeIndex >= helpSlot[helpIndex].trajectory->count) {
                        trajNodeIndex = 0;
                        helpIndex++;
                        initFlag = DTL_JUMP_TAKEOFF;
                        if (helpIndex >= helpCount) {
                            helpIndex = -1;
                            PlayAnim(AROBIN01_ANIM_STAND1, 1, 0);
                        }
                        currentText = 0;
                        if (helpSlot[helpIndex].type > 10)
                            retryPhase = 0;
                        cameraNotify = 1;
                        SetMode(DTL_MODE_IDLE, DTL_WATCH_KEEP);
                    }
                }
            }
            break;
    }
    switch (watchState) {
        case DTL_WATCH_OFF:
            if (startBox && startBox->ContainsXZ(&g_pWolf->pos))
                watchState = DTL_WATCH_READY;
            break;
        case DTL_WATCH_READY:
            savedHelpIndex = helpIndex;
            if (!startBox->Contains(&g_pWolf->pos)) {
                /* cast kept: arg carries a number */
                watch->HandleMessage(this, MSG_WATCH_START, (void *)((u32)(challengeDuration << 12) / 1000));
                watch->HandleMessage(this, MSG_WATCH_TICK, (void *)1); /* cast kept: arg carries a number */
                watchState = DTL_WATCH_RUNNING;
            }
            break;
        case DTL_WATCH_RUNNING:
            if (finishBox && finishBox->Contains(&g_pWolf->pos)) {
                watch->HandleMessage(this, MSG_WATCH_TICK, 0);
                watchState = DTL_WATCH_DONE;
            }
            if (watch->HandleMessage(this, MSG_WATCH_TIME_LEFT, 0) <= 0) {
                watch->HandleMessage(this, MSG_WATCH_TICK, 0);
                watchState = DTL_WATCH_FAILED;
                deathDelay = 0x32;
                if (g_pWolf->HandleMessage(this, MSG_FREEZE, 0) != 1 || wolfFrozen == 1)
                    /* cast kept: arg carries a number */
                    g_pWolf->HandleMessage(this, MSG_KILL, (void *)KILL_RESTART_ONLY);
            }
            break;
        case DTL_WATCH_FAILED:
            if (deathDelay-- < 0)
                /* cast kept: arg carries a number */
                g_pWolf->HandleMessage(this, MSG_KILL, (void *)KILL_RESTART_ONLY);
            break;
        case DTL_WATCH_DONE:
            break;
        default:
            watchState = DTL_WATCH_OFF;
    }
    AdvanceAnim();
}

/* 0x440d56 */
s32 DaffyTrainingLevel::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    switch (msgId) {
        case MSG_FREEZE:
            SetMode(DTL_MODE_IDLE, DTL_WATCH_KEEP);
            wolfFrozen = 0;
            return 1;
        case MSG_QUERY_ACTION:
            if (!sender->GetClassId()) {
                if (!mode && helpIndex != -1) {
                    if ((helpIndex != 17 && helpIndex != 18) || (g_pRocket && g_pRocket->IsLive()))
                        return CTX_TALK;
                } else
                    return CTX_NONE;
            }
            break;
        case MSG_USE:
            SetHelpLine((u8)(helpIndex > 0 ? helpIndex - 1 : helpIndex));
            replayingHelp = 1;
            PlayAnim(AROBIN01_ANIM_STAND1A, 1, 1);
            ParseTextTags();
            if (g_pWolf->HandleMessage(this, MSG_FREEZE, 0) == 1) {
                wolfFrozen = 1;
                useLatched = 1;
                SetMode(DTL_MODE_TALK, DTL_WATCH_KEEP);
            }
            return 1;
    }
    return 0;
}
