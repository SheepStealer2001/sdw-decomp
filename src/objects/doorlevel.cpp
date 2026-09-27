/* T135 - original object DoorLevel.cpp (guessed name).
 * Ranges: .text 0x4b3ae0-0x4b4270, .rdata 0x575e9c-0x575ec0 (vtable), .data 0x57b590-0x57b61c (the label literals,
 * g_levelLabels, then HandleMessage's format literal), .bss 0x6cf610-0x6cf614 (g_emptyLevelLabel). */
/* BYTES: slot-group. */
/* ScnObject::Text_GetClassString is declared as char *(u8), as it is defined, so the decorated names agree at link. */
/* PAL PC 0x4b3ae0-0x4b4270. Whole-function comparison includes dormant inline branches.
 * g_emptyLevelLabel is a zero-filled char[4] in .bss, used as an empty string.
 */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *instance, Animator *animator, u16 id, u32 flags);

#define SDW_MEMBERS_ScnObject            \
    static void *operator new(u32 size); \
    void SetUpdateMode(u8 mode);


#include "sdw_classes.h"
#include "../engine/progress.h"
#include "../engine/scn_tools.h"
#include "animation.h"
#include "../engine/fade.h"
#include "../engine/text.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_GETHEADING 1
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_GETHEADING
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_PROGRESS_SETSCENEEXITTARGET_S8 1
#include "../engine/progress_inlines.h"
#undef SDW_INLINE_PROGRESS_SETSCENEEXITTARGET_S8
#define SDW_INLINE_PROGRESS_GETLEVELINDEXA 1
#include "../engine/progress_inlines.h"
#undef SDW_INLINE_PROGRESS_GETLEVELINDEXA
extern u32 *g_screenLayerBase;
/* 0x57b590 .data: the level labels (B1 = disc Lvl-05, B2 = Lvl-10); the literals come first, then the array
 * (0x57b5c8) */
const char *g_levelLabels[18] = {"0", "1",  "2", "3",  "4",  "B1", "5",  "6",  "7",
                                 "8", "B2", "9", "10", "11", "12", "13", "14", "X"};
/* 0x6cf610 .bss: the label returned for an out-of-range level (an empty string) */
char g_emptyLevelLabel[1];

#define SDW_INLINE_FREE_PROPERTY_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_PROPERTY_VOID_U32
inline Box *PropertyBox(void *record, s32 offset)
{
    return Scn_GetPropBox(record, offset);
}
inline s32 BoxContains(Box *box, Vec3s *point)
{
    return point->x >= box->min[0] && point->x <= box->max[0] && point->y >= box->min[1] && point->y <= box->max[1] &&
           point->z >= box->min[2] && point->z <= box->max[2];
}
inline s16 AngleDifference(s32 a, s32 b)
{
    return (s16)((a - b + 0x800) & 0xfff) - 0x800;
}
#define ABS_VALUE(x) ((x) >= 0 ? (x) : -(x))
#define SDW_INLINE_FREE_SCREENWIDTHS32 1
#include "../engine/screen_inlines.h"
#undef SDW_INLINE_FREE_SCREENWIDTHS32
#define SDW_INLINE_FREE_SCREENHEIGHTS32 1
#include "../engine/screen_inlines.h"
#undef SDW_INLINE_FREE_SCREENHEIGHTS32

const char *DoorLevel::GetLevelLabel(s8 level)
{
    if (level < SCENE_LVL_00 || level > SCENE_LVL_17)
        return g_emptyLevelLabel;
    return g_levelLabels[level];
}

void DoorLevel::PostLoadInit()
{
    u16 *props;
    enteredFromBack = 0;
    wolfFacingDoor = 0;
    props = record;
    levelNumber = (s8)Property(props, 4);
    backBox = PropertyBox(record, 0);
    state = DOORLEVEL_ST_INIT;
    SetUpdateMode(SCN_UPD_NORMAL);
}

