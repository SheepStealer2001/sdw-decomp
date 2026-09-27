/* PAL PC Bat, 0x495900-0x496188. Local identifiers are not recovered. */
#include "sdw_types.h"
#include "sdw_enums.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *, Animator *, u16, u32);
#include "../engine/sound_mgr.h"
#include "../engine/maths.h"
#include "../engine/scn_tools.h"
#include "world_draw.h"
#include "../engine/fixed_math.h"

#define SDW_MEMBERS_ScnObject static void *operator new(u32);


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
#define SDW_INLINE_SCNOBJECT_STOPSOUND_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_INSTFLAGS_U16
#undef SDW_INLINE_SCNOBJECT_STOPSOUND_U16
#define SDW_INLINE_SCNBODY_ANIMFLAGS_U16 1
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_ANIMFLAGS_U16
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
extern Wolf *g_pWolf;
extern s32 g_dt, g_dtMs;
s32 Rand_Bounded(s32);
u16 Sound_Play(u16, void *, u16, u8, s32);
s32 Vec3s_DistXZ(Vec3s *, Vec3s *);
s32 Vec3s_Dist(Vec3s *, Vec3s *);
#include "../sdk/crt.h"
extern "C" s16 Math_RadiansToAngle4096(float);
void Bat::PostLoadInit()
{
    homePos = pos;
    hoverHeight = (s16)Rand_Range(0x80, 0xd4);
    wasDrawnLastRender = 0;
    soundHandle = 0;
    SetState(BAT_ST_PERCHED);
}
void Bat::Update()
{
    Mat34s matrix;
    Vec4i transformedOrbit;
    s16 verticalDelta;
    Vec3s newPos, flightVelocity;
    u32 distance;
    Vec3s frameDelta;
    targetPos = g_pWolf->pos;
    targetPos.y -= hoverHeight;
    switch (state) {
        case BAT_ST_PERCHED:
            distance = Vec3s_DistSq(&pos, &targetPos);
            if (distance < 640000) {
                if (takeoffFrames >= 2)
                    SetState(BAT_ST_APPROACH);
                if (!wasDrawnLastRender)
                    takeoffFrames++;
            } else
                takeoffFrames = 0;
            break;
        case BAT_ST_APPROACH:
            distance = Vec3s_DistXZ(&pos, &targetPos);
            if (distance < (u32)approachRadius) {
                SetState(BAT_ST_CIRCLE);
                break;
            }
            if (distance > 1000) {
                SetState(BAT_ST_RETURN);
                break;
            }
            toTarget.x = targetPos.x - pos.x;
            toTarget.y = targetPos.y - pos.y;
            toTarget.z = targetPos.z - pos.z;
            SetHeading((Math_RadiansToAngle4096((float)atan2((double)toTarget.x, (double)toTarget.z)) + 0x800) & 0xfff);
            flightVelocity.x = toTarget.x * 350 / (s32)distance;
            flightVelocity.y = toTarget.y * 350 / (s32)distance;
            flightVelocity.z = toTarget.z * 350 / (s32)distance;
            Vec3s_ScaleByDt(&flightVelocity, &frameDelta);
            Translate(&frameDelta);
            break;
        case BAT_ST_CIRCLE:
            distance = Vec3s_Dist(&pos, &targetPos);
            circleTimeMs -= g_dtMs;
            if (circleTimeMs < 0) {
                SetState(BAT_ST_RETURN);
                break;
            }
            orbitRot.y += (s16)(spinSpeed * g_dt / 4096);
            transformedOrbit.x = orbitOffset.x;
            transformedOrbit.y = orbitOffset.y;
            transformedOrbit.z = orbitOffset.z;
            Mat34s_FromEulerScaled(&orbitRot, &matrix, 0);
            /* cast kept: the input is the xyz of the Vec4i it writes back */
            Mat34s_TransformVec3i(&matrix, (Vec3i *)&transformedOrbit, &transformedOrbit);
            newPos.x = targetPos.x + (s16)transformedOrbit.x;
            newPos.z = targetPos.z + (s16)transformedOrbit.z;
            SetHeading(GetHeading() + spinSpeed * g_dt / 4096);
            verticalDelta = targetPos.y - pos.y;
            verticalDelta = verticalDelta * 350 / (s32)distance;
            verticalDelta = verticalDelta * g_dt / 4096;
            newPos.y = pos.y + verticalDelta;
            SetPosition(&newPos);
            hoverHeight = (s16)Rand_Range(0x55, 0xff);
            break;
        case BAT_ST_RETURN:
            if (AnimFlags(ANIM_F_FINISHED))
                SetState(BAT_ST_PERCHED);
            break;
    }
    wasDrawnLastRender = 0;
    AdvanceAnim();
}
s32 Bat::HandleMessage(ScnObject *, u32, void *)
{
    return 0;
}
void Bat::SetState(u8 value)
{
    s16 direction;
    if (Sound_IsPlaying(soundHandle))
        StopSound(soundHandle);
    state = value;
    switch (value) {
        case BAT_ST_PERCHED:
            PlayAnim(ACHAUV01_ANIM_STAND, 1, 1);
            SetPosition(&homePos);
            takeoffFrames = 0;
            break;
        case BAT_ST_APPROACH:
            if (!Sound_IsSampleIdPlaying(SND_BAT_FLAP))
                soundHandle = Sound_Play(SND_BAT_FLAP, this, 255, SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 4096);
            PlayAnim(ACHAUV01_ANIM_FLY, 1, 1);
            approachRadius = Rand_Range(0x5a, 0x96);
            break;
        case BAT_ST_CIRCLE:
            if (!Sound_IsSampleIdPlaying(SND_BAT_FLAP))
                soundHandle =
                    Sound_Play(SND_BAT_FLAP, this, 255, SNDF_LOOP | SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 4096);
            PlayAnim(ACHAUV01_ANIM_FLY, 1, 1);
            orbitOffset = pos;
            orbitOffset.x -= targetPos.x;
            orbitOffset.y -= targetPos.y;
            orbitOffset.z -= targetPos.z;
            orbitRot.x = 0;
            orbitRot.y = 0;
            orbitRot.z = 0;
            circleTimeMs = (s16)Rand_Range(0x1482, 0x222e);
            spinSpeed = (s16)Rand_Range(0x500, 0x780);
            Rand_Bounded(2) ? direction = -1 : direction = 1;
            spinSpeed *= direction;
            SetHeading(GetHeading() + (s16)(direction * 0x800 / 2));
            unk68 = 0;
            break;
        case BAT_ST_RETURN:
            PlayAnim(ACHAUV01_ANIM_FLY2, 0, 1);
            break;
    }
}
void Bat::Render(Camera *view)
{
    wasDrawnLastRender = InstFlags(INST_F_DRAWN);
    if (state)
        ScnBody::Render(view);
}
ScnObject *Bat_Create(void *record)
{
    ScnBody *object = new Bat;
    object = object->Init(record, 0);
    return object;
}
