#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "masked_fors.h"
#include "masked_utils.h"
#include "masked_utilsx1.h"
#include "masked_hash.h"
#include "masked_thash.h"
#include "address.h"
#include "randombytes.h"

// Non-masked functions from original file
struct fors_gen_leaf_info {
    uint32_t leaf_addrx[8];
};

static void message_to_indices(uint32_t *indices, const unsigned char *m)
{
    unsigned int i, j;
    unsigned int offset = 0;

    for (i = 0; i < SPX_FORS_TREES; i++) {
        indices[i] = 0;
        for (j = 0; j < SPX_FORS_HEIGHT; j++) {
            indices[i] ^= ((m[offset >> 3] >> (offset & 0x7)) & 1u) << j;
            offset++;
        }
    }
}

// MAsked version of fors_gen_sk
static void fors_gen_sk_masked(unsigned char *sk1, unsigned char *sk2,
                               const spx_ctx *ctx,
                               uint32_t fors_leaf_addr[8])
{
    masked_prf_addr(sk1, sk2, ctx, fors_leaf_addr);
}

// Masked version of fors_sk_to_leaf
static void fors_sk_to_leaf_masked(unsigned char *leaf1, unsigned char *leaf2,
                                   const unsigned char *sk1, const unsigned char *sk2,
                                   const spx_ctx *ctx,
                                   uint32_t fors_leaf_addr[8])
{
    masked_thash(leaf1, leaf2, sk1, sk2, 1, ctx, fors_leaf_addr);
}


// Masked version of fors_gen_leafx1
static void fors_gen_leafx1_masked(unsigned char *leaf1, unsigned char *leaf2,
                                   const spx_ctx *ctx,
                                   uint32_t addr_idx, void *info)
{
    struct fors_gen_leaf_info *fors_info = info;
    uint32_t *fors_leaf_addr = fors_info->leaf_addrx;

    set_tree_index(fors_leaf_addr, addr_idx); // Only set the parts that the caller doesn't set 
    set_type(fors_leaf_addr, SPX_ADDR_TYPE_FORSPRF);

    // Generate masked secret scalar 
    fors_gen_sk_masked(leaf1, leaf2, ctx, fors_leaf_addr);

    // Hash scalar to leaf 
    set_type(fors_leaf_addr, SPX_ADDR_TYPE_FORSTREE);
    fors_sk_to_leaf_masked(leaf1, leaf2, 
                           leaf1, leaf2, // Input is the previous generated scalar
                           ctx, fors_leaf_addr);
}


// Masked version of fors_sign
void fors_sign_masked(unsigned char *sig, unsigned char *pk,
                      const unsigned char *m,
                      const spx_ctx *ctx,
                      const uint32_t fors_addr[8])
{
    uint32_t indices[SPX_FORS_TREES];
    
    // DOuble the buffers for the roots of all FORS trees
    SPX_VLA(uint8_t, roots1, SPX_FORS_TREES * SPX_N);
    SPX_VLA(uint8_t, roots2, SPX_FORS_TREES * SPX_N);

    // Temporary buffers for the secret scalar of the chosen leaf
    unsigned char sk_share1[SPX_N];
    unsigned char sk_share2[SPX_N];

    uint32_t fors_tree_addr[8] = {0};
    struct fors_gen_leaf_info fors_info = {0};
    uint32_t *fors_leaf_addr = fors_info.leaf_addrx;
    uint32_t fors_pk_addr[8] = {0};
    uint32_t idx_offset;
    unsigned int i;

    copy_keypair_addr(fors_tree_addr, fors_addr);
    copy_keypair_addr(fors_leaf_addr, fors_addr);

    copy_keypair_addr(fors_pk_addr, fors_addr);
    set_type(fors_pk_addr, SPX_ADDR_TYPE_FORSPK);

    message_to_indices(indices, m);

    for (i = 0; i < SPX_FORS_TREES; i++) {
        idx_offset = i * (1 << SPX_FORS_HEIGHT);

        set_tree_height(fors_tree_addr, 0);
        set_tree_index(fors_tree_addr, indices[i] + idx_offset);
        set_type(fors_tree_addr, SPX_ADDR_TYPE_FORSPRF);

        // Generate the secret key scalar
        fors_gen_sk_masked(sk_share1, sk_share2, ctx, fors_tree_addr);
        
        set_type(fors_tree_addr, SPX_ADDR_TYPE_FORSTREE);

        // Write scalar to signature by recombining shares
        for(int j=0; j<SPX_N; j++) {
            sig[j] = sk_share1[j] ^ sk_share2[j];
        }
        sig += SPX_N;

        // Compute auth path using masked treehash by using the direct sig value of public
        treehashx1_masked(roots1 + i*SPX_N, roots2 + i*SPX_N, // output roots
                          sig,                                // output auth path
                          ctx,
                          indices[i], idx_offset, SPX_FORS_HEIGHT, 
                          fors_gen_leafx1_masked,             // masked callback
                          fors_tree_addr, &fors_info);

        sig += SPX_N * SPX_FORS_HEIGHT;
    }

    // Hash horizontally across all tree roots to derive the public key 
    unsigned char pk1[SPX_N];
    unsigned char pk2[SPX_N];

    masked_thash(pk1, pk2,      // output
                 roots1, roots2, // input
                 SPX_FORS_TREES, ctx, fors_pk_addr);

    // Recombine final PK
    for(int j=0; j<SPX_N; j++) {
        pk[j] = pk1[j] ^ pk2[j];
    }
}

