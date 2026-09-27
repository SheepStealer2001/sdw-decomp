#ifndef SDW_HOOK_H
#define SDW_HOOK_H
#include <stddef.h>

#define HOOK_REL32 1
#define HOOK_BAD 2

int hook_insn_len(const unsigned char *p, int *kind);
/* 0, or -2 with the reason in why */
int hook_install(void *target, void *replacement, void **original, char *why, size_t whySize);

#endif
