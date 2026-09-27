/* match-init: FishingRod_StaticInit_LinePoly */
/* T149 - original object FishingRod.cpp (guessed name).
 * Ranges: .text 0x4bdc30-0x4bf6a2 (FishingRod, CompositeRod, MagnetRod, SaladRod, then the static-initialiser thunks of
 * g_fishingLinePoly, then CompositeRod::Render 0x4beedb), .rdata 0x5760b8-0x576168 (the four vtables), .bss
 * 0x6cf63c-0x6cf65c (g_fishingLinePoly).
 * g_fishingLinePoly is defined after SaladRod_Create, where its $E thunks stand in the exe.
 * PAL PC FishingRod / CompositeRod family, 0x4bdc30-0x4bf6a2.
 * CompositeRod::Render: the first polygon submission is inline and the second stays out of line; the source-only
 * helper names (PolyBatcher::SubmitPolyInline, D3DApp::Render_SetTexture) describe those observed compilation
 * boundaries. */
/* BYTES: inline, layout, slot-group, slot-scope, temp. */
/* BYTES(layout): g_fishingLinePoly (definition position): defined after SaladRod_Create so its static-initialiser thunks follow it, as in the exe */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Mat44;
#include "../sdk/d3d7.h"
#include "../sdk/crt.h"

struct FlameVertex {
    float x, y, z, rhw;
    u32 diffuse, specular;
    float u, v;
};
struct FlatVertex {
    float x, y, z, rhw;
    u32 diffuse, specular;
};
struct XformedVertex {
    float x, y, z, rhw;
};
class Instance;
struct Animator;
u32 Anim_Start(Instance *instance, Animator *animator, u16 id, u32 options);

#define SDW_MEMBERS_ScnObject            \
    static void *operator new(u32 size); \
    void AttachTo(ScnObject *parent, u8 joint, Vec3s *offset, Vec3s *rot, u32 a, u32 b);


#define SDW_MEMBERS_RodParts                                                                                    \
    u32 Occupied()                                                                                              \
    {                                                                                                           \
        return (u32)rod |                                                                                       \
               (u32)bait; /* cast kept: the occupancy test ORs the two pointers as integers, as the original */ \
    }                                                                                                           \
    s32 Matches(u16 a, u16 b)                                                                                   \
    {                                                                                                           \
        if (rodClass == a && baitClass == b)                                                                    \
            return 1;                                                                                           \
        if (rodClass == b && baitClass == a)                                                                    \
            return 1;                                                                                           \
        return 0;                                                                                               \
    }                                                                                                           \
    void Get(ScnObject **out)                                                                                   \
    {                                                                                                           \
        out[0] = rod;                                                                                           \
        out[1] = bait;                                                                                          \
    }                                                                                                           \
    void Set(ScnObject *a, ScnObject *b)                                                                        \
    {                                                                                                           \
        rod = a;                                                                                                \
        bait = b;                                                                                               \
    }                                                                                                           \
    void Clear()                                                                                                \
    {                                                                                                           \
        rod = 0;                                                                                                \
        bait = 0;                                                                                               \
    }
#define SDW_MEMBERS_Mat44 Mat44();
#define SDW_MEMBERS_PolyBatcher    \
    void SubmitPoly(RenderPoly *); \
    void SubmitPolyInline(RenderPoly *);
#define SDW_MEMBERS_D3DApp                                        \
    void SetTransform(u32 state, Mat44 *m);                       \
    IDirect3DDevice7 *GetDevice();                                \
    void DrawTriangleList(void *vertices, u32 count);             \
    void Render_DrawPrimitive(u32, u32, void *, u32);             \
    void Render_SetTexture(Texture *texture, volatile u32 stage); \
    void Render_SetStateFlags(u32);                               \
    void Render_ClearStateFlags(u32);
