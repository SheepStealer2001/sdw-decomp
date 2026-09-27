/*
 * T095 - the original object Wolf_Misc.cpp (guessed name).
 * .text 0x48d3c0-0x491e03 (Wolf_SetFrozenBy .. ScnControllable_IsStickTowardSide), .rdata 0x575930-0x575938
 * (g_sideHeadingTable); no .data (its emitter parameter blocks and put-down offsets are defined by T093, the object
 * before), no .bss.
 * Contents, in address order: the Wolf functions 0x48d3c0-0x491357 (Wolf_SetFrozenBy .. Wolf_Fx0_Start), the Wolf
 * effect helpers 0x491358-0x491bf1, the ScnObject side-quadrant helpers 0x491bf2-0x491d24 and
 * ScnControllable_IsStickTowardSide 0x491d25 (the rest of the ScnControllable code is T096,
 * src/game/scn_controllable.cpp). Uses the shared src/game/wolf.h; the members declared here go through its
 * SDW_EXTRA_ hooks. g_sideHeadingTable is const (it is in .rdata), so its decorated name has the const
 * (?g_sideHeadingTable@@3QBFB).
 *
 * The effect helpers, 0x491358-0x491bf1: the effect start/stop helpers (the prop body +0x2B8 shown with one of the
 * eight prop models and attached to a joint, the splash body +0x3C0 at the water surface; each sets or clears a bit of
 * fxFlags +0x714), the two held-object messages, the active-item helpers (the elastic +0x668), and the fall / jump
 * tests Wolf_IsFalling and Wolf_CanJump. Wolf_Fx0_Start 0x4912c7, the first of the effect helpers, comes before them.
 * WolfStateDesc byte 2 is two 4-bit bitfields (heldMsgArg low, modelSet high), which Wolf_HeldObj_SendMsg15/16 read as
 * `mov cl,[eax+2]; and cl,0xf; xor edx,edx; mov dl,cl` (0x49188c, 0x4918e7).
 *
 * The fxFlags bits as these helpers use them: 1 splash body loaded (Wolf_Init), 2 splash playing, 4 prop 3 (ice block,
 * looped) / 0x100000 prop 4 (Fx3or4_Start), 8 dizzy stars (prop 0), 0x2000 prop 5 at the root joint, 0x4000 prop 6
 * (costume puff), 0x10000 any prop started by FxGeneric_Start, 0x80000 prop 7 on joint 3 (active item), 0x400000
 * prop 8 (Fx8_Start). Several starts refuse while one of the attached effects runs (masks 0x59000c, 0x18200c,
 * 0x19200c).
 */
/* BYTES: dead-code, flow, layout, slot-group, slot-name, temp, view. */
/* BYTES(layout): const because the original has it in .rdata */
#include "sdw_enums.h"
#include "wolf.h"

class Instance;
struct Animator;
#include "../objects/animation.h"
#include "../engine/progress_inventory.h"
#include "wolf_api.h"

/* The trail parameter blocks of Wolf_UpdateTrailFx, 0x57ace4-0x57adb3: defined by T093 (src/game/wolf.cpp), whose .data
 * holds them. */
extern EmitterDriftParams g_wolfDustParams, g_wolfTurnDustParams, g_wolfRocketFxParams, g_wolfLandDustParams,
    g_wolfSkidDustParams, g_wolfBreathParams;
extern EmitterTrailParams g_wolfSnowPrintParams, g_wolfWetPrintParams;
/* 0x575930 - the heading that faces each side quadrant of an object (ScnObject_GetSideHeading) */
extern const s16 g_sideHeadingTable[4] = {1024, 0, 2048, 3072};

/* 0x48d3c0 - message 0xe: frozen (held) by sender; flags 0x40 locks the controls (Wolf_CanControl). */
void Wolf::SetFrozenBy(ScnObject *sender)
{
    grabber = sender;
    flags |= WOLF_FB_FROZEN;
}

/* 0x48d3ee - message 0xf and state entry: released. Flags 0x20000000 swallows the next action press
 * (Wolf_FilterInput). */
void Wolf::Unfreeze()
{
    grabber = 0;
    ClearFlags(WOLF_FB_FROZEN | WOLF_FB_FROZEN_BUT_PROMPT);
    flags |= WOLF_FB_JUST_UNFROZEN;
}

/* 0x48d438 - start the looping state sound unless the one in loopSfxHandle is still playing. The handle goes through a
 * u16 local before Sound_IsPlaying (0x48d459), as in StopLoopSound; the comma keeps the original's single test-and-branch
 * (an early return compiles to an extra jmp). */
/* BYTES(flow): the handle goes through a u16 local (0x48d459) and the comma keeps the original single test-and-branch; an early return adds a jmp */
void Wolf::PlayLoopSound(s16 sfxId)
{
    u16 handle;
    if (!loopSfxHandle || (handle = loopSfxHandle, !Sound_IsPlaying(handle)))
        loopSfxHandle = Sound_Play(sfxId, this, 0xff, SNDF_LOOP | SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 0x1000);
}

/* 0x48d49b - stop the looping state sound. */
/* BYTES(temp): the handle goes through a u16 local, as the original */
void Wolf::StopLoopSound()
{
    u16 handle;
    if (loopSfxHandle) {
        handle = loopSfxHandle;
        Sound_Stop(handle, this);
        loopSfxHandle = 0;
    }
}

/* 0x48d4e1 - the moving loop sound 0x11c while faster than 50. */
void Wolf::UpdateMoveLoopSound()
{
    if (speed > 50)
        PlayLoopSound(SND_SEABOUCL);
    else
        StopLoopSound();
}

/* 0x48d512 - a footstep twice per animation cycle: sound by model set (2: 0x1e, 5: 0x65), in the rabbit costume 0xed
 * once per cycle, in snow with flags 0x8000000 0x78, else 4; pitch random in 12..20 sixteenths. */
/* BYTES(slot-group): locals grouped in work only to pin the original frame offsets; unused fills a gap */
void Wolf::FootstepSound()
{
    struct {
        s32 pitch;
        u16 unused;
        u16 sfx;
        s32 lo;
        s32 hi;
    } work;
    work.sfx = 0;
    if (footstepTimer * 2 >= animDurationTicks) {
        work.lo = 12;
        work.hi = 20;
        switch (modelSet) {
            case WMS_BUSH:
                work.sfx = SND_WOLF_BUSH_STEP;
                break;
            case WMS_GHOSTCOSTUME:
                work.sfx = SND_WOLF_GHOST_STEP;
                break;
            default:
                if (costume == WMS_RABBITCOSTUME) {
                    if (footstepTimer >= animDurationTicks) {
                        work.sfx = SND_SBUSAUT;
                        work.lo = 18;
                        work.hi = 18;
                    }
                } else if ((flags & WOLF_FB_IN_FOOTPRINT_ZONE) && g_weatherType == WEATHER_SNOW)
                    work.sfx = SND_S07NWALK;
                else
                    work.sfx = SND_SCOATTPD;
                break;
        }
        if (work.sfx) {
            work.pitch = (Rand_Range(work.lo, work.hi) << 12) >> 4;
            Sound_Play(work.sfx, this, 0xff, SNDF_POSITIONAL | SNDF_NO_RETRIGGER, work.pitch);
            footstepTimer = 0;
        }
    }
}

/* 0x48d627 - the first eight steps of a sneak: sound 0x48 twice per cycle, fading out (volume (8-n)*255/8) and rising
 * (pitch 0x800 + n*0x100). */
/* BYTES(slot-group): locals grouped in work only to pin the original frame offsets; unused fills a gap */
void Wolf::SneakStepSound()
{
    struct {
        s32 pitch;
        s16 unused;
        s16 volume;
    } work;
    if (stepSoundCount < 8 && footstepTimer * 2 >= animDurationTicks) {
        work.pitch = (stepSoundCount << 11) / 8 + 0x800;
        work.volume = ((8 - stepSoundCount) * 0xff) / 8;
        loopSfxHandle = Sound_Play(SND_SCOSTEP1, this, work.volume, SNDF_POSITIONAL | SNDF_NO_RETRIGGER, work.pitch);
        footstepTimer = 0;
        stepSoundCount++;
    }
}

/* 0x48d6eb - running: the start sound 0xc4 once, then the loop 199. */
void Wolf::RunSound(s32 isStart)
{
    if (isStart && stepSoundCount == 0) {
        loopSfxHandle = Sound_Play(SND_SCOSRUN1, this, 0xff, SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 0x1000);
        stepSoundCount = 1;
    } else
        PlayLoopSound(SND_SCOSRUN4);
}

/* a colour word whose channel bytes are also read and written one by one */
union ColorBytes {
    u32 value;
    u8 c[4];
};

/* 0x48d74e - message 0x74: draw Ralph tinted with the caller's colour (halved and at full amount while fxFlags 0x20000),
 * then the prop body while fxFlags & 0x4008 (mirrored in pitch with 0x4000), then the object flying to the inventory. */
/* BYTES(slot-group): locals grouped in work only to pin the original frame offsets */
void Wolf::Msg74_Draw(DrawMsgArgs *args)
{
    struct {
        ScnObject *flyObj;
        Vec3s rot;
        ColorBytes color;
    } work;
    if (fxFlags & WOLF_FA_BURNT) {
        work.color.value = args->color;
        work.color.c[0] >>= 1;
        work.color.c[1] >>= 1;
        work.color.c[2] >>= 1;
        RenderTinted(args->view, work.color.value, 0x1000, args->mode);
    } else
        RenderTinted(args->view, args->color, args->amount, args->mode);
    if (fxFlags & (WOLF_FA_PROP0 | WOLF_FA_PROP6_BILLBOARD)) {
        if (fxFlags & WOLF_FA_PROP6_BILLBOARD) {
            work.rot = propBody.rot;
            work.rot.x = -work.rot.x & 0xfff;
            propBody.rot = work.rot;
        }
        propBody.RenderTinted(args->view, args->color, args->amount, args->mode);
    }
    work.flyObj = ItemFly_GetObject();
    if (work.flyObj)
        /* cast kept: a downcast: the item flying to the inventory is a ScnBody */
        ((ScnBody *)work.flyObj)->RenderTinted(args->view, args->color, args->amount, args->mode);
}

/* 0x48d894 - show prop model propIndex in the prop body at Ralph's position and facing, playing animId. Returns 0 when
 * the slot has no model. */
