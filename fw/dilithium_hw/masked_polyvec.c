#include "masked_polyvec.h"
#include "ntt_lite.h"
#include "masked_gadgets.h"
#include "masked_symmetric.h"


static void masked_polyvec_unpack(masked_poly *r, const uint8_t *a, unsigned int len, const uint32_t d, const uint32_t c, const unsigned int step) {
    unsigned int i, j;
    uint32_t rhs = c;
    uint32_t *rhs_ptr;

    for(i = 0; i < len; i++) {
        ntt_lite_decode(r[i].share[0].coeffs, (uint32_t*) a, d);
        a += step;
    }

    for(i = 0; i < len; i++) {
        masked_poly_mask(&r[i], &r[i].share[0]);
    }

    for (j = 0; j < MASKING_N; j++) {
        if (j == 0) {
                rhs = c;
        }
        else {
                rhs = 0;
        }
        for(i = 0; i < len; i++) {
            if (i == 0) {
                ntt_lite_set_bound(rhs);
            }
            ntt_lite_sub_rev_const(r[i].share[j].coeffs, r[i].share[j].coeffs);
        }
    }
}




void masked_polyvecl_eta_unpack(masked_polyvecl *r, const uint8_t *a) {
    masked_polyvec_unpack(r->vec, a, L, LOG_ETA, ETA, POLYETA_PACKEDBYTES);
}


void masked_polyvecl_t_eta_unpack(masked_polyvecl_t *r, const uint8_t *a) {
    unsigned int i;
    masked_poly_ptr r_ptr;

    for (i = 0; i < L; i++) {
        masked_polyvecl_t_to_poly_ptr(&r_ptr, r, i);
        masked_poly_ptr_unpack(&r_ptr, a + i * POLYETA_PACKEDBYTES, L, LOG_ETA, ETA);
    }
}


void masked_polyveck_eta_unpack(masked_polyveck *r, const uint8_t *a) {
    masked_polyvec_unpack(r->vec, a, K, LOG_ETA, ETA, POLYETA_PACKEDBYTES);
}


void masked_polyveck_t_eta_unpack(masked_polyveck_t *r, const uint8_t *a) {
    unsigned int i;
    masked_poly_ptr r_ptr;

    for (i = 0; i < K; i++) {
        masked_polyveck_t_to_poly_ptr(&r_ptr, r, i);
        masked_poly_ptr_unpack(&r_ptr, a + i * POLYETA_PACKEDBYTES, L, LOG_ETA, ETA);
    }
}


static void masked_polyvec_ntt(masked_poly *r, unsigned int len) {
    unsigned int i, j;

    for(j = 0; j < MASKING_N; j++)
        for(i = 0; i < len; i++)
            poly_ntt(&r[i].share[j]);
}


void masked_polyvecl_ntt(masked_polyvecl *r) {
    masked_polyvec_ntt(r->vec, L);
}


void masked_polyveck_ntt(masked_polyveck *r) {
    masked_polyvec_ntt(r->vec, K);
}


void masked_polyvecl_ntt_t(masked_polyvecl_t *r) {
    unsigned int i, j;

    for(i = 0; i < MASKING_N; i++)
        for(j = 0; j < L; j++)
            ntt_lite_forward_ntt((uint32_t*) &r->share[i].vec[j].coeffs, (uint32_t*) &r->share[i].vec[j].coeffs);
}


void masked_polyveck_ntt_t(masked_polyveck_t *r) {
    unsigned int i, j;

    for(i = 0; i < MASKING_N; i++)
        for(j = 0; j < K; j++)
            ntt_lite_forward_ntt((uint32_t*) &r->share[i].vec[j].coeffs, (uint32_t*) &r->share[i].vec[j].coeffs);
}



void masked_polyveck_invntt(masked_polyveck *r) {
    unsigned int i, j;

    for(j = 0; j < MASKING_N; j++)
        for(i = 0; i < K; i++) {
            if (i == (K - 1)){
               ntt_lite_set_clr(); 
            }
            poly_invntt(&r->vec[i].share[j]);
        }
}


