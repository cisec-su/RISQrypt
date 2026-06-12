#include <stdint.h>
#include <string.h>

#include "address.h"
#include "masked_fips202.h"
#include "fips202.h"
#include "masked_utils.h"
#include "params.h"
#include "masked_hash.h"
#include "randombytes.h"

void initialize_hash_function_hwmasked(spx_ctx* ctx)
{
    (void)ctx;
}


void gen_message_random_hwmasked(unsigned char *R,
                                 const unsigned char *sk_prf,
                                 const unsigned char *optrand,
                                 const unsigned char *m, unsigned long long mlen,
                                 const spx_ctx *ctx)
{
    (void)ctx;

    // Split SK_PRF into two shares
    unsigned char sk_prf_share1[SPX_N];
    unsigned char sk_prf_share2[SPX_N];
    unsigned char rand_mask[SPX_N];

    randombytes(rand_mask, SPX_N);
    for (int i = 0; i < SPX_N; i++) {
        sk_prf_share1[i] = sk_prf[i] ^ rand_mask[i];
        sk_prf_share2[i] = rand_mask[i];
    }

    // Build input buffers: [SK_PRF | OptRand | Message]
    size_t total_len = 2 * SPX_N + mlen;

    SPX_VLA(uint8_t, buf1, total_len);
    SPX_VLA(uint8_t, buf2, total_len);

    // SK_PRF shares
    memcpy(buf1, sk_prf_share1, SPX_N);
    memcpy(buf2, sk_prf_share2, SPX_N);

    // OptRand shares
    unsigned char opt_mask[SPX_N];
    randombytes(opt_mask, SPX_N);
    for (int i = 0; i < SPX_N; i++) {
        buf1[SPX_N + i] = optrand[i] ^ opt_mask[i];
        buf2[SPX_N + i] = opt_mask[i];
    }

    // Message (public, so share2 = 0)
    for (unsigned long long i = 0; i < mlen; i++) {
        buf1[2 * SPX_N + i] = m[i];
        buf2[2 * SPX_N + i] = 0;
    }

    // HW masked SHAKE256
    unsigned char R1[SPX_N];
    unsigned char R2[SPX_N];

    hwmasked_shake256(R1, R2, SPX_N, buf1, buf2, total_len);

    // Recombine R
    for (int i = 0; i < SPX_N; i++) R[i] = R1[i] ^ R2[i];
}

/**
 * Computes the message hash using R, the public key, and the message.
 * This uses the unmasked incremental API since all inputs are public at this point.
 */
void hash_message_hwmasked(unsigned char *digest, uint64_t *tree, uint32_t *leaf_idx,
                           const unsigned char *R, const unsigned char *pk,
                           const unsigned char *m, unsigned long long mlen,
                           const spx_ctx *ctx)
{
    (void)ctx;
#define SPX_TREE_BITS (SPX_TREE_HEIGHT * (SPX_D - 1))
#define SPX_TREE_BYTES ((SPX_TREE_BITS + 7) / 8)
#define SPX_LEAF_BITS SPX_TREE_HEIGHT
#define SPX_LEAF_BYTES ((SPX_LEAF_BITS + 7) / 8)
#define SPX_DGST_BYTES (SPX_FORS_MSG_BYTES + SPX_TREE_BYTES + SPX_LEAF_BYTES)

    unsigned char buf[SPX_DGST_BYTES];
    unsigned char *bufp = buf;
    uint64_t s_inc[26];

    // All inputs to hash_message are public, so use unmasked HW path
    shake256_inc_init((uint32_t *)s_inc);
    shake256_inc_absorb((uint32_t *)s_inc, R, SPX_N);
    shake256_inc_absorb((uint32_t *)s_inc, pk, SPX_PK_BYTES);
    shake256_inc_absorb((uint32_t *)s_inc, m, mlen);
    shake256_inc_finalize((uint32_t *)s_inc);
    shake256_inc_squeeze(buf, SPX_DGST_BYTES, (uint32_t *)s_inc);

    memcpy(digest, bufp, SPX_FORS_MSG_BYTES);
    bufp += SPX_FORS_MSG_BYTES;

#if SPX_TREE_BITS > 64
    #error For given height and depth, 64 bits cannot represent all subtrees
#endif

    if (SPX_D == 1) {
        *tree = 0;
    } else {
        *tree = bytes_to_ull(bufp, SPX_TREE_BYTES);
        *tree &= (~(uint64_t)0) >> (64 - SPX_TREE_BITS);
    }
    bufp += SPX_TREE_BYTES;

    *leaf_idx = (uint32_t)bytes_to_ull(bufp, SPX_LEAF_BYTES);
    *leaf_idx &= (~(uint32_t)0) >> (32 - SPX_LEAF_BITS);
}

/**
 * Masked version of prf_addr using HW Keccak masking.
 */
void hwmasked_prf_addr(unsigned char *out1, unsigned char *out2,
                       const spx_ctx *ctx,
                       const uint32_t addr[8])
{
    unsigned char buf1[2 * SPX_N + SPX_ADDR_BYTES];
    unsigned char buf2[2 * SPX_N + SPX_ADDR_BYTES];

    // Public seed + address in share1, zeros in share2
    memcpy(buf1, ctx->pub_seed, SPX_N);
    memcpy(buf1 + SPX_N, addr, SPX_ADDR_BYTES);
    memcpy(buf1 + SPX_N + SPX_ADDR_BYTES, ctx->sk_seed1, SPX_N);

    memset(buf2, 0, SPX_N + SPX_ADDR_BYTES);
    memcpy(buf2 + SPX_N + SPX_ADDR_BYTES, ctx->sk_seed2, SPX_N);

    hwmasked_shake256(out1, out2, SPX_N, buf1, buf2, 2 * SPX_N + SPX_ADDR_BYTES);
}
