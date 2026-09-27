/* PAL PC Marvin. Uses the shared MarvinFlagBits. */
/* BYTES: temp. */
#include "sdw_types.h"
#include "sdw_enums.h"
class Instance;
struct Animator;
u32 Anim_Start(Instance *, Animator *, u16, u32);

#define SDW_MEMBERS_ScnObject       \
    static void *operator new(u32); \
    void SetFacing(s16 value);      \
    void SetUpdateMode(u8 mode);

#include "sdw_classes.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_SETFACING_S16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETFACING_S16
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_INWORLD 1
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_INWORLD
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8
#define SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32 1
#include "../engine/scn_body_inlines.h"
#undef SDW_INLINE_SCNBODY_PLAYANIM_U16_S32_S32
extern Wolf *g_pWolf;
#include "../engine/scn_tools.h"
#include "../engine/fade.h"
u8 Dialogue_Say(const char *, s32, ScnObject *, u32);
s32 Rand_Bounded(s32);
/* BYTES(temp, inferred): the offset and the result go through the read struct because the original's expansion keeps both in stack slots */
#define SDW_INLINE_FREE_PROPU32_VOID_U32 1
#include "../engine/scn_tools_inlines.h"
#undef SDW_INLINE_FREE_PROPU32_VOID_U32
#define SDW_INLINE_FREE_INBOX_BOX_VEC3S 1
#include "../engine/coll_box_inlines.h"
#undef SDW_INLINE_FREE_INBOX_BOX_VEC3S
void Marvin::PostLoadInit()
{
    void *properties = record;
    cineBox = 0;
    cineBox = Scn_GetPropBox(properties, 4);
    mockeryVoice[0] = VOICE_CIN_LVL_17_OB02A;
    mockeryVoice[1] = VOICE_CIN_LVL_17_OB03A;
    successVoice[0] = VOICE_CIN_LVL_17_OB04A;
    successVoice[1] = VOICE_CIN_04_SC_RET_01;
    mockeryLine[0] = Text_GetClassString((u8)PropU32(properties, 16));
    mockeryLine[1] = Text_GetClassString((u8)PropU32(properties, 20));
    if (!*mockeryLine[0])
        mockeryLine[0] = 0;
    if (!*mockeryLine[1])
        mockeryLine[1] = 0;
    successLine[0] = Text_GetClassString((u8)PropU32(properties, 24));
    successLine[1] = Text_GetClassString((u8)PropU32(properties, 28));
    if (!*successLine[0])
        mockeryLine[0] = 0;
    if (!*successLine[1])
        mockeryLine[1] = 0;
    dialogueFlags.wolfFrozen = 0;
    dialogueFlags.interrupted = 0;
    dialogueFlags.insideCineBox = 0;
    dialogueFlags.solved = 0;
    SetState(MARVIN_ST_IDLE);
    SetUpdateMode(SCN_UPD_NORMAL);
}
void Marvin::Reset()
{
    if (!InWorld())
        return;
    dialogueFlags.wolfFrozen = 0;
    dialogueFlags.interrupted = 0;
    dialogueFlags.insideCineBox = 0;
    SetState(MARVIN_ST_IDLE);
}
void Marvin::Update()
{
    SetFacing(HeadingTo(&g_pWolf->pos));
    switch (state) {
        case MARVIN_ST_IDLE:
            if (InBox(cineBox, &g_pWolf->pos)) {
                if (!dialogueFlags.insideCineBox && !dialogueFlags.interrupted && !dialogueFlags.wolfFrozen &&
                    g_pWolf->HandleMessage(this, MSG_FREEZE, 0)) {
                    dialogueFlags.insideCineBox = 1;
                    dialogueFlags.wolfFrozen = 1;
                    if (dialogueFlags.solved) {
                        successIndex = 0;
                        SetState(MARVIN_ST_SUCCESS);
                    } else {
                        tauntIndex = (u8)Rand_Bounded(2);
                        SetState(MARVIN_ST_MOCK);
                    }
                }
            } else
                dialogueFlags.insideCineBox = 0;
            break;
        case MARVIN_ST_MOCK:
            if (!Dialogue_Say(mockeryLine[tauntIndex], mockeryVoice[tauntIndex], this, 1) &&
                !dialogueFlags.interrupted && dialogueFlags.wolfFrozen &&
                g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0)) {
                dialogueFlags.wolfFrozen = 0;
                SetState(MARVIN_ST_IDLE);
            }
            break;
        case MARVIN_ST_SUCCESS:
            if (successIndex > 1) {
                Fade_StartLevelExit(4096);
                SetState(MARVIN_ST_EXIT);
            } else if (!Dialogue_Say(successLine[successIndex], successVoice[successIndex], this, 1)) {
                successIndex++;
                successIndex &= 3;
            }
    }
    AdvanceAnim();
}
s32 Marvin::HandleMessage(ScnObject *sender, u32 msg, void *arg)
{
    if (sender) {
        switch (msg) {
            case MSG_MARVIN_SOLVED:
                dialogueFlags.solved = (u8)(u32)arg; /* cast kept: this message's arg carries the flag */
                break;
            case MSG_FREEZE:
                if (!sender->GetClassId()) {
                    dialogueFlags.wolfFrozen = 0;
                    dialogueFlags.interrupted = 1;
                    return 1;
                }
                break;
        }
    }
    return 0;
}
void Marvin::SetState(u8 next)
{
    state = next;
    switch (state) {
        case MARVIN_ST_IDLE:
            PlayAnim(AMARVI01_ANIM_STAND1, 1, 1);
            break;
        case MARVIN_ST_MOCK:
            PlayAnim(AMARVI01_ANIM_TALK1, 1, 1);
            break;
        case MARVIN_ST_SUCCESS:
            PlayAnim(AMARVI01_ANIM_TALK2, 1, 1);
            break;
    }
}
ScnObject *Marvin_Create(void *record)
{
    Marvin *object = new Marvin;
    object = (Marvin *)object->Init(record, 0); /* cast kept: Init returns the object it was called on */
    return object;
}
