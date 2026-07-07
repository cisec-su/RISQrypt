#include <stdint.h>
#include "params.h"
#include "polyvec.h"
#include "poly.h"
#include "ntt_lite.h"


#define INV2 0x3ff001


/*************************************************
* Name:        expand_mat
*
* Description: Implementation of ExpandA. Generates matrix A with uniformly
*              random coefficients a_{i,j} by performing rejection
*              sampling on the output stream of SHAKE128(rho|j|i)
*              or AES256CTR(rho,j|i).
*
* Arguments:   - polyvecl mat[K]: output matrix
*              - const uint8_t rho[]: byte array containing seed rho
**************************************************/
void polyvec_matrix_expand(polyvecl mat[K], const uint8_t rho[SEEDBYTES]) {
    unsigned int i, j, i_next, j_next;
    ntt_lite_set_inv2(STREAM128_BLOCKBYTES >> 2);
    ntt_lite_set_bound(Q);


    i_next = 0;
    j_next = 1;

    stream128_init(rho, 0);

    for(i = 0; i < K; i++) {
        for(j = 0; j < L; j++) {
            poly_uniform_fromhw(&mat[i].vec[j], rho, (i_next << 8) + j_next, (i != (K - 1)) || (j != (L - 1)));
            if (j_next == (L - 1)) {
                j_next = 0;
                i_next++;
            }
            else {
                j_next++;
            }
        }
    }
    ntt_lite_set_inv2(INV2);
}


void polyvec_matrix_pointwise(polyveck *t, const polyvecl mat[K], const polyvecl *v) {
    unsigned int i;

    for(i = 0; i < K; i++)
        polyvecl_pointwise_acc(&t->vec[i], &mat[i], v);
}

void polyvec_matrix_pointwise_onthefly(polyveck *t,
                                       const uint8_t rho[SEEDBYTES],
                                       const polyvecl *v)
{
    unsigned int i, j, i_next, j_next;
    poly row_poly;

    ntt_lite_set_inv2(STREAM128_BLOCKBYTES >> 2);
    ntt_lite_set_bound(Q);

    i_next = 0;
    j_next = 1;

    stream128_init(rho, 0);

    for(i = 0; i < K; i++) {
        for(j = 0; j < L; j++) {
            poly_uniform_fromhw(&row_poly,
                    rho,
                    (i_next << 8) + j_next,
                    (i != (K - 1)) || (j != (L - 1)));

            if(j_next == (L - 1)) {
                j_next = 0;
                i_next++;
            }
            else {
                j_next++;
            }

            if(j == 0) {
                poly_pointwise(&t->vec[i], &row_poly, &v->vec[j]);
            }
            else {
                poly_pointwise_acc(&t->vec[i], &row_poly, &v->vec[j]);
            }
        }
    }

    ntt_lite_set_inv2(INV2);
}

/**************************************************************/
/************ Vectors of polynomials of length L **************/
/**************************************************************/

void polyvecl_uniform_eta(polyvecl *v, const uint8_t seed[CRHBYTES], uint16_t nonce) {
    unsigned int i;
    ntt_lite_set_inv2(SHAKE256_RATE >> 2);
    ntt_lite_set_bound(ETA*2 + 1);

    stream256_init(seed, nonce); 
    for(i = 0; i < L; i++) {
        poly_uniform_eta_fromhw(&v->vec[i], seed, ++nonce, i != (L - 1));
    }

    ntt_lite_set_inv2(INV2);
}

void polyvecl_uniform_gamma1(polyvecl *v, const uint8_t seed[CRHBYTES], uint16_t nonce) {
    unsigned int i;

    stream256_init(seed, L*nonce); 
    for(i = 0; i < L; i++) {
        poly_uniform_gamma1_fromhw(&v->vec[i], seed, L*nonce + i + 1, i != (L - 1));
    }
}


