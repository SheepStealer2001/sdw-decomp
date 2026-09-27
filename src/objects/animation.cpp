/* T292, guessed original file Animation.cpp.
 * match-init: StaticInit_g_animPartMatrices
 *   .text 0x54fe50-0x55244c (the static initialiser of g_animPartMatrices first, .CRT$XCU 0x5790b8)
 *   .data 0x57e764-0x57e7a0 (g_strUnknown, g_fmtAnimNotFound; then padding to the 8-aligned .data of the next object)
 *   .bss  0x6e38d0-0x6e4350 (g_animPartMatrices exactly)
 * The animation helpers, Anim_Start 0x550196, Anim_GetDurationMs 0x5503d8, Instance_CalcWorldMatrix 0x5509b1,
 * Instance_DrawAnimParts 0x550e98 and the packed key readers Anim_DecodeFirstKey 0x551845 / Anim_DecodeKey 0x551dc3,
 * in address order. The two strings are defined here. The float constants are __real COMDATs owned by earlier objects.
 *
 * Notes on the source:
 * - Anim_Start / Anim_GetDurationMs: the helper names and work records (the `w` structs) are not original source, they
 *   express the observed stack layout; the local names are chosen for their stack slots.
 * - Instance_DrawAnimParts: Mesh::TransformRange(u32,u32); Mat44_Mul uses the explicit-out signature;
 *   MatrixReturnStorage preserves the three constructor-free return temporaries.
 * - Instance_CalcWorldMatrix: RawMatrix is trivial storage for the original constructor-free matrix return temporaries.
 */
/* BYTES: dead-code, slot-group, slot-name. */
#include "sdw_types.h"
class Mat44;
#include "sdw_enums.h"
#include "../sdk/d3d7.h"
#include "../sdk/crt.h"

/* the Mat44 members the functions below use */
#define SDW_MEMBERS_Mat44 Mat44();
#define SDW_MEMBERS_D3DApp void SetTransform(u32 state, Mat44 *m);
#include "sdw_classes.h"
#define SDW_INLINE_D3DAPP_SETTRANSFORM_U32_MAT44 1
#include "../app/d3dapp_inlines.h"
#undef SDW_INLINE_D3DAPP_SETTRANSFORM_U32_MAT44
#include "sdw_enums.h"
#include "../sdk/d3d7.h"

/* ---- this object's data, in address order ---- */
Mat44 g_animPartMatrices[42];      /* 0x6e38d0 .bss; its constructor loop is the static initialiser at 0x54fe50 */
char g_strUnknown[] = "<unknown>"; /* 0x57e764 */
char g_fmtAnimNotFound[] = "Cannot find AnimID %d, Model %s, Anim %s\n"; /* 0x57e770 */

/* ---- declarations used by the animation helpers ---- */
#include "../app/app_main.h"
#include "../engine/game_state.h"
#include "../engine/fixed_math.h"
#include "../engine/mat44.h"
#include "../engine/screen.h"
#include "../engine/draw2d.h"
#include "world_draw.h"
#include "../engine/tex_table.h"
#include "instance.h"
#include "../engine/load_warmeshes.h"
u16 Str_Length(const char *);
u32 Anim_Start(Instance *instance, Animator *animation, u16 id, u32 mode);
u32 Anim_GetDurationMs(Instance *instance, u16 id, u8 includeFirstTrack);
void Anim_GetDebugNames(Instance *instance, u16 id, const char **modelName, const char **animationName);
void Debug_Printf(const char *format, ...);
u16 Anim_DecodeFirstKey(Animator *, void *);
u16 Anim_DecodeKey(Animator *, const void *, u16);
void Anim_InterpolatePose(Animator *);
/* Variable-size direct animation table at Model::animTable. */
struct DirectAnimTable {
    u32 count;
    AnimHeader *entries[1];
};

inline u16 NextFrame(u16 current, u16 count)
{
    return (current + 1) % count;
}
inline void SwapPoses(void *&a, void *&b)
{
    void *saved = a;
    a = b;
    b = saved;
}
inline u32 HasFlags(Animator *a, u16 mask)
{
    return a->flags & mask;
}
inline void SetFlags(Animator *a, u16 mask)
{
    a->flags |= mask;
}
inline void ClearFlags(Animator *a, u16 mask)
{
    a->flags &= (u16)~mask;
}

/* ---- declarations used by the instance matrix and part drawing ---- */
/* Trivial storage for the original constructor-free matrix return temporaries.
 * The explicit out-pointer spelling agrees with the declaration of Mat44_Mul's definition. */
struct RawMatrix {
    float m[4][4];
};
struct MatrixReturnStorage {
    float m[4][4];
    MatrixReturnStorage() {}
};

#define g_mirrorRenderFlag (g_screen.mirrorRenderFlag)

#define g_mirrorRenderOn (g_screen.mirrorRenderOn)

#define g_mirrorPlaneVert2x (g_screen.mirrorPlaneVert2x)

void Instance_CalcWorldMatrix(Instance *, Camera *, const Vec3s *, Mat44 *);

