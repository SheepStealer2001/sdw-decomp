/* PAL PC VisibilityManager, 0x503530-0x503ab1. ZoneList views cover
 * the adjacent box-list pointer and u16 count pairs. */
/* BYTES: view. */
/* BYTES(view): view: zonelist views cover the adjacent box-list pointer and u16 count pairs (the access widths are the original's) */
#include "sdw_types.h"
#include "sdw_enums.h"
#define SDW_MEMBERS_ScnObject       \
    static void *operator new(u32); \
    void SetUpdateMode(s32 mode);
#define SDW_MEMBERS_ZoneList void Load(u32);
#include "sdw_classes.h"
#define SDW_INLINE_SCNOBJECT_SETVISIBLE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETVISIBLE_S32
#define SDW_INLINE_SCNOBJECT_ENABLEBOXCOLLIDE_S32 1
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_ENABLEBOXCOLLIDE_S32
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32

#include "../app/app_main.h"
#include "../engine/id_list.h"
#include "../engine/scn_tools.h"
#define g_camPos (g_camera.pos)

#define SDW_INLINE_ZONELIST_FINDCONTAINING_VEC3S 1
#define SDW_INLINE_ZONELIST_LOAD_U32 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_FINDCONTAINING_VEC3S
#undef SDW_INLINE_ZONELIST_LOAD_U32
#define SDW_INLINE_FREE_PROPERTYU32_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_PROPERTYU32_VOID_U32
void VisibilityManager::Update()
{
    s32 inBox;
    u8 newState;
    inBox = whenInBox1.FindContaining(&g_camPos) || whenInBox2.FindContaining(&g_camPos) ||
            whenInBox3.FindContaining(&g_camPos) || whenInBox4.FindContaining(&g_camPos);
    newState = inBox ? VIS_HIDDEN : VIS_SHOWN;
    if (newState != state)
        SetMeshesVisible(!inBox);
    state = newState;
}
s32 VisibilityManager::HandleMessage(ScnObject *, u32, void *)
{
    return 0;
}
void VisibilityManager::PostLoadInit()
{
    SetUpdateMode(SCN_UPD_ALWAYS);
    SetVisible(0);
    EnableBoxCollide(0);
    void *properties = record;
    meshCount = 0;
    state = VIS_INIT;
    CollectMeshes((s16)PropertyU32(properties, 0));
    CollectMeshes((s16)PropertyU32(properties, 4));
    CollectMeshes((s16)PropertyU32(properties, 8));
    whenInBox1.Load(PropertyU32(properties, 0xc));
    whenInBox2.Load(PropertyU32(properties, 0x10));
    whenInBox3.Load(PropertyU32(properties, 0x14));
    whenInBox4.Load(PropertyU32(properties, 0x18));
}
void VisibilityManager::CollectMeshes(s16 id)
{
    void **models;
    s32 i;
    u16 count;
    if ((u16)id == 0)
        return;
    /* cast kept: an export id list holds record pointers of any kind; this one lists models */
    models = (void **)Scn_FindIdList((u16)id, &count);
    while (count--) {
        for (i = 0; i < g_worldObjCount; i++) {
            if (g_worldObjs[i] && g_worldObjs[i]->inst_model == models[count]) {
                if (meshCount >= 100)
                    return;
                meshes[meshCount] = g_worldObjs[i];
                savedKinds[meshCount] = g_worldObjs[i]->inst_kind;
                meshCount++;
            }
        }
    }
}
void VisibilityManager::SetMeshesVisible(s32 visible)
{
    s32 i;
    if (visible) {
        for (i = 0; i < meshCount; i++)
            meshes[i]->inst_kind = savedKinds[i];
    } else {
        for (i = 0; i < meshCount; i++)
            meshes[i]->inst_kind = INST_KIND_RIGID;
    }
}
ScnObject *VisibilityManager_Create(void *record)
{
    VisibilityManager *object = new VisibilityManager;
    object = (VisibilityManager *)object->Init(record); /* cast kept: Init returns the object as its ScnObject base */
    return object;
}
