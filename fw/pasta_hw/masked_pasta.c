#include <stdint.h>
#include <stddef.h>
#include "params.h"
#include "poly.h"
#include "ntt_lite.h"
#include "util.h"
#include "masked_gadgets.h"
#include "masked.h"

/**
 * @brief PASTA S-box Cube layer: B[i] = A[i]^3 mod Q
 *        Uses HW accelerator (ntt_lite_pwm)
 *
 * @param B Output polynomial
 * @param A Input polynomial
 */
void masked_sbox_cube(poly *B, poly *A) {
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
void masked_sbox_feistel(poly *B, const poly *A) {

    // masking with two-share
    // önce index kaydır
    // 


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
void masked_calculate_row(uint32_t *C, const uint32_t *B, const poly *A) {
    ntt_lite_set_bound(B[N]);
    ntt_lite_mul_const(NTT_LITE_OUTPUT_DIS, A->coeffs);
    ntt_lite_add(C, NTT_LITE_INPUT_DIS, B);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief PASTA linear masked_mixing layer: B_left = 2*A_left + A_right, B_right = A_left + 2*A_right
 *        Uses HW accelerator (ntt_lite_mul_const, ntt_lite_add)
 *
 * @param B_left  Output left polynomial
 * @param B_right Output right polynomial
 * @param A_left  Input left polynomial
 * @param A_right Input right polynomial
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
 * @brief Matrix-vector multiplication for PASTA
 *        new_state = M * state, where M is generated row-by-row using masked_calculate_row
 *        Uses HW accelerator (ntt_lite_pwm, ntt_lite_sum)
 *
 * @param new_state Output polynomial (result of matrix-vector multiplication)
 * @param state     Input polynomial (vector to multiply)
 * @param nonce     Nonce for SHAKE128 seed
 * @param block_ctr Block counter for SHAKE128 seed
 * @param poly_ctr  Polynomial counter for SHAKE128 seed
 */
void masked_matmul(poly *new_state, const poly *state, uint64_t nonce, uint64_t block_ctr, uint8_t poly_ctr) {
    poly rand;
    uint32_t curr_row[N << 1];
    size_t i;

    int allow_zero = 0;
    // Generate random vector (no zeros)
    poly_uniform(&rand, nonce, block_ctr, poly_ctr, allow_zero,0);

    ntt_lite_set_bound(0);
    ntt_lite_add_const(curr_row + N, rand.coeffs); 
    ntt_lite_mul_const(curr_row, NTT_LITE_INPUT_DIS);

    // For each row in the matrix
    for (i = 0; i < N; i++) {
        ntt_lite_pwm(NTT_LITE_OUTPUT_DIS, curr_row + N - i, state->coeffs); 
        ntt_lite_sum(&(new_state->coeffs[i]), NTT_LITE_INPUT_DIS);
        // Calculate next row if not last iteration
        if (i != N - 1) {
            masked_calculate_row(curr_row + N - i - 1, curr_row + N - i - 1, &rand); 
        }
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief PASTA round function (HW accelerated)
 *
 * Performs one round: masked_matmul -> add_rc -> masked_mix -> sbox
 *
 * @param C         Output polynomial for state 1
 * @param D         Output polynomial for state 2
 * @param A         Input polynomial for state 1
 * @param B         Input polynomial for state 2
 * @param nonce     Nonce for SHAKE128 seed
 * @param block_ctr Block counter for SHAKE128 seed
 * @param r         Round number (0-indexed)
 */
void masked_pasta_round(masked_poly *C, masked_poly *D, const masked_poly *A, const masked_poly *B, uint64_t nonce, uint64_t block_ctr, int r) {
    poly temp1, temp2;  // After masked_matmul
    poly temp3, temp4;  // After add_rc
    poly temp5, temp6;  // After masked_mix
    poly rand;
    uint8_t poly_ctr;

    poly AA;
    poly BB;
    poly CC;
    poly DD;

    masked_poly m_temp1, m_temp2;  
    masked_poly m_temp3, m_temp4;  
    
    masked_poly_unmask(&AA,A);
    masked_poly_unmask(&BB,B);

    poly_ctr = (r << 2);

    // Matrix multiplication on both states
    masked_matmul(&temp1, &AA, nonce, block_ctr, poly_ctr);
    poly_ctr++;

    masked_matmul(&temp2, &BB, nonce, block_ctr, poly_ctr);
    poly_ctr++;

    masked_poly_mask(&m_temp1,&temp1);
    masked_poly_mask(&m_temp2,&temp2);

    // Add round constants (random polynomials with allow_zero=1)
    poly_uniform(&rand, nonce, block_ctr, poly_ctr, 1,0);
    poly_ctr++;

    masked_poly_add_unmasked(&m_temp1,&rand,&m_temp1);
    masked_poly_unmask(&temp3,&m_temp1);

    //ntt_lite_add(temp3.coeffs, temp1.coeffs, rand.coeffs);

    poly_uniform(&rand, nonce, block_ctr, poly_ctr, 1,0);
    poly_ctr++;

    masked_poly_add_unmasked(&m_temp2,&rand,&m_temp2);
    masked_poly_unmask(&temp4,&m_temp2);

    // Step 3: masked_mix the two states - single bound set for both masked_mix operations
    ntt_lite_set_bound(2);

    // masked_poly_mult_add_const(masked_poly *r, masked_poly *a, masked_poly *b);
    masked_poly_mult_add_const(&m_temp3,&m_temp1,&m_temp2);
    masked_poly_unmask(&temp5,&m_temp3);

    masked_poly_mult_add_const(&m_temp1,&m_temp2,&m_temp1);
    masked_poly_unmask(&temp6,&m_temp1);

    // ntt_lite_mul_const(NTT_LITE_OUTPUT_DIS, temp4.coeffs);
    // ntt_lite_add(temp6.coeffs, NTT_LITE_INPUT_DIS, temp3.coeffs);
    
    // Step 4: Apply S-box (cube for last round, feistel otherwise)
    if (r == PASTA_R - 1) {
        masked_sbox_cube(&CC, &temp5);
        masked_sbox_cube(&DD, &temp6);
    } else {
        masked_sbox_feistel(&CC, &temp5);
        masked_sbox_feistel(&DD, &temp6);
    }

    masked_poly_mask(C,&CC);
    masked_poly_mask(D,&DD);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief PASTA encrypt one block (HW accelerated)
 *
 * Encrypts plaintext using PASTA cipher: keystream = PASTA(key, nonce) then
 * ciphertext = plaintext + keystream mod Q
 *
 * @param ciphertext Output polynomial
 * @param plaintext  Input polynomial
 * @param key        Input key array (size 2*N = 256 elements)
 * @param nonce      Nonce value
 */
void masked_pasta_encrypt_one_block(poly *ciphertext, const poly *plaintext, const int32_t *key, uint64_t nonce) {
    // Generate keystream for this block (hardware-accelerated)

    poly rand;
    poly state1, state2;
    poly temp1, temp2;
    size_t r;
    uint8_t poly_ctr;
    uint64_t block_ctr;
    int allow_zero;
    masked_poly m_state1;
    masked_poly m_state2;

    block_ctr = 0x0;
    allow_zero = 1;
    
    poly_init_q();

    ntt_lite_set_bound(0);
    ntt_lite_add_const(state1.coeffs, key); 
    ntt_lite_add_const(state2.coeffs, key + N);

    // mask state1 and state2 
    masked_gadgets_init_q();
    masked_gadgets_mask_poly(&m_state1,&state1);
    masked_gadgets_mask_poly(&m_state2,&state2);

    // print_string("state1");
    // print_u32_arr(state1.coeffs,16);
    // print_string("m_state1 [0]");
    // print_u32_arr(m_state1.share[0].coeffs,16);
    // print_string("m_state1 [1]");
    // print_u32_arr(m_state1.share[1].coeffs,16);

    // Run PASTA_R rounds
    for (r = 0; r < PASTA_R; r++) {
        masked_pasta_round(&m_state1, &m_state2, &m_state1, &m_state2, nonce, block_ctr, r);
    }

    masked_poly_unmask(&state1,&m_state1);
    masked_poly_unmask(&state2,&m_state2);
    
    poly_ctr = (PASTA_R << 2);

    // Final masked_matmul on both states
    masked_matmul(&temp1, &state1, nonce, block_ctr, poly_ctr);
    poly_ctr++;
    masked_matmul(&temp2, &state2, nonce, block_ctr, poly_ctr);
    poly_ctr++;

    // Add final round constants
    poly_uniform(&rand, nonce, block_ctr, poly_ctr, allow_zero,0);
    poly_ctr++;
    poly_add(&state1, &temp1, &rand);

    poly_uniform(&rand, nonce, block_ctr, poly_ctr, allow_zero,0);
    poly_ctr++;
    poly_add(&state2, &temp2, &rand);

    // Final masked_mix (state1 becomes the keystream)
    ntt_lite_set_bound(2);
    ntt_lite_mul_const(NTT_LITE_OUTPUT_DIS, state1.coeffs);
    ntt_lite_add(NTT_LITE_OUTPUT_DIS, NTT_LITE_INPUT_DIS, state2.coeffs);
    ntt_lite_add(ciphertext->coeffs, NTT_LITE_INPUT_DIS, plaintext->coeffs);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


