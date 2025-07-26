#include <stdint.h>
#include "ntt_lite.h"
#include "params.h"
#include "cbd.h"
#include "symmetric.h"
#include "reduce.h"
#include "poly.h"


static const uint32_t psi[128] = {
	0x6c1, 0xa14, 0xcd9, 0xa52, 0x276, 0x769, 0x350, 0x426, 0x77f, 0xc1, 0x31d, 0xae2, 0xcbc, 0x239, 0x6d2, 0x128, 0x98f, 0x53b, 0x5c4, 0xbe6, 0x38, 0x8c0, 0x535, 0x592, 0x82e, 0x217, 0xb42, 0x959, 0xb3f, 0x7b6, 0x335, 0x121, 0x14b, 0xcb5, 0x6dc, 0x4ad, 0x900, 0x8e5, 0x807, 0x28a, 0x7b9, 0x9d1, 0x278, 0xb31, 0x21, 0x528, 0x77b, 0x90f, 0x59b, 0x327, 0x1c4, 0x59e, 0xb34, 0x5fe, 0x962, 0xa57, 0xa39, 0x5c9, 0x288, 0x9aa, 0xc26, 0x4cb, 0x38e, 0x11, 0xac9, 0x247, 0xa59, 0x665, 0x2d3, 0x8f0, 0x44c, 0x581, 0xa66, 0xcd1, 0xe9, 0x2f4, 0x86c, 0xbc7, 0xbea, 0x6a7, 0x673, 0xae5, 0x6fd, 0x737, 0x3b8, 0x5b5, 0xa7f, 0x3ab, 0x904, 0x985, 0x954, 0x2dd, 0x921, 0x10c, 0x281, 0x630, 0x8fa, 0x7f5, 0xc94, 0x177, 0x9f5, 0x82a, 0x66d, 0x427, 0x13f, 0xad5, 0x2f5, 0x833, 0x231, 0x9a2, 0xa22, 0xaf4, 0x444, 0x193, 0x402, 0x477, 0x866, 0xad7, 0x376, 0x6ba, 0x4bc, 0x752, 0x405, 0x83e, 0xb77, 0x375, 0x86a, 0x1
	};

static const uint32_t psi_inv[128] = {
	0x320, 0x14, 0x7f7, 0xb59, 0x2cc, 0xbc6, 0x7d8, 0x998, 0x564, 0x6a3, 0x790, 0x4f2, 0x620, 0x2c1, 0xaee, 0x4e6, 0x926, 0xe1, 0x1d4, 0x760, 0x575, 0x8ea, 0xa38, 0x3e6, 0x8a1, 0xce5, 0x70e, 0xa1f, 0x3e3, 0x1b9, 0xc6d, 0xb3a, 0x41b, 0x6ee, 0x82c, 0xbbd, 0x39c, 0x164, 0x155, 0x850, 0xa02, 0x767, 0xa32, 0xc1f, 0x4ed, 0x3b3, 0x1f9, 0x2c3, 0xa6d, 0x670, 0xe8, 0xbc5, 0x198, 0x2a4, 0xbbc, 0x27d, 0x20e, 0x881, 0x42a, 0x993, 0x26, 0x5db, 0x5f0, 0x8cc, 0x4c6, 0xc5, 0x8e2, 0x47e, 0x958, 0xaa3, 0x9a4, 0xb46, 0x115, 0x8ce, 0x445, 0xb00, 0x5b7, 0xadf, 0x787, 0x7f0, 0x830, 0x568, 0x267, 0x506, 0x116, 0x5e1, 0x46d, 0x34a, 0x8ec, 0x186, 0x5c5, 0x6b7, 0x286, 0x884, 0x9e9, 0x540, 0xc7b, 0x1f0, 0x512, 0x857, 0x1be, 0x87f, 0x4ab, 0x141, 0x3a6, 0xb25, 0x2e5, 0x302, 0x10e, 0x347, 0x32d, 0x70c, 0x9d, 0x8cb, 0xb87, 0x60c, 0x18, 0x7ce, 0x3c0, 0xadb, 0x889, 0x517, 0x34e, 0x154, 0x55d, 0x11c, 0x678, 0x1
	};

