typedef float f32;
typedef struct { f32 x, y, z; } NuVec;

void NuVecScale_v1(NuVec* dst, const NuVec* src, f32 scale) {
    dst->x = src->x * scale;
    dst->y = src->y * scale;
    dst->z = src->z * scale;
}

void NuVecScale_v2(NuVec* dst, const NuVec* src, f32 scale) {
    f32 x, y, z;
    x = src->x * scale;
    y = src->y * scale;
    z = src->z * scale;
    dst->x = x;
    dst->y = y;
    dst->z = z;
}

f32 NuVecDot_v2(const NuVec* a, const NuVec* b) {
    return a->y * b->y + a->x * b->x + a->z * b->z;
}
