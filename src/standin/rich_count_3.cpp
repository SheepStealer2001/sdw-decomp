/* rich_count_3.cpp - link position: at the end, after T315 (src/engine/stream_player.cpp; rich_count_1, _2, _3 in that order)
 *
 * RECONSTRUCTION - stand-in for a missing original object; it has no code at all; it reproduces the original Rich
 * header's compiler-input count.
 *
 * The original Rich header counts 299 C++ inputs compiled by this compiler (product 49, build 9044) and 29 C inputs
 * (product 48). The 316 recovered objects are 290 C++ and 26 C; the C stand-ins jcapimin.c, jcmarker.c and jdtrans.c
 * and the C++ stand-ins crt_roots, setjmp_root and icf_sort_1..4 bring that to 29 and 296, and rich_count_1..3 supply
 * the remaining three C++ inputs. The count also sets the header's size: with 296 LINK wrote the PE header at 0x118
 * instead of the original 0x110. An object with no sections but its @comp.id changes nothing else in the link.
 * Which missing originals these three stand for (code that was all removed, split files, ...) is not known.
 *
 * match-flags: /Gy
 */
/* BYTES: switches. */
/* BYTES(switches): built with /Gy, not the project recipe; the file header says why */
