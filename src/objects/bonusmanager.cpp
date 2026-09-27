/* T109 - original object BonusManager.cpp (guessed name).
 * Ranges: .text 0x49b620-0x49ce2d, .rdata 0x575ae8-0x575b0c (vtable), .data 0x57b298-0x57b398 (g_bonusEntries, then
 * the /Od string literals of this object in the order the compiler meets them), .bss 0x6cf460-0x6cf5f4.
 * Contents: PostLoadInit, DrawBonusList, DrawEntryInfo, DrawViewerFooter, Update (0x49bd01: all thirteen screens; its
 * frame: `char text[128]` lands at [ebp-0x88], the inline-expansion temporaries [ebp-0xa8..-0xb5] come from the three
 * Progress accessors, which moves `this` to [ebp-0xc8]), HandleMessage, Reset, Create and GetUiString. */
/* BYTES: bss-name, dead-code. */
/* BYTES(bss-name): named for its .bss hash key 850 */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "../sdk/dinput.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *, Animator *, u16, u32);
class ScnObject;
class Camera;
#include "../app/app_main.h"
void Camera_StartScripted(ScnObject *, Camera *, u16, u16, u16, Vec3s *, u16, u32, s32);

#define SDW_MEMBERS_ScnObject       \
    static void *operator new(u32); \
    void SetUpdateMode(u8 mode);    \
    void StartCamera(u16 x, u16 y, u16 z, Vec3s *point, u16 focal, u32 mode, s32 time);

#define SDW_MEMBERS_UiQuad                                                \
    void SetFade(u16 amount)                                              \
    {                                                                     \
        drawFlags = (drawFlags & (u16)~UIQUAD_FADE_MASK) | (amount << 6); \
    }
#define SDW_MEMBERS_Progress        \
    u16 GetBonusPoints()            \
    {                               \
        return bonusPoints;         \
    }                               \
    void SetBonusUnlocked(s8 entry) \
    {                               \
        bonusFlags |= 1 << entry;   \
    }
#define SDW_MEMBERS_PackJpeg \
    u32 GetIndex()           \
    {                        \
        return index;        \
    }

#include "sdw_classes.h"
#include "../engine/progress.h"
#include "../engine/draw2d.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8 1
#define SDW_INLINE_SCNOBJECT_STARTCAMERA_U16_U16_U16_VEC3S_U16_U32_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8
#undef SDW_INLINE_SCNOBJECT_STARTCAMERA_U16_U16_U16_VEC3S_U16_U32_S32
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_UIQUAD_SETCOLOR_U32 1
#include "../engine/ui_quad_inlines.h"
#undef SDW_INLINE_UIQUAD_SETCOLOR_U32
#define SDW_INLINE_PROGRESS_SETSCENEEXITTARGET_S8 1
#include "../engine/progress_inlines.h"
#undef SDW_INLINE_PROGRESS_SETSCENEEXITTARGET_S8
#define SDW_INLINE_PACKJPEG_GETCOUNT 1
#include "pack_jpeg_inlines.h"
#undef SDW_INLINE_PACKJPEG_GETCOUNT
extern Wolf *g_pWolf;
#include "../engine/stream_player.h"
#include "../engine/screen.h"
#include "pack_jpeg.h"
#include "../engine/text.h"
extern u32 g_gameFlags, *g_screenLayerBase;
#include "../engine/time.h"
#include "camera.h"
extern s16 g_rectFooter[4], g_rectDialogBody[4], g_rectChoiceWide[4];

/* .data 0x57b298: the eight bonus entries (kind, price, picture name or text id) */
BonusEntry g_bonusEntries[8] = {{BONUS_KIND_IMAGES, 1, "PERSO"},
                                {BONUS_KIND_IMAGES, 2, "DECOR"},
                                {BONUS_KIND_IMAGES, 2, "STORY"},
                                {BONUS_KIND_IMAGES, 1, "COLOR"},
                                {BONUS_KIND_IMAGES, 2, "TEAM"},
                                {BONUS_KIND_IMAGES, 3, "INTRO"},
                                {BONUS_KIND_TEXT, 3, {BONUSSTR_TIP_AUTUMN_TEXT}},
                                {BONUS_KIND_TEXT, 3, {BONUSSTR_TIP_PAST_PRESENT_TEXT}}};
