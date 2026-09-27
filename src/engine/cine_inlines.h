/* Shared inline definitions. Select helpers before inclusion, then undefine their selectors.
 * BYTES(inline): repeated inclusion preserves each caller's dependency visibility and definition order.
 * Per-helper guards prevent redefinition when a later include selects more helpers. */

#if defined(SDW_INLINE_FREE_CINE_ISFINISHED) && !defined(SDW_INLINE_FREE_CINE_ISFINISHED_DEFINED)
#define SDW_INLINE_FREE_CINE_ISFINISHED_DEFINED
inline s32 Cine_IsFinished()
{
    s32 finished = g_cinePlayer.finished;
    return finished;
}
#endif

#if defined(SDW_INLINE_FREE_CINE_PLAY_U32_U32_BOX_BOX_VOID) && \
    !defined(SDW_INLINE_FREE_CINE_PLAY_U32_U32_BOX_BOX_VOID_DEFINED)
#define SDW_INLINE_FREE_CINE_PLAY_U32_U32_BOX_BOX_VOID_DEFINED
inline void Cine_Play(u32 id, u32 startFlags, Box *box, Box *sheep, void *text)
{
    g_cinePlayer.Start(id, startFlags, box, sheep, text, 0);
}
#endif

#if defined(SDW_INLINE_FREE_STARTCINE_U32_U32_BOX_BOX_VOID) && \
    !defined(SDW_INLINE_FREE_STARTCINE_U32_U32_BOX_BOX_VOID_DEFINED)
#define SDW_INLINE_FREE_STARTCINE_U32_U32_BOX_BOX_VOID_DEFINED
inline void StartCine(u32 id, u32 startFlags, Box *box, Box *sheepBox, void *text)
{
    g_cinePlayer.Start(id, startFlags, box, sheepBox, text, 0);
}
#endif

#if defined(SDW_INLINE_CINE_ISACTIVE) && !defined(SDW_INLINE_CINE_ISACTIVE_DEFINED)
#define SDW_INLINE_CINE_ISACTIVE_DEFINED
/* BYTES(inline): /Od callers retain the return-value temporary before testing
 * it (Fade, 0x53e777 / 0x53e8c6; Wolf, 0x485d06). */
inline s32 Cine::IsActive()
{
    return active;
}
#endif

#if defined(SDW_INLINE_CINE_ISFINISHED) && !defined(SDW_INLINE_CINE_ISFINISHED_DEFINED)
#define SDW_INLINE_CINE_ISFINISHED_DEFINED
/* BYTES(inline): a separate return temporary per read (DaffyElf, 0x436370 /
 * 0x4366d8; Gossamer Level 08, 0x452e83). */
inline s32 Cine::IsFinished()
{
    return finished;
}
#endif

#if defined(SDW_INLINE_CINE_ISWOLFREADY) && !defined(SDW_INLINE_CINE_ISWOLFREADY_DEFINED)
#define SDW_INLINE_CINE_ISWOLFREADY_DEFINED
/* BYTES(inline): the flag goes through a temporary before the test
 * (Porky Level 01, 0x459fc6). */
inline s32 Cine::IsWolfReady()
{
    return wolfReady;
}
#endif
