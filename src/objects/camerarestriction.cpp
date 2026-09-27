/* PAL PC CameraRestriction, 0x4a0dc0-0x4a1666. */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
#include "../engine/id_list.h"
#include "../engine/scn_tools.h"
#include "camera.h"
#define SDW_MEMBERS_ScnObject       \
    static void *operator new(u32); \
    void SetUpdateMode(s32 mode);
#define SDW_MEMBERS_ZoneList \
    void Load(u16 id);       \
    Box *Find(Vec3s *point);
#include "sdw_classes.h"
#define SDW_INLINE_SCNOBJECT_SETVISIBLE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETVISIBLE_S32
#define SDW_INLINE_SCNOBJECT_SETMOVEMENTENABLED_S32 1
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETMOVEMENTENABLED_S32
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32
#define SDW_INLINE_ZONELIST_LOAD_U16 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_LOAD_U16
#define SDW_INLINE_ZONELIST_CLEAR 1
#define SDW_INLINE_ZONELIST_FIND_VEC3S 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_CLEAR
#undef SDW_INLINE_ZONELIST_FIND_VEC3S
extern Wolf *g_pWolf;
s32 Scenaric_FindByClass(u16, ScnObject **, s32);
inline s32 PropS32(void *record, s32 offset)
{
    s32 field = offset;
    /* cast kept: a designer property is a 4-byte slot at a byte offset of the raw WAR record */
    s32 number = *(s32 *)((u8 *)record + field + 0x14);
    return number;
}
void CameraRestriction::Update()
{
    if (robot && robot->HandleMessage(this, MSG_ROBOT_IS_DRIVEN, 0)) {
        if (limits.flags.modeRobot && limits.influenceBoxes.Find(&robot->pos))
            g_camRestriction = &limits;
    } else {
        if (limits.flags.modeFollow && limits.influenceBoxes.Find(&g_pWolf->pos)) {
            g_camRestriction = &limits;
            if (limits.flags.notifyWolf)
                g_pWolf->HandleMessage(this, MSG_WOLF_NO_LOOK, 0);
        }
    }
}
s32 CameraRestriction::HandleMessage(ScnObject *, u32 message, void *arg)
{
    if (message == MSG_CAMRESTRICT_ENABLE) {
        switch ((s32)arg) { /* cast kept: the message arg is a void *; this message carries a number in it */
            case 0:
                SetUpdateMode(SCN_UPD_NEVER);
                break;
            case 1:
                SetUpdateMode(SCN_UPD_ALWAYS);
                break;
        }
        return 1;
    }
    return 0;
}
void CameraRestriction::ReadAngleRange(s16 *outMin, s16 *outMax, s32 minPropOff, s32 maxPropOff)
{
    s32 a, b;
    void *c;
    s16 d[2];
    c = record;
    /* cast kept: a designer property is a 4-byte slot at a byte offset of the raw WAR record */
    a = *(s32 *)((u8 *)c + minPropOff + 0x14);
    b = *(s32 *)((u8 *)c + maxPropOff + 0x14);
    d[0] = (s16)(((a << 11) / 180) & 0xfff);
    d[1] = (s16)(((b << 11) / 180) & 0xfff);
    if (d[0] == d[1] && a != b) {
        d[0] = 0;
        d[1] = 0x1000;
    }
    *outMin = d[0];
    *outMax = d[1];
}
void CameraRestriction::PostLoadInit()
{
    u16 a;
    u32 *b;
    void *c;
    u32 d;
    u16 e;
    SetVisible(0);
    SetMovementEnabled(0);
    robot = 0;
    Scenaric_FindByClass(CLASSID_ROBOT, &robot, 1);
    c = record;
    ReadAngleRange(&limits.pitchMin, &limits.pitchMax, 0x10, 0xc);
    ReadAngleRange(&limits.yawMin, &limits.yawMax, 0x20, 0x1c);
    ReadAngleRange(&limits.rollMin, &limits.rollMax, 0x3c, 0x38);
    limits.pitchSpeed = (PropS32(c, 0x14) << 11) / 180;
    limits.yawSpeed = (PropS32(c, 0x24) << 11) / 180;
    limits.rollSpeed = (PropS32(c, 0x40) << 11) / 180;
    limits.distMax = (s16)PropS32(c, 0x28);
    limits.distMin = (s16)PropS32(c, 0x2c);
    limits.focal = (s16)PropS32(c, 0x30);
    limits.eyeVertMax = (s16)-PropS32(c, 0x18);
    limits.aimOffset.x = (s16)PropS32(c, 0);
    limits.aimOffset.y = (s16)-PropS32(c, 8);
    limits.aimOffset.z = (s16)PropS32(c, 4);
    e = (u16)PropS32(c, 0x44);
    if (e)
        limits.influenceBoxes.Load(e);
    else
        limits.influenceBoxes.Clear();
    limits.trajectory = 0;
    e = (u16)PropS32(c, 0x54);
    if (e) {
        b = Scn_FindIdList(e, &a);
        if (b && a > 0) {
            limits.trajectory = (Trajectory *)*b; /* cast kept: an id list holds record pointers of any kind */
            if (!limits.trajectory->count)
                limits.trajectory = 0;
        }
    }
    if (PropS32(c, 0x50))
        limits.flags.snapYaw = 1;
    else
        limits.flags.snapYaw = 0;
    if (PropS32(c, 0x34))
        limits.flags.forbidPad = 1;
    else
        limits.flags.forbidPad = 0;
    if (PropS32(c, 0x48))
        limits.flags.mirrorYaw = 1;
    else
        limits.flags.mirrorYaw = 0;
    d = PropS32(c, 0x4c);
    if (d & CAMR_MISC_FOLLOW)
        limits.flags.modeFollow = 1;
    else
        limits.flags.modeFollow = 0;
    if (d & CAMR_MISC_ROBOT)
        limits.flags.modeRobot = 1;
    else
        limits.flags.modeRobot = 0;
    if (d & CAMR_MISC_RUN)
        limits.flags.modeRun = 1;
    else
        limits.flags.modeRun = 0;
    if (d & CAMR_MISC_NOTIFY_WOLF)
        limits.flags.notifyWolf = 1;
    else
        limits.flags.notifyWolf = 0;
}
ScnObject *CameraRestriction_Create(void *record)
{
    ScnLogic *object = new CameraRestriction;
    object = (ScnLogic *)object->Init(record); /* cast kept: Init returns the object as a ScnObject * */
    object->SetUpdateMode(SCN_UPD_ALWAYS);
    return object;
}
