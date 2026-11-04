/* Target configuration for src/sn/libgcc/libgcc2.c.
 *
 * Replaces the SN GCC build's tconfig.h/machmode.h/defaults.h chain with the
 * handful of values the DImode helpers use on PowerPC EABI (GameCube, big
 * endian, 8-bit units, 64-bit long double). Written for this project; it
 * contains no GCC source text. */
#ifndef SN_LIBGCC_CONFIG_H
#define SN_LIBGCC_CONFIG_H

#define BITS_PER_UNIT 8
#define WORDS_BIG_ENDIAN 1
#define LONG_DOUBLE_TYPE_SIZE 64
#define LIBGCC2_WORDS_BIG_ENDIAN WORDS_BIG_ENDIAN
#define LIBGCC2_LONG_DOUBLE_TYPE_SIZE LONG_DOUBLE_TYPE_SIZE

#endif
