#include <stdint.h>
#include <stddef.h>
#include "rubato_soft.h"
#include "masked_poly_soft.h"
#include "poly_soft.h"

/* RNG counter for masked polynomial operations */
static uint64_t g_masked_poly_rng_ctr = 1u;

/**
 * @brief Masked polynomial multiplication (software, two-share)
 * @description Computes C = A * B with two-share masking using random refreshing: c0 = a0*b0 - r; c1 = (a0*b1) + (a1*b0) + (a1*b1) + r. Where random r is generated from a PRNG for security refreshing
 * @param C pointer to output masked polynomial
 * @param A pointer to first input masked polynomial
 * @param B pointer to second input masked polynomial
 * @return void
 */
void masked_poly_soft_mult(masked_poly *C, const masked_poly *A, const masked_poly *B) {
    poly c0_temp, c1_temp, r_prime, r;

    // Generate random refreshing polynomial
    rubato_soft_poly_uniform(&r, 0x0123456789abcdef, g_masked_poly_rng_ctr++);

    // c0 = a0 * b0 - r
    poly_soft_pointwise_mult(&c0_temp, &A->share[0], &B->share[0]);
    poly_soft_pointwise_sub(&C->share[0], &c0_temp, &r);

    // r' = a0*b1 + r
    poly_soft_pointwise_mult(&r_prime, &A->share[0], &B->share[1]);
    poly_soft_pointwise_add(&r_prime, &r_prime, &r);

    // r' = (a0*b1 + r) + (b0*a1)
    poly_soft_pointwise_mult(&c1_temp, &A->share[1], &B->share[0]);
    poly_soft_pointwise_add(&r_prime, &r_prime, &c1_temp);

    // c1 = a1*b1 + r'
    poly_soft_pointwise_mult(&c1_temp, &A->share[1], &B->share[1]);
    poly_soft_pointwise_add(&C->share[1], &c1_temp, &r_prime);
}

/**
 * @brief Masked multiply-add with public coefficients.
 * @description Computes D = A*B + C (mod Q) share-wise where A is public and
 *              B, C, D are masked: D.share[j] = A*B.share[j] + C.share[j].
 *              Multiplying by a public value and adding are both linear, so the
 *              shares are processed independently and never combined.
 * @param D Output masked polynomial (may alias B or C).
 * @param A Public coefficient polynomial, coefficients in [0, Q).
 * @param B Masked multiplicand, shares in [0, Q).
 * @param C Masked addend, shares in [0, Q).
 * @return void
 */
void masked_poly_soft_mult_add(masked_poly *D, const poly *A, const masked_poly *B, const masked_poly *C) {
    size_t i;
    size_t j;
    uint64_t acc;

    for (j = 0; j < MASKING_N; j++) {
        for (i = 0; i < N; i++) {
            acc = ((uint64_t)(uint32_t)A->coeffs[i] * (uint32_t)B->share[j].coeffs[i])
                + (uint32_t)C->share[j].coeffs[i];
            D->share[j].coeffs[i] = (int32_t)(acc % (uint64_t)Q);
        }
    }
}

/**
 * @brief Masked polynomial addition (software).
 * @description Computes C = A + B (mod Q) share-wise. Addition is linear, so
 *              each share is added independently and shares are never combined.
 *              Uses poly_soft_pointwise_add per share. Inputs must be
 *              canonical [0, Q); the output is canonical too.
 * @param C Output masked polynomial (may alias A or B).
 * @param A First masked addend, shares in [0, Q).
 * @param B Second masked addend, shares in [0, Q).
 * @return void
 */
void masked_poly_soft_add(masked_poly *C, const masked_poly *A, const masked_poly *B) {
    size_t j;

    for (j = 0; j < MASKING_N; j++) {
        poly_soft_pointwise_add(&C->share[j], &A->share[j], &B->share[j]);
    }
}

/**
 * @brief Add an unmasked polynomial to a masked polynomial (software).
 * @description Computes r = a + b (mod Q) where a is unmasked and b is masked.
 *              An unmasked addend shifts the share sum, so it is added to the
 *              first share only: r.share[0] = a + b.share[0];
 *              r.share[j] = b.share[j] for j > 0.
 *              Uses poly_soft_pointwise_add on share 0. Inputs must be
 *              canonical [0, Q); the output is canonical too.
 *              Software counterpart of masked_poly_add_unmasked (rubato/masked_poly.c).
 *              Unlike the HW version, the remaining shares are copied from b when
 *              r and b are different objects, so r does not need to alias b.
 * @param r Output masked polynomial (may alias b).
 * @param a Unmasked addend, coefficients in [0, Q).
 * @param b Masked addend, shares in [0, Q).
 * @return void
 */
void masked_poly_soft_add_unmasked(masked_poly *r, const poly *a, const masked_poly *b) {
    size_t j;

    poly_soft_pointwise_add(&r->share[0], a, &b->share[0]);

    if (r != b) {
        for (j = 1; j < MASKING_N; j++) {
            r->share[j] = b->share[j];
        }
    }
}

/**
 * @brief Masked polynomial mask (software)
 * @description Creates two-share masking: r0 = random, r1 = a - r0
 * @param r pointer to output masked polynomial
 * @param a pointer to input unmasked polynomial
 * @return void
 */
void masked_poly_soft_mask(masked_poly *r, const poly *a) {
    size_t i, j;
    uint32_t sum;

    /* Generate MASKING_N-1 random shares and set the final share so that
       the sum of all shares == a (mod Q). */
    for (j = 0; j + 1 < MASKING_N; j++) {
        rubato_soft_poly_uniform(&r->share[j], 0x0123456789abcdef, g_masked_poly_rng_ctr++);
    }

    for (i = 0; i < N; i++) {
        sum = 0u;
        for (j = 0; j + 1 < MASKING_N; j++) {
            sum += (uint32_t)r->share[j].coeffs[i];
            if (sum >= Q) sum -= Q;
        }
        /* last share = a - sum (mod Q) */
        uint32_t last = (uint32_t)a->coeffs[i] + Q - sum;
        if (last >= Q) last -= Q;
        r->share[MASKING_N - 1].coeffs[i] = (int32_t)last;
    }
}

/**
 * @brief Masked polynomial unmask (software)
 * @description Recovers unmasked value: a = r0 + r1
 * @param a pointer to output unmasked polynomial
 * @param r pointer to input masked polynomial
 * @return void
 */
void masked_poly_soft_unmask(poly *a, const masked_poly *r) {
    size_t i, j;
    for (i = 0; i < N; i++) {
        uint32_t acc = 0u;
        for (j = 0; j < MASKING_N; j++) {
            acc += (uint32_t)r->share[j].coeffs[i];
            if (acc >= Q) acc -= Q;
        }
        a->coeffs[i] = (int32_t)acc;
    }
}