/*************************************************
* Name:        polyvecl_add
*
* Description: Add vectors of polynomials of length L.
*              No modular reduction is performed.
*
* Arguments:   - polyvecl *w: pointer to output vector
*              - const polyvecl *u: pointer to first summand
*              - const polyvecl *v: pointer to second summand
**************************************************/
void polyvecl_add(polyvecl *w, const polyvecl *u, const polyvecl *v) {
    unsigned int i;

    for(i = 0; i < L; i++)
        poly_add(&w->vec[i], &u->vec[i], &v->vec[i]);
}

/*************************************************
* Name:        polyvecl_ntt
*
* Description: Forward NTT of all polynomials in vector of length L. Output
*              coefficients can be up to 16*Q larger than input coefficients.
*
* Arguments:   - polyvecl *v: pointer to input/output vector
**************************************************/
void polyvecl_ntt(polyvecl *v) {
    unsigned int i;

    for(i = 0; i < L; i++)
        poly_ntt(&v->vec[i]);
}


void polyvecl_invntt(polyvecl *v) {
    unsigned int i;

    for(i = 0; i < L; i++)
        poly_invntt(&v->vec[i]);
}


int polyvecl_invntt_chknorm(polyvecl *v, uint32_t B) {
    unsigned int i;

    for(i = 0; i < L; i++) {
        if (poly_invntt_chknorm(&v->vec[i], B)) {
            return 1;
        }
    }
    return 0;
}


void polyvecl_pointwise_poly(polyvecl *r, const poly *a, const polyvecl *v) {
    unsigned int i;
    const uint32_t *rhs;

    for(i = 0; i < L; i++) {
        if (i == 0) {
            rhs = a->coeffs;
        } else {
            rhs = NTT_LITE_INPUT_DIS;
        }
        ntt_lite_pwm((uint32_t*) &r->vec[i].coeffs, (uint32_t*) &v->vec[i].coeffs, rhs);         
    }
}


int polyvecl_pointwise_add_invntt_chknorm(polyvecl *r, const polyvecl *v, const poly *c, const polyvecl *u, uint32_t B) {
    unsigned int i;
    ntt_lite_set_bound(B);

    for(i = 0; i < L; i++) {
        if (poly_pointwise_add_invntt_chknorm(&r->vec[i], &v->vec[i], c, &u->vec[i], B)) {
            return 1;
        }
    }
    return 0;
}


/*************************************************
* Name:        polyvecl_pointwise_acc
*
* Description: Pointwise multiply vectors of polynomials of length L, multiply
*              resulting vector by 2^{-32} and add (accumulate) polynomials
*              in it. Input/output vectors are in NTT domain representation.
*
* Arguments:   - poly *w: output polynomial
*              - const polyvecl *u: pointer to first input vector
*              - const polyvecl *v: pointer to second input vector
**************************************************/
void polyvecl_pointwise_acc(poly *w, const polyvecl *u, const polyvecl *v) {
    unsigned int i;
    uint32_t *dst;

    ntt_lite_pwm(NTT_LITE_OUTPUT_DIS, (uint32_t*) &u->vec[0].coeffs, (uint32_t*) &v->vec[0].coeffs);
    for(i = 1; i < L; i++) {
        if (i != (L - 1)) {
            dst = NTT_LITE_OUTPUT_DIS;
        }
        else {
            dst = (uint32_t*) w->coeffs;
        }
        ntt_lite_mac(dst, (uint32_t*) &u->vec[i].coeffs, (uint32_t*) &v->vec[i].coeffs);
    }
}

/*************************************************
* Name:        polyvecl_chknorm
*
* Description: Check infinity norm of polynomials in vector of length L.
*              Assumes input polyvecl to be reduced by polyvecl_reduce().
*
* Arguments:   - const polyvecl *v: pointer to vector
*              - int32_t B: norm bound
*
* Returns 0 if norm of all polynomials is strictly smaller than B <= (Q-1)/8
* and 1 otherwise.
**************************************************/
int polyvecl_chknorm(const polyvecl *v, int32_t B) {
    unsigned int i;

    ntt_lite_set_bound(B);

    for(i = 0; i < L; i++)
        if(poly_chknorm(&v->vec[i], B))
            return 1;

    return 0;
}


