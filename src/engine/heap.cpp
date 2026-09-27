/*
 * T289, guessed original file Heap.cpp: .text 0x54d510-0x54e7f4, .data 0x57e6d0-0x57e6f0
 * (g_log2Nibble, g_ctzNibble). The two stubs before it belong to other objects: MCard_Stub_54d4f5 0x54d4f5 (the end of
 * T287, src/objects/mcard.cpp) and Stub_Ret0_54d500 (T288, src/engine/heap_stub.cpp).
 *
 * Heap - the engine's block allocator (SheepD3D.exe 0x54d510-0x54e7f3, one original source file by address).
 *
 * A heap manages one caller-supplied arena. A block is {u32 sizeFlags; HeapBlock *next; HeapBlock *prev; ...}: the size
 * (header included) in bits 2..27, bit 1 = this block is free, bit 0 = the block before it is free (its size is then in
 * the u32 just below this header, its footer), bits 28..31 = log2(align)-2 for Heap_AllocAligned blocks. Free blocks
 * form a doubly linked list headed by freeHead, whose prev points to itself; a permanent 0x10-byte end sentinel ends
 * the arena. An allocated block keeps the magic 0x98765432 in its second word, which nothing checks. Heap_Alloc is a
 * bounded best fit (see there). A second allocator, bitmap pools for small sizes, is set up only for arenas above
 * 1 MB; neither shipped heap (the 512 KB level heap g_scenaricHeap 0x6cff98, the 32 KB list-node heap g_listNodeHeap
 * 0x6defa8) is that large, so it is dead code in the shipped game.
 *
 * Heap_CeilPow2 and Heap_Log2Floor match as C and as C++; they are functions that only /G6 reproduces.
 *
 * Two devices reproduce the original stack frames; they are claims about the machine code, not about the source text:
 * - The inline helpers below. VC6 (/Od /Ob1) expands them in place and leaves some arguments in stack temporaries
 *   (observed here: an argument that dereferences a pointer, a non-trivial one the body uses more than once, and any
 *   parameter the body modifies; a plain local, a constant or a single-use sum of locals is substituted). The original
 *   frames have exactly those temporaries, in the order these helpers create them, and with HeapBlock_Size
 *   Heap_AllocAligned evaluates `size - (a - b)` in the original operand order (the plain mask expression does not).
 *   They have no address of their own, so their names are not recovered.
 * - Local variable names. VC6 lays out the locals of a scope by a hash of their names, so the names were chosen to give
 *   the original offsets. The rule, measured on 1590 names with no exception: with h = h*4 + (h >> 4) + c over the
 *   name's characters (32-bit), bucket = (h ^ (h >> 16)) & 15; locals are placed from EBP-4 downward in bucket order
 *   0..15, the last-declared first inside a bucket, each at its natural alignment. A nested block's locals follow the
 *   enclosing scope's, and inline-expansion temporaries follow all declared locals.
 *
 * Casts: the heap computes block addresses from raw byte offsets, so its pointer / integer casts are kept; each
 * function that has them says so.
 */
/* BYTES: flow, inline. */
#define SDW_MEMBERS_Heap void SetFreeHead(HeapBlock *b);
#include "sdw_classes.h"

u32 Heap_CeilPow2(u32 x);
u8 Heap_Log2Floor(u32 x);

#define HEAP_SIZE_MASK 0x0ffffffc /* bits 2..27 of sizeFlags: the block size */
#define HEAP_PREV_FREE 1
#define HEAP_FREE 2
#define HEAP_MAGIC 0x98765432 /* written into an allocated block's second word, never checked */

/* ---- inline helpers (no address of their own; descriptive names, see the header) ---- */

/* Makes b a free block of `size` bytes linked between prev and next, and writes its footer (the size, in the last u32
 * of the block) and the successor's "previous block is free" bit. */
/* BYTES(inline): source-only inline: its expansion leaves the original's argument temporaries */
inline void HeapBlock_SetFree(HeapBlock *b, HeapBlock *prev, HeapBlock *next, u32 size)
{
    u32 *footer;
    b->next = next;
    b->prev = prev;
    b->sizeFlags = (size & 0x0fffffff) | HEAP_FREE;
    footer = (u32 *)((u8 *)b + size - 4); /* cast kept: the footer is the last u32 of the block, at a byte offset */
    *footer = size;
    footer++;
    *footer |= HEAP_PREV_FREE;
}

