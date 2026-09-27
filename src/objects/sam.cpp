/* T088 - the original object Sam.cpp (guessed name), one file.
 * .text 0x461f20-0x46ed5c (Sam_PutDownCarried .. Sam_NearestPointInBoxesXZ, 68 functions, in address order),
 * .rdata 0x574ff8-0x57501c (the Sam vtable), .data 0x57a988-0x57ab24, .bss 0x6ccd28-0x6ccd38.
 * The members are declared in sam.h next to this file; the declarations of src/engine/grid_queries.h and
 * src/engine/property_math.h are repeated here instead of including them.
 * The embedded HUD head is typed ScnBody followed by its primitive buffers. Sam_Create uses `new Sam`, which emits the
 * real compiler vtables.
 */
/* BYTES: cast, flow, layout, slot-group, view. */
/* BYTES(layout): the Cine.h header static: every object including the cinematic header carries this copy in its .data, referenced or not */
/* BYTES(layout): written '= 0' only to keep definition order in .bss */
/* BYTES(layout): unreferenced trace strings kept as 4-aligned char arrays for the .data layout */
#include "sdw_enums.h"
#include "scenaric_props.h"

#include "sam.h"
#define SDW_INLINE_INSTANCE_INST 1
#include "instance_inlines.h"
#undef SDW_INLINE_INSTANCE_INST
/* src/engine/grid_queries.h */
#include "../engine/scenaric.h"
#include "../engine/scn_tools.h"
#include "../engine/maths.h"
#include "../game/scn_controllable.h"
#include "../engine/fixed_math.h"
#include "../engine/collide.h"
#include "../engine/navigation_api.h"
#include "../engine/screen.h"
#include "../engine/interface.h"
#include "camera.h"
#include "animation.h"
#include "../app/app_main.h"
#include "../engine/id_list.h"
#include "cinematicsmanager.h"
#include "../engine/sound_mgr.h"
#include "../engine/cine.h"
s32 ObjGrid_QueryBoxPoints(const CollBox *query, ScnObject **out);
s32 ObjGrid_QueryBoxOverlap(CollBox *query, ScnObject **out);
/* src/engine/property_math.h */
s32 Vec3s_ManhattanDist(Vec3s *a, Vec3s *b);
s32 Vec3s_DistXZ(Vec3s *a, Vec3s *b);
s32 Vec3s_Dist(Vec3s *a, Vec3s *b);
s32 Vec3s_DistSqXZ(Vec3s *a, Vec3s *b);

/* Sam's layout is generated from data/structs/Sam.csv and its element types (SamPathHist, SamTracked,
 * SamEdgeNormal, SamFetchGoal, NavSearch and the bitfield views).
 * Signed one-bit reads intentionally yield 0 or -1, as in the matched code.
 * Work records pin observed VC6 stack slots; their names are not original source.
 * This file preserves the original defects and unchecked operations. It is a
 * reconstruction, not a patched gameplay implementation.
 */

struct CollisionScratch {
    Vec3i projected, normalSum;
    Vec3s step;
    unsigned char contactClass[16];
};
extern int g_dt, g_dtMs;
extern "C" s16 g_sinTable4096[5122];
extern "C" const s16 *g_pCosTable;
extern unsigned char g_sharedScratch[];
#include "../sdk/crt.h"
extern "C" short Math_RadiansToAngle4096(float radians);
short Collide_GroundYRay(Vec3s *point, Vec3s *normal, short minV);
extern "C" int Coll_BoxGroundQuery(CollBox *box, int *height, ScnObject *self, unsigned char mode,
                                   ScnObject **outObject);

struct SamScreenGeometry {
    unsigned short width, height, x, y, aspect;
};
extern u32 *g_screenLayerBase;
extern unsigned int g_samZoneColors[];
extern short g_samAnimTable[], g_samAnimTableCarryOne[], g_samAnimTableCarryTwo[];
extern Wolf *g_pWolf;
int Vec3s_DistSqXZ(Vec3s *a, Vec3s *b);
class Instance;
struct Animator;
int Rand_Bounded(int limit);
int Scenaric_FindByClass(unsigned short classId, ScnObject **objects, int capacity);
void Camera_StartScripted(ScnObject *owner, Camera *camera, unsigned short pitch, unsigned short yaw,
                          unsigned short roll, Vec3s *eye, unsigned short focal, unsigned int flags, int blendTime);
void NavPath_BeginSearch(void *search, short x, short z, NavNode *startOrNull);
extern CollBox **g_samWolfCanBeHitZoneBoxes;
extern unsigned short g_samWolfCanBeHitZoneCount;
unsigned short Sound_Play(unsigned short sound, void *owner, unsigned short volume, unsigned char flags, int rate);
extern unsigned short g_samFadeAmount;
extern int g_samFadeEnabled;
unsigned int Anim_Start(Instance *instance, Animator *animation, unsigned short id, unsigned int flags);
#define SAM_ROUTE_ABS(v) ((v) >= 0 ? (v) : -(v))
inline int SamView_ContainsXZ(Vec3s *point, CollBox *box)
{
    return point->x >= box->min.x && point->x <= box->max.x && point->z >= box->min.z && point->z <= box->max.z;
}
inline int SamView_ContainsXYZ(Vec3s *point, CollBox *box)
{
    return point->x >= box->min.x && point->x <= box->max.x && point->y >= box->min.y && point->y <= box->max.y &&
           point->z >= box->min.z && point->z <= box->max.z;
}
inline int SamView_ContainsXZ(CollBox *box, Vec3s *point)
{
    return point->x >= box->min.x && point->x <= box->max.x && point->z >= box->min.z && point->z <= box->max.z;
}
inline int SamView_ContainsXYZ(CollBox *box, Vec3s *point)
{
    return point->x >= box->min.x && point->x <= box->max.x && point->y >= box->min.y && point->y <= box->max.y &&
           point->z >= box->min.z && point->z <= box->max.z;
}

/* BEGIN SAM UPDATE WORK RECORD */
struct SamUpdateWork {
    unsigned short catchAnimation1;
    unsigned short robotAnimation;
    void *cineText;
    CollBox *cineSheepBox;  /* EBP - 0x4d8 */
    CollBox *cineBox;       /* EBP - 0x4d4 */
    unsigned int cineFlags; /* EBP - 0x4d0 */
    unsigned int cineId;    /* EBP - 0x4cc */
    int tintEnabled;        /* EBP - 0x4c8 */
    unsigned char pad4c4[2];
    short tintAmount;                     /* EBP - 0x4c2 */
    unsigned short *tintClearFlags;       /* EBP - 0x4c0 */
    unsigned short *tintSetFlags;         /* EBP - 0x4bc */
    short directYaw;                      /* EBP - 0x4b8 */
    unsigned short investigateAnimation3; /* EBP - 0x4b6 */
    unsigned short investigateAnimation2; /* EBP - 0x4b4 */
    short investigateYaw;                 /* EBP - 0x4b2 */
    unsigned short investigateAnimation1; /* EBP - 0x4b0 */
    unsigned short pickupClass2;          /* EBP - 0x4ae */
    unsigned short pickupClass1;          /* EBP - 0x4ac */
    unsigned short overlapClass;          /* EBP - 0x4aa */
    NavSearch *routeSearch;               /* EBP - 0x4a8 */
    unsigned char pad4a4[2];
    short routeYaw;                   /* EBP - 0x4a2 */
    short idleYaw;                    /* EBP - 0x4a0 */
    unsigned short koGetUpId;         /* EBP - 0x49e */
    unsigned int koGetUpFlags;        /* EBP - 0x49c */
    unsigned short koAnimation498;    /* EBP - 0x498 */
    unsigned short koLyingId;         /* EBP - 0x496 */
    unsigned int koLyingFlags;        /* EBP - 0x494 */
    unsigned short koAnimation490;    /* EBP - 0x490 */
    unsigned short koAnimation48e;    /* EBP - 0x48e */
    short patrolYawCopy;              /* EBP - 0x48c */
    unsigned short reactionAnimation; /* EBP - 0x48a */
    unsigned short candidateClass488; /* EBP - 0x488 */
    unsigned short candidateClass486; /* EBP - 0x486 */
    Vec3s *excludePos;                /* EBP - 0x484 */
    CollBox *excludeBox;              /* EBP - 0x480 */
    Vec3s *catchPos;                  /* EBP - 0x47c */
    CollBox *catchBox;                /* EBP - 0x478 */
    unsigned char *homeRecord;        /* EBP - 0x474 */
    int watchDuration;                /* EBP - 0x470 */
    int watchProperty;                /* EBP - 0x46c */
    unsigned char *watchRecord;       /* EBP - 0x468 */
    unsigned short *cameraRecord;     /* EBP - 0x464 */
    int *goalDzOut;                   /* EBP - 0x460 */
    int *goalDxOut;                   /* EBP - 0x45c */
    unsigned char pad458[2];
    short currentYaw; /* EBP - 0x456 */
    int cineActive;   /* EBP - 0x454 */
    unsigned char pad450[2];
    unsigned short soundHandle44e; /* EBP - 0x44e */
    unsigned short initialVolume;  /* EBP - 0x44c */
    unsigned short soundHandle44a; /* EBP - 0x44a */
    unsigned short animation448;   /* EBP - 0x448 */
    unsigned short animation446;   /* EBP - 0x446 */
    unsigned short soundHandle444; /* EBP - 0x444 */
    unsigned short soundVolume;    /* EBP - 0x442 */
    int soundPlaying;              /* EBP - 0x440 */
    int runningAnimation;          /* EBP - 0x43c */
    unsigned char pad438[2];
    unsigned short animation436; /* EBP - 0x436 */
    NavNode *nodeStart;          /* EBP - 0x434 */
    NavNode *nodePathHead;       /* EBP - 0x430 */
    unsigned char pad42c[3];
    unsigned char nodeSearchResult; /* EBP - 0x429 */
    int nodeNearestDistance;        /* EBP - 0x428 */
    Vec3s nodeVelocity;             /* EBP - 0x424 */
    unsigned char pad41e[2];
    Vec3s nodeEnd; /* EBP - 0x41c */
    unsigned char pad416[4];
    short nodeGroundY; /* EBP - 0x412 */
    int nodeDz;        /* EBP - 0x410 */
    Vec3s nodeDelta;   /* EBP - 0x40c */
    unsigned char pad406[2];
    SamContactInfo nodeContacts; /* EBP - 0x404 */
    int nodeDx;                  /* EBP - 0x3e4 */
    int nodeDistance;            /* EBP - 0x3e0 */
    int nodeInside;              /* EBP - 0x3dc */
    int searchBudget;            /* EBP - 0x3d8 */
    int enteredDirect;           /* EBP - 0x3d4 */
    int searchHistory;           /* EBP - 0x3d0 */
    int searchBlocked;           /* EBP - 0x3cc */
    Vec3s searchEdgePoint;       /* EBP - 0x3c8 */
    unsigned char pad3c2[2];
    Vec3s searchVelocity; /* EBP - 0x3c0 */
    unsigned char pad3ba[2];
    Vec3s searchEnd; /* EBP - 0x3b8 */
    unsigned char pad3b2[4];
    short searchGroundY; /* EBP - 0x3ae */
    int searchDistance;  /* EBP - 0x3ac */
    Vec3s searchDelta;   /* EBP - 0x3a8 */
    unsigned char pad3a2[2];
    SamContactInfo searchContacts; /* EBP - 0x3a0 */
    unsigned char pad380[3];
    unsigned char searchEdgeIndex; /* EBP - 0x37d */
    NavNode *searchEdge;           /* EBP - 0x37c */
    unsigned char pad378[2];
    unsigned short searchStallLimit; /* EBP - 0x376 */
    int searchInside;                /* EBP - 0x374 */
    Vec3s directVelocity;            /* EBP - 0x370 */
    unsigned char pad36a[4];
    short directGroundY; /* EBP - 0x366 */
    Vec3s directDelta;   /* EBP - 0x364 */
    unsigned char pad35e[2];
    SamContactInfo directContacts; /* EBP - 0x35c */
    Vec3s directEnd;               /* EBP - 0x33c */
    unsigned char pad336[2];
    Vec3s investigateVelocity; /* EBP - 0x334 */
    unsigned char pad32e[2];
    Vec3s investigateDelta; /* EBP - 0x32c */
    unsigned char pad326[2];
    SamContactInfo investigateContacts; /* EBP - 0x324 */
    int investigateDz;                  /* EBP - 0x304 */
    Vec3s investigateEnd;               /* EBP - 0x300 */
    unsigned char pad2fa[2];
    int investigateDx;             /* EBP - 0x2f8 */
    int investigateDistance;       /* EBP - 0x2f4 */
    SamCarryGoal remainingCarry;   /* EBP - 0x2f0 */
    SamFetchGoal remainingFetch;   /* EBP - 0x2e0 */
    unsigned int pickupCostume;    /* EBP - 0x2d4 */
    int overlapIndex;              /* EBP - 0x2d0 */
    int overlapCount;              /* EBP - 0x2cc */
    ScnObject *overlapObjects[64]; /* EBP - 0x2c8 */
    int forceDrop;                 /* EBP - 0x1c8 */
    Vec3s routeCorrected;          /* EBP - 0x1c4 */
    unsigned char pad1be[4];
    short routeGroundY; /* EBP - 0x1ba */
    Vec3s toWaypoint;   /* EBP - 0x1b8 */
    unsigned char pad1b2[2];
    SamContactInfo routeContacts; /* EBP - 0x1b0 */
    unsigned char pad190[3];
    unsigned char routeResult; /* EBP - 0x18d */
    NavNode *neighbor;         /* EBP - 0x18c */
    Vec3s nearestEdgePoint;    /* EBP - 0x188 */
    unsigned char pad182[2];
    NavNode *nearestNode; /* EBP - 0x180 */
    unsigned char pad17c[3];
    unsigned char edgeIndex; /* EBP - 0x179 */
    NavNode *edgeNode;       /* EBP - 0x178 */
    Vec3s routeVector;       /* EBP - 0x174 */
    unsigned char pad16e[2];
    int nearestDistance; /* EBP - 0x16c */
    Vec3s routeEnd;      /* EBP - 0x168 */
    unsigned char pad162[2];
    Vec3s routeVelocity; /* EBP - 0x160 */
    unsigned char pad15a[2];
    NavNode *destinationNode; /* EBP - 0x158 */
    int needSearch;           /* EBP - 0x154 */
    Vec3s routeDelta;         /* EBP - 0x150 */
    unsigned char pad14a[2];
    int routeDz;           /* EBP - 0x148 */
    int changedNode;       /* EBP - 0x144 */
    int routeDx;           /* EBP - 0x140 */
    int routeDistance;     /* EBP - 0x13c */
    NavNode *savedStart;   /* EBP - 0x138 */
    ScnObject *guardSheep; /* EBP - 0x134 */
    Vec3s jointAngles;     /* EBP - 0x130 */
    unsigned char pad12a[2];
    int patrolReached;    /* EBP - 0x128 */
    Vec3s patrolVelocity; /* EBP - 0x124 */
    unsigned char pad11e[2];
    Vec3s patrolDelta; /* EBP - 0x11c */
    unsigned char pad116[4];
    short patrolYaw;               /* EBP - 0x112 */
    SamContactInfo patrolContacts; /* EBP - 0x110 */
    ScnObject *reactionObject;     /* EBP - 0xf0 */
    SamFetchGoal pendingFetch;     /* EBP - 0xec */
    ScnObject *selected[2];        /* EBP - 0xe0 */
    ScnObject *objects[32];        /* EBP - 0xd8 */
    unsigned char pad058[4];
    int objectIndex; /* EBP - 0x54 */
    int objectCount; /* EBP - 0x50 */
    unsigned char pad04c[3];
    signed char candidateCount; /* EBP - 0x49 */
    ScnObject *candidate;       /* EBP - 0x48 */
    int sheepCostume;           /* EBP - 0x44 */
    int interruptRegion;        /* EBP - 0x40 */
    int invalidFetch;           /* EBP - 0x3c */
    unsigned int costume;       /* EBP - 0x38 */
    int targetRegion;           /* EBP - 0x34 */
    unsigned int hitRegion;     /* EBP - 0x30 */
    ScnObject *firstSheep;      /* EBP - 0x2c */
    int reachDistance;          /* EBP - 0x28 */
    CollBox *cameraBox;         /* EBP - 0x24 */
    int wolfCameraQuery;        /* EBP - 0x20 */
    unsigned char pad01c[3];
    unsigned char collisionMask; /* EBP - 0x19 */
    Vec3s intended;              /* EBP - 0x18 */
    unsigned char pad012[2];
    int wolfDx;     /* EBP - 0x10 */
    Vec3s *selfPos; /* EBP - 0xc */
    int dt;         /* EBP - 0x8 */
    int wolfDz;     /* EBP - 0x4 */
};
/* END SAM UPDATE WORK RECORD */
struct SamDropMessage {
    Vec3s position;
    unsigned short place : 1;
    unsigned short keepPosition : 1;
    unsigned short other : 14;
};
unsigned int Anim_Start(Instance *instance, Animator *animation, unsigned short id, unsigned int flags);

/* ---- data, in address order ----
 * .data 0x57a988: this object's copy of the cinematic opcode stride table. It is a static table of a header every
 * cinematic user includes (one copy per object, src/engine/cine2.cpp); nothing here reads it. */
static u8 g_cineOpStrideCopy[9] = {0, 8, 8, 4, 2, 2, 4, 2, 2};
/* 0x57a994 / 0x57a9c8 / 0x57a9fc - logical -> model animation ids: normal, carrying one sheep, carrying two.
 * Indexed by SamAnim; the model is ASAMSH01 in every level. Entry 25 has no SamAnim value (the enum ends at 24). */
short g_samAnimTable[26] = {ASAMSH01_ANIM_JUMP1,   ASAMSH01_ANIM_JUMP2,
                            ASAMSH01_ANIM_JUMP3,   ASAMSH01_ANIM_PUNCH7,
                            ASAMSH01_ANIM_PUNCH3,  ASAMSH01_ANIM_RUN2,
                            ASAMSH01_ANIM_HEAD1,   ASAMSH01_ANIM_HEAD2,
                            ASAMSH01_ANIM_WALK1,   ASAMSH01_ANIM_WALK2,
                            ASAMSH01_ANIM_HYPNOS1, ASAMSH01_ANIM_HYPNOS2,
                            ASAMSH01_ANIM_SQUASH,  ASAMSH01_ANIM_SQUASH1,
                            ASAMSH01_ANIM_SQUASH2, ASAMSH01_ANIM_CARRY1A,
                            ASAMSH01_ANIM_CARRY1B, ASAMSH01_ANIM_CARRY2A,
                            ASAMSH01_ANIM_CARRY2B, ASAMSH01_ANIM_WIN,
                            ASAMSH01_ANIM_BEE,     ASAMSH01_ANIM_DUPE,
                            ASAMSH01_ANIM_KICK,    ASAMSH01_ANIM_RUN1,
                            ASAMSH01_ANIM_START,   0};
short g_samAnimTableCarryOne[26] = {ASAMSH01_ANIM_JUMP1,   ASAMSH01_ANIM_JUMP2,
                                    ASAMSH01_ANIM_JUMP3,   ASAMSH01_ANIM_PUNCH7,
                                    ASAMSH01_ANIM_PUNCH3,  ASAMSH01_ANIM_WALK4,
                                    ASAMSH01_ANIM_HEAD1,   ASAMSH01_ANIM_HEAD2,
                                    ASAMSH01_ANIM_WALK4,   ASAMSH01_ANIM_WALK2,
                                    ASAMSH01_ANIM_HYPNOS1, ASAMSH01_ANIM_HYPNOS2,
                                    ASAMSH01_ANIM_SQUASH,  ASAMSH01_ANIM_SQUASH1,
                                    ASAMSH01_ANIM_SQUASH2, ASAMSH01_ANIM_CARRY3A,
                                    ASAMSH01_ANIM_CARRY1B, ASAMSH01_ANIM_CARRY2A,
                                    ASAMSH01_ANIM_CARRY2B, ASAMSH01_ANIM_WIN,
                                    ASAMSH01_ANIM_BEE,     ASAMSH01_ANIM_DUPE,
                                    ASAMSH01_ANIM_KICK,    ASAMSH01_ANIM_RUN1,
                                    ASAMSH01_ANIM_START,   0};
short g_samAnimTableCarryTwo[26] = {ASAMSH01_ANIM_JUMP1,   ASAMSH01_ANIM_JUMP2,
                                    ASAMSH01_ANIM_JUMP3,   ASAMSH01_ANIM_PUNCH7,
                                    ASAMSH01_ANIM_PUNCH3,  ASAMSH01_ANIM_WALK5,
                                    ASAMSH01_ANIM_HEAD1,   ASAMSH01_ANIM_HEAD2,
                                    ASAMSH01_ANIM_WALK5,   ASAMSH01_ANIM_WALK2,
                                    ASAMSH01_ANIM_HYPNOS1, ASAMSH01_ANIM_HYPNOS2,
                                    ASAMSH01_ANIM_SQUASH,  ASAMSH01_ANIM_SQUASH1,
                                    ASAMSH01_ANIM_SQUASH2, ASAMSH01_ANIM_CARRY3A,
                                    ASAMSH01_ANIM_CARRY3B, ASAMSH01_ANIM_CARRY4A,
                                    ASAMSH01_ANIM_CARRY4B, ASAMSH01_ANIM_WIN,
                                    ASAMSH01_ANIM_BEE,     ASAMSH01_ANIM_DUPE,
                                    ASAMSH01_ANIM_KICK,    ASAMSH01_ANIM_RUN1,
                                    ASAMSH01_ANIM_START,   0};
/* 0x57aa30 - the alert-zone indicator colours by alertLevel */
unsigned int g_samZoneColors[3] = {0x1f702a, 0xf50a0, 4336};
/* 0x57aa3c-0x57ab24 - state-entry trace strings that nothing in the image refers to (no code or data pointer): kept
 * as the original's bytes, each char array 4-aligned as VC6 places them in .data. The first has the table name of
 * 0x57aa3c (s_EnterFollowTrajectoryState, data/symbols_modules.csv); the other eight are named by address. */
static char s_EnterFollowTrajectoryState[] = "EnterFollowTrajectoryState\n";
static char g_samTraceFollowTrajectoryFast[] = "EnterFollowTrajectoryState\n";
static char g_samTraceLookAfterTimed[] = "EnterLookAfterState\n";
static char g_samTraceLookAfter[] = "EnterLookAfterState\n";
static char g_samTraceFollowNode[] = "Enterfollownodestate\n";
static char g_samTraceReachGoal[] = "EnterReachGoalState\n";
static char g_samTraceSearchNearestNode[] = "EnterSearchNearestNodeState\n";
static char g_samTraceReachNode[] = "EnterReachNodeState\n";
static char g_samTraceFollowNodes[] = "SAM_STT_FOLLOWNODES\n";
/* .bss 0x6ccd28-0x6ccd38. VC6 emits a file's uninitialised globals in the order of a hash of their names
 * ((h<<2)+(h>>4)+c, folded, & 1023), then the explicitly zero-initialised ones in definition order: the first two
 * already hash in address order, the other two are placed after them with `= 0`. */
