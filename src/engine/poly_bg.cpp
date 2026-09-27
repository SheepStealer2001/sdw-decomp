/*
 * T020 - original object PolyBG.cpp (guessed name): SheepD3D.exe .text 0x41a930-0x41a96b, .rdata 0x5743e0-0x5743e4 (BsPolyBlendGouraud's vtable).
 *
 * This object is BsPolyBlendGouraud's constructor and destructor (0x41a930, 0x41a94f). Its ??_G is dropped: the
 * vtable's ??_E slot is a weak external that binds to the real ??_E BsPolyBlendGouraud_VectorDeletingDtor 0x415490
 * (defined in the Mesh object T013, where the mesh code does new[] / delete[]).
 * BsPolyBlendGouraud derives from PolyTri: its constructor calls PolyTri's (0x41aa70) and its destructor ~PolyTri
 * (0x41aa87), so PolyTri's constructor is declared here.
 */
/* BYTES: inline. */
/* BYTES(inline): PolyTri::operator= (SDW_MEMBERS_PolyTri inline): explicit operator= copies the three indices one by one: the implicit one would loop over the array */
#include "sdw_types.h"

#define SDW_MEMBERS_PolyTri PolyTri();

#define SDW_MEMBERS_BsPolyBlendGouraud BsPolyBlendGouraud(); /* 0x41a930 */
#include "sdw_classes.h"
#define SDW_INLINE_POLYTRI_OPERATOR_CONST_POLYTRI 1
#include "poly_tri_inlines.h"
#undef SDW_INLINE_POLYTRI_OPERATOR_CONST_POLYTRI

/* 0x41a930 */
BsPolyBlendGouraud::BsPolyBlendGouraud() {}

/* 0x41a94f */
BsPolyBlendGouraud::~BsPolyBlendGouraud() {}
