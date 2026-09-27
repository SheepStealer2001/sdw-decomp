/* Shared inline definitions. Select helpers before inclusion, then undefine their selectors.
 * BYTES(inline): repeated inclusion preserves each caller's dependency visibility and definition order.
 * Per-helper guards prevent redefinition when a later include selects more helpers. */

#if defined(SDW_INLINE_LAUNCHARC_INITCOEFFICIENTS_S32_S32_S32_S32_S32_S32_S32_S32_S32) && \
    !defined(SDW_INLINE_LAUNCHARC_INITCOEFFICIENTS_S32_S32_S32_S32_S32_S32_S32_S32_S32_DEFINED)
#define SDW_INLINE_LAUNCHARC_INITCOEFFICIENTS_S32_S32_S32_S32_S32_S32_S32_S32_S32_DEFINED
inline void LaunchArc::InitCoefficients(s32 x0, s32 x1, s32 x2, s32 y0, s32 y1, s32 y2, s32 z0, s32 z1, s32 z2)
{
    coeffs[0] = x0;
    coeffs[1] = -3 * x0 - x2 + 4 * x1;
    coeffs[2] = x0 + x2 - 2 * x1;
    coeffs[3] = y0;
    coeffs[4] = -3 * y0 - y2 + 4 * y1;
    coeffs[5] = y0 + y2 - 2 * y1;
    coeffs[6] = z0;
    coeffs[7] = -3 * z0 - z2 + 4 * z1;
    coeffs[8] = z0 + z2 - 2 * z1;
}
#endif

#if defined(SDW_INLINE_LAUNCHARC_STEP_S32_S16_S16_S16) && !defined(SDW_INLINE_LAUNCHARC_STEP_S32_S16_S16_S16_DEFINED)
#define SDW_INLINE_LAUNCHARC_STEP_S32_S16_S16_S16_DEFINED
inline s32 LaunchArc::Step(s32 dt, s16 *outX, s16 *outY, s16 *outZ)
{
    s32 squared;
    if (t < 256) {
        t = dt + t;
        if (t > 256)
            t = 256;
        squared = t * t;
        *outX = coeffs[0] + ((t * coeffs[1]) >> 8) + ((squared * coeffs[2]) >> 15);
        *outY = coeffs[3] + ((t * coeffs[4]) >> 8) + ((squared * coeffs[5]) >> 15);
        *outZ = coeffs[6] + ((t * coeffs[7]) >> 8) + ((squared * coeffs[8]) >> 15);
        return 1;
    }
    return 0;
}
#endif
