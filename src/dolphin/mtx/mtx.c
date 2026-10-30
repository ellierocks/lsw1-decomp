#include <dolphin.h>
#include <dolphin/mtx.h>
#include "fake_tgmath.h"

static f32 Unit01[2] = {
    0.0f,
    1.0f
};

// Keep the stripped C helper to emit 1.0f before 0.0f in the constant pool.
void C_MTXIdentity(Mtx m) {
    ASSERTMSGLINE(189, m, "MtxIdentity():  NULL Mtx 'm' ");
    m[0][0] = 1;
    m[0][1] = 0;
    m[0][2] = 0;
    m[0][3] = 0;
    m[1][0] = 0;
    m[1][1] = 1;
    m[1][2] = 0;
    m[1][3] = 0;
    m[2][0] = 0;
    m[2][1] = 0;
    m[2][2] = 1;
    m[2][3] = 0;
}

void PSMTXIdentity(register Mtx m) {
    register f32 c_zero = 0.0f;
    register f32 c_one = 1.0f;
    register f32 c_01;
    register f32 c_10;

    asm {
        psq_st c_zero, 8(m), 0, 0
        ps_merge01 c_01, c_zero, c_one
        psq_st c_zero, 24(m), 0, 0
        ps_merge10 c_10, c_one, c_zero
        psq_st c_zero, 32(m), 0, 0
        psq_st c_01, 16(m), 0, 0
        psq_st c_10, 0(m), 0, 0
        psq_st c_10, 40(m), 0, 0
    }
}

asm void PSMTXConcat(const register Mtx a, const register Mtx b, register Mtx ab) {
    nofralloc
    stwu r1, -64(r1)
    psq_l f0, 0(a), 0, 0
    stfd f14, 8(r1)
    psq_l f6, 0(b), 0, 0
    lis r6, Unit01@ha
    psq_l f7, 8(b), 0, 0
    stfd f15, 16(r1)
    addi r6, r6, Unit01@l
    stfd f31, 40(r1)
    psq_l f8, 16(b), 0, 0
    ps_muls0 f12, f6, f0
    psq_l f2, 16(a), 0, 0
    ps_muls0 f13, f7, f0
    psq_l f31, 0(r6), 0, 0
    ps_muls0 f14, f6, f2
    psq_l f9, 24(b), 0, 0
    ps_muls0 f15, f7, f2
    psq_l f1, 8(a), 0, 0
    ps_madds1 f12, f8, f0, f12
    psq_l f3, 24(a), 0, 0
    ps_madds1 f14, f8, f2, f14
    psq_l f10, 32(b), 0, 0
    ps_madds1 f13, f9, f0, f13
    psq_l f11, 40(b), 0, 0
    ps_madds1 f15, f9, f2, f15
    psq_l f4, 32(a), 0, 0
    psq_l f5, 40(a), 0, 0
    ps_madds0 f12, f10, f1, f12
    ps_madds0 f13, f11, f1, f13
    ps_madds0 f14, f10, f3, f14
    ps_madds0 f15, f11, f3, f15
    psq_st f12, 0(ab), 0, 0
    ps_muls0 f2, f6, f4
    ps_madds1 f13, f31, f1, f13
    ps_muls0 f0, f7, f4
    psq_st f14, 16(ab), 0, 0
    ps_madds1 f15, f31, f3, f15
    psq_st f13, 8(ab), 0, 0
    ps_madds1 f2, f8, f4, f2
    ps_madds1 f0, f9, f4, f0
    ps_madds0 f2, f10, f5, f2
    lfd f14, 8(r1)
    psq_st f15, 24(ab), 0, 0
    ps_madds0 f0, f11, f5, f0
    psq_st f2, 32(ab), 0, 0
    ps_madds1 f0, f31, f5, f0
    lfd f15, 16(r1)
    psq_st f0, 40(ab), 0, 0
    lfd f31, 40(r1)
    addi r1, r1, 64
    blr
}

#if DEBUG
#pragma push
#pragma optimization_level 1
// This function will not compile at optimization level 0
#endif

#if DEBUG
#pragma pop
#endif

