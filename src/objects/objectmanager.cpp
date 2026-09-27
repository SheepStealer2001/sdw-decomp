/* PAL PC 0x4d9e30-0x4da35e. */
/* BYTES: cast. */
#define SDW_MEMBERS_ScnObject            \
    static void *operator new(u32 size); \
    void SetUpdateMode(s32 mode);

#define SDW_MEMBERS_ZoneList void Load(u32 id);

#include "sdw_enums.h"
#include "sdw_classes.h"
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32 1
#define SDW_INLINE_SCNOBJECT_SETVISIBLE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32
#undef SDW_INLINE_SCNOBJECT_SETVISIBLE_S32
#define SDW_INLINE_SCNOBJECT_SETBOXCOLLIDE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETBOXCOLLIDE_S32

#include "../engine/id_list.h"
#include "../engine/scn_tools.h"
#include "../app/app_main.h"

#define g_camPos (g_camera.pos)


#define SDW_INLINE_FREE_PROPU32_VOID_U32 2
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_PROPU32_VOID_U32

#define SDW_INLINE_ZONELIST_LOAD_U32 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_LOAD_U32

#define SDW_INLINE_ZONELIST_CONTAINS_VEC3S 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_CONTAINS_VEC3S

/* 0x4d9e30 ObjectManager_Update */
void ObjectManager::Update()
{
    Box *p;
    u8 newState;
    p = manageBoxes.Contains(&g_camPos);
    newState = p ? 2 : 1;
    if (newState != camZoneState)
        SetObjectsActive(p);
    camZoneState = newState;
}

/* 0x4d9e9b ObjectManager_HandleMessage */
s32 ObjectManager::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    return 0;
}

/* 0x4d9eaa ObjectManager_Init */
void ObjectManager::PostLoadInit()
{
    void *p;
    u32 i;
    s16 props[20] = {8,    0xc,  0x10, 0x14, 0x18, 0x1c, 0x20, 0x24, 0x28, 0x2c,
                     0x30, 0x34, 0x38, 0x3c, 0x40, 0x44, 0x48, 0x4c, 0x50, 0x54};
    SetUpdateMode(SCN_UPD_ALWAYS);
    SetVisible(0);
    SetBoxCollide(0);
    p = record;
    entryCount = 0;
    camZoneState = OBJMGR_ZONE_UNSET;
    for (i = 0; i < 20; i++)
        AddIdList((u16)PropU32(p, props[i]));
    manageBoxes.Load(PropU32(p, 4));
    keepUpdating = PropU32(p, 0) & 1;
}

/* 0x4da0f6 ObjectManager_AddIdList */
void ObjectManager::AddIdList(u16 id)
{
    ScnObject *p;
    u32 *list;
    u16 length;
    s32 i;
    if (!id)
        return;
    list = Scn_FindIdList(id, &length);
    for (i = 0; i < length; i++) {
        p = Scenaric_FindByRecord((void *)list[i]); /* cast kept: an id list holds record pointers as u32 words */
        if (p) {
            entries[entryCount].obj = p;
            entryCount++;
        }
    }
}

/* 0x4da187 ObjectManager_SetObjectsActive. Preserve the original zero AND mask when keepUpdating is set. */
/* BYTES(cast): the zero AND mask is kept, as the original */
void ObjectManager::SetObjectsActive(Box *camBox)
{
    u16 andMask;
    u16 bits;
    s32 i;
    u16 word;
    if (camBox) {
        andMask = 0xfffb;
        bits = 0;
        if (!keepUpdating)
            andMask &= (u16) ~(SCN_OF_NEVER_UPDATE | SCN_OF_ALWAYS_UPDATE);
        for (i = 0; i < entryCount; i++) {
            word = entries[i].obj->flags;
            word &= andMask;
            word |= bits;
            if (!keepUpdating)
                word |= entries[i].savedUpdateBits;
            entries[i].obj->flags = word;
        }
    } else {
        andMask = 0;
        bits = 4;
        if (!keepUpdating) {
            andMask |= (u16)~SCN_OF_ALWAYS_UPDATE;
            bits |= SCN_OF_NEVER_UPDATE;
        }
        for (i = 0; i < entryCount; i++) {
            word = entries[i].obj->flags;
            entries[i].savedUpdateBits = word & (SCN_OF_NEVER_UPDATE | SCN_OF_ALWAYS_UPDATE);
            word &= andMask;
            word |= bits;
            entries[i].obj->flags = word;
        }
    }
}

/* 0x4da2fa ObjectManager_Create */
ScnObject *ObjectManager_Create(void *record)
{
    ObjectManager *obj = new ObjectManager;
    obj = (ObjectManager *)obj->Init(record); /* cast kept: Init returns the object as a ScnObject * */
    return obj;
}
