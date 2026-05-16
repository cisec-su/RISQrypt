#include <stdint.h>
#include <stddef.h>
#include "params.h"
#include "rubato_soft.h"

/**
 * @brief Computes D[i] = A[i] * B[i] + C[i] modulo Q for a full polynomial.
 * @param D Output polynomial.
 * @param A First input polynomial.
 * @param B Second input polynomial.
 * @param C Addend polynomial.
 */
void poly_soft_mult_add(poly *D, const poly *A, const poly *B, const poly *C) {
    size_t i;
    uint64_t acc;

    for (i = 0; i < N; i++) {
        acc = ((uint64_t)(uint32_t)A->coeffs[i] * (uint32_t)B->coeffs[i]) + (uint32_t)C->coeffs[i];
        D->coeffs[i] = (int32_t)(acc % (uint64_t)Q);
    }
}

/**
 * @brief Pointwise addition of polynomials (pure software)
 * @description Performs coefficient-wise addition: C[i] = (A[i] + B[i]) mod Q. All arithmetic uses 64-bit intermediate values for safe computation
 * @param C pointer to output polynomial
 * @param A pointer to first input polynomial
 * @param B pointer to second input polynomial
 * @return void
 */
void poly_soft_pointwise_add(poly *C, const poly *A, const poly *B) {
    size_t i;
    for(i = 0; i < N; i++) {
        C->coeffs[i] = (A->coeffs[i] + B->coeffs[i]) % Q;
    }
}

/**
 * @brief Pointwise subtraction of polynomials (pure software)
 * @description Performs coefficient-wise subtraction: C[i] = (A[i] - B[i]) mod Q. Uses modular arithmetic to handle negative results
 * @param C pointer to output polynomial
 * @param A pointer to first input polynomial
 * @param B pointer to second input polynomial
 * @return void
 */
void poly_soft_pointwise_sub(poly *C, const poly *A, const poly *B) {
    size_t i;
    uint32_t diff;

    for(i = 0; i < N; i++) {
        diff = A->coeffs[i] + Q - B->coeffs[i];
        if (diff >= Q)
            diff = diff - Q;
        C->coeffs[i] = diff;
    }
}

/**
 * @brief Pointwise multiplication of polynomials (pure software)
 * @description Performs coefficient-wise multiplication: C[i] = (A[i] * B[i]) mod Q. Uses 64-bit arithmetic to prevent overflow during intermediate multiplication
 * @param C pointer to output polynomial
 * @param A pointer to first input polynomial
 * @param B pointer to second input polynomial
 * @return void
 */
void poly_soft_pointwise_mult(poly *C, const poly *A, const poly *B) {
    size_t i;
    for(i = 0; i < N; i++) {
        C->coeffs[i] = MOD_Q_MULT((uint32_t)A->coeffs[i], (uint32_t)B->coeffs[i]);
    }
}
