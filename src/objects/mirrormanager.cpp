/* PAL PC, 0x4d8aa0-0x4d9004. */
#define SDW_MEMBERS_ScnObject                    \
    static void *operator new(u32 size);         \
    void SetUpdateMode(s32 mode);                \
    void SetAlwaysRender(s32 on)                 \
    {                                            \
        if (on)                                  \
            flags |= SCN_OF_NO_PLANE_CULL;       \
        else                                     \
            flags &= (u16)~SCN_OF_NO_PLANE_CULL; \
    }

#define SDW_MEMBERS_ZoneList void Load(u32 id);

#include "sdw_types.h"
#include "sdw_enums.h"
#include "sdw_classes.h"
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32
#define SDW_INLINE_SCNOBJECT_SETBOXCOLLIDE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETBOXCOLLIDE_S32
#define SDW_INLINE_SCNOBJECT_ISINWORLD 1
#define SDW_INLINE_SCNOBJECT_ISVISIBLE 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_ISINWORLD
#undef SDW_INLINE_SCNOBJECT_ISVISIBLE

#include "../engine/id_list.h"
#include "../engine/scn_tools.h"
extern Wolf *g_pWolf;

#define SDW_INLINE_FREE_PROPU32_VOID_U32 2
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_PROPU32_VOID_U32

#define SDW_INLINE_ZONELIST_LOAD_U32 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_LOAD_U32

#define SDW_INLINE_ZONELIST_CLEAR 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_CLEAR

#define SDW_INLINE_ZONELIST_FINDCONTAININGXZ_VEC3S 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_FINDCONTAININGXZ_VEC3S

/* 0x4d8aa0 MirrorManager_Init */
void MirrorManager::PostLoadInit()
{
    void *rec;
    s32 i;
    u32 id;
    SetUpdateMode(SCN_UPD_ALWAYS);
    SetUpdateMode(SCN_UPD_CINE);
    SetAlwaysRender(1);
    SetBoxCollide(0);
    rec = record;
    id = PropU32(rec, 0x28);
    if (id)
        zones.Load(id);
    else
        zones.Clear();
    mirrorPlaneVert2x = (s16)PropU32(rec, 4);
    mirrorColor = PropU32(rec, 0x2c);
    colorIntensity = (s16)PropU32(rec, 0);
    mirrorPlaneVert2x = mirrorPlaneVert2x << 1;
    objectCount = 1;
    for (i = 0; i < 8; i++) {
        objects[objectCount] = Scn_GetPropObject(rec, i * 4 + 8);
        if (objects[objectCount])
            objectCount++;
    }
    wolfLinked = 0;
}

/* 0x4d8db1 MirrorManager_Update */
void MirrorManager::Update()
{
    if (!wolfLinked) {
        objects[0] = g_pWolf;
        wolfLinked = 1;
        SetUpdateMode(SCN_UPD_NEVER);
    }
}

/* 0x4d8e99 MirrorManager_HandleMessage */
s32 MirrorManager::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    return 0;
}

/* 0x4d8ea8 MirrorManager_Render */
void MirrorManager::Render(Camera *view)
{
    struct {
        Camera *view;
        u32 color;
        s16 intensity;
        s16 plane;
    } mirror;
    s32 i;
    ScnBody *p;
    mirror.view = view;
    mirror.color = mirrorColor;
    mirror.intensity = colorIntensity;
    mirror.plane = mirrorPlaneVert2x;
    for (i = 0; i < objectCount; i++) {
        p = (ScnBody *)objects[i]; /* cast kept: a downcast: the mirrored objects are ScnBody objects */
        if (p->IsInWorld() && p->IsVisible()) {
            if (zones.FindContainingXZ(&p->pos)) {
                if (!p->HandleMessage(this, MSG_MIRROR_RENDER, &mirror))
                    p->RenderTinted(view, mirrorColor, colorIntensity, mirrorPlaneVert2x);
            }
        }
    }
}

/* 0x4d8fa3 MirrorManager_Create */
ScnObject *MirrorManager_Create(void *record)
{
    MirrorManager *obj = new MirrorManager;
    obj = (MirrorManager *)obj->Init(record); /* cast kept: Init returns the object as its ScnObject base */
    return obj;
}
