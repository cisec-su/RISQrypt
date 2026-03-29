#include <stdint.h>
#include <stddef.h>
#include "params.h"
#include "pasta_soft.h"
#include "ntt_lite.h"
#include "fips202.h"
#include "util.h"


/**
 * @brief PASTA S-box cube layer (pure software)
 * @description Computes B[i] = A[i]^3 mod Q for all i using pure software arithmetic with 64-bit intermediate values to avoid overflow
 * @param B pointer to output polynomial
 * @param A pointer to input polynomial
 * @return void
 */
void pasta_soft_sbox_cube(poly *B, const poly *A) {
    size_t el;
    uint32_t square;
    uint32_t cube;
    for (el = 0; el < N; el++) {
        square = MOD_Q_MULT(A->coeffs[el], A->coeffs[el]);
        cube = MOD_Q_MULT(square, A->coeffs[el]);
        B->coeffs[el] = cube;
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief PASTA S-box Feistel layer (pure software)
 * @description Computes the Feistel transformation: B[0] = A[0], B[i] = A[i] + A[i-1]^2 mod Q using pure software arithmetic with 64-bit intermediate values
 * @param B pointer to output polynomial
 * @param A pointer to input polynomial
 * @return void
 */
void pasta_soft_sbox_feistel(poly *B, const poly *A) {
    size_t el;
    uint32_t sq_reduced;
    B->coeffs[0] = A->coeffs[0];
    for (el = 1; el < N; el++) {
        sq_reduced = MOD_Q_MULT(A->coeffs[el - 1], A->coeffs[el - 1]);
        B->coeffs[el] = MOD_Q_ADD(sq_reduced, A->coeffs[el]);
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Calculate next row for matrix multiplication (pure software)
 * @description Generates the next row of the PASTA matrix: C[0] = A[0] * B[N-1] mod Q; C[i] = A[i] * B[N-1] + B[i-1] mod Q for i >= 1. Uses 64-bit arithmetic to safely handle intermediate results. B[N] is used as the scalar bound (currently unused in software version)
 * @param C pointer to output coefficient array (length N)
 * @param B pointer to previous row array (length N+1; B[N] is the scalar bound)
 * @param A pointer to first row polynomial
 * @return void
 */
void pasta_soft_calculate_row(int32_t *C, const int32_t *B, const poly *A) {
    size_t i;
    int32_t b_last;
    uint32_t prod_reduced;

    b_last = B[N];

    for (i = 0; i < N; i++) {
        prod_reduced = MOD_Q_MULT((uint32_t)A->coeffs[i], (uint32_t)b_last);
        C[i] = MOD_Q_ADD(prod_reduced, B[i]);
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief PASTA linear mixing layer (pure software)
 * @description Applies the mixing transformation using pure software: B_left[i] = 2*A_left[i] + A_right[i] mod Q; B_right[i] = A_left[i] + 2*A_right[i] mod Q. Uses 64-bit arithmetic for safe intermediate computation
 * @param B_left pointer to output left polynomial
 * @param B_right pointer to output right polynomial
 * @param A_left pointer to input left polynomial
 * @param A_right pointer to input right polynomial
 * @return void
 */
void pasta_soft_mix(poly *B_left, poly *B_right, const poly *A_left, const poly *A_right) {
    size_t i;
    uint32_t left_reduced;
    uint32_t right_reduced;

    for (i = 0; i < N; i++) {
        // B_left[i] = 2*A_left[i] + A_right[i] mod Q
        left_reduced = MOD_Q_MULT((uint32_t)A_left->coeffs[i], 2);
        B_left->coeffs[i] = MOD_Q_ADD(left_reduced, A_right->coeffs[i]);

        // B_right[i] = A_left[i] + 2*A_right[i] mod Q
        right_reduced = MOD_Q_MULT((uint32_t)A_right->coeffs[i], 2);
        B_right->coeffs[i] = MOD_Q_ADD(A_left->coeffs[i], right_reduced);
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Generate uniformly random polynomial (pure software)
 * @description Generates random polynomial coefficients in [0, Q-1] using SHAKE128 with rejection sampling: 1. Constructs seed from nonce || block_ctr || poly_ctr (17 bytes); 2. Calls shake128 to generate random bytes; 3. Reads 64-bit little-endian values, extracts lower 17 bits; 4. Performs rejection sampling: rejects candidates >= Q; 5. If allow_zero=0, also rejects zero coefficients
 * @param p pointer to output polynomial
 * @param nonce 8-byte nonce value
 * @param block_ctr 8-byte block counter
 * @param poly_ctr 1-byte polynomial counter
 * @param allow_zero if 0: reject zero coefficients; if 1: allow all values in [0, Q-1]
 * @return void
 */
void pasta_soft_poly_uniform(poly *p, uint64_t nonce, uint64_t block_ctr, uint8_t poly_ctr, int allow_zero) {
    size_t k;
    size_t pos;
    uint8_t seed[17];
    uint32_t candidate;
    uint64_t val;
    uint8_t buf[SHAKE128_RATE];
    keccak_state state;
    size_t i;

    /* Build seed = nonce || block_ctr || poly_ctr */
    for (k = 0; k < 8; k++) {
        seed[k]     = (nonce >> (8 * k)) & 0xFF;
        seed[8 + k] = (block_ctr >> (8 * k)) & 0xFF;
    }
    seed[16] = poly_ctr;

    shake128_absorb(&state, seed, 17);

    i = 0;
    while (i < N) {
        shake128_squeezeblocks(buf, 1, &state);
        pos = 0;
        while (i < N && pos + 8 <= SHAKE128_RATE) {
            val =
                  ((uint64_t)buf[pos + 0]      )
                | ((uint64_t)buf[pos + 1] <<  8)
                | ((uint64_t)buf[pos + 2] << 16)
                | ((uint64_t)buf[pos + 3] << 24)
                | ((uint64_t)buf[pos + 4] << 32)
                | ((uint64_t)buf[pos + 5] << 40)
                | ((uint64_t)buf[pos + 6] << 48)
                | ((uint64_t)buf[pos + 7] << 56);
            pos += 8;
            candidate = val & 0x1FFFF;
            if (candidate >= Q) continue;
            if (!allow_zero && candidate == 0) continue;
            p->coeffs[i++] = candidate;
        }
    }
}

/**
 * @brief Matrix-vector multiplication for PASTA (pure software)
 * @description Computes new_state = M * state where M is generated dynamically using pasta_soft_calculate_row and a random polynomial (rand) derived from the nonce/block_ctr/poly_ctr seed. The first row is rand, and subsequent rows are computed using pasta_soft_calculate_row. Uses 64-bit arithmetic for safe intermediate computation
 * @param new_state pointer to output polynomial
 * @param state pointer to input state vector polynomial
 * @param nonce nonce for SHAKE128 seed
 * @param block_ctr block counter for SHAKE128 seed
 * @param poly_ctr polynomial counter for SHAKE128 seed
 * @return void
 */
void pasta_soft_matmul(poly *new_state, const poly *state, uint64_t nonce, uint64_t block_ctr, uint8_t poly_ctr) {
    poly rand;
    int32_t curr_row[N << 1];
    size_t i;
    size_t j;
    uint64_t acc;
    uint64_t mult;

    pasta_soft_poly_uniform(&rand, nonce, block_ctr, poly_ctr, 0);

    for (i = 0; i < N; i++) {
        curr_row[i]     = 0;
        curr_row[N + i] = rand.coeffs[i];
    }

    for (i = 0; i < N; i++) {
        acc = 0;
        for (j = 0; j < N; j++) {
            mult = ((uint64_t)curr_row[N - i + j] * state->coeffs[j]);
            acc  = (acc + mult);
        }
        new_state->coeffs[i] = MOD_Q_ACC(acc);

        if (i != N - 1)
            pasta_soft_calculate_row(curr_row + N - i - 1, curr_row + N - i - 1, &rand);
    }
}

/**
 * @brief PASTA round function (pure software)
 * @description Performs one PASTA cipher round using pure software: 1. Computes temp1 = pasta_soft_matmul(A), temp2 = pasta_soft_matmul(B); 2. Generates random constants RC1, RC2 and adds to temp1, temp2; 3. For final round (r == PASTA_R): applies cube to temp1 and adds temp2; For penultimate round (r == PASTA_R-1): applies mixing then cube to both; For other rounds: applies mixing then Feistel to both. All operations use 64-bit arithmetic for safe intermediate computation
 * @param C pointer to output polynomial for state 1
 * @param D pointer to output polynomial for state 2
 * @param A pointer to input polynomial for state 1
 * @param B pointer to input polynomial for state 2
 * @param nonce nonce for SHAKE128 seed
 * @param block_ctr block counter for SHAKE128 seed
 * @param r round number (0-indexed)
 * @return void
 */
void pasta_soft_round(poly *C, poly *D, const poly *A, const poly *B, uint64_t nonce, uint64_t block_ctr, int r) {
    poly temp1, temp2;  // After pasta_matmul
    poly temp3, temp4;  // After add_rc
    poly temp5, temp6;  // After pasta_mix
    uint8_t poly_ctr;
    int allow_zero;

    poly_ctr = r << 2;
    pasta_soft_matmul(&temp1, A, nonce, block_ctr, poly_ctr);
    poly_ctr = poly_ctr + 1;
    pasta_soft_matmul(&temp2, B, nonce, block_ctr, poly_ctr);
    poly_ctr = poly_ctr + 1;

    allow_zero = 1;
    pasta_soft_poly_uniform(&temp3, nonce, block_ctr, poly_ctr, allow_zero);
    poly_ctr = poly_ctr + 1;
    pasta_soft_poly_pointwise_add(&temp3, &temp3, &temp1);

    pasta_soft_poly_uniform(&temp4, nonce, block_ctr, poly_ctr, allow_zero);
    poly_ctr = poly_ctr + 1;
    pasta_soft_poly_pointwise_add(&temp4, &temp4, &temp2);

    if (r == PASTA_R) {
        pasta_soft_mix(C, &temp6, &temp3, &temp4);

    } else if (r == PASTA_R - 1) {
        pasta_soft_mix(&temp5, &temp6, &temp3, &temp4);
        pasta_soft_sbox_cube(C, &temp5);
        pasta_soft_sbox_cube(D, &temp6);
    } else {
        pasta_soft_mix(&temp5, &temp6, &temp3, &temp4);
        pasta_soft_sbox_feistel(C, &temp5);
        pasta_soft_sbox_feistel(D, &temp6);
    }
}
/**
 * @brief PASTA encrypt one block (pure software)
 * @description Encrypts a plaintext block using the PASTA cipher: 1. Initializes two state polynomials from the key (first N coefficients in state1, second N in state2); 2. Runs PASTA_R+1 rounds; 3. Adds plaintext to final keystream state1. Ciphertext = plaintext + PASTA(key, nonce) mod Q
 * @param ciphertext pointer to output polynomial
 * @param plaintext pointer to input plaintext polynomial
 * @param key pointer to key array (size 2*N coefficients)
 * @param nonce 8-byte nonce value
 * @return void
 */
void pasta_soft_encrypt(poly *ciphertext, const poly *plaintext, const int32_t *key, uint64_t nonce) {
    poly state1, state2;
    poly new_state1, new_state2;
    size_t r;
    size_t i;
    uint64_t block_ctr;

    block_ctr = 0;

    // Initialize states from key
    for (i = 0; i < N; i++) {
        state1.coeffs[i] = key[i];
        state2.coeffs[i] = key[N + i];
    }

    // Run PASTA_R+1 rounds
    for (r = 0; r < (size_t)PASTA_R + 1; r++) {
        pasta_soft_round(&new_state1, &new_state2, &state1, &state2, nonce, block_ctr, r);
        state1 = new_state1;
        state2 = new_state2;
    }

    // Add plaintext to state1
    pasta_soft_poly_pointwise_add(ciphertext, &state1, plaintext);
}


/**
 * @brief Pointwise addition of polynomials (pure software)
 * @description Performs coefficient-wise addition: C[i] = (A[i] + B[i]) mod Q. All arithmetic uses 64-bit intermediate values for safe computation
 * @param C pointer to output polynomial
 * @param A pointer to first input polynomial
 * @param B pointer to second input polynomial
 * @return void
 */
void pasta_soft_poly_pointwise_add(poly *C, const poly *A, const poly *B) {
    size_t i;
    for(i = 0; i < N; i++) {
        C->coeffs[i] = MOD_Q_ADD(A->coeffs[i], B->coeffs[i]);
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
void pasta_soft_poly_pointwise_sub(poly *C, const poly *A, const poly *B) {
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
void pasta_soft_poly_pointwise_mult(poly *C, const poly *A, const poly *B) {
    size_t i;
    for(i = 0; i < N; i++) {
        C->coeffs[i] = MOD_Q_MULT((uint32_t)A->coeffs[i], (uint32_t)B->coeffs[i]);
    }
}
/**
 * @brief Generate constant key, plaintext, nonce and block counter values for tests
 * @description Fills provided buffers with fixed constant values suitable for testing.
 * @param key[out]        buffer of 2*N coefficients (int32_t) to receive the key
 * @param nonce[out]      pointer to receive nonce value (may be NULL)
 * @param block_ctr[out]  pointer to receive block counter (may be NULL)
 * @return void
 */
void pasta_soft_key_gen(int32_t *key, uint64_t *nonce, uint64_t *block_ctr){
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
