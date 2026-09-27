/*
 * T247 - original object Scenaric.cpp (guessed name), one translation unit: the scenaric core.
 * .text 0x50d540-0x511f4e, .rdata 0x5770cc-0x577114 (the vtables of ScnGenericLogic and ScnGenericBody),
 * .data 0x57b838-0x57b934 (g_levelExitFlags, s_zoneIds, four strings), .bss 0x6cff98-0x6d0c98.
 * Contents, interleaved in address order as the original has them: the object core (0x50d540-0x50e2dd,
 * 0x50f3d9-0x50f45b) and its data, the position and collision code (0x50e1c5-0x50f7c0 and ScnMobile::UpdateShadow
 * 0x51186d), the rest of the core (0x50f80e-0x511dc1), the four grid queries (0x510987-0x510f34) and
 * ScnObject::Scenaric_SendToClass 0x511529 (the other object lookups are T250).
 * .bss names: VC6 orders .bss by a 1024-bucket hash of the names, later definition first inside a bucket. The
 * descriptive names (g_scenaricHeap, g_scenaricRegistry, g_levelHeapBlock, g_emptyStrings) would give the reverse of the
 * original's order, so the four are g_scenaricLevelHeap, g_scenaricClassRegistry, g_levelHeapStorage and g_emptyNames
 * here, and the other objects that refer to them use these names.
 */
/* BYTES: bss-name, flow, inline, slot-group, slot-name, view. */
/* BYTES(bss-name): named for its .bss hash bucket 556 */
/* BYTES(bss-name): named for its .bss hash bucket 691 */
/* BYTES(bss-name): named for its .bss hash bucket 969 */
/* BYTES(bss-name): named for its .bss hash bucket 1003 */
/* BYTES(inline): ScnAnimSound::IsSet (member-macro inline): source-only inline: the test materialised with setne */
/* BYTES(inline): IsAnimSoundAudible / SetUpdateMode (member-macro inline): source-only inline: expansion temporaries */
/* BYTES(inline): ScnObject::IsVisible (member-macro inline): macro comment "inline: SCN_OF_HIDDEN \ */
/* BYTES(inline): ScnObject::IsInWorld (member-macro inline): source-only inline: registered in the object grid */
/* BYTES(inline): ScnObject::NeverCulled (member-macro inline): source-only inline: expansion temporaries */
/* BYTES(inline): ScnObject::IsFarCulled (member-macro inline): source-only inline: the && materialised in a temp */
/* BYTES(inline): ScnObject::GetClassId (member-macro inline): source-only inline: a u16 temp per read */
/* BYTES(inline): ScnObject::GetFlags (member-macro inline): source-only inline: a u16 temp per read */
/* BYTES(inline): ScnObject::InstFlags (member-macro inline): source-only inline: the mask in a register */
/* BYTES(inline): ScnObject::AddInstFlags / RemoveInstFlags (member-macro inline): source-only inline: expansion temporaries */
/* BYTES(inline): ScnObject::SetInstFlag (member-macro inline): source-only inline: the flag word through a pointer local, the mask in a register */
/* BYTES(inline): Progress::CurrentLevel (member-macro inline): source-only inline: a 1-byte temp */
/* BYTES(inline): Progress::RestartScene (member-macro inline): source-only inline: its 'this' in a temp */
/* BYTES(slot-group): struct GridHits (Scenaric_FindBestInRadius, Scenaric_FindNearestOfClass, Scenaric_BroadcastInRadius): query box and result list are one 8-aligned local, as the original frame has them (the 4 unused bytes above are its alignment) */
/* BYTES(view): struct MatrixReturnStorage (RenderFacingCamera): constructor-less temporary for Mat44_Mul's by-value result (ebp-0x9c), as the original */

#define SDW_MEMBERS_AltModel          \
    s32 IsSet() const                 \
    {                                 \
        return modelResIdx != 0xffff; \
    } /* inline: the test materialised with setne */
#define SDW_MEMBERS_ScnObject                                                                                      \
    static void *operator new(u32 size); /* 0x50d5f4 Scenaric_Alloc */                                             \
    s32 IsAnimSoundAudible()                                                                                       \
    {                                                                                                              \
        return !(flags & SCN_OF_MUTE_ANIM_SOUND);                                                                  \
    }                                                                                                              \
    void SetUpdateMode(s32 mode);                                                                                  \
    ScnObject *Scenaric_FindBestInRadius(Vec3s *center, s16 minY, s16 maxY, u16 radius, u16 *distance,             \
                                         u32 (*score)(ScnObject *, ScnObject *, u32, ScnObject *),                 \
                                         s32 includeHidden); /* 0x50f80e */                                        \
    void AttachTo(ScnObject *parent, u8 joint, Vec3s *offset, Vec3s *rotation, u32 rootRotation, u32 worldOffset); \
    /* inline: registered in the object grid */                                                                    \
    s32 NeverCulled()                                                                                              \
    {                                                                                                              \
        return (flags & SCN_OF_NO_DIST_CULL) != 0;                                                                 \
    } /* inline */                                                                                                 \
    s32 IsFarCulled()                                                                                              \
    {                                                                                                              \
        return !NeverCulled() && camDist2 > 9000000;                                                               \
    }                                                                                                              \
    /* inline: the mask in a register */                                                                           \
    void AddInstFlags(u16 mask)                                                                                    \
    {                                                                                                              \
        u16 *f = &inst_flags;                                                                                      \
        *f |= mask;                                                                                                \
    }                                                                                                              \
    void RemoveInstFlags(u16 mask)                                                                                 \
    {                                                                                                              \
        u16 *f = &inst_flags;                                                                                      \
        *f &= (u16)~mask;                                                                                          \
    }                                                                                                              \
    /* defined below */
#define SDW_MEMBERS_ZoneList void Load(u16 id);
#define SDW_MEMBERS_Progress       \
    /* inline: a 1-byte temp */    \
    void RestartScene()            \
    {                              \
        GotoScene(CurrentLevel()); \
    } /* inline: its `this` in a temp */
#define SDW_MEMBERS_Mat44 Mat44(); /* 0x4077f0 Mat44_Ctor */
#include "sdw_enums.h"
#include "scenaric_props.h"
#include "object_lookup.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "../objects/instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32 1
#include "scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32
#define SDW_INLINE_SCNOBJECT_ISVISIBLE 1
#include "scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_ISVISIBLE
#define SDW_INLINE_SCNOBJECT_ISINWORLD 1
#include "scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_ISINWORLD
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_GETFLAGS 1
#define SDW_INLINE_SCNOBJECT_INSTFLAGS_U16 1
#define SDW_INLINE_SCNOBJECT_SETINSTFLAG_U16_S32 1
#include "scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_GETFLAGS
#undef SDW_INLINE_SCNOBJECT_INSTFLAGS_U16
#undef SDW_INLINE_SCNOBJECT_SETINSTFLAG_U16_S32
#define SDW_INLINE_PROGRESS_CURRENTLEVEL 1
#include "progress_inlines.h"
#undef SDW_INLINE_PROGRESS_CURRENTLEVEL
#include "grid_queries.h"

/* ---- declarations used by the object core ---- */
/* The classless fallback objects Scenaric_CreateObject makes when a class has no factory. The 0x14 bytes after the base
 * are where Install_CinematicResource keeps its own copy of the WAR record. */
struct ScnRecordCopy {
    u16 words[10];
};
class ScnGenericLogic : public ScnLogic { /* vtable 0x5770cc, 0x54 bytes */
public:
    virtual void SetPosition(Vec3s *pos); /* +0x1c 0x50d7eb */
    ScnRecordCopy recordCopy;             /* +0x40 */
};
class ScnGenericBody : public ScnBody { /* vtable 0x5770f0, 0x78 bytes */
public:
    virtual void SetPosition(Vec3s *pos); /* +0x1c 0x50d80d */
    ScnRecordCopy recordCopy;             /* +0x64 */
};

#include "../sdk/crt.h"
extern "C" s16 Math_RadiansToAngle4096(float radians);                    /* 0x5269ce */
void __cdecl Debug_Printf(const char *fmt, ...);                          /* 0x5363a5 */
u16 Sound_Play(u16 soundId, void *owner, u16 volume, u8 flags, s32 rate); /* 0x5491b8 */
u32 *Res_GetValidatedIdList(u16 resId, u16 *outCount);                    /* 0x548381 */
#include "list.h"
#include "../objects/sheep.h"
#include "id_list.h"
#include "../objects/instance.h"
#include "../objects/bounds.h"
#include "shadow.h"
#include "scn_tools.h"
#include "../objects/animation.h"
#include "input.h"
#include "../game/wolf_api.h"
#include "maths.h"
#include "game_state.h"
#include "fixed_math.h"
#include "load_warmeshes.h"
#include "../objects/world_draw.h"
#include "mat44.h"
#include "transition.h"
#include "progress.h"
#include "draw2d.h"
#include "screen.h"
#include "obj_grid.h"
#include "../app/app_main.h"
class Instance;
struct Animator;
void Anim_FireSoundEvent(Animator *anim, ScnObject *owner);  /* 0x50d540 */
ScnObject *Scenaric_CreateObject(u16 classId, void *record); /* 0x50d9af */
ScnObject *ScnGenericLogic_Create(void *record);             /* 0x50d82f */
ScnObject *ScnGenericBody_Create(void *record);              /* 0x50d94b */
void Zones_ResolveGlobalLists();                             /* 0x50d792 */

extern Wolf *g_pWolf; /* 0x6cf310 */

/* ---- declarations used by the rest of the core ---- */
extern "C" s16 Math_RadiansToAngle4096(float radians);                                       /* 0x5269ce */
extern "C" const s16 *g_pCosTable;                                                           /* 0x5814e4 */
s32 ObjGrid_QueryBoxPoints(const CollBox *query, ScnObject **out);                           /* 0x510af8 */
u16 Str_Length(const char *s);                                                               /* 0x5614fb */
void Render_CalcPivotOffset(Vec3s *rot, Vec3s *pivot, Vec3s *outOffset, Vec3s *scaleOrNull); /* 0x511af8 */

extern u32 g_levelExitFlags; /* 0x57b838 */
extern u32 g_gameFlags;      /* 0x6ddf74 */

#define g_mirrorRenderFlag (g_screen.mirrorRenderFlag) /* 0x6d6fe4 */

#define g_mirrorRenderOn (g_screen.mirrorRenderOn) /* 0x6d6fe8 */

#define g_mirrorPlaneVert2x (g_screen.mirrorPlaneVert2x) /* 0x6d6fea */

/* ---- declarations used by the position and collision code ---- */
extern "C" s32 Collide_SweepBox(ScnObject *mover, CollBox *box, Vec3s *disp, s32 *outFrac, s32 *outY,
                                CollContact *contacts, ScnObject *exclude, u8 queryFlags, s32 *outStaticY,
                                ScnObject **excludeList, s32 excludeCount); /* 0x519f6d  returns the contact count */
extern "C" s32 Coll_BoxGroundQuery(CollBox *box, s32 *outY, ScnObject *self, u8 mode,
                                   ScnObject **outHitObj);                 /* 0x51a9de */
extern "C" s16 Collide_GroundYRay(Vec3s *pos, Vec3s *outNormal, s16 minY); /* 0x51b5f3  static triangles only */
extern u8 g_sharedScratch[]; /* 0x6d5468  shared scratch; the resolver keeps a ResolveScratch there */

/* ---- declarations used by the grid queries ---- */

/* ---- declarations used by Scenaric_SendToClass ---- */

/* ---- this object's .data (0x57b838 on) ---- */
/* 0x57b838: this file's .data starts here (the linker places it 8-aligned; the string offsets below only come out like
 * the original's with this dword first). Starts at -1, zeroed by Scenaric_InitLevelState at every level start. */
u32 g_levelExitFlags = 0xffffffff;

/* 0x57b83c: the zone export ids resolved into g_waterZones[0..6], one per ZoneType (ZONE_WATER first). Found in the
 * exe by content. */
static u16 s_zoneIds[7] = {WAR_IDO_WATERBOX,  WAR_IDO_DEATHBOX,   WAR_IDO_ICEBOX,   WAR_IDO_SLIDEBOX,
                           WAR_IDO_PRINTSBOX, WAR_IDO_GRAVITYBOX, WAR_IDO_SHADOWBOX};

