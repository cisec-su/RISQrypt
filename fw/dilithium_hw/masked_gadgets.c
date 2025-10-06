#include "masked_gadgets.h"
#include "params.h"
#include "x2x.h"
#include "util.h"


void masked_gadgets_init_q() {
    x2x_set_modulus(Q, 23, X2X_MODULUS_PRIME, X2X_DUAL_MODE_DIS, X2X_REJ_SAMPLE_EN);
}

void masked_gadgets_mask_poly(masked_poly *r, const poly *a) {
    x2x_a_share((uint32_t*) r->share[1].coeffs, (uint32_t*) r->share[0].coeffs, (uint32_t*) a->coeffs, N);
}

void masked_gadgets_B2A_q(masked_poly *r, const masked_poly *a) {
    x2x_b2a((uint32_t*) r->share[1].coeffs, (uint32_t*) r->share[0].coeffs, (uint32_t*) a->share[1].coeffs, (uint32_t*) a->share[0].coeffs, N);
}


void masked_gadgets_A2B_q_ptr(masked_poly *r, const poly *a[MASKING_N]) {
    x2x_a2b((uint32_t*) r->share[1].coeffs, (uint32_t*) r->share[0].coeffs, (uint32_t*) a[1]->coeffs, (uint32_t*) a[0]->coeffs, N);
}


void masked_gadgets_A2B_q(masked_poly *r, const masked_poly *a) {
    x2x_a2b((uint32_t*) r->share[1].coeffs, (uint32_t*) r->share[0].coeffs, (uint32_t*) a->share[1].coeffs, (uint32_t*) a->share[0].coeffs, N);
}


void masked_gadgets_init_2k(uint32_t p) {
    x2x_set_modulus(p, 0, X2X_MODULUS_POW2, X2X_DUAL_MODE_DIS, 0);
}


void masked_gadgets_B2A_2k(masked_poly *r, const masked_poly *a) {
    x2x_b2a((uint32_t*) r->share[1].coeffs, (uint32_t*) r->share[0].coeffs, (uint32_t*) a->share[1].coeffs, (uint32_t*) a->share[0].coeffs, N);
}


void masked_gadgets_A2B_2k(masked_poly *r, const masked_poly *a) {
    x2x_a2b((uint32_t*) r->share[1].coeffs, (uint32_t*) r->share[0].coeffs, (uint32_t*) a->share[1].coeffs, (uint32_t*) a->share[0].coeffs, N);
}