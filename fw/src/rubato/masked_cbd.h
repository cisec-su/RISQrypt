#ifndef MASKED_CBD_H
#define MASKED_CBD_H

#include <stdint.h>
#include "params.h"
#include "masked_poly.h"

/* Boolean-masked random input per share for one cbd2 sample, in 32-bit words (4 bits per coefficient) */
#define MASKED_CBD2_WORDS ((N*2*2) >> 5)

#define masked_cbd2 RUBATO_NAMESPACE(masked_cbd2)
void masked_cbd2(masked_poly *r, uint32_t *const buf[MASKING_N]);

#endif
