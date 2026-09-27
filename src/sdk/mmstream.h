/* Stand-in for DirectShow and its multimedia streaming (mmstream.h, amstream.h, ddstream.h, strmif.h, uuids.h): the
 * constants, interfaces and GUIDs the decompiled source names, with the SDK's own spelling and value. The interfaces'
 * method order and signatures were checked against Wine's include/{mmstream,amstream,ddstream,axcore,axextend}.idl
 * (public API definitions); an interface ends at the last slot the game uses. Only what src/ uses is here. */
#ifndef SDW_SDK_MMSTREAM_H
#define SDW_SDK_MMSTREAM_H

typedef enum {
    STREAMSTATE_STOP = 0,
    STREAMSTATE_RUN = 1,
} STREAM_STATE;

typedef enum {
    STREAMTYPE_READ = 0,
} STREAM_TYPE;

#include "windef.h"
#include "ddraw.h"

struct AM_MEDIA_TYPE;
struct DDSURFACEDESC;
struct FILTER_INFO;
struct IBaseFilter;
struct IDirectDraw;
struct IDirectDrawPalette;
struct IDirectDrawStreamSample;
struct IDirectDrawSurface;
struct IEnumFilters;
struct IEnumPins;
struct IFilterGraph;
struct IGraphBuilder;
struct IMediaStream;
struct IMediaStreamFilter;
struct IMultiMediaStream;
struct IPin;
struct IReferenceClock;
struct IStreamSample;

struct IMultiMediaStream : IUnknown {
    virtual HRESULT __stdcall GetInformation(u32 *, s32 *) = 0;                  /* +0x0c */
    virtual HRESULT __stdcall GetMediaStream(const GUID &, IMediaStream **) = 0; /* +0x10 */
    virtual HRESULT __stdcall EnumMediaStreams(LONG, IMediaStream **) = 0;       /* +0x14 */
    virtual HRESULT __stdcall GetState(s32 *) = 0;                               /* +0x18 */
    virtual HRESULT __stdcall SetState(LONG) = 0;                                /* +0x1c */
    virtual HRESULT __stdcall GetTime(__int64 *) = 0;                            /* +0x20 */
    virtual HRESULT __stdcall GetDuration(__int64 *) = 0;                        /* +0x24 */
    virtual HRESULT __stdcall Seek(__int64) = 0;                                 /* +0x28 */
    virtual HRESULT __stdcall GetEndOfStreamEventHandle(void **) = 0;            /* +0x2c */
};

struct IMediaStream : IUnknown {
    virtual HRESULT __stdcall GetMultiMediaStream(IMultiMediaStream **) = 0;                    /* +0x0c */
    virtual HRESULT __stdcall GetInformation(GUID *, s32 *) = 0;                                /* +0x10 */
    virtual HRESULT __stdcall SetSameFormat(IMediaStream *, DWORD) = 0;                         /* +0x14 */
    virtual HRESULT __stdcall AllocateSample(DWORD, IStreamSample **) = 0;                      /* +0x18 */
    virtual HRESULT __stdcall CreateSharedSample(IStreamSample *, DWORD, IStreamSample **) = 0; /* +0x1c */
    virtual HRESULT __stdcall SendEndOfStream(DWORD) = 0;                                       /* +0x20 */
};

struct IStreamSample : IUnknown {
    virtual HRESULT __stdcall GetMediaStream(IMediaStream **) = 0;                      /* +0x0c */
    virtual HRESULT __stdcall GetSampleTimes(__int64 *, __int64 *, __int64 *) = 0;      /* +0x10 */
    virtual HRESULT __stdcall SetSampleTimes(const __int64 *, const __int64 *) = 0;     /* +0x14 */
    virtual HRESULT __stdcall Update(DWORD, void *, void(__stdcall *)(u32), DWORD) = 0; /* +0x18 */
    virtual HRESULT __stdcall CompletionStatus(DWORD, DWORD) = 0;                       /* +0x1c */
};

struct IPin : IUnknown {};

struct IEnumPins : IUnknown {
    virtual HRESULT __stdcall Next(DWORD, IPin **, u32 *) = 0; /* +0x0c */
    virtual HRESULT __stdcall Skip(DWORD) = 0;                 /* +0x10 */
    virtual HRESULT __stdcall Reset() = 0;                     /* +0x14 */
    virtual HRESULT __stdcall Clone(IEnumPins **) = 0;         /* +0x18 */
};

