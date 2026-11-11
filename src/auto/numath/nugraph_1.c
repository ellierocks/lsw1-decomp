#include "types.h"
#include "numath/numath.h"
extern s32 lbl_80407B40;
f32 fn_8000CB50(struct nuvec_s* v);
f32 fn_8000CD08(struct nuvec_s* a, struct nuvec_s* b, struct nuvec_s* out);
static inline f32 LineEx(struct nuvec_s* a, struct nuvec_s* b, struct nuvec_s* c, struct nuvec_s* out)
{
    f32 len;
    f32 d;
    f32 dist;
    struct nuvec_s dir;
    struct nuvec_s w;
    struct nuvec_s pt;
    struct nuvec_s tmp;
    NuVecSub(&dir, b, a);
    len = fn_8000CB50(&dir);
    NuVecScale(&dir, &dir, 1.0f / len);
    NuVecSub(&w, c, a);
    d = NuVecDot(&dir, &w);
    NuVecScale(&pt, &dir, d);
    NuVecAdd(&pt, &pt, a);
    dist = fn_8000CD08(&pt, c, &tmp);
    if (out != 0)
        *out = pt;
    return dist;
}

f32 NuPlnLine3(f32* pln, f32* p, f32* dir, f32* out)
{
    f32 d;
    f32 den;
    f32 t;
    NuVecNorm(dir, dir);
    d = -pln[3];
    den = dir[0] * pln[0] + dir[1] * pln[1] + dir[2] * pln[2];
    t = ((pln[0] * d - p[0]) * pln[0] + (pln[1] * d - p[1]) * pln[1] + (pln[2] * d - p[2]) * pln[2]) / den;
    out[0] = p[0] + t * dir[0];
    out[1] = p[1] + t * dir[1];
    out[2] = p[2] + t * dir[2];
    return t;
}

f32 fn_8000BDD0(struct nuvec_s* a, struct nuvec_s* b, struct nuvec_s* c)
{
    return LineEx(a, b, c, 0);
}

// Retail .rodata keeps the constant pools of functions the linker dead-stripped
// from this file. Nothing in the DOL references these bytes; they are carried
// verbatim at their definition points (docs/build_config.md, "Stripped constant pools").
static const u32 unk_8018E99C[1] __attribute__((section(".rodata"))) = { 0x3F800000 };

s32 fn_8000BE70(f32* v, f32* min, f32* max)
{
    s32 code = 0;
    if (v[0] >= min[0]) {
        code = 1;
    } else if (v[0] <= max[0]) {
        code = 8;
    }
    if (v[1] >= min[1]) {
        code |= 2;
    } else if (v[1] <= max[1]) {
        code |= 0x10;
    }
    if (v[2] >= min[2]) {
        code |= 4;
    } else if (v[2] <= max[2]) {
        code |= 0x20;
    }
    return code;
}

void fn_8000BF0C(f32* dst, f32* a, f32* b, f32* t)
{
    f32 d;
    if (t != 0) {
        dst[0] = *t;
    } else {
        dst[0] = 0.0f;
    }
    d = dst[0] - a[0];
    dst[1] = a[1] + d * b[1] / b[0];
    dst[2] = a[2] + d * b[2] / b[0];
}

void fn_8000BF70(f32* dst, f32* a, f32* b, f32* t)
{
    f32 d;
    if (t != 0) {
        dst[1] = *t;
    } else {
        dst[1] = 0.0f;
    }
    d = dst[1] - a[1];
    dst[0] = a[0] + d * b[0] / b[1];
    dst[2] = a[2] + d * b[2] / b[1];
}

void fn_8000BFD4(f32* dst, f32* a, f32* b, f32* t)
{
    f32 d;
    if (t != 0) {
        dst[2] = *t;
    } else {
        dst[2] = 0.0f;
    }
    d = dst[2] - a[2];
    dst[0] = a[0] + d * b[0] / b[2];
    dst[1] = a[1] + d * b[1] / b[2];
}

// This source file ends here. 0x8018E9B0 starts another one's .rodata with its
// VuMath.h strings, after the zero padding that 8-aligns each TU's .rodata.
static const u32 unk_8018E9AC[1] __attribute__((section(".rodata"))) = { 0 };
#include "numath/VuMath.h"

f32 fn_8000C038(void)
{
    union {
        s32 i;
        f32 f;
    } u;
    s32 v;
    v = lbl_80407B40 * 1664525 + 1013904223;
    lbl_80407B40 = v;
    u.i = (v & 0x7fffff) | 0x3f800000;
    return u.f - 1.0f;
}

f32 NuRandIntSeeded(s32* seed)
{
    union {
        s32 i;
        f32 f;
    } u;
    s32 v;
    v = *seed * 1664525 + 1013904223;
    *seed = v;
    u.i = (v & 0x7fffff) | 0x3f800000;
    return u.f - 1.0f;
}

