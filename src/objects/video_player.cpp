/* match-flags: /O2 /Oy- /Ob2 */
/*
 * T035 - original object VideoPlayer.cpp (guessed name): SheepD3D.exe .text COMDATs 0x421ba0-0x421db0 (VideoPlayer's
 * constructor, ??_GVideoPlayer with the destructor inlined, the destructor, Init, PlayFile, VideoPlayer_BlitFrame);
 * .rdata 0x5744e0-0x5744e4 (VideoPlayer's vtable); .bss 0x6cc848-0x6cc860 (g_videoStretchFlag, g_videoDestRect,
 * g_pVideoPrimarySurface, used only by these functions). Built /O2 /Oy- /Ob2.
 *
 * This object is the VideoPlayer part of the video code (0x421ba0-0x421d50); the Video object is src/objects/video.cpp,
 * and Init calls Video::Video, CreateStream and SetFrameCallback out of line across that boundary.
 * COM declarations (src/sdk/mmstream.h) are SDK interfaces, not replacement game classes.
 * Init names the DirectDraw pointer in a local (register choice ecx/dl): a representation, not proof of the original.
 *
 * The three .bss globals are defined here. VC6 lays out an object's .bss by a hash of the (undecorated) names, not by
 * definition order (the .bss rule in src/README.md): g_videoStretchFlag (0x6cc848), g_videoDestRect and
 * g_pVideoPrimarySurface (0x6cc85c) hash 342 < 724 < 915 and give the exe's order, which the descriptive names
 * g_videoStretchMode (952) and g_pVideoPrimary (331) would reverse. Neither name is recovered from the original;
 * data/symbols.csv uses the same two.
 */
/* BYTES: bss-name, switches, temp. */
/* BYTES(bss-name): named for its .bss hash key 342 */
/* BYTES(bss-name): named for its .bss hash key 915 */
/* BYTES(switches): built with /O2 /Oy- /Ob2, not the project recipe; the file header says why */
#include "../sdk/mmstream.h"
#include "../sdk/ddraw.h"
#define SDW_MEMBERS_VideoPlayer VideoPlayer();
#define SDW_MEMBERS_Video \
    Video();              \
    VideoFrameProc SetFrameCallback(VideoFrameProc);
#include "sdw_classes.h"

/* ---- .bss (this object's, 0x6cc848-0x6cc860) ---- */
u8 g_videoStretchFlag;                       /* 0x6cc848  PlayFile's stretch argument, read by VideoPlayer_BlitFrame */
RECT g_videoDestRect;                        /* 0x6cc84c  the D3DApp client rect, the stretched blit's destination */
IDirectDrawSurface7 *g_pVideoPrimarySurface; /* 0x6cc85c  the primary surface the frames are blitted to */

s32 VideoPlayer_BlitFrame(IDirectDrawSurface *, RECT *);

/* 0x421ba0 */
VideoPlayer::VideoPlayer()
{
    ready = 0;
    app = 0;
    stream = 0;
    g_pVideoPrimarySurface = 0;
}
/* 0x421c00 (inlined in the deleting destructor 0x421bc0) */
VideoPlayer::~VideoPlayer()
{
    if (stream)
        delete stream;
    app = 0;
}
/* 0x421c30 */
/* BYTES(temp): the DirectDraw pointer goes through a local for the original register choice (ecx / dl) */
u8 VideoPlayer::Init(D3DApp *device)
{
    ready = 0;
    if (device->deviceReady == 1) {
        app = device;
        g_pVideoPrimarySurface = app->pPrimary;
        g_videoDestRect = app->clientRect;
        if (g_pVideoPrimarySurface) {
            if (stream)
                delete stream;
            stream = new Video;
            IDirectDraw7 *dd = app->pDD;
            ready = stream->CreateStream(dd) != 0;
            stream->SetFrameCallback(VideoPlayer_BlitFrame);
        }
    }
    return ready;
}
/* 0x421ce0 */
u8 VideoPlayer::PlayFile(const char *path, u8 stretch)
{
    u8 result = 0;
    if (stream->OpenFile(path) == 1) {
        s32 played;
        g_videoStretchFlag = stretch;
        stream->ConnectAudioStream(0);
        played = stream->RunLoop();
        stream->ReleaseFilter(0);
        if (played == 1 && stream->CloseFile() == 1)
            result = 1;
    }
    return result;
}
/* 0x421d50 */
s32 VideoPlayer_BlitFrame(IDirectDrawSurface *source, RECT *sourceRect)
{
    /* cast kept (both calls): the stream hands its frame over as an IDirectDrawSurface; Blt takes the version-7 one */
    if (g_videoStretchFlag == 1)
        g_pVideoPrimarySurface->Blt(&g_videoDestRect, (IDirectDrawSurface7 *)source, sourceRect, DDBLT_WAIT, 0);
    else
        g_pVideoPrimarySurface->Blt(sourceRect, (IDirectDrawSurface7 *)source, sourceRect, DDBLT_WAIT, 0);
    return 1;
}
