/*
 * T294 - original object guessed as Instance.cpp (tu_map). Ranges: .text 0x55b1a0-0x55c0d8, .data 0x57ead0-0x57eaf0
 * (g_aabbEdgePairs), .bss 0x6e4620-0x6e4a10 (g_attachLinkStorage, g_attachLinkPool, g_attachLinkCount); static
 * initialiser 0x55b1a0 (.CRT$XCU). Contents, in address order: the attach-link storage and its initialiser
 * (0x55b1a0/0x55b1aa), the instance initialisers (0x55b1ee-0x55b768), the attach links (0x55b769-0x55bbe5) and
 * Cull_IsAabbVisible 0x55bbe6 with its CullCorner type.
 * match-init: StaticInit_g_attachLinkStorage
 *
 * .data/.bss defined here in address order: the storage (a global with a constructor, Mat44 member) is uninitialised
 * and so leads the .bss; the pool and count carry explicit zero initialisers, which VC6 keeps in definition order
 * after it. A candidate split at 0x55b510 (data/tu_map.json) is not taken: the object is kept as mapped.
 */
/* BYTES: dead-code, layout, slot-name. */
/* BYTES(layout): written '= 0' only to keep definition order in .bss */
#include "sdw_types.h"
#include "sdw_enums.h"
class Mat44;
#include "../sdk/d3d7.h"
#include "../sdk/crt.h"

#define SDW_MEMBERS_Mat44 Mat44();
#include "sdw_classes.h"

/* ---- this object's data, in address order ---- */
AttachLink g_attachLinkStorage[10];     /* 0x6e4620  the ten attach links (constructed: Mat44 member) */
AttachLink *g_attachLinkPool[10] = {0}; /* 0x6e49e0  used links first, free ones after */
u16 g_attachLinkCount = 0;              /* 0x6e4a08  links in use */
/* 0x57ead0 - the 12 edges and 4 diagonals of a box, as corner-index pairs (Cull_IsAabbVisible) */
u8 g_aabbEdgePairs[16][2] = {{0, 1}, {0, 2}, {0, 4}, {7, 3}, {7, 5}, {7, 6}, {1, 3}, {1, 5},
                             {2, 3}, {2, 6}, {4, 5}, {4, 6}, {0, 7}, {1, 6}, {2, 5}, {3, 4}};

#include "../engine/game_state.h"
#include "animation.h"
#include "bounds.h"
#include "../engine/draw2d.h"
u32 Anim_Start(Instance *, Animator *, u16, u32);

