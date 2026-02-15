#include <stdint.h>
#include <stddef.h>
#include "params.h"
#include "ntt_lite.h"
#include "fips202.h"
#include "util.h"

/**
 * @brief PASTA S-box Cube layer: B[i] = A[i]^3 mod Q
 *        Pure software implementation
 *
 * @param B Output polynomial
 * @param A Input polynomial
 */
void sbox_cube_soft(poly *B, const poly *A) {
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
 * @brief PASTA S-box Feistel layer: B[0]=A[0], B[i]=A[i]+A[i-1]^2 mod Q
 *        Pure software implementation
 *
 * @param B Output polynomial
 * @param A Input polynomial
 */
void sbox_feistel_soft(poly *B, const poly *A) {
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
 * @brief Calculate row operation: C[0] = A[0]*B[N-1], C[i] = A[i]*B[N-1] + B[i-1] for i >= 1
 *        Pure software implementation
 *
 * @param C Output polynomial
 * @param B Input polynomial (previous row)
 * @param A Input polynomial (first row)
 */
void calculate_row_soft(poly *C, const poly *B, const poly *A) {
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
 * @brief PASTA linear mixing layer: B_left = 2*A_left + A_right, B_right = A_left + 2*A_right
 *        Pure software implementation
 *
 * @param B_left  Output left polynomial
 * @param B_right Output right polynomial
 * @param A_left  Input left polynomial
 * @param A_right Input right polynomial
 */
void mix_soft(poly *B_left, poly *B_right, const poly *A_left, const poly *A_right) {
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
 * @brief Performs element-wise modular multiplication: c[i] = (a[i] * b[i]) mod Q
 *        Pure software implementation
 *
 * @param c Output polynomial
 * @param a First input polynomial
 * @param b Second input polynomial
 */
void poly_pointwise_soft(poly *c, const poly *a, const poly *b) {
    size_t i;
    uint64_t product;
    for (i = 0; i < N; i++) {
        product = (uint64_t)a->coeffs[i] * (uint64_t)b->coeffs[i];
        c->coeffs[i] = (int32_t)(product % Q);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Generate a random polynomial using SHAKE128
 *        Pure software implementation using fips202
 *
 * @param p          Output polynomial
 * @param nonce      Nonce for SHAKE128 seed
 * @param block_ctr  Block counter for SHAKE128 seed
 * @param poly_ctr   Polynomial counter for SHAKE128 seed
 * @param allow_zero If 0: 16-bit extraction + 1, if 1: 17-bit rejection sampling
 */
void poly_uniform_soft(poly *p, uint64_t nonce, uint64_t block_ctr, uint8_t poly_ctr, int allow_zero) {
    int i;
    size_t pos;
    uint8_t seed[17];
    uint32_t BUFSIZE = 8*128*4;
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
 * @brief Matrix-vector multiplication for PASTA
 *        new_state = M * state, where M is generated row-by-row using calculate_row
 *        Pure software implementation
 *
 * @param new_state Output polynomial (result of matrix-vector multiplication)
 * @param state     Input polynomial (vector to multiply)
 * @param nonce     Nonce for SHAKE128 seed
 * @param block_ctr Block counter for SHAKE128 seed
 * @param poly_ctr  Polynomial counter for SHAKE128 seed
 */
void matmul_soft(poly *new_state, const poly *state, uint64_t nonce, uint64_t block_ctr, uint8_t poly_ctr) {
    poly rand;
    poly curr_row;
    poly next_row;
    size_t i;
    size_t j;
    uint64_t acc;
    uint64_t mult;
    int allow_zero;

    allow_zero = 0;
    poly_uniform_soft(&rand,nonce,block_ctr,poly_ctr, allow_zero);

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
            calculate_row_soft(&next_row, &curr_row, &rand);
            // Update curr_row for next iteration
            for (size_t k = 0; k < N; k++) {
                curr_row.coeffs[k] = next_row.coeffs[k];
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief PASTA round function (pure software)
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
void pasta_round_soft(poly *C, poly *D, const poly *A, const poly *B, uint64_t nonce, uint64_t block_ctr, int r) {
    poly temp1, temp2;  // After matmul
    poly temp3, temp4;  // After add_rc
    poly temp5, temp6;  // After mix
    uint8_t poly_ctr;
    uint8_t i;
    int allow_zero;

    poly_ctr = r << 2; 
    // Step 1: Matrix multiplication on both states
    matmul_soft(&temp1, A, nonce, block_ctr, poly_ctr);
    poly_ctr = poly_ctr + 1;
    matmul_soft(&temp2, B, nonce, block_ctr, poly_ctr);
    poly_ctr = poly_ctr + 1;

    allow_zero = 1;
    poly_uniform_soft(&temp3,nonce,block_ctr,poly_ctr,allow_zero);
    poly_ctr = poly_ctr + 1;

    for(i=0;i<N;i++)
        temp3.coeffs[i] = (temp3.coeffs[i] + temp1.coeffs[i]) % Q;
    
    poly_uniform_soft(&temp4,nonce,block_ctr,poly_ctr,allow_zero);
    poly_ctr = poly_ctr + 1;
    for(i=0;i<N;i++)
        temp4.coeffs[i] = (temp4.coeffs[i] + temp2.coeffs[i]) % Q;
    // Step 3: Mix the two states
    mix_soft(&temp5, &temp6, &temp3, &temp4);

    // Step 4: Apply S-box (cube for last round, feistel otherwise)
    if (r == PASTA_R - 1) {
        sbox_cube_soft(C, &temp5);
        sbox_cube_soft(D, &temp6);
    } else {
        sbox_feistel_soft(C, &temp5);
        sbox_feistel_soft(D, &temp6);
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief PASTA encrypt one block (pure software)
 *
 * Encrypts plaintext using PASTA cipher: keystream = PASTA(key, nonce) then
 * ciphertext = plaintext + keystream mod Q
 *
 * @param ciphertext Output polynomial
 * @param plaintext  Input polynomial
 * @param key        Input key array (size 2*N = 256 elements)
 * @param nonce      Nonce value
 */
void pasta_encrypt_one_block_soft(poly *ciphertext, const poly *plaintext, const int32_t *key, uint64_t nonce) {
    poly state1, state2;
    poly temp1, temp2;
    poly new_state1, new_state2;
    poly final_state2;
    size_t i;
    uint8_t poly_ctr;
    uint64_t block_ctr;
    int allow_zero;
    int r;

    block_ctr = 0;

    // Initialize states from key
    for (i = 0; i < N; i++) {
        state1.coeffs[i] = key[i];
        state2.coeffs[i] = key[N + i];
    }

    // Run PASTA_R rounds
    for (r = 0; r < PASTA_R; r++) {
        pasta_round_soft(&new_state1, &new_state2, &state1, &state2, nonce, block_ctr, r);
        state1 = new_state1;
        state2 = new_state2;
    }
    poly_ctr = PASTA_R << 2;

    // Final matmul on both states
    matmul_soft(&new_state1, &state1, nonce, block_ctr, poly_ctr);

    poly_ctr = poly_ctr + 1;
    matmul_soft(&new_state2, &state2, nonce, block_ctr, poly_ctr);
    poly_ctr = poly_ctr + 1;

    //add_rc_soft(&state1, &temp1);
    allow_zero = 1;
    poly_uniform_soft(&temp1,nonce, block_ctr, poly_ctr,allow_zero);
    
    poly_ctr = poly_ctr + 1;
    for(i=0;i<N;i++)
        state1.coeffs[i] = (new_state1.coeffs[i] + temp1.coeffs[i]) % Q;
    
    //add_rc_soft(&state2, &temp2);
    poly_uniform_soft(&temp2,nonce, block_ctr, poly_ctr,allow_zero);
    poly_ctr = poly_ctr + 1;
    for(i=0;i<N;i++)
        state2.coeffs[i] = (new_state2.coeffs[i] + temp2.coeffs[i]) % Q;
    // Step 3: Mix the two states

    // Final mix (state1 becomes the keystream)
    mix_soft(&temp1, &final_state2, &state1, &state2);

    //add_rc_soft(&state2, &temp2);
    for(i=0;i<N;i++)
        ciphertext->coeffs[i] = (temp1.coeffs[i] + plaintext->coeffs[i]) % Q;
}



