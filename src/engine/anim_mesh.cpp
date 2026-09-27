/*
 * Object T012 (data/tu_map.json), guessed original file AnimMesh.cpp.
 *   .text 0x40b630-0x40bfe2, then the COMDATs ??_GAnimMesh 0x40bff0, ??_EMeshPart 0x40c020, the vector constructor
 *   iterator ??_H 0x40c0d0 and ??_EMeshAnimSeq 0x40c100 (the ??_I this object also emits is unreferenced: /OPT:REF)
 *   .rdata 0x574358-0x574370 (??_7AnimMesh)
 * MeshAnimSeq's constructor and destructor are T011 (src/engine/mesh_anim_seq.cpp) and Mesh is T013 (src/engine/mesh.cpp).
 */
/* BYTES: flow, inline, layout, slot-name. */
/* BYTES(inline): D3DApp::SetTransform / GetDevice / CreateVBWithResult (inline): source-only inline: its expansion gives the original's stack temporaries */
/* BYTES(inline): Vec3f() {} (member macro): empty user-declared constructor: it gives the original's empty loop in new Vec3f[n] */
/* BYTES(layout): keep AnimMesh::BuildFromBsFile first: its two new[] must take $S1 / $S2 so Mesh::BuildFromBsFile's count temp is $S3 */
/*
 * The mesh layer 0x40b600-0x40d238: MeshAnimSeq and AnimMesh (0x40b600-0x40bfdf, then their compiler-generated COMDATs
 * 0x40bff0-0x40c19f), and Mesh up to Mesh_TransformRange (0x40c1b0-0x40d238); this object is the AnimMesh part.
 *
 * match-addr: ?BuildFromBsFile@AnimMesh@@UAEEPAVD3DApp@@PAVBsFile@@@Z=0x40b6e1 ?TransformAll@AnimMesh@@UAEXXZ=0x40b865
 * match-addr: ?DrawAll@AnimMesh@@UAEXPAUMat44@@00@Z=0x40b870 ??_EMeshAnimSeq@@UAEPAXI@Z=0x40c100
 * match-addr: ??_EMeshPart@@UAEPAXI@Z=0x40c020 ??_H@YGXPAXIHP6EX0@Z@Z=0x40c0d0
 *
 * A Mesh is one Black Sheep geometry frame turned into two D3D7 vertex buffers (vbPositions: D3DFVF_XYZ, vbTransformed:
 * D3DFVF_XYZRHW, the ProcessVertices destination) plus a RenderPoly per face. Mesh::BuildFromBsFile reads the frame's
 * vertices and its six face kinds (flat, Gouraud, textured flat / Gouraud, blended flat / Gouraud; a quad entry counts
 * as two triangles) into temporary BsPoly arrays and converts every one into a RenderPoly.
 * An AnimMesh adds a part hierarchy (MeshPart, one world matrix each) and animation sequences (MeshAnimSeq, frames of
 * per-part poses); AnimMesh::DrawAll steps the animation on a private wall-clock Timer and transforms each part's
 * vertex range with its own matrix. AnimMesh derives from Mesh (AnimMesh_Construct 0x40b63c calls Mesh_Construct before
 * storing its vtable); tools/structs_to_c.py declares the Mesh / PolyTri base classes.
 *
 * The match-addr pins place the three AnimMesh overrides (their table names AnimMesh_Load / _TransformAll_Stub / _Draw
 * are not the slot names BuildFromBsFile / TransformAll / DrawAll they must have in C++) and the compiler-generated
 * functions whose table names are not VC6's (MeshAnimSeq_VecDeletingDtor, VecDeletingDtor_41a209, Cpp_VectorCtorIterator).
 * The object also carries Mesh's scalar deleting destructor (0x4150f0) and, unplaced, the vector deleting destructors of
 * the BsPoly classes and RenderPoly and the vector destructor iterator: COMDATs the original linker took from the end of
 * the Mesh file (0x415120-0x415540) or dropped.
 *
 * Shapes (a shape that reproduces the bytes is a representation, not proof that the original source read this way):
 *  - The inline helpers D3DApp::SetTransform / GetDevice / CreateVBWithResult have no bodies in the exe; their expansions give the
 *    original's stack temporaries, as in emitter.cpp, elastic.cpp and holefx.cpp.
 *  - The `new T[n]` loops are VC6's own /Ob1 expansion of the vector constructor iterator.
 *  - AnimMesh::BuildFromBsFile, GetFlag and Mesh::BuildFromBsFile return through a u8 local (the original stores it in
 *    every exit branch and loads it once), so they are written single-exit with nested if / else. In
 *    Mesh::BuildFromBsFile the locals other than the result and the vertex scratch are declared in the first else
 *    block: that is what puts VC6's count temporary for new Vec3f[] ($S3) among them at EBP-0x20.
 *  - Local names are chosen for their stack slots (tools/vc6_locals.py). $S3's number (hence its hash bucket) assumes
 *    the two new[] of AnimMesh::BuildFromBsFile ($S1, $S2) come first in this file.
 */
