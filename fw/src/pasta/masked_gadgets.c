#include "masked_gadgets.h"
#include "params.h"
#include "x2x.h"
#include "util.h"


/**
 * @brief Initialize masked gadget operations with modulus Q
 * @description Sets up the x2x hardware gadget with modulus Q, security parameter 17, and proper configuration for masked polynomial operations. Must be called before any masked gadget operations
 * @return void
 */
void masked_gadgets_init_q() {
    x2x_set_modulus(Q, 17, X2X_MODULUS_PRIME, X2X_DUAL_MODE_DIS, X2X_REJ_SAMPLE_DIS);
}

/**
 * @brief Mask polynomial using x2x gadget
 * @description Creates a two-share masking of polynomial a using the x2x_a_share gadget function. Generates statistically independent share[0] and share[1] such that share[0] + share[1] ≡ a (mod Q)
 * @param r pointer to output masked polynomial
 * @param a pointer to input unmasked polynomial
 * @return void
 */
void masked_gadgets_mask_poly(masked_poly *r, const poly *a) {
    x2x_a_share((uint32_t*) r->share[1].coeffs, (uint32_t*) r->share[0].coeffs, (uint32_t*) a->coeffs, N);
}


/**
 * @brief Apply affine transformation to masked polynomial
 * @description Applies the x2x_a_ref gadget for affine transformation on masked shares. Used as part of refreshing and higher-order masking operations
 * @param r pointer to output masked polynomial
 * @param a pointer to input masked polynomial
 * @return void
 */
void masked_gadgets_x2x_a_ref(masked_poly *r, const masked_poly *a) {
    x2x_a_ref(
        (uint32_t *)r->share[0].coeffs,
        (uint32_t *)r->share[1].coeffs,
        (uint32_t *)a->share[0].coeffs,
        (uint32_t *)a->share[1].coeffs,
        N);
}

/**
 * @brief Read random values from x2x PRNG
 * @description Reads random polynomial coefficients from the x2x PRNG gadget. Used for generating random masks and refreshing values in masked operations
 * @param r pointer to output polynomial to be filled with random values
 * @param len number of coefficients to generate
 * @return void
 */
void masked_gadgets_x2x_prng_read(poly *r, size_t len) {
    x2x_prng_read((uint32_t*)r, len);

}

