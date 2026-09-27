/* match-flags: /O2 /Oy- /Ob2 */
/*
 * T031 - original object Sound.cpp (guessed name; the abstract DirectSound buffer base): SheepD3D.exe .text COMDATs
 * Sound::Sound 0x4205b0-0x4205e0, ??_GSound 0x4205e0-0x420640 (with ~Sound inlined) and Sound::~Sound
 * 0x420640-0x420680; .rdata 0x57441c-0x574458 (Sound's vtable: slot 0 the scalar deleting destructor, slots 1-14
 * _purecall 0x567340). Built /O2 /Oy- /Ob2 (every function a /Gy COMDAT), like the rest of the sound classes.
 *
 * This object is the Sound part of the sound classes (the constructor and the destructor). Being an object of its own
 * is what makes StaticSound / StreamSound call Sound::Sound out of line.
 *
 * The code is a game-side rework of the DirectX SDK samples: Sound / StaticSound / StreamSound follow the DX8 dsutil
 * CSound / CStreamingSound.
 */
/* BYTES: switches. */
/* BYTES(switches): built with /O2 /Oy- /Ob2, not the project recipe; the file header says why */
#define SDW_MEMBERS_Sound Sound(); /* 0x4205b0 Sound_Construct */
#include "sdw_classes.h"

#include "../sdk/win32.h"
#include "../sdk/mmsystem.h"
#include "../sdk/dsound.h"
#include "../sdk/crt.h"

/* ================================================================ Sound, the abstract base (vtable 0x57441c) */

/* 0x4205b0 */
Sound::Sound()
{
    looping = 0;
    waveIsExternal = 1;
    bufferBytes = 0;
    wave = 0;
    buffer = 0;
    volumeCb = 0;
    panCb = 0;
    baseFrequency = 0;
}

/* 0x420640 (0x4205e0 is the scalar deleting destructor, with this inlined) */
Sound::~Sound()
{
    if (wave && !waveIsExternal) {
        delete wave;
        wave = 0;
    }
    if (buffer) {
        buffer->Release();
        buffer = 0;
    }
}
