typedef float f32;
typedef struct { f32 x, y, z; } NuVec;

/* try explicit tmp to get f13 for first component */
void NuVecScale_tmp(NuVec* dst, const NuVec* src, f32 scale) {
    f32 tmp;
    tmp = src->x * scale; dst->x = tmp;
    tmp = src->y * scale; dst->y = tmp;
    tmp = src->z * scale; dst->z = tmp;
}

/* try with volatile ptr dereference to force sequential load */
void NuVecScale_ptr(NuVec* dst, const NuVec* src, f32 scale) {
    const f32* s = (const f32*)src;
    f32* d = (f32*)dst;
    d[0] = s[0] * scale;
    d[1] = s[1] * scale;
    d[2] = s[2] * scale;
}

/* NuVecDot - try explicit tmp accumulation */
f32 NuVecDot_tmp(const NuVec* a, const NuVec* b) {
    f32 dot;
    dot = a->y * b->y;
    dot += a->x * b->x;
    dot += a->z * b->z;
    return dot;
}