/* .bss 0x6cf460-0x6cf5f1 */
char g_bonusPath[256];           /* 0x6cf460 */
char *g_bonusCategoryStrings[4]; /* 0x6cf560 */
char g_bonusUiBuf[128];          /* 0x6cf570; named for its .bss order: VC6 allocates .bss in the order of a
                                       1024-bucket hash of the names, and g_bonusUiText (bucket 1017) would land after
                                       g_bonusFooterInitial (876); g_bonusUiBuf (850) lands before it, as in the exe */
char g_bonusFooterInitial;       /* 0x6cf5f0 */

#include "../sdk/crt.h"
extern "C" {
}
#include "../engine/scn_tools.h"
u32 *Res_GetValidatedIdList(u16, u16 *);
#include "../engine/interface.h"
#include "../engine/prompt.h"
#include "../engine/maths.h"
#include "../engine/input.h"
#include "../engine/fade.h"
#define SDW_INLINE_FREE_SCREENWIDTHS16 1
#include "../engine/screen_inlines.h"
#undef SDW_INLINE_FREE_SCREENWIDTHS16
#define SDW_INLINE_FREE_SCREENHEIGHTS16 1
#include "../engine/screen_inlines.h"
#undef SDW_INLINE_FREE_SCREENHEIGHTS16
inline u32 *Layer(u16 index)
{
    /* cast kept: scratch60 is a malloc'd block the text code lays out as u32 layer words */
    return (u32 *)g_screen.scratch60 + index;
}
inline s32 BonusIsUnlocked(s8 entry)
{
    return (g_pProgress->bonusFlags >> entry) & 1;
}
#define SDW_INLINE_FREE_SETGAMEFLAGS_U32 1
#include "../engine/game_state_inlines.h"
#undef SDW_INLINE_FREE_SETGAMEFLAGS_U32
#define SDW_INLINE_FREE_CLEARGAMEFLAGS_U32 1
#include "../engine/game_state_inlines.h"
#undef SDW_INLINE_FREE_CLEARGAMEFLAGS_U32
void BonusManager::PostLoadInit()
{
    void *props;
    u32 *ids;
    u16 *image;
    u16 count, item;
    strcpy(g_bonusPath, g_dirBonusGame);
    props = record;
    camera = Scn_GetPropCamera(props, 0);
    PlayAnim(ACBONUS1_ANIM_STAND, 1, 0);
    SetUpdateMode(SCN_UPD_NORMAL);
    for (item = 0; item < 4; item++)
        g_bonusCategoryStrings[item] = GetUiString(item + BONUSSTR_PICTURE);
    for (item = 0; item < 2; item++) {
        ids = Res_GetValidatedIdList(item + DAV_IDI_IBOCKOI_, &count);
        if (ids && count) {
            /* cast kept: an id list holds record pointers as u32 words; this one is an icon image */
            image = (u16 *)*ids;
            iconQuads[item].UiQuad_SetFromBitmap(image, 0, 0, 0, 0, 1024, 1024);
            iconQuads[item].SetColor(0x808080);
        }
    }
    Reset();
}

void BonusManager::DrawBonusList()
{
    s16 originX = 32, originY = 36;
    s8 index;
    for (index = 0; index < 4; index++) {
        s16 rect[4];
        s8 icon;
        rect[0] = (index + 1) * 80 - 8 + originX;
        rect[1] = originY;
        rect[2] = 63;
        rect[3] = 47;
        Ui_DrawRectOutline(rect, cursor - firstVisible == index ? 0x4040f0 : 0xb0b0b0);
        icon = (s8)BonusIsUnlocked(firstVisible + index);
        iconQuads[icon].x = rect[0];
        iconQuads[icon].y = rect[1];
        iconQuads[icon].SetFade(0);
        if (index != cursor - firstVisible)
            iconQuads[icon].SetFade(4);
        iconQuads[icon].UiQuad_Draw(2);
    }
    Ui_DrawScrollArrow(originX, originY, SPRFLIP_ROT90);
    Ui_DrawScrollArrow(originX + 400, originY, SPRFLIP_ROT270);
}

