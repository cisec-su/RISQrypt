#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "params.h"
#include "pasta_soft.h"
#include "fips202.h"
#include "util.h"
#include "masked_poly.h"

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
    pasta_soft_poly_uniform(&r, 0, 0, 0, 1);

    // c0 = a0 * b0 - r
    pasta_soft_poly_pointwise_mult(&c0_temp, &A->share[0], &B->share[0]);
    pasta_soft_poly_pointwise_sub(&C->share[0], &c0_temp, &r);

    // r' = a0*b1 + r
    pasta_soft_poly_pointwise_mult(&r_prime, &A->share[0], &B->share[1]);
    pasta_soft_poly_pointwise_add(&r_prime, &r_prime, &r);

    // r' = (a0*b1 + r) + (b0*a1)
    pasta_soft_poly_pointwise_mult(&c1_temp, &A->share[1], &B->share[0]);
    pasta_soft_poly_pointwise_add(&r_prime, &r_prime, &c1_temp);

    // c1 = a1*b1 + r'
    pasta_soft_poly_pointwise_mult(&c1_temp, &A->share[1], &B->share[1]);
    pasta_soft_poly_pointwise_add(&C->share[1], &c1_temp, &r_prime);
}

/**
 * @brief Masked polynomial square (software)
 * @description Computes B = A^2 using pure software pointwise multiplication with masking
 * @param B pointer to output masked polynomial
 * @param A pointer to input masked polynomial
 * @return void
 */
void masked_poly_soft_square(masked_poly *B, const masked_poly *A) {
    // B = A^2 = (a0 + a1)^2 = a0^2 + 2*a0*a1 + a1^2
    // b0 = a0^2, b1 = 2*a0*a1 + a1^2
    masked_poly_soft_mult(B, A, A);
}

/**
 * @brief Masked polynomial cube (software)
 * @description Computes B = A^3 using pure software operations: B = A * (A * A). Uses two-share masking
 * @param B pointer to output masked polynomial
 * @param A pointer to input masked polynomial
 * @return void
 */
void masked_poly_soft_cube(masked_poly *B, const masked_poly *A) {
    masked_poly A2, A3;
    masked_poly_soft_square(&A2, A);
    masked_poly_soft_mult(B, A, &A2);
}

/**
 * @brief Masked polynomial add (software)
 * @description Computes C = A + B with masking: c[i] = a[i] + b[i] for each share
 * @param C pointer to output masked polynomial
 * @param A pointer to first input masked polynomial
 * @param B pointer to second input masked polynomial
 * @return void
 */
void masked_poly_soft_add(masked_poly *C, const masked_poly *A, const masked_poly *B) {
    size_t i, j;
    for (j = 0; j < MASKING_N; j++) {
        for (i = 0; i < N; i++) {
            C->share[j].coeffs[i] = MOD_Q_ADD(A->share[j].coeffs[i], B->share[j].coeffs[i]);
        }
    }
}

/**
 * @brief Masked polynomial right shift (software)
 * @description Shifts polynomial left by shift_count positions: B[shift_count..N-1] = A[0..N-shift_count-1]
 * @param B pointer to output masked polynomial
 * @param A pointer to input masked polynomial
 * @param shift_count number of leading zeros
 * @return void
 */
