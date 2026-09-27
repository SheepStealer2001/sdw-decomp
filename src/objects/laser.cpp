/* PAL PC Laser, 0x4d04f0-0x4d100e. */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
class ScnObject;
class Instance;
struct Animator;
u32 Anim_Start(Instance *, Animator *, u16, u32);
#include "../engine/sound_mgr.h"
#include "../engine/scn_tools.h"

#define SDW_MEMBERS_ScnObject                                                                                   \
    static void *operator new(u32 size);                                                                        \
    void DisableBoxCollide()                                                                                    \
    {                                                                                                           \
        flags |= SCN_OF_NO_BOX_COLLIDE;                                                                         \
    }                                                                                                           \
    s32 ShouldUpdate()                                                                                          \
    {                                                                                                           \
        if (!(GetFlags() & SCN_OF_NEVER_UPDATE) && (camDist2 < 9000000 || (GetFlags() & SCN_OF_ALWAYS_UPDATE))) \
            return 1;                                                                                           \
        return 0;                                                                                               \
    }                                                                                                           \
    void SetUpdateMode(u8 mode);

#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_GETFLAGS 1
#define SDW_INLINE_SCNOBJECT_GETHEADING 1
#define SDW_INLINE_SCNOBJECT_SETVISIBLE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_GETFLAGS
#undef SDW_INLINE_SCNOBJECT_GETHEADING
#undef SDW_INLINE_SCNOBJECT_SETVISIBLE_S32
#define SDW_INLINE_SCNOBJECT_ISSOUNDPLAYING_U16 1
#define SDW_INLINE_SCNOBJECT_STOPSOUND_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_ISSOUNDPLAYING_U16
#undef SDW_INLINE_SCNOBJECT_STOPSOUND_U16
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
extern Wolf *g_pWolf;
CollBox g_laserWolfBox; /* T176 .bss 0x6cf678..0x6cf688 */
extern s32 g_gameTimeMs;
extern "C" s16 g_sinTable4096[5122];
extern "C" const s16 *g_pCosTable;
u16 Sound_Play(u16, void *, u16, u8, s32);
static inline u32 LaserProperty(void *record, u32 offset)
{
    /* cast kept: designer properties are 4-byte slots at byte offsets of the raw WAR record */
    return *(u32 *)((u8 *)record + offset + 0x14);
}
#define SDW_INLINE_FREE_BOXOVERLAP4_S32_S32_S32_S32 1
#include "../engine/coll_box_inlines.h"
#undef SDW_INLINE_FREE_BOXOVERLAP4_S32_S32_S32_S32
#define SDW_INLINE_FREE_BOXOVERLAP2_S32_S32 1
#include "../engine/coll_box_inlines.h"
#undef SDW_INLINE_FREE_BOXOVERLAP2_S32_S32

