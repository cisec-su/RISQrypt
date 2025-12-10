#ifndef VICTIMS_COMMONS_KYBER_H
#define VICTIMS_COMMONS_KYBER_H

#include <stdint.h>
#include "params.h"
#include "masked_poly.h"
#include "masked_polyvec.h"


#define VCD_SEED_LEN 32

void vcd_masked_poly_from_seed(masked_poly *dst, const uint8_t src[VCD_SEED_LEN]);

void vcd_masked_poly_gamma2_from_seed(masked_poly *dst, const uint8_t src[VCD_SEED_LEN]);

void vcd_print_poly_shares(const masked_poly *mp, const char *label);

void vcd_print_poly_unmasked(const masked_poly *mp, const char *label);


#endif // VICTIMS_COMMONS_KYBER_H