/* ---- declarations used by the key readers ---- */

/* 0x54fe9b */
u32 Anim_StartDirect(Instance *instance, Animator *animation, u16 id, u32 mode)
{
    /* cast kept: Model.animTable is an untyped block, the direct table followed by the mapped one */
    AnimHeader *a = ((DirectAnimTable *)instance->inst_model->animTable)->entries[id];
    if (a == 0)
        return 0;
    animation->cur = a;
    animation->animId = id;
    animation->timeAcc = 0;
    switch (mode) {
        case 0:
            animation->frame = NextFrame(0, animation->cur->keyCount);
            Anim_DecodeFirstKey(animation, animation->bufA);
            Anim_DecodeFirstKey(animation, animation->bufC);
            animation->frameDuration = Anim_DecodeKey(animation, animation->bufA, animation->frame);
            break;
        case 1:
            animation->frame = 0;
            SwapPoses(animation->bufA, animation->bufC);
            animation->frameDuration = Anim_DecodeFirstKey(animation, animation->bufB);
            break;
    }
    return 1;
}

/* 0x54ffb0 */
AnimKey *Anim_GetCurrentKey(Animator *animation)
{
    u16 a = 0;
    /* cast kept: the key records are variable-length; AnimHeader.keys holds their bytes */
    AnimKey *b = (AnimKey *)animation->cur->keys;
    while (a < animation->frame) {
        ++a;
        /* cast kept: the next variable-length key record starts 4 + payloadWords u16 words on */
        b = (AnimKey *)((u16 *)b + b->payloadWords + 4);
    }
    return b;
}

/* 0x54fffc */
AnimKey *Anim_GetNextKey(Animator *animation, u16 frame)
{
    u16 a = 0;
    u16 b = (frame + 1) % animation->cur->keyCount;
    /* cast kept: the key records are variable-length; AnimHeader.keys holds their bytes */
    AnimKey *c = (AnimKey *)animation->cur->keys;
    while (a != b) {
        ++a;
        /* cast kept: the next variable-length key record starts 4 + payloadWords u16 words on */
        c = (AnimKey *)((u16 *)c + c->payloadWords + 4);
    }
    return c;
}

/* 0x55005c */
void Anim_GetDebugNames(Instance *instance, u16 id, const char **modelName, const char **animationName)
{
    u16 a;
    u16 *b;
    u16 c, d, e;
    const char *f = g_strUnknown;
    if (!g_animNameTable) {
        *animationName = f;
        *modelName = f;
        return;
    }
    /* cast kept: the name table is a raw block of u16 counts and ids with the strings embedded in it */
    b = (u16 *)g_animNameTable;
    a = *b++;
    while (a--) {
        e = *b++;
        c = *b++;
        *modelName = (char *)b; /* cast kept: a string embedded in the u16 stream */
        b += (Str_Length(*modelName) + 2) / 2;
        while (c--) {
            d = *b++;
            *animationName = (char *)b; /* cast kept: a string embedded in the u16 stream */
            b += (Str_Length(*animationName) + 2) / 2;
            if (e == *instance->record && d == id)
                return;
        }
    }
    *animationName = f;
    *modelName = f;
}

/* ---- 0x550196: Anim_Start ---- */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
inline AnimHeader *StartLookup(Instance *instance, u16 id, u32 mode)
{
    struct {
        const char *modelName;
        const char *animationName;
        u32 *mappedTable;
        AnimHeader *entry;
    } w;
    /* Skip directCount and that many direct entries. The 32-bit
     * address expression preserves VC6's original count-before-base loads. */
    /* cast kept: Model.animTable is an untyped block; the mapped table's entries are addresses stored as u32 */
    w.mappedTable = (u32 *)(4 * *(u32 *)instance->inst_model->animTable + 4 + (u32)instance->inst_model->animTable);
    if (id >= w.mappedTable[0])
        return 0;
    w.entry = (AnimHeader *)w.mappedTable[id + 1]; /* cast kept: an address stored as u32 */
    if (w.entry == 0) {
        if (mode & ANIM_SET_OPTIONAL)
            return 0;
        Anim_GetDebugNames(instance, id, &w.modelName, &w.animationName);
        Debug_Printf(g_fmtAnimNotFound, id, w.modelName, w.animationName);
    }
    return w.entry;
}
inline void StartSetFlags(Animator *animation, u16 mask)
{
    animation->flags |= mask;
}
inline void StartClearFlags(Animator *animation, u16 mask)
{
    animation->flags &= (u16)~mask;
}

