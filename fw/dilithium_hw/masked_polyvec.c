#include "masked_polyvec.h"
#include "ntt_lite.h"
#include "masked_gadgets.h"
#include "masked_symmetric.h"



void masked_polyvecl_eta_unpack(masked_polyvecl *r, const uint8_t *a) {
    unsigned int i;
    masked_poly_ptr r_ptr;

    for (i = 0; i < L; i++) {
        masked_polyvecl_to_poly_ptr(&r_ptr, r, i);
        masked_poly_ptr_unpack(&r_ptr, a + i * POLYETA_PACKEDBYTES, L, LOG_ETA, ETA);
    }
}


void masked_polyveck_eta_unpack(masked_polyveck *r, const uint8_t *a) {
    unsigned int i;
    masked_poly_ptr r_ptr;

    for (i = 0; i < K; i++) {
        masked_polyveck_to_poly_ptr(&r_ptr, r, i);
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
    unsigned int i, j;

    for(i = 0; i < MASKING_N; i++)
        for(j = 0; j < L; j++)
            ntt_lite_forward_ntt((uint32_t*) &r->share[i].vec[j].coeffs, (uint32_t*) &r->share[i].vec[j].coeffs);
}


void masked_polyveck_ntt(masked_polyveck *r) {
    unsigned int i, j;

    for(i = 0; i < MASKING_N; i++)
        for(j = 0; j < K; j++)
            ntt_lite_forward_ntt((uint32_t*) &r->share[i].vec[j].coeffs, (uint32_t*) &r->share[i].vec[j].coeffs);
}


void masked_polyveck_invntt(masked_polyveck *r) {
    unsigned int i, j;

    for(i = 0; i < MASKING_N; i++)
        for(j = 0; j < K; j++)
            ntt_lite_backward_ntt((uint32_t*) &r->share[i].vec[j].coeffs, (uint32_t*) &r->share[i].vec[j].coeffs);
}


void masked_polyvecl_uniform_gamma1(masked_polyvecl *y, const masked_crh rhoprime, uint16_t nonce) {
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


void masked_polyvecl_pointwise_acc(masked_poly_ptr *w, const polyvecl *u, const masked_polyvecl *v)
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


void masked_polyvec_matrix_pointwise(masked_polyveck *t, const polyvecl mat[K], const masked_polyvecl *v) {
    unsigned int i;
    masked_poly_ptr t_ptr;

    for(i = 0; i < K; i++) {
        masked_polyveck_to_poly_ptr(&t_ptr, t, i);
        masked_polyvecl_pointwise_acc(&t_ptr, &mat[i], v);
    }
}


void masked_polyveck_decompose(polyveck *v1, masked_polyveck *v0, const masked_polyveck *v) {
    unsigned int i, j;
    masked_poly_ptr_const v_ptr;
    masked_poly_ptr v0_ptr;
    
    for (i = 0; i < K; i++) {
        masked_polyveck_to_poly_ptr_const(&v_ptr, v, i);
        masked_polyveck_to_poly_ptr(&v0_ptr, v0, i);

        masked_poly_ptr_decompose(&v1->vec[i], &v0_ptr, &v_ptr);
    }
}


int masked_polyvecl_pointwise_add_invntt_chknorm(masked_polyvecl *r, const masked_polyvecl *v, const poly *c, masked_polyvecl *u, uint32_t B) {
    unsigned int i;
    masked_poly_ptr_const v_ptr;
    masked_poly_ptr u_ptr, r_ptr;

    ntt_lite_set_bound(B - 1);

    for (i = 0; i < L; i++) {
        masked_polyvecl_to_poly_ptr(&r_ptr, r, i);
        masked_polyvecl_to_poly_ptr(&u_ptr, u, i);
        masked_polyvecl_to_poly_ptr_const(&v_ptr, v, i);

        if (masked_poly_ptr_pointwise_add_invntt_chknorm(&r_ptr, &v_ptr, c, &u_ptr, B)) {
            return 1;
        }
    }
    return 0;
}


int masked_polyveck_pointwise_invntt_sub_chknorm(masked_polyveck *r, const masked_polyveck *v, const poly *c, const masked_polyveck *u, masked_poly_ptr *temp, uint32_t B) {
    unsigned int i;
    masked_poly_ptr_const u_ptr, v_ptr;
    masked_poly_ptr r_ptr;

    ntt_lite_set_bound(B);

    for(i = 0; i < K; i++) {
        masked_polyveck_to_poly_ptr(&r_ptr, r, i);
        masked_polyveck_to_poly_ptr_const(&u_ptr, u, i);
        masked_polyveck_to_poly_ptr_const(&v_ptr, v, i);
        
        if (masked_poly_ptr_pointwise_invntt_sub_chknorm(&r_ptr, &v_ptr, c, &u_ptr, temp, B)) {
            return 1;
        }
    }
    return 0;
}


void masked_polyveck_unmask(polyveck *a, const masked_polyveck *r) {
    unsigned int i;
    masked_poly_ptr_const r_ptr;
    for (i = 0; i < K; i++) {
        masked_polyveck_to_poly_ptr_const(&r_ptr, r, i);
        masked_poly_ptr_unmask(&a->vec[i], &r_ptr);
    }
}


void masked_polyvecl_unmask(polyvecl *a, const masked_polyvecl *r) {
    unsigned int i;
    masked_poly_ptr_const r_ptr;
    
    for (i = 0; i < L; i++) {
        masked_polyvecl_to_poly_ptr_const(&r_ptr, r, i);
        masked_poly_ptr_unmask(&a->vec[i], &r_ptr);
    }
}