/*
 * T017 - original object MeshAnimFrame.cpp (guessed name): SheepD3D.exe .text 0x41a830-0x41a85b, .rdata 0x5743d4-0x5743d8 (MeshAnimFrame's vtable).
 * MeshAnimFrame's constructor and destructor (0x41a830, 0x41a847). Its ??_G is dropped: the vtable's ??_E slot is a weak
 * external that binds to the real ??_E 0x420450 (MeshAnimFrame_VectorDeletingDtor, defined where BsFile does new[] /
 * delete[]).
 */
#include "sdw_types.h"

#define SDW_MEMBERS_MeshAnimFrame MeshAnimFrame(); /* 0x41a830 */
#include "sdw_classes.h"

/* 0x41a830 */
MeshAnimFrame::MeshAnimFrame() {}

/* 0x41a847 */
MeshAnimFrame::~MeshAnimFrame() {}
