#ifndef MASKED_CBD_H
#define MASKED_CBD_H

#include <stdint.h>
#include "params.h"
#include "masked_poly.h"
#include "masked_polyvec.h"

#define masked_cbd_eta1_i KYBER_NAMESPACE(_masked_cbd_eta1_i)
void masked_cbd_eta1_i(masked_polyvec *r, const uint8_t buf[MASKING_N][(KYBER_N*KYBER_ETA1*2)/8], unsigned int i);

#define masked_cbd_eta2_i KYBER_NAMESPACE(_masked_cbd_eta2_i)
void masked_cbd_eta2_i(masked_polyvec *r, const uint8_t buf[MASKING_N][(KYBER_N*KYBER_ETA2*2)/8], unsigned int i);

#define masked_cbd_eta2 KYBER_NAMESPACE(_masked_cbd_eta2)
void masked_cbd_eta2(masked_poly *r, const uint8_t buf[MASKING_N][(KYBER_N*KYBER_ETA2*2)/8]);

#endif