static const uint32_t zetas[128] = {
  0x110000, 0xcf00000, 0xac90000, 0x2380000, 0x2470000, 0xaba0000, 0xa590000, 0x2a80000, 0x6650000, 0x69c0000, 0x2d30000, 0xa2e0000, 0x8f00000, 0x4110000, 0x44c0000, 0x8b50000, 0x5810000, 0x7800000, 0xa660000, 0x29b0000, 0xcd10000, 0x300000, 0xe90000, 0xc180000, 0x2f40000, 0xa0d0000, 0x86c0000, 0x4950000, 0xbc70000, 0x13a0000, 0xbea0000, 0x1170000, 0x6a70000, 0x65a0000, 0x6730000, 0x68e0000, 0xae50000, 0x21c0000, 0x6fd0000, 0x6040000, 0x7370000, 0x5ca0000, 0x3b80000, 0x9490000, 0x5b50000, 0x74c0000, 0xa7f0000, 0x2820000, 0x3ab0000, 0x9560000, 0x9040000, 0x3fd0000, 0x9850000, 0x37c0000, 0x9540000, 0x3ad0000, 0x2dd0000, 0xa240000, 0x9210000, 0x3e00000, 0x10c0000, 0xbf50000, 0x2810000, 0xa800000, 0x6300000, 0x6d10000, 0x8fa0000, 0x4070000, 0x7f50000, 0x50c0000, 0xc940000, 0x6d0000, 0x1770000, 0xb8a0000, 0x9f50000, 0x30c0000, 0x82a0000, 0x4d70000, 0x66d0000, 0x6940000, 0x4270000, 0x8da0000, 0x13f0000, 0xbc20000, 0xad50000, 0x22c0000, 0x2f50000, 0xa0c0000, 0x8330000, 0x4ce0000, 0x2310000, 0xad00000, 0x9a20000, 0x35f0000, 0xa220000, 0x2df0000, 0xaf40000, 0x20d0000, 0x4440000, 0x8bd0000, 0x1930000, 0xb6e0000, 0x4020000, 0x8ff0000, 0x4770000, 0x88a0000, 0x8660000, 0x49b0000, 0xad70000, 0x22a0000, 0x3760000, 0x98b0000, 0x6ba0000, 0x6470000, 0x4bc0000, 0x8450000, 0x7520000, 0x5af0000, 0x4050000, 0x8fc0000, 0x83e0000, 0x4c30000, 0xb770000, 0x18a0000, 0x3750000, 0x98c0000, 0x86a0000, 0x4970000
  };

/*************************************************
* Name:        poly_compress
*
* Description: Compression and subsequent serialization of a polynomial
*
* Arguments:   - uint8_t *r: pointer to output byte array
*                            (of length KYBER_POLYCOMPRESSEDBYTES)
*              - poly *a:    pointer to input polynomial
**************************************************/
void poly_compress(uint8_t r[KYBER_POLYCOMPRESSEDBYTES], poly *a)
{
  ntt_lite_compress(NTT_LITE_OUTPUT_DIS, (uint32_t*) a->coeffs, KYBER_DV);
  ntt_lite_encode((uint32_t*) r, NTT_LITE_INPUT_DIS, KYBER_DV);
}


void poly_decompress_compress(poly *r, const uint8_t a[KYBER_POLYCOMPRESSEDBYTES])
{
  ntt_lite_decode(NTT_LITE_OUTPUT_DIS, (uint32_t*) a, KYBER_DV);
  ntt_lite_decompress(NTT_LITE_OUTPUT_DIS, NTT_LITE_INPUT_DIS, KYBER_DV);
  ntt_lite_decompress((uint32_t*) r->coeffs, NTT_LITE_INPUT_DIS, KYBER_DV);
}



