#include <stddef.h>
#include <stdint.h>
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

static hw_keccak_ctx* get_ctx(uint32_t *s_inc) {
    return (hw_keccak_ctx*)s_inc;
}

static void hw_absorb(hw_keccak_ctx *ctx, const uint8_t *input, size_t inlen) {
    // flush any buffered bytes by continuing to fill the buffer
    while (inlen > 0 && ctx->buf_len > 0) {
        ctx->buf[ctx->buf_len++] = *input++;
        inlen--;
        if (ctx->buf_len == 4) {
            keccak_absorb((uint32_t*)ctx->buf, NULL, 1);
            ctx->buf_len = 0;
        }
    }

    // transfer aligned words in bulk
    if ((((uintptr_t)input) & 0x3) == 0 && inlen >= 4) {
        size_t words = inlen >> 2;
        keccak_absorb((const uint32_t*)input, NULL, words);
        input += (words << 2);
        inlen -= (words << 2);
    }

    // Buffer remaining bytes
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
    for (uint32_t _i = 0; _i < ctx->buf_len; _i++)
        ((uint8_t *)&t)[_i] = ctx->buf[_i];
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

    // Fast path: output aligned and need multiple words
    if ((((uintptr_t)output) & 0x3) == 0 && outlen >= 4) {
        size_t words = outlen >> 2;
        keccak_squeeze((uint32_t*)output, NULL, words);
        output += (words << 2);
        outlen -= (words << 2);
    } else {
        // Unaligned output: squeeze word by word into temp buffer
        while (outlen >= 4) {
            uint32_t t;
            keccak_squeeze(&t, NULL, 1);
            for (unsigned int _i = 0; _i < 4; _i++) output[_i] = ((uint8_t *)&t)[_i];
            output += 4;
            outlen -= 4;
        }
    }

    // Handle remaining bytes (< 4)
    if (outlen > 0) {
        uint32_t t;
        keccak_squeeze(&t, NULL, 1);
        for (unsigned int _i = 0; _i < 4; _i++) ctx->squeeze_buf[_i] = ((uint8_t *)&t)[_i];
        for (unsigned int _i = 0; _i < outlen; _i++) output[_i] = ctx->squeeze_buf[_i];
        ctx->squeeze_ptr = outlen;
        ctx->squeeze_rem = 4 - outlen;
    }
}

// SHAKE128
void shake128_inc_init(uint32_t *s_inc) {
    hw_keccak_ctx *ctx = get_ctx(s_inc);
    ctx->buf_len = 0;
    ctx->rate_words = SHAKE128_RATE >> 3;
    ctx->pad = SHAKE_PAD;
    ctx->finalized = 0;
    ctx->squeeze_rem = 0;
    keccak_init(ctx->rate_words, KECCAK_MASK_DIS);
}

void shake128_inc_absorb(uint32_t *s_inc, const uint8_t *input, size_t inlen) {
    hw_absorb(get_ctx(s_inc), input, inlen);
}

void shake128_inc_finalize(uint32_t *s_inc) {
    hw_finalize(get_ctx(s_inc));
}

void shake128_inc_squeeze(uint8_t *output, size_t outlen, uint32_t *s_inc) {
    hw_squeeze(get_ctx(s_inc), output, outlen);
}

void shake128_absorb(uint32_t *s, const uint8_t *input, size_t inlen) {
    shake128_inc_init(s);
    shake128_inc_absorb(s, input, inlen);
}

void shake128_squeezeblocks(uint8_t *output, size_t nblocks, uint32_t *s) {
    hw_finalize(get_ctx(s));
    hw_squeeze(get_ctx(s), output, nblocks * SHAKE128_RATE);
}

void shake128(uint8_t *output, size_t outlen, const uint8_t *input, size_t inlen) {
    volatile uint32_t t;
    keccak_init(SHAKE128_RATE >> 3, KECCAK_MASK_DIS);
    keccak_absorb((const uint32_t*)input, NULL, inlen >> 2);
    t = SHAKE_PAD;
    keccak_finish((uint32_t*)&t);
    keccak_squeeze((uint32_t*)output, NULL, outlen >> 2);
    return;
}

// SHAKE256
void shake256_inc_init(uint32_t *s_inc) {
    hw_keccak_ctx *ctx = get_ctx(s_inc);
    ctx->buf_len = 0;
    ctx->rate_words = SHAKE256_RATE >> 3;
    ctx->pad = SHAKE_PAD;
    ctx->finalized = 0;
    ctx->squeeze_rem = 0;
    keccak_init(ctx->rate_words, KECCAK_MASK_DIS);
}

void shake256_inc_absorb(uint32_t *s_inc, const uint8_t *input, size_t inlen) {
    hw_absorb(get_ctx(s_inc), input, inlen);
}

void shake256_inc_finalize(uint32_t *s_inc) {
    hw_finalize(get_ctx(s_inc));
}

void shake256_inc_squeeze(uint8_t *output, size_t outlen, uint32_t *s_inc) {
    hw_squeeze(get_ctx(s_inc), output, outlen);
}

void shake256_absorb(uint32_t *s, const uint8_t *input, size_t inlen) {
    shake256_inc_init(s);
    shake256_inc_absorb(s, input, inlen);
}

void shake256_squeezeblocks(uint8_t *output, size_t nblocks, uint32_t *s) {
    hw_finalize(get_ctx(s));
    hw_squeeze(get_ctx(s), output, nblocks * SHAKE256_RATE);
}

void shake256(uint8_t *output, size_t outlen, const uint8_t *input, size_t inlen) {
    volatile uint32_t t;
    keccak_init(SHAKE256_RATE >> 3, KECCAK_MASK_DIS);
    keccak_absorb((const uint32_t*)input, NULL, inlen >> 2);
    t = SHAKE_PAD;
    keccak_finish((uint32_t*)&t);
    keccak_squeeze((uint32_t*)output, NULL, outlen >> 2);
    return;
} 