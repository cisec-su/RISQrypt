#include <stdint.h>
#include <stddef.h>
#include "params.h"
#include "poly.h"
#include "ntt_lite.h"
#include "util.h"
#include <stdint.h>
#include "params.h"
#include "masked_sign.h"
#include "masked_packing.h"
#include "masked_polyvec.h"
#include "masked_poly.h"
#include "masked_gadgets.h"
#include "masked_symmetric.h"


/**
 * @brief Masked two-share multiplication for PASTA
 *        C = A * B with two-share masking using refreshing
 *        A = a0 + a1, B = b0 + b1
 *        C = A*B = (a0*b0) + (a0*b1) + (a1*b0) + (a1*b1)
 *        Using random refreshing:
 *        c0 = a0*b0 + r
 *        c1 = (a0*b1) + (a1*b0) + (a1*b1) - r
 *
 * @param C Output masked polynomial (two shares)
 * @param A Input masked polynomial A (two shares)
 * @param B Input masked polynomial B (two shares)
 */
void masked_poly_mult2(masked_poly *C, masked_poly *A, masked_poly *B) {
    masked_poly temp1, temp2, temp3, temp4;
    poly r;
    unsigned int i;

    // Compute a0*b0 and store in temp1
    masked_poly_pointwise(&temp1, &A->share[0], B);

    // Compute a0*b1 and store in temp2
    masked_poly_pointwise(&temp2, &A->share[0], B);

    // Compute a1*b0 and store in temp3
    masked_poly_pointwise(&temp3, &A->share[1], B);

    // Compute a1*b1 and store in temp4
    masked_poly_pointwise(&temp4, &A->share[1], B);

    // Generate random polynomial r using shake128
    stream128_init(0, 0, 0);
    stream128_squeeze((uint8_t *)r.coeffs, sizeof(r.coeffs));

    // c0 = a0*b0 + r
    for (i = 0; i < N; i++) {
        C->share[0].coeffs[i] = temp1.share[0].coeffs[i] + r.coeffs[i];
    }

    // c1 = (a0*b1) + (a1*b0) + (a1*b1) - r
    for (i = 0; i < N; i++) {
        C->share[1].coeffs[i] = temp2.share[0].coeffs[i] + temp3.share[0].coeffs[i] + temp4.share[0].coeffs[i] - r.coeffs[i];
    }
}



/**
 * @brief Masked PASTA S-box Cube layer: B[i] = A[i]^3 mod Q
 *        Uses HW accelerator (ntt_lite_pwm) with two-share masking
 *
 * @param B Output masked polynomial (two shares)
 * @param A Input masked polynomial (two shares)
 */
void masked_sbox_cube(masked_poly *B, masked_poly *A) {

    /*
    
    A   = a0 + a1 
    A^2 = (a0 + a1) * (a0 + a1)  

    
    */


    masked_poly A_squared;

    // First compute A^2 using pointwise multiplication (A * A)
    masked_poly_pointwise(&A_squared, &A->share[0], A);

    // Then compute A^3 = A * A^2 using pointwise multiplication
    masked_poly_pointwise(B, &A_squared.share[0], &A_squared);
}
