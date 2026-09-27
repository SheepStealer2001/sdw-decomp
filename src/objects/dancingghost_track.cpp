/* T133b - DancingGhostManager::UpdateBeatTrack as its own original object (the map's alternative for T133, taken
 * because the COMDAT reading cannot be reproduced; see src/objects/dancingghost_manager.cpp). Ranges: .text
 * 0x4b2840-0x4b3265, .bss 0x6cf60c-0x6cf610 (g_dgMetronomeLatch). */
/* BYTES: layout. */
/* BYTES(layout): its own object: in the manager's file /Ob1 would inline it into Update (the COMDAT reading cannot be reproduced) */
/* PAL PC DancingGhostManager rhythm track, including the unreachable branch
 * retained by the original unoptimized compiler. */
#include "sdw_types.h"
#include "sdw_enums.h"
#define SDW_MEMBERS_Sprite void DrawAt(u32 *layer, s32 drawX, s32 yPos);
#include "sdw_classes.h"
#define SDW_INLINE_SPRITE_DRAWAT_U32_S32_S32 1
#include "../engine/sprite_inlines.h"
#undef SDW_INLINE_SPRITE_DRAWAT_U32_S32_S32
extern s32 g_dt;
s32 g_dgMetronomeLatch; /* 0x6cf60c .bss: the metronome one-shot latch, used only here */
#include "../engine/input.h"
#include "dancingghost_manager.h"
#define g_padPrevButtons (g_pad.prev.buttons)
#define g_padCurButtons (g_pad.cur.buttons)
extern u32 *g_screenLayerBase;
u16 Sound_Play(u16, void *, u16, u8, s32);
s32 DancingGhostManager::UpdateBeatTrack()
{
    u16 j, expected;
    s16 drawX, yPos;
    s32 changed;
    u16 beatSlot, noteIndex;
    changed = 0;
    beatSlot = 0xffff;
    expected = PAD_ALL_RELEASED;
    if ((u32)danceClock / beatPeriod != (u32)(danceClock - g_dt) / beatPeriod)
        changed = 1;
    if (beatIndex > 2) {
        beatSlot = beatIndex - 8;
        if (beatSlot < sequenceLengths[stepIndex])
            expected = sequenceButtons[stepIndex][beatSlot];
        else if ((s16)beatSlot >= sequenceLengths[stepIndex])
            return 1;
    }
    if (changed) {
        g_dgMetronomeLatch = 0;
        if (pressedMask != expected) {
            if (!eventCode)
                eventCode = DANCE_EV_3;
            return 0;
        }
        if (previousStep != AFANTO02_ANIM_DANCE5)
            previousStep = 0xffff;
        pressedMask = PAD_ALL_RELEASED;
        pressHandled = 0;
        beatIndex++;
        beatTimeMs = (u32)(danceClock * 1000) >> 12;
        beatElapsedMs = 0;
    } else {
        beatElapsedMs = ((u32)(danceClock * 1000) >> 12) - beatTimeMs;
        if (beatElapsedMs >= 50 && !g_dgMetronomeLatch) {
            beatSound = Sound_Play(SND_BATTERY_PICKUP, this, 255, SNDF_NO_RETRIGGER, 4096);
            g_dgMetronomeLatch = 1;
        }
    }
    if (beatIndex > 2 && !pressHandled) {
        if (pressedMask == PAD_ALL_RELEASED && g_padPrevButtons != g_padCurButtons) {
            padMask = g_padCurButtons & (PAD_TRIANGLE | PAD_CIRCLE | PAD_CROSS | PAD_SQUARE);
            padMask |= (u16) ~(PAD_TRIANGLE | PAD_CIRCLE | PAD_CROSS | PAD_SQUARE);
            if (padMask != PAD_ALL_RELEASED)
                pressedMask = padMask;
        }
        if (pressedMask != PAD_ALL_RELEASED) {
            pressTimeMs = (u32)(danceClock * 1000) >> 12;
            pressLateMs = pressTimeMs - beatTimeMs;
            pressLateMinus250Ms = pressTimeMs - (beatTimeMs + 250);
            if (pressLateMs > 200 || (pressLateMinus250Ms > 0 && pressLateMinus250Ms > 200)) {
                if (pressedMask == expected) {
                    if (!eventCode)
                        eventCode = DANCE_EV_1;
                    return 0;
                } else {
                    if (!eventCode)
                        eventCode = DANCE_EV_3;
                    return 0;
                }
            } else if (pressedMask != expected) {
                if (!eventCode)
                    eventCode = DANCE_EV_2;
                return 0;
            }
            pressHandled = 1;
        } else if (expected != PAD_ALL_RELEASED && beatElapsedMs > 450) {
            if (!eventCode)
                eventCode = DANCE_EV_3;
            return 0;
        }
    }
    spriteTrack.Draw(g_screenLayerBase + 10, trackX, trackY, trackX + (trackWidth >> 1), trackY + trackHeight, 0x808080,
                     0);
    spriteTrack.Draw(g_screenLayerBase + 10, trackX + (trackWidth >> 1), trackY, trackX + trackWidth,
                     trackY + trackHeight, 0x808080, 2);
    for (noteIndex = 0; noteIndex < sequenceLengths[stepIndex]; noteIndex++) {
        drawX = (cellW >> 1) * (beatIndex - noteIndex) + noteLeft;
        if (!changed && beatElapsedMs >= 250)
            drawX += (s16)((cellW >> 1) * (beatElapsedMs >> 1) / 500);
        yPos = noteTop + (cellH >> 1);
        if (drawX > trackX && drawX + (cellW >> 1) < noteRight) {
            if (noteIndex % 4 == 0)
                currentSprite = &spriteBarNote;
            else
                currentSprite = &spriteNote;
            if (currentSprite)
                currentSprite->Draw(g_screenLayerBase + 9, drawX, (s16)(yPos - (cellH >> 1)),
                                    (s16)(drawX + (cellW >> 1)), (s16)(yPos + cellH + (cellH >> 1)), 0x808080, 0);
            switch (sequenceButtons[stepIndex][noteIndex]) {
                case (u16)~PAD_SQUARE:
                    currentSprite = &spriteSquare;
                    drawnStep = AFANTO02_ANIM_DANCE3;
                    break;
                case (u16)~PAD_TRIANGLE:
                    currentSprite = &spriteTriangle;
                    drawnStep = AFANTO02_ANIM_DANCE;
                    break;
                case (u16)~PAD_CIRCLE:
                    currentSprite = &spriteCircle;
                    drawnStep = AFANTO02_ANIM_DANCE2;
                    break;
                case (u16)~PAD_CROSS:
                    currentSprite = &spriteCross;
                    drawnStep = AFANTO02_ANIM_DANCE4;
                    break;
                case PAD_ALL_RELEASED:
                    currentSprite = 0;
                    drawnStep = AFANTO02_ANIM_DANCE5;
                    break;
            }
            if (currentSprite)
                currentSprite->DrawAt(g_screenLayerBase + 8, drawX, yPos);
            if (noteIndex == beatSlot)
                currentStep = drawnStep;
        }
    }
    spriteFrame.DrawFrame(g_screenLayerBase + 3, noteRight - cellW - (cellW >> 2), noteTop - (cellH >> 1),
                          noteRight - (cellW >> 1) + (cellW >> 2), noteBottom + (cellH >> 1), 0xa0a0a, 1, 0);
    spriteFrameDark.DrawFrame(g_screenLayerBase + 6, noteRight - cellW - (cellW >> 2), noteTop - (cellH >> 1),
                              noteRight - (cellW >> 1) + (cellW >> 2), noteBottom + (cellH >> 1), 0x505050, 1, 0);
    if (drawX > noteRight + (cellW >> 1))
        expected = 0;
    if (previousStep != currentStep) {
        previousStep = currentStep;
        for (j = 0; j < dancerCount; j++) {
            dancers[j]->PushStep(DG_STEP_DANCE, 0, g_dgStepDefaultPos, currentStep);
            dancers[j]->Update();
        }
    }
    if (expected == 0)
        return 1;
    return 0;
}
