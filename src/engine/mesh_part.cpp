/*
 * T016 - original object MeshPart.cpp (guessed name): SheepD3D.exe .text 0x41a070-0x41a822, .rdata 0x5743c4-0x5743d4
 * (MeshPart's vtable, then -pi, 2pi and pi). No COMDAT of its own survives: its ??_G is dropped, the vtable slot
 * resolving to the ??_E 0x40c020 defined where AnimMesh does new[] / delete[].
 *
 *   MeshPart: one joint of an animated mesh (AnimMesh.parts, 0x70 bytes): pose blending and the part matrices, with
 *   the two angle wrappers.
 *
 * Local names are chosen for their stack slots (tools/vc6_locals.py). A shape that reproduces the bytes is a
 * representation, not proof that the original source read this way.
 */
/* BYTES: dead-code, slot-name, view. */
#include "sdw_types.h"

class Mat44;

#define SDW_MEMBERS_Mat44 Mat44();       /* 0x4077f0 */
#define SDW_MEMBERS_MeshPart MeshPart(); /* 0x41a070 */
#include "sdw_classes.h"

/* BYTES(view): Mat44_Mul returns by value in the original: the explicit out pointer and the constructor-less MatrixReturnStorage temporary reproduce its hidden return slot */
#include "mat44.h"
/* Preserve the compiler return temporary without constructing a Mat44. */
struct MatrixReturnStorage {
    float m[4][4];
    MatrixReturnStorage() {}
};

/* ======================================================================== MeshPart */

/* 0x41a070 */
MeshPart::MeshPart()
{
    matrix = new Mat44;
    localMatrix = new Mat44;
    scaleMatrix = new Mat44;
    parentIndex = 0xffff;
    field_08 = 0;
    vertexCount = 0;
    offset[0] = offset[1] = offset[2] = 0.0f;
    rot[0] = rot[1] = rot[2] = 0.0f;
    targetRot[0] = targetRot[1] = targetRot[2] = 0.0f;
    pos[0] = pos[1] = pos[2] = 0.0f;
    targetPos[0] = targetPos[1] = targetPos[2] = 0.0f;
    scale[0] = scale[1] = scale[2] = 1.0f;
    targetScale[0] = targetScale[1] = targetScale[2] = 1.0f;
}

/* 0x41a209 - frees only the part matrix (the other two leak; never reached in the shipped build). */
MeshPart::~MeshPart()
{
    delete matrix;
}

/* 0x41a234 - moves the current pose to where the running blend has got to (angles by the short way round), then makes
 * `pose` the new target. */
void MeshPart::BlendThenSetTarget(MeshPartPose *pose, float t)
{
    rot[0] = WrapAngle2Pi(WrapAnglePi(targetRot[0] - rot[0]) * t + rot[0]);
    rot[1] = WrapAngle2Pi(WrapAnglePi(targetRot[1] - rot[1]) * t + rot[1]);
    rot[2] = WrapAngle2Pi(WrapAnglePi(targetRot[2] - rot[2]) * t + rot[2]);
    pos[0] = (targetPos[0] - pos[0]) * t + pos[0];
    pos[1] = (targetPos[1] - pos[1]) * t + pos[1];
    pos[2] = (targetPos[2] - pos[2]) * t + pos[2];
    scale[0] = (targetScale[0] - scale[0]) * t + scale[0];
    scale[1] = (targetScale[1] - scale[1]) * t + scale[1];
    scale[2] = (targetScale[2] - scale[2]) * t + scale[2];
    targetRot[0] = pose->rot[0];
    targetRot[1] = pose->rot[1];
    targetRot[2] = pose->rot[2];
    targetPos[0] = pose->pos[0];
    targetPos[1] = pose->pos[1];
    targetPos[2] = pose->pos[2];
    targetScale[0] = pose->scale[0];
    targetScale[1] = pose->scale[1];
    targetScale[2] = pose->scale[2];
}

