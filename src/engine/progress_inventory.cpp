/*
 * T246 - original object Inventory.cpp (guessed name), one translation unit: Inventory_CountClass 0x50c380 ..
 * Inventory_CommitAtCheckpoint, ending 0x50d533.
 * .text 0x50c380-0x50d533, .data 0x57b834-0x57b838 (the string "x%d"), .bss 0x6cfc28-0x6cff98.
 * The Progress half is T245 (src/engine/progress.cpp). Twelve object-private .bss globals are named for their .bss
 * order, see the block before the functions.
 */
/* BYTES: bss-name, dead-code, flow, inline, slot-name, view. */
/* BYTES(inline): shared ScnObject::SetPos / GetClassId / IsKept / SetKept: 'this' is a temporary (0x50d00f) */
/* BYTES(inline): shared Progress::SetSecondDemoNext: neg/sbb/neg on a constant (0x50c08d) */
/* BYTES(inline): shared Progress::FieldAcFlagClear: the branchy 0/1 temporary at 0x50bc3d */
/* BYTES(view, inferred): RUNTIME_BITS / OPTION_BITS macros: bitfield views of runtimeFlags / optionBits: the original reads them with VC6's bitfield code */
/* BYTES(bss-name): named for its .bss hash bucket 93 */
/* BYTES(bss-name): named for its .bss hash bucket 123 */
/* BYTES(bss-name): named for its .bss hash bucket 164 */
/* BYTES(bss-name): named for its .bss hash bucket 213 */
/* BYTES(bss-name): named for its .bss hash bucket 225 */
/* BYTES(bss-name): named for its .bss hash bucket 278 */
/* BYTES(bss-name): named for its .bss hash bucket 326 */
/* BYTES(bss-name): named for its .bss hash bucket 486 */
/* BYTES(bss-name): named for its .bss hash bucket 544 */
/* BYTES(bss-name): named for its .bss hash bucket 567 */
/* BYTES(bss-name): named for its .bss hash bucket 568 */
/* BYTES(bss-name): named for its .bss hash bucket 616 */
/* BYTES(bss-name): placeholder: an unreferenced 2-byte gap, named for its .bss hash bucket 102 */
/* BYTES(bss-name): placeholder: an unreferenced 2-byte gap, named for its .bss hash bucket 313 */
/* BYTES(bss-name): placeholder: an unreferenced 2-byte gap, named for its .bss hash bucket 441 */
/* BYTES(bss-name, inferred): named for its .bss hash bucket (450 / 699) */
/*
 * The game-progress object and the inventory, SheepD3D.exe 0x50b920-0x50d532 (36 functions). By the int3 padding at
 * 0x50c372 these are two original files: the Progress methods (0x50b920-0x50c371: reset, the TimeKeeper and level-done
 * bitsets, the saved 0x2c-byte record, the scene transitions Level_FinishScene / GotoScene and the scene paths), and the
 * inventory (0x50c380-0x50d532: one list per class id, the selection carousel "InvWheel", the item-fly effect that
 * carries an object between the world and the camera, and the checkpoint commit / rollback of picked-up items).
 * This object is the inventory; the Progress methods are T245.
 *
 * The inline helpers below (IsLevelScene, Progress::SetSecondDemoNext / FieldAcFlagClear / SetLevelDone, List_First,
 * GameFlags_Clear, ScnObject::GetClassId / IsKept / SetKept / SetPos, Scenaric_ClassFlags) have no out-of-line copy in
 * the exe, so their names are not recovered. They are here because their expansions give the original's temporaries: an
 * inline argument that is a plain variable or constant is substituted, anything else (a member, a global pointer used as
 * `this`) is copied into a temporary below the named locals, and a multi-return inline materialises its result through
 * branches (FieldAcFlagClear, 0x50bc3d). Locals are named for their stack slots (src/README.md). A shape that
 * reproduces the bytes is a representation, not proof that the original source read this way.
 */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"


#define SDW_MEMBERS_ScnObject void SetPos(Vec3s *p);