void masked_poly_soft_right_shift(masked_poly *B, const masked_poly *A, size_t shift_count) {
    size_t i, j;

    if (shift_count > N) {
        for (i = 0; i < MASKING_N; i++) {
            for (j = 0; j < N; j++)
                B->share[i].coeffs[j] = 0;
        }
        return;
    }

    for (i = 0; i < MASKING_N; i++) {
        for (j = 0; j < shift_count; j++)
            B->share[i].coeffs[j] = 0;
        for (j = 0; j < N - shift_count; j++)
            B->share[i].coeffs[shift_count + j] = A->share[i].coeffs[j];
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
    size_t i;
    uint32_t diff;
    poly rand;

    // Generate random mask r0 using pasta_soft_poly_uniform
    pasta_soft_poly_uniform(&rand, 0, 0, 0, 1);
    for (i = 0; i < N; i++) {
        r->share[0].coeffs[i] = rand.coeffs[i];
    }

    // r1 = a - r0
    for (i = 0; i < N; i++) {
        diff = a->coeffs[i] + Q - r->share[0].coeffs[i];
        if (diff >= Q)
            diff = diff - Q;
        r->share[1].coeffs[i] = diff;
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
    size_t i;
    for (i = 0; i < N; i++) {
        a->coeffs[i] = MOD_Q_ADD(r->share[0].coeffs[i], r->share[1].coeffs[i]);
    }
}

/**
 * @brief Masked PASTA S-box cube layer (software)
 * @description Computes B = A^3 mod Q using masked polynomial operations
 * @param B pointer to output masked polynomial
 * @param A pointer to input masked polynomial
 * @return void
 */
void masked_pasta_soft_sbox_cube(masked_poly *B, const masked_poly *A) {
    masked_poly_soft_cube(B, A);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Masked PASTA S-box Feistel layer (software)
 * @description Computes B[0] = A[0], B[i] = A[i] + A[i-1]^2 using masked polynomial operations
 * @param B pointer to output masked polynomial
 * @param A pointer to input masked polynomial
 * @return void
 */
void masked_pasta_soft_sbox_feistel(masked_poly *B, const masked_poly *A) {
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
            B->share[j].coeffs[i] = MOD_Q_ADD(A->share[j].coeffs[i], A_squared.share[j].coeffs[i - 1]);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Calculate next row for masked matrix multiplication (software)
 * @description Generates the next row for the dynamic matrix in masked pasta_matmul: C[0] = A[0] * B[N-1]; C[i] = A[i] * B[N-1] + B[i-1] for i >= 1. Uses pure software arithmetic. B[N] is used as the scalar bound. The input A is an unmasked polynomial (the random generator), while C and B are raw coefficient arrays
 * @param C pointer to output coefficient array (length N)
 * @param B pointer to previous row coefficient array (length N+1; B[N] is the scalar)
 * @param A pointer to unmasked polynomial (first row/generator)
 * @return void
 */
void masked_pasta_soft_calculate_row(int32_t *C, const int32_t *B, const poly *A) {
    pasta_soft_calculate_row(C, B, A);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Masked matrix-vector multiplication for PASTA (software)
 * @description Computes new_state = M * state where M is generated row-by-row, with operations performed directly on masked shares. For each row i and each share j: new_state.share[j][i] = sum_k (M[i][k] * state.share[j][k]). The matrix is generated using an unmasked random polynomial (rand), and the linear structure of pasta_matmul allows applying it independently to each masked share
 * @param new_state pointer to output masked polynomial
 * @param state pointer to input masked polynomial
 * @param nonce nonce for SHAKE128 seed
 * @param block_ctr block counter for SHAKE128 seed
 * @param poly_ctr polynomial counter for SHAKE128 seed
 * @return void
 */
void masked_pasta_soft_matmul(masked_poly *new_state, const masked_poly *state, uint64_t nonce, uint64_t block_ctr, uint8_t poly_ctr) {
    poly rand;
    size_t j;
    size_t i;
    size_t k;
    size_t allow_zero;
    int32_t curr_row[N << 1];
    uint64_t sum;
    int32_t rand_last;
    uint64_t prod;

    allow_zero = 0;
    // Generate random vector (no zeros)
    pasta_soft_poly_uniform(&rand, nonce, block_ctr, poly_ctr, allow_zero);

    // Initialize first row: curr_row[N..2N-1] = rand, curr_row[0..N-1] = rand * rand[N-1]
    rand_last = rand.coeffs[N - 1];
    for (i = 0; i < N; i++) {
        curr_row[i]     = 0;
        curr_row[N + i] = rand.coeffs[i];
    }

    // For each row in the matrix
    for (i = 0; i < N; i++) {
        for (j = 0; j < MASKING_N; j++) {
            // Pointwise multiply state.share[j] with curr_row (shifted)
            // Then sum to get new_state.share[j][i]
            sum = 0;
            for (k = 0; k < N; k++) {
                prod = (uint64_t)state->share[j].coeffs[k] * curr_row[N - i + k];
                sum = sum + prod;
            }
            new_state->share[j].coeffs[i] = MOD_Q_ACC(sum);
        }

        // Calculate next row if not last iteration
        if (i != N - 1)
            masked_pasta_soft_calculate_row(curr_row + N - i - 1, curr_row + N - i - 1, &rand);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Masked mixing: r = 2*a + b (software)
 * @description Computes r = 2*a + b for each coefficient with masking
 * @param r pointer to output masked polynomial
 * @param a pointer to first input masked polynomial
 * @param b pointer to second input masked polynomial
 * @return void
 */
void masked_poly_soft_mix_2a_b(masked_poly *r, const masked_poly *a, const masked_poly *b) {
    size_t i, j;
    uint64_t prod;
    uint32_t prod_reduced;
    for (j = 0; j < MASKING_N; j++) {
        for (i = 0; i < N; i++) {
            prod = 2 * (uint64_t)a->share[j].coeffs[i];
            prod_reduced = MOD_Q(prod);
            r->share[j].coeffs[i] = MOD_Q_ADD(prod_reduced, b->share[j].coeffs[i]);
        }
    }
}

/**
 * @brief Masked PASTA round function (software)
 * @description Performs one masked PASTA cipher round: 1. Masked matrix multiplication; 2. Add round constants; 3. Masked mixing (2*A + B, A + 2*B); 4. Masked S-box operations
 * @param C pointer to output masked polynomial for state 1
 * @param D pointer to output masked polynomial for state 2
 * @param A pointer to input masked polynomial for state 1
 * @param B pointer to input masked polynomial for state 2
 * @param nonce nonce for SHAKE128 seed
 * @param block_ctr block counter for SHAKE128 seed
 * @param r round number (0-indexed)
 * @return void
 */
void masked_pasta_soft_round(masked_poly *C, masked_poly *D, const masked_poly *A, const masked_poly *B, uint64_t nonce, uint64_t block_ctr, size_t r) {
    uint8_t poly_ctr;
    poly rand;
    masked_poly m_temp1, m_temp2, m_temp3;
    int allow_zero;


    poly_ctr = (r << 2);
    masked_pasta_soft_matmul(&m_temp1, A, nonce, block_ctr, poly_ctr);
    poly_ctr++;
    masked_pasta_soft_matmul(&m_temp2, B, nonce, block_ctr, poly_ctr);
    poly_ctr++;

    allow_zero = 1;
    pasta_soft_poly_uniform(&rand, nonce, block_ctr, poly_ctr, allow_zero);
    pasta_soft_poly_pointwise_add(&m_temp1.share[0], &m_temp1.share[0], &rand);
    poly_ctr++;

    pasta_soft_poly_uniform(&rand, nonce, block_ctr, poly_ctr, allow_zero);
    pasta_soft_poly_pointwise_add(&m_temp2.share[0], &m_temp2.share[0], &rand);
    poly_ctr++;

    if (r == PASTA_R) {
        masked_poly_soft_mix_2a_b(C, &m_temp1, &m_temp2);
    } else if (r == PASTA_R - 1) {
        masked_poly_soft_mix_2a_b(&m_temp3, &m_temp1, &m_temp2);
        masked_poly_soft_mix_2a_b(&m_temp1, &m_temp2, &m_temp1);

        masked_pasta_soft_sbox_cube(D, &m_temp1);
        masked_pasta_soft_sbox_cube(C, &m_temp3);
    } else {
        // Regular round: apply mixing then Feistel S-box
        masked_poly_soft_mix_2a_b(&m_temp3, &m_temp1, &m_temp2);
        masked_poly_soft_mix_2a_b(&m_temp1, &m_temp2, &m_temp1);

        masked_pasta_soft_sbox_feistel(D, &m_temp1);
        masked_pasta_soft_sbox_feistel(C, &m_temp3);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Masked PASTA encrypt one block (software)
 * @description Encrypts a plaintext block using masked PASTA cipher: 1. Initializes unmasked key states from key array; 2. Masks both states into two-share form; 3. Runs PASTA_R+1 masked rounds; 4. Adds plaintext to masked state (adds to share[0] only); 5. Unmasks final state to get ciphertext
 * @param ciphertext pointer to output polynomial
 * @param plaintext pointer to input plaintext polynomial
 * @param key pointer to key array (size 2*N coefficients)
 * @param nonce 8-byte nonce value
 * @return void
 */
void masked_pasta_soft_encrypt(poly *ciphertext, const poly *plaintext, const int32_t *key, uint64_t nonce) {
    size_t r;
    size_t i;
    uint64_t block_ctr;
    poly state1, state2;
    masked_poly m_state1;
    masked_poly m_state2;

    block_ctr = 0x0;

    // Initialize states from key
    for (i = 0; i < N; i++) {
        state1.coeffs[i] = key[i];
        state2.coeffs[i] = key[N + i];
    }

    // Mask state1 and state2
    masked_poly_soft_mask(&m_state1, &state1);
    masked_poly_soft_mask(&m_state2, &state2);

    // Run PASTA_R+1 rounds
    for (r = 0; r < PASTA_R + 1; r++) {
        masked_pasta_soft_round(&m_state1, &m_state2, &m_state1, &m_state2, nonce, block_ctr, r);
    }

    // Add plaintext to state (add to share[0] only)
    pasta_soft_poly_pointwise_add(&m_state1.share[0], &m_state1.share[0], plaintext);

    masked_poly_soft_unmask(ciphertext, &m_state1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
