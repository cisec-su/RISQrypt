#include <stddef.h>
#include <string.h>
#include <stdint.h>

#include "api.h"
#include "params.h"
#include "wots_.h"
#include "fors_masked.h"
#include "hash_masked.h"
#include "thash_masked.h"
#include "address.h"
#include "randombytes.h"
#include "utils_masked.h"
#include "merkle_masked.h"
// Non masked functions from original file
unsigned long long crypto_sign_secretkeybytes(void)
{
    return CRYPTO_SECRETKEYBYTES;
}

/*
 * Returns the length of a public key, in bytes
 */
unsigned long long crypto_sign_publickeybytes(void)
{
    return CRYPTO_PUBLICKEYBYTES;
}

/*
 * Returns the length of a signature, in bytes
 */
unsigned long long crypto_sign_bytes(void)
{
    return CRYPTO_BYTES;
}

/*
 * Returns the length of the seed required to generate a key pair, in bytes
 */
unsigned long long crypto_sign_seedbytes(void)
{
    return CRYPTO_SEEDBYTES;
}


// Masked version of crypto_sign_seed_keypair
int crypto_sign_seed_keypair_masked(unsigned char *pk, unsigned char *sk,
                             const unsigned char *seed)
{
    spx_ctx ctx;
    unsigned char rand_mask[SPX_N];

    //Initialize SK_SEED, SK_PRF and PUB_SEED from seed
    memcpy(sk, seed, CRYPTO_SEEDBYTES);

    memcpy(pk, sk + 2*SPX_N, SPX_N);

    memcpy(ctx.pub_seed, pk, SPX_N);

    // Splitting the SK_SEED so the masked root generation works
    randombytes(rand_mask, SPX_N);
    for(int i = 0; i < SPX_N; i++) {
        ctx.sk_seed1[i] = sk[i] ^ rand_mask[i];
        ctx.sk_seed2[i] = rand_mask[i];
    }

    // This hook allows the hash function instantiation to do whatever
    // preparation or computation it needs, based on the public seed
    initialize_hash_function(&ctx);

    // Compute root node of the top-most subtree using masked function
    merkle_gen_root_masked(sk + 3*SPX_N, &ctx);

    memcpy(pk + SPX_N, sk + 3*SPX_N, SPX_N);

    return 0;
}

// MAsked version of crypto_sign_signature
int crypto_sign_signature_masked(uint8_t *sig, size_t *siglen,
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

    // Generate a random mask
    unsigned char rand_mask[SPX_N];
    randombytes(rand_mask, SPX_N);

    // Load public seed 
    memcpy(ctx.pub_seed, pk, SPX_N);

    // Split the secret seed
    for(int j = 0; j < SPX_N; j++) {
        ctx.sk_seed1[j] = sk[j] ^ rand_mask[j]; //SK_SEED ^ R
        ctx.sk_seed2[j] = rand_mask[j]; // R
    }

    initialize_hash_function(&ctx);

    set_type(wots_addr, SPX_ADDR_TYPE_WOTS);
    set_type(tree_addr, SPX_ADDR_TYPE_HASHTREE);

    randombytes(optrand, SPX_N);
    
    //Compute the digest randomization value
    gen_message_random(sig, sk_prf, optrand, m, mlen, &ctx);

    // Derive the message digest and leaf index from R, PK and M
    hash_message(mhash, &tree, &idx_leaf, sig, pk, m, mlen, &ctx);
    sig += SPX_N;

    set_tree_addr(wots_addr, tree);
    set_keypair_addr(wots_addr, idx_leaf);

    // Sign the message hash using masked FORS.
    fors_sign_masked(sig, root, mhash, &ctx, wots_addr);
    
    sig += SPX_FORS_BYTES;

    for (i = 0; i < SPX_D; i++) {
        set_layer_addr(tree_addr, i);
        set_tree_addr(tree_addr, tree);

        copy_subtree_addr(wots_addr, tree_addr);
        set_keypair_addr(wots_addr, idx_leaf);

        // Sign the subtree using masked Merkle logic
        merkle_sign_masked(sig, root, &ctx, wots_addr, tree_addr, idx_leaf);
        
        sig += SPX_WOTS_BYTES + SPX_TREE_HEIGHT * SPX_N;

        // Update the indices for the next layer.
        idx_leaf = (tree & ((1 << SPX_TREE_HEIGHT)-1));
        tree = tree >> SPX_TREE_HEIGHT;
    }

    *siglen = SPX_BYTES;

    return 0;
}