#define SDW_MEMBERS_Mat44 Mat44(); /* 0x4077f0 Mat44_Ctor */
#include "sdw_classes.h"
#define SDW_INLINE_PROGRESS_SETLEVELDONE_S8 1
#include "progress_inlines.h"
#undef SDW_INLINE_PROGRESS_SETLEVELDONE_S8
#define SDW_INLINE_SCNOBJECT_SETPOS_VEC3S 1
#include "scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETPOS_VEC3S
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_ISKEPT 1
#define SDW_INLINE_SCNOBJECT_SETKEPT_S32 1
#include "scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_ISKEPT
#undef SDW_INLINE_SCNOBJECT_SETKEPT_S32
#include "sdw_empty_call.h"

#include "../sdk/windef.h"
#include "../sdk/crt.h"

/* ---- globals (named in the tables) ---- */
#include "progress.h"
#include "draw2d.h"
#include "stream_player.h"
#include "../objects/video_sequence.h"
#include "interface.h"
#include "../app/app_main.h"
#include "scenaric.h"
#include "sfx_volume.h"
#include "input.h"
#include "game_level.h"
#include "list.h"
#include "text.h"
#include "scn_tools.h"
#include "approach.h"
#include "lerp.h"
extern const float g_viewDistFar;  /* 0x5770c4  12000.0, the view distance at setting 0 (T245 .rdata) */
extern const float g_viewDistNear; /* 0x5770c8  4000.0, the view distance at setting 255 (T245 .rdata) */
extern u32 g_gameFlags;            /* 0x6ddf74 */
extern s32 g_dt;                   /* 0x71b300 */
extern s32 g_frameCount2;          /* 0x71b308 */
extern u32 *g_screenLayerBase;     /* 0x585044 */

#define g_camPos (g_camera.pos) /* 0x584d20 */
extern "C" s16 g_sinTable4096[5122];
extern "C" const s16 *g_pCosTable;

/* ---- functions ---- */
LONG Reg_CreateSubKey(HKEY *out, const char *name);                         /* 0x55f8dc */
void Reg_CloseKey(HKEY *key);                                               /* 0x55f93a */
DWORD Reg_ReadBinary(HKEY key, const char *name, void *buf, DWORD bufSize); /* 0x55fb41 */
char Video_PlaySequence(FmvList *list);                                     /* 0x56160f */
s32 Scenaric_FindByClass(u16 classId, ScnObject **out, s32 maximum);        /* 0x5145c5 */

/* a colour word whose channel bytes are also read and written one by one */
union ColorBytes {
    u32 value;
    u8 c[4];
};

u16 Inventory_NextClass(u16 classId);
u16 Inventory_PrevClass(u16 classId);
void Inventory_Add(ScnObject *obj);
void Inventory_Remove(ScnObject *obj);
ScnObject *Inventory_GetSelectedObject();
void InvWheel_DrawSlot(u16 classId, s32 x, s32 y, u32 color, u8 crayonFrame);
void InvWheel_GetSlotPosColor(s32 *outXY, ColorBytes *outColor, s16 angle, s32 slideY);
void InvWheel_Draw(s16 angle, u8 crayonFrame, s32 slideY);
void ItemFly_Stop();

/* inline: a scene number that is a real level (0..31), materialised as 0/1 (0x50bf10) */
/* BYTES(inline): source-only inline: materialised as 0/1 (0x50bf10) */
#define SDW_INLINE_FREE_ISLEVELSCENE_S8 1
#include "progress_inlines.h"
#undef SDW_INLINE_FREE_ISLEVELSCENE_S8

/* inline: the attract-demo alternation bit, stored as `on != 0` (neg/sbb/neg on a constant, 0x50c08d) */
#define SDW_INLINE_PROGRESS_SETSECONDDEMONEXT_S32 1
#include "progress_inlines.h"
#undef SDW_INLINE_PROGRESS_SETSECONDDEMONEXT_S32

