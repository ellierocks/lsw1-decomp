#include "types.h"
#include "numath/numath.h"

typedef union {
    void* ptr;
    u32 addr;
} NuPortalVariptr;

extern void* lbl_80407C38;
extern f32 lbl_801905E8[];
extern void NuAnimData2Clone(void*, void*, void*, f32);
extern f32* NuCameraGetProjectionMtx(void);
typedef struct { f32 x, y, z, w; } NuPortalVec4;
typedef struct { f32 x, y, z; } NuPortalVec3;
typedef struct { f32 x, y, z, d; } NuPortalPlane;
typedef struct {
    f32 transposed[32];
    NuPortalPlane* planes;
    NuPortalVec3 minimum;
    NuPortalVec3 maximum;
    s16 plane_count;
    s16 room_id;
} NuPortalFrustum;
extern NuPortalPlane lbl_803145D0;
extern NuPortalVariptr lbl_8040A328;
extern NuPortalVariptr lbl_8040A32C;
extern s32* lbl_8040A330;
extern void** lbl_8040A334;
extern char lbl_80190974[];
extern char lbl_801909A4[];
extern char lbl_801909C4[];
extern f32 lbl_80190A28[];
extern void (*fn_80009948(char*, s32))(char*, ...);

void NuAnimBuffCreate(s32 max_joints, NuPortalVariptr* buffer, NuPortalVariptr buffer_end)
{
    u32* anim_buffer = (u32*)((buffer->addr + 15) & ~15);
    buffer->addr = (u32)(anim_buffer + 3);
    anim_buffer[2] = (u32)(anim_buffer + 4);
    buffer->addr = (buffer->addr + 15 & ~15) + max_joints * 0x24;
    anim_buffer[1] = max_joints;
    anim_buffer[0] = 0;
    lbl_80407C38 = anim_buffer;
}

typedef struct {
    s32 count;
    s32 max_joints;
    f32* data;
} NuPortalAnimBuff;
typedef struct {
    u8 pad0[0x50];
    u8 parent_ix;
    u8 pad51[0xf];
} NuPortalJoint;
typedef struct {
    u8 pad0[0x14];
    NuPortalJoint* joints;
    u8 pad18[0x64];
    u8 num_joints;
} NuPortalHGobj;
void NuAnimData2Apply(void* anim, NuPortalAnimBuff* buffer, s32 count, s32 first, f32 time);
void NuAnimData2CalcMatrix(void* anim, NuPortalAnimBuff* buffer, s32 count, s32 first, NuPortalVec3* root, f32 time);

void NuAnimBuffEvaluate(NuPortalAnimBuff* buffer, void* anim, f32 time, s32 apply, f32 blend, s32 start,
                        NuPortalHGobj* hgobj, NuPortalVec3* root)
{
    s32 end;
    if (buffer == NULL)
        buffer = lbl_80407C38;
    buffer->count = hgobj->num_joints;
    if (start != 0) {
        u8 parent = hgobj->joints[start].parent_ix;
        end = start + 1;
        if (end < hgobj->num_joints) {
            do {
                if (hgobj->joints[end].parent_ix == parent)
                    break;
                end++;
            } while (end < hgobj->num_joints);
        }
    } else {
        end = hgobj->num_joints;
    }
    if (apply) {
        NuAnimData2Apply(anim, buffer, end - start, start, time - 1.0f);
        if (root != NULL) {
            root->x = buffer->data[0];
            root->y = buffer->data[1];
            root->z = -buffer->data[2];
        }
    } else {
        NuAnimData2CalcMatrix(anim, buffer, end - start, start, root, time - 1.0f);
    }
}

typedef struct {
    struct nuvec_s t;
    struct nuvec_s r;
    struct nuvec_s s;
} NuPortalJointKey;
typedef struct {
    u8 pad0[0x34];
    u8* flags;
} NuPortalAnimData;
void fn_8000EB24(struct numtx_s* m, struct nuangvec_s* ang);
void fn_8000EA94(struct numtx_s* m, struct nuvec_s* scale);
void NuMtxTranslate(struct numtx_s* m, struct nuvec_s* t);

void fn_8001BE94(NuPortalAnimData* anim, s32 joint, struct numtx_s* m, f32 time)
{
    NuPortalJointKey* key = (NuPortalJointKey*)((NuPortalAnimBuff*)lbl_80407C38)->data;
    u8 flags = anim->flags[joint];
    struct nuangvec_s ang;
    NuAnimData2Clone(anim, key, (void*)joint, time - 1.0f);
    if (flags & 1) {
        ang.x = key->r.x * 10430.378f;
        ang.y = key->r.y * 10430.378f;
        ang.z = key->r.z * 10430.378f;
        fn_8000EB24(m, &ang);
    } else {
        NuMtxSetIdentity(m);
    }
    if (flags & 8)
        fn_8000EA94(m, &key->s);
    NuMtxTranslate(m, &key->t);
    m->_02 = -m->_02;
    m->_12 = -m->_12;
    m->_20 = -m->_20;
    m->_21 = -m->_21;
    m->_23 = -m->_23;
    m->_32 = -m->_32;
}

void* fn_8001BFC4(void* data, void* source, f32 time)
{
    void* buffer = ((void**)lbl_80407C38)[2];
    NuAnimData2Clone(data, buffer, source, time - lbl_801905E8[0]);
    return buffer;
}

