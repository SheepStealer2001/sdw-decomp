/* T220 - original object Spot*.cpp (guessed; class LightSpot, sorts between SnowyGround and SuperButton), one TU.
 * .text 0x4f64c0-0x4f8891, .rdata 0x576cfc-0x576d38 (vtable + 6 float COMDATs), .bss 0x6cfadc-0x6cfb50.
 * LightSpot::Render and its inline helpers sit between MsgStub and Stub, where the original has them.
 * .bss names (object-private; VC6 orders .bss by a 1024-bucket hash of the names, later definition first inside a
 * bucket): the descriptive names g_lightSpotPoly and g_lightSpotVb would give the wrong order, so they are
 * g_lightSpotBeamFace (bucket 34) and g_lightSpotXyzVb (bucket 977) here; g_lightSpotUnknownArea is the 20
 * unreferenced bytes at 0x6cfb34. */
/* BYTES: bss-name, dead-code, inline, layout, slot-name, slot-scope, temp. */
/* BYTES(bss-name): named for its .bss hash key 34 */
/* BYTES(bss-name): named for its .bss hash key 977 */
/* BYTES(layout): placeholder: 20 unreferenced bytes, defined after g_lightSpotXyzVb (same bucket 977, the later definition gets the lower address) */
/* BYTES(inline, inferred): D3DApp::SetFlagsInline, PolyBatcher::SubmitBeamA / SubmitBeamB (__forceinline twins): __forceinline twins: the original expands these in Render */
/* match-init: 0x4f6841 */
/* (that is the _initterm root of g_lightSpotBeamFace, StaticInit_g_lightSpotPoly in the tables, given by address) */
/*
 * LightSpot (class 124, CLASSID_LIGHTSPOT, vtable 0x576cfc, sizeof 0x1248), SheepD3D.exe 0x4f64c0-0x4f8891: the
 * searchlight of Level 12 that gives Ralph away. It draws a cone of light between its own position and an aim point
 * on the floor, and sweeps that aim point along its TRAJECTORY. The only object it ever looks at is the level's
 * single CLASSID_ROBOT (101): when the robot answers message 0x4c and its position falls inside the lit cone, the
 * spot tells Sam (class 1) message 0x33 and locks on - it then snaps the aim point onto the robot every frame, for
 * as long as the robot stays inside the AUTHORIZEDBOX id list, and returns to the patrol path when it leaves.
 * Designer properties (lightSpotProps): AUTHORIZEDBOX +0, COLOR +4, NBHEIGHTSEGS +8, NUMSIDES +0xc, RADIUS +0x10,
 * TRAJECTORY +0x14.
 * The four static-initialiser thunks of g_lightSpotBeamFace are emitted from its definition.
 *
 * The inline helpers have no bodies of their own in the exe, so their names are not recovered; each is there because
 * its expansion gives the original's shapes: SetBoxCollide / SetIgnoreCullPlane a constant tested as `xor r,r; test`,
 * PropU32 its offset argument and its value in temps, BoxIdList::Load / ::Find their `this` in a temp,
 * D3DApp::CreateVB the DirectX SDK samples' "system memory unless TnL HAL" vertex-buffer idiom (also in bipbip and
 * Sfx_CreateVertexBuffers 0x52a89f). A shape that reproduces the bytes is a representation, not proof that the
 * original source read this way.
 */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "../sdk/d3d7.h"
#include "../sdk/ddraw.h"
#include "../sdk/win32.h"
#include "scenaric_props.h"
class Mat44;
#include "../sdk/crt.h"
class Mat44;
#define IsEqualGUID(a, b) (!memcmp((a), (b), 16)) /* sizeof(GUID); the type is incomplete here */

#include "../engine/id_list.h"
#include "../engine/scn_tools.h"
#include "../engine/load_dav.h"
#include "../engine/load_warmeshes.h"
#include "../engine/game_state.h"
#include "../engine/draw2d.h"
#include "../engine/screen.h"

#define SDW_MEMBERS_ScnObject                                                        \
    static void *operator new(u32 size);                                             \
    Vec3s *Pos()                                                                     \
    {                                                                                \
        Vec3s *point = &pos;                                                         \
        return point;                                                                \
    } /* inline: its value is a stack temp (0x4f7379); the spelling Render uses */   \
    void SetIgnoreCullPlane(s32 on) /* inline: SCN_OF_NO_PLANE_CULL 0x10 on / off */ \
    {                                                                                \
        if (on)                                                                      \
            flags |= SCN_OF_NO_PLANE_CULL;                                           \
        else                                                                         \
            flags &= (u16)~SCN_OF_NO_PLANE_CULL;                                     \
    }
