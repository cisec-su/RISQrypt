#include <stdint.h>
#include "params.h"
#include "sign.h"
#include "packing.h"
#include "polyvec.h"
#include "poly.h"
#include "randombytes.h"
#include "symmetric.h"

#define Z_CHECK_ITERATIVE
#define W0CS2_CHECK_ITERATIVE

/*************************************************
* Name:        crypto_sign_keypair
*
* Description: Generates public and private key.
*
* Arguments:   - uint8_t *pk: pointer to output public key (allocated
*                             array of CRYPTO_PUBLICKEYBYTES bytes)
*              - uint8_t *sk: pointer to output private key (allocated
*                             array of CRYPTO_SECRETKEYBYTES bytes)
*
* Returns 0 (success)
**************************************************/
int crypto_sign_keypair(uint8_t *pk, uint8_t *sk) {
    uint8_t seedbuf[2*SEEDBYTES + CRHBYTES];
    uint8_t tr[SEEDBYTES];
    const uint8_t *rho, *rhoprime, *key;
    polyvecl s1;
    polyveck s2, t1, t0;

    poly_init_q();
    
    /* Get randomness for rho, rhoprime and key */
    randombytes(seedbuf, SEEDBYTES);
    
    /* Expand with SHAKE256 into rho, rhoprime and key */
    dilithium_shake256(seedbuf, 2*SEEDBYTES + CRHBYTES, seedbuf, SEEDBYTES);

    rho = seedbuf;
    rhoprime = rho + SEEDBYTES;
    key = rhoprime + CRHBYTES;

    /* Sample short vectors s1 and s2 */
    polyvecl_uniform_eta(&s1, rhoprime, 0);
    polyveck_uniform_eta(&s2, rhoprime, L);

    pack_sk_s1(sk, &s1);

    poly_init_ntt();
    polyvecl_ntt(&s1);

    polyvec_matrix_pointwise_onthefly(&t1, rho, &s1);

    poly_init_invntt();
    polyveck_invntt(&t1);

    /* Add error vector s2 */
    polyveck_add(&t1, &t1, &s2);

    /* Extract t1 and write public key */
    polyveck_power2round(&t1, &t0, &t1);

    pack_pk(pk, rho, &t1);
    
    /* Compute H(rho, t1) and write secret key CRYPTO_PUBLICKEYBYTES */ 
    dilithium_shake256(tr, SEEDBYTES, pk, CRYPTO_PUBLICKEYBYTES);

    pack_sk(sk, rho, tr, key, &t0, NULL, &s2);

    return 0;
}


