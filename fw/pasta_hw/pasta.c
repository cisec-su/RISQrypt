#include <stdint.h>
#include <stddef.h>
#include "params.h"
#include "poly.h"
#include "ntt_lite.h"
#include "util.h"



/**
 * @brief Internal state for incremental 64-bit squeeze operations
 */
static uint64_t squeeze_counter = 0;


/**
 * @brief PASTA S-box Cube layer: B[i] = A[i]^3 mod Q
 * 
 * @param B Output polynomial
 * @param A Input polynomial
 */
void sbox_cube(poly *B, poly *A) {
    ntt_lite_pwm(NTT_LITE_OUTPUT_DIS, A->coeffs, A->coeffs);
    ntt_lite_pwm(B->coeffs, NTT_LITE_INPUT_DIS, A->coeffs);
}

void sbox_cube_soft(poly *B, const poly *A) {
    for (size_t el = 0; el < N; el++) {
        uint64_t square = (uint64_t)A->coeffs[el] * (uint64_t)A->coeffs[el];
        square = square % Q;
        uint64_t cube = square * (uint64_t)A->coeffs[el];
        B->coeffs[el] = (int32_t)(cube % Q);
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief PASTA S-box Feistel layer
 * 
 * @param B Output polynomial
 * @param A Input polynomial
 */
void sbox_feistel(poly *B, const poly *A) {
    // Feistel function implementation
    uint32_t C[N+1];  // Shifted version of A
    ntt_lite_set_bound(0);
    C[0] = 0x00000;
    //ntt_lite_mul_const(C,NTT_LITE_INPUT_DIS); //C[0] = 0x00000;
    ntt_lite_add_const(C + 1, A->coeffs); 
    ntt_lite_pwm(NTT_LITE_OUTPUT_DIS, C, C);  // C_square = C^2
    ntt_lite_add(B->coeffs, NTT_LITE_INPUT_DIS, A->coeffs); // B = A + C^2
}

void sbox_feistel_soft(poly *B, const poly *A) {
    B->coeffs[0] = A->coeffs[0];
    
    for (size_t el = 1; el < N; el++) {
        uint64_t square = (uint64_t)A->coeffs[el - 1] * (uint64_t)A->coeffs[el - 1];
        uint64_t sum = square + A->coeffs[el];
        B->coeffs[el] = (int32_t)(sum % Q);
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Calculate row operation: C[0] = A[0]*B[N-1], C[i] = A[i]*B[N-1] + B[i-1] for i >= 1
 * 
 * @param C Output polynomial
 * @param A First input polynomial
 * @param B Second input polynomial
 */
void calculate_row(uint32_t *C, const uint32_t *B, const poly *A) {
    ntt_lite_set_bound(B[N]);
    ntt_lite_mul_const(NTT_LITE_OUTPUT_DIS, A->coeffs);
    ntt_lite_add(C, NTT_LITE_INPUT_DIS, B);
}

void calculate_row_soft(poly *C, const poly *B, const poly *A) {
    int32_t b_last = B->coeffs[N - 1];
    
    // C[0] = A[0] * B[N-1] mod Q
    uint64_t product = (uint64_t)A->coeffs[0] * (uint64_t)b_last;
    C->coeffs[0] = (int32_t)(product % Q);
    
    // C[i] = A[i] * B[N-1] + B[i-1] mod Q for i = 1 to N-1
    for (size_t i = 1; i < N; i++) {
        uint64_t mult = (uint64_t)A->coeffs[i] * (uint64_t)b_last;
        uint64_t sum = mult + (uint64_t)B->coeffs[i - 1];
        C->coeffs[i] = (int32_t)(sum % Q);
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief PASTA linear mixing layer 
 * 
 * @param B_left Output left polynomial
 * @param B_right Output right polynomial
 * @param A_left Input left polynomial
 * @param A_right Input right polynomial
 */
void mix(poly *B_left, poly *B_right, const poly *A_left, const poly *A_right) {

    ntt_lite_set_bound(2);
    ntt_lite_mul_const(NTT_LITE_OUTPUT_DIS, A_left->coeffs);
    ntt_lite_add(B_left->coeffs, NTT_LITE_INPUT_DIS, A_right->coeffs);
    ntt_lite_mul_const(NTT_LITE_OUTPUT_DIS, A_right->coeffs); 
    ntt_lite_add(B_right->coeffs, NTT_LITE_INPUT_DIS, A_left->coeffs);

    // ----- B_left = 2*A_left + A_right -----
    //ntt_lite_add(NTT_LITE_OUTPUT_DIS,A_left->coeffs,A_left->coeffs);
    //ntt_lite_add(B_left->coeffs,NTT_LITE_INPUT_DIS,A_right->coeffs);
    //ntt_lite_add(NTT_LITE_OUTPUT_DIS,A_right->coeffs,A_right->coeffs);
    //ntt_lite_add(B_right->coeffs,NTT_LITE_INPUT_DIS,A_left->coeffs);

    //poly C;
    //ntt_lite_add(C.coeffs, A_left->coeffs, A_right->coeffs);
    //ntt_lite_add(B_left->coeffs, C.coeffs, A_left->coeffs);
    //ntt_lite_add(B_right->coeffs, C.coeffs, A_right->coeffs);
}

void mix_soft(poly *B_left, poly *B_right, const poly *A_left, const poly *A_right) {
    for (size_t i = 0; i < N; i++) {
        // B_left[i] = 2*A_left[i] + A_right[i] mod Q
        uint64_t left_val = 2 * (uint64_t)A_left->coeffs[i] + (uint64_t)A_right->coeffs[i];
        B_left->coeffs[i] = (int32_t)(left_val % Q);
        
        // B_right[i] = A_left[i] + 2*A_right[i] mod Q
        uint64_t right_val = (uint64_t)A_left->coeffs[i] + 2 * (uint64_t)A_right->coeffs[i];
        B_right->coeffs[i] = (int32_t)(right_val % Q);
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Performs element-wise modular multiplication: c[i] = (a[i] * b[i]) mod MODULUS
 * 
 * @param c Output poly
 * @param a First input poly
 * @param b Second input poly
 */
void poly_pointwise_soft(poly *c, const poly *a, const poly *b) {
    for (size_t i = 0; i < N; i++) {
        uint64_t product = (uint64_t)a->coeffs[i] * (uint64_t)b->coeffs[i];
        c->coeffs[i] = (int32_t)(product % Q);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



void reset_random_state(void) {
    squeeze_counter = 0;
}

/**
 * @brief Returns 64-bit values incrementally
 * 
 * @return uint64_t The next 64-bit value in sequence
 */
uint64_t squeeze_u64(void) {
    // This is a placeholder implementation that returns incrementing values
    // Replace with actual SHAKE256 or your preferred PRNG implementation
    uint64_t result = squeeze_counter;
    result = squeeze_counter;
    squeeze_counter++;
    
    // Optional: Add some mixing to make it more random-looking
    //result ^= (result << 21);
    //result ^= (result >> 35);
    //result ^= (result << 4);
    return result;
}

/**
 * @brief Generate a random field element < Q (Pasta prime)
 *
 * Loops until finding an element within the field. Uses squeeze_u64()
 * to get random 64-bit values and masks them to fit within potential
 * field range before checking against Q.
 *
 * @param allow_zero If 0 (false), reject zero values; if non-zero (true), allow zero
 * @return uint32_t A valid field element in range [0, Q) or [1, Q) depending on allow_zero
 */
uint32_t generate_random_field_element(int allow_zero) {
    while (1) {
        // Get 64-bit random value and mask to potential field size
        uint64_t ele = squeeze_u64() & MODULUS_BIT_MASK;

        // Reject zero if not allowed
        if (!allow_zero && ele == 0) {
            continue;
        }

        // Accept if less than Q
        if (ele < Q) {
            return (uint32_t)ele;
        }
    }
}

/**
 * @brief Generate a random vector (polynomial) of field elements
 *
 * Fills the polynomial with N random field elements, each < Q.
 *
 * @param p Pointer to polynomial to fill with random elements
 * @param allow_zero If 0 (false), reject zero values; if non-zero (true), allow zero
 */
void get_random_vector(poly *p, int allow_zero) {
    for (size_t i = 0; i < N; i++) {
        p->coeffs[i] = (int32_t)generate_random_field_element(allow_zero);
    }
}

/**
 * @brief Matrix-vector multiplication for PASTA (software implementation)
 *
 * Performs matrix multiplication: new_state = M * state, where M is a
 * random matrix generated row-by-row using calculate_row. The first row
 * is a random vector, and each subsequent row is calculated from the
 * previous row and the first random vector.
 *
 * @param new_state Output polynomial (result of matrix-vector multiplication)
 * @param state Input polynomial (vector to multiply)
 */
void matmul(poly *new_state, const poly *state) {
    poly rand;
    uint32_t curr_row[N << 1];
    size_t i;

    // Generate random vector (no zeros)
    get_random_vector(&rand, 0);

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
void matmul_soft(poly *new_state, const poly *state) {
    poly rand;
    poly curr_row;

    // Generate random vector (no zeros)
    get_random_vector(&rand, 0);

    // Initialize curr_row with rand
    for (size_t i = 0; i < N; i++) {
        curr_row.coeffs[i] = rand.coeffs[i];
    }

    // For each row in the matrix
    for (size_t i = 0; i < N; i++) {

        // Compute dot product: acc = sum(curr_row[j] * state[j]) mod Q
        uint64_t acc = 0;
        for (size_t j = 0; j < N; j++) {
            uint64_t mult = ((uint64_t)curr_row.coeffs[j] * (uint64_t)state->coeffs[j]) % Q;
            acc = (acc + mult) % Q;
        }
        new_state->coeffs[i] = (int32_t)acc;

        // Calculate next row if not last iteration
        if (i != N - 1) {
            poly next_row;
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
 * @brief Add random constants to state
 * 
 * Adds a random field element to each coefficient of the input polynomial
 * and reduces modulo Q. Output is stored in B.
 * 
 * @param B Output polynomial
 * @param A Input polynomial
 */
void add_rc(poly *B, const poly *A) {
    poly rand;
    get_random_vector(&rand, 1);
    poly_add(B, A, &rand);
}

void add_rc_soft(poly *B, const poly *A) {
    for (size_t i = 0; i < N; i++) {
        uint32_t rc = generate_random_field_element(1);  // allow_zero = true
        uint64_t sum = (uint64_t)A->coeffs[i] + (uint64_t)rc;
        B->coeffs[i] = (int32_t)(sum % Q);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief PASTA round function  
 * 
 * Performs one round of PASTA cipher including:
 * - Matrix multiplication on both states
 * - Adding random constants to both states
 * - Mixing the states
 * - Applying S-box (cube for last round, feistel otherwise)
 * 
 * @param C Output polynomial for state 1
 * @param D Output polynomial for state 2
 * @param A Input polynomial for state 1
 * @param B Input polynomial for state 2
 * @param r Round number (0-indexed)
 */
void pasta_round(poly *C, poly *D, const poly *A, const poly *B, int r) {
    poly temp1, temp2;  // After matmul
    poly temp3, temp4;  // After add_rc
    poly temp5, temp6;  // After mix
    
    // Step 1: Matrix multiplication on both states
    matmul(&temp1, A);
    matmul(&temp2, B);
    
    // Step 2: Add random constants to both states
    poly rand;
    //add_rc(&temp3, &temp1);
    get_random_vector(&rand, 1);
    ntt_lite_add(temp3.coeffs, temp1.coeffs, rand.coeffs);

    //add_rc(&temp4, &temp2);
    get_random_vector(&rand, 1);
    ntt_lite_add(temp4.coeffs, temp2.coeffs, rand.coeffs);
    
    // Step 3: Mix the two states
    //mix(&temp5, &temp6, &temp3, &temp4);
    ntt_lite_set_bound(2);
    ntt_lite_mul_const(NTT_LITE_OUTPUT_DIS, temp3.coeffs);
    ntt_lite_add(temp5.coeffs, NTT_LITE_INPUT_DIS, temp4.coeffs);
    ntt_lite_mul_const(NTT_LITE_OUTPUT_DIS, temp4.coeffs); 
    ntt_lite_add(temp6.coeffs, NTT_LITE_INPUT_DIS, temp3.coeffs);
    
    // Step 4: Apply S-box (cube for last round, feistel otherwise)
    if (r == PASTA_R - 1) {
        //sbox_cube(C, &temp5);
        ntt_lite_pwm(NTT_LITE_OUTPUT_DIS, temp5.coeffs, temp5.coeffs);
        ntt_lite_pwm(C->coeffs, NTT_LITE_INPUT_DIS, temp5.coeffs);
        //sbox_cube(D, &temp6);
        ntt_lite_pwm(NTT_LITE_OUTPUT_DIS, temp6.coeffs, temp6.coeffs);
        ntt_lite_pwm(D->coeffs, NTT_LITE_INPUT_DIS, temp6.coeffs);
    } else {
        //sbox_feistel(C, &temp5);
        // Feistel function implementation
        uint32_t CC[N+1];  // Shifted version of A
        ntt_lite_set_bound(0);
        CC[0] = 0x00000;
        //ntt_lite_mul_const(C,NTT_LITE_INPUT_DIS); //C[0] = 0x00000;
        ntt_lite_add_const(CC + 1, temp5.coeffs); 
        ntt_lite_pwm(NTT_LITE_OUTPUT_DIS, CC, CC);  // C_square = C^2
        ntt_lite_add(C->coeffs, NTT_LITE_INPUT_DIS, temp5.coeffs); // B = A + C^2

        //sbox_feistel(D, &temp6);
        //uint32_t CC[N+1];  // Shifted version of A
        ntt_lite_set_bound(0);
        CC[0] = 0x00000;
        //ntt_lite_mul_const(C,NTT_LITE_INPUT_DIS); //C[0] = 0x00000;
        ntt_lite_add_const(CC + 1, temp6.coeffs); 
        ntt_lite_pwm(NTT_LITE_OUTPUT_DIS, CC, CC);  // C_square = C^2
        ntt_lite_add(D->coeffs, NTT_LITE_INPUT_DIS, temp6.coeffs); // B = A + C^2
    }
}

void pasta_round_soft(poly *C, poly *D, const poly *A, const poly *B, int r) {
    poly temp1, temp2;  // After matmul
    poly temp3, temp4;  // After add_rc
    poly temp5, temp6;  // After mix

    // Step 1: Matrix multiplication on both states
    matmul_soft(&temp1, A);
    matmul_soft(&temp2, B);

    // Step 2: Add random constants to both states
    add_rc_soft(&temp3, &temp1);
    add_rc_soft(&temp4, &temp2);

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
 * @brief Generate PASTA keystream 
 *
 * Generates a keystream from the key, nonce, and block counter using the PASTA cipher.
 * Performs PASTA_R rounds followed by final matmul, add_rc, and mix operations.
 * Uses hardware-accelerated functions.
 *
 * @param keystream Output polynomial containing the generated keystream
 * @param key Input key array (size 2*N = 256 elements)
 */
void gen_keystream(poly *keystream, const int32_t *key) {
    // Initialize random state (equivalent to init_shake)
    reset_random_state();

    poly state1, state2;
    poly final_state2;
    size_t i;
    int r;
    poly temp1, temp2;

    ntt_lite_set_bound(0);
    ntt_lite_add_const(state1.coeffs, key); 
    ntt_lite_add_const(state2.coeffs, key + N);


    // Run PASTA_R rounds
    for (r = 0; r < PASTA_R; r++) {
        pasta_round(&state1, &state2, &state1, &state2, r);
    }

    // Final matmul on both states
    matmul(&temp1, &state1);
    matmul(&temp2, &state2);

    poly rand;

    // Final add_rc on both states
    //add_rc(&state1, &temp1);
    //add_rc(&state2, &temp2);
    get_random_vector(&rand, 1);
    poly_add(&state1, &temp1, &rand);
    get_random_vector(&rand, 1);
    poly_add(&state2, &temp2, &rand);

    // Final mix (state1 becomes the keystream)
    //mix(keystream, &final_state2, &state1, &state2);
    ntt_lite_set_bound(2);
    ntt_lite_mul_const(NTT_LITE_OUTPUT_DIS, state1.coeffs);
    ntt_lite_add(keystream->coeffs, NTT_LITE_INPUT_DIS, state2.coeffs);
    //no need!.. ntt_lite_mul_const(NTT_LITE_OUTPUT_DIS, state2.coeffs); 
    //no need!.. ntt_lite_add(final_state2.coeffs, NTT_LITE_INPUT_DIS, state1.coeffs);

}

void gen_keystream_soft(poly *keystream, const int32_t *key, uint64_t nonce, uint64_t block_counter) {
    // Initialize random state (equivalent to init_shake)
    reset_random_state();

    poly state1, state2;

    // Initialize states from key
    for (size_t i = 0; i < N; i++) {
        state1.coeffs[i] = key[i];
        state2.coeffs[i] = key[N + i];
    }

    // Run PASTA_R rounds
    for (int r = 0; r < PASTA_R; r++) {
        poly new_state1, new_state2;
        pasta_round_soft(&new_state1, &new_state2, &state1, &state2, r);
        state1 = new_state1;
        state2 = new_state2;
    }

    // Final matmul on both states
    poly temp1, temp2;
    matmul_soft(&temp1, &state1);
    matmul_soft(&temp2, &state2);

    // Final add_rc on both states
    add_rc_soft(&state1, &temp1);
    add_rc_soft(&state2, &temp2);

    // Final mix (state1 becomes the keystream)
    poly final_state2;
    mix_soft(keystream, &final_state2, &state1, &state2);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief PASTA encrypt function 
 *
 * Encrypts plaintext using PASTA cipher by generating keystream for each block
 * and adding it to the plaintext modulo Q.
 *
 * @param ciphertext Output array for encrypted data (must be allocated with plaintext_size elements)
 * @param plaintext Input plaintext array
 * @param plaintext_size Size of plaintext in elements
 * @param key Input key array (size 2*N = 256 elements)
 */
void pasta_encrypt_one_block(poly *ciphertext, const poly *plaintext, const int32_t *key) {
    // Generate keystream for this block (hardware-accelerated)

    // Initialize random state (equivalent to init_shake)
    reset_random_state();
    poly rand;
    poly state1, state2;
    poly temp1, temp2;
    size_t r;

    ntt_lite_set_bound(0);
    ntt_lite_add_const(state1.coeffs, key); 
    ntt_lite_add_const(state2.coeffs, key + N);

    // Run PASTA_R rounds
    for (r = 0; r < PASTA_R; r++) {
        pasta_round(&state1, &state2, &state1, &state2, r);
    }

    // Final matmul on both states
    matmul(&temp1, &state1);
    matmul(&temp2, &state2);

    // Final add_rc on both states
    //add_rc(&state1, &temp1);
    //add_rc(&state2, &temp2);
    get_random_vector(&rand, 1);
    poly_add(&state1, &temp1, &rand);
    get_random_vector(&rand, 1);
    poly_add(&state2, &temp2, &rand);

    // Final mix (state1 becomes the keystream)
    //mix(keystream, &final_state2, &state1, &state2);
    ntt_lite_set_bound(2);
    ntt_lite_mul_const(NTT_LITE_OUTPUT_DIS, state1.coeffs);
    ntt_lite_add(NTT_LITE_OUTPUT_DIS, NTT_LITE_INPUT_DIS, state2.coeffs);

    ntt_lite_add(ciphertext->coeffs, NTT_LITE_INPUT_DIS, plaintext->coeffs);
}
void pasta_encrypt_one_block_soft(poly *ciphertext, const poly *plaintext, const int32_t *key) {
    uint64_t nonce = NONCE_CONST;

    // Copy plaintext to ciphertext
    for (size_t i = 0; i < N; i++) {
        ciphertext->coeffs[i] = plaintext->coeffs[i];
    }

    // Process each block
    poly keystream;

    // Generate keystream for this block
    gen_keystream_soft(&keystream, key, nonce, 0);

    // XOR plaintext with keystream (add mod Q)
    for (size_t i = 0; i < N; i++) {
        uint64_t sum = (uint64_t)ciphertext->coeffs[i] + (uint64_t)keystream.coeffs[i];
        ciphertext->coeffs[i] = (int32_t)(sum % Q);
    }
}