/* inline: runtime flag 0 as a 0/1 value, clear = 1 (the branchy 0/1 temporary at 0x50bc3d) */
#define SDW_INLINE_PROGRESS_FIELDACFLAGCLEAR 1
#include "progress_inlines.h"
#undef SDW_INLINE_PROGRESS_FIELDACFLAGCLEAR

/* inline: the first node of a list */
/* BYTES(inline): source-only inline: expansion temporaries */
#define SDW_INLINE_FREE_LIST_FIRST_LISTNODE 1
#include "list_inlines.h"
#undef SDW_INLINE_FREE_LIST_FIRST_LISTNODE

/* inline: clears game flags; the mask is complemented at run time (mov eax,0x100; not eax, 0x50ce39) */
/* BYTES(inline): source-only inline: mov eax,0x100; not eax (0x50ce39) */
#define SDW_INLINE_FREE_GAMEFLAGS_CLEAR_U32 1
#include "game_state_inlines.h"
#undef SDW_INLINE_FREE_GAMEFLAGS_CLEAR_U32

/* BYTES(inline): source-only inline: expansion temporaries */
#define SDW_INLINE_FREE_SCENARIC_CLASSFLAGS_U16 1
#include "scenaric_inlines.h"
#undef SDW_INLINE_FREE_SCENARIC_CLASSFLAGS_U16

/* ---- this object's .bss, 0x6cfc28-0x6cff98 ----
 * VC6 lays out .bss by a hash of the NAMES ((h ^ h>>16) & 1023, h = (h<<2)+(h>>4)+c; later definition first inside a
 * bucket; items of 3..63 bytes 4-aligned, >= 64 bytes 8-aligned), so to reproduce the original's order these globals
 * are named for it (all are private to this object; the descriptive names are in the comments). The three 2-byte gaps at
 * 0x6cfc2a, 0x6cfc4a and 0x6cfc4e are referenced by nothing in the exe: unknown globals of the original, kept as u16
 * fillers. */
u16 g_invSelClass;                     /* 0x6cfc28  g_invSelectedClass: 0xffff = empty hands          bucket 93 */
u16 g_invSpare_6cfc2a;                 /* 0x6cfc2a  unreferenced                                        bucket 102 */
u8 g_invFlyDir;                        /* 0x6cfc2c  g_itemFlyMode                                       bucket 123 */
ScnObject *g_itemFlyObj;               /* 0x6cfc30  g_itemFlyObject                                     bucket 164 */
Vec3s g_itemFlyStart;                  /* 0x6cfc34  g_itemFlyFrom                                       bucket 213 */
Vec3s g_itemFlightViewOffset;          /* 0x6cfc3c  g_itemFlyCamOffset                                  bucket 225 */
Vec3s g_itemFlyCur;                    /* 0x6cfc44  g_itemFlyPos                                        bucket 278 */
u16 g_invUnused6cfc4a;                 /* 0x6cfc4a  unreferenced                                        bucket 313 */
u16 g_itemFlightTime;                  /* 0x6cfc4c  g_itemFlyT                                          bucket 326 */
u16 g_invUnused_6cfc4e;                /* 0x6cfc4e  unreferenced                                        bucket 441 */
u8 g_invMailboxCount;                  /* 0x6cfc50                                                      bucket 450 */
ScnObject *g_inventoryMailboxTable[6]; /* 0x6cfc54  g_invMailboxes                                  bucket 486 */
u8 g_invWheelTurn;                     /* 0x6cfc6c  g_invWheelSide                                      bucket 544 */
s16 g_inventoryWheelYaw;               /* 0x6cfc6e  g_invWheelAngle                                     bucket 567 */
s16 g_invWheelSlide;                   /* 0x6cfc70  g_invWheelSlideY                                    bucket 568 */
u8 g_inventoryAllCommit;               /* 0x6cfc72  g_invAllCommitted                                   bucket 616 */
ListNode *g_inventoryLists[200];       /* 0x6cfc78  one list head per class id                          bucket 699 */

/* ======================================================================== Inventory (0x50c380-0x50d532) */

/* 0x50c380 */
u32 Inventory_CountClass(u16 classId)
{
    return List_Count(&g_inventoryLists[classId]);
}