// Masked version of fors_pk_from_sig
void fors_pk_from_sig_masked(unsigned char *pk,
                      const unsigned char *sig, const unsigned char *m,
                      const spx_ctx* ctx,
                      const uint32_t fors_addr[8])
{
    uint32_t indices[SPX_FORS_TREES];
    
    // Roots of subtrees
    SPX_VLA(uint8_t, roots1, SPX_FORS_TREES * SPX_N);
    SPX_VLA(uint8_t, roots2, SPX_FORS_TREES * SPX_N);
    
    // Leaf buffer shares
    unsigned char leaf1[SPX_N];
    unsigned char leaf2[SPX_N];
    
    // Random mask for splitting public signature
    unsigned char rand_mask[SPX_N];

    uint32_t fors_tree_addr[8] = {0};
    uint32_t fors_pk_addr[8] = {0};
    uint32_t idx_offset;
    unsigned int i;

    copy_keypair_addr(fors_tree_addr, fors_addr);
    copy_keypair_addr(fors_pk_addr, fors_addr);

    set_type(fors_tree_addr, SPX_ADDR_TYPE_FORSTREE);
    set_type(fors_pk_addr, SPX_ADDR_TYPE_FORSPK);

    message_to_indices(indices, m);

    for (i = 0; i < SPX_FORS_TREES; i++) {
        idx_offset = i * (1 << SPX_FORS_HEIGHT);

        set_tree_height(fors_tree_addr, 0);
        set_tree_index(fors_tree_addr, indices[i] + idx_offset);

        // Derive the leaf from the included secret key part
        randombytes(rand_mask, SPX_N);
        unsigned char sk_in1[SPX_N];
        unsigned char sk_in2[SPX_N];
        
        for(int j=0; j<SPX_N; j++) {
            sk_in1[j] = sig[j] ^ rand_mask[j];
            sk_in2[j] = rand_mask[j];
        }
        
        fors_sk_to_leaf_masked(leaf1, leaf2, sk_in1, sk_in2, ctx, fors_tree_addr);
        sig += SPX_N;

        // Derive the corresponding root node of this tree
        compute_root_masked(roots1 + i*SPX_N, roots2 + i*SPX_N, 
                            leaf1, leaf2, 
                            indices[i], idx_offset,
                            sig, SPX_FORS_HEIGHT, ctx, fors_tree_addr);
                            
        sig += SPX_N * SPX_FORS_HEIGHT;
    }

    // Hash horizontally 
    unsigned char pk1[SPX_N];
    unsigned char pk2[SPX_N];
    
    masked_thash(pk1, pk2, roots1, roots2, SPX_FORS_TREES, ctx, fors_pk_addr);

    // Recombine 
    for(int j=0; j<SPX_N; j++) {
        pk[j] = pk1[j] ^ pk2[j];
    }
}
