#ifndef MASKED_GADGETS_H
#define MASKED_GADGETS_H

#include <stdint.h>
#include "masked_poly.h"

#define masked_gadgets_init_q PASTA_NAMESPACE(masked_gadgets_init_q)
void masked_gadgets_init_q();

#define masked_gadgets_mask_poly PASTA_NAMESPACE(masked_gadgets_mask_poly)
void masked_gadgets_mask_poly(masked_poly *r, const poly *a);

#endif