u32 Anim_Start(Instance *instance, Animator *animation, u16 id, u32 mode)
{
    AnimHeader *data = StartLookup(instance, id, mode);
    if (data == 0) {
        StartClearFlags(animation, ANIM_F_LOOP);
        StartSetFlags(animation, ANIM_F_FINISHED);
        StartSetFlags(animation, ANIM_F_PLAYING);
        animation->animId = id;
        return 0;
    }
    if (mode & ANIM_SET_LOOP)
        StartSetFlags(animation, ANIM_F_LOOP);
    else
        StartClearFlags(animation, ANIM_F_LOOP);
    StartClearFlags(animation, ANIM_F_FINISHED);
    StartSetFlags(animation, ANIM_F_PLAYING);
    animation->cur = data;
    animation->animId = id;
    animation->timeAcc = 0;
    if (mode & ANIM_SET_BLEND) {
        animation->frame = 0;
        SwapPoses(animation->bufA, animation->bufC);
        animation->frameDuration = Anim_DecodeFirstKey(animation, animation->bufB);
    } else {
        animation->frame = NextFrame(0, animation->cur->keyCount);
        Anim_DecodeFirstKey(animation, animation->bufA);
        Anim_DecodeFirstKey(animation, animation->bufC);
        animation->frameDuration = Anim_DecodeKey(animation, animation->bufA, animation->frame);
    }
    return 1;
}

/* ---- 0x5503d8: Anim_GetDurationMs ---- */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
static __inline AnimHeader *Anim_LookupDuration(Instance *instance, u16 animationId)
{
    struct {
        const char *modelName;
        const char *animationName;
        u32 *mapped;
        AnimHeader *found;
    } w;

    /* 32-bit count-before-base address arithmetic, as in Anim_Start.
     * Skip the direct entries to reach the mapped count. */
    /* cast kept: Model.animTable is an untyped block; the mapped table's entries are addresses stored as u32 */
    w.mapped = (u32 *)(4 * *(u32 *)instance->inst_model->animTable + 4 + (u32)instance->inst_model->animTable);
    if ((u32)animationId >= *w.mapped)
        return 0;
    w.found = (AnimHeader *)w.mapped[animationId + 1]; /* cast kept: an address stored as u32 */
    if (!w.found) {
        /* The original keeps this constant-false branch from its lookup path. */
        if (0)
            return 0;
        Anim_GetDebugNames(instance, animationId, &w.modelName, &w.animationName);
        Debug_Printf(g_fmtAnimNotFound, animationId, w.modelName, w.animationName);
    }
    return w.found;
}

/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; pad fill gaps */
u32 Anim_GetDurationMs(Instance *instance, u16 animationId, u8 includeFirstTrack)
{
    /* Existing caller name retained; this byte includes the first KEY. */
    struct {
        AnimKey *key;
        u16 pad;
        u16 index;
        u32 total;
        AnimHeader *animation;
    } w;
    w.animation = Anim_LookupDuration(instance, animationId);
    if (!w.animation)
        return 0;

    /* cast kept: the key records are variable-length; AnimHeader.keys holds their bytes */
    w.key = (AnimKey *)w.animation->keys;
    if (includeFirstTrack)
        w.total = w.key->durationMs;
    else
        w.total = 0;
    for (w.index = 1; w.index < w.animation->keyCount; w.index++) {
        /* cast kept: the next variable-length key record starts 4 + payloadWords u16 words on */
        w.key = (AnimKey *)((u16 *)w.key + w.key->payloadWords + 4);
        w.total += w.key->durationMs;
    }
    return w.total;
}

/* 0x5504e1 */
void Anim_RestorePose(Animator *animation)
{
    u16 a = animation->frame - 1;
    u16 b;
    if (a == 0xffff)
        a = 0;
    Anim_DecodeFirstKey(animation, animation->bufA);
    SwapPoses(animation->bufA, animation->bufB);
    for (b = 1; b <= a; ++b)
        Anim_DecodeKey(animation, animation->bufB, b);
    Anim_DecodeFirstKey(animation, animation->bufA);
    SwapPoses(animation->bufA, animation->bufB);
    for (b = 1; b <= animation->frame; ++b)
        Anim_DecodeKey(animation, animation->bufB, b);
    Anim_InterpolatePose(animation);
}

/* 0x55060c */
void Anim_Advance(Animator *animation)
{
    if (HasFlags(animation, ANIM_F_PLAYING))
        Anim_InterpolatePose(animation);
    else
        return;
    if (!HasFlags(animation, ANIM_F_LOOP) && HasFlags(animation, ANIM_F_FINISHED))
        return;
    animation->timeAcc += ((u32)g_animDt * animation->speed) >> 12;
    ClearFlags(animation, ANIM_F_FINISHED);
    while (animation->timeAcc >= (u32)(animation->frameDuration << 10)) {
        animation->timeAcc -= animation->frameDuration << 10;
        animation->frame = NextFrame(animation->frame, animation->cur->keyCount);
        if (animation->frame == 0) {
            SetFlags(animation, ANIM_F_FINISHED);
            if (!HasFlags(animation, ANIM_F_LOOP)) {
                SwapPoses(animation->bufC, animation->bufB);
                ClearFlags(animation, ANIM_F_PLAYING);
                return;
            }
            if (!HasFlags(animation, ANIM_F_ALT_WRAP)) {
                Anim_DecodeFirstKey(animation, animation->bufA);
                animation->frame = NextFrame(0, animation->cur->keyCount);
                animation->frameDuration = Anim_DecodeKey(animation, animation->bufA, animation->frame);
            } else {
                SwapPoses(animation->bufA, animation->bufB);
                animation->frameDuration = Anim_DecodeFirstKey(animation, animation->bufB);
            }
        } else {
            SwapPoses(animation->bufA, animation->bufB);
            animation->frameDuration = Anim_DecodeKey(animation, animation->bufA, animation->frame);
        }
    }
}

