#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include "util.h"

#include "api.h"
#include "masked_thash.h"
#include "params.h"
#include "masked_wots.h"
#include "masked_fors.h"
#include "masked_hash.h"
#include "thash.h"
#include "address.h"
#include "randombytes.h"
#include "masked_utils.h"
#include "masked_merkle.h"

// HW masked version of crypto_sign_seed_keypair
int masked_crypto_sign_seed_keypair(unsigned char *pk, unsigned char *sk,
                                      const unsigned char *seed)
{
    spx_ctx ctx;
    unsigned char rand_mask[SPX_N];

    memcpy(sk, seed, CRYPTO_SEEDBYTES);
    memcpy(pk, sk + 2*SPX_N, SPX_N);
    memcpy(ctx.pub_seed, pk, SPX_N);

    randombytes(rand_mask, SPX_N);
    for(int i = 0; i < SPX_N; i++) {
        ctx.sk_seed1[i] = sk[i] ^ rand_mask[i];
        ctx.sk_seed2[i] = rand_mask[i];
    }

    masked_initialize_hash_function(&ctx);

    masked_merkle_gen_root(sk + 3*SPX_N, &ctx);

    memcpy(pk + SPX_N, sk + 3*SPX_N, SPX_N);

    return 0;
}

/*
 * Generates an SPX key pair.
 * Format sk: [SK_SEED || SK_PRF || PUB_SEED || root]
 * Format pk: [PUB_SEED || root]
 */
int masked_crypto_sign_keypair(unsigned char *pk, unsigned char *sk)
{
  unsigned char seed[CRYPTO_SEEDBYTES];
  randombytes(seed, CRYPTO_SEEDBYTES);
  masked_crypto_sign_seed_keypair(pk, sk, seed);
  return 0;
}

// HW masked version of crypto_sign_signature
int masked_crypto_sign_signature(uint8_t *sig, size_t *siglen,
                                   const uint8_t *m, size_t mlen, const uint8_t *sk)
{
    spx_ctx ctx;

    const unsigned char *sk_prf = sk + SPX_N;
    const unsigned char *pk = sk + 2*SPX_N;

    unsigned char optrand[SPX_N];
    unsigned char mhash[SPX_FORS_MSG_BYTES];
    unsigned char root[SPX_N];
    uint32_t i;
    uint64_t tree;
    uint32_t idx_leaf;
    uint32_t wots_addr[8] = {0};
    uint32_t tree_addr[8] = {0};
    uint32_t tree_addr_bytes[2];

    unsigned char rand_mask[SPX_N];
    randombytes(rand_mask, SPX_N);

    memcpy(ctx.pub_seed, pk, SPX_N);

    for(int j = 0; j < SPX_N; j++) {
        ctx.sk_seed1[j] = sk[j] ^ rand_mask[j];
        ctx.sk_seed2[j] = rand_mask[j];
    }

    masked_initialize_hash_function(&ctx);

    set_type(wots_addr, SPX_ADDR_TYPE_WOTS);
    set_type(tree_addr, SPX_ADDR_TYPE_HASHTREE);

    randombytes(optrand, SPX_N);

    masked_gen_message_random(sig, sk_prf, optrand, m, mlen, &ctx);

    masked_hash_message(mhash, &tree, &idx_leaf, sig, pk, m, mlen, &ctx);
    sig += SPX_N;

    tree_addr_bytes[0] = (uint32_t)(tree);
    tree_addr_bytes[1] = (uint32_t)(tree >> 32);

    set_tree_addr(wots_addr, tree_addr_bytes);
    set_keypair_addr(wots_addr, idx_leaf);

    masked_fors_sign(sig, root, mhash, &ctx, wots_addr);

    sig += SPX_FORS_BYTES;

    for (i = 0; i < SPX_D; i++) {
        set_layer_addr(tree_addr, i);
        tree_addr_bytes[0] = (uint32_t)(tree);
        tree_addr_bytes[1] = (uint32_t)(tree >> 32);
        set_tree_addr(tree_addr, tree_addr_bytes);

        copy_subtree_addr(wots_addr, tree_addr);
        set_keypair_addr(wots_addr, idx_leaf);

        masked_merkle_sign(sig, root, &ctx, wots_addr, tree_addr, idx_leaf);

        sig += SPX_WOTS_BYTES + SPX_TREE_HEIGHT * SPX_N;

        idx_leaf = (tree & ((1 << SPX_TREE_HEIGHT)-1));
        tree = tree >> SPX_TREE_HEIGHT;
    }

    *siglen = SPX_BYTES;

    return 0;
}

