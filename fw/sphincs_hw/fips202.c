#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "fips202.h"
#include "keccak.h"

// Context structure mapped to s_inc (uint64_t[26])
typedef struct {
    uint8_t buf[8];
    uint32_t buf_len;
    uint32_t rate_words;
    uint32_t pad;
    uint8_t finalized;
    
    uint8_t squeeze_buf[4];
    uint32_t squeeze_ptr;
    uint32_t squeeze_rem;
} hw_keccak_ctx;

static hw_keccak_ctx* get_ctx(uint64_t *s_inc) {
    return (hw_keccak_ctx*)s_inc;
}

static void hw_absorb(hw_keccak_ctx *ctx, const uint8_t *input, size_t inlen) {
    while (inlen > 0) {
        ctx->buf[ctx->buf_len++] = *input++;
        inlen--;
        if (ctx->buf_len == 4) {
            keccak_absorb((uint32_t*)ctx->buf, NULL, 1);
            ctx->buf_len = 0;
        }
    }
}

static void hw_finalize(hw_keccak_ctx *ctx) {
    if (ctx->finalized) {
        return;
    }
    volatile uint32_t t = 0;
    memcpy((void*)&t, ctx->buf, ctx->buf_len);
    uint8_t *t_bytes = (uint8_t*)&t;
    t_bytes[ctx->buf_len] = (uint8_t)ctx->pad;
    keccak_finish((uint32_t*)&t);
    ctx->finalized = 1;
    ctx->squeeze_rem = 0;
}

static void hw_squeeze(hw_keccak_ctx *ctx, uint8_t *output, size_t outlen) {
    // Consume remaining bytes from squeeze_buf
    while (outlen > 0 && ctx->squeeze_rem > 0) {
        *output++ = ctx->squeeze_buf[ctx->squeeze_ptr++];
        ctx->squeeze_rem--;
        outlen--;
    }

    if (outlen == 0) return;
        // Output not aligned. Squeeze word by word into temp buffer.
        while (outlen >= 4) {
            uint32_t t;
            keccak_squeeze(&t, NULL, 1);
            memcpy(output, &t, 4);
            output += 4;
            outlen -= 4;
        }

    // Handle remaining bytes (< 4)
    if (outlen > 0) {
        uint32_t t;
        keccak_squeeze(&t, NULL, 1);
        // Store in squeeze_buf
        memcpy(ctx->squeeze_buf, &t, 4);
        
        size_t to_copy = outlen; // outlen is 1, 2, or 3
        memcpy(output, ctx->squeeze_buf, to_copy);
        
        ctx->squeeze_ptr = to_copy;
        ctx->squeeze_rem = 4 - to_copy;
    }
}

// SHAKE128
void shake128_inc_init(uint64_t *s_inc) {
    hw_keccak_ctx *ctx = get_ctx(s_inc);
    ctx->buf_len = 0;
    ctx->rate_words = SHAKE128_RATE >> 3;
    ctx->pad = 0x1F;
    ctx->finalized = 0;
    ctx->squeeze_rem = 0;
    keccak_init(ctx->rate_words, KECCAK_MASK_DIS);
}

void shake128_inc_absorb(uint64_t *s_inc, const uint8_t *input, size_t inlen) {
    hw_absorb(get_ctx(s_inc), input, inlen);
}

void shake128_inc_finalize(uint64_t *s_inc) {
    hw_finalize(get_ctx(s_inc));
}

void shake128_inc_squeeze(uint8_t *output, size_t outlen, uint64_t *s_inc) {
    hw_squeeze(get_ctx(s_inc), output, outlen);
}

void shake128_absorb(uint64_t *s, const uint8_t *input, size_t inlen) {
    shake128_inc_init(s);
    shake128_inc_absorb(s, input, inlen);
}

void shake128_squeezeblocks(uint8_t *output, size_t nblocks, uint64_t *s) {
    hw_finalize(get_ctx(s));
    hw_squeeze(get_ctx(s), output, nblocks * SHAKE128_RATE);
}

void shake128(uint8_t *output, size_t outlen, const uint8_t *input, size_t inlen) {
    uint64_t s[26];
    shake128_inc_init(s);
    shake128_inc_absorb(s, input, inlen);
    shake128_inc_finalize(s);
    shake128_inc_squeeze(output, outlen, s);
}

// SHAKE256
void shake256_inc_init(uint64_t *s_inc) {
    hw_keccak_ctx *ctx = get_ctx(s_inc);
    ctx->buf_len = 0;
    ctx->rate_words = SHAKE256_RATE >> 3;
    ctx->pad = 0x1F;
    ctx->finalized = 0;
    ctx->squeeze_rem = 0;
    keccak_init(ctx->rate_words, KECCAK_MASK_DIS);
}

void shake256_inc_absorb(uint64_t *s_inc, const uint8_t *input, size_t inlen) {
    hw_absorb(get_ctx(s_inc), input, inlen);
}

void shake256_inc_finalize(uint64_t *s_inc) {
    hw_finalize(get_ctx(s_inc));
}

void shake256_inc_squeeze(uint8_t *output, size_t outlen, uint64_t *s_inc) {
    hw_squeeze(get_ctx(s_inc), output, outlen);
}

void shake256_absorb(uint64_t *s, const uint8_t *input, size_t inlen) {
    shake256_inc_init(s);
    shake256_inc_absorb(s, input, inlen);
}

void shake256_squeezeblocks(uint8_t *output, size_t nblocks, uint64_t *s) {
    hw_finalize(get_ctx(s));
    hw_squeeze(get_ctx(s), output, nblocks * SHAKE256_RATE);
}

void shake256(uint8_t *output, size_t outlen, const uint8_t *input, size_t inlen) {
    uint64_t s[26];
    shake256_inc_init(s);
    shake256_inc_absorb(s, input, inlen);
    shake256_inc_finalize(s);
    shake256_inc_squeeze(output, outlen, s);
}