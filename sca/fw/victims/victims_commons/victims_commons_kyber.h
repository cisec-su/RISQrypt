#ifndef VICTIMS_COMMONS_KYBER_H
#define VICTIMS_COMMONS_KYBER_H

#include <stdint.h>
#include "params.h"
#include "masked_poly.h"

#define POLY_SAMPLE_BYTES (KYBER_N * sizeof(uint32_t))


void vck_masked_poly_from_seed(masked_poly *dst, const uint8_t src[KYBER_SYMBYTES], uint8_t nonce);

void vck_print_shares(const masked_poly *mp, const char *label);

#endif // VICTIMS_COMMONS_KYBER_H