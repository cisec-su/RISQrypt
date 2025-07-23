#include <stdint.h>
#include "params.h"
#include "masked_sign.h"
#include "masked_packing.h"
#include "masked_polyvec.h"
#include "masked_poly.h"
#include "randombytes.h"
#include "keccak.h"
#include "masked_symmetric.h"
#include "util.h"

#define Z_CHECK_ITERATIVE
#define W0CS2_CHECK_ITERATIVE


void unmask_and_print_coeffs(masked_poly *a, const char *name) {
  unsigned int i;
  print_string(name);
  print_string(" coefficients:\n");
  for(i = 0; i < 8; ++i) {
    print_u32(i);
    print_string(":\t");
    print_u32((a->share[0].coeffs[i] + a->share[1].coeffs[i]) % Q);
    print_string("\n");
  }
  print_string("\n");
}

void unmask_and_print_coeffs_bool(masked_poly *a, const char *name) {
  unsigned int i;
  print_string(name);
  print_string(" coefficients:\n");
  for(i = 0; i < 8; ++i) {
    print_u32(i);
    print_string(":\t");
    print_u32((a->share[0].coeffs[i] ^ a->share[1].coeffs[i]));
    print_string("\n");
  }
  print_string("\n");
}


void unmask_and_print_coeffs_2k(masked_poly *a, const char *name) {
  unsigned int i;
  print_string(name);
  print_string(" coefficients:\n");
  for(i = 0; i < 8; ++i) {
    print_u32(i);
    print_string(":\t");
    print_u32((a->share[0].coeffs[i] + a->share[1].coeffs[i]));
    print_string("\n");
  }
  print_string("\n");
}

void unmask_and_print_coeffs_2k_sub(masked_poly *a, const char *name) {
  unsigned int i;
  print_string(name);
  print_string(" coefficients:\n");
  for(i = 0; i < 8; ++i) {
    print_u32(i);
    print_string(":\t");
    print_u32((a->share[0].coeffs[i] - a->share[1].coeffs[i]));
    print_string("\n");
  }
  print_string("\n");
}


uint8_t buff[512];
void unmask_and_print(uint8_t *src_0, uint8_t *src_1, size_t len, const char *name) {
  unsigned int i;
  for(i = 0; i < len; ++i) {
    buff[i] = src_0[i] ^ src_1[i];
  }
  print_string(name);
  print_string("\n");
  print_hex(buff, len, 0);
  print_string("\n");
}


