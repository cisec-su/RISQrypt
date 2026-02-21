#include "masked_poly.h"
#include "masked_gadgets.h"
#include "masked_symmetric.h"
#include "ntt_lite.h"


void masked_poly_ptr_mask(const masked_poly_ptr *r, const poly_u *a) {
    masked_gadgets_mask_poly_ptr(r, a);
}


void masked_poly_ptr_unmask(poly *a, masked_poly_ptr_const *r) {
    unsigned int i;
    uint32_t *lhs;
    uint32_t *dst;


    for (i = 1; i < MASKING_N; i++) {
        if (i == 1) {
            lhs = (uint32_t*) &r->share[0]->coeffs;
        }
        else {
            lhs = NTT_LITE_INPUT_DIS;
        }
        if (i != (MASKING_N - 1)) {
            dst = NTT_LITE_OUTPUT_DIS;
        }
        else {
            dst = (uint32_t*) &a->coeffs;
        }
        ntt_lite_add(dst, lhs, (uint32_t*) &r->share[i]->coeffs);
    }
}


void masked_poly_ptr_unpack(const masked_poly_ptr *r, const uint8_t *a, unsigned int len, const uint32_t d, const uint32_t c) {
    unsigned int i;
    uint32_t rhs = c;
    uint32_t *rhs_ptr;

    ntt_lite_decode((uint32_t*) r->share[0]->coeffs, (uint32_t*) a, d);

    masked_poly_ptr_mask(r, r->share[0]);

    for (i = 0; i < MASKING_N; i++) {
        if (i == 0) {
                rhs = c;
        }
        else {
                rhs = 0;
        }
        ntt_lite_set_bound(rhs);
        ntt_lite_set_clr();
        ntt_lite_sub_rev_const(r->share[i]->coeffs, r->share[i]->coeffs);
    }
}


void masked_poly_ptr_uniform_gamma1_fromhw_inner(const masked_poly_ptr *y, const masked_crh rhoprime, const uint32_t buf[MASKING_N][POLYZ_PACKEDBYTES >> 2]) {
    unsigned int i;

    for(i = 0; i < MASKING_N; i++) {
        ntt_lite_set_clr_with_twiddle();
        ntt_lite_decode((uint32_t*) y->share[i]->coeffs, (uint32_t*) buf[i], LOG_GAMMA1);
    }

    masked_gadgets_B2A_q_ptr(y, (masked_poly_ptr_const*) y);
    ntt_lite_set_bound(GAMMA1);
    for(i = 0; i < MASKING_N; i++) {
        ntt_lite_set_clr();
        ntt_lite_sub_rev_const((uint32_t*) y->share[i]->coeffs, (uint32_t*) y->share[i]->coeffs);
        ntt_lite_set_bound(0);
    }
}


void masked_poly_ptr_uniform_gamma1_fromhw(const masked_poly_ptr *y, const masked_crh rhoprime, uint16_t nonce_next, int init_next) {
    unsigned int i;
    uint32_t buf[MASKING_N][POLYZ_PACKEDBYTES >> 2];

    masked_stream256_squeeze((masked_flat_ptr) buf, POLYZ_PACKEDBYTES);
    if (init_next) {
        masked_stream256_init(rhoprime, nonce_next);
    }

    masked_poly_ptr_uniform_gamma1_fromhw_inner(y, rhoprime, buf);
}


int masked_poly_ptr_chknorm(const masked_poly_ptr *r, const masked_poly_ptr *temp, uint32_t B) {
    unsigned int i;
    int flag;
    masked_poly_ptr_const ptr;
    uint32_t B2 = (B << 1) - 1;
    uint32_t *dst;

    ntt_lite_set_bound(B - 1);
    ntt_lite_add_const((uint32_t*) temp->share[MASKING_N - 1]->coeffs, NTT_LITE_INPUT_DIS);

    for (i = 0; i < MASKING_N; i++) {
        if (i == (MASKING_N - 1)) {
            ptr.share[i] = temp->share[MASKING_N - 1];
        }
        else {
            ptr.share[i] = r->share[i];
        }
    }

    // modulus switching from q to 2^32
    masked_gadgets_init_q();
    masked_gadgets_A2B_q_ptr(temp, &ptr);

    masked_gadgets_init_2k(0xFFFFFF);
    masked_gadgets_B2A_2k_ptr(temp, (masked_poly_ptr_const*) temp);

    ntt_lite_set_q(1 << 24);
    ntt_lite_set_bound(B2);
    ntt_lite_sub_const((uint32_t*) temp->share[MASKING_N - 1]->coeffs, (uint32_t*) temp->share[MASKING_N - 1]->coeffs);
    masked_gadgets_A2B_2k_ptr(temp, (masked_poly_ptr_const*) temp);

    ntt_lite_set_q(1);
    for (i = 0; i < MASKING_N; i++) {
        if (i == (MASKING_N - 1)) {
            dst = NTT_LITE_OUTPUT_DIS;
        }
        else {
            ntt_lite_set_clr_with_twiddle();
            dst = (uint32_t*) temp->share[i]->coeffs;
        }
        ntt_lite_decompress_floor(dst, (uint32_t*) temp->share[i]->coeffs, 23);
    }
    
    // unmasking
#ifndef TTEST
    ntt_lite_set_q(2);
    for (i = 0; i < (MASKING_N - 1); i++) {
        ntt_lite_add(NTT_LITE_OUTPUT_DIS, NTT_LITE_INPUT_DIS, (uint32_t*) temp->share[i]->coeffs);
    }
    ntt_lite_set_bound(1);
    ntt_lite_add_const(NTT_LITE_OUTPUT_DIS, NTT_LITE_INPUT_DIS);

    ntt_lite_set_bound(0);
    flag = ntt_lite_chkinfnorm(NTT_LITE_INPUT_DIS);
#else
    flag = NTT_LITE_CHKNORM_SUCC;
#endif
    poly_set_q(); // reset the modulus to Dilithium's Q
    if (flag == NTT_LITE_CHKNORM_FAIL) {
        return 1;
    }
    else {
        return 0;
    }
}


