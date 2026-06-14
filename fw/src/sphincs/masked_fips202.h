#ifndef MASKED_FIPS202_H
#define MASKED_FIPS202_H

#include <stdint.h>
#include <stddef.h>

#define MASKED_N_SHARES 2

/**
 * HW-accelerated masked SHAKE256.
 * Uses the Keccak hardware accelerator with KECCAK_MASK_EN.
 * Inputs and outputs are split into two shares (share1 ^ share2 = plaintext).
 */
void masked_shake256(unsigned char *out1, unsigned char *out2, size_t outlen,
                       const unsigned char *in1, const unsigned char *in2, size_t inlen);

/**
 * Incremental API for HW masked SHAKE256.
 * The context is stored in s_inc (same layout as hw_keccak_ctx).
 */
void masked_shake256_inc_init(uint32_t *s_inc);
void masked_shake256_inc_absorb(uint32_t *s_inc,
                                   const uint8_t *input1, const uint8_t *input2,
                                   size_t inlen);
void masked_shake256_inc_finalize(uint32_t *s_inc);
void masked_shake256_inc_squeeze(uint8_t *output1, uint8_t *output2,
                                    size_t outlen, uint32_t *s_inc);

#endif