#define _STDARG_H
#include "sn/include/va-ppc.h"
typedef __gnuc_va_list va_list;
extern char lbl_80407C40[8];
extern char* lbl_8040AD34;
extern char* lbl_8040AD38;
extern char lbl_803132D0[];
extern s32 lbl_80407C48;
extern char lbl_8019063C[];
extern char lbl_80190684[];
s32 vsprintf(char*, char*, va_list);
s32 NuStrLen(char*);
void fn_80005A4C(s32, char*);

void fn_8001C00C(char* fmt, ...)
{
    va_list args;
    if (fmt == NULL || *fmt == 0)
        fmt = lbl_80407C40;
    va_start(args, fmt);
    vsprintf(lbl_8040AD34, fmt, args);
    lbl_8040AD34 += NuStrLen(lbl_8040AD34);
    if ((u32)lbl_8040AD34 > (u32)lbl_8040AD38) {
        fn_80005A4C(lbl_80407C48, lbl_803132D0);
        lbl_8040AD34 = lbl_803132D0;
        lbl_8040AD38 = lbl_803132D0 + 0xc00;
    }
    va_end(args);
}

void fn_8001C110(char* fmt, ...)
{
    va_list args;
    if (fmt == NULL || *fmt == 0)
        fmt = lbl_80407C40;
    fn_8001C00C(lbl_8019063C, 0x25);
    va_start(args, fmt);
    vsprintf(lbl_8040AD34, fmt, args);
    lbl_8040AD34 += NuStrLen(lbl_8040AD34);
    fn_8001C00C(lbl_80190684);
    va_end(args);
}

void NuAnimCurveEval(f32* out)
{
    f32* projection = NuCameraGetProjectionMtx();
    out[0] = -projection[0];
    out[4] = projection[1];
    out[8] = -projection[2];
    out[1] = -projection[4];
    out[5] = projection[5];
    out[9] = -projection[6];
    out[2] = -projection[8];
    out[6] = projection[9];
    out[10] = -projection[10];
    out[11] = 0.0f;
    out[7] = 0.0f;
    out[3] = 0.0f;
    out[15] = 1.0f;
}

s32 clipTestSphere(NuPortalVec4* sphere, NuPortalFrustum* frustum)
{
    s32 inside = 0;
    s32 i;
    for (i = 0; i < frustum->plane_count; i++) {
        NuPortalPlane* plane = &frustum->planes[i];
        f32 distance = plane->x * sphere->x + plane->y * sphere->y + plane->z * sphere->z + plane->d;
        if (distance < -sphere->w) return 0;
        if (distance > sphere->w) inside++;
    }
    {
        NuPortalPlane* plane = &lbl_803145D0;
        f32 distance = plane->x * sphere->x + plane->y * sphere->y + plane->z * sphere->z + plane->d;
        if (distance < -sphere->w) return 0;
        if (distance > sphere->w) inside++;
    }
    return inside == frustum->plane_count + 1 ? 1 : 2;
}

extern f32 lbl_803132B0[];
extern f32 lbl_8040AD3C;
extern f32 lbl_8040AD40;
extern f32 lbl_8040AD44;
extern f32 lbl_8040AD48;
extern struct numtx_s lbl_80314430;
extern struct numtx_s lbl_80314470;
extern s32 lbl_80407C88;
void fn_8000DBA0(struct numtx_s* m);

static inline f32 portal_mul(f32 arg0, f32 arg1)
{
    return arg0 * arg1;
}

void fn_8001D0D8(void)
{
    f32 scale;
    f32 scale_sq;
    f32 a;
    f32 b;
    f32 c;
    f32 d;
    f32 inv_a;
    f32 inv_b;
    f32 inv_c;
    f32 inv_d;
    f32 x;
    f32 y;
    f32 z;
    f32 w;
    scale = lbl_803132B0[0];
    scale_sq = portal_mul(scale, scale);
    a = portal_mul(scale, lbl_8040AD40);
    b = portal_mul(scale, lbl_8040AD3C);
    lbl_80407C88 = 1;
    inv_a = 1.0f / NuFsqrt(portal_mul(a, a) + scale_sq);
    inv_b = 1.0f / NuFsqrt(portal_mul(b, b) + scale_sq);
    fn_8000DBA0(&lbl_80314430);
    c = portal_mul(scale, lbl_8040AD44);
    d = portal_mul(scale, lbl_8040AD48);
    x = portal_mul(-scale, inv_a);
    y = portal_mul(scale, inv_b);
    z = portal_mul(-a, inv_a);
    w = portal_mul(-b, inv_b);
    lbl_80314430._00 = x;
    lbl_80314430._12 = y;
    lbl_80314430._20 = z;
    lbl_80314430._21 = z;
    lbl_80314430._01 = -x;
    lbl_80314430._13 = -y;
    lbl_80314430._22 = w;
    lbl_80314430._23 = w;
    inv_c = 1.0f / NuFsqrt(portal_mul(c, c) + scale_sq);
    inv_d = 1.0f / NuFsqrt(portal_mul(d, d) + scale_sq);
    fn_8000DBA0(&lbl_80314470);
    x = portal_mul(-scale, inv_c);
    y = portal_mul(scale, inv_d);
    z = portal_mul(-c, inv_c);
    w = portal_mul(-d, inv_d);
    lbl_80314470._00 = x;
    lbl_80314470._12 = y;
    lbl_80314470._01 = -x;
    lbl_80314470._13 = -y;
    lbl_80314470._20 = z;
    lbl_80314470._21 = z;
    lbl_80314470._22 = w;
    lbl_80314470._23 = w;
}

