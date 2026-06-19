typedef float f32;
typedef struct { f32 x, y, z; } NuVec;

void NuVecScale(NuVec* dst, const NuVec* src, f32 scale) {
    dst->x = src->x * scale;
    dst->y = src->y * scale;
    dst->z = src->z * scale;
}

void NuVecScaleAccum(NuVec* dst, const NuVec* accum, const NuVec* src, f32 scale) {
    dst->x = src->x * scale + accum->x;
    dst->y = src->y * scale + accum->y;
    dst->z = src->z * scale + accum->z;
}

f32 NuVecDot(const NuVec* a, const NuVec* b) {
    return a->x * b->x + a->y * b->y + a->z * b->z;
}
