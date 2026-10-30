#include <dolphin.h>
#include <dolphin/mtx.h>
#include "fake_tgmath.h"

asm void PSMTXMultVec(const register Mtx m, const register Vec* src, register Vec* dst) {
    nofralloc
    psq_l f0, Vec.x(src), 0, 0
    psq_l f2, 0(m), 0, 0
    psq_l f1, Vec.z(src), 1, 0
    ps_mul f4, f2, f0
    psq_l f3, 8(m), 0, 0
    ps_madd f5, f3, f1, f4
    psq_l f8, 16(m), 0, 0
    ps_sum0 f6, f5, f6, f5
    psq_l f9, 24(m), 0, 0
    ps_mul f10, f8, f0
    psq_st f6, Vec.x(dst), 1, 0
    ps_madd f11, f9, f1, f10
    psq_l f2, 32(m), 0, 0
    ps_sum0 f12, f11, f12, f11
    psq_l f3, 40(m), 0, 0
    ps_mul f4, f2, f0
    psq_st f12, Vec.y(dst), 1, 0
    ps_madd f5, f3, f1, f4
    ps_sum0 f6, f5, f6, f5
    psq_st f6, Vec.z(dst), 1, 0
    blr
}

asm void PSMTXMultVecArray(const register Mtx m, const register Vec* srcBase, register Vec* dstBase, register u32 count) {
    nofralloc
    psq_l f13, 0x0(m), 0, 0
    psq_l f12, 0x10(m), 0, 0
    subi count, count, 0x1
    psq_l f11, 0x8(m), 0, 0
    ps_merge00 f0, f13, f12
    subi dstBase, dstBase, 0x4
    psq_l f10, 0x18(m), 0, 0
    ps_merge11 f1, f13, f12
    mtctr count
    psq_l f4, 0x20(m), 0, 0
    ps_merge00 f2, f11, f10
    psq_l f5, 0x28(m), 0, 0
    ps_merge11 f3, f11, f10
    psq_l f6, 0x0(srcBase), 0, 0
    psq_lu f7, 0x8(srcBase), 1, 0
    ps_madds0 f8, f0, f6, f3
    ps_mul f9, f4, f6
    ps_madds1 f8, f1, f6, f8
    ps_madd f10, f5, f7, f9
L_000003C4:
    psq_lu f6, 0x4(srcBase), 0, 0
    ps_madds0 f12, f2, f7, f8
    psq_lu f7, 0x8(srcBase), 1, 0
    ps_sum0 f13, f10, f9, f10
    ps_madds0 f8, f0, f6, f3
    ps_mul f9, f4, f6
    psq_stu f12, 0x4(dstBase), 0, 0
    ps_madds1 f8, f1, f6, f8
    psq_stu f13, 0x8(dstBase), 1, 0
    ps_madd f10, f5, f7, f9
    bdnz L_000003C4
    ps_madds0 f12, f2, f7, f8
    ps_sum0 f13, f10, f9, f10
    psq_stu f12, 0x4(dstBase), 0, 0
    psq_stu f13, 0x8(dstBase), 1, 0
    blr
}
