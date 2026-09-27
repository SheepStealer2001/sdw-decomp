/* PAL PC Jail, 0x4cf2b0-0x4cff6f. */
/* BYTES: slot-group, view. */
/* BYTES(view): heightTop / lowerBox ... field macros over _pad bytes: field view over class padding until the struct CSV names these bytes */
#include "sdw_types.h"
class ScnObject;
class Instance;
struct Animator;
u32 Anim_Start(Instance *, Animator *, u16, u32);
#include "../engine/sound_mgr.h"
#include "../engine/scn_tools.h"

#define SDW_MEMBERS_ScnObject            \
    static void *operator new(u32 size); \
    void GetModelBoxes(CollBox **out, u32 *count);


#include "sdw_enums.h"
#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_ISSOUNDPLAYING_U16 1
#define SDW_INLINE_SCNOBJECT_STOPSOUND_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_ISSOUNDPLAYING_U16
#undef SDW_INLINE_SCNOBJECT_STOPSOUND_U16
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
extern s32 g_dtMs;
u16 Sound_Play(u16 id, void *owner, u16 volume, u8 flags, s32 rate);
s32 ObjGrid_QueryBoxOverlap(CollBox *box, ScnObject **out);
s32 Box_GroundQueryFlatTop(GroundQuery *query, CollBox *box, Vec3s *pos, s32 margin);
#define SDW_INLINE_SCNOBJECT_GETMODELBOXES_COLLBOX_U32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETMODELBOXES_COLLBOX_U32
static inline void SwapHeight(s16 *a, s16 *b)
{
    s16 temporary = *a;
    *a = *b;
    *b = temporary;
}