#define SDW_MEMBERS_RenderPoly RenderPoly(); /* virtual ~RenderPoly() is generated */
#include "../sdk/ddraw.h"
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_ISINWORLD 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_ISINWORLD
#define SDW_INLINE_SCNOBJECT_INSTFLAGS_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_INSTFLAGS_U16
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_GETANIMID 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_GETANIMID
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_SHADOW_SETVISIBLE_S32 1
#include "../engine/shadow_inlines.h"
#undef SDW_INLINE_SHADOW_SETVISIBLE_S32
#define SDW_INLINE_D3DAPP_SETTRANSFORM_U32_MAT44 1
#include "../app/d3dapp_inlines.h"
#undef SDW_INLINE_D3DAPP_SETTRANSFORM_U32_MAT44
#define SDW_INLINE_D3DAPP_GETDEVICE 1
#include "../app/d3dapp_inlines.h"
#undef SDW_INLINE_D3DAPP_GETDEVICE
#define SDW_INLINE_D3DAPP_DRAWTRIANGLELIST_VOID_U32 1
#include "../app/d3dapp_inlines.h"
#undef SDW_INLINE_D3DAPP_DRAWTRIANGLELIST_VOID_U32
#define SDW_INLINE_SCNOBJECT_GETPARENT 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETPARENT
extern u8 g_sharedScratch[];
#include "world_draw.h"
#include "../engine/fixed_math.h"
#include "instance.h"
#include "../engine/draw2d.h"
#include "../engine/screen.h"
extern "C" s32 Coll_BoxGroundQuery(CollBox *box, s32 *height, ScnObject *self, u8 mode, ScnObject **outObject);
#include "instance.h"
#define SDW_INLINE_SCNOBJECT_SETATTACHMENT_U8_CONST_VEC3S_CONST_VEC3S_S32_CONST_VEC3S 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETATTACHMENT_U8_CONST_VEC3S_CONST_VEC3S_S32_CONST_VEC3S

