#include "types.h"

extern void fn_8001FD98(s32 a, s32 b, s32 c, s32 d, s32 e);

extern f32 lbl_80407CBC;

extern f32 lbl_80407CC4;

void fn_800203A4(f32 a)
{
    lbl_80407CC4 = a;
}

f32 fn_800203AC(u8* p)
{
    return **(f32**)(p + 0x30) * *(f32*)(p + 0x14) * lbl_80407CBC;
}

void fn_8002043C(s32 a, s32 b, s32 c, s32 d)
{
    fn_8001FD98(0, a, b, c, d);
}

typedef union { void* void_ptr; u32 addr; u8* u8_ptr; } VARIPTR;
extern u8 lbl_8020D8F4[0x1000];
extern VARIPTR lbl_8040A324;
extern VARIPTR lbl_8040A328;
extern VARIPTR lbl_8040A32C;

void NuPortalInit(void)
{
    lbl_8040A324.void_ptr = lbl_8020D8F4;
    lbl_8040A328 = lbl_8040A324;
    lbl_8040A32C.void_ptr = lbl_8020D8F4 + 0x1000;
    lbl_8040A328.addr = (lbl_8040A328.addr + 0xF) & ~0xF;
}

typedef struct { f32 x, y, z; } NUVEC;
typedef struct { NUVEC center; f32 radius; } NUPORTALSPHERE;
typedef struct { f32 x, y, z, d; } NUPLANE;
typedef struct NUFRUSTRUM {
    f32 transposed_planes[32]; /* 0x00 */
    NUPLANE* planes;           /* 0x80 */
    NUVEC minimum;
    NUVEC maximum;
    s16 plane_count;           /* 0x9c */
    s16 room_id;               /* 0x9e */
} NUFRUSTRUM;
typedef struct nuportal_s {
    NUPLANE plane;     /* 0x00 */
    NUVEC* vertices;   /* 0x10 */
    s16 vertex_count;  /* 0x14 */
    s16 front_room;    /* 0x16 */
    s16 back_room;     /* 0x18 */
    u8 pad1a[6];
} NUPORTAL;
typedef struct nuroom_s {
    u8 pad00[8];
    s16* portalIndices;
    u8 pad0c[3];
    u8 portalCount;
    u32 flags;
    u8 priority;       /* 0x14 */
    u8 pad15[3];
} NUROOM;
typedef struct nuportalinfo_s {
    s32 max_portals;      /* 0x00 */
    NUPORTAL* portals;
    s32 numRooms;
    NUROOM* rooms;        /* 0x0c */
    u8 pad10[0x20];
    NUFRUSTRUM* frusta[16]; /* 0x30 */
    s32 num_frusta;       /* 0x70 */
    s32 camera_room;      /* 0x74 */
    s32 depth;
} NUPORTALINFO;
typedef struct nugscn_s {
    u8 pad[0x88];
    NUPORTALINFO portal;
    u8 pad104[0x7c];
    s32 sphereCount;
    s32 portalInstanceCount;
    NUPORTALSPHERE* spheres;
} NUGSCN;
extern NUVEC lbl_8020D8E8;

s32 checkPortalVisibility(NUPORTAL* portal, NUFRUSTRUM* frustum, NUGSCN* scene, s32 room_id, s32 next_room)
{
    s32 inside;
    s32 i;
    s32 j;
    s32 outside;
    if (scene->portal.rooms[room_id].priority < scene->portal.rooms[next_room].priority) {
        f32 d = portal->plane.x * lbl_8020D8E8.x + portal->plane.y * lbl_8020D8E8.y + portal->plane.z * lbl_8020D8E8.z + portal->plane.d;
        if ((portal->back_room == room_id && d < 0.0f) || (portal->front_room == room_id && d > 0.0f)) {
            return 0;
        }
    }
    inside = 0;
    for (i = 0; i < frustum->plane_count; i++) {
        outside = 0;
        for (j = 0; j < portal->vertex_count; j++) {
            if (frustum->planes[i].x * portal->vertices[j].x + frustum->planes[i].y * portal->vertices[j].y + frustum->planes[i].z * portal->vertices[j].z + frustum->planes[i].d >= 0.0f) {
                inside++;
            } else {
                outside++;
            }
        }
        if (outside == portal->vertex_count) {
            return 0;
        }
    }
    if (inside == frustum->plane_count * portal->vertex_count) {
        return 1;
    }
    return 2;
}

