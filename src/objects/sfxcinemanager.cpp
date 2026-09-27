/* PAL PC SfxCineManager, 0x4f06b0-0x4f0d97. */
/* BYTES: dead-code. */
#define SDW_MEMBERS_ScnObject                    \
    static void *operator new(u32 size);         \
    void SetUpdateDuringCine(s32 on)             \
    {                                            \
        if (on)                                  \
            flags |= SCN_OF_NO_PLANE_CULL;       \
        else                                     \
            flags &= (u16)~SCN_OF_NO_PLANE_CULL; \
    }
#define SDW_MEMBERS_ParticleEmitter \
    ParticleEmitter()               \
    {                               \
        Emitter_Init(0);            \
    }
#include "sdw_enums.h"
#include "sdw_classes.h"
#define SDW_INLINE_SCNOBJECT_FACING 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_FACING
extern s32 g_dtMs;
#include "../engine/scn_tools.h"

#define SDW_INLINE_FREE_SCN_GETPROPS32_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_SCN_GETPROPS32_VOID_U32

/* 0x4f06b0. The unused target-position calculation and raw-count division are original. */
/* BYTES(dead-code): point is computed and never used, as in the original */
void SfxCineManager::PostLoadInit()
{
    void *rec = record;
    Vec3s point;
    s32 n;
    SetUpdateDuringCine(1);
    n = Scn_GetPropS32(rec, 0xc);
    emitter.Emitter_Init(n < 32 ? n : 32);
    typeSfx = (u8)Scn_GetPropS32(rec, 0x24);
    target = Scn_GetPropObject(rec, 0x20);
    targetOffset.x = (s16)Scn_GetPropS32(rec, 0x10);
    targetOffset.y = (s16)Scn_GetPropS32(rec, 0x14);
    targetOffset.z = (s16)Scn_GetPropS32(rec, 0x18);
    if (target) {
        point.x = target->pos.x;
        point.y = target->pos.y;
        point.z = target->pos.z;
        point.x += targetOffset.x;
        point.y += targetOffset.y;
        point.z += targetOffset.z;
    }
    switch (typeSfx) {
        case SFXCINE_TYPE_SMOKE:
            emitter.Emitter_Reset();
            driftParams.vSpeed = -60;
            driftParams.life = Scn_GetPropS32(rec, 4) << 12;
            driftParams.spawnInterval = driftParams.life / 16;
            driftParams.sizeStart = (s16)Scn_GetPropS32(rec, 0x1c);
            driftParams.hSpeed = (s16)Scn_GetPropS32(rec, 8);
            driftParams.sizeEnd = (s16)Scn_GetPropS32(rec, 0);
            driftParams.sheetIndex = 0;
            break;
        case SFXCINE_TYPE_RIPPLES:
            emitter.Emitter_Reset();
            fadeParams.life = Scn_GetPropS32(rec, 4) << 12;
            fadeParams.fadeStart = 0;
            fadeParams.spawnInterval = 0x400;
            fadeParams.sizeStart = (s16)Scn_GetPropS32(rec, 0x1c);
            fadeParams.sizeEnd = (s16)Scn_GetPropS32(rec, 0);
            fadeParams.sheetIndex = 1;
            break;
        case SFXCINE_TYPE_BUBBLES:
            emitter.Emitter_Reset();
            riseParams.riseSpeed = -120;
            riseParams.life = Scn_GetPropS32(rec, 4) << 12;
            riseParams.spawnInterval = 0x400;
            riseParams.size = (s16)Scn_GetPropS32(rec, 0x1c);
            riseParams.cap = 0;
            riseParams.sheetIndex = 2;
            break;
    }
    spawnPeriod = (Scn_GetPropS32(rec, 4) << 12) / n;
    spawnTimer = spawnPeriod;
    spawnPending = 0;
}

/* 0x4f0b11 */
void SfxCineManager::Render(Camera *view)
{
    if (emitter.flags.active) {
        switch (typeSfx) {
            case SFXCINE_TYPE_SMOKE:
            case SFXCINE_TYPE_BUBBLES:
                emitter.Emitter_Render(view, 0);
                break;
            case SFXCINE_TYPE_RIPPLES:
                emitter.Emitter_RenderFlat_Fwd(view);
                break;
        }
    }
}

/* 0x4f0b70. done is intentionally uninitialized for an unsupported typeSfx. */
void SfxCineManager::Update()
{
    s32 done;
    Vec3s point;
    point.x = pos.x;
    point.y = pos.y;
    point.z = pos.z;
    if (target) {
        point.x = target->pos.x;
        point.y = target->pos.y;
        point.z = target->pos.z;
        point.x += targetOffset.x;
        point.y += targetOffset.y;
        point.z += targetOffset.z;
    }
    spawnTimer -= g_dtMs;
    if (spawnTimer < 0) {
        spawnTimer = spawnPeriod;
        spawnPending = 1;
    }
    switch (typeSfx) {
        case SFXCINE_TYPE_BUBBLES:
            done = emitter.Emitter_UpdateRiseToCap(&riseParams, &point, spawnPending);
            break;
        case SFXCINE_TYPE_RIPPLES:
            done = emitter.Emitter_UpdateFade(&fadeParams, &point, 0, spawnPending);
            break;
        case SFXCINE_TYPE_SMOKE:
            done =
                emitter.Emitter_UpdateDrift(&driftParams, &point, target ? target->Facing() : Facing(), spawnPending);
            break;
    }
    if (done)
        spawnPending = 0;
}

/* 0x4f0d1f */
ScnObject *SfxCineManager_Create(void *record)
{
    ScnLogic *obj = new SfxCineManager;
    obj = (ScnLogic *)obj->Init(record); /* cast kept: Init returns the object as its ScnObject base */
    return obj;
}