#define SDW_MEMBERS_Mat44 Mat44(); /* 0x4077f0 Mat44_Ctor, empty and out of line */
#define SDW_MEMBERS_PolyBatcher                                              \
    void SubmitPoly(RenderPoly *poly); /* 0x415c80 PolyBatcher_SubmitPoly */ \
    void SubmitBeamA(RenderPoly *poly);                                      \
    void SubmitBeamB(RenderPoly *poly); /* inline, defined before Render */
#define SDW_MEMBERS_RenderPoly                                              \
    RenderPoly();                            /* 0x41aad0 RenderPoly_Ctor */ \
    /* virtual ~RenderPoly() is generated */ /* 0x41ac04 RenderPoly_Dtor */
#define SDW_MEMBERS_ZoneList \
    void Load(u16 id);       \
    Box *Find(Vec3s *p);
#define SDW_MEMBERS_D3DApp                                                  \
    void CreateVB(D3DVERTEXBUFFERDESC *desc, IDirect3DVertexBuffer7 **out); \
    IDirect3DDevice7 *GetDevice();                                          \
    void SetTransform(u32 state, Mat44 *m); /* inline */                    \
    void Render_SetStateFlags(u32);                                         \
    void Render_ClearStateFlags(u32);                                       \
    void SetFlagsInline(u32);                                               \
    void DrawPrimitiveInline(u32, u32, void *, u32);                        \
    void Render_SetTexture(Texture *, volatile u32);
#include "sdw_classes.h"
#define SDW_INLINE_SCNOBJECT_SETBOXCOLLIDE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETBOXCOLLIDE_S32
#define SDW_INLINE_ZONELIST_LOAD_U16 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_LOAD_U16
#define SDW_INLINE_ZONELIST_FIND_VEC3S 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_FIND_VEC3S
#define SDW_INLINE_D3DAPP_CREATEVB_D3DVERTEXBUFFERDESC_IDIRECT3DVERTEXBUFFER7 1
#define SDW_INLINE_D3DAPP_GETDEVICE 1
#define SDW_INLINE_D3DAPP_SETTRANSFORM_U32_MAT44 1
#include "../app/d3dapp_inlines.h"
#undef SDW_INLINE_D3DAPP_CREATEVB_D3DVERTEXBUFFERDESC_IDIRECT3DVERTEXBUFFER7
#undef SDW_INLINE_D3DAPP_GETDEVICE
#undef SDW_INLINE_D3DAPP_SETTRANSFORM_U32_MAT44
#include "../engine/property_math.h"

/* The DAV directory (DavHeader.dir): packed, its pointers sit at +6/+0xa/+0x12 (src/engine/load_dav.cpp), which the
 * struct generator cannot lay out, so it is declared here. */
#pragma pack(push, 1)
struct DavDirectory {
    u16 indexCount;
    u16 bitmapCount;
    u16 unk04;
    u16 *indices;
    DavBitmapRec *bitmaps; /* +0x0a 10-byte records (TexAtlas_GetPage) */
    u32 fileSize;
    u32 *idLists;
};
#pragma pack(pop)

s32 Scenaric_FindByClass(u16 classId, ScnObject **out, s32 max); /* 0x5145c5 */
u32 *Res_GetValidatedIdList(u16 resId, u16 *outCount);           /* 0x548381 */
extern u32 *g_screenLayerBase;                                   /* 0x585044 */
void Draw2D_TexRect(float z, float x0, float y0, float x1, float y1, s32 texIndex, float uTL, float vTL, u32 cTL,
                    float uBL, float vBL, u32 cBL, float uTR, float vTR, u32 cTR, float uBR, float vBR,
                    u32 cBR); /* 0x5242df */

/* A transformed vertex as ProcessVertices writes it (D3DFVF_XYZRHW, 0x10 bytes) and a textured, lit vertex as the
 * batcher wants it (D3DFVF_XYZRHW | DIFFUSE | SPECULAR | TEX1 = 0x1c4, 0x20 bytes). */
struct XyzrhwVertex {
    float x, y, z, rhw;
};
struct TlVertex {
    float x, y, z, rhw;
    u32 diffuse, specular;
    float u, v;
};
struct FlatVertex {
    float x, y, z, rhw;
    u32 diffuse, specular;
};
struct FlameVertex {
    float x, y, z, rhw;
    u32 diffuse, specular;
    float u, v;
};

/* One of the two textures the beam is drawn with: the colour written into every vertex and the page and corner UVs
 * of one DAV bitmap. This file's statics (.bss, no table names yet). */
