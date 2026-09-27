#include <stdint.h>
#include "ntt_lite.h"
#include "params.h"
#include "cbd.h"


/**
 * @brief Centered binomial sampling with eta = 2
 * @description Given an array of uniformly random bytes, computes a polynomial whose
 *              coefficients follow a centered binomial distribution with eta = 2, using
 *              the hardware accelerator. Coefficient k is (b0 + b1) - (b2 + b3), where
 *              b0..b3 are bits 4k..4k+3 of buf (little-endian bit order)
 * @param r pointer to output polynomial
 * @param buf pointer to input byte array of 2*N/4 bytes (must be 4-byte aligned)
 * @return void
 */
void cbd2(poly *r, const uint8_t buf[2*N/4])
{
  ntt_lite_cbd((uint32_t*) r->coeffs, (const uint32_t*) buf, 2);
}

/**
 * @brief Centered binomial sampling with eta = 3
 * @description Given an array of uniformly random bytes, computes a polynomial whose
 *              coefficients follow a centered binomial distribution with eta = 3, using
 *              the hardware accelerator. Coefficient k is (b0 + b1 + b2) - (b3 + b4 + b5),
 *              where b0..b5 are bits 6k..6k+5 of buf (little-endian bit order)
 * @param r pointer to output polynomial
 * @param buf pointer to input byte array of 3*N/4 bytes (must be 4-byte aligned)
 * @return void
 */
void cbd3(poly *r, const uint8_t buf[3*N/4])
{
  ntt_lite_cbd((uint32_t*) r->coeffs, (const uint32_t*) buf, 3);
}
