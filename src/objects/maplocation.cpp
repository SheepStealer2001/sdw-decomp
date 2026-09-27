/*
 * T182 - original object MapLocation.cpp (guessed name), one translation unit.
 *   .text  0x4d3e40-0x4d42bb (MapLocation_AddSlot .. MapLocation_Create)
 *          + COMDAT 0x4d42c0 ScnObject_Nop: the empty inline virtual that /OPT:ICF folded the base classes' empty slots
 *          into (ScnObject::Reset, ScnLogic::Update, ScnBody's PostLoadInit, ...). MapLocation's copy is the one LINK
 *          kept, so this object defines the fold-group member ScnObject::Reset as an inline (a COMDAT, emitted because
 *          MapLocation's vtable refers to it).
 *   .rdata 0x576600-0x576624 (??_7MapLocation)
 */
/* BYTES: layout. */
/* BYTES(layout): ScnObject::Reset (inline, COMDAT 0x4d42c0): inline so this object emits the COMDAT that /OPT:ICF keeps for every empty base virtual (0x4d42c0) */
/* PAL PC 0x4d3e40-0x4d42bb. The UiQuad declarations follow the definitions at 0x539c29 / 0x539d91 (s32 x/y
 * parameters). */
#include "sdw_types.h"
#define SDW_MEMBERS_ScnObject            \
    static void *operator new(u32 size); \
    void SetUpdateMode(s32 mode);
#define SDW_MEMBERS_ZoneList void Load(u32 id);


#include "sdw_enums.h"
#include "scenaric_props.h"
#include "sdw_classes.h"
#include "../engine/id_list.h"
#include "../engine/scn_tools.h"
#define SDW_INLINE_SCNOBJECT_SETVISIBLE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETVISIBLE_S32
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32
#define SDW_INLINE_UIQUAD_SETFADELEVEL_U8 1
#include "../engine/ui_quad_inlines.h"
#undef SDW_INLINE_UIQUAD_SETFADELEVEL_U8
#define SDW_INLINE_UIQUAD_SETCOLOR_U32 1
#include "../engine/ui_quad_inlines.h"
#undef SDW_INLINE_UIQUAD_SETCOLOR_U32
extern Wolf *g_pWolf;
#define SDW_INLINE_FREE_PROPU32_VOID_U32 2
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_PROPU32_VOID_U32
#define SDW_INLINE_ZONELIST_LOAD_U32 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_LOAD_U32
#define SDW_INLINE_ZONELIST_CONTAINSXZ_VEC3S 1
#include "../engine/zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_CONTAINSXZ_VEC3S

/* 0x4d3e40 MapLocation_AddSlot */
void MapLocation::AddSlot(s32 boxOffset, s32 xOffset, s32 yOffset)
{
    u32 p = PropU32(props, boxOffset);
    if (p) {
        slots[slotCount].zones.Load(p);
        if (slots[slotCount].zones.count > 0) {
            slots[slotCount].mapX = PropU32(props, xOffset) << 1;
            slots[slotCount].mapY = PropU32(props, yOffset);
            slotCount++;
        }
    }
}

/* 0x4d3f1d MapLocation_Init */
void MapLocation::PostLoadInit()
{
    u16 p;
    void *q = record;
    props = q;
    slotCount = 0;
    AddSlot(0, 4, 8);
    AddSlot(0xc, 0x10, 0x14);
    AddSlot(0x18, 0x1c, 0x20);
    AddSlot(0x24, 0x28, 0x2c);
    AddSlot(0x30, 0x34, 0x38);
    AddSlot(0x3c, 0x40, 0x44);
    AddSlot(0x48, 0x4c, 0x50);
    AddSlot(0x54, 0x58, 0x5c);
    AddSlot(0x60, 0x64, 0x68);
    AddSlot(0x6c, 0x70, 0x74);
    AddSlot(0x78, 0x7c, 0x80);
    AddSlot(0x84, 0x88, 0x8c);
    AddSlot(0x90, 0x94, 0x98);
    AddSlot(0x9c, 0xa0, 0xa4);
    AddSlot(0xa8, 0xac, 0xb0);
    p = 1;
    markerBitmap = IdList_FindWithCount(DAV_IDI_IGLCOYO1, &p);
    SetVisible(0);
    SetUpdateMode(SCN_UPD_NEVER);
}

/* 0x4d4148 MapLocation_DrawWolfMarker */
s32 MapLocation::DrawWolfMarker(u8 fade, u32 color)
{
    s32 p;
    for (p = 0; p < slotCount; p++) {
        if (slots[p].zones.ContainsXZ(&g_pWolf->pos)) {
            /* cast kept: an id list holds record pointers of any kind (a bitmap record here) */
            marker.UiQuad_SetFromBitmap((const u16 *)*markerBitmap, slots[p].mapX, slots[p].mapY, 1, 1, 0x400, 0x400);
            marker.SetFadeLevel(fade);
            marker.SetColor(color);
            marker.UiQuad_Draw(0xb);
            return 1;
        }
    }
    return 0;
}

/* 0x4d4256 MapLocation_Create */
ScnObject *MapLocation_Create(void *record)
{
    MapLocation *obj = new MapLocation;
    obj = (MapLocation *)obj->Init(record); /* cast kept: Init returns the object as a ScnObject * */
    return obj;
}

/* 0x4d42c0 ScnObject_Nop: the inherited Reset slot. Inline: the original's empty virtuals are header inlines, emitted
   as COMDATs where a vtable needs them. */
inline void ScnObject::Reset() {}