void PSMTXTranspose(const register Mtx src, register Mtx xPose) {
    register f32 c_zero = 0;
    register f32 row0a;
    register f32 row1a;
    register f32 row0b;
    register f32 row1b;
    register f32 trns0;
    register f32 trns1;
    register f32 trns2;

    asm {
        psq_l row0a, 0(src), 0, 0
    }
    xPose[2][3] = c_zero;
    asm {
        psq_l row1a, 16(src), 0, 0
        ps_merge00 trns0, row0a, row1a
        psq_l row0b, 8(src), 1, 0
        ps_merge11 trns1, row0a, row1a
        psq_l row1b, 24(src), 1, 0
        psq_st trns0, 0(xPose), 0, 0
        psq_l row0a, 32(src), 0, 0
        ps_merge00 trns2, row0b, row1b
        psq_st trns1, 16(xPose), 0, 0
        ps_merge00 trns0, row0a, c_zero
        psq_st trns2, 32(xPose), 0, 0
        ps_merge10 trns1, row0a, c_zero
        psq_st trns0, 8(xPose), 0, 0
    }
    row0b = src[2][2];
    asm {
        psq_st trns1, 24(xPose), 0, 0
    }
    xPose[2][2] = row0b;
}

asm u32 PSMTXInverse(const register Mtx src, register Mtx inv) {
    psq_l f0, 0(src), 1, 0
    psq_l f1, 4(src), 0, 0
    psq_l f2, 16(src), 1, 0
    ps_merge10 f6, f1, f0
    psq_l f3, 20(src), 0, 0
    psq_l f4, 32(src), 1, 0
    ps_merge10 f7, f3, f2
    psq_l f5, 36(src), 0, 0
    ps_mul f11, f3, f6
    ps_mul f13, f5, f7
    ps_merge10 f8, f5, f4
    ps_msub f11, f1, f7, f11
    ps_mul f12, f1, f8
    ps_msub f13, f3, f8, f13
    ps_mul f10, f3, f4
    ps_msub f12, f5, f6, f12
    ps_mul f9, f0, f5
    ps_mul f8, f1, f2
    ps_sub f6, f6, f6
    ps_msub f10, f2, f5, f10
    ps_mul f7, f0, f13
    ps_msub f9, f1, f4, f9
    ps_madd f7, f2, f12, f7
    ps_msub f8, f0, f3, f8
    ps_madd f7, f4, f11, f7
    ps_cmpo0 cr0, f7, f6
    bne skip_return
    li r3, 0
    blr
skip_return:
    fres f0, f7
    ps_add f6, f0, f0
    ps_mul f5, f0, f0
    ps_nmsub f0, f7, f5, f6
    lfs f1, 12(src)
    ps_muls0 f13, f13, f0
    lfs f2, 28(src)
    ps_muls0 f12, f12, f0
    lfs f3, 44(src)
    ps_muls0 f11, f11, f0
    ps_merge00 f5, f13, f12
    ps_muls0 f10, f10, f0
    ps_merge11 f4, f13, f12
    ps_muls0 f9, f9, f0
    psq_st f5, 0(inv), 0, 0
    ps_mul f6, f13, f1
    psq_st f4, 16(inv), 0, 0
    ps_muls0 f8, f8, f0
    ps_madd f6, f12, f2, f6
    psq_st f10, 32(inv), 1, 0
    ps_nmadd f6, f11, f3, f6
    psq_st f9, 36(inv), 1, 0
    ps_mul f7, f10, f1
    ps_merge00 f5, f11, f6
    psq_st f8, 40(inv), 1, 0
    ps_merge11 f4, f11, f6
    psq_st f5, 8(inv), 0, 0
    ps_madd f7, f9, f2, f7
    psq_st f4, 24(inv), 0, 0
    ps_nmadd f7, f8, f3, f7
    li r3, 1
    psq_st f7, 44(inv), 1, 0
}

void PSMTXTrans(register Mtx m, register f32 xT, register f32 yT, register f32 zT) {
    register f32 c0 = 0.0f;
    register f32 c1 = 1.0f;

	asm {
		stfs xT, 12(m)
		stfs yT, 28(m)
		psq_st c0, 4(m), 0, 0
		psq_st c0, 32(m), 0, 0
		stfs c0, 16(m)
		stfs c1, 20(m)
		stfs c0, 24(m)
		stfs c1, 40(m)
		stfs zT, 44(m)
		stfs c1, 0(m)
	}
}

void PSMTXScale(register Mtx m, register f32 xS, register f32 yS, register f32 zS) {
    register f32 c0 = 0.0f;

	asm {
		stfs xS, 0(m)
		psq_st c0, 4(m), 0, 0
		psq_st c0, 12(m), 0, 0
		stfs yS, 20(m)
		psq_st c0, 24(m), 0, 0
		psq_st c0, 32(m), 0, 0
		stfs zS, 40(m)
		stfs c0, 44(m)
	}
}
