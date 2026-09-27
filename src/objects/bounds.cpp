/* PAL PC bounding volumes, 0x55c0e0/0x55c304/0x55c797.
 * Work records describe observed stack slots, not substitute game layouts. */
#include "sdw_types.h"
#include "../sdk/ddraw.h"
#include "../sdk/d3d7.h"
#include "../sdk/crt.h"
#include "sdw_classes.h"
#include "../engine/tex_table.h"
struct RigidBoundsWork {
    Vec3f *vertices;
    Mesh *mesh;
    u32 byteCount;
    u16 count, index;
    Vec3s minimum;
    u16 gap1;
    Vec3s maximum;
    u16 gap2;
    void *locked;
};
SDW_SIZE(RigidBoundsWork, 0x24);
#define INIT_LIMITS(w)     \
    w.minimum.x = 0x7fff;  \
    w.minimum.y = 0x7fff;  \
    w.minimum.z = 0x7fff;  \
    w.maximum.x = -0x7fff; \
    w.maximum.y = -0x7fff; \
    w.maximum.z = -0x7fff;
#define SCAN_COORD(w, field, expr) \
    if ((expr) < w.minimum.field)  \
        w.minimum.field = (expr);
#define SCAN_MAX(w, field, expr)  \
    if ((expr) > w.maximum.field) \
        w.maximum.field = (expr);
#define CENTER_RADIUS(w)                                       \
    center->x = w.minimum.x + (w.maximum.x - w.minimum.x) / 2; \
    center->y = w.minimum.y + (w.maximum.y - w.minimum.y) / 2; \
    center->z = w.minimum.z + (w.maximum.z - w.minimum.z) / 2; \
    *radius = w.maximum.x - center->x;                         \
    if (w.maximum.y - center->y > *radius)                     \
        *radius = w.maximum.y - center->y;                     \
    if (w.maximum.z - center->z > *radius)                     \
        *radius = w.maximum.z - center->z;
void WorldObj_CalcMeshAabb(WorldObj *object)
{
    RigidBoundsWork w;
    INIT_LIMITS(w)
    /* cast kept: Texture_FindByResource returns the resource untyped; a model's is its Mesh */
    w.mesh = (Mesh *)Texture_FindByResource(object->inst_model);
    w.mesh->vbPositions->Lock(DDLOCK_WAIT, &w.locked, &w.byteCount);
    /* cast kept: the locked vertex buffer is untyped; its format (D3DFVF_XYZ) makes it Vec3f */
    w.vertices = (Vec3f *)w.locked;
    w.count = (u16)w.mesh->vertexCount;
    for (w.index = 0; w.index < w.count; w.index++) {
        SCAN_COORD(w, x, (s16)w.vertices[w.index].x)
        SCAN_COORD(w, y, (s16)w.vertices[w.index].y)
        SCAN_COORD(w, z, (s16)w.vertices[w.index].z)
        SCAN_MAX(w, x, (s16)w.vertices[w.index].x)
        SCAN_MAX(w, y, (s16)w.vertices[w.index].y)
        SCAN_MAX(w, z, (s16)w.vertices[w.index].z)
    }
    object->aabb.min.x = w.minimum.x;
    object->aabb.min.y = w.minimum.y;
    object->aabb.min.z = w.minimum.z;
    object->aabb.max.x = w.maximum.x;
    object->aabb.max.y = w.maximum.y;
    object->aabb.max.z = w.maximum.z;
    w.mesh->vbPositions->Unlock();
}
struct AnimBoundsWork {
    Vec3s offset;
    u16 gap0;
    ModelJoint *joint;
    u32 index;
    u32 unused[3];
    Vec3f *vertices;
    Mesh *mesh;
    u32 byteCount;
    u16 gap1, count;
    ModelJoint *joints;
    Vec3s minimum;
    u16 gap2;
    Vec3s maximum;
    u16 gap3;
    void *locked;
    u16 accumulated, jointIndex;
};
SDW_SIZE(AnimBoundsWork, 0x48);
void Bounds_FromAnimModel(Vec3s *center, u16 *radius, Model *model)
{
    AnimBoundsWork w;
    w.accumulated = 0;
    INIT_LIMITS(w)
    /* cast kept: Texture_FindByResource returns the resource untyped; a model's is its Mesh */
    w.mesh = (Mesh *)Texture_FindByResource(model);
    w.mesh->vbPositions->Lock(DDLOCK_WAIT, &w.locked, &w.byteCount);
    /* cast kept: the locked vertex buffer is untyped; its format (D3DFVF_XYZ) makes it Vec3f */
    w.vertices = (Vec3f *)w.locked;
    w.count = model->nbJoints;
    w.joints = (ModelJoint *)malloc(w.count * sizeof(ModelJoint)); /* cast kept: malloc returns untyped memory */
    memcpy(w.joints, model->joints, w.count * sizeof(ModelJoint));
    for (w.jointIndex = 1; w.jointIndex < w.count; w.jointIndex++) {
        w.joint = &w.joints[w.jointIndex];
        w.joint->offset.x += w.joints[w.joint->parent].offset.x;
        w.joint->offset.y += w.joints[w.joint->parent].offset.y;
        w.joint->offset.z += w.joints[w.joint->parent].offset.z;
        for (w.index = w.accumulated; w.index < (u32)(w.accumulated + w.joint->vertexCount); w.index++) {
            w.offset.x = w.joint->offset.x;
            w.offset.y = w.joint->offset.y;
            w.offset.z = w.joint->offset.z;
            SCAN_COORD(w, x, (s16)(w.vertices[w.index].x + w.offset.x) / 8)
            SCAN_COORD(w, y, (s16)(w.vertices[w.index].y + w.offset.y) / 8)
            SCAN_COORD(w, z, (s16)(w.vertices[w.index].z + w.offset.z) / 8)
            SCAN_MAX(w, x, (s16)(w.vertices[w.index].x + w.offset.x) / 8)
            SCAN_MAX(w, y, (s16)(w.vertices[w.index].y + w.offset.y) / 8)
            SCAN_MAX(w, z, (s16)(w.vertices[w.index].z + w.offset.z) / 8)
        }
        w.accumulated += w.joint->vertexCount;
    }
    CENTER_RADIUS(w)
    free(w.joints);
    w.mesh->vbPositions->Unlock();
}
void Bounds_FromRigidModel(Vec3s *center, u16 *radius, void *model)
{
    RigidBoundsWork w;
    INIT_LIMITS(w)
    /* cast kept: the model parameter is void * (see engine/scenaric.cpp); its resource comes back untyped: a Mesh */
    w.mesh = (Mesh *)Texture_FindByResource((Model *)model);
    w.mesh->vbPositions->Lock(DDLOCK_WAIT, &w.locked, &w.byteCount);
    /* cast kept: the locked vertex buffer is untyped; its format (D3DFVF_XYZ) makes it Vec3f */
    w.vertices = (Vec3f *)w.locked;
    w.count = (u16)w.mesh->vertexCount;
    for (w.index = 0; w.index < w.count; w.index++) {
        SCAN_COORD(w, x, (s16)w.vertices[w.index].x)
        SCAN_COORD(w, y, (s16)w.vertices[w.index].y)
        SCAN_COORD(w, z, (s16)w.vertices[w.index].z)
        SCAN_MAX(w, x, (s16)w.vertices[w.index].x)
        SCAN_MAX(w, y, (s16)w.vertices[w.index].y)
        SCAN_MAX(w, z, (s16)w.vertices[w.index].z)
    }
    CENTER_RADIUS(w)
    w.mesh->vbPositions->Unlock();
}