#define SDW_INLINE_FREE_INSTANCESETFLAGS_INSTANCEBASE_U16 1
#include "instance_inlines.h"
#undef SDW_INLINE_FREE_INSTANCESETFLAGS_INSTANCEBASE_U16
inline void AnimatorSetFlags(Animator *animation, u16 mask)
{
    animation->flags |= mask;
}
void *Dav_GetResourcePtr(u16 index)
{
    u8 *a;
    u32 b;
    b = g_pDav->war.table[index];
    a = g_pDav->war.blob;
    return a + (b & 0xffffff);
}
void Instance_InitBase(InstanceBase *instance, void *model, u8 mode, s32 unused)
{
    instance->inst_flags = 0;
    instance->inst_kind = INST_KIND_RIGID;
    instance->inst_mode = mode;
    /* cast kept: resource data is untyped: the caller passes the model resource as a void * */
    instance->inst_model = (Model *)model;
    if ((mode >> 5) & 1)
        InstanceSetFlags(instance, INST_F_SCN_OR_SKY);
}
void Instance_SetSecondaryRes(Instance *instance, void *resource, s32 unused)
{
    instance->secondaryRes = resource;
    InstanceSetFlags(instance, INST_F_HAS_SECONDARY);
}
/* BYTES(dead-code): c is stored and never read, as in the original */
void Instance_InitFromWarRecord(Instance *instance, u16 *record, u8 mode, s32 keepTransform)
{
    u8 *a;
    u32 b;
    u32 c;
    b = g_pDav->war.table[record[0]];
    a = g_pDav->war.blob;
    Instance_InitBase(instance, a + (b & 0xffffff), mode, keepTransform);
    instance->record = record;
    instance->attachLink = 0;
    instance->attachedChildCount = 0;
    instance->partHeight = 0x10;
    if (!keepTransform) {
        /* cast kept: the WAR record is u16 words; words 2-4 are the position */
        instance->pos = *(Vec3s *)(instance->record + 2);
        instance->rot.x = record[6];
        instance->rot.y = record[7];
        instance->rot.z = record[8];
    }
    c = mode;
    if (record[1] != 0xffff) {
        b = g_pDav->war.table[record[1]];
        if ((((b >> 24) & 0xff) & 0xbf) == WAR_RES_MESH)
            Instance_SetSecondaryRes(instance, a + (b & 0xffffff), keepTransform);
        else
            record[1] = 0xffff;
    }
}
void Animator_Init(Instance *instance, Animator *animation, void *poseBuffers, s32 keepBounds)
{
    u16 i = 0;
    InstanceSetFlags(instance, INST_F_ANIMATED);
    instance->inst_kind = INST_KIND_ANIMATED;
    animation->flags = 0;
    AnimatorSetFlags(animation, ANIM_F_PLAYING | ANIM_F_ALT_WRAP);
    animation->nbJoints = instance->inst_model->nbJoints;
    animation->bufA = poseBuffers;
    /* cast kept (bufB, bufC): the three pose buffers share one block; each is the previous one plus nbJoints poses,
     * computed on the address as an integer */
    animation->bufB = (void *)(instance->inst_model->nbJoints * sizeof(AnimJointPose) + (u32)animation->bufA);
    animation->bufC = (void *)(instance->inst_model->nbJoints * sizeof(AnimJointPose) + (u32)animation->bufB);
    animation->speed = 0x1000;
    animation->pendingSoundId = 0;
    do {
        /* cast kept: resource data is untyped: the animation table starts with its u32 count */
        if (i >= *(u32 *)instance->inst_model->animTable) {
            Anim_StartDirect(instance, animation, 0, 0);
            break;
        }
        if (Anim_Start(instance, animation, i, ANIM_SET_LOOP | ANIM_SET_OPTIONAL))
            break;
        i++;
    } while (1);
    Anim_PostInitStub(instance, animation);
    if (!keepBounds)
        Bounds_FromAnimModel(&instance->boundCenter, &instance->boundRadius, instance->inst_model);
    /* cast kept: resource data is untyped: the table's u32 count; the current animation read as its header */
    if (*(u32 *)instance->inst_model->animTable != 1 || ((AnimHeader *)animation->cur)->keyCount != 1)
        AnimatorSetFlags(animation, ANIM_F_LOOP);
}
/* BYTES(dead-code): unused_9 is zeroed and never read, as in the original */
/* BYTES(slot-name, inferred): local names chosen for their stack slots (tools/vc6_locals.py), not recovered */
void *WorldObj_CreateFromResource(u32 index)
{
    Model *model_6;
    u8 *blob_6;
    u32 unused_9;
    WorldObj *object_7;
    Model *animModel_16;
    u32 size_9;
    model_6 = 0;
    animModel_16 = 0;
    unused_9 = 0;
    object_7 = 0;
    blob_6 = g_pDav->war.blob;
    switch (((g_pDav->war.table[index] >> 24) & 0xff)) {
        case WAR_RES_MESH:
        case WAR_RES_MESH_B:
        case WAR_RES_SKY:
            /* cast kept: file and resource data are raw bytes until parsed */
            model_6 = (Model *)(blob_6 + (g_pDav->war.table[index] & 0xffffff));
            size_9 = 0x18;
            object_7 = (WorldObj *)malloc(size_9); /* cast kept: malloc returns untyped memory */
            object_7->inst_model = model_6;
            Instance_InitBase(object_7, model_6, ((g_pDav->war.table[index] >> 24) & 0xff), 0);
            WorldObj_CalcMeshAabb(object_7);
            switch (((g_pDav->war.table[index] >> 24) & 0xff)) {
                case WAR_RES_MESH_B:
                    object_7->inst_kind = INST_KIND_WAR_0B;
                    break;
                case WAR_RES_MESH:
                    object_7->inst_kind = INST_KIND_WAR_03;
                    break;
                case WAR_RES_SKY:
                    object_7->inst_kind = INST_KIND_WAR_26;
                    break;
            }
            break;
        case WAR_RES_MODEL:
            /* cast kept: file and resource data are raw bytes until parsed */
            animModel_16 = (Model *)(blob_6 + (g_pDav->war.table[index] & 0xffffff));
            size_9 = animModel_16->nbJoints * 3 * sizeof(AnimJointPose) + sizeof(AnimatedWorldObj);
            object_7 = (WorldObj *)malloc(size_9); /* cast kept: malloc returns untyped memory */
            Instance_InitBase(object_7, animModel_16, INST_MODE_WAR_ANIMATED, 0);
            /* cast kept (every (AnimatedWorldObj *)object_7 below): a downcast: a type-4 model's world object is an
             * AnimatedWorldObj, and its pose buffers follow it in the same block */
            Animator_Init(&((AnimatedWorldObj *)object_7)->inst, &((AnimatedWorldObj *)object_7)->anim,
                          (u8 *)object_7 + sizeof(AnimatedWorldObj), 0);
            /* cast kept: the object is known to be of this class here */
            ((AnimatedWorldObj *)object_7)->inst.rot.x = 0;
            ((AnimatedWorldObj *)object_7)->inst.rot.y = 0;
            ((AnimatedWorldObj *)object_7)->inst.rot.z = 0;
            ((AnimatedWorldObj *)object_7)->inst.pos.x = 0;
            ((AnimatedWorldObj *)object_7)->inst.pos.y = 0;
            ((AnimatedWorldObj *)object_7)->inst.pos.z = 0;
            ((AnimatedWorldObj *)object_7)->inst.record = 0;
            ((AnimatedWorldObj *)object_7)->inst.attachLink = 0;
            ((AnimatedWorldObj *)object_7)->inst.attachedChildCount = 0;
            ((AnimatedWorldObj *)object_7)->inst.partHeight = 0x10;
            break;
    }
    return object_7;
}
/* BYTES(dead-code): a and b are loaded and never used, as in the original */
void WorldObj_Free(WorldObj *object)
{
    void *a = object;
    u32 b = object->inst_mode;
    if (object) {
        free(object);
        object = 0;
    }
}

