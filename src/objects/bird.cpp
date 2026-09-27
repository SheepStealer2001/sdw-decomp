/* PAL PC Bird, 0x499e30-0x49ad4d. */
#include "sdw_types.h"
#include "sdw_enums.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *, Animator *, u16, u32);

#define SDW_MEMBERS_ScnObject       \
    static void *operator new(u32); \
    void SetUpdateMode(s32 mode);

#define SDW_MEMBERS_ZoneList void Load(u32);
#define SDW_MEMBERS_CollBox s32 ContainsXZ(Vec3s *point);
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETHEADING 1
#define SDW_INLINE_SCNOBJECT_SETHEADING_S16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETHEADING
#undef SDW_INLINE_SCNOBJECT_SETHEADING_S16
#define SDW_INLINE_SCNOBJECT_INSTFLAGS_U16 1
#define SDW_INLINE_SCNOBJECT_SETVISIBLE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_INSTFLAGS_U16
#undef SDW_INLINE_SCNOBJECT_SETVISIBLE_S32
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
#define SDW_INLINE_COLLBOX_CONTAINSXZ_VEC3S 1
#include "../engine/coll_box_inlines.h"
#undef SDW_INLINE_COLLBOX_CONTAINSXZ_VEC3S
extern Wolf *g_pWolf;
extern s32 g_dt, g_dtMs;
s32 Rand_Bounded(s32);
#include "../engine/id_list.h"
#include "../engine/scn_tools.h"
#include "world_draw.h"
#include "../engine/fixed_math.h"
#define SDW_INLINE_ZONELIST_LOAD_U32 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_LOAD_U32
#define SDW_INLINE_FREE_PROPERTYU32_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_PROPERTYU32_VOID_U32
#define ABS_VALUE(v) ((v) >= 0 ? (v) : -(v))
void Bird::PostLoadInit()
{
    Vec3s rotation;
    void *properties = record;
    rotation = rot;
    rotation.x += 0x400;
    rotation.y += 0x800;
    rot = rotation;
    appearBoxes.Load(PropertyU32(properties, 0));
    SetVisible(0);
    SetState(BIRD_ST_LEAVE);
    SetUpdateMode(SCN_UPD_ALWAYS);
    SetUpdateMode(SCN_UPD_CINE);
}
void Bird::Reset()
{
    SetState(BIRD_ST_LEAVE);
}
void Bird::Update()
{
    Vec3s translation;
    Mat34s matrix;
    Vec4i transformedOrbit;
    Vec3s newPos;
    switch (state) {
        case BIRD_ST_APPEAR_WAIT:
            if (offscreenTicks >= 2)
                SetState(BIRD_ST_CIRCLE);
            if (!wasOnScreen)
                offscreenTicks++;
            break;
        case BIRD_ST_CIRCLE:
            orbitRot.y += (s16)(orbitAngSpeed * g_dt / 4096);
            if (ABS_VALUE(orbitRot.y) >= 0x3ff) {
                if (Rand_Bounded(2)) {
                    reverseNext = Rand_Bounded(2);
                    SetState(BIRD_ST_CIRCLE);
                    break;
                } else {
                    takeoffPos = pos;
                    SetState(BIRD_ST_TAKEOFF);
                    break;
                }
            }
            transformedOrbit.x = orbitRadius.x;
            transformedOrbit.y = orbitRadius.y;
            transformedOrbit.z = orbitRadius.z;
            Mat34s_FromEulerScaled(&orbitRot, &matrix, 0);
            /* cast kept: transformedOrbit is the Vec3i input and the Vec4i output of the in-place transform */
            Mat34s_TransformVec3i(&matrix, (Vec3i *)&transformedOrbit, &transformedOrbit);
            newPos.x = orbitCentre.x + (s16)transformedOrbit.x;
            newPos.y = pos.y;
            newPos.z = orbitCentre.z + (s16)transformedOrbit.z;
            SetHeading(GetHeading() + orbitAngSpeed * g_dt / 4096);
            SetPosition(&newPos);
            break;
        case BIRD_ST_TAKEOFF:
            flightTimerMs -= (s16)g_dtMs;
            if (flightTimerMs <= 0)
                SetState(BIRD_ST_DESCEND);
            Translate(&flightStep);
            break;
        case BIRD_ST_DESCEND:
            flightTimerMs -= (s16)g_dtMs;
            if (flightTimerMs <= 0) {
                translation.x = pos.x - takeoffPos.x;
                translation.y = pos.y - takeoffPos.y;
                translation.z = pos.z - takeoffPos.z;
                orbitCentre.x += translation.x;
                orbitCentre.y += translation.y;
                orbitCentre.z += translation.z;
                reverseNext = Rand_Bounded(2);
                SetState(BIRD_ST_CIRCLE);
            }
            Translate(&flightStep);
            break;
        case BIRD_ST_LEAVE:
            if (AnimFlags(ANIM_F_FINISHED))
                for (boxIndex = 0; boxIndex < appearBoxes.count; boxIndex++) {
                    /* cast kept: Box and CollBox are two views of one 16-byte zone record */
                    if (((CollBox *)appearBoxes.boxes[boxIndex])->ContainsXZ(&g_pWolf->pos)) {
                        appearPos.x = appearBoxes.boxes[boxIndex]->max[0] + appearBoxes.boxes[boxIndex]->min[0];
                        appearPos.y = appearBoxes.boxes[boxIndex]->max[1] + appearBoxes.boxes[boxIndex]->min[1];
                        appearPos.z = appearBoxes.boxes[boxIndex]->max[2] + appearBoxes.boxes[boxIndex]->min[2];
                        appearPos.x /= 2;
                        appearPos.y /= 2;
                        appearPos.z /= 2;
                        SetPosition(&appearPos);
                        SetVisible(1);
                        SetState(BIRD_ST_APPEAR_WAIT);
                        break;
                    }
                }
            break;
    }
    wasOnScreen = 0;
    AdvanceAnim();
}
s32 Bird::HandleMessage(ScnObject *, u32, void *)
{
    return 0;
}
void Bird::SetState(u8 value)
{
    state = value;
    switch (value) {
        case BIRD_ST_APPEAR_WAIT:
            offscreenTicks = 0;
            orbitCentre.x = appearPos.x;
            orbitCentre.y = appearPos.y;
            orbitCentre.z = appearPos.z;
            rot.y = 0x800;
            orbitCentre.x += 400;
            reverseNext = 0;
            orbitCentreVertCopy = orbitCentre.y;
            orbitDir = 1;
            break;
        case BIRD_ST_CIRCLE:
            /* cast kept: Box and CollBox are two views of one 16-byte zone record */
            if (!((CollBox *)appearBoxes.boxes[boxIndex])->ContainsXZ(&pos)) {
                SetState(BIRD_ST_LEAVE);
                break;
            }
            PlayAnim(AOISEA01_ANIM_FLY, 1, 1);
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
            orbitAngSpeed = orbitDir * 550;
            break;
        case BIRD_ST_TAKEOFF: {
            Vec3s velocity;
            s16 heading;
            /* cast kept: Box and CollBox are two views of one 16-byte zone record */
            if (!((CollBox *)appearBoxes.boxes[boxIndex])->ContainsXZ(&pos)) {
                SetState(BIRD_ST_LEAVE);
                break;
            }
            PlayAnim(AOISEA01_ANIM_SPEED, 1, 1);
            heading = GetHeading() & 0xfff;
            if (heading >= 0x200 && heading < 0x600) {
                velocity.x = -500;
                velocity.y = -100;
                velocity.z = 0;
            } else if (heading >= 0x600 && heading < 0xa00) {
                velocity.x = 0;
                velocity.y = -100;
                velocity.z = 500;
            } else if (heading >= 0xa00 && heading < 0xe00) {
                velocity.x = 500;
                velocity.y = -100;
                velocity.z = 0;
            } else {
                velocity.x = 0;
                velocity.y = -100;
                velocity.z = -500;
            }
            Vec3s_ScaleByDt(&velocity, &flightStep);
            flightTimerMs = 1000;
            break;
        }
        case BIRD_ST_DESCEND: {
            Vec3s velocity;
            s16 heading;
            /* cast kept: Box and CollBox are two views of one 16-byte zone record */
            if (!((CollBox *)appearBoxes.boxes[boxIndex])->ContainsXZ(&pos)) {
                SetState(BIRD_ST_LEAVE);
                break;
            }
            PlayAnim(AOISEA01_ANIM_SLOW, 1, 1);
            heading = GetHeading() & 0xfff;
            if (heading >= 0x200 && heading < 0x600) {
                velocity.x = -400;
                velocity.y = 66;
                velocity.z = 0;
            } else if (heading >= 0x600 && heading < 0xa00) {
                velocity.x = 0;
                velocity.y = 66;
                velocity.z = 400;
            } else if (heading >= 0xa00 && heading < 0xe00) {
                velocity.x = 400;
                velocity.y = 66;
                velocity.z = 0;
            } else {
                velocity.x = 0;
                velocity.y = 66;
                velocity.z = -400;
            }
            Vec3s_ScaleByDt(&velocity, &flightStep);
            flightTimerMs = 1500;
            break;
        }
        case BIRD_ST_LEAVE:
            PlayAnim(AOISEA01_ANIM_FLY2, 0, 1);
            break;
    }
}
void Bird::Render(Camera *view)
{
    wasOnScreen = InstFlags(INST_F_DRAWN);
    ScnBody::Render(view);
}
ScnObject *Bird_Create(void *record)
{
    Bird *object = new Bird;
    object = (Bird *)object->Init(record, 0); /* cast kept: Init returns the base class */
    return object;
}
