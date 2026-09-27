/* icf_sort_4.cpp - link position: after T288 (src/engine/heap_stub.cpp), before T289 (src/engine/heap.cpp)
 *
 * A stand-in for a missing original object: its code is discarded by the linker; it reproduces the original /OPT:ICF
 * survivors (together with the other icf_sort_*.cpp, see below).
 *
 * LINK 6.00.8447 folds identical functions by sorting every code COMDAT of the link, dead or alive, with MSVCRT's
 * qsort on (size, relocation count) and keeping, in each group of identical bodies, the live copy that sorts first.
 * qsort is not stable, so which copy survives depends on the whole candidate population and its order, including
 * COMDATs of objects the original link had and /OPT:REF later removed. With the recovered objects alone, 5 of the 15
 * fold groups keep the wrong copy (JPEG empty callback 0x422c30 in T040, Video_NullFrameProc 0x4222b0 in T036, the
 * jpeg_get/free_* order 0x42bc10/0x42bc30 in T059, StreamSound::SetBufferPan in T033, LIBCMT's woutput _get_int64_arg).
 * The four icf_sort_*.cpp stand-ins add unreferenced functions of these (size, relocation count) shapes at these link
 * positions, which a model of LINK's sort and the real link both show give all 15 original survivors:
 *   icf_sort_1.cpp after T095: 39/0, 111/1      icf_sort_2.cpp after T132: 5/0, 128/1
 *   icf_sort_3.cpp after T241: 43/3, 93/0       icf_sort_4.cpp after T288: 171/1, 10/0, 42/0, 47/0
 * The shapes are what matters, not the bodies (padding with nop, calls to the file's own first function); nothing
 * here is recovered game code. Every function is unreferenced, so /OPT:REF drops it: the exe keeps none of these
 * bytes, and the file has no data or bss. Changing any function's size or relocation count, or any other COMDAT in
 * the link, changes the sort and needs a new search (tools/standin.py builds and checks the stand-ins).
 *
 * match-flags: /Gy
 */
/* BYTES: switches. */
/* BYTES(switches): built with /Gy, not the project recipe; the file header says why */

void StandIn_IcfSort4_1();

/* 171 bytes, 1 relocation */
void StandIn_IcfSort4_1()
{
    __asm call StandIn_IcfSort4_1
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
}

/* 10 bytes, 0 relocations */
void StandIn_IcfSort4_2()
{
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
}

/* 42 bytes, 0 relocations */
void StandIn_IcfSort4_3()
{
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
}

/* 47 bytes, 0 relocations */
void StandIn_IcfSort4_4()
{
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
    __asm nop
}
