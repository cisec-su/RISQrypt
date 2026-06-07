#ifndef PARAMS_H
#define PARAMS_H

#include <stdint.h>

/* Select one parameter set: PARAM_80_S, PARAM_80_M, PARAM_80_L,
 *                           PARAM_128_S, PARAM_128_M, PARAM_128_L */
#define PARAM_128_M
/* Select XOF: XOF_SHAKE128, XOF_SHAKE256 */
#define XOF_SHAKE128
/* ---------- per-parameter constants ---------- */
#if defined(PARAM_80_S)
#  define RUBATO_BLOCKSIZE   16
#  define V                  4
#  define RUBATO_OUTPUTSIZE  12
#  define RUBATO_R           2
#  define cbd_d              40
#  define Q                  0x3EE0001
#  define MODULUS_BIT_MASK   0x3FFFFFF
#  define LINEAR_COEFFS      {2, 3, 1, 1}
#  define LINEAR_COEFFS_COUNT 4

#elif defined(PARAM_80_M)
#  define RUBATO_BLOCKSIZE   36
#  define V                  6
#  define RUBATO_OUTPUTSIZE  32
#  define RUBATO_R           2
#  define cbd_d              3
#  define Q                  0x1FC0001
#  define MODULUS_BIT_MASK   0x1FFFFFF
#  define LINEAR_COEFFS      {4, 2, 4, 3, 1, 1}
#  define LINEAR_COEFFS_COUNT 6

#elif defined(PARAM_80_L)
#  define RUBATO_BLOCKSIZE   64
#  define V                  8
#  define RUBATO_OUTPUTSIZE  60
#  define RUBATO_R           2
#  define cbd_d              1
#  define Q                  0x1FC0001
#  define MODULUS_BIT_MASK   0x1FFFFFF
#  define LINEAR_COEFFS      {5, 3, 4, 3, 6, 2, 1, 1}
#  define LINEAR_COEFFS_COUNT 8

#elif defined(PARAM_128_S)
#  define RUBATO_BLOCKSIZE   16
#  define V                  4
#  define RUBATO_OUTPUTSIZE  12
#  define RUBATO_R           5
#  define cbd_d              36
#  define Q                  0x3EE0001
#  define MODULUS_BIT_MASK   0x3FFFFFF
#  define LINEAR_COEFFS      {2, 3, 1, 1}
#  define LINEAR_COEFFS_COUNT 4

#elif defined(PARAM_128_M)
#  define RUBATO_BLOCKSIZE   36
#  define V                  6
#  define RUBATO_OUTPUTSIZE  32
#  define RUBATO_R           3
#  define cbd_d              6
#  define Q                  0x1FC0001
#  define MODULUS_BIT_MASK   0x1FFFFFF
#  define LINEAR_COEFFS      {4, 2, 4, 3, 1, 1}
#  define LINEAR_COEFFS_COUNT 6

#elif defined(PARAM_128_L)
#  define RUBATO_BLOCKSIZE   64
#  define V                  8
#  define RUBATO_OUTPUTSIZE  60
#  define RUBATO_R           2
#  define cbd_d              6
#  define Q                  0x1FC0001
#  define MODULUS_BIT_MASK   0x1FFFFFF
#  define LINEAR_COEFFS      {5, 3, 4, 3, 6, 2, 1, 1}
#  define LINEAR_COEFFS_COUNT 8

#else
#  error "No Rubato parameter set defined."
#endif

/* N is the block (state) size */
#define N RUBATO_BLOCKSIZE

/* XOF rate in bytes (used for coefficient generation) */
#if defined(XOF_SHAKE128)
#  define XOF_RATE_BYTES 168
#elif defined(XOF_SHAKE256)
#  define XOF_RATE_BYTES 136
#else
#  error "No XOF defined."
#endif

/* Number of XOF coefficients needed per encryption: (R+1)*N */
#define RUBATO_XOF_ELEM_COUNT ((RUBATO_R + 1) * N)

/* Two-share masking */
#define MASKING_N 2

#define RUBATO_NAMESPACE(s) rubato_##s

typedef struct {
    int32_t coeffs[N];
} poly;

#endif