struct LightSpotTex {
    u32 color;
    u32 page;
    float u0, v0, u1, v1;
};
extern RenderPoly g_lightSpotBeamFace;           /* 0x6cfadc  the one triangle every beam face is pushed through */
extern Box **g_lightSpotSamBoxes;                /* 0x6cfafc  id list 0x46: where Sam can be told about the robot */
extern u16 g_lightSpotSamBoxCount;               /* 0x6cfb00 */
extern LightSpotTex g_lightSpotTexB;             /* 0x6cfb04  DAV id list 0xf0 */
extern LightSpotTex g_lightSpotTexA;             /* 0x6cfb1c  DAV id list 0xef */
extern IDirect3DVertexBuffer7 *g_lightSpotXyzVb; /* 0x6cfb48  D3DFVF_XYZ, 41 vertices: the cone in object space */
extern IDirect3DVertexBuffer7 *g_lightSpotVbXf;  /* 0x6cfb4c  D3DFVF_XYZRHW: ProcessVertices' destination */

/* A designer property: the dword at record + 0x14 + off (lightSpotProps). Inline: the offset is a stack temp. */
#define SDW_INLINE_FREE_PROPU32_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_PROPU32_VOID_U32

/* 0x4f64c0 - vtable +0x04. Unlocked: if the robot answers 0x4c and stands in the lit cone, alert Sam and lock on;
 * otherwise walk the aim point along the trajectory - the waypoint loop is written out here rather than calling
 * TrajFollower_Step, because what moves is the aim point, not the object. Locked: keep the aim point on the robot
 * while it is inside an authorized box, and hold the beam's height at the path's own height. */
/* BYTES(dead-code): wrap is set and never read, as in the original */
/* BYTES(slot-name): names chosen for their stack slots; the local follower shadows the member on purpose */
void LightSpot::Update()
{
    /* the local names are chosen for their stack slots (tools/vc6_locals.py); `follower` shadows the member, which
     * the locked branch therefore reaches as this->follower */
    s32 dist2;
    s32 wrap;
    Vec3s velocity;
    Vec3s *next;
    Vec3s step;
    s32 arrived;
    s32 dy2;
    s32 dx;
    TrajFollower *follower;

    wrap = 0;
    follower = &this->follower;
    switch (locked) {
        case 0:
            if (robot && robot->HandleMessage(this, MSG_QUERY_CONTROLLED, 0) && IsPointLit(&robot->pos)) {
                ScnObject *sams[2];
                if (Scenaric_FindByClass(CLASSID_SAM, sams, 1))
                    sams[0]->HandleMessage(this, MSG_TRAP_STATE, 0);
                locked = 1;
                break;
            }
            if (!follower->traj)
                break;
            follower->advanced = 0;
            follower->moving = 1;
            do {
                next = &follower->traj->pts[follower->pointIndex];
                dx = next->x - aim.x;
                dy2 = next->z - aim.z;
                dist2 = dx * dx + dy2 * dy2;
                arrived = dist2 < follower->arriveRadiusSq;
                if (arrived) {
                    follower->pointIndex++;
                    follower->advanced = 1;
                    if (follower->pointIndex >= follower->traj->count) {
                        wrap = 1;
                        follower->pointIndex = 0;
                    }
                }
            } while (arrived);
            dist2 = (s32)sqrt((double)dist2);
            velocity.x = (s16)(dx * follower->speed / dist2);
            velocity.z = (s16)(dy2 * follower->speed / dist2);
            velocity.y = 0;
            Vec3s_ScaleByDt(&velocity, &step);
            aim.x = aim.x + step.x;
            aim.y = aim.y + step.y;
            aim.z = aim.z + step.z;
            break;
        case 1:
            if (!authBoxes.Find(&robot->pos))
                locked = 0;
            else
                aim = robot->pos;
            aim.y = this->follower.traj->pts[0].y;
            break;
    }
}

/* 0x4f677b - vtable +0x10: the spotlight answers nothing. */
s32 LightSpot::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    return 0;
}

/* 0x4f678a - vtable +0x14: unlocked again, aim point back on the first waypoint. */
void LightSpot::Reset()
{
    locked = 0;
    if (follower.traj && follower.traj->count)
        aim = follower.traj->pts[0];
}

/* 0x4f67dc - the class factory for CLASSID 124 "LightSpot": new LightSpot (the base vtables in turn, then
 * LightSpot's), then ScnLogic_Init(record) through vtable slot +0x20. */
ScnObject *LightSpot_Create(void *record)
{
    LightSpot *obj = new LightSpot;
    obj = (LightSpot *)obj->Init(record); /* cast kept: Init returns the object as its ScnObject base */
    return obj;
}