/* 0x550868 */
void Anim_PostInitStub(Instance *instance, Animator *animation) {}

/* 0x55086d */
/* BYTES(dead-code): d is never used: it fills an unreferenced word of the original 0x70-byte frame */
void Anim_GetRootOffset(Instance *instance, Animator *animation, Vec3s *out)
{
    ModelJoint *a;
    Vec3f b;
    AnimJointPose *c;
    u32 d; /* original unreferenced word in the 0x70-byte frame */
    Mat44 e;
    Vec3f f;
    /* cast kept: Animator's pose buffers are untyped (void *) blocks of AnimJointPose */
    c = (AnimJointPose *)animation->bufC;
    a = instance->inst_model->joints;
    e.SetRotXZY(Math_Angle4096ToRadians_2(instance->rot.x & 0xfff), Math_Angle4096ToRadians_2(instance->rot.y & 0xfff),
                Math_Angle4096ToRadians_2(instance->rot.z & 0xfff));
    b.x = (float)a->offset.x + c->pos[0];
    b.y = (float)a->offset.y + c->pos[1];
    b.z = (float)a->offset.z + c->pos[2];
    f.x = e.m[0][0] * b.x + e.m[1][0] * b.y + e.m[2][0] * b.z + e.m[3][0];
    f.y = e.m[0][1] * b.x + e.m[1][1] * b.y + e.m[2][1] * b.z + e.m[3][1];
    f.z = e.m[0][2] * b.x + e.m[1][2] * b.y + e.m[2][2] * b.z + e.m[3][2];
    out->x = (s16)(f.x / 8.0f);
    out->y = (s16)(f.y / 8.0f);
    out->z = (s16)(f.z / 8.0f);
}