/* 0x50c399 - the first class after classId that holds an item; from empty hands (0xffff) the first one of all */
u16 Inventory_NextClass(u16 classId)
{
    s32 i;

    if (classId != CLASSID_NONE) {
        for (i = (u16)(classId + 1); i < 200; i++)
            if (List_First(&g_inventoryLists[i]))
                return i;
    } else {
        for (i = 0; i < 200; i++)
            if (List_First(&g_inventoryLists[i]))
                return i;
    }
    return CLASSID_NONE;
}

/* 0x50c428 - the last class before classId that holds an item; from empty hands the last one of all (down to 1) */
u16 Inventory_PrevClass(u16 classId)
{
    s32 i;

    if (classId != CLASSID_NONE) {
        for (i = classId - 1; i >= 0; i--)
            if (List_First(&g_inventoryLists[i]))
                return i;
    } else {
        for (i = 199; i > 0; i--)
            if (List_First(&g_inventoryLists[i]))
                return i;
    }
    return CLASSID_NONE;
}

/* 0x50c4ac - carries an object. A composite whose two parts are both kept is kept itself; an unkept object means
 * the inventory has pickups a checkpoint has not committed yet. */
void Inventory_Add(ScnObject *obj)
{
    ListNode *node;
    ScnObject *parts[2];

    List_AllocateNode(&node);
    node->data = obj;
    List_PushFront(&g_inventoryLists[obj->GetClassId()], node);
    if ((Scenaric_ClassFlags(obj->GetClassId()) & SCN_CF_COMPOSITE_ITEM) &&
        obj->HandleMessage(0, MSG_ITEM_SPLIT_QUERY, parts) && parts[0]->IsKept() && parts[1]->IsKept())
        obj->SetKept(1);
    if (!obj->IsKept())
        g_inventoryAllCommit = 0;
}

/* 0x50c59e - drops an object from its class list; the selection falls back to empty hands when that list empties */
/* BYTES(slot-name): n is named for its stack slot (tools/vc6_locals.py) */
void Inventory_Remove(ScnObject *obj)
{
    ListNode *n; /* named for its stack slot (src/README.md) */
    ListNode **list;

    list = &g_inventoryLists[obj->GetClassId()];
    for (n = *list; n; n = n->next) {
        if (n->data == obj) {
            List_Remove(list, n);
            List_FreeNode(n);
            if (g_invSelClass == obj->GetClassId() && !List_First(list))
                g_invSelClass = CLASSID_NONE;
            break;
        }
    }
}

/* 0x50c635 - the object in hand: the first of the selected class, or 0 for empty hands */
ScnObject *Inventory_GetSelectedObject()
{
    ListNode **list;

    if (g_invSelClass == CLASSID_NONE)
        return 0;
    list = &g_inventoryLists[g_invSelClass];
    return (ScnObject *)List_First(list)->data; /* cast kept: a list node carries its payload as a void * */
}

/* 0x50c66f */
void Inventory_SelectClass(u16 classId)
{
    if (List_First(&g_inventoryLists[classId]))
        g_invSelClass = classId;
    else
        g_invSelClass = CLASSID_NONE;
}

/* 0x50c6a1 - one step forward on the carousel. With only two stops (one class and empty hands) the side flag picks
 * which neighbour is shown, and from empty hands the first press only turns the wheel. */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py) */
ScnObject *Inventory_SelectNext()
{
    u16 after; /* the three names give the original's stack slots (src/README.md) */
    u16 newSel;
    u16 selected;

    newSel = Inventory_NextClass(g_invSelClass);
    if (newSel != g_invSelClass) {
        selected = g_invSelClass;
        after = Inventory_NextClass(newSel);
        if (selected == after) {
            if (g_invSelClass != CLASSID_NONE) {
                g_invWheelTurn = 0;
                g_invSelClass = newSel;
            } else if (g_invWheelTurn) {
                g_invWheelTurn = 0;
                g_invSelClass = newSel;
            } else {
                g_invWheelTurn = 1;
            }
        } else {
            g_invSelClass = newSel;
        }
        g_inventoryWheelYaw = 0xd55;
    }
    return Inventory_GetSelectedObject();
}