typedef struct { NuPortalVec4 rows[4]; } NuPortalMtx;
typedef struct { struct nuvec_s normal; f32 pad0c; struct nuvec_s point; } NuPortalCamPlane;
f32 fn_8000ED40(f32 value);
extern struct numtx_s lbl_801B1638;
extern NuPortalCamPlane lbl_80313288;
extern NuPortalMtx lbl_803144B0;
extern NuPortalMtx lbl_803144F0;
extern NuPortalVec4 lbl_80314530;
extern NuPortalVec4 lbl_80314540;
extern NuPortalMtx lbl_80314550;
extern NuPortalMtx lbl_80314590;

static inline void portal_abs_row(NuPortalVec4* out, NuPortalVec4* in)
{
    out->x = fn_8000ED40(in->x);
    out->y = fn_8000ED40(in->y);
    out->z = fn_8000ED40(in->z);
    out->w = fn_8000ED40(in->w);
}

static inline void portal_set_row(NuPortalVec4* row, f32 x, f32 y, f32 z, f32 w)
{
    row->x = x;
    row->y = y;
    row->z = z;
    row->w = w;
}

static inline void portal_abs_mtx(NuPortalMtx* out, NuPortalMtx* in)
{
    portal_abs_row(&out->rows[0], &in->rows[0]);
    portal_abs_row(&out->rows[1], &in->rows[1]);
    portal_abs_row(&out->rows[2], &in->rows[2]);
    portal_set_row(&out->rows[3], 0.0f, 0.0f, 0.0f, 0.0f);
}

void fn_8001D25C(void)
{
    NuMtxMul((struct numtx_s*)&lbl_803144B0, &lbl_801B1638, &lbl_80314430);
    NuMtxMul((struct numtx_s*)&lbl_803144F0, &lbl_801B1638, &lbl_80314470);
    *(struct nuvec_s*)&lbl_80314530 = lbl_80313288.normal;
    lbl_80314530.w = -NuVecDot(&lbl_80313288.point, &lbl_80313288.normal);
    portal_abs_row(&lbl_80314540, &lbl_80314530);
    lbl_80314540.w = 0.0f;
    portal_abs_mtx(&lbl_80314550, &lbl_803144B0);
    portal_abs_mtx(&lbl_80314590, &lbl_803144F0);
}

static inline void portal_transform_point(NuPortalVec4* out, struct nuvec_s* v, NuPortalMtx* m)
{
    out->x = v->x * m->rows[0].x + v->y * m->rows[1].x + v->z * m->rows[2].x + m->rows[3].x;
    out->y = v->x * m->rows[0].y + v->y * m->rows[1].y + v->z * m->rows[2].y + m->rows[3].y;
    out->z = v->x * m->rows[0].z + v->y * m->rows[1].z + v->z * m->rows[2].z + m->rows[3].z;
    out->w = v->x * m->rows[0].w + v->y * m->rows[1].w + v->z * m->rows[2].w + m->rows[3].w;
}

static inline void portal_project_box(NuPortalVec4* clip_center, NuPortalVec4* clip_extent, struct nuvec_s* center,
                                      struct nuvec_s* extent, NuPortalMtx* mtx, NuPortalMtx* abs_mtx)
{
    portal_transform_point(clip_center, center, mtx);
    NuVecMtxTransform((struct nuvec_s*)clip_extent, extent, (struct numtx_s*)abs_mtx);
}

s32 fn_8001D4B8(struct nuvec_s* center, struct nuvec_s* extent, f32 distance, s32 leaf)
{
    NuPortalVec4 clip_extent;
    NuPortalVec4 clip_center;
    NuPortalVec4* plane;
    f32 radius;
    f32 plane_distance;
    f32 near_distance;
    f32 far_distance;
    radius = NuVecDot((struct nuvec_s*)&lbl_80314540, extent);
    plane = &lbl_80314530;
    plane_distance = NuVecDot((struct nuvec_s*)plane, center) + plane->w;
    near_distance = plane_distance - distance;
    far_distance = plane_distance - lbl_803132B0[0];
    if (near_distance > radius || far_distance < -radius)
        goto outside;
    portal_project_box(&clip_center, &clip_extent, center, extent, &lbl_803144B0, &lbl_80314550);
    if (clip_center.x > clip_extent.x || clip_center.y > clip_extent.y || clip_center.z > clip_extent.z
        || clip_center.w > clip_extent.w)
        goto outside;
    if (clip_center.x > -clip_extent.x || clip_center.y > -clip_extent.y || clip_center.z > -clip_extent.z
        || clip_center.w > -clip_extent.w) {
        if (leaf == 0)
            goto partial;
        portal_project_box(&clip_center, &clip_extent, center, extent, &lbl_803144F0, &lbl_80314590);
        if (clip_center.x > clip_extent.x || clip_center.y > clip_extent.y || clip_center.z > clip_extent.z
            || clip_center.w > clip_extent.w) {
outside:
            return 0;
        }
        if (clip_center.x > -clip_extent.x || clip_center.y > -clip_extent.y || clip_center.z > -clip_extent.z
            || clip_center.w > -clip_extent.w)
            goto partial;
        goto inside;
    }
    if (leaf != 0)
        goto inside;
    if (near_distance > -radius) {
partial:
        return 2;
    }
    if (far_distance < radius)
        return 2;
inside:
    return 1;
}
typedef struct {
    struct nuvec_s minimum;
    f32 pad0c;
    struct nuvec_s maximum;
    f32 pad1c;
} NuPortalBox;
typedef struct {
    u16 box;
    u16 total_leaves;
    u16 child_count;
    u16 first_child;
    u16 leaf_count;
    u16 first_leaf;
} NuPortalNode;
typedef struct {
    u8 pad0[8];
    u16* leaves;
    NuPortalBox* leaf_boxes;
    u8 pad10[4];
    NuPortalNode* nodes;
    NuPortalBox* node_boxes;
    u16 count;
    u8 pad1e[2];
    u16* roots;
    f32* distances;
    u8 pad28[4];
    u16* leaf_lods;
} NuPortalSet;
typedef struct {
    s32 count;
    u32* items;
    u8 pad8[4];
} NuPortalLodItems;
typedef struct {
    u8 pad0[0x14];
    NuPortalLodItems* lods;
    u8 pad18[0x10];
    f32* lod_distances;
    u8 pad2c[4];
    u8* lod_bits[10];
    u8* item_bits[4];
    s32 buffer;
} NuPortalLodObj;
typedef struct {
    u8 pad0[0x14];
    NuPortalSet* set;
    u8 pad18[0xc];
    s32 unk24;
    u32 flags0 : 4;
    u32 enabled : 1;
    u32 flags5 : 27;
} NuPortalOwner;
extern s32 lbl_80407C80;
void fn_8001D0D8(void);
void fn_8001D25C(void);
s32 fn_8001D4B8(struct nuvec_s* minimum, struct nuvec_s* maximum, f32 distance, s32 leaf);
void fn_8002C24C(NuPortalLodObj* obj, s32 leaf, u32 lod, s32 visibility, ...);
extern f32 lbl_803132B4[];
extern struct nuvec_s lbl_80313298;