/**************************************************************/
/************ Vectors of polynomials of length K **************/
/**************************************************************/

void polyveck_uniform_eta(polyveck *v, const uint8_t seed[CRHBYTES], uint16_t nonce) {
    unsigned int i;
    ntt_lite_set_inv2(SHAKE256_RATE >> 2);
    ntt_lite_set_bound(ETA*2 + 1);

    stream256_init(seed, nonce); 
    for(i = 0; i < K; i++) {
        poly_uniform_eta_fromhw(&v->vec[i], seed, ++nonce, i != (K - 1));
    }

    ntt_lite_set_inv2(INV2);
}


/*************************************************
* Name:        polyveck_add
*
* Description: Add vectors of polynomials of length K.
*              No modular reduction is performed.
*
* Arguments:   - polyveck *w: pointer to output vector
*              - const polyveck *u: pointer to first summand
*              - const polyveck *v: pointer to second summand
**************************************************/
void polyveck_add(polyveck *w, const polyveck *u, const polyveck *v) {
    unsigned int i;

    for(i = 0; i < K; i++)
        poly_add(&w->vec[i], &u->vec[i], &v->vec[i]);
}

/*************************************************
* Name:        polyveck_sub
*
* Description: Subtract vectors of polynomials of length K.
*              No modular reduction is performed.
*
* Arguments:   - polyveck *w: pointer to output vector
*              - const polyveck *u: pointer to first input vector
*              - const polyveck *v: pointer to second input vector to be
*                                   subtracted from first input vector
**************************************************/
void polyveck_sub(polyveck *w, const polyveck *u, const polyveck *v) {
    unsigned int i;

    for(i = 0; i < K; i++)
        poly_sub(&w->vec[i], &u->vec[i], &v->vec[i]);
}


/*************************************************
* Name:        polyveck_ntt
*
* Description: Forward NTT of all polynomials in vector of length K. Output
*              coefficients can be up to 16*Q larger than input coefficients.
*
* Arguments:   - polyveck *v: pointer to input/output vector
**************************************************/
void polyveck_ntt(polyveck *v) {
    unsigned int i;

    for(i = 0; i < K; i++)
        poly_ntt(&v->vec[i]);
}


void polyveck_shiftl_ntt(polyveck *v) {
    unsigned int i;
    ntt_lite_set_bound(1 << D);
    for(i = 0; i < K; i++) {
        ntt_lite_mul_const(NTT_LITE_OUTPUT_DIS, (uint32_t*) v->vec[i].coeffs);
        ntt_lite_forward_ntt((uint32_t*) v->vec[i].coeffs, NTT_LITE_OUTPUT_DIS);
    }
}


/*************************************************
* Name:        polyveck_invntt
*
* Description: Inverse NTT and multiplication by 2^{32} of polynomials
*              in vector of length K. Input coefficients need to be less
*              than 2*Q.
*
* Arguments:   - polyveck *v: pointer to input/output vector
**************************************************/
void polyveck_invntt(polyveck *v) {
    unsigned int i;

    for(i = 0; i < K; i++)
        poly_invntt(&v->vec[i]);
}


void polyveck_invntt_sub(polyveck *r, polyveck *v, polyveck *u) {
    unsigned int i;

    for(i = 0; i < (K - 1); i++) {
        poly_invntt(&u->vec[i]);
    }

    poly_invntt_sub(&r->vec[K - 1], &v->vec[K - 1], &u->vec[K - 1]);
    
    for(i = 0; i < (K - 1); i++) {
        poly_sub(&r->vec[i], &v->vec[i], &u->vec[i]);
    }
}


int polyveck_invntt_chknorm(polyveck *v, uint32_t B) {
    unsigned int i;
    int flag;

    ntt_lite_set_bound(B);

    for(i = 0; i < K; i++) {
        ntt_lite_backward_ntt(NTT_LITE_OUTPUT_DIS, (uint32_t*) &v->vec[i].coeffs);
        flag = ntt_lite_chknorm(NTT_LITE_INPUT_DIS);
        if (flag == NTT_LITE_CHKNORM_FAIL) {
            return 1;
        }
        ntt_lite_read_poly((uint32_t*) &v->vec[i].coeffs);
    }
    return 0;
}