void AttachLink_InitPool()
{
    u32 i;
    g_attachLinkCount = 0;
    for (i = 0; i < 10; i++) {
        g_attachLinkPool[i] = &g_attachLinkStorage[i];
        g_attachLinkStorage[i].parentInst = 0;
        g_attachLinkStorage[i].parentObj = 0;
    }
}
AttachLink **AttachLink_FindFirstChildSlot(Instance *parent)
{
    u16 i = 0;
    while (i < g_attachLinkCount && g_attachLinkPool[i]->parentInst != parent)
        i++;
    if (i < g_attachLinkCount && g_attachLinkPool[i]->parentInst == parent)
        return &g_attachLinkPool[i];
    return 0;
}
AttachLink *AttachLink_InsertFreeAt(u16 index)
{
    AttachLink *link = g_attachLinkPool[g_attachLinkCount];
    u16 i;
    i = g_attachLinkCount;
    while (i > index) {
        g_attachLinkPool[i] = g_attachLinkPool[i - 1];
        i--;
    }
    g_attachLinkPool[index] = link;
    g_attachLinkCount++;
    return link;
}
void AttachLink_SetParams(AttachLink *link, u8 partIndex, const Vec3s *localOffset, const Vec3s *rot, s32 rootRotation,
                          const Vec3s *worldOffset)
{
    link->partIndex = partIndex;
    if (localOffset) {
        link->localOffset.x = localOffset->x;
        link->localOffset.y = localOffset->y;
        link->localOffset.z = localOffset->z;
        link->flags.hasLocal = 1;
    } else {
        link->localOffset.x = 0;
        link->localOffset.y = 0;
        link->localOffset.z = 0;
        link->flags.hasLocal = 0;
    }
    if (rot) {
        link->rot.x = rot->x;
        link->rot.y = rot->y;
        link->rot.z = rot->z;
        link->flags.hasRot = 1;
    } else {
        link->rot.x = 0;
        link->rot.y = 0;
        link->rot.z = 0;
        link->flags.hasRot = 0;
    }
    if (worldOffset) {
        link->worldOffset.x = worldOffset->x;
        link->worldOffset.y = worldOffset->y;
        link->worldOffset.z = worldOffset->z;
        link->flags.hasWorld = 1;
    } else {
        link->worldOffset.x = 0;
        link->worldOffset.y = 0;
        link->worldOffset.z = 0;
        link->flags.hasWorld = 0;
    }
    if (rootRotation)
        link->flags.rootRotation = 1;
    else
        link->flags.rootRotation = 0;
}
AttachLink *AttachLink_Alloc(Instance *parent, ScnObject *owner, u8 partIndex, const Vec3s *offset, const Vec3s *rot,
                             s32 rootRotation, const Vec3s *worldOffset)
{
    AttachLink *link;
    u16 i;
    if (g_attachLinkCount == 10)
        return 0;
    /* Embedded render-instance child count, ScnObject +0x12 / instance +0xe. */
    if (!parent->attachedChildCount) {
        link = g_attachLinkPool[g_attachLinkCount];
        g_attachLinkCount++;
    } else {
        i = 0;
        link = g_attachLinkPool[0];
        while (g_attachLinkPool[i]->parentInst != parent)
            i++;
        while (i < g_attachLinkCount && g_attachLinkPool[i]->parentInst == parent)
            i++;
        link = AttachLink_InsertFreeAt(i);
    }
    link->parentInst = parent;
    link->parentObj = owner;
    link->flags.matrixValid = 0;
    AttachLink_SetParams(link, partIndex, offset, rot, rootRotation, worldOffset);
    return link;
}
void AttachLink_Free(AttachLink *link)
{
    AttachLink *a;
    u16 i;
    i = 0;
    a = g_attachLinkPool[0];
    while (i < g_attachLinkCount && g_attachLinkPool[i] != link)
        i++;
    if (i < g_attachLinkCount) {
        a = g_attachLinkPool[i];
        g_attachLinkCount--;
        while (i < g_attachLinkCount) {
            g_attachLinkPool[i] = g_attachLinkPool[i + 1];
            i++;
        }
        g_attachLinkPool[g_attachLinkCount] = a;
        a->parentInst = 0;
        a->parentObj = 0;
    }
}

