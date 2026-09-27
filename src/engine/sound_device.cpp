/*
 * Object T004 (data/tu_map.json), guessed original file SoundDevice.cpp.
 *   .text 0x4065a0-0x406d7a, then the COMDAT ??_GSoundDevice 0x406d80-0x406dae
 *   .rdata 0x574304-0x574308 (the vtable)       .bss 0x5c7638-0x5c7a9c
 * .bss order: VC6 emits a file's UNINITIALISED globals (and globals with constructors) first, ordered by a hash of their
 * names (key = (h ^ h>>16) & 0x3ff ascending, h = h*4 + h>>4 + c, ties last-declared first), and after them the globals
 * explicitly initialised to zero, in definition order. Without initialisers the count (key 123) would come before the
 * table (key 461); the exe has the table first, so both are written `= 0` here.
 */
/* BYTES: dead-code, layout, slot-name. */
/* BYTES(layout): written '= 0' only to keep definition order in .bss */
/*
 * SoundDevice - the DirectSound device wrapper (one instance, g_pSoundSystem 0x6d5320), SheepD3D.exe 0x4065a0-0x406daf.
 * The original object ends at the int3 padding before 0x406d80; the compiler-generated scalar deleting destructor
 * 0x406d80 is emitted after it (as a COMDAT).
 *
 * The launcher picks a device from the list DirectSoundEnumerateA builds (DS_EnumCallback fills a 20-entry scratch table,
 * the constructor copies it); WinMain then calls Init(hwnd, 22050, 16): CoInitialize, DirectSoundCreate on that device,
 * DSSCL_PRIORITY, a primary buffer set to 16-bit stereo at the requested rate and left playing (looped) for the whole
 * session. The device also owns the 16 auto-reset events (CreateEventA(0, 0, 0, 0)) that the streamed sounds' position
 * notifications signal: Main_Loop calls PollStreamEvents first thing every frame, which services the one stream whose
 * event fired.
 *
 * The names are descriptive, not recovered (the binary has no symbols); local names were chosen for their stack slots
 * (src/README.md). COM calls are written as C++ interface calls on the pointer member: VC6 /Od evaluates `this->pDS`
 * twice for them (once for the vtable, once for the pushed `this`), which is exactly the original's shape.
 */
#define SDW_MEMBERS_SoundDevice SoundDevice(); /* 0x4065a0 SoundDevice_Construct */

#include "sdw_enums.h"
#include "../sdk/win32.h"
#include "../sdk/mmsystem.h"
#include "../sdk/dsound.h"
#include "../sdk/crt.h"

#include "sdw_classes.h"

/* ---- this file's statics ---- */
/* The scratch table DS_EnumCallback fills, 0x38 bytes per device (char description[0x28]; GUID guid), and its count. */
SoundDeviceEntry g_enumSoundDevices[20] = {0}; /* 0x5c7638 */
u32 g_enumSoundDeviceCount = 0;                /* 0x5c7a98  compared unsigned (jb at 0x406d13) */

BOOL __stdcall DS_EnumCallback(GUID *guid, const char *description, const char *module, void *context);

/* 0x4065a0 - enumerates the DirectSound devices into `devices`, clears the COM pointers and the stream slots and creates
 * the 16 stream events (auto-reset, unsignalled). */
SoundDevice::SoundDevice()
{
    u32 i;

    initialized = 0;
    deviceCount = 0;
    deviceIndex = 0;
    g_enumSoundDeviceCount = 0;
    memset(g_enumSoundDevices, 0, sizeof(g_enumSoundDevices));
    DirectSoundEnumerateA(DS_EnumCallback, 0);
    deviceCount = g_enumSoundDeviceCount;
    memcpy(devices, g_enumSoundDevices, deviceCount * sizeof(SoundDeviceEntry));
    pDS = 0;
    pPrimaryBuffer = 0;
    memset(streamOwners, 0, sizeof(streamOwners));
    memset(streamEvents, 0, sizeof(streamEvents));
    streamCount = 0;
    for (i = 0; i < 16; i++)
        streamEvents[i] = CreateEventA(0, 0, 0, 0);
}

/* 0x4066b5 - stops and releases the primary buffer, releases DirectSound, CoUninitialize, closes the 16 events. */
SoundDevice::~SoundDevice()
{
    u32 i;

    if (pPrimaryBuffer) {
        pPrimaryBuffer->Stop();
        pPrimaryBuffer->Release();
    }
    if (pDS)
        pDS->Release();
    CoUninitialize();
    for (i = 0; i < 16; i++)
        CloseHandle(streamEvents[i]);
}

