#include <stddef.h>
#include <stdint.h>
#include "params.h"
#include "symmetric.h"
#include "keccak.h"
#include "util.h"

/**
 * @brief Initializes a SHAKE128 stream with seed and nonce.
 * @description Prepares the Keccak-based SHAKE128 state for squeezing pseudorandom bytes.
 * @param seed 64-bit seed value for stream initialization
 * @param nonce 64-bit nonce value for stream initialization
 */
void rubato_shake128_stream_init(uint64_t seed, uint64_t nonce, uint8_t poly_ctr) {
    volatile uint32_t t;
    keccak_init(SHAKE128_RATE >> 3, KECCAK_MASK_DIS);
    keccak_absorb((uint32_t*)&seed, NULL, sizeof(uint64_t) >> 2);
    keccak_absorb((uint32_t*)&nonce, NULL, sizeof(uint64_t) >> 2);
    t = (SHAKE_PAD << 8) | ((uint32_t)poly_ctr);
    keccak_finish((uint32_t*)&t);
}

/**
 * @brief Squeezes bytes from the SHAKE128 stream.
 * @description Extracts pseudorandom bytes from the SHAKE128 state.
 * @param dst Output buffer for squeezed bytes
 * @param dst_len Number of bytes to squeeze (must be multiple of 4)
 */
void rubato_shake128_squeeze(uint8_t *dst, unsigned int dst_len) {
    keccak_squeeze((uint32_t*) dst, NULL, dst_len >> 2);
}

/**
 * @brief Squeezes complete SHAKE128 blocks from the stream.
 * @description Extracts multiple SHAKE128 rate-sized blocks of pseudorandom data.
 * @param dst Output buffer for squeezed blocks
 * @param num_blocks Number of SHAKE128 blocks to squeeze
 */
void rubato_shake128_squeezeblocks(uint8_t *dst, unsigned int num_blocks) {
    keccak_squeeze((uint32_t*) dst, NULL, num_blocks*(SHAKE128_RATE >> 2));
}

/**
 * @brief Initializes a SHAKE256 stream with seed and nonce.
 */
void rubato_shake256_stream_init(uint64_t seed, uint64_t nonce, uint8_t poly_ctr) {
    volatile uint32_t t;
    keccak_init(SHAKE256_RATE >> 3, KECCAK_MASK_DIS);
    keccak_absorb((uint32_t*)&seed, NULL, sizeof(uint64_t) >> 2);
    keccak_absorb((uint32_t*)&nonce, NULL, sizeof(uint64_t) >> 2);
    t = (SHAKE_PAD << 8) | ((uint32_t)poly_ctr);
    keccak_finish((uint32_t*)&t);
}

/**
 * @brief Squeezes bytes from the SHAKE256 stream.
 */
void rubato_shake256_squeeze(uint8_t *dst, unsigned int dst_len) {
    keccak_squeeze((uint32_t*) dst, NULL, dst_len >> 2);
}

/**
 * @brief Squeezes complete SHAKE256 blocks from the stream.
 */
void rubato_shake256_squeezeblocks(uint8_t *dst, unsigned int num_blocks) {
    keccak_squeeze((uint32_t*) dst, NULL, num_blocks*(SHAKE256_RATE >> 2));
}

/**
 * @brief Initializes a masked SHAKE128 stream with public seed and nonce.
 * @description Same input as rubato_shake128_stream_init, but Keccak runs in masked mode so
 *              the squeezed output is returned as two Boolean shares (dst_0 ^ dst_1 equals
 *              the unmasked stream). The public inputs are absorbed with keccak_absorb_public.
 * @param seed 64-bit seed value for stream initialization
 * @param nonce 64-bit nonce value for stream initialization
 * @param poly_ctr domain separator
 */
void rubato_masked_shake128_stream_init(uint64_t seed, uint64_t nonce, uint8_t poly_ctr) {
    volatile uint32_t t;
    keccak_init(SHAKE128_RATE >> 3, KECCAK_MASK_EN);
    keccak_absorb_public((uint32_t*)&seed, sizeof(uint64_t) >> 2);
    keccak_absorb_public((uint32_t*)&nonce, sizeof(uint64_t) >> 2);
    t = (SHAKE_PAD << 8) | ((uint32_t)poly_ctr);
    keccak_finish((uint32_t*)&t);
}

/**
 * @brief Squeezes Boolean-shared bytes from the masked SHAKE128 stream.
 * @param dst_0 Output buffer for share 0
 * @param dst_1 Output buffer for share 1
 * @param dst_len Number of bytes per share (must be multiple of 4)
 */
void rubato_masked_shake128_squeeze(uint8_t *dst_0, uint8_t *dst_1, unsigned int dst_len) {
    keccak_squeeze((uint32_t*) dst_0, (uint32_t*) dst_1, dst_len >> 2);
}

/**
 * @brief Initializes a masked SHAKE256 stream with public seed and nonce.
 */
void rubato_masked_shake256_stream_init(uint64_t seed, uint64_t nonce, uint8_t poly_ctr) {
    volatile uint32_t t;
    keccak_init(SHAKE256_RATE >> 3, KECCAK_MASK_EN);
    keccak_absorb_public((uint32_t*)&seed, sizeof(uint64_t) >> 2);
    keccak_absorb_public((uint32_t*)&nonce, sizeof(uint64_t) >> 2);
    t = (SHAKE_PAD << 8) | ((uint32_t)poly_ctr);
    keccak_finish((uint32_t*)&t);
}

/**
 * @brief Squeezes Boolean-shared bytes from the masked SHAKE256 stream.
 */
void rubato_masked_shake256_squeeze(uint8_t *dst_0, uint8_t *dst_1, unsigned int dst_len) {
    keccak_squeeze((uint32_t*) dst_0, (uint32_t*) dst_1, dst_len >> 2);
}
