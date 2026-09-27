/* PAL PC 0x42bc80: the observed callable body is a single RET.
 * Its original source identity and source-file owner are unknown. The independent
 * empty definitions live in the optimized T314 integration source. They do not
 * equate these game calls with JPEG's separately folded jpeg_mem_term symbol.
 * Keep the u8 argument conversion used by Progress and the option menu.
 */
/* BYTES: view. */
/* BYTES(view): Stub_Ret(u8) and Stub_Ret() prototypes: two prototypes for one RET at 0x42bc80: the original calls it both with and without an argument */
#ifndef SDW_EMPTY_CALL_H
#define SDW_EMPTY_CALL_H
void Stub_Ret(u8 value);
void Stub_Ret();
#endif