/* b becomes the head of the free list (the head's prev points to itself). */
/* BYTES(inline): source-only inline: its expansion leaves the original's argument temporaries */
inline void Heap::SetFreeHead(HeapBlock *b)
{
    freeHead = b;
    freeHead->prev = freeHead;
}

/* Links b after a on the free list. */
/* BYTES(inline): source-only inline: its expansion leaves the original's argument temporaries */
inline void HeapBlock_Link(HeapBlock *a, HeapBlock *b)
{
    a->next = b;
    a->next->prev = a;
}

/* Size of the block before b if that block is free (its footer, the u32 just below b), else 0. */
/* BYTES(inline): source-only inline: its expansion leaves the original's argument temporaries */
inline u32 HeapBlock_PrevFreeSize(HeapBlock *b)
{
    u32 *footer;
    if (b->sizeFlags & HEAP_PREV_FREE) {
        footer = (u32 *)b - 1; /* cast kept: the footer is the u32 just below the header */
        return *footer;
    }
    return 0;
}

/* Block size in bytes, header included. */
/* BYTES(inline): source-only inline: its expansion leaves the original's argument temporaries and keeps the size - (a - b) operand order of Heap_AllocAligned */
inline u32 HeapBlock_Size(HeapBlock *b)
{
    return b->sizeFlags & HEAP_SIZE_MASK;
}

/* Records log2(align)-2 in bits 28..31 of the header. */
/* BYTES(inline): source-only inline: its expansion leaves the original's argument temporaries */
inline void HeapBlock_SetAlignShift(HeapBlock *b, u8 shift)
{
    b->sizeFlags |= shift << 28;
}

/* x rounded up to a multiple of align (a power of 2). */
/* BYTES(inline): source-only inline: its expansion leaves the original's argument temporaries */
inline u32 Heap_AlignUp(u32 x, u32 align)
{
    align--;
    return (x + align) & ~align;
}

/* ---- the heap ---- */

/* 0x54d510 - hands the arena [block, block+size) to the heap; arenas above 1 MB also get the small-block pools. */
void Heap::Init(u8 *block, u32 size)
{
    InitArena(block, block + size, 4);
    if (size > 0x100000) {
        InitSmallPools(0x20000, 0x40, 0x1000);
        InitAux(0x8000, 0x400);
    }
}

/* 0x54d561 - teardown in reverse order; the arena itself belongs to the caller. */
void Heap::Term()
{
    TermAux();
    TermSmallPools();
    TermArena();
}

/* 0x54d584 - the arena becomes one free block between the (32-byte aligned) first block and a permanent 0x10-byte end
 * sentinel. */
void Heap::InitArena(u8 *start, u8 *end, u32 align)
{
    /* cast kept (the casts in this function): the heap computes block addresses from raw byte offsets */
    u8 *sentinel;
    align = Heap_CeilPow2(align);
    if (align < 4)
        align = 4;
    alignMask = align - 1;
    alignInvMask = ~alignMask;
    if (alignMask < 0x1f) {
        /* cast kept: the allocator lays out its own blocks in raw memory */
        firstBlock = (HeapBlock *)(Heap_AlignUp((u32)start + 8, 0x20) - 8);
    } else {
        firstBlock = (HeapBlock *)((((u32)start + 8 + alignMask) & alignInvMask) - 8);
    }
    sentinel = (u8 *)((((u32)end - 0x10) & alignInvMask) - 8);
    freeHead = firstBlock;
    endSentinel = (HeapBlock *)sentinel;
    freeCount = 1;
    endSentinel->next = endSentinel;
    endSentinel->prev = freeHead;
    endSentinel->sizeFlags = 0x10;
    /* cast kept: the allocator lays out its own blocks in raw memory */
    HeapBlock_SetFree(freeHead, freeHead, endSentinel, sentinel - (u8 *)firstBlock);
    smallBase = 0;
    smallBitmapWords = 0;
    smallEnd = 0;
    smallBitmaps = 0;
    smallMaxSlot = 0;
    smallMinSlot = 0;
}

/* 0x54d722 - empty. */
void Heap::TermArena() {}

/* 0x54d72d - carves the bitmap pools out of one Heap_Alloc block: total = the largest power of 2 not above poolBytes
 * (nothing below 1 KB), slot sizes minSlot..maxSlot rounded down to powers of 2, halved until the largest pool has at
 * least two slots. Block layout: nPools bitmap pointers, the bitmap words, then the pools. */
