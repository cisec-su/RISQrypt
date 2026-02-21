#include <stdint.h>
#include <stddef.h>
#include "params.h"
#include "poly.h"
#include "ntt_lite.h"
#include "util.h"

/**
 * @brief PASTA S-box Cube layer: B[i] = A[i]^3 mod Q
 *        Uses HW accelerator (ntt_lite_pwm)
 *
 * @param B Output polynomial
 * @param A Input polynomial
 */
void sbox_cube(poly *B, poly *A) {
    ntt_lite_pwm(NTT_LITE_OUTPUT_DIS, A->coeffs, A->coeffs);
    ntt_lite_pwm(B->coeffs, NTT_LITE_INPUT_DIS, A->coeffs);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief PASTA S-box Feistel layer: B[0]=A[0], B[i]=A[i]+A[i-1]^2 mod Q
 *        Uses HW accelerator (ntt_lite_pwm, ntt_lite_add)
 *
 * @param B Output polynomial
 * @param A Input polynomial
 */
void sbox_feistel(poly *B, const poly *A) {


    // Feistel function implementation: B[0]=A[0], B[i]=A[i]+A[i-1]^2 mod Q
    uint32_t C[N+1];  // Shifted version of A
    ntt_lite_set_bound(0);
    C[0] = 0x00000;
    ntt_lite_add_const(C + 1, A->coeffs);
    ntt_lite_pwm(NTT_LITE_OUTPUT_DIS, C, C);  // C_square = C^2
    ntt_lite_add(B->coeffs, NTT_LITE_INPUT_DIS, A->coeffs); // B = A + C^2
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Calculate row operation: C[0] = A[0]*B[N-1], C[i] = A[i]*B[N-1] + B[i-1] for i >= 1
 *        Uses HW accelerator (ntt_lite_mul_const, ntt_lite_add)
 *
 * @param C Output array
 * @param B Input array (previous row)
 * @param A Input polynomial (first row)
 */
void calculate_row(uint32_t *C, const uint32_t *B, const poly *A) {
    ntt_lite_set_bound(B[N]);
    ntt_lite_mul_const(NTT_LITE_OUTPUT_DIS, A->coeffs);
    ntt_lite_add(C, NTT_LITE_INPUT_DIS, B);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief PASTA linear mixing layer: B_left = 2*A_left + A_right, B_right = A_left + 2*A_right
 *        Uses HW accelerator (ntt_lite_mul_const, ntt_lite_add)
 *
 * @param B_left  Output left polynomial
 * @param B_right Output right polynomial
 * @param A_left  Input left polynomial
 * @param A_right Input right polynomial
 */
void mix(poly *B_left, poly *B_right, const poly *A_left, const poly *A_right) {
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
 * @brief Matrix-vector multiplication for PASTA
 *        new_state = M * state, where M is generated row-by-row using calculate_row
 *        Uses HW accelerator (ntt_lite_pwm, ntt_lite_sum)
 *
 * @param new_state Output polynomial (result of matrix-vector multiplication)
 * @param state     Input polynomial (vector to multiply)
 * @param nonce     Nonce for SHAKE128 seed
 * @param block_ctr Block counter for SHAKE128 seed
 * @param poly_ctr  Polynomial counter for SHAKE128 seed
 */
void matmul(poly *new_state, const poly *state, uint64_t nonce, uint64_t block_ctr, uint8_t poly_ctr) {
    poly rand;
    uint32_t curr_row[N << 1];
    size_t i;

    int allow_zero = 0;
    // Generate random vector (no zeros)
    poly_uniform(&rand, nonce, block_ctr, poly_ctr, allow_zero, 0);

    ntt_lite_set_bound(0);
    ntt_lite_add_const(curr_row + N, rand.coeffs); 
    ntt_lite_mul_const(curr_row, NTT_LITE_INPUT_DIS);

    // For each row in the matrix
    for (i = 0; i < N; i++) {
        ntt_lite_pwm(NTT_LITE_OUTPUT_DIS, curr_row + N - i, state->coeffs); 
        ntt_lite_sum(&(new_state->coeffs[i]), NTT_LITE_INPUT_DIS);
        // Calculate next row if not last iteration
        if (i != N - 1) {
            calculate_row(curr_row + N - i - 1, curr_row + N - i - 1, &rand); 
        }
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief PASTA round function (HW accelerated)
 *
 * Performs one round: matmul -> add_rc -> mix -> sbox
 *
 * @param C         Output polynomial for state 1
 * @param D         Output polynomial for state 2
 * @param A         Input polynomial for state 1
 * @param B         Input polynomial for state 2
 * @param nonce     Nonce for SHAKE128 seed
 * @param block_ctr Block counter for SHAKE128 seed
 * @param r         Round number (0-indexed)
 */
void pasta_round(poly *C, poly *D, const poly *A, const poly *B, uint64_t nonce, uint64_t block_ctr, int r) {
    poly temp1, temp2;  // After matmul
    poly temp3, temp4;  // After add_rc
    poly rand;
    uint8_t poly_ctr;

    poly_ctr = (r << 2);

    // Matrix multiplication on both states
    matmul(&temp1, A, nonce, block_ctr, poly_ctr);
    poly_ctr++;

    matmul(&temp2, B, nonce, block_ctr, poly_ctr);
    poly_ctr++;

    // Add round constants (random polynomials with allow_zero=1)
    poly_uniform(&rand, nonce, block_ctr, poly_ctr, 1,1);
    ntt_lite_add(temp3.coeffs, NTT_LITE_INPUT_DIS, temp1.coeffs);
    poly_ctr++;

    poly_uniform(&rand, nonce, block_ctr, poly_ctr, 1,1);
    ntt_lite_add(temp4.coeffs, NTT_LITE_INPUT_DIS, temp2.coeffs);
    poly_ctr++;

    // Step 3: Mix the two states - single bound set for both mix operations
    ntt_lite_set_bound(2);

    // Step 4: Apply S-box (cube for last round, feistel otherwise)
    if (r == PASTA_R ){
         // Final round: D output is discarded, only C is used
        ntt_lite_mul_const(NTT_LITE_OUTPUT_DIS, temp3.coeffs);
        ntt_lite_add(C->coeffs, NTT_LITE_INPUT_DIS, temp4.coeffs);
    } else if (r == PASTA_R - 1) {
        ntt_lite_mul_const(NTT_LITE_OUTPUT_DIS, temp3.coeffs);
        ntt_lite_add(temp1.coeffs, NTT_LITE_INPUT_DIS, temp4.coeffs);

        ntt_lite_mul_const(NTT_LITE_OUTPUT_DIS, temp4.coeffs);
        ntt_lite_add(temp2.coeffs, NTT_LITE_INPUT_DIS, temp3.coeffs);
        sbox_cube(C, &temp1);
        sbox_cube(D, &temp2);
    } else {
        ntt_lite_mul_const(NTT_LITE_OUTPUT_DIS, temp3.coeffs);
        ntt_lite_add(temp1.coeffs, NTT_LITE_INPUT_DIS, temp4.coeffs);

        ntt_lite_mul_const(NTT_LITE_OUTPUT_DIS, temp4.coeffs);
        ntt_lite_add(temp2.coeffs, NTT_LITE_INPUT_DIS, temp3.coeffs);
        sbox_feistel(C, &temp1);
        sbox_feistel(D, &temp2);
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief PASTA encrypt one block (unmasked, HW accelerated)
 *
 * Encrypts plaintext using PASTA cipher: keystream = PASTA(key, nonce) then
 * ciphertext = plaintext + keystream mod Q
 *
 * @param ciphertext Output polynomial (plaintext XORed with keystream)
 * @param plaintext  Input polynomial (unmasked)
 * @param key        Input key array (size 2*N = 256 elements, two 128-element states)
 * @param nonce      Nonce value for SHAKE128 seed
 */
void pasta_encrypt_one_block(poly *ciphertext, const poly *plaintext, const int32_t *key, uint64_t nonce) {
    // Generate keystream for this block (hardware-accelerated)
    poly state1, state2;
    size_t r;
    uint64_t block_ctr;

    block_ctr = 0x0;
    
    poly_init_q();

    ntt_lite_set_bound(0);
    ntt_lite_add_const(state1.coeffs, key); 
    ntt_lite_add_const(state2.coeffs, key + N);

    // Run PASTA_R rounds
    for (r = 0; r < PASTA_R+1; r++) {
        pasta_round(&state1, &state2, &state1, &state2, nonce, block_ctr, r);
    }
    ntt_lite_add(ciphertext->coeffs, state1.coeffs, plaintext->coeffs);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

