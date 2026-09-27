#include <stdint.h>
#include "ntt_lite.h"
#include "masked_cbd.h"
#include "x2x.h"
#include "poly.h"

#if MASKING_N != 2
#error "This implementation requires MASKING_N = 2"
#endif


/**
 * @brief Masked centered binomial sampling with eta = 2
 * @description Masked counterpart of cbd2(). The x2x 1-bit B2A gadget converts every
 *              Boolean-shared input bit into arithmetic shares mod Q. With stride 4, bit j of
 *              every coefficient lands in t[.][j], so r = (t0 + t1) - (t2 + t3) is computed
 *              share-wise on the accelerator (addition is linear, shares stay separate)
 * @param r pointer to output masked polynomial (arithmetic shares mod Q)
 * @param buf buf[i] points to Boolean share i of the 4*N random input bits (MASKED_CBD2_WORDS words)
 * @return void
 */
void masked_cbd2(masked_poly *r, uint32_t *const buf[MASKING_N]) {
    size_t i;
    poly t[MASKING_N][4];

    x2x_ref_b2a_1bit((uint32_t*) t[1][0].coeffs, (uint32_t*) t[0][0].coeffs, buf[1], buf[0], 2, N*4);

    for (i = 0; i < MASKING_N; i++) {
        ntt_lite_add(NTT_LITE_OUTPUT_DIS, (uint32_t*) t[i][1].coeffs, (uint32_t*) t[i][0].coeffs);
        ntt_lite_sub(NTT_LITE_OUTPUT_DIS, NTT_LITE_INPUT_DIS, (uint32_t*) t[i][2].coeffs);
        ntt_lite_set_clr_with_twiddle();
        ntt_lite_sub((uint32_t*) r->share[i].coeffs, NTT_LITE_INPUT_DIS, (uint32_t*) t[i][3].coeffs);
    }
}
