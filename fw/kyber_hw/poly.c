#include <stdint.h>
#include "ntt_lite.h"
#include "params.h"
#include "cbd.h"
#include "symmetric.h"
#include "poly.h"


static const uint32_t psi[128] = {
	0x6c106c1, 0xa140a14, 0xcd90cd9, 0xa520a52, 0x2760276, 0x7690769, 0x3500350, 0x4260426, 0x77f077f, 0xc100c1, 0x31d031d, 0xae20ae2, 0xcbc0cbc, 0x2390239, 0x6d206d2, 0x1280128, 0x98f098f, 0x53b053b, 0x5c405c4, 0xbe60be6, 0x380038, 0x8c008c0, 0x5350535, 0x5920592, 0x82e082e, 0x2170217, 0xb420b42, 0x9590959, 0xb3f0b3f, 0x7b607b6, 0x3350335, 0x1210121, 0x14b014b, 0xcb50cb5, 0x6dc06dc, 0x4ad04ad, 0x9000900, 0x8e508e5, 0x8070807, 0x28a028a, 0x7b907b9, 0x9d109d1, 0x2780278, 0xb310b31, 0x210021, 0x5280528, 0x77b077b, 0x90f090f, 0x59b059b, 0x3270327, 0x1c401c4, 0x59e059e, 0xb340b34, 0x5fe05fe, 0x9620962, 0xa570a57, 0xa390a39, 0x5c905c9, 0x2880288, 0x9aa09aa, 0xc260c26, 0x4cb04cb, 0x38e038e, 0x110011, 0xac90ac9, 0x2470247, 0xa590a59, 0x6650665, 0x2d302d3, 0x8f008f0, 0x44c044c, 0x5810581, 0xa660a66, 0xcd10cd1, 0xe900e9, 0x2f402f4, 0x86c086c, 0xbc70bc7, 0xbea0bea, 0x6a706a7, 0x6730673, 0xae50ae5, 0x6fd06fd, 0x7370737, 0x3b803b8, 0x5b505b5, 0xa7f0a7f, 0x3ab03ab, 0x9040904, 0x9850985, 0x9540954, 0x2dd02dd, 0x9210921, 0x10c010c, 0x2810281, 0x6300630, 0x8fa08fa, 0x7f507f5, 0xc940c94, 0x1770177, 0x9f509f5, 0x82a082a, 0x66d066d, 0x4270427, 0x13f013f, 0xad50ad5, 0x2f502f5, 0x8330833, 0x2310231, 0x9a209a2, 0xa220a22, 0xaf40af4, 0x4440444, 0x1930193, 0x4020402, 0x4770477, 0x8660866, 0xad70ad7, 0x3760376, 0x6ba06ba, 0x4bc04bc, 0x7520752, 0x4050405, 0x83e083e, 0xb770b77, 0x3750375, 0x86a086a, 0x1
	};