void masked_polyveck_t_invntt(masked_polyveck_t *r) {
    unsigned int i, j;

    for(i = 0; i < MASKING_N; i++)
        for(j = 0; j < K; j++)
            ntt_lite_backward_ntt((uint32_t*) &r->share[i].vec[j].coeffs, (uint32_t*) &r->share[i].vec[j].coeffs);
}


void masked_polyvecl_uniform_gamma1(masked_polyvecl *y, const masked_crh rhoprime, uint16_t nonce) {
    unsigned int i;
    masked_stream256_init(rhoprime, L*nonce);
    for (i = 0; i < L; i++) {
        masked_poly_uniform_gamma1_fromhw(&y->vec[i], rhoprime, L*nonce + i + 1, i != (L - 1));
    }
}


void masked_polyvecl_t_uniform_gamma1(masked_polyvecl_t *y, const masked_crh rhoprime, uint16_t nonce) {
    unsigned int i, j;
    masked_poly_ptr y_ptr;
    masked_stream256_init(rhoprime, L*nonce);
    for (i = 0; i < L; i++) {
        for (j = 0; j < MASKING_N; j++) {
            y_ptr.share[j] = &y->share[j].vec[i];
        }

        masked_poly_ptr_uniform_gamma1_fromhw(&y_ptr, rhoprime, L*nonce + i + 1, i != (L - 1));
    }
}


void masked_polyvecl_pointwise_acc(masked_poly *w, const polyvecl *u, const masked_polyvecl *v)
{
    unsigned int i, j;
    uint32_t *dst;

    for (j = 0; j < MASKING_N; j++) {
        ntt_lite_pwm(NTT_LITE_OUTPUT_DIS, (uint32_t*) &v->vec[0].share[j].coeffs, (uint32_t*) &u->vec[0].coeffs);
        for(i = 1; i < L; i++) {
            if (i != (L - 1)) {
                dst = NTT_LITE_OUTPUT_DIS;
            }
            else {
                dst = (uint32_t*) w->share[j].coeffs;
                ntt_lite_set_clr();
            }
            ntt_lite_mac(dst, (uint32_t*) &v->vec[i].share[j].coeffs, (uint32_t*) &u->vec[i].coeffs);
        }
    }
}


void masked_polyvec_matrix_pointwise(masked_polyveck *t, const polyvecl mat[K], const masked_polyvecl *v) {
    unsigned int i;

    for(i = 0; i < K; i++) {
        masked_polyvecl_pointwise_acc(&t->vec[i], &mat[i], v);
    }
}



void masked_polyvecl_t_pointwise_acc(masked_poly_ptr *w, const polyvecl *u, const masked_polyvecl_t *v)
{
    unsigned int i, j;
    uint32_t *dst;

    for (j = 0; j < MASKING_N; j++) {
        ntt_lite_pwm(NTT_LITE_OUTPUT_DIS, (uint32_t*) &v->share[j].vec[0].coeffs, (uint32_t*) &u->vec[0].coeffs);
        for(i = 1; i < L; i++) {
            if (i != (L - 1)) {
                dst = NTT_LITE_OUTPUT_DIS;
            }
            else {
                dst = (uint32_t*) w->share[j]->coeffs;
                ntt_lite_set_clr();
            }
            ntt_lite_mac(dst, (uint32_t*) &v->share[j].vec[i].coeffs, (uint32_t*) &u->vec[i].coeffs);
        }
    }
}


void masked_polyvec_t_matrix_pointwise(masked_polyveck_t *t, const polyvecl mat[K], const masked_polyvecl_t *v) {
    unsigned int i;
    masked_poly_ptr t_ptr;

    for(i = 0; i < K; i++) {
        masked_polyveck_t_to_poly_ptr(&t_ptr, t, i);
        masked_polyvecl_t_pointwise_acc(&t_ptr, &mat[i], v);
    }
}




void masked_polyveck_decompose(polyveck *v1, masked_polyveck *v0, const masked_polyveck *v) {
    unsigned int i;
    for (i = 0; i < K; i++) {
        masked_poly_decompose(&v1->vec[i], &v0->vec[i], &v->vec[i]);
    }
}