/* BYTES(dead-code, inferred): rect[0] / rect[1] are stored again unchanged, as the original does */
void BonusManager::DrawEntryInfo()
{
    s16 rect[4];
    rect[0] = 32;
    rect[1] = 92;
    rect[2] = ScreenWidthS16() - 64;
    rect[3] = ScreenHeightS16() - 148;
    Text_SetWindowRect(Layer(2), rect, 1);
    rect[0] = rect[0];
    rect[1] = rect[1];
    rect[2] = ScreenWidthS16() - 65;
    rect[3] = ScreenHeightS16() - 149;
    Ui_DrawRectOutline(rect, 0xb0b0b0);
    Text_Printf(TEXTALIGN_LEFT, "%s %d:\n%s", GetUiString(BONUSSTR_BONUS), cursor + 1,
                g_bonusCategoryStrings[g_bonusEntries[cursor].kind]);
    if (!BonusIsUnlocked(cursor))
        Text_Printf(TEXTALIGN_CONTINUE, " (%d %s)", g_bonusEntries[cursor].price, GetUiString(BONUSSTR_POINTS));
    Text_NewLine(1);
    Text_Printf(TEXTALIGN_LEFT, GetUiString(cursor + BONUSSTR_CHARACTER_SKETCH));
    Hud_EndBox_stub();
}

void BonusManager::DrawViewerFooter()
{
    char output[128] = {g_bonusFooterInitial};
    char previousLabel[] = "$B_LEFT$", rightLabel[] = "$B_RIGHT$";
    char key[64];
    Text_SetFont(FONT_GAME);
    Text_SetWindow(Layer(2), 0, ScreenHeightS16() - g_pCurFont->lineHeight * 3 / 2, 512, g_pCurFont->lineHeight, 1);
    if (g_packJpeg.GetIndex() > 0) {
        Str_Copy(output, previousLabel);
        strcat(output, ",");
    }
    if (g_packJpeg.GetIndex() < g_packJpeg.GetCount() - 1) {
        Str_Concat2(output, output, rightLabel);
        strcat(output, ",");
    }
    g_inputMgr.Input_GetKeyName(DIK_ESCAPE, key, 64);
    strcat(output, " $B_CANCEL$/$C_CYAN$");
    strcat(output, key);
    strcat(output, "$C_DEFAULT$");
    strcat(output, GetUiString(BONUSSTR_BROWSE_FOOTER));
    Text_Printf(TEXTALIGN_CENTER, output);
    Hud_EndBox_stub();
    g_packJpeg.DrawImage();
}

