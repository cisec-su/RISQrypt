#include <stdint.h>
#include <stddef.h>
#include "params.h"
#include "poly.h"
#include "ntt_lite.h"
#include "util.h"
#include "masked_gadgets.h"

/**
 * @brief PASTA S-box Cube layer: B[i] = A[i]^3 mod Q
 *        Unmasks A, applies cube via HW accelerator (ntt_lite_pwm), then re-masks into B.
 *
 * @param B Output masked polynomial
 * @param A Input masked polynomial
 */
void masked_sbox_cube(masked_poly *B, const masked_poly *A) {
    poly AA;
    poly BB;

    masked_poly_unmask(&AA, A);

    ntt_lite_pwm(NTT_LITE_OUTPUT_DIS, AA.coeffs, AA.coeffs);
    ntt_lite_pwm(BB.coeffs, NTT_LITE_INPUT_DIS, AA.coeffs);

    masked_poly_mask(B, &BB);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief PASTA S-box Feistel layer: B[0]=A[0], B[i]=A[i]+A[i-1]^2 mod Q
 *        Unmasks A, applies Feistel via HW accelerator (ntt_lite_pwm, ntt_lite_add), then re-masks into B.
 *
 * @param B Output masked polynomial
 * @param A Input masked polynomial
 */
void masked_sbox_feistel(masked_poly *B, const masked_poly *A) {
    // Feistel function implementation: B[0]=A[0], B[i]=A[i]+A[i-1]^2 mod Q

    poly AA;
    poly BB;
    uint32_t C[N+1];  // Shifted version of A

    masked_poly_unmask(&AA, A);

    ntt_lite_set_bound(0);
    C[0] = 0x00000;
    ntt_lite_add_const(C + 1, AA.coeffs);
    ntt_lite_pwm(NTT_LITE_OUTPUT_DIS, C, C);  // C_square = C^2
    ntt_lite_add(BB.coeffs, NTT_LITE_INPUT_DIS, AA.coeffs); // B = A + C^2

    masked_poly_mask(B, &BB);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Calculate row operation: C[0] = A[0]*B[N-1], C[i] = A[i]*B[N-1] + B[i-1] for i >= 1
 *        Uses HW accelerator (ntt_lite_mul_const, ntt_lite_add).
 *        B[N] (element past the N-coefficient array) is used as the scalar bound.
 *
 * @param C Output raw coefficient array (length N)
 * @param B Input raw coefficient array of previous row (length N+1, B[N] is the scalar)
 * @param A Input polynomial providing the first row (used as the scalar multiplier source)
 */
void masked_calculate_row(uint32_t *C, const uint32_t *B, const poly *A) {
    ntt_lite_set_bound(B[N]);
    ntt_lite_mul_const(NTT_LITE_OUTPUT_DIS, A->coeffs);
    ntt_lite_add(C, NTT_LITE_INPUT_DIS, B);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief PASTA linear mixing layer (unmasked): B_left = 2*A_left + A_right, B_right = A_left + 2*A_right
 *        Uses HW accelerator (ntt_lite_mul_const, ntt_lite_add)
 *
 * @param B_left  Output left polynomial (unmasked)
 * @param B_right Output right polynomial (unmasked)
 * @param A_left  Input left polynomial (unmasked)
 * @param A_right Input right polynomial (unmasked)
 */
void masked_mix(poly *B_left, poly *B_right, const poly *A_left, const poly *A_right) {
    // B_left = 2*A_left + A_right, B_right = A_left + 2*A_right
    // Uses single ntt_lite_set_bound call for both operations
    ntt_lite_set_bound(2);
    ntt_lite_mul_const(NTT_LITE_OUTPUT_DIS, A_left->coeffs);
    ntt_lite_add(B_left->coeffs, NTT_LITE_INPUT_DIS, A_right->coeffs);
    ntt_lite_mul_const(NTT_LITE_OUTPUT_DIS, A_right->coeffs);
    ntt_lite_add(B_right->coeffs, NTT_LITE_INPUT_DIS, A_left->coeffs);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Masked matrix-vector multiplication for PASTA
 *        new_state = M * state, where M is generated row-by-row using masked_calculate_row.
 *        Unmasks state for dot products, computes result, then re-masks into new_state.
 *        Uses HW accelerator (ntt_lite_pwm, ntt_lite_sum)
 *
 * @param new_state Output masked polynomial (result of matrix-vector multiplication)
 * @param state     Input masked polynomial (vector to multiply)
 * @param nonce     Nonce for SHAKE128 seed
 * @param block_ctr Block counter for SHAKE128 seed
 * @param poly_ctr  Polynomial counter for SHAKE128 seed
 */
void masked_matmul(masked_poly *new_state, const masked_poly *state, uint64_t nonce, uint64_t block_ctr, uint8_t poly_ctr) {
    poly rand;
    uint32_t curr_row[N << 1];
    size_t i;

    poly unmasked_new_state;
    poly unmasked_state;

    masked_poly_unmask(&unmasked_state, state);

    int allow_zero;
    
    allow_zero = 0;
    // Generate random vector (no zeros)
    poly_uniform(&rand, nonce, block_ctr, poly_ctr, allow_zero,0);

    ntt_lite_set_bound(0);
    ntt_lite_add_const(curr_row + N, rand.coeffs); 
    ntt_lite_mul_const(curr_row, NTT_LITE_INPUT_DIS);

    // For each row in the matrix
    for (i = 0; i < N; i++) {
        ntt_lite_pwm(NTT_LITE_OUTPUT_DIS, curr_row + N - i, unmasked_state.coeffs); 
        ntt_lite_sum(&(unmasked_new_state.coeffs[i]), NTT_LITE_INPUT_DIS);
        // Calculate next row if not last iteration
        if (i != N - 1) {
            masked_calculate_row(curr_row + N - i - 1, curr_row + N - i - 1, &rand); 
        }
    }

    masked_poly_mask(new_state, &unmasked_new_state);

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief PASTA masked round function (HW accelerated)
 *
 * Performs one round: masked_matmul -> add_rc -> masked_mix -> masked_sbox
 *
 * @param C         Output masked polynomial for state 1
 * @param D         Output masked polynomial for state 2
 * @param A         Input masked polynomial for state 1
 * @param B         Input masked polynomial for state 2
 * @param nonce     Nonce for SHAKE128 seed
 * @param block_ctr Block counter for SHAKE128 seed
 * @param r         Round number (0-indexed)
 */
void masked_pasta_round(masked_poly *C, masked_poly *D, const masked_poly *A, const masked_poly *B, uint64_t nonce, uint64_t block_ctr, int r) {
    poly temp1, temp2;  // After masked_matmul
    poly temp3, temp4;  // After add_rc
    poly rand;
    uint8_t poly_ctr;

    poly AA;
    poly BB;

    masked_poly m_temp1, m_temp2;  
    masked_poly m_temp3, m_temp4;  
    
    poly_ctr = (r << 2);

    // Matrix multiplication on both states
    masked_matmul(&m_temp1, A, nonce, block_ctr, poly_ctr);
    poly_ctr++;

    masked_matmul(&m_temp2, B, nonce, block_ctr, poly_ctr);
    poly_ctr++;

    // Add round constants (random polynomials with allow_zero=1)
    poly_uniform(&rand, nonce, block_ctr, poly_ctr, 1,1);
    ntt_lite_add(m_temp1.share[0].coeffs,NTT_LITE_INPUT_DIS,m_temp1.share[0].coeffs);
    poly_ctr++;

    poly_uniform(&rand, nonce, block_ctr, poly_ctr, 1,1);
    ntt_lite_add(m_temp2.share[0].coeffs,NTT_LITE_INPUT_DIS,m_temp2.share[0].coeffs);
    poly_ctr++;

    masked_poly_unmask(&temp4,&m_temp2);
    ntt_lite_set_bound(2);

    if (r == PASTA_R) {
        // Step 3: masked_mix the two states - single bound set for both masked_mix operations
        masked_poly_mult_add_const(D,&m_temp2,&m_temp1);
        masked_poly_mult_add_const(C,&m_temp1,&m_temp2);
    } else if (r == PASTA_R - 1){
        // Step 3: masked_mix the two states - single bound set for both masked_mix operations
        masked_poly_mult_add_const(&m_temp3,&m_temp1,&m_temp2);
        masked_poly_mult_add_const(&m_temp1,&m_temp2,&m_temp1);

        masked_sbox_cube(D, &m_temp1);
        masked_sbox_cube(C, &m_temp3);
    } else {
        // Step 3: masked_mix the two states - single bound set for both masked_mix operations
        masked_poly_mult_add_const(&m_temp3,&m_temp1,&m_temp2);
        masked_poly_mult_add_const(&m_temp1,&m_temp2,&m_temp1);

        masked_sbox_feistel(D, &m_temp1);
        masked_sbox_feistel(C, &m_temp3);
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief PASTA encrypt one block (masked, HW accelerated)
 *
 * Encrypts plaintext using masked PASTA cipher: masks key state, runs PASTA_R+1 masked rounds,
 * then adds plaintext to the unmasked final state to produce ciphertext.
 *
 * @param ciphertext Output polynomial (plaintext XORed with keystream)
 * @param plaintext  Input polynomial (unmasked)
 * @param key        Input key array (size 2*N = 256 elements, two 128-element states)
 * @param nonce      Nonce value for SHAKE128 seed
 */
void masked_pasta_encrypt_one_block(poly *ciphertext, const poly *plaintext, const int32_t *key, uint64_t nonce) {
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

    masked_poly_add_unmasked(&m_state1, (poly *)plaintext, &m_state1);
    masked_poly_unmask(ciphertext, &m_state1);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