/* ---- this object's .bss, 0x6cff98-0x6d0c98, in hash-bucket order ((h ^ h>>16) & 1023, h = (h<<2)+(h>>4)+c;
 * items of 64 bytes or more are 8-aligned). The descriptive names are in the comments. ---- */
Heap g_scenaricLevelHeap;                      /* 0x6cff98  bucket 556  (g_scenaricHeap) the level's object heap */
ScnClassRegEntry g_scenaricClassRegistry[200]; /* 0x6cffd8  bucket 691  (g_scenaricRegistry) 16 bytes per class id */
ZoneList g_waterZones[7];                      /* 0x6d0c58  bucket 776  seven lists, stride 8 */
void *g_levelHeapStorage;                      /* 0x6d0c90  bucket 969  (g_levelHeapBlock) */
char g_emptyNames[3]; /* 0x6d0c94  bucket 1003 (g_emptyStrings) three NULs, one per failure of Text_GetClassString */

/* ---- inline helpers (no bodies of their own in the exe) ---- */
/* BYTES(inline): source-only inline: its expansion gives the original's shape */
#define SDW_INLINE_ZONELIST_LOAD_U16 1
#include "zone_list_inlines.h"
#undef SDW_INLINE_ZONELIST_LOAD_U16

/* A WAR resource's model type: the table entry's type byte without bit 0x40. */
/* BYTES(inline): source-only inline: its expansion gives the original's shape */
inline u32 Res_ModelType(u16 resIdx)
{
    u32 entry = g_pDav->war.table[resIdx];
    return ((entry >> 24) & 0xff) & WAR_RES_TYPE_MASK;
}

/* BYTES(inline): source-only inline: its expansion gives the original's shape */
#define SDW_INLINE_SCNOBJECT_FLAGSCLEAR_U32 1
#include "scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_FLAGSCLEAR_U32

/* BYTES(inline): source-only inline: its expansion gives the original's shape */
static inline u32 GridOverlapFour(s32 a, s32 b, s32 c, s32 d)
{
    return ~(a | b | c | d) & 0x80000000U;
}

static inline u32 GridOverlapTwo(s32 a, s32 b)
{
    return ~(a | b) & 0x80000000U;
}

/* BYTES(inline): source-only inline: its expansion gives the original's shape */
static inline s32 GridContainsPoint(const CollBox *query, const Vec3s *point)
{
    return point->x >= query->min.x && point->x <= query->max.x && point->y >= query->min.y &&
           point->y <= query->max.y && point->z >= query->min.z && point->z <= query->max.z;
}

/* The grid query box and its result list: one 8-aligned local (the 4 unused bytes above it in 0x50f80e and 0x50fc62
 * are that alignment). The other locals of the three searches are named for the stack slots they give (see the file
 * header): the suffixes are not meaningful. */
struct GridHits {
    CollBox query;
    ScnObject *list[64];
};

/* A constructor-free 64-byte temporary for Mat44_Mul's result: the product lands in an unnamed temp (after the named
 * locals, ebp-0x9c) and no Mat44 constructor is called for it (as in src/objects/animation.cpp). */
struct MatrixReturnStorage {
    float m[4][4];
    MatrixReturnStorage() {}
};

/* 0x50d540  Plays the key-frame sound event pending in the animator, then clears it. With no owner it is played flat;
 * an owner flagged 2 (muted animation sounds) plays nothing; id 0x158 is a rumble (empty on PC) instead of a sound. */
void Anim_FireSoundEvent(Animator *anim, ScnObject *owner)
{
    if (owner) {
        if (owner->IsAnimSoundAudible()) {
            if (anim->pendingSoundId == SND_RUMBLE) {
                g_pad.Rumble_stub(500, g_rumbleSeqFallingRock, 0x1000);
            } else {
                s32 pitch = (anim->speed << 12) / 0x1000;
                u16 soundId = (u16)anim->pendingSoundId;
                Sound_Play(soundId, owner, 0xff, SNDF_POSITIONAL | SNDF_NO_RETRIGGER, pitch);
            }
        }
    } else {
        Sound_Play((u16)anim->pendingSoundId, 0, 0xff, SNDF_NO_RETRIGGER, 0x1000);
    }
    anim->pendingSoundId = 0;
}

/* 0x50d5f4  ScnObject's class-specific operator new: every scenaric object lives in the per-level heap. */
void *ScnObject::operator new(u32 size)
{
    void *p = g_scenaricLevelHeap.Alloc(size);
    return p;
}

/* 0x50d610  The counterpart of operator new; nothing calls it (scenaric objects die with the level heap). */
void Scenaric_Free(void *obj)
{
    Debug_Printf("Delete called for a scenaric object\n");
    g_scenaricLevelHeap.Free(obj);
}

/* 0x50d630  Registers a class factory, its flags and its two inventory icons (the first entry of each id list). */
void Scenaric_RegisterClass_2(u16 classId, ScnObject *(*factory)(void *), u32 classFlags, u16 iconIdA, u16 iconIdB)
{
    u16 count;
    void **list;

    g_scenaricClassRegistry[classId].factory = factory;
    g_scenaricClassRegistry[classId].classFlags = classFlags;
    /* cast kept: an id list holds resource pointers of any kind; these are the icon images */
    list = iconIdA ? (void **)Res_GetValidatedIdList(iconIdA, &count) : 0;
    if (!count)
        list = 0;
    g_scenaricClassRegistry[classId].iconA = list ? *list : 0;
    /* cast kept: an id list holds resource pointers of any kind; these are the icon images */
    list = iconIdB ? (void **)Res_GetValidatedIdList(iconIdB, &count) : 0;
    if (!count)
        list = 0;
    g_scenaricClassRegistry[classId].iconB = list ? *list : 0;
}

/* 0x50d71c  The per-scene 512 KB block for the scenaric heap, and the list-node heap's 32 KB block. */
void Scenaric_AllocLevelHeap()
{
    g_levelHeapStorage = malloc(0x80000);
    Scratch32k_Alloc();
}

/* 0x50d738 */
void Scenaric_InitLevelState()
{
    memset(g_levelHeapStorage, 0, 0x80000);
    /* cast kept: g_levelHeapStorage is the malloc'd block (void *) that the heap takes as bytes */
    g_scenaricLevelHeap.Init((u8 *)g_levelHeapStorage, 0x80000);
    Scratch32k_Install();
    g_levelExitFlags = 0;
    Zones_ResolveGlobalLists();
    g_pWolf = 0;
    g_wolfInstanceCount = 0;
    Flock_ResetGlobals();
}

/* 0x50d792 */
void Zones_ResolveGlobalLists()
{
    s32 i;
    for (i = 0; i < 7; i++)
        g_waterZones[i].Load(s_zoneIds[i]);
}

/* 0x50d7eb  Stores the position only: the fallback objects are never in the object grid. */
void ScnGenericLogic::SetPosition(Vec3s *newPos)
{
    pos = *newPos;
}

/* 0x50d80d */
void ScnGenericBody::SetPosition(Vec3s *newPos)
{
    pos = *newPos;
}

/* 0x50d82f  Fallback for a rigid model (type 3): never updated. */
ScnObject *ScnGenericLogic_Create(void *record)
{
    ScnLogic *obj = new ScnGenericLogic;
    obj = (ScnLogic *)obj->Init(record); /* cast kept: Init returns this as a ScnObject * */
    obj->SetUpdateMode(SCN_UPD_NEVER);
    return obj;
}

/* 0x50d94b  Fallback for an animated model (type 4). */
ScnObject *ScnGenericBody_Create(void *record)
{
    ScnBody *obj = new ScnGenericBody;
    obj = obj->Init(record, 0);
    return obj;
}

/* 0x50d9af  Creates an object through its class factory; a class without one (or classless, 0xffff) falls back on the
 * generic object for its model type. Any other model type returns an uninitialised pointer. */
ScnObject *Scenaric_CreateObject(u16 classId, void *record)
{
    ScnClassRegEntry *entry;
    ScnObject *obj;

    if (classId != CLASSID_NONE)
        entry = &g_scenaricClassRegistry[classId];
    else
        entry = 0;
    if (entry && entry->factory) {
        obj = entry->factory(record);
    } else {
        if (classId != CLASSID_NONE)
            Debug_Printf("Class %d : According creation function not found\n", classId);
        switch (Res_ModelType(*(u16 *)record)) { /* cast kept: the WAR record is raw u16 words */
            case WAR_RES_MESH:
                obj = ScnGenericLogic_Create(record);
                break;
            case WAR_RES_MODEL:
                obj = ScnGenericBody_Create(record);
                break;
        }
    }
    return obj;
}

/* 0x50da72  The rotation that makes the object face the camera: yaw from the horizontal offset (plus baseRot's), and
 * unless yawOnly, pitch from the vertical offset over the horizontal distance. */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py) */
void ScnObject::CalcFacingCameraRot(Vec3s *outRot, Camera *view, s32 yawOnly, const Vec3s *baseRot)
{
    /* names chosen for their /Od stack slots (tools/vc6_locals.py): the x, y and z offsets and their squares */
    s32 xx, dy2, sqZ, dx, y, dz;

    if (!baseRot)
        baseRot = g_pZeroVec3s;
    dx = view->pos.x - pos.x;
    y = view->pos.y - pos.y;
    dz = view->pos.z - pos.z;
    xx = dx * dx;
    dy2 = y * y;
    sqZ = dz * dz;
    outRot->y = (Math_RadiansToAngle4096((float)atan2(dx, dz)) + baseRot->y + 0x800) & 0xfff;
    outRot->z = baseRot->z;
    outRot->x = baseRot->x;
    if (!yawOnly)
        outRot->x = (outRot->x + Math_RadiansToAngle4096((float)atan2(y, (s32)sqrt((double)xx + sqZ)))) & 0xfff;
}

/* 0x50dbb4  vtable +0x20: header fields, the rigid instance (mode 0xfe) and the model's bounds. */
ScnObject *ScnLogic::Init(void *record)
{
    flags = SCN_OF_INITIALISED;
    weight = 0;
    classId = ((u16 *)record)[5]; /* cast kept: the WAR record is raw u16 words */
    camDist2 = 0;
    /* cast kept: the WAR record is raw u16 words */
    Instance_InitFromWarRecord(Inst(), (u16 *)record, INST_MODE_SCN_LOGIC, 0);
    Bounds_FromRigidModel(&boundCenter, &boundRadius, inst_model);
    return this;
}

/* 0x50dc25  vtable +0x20: ScnLogic::Init, then a blob shadow 1.5 times the first model box's max.x wide (10 without). */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py) */
ScnObject *ScnLogicShadowed::Init(void *record)
{
    u8 size; /* the shadow radius; the three names pin the /Od stack slots (tools/vc6_locals.py) */
    ModelBoxList *list;
    CollBox *first;

    ScnLogic::Init(record);
    size = 10;
    list = inst_model->boxes;
    if (list)
        first = list->boxes;
    else
        first = 0;
    if (first)
        size = first->max.x * 3 / 2;
    Shadow_Init(&shadow, size);
    return this;
}

/* 0x50dc96  vtable +0x20: header fields, the animated instance (mode 0xff) and an animator with pose buffers for
 * nbJoints joints (the model's count when 0) from the level heap. */
ScnBody *ScnBody::Init(void *record, u32 nbJoints)
{
    flags = SCN_OF_INITIALISED;
    weight = 0;
    classId = ((u16 *)record)[5]; /* cast kept: the WAR record is raw u16 words */
    camDist2 = 0;
    /* cast kept: the WAR record is raw u16 words */
    Instance_InitFromWarRecord(Inst(), (u16 *)record, INST_MODE_SCN_BODY, 0);
    if (!nbJoints)
        nbJoints = inst_model->nbJoints;
    Animator_Init(Inst(), &anim, g_scenaricLevelHeap.Alloc(nbJoints * 3 * 0x28), 0);
    return this;
}

/* 0x50dd2a  vtable +0x20: ScnBody::Init, then the blob shadow as in ScnLogicShadowed::Init. */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py) */
ScnBody *ScnMobile::Init(void *record, u32 nbJoints)
{
    u8 size; /* the shadow radius; the three names pin the /Od stack slots (tools/vc6_locals.py) */
    ModelBoxList *list;
    CollBox *first;

    ScnBody::Init(record, nbJoints);
    size = 10;
    list = inst_model->boxes;
    if (list)
        first = list->boxes;
    else
        first = 0;
    if (first)
        size = first->max.x * 3 / 2;
    Shadow_Init(&shadow, size);
    return this;
}

