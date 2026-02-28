#ifndef MASKED_GADGETS_H
#define MASKED_GADGETS_H

#include <stdint.h>
#include "masked_poly.h"

#define masked_gadgets_init_q PASTA_NAMESPACE(masked_gadgets_init_q)
void masked_gadgets_init_q();

#define masked_gadgets_mask_poly PASTA_NAMESPACE(masked_gadgets_mask_poly)
void masked_gadgets_mask_poly(masked_poly *r, const poly *a);

#define masked_gadgets_x2x_prng_read PASTA_NAMESPACE(masked_gadgets_x2x_prng_read)
void masked_gadgets_x2x_prng_read(poly *r, size_t len);

#define masked_gadgets_x2x_a_ref PASTA_NAMESPACE(masked_gadgets_x2x_a_ref)
void masked_gadgets_x2x_a_ref(masked_poly *r, const masked_poly *a);

#endif