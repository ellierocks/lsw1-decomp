#include "numath/numath.h"
#include "numath/nuvec_ps.h"

struct fn_800127D8_inner {
    u8 pad0[0x8];
    s32 f8;
    u8 padC[0x8];
    s32 f14;
    u8 pad18[0x6];
    s16 f1e;
    s32 f20;
    s16 f24;
    s16 f26;
    s16 f28;
};

struct fn_800127D8_outer {
    u8 pad0[0x8];
    struct fn_800127D8_inner* inner;
    u8 padC[0x4];
    s32 f10;
    s32 f14;
};

s32 fn_8000D010();

void fn_8000EFE0(struct numtx_s*, struct numtx_s*, struct numtx_s*);

s32 fn_8000F65C(s32, s32, s32);
extern s32 lbl_80407B84;

extern f64 nan();

void NuVec4MtxTransformVU0x3(struct nuvec4_s* dst, struct nuvec4_s* src, struct numtx_s* m)
{
    s32 i;
    for (i = 0; i < 3; i++) {
        f32 x = src->x * m->_00 + src->y * m->_10 + src->z * m->_20 + src->w * m->_30;
        f32 y = src->x * m->_01 + src->y * m->_11 + src->z * m->_21 + src->w * m->_31;
        f32 z = src->x * m->_02 + src->y * m->_12 + src->z * m->_22 + src->w * m->_32;
        f32 w = src->x * m->_03 + src->y * m->_13 + src->z * m->_23 + src->w * m->_33;
        dst->x = x;
        dst->y = y;
        dst->z = z;
        dst->w = w;
        src++;
        dst++;
    }
}

void NuVec4MtxTransformVU0x4(struct numtx_s *arg0, struct nuvec_s *arg1) {
    NuMtxScale(arg0, arg1);
}

void fn_8000EA94(struct numtx_s *arg0, struct nuvec_s *arg1) {
    NuMtxPreScale(arg0, arg1);
}

void NuMtxPreScaleUVU0(struct numtx_s* m, f32 scale)
{
    f32 a = m->_00 * scale;
    f32 b = m->_01 * scale;
    f32 c = m->_02 * scale;
    f32 d = m->_10 * scale;
    f32 e = m->_11 * scale;
    f32 f = m->_12 * scale;
    f32 g = m->_20 * scale;
    f32 h = m->_21 * scale;
    f32 i = m->_22 * scale;
    m->_00 = a;
    m->_01 = b;
    m->_02 = c;
    m->_10 = d;
    m->_11 = e;
    m->_12 = f;
    m->_20 = g;
    m->_21 = h;
    m->_22 = i;
}

void fn_8000EB24(void) {
    fn_8000D010();
}

void fn_8000EB44(struct numtx_s *arg0, struct numtx_s *arg1, struct numtx_s *arg2) {
    NuMtxMul(arg0, arg1, arg2);
}

void fn_8000EB64(struct numtx_s* dst, struct numtx_s* a, struct numtx_s* b, s32 count)
{
    struct numtx_s* right;
    struct numtx_s* left;
    struct numtx_s* out;
    s32 remaining;
    if (count > 0) {
        right = b;
        left = a;
        out = dst;
        remaining = count;
        do {
            NuMtxMul(out, left, right);
            left++;
            right++;
            out++;
            remaining--;
        } while (remaining != 0);
    }
}

void fn_8000EBC8(struct numtx_s* dst, struct numtx_s* a, struct numtx_s* b) {
    fn_8000EFE0(dst, a, b);
}

void fn_8000EBE8(struct numtx_s* dst, struct numtx_s* src)
{
    f32 x = -src->_30;
    f32 y = -src->_31;
    f32 z = -src->_32;
    f32 t;
    t = src->_01;
    dst->_01 = src->_10;
    dst->_10 = t;
    t = src->_02;
    dst->_02 = src->_20;
    dst->_20 = t;
    t = src->_12;
    dst->_12 = src->_21;
    dst->_21 = t;
    dst->_00 = src->_00;
    dst->_11 = src->_11;
    dst->_22 = src->_22;
    dst->_30 = x * dst->_00 + y * dst->_10 + z * dst->_20;
    dst->_31 = x * dst->_01 + y * dst->_11 + z * dst->_21;
    dst->_32 = x * dst->_02 + y * dst->_12 + z * dst->_22;
    dst->_03 = dst->_13 = dst->_23 = 0.0f;
    dst->_33 = 1.0f;
}

