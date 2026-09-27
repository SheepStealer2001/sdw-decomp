/* match-flags: /O2 /Oy- /Ob2 */
/* match-addr: _IID_IBaseFilter=0x577748 _IID_IDirectDrawMediaStream=0x577758 _CLSID_AMMultiMediaStream=0x577768 */
/* match-addr: _IID_IAMMultiMediaStream=0x577778 _CLSID_AudioRender=0x577788 _MSPID_PrimaryVideo=0x577798 */
/*
 * T036 - original object Video.cpp (guessed name; the DirectShow amstream wrapper): SheepD3D.exe .text COMDATs
 * 0x421db0-0x4222c0 (Video's constructor, ??_GVideo with ~Video inlined, OpenFile, CreateStream, CloseFile, RunLoop,
 * SetFrameCallback, ConnectAudioStream, ReleaseFilter, Video_NullFrameProc 0x4222b0 - the /OPT:ICF survivor that
 * jmemnobs' jpeg_mem_init shares); .rdata 0x5744e4-0x5744e8 (Video's vtable); .data 0x5794fc-0x579570 (the three /GF
 * wide-string COMDATs of ConnectAudioStream). Built /O2 /Oy- /Ob2. VideoPlayer before it is T035.
 *
 * This object is the Video part of the player (0x421db0-0x4222b3); VideoPlayer, before it, is T035. The six DirectShow
 * GUIDs are strmiids.lib data at 0x577748-0x5777a7, outside this object (defining them here would put 96 bytes into
 * this object's .rdata). They are declared as that library declares them (extern "C", the SDK names IID_IBaseFilter,
 * IID_IDirectDrawMediaStream, CLSID_AMMultiMediaStream, IID_IAMMultiMediaStream, CLSID_AudioRender,
 * MSPID_PrimaryVideo - values checked against the exe bytes); the match-addr lines pin them to their addresses, which
 * the symbol tables also give.
 *
 * COM declarations (src/sdk/mmstream.h) are SDK interfaces, not replacement game classes. Shapes that fixed block order
 * (representations, not proof of the original text): OpenFile / CreateStream / ReleaseFilter use `goto fail` chains
 * (return 0 block laid out before return 1); RunLoop's loop is one `while(a && b) ;`.
 */
/* BYTES: flow, switches. */
/* BYTES(switches): built with /O2 /Oy- /Ob2, not the project recipe; the file header says why */
#include "../sdk/mmstream.h"
#include "../sdk/mmstream.h"
#include "../sdk/win32.h"
#define SDW_MEMBERS_VideoPlayer VideoPlayer();
#define SDW_MEMBERS_Video \
    Video();              \
    VideoFrameProc SetFrameCallback(VideoFrameProc);
#include "sdw_classes.h"
#include "../sdk/crt.h"
#define RELEASE(p)      \
    if (p)              \
        (p)->Release(); \
    p = 0
s32 Video_NullFrameProc(IDirectDrawSurface *, RECT *);

