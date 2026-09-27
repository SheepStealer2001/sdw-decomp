/* PAL PC CheckpointManager, 0x4ab390-0x4abcbc. */
/* BYTES: dead-code, slot-group. */
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
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_ENABLEBOXCOLLIDE_S32
extern Wolf *g_pWolf;
#include "sheep.h"
#include "../engine/id_list.h"
#include "../engine/scn_tools.h"
extern u32 g_gameTime, g_gameFlags;
s32 Scenaric_FindByClass(u16 id, ScnObject **out, s32 maximum);
/* Original 57b420, retained by content relocation, not an address alias. */
static u16 checkpointPropertyOffsets[8][4] = {{0, 4, 12, 8},       {16, 20, 28, 24},    {32, 36, 44, 40},
                                              {48, 52, 60, 56},    {64, 68, 76, 72},    {80, 84, 92, 88},
                                              {96, 100, 108, 104}, {112, 116, 124, 120}};
#define SDW_INLINE_FREE_READPROPERTY_VOID_U32 2
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_READPROPERTY_VOID_U32

CheckpointEntry *CheckpointManager::FindEntryAt(Vec3s *position)
{
    s32 count;
    CheckpointEntry *entry;
    for (count = 0, entry = entries; count < entryCount; ++count, ++entry) {
        if (!entry->triggers)
            continue;
        if (BoxList_FindContainingPoint(position, entry->triggers, entry->count))
            return entry;
    }
    return 0;
}
/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets */
/* BYTES(dead-code): the first three sums are overwritten, as in the original */
void CheckpointManager::Update()
{
    struct Work {
        s32 earlier;
        CollBox *box;
        s32 index;
        Sheep *sheep;
        CheckpointEntry *sheepEntry;
        Vec3s position;
        u16 yaw;
        CheckpointEntry *wolfEntry;
        s32 wolfBusy;
    } w;
    if (sam && sam->HandleMessage(this, MSG_SAM_IS_RED, 0))
        lastChaseTime = g_gameTime;
    w.wolfBusy = g_pWolf->HandleMessage(this, MSG_WOLF_IS_DEAD, 0);
    if (g_gameTime >= lastChaseTime + 0x2000 && !w.wolfBusy && !(g_gameFlags & GF_FADE_RESTART)) {
        w.wolfEntry = FindEntryAt(&g_pWolf->pos);
        if (w.wolfEntry && !w.wolfEntry->wolfDestination)
            w.wolfEntry = 0;
        w.sheep = g_pSheepOutOfZone;
        if (w.sheep) {
            w.sheepEntry = FindEntryAt(&w.sheep->pos);
            if (w.sheepEntry && !w.sheepEntry->sheepDestination)
                w.sheepEntry = 0;
        } else
            w.sheepEntry = 0;
        if (w.wolfEntry) {
            w.index = w.wolfEntry - entries;
            if (w.wolfEntry == w.sheepEntry || !(jointMask & (1 << w.index))) {
                w.yaw = w.wolfEntry->yaw;
                w.box = w.wolfEntry->wolfDestination;
                /* These first three sums are really overwritten in the original. */
                w.position.x = w.box->min.x + w.box->max.x;
                w.position.y = w.box->min.y + w.box->max.y;
                w.position.z = w.box->min.z + w.box->max.z;
                w.position.x = (w.box->min.x + w.box->max.x) >> 1;
                w.position.y = (w.box->min.y + w.box->max.y) >> 1;
                w.position.z = (w.box->min.z + w.box->max.z) >> 1;
                g_pWolf->HandleMessage(this, MSG_WOLF_SAVE_RESPAWN, &w.position);
                if (disableEarlierMask & (1 << w.index)) {
                    for (w.earlier = 0; w.earlier < w.index; ++w.earlier)
                        entries[w.earlier].triggers = 0;
                    w.sheep->HandleMessage(this, MSG_SHEEP_KEEP_CHECKPOINT, 0);
                }
            }
        }
        if (w.sheepEntry) {
            w.index = w.sheepEntry - entries;
            if (w.wolfEntry == w.sheepEntry || !(jointMask & (1 << w.index))) {
                w.yaw = w.sheepEntry->yaw;
                w.box = w.sheepEntry->sheepDestination;
                w.position.x = (w.box->min.x + w.box->max.x) >> 1;
                w.position.y = (w.box->min.y + w.box->max.y) >> 1;
                w.position.z = (w.box->min.z + w.box->max.z) >> 1;
                w.sheep->HandleMessage(this, MSG_SHEEP_SAVE_RESPAWN, &w.position);
            }
        }
    }
}
s32 CheckpointManager::HandleMessage(ScnObject *sender, u32 msgId, void *arg)
{
    if (msgId == MSG_CHECKPOINT_ACTIVATE) {
        switch ((s32)arg) { /* cast kept: this message passes a number in its void * argument */
            case 0:
                SetUpdateMode(SCN_UPD_NEVER);
                break;
            case 1:
                SetUpdateMode(SCN_UPD_ALWAYS);
                break;
        }
        lastChaseTime = 0;
        return 1;
    }
    return 0;
}
void CheckpointManager::Reset()
{
    lastChaseTime = 0;
}
/* BYTES(slot-group, inferred): locals grouped in w only to pin the original frame offsets */
void CheckpointManager::PostLoadInit()
{
    struct Work {
        u32 offset;
        CheckpointEntry *entry;
        s32 index;
        u16 unused, id;
        u32 *list;
        void *props;
        u16 unused2, count;
        u32 mask;
    } w;
    SetVisible(0);
    EnableBoxCollide(0);
    entryCount = 0;
    jointMask = 0;
    disableEarlierMask = 0;
    w.props = record;
    w.offset = 0x80;
    /* cast kept (both): designer properties are 4-byte slots at byte offsets of the raw WAR record */
    w.mask = *(u32 *)((u8 *)w.props + w.offset + 0x14);
    for (w.index = 0; w.index < 8; ++w.index) {
        w.id = (u16)ReadProperty(w.props, checkpointPropertyOffsets[w.index][0]);
        if (w.id) {
            w.list = Scn_FindIdList(w.id, &w.count);
            if (w.list) {
                w.entry = entries + entryCount;
                /* cast kept: an export id list holds record pointers of any kind; this one lists boxes */
                w.entry->triggers = (Box **)w.list;
                w.entry->count = w.count;
                w.entry->wolfDestination = 0;
                w.entry->sheepDestination = 0;
                w.id = (u16)ReadProperty(w.props, checkpointPropertyOffsets[w.index][2]);
                if (w.id) {
                    w.list = Scn_FindIdList(w.id, &w.count);
                    if (w.list)
                        /* cast kept: an export id list holds record pointers of any kind; this one lists a box */
                        w.entry->wolfDestination = (CollBox *)*w.list;
                }
                w.id = (u16)ReadProperty(w.props, checkpointPropertyOffsets[w.index][3]);
                if (w.id) {
                    w.list = Scn_FindIdList(w.id, &w.count);
                    if (w.list)
                        /* cast kept: an export id list holds record pointers of any kind; this one lists a box */
                        w.entry->sheepDestination = (CollBox *)*w.list;
                }
                w.entry->yaw =
                    (((s32)ReadProperty(w.props, checkpointPropertyOffsets[w.index][1]) << 11) / 180) & 0xfff;
                if (w.mask & (1 << w.index))
                    jointMask |= 1 << entryCount;
                if (w.mask & (1 << (w.index + 20)))
                    disableEarlierMask |= 1 << entryCount;
                ++entryCount;
            }
        }
    }
    sam = 0;
    /* cast kept: Scenaric_FindByClass fills a ScnObject * array; the field is typed as the Sam it finds */
    Scenaric_FindByClass(CLASSID_SAM, (ScnObject **)&sam, 1);
    lastChaseTime = 0;
}
ScnObject *CheckpointManager_Create(u16 *record)
{
    CheckpointManager *object = new CheckpointManager;
    object = (CheckpointManager *)object->Init(record); /* cast kept: Init returns the ScnObject base */
    object->SetUpdateMode(SCN_UPD_ALWAYS);
    return object;
}
