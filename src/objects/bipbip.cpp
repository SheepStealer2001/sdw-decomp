/*
 * bipbip (class 31, CLASSID_BIPBIP), SheepD3D.exe 0x499300-0x499e22: the Road Runner of Level 0's long run. Hidden
 * until Ralph steps into BOX_DECL; then it runs the TRAJECTORY point path (SPEED_ONE for the first segment, SPEED_TWO
 * after) with a dust trail and a blob shadow, and when that path reaches its last segment it also starts the TRAJ_WALL
 * path and is drawn "into" the painted tunnel: every frame its transformed model is moved on screen onto the projected
 * wall-path point and pushed back to that point's depth. When the road path ends it resets (hidden, waiting again).
 * Designer properties (bipbipProps): BOX_DECL +0, SPEED_ONE +4, SPEED_TWO +8, TRAJ_WALL +0xc, TRAJECTORY +0x10.
 * The PathFollower struct (0x28 bytes, 0x546c8e-0x54705e) is in data/structs.
 *
 * The inline helpers have no bodies of their own in the exe; their names are descriptive, and each is there because its
 * expansion gives the original's shapes: SetVisible / SetBoxCollide a constant tested as `xor r,r; test`,
 * SetUpdateMode a 4-way jump table on a constant (ScnUpdateMode; table 0x49970f), SetPartHeight an unfolded shift of a
 * constant, SetFacing / Facing a 2-byte temp, Box_ContainsPoint its two arguments and its value in temps (0x499760),
 * PropU32 its offset argument in a temp (0x499312), D3DApp::CreateVB (the DirectX SDK samples' "system memory unless
 * TnL HAL" vertex buffer idiom, also in Sfx_CreateVertexBuffers 0x52a89f) its GUID local above its `this` temp,
 * D3DApp::SetTransform its `this` temp (0x499b63), D3DApp::GetDevice its value in a temp (0x499bad). Local names are
 * chosen for their stack slots (tools/vc6_locals.py).
 */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "../sdk/d3d7.h"
#include "../sdk/ddraw.h"
#include "../sdk/crt.h"
struct D3DXyzrhwVertex { /* a D3DFVF_XYZRHW vertex, as ProcessVertices writes it */
    float x, y, z, rhw;
};
class Mat44;
#define IsEqualGUID(a, b) (!memcmp((a), (b), 16)) /* sizeof(GUID); the type is incomplete here */

class Instance;
struct Animator;
u32 Anim_Start(Instance *inst, Animator *animator, u16 id, u32 opts); /* 0x550196 (used by the PlayAnim inline) */


#define SDW_MEMBERS_ScnObject            \
    static void *operator new(u32 size); \
    void SetUpdateMode(u8 mode);         \
    void SetFacing(s16 f);               \
    /* inline: the mask in a register (0x499a68) */
#define SDW_MEMBERS_TrailEmitter TrailEmitter(); /* inline: the pools are the inline buffers */
#define SDW_MEMBERS_Mat44 Mat44();               /* 0x4077f0 Mat44_Ctor, empty and out of line */
#define SDW_MEMBERS_D3DApp                                                  \
    void CreateVB(D3DVERTEXBUFFERDESC *desc, IDirect3DVertexBuffer7 **out); \
    void SetTransform(u32 state, Mat44 *m);                                 \
    IDirect3DDevice7 *GetDevice(); /* inline */
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_SETVISIBLE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETVISIBLE_S32
#define SDW_INLINE_SCNOBJECT_SETBOXCOLLIDE_S32 1
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETBOXCOLLIDE_S32
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8
#define SDW_INLINE_SCNOBJECT_SETPARTHEIGHT_U32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETPARTHEIGHT_U32
#define SDW_INLINE_SCNOBJECT_FACING 1
#define SDW_INLINE_SCNOBJECT_SETFACING_S16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_FACING
#undef SDW_INLINE_SCNOBJECT_SETFACING_S16
#define SDW_INLINE_SCNOBJECT_INSTFLAGS_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_INSTFLAGS_U16
#define SDW_INLINE_D3DAPP_CREATEVB_D3DVERTEXBUFFERDESC_IDIRECT3DVERTEXBUFFER7 1
#define SDW_INLINE_D3DAPP_SETTRANSFORM_U32_MAT44 1
#include "../app/d3dapp_inlines.h"
#undef SDW_INLINE_D3DAPP_CREATEVB_D3DVERTEXBUFFERDESC_IDIRECT3DVERTEXBUFFER7
#undef SDW_INLINE_D3DAPP_SETTRANSFORM_U32_MAT44
#define SDW_INLINE_D3DAPP_GETDEVICE 1
#include "../app/d3dapp_inlines.h"
#undef SDW_INLINE_D3DAPP_GETDEVICE
inline TrailEmitter::TrailEmitter()
{
    base.slotPool = slotBuf;
    base.particles = particleBuf;
    base.count = 16;
    base.Emitter_Reset();
}