/* 0x50dd9f  Resolves count alternate models (each the first entry of an id list), initialises the object through its
 * Init slot with pose buffers big enough for the largest skeleton (with the first alternate's model when the record
 * has none), snapshots the main model into outMain and gives every alternate the same buffer block. */
/* BYTES(slot-name): names and declaration order chosen for their stack slots (tools/vc6_locals.py) */
ScnObject *ScnObject::InitWithAltModels(void *record, AltModel *outMain, s32 count, const u16 *ids, AltModel *outAlts)
{
    /* names and declaration order chosen for the /Od stack slots (tools/vc6_locals.py) */
    s32 iAlt;
    u32 *found;
    u16 nIds;
    u16 resIdx; /* the model the object is initialised with: the record's, else the first alternate's */
    s32 maxJoints;
    u16 saved;
    AltModel *entry;
    Model *model;

    if (((u16 *)record)[0] != 0xffff) { /* cast kept: the WAR record is raw u16 words */
        /* cast kept: a resource is untyped; the record's word 0 names a model */
        model = (Model *)Dav_GetResourcePtr(((u16 *)record)[0]);
        maxJoints = model->nbJoints;
        resIdx = ((u16 *)record)[0]; /* cast kept: the WAR record is raw u16 words */
    } else {
        maxJoints = 0;
        resIdx = 0xffff;
    }
    outMain->modelResIdx = 0xffff;
    outMain->res2Idx = ((u16 *)record)[1]; /* cast kept: the WAR record is raw u16 words */
    outMain->animBuf = 0;
    for (iAlt = 0, entry = outAlts; iAlt < count; iAlt++, entry++) {
        found = Scn_FindIdList(ids[iAlt], &nIds);
        if (nIds > 0) {
            /* cast kept: an id list holds resource pointers of any kind; these lists hold models */
            model = (Model *)*found;
            entry->modelResIdx = Dav_FindResourceIndex(model);
            if (model)
                Bounds_FromAnimModel(&entry->boundCenter, &entry->boundRadius, model);
            if (model->nbJoints > maxJoints)
                maxJoints = model->nbJoints;
            if (resIdx == 0xffff)
                resIdx = entry->modelResIdx;
        } else {
            entry->modelResIdx = 0xffff;
        }
        entry->res2Idx = ((u16 *)record)[1]; /* cast kept: the WAR record is raw u16 words */
        entry->animBuf = 0;
    }
    if (((u16 *)record)[0] == 0xffff) { /* cast kept: the WAR record is raw u16 words */
        if (resIdx != 0xffff) {
            saved = ((u16 *)record)[0];  /* cast kept: the WAR record is raw u16 words */
            ((u16 *)record)[0] = resIdx; /* cast kept: the WAR record is raw u16 words */
            /* cast kept: only ScnBody classes initialise through InitWithAltModels */
            ((ScnBody *)this)->Init(record, maxJoints);
            ((u16 *)record)[0] = saved; /* cast kept: the WAR record is raw u16 words */
            /* cast kept: only ScnBody classes initialise through InitWithAltModels */
            ((ScnBody *)this)->AltModel_SaveBody(outMain);
        }
    } else {
        /* cast kept: only ScnBody classes initialise through InitWithAltModels */
        ((ScnBody *)this)->Init(record, maxJoints);
        /* cast kept: only ScnBody classes initialise through InitWithAltModels */
        ((ScnBody *)this)->AltModel_SaveBody(outMain);
    }
    for (iAlt = 0, entry = outAlts; iAlt < count; iAlt++, entry++)
        entry->animBuf = outMain->animBuf;
    return this;
}

/* 0x50df9d  A type-5 WAR resource (a scenaric record) becomes an object of the record's class. */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py) */
ScnObject *Install_ScenaricResource(u32 resIdx)
{
    u8 *base; /* names pin the /Od stack slots (tools/vc6_locals.py) */
    u16 *record = 0;

    if (resIdx >= g_pDav->war.header->resourceCount)
        return 0;
    base = g_pDav->war.blob;
    if (((g_pDav->war.table[resIdx] >> 24) & 0xff) != WAR_RES_SCENARIC)
        return 0;
    /* cast kept: a WAR resource is raw bytes at its table offset */
    record = (u16 *)(base + (g_pDav->war.table[resIdx] & 0xffffff));
    switch (Res_ModelType(record[0])) {
        case WAR_RES_MESH:
        case WAR_RES_MODEL:
            return Scenaric_CreateObject(record[5], record);
    }
    Debug_Printf("Error in Install_ScenaricResource: Unknown Scenaric Object Type!\n");
    return 0;
}

/* 0x50e069  A type-10 WAR resource (a cinematic actor) becomes a classless generic object that keeps its own copy of
 * the record. */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py) */
ScnObject *Install_CinematicResource(u32 resIdx)
{
    u8 *base; /* names pin the /Od stack slots (tools/vc6_locals.py) */
    ScnObject *obj;
    u16 *record;

    if (resIdx >= g_pDav->war.header->resourceCount)
        return 0;
    base = g_pDav->war.blob;
    if (((g_pDav->war.table[resIdx] >> 24) & 0xff) != WAR_RES_CINEMATIC)
        return 0;
    /* cast kept: a WAR resource is raw bytes at its table offset */
    record = (u16 *)(base + (g_pDav->war.table[resIdx] & 0xffffff));
    switch (Res_ModelType(record[0])) {
        case WAR_RES_MESH:
            obj = Scenaric_CreateObject(CLASSID_NONE, record);
            /* cast kept: a classless rigid object is a ScnGenericLogic; the copy takes the record's first 0x14 bytes as one block */
            ((ScnGenericLogic *)obj)->recordCopy = *(ScnRecordCopy *)obj->record;
            /* cast kept: a classless rigid object is a ScnGenericLogic */
            obj->record = ((ScnGenericLogic *)obj)->recordCopy.words;
            return obj;
        case WAR_RES_MODEL:
            obj = Scenaric_CreateObject(CLASSID_NONE, record);
            /* cast kept: a classless animated object is a ScnGenericBody; the copy takes the record's first 0x14 bytes as one block */
            ((ScnGenericBody *)obj)->recordCopy = *(ScnRecordCopy *)obj->record;
            /* cast kept: a classless animated object is a ScnGenericBody */
            obj->record = ((ScnGenericBody *)obj)->recordCopy.words;
            return obj;
    }
    Debug_Printf("Error in Install_CinematicResource: Unknown Cinematic Object Type!\n");
    return 0;
}

/* 0x50e18f  Steps the animation and fires the key-frame sound it reached, if any. */
void ScnBody::AdvanceAnim()
{
    Anim_Advance(&anim);
    if (anim.pendingSoundId)
        Anim_FireSoundEvent(&anim, this);
}

/* 0x50e1c5  vtable +0x1c base: if the object is registered in the object grid (flag 0x100) and the move changes its cell,
 * move its node from the old cell's list to the new one's; then store the position. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
void ScnObject::SetPosition(Vec3s *newPos)
{
    struct {
        u16 unused20, flags;
        ListNode *node;
        s16 newX, oldX;
        ListNode *iter;
        s16 unused10, newZ;
        Vec3s *oldPos;
        s16 unused08, oldZ;
        ListNode **list;
    } work;
    work.oldPos = &pos;
    work.flags = flags;
    if (work.flags & SCN_OF_IN_WORLD) {
        ObjGrid_CellFromXZ_Unclamped(work.oldPos->x, work.oldPos->z, &work.oldX, &work.oldZ);
        ObjGrid_CellFromXZ_Unclamped(newPos->x, newPos->z, &work.newX, &work.newZ);
        if ((work.oldX - work.newX) | (work.oldZ - work.newZ)) {
            work.list = ObjGrid_GetCellList(work.oldX, work.oldZ);
            for (work.iter = *work.list; work.iter; work.iter = work.iter->next) {
                if (work.iter->data == this) {
                    List_Remove(work.list, work.iter);
                    work.node = work.iter;
                    break;
                }
            }
            work.list = ObjGrid_GetCellList(work.newX, work.newZ);
            /* No check that the node was found: an object flagged 0x100 is assumed to be in its old cell's list. */
            List_PushFront(work.list, work.node);
        }
    }
    pos = *newPos;
}

/* 0x50e2dd  vtable +0x1c: ScnObject::SetPosition, then mark the shadow for re-projection. */
void ScnLogicShadowed::SetPosition(Vec3s *newPos)
{
    Shadow *sh;
    ScnObject::SetPosition(newPos);
    sh = &shadow;
    sh->flags |= SHADOW_F_REPROJECT;
}

/* 0x50e311  vtable +0x1c: ScnObject::SetPosition, then mark the shadow for re-projection (Shadow +0x14 |= 2). */
void ScnMobile::SetPosition(Vec3s *newPos)
{
    Shadow *sh;
    ScnObject::SetPosition(newPos);
    sh = &shadow; /* the 0x18-byte Shadow at +0x64 (Shadow_Update's argument at 0x5118ce) */
    sh->flags |= SHADOW_F_REPROJECT;
}

/* 0x50e345  virtual SetPosition(pos + delta). */
void ScnObject::Translate(Vec3s *delta)
{
    Vec3s next;
    next.x = pos.x + delta->x;
    next.y = pos.y + delta->y;
    next.z = pos.z + delta->z;
    SetPosition(&next);
}

/* 0x50e39e  This object's index in g_scnActive, or -1. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
s32 ScnObject::Scenaric_FindActiveIndex()
{
    struct {
        ScnObject **end, **slot;
    } w;

    w.slot = g_scnActive;
    w.end = g_scnActive + g_scnActiveHigh;
    for (; w.slot < w.end; ++w.slot) {
        if (*w.slot == this)
            return w.slot - g_scnActive;
    }
    return -1;
}

/* 0x50e3f6  This object's index in g_scnObjects (the objects the level file created), or -1. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
s32 ScnObject::Scenaric_FindObjectIndex()
{
    struct {
        ScnObject **end, **slot;
    } w;

    w.end = g_scnObjects + g_scnObjectCount;
    for (w.slot = g_scnObjects; w.slot < w.end; ++w.slot) {
        if (this == *w.slot)
            return w.slot - g_scnObjects;
    }
    return -1;
}

/* 0x50e44e  Registers the object in the world: its cell's object-grid list, and a slot in g_scnActive. An object created
 * by the level file goes back into its own slot; anything else takes the first free DYNAMIC slot, the ten the loader
 * reserves past g_scnObjectCount. Nothing checks that the scan found one: with all ten taken, the write at the end goes
 * one past the reserve. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
void ScnObject::AddToWorld(Vec3s *posOrNull)
{
    struct {
        u16 unused1C, flags;
        ScnObject **end, **slot;
        s32 index;
        ListNode *node;
        s16 cellZ, cellX;
        ListNode **list;
    } w;

    if (posOrNull)
        pos = *posOrNull;
    else
        posOrNull = &pos;
    w.flags = flags;
    if (!(w.flags & SCN_OF_IN_WORLD)) {
        ObjGrid_CellFromXZ_Unclamped(posOrNull->x, posOrNull->z, &w.cellX, &w.cellZ);
        w.list = ObjGrid_GetCellList(w.cellX, w.cellZ);
        List_AllocateNode(&w.node);
        w.node->data = this;
        List_PushFront(w.list, w.node);
        flags |= SCN_OF_IN_WORLD;
        w.index = Scenaric_FindObjectIndex();
        if (w.index == -1) {
            w.slot = g_scnActive + g_scnObjectCount;
            w.end = g_scnActive + g_scnActiveCapacity;
            while (w.slot < w.end && *w.slot)
                ++w.slot;
            w.index = w.slot - g_scnActive;
        }
        g_scnActive[w.index] = this;
        if (g_scnActiveHigh <= w.index)
            g_scnActiveHigh = (u16)(w.index + 1);
    }
}

/* 0x50e595  The counterpart: unlinks the object from its grid cell, clears its g_scnActive slot and walks the high-water
 * mark back over the empty slots below it. The index is used unchecked, so a -1 from FindActiveIndex would write below
 * the array. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
void ScnObject::RemoveFromWorld()
{
    struct {
        u16 unused20, flags;
        ScnObject **slot;
        s32 index;
        s16 unused14, cellX;
        ListNode *node;
        Vec3s *position;
        s16 unused08, cellZ;
        ListNode **list;
    } w;

    w.position = &pos;
    w.flags = flags;
    if (w.flags & SCN_OF_IN_WORLD) {
        ObjGrid_CellFromXZ_Unclamped(w.position->x, w.position->z, &w.cellX, &w.cellZ);
        w.list = ObjGrid_GetCellList(w.cellX, w.cellZ);
        for (w.node = *w.list; w.node; w.node = w.node->next) {
            if (w.node->data == this) {
                List_Remove(w.list, w.node);
                List_FreeNode(w.node);
                break;
            }
        }
        w.index = Scenaric_FindActiveIndex();
        g_scnActive[w.index] = 0;
        if (g_scnActiveHigh - 1 == w.index) {
            for (w.slot = g_scnActive + g_scnActiveHigh - 1; !*w.slot; --w.slot)
                --g_scnActiveHigh;
        }
        flags &= (u16)~SCN_OF_IN_WORLD;
    }
}

/* 0x50e6b7  Move-and-slide. Sweeps the box (the first solid one unless given) from startPos (pos unless given) along
 * *ioDelta, up to 3 times (once when flags == 0). Contacts whose normal points up more steeply than floorNormalCutoff are
 * floors (class 1, or 4 when the box corner lies more than half a unit below the contact plane), the rest walls (2).
 * A floor within stepHeight of the box bottom is stepped onto; otherwise the displacement is projected off the planes of
 * the classes in `flags` (walls first, then floors; flags & 8 keeps the vertical part) and swept again. Writes the
 * displacement actually travelled back to *ioDelta, fills *outInfo, returns the contact classes met (bit 0 = grounded).
 * flags 0x20 / 0x40 / 0x60 add sweep query bits 0x10 / drop ground query 1 / add 0x20. */
