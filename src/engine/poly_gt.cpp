/*
 * T024 - original object PolyGT.cpp (guessed name): SheepD3D.exe .text 0x41aa30-0x41aa6b, .rdata 0x5743f0-0x5743f4 (BsPolyTexGouraud's vtable).
 *
 * This object is BsPolyTexGouraud's constructor and destructor (0x41aa30, 0x41aa4f). Its ??_G is dropped: the vtable's
 * ??_E slot is a weak external that binds to the real ??_E
 * BsPolyTexGouraud_VectorDeletingDtor 0x415330 (defined in the Mesh object T013, where the mesh code does new[] / delete[]).
 * BsPolyTexGouraud derives from PolyTri: its constructor calls PolyTri's (0x41aa70) and its destructor ~PolyTri
 * (0x41aa87), so PolyTri's constructor is declared here.
 */
/* BYTES: inline. */
/* BYTES(inline): PolyTri::operator= (SDW_MEMBERS_PolyTri inline): explicit operator= copies the three indices one by one: the implicit one would loop over the array */
#include "sdw_types.h"

#define SDW_MEMBERS_PolyTri PolyTri();

#define SDW_MEMBERS_BsPolyTexGouraud BsPolyTexGouraud(); /* 0x41aa30 */
#include "sdw_classes.h"
#define SDW_INLINE_POLYTRI_OPERATOR_CONST_POLYTRI 1
#include "poly_tri_inlines.h"
#undef SDW_INLINE_POLYTRI_OPERATOR_CONST_POLYTRI

/* 0x41aa30 */
BsPolyTexGouraud::BsPolyTexGouraud() {}

/* 0x41aa4f */
BsPolyTexGouraud::~BsPolyTexGouraud() {}
