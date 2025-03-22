#include "rng.h"
#include "masked_gadgets.h"
#include "ntt_lite.h"
#include "masked_cbd.h"
#include "masked_symmetric.h"
#include "masked_poly.h"
#include "masked_polyvec.h"


void masked_polyvec_ntt(masked_polyvec *r) {
    unsigned int i, j;
    for (i = 0; i < MASKING_N; i++) {
        for (j = 0; j < KYBER_K; j++) {
            ntt_lite_forward_ntt((uint32_t*) r->share[i].vec[j].coeffs, (uint32_t*) r->share[i].vec[j].coeffs);
        }
    }
}


void masked_polyvec_pointwise_acc_invntt_i(masked_polyvec *r, const masked_polyvec *a, const polyvec *b, unsigned int i) {
    unsigned int j;
    for (j = 0; j < MASKING_N; j++) {
        polyvec_pointwise_acc_invntt(&(r->share[j].vec[i]), &(a->share[j]), b);
    }
}


void masked_polyvec_pointwise_acc_invntt(masked_poly *r, const masked_polyvec *a, const polyvec *b) {
    unsigned int i;
    for (i = 0; i < MASKING_N; i++) {
        polyvec_pointwise_acc_invntt(&(r->share[i]), &(a->share[i]), b);
    }
}


void masked_polyvec_add(masked_polyvec *r, const masked_polyvec *a, const masked_polyvec *b) {
    unsigned int i, j;
    for (i = 0; i < MASKING_N; i++) {
        for (j = 0; j < KYBER_K; j++) {
            poly_add(&(r->share[i].vec[j]), &(a->share[i].vec[j]), &(b->share[i].vec[j]));
        }
    }
}


void masked_polyvec_compress(masked_polyvec *r, const masked_polyvec *a) {
    unsigned int i;
    const poly *a_[MASKING_N];
    poly *r_[MASKING_N];
    for (i = 0; i < KYBER_K; i++) {
        a_[0] = &(a->share[0].vec[i]);
        a_[1] = &(a->share[1].vec[i]);
        r_[0] = &(r->share[0].vec[i]);
        r_[1] = &(r->share[1].vec[i]);
        masked_poly_compress_du(r_, a_);
    }
}



void masked_polyvec_cmp(masked_polyvec *r, const masked_polyvec *a, const polyvec *b) {
    unsigned int i;
    for (i = 0; i < MASKING_N; i++) {
        poly_sub(&(r->share[i].vec[0]), &(r->share[i].vec[0]), &(b->vec[i]));
    }
}



void masked_polyvec_mask(masked_polyvec *r, polyvec *a) {
    unsigned int i, j, k;
    uint16_t t;

    for(k = 0; k < KYBER_K; k++) {
        for(j = 0; j < KYBER_N; j++) {
            t = 0;
            for(i = 0; i < MASKING_N - 1; i++) {
                r->share[i].vec[k].coeffs[j] = rand16() & 0x7FF;
                t = t + r->share[i].vec[k].coeffs[j];
                if (t >= KYBER_Q) {
                    t -= KYBER_Q;
                }
            }
            r->share[MASKING_N - 1].vec[k].coeffs[j] = a->vec[k].coeffs[j] - t;
            if (r->share[MASKING_N - 1].vec[k].coeffs[j] < 0) {
                r->share[MASKING_N - 1].vec[k].coeffs[j] += KYBER_Q;
            }
        }
    }
}


void masked_polyvec_getnoise_eta1(masked_polyvec *r, const masked_sym seed, uint8_t *nonce) {
#if MASKING_N != 2
#error "This implementation requires MASKING_N = 2"
#endif
    uint8_t buf[MASKING_N][(KYBER_N*KYBER_ETA1*2)/8];
    masked_ptr ptr = {buf[0], buf[1]};
    unsigned int i;
    for (i = 0; i < KYBER_K; i++) {
        print_string("seed0: ");
        print_hex(seed[0], KYBER_SYMBYTES);
        print_string("\nseed1: ");
        print_hex(seed[1], KYBER_SYMBYTES);
        print_string("\n");        
        masked_prf(ptr, /*sizeof(buf) / MASKING_N*/4, seed, (*nonce)++);
        print_string("buf[0]: ");
        print_u32_arr((uint32_t*) buf[0], 8);
        print_string("buf[1]: ");
        print_u32_arr((uint32_t*) buf[1], 8);
        print_string("flag: \n");
        print_u32_arr(((uint32_t*) buf)[0] ^ ((uint32_t*) buf)[1], 8);
        masked_cbd_eta1_i(r, buf, i);
    }    
}


void masked_polyvec_getnoise_eta2(masked_polyvec *r, const masked_sym seed, uint8_t *nonce) {
#if KYBER_ETA2 == KYBER_ETA1
    masked_polyvec_getnoise_eta1(r, seed, nonce);
#else
#if MASKING_N != 2
#error "This implementation requires MASKING_N = 2"
#endif
    uint8_t buf[MASKING_N][(KYBER_N*KYBER_ETA2*2)/8];
    masked_ptr ptr = {buf[0], buf[1]};
    unsigned int i;
    for (i = 0; i < KYBER_K; i++) {
        masked_prf(ptr, sizeof(buf) / MASKING_N, seed, (*nonce)++);
        masked_cbd_eta2_i(r, buf, i);
    }
#endif    
}


void masked_polyvec_sub_exp(masked_polyvec *r, const masked_polyvec *a, const polyvec *b) {
    unsigned int i, j;
    for (j = 0; j < KYBER_K; j++) {
        for (i = 0; i < MASKING_N; i++) {
            if (i == 0) {
                poly_sub_exp(&(r->share[i].vec[j]), &(a->share[i].vec[j]), &(b->vec[j]));
            }
            else {
                poly_exp(&(r->share[i].vec[j]), &(a->share[i].vec[j]));
            }
        }
    }
}


void masked_polyvec_sub_one(masked_polyvec *r, const masked_polyvec *a) {
    unsigned int i, j;
    uint32_t one[KYBER_N/2];
    uint32_t *src;
    for (i = 0; i < KYBER_N/2; i++) {
        one[i] = 0x10001;
    }
    for (j = 0; j < KYBER_K; j++) {
        for (i = 0; i < MASKING_N; i++) {
            if (i == 0 && j == 0) {
                src = one;
            }
            else {
                src = NTT_LITE_INPUT_DIS;
            }
            ntt_lite_sub((uint32_t*) &(r->share[i].vec[j].coeffs), (uint32_t*) &(r->share[i].vec[j].coeffs), src);
        }
    }
}


void masked_polyvec_acc(masked_poly *r, const masked_polyvec *a, const masked_poly *b) {
    unsigned int i, j;
    uint32_t *src;
    uint32_t *dst;
    for (j = 0; j < MASKING_N; j++) {
        for (i = 0; i < KYBER_K; i++) {
            if (i == 0) {
                src = (uint32_t*) &(b->share[j].coeffs);
            }
            else {
                src = NTT_LITE_INPUT_DIS;
            }
            if (i == KYBER_K - 1) {
                dst = (uint32_t*) &(r->share[j].coeffs);
            }
            else {
                dst = NTT_LITE_OUTPUT_DIS;
            }
            ntt_lite_add(dst, src, (uint32_t*) &(a->share[j].vec[i].coeffs));
        }
    }
}
