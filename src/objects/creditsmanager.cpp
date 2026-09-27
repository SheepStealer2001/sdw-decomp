/* PAL PC CreditsManager. */
/* BYTES: slot-scope. */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
#include "../sdk/d3d7.h"
#include "../sdk/ddraw.h"
class Mat44;
#include "../sdk/crt.h"
#define SDW_MEMBERS_D3DApp                                                  \
    void CreateVB(D3DVERTEXBUFFERDESC *desc, IDirect3DVertexBuffer7 **out); \
    IDirect3DDevice7 *GetDevice();                                          \
    void SetTransform(u32 state, Mat44 *matrix);
#define SDW_MEMBERS_ScnObject       \
    static void *operator new(u32); \
    void SetUpdateMode(s32 mode);
#define SDW_MEMBERS_Mat44 Mat44();


#include "sdw_classes.h"
#include "../engine/draw2d.h"
#include "../engine/text.h"
#include "../engine/progress.h"
#include "../engine/screen.h"
#include "world_draw.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_D3DAPP_CREATEVB_D3DVERTEXBUFFERDESC_IDIRECT3DVERTEXBUFFER7 2
#define SDW_INLINE_D3DAPP_GETDEVICE 1
#define SDW_INLINE_D3DAPP_SETTRANSFORM_U32_MAT44 1
#include "../app/d3dapp_inlines.h"
#undef SDW_INLINE_D3DAPP_CREATEVB_D3DVERTEXBUFFERDESC_IDIRECT3DVERTEXBUFFER7
#undef SDW_INLINE_D3DAPP_GETDEVICE
#undef SDW_INLINE_D3DAPP_SETTRANSFORM_U32_MAT44
#define SDW_INLINE_SCNOBJECT_INSTFLAGS_U16 1
#define SDW_INLINE_SCNOBJECT_ISVISIBLE 1
#define SDW_INLINE_SCNOBJECT_SETVISIBLE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_INSTFLAGS_U16
#undef SDW_INLINE_SCNOBJECT_ISVISIBLE
#undef SDW_INLINE_SCNOBJECT_SETVISIBLE_S32
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32
#define SDW_INLINE_PROGRESS_CURRENTLEVEL 1
#include "../engine/progress_inlines.h"
#undef SDW_INLINE_PROGRESS_CURRENTLEVEL
#define SDW_INLINE_SCREEN_LAYERS60_U16 1
#include "../engine/screen_inlines.h"
#undef SDW_INLINE_SCREEN_LAYERS60_U16
/* T130 .bss 0x6cf5fc..0x6cf604, in recovered address order. */
struct CreditsBuffers {
    IDirect3DVertexBuffer7 *source, *destination;
};
CreditsBuffers g_creditsBuffers;
#define g_creditsSrcVB g_creditsBuffers.source
#define g_creditsDstVB g_creditsBuffers.destination
extern s32 g_dtRawMs;
void CreditsManager_CreateProjectionVBs();
#define SDW_INLINE_FREE_PROPERTYU32_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_PROPERTYU32_VOID_U32
void CreditsManager::PostLoadInit()
{
    SetUpdateMode(SCN_UPD_ALWAYS);
    SetUpdateMode(SCN_UPD_CINE);
    SetVisible(0);
    prevVisible = 0;
    void *properties = record;
    color = PropertyU32(properties, 0);
    textJust = (u8)PropertyU32(properties, 4);
    text = Text_GetClassString((u8)PropertyU32(properties, 8));
    timeAppearMs = (u16)PropertyU32(properties, 12);
    timeDisappearMs = (u16)PropertyU32(properties, 16);
    active = 0;
    timerMs = 0;
    state = CREDITS_WAIT;
    posLatched = 0;
    CreditsManager_CreateProjectionVBs();
}
void CreditsManager::Update()
{
    s8 visible = (s8)IsVisible();
    if ((s8)active) {
        switch ((s8)state) {
            case CREDITS_WAIT:
                if (visible && !(s8)prevVisible)
                    state = CREDITS_FADE_IN;
                break;
            case CREDITS_FADE_IN:
                timerMs += (s16)g_dtRawMs;
                if (timerMs >= timeAppearMs) {
                    timerMs = 0;
                    state = CREDITS_SHOWN;
                }
                break;
            case CREDITS_SHOWN:
                if (!visible && (s8)prevVisible) {
                    state = CREDITS_FADE_OUT;
                    SetVisible(1);
                }
                break;
            case CREDITS_FADE_OUT:
                timerMs += (s16)g_dtRawMs;
                if (timerMs >= timeDisappearMs) {
                    Text_SetFont(FONT_GAME);
                    Text_SetColor(0x808080);
                    SetUpdateMode(SCN_UPD_NEVER);
                    SetVisible(0);
                    state = CREDITS_WAIT;
                    timerMs = 0;
                    active = 0;
                }
                break;
        }
        prevVisible = visible;
    }
}
s32 CreditsManager::HandleMessage(ScnObject *, u32 msgId, void *)
{
    switch (msgId) {
        case MSG_CINE_PLACE:
            active = 1;
            timerMs = 0;
            if (IsVisible())
                state = CREDITS_FADE_IN;
            else
                state = CREDITS_WAIT;
            posLatched = 0;
            break;
        case MSG_CINE_END:
            active = 0;
            SetVisible(0);
            break;
    }
    return 0;
}
ScnObject *CreditsManager_Create(void *record)
{
    CreditsManager *object = new CreditsManager;
    object = (CreditsManager *)object->Init(record); /* cast kept: Init returns the object as its base class */
    return object;
}
void CreditsManager_CreateProjectionVBs()
{
    D3DVERTEXBUFFERDESC desc;
    if (!g_creditsSrcVB) {
        memset(&desc, 0, sizeof(desc));
        desc.dwSize = sizeof(desc);
        desc.dwCaps = D3DVBCAPS_DONOTCLIP;
        desc.dwFVF = D3DFVF_XYZ;
        desc.dwNumVertices = 1;
        g_pD3DAppMain->CreateVB(&desc, &g_creditsSrcVB);
    }
    if (!g_creditsDstVB) {
        memset(&desc, 0, sizeof(desc));
        desc.dwSize = sizeof(desc);
        desc.dwCaps = D3DVBCAPS_DONOTCLIP;
        desc.dwFVF = D3DFVF_XYZRHW;
        desc.dwNumVertices = 1;
        g_pD3DAppMain->CreateVB(&desc, &g_creditsDstVB);
    }
}

