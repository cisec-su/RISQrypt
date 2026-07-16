#include "masked_polyvec.h"
#include "ntt_lite.h"
#include "masked_gadgets.h"
#include "masked_symmetric.h"

#define INV2 0x3ff001

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


static __attribute__((noinline, noclone)) void masked_polyvecl_share_ntt(polyvecl_u *r) {
    unsigned int j;

    for (j = 0; j < L; j++) {
        ntt_lite_forward_ntt((uint32_t*) &r->vec[j].coeffs, (uint32_t*) &r->vec[j].coeffs);
    }
}

void masked_polyvecl_ntt(masked_polyvecl *r) {
    masked_polyvecl_share_ntt(&r->share[0]);
    masked_polyvecl_share_ntt(&r->share[1]);
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
        for(j = 0; j < K; j++) {
            if (j == (K - 1)) {
                ntt_lite_set_clr();
            }
            ntt_lite_backward_ntt((uint32_t*) &r->share[i].vec[j].coeffs, (uint32_t*) &r->share[i].vec[j].coeffs);
        }
}


void masked_polyvecl_uniform_gamma1(masked_polyvecl *y, const masked_crh rhoprime, uint16_t nonce) {
    unsigned int i, j;
    masked_poly_ptr y_ptr;
    masked_stream256_init(rhoprime, L*nonce);
    for (i = 0; i < L; i++) {
        masked_polyvecl_to_poly_ptr(&y_ptr, y, i);

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
                ntt_lite_set_clr_with_twiddle();
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

void masked_polyvec_matrix_pointwise_onthefly(masked_polyveck *t, const uint8_t rho[SEEDBYTES], const masked_polyvecl *v) {
    unsigned int i, j, i_next, j_next;
    polyvecl row;
    masked_poly_ptr t_ptr;

    i_next = 0;
    j_next = 1;

    stream128_init(rho, 0);

    for(i = 0; i < K; i++) {
        ntt_lite_set_inv2(STREAM128_BLOCKBYTES >> 2);
        ntt_lite_set_bound(Q);

        for(j = 0; j < L; j++) {
            poly_uniform_fromhw(&row.vec[j], rho, (i_next << 8) + j_next, (i != (K - 1)) || (j != (L - 1)));

            if(j_next == (L - 1)) {
                j_next = 0;
                i_next++;
            }
            else {
                j_next++;
            }
        }

        ntt_lite_set_inv2(INV2);

        masked_polyveck_to_poly_ptr(&t_ptr, t, i);
        masked_polyvecl_pointwise_acc(&t_ptr, &row, v);
    }
}


static void masked_poly_u_to_poly_ptr(masked_poly_ptr *dst, masked_poly_u *src) {
    unsigned int i;

    for (i = 0; i < MASKING_N; i++) {
        dst->share[i] = &src->share[i];
    }
}


static void masked_poly_u_to_poly_ptr_const(masked_poly_ptr_const *dst, const masked_poly_u *src) {
    unsigned int i;

    for (i = 0; i < MASKING_N; i++) {
        dst->share[i] = &src->share[i];
    }
}


static void masked_poly_ptr_invntt(masked_poly_ptr *r) {
    unsigned int i;

    for (i = 0; i < MASKING_N; i++) {
        poly_init_invntt();

        if (i != (MASKING_N - 1)) {
            ntt_lite_set_clr_with_twiddle();
        }

        ntt_lite_backward_ntt((uint32_t*) r->share[i]->coeffs, (uint32_t*) r->share[i]->coeffs);
    }
}


void masked_polyvec_matrix_pointwise_decompose_onthefly(uint8_t packed_w1[K * POLYW1_PACKEDBYTES], masked_polyveck *w0, const uint8_t rho[SEEDBYTES], const masked_polyvecl *v) {
    unsigned int i, j, nonce_next;
    polyvecl row;
    poly w1_tmp;
    masked_poly_u w_tmp;
    masked_poly_ptr w_tmp_ptr;
    masked_poly_ptr_const w_tmp_ptr_const;
    masked_poly_ptr w0_ptr;

    masked_poly_u_to_poly_ptr(&w_tmp_ptr, &w_tmp);
    masked_poly_u_to_poly_ptr_const(&w_tmp_ptr_const, &w_tmp);

    nonce_next = 1;

    stream128_init(rho, 0);

    for(i = 0; i < K; i++) {
        ntt_lite_set_inv2(STREAM128_BLOCKBYTES >> 2);
        ntt_lite_set_bound(Q);

        for (j = 0; j < L; j++) {
            poly_uniform_fromhw(&row.vec[j], rho, nonce_next, (i != (K - 1)) || (j != (L - 1)));

            if (j == (L - 2)) {
                nonce_next += (1 << 8) - (L - 1);
            } else {
                nonce_next++;
            }
        }

        ntt_lite_set_inv2(INV2);

        masked_polyvecl_pointwise_acc(&w_tmp_ptr, &row, v);

        masked_poly_ptr_invntt(&w_tmp_ptr);

        masked_polyveck_to_poly_ptr(&w0_ptr, w0, i);
        masked_poly_ptr_decompose(&w1_tmp, &w0_ptr, &w_tmp_ptr_const);

        polyw1_pack(&packed_w1[i * POLYW1_PACKEDBYTES], &w1_tmp);
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