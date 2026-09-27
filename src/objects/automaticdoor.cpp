/* PAL PC AutomaticDoor, 0x495160-0x495540. sensibleBoxes at +0x64 is a ZoneList. */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *, Animator *, u16, u32);

#define SDW_MEMBERS_ScnObject static void *operator new(u32);

#define SDW_MEMBERS_ZoneList    \
    s32 ContainsPoint(Vec3s *); \
    void Load(u32);
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
extern Wolf *g_pWolf;
ScnObject *s_pAutomaticDoorSam; /* T100 .bss 0x6cf448 */
s32 Scenaric_FindByClass(u16, ScnObject **, s32);
#include "../engine/id_list.h"
#include "../engine/scn_tools.h"
inline s32 ZoneList::ContainsPoint(Vec3s *point)
{
    return BoxList_FindContainingPoint(point, boxes, count) != 0;
}
#define SDW_INLINE_ZONELIST_LOAD_U32 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_LOAD_U32
#define SDW_INLINE_FREE_PROPERTYU32_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_PROPERTYU32_VOID_U32
void AutomaticDoor::Update()
{
    s32 occupied = sensibleBoxes.ContainsPoint(&g_pWolf->pos);
    occupied |= sensibleBoxes.ContainsPoint(&s_pAutomaticDoorSam->pos);
    switch (state) {
        case AUTODOOR_ST_CLOSED:
            if (occupied)
                SetState(AUTODOOR_ST_OPENING);
            break;
        case AUTODOOR_ST_OPEN:
            if (!occupied) {
                SetCollision(1);
                if (TestBodyAt(&pos, CQ_OBJECTS))
                    SetCollision(0);
                else
                    SetState(AUTODOOR_ST_CLOSING);
            }
            break;
        case AUTODOOR_ST_CLOSING:
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(AUTODOOR_ST_CLOSED);
            break;
        case AUTODOOR_ST_OPENING:
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(AUTODOOR_ST_OPEN);
            break;
    }
    AdvanceAnim();
}
s32 AutomaticDoor::HandleMessage(ScnObject *, u32, void *)
{
    return 0;
}
void AutomaticDoor::Reset() {}
void AutomaticDoor::SetState(s8 value)
{
    state = value;
    switch (state) {
        case AUTODOOR_ST_CLOSED:
            PlayAnim(AUTODOOR_ANIM_CLOSED, 0, 1);
            break;
        case AUTODOOR_ST_OPEN:
            PlayAnim(AUTODOOR_ANIM_OPEN, 0, 1);
            break;
        case AUTODOOR_ST_OPENING:
            PlayAnim(AUTODOOR_ANIM_OPENING, 0, 1);
            SetCollision(0);
            break;
        case AUTODOOR_ST_CLOSING:
            PlayAnim(AUTODOOR_ANIM_CLOSING, 0, 1);
            SetCollision(1);
            break;
    }
}
void AutomaticDoor::PostLoadInit()
{
    if (!Scenaric_FindByClass(CLASSID_SAM, &s_pAutomaticDoorSam, 1))
        s_pAutomaticDoorSam = 0;
    void *properties = record;
    sensibleBoxes.Load(PropertyU32(properties, 0));
    SetState(AUTODOOR_ST_CLOSED);
    SetCollision(1);
}
void AutomaticDoor::SetCollision(s32 enabled)
{
    if (enabled)
        EnableBoxes(COLLBOX_DOOR_SET);
    else
        DisableBoxes(COLLBOX_DOOR_SET);
}
ScnObject *AutomaticDoor_Create(void *record)
{
    AutomaticDoor *object = new AutomaticDoor;
    object = (AutomaticDoor *)object->Init(record, 0); /* cast kept: Init returns the ScnObject base of this door */
    return object;
}
