#include <stddef.h>
#include <stdint.h>
#include "params.h"
#include "indcpa.h"
#include "poly.h"
#include "polyvec.h"
#include "rng.h"
#include "symmetric.h"
#include "pack.h"


/*************************************************
* Name:        indcpa_keypair
*
* Description: Generates public and private key for the CPA-secure
*              public-key encryption scheme underlying Kyber
*
* Arguments:   - uint8_t *pk: pointer to output public key
*                             (of length KYBER_INDCPA_PUBLICKEYBYTES bytes)
*              - uint8_t *sk: pointer to output private key
                              (of length KYBER_INDCPA_SECRETKEYBYTES bytes)
**************************************************/
void indcpa_keypair(uint8_t pk[KYBER_INDCPA_PUBLICKEYBYTES],
                    uint8_t sk[KYBER_INDCPA_SECRETKEYBYTES])
{
  unsigned int i;
  uint8_t buf[2*KYBER_SYMBYTES] __attribute__((aligned(4)));
  const uint8_t *publicseed = buf;
  const uint8_t *noiseseed = buf+KYBER_SYMBYTES;
  uint8_t nonce = 0;
  polyvec a[KYBER_K], e, skpv;

  poly_init_q();
  poly_init_zeta();

  rng_init();
  randombytes(buf, KYBER_SYMBYTES);
  hash_g(buf, buf, KYBER_SYMBYTES);

  prf_absorb(noiseseed, nonce++);
  for(i = 0; i < KYBER_K; i++)
    poly_getnoise_eta1_fromhw(&skpv.vec[i], noiseseed, nonce++, 1);
  for(i = 0; i < KYBER_K; i++)
    poly_getnoise_eta1_fromhw(&e.vec[i], noiseseed, nonce++, i != (KYBER_K - 1));

  poly_init_ntt();
  polyvec_ntt(&skpv);
  polyvec_ntt(&e);

  // matrix-vector multiplication
  absorb_routine(publicseed, 0, 0, 0);
  for(i=0;i<KYBER_K;i++) {
    polyvec_pointwise_acc_fromseed_add_tobytes(pk + i*KYBER_POLYBYTES, publicseed, i, &skpv, &e.vec[i]);
  }

  pack_sk(sk, &skpv);
  for(i=0;i<KYBER_SYMBYTES;i++)
    pk[i+KYBER_POLYVECBYTES] = publicseed[i];
}

/*************************************************
* Name:        indcpa_enc
*
* Description: Encryption function of the CPA-secure
*              public-key encryption scheme underlying Kyber.
*
* Arguments:   - uint8_t *c:           pointer to output ciphertext
*                                      (of length KYBER_INDCPA_BYTES bytes)
*              - const uint8_t *m:     pointer to input message
*                                      (of length KYBER_INDCPA_MSGBYTES bytes)
*              - const uint8_t *pk:    pointer to input public key
*                                      (of length KYBER_INDCPA_PUBLICKEYBYTES)
*              - const uint8_t *coins: pointer to input random coins
*                                      used as seed (of length KYBER_SYMBYTES)
*                                      to deterministically generate all
*                                      randomness
**************************************************/
void indcpa_enc(uint8_t c[KYBER_INDCPA_BYTES],
                const uint8_t m[KYBER_INDCPA_MSGBYTES],
                const uint8_t pk[KYBER_INDCPA_PUBLICKEYBYTES],
                const uint8_t coins[KYBER_SYMBYTES])
{
  unsigned int i;
  const uint8_t *seed = pk + KYBER_POLYVECBYTES;
  uint8_t nonce = 0;
  polyvec sp, ep, at[KYBER_K], bp;
  poly v, k, epp;

  poly_init_q();
  poly_init_zeta();

  poly_frommsg(&k, m);

  prf_absorb(coins, nonce++);
  for(i = 0; i < KYBER_K; i++)
    poly_getnoise_eta1_fromhw(sp.vec+i, coins, nonce++, 1);

  for(i = 0; i < KYBER_K; i++)
    poly_getnoise_eta2_fromhw(ep.vec+i, coins, nonce++, 1);
  poly_getnoise_eta2_fromhw(&epp, coins, nonce++, 0);

  poly_init_ntt();
  polyvec_ntt(&sp);

  // // matrix-vector multiplication
  absorb_routine(seed, 0, 0, 0);
  for(i = 0; i < KYBER_K; i++) {
    polyvec_pointwise_acc_invntt_fromseed_tohw(&bp.vec[i], seed, i, &sp);
    poly_add_pack_du_fromhw(c + i*(KYBER_POLYVECCOMPRESSEDBYTES/KYBER_K), &ep.vec[i]);
  }

  polyvec_pointwise_acc_invntt_frombytes_tohw(&v, pk, &sp);
  poly_add_chain_pack_dv_fromhw(c + KYBER_POLYVECCOMPRESSEDBYTES, &epp, &k);

}


/*************************************************
* Name:        indcpa_dec
*
* Description: Decryption function of the CPA-secure
*              public-key encryption scheme underlying Kyber.
*
* Arguments:   - uint8_t *m:        pointer to output decrypted message
*                                   (of length KYBER_INDCPA_MSGBYTES)
*              - const uint8_t *c:  pointer to input ciphertext
*                                   (of length KYBER_INDCPA_BYTES)
*              - const uint8_t *sk: pointer to input secret key
*                                   (of length KYBER_INDCPA_SECRETKEYBYTES)
**************************************************/
void indcpa_dec(uint8_t m[KYBER_INDCPA_MSGBYTES],
                const uint8_t c[KYBER_INDCPA_BYTES],
                const uint8_t sk[KYBER_INDCPA_SECRETKEYBYTES])
{
  polyvec bp, skpv;
  poly v, mp;

  poly_init_q();

  poly_decompress(&v, c + KYBER_POLYVECCOMPRESSEDBYTES);
  
  polyvec_unpack_ntt(&bp, c);

  polyvec_pointwise_acc_invntt_frombytes_tohw(&mp, sk, &bp);

  poly_sub_tomsg_fromhw(m, &v);
}