/* ---- 0x5509b1: Instance_CalcWorldMatrix ---- */
#define SDW_INLINE_FREE_INSTANCEHASFLAGS_INSTANCEBASE_U16 1
#define SDW_INLINE_FREE_INSTANCESETFLAGS_INSTANCEBASE_U16 1
#include "instance_inlines.h"
#undef SDW_INLINE_FREE_INSTANCEHASFLAGS_INSTANCEBASE_U16
#undef SDW_INLINE_FREE_INSTANCESETFLAGS_INSTANCEBASE_U16
#define SDW_INLINE_FREE_INSTANCECLEARFLAGS_INSTANCEBASE_U16 1
#include "instance_inlines.h"
#undef SDW_INLINE_FREE_INSTANCECLEARFLAGS_INSTANCEBASE_U16
/* BYTES(dead-code): unused_2 is never used: it fills an unreferenced slot of the original frame */
/* BYTES(slot-name, inferred): local names chosen for their stack slots (tools/vc6_locals.py), not recovered */
void Instance_CalcWorldMatrix(Instance *instance, Camera *camera, const Vec3s *scale, Mat44 *out)
{
    if (InstanceHasFlags(instance, INST_F_ATTACHED)) {
        AttachLink *link_18 = instance->attachLink;
        if (link_18->flags.matrixValid) {
            *out = link_18->matrix;
            if (link_18->flags.hasLocal) {
                Vec3f point_9, result_18;
                point_9.x = link_18->localOffset.x * 8.0f;
                point_9.y = link_18->localOffset.y * 8.0f;
                point_9.z = link_18->localOffset.z * 8.0f;
                result_18.x =
                    point_9.x * out->m[0][0] + point_9.y * out->m[1][0] + point_9.z * out->m[2][0] + out->m[3][0];
                result_18.y =
                    point_9.x * out->m[0][1] + point_9.y * out->m[1][1] + point_9.z * out->m[2][1] + out->m[3][1];
                result_18.z =
                    point_9.x * out->m[0][2] + point_9.y * out->m[1][2] + point_9.z * out->m[2][2] + out->m[3][2];
                out->m[3][0] = result_18.x;
                out->m[3][1] = result_18.y;
                out->m[3][2] = result_18.z;
            }
            if (link_18->flags.hasWorld) {
                out->m[3][0] += link_18->worldOffset.x * 8.0f;
                out->m[3][1] += link_18->worldOffset.y * 8.0f;
                out->m[3][2] += link_18->worldOffset.z * 8.0f;
            }
            InstanceSetFlags(instance, INST_F_DRAWN);
        } else
            InstanceClearFlags(instance, INST_F_DRAWN);
    } else {
        Mat44 scaleMatrix_7;
        Mat44 rotationMatrix_8;
        Mat44 translationMatrix_0;
        Vec4i position_14, transformed_5, bounds_1;
        u32 unused_2;
        RawMatrix product_29, worldProduct_18;
        if (scale)
            scaleMatrix_7.SetScale(Math_Fixed10ToFloat_s16(scale->x), Math_Fixed10ToFloat_s16(scale->y),
                                   Math_Fixed10ToFloat_s16(scale->z));
        else
            scaleMatrix_7.SetIdentity();
        rotationMatrix_8.SetRotXZY(Math_Angle4096ToRadians_2(instance->rot.x & 0xfff),
                                   Math_Angle4096ToRadians_2(instance->rot.y & 0xfff),
                                   Math_Angle4096ToRadians_2(instance->rot.z & 0xfff));
        if (g_mirrorRenderFlag == -1 || g_mirrorRenderOn == 1) {
            rotationMatrix_8.m[0][1] = -rotationMatrix_8.m[0][1];
            rotationMatrix_8.m[1][1] = -rotationMatrix_8.m[1][1];
            rotationMatrix_8.m[2][1] = -rotationMatrix_8.m[2][1];
            instance->pos.y = g_mirrorPlaneVert2x - instance->pos.y;
        }
        translationMatrix_0.SetTranslation(instance->pos.x * 8.0f, instance->pos.y * 8.0f, instance->pos.z * 8.0f);
        /* cast kept: RawMatrix gives Mat44_Mul the original's constructor-free return temporaries */
        *out = *Mat44_Mul((Mat44 *)&worldProduct_18, Mat44_Mul((Mat44 *)&product_29, &scaleMatrix_7, &rotationMatrix_8),
                          &translationMatrix_0);
        bounds_1.x = instance->boundCenter.x;
        bounds_1.y = instance->boundCenter.y;
        bounds_1.z = instance->boundCenter.z;
        position_14.x = instance->pos.x;
        position_14.y = instance->pos.y;
        position_14.z = instance->pos.z;
        transformed_5.x = (s16)(position_14.x * camera->viewMat.m[0][0] + position_14.y * camera->viewMat.m[1][0] +
                                position_14.z * camera->viewMat.m[2][0] + camera->viewMat.m[3][0]);
        transformed_5.y = (s16)(position_14.x * camera->viewMat.m[0][1] + position_14.y * camera->viewMat.m[1][1] +
                                position_14.z * camera->viewMat.m[2][1] + camera->viewMat.m[3][1]);
        transformed_5.z = (s16)(position_14.x * camera->viewMat.m[0][2] + position_14.y * camera->viewMat.m[1][2] +
                                position_14.z * camera->viewMat.m[2][2] + camera->viewMat.m[3][2]);
        bounds_1.x += transformed_5.x;
        bounds_1.y += transformed_5.y;
        bounds_1.z += transformed_5.z;
        /* cast kept: bounds_1 is a Vec4i, the 16-byte slot of the original frame (as a Vec3i it no longer matches) */
        Instance_UpdateVisibility(instance, (const Vec3i *)&bounds_1, instance->boundRadius, 1);
        if (g_mirrorRenderFlag == -1 || g_mirrorRenderOn == 1)
            instance->pos.y = g_mirrorPlaneVert2x - instance->pos.y;
    }
}

