/* Sam member declarations; real layouts and ancestry come from sdw_classes.h.
 * Numeric stack slots and typed views preserve the matched accesses.
 * Reference commit: b7d5e995b1148dd87a38e6d3603d6ee855f8c46c.
 */
/* BYTES: inline. */
/* BYTES(inline): Sam::GetBeachCurrentBox (inline): source-only inline: in a conditional expression its expansion reproduces the original dead stack reads */
#ifndef SDW_SAM_MEMBERS_H
#define SDW_SAM_MEMBERS_H
#include "sdw_types.h"
struct SamCarryGoal;
struct SamFetchGoal;
struct SamEdgeNormal;
struct SamScreenGeometry;
inline void SamView_SetFlag16(unsigned short &word, unsigned short mask)
{
    word |= mask;
}
inline void SamView_ClearFlag16(unsigned short &word, int mask)
{
    word &= (unsigned short)~mask;
}
inline unsigned short SamUpdate_SetMask16(unsigned short mask)
{
    return mask;
}
inline unsigned short SamUpdate_ClearMask16(int mask)
{
    return (unsigned short)~mask;
}
#define SDW_MEMBERS_ScnObject                                           \
    static void *operator new(u32 size);                                \
    int SamView_HasInstanceFlag(unsigned short mask)                    \
    {                                                                   \
        return inst_flags & mask;                                       \
    }                                                                   \
    unsigned short SamView_GetClassId()                                 \
    {                                                                   \
        return classId;                                                 \
    }                                                                   \
    void SamView_SetTint(unsigned int color, short amount, int enabled) \
    {                                                                   \
        tintColor = color;                                              \
        tintAmount = amount;                                            \
        if (enabled)                                                    \
            SamView_SetFlag16(inst_flags, INST_F_TINT);                 \
        else                                                            \
            SamView_ClearFlag16(inst_flags, INST_F_TINT);               \
    }
#define SDW_MEMBERS_Mat44 Mat44();
#define SDW_MEMBERS_Screen SamScreenGeometry *GetGeometry(SamScreenGeometry *out);
#define SDW_MEMBERS_Sam                                                          \
    short GetYaw()                                                               \
    {                                                                            \
        return rot.y;                                                            \
    }                                                                            \
    void SetYaw(int yaw)                                                         \
    {                                                                            \
        rot.y = yaw & 4095;                                                      \
    }                                                                            \
    int HasDistanceOverride()                                                    \
    {                                                                            \
        return (flags & SCN_OF_NO_DIST_CULL) != 0;                               \
    }                                                                            \
    int IsDistantForRender()                                                     \
    {                                                                            \
        return !HasDistanceOverride() && camDist2 > 9000000;                     \
    }                                                                            \
    int IsAlert()                                                                \
    {                                                                            \
        return alertLevel == SAM_ALERT_RED;                                      \
    } /* VC6 /Ob1 reproduces the original dead stack reads when this accessor  \
     * appears in a conditional expression. Its field load overwrites the  \
     * scratch value before use; no uninitialized pointer is dereferenced. */ \
    CollBox *GetBeachCurrentBox()                                                \
    {                                                                            \
        return beachCsBoxCur;                                                    \
    }                                                                            \
    void SetYawRaw(short yaw)                                                    \
    {                                                                            \
        rot.y = yaw;                                                             \
    }                                                                            \
    unsigned short CurrentAnim()                                                 \
    {                                                                            \
        return anim.animId;                                                      \
    }                                                                            \
    int HasAnimFlags(unsigned short flags)                                       \
    {                                                                            \
        return anim.flags & flags;                                               \
    }
#include "sdw_classes.h"
#endif