/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; unused fill gaps */
void Jail::PostLoadInit()
{
    /* Match the observed local region -0x28..-4; semantics of all slots known. */
    struct Work {
        Vec3s point;
        u16 unused;
        s32 sum;
        u32 count;
        Trajectory *path;
        CollBox *boxes;
        s32 minimum, maximum;
        u32 index;
        u16 *props;
    } w;
    w.count = 0;
    w.props = record;
    topSwitch = Scn_GetPropObject(w.props, 0);
    middleSwitch = Scn_GetPropObject(w.props, 8);
    bottomSwitch = Scn_GetPropObject(w.props, 4);
    w.path = Scn_GetPropTrajectory(w.props, 16);
    heightTop = w.path->pts[0].y;
    heightMiddle = w.path->pts[1].y;
    heightBottom = w.path->pts[2].y;
    if (heightTop > heightMiddle)
        SwapHeight(&heightTop, &heightMiddle);
    if (heightMiddle > heightBottom)
        SwapHeight(&heightMiddle, &heightBottom);
    if (heightTop > heightMiddle)
        SwapHeight(&heightTop, &heightMiddle);
    w.point.x = pos.x;
    w.point.y = pos.y;
    w.point.z = pos.z;
    w.point.y = heightTop;
    SetPosition(&w.point);
    GetModelBoxes(&w.boxes, &w.count);
    w.index = w.count;
    while (w.count) {
        w.count--;
        if (w.boxes[w.count].flags & COLLBOX_NONSOLID) {
            carryBox.min.x = w.boxes[w.count].min.x;
            carryBox.min.y = w.boxes[w.count].min.y;
            carryBox.min.z = w.boxes[w.count].min.z;
            carryBox.max.x = w.boxes[w.count].max.x;
            carryBox.max.y = w.boxes[w.count].max.y;
            carryBox.max.z = w.boxes[w.count].max.z;
            carryBox.min.x += pos.x;
            carryBox.min.y += pos.y;
            carryBox.min.z += pos.z;
            carryBox.min.y -= 200;
            carryBox.max.x += pos.x;
            carryBox.max.y += pos.y;
            carryBox.max.z += pos.z;
            w.count++;
            break;
        }
    }
    lowerBox = w.boxes;
    upperBox = w.boxes;
    w.maximum = lowerBox->min.y + lowerBox->max.y;
    w.minimum = w.maximum;
    while (w.index) {
        w.index--;
        w.sum = w.boxes[w.index].min.y + w.boxes[w.index].max.y;
        if (w.sum > w.maximum)
            lowerBox = &w.boxes[w.index];
        if (w.sum < w.minimum)
            upperBox = &w.boxes[w.index];
    }
    clearBox.min.x = carryBox.min.x;
    clearBox.min.y = carryBox.min.y;
    clearBox.min.z = carryBox.min.z;
    clearBox.max.x = carryBox.max.x;
    clearBox.max.y = carryBox.max.y;
    clearBox.max.z = carryBox.max.z;
    clearBox.min.y += (s16)(carryBox.max.y - carryBox.min.y);
    clearBox.max.y += (s16)(carryBox.max.y - carryBox.min.y);
    clearBox.min.y += 60;
    clearBox.max.y += 60;
    soundHandle = 0;
    targetVert = pos.y;
    open = 0;
    SetState(JAIL_ST_STAND);
}
s32 Jail::AtTargetHeight()
{
    if (previousPos.y <= targetVert + 10 && previousPos.y >= targetVert - 10)
        return 1;
    return 0;
}
/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets; unused40, unused00 fill gaps */
void Jail::Update()
{
    struct Work {
        Vec3s movement;
        u16 unused40;
        u32 count, index;
        ScnObject *objects[10];
        Vec3s velocity;
        u16 unused00;
    } w;
    previousPos = pos;
    switch (state) {
        case JAIL_ST_AT_HEIGHT:
            delay -= g_dtMs;
            if (!AtTargetHeight() && delay <= 0)
                SetState(JAIL_ST_MOVING);
            break;
        case JAIL_ST_MOVING:
            if (AtTargetHeight()) {
                if (IsSoundPlaying(soundHandle))
                    StopSound(soundHandle);
                SetState(JAIL_ST_AT_HEIGHT);
            } else {
                w.velocity.x = 0;
                w.velocity.z = 0;
                w.velocity.y = 0;
                if (!ObjGrid_QueryBoxOverlap(&clearBox, w.objects)) {
                    if (previousPos.y < targetVert)
                        w.velocity.y = 250;
                    else
                        w.velocity.y = -250;
                    if (!IsSoundPlaying(soundHandle))
                        soundHandle = Sound_Play(SND_JAIL_MOVE, this, 0xff, SNDF_LOOP | SNDF_POSITIONAL, 0x1000);
                } else if (IsSoundPlaying(soundHandle))
                    StopSound(soundHandle);
                Vec3s_ScaleByDt(&w.velocity, &w.movement);
                w.count = ObjGrid_QueryBoxOverlap(&carryBox, w.objects);
                for (w.index = 0; w.index < w.count; w.index++) {
                    if (w.objects[w.index] != this)
                        w.objects[w.index]->Translate(&w.movement);
                }
                carryBox.min.x += w.movement.x;
                carryBox.min.y += w.movement.y;
                carryBox.min.z += w.movement.z;
                carryBox.max.x += w.movement.x;
                carryBox.max.y += w.movement.y;
                carryBox.max.z += w.movement.z;
                clearBox.min.x += w.movement.x;
                clearBox.min.y += w.movement.y;
                clearBox.min.z += w.movement.z;
                clearBox.max.x += w.movement.x;
                clearBox.max.y += w.movement.y;
                clearBox.max.z += w.movement.z;
                Translate(&w.movement);
            }
            break;
        case JAIL_ST_OPEN:
        case JAIL_ST_STAND:
        case JAIL_ST_CLOSE:
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(JAIL_ST_AT_HEIGHT);
            break;
    }
    AdvanceAnim();
}
s32 Jail::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    switch (msgId) {
        case MSG_SWITCH_ON:
            if (sender == bottomSwitch) {
                targetVert = heightBottom;
                delay = 0;
            }
            if (sender == middleSwitch) {
                targetVert = heightMiddle;
                delay = 0;
            }
            if (sender == topSwitch) {
                targetVert = heightTop;
                delay = 0x400;
            }
            break;
        case MSG_JAIL_OPEN:
            if (!open)
                SetState(JAIL_ST_OPEN);
            delay = 0;
            targetVert = heightBottom;
            break;
        case MSG_JAIL_CLOSE:
            if (open)
                SetState(JAIL_ST_CLOSE);
            delay = 0x400;
            targetVert = heightTop;
            break;
        case MSG_GROUND_QUERY: {
            /* cast kept: the message arg is a void *; what it carries depends on the message */
            GroundQuery *query = (GroundQuery *)arg;
            if (query->pos.y <= upperBox->min.y + pos.y)
                return Box_GroundQueryFlatTop(query, upperBox, &pos, 0);
            return Box_GroundQueryFlatTop(query, lowerBox, &pos, 0);
        }
    }
    return 0;
}
void Jail::SetState(u8 value)
{
    switch (value) {
        case JAIL_ST_STAND:
            PlayAnim(ACAGE01_ANIM_STAND, 0, 0);
            break;
        case JAIL_ST_CLOSE:
            PlayAnim(ACAGE01_ANIM_CLOSE, 0, 0);
            EnableBoxes(COLLBOX_ALT_SET);
            open = 0;
            break;
        case JAIL_ST_MOVING:
            soundHandle = Sound_Play(SND_JAIL_MOVE, this, 0xff, SNDF_LOOP | SNDF_POSITIONAL, 0x1000);
            break;
        case JAIL_ST_OPEN:
            PlayAnim(ACAGE01_ANIM_OPEN, 0, 0);
            DisableBoxes(COLLBOX_ALT_SET);
            open = 1;
            break;
    }
    state = value;
}
void Jail::Reset()
{
    Vec3s point = pos;
    point.y = heightTop;
    targetVert = heightTop;
    SetPosition(&point);
    SetState(JAIL_ST_AT_HEIGHT);
}
ScnObject *Jail_Create(void *record)
{
    ScnBody *obj = new Jail;
    obj = obj->Init(record, 0);
    return obj;
}