/* ---- 0x550e98: Instance_DrawAnimParts ---- */
/* BYTES(dead-code): unused_9 is never used: it fills an unreferenced slot of the original frame */
/* BYTES(slot-name, inferred): local names chosen for their stack slots (tools/vc6_locals.py), not recovered */
void Instance_DrawAnimParts(Instance *instance, Animator *animation, Camera *camera, const Vec3s *scale)
{
    Mat44 world;
    Instance_CalcWorldMatrix(instance, camera, scale, &world);
    g_pD3DAppMain->SetTransform(D3DTRANSFORMSTATE_VIEW, &camera->viewMat);
    if (InstanceHasFlags(instance, INST_F_DRAWN)) {
        u32 unused_9;
        Mesh *mesh_9;
        u16 vertexCount_0;
        AnimJointPose *pose_9;
        Mat44 *matrix_6;
        u16 index_0;
        AnimJointPose *parentPose_3;
        u16 firstVertex_17;
        ModelJoint *joint_13;
        AttachLink *link_2;
        AttachLink **links_8;
        u8 childCount_2;
        /* cast kept: Animator's pose buffers are untyped (void *) blocks of AnimJointPose */
        pose_9 = (AnimJointPose *)animation->bufC;
        joint_13 = instance->inst_model->joints;
        matrix_6 = g_animPartMatrices;
        matrix_6->SetTranslation(joint_13->offset.x + pose_9->pos[0], joint_13->offset.y + pose_9->pos[1],
                                 joint_13->offset.z + pose_9->pos[2]);
        matrix_6->MulInPlace(world);
        pose_9++;
        joint_13++;
        matrix_6++;
        for (index_0 = 1; index_0 < animation->nbJoints; index_0++, pose_9++, joint_13++, matrix_6++) {
            Mat44 translation;
            /* cast kept: Animator's pose buffers are untyped (void *) blocks of AnimJointPose */
            parentPose_3 = (AnimJointPose *)animation->bufC + joint_13->parent;
            matrix_6->SetRotYXZ(pose_9->rot[0], pose_9->rot[1], pose_9->rot[2]);
            translation.SetTranslation(joint_13->offset.x * parentPose_3->scale[0] + pose_9->pos[0],
                                       joint_13->offset.y * parentPose_3->scale[1] + pose_9->pos[1],
                                       joint_13->offset.z * parentPose_3->scale[2] + pose_9->pos[2]);
            /* cast kept: MatrixReturnStorage gives Mat44_Mul the original's constructor-free return temporaries */
            *matrix_6 = *Mat44_Mul((Mat44 *)&MatrixReturnStorage(),
                                   Mat44_Mul((Mat44 *)&MatrixReturnStorage(), matrix_6, &translation),
                                   &g_animPartMatrices[joint_13->parent]);
        }
        joint_13 = instance->inst_model->joints + 1;
        /* cast kept: Animator's pose buffers are untyped (void *) blocks of AnimJointPose */
        pose_9 = (AnimJointPose *)animation->bufC + 1;
        for (index_0 = 1; index_0 < animation->nbJoints; index_0++, joint_13++, pose_9++) {
            Mat44 scaling;
            if (pose_9->channels & (ANIMKEY_SCALE_X | ANIMKEY_SCALE_Y | ANIMKEY_SCALE_Z)) {
                if (scale)
                    scaling.SetScale(Math_U16ToUnitFloat(scale->x) * pose_9->scale[0],
                                     Math_U16ToUnitFloat(scale->y) * pose_9->scale[1],
                                     Math_U16ToUnitFloat(scale->z) * pose_9->scale[2]);
                else
                    scaling.SetScale(pose_9->scale[0], pose_9->scale[1], pose_9->scale[2]);
                /* cast kept: MatrixReturnStorage gives Mat44_Mul the original's constructor-free return temporary */
                g_animPartMatrices[index_0] =
                    *Mat44_Mul((Mat44 *)&MatrixReturnStorage(), &scaling, &g_animPartMatrices[index_0]);
            }
        }
        if (instance->attachedChildCount) {
            links_8 = AttachLink_FindFirstChildSlot(instance);
            childCount_2 = instance->attachedChildCount;
            while (childCount_2 > 0) {
                link_2 = *links_8++;
                childCount_2--;
                if (!link_2->flags.hasRot) {
                    if (!link_2->flags.rootRotation)
                        (link_2)->matrix = g_animPartMatrices[link_2->partIndex];
                    else {
                        (link_2)->matrix = g_animPartMatrices[0];
                        (link_2)->matrix.m[3][0] = g_animPartMatrices[link_2->partIndex].m[3][0];
                        (link_2)->matrix.m[3][1] = g_animPartMatrices[link_2->partIndex].m[3][1];
                        (link_2)->matrix.m[3][2] = g_animPartMatrices[link_2->partIndex].m[3][2];
                    }
                } else {
                    Mat44 rotation;
                    if (!link_2->flags.rootRotation) {
                        rotation.SetRotXYZ(Math_Angle4096ToRadians_2(link_2->rot.x),
                                           Math_Angle4096ToRadians_2(link_2->rot.y),
                                           Math_Angle4096ToRadians_2(link_2->rot.z));
                        (link_2)->matrix = g_animPartMatrices[link_2->partIndex];
                        (link_2)->matrix.MulInPlace(rotation);
                    } else {
                        rotation.SetRotXYZ(Math_Angle4096ToRadians_2(link_2->rot.x),
                                           Math_Angle4096ToRadians_2(link_2->rot.y),
                                           Math_Angle4096ToRadians_2(link_2->rot.z));
                        (link_2)->matrix = g_animPartMatrices[0];
                        (link_2)->matrix.m[3][0] = (link_2)->matrix.m[3][1] = (link_2)->matrix.m[3][2] = 0.0f;
                        (link_2)->matrix.MulInPlace(rotation);
                        (link_2)->matrix.m[3][0] = g_animPartMatrices[link_2->partIndex].m[3][0];
                        (link_2)->matrix.m[3][1] = g_animPartMatrices[link_2->partIndex].m[3][1];
                        (link_2)->matrix.m[3][2] = g_animPartMatrices[link_2->partIndex].m[3][2];
                    }
                }
                link_2->flags.matrixValid = 1;
            }
        }
        firstVertex_17 = 0;
        mesh_9 = (Mesh *)Texture_FindByResource(instance->inst_model); /* cast kept: the lookup returns void * */
        joint_13 = instance->inst_model->joints + 1;
        /* cast kept: Animator's pose buffers are untyped (void *) blocks of AnimJointPose */
        pose_9 = (AnimJointPose *)animation->bufC + 1;
        for (index_0 = 1; index_0 < animation->nbJoints; index_0++, joint_13++, pose_9++) {
            vertexCount_0 = joint_13->vertexCount;
            g_animPartMatrices[index_0].MulInPlace(g_matUnk6d5428);
            g_pD3DAppMain->SetTransform(D3DTRANSFORMSTATE_WORLD, &g_animPartMatrices[index_0]);
            mesh_9->TransformRange(firstVertex_17, vertexCount_0);
            firstVertex_17 += vertexCount_0;
        }
    }
}

