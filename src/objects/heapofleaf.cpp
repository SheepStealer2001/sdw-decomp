/* PAL PC HeapOfLeaf, 0x4c9140-0x4c9890. */
/* BYTES: view. */
/* BYTES(view): view: inline storage has the particleemitter header at offset zero (the access widths are the original's) */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
#define SDW_MEMBERS_ScnObject                    \
    static void *operator new(u32);              \
    void SetCustomRender(s32 on)                 \
    {                                            \
        if (on)                                  \
            flags |= SCN_OF_NO_PLANE_CULL;       \
        else                                     \
            flags &= (u16)~SCN_OF_NO_PLANE_CULL; \
    }                                            \
    void SetUpdateMode(s32 mode);
#define SDW_MEMBERS_InlineEmitter16 InlineEmitter16();
#include "sdw_classes.h"
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_GETFACING 1
#define SDW_INLINE_SCNOBJECT_SETMOVEMENTENABLED_S32 1
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_GETFACING
#undef SDW_INLINE_SCNOBJECT_SETMOVEMENTENABLED_S32
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32
inline InlineEmitter16::InlineEmitter16()
{
    base.slotPool = slotBuf;
    base.particles = particleBuf;
    base.count = 16;
    base.Emitter_Reset();
}
/* Inline storage has the ParticleEmitter header at offset zero. */
#include "../engine/id_list.h"
#include "../engine/scn_tools.h"
#include "../engine/scenaric.h"
u16 Sound_Play(u16, void *, u16, u8, s32);
#define SDW_INLINE_FREE_PROPU32_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_PROPU32_VOID_U32
#define ABS_VALUE(x) ((x) >= 0 ? (x) : -(x))
void HeapOfLeaf::PostLoadInit()
{
    void *a;
    u16 b;
    u16 c;
    u32 *d;
    disabled = 0;
    a = record;
    b = (u16)PropU32(a, 8);
    d = Scn_FindIdList(b, &c);
    polygon = (Trajectory *)*d; /* cast kept: an id list holds record addresses as u32 */
    if (!polygon)
        disabled = 1;
    else if (polygon->count > 1)
        ComputeRadius();
    else
        disabled = 1;
    zone = Scn_GetPropBox(a, 0);
    if (!zone)
        disabled = 1;
    guardian = Scn_GetPropObject(a, 4);
    leafFx.base.Emitter_Reset();
    driftParams.hSpeed = 10;
    driftParams.vSpeed = 20;
    driftParams.life = 0x800;
    driftParams.spawnInterval = 0x333;
    driftParams.sizeStart = 40;
    driftParams.sizeEnd = 90;
    driftParams.sheetIndex = 4;
    SetMovementEnabled(0);
    SetUpdateMode(SCN_UPD_ALWAYS);
    SetCustomRender(1);
}
void HeapOfLeaf::Update()
{
    switch (disabled) {
        case 0:
            DetectMovers();
            break;
    }
    AdvanceAnim();
}
void HeapOfLeaf::DetectMovers()
{
    s32 a;
    Vec3s b;
    ScnObject *c[64];
    ScnObject *d;
    s32 e;
    a = ObjGrid_QueryPointsInRectXZ(zone->min[0], zone->min[2], zone->max[0], zone->max[2], c);
    for (e = 0; e < a; e++) {
        d = c[e];
        if ((d->GetClassId() == CLASSID_WOLF || d->GetClassId() == CLASSID_SHEEP || d->GetClassId() == CLASSID_BULL) &&
            Vec3s_ManhattanDistXZ(&pos, &d->pos) < radius && ABS_VALUE(pos.y - d->pos.y) < 60 &&
            PointInPolygon(&d->pos) && d->HandleMessage(this, MSG_QUERY_MOVED, 0) == 1) {
            Sound_Play(SND_S04FEMOR, this, 0xff, SNDF_NO_RETRIGGER, 0x1000);
            b = d->pos;
            b.y -= 100;
            if (guardian)
                /* cast kept: arg carries a number */
                guardian->HandleMessage(this, MSG_LOUD_NOISE, (void *)(u32)d->GetClassId());
            leafFx.base.Emitter_UpdateDriftAnimated(&driftParams, &b, d->GetFacing(), 1);
            return;
        }
    }
    /* Source position is intentionally unset on the non-spawning update path. */
    leafFx.base.Emitter_UpdateDriftAnimated(&driftParams, &b, GetFacing(), 0);
}
s32 HeapOfLeaf::PointInPolygon(Vec3s *point)
{
    s32 a;
    u8 b;
    s32 c;
    u8 d;
    s32 e;
    b = 0;
    a = 0;
    pipPrev = polygon->pts;
    c = point->z >= pipPrev->z;
    do {
        /* cast kept: a pointer or handle used as a number here */
        if ((u32)b == polygon->count - 1)
            d = 0;
        else
            d = b + 1;
        pipCur = &polygon->pts[d];
        e = point->z >= pipCur->z;
        if (c != e && ((pipCur->z - point->z) * (pipPrev->x - pipCur->x) >=
                       (pipCur->x - point->x) * (pipPrev->z - pipCur->z)) == e)
            a = !a;
        pipPrev = pipCur;
        b = d;
        c = e;
    } while (b);
    return a;
}
void HeapOfLeaf::ComputeRadius()
{
    u16 a;
    u16 b;
    radius = (u16)Vec3s_ManhattanDistXZ(&pos, polygon->pts);
    for (a = 1; a < polygon->count; a++) {
        b = (u16)Vec3s_ManhattanDistXZ(&pos, &polygon->pts[a]);
        if (radius < b)
            radius = b;
    }
}
s32 HeapOfLeaf::HandleMessage(ScnObject *sender, u32 message, void *arg)
{
    return 0;
}
void HeapOfLeaf::Render(Camera *view)
{
    if (leafFx.base.flags.active)
        leafFx.base.Emitter_Render(view, 0);
}
void HeapOfLeaf::Reset() {}
ScnObject *HeapOfLeaf_Create(void *record)
{
    ScnBody *object = new HeapOfLeaf;
    object = object->Init(record, 0);
    return object;
}
