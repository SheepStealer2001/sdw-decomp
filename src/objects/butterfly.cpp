/* PAL PC Butterfly, 0x49f260-0x49fea8. */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *, Animator *, u16, u32);
#include "../engine/id_list.h"
#include "../engine/maths.h"
#include "world_draw.h"
#include "../engine/fixed_math.h"

#define SDW_MEMBERS_ScnObject       \
    static void *operator new(u32); \
    s16 GetYaw()                    \
    {                               \
        return rot.y;               \
    }                               \
    void SetYaw(s16 value)          \
    {                               \
        rot.y = value;              \
    }                               \
    void SetUpdateMode(s32 mode);

#define SDW_MEMBERS_ZoneList void Load(u16 id);
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_SETVISIBLE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETVISIBLE_S32
#define SDW_INLINE_SCNOBJECT_INSTFLAGS_U16 1
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_INSTFLAGS_U16
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_ZONELIST_LOAD_U16 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_LOAD_U16
extern Wolf *g_pWolf;
extern s32 g_dt;
s32 Rand_Bounded(s32);
#define SDW_INLINE_FREE_PROPU32_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_PROPU32_VOID_U32
#define SDW_INLINE_FREE_BOX_CONTAINSPOINTXZ_BOX_VEC3S 1
#include "../engine/coll_box_inlines.h"
#undef SDW_INLINE_FREE_BOX_CONTAINSPOINTXZ_BOX_VEC3S
void Butterfly::PostLoadInit()
{
    void *a = record;
    appearBoxes.Load((u16)PropU32(a, 4));
    variant = (s16)PropU32(a, 0);
    if (variant)
        SwapModel(&altModels[1]);
    else
        SwapModel(&altModels[0]);
    appearDelay = 0;
    wasOnScreen = 0;
    rot.y = 0x800;
    SetUpdateMode(SCN_UPD_ALWAYS);
    SetUpdateMode(SCN_UPD_CINE);
    SetState(BUTTERFLY_ST_LEAVE);
}
void Butterfly::Update()
{
    Vec3s a;
    Vec4i b;
    Mat34s c;
    switch (state) {
        case BUTTERFLY_ST_FLUTTER:
            orbitRot.y += (s16)(orbitAngSpeed * g_dt / 0x1000);
            if ((orbitRot.y >= 0 ? orbitRot.y : -orbitRot.y) >= 0x3ff) {
                reverseNext = Rand_Bounded(2);
                SetState(BUTTERFLY_ST_FLUTTER);
                break;
            }
            b.x = orbitRadius.x;
            b.y = orbitRadius.y;
            b.z = orbitRadius.z;
            Mat34s_FromEulerScaled(&orbitRot, &c, 0);
            Mat34s_TransformVec3i(&c, (Vec3i *)&b,
                                  &b); /* cast kept: the in-place transform reads the Vec4i's x, y, z */
            if ((targetVert - pos.y >= 0 ? targetVert - pos.y : -(targetVert - pos.y)) < 20)
                targetVert = (s16)Rand_Range(appearPos.y - 200, appearPos.y);
            a.x = orbitCentre.x + (s16)b.x;
            a.z = orbitCentre.z + (s16)b.z;
            SetYaw((s16)(GetYaw() + orbitAngSpeed * g_dt / 0x1000));
            vertDelta = targetVert - pos.y;
            if (vertDelta) {
                vertStep = (s16)(vertDelta * 80 / (vertDelta >= 0 ? vertDelta : -vertDelta));
                vertStep = (s16)(vertStep * g_dt / 0x1000);
            } else
                vertStep = 0;
            a.y = pos.y + vertStep;
            SetPosition(&a);
            break;
        case BUTTERFLY_ST_LEAVE:
            if (AnimFlags(ANIM_F_FINISHED)) {
                SetVisible(0);
                for (boxIndex = 0; boxIndex < appearBoxes.count; boxIndex++) {
                    if (Box_ContainsPointXZ(appearBoxes.boxes[boxIndex], &g_pWolf->pos)) {
                        appearPos.x = appearBoxes.boxes[boxIndex]->max[0] + appearBoxes.boxes[boxIndex]->min[0];
                        appearPos.y = appearBoxes.boxes[boxIndex]->max[1] + appearBoxes.boxes[boxIndex]->min[1];
                        appearPos.z = appearBoxes.boxes[boxIndex]->max[2] + appearBoxes.boxes[boxIndex]->min[2];
                        appearPos.x /= 2;
                        appearPos.y /= 2;
                        appearPos.z /= 2;
                        SetPosition(&appearPos);
                        orbitCentre.x = appearPos.x;
                        orbitCentre.y = appearPos.y;
                        orbitCentre.z = appearPos.z;
                        rot.y = 0x800;
                        orbitCentre.x += 200;
                        reverseNext = 0;
                        orbitDir = 1;
                        targetVert = orbitCentre.y;
                        if (appearDelay >= 3) {
                            SetVisible(1);
                            SetState(BUTTERFLY_ST_FLUTTER);
                            break;
                        } else if (!wasOnScreen)
                            appearDelay++;
                        break;
                    }
                }
            }
            break;
    }
    wasOnScreen = 0;
    AdvanceAnim();
}
s32 Butterfly::HandleMessage(ScnObject *, u32, void *)
{
    return 0;
}
void Butterfly::SetState(u8 next)
{
    state = next;
    switch (next) {
        case BUTTERFLY_ST_LEAVE:
            switch (variant) {
                case 0:
                    PlayAnim(APAPIL03_ANIM_FLY1, 0, 1);
                    break;
                case 1:
                    PlayAnim(APAPIL03_ANIM_FLY2, 0, 1);
                    break;
                case 2:
                    PlayAnim(APAPIL03_ANIM_FLY1A, 0, 1);
                    break;
            }
            break;
        case BUTTERFLY_ST_FLUTTER:
            if (!Box_ContainsPointXZ(appearBoxes.boxes[boxIndex], &pos)) {
                SetState(BUTTERFLY_ST_LEAVE);
                break;
            }
            switch (variant) {
                case 0:
                    PlayAnim(APAPIL03_ANIM_FLY, 1, 1);
                    break;
                case 1:
                    PlayAnim(APAPIL03_ANIM_FLY, 1, 1);
                    break;
                case 2:
                    PlayAnim(APAPIL03_ANIM_FLY1, 1, 1);
                    break;
            }
            if (reverseNext) {
                orbitCentre.x = pos.x - orbitCentre.x;
                orbitCentre.y = pos.y - orbitCentre.y;
                orbitCentre.z = pos.z - orbitCentre.z;
                orbitCentre.x = pos.x + orbitCentre.x;
                orbitCentre.y = pos.y + orbitCentre.y;
                orbitCentre.z = pos.z + orbitCentre.z;
                reverseNext = 0;
                orbitDir *= -1;
            }
            orbitRadius.x = pos.x;
            orbitRadius.y = pos.y;
            orbitRadius.z = pos.z;
            orbitRadius.x -= orbitCentre.x;
            orbitRadius.y -= orbitCentre.y;
            orbitRadius.z -= orbitCentre.z;
            orbitRot.x = 0;
            orbitRot.y = 0;
            orbitRot.z = 0;
            orbitAngSpeed = orbitDir * 0x226;
            appearDelay = 0;
            break;
    }
}
void Butterfly::Render(Camera *view)
{
    wasOnScreen = InstFlags(INST_F_DRAWN);
    ScnBody::Render(view);
}
ScnObject *Butterfly_Create(void *record)
{
    u16 a[2];
    Butterfly *b = new Butterfly;
    a[0] = WAR_IDO_APAPIL02;
    a[1] = WAR_IDO_APAPIL03;
    /* cast kept: InitWithAltModels returns the object as its ScnObject base */
    b = (Butterfly *)b->InitWithAltModels(record, &b->mainModel, 2, a, b->altModels);
    return b;
}