/* 0x406747 - the launcher's device combo: steps deviceIndex forward (forward == 1) or back. 2 when it ran off the end
 * (clamped to the last device), 1 when it ran off the start - which cannot happen: deviceIndex is unsigned, so stepping
 * back from 0 wraps it to 0xffffffff. */
u8 SoundDevice::StepDevice(u8 forward)
{
    u8 result;

    result = SEL_OK;
    if (forward == 1) {
        deviceIndex++;
        if (deviceIndex >= deviceCount) {
            deviceIndex = deviceCount - 1;
            result = SEL_CLAMPED_AT_END;
        }
    } else {
        deviceIndex--;
        if (deviceIndex < 0) {
            deviceIndex = 0;
            result = SEL_CLAMPED_AT_START;
        }
    }
    return result;
}

/* 0x4067dc - selects device `index`: 0, or 3 when there is no such device. */
u8 SoundDevice::SetDeviceIndex(u8 index)
{
    u8 result;

    result = SEL_OUT_OF_RANGE;
    if (index < deviceCount) {
        deviceIndex = index;
        result = SEL_OK;
    }
    return result;
}

/* 0x406812 - copies the selected device's description (unbounded strcpy). */
void SoundDevice::GetDeviceName(char *out)
{
    strcpy(out, devices[deviceIndex].description);
}

/* 0x40683f */
s32 SoundDevice::Stub0()
{
    return 0;
}

/* 0x40684c */
s32 SoundDevice::Stub1()
{
    return 0;
}

/* 0x406859 - opens the selected device and starts the primary buffer at rate Hz, `bits`-bit stereo PCM. Returns the
 * first failing HRESULT, else that of the final Play. */
/* BYTES(slot-name): declared in this order for the original slots: result -4, devGuid -8, dsbd -0x2c, format -0x40, dscaps -0xa0 */
HRESULT SoundDevice::Init(HWND hwnd, u32 rate, u8 bits)
{
    /* declared in this order for the original slots: result -4, devGuid -8, dsbd -0x2c, format -0x40, dscaps -0xa0 */
    DSCAPS dscaps;
    WAVEFORMATEX format;
    DSBUFFERDESC dsbd;
    GUID *devGuid;
    HRESULT result;

    sampleRate = rate;
    bitsPerSample = bits;
    devGuid = &devices[deviceIndex].guid;
    result = CoInitialize(0);
    if (result >= 0) {
        result = DirectSoundCreate(devGuid, &pDS, 0);
        if (result >= 0) {
            result = pDS->SetCooperativeLevel(hwnd, DSSCL_PRIORITY);
            if (result >= 0) {
                memset(&dsbd, 0, sizeof(dsbd));
                dsbd.dwSize = sizeof(dsbd);
                dsbd.dwFlags = DSBCAPS_PRIMARYBUFFER;
                dsbd.dwBufferBytes = 0;
                dsbd.lpwfxFormat = 0;
                result = pDS->CreateSoundBuffer(&dsbd, &pPrimaryBuffer, 0);
                if (result >= 0) {
                    memset(&format, 0, sizeof(format));
                    format.wFormatTag = WAVE_FORMAT_PCM;
                    format.nChannels = 2;
                    format.nSamplesPerSec = rate;
                    format.wBitsPerSample = bits;
                    format.nBlockAlign = format.wBitsPerSample / 8 * format.nChannels;
                    format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;
                    result = pPrimaryBuffer->SetFormat(&format);
                    if (result >= 0) {
                        dscaps.dwSize = sizeof(dscaps);
                        result = pDS->GetCaps(&dscaps);
                        if (result >= 0) {
                            hasHwStreamingMixer = dscaps.dwMaxHwMixingStreamingBuffers > 0 ? 1 : 0;
                            initialized = 1;
                        }
                    }
                    if (pPrimaryBuffer)
                        result = pPrimaryBuffer->Play(0, 0, DSBPLAY_LOOPING);
                }
            }
        }
    }
    return result;
}

/* 0x406a5b */
IDirectSound *SoundDevice::GetDSound()
{
    return pDS;
}

/* 0x406a6c - the location flag the secondary buffers are created with: LOCDEFER on a card with hardware streaming
 * mixers, else STATIC. */
