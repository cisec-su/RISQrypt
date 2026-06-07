#include <stdint.h>
#include <stddef.h>
#include "params.h"
#include "poly.h"
#include "symmetric.h"
#include "ntt_lite.h"
#include "util.h"

#define INV2 0x8001 

void poly_init_q(void) {
    const uint32_t q = (uint32_t)Q;
    /* mu = floor(2^64 / Q), split as mu[0]=low32 and mu[1]=high32 for ntt_lite_load_q */
    const uint32_t mu[2] = {
#if (Q == 0x3EE0001)
        0x25271FC5u, 0x00000041u
#elif (Q == 0x1FC0001)
        0x0203C70Du, 0x00000081u
#else
#error "Unsupported Q for HW mu constants"
#endif
    };
    const uint32_t inv2 = (q + 1u) >> 1;
    uint32_t logn;

#if defined(PARAM_80_S) || defined(PARAM_128_S)
    logn = 4u;
#elif defined(PARAM_80_L) || defined(PARAM_128_L)
    logn = 6u;
#else
#error "Unsupported RUBATO parameter set"
#endif

    ntt_lite_load_q(q, mu, logn, LOGQ, inv2, NTT_LITE_MODE_SINGLE);
    ntt_lite_set_bound(0);
}


/**
 * @brief Add two polynomials with no modular reduction
 * @description Performs element-wise addition of polynomial coefficients using the hardware accelerator
 * @param c pointer to output polynomial
 * @param a pointer to first summand polynomial
 * @param b pointer to second summand polynomial
 * @return void
 */
void poly_add(poly *c, const poly *a, const poly *b) {
    ntt_lite_add((uint32_t*)c->coeffs, (uint32_t*)a->coeffs, (uint32_t*)b->coeffs);
}


/**
 * @brief Subtract two polynomials with no modular reduction
 * @description Performs element-wise subtraction (c = a - b) using the hardware accelerator
 * @param c pointer to output polynomial
 * @param a pointer to minuend polynomial
 * @param b pointer to subtrahend polynomial
 * @return void
 */
void poly_sub(poly *c, const poly *a, const poly *b) {
    ntt_lite_sub((uint32_t*) c->coeffs, (uint32_t*) a->coeffs, (uint32_t*) b->coeffs);
}

/**
 * @brief Pointwise multiplication of polynomials in NTT domain
 * @description Performs coefficient-wise multiplication using the hardware accelerator. Result is automatically multiplied by 2^{-32} for NTT inverse scaling
 * @param c pointer to output polynomial
 * @param a pointer to first input polynomial
 * @param b pointer to second input polynomial
 * @return void
 */
void poly_pointwise(poly *c, const poly *a, const poly *b) {
    ntt_lite_pwm((uint32_t*)c->coeffs, (uint32_t*)a->coeffs, (uint32_t*)b->coeffs);
}

/**
 * @brief Computes D[i] = A[i] * B[i] + C[i] modulo Q for a raw coefficient span.
 * @param D Output polynomial.
 * @param A First input coefficient pointer.
 * @param B Second input coefficient pointer.
 * @param C Addend polynomial.
 * @param count Number of coefficients to process.
 */
void poly_mult_add(poly *D, const poly *A, const poly *B, const poly *C) {
    ntt_lite_pwm(NTT_LITE_OUTPUT_DIS, A->coeffs, B->coeffs); // tmp = A * B
    ntt_lite_add(D->coeffs, NTT_LITE_INPUT_DIS, C->coeffs);
}

#define POLY_UNIFORM_BLOCKS 2
#define POLY_UNIFORM_WORDS  ((POLY_UNIFORM_BLOCKS * RUBATO_STREAM_BLOCKBYTES) / 4)

/**
 * @brief Sample one polynomial uniformly mod Q from a fresh XOF stream.
 *
 * Initializes the stream from (nonce, block_ctr, poly_ctr) so distinct
 * poly_ctr values yield independent polynomials. Configures NTT-lite for
 * rejection sampling (bound=Q, inv2=POLY_UNIFORM_WORDS), consumes
 * POLY_UNIFORM_BLOCKS stream blocks, and restores bound=0 on exit so
 * subsequent non-rejsamp HW ops see the expected state.
 */
void poly_uniform(poly *p, uint64_t nonce, uint64_t block_ctr, uint8_t poly_ctr) {
    uint32_t buf[POLY_UNIFORM_WORDS];

    rubato_stream_init(nonce, block_ctr, poly_ctr);

    ntt_lite_set_bound(Q);
    ntt_lite_set_inv2(POLY_UNIFORM_WORDS);

    rubato_stream_squeezeblocks((uint8_t *)buf, POLY_UNIFORM_BLOCKS);
    ntt_lite_rejsamp((uint32_t *)p->coeffs, buf, LOGQ, NTT_LITE_REJSAMP_CENTER_DIS);

    ntt_lite_set_bound(0);

    // for (size_t i = 0; i < N; i++)
    // {
    //     /* code */
    //     p->coeffs[i] = 1;
    // }
    

}
