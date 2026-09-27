/* T098 - original object AmbientSoundManager.cpp (guessed name).
 * Ranges: .text 0x494170-0x494de5, .rdata 0x575938-0x575980 (own vtable, then the ??_7ScnLogic COMDAT it emits first).
 * The functions are in the original address order (Update, PickRepeatDelay, HandleMessage, PostLoadInit, Create). */
/* BYTES: slot-group, view. */
/* BYTES(view): view: bitfield views keep the original partial-word writes and signed timer reads (the access widths are the original's) */
/* PAL PC AmbientSoundManager. Bitfield views keep the original
 * partial-word writes and signed timer reads without replacing class storage. */
#include "sdw_types.h"
#include "sdw_enums.h"
#define SDW_MEMBERS_ScnObject       \
    static void *operator new(u32); \
    void SetUpdateMode(s32 mode);

#include "sdw_classes.h"
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32 1
#define SDW_INLINE_SCNOBJECT_SETVISIBLE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32
#undef SDW_INLINE_SCNOBJECT_SETVISIBLE_S32
#define SDW_INLINE_SCNOBJECT_ENABLEBOXCOLLIDE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_ENABLEBOXCOLLIDE_S32

#include "../app/app_main.h"
#include "../engine/scn_tools.h"
#include "../engine/sound_mgr.h"
#define g_camPos (g_camera.pos)

extern s32 g_dtMs;
s32 Rand_Bounded(s32);
u16 Sound_Play(u16, void *, u16, u8, s32);
#define SDW_INLINE_FREE_SCN_GETPROPU32_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_SCN_GETPROPU32_VOID_U32
#define SDW_INLINE_FREE_SCN_GETPROPS32_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_SCN_GETPROPS32_VOID_U32
inline s32 AmbientDeltaMs()
{
    return g_dtMs;
}

/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets */
void AmbientSoundManager::Update()
{
    struct Work {
        u16 volumeHandle2, checkHandle2;
        u8 unused1, playFlags2;
        u16 playSound2, volumeHandle1;
        u8 unused2, playFlags1;
        u16 playSound1, checkHandle1;
        u32 distanceValue, distanceOffset, flagsValue2, flagsOffset2, flagsValue1, flagsOffset1;
        Box *box;
        float floatVolume2, floatVolume1;
        s32 weightX, weightY, weightZ, unused3, outputVolume, inRange;
        s16 unused4, maxDist;
        s32 distance;
        void *properties;
    } w;
    w.properties = record;
    w.inRange = 1;
    w.outputVolume = volume;
    if (soundBox) {
        w.box = soundBox;
        w.inRange = g_camPos.x >= w.box->min[0] && g_camPos.x <= w.box->max[0] && g_camPos.y >= w.box->min[1] &&
                    g_camPos.y <= w.box->max[1] && g_camPos.z >= w.box->min[2] && g_camPos.z <= w.box->max[2];
        if (w.inRange) {
            if (maxVolBox) {
#define WEIGHT(axis, index, destination)                                                                           \
    if (g_camPos.axis > maxVolBox->max[index])                                                                     \
        destination =                                                                                              \
            256 - ((g_camPos.axis - maxVolBox->max[index]) << 8) / (soundBox->max[index] - maxVolBox->max[index]); \
    else if (g_camPos.axis < maxVolBox->min[index])                                                                \
        destination =                                                                                              \
            256 - ((maxVolBox->min[index] - g_camPos.axis) << 8) / (maxVolBox->min[index] - soundBox->min[index]); \
    else                                                                                                           \
        destination = 256;
                WEIGHT(x, 0, w.weightX) WEIGHT(z, 2, w.weightZ) if (w.weightX < 0) w.weightX = 0;
                if (w.weightZ < 0)
                    w.weightZ = 0;
                w.floatVolume1 = (float)volume;
                w.floatVolume1 = w.weightX * w.floatVolume1 / 256.0f;
                w.floatVolume1 = w.weightZ * w.floatVolume1 / 256.0f;
                w.flagsOffset1 = 0;
                /* cast kept: a designer-property read (4-byte slot of the raw record), spelled out for its temporaries */
                w.flagsValue1 = *(u32 *)((u8 *)w.properties + w.flagsOffset1 + 0x14);
                if (!(w.flagsValue1 & AMBS_POSITIONAL_HORIZONTAL)) {
                    WEIGHT(y, 1, w.weightY) if (w.weightY < 0) w.weightY = 0;
                    w.floatVolume1 = w.weightY * w.floatVolume1 / 256.0f;
                }
                w.outputVolume = (s32)w.floatVolume1;
#undef WEIGHT
            } else {
#define WEIGHT(axis, index, destination)                                                           \
    if (g_camPos.axis > pos.axis)                                                                  \
        destination = 256 - ((g_camPos.axis - pos.axis) << 8) / (soundBox->max[index] - pos.axis); \
    else if (g_camPos.axis < pos.axis)                                                             \
        destination = 256 - ((pos.axis - g_camPos.axis) << 8) / (pos.axis - soundBox->min[index]); \
    else                                                                                           \
        destination = 256;
                WEIGHT(x, 0, w.weightX) WEIGHT(z, 2, w.weightZ) if (w.weightX < 0) w.weightX = 0;
                if (w.weightZ < 0)
                    w.weightZ = 0;
                w.floatVolume2 = (float)volume;
                w.floatVolume2 = w.weightX * w.floatVolume2 / 256.0f;
                w.floatVolume2 = w.weightZ * w.floatVolume2 / 256.0f;
                w.flagsOffset2 = 0;
                /* cast kept: a designer-property read (4-byte slot of the raw record), spelled out for its temporaries */
                w.flagsValue2 = *(u32 *)((u8 *)w.properties + w.flagsOffset2 + 0x14);
                if (!(w.flagsValue2 & AMBS_POSITIONAL_HORIZONTAL)) {
                    WEIGHT(y, 1, w.weightY) if (w.weightY < 0) w.weightY = 0;
                    w.floatVolume2 = w.weightY * w.floatVolume2 / 256.0f;
                }
                w.outputVolume = (s32)w.floatVolume2;
#undef WEIGHT
            }
        } else
            w.outputVolume = 0;
    } else {
        w.distanceOffset = 4;
        /* cast kept: a designer-property read (4-byte slot of the raw record), spelled out for its temporaries */
        w.distanceValue = *(u32 *)((u8 *)w.properties + w.distanceOffset + 0x14);
        w.maxDist = (s16)w.distanceValue;
        w.distance = Vec3s_ManhattanDistXZ(&g_camPos, &pos);
        w.inRange = w.distance < w.maxDist;
        if (linearFalloff) {
            w.outputVolume = volume - w.distance * volume / w.maxDist;
            if (w.outputVolume < 0)
                w.outputVolume = 0;
        }
    }
    if (w.inRange) {
        if (playFlags & SNDF_LOOP) {
            if (!(loopPollCounter++ & 1)) {
                w.checkHandle1 = soundHandle;
                if (!Sound_IsPlaying(w.checkHandle1)) {
                    w.playFlags1 = playFlags;
                    w.playSound1 = soundId;
                    soundHandle = Sound_Play(w.playSound1, this, (u16)w.outputVolume, w.playFlags1, 4096);
                }
            }
            w.volumeHandle1 = soundHandle;
            Sound_SetVolume(w.volumeHandle1, (u16)w.outputVolume);
        } else {
            repeatTimerMs -= AmbientDeltaMs();
            if (repeatTimerMs < 0) {
                w.playFlags2 = playFlags;
                w.playSound2 = soundId;
                soundHandle = Sound_Play(w.playSound2, this, (u16)w.outputVolume, w.playFlags2, 4096);
                PickRepeatDelay();
            } else if (soundHandle) {
                w.checkHandle2 = soundHandle;
                if (!Sound_IsPlaying(w.checkHandle2))
                    soundHandle = 0;
            }
            w.volumeHandle2 = soundHandle;
            Sound_SetVolume(w.volumeHandle2, (u16)w.outputVolume);
        }
    } else if (soundHandle) {
        u16 stopHandle = soundHandle;
        Sound_Stop(stopHandle, this);
        soundHandle = 0;
    }
}

