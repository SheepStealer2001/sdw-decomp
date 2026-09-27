/* Stand-in for multimedia I/O (mmsystem.h): the constants the decompiled source names, with the SDK's own spelling and value.
 * Only what src/ uses is here: constants, types and functions (depends on sdw_types.h). */
#ifndef SDW_SDK_MMSYSTEM_H
#define SDW_SDK_MMSYSTEM_H

#define FOURCC_MEM mmioFOURCC('M', 'E', 'M', ' ')
#define FOURCC_RIFF mmioFOURCC('R', 'I', 'F', 'F')
#define MMIO_ALLOCBUF 0x00010000
#define MMIO_EXCLUSIVE 0x00000010
#define MMIO_FINDCHUNK 0x0010
#define MMIO_READ 0x00000000
#define WAVE_FORMAT_PCM 1
#define mmioFOURCC(ch0, ch1, ch2, ch3) \
    ((u32)(u8)(ch0) | ((u32)(u8)(ch1) << 8) | ((u32)(u8)(ch2) << 16) | ((u32)(u8)(ch3) << 24))

#include "windef.h"

struct HMMIO__;
struct MMCKINFO;
struct MMIOINFO;

#pragma pack(push, 1)
struct WAVEFORMATEX { /* mmreg.h, packed: 0x12 bytes */
    u16 wFormatTag;
    u16 nChannels;
    u32 nSamplesPerSec;
    u32 nAvgBytesPerSec;
    u16 nBlockAlign;
    u16 wBitsPerSample;
    u16 cbSize;
};
#pragma pack(pop)

typedef HMMIO__ *HMMIO;
typedef UINT MMRESULT;

struct MMIOINFO { /* mmsystem.h, 0x48 bytes */
    DWORD dwFlags;
    u32 fccIOProc;
    void *pIOProc;
    u32 wErrorRet;
    void *htask;
    s32 cchBuffer;
    char *pchBuffer;
    char *pchNext;
    char *pchEndRead;
    char *pchEndWrite;
    s32 lBufOffset;
    s32 lDiskOffset;
    DWORD adwInfo[3];
    DWORD dwReserved1;
    DWORD dwReserved2;
    HMMIO hmmio;
};

/* ReadRiffHeader's declarations */
struct PCMWAVEFORMAT {
    u16 wFormatTag, nChannels;
    u32 nSamplesPerSec, nAvgBytesPerSec;
    u16 nBlockAlign, wBitsPerSample;
};

extern "C" __declspec(dllimport) MMRESULT __stdcall mmioAdvance(HMMIO hmmio, MMIOINFO *pmmioinfo, UINT fuAdvance);
extern "C" __declspec(dllimport) MMRESULT __stdcall mmioAscend(HMMIO hmmio, MMCKINFO *pmmcki, UINT fuAscend);
extern "C" __declspec(dllimport) MMRESULT __stdcall mmioClose(HMMIO hmmio, UINT fuClose);
extern "C" __declspec(dllimport) MMRESULT __stdcall mmioDescend(HMMIO hmmio, MMCKINFO *pmmcki,
                                                                const MMCKINFO *pmmckiParent, UINT fuDescend);
extern "C" __declspec(dllimport) MMRESULT __stdcall mmioGetInfo(HMMIO hmmio, MMIOINFO *pmmioinfo, UINT fuInfo);
extern "C" __declspec(dllimport) HMMIO __stdcall mmioOpenA(char *pszFileName, MMIOINFO *pmmioinfo, DWORD fdwOpen);
extern "C" __declspec(dllimport) LONG __stdcall mmioRead(HMMIO hmmio, char *pch, LONG cch);
extern "C" __declspec(dllimport) LONG __stdcall mmioSeek(HMMIO hmmio, LONG lOffset, int iOrigin);
extern "C" __declspec(dllimport) MMRESULT __stdcall mmioSetInfo(HMMIO hmmio, const MMIOINFO *pmmioinfo, UINT fuInfo);

#endif
