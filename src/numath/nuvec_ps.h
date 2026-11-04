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

#endif