/* BYTES(slot-group): locals grouped in w (EBP-0x1C8) and top (-0x1CC) only to pin the original frame; the unusedNNN members fill gaps */
u16 ScnObject::Collide_ResolveMove(Vec3s *ioDelta, ContactInfo *outInfo, u16 floorNormalCutoff, u16 flags,
                                   Vec3s *startPosOrNull, CollBox *boxOrNull, s16 stepHeight, ScnObject **excludeList,
                                   s32 excludeCount)
{
    /* w starts at EBP-0x1C8, top at -0x1CC, this at -0x1D0. */
    struct {
        s16 unused1C8;
        u16 wallClassId;
        u32 floorClassFlags;
        s16 unused1C0;
        u16 floorClassId;
        s32 projection;
        u8 unused1B8[3], contactClass;
        ResolveScratch *scratch;
        u8 unused1B0, classes;
        u16 group;
        s32 pass;
        CollContact *contact;
        s32 i, biasCount;
        Vec3s start;
        s16 unused196;
        s32 height;
        CollBox worldBox;
        CollContact contacts[16];
        s32 unused40, staticY;
        u8 unused38[3], queryA;
        u32 budget;
        Vec3s remaining;
        u8 unused2A[5], queryB;
        s32 wallCount;
        Vec3i wallSum;
        s32 fraction, count;
        Vec3s current;
        u8 unused06[4], result, queryGround;
    } w;
    struct {
        u32 wallClassFlags;
    } top;
    w.queryA = 0;
    w.queryGround = CQ_STATIC;
    w.queryB = 0;
    if (flags & RESOLVE_ASK_MOVER)
        w.queryA = CQ_ASK_MOVER;
    if (flags & RESOLVE_NO_STATIC)
        w.queryGround = 0;
    if (flags & (RESOLVE_ASK_MOVER | RESOLVE_NO_STATIC))
        w.queryB = CQ_IGNORE_SKIPBOX;
    if (!boxOrNull)
        boxOrNull = GetFirstSolidBox();
    if (!boxOrNull)
        return 0;
    if (startPosOrNull)
        w.start = *startPosOrNull;
    else
        w.start = pos;
    if (flags == 0)
        w.budget = 1;
    else
        w.budget = 3;
    w.remaining.x = ioDelta->x;
    w.remaining.y = ioDelta->y;
    w.remaining.z = ioDelta->z;
    w.current.x = w.start.x;
    w.current.y = w.start.y;
    w.current.z = w.start.z;
    w.worldBox.Box_Translate(boxOrNull, &w.current);
    w.result = 0;
    w.wallSum.x = 0;
    w.wallSum.y = 0;
    w.wallSum.z = 0;
    w.wallCount = 0;
    if (outInfo) {
        outInfo->minAuxY = w.worldBox.max.y;
        outInfo->minContactY = w.worldBox.max.y;
        outInfo->floorNormal.x = 0;
        outInfo->floorNormal.y = -4096;
        outInfo->floorNormal.z = 0;
        outInfo->wallNormalMean.x = 0;
        outInfo->wallNormalMean.y = 0;
        outInfo->wallNormalMean.z = 0;
        outInfo->floorObj = 0;
        outInfo->wallObj = 0;
        outInfo->movableObj = 0;
    }
    do {
        --w.budget;
        w.count =
            Collide_SweepBox(this, &w.worldBox, &w.remaining, &w.fraction, &w.height, w.contacts, this,
                             w.queryGround | CQ_OBJECTS | w.queryA | w.queryB, &w.staticY, excludeList, excludeCount);
        if (w.count == 0) {
            w.current.x += w.remaining.x;
            w.current.y += w.remaining.y;
            w.current.z += w.remaining.z;
            break;
        }
        if (outInfo) {
            if (w.height < outInfo->minContactY)
                outInfo->minContactY = (s16)w.height;
            if (w.staticY < outInfo->minAuxY)
                outInfo->minAuxY = (s16)w.staticY;
        }
        /* cast kept: g_sharedScratch is one byte buffer that each user lays out its own way */
        w.scratch = (ResolveScratch *)g_sharedScratch;
        if (w.fraction > 0) {
            /* advance to the first contact */
            w.scratch->step.x = (s16)(w.remaining.x * w.fraction / 4096);
            w.scratch->step.y = (s16)(w.remaining.y * w.fraction / 4096);
            w.scratch->step.z = (s16)(w.remaining.z * w.fraction / 4096);
            w.current.x += w.scratch->step.x;
            w.current.y += w.scratch->step.y;
            w.current.z += w.scratch->step.z;
            w.remaining.x -= w.scratch->step.x;
            w.remaining.y -= w.scratch->step.y;
            w.remaining.z -= w.scratch->step.z;
            w.worldBox.min.x += w.scratch->step.x;
            w.worldBox.min.y += w.scratch->step.y;
            w.worldBox.min.z += w.scratch->step.z;
            w.worldBox.max.x += w.scratch->step.x;
            w.worldBox.max.y += w.scratch->step.y;
            w.worldBox.max.z += w.scratch->step.z;
        }
        w.classes = 0;
        for (w.i = 0; w.i < w.count; ++w.i) {
            w.contact = &w.contacts[w.i];
            if (w.contact->normal.y < -floorNormalCutoff) {
                /* floor: the box corner nearest the plane, relative to the contact point, against the normal */
                if (w.contact->normal.x > 0)
                    w.scratch->step.x = w.worldBox.min.x;
                else
                    w.scratch->step.x = w.worldBox.max.x;
                if (w.contact->normal.y > 0)
                    w.scratch->step.y = w.worldBox.min.y;
                else
                    w.scratch->step.y = w.worldBox.max.y;
                if (w.contact->normal.z > 0)
                    w.scratch->step.z = w.worldBox.min.z;
                else
                    w.scratch->step.z = w.worldBox.max.z;
                w.scratch->step.x -= w.contact->point.x;
                w.scratch->step.y -= w.contact->point.y;
                w.scratch->step.z -= w.contact->point.z;
                if (w.scratch->step.x * w.contact->normal.x + w.scratch->step.y * w.contact->normal.y +
                        w.scratch->step.z * w.contact->normal.z >=
                    -2048) {
                    if (outInfo)
                        outInfo->floorNormal = w.contact->normal;
                    w.scratch->contactClass[w.i] = COLL_FLOOR;
                } else
                    w.scratch->contactClass[w.i] = COLL_FLOOR_EDGE;
                if (outInfo && w.contact->obj) {
                    outInfo->floorObj = w.contact->obj;
                    w.floorClassId = w.contact->obj->classId;
                    w.floorClassFlags = g_scenaricClassRegistry[w.floorClassId].classFlags;
                    if (w.floorClassFlags & SCN_CF_CARRIER)
                        outInfo->movableObj = w.contact->obj;
                }
            } else {
                w.scratch->contactClass[w.i] = COLL_WALL;
                if (outInfo) {
                    if (w.contact->obj) {
                        outInfo->wallObj = w.contact->obj;
                        w.wallClassId = w.contact->obj->classId;
                        top.wallClassFlags = g_scenaricClassRegistry[w.wallClassId].classFlags;
                        if (top.wallClassFlags & SCN_CF_CARRIER)
                            outInfo->movableObj = w.contact->obj;
                    }
                    w.wallSum.x += w.contact->normal.x;
                    w.wallSum.y += w.contact->normal.y;
                    w.wallSum.z += w.contact->normal.z;
                    ++w.wallCount;
                }
            }
            w.classes |= w.scratch->contactClass[w.i];
        }
        if ((w.classes & (COLL_FLOOR | COLL_FLOOR_EDGE)) && w.worldBox.max.y - w.height <= stepHeight) {
            /* a floor within the step height: stand on it */
            w.remaining.y = (s16)(w.height - w.worldBox.max.y);
            w.result |= COLL_FLOOR;
            if (outInfo && outInfo->floorNormal.y == -4096) {
                for (w.i = 0; w.i < w.count; ++w.i)
                    if (w.scratch->contactClass[w.i] & (COLL_FLOOR | COLL_FLOOR_EDGE))
                        outInfo->floorNormal = w.contacts[w.i].normal;
            }
        } else if (flags & (w.classes & ~COLL_FLOOR_EDGE)) {
            /* slide: project the rest of the displacement (x1024) off the walls, then off the floors */
            w.scratch->projected.x = w.remaining.x << 10;
            w.scratch->projected.y = w.remaining.y << 10;
            w.scratch->projected.z = w.remaining.z << 10;
            w.group = COLL_WALL;
            for (w.pass = 0; w.pass < 2; ++w.pass) {
                if (w.classes & w.group) {
                    w.scratch->normalSum.x = 0;
                    w.scratch->normalSum.y = 0;
                    w.scratch->normalSum.z = 0;
                    w.biasCount = 0;
                    for (w.i = 0; w.i < w.count; ++w.i) {
                        w.contactClass = w.scratch->contactClass[w.i];
                        if (flags & w.contactClass & w.group) {
                            w.contact = &w.contacts[w.i];
                            w.result |= w.contactClass;
                            w.projection = (w.contact->normal.x * w.scratch->projected.x +
                                            w.contact->normal.y * w.scratch->projected.y +
                                            w.contact->normal.z * w.scratch->projected.z) >>
                                           10;
                            w.scratch->projected.x -= (w.projection * w.contact->normal.x) >> 14;
                            w.scratch->projected.y -= (w.projection * w.contact->normal.y) >> 14;
                            w.scratch->projected.z -= (w.projection * w.contact->normal.z) >> 14;
                            w.scratch->normalSum.x = w.contact->normal.x + w.scratch->normalSum.x;
                            w.scratch->normalSum.y = w.contact->normal.y + w.scratch->normalSum.y;
                            w.scratch->normalSum.z = w.contact->normal.z + w.scratch->normalSum.z;
                            ++w.biasCount;
                        } else if (w.contactClass & w.group)
                            w.result |= w.contactClass;
                    }
                    if (w.biasCount > 0) {
                        /* push 3/8 of the mean normal back out, so the next sweep starts off the plane */
                        w.scratch->projected.x =
                            (w.scratch->normalSum.x * 3) / (w.biasCount << 3) + w.scratch->projected.x;
                        w.scratch->projected.y =
                            (w.scratch->normalSum.y * 3) / (w.biasCount << 3) + w.scratch->projected.y;
                        w.scratch->projected.z =
                            (w.scratch->normalSum.z * 3) / (w.biasCount << 3) + w.scratch->projected.z;
                    }
                }
                w.group = COLL_FLOOR;
            }
            w.remaining.x = (s16)(w.scratch->projected.x / 1024);
            if (!(flags & RESOLVE_KEEP_Y))
                w.remaining.y = (s16)(w.scratch->projected.y / 1024);
            w.remaining.z = (s16)(w.scratch->projected.z / 1024);
        } else {
            /* blocked */
            if (w.classes & COLL_FLOOR_EDGE) {
                if (w.worldBox.max.y <= w.height || (ioDelta->x | ioDelta->z) == 0) {
                    w.result |= (u8)((w.classes & (u8)~COLL_FLOOR_EDGE) | COLL_FLOOR);
                    if (outInfo && outInfo->floorNormal.y == -4096) {
                        for (w.i = 0; w.i < w.count; ++w.i)
                            if (w.scratch->contactClass[w.i] & COLL_FLOOR_EDGE)
                                outInfo->floorNormal = w.contacts[w.i].normal;
                    }
                } else
                    w.result |= (u8)((w.classes & (u8)~COLL_FLOOR_EDGE) | COLL_WALL);
            } else
                w.result |= w.classes;
            if (w.fraction <= 0 && (w.remaining.x | w.remaining.z) == 0)
                w.remaining.y = 0;
            else {
                w.remaining.x = 0;
                w.remaining.z = 0;
            }
        }
    } while ((w.remaining.x | w.remaining.y | w.remaining.z) != 0 && w.budget > 0);
    ioDelta->x = w.current.x - w.start.x;
    ioDelta->y = w.current.y - w.start.y;
    ioDelta->z = w.current.z - w.start.z;
    if (outInfo && w.wallCount > 0) {
        outInfo->wallNormalMean.x = (s16)(w.wallSum.x / w.wallCount);
        outInfo->wallNormalMean.y = (s16)(w.wallSum.y / w.wallCount);
        outInfo->wallNormalMean.z = (s16)(w.wallSum.z / w.wallCount);
    }
    return w.result;
}

