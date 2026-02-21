#include "masked_gadgets.h"
#include "params.h"
#include "x2x.h"
#include "util.h"


void masked_gadgets_init_q() {
    x2x_set_modulus(Q, 17, X2X_MODULUS_PRIME, X2X_DUAL_MODE_DIS, X2X_REJ_SAMPLE_DIS);
}

void masked_gadgets_mask_poly(masked_poly *r, const poly *a) {
    x2x_a_share((uint32_t*) r->share[1].coeffs, (uint32_t*) r->share[0].coeffs, (uint32_t*) a->coeffs, N);
}