void FishingRod::CalcTipPos(const Vec3s *position, const Vec3s *rotation, Vec3s *out)
{
    /* cast kept: g_sharedScratch is one byte buffer that each user lays out its own way */
    SharkFxScratch *scratch = (SharkFxScratch *)g_sharedScratch;
    Mat34s_FromEulerScaled(rotation, &scratch->m, 0);
    scratch->offset.x = 0;
    scratch->offset.y = -80;
    scratch->offset.z = -230;
    Mat34s_TransformVec3s(&scratch->m, &scratch->offset, &scratch->out);
    out->x = scratch->out.x + position->x;
    out->y = scratch->out.y + position->y;
    out->z = scratch->out.z + position->z;
}
s32 FishingRod::IsTipBlocked(const Vec3s *position, const Vec3s *rotation)
{
    Vec3s tip;
    CollBox box;
    s32 height;
    CalcTipPos(position, rotation, &tip);
    box.flags = 0;
    if (position->x < tip.x) {
        box.min.x = position->x;
        box.max.x = tip.x;
    } else {
        box.min.x = tip.x;
        box.max.x = position->x;
    }
    box.min.x -= 15;
    box.max.x += 15;
    if (position->z < tip.z) {
        box.min.z = position->z;
        box.max.z = tip.z;
    } else {
        box.min.z = tip.z;
        box.max.z = position->z;
    }
    box.min.z -= 15;
    box.max.z += 15;
    box.min.y = tip.y - 15;
    box.max.y = tip.y + 40;
    return Coll_BoxGroundQuery(&box, &height, GetParent(), CQ_STATIC, 0);
}
s32 FishingRod::GetLineFloorY(Vec3s *position)
{
    return World_GroundYRay(position, 1) - 25;
}
void FishingRod::BeginFishing()
{
    rodFlags.lineOut = 1;
    lineLen = 0;
}
void FishingRod::EndFishing()
{
    rodFlags.lineOut = 0;
}
void FishingRod::Update()
{
    switch (state) {
        case FR_ST_CASTING:
            if (AnimFlags(ANIM_F_FINISHED)) {
                state = FR_ST_FISHING;
                PlayAnim(ACANNE01_ANIM_FISH2, 1, 1);
                BeginFishing();
            }
            break;
        case FR_ST_RETRACTING:
            if (AnimFlags(ANIM_F_FINISHED)) {
                state = FR_ST_IDLE;
                PlayAnim(ACANNE01_ANIM_LINK, 0, 1);
            }
            break;
    }
    AdvanceAnim();
}
s32 FishingRod::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    s32 length;
    switch (msgId) {
        case MSG_QUERY_ACTION:
            if (sender->GetClassId() == CLASSID_WOLF)
                return CTX_PICKUP;
            break;
        case MSG_QUERY_HELD_ACTION:
            return HELD_FISHINGROD;
        case MSG_HELD_STATE_BEGIN:
            state = FR_ST_CASTING;
            PlayAnim(ACANNE01_ANIM_FISH1, 0, 1);
            return 1;
        case MSG_HELD_STATE_END:
            if (rodFlags.lineOut) {
                EndFishing();
                state = FR_ST_RETRACTING;
                PlayAnim(ACANNE01_ANIM_FISH6, 0, 1);
            } else {
                state = FR_ST_IDLE;
                PlayAnim(ACANNE01_ANIM_LINK, 0, 1);
            }
            return 1;
        case MSG_PICKUP:
            /* cast kept: HandleMessage's arg is a void *; MSG_PICKUP passes the joint number in it */
            AttachTo(sender, (u8)(u32)arg, 0, 0, 0, 0);
            shadow.SetVisible(0);
            PlayAnim(ACANNE01_ANIM_LINK, 0, 0);
            return 1;
        case MSG_DROP: {
            /* cast kept: HandleMessage's arg is a void *; MSG_DROP passes the drop record */
            DropMsgArg *drop = (DropMsgArg *)arg;
            Detach();
            SetPosition(&drop->pos);
            shadow.SetVisible(1);
            state = FR_ST_IDLE;
            PlayAnim(ACANNE01_ANIM_OBJET, 0, 0);
            return 1;
        }
        case MSG_CARRY_ANIM:
            if (rodFlags.lineOut) {
                switch ((s32)arg) { /* cast kept: HandleMessage's arg is a void *; this message passes a number in it */
                    case WOLF_CUE_FISH_A:
                        if (GetAnimId() != ACANNE01_ANIM_FISH3)
                            PlayAnim(ACANNE01_ANIM_FISH3, 1, 1);
                        break;
                    case WOLF_CUE_FISH_B:
                        if (GetAnimId() != ACANNE01_ANIM_FISH4)
                            PlayAnim(ACANNE01_ANIM_FISH4, 1, 1);
                        break;
                    case WOLF_CUE_FISH_C:
                        if (GetAnimId() != ACANNE01_ANIM_FISH5)
                            PlayAnim(ACANNE01_ANIM_FISH5, 1, 1);
                        break;
                    default:
                        if (GetAnimId() != ACANNE01_ANIM_FISH2)
                            PlayAnim(ACANNE01_ANIM_FISH2, 1, 1);
                        break;
                }
            }
            return 1;
        case MSG_CONTAINER_STATE:
            switch ((s32)arg) { /* cast kept: HandleMessage's arg is a void *; this message passes a number in it */
                case CONTAINER_RELEASED:
                    homePos = pos;
                    break;
            }
            return 1;
        case MSG_CHECKPOINT_ROLLBACK:
            Reset();
            return 1;
        case MSG_ROD_REEL_IN:
            /* cast kept: HandleMessage's arg is a void *; this message passes the length in it */
            length = lineLen - (s32)arg;
            if (length < 0)
                length = 0;
            lineLen = (s16)length;
            return 1;
        case MSG_ROD_REEL_OUT:
            /* cast kept: HandleMessage's arg is a void *; this message passes the length in it */
            length = lineLen + (s32)arg;
            if (length > 575)
                length = 575;
            lineLen = (s16)length;
            return 1;
        case MSG_ROD_TIP_BLOCKED:
            /* cast kept: HandleMessage's arg is a void *; this message passes the position */
            return IsTipBlocked((Vec3s *)arg, &sender->rot);
    }
    return 0;
}
void FishingRod::Reset()
{
    lineLen = 0;
    shadow.SetVisible(1);
    state = FR_ST_IDLE;
    PlayAnim(ACANNE01_ANIM_OBJET, 0, 0);
    if (rodFlags.lineOut)
        EndFishing();
    if (IsInWorld())
        SetPosition(&homePos);
}
void FishingRod::PostLoadInit()
{
    shadow.radius = 15;
    shadow.SetVisible(1);
    if (IsInWorld())
        SnapToGround(1);
    lineLen = 0;
    state = FR_ST_IDLE;
    PlayAnim(ACANNE01_ANIM_OBJET, 0, 0);
    rodFlags.lineOut = 0;
    homePos = pos;
}
ScnObject *FishingRod_Create(void *record)
{
    FishingRod *object = new FishingRod;
    object = (FishingRod *)object->Init(record, 0); /* cast kept: Init returns this as a ScnBody * */
    return object;
}
void CompositeRod::BeginFishing()
{
    rodFlags.lineOut = 1;
    lineLen = 0;
    rodParts.bait->HandleMessage(this, MSG_INVENTORY_TAKE_OUT, 0);
    rodParts.bait->AddToWorld(0);
    /* cast kept: HandleMessage's arg is a void *; MSG_PICKUP passes the joint number in it */
    rodParts.bait->HandleMessage(this, MSG_PICKUP, (void *)4);
}
void CompositeRod::EndFishing()
{
    DropMsgArg drop;
    rodFlags.lineOut = 0;
    drop.pos = pos;
    drop.placed = 0;
    drop.flag1 = 1;
    rodParts.bait->HandleMessage(this, MSG_DROP, &drop);
    rodParts.bait->HandleMessage(this, MSG_INVENTORY_STORED, 0);
    rodParts.bait->RemoveFromWorld();
}
void CompositeRod::Update()
{
    s32 ground;
    Vec3s point;
    FishingRod::Update();
    if (rodFlags.lineOut) {
        point.x = 0;
        point.y = lineLen;
        point.z = 0;
        rodParts.bait->SetAttachment(4, 0, 0, 1, &point);
        CalcTipPos(&GetParent()->pos, &GetParent()->rot, &point);
        ground = GetLineFloorY(&point);
        if (ground < point.y + lineLen) {
            if (ground < point.y)
                ground = point.y;
            lineLen = (s16)(ground - point.y);
            point.y = (s16)ground;
        } else
            point.y += lineLen;
        rodParts.bait->SetPosition(&point);
    }
}
/* BYTES(slot-group, inferred): locals grouped in Query / w only to pin the original frame offsets */
s32 CompositeRod::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    struct Query {
        u32 occupied;
        u16 a, b;
    };
    struct Work {
        ScnObject **objects;
        Query *query;
    } w;
    switch (msgId) {
        case SCN_MSG_ITEM_MATCH_QUERY:
            /* cast kept: HandleMessage's arg is a void *; this message passes the query record */
            w.query = (Query *)arg;
            w.query->occupied = rodParts.Occupied();
            return rodParts.Matches(w.query->a, w.query->b);
        case MSG_COMPOSITE_PARTS_QUERY:
            /* cast kept: HandleMessage's arg is a void *; this message passes the query record */
            w.query = (Query *)arg;
            w.query->occupied = rodParts.Occupied();
            w.query->a = rodParts.rodClass;
            w.query->b = rodParts.baitClass;
            return 1;
        case MSG_ITEM_SPLIT_QUERY:
            /* cast kept: HandleMessage's arg is a void *; this message passes the two part objects */
            w.objects = (ScnObject **)arg;
            rodParts.Get(w.objects);
            return 1;
        case MSG_ITEM_COMBINE:
            /* cast kept: HandleMessage's arg is a void *; this message passes the two part objects */
            w.objects = (ScnObject **)arg;
            if (w.objects[0]->GetClassId() == CLASSID_FISHINGROD)
                rodParts.Set(w.objects[0], w.objects[1]);
            else
                rodParts.Set(w.objects[1], w.objects[0]);
            return 1;
        case MSG_ITEM_CONSUMED:
            rodParts.Clear();
            return 1;
        case MSG_QUERY_NEAREST_TARGET:
            if (rodFlags.lineOut)
                return rodParts.bait->HandleMessage(this, msgId, arg);
            break;
        default:
            return FishingRod::HandleMessage(sender, msgId, arg);
    }
    return 0;
}
void CompositeRod::Reset()
{
    FishingRod::Reset();
}
void CompositeRod::InitParts(u16 baitClass)
{
    RodParts *parts = &rodParts;
    parts->rodClass = CLASSID_FISHINGROD;
    parts->baitClass = baitClass;
    parts->rod = 0;
    parts->bait = 0;
    RemoveFromWorld();
    state = FR_ST_IDLE;
    PlayAnim(ACANNE01_ANIM_OBJET, 0, 0);
    rodFlags.lineOut = 0;
}
void MagnetRod::PostLoadInit()
{
    InitParts(CLASSID_MAGNET);
}
ScnObject *MagnetRod_Create(void *record)
{
    MagnetRod *object = new MagnetRod;
    object = (MagnetRod *)object->Init(record, 0); /* cast kept: Init returns this as a ScnBody * */
    return object;
}
void SaladRod::PostLoadInit()
{
    InitParts(CLASSID_SALAD);
}
ScnObject *SaladRod_Create(void *record)
{
    SaladRod *object = new SaladRod;
    object = (SaladRod *)object->Init(record, 0); /* cast kept: Init returns this as a ScnBody * */
    return object;
}
/* 0x6cf63c .bss - defined after SaladRod_Create: its static-initialiser thunks ($E4/$E1/$E3/$E2, 0x4bee9c..0x4beeda)
 * sit between SaladRod_Create and CompositeRod::Render in the exe, i.e. where the definition stands in the file */
