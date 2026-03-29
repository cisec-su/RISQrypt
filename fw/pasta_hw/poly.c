#include <stdint.h>
#include <stddef.h>
#include "params.h"
#include "poly.h"
#include "symmetric.h"
#include "ntt_lite.h"
#include "util.h"

#define INV2 0x8001 

/**
 * @brief Initialize polynomial modular arithmetic with modulus Q
 * @description Sets up hardware accelerator (ntt_lite) with modulus Q, multiplication factor mu, and inverse of 2
 * @return void
 */
void poly_init_q() {
    const uint32_t q = Q;
    const uint32_t mu[2] = {0x0000ffff, 0x0000ffff}; // mu = floor(2^{2*word_size} / q)
    const uint32_t inv2 = INV2;
    ntt_lite_load_q(q, mu, 7, 17, inv2, NTT_LITE_MODE_SINGLE); //pasta
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
 * @brief Generate polynomial with uniformly random coefficients via rejection sampling
 * @description Generates random polynomial coefficients in [0, Q-1] using SHAKE128 stream cipher. Two modes: if allow_zero=0 extracts 16-bit values and adds 1 (coefficients in [1, Q-1]); if allow_zero=1 uses 17-bit rejection sampling (coefficients in [0, Q-1]). Output can be sent to hardware accelerator (to_hw=1) or stored in polynomial array (to_hw=0)
 * @param a pointer to output polynomial
 * @param nonce 8-byte nonce for SHAKE128 seed
 * @param block_ctr block counter for SHAKE128 seed
 * @param poly_ctr polynomial counter for SHAKE128 seed
 * @param allow_zero if 0: no zero coefficients; if 1: allows zero coefficients
 * @param to_hw if 1: output to hardware register; if 0: output to polynomial
 * @return void
 */
void poly_uniform(poly *a, uint64_t nonce, uint64_t block_ctr, uint8_t poly_ctr, int allow_zero, int to_hw)
{
    poly b;
    size_t buflen = 4*STREAM128_BLOCKBYTES;
    uint32_t buf[(STREAM128_BLOCKBYTES>>2)*4]; // 316 -> 128

    if (allow_zero == 0) {
        stream128_init(nonce, block_ctr, poly_ctr);
        stream128_squeeze((uint8_t*) buf, (N >> 1) << 2);
        ntt_lite_decode(NTT_LITE_OUTPUT_DIS, buf, 16);
        ntt_lite_set_bound(1); // add +1
        ntt_lite_add_const(a->coeffs, NTT_LITE_INPUT_DIS);
    } else {

#ifdef REJ_SAMP_DIS
        stream128_init(nonce, block_ctr, poly_ctr);
        stream128_squeeze((uint8_t*) buf, (N >> 1) << 2);
        ntt_lite_decode(a->coeffs, buf, 16);
#else
        stream128_init(nonce, block_ctr, poly_ctr);
        stream128_squeezeblocks((uint8_t*) buf, 4);
        ntt_lite_set_inv2((STREAM128_BLOCKBYTES>>2)*4);
        ntt_lite_set_bound(Q);
        ntt_lite_rejsamp(a->coeffs, buf, 17, NTT_LITE_REJSAMP_CENTER_DIS);
#endif

    }
}