int masked_crypto_sign_signature(uint8_t *sig,
                                 size_t *siglen,
                                 const uint8_t *m,
                                 size_t mlen,
                                 const uint8_t *sk)
{
  unsigned int n;
  uint8_t seedbuf[2*SEEDBYTES + CRHBYTES];
  // uint8_t *rho, *tr, *key, *mu, *rhoprime;
  uint8_t *rho, *mu, *tr;
  masked_seed key;
  masked_crh rhoprime;
  uint16_t nonce = 0;
  polyvecl mat[K];//, s1, y, z;
  // polyveck t0, s2, w1, w0, h;
  masked_polyveck t0, s2, w1, w0;
  masked_polyvecl s1, y, z;
  poly cp;
  int flag;
  rho = seedbuf;
  tr = rho + SEEDBYTES;
  mu = tr + SEEDBYTES;
  poly_init_q();

  masked_unpack_sk(rho, tr, key, &t0, &s1, &s2, sk);

  // unmask_and_print_coeffs(&t0.vec[0], "t0.vec[0]");
  // unmask_and_print_coeffs(&s1.vec[0], "s1.vec[0]");
  // unmask_and_print_coeffs(&s1.vec[1], "s1.vec[1]");  
  // unmask_and_print_coeffs(&s2.vec[0], "s2.vec[0]");

  /* Compute mu = CRH(tr | msg) rename  */
  dilithium_shake256_absorb_double(mu, CRHBYTES, tr, SEEDBYTES, m, mlen);

#ifdef DILITHIUM_RANDOMIZED_SIGNING
  #error "Randomized signing not supported in masked version"
#else
  /* Compute rhoprime = SHAKE256(key | mu) */
  dilithium_masked_shake256_absorb_double((masked_flat_ptr) rhoprime, CRHBYTES, (masked_flat_ptr) key, SEEDBYTES, mu, CRHBYTES);
#endif

  // unmask_and_print(rhoprime[0], rhoprime[1], CRHBYTES, "rhoprime");


  /* Expand matrix and transform vectors */
  polyvec_matrix_expand(mat, rho);

  poly_init_ntt();
  masked_polyvecl_ntt(&s1);
  masked_polyveck_ntt(&s2); 
  masked_polyveck_ntt(&t0); 

  // unmask_and_print_coeffs(&s1.vec[0], "s1.vec[0]");
  // unmask_and_print_coeffs(&s1.vec[1], "s1.vec[1]");  
  // unmask_and_print_coeffs(&s2.vec[0], "s2.vec[0]");
  // unmask_and_print_coeffs(&s2.vec[1], "s2.vec[1]");  
  // unmask_and_print_coeffs(&t0.vec[0], "t0.vec[0]");
  // unmask_and_print_coeffs(&t0.vec[1], "t0.vec[1]");  


  rej:
    /* Sample intermediate vector y */
  masked_polyvecl_uniform_gamma1(&y, rhoprime, nonce++);

  // unmask_and_print_coeffs(&y.vec[0], "y.vec[0]");
  // unmask_and_print_coeffs(&y.vec[1], "y.vec[1]");

  /* Matrix-vector multiplication */ 
  poly_init_ntt(); // re-init NTT since uniform_gamma1 uses NTT-Lite
  masked_polyvecl_ntt(&y);

  // unmask_and_print_coeffs(&y.vec[0], "y.vec ntt[0]");
  // unmask_and_print_coeffs(&y.vec[1], "y.vec ntt[1]");
  // print_coeffs(&mat[0].vec[0], "mat0");

  masked_polyvec_matrix_pointwise(&w1, mat, &y);

  // unmask_and_print_coeffs(&w1.vec[0], "w1.vec[0]");
  // unmask_and_print_coeffs(&w1.vec[1], "w1.vec[1]");


  poly_init_invntt();
  masked_polyveck_invntt(&w1);

  // unmask_and_print_coeffs(&w1.vec[0], "w.vec intt[0]");
  // unmask_and_print_coeffs(&w1.vec[1], "w.vec intt[1]");


//   /* Decompose w and call the random oracle */
  masked_polyveck_decompose(&w1, &w0, &w1);

  // print_coeffs(&w1.vec[0], "w1.vec intt[0]");
  // print_coeffs(&w1.vec[1], "w1.vec intt[1]");
  // unmask_and_print_coeffs(&w0.vec[0], "w0.vec intt[0]");
  // unmask_and_print_coeffs(&w0.vec[1], "w0.vec intt[1]");


  polyveck_pack_w1(sig, &w1);
  dilithium_shake256_absorb_double(sig, SEEDBYTES,  mu, CRHBYTES, sig, K*POLYW1_PACKEDBYTES);
  // print_hex_with_label("c", sig, SEEDBYTES);

  poly_challenge(&cp, sig);

  poly_init_ntt();

  poly_ntt(&cp);

  print_coeffs(&cp, "cp");


  /* Compute z, reject if it reveals secret */
  flag = masked_polyvecl_pointwise_add_invntt_chknorm(&z, &s1, &cp, &y, GAMMA1 - BETA);
  while(1);
  if(flag) {
    goto rej;
  }

//   /* w0 - cs2. Check that subtracting cs2 does not change high bits of w and low bits
//    * do not reveal secret information */
// #ifdef W0CS2_CHECK_ITERATIVE
//   flag = polyveck_pointwise_invntt_sub_chknorm(&w0, &s2, &cp, &w0, GAMMA2 - BETA);
// #else
//   polyveck_pointwise_poly(&h, &cp, &s2);
//   poly_init_invntt();
//   polyveck_invntt_sub(&w0, &w0, &h);
//   flag = polyveck_chknorm(&w0, GAMMA2 - BETA);
// #endif
//   if(flag) {
//      goto rej;
//   }

//   /* Compute hints for w1 */
//   polyveck_pointwise_poly(&h, &cp, &t0);
//   poly_init_invntt();
//   flag = polyveck_invntt_chknorm(&h, GAMMA2);
//   if(flag) {
//       goto rej;
//   }

//   n = polyveck_add_make_hint(&h, &w0, &w1, &h);
//   if(n > OMEGA) {
//       goto rej; // if this is rare, we can merge with packing
//   }

//   /* Write signature */
//   pack_sig(sig, sig, &z, &h);
//   *siglen = CRYPTO_BYTES;

//   return 0;
}
