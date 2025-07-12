#include <stdint.h>
#include <stddef.h>
#include "params.h"
#include "poly.h"
#include "rounding.h"
#include "symmetric.h"
#include "keccak.h"
#include "ntt_lite.h"
#include "reduce.h"
#include "util.h"



const int32_t psi[N] = { 
   0x495e02, 0x397567, 0x396569, 0x4f062b, 0x53df73, 0x4fe033, 0x4f066b, 0x76b1ae, 0x360dd5, 0x28edb0, 0x207fe4, 0x397283, 0x70894a, 0x88192, 0x6d3dc8, 0x4c7294, 0x41e0b4, 0x28a3d2, 0x66528a, 0x4a18a7, 0x794034, 0xa52ee, 0x6b7d81, 0x4e9f1d, 0x1a2877, 0x2571df, 0x1649ee, 0x7611bd, 0x492bb7, 0x2af697, 0x22d8d5, 0x36f72a, 0x30911e, 0x29d13f, 0x492673, 0x50685f, 0x2010a2, 0x3887f7, 0x11b2c3, 0x603a4, 0xe2bed, 0x10b72c, 0x4a5f35, 0x1f9d15, 0x428cd4, 0x3177f4, 0x20e612, 0x341c1d, 0x1ad873, 0x736681, 0x49553f, 0x3952f6, 0x62564a, 0x65ad05, 0x439a1c, 0x53aa5f, 0x30b622, 0x87f38, 0x3b0e6d, 0x2c83da, 0x1c496e, 0x330e2b, 0x1c5b70, 0x2ee3f1, 0x137eb9, 0x57a930, 0x3ac6ef, 0x3fd54c, 0x4eb2ea, 0x503ee1, 0x7bb175, 0x2648b4, 0x1ef256, 0x1d90a2, 0x45a6d4, 0x2ae59b, 0x52589c, 0x6ef1f5, 0x3f7288, 0x175102, 0x75d59, 0x1187ba, 0x52aca9, 0x773e9e, 0x296d8, 0x2592ec, 0x4cff12, 0x404ce8, 0x4aa582, 0x1e54e6, 0x4f16c1, 0x1a7e79, 0x3978f, 0x4e4817, 0x31b859, 0x5884cc, 0x1b4827, 0x5b63d0, 0x5d787a, 0x35225e, 0x400c7e, 0x6c09d1, 0x5bd532, 0x6bc4d3, 0x258ecb, 0x2e534c, 0x97a6c, 0x3b8820, 0x6d285c, 0x2ca4f8, 0x337caa, 0x14b2a0, 0x558536, 0x28f186, 0x55795d, 0x4af670, 0x234a86, 0x75e826, 0x78de66, 0x5528c, 0x7adf59, 0xf6e17, 0x5bf3da, 0x459b7e, 0x628b34, 0x5dbecb, 0x1a9e7b, 0x6d9, 0x6257c5, 0x574b3c, 0x69a8ef, 0x289838, 0x64b5fe, 0x7ef8f5, 0x2a4e78, 0x120a23, 0x154a8, 0x9b7ff, 0x435e87, 0x437ff8, 0x5cd5b4, 0x4dc04e, 0x4728af, 0x7f735d, 0xc8d0d, 0xf66d5, 0x5a6d80, 0x61ab98, 0x185d96, 0x437f31, 0x468298, 0x662960, 0x4bd579, 0x28de06, 0x465d8d, 0x49b0e3, 0x9b434, 0x7c0db3, 0x5a68b0, 0x409ba9, 0x64d3d5, 0x21762a, 0x658591, 0x246e39, 0x48c39b, 0x7bc759, 0x4f5859, 0x392db2, 0x230923, 0x12eb67, 0x454df2, 0x30c31c, 0x285424, 0x13232e, 0x7faf80, 0x2dbfcb, 0x22a0b, 0x7e832c, 0x26587a, 0x6b3375, 0x95b76, 0x6be1cc, 0x5e061e, 0x78e00d, 0x628c37, 0x3da604, 0x4ae53c, 0x1f1d68, 0x6330bb, 0x7361b8, 0x5ea06c, 0x671ac7, 0x201fc6, 0x5ba4ff, 0x60d772, 0x8f201, 0x6de024, 0x80e6d, 0x56038e, 0x695688, 0x1e6d3e, 0x2603bd, 0x6a9dfa, 0x7c017, 0x6dbfd4, 0x74d0bd, 0x63e1e3, 0x519573, 0x7ab60d, 0x2867ba, 0x2decd4, 0x58018c, 0x3f4cf5, 0xb7009, 0x427e23, 0x3cbd37, 0x273333, 0x673957, 0x1a4b5d, 0x196926, 0x1ef206, 0x11c14e, 0x4c76c8, 0x3cf42f, 0x7fb19a, 0x6af66c, 0x2e1669, 0x3352d6, 0x34760, 0x85260, 0x741e78, 0x2f6316, 0x6f0a11, 0x7c0f1, 0x776d0b, 0xd1ff0, 0x345824, 0x223d4, 0x68c559, 0x5e8885, 0x2faa32, 0x23fc65, 0x5e6942, 0x51e0ed, 0x65adb3, 0x2ca5e6, 0x79e1fe, 0x7b4064, 0x35e1dd, 0x433aac, 0x464ade, 0x1cfe14, 0x73f1ce, 0x10170e, 0x74b6d7, 0x0
};

