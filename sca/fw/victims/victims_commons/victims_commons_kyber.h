#ifndef VICTIMS_COMMONS_KYBER_H
#define VICTIMS_COMMONS_KYBER_H

#include <stdint.h>
#include "params.h"
#include "masked_poly.h"
#include "masked_polyvec.h"

#define POLY_SAMPLE_BYTES (KYBER_N * sizeof(uint32_t))


void vck_masked_poly_from_seed(masked_poly *dst, const uint8_t src[KYBER_SYMBYTES]);

void vck_masked_polyvec_from_seed(masked_polyvec *dst, const uint8_t src[KYBER_SYMBYTES]);

void vck_print_poly_shares(const masked_poly *mp, const char *label);

void vck_print_polyvec_shares(const masked_polyvec *mpv, const char *label);

void vck_print_msg_shares(const masked_msg mm, const char *label);

void vck_masked_msg_from_seed(masked_msg dst, const uint8_t src[KYBER_SYMBYTES]);


#endif // VICTIMS_COMMONS_KYBER_H