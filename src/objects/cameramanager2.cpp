/* T117 - original object CameraManager2.cpp (guessed name).
 * Ranges: .text 0x4a07b0-0x4a0db5, .rdata 0x575c08-0x575c2c (vtable). No .data/.bss.
 * The functions are in the original order (Update, Reset, PostLoadInit, Create).
 * PAL PC CameraManager2, 0x4a07b0-0x4a0d53. */
/* BYTES: slot-group. */
#include "sdw_types.h"
#include "../engine/id_list.h"
#include "../app/app_main.h"
#include "camera.h"
#include "../engine/scn_tools.h"
#define SDW_MEMBERS_ScnObject       \
    static void *operator new(u32); \
    void SetUpdateMode(s32 mode);
#define SDW_MEMBERS_ZoneList void Load(u16 id);

#include "sdw_enums.h"
#include "scenaric_props.h"
#include "sdw_classes.h"
#define SDW_INLINE_SCNOBJECT_SETVISIBLE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETVISIBLE_S32
#define SDW_INLINE_SCNOBJECT_SETMOVEMENTENABLED_S32 1
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETMOVEMENTENABLED_S32
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32
#define SDW_INLINE_ZONELIST_LOAD_U16 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_LOAD_U16
#define SDW_INLINE_ZONELIST_CLEAR 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_CLEAR
#include "sdw_global_views.h"
extern Wolf *g_pWolf;

extern CamScriptState g_camScriptState;
#define g_camScriptEye (g_camScriptState.eye)

void Camera_StartScripted(ScnObject *, Camera *, u16, u16, u16, Vec3s *, u16, u32, s32);
#define SDW_INLINE_FREE_CAMERAISSCRIPTED 1
#include "camera_inlines.h"
#undef SDW_INLINE_FREE_CAMERAISSCRIPTED
#define SDW_INLINE_FREE_PROPU32_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_PROPU32_VOID_U32
inline u32 PropId(void *record, u32 offset)
{
    u32 field = offset;
    /* cast kept: a designer property is a 4-byte slot at a byte offset of the raw WAR record */
    return *(u32 *)((u8 *)record + field + 0x14);
}
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; alignment fill gaps */
void CameraManager2::Update()
{
    /* Explicit recovered frame keeps the original inline argument lifetimes. */
    struct Work {
        u32 resumeFlags;
        u16 resumeFocal, resumeZ, resumeY, resumeX;
        u32 startFlags;
        u16 alignment;
        u16 startFocal, startZ, startY, startX, ownerClass;
        Vec3s *point;
        ZoneList *list;
        ScnObject *owner;
        Vec3s eye;
    } w;
    w.point = &g_pWolf->pos;
    w.list = &zones;
    if (BoxList_FindContainingPoint(w.point, w.list->boxes, w.list->count)) {
        if (CameraIsScripted())
            w.owner = g_camScriptOwner;
        else
            w.owner = 0;
        if (!scriptActive || !w.owner || this == w.owner ||
            (w.ownerClass = w.owner->classId, w.ownerClass != CLASSID_CAMERAMANAGER2)) {
            if (flags & CM2_TRACK_X)
                w.eye.x = g_pWolf->pos.x;
            else
                w.eye.x = camera->eye.x;
            if (flags & CM2_TRACK_VERT)
                w.eye.y = g_pWolf->pos.y;
            else
                w.eye.y = camera->eye.y;
            if (flags & CM2_TRACK_Y2)
                w.eye.z = g_pWolf->pos.z;
            else
                w.eye.z = camera->eye.z;
            if (this != w.owner) {
                w.startFlags = scriptFlags;
                w.startFocal = camera->focal;
                w.startZ = camera->rot[2];
                w.startY = camera->rot[1];
                w.startX = camera->rot[0];
                Camera_StartScripted(this, &g_camera, w.startX, w.startY, w.startZ, &w.eye, w.startFocal, w.startFlags,
                                     0x1000);
            } else {
                if (g_camMode != CAM_SCRIPT_TO_SCRIPT) {
                    w.resumeFlags = scriptFlags & ~CAMSCR_CHAIN;
                    w.resumeFocal = camera->focal;
                    w.resumeZ = camera->rot[2];
                    w.resumeY = camera->rot[1];
                    w.resumeX = camera->rot[0];
                    Camera_StartScripted(this, &g_camera, w.resumeX, w.resumeY, w.resumeZ, &w.eye, w.resumeFocal,
                                         w.resumeFlags, 0x1000);
                } else
                    g_camScriptEye = w.eye;
            }
            scriptActive = 1;
        }
    } else if (scriptActive) {
        Camera_ReleaseScripted(this);
        scriptActive = 0;
    }
}
void CameraManager2::Reset()
{
    if (scriptActive) {
        Camera_ReleaseScripted(this);
        scriptActive = 0;
    }
}
void CameraManager2::PostLoadInit()
{
    void *a;
    u32 b;
    a = record;
    flags = PropU32(a, 8);
    camera = Scn_GetPropCamera(a, 4);
    b = PropId(a, 0);
    if (b)
        zones.Load((u16)b);
    else
        zones.Clear();
    if (camera && zones.count)
        SetUpdateMode(SCN_UPD_ALWAYS);
    else
        SetUpdateMode(SCN_UPD_NEVER);
    scriptFlags = 0;
    if (flags & CM2_BLEND_IN)
        scriptFlags |= CAMSCR_BLEND_IN;
    if (flags & CM2_BLEND_OUT)
        scriptFlags |= CAMSCR_BLEND_OUT;
    if (flags & CM2_CHAIN)
        scriptFlags |= CAMSCR_CHAIN;
    scriptActive = 0;
    SetVisible(0);
    SetMovementEnabled(0);
}
ScnObject *CameraManager2_Create(void *record)
{
    ScnLogic *object = new CameraManager2;
    object = (ScnLogic *)object->Init(record); /* cast kept: Init returns the object as a ScnObject * */
    return object;
}