/* 0x55154e */
void Anim_InterpolatePose(Animator *animation)
{
    float a = (float)animation->timeAcc / (float)(animation->frameDuration << 10);
    u16 b;
    AnimJointPose *c, *d, *e;
    float f, g, h, i, j, k;
    for (b = 0; b < animation->nbJoints; ++b) {
        /* cast kept: Animator's pose buffers are untyped (void *) blocks of AnimJointPose */
        e = (AnimJointPose *)animation->bufA + b;
        d = (AnimJointPose *)animation->bufB + b;
        c = (AnimJointPose *)animation->bufC + b;
        if ((f = d->rot[0] - e->rot[0]) > 3.1415927f)
            f -= 6.2831855f;
        if (f < -3.1415927f)
            f += 6.2831855f;
        if ((g = f * a + e->rot[0]) < 0.0f)
            g += 6.2831855f;
        else
            while (g >= 6.2831855f)
                g -= 6.2831855f;
        c->rot[0] = g;
        if ((h = d->rot[1] - e->rot[1]) > 3.1415927f)
            h -= 6.2831855f;
        if (h < -3.1415927f)
            h += 6.2831855f;
        if ((i = h * a + e->rot[1]) < 0.0f)
            i += 6.2831855f;
        else
            while (i >= 6.2831855f)
                i -= 6.2831855f;
        c->rot[1] = i;
        if ((j = d->rot[2] - e->rot[2]) > 3.1415927f)
            j -= 6.2831855f;
        if (j < -3.1415927f)
            j += 6.2831855f;
        if ((k = j * a + e->rot[2]) < 0.0f)
            k += 6.2831855f;
        else
            while (k >= 6.2831855f)
                k -= 6.2831855f;
        c->rot[2] = k;
        c->pos[0] = (d->pos[0] - e->pos[0]) * a + e->pos[0];
        c->pos[1] = (d->pos[1] - e->pos[1]) * a + e->pos[1];
        c->pos[2] = (d->pos[2] - e->pos[2]) * a + e->pos[2];
        c->scale[0] = (d->scale[0] - e->scale[0]) * a + e->scale[0];
        c->scale[1] = (d->scale[1] - e->scale[1]) * a + e->scale[1];
        c->scale[2] = (d->scale[2] - e->scale[2]) * a + e->scale[2];
        c->channels = d->channels;
    }
}

/* ---- 0x551845-0x5523bc: the packed key readers ---- */
/* The original decoders share this expansion. Each listed channel is read
 * separately; cursor alignment and signed/unsigned conversion are preserved.
 * cast kept (BYTES, WORDS, CURSOR): an entry's values are s8 bytes or s16 words by its ANIMKEY_8BIT bit, and the
 * byte cursor is realigned to the next u16 by its address. */
