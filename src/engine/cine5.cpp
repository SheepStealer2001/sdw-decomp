/* match-flags: /O2 /Oy- */
/*
 * T311 - original object guessed as Cine5.cpp (data/tu_map.json). Ranges: .data 0x58172c-0x581738 (copy #5 of the
 * cinematic header's opcode-stride table). No code of its own in the map: the object is known only from its table copy,
 * one of four (#2..#5, 0x581708..0x58172c) that sit between the Cine_Construct object (T307) and the Cine_Update object
 * (T312); its code, if any, is somewhere in 0x561be0-0x5626d0, whose functions the map places in T308 as a placeholder
 * (the internal boundaries are not recoverable from codegen: every function there also matches first in its TU).
 *
 * The object's only known content, the table, is defined here: a header static (internal linkage, not const: it is in
 * .data), which VC6 emits even when nothing refers to it (at /O2 too).
 */
/* BYTES: layout, switches. */
/* BYTES(layout, inferred): the Cine.h header static: every object including the cinematic header carries this copy in its .data, referenced or not */
/* BYTES(switches): built with /O2 /Oy-, not the project recipe; the file header says why */
#include "sdw_types.h"

static u8 g_cineOpStride_58172c[9] = {0, 8, 8, 4, 2, 2, 4, 2, 2}; /* 0x58172c */
