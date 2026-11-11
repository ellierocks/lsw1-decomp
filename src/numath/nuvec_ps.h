#ifndef NUVEC_PS_H
#define NUVEC_PS_H

/* Gekko paired-single vector add/sub as GCC 2.95 inline asm, the form retail code has
 * inlined throughout the game (chris, podrace) and as the bodies of NuVecAdd/NuVecSub.
 * Three floats are moved as one psq pair (x,y) plus one single (z) through f1-f4:
 * dst = a +/- b. Operands are base registers; the numeric register syntax is what the
 * build's binutils (-mgekko) assembles. */
#define NuVecAddPS(dst, a, b) \
    asm volatile("psq_l 1, 0(%1), 0, 0\n\tpsq_l 3, 0(%2), 0, 0\n\tpsq_l 2, 8(%1), 1, 0\n\t" \
                 "psq_l 4, 8(%2), 1, 0\n\tps_add 1, 1, 3\n\tps_add 2, 2, 4\n\t" \
                 "psq_st 1, 0(%0), 0, 0\n\tpsq_st 2, 8(%0), 1, 0" \
                 : : "b"(dst), "b"(a), "b"(b) : "fr1", "fr2", "fr3", "fr4", "memory")

#define NuVecSubPS(dst, a, b) \
    asm volatile("psq_l 1, 0(%1), 0, 0\n\tpsq_l 3, 0(%2), 0, 0\n\tpsq_l 2, 8(%1), 1, 0\n\t" \
                 "psq_l 4, 8(%2), 1, 0\n\tps_sub 1, 1, 3\n\tps_sub 2, 2, 4\n\t" \
                 "psq_st 1, 0(%0), 0, 0\n\tpsq_st 2, 8(%0), 1, 0" \
                 : : "b"(dst), "b"(a), "b"(b) : "fr1", "fr2", "fr3", "fr4", "memory")

/* dst = src * mtx (row vectors, translation in row 3), the paired-single body of
 * NuVecMtxTransform: x,y from rows 0-3 as pairs, z through ps_sum0. Uses f0-f8. */
#define NuVecMtxTransformPS(dst, src, mtx) \
    asm volatile("psq_l 0, 0(%1), 0, 0\n\tpsq_l 2, 0(%2), 0, 0\n\tpsq_l 1, 8(%1), 1, 0\n\t" \
                 "psq_l 6, 8(%2), 1, 0\n\tpsq_l 7, 24(%2), 1, 0\n\tps_muls0 3, 2, 0\n\t" \
                 "psq_l 2, 16(%2), 0, 0\n\tps_muls0 8, 6, 0\n\tpsq_l 4, 32(%2), 0, 0\n\t" \
                 "ps_madds1 8, 7, 0, 8\n\tpsq_l 6, 40(%2), 1, 0\n\tps_madds1 3, 2, 0, 3\n\t" \
                 "ps_madds0 8, 6, 1, 8\n\tpsq_l 7, 52(%2), 0, 0\n\tpsq_l 5, 48(%2), 0, 0\n\t" \
                 "ps_madds0 3, 4, 1, 3\n\tps_sum0 8, 8, 1, 7\n\tps_add 3, 5, 3\n\t" \
                 "psq_st 8, 8(%0), 1, 0\n\tpsq_st 3, 0(%0), 0, 0" \
                 : : "b"(dst), "b"(src), "b"(mtx) \
                 : "fr0", "fr1", "fr2", "fr3", "fr4", "fr5", "fr6", "fr7", "fr8", "memory")

#endif