unsigned short g_samFadeAmount;                /* 0x6ccd28 */
int g_samFadeEnabled;                          /* 0x6ccd2c */
CollBox **g_samWolfCanBeHitZoneBoxes = 0;      /* 0x6ccd30 */
unsigned short g_samWolfCanBeHitZoneCount = 0; /* 0x6ccd34 */

/* 0x461F20 - Sam_PutDownCarried; 335 compared bytes including any local tables. */
void Sam::PutDownCarried(unsigned short keepPosition)
{
    Vec3s *w;
    SamDropMessage top;
    w = &top.position;
    /* Preserve the original partial flags write; upper bits are untouched. */
    top.place = 1;
    top.keepPosition = keepPosition;
    w->x = pos.x + 100;
    w->z = pos.z;
    w->y = pos.y - 300;
    w->y = carryGoal.object1->QueryGroundY(w, 1);
    RemoveCarried(carryGoal.object1);
    carryGoal.object1->HandleMessage(this, MSG_DROP, &top);
    if (carryGoal.object2) {
        w->x = pos.x - 100;
        w->z = pos.z;
        w->y = pos.y - 300;
        w->y = carryGoal.object2->QueryGroundY(w, 1);
        RemoveCarried(carryGoal.object2);
        carryGoal.object2->HandleMessage(this, MSG_DROP, &top);
    }
}

/* 0x46206F - Sam_IsTargetNearFinalPathEdge; 258 compared bytes including any local tables. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
int Sam::IsTargetNearFinalPathEdge()
{
    struct {
        Vec3s closest;
        int distance;
    } w;
    if (waypointCount >= 3) {
        if (Vec3s_ManhattanDistXZ(&targetPos, &waypoints[1]) < 300 ||
            Vec3s_ManhattanDistXZ(&targetPos, &waypoints[2]) < 300) {
            w.distance = SamNav_DistToEdge(&targetPos, &followEdgeNormal, waypoints[1].x, waypoints[1].z,
                                           waypoints[2].x, waypoints[2].z, &w.closest);
            if (w.distance < 150)
                return 1;
        }
        return 0;
    } else {
        if (waypointCount >= 2 && Vec3s_ManhattanDistXZ(&targetPos, &waypoints[1]) < 150)
            return 1;
        return 0;
    }
}

/* 0x462171 - Sam_IsDistBelowHistoryThreshold; 51 compared bytes including any local tables. */
int Sam::IsDistBelowHistoryThreshold(int distance)
{
    if (speed <= 200)
        return distance < 12;
    else
        return distance < 50;
}

/* 0x4621A4 - Sam_FreezePickupTarget; 86 compared bytes including any local tables. */
void Sam::FreezePickupTarget(ScnObject *object)
{
    unsigned short kind = object->classId;
    if (kind == CLASSID_WOLF)
        object->HandleMessage(this, MSG_FREEZE, 0);
    else
        object->HandleMessage(this, MSG_SCRIPT_HOLD, 0);
    frozenPickupObj = object;
}

/* 0x4621FA - Sam_UnfreezePickupTarget; 108 compared bytes including any local tables. */
void Sam::UnfreezePickupTarget(ScnObject *object)
{
    if (!object)
        frozenPickupObj = 0;
    else {
        if (object->SamView_GetClassId() == CLASSID_WOLF)
            object->HandleMessage(this, MSG_UNFREEZE, 0);
        else
            object->HandleMessage(this, MSG_SCRIPT_RELEASE, 0);
        frozenPickupObj = 0;
    }
}

/* 0x462266 - Sam_PointInBoxListXZ; 183 compared bytes including any local tables. */
int Sam::PointInBoxListXZ(Vec3s *point, CollBox **boxes, unsigned short count)
{
    unsigned short i;
    CollBox *box;
    if (boxes) {
        for (i = 0; i < count; ++i) {
            box = boxes[i];
            if (SamView_ContainsXZ(point, box))
                return 1;
        }
        return 0;
    }
    return 1;
}

/* 0x46231D - Sam_PointInBoxListXYZ; 223 compared bytes including any local tables. */
int Sam::PointInBoxListXYZ(Vec3s *point, CollBox **boxes, unsigned short count)
{
    unsigned short i;
    CollBox *box;
    if (boxes) {
        for (i = 0; i < count; ++i) {
            box = boxes[i];
            if (SamView_ContainsXYZ(point, box))
                return 1;
        }
        return 0;
    }
    return 1;
}

/* 0x4623FC - Sam_ClearPathHistory; 74 compared bytes including any local tables. */
void Sam::ClearPathHistory()
{
    unsigned short i;
    for (i = 0; i < 16; ++i)
        pathHistory[i].type = SAM_HIST_EMPTY;
    pathHistoryIdx = 15;
}

/* 0x462446 - Sam_Init; 2564 compared bytes including any local tables. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; unused22 fill gaps */
/* BYTES(slot-group): the local record keeps the original inlined property-access slots */
/* BYTES(flow): a switch on a constant: VC6 keeps all four arms, as in the original */
void Sam::PostLoadInit()
{
    /* cast kept (every (CollBox **) and (Trajectory **) id-list read in this function): an export id list holds
     * record pointers of any kind; the caller knows which kind it asked for */
    /* Explicit local record preserves the original inlined property-access slots. */
    struct {
        int cineTextValue, cineTextOffset, cineFlagsOffset, cineSheepBoxOffset, cineBoxOffset;
        int cineIdValue, cineIdOffset, modeValue, modeOffset, patrolDistanceValue, patrolDistanceOffset;
        int trajectoryOffset, maxAngleValue, maxAngleOffset, headSpeedValue, headSpeedOffset;
        unsigned short jumpAnim;
        short initialYaw;
        int dontCatchOffset, catchOffset, hypnotizedOffset, authorizedOffset, orangeOffset, greenOffset;
        Trajectory **trajectoryList;
        CollBox *putBox;
        CollBox **boxList;
        Vec3s groundedPosition;
        unsigned short unused22;
        unsigned int cineValue;
        unsigned short unused1C, boxCount;
        ScnObject *firstSheep;
        int trajectoryId;
        unsigned short unused10, trajectoryCount;
        unsigned char *record;
        CollBox **putBoxes;
        int value;
    } w;
/* cast kept (SAM_INIT_PROP and w.record): designer properties are 4-byte slots at byte offsets of the raw WAR record */
#define SAM_INIT_PROP(slot, offset, destination) \
    w.slot = offset;                             \
    destination = *(int *)(w.record + w.slot + 0x14)
    /* cast kept: the level record read as bytes */
    w.record = (unsigned char *)this->record;
    if (!Scenaric_FindByClass(CLASSID_ROBOT, &robot, 1))
        robot = 0;
    target = g_pWolf;
    animTable = g_samAnimTable;
    carriedCount = 0;
    speed = 1100;
    chaseSoundHandle = 0;
    chaseSoundBits.volume = 0;
    sheepFlags &= ~SAM_SHEEP_HEARD_CALL;
    catapultLoadedObj = 0;
    Scenaric_FindByClass(CLASSID_SHEEP, &w.firstSheep, 1);
    firstSheepSpawnPos = w.firstSheep->pos;

    SAM_INIT_PROP(greenOffset, 0x2C, w.value);
    if (w.value)
        /* cast kept: an export id list holds its boxes' pointers as u32 words */
        greenZoneBoxes = (CollBox **)Scn_FindIdList((unsigned short)w.value, &greenZoneCount);
    else {
        greenZoneBoxes = 0;
        greenZoneCount = 0;
    }
    SAM_INIT_PROP(orangeOffset, 0x44, w.value);
    if (w.value)
        /* cast kept: an export id list holds its boxes' pointers as u32 words */
        orangeZoneBoxes = (CollBox **)Scn_FindIdList((unsigned short)w.value, &orangeZoneCount);
    else {
        orangeZoneBoxes = 0;
        orangeZoneCount = 0;
    }
    SAM_INIT_PROP(authorizedOffset, 4, w.value);
    if (w.value)
        /* cast kept: an export id list holds its boxes' pointers as u32 words */
        authorizedZoneBoxes = (CollBox **)Scn_FindIdList((unsigned short)w.value, &authorizedZoneCount);
    else {
        authorizedZoneBoxes = 0;
        authorizedZoneCount = 0;
    }
    SAM_INIT_PROP(hypnotizedOffset, 0x38, w.value);
    if (w.value)
        /* cast kept: an export id list holds its boxes' pointers as u32 words */
        hypnotizedZoneBoxes = (CollBox **)Scn_FindIdList((unsigned short)w.value, &hypnotizedZoneCount);
    else {
        hypnotizedZoneBoxes = 0;
        hypnotizedZoneCount = 0;
    }
    SAM_INIT_PROP(catchOffset, 8, w.value);
    catchableSheepBox = 0;
    if (w.value) {
        /* cast kept: an export id list holds its boxes' pointers as u32 words */
        w.boxList = (CollBox **)Scn_FindIdList((unsigned short)w.value, &w.boxCount);
        if (w.boxCount == 1)
            catchableSheepBox = *w.boxList;
    }
    SAM_INIT_PROP(dontCatchOffset, 0x28, w.value);
    dontTryToCatchSheepBox = 0;
    if (w.value) {
        /* cast kept: an export id list holds its boxes' pointers as u32 words */
        w.boxList = (CollBox **)Scn_FindIdList((unsigned short)w.value, &w.boxCount);
        if (w.boxCount == 1)
            dontTryToCatchSheepBox = *w.boxList;
    }
    /* cast kept: an export id list holds its records' pointers as u32 words */
    w.putBoxes = (CollBox **)Scn_GetPropIdList(w.record, 0x48, &w.boxCount);
    if (w.boxCount == 1) {
        w.putBox = *w.putBoxes;
        putSheepPos.x = (w.putBox->min.x + w.putBox->max.x) / 2;
        putSheepPos.y = (w.putBox->min.y + w.putBox->max.y) / 2;
        putSheepPos.z = (w.putBox->min.z + w.putBox->max.z) / 2;
    } else
        putSheepPos = pos;
    /* cast kept: an export id list holds its records' pointers as u32 words */
    wolfCanBeHitZoneBoxes = (CollBox **)Scn_GetPropIdList(w.record, 0x54, &wolfCanBeHitZoneCount);
    g_samWolfCanBeHitZoneBoxes = wolfCanBeHitZoneBoxes;
    g_samWolfCanBeHitZoneCount = wolfCanBeHitZoneCount;
    hidingBoxes = (CollBox **)Scn_GetPropIdList(w.record, 0x34, &hidingBoxCount);
    UpdateZoneState();
    investigateTimerMs = -1;
    lifeTimerMs = 0;
    SetGoalWolf();
    yawToWolf = 0;
    frameCounter = 0;
    jumpPhase = SAM_JUMP_NONE;
    w.initialYaw = rot.y;
    spawnYaw = w.initialYaw;
    alertLevel = SAM_ALERT_NONE;
    restartStatePending = 0;
    frozenPickupObj = 0;
    freezeTimerMs = 0;
    chaseFlags &= ~SAM_CHASE_AFTER_DROP;
    sheepMovedAccum = 0;
    w.jumpAnim = GetAnimId(SAM_ANIM_JUMP_AIR);
    jumpDurationMs = Anim_GetDurationMs(Inst(), w.jumpAnim, 0);
    w.groundedPosition = pos;
    w.groundedPosition.y = QueryGroundY_Shrunk(&w.groundedPosition);
    SetPosition(&w.groundedPosition);
    SetState(SAM_STT_IDLE);
    SamNav_BuildGraph(WAR_IDO_SAMTRAJ);
    NavPath_BeginSearch(&navSearch, g_pWolf->pos.x, g_pWolf->pos.z, 0);
    waypointCount = 0;
    lastNodeTarget.z = -32768;
    lastNodeTarget.y = -32768;
    lastNodeTarget.x = -32768;
    homePos = pos;
    /* Constant choice, but VC6 preserves all four original switch arms. */
    switch ((u32)SCN_UPD_ALWAYS) {
        case SCN_UPD_NORMAL:
            flags &= (u16)~SCN_OF_ALWAYS_UPDATE;
            flags &= (u16)~SCN_OF_NEVER_UPDATE;
            break;
        case SCN_UPD_ALWAYS:
            flags |= SCN_OF_ALWAYS_UPDATE;
            flags &= (u16)~SCN_OF_NEVER_UPDATE;
            break;
        case SCN_UPD_NEVER:
            flags &= (u16)~SCN_OF_ALWAYS_UPDATE;
            flags |= SCN_OF_NEVER_UPDATE;
            break;
        case SCN_UPD_CINE:
            flags |= SCN_OF_CINE_UPDATE;
            break;
    }
    ClearPathHistory();
    InitHeadRecord(headRecord);
    headSweepPhase = 0;
    headYaw = 0;
    SAM_INIT_PROP(headSpeedOffset, 0x30, w.headSpeedValue);
    headSpeed = (short)w.headSpeedValue;
    SAM_INIT_PROP(maxAngleOffset, 0x3C, w.maxAngleValue);
    maxAngle = (short)w.maxAngleValue;
    relAngleToWolf = 0;
    head.Init(headRecord, 0);
    SAM_INIT_PROP(trajectoryOffset, 0x4C, w.trajectoryId);
    trajectory = 0;
    if (w.trajectoryId) {
        /* cast kept: an export id list holds its records' pointers as u32 words */
        w.trajectoryList = (Trajectory **)Scn_FindIdList((unsigned short)w.trajectoryId, &w.trajectoryCount);
        if (w.trajectoryCount == 1)
            trajectory = *w.trajectoryList;
    }
    SAM_INIT_PROP(patrolDistanceOffset, 0x24, w.patrolDistanceValue);
    distCatchPatrol = w.patrolDistanceValue;
    SAM_INIT_PROP(modeOffset, 0x40, w.modeValue);
    mode = w.modeValue;
    EnterDefaultState();
    SAM_INIT_PROP(cineIdOffset, 0xC, w.cineIdValue);
    cin01 = w.cineIdValue;
    SAM_INIT_PROP(cineBoxOffset, 0x10, w.cineValue);
    /* cast kept: an export id list holds its boxes' pointers as u32 words */
    cin01Box = w.cineValue ? *(CollBox **)Scn_FindIdList((unsigned short)w.cineValue, &w.trajectoryCount) : 0;
    SAM_INIT_PROP(cineSheepBoxOffset, 0x18, w.cineValue);
    cin01SheepBox = w.cineValue ? *(CollBox **)Scn_FindIdList((unsigned short)w.cineValue, &w.trajectoryCount) : 0;
    SAM_INIT_PROP(cineFlagsOffset, 0x14, w.cineValue);
    cin01Flags = w.cineValue;
    SAM_INIT_PROP(cineTextOffset, 0x1C, w.cineTextValue);
    /* cast kept: Cine_ResolveText returns a const char *; the handle is kept as a void * */
    cin01TextHandle = (void *)Cine_ResolveText(this, w.cineValue, (unsigned char)w.cineTextValue);
    if (mode & SAM_MODE_LEVEL_VARIANT)
        InitBeachVariant();
    SnapshotSheepPositions();
#undef SAM_INIT_PROP
}

/* 0x462E4A - Sam_GetAnimId; 27 compared bytes including any local tables. */
unsigned short Sam::GetAnimId(unsigned short index)
{
    return ((unsigned short *)animTable)[index]; /* cast kept: the table is s16; the id is read unsigned */
}

/* 0x462E65 - Sam_SetupJump; 571 compared bytes including any local tables. */
/* BYTES(slot-group): locals grouped in w / top only to pin the original frame offsets; unused172, unused16, unused2 fill gaps */
void Sam::SetupJump(Vec3s *point)
{
    struct {
        ModelBoxList *boxList;
        int height;
        Vec3s probeDelta;
        short unused172;
        CollBox worldBox;
        CollContact contacts[16];
        int staticHeight;
        Vec3s unusedVector;
        short unused16;
        int length, fraction, minHeight;
        Vec3s offset;
        short unused2;
    } w;
    struct {
        CollBox *box;
    } top;
    w.unusedVector.x = 0;
    w.unusedVector.y = 0;
    w.unusedVector.z = 0;
    w.probeDelta.x = 0;
    w.probeDelta.y = 100;
    w.probeDelta.z = 0;
    w.offset.x = point->x - pos.x;
    w.offset.y = point->y - pos.y;
    w.offset.z = point->z - pos.z;
    jumpLandPos = *point;
    w.length = (int)sqrt((double)w.offset.x * w.offset.x + w.offset.z * w.offset.z);
    if (w.length != 0) {
        w.offset.x = (short)(w.offset.x * 100 / w.length);
        w.offset.z = (short)(w.offset.z * 100 / w.length);
        w.offset.y = 0;
        jumpLandPos.x = w.offset.x + jumpLandPos.x;
        jumpLandPos.y = w.offset.y + jumpLandPos.y;
        jumpLandPos.z = w.offset.z + jumpLandPos.z;
    }
    w.boxList = inst_model->boxes;
    if (w.boxList)
        top.box = w.boxList->boxes;
    else
        top.box = 0;
    w.worldBox.Box_Translate(top.box, &jumpLandPos);
    if ((int)Collide_SweepBox_Sam(this, &w.worldBox, &w.probeDelta, &w.fraction, &w.height, &w.minHeight, w.contacts,
                                  this, CQ_STATIC | CQ_OBJECTS | CQ_SAM_WALL_OVERLAP, &w.staticHeight) >= 0x8000)
        jumpPhase = SAM_JUMP_NONE;
    else
        AddPathHistory(SAM_HIST_JUMP, 0, 0);
}

/* 0x4630A0 - Sam_PlayAnim; 111 compared bytes including any local tables. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; unused fill gaps */
void Sam::PlayAnim(unsigned short index, int loop, int flag2, short speed)
{
    struct {
        short unused;
        unsigned short id;
        unsigned int flags;
    } w;
    anim.speed = speed;
    w.id = GetAnimId(index);
    w.flags = 0;
    if (loop)
        w.flags |= ANIM_SET_LOOP;
    if (flag2)
        w.flags |= ANIM_SET_BLEND;
    Anim_Start(Inst(), &anim, w.id, w.flags);
}

/* 0x46310F - Sam_UpdateJump; 822 compared bytes including any local tables. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; unused2A, unused22 fill gaps */
void Sam::UpdateJump()
{
    struct {
        Vec3s delta;
        short unused2A;
        Vec3s next;
        short unused22;
        ContactInfo info;
        int index;
    } w;
    switch (jumpPhase) {
        case SAM_JUMP_START:
            PlayAnim(SAM_ANIM_JUMP_START, 0, 0, 4096);
            ++jumpPhase;
            break;
        case SAM_JUMP_LAUNCH:
            stateTimerMs = 0;
            jumpStartPos = pos;
            jumpPrevPos = jumpStartPos;
            jumpPrevPos.x = (jumpPrevPos.x - jumpStartPos.x) * 2 + jumpStartPos.x;
            jumpPrevPos.y = (jumpPrevPos.y - jumpStartPos.y) * 2 + jumpStartPos.y;
            jumpPrevPos.z = (jumpPrevPos.z - jumpStartPos.z) * 2 + jumpStartPos.z;
            PlayAnim(SAM_ANIM_JUMP_AIR, 0, 0, 4096);
            ++jumpPhase;
            break;
        case SAM_JUMP_LAND:
            PlayAnim(SAM_ANIM_JUMP_LAND, 0, 0, 4096);
            ++jumpPhase;
            break;
        case SAM_JUMP_AIR:
            if (jumpDurationMs != 0) {
                w.index = (stateTimerMs << 11) / jumpDurationMs;
                w.next.y = (short)(((g_sinTable4096[w.index] * -400) >> 12) + jumpStartPos.y);
                w.next.x = (short)((jumpLandPos.x - jumpStartPos.x) * stateTimerMs / jumpDurationMs + jumpStartPos.x);
                w.next.z = (short)((jumpLandPos.z - jumpStartPos.z) * stateTimerMs / jumpDurationMs + jumpStartPos.z);
                w.delta.x = w.next.x - jumpPrevPos.x;
                w.delta.y = w.next.y - jumpPrevPos.y;
                w.delta.z = w.next.z - jumpPrevPos.z;
                jumpPrevPos = w.next;
                Collide_ResolveMove(&w.delta, &w.info, 2900, RESOLVE_SLIDE_ALL | RESOLVE_KEEP_Y | RESOLVE_ASK_MOVER, 0,
                                    0, 10, 0, 0);
                Translate(&w.delta);
            }
            /* The original shares the animation-completion check with 2 and 6. */
        case SAM_JUMP_START_WAIT:
        case SAM_JUMP_LAND_WAIT:
            if (HasAnimFlags(ANIM_F_FINISHED))
                ++jumpPhase;
            break;
        case SAM_JUMP_DONE:
            jumpPhase = SAM_JUMP_NONE;
            stateTimerMs = 0;
            SetState((signed char)state);
            break;
    }
}

/* 0x463445 - Sam_Reset; 470 compared bytes including any local tables. */
void Sam::Reset()
{
    chaseSoundHandle = 0;
    chaseSoundBits.volume = 0;
    sheepFlags &= ~SAM_SHEEP_HEARD_CALL;
    if (mode & SAM_MODE_LEVEL_VARIANT)
        InitBeachVariant();
    homePos.y = QueryGroundY(&homePos, 1) + 5;
    SetPosition(&homePos);
    SnapToGround(0);
    restartStatePending = 0;
    EnterDefaultState();
    UpdateZoneState();
    investigateTimerMs = -1;
    freezeTimerMs = 0;
    chaseFlags &= ~SAM_CHASE_AFTER_DROP;
    target = g_pWolf;
    DropAllCarried();
    animTable = g_samAnimTable;
    SamView_SetTint(0, 0, 0);
    head.SamView_SetTint(0, 0, 0);
    ClearPathHistory();
}

/* 0x46361B - Sam_AddPathHistory; 183 compared bytes including any local tables. */
void Sam::AddPathHistory(unsigned char kind, short a, short b)
{
    pathHistoryIdx = (pathHistoryIdx + 1) & 15;
    pathHistory[pathHistoryIdx].type = kind;
    pathHistory[pathHistoryIdx].x = pos.x;
    pathHistory[pathHistoryIdx].z = pos.z;
    pathHistory[pathHistoryIdx].a = a;
    pathHistory[pathHistoryIdx].b = b;
}

