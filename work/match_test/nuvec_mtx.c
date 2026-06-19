typedef float f32;

struct nuvec_s { f32 x, y, z; };
struct Mtx {
    f32 m11, m12, m13, m14;
    f32 m21, m22, m23, m24;
    f32 m31, m32, m33, m34;
    f32 m41, m42, m43, m44;
};

/* test 1: explicit temporaries */
void NuVecMtxRotate_v1(struct nuvec_s* dest, struct nuvec_s* a, struct Mtx* b)
{
    f32 ax = a->x, ay = a->y, az = a->z;
    dest->x = ax * b->m11 + ay * b->m21 + az * b->m31;
    dest->y = ax * b->m12 + ay * b->m22 + az * b->m32;
    dest->z = ax * b->m13 + ay * b->m23 + az * b->m33;
}

/* test 2: reorder by inner term (y group first) */
void NuVecMtxRotate_v2(struct nuvec_s* dest, struct nuvec_s* a, struct Mtx* b)
{
    f32 ax = a->x, ay = a->y, az = a->z;
    f32 px = ay * b->m21, py = ay * b->m22, pz = ay * b->m23;
    px = ax * b->m11 + px;
    py = ax * b->m12 + py;
    pz = ax * b->m13 + pz;
    dest->x = az * b->m31 + px;
    dest->y = az * b->m32 + py;
    dest->z = az * b->m33 + pz;
}

/* InvMtxRotate v1: explicit temps */
void NuVecInvMtxRotate_v1(struct nuvec_s* dest, struct nuvec_s* a, struct Mtx* b)
{
    f32 ax = a->x, ay = a->y, az = a->z;
    dest->x = ax * b->m11 + ay * b->m12 + az * b->m13;
    dest->y = ax * b->m21 + ay * b->m22 + az * b->m23;
    dest->z = ax * b->m31 + ay * b->m32 + az * b->m33;
}
