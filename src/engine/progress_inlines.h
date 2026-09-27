/* Shared inline definitions. Select helpers before inclusion, then undefine their selectors.
 * BYTES(inline): repeated inclusion preserves each caller's dependency visibility and definition order.
 * Per-helper guards prevent redefinition when a later include selects more helpers. */

#if defined(SDW_INLINE_FREE_ISLEVELSCENE_S8) && !defined(SDW_INLINE_FREE_ISLEVELSCENE_S8_DEFINED)
#define SDW_INLINE_FREE_ISLEVELSCENE_S8_DEFINED
/* BYTES(inline): the level-range test is materialized as 0/1 (0x50bf10). */
inline s32 IsLevelScene(s8 scene)
{
    return scene >= SCENE_LVL_00 && scene < SCENE_LEVEL_COUNT;
}
#endif

#if defined(SDW_INLINE_PROGRESS_CURRENTLEVEL) && !defined(SDW_INLINE_PROGRESS_CURRENTLEVEL_DEFINED)
#define SDW_INLINE_PROGRESS_CURRENTLEVEL_DEFINED
/* BYTES(inline): the byte return is a temporary (Weather, 0x52e0f4; Wolf, 0x48840d). */
inline s8 Progress::CurrentLevel()
{
    return currentLevel;
}
#endif

#if defined(SDW_INLINE_PROGRESS_FIELDACFLAGCLEAR) && !defined(SDW_INLINE_PROGRESS_FIELDACFLAGCLEAR_DEFINED)
#define SDW_INLINE_PROGRESS_FIELDACFLAGCLEAR_DEFINED
/* BYTES(inline): two returns preserve the branchy 0/1 temporary at 0x50bc3d. */
inline s32 Progress::FieldAcFlagClear()
{
    if (runtimeBits.fieldAcFlag)
        return 0;
    return 1;
}
#endif

#if defined(SDW_INLINE_PROGRESS_GETLANGUAGE) && !defined(SDW_INLINE_PROGRESS_GETLANGUAGE_DEFINED)
#define SDW_INLINE_PROGRESS_GETLANGUAGE_DEFINED
/* BYTES(inline): the expansion supplies the byte temporary that Load_MLT reads twice. */
inline u8 Progress::GetLanguage()
{
    return language;
}
#endif

#if defined(SDW_INLINE_PROGRESS_GETLEVEL) && !defined(SDW_INLINE_PROGRESS_GETLEVEL_DEFINED)
#define SDW_INLINE_PROGRESS_GETLEVEL_DEFINED
inline s8 Progress::GetLevel()
{
    return currentLevel;
}
#endif

#if defined(SDW_INLINE_PROGRESS_GETLEVELINDEXA) && !defined(SDW_INLINE_PROGRESS_GETLEVELINDEXA_DEFINED)
#define SDW_INLINE_PROGRESS_GETLEVELINDEXA_DEFINED
inline s8 Progress::GetLevelIndexA()
{
    return levelIndexA;
}
#endif

#if defined(SDW_INLINE_PROGRESS_SETLEVELDONE_S8) && !defined(SDW_INLINE_PROGRESS_SETLEVELDONE_S8_DEFINED)
#define SDW_INLINE_PROGRESS_SETLEVELDONE_S8_DEFINED
/* Sets the bit within its byte of levelDoneBits; preserve the byte store. */
inline void Progress::SetLevelDone(s8 level)
{
    levelDoneBits[level >> 3] |= (u8)(1 << (level & 7));
}
#endif

#if defined(SDW_INLINE_PROGRESS_SETSCENEEXITTARGET_S8) && !defined(SDW_INLINE_PROGRESS_SETSCENEEXITTARGET_S8_DEFINED)
#define SDW_INLINE_PROGRESS_SETSCENEEXITTARGET_S8_DEFINED
inline void Progress::SetSceneExitTarget(s8 level)
{
    sceneExitTarget = level;
}
#endif

#if defined(SDW_INLINE_PROGRESS_SETSECONDDEMONEXT_S32) && !defined(SDW_INLINE_PROGRESS_SETSECONDDEMONEXT_S32_DEFINED)
#define SDW_INLINE_PROGRESS_SETSECONDDEMONEXT_S32_DEFINED
/* BYTES(inline): storing on != 0 materializes neg/sbb/neg even for a constant
 * argument (0x50c08d). */
inline void Progress::SetSecondDemoNext(s32 on)
{
    runtimeBits.secondDemoNext = on ? 1 : 0;
}
#endif