// Nothing in retail references this stripped constant-pool word.
static const u32 unk_8018F134[1] __attribute__((section(".rodata"))) = { 0 };

f32 NuFsqrt(f32 x) {
    f32 y;
    f32 h;

    if (x > 0.0f) {
        asm("frsqrte %0, %1" : "=f"(y) : "f"(x));
        h = x * 0.5f;
        y = y * (1.5f - h * y * y);
        y = y * (1.5f - h * y * y);
        y = y * (1.5f - h * y * y);
        y = y * (1.5f - h * y * y);
        return y * x;
    }
    if (x == 0.0f) {
        return 0.0f;
    }
    return nan();
}

static const u32 unk_8018F144[2] __attribute__((section(".rodata"))) = { 0, 0x3F800000 };

f32 fn_8000ED40(f32 x) {
    if (x < 0.0f) {
        return -x;
    }
    return x;
}

f32 NuFdiv(f32 numerator, f32 denominator) {
    if ((denominator == 0.0f) || (numerator == 0.0f)) {
        return 0.0f;
    }
    return numerator / denominator;
}

void NuVecMtxTransform(struct nuvec_s *dst, struct nuvec_s *src, struct numtx_s *mtx) {
    NuVecMtxTransformPS(dst, src, mtx);
}

void NuVecAdd(struct nuvec_s *arg0, struct nuvec_s *arg1, struct nuvec_s *arg2) {
    NuVecAddPS(arg0, arg1, arg2);
}

void NuVecSub(struct nuvec_s *arg0, struct nuvec_s *arg1, struct nuvec_s *arg2) {
    NuVecSubPS(arg0, arg1, arg2);
}

void NuVecCross(struct nuvec_s* dst, struct nuvec_s* a, struct nuvec_s* b)
{
    /* Retail uses Gekko paired-single operations. */
    asm volatile(
        "psq_l 2, 0x0(%2), 0, 0\n\t"
        "lfs 3, 0x8(%1)\n\t"
        "psq_l 1, 0x0(%1), 0, 0\n\t"
        "ps_merge10 7, 2, 2\n\t"
        "lfs 4, 0x8(%2)\n\t"
        "ps_mul 5, 2, 3\n\t"
        "ps_muls0 8, 2, 1\n\t"
        "ps_msub 6, 1, 4, 5\n\t"
        "ps_msub 9, 1, 7, 8\n\t"
        "ps_merge11 10, 6, 6\n\t"
        "ps_merge01 11, 6, 9\n\t"
        "psq_st 10, 0x0(%0), 1, 0\n\t"
        "ps_neg 11, 11\n\t"
        "psq_st 11, 0x4(%0), 0, 0\n\t"
        :
        : "b"(dst), "b"(a), "b"(b)
        : "fr1", "fr2", "fr3", "fr4", "fr5", "fr6", "fr7", "fr8", "fr9", "fr10", "fr11", "memory");
}

f32 NuVecMagSqr(struct nuvec_s* v)
{
    register f32 result asm("fr1");
    /* Retail uses Gekko paired-single operations. */
    asm volatile(
        "psq_l 2, 0x0(%1), 0, 0\n\t"
        "ps_mul 2, 2, 2\n\t"
        "lfs 3, 0x8(%1)\n\t"
        "ps_madd %0, 3, 3, 2\n\t"
        "ps_sum0 %0, %0, 2, 2\n\t"
        : "=f"(result)
        : "b"(v)
        : "fr2", "fr3", "memory");
    return result;
}

void fn_8000EE70(struct nuvec4_s* dst, struct nuvec4_s* src, struct numtx_s* m)
{
    f32 x = src->x * m->_00 + src->y * m->_10 + src->z * m->_20;
    f32 y = src->x * m->_01 + src->y * m->_11 + src->z * m->_21;
    f32 z = src->x * m->_02 + src->y * m->_12 + src->z * m->_22;
    dst->x = x;
    dst->y = y;
    dst->z = z;
    dst->w = src->w;
}