void Laser::PostLoadInit()
{
    u16 *props = record;
    flags = 0;
    soundHandle = 0;
    activationDelayMs = (u16)LaserProperty(props, 0);
    type = (u16)LaserProperty(props, 12);
    switch (type) {
        case LASER_TYPE_STAND4:
            animId = ALASER01_ANIM_STAND4;
            break;
        case LASER_TYPE_STAND6:
            animId = ALASER01_ANIM_STAND6;
            break;
        case LASER_TYPE_MIDDLE1:
            animId = ALASER01_ANIM_MIDDLE1;
            break;
        case LASER_TYPE_MIDDLE2:
            animId = ALASER01_ANIM_MIDDLE2;
            break;
        case LASER_TYPE_LEFT12:
            animId = ALASER01_ANIM_LEFT12;
            break;
        case LASER_TYPE_RIGHT12:
            animId = ALASER01_ANIM_RIGHT12;
            break;
        case LASER_TYPE_LEFT14:
            animId = ALASER01_ANIM_LEFT14;
            break;
        case LASER_TYPE_RIGHT14:
            animId = ALASER01_ANIM_RIGHT14;
            break;
        case LASER_TYPE_LEFT26:
            animId = ALASER01_ANIM_LEFT26;
            break;
        case LASER_TYPE_RIGHT26:
            animId = ALASER01_ANIM_RIGHT26;
            break;
    }
    if (LaserProperty(props, 8))
        flags |= LASER_F_START_OFF;
    beamBox = 0;
    /* cast kept: Box and CollBox are two views of one 16-byte zone record */
    beamBox = (CollBox *)Scn_GetPropBox(props, 4);
    DisableBoxCollide();
    normX = (g_sinTable4096[(s16)(GetHeading() & 0xfff)] * 600) >> 12;
    normZ = (g_pCosTable[(s16)(GetHeading() & 0xfff)] * 600) >> 12;
    Reset();
}
void Laser::Reset()
{
    flags |= LASER_F_SWITCH_ARMED | LASER_F_TOGGLE;
    SetState(LASER_ST_INIT);
    StopSound(soundHandle);
    soundHandle = 0;
}
void Laser::Update()
{
    switch (state) {
        case LASER_ST_ON:
            if (Vec3s_ManhattanDistXZ(&g_pWolf->pos, &pos) < 750) {
                if (!IsSoundPlaying(soundHandle) && !soundHandle)
                    soundHandle = Sound_Play(SND_LASER_HUM, this, 0x7f,
                                             SNDF_LOOP | SNDF_POSITIONAL | SNDF_DIST_HORIZONTAL, 0x1000);
                if (g_pWolf->HandleMessage(this, MSG_QUERY_SIZE, 0) != WOLF_SIZE_SMALL) {
                    g_laserWolfBox.Box_Translate(g_pWolf->GetFirstSolidBox(), &g_pWolf->pos);
                    if (BoxOverlap4(g_laserWolfBox.max.x - beamBox->min.x, beamBox->max.x - g_laserWolfBox.min.x,
                                    g_laserWolfBox.max.z - beamBox->min.z, beamBox->max.z - g_laserWolfBox.min.z) &&
                        BoxOverlap2(g_laserWolfBox.max.y - beamBox->min.y, beamBox->max.y - g_laserWolfBox.min.y)) {
                        if (!g_pWolf->HandleMessage(this, MSG_WOLF_IS_DEAD, 0))
                            /* cast kept: HandleMessage's arg is a void *; MSG_KILL passes the WolfKillType in it */
                            g_pWolf->HandleMessage(this, MSG_KILL, (void *)KILL_ZAP);
                    }
                }
            } else if (IsSoundPlaying(soundHandle) && soundHandle) {
                StopSound(soundHandle);
                soundHandle = 0;
            }
            AdvanceAnim();
            break;
        case LASER_ST_INIT:
            if (!(flags & LASER_F_START_OFF))
                SetState(LASER_ST_ON);
            else
                SetState(LASER_ST_OFF);
            break;
        case LASER_ST_ARMING:
            activationElapsedMs = (u16)(g_gameTimeMs - activationStartMs);
            if (activationElapsedMs >= activationDelayMs)
                SetState(LASER_ST_ON);
            break;
    }
}
s32 Laser::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    if (sender)
        switch (msgId) {
            case MSG_FREEZE:
                if (sender->GetClassId() == CLASSID_WOLF)
                    return 1;
                break;
            case MSG_SWITCH_ON:
                if (state != LASER_ST_INIT) {
                    flags &= ~LASER_F_SWITCH_ARMED;
                    if (flags & LASER_F_TOGGLE) {
                        if (!(flags & LASER_F_START_OFF) && state != LASER_ST_OFF)
                            SetState(LASER_ST_OFF);
                        else if ((flags & LASER_F_START_OFF) && state == LASER_ST_OFF)
                            SetState(LASER_ST_ARMING);
                    } else {
                        if (!(flags & LASER_F_START_OFF) && state == LASER_ST_OFF)
                            SetState(LASER_ST_ARMING);
                        else if ((flags & LASER_F_START_OFF) && state != LASER_ST_OFF)
                            SetState(LASER_ST_OFF);
                    }
                }
                break;
            case MSG_SWITCH_OFF:
                if (!(flags & LASER_F_SWITCH_ARMED)) {
                    flags |= LASER_F_SWITCH_ARMED;
                    if (flags & LASER_F_TOGGLE)
                        flags &= ~LASER_F_TOGGLE;
                    else
                        flags |= LASER_F_TOGGLE;
                }
                break;
        }
    return 0;
}
void Laser::SetState(u8 value)
{
    state = value;
    switch (value) {
        case LASER_ST_INIT:
            SetUpdateMode(SCN_UPD_NORMAL);
            beamOn = 0;
            break;
        case LASER_ST_ARMING:
            SetUpdateMode(SCN_UPD_ALWAYS);
            activationStartMs = g_gameTimeMs;
            break;
        case LASER_ST_ON:
            SetUpdateMode(SCN_UPD_NORMAL);
            SetVisible(1);
            PlayAnim(animId, 1, 0);
            beamOn = 1;
            break;
        case LASER_ST_OFF:
            SetUpdateMode(SCN_UPD_NEVER);
            SetVisible(0);
            beamOn = 0;
            StopSound(soundHandle);
            soundHandle = 0;
            break;
    }
}
ScnObject *Laser_Create(void *record)
{
    Laser *obj = new Laser;
    obj = (Laser *)obj->Init(record, 0); /* cast kept: Init returns the object as its base class */
    obj->flags |= LASER_F_TOGGLE;
    return obj;
}
void Laser::Render(Camera *view)
{
    if (ShouldUpdate())
        ScnBody::Render(view);
}