void Heap::InitSmallPools(u32 poolBytes, u32 minSlot, u32 maxSlot)
{
    /* cast kept (the casts in this function): the heap computes block addresses from raw byte offsets */
    u32 headerSize;
    u32 n;
    u32 k;
    u32 minSlots; /* slot count of the last (largest-slot) pool, the fewest of any pool */
    u32 poolCount;
    u32 *off; /* a byte offset kept as a u32 pointer: it advances by whole bitmap words */
    u32 total;
    total = Heap_CeilPow2(poolBytes + 1) >> 1;
    if (total < 0x400)
        return;
    if (minSlot < alignMask)
        minSlot = alignMask + 1;
    minSlots = 0;
    while (minSlots < 2) {
        smallMinSlot = Heap_CeilPow2(minSlot + 1) >> 1;
        smallMaxSlot = Heap_CeilPow2(maxSlot + 1) >> 1;
        if (smallMinSlot < 8)
            smallMinSlot = 8;
        if (smallMaxSlot < smallMinSlot)
            smallMaxSlot = smallMinSlot;
        k = Heap_Log2Floor(smallMinSlot);
        n = Heap_Log2Floor(smallMaxSlot);
        poolCount = Heap_CeilPow2(n - k + 1);
        smallMaxSlot = smallMinSlot * (1 << (poolCount - 1));
        smallPool0Slots = total / (poolCount * smallMinSlot);
        minSlots = total / (poolCount * smallMaxSlot);
        minSlot >>= 1;
        maxSlot >>= 1;
    }
    smallMinShift = Heap_Log2Floor(smallMinSlot);
    smallPoolBytes = smallPool0Slots << smallMinShift;
    smallPoolShift = Heap_Log2Floor(smallPoolBytes);
    off = 0;
    n = smallPool0Slots;
    for (k = 0; k < poolCount; k++) {
        off += (n + 31) >> 5;
        n >>= 1;
    }
    headerSize = ((u32)(off + poolCount) + alignMask) & alignInvMask;
    /* cast kept: the allocator lays out its own blocks in raw memory */
    smallBitmaps = (u32 **)Alloc(total + headerSize);
    if (smallBitmaps != 0) {
        smallBitmapWords = (u32 *)(smallBitmaps + poolCount);
        smallBase = (u8 *)smallBitmaps + headerSize;
        smallEnd = smallBase + total;
        off = 0;
        n = smallPool0Slots;
        for (k = 0; k < poolCount; k++) {
            /* cast kept: the allocator lays out its own blocks in raw memory */
            smallBitmaps[k] = (u32 *)((u32)smallBitmapWords + (u32)off);
            BitmapFill(smallBitmaps[k], n);
            off += (n + 31) >> 5;
            n >>= 1;
        }
    } else {
        smallMinSlot = 0;
        smallMaxSlot = 0;
    }
}

/* 0x54d9ff - frees the pool block. smallBase/smallEnd and the slot sizes are left as they were. */
void Heap::TermSmallPools()
{
    if (smallBitmaps != 0) {
        Free(smallBitmaps);
        smallBitmaps = 0;
    }
}

/* 0x54da2c - stores its first argument, which nothing reads; the second is ignored. */
void Heap::InitAux(u32 value, u32 unused)
{
    auxParam = value;
}

/* 0x54da42 - empty. */
void Heap::TermAux() {}

/* 0x54da4d - empty, unreferenced. */
void Heap::Stub_54da4d() {}

/* 0x54da58 - empty, unreferenced. */
void Heap::Stub_54da58() {}

/* 0x54da63 - empty, unreferenced; three stack arguments. */
void Heap::Stub3_54da63(u32 a, u32 b, u32 c) {}

/* 0x54da70 - empty, unreferenced; three stack arguments. */
void Heap::Stub3_54da70(u32 a, u32 b, u32 c) {}

/* This file's data, in .data (not const: VC6 would put a const table in .rdata). */
u8 g_log2Nibble[16] = {0, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3}; /* 0x57e6d0  highest set bit of a nibble */
u8 g_ctzNibble[16] = {0, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0};  /* 0x57e6e0  lowest set bit of a nibble */

/* 0x54da7d - x rounded up to a power of 2; 0 for 0, and 0 (wrapped) for a non-power of 2 above 0x80000000. */
u32 Heap_CeilPow2(u32 x)
{
    if (x) {
        u32 p = 1 << Heap_Log2Floor(x);
        if (p != x)
            p <<= 1;
        return p;
    }
    return 0;
}

