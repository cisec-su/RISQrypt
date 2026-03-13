#include <stdint.h>
#include <string.h>

#include "swmasked_utils.h"
#include "swmasked_utilsx1.h"
#include "swmasked_wots.h"
#include "swmasked_wotsx1.h"
#include "swmasked_merkle.h"
#include "address.h"
#include "params.h"
//  To generate masked merkle tree structure, unlike previous implementations,
// we need to change the fundemental logic rather than provide masked
// options. We will change how the data is stored in the treehash structure to
// so that each stack contains share pairs rather than single nodes.

// Masked version of merkle_sign
void merkle_sign_masked(uint8_t *sig, unsigned char *root,
                        const spx_ctx *ctx,
                        uint32_t wots_addr[8], uint32_t tree_addr[8],
                        uint32_t idx_leaf)
{
    // The Auth Path is written directly to the output buffer 
    unsigned char *auth_path = sig + SPX_WOTS_BYTES;
    
    unsigned int steps[ SPX_WOTS_LEN ];
    struct leaf_info_x1_masked info = { 0 };

    INITIALIZE_LEAF_INFO_X1_MASKED(info, wots_addr, steps);

    // Prepare split buffers for the WOTS signature
    SPX_VLA(uint8_t, wots_sig1, SPX_WOTS_BYTES);
    SPX_VLA(uint8_t, wots_sig2, SPX_WOTS_BYTES);

    info.wots_sig1 = wots_sig1;
    info.wots_sig2 = wots_sig2;
    
    // Calculate WOTS steps
    chain_lengths(steps, root); // The message being signed is treated as public 
    info.wots_steps = steps;

    // Setup addresses
    set_type(&tree_addr[0], SPX_ADDR_TYPE_HASHTREE);
    set_type(&info.pk_addr[0], SPX_ADDR_TYPE_WOTSPK);
    copy_subtree_addr(&info.leaf_addr[0], wots_addr);
    copy_subtree_addr(&info.pk_addr[0], wots_addr);

    info.wots_sign_leaf = idx_leaf;

    // Prepare buffers for the new root 
    unsigned char root1[SPX_N];
    unsigned char root2[SPX_N];

    // Run masked treehash to generate the auth path and new root
    treehashx1_masked(root1, root2, auth_path, 
                      ctx,
                      idx_leaf, 0,
                      SPX_TREE_HEIGHT,
                      wots_gen_leafx1_masked, // Masked leaf generation function
                      tree_addr, &info);

    for(int i = 0; i < SPX_WOTS_BYTES; i++) {
        sig[i] = wots_sig1[i] ^ wots_sig2[i]; // Recombine signature shares by share1 ^share2
    }

    // Recombine the new root
    for(int i = 0; i < SPX_N; i++) {
        root[i] = root1[i] ^ root2[i];
    }
}

// Masked version of merkle_gen_root
void merkle_gen_root_masked(unsigned char *root, const spx_ctx *ctx)
{
    // Allocate temporary buffer to discard auth path output 
    SPX_VLA(uint8_t, auth_path, SPX_TREE_HEIGHT * SPX_N + SPX_WOTS_BYTES);
    
    uint32_t top_tree_addr[8] = {0};
    uint32_t wots_addr[8] = {0};

    set_layer_addr(top_tree_addr, SPX_D - 1);
    set_layer_addr(wots_addr, SPX_D - 1);

    // Call the masked signer with leaf index ~0
    merkle_sign_masked(auth_path, root, ctx,
                       wots_addr, top_tree_addr,
                       (uint32_t)~0);
}
