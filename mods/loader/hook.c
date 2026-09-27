/* Function hooks: the first instructions of a game function are moved into a trampoline (followed by a jump back), and
 * replaced by a jump to the mod's function. The instruction-length decoder covers what a VC6 function can start with;
 * checked against the disassembly of every game function (tools/build_mods.py --check-hooks). */
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "hook.h"

static const unsigned char *g_pool;
static size_t g_poolUsed, g_poolSize;

static int in(unsigned char op, const unsigned char *set, int n)
{
    int i;
    for (i = 0; i < n; i++)
        if (set[i] == op)
            return 1;
    return 0;
}

static int modrm_len(const unsigned char *p)
{
    unsigned m = p[0], mod = m >> 6, rm = m & 7;
    int n = 1;
    if (mod != 3 && rm == 4) {
        n++;
        if (mod == 0 && (p[1] & 7) == 5)
            n += 4;
    }
    if (mod == 1)
        n += 1;
    else if (mod == 2)
        n += 4;
    else if (mod == 0 && rm == 5)
        n += 4;
    return n;
}

/* The length of the instruction at p, and its kind: 0 plain, HOOK_REL32 (E8 / E9 / 0F 8x, relocatable), HOOK_BAD
 * (a short jump or an opcode this decoder does not know: not movable). */
int hook_insn_len(const unsigned char *p, int *kind)
{
    static const unsigned char noimm[] = {0x27, 0x2F, 0x37, 0x3F, 0x60, 0x61, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96,
                                          0x97, 0x98, 0x99, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F, 0xA4, 0xA5, 0xA6, 0xA7, 0xAA,
                                          0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xC3, 0xC9, 0xCB, 0xCC, 0xCE, 0xCF, 0xD7, 0xF4,
                                          0xF5, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0x6C, 0x6D, 0x6E, 0x6F};
    static const unsigned char imm8[] = {0x04, 0x0C, 0x14, 0x1C, 0x24, 0x2C, 0x34, 0x3C,
                                         0x6A, 0xA8, 0xCD, 0xE4, 0xE5, 0xE6, 0xE7};
    static const unsigned char immz[] = {0x05, 0x0D, 0x15, 0x1D, 0x25, 0x2D, 0x35, 0x3D, 0x68, 0xA9};
    static const unsigned char modrm[] = {0x62, 0x63, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C,
                                          0x8D, 0x8E, 0x8F, 0xC4, 0xC5, 0xD0, 0xD1, 0xD2, 0xD3, 0xFE, 0xFF};
    static const unsigned char modrm_imm8[] = {0x80, 0x82, 0x83, 0x6B, 0xC0, 0xC1, 0xC6};
    static const unsigned char modrm_immz[] = {0x81, 0x69, 0xC7};
    const unsigned char *s = p;
    int o16 = 0, z;
    unsigned char op;
    *kind = 0;
    while (*p == 0x66 || *p == 0x67 || *p == 0xF2 || *p == 0xF3 || *p == 0x2E || *p == 0x36 || *p == 0x3E ||
           *p == 0x26 || *p == 0x64 || *p == 0x65 || *p == 0xF0) {
        o16 |= *p == 0x66;
        p++;
    }
    z = o16 ? 2 : 4;
    op = *p++;
    if (op == 0x0F) {
        unsigned char op2 = *p++;
        if (op2 >= 0x80 && op2 <= 0x8F) {
            *kind = HOOK_REL32;
            return (int)(p - s) + 4;
        }
        if (op2 == 0x31 || op2 == 0xA2)
            return (int)(p - s);
        if (op2 == 0xA4 || op2 == 0xAC || op2 == 0xBA)
            return (int)(p - s) + modrm_len(p) + 1;
        if (op2 == 0xAF || op2 == 0xB6 || op2 == 0xB7 || op2 == 0xBE || op2 == 0xBF || op2 == 0xA3 || op2 == 0xAB ||
            op2 == 0xB3 || op2 == 0xBB || op2 == 0xA5 || op2 == 0xAD || (op2 >= 0x40 && op2 <= 0x4F) ||
            (op2 >= 0x90 && op2 <= 0x9F))
            return (int)(p - s) + modrm_len(p);
        *kind = HOOK_BAD;
        return (int)(p - s);
    }
    if ((op >= 0x40 && op <= 0x5F) || in(op, noimm, sizeof noimm))
        return (int)(p - s);
    if ((op >= 0xB0 && op <= 0xB7) || in(op, imm8, sizeof imm8))
        return (int)(p - s) + 1;
    if ((op >= 0x70 && op <= 0x7F) || op == 0xEB || (op >= 0xE0 && op <= 0xE3)) {
        *kind = HOOK_BAD; /* a short jump: its target would be out of reach from the trampoline */
        return (int)(p - s) + 1;
    }
    if ((op >= 0xB8 && op <= 0xBF) || in(op, immz, sizeof immz))
        return (int)(p - s) + z;
    if (op == 0xE8 || op == 0xE9) {
        *kind = HOOK_REL32;
        return (int)(p - s) + 4;
    }
    if (op >= 0xA0 && op <= 0xA3)
        return (int)(p - s) + 4;
    if (op == 0xC2 || op == 0xCA)
        return (int)(p - s) + 2;
    if (op == 0xC8)
        return (int)(p - s) + 3;
    if ((op < 0x40 && (op & 7) < 4) || in(op, modrm, sizeof modrm) || (op >= 0xD8 && op <= 0xDF))
        return (int)(p - s) + modrm_len(p);
    if (in(op, modrm_imm8, sizeof modrm_imm8))
        return (int)(p - s) + modrm_len(p) + 1;
    if (in(op, modrm_immz, sizeof modrm_immz))
        return (int)(p - s) + modrm_len(p) + z;
    if (op == 0xF6 || op == 0xF7) {
        int reg = (p[0] >> 3) & 7, n = modrm_len(p);
        if (reg == 0 || reg == 1)
            n += op == 0xF6 ? 1 : z;
        return (int)(p - s) + n;
    }
    *kind = HOOK_BAD;
    return (int)(p - s);
}

