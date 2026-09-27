/*
 * T018 - original object MeshPartPose.cpp (guessed name): SheepD3D.exe .text 0x41a860-0x41a8e5, .rdata 0x5743d8-0x5743dc (MeshPartPose's vtable).
 *
 * This object is MeshPartPose's constructor and destructor (0x41a860, 0x41a8d1). Its ??_G is dropped: the vtable's ??_E
 * slot is a weak external that binds to the real ??_E 0x420500 (MeshPartPose_VectorDeletingDtor, defined where BsFile
 * does new[] / delete[]).
 */
#include "sdw_types.h"

#define SDW_MEMBERS_MeshPartPose MeshPartPose(); /* 0x41a860 */
#include "sdw_classes.h"

/* 0x41a860 */
MeshPartPose::MeshPartPose()
{
    rot[0] = rot[1] = rot[2] = 0.0f;
    pos[0] = pos[1] = pos[2] = 0.0f;
    scale[0] = scale[1] = scale[2] = 1.0f;
}

/* 0x41a8d1 */
MeshPartPose::~MeshPartPose() {}