/* 0x50f3d9  Marks every model box whose flags share a bit with mask as disabled (flag 1). The boxes are the model's,
 * so every object sharing the model is affected. */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py) */
void ScnObject::DisableBoxes(u32 mask)
{
    ModelBoxList *boxList; /* names pin the /Od stack slots (tools/vc6_locals.py) */
    u32 i;
    CollBox *boxes;

    boxList = inst_model->boxes;
    if (!boxList) {
        i = 0;
        boxes = 0;
    } else {
        i = boxList->count;
        boxes = boxList->boxes;
    }
    while (i--) {
        if (boxes[i].flags & mask)
            boxes[i].flags |= COLLBOX_NONSOLID;
    }
}

/* 0x50f45b  The reverse: clears flag 1 on every model box whose flags share a bit with mask. */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py) */
void ScnObject::EnableBoxes(u32 mask)
{
    ModelBoxList *boxList; /* names pin the /Od stack slots (tools/vc6_locals.py) */
    u32 i;
    CollBox *boxes;

    boxList = inst_model->boxes;
    if (!boxList) {
        i = 0;
        boxes = 0;
    } else {
        i = boxList->count;
        boxes = boxList->boxes;
    }
    while (i--) {
        if (boxes[i].flags & mask)
            boxes[i].flags &= ~COLLBOX_NONSOLID;
    }
}

/* 0x50f4dd  The first box of the model's box list whose flags bit 0 (non-solid) is clear, or NULL. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
CollBox *ScnObject::GetFirstSolidBox()
{
    struct {
        ModelBoxList *list;
        CollBox *box;
        u32 count;
    } work;
    work.list = inst_model->boxes;
    if (!work.list) {
        work.count = 0;
        work.box = 0;
    } else {
        work.count = work.list->count;
        work.box = work.list->boxes;
    }
    while (work.count > 0) {
        if (!(work.box->flags & COLLBOX_NONSOLID))
            return work.box;
        --work.count;
        ++work.box;
    }
    return 0;
}

/* 0x50f54a  Ground height under pos from the static triangles, and optionally the object tops (lower = higher up). */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; unused02 is a filler */
s16 ScnObject::World_GroundYRay(Vec3s *qpos, s32 includeObjects)
{
    struct {
        s32 height;
        s16 unused0C, objectHeight;
        Vec3s normal;
        s16 unused02;
    } w;
    w.height = Collide_GroundYRay(qpos, &w.normal, qpos->y);
    if (includeObjects) {
        w.objectHeight = ObjGrid_QueryGroundY(qpos, &w.normal, qpos->y, this);
        if (w.objectHeight < w.height)
            w.height = w.objectHeight;
    }
    return (s16)w.height;
}

/* 0x50f5b1  Ground height for the object's first solid box placed at qpos and stretched down to 32000: the height the
 * object's origin would have standing there, or 32000 when nothing is below. Bodyless objects use the vertical ray. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets (and top) */
s16 ScnObject::QueryGroundY(Vec3s *qpos, s32 includeObjects)
{
    struct {
        CollBox world;
        CollBox *box;
        u8 unused04[3], mode;
    } w;
    struct {
        s32 height;
    } top;
    w.box = GetFirstSolidBox();
    if (w.box) {
        w.world.Box_Translate(w.box, qpos);
        w.world.max.y = 32000;
        if (includeObjects)
            w.mode = CQ_STATIC | CQ_OBJECTS;
        else
            w.mode = CQ_STATIC;
        if (!Coll_BoxGroundQuery(&w.world, &top.height, this, w.mode | CQ_FROM_QUERY_GROUND_Y, 0))
            top.height = 32000;
        else
            top.height -= w.box->max.y;
    } else {
        top.height = World_GroundYRay(qpos, includeObjects);
    }
    return (s16)top.height;
}

/* 0x50f650  Like QueryGroundY (mode 0x85) with the box shrunk by 10 on each X/Y2 side, returning the raw surface height
 * (the box offset is not subtracted). Bodyless objects use the static-triangle ray only. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; unused1A is a filler */
s16 ScnObject::QueryGroundY_Shrunk(Vec3s *qpos)
{
    struct {
        Vec3s normal;
        s16 unused1A;
        s32 height;
        CollBox world;
        CollBox *box;
    } w;
    w.box = GetFirstSolidBox();
    if (w.box) {
        w.world.flags = w.box->flags;
        w.world.min.x = w.box->min.x + 10 + qpos->x;
        w.world.max.x = w.box->max.x - 10 + qpos->x;
        w.world.min.z = w.box->min.z + 10 + qpos->z;
        w.world.max.z = w.box->max.z - 10 + qpos->z;
        w.world.min.y = w.box->min.y + qpos->y;
        w.world.max.y = w.box->max.y + qpos->y;
        w.world.max.y = 32000;
        if (!Coll_BoxGroundQuery(&w.world, &w.height, this, CQ_STATIC | CQ_OBJECTS | CQ_FROM_QUERY_GROUND_Y, 0))
            w.height = 32000;
    } else {
        w.height = Collide_GroundYRay(qpos, &w.normal, qpos->y);
    }
    return (s16)w.height;
}

/* 0x50f74a  QueryGroundY (mode 0x85) that also returns the object stood on. No NULL-box check. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
s16 ScnObject::QueryGroundYAndObject(Vec3s *qpos, ScnObject **outHitObj)
{
    struct {
        s32 height;
        CollBox world;
        CollBox *box;
    } w;
    *outHitObj = 0;
    w.box = GetFirstSolidBox();
    w.world.Box_Translate(w.box, qpos);
    w.world.max.y = 32000;
    if (!Coll_BoxGroundQuery(&w.world, &w.height, this, CQ_STATIC | CQ_OBJECTS | CQ_FROM_QUERY_GROUND_Y, outHitObj))
        w.height = 32000;
    else
        w.height -= w.box->max.y;
    return (s16)w.height;
}

/* 0x50f7c0  Contact count of the first solid box placed at qpos with its real extents; 0 when the object has none. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
s32 ScnObject::TestBodyAt(Vec3s *qpos, u8 mode)
{
    struct {
        s32 height;
        CollBox world;
        CollBox *box;
    } w;
    w.box = GetFirstSolidBox();
    if (w.box) {
        w.world.Box_Translate(w.box, qpos);
        return Coll_BoxGroundQuery(&w.world, &w.height, this, mode, 0);
    } else {
        return 0;
    }
}

/* 0x50f80e - the visible object (or any, with includeHidden) whose XZ distance is within `radius` and that `score`
 * rates lowest; the score gets (this, candidate, squared distance, best so far). The distance of the winner goes to
 * *distance. The vertical window only bounds the grid query. */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py); the suffixes are not meaningful */
ScnObject *ScnObject::Scenaric_FindBestInRadius(Vec3s *center, s16 minY, s16 maxY, u16 radius, u16 *distance,
                                                u32 (*score)(ScnObject *, ScnObject *, u32, ScnObject *),
                                                s32 includeHidden)
{
    s32 dx7, dy, dzN;
    ScnObject *objN;
    u32 bestScoreX;
    s32 bestDist2;
    s32 i3;
    GridHits grid4;
    ScnObject *best2;
    u32 distX;
    u32 radiusSq3;
    u32 value2;
    s32 count;
    best2 = 0;
    radiusSq3 = radius * radius;
    bestScoreX = 0xffffffff;
    bestDist2 = radiusSq3;
    grid4.query.min.x = center->x - radius;
    grid4.query.min.y = minY;
    grid4.query.min.z = center->z - radius;
    grid4.query.max.x = center->x + radius;
    grid4.query.max.y = maxY;
    grid4.query.max.z = center->z + radius;
    count = ObjGrid_QueryBoxPoints(&grid4.query, grid4.list);
    for (i3 = 0; i3 < count; i3++) {
        objN = grid4.list[i3];
        if (objN != this) {
            if (objN->IsVisible() || includeHidden) {
                dx7 = objN->pos.x - center->x;
                dy = objN->pos.y - center->y;
                dzN = objN->pos.z - center->z;
                dx7 *= dx7;
                dy *= dy;
                dzN *= dzN;
                distX = dx7 + dzN;
                if (distX <= radiusSq3) {
                    value2 = score(this, objN, distX, best2);
                    if (value2 < bestScoreX) {
                        best2 = objN;
                        bestScoreX = value2;
                        bestDist2 = distX;
                    }
                }
            }
        }
    }
    if (distance)
        *distance = (u16)sqrt((double)bestDist2);
    return best2;
}

/* 0x50fa46 - the nearest (XZ) visible object of class `classId` within `radius`, not this one; its distance to *outDist. */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py); the suffixes are not meaningful */
ScnObject *ScnObject::Scenaric_FindNearestOfClass(Vec3s *center, s16 classId, s16 minY, s16 maxY, u16 radius,
                                                  s16 *outDist, s32 includeHidden)
{
    s32 dx7, dy, dzN;
    ScnObject *objN;
    s32 bestDist2;
    s32 i3;
    GridHits grid4;
    ScnObject *best2;
    u32 dist2;
    s32 count;
    best2 = 0;
    bestDist2 = radius * radius;
    grid4.query.min.x = center->x - radius;
    grid4.query.min.y = minY;
    grid4.query.min.z = center->z - radius;
    grid4.query.max.x = center->x + radius;
    grid4.query.max.y = maxY;
    grid4.query.max.z = center->z + radius;
    count = ObjGrid_QueryBoxPoints(&grid4.query, grid4.list);
    for (i3 = 0; i3 < count; i3++) {
        objN = grid4.list[i3];
        if (objN->GetClassId() == (u16)classId && objN != this) {
            if (objN->IsVisible() || includeHidden) {
                dx7 = objN->pos.x - center->x;
                dy = objN->pos.y - center->y;
                dzN = objN->pos.z - center->z;
                dx7 *= dx7;
                dy *= dy;
                dzN *= dzN;
                dist2 = dx7 + dzN;
                if (dist2 <= (u32)bestDist2) {
                    best2 = objN;
                    bestDist2 = dist2;
                }
            }
        }
    }
    if (outDist)
        *outDist = (s16)sqrt((double)bestDist2);
    return best2;
}

