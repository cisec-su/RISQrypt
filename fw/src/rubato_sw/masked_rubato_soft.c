#include <stdint.h>
#include <stddef.h>
#include "params.h"
#include "rubato_soft.h"
#include "masked_poly_soft.h"
#include "poly_soft.h"

/**
 * @brief Masked RUBATO S-box Feistel layer (software)
 * @description Computes B[0] = A[0], B[i] = A[i] + A[i-1]^2 using masked polynomial operations
 * @param B pointer to output masked polynomial
 * @param A pointer to input masked polynomial
 * @return void
 */
void masked_rubato_soft_sbox_feistel(masked_poly *B, const masked_poly *A) {
    // Feistel: B[0]=A[0], B[i]=A[i]+A[i-1]^2 mod Q
    size_t i;
    size_t j;
    masked_poly A_squared;

    // Compute A^2 for all elements
    masked_poly_soft_mult(&A_squared, A, A);

    // B[0] = A[0]
    for (j = 0; j < MASKING_N; j++)
        B->share[j].coeffs[0] = A->share[j].coeffs[0];

    // B[i] = A[i] + A[i-1]^2 for i >= 1
    for (i = 1; i < N; i++) {
        for (j = 0; j < MASKING_N; j++) {
            B->share[j].coeffs[i] = (A->share[j].coeffs[i] + A_squared.share[j].coeffs[i - 1]) % Q;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Masked RUBATO encrypt one block (software)
 * @description Generates one masked RUBATO keystream block in the same
 *              control flow as rubato_soft_encrypt.
 * @param ciphertext pointer to output polynomial
 * @param plaintext pointer to input plaintext coefficients
 * @param key pointer to key array (size 2*N coefficients)
 * @param nonce 8-byte nonce value
 * @param block_ctr block counter value
 * @return void
 */
void masked_rubato_soft_encrypt(poly *ciphertext, poly *plaintext, poly *key, uint64_t nonce, uint64_t block_ctr) {
    poly coeffs[RUBATO_R + 1];
    poly state;
    size_t r;
    size_t i;
    masked_poly m_state;
    masked_poly m_key;
    masked_poly m_keystream;
    poly keystream;
    size_t j;

    rubato_soft_sampling(coeffs, nonce, block_ctr);

    for (i = 0; i < N; i++) {
        state.coeffs[i] = (int32_t)(i + 1);
    }

    masked_poly_soft_mask(&m_state, &state);
    masked_poly_soft_mask(&m_key, key);

    for (r = 0; r < (size_t)RUBATO_R; r++) {
        masked_poly_soft_mult_add(&m_state, &coeffs[r], &m_key, &m_state);

        for (j = 0; j < MASKING_N; j++) {
            rubato_soft_linear_layer(&m_state.share[j], &m_state.share[j]);
        }

        masked_rubato_soft_sbox_feistel(&m_state, &m_state);
    }

    for (j = 0; j < MASKING_N; j++) {
        rubato_soft_linear_layer(&m_state.share[j], &m_state.share[j]);
    }

    masked_poly_soft_mult_add(&m_keystream, &coeffs[RUBATO_R], &m_key, &m_state);
    masked_poly_soft_unmask_n(&keystream, &m_keystream, RUBATO_OUTPUTSIZE);
    poly_soft_pointwise_add(ciphertext, &keystream, plaintext);
    
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
