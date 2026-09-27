/* Stand-in for DirectSound (dsound.h of the DirectX 8 SDK): the constants the decompiled source names, with the SDK's own spelling and value.
 * Only what src/ uses is here: constants, types and functions (depends on sdw_types.h). */
#ifndef SDW_SDK_DSOUND_H
#define SDW_SDK_DSOUND_H

#define DSBCAPS_CTRLFREQUENCY 0x00000020
#define DSBCAPS_CTRLPAN 0x00000040
#define DSBCAPS_CTRLPOSITIONNOTIFY 0x00000100
#define DSBCAPS_CTRLVOLUME 0x00000080
#define DSBCAPS_GETCURRENTPOSITION2 0x00010000
#define DSBCAPS_LOCDEFER 0x00040000
#define DSBCAPS_PRIMARYBUFFER 0x00000001
#define DSBCAPS_STATIC 0x00000002
#define DSBFREQUENCY_MAX 100000 /* before DIRECTSOUND_VERSION 0x0900 */
#define DSBFREQUENCY_MIN 100
#define DSBPAN_LEFT (-10000)
#define DSBPAN_RIGHT 10000
#define DSBPLAY_LOOPING 0x00000001
#define DSBSTATUS_BUFFERLOST 0x00000002
#define DSBSTATUS_PLAYING 0x00000001
#define DSBVOLUME_MAX 0
#define DSERR_BUFFERLOST ((HRESULT)0x88780096L) /* MAKE_DSHRESULT(150) */
#define DSSCL_PRIORITY 0x00000002

#include "windef.h"

struct DSBPOSITIONNOTIFY;
struct DSBUFFERDESC;
struct DSCAPS;
struct IDirectSound;
struct IDirectSoundBuffer;
struct WAVEFORMATEX;

struct DSBUFFERDESC { /* DirectX 7 dsound.h, 0x24 bytes */
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwBufferBytes;
    DWORD dwReserved;
    WAVEFORMATEX *lpwfxFormat;
    GUID guid3DAlgorithm;
};

struct DSCAPS { /* 0x60 bytes */
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwMinSecondarySampleRate;
    DWORD dwMaxSecondarySampleRate;
    DWORD dwPrimaryBuffers;
    DWORD dwMaxHwMixingAllBuffers;
    DWORD dwMaxHwMixingStaticBuffers;
    DWORD dwMaxHwMixingStreamingBuffers;
    DWORD dwFreeHwMixingAllBuffers;
    DWORD dwFreeHwMixingStaticBuffers;
    DWORD dwFreeHwMixingStreamingBuffers;
    DWORD dwMaxHw3DAllBuffers;
    DWORD dwMaxHw3DStaticBuffers;
    DWORD dwMaxHw3DStreamingBuffers;
    DWORD dwFreeHw3DAllBuffers;
    DWORD dwFreeHw3DStaticBuffers;
    DWORD dwFreeHw3DStreamingBuffers;
    DWORD dwTotalHwMemBytes;
    DWORD dwFreeHwMemBytes;
    DWORD dwMaxContigFreeHwMemBytes;
    DWORD dwUnlockTransferRateHwBuffers;
    DWORD dwPlayCpuOverheadSwBuffers;
    DWORD dwReserved1;
    DWORD dwReserved2;
};

typedef BOOL(__stdcall *LPDSENUMCALLBACKA)(GUID *guid, const char *description, const char *module, void *context);

struct DSBPOSITIONNOTIFY {
    DWORD dwOffset;
    void *hEventNotify;
};

struct IDirectSoundBuffer : IUnknown {
    virtual HRESULT __stdcall GetCaps(void *caps) = 0;                                      /* +0x0c */
    virtual HRESULT __stdcall GetCurrentPosition(DWORD *play, DWORD *write) = 0;            /* +0x10 */
    virtual HRESULT __stdcall GetFormat(WAVEFORMATEX *fmt, DWORD size, DWORD *written) = 0; /* +0x14 */
    virtual HRESULT __stdcall GetVolume(s32 *volume) = 0;                                   /* +0x18 */
    virtual HRESULT __stdcall GetPan(s32 *pan) = 0;                                         /* +0x1c */
    virtual HRESULT __stdcall GetFrequency(u32 *freq) = 0;                                  /* +0x20 */
    virtual HRESULT __stdcall GetStatus(DWORD *status) = 0;                                 /* +0x24 */
    virtual HRESULT __stdcall Initialize(IDirectSound *ds, const DSBUFFERDESC *desc) = 0;   /* +0x28 */
    virtual HRESULT __stdcall Lock(DWORD off, DWORD bytes, void **p1, DWORD *n1, void **p2, DWORD *n2,
                                   DWORD flags) = 0;                                  /* +0x2c */
    virtual HRESULT __stdcall Play(DWORD reserved1, DWORD priority, DWORD flags) = 0; /* +0x30 */
    virtual HRESULT __stdcall SetCurrentPosition(DWORD pos) = 0;                      /* +0x34 */
    virtual HRESULT __stdcall SetFormat(const WAVEFORMATEX *fmt) = 0;                 /* +0x38 */
    virtual HRESULT __stdcall SetVolume(LONG volume) = 0;                             /* +0x3c */
    virtual HRESULT __stdcall SetPan(LONG pan) = 0;                                   /* +0x40 */
    virtual HRESULT __stdcall SetFrequency(DWORD freq) = 0;                           /* +0x44 */
    virtual HRESULT __stdcall Stop() = 0;                                             /* +0x48 */
    virtual HRESULT __stdcall Unlock(void *p1, DWORD n1, void *p2, DWORD n2) = 0;     /* +0x4c */
    virtual HRESULT __stdcall Restore() = 0;                                          /* +0x50 */
};

struct IDirectSound : IUnknown {
    virtual HRESULT __stdcall CreateSoundBuffer(const DSBUFFERDESC *desc, IDirectSoundBuffer **out,
                                                void *outer) = 0;                                          /* +0x0c */
    virtual HRESULT __stdcall GetCaps(DSCAPS *caps) = 0;                                                   /* +0x10 */
    virtual HRESULT __stdcall DuplicateSoundBuffer(IDirectSoundBuffer *src, IDirectSoundBuffer **out) = 0; /* +0x14 */
    virtual HRESULT __stdcall SetCooperativeLevel(HWND hwnd, DWORD level) = 0;                             /* +0x18 */
    virtual HRESULT __stdcall Compact() = 0;                                                               /* +0x1c */
    virtual HRESULT __stdcall GetSpeakerConfig(DWORD *config) = 0;                                         /* +0x20 */
    virtual HRESULT __stdcall SetSpeakerConfig(DWORD config) = 0;                                          /* +0x24 */
    virtual HRESULT __stdcall Initialize(const GUID *device) = 0;                                          /* +0x28 */
};

struct IDirectSoundNotify : IUnknown {
    virtual HRESULT __stdcall SetNotificationPositions(DWORD count, const DSBPOSITIONNOTIFY *pos) = 0; /* +0x0c */
};

/* DSOUND.dll is imported by ordinal through the import library's thunks: 0x565836 jmp [0x574038], 0x56583c jmp [0x57403c]. */
extern "C" HRESULT __stdcall DirectSoundCreate(GUID *device, IDirectSound **out, void *outer); /* 0x56583c */
extern "C" HRESULT __stdcall DirectSoundEnumerateA(LPDSENUMCALLBACKA callback, void *context); /* 0x565836 */
extern "C" const GUID
    IID_IDirectSoundNotify; /* 0x577638 {b0210783-89cd-11d0-af08-00a0c925cd16}: DirectX SDK library data, C linkage */

#endif