/* 0x4636D2 - Sam_FindPathHistory; 276 compared bytes including any local tables. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
int Sam::FindPathHistory(unsigned char kind, short a)
{
    struct {
        int distance, dz, dx, divisor, k;
        SamPathHist *entry;
    } w;
    w.divisor = 1;
    if (speed < 1100)
        w.divisor = 16;
    if (mode & SAM_MODE_NODES_ONLY)
        return -1;
    for (w.k = 0; w.k < 16; ++w.k) {
        w.entry = &pathHistory[(pathHistoryIdx - w.k + 15) & 15];
        if (w.entry->type == kind) {
            w.dx = w.entry->x - pos.x;
            if (w.dx < 0)
                w.dx = -w.dx;
            w.dz = w.entry->z - pos.z;
            if (w.dz < 0)
                w.distance = w.dx - w.dz;
            else
                w.distance = w.dx + w.dz;
            if (IsDistBelowHistoryThreshold(w.distance / w.divisor) && w.entry->a == a)
                return w.k;
        }
    }
    return -1;
}

/* 0x4637E6 - Sam_FindPathHistoryExact; 233 compared bytes including any local tables. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
int Sam::FindPathHistoryExact(unsigned char kind, short a, short b)
{
    struct {
        int distance, dz, dx, k;
        SamPathHist *entry;
    } w;
    for (w.k = 0; w.k < 16; ++w.k) {
        w.entry = &pathHistory[(pathHistoryIdx - w.k + 15) & 15];
        if (w.entry->type == kind) {
            w.dx = w.entry->x - pos.x;
            if (w.dx < 0)
                w.dx = -w.dx;
            w.dz = w.entry->z - pos.z;
            if (w.dz < 0)
                w.distance = w.dx - w.dz;
            else
                w.distance = w.dx + w.dz;
            if (IsDistBelowHistoryThreshold(w.distance) && w.entry->a == a && w.entry->b == b)
                return w.k;
        }
    }
    return -1;
}

/* 0x4638CF - Sam_EnterFollowTrajectoryState; 191 compared bytes including any local tables. */
void Sam::EnterFollowTrajectoryState()
{
    alertLevel = SAM_ALERT_NONE;
    SetGoalWolf();
    jumpPhase = SAM_JUMP_NONE;
    patrolFlags &= ~SAM_PATROL_STARTED;
    investigateTimerMs = -1;
    SetYawRaw(spawnYaw);
    SetState(SAM_STT_FOLLOWTRAJECTORY);
    ResetStallAnchor();
    if (mode & SAM_MODE_GUARD_SHEEP)
        SnapshotSheepPositions();
    TrajFollower_Init(&trajFollower, trajectory, 200, 0x800, 1, 0, 50);
}

/* 0x46398E - Sam_EnterFollowTrajectoryFastState; 163 compared bytes including any local tables. */
void Sam::EnterFollowTrajectoryFastState()
{
    alertLevel = SAM_ALERT_NONE;
    SetGoalWolf();
    jumpPhase = SAM_JUMP_NONE;
    patrolFlags &= ~SAM_PATROL_STARTED;
    investigateTimerMs = -1;
    SetYawRaw(spawnYaw);
    SetState(SAM_STT_FOLLOWTRAJECTORY_FAST);
    ResetStallAnchor();
    TrajFollower_Init(&trajFollower, trajectory, 1100, 0x800, 1, 0, 50);
}

/* 0x463A31 - Sam_EnterDefaultState; 57 compared bytes including any local tables. */
void Sam::EnterDefaultState()
{
    switch (mode & SAM_MODE_BEHAVIOUR_MASK) {
        case SAM_MODE_PATROL:
            EnterFollowTrajectoryState();
            break;
        default:
            EnterLookAfterState();
            break;
    }
}

/* 0x463A6A - Sam_EnterLookAfterTimedState; 120 compared bytes including any local tables. */
void Sam::EnterLookAfterTimedState(short durationMs)
{
    short yaw;
    alertLevel = SAM_ALERT_NONE;
    SetGoalWolf();
    jumpPhase = SAM_JUMP_NONE;
    investigateTimerMs = -1;
    lookAfterTimeoutMs = durationMs;
    forcedAlert = 0;
    SetState(SAM_STT_LOOKAFTER_TIMED);
    yaw = spawnYaw;
    rot.y = yaw;
}

/* 0x463AE2 - Sam_EnterLookAfterState; 272 compared bytes including any local tables. */
void Sam::EnterLookAfterState()
{
    alertLevel = SAM_ALERT_NONE;
    SetGoalWolf();
    jumpPhase = SAM_JUMP_NONE;
    investigateTimerMs = -1;
    lookAfterTimeoutMs = -1;
    forcedAlert = 0;
    sheepGuardCooldownMs = 20000;
    SetState(SAM_STT_LOOKAFTER);
    Camera_ReleaseScripted(this);
    if (mode & SAM_MODE_LEVEL_VARIANT) {
        if (beachBits.onSide2)
            SetYawRaw((spawnYaw + 0x800) & 0xFFF);
        else
            SetYawRaw(spawnYaw);
    } else
        SetYawRaw(spawnYaw);
    if (mode & SAM_MODE_GUARD_SHEEP)
        SnapshotSheepPositions();
}

/* 0x463BF2 - Sam_EnterInvestigateState; 64 compared bytes including any local tables. */
void Sam::EnterInvestigateState()
{
    alertLevel = SAM_ALERT_NONE;
    SetGoalWolf();
    jumpPhase = SAM_JUMP_NONE;
    investigateStopDelayMs = 0;
    SetState(SAM_STT_INVESTIGATE);
}

/* 0x463C32 - Sam_EnterFollowNodeState; 442 compared bytes including any local tables. */
void Sam::EnterFollowNodeState(Vec3s *firstPos, NavNode *pathHead, int resetCooldown)
{
    NavNode *head;
    int i;
    head = pathHead;
    jumpPhase = SAM_JUMP_NONE;
    followBits.dropToNode = 0;
    waypointCount = 1;
    waypoints[0].x = firstPos->x;
    waypoints[0].z = firstPos->z;
    if (resetCooldown)
        renavCooldownMs = 0;
    while (pathHead) {
        waypoints[waypointCount].x = pathHead->x;
        waypoints[waypointCount].y = pathHead->groundY;
        waypoints[waypointCount].z = pathHead->z;
        ++waypointCount;
        pathHead = pathHead->parent;
    }
    if (waypointCount >= 3) {
        for (i = 0; i < head->nbSons; ++i) {
            if (head->sons[i] == head->parent)
                break;
        }
        followEdgeNormal = head->edgeNormals[i];
    }
    followBits.midRoute = 0;
    followBits.dropping = 0;
    Stub_46eb4c(GetYaw(), 0);
    SetState(SAM_STT_FOLLOWNODES);
    AddPathHistory(SAM_HIST_FOLLOWNODES, 0, 0);
    ResetStallAnchor();
}

/* 0x463DEC - Sam_EnterReachGoalState; 53 compared bytes including any local tables. */
void Sam::EnterReachGoalState()
{
    ResetStallAnchor();
    SetState(SAM_STT_REACHGOAL);
    jumpPhase = SAM_JUMP_NONE;
    AddPathHistory(SAM_HIST_REACHGOAL, 0, 0);
}

/* 0x463E21 - Sam_EnterSearchNearestNodeState; 454 compared bytes including any local tables. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
void Sam::EnterSearchNearestNodeState(int dx, int dz)
{
    struct {
        short angle, neighborHeading;
        int history;
    } w;
    jumpPhase = SAM_JUMP_NONE;
    if (mode & SAM_MODE_NODES_ONLY) {
        EnterFollowNodeState(&goalPos, 0, 0);
    } else {
        w.angle = Math_RadiansToAngle4096((float)atan2((double)dx, (double)dz)) & 0xFFF;
        searchHeading = (w.angle + 0x100) & ~0x1FF;
        w.history = FindPathHistory(SAM_HIST_SEARCH, searchHeading);
        if (w.history >= 0) {
            searchHeadingStep = -pathHistory[(pathHistoryIdx - w.history + 15) & 15].b;
        } else {
            w.neighborHeading =
                (w.angle & 0x200) == searchHeading ? (short)(searchHeading + 0x200) : (short)(searchHeading - 0x200);
            searchHeadingStep = searchHeading < w.neighborHeading ? 0x200 : -0x200;
        }
        searchTurnCounter = 0;
        searchBlockedMs = 0;
        SetYawRaw((searchHeading + 0x800) & 0xFFF);
        ResetStallAnchor();
        SetState(SAM_STT_SEARCHNEARESTNODE);
        AddPathHistory(SAM_HIST_SEARCH, searchHeading, searchHeadingStep);
    }
}

/* 0x463FE7 - Sam_EnterReachNodeState; 652 compared bytes including any local tables. */
void Sam::EnterReachNodeState(NavNode *node, unsigned char neighborIndex, int fromSearch)
{
    Vec3s delta;
    NavNode *neighbor;
    waypointCount = 1;
    SetState(SAM_STT_REACHNODE);
    ResetStallAnchor();
    jumpPhase = SAM_JUMP_NONE;
    if (fromSearch && node->x == lastNodeTarget.x && node->z == lastNodeTarget.z) {
        node = node->sons[neighborIndex];
    } else {
        delta.x = goalPos.x - pos.x;
        delta.y = 0;
        delta.z = goalPos.z - pos.z;
        goalDist = (int)sqrt((double)delta.x * delta.x + delta.z * delta.z);
        if (goalDist == 0)
            goalDist = 1;
        if (fromSearch) {
            neighbor = node->sons[neighborIndex];
            if (SAM_ROUTE_ABS(neighbor->x - pos.x) + SAM_ROUTE_ABS(neighbor->z - pos.z) <
                SAM_ROUTE_ABS(node->x - pos.x) + SAM_ROUTE_ABS(node->z - pos.z))
                node = neighbor;
        }
    }
    lastNodeTarget.x = node->x;
    lastNodeTarget.z = node->z;
    waypoints[0].x = node->x;
    waypoints[0].y = node->groundY;
    waypoints[0].z = node->z;
    AddPathHistory(SAM_HIST_REACHNODE, 0, 0);
}

/* 0x464273 - Sam_EnterCin01State; 201 compared bytes including any local tables. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
void Sam::EnterCin01State()
{
    struct {
        short unused10, yaw;
        int dz, dx, length;
    } w;
    w.dx = g_pWolf->pos.x - pos.x;
    w.dz = g_pWolf->pos.z - pos.z;
    w.length = (int)sqrt((double)w.dx * w.dx + w.dz * w.dz);
    if (w.length == 0)
        w.length = 1;
    SetState(SAM_STT_CIN01);
    w.yaw = (Math_RadiansToAngle4096((float)atan2((double)w.dx, (double)w.dz)) + 0x800) & 0xFFF;
    rot.y = w.yaw;
    jumpPhase = SAM_JUMP_NONE;
}

/* 0x46433C - Sam_TryCatchWolf; 526 compared bytes including any local tables. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
int Sam::TryCatchWolf()
{
    struct {
        short unused18, yaw;
        int dy, dz, sheepCostume, dx, length;
    } w;
    w.sheepCostume = g_pWolf->HandleMessage(this, MSG_WOLF_IS_SHEEPCOSTUME, 0);
    w.dx = g_pWolf->pos.x - pos.x;
    w.dz = g_pWolf->pos.z - pos.z;
    if (!g_pWolf->HandleMessage(this, MSG_WOLF_IS_CATCHABLE, 0))
        return 0;
    w.dy = g_pWolf->pos.y - pos.y;
    if (w.dy > -30 && w.dy < 50 && (w.sheepCostume || Rand_Bounded(100) < 50)) {
        /* cast kept: HandleMessage's arg is a void *; this message passes a number in it */
        if (!g_pWolf->HandleMessage(this, MSG_WOLF_CAUGHT, (void *)CAUGHT_GRAB)) {
            freezeTimerMs = 5000;
            return 0;
        }
        PlayAnim(w.sheepCostume ? SAM_ANIM_HIT_ROBOT : SAM_ANIM_GRAB, 0, 1, 0x1000);
    } else if (!w.sheepCostume && w.dy > -150 && w.dy < 100) {
        /* cast kept: HandleMessage's arg is a void *; this message passes a number in it */
        if (!g_pWolf->HandleMessage(this, MSG_WOLF_CAUGHT, (void *)CAUGHT_HIT)) {
            freezeTimerMs = 5000;
            return 0;
        }
        PlayAnim(SAM_ANIM_HIT, 0, 1, 0x1000);
    } else
        return 0;
    w.length = (int)sqrt((double)w.dx * w.dx + w.dz * w.dz);
    if (w.length == 0)
        w.length = 1;
    SetState(SAM_STT_CATCHWOLF);
    alertLevel = SAM_ALERT_RED;
    w.yaw = (Math_RadiansToAngle4096((float)atan2((double)w.dx, (double)w.dz)) + 0x800) & 0xFFF;
    rot.y = w.yaw;
    jumpPhase = SAM_JUMP_NONE;
    return 1;
}

/* 0x46454A - Sam_KickRobot; 452 compared bytes including any local tables. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
int Sam::KickRobot(int force)
{
    struct {
        short yaw, anim;
        int property, propertyOffset;
        unsigned char *record;
        int dy, dz, dx, length;
    } w;
    w.dx = robot->pos.x - pos.x;
    w.dz = robot->pos.z - pos.z;
    if (force == 0) {
        w.dy = robot->pos.y - pos.y;
        if (w.dy > -80 && w.dy < 80) {
            /* cast kept: HandleMessage's arg is a void *; this message passes a number in it */
            if (!robot->HandleMessage(this, MSG_KILL, (void *)KILL_GENERIC))
                return 0;
        } else
            return 0;
        /* cast kept (both): designer properties are 4-byte slots at byte offsets of the raw WAR record */
        w.record = (unsigned char *)this->record;
        w.propertyOffset = 0x50;
        w.property = *(int *)(w.record + w.propertyOffset + 20);
        actionTimerMs = w.property;
    } else {
        /* cast kept: HandleMessage's arg is a void *; this message passes a number in it */
        robot->HandleMessage(this, MSG_KILL, (void *)KILL_GENERIC);
        w.anim = GetAnimId(SAM_ANIM_HIT_ROBOT);
        actionTimerMs = Anim_GetDurationMs(Inst(), w.anim, 0);
    }
    w.length = (int)sqrt((double)w.dx * w.dx + w.dz * w.dz);
    if (w.length == 0)
        w.length = 1;
    SetState(SAM_STT_HITROBOT);
    PlayAnim(SAM_ANIM_HIT_ROBOT, 0, 0, 0x1000);
    w.yaw = (Math_RadiansToAngle4096((float)atan2((double)w.dx, (double)w.dz)) + 0x800) & 0xFFF;
    rot.y = w.yaw;
    jumpPhase = SAM_JUMP_NONE;
    return 1;
}

/* 0x46470E - Sam_SetState; 1069 compared bytes including any local tables. */
void Sam::SetState(unsigned char state)
{
    short playback;
    int running;
    running = 0;
    sheepMovedAccum = 0;
    this->state = state;
    stateTimerMs = 0;
    headYaw = 0;
    playback = chaseBits.chaseAfterDrop ? 0x3000 : 0x1000;
    if ((mode & (SAM_MODE_LEVEL_VARIANT | SAM_MODE_ALT_BEHAVIOUR)) && goalKind != SAM_GOAL_TARGET) {
        switch (this->state) {
            case SAM_STT_FOLLOWNODES:
            case SAM_STT_REACHGOAL:
            case SAM_STT_SEARCHNEARESTNODE:
                if ((mode & SAM_MODE_ALT_BEHAVIOUR) && goalKind == SAM_GOAL_HOME &&
                    Vec3s_ManhattanDistXZ(&targetPos, &pos) >= 400) {
                    PlayAnim(SAM_ANIM_RUN, 1, 1, 0x1000);
                    running = 1;
                } else
                    PlayAnim(SAM_ANIM_WALK, 1, 1, playback);
                break;
        }
    } else if (goalKind != SAM_GOAL_TARGET) {
        switch (this->state) {
            case SAM_STT_FOLLOWNODES:
            case SAM_STT_REACHGOAL:
            case SAM_STT_SEARCHNEARESTNODE:
                if (animTable == g_samAnimTableCarryOne || animTable == g_samAnimTableCarryTwo)
                    playback = 0x4000;
                else
                    playback = 0x1000;
                PlayAnim(SAM_ANIM_RUN, 1, 1, playback);
                running = 1;
                break;
        }
    } else {
        switch (this->state) {
            case SAM_STT_FOLLOWNODES:
            case SAM_STT_REACHGOAL:
            case SAM_STT_SEARCHNEARESTNODE:
                if (CurrentAnim() != GetAnimId(SAM_ANIM_RUN_LOOP) && CurrentAnim() != GetAnimId(SAM_ANIM_RUN_START))
                    PlayAnim(SAM_ANIM_RUN_START, 0, 1, 0x1000);
                running = 1;
                break;
        }
    }
    switch (this->state) {
        case SAM_STT_ANGRY:
            PlayAnim(SAM_ANIM_ANGRY, 0, 1, 0x1000);
            StartReactionCamera();
            break;
        case SAM_STT_WATCHROBOT:
            PlayAnim(SAM_ANIM_ANGRY, 1, 1, 0x1000);
            break;
        case SAM_STT_DISCOVERWOLF:
            PlayAnim(SAM_ANIM_WALK, 0, 1, 0x1000);
            break;
        case SAM_STT_IDLE:
            PlayAnim(SAM_ANIM_IDLE, 1, 1, 0x1000);
            break;
        case SAM_STT_LOOKAFTER:
        case SAM_STT_LOOKAFTER_TIMED:
            PlayAnim(SAM_ANIM_LOOKAFTER, 1, 1, 0x1000);
            break;
        case SAM_STT_INVESTIGATE:
            PlayAnim(SAM_ANIM_INVESTIGATE_START, 0, 0, 0x1000);
            break;
        case SAM_STT_FOLLOWTRAJECTORY:
            PlayAnim(SAM_ANIM_WALK, 1, 1, 0x1000);
            break;
        case SAM_STT_FOLLOWTRAJECTORY_FAST:
            PlayAnim(SAM_ANIM_PATROL_FAST, 1, 1, 0x1000);
            break;
        case SAM_STT_PICKUP:
            PlayAnim(SAM_ANIM_PICKUP, 0, 1, 0x1000);
            break;
        case SAM_STT_LIFT:
            PlayAnim(SAM_ANIM_LIFT, 0, 1, 0x1000);
            break;
        case SAM_STT_PUTDOWN:
            PlayAnim(SAM_ANIM_PUTDOWN, 0, 1, playback);
            break;
        case SAM_STT_AFTERPUTDOWN:
            PlayAnim(SAM_ANIM_AFTERPUTDOWN, 0, 1, playback);
            break;
        case SAM_STT_CATCHDONE:
            PlayAnim(SAM_ANIM_CATCHDONE, 0, 1, 0x1000);
            break;
    }
    if (running)
        speed = (animTable != g_samAnimTableCarryOne && animTable != g_samAnimTableCarryTwo) ? 1100 : 1100;
    if ((mode & (SAM_MODE_LEVEL_VARIANT | SAM_MODE_ALT_BEHAVIOUR)) && goalKind != SAM_GOAL_TARGET)
        speed = 200;
    if (chaseBits.chaseAfterDrop)
        speed = 1100;
}

/* 0x464B3B - Sam_ResetStallAnchor; 74 compared bytes including any local tables. */
void Sam::ResetStallAnchor()
{
    stallClockMs = (unsigned short)((g_dt * 1000) >> 12);
    stallAnchor = pos;
    stallBestDist = 0;
}

/* 0x464B85 - Sam_UpdateStallTracker; 108 compared bytes including any local tables. */
void Sam::UpdateStallTracker()
{
    int distance;
    stallClockMs += (unsigned short)((g_dt * 1000) >> 12);
    distance = Vec3s_ManhattanDistXZ(&pos, &stallAnchor);
    if (distance > stallBestDist)
        stallBestDist = distance;
}

/* 0x464BF1 - Sam_CollectMovedSheep; 220 compared bytes including any local tables. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
int Sam::CollectMovedSheep(ScnObject **out)
{
    struct {
        int index, count;
        ScnObject *object;
    } w;
    w.count = 0;
    for (w.index = frameCounter; w.index < trackedCount; w.index += 8) {
        w.object = trackedObjs[w.index].object;
        if (w.object->pos.x != trackedObjs[w.index].x || w.object->pos.z != trackedObjs[w.index].z) {
            trackedObjs[w.index].x = w.object->pos.x;
            trackedObjs[w.index].z = w.object->pos.z;
            if (!w.object->SamView_HasInstanceFlag(INST_F_ATTACHED)) {
                out[w.count] = w.object;
                w.count++;
            }
        }
    }
    return w.count;
}

/* 0x464CCD - Sam_SnapshotSheepPositions; 224 compared bytes including any local tables. */
void Sam::SnapshotSheepPositions()
{
    /* The original reserves 50 pointer slots but asks for at most 24 sheep.
     * Ralph occupies slot zero, with the class lookup filling from slot one. */
    ScnObject *objects[50];
    int index;
    objects[0] = g_pWolf;
    trackedCount = Scenaric_FindByClass(CLASSID_SHEEP, objects + 1, 24) + 1;
    for (index = 0; index < trackedCount; index++) {
        trackedObjs[index].object = objects[index];
        trackedObjs[index].x = objects[index]->pos.x;
        trackedObjs[index].z = objects[index]->pos.z;
    }
}