const uint32_t psi_inv[N] = {
   0x5b3100, 0x233d4c, 0x23354d, 0x186ccb, 0x17ffe7, 0x160047, 0x186ceb, 0x49411d, 0x7b9f38, 0x479b5c, 0x2336bf, 0x6fa00f, 0x6b6929, 0x24e916, 0x44872a, 0x2e8396, 0x2a74b5, 0x1b5a25, 0x4e722, 0x74bb0a, 0x2d3711, 0x32dbc5, 0x18a072, 0xa3140, 0x7ab68a, 0x433fe7, 0x1ae3ad, 0x4cb6bc, 0x6b8e18, 0x5eefa7, 0x59a6b7, 0x71b249, 0x2668eb, 0x71bb4a, 0x699e14, 0x2268ca, 0x7ba065, 0x6784f0, 0x161ad1, 0x5e12f3, 0xd197e, 0x4eb4dc, 0x633686, 0x1b4561, 0x63cc0, 0x3283c7, 0x25e1f2, 0x6f6cf8, 0x672407, 0x5e9997, 0x302176, 0x1ac066, 0x77846b, 0x38da0a, 0x7cde2f, 0x37169f, 0x23ac05, 0x6fd7b0, 0x17bbd1, 0x1b5cc7, 0x2b0761, 0x679772, 0x64646c, 0x32a0c3, 0x11109b, 0x4e9a67, 0x5d1242, 0x51e614, 0x3838f5, 0x28054, 0x7d36bb, 0x4370ce, 0x44ebee, 0x6e3abe, 0x5a64c9, 0x153352, 0x6b673e, 0x551d66, 0x7586b1, 0x6621ac, 0x698d85, 0x494bd3, 0x621bf1, 0x7b22cb, 0x68b65b, 0x2d289b, 0xa0d97, 0x51f568, 0x9eb18, 0x5fd9c2, 0x654ed2, 0x5123c4, 0x522e19, 0x324bed, 0x539d9b, 0x2713d4, 0x18cbf5, 0x3e2439, 0x32b0c4, 0x1864a0, 0x70b58e, 0x5a8d40, 0x5fb98d, 0x596078, 0x6d168b, 0x7e9495, 0x4440b2, 0x1699ac, 0x771c24, 0x3c4154, 0x743780, 0x6026bd, 0x87706, 0x56b3b3, 0x2a7d33, 0x5d0c97, 0x7117b0, 0x7066d6, 0x6cbba7, 0x21746, 0x17d090, 0x58868c, 0x5ff55b, 0x228c89, 0x540b69, 0x3630a4, 0x287e08, 0x59495, 0x77d47a, 0x45e71a, 0x7160f7, 0x5cba92, 0x5e42ab, 0x24ff12, 0x423fcf, 0x42ef02, 0x698d0e, 0xd1927, 0x16ff8a, 0x50ab60, 0x2df1ce, 0x680ae8, 0x10abbe, 0xb8d54, 0x7ece17, 0x65b3ef, 0x795009, 0x4397b, 0x3c0f88, 0x86af8, 0x682e76, 0x45d0c5, 0x7bb6d1, 0x7e3c51, 0x663696, 0x28e4cc, 0x4a64cb, 0x400734, 0x2175e9, 0x59a49d, 0x76ff5a, 0x7066fe, 0x732b6e, 0x32ca52, 0xc5355, 0x2c5667, 0x219165, 0x1eb0ef, 0x3a37fc, 0x204986, 0x53df3b, 0x68e997, 0x6bac24, 0x294fa, 0x172547, 0xdff0f, 0x587a2, 0x490017, 0x3c0ff5, 0x4a9104, 0x2cee22, 0x70a962, 0x4b34bd, 0x54de3a, 0x3be8ca, 0x48efef, 0x3b7700, 0x4f7448, 0x121d81, 0x6fd01e, 0xc629d, 0x508fcb, 0x462f25, 0xe57a3, 0x70514d, 0x5a6d63, 0x610cff, 0xea9e5, 0x37ffa, 0x50dcf2, 0x49ef1b, 0x7b3246, 0xa5646, 0x6cb3c4, 0x409e6b, 0x3edafb, 0x29101b, 0x400841, 0x764e6a, 0x6bb5ef, 0x677e73, 0x5d3908, 0x367a4d, 0x2e6b6f, 0x634928, 0x1843d4, 0x20c54, 0x1b8e33, 0x2db8e4, 0xd2d38, 0x6f24ec, 0xd8616, 0x1fa22c, 0x52aba9, 0x1e927, 0x7b05e7, 0x1b178f, 0x1cc13a, 0x6b70fe, 0x1a0544, 0x4ccb51, 0x5c9eb5, 0x1e3068, 0x73b136, 0x4f0a35, 0x52a941, 0x383c96, 0x39a97a, 0x3652, 0x1c5ba9, 0x58ffda, 0x517527, 0x5e2005, 0x1e40bd, 0x3b1401, 0x7f35ad, 0x36eaef, 0x6ab8c5, 0x7386, 0x4d8502, 0x6b93e5, 0xb1b89, 0x543a63, 0xec41e, 0x3fec94, 0x0
};