extern s32 clipTestSphere(NUPORTALSPHERE* sphere, NUFRUSTRUM* frustum);

s32 NuPortalClipTest(NUGSCN* scene, NUVEC* pos, f32 radius, s16 room_id)
{
    NUPORTALINFO* p = &scene->portal;
    s32 i;
    if (p->max_portals == 0 || room_id == p->camera_room) {
        return 1;
    }
    for (i = 0; i < p->num_frusta; i++) {
        if (p->frusta[i]->room_id == room_id) {
            NUPORTALSPHERE s;
            s32 r;
            s.center.x = pos->x;
            s.center.y = pos->y;
            s.center.z = pos->z;
            s.radius = radius;
            r = clipTestSphere(&s, p->frusta[i]);
            if (r != 0) return r;
        }
    }
    return 0;
}

typedef struct vufntchar_s { f32 x; f32 y; f32 width; } VUFNTCHAR;
typedef struct vufnt_s {
    u8 filler0[6];
    u16 flags;
    s32 size;
    s32 glyph_count;
    s32 unicode_count;
    f32 height;      /* 0x14 */
    f32 baseline;
    f32 space_width;
    s32 mode;
    f32 ic_gap;      /* 0x24 */
    void* legacy_font;
    f32* x_scale;    /* 0x2c */
    f32* y_scale;    /* 0x30 */
    VUFNTCHAR* glyphs; /* 0x34 */
} VUFNT;
extern f32 lbl_80407CB8;

f32 fn_800203C8(VUFNT* font, u16* text)
{
    f32 scale = *font->x_scale;
    f32 width = 0.0f;
    while (*text != 0) {
        u16 c = *text++;
        f32 w = font->glyphs[c].width;
        if (c == 0x20 && lbl_80407CC4 != 0.0f) {
            w = lbl_80407CC4;
        }
        width += (w + font->ic_gap) * scale;
    }
    return width * lbl_80407CB8;
}

extern f32 lbl_80407CA8;
extern f32 lbl_80407CAC;
extern f32 lbl_80407CB0;
extern f32 lbl_80407CB4;
extern s32 lbl_80407CC0;
extern s32 lbl_8040AD20;
extern s32 lbl_8040AD24;
extern s32 lbl_8040AD4C;
extern s32 lbl_8040AD50;

s32 fn_8001FBB0(u32 mode)
{
    s32 old = lbl_80407CC0;
    lbl_80407CC0 = mode;
    switch (mode) {
    case 0:
    default:
        lbl_80407CA8 = 0.0625f / lbl_8040AD20;
        lbl_80407CAC = 0.125f / lbl_8040AD24;
        lbl_80407CB0 = 1.0f;
        lbl_80407CB4 = 1.0f;
        lbl_80407CB8 = 16.0f;
        lbl_80407CBC = 4.0f;
        break;
    case 2:
        lbl_80407CA8 = 1.0f / lbl_8040AD20;
        lbl_80407CAC = 0.5f / lbl_8040AD24;
        lbl_80407CB0 = 1.0f;
        lbl_80407CB4 = 1.0f;
        lbl_80407CB8 = 1.0f;
        lbl_80407CBC = 1.0f;
        break;
    case 3:
        lbl_80407CA8 = 0.5f;
        lbl_80407CAC = -0.5f;
        lbl_80407CB0 = 0.0f;
        lbl_80407CB4 = 0.0f;
        lbl_80407CB8 = 2.0f / lbl_8040AD4C;
        lbl_80407CBC = -2.0f / lbl_8040AD50;
        break;
    case 4:
        lbl_80407CB8 = 1.0f;
        lbl_80407CBC = -1.0f;
        break;
    }
    return old;
}