RenderPoly g_fishingLinePoly;

/* ---- CompositeRod::Render and its helpers */
/* Volatile applies to this source-only inline value parameter. It preserves the */
/* original stage stack store/read; it is not a claim about developer spelling. */
/* BYTES(temp): volatile by-value parameter: the original copies the stage to a stack slot before reading texture and this */
inline void D3DApp::Render_SetTexture(Texture *texture, volatile u32 stage)
{
    pD3DDevice->SetTexture(stage, texture->surface);
}
extern IDirect3DVertexBuffer7 *g_pVertexBufSrc, *g_pVertexBufXf;
/* BYTES(slot-scope, inferred): the nested block(s) only order the frame: their locals are allocated after the enclosing scope's */
/* BYTES(inline): source-only inline: the original expands the first submission and calls the second */
#define SDW_INLINE_POLYBATCHER_SUBMITPOLYINLINE_RENDERPOLY 3
#include "../engine/polybatcher_inlines.h"
#undef SDW_INLINE_POLYBATCHER_SUBMITPOLYINLINE_RENDERPOLY

/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; unknown20 fill gaps */
void CompositeRod::Render(Camera *view)
{
    ScnMobile::Render(view);
    if (InstFlags(INST_F_DRAWN) && rodFlags.lineOut) {
        /* The work record keeps the original's local layout. The slot at -0x20 */
        /* has no accesses in the original and is deliberately left untouched. */
        struct Work {
            Mat44 world;
            u32 unknown20;
            u32 size;
            Vec3f offset;
            XformedVertex *vertices;
            float width;
            void *data;
        } w;
        g_pVertexBufSrc->Lock(DDLOCK_WAIT, &w.data, &w.size);
        memset(w.data, 0, 0x18);
        g_pVertexBufSrc->Unlock();
        g_pD3DAppMain->SetTransform(D3DTRANSFORMSTATE_VIEW, &view->viewMat);
        w.world = rodParts.bait->attachLink->matrix;
        w.world.MulInPlace(g_matUnk6d5428);
        g_pD3DAppMain->SetTransform(D3DTRANSFORMSTATE_WORLD, &w.world);
        g_pVertexBufXf->ProcessVertices(D3DVOP_TRANSFORM, 0, 1, g_pVertexBufSrc, 0, g_pD3DAppMain->GetDevice(),
                                        D3DPV_DONOTCOPYDATA);
        w.offset.x = w.offset.z = 0;
        w.offset.y = (float)lineLen;
        w.world.m[3][0] += w.offset.x;
        w.world.m[3][1] += w.offset.y;
        w.world.m[3][2] += w.offset.z;
        g_pD3DAppMain->SetTransform(D3DTRANSFORMSTATE_WORLD, &w.world);
        g_pVertexBufXf->ProcessVertices(D3DVOP_TRANSFORM, 1, 1, g_pVertexBufSrc, 1, g_pD3DAppMain->GetDevice(),
                                        D3DPV_DONOTCOPYDATA);
        g_pVertexBufXf->Lock(DDLOCK_WAIT, &w.data, &w.size);
        /* cast kept: vertex memory is untyped; the transformed buffer holds XformedVertex */
        w.vertices = (XformedVertex *)w.data;
        g_fishingLinePoly.type = RPOLY_BLEND;
        /* cast kept: vertex memory is untyped; the line polygon's are FlatVertex */
        ((FlatVertex *)g_fishingLinePoly.verts)[0].diffuse = ((FlatVertex *)g_fishingLinePoly.verts)[1].diffuse =
            ((FlatVertex *)g_fishingLinePoly.verts)[2].diffuse = 0x80808080;
        /* cast kept: vertex memory is untyped; the line polygon's are FlatVertex */
        ((FlatVertex *)g_fishingLinePoly.verts)[0].specular = ((FlatVertex *)g_fishingLinePoly.verts)[1].specular =
            ((FlatVertex *)g_fishingLinePoly.verts)[2].specular = 0xff000000;
        w.width = g_screen.ScaleX(1);
        memcpy(g_fishingLinePoly.verts, w.vertices, 0x10);
        /* cast kept: vertex memory is untyped; the line polygon's are FlatVertex */
        memcpy((FlatVertex *)g_fishingLinePoly.verts + 1, w.vertices, 0x10);
        /* cast kept: vertex memory is untyped; the line polygon's are FlatVertex */
        memcpy((FlatVertex *)g_fishingLinePoly.verts + 2, w.vertices + 1, 0x10);
        /* cast kept: vertex memory is untyped; the line polygon's are FlatVertex */
        ((FlatVertex *)g_fishingLinePoly.verts)[1].x = w.width + ((FlatVertex *)g_fishingLinePoly.verts)[1].x;
        g_pPolyBin->SubmitPolyInline(&g_fishingLinePoly);
        memcpy(g_fishingLinePoly.verts, w.vertices + 1, 0x10);
        /* cast kept: vertex memory is untyped; the line polygon's are FlatVertex */
        ((FlatVertex *)g_fishingLinePoly.verts)[2].x = w.width + ((FlatVertex *)g_fishingLinePoly.verts)[2].x;
        g_pPolyBin->SubmitPoly(&g_fishingLinePoly);
        g_pVertexBufXf->Unlock();
    }
}
