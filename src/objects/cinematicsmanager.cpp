/* T128 - original object CinematicsManager.cpp (guessed name).
 * Ranges: .text 0x4ac5b0-0x4acd80, .rdata 0x575d98-0x575dbc (vtable), .data 0x57b460-0x57b540 (the cinematic
 * header's static opcode-stride copy, g_cineMgrPropOffsets, then the string literal of PostLoadInit).
 * CinematicsManager's methods, then Cine_ResolveText 0x4acd5f (the last function of the main .text; only caller
 * Sam_Init).
 * PAL PC CinematicsManager, 0x4ac5b0-0x4acd5f. */
/* BYTES: layout, slot-group. */
/* BYTES(layout): the Cine.h header static: every object including the cinematic header carries this copy in its .data, referenced or not */
/* ScnObject::Text_GetClassString is declared returning char *, as it is defined, so the decorated names agree at link. */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
#define SDW_MEMBERS_ScnObject            \
    static void *operator new(u32 size); \
    void SetUpdateMode(s32 mode);


#include "sdw_classes.h"
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32 1
#define SDW_INLINE_SCNOBJECT_SETVISIBLE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32
#undef SDW_INLINE_SCNOBJECT_SETVISIBLE_S32
#define SDW_INLINE_SCNOBJECT_ENABLEBOXCOLLIDE_S32 1
#define SDW_INLINE_SCNOBJECT_INSTFLAGS_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_ENABLEBOXCOLLIDE_S32
#undef SDW_INLINE_SCNOBJECT_INSTFLAGS_U16
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#define SDW_INLINE_CINE_ISACTIVE 1
#include "../engine/cine_inlines.h"
#undef SDW_INLINE_CINE_ISACTIVE
extern Wolf *g_pWolf;
#include "../engine/cine.h"
#include "../engine/id_list.h"
#include "../engine/input.h"

/* 0x57b460 - the cinematic header's static copy of the 9-byte opcode stride table (src/engine/cine.cpp,
 * g_cineOpStride 0x5816fc). The original header defined it static, so every object including it carries its own
 * unreferenced copy at the head of its .data; defined here in its place. */
static u8 s_cineOpStride[9] = {0, 8, 8, 4, 2, 2, 4, 2, 2};
/* 0x57b470 - the property offsets of the fifteen trigger groups {id, act box, box, flags, text, sheep box} */
u16 g_cineMgrPropOffsets[15][6] = {
    {0, 4, 8, 12, 20, 16},          {24, 28, 32, 36, 44, 40},       {48, 52, 56, 60, 68, 64},
    {72, 76, 80, 84, 92, 88},       {96, 100, 104, 108, 116, 112},  {120, 124, 128, 132, 140, 136},
    {144, 148, 152, 156, 164, 160}, {168, 172, 176, 180, 188, 184}, {192, 196, 200, 204, 212, 208},
    {216, 220, 224, 228, 236, 232}, {240, 244, 248, 252, 260, 256}, {264, 268, 272, 276, 284, 280},
    {288, 292, 296, 300, 308, 304}, {312, 316, 320, 324, 332, 328}, {336, 340, 344, 348, 356, 352}};
s32 Scenaric_FindByClass(u16 id, ScnObject **out, s32 max);
void Debug_Printf(const char *fmt, ...);
#define SDW_INLINE_SCNOBJECT_GETPARENT 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETPARENT
inline void StartTrigger(u32 id, u32 flags, Box *box, Box *sheep, void *text)
{
    g_cinePlayer.Start(id, flags, box, sheep, text, 0);
}