/* 0x41a3e8 */
void MeshPart::SetPose(MeshPartPose *pose)
{
    rot[0] = pose->rot[0];
    rot[1] = pose->rot[1];
    rot[2] = pose->rot[2];
    pos[0] = pose->pos[0];
    pos[1] = pose->pos[1];
    pos[2] = pose->pos[2];
    scale[0] = pose->scale[0];
    scale[1] = pose->scale[1];
    scale[2] = pose->scale[2];
}

/* 0x41a461 */
void MeshPart::SetTargetPose(MeshPartPose *pose)
{
    targetRot[0] = pose->rot[0];
    targetRot[1] = pose->rot[1];
    targetRot[2] = pose->rot[2];
    targetPos[0] = pose->pos[0];
    targetPos[1] = pose->pos[1];
    targetPos[2] = pose->pos[2];
    targetScale[0] = pose->scale[0];
    targetScale[1] = pose->scale[1];
    targetScale[2] = pose->scale[2];
}

/* 0x41a4da - the blended pose at t (not stored): localMatrix = rotation * translation * parentMat, the offset scaled
 * by the parent's scale diagonal; matrix = scaleMatrix * localMatrix. */
/* BYTES(dead-code, inferred): unusedMat is never used; it occupies a 64-byte slot the original frame has */
/* BYTES(slot-name, inferred): pivot_x is spelled differently from pivotY / pivotZ to move its stack slot (tools/vc6_locals.py) */
/* BYTES(view): constructor-less temporaries standing in for Mat44_Mul's by-value return (see MatrixReturnStorage) */
void MeshPart::BuildMatrices(Mat44 *parentMat, Mat44 *parentScale, float t)
{
    Mat44 rotMat;
    Mat44 matTrans;
    Mat44 unusedMat;
    float pivot_x;
    float pivotY;
    float pivotZ;
    pivot_x = offset[0] * parentScale->m[0][0];
    pivotY = offset[1] * parentScale->m[1][1];
    pivotZ = offset[2] * parentScale->m[2][2];
    rotMat.SetRotYXZ(-WrapAngle2Pi(WrapAnglePi(targetRot[0] - rot[0]) * t + rot[0]),
                     WrapAngle2Pi(WrapAnglePi(targetRot[1] - rot[1]) * t + rot[1]),
                     -WrapAngle2Pi(WrapAnglePi(targetRot[2] - rot[2]) * t + rot[2]));
    matTrans.SetTranslation((targetPos[0] - pos[0]) * t + pos[0] + pivot_x,
                            (targetPos[1] - pos[1]) * t + pos[1] + pivotY,
                            (targetPos[2] - pos[2]) * t + pos[2] + pivotZ);
    scaleMatrix->SetScale((targetScale[0] - scale[0]) * t + scale[0], (targetScale[1] - scale[1]) * t + scale[1],
                          (targetScale[2] - scale[2]) * t + scale[2]);
    /* cast kept: MatrixReturnStorage is the byte device that gives Mat44_Mul the original's return slot (BYTES) */
    *localMatrix = *Mat44_Mul((Mat44 *)&MatrixReturnStorage(),
                              Mat44_Mul((Mat44 *)&MatrixReturnStorage(), &rotMat, &matTrans), parentMat);
    /* cast kept: the same return-slot device */
    *matrix = *Mat44_Mul((Mat44 *)&MatrixReturnStorage(), scaleMatrix, localMatrix);
}

/* 0x41a77e - into (-pi, pi] */
float MeshPart::WrapAnglePi(float a)
{
    float r = a;
    if (r > 3.1415927f)
        r -= 6.2831855f;
    if (r < -3.1415927f)
        r += 6.2831855f;
    return r;
}

/* 0x41a7ce - into [0, 2pi) (a negative angle only gets one turn added) */
float MeshPart::WrapAngle2Pi(float a)
{
    float r = a;
    if (r < 0.0f)
        r += 6.2831855f;
    else
        while (r >= 6.2831855f)
            r -= 6.2831855f;
    return r;
}
