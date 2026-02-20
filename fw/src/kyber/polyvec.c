#include <stdint.h>
#include "ntt_lite.h"
#include "params.h"
#include "poly.h"
#include "symmetric.h"
#include "polyvec.h"
#include "pack.h"

/*************************************************
* Name:        polyvec_compress
*
* Description: Compress and serialize vector of polynomials
*
* Arguments:   - uint8_t *r: pointer to output byte array
*                            (needs space for KYBER_POLYVECCOMPRESSEDBYTES)
*              - polyvec *a: pointer to input vector of polynomials
**************************************************/
void polyvec_compress(uint8_t r[KYBER_POLYVECCOMPRESSEDBYTES], polyvec *a)
{
  ntt_lite_compress(NTT_LITE_OUTPUT_DIS, (uint32_t*) &(a->vec[0]), KYBER_DU);
  ntt_lite_encode((uint32_t*) r, NTT_LITE_INPUT_DIS, KYBER_DU);

  ntt_lite_compress(NTT_LITE_OUTPUT_DIS, (uint32_t*) &(a->vec[1]), KYBER_DU);
  ntt_lite_encode((uint32_t*) (r + KYBER_POLYVECCOMPRESSEDBYTES/3), NTT_LITE_INPUT_DIS, KYBER_DU);

  ntt_lite_compress(NTT_LITE_OUTPUT_DIS, (uint32_t*) &(a->vec[2]), KYBER_DU);
  ntt_lite_encode((uint32_t*) (r + 2*KYBER_POLYVECCOMPRESSEDBYTES/3), NTT_LITE_INPUT_DIS, KYBER_DU);
}

/*************************************************
* Name:        polyvec_decompress
*
* Description: De-serialize and decompress vector of polynomials;
*              approximate inverse of polyvec_compress
*
* Arguments:   - polyvec *r:       pointer to output vector of polynomials
*              - const uint8_t *a: pointer to input byte array
*                                  (of length KYBER_POLYVECCOMPRESSEDBYTES)
**************************************************/
void polyvec_decompress(polyvec *r, const uint8_t a[KYBER_POLYVECCOMPRESSEDBYTES])
{
  ntt_lite_decode(NTT_LITE_OUTPUT_DIS, (uint32_t*) a, KYBER_DU);
  ntt_lite_decompress((uint32_t*) &(r->vec[0]), NTT_LITE_INPUT_DIS, KYBER_DU);

  ntt_lite_decode(NTT_LITE_OUTPUT_DIS, (uint32_t*) (a + KYBER_POLYVECCOMPRESSEDBYTES/3), KYBER_DU);
  ntt_lite_decompress((uint32_t*) &(r->vec[1]), NTT_LITE_INPUT_DIS, KYBER_DU);

  ntt_lite_decode(NTT_LITE_OUTPUT_DIS, (uint32_t*) (a + 2*KYBER_POLYVECCOMPRESSEDBYTES/3), KYBER_DU);
  ntt_lite_decompress((uint32_t*) &(r->vec[2]), NTT_LITE_INPUT_DIS, KYBER_DU);
}

/*************************************************
* Name:        polyvec_tobytes
*
* Description: Serialize vector of polynomials
*
* Arguments:   - uint8_t *r: pointer to output byte array
*                            (needs space for KYBER_POLYVECBYTES)
*              - polyvec *a: pointer to input vector of polynomials
**************************************************/
void polyvec_tobytes(uint8_t r[KYBER_POLYVECBYTES], polyvec *a)
{
  unsigned int i;
  for(i=0;i<KYBER_K;i++)
    poly_tobytes(r+i*KYBER_POLYBYTES, &a->vec[i]);
}

/*************************************************
* Name:        polyvec_frombytes
*
* Description: De-serialize vector of polynomials;
*              inverse of polyvec_tobytes
*
* Arguments:   - uint8_t *r:       pointer to output byte array
*              - const polyvec *a: pointer to input vector of polynomials
*                                  (of length KYBER_POLYVECBYTES)
**************************************************/
void polyvec_frombytes(polyvec *r, const uint8_t a[KYBER_POLYVECBYTES])
{
  unsigned int i;
  for(i=0;i<KYBER_K;i++)
    poly_frombytes(&r->vec[i], a+i*KYBER_POLYBYTES);
}