#include "sdw_types.h"

class Mat44;
#include "sdw_enums.h"
#include "../sdk/d3d7.h"
#include "../sdk/crt.h"

#include "timer.h"
#define SDW_MEMBERS_Mat44 Mat44();
#define SDW_MEMBERS_Vec3f \
    Vec3f() {} /* empty but user-declared: gives the original's empty loop in new Vec3f[n] */
#define SDW_MEMBERS_D3DApp                  \
    void SetTransform(u32 state, Mat44 *m); \
    IDirect3DDevice7 *GetDevice();          \
    long CreateVBWithResult(D3DVERTEXBUFFERDESC *desc, IDirect3DVertexBuffer7 **out);
#define SDW_MEMBERS_MeshPart MeshPart(); /* 0x41a070 */
#define SDW_MEMBERS_MeshAnimSeq MeshAnimSeq();
#define SDW_MEMBERS_BsPolyFlat BsPolyFlat();
#define SDW_MEMBERS_BsPolyGouraud BsPolyGouraud();
#define SDW_MEMBERS_BsPolyTexFlat BsPolyTexFlat();
#define SDW_MEMBERS_BsPolyTexGouraud BsPolyTexGouraud();
#define SDW_MEMBERS_BsPolyBlendFlat BsPolyBlendFlat();
#define SDW_MEMBERS_BsPolyBlendGouraud BsPolyBlendGouraud();
#define SDW_MEMBERS_RenderPoly RenderPoly(); /* 0x41aad0 */
#define SDW_MEMBERS_Mesh Mesh();
#define SDW_MEMBERS_AnimMesh AnimMesh();
#include "sdw_classes.h"
#define SDW_INLINE_D3DAPP_SETTRANSFORM_U32_MAT44 1
#include "../app/d3dapp_inlines.h"
#undef SDW_INLINE_D3DAPP_SETTRANSFORM_U32_MAT44
#define SDW_INLINE_D3DAPP_GETDEVICE 1
#include "../app/d3dapp_inlines.h"
#undef SDW_INLINE_D3DAPP_GETDEVICE
#define SDW_INLINE_D3DAPP_CREATEVBWITHRESULT_D3DVERTEXBUFFERDESC_IDIRECT3DVERTEXBUFFER7 1
#include "../app/d3dapp_inlines.h"
#undef SDW_INLINE_D3DAPP_CREATEVBWITHRESULT_D3DVERTEXBUFFERDESC_IDIRECT3DVERTEXBUFFER7

/* Bs_IsGeometryResource 0x41ba1d is a BsFile member (declared above). */

/* ---------------------------------------------------------------- AnimMesh */

/* 0x40b630: a private Timer drives the animation in wall-clock milliseconds (stopped until the first update). */
AnimMesh::AnimMesh()
{
    animTimer = new Timer;
    animTimer->Stop();
    requestedSeq = -1;
    currentSeq = -1;
    frameIndex = 0;
    elapsedMs = 0;
    frameDurationMs = 1.0f;
    playing = 0;
}

/* 0x40b6c5 */
AnimMesh::~AnimMesh() {}

/* 0x40b6e1: vtable slot 1. Accepts only a type-4 (animated) frame of an opened file: reads the part table and the
 * animation names, then builds the geometry like any Mesh. */
/* BYTES(flow): single exit through a u8 result local: the original stores it in every branch and loads it once */
u8 AnimMesh::BuildFromBsFile(D3DApp *app, BsFile *file)
{
    u8 result;

    if (file->frameType != WAR_RES_MODEL || !file->ok) {
        result = 0;
    } else {
        partCount = file->Type4_GetField18();
        parts = new MeshPart[partCount];
        file->Type4_ReadTable14(parts);
        seqCount = file->Type4_GetTableCount();
        sequences = new MeshAnimSeq[seqCount];
        file->ReadAnimNames(sequences, partCount);
        result = Mesh::BuildFromBsFile(app, file);
    }
    return result;
}

/* 0x40b865: vtable slot 3. Empty: each part needs its own world matrix, so the whole-mesh transform does not apply. */
void AnimMesh::TransformAll() {}

/* 0x40b870: vtable slot 5. Steps the animation, then builds the part matrices root first (a child's parent always has a
 * lower index) and transforms each part's vertex range under its own world matrix. */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py): t -4, part -8, idMat -0x48, firstVertex -0x4c, parentIdx -0x50 */
