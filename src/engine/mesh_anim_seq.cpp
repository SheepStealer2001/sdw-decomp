/*
 * Object T011 (data/tu_map.json), guessed original file MeshAnimSeq.cpp.
 *   .text 0x40b600-0x40b62b (the constructor and the virtual destructor)
 *   .rdata 0x574354-0x574358 (??_7MeshAnimSeq, emitted here with the destructor)
 * The compiler also emits ??_GMeshAnimSeq here as a COMDAT; the original linker dropped it (/OPT:REF): the vtable's ??_E
 * slot is a weak external that binds to the real ??_EMeshAnimSeq 0x40c100 of the AnimMesh object (T012,
 * src/engine/anim_mesh.cpp), so nothing refers to this ??_G.
 * MeshAnimSeq is a per-animation record (name and frames) that BsFile::ReadAnimNames fills; the mesh layer is described
 * in src/engine/anim_mesh.cpp.
 */
#include "sdw_types.h"

#define SDW_MEMBERS_MeshAnimSeq MeshAnimSeq();
#include "sdw_classes.h"

/* ---------------------------------------------------------------- MeshAnimSeq */

/* 0x40b600 */
MeshAnimSeq::MeshAnimSeq() {}

/* 0x40b617 */
MeshAnimSeq::~MeshAnimSeq() {}