#include "../engine/id_list.h"
#include "animation.h"
#include "../engine/tex_table.h"
#include "../engine/shadow.h"
#include "../engine/draw2d.h"
extern Wolf *g_pWolf; /* 0x6cf310 */
/* This file's statics (.bss): the two 2-vertex buffers of the tunnel projection, and where the bipbip stood when
 * Ralph started the run (written only; nothing reads it). */
/* Recovered T106 .bss owner, 0x6cf450..0x6cf460. Aggregate spelling is not recovered. */
struct BipbipRenderStorage {
    IDirect3DVertexBuffer7 *transformed; /* +0x00 D3DFVF_XYZRHW */
    IDirect3DVertexBuffer7 *source;      /* +0x04 D3DFVF_XYZ */
    Vec3s runStartPos;                   /* +0x08 */
    u8 pad00e[2];
};
BipbipRenderStorage g_bipbipRenderStorage;
#define g_bipbipVbXf (g_bipbipRenderStorage.transformed)
#define g_bipbipVb (g_bipbipRenderStorage.source)
#define g_bipbipRunStartPos (g_bipbipRenderStorage.runStartPos)

/* A designer property: the dword at record + 0x14 + off (bipbipProps). Inline: the offset is a stack temp. */
#define SDW_INLINE_FREE_PROPU32_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_PROPU32_VOID_U32

/* Whether p lies inside box (both faces inclusive): its two parameters are stack temps. */
#define SDW_INLINE_FREE_BOX_CONTAINSPOINT_BOX_VEC3S 1
#include "../engine/coll_box_inlines.h"
#undef SDW_INLINE_FREE_BOX_CONTAINSPOINT_BOX_VEC3S

/* 0x499300 - vtable +0x00: the properties (startBox and the two paths only if all three resolve), Reset, no box
 * collision, shadow radius 20, the dust parameters, and the two vertex buffers of the tunnel projection. */