void AnimMesh::DrawAll(Mat44 *world, Mat44 *view, Mat44 *proj)
{
    /* local names chosen for their stack slots (tools/vc6_locals.py): t -4, part -8, idMat -0x48, firstVertex -0x4c,
     * parentIdx -0x50 */
    u32 firstVertex = 0;
    Mat44 idMat;
    float t;
    u32 part;
    u32 parentIdx;

    if (playing == 1 || currentSeq != requestedSeq)
        UpdateAnim();
    t = elapsedMs / frameDurationMs;
    app->SetTransform(D3DTRANSFORMSTATE_VIEW, view);
    app->SetTransform(D3DTRANSFORMSTATE_PROJECTION, proj);
    idMat.SetIdentity();
    parts->BuildMatrices(world, &idMat, t);
    app->SetTransform(D3DTRANSFORMSTATE_WORLD, parts->matrix);
    vbTransformed->ProcessVertices(1, firstVertex, parts->vertexCount, vbPositions, firstVertex, app->GetDevice(), 0);
    firstVertex += parts->vertexCount;
    for (part = 1; part < partCount; part++) {
        parentIdx = parts[part].parentIndex;
        parts[part].BuildMatrices(parts[parentIdx].localMatrix, parts[parentIdx].scaleMatrix, t);
        app->SetTransform(D3DTRANSFORMSTATE_WORLD, parts[part].matrix);
        vbTransformed->ProcessVertices(1, firstVertex, parts[part].vertexCount, vbPositions, firstVertex,
                                       app->GetDevice(), 0);
        firstVertex += parts[part].vertexCount;
    }
}

/* 0x40ba9b: no callers in the binary. */
void AnimMesh::SetSequence(s32 seq, u8 loop, u8 blend)
{
    requestedSeq = seq;
    this->loop = loop;
    this->blend = blend;
}

/* 0x40bac3: empty, no callers. */
void AnimMesh::Stub_40bac3() {}

/* 0x40bace: empty, no callers. */
void AnimMesh::Stub_40bace() {}

/* 0x40bad9: 0 -> the loop flag, 1 -> the blend flag. No callers. */
/* BYTES(flow): single exit through a u8 result local, as the original */
u8 AnimMesh::GetFlag(char which)
{
    u8 result;

    switch (which) {
        case 0:
            result = loop;
            break;
        case 1:
            result = blend;
            break;
        default:
            result = 0;
    }
    return result;
}

/* 0x40bb19: the animation state machine (see the AnimMesh.csv field notes). */
void AnimMesh::UpdateAnim()
{
    float delta;

    if (!playing) {
        currentSeq = requestedSeq;
        frameIndex = 0;
        ApplyFrame(&sequences[currentSeq].frames[frameIndex], 0.0f);
        frameDurationMs = sequences[currentSeq].frames[frameIndex].halfDurationMs * 2.0f;
        frameIndex++;
        elapsedMs = 0;
        playing = 1;
        animTimer->Start();
    } else {
        delta = animTimer->GetDelta(TIMER_MILLISECONDS);
        elapsedMs = delta + elapsedMs;
        if (currentSeq == requestedSeq) {
            if (elapsedMs >= frameDurationMs) {
                elapsedMs -= frameDurationMs;
                if (frameIndex >= sequences[currentSeq].frameCount) {
                    frameIndex = 0;
                    playing = loop;
                }
                ApplyFrame(&sequences[currentSeq].frames[frameIndex], 1.0f);
                frameDurationMs = sequences[currentSeq].frames[frameIndex].halfDurationMs * 2.0f;
                frameIndex++;
            }
        } else if (elapsedMs >= frameDurationMs) {
            elapsedMs -= frameDurationMs;
            currentSeq = requestedSeq;
            frameIndex = 0;
            if (blend == 1) {
                ApplyFrame(&sequences[currentSeq].frames[frameIndex], 1.0f);
            } else {
                CapturePose(&sequences[currentSeq].frames[frameIndex++]);
                SetBlendTarget(&sequences[currentSeq].frames[frameIndex]);
            }
            frameDurationMs = sequences[currentSeq].frames[frameIndex].halfDurationMs * 2.0f;
            frameIndex++;
        } else {
            currentSeq = requestedSeq;
            frameIndex = 0;
            if (blend == 1) {
                ApplyFrame(&sequences[currentSeq].frames[frameIndex], elapsedMs / frameDurationMs);
            } else {
                CapturePose(&sequences[currentSeq].frames[frameIndex++]);
                SetBlendTarget(&sequences[currentSeq].frames[frameIndex]);
            }
            frameDurationMs = sequences[currentSeq].frames[frameIndex].halfDurationMs * 2.0f;
            frameIndex++;
            elapsedMs = 0;
        }
    }
}

/* 0x40bee2 */
void AnimMesh::ApplyFrame(MeshAnimFrame *frame, float weight)
{
    MeshPartPose *poses = frame->poses;
    u32 k;

    for (k = 0; k < partCount; k++)
        parts[k].BlendThenSetTarget(&poses[k], weight);
}

/* 0x40bf3a */
void AnimMesh::CapturePose(MeshAnimFrame *frame)
{
    MeshPartPose *poses = frame->poses;
    u32 k;

    for (k = 0; k < partCount; k++)
        parts[k].SetPose(&poses[k]);
}

/* 0x40bf8e */
void AnimMesh::SetBlendTarget(MeshAnimFrame *frame)
{
    MeshPartPose *poses = frame->poses;
    u32 k;

    for (k = 0; k < partCount; k++)
        parts[k].SetTargetPose(&poses[k]);
}

/* ---------------------------------------------------------------- Mesh */
