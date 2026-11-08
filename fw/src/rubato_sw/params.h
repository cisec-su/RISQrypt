#ifndef PARAMS_H
#define PARAMS_H

#include <stdint.h>

/* Select one parameter set: PARAM_80_S, PARAM_80_M, PARAM_80_L,
 *                           PARAM_128_S, PARAM_128_M, PARAM_128_L */
#define PARAM_80_S
/* Select XOF: XOF_SHAKE128, XOF_SHAKE256 */
#define XOF_SHAKE256
/* ---------- per-parameter constants ----------
 * NOISE_CDT: discrete Gaussian CDF table (31-bit) from the Rubato reference
 * (parms.h CDF_TABLE_*), one copy per entry instead of the 8x AVX2 broadcast.
 * Entries equal to 2^31-1 are dropped: they can never be below a 31-bit sample,
 * so they never contribute to the count (this also removes the non-monotonic
 * entry of CDF_TABLE_128_ML). */
#if defined(PARAM_80_S)
#  define RUBATO_BLOCKSIZE   16
#  define V                  4
#  define RUBATO_OUTPUTSIZE  12
#  define RUBATO_R           2
#  define Q                  0x3EE0001
#  define MODULUS_BIT_MASK   0x3FFFFFF
#  define LINEAR_COEFFS      {2, 3, 1, 1}
#  define LINEAR_COEFFS_COUNT 4
#  define NOISE_CDT_LEN      28
#  define NOISE_CDT          {193466994, 570659710, 920075514, 1227667221, 1484978918, 1689527875, 1844048991, \
                              1954974452, 2030645103, 2079699341, 2109918225, 2127608442, 2137449475, 2142651843, \
                              2145265294, 2146512908, 2147078886, 2147322875, 2147422828, 2147461739, 2147476134, \
                              2147481194, 2147482885, 2147483421, 2147483583, 2147483630, 2147483642, 2147483646}

#elif defined(PARAM_80_M)
#  define RUBATO_BLOCKSIZE   36
#  define V                  6
#  define RUBATO_OUTPUTSIZE  32
#  define RUBATO_R           2
#  define Q                  0x1FC0001
#  define MODULUS_BIT_MASK   0x1FFFFFF
#  define LINEAR_COEFFS      {4, 2, 4, 3, 1, 1}
#  define LINEAR_COEFFS_COUNT 6
#  define NOISE_CDT_LEN      7
#  define NOISE_CDT          {795364312, 1829169833, 2112940235, 2145839095, 2147450037, 2147483354, 2147483645}

#elif defined(PARAM_80_L)
#  define RUBATO_BLOCKSIZE   64
#  define V                  8
#  define RUBATO_OUTPUTSIZE  60
#  define RUBATO_R           2
#  define Q                  0x1FC0001
#  define MODULUS_BIT_MASK   0x1FFFFFF
#  define LINEAR_COEFFS      {5, 3, 4, 3, 6, 2, 1, 1}
#  define LINEAR_COEFFS_COUNT 8
#  define NOISE_CDT_LEN      4
#  define NOISE_CDT          {1341314782, 2127638171, 2147440791, 2147483639}

#elif defined(PARAM_128_S)
#  define RUBATO_BLOCKSIZE   16
#  define V                  4
#  define RUBATO_OUTPUTSIZE  12
#  define RUBATO_R           5
#  define Q                  0x3EE0001
#  define MODULUS_BIT_MASK   0x3FFFFFF
#  define LINEAR_COEFFS      {2, 3, 1, 1}
#  define LINEAR_COEFFS_COUNT 4
#  define NOISE_CDT_LEN      27
#  define NOISE_CDT          {204522251, 602075463, 967055725, 1283569708, 1542847805, 1743474246, 1890116766, \
                              1991363511, 2057395083, 2098074209, 2121746538, 2134759009, 2141515606, 2144829553, \
                              2146364921, 2147036859, 2147314635, 2147423105, 2147463116, 2147477056, 2147481645, \
                              2147483071, 2147483490, 2147483606, 2147483637, 2147483644, 2147483646}

#elif defined(PARAM_128_M)
#  define RUBATO_BLOCKSIZE   36
#  define V                  6
#  define RUBATO_OUTPUTSIZE  32
#  define RUBATO_R           3
#  define Q                  0x1FC0001
#  define MODULUS_BIT_MASK   0x1FFFFFF
#  define LINEAR_COEFFS      {4, 2, 4, 3, 1, 1}
#  define LINEAR_COEFFS_COUNT 6
#  define NOISE_CDT_LEN      9
#  define NOISE_CDT          {523776498, 1392759815, 1888801005, 2083648537, 2146112362, 2147366230, 2147476666, \
                              2147483359, 2147483638}

#elif defined(PARAM_128_L)
#  define RUBATO_BLOCKSIZE   64
#  define V                  8
#  define RUBATO_OUTPUTSIZE  60
#  define RUBATO_R           2
#  define Q                  0x1FC0001
#  define MODULUS_BIT_MASK   0x1FFFFFF
#  define LINEAR_COEFFS      {5, 3, 4, 3, 6, 2, 1, 1}
#  define LINEAR_COEFFS_COUNT 8
#  define NOISE_CDT_LEN      9
#  define NOISE_CDT          {523776498, 1392759815, 1888801005, 2083648537, 2146112362, 2147366230, 2147476666, \
                              2147483359, 2147483638}

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

/* XOF blocks needed for one 32-bit word per noise coefficient */
#define NOISE_CDT_BLOCKS ((4 * N + XOF_RATE_BYTES - 1) / XOF_RATE_BYTES)

/* Two-share masking */
#define MASKING_N 2

typedef struct {
    int32_t coeffs[N];
} poly;

#endif
