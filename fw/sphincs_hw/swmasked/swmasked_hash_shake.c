#include <stdint.h>
#include <string.h>

#include "address.h"
#include "swmasked_fips202.h"
#include "fips202.h"
#include "swmasked_utils.h"
#include "params.h"
#include "swmasked_hash.h"
#include "randombytes.h"

// Non-masked functions from original file
void initialize_hash_function_masked(spx_ctx* ctx)
{
    (void)ctx; /* Suppress an 'unused parameter' warning. */
}

// Masked version of gen_message_random
void gen_message_random_masked(unsigned char *R,
                               const unsigned char *sk_prf,
                               const unsigned char *optrand,
                               const unsigned char *m, unsigned long long mlen,
                               const spx_ctx *ctx)
{
    // Prepare split key shares
    unsigned char sk_prf_share1[SPX_N]; // Reuse the splitting logic from sign.c
    unsigned char sk_prf_share2[SPX_N];
    unsigned char rand_mask[SPX_N];
    
    randombytes(rand_mask, SPX_N);
    for(int i=0; i<SPX_N; i++) {
        sk_prf_share1[i] = sk_prf[i] ^ rand_mask[i];
        sk_prf_share2[i] = rand_mask[i];
    }

    // Prepare message shares
    size_t total_len = 2*SPX_N + mlen; // Split so that hardware sees random data.
    
    SPX_VLA(uint8_t, buf1, total_len);
    SPX_VLA(uint8_t, buf2, total_len);
    
    // Copy SK_PRF shares
    memcpy(buf1, sk_prf_share1, SPX_N);
    memcpy(buf2, sk_prf_share2, SPX_N);
    
    // Copy OptRand
    unsigned char opt_mask[SPX_N];
    randombytes(opt_mask, SPX_N);
    for(int i=0; i<SPX_N; i++) {
        buf1[SPX_N + i] = optrand[i] ^ opt_mask[i];
        buf2[SPX_N + i] = opt_mask[i];
    }

    // Copy masked message. NOTE: to check if code correct use simpler approach where buf2 = 0
    for(unsigned long long i=0; i<mlen; i++) {
        buf1[2*SPX_N + i] = m[i];
        buf2[2*SPX_N + i] = 0; // No masking for message body bc it's public and saves time
    }

    // Perform masked hash
    unsigned char R1[SPX_N];
    unsigned char R2[SPX_N];
    
    swmasked_shake256(R1, R2, SPX_N, buf1, buf2, total_len);
    
    // Recombine R
    for(int i=0; i<SPX_N; i++) R[i] = R1[i] ^ R2[i];
}

/**
 * Computes the message hash using R, the public key, and the message.
 * Outputs the message digest and the index of the leaf. The index is split in
 * the tree index and the leaf index, for convenient copying to an address.
 */
void hash_message_masked(unsigned char *digest, uint64_t *tree, uint32_t *leaf_idx,
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

// Masked version of prf_addr
void swmasked_prf_addr(unsigned char *out1, unsigned char *out2, 
                    const spx_ctx *ctx,
                    const uint32_t addr[8]){
    unsigned char buf1[2*SPX_N + SPX_ADDR_BYTES];
    unsigned char buf2[2*SPX_N + SPX_ADDR_BYTES]; // Two buffers for two shares
    memcpy(buf1, ctx->pub_seed, SPX_N); // Copy public seed to first buffer
    memcpy(buf1 + SPX_N, addr, SPX_ADDR_BYTES); //Copy address
    memcpy(buf1 + SPX_N + SPX_ADDR_BYTES, ctx->sk_seed1, SPX_N); // Copy first share 
    memset(buf2, 0, SPX_N+SPX_ADDR_BYTES); // Public seed and adress equal to 0 for XOR
    memcpy(buf2 + SPX_N + SPX_ADDR_BYTES, ctx->sk_seed2, SPX_N); // Copy second share
    swmasked_shake256(out1,out2, SPX_N, buf1, buf2, 2*SPX_N + SPX_ADDR_BYTES); // Masked SHAKE256
}
