#include "masked_poly.h"
#include "masked_gadgets.h"
#include "masked_symmetric.h"
#include "ntt_lite.h"




void masked_poly_mask(masked_poly *r, const poly *a) {
    masked_gadgets_mask_poly(r, a);
}


void masked_poly_unmask(poly *a, const masked_poly *r) {
    unsigned int i;
    uint32_t *lhs;
    uint32_t *dst;


    for (i = 1; i < MASKING_N; i++) {
        if (i == 1) {
            lhs = (uint32_t*) &r->share[0].coeffs;
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
        ntt_lite_add(dst, lhs, (uint32_t*) &r->share[i].coeffs);
    }
}



void masked_poly_ntt(masked_poly *r) {
    unsigned int i;

    for(i = 0; i < MASKING_N; i++) {
        poly_ntt(&r->share[i]);
    }
}


void masked_poly_invntt(masked_poly *r) {
    unsigned int i;

    for(i = 0; i < MASKING_N; i++) {
        poly_invntt(&r->share[i]);
    }
}


void masked_poly_uniform_gamma1(masked_poly *y, const masked_crh rhoprime, uint16_t nonce) {
    unsigned int i;
    uint8_t buf[MASKING_N][POLYZ_PACKEDBYTES];
    uint32_t rhs = GAMMA1;
    uint32_t *rhs_ptr;

    dilithium_masked_shake256_absorb_nonce((masked_flat_ptr) buf, POLYZ_PACKEDBYTES, (masked_flat_ptr) rhoprime, CRHBYTES, nonce);

    for(i = 0; i < MASKING_N; i++) {
        ntt_lite_decode((uint32_t*) y->share[i].coeffs, (uint32_t*) buf[i], LOG_GAMMA1);
    }

    masked_gadgets_B2A_q(y, y);
    
    for(i = 0; i < MASKING_N; i++) {
        if (i == 2) {
            rhs_ptr = NTT_LITE_INPUT_DIS;
        }
        else {
            rhs_ptr = &rhs;
        }
        if (i == 1) {
            rhs = 0;
        }
        ntt_lite_sub_rev_const((uint32_t*) y->share[i].coeffs, (uint32_t*) y->share[i].coeffs, rhs_ptr);
    }

}


void masked_poly_pointwise_acc(masked_poly *c, const poly *a, const masked_poly *b) {
    unsigned int i;

    for (i = 0; i < MASKING_N; i++) {
        poly_pointwise_acc(&c->share[i], a, &b->share[i]);
    }
}


void masked_poly_pointwise(masked_poly *c, const poly *a, const masked_poly *b) {
    unsigned int i;
    uint32_t *rhs;

    for (i = 0; i < MASKING_N; i++) {
        if ((i == 0) || 1) {
            rhs = (uint32_t*) &a->coeffs;
        }
        else {
            rhs = NTT_LITE_INPUT_DIS;
        }
        ntt_lite_pwm((uint32_t*) &c->share[i].coeffs, (uint32_t*) &b->share[i].coeffs, rhs);
    }
}


static int masked_poly_chknorm(const masked_poly *r, uint32_t B) {
    unsigned int i;
    int flag;
    masked_poly temp;
    const poly *ptr[MASKING_N];
    uint32_t B2 = (B << 1) - 1;
    uint32_t *dst;

    ntt_lite_set_bound(B - 1);
    ntt_lite_add_const((uint32_t*) &temp.share[MASKING_N - 1].coeffs, NTT_LITE_INPUT_DIS, NTT_LITE_INPUT_DIS);

    for (i = 0; i < MASKING_N; i++) {
        if (i == (MASKING_N - 1)) {
            ptr[i] = &temp.share[MASKING_N - 1];
        }
        else {
            ptr[i] = &r->share[i];
        }
    }

    // modulus switching from q to 2^32
    masked_gadgets_init_q();
    masked_gadgets_A2B_q_ptr(&temp, ptr);
    masked_gadgets_init_2k(0xFFFFFFFF);
    masked_gadgets_B2A_2k(&temp, &temp);

    ntt_lite_set_q(0);
    ntt_lite_set_bound(B2);

    ntt_lite_sub_const((uint32_t*) &temp.share[0].coeffs, (uint32_t*) &temp.share[0].coeffs, NTT_LITE_INPUT_DIS);
    
    masked_gadgets_A2B_2k(&temp, &temp);

    ntt_lite_set_q(1);
    for (i = 0; i < MASKING_N; i++) {
        if (i == (MASKING_N - 1)) {
            dst = NTT_LITE_OUTPUT_DIS;
        }
        else {
            dst = (uint32_t*) &temp.share[i].coeffs;
        }
        ntt_lite_decompress_floor(dst, (uint32_t*) temp.share[i].coeffs, 31);
    }
    
    // unmasking
    ntt_lite_set_q(2);
    for (i = 0; i < (MASKING_N - 1); i++) {
        ntt_lite_add(NTT_LITE_OUTPUT_DIS, NTT_LITE_INPUT_DIS, (uint32_t*) temp.share[i].coeffs);
    }
    ntt_lite_set_bound(1);
    ntt_lite_add_const(NTT_LITE_OUTPUT_DIS, NTT_LITE_INPUT_DIS, NTT_LITE_INPUT_DIS);

    ntt_lite_set_bound(0);
    flag = ntt_lite_chknorm(NTT_LITE_INPUT_DIS);
    poly_set_q(); // reset the modulus to Dilithium's Q
    if (flag == NTT_LITE_CHKNORM_FAIL) {
        return 1;
    }
    else {
        return 0;
    }
}


int masked_poly_pointwise_add_invntt_chknorm(masked_poly *r, const masked_poly *v, const poly *c, const masked_poly *u, uint32_t B) {
    masked_poly temp;
    unsigned int i, j;
    int flag;
    poly *ptr[MASKING_N];
    uint32_t B2 = (B << 1) - 1;
    uint32_t *dst;

    ntt_lite_set_bound(B - 1);

    for (i = 0; i < MASKING_N; i++) {
        ntt_lite_pwm(NTT_LITE_OUTPUT_DIS, (uint32_t*) &v->share[i].coeffs, (uint32_t*) &c->coeffs);
        ntt_lite_add(NTT_LITE_OUTPUT_DIS, NTT_LITE_INPUT_DIS, (uint32_t*) u->share[i].coeffs);
        poly_init_invntt();
        ntt_lite_backward_ntt((uint32_t*) &r->share[i].coeffs, NTT_LITE_INPUT_DIS);
    }

    return masked_poly_chknorm(r, B);
}



int masked_poly_pointwise_invntt_sub_chknorm(masked_poly *r, const masked_poly *v, const poly *c, const masked_poly *u, uint32_t B) {
    unsigned int i;
    int flag;

    for (i = 0; i < MASKING_N; i++) {
        ntt_lite_pwm(NTT_LITE_OUTPUT_DIS, (uint32_t*) &v->share[i].coeffs, (uint32_t*) &c->coeffs);
        poly_init_invntt();
        ntt_lite_backward_ntt(NTT_LITE_OUTPUT_DIS, NTT_LITE_INPUT_DIS);
        ntt_lite_sub_rev((uint32_t*) r->share[i].coeffs, NTT_LITE_INPUT_DIS, (uint32_t*) u->share[i].coeffs);

    }

    return masked_poly_chknorm(r, B);
}

// https://eprint.iacr.org/2023/896.pdf Algorithm 10-11
void masked_poly_decompose(poly *v1, masked_poly *v0, const masked_poly *v) {
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
        ntt_lite_mul_const(dst, (uint32_t*) &v->share[i].coeffs, NTT_LITE_INPUT_DIS);
        if (i == 0) {
            ntt_lite_set_bound((Q - 1) >> 1);
            ntt_lite_add_const((uint32_t*) &temp.share[i].coeffs, NTT_LITE_INPUT_DIS, NTT_LITE_INPUT_DIS);
            ntt_lite_set_bound(Q - gamma);
        }
    }

    masked_gadgets_A2B_q(&temp, &temp);

    ntt_lite_set_q(gamma);
    ntt_lite_set_mu(mu, NTT_LITE_MODE_SINGLE);
    ntt_lite_set_bound(1);
    for (i = 0; i < MASKING_N; i++) {
        ntt_lite_mul_const((uint32_t*) &temp.share[i].coeffs, (uint32_t*) &temp.share[i].coeffs, NTT_LITE_INPUT_DIS);
    }

    for (i = 0; i < N; i++) {
        v1->coeffs[i] = temp.share[0].coeffs[i];
        for (j = 1; j < MASKING_N; j++) {
            v1->coeffs[i] ^= temp.share[j].coeffs[i];
        }
    }
    
    poly_init_q();
    ntt_lite_set_bound(GAMMA2 << 1);
    ntt_lite_mul_const(NTT_LITE_OUTPUT_DIS, (uint32_t*) v1->coeffs, NTT_LITE_INPUT_DIS);

    ntt_lite_sub_rev((uint32_t*) &v0->share[0].coeffs, NTT_LITE_INPUT_DIS, (uint32_t*) &v->share[0].coeffs);
    for (i = 1; i < MASKING_N; i++) {
        for (j = 0; j < N; j++) {
            v0->share[i].coeffs[j] = v->share[i].coeffs[j];
        }
    }


#endif
}