/* 0x50c75c - the mirror of Inventory_SelectNext */
ScnObject *Inventory_SelectPrev()
{
    u16 prev;
    u16 before;
    u16 cur;

    prev = Inventory_PrevClass(g_invSelClass);
    if (prev != g_invSelClass) {
        before = Inventory_PrevClass(prev);
        cur = g_invSelClass;
        if (before == cur) {
            if (g_invSelClass != CLASSID_NONE) {
                g_invWheelTurn = 1;
                g_invSelClass = prev;
            } else if (g_invWheelTurn) {
                g_invWheelTurn = 0;
            } else {
                g_invSelClass = prev;
            }
        } else {
            g_invSelClass = prev;
        }
        g_inventoryWheelYaw = 0xaab;
    }
    return Inventory_GetSelectedObject();
}

/* 0x50c811 */
void Inventory_ClearSelection()
{
    g_invSelClass = CLASSID_NONE;
}

/* 0x50c81f - one carousel slot: the class icon with its count, over the crayon frame */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py) */
void InvWheel_DrawSlot(u16 classId, s32 x, s32 y, u32 color, u8 crayonFrame)
{
    s32 top; /* the names give the original's stack slots (src/README.md) */
    s32 sx;
    u32 *dst;
    u32 count;
    ListNode **list;

    if (classId != CLASSID_NONE) {
        list = &g_inventoryLists[classId];
        count = List_Count(list);
        if (count > 1) {
            Text_SetCursor(x + 2, y + 2);
            Text_PrintFmt("x%d", count);
        }
        Scenaric_DrawClassIcon(g_screenLayerBase + 10, classId, x, y, 0, 0, color);
    }
    top = y - (g_animSpriteCrayon1.height >> 1);
    sx = x - (g_animSpriteCrayon1.width >> 1);
    dst = g_screenLayerBase + 10;
    g_animSpriteCrayon1.Draw(dst, sx, top, sx + g_animSpriteCrayon1.width, top + g_animSpriteCrayon1.height, color,
                             crayonFrame, 0);
}

/* 0x50c90d */
s32 InvWheel_IsRotating()
{
    return g_inventoryWheelYaw != 0xc00;
}

/* 0x50c925 - the object the item-fly effect is carrying, while it runs */
ScnObject *ItemFly_GetObject()
{
    if (g_gameFlags & GF_ITEM_FLY)
        return g_itemFlyObj;
    return 0;
}

/* 0x50c941 - opening the wheel resets its angle */
void Inventory_SetWheelOpen(s32 open)
{
    if (open) {
        if (!(g_gameFlags & GF_ITEM_WHEEL_OPEN)) {
            g_gameFlags |= GF_ITEM_WHEEL_OPEN;
            g_inventoryWheelYaw = 0xc00;
        }
    } else {
        GameFlags_Clear(GF_ITEM_WHEEL_OPEN);
    }
}

/* 0x50c98a - a slot's screen position on the wheel's ellipse, and its colour: 0x808080 at the front (angle 0xc00),
 * fading to 0x303030 over a third of a slot step (0x155) either side. */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py) */
/* BYTES(flow, inferred): if (1) with a dead else: the original tests a constant here (probably a compiled-out option) */
/* BYTES(view, inferred): colour read byte by byte through the ColorBytes union and written as one u32 */
void InvWheel_GetSlotPosColor(s32 *outXY, ColorBytes *outColor, s16 angle, s32 slideY)
{
    ColorBytes hi; /* the names give the original's stack slots (src/README.md) */
    ColorBytes dim;
    s32 d;

    outXY[0] = 256 + (g_pCosTable[angle] * 150 >> 12);
    outXY[1] = -150 - (g_sinTable4096[angle] * 200 >> 12) + slideY;
    if (1) {
        d = (s16)((s16)((angle - 0x400) & 0xfff) - 0x800);
        if (d != 0) {
            d = d >= 0 ? d : -d;
            if (d < 0x155) {
                hi.value = 0x808080;
                dim.value = 0x303030;
                d = (d << 12) / 0x155;
                outColor->c[0] = hi.c[0] + ((dim.c[0] - hi.c[0]) * d >> 12);
                outColor->c[1] = hi.c[1] + ((dim.c[1] - hi.c[1]) * d >> 12);
                outColor->c[2] = hi.c[2] + ((dim.c[2] - hi.c[2]) * d >> 12);
            } else {
                outColor->value = 0x303030;
            }
        } else {
            outColor->value = 0x808080;
        }
    } else {
        outColor->value = 0x303030;
    }
}