void masked_polyveck_t_decompose(polyveck *v1, masked_polyveck_t *v0, const masked_polyveck_t *v) {
    unsigned int i, j;
    masked_poly_ptr v0_ptr, v_ptr;
    
    for (i = 0; i < K; i++) {
        for (j = 0; j < MASKING_N; j++) {
            v0_ptr.share[j] = &v0->share[j].vec[i];
            v_ptr.share[j] = &v->share[j].vec[i];
        }

        masked_poly_ptr_decompose(&v1->vec[i], &v0_ptr, &v_ptr);
    }
}


int masked_polyvecl_pointwise_add_invntt_chknorm(masked_polyvecl *r, const masked_polyvecl *v, const poly *c, const masked_polyvecl *u, uint32_t B) {
    unsigned int i;

    for (i = 0; i < L; i++) {
        if (masked_poly_pointwise_add_invntt_chknorm(&r->vec[i], &v->vec[i], c, &u->vec[i], B)) {
            return 1;
        }
    }
    return 0;
}


int masked_polyvecl_pointwise_add_invntt_chknorm_t(masked_polyvecl_t *r, const masked_polyvecl_t *v, const poly *c, masked_polyvecl_t *u, uint32_t B) {
    unsigned int i;
    masked_poly_ptr u_ptr, v_ptr, r_ptr;

    ntt_lite_set_bound(B - 1);

    for (i = 0; i < L; i++) {
        masked_polyvecl_t_to_poly_ptr(&r_ptr, r, i);
        masked_polyvecl_t_to_poly_ptr(&u_ptr, u, i);
        masked_polyvecl_t_to_poly_ptr(&v_ptr, v, i);

        if (masked_poly_ptr_pointwise_add_invntt_chknorm(&r_ptr, &v_ptr, c, &u_ptr, B)) {
            return 1;
        }
    }
    return 0;
}


int masked_polyveck_pointwise_invntt_sub_chknorm(masked_polyveck *r, const masked_polyveck *v, const poly *c, const masked_polyveck *u, uint32_t B) {
    unsigned int i;

    ntt_lite_set_bound(B);

    for(i = 0; i < K; i++) {
        if (masked_poly_pointwise_invntt_sub_chknorm(&r->vec[i], &v->vec[i], c, &u->vec[i], B)) {
            return 1;
        }
    }
    return 0;
}


int masked_polyveck_t_pointwise_invntt_sub_chknorm(masked_polyveck_t *r, const masked_polyveck_t *v, const poly *c, const masked_polyveck_t *u, masked_poly_ptr *temp, uint32_t B) {
    unsigned int i;
    masked_poly_ptr u_ptr, v_ptr, r_ptr;

    ntt_lite_set_bound(B);

    for(i = 0; i < K; i++) {
        masked_polyveck_t_to_poly_ptr(&r_ptr, r, i);
        masked_polyveck_t_to_poly_ptr(&u_ptr, u, i);
        masked_polyveck_t_to_poly_ptr(&v_ptr, v, i);
        
        if (masked_poly_ptr_pointwise_invntt_sub_chknorm(&r_ptr, &v_ptr, c, &u_ptr, temp, B)) {
            return 1;
        }
    }
    return 0;
}


void masked_polyvecl_unmask(polyvecl *a, const masked_polyvecl *r) {
    unsigned int i;
    for (i = 0; i < L; i++) {
        masked_poly_unmask(&a->vec[i], &r->vec[i]);
    }
}


void masked_polyveck_unmask(polyveck *a, const masked_polyveck *r) {
    unsigned int i;
    for (i = 0; i < K; i++) {
        masked_poly_unmask(&a->vec[i], &r->vec[i]);
    }
}


void masked_polyveck_t_unmask(polyveck *a, const masked_polyveck_t *r) {
    unsigned int i;
    masked_poly_ptr r_ptr;
    for (i = 0; i < K; i++) {
        masked_polyveck_t_to_poly_ptr(&r_ptr, r, i);
        masked_poly_ptr_unmask(&a->vec[i], &r_ptr);
    }
}


void masked_polyvecl_t_unmask(polyvecl *a, const masked_polyvecl_t *r) {
    unsigned int i;
    masked_poly_ptr r_ptr;
    for (i = 0; i < L; i++) {
        masked_polyvecl_t_to_poly_ptr(&r_ptr, r, i);
        masked_poly_ptr_unmask(&a->vec[i], &r_ptr);
    }
}