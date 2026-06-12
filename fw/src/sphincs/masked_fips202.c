#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "hwmasked_fips202.h"
#include "fips202.h"
#include "keccak.h"


typedef struct {
    uint8_t buf1[4];
    uint8_t buf2[4];
    uint32_t buf_len;
    uint32_t rate_words;
    uint32_t pad;
    uint8_t finalized;

    uint8_t squeeze_buf1[4];
    uint8_t squeeze_buf2[4];
    uint32_t squeeze_ptr;
    uint32_t squeeze_rem;
} hwmasked_keccak_ctx;

static hwmasked_keccak_ctx* hwm_get_ctx(uint32_t *s_inc) {
    return (hwmasked_keccak_ctx*)s_inc;
}

// --------------------------------------------------------------------------
// Absorb (both shares)
// --------------------------------------------------------------------------
static void hwm_absorb(hwmasked_keccak_ctx *ctx,
                        const uint8_t *input1, const uint8_t *input2,
                        size_t inlen)
{
    // Flush any buffered bytes by filling up to a word
    while (inlen > 0 && ctx->buf_len > 0) {
        ctx->buf1[ctx->buf_len] = *input1++;
        ctx->buf2[ctx->buf_len] = *input2++;
        ctx->buf_len++;
        inlen--;
        if (ctx->buf_len == 4) {
            keccak_absorb((uint32_t*)ctx->buf1, (uint32_t*)ctx->buf2, 1);
            ctx->buf_len = 0;
        }
    }

    // Bulk absorb aligned words
    if ((((uintptr_t)input1) & 0x3) == 0 &&
        (((uintptr_t)input2) & 0x3) == 0 &&
        inlen >= 4)
    {
        size_t words = inlen >> 2;
        keccak_absorb((const uint32_t*)input1, (const uint32_t*)input2, words);
        input1 += (words << 2);
        input2 += (words << 2);
        inlen  -= (words << 2);
    }

    // Buffer remaining bytes
    while (inlen > 0) {
        ctx->buf1[ctx->buf_len] = *input1++;
        ctx->buf2[ctx->buf_len] = *input2++;
        ctx->buf_len++;
        inlen--;
        if (ctx->buf_len == 4) {
            keccak_absorb((uint32_t*)ctx->buf1, (uint32_t*)ctx->buf2, 1);
            ctx->buf_len = 0;
        }
    }
}

// --------------------------------------------------------------------------
// Finalize
// --------------------------------------------------------------------------
static void hwm_finalize(hwmasked_keccak_ctx *ctx) {
    if (ctx->finalized) return;

    volatile uint32_t t = 0;
    memcpy((void*)&t, ctx->buf1, ctx->buf_len);
    // XOR in the mask share's buffered bytes — they need to be absorbed too
    // But for keccak_finish, the pad word is absorbed as a public (single-share)
    // word. So we need to flush any remaining buffered masked data first.
    if (ctx->buf_len > 0) {
        // Pad the partial word buffers with zeros and absorb
        for (uint32_t i = ctx->buf_len; i < 4; i++) {
            ctx->buf1[i] = 0;
            ctx->buf2[i] = 0;
        }
        keccak_absorb((uint32_t*)ctx->buf1, (uint32_t*)ctx->buf2, 1);
        ctx->buf_len = 0;
        // Now the pad word is separate — only contains the SHAKE padding
        t = ctx->pad;
    } else {
        // No buffered data, pad byte goes at position 0
        uint8_t *t_bytes = (uint8_t*)&t;
        t_bytes[0] = (uint8_t)ctx->pad;
    }

    keccak_finish((uint32_t*)&t);
    ctx->finalized = 1;
    ctx->squeeze_rem = 0;
}