#define DECODE_ENTRY(CURSOR, POSE, JOINT, CTRL, BYTES, WORDS)                                                \
    CTRL = *CURSOR++;                                                                                        \
    JOINT = CTRL & 0x3f;                                                                                     \
    POSE[JOINT].rot[0] = POSE[JOINT].rot[1] = POSE[JOINT].rot[2] = POSE[JOINT].pos[0] = POSE[JOINT].pos[1] = \
        POSE[JOINT].pos[2] = 0.0f;                                                                           \
    POSE[JOINT].scale[0] = POSE[JOINT].scale[1] = POSE[JOINT].scale[2] = 1.0f;                               \
    POSE[JOINT].channels = CTRL;                                                                             \
    if (CTRL & ANIMKEY_8BIT) {                                                                               \
        /* cast kept: the packed key stream is read as bytes and as words */                                 \
        BYTES = (s8 *)CURSOR;                                                                                \
        if (CTRL & ANIMKEY_ROT_X)                                                                            \
            POSE[JOINT].rot[0] = Math_Angle128ToRadians(*BYTES++);                                           \
        if (CTRL & ANIMKEY_ROT_Y)                                                                            \
            POSE[JOINT].rot[1] = Math_Angle128ToRadians(*BYTES++);                                           \
        if (CTRL & ANIMKEY_ROT_Z)                                                                            \
            POSE[JOINT].rot[2] = Math_Angle128ToRadians(*BYTES++);                                           \
        if (CTRL & ANIMKEY_POS_X)                                                                            \
            POSE[JOINT].pos[0] = (float)*BYTES++;                                                            \
        if (CTRL & ANIMKEY_POS_Y)                                                                            \
            POSE[JOINT].pos[1] = (float)*BYTES++;                                                            \
        if (CTRL & ANIMKEY_POS_Z)                                                                            \
            POSE[JOINT].pos[2] = (float)*BYTES++;                                                            \
        if (CTRL & ANIMKEY_SCALE_X)                                                                          \
            POSE[JOINT].scale[0] = Math_U8ToUnitFloat((u8) * BYTES++);                                       \
        if (CTRL & ANIMKEY_SCALE_Y)                                                                          \
            POSE[JOINT].scale[1] = Math_U8ToUnitFloat((u8) * BYTES++);                                       \
        if (CTRL & ANIMKEY_SCALE_Z)                                                                          \
            POSE[JOINT].scale[2] = Math_U8ToUnitFloat((u8) * BYTES++);                                       \
        if ((u32)BYTES % 2) {                                                                                \
            BYTES++;                                                                                         \
            /* cast kept: the packed key stream is read as bytes and as words */                             \
            CURSOR = (u16 *)BYTES;                                                                           \
        } else                                                                                               \
            CURSOR = (u16 *)BYTES;                                                                           \
    } else {                                                                                                 \
        WORDS = (s16 *)CURSOR;                                                                               \
        if (CTRL & ANIMKEY_ROT_X)                                                                            \
            POSE[JOINT].rot[0] = Math_Angle4096ToRadians(*WORDS++);                                          \
        if (CTRL & ANIMKEY_ROT_Y)                                                                            \
            POSE[JOINT].rot[1] = Math_Angle4096ToRadians(*WORDS++);                                          \
        if (CTRL & ANIMKEY_ROT_Z)                                                                            \
            POSE[JOINT].rot[2] = Math_Angle4096ToRadians(*WORDS++);                                          \
        if (CTRL & ANIMKEY_POS_X)                                                                            \
            POSE[JOINT].pos[0] = (float)*WORDS++;                                                            \
        if (CTRL & ANIMKEY_POS_Y)                                                                            \
            POSE[JOINT].pos[1] = (float)*WORDS++;                                                            \
        if (CTRL & ANIMKEY_POS_Z)                                                                            \
            POSE[JOINT].pos[2] = (float)*WORDS++;                                                            \
        if (CTRL & ANIMKEY_SCALE_X)                                                                          \
            POSE[JOINT].scale[0] = Math_U16ToUnitFloat((u16) * WORDS++);                                     \
        if (CTRL & ANIMKEY_SCALE_Y)                                                                          \
            POSE[JOINT].scale[1] = Math_U16ToUnitFloat((u16) * WORDS++);                                     \
        if (CTRL & ANIMKEY_SCALE_Z)                                                                          \
            POSE[JOINT].scale[2] = Math_U16ToUnitFloat((u16) * WORDS++);                                     \
        /* cast kept: the packed key stream is read as bytes and as words */                                 \
        CURSOR = (u16 *)WORDS;                                                                               \
    }

/* BYTES(dead-code): g is never used: it fills an unreferenced halfword of the original frame */
u16 Anim_DecodeFirstKey(Animator *animation, void *destination)
{
    u16 *a;
    u16 b, c, d, e, f;
    u16 g; /* unreferenced original halfword */
    s8 *h;
    s16 *i;
#define POSE ((AnimJointPose *)destination) /* cast kept: the destination pose buffer is passed as void * */
    a = (u16 *)animation->cur->keys;        /* cast kept: the key records are read as a u16 stream */
    d = *a++;
    c = *a++;
    a++;
    animation->pendingSoundId = *a++;
    for (b = 0; b < c; ++b) {
        DECODE_ENTRY(a, POSE, e, f, h, i)
    }
    return d;
#undef POSE
}

/* BYTES(dead-code): j is never used: it fills an unreferenced halfword of the original frame */
u16 Anim_DecodeKey(Animator *animation, const void *source, u16 keyIndex)
{
    u16 *a;
    u16 b, c, d, e;
    AnimJointPose *f;
    u16 g, h, i;
    u16 j; /* unreferenced original halfword */
    s8 *k;
    s16 *l;
    /* cast kept: Animator's pose buffers are untyped (void *) blocks of AnimJointPose */
    f = (AnimJointPose *)animation->bufB;
    memcpy(f, source, animation->nbJoints * sizeof(AnimJointPose));
    a = (u16 *)animation->cur->keys; /* cast kept: the key records are read as a u16 stream */
    for (c = 0; c < keyIndex; ++c) {
        g = a[2];
        a += 4;
        a += g;
    }
    e = *a++;
    d = *a++;
    a++;
    if (*a)
        animation->pendingSoundId = *a++;
    else
        a++;
    for (b = 0; b < d; ++b) {
        DECODE_ENTRY(a, f, h, i, k, l)
    }
    return e;
}

/* 0x5523bd */
void Anim_ClearPose(Animator *animation, AnimJointPose *pose)
{
    u16 a;
    for (a = 0; a < animation->nbJoints; ++a) {
        pose->rot[0] = pose->rot[1] = pose->rot[2] = 0.0f;
        pose->pos[0] = pose->pos[1] = pose->pos[2] = 0.0f;
        pose->scale[0] = pose->scale[1] = pose->scale[2] = 1024.0f;
        pose->channels = 0;
    }
}