/* 0x4f6841-0x4f687f: the static initialiser of g_lightSpotBeamFace - the root StaticInit_g_lightSpotPoly (the _initterm
 * entry), the constructor call, the atexit registration and the destructor. VC6 generates all four from the plain
 * definition below; the file's `match-init:` line places the root and the matcher follows its calls. */
RenderPoly g_lightSpotBeamFace; /* 0x6cfadc  bucket 34   (g_lightSpotPoly) */
Box **g_lightSpotSamBoxes;      /* 0x6cfafc */
u16 g_lightSpotSamBoxCount;     /* 0x6cfb00 */
LightSpotTex g_lightSpotTexB;   /* 0x6cfb04 */
LightSpotTex g_lightSpotTexA;   /* 0x6cfb1c */
/* 0x6cfb34-0x6cfb48: 20 bytes nothing in the exe refers to; the original had a global here (zero-initialised .bss,
 * so its type is unknowable: opaque words). It must come after g_lightSpotXyzVb in the file: the two share hash
 * bucket 977, and inside a bucket the later definition gets the lower address. */
IDirect3DVertexBuffer7 *g_lightSpotXyzVb; /* 0x6cfb48  bucket 977  (g_lightSpotVb) */
u32 g_lightSpotUnknownArea[5];            /* 0x6cfb34 */
IDirect3DVertexBuffer7 *g_lightSpotVbXf;  /* 0x6cfb4c */

/* 0x4f6880 - vtable +0x00. The two vertex buffers and the two beam textures are shared by every LightSpot, so they
 * are only built once; then the six designer properties, the trajectory follower (speed 200, arrive radius 50) with
 * the aim point on waypoint 0, and the level's one Robot. The spot has no box collision and is never cut by the
 * near view plane, which is what lets the beam be drawn from inside it. */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py) */
void LightSpot::PostLoadInit()
{
    /* the local names are chosen for their stack slots (tools/vc6_locals.py) */
    D3DVERTEXBUFFERDESC desc;
    s32 count;
    DavBitmapRec *pic;
    u16 *rec;
    Trajectory *traj;
    u16 num;
    void **idlist;
    ScnObject *objList[64];
    u16 *id;

    if (!g_lightSpotXyzVb) {
        memset(&desc, 0, sizeof(desc));
        desc.dwSize = sizeof(desc);
        desc.dwCaps = D3DVBCAPS_DONOTCLIP;
        desc.dwFVF = D3DFVF_XYZ;
        desc.dwNumVertices = 41;
        g_pD3DAppMain->CreateVB(&desc, &g_lightSpotXyzVb);
    }
    if (!g_lightSpotVbXf) {
        memset(&desc, 0, sizeof(desc));
        desc.dwSize = sizeof(desc);
        desc.dwCaps = D3DVBCAPS_DONOTCLIP;
        desc.dwFVF = D3DFVF_XYZRHW;
        desc.dwNumVertices = 41;
        g_pD3DAppMain->CreateVB(&desc, &g_lightSpotVbXf);
    }
    /* cast kept: an export id list holds record pointers of any kind; this one lists boxes */
    g_lightSpotSamBoxes = (Box **)Scn_FindIdList(WAR_IDO_BOXSAMORANGE, &g_lightSpotSamBoxCount);
    /* cast kept: a DAV id list holds untyped resource pointers */
    idlist = (void **)Res_GetValidatedIdList(DAV_IDI_IGLLIGF_, &num);
    if (idlist && num) {
        g_lightSpotTexA.color = 0xe8d94e;
        id = (u16 *)*idlist; /* cast kept: this id list entry points at a u16 bitmap index */
        pic = &g_pDav->header->dir->bitmaps[*id];
        g_lightSpotTexA.page = TexAtlas_GetPage(pic);
        g_lightSpotTexA.u0 = Tex_CornerUV(0, pic->width - 1, pic->u);
        g_lightSpotTexA.v0 = Tex_CornerUV(0, pic->height - 1, pic->v);
        g_lightSpotTexA.u1 = Tex_CornerUV(pic->width - 1, pic->width - 1, pic->u);
        g_lightSpotTexA.v1 = Tex_CornerUV(pic->height - 1, pic->height - 1, pic->v);
    }
    /* cast kept: a DAV id list holds untyped resource pointers */
    idlist = (void **)Res_GetValidatedIdList(DAV_IDI_IGLLIGB_, &num);
    if (idlist && num) {
        g_lightSpotTexB.color = 0xa0a30;
        id = (u16 *)*idlist; /* cast kept: this id list entry points at a u16 bitmap index */
        pic = &g_pDav->header->dir->bitmaps[*id];
        g_lightSpotTexB.page = TexAtlas_GetPage(pic);
        g_lightSpotTexB.u0 = Tex_CornerUV(0, pic->width - 1, pic->u);
        g_lightSpotTexB.v0 = Tex_CornerUV(0, pic->height - 1, pic->v);
        g_lightSpotTexB.u1 = Tex_CornerUV(pic->width - 1, pic->width - 1, pic->u);
        g_lightSpotTexB.v1 = Tex_CornerUV(pic->height - 1, pic->height - 1, pic->v);
    }
    SetBoxCollide(0);
    SetIgnoreCullPlane(1);
    rec = record;
    radius = PropU32(rec, 0x10);
    numSides = (u16)PropU32(rec, 0xc);
    nbHeightSegs = (u16)PropU32(rec, 8);
    color = PropU32(rec, 4);
    traj = Scn_GetPropTrajectory(rec, 0x14);
    unk120c = 0;
    unk120f = 0;
    unk120e = 3;
    unk1210 = block11fc;
    if (traj && traj->count) {
        TrajFollower_Init(&follower, traj, 200, 0, 0, 0, 0x32);
        aim = traj->pts[0];
    } else {
        follower.traj = 0;
    }
    locked = 0;
    count = Scenaric_FindByClass(CLASSID_ROBOT, objList, 64);
    if (count == 1)
        robot = objList[0];
    else
        robot = 0;
    authBoxes.Load((u16)PropU32(rec, 0));
}