/* 0x464DAD - Sam_DetectWolf; 3379 compared bytes including any local tables. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; unused20 fill gaps */
void Sam::DetectWolf(Vec3s *wolfPos, short halfAngle)
{
    struct {
        SamFetchGoal selection;
        int authorized, acceptableClass, inCone, outsideNoCatch, collectible;
        int fallbackIndex, boxIndex;
        ScnObject *boxObjects[64];
        int boxCount;
        short unusedA4, relativeAngle;
        ScnObject *chosen[3];
        int index;
        ScnObject *candidates[27];
        short unused24, heading;
        unsigned char unused20;
        signed char chosenCount;
        short nearestDistance;
        ScnObject *object;
        int candidateCount;
        ScnObject *partner;
        int robotDy;
        ScnObject *firstSheep;
        int disguised, sheepCostume;
    } w;
    w.disguised = 0;
    w.sheepCostume = 0;
    catapultLoadedObj = 0;
    if (g_pWolf->HandleMessage(this, MSG_WOLF_IS_DEAD, 0))
        return;
    w.disguised = g_pWolf->HandleMessage(this, MSG_WOLF_IS_SHEEPCOSTUME, 0);
    w.sheepCostume = w.disguised;
    w.disguised |= g_pWolf->HandleMessage(this, MSG_WOLF_IS_RABBITCOSTUME, 0);
    if ((mode & SAM_MODE_ALT_BEHAVIOUR) && sheepBits.heardCall) {
        w.firstSheep = 0;
        Scenaric_FindByClass(CLASSID_SHEEP, &w.firstSheep, 1);
        if (w.firstSheep && PointInBoxListXZ(&g_pWolf->pos, orangeZoneBoxes, orangeZoneCount) &&
            Vec3s_DistSqXZ(&w.firstSheep->pos, &g_pWolf->pos) < 0x9C432) {
            w.disguised = 0;
            relAngleToWolf = 0;
        }
    }
    if (robot && Vec3s_ManhattanDistXZ(&robot->pos, &pos) < 100 && (w.robotDy = robot->pos.y - pos.y) > -80 &&
        w.robotDy < 80 && KickRobot(1)) {
        alertLevel = SAM_ALERT_RED;
        return;
    }
    if (forcedAlert || (alertLevel == SAM_ALERT_ORANGE && !w.disguised &&
                        ((frameCounter == 0 && g_pWolf->HandleMessage(this, MSG_WOLF_IS_NOISY, 0)) || goalDist < 100 ||
                         (SAM_ROUTE_ABS(relAngleToWolf) < halfAngle &&
                          g_pWolf->HandleMessage(this, MSG_WOLF_IS_VISIBLE, 0) && !IsWolfBehindHidingBox())))) {
        SetGoalWolf();
        if (!IsWolfOutsideHitZone(1, 1)) {
            alertLevel = SAM_ALERT_RED;
            StartSearchTowardGoal(&goalDx, &goalDz, &goalDist);
        }
        return;
    }
    if ((mode & SAM_MODE_GUARD_SHEEP) && catchableSheepBox) {
        w.chosen[0] = 0;
        w.chosen[1] = 0;
        w.chosen[2] = 0;
        w.chosenCount = 0;
        if (mode & SAM_MODE_LEVEL_VARIANT) {
            w.boxCount = ObjGrid_QueryBoxOverlap(GetBeachCurrentBox(), w.boxObjects);
            for (w.boxIndex = 0; w.boxIndex < w.boxCount; w.boxIndex++) {
                if (w.boxObjects[w.boxIndex]->SamView_GetClassId() == CLASSID_SHEEP) {
                    w.chosen[w.chosenCount] = w.boxObjects[w.boxIndex];
                    w.chosenCount++;
                }
            }
            if (w.chosenCount < 3) {
                for (w.boxIndex = 0; w.boxIndex < w.boxCount; w.boxIndex++) {
                    if (w.boxObjects[w.boxIndex]->SamView_GetClassId() == CLASSID_INFLATABLESHEEP &&
                        w.boxObjects[w.boxIndex]->HandleMessage(this, MSG_INFLATABLE_QUERY, 0)) {
                        w.chosen[w.chosenCount] = w.boxObjects[w.boxIndex];
                        w.chosenCount++;
                        break;
                    }
                }
            }
            if (w.chosenCount == 3)
                sheepMovedAccum += g_dt;
            else
                sheepMovedAccum = 0;
            if (w.chosenCount != 2) {
                if (sheepGuardCooldownMs < 2000)
                    sheepGuardCooldownMs = 2000;
                if (w.chosenCount == 3 && sheepMovedAccum < 0x3000) {
                    /* The original retains a separate branch for this delay. */
                } else {
                    SetGoalWolf();
                    if (!IsWolfOutsideHitZone(1, 1)) {
                        alertLevel = SAM_ALERT_RED;
                        StartSearchTowardGoal(&goalDx, &goalDz, &goalDist);
                        return;
                    }
                }
            }
            if (sheepGuardCooldownMs > 0) {
                w.chosenCount = 0;
                sheepGuardCooldownMs -= (short)g_dtMs;
            }
        } else {
            w.candidateCount = CollectMovedSheep(w.candidates);
            if ((mode & (SAM_MODE_PATROL | SAM_MODE_ALT_BEHAVIOUR)) && w.candidateCount == 0) {
                for (w.fallbackIndex = 0; w.fallbackIndex < trackedCount && w.candidateCount < 2; w.fallbackIndex++) {
                    w.object = trackedObjs[w.fallbackIndex].object;
                    if (!w.object->SamView_HasInstanceFlag(INST_F_ATTACHED)) {
                        w.candidates[w.candidateCount] = w.object;
                        w.candidateCount++;
                    }
                }
            }
            for (w.index = 0; w.index < w.candidateCount; w.index++) {
                w.heading = (Math_RadiansToAngle4096((float)atan2((double)w.candidates[w.index]->pos.x - pos.x,
                                                                  (double)w.candidates[w.index]->pos.z - pos.z)) +
                             0x800) &
                            0xFFF;
                w.relativeAngle = (short)((w.heading - (GetYaw() + headYaw) + 0x800) & 0xFFF) - 0x800;
                w.object = w.candidates[w.index];
                w.inCone = SAM_ROUTE_ABS(w.relativeAngle) < halfAngle + 0x92;
                if ((mode & SAM_MODE_ALT_BEHAVIOUR) && this->state != SAM_STT_FOLLOWTRAJECTORY) {
                    w.collectible = !w.object->SamView_HasInstanceFlag(INST_F_ATTACHED) &&
                                    !PointInBoxListXZ(&w.object->pos, orangeZoneBoxes, orangeZoneCount);
                    if (w.object->SamView_GetClassId() == CLASSID_SHEEP)
                        w.inCone = 1;
                } else {
                    if (w.object->SamView_HasInstanceFlag(INST_F_ATTACHED))
                        w.collectible = 0;
                    else
                        w.collectible = SamView_ContainsXYZ(catchableSheepBox, &w.object->pos);
                }
                w.outsideNoCatch =
                    dontTryToCatchSheepBox && !SamView_ContainsXZ(dontTryToCatchSheepBox, &w.object->pos);
                w.acceptableClass = w.object->SamView_GetClassId() == CLASSID_SHEEP ||
                                    (w.object->SamView_GetClassId() == CLASSID_WOLF && w.sheepCostume);
                w.authorized = PointInBoxListXZ(&w.object->pos, authorizedZoneBoxes, authorizedZoneCount);
                if (w.collectible && w.outsideNoCatch && w.inCone && w.acceptableClass && w.authorized) {
                    w.chosen[w.chosenCount] = w.candidates[w.index];
                    w.chosenCount++;
                    if (w.chosenCount == 2)
                        break;
                    w.partner = Scenaric_FindNearestOfClass(&w.candidates[w.index]->pos,
                                                            w.candidates[w.index]->SamView_GetClassId() ? 0 : 11,
                                                            -32000, 32000, 250, &w.nearestDistance, 0);
                    if (!w.partner)
                        continue;
                    if (w.partner->SamView_GetClassId() == CLASSID_WOLF && !w.sheepCostume)
                        continue;
                    w.chosen[w.chosenCount] = w.partner;
                    w.chosenCount++;
                    break;
                }
            }
            if (sheepBits.heardCall) {
                w.chosenCount = 1;
                w.chosen[0] = g_pWolf;
                sheepBits.heardCall = 0;
            }
        }
        if (w.chosenCount) {
            if (w.chosenCount == 2 && w.chosen[1] == g_pWolf) {
                w.chosen[1] = w.chosen[0];
                w.chosen[0] = g_pWolf;
            }
            w.selection.object1 = w.chosen[0];
            w.selection.object2 = w.chosen[1];
            SetGoalFetch(&w.selection);
            StartSearchTowardGoal(&goalDx, &goalDz, &goalDist);
        }
    }
}

/* 0x465AE0 - Sam_StartSearchTowardGoal; 172 compared bytes including any local tables. */
void Sam::StartSearchTowardGoal(int *dxOut, int *dzOut, int *distanceOut)
{
    ClearPathHistory();
    *dxOut = goalPos.x - pos.x;
    *dzOut = goalPos.z - pos.z;
    *distanceOut = (int)sqrt((double)*dxOut * *dxOut + *dzOut * *dzOut);
    EnterSearchNearestNodeState(goalPos.x - pos.x, goalPos.z - pos.z);
}

/* 0x465B8C - Sam_DetectWolfPatrol; 283 compared bytes including any local tables. */
void Sam::DetectWolfPatrol(Vec3s *wolfPos, short halfAngle)
{
    if (mode & SAM_MODE_ALT_BEHAVIOUR) {
        DetectWolf(wolfPos, halfAngle);
        return;
    }
    if ((frameCounter == 0 && g_pWolf->HandleMessage(this, MSG_WOLF_IS_NOISY, 0) && goalDist < 400) || goalDist < 100 ||
        (goalDist < distCatchPatrol && SAM_ROUTE_ABS(relAngleToWolf) < halfAngle &&
         g_pWolf->HandleMessage(this, MSG_WOLF_IS_VISIBLE, 0))) {
        SetGoalWolf();
        StartSearchTowardGoal(&goalDx, &goalDz, &goalDist);
    }
}

/* 0x465CA7 - Sam_NoticesWolfWhileBusy; 1317 compared bytes including any local tables. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
/* BYTES(flow, inferred): if (1) with a dead else: the original tests a constant here */
int Sam::NoticesWolfWhileBusy(int kind)
{
    struct {
        int fetchWasOrange, carryWasOrange, disguised, inside;
    } w;
    if (!g_pWolf->HandleMessage(this, MSG_WOLF_IS_CATCHABLE, 0))
        return 0;
    switch (kind) {
        case SAM_GOAL_CARRY:
            if (!chaseBits.chaseAfterDrop) {
                w.carryWasOrange = alertLevel == SAM_ALERT_ORANGE;
                w.inside = PointInBoxListXZ(&g_pWolf->pos, orangeZoneBoxes, orangeZoneCount);
                w.disguised = g_pWolf->HandleMessage(this, MSG_WOLF_IS_SHEEPCOSTUME, 0);
                w.disguised |= g_pWolf->HandleMessage(this, MSG_WOLF_IS_RABBITCOSTUME, 0);
                if (!w.disguised && w.carryWasOrange) {
                    if (frameCounter == 0 && g_pWolf->HandleMessage(this, MSG_WOLF_IS_NOISY, 0))
                        w.inside = 1;
                    else if (SAM_ROUTE_ABS(relAngleToWolf) < 0x71 &&
                             g_pWolf->HandleMessage(this, MSG_WOLF_IS_VISIBLE, 0))
                        w.inside = 1;
                    else
                        w.disguised = 1;
                }
                return !w.disguised && w.inside ? 1 : 0;
            } else
                return 0;
        case SAM_GOAL_FETCH:
            w.fetchWasOrange = alertLevel == SAM_ALERT_ORANGE;
            w.inside = PointInBoxListXZ(&g_pWolf->pos, orangeZoneBoxes, orangeZoneCount);
            w.disguised = g_pWolf->HandleMessage(this, MSG_WOLF_IS_SHEEPCOSTUME, 0);
            w.disguised |= g_pWolf->HandleMessage(this, MSG_WOLF_IS_RABBITCOSTUME, 0);
            if (!w.disguised && w.fetchWasOrange) {
                if (frameCounter == 0 && g_pWolf->HandleMessage(this, MSG_WOLF_IS_NOISY, 0))
                    w.inside = 1;
                else if (SAM_ROUTE_ABS(relAngleToWolf) < 0x71 && g_pWolf->HandleMessage(this, MSG_WOLF_IS_VISIBLE, 0))
                    w.inside = 1;
                else
                    w.disguised = 1;
            }
            if (1)
                return !w.disguised && w.inside ? 1 : 0;
            else
                return !fetchGoal.flags.bits.picked1 && !fetchGoal.flags.bits.picked2 && !w.disguised && w.inside ? 1
                                                                                                                  : 0;
        case SAM_GOAL_HOME:
            if (g_pWolf->HandleMessage(this, MSG_WOLF_IS_DEAD, 0))
                return 0;
            if (chaseBits.chaseAfterDrop) {
                chaseBits.chaseAfterDrop = 0;
                return 1;
            }
            w.inside = PointInBoxListXZ(&g_pWolf->pos, orangeZoneBoxes, orangeZoneCount);
            w.disguised = g_pWolf->HandleMessage(this, MSG_WOLF_IS_SHEEPCOSTUME, 0);
            w.disguised |= g_pWolf->HandleMessage(this, MSG_WOLF_IS_RABBITCOSTUME, 0);
            if (!w.disguised && (alertLevel == SAM_ALERT_ORANGE || Vec3s_ManhattanDistXZ(&g_pWolf->pos, &pos) < 150)) {
                if (frameCounter == 0 && g_pWolf->HandleMessage(this, MSG_WOLF_IS_NOISY, 0))
                    w.inside = 1;
                else if (SAM_ROUTE_ABS(relAngleToWolf) < 0x71 && g_pWolf->HandleMessage(this, MSG_WOLF_IS_VISIBLE, 0))
                    w.inside = 1;
                else
                    w.disguised = 1;
            }
            return w.inside && !w.disguised ? 1 : 0;
    }
    return 0;
}

