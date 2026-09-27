/* T139 - original object elastic.cpp (guessed name).
 * Ranges: .text 0x4b5950-0x4b8f12, .rdata 0x575f2c-0x575f5c (an unreferenced 8-byte main-CONST item, the vtable, then
 * the __real@c1400000 COMDAT this object emits first).
 * The functions are in the original address order, plus the unreferenced const. */
/* BYTES: dead-code, layout, slot-name, slot-scope, view. */
/* BYTES(layout): placeholder: unreferenced extern const that reproduces the bytes at 0x575f2c; type and name unknown */
/* Rgb24_Lerp and Math_Fixed12ToFloat_s16 are declared extern "C", and ScnObject::Scenaric_FindNearestOfClass with its
 * defined parameter types, so the decorated names agree at link. */
/* PAL PC elastic (0x4b5950-0x4b8e7a). */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
#include "../sdk/d3d7.h"
class Mat44;
#include "../sdk/crt.h"
#define SDW_MEMBERS_Mat44 Mat44();
#define SDW_MEMBERS_Mesh Mesh();
#define SDW_MEMBERS_RenderPoly RenderPoly(); /* virtual ~RenderPoly() is generated */
#define SDW_MEMBERS_D3DApp                                                  \
    void CreateVB(D3DVERTEXBUFFERDESC *desc, IDirect3DVertexBuffer7 **out); \
    void SetTransform(u32 state, Mat44 *m);
#define SDW_MEMBERS_ScnObject                                                                       \
    static void *operator new(u32 size);                                                            \
    s32 IsInScene()                                                                                 \
    {                                                                                               \
        return (flags & SCN_OF_IN_WORLD) != 0;                                                      \
    }                                                                                               \
    void SetUpdateMode(u8 mode);                                                                    \
    ScnObject *Scenaric_FindBestInRadius(Vec3s *, s16, s16, u16, u16 *,                             \
                                         u32 (*)(ScnObject *, ScnObject *, u32, ScnObject *), s32); \
    void AttachTo(ScnObject *, u8, Vec3s *, Vec3s *, u32, u32);
#include "sdw_classes.h"
#define SDW_INLINE_D3DAPP_CREATEVB_D3DVERTEXBUFFERDESC_IDIRECT3DVERTEXBUFFER7 3
#define SDW_INLINE_D3DAPP_SETTRANSFORM_U32_MAT44 1
#include "../app/d3dapp_inlines.h"
#undef SDW_INLINE_D3DAPP_CREATEVB_D3DVERTEXBUFFERDESC_IDIRECT3DVERTEXBUFFER7
#undef SDW_INLINE_D3DAPP_SETTRANSFORM_U32_MAT44
#define SDW_INLINE_SCNOBJECT_INSTFLAGS_U16 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_INSTFLAGS_U16
#define SDW_INLINE_SCNOBJECT_GETCLASSID 1
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_GETCLASSID
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_U8
extern Wolf *g_pWolf;
extern s32 g_dtMs;
extern "C" s16 g_sinTable4096[5122];
extern "C" const s16 *g_pCosTable;
extern "C" s16 Math_RadiansToAngle4096(float);
#include "../engine/fixed_math.h"
#include "../engine/lerp.h"
#include "../app/app_main.h"
#include "camera.h"
#include "../engine/tex_table.h"
#include "../engine/draw2d.h"
#include "../engine/id_list.h"
extern "C" u32 Rgb24_Lerp(u32, u32, s16);
void Camera_StartScripted(ScnObject *, Camera *, u16, u16, u16, Vec3s *, u16, u32, s32);
u16 Sound_Play(u16, void *, u16, u8, s32);
inline u32 ElasticProperty(void *record, u32 offset)
{
    u32 field = offset;
    u32 number = *(u32 *)((u8 *)record + field + 0x14); /* cast kept: a property is a 4-byte slot of the raw record */
    return number;
}
inline u32 ElasticPropertyRaw(void *record, u32 offset)
{
    return *(u32 *)((u8 *)record + offset + 0x14); /* cast kept: a property is a 4-byte slot of the raw record */
}
/* 0x575f2c .rdata (main CONST, ahead of the vtable): 00 06 00 00 00 00 (+2 bytes of alignment pad). Nothing in the
 * image refers to it; an unreferenced const is only emitted with external linkage, so the original had an extern
 * const here. Its type and name are unknown: a Vec3s {0x600, 0, 0} reproduces the bytes (descriptive name). */
extern const Vec3s g_elasticUnrefConst = {0x600, 0, 0};
u32 elastic_LaunchTargetScoreCB(ScnObject *self, ScnObject *candidate, u32 score,
                                ScnObject *selected); /* defined after Update */
