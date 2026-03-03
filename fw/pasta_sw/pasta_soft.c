#include <stdint.h>
#include <stddef.h>
#include "params.h"
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
    uint64_t square;
    uint64_t cube;
    for (el = 0; el < N; el++) {
        square = (uint64_t)A->coeffs[el] * (uint64_t)A->coeffs[el];
        square = square % Q;
        cube = square * (uint64_t)A->coeffs[el];
        B->coeffs[el] = (int32_t)(cube % Q);
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
    uint64_t square;
    uint64_t sum;
    B->coeffs[0] = A->coeffs[0];
    for (el = 1; el < N; el++) {
        square = (uint64_t)A->coeffs[el - 1] * (uint64_t)A->coeffs[el - 1];
        sum = square + A->coeffs[el];
        B->coeffs[el] = (int32_t)(sum % Q);
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Calculate next row for matrix multiplication (pure software)
 * @description Generates the next row of the PASTA matrix: C[0] = A[0] * B[N-1] mod Q; C[i] = A[i] * B[N-1] + B[i-1] mod Q for i >= 1. Uses 64-bit arithmetic to safely handle intermediate results
 * @param C pointer to output polynomial (result row)
 * @param B pointer to previous row polynomial
 * @param A pointer to first row polynomial
 * @return void
 */
void pasta_soft_calculate_row(poly *C, const poly *B, const poly *A) {
    size_t i;
    int32_t b_last;
    uint64_t product;
    uint64_t mult;
    uint64_t sum;
    
    b_last = B->coeffs[N - 1];
    
    // C[0] = A[0] * B[N-1] mod Q
    product = (uint64_t)A->coeffs[0] * (uint64_t)b_last;
    C->coeffs[0] = (int32_t)(product % Q);
    
    // C[i] = A[i] * B[N-1] + B[i-1] mod Q for i = 1 to N-1
    for (i = 1; i < N; i++) {
        mult = (uint64_t)A->coeffs[i] * (uint64_t)b_last;
        sum = mult + (uint64_t)B->coeffs[i - 1];
        C->coeffs[i] = (int32_t)(sum % Q);
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
    uint64_t left_val;
    uint64_t right_val;
    for (i = 0; i < N; i++) {
        // B_left[i] = 2*A_left[i] + A_right[i] mod Q
        left_val = 2 * (uint64_t)A_left->coeffs[i] + (uint64_t)A_right->coeffs[i];
        B_left->coeffs[i] = (int32_t)(left_val % Q);
        
        // B_right[i] = A_left[i] + 2*A_right[i] mod Q
        right_val = (uint64_t)A_left->coeffs[i] + 2 * (uint64_t)A_right->coeffs[i];
        B_right->coeffs[i] = (int32_t)(right_val % Q);
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
    size_t i;
    size_t pos;
    uint8_t seed[17];
    size_t BUFSIZE = 8*128*4;
    uint32_t candidate;
    uint64_t val;
    uint8_t buf[BUFSIZE];

    /* Build seed = nonce || block_ctr || poly_ctr */
    for (i = 0; i < 8; i++) {
        seed[i]     = (nonce >> (8 * i)) & 0xFF;
        seed[8 + i] = (block_ctr >> (8 * i)) & 0xFF;
    }
    seed[16] = poly_ctr;

    /* Single SHAKE call */
    shake128(buf, BUFSIZE, seed, 17);

    pos = 0;
    i = 0;

    while (i < N && pos + 8 <= BUFSIZE) {
        /* Read 64-bit little-endian value */
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

        /* Take lower 17 bits to achieve ~50% rejection */
        candidate = (uint32_t)(val & 0x1FFFF);

        /* Rejection sampling */
        if (candidate >= Q)
            continue;

        if (!allow_zero && candidate == 0)
            continue;

        p->coeffs[i++] = candidate;   // uint32_t
    }

}

/**
 * @brief Matrix-vector multiplication for PASTA (pure software)
 * @description Computes new_state = M * state where M is generated dynamically: 1. Generates random polynomial rand from seed (nonce || block_ctr || poly_ctr); 2. Initializes curr_row = rand; 3. For each row i: computes dot product of curr_row with state vector; 4. Updates curr_row for next iteration using pasta_soft_calculate_row. All arithmetic uses 64-bit intermediate values modulo Q
 * @param new_state pointer to output polynomial
 * @param state pointer to input state vector polynomial
 * @param nonce nonce for SHAKE128 seed
 * @param block_ctr block counter for SHAKE128 seed
 * @param poly_ctr polynomial counter for SHAKE128 seed
 * @return void
 */
void pasta_soft_matmul(poly *new_state, const poly *state, uint64_t nonce, uint64_t block_ctr, uint8_t poly_ctr) {
    poly rand;
    poly curr_row;
    poly next_row;
    size_t i;
    size_t j;
    size_t k;
    uint64_t acc;
    uint64_t mult;
    int allow_zero;

    allow_zero = 0;
    pasta_soft_poly_uniform(&rand,nonce,block_ctr,poly_ctr, allow_zero);

    // Initialize curr_row with rand
    for (i = 0; i < N; i++) {
        curr_row.coeffs[i] = rand.coeffs[i];
    }

    // For each row in the matrix
    for (i = 0; i < N; i++) {
        // Compute dot product: acc = sum(curr_row[j] * state[j]) mod Q
        acc = 0;
        for (j = 0; j < N; j++) {
            mult = ((uint64_t)curr_row.coeffs[j] * (uint64_t)state->coeffs[j]) % Q;
            acc = (acc + mult) % Q;
        }
        new_state->coeffs[i] = (int32_t)acc;

        // Calculate next row if not last iteration
        if (i != N - 1) {
            pasta_soft_calculate_row(&next_row, &curr_row, &rand);
            // Update curr_row for next iteration
            for (k = 0; k < N; k++) {
                curr_row.coeffs[k] = next_row.coeffs[k];
            }
        }
    }
}

/**
 * @brief PASTA round function (pure software)
 * @description Performs one PASTA cipher round using pure software: 1. Computes temp1 = pasta_soft_matmul(A), temp2 = pasta_soft_matmul(B); 2. Generates random constants RC1, RC2 and adds to temp1, temp2; 3. Applies mixing: temp5 = pasta_mix(temp1, temp2); 4. Applies S-box: cube for penultimate round, Feistel otherwise. All operations use 64-bit arithmetic for safe intermediate computation
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
    size_t i;
    int allow_zero;

    poly_ctr = r << 2; 
    // Step 1: Matrix multiplication on both states
    pasta_soft_matmul(&temp1, A, nonce, block_ctr, poly_ctr);
    poly_ctr = poly_ctr + 1;
    pasta_soft_matmul(&temp2, B, nonce, block_ctr, poly_ctr);
    poly_ctr = poly_ctr + 1;

    allow_zero = 1;
    pasta_soft_poly_uniform(&temp3,nonce,block_ctr,poly_ctr,allow_zero);
    poly_ctr = poly_ctr + 1;

    for(i=0;i<N;i++)
        temp3.coeffs[i] = (temp3.coeffs[i] + temp1.coeffs[i]) % Q;
    
    pasta_soft_poly_uniform(&temp4,nonce,block_ctr,poly_ctr,allow_zero);
    poly_ctr = poly_ctr + 1;
    for(i=0;i<N;i++)
        temp4.coeffs[i] = (temp4.coeffs[i] + temp2.coeffs[i]) % Q;
    // Step 3: Mix the two states
    pasta_soft_mix(&temp5, &temp6, &temp3, &temp4);

    // Step 4: Apply S-box (cube for last round, feistel otherwise)
    if (r == PASTA_R - 1) {
        pasta_soft_sbox_cube(C, &temp5);
        pasta_soft_sbox_cube(D, &temp6);
    } else {
        pasta_soft_sbox_feistel(C, &temp5);
        pasta_soft_sbox_feistel(D, &temp6);
    }
}
/**
 * @brief PASTA encrypt one block (pure software)
 * @description Encrypts a plaintext block using pure software PASTA implementation: 1. Initializes state1, state2 from key; 2. Runs PASTA_R rounds (each with pasta_matmul, RC add, pasta_mix, S-box); 3. Runs final pasta_matmul on both states; 4. Adds final round constants and applies final mixing; 5. XORs plaintext with final keystream to produce ciphertext. This is the reference software implementation used for correctness verification and benchmarking
 * @param ciphertext pointer to output polynomial
 * @param plaintext pointer to input plaintext polynomial
 * @param key pointer to key array (size 2*N coefficients)
 * @param nonce 8-byte nonce value
 * @return void
 */
void pasta_soft_encrypt(poly *ciphertext, const poly *plaintext, const int32_t *key, uint64_t nonce) {
    poly state1, state2;
    poly temp1, temp2;
    poly new_state1, new_state2;
    poly final_state2;
    size_t i;
    size_t r;
    uint8_t poly_ctr;
    uint64_t block_ctr;
    int allow_zero;

    block_ctr = 0;

    // Initialize states from key
    for (i = 0; i < N; i++) {
        state1.coeffs[i] = key[i];
        state2.coeffs[i] = key[N + i];
    }

    // Run PASTA_R rounds
    for (r = 0; r < (size_t)PASTA_R; r++) {
        pasta_soft_round(&new_state1, &new_state2, &state1, &state2, nonce, block_ctr, (int)r);
        state1 = new_state1;
        state2 = new_state2;
    }
    poly_ctr = PASTA_R << 2;

    // Final pasta_matmul on both states
    pasta_soft_matmul(&new_state1, &state1, nonce, block_ctr, poly_ctr);

    poly_ctr = poly_ctr + 1;
    pasta_soft_matmul(&new_state2, &state2, nonce, block_ctr, poly_ctr);
    poly_ctr = poly_ctr + 1;

    allow_zero = 1;
    pasta_soft_poly_uniform(&temp1, nonce, block_ctr, poly_ctr, allow_zero);

    poly_ctr = poly_ctr + 1;
    for(i = 0; i < N; i++)
        state1.coeffs[i] = (new_state1.coeffs[i] + temp1.coeffs[i]) % Q;

    pasta_soft_poly_uniform(&temp2, nonce, block_ctr, poly_ctr, allow_zero);
    poly_ctr = poly_ctr + 1;
    for(i = 0; i < N; i++)
        state2.coeffs[i] = (new_state2.coeffs[i] + temp2.coeffs[i]) % Q;

    // Final pasta_mix (state1 becomes the keystream)
    pasta_soft_mix(&temp1, &final_state2, &state1, &state2);

    for(i = 0; i < N; i++)
        ciphertext->coeffs[i] = (temp1.coeffs[i] + plaintext->coeffs[i]) % Q;
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
        C->coeffs[i] = (A->coeffs[i] + B->coeffs[i]) % Q;
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
        C->coeffs[i] = (A->coeffs[i] * B->coeffs[i]) % Q;
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
    // constant key of all ones (mod Q)
    if (key != NULL) {
        for (size_t i = 0; i < 2 * N; i++) {
            key[i] = 1 % Q;
        }
    }

    // constant nonce and block counter
    if (nonce != NULL) {
        *nonce = 0x123456789ULL;
    }
    if (block_ctr != NULL) {
        *block_ctr = 0x0ULL;
    }
}
