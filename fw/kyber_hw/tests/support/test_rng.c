#include "test_rng.h"
#include <string.h>
#include <stdint.h>

#if defined(__has_include)
#  if __has_include("params.h")
#    include "params.h"
#  endif
#endif

static uint64_t g_s[2] = {0x0123456789ABCDEFULL, 0xF0E1D2C3B4A59687ULL};

static inline uint64_t rotl64(uint64_t x, int k) { return (x << k) | (x >> (64 - k)); }

static uint64_t xr_next(void) {
    uint64_t s0 = g_s[0], s1 = g_s[1];
    uint64_t r  = s0 + s1;
    s1 ^= s0;
    g_s[0] = rotl64(s0, 55) ^ s1 ^ (s1 << 14);
    g_s[1] = rotl64(s1, 36);
    return r;
}

static uint64_t smix(uint64_t x) {
    x += 0x9E3779B97F4A7C15ULL;
    x = (x ^ (x >> 30)) * 0xBF58476D1CE4E5B9ULL;
    x = (x ^ (x >> 27)) * 0x94D049BB133111EBULL;
    return x ^ (x >> 31);
}

static void seed_from_u128(uint64_t a, uint64_t b) {
    if (a == 0 && b == 0) { b = 1; }
    g_s[0] = a; g_s[1] = b;
    for (int i = 0; i < 16; i++) (void)xr_next();
}

void test_rng_seed_bytes(const uint8_t *seed, size_t len) {
    uint64_t acc = 0x6A09E667F3BCC908ULL ^ (uint64_t)len;
    const uint8_t *p = seed;
    while (len) {
        uint64_t w = 0;
        for (int i = 0; i < 8 && len; i++, len--) w |= ((uint64_t)(*p++)) << (8 * i);
        acc = smix(acc ^ w);
    }
    uint64_t a = smix(acc ^ 0xA5A5A5A5A5A5A5A5ULL);
    uint64_t b = smix(acc ^ 0x5A5A5A5A5A5A5A5AULL);
    seed_from_u128(a, b);
}

static int hexval(int c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return 10 + (c - 'a');
    if (c >= 'A' && c <= 'F') return 10 + (c - 'A');
    return -1;
}

void test_rng_seed_hex(const char *hex) {
    uint8_t buf[256]; size_t n = 0; int hi = -1;
    for (const char *p = hex; *p; p++) {
        int v = hexval(*p);
        if (v < 0) continue;
        if (hi < 0) hi = v;
        else {
            buf[n++] = (uint8_t)((hi << 4) | v);
            hi = -1;
            if (n == sizeof(buf)) break;
        }
    }
    if (n == 0) { const uint8_t dflt[16] = {0}; test_rng_seed_bytes(dflt, sizeof dflt); }
    else        { test_rng_seed_bytes(buf, n); }
}

void test_rng_seed_case(uint32_t suite_id, uint32_t round_id) {
    uint64_t a = 0xC001D00DFEEDFACEULL ^ ((uint64_t)suite_id << 32) ^ (uint64_t)round_id;
#ifdef KYBER_K
    a ^= ((uint64_t)KYBER_K) * 0x9E3779B97F4A7C15ULL;
#endif
    uint64_t b = 0xDEADBEEFCAFEBABEULL ^ ((uint64_t)round_id << 32) ^ (uint64_t)suite_id;
    seed_from_u128(smix(a), smix(b));
}

void randombytes(uint8_t *out, size_t outlen) {
    while (outlen >= 8) { uint64_t w = xr_next(); memcpy(out, &w, 8); out += 8; outlen -= 8; }
    if (outlen) { uint64_t w = xr_next(); memcpy(out, &w, outlen); }
}
