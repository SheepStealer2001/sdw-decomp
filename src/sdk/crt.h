/* Stand-in for the C runtime (stdio.h, fcntl.h, sys/stat.h): the constants the decompiled source names, with the SDK's own spelling and value.
 * Only what src/ uses is here: constants, types and functions (depends on sdw_types.h). */
#ifndef SDW_SDK_CRT_H
#define SDW_SDK_CRT_H

typedef unsigned int size_t;
#define NULL 0 /* C++: stddef.h / stdio.h / windef.h */
#define SEEK_CUR 1
#define SEEK_END 2
#define SEEK_SET 0
#define _O_BINARY 0x8000
#define _O_CREAT 0x0100
#define _O_RDWR 0x0002
#define _P_NOWAIT 1
#define _P_WAIT 0
#define _S_IREAD 0000400
#define _S_IWRITE 0000200


struct FILE;

typedef char *va_list; /* stdarg.h: the argument pointer walks the stack in 4-byte steps */
#define va_start(ap, v) (ap = (va_list) & v + ((sizeof(v) + 3) & ~3))
#define va_arg(ap, t) (*(t *)((ap += ((sizeof(t) + 3) & ~3)) - ((sizeof(t) + 3) & ~3)))
#define va_end(ap) (ap = (va_list)0)

/* ---- CRT (static LIBCMT; no headers for VC6 here) ---- */
struct div_t {
    int quot;
    int rem;
};

/* ---- libjpeg 6 (0x4222c0-0x42dae0, a library: declared as far as these callers use it) ---- */
typedef int jmp_buf[16];

extern "C" int _close(int fd); /* 0x568469 */
extern "C" int _isnan(double);
extern "C" long _lseek(int fd, long offset, int origin); /* 0x567c7d */
extern "C" size_t __cdecl _msize(void *);
extern "C" int _open(const char *path, int oflag, ...);  /* 0x567d55 */
extern "C" int _read(int fd, void *buf, unsigned int n); /* 0x56803b */
extern "C" int __cdecl _setjmp(jmp_buf env);             /* VC6 turns it into __setjmp3(env, 0) 0x568570 */
extern "C" int _spawnv(int mode, const char *path, const char *const *argv); /* 0x565bbe */
extern "C" int _write(int fd, const void *buf, unsigned int n);              /* 0x568279 */
extern "C" int abs(int v);                                                   /* CRT, 0x566d35 */
extern "C" double __cdecl atan(double x);                                    /* 0x566e14 */
extern "C" double atan2(double, double);
extern "C" int atexit(void (*fn)(void));             /* 0x565e26 */
extern "C" double __cdecl cos(double);               /* CRT */
extern "C" div_t div(int num, int denom);            /* 0x568549 */
extern "C" __declspec(noreturn) void exit(int code); /* 0x56597b (VC6 stdlib.h: noreturn) */
extern "C" double __cdecl exp(double x);             /* 0x566ea0 */
extern "C" double fabs(double);
extern "C" int fclose(FILE *f);    /* 0x565c5b */
extern "C" int fcloseall();        /* 0x56d44c */
extern "C" double floor(double x); /* CRT 0x567aa3 */
extern "C" double fmod(double, double);
extern "C" FILE *fopen(const char *name, const char *mode);                    /* 0x565d09 */
extern "C" size_t fread(void *buf, size_t size, size_t count, FILE *f);        /* 0x5670f0 */
extern "C" void free(void *p);                                                 /* 0x566c4c */
extern "C" int fseek(FILE *f, long offset, int origin);                        /* 0x566eb4 */
extern "C" long ftell(FILE *f);                                                /* 0x566f6d */
extern "C" size_t fwrite(const void *buf, size_t size, size_t count, FILE *f); /* 0x567207 */
extern "C" char *itoa(int value, char *out, int radix);                        /* 0x573706 */
extern "C" __declspec(noreturn) void __cdecl longjmp(jmp_buf env, int value);  /* 0x5685ec */
extern "C" void *malloc(size_t size);
extern "C" int memcmp(const void *a, const void *b, size_t n); /* 0x5661f0 */
extern "C" void *memcpy(void *d, const void *s, size_t n);     /* CRT, 0x565e70 */
extern "C" void *memset(void *p, int c, size_t n);             /* 0x565d50 */
extern "C" double __cdecl pow(double, double);
extern "C" int printf(const char *fmt, ...); /* 0x567a62 */
extern "C" void qsort(void *base, size_t n, size_t size,
                      int (*compare)(const void *, const void *)); /* CRT, 0x566944 */
extern "C" int rand(void);                                         /* CRT, 0x567b7f */
extern "C" double __cdecl sin(double);                             /* CRT */
extern "C" int sprintf(char *buf, const char *fmt, ...);           /* 0x5677ee */
extern "C" double sqrt(double);                                    /* CRT 0x566594 */
extern "C" void srand(unsigned int seed);                          /* CRT, 0x567b72 */
extern "C" char *strcat(char *dst, const char *src);               /* 0x565860 */
extern "C" char *strchr(const char *s, int c);                     /* 0x5673f0, CRT */
extern "C" int strcmp(const char *a, const char *b);               /* 0x5662a0 */
extern "C" char *strcpy(char *d, const char *s);                   /* CRT, 0x565850 */
extern "C" size_t strlen(const char *s);                           /* 0x565be0 */
extern "C" int strncmp(const char *a, const char *b, size_t n);    /* 0x5661b0 */
extern "C" char *strncpy(char *dst, const char *src, size_t n);    /* 0x565a70 */
extern "C" char *strrchr(const char *, int);
extern "C" int swprintf(unsigned short *, const unsigned short *, ...);
extern "C" double __cdecl tan(double x); /* 0x566d54 */
extern "C" long time(long *t);           /* CRT, 0x567ba1 */

#endif