/*************************************************
* Name:        crypto_sign_signature
*
* Description: Computes signature.
*
* Arguments:   - uint8_t *sig:   pointer to output signature (of length CRYPTO_BYTES)
*              - size_t *siglen: pointer to output length of signature
*              - uint8_t *m:     pointer to message to be signed
*              - size_t mlen:    length of message
*              - uint8_t *sk:    pointer to bit-packed secret key
*
* Returns 0 (success)
**************************************************/
int crypto_sign_signature(uint8_t *sig,
                          size_t *siglen,
                          const uint8_t *m,
                          size_t mlen,
                          const uint8_t *sk)
{
    unsigned int n;
    uint32_t seedbuf_32[(3*SEEDBYTES + 2*CRHBYTES) >> 2];
    uint8_t *seedbuf = (uint8_t *) seedbuf_32;
    uint8_t *rho, *tr, *key, *mu, *rhoprime;
    uint16_t nonce = 0;
    polyvecl s1, y;
    #ifndef Z_CHECK_ITERATIVE
        polyvecl z;
    #endif
    polyveck t0, s2, w1, w0, h;
    poly cp;
    int flag;

    rho = seedbuf;
    tr = rho + SEEDBYTES;
    key = tr + SEEDBYTES;
    mu = key + SEEDBYTES;
    rhoprime = mu + CRHBYTES;

    poly_init_q();
    
    unpack_sk(rho, tr, key, &t0, &s1, &s2, sk);
    
    /* Compute CRH(tr, msg) rename    */
    dilithium_shake256_absorb_double(mu, CRHBYTES, tr, SEEDBYTES, m, mlen);

#ifdef DILITHIUM_RANDOMIZED_SIGNING
    randombytes(rhoprime, CRHBYTES);
#else
    /* Compute rhoprime = SHAKE256(key) */
    dilithium_shake256(rhoprime, CRHBYTES, key, SEEDBYTES + CRHBYTES);
#endif



    poly_init_ntt();
    polyvecl_ntt(&s1);
    polyveck_ntt(&s2); 
    polyveck_ntt(&t0); 

rej:
    /* Sample intermediate vector y */
    polyvecl_uniform_gamma1(&y, rhoprime, nonce++);

    /* Matrix-vector multiplication */ 
    poly_init_ntt(); // re-init NTT since uniform_gamma1 uses NTT-Lite
    polyvecl_ntt(&y);

    polyvec_matrix_pointwise_onthefly(&w1, rho, &y);

    poly_init_invntt();
    polyveck_invntt(&w1);

    /* Decompose w and call the random oracle */
    polyveck_decompose(&w1, &w0, &w1);

    polyveck_pack_w1(sig, &w1);
    dilithium_shake256_absorb_double(sig, SEEDBYTES, mu, CRHBYTES, sig, K*POLYW1_PACKEDBYTES);

    poly_challenge(&cp, sig);

    poly_init_ntt();

    poly_ntt(&cp);

    /* Compute z, reject if it reveals secret */
#ifdef Z_CHECK_ITERATIVE
    /*
     * Compute z = y + c*s1 in-place.
     * The original y is no longer needed after this operation,
     * so the result z is stored in y to reduce stack usage.
     */
    flag = polyvecl_pointwise_add_invntt_chknorm(
        &y, &s1, &cp, &y, GAMMA1 - BETA
    );
#else
    polyvecl_pointwise_poly(&z, &cp, &s1);
    polyvecl_add(&z, &z, &y);
    poly_init_invntt();
    flag = polyvecl_invntt_chknorm(&z, GAMMA1 - BETA);
#endif
    if(flag) {
        goto rej;
    }

    /* w0 - cs2. Check that subtracting cs2 does not change high bits of w and low bits
     * do not reveal secret information */
#ifdef W0CS2_CHECK_ITERATIVE
    flag = polyveck_pointwise_invntt_sub_chknorm(&w0, &s2, &cp, &w0, GAMMA2 - BETA);
#else
    polyveck_pointwise_poly(&h, &cp, &s2);
    poly_init_invntt();
    polyveck_invntt_sub(&w0, &w0, &h);
    flag = polyveck_chknorm(&w0, GAMMA2 - BETA);
#endif
    if(flag) {
         goto rej;
    }

    /* Compute hints for w1 */
    polyveck_pointwise_poly(&h, &cp, &t0);
    poly_init_invntt();
    flag = polyveck_invntt_chknorm(&h, GAMMA2);
    if(flag) {
            goto rej;
    }

    n = polyveck_add_make_hint(&h, &w0, &w1, &h);
    if(n > OMEGA) {
            goto rej; // if this is rare, we can merge with packing
    }

    /* Write signature */
    #ifdef Z_CHECK_ITERATIVE
    pack_sig(sig, sig, &y, &h);
    #else
        pack_sig(sig, sig, &z, &h);
    #endif
    *siglen = CRYPTO_BYTES;

    return 0;
}

/*************************************************
* Name:        crypto_sign
*
* Description: Compute signed message.
*
* Arguments:   - uint8_t *sm: pointer to output signed message (allocated
*                             array with CRYPTO_BYTES + mlen bytes),
*                             can be equal to m
*              - size_t *smlen: pointer to output length of signed
*                               message
*              - const uint8_t *m: pointer to message to be signed
*              - size_t mlen: length of message
*              - const uint8_t *sk: pointer to bit-packed secret key
*
* Returns 0 (success)
**************************************************/
int crypto_sign(uint8_t *sm,
                size_t *smlen,
                const uint8_t *m,
                size_t mlen,
                const uint8_t *sk)
{
    size_t i;

    for(i = 0; i < mlen; ++i)
        sm[CRYPTO_BYTES + mlen - 1 - i] = m[mlen - 1 - i];
    crypto_sign_signature(sm, smlen, sm + CRYPTO_BYTES, mlen, sk);
    *smlen += mlen;
    return 0;
}