/* 0x54dabc - index of the highest set bit; 0 for both 0 and 1. */
u8 Heap_Log2Floor(u32 x)
{
    u8 result = 0;
    if (x & 0xFFFF0000) {
        result = 16;
        x >>= 16;
    }
    if (x & 0xFF00) {
        result += 8;
        x >>= 8;
    }
    if (x & 0xF0) {
        result += 4;
        x >>= 4;
    }
    result += g_log2Nibble[x];
    return result;
}

/* 0x54db37 - index of the lowest set bit (28 for 0). Unreferenced. */
u8 Heap_LowestSetBit(u32 x)
{
    u8 result = 0;
    if (!(x & 0xFFFF)) {
        result = 16;
        x >>= 16;
    }
    if (!(x & 0xFF)) {
        result += 8;
        x >>= 8;
    }
    if (!(x & 0xF)) {
        result += 4;
        x >>= 4;
    }
    result += g_ctzNibble[x & 0xF];
    return result;
}

/* 0x54dbb2 - marks the first nBits slots of a pool free: whole words to all ones, then the top (nBits & 31) bits of the
 * next word (MSB first). */
void Heap::BitmapFill(u32 *bitmap, u32 nBits)
{
    u32 *p;
    u32 i;
    p = bitmap;
    for (i = 0; i < nBits >> 5; i++) {
        *p = 0xffffffff;
        p++;
    }
    nBits &= 31;
    if (nBits) {
        nBits = 32 - nBits;
        *p = ~((1 << nBits) - 1);
    }
}

/* 0x54dc26 - takes `need` bytes from the front of the free block `block` (blockSize bytes). A remainder of 0x18 bytes or
 * more stays on the free list in block's place; otherwise the whole block is taken and unlinked. */
void Heap::CarveBlock(HeapBlock *block, u32 blockSize, u32 need)
{
    /* cast kept (the casts in this function): the heap computes block addresses from raw byte offsets */
    HeapBlock *next;
    HeapBlock *p;
    u32 rem;
    rem = blockSize - need;
    if (rem >= 0x18) {
        /* cast kept: the allocator lays out its own blocks in raw memory */
        p = (HeapBlock *)((u8 *)block + need);
        block->next->prev = p;
        if (block == freeHead) {
            HeapBlock_SetFree(p, p, block->next, rem);
            freeHead = p;
        } else {
            HeapBlock_SetFree(p, block->prev, block->next, rem);
            block->prev->next = p;
        }
        rem += HEAP_FREE; /* subtracted from the header below: the size AND the free bit */
    } else {
        freeCount--;
        if (block == freeHead) {
            SetFreeHead(block->next);
        } else {
            p = block->prev;
            HeapBlock_Link(p, block->next);
        }
        /* cast kept: the allocator lays out its own blocks in raw memory */
        next = (HeapBlock *)((u8 *)block + HeapBlock_Size(block));
        next->sizeFlags -= HEAP_PREV_FREE;
        rem = HEAP_FREE;
    }
    block->sizeFlags -= rem;
}

/* 0x54dddb - the block just before `block` in address order, found by walking from firstBlock (linear in the number
 * of blocks). */
HeapBlock *Heap::FindPrevBlock(HeapBlock *block)
{
    /* cast kept (the casts in this function): the heap computes block addresses from raw byte offsets */
    HeapBlock *p;
    HeapBlock *next;
    p = firstBlock;
    /* cast kept: the allocator lays out its own blocks in raw memory */
    next = (HeapBlock *)((u8 *)p + HeapBlock_Size(p));
    while (next < block) {
        p = next;
        next = (HeapBlock *)((u8 *)p + HeapBlock_Size(p));
    }
    return p;
}

/* 0x54de2a - bounded best fit: it looks at no more than freeCount free blocks and, at each better fit, jumps the
 * counter halfway to the end, so it returns the smallest fit it happened to see, not the smallest there is.
 * Sizes up to smallMaxSlot try the bitmap pools first. NULL when nothing fits. */
