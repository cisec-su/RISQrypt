#include <stdint.h>
#include "params.h"
#include "masked_sign.h"
#include "masked_packing.h"
#include "masked_polyvec.h"
#include "masked_poly.h"
#include "masked_gadgets.h"
#include "randombytes.h"
#include "masked_symmetric.h"


// typedef struct {
//   masked_poly vec[L];
// } masked_polyvecl;


// typedef struct {
//   polyvecl share[MASKING_N];
// } masked_polyvecl_t;


// typedef struct {
//   masked_poly vec[K];
// } masked_polyveck;


// typedef struct {
//   polyveck share[MASKING_N];
// } masked_polyveck_t;

/* Convert between masked_polyvecl and masked_polyvecl_t representations */
// void masked_polyvecl_to_shares(masked_polyvecl_t *dst, const masked_polyvecl *src) {
//     for(int i = 0; i < MASKING_N; i++) {
//         for(int j = 0; j < L; j++) {
//             for (int k = 0; k < N; k++) {
//                 dst->share[i].vec[j].coeffs[k] = src->vec[j].share[i].coeffs[k];
//             }
//         }
//     }
// }



// void masked_polyvecl_from_shares(masked_polyvecl *dst, const masked_polyvecl_t *src) {
//     for(int i = 0; i < MASKING_N; i++) {
//         for(int j = 0; j < L; j++) {
//             for (int k = 0; k < N; k++) {
//                 dst->vec[j].share[i].coeffs[k] = src->share[i].vec[j].coeffs[k];
//             }
//         }
//     }
// }

// /* Convert between masked_polyveck and masked_polyveck_t representations */
// void masked_polyveck_to_shares(masked_polyveck_t *dst, const masked_polyveck *src) {
//     for(int i = 0; i < MASKING_N; i++) {
//         for(int j = 0; j < K; j++) {
//             for (int k = 0; k < N; k++) {
//                 dst->share[i].vec[j].coeffs[k] = src->vec[j].share[i].coeffs[k];
//             }
//         }
//     }
// }

// void masked_polyveck_from_shares(masked_polyveck *dst, const masked_polyveck_t *src) {
//     for(int i = 0; i < MASKING_N; i++) {
//         for(int j = 0; j < K; j++) {
//             for (int k = 0; k < N; k++) {
//                 dst->vec[j].share[i].coeffs[k] = src->share[i].vec[j].coeffs[k];
//             }
//         }
//     }
// }



int masked_crypto_sign_signature_init(polyvecl mat[K],
                                      uint8_t *tr,
                                      polyveck *t0,
                                      masked_polyvecl_t *s1,
                                      masked_polyveck_t *s2,
                                      masked_seed key,
                                      const uint8_t *sk)
{
    uint8_t rho[SEEDBYTES];

    poly_init_q();
    masked_gadgets_init_q();

    masked_unpack_sk(rho, tr, key, t0, s1, s2, sk);

    /* Expand matrix and transform vectors */
    polyvec_matrix_expand(mat, rho);
    return 0;
}


int masked_crypto_sign_signature_core(uint8_t *sig,
                                      size_t *siglen,
                                      const uint8_t *m,
                                      size_t mlen,
                                      const polyvecl mat[K],
                                      const uint8_t *tr,
                                      const masked_seed key,
                                      polyveck *t0,
                                      masked_polyvecl_t *s1,
                                      masked_polyveck_t *s2)
{
    unsigned int n;
    uint8_t mu[CRHBYTES];
    masked_crh rhoprime;
    uint16_t nonce = 0;
    polyveck w1, h;
    masked_polyveck w;
    masked_polyveck *w0;
    masked_polyvecl_t y_t, z_t, s1_t;
    masked_polyveck_t s2_t, w_t, w0_t;
    polyvecl *z_unmasked;
    polyveck *w0_unmasked;
    poly cp;
    masked_poly_ptr temp_ptr;

    int i;
    int flag;

    temp_ptr.share[0] = &y_t.share[0].vec[0];
    temp_ptr.share[1] = &y_t.share[1].vec[0];

    w0_unmasked = (polyveck*) &w0_t;
    w0 = &w;
    z_unmasked = (polyvecl*) &z_t;

    /* Compute mu = CRH(tr | msg) rename    */
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
    polyveck_ntt(t0);

rej:
    /* Sample intermediate vector y */
    if (nonce) {
        masked_gadgets_init_q();
    }
    masked_polyvecl_uniform_gamma1(&y_t, rhoprime, nonce++);

    /* Matrix-vector multiplication */ 
    poly_init_ntt(); // re-init NTT since uniform_gamma1 uses NTT-Lite
    masked_polyvecl_ntt(&y_t);

    masked_polyvec_matrix_pointwise(&w_t, mat, &y_t);

    poly_init_invntt();
    masked_polyveck_invntt(&w_t);

    /* Decompose w and call the random oracle */
    masked_polyveck_decompose(&w1, &w0_t, &w_t);

    polyveck_pack_w1(sig, &w1);
    dilithium_shake256_absorb_double(sig, SEEDBYTES, mu, CRHBYTES, sig, K*POLYW1_PACKEDBYTES);
    poly_challenge(&cp, sig);

    poly_init_ntt();

    poly_ntt(&cp);

    /* Compute z, reject if it reveals secret */
    flag = masked_polyvecl_pointwise_add_invntt_chknorm(&z_t, s1, &cp, &y_t, GAMMA1 - BETA);
#ifndef TTEST // In t-test setting, we only perform a single iteration of the rejection sampling loop
    if(flag) {
        goto rej;
    }
#endif
    // return 0 ;

    /* w0 - cs2. Check that subtracting cs2 does not change high bits of w and low bits
     * do not reveal secret information */
    flag = masked_polyveck_pointwise_invntt_sub_chknorm(&w0_t, s2, &cp, &w0_t, &temp_ptr, GAMMA2 - BETA);
#ifndef TTEST
    if(flag) {
        goto rej;
    }
#endif

#ifndef TTEST // Rest of the computations are either unmasking or public operations
    /* Compute hints for w1 */
    polyveck_pointwise_poly(&h, &cp, t0);
    poly_init_invntt();
    flag = polyveck_invntt_chknorm(&h, GAMMA2);
    if(flag) {
        goto rej;
    }

    masked_polyveck_unmask(w0_unmasked, &w0_t);

    n = polyveck_add_make_hint(&h, w0_unmasked, &w1, &h);
    if(n > OMEGA) {
        goto rej;
    }
    masked_polyvecl_unmask(z_unmasked, &z_t);
    /* Write signature */
    pack_sig(sig, sig, z_unmasked, &h);
    *siglen = CRYPTO_BYTES;
#endif

    return 0;
}




int masked_crypto_sign_signature(uint8_t *sig,
                                 size_t *siglen,
                                 const uint8_t *m,
                                 size_t mlen,
                                 const uint8_t *sk)
{
    polyvecl mat[K];
    uint8_t rho[SEEDBYTES];
    uint8_t tr[SEEDBYTES];
    masked_polyvecl_t s1;
    masked_polyveck_t s2;
    polyveck t0;
    masked_seed key;

    masked_crypto_sign_signature_init(mat, tr, &t0, &s1, &s2, key, sk);

    return masked_crypto_sign_signature_core(sig, siglen, m, mlen, mat, tr, key, &t0, &s1, &s2);
}