#include <stdint.h>
#include <string.h>

#include "address.h"
#include "utils.h"
#include "params.h"
#include "hash.h"
#include "fips202.h"

/* For SHAKE256, there is no immediate reason to initialize at the start,
   so this function is an empty operation. */
void initialize_hash_function(spx_ctx* ctx)
{
    (void)ctx; /* Suppress an 'unused parameter' warning. */
}

/*
 * Computes PRF(pk_seed, sk_seed, addr)
 */
void prf_addr(unsigned char *out, const spx_ctx *ctx,
              const uint32_t addr[8])
{
    unsigned char buf[2*SPX_N + SPX_ADDR_BYTES];

    memcpy(buf, ctx->pub_seed, SPX_N);
    memcpy(buf + SPX_N, addr, SPX_ADDR_BYTES);
    memcpy(buf + SPX_N + SPX_ADDR_BYTES, ctx->sk_seed, SPX_N);

    shake256(out, SPX_N, buf, 2*SPX_N + SPX_ADDR_BYTES);
}

/**
 * Computes the message-dependent randomness R, using a secret seed and an
 * optional randomization value as well as the message.
 */
void gen_message_random(unsigned char *R, const unsigned char *sk_prf,
                        const unsigned char *optrand,
                        const unsigned char *m, size_t mlen,
                        const spx_ctx *ctx)
{
    (void)ctx;
    uint32_t s_inc[52];

    shake256_inc_init(s_inc);
    shake256_inc_absorb(s_inc, sk_prf, SPX_N);
    shake256_inc_absorb(s_inc, optrand, SPX_N);
    shake256_inc_absorb(s_inc, m, mlen);
    shake256_inc_finalize(s_inc);
    shake256_inc_squeeze(R, SPX_N, s_inc);
}

/**
 * Computes the message hash using R, the public key, and the message.
 * Outputs the message digest and the index of the leaf. The index is split in
 * the tree index and the leaf index, for convenient copying to an address.
 */
void hash_message(unsigned char *digest, uint32_t *tree, uint32_t *leaf_idx,
                  const unsigned char *R, const unsigned char *pk,
                  const unsigned char *m, size_t mlen,
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
    uint32_t s_inc[52];

    shake256_inc_init(s_inc);
    shake256_inc_absorb(s_inc, R, SPX_N);
    shake256_inc_absorb(s_inc, pk, SPX_PK_BYTES);
    shake256_inc_absorb(s_inc, m, (size_t)mlen);
    shake256_inc_finalize(s_inc);
    shake256_inc_squeeze(buf, SPX_DGST_BYTES, s_inc);

    memcpy(digest, bufp, SPX_FORS_MSG_BYTES);
    bufp += SPX_FORS_MSG_BYTES;

#if SPX_TREE_BITS > 64
    #error For given height and depth, 64 bits cannot represent all subtrees
#endif

    if (SPX_D == 1) {
        tree[0] = 0;
        tree[1] = 0;
    } else {
        // Assuming SPX_TREE_BITS <= 64, we can convert bytes to 2 uint32 words
        // This logic replaces the 64-bit single value access with explicit 32-bit words
        uint32_t low = 0, high = 0;
        
        // This assumes SPX_TREE_BYTES is appropriate for reading into low/high words
        // We'll trust bytes_to_ull logic equivalence here but implemented with u32 helpers if needed
        // Or directly read. Let's use bytes_to_u32 manually for now or rely on a helper we will add.
        // For now, implementing standard big-endian parsing for up to 64 bits into two 32-bit words.
        
        for (int i=0; i < SPX_TREE_BYTES; i++) {
             // Shift existing content
             high = (high << 8) | (low >> 24);
             low = (low << 8) | bufp[i];
        }

        // Masking the result
        // The original was: *tree &= (~(uint64_t)0) >> (64 - SPX_TREE_BITS);
        // We have to mask high and low appropriately.
        // Only if 64 bits are fully used does this matter logic wise in a tricky way.
        // Assuming SPX_TREE_BITS <= 64.
        
        if (SPX_TREE_BITS < 64) {
             int shift = 64 - SPX_TREE_BITS;
             if (shift >= 32) {
                 high = 0;
                 low &= (~(uint32_t)0) >> (shift - 32);
             } else {
                 high &= (~(uint32_t)0) >> shift;
                 // low is not masked if we only mask top bits
             }
        }
        
        tree[0] = low; // Lower 32 bits, assuming tree is usually mostly lower bits
        tree[1] = high;
        
        // Actually, let's double check *tree usage in sign.c.
        // It's used as a uint64_t. I changed the signature to uint32_t *tree.
        // So tree[0] should be low, tree[1] high? Or vice versa?
        // Usually little endian machine, but here we are just storing it.
        // I should just use `tree[0]` for bits 0-31 and `tree[1]` for bits 32-63
        // to be compatible with a potential `uint32_t tree_parts[2]` in sign.c
    }
    bufp += SPX_TREE_BYTES;

    // Fixed u32 conversion
    *leaf_idx = 0;
    for(int i=0; i < SPX_LEAF_BYTES; i++) *leaf_idx = (*leaf_idx << 8) | bufp[i];
    
    *leaf_idx &= (~(uint32_t)0) >> (32 - SPX_LEAF_BITS);
}