/* 0x4f6ebe LightSpot_MsgStub: a message handler that answers nothing. It is in no vtable and nothing
 * calls it. */
s32 LightSpot::MsgStub(ScnObject *sender, u32 msgId, void *arg)
{
    return 0;
}

/* 0x4f6ecd - vtable +0x0c: LightSpot::Render, after its inline helpers, which must precede it. */
/* BYTES(temp): volatile by-value parameter: the original copies the stage to a stack slot before reading texture and this */
inline void D3DApp::Render_SetTexture(Texture *texture, volatile u32 stage)
{
    pD3DDevice->SetTexture(stage, texture->surface);
}
#define SDW_INLINE_D3DAPP_DRAWPRIMITIVEINLINE_U32_U32_VOID_U32 1
#define SDW_INLINE_D3DAPP_SETFLAGSINLINE_U32 1
#include "../app/d3dapp_inlines.h"
#undef SDW_INLINE_D3DAPP_DRAWPRIMITIVEINLINE_U32_U32_VOID_U32
#undef SDW_INLINE_D3DAPP_SETFLAGSINLINE_U32
/* BYTES(slot-scope, inferred): the nested block(s) only order the frame: their locals are allocated after the enclosing scope's */
__forceinline void PolyBatcher::SubmitBeamA(RenderPoly *poly)
{
    switch (poly->type) {
        case RPOLY_OPAQUE:
            if (flatBatchCount <= batchCapacity) {
                /* cast kept: the flat batch is untyped vertex memory, 3 FlatVertex (FVF 0xc4) per triangle */
                memcpy((FlatVertex *)flatBatchVerts + flatBatchCount * 3, poly->verts, 0x48);
                ++flatBatchCount;
            }
            if (flatBatchCount >= batchCapacity) {
                renderer->SetFlagsInline(stateFlags);
                renderer->DrawPrimitiveInline(D3DPT_TRIANGLELIST, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR,
                                              flatBatchVerts, batchCapacity * 3);
                renderer->Render_ClearStateFlags(stateFlags);
                flatBatchCount = 0;
                textureDirty = 1;
            }
            break;
        case RPOLY_BLEND:
            if (computeSortZ == 1)
                poly->sortZ = poly->verts[2] + poly->verts[8] + poly->verts[14];
            sortedPolys[sortedCount].Assign(poly);
            sortedList[sortedCount] = &sortedPolys[sortedCount];
            ++sortedCount;
            break;
        case RPOLY_ADD:
            if (computeSortZ == 1)
                poly->sortZ = poly->verts[2] + poly->verts[8] + poly->verts[14];
            sortedPolys[sortedCount].Assign(poly);
            sortedList[sortedCount] = &sortedPolys[sortedCount];
            ++sortedCount;
            break;
        default:
            /* cast kept: RenderPoly.verts is a float array; a textured poly holds FlameVertex records (FVF 0x1c4) */
            FlameVertex *vertices = (FlameVertex *)poly->verts;
            u32 texture = (poly->type - RPOLY_TEXTURED_BASE) & ~RPOLY_F_8000;
            if (texture < immediateTexCount) {
                u32 *flags;
                {
                    /* cast kept: a texture batch is untyped vertex memory of FlameVertex records */
                    FlameVertex *batch = (FlameVertex *)texBatchVerts[texture];
                    {
                        u32 *count = &texBatchCounts[texture];
                        flags = &texStateFlags[texture];
                        if (*count <= batchCapacity) {
                            memcpy(batch + *count * 3, vertices, 0x60);
                            ++*count;
                        }
                        if (*count >= batchCapacity) {
                            if (texture != lastTextureIndex || textureDirty == 1) {
                                renderer->Render_SetTexture(textures[texture], 0);
                                lastTextureIndex = texture;
                                textureDirty = 0;
                            }
                            renderer->Render_SetStateFlags(*flags);
                            renderer->DrawPrimitiveInline(
                                D3DPT_TRIANGLELIST, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1,
                                batch, batchCapacity * 3);
                            renderer->Render_ClearStateFlags(*flags);
                            *count = 0;
                        }
                    }
                }
            } else {
                if (computeSortZ == 1)
                    poly->sortZ = vertices[0].z + vertices[1].z + vertices[2].z;
                sortedPolys[sortedCount].Assign(poly);
                sortedList[sortedCount] = &sortedPolys[sortedCount];
                ++sortedCount;
            }
    }
}
/* BYTES(slot-scope, inferred): the nested block(s) only order the frame: their locals are allocated after the enclosing scope's */
__forceinline void PolyBatcher::SubmitBeamB(RenderPoly *poly)
{
    switch (poly->type) {
        case RPOLY_OPAQUE:
            if (flatBatchCount <= batchCapacity) {
                /* cast kept: the flat batch is untyped vertex memory, 3 FlatVertex (FVF 0xc4) per triangle */
                memcpy((FlatVertex *)flatBatchVerts + flatBatchCount * 3, poly->verts, 0x48);
                ++flatBatchCount;
            }
            if (flatBatchCount >= batchCapacity) {
                renderer->Render_SetStateFlags(stateFlags);
                renderer->DrawPrimitiveInline(D3DPT_TRIANGLELIST, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR,
                                              flatBatchVerts, batchCapacity * 3);
                renderer->Render_ClearStateFlags(stateFlags);
                flatBatchCount = 0;
                textureDirty = 1;
            }
            break;
        case RPOLY_BLEND:
            if (computeSortZ == 1)
                poly->sortZ = poly->verts[2] + poly->verts[8] + poly->verts[14];
            sortedPolys[sortedCount].Assign(poly);
            sortedList[sortedCount] = &sortedPolys[sortedCount];
            ++sortedCount;
            break;
        case RPOLY_ADD:
            if (computeSortZ == 1)
                poly->sortZ = poly->verts[2] + poly->verts[8] + poly->verts[14];
            sortedPolys[sortedCount].Assign(poly);
            sortedList[sortedCount] = &sortedPolys[sortedCount];
            ++sortedCount;
            break;
        default:
            /* cast kept: RenderPoly.verts is a float array; a textured poly holds FlameVertex records (FVF 0x1c4) */
            FlameVertex *vertices = (FlameVertex *)poly->verts;
            u32 texture = (poly->type - RPOLY_TEXTURED_BASE) & ~RPOLY_F_8000;
            if (texture < immediateTexCount) {
                u32 *flags;
                {
                    /* cast kept: a texture batch is untyped vertex memory of FlameVertex records */
                    FlameVertex *batch = (FlameVertex *)texBatchVerts[texture];
                    {
                        u32 *count = &texBatchCounts[texture];
                        flags = &texStateFlags[texture];
                        if (*count <= batchCapacity) {
                            memcpy(batch + *count * 3, vertices, 0x60);
                            ++*count;
                        }
                        if (*count >= batchCapacity) {
                            if (texture != lastTextureIndex || textureDirty == 1) {
                                renderer->Render_SetTexture(textures[texture], 0);
                                lastTextureIndex = texture;
                                textureDirty = 0;
                            }
                            renderer->Render_SetStateFlags(*flags);
                            renderer->DrawPrimitiveInline(
                                D3DPT_TRIANGLELIST, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1,
                                batch, batchCapacity * 3);
                            renderer->Render_ClearStateFlags(*flags);
                            *count = 0;
                        }
                    }
                }
            } else {
                if (computeSortZ == 1)
                    poly->sortZ = vertices[0].z + vertices[1].z + vertices[2].z;
                sortedPolys[sortedCount].Assign(poly);
                sortedList[sortedCount] = &sortedPolys[sortedCount];
                ++sortedCount;
            }
    }
}
/* BYTES(dead-code): unused_9 is set and never read and unusedFrameWord_9 fills the original's unreferenced four-byte gap */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py) */
void LightSpot::Render(Camera *view)
{
    /* Names reproduce the original VC6 stack slots; unusedFrameWord records
     * its unreferenced four-byte gap. These names are not recovered. */
    u8 unused_9;
    TlVertex *v2_18;
    TlVertex *v0_1;
    TlVertex *v1_19;
    u8 prev_7;
    float z_28;
    u32 lockSize_10;
    u8 nbSides_1;
    u8 i_4;
    XyzrhwVertex *tv_1;
    Vec3f *ring_1;
    Vec3s d_5;
    float d2_3;
    Mat44 rot_9;
    void *lockPtr_16;
    u8 lit_5;
    Mat44 world_26;
    u32 unusedFrameWord_9;
    u8 face_5;

    g_lightSpotXyzVb->Lock(DDLOCK_WAIT, &lockPtr_16, &lockSize_10);
    /* cast kept: the locked vertex buffer is untyped; its format (D3DFVF_XYZ) makes it Vec3f */
    ring_1 = (Vec3f *)lockPtr_16;
    d_5.x = view->pos.x - aim.x;
    d_5.y = view->pos.y - aim.y;
    d_5.z = view->pos.z - aim.z;
    d2_3 = (float)(d_5.x * d_5.x + d_5.z * d_5.z);
    if (d2_3 >= 6250000.0f)
        nbSides_1 = 3;
    else if (d2_3 <= 160000.0f)
        nbSides_1 = 20;
    else
        nbSides_1 = (u8)((d2_3 - 160000.0f) / 6090000.0f * -17.0f) + 20;
    rot_9.SetRotZXY(0.0f, 3.1415927f / nbSides_1, 0.0f);
    ring_1[0].x = (float)radius;
    ring_1[0].z = 0.0f;
    ring_1[0].y = 0.0f;
    ring_1[nbSides_1].x = (float)-radius;
    ring_1[nbSides_1].z = 0.0f;
    ring_1[nbSides_1].y = 0.0f;
    for (i_4 = 1; i_4 < nbSides_1; i_4++) {
        prev_7 = i_4 - 1;
        rot_9.TransformPoint(&ring_1[prev_7], &ring_1[i_4]);
        rot_9.TransformPoint(&ring_1[prev_7 + nbSides_1], &ring_1[i_4 + nbSides_1]);
    }
    ring_1[nbSides_1 * 2].x = (float)(pos.x - aim.x);
    ring_1[nbSides_1 * 2].y = (float)(pos.y - aim.y);
    ring_1[nbSides_1 * 2].z = (float)(pos.z - aim.z);
    g_lightSpotXyzVb->Unlock();
    world_26.SetTranslation((float)aim.x, (float)aim.y, (float)aim.z);
    g_pD3DAppMain->SetTransform(D3DTRANSFORMSTATE_WORLD, &world_26);
    g_pD3DAppMain->SetTransform(D3DTRANSFORMSTATE_VIEW, &view->viewMat);
    g_lightSpotVbXf->ProcessVertices(D3DVOP_TRANSFORM, 0, nbSides_1 * 2 + 1, g_lightSpotXyzVb, 0,
                                     g_pD3DAppMain->GetDevice(), D3DPV_DONOTCOPYDATA);
    g_lightSpotVbXf->Lock(DDLOCK_WAIT, &lockPtr_16, &lockSize_10);
    /* cast kept: the locked vertex buffer is untyped; its format (D3DFVF_XYZRHW) makes it XyzrhwVertex */
    tv_1 = (XyzrhwVertex *)lockPtr_16;
    g_pPolyBin->SetComputeSortZ(0);
    if (BoxList_FindContainingPointXZ(robot->Pos(), g_lightSpotSamBoxes, g_lightSpotSamBoxCount) &&
        robot->HandleMessage(this, MSG_ROBOT_IS_DRIVEN, 0) && !robot->HandleMessage(this, MSG_ROBOT_IS_EJECTED, 0))
        z_28 = g_screen.Draw2D_LayerToZ(g_screenLayerBase + 12) * 3.0f;
    else
        z_28 = (tv_1[0].z + tv_1[nbSides_1].z) * 1.5f;
    memcpy(g_lightSpotBeamFace.verts, &tv_1[nbSides_1 * 2], 0x10);
    lit_5 = IsPointLit(&view->pos);
    for (face_5 = 0; face_5 < nbSides_1 * 2; face_5++) {
        unused_9 = 1;
        /* cast kept (these three lines): RenderPoly.verts is a float array; the vertices are TlVertex records */
        v0_1 = (TlVertex *)g_lightSpotBeamFace.verts;
        v1_19 = (TlVertex *)g_lightSpotBeamFace.verts + 1;
        v2_18 = (TlVertex *)g_lightSpotBeamFace.verts + 2;
        memcpy(v1_19, &tv_1[face_5], 0x10);
        memcpy(v2_18, &tv_1[(face_5 + 1) % (nbSides_1 * 2)], 0x10);
        if ((v0_1->z > 1.0f) ^ (v1_19->z > 1.0f) ^ (v2_18->z > 1.0f) ^
            ((v1_19->x - v0_1->x) * (v2_18->y - v0_1->y) - (v1_19->y - v0_1->y) * (v2_18->x - v0_1->x) <= 0.0f)) {
            g_lightSpotBeamFace.type = g_lightSpotTexB.page + RPOLY_TEXTURED_BASE;
            v2_18->diffuse = g_lightSpotTexB.color;
            v1_19->diffuse = g_lightSpotTexB.color;
            v0_1->diffuse = g_lightSpotTexB.color;
            v2_18->specular = 0xff000000;
            v1_19->specular = 0xff000000;
            v0_1->specular = 0xff000000;
            v0_1->u = g_lightSpotTexB.u0;
            v0_1->v = g_lightSpotTexB.v0;
            v1_19->u = g_lightSpotTexB.u0;
            v1_19->v = g_lightSpotTexB.v1;
            v2_18->u = g_lightSpotTexB.u1;
            v2_18->v = g_lightSpotTexB.v1;
            g_lightSpotBeamFace.sortZ = z_28;
            g_pPolyBin->SubmitBeamA(&g_lightSpotBeamFace);
        } else if (!lit_5) {
            g_lightSpotBeamFace.type = g_lightSpotTexA.page + RPOLY_TEXTURED_BASE;
            v2_18->diffuse = g_lightSpotTexA.color;
            v1_19->diffuse = g_lightSpotTexA.color;
            v0_1->diffuse = g_lightSpotTexA.color;
            v2_18->specular = 0xff000000;
            v1_19->specular = 0xff000000;
            v0_1->specular = 0xff000000;
            v0_1->u = g_lightSpotTexA.u0;
            v0_1->v = g_lightSpotTexA.v0;
            v1_19->u = g_lightSpotTexA.u0;
            v1_19->v = g_lightSpotTexA.v1;
            v2_18->u = g_lightSpotTexA.u1;
            v2_18->v = g_lightSpotTexA.v1;
            g_lightSpotBeamFace.sortZ = z_28 - 0.0001f;
            g_pPolyBin->SubmitBeamB(&g_lightSpotBeamFace);
        }
    }
    if (lit_5 == 1)
        Draw2D_TexRect(g_screen.Draw2D_LayerToZ(g_screenLayerBase + 11), 0.0f, 0.0f, g_pViewFrustum->viewportWidth,
                       g_pViewFrustum->viewportHeight, g_lightSpotTexA.page, g_lightSpotTexA.u0, g_lightSpotTexA.v0,
                       g_lightSpotTexA.color, g_lightSpotTexA.u0, g_lightSpotTexA.v1, g_lightSpotTexA.color,
                       g_lightSpotTexA.u1, g_lightSpotTexA.v0, g_lightSpotTexA.color, g_lightSpotTexA.u1,
                       g_lightSpotTexA.v1, g_lightSpotTexA.color);
    g_lightSpotVbXf->Unlock();
    g_pPolyBin->SetComputeSortZ(1);
}