typedef struct { f32 m[4][4]; } NUMTX;
extern NUMTX lbl_8020D818;
extern NUMTX lbl_8020D858;
extern NUMTX lbl_8020D898;
extern NUPLANE lbl_803145D0;
extern NUVEC lbl_801B0ED8;
extern NUVEC lbl_801B0EE4;
extern void NuCameraGetClipMtx(NUMTX*, s32);
extern void fn_8000D4DC(NUMTX*, NUMTX*);
extern f32 NuVecMtxTransformH(NUVEC*, NUVEC*, NUMTX*);
extern void NuVecMax(NUVEC*, NUVEC*, NUVEC*);
extern void NuVecMin(NUVEC*, NUVEC*, NUVEC*);
extern void NuVecSub(NUVEC*, NUVEC*, NUVEC*);
extern void NuVecCross(NUVEC*, NUVEC*, NUVEC*);
extern f32 NuVecNorm(NUVEC*, NUVEC*);
extern NUFRUSTRUM* NuFrustrumAlloc(s16);
extern void transposeClipPlanes(NUFRUSTRUM*);
extern void drawClippedPortal(NUPORTAL*, NUROOM*, NUVEC*, NUVEC*, s32);
extern NUFRUSTRUM* buildFrustrum(NUVEC*, NUVEC*, s16);

static inline void computePortalExtents(NUPORTAL* portal, NUVEC* maximum, NUVEC* minimum)
{
    NUVEC point;
    s32 i;
    NuCameraGetClipMtx(&lbl_8020D818, 0);
    fn_8000D4DC(&lbl_8020D858, &lbl_8020D818);
    NuVecMtxTransformH(maximum, portal->vertices, &lbl_8020D818);
    *minimum = *maximum;
    for (i = 1; i < portal->vertex_count; i++) {
        NuVecMtxTransformH(&point, &portal->vertices[i], &lbl_8020D818);
        NuVecMax(maximum, maximum, &point);
        NuVecMin(minimum, minimum, &point);
    }
    NuVecMin(maximum, maximum, &lbl_801B0ED8);
    NuVecMax(maximum, maximum, &lbl_801B0EE4);
    NuVecMin(minimum, minimum, &lbl_801B0ED8);
    NuVecMax(minimum, minimum, &lbl_801B0EE4);
}

static inline s32 clipPortalExtents(NUVEC* minimum, NUVEC* maximum, NUVEC* clip_min, NUVEC* clip_max)
{
    if (maximum->x <= clip_min->x || maximum->y <= clip_min->y || minimum->x >= clip_max->x || minimum->y >= clip_max->y) {
        return 0;
    } else {
        if (minimum->x > clip_min->x) minimum->x = minimum->x;
        else minimum->x = clip_min->x;
        if (!(minimum->y > clip_min->y)) minimum->y = clip_min->y;
        if (!(maximum->x < clip_max->x)) maximum->x = clip_max->x;
        if (!(maximum->y < clip_max->y)) maximum->y = clip_max->y;
        return 1;
    }
}

s32 clipPortal(NUPORTAL* portal, NUROOM* room, s32 room_id, NUFRUSTRUM* frustum, NUFRUSTRUM** output)
{
    NUVEC maximum;
    NUVEC minimum;
    s32 count = 0;
    s32 i;
    s32 visible;
    for (i = 0; i < portal->vertex_count; i++) {
        if (lbl_803145D0.x * portal->vertices[i].x + lbl_803145D0.y * portal->vertices[i].y + lbl_803145D0.z * portal->vertices[i].z + lbl_803145D0.d > 0.0f) {
            count++;
        }
    }
    if (count == 0) return 0;
    if (count != portal->vertex_count) return 4;
    computePortalExtents(portal, &maximum, &minimum);
    visible = clipPortalExtents(&minimum, &maximum, &frustum->minimum, &frustum->maximum);
    if (visible) {
        drawClippedPortal(portal, room, &minimum, &maximum, 0xFF00);
        *output = buildFrustrum(&minimum, &maximum, room_id);
        return 1;
    }
    return 0;
}

