/* Shared inline definitions. Select helpers before inclusion, then undefine their selectors.
 * BYTES(inline): repeated inclusion preserves each caller's dependency visibility and definition order.
 * Per-helper guards prevent redefinition when a later include selects more helpers. */

#if defined(SDW_INLINE_FREE_CAMERAISSCRIPTED) && !defined(SDW_INLINE_FREE_CAMERAISSCRIPTED_DEFINED)
#define SDW_INLINE_FREE_CAMERAISSCRIPTED_DEFINED
inline s32 CameraIsScripted()
{
    return g_camMode == CAM_SCRIPT_BLEND_IN || g_camMode == CAM_SCRIPT_TO_SCRIPT || g_camMode == CAM_SCRIPTED;
}
#endif

#if defined(SDW_INLINE_FREE_CAMERA_GETSCRIPTOWNER) && !defined(SDW_INLINE_FREE_CAMERA_GETSCRIPTOWNER_DEFINED)
#define SDW_INLINE_FREE_CAMERA_GETSCRIPTOWNER_DEFINED
/* BYTES(inline): the mode predicate has a temporary, while the owner result
 * goes straight to the assigned variable (Wolf, 0x481d2b). */
inline ScnObject *Camera_GetScriptOwner()
{
    if (Camera_HasScriptOwner())
        return g_camScriptOwner;
    return 0;
}
#endif

#if defined(SDW_INLINE_FREE_CAMERA_HASSCRIPTOWNER) && !defined(SDW_INLINE_FREE_CAMERA_HASSCRIPTOWNER_DEFINED)
#define SDW_INLINE_FREE_CAMERA_HASSCRIPTOWNER_DEFINED
inline s32 Camera_HasScriptOwner()
{
    return g_camMode == CAM_SCRIPT_BLEND_IN || g_camMode == CAM_SCRIPT_TO_SCRIPT || g_camMode == CAM_SCRIPTED;
}
#endif

#if defined(SDW_INLINE_FREE_CAMERA_ISSCRIPTCONTROLLED) && !defined(SDW_INLINE_FREE_CAMERA_ISSCRIPTCONTROLLED_DEFINED)
#define SDW_INLINE_FREE_CAMERA_ISSCRIPTCONTROLLED_DEFINED
inline s32 Camera_IsScriptControlled()
{
    return g_camMode == CAM_SCRIPT_BLEND_IN || g_camMode == CAM_SCRIPT_TO_SCRIPT || g_camMode == CAM_SCRIPTED;
}
#endif

#if defined(SDW_INLINE_FREE_CAMERA_SCRIPT_SCNOBJECT_CAMERA_U16_U16_U16_VEC3S_U16_U32_S32) && \
    !defined(SDW_INLINE_FREE_CAMERA_SCRIPT_SCNOBJECT_CAMERA_U16_U16_U16_VEC3S_U16_U32_S32_DEFINED)
#define SDW_INLINE_FREE_CAMERA_SCRIPT_SCNOBJECT_CAMERA_U16_U16_U16_VEC3S_U16_U32_S32_DEFINED
/* BYTES(inline): non-variable rotation, position, and focal arguments get stack
 * temporaries stored right to left (TriggedStone, 0x5027cb-0x502819). */
inline void Camera_Script(ScnObject *owner, Camera *cam, u16 rotX, u16 rotY, u16 rotZ, Vec3s *pos, u16 focal, u32 mode,
                          s32 time)
{
    Camera_StartScripted(owner, cam, rotX, rotY, rotZ, pos, focal, mode, time);
}
#endif

#if defined(SDW_INLINE_CAMERA_VIEWDIR) && !defined(SDW_INLINE_CAMERA_VIEWDIR_DEFINED)
#define SDW_INLINE_CAMERA_VIEWDIR_DEFINED
/* BYTES(inline): return the third fixed-point rotation row by value, preserving
 * the six-byte temporary in Weather (0x52e08c). */
inline Vec3s Camera::ViewDir()
{
    return viewMatS.rows[2];
}
#endif