void NuMtxMul(struct numtx_s* dst, struct numtx_s* a, struct numtx_s* b)
{
    /* Retail uses Gekko paired-single operations. */
    asm volatile(
        "psq_l 0, 0x0(%1), 0, 0\n\t"
        "psq_l 2, 0x0(%2), 0, 0\n\t"
        "ps_muls0 6, 2, 0\n\t"
        "psq_l 3, 0x10(%2), 0, 0\n\t"
        "psq_l 4, 0x20(%2), 0, 0\n\t"
        "ps_madds1 6, 3, 0, 6\n\t"
        "psq_l 1, 0x8(%1), 0, 0\n\t"
        "psq_l 5, 0x30(%2), 0, 0\n\t"
        "ps_madds0 6, 4, 1, 6\n\t"
        "psq_l 0, 0x10(%1), 0, 0\n\t"
        "ps_madds1 6, 5, 1, 6\n\t"
        "psq_l 1, 0x18(%1), 0, 0\n\t"
        "ps_muls0 8, 2, 0\n\t"
        "ps_madds1 8, 3, 0, 8\n\t"
        "psq_l 0, 0x20(%1), 0, 0\n\t"
        "ps_madds0 8, 4, 1, 8\n\t"
        "ps_madds1 8, 5, 1, 8\n\t"
        "psq_l 1, 0x28(%1), 0, 0\n\t"
        "ps_muls0 10, 2, 0\n\t"
        "ps_madds1 10, 3, 0, 10\n\t"
        "psq_l 0, 0x30(%1), 0, 0\n\t"
        "ps_madds0 10, 4, 1, 10\n\t"
        "ps_madds1 10, 5, 1, 10\n\t"
        "psq_l 1, 0x38(%1), 0, 0\n\t"
        "ps_muls0 12, 2, 0\n\t"
        "psq_l 2, 0x8(%2), 0, 0\n\t"
        "ps_madds1 12, 3, 0, 12\n\t"
        "psq_l 0, 0x0(%1), 0, 0\n\t"
        "ps_madds0 12, 4, 1, 12\n\t"
        "psq_l 3, 0x18(%2), 0, 0\n\t"
        "ps_madds1 12, 5, 1, 12\n\t"
        "psq_l 1, 0x8(%1), 0, 0\n\t"
        "ps_muls0 7, 2, 0\n\t"
        "psq_l 4, 0x28(%2), 0, 0\n\t"
        "ps_madds1 7, 3, 0, 7\n\t"
        "psq_l 5, 0x38(%2), 0, 0\n\t"
        "ps_madds0 7, 4, 1, 7\n\t"
        "psq_l 0, 0x10(%1), 0, 0\n\t"
        "ps_madds1 7, 5, 1, 7\n\t"
        "psq_l 1, 0x18(%1), 0, 0\n\t"
        "ps_muls0 9, 2, 0\n\t"
        "psq_st 6, 0x0(%0), 0, 0\n\t"
        "ps_madds1 9, 3, 0, 9\n\t"
        "psq_l 0, 0x20(%1), 0, 0\n\t"
        "ps_madds0 9, 4, 1, 9\n\t"
        "psq_st 8, 0x10(%0), 0, 0\n\t"
        "ps_madds1 9, 5, 1, 9\n\t"
        "psq_l 1, 0x28(%1), 0, 0\n\t"
        "ps_muls0 11, 2, 0\n\t"
        "psq_st 10, 0x20(%0), 0, 0\n\t"
        "ps_madds1 11, 3, 0, 11\n\t"
        "psq_l 0, 0x30(%1), 0, 0\n\t"
        "ps_madds0 11, 4, 1, 11\n\t"
        "psq_st 12, 0x30(%0), 0, 0\n\t"
        "ps_madds1 11, 5, 1, 11\n\t"
        "psq_l 1, 0x38(%1), 0, 0\n\t"
        "ps_muls0 13, 2, 0\n\t"
        "psq_st 7, 0x8(%0), 0, 0\n\t"
        "ps_madds1 13, 3, 0, 13\n\t"
        "psq_st 9, 0x18(%0), 0, 0\n\t"
        "ps_madds0 13, 4, 1, 13\n\t"
        "psq_st 11, 0x28(%0), 0, 0\n\t"
        "ps_madds1 13, 5, 1, 13\n\t"
        "psq_st 13, 0x38(%0), 0, 0\n\t"
        :
        : "b"(dst), "b"(a), "b"(b)
        : "fr0", "fr1", "fr2", "fr3", "fr4", "fr5", "fr6", "fr7", "fr8", "fr9", "fr10", "fr11", "fr12", "fr13", "memory");
}