/* 0x50caca - the carousel: the selected class in front, its neighbours either side, and while the wheel turns the
 * class coming into view. */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py) */
void InvWheel_Draw(s16 angle, u8 crayonFrame, s32 slideY)
{
    s32 a4; /* the names give the original's stack slots (src/README.md) */
    u16 next;
    s32 pos[2];
    u16 prev;
    ColorBytes color;
    u16 extra;
    s32 single;

    Text_SetFont(FONT_DEBUG);
    Text_SetColor(0x505090);
    Text_SetWindow(g_screenLayerBase + 8, 0, 0, 0x200, 0xf0, 1);
    prev = Inventory_PrevClass(g_invSelClass);
    next = Inventory_NextClass(g_invSelClass);
    single = 0;
    if (prev == next) {
        if (prev == g_invSelClass)
            single = 1;
        else if (g_invWheelTurn)
            prev = CLASSID_NONE;
        else
            next = CLASSID_NONE;
    }
    InvWheel_GetSlotPosColor(pos, &color, angle & 0xfff, slideY);
    InvWheel_DrawSlot(g_invSelClass, pos[0], pos[1], color.value, crayonFrame);
    if (!single) {
        InvWheel_GetSlotPosColor(pos, &color, (angle - 0x155) & 0xfff, slideY);
        InvWheel_DrawSlot(prev, pos[0], pos[1], color.value, 0);
        InvWheel_GetSlotPosColor(pos, &color, (angle + 0x155) & 0xfff, slideY);
        InvWheel_DrawSlot(next, pos[0], pos[1], color.value, 0);
    }
    if (angle != 0xc00) {
        if (angle > 0xc00) {
            extra = Inventory_PrevClass(prev);
            a4 = angle - 0x2aa;
        } else {
            extra = Inventory_NextClass(next);
            a4 = angle + 0x2aa;
        }
        if (extra == g_invSelClass)
            extra = CLASSID_NONE;
        InvWheel_GetSlotPosColor(pos, &color, a4 & 0xfff, slideY);
        InvWheel_DrawSlot(extra, pos[0], pos[1], color.value, 0);
    }
    Text_SetColor(0x808080);
    Hud_EndBox_stub();
}

/* 0x50ccf1 - once per frame from Scenaric_RenderAll, while the wheel is at least partly on screen */
void InvWheel_Render()
{
    if (g_invWheelSlide > -0x20)
        InvWheel_Draw(g_inventoryWheelYaw, ((u32)g_frameCount2 >> 2) & 0xff, g_invWheelSlide);
}

/* 0x50cd29 - starts the item-fly effect: mode 2 carries obj from fromPos into the camera (picked up), mode 1 from the
 * camera back to fromPos (taken out). The carried class becomes the selection. */
void ItemFly_Start(ScnObject *obj, u8 mode, const Vec3s *fromPos)
{
    g_itemFlyObj = obj;
    g_invFlyDir = mode;
    g_itemFlyStart.x = fromPos->x;
    g_itemFlyStart.y = fromPos->y;
    g_itemFlyStart.z = fromPos->z;
    g_itemFlightViewOffset.x = 0;
    g_itemFlightViewOffset.y = -0x78;
    g_itemFlightViewOffset.z = 0x180;
    if (mode == ITEMFLY_TO_INVENTORY)
        g_itemFlightTime = 0;
    else
        g_itemFlightTime = 0xffe;
    g_gameFlags |= GF_ITEM_FLY;
    g_inventoryWheelYaw = 0xc00;
    if (List_First(&g_inventoryLists[g_itemFlyObj->GetClassId()]))
        g_invSelClass = g_itemFlyObj->GetClassId();
    else
        g_invSelClass = CLASSID_NONE;
}

