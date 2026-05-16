#ifndef PARAMS_H
#define PARAMS_H

#include <stdint.h>

/* Select one parameter set: PARAM_80_S, PARAM_80_L,
 *                           PARAM_128_S, PARAM_128_L */
#define PARAM_80_S
/* Select XOF: XOF_SHAKE128, XOF_SHAKE256 */
#define XOF_SHAKE128
/* ---------- per-parameter constants ---------- */
#if defined(PARAM_80_S)
#define RUBATO_BLOCKSIZE 16
#define V 4
#define RUBATO_OUTPUTSIZE 12
#define RUBATO_R 2
#define Q 0x3EE0001
#define LOGQ 26
#define MODULUS_BIT_MASK 0x3FFFFFF
#define LINEAR_COEFFS {2, 3, 1, 1}
#define LINEAR_COEFFS_COUNT 4
#define RUBATO_STATE_INIT {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16}

#elif defined(PARAM_80_L)
#define RUBATO_BLOCKSIZE 64
#define V 8
#define RUBATO_OUTPUTSIZE 60
#define RUBATO_R 2
#define Q 0x1FC0001
#define LOGQ 25
#define MODULUS_BIT_MASK 0x1FFFFFF
#define LINEAR_COEFFS {5, 3, 4, 3, 6, 2, 1, 1}
#define LINEAR_COEFFS_COUNT 8
#define RUBATO_STATE_INIT {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64}

#elif defined(PARAM_128_S)
#define RUBATO_BLOCKSIZE 16
#define V 4
#define RUBATO_OUTPUTSIZE 12
#define RUBATO_R 5
#define Q 0x3EE0001
#define LOGQ 26
#define MODULUS_BIT_MASK 0x3FFFFFF
#define LINEAR_COEFFS {2, 3, 1, 1}
#define LINEAR_COEFFS_COUNT 4
#define RUBATO_STATE_INIT {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16}

#elif defined(PARAM_128_L)
#define RUBATO_BLOCKSIZE 64
#define V 8
#define RUBATO_OUTPUTSIZE 60
#define RUBATO_R 2
#define Q 0x1FC0001
#define LOGQ 25
#define MODULUS_BIT_MASK 0x1FFFFFF
#define LINEAR_COEFFS {5, 3, 4, 3, 6, 2, 1, 1}
#define LINEAR_COEFFS_COUNT 8
#define RUBATO_STATE_INIT {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64}

#else
#error "No Rubato parameter set defined."
#endif

/* N is the block (state) size */
#define N RUBATO_BLOCKSIZE

/* XOF rate in bytes (used for coefficient generation) */
#if defined(XOF_SHAKE128)
#define XOF_RATE_BYTES 168
#elif defined(XOF_SHAKE256)
#define XOF_RATE_BYTES 136
#else
#error "No XOF defined."
#endif

/* Number of XOF coefficients needed per encryption: (R+1)*N */
#define RUBATO_XOF_ELEM_COUNT ((RUBATO_R + 1) * N)

/* Two-share masking */
#define MASKING_N 2

#define RUBATO_NAMESPACE(s) rubato_##s

#endif
