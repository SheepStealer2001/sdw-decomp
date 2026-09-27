/* PAL PC FogManager, 0x4c2760-0x4c2978. Full inline update-mode switch retained. */
/* BYTES: slot-group, temp. */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
#define SDW_MEMBERS_ScnObject            \
    static void *operator new(u32 size); \
    void SetUpdateMode(s32 mode);
#define SDW_MEMBERS_Frustrum \
    u32 GetFogColor()        \
    {                        \
        return fogColor;     \
    }
#include "sdw_classes.h"
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32
extern u32 g_fogPauseColor, g_waterColor;
#include "../engine/draw2d.h"
#include "timemachine.h"
/* BYTES(temp, inferred): the offset and the result go through the read struct because the original's expansion keeps both in stack slots */
#define SDW_INLINE_FREE_PROPU32_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_PROPU32_VOID_U32
/* BYTES(slot-group): locals grouped in work only to pin the original frame offsets; unknown04 fill gaps */
void FogManager::PostLoadInit()
{
    /* Original locals occupy -0x14..-4, with only props (-8) and color
       (-0x10) accessed. Unknown slots preserve observed storage, not inferred
       semantics; they are deliberately neither initialized nor read. */
    struct Work {
        u32 unknown14, color, unknown0c;
        void *props;
        u32 unknown04;
    } work;
    work.props = record;
    g_fogPauseColor = PropU32(work.props, 0x14);
    g_waterColor = PropU32(work.props, 0x18);
    work.color = PropU32(work.props, 4);
    SetUpdateMode(SCN_UPD_NEVER);
    levelFogColor = g_pViewFrustum->GetFogColor();
    fog2Color = work.color;
    RemoveFromWorld();
}
s32 FogManager::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    switch (msgId) {
        case MSG_TIMEMACHINE_SWAP:
            if (sender->GetClassId() == CLASSID_TIMEMACHINESPHERE) {
                if (TimeMachine_IsInPresent(sender) == 0)
                    g_pViewFrustum->SetFogColor(levelFogColor);
                else
                    g_pViewFrustum->SetFogColor(fog2Color);
            }
            return 1;
    }
    return 0;
}
ScnObject *FogManager_Create(void *record)
{
    FogManager *object = new FogManager;
    object = (FogManager *)object->Init(record); /* cast kept: Init returns the ScnObject * base of this object */
    return object;
}