s32 Wolf::ShowProp(s32 propIndex, u16 animId, s32 loop)
{
    if (propModels[propIndex].IsValid()) {
        propBody.SwapModel(&propModels[propIndex]);
        propBody.pos = pos;
        propBody.rot = rot;
        propBody.PlayAnim(animId, loop, 0);
        return 1;
    }
    return 0;
}

/* 0x48d973 - state 0x4f (held): count the action presses; fewer than 10 within 0x2000 ticks start the count again. */
void Wolf::MashCounterStep()
{
    if (padBits & WOLF_ACT_ACTION_EDGE)
        mashCount++;
    if (mashCount < 10 && runTapTimer >= 0x2000) {
        mashCount = 0;
        runTapTimer = 0;
    }
}

/* 0x48d9dd - ask obj (message 0x5b) how to square a heading to it: reply 0 rounds to the nearest quarter turn, 1 to the
 * nearer of 0 / 0x800, 2 to 0x400 for headings in 0..0x7ff and 0xc00 for the other half; any other reply leaves it. */
s16 Wolf::SnapHeadingToObject(ScnObject *obj, s16 heading)
{
    s32 mode;
    mode = obj->HandleMessage(this, MSG_QUERY_PUSH_AXIS, 0);
    switch (mode) {
        case PUSH_AXIS_FREE:
            heading = (heading + 0x200) & 0xfff;
            heading = heading - (heading & 0x3ff);
            break;
        case PUSH_AXIS_X_LOCKED:
            heading = (heading + 0x400) & 0xfff;
            heading = heading - (heading & 0x7ff);
            break;
        case PUSH_AXIS_Z_LOCKED:
            heading = (heading + 0x800) & 0xfff;
            heading = (heading - 0x400 - (heading & 0x7ff)) & 0xfff;
            break;
    }
    return heading;
}

/* 0x48daa8 - resize Ralph (0x400 = 1.0): the wade/swim offset (90 at full size), the collision box from its saved
 * copy scaled, the shadow radius from the box, then snap to the ground. */
void Wolf::SetScale(s16 newScale)
{
    CollBox *box;
    if (newScale != scale) {
        scale = newScale;
        waterDepthOffset = (scale * 90) / 1024;
        box = GetFirstModelBox();
        *box = *(CollBox *)savedBodyBox; /* cast kept: Box and CollBox are two views of one 16-byte record */
        ScaleVec(&box->min);
        ScaleVec(&box->max);
        SetShadowRadius((box->max.x * 3) / 2);
        SnapToGround(1);
    }
}

/* 0x48db86 - scale a vector by Ralph's size (rounding toward zero). */
void Wolf::ScaleVec(Vec3s *v)
{
    if (scale != 0x400) {
        v->x = (v->x * scale) / 1024;
        v->y = (v->y * scale) / 1024;
        v->z = (v->z * scale) / 1024;
    }
}

/* 0x48dc18 - the fall animation (mode 0): the state's own, 0x4d with flags 0x2000000, or 1 (falling into water) when
 * already playing 1 or more than 200 above the ground, over a water zone. */
/* BYTES(slot-group): locals grouped in work only to pin the original frame offsets; unused fills a gap */
void Wolf::UpdateFallAnim()
{
    struct {
        WolfStateDesc *desc;
        u16 unused;
        u16 anim;
    } work;
    if (!mode) {
        work.desc = &g_wolfMoveBank0[mode].states[state];
        work.anim = work.desc->anim;
        if (flags & WOLF_FB_FORCE_FALL)
            work.anim = ACOYOT01_ANIM_SEESAW;
        else if (AnimId() == ACOYOT01_ANIM_DIVE1 || GroundY() - pos.y > 200) {
            if (Zones_Get(ZONE_WATER)->FindContaining(&shadow.groundPos))
                work.anim = ACOYOT01_ANIM_DIVE1;
        }
        if (AnimId() != work.anim)
            PlayAnim(work.anim, 1, 1);
    }
}

/* 0x48dd48 - the first enabled climbable box (flags 0x10) of the context object's model, when the context action is
 * 0xf; else 0. */
/* BYTES(slot-group): locals grouped in work only to pin the original frame offsets */
CollBox *Wolf::FindClimbBoxOnTarget()
{
    struct {
        ScnObject *obj;
        ModelBoxList *list;
        CollBox *box;
        u32 count;
    } work;
    if (ctxAction.action == CTX_CLIMB && ctxAction.target) {
        work.obj = ctxAction.target;
        work.list = work.obj->inst_model->boxes;
        if (!work.list) {
            work.count = 0;
            work.box = 0;
        } else {
            work.count = work.list->count;
            work.box = work.list->boxes;
        }
        while (work.count > 0) {
            if (!(work.box->flags & COLLBOX_NONSOLID) && (work.box->flags & COLLBOX_CYLINDER))
                return work.box;
            work.count--;
            work.box++;
        }
    }
    return 0;
}

/* 0x48dde9 - message 0x6a to everything riding Ralph. */
void Wolf::NotifyAttached_6A()
{
    s32 i;
    i = 0;
    while (i < riderCount) {
        riders[i]->HandleMessage(this, MSG_WOLF_RIDER_NOTIFY, 0);
        i++;
    }
}

/* 0x48de3e - death without a death state: dead flag, riders told, restart fade after delayTicks. */
void Wolf::RestartNoAnim(s32 delayTicks)
{
    flags |= WOLF_FB_DEAD;
    NotifyAttached_6A();
    Fade_StartRestart(delayTicks);
}

/* 0x48de74 - die into deathState (once): riders told, the held object dropped (dropFlags & 1: back to mode 0 first),
 * dead flag, restart fade after the death animation plus extraDelay. */
void Wolf::Die(u8 deathState, u32 dropFlags, s32 extraDelay)
{
    if (state != deathState) {
        NotifyAttached_6A();
        DropHeld(dropFlags);
        if (dropFlags & WOLF_DROP_RELEASE)
            SetMode(WOLF_MODE_NORMAL, deathState);
        else
            SetState(deathState);
        flags |= WOLF_FB_DEAD;
        Fade_StartRestart(animDurationTicks + extraDelay);
    }
}

/* 0x48def7 - the run tap: a second run press within bank params[5]..params[6] ticks of the first sets flags 4 (run),
 * an earlier one clears it; the timer restarts either way, and past params[6] the run is dropped. */
void Wolf::RunTapCheck(s32 playStartSound)
{
    WolfMoveBank *bank;
    bank = &g_wolfMoveBank0[mode];
    if (runTapTimer > bank->params[6]) {
        ClearFlags(WOLF_FB_RUN);
        runTapTimer = 0;
    } else if (padBits & WOLF_ACT_RUN) {
        if (runTapTimer >= bank->params[5])
            flags |= WOLF_FB_RUN;
        else
            ClearFlags(WOLF_FB_RUN);
        runTapTimer = 0;
    }
    RunSound(playStartSound);
}

/* 0x48dfc5 - whether the inventory may be used: not dead/frozen (flags 0x43), no transition/pause (g_gameFlags 0xe01),
 * menus allowed, map closed, normal camera, and SelectMenu_HasItems. */
/* BYTES(view, inferred): g_animDt is the first field of the GameState block; the call reinterprets its address until that block is one object */
s32 Wolf::CanUseInventory()
{
    /* cast kept: GameState is a view over the globals from g_animDt */
    return !(flags & (WOLF_FB_DEAD | WOLF_FB_WON | WOLF_FB_FROZEN)) &&
           !(g_gameFlags & (GF_BIT0 | GF_FADE_RESTART | GF_FADE_IN | GF_FADE_EXIT)) &&
           ((GameState *)&g_animDt)->Game_CanOpenMenu() && !Map_IsOpen() && !g_camDebugMode && SelectMenu_HasItems();
}

/* 0x48e030 - whether the stick and buttons drive Ralph. */
s32 Wolf::CanControl()
{
    return !(g_gameFlags & GF_ITEM_WHEEL_OPEN) && !(flags & (WOLF_FB_DEAD | WOLF_FB_WON | WOLF_FB_FROZEN)) &&
           !(g_gameFlags & (GF_BIT0 | GF_FADE_RESTART | GF_FADE_EXIT)) && !Map_IsOpen() && !g_camDebugMode;
}

/* 0x48e090 - read the pad into the stick and action bits, then mask them: no run (4) in a RESTRICTIONBOX with flag
 * 0x40000000, nor with flags 0x40000; in the rabbit costume neither run nor 0x10; no action (0x40) once after a
 * release (flags 0x20000000). */
/* BYTES(slot-group): locals grouped in work only to pin the original frame offsets */
void Wolf::FilterInput(Pad *pad, u32 inputMask)
{
    struct {
        s32 canControl;
        Box *box;
    } work;
    work.canControl = CanControl();
    ReadPad(pad, g_camMode != CAM_LOOK && work.canControl, work.canControl || CanUseInventory());
    work.box = restrictions.FindContaining(&pos);
    if (work.box && (work.box->flags & RESTRICT_F_NO_RUN))
        inputMask &= ~WOLF_ACT_RUN;
    if (costume == WMS_RABBITCOSTUME)
        inputMask &= ~(WOLF_ACT_RUN | WOLF_ACT_SNEAK_HELD);
    if (flags & WOLF_FB_SPECIAL_OBJECT)
        inputMask &= ~WOLF_ACT_RUN;
    if (flags & WOLF_FB_JUST_UNFROZEN)
        inputMask &= ~WOLF_ACT_ACTION_EDGE;
    padBits &= inputMask;
}

/* 0x48e1b1 - stop every effect: landing dust, the effect flags 0x2687f0, the 0x10000 effect, the three emitters, the
 * 0x400000 effect body. */
void Wolf::ClearEffects()
{
    landDustTimer = 0;
    ClearFxFlags(WOLF_FA_TRAIL_RUN | WOLF_FA_TRAIL_TURN | WOLF_FA_TRAIL_ROCKET | WOLF_FA_TRAIL_LAND |
                 WOLF_FA_TRAIL_SKID | WOLF_FA_BUBBLES | WOLF_FA_RIPPLES | WOLF_FA_FOOTPRINTS | WOLF_FA_BURNT |
                 WOLF_FA_BREATH | WOLF_FA_STATE_DUST);
    if (fxFlags & WOLF_FA_DEATHZONE_PROP)
        FxGeneric_Stop();
    if (trailA.base.flags.active)
        trailA.base.Emitter_Reset();
    if (particles.flags.active)
        particles.Emitter_Reset();
    if (trailB.base.flags.active)
        trailB.base.Emitter_Reset();
    if (fxFlags & WOLF_FA_PROP8)
        Fx8_Stop();
}

