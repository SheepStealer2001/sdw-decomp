/*
 * T159 - original object Goal.cpp (guessed name), one translation unit.
 *   .text  0x4c74a0-0x4c7922 (Goal_Update .. Goal_Create)
 *   .rdata 0x5762c4-0x5762e8 (??_7Goal)
 *   .data  0x57b66c-0x57b678 (this TU's copy of the Cine header's 9-byte opcode stride table, + 3 pad)
 */
/* BYTES: dead-code, layout, slot-scope. */
/* BYTES(layout): the Cine.h header static: every object including the cinematic header carries this copy in its .data, referenced or not */
/* PAL PC Goal, 0x4c74a0-0x4c7921. Original update and message-slot behavior. */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
#define SDW_MEMBERS_ScnObject            \
    static void *operator new(u32 size); \
    CollBox *GetFirstModelBox();

#define SDW_MEMBERS_CollBox u32 ContainsPointXZMask(Vec3s *point);
#include "sdw_classes.h"
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_INSTFLAGS_U16 1
#define SDW_INLINE_SCNOBJECT_SETVISIBLE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_INSTFLAGS_U16
#undef SDW_INLINE_SCNOBJECT_SETVISIBLE_S32
#define SDW_INLINE_SCNOBJECT_SETMOVEMENTENABLED_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETMOVEMENTENABLED_S32
/* 0x57b66c - this TU's copy of the cinematic header's static opcode stride table (payload bytes per key, by opcode;
   the copies src/engine/cine.cpp and cine2.cpp index are g_cineOpStride 0x5816fc, g_cineOpStride2 and
   g_cineOpStride3). A static table in a header that every cinematic .cpp includes: each includer gets its own
   unreferenced copy in .data, ahead of its own data. Local definition standing in for that header, named
   s_cineOpStride as in the other batches (a static must not reuse the global name g_cineOpStride). */
static u8 s_cineOpStride[9] = {0, 8, 8, 4, 2, 2, 4, 2, 2};
extern Wolf *g_pWolf;
#include "../engine/cine.h"
#include "../engine/scn_tools.h"
s32 Vec3s_DistSqXZ(Vec3s *, Vec3s *);
/* View of the original packed flag byte, with no change to the class layout. */
#define SDW_INLINE_SCNOBJECT_GETPARENT 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETPARENT
inline CollBox *ScnObject::GetFirstModelBox()
{
    ModelBoxList *list = inst_model->boxes;
    if (list)
        return list->boxes;
    return 0;
}
inline u32 GoalProperty(void *record, u32 offset)
{
    /* cast kept: designer properties are 4-byte slots at byte offsets of the raw WAR record */
    return *(u32 *)((u8 *)record + offset + 0x14);
}
inline void GoalCine(u32 id, u32 flags, Box *box, Box *sheep, void *text)
{
    g_cinePlayer.Start(id, flags, box, sheep, text, 0);
}
#define SDW_INLINE_FREE_BOXOVERLAP4_S32_S32_S32_S32 1
#include "../engine/coll_box_inlines.h"
#undef SDW_INLINE_FREE_BOXOVERLAP4_S32_S32_S32_S32
inline u32 CollBox::ContainsPointXZMask(Vec3s *point)
{
    s32 z = point->z;
    s32 x = point->x;
    return BoxOverlap4(max.x - x, x - min.x, max.z - z, z - min.z);
}

/* BYTES(slot-scope, inferred): the nested block(s) only order the frame: their locals are allocated after the enclosing scope's */
/* BYTES(dead-code): reply is stored and never read, as in the original */
void Goal::Update()
{
    s32 reply;
    {
        CollBox bounds;
        {
            ScnObject *sheep;
            {
                Vec3s *wolfPos;
                if (!goalFlags.triggered) {
                    CollBox *box = GetFirstModelBox();
                    wolfPos = &g_pWolf->pos;
                    bounds.Box_Translate(box, &pos);
                    if (bounds.ContainsPointXZMask(wolfPos) && wolfPos->y >= bounds.min.y &&
                        wolfPos->y <= bounds.max.y && Vec3s_DistSqXZ(wolfPos, &pos) <= (s32)winRadiusSq) {
                        sheep = Scenaric_FindNearestOfClass(&pos, CLASSID_SHEEP, bounds.min.y, bounds.max.y, winRadius,
                                                            0, 0);
                        if (sheep || goalFlags.noSheep) {
                            /* Preserve the original dereference even on the no-sheep branch. */
                            if (sheep->InstFlags(INST_F_ATTACHED) && sheep->GetParent()->GetClassId() == CLASSID_SAM)
                                return;
                            if (goalFlags.cinematic) {
                                GoalCine(cinId, cinFlags, cinBox, cinSheepBox, cinText);
                                goalFlags.triggered = 1;
                            } else {
                                reply = g_pWolf->HandleMessage(this, MSG_WOLF_WIN, 0);
                            }
                        }
                    }
                }
            }
        }
    }
}

/* Original symbol Goal_Reset occupies the message slot and simply returns zero. */
s32 Goal::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    return 0;
}

/* BYTES(slot-scope, inferred): the nested block(s) only order the frame: their locals are allocated after the enclosing scope's */
void Goal::PostLoadInit()
{
    u32 options;
    {
        u16 *props = record;
        SetVisible(0);
        SetMovementEnabled(0);
        winRadius = GetFirstModelBox()->max.x * 90 / 128;
        winRadiusSq = winRadius * winRadius;
        goalFlags.triggered = 0;
        options = GoalProperty(props, 12);
        if (options & GOAL_OPT_CINEMATIC)
            goalFlags.cinematic = 1;
        else
            goalFlags.cinematic = 0;
        if (options & GOAL_OPT_NO_SHEEP)
            goalFlags.noSheep = 1;
        else
            goalFlags.noSheep = 0;
        if (goalFlags.cinematic) {
            cinId = (u16)GoalProperty(props, 4);
            cinFlags = GoalProperty(props, 16);
            cinBox = Scn_GetPropBox(props, 0);
            cinSheepBox = Scn_GetPropBox(props, 8);
            if (cinFlags & CINE_HAS_TEXT)
                cinText = Text_GetClassString((u8)GoalProperty(props, 20));
            else
                cinText = 0;
        }
    }
}

ScnObject *Goal_Create(void *record)
{
    Goal *obj = new Goal;
    obj = (Goal *)obj->Init(record); /* cast kept: Init returns the object as a ScnObject * */
    return obj;
}
