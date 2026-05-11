#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "params.h"
#include "poly.h"
#include "ntt_lite.h"
#include "util.h"
#include "pasta.h"


/**
 * @brief PASTA S-box cube layer
 * @description Computes B[i] = A[i]^3 mod Q for all i using the hardware accelerator (ntt_lite_pwm). First computes A^2, then multiplies result by A
 * @param B pointer to output polynomial
 * @param A pointer to input polynomial
 * @return void
 */
void pasta_sbox_cube(poly *B, poly *A) {
    ntt_lite_pwm(NTT_LITE_OUTPUT_DIS, A->coeffs, A->coeffs);
    ntt_lite_pwm(B->coeffs, NTT_LITE_INPUT_DIS, A->coeffs);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief PASTA S-box Feistel layer
 * @description Computes the Feistel transformation: B[0] = A[0], B[i] = A[i] + A[i-1]^2 mod Q using the hardware accelerator. Shifts A left by one position (padding with 0), computes square of shifted version, then adds original A
 * @param B pointer to output polynomial
 * @param A pointer to input polynomial * @return void
 */
void pasta_sbox_feistel(poly *B, const poly *A) {

    // Feistel function implementation: B[0]=A[0], B[i]=A[i]+A[i-1]^2 mod Q
    uint32_t C[N+1];  // Shifted version of A
    C[0] = 0x00000;
    memcpy(C + 1, A->coeffs, N * sizeof(uint32_t));
    ntt_lite_pwm(NTT_LITE_OUTPUT_DIS, C, C);  // C_square = C^2
    ntt_lite_add(B->coeffs, NTT_LITE_INPUT_DIS, A->coeffs); // B = A + C^2
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Calculate next row for matrix multiplication
 * @description Generates the next row of the PASTA matrix for row i: C[0] = A[0] * B[N-1]; C[i] = A[i] * B[N-1] + B[i-1] for i >= 1. Uses the hardware accelerator (ntt_lite_mul_const, ntt_lite_add). B[N] is used as the scalar bound
 * @param C pointer to output coefficient array (length N)
 * @param B pointer to previous row array (length N+1; B[N] is the scalar bound)
 * @param A pointer to first row polynomial
 * @return void
 */
void pasta_calculate_row(uint32_t *C, const uint32_t *B, const poly *A) {
    ntt_lite_set_bound(B[N]);
    ntt_lite_mul_const(NTT_LITE_OUTPUT_DIS, A->coeffs);
    ntt_lite_add(C, NTT_LITE_INPUT_DIS, B);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief PASTA linear mixing layer
 * @description Applies the mixing transformation: B_left = 2 * A_left + A_right mod Q; B_right = A_left + 2 * A_right mod Q. Uses single hardware accelerator bound setting for both operations (bound=2)
 * @param B_left pointer to output left polynomial
 * @param B_right pointer to output right polynomial
 * @param A_left pointer to input left polynomial
 * @param A_right pointer to input right polynomial
 * @return void
 */
void pasta_mix(poly *B_left, poly *B_right, const poly *A_left, const poly *A_right) {
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
 * @description Computes new_state = M * state, where the matrix M is generated row-by-row dynamically using pasta_calculate_row and a random polynomial (rand) derived from the nonce/block_ctr/poly_ctr seed. The first row is rand, and subsequent rows are computed using pasta_calculate_row. Uses the hardware accelerator (ntt_lite_pwm for dot product, ntt_lite_sum for accumulation)
 * @param new_state pointer to output polynomial
 * @param state pointer to input state vector polynomial
 * @param nonce nonce for SHAKE128 seed
 * @param block_ctr block counter for SHAKE128 seed
 * @param poly_ctr polynomial counter for SHAKE128 seed
 * @return void
 */
void pasta_matmul(poly *new_state, const poly *state, uint64_t nonce, uint64_t block_ctr, uint8_t poly_ctr) {
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
            pasta_calculate_row(curr_row + N - i - 1, curr_row + N - i - 1, &rand);
        }
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief PASTA round function
 * @description Performs one PASTA cipher round consisting of: 1. Matrix multiplication (pasta_matmul) on both state A and B; 2. Add round constants (random polynomials); 3. Linear mixing (pasta_mix); 4. S-box operation: cube for last round (r == PASTA_R), Feistel for other rounds. Uses the hardware accelerator for all sub-operations
 * @param C pointer to output polynomial for state 1
 * @param D pointer to output polynomial for state 2
 * @param A pointer to input polynomial for state 1
 * @param B pointer to input polynomial for state 2
 * @param nonce nonce for SHAKE128 seed
 * @param block_ctr block counter for SHAKE128 seed
 * @param r round number (0-indexed)
 * @return void
 */
void pasta_round(poly *C, poly *D, const poly *A, const poly *B, uint64_t nonce, uint64_t block_ctr, size_t r) {
    poly temp1, temp2;  // After pasta_matmul
    poly temp3, temp4;  // After add_rc
    poly rand;
    uint8_t poly_ctr;

    poly_ctr = (r << 2);

    // Matrix multiplication on both states
    pasta_matmul(&temp1, A, nonce, block_ctr, poly_ctr);
    poly_ctr++;

    pasta_matmul(&temp2, B, nonce, block_ctr, poly_ctr);
    poly_ctr++;

    // Add round constants (random polynomials with allow_zero=1)
    poly_uniform(&rand, nonce, block_ctr, poly_ctr, 1, 1);
    ntt_lite_add(temp3.coeffs, NTT_LITE_INPUT_DIS, temp1.coeffs);
    poly_ctr++;

    poly_uniform(&rand, nonce, block_ctr, poly_ctr, 1, 1);
    ntt_lite_add(temp4.coeffs, NTT_LITE_INPUT_DIS, temp2.coeffs);
    poly_ctr++;

    // Step 3: Mix the two states - single bound set for both pasta_mix operations
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

        pasta_sbox_cube(C, &temp1);
        pasta_sbox_cube(D, &temp2);
    } else {
        ntt_lite_mul_const(NTT_LITE_OUTPUT_DIS, temp3.coeffs);
        ntt_lite_add(temp1.coeffs, NTT_LITE_INPUT_DIS, temp4.coeffs);

        ntt_lite_mul_const(NTT_LITE_OUTPUT_DIS, temp4.coeffs);
        ntt_lite_add(temp2.coeffs, NTT_LITE_INPUT_DIS, temp3.coeffs);

        pasta_sbox_feistel(C, &temp1);
        pasta_sbox_feistel(D, &temp2);
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief PASTA encrypt one block (unmasked, HW accelerated)
 * @description Encrypts a plaintext block using the PASTA cipher: 1. Initializes two state polynomials from the key (first N coefficients in state1, second N in state2); 2. Runs PASTA_R+1 rounds; 3. Adds plaintext to final keystream state1. Ciphertext = plaintext + PASTA(key, nonce) mod Q
 * @param ciphertext pointer to output polynomial
 * @param plaintext pointer to input plaintext polynomial
 * @param key pointer to key array (size 2*N coefficients)
 * @param nonce 8-byte nonce value
 * @return void
 */
void pasta_encrypt(poly *ciphertext, const poly *plaintext, const int32_t *key, uint64_t nonce) {
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
    for (r = 0; r < (size_t)PASTA_R+1; r++) {
        pasta_round(&state1, &state2, &state1, &state2, nonce, block_ctr, r);
    }
    ntt_lite_add(ciphertext->coeffs, state1.coeffs, plaintext->coeffs);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * @brief Generate constant key, nonce and block counter values for tests
 * @param key[out]        buffer of 2*N coefficients (int32_t) to receive the key
 * @param nonce[out]      pointer to receive the nonce value (may be NULL)
 * @param block_ctr[out]  pointer to receive the block counter (may be NULL)
 *
 * The values returned are fixed constants defined in the implementation.
 */
void pasta_key_gen(int32_t *key, uint64_t *nonce, uint64_t *block_ctr){
    size_t i;

    if (key != NULL) {
        for (i = 0; i < 2 * N; i++) {
            key[i] = 1;
        }
    }

    if (nonce != NULL) {
        *nonce = 0x123456789ULL;
    }
    if (block_ctr != NULL) {
        *block_ctr = 0x0ULL;
    }
}