int polyveck_pointwise_invntt_sub_chknorm(polyveck *r, const polyveck *v, const poly *c, const polyveck *u, uint32_t B) {
    unsigned int i;

    ntt_lite_set_bound(B);

    for(i = 0; i < K; i++) {
        if (poly_pointwise_invntt_sub_chknorm(&r->vec[i], &v->vec[i], c, &u->vec[i], B)) {
            return 1;
        }
    }
    return 0;
}


void polyveck_pointwise_poly(polyveck *r, const poly *a, const polyveck *v) {
    unsigned int i;
    const uint32_t *rhs;

    for(i = 0; i < K; i++) {
        if (i == 0) {
            rhs = a->coeffs;
        } else {
            rhs = NTT_LITE_INPUT_DIS;
        }
        ntt_lite_pwm((uint32_t*) &r->vec[i].coeffs, (uint32_t*) &v->vec[i].coeffs, rhs);
    }
}


void polyveck_pointwise_poly_sub(polyveck *r, const poly *a, const polyveck *v, const polyveck *u) {
    unsigned int i;

    for(i = 0; i < K; i++) {
        ntt_lite_pwm(NTT_LITE_OUTPUT_DIS, (uint32_t*) &v->vec[i].coeffs, (uint32_t*) &a->coeffs);
        ntt_lite_sub_rev((uint32_t*) &r->vec[i].coeffs, NTT_LITE_INPUT_DIS, (uint32_t*) &u->vec[i].coeffs);
    }
}


/*************************************************
* Name:        polyveck_chknorm
*
* Description: Check infinity norm of polynomials in vector of length K.
*              Assumes input polyveck to be reduced by polyveck_reduce().
*
* Arguments:   - const polyveck *v: pointer to vector
*              - int32_t B: norm bound
*
* Returns 0 if norm of all polynomials are strictly smaller than B <= (Q-1)/8
* and 1 otherwise.
**************************************************/
int polyveck_chknorm(const polyveck *v, uint32_t B) {
    unsigned int i;

    ntt_lite_set_bound(B);

    for(i = 0; i < K; i++)
        if(poly_chknorm(&v->vec[i], B))
            return 1;

    return 0;
}


/*************************************************
* Name:        polyveck_power2round
*
* Description: For all coefficients a of polynomials in vector of length K,
*              compute a0, a1 such that a mod^+ Q = a1*2^D + a0
*              with -2^{D-1} < a0 <= 2^{D-1}. Assumes coefficients to be
*              standard representatives.
*
* Arguments:   - polyveck *v1: pointer to output vector of polynomials with
*                              coefficients a1
*              - polyveck *v0: pointer to output vector of polynomials with
*                              coefficients a0
*              - const polyveck *v: pointer to input vector
**************************************************/
void polyveck_power2round(polyveck *v1, polyveck *v0, const polyveck *v) {
    unsigned int i;
    uint32_t mu[2] = {0, 1 << (32 - D)};

    ntt_lite_set_inv2(0xFFFFFFFF);
    ntt_lite_set_mu(mu, NTT_LITE_MODE_SINGLE);
    ntt_lite_set_bound(1 << D);

    for(i = 0; i < K; i++)
        poly_power2round(&v1->vec[i], &v0->vec[i], &v->vec[i]);

    poly_init_q();
}

