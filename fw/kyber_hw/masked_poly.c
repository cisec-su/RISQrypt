#include <stdint.h>
#include "ntt_lite.h"
#include "masked_poly.h"


void A2B_dummy(masked_poly *r, masked_poly *a, uint32_t p) {
    unsigned int i,j;
    uint16_t t, k;

    for(j = 0; j < KYBER_N; j++) {
        t = 0;
        k = 0;
        for(i = 0; i < MASKING_N - 1; i++) {
            r->share[i].coeffs[j] = a->share[i].coeffs[j];
            t = t + a->share[i].coeffs[j];
            k = k ^ a->share[i].coeffs[j];
            t = t & p;
        }
        t = t + a->share[MASKING_N - 1].coeffs[j];
        t = t & p;
        r->share[MASKING_N - 1].coeffs[j] = k ^ t;        
    }

}



/* https://eprint.iacr.org/2022/158: Algorithm 15
 * Distinctively, we use mod c+alpha+1 during decompress.
 */
void masked_poly_tomsg(masked_msg msg, masked_poly *a)
{

#if (MASKING_N != 2)
#error "masked_poly_tomsg supports only MASKING_N == 2"
#endif

    unsigned int i;
    const uint32_t q = KYBER_Q << 1;
    const uint32_t mu = 0x275f;
    const uint32_t inv2 = 0x681;
    const uint32_t alpha = 12 + LOG_MASKING_N;
    const uint32_t alpha_dual = ((1 << (alpha - 1)) << 16) | (1 << (alpha - 1));
    const uint32_t d_ = alpha + 2; // alpha + d + 1
    uint32_t t[KYBER_N >> 1];

    ntt_lite_load_q(q, &mu, 7, 13, inv2, NTT_LITE_MODE_POLY);

    for (i = 0; i < MASKING_N - 1; i++) {
        ntt_lite_compress((uint32_t*) a->share[i].coeffs, (uint32_t*) a->share[i].coeffs, d_);
    }
    ntt_lite_compress(NTT_LITE_OUTPUT_DIS, (uint32_t*) a->share[i].coeffs, d_);

    for (i = 0; i < (KYBER_N >> 1); i++) {
        t[i] = alpha_dual;
    }
    ntt_lite_load_q((1 << d_), &mu, 7, 13, inv2, NTT_LITE_MODE_POLY);
    ntt_lite_add((uint32_t*) a->share[MASKING_N-1].coeffs, NTT_LITE_INPUT_DIS, (uint32_t*) t);

    A2B_dummy(a, a, (1 << d_) - 1);

    ntt_lite_load_q(1, &mu, 7, 1, inv2, NTT_LITE_MODE_POLY);

    for (i = 0; i < MASKING_N; i++) {
        ntt_lite_decompress_floor(NTT_LITE_OUTPUT_DIS, (uint32_t*) a->share[i].coeffs, alpha);
        ntt_lite_encode((uint32_t*) msg[i], NTT_LITE_INPUT_DIS, 1);
    }
}