/* 0x421db0 */
Video::Video()
{
    frameCallback = Video_NullFrameProc;
    graph = 0;
    mediaStream = 0;
    unknownStream = 0;
    sample = 0;
    surface = 0;
    audioFilter = 0;
}
/* inlined in the deleting destructor 0x421de0 */
inline Video::~Video()
{
    RELEASE(graph);
    RELEASE(surface);
    RELEASE(sample);
    RELEASE(unknownStream);
    RELEASE(mediaStream);
}
/* 0x421e60 */
/* BYTES(flow): goto fail chain: it lays the failure return out before the success return, as the original */
s32 Video::OpenFile(const char *path)
{
    IMediaStream *media = 0;
    IDirectDrawMediaStream *draw = 0;
    unsigned short widePath[260];
    if (!mediaStream)
        goto fail;
    MultiByteToWideChar(CP_ACP, 0, path, -1, widePath, 260);
    if (mediaStream->OpenFile(widePath, 0) < 0)
        goto fail;
    if (mediaStream->GetFilterGraph(&graph) < 0)
        goto fail;
    if (mediaStream->GetMediaStream(MSPID_PrimaryVideo, &media) < 0)
        goto fail;
    /* cast kept: COM returns the interface through a void ** out parameter */
    if (media->QueryInterface(IID_IDirectDrawMediaStream, (void **)&draw) < 0)
        goto fail;
    if (draw->CreateSample(0, 0, 0, &sample) < 0)
        goto fail;
    if (sample->GetSurface(&surface, &sourceRect) < 0)
        goto fail;
    RELEASE(draw);
    RELEASE(media);
    return 1;
fail:
    RELEASE(draw);
    RELEASE(media);
    return 0;
}
/* 0x421f80 */
s32 Video::CreateStream(IDirectDraw7 *directDraw)
{
    /* cast kept: COM returns the interface through a void ** out parameter */
    if (CoCreateInstance(CLSID_AMMultiMediaStream, 0, CLSCTX_INPROC_SERVER, IID_IAMMultiMediaStream,
                         (void **)&mediaStream) < 0)
        goto fail;
    if (mediaStream->Initialize(STREAMTYPE_READ, 0, 0) < 0)
        goto fail;
    if (mediaStream->AddMediaStream(directDraw, &MSPID_PrimaryVideo, 0, 0) < 0)
        goto fail;
    return 1;
fail:
    return 0;
}
/* 0x421fe0 */
s32 Video::CloseFile()
{
    RELEASE(graph);
    RELEASE(surface);
    RELEASE(sample);
    RELEASE(unknownStream);
    return 1;
}
/* 0x422030 */
/* BYTES(flow): one while (a && b) ; loop, for the original block order */
s32 Video::RunLoop()
{
    if (mediaStream && sample && mediaStream->SetState(STREAMSTATE_RUN) >= 0) {
        while (sample->Update(0, 0, 0, 0) == S_OK && frameCallback(surface, &sourceRect))
            ;
        if (mediaStream->SetState(STREAMSTATE_STOP) >= 0)
            return 1;
    }
    return 0;
}
/* 0x4220a0 */
VideoFrameProc Video::SetFrameCallback(VideoFrameProc callback)
{
    VideoFrameProc old = frameCallback;
    frameCallback = callback;
    return old;
}
/* 0x4220b0 */
s32 Video::ConnectAudioStream(s32 audioIndex)
{
    unsigned short name[256];
    IBaseFilter *splitter = 0, *existing = 0;
    IPin *input = 0, *output;
    IEnumPins *pins;
    u32 fetched;
    s32 result = 0;
    if (graph) {
        swprintf(name, L"AudioRender %d", audioIndex);
        if (graph->FindFilterByName(name, &existing) < 0 &&
            /* cast kept: COM returns the interface through a void ** out parameter */
            CoCreateInstance(CLSID_AudioRender, 0, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **)&audioFilter) >= 0 &&
            graph->AddFilter(audioFilter, name) >= 0 &&
            audioFilter->FindPin(L"Audio Input pin (rendered)", &input) >= 0 &&
            graph->FindFilterByName(L"AVI Splitter", &splitter) >= 0) {
            u16 index = 0;
            pins = 0;
            splitter->EnumPins(&pins);
            pins->Reset();
            output = 0;
            while (pins->Next(1, &output, &fetched) == S_OK) {
                if (graph->Connect(output, input) < 0)
                    output->Release();
                else {
                    if (index == audioIndex)
                        break;
                    ++index;
                    graph->Disconnect(input);
                    graph->Disconnect(output);
                }
            }
            if (output)
                result = 1;
        }
    }
    RELEASE(pins);
    RELEASE(output);
    RELEASE(splitter);
    RELEASE(input);
    return result;
}
/* 0x422270 */
s32 Video::ReleaseFilter(s32 unused)
{
    long hr;
    if (!graph)
        goto fail;
    hr = graph->RemoveFilter(audioFilter);
    RELEASE(audioFilter);
    if (hr < 0)
        goto fail;
    return 1;
fail:
    return 0;
}
/* 0x4222b0 */
s32 Video_NullFrameProc(IDirectDrawSurface *, RECT *)
{
    return 0;
}
#undef RELEASE
