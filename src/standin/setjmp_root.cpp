/* setjmp_root.cpp - link position: after T270 (src/engine/text.cpp), before T271 (src/engine/game_state.cpp)
 *
 * RECONSTRUCTION - stand-in for a missing original object; its code is discarded by the linker; it reproduces the
 * original LIBCMT extraction order div -> setjmp3 -> longjmp.
 *
 * Evidence: the original exe has div 0x568549, __setjmp3 0x568570, longjmp 0x5685ec. The only
 * retained users are T283 (src/engine/jpeg_mlt.cpp), whose object references longjmp before the compiler-generated
 * __setjmp3 (the error callback precedes the decoders), and no source form of T283 that keeps its layout reverses
 * that. So __setjmp3 was first referenced by removed code between T270's div and T283's longjmp.
 *
 * The one function here is unreferenced, so /OPT:REF drops it: the exe keeps none of its bytes, and the file has no
 * data or bss. Its size and relocation count (19/1) take part in /OPT:ICF's sort of identical-function candidates, and
 * the stand-ins in icf_sort_*.cpp were chosen with it present: do not change the body.
 *
 * match-flags: /Gy
 */
/* BYTES: switches. */
/* BYTES(switches): built with /Gy, not the project recipe; the file header says why */
typedef int jmp_buf[16];
extern "C" int __cdecl _setjmp(jmp_buf env); /* VC6 turns it into __setjmp3(env, 0) */

int StandIn_SetjmpRoot(int *b)
{
    return _setjmp(b);
}