/* 0x4661CC - Sam_CheckStepUpAhead; 466 compared bytes including any local tables. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
unsigned char Sam::CheckStepUpAhead(Vec3s *end, short firstHeight)
{
    struct {
        ModelBoxList *list;
        int firstGap, height;
        unsigned char unused1C[3], result;
        CollBox box;
        int secondGap;
        CollBox *source;
    } w;
    if (frameCounter != 4 || (mode & SAM_MODE_NO_JUMP))
        return 0;
    w.list = inst_model->boxes;
    if (w.list)
        w.source = w.list->boxes;
    else
        w.source = 0;
    w.firstGap = pos.y - firstHeight;
    if (w.firstGap <= 25 || FindPathHistory(SAM_HIST_JUMP, 0) >= 0 || w.firstGap >= 350)
        return 0;
    w.box.flags = w.source->flags;
    w.box.min.x = (pos.x < end->x ? pos.x : end->x) - 150;
    w.box.min.z = (pos.z < end->z ? pos.z : end->z) - 150;
    w.box.min.y = -3000;
    w.box.max.x = (pos.x > end->x ? pos.x : end->x) + 150;
    w.box.max.z = (pos.z > end->z ? pos.z : end->z) + 150;
    w.box.max.y = 3000;
    if (!Coll_BoxGroundQuery(&w.box, &w.height, this, CQ_STATIC | CQ_OBJECTS, 0))
        return 0;
    w.secondGap = pos.y - w.height;
    w.result = (w.secondGap > 25 && w.secondGap < 350) ? 1 : 0;
    return w.result;
}

#define SAMU_START_SEARCH() StartSearchTowardGoal(&goalDx, &goalDz, &goalDist)
#define SAMU_ADD_POS(out, delta) \
    (out).x = pos.x + (delta).x; \
    (out).y = pos.y + (delta).y; \
    (out).z = pos.z + (delta).z
#define SAMU_CONTAINS3(box, point)                                                                  \
    ((point)->x >= (box)->min.x && (point)->x <= (box)->max.x && (point)->y >= (box)->min.y &&      \
             (point)->y <= (box)->max.y && (point)->z >= (box)->min.z && (point)->z <= (box)->max.z \
         ? 1                                                                                        \
         : 0)
#define SAMU_CONTAINS2(box, point)                                                             \
    ((point)->x >= (box)->min.x && (point)->x <= (box)->max.x && (point)->z >= (box)->min.z && \
             (point)->z <= (box)->max.z                                                        \
         ? 1                                                                                   \
         : 0)
/* 0x46639E - Sam_Update; 20480 compared bytes including any local tables. */
/* BYTES(slot-group): Sam::Update's locals are one work record only to pin the 0x4d8-byte frame (offsets per member) */
/* BYTES(flow): the original's inline expansion with constant loop / blend arguments */
void Sam::Update()
{
    SamUpdateWork w;
    w.selfPos = &pos;
    frameToggle = !frameToggle;
    UpdateGoalPos();
    frameCounter = (frameCounter + 1) & 7;
    w.wolfDx = g_pWolf->pos.x - w.selfPos->x;
    w.wolfDz = g_pWolf->pos.z - w.selfPos->z;
    yawToWolf = (Math_RadiansToAngle4096((float)atan2((double)w.wolfDx, (double)w.wolfDz)) + 0x800) & 0xFFF;
    w.dt = (g_dt * 1000) >> 12;
    stateTimerMs += w.dt;
    lifeTimerMs += w.dt;
    if (HasAnimFlags(ANIM_F_FINISHED)) {
        w.animation436 = anim.animId;
        if (w.animation436 == GetAnimId(SAM_ANIM_RUN_START))
            PlayAnim(SAM_ANIM_RUN_LOOP, 1, 1, 0x1000);
    }
    AdvanceAnim();
    w.soundPlaying = chaseSoundHandle && (w.soundHandle444 = chaseSoundHandle, Sound_IsPlaying(w.soundHandle444));
    w.runningAnimation = (w.animation446 = anim.animId, w.animation446 == ASAMSH01_ANIM_START) ||
                         (w.animation448 = anim.animId, w.animation448 == ASAMSH01_ANIM_RUN1);
    if (w.soundPlaying && !w.runningAnimation) {
        w.soundHandle44a = chaseSoundHandle;
        Sound_Stop(w.soundHandle44a, this);
        chaseSoundHandle = 0;
    } else if (w.runningAnimation) {
        if (!w.soundPlaying) {
            chaseSoundBits.volume = 1;
            w.initialVolume = chaseSoundBits.volume;
            chaseSoundHandle =
                Sound_Play(SND_SGORUN, this, w.initialVolume, SNDF_LOOP | SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 0x1000);
        } else {
            if (chaseSoundBits.volume != 255) {
                w.soundVolume = chaseSoundBits.volume + ((g_dtMs * 255) >> 10) + 1;
                if (w.soundVolume > 255)
                    w.soundVolume = 255;
            } else
                w.soundVolume = chaseSoundBits.volume;
            w.soundHandle44e = chaseSoundHandle;
            Sound_SetVolume(w.soundHandle44e, w.soundVolume);
            chaseSoundBits.volume = w.soundVolume;
        }
    }
    if (restartStatePending) {
        w.cineActive = g_cinePlayer.active;
        if (w.cineActive)
            return;
        SetState(this->state);
        restartStatePending = 0;
    }
    UpdateZoneState();
    if (this->state != SAM_STT_LOOKAFTER_TIMED && this->state != SAM_STT_KNOCKEDOUT)
        DrawZoneIndicator();
    if (SAM_ROUTE_ABS(w.wolfDx) + SAM_ROUTE_ABS(w.wolfDz) < 50)
        relAngleToWolf = 0;
    else {
        w.currentYaw = rot.y;
        relAngleToWolf = (short)((yawToWolf - (w.currentYaw + headYaw) + 0x800) & 0xFFF) - 0x800;
    }
    w.goalDzOut = &goalDz;
    w.goalDxOut = &goalDx;
    *w.goalDxOut = goalPos.x - pos.x;
    *w.goalDzOut = goalPos.z - pos.z;
    goalDist = (int)sqrt((double)*w.goalDxOut * *w.goalDxOut + *w.goalDzOut * *w.goalDzOut);
    if (freezeTimerMs > 0) {
        freezeTimerMs -= (short)g_dtMs;
        return;
    }
    if (mode & SAM_MODE_CHASE_CAMERA) {
        w.wolfCameraQuery = g_pWolf->HandleMessage(this, MSG_WOLF_IS_DEAD, 0);
        if (alertLevel == SAM_ALERT_RED &&
            !(g_camMode == CAM_SCRIPT_BLEND_IN || g_camMode == CAM_SCRIPT_TO_SCRIPT || g_camMode == CAM_SCRIPTED ? 1
                                                                                                                 : 0) &&
            w.wolfCameraQuery == 0) {
            if (g_camMode != CAM_SAM_CHASE) {
                w.cameraRecord = this->record;
                /* cast kept: Box and CollBox are two views of one 16-byte record; the getter returns the Box view */
                w.cameraBox = (CollBox *)Scn_GetPropBox(w.cameraRecord, 0);
                if (w.cameraBox)
                    Camera_SetSamChase(&pos, w.cameraBox->max.y, w.cameraBox);
                else
                    Camera_SetSamChase(&pos, -600, 0);
            } else
                g_camChaseTarget2 = pos;
        } else if (alertLevel != SAM_ALERT_RED || w.wolfCameraQuery != 0) {
            if (!(g_camMode == CAM_SCRIPT_BLEND_IN || g_camMode == CAM_SCRIPT_TO_SCRIPT || g_camMode == CAM_SCRIPTED
                      ? 1
                      : 0) &&
                g_camMode == CAM_SAM_CHASE)
                Camera_ReleaseAny();
        }
    }
    switch (this->state) {
        case SAM_STT_FOLLOWTRAJECTORY:
            if (target == g_pWolf && jumpPhase == SAM_JUMP_NONE && goalKind == SAM_GOAL_TARGET &&
                Vec3s_ManhattanDistXZ(&targetPos, &pos) < 150)
                TryCatchWolf();
        case SAM_STT_IDLE:
        case SAM_STT_LOOKAFTER_TIMED:
        case SAM_STT_CATCHWOLF:
        case SAM_STT_INVESTIGATE:
        case SAM_STT_CIN01:
        case SAM_STT_KNOCKEDOUT:
        case SAM_STT_PICKUP:
        case SAM_STT_LIFT:
        case SAM_STT_PUTDOWN:
        case SAM_STT_AFTERPUTDOWN:
        case SAM_STT_CATCHDONE:
        case SAM_STT_FOLLOWTRAJECTORY_FAST:
        case SAM_STT_HITROBOT:
        case SAM_STT_DISCOVERWOLF:
        case SAM_STT_ANGRY:
        case SAM_STT_WATCHROBOT:
            break;
        default:
            if (this->state == SAM_STT_REACHGOAL && (mode & SAM_MODE_ALT_BEHAVIOUR) && target == g_pWolf &&
                jumpPhase == SAM_JUMP_NONE && goalKind == SAM_GOAL_TARGET &&
                Vec3s_ManhattanDistXZ(&targetPos, &pos) < 150) {
                TryCatchWolf();
                break;
            }
            if ((mode & SAM_MODE_ALT_BEHAVIOUR) && !(mode & SAM_MODE_PATROL) && alertLevel != SAM_ALERT_RED &&
                goalKind == SAM_GOAL_TARGET) {
                Scenaric_FindByClass(CLASSID_SHEEP, &w.firstSheep, 1);
                if (Vec3s_ManhattanDistXZ(&w.firstSheep->pos, &firstSheepSpawnPos) > 20 &&
                    !IsWolfOutsideHitZone(1, 0)) {
                    SetGoalWolf();
                    alertLevel = SAM_ALERT_RED;
                    SAMU_START_SEARCH();
                    return;
                }
            }
            if (this->state == SAM_STT_LOOKAFTER)
                break;
            if (jumpPhase != SAM_JUMP_NONE) {
                UpdateStallTracker();
                UpdateJump();
                return;
            }
            if (this->state == SAM_STT_FOLLOWNODES && followBits.dropToNode)
                break;
            if (frameCounter == 1 && goalKind == SAM_GOAL_TARGET && g_pWolf->HandleMessage(this, MSG_WOLF_IS_DEAD, 0)) {
                SetGoalHome();
                SAMU_START_SEARCH();
            }
            if ((mode & SAM_MODE_ALT_BEHAVIOUR) && catapultLoadedObj == g_pWolf) {
                catapultLoadedObj = 0;
                if (animTable == g_samAnimTable && goalKind == SAM_GOAL_FETCH &&
                    (fetchGoal.object1 == g_pWolf || fetchGoal.object2 == g_pWolf)) {
                    SetGoalHome();
                    SetState(SAM_STT_DISCOVERWOLF);
                    return;
                }
            }
            if ((mode & SAM_MODE_ALT_BEHAVIOUR) && goalKind == SAM_GOAL_HOME) {
                if (Vec3s_ManhattanDistXZ(&targetPos, &pos) < 400 && !chaseBits.chaseAfterDrop) {
                    if (speed != 200) {
                        speed = 200;
                        PlayAnim(SAM_ANIM_WALK, 1, 1, 0x1000);
                    }
                } else if (speed != 1100) {
                    PlayAnim(SAM_ANIM_RUN, 1, 1, 0x1000);
                    speed = 1100;
                }
            }
            if ((mode & SAM_MODE_ALT_BEHAVIOUR) && speed <= 200 && goalKind == SAM_GOAL_HOME)
                w.reachDistance = 30;
            else
                w.reachDistance = 150;
            if (Vec3s_ManhattanDistXZ(&targetPos, &pos) < w.reachDistance) {
                if (goalKind == SAM_GOAL_TARGET) {
                    if (target == robot) {
                        if (mode & SAM_MODE_WATCH_ROBOT) {
                            /* cast kept (both): designer properties are 4-byte slots at byte offsets of the record */
                            w.watchRecord = (unsigned char *)this->record;
                            w.watchProperty = 0x50;
                            w.watchDuration = *(int *)(w.watchRecord + w.watchProperty + 0x14);
                            actionTimerMs = w.watchDuration;
                            robotWatchPos = robot->pos;
                            SetState(SAM_STT_WATCHROBOT);
                            return;
                        }
                        if (KickRobot(0)) {
                            OnGoalReached(1, 0);
                            return;
                        }
                    } else if (TryCatchWolf()) {
                        OnGoalReached(1, 0);
                        return;
                    }
                } else {
                    OnGoalReached(1, 0);
                    if ((mode & SAM_MODE_ALT_BEHAVIOUR) && (mode & SAM_MODE_BEHAVIOUR_MASK) == SAM_MODE_PATROL) {
                        EnterFollowTrajectoryState();
                        mode &= ~SAM_MODE_BEHAVIOUR_MASK;
                        /* cast kept (both): the scenaric record's position sits at +4 of the raw WAR record */
                        w.homeRecord = (unsigned char *)this->record;
                        homePos = *(Vec3s *)(w.homeRecord + 4);
                        return;
                    }
                }
            } else {
                if (NoticesWolfWhileBusy(goalKind)) {
                    w.hitRegion = PointInBoxListXYZ(&g_pWolf->pos, wolfCanBeHitZoneBoxes, wolfCanBeHitZoneCount);
                    if (robot)
                        w.hitRegion |= PointInBoxListXYZ(&robot->pos, wolfCanBeHitZoneBoxes, wolfCanBeHitZoneCount);
                    if (w.hitRegion) {
                        if (goalKind == SAM_GOAL_FETCH && fetchGoal.flags.bits.picked1) {
                            OnGoalReached(0, 1);
                            SAMU_START_SEARCH();
                        } else if (goalKind == SAM_GOAL_CARRY) {
                            OnGoalReached(0, 1);
                            SAMU_START_SEARCH();
                        } else {
                            SetGoalWolf();
                            SAMU_START_SEARCH();
                        }
                        return;
                    }
                }
                if (goalKind == SAM_GOAL_CARRY && Vec3s_ManhattanDistXZ(&g_pWolf->pos, &pos) < 150 &&
                    carryGoal.object1 != g_pWolf && carryGoal.object2 != g_pWolf &&
                    !g_pWolf->HandleMessage(this, MSG_WOLF_IS_SHEEPCOSTUME, 0) && TryCatchWolf()) {
                    PutDownCarried(0);
                    animTable = g_samAnimTable;
                    return;
                }
                if (goalKind == SAM_GOAL_FETCH) {
                    w.invalidFetch = 0;
                    w.costume = g_pWolf->HandleMessage(this, MSG_WOLF_IS_SHEEPCOSTUME, 0);
                    w.costume |= g_pWolf->HandleMessage(this, MSG_WOLF_IS_RABBITCOSTUME, 0);
                    w.targetRegion =
                        PointInBoxListXYZ(&fetchGoal.object1->pos, authorizedZoneBoxes, authorizedZoneCount);
                    if (!fetchGoal.flags.bits.picked1 && fetchGoal.object1 &&
                        (!w.targetRegion || fetchGoal.object1->SamView_HasInstanceFlag(INST_F_ATTACHED) ||
                         (fetchGoal.object1 == g_pWolf && !w.costume)))
                        w.invalidFetch = 1;
                    else if (!fetchGoal.flags.bits.picked2 && fetchGoal.object2 &&
                             (!PointInBoxListXYZ(&fetchGoal.object2->pos, authorizedZoneBoxes, authorizedZoneCount) ||
                              fetchGoal.object2->SamView_HasInstanceFlag(INST_F_ATTACHED) ||
                              (fetchGoal.object1 == g_pWolf && !w.costume)))
                        w.invalidFetch = 1;
                    if (w.invalidFetch && (mode & (SAM_MODE_LEVEL_VARIANT | SAM_MODE_ALT_BEHAVIOUR))) {
                        w.interruptRegion =
                            PointInBoxListXYZ(&g_pWolf->pos, wolfCanBeHitZoneBoxes, wolfCanBeHitZoneCount);
                        if (w.interruptRegion) {
                            OnGoalReached(0, 1);
                            SAMU_START_SEARCH();
                            w.invalidFetch = 0;
                        }
                    }
                    if (w.invalidFetch) {
                        OnGoalReached(0, 0);
                        SAMU_START_SEARCH();
                    }
                } else if (goalKind == SAM_GOAL_TARGET && target == g_pWolf &&
                           !g_pWolf->HandleMessage(this, MSG_WOLF_IS_CATCHABLE, 0)) {
                    SetGoalHome();
                    alertLevel = SAM_ALERT_NONE;
                    SAMU_START_SEARCH();
                }
                if (goalKind == SAM_GOAL_HOME && (mode & SAM_MODE_GUARD_SHEEP) && catchableSheepBox) {
                    w.candidateCount = 0;
                    w.objectCount = Scenaric_FindByClass(CLASSID_SHEEP, w.objects, 32);
                    w.sheepCostume = g_pWolf->HandleMessage(this, MSG_WOLF_IS_SHEEPCOSTUME, 0);
                    for (w.objectIndex = 0; w.objectIndex < w.objectCount; ++w.objectIndex) {
                        w.candidate = w.objects[w.objectIndex];
                        if (!w.candidate->SamView_HasInstanceFlag(INST_F_ATTACHED)) {
                            w.catchPos = &w.candidate->pos;
                            w.catchBox = catchableSheepBox;
                            if (SAMU_CONTAINS3(w.catchBox, w.catchPos) && dontTryToCatchSheepBox) {
                                w.excludePos = &w.candidate->pos;
                                w.excludeBox = dontTryToCatchSheepBox;
                                if (!SAMU_CONTAINS2(w.excludeBox, w.excludePos) &&
                                    ((w.candidateClass486 = w.candidate->classId,
                                      w.candidateClass486 == CLASSID_SHEEP) ||
                                     (w.sheepCostume && (w.candidateClass488 = w.candidate->classId,
                                                         w.candidateClass488 == CLASSID_WOLF))) &&
                                    PointInBoxListXZ(&w.candidate->pos, authorizedZoneBoxes, authorizedZoneCount)) {
                                    w.selected[w.candidateCount++] = w.candidate;
                                    if (w.candidateCount == 2)
                                        break;
                                }
                            }
                        }
                    }
                    if (w.candidateCount) {
                        if (w.candidateCount == 1)
                            w.selected[1] = 0;
                        w.pendingFetch.object1 = w.selected[0];
                        w.pendingFetch.object2 = w.selected[1];
                        SetGoalFetch(&w.pendingFetch);
                        SAMU_START_SEARCH();
                    }
                }
            }
            break;
    }
    switch (this->state) {
        case SAM_STT_ANGRY:
            if (HasAnimFlags(ANIM_F_FINISHED)) {
                Camera_ReleaseScripted(this);
                SetGoalWolf();
                alertLevel = SAM_ALERT_RED;
                SAMU_START_SEARCH();
            }
            break;
        case SAM_STT_DISCOVERWOLF:
            w.reactionAnimation = anim.animId;
            if (w.reactionAnimation == GetAnimId(SAM_ANIM_WALK)) {
                if (!Scenaric_FindByClass(CLASSID_CATAPULT, &w.reactionObject, 1))
                    return;
                if (w.reactionObject->HandleMessage(this, MSG_QUERY_IDLE, 0)) {
                    PlayAnim(SAM_ANIM_ANGRY, 0, 1, 0x1000);
                    StartReactionCamera();
                }
            } else if (HasAnimFlags(ANIM_F_FINISHED)) {
                Camera_ReleaseScripted(this);
                UpdateGoalPos();
                SAMU_START_SEARCH();
            }
            break;
        case SAM_STT_FOLLOWTRAJECTORY:
        case SAM_STT_FOLLOWTRAJECTORY_FAST:
            UpdateStallTracker();
            if ((int)(patrolFlags << 31) >> 31) {
                if (stallClockMs > 1000) {
                    ResetStallAnchor();
                    patrolFlags &= ~SAM_PATROL_STARTED;
                } else {
                    w.patrolVelocity.x = patrolSidestepDir.x;
                    w.patrolVelocity.y = 1000;
                    w.patrolVelocity.z = patrolSidestepDir.z;
                    Vec3s_ScaleByDt(&w.patrolVelocity, &w.patrolDelta);
                    ResolveMove(&w.patrolDelta, &w.patrolContacts, 0xB54,
                                CQ_STATIC | CQ_STATIC_EXT | CQ_OBJECTS | CQ_ASK_MOVER | CQ_IGNORE_SKIPBOX, 0, 0);
                    Translate(&w.patrolDelta);
                }
                headYaw = 0;
                if (this->state != SAM_STT_FOLLOWTRAJECTORY_FAST)
                    DetectWolfPatrol(&g_pWolf->pos, 0x300);
                return;
            } else {
                w.patrolReached = TrajFollower_Step(&trajFollower, &w.patrolVelocity, &w.patrolYaw);
                if (stallClockMs > 3000) {
                    if (stallBestDist < 100) {
                        patrolFlags |= SAM_PATROL_STARTED;
                        patrolSidestepDir.x = w.patrolVelocity.z;
                        patrolSidestepDir.z = -w.patrolVelocity.x;
                    }
                    ResetStallAnchor();
                    return;
                } else {
                    w.patrolYawCopy = w.patrolYaw;
                    rot.y = w.patrolYawCopy;
                    w.patrolVelocity.y = 1000;
                    Vec3s_ScaleByDt(&w.patrolVelocity, &w.patrolDelta);
                    ResolveMove(&w.patrolDelta, &w.patrolContacts, 0xB54,
                                CQ_STATIC | CQ_STATIC_EXT | CQ_OBJECTS | CQ_SAM_WALL_OVERLAP | CQ_ASK_MOVER |
                                    CQ_IGNORE_SKIPBOX,
                                0, 0);
                    Translate(&w.patrolDelta);
                    headYaw = 0;
                    if (w.patrolReached && (mode & SAM_MODE_STOP_AT_NODES)) {
                        EnterLookAfterTimedState(10000);
                        return;
                    } else if (this->state != SAM_STT_FOLLOWTRAJECTORY_FAST)
                        DetectWolfPatrol(&g_pWolf->pos, 0x300);
                }
            }
            break;
        case SAM_STT_LOOKAFTER:
        case SAM_STT_LOOKAFTER_TIMED:
            headSweepPhase = (headSweepPhase + headSpeed * (g_dtMs >> 1)) & 0xFFF;
            headYaw = (g_sinTable4096[headSweepPhase] * maxAngle) / 360;
            w.jointAngles.x = 0;
            w.jointAngles.y = headYaw;
            w.jointAngles.z = 0;
            SetJointOverride(9, &w.jointAngles, 0, 0);
            if (lookAfterTimeoutMs >= 0 && this->state == SAM_STT_LOOKAFTER_TIMED) {
                lookAfterTimeoutMs -= (short)g_dtMs;
                if (lookAfterTimeoutMs < 0) {
                    EnterFollowTrajectoryState();
                    return;
                }
            }
            if (investigateTimerMs >= 0 && this->state != SAM_STT_LOOKAFTER_TIMED) {
                if (mode & SAM_MODE_CIN01_ON_NOISE)
                    EnterCin01State();
                else
                    EnterInvestigateState();
            } else
                DetectWolf(&goalPos, 0x71);
            if ((mode & SAM_MODE_ALT_BEHAVIOUR) &&
                (!Scenaric_FindByClass(CLASSID_SHEEP, &w.guardSheep, 1) ||
                 !PointInBoxListXZ(&w.guardSheep->pos, authorizedZoneBoxes, authorizedZoneCount)) &&
                trajectory) {
                homePos = trajectory->pts[0];
                SetGoalHome();
                mode = (mode & ~SAM_MODE_BEHAVIOUR_MASK) | SAM_MODE_PATROL;
                SAMU_START_SEARCH();
                return;
            }
            break;
        case SAM_STT_KNOCKEDOUT:
            if (HasAnimFlags(ANIM_F_FINISHED)) {
                w.koAnimation48e = anim.animId;
                if (w.koAnimation48e == koGetUpAnim) {
                    SetGoalWolf();
                    SAMU_START_SEARCH();
                    return;
                }
                w.koAnimation490 = anim.animId;
                if (w.koAnimation490 == koFallAnim) {
                    w.koLyingId = koLyingAnim;
                    w.koLyingFlags = 0;
                    if (1)
                        w.koLyingFlags |= ANIM_SET_LOOP;
                    if (0)
                        w.koLyingFlags |= ANIM_SET_BLEND;
                    Anim_Start(Inst(), &anim, w.koLyingId, w.koLyingFlags);
                    return;
                }
            }
            w.koAnimation498 = anim.animId;
            if (w.koAnimation498 == koLyingAnim) {
                actionTimerMs -= g_dtMs;
                if (actionTimerMs < 0) {
                    w.koGetUpId = koGetUpAnim;
                    w.koGetUpFlags = 0;
                    if (0)
                        w.koGetUpFlags |= ANIM_SET_LOOP;
                    if (0)
                        w.koGetUpFlags |= ANIM_SET_BLEND;
                    Anim_Start(Inst(), &anim, w.koGetUpId, w.koGetUpFlags);
                }
            }
            break;
        case SAM_STT_IDLE:
            if (g_pWolf) {
                w.idleYaw = yawToWolf;
                rot.y = w.idleYaw;
            }
            break;
        case SAM_STT_FOLLOWNODES:
            if (mode & SAM_MODE_NODES_ONLY) {
                w.routeYaw = (Math_RadiansToAngle4096((float)atan2((double)goalDx, (double)goalDz)) + 0x800) & 0xFFF;
                rot.y = w.routeYaw;
                if (goalDist != 0) {
                    w.routeVelocity.x = goalDx * ((speed * 3) / 2) / goalDist;
                    w.routeVelocity.y = 0;
                    w.routeVelocity.z = goalDz * ((speed * 3) / 2) / goalDist;
                } else {
                    w.routeVelocity.x = 0;
                    w.routeVelocity.y = 0;
                    w.routeVelocity.z = 0;
                }
                if (goalKind == SAM_GOAL_TARGET && Vec3s_ManhattanDistXZ(&targetPos, &pos) < 150) {
                    w.routeDelta.x = 0;
                    w.routeDelta.y = 0;
                    w.routeDelta.z = 0;
                } else
                    Vec3s_ScaleByDt(&w.routeVelocity, &w.routeDelta);
                SAMU_ADD_POS(w.intended, w.routeDelta);
                SAMU_ADD_POS(w.routeEnd, w.routeDelta);
                if (IsWolfOutsideHitZone(0, 1)) {
                    SetGoalHome();
                    NavPath_ClearSearch(&navSearch);
                    EnterSearchNearestNodeState(-goalDx, -goalDz);
                    return;
                }
                if (!PointInBoxListXZ(&w.routeEnd, authorizedZoneBoxes, authorizedZoneCount)) {
                    if (goalKind == SAM_GOAL_TARGET)
                        OnGoalReached(1, 0);
                    return;
                }
                Translate(&w.routeDelta);
                if (stallClockMs > 200)
                    ResetStallAnchor();
                return;
            }
            if (renavCooldownMs > 0)
                renavCooldownMs -= g_dtMs;
            if (waypointCount != 0) {
                waypoints[0].x = goalPos.x;
                waypoints[0].y = goalPos.y;
                waypoints[0].z = goalPos.z;
                w.routeDx = waypoints[waypointCount - 1].x - pos.x;
                w.routeDz = waypoints[waypointCount - 1].z - pos.z;
                w.routeDistance = (int)sqrt((double)w.routeDx * w.routeDx + w.routeDz * w.routeDz);
                if (w.routeDistance == 0)
                    w.routeDistance = 1;
            }
            w.destinationNode = SamNav_FindNearestReachableNode(pos.x, pos.z, goalPos.x, goalPos.y, goalPos.z);
            w.needSearch = navSearch.active == 0 && waypointCount == 0;
            w.changedNode =
                w.destinationNode != 0 && renavCooldownMs <= 0 &&
                !(w.routeSearch = &navSearch,
                  w.destinationNode->x == w.routeSearch->goalX && w.destinationNode->z == w.routeSearch->goalZ ? 1 : 0);
            if (followBits.midRoute && followBits.dropping)
                w.changedNode = 0;
            if (w.destinationNode && (w.needSearch || w.changedNode)) {
                w.edgeNode = SamNav_FindEdgeNear(&pos, &w.edgeIndex, &w.nearestEdgePoint);
                w.nearestNode = SamNav_FindNearestNode(pos.x, pos.z, &w.nearestDistance);
                w.routeVector.x = w.routeDx;
                w.routeVector.y = 0;
                w.routeVector.z = w.routeDz;
                renavCooldownMs = 500;
                NavPath_ClearSearch(&navSearch);
                if (w.nearestNode && w.nearestDistance < 2500)
                    w.savedStart = w.nearestNode;
                else {
                    if (w.edgeNode) {
                        w.neighbor = w.edgeNode->sons[w.edgeIndex];
                        if (SAM_ROUTE_ABS(w.neighbor->x - goalPos.x) + SAM_ROUTE_ABS(w.neighbor->z - goalPos.z) <
                            SAM_ROUTE_ABS(w.edgeNode->x - goalPos.x) + SAM_ROUTE_ABS(w.edgeNode->z - goalPos.z))
                            w.edgeNode = w.neighbor;
                        EnterReachNodeState(w.edgeNode, 0, 0);
                        return;
                    }
                    EnterSearchNearestNodeState(w.routeDx, w.routeDz);
                    return;
                }
                NavPath_BeginSearch(&navSearch, w.destinationNode->x, w.destinationNode->z, w.savedStart);
                if (SAM_ROUTE_ABS(pos.x - w.destinationNode->x) + SAM_ROUTE_ABS(pos.z - w.destinationNode->z) < 50 ||
                    w.savedStart == w.destinationNode) {
                    if (SAM_ROUTE_ABS(goalPos.y - pos.y) < 50) {
                        lastNodeTarget = waypoints[waypointCount];
                        waypointCount = 0;
                        NavPath_ClearSearch(&navSearch);
                        EnterReachGoalState();
                        return;
                    }
                }
            }
            if (navSearch.active != 0) {
                do {
                    w.routeResult = NavPath_StepSearch(&navSearch, &w.destinationNode);
                } while (w.routeResult == 0 && frameCounter == 0);
                if (w.routeResult == 1) {
                    NavPath_ClearSearch(&navSearch);
                    if (!followBits.midRoute || !followBits.dropping) {
                        EnterFollowNodeState(&goalPos, w.destinationNode, 0);
                        w.routeDx = waypoints[waypointCount - 1].x - pos.x;
                        w.routeDz = waypoints[waypointCount - 1].z - pos.z;
                        w.routeDistance = (int)sqrt((double)w.routeDx * w.routeDx + w.routeDz * w.routeDz);
                        if (w.routeDistance == 0)
                            w.routeDistance = 1;
                    }
                }
            }
            if (waypointCount != 0) {
                UpdateStallTracker();
                if (w.routeDistance < 50) {
                    --waypointCount;
                    followBits.midRoute = waypointCount > 1;
                    followBits.dropping = 0;
                    followBits.dropToNode = 0;
                    ResetStallAnchor();
                    return;
                }
                w.toWaypoint.x = w.routeDx;
                w.toWaypoint.y = 0;
                w.toWaypoint.z = w.routeDz;
                if (SAM_ROUTE_ABS(goalPos.y - pos.y) < 50 &&
                    SamNav_IsGoalNearOrNotAhead(&pos, &w.toWaypoint, w.routeDistance, &goalPos) &&
                    (!followBits.midRoute || !followBits.dropping) && !IsTargetNearFinalPathEdge()) {
                    lastNodeTarget = waypoints[waypointCount];
                    waypointCount = 0;
                    NavPath_ClearSearch(&navSearch);
                    EnterReachGoalState();
                    return;
                }
                if (stallClockMs > 200 && IsDistBelowHistoryThreshold(stallBestDist) &&
                    (!followBits.midRoute || !followBits.dropping)) {
                    lastNodeTarget = waypoints[waypointCount];
                    waypointCount = 0;
                    NavPath_ClearSearch(&navSearch);
                    EnterSearchNearestNodeState(w.routeDx, w.routeDz);
                    return;
                }
                w.forceDrop = 0;
                if (followBits.midRoute && !followBits.dropping && waypoints[waypointCount - 1].y > pos.y + 40) {
                    w.forceDrop = 1;
                    followBits.dropToNode = 1;
                }
                TurnYawToward((Math_RadiansToAngle4096((float)atan2((double)w.routeDx, (double)w.routeDz)) + 0x800) &
                              0xFFF);
                if (w.routeDistance == 0)
                    w.routeDistance = 1;
                w.routeVelocity.x = w.routeDx * speed / w.routeDistance;
                w.routeVelocity.y = followBits.dropToNode ? 2000 : 1000;
                w.routeVelocity.z = w.routeDz * speed / w.routeDistance;
                Vec3s_ScaleByDt(&w.routeVelocity, &w.routeDelta);
                w.collisionMask = 0xF;
                if (!followBits.midRoute || !followBits.dropping)
                    w.collisionMask |= CQ_ASK_MOVER;
                ResolveMove(&w.routeDelta, &w.routeContacts, 0xB54, w.collisionMask | CQ_IGNORE_SKIPBOX, 0, 0);
                SAMU_ADD_POS(w.routeCorrected, w.routeDelta);
                w.routeGroundY = QueryGroundY_Shrunk(&w.routeCorrected);
                if (w.routeGroundY - pos.y > 100 || w.forceDrop) {
                    if (followBits.midRoute)
                        followBits.dropping = 1;
                    else {
                        NavPath_ClearSearch(&navSearch);
                        EnterSearchNearestNodeState(w.routeDx, w.routeDz);
                        return;
                    }
                }
                if (!followBits.dropping && IsWolfOutsideHitZone(0, 1)) {
                    SetGoalHome();
                    NavPath_ClearSearch(&navSearch);
                    EnterSearchNearestNodeState(-w.routeDx, -w.routeDz);
                    return;
                }
                if (!PointInBoxListXZ(&w.routeCorrected, authorizedZoneBoxes, authorizedZoneCount)) {
                    if (followBits.dropping)
                        return;
                    if (goalKind == SAM_GOAL_TARGET)
                        OnGoalReached(1, 0);
                    NavPath_ClearSearch(&navSearch);
                    EnterSearchNearestNodeState(-w.routeDx, -w.routeDz);
                    ResetStallAnchor();
                    return;
                }
                Translate(&w.routeDelta);
                if (stallClockMs > 200)
                    ResetStallAnchor();
            }
            break;
        case SAM_STT_PUTDOWN:
            rot.y = 0x800;
            if (HasAnimFlags(ANIM_F_FINISHED)) {
                PutDownCarried(0);
                SetState(SAM_STT_AFTERPUTDOWN);
            }
            break;
        case SAM_STT_AFTERPUTDOWN:
            if (HasAnimFlags(ANIM_F_FINISHED)) {
                animTable = g_samAnimTable;
                if (mode & SAM_MODE_LEVEL_VARIANT) {
                    goalDx = goalPos.x - pos.x;
                    goalDz = goalPos.z - pos.z;
                    goalDist = (int)sqrt((double)goalDx * goalDx + goalDz * goalDz);
                    if (goalDist == 0)
                        goalDist = 1;
                }
                if ((mode & SAM_MODE_ALT_BEHAVIOUR) && (carryGoal.object1 == g_pWolf || carryGoal.object2 == g_pWolf)) {
                    w.overlapCount = ObjGrid_QueryBoxOverlap(catchableSheepBox, w.overlapObjects);
                    for (w.overlapIndex = 0; w.overlapIndex < w.overlapCount; ++w.overlapIndex) {
                        w.overlapClass = w.overlapObjects[w.overlapIndex]->classId;
                        if (w.overlapClass == CLASSID_SHEEP)
                            break;
                    }
                    if (w.overlapIndex != w.overlapCount) {
                        SetState(SAM_STT_ANGRY);
                        return;
                    }
                }
                EnterSearchNearestNodeState(goalDx, goalDz);
            }
            break;
        case SAM_STT_PICKUP:
            if (HasAnimFlags(ANIM_F_FINISHED)) {
                w.pickupCostume = g_pWolf->HandleMessage(this, MSG_WOLF_IS_SHEEPCOSTUME, 0);
                w.pickupCostume |= g_pWolf->HandleMessage(this, MSG_WOLF_IS_RABBITCOSTUME, 0);
                if (!fetchGoal.flags.bits.picked2) {
                    UnfreezePickupTarget(fetchGoal.object1);
                    if ((fetchGoal.object1 == g_pWolf && !w.pickupCostume) ||
                        (fetchGoal.object1 &&
                         (w.pickupClass1 = fetchGoal.object1->classId, w.pickupClass1 == CLASSID_INFLATABLESHEEP) &&
                         !fetchGoal.object1->HandleMessage(this, MSG_INFLATABLE_QUERY, 0))) {
                        w.remainingFetch = fetchGoal;
                        if (w.remainingFetch.object2) {
                            w.remainingFetch.object1 = w.remainingFetch.object2;
                            w.remainingFetch.object2 = 0;
                            w.remainingFetch.flags.bits.picked1 = w.remainingFetch.flags.bits.picked2;
                            w.remainingFetch.flags.bits.picked2 = 0;
                            SetGoalFetch(&w.remainingFetch);
                        } else
                            SetGoalHome();
                        EnterSearchNearestNodeState(goalDx, goalDz);
                        return;
                    }
                    AddCarried(fetchGoal.object1);
                    /* cast kept: the joint in the void * */
                    fetchGoal.object1->HandleMessage(this, MSG_PICKUP, (void *)0xE);
                    animTable = g_samAnimTableCarryOne;
                } else {
                    UnfreezePickupTarget(fetchGoal.object2);
                    if ((fetchGoal.object2 == g_pWolf && !w.pickupCostume) ||
                        (fetchGoal.object2 &&
                         (w.pickupClass2 = fetchGoal.object2->classId, w.pickupClass2 == CLASSID_INFLATABLESHEEP) &&
                         !fetchGoal.object2->HandleMessage(this, MSG_INFLATABLE_QUERY, 0))) {
                        w.remainingCarry = carryGoal;
                        w.remainingCarry.object2 = 0;
                        SetGoalCarry(&w.remainingCarry);
                        SAMU_START_SEARCH();
                        return;
                    }
                    AddCarried(fetchGoal.object2);
                    /* cast kept: the joint in the void * */
                    fetchGoal.object2->HandleMessage(this, MSG_PICKUP, (void *)0x14);
                    animTable = g_samAnimTableCarryTwo;
                }
                SetState(SAM_STT_LIFT);
            }
            break;
        case SAM_STT_LIFT:
            if (HasAnimFlags(ANIM_F_FINISHED))
                EnterSearchNearestNodeState(goalDx, goalDz);
            break;
        case SAM_STT_INVESTIGATE:
            w.investigateDx = investigatePos.x - pos.x;
            w.investigateDz = investigatePos.z - pos.z;
            w.investigateDistance =
                (int)sqrt((double)w.investigateDx * w.investigateDx + w.investigateDz * w.investigateDz);
            if (w.investigateDistance == 0)
                w.investigateDistance = 1;
            w.investigateAnimation1 = anim.animId;
            if (w.investigateAnimation1 == GetAnimId(SAM_ANIM_INVESTIGATE_START)) {
                if (HasAnimFlags(ANIM_F_FINISHED))
                    PlayAnim(SAM_ANIM_INVESTIGATE_IDLE, 1, 1, 0x1000);
                return;
            } else if (investigateTimerMs < 0) {
                EnterReachGoalState();
                return;
            } else {
                investigateTimerMs -= (short)g_dtMs;
                w.investigateYaw =
                    (Math_RadiansToAngle4096((float)atan2((double)w.investigateDx, (double)w.investigateDz)) + 0x800) &
                    0xFFF;
                rot.y = w.investigateYaw;
                w.investigateVelocity.x = w.investigateDx * 200 / w.investigateDistance;
                w.investigateVelocity.y = 1000;
                w.investigateVelocity.z = w.investigateDz * 200 / w.investigateDistance;
                Vec3s_ScaleByDt(&w.investigateVelocity, &w.investigateDelta);
                SAMU_ADD_POS(w.intended, w.investigateDelta);
                ResolveMove(&w.investigateDelta, &w.investigateContacts, 0xB54,
                            CQ_STATIC | CQ_STATIC_EXT | CQ_OBJECTS | CQ_SAM_WALL_OVERLAP | CQ_ASK_MOVER |
                                CQ_IGNORE_SKIPBOX,
                            0, 0);
                SAMU_ADD_POS(w.investigateEnd, w.investigateDelta);
                if (w.investigateDistance < 200 ||
                    !PointInBoxListXZ(&w.investigateEnd, hypnotizedZoneBoxes, hypnotizedZoneCount)) {
                    if (investigateStopDelayMs <= 0) {
                        w.investigateAnimation2 = anim.animId;
                        if (w.investigateAnimation2 != GetAnimId(SAM_ANIM_INVESTIGATE_IDLE))
                            PlayAnim(SAM_ANIM_INVESTIGATE_IDLE, 1, 1, 0x1000);
                    } else
                        investigateStopDelayMs -= (short)g_dtMs;
                    return;
                } else {
                    investigateStopDelayMs = 200;
                    w.investigateAnimation3 = anim.animId;
                    if (w.investigateAnimation3 != GetAnimId(SAM_ANIM_INVESTIGATE_WALK))
                        PlayAnim(SAM_ANIM_INVESTIGATE_WALK, 1, 1, 0x1000);
                    Translate(&w.investigateDelta);
                }
            }
            break;
        case SAM_STT_REACHGOAL:
            UpdateStallTracker();
            w.directYaw = (Math_RadiansToAngle4096((float)atan2((double)goalDx, (double)goalDz)) + 0x800) & 0xFFF;
            rot.y = w.directYaw;
            if (goalDist == 0)
                goalDist = 1;
            w.directVelocity.x = goalDx * speed / goalDist;
            w.directVelocity.z = goalDz * speed / goalDist;
            w.directVelocity.y = 1000;
            Vec3s_ScaleByDt(&w.directVelocity, &w.directDelta);
            SAMU_ADD_POS(w.intended, w.directDelta);
            ResolveMove(&w.directDelta, &w.directContacts, 0xB54,
                        CQ_STATIC | CQ_STATIC_EXT | CQ_OBJECTS | CQ_SAM_WALL_OVERLAP | CQ_ASK_MOVER | CQ_IGNORE_SKIPBOX,
                        0, 0);
            SAMU_ADD_POS(w.directEnd, w.directDelta);
            w.directGroundY = QueryGroundY_Shrunk(&w.directEnd);
            jumpPhase = CheckStepUpAhead(&w.intended, w.directContacts.minStaticY);
            if (jumpPhase != SAM_JUMP_NONE)
                SetupJump(&w.intended);
            else if (IsWolfOutsideHitZone(0, 1)) {
                SetGoalHome();
                NavPath_ClearSearch(&navSearch);
                EnterSearchNearestNodeState(-goalDx, -goalDz);
                return;
            }
            if (w.directGroundY - pos.y > 100 || (stallClockMs > 200 && IsDistBelowHistoryThreshold(stallBestDist)) ||
                goalDist > 1400) {
                EnterSearchNearestNodeState(goalDx, goalDz);
                return;
            } else if (!PointInBoxListXZ(&w.directEnd, authorizedZoneBoxes, authorizedZoneCount)) {
                if (goalKind == SAM_GOAL_TARGET)
                    OnGoalReached(1, 0);
                EnterSearchNearestNodeState(-goalDx, -goalDz);
                return;
            } else {
                Translate(&w.directDelta);
                if (stallClockMs > 200)
                    ResetStallAnchor();
            }
            break;
        case SAM_STT_SEARCHNEARESTNODE:
            UpdateStallTracker();
            if (speed < 1100)
                w.searchStallLimit = 1600;
            else
                w.searchStallLimit = 200;
            w.searchDistance = Vec3s_ManhattanDistXZ(&pos, &goalPos);
            if (w.searchDistance == 0)
                w.searchDistance = 1;
            if (w.searchDistance < 500 && stateTimerMs > 3000 && FindPathHistory(SAM_HIST_REACHGOAL, 0) < 0) {
                EnterReachGoalState();
                return;
            }
            w.searchVelocity.x = (g_sinTable4096[searchHeading] * speed) >> 12;
            w.searchVelocity.y = 1000;
            w.searchVelocity.z = (g_pCosTable[searchHeading] * speed) >> 12;
            Vec3s_ScaleByDt(&w.searchVelocity, &w.searchDelta);
            SAMU_ADD_POS(w.intended, w.searchDelta);
            ResolveMove(&w.searchDelta, &w.searchContacts, 0xB54,
                        CQ_STATIC | CQ_STATIC_EXT | CQ_OBJECTS | CQ_SAM_WALL_OVERLAP | CQ_ASK_MOVER | CQ_IGNORE_SKIPBOX,
                        0, 0);
            SAMU_ADD_POS(w.searchEnd, w.searchDelta);
            w.searchGroundY = QueryGroundY_Shrunk(&w.searchEnd);
            w.searchBlocked = 0;
            jumpPhase = CheckStepUpAhead(&w.intended, w.searchContacts.minStaticY);
            if (jumpPhase != SAM_JUMP_NONE)
                SetupJump(&w.intended);
            w.searchInside = PointInBoxListXZ(&w.searchEnd, authorizedZoneBoxes, authorizedZoneCount);
            if (w.searchGroundY - pos.y > 100 ||
                (stallClockMs > w.searchStallLimit && IsDistBelowHistoryThreshold(stallBestDist)) || w.searchBlocked ||
                !w.searchInside) {
                if (!w.searchInside && goalKind == SAM_GOAL_TARGET)
                    OnGoalReached(1, 0);
                if (w.searchBlocked && searchBlockedMs > 5000) {
                    searchBlockedMs = 0;
                    searchHeading = (searchHeading + 4 * searchHeadingStep) & 0xFFF;
                    searchTurnCounter = (searchTurnCounter + 4) & 7;
                    AddPathHistory(SAM_HIST_SEARCH, searchHeading, searchHeadingStep);
                } else {
                    w.searchBudget = Rand_Range(3, 6);
                    do {
                        w.enteredDirect = 0;
                        searchHeading += searchHeadingStep;
                        searchHeading = searchHeading & 0xFFF;
                        searchTurnCounter = (searchTurnCounter + 1) & 7;
                        --w.searchBudget;
                        if (w.searchBudget < 0)
                            break;
                        if (searchTurnCounter == 0 && stateTimerMs > 10000) {
                            EnterReachGoalState();
                            w.enteredDirect = 1;
                            break;
                        }
                        w.searchHistory = FindPathHistory(SAM_HIST_SEARCH, searchHeading);
                    } while (w.searchHistory >= 0);
                    if (w.enteredDirect != 0)
                        return;
                    AddPathHistory(SAM_HIST_SEARCH, searchHeading, searchHeadingStep);
                }
                TurnYawToward((searchHeading + 0x800) & 0xFFF);
                ResetStallAnchor();
                return;
            }
            if (stallClockMs > w.searchStallLimit)
                ResetStallAnchor();
            Translate(&w.searchDelta);
            TurnYawToward((searchHeading + 0x800) & 0xFFF);
            w.searchEdge = SamNav_FindEdgeNear(&pos, &w.searchEdgeIndex, &w.searchEdgePoint);
            if (w.searchEdge && FindPathHistory(SAM_HIST_REACHNODE, 0) < 0)
                EnterReachNodeState(w.searchEdge, w.searchEdgeIndex, 1);
            break;
        case SAM_STT_REACHNODE:
            UpdateStallTracker();
            w.nodeDx = waypoints[0].x - pos.x;
            w.nodeDz = waypoints[0].z - pos.z;
            w.nodeDistance = (int)sqrt((double)w.nodeDx * w.nodeDx + w.nodeDz * w.nodeDz);
            if (w.nodeDistance == 0)
                w.nodeDistance = 1;
            if (w.nodeDistance < 50) {
                w.nodePathHead = SamNav_FindNearestReachableNode(pos.x, pos.z, goalPos.x, goalPos.y, goalPos.z);
                w.nodeStart = SamNav_FindNearestNode(pos.x, pos.z, &w.nodeNearestDistance);
                NavPath_ClearSearch(&navSearch);
                NavPath_BeginSearch(&navSearch, w.nodePathHead->x, w.nodePathHead->z, w.nodeStart);
                do {
                    w.nodeSearchResult = NavPath_StepSearch(&navSearch, &w.nodePathHead);
                } while (w.nodeSearchResult == 0);
                NavPath_ClearSearch(&navSearch);
                EnterFollowNodeState(&goalPos, w.nodePathHead, 0);
                renavCooldownMs = 500;
                return;
            }
            if (stallClockMs > 200 && IsDistBelowHistoryThreshold(stallBestDist)) {
                EnterSearchNearestNodeState(w.nodeDx, w.nodeDz);
                return;
            }
            TurnYawToward((Math_RadiansToAngle4096((float)atan2((double)w.nodeDx, (double)w.nodeDz)) + 0x800) & 0xFFF);
            w.nodeVelocity.x = w.nodeDx * speed / w.nodeDistance;
            w.nodeVelocity.y = 1000;
            w.nodeVelocity.z = w.nodeDz * speed / w.nodeDistance;
            Vec3s_ScaleByDt(&w.nodeVelocity, &w.nodeDelta);
            SAMU_ADD_POS(w.intended, w.nodeDelta);
            ResolveMove(&w.nodeDelta, &w.nodeContacts, 0xB54,
                        CQ_STATIC | CQ_STATIC_EXT | CQ_OBJECTS | CQ_SAM_WALL_OVERLAP | CQ_ASK_MOVER | CQ_IGNORE_SKIPBOX,
                        0, 0);
            SAMU_ADD_POS(w.nodeEnd, w.nodeDelta);
            w.nodeGroundY = QueryGroundY_Shrunk(&w.nodeEnd);
            jumpPhase = CheckStepUpAhead(&w.intended, w.nodeContacts.minStaticY);
            if (jumpPhase != SAM_JUMP_NONE)
                SetupJump(&w.intended);
            w.nodeInside = PointInBoxListXZ(&w.nodeEnd, authorizedZoneBoxes, authorizedZoneCount);
            if (w.nodeGroundY - pos.y > 100 || !w.nodeInside) {
                if (!w.nodeInside && goalKind == SAM_GOAL_TARGET)
                    OnGoalReached(1, 0);
                EnterSearchNearestNodeState(w.nodeDx, w.nodeDz);
                return;
            }
            Translate(&w.nodeDelta);
            if (stallClockMs > 200)
                ResetStallAnchor();
            break;
        case SAM_STT_FADE:
            g_samFadeAmount = 0;
            g_samFadeEnabled = 1;
            w.tintEnabled = g_samFadeEnabled;
            g_samFadeAmount += 64;
            w.tintAmount = g_samFadeAmount;
            tintColor = 0;
            tintAmount = w.tintAmount;
            if (w.tintEnabled) {
                w.tintSetFlags = &inst_flags;
                *w.tintSetFlags |= SamUpdate_SetMask16(INST_F_TINT);
            } else {
                w.tintClearFlags = &inst_flags;
                *w.tintClearFlags &= SamUpdate_ClearMask16(INST_F_TINT);
            }
            if (!g_samFadeEnabled)
                SetState(SAM_STT_IDLE);
            if (g_samFadeAmount == 0x1000) {
                g_samFadeAmount = 0;
                g_samFadeEnabled = 0;
            }
            break;
        case SAM_STT_CIN01:
            /* cast kept: HandleMessage's arg is a void *; this message passes a number in it */
            g_pWolf->HandleMessage(this, MSG_WOLF_TAKE_ITEM_CLASS, (void *)CLASSID_FLUTE);
            w.cineText = cin01TextHandle;
            w.cineSheepBox = cin01SheepBox;
            w.cineBox = cin01Box;
            w.cineFlags = cin01Flags;
            w.cineId = cin01;
            /* cast kept (both): Box and CollBox are two views of one 16-byte record; Cine::Start takes the Box view */
            g_cinePlayer.Start(w.cineId, w.cineFlags, (Box *)w.cineBox, (Box *)w.cineSheepBox, w.cineText, 0);
            restartStatePending = 1;
            mode = (mode & ~SAM_MODE_BEHAVIOUR_MASK) + 1;
            EnterFollowTrajectoryState();
            break;
        case SAM_STT_HITROBOT:
            if (HasAnimFlags(ANIM_F_FINISHED)) {
                w.robotAnimation = anim.animId;
                if (w.robotAnimation == GetAnimId(SAM_ANIM_HIT_ROBOT))
                    PlayAnim(SAM_ANIM_ANGRY, 1, 1, 0x1000);
            }
            if (actionTimerMs < 0) {
                SetGoalHome();
                SAMU_START_SEARCH();
            } else
                actionTimerMs -= g_dtMs;
            break;
        case SAM_STT_WATCHROBOT:
            UpdateGoalPos();
            if (Vec3s_ManhattanDistXZ(&robot->pos, &robotWatchPos) > 10) {
                KickRobot(1);
                return;
            } else {
                actionTimerMs -= g_dtMs;
                if (actionTimerMs < 0) {
                    KickRobot(1);
                    return;
                }
            }
            break;
        case SAM_STT_CATCHWOLF:
            if (HasAnimFlags(ANIM_F_FINISHED)) {
                w.catchAnimation1 = anim.animId;
                if (w.catchAnimation1 == GetAnimId(SAM_ANIM_GRAB)) {
                    /* cast kept: HandleMessage's arg is a void *; this message passes a number in it */
                    g_pWolf->HandleMessage(this, MSG_WOLF_CAUGHT, (void *)CAUGHT_END);
                    SetState(SAM_STT_CATCHDONE);
                } else {
                    if (CurrentAnim() == GetAnimId(SAM_ANIM_HIT))
                        SetState(SAM_STT_CATCHDONE);
                }
            }
            break;
    }
}
#undef SAMU_START_SEARCH
#undef SAMU_ADD_POS
#undef SAMU_CONTAINS3
#undef SAMU_CONTAINS2