/* 0x50ce02 */
void ItemFly_SetFromPos(const Vec3s *pos)
{
    g_itemFlyStart.x = pos->x;
    g_itemFlyStart.y = pos->y;
    g_itemFlyStart.z = pos->z;
}

/* 0x50ce2f */
void ItemFly_Stop()
{
    g_invFlyDir = ITEMFLY_IDLE;
    GameFlags_Clear(GF_ITEM_FLY);
}

/* 0x50ce50 - once per frame at the end of Game_Frame: moves the flying item along its path (the camera-space point
 * (0, -120, 384) turned into world space, and fromPos) and draws it; turns the wheel back to rest; slides the wheel in
 * while it is open or an item flies, and out otherwise. */
void Inventory_UpdateFx()
{
    s16 dt;
    Vec3s target;

    dt = 0;
    switch (g_invFlyDir) {
        case ITEMFLY_TO_WOLF:
        case ITEMFLY_TO_INVENTORY:
            dt = g_dt * 0x2580 >> 12;
            if (g_invFlyDir != ITEMFLY_TO_INVENTORY)
                dt = -dt;
            if (g_itemFlightTime < 0xfff) {
                Mat44 m;

                g_camera.viewMat.Transpose3x3(&m);
                m.m[3][0] = m.m[3][1] = m.m[3][2] = 0;
                target.x = g_itemFlightViewOffset.x * m.m[0][0] + g_itemFlightViewOffset.y * m.m[1][0] +
                           g_itemFlightViewOffset.z * m.m[2][0] + m.m[3][0];
                target.y = g_itemFlightViewOffset.x * m.m[0][1] + g_itemFlightViewOffset.y * m.m[1][1] +
                           g_itemFlightViewOffset.z * m.m[2][1] + m.m[3][1];
                target.z = g_itemFlightViewOffset.x * m.m[0][2] + g_itemFlightViewOffset.y * m.m[1][2] +
                           g_itemFlightViewOffset.z * m.m[2][2] + m.m[3][2];
                target.x += g_camPos.x;
                target.y += g_camPos.y;
                target.z += g_camPos.z;
                Lerp_SetVecTarget(&target);
                Vec3s_LerpToTarget(&g_itemFlyCur, &g_itemFlyStart, g_itemFlightTime);
                g_itemFlightTime += (u16)dt;
                g_itemFlyObj->SetPos(&g_itemFlyCur);
                g_itemFlyObj->Render(&g_camera);
            } else {
                ItemFly_Stop();
            }
            break;
    }
    g_inventoryWheelYaw = Math_StepAngleTowards(g_inventoryWheelYaw, 0xc00, 0x400);
    if ((g_gameFlags & GF_ITEM_FLY) || (g_gameFlags & GF_ITEM_WHEEL_OPEN)) {
        if (g_invWheelSlide < 0) {
            g_invWheelSlide += (s16)(g_dt * 0xb4 >> 12);
            if (g_invWheelSlide > 0)
                g_invWheelSlide = 0;
        }
    } else if (g_invWheelSlide > -0x20) {
        g_invWheelSlide -= (s16)(g_dt * 0xb4 >> 12);
        if (g_invWheelSlide <= -0x20)
            g_invWheelSlide = -0x20;
    }
}

/* 0x50d10e - per level: empty hands, all lists emptied (their nodes live in the level heap), the level's Mailboxes */
void Inventory_Init()
{
    s32 i;

    g_invSelClass = CLASSID_NONE;
    for (i = 0; i < 200; i++)
        g_inventoryLists[i] = 0;
    g_invMailboxCount = Scenaric_FindByClass(CLASSID_MAILBOX, g_inventoryMailboxTable, 6);
    g_invWheelSlide = -0x20;
    g_inventoryWheelYaw = 0xc00;
    g_invWheelTurn = 0;
    g_inventoryAllCommit = 1;
}

