/* crt_roots.cpp - link position: after T029 (src/engine/bs_io.cpp), before T030 (src/engine/bs_file.cpp)
 *
 * A stand-in for a missing original object: its code is discarded by the linker; it reproduces the original LIBCMT
 * extraction order purevirt -> atox -> strchr -> swprintf -> fflush right after T029's fwrite, and through it the
 * original KERNEL32 import order.
 *
 * In the original exe the CRT code runs fwrite 0x567236, _purecall 0x567340, atol 0x567349, strchr 0x5673e0, swprintf
 * 0x5674ac, _fflush_lk 0x56751b. No retained function calls atoi/atol or fflush that early (atol's only callers are CRT
 * timezone code, fflush's own body is not in the exe at all), so the first references came from code that /OPT:REF
 * removed. LINK extracts library members in the order their symbols were first referenced, and orders each DLL's
 * import table by a qsort over the import contributions in that same order: atox pulls isctype -> a_str
 * (GetStringTypeA/W right after CreateProcessA) and fflush pulls FlushFileBuffers right after GetStdHandle, as in the
 * original IAT.
 *
 * Every function here is unreferenced, so /OPT:REF drops it: the exe keeps none of these bytes, and the file has no
 * data or bss. The functions' sizes and relocation counts (10/1, 50/2, 21/1, 17/1 bytes/relocations) also take part
 * in /OPT:ICF's sort of identical-function candidates, and the stand-ins in icf_sort_*.cpp were chosen with these
 * present: do not change the bodies.
 *
 * The external symbols must stay in this order in the object (VC6 emits atoi before strchr for this body).
 *
 * match-flags: /Gy
 */
/* BYTES: switches. */
/* BYTES(switches): built with /Gy, not the project recipe; the file header says why */
extern "C" int __cdecl _purecall(void);
extern "C" int __cdecl atoi(const char *);
extern "C" char *__cdecl strchr(const char *, int);
extern "C" int __cdecl swprintf(unsigned short *, const unsigned short *, ...);
struct _iobuf;
extern "C" int __cdecl fflush(_iobuf *);

int StandIn_CrtRoots_PureCall()
{
    return (int)&_purecall; /* cast kept: the body only has to reference _purecall, at this fixed size (see above) */
}
int StandIn_CrtRoots_Parse(const char *s)
{
    const char *p = strchr(s, ':');
    if (!p)
        return 0;
    return atoi(p + 1);
}
int StandIn_CrtRoots_Format(unsigned short *b)
{
    return swprintf(b, b);
}
int StandIn_CrtRoots_Flush(_iobuf *f)
{
    return fflush(f);
}
