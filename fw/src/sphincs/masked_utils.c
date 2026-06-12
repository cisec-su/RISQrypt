#include <string.h>

#include "hwmasked_utils.h"
#include "params.h"
#include "hwmasked_hash.h"
#include "hwmasked_thash.h"
#include "address.h"
#include "randombytes.h"
#include "hwmasked_wotsx1.h"

// HW Masked version of compute_root
void compute_root_hwmasked(unsigned char *root1, unsigned char *root2,
                           const unsigned char *leaf1, const unsigned char *leaf2,
                           uint32_t leaf_idx, uint32_t idx_offset,
                           const unsigned char *auth_path, uint32_t tree_height,
                           const spx_ctx *ctx, uint32_t addr[8])
{
    uint32_t i;
    unsigned char buffer1[2 * SPX_N];
    unsigned char buffer2[2 * SPX_N];
    unsigned char auth_mask[SPX_N];

    if (leaf_idx & 1) {
        memcpy(buffer1 + SPX_N, leaf1, SPX_N);
        memcpy(buffer2 + SPX_N, leaf2, SPX_N);

        randombytes(auth_mask, SPX_N);
        for (int j=0; j<SPX_N; j++) {
            buffer1[j] = auth_path[j] ^ auth_mask[j];
            buffer2[j] = auth_mask[j];
        }
    }
    else {
        memcpy(buffer1, leaf1, SPX_N);
        memcpy(buffer2, leaf2, SPX_N);

        randombytes(auth_mask, SPX_N);
        for (int j=0; j<SPX_N; j++) {
            buffer1[SPX_N + j] = auth_path[j] ^ auth_mask[j];
            buffer2[SPX_N + j] = auth_mask[j];
        }
    }
    auth_path += SPX_N;

    for (i = 0; i < tree_height - 1; i++) {
        leaf_idx >>= 1;
        idx_offset >>= 1;

        set_tree_height(addr, i + 1);
        set_tree_index(addr, leaf_idx + idx_offset);

        if (leaf_idx & 1) {
            hwmasked_thash(buffer1 + SPX_N, buffer2 + SPX_N,
                           buffer1, buffer2,
                           2, ctx, addr);

            randombytes(auth_mask, SPX_N);
            for (int j=0; j<SPX_N; j++) {
                buffer1[j] = auth_path[j] ^ auth_mask[j];
                buffer2[j] = auth_mask[j];
            }
        }
        else {
            hwmasked_thash(buffer1, buffer2,
                           buffer1, buffer2,
                           2, ctx, addr);

            randombytes(auth_mask, SPX_N);
            for (int j=0; j<SPX_N; j++) {
                buffer1[SPX_N + j] = auth_path[j] ^ auth_mask[j];
                buffer2[SPX_N + j] = auth_mask[j];
            }
        }
        auth_path += SPX_N;
    }

    leaf_idx >>= 1;
    idx_offset >>= 1;
    set_tree_height(addr, tree_height);
    set_tree_index(addr, leaf_idx + idx_offset);

    hwmasked_thash(root1, root2, buffer1, buffer2, 2, ctx, addr);
}