static const uint32_t psi_inv[128] = {
  0x3200320, 0x140014, 0x7f707f7, 0xb590b59, 0x2cc02cc, 0xbc60bc6, 0x7d807d8, 0x9980998, 0x5640564, 0x6a306a3, 0x7900790, 0x4f204f2, 0x6200620, 0x2c102c1, 0xaee0aee, 0x4e604e6, 0x9260926, 0xe100e1, 0x1d401d4, 0x7600760, 0x5750575, 0x8ea08ea, 0xa380a38, 0x3e603e6, 0x8a108a1, 0xce50ce5, 0x70e070e, 0xa1f0a1f, 0x3e303e3, 0x1b901b9, 0xc6d0c6d, 0xb3a0b3a, 0x41b041b, 0x6ee06ee, 0x82c082c, 0xbbd0bbd, 0x39c039c, 0x1640164, 0x1550155, 0x8500850, 0xa020a02, 0x7670767, 0xa320a32, 0xc1f0c1f, 0x4ed04ed, 0x3b303b3, 0x1f901f9, 0x2c302c3, 0xa6d0a6d, 0x6700670, 0xe800e8, 0xbc50bc5, 0x1980198, 0x2a402a4, 0xbbc0bbc, 0x27d027d, 0x20e020e, 0x8810881, 0x42a042a, 0x9930993, 0x260026, 0x5db05db, 0x5f005f0, 0x8cc08cc, 0x4c604c6, 0xc500c5, 0x8e208e2, 0x47e047e, 0x9580958, 0xaa30aa3, 0x9a409a4, 0xb460b46, 0x1150115, 0x8ce08ce, 0x4450445, 0xb000b00, 0x5b705b7, 0xadf0adf, 0x7870787, 0x7f007f0, 0x8300830, 0x5680568, 0x2670267, 0x5060506, 0x1160116, 0x5e105e1, 0x46d046d, 0x34a034a, 0x8ec08ec, 0x1860186, 0x5c505c5, 0x6b706b7, 0x2860286, 0x8840884, 0x9e909e9, 0x5400540, 0xc7b0c7b, 0x1f001f0, 0x5120512, 0x8570857, 0x1be01be, 0x87f087f, 0x4ab04ab, 0x1410141, 0x3a603a6, 0xb250b25, 0x2e502e5, 0x3020302, 0x10e010e, 0x3470347, 0x32d032d, 0x70c070c, 0x9d009d, 0x8cb08cb, 0xb870b87, 0x60c060c, 0x180018, 0x7ce07ce, 0x3c003c0, 0xadb0adb, 0x8890889, 0x5170517, 0x34e034e, 0x1540154, 0x55d055d, 0x11c011c, 0x6780678, 1
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
* Name:        poly_tobytes
*
* Description: Serialization of a polynomial
*
* Arguments:   - uint8_t *r: pointer to output byte array
*                            (needs space for KYBER_POLYBYTES bytes)
*              - poly *a:    pointer to input polynomial
**************************************************/
void poly_tobytes_fromhw(uint8_t r[KYBER_POLYBYTES])
{
  ntt_lite_encode((uint32_t*) r, NTT_LITE_INPUT_DIS, 12);
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

void poly_set_ctrl() {
  ntt_lite_set_ctrl(7, 12, NTT_LITE_MODE_POLY);
}

void poly_init_q() {
  const uint32_t q = (KYBER_Q << 16) | KYBER_Q;
  const uint32_t mu[2] = {0x13afb7, 0x13afb7};
  const uint32_t inv2 = 0x06810681;
  ntt_lite_load_q(q, mu, 7, 12, inv2, NTT_LITE_MODE_POLY);
}

void poly_set_inv2() {
  const uint32_t inv2 = 0x06810681;
  ntt_lite_set_inv2(inv2);
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


void poly_add_chain_pack_dv_fromhw(uint8_t r[KYBER_POLYCOMPRESSEDBYTES], const poly *b, const poly *c)
{
  ntt_lite_add(NTT_LITE_OUTPUT_DIS, NTT_LITE_INPUT_DIS, (uint32_t*) b->coeffs);
  ntt_lite_add(NTT_LITE_OUTPUT_DIS, NTT_LITE_INPUT_DIS, (uint32_t*) c->coeffs);
  ntt_lite_compress(NTT_LITE_OUTPUT_DIS, NTT_LITE_INPUT_DIS, KYBER_DV);
  ntt_lite_encode((uint32_t*) r, NTT_LITE_INPUT_DIS, KYBER_DV);
}


void poly_add_pack_du_fromhw(uint8_t *r, const poly *b)
{
  ntt_lite_add(NTT_LITE_OUTPUT_DIS, NTT_LITE_INPUT_DIS, (uint32_t*) b->coeffs);
  ntt_lite_compress(NTT_LITE_OUTPUT_DIS, NTT_LITE_INPUT_DIS, KYBER_DU);
  ntt_lite_encode((uint32_t*) r, NTT_LITE_INPUT_DIS, KYBER_DU);
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


void poly_sub_tomsg_fromhw(uint8_t msg[KYBER_INDCPA_MSGBYTES], const poly *a)
{
  ntt_lite_sub_rev(NTT_LITE_OUTPUT_DIS, NTT_LITE_INPUT_DIS, (uint32_t*) a->coeffs);
  ntt_lite_compress(NTT_LITE_OUTPUT_DIS, NTT_LITE_INPUT_DIS, 1);
  ntt_lite_encode((uint32_t*) msg, NTT_LITE_INPUT_DIS, 1);
}
