#include <stdint.h>
#include <stddef.h>
#include "params.h"
#include "rubato_soft.h"
#include "masked_poly_soft.h"
#include "poly_soft.h"

/**
 * @brief Masked RUBATO S-box Feistel layer (software)
 * @description Computes B[0] = A[0], B[i] = A[i] + A[i-1]^2 using masked polynomial operations.
 *              The squares come from masked_poly_soft_mult; the addition is linear,
 *              so it is done share-wise with a branch-free reduction (no division).
 * @param B pointer to output masked polynomial (may alias A)
 * @param A pointer to input masked polynomial, shares in [0, Q)
 * @return void
 */
void masked_rubato_soft_sbox_feistel(masked_poly *B, const masked_poly *A) {
    // Feistel: B[0]=A[0], B[i]=A[i]+A[i-1]^2 mod Q
    size_t i;
    size_t j;
    uint32_t sum;
    masked_poly A_squared;

    // Compute A^2 for all elements
    masked_poly_soft_mult(&A_squared, A, A);

    // B[0] = A[0]
    for (j = 0; j < MASKING_N; j++)
        B->share[j].coeffs[0] = A->share[j].coeffs[0];

    // B[i] = A[i] + A[i-1]^2 for i >= 1 (A_squared is a copy, so B may alias A)
    for (i = 1; i < N; i++) {
        for (j = 0; j < MASKING_N; j++) {
            sum = (uint32_t)A->share[j].coeffs[i] + (uint32_t)A_squared.share[j].coeffs[i - 1];
            B->share[j].coeffs[i] = (int32_t)csub_q(sum);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Masked RUBATO encrypt one block (software)
 * @description Generates one masked RUBATO block in the same control flow as
 *              rubato_soft_encrypt. Keystream, noise and plaintext are all
 *              combined in the masked domain; only the final ciphertext is
 *              unmasked. All N coefficients are produced; truncation to
 *              RUBATO_OUTPUTSIZE is left to the caller.
 * @param ciphertext pointer to output polynomial (all N coefficients in [0, Q))
 * @param plaintext pointer to input plaintext coefficients (any int32, reduced
 *                  mod Q to [0, Q) before masked_poly_soft_add_unmasked, same as
 *                  rubato_soft_encrypt)
 * @param key pointer to secret key polynomial (N coefficients in [0, Q))
 * @param nonce 8-byte nonce value
 * @param block_ctr block counter value
 * @return void
 */
void masked_rubato_soft_encrypt(poly *ciphertext, const poly *plaintext, const poly *key, uint64_t nonce, uint64_t block_ctr) {
    poly coeffs[RUBATO_R + 1];
    poly state;
    size_t r;
    size_t i;
    size_t j;
    masked_poly m_state;
    masked_poly m_key;
    masked_poly m_keystream;
    masked_poly m_noise;
    poly noise;

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

    /* + noise: sample, mask, add share-wise */
    rubato_soft_getnoise(&noise, nonce, block_ctr, RUBATO_R + 1);
    masked_poly_soft_mask(&m_noise, &noise);
    masked_poly_soft_add(&m_keystream, &m_keystream, &m_noise);
    masked_poly_soft_add_unmasked(&m_keystream, plaintext, &m_keystream);
    masked_poly_soft_unmask(ciphertext, &m_keystream);
}
