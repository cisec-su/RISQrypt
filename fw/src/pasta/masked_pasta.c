#include <stdint.h>
#include <stddef.h>
#include "params.h"
#include "poly.h"
#include "pasta.h"
#include "ntt_lite.h"
#include "util.h"
#include "masked_gadgets.h"

/**
 * @brief Masked PASTA S-box cube layer
 * @description Computes B = A^3 mod Q using masked polynomial operations. Calls masked_poly_cube internally to preserve masking throughout the computation
 * @param B pointer to output masked polynomial
 * @param A pointer to input masked polynomial
 * @return void
 */
void masked_pasta_sbox_cube(masked_poly *B, const masked_poly *A) {
    masked_poly_cube(B,A);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Masked PASTA S-box Feistel layer
 * @description Computes B[0] = A[0], B[i] = A[i] + A[i-1]^2 using masked polynomial operations. Shifts A left by one position per share, then calls masked_poly_mac to compute B = shift(A)^2 + A, preserving masking throughout
 * @param B pointer to output masked polynomial
 * @param A pointer to input masked polynomial
 * @return void
 */
void masked_pasta_sbox_feistel(masked_poly *B, const masked_poly *A) {
    // Feistel function implementation: B[0]=A[0], B[i]=A[i]+A[i-1]^2 mod Q
    static masked_poly A_shifted, A_refreshed; // 0 a0 a1 a2 a3 a4 ... a126
    masked_poly_right_shift(&A_shifted,A,1);
    masked_gadgets_x2x_a_ref(&A_refreshed, &A_shifted);
    masked_poly_mac(B,&A_shifted,&A_refreshed,A);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Calculate next row for masked matrix multiplication
 * @description Generates the next row for the dynamic matrix in masked pasta_matmul: C[0] = A[0] * B[N-1]; C[i] = A[i] * B[N-1] + B[i-1] for i >= 1. Uses hardware accelerator (ntt_lite_mul_const, ntt_lite_add). B[N] is used as the scalar bound. The input A is an unmasked polynomial (the random generator), while C and B are raw coefficient arrays
 * @param C pointer to output coefficient array (length N)
 * @param B pointer to previous row coefficient array (length N+1; B[N] is the scalar)
 * @param A pointer to unmasked polynomial (first row/generator)
 * @return void
 */
void masked_pasta_calculate_row(uint32_t *C, const uint32_t *B, const poly *A) {
    ntt_lite_set_bound(B[N]);
#ifdef MEMORY_OPT_DIS
    uint32_t temp[N];
    ntt_lite_mul_const(temp, A->coeffs);
    ntt_lite_add(C, temp, B);
#else
    ntt_lite_mul_const(NTT_LITE_OUTPUT_DIS, A->coeffs);
    ntt_lite_add(C, NTT_LITE_INPUT_DIS, B);
#endif
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Masked matrix-vector multiplication for PASTA
 * @description Computes new_state = M * state where M is generated row-by-row, with operations performed directly on masked shares. For each row i and each share j: new_state.share[j][i] = sum_k (M[i][k] * state.share[j][k]). The matrix is generated using an unmasked random polynomial (rand), and the linear structure of pasta_matmul allows applying it independently to each masked share. Uses hardware accelerator (ntt_lite_pwm, ntt_lite_sum)
 * @param new_state pointer to output masked polynomial
 * @param state pointer to input masked polynomial
 * @param nonce nonce for SHAKE128 seed
 * @param block_ctr block counter for SHAKE128 seed
 * @param poly_ctr polynomial counter for SHAKE128 seed
 * @return void
 */
void masked_pasta_matmul(masked_poly *new_state, const masked_poly *state, uint64_t nonce, uint64_t block_ctr, uint8_t poly_ctr) {
    static poly rand;
    size_t j;
    size_t i;
    size_t allow_zero;
    static uint32_t curr_row[N << 1];
#ifdef MEMORY_OPT_DIS
    poly temp_pwm;
#endif

    allow_zero = 0;
    // Generate random vector (no zeros)
    poly_uniform(&rand, nonce, block_ctr, poly_ctr, allow_zero,0);

    ntt_lite_set_bound(0);
    ntt_lite_add_const(curr_row + N, rand.coeffs);
#ifdef MEMORY_OPT_DIS
    ntt_lite_mul_const(curr_row, curr_row + N);
#else
    ntt_lite_mul_const(curr_row, NTT_LITE_INPUT_DIS);
#endif

    // For each row in the matrix
    for (i = 0; i < N; i++) {
        for (j=0; j<MASKING_N; j++) {
#ifdef MEMORY_OPT_DIS
            ntt_lite_pwm(temp_pwm.coeffs, curr_row + N - i, state->share[j].coeffs);
            ntt_lite_sum(&(new_state->share[j].coeffs[i]), temp_pwm.coeffs);
#else
            ntt_lite_pwm(NTT_LITE_OUTPUT_DIS, curr_row + N - i, state->share[j].coeffs);
            ntt_lite_sum(&(new_state->share[j].coeffs[i]), NTT_LITE_INPUT_DIS);
#endif
        }
        // Calculate next row if not last iteration, public
        if (i != N - 1)
            masked_pasta_calculate_row(curr_row + N - i - 1, curr_row + N - i - 1, &rand);
    }

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Masked PASTA round function
 * @description Performs one masked PASTA cipher round: 1. Masked matrix multiplication (masked_pasta_matmul) on states A and B; 2. Add round constants (unmasked random polynomials added only to share[0]); 3. Masked mixing via masked_poly_mult_add_const (implements 2*A + B and A + 2*B); 4. Masked S-box: cube for last round, Feistel otherwise. All operations preserve the two-share masking structure
 * @param C pointer to output masked polynomial for state 1
 * @param D pointer to output masked polynomial for state 2
 * @param A pointer to input masked polynomial for state 1
 * @param B pointer to input masked polynomial for state 2
 * @param nonce nonce for SHAKE128 seed
 * @param block_ctr block counter for SHAKE128 seed
 * @param r round number (0-indexed)
 * @return void
 */
void masked_pasta_round(masked_poly *C, masked_poly *D, const masked_poly *A, const masked_poly *B, uint64_t nonce, uint64_t block_ctr, size_t r) {
    uint8_t poly_ctr;
    static poly rand;
    static masked_poly m_temp1, m_temp2;
    static masked_poly m_temp3;

    poly_ctr = (r << 2);

    // Matrix multiplication on both states
    masked_pasta_matmul(&m_temp1, A, nonce, block_ctr, poly_ctr);
    poly_ctr++;

    masked_pasta_matmul(&m_temp2, B, nonce, block_ctr, poly_ctr);
    poly_ctr++;

    // Add round constants (random polynomials with allow_zero=1)
#ifdef MEMORY_OPT_DIS
    poly_uniform(&rand, nonce, block_ctr, poly_ctr, 1, 0);
    ntt_lite_add(m_temp1.share[0].coeffs, rand.coeffs, m_temp1.share[0].coeffs);
#else
    poly_uniform(&rand, nonce, block_ctr, poly_ctr, 1, 1);
    ntt_lite_add(m_temp1.share[0].coeffs, NTT_LITE_INPUT_DIS, m_temp1.share[0].coeffs);
#endif
    poly_ctr++;

#ifdef MEMORY_OPT_DIS
    poly_uniform(&rand, nonce, block_ctr, poly_ctr, 1, 0);
    ntt_lite_add(m_temp2.share[0].coeffs, rand.coeffs, m_temp2.share[0].coeffs);
#else
    poly_uniform(&rand, nonce, block_ctr, poly_ctr, 1, 1);
    ntt_lite_add(m_temp2.share[0].coeffs, NTT_LITE_INPUT_DIS, m_temp2.share[0].coeffs);
#endif
    poly_ctr++;

    ntt_lite_set_bound(2);

    if (r == PASTA_R) {
        // Step 3: only C is used after the final round; skip D computation
        masked_poly_mult_add_const(C,&m_temp1,&m_temp2);
    } else if (r == PASTA_R - 1){
        // Step 3: masked_mix the two states - single bound set for both masked_mix operations
        masked_poly_mult_add_const(&m_temp3,&m_temp1,&m_temp2);
        masked_poly_mult_add_const(&m_temp1,&m_temp2,&m_temp1);

        masked_pasta_sbox_cube(D, &m_temp1);
        masked_pasta_sbox_cube(C, &m_temp3);
    } else {
        // Step 3: masked_mix the two states - single bound set for both masked_mix operations
        masked_poly_mult_add_const(&m_temp3,&m_temp1,&m_temp2);
        masked_poly_mult_add_const(&m_temp1,&m_temp2,&m_temp1);

        masked_pasta_sbox_feistel(D, &m_temp1);
        masked_pasta_sbox_feistel(C, &m_temp3);
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Masked PASTA encrypt one block (HW accelerated)
 * @description Encrypts a plaintext block using masked PASTA cipher: 1. Initializes unmasked key states from key array; 2. Masks both states into two-share form; 3. Runs PASTA_R+1 masked rounds; 4. Adds plaintext to masked state (adds to share[0] only); 5. Unmasks final state to get ciphertext. The entire cipher computation is performed with two-share masking to protect against first-order side-channel attacks
 * @param ciphertext pointer to output polynomial
 * @param plaintext pointer to input plaintext polynomial
 * @param key pointer to key array (size 2*N coefficients)
 * @param nonce 8-byte nonce value
 * @return void
 */
void masked_pasta_encrypt(poly *ciphertext, const poly *plaintext, const int32_t *key, uint64_t nonce) {
    // Generate keystream for this block (hardware-accelerated)
    size_t r;
    uint64_t block_ctr;
    poly state1, state2;
    masked_poly m_state1;
    masked_poly m_state2;

    block_ctr = 0x0;

    poly_init_q();
    ntt_lite_set_bound(0);
    ntt_lite_add_const(state1.coeffs, key);
    ntt_lite_add_const(state2.coeffs, key + N);

    // mask state1 and state2
    masked_gadgets_init_q();
    masked_poly_mask(&m_state1,&state1);
    masked_poly_mask(&m_state2,&state2);

    // Run PASTA_R rounds
    for (r = 0; r < PASTA_R+1; r++) {
        masked_pasta_round(&m_state1, &m_state2, &m_state1, &m_state2, nonce, block_ctr, r);
    }

    masked_poly_add_unmasked(&m_state1, plaintext, &m_state1);
    masked_poly_unmask(ciphertext, &m_state1);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