/*************************************************
* Name:        polyveck_decompose
*
* Description: For all coefficients a of polynomials in vector of length K,
*              compute high and low bits a0, a1 such a mod^+ Q = a1*ALPHA + a0
*              with -ALPHA/2 < a0 <= ALPHA/2 except a1 = (Q-1)/ALPHA where we
*              set a1 = 0 and -ALPHA/2 <= a0 = a mod Q - Q < 0.
*              Assumes coefficients to be standard representatives.
*
* Arguments:   - polyveck *v1: pointer to output vector of polynomials with
*                              coefficients a1
*              - polyveck *v0: pointer to output vector of polynomials with
*                              coefficients a0
*              - const polyveck *v: pointer to input vector
**************************************************/
void polyveck_decompose(polyveck *v1, polyveck *v0, const polyveck *v) {
    unsigned int i;
    uint32_t mu[2] = {0x02008020, 0x2008};

    ntt_lite_set_inv2(GAMMA2_D >> 1);
    ntt_lite_set_mu(mu, NTT_LITE_MODE_SINGLE);
    ntt_lite_set_bound(GAMMA2 << 1);

    for(i = 0; i < K; i++)
        poly_decompose(&v1->vec[i], &v0->vec[i], &v->vec[i]);

    poly_init_q();
}


unsigned int polyveck_add_make_hint(polyveck *h, const polyveck *v0, const polyveck *v1, const polyveck *u) {
    unsigned int i;
    volatile uint32_t s;

    ntt_lite_set_bound(GAMMA2);
    ntt_lite_set_inv2(Q - GAMMA2);

    for(i = 0; i < K; i++) {
        ntt_lite_add(NTT_LITE_OUTPUT_DIS, (uint32_t*) v0->vec[i].coeffs, (uint32_t*) u->vec[i].coeffs);
        ntt_lite_make_hint((uint32_t*) &h->vec[i].coeffs, NTT_LITE_INPUT_DIS, (uint32_t*) v1->vec[i].coeffs);
        ntt_lite_sum((uint32_t*) &s, NTT_LITE_INPUT_DIS);
        if (s > OMEGA) {
            break;
        }
    }
    return s;
}

unsigned int polyveck_add_make_hint_packed(polyveck *h, const polyveck *v0, const uint8_t packed_w1[K * POLYW1_PACKEDBYTES], const polyveck *u) {
    unsigned int i;
    volatile uint32_t s;
    poly w1_tmp;

    ntt_lite_set_bound(GAMMA2);
    ntt_lite_set_inv2(Q - GAMMA2);

    for(i = 0; i < K; i++) {
        polyw1_unpack(&w1_tmp, &packed_w1[i * POLYW1_PACKEDBYTES]);

        ntt_lite_add(NTT_LITE_OUTPUT_DIS, (uint32_t*) v0->vec[i].coeffs, (uint32_t*) u->vec[i].coeffs);
        ntt_lite_make_hint((uint32_t*) &h->vec[i].coeffs, NTT_LITE_INPUT_DIS, (uint32_t*) w1_tmp.coeffs);
        ntt_lite_sum((uint32_t*) &s, NTT_LITE_INPUT_DIS);

        if(s > OMEGA) {
            break;
        }
    }

    return s;
}


/*************************************************
* Name:        polyveck_use_hint
*
* Description: Use hint vector to correct the high bits of input vector.
*
* Arguments:   - polyveck *w: pointer to output vector of polynomials with
*                             corrected high bits
*              - const polyveck *u: pointer to input vector
*              - const polyveck *h: pointer to input hint vector
**************************************************/
void polyveck_use_hint_pack_init(void) {
    uint32_t mu[2] = {0x02008020, 0x2008};

    ntt_lite_set_inv2(GAMMA2_D >> 1);
    ntt_lite_set_mu(mu, NTT_LITE_MODE_SINGLE);
    ntt_lite_set_bound(GAMMA2 << 1);
}


void polyveck_use_hint_pack(uint8_t r[K*POLYW1_PACKEDBYTES], const polyveck *u, const polyveck *h) {
    unsigned int i;

    polyveck_use_hint_pack_init();

    for(i = 0; i < K; i++)
        poly_use_hint_pack(&r[i*POLYW1_PACKEDBYTES], &u->vec[i], &h->vec[i]);
}


void polyveck_pack_w1(uint8_t r[K*POLYW1_PACKEDBYTES], const polyveck *w1) {
    unsigned int i;

    for(i = 0; i < K; i++)
        polyw1_pack(&r[i*POLYW1_PACKEDBYTES], &w1->vec[i]);
}