/* 0x46B39E - Sam_DrawZoneIndicator; 483 compared bytes including any local tables. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; unused108, unusedD0, translation, unused12 fill gaps */
void Sam::DrawZoneIndicator()
{
    struct HudWork {
        unsigned short unused108;
        SamScreenGeometry screenGeometry;
        short top, bottom, right, left;
        int flags;
        Mat34s fixedMatrix;
        unsigned char unusedD0[0x20];
        Mat44 viewMatrix, copyMatrix;
        int translation[4];
        Vec3s eye;
        short distance;
        Vec3s angles;
        unsigned char unused12[6];
        int viewFlags;
        short screenX, screenY, unused4, angle;
    } w;
    w.viewFlags = 0x30;
    w.flags = 0x30;
    w.angle = relAngleToWolf & 0xFFF;
    if (alertLevel >= SAM_ALERT_GREEN) {
        Mat34s_Identity(&w.fixedMatrix);
        w.angles.x = w.angles.y = w.angles.z = 0;
        head.rot.y = -w.angle;
        w.distance = 300;
        Vec3s_OffsetAlongAngles(&w.eye, &w.angles, w.distance, g_pZeroVec3s);
        /* cast kept (both (Camera *) views): a Camera begins with its Mat34s view matrix; Sam passes its local one */
        Camera_BuildViewMatrix((Camera *)&w.fixedMatrix, g_pZeroVec3s);
        w.screenX = g_screen.virtWidth - (g_screen.virtWidth >> 3);
        w.screenY = g_screen.GetGeometry(&w.screenGeometry)->height >> 2;
        /* cast kept: the same local Mat34s passed where a Camera is taken (see above) */
        head.RenderEx((Camera *)&w.fixedMatrix, frameToggle ? headRenderScratch : headRenderScratch + 400, 100, 0x180,
                      &w.screenX);
        w.left = w.screenX - 35;
        w.bottom = w.screenY;
        w.right = w.screenX + 35;
        w.top = w.screenY - 40;
        g_animSpriteCrayon1.Draw(g_screenLayerBase + 9, w.left, w.bottom, w.right, w.top, g_samZoneColors[alertLevel],
                                 0, 0);
    }
}

