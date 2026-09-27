/* jconfig.h - NOT PART OF THE IJG DISTRIBUTION. Reconstructed for Sheep, Dog 'n' Wolf's build of IJG release 6.
 *
 * Release 6 ships no configuration for Microsoft Visual C++ (jconfig.vc first appears in 6a), so the game's
 * developers wrote their own. This is the configuration that reproduces every libjpeg function in SheepD3D.exe
 * byte for byte, compiled /O2 /Oy- /Ob2 with the real VC6 headers (see README-SDW.md). It differs from what an
 * accurate description of VC6 would say in one place: HAVE_STDDEF_H is left undefined, so jinclude.h never
 * includes <stddef.h>. That changes no behaviour (stdio.h and stdlib.h define the same basics), but it changes
 * which names the compiler has seen, and with them one register tie-break in start_pass_1_quant (jquant1.c,
 * 0x42c0e0): only this configuration gives the original's order of two reloads after the dither-table loop.
 */
/* BYTES: switches. */
/* BYTES(switches): HAVE_STDDEF_H stays undefined: it changes no behaviour but gives the register tie-break start_pass_1_quant 0x42c0e0 has in the original */
#define HAVE_PROTOTYPES
#define HAVE_UNSIGNED_CHAR
#define HAVE_UNSIGNED_SHORT
#undef void
#undef const
#undef CHAR_IS_UNSIGNED
#undef HAVE_STDDEF_H
#define HAVE_STDLIB_H
#undef NEED_BSD_STRINGS
#undef NEED_SYS_TYPES_H
#undef NEED_FAR_POINTERS
#undef NEED_SHORT_EXTERNAL_NAMES
#undef INCOMPLETE_TYPES_BROKEN
#ifdef JPEG_INTERNALS
#undef RIGHT_SHIFT_IS_UNSIGNED
#endif
