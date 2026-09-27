/* match-flags: /O2 /Oy- /Ob2 */
/*
 * T034 - original object WaveFile.cpp (guessed name; the DX7 wavread CWaveSoundRead): SheepD3D.exe .text COMDATs
 * 0x421740-0x421ba0 (the constructor, ??_GWaveFile with the destructor and Close inlined, the destructor, Open,
 * ResetFile, Read, Close, ReadMmio (WaveReadFile), ReadRiffHeader (WaveOpenFile's header parse)); .rdata
 * 0x5744dc-0x5744e0 (WaveFile's vtable). Built /O2 /Oy- /Ob2.
 * The local PCMWAVEFORMAT and the operator new / delete declarations are ReadRiffHeader's; WAVEFORMATEX is the 18-byte
 * SDK layout.
 *
 * WaveFile::Open is written as two __forceinline halves (OpenDisk / OpenMemory): each keeps its own `lea &ckRiff`
 * (they do not cross-jump then), which leaves EBX free for hr, as the original has it. A byte-matching shape, not
 * evidence that the original source was split this way.
 */
/* BYTES: inline, switches. */
/* BYTES(switches): built with /O2 /Oy- /Ob2, not the project recipe; the file header says why */
#define SDW_MEMBERS_WaveFile                                                                                    \
    WaveFile(); /* 0x421740 WaveFile_Construct */ /* The two halves of Open, forced inline (see the header). */ \
    __forceinline s32 OpenDisk(char *name);                                                                     \
    __forceinline s32 OpenMemory(char *name, u32 size);
#include "sdw_classes.h"

#include "../sdk/win32.h"
#include "../sdk/mmsystem.h"
#include "../sdk/crt.h"

/* the RIFF WAVE chunk ids, named here: the SDK has no constants for them (its samples write mmioFOURCC at each use) */
#define FOURCC_DATA mmioFOURCC('d', 'a', 't', 'a')
#define FOURCC_WAVE mmioFOURCC('W', 'A', 'V', 'E')
#define FOURCC_FMT mmioFOURCC('f', 'm', 't', ' ')

void *operator new(u32);
void operator delete(void *);

/* ================================================================ WaveFile (vtable 0x5744dc), the DX7 CWaveSoundRead */

/* 0x421740 */
WaveFile::WaveFile()
{
    format = 0;
}

/* 0x4217a0 (0x421750 is the scalar deleting destructor) */
WaveFile::~WaveFile()
{
    Close();
    if (format) {
        delete format;
        format = 0;
    }
}

/* 0x4217d0 - opens a .wav file (fromMemory 0) or `size` bytes of RIFF image at name (fromMemory 1, the 'MEM ' IOProc),
 * parses the header into ckRiff / format and descends to the 'data' chunk. The two branches are the forced-inline
 * halves OpenDisk and OpenMemory: each keeps its own `lea` for &ckRiff (so they do not cross-jump) and the inlined
 * ResetFile computes a third, which leaves EBX for hr, as the original has it (see the file header). */
/* BYTES(inline): Open is split into two __forceinline halves so each keeps its own lea &ckRiff and EBX stays free for hr */
__forceinline s32 WaveFile::OpenDisk(char *name)
{
    s32 hr = E_FAIL;
    HMMIO h;

    h = mmioOpenA(name, 0, MMIO_ALLOCBUF | MMIO_READ);
    if (h) {
        /* success first: that branch order is the original's */
        if ((hr = ReadRiffHeader(h, &ckRiff, &format)) >= 0) {
            hmmio = h;
            hr = S_OK;
        } else {
            mmioClose(h, 0);
        }
    }
    return hr;
}

__forceinline s32 WaveFile::OpenMemory(char *name, u32 size)
{
    s32 hr = E_FAIL;
    HMMIO h;
    MMIOINFO info;

    memset(&info, 0, sizeof info);
    info.pchBuffer = name;
    info.cchBuffer = size;
    info.fccIOProc = FOURCC_MEM;
    h = mmioOpenA(0, &info, MMIO_READ);
    if (h) {
        if ((hr = ReadRiffHeader(h, &ckRiff, &format)) >= 0) {
            hmmio = h;
            hr = S_OK;
        } else {
            mmioClose(h, 0);
        }
    }
    return hr;
}

