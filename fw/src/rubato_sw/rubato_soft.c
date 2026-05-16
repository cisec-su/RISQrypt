#include <stdint.h>
#include <stddef.h>
#include "params.h"
#include "rubato_soft.h"
#include "poly_soft.h"
#include "fips202.h"

/**
 * @brief Applies RUBATO Feistel S-box layer.
 * @param B Output polynomial.
 * @param A Input polynomial.
 */
void rubato_soft_sbox_feistel(poly *B, const poly *A) {
    size_t el;
    uint32_t sq_reduced;
    uint32_t prev;
    uint32_t cur;

    prev = (uint32_t)A->coeffs[0];
    B->coeffs[0] = (int32_t)prev;
    for (el = 1; el < N; el++) {
        cur = (uint32_t)A->coeffs[el];
        sq_reduced = MOD_Q_MULT(prev, prev);
        B->coeffs[el] = (sq_reduced + cur) % Q;
        prev = cur;
    }
}

/**
 * @brief Applies RUBATO linear layer A = M * B * M_transpoze over Z_q.
 * @param A Output polynomial.
 * @param B Input polynomial.
 */
void rubato_soft_linear_layer(poly *A, const poly *B) {
    static const uint32_t lin_coeffs[] = LINEAR_COEFFS;
    const size_t v = V;
    const size_t coeff_count = LINEAR_COEFFS_COUNT;
    size_t row, col, i;
    size_t src_row, src_col;
    uint64_t acc;
    uint64_t mixed_raw[N];

    for (row = 0; row < v; row++) {
        for (col = 0; col < v; col++) {
            acc = 0;
            src_row = row;
            for (i = 0; i < coeff_count; i++) {
                acc += (uint64_t)lin_coeffs[i] * (uint32_t)B->coeffs[src_row * v + col];
                src_row++;
                if (src_row == v) {
                    src_row = 0;
                }
            }
            mixed_raw[row * v + col] = acc;
        }
    }

    for (row = 0; row < v; row++) {
        for (col = 0; col < v; col++) {
            acc = 0;
            src_col = col;
            for (i = 0; i < coeff_count; i++) {
                acc += (uint64_t)lin_coeffs[i] * mixed_raw[row * v + src_col];
                src_col++;
                if (src_col == v) {
                    src_col = 0;
                }
            }
            A->coeffs[row * v + col] = (int32_t)(acc % (uint64_t)Q);
        }
    }
}

/**
 * @brief Samples round coefficients from SHAKE stream with rejection sampling.
 * @param coeffs Output coefficient polynomials (R+1 polynomials).
 * @param nonce Nonce value for stream seed.
 * @param block_ctr Block counter for stream seed.
 */
void rubato_soft_sampling(poly coeffs[RUBATO_R + 1], uint64_t nonce, uint64_t block_ctr) {
    uint8_t seed[16];
    uint8_t buf[XOF_RATE_BYTES];
    keccak_state state;
    size_t k, accepted, pos;
    size_t r;
    size_t idx;
    uint32_t elem;

    for (r = 0; r < (size_t)(RUBATO_R + 1); r++) {
        for (k = 0; k < N; k++) {
            coeffs[r].coeffs[k] = 0;
        }
    }

    for (k = 0; k < 8; k++) {
        seed[k] = (uint8_t)((nonce >> (8 * k)) & 0xFF);
        seed[8 + k] = (uint8_t)((block_ctr >> (8 * k)) & 0xFF);
    }

#if defined(XOF_SHAKE128)
    shake128_absorb(&state, seed, 16);
#elif defined(XOF_SHAKE256)
    shake256_absorb(&state, seed, 16);
#endif

    accepted = 0;
    while (accepted < RUBATO_XOF_ELEM_COUNT) {
#if defined(XOF_SHAKE128)
        shake128_squeezeblocks(buf, 1, &state);
#elif defined(XOF_SHAKE256)
        shake256_squeezeblocks(buf, 1, &state);
#endif
        pos = 0;
        while (accepted < RUBATO_XOF_ELEM_COUNT && pos + 4 <= XOF_RATE_BYTES) {
            elem = (uint32_t)buf[pos + 0]
                 | ((uint32_t)buf[pos + 1] << 8)
                 | ((uint32_t)buf[pos + 2] << 16)
                 | ((uint32_t)buf[pos + 3] << 24);
            pos += 4;
            elem &= MODULUS_BIT_MASK;
            if (elem < (uint32_t)Q) {
                r = accepted / N;
                idx = accepted % N;
                coeffs[r].coeffs[idx] = (int32_t)elem;
                accepted++;
            }
        }
    }

    // print_string("\n=== COEFFICIENTS ===\n");
    // for (r = 0; r < (size_t)(RUBATO_R + 1); r++) {
    //     for (k = 0; k < N; k++) {
    //         print_u32((uint32_t)coeffs[r].coeffs[k]);
    //         print_string(" ");
    //     }
    //     print_string("\n");
    // }
    // print_string("\n");

}  