/* 0x49bd01 */
void BonusManager::Update()
{
    s16 viewerRect[4];
    char text[128];
    u8 answer;
    s8 scroll;
    Text_SetFont(FONT_GAME);
    switch (screen) {
        case BONUS_SCR_OPEN:
            ClearGameFlags(GF_TRANSITION_IDLE);
            g_pStreamPlayer->SetPaused(1);
            StartCamera(camera->rot[0], camera->rot[1], camera->rot[2], &camera->eye, camera->focal,
                        CAMSCR_BLEND_IN | CAMSCR_BLEND_OUT_KEEP_ROT, 4096);
            if (g_pWolf) {
                g_pWolf->HandleMessage(this, MSG_FREEZE, 0);
                g_pWolf->HandleMessage(this, MSG_WOLF_SET_OBJFLAG2, 0);
            }
            points = g_pProgress->GetBonusPoints();
            cursor = 0;
            firstVisible = 0;
            backdropFade = 7;
            boughtSomething = 0;
            screen = BONUS_SCR_LIST;
            break;
        case BONUS_SCR_LIST:
            if ((g_camMode == CAM_SCRIPT_BLEND_IN || g_camMode == CAM_SCRIPT_TO_SCRIPT ||
                 g_camMode == CAM_SCRIPT_RETURN) == 0) {
                g_renderWorldFlag = 0;
                ClearGameFlags(GF_BIT0 | GF_RENDER_WORLD);
                if (backdropFade > 0)
                    backdropFade--;
                Ui_DrawMemCardBackdrop(backdropFade);
                DrawBonusList();
                Text_SetWindow(Layer(2), 16, 16, ScreenWidthS16() - 32, 20, 1);
                Text_Printf(TEXTALIGN_CENTER, GetUiString(BONUSSTR_YOU_HAVE_POINTS), points);
                Hud_EndBox_stub();
                DrawEntryInfo();
                Text_SetWindow(Layer(2), 16, ScreenHeightS16() - 48, ScreenWidthS16() - 32, 48, 1);
                if (BonusIsUnlocked(cursor))
                    Text_Printf(TEXTALIGN_CENTER, GetUiString(BONUSSTR_VIEW_PROMPT));
                else
                    Text_Printf(TEXTALIGN_CENTER, GetUiString(BONUSSTR_BUY_PROMPT));
                Text_NewLine(1);
                Text_Printf(TEXTALIGN_CENTER, GetUiString(BONUSSTR_EXIT));
                Hud_EndBox_stub();
                if (Pad_MenuRepeat((u16)~PAD_LEFT) && cursor > 0) {
                    Ui_PlayMoveSound();
                    cursor--;
                    if (cursor < firstVisible)
                        firstVisible--;
                }
                if (Pad_MenuRepeat((u16)~PAD_RIGHT) && cursor < 7) {
                    Ui_PlayMoveSound();
                    cursor++;
                    if (cursor - firstVisible == 4)
                        firstVisible++;
                }
                if (Pad_MenuPressed((u16)~PAD_CROSS)) {
                    Ui_PlayConfirmSound();
                    if (!BonusIsUnlocked(cursor))
                        screen = BONUS_SCR_BUY_PROMPT;
                    else
                        switch (g_bonusEntries[cursor].kind) {
                            case BONUS_KIND_IMAGES:
                                screen = BONUS_SCR_VIEWER_OPEN;
                                break;
                            case BONUS_KIND_TEXT: {
                                s16 rect[4];
                                rect[0] = 36;
                                rect[1] = 95;
                                rect[2] = ScreenWidthS16() - 72;
                                rect[3] = ScreenHeightS16() - 154;
                                scrollText.Init(GetUiString(g_bonusEntries[cursor].payload[0]), rect);
                                screen = BONUS_SCR_TEXT;
                                break;
                            }
                            case BONUS_KIND_SCENE:
                                g_pProgress->SetSceneExitTarget(g_bonusEntries[cursor].payload[0]);
                                Fade_StartLevelExit(4096);
                                screen = BONUS_SCR_EXIT;
                                break;
                        }
                }
                if (Pad_MenuPressed((u16)~PAD_TRIANGLE)) {
                    Ui_PlayCancelSound();
                    screen = BONUS_SCR_EXIT;
                }
            }
            break;
        case BONUS_SCR_BUY_PROMPT:
            Ui_DrawMemCardBackdrop(backdropFade);
            answer = Ui_PromptYesNo(GetUiString(BONUSSTR_CONFIRM_BUY), GetUiString(BONUSSTR_BUY),
                                    Text_GetUiString(UISTR_CANCEL), &promptState);
            Text_SetWindowRect(g_screenLayerBase + 2, g_rectFooter, 1);
            Text_Printf(TEXTALIGN_LEFT, "$B_VALID$ %s", Text_GetUiString(UISTR_VALIDATE));
            Text_Printf(TEXTALIGN_RIGHT, "$B_CANCEL$ %s", Text_GetUiString(UISTR_CANCEL));
            Hud_EndBox_stub();
            if (answer == MCARD_CUR_LEFT) {
                if (g_bonusEntries[cursor].price <= points) {
                    points -= g_bonusEntries[cursor].price;
                    g_pProgress->SetBonusPoints(points);
                    g_pProgress->SetBonusUnlocked(cursor);
                    screen = BONUS_SCR_BOUGHT;
                } else
                    screen = BONUS_SCR_NO_POINTS;
            } else if (answer)
                screen = BONUS_SCR_LIST;
            break;
        case BONUS_SCR_BOUGHT:
            Ui_DrawMemCardBackdrop(backdropFade);
            Text_Sprintf(text, "\n$B_VALID$ %s", GetUiString(BONUSSTR_OK));
            boughtSomething = 1;
            Text_SetColor(0x4bccff);
            Ui_DrawTextInRect(g_rectDialogBody, TEXTALIGN_CENTER, 0, GetUiString(BONUSSTR_BOUGHT));
            Ui_DrawTextInRect(g_rectChoiceWide, TEXTALIGN_CENTER, 1, text);
            if (Pad_MenuPressed((u16)~PAD_CROSS)) {
                Ui_PlayConfirmSound();
                screen = BONUS_SCR_LIST;
            }
            break;
        case BONUS_SCR_NO_POINTS:
            Ui_DrawMemCardBackdrop(backdropFade);
            answer = Ui_PromptConfirm(GetUiString(BONUSSTR_NOT_ENOUGH_POINTS), GetUiString(BONUSSTR_OK));
            if (answer)
                screen = BONUS_SCR_LIST;
            break;
        case BONUS_SCR_VIEWER_OPEN:
            if (g_bonusEntries[cursor].kind == BONUS_KIND_IMAGES)
                Str_Concat2(text, g_bonusPath, g_bonusEntries[cursor].payload);
            if (!g_packJpeg.Open(text))
                screen = BONUS_SCR_CLOSED;
            else {
                unknownB8 = 1;
                g_packJpeg.BeginImage();
                viewerNeedsDraw = 1;
                screen = BONUS_SCR_VIEWER_WAIT;
            }
            break;
        case BONUS_SCR_VIEWER_WAIT:
            viewerDelay++;
            if (viewerDelay > 10) {
                screen = BONUS_SCR_VIEWER;
                viewerDelay = 0;
            }
            break;
        case BONUS_SCR_VIEWER:
            if (viewerNeedsDraw) {
                g_packJpeg.DecodeToTextures(0);
                viewerNeedsDraw = 0;
            }
            if (!viewerNeedsDraw) {
                DrawViewerFooter();
                if (Pad_MenuPressed((u16)~PAD_LEFT))
                    screen = BONUS_SCR_VIEWER_PREV;
                if (Pad_MenuPressed((u16)~PAD_RIGHT))
                    screen = BONUS_SCR_VIEWER_NEXT;
                if (Pad_MenuPressed((u16)~PAD_TRIANGLE)) {
                    Ui_PlayCancelSound();
                    Font_SetCellSize(15, 15);
                    screen = BONUS_SCR_VIEWER_CLOSE;
                }
            }
            break;
        case BONUS_SCR_VIEWER_CLOSE:
            viewerDelay++;
            if (viewerDelay > 10) {
                g_packJpeg.ReleaseImage();
                g_packJpeg.Close();
                screen = BONUS_SCR_LIST;
                viewerDelay = 0;
            }
            break;
        case BONUS_SCR_VIEWER_PREV:
            if (g_packJpeg.GetIndex() > 0) {
                Ui_PlayMoveSound();
                g_packJpeg.Prev();
                g_packJpeg.ReleaseImage();
                g_packJpeg.BeginImage();
                viewerNeedsDraw = 1;
                screen = BONUS_SCR_VIEWER_WAIT;
            } else
                screen = BONUS_SCR_VIEWER;
            break;
        case BONUS_SCR_VIEWER_NEXT:
            if (g_packJpeg.GetIndex() < g_packJpeg.GetCount() - 1) {
                Ui_PlayMoveSound();
                g_packJpeg.Next();
                g_packJpeg.ReleaseImage();
                g_packJpeg.BeginImage();
                viewerNeedsDraw = 1;
                screen = BONUS_SCR_VIEWER_WAIT;
            } else
                screen = BONUS_SCR_VIEWER;
            break;
        case BONUS_SCR_TEXT:
            Ui_DrawMemCardBackdrop(backdropFade);
            DrawBonusList();
            viewerRect[0] = 32;
            viewerRect[1] = 92;
            viewerRect[2] = ScreenWidthS16() - 65;
            viewerRect[3] = ScreenHeightS16() - 149;
            Ui_DrawRectOutline(viewerRect, 0xb0b0b0);
            scrollText.Draw(2, 1);
            Text_SetWindowRect(Layer(2), g_rectFooter, 1);
            Text_Printf(TEXTALIGN_CENTER, GetUiString(BONUSSTR_BROWSE_FOOTER));
            Hud_EndBox_stub();
            scroll = 0;
            if (Pad_MenuHeld((u16)~PAD_UP))
                scroll = -1;
            else if (Pad_MenuHeld((u16)~PAD_DOWN))
                scroll = 1;
            scrollText.ScrollList_Update(scroll);
            if (Pad_MenuPressed((u16)~PAD_TRIANGLE))
                screen = BONUS_SCR_LIST;
            break;
        case BONUS_SCR_EXIT:
            g_renderWorldFlag = 1;
            SetGameFlags(GF_RENDER_WORLD);
            if ((g_camMode == CAM_SCRIPT_BLEND_IN || g_camMode == CAM_SCRIPT_TO_SCRIPT ||
                 g_camMode == CAM_SCRIPT_RETURN) == 0) {
                SetGameFlags(GF_TRANSITION_IDLE);
                if (boughtSomething && g_pProgress->runtimeBits.saveSlotValid && g_pProgress->runtimeBits.autoSaveOn)
                    Scenaric_SendToClass(CLASSID_MCARDMANAGER, MSG_MCARD_AUTOSAVE, 0);
                else
                    Camera_ReleaseAny();
                if (g_pWolf) {
                    g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0);
                    /* cast kept: HandleMessage's arg is a void *; this message passes a number in it */
                    g_pWolf->HandleMessage(this, MSG_WOLF_SET_OBJFLAG2, (void *)1);
                }
                g_pStreamPlayer->SetPaused(0);
                screen = BONUS_SCR_CLOSED;
            }
            break;
    }
    AdvanceAnim();
}