// --------------------------------------------------------------------------
// Squeeze (both shares)
// --------------------------------------------------------------------------
static void hwm_squeeze(hwmasked_keccak_ctx *ctx,
                         uint8_t *output1, uint8_t *output2,
                         size_t outlen)
{
    while (outlen > 0 && ctx->squeeze_rem > 0) {
        *output1++ = ctx->squeeze_buf1[ctx->squeeze_ptr];
        *output2++ = ctx->squeeze_buf2[ctx->squeeze_ptr];
        ctx->squeeze_ptr++;
        ctx->squeeze_rem--;
        outlen--;
    }

    if (outlen == 0) return;

    if ((((uintptr_t)output1) & 0x3) == 0 &&
        (((uintptr_t)output2) & 0x3) == 0 &&
        outlen >= 4)
    {
        size_t words = outlen >> 2;
        keccak_squeeze((uint32_t*)output1, (uint32_t*)output2, words);
        output1 += (words << 2);
        output2 += (words << 2);
        outlen  -= (words << 2);
    } else {
        while (outlen >= 4) {
            uint32_t t1, t2;
            keccak_squeeze(&t1, &t2, 1);
            memcpy(output1, &t1, 4);
            memcpy(output2, &t2, 4);
            output1 += 4;
            output2 += 4;
            outlen -= 4;
        }
    }

    if (outlen > 0) {
        uint32_t t1, t2;
        keccak_squeeze(&t1, &t2, 1);
        memcpy(ctx->squeeze_buf1, &t1, 4);
        memcpy(ctx->squeeze_buf2, &t2, 4);
        memcpy(output1, ctx->squeeze_buf1, outlen);
        memcpy(output2, ctx->squeeze_buf2, outlen);
        ctx->squeeze_ptr = outlen;
        ctx->squeeze_rem = 4 - outlen;
    }
}

// ==========================================================================
// Public Incremental API
// ==========================================================================

void hwmasked_shake256_inc_init(uint32_t *s_inc) {
    hwmasked_keccak_ctx *ctx = hwm_get_ctx(s_inc);
    ctx->buf_len = 0;
    ctx->rate_words = SHAKE256_RATE >> 3;
    ctx->pad = SHAKE_PAD;
    ctx->finalized = 0;
    ctx->squeeze_rem = 0;
    keccak_init(ctx->rate_words, KECCAK_MASK_EN);
}

void hwmasked_shake256_inc_absorb(uint32_t *s_inc,
                                   const uint8_t *input1, const uint8_t *input2,
                                   size_t inlen)
{
    hwm_absorb(hwm_get_ctx(s_inc), input1, input2, inlen);
}

void hwmasked_shake256_inc_finalize(uint32_t *s_inc) {
    hwm_finalize(hwm_get_ctx(s_inc));
}

void hwmasked_shake256_inc_squeeze(uint8_t *output1, uint8_t *output2,
                                    size_t outlen, uint32_t *s_inc)
{
    hwm_squeeze(hwm_get_ctx(s_inc), output1, output2, outlen);
}

// ==========================================================================
// One-shot HW masked SHAKE256
// ==========================================================================

void hwmasked_shake256(unsigned char *out1, unsigned char *out2, size_t outlen,
                       const unsigned char *in1, const unsigned char *in2, size_t inlen)
{
    keccak_init(SHAKE256_RATE >> 3, KECCAK_MASK_EN);

    // Bulk absorb aligned words
    size_t word_len = inlen >> 2;
    if (word_len > 0) {
        keccak_absorb((const uint32_t*)in1, (const uint32_t*)in2, word_len);
    }

    // Handle remaining bytes (< 4) combined with padding
    size_t rem = inlen & 0x3;
    volatile uint32_t t = 0;
    if (rem > 0) {
        uint8_t *t_bytes = (uint8_t*)&t;
        const uint8_t *tail1 = in1 + (word_len << 2);
        const uint8_t *tail2 = in2 + (word_len << 2);

        // XOR the two shares' tail bytes together into the pad word
        // (pad word is absorbed as public, so we need plaintext tail + pad byte)
        for (size_t i = 0; i < rem; i++) {
            t_bytes[i] = tail1[i] ^ tail2[i];
        }
        t_bytes[rem] = SHAKE_PAD;
    } else {
        t = SHAKE_PAD;
    }

    keccak_finish((uint32_t*)&t);

    // Squeeze
    size_t out_words = outlen >> 2;
    if (out_words > 0) {
        keccak_squeeze((uint32_t*)out1, (uint32_t*)out2, out_words);
    }

    // Handle remaining output bytes (< 4)
    size_t out_rem = outlen & 0x3;
    if (out_rem > 0) {
        uint32_t t1, t2;
        keccak_squeeze(&t1, &t2, 1);
        memcpy(out1 + (out_words << 2), &t1, out_rem);
        memcpy(out2 + (out_words << 2), &t2, out_rem);
    }
}