/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
void CinematicsManager::PostLoadInit()
{
    /* Observed EBP -28..-4 storage; names and aggregation are not recovered. */
    struct Work {
        u32 sheepBoxId, actBoxId;
        u32 *list;
        u32 flags, index, text;
        u16 unused, count;
        u32 id;
        void *props;
        u32 boxId;
    } w;
    SetUpdateMode(SCN_UPD_ALWAYS);
    SetVisible(0);
    EnableBoxCollide(0);
    mcardManager = 0;
    Scenaric_FindByClass(CLASSID_MCARDMANAGER, &mcardManager, 1);
    saveRequested = 0;
    w.props = record;
    triggerCount = 0;
    for (w.index = 0; w.index < 15; ++w.index) {
        /* cast kept (these six lines): designer properties are 4-byte slots at byte offsets of the raw WAR record */
        w.id = *(u32 *)((u8 *)w.props + g_cineMgrPropOffsets[w.index][0] + 0x14);
        w.actBoxId = *(u32 *)((u8 *)w.props + g_cineMgrPropOffsets[w.index][1] + 0x14);
        w.boxId = *(u32 *)((u8 *)w.props + g_cineMgrPropOffsets[w.index][2] + 0x14);
        /* cast kept: a designer-property record read at its byte offset */
        w.flags = *(u32 *)((u8 *)w.props + g_cineMgrPropOffsets[w.index][3] + 0x14);
        w.text = *(u32 *)((u8 *)w.props + g_cineMgrPropOffsets[w.index][4] + 0x14);
        w.sheepBoxId = *(u32 *)((u8 *)w.props + g_cineMgrPropOffsets[w.index][5] + 0x14);
        if (w.actBoxId) {
            triggers[triggerCount].cineId = (u16)w.id;
            triggers[triggerCount].flags = w.flags;
            triggers[triggerCount].played = 0;
            w.list = Scn_FindIdList((u16)w.actBoxId, &w.count);
            if (!w.list) {
                Debug_Printf("Erreur CinematicsManager");
                continue;
            }
            /* cast kept (the three box lookups): an id list holds record addresses as u32 words; these list boxes */
            triggers[triggerCount].actBox = (Box *)*w.list;
            triggers[triggerCount].text = (w.flags & CINE_HAS_TEXT) ? Text_GetClassString((u8)w.text) : 0;
            w.list = Scn_FindIdList((u16)w.boxId, &w.count);
            /* cast kept: the id list holds this box */
            triggers[triggerCount].cineBox = w.count ? (Box *)*w.list : 0;
            w.list = Scn_FindIdList((u16)w.sheepBoxId, &w.count);
            triggers[triggerCount].sheepBox = w.count ? (Box *)*w.list : 0;
            triggers[triggerCount].started = 0;
            ++triggerCount;
        }
    }
}
/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets; unused2 fill gaps */
void CinematicsManager::Update()
{
    struct Work {
        u32 flags;
        u16 unused, cineId;
        CollBox *box;
        s32 index;
        Vec3s pos;
        u16 unused2;
    } w;
    w.pos = g_pWolf->pos;
    if (saveRequested && mcardManager && mcardManager->HandleMessage(this, MSG_QUERY_ACTION, 0) != CTX_SAVE)
        return;
    for (w.index = 0; w.index < triggerCount; ++w.index) {
        /* cast kept: Box and CollBox are two views of one 16-byte record */
        w.box = (CollBox *)triggers[w.index].actBox;
        w.cineId = triggers[w.index].cineId;
        if (g_cinePlayer.IsActive())
            return;
        if (w.pos.x >= w.box->min.x && w.pos.x <= w.box->max.x && w.pos.y >= w.box->min.y && w.pos.y <= w.box->max.y &&
            w.pos.z >= w.box->min.z && w.pos.z <= w.box->max.z) {
            if (!g_pWolf->InstFlags(INST_F_ATTACHED) || g_pWolf->GetParent()->GetClassId() != CLASSID_SAM) {
                w.flags = triggers[w.index].flags;
                if (triggers[w.index].flags & CINE_REQUIRE_CONFIRM) {
                    if (Pad_MenuPressed((u16)~PAD_CROSS)) {
                        StartTrigger(w.cineId, w.flags, triggers[w.index].cineBox, triggers[w.index].sheepBox,
                                     triggers[w.index].text);
                        triggers[w.index].started = 1;
                    }
                } else if (!triggers[w.index].played) {
                    if ((triggers[w.index].flags & CINE_SAVE_FIRST) && !saveRequested) {
                        saveRequested = 1;
                        if (!mcardManager->HandleMessage(this, MSG_MCARD_AUTOSAVE, 0))
                            mcardManager->HandleMessage(this, MSG_USE, 0);
                        return;
                    }
                    if (!(triggers[w.index].flags & CINE_REPEATABLE))
                        triggers[w.index].played = 1;
                    StartTrigger(w.cineId, w.flags, triggers[w.index].cineBox, triggers[w.index].sheepBox,
                                 triggers[w.index].text);
                    triggers[w.index].started = 1;
                }
            }
        }
    }
}
s32 CinematicsManager::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    s32 index;
    switch (msgId) {
        case MSG_CINE_REARM:
            for (index = 0; index < triggerCount; ++index)
                if (triggers[index].actBox == arg)
                    triggers[index].played = 0;
            return 1;
    }
    return 0;
}
ScnObject *CinematicsManager_Create(u16 *record)
{
    CinematicsManager *object = new CinematicsManager;
    object = (CinematicsManager *)object->Init(record); /* cast kept: Init returns the object as its ScnObject base */
    return object;
}

/* 0x4acd5f - class-local cinematic dialogue lookup */
const char *Cine_ResolveText(ScnObject *owner, u32 cineFlags, u8 textIndex)
{
    if (cineFlags & CINE_HAS_TEXT)
        return owner->Text_GetClassString(textIndex);
    return 0;
}