NUFRUSTRUM* createPortalFrustrum(NUPORTAL* portal, s16 room_id)
{
    NUVEC camera;
    NUVEC first;
    NUVEC second;
    NUFRUSTRUM* frustum;
    f32 side;
    s32 i;
    frustum = NuFrustrumAlloc(portal->vertex_count);
    frustum->room_id = room_id;
    camera.x = lbl_8020D898.m[3][0];
    camera.y = lbl_8020D898.m[3][1];
    camera.z = lbl_8020D898.m[3][2];
    side = portal->plane.x * camera.x + portal->plane.y * camera.y + portal->plane.z * camera.z + portal->plane.d;
    for (i = 0; i < portal->vertex_count; i++) {
        NuVecSub(&first, &camera, &portal->vertices[i]);
        NuVecSub(&second, &camera, &portal->vertices[(i + 1) % portal->vertex_count]);
        NuVecCross(&first, &first, &second);
        NuVecNorm(&first, &first);
        if (side < 0.0f) {
            frustum->planes[i].x = first.x;
            frustum->planes[i].y = first.y;
            frustum->planes[i].z = first.z;
            frustum->planes[i].d = -(first.x * camera.x + first.y * camera.y + first.z * camera.z);
        } else {
            frustum->planes[i].x = -first.x;
            frustum->planes[i].y = -first.y;
            frustum->planes[i].z = -first.z;
            frustum->planes[i].d = first.x * camera.x + first.y * camera.y + first.z * camera.z;
        }
    }
    computePortalExtents(portal, &frustum->maximum, &frustum->minimum);
    transposeClipPlanes(frustum);
    return frustum;
}

extern s16 lbl_8040A31C;
extern void fn_80037D3C(NUROOM*, NUGSCN*);
extern void clipRoomAgainstFrustrum(NUGSCN*, NUROOM*, NUFRUSTRUM*);
extern void drawPortal(NUPORTAL*, NUROOM*, s32);

void roomRecursive(NUGSCN* scene, NUFRUSTRUM* frustum, s16 room_id, s16 previous_room, s32 depth)
{
    NUFRUSTRUM* next_frustum;
    NUROOM* room = &scene->portal.rooms[room_id];
    u32 i;
    s16 next_room;
    s32 j;
    NUPORTAL* portal;
    if (room_id == lbl_8040A31C) fn_80037D3C(room, scene);
    else clipRoomAgainstFrustrum(scene, room, frustum);
    room->flags |= 0x20000000;
    if (depth >= scene->portal.depth) return;
    for (i = 0; i < room->portalCount; i++) {
        portal = &scene->portal.portals[room->portalIndices[i]];
        if (portal->front_room != room_id) next_room = portal->front_room;
        else next_room = portal->back_room;
        if (next_room == previous_room) continue;
        switch ((u32)checkPortalVisibility(portal, frustum, scene, room_id, next_room)) {
        case 0:
            break;
        case 1:
            drawPortal(portal, &scene->portal.rooms[next_room], 0xFFFF00FF);
            next_frustum = createPortalFrustrum(portal, next_room);
            roomRecursive(scene, next_frustum, next_room, room_id, depth + 1);
            break;
        case 2:
            switch (clipPortal(portal, &scene->portal.rooms[next_room], next_room, frustum, &next_frustum)) {
            case 0:
                break;
            case 1:
                roomRecursive(scene, next_frustum, next_room, room_id, depth + 1);
                break;
            case 4:
                {
                    NUFRUSTRUM* copy = NuFrustrumAlloc(frustum->plane_count);
                    *copy = *frustum;
                    for (j = 0; j < frustum->plane_count; j++) copy->planes[j] = frustum->planes[j];
                    next_frustum = copy;
                    copy->room_id = next_room;
                    roomRecursive(scene, next_frustum, next_room, room_id, depth + 1);
                }
                break;
            }
            break;
        default:
            roomRecursive(scene, frustum, next_room, room_id, depth + 1);
            break;
        }
    }
}