u16 Text_CountWrappedLines(const char *);
/* a colour word whose channel bytes are also read and written one by one */
union ColorBytes {
    u32 value;
    u8 c[4];
};
/* BYTES(slot-scope, inferred): the nested block(s) only order the frame: their locals are allocated after the enclosing scope's */
void CreditsManager::Render(Camera *view)
{
    void *buffer;
    u16 lines;
    s16 textHeight, y, x, textWidth;
    ColorBytes tint;
    u16 lineLength;
    Instance_SetRigidTransforms(Inst(), view, 0, 0);
    if ((s8)state && !InstFlags(INST_F_DRAWN))
        return;
    {
        Text_SetFont(FONT_DEBUG);
        if (g_pCurFont->cellW == 8 && g_pProgress->CurrentLevel() == SCENE_INTRO)
            Font_LoadFromRes(FONT_DEBUG, DAV_IDI_IGLFONTE, FONT_KIND_GAME);
        switch (state) {
            case CREDITS_FADE_IN:
                if (g_pProgress->CurrentLevel() == SCENE_INTRO)
                    Text_SetFont(FONT_DEBUG);
                else
                    Text_SetFont(FONT_GAME);
                tint.c[0] = (u8)(colorBytes[0] * ((float)timerMs / timeAppearMs));
                tint.c[1] = (u8)(colorBytes[1] * ((float)timerMs / timeAppearMs));
                tint.c[2] = (u8)(colorBytes[2] * ((float)timerMs / timeAppearMs));
                Text_SetColor(tint.value);
                break;
            case CREDITS_SHOWN:
                Text_SetFont(FONT_GAME);
                Text_SetColor(color);
                break;
            case CREDITS_FADE_OUT:
                if (g_pProgress->CurrentLevel() == SCENE_INTRO)
                    Text_SetFont(FONT_DEBUG);
                else
                    Text_SetFont(FONT_GAME);
                tint.c[0] = (u8)(colorBytes[0] * (1.0f - (float)timerMs / timeAppearMs));
                tint.c[1] = (u8)(colorBytes[1] * (1.0f - (float)timerMs / timeAppearMs));
                tint.c[2] = (u8)(colorBytes[2] * (1.0f - (float)timerMs / timeAppearMs));
                Text_SetColor(tint.value);
                break;
        }
        switch (posLatched) {
            case 0: {
                u32 bytes;
                float *projectedVertex, *sourceVertex;
                float projectedY, projectedX;
                Mat44 identity;
                Vec3s position;
                position = pos;
                g_creditsSrcVB->Lock(DDLOCK_WAIT, &buffer, &bytes);
                /* cast kept: vertex memory is untyped; this buffer holds one D3DFVF_XYZ vertex */
                sourceVertex = (float *)buffer;
                sourceVertex[0] = (float)position.x;
                sourceVertex[1] = (float)position.y;
                sourceVertex[2] = (float)position.z;
                g_creditsSrcVB->Unlock();
                identity.SetIdentity();
                g_pD3DAppMain->SetTransform(D3DTRANSFORMSTATE_WORLD, &identity);
                g_pD3DAppMain->SetTransform(D3DTRANSFORMSTATE_VIEW, &view->viewMatCopy);
                g_creditsDstVB->ProcessVertices(D3DVOP_TRANSFORM, 0, 1, g_creditsSrcVB, 0, g_pD3DAppMain->GetDevice(),
                                                D3DPV_DONOTCOPYDATA);
                g_creditsDstVB->Lock(DDLOCK_WAIT, &buffer, &bytes);
                /* cast kept: vertex memory is untyped; this one is a D3DFVF_XYZRHW vertex */
                projectedVertex = (float *)buffer;
                projectedX = projectedVertex[0];
                projectedY = projectedVertex[1];
                g_creditsDstVB->Unlock();
                x = (s16)(projectedX * 512.0f / g_pViewFrustum->viewportWidth);
                y = (s16)(projectedY * 240.0f / g_pViewFrustum->viewportHeight);
                if (rot.x - prevRotX > 0) {
                    latchedScreenX = x;
                    latchedScreenY = y;
                    posLatched = 1;
                }
            } break;
            case 1:
                x = latchedScreenX;
                y = latchedScreenY;
                if (rot.x - prevRotX < 0)
                    posLatched = 0;
                break;
        }
        Text_SetWindow(g_screen.Layers60(2), 0, 0, 512, 240, 0);
        lines = Text_CountWrappedLines(text);
        lineLength = Text_MaxLineLength(text);
        textWidth = lineLength * g_pCurFont->glyphWidth;
        textHeight = lines * g_pCurFont->lineHeight;
        x -= (s16)(textWidth / 2);
        y -= (s16)(g_pCurFont->lineHeight / 2);
        if (x < 512 && x + textWidth > 0 && y < 240 && y + textHeight > 0) {
            Text_SetNoClipOnce();
            Text_SetWindow(g_screen.Layers60(2), x, y, textWidth, textHeight, 1);
            Text_Printf(textJust, text);
        }
        prevRotX = rot.x;
    }
}
