/*
 * T214 - original object SignPost.cpp (guessed name), one translation unit: SignPost, SignPostSimple and
 * SignPostAnimated, whose methods the original interleaves by kind (all Init/Reset, then all Update, then all
 * HandleMessage, then the three factories; the transitions 0x4f10d5 .. 0x4f1e4c are unaligned).
 *   .text  0x4f0fa0-0x4f1eae (SignPostAnimated_Init .. SignPost_Create, in address order)
 *   .rdata 0x576bd4-0x576c40 (??_7SignPostSimple, ??_7SignPostAnimated, ??_7SignPost: the order of the factories)
 *   .bss   0x6cfa5c-0x6cfa60 (g_signPostReadCount + 3 pad)
 * The three classes' SDW_MEMBERS lists and shared helpers are declared below; the UI functions they call are declared
 * locally.
 */
/* BYTES: slot-scope. */

#define SDW_MEMBERS_ScnObject                    \
    static void *operator new(u32 size);         \
    void SetTint(u32 color, s16 amount, s32 on); \
    void StartCamera(u16 rx, u16 ry, u16 rz, Vec3s *at, u16 focal);


#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *inst, Animator *animator, u16 id, u32 opts);
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
#define SDW_INLINE_SCNOBJECT_SETCOLLIDABLE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETCOLLIDABLE_S32
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32

extern Wolf *g_pWolf;
#include "../app/app_main.h"
#include "../engine/scenaric.h"
#include "../engine/input.h"
#include "../engine/scn_tools.h"
#include "../engine/id_list.h"
#include "../engine/interface.h"
#include "camera.h"
u8 g_signPostReadCount; /* 0x6cfa5c */
void Ui_DrawTextBox(TextBox *box, u16 lineCount);
void Camera_StartScripted(ScnObject *owner, Camera *cam, u16 rx, u16 ry, u16 rz, Vec3s *at, u16 focal, u32 flags,
                          s32 time);

#define SDW_INLINE_FREE_SCN_GETPROPU32_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_SCN_GETPROPU32_VOID_U32
#define SDW_INLINE_FREE_SCREENWIDTHS16 1
#include "../engine/screen_inlines.h"
#undef SDW_INLINE_FREE_SCREENWIDTHS16
#define SDW_INLINE_FREE_INSTFLAGS_SET_U16_U16 1
#include "instance_inlines.h"
#undef SDW_INLINE_FREE_INSTFLAGS_SET_U16_U16
#define SDW_INLINE_FREE_INSTFLAGS_CLEAR_U16_U16 1
#include "instance_inlines.h"
#undef SDW_INLINE_FREE_INSTFLAGS_CLEAR_U16_U16
#define SDW_INLINE_SCNOBJECT_SETTINT_U32_S16_S32 1
#define SDW_INLINE_SCNOBJECT_STARTCAMERA_U16_U16_U16_VEC3S_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETTINT_U32_S16_S32
#undef SDW_INLINE_SCNOBJECT_STARTCAMERA_U16_U16_U16_VEC3S_U16
#define SDW_INLINE_FREE_ZONES_GET_U8 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_FREE_ZONES_GET_U8
#define SDW_INLINE_ZONELIST_FINDCONTAINING_VEC3S 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_FINDCONTAINING_VEC3S

/* 0x4f0fa0 */
void SignPostAnimated::PostLoadInit()
{
    void *rec = record;
    textIndex = Scn_GetPropU32(rec, 8);
    hidden = Scn_GetPropU32(rec, 4);
    text = Text_GetClassString((u8)textIndex);
    distance = (u16)Scn_GetPropU32(rec, 0);
    state = SIGNANIM_ST_IDLE;
    if (hidden) {
        SetVisible(0);
        SetCollidable(0);
    }
    PlayAnim(SIGNANIM_ANIM_IDLE, 1, 0);
}

/* 0x4f10d5 */
void SignPostSimple::PostLoadInit()
{
    wolfFrozen = 0;
    void *rec = record;
    textIndex = Scn_GetPropU32(rec, 8);
    text = Text_GetClassString((u8)textIndex);
    distance = (u16)Scn_GetPropU32(rec, 0);
    state = SIGN_ST_IDLE;
    if (Zones_Get(ZONE_SHADOW)->FindContaining(&pos))
        SetTint(0, 0x600, 1);
    savedPos.x = pos.x;
    savedPos.y = pos.y;
    savedPos.z = pos.z;
}

/* 0x4f1202 */
void SignPostSimple::Reset()
{
    if (IsInWorld()) {
        SetPosition(&savedPos);
        SnapToGround(0);
    }
}

