#ifndef MASKED_SYMMETRIC_H
#define MASKED_SYMMETRIC_H

#include "symmetric.h"
#include "masked.h"



void dilithium_masked_shake256_absorb_double(masked_flat_ptr dst, unsigned int dst_len, const masked_flat_ptr src_priv, unsigned int src_priv_len, const uint8_t *src_pub, unsigned int src_pub_len);

void dilithium_masked_shake256_absorb_nonce(masked_flat_ptr dst, unsigned int dst_len, const masked_flat_ptr src, unsigned int src_len, const uint16_t nonce);

#endif
