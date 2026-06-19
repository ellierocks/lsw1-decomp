typedef float f32;
typedef signed int s32;

struct nuvec_s { f32 x, y, z; };

/* exact crashwoc signatures */
void NuVecScale(f32 scale, struct nuvec_s* dest, struct nuvec_s* v)
{
    dest->x = v->x * scale;
    dest->y = v->y * scale;
    dest->z = v->z * scale;
}

void NuVecScaleAccum_crash(f32 scale, struct nuvec_s* dest, struct nuvec_s* v)
{
    dest->x = v->x * scale + dest->x;
    dest->y = v->y * scale + dest->y;
    dest->z = v->z * scale + dest->z;
}

f32 NuVecDot(struct nuvec_s* a, struct nuvec_s* b)
{
    return a->x * b->x + a->y * b->y + a->z * b->z;
}
