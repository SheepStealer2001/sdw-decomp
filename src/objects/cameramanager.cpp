/* T116 - original object CameraManager.cpp (guessed name).
 * Ranges: .text 0x4a00c0-0x4a07a5, .rdata 0x575be4-0x575c08 (vtable), .data 0x57b398-0x57b410 (g_camMgrPropOffsets,
 * defined here).
 * PAL PC CameraManager, 0x4a00c0-0x4a0740. */
#include "sdw_types.h"
#include "sdw_enums.h"
#define SDW_MEMBERS_ScnObject       \
    static void *operator new(u32); \
    void SetUpdateMode(s32 mode);
#include "sdw_classes.h"
#define SDW_INLINE_SCNOBJECT_SETVISIBLE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETVISIBLE_S32
#define SDW_INLINE_SCNOBJECT_SETMOVEMENTENABLED_S32 1
#define SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32 1
#include "../engine/scenaric_inlines.h"
#undef SDW_INLINE_SCNOBJECT_SETMOVEMENTENABLED_S32
#undef SDW_INLINE_SCNOBJECT_SETUPDATEMODE_S32
/* 0x57b398 .data: the property offsets of the fifteen {CAMERAnn, BOX, INTB, INTE} groups */
u16 g_camMgrPropOffsets[15][4] = {
    {0, 4, 8, 12},        {16, 20, 24, 28},     {32, 36, 40, 44},     {48, 52, 56, 60},     {64, 68, 72, 76},
    {80, 84, 88, 92},     {96, 100, 104, 108},  {112, 116, 120, 124}, {128, 132, 136, 140}, {144, 148, 152, 156},
    {160, 164, 168, 172}, {176, 180, 184, 188}, {192, 196, 200, 204}, {208, 212, 216, 220}, {224, 228, 232, 236}};
extern Wolf *g_pWolf;
#include "../app/app_main.h"
#include "camera.h"
#include "../engine/id_list.h"
void Camera_StartScripted(ScnObject *, Camera *, u16, u16, u16, Vec3s *, u16, u32, s32);
#define SDW_INLINE_FREE_CAMERAISSCRIPTED 1
#include "camera_inlines.h"
#undef SDW_INLINE_FREE_CAMERAISSCRIPTED
void CameraManager::PostLoadInit()
{
    u32 a;
    u8 *b;
    u32 c;
    u16 d;
    u32 e;
    u32 *f;
    u32 g;
    u32 h;
    SetUpdateMode(SCN_UPD_ALWAYS);
    SetVisible(0);
    SetMovementEnabled(0);
    for (e = 0; (s32)e < 2; e++)
        enableMask[e] = 0xff;
    b = (u8 *)record; /* cast kept: the level record is raw bytes */
    entryCount = 0;
    /* cast kept (the u32 reads): designer properties are 4-byte slots at byte offsets of the raw WAR record */
    for (e = 0; e < 15; e++) {
        c = *(u32 *)(b + g_camMgrPropOffsets[e][0] + 0x14);
        a = *(u32 *)(b + g_camMgrPropOffsets[e][1] + 0x14);
        if (c && a) {
            f = Scn_FindIdList((u16)c, &d);
            entries[entryCount].camera = (CamSetup *)*f; /* cast kept: an export id list's entry, here a camera */
            f = Scn_FindIdList((u16)a, &d);
            g = *(u32 *)(b + g_camMgrPropOffsets[e][2] + 0x14);
            entries[entryCount].blendIn = g;
            /* cast kept: a designer-property record read at its byte offset */
            h = *(u32 *)(b + g_camMgrPropOffsets[e][3] + 0x14);
            entries[entryCount].blendOut = h;
            entrySlot[entryCount] = (s8)e;
            entries[entryCount].box = (Box *)*f; /* cast kept: an export id list's entry, here a zone box */
            entryCount++;
        }
    }
    for (e = entryCount; (s32)e < 16; e++)
        entrySlot[e] = -1;
    scriptActive = 0;
}
void CameraManager::Update()
{
    Vec3s a = g_pWolf->pos;
    s32 b;
    CamSetup *c;
    s32 d;
    s8 e;
    Box *f;
    u16 g, h, i, j;
    u32 k;
    for (b = 0; b < entryCount; b++) {
        f = entries[b].box;
        c = entries[b].camera;
        e = entrySlot[b];
        d = enableMask[e >> 3] & (1 << (e & 7));
        if (!d)
            continue;
        if (a.x >= f->min[0] && a.x <= f->max[0] && a.y >= f->min[1] && a.y <= f->max[1] && a.z >= f->min[2] &&
            a.z <= f->max[2]) {
            k = (entries[b].blendIn != 0) + (entries[b].blendOut ? 2 : 0);
            j = c->focal;
            i = c->rot[2];
            h = c->rot[1];
            g = c->rot[0];
            Camera_StartScripted(this, &g_camera, g, h, i, &c->eye, j, k, 0x1000);
            scriptActive = 1;
            return;
        }
    }
    if (scriptActive && CameraIsScripted()) {
        Camera_ReleaseAny();
        scriptActive = 0;
    }
}
s32 CameraManager::HandleMessage(ScnObject *sender, u32 message, void *arg)
{
    u16 a, b;
    CamMgrEntry *c;
    u8 d;
    s32 e;
    switch (message) {
        case MSG_CAMMGR_ENABLE_ONLY_BOX:
            for (a = 0; a < 16; a++) {
                d = (u8)(1 << (a & 7));
                b = 0xffff;
                for (e = 0; e < entryCount; e++)
                    if (entrySlot[e] == a)
                        b = (u16)e;
                if (b != 0xffff && entries[b].box == arg && arg)
                    enableMask[a / 8] = enableMask[a / 8] | d;
                else
                    enableMask[a / 8] = enableMask[a / 8] & ~d;
            }
            break;
        case MSG_CAMMGR_GET_ENTRY:
            a = 0;
            while (a < 16) {
                if (entries[a].box == arg)
                    c = &entries[a];
                a++;
            }
            return (s32)c; /* cast kept: the s32 reply carries the entry's address */
    }
    return 0;
}
void CameraManager::Reset()
{
    scriptActive = 0;
}
ScnObject *CameraManager_Create(void *record)
{
    ScnLogic *object = new CameraManager;
    object = (ScnLogic *)object->Init(record); /* cast kept: Init returns the base class */
    return object;
}
