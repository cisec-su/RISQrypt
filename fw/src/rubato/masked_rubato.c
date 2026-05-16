#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "params.h"
#include "symmetric.h"
#include "ntt_lite.h"
#include "masked_poly.h"
#include "masked_gadgets.h"
#include "rubato.h"
#include "poly.h"
#include "util.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Masked RUBATO S-box Feistel layer
 * @description Computes B[0] = A[0], B[i] = A[i] + A[i-1]^2 using masked polynomial operations. Shifts A left by one position per share, then calls masked_poly_mac to compute B = shift(A)^2 + A, preserving masking throughout
 * @param B pointer to output masked polynomial
 * @param A pointer to input masked polynomial
 * @return void
 */
void masked_rubato_sbox_feistel(masked_poly *B, const masked_poly *A) {
    // Feistel function implementation: B[0]=A[0], B[i]=A[i]+A[i-1]^2 mod Q
    static masked_poly A_shifted, A_refreshed; // 0 a0 a1 a2 a3 a4 ... a126
    masked_poly_right_shift(&A_shifted,A,1);
    masked_gadgets_x2x_a_ref(&A_refreshed, &A_shifted);
    masked_poly_mac(B,&A_shifted,&A_refreshed,A);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Generates one RUBATO keystream block.
 * @param ciphertext Output polynomial.
 * @param plaintext  Input polynomial to XOR with the keystream.
 * @param key        Secret key polynomial.
 * @param nonce      64-bit nonce.
 * @param block_ctr  64-bit block counter.
 */
void masked_rubato_encrypt(poly *ciphertext, const poly *plaintext, poly *key, uint64_t nonce, uint64_t block_ctr){
    
    size_t r;
    size_t i;
    poly rnd;
    poly state;
    masked_poly m_state;
    masked_poly m_key;
    static const int32_t state_init[N] = RUBATO_STATE_INIT;


    poly_init_q();
    ntt_lite_set_bound(0);
    ntt_lite_add_const(state.coeffs, state_init);

    masked_gadgets_init_q();
    masked_poly_mask(&m_state,&state);
    masked_poly_mask(&m_key,key);

    for (r = 0; r < (size_t) RUBATO_R; r++) {

        poly_uniform(&rnd, nonce, block_ctr, r);
        masked_poly_mult_add_umm(&m_state,&rnd,&m_key,&m_state);

        for (i = 0; i < MASKING_N; i++) {
            rubato_linear_layer(&m_state.share[i], &m_state.share[i]);
        }

        masked_rubato_sbox_feistel(&m_state, &m_state);
    }

    for (i = 0; i < MASKING_N; i++) {
        rubato_linear_layer(&m_state.share[i], &m_state.share[i]);
    }


    poly_uniform(&rnd, nonce, block_ctr, RUBATO_R);
    masked_poly_mult_add_umm(&m_state,&rnd,&m_key,&m_state);
    masked_poly_unmask(&state, &m_state);
    ntt_lite_add(ciphertext->coeffs, state.coeffs, plaintext->coeffs);

}