u32 SoundDevice::GetBufferLocFlags()
{
    if (hasHwStreamingMixer == 1)
        return DSBCAPS_LOCDEFER;
    return DSBCAPS_STATIC;
}

/* 0x406a8f - the lip-sync level of the newest stream: streamOwners[streamCount - 1]. streamCount is a count, not the
 * top slot, so after an earlier slot has been freed this reads the wrong (or an empty) slot. */
u32 SoundDevice::GetVoiceAmplitude(s32 refresh)
{
    if (streamCount != 0 && streamOwners[streamCount - 1] != 0)
        return streamOwners[streamCount - 1]->GetVoiceAmplitude(refresh);
    return 0;
}

/* 0x406ade - byte-identical twin of 0x406a8f, unreferenced. */
u32 SoundDevice::GetVoiceAmplitude_Unused(s32 refresh)
{
    if (streamCount != 0 && streamOwners[streamCount - 1] != 0)
        return streamOwners[streamCount - 1]->GetVoiceAmplitude(refresh);
    return 0;
}

/* 0x406b2d - polls the 16 stream events without waiting (and returns on any input message); services the stream whose
 * event fired. 1 if an event fired, 0 otherwise. Only one stream is serviced per call. */
u8 SoundDevice::PollStreamEvents()
{
    DWORD signalled;

    signalled = MsgWaitForMultipleObjects(16, streamEvents, 0, 0, QS_ALLINPUT);
    signalled = signalled - WAIT_OBJECT_0;
    if (signalled < 16) {
        if (streamOwners[signalled])
            streamOwners[signalled]->ServiceNotify();
        return 1;
    }
    return 0;
}

/* 0x406b8b - gives `stream` the first free notification slot and hands back its event. S_OK, or E_FAIL when all 16 are
 * taken, stream is NULL, or stream already holds a slot (then *outEvent stays NULL). */
/* BYTES(slot-name): declared in this order for the original slots: added -1, already -2, slot -8, hr -0xc, junk -0x10 */
/* BYTES(dead-code): junk is set and never read, as in the original */
HRESULT SoundDevice::RegisterStreamEvent(void *stream, HANDLE *outEvent)
{
    /* declared in this order for the original slots: added -1, already -2, slot -8, hr -0xc, junk -0x10 (set, never read) */
    u32 slot;
    u8 already;
    u8 added;
    HRESULT hr;
    u32 junk;

    hr = E_FAIL;
    slot = 0;
    junk = 0;
    already = 0;
    added = 0;
    *outEvent = 0;
    if (stream) {
        do {
            if (streamOwners[slot] == 0) {
                hr = 0;
                /* cast kept: the callers (stream_sound.cpp) declare this method with a void * owner */
                streamOwners[slot] = (StreamSound *)stream;
                added = 1;
                streamCount++;
                *outEvent = streamEvents[slot];
            } else if (streamOwners[slot] == stream) {
                already = 1;
            }
            slot++;
        } while (slot < 16 && !added && !already);
    }
    return hr;
}

/* 0x406c5a - frees stream's slot; resets ALL 16 events (not just that slot's). Always 0. */
u32 SoundDevice::UnregisterStreamEvent(void *stream)
{
    u32 slot;
    u8 found;

    slot = 0;
    found = 0;
    do {
        if (streamOwners[slot] == stream) {
            u32 e;
            streamOwners[slot] = 0;
            for (e = 0; e < 16; e++)
                ResetEvent(streamEvents[e]);
            found = 1;
            streamCount--;
        }
        slot++;
    } while (slot < 16 && !found);
    return 0;
}

/* 0x406cfb - DirectSoundEnumerateA callback: appends {description, GUID} to g_enumSoundDevices (a NULL guid - the
 * primary device - leaves the GUID zero). Always continues; beyond 20 devices it just stops adding. */
BOOL __stdcall DS_EnumCallback(GUID *guid, const char *description, const char *module, void *context)
{
    GUID *dst;

    dst = 0;
    if (guid) {
        if (g_enumSoundDeviceCount >= 20)
            return 1;
        dst = &g_enumSoundDevices[g_enumSoundDeviceCount].guid;
        memcpy(dst, guid, sizeof(GUID));
    }
    if (description)
        strcpy(g_enumSoundDevices[g_enumSoundDeviceCount].description, description);
    g_enumSoundDeviceCount++;
    return 1;
}