static inline void portal_select_lod(u32* lod, f32* distances, struct nuvec_s* position)
{
    struct nuvec_s delta;
    f32 distance;
    if (distances[*lod] != 0.0f) {
        NuVecSub(&delta, position, &lbl_80313298);
        distance = NuVecMagSqr(&delta);
        while (distance < distances[*lod])
            (*lod)++;
    }
}

static inline void portal_add_leaf(NuPortalSet* set, NuPortalLodObj* obj, u32 leaf, NuPortalBox* box, s32 visibility)
{
    u32 lod = set->leaf_lods[leaf];
    NuPortalLodItems* items;
    u32* item;
    u32* end;
    u8* bits;
    u8* lod_bits;
    portal_select_lod(&lod, obj->lod_distances, &box->minimum);
    fn_8002C24C(obj, leaf, lod, visibility);
    bits = obj->item_bits[obj->buffer];
    items = &obj->lods[lod];
    item = items->items;
    end = item + items->count;
    for (; item != end; item++)
        bits[*item >> 3] |= 1 << (*item & 7);
    lod_bits = obj->lod_bits[obj->buffer];
    lod_bits[lod >> 2] |= visibility << ((lod & 3) * 2);
}

static inline void portal_add_all(NuPortalSet* set, NuPortalLodObj* obj, NuPortalNode* node, u8* mask)
{
    u16* leaf = &set->leaves[node->first_leaf];
    u16* end = leaf + node->total_leaves;
    NuPortalBox* box = &set->leaf_boxes[node->first_leaf];
    for (; leaf != end; box++) {
        u16 id = *leaf++;
        if (mask != NULL && !((s8)mask[id >> 3] >> (id & 7) & 1))
            continue;
        portal_add_leaf(set, obj, id, box, 1);
    }
}

void fn_8001D7EC(NuPortalSet* set, s32 index, NuPortalLodObj* obj, u8* mask)
{
    NuPortalNode* stack[100];
    NuPortalNode** top = &stack[1];
    NuPortalNode* node;
    f32 distance;
    s32 visibility;
    u16* leaf;
    u16* end;
    NuPortalBox* box;
    stack[0] = &set->nodes[set->roots[index]];
    distance = set->distances[index];
    if (distance == 0.0f)
        distance = lbl_803132B4[0];
    while (top != stack) {
        node = *--top;
        visibility = fn_8001D4B8(&set->node_boxes[node->box].minimum, &set->node_boxes[node->box].maximum, distance, 0);
        if (visibility == 0)
            continue;
        if (visibility == 1) {
            portal_add_all(set, obj, node, mask);
        } else {
            if (node->child_count != 0) {
                NuPortalNode* child = &set->nodes[node->first_child];
                NuPortalNode* child_end = child + node->child_count;
                for (; child != child_end; child++)
                    *top++ = child;
            }
            if (node->leaf_count != 0) {
                leaf = &set->leaves[node->first_leaf];
                end = leaf + node->leaf_count;
                box = &set->leaf_boxes[node->first_leaf];
                for (; leaf != end; box++) {
                    u16 id = *leaf++;
                    if (mask != NULL && !((s8)mask[id >> 3] >> (id & 7) & 1))
                        continue;
                    visibility = fn_8001D4B8(&box->minimum, &box->maximum, distance, 1);
                    if (visibility != 0)
                        portal_add_leaf(set, obj, id, box, visibility);
                }
            }
        }
    }
}

void fn_8001DC4C(NuPortalOwner* owner, u8* scene)
{
    s32 i;
    owner->enabled = owner->set != NULL && lbl_80407C80 != 0;
    if (owner->enabled) {
        if (lbl_80407C88 == 0)
            fn_8001D0D8();
        fn_8001D25C();
        i = owner->set->count;
        while (--i >= 0)
            fn_8001D7EC(owner->set, i, *(NuPortalLodObj**)(scene + 0x1a0), (u8*)owner->unk24);
    }
}

