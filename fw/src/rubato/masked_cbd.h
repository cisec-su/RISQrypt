#ifndef MASKED_CBD_H
#define MASKED_CBD_H

#include <stdint.h>
#include "params.h"
#include "masked_poly.h"

/* Boolean-masked random input per share for one cbd2 sample, in 32-bit words (4 bits per coefficient) */
#define MASKED_CBD2_WORDS ((N*2*2) >> 5)

#define masked_cbd2 RUBATO_NAMESPACE(masked_cbd2)
void masked_cbd2(masked_poly *r, uint32_t *const buf[MASKING_N]);

#define masked_cbd2_n RUBATO_NAMESPACE(masked_cbd2_n)
void masked_cbd2_n(uint32_t *dst[MASKING_N], uint32_t *const buf[MASKING_N],
				  uint32_t n);

#define masked_cbd2_sum_stage_n RUBATO_NAMESPACE(masked_cbd2_sum_stage_n)
void masked_cbd2_sum_stage_n(uint32_t *dst[MASKING_N],
							 uint32_t *const buf[MASKING_N],
							 uint32_t n, uint32_t logn, uint32_t stage);

#endif