s32 WaveFile::Open(char *name, u8 fromMemory, u32 size)
{
    s32 hr;

    Close();
    if (format) {
        delete format;
        format = 0;
    }
    if (!fromMemory)
        hr = OpenDisk(name);
    else
        hr = OpenMemory(name, size);
    if (hr >= 0)
        hr = ResetFile();
    return hr;
}

/* 0x4218e0 - seeks back to the start of the RIFF payload and descends into 'data' again (restoring ckData.cksize). */
s32 WaveFile::ResetFile()
{
    MMCKINFO *riff = &ckRiff;
    MMCKINFO *ck = &ckData;
    s32 hr = E_FAIL;

    if (mmioSeek(hmmio, riff->dwDataOffset + 4, SEEK_SET) != -1) {
        ck->ckid = FOURCC_DATA;
        if (mmioDescend(hmmio, ck, riff, MMIO_FINDCHUNK) == 0)
            return S_OK;
    }
    return hr;
}

/* 0x421930 */
s32 WaveFile::Read(u32 size, u8 *dest, u32 *read)
{
    return ReadMmio(hmmio, size, dest, &ckData, read);
}

/* 0x421950 - closes the handle but neither frees `format` nor clears hmmio. */
s32 WaveFile::Close()
{
    mmioClose(hmmio, 0);
    return S_OK;
}

/* 0x421960 - the DX7 WaveReadFile: copies up to min(size, ck->cksize) bytes through the mmio buffer. */
s32 WaveFile::ReadMmio(HMMIO h, u32 size, u8 *dest, MMCKINFO *ck, u32 *read)
{
    MMIOINFO info;
    s32 hr = E_FAIL;
    u32 n;
    u32 i;

    *read = 0;
    if (mmioGetInfo(h, &info, 0) == 0) {
        n = size;
        if (n > ck->cksize)
            n = ck->cksize;
        ck->cksize -= n;
        for (i = 0; i < n; i++) {
            if (info.pchNext == info.pchEndRead) {
                if (mmioAdvance(h, &info, MMIO_READ) != 0)
                    return E_FAIL;
                if (info.pchNext == info.pchEndRead)
                    return E_FAIL;
            }
            dest[i] = *info.pchNext;
            info.pchNext++;
        }
        if (mmioSetInfo(h, &info, 0) == 0) {
            *read = n;
            hr = S_OK;
        }
    }
    return hr;
}

s32 WaveFile::ReadRiffHeader(HMMIO__ *h, MMCKINFO *riff, WAVEFORMATEX **format)
{
    s32 result = E_FAIL;
    PCMWAVEFORMAT pcm;
    MMCKINFO chunk;
    *format = 0;
    if (mmioDescend(h, riff, 0, 0) == 0 && riff->ckid == FOURCC_RIFF && riff->fccType == FOURCC_WAVE) {
        chunk.ckid = FOURCC_FMT;
        /* cast kept: mmioRead fills a char buffer (the SDK's HPSTR) */
        if (mmioDescend(h, &chunk, riff, MMIO_FINDCHUNK) == 0 && chunk.cksize >= 16 &&
            mmioRead(h, (char *)&pcm, 16) == 16) {
            if (pcm.wFormatTag == WAVE_FORMAT_PCM) {
                *format = (WAVEFORMATEX *)operator new(18); /* cast kept: operator new returns void * */
                if (*format) {
                    *(PCMWAVEFORMAT *)*format =
                        pcm; /* cast kept: a WAVEFORMATEX begins with the PCMWAVEFORMAT fields */
                    (*format)->cbSize = 0;
                    result = S_OK;
                }
            } else {
                u32 extra = 0;
                if (mmioRead(h, (char *)&extra, 2) == 2) {              /* cast kept: mmioRead fills a char buffer */
                    *format = (WAVEFORMATEX *)operator new(18 + extra); /* cast kept: operator new returns void * */
                    if (*format) {
                        *(PCMWAVEFORMAT *)*format =
                            pcm; /* cast kept: a WAVEFORMATEX begins with the PCMWAVEFORMAT fields */
                        (*format)->cbSize = (u16)extra;
                        /* cast kept: the extra format bytes follow the 18-byte WAVEFORMATEX */
                        if (mmioRead(h, (char *)*format + 18, extra) != extra) {
                            operator delete(*format);
                            *format = 0;
                        } else
                            result = S_OK;
                    }
                }
            }
            if (result == S_OK && mmioAscend(h, &chunk, 0) != 0) {
                operator delete(*format);
                result = E_FAIL;
                *format = 0;
            }
        }
    }
    return result;
}
