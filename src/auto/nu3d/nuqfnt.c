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
    u8 pad84[0x18];
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
    u8 pad[0x14];
    u8 priority;       /* 0x14 */
    u8 pad15[3];
} NUROOM;
typedef struct nuportalinfo_s {
    s32 max_portals;      /* 0x00 */
    u8 pad04[8];
    NUROOM* rooms;        /* 0x0c */
    u8 pad10[0x20];
    NUFRUSTRUM* frusta[16]; /* 0x30 */
    s32 num_frusta;       /* 0x70 */
    s32 camera_room;      /* 0x74 */
} NUPORTALINFO;
typedef struct nugscn_s {
    u8 pad[0x88];
    NUPORTALINFO portal;
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