void *Heap::Alloc(u32 size)
{
    /* cast kept (the casts in this function): the heap computes block addresses from raw byte offsets */
    HeapBlock *retval;
    HeapBlock *freeBlk;
    u32 i;
    HeapBlock *bestBlock;
    u32 bestLen;
    u32 need;
    u32 bsize;
    if (size <= smallMaxSlot) {
        void *small = AllocSmall(size);
        if (small)
            return small;
    }
    if (size < 8)
        size = 8;
    retval = 0;
    need = (size + 8 + alignMask) & alignInvMask;
    bestBlock = 0;
    bestLen = 0xffffffff;
    freeBlk = freeHead;
    for (i = 0; i < freeCount; i++) {
        bsize = HeapBlock_Size(freeBlk);
        if (bsize >= need && bestLen > bsize) {
            bestLen = bsize;
            bestBlock = freeBlk;
            i = (i + freeCount) >> 1;
        }
        freeBlk = freeBlk->next;
    }
    if (bestBlock) {
        CarveBlock(bestBlock, bestLen, need);
        retval = bestBlock;
        /* cast kept: the allocator lays out its own blocks in raw memory */
        retval->next = (HeapBlock *)HEAP_MAGIC;
        retval = (HeapBlock *)((u8 *)retval + 8);
    }
    return retval;
}

/* 0x54df38 - allocation whose user pointer is aligned to `align` (a power of 2). The same bounded best fit, scored by
 * the padding needed in front; padding of 0x18 bytes or more becomes a free block of its own, a smaller one is added to
 * the block before. log2(align)-2 goes into bits 28..31 of the header. */
/* BYTES(flow, inferred): CarveBlock is called in both arms (two call sites in the original), not hoisted */
void *Heap::AllocAligned(u32 size, u32 align)
{
    /* cast kept (the casts in this function): the heap computes block addresses from raw byte offsets */
    HeapBlock *block;
    HeapBlock *cur;
    HeapBlock *found;
    HeapBlock *newBlk;
    u32 k;
    u32 lead;
    u32 minPad;
    u32 bestLen;
    u32 need;
    u32 bsize;
    if (align <= alignMask + 1)
        return Alloc(size);
    block = 0;
    if (size < 8)
        size = 8;
    need = (size + alignMask) & alignInvMask;
    cur = freeHead;
    found = 0;
    bestLen = 0xffffffff;
    minPad = 0xffffffff;
    for (k = 0; k < freeCount; k++) {
        bsize = HeapBlock_Size(cur);
        if (bestLen >= bsize) {
            lead = Heap_AlignUp((u32)cur + 8, align) - (u32)cur;
            if (bsize >= need + lead && minPad > lead) {
                bestLen = bsize;
                found = cur;
                minPad = lead;
                k = (k + freeCount) >> 1;
            }
        }
        cur = cur->next;
    }
    if (found) {
        minPad -= 8; /* from the start of the found block to the new header */
        need += 8;
        /* cast kept: the allocator lays out its own blocks in raw memory */
        newBlk = (HeapBlock *)((u8 *)found + minPad);
        if (minPad != 0) {
            if (minPad >= 0x18) {
                /* the padding stays a free block of its own, newBlk is linked after it */
                block = found->next;
                found->next->prev = newBlk;
                HeapBlock_SetFree(found, found->prev, newBlk, HeapBlock_Size(found) - (bestLen - minPad));
                HeapBlock_SetFree(newBlk, found, block, bestLen - minPad);
                freeCount++;
                CarveBlock(newBlk, bestLen - minPad, need);
                newBlk->sizeFlags |= HEAP_PREV_FREE;
            } else {
                /* too small for a block: unlink found and give the padding to the block before it */
                if (found != freeHead) {
                    found->next->prev = found->prev;
                    found->prev->next = found->next;
                } else {
                    freeHead = found->next;
                    found->prev = freeHead;
                }
                block = FindPrevBlock(found);
                block->sizeFlags += HeapBlock_Size(found) - (bestLen - minPad);
                HeapBlock_SetFree(newBlk, newBlk, freeHead, bestLen - minPad);
                freeHead->prev = newBlk;
                freeHead = newBlk;
                CarveBlock(newBlk, bestLen - minPad, need);
            }
        } else {
            CarveBlock(newBlk, bestLen - minPad, need);
        }
        block = newBlk;
        if (align > alignMask + 1)
            HeapBlock_SetAlignShift(newBlk, Heap_Log2Floor(align) - 2);
        /* cast kept: the allocator lays out its own blocks in raw memory */
        block->next = (HeapBlock *)HEAP_MAGIC;
        block = (HeapBlock *)((u8 *)block + 8);
    }
    return block;
}