/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets */
void AmbientSoundManager::PickRepeatDelay()
{
    struct Work {
        s32 high;
        void *properties;
        s32 low;
    } w;
    w.properties = record;
    w.low = Scn_GetPropS32(w.properties, 12);
    w.high = Scn_GetPropS32(w.properties, 8);
    if (w.low > w.high)
        w.low = w.high;
    repeatTimerMs = (s16)(w.low + Rand_Bounded(w.high - w.low + 1));
}
s32 AmbientSoundManager::HandleMessage(ScnObject *, u32, void *)
{
    return 0;
}
void AmbientSoundManager::PostLoadInit()
{
    u32 propertiesFlags;
    void *properties = record;
    SetUpdateMode(SCN_UPD_ALWAYS);
    SetVisible(0);
    EnableBoxCollide(0);
    soundId = (u16)Scn_GetPropU32(properties, 20);
    volume = (u16)Scn_GetPropU32(properties, 28);
    propertiesFlags = Scn_GetPropU32(properties, 0);
    soundBox = Scn_GetPropBox(properties, 16);
    maxVolBox = Scn_GetPropBox(properties, 24);
    playFlags = SNDF_NO_RETRIGGER;
    soundHandle = 0;
    if (propertiesFlags & AMBS_CINE_UPDATE)
        SetUpdateMode(SCN_UPD_CINE);
    if (propertiesFlags & AMBS_POSITIONAL)
        playFlags |= SNDF_POSITIONAL;
    else if (propertiesFlags & AMBS_POSITIONAL_HORIZONTAL)
        playFlags |= SNDF_POSITIONAL | SNDF_DIST_HORIZONTAL;
    if (propertiesFlags & AMBS_LOOP)
        playFlags |= SNDF_LOOP;
    if (!(playFlags & SNDF_LOOP))
        PickRepeatDelay();
    linearFalloff = (propertiesFlags & AMBS_LINEAR_FALLOFF) != 0;
    if (linearFalloff || soundBox)
        playFlags |= SNDF_NO_ATTENUATION;
}
ScnObject *AmbientSoundManager_Create(void *record)
{
    AmbientSoundManager *object = new AmbientSoundManager;
    object = (AmbientSoundManager *)object->Init(record); /* cast kept: Init returns the object as its ScnObject base */
    return object;
}