/* 0x50d180 - on a restart (Wolf_Reset): everything picked up since the last checkpoint goes back. Pass 1 splits the
 * unkept composites into their parts; pass 2 puts every unkept object back into the world and resets it (msg 0x55).
 * The Mailboxes are told to roll back as well. */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py) */
/* BYTES(dead-code): ret is stored and never read, as in the original */
void Inventory_DropUncommitted()
{
    ScnObject *parts[2]; /* the names give the original's stack slots (src/README.md) */
    s32 i;
    s32 ret;
    ListNode *walk;
    ScnObject *item;
    ListNode **head;

    if (!g_inventoryAllCommit) {
        for (i = 0, head = g_inventoryLists; i < 200; i++, head++) {
            walk = *head;
            while (walk) {
                item = (ScnObject *)walk->data; /* cast kept: a list node carries its payload as a void * */
                walk = walk->next;
                if ((Scenaric_ClassFlags(item->GetClassId()) & SCN_CF_COMPOSITE_ITEM) && !item->IsKept() &&
                    item->HandleMessage(0, MSG_ITEM_SPLIT_QUERY, parts)) {
                    Inventory_Add(parts[0]);
                    parts[0]->HandleMessage(0, MSG_INVENTORY_STORED, 0);
                    Inventory_Add(parts[1]);
                    parts[1]->HandleMessage(0, MSG_INVENTORY_STORED, 0);
                    item->HandleMessage(0, MSG_ITEM_CONSUMED, 0);
                    item->HandleMessage(0, MSG_INVENTORY_TAKE_OUT, 0);
                    Inventory_Remove(item);
                }
            }
        }
        for (i = 0, head = g_inventoryLists; i < 200; i++, head++) {
            walk = *head;
            while (walk) {
                item = (ScnObject *)walk->data; /* cast kept: a list node carries its payload as a void * */
                walk = walk->next;
                if (!item->IsKept()) {
                    Inventory_Remove(item);
                    item->AddToWorld(0);
                    ret = item->HandleMessage(0, MSG_CHECKPOINT_ROLLBACK, 0);
                }
            }
        }
    }
    for (i = 0; i < g_invMailboxCount; i++)
        /* cast kept: HandleMessage's arg is a void *; this message passes a number in it */
        g_inventoryMailboxTable[i]->HandleMessage(0, MSG_CHECKPOINT_ROLLBACK, (void *)1);
}

/* 0x50d37f - at a checkpoint (Wolf message 0x402): every carried object, and both parts of a carried composite,
 * become kept. The Mailboxes are told to commit (msg 0x600) every time. */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py) */
void Inventory_CommitAtCheckpoint()
{
    ScnObject *parts[2]; /* the names give the original's stack slots (src/README.md) */
    s32 i;
    ListNode *walk;
    ScnObject *item;
    ListNode **head;

    if (!g_inventoryAllCommit) {
        head = g_inventoryLists;
        for (i = 0; i < 200; i++) {
            for (walk = *head; walk; walk = walk->next) {
                item = (ScnObject *)walk->data; /* cast kept: a list node carries its payload as a void * */
                if (!item->IsKept()) {
                    item->SetKept(1);
                    if ((Scenaric_ClassFlags(item->GetClassId()) & SCN_CF_COMPOSITE_ITEM) &&
                        item->HandleMessage(0, MSG_ITEM_SPLIT_QUERY, parts)) {
                        parts[0]->SetKept(1);
                        parts[1]->SetKept(1);
                    }
                }
            }
            head++;
        }
        g_inventoryAllCommit = 1;
    }
    for (i = 0; i < g_invMailboxCount; i++)
        /* cast kept: HandleMessage's arg is a void *; this message passes a number in it */
        g_inventoryMailboxTable[i]->HandleMessage(0, MSG_CHECKPOINT_COMMIT, (void *)1);
}
