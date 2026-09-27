/* PAL PC Rocks, 0x4e5550-0x4e58dc. */
#include "sdw_types.h"
#include "sdw_enums.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *inst, Animator *animator, u16 id, u32 opts); /* 0x550196 */
#include "../engine/scn_tools.h"
#include "../engine/scenaric.h"
#define SDW_INLINE_FREE_INSTFLAGS_SET_U16_U16 1
#include "instance_inlines.h"
#undef SDW_INLINE_FREE_INSTFLAGS_SET_U16_U16
#define SDW_INLINE_FREE_INSTFLAGS_CLEAR_U16_U16 1
#include "instance_inlines.h"
#undef SDW_INLINE_FREE_INSTFLAGS_CLEAR_U16_U16

#define SDW_MEMBERS_ScnObject            \
    static void *operator new(u32 size); \
    void SetUpdateMode(u8 mode);         \
    void SetTint(u32 color, s16 amount, s32 on);

#define SDW_MEMBERS_ZoneList Box *Find(Vec3s *point);
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8
#define SDW_INLINE_SCNOBJECT_SETTINT_U32_S16_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETTINT_U32_S16_S32
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#define SDW_INLINE_ZONELIST_FIND_VEC3S 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_FIND_VEC3S

#define SDW_INLINE_FREE_ZONE_GETLIST_U8 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_FREE_ZONE_GETLIST_U8

/* 0x4e5550 */
void Rocks::PostLoadInit()
{
    state = ROCKS_ST_INTACT;
    PlayAnim(AROCHE06_ANIM_STAND1, 0, 0);
    if (Zone_GetList(ZONE_SHADOW)->Find(&pos))
        SetTint(0, 0xc00, 1);
}

/* 0x4e5632 */
void Rocks::Update()
{
    switch (state) {
        case ROCKS_ST_HIT:
            state = ROCKS_ST_CRUMBLING;
            PlayAnim(AROCHE06_ANIM_BOOM01, 0, 0);
            break;
        case ROCKS_ST_CRUMBLING:
            if (AnimFlags(ANIM_F_FINISHED)) {
                SetUpdateMode(SCN_UPD_NORMAL);
                RemoveFromWorld();
            }
            break;
    }
    AdvanceAnim();
}

/* 0x4e5784 */
s32 Rocks::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    switch (msgId) {
        case MSG_KILL:
            if (!arg) {
                state = ROCKS_ST_HIT;
                SetUpdateMode(SCN_UPD_ALWAYS);
            }
            return 1;
    }
    return 0;
}

/* 0x4e5878 */
ScnObject *Rocks_Create(void *record)
{
    Rocks *obj = new Rocks;
    obj = (Rocks *)obj->Init(record, 0); /* cast kept: Init returns the ScnBody * base of this object */
    return obj;
}
