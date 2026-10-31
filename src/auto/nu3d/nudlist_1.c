#include "numath/numath.h"

void fn_8000CB74(struct nuvec_s* dst, struct nuvec_s* src);
void fn_8000D874(struct numtx_s* mtx, struct nuvec_s* v);
void fn_8000D95C(struct numtx_s* mtx, struct nuvec_s* v);

f32 NuMtxDet3(struct numtx_s* m)
{
    return m->_00 * (m->_11 * m->_22 - m->_12 * m->_21)
         - m->_01 * (m->_10 * m->_22 - m->_12 * m->_20)
         + m->_02 * (m->_10 * m->_21 - m->_11 * m->_20);
}

void fn_8000E830(struct numtx_s* mtx, struct nuvec_s* p)
{
    struct nuvec_s v;
    v.x = p->x - mtx->_30;
    v.y = p->y - mtx->_31;
    v.z = p->z - mtx->_32;
    fn_8000CB74(&v, &v);
    fn_8000D874(mtx, &v);
}

void fn_8000E8A4(struct numtx_s* mtx, struct nuvec_s* p)
{
    struct nuvec_s v;
    v.x = p->x - mtx->_30;
    v.y = p->y - mtx->_31;
    v.z = p->z - mtx->_32;
    fn_8000CB74(&v, &v);
    fn_8000D95C(mtx, &v);
}