/* 0x50fc62 - sends msgId(arg) from this object to every visible object of class classFilter (0xffff = any) within
 * `radius` (XZ) of it. */
/* BYTES(slot-name): names chosen for their stack slots (tools/vc6_locals.py); the suffixes are not meaningful */
void ScnObject::Scenaric_BroadcastInRadius(u16 classFilter, s16 minY, s16 maxY, u16 radius, u32 msgId, void *arg,
                                           s32 includeHidden)
{
    s32 dx7, dy, dzN;
    ScnObject *obj;
    s32 i2;
    GridHits grid2;
    u32 distX;
    Vec3s center3;
    u32 radiusSq3;
    s32 count;
    center3 = pos;
    radiusSq3 = radius * radius;
    grid2.query.min.x = center3.x - radius;
    grid2.query.min.y = minY;
    grid2.query.min.z = center3.z - radius;
    grid2.query.max.x = center3.x + radius;
    grid2.query.max.y = maxY;
    grid2.query.max.z = center3.z + radius;
    count = ObjGrid_QueryBoxPoints(&grid2.query, grid2.list);
    for (i2 = 0; i2 < count; i2++) {
        obj = grid2.list[i2];
        if ((classFilter == CLASSID_NONE || obj->GetClassId() == classFilter) && obj != this) {
            if (obj->IsVisible() || includeHidden) {
                dx7 = obj->pos.x - center3.x;
                dy = obj->pos.y - center3.y;
                dzN = obj->pos.z - center3.z;
                dx7 *= dx7;
                dy *= dy;
                dzN *= dzN;
                distX = dx7 + dzN;
                if (distX <= radiusSq3)
                    obj->HandleMessage(this, msgId, arg);
            }
        }
    }
}

/* 0x50fe66 - string `index` of this object's class list in the level's string bank (list 1 + classId). */
char *ScnObject::Text_GetClassString(u8 index)
{
    char *cur;
    u8 count;
    StringBank *sb;
    sb = &g_pDav->strings;
    if (classId + 1 >= sb->listCount)
        return &g_emptyNames[0];
    if (sb->lists) {
        cur = sb->lists[classId + 1];
        count = *cur;
        cur++;
        if (index >= count)
            return &g_emptyNames[1];
        while (index--)
            cur += Str_Length(cur) + 1;
        return cur;
    }
    return &g_emptyNames[2];
}

/* 0x50ff14 - hangs this object on joint `joint` of `parent` (an AttachLink from the pool of 10); no-op when already
 * attached or the pool is full. A registered object is moved to the parent's position at once. */
void ScnObject::AttachTo(ScnObject *parent, u8 joint, Vec3s *offset, Vec3s *rotation, u32 rootRotation, u32 worldOffset)
{
    AttachLink *link;
    if (InstFlags(INST_F_ATTACHED))
        return;
    /* cast kept: AttachTo's last parameter is u32 in every object's declaration */
    link = AttachLink_Alloc(parent->Inst(), parent, joint, offset, rotation, rootRotation, (Vec3s *)worldOffset);
    if (!link)
        return;
    attachLink = link;
    AddInstFlags(INST_F_ATTACHED);
    parent->attachedChildCount++;
    if (GetFlags() & SCN_OF_IN_WORLD)
        SetPosition(&parent->pos);
}

/* 0x50ffca - releases the attachment link (and the parent's child count). */
void ScnObject::Detach()
{
    if (!InstFlags(INST_F_ATTACHED))
        return;
    attachLink->parentInst->attachedChildCount--;
    AttachLink_Free(attachLink);
    RemoveInstFlags(INST_F_ATTACHED);
    attachLink = 0;
}

/* 0x510041 - drops the object onto the ground under it (searched from 50 units above); a body without a solid box
 * sits 2 units higher. */
void ScnObject::SnapToGround(s32 includeObjects)
{
    Vec3s p;
    p.x = pos.x;
    p.y = pos.y - 0x32;
    p.z = pos.z;
    p.y = QueryGroundY(&p, includeObjects);
    if (!GetFirstSolidBox())
        p.y = p.y - 2;
    SetPosition(&p);
}

/* 0x5100ae - fills a minimal WAR scenaric record for the model of export list `resourceId` (model index, no secondary
 * resource, position, rotation, zero scale words), so the export can be instantiated without a designer placement.
 * Returns the model resource, or NULL when the list is empty. */
void *Scn_BuildRecordFromExport(u16 resourceId, u16 *out, u16 rotation, const Vec3s *at)
{
    u32 *list;
    u16 count;
    void *model;
    list = Scn_FindIdList(resourceId, &count);
    model = 0;
    if (count > 0) {
        model = (void *)list[0]; /* cast kept: an id list holds resource pointers as u32 words */
        out[0] = Dav_FindResourceIndex(model);
    } else {
        out[0] = 0xffff;
    }
    out[1] = 0xffff;
    if (at)
        *(Vec3s *)(out + 2) = *at; /* cast kept: the record is raw u16 words; words 2-4 are the position */
    out[5] = rotation;
    out[6] = 0;
    out[7] = 0;
    out[8] = 0;
    return model;
}

/* 0x51014c - an alternate rigid model from id list `idListId`: its resource index and bounds, no pose buffers. */
void ScnObject::AltModel_InitRigid(AltModel *out, u16 idListId)
{
    u32 *list;
    u16 count;
    void *model;
    list = Scn_FindIdList(idListId, &count);
    model = 0;
    if (count > 0) {
        model = (void *)list[0]; /* cast kept: an id list holds resource pointers as u32 words */
        out->modelResIdx = Dav_FindResourceIndex(model);
    } else {
        out->modelResIdx = 0xffff;
    }
    out->res2Idx = 0xffff;
    out->animBuf = 0;
    if (model)
        Bounds_FromRigidModel(&out->boundCenter, &out->boundRadius, model);
}

/* 0x5101d5 - an alternate animated model: reuses this body's pose buffers when they have room for its joints, else
 * allocates three poses per joint from the level heap (no heap: the model is dropped). */
void ScnBody::AltModel_InitBody(AltModel *out, u16 idListId)
{
    u32 *list;
    u16 count;
    Model *model;
    list = Scn_FindIdList(idListId, &count);
    model = 0;
    if (count > 0) {
        model = (Model *)list[0]; /* cast kept: an id list holds resource pointers as u32 words */
        out->modelResIdx = Dav_FindResourceIndex(model);
    } else {
        out->modelResIdx = 0xffff;
    }
    out->res2Idx = 0xffff;
    if (model) {
        if (anim.nbJoints >= model->nbJoints) {
            out->animBuf = anim.bufA;
            if (anim.bufB < out->animBuf)
                out->animBuf = anim.bufB;
            if (anim.bufC < out->animBuf)
                out->animBuf = anim.bufC;
        } else {
            out->animBuf = g_scenaricLevelHeap.Alloc(model->nbJoints * 3 * 0x28);
        }
        if (!out->animBuf)
            out->modelResIdx = 0xffff;
        else
            Bounds_FromAnimModel(&out->boundCenter, &out->boundRadius, model);
    }
}

/* 0x5102dd - snapshots the current rigid model into `out` (no callers in the exe). */
void ScnObject::AltModel_SaveRigid(AltModel *out)
{
    out->modelResIdx = record[0];
    out->res2Idx = record[1];
    out->animBuf = 0;
    out->boundCenter = boundCenter;
    out->boundRadius = boundRadius;
}

/* 0x51033a - the same for a body; its pose-buffer block starts at the lowest of the three buffers. */
void ScnBody::AltModel_SaveBody(AltModel *out)
{
    out->modelResIdx = record[0];
    out->res2Idx = record[1];
    out->boundCenter = boundCenter;
    out->boundRadius = boundRadius;
    out->animBuf = anim.bufA;
    if (anim.bufB < out->animBuf)
        out->animBuf = anim.bufB;
    if (anim.bufC < out->animBuf)
        out->animBuf = anim.bufC;
}

/* 0x5103cd - switches a rigid object to model `m` (its record words 0/1, the instance, the bounds). */
void ScnLogic::SwapModel(const AltModel *m)
{
    if (m->IsSet()) {
        record[0] = m->modelResIdx;
        record[1] = m->res2Idx;
        Instance_InitFromWarRecord(Inst(), record, INST_MODE_SCN_LOGIC, 1);
        boundCenter = m->boundCenter;
        boundRadius = m->boundRadius;
    }
}

/* 0x510452 - the same for a body, which also gets the model's pose buffers. */
void ScnBody::SwapModel(AltModel *m)
{
    if (m->IsSet()) {
        record[0] = m->modelResIdx;
        record[1] = m->res2Idx;
        Instance_InitFromWarRecord(Inst(), record, INST_MODE_SCN_BODY, 1);
        Animator_Init(Inst(), &anim, m->animBuf, 1);
        boundCenter = m->boundCenter;
        boundRadius = m->boundRadius;
    }
}

/* 0x5104fa - gives the model resource *resIdx this body's animation table when both have the same joint count. */
void ScnBody::ShareAnimTable(const u16 *resIdx)
{
    u32 entry;
    u8 *blob;
    Model *model;
    /* cast kept: the callers pass &alt->modelResIdx, the first field of an AltModel */
    if (((const AltModel *)resIdx)->IsSet()) {
        entry = g_pDav->war.table[*resIdx];
        blob = g_pDav->war.blob;
        model = (Model *)(blob + (entry & 0xffffff)); /* cast kept: a WAR resource is raw bytes at its table offset */
        if (model->nbJoints == inst_model->nbJoints)
            model->animTable = inst_model->animTable;
    }
}

/* 0x510574 - the respawn wobble lasts 0x1000 ms. */
s32 ScnObject::IsRespawnWobbleDone(s32 elapsed)
{
    return elapsed >= 0x1000;
}

/* 0x51058d - draws the object squashed and stretched by two damped cosines of the (squared) time since it respawned. */
void ScnObject::RenderRespawnWobble(Camera *view, s32 elapsed)
{
    s32 wave;
    s32 step;
    Vec3s stretch;
    if (elapsed < 0x1000) {
        step = elapsed >> 6;
        step = step * step;
        wave = (g_pCosTable[(s16)(((step << 12) * 0x16 / 7 >> 12) & 0xfff)] << 10) >> 12;
        stretch.z = 0x400 - wave * (0x1000 - elapsed) / 0x1000;
        stretch.x = stretch.z;
        wave = (g_pCosTable[(s16)(((step << 12) * 2 >> 12) & 0xfff)] << 10) >> 12;
        stretch.y = 0x400 - wave * (0x1000 - elapsed) / 0x1000;
        RenderScaled(view, &stretch);
    }
}

/* 0x510675 - runs the level exit that g_levelExitFlags asks for once its fade has finished (bit 0): 4 abort to the
 * menu, 0x20 restart the scene, 2 the attract demo, else the scene is finished with success. Bit 0x10 waits for the
 * transition and then raises g_gameFlags 0x10. */
void Level_ExitUpdate()
{
    if (g_levelExitFlags & LEVEL_EXIT_NEXT) {
        if (g_levelExitFlags & LEVEL_EXIT_TO_MENU)
            g_pProgress->Level_FinishScene(0);
        else if (g_levelExitFlags & LEVEL_EXIT_RESTART)
            g_pProgress->RestartScene();
        else if (g_levelExitFlags & LEVEL_EXIT_DEMO)
            g_pProgress->StartAttractDemo();
        else
            g_pProgress->Level_FinishScene(1);
    } else if (g_levelExitFlags & LEVEL_EXIT_TRANSITION) {
        if (!Transition_Update()) {
            g_gameFlags |= GF_TRANSITION_IDLE;
            g_levelExitFlags -= 0x10;
        }
    }
}

/* 0x510731 - starts follower `f` on trajectory `trajectory` (u16 count, then Vec3s points). `this` is not used. */
void ScnObject::TrajFollower_Init(TrajFollower *f, Trajectory *trajectory, s16 speed, s16 headingBias,
                                  u32 continuousHeading, u32 use3dDistance, s16 arriveRadius)
{
    f->traj = trajectory;
    f->pointIndex = 0;
    f->arriveRadiusSq = arriveRadius * arriveRadius;
    f->speed = speed;
    f->headingBias = headingBias;
    f->heading = 0;
    f->continuousHeading = continuousHeading;
    f->use3dDistance = use3dDistance;
    f->moving = 0;
}