/*************************************************
* Name:        poly_decompress
*
* Description: De-serialization and subsequent decompression of a polynomial;
*              approximate inverse of poly_compress
*
* Arguments:   - poly *r:          pointer to output polynomial
*              - const uint8_t *a: pointer to input byte array
*                                  (of length KYBER_POLYCOMPRESSEDBYTES bytes)
**************************************************/
void poly_decompress(poly *r, const uint8_t a[KYBER_POLYCOMPRESSEDBYTES])
{
  ntt_lite_decode(NTT_LITE_OUTPUT_DIS, (uint32_t*) a, KYBER_DV);
  ntt_lite_decompress((uint32_t*) r->coeffs, NTT_LITE_INPUT_DIS, KYBER_DV);
}

/*************************************************
* Name:        poly_tobytes
*
* Description: Serialization of a polynomial
*
* Arguments:   - uint8_t *r: pointer to output byte array
*                            (needs space for KYBER_POLYBYTES bytes)
*              - poly *a:    pointer to input polynomial
**************************************************/
void poly_tobytes(uint8_t r[KYBER_POLYBYTES], poly *a)
{
  ntt_lite_encode((uint32_t*) r, (uint32_t*) a->coeffs, 12);
}

/*************************************************
* Name:        poly_frombytes
*
* Description: De-serialization of a polynomial;
*              inverse of poly_tobytes
*
* Arguments:   - poly *r:          pointer to output polynomial
*              - const uint8_t *a: pointer to input byte array
*                                  (of KYBER_POLYBYTES bytes)
**************************************************/
void poly_frombytes(poly *r, const uint8_t a[KYBER_POLYBYTES])
{
  ntt_lite_decode((uint32_t*) r->coeffs, (uint32_t*) a, 12);
}

/*************************************************
* Name:        poly_frommsg
*
* Description: Convert 32-byte message to polynomial
*
* Arguments:   - poly *r:            pointer to output polynomial
*              - const uint8_t *msg: pointer to input message
**************************************************/
void poly_frommsg(poly *r, const uint8_t msg[KYBER_INDCPA_MSGBYTES])
{
  ntt_lite_decode(NTT_LITE_OUTPUT_DIS, (uint32_t*) msg, 1);
  ntt_lite_decompress((uint32_t*) r->coeffs, NTT_LITE_INPUT_DIS, 1);
}

/*************************************************
* Name:        poly_tomsg
*
* Description: Convert polynomial to 32-byte message
*
* Arguments:   - uint8_t *msg: pointer to output message
*              - poly *a:      pointer to input polynomial
**************************************************/
void poly_tomsg(uint8_t msg[KYBER_INDCPA_MSGBYTES], poly *a)
{
  ntt_lite_compress(NTT_LITE_OUTPUT_DIS, (uint32_t*) a->coeffs, 1);
  ntt_lite_encode((uint32_t*) msg, NTT_LITE_INPUT_DIS, 1);
}

/*************************************************
* Name:        poly_getnoise_eta1
*
* Description: Sample a polynomial deterministically from a seed and a nonce,
*              with output polynomial close to centered binomial distribution
*              with parameter KYBER_ETA1
*
* Arguments:   - poly *r:             pointer to output polynomial
*              - const uint8_t *seed: pointer to input seed
*                                     (of length KYBER_SYMBYTES bytes)
*              - uint8_t nonce:       one-byte input nonce
**************************************************/
void poly_getnoise_eta1(poly *r, const uint8_t seed[KYBER_SYMBYTES], uint8_t nonce)
{
  uint8_t buf[KYBER_ETA1*KYBER_N/4];
  prf(buf, sizeof(buf), seed, nonce);
  cbd_eta1(r, buf);
}

/*************************************************
* Name:        poly_getnoise_eta2
*
* Description: Sample a polynomial deterministically from a seed and a nonce,
*              with output polynomial close to centered binomial distribution
*              with parameter KYBER_ETA2
*
* Arguments:   - poly *r:             pointer to output polynomial
*              - const uint8_t *seed: pointer to input seed
*                                     (of length KYBER_SYMBYTES bytes)
*              - uint8_t nonce:       one-byte input nonce
**************************************************/
void poly_getnoise_eta2(poly *r, const uint8_t seed[KYBER_SYMBYTES], uint8_t nonce)
{
  uint8_t buf[KYBER_ETA2*KYBER_N/4];
  prf(buf, sizeof(buf), seed, nonce);
  cbd_eta2(r, buf);
}

