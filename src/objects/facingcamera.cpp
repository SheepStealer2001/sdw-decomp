/* PAL PC 0x4b9110-0x4b9259. */
#include "sdw_types.h"
#include "sdw_enums.h"
#define SDW_MEMBERS_ScnObject            \
    static void *operator new(u32 size); \
    void SetUpdateMode(s32 mode);
#include "sdw_classes.h"
#include "../app/app_main.h"
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32
void FacingCamera::PostLoadInit()
{
    SetUpdateMode(SCN_UPD_NEVER);
}
void FacingCamera::Render(Camera *view)
{
    RenderFacingCamera(&g_camera, 0, 0, 0);
}
ScnObject *FacingCamera_Create(void *record)
{
    FacingCamera *object = new FacingCamera;
    object = (FacingCamera *)object->Init(record); /* cast kept: Init returns the ScnObject * base of this object */
    return object;
}
