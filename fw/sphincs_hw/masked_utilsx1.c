#include <string.h>

#include "masked_utils.h"
#include "masked_utilsx1.h"
#include "params.h"
#include "masked_thash.h"
#include "address.h"

// Masked version of the treehashx1 function
void treehashx1_masked(unsigned char *root1, unsigned char *root2,
                       unsigned char *auth_path,
                       const spx_ctx* ctx,
                       uint32_t leaf_idx, uint32_t idx_offset,
                       uint32_t tree_height,
                       void (*gen_leaf_masked)(
                           unsigned char*,
                           unsigned char* ,
                           const spx_ctx*,
                           uint32_t idx, void *info),
                       uint32_t tree_addr[8],
                       void *info)
{
    // Two stacks to keep shares separate 
    SPX_VLA(uint8_t, stack1, tree_height * SPX_N);
    SPX_VLA(uint8_t, stack2, tree_height * SPX_N);

    uint32_t idx;
    uint32_t max_idx = (uint32_t)((1 << tree_height) - 1);

    for (idx = 0;; idx++) {
        // Logical node buffers
        // Size is 2*SPX_N => thash: (Left || Right)
        unsigned char current1[2 * SPX_N];
        unsigned char current2[2 * SPX_N];

        // Generate masked leaf
        // Write to index
        gen_leaf_masked(&current1[SPX_N], &current2[SPX_N], 
                        ctx, idx + idx_offset, info);

        // Now combine the right node with left ones
        uint32_t internal_idx_offset = idx_offset;
        uint32_t internal_idx = idx;
        uint32_t internal_leaf = leaf_idx;
        uint32_t h;     
        
        for (h = 0;; h++, internal_idx >>= 1, internal_leaf >>= 1) {

            // Check if it hits the top of the tree
            if (h == tree_height) {
                // Yes, return shares
                memcpy(root1, &current1[SPX_N], SPX_N);
                memcpy(root2, &current2[SPX_N], SPX_N);
                return;
            }

            if ((internal_idx ^ internal_leaf) == 0x01) { // Check if the node we have is a part of the authentication path
                for(int j=0; j<SPX_N; j++) {
                    auth_path[h * SPX_N + j] = current1[SPX_N + j] ^ current2[SPX_N + j]; //  Share1 ^ Share2
                }
            }

            if ((internal_idx & 1) == 0 && idx < max_idx) { // Check if we're at a left child
                break;
            }

            internal_idx_offset >>= 1;
            set_tree_height(tree_addr, h + 1);
            set_tree_index(tree_addr, internal_idx/2 + internal_idx_offset ); // Combine the left and right logical nodes together 

            // Load Left Node from Stacks 
            unsigned char *left1 = &stack1[h * SPX_N];
            unsigned char *left2 = &stack2[h * SPX_N];
            
            // Copy Left Child to the initial slot of current buffers
            memcpy(&current1[0], left1, SPX_N);
            memcpy(&current2[0], left2, SPX_N);

            // Call masked thash 
            masked_thash(&current1[1 * SPX_N], &current2[1 * SPX_N], // Output
                         &current1[0 * SPX_N], &current2[0 * SPX_N], // Input
                         2, ctx, tree_addr);
        }

        // Find a left child, save the current shares to the stacks 
        memcpy(&stack1[h * SPX_N], &current1[SPX_N], SPX_N);
        memcpy(&stack2[h * SPX_N], &current2[SPX_N], SPX_N);
    }
}