void poly_init_q() {
  const uint32_t q = KYBER_Q;
  const uint32_t mu = 0x13afb7; 
  const uint32_t inv2 = 0x681;
  ntt_lite_load_q(q, &mu, 7, 12, inv2, NTT_LITE_MODE_POLY);
}

void poly_init_ntt() {
	ntt_lite_load_twiddle((uint32_t*) psi);
}

void poly_init_invntt() {
	ntt_lite_load_twiddle((uint32_t*) psi_inv);
}

void poly_init_zeta() {
  ntt_lite_load_zeta((uint32_t*) zetas);
}

/*************************************************
* Name:        poly_ntt
*
* Description: Computes negacyclic number-theoretic transform (NTT) of
*              a polynomial in place;
*              inputs assumed to be in normal order, output in bitreversed order
*
* Arguments:   - uint16_t *r: pointer to in/output polynomial
**************************************************/
void poly_ntt(poly *r)
{
	ntt_lite_forward_ntt((uint32_t*) r->coeffs, (uint32_t*) r->coeffs);
}

/*************************************************
* Name:        poly_invntt
*
* Description: Computes inverse of negacyclic number-theoretic transform (NTT)
*              of a polynomial in place;
*              inputs assumed to be in bitreversed order, output in normal order
*
* Arguments:   - uint16_t *a: pointer to in/output polynomial
**************************************************/
void poly_invntt(poly *r)
{
	ntt_lite_backward_ntt((uint32_t*) r->coeffs, (uint32_t*) r->coeffs);
}

/*************************************************
* Name:        poly_basemul_montgomery
*
* Description: Multiplication of two polynomials in NTT domain
*
* Arguments:   - poly *r:       pointer to output polynomial
*              - const poly *a: pointer to first input polynomial
*              - const poly *b: pointer to second input polynomial
**************************************************/
void poly_basemul(poly *r, const poly *a, const poly *b)
{
  ntt_lite_pwm((uint32_t*) r->coeffs, (uint32_t*) a->coeffs, (uint32_t*) b->coeffs);
}


/*************************************************
* Name:        poly_add
*
* Description: Add two polynomials
*
* Arguments: - poly *r:       pointer to output polynomial
*            - const poly *a: pointer to first input polynomial
*            - const poly *b: pointer to second input polynomial
**************************************************/
void poly_add(poly *r, const poly *a, const poly *b)
{
  ntt_lite_add((uint32_t*) r->coeffs, (uint32_t*) a->coeffs, (uint32_t*) b->coeffs);
}


void poly_add_chain(poly *r, const poly *a, const poly *b, const poly *c)
{
  ntt_lite_add(NTT_LITE_OUTPUT_DIS, (uint32_t*) a->coeffs, (uint32_t*) b->coeffs);
  ntt_lite_add((uint32_t*) r->coeffs, NTT_LITE_INPUT_DIS, (uint32_t*) c->coeffs);
}


/*************************************************
* Name:        poly_sub
*
* Description: Subtract two polynomials
*
* Arguments: - poly *r:       pointer to output polynomial
*            - const poly *a: pointer to first input polynomial
*            - const poly *b: pointer to second input polynomial
**************************************************/
void poly_sub(poly *r, const poly *a, const poly *b)
{
  ntt_lite_sub((uint32_t*) r->coeffs, (uint32_t*) a->coeffs, (uint32_t*) b->coeffs);
}


void poly_sub_tomsg(uint8_t msg[KYBER_INDCPA_MSGBYTES], const poly *a, const poly *b)
{
  ntt_lite_sub(NTT_LITE_OUTPUT_DIS, (uint32_t*) a->coeffs, (uint32_t*) b->coeffs);
  ntt_lite_compress(NTT_LITE_OUTPUT_DIS, NTT_LITE_INPUT_DIS, 1);
  ntt_lite_encode((uint32_t*) msg, NTT_LITE_INPUT_DIS, 1);
}
