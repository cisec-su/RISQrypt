#include <stdint.h>
#include <stddef.h>
#include "params.h"
#include "poly.h"
#include "symmetric.h"
#include "ntt_lite.h"
#include "util.h"

#define INV2 0x8001 


void poly_init_q() {
    const uint32_t q = Q;
    const uint32_t mu[2] = {0x0000ffff, 0x0000ffff}; // mu = floor(2^{2*word_size} / q) 
    const uint32_t inv2 = INV2; 
    ntt_lite_load_q(q, mu, 7, 17, inv2, NTT_LITE_MODE_SINGLE); //pasta
}

void poly_set_q() {
    ntt_lite_set_q(Q);
}

/*************************************************
* Name:        poly_add
*
* Description: Add polynomials. No modular reduction is performed.
*
* Arguments:   - poly *c: pointer to output polynomial
*              - const poly *a: pointer to first summand
*              - const poly *b: pointer to second summand
**************************************************/
void poly_add(poly *c, const poly *a, const poly *b) {
    ntt_lite_add((uint32_t*)c->coeffs, (uint32_t*)a->coeffs, (uint32_t*)b->coeffs);
}


/*************************************************
* Name:        poly_sub
*
* Description: Subtract polynomials. No modular reduction is
*              performed.
*
* Arguments:   - poly *c: pointer to output polynomial
*              - const poly *a: pointer to first input polynomial
*              - const poly *b: pointer to second input polynomial to be
*                               subtraced from first input polynomial
**************************************************/
void poly_sub(poly *c, const poly *a, const poly *b) {    
    ntt_lite_sub((uint32_t*) c->coeffs, (uint32_t*) a->coeffs, (uint32_t*) b->coeffs);
}


/*************************************************
* Name:        poly_ntt
*
* Description: Inplace forward NTT. Coefficients can grow by
*              8*Q in absolute value.
*
* Arguments:   - poly *a: pointer to input/output polynomial
**************************************************/
void poly_ntt(poly *a) {
    ntt_lite_forward_ntt((uint32_t*)a->coeffs, (uint32_t*)a->coeffs);
}

/*************************************************
* Name:        poly_invntt
*
* Description: Inplace inverse NTT and multiplication by 2^{32}.
*              Input coefficients need to be less than Q in absolute
*              value and output coefficients are again bounded by Q.
*
* Arguments:   - poly *a: pointer to input/output polynomial
**************************************************/
void poly_invntt(poly *a) {
    ntt_lite_backward_ntt((uint32_t*)a->coeffs, (uint32_t*)a->coeffs);
}


void poly_invntt_sub(poly *a, poly *b, poly *c) {
    ntt_lite_backward_ntt(NTT_LITE_OUTPUT_DIS, c->coeffs);
    ntt_lite_sub_rev(a->coeffs, NTT_LITE_INPUT_DIS, b->coeffs);
}

/*************************************************
* Name:        poly_pointwise
*
* Description: Pointwise multiplication of polynomials in NTT domain
*              representation and multiplication of resulting polynomial
*              by 2^{-32}.
*
* Arguments:   - poly *c: pointer to output polynomial
*              - const poly *a: pointer to first input polynomial
*              - const poly *b: pointer to second input polynomial
**************************************************/
void poly_pointwise(poly *c, const poly *a, const poly *b) {
    ntt_lite_pwm((uint32_t*)c->coeffs, (uint32_t*)a->coeffs, (uint32_t*)b->coeffs); 
}

/*************************************************
* Name:        poly_pointwise_acc
*
* Description: Pointwise multiplication of polynomials in NTT domain
*              representation, multiplication of resulting polynomial
*              by 2^{-32} and accumulate.
*
* Arguments:   - poly *c: pointer to output (accumulating) polynomial
*              - const poly *a: pointer to first input polynomial
*              - const poly *b: pointer to second input polynomial
**************************************************/
void poly_pointwise_acc(poly *c, const poly *a, const poly *b) {
    ntt_lite_pwm(NTT_LITE_OUTPUT_DIS, (uint32_t*)a->coeffs, (uint32_t*)b->coeffs);
    ntt_lite_add((uint32_t*)c->coeffs, NTT_LITE_INPUT_DIS,(uint32_t*)c->coeffs);
}


/*************************************************
* Name:        poly_uniform
*
* Description: Sample polynomial with uniformly random coefficients
*              in [0,Q-1] by performing rejection sampling on the
*              output stream of SHAKE256(nonce|block_ctr|poly_ctr) 
*
* Arguments:   - poly *a: pointer to output polynomial
*              - const uint8_t nonce[]: byte array with nonce of length nonceBYTES
*              - uint16_t block_ctr: 2-byte block_ctr
**************************************************/
void poly_uniform(poly *a, uint64_t nonce, uint64_t block_ctr, uint8_t poly_ctr, int allow_zero)
{

    poly b;
    unsigned int buflen = 4*STREAM128_BLOCKBYTES;
    uint32_t buf[(STREAM128_BLOCKBYTES>>2)*4]; // 316 -> 128

    if (allow_zero == 0) {
        stream128_init(nonce, block_ctr, poly_ctr);
        stream128_squeeze((uint8_t*) buf, (N >> 1) << 2);
        ntt_lite_decode(NTT_LITE_OUTPUT_DIS, buf, 16);
        ntt_lite_set_bound(1); // add +1
        ntt_lite_add_const((uint32_t*)a->coeffs,NTT_LITE_INPUT_DIS);
    } else {

#ifdef REJ_SAMP_DIS
        stream128_init(nonce, block_ctr, poly_ctr);
        stream128_squeeze((uint8_t*) buf, (N >> 1) << 2);
        ntt_lite_decode((uint32_t*)a->coeffs, buf, 16);
#else 
        stream128_init(nonce, block_ctr, poly_ctr);
        stream128_squeezeblocks((uint8_t*) buf, 4);
        ntt_lite_set_inv2((STREAM128_BLOCKBYTES>>2)*4); //input size 1008 --todo! bunlari bir defa set edebilirsin
        ntt_lite_set_bound(Q);
        ntt_lite_rejsamp((uint32_t*) a->coeffs, buf, 17, NTT_LITE_REJSAMP_CENTER_DIS);
#endif

    }
}
