#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "masked_fips202.h"
#include "fips202.h"
#include "keccak.h"

// --------------------------------------------------------------------------
// Absorb (both shares)
// --------------------------------------------------------------------------
static void masked_fips202_absorb(const uint8_t *input1, const uint8_t *input2, size_t inlen)
{
    // Assume input length is multiple of 4 and word-aligned
    size_t words = inlen >> 2;
    if (words > 0) {
        keccak_absorb((const uint32_t*)input1, (const uint32_t*)input2, words);
    }
}

// --------------------------------------------------------------------------
// Finalize
// --------------------------------------------------------------------------
static void masked_fips202_finalize(void) {
    volatile uint32_t t = SHAKE_PAD;
    keccak_finish((uint32_t*)&t);
}

// --------------------------------------------------------------------------
// Squeeze (both shares)
// --------------------------------------------------------------------------
static void masked_fips202_squeeze(uint8_t *output1, uint8_t *output2, size_t outlen)
{
    size_t words = outlen >> 2;
    if (words > 0) {
        keccak_squeeze((uint32_t*)output1, (uint32_t*)output2, words);
    }
}

// ==========================================================================
// Public Incremental API
// ==========================================================================

void masked_shake256_inc_init(uint32_t *s_inc) {
    (void)s_inc;
    keccak_init(SHAKE256_RATE >> 3, KECCAK_MASK_EN);
}

void masked_shake256_inc_absorb(uint32_t *s_inc,
                                   const uint8_t *input1, const uint8_t *input2,
                                   size_t inlen)
{
    (void)s_inc;
    masked_fips202_absorb(input1, input2, inlen);
}

void masked_shake256_inc_finalize(uint32_t *s_inc) {
    (void)s_inc;
    masked_fips202_finalize();
}

void masked_shake256_inc_squeeze(uint8_t *output1, uint8_t *output2,
                                    size_t outlen, uint32_t *s_inc)
{
    (void)s_inc;
    masked_fips202_squeeze(output1, output2, outlen);
}

// ==========================================================================
// One-shot HW masked SHAKE256
// ==========================================================================

void masked_shake256(unsigned char *out1, unsigned char *out2, size_t outlen,
                       const unsigned char *in1, const unsigned char *in2, size_t inlen)
{
    keccak_init(SHAKE256_RATE >> 3, KECCAK_MASK_EN);

    // Bulk absorb aligned words
    size_t word_len = inlen >> 2;
    if (word_len > 0) {
        keccak_absorb((const uint32_t*)in1, (const uint32_t*)in2, word_len);
    }

    volatile uint32_t t = SHAKE_PAD;
    keccak_finish((uint32_t*)&t);

    // Squeeze
    size_t out_words = outlen >> 2;
    if (out_words > 0) {
        keccak_squeeze((uint32_t*)out1, (uint32_t*)out2, out_words);
    }
}