void bipbip::PostLoadInit()
{
    u32 *wallList;
    u16 num;
    u32 *trajList;
    u16 *rec;
    u32 *boxIds;
    D3DVERTEXBUFFERDESC desc;
    rec = record;
    boxIds = Scn_FindIdList(PropU32(rec, 0), &num);
    trajList = Scn_FindIdList(PropU32(rec, 0x10), &num);
    wallList = Scn_FindIdList(PropU32(rec, 0xc), &num);
    speedOne = (s16)PropU32(rec, 4);
    speedTwo = (s16)PropU32(rec, 8);
    if (boxIds && trajList && wallList) {
        /* cast kept (these three): an export id list holds record pointers as u32 words; a box, then
         * two trajectories */
        startBox = (Box *)*boxIds;
        roadPath.path = (Trajectory *)*trajList;
        /* cast kept: an export id list holds its records' pointers as u32 words */
        wallPath.path = (Trajectory *)*wallList;
    }
    Reset();
    SetBoxCollide(0);
    shadow.radius = 20;
    dustParams.hSpeed = 100;
    dustParams.vSpeed = -20;
    dustParams.life = 0xa30;
    dustParams.spawnInterval = 0xa3;
    dustParams.sizeStart = 40;
    dustParams.sizeEnd = 120;
    dustParams.sheetIndex = 0;
    memset(&desc, 0, sizeof(desc));
    desc.dwSize = sizeof(desc);
    desc.dwCaps = D3DVBCAPS_DONOTCLIP;
    desc.dwFVF = D3DFVF_XYZ;
    desc.dwNumVertices = 2;
    g_pD3DAppMain->CreateVB(&desc, &g_bipbipVb);
    memset(&desc, 0, sizeof(desc));
    desc.dwSize = sizeof(desc);
    desc.dwCaps = D3DVBCAPS_DONOTCLIP;
    desc.dwFVF = D3DFVF_XYZRHW;
    desc.dwNumVertices = 2;
    g_pD3DAppMain->CreateVB(&desc, &g_bipbipVbXf);
}

/* 0x4995c2 - vtable +0x14: waiting again: dust cleared, shadow on, hidden but always updated, back at the start of the
 * road path (at SPEED_ONE). */
void bipbip::Reset()
{
    dust.base.Emitter_Reset();
    shadowOn = 1;
    state = BIPBIP_ST_WAIT;
    SetVisible(0);
    SetUpdateMode(SCN_UPD_ALWAYS);
    roadPath.Start(speedOne);
    SetPartHeight(0x100);
    SetPosition(&roadPath.pos);
}

/* 0x49971f - vtable +0x04: wait for Ralph in startBox, run the road path (SPEED_TWO from the second segment; at the
 * last one the wall path starts), then follow both into the tunnel until the road path ends and Reset. */
void bipbip::Update()
{
    u8 roadEnd;
    switch (state) {
        case BIPBIP_ST_WAIT:
            inTunnel = 0;
            if (Box_ContainsPoint(startBox, &g_pWolf->pos)) {
                SetVisible(1);
                g_bipbipRunStartPos.x = pos.x;
                g_bipbipRunStartPos.y = pos.y;
                g_bipbipRunStartPos.z = pos.z;
                state = BIPBIP_ST_RUN_ROAD;
                dust.base.Emitter_Reset();
            }
            break;
        case BIPBIP_ST_RUN_ROAD:
            if (roadPath.Step() == PATHSTEP_NEXT_SEGMENT) {
                roadPath.speed = speedTwo;
                if (roadPath.toIdx >= roadPath.path->count - 1) {
                    shadowOn = 0;
                    state = BIPBIP_ST_RUN_TUNNEL;
                    SetPartHeight(0);
                    wallPath.Start(speedTwo);
                    wallPath.rate = roadPath.rate;
                }
            }
            SetFacing(roadPath.heading);
            SetPosition(&roadPath.pos);
            dust.base.Emitter_UpdateDrift(&dustParams, &pos, Facing(), 1);
            break;
        case BIPBIP_ST_RUN_TUNNEL:
            inTunnel = 1;
            roadEnd = roadPath.Step();
            wallPath.Step();
            SetFacing(roadPath.heading);
            SetPosition(&roadPath.pos);
            dust.base.Emitter_UpdateDrift(&dustParams, &pos, Facing(), 0);
            if (roadEnd == PATHSTEP_END)
                Reset();
            break;
    }
    AdvanceAnim();
}

/* 0x499a18 - vtable +0x08: the dust, the animated model, and in the tunnel the projection trick: transform the
 * wall-path point and the bipbip's position, move every transformed vertex of the model by the screen offset between
 * them and set its depth so the farthest vertex lies at the wall point's; then submit the mesh, and the shadow. */
