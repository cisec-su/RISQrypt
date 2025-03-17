#include <stdint.h>
#include "params.h"
#include "poly.h"
#include "polyvec.h"
#include "ntt_lite.h"

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
  ntt_lite_compress(NTT_LITE_OUTPUT_DIS, &(a->vec[0]), KYBER_DU);
  ntt_lite_encode(r, NTT_LITE_INPUT_DIS, KYBER_DU);

  ntt_lite_compress(NTT_LITE_OUTPUT_DIS, &(a->vec[1]), KYBER_DU);
  ntt_lite_encode(r + KYBER_POLYVECCOMPRESSEDBYTES/3, NTT_LITE_INPUT_DIS, KYBER_DU);

  ntt_lite_compress(NTT_LITE_OUTPUT_DIS, &(a->vec[2]), KYBER_DU);
  ntt_lite_encode(r + 2*KYBER_POLYVECCOMPRESSEDBYTES/3, NTT_LITE_INPUT_DIS, KYBER_DU);
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
void polyvec_decompress(polyvec *r,
                        const uint8_t a[KYBER_POLYVECCOMPRESSEDBYTES])
{
  ntt_lite_decode(NTT_LITE_OUTPUT_DIS, a, KYBER_DU);
  ntt_lite_decompress(&(r->vec[0]), NTT_LITE_INPUT_DIS, KYBER_DU);

  ntt_lite_decode(NTT_LITE_OUTPUT_DIS, a +   KYBER_POLYVECCOMPRESSEDBYTES/3, KYBER_DU);
  ntt_lite_decompress(&(r->vec[1]), NTT_LITE_INPUT_DIS, KYBER_DU);

  ntt_lite_decode(NTT_LITE_OUTPUT_DIS, a + 2*KYBER_POLYVECCOMPRESSEDBYTES/3, KYBER_DU);
  ntt_lite_decompress(&(r->vec[2]), NTT_LITE_INPUT_DIS, KYBER_DU);
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

/*************************************************
* Name:        polyvec_invntt_tomont
*
* Description: Apply inverse NTT to all elements of a vector of polynomials
*              and multiply by Montgomery factor 2^16
*
* Arguments:   - polyvec *r: pointer to in/output vector of polynomials
**************************************************/
void polyvec_invntt_tomont(polyvec *r)
{
  unsigned int i;
  for(i=0;i<KYBER_K;i++)
    poly_invntt_tomont(&r->vec[i]);
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
void polyvec_pointwise_acc_invntt(poly *r,
                                  const polyvec *a,
                                  const polyvec *b)
{
  unsigned int i;
  poly t;

  poly_basemul(r, &a->vec[0], &b->vec[0]);

  for(i = 1; i < KYBER_K; i++) {
    poly_basemul(&t, &a->vec[i], &b->vec[i]);
    if (i == (KYBER_K - 1)) {
      poly_add(NTT_LITE_OUTPUT_DIS, r, &t);
    } else {
      poly_add(r, r, &t);
    }
  }
  poly_init_invntt();  
  ntt_lite_backward_ntt(r->coeffs, NTT_LITE_INPUT_DIS);
}

/*************************************************
* Name:        polyvec_reduce
*
* Description: Applies Barrett reduction to each coefficient
*              of each element of a vector of polynomials
*              for details of the Barrett reduction see comments in reduce.c
*
* Arguments:   - poly *r: pointer to input/output polynomial
**************************************************/
void polyvec_reduce(polyvec *r)
{
  unsigned int i;
  for(i=0;i<KYBER_K;i++)
    poly_reduce(&r->vec[i]);
}

/*************************************************
* Name:        polyvec_csubq
*
* Description: Applies conditional subtraction of q to each coefficient
*              of each element of a vector of polynomials
*              for details of conditional subtraction of q see comments in
*              reduce.c
*
* Arguments:   - poly *r: pointer to input/output polynomial
**************************************************/
void polyvec_csubq(polyvec *r)
{
  unsigned int i;
  for(i=0;i<KYBER_K;i++)
    poly_csubq(&r->vec[i]);
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