/**
 * @brief Samples a single RUBATO polynomial from SHAKE with rejection sampling.
 * @param p Output polynomial.
 * @param nonce Nonce value for stream seed.
 * @param block_ctr Block counter for stream seed.
 */
void rubato_soft_poly_uniform(poly *p, uint64_t nonce, uint64_t block_ctr) {
    uint8_t seed[16];
    uint8_t buf[XOF_RATE_BYTES];
    keccak_state state;
    size_t k, offset, pos;
    uint32_t elem;

    for (k = 0; k < 8; k++) {
        seed[k] = (uint8_t)((nonce >> (8 * k)) & 0xFF);
        seed[8 + k] = (uint8_t)((block_ctr >> (8 * k)) & 0xFF);
    }

#if defined(XOF_SHAKE128)
    shake128_absorb(&state, seed, 16);
#elif defined(XOF_SHAKE256)
    shake256_absorb(&state, seed, 16);
#endif

    offset = 0;
    while (offset < N) {
#if defined(XOF_SHAKE128)
        shake128_squeezeblocks(buf, 1, &state);
#elif defined(XOF_SHAKE256)
        shake256_squeezeblocks(buf, 1, &state);
#endif
        pos = 0;
        while (offset < N && pos + 4 <= XOF_RATE_BYTES) {
            elem = (uint32_t)buf[pos + 0]
                 | ((uint32_t)buf[pos + 1] << 8)
                 | ((uint32_t)buf[pos + 2] << 16)
                 | ((uint32_t)buf[pos + 3] << 24);
            pos += 4;
            elem &= MODULUS_BIT_MASK;
            if (elem < (uint32_t)Q) {
                p->coeffs[offset++] = (int32_t)elem;
            }
        }
    }
}


/**
 * @brief Generates one RUBATO ciphertext block from plaintext and keystream.
 * @param ciphertext Output polynomial holding RUBATO_OUTPUTSIZE coefficients.
 * @param plaintext Input plaintext coefficients.
 * @param key Secret key coefficients.
 * @param nonce Nonce value.
 * @param block_ctr Block counter.
 */
void rubato_soft_encrypt(poly *ciphertext, const poly *plaintext, const poly *key, uint64_t nonce, uint64_t block_ctr) {
    poly coeffs[RUBATO_R + 1];
    poly state;
    size_t r;
    size_t i;
    poly keystream;

    rubato_soft_sampling(coeffs, nonce, block_ctr);

    for (i = 0; i < N; i++) {
        state.coeffs[i] = (int32_t)(i + 1);
    }

    for (r = 0; r < (size_t)RUBATO_R; r++) {
        poly_soft_mult_add(&state, &coeffs[r], key, &state);
        rubato_soft_linear_layer(&state, &state);
        rubato_soft_sbox_feistel(&state, &state);
    }

    rubato_soft_linear_layer(&state, &state);
    poly_soft_mult_add(&keystream, &coeffs[RUBATO_R], key, &state);

    for (i = 0; i < N; i++) {
        ciphertext->coeffs[i] = (keystream.coeffs[i] + plaintext->coeffs[i]) % Q;
    }
    
}