struct CullCorner {
    Vec3s pos;
    s16 flags;
};
/* BYTES(dead-code): f is never used: a 64-byte slot the original frame has */
s8 Cull_IsAabbVisible(const Aabb *aabb, const Camera *camera)
{
    float a;
    CullCorner b[8];
    float c;
    u32 d, e;
    Mat44 f;
    float g, h, i, j;
    CullCorner *k;
    Vec3s l;
    CullCorner *m, *n;
    a = (double)g_pViewFrustum->tanHalfFov;
    h = g_pViewFrustum->aspect;
    g = g_pViewFrustum->nearZ;
    c = (double)g_pViewFrustum->viewDistance;
    b[0].pos.x = b[1].pos.x = b[2].pos.x = b[3].pos.x = aabb->min.x;
    b[4].pos.x = b[5].pos.x = b[6].pos.x = b[7].pos.x = aabb->max.x;
    b[0].pos.y = b[1].pos.y = b[4].pos.y = b[5].pos.y = aabb->min.y;
    b[2].pos.y = b[3].pos.y = b[6].pos.y = b[7].pos.y = aabb->max.y;
    b[0].pos.z = b[2].pos.z = b[4].pos.z = b[6].pos.z = aabb->min.z;
    b[1].pos.z = b[3].pos.z = b[5].pos.z = b[7].pos.z = aabb->max.z;
    for (d = 0; d < 8; d++) {
        k = &b[d];
        l.x = k->pos.x * camera->viewMatCopy.m[0][0] + k->pos.y * camera->viewMatCopy.m[1][0] +
              k->pos.z * camera->viewMatCopy.m[2][0] + camera->viewMatCopy.m[3][0];
        l.y = k->pos.x * camera->viewMatCopy.m[0][1] + k->pos.y * camera->viewMatCopy.m[1][1] +
              k->pos.z * camera->viewMatCopy.m[2][1] + camera->viewMatCopy.m[3][1];
        l.z = k->pos.x * camera->viewMatCopy.m[0][2] + k->pos.y * camera->viewMatCopy.m[1][2] +
              k->pos.z * camera->viewMatCopy.m[2][2] + camera->viewMatCopy.m[3][2];
        k->pos.x = l.x;
        k->pos.y = l.y;
        k->pos.z = l.z;
        j = (i = k->pos.z * a) * h;
        k->flags = 0;
        if (k->pos.x > i)
            k->flags |= CULL_OC_RIGHT;
        else if (k->pos.x < -i)
            k->flags |= CULL_OC_LEFT;
        if (-(float)k->pos.y > j)
            k->flags |= CULL_OC_VPOS;
        else if (-(float)k->pos.y < -j)
            k->flags |= CULL_OC_VNEG;
        if (k->pos.z < g)
            k->flags |= CULL_OC_NEAR;
        else if (k->pos.z > c)
            k->flags |= CULL_OC_FAR;
        if (!k->flags)
            return true;
    }
    for (e = 0; e < 16; e++) {
        n = &b[g_aabbEdgePairs[e][0]];
        m = &b[g_aabbEdgePairs[e][1]];
        if (!(n->flags & m->flags))
            return true;
    }
    return false;
}