void poly_init_q() {
  const uint32_t q = Q;
  const uint32_t mu[2] = {0x801C0601, 0x00000200}; 
  const uint32_t inv2 = 0x3ff001;
  ntt_lite_load_q(q, mu, 8, 23, inv2, NTT_LITE_MODE_SINGLE);
}


void poly_set_q() {
  ntt_lite_set_q(Q);
}


void poly_init_ntt() {
	ntt_lite_load_twiddle((uint32_t*) psi);
}


void poly_init_invntt() {
	ntt_lite_load_twiddle((uint32_t*) psi_inv);
}

/*************************************************
* Name:        poly_reduce
*
* Description: Inplace reduction of all coefficients of polynomial to
*              representative in [-6283008,6283008].
*
* Arguments:   - poly *a: pointer to input/output polynomial
**************************************************/
void poly_reduce(poly *a) {
  unsigned int i;

  for(i = 0; i < N; ++i)
    a->coeffs[i] = reduce32(a->coeffs[i]);
}

/*************************************************
* Name:        poly_caddq
*
* Description: For all coefficients of in/out polynomial add Q if
*              coefficient is negative.
*
* Arguments:   - poly *a: pointer to input/output polynomial
**************************************************/
void poly_caddq(poly *a) {
    for (int i = 0; i < N; i++) {  // N polynom length
        a->coeffs[i] = caddq(a->coeffs[i]);  // Call caddq with the value, not a pointer
    }
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
void poly_add(poly *c, const poly *a, const poly *b)  {
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
void poly_sub(poly *c, const poly *a, const poly *b)  {
  ntt_lite_sub((uint32_t*)c->coeffs, (uint32_t*)a->coeffs, (uint32_t*)b->coeffs);
}


/*************************************************
* Name:        poly_shiftl
*
* Description: Multiply polynomial by 2^D without modular reduction. Assumes
*              input coefficients to be less than 2^{31-D} in absolute value.
*
* Arguments:   - poly *a: pointer to input/output polynomial
**************************************************/
void poly_shiftl(poly *a) {
  unsigned int i;

  for(i = 0; i < N; ++i)
    a->coeffs[i] <<= D;  
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


void poly_invntt_sub_add_constant(poly *a, poly *b, poly *c, uint32_t d) {
  ntt_lite_backward_ntt(NTT_LITE_OUTPUT_DIS, c->coeffs);
  ntt_lite_sub_rev(b->coeffs, NTT_LITE_INPUT_DIS, b->coeffs);
  ntt_lite_add_const((uint32_t*)a->coeffs, NTT_LITE_INPUT_DIS, &d);
}


int poly_invntt_chknorm(poly *a, uint32_t B, poly *temp) {
  unsigned int i;
  uint32_t *rhs = &B;

  ntt_lite_backward_ntt((uint32_t*) a->coeffs, (uint32_t*) a->coeffs);
  ntt_lite_add_const((uint32_t*) temp->coeffs, NTT_LITE_INPUT_DIS, rhs);
  if (poly_chknorm_shifted(temp, B)) {
    return 1;
  }
  return 0;
}


int poly_pointwise_add_invntt_chknorm(poly *r, const poly *v, const poly *c, const poly *u, uint32_t B) {
  unsigned int i;
  poly temp;
  uint32_t *rhs = &B;

  ntt_lite_pwm(NTT_LITE_OUTPUT_DIS, (uint32_t*) &v->coeffs, (uint32_t*) &c->coeffs);
  ntt_lite_add(NTT_LITE_OUTPUT_DIS, NTT_LITE_INPUT_DIS, (uint32_t*) u->coeffs);
  poly_init_invntt();
  ntt_lite_backward_ntt((uint32_t*) r->coeffs, NTT_LITE_INPUT_DIS);
  ntt_lite_add_const((uint32_t*) temp.coeffs, NTT_LITE_INPUT_DIS, rhs);
  if (poly_chknorm_shifted(&temp, B)) {
    return 1;
  }
  else {
    return 0;
  }
}


int poly_pointwise_invntt_sub_chknorm(poly *r, const poly *v, const poly *c, const poly *u, uint32_t B) {
  unsigned int i;
  poly temp;
  uint32_t *rhs = &B;

  ntt_lite_pwm(NTT_LITE_OUTPUT_DIS, (uint32_t*) &v->coeffs, (uint32_t*) &c->coeffs);
  poly_init_invntt();
  ntt_lite_backward_ntt(NTT_LITE_OUTPUT_DIS, NTT_LITE_INPUT_DIS);
  ntt_lite_sub_rev((uint32_t*) r->coeffs, NTT_LITE_INPUT_DIS, (uint32_t*) u->coeffs);
  ntt_lite_add_const((uint32_t*) temp.coeffs, NTT_LITE_INPUT_DIS, rhs);
  if (poly_chknorm_shifted(&temp, B)) {
    return 1;
  }
  else {
    return 0;
  }
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
* Name:        poly_power2round
*
* Description: For all coefficients c of the input polynomial,
*              compute c0, c1 such that c mod Q = c1*2^D + c0
*              with -2^{D-1} < c0 <= 2^{D-1}. Assumes coefficients to be
*              standard representatives.
*
* Arguments:   - poly *a1: pointer to output polynomial with coefficients c1
*              - poly *a0: pointer to output polynomial with coefficients c0
*              - const poly *a: pointer to input polynomial
**************************************************/
void poly_power2round(poly *a1, poly *a0, const poly *a) {
  unsigned int i;
  
  for(i = 0; i < N; ++i)
    a1->coeffs[i] = power2round(&a0->coeffs[i], a->coeffs[i]);
}

/*************************************************
* Name:        poly_decompose
*
* Description: For all coefficients c of the input polynomial,
*              compute high and low bits c0, c1 such c mod Q = c1*ALPHA + c0
*              with -ALPHA/2 < c0 <= ALPHA/2 except c1 = (Q-1)/ALPHA where we
*              set c1 = 0 and -ALPHA/2 <= c0 = c mod Q - Q < 0.
*              Assumes coefficients to be standard representatives.
*
* Arguments:   - poly *a1: pointer to output polynomial with coefficients c1
*              - poly *a0: pointer to output polynomial with coefficients c0
*              - const poly *a: pointer to input polynomial
**************************************************/
void poly_decompose(poly *a1, poly *a0, const poly *a) {
  unsigned int i;
  

  for(i = 0; i < N; ++i)
    a1->coeffs[i] = decompose(&a0->coeffs[i], a->coeffs[i]);

}

/*************************************************
* Name:        poly_make_hint
*
* Description: Compute hint polynomial. The coefficients of which indicate
*              whether the low bits of the corresponding coefficient of
*              the input polynomial overflow into the high bits.
*
* Arguments:   - poly *h: pointer to output hint polynomial
*              - const poly *a0: pointer to low part of input polynomial
*              - const poly *a1: pointer to high part of input polynomial
*
* Returns number of 1 bits.
**************************************************/
unsigned int poly_make_hint(poly *h, const poly *a0, const poly *a1) {
  unsigned int i, s = 0;
  

  for(i = 0; i < N; ++i) {
    h->coeffs[i] = make_hint(a0->coeffs[i], a1->coeffs[i]);
    s += h->coeffs[i];
  }

  return s;
}

/*************************************************
* Name:        poly_use_hint
*
* Description: Use hint polynomial to correct the high bits of a polynomial.
*
* Arguments:   - poly *b: pointer to output polynomial with corrected high bits
*              - const poly *a: pointer to input polynomial
*              - const poly *h: pointer to input hint polynomial
**************************************************/
void poly_use_hint(poly *b, const poly *a, const poly *h) {
  unsigned int i;
  

  for(i = 0; i < N; ++i)
    b->coeffs[i] = use_hint(a->coeffs[i], h->coeffs[i]);

}

/*************************************************
* Name:        poly_chknorm
*
* Description: Check infinity norm of polynomial against given bound.
*              Assumes input coefficients were reduced by reduce32().
*
* Arguments:   - const poly *a: pointer to polynomial
*              - int32_t B: norm bound
*
* Returns 0 if norm is strictly smaller than B <= (Q-1)/8 and 1 otherwise.
**************************************************/
int poly_chknorm(const poly *a, int32_t B) {
  unsigned int i;
  int32_t t;
  

  if(B > (Q-1)/8)
    return 1;

  /* It is ok to leak which coefficient violates the bound since
     the probability for each coefficient is independent of secret
     data but we must not leak the sign of the centralized representative. */
  for(i = 0; i < N; ++i) {
    t = a->coeffs[i] + B;

    if(((uint32_t)t) >= (2*B)) {
      return 1;
    }
  }

  return 0;
}



int poly_chknorm_shifted(const poly *a, int32_t B) {
  unsigned int i;
  int32_t t;
  
  if(B > (Q-1)/8)
    return 1;

  /* It is ok to leak which coefficient violates the bound since
     the probability for each coefficient is independent of secret
     data but we must not leak the sign of the centralized representative. */
  for(i = 0; i < N; ++i) {
    if(((uint32_t) a->coeffs[i]) >= (2*B)) {
      return 1;
    }
  }

  return 0;
}


/*************************************************
* Name:        rej_uniform
*
* Description: Sample uniformly random coefficients in [0, Q-1] by
*              performing rejection sampling on array of random bytes.
*
* Arguments:   - int32_t *a: pointer to output array (allocated)
*              - unsigned int len: number of coefficients to be sampled
*              - const uint8_t *buf: array of random bytes
*              - unsigned int buflen: length of array of random bytes
*
* Returns number of sampled coefficients. Can be smaller than len if not enough
* random bytes were given.
**************************************************/
static unsigned int rej_uniform(int32_t *a,
                                unsigned int len,
                                const uint8_t *buf,
                                unsigned int buflen)
{
  unsigned int ctr, pos;
  uint32_t t;

  ctr = pos = 0;
  while(ctr < len && pos + 3 <= buflen) {
    t  = buf[pos++];
    t |= (uint32_t)buf[pos++] << 8;
    t |= (uint32_t)buf[pos++] << 16;
    t &= 0x7FFFFF;

    if(t < Q)
      a[ctr++] = t;
  }

  return ctr;
}

/*************************************************
* Name:        poly_uniform
*
* Description: Sample polynomial with uniformly random coefficients
*              in [0,Q-1] by performing rejection sampling on the
*              output stream of SHAKE256(seed|nonce) or AES256CTR(seed,nonce).
*
* Arguments:   - poly *a: pointer to output polynomial
*              - const uint8_t seed[]: byte array with seed of length SEEDBYTES
*              - uint16_t nonce: 2-byte nonce
**************************************************/
#define POLY_UNIFORM_NBLOCKS ((768 + STREAM128_BLOCKBYTES - 1)/STREAM128_BLOCKBYTES)
void poly_uniform(poly *a,
  const uint8_t seed[SEEDBYTES],
  uint16_t nonce)
{
  unsigned int i, ctr, off;
  unsigned int buflen = POLY_UNIFORM_NBLOCKS*STREAM128_BLOCKBYTES;
  uint8_t buf[POLY_UNIFORM_NBLOCKS*STREAM128_BLOCKBYTES + 2];

  stream128_init(seed, nonce);
  stream128_squeezeblocks(buf, POLY_UNIFORM_NBLOCKS);

  ctr = rej_uniform(a->coeffs, N, buf, buflen);

  while(ctr < N) {
    off = buflen % 3;
    for(i = 0; i < off; ++i)
      buf[i] = buf[buflen - off + i];

    stream128_squeezeblocks(buf + off, 1);
    buflen = STREAM128_BLOCKBYTES + off;
    ctr += rej_uniform(a->coeffs + ctr, N - ctr, buf, buflen);
  }
}

/*************************************************
* Name:        rej_eta
*
* Description: Sample uniformly random coefficients in [-ETA, ETA] by
*              performing rejection sampling on array of random bytes.
*
* Arguments:   - int32_t *a: pointer to output array (allocated)
*              - unsigned int len: number of coefficients to be sampled
*              - const uint8_t *buf: array of random bytes
*              - unsigned int buflen: length of array of random bytes
*
* Returns number of sampled coefficients. Can be smaller than len if not enough
* random bytes were given.
**************************************************/
static unsigned int rej_eta(int32_t *a,
                            unsigned int len,
                            const uint8_t *buf,
                            unsigned int buflen)
{
  unsigned int ctr, pos;
  uint32_t t0, t1;
  

  ctr = pos = 0;
  while(ctr < len && pos < buflen) {
    t0 = buf[pos] & 0x0F;
    t1 = buf[pos++] >> 4;

#if ETA == 2
    if(t0 < 15) {
      t0 = t0 - (205*t0 >> 10)*5;
      a[ctr++] = 2 - t0;
    }
    if(t1 < 15 && ctr < len) {
      t1 = t1 - (205*t1 >> 10)*5;
      a[ctr++] = 2 - t1;
    }
#elif ETA == 4
    if(t0 < 9)
      a[ctr++] = 4 - t0;
    if(t1 < 9 && ctr < len)
      a[ctr++] = 4 - t1;
#endif
  }

  return ctr;
}

/*************************************************
* Name:        poly_uniform_eta
*
* Description: Sample polynomial with uniformly random coefficients
*              in [-ETA,ETA] by performing rejection sampling on the
*              output stream from SHAKE256(seed|nonce) or AES256CTR(seed,nonce).
*
* Arguments:   - poly *a: pointer to output polynomial
*              - const uint8_t seed[]: byte array with seed of length SEEDBYTES
*              - uint16_t nonce: 2-byte nonce
**************************************************/
#if ETA == 2
#define POLY_UNIFORM_ETA_NBLOCKS ((136 + STREAM256_BLOCKBYTES - 1)/STREAM256_BLOCKBYTES)
#elif ETA == 4
#define POLY_UNIFORM_ETA_NBLOCKS ((227 + STREAM256_BLOCKBYTES - 1)/STREAM256_BLOCKBYTES)
#endif
void poly_uniform_eta(poly *a, const uint8_t seed[CRHBYTES], uint16_t nonce) {
    uint8_t buf[POLY_UNIFORM_ETA_NBLOCKS * SHAKE256_RATE];
    uint8_t extseed[CRHBYTES + 2] __attribute__((aligned(4)));  // align for Keccak*/
    unsigned int ctr;

    for (size_t i = 0; i < CRHBYTES; i++) {
        extseed[i] = seed[i];
    }
    extseed[CRHBYTES] = nonce & 0xFF;
    extseed[CRHBYTES + 1] = nonce >> 8;

    dilithium_shake256_stream_init(extseed, nonce); 
    dilithium_shake256_squeezeblocks(buf, POLY_UNIFORM_ETA_NBLOCKS);

    ctr = rej_eta(a->coeffs, N, buf, sizeof(buf));

    while (ctr < N) {
        dilithium_shake256_squeezeblocks(buf, 1);
        ctr += rej_eta(a->coeffs + ctr, N - ctr, buf, SHAKE256_RATE);
    }
}

/*************************************************
* Name:        poly_uniform_gamma1m1
*
* Description: Sample polynomial with uniformly random coefficients
*              in [-(GAMMA1 - 1), GAMMA1] by unpacking output stream
*              of SHAKE256(seed|nonce) or AES256CTR(seed,nonce).
*
* Arguments:   - poly *a: pointer to output polynomial
*              - const uint8_t seed[]: byte array with seed of length CRHBYTES
*              - uint16_t nonce: 16-bit nonce
**************************************************/
#define POLY_UNIFORM_GAMMA1_NBLOCKS ((POLYZ_PACKEDBYTES + STREAM256_BLOCKBYTES - 1)/STREAM256_BLOCKBYTES)
void poly_uniform_gamma1(poly *a,
                         const uint8_t seed[CRHBYTES],
                         uint16_t nonce) {
  uint8_t buf[POLYZ_PACKEDBYTES];
  volatile uint32_t t;

 
  keccak_init(SHAKE256_RATE >> 3, KECCAK_MASK_DIS);
  
  keccak_absorb((uint32_t*)seed, NULL, (CRHBYTES) >> 2);
  t = (SHAKE_PAD << 16) | ((uint32_t) nonce);
  keccak_finish((uint32_t*) &t);
  keccak_squeeze((uint32_t*)buf, NULL, (POLYZ_PACKEDBYTES) >> 2);
  polyz_unpack(a, buf);
}

/*************************************************
* Name:        challenge
*
* Description: Implementation of H. Samples polynomial with TAU nonzero
*              coefficients in {-1,1} using the output stream of
*              SHAKE256(seed).
*
* Arguments:   - poly *c: pointer to output polynomial
*              - const uint8_t mu[]: byte array containing seed of length SEEDBYTES
**************************************************/
void poly_challenge(poly *c, const uint8_t seed[SEEDBYTES]) {
  unsigned int i, b, pos;
  uint64_t signs;
  uint8_t buf[SHAKE256_RATE];
  volatile uint32_t t;

  keccak_init(SHAKE256_RATE >> 3, KECCAK_MASK_DIS);
  keccak_absorb((uint32_t*)seed, NULL, (SEEDBYTES) >> 2); // SEEDBYTES + padding
  t = SHAKE_PAD;
  keccak_finish((uint32_t*) &t);
  keccak_squeeze((uint32_t*)buf, NULL, (SHAKE256_RATE) >> 2);

  signs = 0;
  for(i = 0; i < 8; ++i)
    signs |= (uint64_t)buf[i] << 8*i;
  pos = 8;

  for(i = 0; i < N; ++i)
    c->coeffs[i] = 0;
  for(i = N-TAU; i < N; ++i) {
    do {
      if(pos >= SHAKE256_RATE) {
        keccak_squeeze((uint32_t*)buf, NULL, (SHAKE256_RATE) >> 2);
        pos = 0;
      }

      b = buf[pos++];
    } while(b > i);

    c->coeffs[i] = c->coeffs[b];
    c->coeffs[b] = 1 + ((Q - 2) & (-(signs & 1)));
    signs >>= 1;
  }
}


/*************************************************
* Name:        polyeta_pack
*
* Description: Bit-pack polynomial with coefficients in [-ETA,ETA].
*
* Arguments:   - uint8_t *r: pointer to output byte array with at least
*                            POLYETA_PACKEDBYTES bytes
*              - const poly *a: pointer to input polynomial
**************************************************/
void polyeta_pack(uint8_t *r, const poly *a) {
  const uint32_t eta_c = ETA;
  ntt_lite_sub_rev_const(NTT_LITE_OUTPUT_DIS, a->coeffs, &eta_c);
  ntt_lite_encode((uint32_t*) r, NTT_LITE_INPUT_DIS, LOG_ETA);
}

/*************************************************
* Name:        polyeta_unpack
*
* Description: Unpack polynomial with coefficients in [-ETA,ETA].
*
* Arguments:   - poly *r: pointer to output polynomial
*              - const uint8_t *a: byte array with bit-packed polynomial
**************************************************/
void polyeta_unpack(poly *r, const uint8_t *a) {
  const uint32_t eta_c = ETA;
  ntt_lite_decode(NTT_LITE_OUTPUT_DIS, (uint32_t*) a, LOG_ETA);
  ntt_lite_sub_rev_const(r->coeffs, NTT_LITE_INPUT_DIS, &eta_c);
}

/*************************************************
* Name:        polyt1_pack
*
* Description: Bit-pack polynomial t1 with coefficients fitting in 10 bits.
*              Input coefficients are assumed to be standard representatives.
*
* Arguments:   - uint8_t *r: pointer to output byte array with at least
*                            POLYT1_PACKEDBYTES bytes
*              - const poly *a: pointer to input polynomial
**************************************************/
void polyt1_pack(uint8_t *r, const poly *a) {
  ntt_lite_encode((uint32_t*) r, (uint32_t*) a, (POLYT1_PACKEDBYTES << 3) / N);
}

/*************************************************
* Name:        polyt1_unpack
*
* Description: Unpack polynomial t1 with 10-bit coefficients.
*              Output coefficients are standard representatives.
*
* Arguments:   - poly *r: pointer to output polynomial
*              - const uint8_t *a: byte array with bit-packed polynomial
**************************************************/
void polyt1_unpack(poly *r, const uint8_t *a) {
  ntt_lite_decode((uint32_t*) r, (uint32_t*) a, (POLYT1_PACKEDBYTES << 3) / N);
}


void poly_init_pack() {
  const uint32_t q = 0;
  const uint32_t mu[2] = {0x801C0601, 0x00000200}; 
  const uint32_t inv2 = 0x3ff001;
  ntt_lite_load_q(q, mu, 8, 23, inv2, NTT_LITE_MODE_SINGLE);
}


void poly_set_pack() {
  ntt_lite_set_q(0);
}


/*************************************************
* Name:        polyt0_pack
*
* Description: Bit-pack polynomial t0 with coefficients in ]-2^{D-1}, 2^{D-1}].
*
* Arguments:   - uint8_t *r: pointer to output byte array with at least
*                            POLYT0_PACKEDBYTES bytes
*              - const poly *a: pointer to input polynomial
**************************************************/
void polyt0_pack(uint8_t *r, const poly *a) {
  const uint32_t t0_c = 1 << (D - 1);
  ntt_lite_sub_rev_const(NTT_LITE_OUTPUT_DIS, a->coeffs, &t0_c);
  ntt_lite_encode((uint32_t*) r, NTT_LITE_INPUT_DIS, D);
}

/*************************************************
* Name:        polyt0_unpack
*
* Description: Unpack polynomial t0 with coefficients in ]-2^{D-1}, 2^{D-1}].
*
* Arguments:   - poly *r: pointer to output polynomial
*              - const uint8_t *a: byte array with bit-packed polynomial
**************************************************/
void polyt0_unpack(poly *r, const uint8_t *a) {
  const uint32_t t0_c = 1 << (D - 1);
  ntt_lite_decode(NTT_LITE_OUTPUT_DIS, (uint32_t*) a, D);
  ntt_lite_sub_rev_const(r->coeffs, NTT_LITE_INPUT_DIS, &t0_c);
}


/*************************************************
* Name:        polyz_pack
*
* Description: Bit-pack polynomial with coefficients
*              in [-(GAMMA1 - 1), GAMMA1].
*
* Arguments:   - uint8_t *r: pointer to output byte array with at least
*                            POLYZ_PACKEDBYTES bytes
*              - const poly *a: pointer to input polynomial
**************************************************/
void polyz_pack(uint8_t *r, const poly *a) {
  const uint32_t gamma1 = GAMMA1;
  ntt_lite_sub_rev_const(NTT_LITE_OUTPUT_DIS, a->coeffs, &gamma1);
  ntt_lite_encode((uint32_t*) r, NTT_LITE_INPUT_DIS, LOG_GAMMA1);
}

/*************************************************
* Name:        polyz_unpack
*
* Description: Unpack polynomial z with coefficients
*              in [-(GAMMA1 - 1), GAMMA1].
*
* Arguments:   - poly *r: pointer to output polynomial
*              - const uint8_t *a: byte array with bit-packed polynomial
**************************************************/
void polyz_unpack(poly *r, const uint8_t *a) {
  const uint32_t gamma1 = GAMMA1;
  ntt_lite_decode(NTT_LITE_OUTPUT_DIS, (uint32_t*) a, LOG_GAMMA1);
  ntt_lite_sub_rev_const(r->coeffs, NTT_LITE_INPUT_DIS, &gamma1);
}

/*************************************************
* Name:        polyw1_pack
*
* Description: Bit-pack polynomial w1 with coefficients in [0,15] or [0,43].
*              Input coefficients are assumed to be standard representatives.
*
* Arguments:   - uint8_t *r: pointer to output byte array with at least
*                            POLYW1_PACKEDBYTES bytes
*              - const poly *a: pointer to input polynomial
**************************************************/
void polyw1_pack(uint8_t *r, const poly *a) {
  ntt_lite_encode((uint32_t*) r, a->coeffs, LOG_GAMMA2);
}