int masked_poly_ptr_pointwise_add_invntt_chknorm(const masked_poly_ptr *r_ptr, const masked_poly_ptr_const *v_ptr, const poly *c, const masked_poly_ptr *u_ptr, uint32_t B) {
    masked_poly temp;
    unsigned int i, j;
    int flag;
    poly *ptr[MASKING_N];
    uint32_t B2 = (B << 1) - 1;
    uint32_t *dst;
    ntt_lite_set_bound(B - 1);

    for (i = 0; i < MASKING_N; i++) {
        ntt_lite_pwm(NTT_LITE_OUTPUT_DIS, (uint32_t*) v_ptr->share[i]->coeffs, (uint32_t*) &c->coeffs);
        ntt_lite_add(NTT_LITE_OUTPUT_DIS, NTT_LITE_INPUT_DIS, (uint32_t*) u_ptr->share[i]->coeffs);
        poly_init_invntt();
        if (i != (MASKING_N - 1)) {
            ntt_lite_set_clr_with_twiddle();
        }
        ntt_lite_backward_ntt((uint32_t*) r_ptr->share[i]->coeffs, NTT_LITE_INPUT_DIS);
    }
    return masked_poly_ptr_chknorm(r_ptr, u_ptr, B);
}


int masked_poly_ptr_pointwise_invntt_sub_chknorm(const masked_poly_ptr *r, const masked_poly_ptr_const *v, const poly *c, const masked_poly_ptr_const *u, const masked_poly_ptr *temp, uint32_t B) {
    unsigned int i;
    int flag;
    for (i = 0; i < MASKING_N; i++) {
        ntt_lite_pwm(NTT_LITE_OUTPUT_DIS, (uint32_t*) v->share[i]->coeffs, (uint32_t*) &c->coeffs);
        poly_init_invntt();
        ntt_lite_backward_ntt(NTT_LITE_OUTPUT_DIS, NTT_LITE_INPUT_DIS);
        if (i != (MASKING_N - 1)) {
            ntt_lite_set_clr_with_twiddle();
        }
        ntt_lite_sub_rev((uint32_t*) r->share[i]->coeffs, NTT_LITE_INPUT_DIS, (uint32_t*) u->share[i]->coeffs);

    }
    return masked_poly_ptr_chknorm(r, temp, B);
}


// https://eprint.iacr.org/2023/896.pdf Algorithm 10-11
void masked_poly_ptr_decompose(poly *v1, const masked_poly_ptr *v0, const masked_poly_ptr_const *v) {
#if DILITHIUM_MODE == 2
    #error "This implementation requires DILITHIUM_MODE = 3 or 5"
#else
    unsigned int i, j;
    uint32_t *lhs, *dst;
    const uint32_t gamma = GAMMA2_D >> 1;
    const uint32_t mu[2] = {0, 1 << 28}; // hard-coded
    masked_poly temp;

    ntt_lite_set_bound(Q - gamma);
    for (i = 0; i < MASKING_N; i++) {
        if (i == 0) {
            dst = NTT_LITE_INPUT_DIS;
        }
        else {
            dst = (uint32_t*) &temp.share[i].coeffs;
        }
        ntt_lite_set_clr();
        ntt_lite_mul_const(dst, (uint32_t*) &v->share[i]->coeffs);
        if (i == 0) {
            ntt_lite_set_bound((Q - 1) >> 1);
            ntt_lite_add_const((uint32_t*) &temp.share[i].coeffs, NTT_LITE_INPUT_DIS);
            ntt_lite_set_bound(Q - gamma);
        }
    }

    masked_gadgets_A2B_q(&temp, &temp);

    ntt_lite_set_q(gamma);
    ntt_lite_set_mu(mu, NTT_LITE_MODE_SINGLE);
    ntt_lite_set_bound(1);
    for (i = 0; i < MASKING_N; i++) {
        ntt_lite_set_clr();
        ntt_lite_mul_const((uint32_t*) &temp.share[i].coeffs, (uint32_t*) &temp.share[i].coeffs);
    }

#ifdef TTEST
    // Setting a fixed value for v1 to avoid leakage in t-test setting.
    // Recall that v1 is public
    for (i = 0; i < N; i++) {
        v1->coeffs[i] = 1;
    }
#else
    for (i = 0; i < N; i++) {
        v1->coeffs[i] = temp.share[0].coeffs[i];
        for (j = 1; j < MASKING_N; j++) {
            v1->coeffs[i] ^= temp.share[j].coeffs[i];
        }
    }
#endif
    
    poly_init_q();
    ntt_lite_set_bound(GAMMA2 << 1);
    ntt_lite_mul_const(NTT_LITE_OUTPUT_DIS, (uint32_t*) v1->coeffs);
    ntt_lite_set_clr_with_twiddle();
    ntt_lite_sub_rev((uint32_t*) &v0->share[0]->coeffs, NTT_LITE_INPUT_DIS, (uint32_t*) &v->share[0]->coeffs);
    ntt_lite_set_bound(0);
    for (i = 1; i < MASKING_N; i++) {
        ntt_lite_set_clr();
        ntt_lite_add_const(v0->share[i]->coeffs, v->share[i]->coeffs);
    }


#endif
}