/*************************************************
* Name:        polyvec_ntt
*
* Description: Apply forward NTT to all elements of a vector of polynomials
*
* Arguments:   - polyvec *r: pointer to in/output vector of polynomials
**************************************************/
void polyvec_ntt(polyvec *r)
{
  unsigned int i;
  for(i=0;i<KYBER_K;i++)
    poly_ntt(&r->vec[i]);
}


void polyvec_unpack_ntt(polyvec *r, const uint8_t a[KYBER_POLYVECCOMPRESSEDBYTES])
{
  unsigned int i;
  for(i=0;i<KYBER_K;i++) {
    ntt_lite_decode(NTT_LITE_OUTPUT_DIS, (uint32_t*) (a + i*KYBER_POLYVECCOMPRESSEDBYTES/3), KYBER_DU);
    ntt_lite_decompress(NTT_LITE_OUTPUT_DIS, NTT_LITE_INPUT_DIS, KYBER_DU);
    poly_init_ntt();
    ntt_lite_forward_ntt((uint32_t*) &r->vec[i].coeffs, NTT_LITE_INPUT_DIS);
  }
}


void polyvec_pointwise_acc_core(poly *r, const polyvec *a, const polyvec *b, int intt, int tohw, int clr)
{
  unsigned int i;
  uint32_t *dst;

  ntt_lite_pwm(NTT_LITE_OUTPUT_DIS, (uint32_t*) &a->vec[0].coeffs, (uint32_t*) &b->vec[0].coeffs);

  for(i = 1; i < KYBER_K; i++) {
    if ((i == (KYBER_K - 1)) && !(tohw || intt)) {
      dst = (uint32_t*) r->coeffs;
    } else {
      dst = NTT_LITE_OUTPUT_DIS;
    }

    if ((i == (KYBER_K - 1)) && clr)
      ntt_lite_set_clr();

    ntt_lite_mac(dst, (uint32_t*) &a->vec[i].coeffs, (uint32_t*) &b->vec[i].coeffs);
  }
  if (tohw)
    dst = NTT_LITE_OUTPUT_DIS;
  else
    dst = (uint32_t*) r->coeffs;
  if (intt) {
    poly_init_invntt();  
    ntt_lite_backward_ntt(dst, NTT_LITE_INPUT_DIS);
  }
}


void polyvec_pointwise_acc_fromseed_core(poly *r, const uint8_t seed[KYBER_SYMBYTES], int j, const polyvec *b, int intt, int tohw, int clr, int transposed)
{
  unsigned int i;
  uint32_t *dst;
  int next_i, next_j;

  if (tohw)
    dst = NTT_LITE_OUTPUT_DIS;
  else
    dst = (uint32_t*) r->coeffs;

  ntt_lite_set_bound((KYBER_Q << 16) | (KYBER_Q));
  ntt_lite_set_inv2(XOF_BLOCKBYTES >> 2);
  gen_poly_tohw(seed, j, 1, transposed, 1);
  ntt_lite_pwm((uint32_t*) r->coeffs, NTT_LITE_INPUT_DIS, (uint32_t*) &b->vec[0].coeffs);

  for(i = 1; i < KYBER_K; i++) {

    if (i == (KYBER_K - 1)) {
      next_i = 0;
      next_j = j + 1;
    }
    else {
      next_i = i + 1;
      next_j = j;
    }

    gen_poly_tohw(seed, next_j, next_i, transposed, (next_j != KYBER_K));
    ntt_lite_pwm(NTT_LITE_OUTPUT_DIS, NTT_LITE_INPUT_DIS, (uint32_t*) &b->vec[i].coeffs);
    if ((i == (KYBER_K - 1)) && clr)
      ntt_lite_set_clr_with_twiddle();
    if ((i == (KYBER_K - 1)) && (tohw || intt)) {
      ntt_lite_add(NTT_LITE_OUTPUT_DIS, NTT_LITE_INPUT_DIS, (uint32_t*) r->coeffs);
    } else {
      ntt_lite_add((uint32_t*) r->coeffs, NTT_LITE_INPUT_DIS, (uint32_t*) r->coeffs);      
    }
  }
  poly_set_inv2();
  if (intt) {
    poly_init_invntt();
    ntt_lite_backward_ntt(dst, NTT_LITE_INPUT_DIS);
  }
}