// Masked version of crypto_sign_verify
int crypto_sign_verify_masked(const uint8_t *sig, size_t siglen,
                       const uint8_t *m, size_t mlen, const uint8_t *pk)
{
    spx_ctx ctx;
    const unsigned char *pub_root = pk + SPX_N;
    unsigned char mhash[SPX_FORS_MSG_BYTES];
    unsigned char wots_pk[SPX_WOTS_BYTES];
    unsigned char root[SPX_N];
    unsigned char leaf[SPX_N];
    
    // Temp buffers for masked outputs
    unsigned char root1[SPX_N], root2[SPX_N];
    
    unsigned int i;
    uint64_t tree;
    uint32_t idx_leaf;
    uint32_t wots_addr[8] = {0};
    uint32_t tree_addr[8] = {0};
    uint32_t wots_pk_addr[8] = {0};

    if (siglen != SPX_BYTES) {
        return -1;
    }

    memcpy(ctx.pub_seed, pk, SPX_N);

    initialize_hash_function(&ctx);

    set_type(wots_addr, SPX_ADDR_TYPE_WOTS);
    set_type(tree_addr, SPX_ADDR_TYPE_HASHTREE);
    set_type(wots_pk_addr, SPX_ADDR_TYPE_WOTSPK);

    hash_message(mhash, &tree, &idx_leaf, sig, pk, m, mlen, &ctx);
    sig += SPX_N;

    set_tree_addr(wots_addr, tree);
    set_keypair_addr(wots_addr, idx_leaf);

    fors_pk_from_sig_masked(root, sig, mhash, &ctx, wots_addr);
    sig += SPX_FORS_BYTES;

    for (i = 0; i < SPX_D; i++) {  // For each subtree
        set_layer_addr(tree_addr, i);
        set_tree_addr(tree_addr, tree);

        copy_subtree_addr(wots_addr, tree_addr);
        set_keypair_addr(wots_addr, idx_leaf);

        copy_keypair_addr(wots_pk_addr, wots_addr);

        wots_pk_from_sig_masked(wots_pk, sig, root, &ctx, wots_addr);
        sig += SPX_WOTS_BYTES;

        thash(leaf, wots_pk, SPX_WOTS_LEN, &ctx, wots_pk_addr);

        

        // Share1 = leaf, Share2 = 0s.
        unsigned char zero_buf[SPX_N] = {0};
        compute_root_masked(root1, root2, leaf, zero_buf, 
                            idx_leaf, 0, sig, SPX_TREE_HEIGHT, &ctx, tree_addr);

        // Recombine root for next iteration
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

// Masked version of crypto_sign
int crypto_sign_masked(unsigned char *sm, unsigned long long *smlen,
                const unsigned char *m, unsigned long long mlen,
                const unsigned char *sk)
{
    size_t siglen;

    crypto_sign_signature_masked(sm, &siglen, m, (size_t)mlen, sk);

    memmove(sm + SPX_BYTES, m, mlen);
    *smlen = siglen + mlen;

    return 0;
}

// Masked version of crypto_sign_open
int crypto_sign_open_masked(unsigned char *m, unsigned long long *mlen,
                     const unsigned char *sm, unsigned long long smlen,
                     const unsigned char *pk)
{
    if (smlen < SPX_BYTES) {
        memset(m, 0, smlen);
        *mlen = 0;
        return -1;
    }

    *mlen = smlen - SPX_BYTES;

    if (crypto_sign_verify_masked(sm, SPX_BYTES, sm + SPX_BYTES, (size_t)*mlen, pk)) {
        memset(m, 0, smlen);
        *mlen = 0;
        return -1;
    }

    memmove(m, sm + SPX_BYTES, *mlen);

    return 0;
}