/* 0x51079b - one step of the follower from this object's position: consumes every waypoint already within the arrival
 * radius (wrapping to the first; returns 1 when it wrapped), then the velocity toward the current one at f->speed and
 * the heading to it (recomputed only on a waypoint change unless continuous). */
/* BYTES(flow, inferred): the store is written in both arms because the original has it in both */
s32 ScnObject::TrajFollower_Step(TrajFollower *f, Vec3s *outVel, s16 *outHeading)
{
    Vec3s *cur;
    s32 arrived;
    s32 dy2;
    s32 ddv;
    s32 x;
    s32 dist;
    s32 result;
    result = 0;
    f->advanced = 0;
    f->moving = 1;
    do {
        cur = &f->traj->pts[f->pointIndex];
        x = cur->x - pos.x;
        dy2 = cur->z - pos.z;
        dist = x * x + dy2 * dy2;
        if (f->use3dDistance) {
            ddv = cur->y - pos.y;
            dist += ddv * ddv;
        }
        arrived = dist < f->arriveRadiusSq;
        if (arrived) {
            f->pointIndex++;
            f->advanced = 1;
            if (f->pointIndex >= f->traj->count) {
                result = 1;
                f->pointIndex = 0;
            }
        }
    } while (arrived);
    dist = (s32)sqrt((double)dist);
    outVel->x = x * f->speed / dist;
    outVel->z = dy2 * f->speed / dist;
    if (f->use3dDistance)
        outVel->y = ddv * f->speed / dist;
    else
        outVel->y = 0;
    if (f->advanced || f->continuousHeading) {
        f->heading = (f->headingBias + Math_RadiansToAngle4096((float)atan2(x, dy2))) & 0xfff;
        *outHeading = f->heading;
    } else {
        *outHeading = f->heading;
    }
    return result;
}

/* 0x510987, 369 bytes: origin points, X/Z only, inclusive bounds. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
s32 ObjGrid_QueryPointsInRectXZ(s32 minX, s32 minZ, s32 maxX, s32 maxZ, ScnObject **out)
{
    struct {
        s32 wideZ, wideX;
        Vec3s *point;
        ListNode **list;
        ListNode *node;
        ListNode ***cell;
        s16 cellsX, cellsZ, cellX, cellZ;
        ListNode ***row;
        ScnObject *object;
        s32 count;
    } w;
    w.count = 0;
    ObjGrid_CellFromXZ(minX, minZ, &w.cellX, &w.cellZ);
    ObjGrid_CellFromXZ(maxX, maxZ, &w.cellsX, &w.cellsZ);
    w.cellsZ -= w.cellZ;
    w.cellsX -= w.cellX;
    w.row = &g_objGridCells[w.cellX + w.cellZ * g_objGridDimX];
    while (w.cellsZ >= 0) {
        w.cell = w.row;
        w.cellX = w.cellsX;
        while (w.cellX >= 0) {
            w.list = *w.cell;
            w.node = *w.list;
            while (w.node) {
                w.object = (ScnObject *)w.node->data; /* cast kept: a list node carries its payload as void * */
                w.point = &w.object->pos;
                w.wideZ = w.point->z;
                w.wideX = w.point->x;
                if (GridOverlapFour(maxX - w.wideX, w.wideX - minX, maxZ - w.wideZ, w.wideZ - minZ) && w.count < 64) {
                    out[w.count] = w.object;
                    ++w.count;
                }
                w.node = w.node->next;
            }
            ++w.cell;
            --w.cellX;
        }
        w.row += g_objGridDimX;
        --w.cellsZ;
    }
    return w.count;
}

/* 0x510af8, 453 bytes: origin points, all three axes, inclusive bounds. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
s32 ObjGrid_QueryBoxPoints(const CollBox *query, ScnObject **out)
{
    struct {
        Vec3s *point;
        ListNode **list;
        ListNode *node;
        ListNode ***cell;
        s16 cellsX, cellsZ, cellX, cellZ;
        ListNode ***row;
        ScnObject *object;
        s32 count;
    } w;
    w.count = 0;
    ObjGrid_CellFromXZ(query->min.x, query->min.z, &w.cellX, &w.cellZ);
    ObjGrid_CellFromXZ(query->max.x, query->max.z, &w.cellsX, &w.cellsZ);
    w.cellsZ -= w.cellZ;
    w.cellsX -= w.cellX;
    w.row = &g_objGridCells[w.cellX + w.cellZ * g_objGridDimX];
    while (w.cellsZ >= 0) {
        w.cell = w.row;
        w.cellX = w.cellsX;
        while (w.cellX >= 0) {
            w.list = *w.cell;
            w.node = *w.list;
            while (w.node) {
                w.object = (ScnObject *)w.node->data; /* cast kept: a list node carries its payload as void * */
                w.point = &w.object->pos;
                if (GridContainsPoint(query, w.point) && w.count < 64) {
                    out[w.count] = w.object;
                    ++w.count;
                }
                w.node = w.node->next;
            }
            ++w.cell;
            --w.cellX;
        }
        w.row += g_objGridDimX;
        --w.cellsZ;
    }
    return w.count;
}

/* 0x510cbd, 631 bytes: solid-box X/Z overlap; fallback to origin when no solid box remains. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
s32 ObjGrid_QueryBoxesInRectXZ(s32 minX, s32 minZ, s32 maxX, s32 maxZ, ScnObject **out)
{
    u32 hit;
    u32 boxCount;
    CollBox *boxIterator;
    CollBox aabb;
    struct {
        s32 wideZ, wideX;
        ModelBoxList *boxList;
        Vec3s *point;
        ListNode **list;
        ListNode *node;
        ListNode ***cell;
        s16 cellsX, cellsZ, cellX, cellZ;
        ListNode ***row;
        ScnObject *object;
        s32 count;
    } w;
    w.count = 0;
    ObjGrid_CellFromXZ(minX, minZ, &w.cellX, &w.cellZ);
    ObjGrid_CellFromXZ(maxX, maxZ, &w.cellsX, &w.cellsZ);
    w.cellsZ -= w.cellZ;
    w.cellsX -= w.cellX;
    w.row = &g_objGridCells[w.cellX + w.cellZ * g_objGridDimX];
    while (w.cellsZ >= 0) {
        w.cell = w.row;
        w.cellX = w.cellsX;
        while (w.cellX >= 0) {
            w.list = *w.cell;
            w.node = *w.list;
            while (w.node) {
                w.object = (ScnObject *)w.node->data; /* cast kept: a list node carries its payload as void * */
                if (w.object->FlagsClear(SCN_OF_NO_BOX_COLLIDE)) {
                    w.boxList = w.object->inst_model->boxes;
                    if (!w.boxList) {
                        boxCount = 0;
                        boxIterator = 0;
                    } else {
                        boxCount = w.boxList->count;
                        boxIterator = w.boxList->boxes;
                    }
                    while (boxCount > 0 && (boxIterator->flags & COLLBOX_NONSOLID)) {
                        --boxCount;
                        ++boxIterator;
                    }
                } else {
                    boxCount = 0;
                    boxIterator = 0;
                }
                w.point = &w.object->pos;
                if (boxCount > 0) {
                    hit = 0;
                    while (boxCount > 0 && !hit) {
                        if (!(boxIterator->flags & COLLBOX_NONSOLID)) {
                            aabb.Box_Translate(boxIterator, w.point);
                            hit = GridOverlapFour(aabb.max.x - minX, maxX - aabb.min.x, aabb.max.z - minZ,
                                                  maxZ - aabb.min.z);
                        }
                        --boxCount;
                        ++boxIterator;
                    }
                } else {
                    w.wideZ = w.point->z;
                    w.wideX = w.point->x;
                    hit = GridOverlapFour(maxX - w.wideX, w.wideX - minX, maxZ - w.wideZ, w.wideZ - minZ);
                }
                if (hit && w.count < 64) {
                    out[w.count] = w.object;
                    ++w.count;
                }
                w.node = w.node->next;
            }
            ++w.cell;
            --w.cellX;
        }
        w.row += g_objGridDimX;
        --w.cellsZ;
    }
    return w.count;
}

/* 0x510f34, 817 bytes: solid-box overlap in all three axes; origin fallback. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
s32 ObjGrid_QueryBoxOverlap(CollBox *query, ScnObject **out)
{
    s32 hit;
    u32 boxCount;
    CollBox *boxIterator;
    CollBox aabb;
    struct {
        ModelBoxList *boxList;
        Vec3s *point;
        ListNode **list;
        ListNode *node;
        ListNode ***cell;
        s16 cellsX, cellsZ, cellX, cellZ;
        ListNode ***row;
        ScnObject *object;
        s32 count;
    } w;
    w.count = 0;
    ObjGrid_CellFromXZ(query->min.x, query->min.z, &w.cellX, &w.cellZ);
    ObjGrid_CellFromXZ(query->max.x, query->max.z, &w.cellsX, &w.cellsZ);
    w.cellsZ -= w.cellZ;
    w.cellsX -= w.cellX;
    w.row = &g_objGridCells[w.cellX + w.cellZ * g_objGridDimX];
    while (w.cellsZ >= 0) {
        w.cell = w.row;
        w.cellX = w.cellsX;
        while (w.cellX >= 0) {
            w.list = *w.cell;
            w.node = *w.list;
            while (w.node) {
                w.object = (ScnObject *)w.node->data; /* cast kept: a list node carries its payload as void * */
                if (w.object->FlagsClear(SCN_OF_NO_BOX_COLLIDE)) {
                    w.boxList = w.object->inst_model->boxes;
                    if (!w.boxList) {
                        boxCount = 0;
                        boxIterator = 0;
                    } else {
                        boxCount = w.boxList->count;
                        boxIterator = w.boxList->boxes;
                    }
                    while (boxCount > 0 && (boxIterator->flags & COLLBOX_NONSOLID)) {
                        --boxCount;
                        ++boxIterator;
                    }
                } else {
                    boxCount = 0;
                    boxIterator = 0;
                }
                w.point = &w.object->pos;
                if (boxCount > 0) {
                    hit = 0;
                    while (boxCount > 0 && !hit) {
                        if (!(boxIterator->flags & COLLBOX_NONSOLID)) {
                            aabb.Box_Translate(boxIterator, w.point);
                            hit = GridOverlapFour(aabb.max.x - query->min.x, query->max.x - aabb.min.x,
                                                  aabb.max.z - query->min.z, query->max.z - aabb.min.z) &&
                                  GridOverlapTwo(aabb.max.y - query->min.y, query->max.y - aabb.min.y);
                        }
                        --boxCount;
                        ++boxIterator;
                    }
                } else {
                    hit = GridContainsPoint(query, w.point);
                }
                if (hit && w.count < 64) {
                    out[w.count] = w.object;
                    ++w.count;
                }
                w.node = w.node->next;
            }
            ++w.cell;
            --w.cellX;
        }
        w.row += g_objGridDimX;
        --w.cellsZ;
    }
    return w.count;
}

/* 0x511265 - the 12-bit heading from `target` to this object (atan2 of the x and z differences). */
s16 ScnObject::HeadingTo(Vec3s *target)
{
    return Math_RadiansToAngle4096((float)atan2((double)pos.x - target->x, (double)pos.z - target->z)) & 0xfff;
}

/* 0x5112d6 - the water current of a zone box as a velocity: 100 / 400 / 200 by the speed bits (0x800000 slow,
 * 0x4000000 fast), about 1/sqrt(2) of it on each axis when an x bit and a z bit are both set. */
void Zone_GetFlowVelocity(const Box *zone, Vec3s *out)
{
    s16 rate;
    s32 diagonal;
    diagonal = (zone->flags & (FLOW_NEG_Z | FLOW_POS_Z)) && (zone->flags & (FLOW_NEG_X | FLOW_POS_X));
    if (zone->flags & FLOW_SLOW) {
        if (diagonal)
            rate = 0x47;
        else
            rate = 0x64;
    } else if (zone->flags & FLOW_FAST) {
        if (diagonal)
            rate = 0x11b;
        else
            rate = 0x190;
    } else {
        if (diagonal)
            rate = 0x8e;
        else
            rate = 0xc8;
    }
    out->y = 0;
    if (zone->flags & FLOW_POS_Z)
        out->z = rate;
    else if (zone->flags & FLOW_NEG_Z)
        out->z = -rate;
    else
        out->z = 0;
    if (zone->flags & FLOW_POS_X)
        out->x = rate;
    else if (zone->flags & FLOW_NEG_X)
        out->x = -rate;
    else
        out->x = 0;
}