// Stripped pools, then the .rodata of two more source files whose code is all
// dead-stripped (VuMath.h copies at 0x8018EA18 and 0x8018EB98; nugraph.c's
// __FILE__ and messages), then the VuMath.h copy at 0x8018EED8 that starts the
// source file whose code begins in nugraph_2. Nothing in the DOL references these bytes.
static const u32 unk_8018EA04[5] = { 0, 0x43300000, 0x80000000, 0x30000000, 0 };
static const char unk_8018EA18[] = "../nu2api.legogc/numath/ngc/VuMath.h";
static const char unk_8018EA40[] = "VuMtxInitRot NOT fully implemented!";
static const u32 unk_8018EA64[77] = { 0x3F800000, 0x40400000, 0, 0x3F800000, 0x40400000, 0xC0400000, 0x41100000, 0xC1100000, 0x40C00000, 0x41900000, 0xC1400000, 0xC0C00000, 0xC1900000, 0, 0x42100000, 0xC2900000, 0x43100000, 0xC1900000, 0x42100000, 0x41900000, 0xC2100000, 0xC1900000, 0x41900000, 0x42100000, 0xC2100000, 0xC2100000, 0x42900000, 0x42D80000, 0xC3580000, 0xC2D80000, 0x43580000, 0x42100000, 0xC2900000, 0xBF800000, 0x40400000, 0xC0400000, 0x3F800000, 0x40400000, 0xC0C00000, 0x40400000, 0, 0xC0400000, 0x40400000, 0, 0, 0x3F800000, 0, 0, 0, 0xC0400000, 0x40400000, 0xC0400000, 0x40400000, 0x40C00000, 0xC1400000, 0xC0C00000, 0x41900000, 0xC1900000, 0x40C00000, 0x40C00000, 0xC1400000, 0xC0C00000, 0x41900000, 0xC1900000, 0x40C00000, 0x3F800000, 0x40000000, 0xC0800000, 0x40000000, 0xC0800000, 0x40000000, 0xC0800000, 0x40400000, 0x40400000, 0x40C00000, 0x3F800000, 0x3F800000 };
static const char unk_8018EB98[] = "../nu2api.legogc/numath/ngc/VuMath.h";
static const char unk_8018EBC0[] = "VuMtxInitRot NOT fully implemented!";
static const char unk_8018EBE4[] = "../nu2api.legogc/numath/ngc_obj/../nugraph.c";
static const char unk_8018EC14[] = "nugraph: Cannot add point, too many already";
static const char unk_8018EC40[] = "nugraph: Cannot add point, less than 2 existing!";
static const char unk_8018EC74[] = "nugraph: Cannot add point, one here already.";
static const u32 unk_8018ECA4[5] = { 0, 0x43300000, 0x80000000, 0x3F800000, 0 };
static const char unk_8018ECB8[] = "Not enough control points!";
static const u32 unk_8018ECD4[17] = { 0x3DCCCCCD, 0x43300000, 0x80000000, 0x40A00000, 0x40800000, 0x40400000, 0x3F000000, 0, 0, 0x43300000, 0x80000000, 0, 0, 0x43300000, 0x80000000, 0x3F800000, 0 };
static const char unk_8018ED18[] = "Out of memory for calculating graph curve!";
static const u32 unk_8018ED44[4] = { 0, 0x43300000, 0x80000000, 0x3F800000 };
static const char unk_8018ED54[] = "B-Spline runtime evaluation not implented yet - ask ChrisP nicely :)";
static const char unk_8018ED9C[] = "nugraphGetYatX(): Couldn't find point %.02f in graph";
static const u32 unk_8018EDD4[7] = { 0, 0x3F800000, 0x3F000000, 0x3DCCCCCD, 0x40A00000, 0x40800000, 0x40400000 };
static const char unk_8018EDF0[] = "nugraphGenerateLookupTable: tx<x1";
static const char unk_8018EE14[] = "nugraphGenerateLookupTable: x2<tx";
static const char unk_8018EE38[] = "nugraphGenerateLookupTable: value > 1.0";
static const u32 unk_8018EE60[30] = { 0, 0, 0x43300000, 0x80000000, 0x3F800000, 0x3C257EB5, 0x3F000000, 0, 0x3F800000, 0x41200000, 0x43300000, 0x80000000, 0x3F000000, 0, 0x43300000, 0x80000000, 0x3F000000, 0, 0, 0, 0, 0, 0x43300000, 0x80000000, 0, 0, 0x43300000, 0x80000000, 0x3F800000, 0 };
static const char unk_8018EED8[] = "../nu2api.legogc/numath/ngc/VuMath.h";
static const char unk_8018EF00[] = "VuMtxInitRot NOT fully implemented!";