NuPortalFrustum* NuFrustrumAlloc(s32 count)
{
    NuPortalFrustum* frustum = (NuPortalFrustum*)lbl_8040A328.ptr;
    lbl_8040A328.addr = (lbl_8040A328.addr + 15 & ~15) + 0xa0;
    if (lbl_8040A328.addr >= lbl_8040A32C.addr)
        fn_80009948(lbl_80190974, 0xdd)(lbl_801909A4);
    frustum->planes = lbl_8040A328.ptr;
    lbl_8040A328.addr += count * 16;
    if (lbl_8040A328.addr >= lbl_8040A32C.addr)
        fn_80009948(lbl_80190974, 0xe3)(lbl_801909A4);
    frustum->plane_count = count;
    lbl_8040A334[(*lbl_8040A330)++] = frustum;
    if (*lbl_8040A330 > 15)
        fn_80009948(lbl_80190974, 0xeb)(lbl_801909C4);
    return frustum;
}

typedef struct {
    u8 pad0[4];
    NuPortalPlane* planes;
    s16* portal_indices;
    u8 pad0c[2];
    u8 plane_count;
    u8 portal_count;
    s32 flags;
    u8 priority;
    u8 pad15[3];
} NuPortalRoom;
f32 NuPlnLine3(void* plane, struct nuvec_s* origin, struct nuvec_s* dir, struct nuvec_s* out);
typedef struct {
    NuPortalPlane plane;
    struct nuvec_s* vertices;
    s16 vertex_count;
    s16 front_room;
    s16 back_room;
    u8 pad1a[6];
} NuPortalPortal;
extern s32 lbl_80407C90;
extern struct nuvec_s lbl_8020D8E8;
void NuCameraTransformScreenClip(void);
void NuPs2CreateSkin(void);
void fn_8002782C(u32 colour, f32 x0, f32 y0, f32 z0, f32 x1, f32 y1, f32 z1);

void drawPortal(NuPortalPortal* portal, NuPortalRoom* room)
{
    struct nuvec_s dir;
    struct nuvec_s point;
    struct nuvec_s nearest_point;
    f32 nearest;
    f32 distance;
    s32 i;
    s32 j;
    if (lbl_80407C90 != 0) {
        NuCameraTransformScreenClip();
        for (i = 0; i < portal->vertex_count; i++) {
            struct nuvec_s* a = &portal->vertices[i];
            struct nuvec_s* b = &portal->vertices[(i + 1) % portal->vertex_count];
            NuVecSub(&dir, a, b);
            fn_8002782C(0xffff00ff, a->x, a->y, a->z, b->x, b->y, b->z);
        }
        for (i = 0; i < portal->vertex_count; i++) {
            NuVecSub(&dir, &portal->vertices[i], &lbl_8020D8E8);
            NuVecNorm(&dir, &dir);
            nearest = 3.4028234e+38f;
            for (j = 0; j < room->plane_count; j++) {
                distance = NuPlnLine3(&room->planes[j], &portal->vertices[i], &dir, &point);
                if (distance > 0.0f && distance < nearest) {
                    nearest_point = point;
                    nearest = distance;
                }
            }
            fn_8002782C(0xffff00ff, nearest_point.x, nearest_point.y, nearest_point.z,
                portal->vertices[i].x, portal->vertices[i].y, portal->vertices[i].z);
        }
        NuPs2CreateSkin();
    }
}

typedef struct {
    f32 x, y, z, w;
    u32 colour;
    f32 u, v;
} NuPortalLineVertex2D;
void fn_80027824(NuPortalLineVertex2D* lines, void* unused, s32 count);

void drawSquare(NuPortalVec3* minimum, NuPortalVec3* maximum, u32 colour)
{
    NuPortalLineVertex2D lines[8];
    f32 left, top, right, bottom;
    left = minimum->x * 0.5f + 0.5f;
    top = -minimum->y * 0.5f + 0.5f;
    bottom = -maximum->y * 0.5f + 0.5f;
    right = maximum->x * 0.5f + 0.5f;
    lines[0].x = left;
    lines[0].y = top;
    lines[0].z = 1.0f;
    lines[0].colour = colour;
    lines[1].x = left;
    lines[1].y = bottom;
    lines[1].z = 1.0f;
    lines[1].colour = colour;
    lines[2].x = left;
    lines[2].y = bottom;
    lines[2].z = 1.0f;
    lines[2].colour = colour;
    lines[3].x = right;
    lines[3].y = bottom;
    lines[3].z = 1.0f;
    lines[3].colour = colour;
    lines[4].x = right;
    lines[4].y = bottom;
    lines[4].z = 1.0f;
    lines[4].colour = colour;
    lines[5].x = right;
    lines[5].y = top;
    lines[5].z = 1.0f;
    lines[5].colour = colour;
    lines[6].x = right;
    lines[6].y = top;
    lines[6].z = 1.0f;
    lines[6].colour = colour;
    lines[7].x = left;
    lines[7].y = top;
    lines[7].z = 1.0f;
    lines[7].colour = colour;
    fn_80027824(lines, 0, 8);
}

extern s32 lbl_80407C90;
extern struct nuvec_s lbl_8020D8E8;
void NuCameraEnableClipping(struct nuvec_s* out, f32 x, f32 y);
void NuCameraTransformScreenClip(void);
void NuPs2CreateSkin(void);
void fn_8002782C(u32 colour, f32 x0, f32 y0, f32 z0, f32 x1, f32 y1, f32 z1);