/*************************************************
* Name:        crypto_sign_verify
*
* Description: Verifies signature.
*
* Arguments:   - uint8_t *m: pointer to input signature
*              - size_t siglen: length of signature
*              - const uint8_t *m: pointer to message
*              - size_t mlen: length of message
*              - const uint8_t *pk: pointer to bit-packed public key
*
* Returns 0 if signature could be verified correctly and -1 otherwise
**************************************************/
int crypto_sign_verify(const uint8_t *sig,
                       size_t siglen,
                       const uint8_t *m,
                       size_t mlen,
                       const uint8_t *pk)
{
    unsigned int i;
    uint8_t buf[POLYW1_PACKEDBYTES];
    uint8_t rho[SEEDBYTES];
    uint8_t mu[CRHBYTES];
    uint8_t c[SEEDBYTES];
    uint8_t c2[SEEDBYTES];
    poly cp;
    polyvecl z;
    polyveck t1, w1, h;

    if(siglen != CRYPTO_BYTES)
        return -1;

    poly_init_q();

    unpack_pk(rho, &t1, pk);
    if(unpack_sig(c, &z, &h, sig))
        return -1;
    if(polyvecl_chknorm(&z, GAMMA1 - BETA))
        return -1;

    /* Compute CRH(h(rho, t1), msg) */
    dilithium_shake256_mu_crh(mu, pk, m, mlen);

    /* Matrix-vector multiplication; compute Az - c2^dt1 */
    poly_challenge(&cp, c);

    poly_init_ntt();

    polyvecl_ntt(&z);

    poly_ntt(&cp);

    polyveck_shiftl_ntt(&t1);

    polyvec_matrix_pointwise_onthefly(&w1, rho, &z);

    polyveck_pointwise_poly_sub(&w1, &cp, &t1, &w1);

    poly_init_invntt();
    polyveck_invntt(&w1);

    /* Reconstruct w1 and absorb each packed polynomial */
    polyveck_use_hint_pack_init();
    dilithium_shake256_challenge_init(mu);

    for(i = 0; i < K; i++) {
        poly_use_hint_pack(buf, &w1.vec[i], &h.vec[i]);
        dilithium_shake256_challenge_absorb(buf);
    }

    /* Call random oracle and verify challenge */
    dilithium_shake256_challenge_finalize(c2);

    for(i = 0; i < SEEDBYTES; ++i)
        if(c[i] != c2[i])
            return -1;

    return 0;
}

/*************************************************
* Name:        crypto_sign_open
*
* Description: Verify signed message.
*
* Arguments:   - uint8_t *m: pointer to output message (allocated
*                            array with smlen bytes), can be equal to sm
*              - size_t *mlen: pointer to output length of message
*              - const uint8_t *sm: pointer to signed message
*              - size_t smlen: length of signed message
*              - const uint8_t *pk: pointer to bit-packed public key
*
* Returns 0 if signed message could be verified correctly and -1 otherwise
**************************************************/
int crypto_sign_open(uint8_t *m,
                     size_t *mlen,
                     const uint8_t *sm,
                     size_t smlen,
                     const uint8_t *pk)
{
    size_t i;

    if(smlen < CRYPTO_BYTES)
        goto badsig;

    *mlen = smlen - CRYPTO_BYTES;
    if(crypto_sign_verify(sm, CRYPTO_BYTES, sm + CRYPTO_BYTES, *mlen, pk))
        goto badsig;
    else {
        /* All good, copy msg, return 0 */
        for(i = 0; i < *mlen; ++i)
            m[i] = sm[CRYPTO_BYTES + i];
        return 0;
    }

badsig:
    /* Signature verification failed */
    *mlen = -1;
    for(i = 0; i < smlen; ++i)
        m[i] = 0;

    return -1;
}