void Vec3s_ScaleByRatio(Vec3s *out, const Vec3s *v, s16 num, s16 den)
{
    s32 ratio = (num << 12) / den;
    out->x = v->x * ratio / 4096;
    out->y = v->y * ratio / 4096;
    out->z = v->z * ratio / 4096;
}
ScnObject *elastic_Create(void *record)
{
    elastic *object = new elastic;
    object = (elastic *)object->Init(record, 0); /* cast kept: Init returns the object as its ScnBody base */
    return object;
}
void elastic::AddWolfRider()
{
    if (!isWolfRider) {
        g_pWolf->HandleMessage(this, MSG_RIDER_ADD, 0);
        isWolfRider = 1;
    }
}
void elastic::RemoveWolfRider()
{
    if (isWolfRider) {
        g_pWolf->HandleMessage(this, MSG_RIDER_REMOVE, 0);
        isWolfRider = 0;
    }
}
void elastic::ResetToItem()
{
    SetBandMode(0);
    tension = 0;
    launchTension = 0;
    holdMs = 0;
    overstretched = 0;
    unk2e5 = 0;
    tree1 = 0;
    tree2 = 0;
    camRestrict1 = 0;
    camRestrict2 = 0;
    engaged = 0;
    engagedSide = ELASTIC_SIDE_NONE;
    isWolfRider = 0;
    wolfFrozen = 0;
    state = ELASTIC_ST_ITEM;
    subState = ELASTIC_SUB_FOLLOW;
}
void elastic::Reset()
{
    if (camRestrict1)
        camRestrict1->HandleMessage(this, MSG_CAMRESTRICT_ENABLE, 0);
    if (camRestrict2)
        camRestrict2->HandleMessage(this, MSG_CAMRESTRICT_ENABLE, 0);
    if (state < ELASTIC_ST_STRUNG) {
        ResetToItem();
        if (IsInScene())
            SetPosition(&homePos);
    } else
        state = ELASTIC_ST_SNAP_END;
    wolfPrevPos.x = 0;
    wolfPrevPos.y = 0;
    wolfPrevPos.z = 0;
    wolfFrozen = 0;
}
/* BYTES(view, inferred): two rotation cells written with one 32-bit store, as the original */
void elastic::BuildBandFrame()
{
    s32 e = anchorB.x - anchorA.x;
    s32 b = anchorB.z - anchorA.z;
    s16 c = (Math_RadiansToAngle4096((float)atan2(b, e)) + 0x800) & 0xfff;
    s16 d = g_pCosTable[c];
    s16 a = g_sinTable4096[c];
    /* cast kept (four dword stores): the original writes whole dwords, so each sign-extends into the next element */
    *(s32 *)&bandFrame.rot[0] = d;
    *(s32 *)&bandFrame.rot[2] = a;
    *(s32 *)&bandFrame.rot[4] = 0x1000;
    /* cast kept: two s16 matrix cells stored as one s32 */
    *(s32 *)&bandFrame.rot[6] = (s16)(-a & 0xffff);
    bandFrame.rot[8] = d;
    bandFrame.trans[0] = 0;
    bandFrame.trans[1] = 0;
    bandFrame.trans[2] = 0;
}
s32 elastic::TrackWolfCrossing(const Vec3s *step)
{
    Vec3s a;
    s8 b;
    s8 c;
#define next a
#define oldSide b
#define newSide c
    next.x = g_pWolf->pos.x + step->x;
    next.y = g_pWolf->pos.y + step->y;
    next.z = g_pWolf->pos.z + step->z;
    if (!engaged) {
        oldSide = ProjectOntoBand(&g_pWolf->pos);
        newSide = ProjectOntoBand(&next);
        if (oldSide == -newSide && next.y > anchorA.y && next.y < anchorA.y + 0xa0) {
            engaged = 1;
            engagedSide = newSide;
            SetPosition(&next);
            return 1;
        }
    } else {
        newSide = ProjectOntoBand(&next);
        if (engagedSide == -newSide) {
            engaged = 0;
            tension = 0;
            return 0;
        }
        SetPosition(&next);
        return 1;
    }
    return 0;
#undef next
#undef oldSide
#undef newSide
}
s32 elastic::ApplyTension(const Vec3s *amount, const Vec3s *velocity, Vec3s *step)
{
    s16 a;
    s16 b;
    Vec3s c;
    Vec3s d;
#define len a
#define speed b
#define combined c
#define force d
    combined.x = amount->x + step->x;
    combined.y = amount->y + step->y;
    combined.z = amount->z + step->z;
    len = (s16)sqrt((double)Vec3s_LengthSq(&combined));
    speed = (s16)sqrt((double)Vec3s_LengthSq(velocity));
    if (len > LONGUEUR_MAX) {
        Vec3s_ScaleByRatio(&combined, &combined, LONGUEUR_MAX, len);
        if (tree1 && !tree2)
            Vec3s_ScaleByRatio(step, &combined, -20, speed);
        else {
            step->x = 0;
            step->z = 0;
            len = LONGUEUR_MAX;
        }
        overstretched = 1;
    } else if (len > LONGUEUR_MAX / 2) {
        overstretched = 0;
        tension = (len - LONGUEUR_MAX / 2) * 4096 / (LONGUEUR_MAX / 2);
        if (speed < 600)
            speed = len - LONGUEUR_MAX / 2;
        else
            speed = speed * tension / 4096;
        speed = ((speed << 10) / 1000) * g_dtMs / 1024;
        Vec3s_ScaleByRatio(&force, &combined, speed, len);
        step->x -= force.x;
        step->y -= force.y;
        step->z -= force.z;
    }
    tension = (len - LONGUEUR_MAX / 2) * 4096 / (LONGUEUR_MAX / 2);
    if (tension < 0)
        tension = 0;
    return len;
#undef len
#undef speed
#undef combined
#undef force
}
void elastic::UpdateTensionColour()
{
    u16 i;
    bandColour = Rgb24_Lerp(0x10040ff, 0x10000d0, tension);
    for (i = 0; i < 30; i++)
        /* cast kept: the table is written at a two-dword stride; a u32[30][2] field would overlap bandMeshKey by 4 bytes */
        ((u32(*)[2])tensionColourTable)[i][0] = bandColour;
    if (tension > 0)
        tension -= 0x40;
    if (tension < 0)
        tension = 0;
}
void elastic::SetupLaunch(s32 baseY, const Vec3s *amount, s32 height, s32 distance, s32 speed)
{
    s16 len = (s16)sqrt((double)Vec3s_LengthSq(amount));
    launchDir.x = -(amount->x * 4096) / len;
    launchDir.y = 0;
    launchDir.z = -(amount->z * 4096) / len;
    launchTravelled = 0;
    launchSpeed = (speed << 10) / 1000;
    launchAnglePerUnit = 0x800000 / distance;
    launchHeight = height;
    launchBaseY = baseY;
}
void elastic::StepLaunch(Vec3s *out)
{
    s32 step = launchSpeed * g_dtMs / 1024;
    launchTravelled += step;
    out->y = launchBaseY - launchHeight * g_sinTable4096[launchTravelled * launchAnglePerUnit / 4096] / 4096;
    out->x = launchDir.x * step / 4096;
    out->z = launchDir.z * step / 4096;
}
void elastic::StartVibration(const Vec3s *from, const Vec3s *centre, s32 rate)
{
    vibFrom.x = from->x;
    vibFrom.y = from->y;
    vibFrom.z = from->z;
    vibTo.x = 2 * centre->x - vibFrom.x;
    vibTo.y = 2 * centre->y + 0xa0 - vibFrom.y;
    vibTo.z = 2 * centre->z - vibFrom.z;
    vibT = 0;
    vibRate = (rate << 12) / 1000;
    vibAmp = 0x800;
}
s32 elastic::StepVibration(Vec3s *out)
{
    vibT += (s16)(vibRate * g_dtMs / 4096);
    if (vibAmp <= 0)
        return 0;
    if ((vibT - 0x800 >= 0 ? vibT - 0x800 : -(vibT - 0x800)) >= vibAmp) {
        vibRate = -vibRate;
        vibAmp = vibAmp * 3 / 4;
        vibT = (vibT - 0x800 >= 0 ? 1 : -1) * vibAmp + 0x800;
    }
    Lerp_SetVecTarget(&vibTo);
    Vec3s_LerpToTarget(out, &vibFrom, vibT);
    return 1;
}
void elastic::StartPullUp(const Vec3s *start, s32 speed)
{
    s32 drop = (start->y - anchorA.y) * 1000 / speed;
    pullUpAngle = 0;
    pullUpRate = 0x800000 / drop;
    pullUpStart = *start;
}
s32 elastic::StepPullUp(Vec3s *out)
{
    s16 c = 0x800 - g_pCosTable[pullUpAngle] / 2;
    pullUpAngle += (s16)(pullUpRate * g_dtMs / 4096);
    Lerp_SetVecTarget(pullUpTarget);
    Vec3s_LerpToTarget(out, &pullUpStart, c);
    if (pullUpAngle >= 0xa00) {
        pullUpAngle = 0;
        return 0;
    }
    return 1;
}
/* BYTES(dead-code): f is computed and never read, as in the original */
void elastic::AimCamera(const Vec3s *target, CamSetup *setup, u32 mode)
{
    s32 c = target->x - setup->eye.x;
    s32 b = target->y - setup->eye.y;
    s32 a = target->z - setup->eye.z;
    s32 g = c * c;
    s32 f = b * b;
    s32 e = a * a;
    Vec3s d;
    u16 h;
    d.x = Math_RadiansToAngle4096((float)atan2(b, (s32)sqrt((double)g + e))) & 0xfff;
    d.y = Math_RadiansToAngle4096((float)atan2(-c, a)) & 0xfff;
    d.z = 0;
    h = setup->focal;
    Camera_StartScripted(this, &g_camera, d.x, d.y, d.z, &setup->eye, h, mode, 0x1000);
}
void elastic::SetWolfFrozen(s32 freeze)
{
    s32 result;
    if (freeze && !wolfFrozen) {
        result = g_pWolf->HandleMessage(this, MSG_FREEZE, 0);
        if (result)
            wolfFrozen = 1;
    }
    if (!freeze && wolfFrozen) {
        result = g_pWolf->HandleMessage(this, MSG_UNFREEZE, 0);
        wolfFrozen = 0;
    }
}
void elastic::Update()
{
    Vec3s a;
    Vec3s b;
    ContactInfo c;
    u16 d;
    ScnObject *e = 0;
    u16 f;
    Vec3s g;
    switch (state) {
        case ELASTIC_ST_TIED_ONE:
            wolfPrevPos.x = g_pWolf->pos.x;
            wolfPrevPos.y = g_pWolf->pos.y;
            wolfPrevPos.z = g_pWolf->pos.z;
            break;
        case ELASTIC_ST_SNAP:
            if (launched) {
                if (camera && launched != g_pWolf) {
                    SetWolfFrozen(1);
                    AimCamera(&launched->pos, camera, CAMSCR_BLEND_OUT);
                }
                if (launchTravelled < DISTANCE_PROJECTION * launchTension / 4096) {
                    StepLaunch(&g);
                    g.y -= launched->pos.y;
                    a.x = g.x / 3;
                    a.y = g.y / 3;
                    a.z = g.z / 3;
                    b = a;
                    g.x -= (s16)(a.x * 2);
                    g.y -= (s16)(a.y * 2);
                    g.z -= (s16)(a.z * 2);
                    d = launched->Collide_ResolveMove(&g, &c, 0x578, 0, 0, 0, 10, 0, 0);
                    g.x += launched->pos.x;
                    g.y += launched->pos.y;
                    g.z += launched->pos.z;
                    d |= launched->Collide_ResolveMove(&a, &c, 0x578, 0, &g, 0, 10, 0, 0);
                    g.x += a.x;
                    g.y += a.y;
                    g.z += a.z;
                    d |= launched->Collide_ResolveMove(&b, &c, 0x578, 0, &g, 0, 10, 0, 0);
                    g.x += b.x;
                    g.y += b.y;
                    g.z += b.z;
                    if (!d) {
                        launched->SetPosition(&g);
                        if (!launched->GetClassId())
                            launched->HandleMessage(this, MSG_WOLF_FORCE_FALL, 0);
                    } else {
                        launched->HandleMessage(this, MSG_LANDED, (void *)1); /* cast kept: an argument is a void * */
                        state = ELASTIC_ST_SNAP_END;
                    }
                } else {
                    launched->HandleMessage(this, MSG_LANDED, (void *)1); /* cast kept: an argument is a void * */
                    state = ELASTIC_ST_SNAP_END;
                }
            } else if (launchTension && (e = Scenaric_FindBestInRadius(&pos, pos.y - 0x80, pos.y + 0x7f, 0x50, &f,
                                                                       elastic_LaunchTargetScoreCB, 0))) {
                launched = e;
                SetupLaunch(launched->pos.y, &stretch, (s16)(HAUTEUR_PROJECTION * launchTension / 4096),
                            (s16)(DISTANCE_PROJECTION * launchTension / 4096), VITESSE_PROJECTION);
            }
            if (StepVibration(&g))
                SetPosition(&g);
            else if (!launched)
                state = ELASTIC_ST_SNAP_END;
            UpdateBandMesh(&pos, 1);
            break;
        case ELASTIC_ST_SNAP_END:
            if (camera && launched != g_pWolf && launched) {
                SetWolfFrozen(0);
                if (launched != g_pWolf)
                    Camera_ReleaseAny();
            }
            engaged = 0;
            engagedSide = ELASTIC_SIDE_NONE;
            AddWolfRider();
            state = ELASTIC_ST_STRUNG;
            break;
        case ELASTIC_ST_STRUNG_IDLE:
            UpdateBandMesh(&tree2->pos, 0);
            break;
    }
    UpdateTensionColour();
}
struct ElasticMove {
    Vec3s step;
    u16 grounded : 1;
    u16 pushing : 1;
    Vec3s velocity;
};
struct ElasticTreeReply {
    Vec3s *point;
    ScnObject *restriction;
};
struct ElasticGrabArgs {
    u32 engaged;
    Vec3s *a;
    Vec3s *b;
};
u32 elastic_LaunchTargetScoreCB(ScnObject *self, ScnObject *candidate, u32 score, ScnObject *selected)
{
    if (candidate != g_pWolf && candidate->InstFlags(INST_F_ANIMATED)) {
        ModelBoxList *boxes = candidate->inst_model->boxes;
        if (boxes ? &boxes->boxes[0] : 0)
            return score;
    }
    return 0xffffffffU;
}
/* BYTES(dead-code): c and g are never used: slots the original frame has */
s32 elastic::HandleMessage(ScnObject *sender, u32 message, void *arg)
{
    Vec3s *a;
    ScnObject *b;
    s32 c;
    Vec3s d;
    ElasticGrabArgs e;
    ElasticMove *f = (ElasticMove *)arg; /* cast kept: MSG_MODIFY_MOVE passes the move in the void * argument */
    s16 g;
    Vec3s h;
    Vec3s i;
    u8 j;
    Vec3s k;
    ElasticTreeReply l;
    s8 m;
    if (sender)
        k = sender->pos;
    switch (message) {
        case MSG_FREEZE:
            if (sender->GetClassId() == CLASSID_WOLF)
                wolfFrozen = 0;
            break;
        case MSG_QUERY_ACTION:
            if (sender->GetClassId() == CLASSID_WOLF) {
                switch (state) {
                    case ELASTIC_ST_ITEM:
                        return CTX_PICKUP;
                    case ELASTIC_ST_HELD:
                        return CTX_USE;
                    case ELASTIC_ST_TIED_ONE:
                        tree2 =
                            Scenaric_FindNearestOfClass(&k, CLASSID_ELASTICTREE, k.y - 0x80, k.y + 0x7f, 0x50, 0, 0);
                        if (tree2) {
                            if (tree2 != tree1)
                                return CTX_ELASTIC_TIE;
                            return CTX_ELASTIC_TAKE_BACK;
                        }
                        return CTX_NONE;
                    case ELASTIC_ST_STRUNG:
                        b = Scenaric_FindNearestOfClass(&k, CLASSID_ELASTICTREE, k.y - 0x80, k.y + 0x7f, 0x50, 0, 0);
                        if (b == tree1 || b == tree2) {
                            m = ProjectOntoBand(&k);
                            if (m != ELASTIC_SIDE_NONE)
                                return CTX_ELASTIC_GRAB;
                            return CTX_NONE;
                        } else if (perpDist < 100 || engaged)
                            return CTX_ELASTIC_PULL;
                        break;
                    case ELASTIC_ST_PULLED:
                        return CTX_ELASTIC_RELEASE;
                }
            }
            break;
        case MSG_USE:
            if (sender->GetClassId() == CLASSID_WOLF) {
                switch (state) {
                    case ELASTIC_ST_TIED_ONE:
                        if (tree2 == tree1) {
                            if (camRestrict1)
                                camRestrict1->HandleMessage(this, MSG_CAMRESTRICT_ENABLE, 0);
                            RemoveWolfRider();
                            SetBandMode(0);
                            state = ELASTIC_ST_ITEM;
                            return 1;
                        } else if (tree2) {
                            tree2->HandleMessage(this, MSG_ELASTICTREE_GET_INFO, &l);
                            camRestrict2 = l.restriction;
                            pullUpTarget = l.point;
                            if (camRestrict1)
                                camRestrict1->HandleMessage(this, MSG_CAMRESTRICT_ENABLE, 0);
                            anchorB.x = tree2->pos.x;
                            anchorB.y = tree2->pos.y - 0x50;
                            anchorB.z = tree2->pos.z;
                            BuildBandFrame();
                            state = ELASTIC_ST_STRUNG;
                            return 1;
                        }
                        break;
                    case ELASTIC_ST_STRUNG:
                        b = Scenaric_FindNearestOfClass(&k, CLASSID_ELASTICTREE, k.y - 0x80, k.y + 0x7f, 0x50, 0, 0);
                        if (b == tree1) {
                            if (camRestrict2) /* cast kept: an argument is a void * */
                                camRestrict2->HandleMessage(this, MSG_CAMRESTRICT_ENABLE, (void *)1);
                            anchorA.x = anchorB.x;
                            anchorA.y = anchorB.y;
                            anchorA.z = anchorB.z;
                            tree1 = tree2;
                            tree2 = 0;
                            state = ELASTIC_ST_TIED_ONE;
                            return 1;
                        } else if (b == tree2) {
                            if (camRestrict1) /* cast kept: an argument is a void * */
                                camRestrict1->HandleMessage(this, MSG_CAMRESTRICT_ENABLE, (void *)1);
                            tree2 = 0;
                            state = ELASTIC_ST_TIED_ONE;
                            return 1;
                        } else {
                            e.engaged = engaged;
                            e.a = &anchorA;
                            e.b = &anchorB;
                            g_pWolf->HandleMessage(this, MSG_SET_ACTION_HEADING, &e);
                            state = ELASTIC_ST_PULLED;
                            return 1;
                        }
                        break;
                    case ELASTIC_ST_PULLED:
                        if (!engaged) {
                            bandMid.x = (anchorA.x + anchorB.x) / 2;
                            bandMid.y = (anchorA.y + anchorB.y) / 2;
                            bandMid.z = (anchorA.z + anchorB.z) / 2;
                            launchTension = tension;
                            StartVibration(&k, &bandMid, 0x2000);
                            RemoveWolfRider();
                            launched = 0;
                            state = ELASTIC_ST_SNAP;
                            Sound_Play(SND_SEAELAST, this, 0xff, SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 0x1000);
                        } else
                            state = ELASTIC_ST_STRUNG;
                        return 1;
                    default:
                        return 0;
                }
            }
            break;
        case MSG_PICKUP:
            AttachTo(sender, (u8)(u32)arg, 0, 0, 0, 0); /* cast kept: MSG_PICKUP passes the joint in the void * */
            state = ELASTIC_ST_HELD;
            return 1;
        case MSG_DROP:
            a = (Vec3s *)arg; /* cast kept: MSG_DROP passes the drop point in the void * argument */
            Detach();
            SetPosition(a);
            state = ELASTIC_ST_HELD;
            return 1;
        case MSG_QUERY_HELD_ACTION:
            if (state == ELASTIC_ST_HELD) {
                tree1 = Scenaric_FindNearestOfClass(&k, CLASSID_ELASTICTREE, k.y - 0x80, k.y + 0x7f, 0x50, 0, 0);
                if (tree1)
                    return HELD_DROP_IN_PLACE;
            }
            return HELD_NONE;
        case MSG_INVENTORY_STORED:
            if (state == ELASTIC_ST_TIED_ONE && tree2 == tree1) {
                if (camRestrict1)
                    camRestrict1->HandleMessage(this, MSG_CAMRESTRICT_ENABLE, 0);
                RemoveWolfRider();
                SetBandMode(0);
                state = ELASTIC_ST_ITEM;
                return 1;
            }
            break;
        case MSG_INVENTORY_TAKE_OUT:
            return 1;
        case MSG_MODIFY_MOVE:
            if (sender->GetClassId() == CLASSID_WOLF) {
                switch (state) {
                    case ELASTIC_ST_TIED_ONE:
                        stretch.x = k.x - anchorA.x;
                        stretch.y = k.y - anchorA.y;
                        stretch.z = k.z - anchorA.z;
                        switch (subState) {
                            case ELASTIC_SUB_WOLF_INTERACTING:
                                overstretched = 0;
                                d.x = k.x + f->step.x;
                                d.y = k.y + f->step.y;
                                d.z = k.z + f->step.z;
                                UpdateBandMesh(&d, 0);
                                SetPosition(&d);
                                if (!f->pushing) {
                                    i.x = stretch.x + f->step.x;
                                    i.y = stretch.y + f->step.y;
                                    i.z = stretch.z + f->step.z;
                                    g = (s16)sqrt((double)Vec3s_LengthSq(&i));
                                    if (pos.y - 300 > anchorA.y) {
                                        StartPullUp(&pos, 0x266);
                                        launchTension = tension;
                                        subState = ELASTIC_SUB_PULL_UP;
                                        Sound_Play(SND_SEAELAST, this, 0xff, SNDF_POSITIONAL | SNDF_NO_RETRIGGER,
                                                   0x1000);
                                    } else
                                        subState = ELASTIC_SUB_FOLLOW;
                                }
                                break;
                            case ELASTIC_SUB_FOLLOW:
                                d.x = k.x - wolfPrevPos.x;
                                d.y = k.y - wolfPrevPos.y;
                                d.z = k.z - wolfPrevPos.z;
                                f->step.x += d.x;
                                f->step.y += d.y;
                                f->step.z += d.z;
                                c = ApplyTension(&stretch, &f->velocity, &f->step);
                                f->step.x -= d.x;
                                f->step.y -= d.y;
                                f->step.z -= d.z;
                                if (f->pushing) {
                                    f->step.x = f->step.z = 0;
                                    subState = ELASTIC_SUB_WOLF_INTERACTING;
                                }
                                if (overstretched && !f->grounded && pos.y - 300 > anchorA.y) {
                                    f->step.x = 0;
                                    f->step.y = 0;
                                    f->step.z = 0;
                                }
                                d.x = k.x + f->step.x;
                                d.y = k.y + f->step.y;
                                d.z = k.z + f->step.z;
                                UpdateBandMesh(&d, 0);
                                SetPosition(&d);
                                if (pos.y - 300 > anchorA.y && overstretched) {
                                    StartPullUp(&pos, 0x266);
                                    launchTension = tension;
                                    subState = ELASTIC_SUB_PULL_UP;
                                    Sound_Play(SND_SEAELAST, this, 0xff, SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 0x1000);
                                }
                                break;
                            case ELASTIC_SUB_PULL_UP:
                                if (!StepPullUp(&h)) {
                                    overstretched = 0;
                                    subState = ELASTIC_SUB_FOLLOW;
                                }
                                f->step.x = h.x - k.x;
                                f->step.y = h.y - k.y;
                                f->step.z = h.z - k.z;
                                SetPosition(&h);
                                UpdateBandMesh(&pos, 0);
                                break;
                        }
                        return 1;
                    case ELASTIC_ST_STRUNG:
                        stretch.x = k.x * 2 - (anchorA.x + anchorB.x);
                        stretch.y = k.y * 2 - (anchorA.y + anchorB.y);
                        stretch.z = k.z * 2 - (anchorA.z + anchorB.z);
                        if (TrackWolfCrossing(&f->step)) {
                            if (!f->grounded && tension) {
                                f->step.x /= 4;
                                f->step.z /= 4;
                            }
                            ApplyTension(&stretch, &f->velocity, &f->step);
                            UpdateBandMesh(&pos, 1);
                            if (tension && perpDist > 100) {
                                if (tension >= 0xe00)
                                    holdMs += (s16)g_dtMs;
                                else
                                    holdMs = 0;
                                if ((!f->grounded || holdMs >= 1000) &&
                                    (sender->GetClassId() != CLASSID_WOLF ||
                                     !sender->HandleMessage(this, MSG_WOLF_IS_DEAD, 0))) {
                                    if (sender->GetClassId() == CLASSID_WOLF)
                                        sender->HandleMessage(this, MSG_WOLF_FORCE_FALL, 0);
                                    holdMs = 0;
                                    launched = sender;
                                    RemoveWolfRider();
                                    launchTension = tension;
                                    SetupLaunch(launched->pos.y, &stretch,
                                                (s16)(HAUTEUR_PROJECTION * launchTension / 4096),
                                                (s16)(DISTANCE_PROJECTION * launchTension / 4096), VITESSE_PROJECTION);
                                    bandMid.x = (anchorA.x + anchorB.x) / 2;
                                    bandMid.y = (anchorA.y + anchorB.y) / 2;
                                    bandMid.z = (anchorA.z + anchorB.z) / 2;
                                    StartVibration(&pos, &bandMid, 0x2000);
                                    state = ELASTIC_ST_SNAP;
                                    Sound_Play(SND_SEAELAST, this, 0xff, SNDF_POSITIONAL | SNDF_NO_RETRIGGER, 0x1000);
                                }
                            }
                        } else
                            UpdateBandMesh(&tree2->pos, 0);
                        return 1;
                    case ELASTIC_ST_PULLED:
                        TrackWolfCrossing(&f->step);
                        stretch.x = k.x * 2 - (anchorA.x + anchorB.x);
                        stretch.y = k.y * 2 - (anchorA.y + anchorB.y);
                        stretch.z = k.z * 2 - (anchorA.z + anchorB.z);
                        ApplyTension(&stretch, &f->velocity, &f->step);
                        d.x = k.x + f->step.x;
                        d.y = k.y + f->step.y;
                        d.z = k.z + f->step.z;
                        UpdateBandMesh(&d, 1);
                        SetPosition(&d);
                        return 1;
                }
            }
            break;
        case MSG_HELD_STATE_BEGIN:
            switch (state) {
                case ELASTIC_ST_HELD:
                    tree1->HandleMessage(this, MSG_ELASTICTREE_GET_INFO, &l);
                    camRestrict1 = l.restriction;
                    pullUpTarget = l.point;
                    if (camRestrict1) /* cast kept: an argument is a void * */
                        camRestrict1->HandleMessage(this, MSG_CAMRESTRICT_ENABLE, (void *)1);
                    anchorA.x = tree1->pos.x;
                    anchorA.y = tree1->pos.y - 0x50;
                    anchorA.z = tree1->pos.z;
                    SetBandMode(1);
                    UpdateBandMesh(&anchorA, 0);
                    AddWolfRider();
                    state = ELASTIC_ST_TIED_ONE;
                    return 1;
            }
            return 0;
        case MSG_CONTAINER_STATE:
            switch ((u32)arg) { /* cast kept: the container state travels in the void * argument */
                case CONTAINER_RELEASED:
                    homePos = pos;
                    break;
            }
            return 1;
        case MSG_CHECKPOINT_ROLLBACK:
        case MSG_INVENTORY_STORE_BEGIN:
            Reset();
            return 1;
        case MSG_WOLF_RIDER_NOTIFY:
            j = state;
            if (j != ELASTIC_ST_TIED_ONE)
                Reset();
            if (j == ELASTIC_ST_STRUNG)
                state = ELASTIC_ST_STRUNG_IDLE;
            return 1;
    }
    return 0;
}
void elastic::Render(Camera *view)
{
    if (flags & ELASTIC_F_BAND_VISIBLE)
        RenderBand(view);
    else
        ScnBody::Render(view);
}
/* BYTES(slot-name): names chosen for their buckets (p 0, q 1, five in 2, reverse declaration order): all seven stay before the new[] count temp $S1 (bucket 3) */
void elastic::PostLoadInit()
{
    /* VC6 hash buckets: p=0, q=1; the remaining five=2 (reverse declaration order). */
    /* Keeps all seven locals before the compiler's new[] count temporary ($S1, bucket 3). */
    D3DVERTEXBUFFERDESC p;
    void *q;
    u32 *tk;
    Mesh *so;
    u32 rs;
    u16 rc;
    u32 r;
#define a p
#define b q
#define c r
#define d rc
#define e rs
#define f so
#define g tk
    g_texObjects[g_texCount] = new Mesh;
    f = g_texObjects[g_texCount];
    f->app = g_pD3DAppMain;
    f->flags = 3;
    f->vertexCount = 6;
    memset(&a, 0, 16);
    a.dwSize = 16;
    a.dwCaps = D3DVBCAPS_DONOTCLIP;
    a.dwFVF = D3DFVF_XYZ;
    a.dwNumVertices = f->vertexCount;
    g_pD3DAppMain->CreateVB(&a, &f->vbPositions);
    memset(&a, 0, 16);
    a.dwSize = 16;
    a.dwCaps = D3DVBCAPS_DONOTCLIP;
    a.dwFVF = D3DFVF_XYZRHW;
    a.dwNumVertices = f->vertexCount;
    g_pD3DAppMain->CreateVB(&a, &f->vbTransformed);
    g_texKeys[g_texCount] = (u32)bandMeshKey; /* cast kept: the texture key table holds addresses as u32 */
    g_texCount++;
    itemModel = inst_model;
    memcpy(bandMeshKey, itemModel, 16);
    f->faceCount = 8;
    f->faces = new RenderPoly[f->faceCount];
    for (e = 0; e < 8; e++) {
        f->faces[e].type = RPOLY_OPAQUE;
        /* cast kept: the raw float block's entries 16 / 10 / 4 are the three flat vertices' diffuse colours */
        ((u32 *)f->faces[e].verts)[16] = 0x6b1917;
        ((u32 *)f->faces[e].verts)[10] = 0x6b1917;
        ((u32 *)f->faces[e].verts)[4] = 0x6b1917;
    }
    for (e = 0; e < 4; e++) {
        f->faces[e * 2].poly.idx[0] = f->faces[e * 2 + 1].poly.idx[0] = e;
        f->faces[e * 2].poly.idx[1] = f->faces[e * 2 + 1].poly.idx[2] = e + 1;
        f->faces[e * 2].poly.idx[2] = f->faces[e * 2 + 1].poly.idx[1] = e + 2;
    }
    b = record;
    VITESSE_PROJECTION = (s16)ElasticProperty(b, 16);
    DISTANCE_PROJECTION = (s16)ElasticProperty(b, 0);
    HAUTEUR_PROJECTION = (s16)ElasticProperty(b, 4);
    LONGUEUR_MAX = (s16)ElasticProperty(b, 12);
    if (IsInScene())
        SnapToGround(1);
    homePos = pos;
    c = ElasticPropertyRaw(b, 8);
    g = Scn_FindIdList((u16)c, &d);
    camera = g ? (CamSetup *)g[0] : 0; /* cast kept: an id list holds record addresses as u32 */
    ResetToItem();
#undef a
#undef b
#undef c
#undef d
#undef e
#undef f
#undef g
}
/* BYTES(slot-scope, inferred): the nested block(s) only order the frame: their locals are allocated after the enclosing scope's */
s8 elastic::ProjectOntoBand(const Vec3s *point)
{
    Vec3f a;
    Vec3f b;
    Vec3f c;
    Vec3f d;
    Mat44 e;
    Vec3s f;
    e.SetIdentity();
    e.m[0][0] = Math_Fixed12ToFloat_s16(bandFrame.rot[0]);
    e.m[0][1] = Math_Fixed12ToFloat_s16(bandFrame.rot[3]);
    e.m[0][2] = Math_Fixed12ToFloat_s16(bandFrame.rot[6]);
    e.m[1][0] = Math_Fixed12ToFloat_s16(bandFrame.rot[1]);
    e.m[1][1] = Math_Fixed12ToFloat_s16(bandFrame.rot[4]);
    e.m[1][2] = Math_Fixed12ToFloat_s16(bandFrame.rot[7]);
    e.m[2][0] = Math_Fixed12ToFloat_s16(bandFrame.rot[2]);
    e.m[2][1] = Math_Fixed12ToFloat_s16(bandFrame.rot[5]);
    e.m[2][2] = Math_Fixed12ToFloat_s16(bandFrame.rot[8]);
    e.m[3][0] = (float)bandFrame.trans[0];
    e.m[3][1] = (float)bandFrame.trans[1];
    e.m[3][2] = (float)bandFrame.trans[2];
#define ELASTIC_TRANSFORM(out, in)                                              \
    out.x = e.m[0][0] * in.x + e.m[1][0] * in.y + e.m[2][0] * in.z + e.m[3][0]; \
    out.y = e.m[0][1] * in.x + e.m[1][1] * in.y + e.m[2][1] * in.z + e.m[3][1]; \
    out.z = e.m[0][2] * in.x + e.m[1][2] * in.y + e.m[2][2] * in.z + e.m[3][2]
    c.x = (float)(anchorA.x - anchorB.x);
    c.y = (float)(anchorA.y - anchorB.y);
    c.z = (float)(anchorA.z - anchorB.z);
    ELASTIC_TRANSFORM(a, c);
    c.x = (float)(anchorA.x - point->x);
    c.y = (float)(anchorA.y - point->y);
    c.z = (float)(anchorA.z - point->z);
    ELASTIC_TRANSFORM(b, c);
    perpDist = 0;
    if (b.x < 0.0f || b.x > a.x)
        return ELASTIC_SIDE_NONE;
    {
        e.Transpose3x3InPlace();
        c.x = b.x;
        c.y = -80.0f;
        c.z = 0.0f;
        ELASTIC_TRANSFORM(d, c);
        f.x = anchorA.x - (s16)d.x;
        f.y = anchorA.y - (s16)d.y;
        f.z = anchorA.z - (s16)d.z;
        SetPosition(&f);
        perpDist = (s16)fabs((double)b.z);
        if (b.z > 0.0f)
            return ELASTIC_SIDE_POS;
        return ELASTIC_SIDE_NEG;
    }
#undef ELASTIC_TRANSFORM
}
void elastic::SetBandMode(s32 on)
{
    if (on) {
        SetUpdateMode(SCN_UPD_ALWAYS);
        AddWolfRider();
        flags |= ELASTIC_F_BAND_VISIBLE;
        inst_model = (Model *)bandMeshKey; /* cast kept: in band mode the model is the 16-byte header copy */
    } else {
        SetUpdateMode(SCN_UPD_NORMAL);
        RemoveWolfRider();
        flags &= ~ELASTIC_F_BAND_VISIBLE;
        inst_model = itemModel;
    }
}
void elastic::UpdateBandMesh(const Vec3s *handle, s32 twoAnchors)
{
    float *a;
    void *b;
    Vec3s c;
    u32 d;
    /* cast kept: the lookup returns the Mesh as a void * and is keyed by the header copy's address */
    Mesh *e = (Mesh *)Texture_FindByResource((Model *)bandMeshKey);
    e->vbPositions->Lock(1, &b, &d);
    a = (float *)b; /* cast kept: Lock returns the vertex data through a void ** */
    c.x = handle->x;
    c.y = handle->y - 0x50;
    c.z = handle->z;
    if (!twoAnchors) {
        a[0] = a[3] = (float)c.x;
        a[2] = a[5] = (float)c.z;
        a[1] = (float)c.y;
        a[4] = a[1] - -12.0f;
        a[12] = a[15] = (float)anchorA.x;
        a[14] = a[17] = (float)anchorA.z;
        a[13] = (float)anchorA.y;
        a[16] = a[13] - -12.0f;
        a[6] = a[9] = (float)(c.x + anchorA.x) / 2.0f;
        a[8] = a[11] = (float)(c.z + anchorA.z) / 2.0f;
        a[7] = (float)(c.y + anchorA.y) / 2.0f;
        a[10] = a[7] - -12.0f;
    } else {
        a[0] = a[3] = (float)anchorA.x;
        a[2] = a[5] = (float)anchorA.z;
        a[1] = (float)anchorA.y;
        a[4] = a[1] - -12.0f;
        a[12] = a[15] = (float)anchorB.x;
        a[14] = a[17] = (float)anchorB.z;
        a[13] = (float)anchorB.y;
        a[16] = a[13] - -12.0f;
        a[6] = a[9] = (float)c.x;
        a[8] = a[11] = (float)c.z;
        a[7] = (float)c.y;
        a[10] = a[7] - -12.0f;
    }
    e->vbPositions->Unlock();
}
void elastic::RenderBand(Camera *view)
{
    Mat44 a;
    Mesh *b = (Mesh *)Texture_FindByResource(inst_model); /* cast kept: the lookup returns the Mesh as a void * */
    a.SetIdentity();
    g_pD3DAppMain->SetTransform(D3DTRANSFORMSTATE_WORLD, &a);
    g_pD3DAppMain->SetTransform(D3DTRANSFORMSTATE_VIEW, &view->viewMatCopy);
    b->TransformAll();
    b->DrawImmediate(g_pPolyBin, g_pViewFrustum);
}