/* 0x4f1240 */
/* BYTES(slot-scope, inferred): the nested block(s) only order the frame: their locals are allocated after the enclosing scope's */
void SignPost::PostLoadInit()
{
    void *rec = record;
    u32 anvilId;
    textNum = Scn_GetPropU32(rec, 12);
    text = Text_GetClassString((u8)textNum);
    g_signPostReadCount = 0;
    textVariant = -1;
    {
        u32 resource = Scn_GetPropU32(rec, 8);
        u16 count;
        u32 *cameraList = Scn_FindIdList((u16)resource, &count);
        /* cast kept: an export id list holds record pointers of any kind */
        if (cameraList)
            camera = (CamSetup *)*cameraList;
        else
            camera = 0;
    }
    anvilId = Scn_GetPropU32(rec, 4);
    if (anvilId == 0)
        anvil = 0;
    else
        anvil = Scenaric_FindByIdList((u16)anvilId);
    distance = (u16)Scn_GetPropU32(rec, 0);
    state = SIGNPOST_ST_IDLE;
    wolfFrozen = 0;
    if (Zones_Get(ZONE_SHADOW)->FindContaining(&pos))
        SetTint(0, 0x600, 1);
    savedPos.x = pos.x;
    savedPos.y = pos.y;
    savedPos.z = pos.z;
}

/* 0x4f13f7 */
void SignPost::Reset()
{
    if (IsInWorld()) {
        SetPosition(&savedPos);
        SnapToGround(0);
    }
}

/* 0x4f1435 */
void SignPostAnimated::Update()
{
    s16 rect[4];
    rect[0] = 50;
    rect[1] = 50;
    rect[2] = ScreenWidthS16() - 100;
    rect[3] = 140;
    switch (state) {
        case SIGNANIM_ST_OPENING:
            if (!Pad_MenuPressed((u16)~PAD_CROSS))
                state = SIGNANIM_ST_SHOWING;
            break;
        case SIGNANIM_ST_SHOWING:
            Ui_DrawSubtitleBox(text, rect, DAV_IDI_IGLCADP_);
            if (Pad_MenuPressed((u16)~PAD_CROSS)) {
                state = SIGNANIM_ST_CLOSING;
                break;
            }
            break;
        case SIGNANIM_ST_CLOSING:
            if (!Pad_MenuPressed((u16)~PAD_CROSS)) {
                if (wolfFrozen)
                    g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0);
                state = SIGNANIM_ST_IDLE;
            }
            break;
        case SIGNANIM_ST_REVEALING:
            if (AnimFlags(ANIM_F_FINISHED)) {
                state = SIGNANIM_ST_IDLE;
                SetCollidable(1);
                PlayAnim(SIGNANIM_ANIM_IDLE, 1, 0);
            }
            break;
    }
    AdvanceAnim();
}

/* 0x4f15c8 */
void SignPostSimple::Update()
{
    s16 rect[4];
    rect[0] = 50;
    rect[1] = 50;
    rect[2] = 412;
    rect[3] = 140;
    switch (state) {
        case SIGN_ST_OPENING:
            if (!Pad_MenuPressed((u16)~PAD_CROSS))
                state = SIGN_ST_SHOWING;
            break;
        case SIGN_ST_SHOWING:
            Ui_DrawSubtitleBox(text, rect, DAV_IDI_IGLCADP_);
            if (Pad_MenuPressed((u16)~PAD_CROSS)) {
                state = SIGN_ST_CLOSING;
                break;
            }
            break;
        case SIGN_ST_CLOSING:
            if (!Pad_MenuPressed((u16)~PAD_CROSS)) {
                if (wolfFrozen)
                    g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0);
                state = SIGN_ST_IDLE;
            }
            break;
    }
}

/* 0x4f169c. The original copies the rectangle as two dwords. */
void SignPost::Update()
{
    TextBox box;
    s16 rects[4];
    rects[0] = 50;
    rects[1] = 50;
    rects[2] = 412;
    rects[3] = 140;
    box.text = text;
    /* cast kept (both lines): the rectangle is copied as two dwords, as the original does */
    ((u32 *)box.rect)[0] = ((u32 *)rects)[0];
    ((u32 *)box.rect)[1] = ((u32 *)rects)[1];
    box.bgColor = 0xa0a0a;
    box.fits = 0;
    switch (state) {
        case SIGNPOST_ST_OPENING:
            if (!Pad_MenuPressed((u16)~PAD_CROSS))
                state = SIGNPOST_ST_SHOWING;
            break;
        case SIGNPOST_ST_SHOWING:
            if (textVariant == 1)
                Ui_DrawTextBox(&box, 0xffff);
            else
                Ui_DrawSubtitleBox(text, rects, DAV_IDI_IGLCADP_);
            if (Pad_MenuPressed((u16)~PAD_CROSS)) {
                if (box.fits && (s8)box.confirmChoice == 0 && anvil) {
                    anvil->HandleMessage(this, MSG_ANVIL_DROP, 0);
                    g_pWolf->HandleMessage(this, MSG_WOLF_DAZE, 0);
                    state = SIGNPOST_ST_ANVIL;
                    if (camera)
                        StartCamera(camera->rot[0], camera->rot[1], camera->rot[2], &camera->eye, camera->focal);
                    break;
                } else {
                    state = SIGNPOST_ST_CLOSING;
                    break;
                }
            }
            break;
        case SIGNPOST_ST_CLOSING:
            if (!Pad_MenuPressed((u16)~PAD_CROSS)) {
                if (wolfFrozen)
                    g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0);
                state = SIGNPOST_ST_IDLE;
            }
            break;
        case SIGNPOST_ST_ANVIL:
            /* cast kept: the anvil is found by its id list as a ScnObject; it is an Anvil */
            if (((Anvil *)anvil)->dropping == 0) {
                Camera_ReleaseAny();
                state = SIGNPOST_ST_SHOWING;
            }
            break;
    }
}