void drawViewFrustrum(void)
{
    struct nuvec_s corners[4];
    struct nuvec_s points[4];
    s32 i;
    if (lbl_80407C90 != 0) {
        NuCameraEnableClipping(&corners[0], -1.0f, -1.0f);
        NuCameraEnableClipping(&corners[1], -1.0f, 1.0f);
        NuCameraEnableClipping(&corners[2], 1.0f, 1.0f);
        NuCameraEnableClipping(&corners[3], 1.0f, -1.0f);
        for (i = 0; i < 4; i++) {
            struct nuvec_s* p = &points[i];
            NuVecScale(p, &corners[i], 0.2f);
            NuVecAdd(p, p, &lbl_8020D8E8);
        }
        NuCameraTransformScreenClip();
        for (i = 0; i < 4; i++) {
            struct nuvec_s* a = &points[i];
            struct nuvec_s* b = &points[(i + 1) % 4];
            fn_8002782C(-1, lbl_8020D8E8.x, lbl_8020D8E8.y, lbl_8020D8E8.z,
                a->x, a->y, a->z);
            fn_8002782C(0xffff0000, a->x, a->y, a->z, b->x, b->y, b->z);
        }
        NuPs2CreateSkin();
    }
}


void drawClippedPortal(NuPortalPlane* portal, NuPortalRoom* room, NuPortalVec3* minimum, NuPortalVec3* maximum)
{
    struct nuvec_s rays[4];
    struct nuvec_s hits[4];
    struct nuvec_s point;
    struct nuvec_s nearest_point;
    f32 nearest;
    f32 distance;
    s32 i;
    s32 j;
    if (lbl_80407C90 != 0) {
        NuCameraEnableClipping(&rays[0], minimum->x, minimum->y);
        NuCameraEnableClipping(&rays[1], minimum->x, maximum->y);
        NuCameraEnableClipping(&rays[2], maximum->x, maximum->y);
        NuCameraEnableClipping(&rays[3], maximum->x, minimum->y);
        for (i = 0; i < 4; i++) {
            NuPlnLine3(portal, &lbl_8020D8E8, &rays[i], &hits[i]);
        }
        NuCameraTransformScreenClip();
        drawSquare(minimum, maximum, 0xff00);
        for (i = 0; i < 4; i++) {
            struct nuvec_s* a = &hits[i];
            struct nuvec_s* b = &hits[(i + 1) % 4];
            fn_8002782C(0xff0000ff, a->x, a->y, a->z, b->x, b->y, b->z);
        }
        for (i = 0; i < 4; i++) {
            NuVecNorm(&rays[i], &rays[i]);
            nearest = 3.4028234e+38f;
            for (j = 0; j < room->plane_count; j++) {
                distance = NuPlnLine3(&room->planes[j], &hits[i], &rays[i], &point);
                if (distance > 0.0f && distance < nearest) {
                    nearest_point = point;
                    nearest = distance;
                }
            }
            fn_8002782C(0xff0000ff, nearest_point.x, nearest_point.y, nearest_point.z,
                hits[i].x, hits[i].y, hits[i].z);
        }
        NuPs2CreateSkin();
    }
}

typedef struct {
    u8 pad0[4];
    NuPortalPortal* portals;
    s32 num_rooms;
    NuPortalRoom* rooms;
} NuPortalRoomSet;
extern char lbl_80190A08[];

static inline s32 portal_room_contains(NuPortalRoom* room, NuPortalVec3* position)
{
    f32 distance = 0.0f;
    s16 j;
    for (j = 0; j < room->plane_count; j++) {
        NuPortalPlane* plane = &room->planes[j];
        distance = position->x * plane->x + position->y * plane->y + position->z * plane->z + plane->d;
        if (distance > 0.0f)
            goto done;
    }
done:
    {
        s32 r = 0;
        if (distance <= 0.0f)
            r = 1;
        return r;
    }
}

s32 NuPortalWhichRoom(u8* scene, NuPortalVec3* position)
{
    NuPortalRoomSet* set = (NuPortalRoomSet*)(scene + 0x88);
    s16 candidates[2] = {0, 0};
    s16 count = 0;
    s16 i;
    s16 j;
    f32 distance;
    NuPortalRoom* room;
    NuPortalPlane* plane;
    NuPortalRoom* first;
    NuPortalRoom* second;
    NuPortalPortal* portal;
    s16 index;
    for (i = 0; i < set->num_rooms; i++) {
        if (portal_room_contains(&set->rooms[i], position)) {
            first = &set->rooms[candidates[0]];
            second = &set->rooms[candidates[1]];
            if (count == 2) {
                if (first->priority == second->priority)
                    fn_80009948(lbl_80190974, 0x20f)(lbl_80190A08);
                if (first->priority < second->priority) {
                    if (first->flags >= 0)
                        fn_80009948(lbl_80190974, 0x214)(lbl_80190A08);
                    candidates[0] = i;
                } else {
                    if (second->flags >= 0)
                        fn_80009948(lbl_80190974, 0x21b)(lbl_80190A08);
                    candidates[1] = i;
                }
            }
            candidates[count++] = i;
            if (count == 2 && first->flags >= 0 && second->flags >= 0)
                break;
        }
    }
    if (count == 1)
        return candidates[0];
    if (count == 0)
        return -1;
    for (i = 0; i < set->rooms[candidates[0]].portal_count; i++) {
        index = set->rooms[candidates[0]].portal_indices[i];
        for (j = 0; j < set->rooms[candidates[1]].portal_count; j++) {
            if (index == set->rooms[candidates[1]].portal_indices[j]) {
                portal = &set->portals[index];
                if (portal->plane.x * position->x + portal->plane.y * position->y + portal->plane.z * position->z
                        + portal->plane.d < 0.0f)
                    return portal->front_room;
                return portal->back_room;
            }
        }
    }
    return -1;
}

