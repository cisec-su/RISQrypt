#ifndef MASKED_SYMMETRIC_H
#define MASKED_SYMMETRIC_H

#include "symmetric.h"
#include "masked.h"



void dilithium_masked_shake256_absorb_double(masked_flat_ptr dst, unsigned int dst_len, const masked_flat_ptr src_priv, unsigned int src_priv_len, const uint8_t *src_pub, unsigned int src_pub_len);

void dilithium_masked_shake256_stream_init(const masked_crh src, const uint16_t nonce);
#define masked_stream256_init(SEED, NONCE) dilithium_masked_shake256_stream_init(SEED, NONCE)

void dilithium_masked_shake_squeeze(masked_flat_ptr dst, unsigned int dst_len);
#define masked_stream256_squeeze(OUT, OUTBYTES) \
        dilithium_masked_shake_squeeze(OUT, OUTBYTES)


#endif
