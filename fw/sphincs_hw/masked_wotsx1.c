#include <stdint.h>
#include <string.h>

#include "utils_masked.h"
#include "hash_masked.h"
#include "thash_masked.h"
#include "wots_masked.h"
#include "wotsx1_masked.h"
#include "address.h"
#include "params.h"

// Masked version of wots_gen_leafx1
void wots_gen_leafx1_masked(unsigned char *dest1, unsigned char *dest2,
                            const spx_ctx *ctx,
                            uint32_t leaf_idx, void *v_info) 
{
    struct leaf_info_x1 *info = v_info;
    uint32_t *leaf_addr = info->leaf_addr;
    uint32_t *pk_addr = info->pk_addr;
    unsigned int i, k;
    
    unsigned char pk_buffer1[ SPX_WOTS_BYTES ]; // Doubled buffer size for two shares (NOTE: Might need the increase)
    unsigned char pk_buffer2[ SPX_WOTS_BYTES ];
    
    unsigned char *buffer1;
    unsigned char *buffer2;
    uint32_t wots_k_mask;

    // Are we signing or just building the tree
    if (leaf_idx == info->wots_sign_leaf) {
        wots_k_mask = 0;
    } else {
        wots_k_mask = (uint32_t)~0;
    }

    set_keypair_addr( leaf_addr, leaf_idx );
    set_keypair_addr( pk_addr, leaf_idx );

    // Iterate over the WOTS chains 
    for (i = 0; i < SPX_WOTS_LEN; i++) {
        // Calculate current pointers into the large buffers
        buffer1 = pk_buffer1 + i * SPX_N;
        buffer2 = pk_buffer2 + i * SPX_N;

        uint32_t wots_k = info->wots_steps[i] | wots_k_mask;

        // Start with the secret seed and generate masked priv key 
        set_chain_addr(leaf_addr, i);
        set_hash_addr(leaf_addr, 0);
        set_type(leaf_addr, SPX_ADDR_TYPE_WOTSPRF);

        // Split the secret seed into two shares for the start of the chain
        masked_prf_addr(buffer1, buffer2, ctx, leaf_addr);

        set_type(leaf_addr, SPX_ADDR_TYPE_WOTS);

        // Iterate down the masked WOTS chain
        for (k=0;; k++) {
            // Check if this is the value needed for the signature 
            if (k == wots_k) {
                // Save both shares
                memcpy( info->wots_sig1 + i * SPX_N, buffer1, SPX_N );
                memcpy( info->wots_sig2 + i * SPX_N, buffer2, SPX_N );
            }

            // Check if we hit the top of the chain 
            if (k == SPX_WOTS_W - 1) break;

            // Iterate one step on the chain
            set_hash_addr(leaf_addr, k);

            masked_thash(buffer1, buffer2, // Output
                         buffer1, buffer2, // Input
                         1, ctx, leaf_addr);
        }
    }

    // Do the final thash to generate the masked pub keys
    masked_thash(dest1, dest2, 
                 pk_buffer1, pk_buffer2, 
                 SPX_WOTS_LEN, ctx, pk_addr);
}