s32 clipTestBox(NuPortalVec3* minimum, NuPortalVec3* maximum, NuPortalPlane* planes, s32 count)
{
    NuPortalVec3 corners[8];
    s32 plane_index;
    s32 inside = 0;
    corners[0].x = minimum->x;
    corners[0].y = minimum->y;
    corners[0].z = minimum->z;
    corners[1].x = maximum->x;
    corners[1].y = minimum->y;
    corners[1].z = minimum->z;
    corners[2].x = minimum->x;
    corners[2].y = maximum->y;
    corners[2].z = minimum->z;
    corners[3].x = maximum->x;
    corners[3].y = maximum->y;
    corners[3].z = minimum->z;
    corners[4].x = minimum->x;
    corners[4].y = minimum->y;
    corners[4].z = maximum->z;
    corners[5].x = maximum->x;
    corners[5].y = minimum->y;
    corners[5].z = maximum->z;
    corners[6].x = minimum->x;
    corners[6].y = maximum->y;
    corners[6].z = maximum->z;
    corners[7].x = maximum->x;
    corners[7].y = maximum->y;
    corners[7].z = maximum->z;
    for (plane_index = 0; plane_index < count; plane_index++) {
        s32 outside = 0;
        s32 corner_index;
        for (corner_index = 0; corner_index < 8; corner_index++) {
            f32 distance = planes->x * corners[corner_index].x + planes->y * corners[corner_index].y + planes->z * corners[corner_index].z + planes->d;
            if (distance < lbl_80190A28[0]) outside++;
            else inside++;
        }
        if (outside == 8) return 0;
        planes++;
    }
    {
        s32 result = 1;
        if (inside != count * 8) result = 2;
        return result;
    }
}

void transposeClipPlanes(void *arg0) {
    s16 temp_r0;
    void *temp_r11;

    (*(f32 *)((s8 *)(arg0) + (0xC))) = (f32) lbl_803145D0.x;
    (*(f32 *)((s8 *)(arg0) + (0x1C))) = (f32) lbl_803145D0.y;
    (*(f32 *)((s8 *)(arg0) + (0x2C))) = (f32) lbl_803145D0.z;
    (*(f32 *)((s8 *)(arg0) + (0x3C))) = (f32) lbl_803145D0.d;
    (*(f32 *)((s8 *)(arg0) + (0x40))) = (f32) lbl_803145D0.x;
    (*(f32 *)((s8 *)(arg0) + (0x50))) = (f32) lbl_803145D0.y;
    (*(f32 *)((s8 *)(arg0) + (0x60))) = (f32) lbl_803145D0.z;
    (*(f32 *)((s8 *)(arg0) + (0x70))) = (f32) lbl_803145D0.d;
    (*(f32 *)((s8 *)(arg0) + (0x44))) = (f32) lbl_803145D0.x;
    (*(f32 *)((s8 *)(arg0) + (0x54))) = (f32) lbl_803145D0.y;
    (*(f32 *)((s8 *)(arg0) + (0x64))) = (f32) lbl_803145D0.z;
    (*(f32 *)((s8 *)(arg0) + (0x74))) = (f32) lbl_803145D0.d;
    (*(f32 *)((s8 *)(arg0) + (0x48))) = (f32) lbl_803145D0.x;
    (*(f32 *)((s8 *)(arg0) + (0x58))) = (f32) lbl_803145D0.y;
    (*(f32 *)((s8 *)(arg0) + (0x68))) = (f32) lbl_803145D0.z;
    (*(f32 *)((s8 *)(arg0) + (0x78))) = (f32) lbl_803145D0.d;
    (*(f32 *)((s8 *)(arg0) + (0x4C))) = (f32) lbl_803145D0.x;
    temp_r11 = (*(void **)((s8 *)(arg0) + (0x80)));
    (*(f32 *)((s8 *)(arg0) + (0x5C))) = (f32) lbl_803145D0.y;
    temp_r0 = (*(s16 *)((s8 *)(arg0) + (0x9C)));
    (*(f32 *)((s8 *)(arg0) + (0x6C))) = (f32) lbl_803145D0.z;
    (*(f32 *)((s8 *)(arg0) + (0x7C))) = (f32) lbl_803145D0.d;
    (*(f32 *)((s8 *)(arg0) + (0))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0)));
    (*(f32 *)((s8 *)(arg0) + (0x10))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (4)));
    (*(f32 *)((s8 *)(arg0) + (0x20))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (8)));
    (*(f32 *)((s8 *)(arg0) + (0x30))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0xC)));
    (*(f32 *)((s8 *)(arg0) + (4))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0x10)));
    (*(f32 *)((s8 *)(arg0) + (0x14))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0x14)));
    (*(f32 *)((s8 *)(arg0) + (0x24))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0x18)));
    (*(f32 *)((s8 *)(arg0) + (0x34))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0x1C)));
    (*(f32 *)((s8 *)(arg0) + (8))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0x20)));
    (*(f32 *)((s8 *)(arg0) + (0x18))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0x24)));
    (*(f32 *)((s8 *)(arg0) + (0x28))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0x28)));
    (*(f32 *)((s8 *)(arg0) + (0x38))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0x2C)));
    if (temp_r0 > 3) {
        (*(f32 *)((s8 *)(arg0) + (0xC))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0x30)));
        (*(f32 *)((s8 *)(arg0) + (0x1C))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0x34)));
        (*(f32 *)((s8 *)(arg0) + (0x2C))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0x38)));
        (*(f32 *)((s8 *)(arg0) + (0x3C))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0x3C)));
        if (temp_r0 > 4) {
            (*(f32 *)((s8 *)(arg0) + (0x40))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0x40)));
            (*(f32 *)((s8 *)(arg0) + (0x50))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0x44)));
            (*(f32 *)((s8 *)(arg0) + (0x60))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0x48)));
            (*(f32 *)((s8 *)(arg0) + (0x70))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0x4C)));
            if (temp_r0 > 5) {
                (*(f32 *)((s8 *)(arg0) + (0x44))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0x50)));
                (*(f32 *)((s8 *)(arg0) + (0x54))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0x54)));
                (*(f32 *)((s8 *)(arg0) + (0x64))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0x58)));
                (*(f32 *)((s8 *)(arg0) + (0x74))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0x5C)));
                if (temp_r0 > 6) {
                    (*(f32 *)((s8 *)(arg0) + (0x48))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0x60)));
                    (*(f32 *)((s8 *)(arg0) + (0x58))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0x64)));
                    (*(f32 *)((s8 *)(arg0) + (0x68))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0x68)));
                    (*(f32 *)((s8 *)(arg0) + (0x78))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0x6C)));
                    if (temp_r0 > 7) {
                        (*(f32 *)((s8 *)(arg0) + (0x4C))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0x70)));
                        (*(f32 *)((s8 *)(arg0) + (0x5C))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0x74)));
                        (*(f32 *)((s8 *)(arg0) + (0x6C))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0x78)));
                        (*(f32 *)((s8 *)(arg0) + (0x7C))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0x7C)));
                    }
                }
            }
        }
    }
}

