/* match-flags: /O2 /Oy- */
/*
 * T314 - original object guessed as SfxVolume.cpp (data/tu_map.json). Ranges: .data 0x581750-0x581760 (copy #8 of the
 * cinematic header's opcode-stride table), .bss 0x71cdb0-0x71cdb8 (g_sfxVolume); code: the COMDAT Sound_SetSfxVolume
 * 0x5634c0.
 * The object exists only under the map's hypothesis H9 (followed here); under H8 copy #8 opens the stream player's .data
 * and Sound_SetSfxVolume / g_sfxVolume go with Cine_Rewind.
 * The table is a header static (unreferenced here).
 */
/* BYTES: layout, switches. */
/* BYTES(layout): Stub_Ret(u8) / Stub_Ret() definitions: two empty COMDATs that /OPT:ICF folds onto the one RET at 0x42bc80; no source identity is claimed */
/* BYTES(layout): the Cine.h header static: every object including the cinematic header carries this copy in its .data, referenced or not */
/* BYTES(switches): built with /O2 /Oy-, not the project recipe; the file header says why */
#include "sdw_types.h"

static u8 g_cineOpStride_581750[9] = {0, 8, 8, 4, 2, 2, 4, 2, 2}; /* 0x581750  unreferenced here */
u8 g_sfxVolume;                                                   /* 0x71cdb0  the sound-effect volume */

/* 0x5634c0 - sets the sound-effect volume g_sfxVolume (Cine_Start mutes it with flags & 0x4000) */
void Sound_SetSfxVolume(u8 v)
{
    g_sfxVolume = v;
}

/* The two call signatures that share the one-byte RET at 0x42bc80 (Stub_Ret). Which original source file held them is
 * not recovered. This optimised object emits them as independent ordinary function COMDATs, so LINK folds them as it
 * folds any identical functions; no JPEG identity or linker alias is claimed. */
void Stub_Ret(u8 value) {}
void Stub_Ret() {}