/* 0x54e2f1 - frees a block and merges it with free neighbours; pointers inside the pool range go to Heap_FreeSmall. */
void Heap::Free(void *ptr)
{
    /* cast kept (the casts in this function): the heap computes block addresses from raw byte offsets */
    HeapBlock *block;
    HeapBlock *n;
    HeapBlock *prev;
    HeapBlock *nextBlk;
    u32 size;
    /* cast kept: the allocator lays out its own blocks in raw memory */
    if ((u8 *)ptr < smallEnd && (u8 *)ptr >= smallBase) {
        FreeSmall(ptr);
        return;
    }
    /* cast kept: the allocator lays out its own blocks in raw memory */
    block = (HeapBlock *)((u8 *)ptr - 8);
    prev = (HeapBlock *)((u8 *)block - HeapBlock_PrevFreeSize(block));
    nextBlk = (HeapBlock *)((u8 *)block + HeapBlock_Size(block));
    if (prev != block) {
        /* the block before is free: grow it over this one (and over the next one too when that is free) */
        if (nextBlk->sizeFlags & HEAP_FREE) {
            n = nextBlk;
            if (n != freeHead) {
                n->next->prev = n->prev;
                n->prev->next = n->next;
            } else {
                freeHead = n->next;
                freeHead->prev = freeHead;
            }
            size = HeapBlock_Size(prev) + HeapBlock_Size(block) + HeapBlock_Size(nextBlk);
            freeCount--;
        } else {
            size = HeapBlock_Size(prev) + HeapBlock_Size(block);
        }
        HeapBlock_SetFree(prev, prev->prev, prev->next, size);
    } else if (nextBlk->sizeFlags & HEAP_FREE) {
        /* only the next block is free: this block takes its place on the list */
        n = nextBlk;
        size = HeapBlock_Size(block) + HeapBlock_Size(nextBlk);
        if (n == freeHead) {
            n->next->prev = block;
            HeapBlock_SetFree(block, block, n->next, size);
            freeHead = block;
        } else {
            HeapBlock_SetFree(block, n->prev, n->next, size);
            n->next->prev = block;
            n->prev->next = block;
        }
    } else {
        /* no free neighbour: push it on the head of the list */
        size = HeapBlock_Size(block);
        HeapBlock_SetFree(block, block, freeHead, size);
        freeHead->prev = block;
        freeHead = block;
        freeCount++;
    }
}

/* 0x54e63d - takes the first free slot (highest set bit of the first non-zero bitmap word) of the pool for
 * ceil(log2(size)); NULL when that pool is full. The size is not checked against the pool's slot size. */
void *Heap::AllocSmall(u32 size)
{
    /* cast kept (the casts in this function): the heap computes block addresses from raw byte offsets */
    u32 wordBytes;
    u8 *addr;
    u32 nSlots;
    u32 pool;
    u32 i;
    u32 highBit;
    u32 *bitmap;
    pool = Heap_Log2Floor(size);
    if ((u32)(1 << pool) != size)
        pool++;
    if (smallMinShift < pool)
        pool -= smallMinShift;
    else
        pool = 0;
    /* cast kept: the allocator lays out its own blocks in raw memory */
    addr = (u8 *)((pool << smallPoolShift) + (u32)smallBase);
    bitmap = smallBitmaps[pool];
    nSlots = smallPool0Slots >> pool;
    wordBytes = smallMinSlot << (pool + 5);
    for (i = 0; i < (nSlots + 31) >> 5; i++) {
        if (*bitmap != 0) {
            highBit = Heap_Log2Floor(*bitmap);
            *bitmap ^= 1 << highBit;
            highBit = 31 - highBit; /* MSB first: bit 31 is slot 0 */
            return addr + (highBit << (smallMinShift + pool));
        }
        bitmap++;
        addr += wordBytes;
    }
    return 0;
}

/* 0x54e767 - sets the slot's bit again. */
void Heap::FreeSmall(void *ptr)
{
    /* cast kept (the casts in this function): the heap computes block addresses from raw byte offsets */
    u32 *bitmap;
    u32 off;
    u32 pool;
    u32 slotShift;
    /* cast kept: the allocator lays out its own blocks in raw memory */
    off = (u8 *)ptr - smallBase;
    pool = off >> smallPoolShift;
    slotShift = smallMinShift + pool;
    bitmap = smallBitmaps[pool];
    pool = (off - (pool << smallPoolShift)) >> slotShift; /* now the slot index */
    bitmap += pool >> 5;
    pool &= 31;
    *bitmap |= 0x80000000 >> pool;
}
