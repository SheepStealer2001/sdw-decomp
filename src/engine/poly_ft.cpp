/*
 * T022 - original object PolyFT.cpp (guessed name): SheepD3D.exe .text 0x41a9b0-0x41a9eb, .rdata 0x5743e8-0x5743ec (BsPolyTexFlat's vtable).
 *
 * This object is BsPolyTexFlat's constructor and destructor (0x41a9b0, 0x41a9cf). Its ??_G is dropped: the vtable's ??_E
 * slot is a weak external that binds to the real ??_E BsPolyTexFlat_VectorDeletingDtor 0x415280 (defined in the Mesh
 * object T013, where the mesh code does new[] / delete[]).
 * BsPolyTexFlat derives from PolyTri: its constructor calls PolyTri's (0x41aa70) and its destructor ~PolyTri
 * (0x41aa87), so PolyTri's constructor is declared here.
 */
/* BYTES: inline. */
/* BYTES(inline): PolyTri::operator= (SDW_MEMBERS_PolyTri inline): explicit operator= copies the three indices one by one: the implicit one would loop over the array */
#include "sdw_types.h"

#define SDW_MEMBERS_PolyTri PolyTri();

#define SDW_MEMBERS_BsPolyTexFlat BsPolyTexFlat(); /* 0x41a9b0 */
#include "sdw_classes.h"
#define SDW_INLINE_POLYTRI_OPERATOR_CONST_POLYTRI 1
#include "poly_tri_inlines.h"
#undef SDW_INLINE_POLYTRI_OPERATOR_CONST_POLYTRI

/* 0x41a9b0 */
BsPolyTexFlat::BsPolyTexFlat() {}

/* 0x41a9cf */
BsPolyTexFlat::~BsPolyTexFlat() {}