typedef struct {
    u8 pad0[0x40];
    f32 fov;
    f32 aspect;
} NuPortalCamera;
extern NuPortalCamera* lbl_8040A320;
extern struct numtx_s lbl_8020D898;
f32 NuSinApprox(s32 angle);
f32 NuCosApprox(s32 angle);
void NuTexLoadTextureFromDDSFile(NuPortalPlane* out, NuPortalPlane* in, struct numtx_s* m);

static inline f32 portal_tan(f32 x)
{
    s32 angle = (s32)x;
    return NuSinApprox(angle) / NuCosApprox(angle);
}

NuPortalFrustum* buildFrustrum(NuPortalVec3* minimum, NuPortalVec3* maximum, s16 room_id)
{
    NuPortalFrustum* frustum = NuFrustrumAlloc(4);
    NuPortalPlane* planes;
    NuPortalVec3 pos;
    f32 tangent, horizontal, edge, inv;
    s32 angle;
    planes = frustum->planes;
    frustum->minimum = *minimum;
    frustum->maximum = *maximum;
    frustum->room_id = room_id;
    tangent = portal_tan(lbl_8040A320->fov * 0.5f * 10430.378f);
    horizontal = tangent / lbl_8040A320->aspect;

    edge = tangent * minimum->y;
    inv = 1.0f / NuFsqrt(edge * edge + 1.0f);
    planes[2].x = 0.0f;
    planes[2].d = 0.0f;
    planes[2].y = inv;
    planes[2].z = -edge * inv;

    edge = tangent * maximum->y;
    inv = 1.0f / NuFsqrt(edge * edge + 1.0f);
    planes[3].x = 0.0f;
    planes[3].d = 0.0f;
    planes[3].z = edge * inv;
    planes[3].y = -inv;

    edge = horizontal * maximum->x;
    inv = 1.0f / NuFsqrt(edge * edge + 1.0f);
    planes[0].y = 0.0f;
    planes[0].d = 0.0f;
    planes[0].z = edge * inv;
    planes[0].x = -inv;

    edge = horizontal * minimum->x;
    inv = 1.0f / NuFsqrt(edge * edge + 1.0f);
    planes[1].y = 0.0f;
    planes[1].d = 0.0f;
    planes[1].x = inv;
    planes[1].z = -edge * inv;

    pos.x = lbl_8020D898._30;
    pos.y = lbl_8020D898._31;
    pos.z = lbl_8020D898._32;
    NuTexLoadTextureFromDDSFile(&planes[0], &planes[0], &lbl_8020D898);
    planes[0].d = -(pos.x * planes[0].x + pos.y * planes[0].y + pos.z * planes[0].z);
    NuTexLoadTextureFromDDSFile(&planes[1], &planes[1], &lbl_8020D898);
    planes[1].d = -(pos.x * planes[1].x + pos.y * planes[1].y + pos.z * planes[1].z);
    NuTexLoadTextureFromDDSFile(&planes[2], &planes[2], &lbl_8020D898);
    planes[2].d = -(pos.x * planes[2].x + pos.y * planes[2].y + pos.z * planes[2].z);
    NuTexLoadTextureFromDDSFile(&planes[3], &planes[3], &lbl_8020D898);
    planes[3].d = -(pos.x * planes[3].x + pos.y * planes[3].y + pos.z * planes[3].z);
    transposeClipPlanes(frustum);
    return frustum;
}
