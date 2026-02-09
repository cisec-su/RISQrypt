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
    poly A_square;
    poly_pointwise(&A_square, A, A);
    poly_pointwise(B, A, (const poly *)&A_square);
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
    poly C_square;
    poly C;  // Shifted version of A

    // Shift A by one element to the right
    C.coeffs[0] = 0x00000;
    for (size_t i = 1; i < N; i++) {
        C.coeffs[i] = A->coeffs[i - 1];
    }

    // Now C is shifted: C[0]=0, C[1]=A[0], C[2]=A[1], ..., C[N-1]=A[N-2]
    poly_pointwise(&C_square, (const poly *)&C, (const poly *)&C);  // C_square = C^2
    poly_add(B, A, (const poly *)&C_square);          // B = A + C^2

    // poly_add doesn't do modular reduction, so we need to do it manually
    for (size_t i = 0; i < N; i++) {
        int32_t val = B->coeffs[i];
        if (val >= Q) {
            B->coeffs[i] = val % Q;
        } else if (val < 0) {
            B->coeffs[i] = (val % Q + Q) % Q;
        }
    }
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
void calculate_row(poly *C, const poly *B, const poly *A) {

    poly AB_LAST;
    ntt_lite_set_clr();
    ntt_lite_set_bound(B->coeffs[N-1]);
    ntt_lite_mul_const((uint32_t*)AB_LAST.coeffs, (const uint32_t*)A->coeffs);

    poly B_shifted;  // Shifted version of B
    // Shift B by one element to the right
    B_shifted.coeffs[0] = 0;
    for (size_t i = 1; i < N; i++) {
        B_shifted.coeffs[i] = B->coeffs[i - 1];
    }

    poly_add(C, (const poly *)&AB_LAST, &B_shifted);

    // poly_add doesn't do modular reduction, so we need to do it manually
    for (size_t i = 0; i < N; i++) {
        int32_t val = C->coeffs[i];
        if (val >= Q) {
            C->coeffs[i] = val % Q;
        } else if (val < 0) {
            C->coeffs[i] = (val % Q + Q) % Q;
        }
    }

    // Reset bound to default to avoid affecting subsequent operations
    ntt_lite_set_bound(0);

    // Bu print'i silince bir şeyler yanlış oluyor
    // ilk eleman hatalı üretiliyor 
    if(squeeze_counter < 130) {
        print_string("ERROR!!!! ..\n");
        print_u32_arr(C->coeffs,5);
    }
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

    poly TWO_A_left, TWO_A_right;

    ntt_lite_set_clr();
    ntt_lite_mul_const((uint32_t*)TWO_A_left.coeffs, (const uint32_t*)A_left->coeffs);
    ntt_lite_set_clr();
    ntt_lite_mul_const((uint32_t*)TWO_A_right.coeffs, (const uint32_t*)A_right->coeffs);

    poly_add(B_left, (const poly *)&TWO_A_left, A_right);
    poly_add(B_right, A_left, (const poly *)&TWO_A_right);

    // poly_add doesn't do modular reduction, so we need to do it manually
    for (size_t i = 0; i < N; i++) {
        int32_t val = B_left->coeffs[i];
        if (val >= Q) {
            B_left->coeffs[i] = val % Q;
        } else if (val < 0) {
            B_left->coeffs[i] = (val % Q + Q) % Q;
        }

        val = B_right->coeffs[i];
        if (val >= Q) {
            B_right->coeffs[i] = val % Q;
        } else if (val < 0) {
            B_right->coeffs[i] = (val % Q + Q) % Q;
        }
    }

    // Reset bound to default to avoid affecting subsequent operations
    ntt_lite_set_bound(0);
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
    poly curr_row;

    // Generate random vector (no zeros)
    get_random_vector(&rand, 0);

    // Initialize curr_row with rand
    for (size_t i = 0; i < N; i++) {
        curr_row.coeffs[i] = rand.coeffs[i];
    }

    // For each row in the matrix
    for (size_t i = 0; i < N; i++) {


        //print_string("curr_row...\n");
        //print_u32_arr(curr_row.coeffs,5);

        // Compute dot product: acc = sum(curr_row[j] * state[j]) mod Q
        uint64_t acc = 0;
        poly temp;
        ntt_lite_set_clr();

        poly_pointwise(&temp, (const poly *)&curr_row, state);
        
        // Ensure modular reduction after pointwise multiplication
        for (size_t j = 0; j < N; j++) {
            int32_t val = temp.coeffs[j];
            if (val >= Q) {
                temp.coeffs[j] = val % Q;
            } else if (val < 0) { // Handle negative values???? WHY?
                temp.coeffs[j] = (val % Q + Q) % Q;
            }
        }

        for (size_t j = 0; j < N; j++) {
            uint64_t mult = (uint64_t)temp.coeffs[j];
            acc = (acc + mult) % Q;
        }

        new_state->coeffs[i] = (int32_t)acc;

        // Calculate next row if not last iteration
        if (i != N - 1) {
            poly next_row;
            calculate_row(&next_row, (const poly *)&curr_row, &rand);
            // Update curr_row for next iteration
            for (size_t k = 0; k < N; k++) {
                curr_row.coeffs[k] = next_row.coeffs[k];
            }
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
    poly_add(B, A, (const poly *)&rand);

    // poly_add doesn't do modular reduction, so we need to do it manually
    for (size_t i = 0; i < N; i++) {
        int32_t val = B->coeffs[i];
        if (val >= Q) {
            B->coeffs[i] = val % Q;
        } else if (val < 0) {
            B->coeffs[i] = (val % Q + Q) % Q;
        }
    }
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

    //print_string("temp1: \n");
    //print_u32_arr(temp1.coeffs,5);
    //print_string("temp2: \n");
    //print_u32_arr(temp2.coeffs,5);
    
    // Step 2: Add random constants to both states
    add_rc(&temp3, &temp1);
    add_rc(&temp4, &temp2);
    
    // Step 3: Mix the two states
    mix(&temp5, &temp6, &temp3, &temp4);
    
    // Step 4: Apply S-box (cube for last round, feistel otherwise)
    if (r == PASTA_R - 1) {
        sbox_cube(C, &temp5);
        sbox_cube(D, &temp6);
    } else {
        sbox_feistel(C, &temp5);
        sbox_feistel(D, &temp6);
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
 * @param nonce Nonce value
 * @param block_counter Block counter value
 */
void gen_keystream(poly *keystream, const int32_t *key, uint64_t nonce, uint64_t block_counter) {
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
        //print_string("state1state1:\n");
        //print_u32_arr(state1.coeffs,5);
        pasta_round(&new_state1, &new_state2, &state1, &state2, r);
        state1 = new_state1;
        state2 = new_state2;
    }

    // Final matmul on both states
    poly temp1, temp2;
    matmul(&temp1, &state1);
    matmul(&temp2, &state2);

    // Final add_rc on both states
    add_rc(&state1, &temp1);
    add_rc(&state2, &temp2);

    // Final mix (state1 becomes the keystream)
    poly final_state2;
    mix(keystream, &final_state2, &state1, &state2);
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
    uint64_t nonce = NONCE_CONST;

    // Copy plaintext to ciphertext
    for (size_t i = 0; i < N; i++) {
        ciphertext->coeffs[i] = plaintext->coeffs[i];
    }

    // Process each block
    poly keystream;

    // Generate keystream for this block (hardware-accelerated)
    gen_keystream(&keystream, key, nonce, 0);
    poly_add(ciphertext, plaintext, &keystream);
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