#include "numath/VuMath.h"

// An additional TU header copy retained by the retail linker; unreferenced.
static const char unk_8018F1A0[] = "../nu2api.legogc/numath/ngc/VuMath.h";
static const char unk_8018F1C8[] = "VuMtxInitRot NOT fully implemented!";

void fn_8000EFE0(struct numtx_s* dst, struct numtx_s* a, struct numtx_s* b)
{
    /* Retail multiplies the 3x3 basis with paired-single operations. */
    asm volatile(
        "psq_l 0, 0x0(%1), 0, 0\n\t"
        "psq_l 2, 0x0(%2), 0, 0\n\t"
        "psq_l 3, 0x10(%2), 0, 0\n\t"
        "ps_muls0 6, 2, 0\n\t"
        "psq_l 4, 0x20(%2), 0, 0\n\t"
        "ps_madds1 6, 3, 0, 6\n\t"
        "psq_l 1, 0x8(%1), 1, 0\n\t"
        "ps_madds0 6, 4, 1, 6\n\t"
        "psq_l 5, 0x8(%2), 1, 0\n\t"
        "psq_st 6, 0x0(%0), 0, 0\n\t"
        "ps_mul 9, 5, 0\n\t"
        "psq_l 7, 0x18(%2), 1, 0\n\t"
        "psq_l 8, 0x28(%2), 1, 0\n\t"
        "ps_madds1 9, 7, 0, 9\n\t"
        "psq_l 0, 0x10(%1), 0, 0\n\t"
        "ps_madds0 9, 8, 1, 9\n\t"
        "ps_muls0 6, 2, 0\n\t"
        "psq_l 1, 0x18(%1), 1, 0\n\t"
        "ps_madds1 6, 3, 0, 6\n\t"
        "psq_st 9, 0x8(%0), 1, 0\n\t"
        "ps_madds0 6, 4, 1, 6\n\t"
        "ps_mul 9, 5, 0\n\t"
        "psq_st 6, 0x10(%0), 0, 0\n\t"
        "ps_madds1 9, 7, 0, 9\n\t"
        "psq_l 0, 0x20(%1), 0, 0\n\t"
        "ps_madds0 9, 8, 1, 9\n\t"
        "ps_muls0 6, 2, 0\n\t"
        "psq_l 1, 0x28(%1), 1, 0\n\t"
        "ps_madds1 6, 3, 0, 6\n\t"
        "psq_st 9, 0x18(%0), 1, 0\n\t"
        "ps_mul 9, 5, 0\n\t"
        "ps_madds0 6, 4, 1, 6\n\t"
        "ps_madds1 9, 7, 0, 9\n\t"
        "psq_st 6, 0x20(%0), 0, 0\n\t"
        "ps_madds0 9, 8, 1, 9\n\t"
        "psq_st 9, 0x28(%0), 1, 0\n\t"
        :
        : "b"(dst), "b"(a), "b"(b)
        : "fr0", "fr1", "fr2", "fr3", "fr4", "fr5", "fr6", "fr7", "fr8", "fr9", "memory");
    dst->_30 = 0.0f;
    dst->_33 = 1.0f;
    dst->_31 = 0.0f;
    dst->_32 = 0.0f;
}

void fn_8000F840(void *arg0) {
    s32 *temp_r4;
    s32 var_r0;

    temp_r4 = (*(s32 **)((s8 *)(arg0) + (4)));
    if ((temp_r4 != NULL) && ((s32) lbl_80407B84 != 0)) {
        fn_8000F65C((*(s32 *)((s8 *)(arg0) + (0x20))), *temp_r4, (*(s32 *)((s8 *)(arg0) + (0))));
        var_r0 = (*(s32 *)((s8 *)(arg0) + (0x28))) | 0x20000000;
    } else {
        var_r0 = (*(s32 *)((s8 *)(arg0) + (0x28))) & 0xDFFFFFFF;
    }
    (*(s32 *)((s8 *)(arg0) + (0x28))) = var_r0;
}