struct IFilterGraph : IUnknown {
    virtual HRESULT __stdcall AddFilter(IBaseFilter *, const unsigned short *) = 0;         /* +0x0c */
    virtual HRESULT __stdcall RemoveFilter(IBaseFilter *) = 0;                              /* +0x10 */
    virtual HRESULT __stdcall EnumFilters(IEnumFilters **) = 0;                             /* +0x14 */
    virtual HRESULT __stdcall FindFilterByName(const unsigned short *, IBaseFilter **) = 0; /* +0x18 */
    virtual HRESULT __stdcall ConnectDirect(IPin *, IPin *, const AM_MEDIA_TYPE *) = 0;     /* +0x1c */
    virtual HRESULT __stdcall Reconnect(IPin *) = 0;                                        /* +0x20 */
    virtual HRESULT __stdcall Disconnect(IPin *) = 0;                                       /* +0x24 */
    virtual HRESULT __stdcall SetDefaultSyncSource() = 0;                                   /* +0x28 */
};

struct IAMMultiMediaStream : IMultiMediaStream {
    virtual HRESULT __stdcall Initialize(LONG, DWORD, IGraphBuilder *) = 0;                         /* +0x30 */
    virtual HRESULT __stdcall GetFilterGraph(IGraphBuilder **) = 0;                                 /* +0x34 */
    virtual HRESULT __stdcall GetFilter(IMediaStreamFilter **) = 0;                                 /* +0x38 */
    virtual HRESULT __stdcall AddMediaStream(IUnknown *, const GUID *, DWORD, IMediaStream **) = 0; /* +0x3c */
    virtual HRESULT __stdcall OpenFile(const unsigned short *, DWORD) = 0;                          /* +0x40 */
};

struct IDirectDrawStreamSample : IStreamSample {
    virtual HRESULT __stdcall GetSurface(IDirectDrawSurface **, RECT *) = 0; /* +0x20 */
    virtual HRESULT __stdcall SetRect(const RECT *) = 0;                     /* +0x24 */
};

struct IDirectDrawMediaStream : IMediaStream {
    virtual HRESULT __stdcall GetFormat(DDSURFACEDESC *, IDirectDrawPalette **, DDSURFACEDESC *, u32 *) = 0; /* +0x24 */
    virtual HRESULT __stdcall SetFormat(const DDSURFACEDESC *, IDirectDrawPalette *) = 0;                    /* +0x28 */
    virtual HRESULT __stdcall GetDirectDraw(IDirectDraw **) = 0;                                             /* +0x2c */
    virtual HRESULT __stdcall SetDirectDraw(IDirectDraw *) = 0;                                              /* +0x30 */
    virtual HRESULT __stdcall CreateSample(IDirectDrawSurface *, const RECT *, DWORD,
                                           IDirectDrawStreamSample **) = 0; /* +0x34 */
};

struct IMediaFilter : IPersist {
    virtual HRESULT __stdcall Stop() = 0;                            /* +0x10 */
    virtual HRESULT __stdcall Pause() = 0;                           /* +0x14 */
    virtual HRESULT __stdcall Run(__int64) = 0;                      /* +0x18 */
    virtual HRESULT __stdcall GetState(DWORD, s32 *) = 0;            /* +0x1c */
    virtual HRESULT __stdcall SetSyncSource(IReferenceClock *) = 0;  /* +0x20 */
    virtual HRESULT __stdcall GetSyncSource(IReferenceClock **) = 0; /* +0x24 */
};

struct IGraphBuilder : IFilterGraph {
    virtual HRESULT __stdcall Connect(IPin *, IPin *) = 0; /* +0x2c */
};

struct IBaseFilter : IMediaFilter {
    virtual HRESULT __stdcall EnumPins(IEnumPins **) = 0;                                  /* +0x28 */
    virtual HRESULT __stdcall FindPin(const unsigned short *, IPin **) = 0;                /* +0x2c */
    virtual HRESULT __stdcall QueryFilterInfo(FILTER_INFO *) = 0;                          /* +0x30 */
    virtual HRESULT __stdcall JoinFilterGraph(IFilterGraph *, const unsigned short *) = 0; /* +0x34 */
    virtual HRESULT __stdcall QueryVendorInfo(unsigned short **) = 0;                      /* +0x38 */
};

/* strmiids.lib (DirectShow SDK) data, 0x577748-0x5777a7 */
extern "C" const GUID CLSID_AMMultiMediaStream;   /* 0x577768 {49c47ce5-9ba4-11d0-8212-00c04fc32c45} */
extern "C" const GUID CLSID_AudioRender;          /* 0x577788 {e30629d1-27e5-11ce-875d-00608cb78066} */
extern "C" const GUID IID_IAMMultiMediaStream;    /* 0x577778 {bebe595c-9a6f-11d0-8fde-00c04fd9189d} */
extern "C" const GUID IID_IBaseFilter;            /* 0x577748 {56a86895-0ad4-11ce-b03a-0020af0ba770} */
extern "C" const GUID IID_IDirectDrawMediaStream; /* 0x577758 {f4104fce-9a70-11d0-8fde-00c04fd9189d} */
extern "C" const GUID MSPID_PrimaryVideo;         /* 0x577798 {a35ff56a-9fda-11d0-8fdf-00c04fd9189d} */

#endif