/* 0x48e27f - the trail effects, once per frame. trailA shows at most one of, in this priority: bubbles while
 * underwater (fxFlags 0x200), dust of states with flag 0x40000000 (0x200000), run dust (state flag 0x40; 0x10), rocket
 * exhaust (state flag 0x20; 0x40), skid (fxFlags 0x800; 0x100), sharp turn (0x1000; 0x20), landing dust while
 * landDustTimer runs (0x80), cold breath in snow (state flag 2, one second in four; 0x40000). An effect keeps being
 * updated after its trigger ends until its last particle dies (its fxFlags bit). With flags 8 none of the masked ones
 * spawns. Then trailB: wake rings while wading (0x400); then the particle emitter: footprints (0x8000), snow prints
 * (permanent) or wet prints while < 0x14000 ticks out of the water. */
/* BYTES(slot-group): locals grouped in work only to pin the original frame offsets */
/* BYTES(view, inferred): work.off is passed as the Vec3s it starts with */
void Wolf::UpdateTrailFx()
{
    struct {
        s32 fwdY2;
        s32 fwdX;
        s32 emit;
        s32 mask;
        Mat34s mat;
        Vec4s off;
        Vec3s p;
        s32 div;
        WolfStateDesc *desc;
    } work;
    work.desc = &g_wolfMoveBank0[mode].states[state];
    work.p = pos;
    work.emit = waterZone && !(work.desc->flags & WSF_ICE_BLOCK) && pos.y >= waterZone->min[1] + waterDepthOffset + 50;
    if (work.emit | (fxFlags & WOLF_FA_BUBBLES)) {
        if (!(fxFlags & WOLF_FA_BUBBLES))
            trailA.base.Emitter_Reset();
        if (!(speed | velocity.y) || !Rand_Bounded(4)) {
            work.p.y += (s16)Rand_Range(-150, -125);
            work.p.x += (s16)Rand_Range(-25, 25);
            work.p.z += (s16)Rand_Range(-25, 25);
        } else {
            work.off.z = 100;
            work.off.y = 0;
            work.off.x = 0;
            Mat34s_FromEulerScaled(&rot, &work.mat, 0);
            /* cast kept: the input is the xyz of the Vec4s it writes back */
            Mat34s_TransformVec3s(&work.mat, (Vec3s *)&work.off, &work.off);
            work.p.y += (s16)(work.off.y + Rand_Range(-60, -40));
            work.p.x += (s16)(work.off.x + Rand_Range(-15, 15));
            work.p.z += (s16)(work.off.z + Rand_Range(-15, 15));
        }
        if (waterZone)
            g_wolfBubbleParams.cap = waterZone->min[1];
        g_wolfBubbleParams.size = (s16)Rand_Range(10, 20);
        if (trailA.base.Emitter_UpdateRiseToCap(&g_wolfBubbleParams, &work.p, work.emit))
            fxFlags |= WOLF_FA_BUBBLES;
        else
            ClearFxFlags(WOLF_FA_BUBBLES);
        landDustTimer = 0;
    } else {
        if (flags & WOLF_FB_IN_WATER)
            work.mask = 0;
        else
            work.mask = -1;
        work.fwdX = -g_sinTable4096[Facing()];
        work.fwdY2 = -g_pCosTable[Facing()];
        work.emit = work.desc->flags & WSF_SMOKE_TRAIL & work.mask;
        if (work.emit | (fxFlags & WOLF_FA_STATE_DUST)) {
            if (!(fxFlags & WOLF_FA_STATE_DUST))
                trailA.base.Emitter_Reset();
            work.p.y -= (s16)(g_wolfDustParams.sizeStart >> 1);
            if (speed < 50)
                work.div = 50;
            else
                work.div = speed;
            g_wolfDustParams.spawnInterval = (g_wolfMoveBank0[0].profiles[surface][2].maxSpeed * 0xa3) / work.div;
            if (trailA.base.Emitter_UpdateDrift(&g_wolfDustParams, &work.p, Facing(), work.emit))
                fxFlags |= WOLF_FA_STATE_DUST;
            else
                ClearFxFlags(WOLF_FA_STATE_DUST);
            landDustTimer = 0;
        } else {
            work.emit = work.desc->flags & WSF_RUN_COLLISION & work.mask;
            if (work.emit | (fxFlags & WOLF_FA_TRAIL_RUN)) {
                if (!(fxFlags & WOLF_FA_TRAIL_RUN))
                    trailA.base.Emitter_Reset();
                work.p.y -= (s16)(g_wolfDustParams.sizeStart >> 1);
                if (speed < 50)
                    work.div = 50;
                else
                    work.div = speed;
                g_wolfDustParams.spawnInterval = (g_wolfMoveBank0[0].profiles[surface][2].maxSpeed * 0xa3) / work.div;
                if (trailA.base.Emitter_UpdateDrift(&g_wolfDustParams, &work.p, Facing(), work.emit))
                    fxFlags |= WOLF_FA_TRAIL_RUN;
                else
                    ClearFxFlags(WOLF_FA_TRAIL_RUN);
                landDustTimer = 0;
            } else {
                work.emit = work.desc->flags & WSF_NO_DISTANCE_COUNT;
                if (work.emit | (fxFlags & WOLF_FA_TRAIL_ROCKET)) {
                    if (!(fxFlags & WOLF_FA_TRAIL_ROCKET))
                        trailA.base.Emitter_Reset();
                    work.p.x -= (s16)((work.fwdX << 5) >> 12);
                    work.p.z -= (s16)((work.fwdY2 << 5) >> 12);
                    work.p.y -= 10;
                    if (speed < 50)
                        work.div = 50;
                    else
                        work.div = speed;
                    g_wolfRocketFxParams.spawnInterval =
                        (g_wolfMoveBank0[0].profiles[surface][9].maxSpeed * 0xcc) / work.div;
                    if (trailA.base.Emitter_UpdateDrift(&g_wolfRocketFxParams, &work.p, (Facing() + 0x800) & 0xfff,
                                                        work.emit))
                        fxFlags |= WOLF_FA_TRAIL_ROCKET;
                    else
                        ClearFxFlags(WOLF_FA_TRAIL_ROCKET);
                    landDustTimer = 0;
                } else {
                    work.emit = fxFlags & WOLF_FA_REQ_SKID & work.mask;
                    if (work.emit | (fxFlags & WOLF_FA_TRAIL_SKID)) {
                        if (!(fxFlags & WOLF_FA_TRAIL_SKID))
                            trailA.base.Emitter_Reset();
                        work.p.x -= (s16)((work.fwdX << 5) >> 12);
                        work.p.z -= (s16)((work.fwdY2 << 5) >> 12);
                        work.p.y -= (s16)(g_wolfSkidDustParams.sizeStart >> 1);
                        if (trailA.base.Emitter_UpdateDrift(&g_wolfSkidDustParams, &work.p, (Facing() + 0x800) & 0xfff,
                                                            work.emit))
                            fxFlags |= WOLF_FA_TRAIL_SKID;
                        else
                            ClearFxFlags(WOLF_FA_TRAIL_SKID);
                        landDustTimer = 0;
                    } else {
                        work.emit = fxFlags & WOLF_FA_REQ_TURN & work.mask;
                        if (work.emit | (fxFlags & WOLF_FA_TRAIL_TURN)) {
                            if (!(fxFlags & WOLF_FA_TRAIL_TURN))
                                trailA.base.Emitter_Reset();
                            work.p.y -= (s16)(g_wolfTurnDustParams.sizeStart >> 1);
                            if (trailA.base.Emitter_UpdateDrift(&g_wolfTurnDustParams, &work.p,
                                                                (wantedDir + 0x800) & 0xfff, work.emit))
                                fxFlags |= WOLF_FA_TRAIL_TURN;
                            else
                                ClearFxFlags(WOLF_FA_TRAIL_TURN);
                            landDustTimer = 0;
                        } else {
                            work.emit = (landDustTimer > 0) & work.mask;
                            if (work.emit | (fxFlags & WOLF_FA_TRAIL_LAND)) {
                                if (!(fxFlags & WOLF_FA_TRAIL_LAND))
                                    trailA.base.Emitter_Reset();
                                work.p.y -= (s16)(g_wolfLandDustParams.sizeStart >> 1);
                                if (trailA.base.Emitter_UpdateDrift(&g_wolfLandDustParams, &work.p,
                                                                    (wantedDir + 0x800) & 0xfff, work.emit))
                                    fxFlags |= WOLF_FA_TRAIL_LAND;
                                else
                                    ClearFxFlags(WOLF_FA_TRAIL_LAND);
                                landDustTimer -= g_dt;
                                if (landDustTimer < 0)
                                    landDustTimer = 0;
                            } else {
                                work.emit = (work.desc->flags & WSF_STATIONARY) && g_weatherType == WEATHER_SNOW &&
                                            ((stateTime / 2048) & 3) == 0;
                                if (work.emit | (fxFlags & WOLF_FA_BREATH)) {
                                    if (!(fxFlags & WOLF_FA_BREATH))
                                        trailA.base.Emitter_Reset();
                                    if (modelSet == WMS_SHEEPCOSTUME) {
                                        work.p.y -= 40;
                                        work.p.x += (s16)((work.fwdX * 70) >> 12);
                                        work.p.z += (s16)((work.fwdY2 * 70) >> 12);
                                    } else {
                                        work.p.y -= 100;
                                        work.p.x += (s16)((work.fwdX * 20) >> 12);
                                        work.p.z += (s16)((work.fwdY2 * 20) >> 12);
                                    }
                                    if (trailA.base.Emitter_UpdateDrift(&g_wolfBreathParams, &work.p, Facing(),
                                                                        work.emit))
                                        fxFlags |= WOLF_FA_BREATH;
                                    else
                                        ClearFxFlags(WOLF_FA_BREATH);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    work.p = pos;
    work.emit = waterZone && !(work.desc->flags & WSF_ICE_BLOCK) && pos.y >= waterZone->min[1] &&
                pos.y < waterZone->min[1] + waterDepthOffset + 50;
    if (work.emit | (fxFlags & WOLF_FA_RIPPLES)) {
        if (!(fxFlags & WOLF_FA_RIPPLES))
            trailB.base.Emitter_Reset();
        if (waterZone)
            work.p.y = waterZone->min[1];
        if (speed > 600)
            work.div = 600;
        else if (speed < 20)
            work.div = Rand_Range(10, 50);
        else
            work.div = speed;
        g_wolfWakeParams.spawnInterval = 0x4b000 / work.div;
        if (trailB.base.Emitter_UpdateFade(&g_wolfWakeParams, &work.p, Facing(), work.emit))
            fxFlags |= WOLF_FA_RIPPLES;
        else
            ClearFxFlags(WOLF_FA_RIPPLES);
        landDustTimer = 0;
    }
    work.p = pos;
    work.emit = (flags & WOLF_FB_IN_FOOTPRINT_ZONE) && !(flags & WOLF_FB_RECENT_OBJECT_CONTACT) &&
                (work.desc->flags & WSF_GROUND_STATE) && airTime == 0 && groundNormal.y < -3726 &&
                GroundY() - pos.y < 20 && modelSet != WMS_SHEEPCOSTUME && !(fxFlags & WOLF_FA_REQ_SKID);
    if (work.emit | (fxFlags & WOLF_FA_FOOTPRINTS)) {
        if (!(fxFlags & WOLF_FA_FOOTPRINTS))
            particles.Emitter_Reset();
        work.p.y -= 10;
        if (g_weatherType != WEATHER_SNOW) {
            work.emit = work.emit && timeOutOfWater > 0 && timeOutOfWater < 0x14000;
            if (particles.Emitter_UpdateTrail(&g_wolfWetPrintParams, &work.p, (speed * g_dt) >> 12, Facing(),
                                              work.emit))
                fxFlags |= WOLF_FA_FOOTPRINTS;
            else
                ClearFxFlags(WOLF_FA_FOOTPRINTS);
        } else {
            if (particles.Emitter_UpdateTrail(&g_wolfSnowPrintParams, &work.p, (speed * g_dt) >> 12, Facing(),
                                              work.emit))
                fxFlags |= WOLF_FA_FOOTPRINTS;
            else
                ClearFxFlags(WOLF_FA_FOOTPRINTS);
        }
    }
}

/* 0x48f120 - take obj in hand at once (message 4 with 0x12, or 3 for a rocket), only with nothing held, in the plain
 * model set and on foot; a shrunk Ralph (scale < 0x400) is killed instead (state 0x10). */
/* BYTES(slot-group): locals grouped in work only to pin the original frame offsets */
void Wolf::EquipItem(ScnObject *obj)
{
    struct {
        s32 msg;
        s32 accepted;
    } work;
    if (!heldObject && obj && !modelSet && !mode) {
        if (scale < 0x400) {
            Die(WOLF_ST_DIE_CRUSHED, WOLF_DROP_STOP, 0x2000);
            killer = 0;
        } else {
            obj->HandleMessage(this, MSG_INVENTORY_TAKE_OUT, 0);
            obj->AddToWorld(0);
            switch (IsRocket(obj)) {
                case 0:
                    work.msg = 0x12;
                    break;
                case 1:
                    work.msg = 3;
                    break;
            }
            /* cast kept: arg carries a number */
            work.accepted = obj->HandleMessage(this, MSG_PICKUP, (void *)work.msg);
            if (work.accepted)
                heldObject = obj;
        }
    }
}

/* 0x48f212 - whether the held item may be changed now: on foot (mode 0), the state allows it (flags 0x2040000 clear),
 * Wolf flags 0x43 and game flags 0xe01 clear, and - when needStateFlag - the state has flag 0x100 (the quick-inventory
 * states, which include the airborne ones). */
s32 Wolf::CanSwapItem(s32 needStateFlag)
{
    WolfStateDesc *desc = &g_wolfMoveBank0[mode].states[state];
    return mode == WOLF_MODE_NORMAL && !(desc->flags & (WSF_ITEM_IN_USE | WSF_UNTOUCHABLE)) &&
           !(flags & (WOLF_FB_DEAD | WOLF_FB_WON | WOLF_FB_FROZEN)) &&
           !(g_gameFlags & (GF_BIT0 | GF_FADE_RESTART | GF_FADE_IN | GF_FADE_EXIT)) &&
           (!needStateFlag || (desc->flags & WSF_ITEM_WHEEL_OK));
}

/* 0x48f2a8 - make the inventory selection the held item. The old item is put down at the Wolf (message 5, then 7) and
 * taken out of the world, flying into the inventory when animated; the three costumes (sheep, rabbit, ghost) are put
 * on through a state (model sets 3/4/5) instead of held; an ordinary item is equipped at once or, animated, flies out
 * of the inventory (state 0x2b). The state only changes when its flags 0x8c004 are clear. Choosing empty hands while
 * wearing a costume takes it off (states 0x90/0xa2/0xc2). Returns whether a state was entered. The reply to message 5
 * is not used: the item leaves the hand whatever it answers.
 * Stowing an item enters state 0x2c from whatever state allowed the swap. */
/* BYTES(slot-name): names chosen for their stack slots: result -4, putDown -0xc, origin -0x14, stateDesc -0x18, choice -0x1c, free -0x20, reply -0x24, from2 -0x2c */
/* BYTES(dead-code): reply is stored and never read, as in the original */
s32 Wolf::SwapHeldItem(s32 animated, s32 needStateFlag)
{
    Vec3s from2; /* the names order the frame: result -4, putDown -0xc, origin -0x14, stateDesc -0x18, */
    s32 reply;   /* choice -0x1c, free -0x20, reply -0x24, from2 -0x2c */
    s32 free;
    ScnObject *choice;
    WolfStateDesc *stateDesc;
    Vec3s origin;
    DropMsgArg putDown;
    s32 result;
    result = 0;
    if (CanSwapItem(needStateFlag)) {
        choice = Inventory_GetSelectedObject();
        stateDesc = &g_wolfMoveBank0[mode].states[state];
        free = !(stateDesc->flags & (WSF_BUSY_OR_QUIET | WSF_IN_WATER | WSF_KEEP_COVER | WSF_AIRBORNE));
        if (choice != heldObject) {
            if (heldObject) {
                putDown.pos = pos;
                putDown.placed = 0;
                putDown.flag1 = 0;
                reply = heldObject->HandleMessage(this, MSG_DROP, &putDown);
                heldObject->HandleMessage(this, MSG_INVENTORY_STORED, 0);
                heldObject->RemoveFromWorld();
                if (animated && !choice) {
                    origin.x = g_wolfEquipPreviewOffset.x + pos.x;
                    origin.y = g_wolfEquipPreviewOffset.y + pos.y;
                    origin.z = g_wolfEquipPreviewOffset.z + pos.z;
                    ItemFly_Start(heldObject, ITEMFLY_TO_INVENTORY, &origin);
                }
                heldObject = 0;
                if (free) {
                    if (!choice)
                        SetState(WOLF_ST_ITEM_STOW);
                    else
                        SetIdleState();
                    result = 1;
                }
            }
            if (choice && !modelSet) {
                switch (choice->GetClassId()) {
                    case CLASSID_SHEEPCOSTUME:
                        if (modelSet != WMS_SHEEPCOSTUME && !(flags & WOLF_FB_SPECIAL_OBJECT)) {
                            SetState(WOLF_ST_SHEEPCOSTUME_PUT_ON);
                            Fx6_Start(1);
                            result = 1;
                        }
                        break;
                    case CLASSID_RABBITCOSTUME:
                        if (modelSet != WMS_RABBITCOSTUME && !(flags & WOLF_FB_SPECIAL_OBJECT)) {
                            SetState(WOLF_ST_RABBITCOSTUME_PUT_ON);
                            Fx6_Start(1);
                            result = 1;
                        }
                        break;
                    case CLASSID_GHOSTCOSTUME:
                        if (modelSet != WMS_GHOSTCOSTUME && !(flags & WOLF_FB_SPECIAL_OBJECT)) {
                            SetState(WOLF_ST_GHOSTCOSTUME_PUT_ON);
                            Fx6_Start(1);
                            result = 1;
                        }
                        break;
                    default:
                        if (animated) {
                            from2.x = g_wolfEquipPreviewOffset.x + pos.x;
                            from2.y = g_wolfEquipPreviewOffset.y + pos.y;
                            from2.z = g_wolfEquipPreviewOffset.z + pos.z;
                            ItemFly_Start(choice, ITEMFLY_TO_WOLF, &from2);
                            flags |= WOLF_FB_EQUIP_PENDING;
                            if (free) {
                                SetState(WOLF_ST_ITEM_TAKE_OUT);
                                result = 1;
                            }
                        } else {
                            EquipItem(choice);
                            if (free) {
                                SetIdleState();
                                result = 1;
                            }
                        }
                }
            }
        } else if (!choice) {
            if (modelSet == WMS_SHEEPCOSTUME) {
                result = 1;
                SetState(WOLF_ST_SHEEPCOSTUME_TAKE_OFF);
                Fx6_Start(1);
            }
            if (modelSet == WMS_RABBITCOSTUME) {
                result = 1;
                SetState(WOLF_ST_RABBITCOSTUME_TAKE_OFF);
                Fx6_Start(1);
            }
            if (modelSet == WMS_GHOSTCOSTUME) {
                result = 1;
                SetState(WOLF_ST_GHOSTCOSTUME_TAKE_OFF);
                Fx6_Start(1);
            }
        }
    }
    return result;
}

/* 0x48f69a - the item wheel, every frame from Wolf_Update. On foot: the inventory button's press edge opens the wheel
 * when the state has flag 0x100 and Wolf_CanUseInventory; while it is open, left/right turn it (not while it is still
 * rotating) and releasing the button closes it and returns 1 (the caller then calls Wolf_SwapHeldItem(1, 1)); losing
 * the permission closes it with 1 as well. Carrying (mode 1) just closes it. */
/* BYTES(slot-name): names chosen for their stack slots: result -4, allowed -8, desc -0xc, wheelOpen -0x10 */
s32 Wolf::QuickInventory(Pad *pad)
{
    u32 wheelOpen;
    WolfStateDesc *desc;
    s32 allowed;
    s32 result; /* the names order the frame: result -4, allowed -8, desc -0xc, wheelOpen -0x10 */
    result = 0;
    switch (mode) {
        case WOLF_MODE_NORMAL:
            desc = &g_wolfMoveBank0[mode].states[state];
            allowed = (desc->flags & WSF_ITEM_WHEEL_OK) && CanUseInventory();
            wheelOpen = g_gameFlags & GF_ITEM_WHEEL_OPEN;
            if (wheelOpen) {
                if (allowed) {
                    if (!InvWheel_IsRotating()) {
                        if (Pad_MenuHeld((u16)~PAD_LEFT))
                            Inventory_SelectPrev();
                        else if (Pad_MenuHeld((u16)~PAD_RIGHT))
                            Inventory_SelectNext();
                    }
                    if (pad->cur.buttons & ~g_padMasks[6]) {
                        Inventory_SetWheelOpen(0);
                        result = 1;
                    }
                } else {
                    Inventory_SetWheelOpen(0);
                    result = 1;
                }
            } else if (!(pad->cur.buttons & ~g_padMasks[6]) && (pad->prev.buttons & ~g_padMasks[6]) && allowed &&
                       !(g_gameFlags & GF_ITEM_FLY)) {
                Inventory_ClearSelection();
                Inventory_SetWheelOpen(1);
            }
            break;
        case WOLF_MODE_CARRY:
            Inventory_SetWheelOpen(0);
    }
    return result;
}

/* 0x48f81e - fill the camera request for this frame (unless the object holding the Wolf answers message 0x49, or the
 * debug camera runs): target = position, aim = bounding centre, velocity, yaw behind the Wolf, speed factor. The mode
 * comes from camOverride or the state descriptor (>= 13: mode 5 with that sub-mode). Changing mode goes through
 * Camera_SetMode; returning from mode 6 or a scripted mode restores the camera saved in savedCamRot/savedCamDist. Per
 * mode: 0 follow (the view button's press edge enters look mode 1), 1 look (aim 30 higher; close up and turned by the
 * player, Ralph faces where the camera looks; releasing the button leaves it), 3 rocket (half the Wolf's pitch and
 * roll, distance from speed, lower in FLYBOX's bottom 600 units), 5 views by sub-mode 13..20. In modes 0-5 the camera
 * is saved every frame the mode did not change. */
/* BYTES(slot-name): names chosen for their stack slots: go -4, req -5, newMode -6, switched -0xc, bringBack -0x10, doSave -0x14, stateD -0x18, allowLook -0x1c, height -0x20 */
void Wolf::UpdateCamera(Pad *pad)
{
    s32 height;    /* the names order the frame: go -4, req -5, newMode -6, switched -0xc, bringBack -0x10, */
    s32 allowLook; /* doSave -0x14, stateD -0x18, allowLook -0x1c, height -0x20 */
    WolfStateDesc *stateD;
    s32 doSave;
    s32 bringBack;
    s32 switched;
    u8 newMode;
    u8 req;
    s32 go;
    if (grabber)
        go = !grabber->HandleMessage(this, MSG_GRABBER_CAMERA, pad);
    else
        go = 1;
    if (go && !g_camDebugMode) {
        stateD = &g_wolfMoveBank0[mode].states[state];
        allowLook = (stateD->flags & WSF_LOOK_MODE_OK) && !(flags & WOLF_FB_NO_LOOK) &&
                    ((flags & WOLF_FB_LOOK_ALLOWED) || CanControl());
        g_camReqTarget = pos;
        g_camReqAimOffset = boundCenter;
        g_camReqVel = velocity;
        g_camReqRot.x = 0;
        g_camReqRot.y = (-Facing() + 0x800) & 0xfff;
        g_camReqRot.z = 0;
        g_camReqDist = 0;
        g_camReqFocal = 0;
        if (speed < 0x200)
            g_camReqSpeedFactor = speed * 0x1000 / 0x200;
        else
            g_camReqSpeedFactor = 0x1000;
        if ((stateD->flags & WSF_CAM_LEDGE_PROBE) && !(flags & WOLF_FB_RECENT_OBJECT_CONTACT))
            g_camReqFlags.ledgeProbe = 1;
        else
            g_camReqFlags.ledgeProbe = 0;
        if (flags & WOLF_FB_SEESAW_CAM)
            g_camReqFlags.snapYaw = 1;
        else
            g_camReqFlags.snapYaw = 0;
        g_camReqFlags.usePitch = 0;
        g_camReqFlags.useYaw = 0;
        g_camReqFlags.padRotate = 1;
        g_camReqFlags.restriction = 0;
        if (camOverride == CAM_REQ_NONE)
            req = stateD->camMode;
        else
            req = camOverride;
        if (req >= CAM_REQ_DIR_BEHIND)
            newMode = CAM_DIRECTED;
        else
            newMode = req;
        if (newMode != g_camMode) {
            bringBack = 0;
            switch (g_camMode) {
                case CAM_FOLLOW:
                case CAM_FOLLOW_RUN:
                case CAM_ROCKET:
                case CAM_FIXED_LOOKAT:
                case CAM_DIRECTED:
                    Camera_SetMode(newMode, 1);
                    allowLook = 0;
                    break;
                case CAM_ROBOT:
                    Camera_SetMode(newMode, 0);
                    bringBack = 1;
                    allowLook = 0;
                    break;
                case CAM_SAM_CHASE:
                    if (newMode == CAM_DIRECTED)
                        Camera_SetMode(newMode, 1);
                    allowLook = 0;
                    break;
                default:
                    if (Camera_IsScriptedOrReturning() && g_camScriptReturnMode) {
                        g_camScriptFlags &= ~CAMSCR_BLEND_OUT;
                        Camera_SetMode(newMode, 0);
                        bringBack = 1;
                        allowLook = 0;
                    }
            }
            if (bringBack && newMode == g_camMode) {
                g_camReqFlags.useYaw = 1;
                g_camReqFlags.usePitch = 1;
                g_camReqRot = savedCamRot;
                g_camReqDist = savedCamDist;
                g_camReqVel.x = g_camReqVel.y = g_camReqVel.z = 0;
            }
            switched = 1;
        } else
            switched = 0;
        doSave = 1;
        switch (g_camMode) {
            case CAM_FOLLOW:
                if (stateD->flags & (WSF_CAM_FIXED_PITCH | WSF_CAM_USE_WOLF_ANGLE)) {
                    g_camReqFlags.usePitch = 1;
                    if (stateD->flags & WSF_CAM_FIXED_PITCH)
                        g_camReqRot.x = 0x200;
                    else
                        g_camReqRot.x = rot.x;
                }
                if (!(pad->cur.buttons & ~g_padMasks[8]) && (pad->prev.buttons & ~g_padMasks[8]) && allowLook)
                    Camera_SetMode(CAM_LOOK, 1);
                break;
            case CAM_LOOK:
                if (g_camera.dist <= 0x14 && g_camFlags.manual) {
                    moveDir = (-g_camera.rot.y + 0x800) & 0xfff;
                    SetFacing(moveDir);
                }
                g_camReqAimOffset.y -= 0x1e;
                if ((pad->cur.buttons & ~g_padMasks[8]) || !allowLook)
                    Camera_SetMode(newMode, 1);
                break;
            case CAM_FOLLOW_RUN:
            case CAM_FIXED_LOOKAT:
                break;
            case CAM_ROCKET:
                g_camReqRot.x = rot.x & 0xfff;
                if (g_camReqRot.x > 0x800)
                    g_camReqRot.x = ((g_camReqRot.x - 0x1000) >> 1) & 0xfff;
                else
                    g_camReqRot.x = g_camReqRot.x >> 1;
                g_camReqRot.z = rot.z;
                if (g_camReqRot.z > 0x800)
                    g_camReqRot.z = ((g_camReqRot.z - 0x1000) >> 1) & 0xfff;
                else
                    g_camReqRot.z = g_camReqRot.z >> 1;
                g_camReqDist = (600 - speed) * -140 / -600 + 510;
                g_camReqFocal = g_camReqDist * 0x180 / 0x262;
                if (flyBox) {
                    height = pos.y - flyBox->min[1];
                    if (height < 0)
                        height = 0;
                    if (height <= 600) {
                        if (height <= 400) {
                            g_camReqAimOffset.y += (s16)((400 - height) * 300 / 400);
                            g_camReqRot.x = 0;
                        } else {
                            g_camReqRot.x = (s16)((g_camReqRot.x + 0x800) & 0xfff) - 0x800;
                            g_camReqRot.x = (g_camReqRot.x * (height - 200) / 200) & 0xfff;
                        }
                    }
                }
                break;
            case CAM_DIRECTED:
                g_camReqRot.x = g_camMinPitch;
                switch (req) {
                    case CAM_REQ_DIR_COVER:
                        g_camReqFlags.restriction = 1;
                        g_camReqDist = 0x262;
                        if (coverAction.action == CTX_USE)
                            g_camReqRot.y = -GetSideHeading(coverAction.target) & 0xfff;
                        else
                            g_camReqRot.y = -Facing() & 0xfff;
                        break;
                    case CAM_REQ_DIR_BEHIND:
                        g_camReqFlags.padRotate = 0;
                        g_camReqDist = 0x262;
                        g_camReqRot.y = (0x800 - Facing()) & 0xfff;
                        break;
                    case CAM_REQ_DIR_CLIMB:
                        g_camReqFlags.padRotate = 0;
                        g_camReqFlags.restriction = 1;
                        g_camReqDist = 0x262;
                        g_camReqRot.y = (0x800 - Facing()) & 0xfff;
                        if (SDW_ABS(velocity.y) <= 0x3c)
                            g_camReqRot.x = g_camMinPitch;
                        else if (velocity.y < 0)
                            g_camReqRot.x = 0xd56;
                        else
                            g_camReqRot.x = 0x2aa;
                        break;
                    case CAM_REQ_DIR_HIGH:
                        g_camReqFlags.padRotate = 0;
                        g_camReqFlags.restriction = 1;
                        g_camReqDist = 0x262;
                        g_camReqRot.y = (0x800 - Facing()) & 0xfff;
                        g_camReqRot.x = 0x200;
                        break;
                    case CAM_REQ_DIR_FRONT:
                        g_camReqFlags.padRotate = 0;
                        g_camReqDist = 0x262;
                        g_camReqRot.y = -Facing() & 0xfff;
                        g_camReqRot.x = 0x2aa;
                        g_camReqAimOffset.x = g_sinTable4096[Facing()] * -300 / 4096;
                        g_camReqAimOffset.z = g_pCosTable[Facing()] * -300 / 4096;
                        break;
                    case CAM_REQ_DIR_HOLD:
                    case CAM_REQ_DIR_HOLD2:
                    case CAM_REQ_DIR_HOLD3:
                        g_camReqRot.y = g_camera.rot.y;
                        g_camReqRot.x = g_camera.rot.x;
                        g_camReqRot.z = 0;
                }
                break;
            default:
                doSave = 0;
        }
        ScaleVec(&g_camReqAimOffset);
        if (doSave && !switched) {
            savedCamRot = g_camera.rot;
            savedCamDist = g_camera.dist;
        }
    }
}

/* 0x490286 - find this frame's context action (+0x640) and held-item action (+0x648): only in states with flag 0x2000,
 * not with Wolf flag 1 and not in model set 4 (the rabbit costume). On foot with empty hands the objects around answer
 * message 2 (and 0x26 for cover); failing that a CLIMBBOXES zone gives action 0xf or, with box flag 0x4000000, 0x11.
 * Holding something, the item's message-0x17 answer is the held action and only cover is scanned. The held item is
 * asked for its HUD prompt with message 6 every frame. */
void Wolf::ScanContextActions()
{
    Box *box;
    s32 allowed;
    WolfStateDesc *desc;
    ctxAction.target = 0;
    ctxAction.action = CTX_NONE;
    heldActionType = HELD_NONE;
    ctx4PromptIndex = 0;
    coverAction.target = 0;
    coverAction.action = CTX_NONE;
    coverWord2 = 0;
    coverWord3 = 0;
    itemPromptClassId = 0;
    desc = &g_wolfMoveBank0[mode].states[state];
    allowed = (desc->flags & WSF_CONTEXT_ACTIONS) && !(flags & WOLF_FB_DEAD);
    if (modelSet == WMS_RABBITCOSTUME)
        allowed = 0;
    if (allowed) {
        switch (mode) {
            case WOLF_MODE_NORMAL:
                if ((g_gameFlags & GF_ITEM_FLY) || (flags & WOLF_FB_EQUIP_PENDING))
                    ScanInteractables(&ctxAction, &coverAction, 0x32, 0x3c, 100, 0x400, 1);
                else if (!heldObject) {
                    ScanInteractables(&ctxAction, &coverAction, 0x32, 0x3c, 100, 0x400, 0);
                    if (!ctxAction.action) {
                        box = climbZones.FindContaining(&pos);
                        if (box) {
                            if (box->flags & CLIMB_F_WALKABLE)
                                ctxAction.action = CTX_CLIMBBOXWALK_ENTER;
                            else
                                ctxAction.action = CTX_CLIMB;
                            ctxAction.target = 0;
                        }
                    }
                } else {
                    heldActionType = heldObject->HandleMessage(this, MSG_QUERY_HELD_ACTION, 0);
                    ScanInteractables(&ctxAction, &coverAction, 0x32, 0x3c, 100, 0x400, 1);
                }
                break;
            case WOLF_MODE_CARRY:
                heldActionType = heldObject->HandleMessage(this, MSG_QUERY_HELD_ACTION, 0);
        }
    }
    if (heldObject)
        heldObject->HandleMessage(this, MSG_QUERY_NEAREST_TARGET, &itemPromptClassId);
}

/* 0x49054c - whether the pending context (else held) action may fire: most need the Wolf on the ground; 2/4 (lift,
 * pick up) also full size; 0xd not with Wolf flag 0x40000; 0xf/0x11 (climb zones) always. Held action 3 (the rocket)
 * needs flag 0x40000 clear and the Wolf inside FLYBOX when there is one; 0x13 needs the ground, flags 0x40030 clear
 * and no air time. */
s32 Wolf::IsActionAvailable(s32 onGround)
{
    s32 ok;
    ok = 0;
    switch (ctxAction.action) {
        case CTX_NONE:
            switch (heldActionType) {
                case HELD_ROCKET:
                    if (!(flags & WOLF_FB_SPECIAL_OBJECT)) {
                        if (!flyBox || Box_ContainsPoint(flyBox, &pos))
                            ok = 1;
                    }
                    break;
                case HELD_TIMEMACHINE:
                    if (onGround &&
                        !(flags & (WOLF_FB_RECENT_CONTACT2 | WOLF_FB_RECENT_OBJECT_CONTACT | WOLF_FB_SPECIAL_OBJECT)) &&
                        !airTime)
                        ok = 1;
                    break;
                case HELD_NONE:
                case HELD_USE:
                case HELD_UMBRELLA:
                case HELD_REMOTE_ALT:
                case HELD_KEY_USE:
                    ok = 1;
                    break;
                case HELD_THROWABLE:
                case HELD_FAN:
                case HELD_FLUTE:
                case HELD_DROP_IN_PLACE:
                case HELD_DROP_ACTIVE:
                case HELD_MINEDETECTOR:
                case HELD_REMOTE:
                case HELD_FISHINGROD:
                case HELD_INFLATABLE_SHEEP:
                case HELD_SEED_PLANT:
                    if (onGround)
                        ok = 1;
            }
            break;
        case CTX_LIFT:
        case CTX_PICKUP:
            if (onGround && scale >= 0x400)
                ok = 1;
            break;
        case CTX_BUSH:
            if (!(flags & WOLF_FB_SPECIAL_OBJECT))
                ok = 1;
            break;
        case CTX_CLIMB:
        case CTX_CLIMBBOXWALK_ENTER:
            ok = 1;
            break;
        case CTX_USE:
        case CTX_ACTIVATE:
        case CTX_PUSH:
        case CTX_READ:
        case CTX_TALK:
        case CTX_TELESCOPE:
        case CTX_USE_0A:
        case CTX_USE_0B:
        case CTX_DEFUSE:
        case CTX_ELASTIC_TIE:
        case CTX_ELASTIC_TAKE_BACK:
        case CTX_ELASTIC_GRAB:
        case CTX_ELASTIC_PULL:
        case CTX_WOLFTRAP:
        case CTX_SWIRLSIGN:
        case CTX_CANNON:
        case CTX_PIPE:
        case CTX_TIMEKEEPER:
        case CTX_ICECUBE:
        case CTX_SAVE:
        case CTX_LEVELDOOR:
        case CTX_CATAPULT:
        case CTX_BONUS:
            if (onGround)
                ok = 1;
    }
    return ok;
}

/* 0x490778 - the action button: start the state for the context action (+0x640), or with none for the held item's
 * action (+0x648). Object actions that the object performs itself get message 1 (use) and return 0, as do the held
 * item's own uses (message 0x15). Held action 0 with empty hands and the stick at rest is the idle action (state 0x36).
 * Returns whether the button was consumed by a state change. */
/* BYTES(slot-name): declared in this order for the original slots: result -4, reply -8, dropArg -0x10 */
/* BYTES(dead-code): reply is stored and never read, as in the original */
s32 Wolf::DoAction(s32 onGround)
{
    DropMsgArg dropArg; /* declared in this order for the frame: result -4, reply -8, dropArg -0x10 */
    s32 reply;
    s32 result;
    result = 1;
    switch (ctxAction.action) {
        case CTX_NONE:
            switch (heldActionType) {
                case HELD_NONE:
                    if (onGround && !heldObject && !stickMag && state != WOLF_ST_SHRUG)
                        SetState(WOLF_ST_SHRUG);
                    else
                        result = 0;
                    break;
                case HELD_THROWABLE:
                case HELD_SEED_PLANT:
                    if (mode == WOLF_MODE_CARRY)
                        SetState(WOLF_ST_PUTDOWN_M0);
                    else
                        SetState(WOLF_ST_THROW_ITEM);
                    heldObject->HandleMessage(this, MSG_HELD_STATE_BEGIN, 0);
                    airTime = 0;
                    break;
                case HELD_ROCKET:
                    if (onGround)
                        SetState(WOLF_ST_ROCKET_MOUNT);
                    else
                        SetState(WOLF_ST_ROCKET_IGNITE);
                    break;
                case HELD_FAN:
                    SetState(WOLF_ST_FAN_IDLE);
                    break;
                case HELD_FLUTE:
                    SetState(WOLF_ST_FLUTE_IDLE);
                    break;
                case HELD_DROP_IN_PLACE:
                    Inventory_Remove(heldObject);
                    dropArg.pos = pos;
                    dropArg.placed = 0;
                    dropArg.flag1 = 0;
                    reply = heldObject->HandleMessage(this, MSG_DROP, &dropArg);
                    heldObject->HandleMessage(this, MSG_HELD_STATE_BEGIN, 0);
                    SetActiveItem(heldObject);
                    heldObject = 0;
                    result = 0;
                    break;
                case HELD_MINEDETECTOR:
                    SetState(WOLF_ST_DETECTOR_START);
                    break;
                case HELD_UMBRELLA:
                    if (onGround)
                        SetState(WOLF_ST_UMBRELLA_OPEN);
                    else
                        SetState(WOLF_ST_UMBRELLA_OPEN_AIR);
                    break;
                case HELD_REMOTE:
                    SetState(WOLF_ST_REMOTE_START);
                    break;
                case HELD_REMOTE_ALT:
                    if (onGround)
                        SetState(WOLF_ST_REMOTE_PRESS);
                    else {
                        heldObject->HandleMessage(this, MSG_HELD_STATE_BEGIN, 0);
                        result = 0;
                    }
                    break;
                case HELD_TIMEMACHINE:
                    SetState(WOLF_ST_TIMEMACHINE_USE);
                    break;
                case HELD_FISHINGROD:
                    lineHeading = (Facing() + 0x200) & 0xfff;
                    lineHeading = lineHeading - (lineHeading & 0x3ff);
                    SetState(WOLF_ST_FISHING_CAST);
                    break;
                case HELD_USE:
                case HELD_KEY_USE:
                    heldObject->HandleMessage(this, MSG_HELD_STATE_BEGIN, 0);
                    result = 0;
                    break;
                case HELD_INFLATABLE_SHEEP:
                    SetState(WOLF_ST_INFLATE_START);
                    Sound_Play(SND_WOLF_ITEM_ACTION, this, 0xff, SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 0x1000);
            }
            break;
        case CTX_LIFT:
            airTime = 0;
            SetState(WOLF_ST_LIFT_START);
            break;
        case CTX_ACTIVATE:
            SetState(WOLF_ST_ACTIVATE);
            break;
        case CTX_PICKUP:
            airTime = 0;
            SetState(WOLF_ST_PICKUP_ITEM);
            break;
        case CTX_PUSH:
            SetState(WOLF_ST_PUSH_START);
            break;
        case CTX_BUSH:
            ctxAction.target->HandleMessage(this, MSG_BUSH_ENTER_BEGIN, 0);
            SetState(WOLF_ST_BUSH_PUT_ON);
            break;
        case CTX_CLIMB:
            StopMotion();
            if (!ctxAction.target)
                SetState(WOLF_ST_CLIMB_ZONE);
            else
                SetState(WOLF_ST_CLIMB_OBJECT);
            break;
        case CTX_CLIMBBOXWALK_ENTER:
            SetState(WOLF_ST_CLIMBBOXWALK_IDLE);
            StopMotion();
            break;
        case CTX_ELASTIC_TAKE_BACK:
            SetState(WOLF_ST_ELASTIC_TAKE_BACK);
            break;
        case CTX_ELASTIC_GRAB:
            SetState(WOLF_ST_ELASTIC_GRAB);
            break;
        case CTX_ELASTIC_TIE:
            ClearActiveItem();
            ctxAction.target->HandleMessage(this, MSG_USE, 0);
            result = 0;
            break;
        case CTX_ELASTIC_PULL:
            flags |= WOLF_FB_ITEM_ACTION;
            SetActiveItem(ctxAction.target);
            lineHeading = 0;
            elastic->HandleMessage(this, MSG_USE, 0);
            SetState(WOLF_ST_ELASTIC_PULL_IDLE);
            break;
        case CTX_DEFUSE:
            StopMotion();
            interactPos = ctxAction.target->pos;
            ctxAction.target->HandleMessage(this, MSG_USE, 0);
            SetState(WOLF_ST_DEFUSE_MINE);
            break;
        case CTX_TALK:
            SetState(WOLF_ST_TALK_TURN);
            result = 1;
            break;
        case CTX_USE:
        case CTX_READ:
        case CTX_TELESCOPE:
        case CTX_USE_0A:
        case CTX_USE_0B:
        case CTX_CANNON:
        case CTX_PIPE:
        case CTX_SAVE:
        case CTX_BONUS:
            ctxAction.target->HandleMessage(this, MSG_USE, 0);
            result = 0;
            break;
        case CTX_WOLFTRAP:
            flags |= WOLF_FB_IN_WOLFTRAP;
            ClearFlags(WOLF_FB_WOLFTRAP_REACT);
            SetState(WOLF_ST_WOLFTRAP_ENTER);
            break;
        case CTX_SWIRLSIGN:
            SetState(WOLF_ST_USE_CTX1A);
            break;
        case CTX_TIMEKEEPER:
            ctxAction.target->HandleMessage(this, MSG_USE, 0);
            interactHeading = HeadingTo(&ctxAction.target->pos);
            SetState(WOLF_ST_TIMEKEEPER_USE);
            break;
        case CTX_ICECUBE:
            SetState(WOLF_ST_PUSH_ICECUBE);
            break;
        case CTX_LEVELDOOR:
            ctxAction.target->HandleMessage(this, MSG_USE, 0);
            interactHeading = HeadingTo(&ctxAction.target->pos);
            SetState(WOLF_ST_ENTER_LEVEL_DOOR);
            break;
        case CTX_CATAPULT:
            ctxAction.target->HandleMessage(this, MSG_USE, 0);
            SetState(WOLF_ST_CATAPULT_OPERATE);
    }
    return result;
}

/* 0x490ecf - drop the pending actions when they may not fire now; on the action button's press edge (padBits 0x40) do
 * the action. */
s32 Wolf::CheckActionButton(s32 onGround)
{
    if (!IsActionAvailable(onGround)) {
        ctxAction.target = 0;
        ctxAction.action = CTX_NONE;
        heldActionType = HELD_NONE;
        ctx4PromptIndex = 0;
    }
    if (padBits & WOLF_ACT_ACTION_EDGE)
        return DoAction(onGround);
    return 0;
}

/* 0x490f40 - start pushing (state 0x5c) the cover object when there is one, enable is set and the stick points at it. */
/* BYTES(dead-code): wanted is a copy the original makes and never reads (0x490f49) */
s32 Wolf::TryStartPush(s32 enable)
{
    s32 wanted = enable; /* a copy the original makes and never reads (0x490f49) */
    switch (coverAction.action) {
        case CTX_USE:
            if (enable && IsStickTowardSide(coverAction.target)) {
                SetState(WOLF_ST_COVER_APPROACH);
                return 1;
            }
    }
    return 0;
}

/* 0x490f98 - world position of the held object for a local offset: pos + R(rot) * (localOffset + the held object's
 * attach-link offset). */
void Wolf::CalcHeldObjWorldPos(const Vec3s *localOffset, Vec3s *outPos)
{
    /* cast kept: g_sharedScratch is one scratch buffer that each user lays out its own way */
    WolfHoldScratch *s = (WolfHoldScratch *)g_sharedScratch;
    Mat34s_FromEulerScaled(&rot, &s->m, 0);
    s->in.x = localOffset->x + heldObject->GetAttachLink()->localOffset.x;
    s->in.y = localOffset->y + heldObject->GetAttachLink()->localOffset.y;
    s->in.z = localOffset->z + heldObject->GetAttachLink()->localOffset.z;
    Mat34s_TransformVec3s(&s->m, &s->in, &s->out);
    outPos->x = s->out.x + pos.x;
    outPos->y = s->out.y + pos.y;
    outPos->z = s->out.z + pos.z;
}

/* 0x491091 - where the held object would be put down, and whether it may be (not with Wolf flag 0x10). atFeet: just in
 * front (0,-2,0), on ground no more than 20 below. Otherwise (0,-70,-92): the body must fit at eight points on the way
 * from there towards the Wolf and at the midpoint, the ground there must not be above it (there is no limit below),
 * and the object under it must not have class flag 4 - except a FloatingBox put on the crocodile of Lvl-09
 * (CrocodileLevel09; in-game Level 8). */
/* BYTES(dead-code): zero is set and never read, as in the original */
/* BYTES(slot-name): names chosen for their stack slots (floorY -2 ... -0x1c) */
/* BYTES(view, inferred): the put-down offsets are read as the Vec3s they start with */
s32 Wolf::CanPutDownHeldObj(s32 atFeet, Vec3s *outPos)
{
    Vec3s at;
    s32 ok;
    ScnObject *below;
    s32 i;
    s32 zero; /* set and never read; the names order the frame (floorY -2 ... at -0x1c) */
    s16 floorY;
    zero = 0;
    ok = 0;
    if (!(flags & WOLF_FB_RECENT_CONTACT2)) {
        if (atFeet) {
            CalcHeldObjWorldPos(&g_wolfPutDownOffsetNear, outPos);
            floorY = heldObject->World_GroundYRay(outPos, 1);
            if (floorY >= outPos->y && floorY <= outPos->y + 0x14)
                ok = 1;
        } else {
            CalcHeldObjWorldPos(&g_wolfPutDownOffsetFront, outPos);
            for (i = 0; i < 0x20; i += 4) {
                at.x = (pos.x * i + outPos->x * (0x20 - i)) >> 5;
                at.y = (pos.y * i + outPos->y * (0x20 - i)) >> 5;
                at.z = (pos.z * i + outPos->z * (0x20 - i)) >> 5;
                if (heldObject->TestBodyAt(&at, CQ_STATIC))
                    return 0;
            }
            at.x = (pos.x + outPos->x) >> 1;
            at.y = (pos.y + outPos->y) >> 1;
            at.z = (pos.z + outPos->z) >> 1;
            if (!heldObject->TestBodyAt(&at, CQ_STATIC) &&
                heldObject->QueryGroundYAndObject(outPos, &below) >= outPos->y &&
                (!below || !(Scenaric_ClassFlags(below->GetClassId()) & SCN_CF_CARRIER) ||
                 (heldObject->GetClassId() == CLASSID_FLOATINGBOX && below->GetClassId() == CLASSID_CROCODILELEVEL09)))
                ok = 1;
        }
    }
    return ok;
}

/* 0x4912c7 - the dizzy stars: stop effect 5, and unless effects 0x100004 (ice block) run, show prop 0 attached to the
 * Wolf's joint 0 at (0,-150,0) and set effect flag 8. */
void Wolf::Fx0_Start()
{
    Vec3s offset;
    offset.x = 0;
    offset.y = -150;
    offset.z = 0;
    if (fxFlags & WOLF_FA_PROP5)
        Fx5_Stop();
    if (!(fxFlags & (WOLF_FA_PROP_ICE | WOLF_FA_PROP4))) {
        if (ShowProp(WOLF_PROP_ETOIL, AETOIL01_ANIM_TURN1, 1)) {
            propBody.AttachTo(this, 0, &offset, 0, 1, 0);
            fxFlags |= WOLF_FA_PROP0;
        }
    }
}

/* 0x491358 - stop the dizzy stars: detach the prop body, clear effect flag 8. */
void Wolf::Fx0_Stop()
{
    propBody.Detach();
    ClearFxFlags(WOLF_FA_PROP0);
}

/* 0x49138c - with the splash body loaded (fxFlags 1) and Ralph in a water zone: put the splash body at his x / z on
 * the water surface (the zone box's top, min.y) and play its animation 0 looped (effect flag 2). */
void Wolf::Fx2Body_StartAtSurface()
{
    Vec3s at;
    if ((fxFlags & WOLF_FA_HAS_SPLASH) && waterZone) {
        at = pos;
        at.y = waterZone->min[1];
        splashBody.pos = at;
        splashBody.PlayAnim(APLOUF01_ANIM_SPLASH, 1, 0);
        fxFlags |= WOLF_FA_SPLASH_ACTIVE;
    }
}

/* 0x49145a */
void Wolf::Fx2Body_Stop()
{
    ClearFxFlags(WOLF_FA_SPLASH_ACTIVE);
}

/* 0x491480 - stop effect 5, then attach prop 3 (animation 2, looped; effect flag 4) or with variant prop 4 (animation 1
 * once; 0x100000) to the Wolf. The locals are named for their stack slots (slot -4, flag -8, animId -0xa, loop -0x10). */
/* BYTES(slot-name): names chosen for their stack slots: slot -4, flag -8, animId -0xa, loop -0x10 */
void Wolf::Fx3or4_Start(s32 variant)
{
    s32 loop;
    u16 animId;
    u32 flag;
    s32 slot;
    if (fxFlags & WOLF_FA_PROP5)
        Fx5_Stop();
    if (variant) {
        slot = WOLF_PROP_GLACON3;
        animId = AGLACON3_ANIM_CLOSED;
        loop = 0;
        flag = WOLF_FA_PROP4;
    } else {
        slot = WOLF_PROP_GLACON1;
        animId = AGLACON1_ANIM_STAND2;
        loop = 1;
        flag = WOLF_FA_PROP_ICE;
    }
    if (ShowProp(slot, animId, loop)) {
        propBody.AttachTo(this, 0, 0, 0, 0, 0);
        fxFlags |= flag;
    }
}

/* 0x491532 */
void Wolf::Fx3or4_Stop()
{
    propBody.Detach();
    ClearFxFlags(WOLF_FA_PROP_ICE | WOLF_FA_PROP4);
}

/* 0x491566 - unless an attached effect runs: prop 5 (animation 2 with variant, else 1) placed, not attached, at the
 * Wolf's root joint - on the water surface when he is in a water zone (effect flag 0x2000). */
void Wolf::Fx5_Start(s32 variant)
{
    u16 animId;
    Vec3s at;
    if (!(fxFlags & (WOLF_FA_PROP_ICE | WOLF_FA_PROP0 | WOLF_FA_DEATHZONE_PROP | WOLF_FA_PROP7 | WOLF_FA_PROP4 |
                     WOLF_FA_PROP8))) {
        if (variant)
            animId = AFEUIL01_ANIM_ACTION2;
        else
            animId = AFEUIL01_ANIM_ACTION1;
        if (ShowProp(WOLF_PROP_FEUIL, animId, 0)) {
            Anim_GetRootOffset(Inst(), &anim, &at);
            at.x += pos.x;
            at.y += pos.y;
            at.z += pos.z;
            if (waterZone)
                at.y = waterZone->min[1];
            propBody.pos = at;
            fxFlags |= WOLF_FA_PROP5;
        }
    }
}

/* 0x491651 */
void Wolf::Fx5_Stop()
{
    ClearFxFlags(WOLF_FA_PROP5);
}

/* 0x491677 - unless an attached effect runs: show a prop with an animation, once, in place (effect flag 0x10000). */
void Wolf::FxGeneric_Start(u8 propIndex, u8 animId)
{
    if (!(fxFlags & (WOLF_FA_PROP_ICE | WOLF_FA_PROP0 | WOLF_FA_PROP5 | WOLF_FA_PROP7 | WOLF_FA_PROP4))) {
        if (ShowProp(propIndex, animId, 0))
            fxFlags |= WOLF_FA_DEATHZONE_PROP;
    }
}

/* 0x4916c8 */
void Wolf::FxGeneric_Stop()
{
    ClearFxFlags(WOLF_FA_DEATHZONE_PROP);
}

/* 0x4916ee - sound 0x13F, and unless an attached effect runs prop 8 (animation 1, looped) attached to the Wolf (effect
 * flag 0x400000). */
void Wolf::Fx8_Start()
{
    Sound_Play(SND_WOLF_FX8, this, 0xff, SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 0x1000);
    if (!(fxFlags & (WOLF_FA_PROP_ICE | WOLF_FA_PROP0 | WOLF_FA_PROP5 | WOLF_FA_DEATHZONE_PROP | WOLF_FA_PROP7 |
                     WOLF_FA_PROP4))) {
        if (ShowProp(WOLF_PROP_ECLAI, AECLAI01_ANIM_TOAST, 1)) {
            propBody.AttachTo(this, 0, 0, 0, 1, 0);
            fxFlags |= WOLF_FA_PROP8;
        }
    }
}

/* 0x49176f */
void Wolf::Fx8_Stop()
{
    propBody.Detach();
    ClearFxFlags(WOLF_FA_PROP8);
}

/* 0x4917a3 - stop effect 5; unless an attached effect runs show prop 6 in place (animation 0 with variant, else 2;
 * effect flag 0x4000). */
void Wolf::Fx6_Start(s32 variant)
{
    u16 animId;
    if (fxFlags & WOLF_FA_PROP5)
        Fx5_Stop();
    if (!(fxFlags & (WOLF_FA_PROP_ICE | WOLF_FA_PROP0 | WOLF_FA_DEATHZONE_PROP | WOLF_FA_PROP7 | WOLF_FA_PROP4 |
                     WOLF_FA_PROP8))) {
        if (variant)
            animId = ACENSURE_ANIM_APPEAR;
        else
            animId = ACENSURE_ANIM_DAPPEAR;
        if (ShowProp(WOLF_PROP_CENSURE, animId, 0))
            fxFlags |= WOLF_FA_PROP6_BILLBOARD;
    }
}

/* 0x49181f */
void Wolf::Fx6_Stop()
{
    ClearFxFlags(WOLF_FA_PROP6_BILLBOARD);
}

/* 0x491845 - set Wolf flag 0x200 and send the held object message 0x15 with the low nibble of the state descriptor's
 * byte 2 as its argument. */
void Wolf::HeldObj_SendMsg15()
{
    WolfStateDesc *desc = &g_wolfMoveBank0[mode].states[state];
    flags |= WOLF_FB_HELD_IN_USE;
    /* cast kept: arg carries a number */
    heldObject->HandleMessage(this, MSG_HELD_STATE_BEGIN, (void *)desc->heldMsgArg);
}

/* 0x4918b8 - message 0x16 with the same argument, then clear Wolf flag 0x200. */
void Wolf::HeldObj_SendMsg16()
{
    WolfStateDesc *desc = &g_wolfMoveBank0[mode].states[state];
    /* cast kept: arg carries a number */
    heldObject->HandleMessage(this, MSG_HELD_STATE_END, (void *)desc->heldMsgArg);
    ClearFlags(WOLF_FB_HELD_IN_USE);
}

/* 0x49192e - whether obj is a Rocket (class 10). A member with an unused this (ret 4). */
/* BYTES(view): a member (thiscall, ret 4) although this is unused, as in the original */
/* BYTES(flow, inferred): one-case switch: the original compares this way */
s32 ScnObject::IsRocket(ScnObject *obj)
{
    switch (obj->GetClassId()) {
        case CLASSID_ROCKET:
            return 1;
    }
    return 0;
}

/* 0x491960 - take the elastic (Wolf flag 0x40000) and, unless flag 0x80000 or effect 0x80000 is already set, show prop
 * 7 attached to joint 3 (effect flag 0x80000). */
void Wolf::SetActiveItem(ScnObject *item)
{
    flags |= WOLF_FB_SPECIAL_OBJECT;
    elastic = item;
    if (!(flags & WOLF_FB_ITEM_ACTION) && !(fxFlags & WOLF_FA_PROP7)) {
        if (ShowProp(WOLF_PROP_CBOUE, ACBOUE01_ANIM_DEFAULT, 0)) {
            propBody.AttachTo(this, 3, 0, 0, 0, 0);
            fxFlags |= WOLF_FA_PROP7;
        }
    }
}

/* 0x4919fc - let go of it: clear flag 0x40000 and the elastic, stop the loop sound, detach prop 7. */
void Wolf::ClearActiveItem()
{
    ClearFlags(WOLF_FB_SPECIAL_OBJECT);
    elastic = 0;
    StopLoopSound();
    if (fxFlags & WOLF_FA_PROP7) {
        propBody.Detach();
        ClearFxFlags(WOLF_FA_PROP7);
    }
}

/* 0x491a72 - put the active item into the inventory: message 0x1900 to it, let go, message 7, out of the world. */
void Wolf::StoreActiveItemInInventory()
{
    ScnObject *item = elastic;
    item->HandleMessage(this, MSG_INVENTORY_STORE_BEGIN, 0);
    ClearActiveItem();
    item->HandleMessage(this, MSG_INVENTORY_STORED, 0);
    item->RemoveFromWorld();
    Inventory_Add(item);
}

/* 0x491ad0 - falling: in the air for more than 0x800 ticks (0.5 s), or more than 0x200 with the ground over 200 below,
 * or a state with flag 0x80000. */
s32 Wolf::IsFalling()
{
    return airTime > 0x800 || (airTime > 0x200 && GroundY() - pos.y > 200) ||
           (g_wolfMoveBank0[mode].states[state].flags & WSF_AIRBORNE);
}

/* 0x491b58 - a jump may start: the ground within 10 below or airborne for no more than 0x111 ticks (coyote time), the
 * ground normal steeper-up than the surface's slope limit (tuning word 2, read unsigned), and not Wolf flag 0x1000. */
s32 Wolf::CanJump()
{
    /* cast kept: the tuning words are read unsigned here */
    const u16 *tuning = (const u16 *)g_wolfMoveBank0[mode].surfaceTuning[surface];
    return (GroundY() - pos.y <= 10 || airTime <= 0x111) && groundNormal.y < -tuning[2] &&
           !(flags & WOLF_FB_IN_SLIDE_ZONE);
}

/* PAL PC object-side helpers, 0x491bf2-0x491d24. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
s32 ScnObject::GetSideQuadrant(ScnObject *other)
{
    struct Work {
        s32 dx, side, dz;
    } w;
    w.dx = pos.x - other->pos.x;
    w.dz = pos.z - other->pos.z;
    w.side = 0;
    if (w.dx > w.dz)
        w.side |= SIDEQ_DX_GT_DZ;
    if (w.dx > -w.dz)
        w.side |= SIDEQ_DX_GT_NEG_DZ;
    return w.side;
}
s32 ScnObject::IsOnSameSideAs(ScnObject *a, ScnObject *b)
{
    switch (GetSideQuadrant(b)) {
        case 0:
            return a->pos.x <= b->pos.x;
        case SIDEQ_DX_GT_DZ:
            return a->pos.z <= b->pos.z;
        case SIDEQ_DX_GT_NEG_DZ:
            return a->pos.z >= b->pos.z;
        case SIDEQ_DX_GT_DZ | SIDEQ_DX_GT_NEG_DZ:
            return a->pos.x >= b->pos.x;
    }
    return 1;
}
s16 ScnObject::GetSideHeading(ScnObject *other)
{
    s32 side = GetSideQuadrant(other);
    return g_sideHeadingTable[side];
}

/* PAL PC ScnControllable input helper, 0x491d25. */
#define ABS_VALUE(x) ((x) >= 0 ? (x) : -(x))
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
s32 ScnControllable::IsStickTowardSide(ScnObject *other)
{
    struct Work {
        s32 answer;
        s16 heading, desired;
    } w;
    w.answer = 0;
    if (padBits & WOLF_ACT_SNEAK_HELD) {
        w.heading = (GetSideHeading(other) + 2048) & 4095;
        w.desired = GetStickHeading(w.heading);
        w.answer = ABS_VALUE((s16)((s16)((w.desired - w.heading + 2048) & 4095) - 2048)) <= 682;
    }
    return w.answer;
}
