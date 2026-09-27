/*
 * T025 - original object PolyTri.cpp (guessed name): SheepD3D.exe .text 0x41aa70-0x41aa9b plus the COMDAT ??_GPolyTri 0x41aaa0-0x41aace, .rdata 0x5743f4-0x5743f8 (PolyTri's vtable).
 * PolyTri's constructor and destructor (0x41aa70, 0x41aa87). Its scalar deleting destructor ??_GPolyTri 0x41aaa0-0x41aace
 * survives as this object's COMDAT: nothing defines a real ??_E PolyTri, so the vtable's weak ??_E slot falls back to it.
 */
/* BYTES: inline. */
/* BYTES(inline): PolyTri::operator= (SDW_MEMBERS_PolyTri inline): explicit operator= copies the three indices one by one: the implicit one would loop over the array */
#include "sdw_types.h"

#define SDW_MEMBERS_PolyTri PolyTri();

#include "sdw_classes.h"
#define SDW_INLINE_POLYTRI_OPERATOR_CONST_POLYTRI 1
#include "poly_tri_inlines.h"
#undef SDW_INLINE_POLYTRI_OPERATOR_CONST_POLYTRI

/* 0x41aa70 */
PolyTri::PolyTri() {}

/* 0x41aa87 */
PolyTri::~PolyTri() {}