/* 0x4f18c2 */
s32 SignPostAnimated::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    switch (msgId) {
        case MSG_USE:
            if (state == SIGNANIM_ST_IDLE && !hidden) {
                wolfFrozen = g_pWolf->HandleMessage(this, MSG_FREEZE, 0);
                state = SIGNANIM_ST_OPENING;
                return 1;
            }
            break;
        case MSG_QUERY_ACTION:
            if (state == SIGNANIM_ST_IDLE && !hidden && sender->GetClassId() == CLASSID_WOLF &&
                Vec3s_ManhattanDistXZ(&pos, &g_pWolf->pos) < distance)
                return CTX_READ;
            break;
        case MSG_FREEZE:
            wolfFrozen = 0;
            state = SIGNANIM_ST_IDLE;
            return 1;
        case MSG_SIGN_REVEAL:
            hidden = 0;
            SetVisible(1);
            PlayAnim(SIGNANIM_ANIM_HIT, 0, 0);
            state = SIGNANIM_ST_REVEALING;
            break;
    }
    return 0;
}

/* 0x4f1a50 */
s32 SignPostSimple::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    switch (msgId) {
        case MSG_USE:
            if (state == SIGN_ST_IDLE) {
                wolfFrozen = g_pWolf->HandleMessage(this, MSG_FREEZE, 0);
                state = SIGN_ST_OPENING;
                return 1;
            }
            break;
        case MSG_QUERY_ACTION:
            if (state == SIGN_ST_IDLE && sender->GetClassId() == CLASSID_WOLF &&
                Vec3s_ManhattanDistXZ(&pos, &g_pWolf->pos) < distance)
                return CTX_READ;
            break;
        case MSG_FREEZE:
            state = SIGN_ST_IDLE;
            return 1;
        case MSG_CINE_END:
            savedPos.x = pos.x;
            savedPos.y = pos.y;
            savedPos.z = pos.z;
            break;
        case MSG_CONTAINER_STATE:
            if ((s32)arg == CONTAINER_RELEASED) { /* cast kept: this message passes a number in its void * */
                savedPos.x = pos.x;
                savedPos.y = pos.y;
                savedPos.z = pos.z;
            }
            break;
        case MSG_TELEPORTED:
            return 1;
    }
    return 0;
}

/* 0x4f1bea */
s32 SignPost::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    switch (msgId) {
        case MSG_USE:
            if (state == SIGNPOST_ST_IDLE) {
                if (textVariant == -1) {
                    text = Text_GetClassString((u8)(g_signPostReadCount + textNum));
                    textVariant = g_signPostReadCount;
                    g_signPostReadCount++;
                } else {
                    text = Text_GetClassString((u8)(textVariant + textNum));
                }
                wolfFrozen = g_pWolf->HandleMessage(this, MSG_FREEZE, 0);
                state = SIGNPOST_ST_OPENING;
                return 1;
            }
            break;
        case MSG_QUERY_ACTION:
            if (state == SIGNPOST_ST_IDLE && sender->GetClassId() == CLASSID_WOLF &&
                Vec3s_ManhattanDistXZ(&pos, &g_pWolf->pos) < distance)
                return CTX_READ;
            break;
        case MSG_FREEZE:
            wolfFrozen = 0;
            state = SIGNPOST_ST_IDLE;
            return 1;
        case MSG_TELEPORTED:
            return 1;
    }
    return 0;
}

/* 0x4f1d86 */
ScnObject *SignPostSimple_Create(void *record)
{
    ScnLogic *obj = new SignPostSimple;
    obj = (ScnLogic *)obj->Init(record); /* cast kept: ScnLogic::Init returns the ScnObject base */
    return obj;
}

/* 0x4f1de8 */
ScnObject *SignPostAnimated_Create(void *record)
{
    ScnBody *obj = new SignPostAnimated;
    obj = obj->Init(record, 0);
    return obj;
}

/* 0x4f1e4c */
ScnObject *SignPost_Create(void *record)
{
    ScnLogic *obj = new SignPost;
    obj = (ScnLogic *)obj->Init(record); /* cast kept: ScnLogic::Init returns the ScnObject base */
    return obj;
}