s32 BonusManager::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    switch (msgId) {
        case MSG_QUERY_ACTION:
            if (!screen && sender->GetClassId() == CLASSID_WOLF && Vec3s_ManhattanDistXZ(&pos, &sender->pos) < 300)
                return CTX_BONUS;
            break;
        case MSG_USE:
            screen = BONUS_SCR_OPEN;
            return 1;
    }
    return 0;
}

void BonusManager::Reset()
{
    promptState = MCARD_CUR_LEFT;
    screen = BONUS_SCR_CLOSED;
    backdropFade = 7;
}

ScnObject *BonusManager_Create(void *record)
{
    BonusManager *object = new BonusManager;
    object = (BonusManager *)object->Init(record, 0); /* cast kept: Init returns the ScnBody * base of this object */
    return object;
}

char *BonusManager::GetUiString(u8 stringId)
{
    char *delimiter;
    char text[64];
    u32 key;
    char keyText[64];
    memset(g_bonusUiBuf, 0, 128);
    if (stringId == BONUSSTR_EXIT)
        key = DIK_ESCAPE;
    else
        key = DIK_RETURN;
    switch (stringId) {
        case BONUSSTR_EXIT:
        case BONUSSTR_BUY_PROMPT:
        case BONUSSTR_VIEW_PROMPT:
            g_inputMgr.Input_GetKeyName(key, keyText, 64);
            strcpy(text, Text_GetClassString(stringId));
            delimiter = strrchr(text, '$');
            strncpy(g_bonusUiBuf, text, delimiter - text + 1);
            strcat(g_bonusUiBuf, "/$C_CYAN$");
            strcat(g_bonusUiBuf, keyText);
            strcat(g_bonusUiBuf, "$C_DEFAULT$");
            strcat(g_bonusUiBuf, delimiter + 1);
            return g_bonusUiBuf;
        case BONUSSTR_BROWSE_FOOTER:
            strcpy(text, Text_GetClassString(stringId));
            delimiter = strrchr(text, '$');
            strcpy(g_bonusUiBuf, delimiter + 1);
            return g_bonusUiBuf;
        default:
            return Text_GetClassString(stringId);
    }
}