/* 0x5113fa - the same current as a heading and a speed (no diagonal reduction); 0 and 0 with no direction bits. */
void Zone_GetFlowHeading(const Box *zone, s16 *heading, s32 *speed)
{
    s32 spd;
    s16 dir;
    if (zone->flags & (FLOW_NEG_X | FLOW_POS_X | FLOW_NEG_Z | FLOW_POS_Z)) {
        if (zone->flags & FLOW_SLOW)
            spd = 0x64;
        else if (zone->flags & FLOW_FAST)
            spd = 0x190;
        else
            spd = 0xc8;
        if (zone->flags & FLOW_POS_Z) {
            if (zone->flags & FLOW_POS_X)
                dir = 0xa00;
            else if (zone->flags & FLOW_NEG_X)
                dir = 0x600;
            else
                dir = 0x800;
        } else if (zone->flags & FLOW_NEG_Z) {
            if (zone->flags & FLOW_POS_X)
                dir = 0xe00;
            else if (zone->flags & FLOW_NEG_X)
                dir = 0x200;
            else
                dir = 0;
        } else if (zone->flags & FLOW_POS_X) {
            dir = 0xc00;
        } else if (zone->flags & FLOW_NEG_X) {
            dir = 0x400;
        } else {
            dir = 0;
        }
    } else {
        spd = 0;
        dir = 0;
    }
    *speed = spd;
    *heading = dir;
}

/* 0x511529, 130 bytes. Dispatch to the first 16 matches, preserving list order. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; pad0 / pad1 are fillers */
s32 ScnObject::Scenaric_SendToClass(u16 classId, s32 msg, void *arg)
{
    struct {
        ScnObject *objects[16];
        u16 pad0;
        u16 count;
        s32 result;
        u16 pad1;
        u16 index;
        s32 reply;
    } w;
    w.result = 0;
    w.count = (u16)Scenaric_FindByClass(classId, w.objects, 16);
    for (w.index = 0; w.index < w.count; w.index++) {
        w.reply = w.objects[w.index]->HandleMessage(this, msg, arg);
        if (w.reply)
            w.result = w.reply;
    }
    return w.result;
}

/* 0x5115ab  vtable +0x08: the rigid instance draw. */
void ScnLogic::Render(Camera *view)
{
    Instance_DrawRigid(Inst(), view, 0, 0);
}

/* 0x5115cf  vtable +0x08: the rigid draw, then the blob shadow when the object was drawn and is in the world. */
void ScnLogicShadowed::Render(Camera *view)
{
    Instance_DrawRigid(Inst(), view, 0, 0);
    if (InstFlags(INST_F_DRAWN) && IsInWorld()) {
        Shadow_Update(&shadow, &pos, this);
        Shadow_Render(&shadow);
    }
}

/* 0x511644  vtable +0x08: the animated draw. */
void ScnBody::Render(Camera *view)
{
    Instance_DrawAnimated(Inst(), &anim, view, 0);
}

/* 0x51166d - the animated draw at a screen position (HUD icons). */
void ScnBody::RenderEx(Camera *view, void *buffer, s32 unused, s32 projDist, s16 *screenXY)
{
    /* cast kept: Instance_DrawAnimatedOnScreen (world_draw.cpp) takes this buffer as an s32 */
    Instance_DrawAnimatedOnScreen(Inst(), &anim, view, (s32)buffer, unused, projDist, screenXY);
}

/* 0x5116a4 - the animated draw mirrored about z = mirrorPlane/2 and tinted, unless culled by camera distance. */
void ScnBody::RenderTinted(Camera *view, u32 color, u16 amount, u16 mirrorPlane)
{
    if (!IsFarCulled()) {
        g_mirrorRenderFlag = -1;
        g_mirrorRenderOn = 1;
        g_pViewFrustum->flipWinding = 0;
        g_mirrorPlaneVert2x = mirrorPlane;
        tintColor = color;
        tintAmount = amount;
        SetInstFlag(INST_F_TINT, 1);
        Instance_DrawAnimated(Inst(), &anim, view, 0);
        g_mirrorRenderFlag = 0;
        g_mirrorRenderOn = 0;
        g_pViewFrustum->flipWinding = 1;
        tintColor = 0;
        tintAmount = 0;
        SetInstFlag(INST_F_TINT, 0);
    }
}

/* 0x511804  vtable +0x08: the animated draw and the blob shadow. */
void ScnMobile::Render(Camera *view)
{
    Instance_DrawAnimated(Inst(), &anim, view, 0);
    if (InstFlags(INST_F_DRAWN) && IsInWorld()) {
        UpdateShadow();
        Shadow_Render(&shadow);
    }
}

/* 0x51186d  Root-joint offset + pos -> Shadow_Update, which re-projects the shadow only when its flags & 2. */
void ScnMobile::UpdateShadow()
{
    Vec3s point;
    Anim_GetRootOffset(Inst(), &anim, &point);
    point.x += pos.x;
    point.y += pos.y;
    point.z += pos.z;
    Shadow_Update(&shadow, &point, this);
}

/* 0x5118de  vtable +0x18: the rigid draw with a scale (0x400 = 1). */
void ScnLogic::RenderScaled(Camera *view, Vec3s *scale)
{
    Instance_DrawRigid(Inst(), view, scale, 0);
}

/* 0x511904  vtable +0x18 */
void ScnLogicShadowed::RenderScaled(Camera *view, Vec3s *scale)
{
    Instance_DrawRigid(Inst(), view, scale, 0);
    if (InstFlags(INST_F_DRAWN) && IsInWorld()) {
        Shadow_Update(&shadow, &pos, this);
        Shadow_Render(&shadow);
    }
}

/* 0x51197c  vtable +0x18 */
void ScnBody::RenderScaled(Camera *view, Vec3s *scale)
{
    Instance_DrawAnimated(Inst(), &anim, view, scale);
}

/* 0x5119a7  vtable +0x18 */
void ScnMobile::RenderScaled(Camera *view, Vec3s *scale)
{
    Instance_DrawAnimated(Inst(), &anim, view, scale);
    if (InstFlags(INST_F_DRAWN) && IsInWorld()) {
        UpdateShadow();
        Shadow_Render(&shadow);
    }
}

/* 0x511a12 - draws the object rotated / scaled about `pivot` (model space) instead of its origin, by moving it for the
 * draw by the offset that keeps the pivot in place. */
void ScnObject::RenderPivoted(Camera *view, Vec3s *pivot, Vec3s *scaleOrNull)
{
    Vec3s offset;
    Vec3s one;
    Render_CalcPivotOffset(&rot, pivot, &offset, scaleOrNull);
    pos.x += offset.x;
    pos.y += offset.y;
    pos.z += offset.z;
    if (scaleOrNull) {
        RenderScaled(view, scaleOrNull);
    } else {
        one.x = one.y = one.z = 0x400;
        RenderScaled(view, &one);
    }
    pos.x -= offset.x;
    pos.y -= offset.y;
    pos.z -= offset.z;
}

/* 0x511af8 - out = pivot * scale - R(rot, scale) * pivot. */
/* BYTES(slot-name, inferred): local names chosen for their stack slots (tools/vc6_locals.py), not recovered */
void Render_CalcPivotOffset(Vec3s *rot, Vec3s *pivot, Vec3s *outOffset, Vec3s *scaleOrNull)
{
    Mat34s matrix;
    Vec4s rpivot;
    Vec3s sp2;
    Mat34s_FromEulerScaled(rot, &matrix, scaleOrNull);
    Mat34s_TransformVec3s(&matrix, pivot, &rpivot);
    if (scaleOrNull) {
        sp2.x = pivot->x * scaleOrNull->x >> 10;
        sp2.y = pivot->y * scaleOrNull->y >> 10;
        sp2.z = pivot->z * scaleOrNull->z >> 10;
        outOffset->x = sp2.x - rpivot.x;
        outOffset->y = sp2.y - rpivot.y;
        outOffset->z = sp2.z - rpivot.z;
    } else {
        outOffset->x = pivot->x - rpivot.x;
        outOffset->y = pivot->y - rpivot.y;
        outOffset->z = pivot->z - rpivot.z;
    }
}

/* 0x511be4 - draws the object turned to face the camera. An attached object gets the rotation through its link's
 * matrix (the parent's rotation is replaced by it); a free one has its rot swapped for the draw. */
void ScnObject::RenderFacingCamera(Camera *view, s32 yawOnly, Vec3s *scaleOrNull, Vec3s *baseRotOrNull)
{
    Vec3s faceRot;
    Vec3s savedRot;
    Vec3s one;
    one.x = one.y = one.z = 0x400;
    CalcFacingCameraRot(&faceRot, view, yawOnly, baseRotOrNull);
    savedRot = rot;
    if (InstFlags(INST_F_ATTACHED)) {
        Mat44 turn;
        Mat44 *m;
        m = &attachLink->matrix;
        m->m[0][0] = m->m[1][1] = m->m[2][2] = 1.0f;
        m->m[0][1] = m->m[0][2] = m->m[1][2] = m->m[1][0] = m->m[2][0] = m->m[2][1] = 0.0f;
        turn.SetRotXZY(Math_Angle4096ToRadians_2(faceRot.x), Math_Angle4096ToRadians_2(faceRot.y),
                       Math_Angle4096ToRadians_2(faceRot.z));
        /* cast kept: MatrixReturnStorage is the constructor-free stand-in for Mat44_Mul's result temporary */
        *m = *Mat44_Mul((Mat44 *)&MatrixReturnStorage(), &turn, m);
        if (scaleOrNull)
            RenderScaled(view, scaleOrNull);
        else
            RenderScaled(view, &one);
    } else {
        rot = faceRot;
        if (scaleOrNull)
            RenderScaled(view, scaleOrNull);
        else
            RenderScaled(view, &one);
        rot = savedRot;
    }
}

/* 0x511dc1 - overrides joint `joint` of the displayed pose: rotation (4096/turn), translation and scale (0x400 = 1),
 * each only when given (channel bits 0x1c0 / 0xe00 / 0x7000), else zero / zero / one. */
void ScnBody::SetJointOverride(s32 joint, Vec3s *rotOrNull, Vec3s *transOrNull, Vec3s *scaleOrNull)
{
    AnimJointPose *pose;
    /* cast kept: the animator's pose buffer is untyped; the displayed pose is an AnimJointPose per joint */
    pose = (AnimJointPose *)anim.bufC + joint;
    pose->channels = 0;
    if (rotOrNull) {
        pose->rot[0] = Math_Angle4096ToRadians_2(rotOrNull->x);
        pose->rot[1] = Math_Angle4096ToRadians_2(rotOrNull->y);
        pose->rot[2] = Math_Angle4096ToRadians_2(rotOrNull->z);
        pose->channels |= ANIMKEY_ROT_X | ANIMKEY_ROT_Y | ANIMKEY_ROT_Z;
    } else {
        pose->rot[0] = pose->rot[1] = pose->rot[2] = 0.0f;
    }
    if (transOrNull) {
        pose->pos[0] = transOrNull->x;
        pose->pos[1] = transOrNull->y;
        pose->pos[2] = transOrNull->z;
        pose->channels |= ANIMKEY_POS_X | ANIMKEY_POS_Y | ANIMKEY_POS_Z;
    } else {
        pose->pos[0] = pose->pos[1] = pose->pos[2] = 0.0f;
    }
    if (scaleOrNull) {
        pose->scale[0] = Math_U16ToUnitFloat(scaleOrNull->x);
        pose->scale[1] = Math_U16ToUnitFloat(scaleOrNull->y);
        pose->scale[2] = Math_U16ToUnitFloat(scaleOrNull->z);
        pose->channels |= ANIMKEY_SCALE_X | ANIMKEY_SCALE_Y | ANIMKEY_SCALE_Z;
    } else {
        pose->scale[0] = pose->scale[1] = pose->scale[2] = 1.0f;
    }
}