/* 0x46B581 - Sam_KnockOut; 175 compared bytes including any local tables. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
/* BYTES(flow): constant-argument inline expansion */
void Sam::KnockOut(unsigned short fallAnim, unsigned short lyingAnim, unsigned short getUpAnim)
{
    struct {
        int duration, offset;
        unsigned int flags;
        unsigned char *record;
    } w;
    /* cast kept (both): designer properties are 4-byte slots at byte offsets of the raw WAR record */
    w.record = (unsigned char *)this->record;
    w.flags = 0;
    if (0)
        w.flags |= ANIM_SET_LOOP;
    if (0)
        w.flags |= ANIM_SET_BLEND;
    Anim_Start(Inst(), &anim, fallAnim, w.flags);
    koGetUpAnim = getUpAnim;
    koLyingAnim = lyingAnim;
    koFallAnim = fallAnim;
    w.offset = 0x20;
    /* cast kept: a designer-property record read at its byte offset */
    w.duration = *(int *)(w.record + w.offset + 0x14);
    actionTimerMs = w.duration;
    SetState(SAM_STT_KNOCKEDOUT);
}

/* 0x46B630 - Sam_Render; 347 compared bytes including any local tables. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
void Sam::Render(Camera *view)
{
    struct {
        int amplitudeShift, phaseShift;
        Vec3s temporaryPos;
        short unused6, unused4, savedY;
    } w;
    if (IsDistantForRender()) {
        if (SamView_HasInstanceFlag(INST_F_HAS_SECONDARY)) {
            if (CurrentAnim() == GetAnimId(SAM_ANIM_RUN)) {
                w.temporaryPos = pos;
                w.savedY = w.temporaryPos.y;
                if (speed < 1100) {
                    w.phaseShift = 9;
                    w.amplitudeShift = 3;
                } else {
                    w.amplitudeShift = 4;
                    w.phaseShift = 8;
                }
                w.temporaryPos.y += (short)(g_sinTable4096[(short)((lifeTimerMs << (12 - w.phaseShift)) & 0xFFF)] >>
                                            (12 - w.amplitudeShift));
                SetPosition(&w.temporaryPos);
                ScnMobile::Render(view);
                w.temporaryPos.y = w.savedY;
                SetPosition(&w.temporaryPos);
            } else
                ScnMobile::Render(view);
        }
    } else
        ScnMobile::Render(view);
}

/* 0x46B78B - Sam_HandleMessage; 1194 compared bytes including any local tables. */
int Sam::HandleMessage(ScnObject *sender, unsigned int message, void *payload)
{
    switch (message) {
        case MSG_COLLIDE_IGNORE_QUERY:
            /* cast kept: this query passes the other object in its void * argument */
            switch (((ScnObject *)payload)->SamView_GetClassId()) {
                case CLASSID_WOODEN_LIFT:
                case CLASSID_INFLATABLESHEEP:
                    return 1;
                default:
                    return 0;
            }
        case MSG_SHEEP_CALL:
            if (PointInBoxListXZ(&sender->pos, orangeZoneBoxes, orangeZoneCount)) {
                if (dontTryToCatchSheepBox && SamView_ContainsXZ(dontTryToCatchSheepBox, &sender->pos) &&
                    this->state == SAM_STT_LOOKAFTER) {
                    /* Suppress the collection request in the guard exclusion area. */
                } else
                    sheepFlags |= SAM_SHEEP_HEARD_CALL;
            }
            return 1;
        case MSG_SAM_IS_RED:
            return IsAlert();
        case MSG_SAM_INVESTIGATE:
            investigateTimerMs = 1000;
            investigatePos = sender->pos;
            return 1;
        case MSG_SAM_FETCH_NOTIFY:
            /* cast kept: the object comes in the void * */
            if (goalKind == SAM_GOAL_FETCH && (fetchGoal.object1 == g_pWolf || fetchGoal.object2 == g_pWolf))
                catapultLoadedObj = (ScnObject *)payload;
            else
                catapultLoadedObj = 0;
            return 1;
        case MSG_FREEZE:
            frozenPickupObj = 0;
            return 1;
        case MSG_TIMEMACHINE_CALL:
            if (this->state == SAM_STT_CATCHWOLF || this->state == SAM_STT_CATCHDONE)
                return 0;
            if (jumpPhase)
                return 0;
            return 1;
        case MSG_TRAP_STATE:
            forcedAlert = 1;
            return 1;
        case MSG_LOUD_NOISE:
            /* cast kept: the noise message passes the maker's class id in its void * argument */
            if (payload == (void *)CLASSID_DYNAMITE &&
                PointInBoxListXZ(&sender->pos, authorizedZoneBoxes, authorizedZoneCount))
                forcedAlert = 1;
            return 1;
        case MSG_BEES_STING:
            if (this->state != SAM_STT_FOLLOWTRAJECTORY_FAST && this->state != SAM_STT_CATCHWOLF)
                EnterFollowTrajectoryFastState();
            return 1;
        case MSG_KILL:
            switch ((int)payload) { /* cast kept: MSG_KILL passes the kill type in its void * argument */
                case KILL_CRUSH:
                    /* cast kept: the arg */
                    if (this->state == SAM_STT_CATCHWOLF)
                        g_pWolf->HandleMessage(this, MSG_WOLF_CAUGHT, (void *)CAUGHT_RELEASE);
                    KnockOut(GetAnimId(SAM_ANIM_KO_FALL), GetAnimId(SAM_ANIM_KO_LYING), GetAnimId(SAM_ANIM_KO_GETUP));
                    break;
                case KILL_GENERIC:
                    SamView_SetTint(0, 0x1000, 1);
                    head.SamView_SetTint(0, 0x1000, 1);
                    break;
            }
            return 1;
        default:
            return 0;
    }
}

/* 0x46BC35 - Sam_Create; 141 bytes. */
ScnObject *Sam_Create(void *record)
{
    Sam *object = new Sam;
    object = (Sam *)object->Init(record, 0); /* cast kept: Init returns the ScnObject base */
    return object;
}

/* 0x46BCC2 - Sam_IsWolfBehindHidingBox; 121 compared bytes including any local tables. */
int Sam::IsWolfBehindHidingBox()
{
    Vec3s delta;
    delta.x = g_pWolf->pos.x - pos.x;
    delta.y = g_pWolf->pos.y - pos.y;
    delta.z = g_pWolf->pos.z - pos.z;
    return Collide_SegmentVsBoxListXZ(&pos, &delta, hidingBoxes, hidingBoxCount);
}

/* 0x46BD3B - Sam_InitHeadRecord; 130 compared bytes including any local tables. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
void Sam::InitHeadRecord(unsigned short *record)
{
    struct {
        unsigned int *resources;
        unsigned short unused8, count;
        unsigned int resource;
    } w;
    record[2] = 0;
    record[3] = 0;
    record[4] = 0;
    record[8] = 0;
    record[7] = 0;
    record[6] = 0;
    record[5] = 0;
    w.resources = Scn_FindIdList(WAR_IDO_ATESAM01, &w.count);
    w.resource = *w.resources;
    record[0] = Dav_FindResourceIndex((void *)w.resource); /* cast kept: the id list holds the pointer as a u32 */
    record[1] = 0xFFFF;
}

/* 0x46BDBD - Sam_UpdateZoneState; 822 compared bytes including any local tables. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
void Sam::UpdateZoneState()
{
    /* Reconstructed local layout and names. Zone tests include height. */
    struct {
        Vec3s *orangePoint;
        CollBox *orangeBox;
        Vec3s *greenPoint;
        CollBox *greenBox;
        int distanceThreshold, inOrange;
        CollBox **cursor;
        ScnObject *wolf;
        CollBox **end;
    } w;
    w.distanceThreshold = 400;
    if (mode & SAM_MODE_ALT_BEHAVIOUR)
        w.distanceThreshold = 1000;
    w.wolf = g_pWolf;
    wolfInGreenZone = 0;
    w.cursor = greenZoneBoxes;
    w.end = w.cursor + greenZoneCount;
    for (; w.cursor < w.end; w.cursor++) {
        w.greenPoint = &w.wolf->pos;
        w.greenBox = *w.cursor;
        if (SamView_ContainsXYZ(w.greenPoint, w.greenBox)) {
            wolfInGreenZone = 1;
            break;
        }
    }
    if (this->state == SAM_STT_FOLLOWTRAJECTORY && (goalDist < w.distanceThreshold + 300 || goalDist < distCatchPatrol))
        wolfInGreenZone = 1;
    if (goalKind == SAM_GOAL_TARGET) {
        switch (this->state) {
            case SAM_STT_FOLLOWNODES:
            case SAM_STT_REACHGOAL:
            case SAM_STT_SEARCHNEARESTNODE:
            case SAM_STT_REACHNODE:
                alertLevel = SAM_ALERT_RED;
                break;
        }
        if (alertLevel == SAM_ALERT_RED)
            return;
    } else {
        switch (this->state) {
            case SAM_STT_CATCHWOLF:
            case SAM_STT_CIN01:
            case SAM_STT_CATCHDONE:
                alertLevel = SAM_ALERT_RED;
                return;
        }
    }
    if (wolfInGreenZone) {
        w.inOrange = 0;
        if (this->state == SAM_STT_FOLLOWTRAJECTORY) {
            if (goalDist < w.distanceThreshold || goalDist < distCatchPatrol)
                w.inOrange = 1;
        } else {
            w.cursor = orangeZoneBoxes;
            w.end = w.cursor + orangeZoneCount;
            for (; w.cursor < w.end; w.cursor++) {
                w.orangePoint = &w.wolf->pos;
                w.orangeBox = *w.cursor;
                if (SamView_ContainsXYZ(w.orangePoint, w.orangeBox)) {
                    w.inOrange = 1;
                    break;
                }
            }
        }
        if (w.inOrange)
            alertLevel = SAM_ALERT_ORANGE;
        else
            alertLevel = SAM_ALERT_GREEN;
    } else
        alertLevel = SAM_ALERT_NONE;
}

/* 0x46C0F3 - Sam_SetGoalWolf; 63 compared bytes including any local tables. */
void Sam::SetGoalWolf()
{
    speed = 1100;
    goalKind = SAM_GOAL_TARGET;
    target = g_pWolf;
    UpdateGoalPos();
    ClearPathHistory();
}

/* 0x46C132 - Sam_SetGoalCarry; 84 compared bytes including any local tables. */
void Sam::SetGoalCarry(SamCarryGoal *goal)
{
    goalKind = SAM_GOAL_CARRY;
    speed = 1100;
    carryGoal = *goal;
    UpdateGoalPos();
    ClearPathHistory();
}

/* 0x46C186 - Sam_SetGoalFetch; 120 compared bytes including any local tables. */
void Sam::SetGoalFetch(SamFetchGoal *goal)
{
    goalKind = SAM_GOAL_FETCH;
    speed = 1100;
    fetchGoal = *goal;
    fetchGoal.flags.all &= ~SAM_FETCH_PICKED1;
    fetchGoal.flags.all &= ~SAM_FETCH_PICKED2;
    UpdateGoalPos();
    ClearPathHistory();
}

/* 0x46C1FE - Sam_SetGoalHome; 269 compared bytes including any local tables. */
void Sam::SetGoalHome()
{
    int inside;
    ScnObject *sheep;
    goalKind = SAM_GOAL_HOME;
    speed = 1100;
    if (mode & SAM_MODE_ALT_BEHAVIOUR) {
        inside = 0;
        Scenaric_FindByClass(CLASSID_SHEEP, &sheep, 1);
        if (PointInBoxListXZ(&sheep->pos, orangeZoneBoxes, orangeZoneCount))
            inside = 1;
        if (inside == 0) {
            mode = (mode & ~SAM_MODE_BEHAVIOUR_MASK) | SAM_MODE_PATROL;
            homePos = trajectory->pts[0];
        } else {
            ScnRecordSynth *record;
            mode &= ~SAM_MODE_BEHAVIOUR_MASK;
            record = (ScnRecordSynth *)this->record; /* cast kept: the WAR record read through its layout */
            homePos = record->pos;
        }
    }
    UpdateGoalPos();
    ClearPathHistory();
}

/* 0x46C30B - Sam_IsWolfOutsideHitZone; 126 compared bytes including any local tables. */
int Sam::IsWolfOutsideHitZone(int ignoreTimer, int requireGoalChase)
{
    if (requireGoalChase && goalKind != SAM_GOAL_TARGET)
        return 0;
    if (target != g_pWolf)
        return 0;
    if (!ignoreTimer && stateTimerMs < 200)
        return 0;
    return !PointInBoxListXYZ(&g_pWolf->pos, wolfCanBeHitZoneBoxes, wolfCanBeHitZoneCount);
}

/* 0x46C389 - Sam_UpdateGoalPos; 1357 compared bytes including any local tables. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
void Sam::UpdateGoalPos()
{
    struct {
        int lead, distance, leadTime;
        short unused8, angle;
        int reply;
    } w;
    switch (goalKind) {
        case SAM_GOAL_TARGET:
            if (robot && PointInBoxListXZ(&robot->pos, authorizedZoneBoxes, authorizedZoneCount) &&
                !PointInBoxListXZ(&g_pWolf->pos, authorizedZoneBoxes, authorizedZoneCount)) {
                goalPos = robot->pos;
                targetPos = goalPos;
                target = robot;
                break;
            }
            target = g_pWolf;
            w.reply = g_pWolf->HandleMessage(this, MSG_WOLF_IS_RUNNING, 0);
            goalPos = g_pWolf->pos;
            targetPos = goalPos;
            if (w.reply && !(mode & SAM_MODE_NODES_ONLY)) {
                w.angle = (-0x400 - g_pWolf->rot.y) & 0xFFF;
                w.distance = Vec3s_ManhattanDistXZ(&goalPos, &pos);
                w.leadTime = (w.distance << 10) / speed;
                if (w.leadTime > 1000)
                    w.leadTime = 1000;
                w.lead = (w.leadTime * 1200) >> 10;
                goalPos.x += (short)((g_pCosTable[w.angle] * w.lead) >> 12);
                goalPos.z += (short)((g_sinTable4096[w.angle] * w.lead) >> 12);
                if ((g_pWolf->pos.x - pos.x) * (goalPos.x - pos.x) + (g_pWolf->pos.z - pos.z) * (goalPos.z - pos.z) < 0)
                    goalPos = targetPos;
            }
            break;
        case SAM_GOAL_FETCH:
            if (fetchGoal.object1 && !fetchGoal.flags.bits.picked1)
                goalPos = fetchGoal.object1->pos;
            else if (fetchGoal.object2 && !fetchGoal.flags.bits.picked2)
                goalPos = fetchGoal.object2->pos;
            else
                goalPos = g_pWolf->pos;
            if (fetchGoal.flags.bits.picked1 && fetchGoal.object1->SamView_HasInstanceFlag(INST_F_ATTACHED))
                fetchGoal.object1->SetPosition(&pos);
            if (fetchGoal.flags.bits.picked2 && fetchGoal.object2->SamView_HasInstanceFlag(INST_F_ATTACHED))
                fetchGoal.object2->SetPosition(&pos);
            targetPos = goalPos;
            break;
        case SAM_GOAL_CARRY:
            goalPos = carryGoal.destination;
            targetPos = goalPos;
            if (carryGoal.object2 && carryGoal.object2->SamView_HasInstanceFlag(INST_F_ATTACHED))
                carryGoal.object2->SetPosition(&pos);
            if (carryGoal.object1 && carryGoal.object1->SamView_HasInstanceFlag(INST_F_ATTACHED))
                carryGoal.object1->SetPosition(&pos);
            break;
        case SAM_GOAL_HOME:
            if (mode & SAM_MODE_LEVEL_VARIANT)
                goalPos = altHomePos;
            else
                goalPos = homePos;
            targetPos = goalPos;
            break;
    }
}

/* 0x46C8D6 - Sam_FindFreeDropSpot; 1204 compared bytes including any local tables. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; pad150, pad26 fill gaps */
Vec3s Sam::FindFreeDropSpot(Vec3s point, CollBox *box)
{
    /* Work record preserves original local slots. XZ denotes the horizontal
     * plane; Vec3s::y is height and Vec3s::z is the second horizontal axis. */
    struct {
        CollBox *firstBox;
        ModelBoxList *boxes;
        unsigned short pad158, kind;
        ScnObject *object;
        short pad150;
        signed char zSign, zLastSign;
        int retries;
        signed char pad148[3], xSign;
        int dz;
        signed char pad140[3], xLastSign;
        int count, dx, outsideZ;
        ScnObject *nearby[65];
        Vec3s candidate;
        short pad26;
        int outsideX;
        CollBox homeBox, bounds;
    } w;
    w.candidate.x = 0;
    w.candidate.y = 0;
    w.candidate.z = 0;
    w.retries = 0;
    w.bounds.min.x = box->min.x + 300;
    w.bounds.min.y = box->min.y;
    w.bounds.min.z = box->min.z + 300;
    w.bounds.max.x = box->max.x - 300;
    w.bounds.max.y = box->max.y;
    w.bounds.max.z = box->max.z - 300;
    w.candidate.y = pos.y;
    w.homeBox.min.x = homePos.x - 200;
    w.homeBox.min.z = homePos.z - 200;
    w.homeBox.max.x = homePos.x + 200;
    w.homeBox.max.z = homePos.z + 200;
    while (1) {
        for (w.dx = 0;; w.dx += 200) {
            w.outsideX = 0;
            if (w.dx)
                w.xLastSign = 1;
            else
                w.xLastSign = 0;
            for (w.xSign = 0; w.xSign <= w.xLastSign; w.xSign++) {
                w.candidate.x = point.x + (w.xSign ? -w.dx : w.dx);
                if (w.candidate.x < w.bounds.min.x || w.candidate.x > w.bounds.max.x) {
                    w.outsideX++;
                    continue;
                }
                for (w.dz = 0;; w.dz += 200) {
                    if (w.dz)
                        w.zLastSign = 1;
                    else
                        w.zLastSign = 0;
                    w.outsideZ = 0;
                    for (w.zSign = 0; w.zSign <= w.zLastSign; w.zSign++) {
                        w.candidate.z = point.z + (w.zSign ? -w.dz : w.dz);
                        if (w.candidate.z < w.bounds.min.z || w.candidate.z > w.bounds.max.z) {
                            w.outsideZ++;
                            continue;
                        }
                        if (SamView_ContainsXZ(&w.candidate, &w.homeBox))
                            continue;
                        w.count = ObjGrid_QueryBoxesInRectXZ(w.candidate.x - 200, w.candidate.z - 200,
                                                             w.candidate.x + 200, w.candidate.z + 200, w.nearby);
                        while (w.count--) {
                            w.object = w.nearby[w.count];
                            w.kind = w.object->classId;
                            if (w.kind != CLASSID_SAM && !w.object->SamView_HasInstanceFlag(INST_F_ATTACHED)) {
                                w.boxes = w.object->inst_model->boxes;
                                if (w.boxes)
                                    w.firstBox = w.boxes->boxes;
                                else
                                    w.firstBox = 0;
                                if (w.firstBox)
                                    break;
                            }
                        }
                        if (w.count < 0) {
                            w.candidate.y = w.bounds.min.y;
                            w.candidate.y = QueryGroundY(&w.candidate, 0);
                            return w.candidate;
                        }
                    }
                    if (w.outsideZ > w.zLastSign)
                        break;
                }
            }
            if (w.outsideX > w.xLastSign) {
                if (w.retries == 1) {
                    point.y = w.bounds.min.y;
                    point.y = QueryGroundY(&point, 0);
                    return point;
                }
                w.retries++;
                point.x = (w.bounds.min.x + w.bounds.max.x) / 2;
                point.z = (w.bounds.min.z + w.bounds.max.z) / 2;
                break;
            }
        }
    }
}

/* 0x46CD8A - Sam_StartReactionCamera; 216 compared bytes including any local tables. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; unused6 fill gaps */
void Sam::StartReactionCamera()
{
    const short distance = 500;
    struct {
        short unused14, cameraYaw, samYaw, angle;
        Vec3s eye;
        short unused6;
    } w;
    w.eye = pos;
    w.samYaw = rot.y;
    w.angle = (-1024 - w.samYaw) & 4095;
    w.eye.y -= 180;
    w.eye.x += (short)((g_pCosTable[w.angle] * distance) >> 12);
    w.eye.z += (short)((g_sinTable4096[w.angle] * distance) >> 12);
    w.cameraYaw = rot.y;
    Camera_StartScripted(this, &g_camera, 0x80, -w.cameraYaw & 4095, 0, &w.eye, 0x6EA, CAMSCR_BLEND_IN, 0x1000);
}