/* 0x4f8783 LightSpot_Stub: empty, in no vtable, no callers. */
void LightSpot::Stub() {}

/* 0x4f878e LightSpot_IsPointLit: is p inside the cone of light? The cone runs from the spot's own
 * position down to the aim point, RADIUS wide where it reaches the aim point's height. The point's height gives the
 * radius of the cone there and the fraction t of the way down the beam; the beam's centre at that height is
 * interpolated between pos and aim, and the horizontal distance to it is compared with the radius. Both faces of
 * the test are inclusive. */
/* BYTES(dead-code): aux is never used: the original allocates it at ebp-0x10 */
u8 LightSpot::IsPointLit(Vec3s *p)
{
    /* the local names are chosen for their stack slots (tools/vc6_locals.py); `aux` is a 4-byte slot the original
     * allocated at ebp-0x10 and never used */
    float dz;
    float scratch;
    float level;
    float aux;
    float spotR;
    float dx;

    spotR = (float)((pos.y - p->y) * radius) / pos.y;
    level = (float)(p->y - pos.y) / (aim.y - pos.y);
    scratch = (float)(aim.x - pos.x);
    dx = scratch * level + (pos.x - p->x);
    scratch = (float)(aim.z - pos.z);
    dz = scratch * level + (pos.z - p->z);
    if (dx * dx + dz * dz <= spotR * spotR)
        return 1;
    else
        return 0;
}