void bipbip::Render(Camera *view)
{
    Mesh *mesh;
    if (dust.base.flags.active)
        dust.base.Emitter_Render(view, 0);
    Instance_DrawAnimParts(Inst(), &anim, view, 0);
    if (!InstFlags(INST_F_DRAWN))
        return;
    /* cast kept: resource data is untyped; this model's is its Mesh */
    mesh = (Mesh *)Texture_FindByResource(inst_model);
    if (inTunnel) {
        float maxZ;
        u32 bytes;
        u32 n;
        Vec3f screenOff;
        Mat44 worldMat;
        float *verts;
        /* cast kept (the three Locks): Lock hands the buffer back through a void ** out parameter (the
         * SDK's signature) */
        g_bipbipVb->Lock(DDLOCK_WAIT, (void **)&verts, &bytes);
        verts[0] = (float)wallPath.pos.x;
        verts[1] = (float)wallPath.pos.y;
        verts[2] = (float)wallPath.pos.z;
        verts[3] = (float)pos.x;
        verts[4] = (float)pos.y;
        verts[5] = (float)pos.z;
        g_bipbipVb->Unlock();
        worldMat.SetIdentity();
        g_pD3DAppMain->SetTransform(D3DTRANSFORMSTATE_WORLD, &worldMat);
        g_pD3DAppMain->SetTransform(D3DTRANSFORMSTATE_VIEW, &view->viewMat);
        g_bipbipVbXf->ProcessVertices(D3DVOP_TRANSFORM, 0, 2, g_bipbipVb, 0, g_pD3DAppMain->GetDevice(),
                                      D3DPV_DONOTCOPYDATA);
        /* cast kept: COM returns interfaces through a void ** */
        g_bipbipVbXf->Lock(DDLOCK_WAIT, (void **)&verts, &bytes);
        screenOff.x = verts[0] - verts[4];
        screenOff.y = verts[1] - verts[5];
        screenOff.z = verts[2];
        g_bipbipVbXf->Unlock();
        maxZ = 0;
        mesh->vbTransformed->Lock(DDLOCK_WAIT, (void **)&verts, &bytes);
        /* cast kept (every (D3DXyzrhwVertex *) below): the transformed buffer is untyped memory holding
         * XYZRHW vertices */
        for (n = 0; n < mesh->vertexCount; n++) {
            ((D3DXyzrhwVertex *)verts)[n].x = screenOff.x + ((D3DXyzrhwVertex *)verts)[n].x;
            /* cast kept: a locked vertex buffer is untyped; its vertex format gives this type */
            ((D3DXyzrhwVertex *)verts)[n].y = screenOff.y + ((D3DXyzrhwVertex *)verts)[n].y;
            if (((D3DXyzrhwVertex *)verts)[n].z > maxZ)
                maxZ = ((D3DXyzrhwVertex *)verts)[n].z;
        }
        screenOff.z -= maxZ;
        for (n = 0; n < mesh->vertexCount; n++)
            /* cast kept: a locked vertex buffer is untyped; its vertex format gives this type */
            ((D3DXyzrhwVertex *)verts)[n].z = screenOff.z + ((D3DXyzrhwVertex *)verts)[n].z;
        mesh->vbTransformed->Unlock();
    }
    mesh->Mesh_DrawOutlined(g_pPolyBin, g_pViewFrustum, -1.0f);
    if (shadowOn) {
        UpdateShadow();
        Shadow_Render(&shadow);
    }
}

/* 0x499d71 - vtable +0x10: no messages; always 0. */
s32 bipbip::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    return 0;
}

/* 0x499d80 - the class factory for CLASSID 31 "bipbip": new bipbip (the base vtables, the dust trail's inline
 * constructor, then bipbip's vtable), then ScnMobile::Init(record, 0) through the vtable. */
ScnObject *bipbip_Create(void *record)
{
    ScnBody *obj = new bipbip;
    obj = obj->Init(record, 0);
    return obj;
}