extern void fn_80037DA8(void);
extern void fn_80037DB8(NUGSCN*);
extern void fn_80037DE8();
extern void* NuCameraGetMtx(void);
extern NUMTX* NuCameraGetViewMtx(void);
extern s16 NuPortalWhichRoom(NUGSCN*, NUVEC*);
extern void drawViewFrustrum(void);
extern void fn_80027BE0(NUPORTALSPHERE*, s32, f32, f32);
extern NUVEC* lbl_80407CA4;
extern s32 lbl_80407C94;
extern s32 lbl_80407C98;
extern void* lbl_8040A320;
extern s32* lbl_8040A330;
extern NUFRUSTRUM** lbl_8040A334;
extern NUVEC lbl_80313298;
extern NUPLANE lbl_8020D8D8;

s32 NuPortalVisibility(NUGSCN* scene)
{
    NUVEC near_position;
    NUVEC* camera_position;
    f32 near_clip;
    f32 x, y, z;
    NUFRUSTRUM* frustum;
    u32 i;
    fn_80037DA8();
    if (lbl_80407CA4 != 0) camera_position = lbl_80407CA4;
    else camera_position = &lbl_8020D8E8;
    if (lbl_80407C94 == 0) {
        fn_80037DB8(scene);
        return 0;
    }
    if (scene->portal.max_portals == 0 || scene->portal.numRooms == 0 || scene->portalInstanceCount == 0) return 0;
    lbl_8040A330 = &scene->portal.num_frusta;
    scene->portal.num_frusta = 0;
    lbl_8040A334 = scene->portal.frusta;
    fn_80037DE8(scene);
    lbl_8040A320 = NuCameraGetMtx();
    lbl_8020D898 = *NuCameraGetViewMtx();
    lbl_8020D8E8 = lbl_80313298;
    near_clip = *(f32*)((u8*)lbl_8040A320 + 0x48);
    lbl_8040A31C = NuPortalWhichRoom(scene, camera_position);
    scene->portal.camera_room = lbl_8040A31C;
    if (lbl_8040A31C == -1) return 0;
    x = lbl_8020D898.m[2][0] * near_clip + lbl_8020D8E8.x;
    y = lbl_8020D898.m[2][1] * near_clip + lbl_8020D8E8.y;
    z = lbl_8020D898.m[2][2] * near_clip + lbl_8020D8E8.z;
    lbl_8020D8D8.x = lbl_8020D898.m[2][0];
    lbl_8020D8D8.y = lbl_8020D898.m[2][1];
    lbl_8020D8D8.z = lbl_8020D898.m[2][2];
    lbl_803145D0.z = lbl_8020D898.m[2][2];
    near_position.x = x;
    near_position.y = y;
    near_position.z = z;
    lbl_8020D8D8.d = -(x * lbl_8020D898.m[2][0] + y * lbl_8020D898.m[2][1] + z * lbl_8020D898.m[2][2]);
    lbl_803145D0.x = lbl_8020D898.m[2][0];
    lbl_803145D0.y = lbl_8020D898.m[2][1];
    lbl_803145D0.d = -(lbl_8020D8E8.x * lbl_8020D898.m[2][0] + lbl_8020D8E8.y * lbl_8020D898.m[2][1] + lbl_8020D8E8.z * lbl_8020D898.m[2][2]);
    frustum = buildFrustrum(&lbl_801B0EE4, &lbl_801B0ED8, -2);
    drawViewFrustrum();
    for (i = 0; i < (u32)scene->portal.numRooms; i++) scene->portal.rooms[i].flags &= ~0x20000000;
    roomRecursive(scene, frustum, lbl_8040A31C, -1, 0);
    if (lbl_80407C98 != -1 && lbl_80407C98 < scene->sphereCount) {
        NUPORTALSPHERE* sphere = &scene->spheres[lbl_80407C98];
        fn_80027BE0(sphere, 0x400000, sphere->radius, 1.0f);
    }
    return 1;
}
