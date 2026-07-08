#include <stdint.h>
#include "params.h"
#include "masked_sign.h"
#include "masked_packing.h"
#include "masked_polyvec.h"
#include "masked_poly.h"
#include "masked_gadgets.h"
#include "randombytes.h"
#include "masked_symmetric.h"
#include "ntt_lite.h"



int masked_crypto_sign_signature_init(uint8_t rho[SEEDBYTES], uint8_t *tr, masked_polyvecl *s1, masked_polyveck *s2, masked_seed key, const uint8_t *sk)
{
    poly_init_q();
    masked_gadgets_init_q();

    masked_unpack_sk(rho, tr, key, NULL, s1, s2, sk);

    return 0;
}


int masked_crypto_sign_signature_core(uint8_t *sig, size_t *siglen, const uint8_t *m, size_t mlen, const uint8_t rho[SEEDBYTES], const uint8_t *tr, const masked_seed key, masked_polyvecl *s1, masked_polyveck *s2, const uint8_t *sk)
{
    unsigned int i, n;
    uint8_t mu[CRHBYTES];
    uint8_t w1_head[SEEDBYTES];
    uint8_t c[SEEDBYTES];
    masked_crh rhoprime;
    uint16_t nonce = 0;
    masked_polyveck w0;
    poly cp;
    poly t0_row;
    polyvecl *z_unmasked;
    polyveck *w0_unmasked;
    masked_poly_ptr temp_ptr;
    int flag;

    union {
        masked_polyvecl y;
        polyveck h;
    } y_h;

    masked_polyvecl z;

    temp_ptr.share[0] = &y_h.y.share[0].vec[0];
    temp_ptr.share[1] = &y_h.y.share[1].vec[0];

    w0_unmasked = (polyveck*) &w0;
    z_unmasked = (polyvecl*) &z;

    /* Compute mu = CRH(tr | msg) */
    dilithium_shake256_absorb_double(mu, CRHBYTES, tr, SEEDBYTES, m, mlen);

#ifdef DILITHIUM_RANDOMIZED_SIGNING
    #error "Randomized signing not supported in masked version"
#else
    /* Compute rhoprime = SHAKE256(key | mu) */
    dilithium_masked_shake256_absorb_double((masked_flat_ptr) rhoprime, CRHBYTES, (masked_flat_ptr) key, SEEDBYTES, mu, CRHBYTES);
#endif

    poly_init_ntt();

    masked_polyvecl_ntt(s1);
    masked_polyveck_ntt(s2);

rej:
    /* Sample intermediate vector y */
    if(nonce) {
        masked_gadgets_init_q();
    }

    masked_polyvecl_uniform_gamma1(&y_h.y, rhoprime, nonce++);

    /* Matrix-vector multiplication, inverse NTT and decomposition */
    poly_init_ntt();
    masked_polyvecl_ntt(&y_h.y);

    masked_polyvec_matrix_pointwise_decompose_onthefly(sig, &w0, rho, &y_h.y);

    /*
     * The challenge hash overwrites the first SEEDBYTES bytes of sig.
     * Preserve the corresponding prefix of packed w1.
     */
    for(i = 0; i < SEEDBYTES; i++) {
        w1_head[i] = sig[i];
    }

    dilithium_shake256_absorb_double(sig, SEEDBYTES, mu, CRHBYTES, sig, K * POLYW1_PACKEDBYTES);

    for(i = 0; i < SEEDBYTES; i++) {
        c[i] = sig[i];
        sig[i] = w1_head[i];
    }

    poly_challenge(&cp, c);

    poly_init_ntt();

    poly_ntt(&cp);

    /* Compute z, reject if it reveals secret */
    flag = masked_polyvecl_pointwise_add_invntt_chknorm(&z, s1, &cp, &y_h.y, GAMMA1 - BETA);

#ifndef TTEST
    if(flag) {
        goto rej;
    }
#endif

    /*
     * y is no longer needed after the z computation.
     * Its storage is reused as temporary scratch through temp_ptr.
     */

    flag = masked_polyveck_pointwise_invntt_sub_chknorm(&w0, s2, &cp, &w0, &temp_ptr, GAMMA2 - BETA);

#ifndef TTEST
    if(flag) {
        goto rej;
    }
#endif

#ifndef TTEST
    /*
     * y scratch is no longer needed.
     * The same union storage can now safely hold h.
     *
     * t0 is unpacked and processed one row at a time.
     */
    for(i = 0; i < K; i++) {
        masked_unpack_sk_t0_row(&t0_row, sk, i);

        poly_init_ntt();
        poly_ntt(&t0_row);

        poly_pointwise(&y_h.h.vec[i], &cp, &t0_row);

        poly_init_invntt();
        ntt_lite_set_bound(GAMMA2);

        flag = poly_invntt_chknorm(&y_h.h.vec[i], GAMMA2);

        if(flag) {
            goto rej;
        }
    }

    masked_polyveck_unmask(w0_unmasked, &w0);

    n = polyveck_add_make_hint_packed(&y_h.h, w0_unmasked, sig, &y_h.h);

    if(n > OMEGA) {
        goto rej;
    }

    masked_polyvecl_unmask(z_unmasked, &z);

    /* Write signature */
    pack_sig(sig, c, z_unmasked, &y_h.h);
    *siglen = CRYPTO_BYTES;
#endif

    return 0;
}

int masked_crypto_sign_signature(uint8_t *sig, size_t *siglen, const uint8_t *m, size_t mlen, const uint8_t *sk)
{
    uint8_t rho[SEEDBYTES];
    uint8_t tr[SEEDBYTES];
    masked_polyvecl s1;
    masked_polyveck s2;
    masked_seed key;

    masked_crypto_sign_signature_init(rho, tr, &s1, &s2, key, sk);

    return masked_crypto_sign_signature_core(sig, siglen, m, mlen, rho, tr, key, &s1, &s2, sk);
}