void DoorLevel::Update()
{
    switch (state) {
        case DOORLEVEL_ST_INIT:
            PlayAnim(APORTE01_ANIM_CLOSE2, 0, 1);
            state = DOORLEVEL_ST_CLOSED;
            break;
        case DOORLEVEL_ST_OPEN:
            if (wolfFacingDoor)
                PlayAnim(APORTE01_ANIM_BACK, 0, 1);
            else
                PlayAnim(APORTE01_ANIM_OPEN, 0, 1);
            state = DOORLEVEL_ST_OPENED;
            if (enteredFromBack && levelNumber)
                g_pProgress->SetSceneExitTarget((s8)(levelNumber + 1));
            else
                g_pProgress->SetSceneExitTarget(levelNumber);
            Fade_StartLevelExit(((Anim_GetDurationMs(Inst(), APORTE01_ANIM_OPEN, 1) << 12) / 1000) + 0x800);
            break;
        case DOORLEVEL_ST_OPENED:
            if (AnimFlags(ANIM_F_FINISHED)) {
                if (wolfFacingDoor)
                    PlayAnim(APORTE01_ANIM_BACK1, 0, 1);
                else
                    PlayAnim(APORTE01_ANIM_OPEN1, 0, 1);
                state = DOORLEVEL_ST_CLOSED;
            }
            break;
    }
    AdvanceAnim();
}

/* BYTES(slot-group): locals grouped in work only to pin the original frame offsets; alignment fill gaps */
s32 DoorLevel::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    /* Explicit work record preserves the original stack slots (-0xc, -5, -4). */
    /* This is an reconstruction, not an assertion about original names. */
    struct Work {
        const char *status;
        u8 alignment[3];
        s8 levelIndex;
        s32 theta;
    } work;
    work.levelIndex = levelNumber;
    switch (msgId) {
        case MSG_QUERY_ACTION:
            if (sender->GetClassId() == CLASSID_WOLF) {
                enteredFromBack = 0;
                if (backBox && BoxContains(backBox, &sender->pos)) {
                    enteredFromBack = 1;
                    work.levelIndex++;
                }
                Text_SetWindow(g_screenLayerBase + 6, 32, ScreenHeightS32() - 48, ScreenWidthS32() - 64, 32, 1);
                Text_SetFont(FONT_GAME);
                if (!g_pProgress->IsLevelDone(work.levelIndex))
                    work.status = Text_GetClassString(DOORLEVELSTR_NOT_COMPLETED);
                else if (!g_pProgress->IsTimeKeeperDone(work.levelIndex)) {
                    if (work.levelIndex)
                        work.status = Text_GetClassString(DOORLEVELSTR_COMPLETED);
                    else
                        work.status = Text_GetClassString(DOORLEVELSTR_FULLY_COMPLETED);
                } else
                    work.status = Text_GetClassString(DOORLEVELSTR_FULLY_COMPLETED);
                Text_PrintfStyled(TEXTALIGN_CENTER, 1, "%s %s: %s", Text_GetClassString(DOORLEVELSTR_LEVEL),
                                  GetLevelLabel(work.levelIndex), work.status);
                Hud_EndBox_stub();
                return CTX_LEVELDOOR;
            }
            break;
        case MSG_USE:
            if (sender->GetClassId() == CLASSID_WOLF) {
                state = DOORLEVEL_ST_OPEN;
                work.theta = HeadingTo(&sender->pos);
                wolfFacingDoor = ABS_VALUE(AngleDifference(work.theta, GetHeading())) < 0x400;
            }
            return 1;
        case MSG_DOORLEVEL_QUERY:
            if (!g_pProgress->IsLevelDone(work.levelIndex))
                return DOORLEVEL_NOT_DONE;
            if (g_pProgress->GetLevelIndexA() == work.levelIndex)
                return DOORLEVEL_CURRENT;
            return DOORLEVEL_DONE;
    }
    return 0;
}

void DoorLevel::Reset()
{
    PlayAnim(APORTE01_ANIM_CLOSE2, 0, 0);
}
void DoorLevel::Render(Camera *view)
{
    if (levelNumber)
        ScnBody::Render(view);
}
ScnObject *DoorLevel_Create(void *record)
{
    ScnBody *object = new DoorLevel;
    object = object->Init(record, 0);
    return object;
}