s32 fn_80011EC4(u8 *, s32, s32, s32, s32);
s32 fn_8001C00C();
s32 fn_8001C110();
extern s32 lbl_8018F860[];
extern s32 lbl_8018F87C[];
extern s32 lbl_8018F89C[];
extern s32 lbl_80407BA0;
extern s32 lbl_8040ACF4;

struct nudlist_dump_name_s {
    char* name;
};
struct nudlist_dump_s {
    s32 count;
    u8* commands;
    u8 pad8[0x14];
    struct nudlist_dump_name_s* info;
};
void fn_80012420(struct nudlist_dump_s* scene)
{
    struct nudlist_dump_name_s* info;
    s32 flags;
    s32 ordinal;
    u8* command;
    if (lbl_80407BA0 != 0) {
        info = scene->info;
        if ((info != NULL) && (info->name != NULL)) {
            fn_8001C110(lbl_8018F87C, info->name, scene->count);
        } else {
            fn_8001C110(lbl_8018F89C, scene->count);
        }
        ordinal = 0;
        fn_8001C00C(lbl_8018F860);
        command = scene->commands;
        flags = lbl_8040ACF4;
        if (command != NULL) {
            do {
                fn_80011EC4(command, ordinal, 0, 0, flags);
                if (command[1] == 1) {
                    command = *(u8**)(command + 4);
                } else {
                    command += 8;
                }
                ordinal++;
            } while (*command != 0x84);
            fn_80011EC4(command, ordinal, 0, 0, flags);
        }
    }
}

void fn_800127D8(struct fn_800127D8_outer* o)
{
    struct fn_800127D8_inner* p = o->inner;
    s32 m = -1;
    p->f8 = 0;
    p->f14 = m;
    p->f20 = m;
    p->f24 = m;
    p->f26 = m;
    p->f28 = m;
    p->f1e = m;
    o->f14 = o->f10;
}

struct nudlist_state_s {
    u8 pad0[0x450];
    u32 materialCount;
    struct numtl_s** materials;
    u8 pad458[0x38];
    s32 insertCount;
    struct numtl_s** inserts;
    s32 deleteCount;
    struct numtl_s** deletes;
    u8 pad4a0[0xa0];
    s32 mutex;
};
extern struct nudlist_state_s lbl_801B0F88;
void fn_8000A834(s32);
void fn_8000A86C(s32);
extern char lbl_8018F3E0[];
extern char lbl_8018F40C[];
void fn_80012AB4(struct numtl_s* material)
{
    struct nudlist_state_s* state = &lbl_801B0F88;
    fn_8000A834(state->mutex);
    if (state->insertCount == 64) {
        fn_80009948(lbl_8018F3E0, 0x3ef)(lbl_8018F40C);
    } else {
        state->inserts[state->insertCount] = material;
        state->insertCount++;
        fn_8000A86C(state->mutex);
    }
}

extern char lbl_8018F438[];
void fn_80012B44(struct numtl_s* material)
{
    struct nudlist_state_s* state = &lbl_801B0F88;
    s32 index = -1;
    u32 i;
    s32 count;
    fn_8000A834(state->mutex);
    if (*(s32*)((u8*)material + 0xcc) == 0) {
        goto unlock;
    }
    for (i = 0; i < state->materialCount; i++) {
        if (state->materials[i] == material) {
            index = i;
            break;
        }
    }
    if (index == -1) {
        goto unlock;
    }
    /* Preserve the retail unlock block and GCC loop notes for allocation. */
    do {
        if ((count = state->deleteCount) == 64) {
            fn_80009948(lbl_8018F3E0, 0x420)(lbl_8018F438);
unlock:
            fn_8000A86C(state->mutex);
            return;
        }
    } while (0);
    state->deletes[count] = material;
    state->deleteCount++;
    fn_8000A86C(state->mutex);
}