/* 0x46CE62 - Sam_OnGoalReached; 2522 compared bytes including any local tables. */
/* BYTES(slot-group): locals grouped in points / drop only to pin the original frame offsets; pad0, pad1, pad2, pad, pad fill gaps */
/* BYTES(cast): the const-reference cast puts the pointer in a temporary, reproducing the original argument spill */
/* BYTES(slot-group): six-byte vectors kept in their original eight-byte slots */
/* BYTES(view): bit-view writes: the original changes only these bits of the word */
void Sam::OnGoalReached(int reached, int interrupted)
{
    /* Preserve independent six-byte vectors in their original eight-byte slots. */
    struct {
        Vec3s reference;
        short pad0;
        Vec3s candidate;
        short pad1;
        Vec3s putPos;
        short pad2;
    } points;
    switch (goalKind) {
        case SAM_GOAL_TARGET:
            if (this->state != SAM_STT_CATCHWOLF) {
                if (!(mode & SAM_MODE_NO_GIVEUP_OUTSIDE_ZONE)) {
                    if (PointInBoxListXZ(&g_pWolf->pos, authorizedZoneBoxes, authorizedZoneCount))
                        SetGoalWolf();
                    else {
                        SetGoalHome();
                        alertLevel = SAM_ALERT_NONE;
                    }
                } else
                    SetGoalWolf();
            } else {
                alertLevel = SAM_ALERT_NONE;
                SetGoalHome();
            }
            break;
        case SAM_GOAL_CARRY:
            points.reference = carryGoal.destination;
            if (interrupted == 1 && reached == 0) {
                struct {
                    Vec3s nearPoint;
                    short pad;
                    CollBox *box;
                } drop;
                drop.box = mode & SAM_MODE_LEVEL_VARIANT ? GetBeachCurrentBox() : catchableSheepBox;
                drop.nearPoint = NearestPointInBoxesXZ(&drop.box, 1, &pos, 300);
                chaseFlags |= SAM_CHASE_AFTER_DROP;
                carryGoal.destination = FindFreeDropSpot(drop.nearPoint, drop.box);
                points.reference = carryGoal.destination;
            }
            if (mode & SAM_MODE_LEVEL_VARIANT)
                points.candidate = FindFreeDropSpot(points.reference, GetBeachCurrentBox());
            else {
                CollBox *box;
                if (dontTryToCatchSheepBox)
                    box = dontTryToCatchSheepBox;
                else
                    box = catchableSheepBox;
                points.candidate = FindFreeDropSpot(points.reference, box);
            }
            if (points.candidate.x != points.reference.x || points.candidate.z != points.reference.z) {
                carryGoal.destination = points.candidate;
                UpdateGoalPos();
            } else if (interrupted != 1 || reached != 0) {
                SetState(SAM_STT_PUTDOWN);
                SetGoalHome();
            }
            break;
        case SAM_GOAL_FETCH:
            if (mode & SAM_MODE_LEVEL_VARIANT) {
                CollBox *box = beachCsBoxOther;
                points.putPos.x = (box->min.x + box->max.x) >> 1;
                points.putPos.y = (box->min.y + box->max.y) >> 1;
                points.putPos.z = (box->min.z + box->max.z) >> 1;
            } else
                points.putPos = putSheepPos;
            if (!fetchGoal.flags.bits.picked1) {
                if (fetchGoal.object1 == g_pWolf && !g_pWolf->HandleMessage(this, MSG_WOLF_IS_SHEEPCOSTUME, 0))
                    reached = 0;
                if (fetchGoal.object1->SamView_HasInstanceFlag(INST_F_ATTACHED))
                    reached = 0;
                if (reached) {
                    FreezePickupTarget(fetchGoal.object1);
                    fetchGoal.flags.all |= SAM_FETCH_PICKED1;
                } else {
                    if (mode & SAM_MODE_LEVEL_VARIANT)
                        SetGoalWolf();
                    else if (fetchGoal.object2) {
                        SamFetchGoal goal;
                        goal.object1 = fetchGoal.object2;
                        goal.object2 = 0;
                        /* Preserve the original two bit clears, including untouched upper bits. */
                        goal.flags.all &= ~SAM_FETCH_PICKED1;
                        goal.flags.all &= ~SAM_FETCH_PICKED2;
                        SetGoalFetch(&goal);
                    } else if ((mode & SAM_MODE_ALT_BEHAVIOUR) &&
                               !g_pWolf->HandleMessage(this, MSG_WOLF_IS_SHEEPCOSTUME, 0) &&
                               fetchGoal.object1 == g_pWolf)
                        SetGoalWolf();
                    else
                        SetGoalHome();
                }
            } else {
                if (fetchGoal.object2 == g_pWolf && !g_pWolf->HandleMessage(this, MSG_WOLF_IS_SHEEPCOSTUME, 0))
                    reached = 0;
                if (fetchGoal.object2 && fetchGoal.object2->SamView_HasInstanceFlag(INST_F_ATTACHED))
                    reached = 0;
                if (reached) {
                    FreezePickupTarget(fetchGoal.object2);
                    fetchGoal.flags.all |= SAM_FETCH_PICKED2;
                } else {
                    SamCarryGoal goal;
                    goal.object1 = fetchGoal.object1;
                    if (interrupted == 1) {
                        struct {
                            Vec3s nearPoint;
                            short pad;
                            CollBox *box;
                        } drop;
                        drop.box = mode & SAM_MODE_LEVEL_VARIANT ? GetBeachCurrentBox() : catchableSheepBox;
                        drop.nearPoint = NearestPointInBoxesXZ(&drop.box, 1, &pos, 300);
                        chaseFlags |= SAM_CHASE_AFTER_DROP;
                        goal.destination = FindFreeDropSpot(drop.nearPoint, drop.box);
                    } else {
                        CollBox *box;
                        if (dontTryToCatchSheepBox)
                            box = dontTryToCatchSheepBox;
                        else
                            box = catchableSheepBox;
                        /* The const-reference cast materializes a pointer value in
                     * a temporary, reproducing the original argument spill. */
                        goal.destination = FindFreeDropSpot(
                            points.putPos,
                            static_cast<CollBox *const &>(mode & SAM_MODE_LEVEL_VARIANT ? GetBeachCurrentBox() : box));
                    }
                    goal.object2 = 0;
                    SetGoalCarry(&goal);
                }
            }
            if (reached) {
                SetState(SAM_STT_PICKUP);
                if (!fetchGoal.object2 || fetchGoal.flags.bits.picked2) {
                    if (mode & SAM_MODE_LEVEL_VARIANT)
                        SwapBeachPositions();
                    SamCarryGoal goal;
                    goal.object1 = fetchGoal.object1;
                    CollBox *box;
                    if (dontTryToCatchSheepBox)
                        box = dontTryToCatchSheepBox;
                    else
                        box = catchableSheepBox;
                    /* Same argument-temporary lifetime as the failed second fetch. */
                    goal.destination = FindFreeDropSpot(
                        points.putPos,
                        static_cast<CollBox *const &>(mode & SAM_MODE_LEVEL_VARIANT ? GetBeachCurrentBox() : box));
                    goal.object2 = fetchGoal.object2;
                    SetGoalCarry(&goal);
                }
            }
            break;
        case SAM_GOAL_HOME:
            if (chaseBits.chaseAfterDrop) {
                chaseFlags &= ~SAM_CHASE_AFTER_DROP;
                if (PointInBoxListXYZ(&g_pWolf->pos, authorizedZoneBoxes, authorizedZoneCount) &&
                    PointInBoxListXYZ(&g_pWolf->pos, wolfCanBeHitZoneBoxes, wolfCanBeHitZoneCount))
                    SetGoalWolf();
                else
                    EnterDefaultState();
            } else
                EnterDefaultState();
            break;
    }
    UpdateGoalPos();
}

/* 0x46D83C - Sam_InitBeachVariant; 620 compared bytes including any local tables. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets */
void Sam::InitBeachVariant()
{
    struct {
        int orangePropertyOffset, greenPropertyOffset;
        CollBox **boxes;
        unsigned short unusedC, count;
        unsigned char *record;
        unsigned int property;
    } w;
    /* cast kept (w.record and its two property reads): designer properties are 4-byte slots at byte offsets of the raw
     * WAR record; (every (CollBox **) here): an export id list holds record pointers of any kind */
    w.record = (unsigned char *)this->record;
    w.boxes = (CollBox **)Scn_FindIdList(WAR_IDO_BEACHCSBOX1, &w.count);
    if (w.count != 1) {
        mode &= ~SAM_MODE_LEVEL_VARIANT;
        return;
    }
    beachCsBoxCur = *w.boxes;
    /* cast kept: an export id list holds its boxes' pointers as u32 words */
    w.boxes = (CollBox **)Scn_FindIdList(WAR_IDO_BEACHCSBOX2, &w.count);
    if (w.count != 1) {
        mode &= ~SAM_MODE_LEVEL_VARIANT;
        return;
    }
    beachCsBoxOther = *w.boxes;
    /* cast kept: an export id list holds its boxes' pointers as u32 words */
    beachOrangeZoneOther = (CollBox **)Scn_FindIdList(WAR_IDO_BEACHORANGEZONE2, &beachOrangeZoneOtherCount);
    if (beachOrangeZoneOtherCount == 0) {
        mode &= ~SAM_MODE_LEVEL_VARIANT;
        return;
    }
    /* cast kept: an export id list holds its boxes' pointers as u32 words */
    beachGreenZoneOther = (CollBox **)Scn_FindIdList(WAR_IDO_BEACHGREENZONE2, &beachGreenZoneOtherCount);
    if (beachGreenZoneOtherCount == 0) {
        mode &= ~SAM_MODE_LEVEL_VARIANT;
        return;
    }
    w.greenPropertyOffset = 0x2C;
    /* cast kept: a designer-property record read at its byte offset */
    w.property = *(unsigned int *)(w.record + w.greenPropertyOffset + 20);
    /* cast kept: an export id list holds its boxes' pointers as u32 words */
    greenZoneBoxes = (CollBox **)Scn_FindIdList((unsigned short)w.property, &greenZoneCount);
    w.orangePropertyOffset = 0x44;
    /* cast kept: a designer-property record read at its byte offset */
    w.property = *(unsigned int *)(w.record + w.orangePropertyOffset + 20);
    /* cast kept: an export id list holds its boxes' pointers as u32 words */
    orangeZoneBoxes = (CollBox **)Scn_FindIdList((unsigned short)w.property, &orangeZoneCount);
    w.boxes = (CollBox **)Scn_FindIdList(WAR_IDO_BEACHSAMIPOS2, &w.count);
    if (w.count != 1) {
        mode &= ~SAM_MODE_LEVEL_VARIANT;
        return;
    }
    beachHomePosOther.x = ((*w.boxes)->min.x + (*w.boxes)->max.x) >> 1;
    beachHomePosOther.y = ((*w.boxes)->min.y + (*w.boxes)->max.y) >> 1;
    beachHomePosOther.z = ((*w.boxes)->min.z + (*w.boxes)->max.z) >> 1;
    altHomePos = homePos;
    beachBits.onSide2 = 0;
}

/* 0x46DAA8 - Sam_SwapBeachPositions; 355 compared bytes including any local tables. */
/* BYTES(slot-group): locals grouped in swap only to pin the original frame offsets; pad0, pad1 fill gaps */
void Sam::SwapBeachPositions()
{
    /* Work record preserves the original local slots; names are analytical. */
    struct {
        Vec3s point;
        short pad0;
        CollBox **boxes;
        CollBox *box;
        short pad1;
        unsigned short count;
    } swap;
    beachBits.onSide2 = 1 - beachBits.onSide2;
    swap.box = beachCsBoxCur;
    beachCsBoxCur = beachCsBoxOther;
    beachCsBoxOther = swap.box;
    swap.point = altHomePos;
    altHomePos = beachHomePosOther;
    beachHomePosOther = swap.point;
    swap.boxes = orangeZoneBoxes;
    orangeZoneBoxes = beachOrangeZoneOther;
    beachOrangeZoneOther = swap.boxes;
    swap.boxes = greenZoneBoxes;
    greenZoneBoxes = beachGreenZoneOther;
    beachGreenZoneOther = swap.boxes;
    swap.count = orangeZoneCount;
    orangeZoneCount = beachOrangeZoneOtherCount;
    beachOrangeZoneOtherCount = swap.count;
    swap.count = greenZoneCount;
    greenZoneCount = beachGreenZoneOtherCount;
    beachGreenZoneOtherCount = swap.count;
}

/* 0x46DC0B - Sam_ResolveMove; 3455 compared bytes including any local tables. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; unused1DC, unused1D4, unused1A6, unused3A fill gaps */
unsigned short Sam::ResolveMove(Vec3s *delta, SamContactInfo *info, unsigned short normalCutoff, unsigned short mask,
                                Vec3s *startPos, CollBox *box)
{
    /* Local slots preserve the original incomplete box copy.
     * w is EBP-0x1E0; saved this is -0x1E4. */
    struct {
        unsigned int wallClassFlags;
        short unused1DC;
        unsigned short wallClassId;
        unsigned int floorClassFlags;
        short unused1D4;
        unsigned short floorClassId;
        ModelBoxList *boxList;
        int projection;
        unsigned char unused1C8[3], contactClass;
        CollisionScratch *scratch;
        unsigned char unused1C0, classes;
        unsigned short group;
        int pass;
        CollContact *contact;
        int i, biasCount;
        Vec3s start;
        short unused1A6;
        int height;
        CollBox worldBox;
        CollContact contacts[16];
        int unused50, auxHeight;
        unsigned char unused48[3], query;
        unsigned int budget;
        Vec3s remaining;
        short unused3A;
        int wallCount;
        Vec3i wallSum;
        int fraction, minHeight, count;
        Vec3s current;
        unsigned char unused16[5], result;
        CollBox defaultBox;
    } w;
    w.query = CQ_STATIC | CQ_OBJECTS | CQ_ASK_MOVER;
    mask &= (u16)~CQ_SAM_WALL_OVERLAP;
    if (mask & CQ_ASK_MOVER)
        w.query |= CQ_SAM_WALL_OVERLAP;
    if (!box) {
        w.boxList = inst_model->boxes;
        if (w.boxList)
            box = w.boxList->boxes;
        else
            box = 0;
        /* Original defect: defaultBox.flags is never initialized. */
        w.defaultBox.min = box->min;
        w.defaultBox.max.x = box->max.x;
        w.defaultBox.max.z = box->max.z;
        w.defaultBox.max.y = box->max.y;
        box = &w.defaultBox;
    }
    if (startPos)
        w.start = *startPos;
    else
        w.start = pos;
    if (mask == 0)
        w.budget = 1;
    else
        w.budget = 3;
    w.remaining.x = delta->x;
    w.remaining.y = delta->y;
    w.remaining.z = delta->z;
    w.current.x = w.start.x;
    w.current.y = w.start.y;
    w.current.z = w.start.z;
    w.worldBox.Box_Translate(box, &w.current);
    w.result = 0;
    w.wallSum.x = 0;
    w.wallSum.y = 0;
    w.wallSum.z = 0;
    w.wallCount = 0;
    if (info) {
        info->minStaticY = w.worldBox.max.y;
        info->minContactY = w.worldBox.max.y;
        info->boxTopY = w.worldBox.min.y;
        info->floorNormal.x = 0;
        info->floorNormal.y = -4096;
        info->floorNormal.z = 0;
        info->wallNormalMean.x = 0;
        info->wallNormalMean.y = 0;
        info->wallNormalMean.z = 0;
        info->floorObj = 0;
        info->wallObj = 0;
        info->carrierObj = 0;
    }
    do {
        --w.budget;
        w.count = Collide_SweepBox_Sam(this, &w.worldBox, &w.remaining, &w.fraction, &w.height, &w.minHeight,
                                       w.contacts, this, w.query, &w.auxHeight);
        if (w.count == 0) {
            w.current.x += w.remaining.x;
            w.current.y += w.remaining.y;
            w.current.z += w.remaining.z;
            break;
        }
        if (info) {
            if (w.height < info->minContactY)
                info->minContactY = (short)w.height;
            if (w.auxHeight < info->minStaticY)
                info->minStaticY = (short)w.auxHeight;
        }
        if (w.count >= 0x8000) {
            delta->z = 0;
            delta->y = 0;
            delta->x = 0;
            return 1;
        }
        /* cast kept: g_sharedScratch is one scratch buffer that each user lays out its own way */
        w.scratch = (CollisionScratch *)g_sharedScratch;
        if (w.fraction > 0) {
            w.scratch->step.x = (short)(w.remaining.x * w.fraction / 4096);
            w.scratch->step.y = (short)(w.remaining.y * w.fraction / 4096);
            w.scratch->step.z = (short)(w.remaining.z * w.fraction / 4096);
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
            if (w.contact->normal.y < -normalCutoff) {
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
                    if (info)
                        info->floorNormal = w.contact->normal;
                    w.scratch->contactClass[w.i] = 1;
                } else
                    w.scratch->contactClass[w.i] = 4;
                if (info && w.contact->obj) {
                    info->floorObj = w.contact->obj;
                    w.floorClassId = w.contact->obj->classId;
                    w.floorClassFlags = g_scenaricClassRegistry[w.floorClassId].classFlags;
                    if (w.floorClassFlags & SCN_CF_CARRIER)
                        info->carrierObj = w.contact->obj;
                }
            } else {
                w.scratch->contactClass[w.i] = 2;
                if (info) {
                    if (w.contact->obj) {
                        info->wallObj = w.contact->obj;
                        w.wallClassId = w.contact->obj->classId;
                        w.wallClassFlags = g_scenaricClassRegistry[w.wallClassId].classFlags;
                        if (w.wallClassFlags & SCN_CF_CARRIER)
                            info->carrierObj = w.contact->obj;
                    }
                    w.wallSum.x += w.contact->normal.x;
                    w.wallSum.y += w.contact->normal.y;
                    w.wallSum.z += w.contact->normal.z;
                    ++w.wallCount;
                }
            }
            w.classes |= w.scratch->contactClass[w.i];
        }
        if ((mask & w.classes & (COLL_FLOOR | COLL_FLOOR_EDGE)) && w.worldBox.max.y - w.height <= 10) {
            w.remaining.y = (short)(w.height - w.worldBox.max.y - 1);
            w.result |= COLL_FLOOR;
            if (info && info->floorNormal.y == -4096) {
                for (w.i = 0; w.i < w.count; ++w.i)
                    if (w.scratch->contactClass[w.i] & (COLL_FLOOR | COLL_FLOOR_EDGE))
                        info->floorNormal = w.contacts[w.i].normal;
            }
        } else if (mask & (w.classes & ~COLL_FLOOR_EDGE)) {
            w.scratch->projected.x = w.remaining.x << 10;
            w.scratch->projected.y = w.remaining.y << 10;
            w.scratch->projected.z = w.remaining.z << 10;
            w.group = 2;
            for (w.pass = 0; w.pass < 2; ++w.pass) {
                if (w.classes & w.group) {
                    w.scratch->normalSum.x = 0;
                    w.scratch->normalSum.y = 0;
                    w.scratch->normalSum.z = 0;
                    w.biasCount = 0;
                    for (w.i = 0; w.i < w.count; ++w.i) {
                        w.contactClass = w.scratch->contactClass[w.i];
                        if (mask & w.contactClass & w.group) {
                            w.contact = &w.contacts[w.i];
                            w.result |= w.contactClass;
                            w.projection = (w.contact->normal.x * w.scratch->projected.x +
                                            w.contact->normal.y * w.scratch->projected.y +
                                            w.contact->normal.z * w.scratch->projected.z) >>
                                           10;
                            w.scratch->projected.x -= (w.projection * w.contact->normal.x) >> 14;
                            if (!(mask & CQ_SAM_WALL_OVERLAP))
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
                        w.scratch->projected.x = w.scratch->normalSum.x / (w.biasCount << 2) + w.scratch->projected.x;
                        w.scratch->projected.y = w.scratch->normalSum.y / (w.biasCount << 2) + w.scratch->projected.y;
                        w.scratch->projected.z = w.scratch->normalSum.z / (w.biasCount << 2) + w.scratch->projected.z;
                    }
                }
                w.group = 1;
            }
            w.remaining.x = (short)(w.scratch->projected.x / 1024);
            w.remaining.y = (short)(w.scratch->projected.y / 1024);
            w.remaining.z = (short)(w.scratch->projected.z / 1024);
        } else {
            if (w.classes & COLL_FLOOR_EDGE) {
                if (w.worldBox.max.y <= w.height || (delta->x | delta->z) == 0) {
                    w.result |= (unsigned char)((w.classes & (u8)~COLL_FLOOR_EDGE) | COLL_FLOOR);
                    if (info && info->floorNormal.y == -4096) {
                        for (w.i = 0; w.i < w.count; ++w.i)
                            if (w.scratch->contactClass[w.i] & COLL_FLOOR_EDGE)
                                info->floorNormal = w.contacts[w.i].normal;
                    }
                } else
                    w.result |= (unsigned char)((w.classes & (u8)~COLL_FLOOR_EDGE) | COLL_WALL);
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
    delta->x = w.current.x - w.start.x;
    delta->y = w.current.y - w.start.y;
    delta->z = w.current.z - w.start.z;
    if (info && w.wallCount > 0) {
        info->wallNormalMean.x = (short)(w.wallSum.x / w.wallCount);
        info->wallNormalMean.y = (short)(w.wallSum.y / w.wallCount);
        info->wallNormalMean.z = (short)(w.wallSum.z / w.wallCount);
    }
    return w.result;
}

/* 0x46E98A - Sam_AddCarried; 60 compared bytes including any local tables. */
void Sam::AddCarried(ScnObject *object)
{
    carried[carriedCount] = object;
    ++carriedCount;
}

/* 0x46E9C6 - Sam_RemoveCarried; 153 compared bytes including any local tables. */
void Sam::RemoveCarried(ScnObject *object)
{
    int i;
    for (i = 0; i < carriedCount; ++i) {
        if (carried[i] == object) {
            for (; i < carriedCount - 1; ++i)
                carried[i] = carried[i + 1];
            --carriedCount;
            break;
        }
    }
}

/* 0x46EA5F - Sam_DropAllCarried; 105 compared bytes including any local tables. */
void Sam::DropAllCarried()
{
    if (carriedCount == 0)
        return;
    carryGoal.object1 = carried[0];
    carryGoal.object2 = carriedCount > 1 ? carried[1] : 0;
    PutDownCarried(1);
}

/* 0x46EAC8 - Sam_TurnYawToward; 132 compared bytes including any local tables. */
void Sam::TurnYawToward(short targetYaw)
{
    short delta;
    delta = (short)((targetYaw - GetYaw() + 2048) & 4095) - 2048;
    if (delta > 227)
        delta = 227;
    else if (delta < -227)
        delta = -227;
    SetYaw(delta + GetYaw());
}

/* 0x46EB4C - Sam_Stub_46eb4c; 13 compared bytes including any local tables. */
void Sam::Stub_46eb4c(short yaw, int unused) {}

/* 0x46EB59 - Sam_ClampPointToBoxXZ; 188 compared bytes including any local tables. */
Vec3s Sam::ClampPointToBoxXZ(CollBox *box, Vec3s *point)
{
    Vec3s w;
    if (point->x > box->max.x)
        w.x = box->max.x;
    else if (point->x < box->min.x)
        w.x = box->min.x;
    else
        w.x = point->x;
    if (point->z > box->max.z)
        w.z = box->max.z;
    else if (point->z < box->min.z)
        w.z = box->min.z;
    else
        w.z = point->z;
    w.y = point->y;
    return w;
}

/* 0x46EC15 - Sam_NearestPointInBoxesXZ; 327 compared bytes including any local tables. */
/* BYTES(slot-group): locals grouped in w only to pin the original frame offsets; unusedE, unused6 fill gaps */
Vec3s Sam::NearestPointInBoxesXZ(CollBox **boxes, short count, Vec3s *point, short margin)
{
    struct {
        CollBox box;
        short unused18, i;
        Vec3s best;
        short unusedE;
        Vec3s candidate;
        short unused6;
        int found;
    } w;
    w.found = 0;
    w.best = *point;
    for (w.i = 0; w.i < count; ++w.i) {
        w.box = *boxes[w.i];
        w.box.min.x += margin;
        w.box.min.z += margin;
        w.box.max.x -= margin;
        w.box.max.z -= margin;
        if (w.box.min.x < w.box.max.x && w.box.min.z < w.box.max.z) {
            w.candidate = ClampPointToBoxXZ(&w.box, point);
            if (w.found == 0) {
                w.best = w.candidate;
                w.found = 1;
            } else if (Vec3s_ManhattanDistXZ(&w.best, point) > Vec3s_ManhattanDistXZ(&w.candidate, point))
                w.best = w.candidate;
        }
    }
    return w.best;
}