// HW masked version of crypto_sign_verify
int masked_crypto_sign_verify(const uint8_t *sig, size_t siglen,
                                const uint8_t *m, size_t mlen, const uint8_t *pk)
{
    spx_ctx ctx;
    const unsigned char *pub_root = pk + SPX_N;
    unsigned char mhash[SPX_FORS_MSG_BYTES];
    unsigned char wots_pk[SPX_WOTS_BYTES];
    unsigned char root[SPX_N];
    unsigned char leaf[SPX_N];

    unsigned char root1[SPX_N], root2[SPX_N];

    unsigned int i;
    uint64_t tree;
    uint32_t idx_leaf;
    uint32_t wots_addr[8] = {0};
    uint32_t tree_addr[8] = {0};
    uint32_t wots_pk_addr[8] = {0};
    uint32_t tree_addr_bytes[2];

    if (siglen != SPX_BYTES) {
        return -1;
    }

    memcpy(ctx.pub_seed, pk, SPX_N);

    masked_initialize_hash_function(&ctx);

    set_type(wots_addr, SPX_ADDR_TYPE_WOTS);
    set_type(tree_addr, SPX_ADDR_TYPE_HASHTREE);
    set_type(wots_pk_addr, SPX_ADDR_TYPE_WOTSPK);

    masked_hash_message(mhash, &tree, &idx_leaf, sig, pk, m, mlen, &ctx);
    sig += SPX_N;

    tree_addr_bytes[0] = (uint32_t)(tree);
    tree_addr_bytes[1] = (uint32_t)(tree >> 32);

    set_tree_addr(wots_addr, tree_addr_bytes);
    set_keypair_addr(wots_addr, idx_leaf);

    masked_fors_pk_from_sig(root, sig, mhash, &ctx, wots_addr);
    sig += SPX_FORS_BYTES;

    for (i = 0; i < SPX_D; i++) {
        set_layer_addr(tree_addr, i);

        tree_addr_bytes[0] = (uint32_t)(tree);
        tree_addr_bytes[1] = (uint32_t)(tree >> 32);
        set_tree_addr(tree_addr, tree_addr_bytes);

        copy_subtree_addr(wots_addr, tree_addr);
        set_keypair_addr(wots_addr, idx_leaf);

        copy_keypair_addr(wots_pk_addr, wots_addr);

        masked_wots_pk_from_sig(wots_pk, sig, root, &ctx, wots_addr);
        sig += SPX_WOTS_BYTES;

        thash(leaf, wots_pk, SPX_WOTS_LEN, &ctx, wots_pk_addr);

        unsigned char zero_buf[SPX_N] = {0};
        masked_compute_root(root1, root2, leaf, zero_buf,
                              idx_leaf, 0, sig, SPX_TREE_HEIGHT, &ctx, tree_addr);

        for(int j=0; j<SPX_N; j++) root[j] = root1[j] ^ root2[j];

        sig += SPX_TREE_HEIGHT * SPX_N;

        idx_leaf = (tree & ((1 << SPX_TREE_HEIGHT)-1));
        tree = tree >> SPX_TREE_HEIGHT;
    }

    if (memcmp(root, pub_root, SPX_N)) {
        return -1;
    }

    return 0;
}

// HW masked version of crypto_sign
int masked_crypto_sign(unsigned char *sm, unsigned long long *smlen,
                         const unsigned char *m, unsigned long long mlen,
                         const unsigned char *sk)
{
    size_t siglen;

    masked_crypto_sign_signature(sm, &siglen, m, (size_t)mlen, sk);

    memmove(sm + SPX_BYTES, m, mlen);
    *smlen = siglen + mlen;

    return 0;
}

// HW masked version of crypto_sign_open
int masked_crypto_sign_open(unsigned char *m, unsigned long long *mlen,
                              const unsigned char *sm, unsigned long long smlen,
                              const unsigned char *pk)
{
    if (smlen < SPX_BYTES) {
        memset(m, 0, smlen);
        *mlen = 0;
        return -1;
    }

    *mlen = smlen - SPX_BYTES;

    if (masked_crypto_sign_verify(sm, SPX_BYTES, sm + SPX_BYTES, (size_t)*mlen, pk)) {
        memset(m, 0, smlen);
        *mlen = 0;
        return -1;
    }

    memmove(m, sm + SPX_BYTES, *mlen);

    return 0;
}