static unsigned char *pool_alloc(size_t n)
{
    unsigned char *r;
    if (!g_pool || g_poolUsed + n > g_poolSize) {
        g_poolSize = 0x10000;
        g_pool = (const unsigned char *)VirtualAlloc(0, g_poolSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
        g_poolUsed = 0;
        if (!g_pool)
            return 0;
    }
    r = (unsigned char *)g_pool + g_poolUsed;
    g_poolUsed += (n + 15) & ~(size_t)15;
    return r;
}

static void put_jmp(unsigned char *at, const void *to)
{
    at[0] = 0xE9;
    *(int *)(at + 1) = (int)((const unsigned char *)to - (at + 5));
}

int hook_install(void *target, void *replacement, void **original, char *why, size_t whySize)
{
    unsigned char *t = (unsigned char *)target, *tramp;
    int len = 0, kind, n;
    DWORD old;
    while (len < 5) {
        n = hook_insn_len(t + len, &kind);
        if (kind == HOOK_BAD) {
            if (why)
                _snprintf(why, whySize, "cannot move the instruction at %p (%02x %02x %02x)", t + len, t[len],
                          t[len + 1], t[len + 2]);
            return -2;
        }
        len += n;
    }
    tramp = pool_alloc(len + 5);
    if (!tramp) {
        if (why)
            _snprintf(why, whySize, "no memory for a trampoline");
        return -2;
    }
    memcpy(tramp, t, len);
    for (n = 0; n < len;) { /* a moved call / jump keeps its target */
        int k, l = hook_insn_len(t + n, &k);
        if (k == HOOK_REL32)
            *(int *)(tramp + n + l - 4) += (int)(t - tramp);
        n += l;
    }
    put_jmp(tramp + len, t + len);
    if (!VirtualProtect(t, len, PAGE_EXECUTE_READWRITE, &old)) {
        if (why)
            _snprintf(why, whySize, "VirtualProtect failed at %p", t);
        return -2;
    }
    put_jmp(t, replacement);
    memset(t + 5, 0xCC, len - 5);
    VirtualProtect(t, len, old, &old);
    FlushInstructionCache(GetCurrentProcess(), t, len);
    if (original)
        *original = tramp;
    return 0;
}