/*************************************************
* Name:        polyvec_pointwise_acc
*
* Description: Pointwise multiply elements of a and b, accumulate into r,
*              and multiply by 2^-16.
*
* Arguments: - poly *r:          pointer to output polynomial
*            - const polyvec *a: pointer to first input vector of polynomials
*            - const polyvec *b: pointer to second input vector of polynomials
**************************************************/
void polyvec_pointwise_acc_invntt(poly *r, const polyvec *a, const polyvec *b)
{
  polyvec_pointwise_acc_core(r, a, b, 1, 0, 0);
}


void polyvec_pointwise_acc_fromseed_add_tobytes(uint8_t r[KYBER_POLYBYTES], const uint8_t seed[KYBER_SYMBYTES], int nonce_j, const polyvec *b, poly *e)
{
  poly temp;
  polyvec_pointwise_acc_fromseed_core(&temp, seed, nonce_j, b, 0, 1, 0, 0);
  ntt_lite_add(NTT_LITE_OUTPUT_DIS, NTT_LITE_INPUT_DIS, (uint32_t*) e->coeffs);
  poly_tobytes_fromhw(r);
}


void polyvec_pointwise_acc_invntt_fromseed_tohw(poly *r, const uint8_t seed[KYBER_SYMBYTES], int nonce_j, const polyvec *b)
{
  polyvec_pointwise_acc_fromseed_core(r, seed, nonce_j, b, 1, 1, 0, 1);
}



void polyvec_pointwise_acc_invntt_tohw(poly *r, const polyvec *a, const polyvec *b)
{
  polyvec_pointwise_acc_core(r, a, b, 1, 1, 0);
}

void polyvec_pointwise_acc(poly *r, const polyvec *a, const polyvec *b)
{
  polyvec_pointwise_acc_core(r, a, b, 0, 0, 0);
}


void polyvec_pointwise_acc_invntt_frombytes_tohw(poly *r, const uint8_t a[KYBER_POLYVECBYTES], const polyvec *b)
{
  unsigned int i;
  uint32_t *dst;
  
  ntt_lite_decode(NTT_LITE_OUTPUT_DIS, (uint32_t*) a, 12);
  ntt_lite_pwm((uint32_t*) r->coeffs, NTT_LITE_INPUT_DIS, (uint32_t*) &b->vec[0].coeffs);

  for(i = 1; i < KYBER_K; i++) {
    ntt_lite_decode(NTT_LITE_OUTPUT_DIS, (uint32_t*) (a + i*(KYBER_POLYBYTES)), 12);
    ntt_lite_pwm(NTT_LITE_OUTPUT_DIS, NTT_LITE_INPUT_DIS, (uint32_t*) &b->vec[i].coeffs);
    if (i == (KYBER_K - 1)) {
      ntt_lite_add(NTT_LITE_OUTPUT_DIS, NTT_LITE_INPUT_DIS, (uint32_t*) r->coeffs);
    } else {
      ntt_lite_add((uint32_t*) r->coeffs, NTT_LITE_INPUT_DIS, (uint32_t*) r->coeffs);      
    }
  }
  poly_init_invntt();  
  ntt_lite_backward_ntt(NTT_LITE_OUTPUT_DIS, NTT_LITE_INPUT_DIS);
}


/*************************************************
* Name:        polyvec_add
*
* Description: Add vectors of polynomials
*
* Arguments: - polyvec *r:       pointer to output vector of polynomials
*            - const polyvec *a: pointer to first input vector of polynomials
*            - const polyvec *b: pointer to second input vector of polynomials
**************************************************/
void polyvec_add(polyvec *r, const polyvec *a, const polyvec *b)
{
  unsigned int i;
  for(i=0;i<KYBER_K;i++)
    poly_add(&r->vec[i], &a->vec[i], &b->vec[i]);
}


void polyvec_add_pack(uint8_t r[KYBER_POLYVECCOMPRESSEDBYTES], const polyvec *a, const polyvec *b)
{
  unsigned int i;
  for(i=0;i<KYBER_K;i++) {
    ntt_lite_add(NTT_LITE_OUTPUT_DIS, (uint32_t*) &a->vec[i].coeffs, (uint32_t*) &b->vec[i].coeffs);
    ntt_lite_compress(NTT_LITE_OUTPUT_DIS, NTT_LITE_INPUT_DIS, KYBER_DU);
    ntt_lite_encode((uint32_t*) (r + i*(KYBER_POLYVECCOMPRESSEDBYTES/3)), NTT_LITE_INPUT_DIS, KYBER_DU);
  }
}
