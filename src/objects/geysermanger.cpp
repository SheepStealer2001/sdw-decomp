/* PAL PC GeyserManger (the original's spelling), 0x4c5920-0x4c5c8f. */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
#define SDW_MEMBERS_ScnObject       \
    static void *operator new(u32); \
    void SetUpdateMode(s32 mode);
#define SDW_MEMBERS_CollBox s32 ContainsXZ(Vec3s *point);
#include "sdw_classes.h"
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_SETVISIBLE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_SETVISIBLE_S32
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32
#define SDW_INLINE_COLLBOX_CONTAINSXZ_VEC3S 1
#include "../engine/coll_box_inlines.h"
#undef SDW_INLINE_COLLBOX_CONTAINSXZ_VEC3S
extern Wolf *g_pWolf;
#include "../engine/scn_tools.h"
void GeyserManger::PostLoadInit()
{
    void *properties = record;
    SetUpdateMode(SCN_UPD_ALWAYS);
    SetVisible(0);
    boxCount = 0;
    geyserCount = 0;
    /* cast kept: an id list holds record pointers of any kind; this one lists the detection boxes */
    boxes = (CollBox **)Scn_GetPropIdList(properties, 0, &boxCount);
    u32 offset = 4;
    do {
        offset += 4;
        geysers[geyserCount] = Scn_GetPropObject(properties, offset);
        if (geysers[geyserCount] && (geysers[geyserCount]->GetClassId() == CLASSID_GEYSERIN ||
                                     geysers[geyserCount]->GetClassId() == CLASSID_GEYSEROUT))
            geyserCount++;
    } while (offset < 0x44);
}
void GeyserManger::Update()
{
    u16 zoneIndex;
    Vec3s point = g_pWolf->pos;
    for (zoneIndex = 0; zoneIndex < boxCount; zoneIndex++) {
        if (boxes[zoneIndex]->ContainsXZ(&point)) {
            for (zoneIndex = 0; zoneIndex < geyserCount; zoneIndex++)
                /* cast kept: HandleMessage's arg is a void *; this message passes a flag in it */
                geysers[zoneIndex]->HandleMessage(this, MSG_GEYSER_ZONE_ACTIVE, (void *)1);
            return;
        }
    }
    for (zoneIndex = 0; zoneIndex < geyserCount; zoneIndex++)
        geysers[zoneIndex]->HandleMessage(this, MSG_GEYSER_ZONE_ACTIVE, 0);
}
ScnObject *GeyserManger_Create(void *record)
{
    GeyserManger *object = new GeyserManger;
    object = (GeyserManger *)object->Init(record); /* cast kept: Init returns this as a ScnObject * */
    return object;
}
