/* PAL PC 0x4b8f20-0x4b910d. */
#include "sdw_types.h"
#include "sdw_enums.h"
#define SDW_MEMBERS_ScnObject            \
    static void *operator new(u32 size); \
    void SetUpdateMode(s32 mode);
#include "sdw_classes.h"
#include "../engine/scn_tools.h"
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32

void ElasticTree::PostLoadInit()
{
    void *properties = record;
    Box *box = Scn_GetPropBox(properties, 4);
    camRestrict = Scn_GetPropObject(properties, 0);
    pullUpPoint = (Vec3s *)&box->min; /* cast kept: Box stores its min corner as s16[3], read here as a Vec3s */
    unk48 = 0;
}
void ElasticTree::Update()
{
    if (camRestrict)
        camRestrict->HandleMessage(this, MSG_CAMRESTRICT_ENABLE, 0);
    SetUpdateMode(SCN_UPD_NEVER);
}
s32 ElasticTree::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    void **out;
    switch (msgId) {
        case MSG_ELASTICTREE_GET_INFO:
            out = (void **)arg; /* cast kept: this message's arg is a two-pointer reply buffer */
            out[0] = pullUpPoint;
            out[1] = camRestrict;
            return 1;
    }
    return 0;
}
void ElasticTree::Reset() {}
ScnObject *ElasticTree_Create(void *record)
{
    ElasticTree *object = new ElasticTree;
    object = (ElasticTree *)object->Init(record); /* cast kept: Init returns the object it was called on */
    return object;
}
