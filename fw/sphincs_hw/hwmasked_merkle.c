#include <stdint.h>
#include <string.h>

#include "hwmasked_utils.h"
#include "hwmasked_utilsx1.h"
#include "hwmasked_wots.h"
#include "hwmasked_wotsx1.h"
#include "hwmasked_merkle.h"
#include "address.h"
#include "params.h"

// HW masked version of merkle_sign
void merkle_sign_hwmasked(uint8_t *sig, unsigned char *root,
                          const spx_ctx *ctx,
                          uint32_t wots_addr[8], uint32_t tree_addr[8],
                          uint32_t idx_leaf)
{
    unsigned char *auth_path = sig + SPX_WOTS_BYTES;

    unsigned int steps[ SPX_WOTS_LEN ];
    struct leaf_info_x1_hwmasked info = { 0 };

    INITIALIZE_LEAF_INFO_X1_HWMASKED(info, wots_addr, steps);

    SPX_VLA(uint8_t, wots_sig1, SPX_WOTS_BYTES);
    SPX_VLA(uint8_t, wots_sig2, SPX_WOTS_BYTES);

    info.wots_sig1 = wots_sig1;
    info.wots_sig2 = wots_sig2;

    chain_lengths(steps, root);
    info.wots_steps = steps;

    set_type(&tree_addr[0], SPX_ADDR_TYPE_HASHTREE);
    set_type(&info.pk_addr[0], SPX_ADDR_TYPE_WOTSPK);
    copy_subtree_addr(&info.leaf_addr[0], wots_addr);
    copy_subtree_addr(&info.pk_addr[0], wots_addr);

    info.wots_sign_leaf = idx_leaf;

    unsigned char root1[SPX_N];
    unsigned char root2[SPX_N];

    treehashx1_hwmasked(root1, root2, auth_path,
                        ctx,
                        idx_leaf, 0,
                        SPX_TREE_HEIGHT,
                        wots_gen_leafx1_hwmasked,
                        tree_addr, &info);

    for(int i = 0; i < SPX_WOTS_BYTES; i++) {
        sig[i] = wots_sig1[i] ^ wots_sig2[i];
    }

    for(int i = 0; i < SPX_N; i++) {
        root[i] = root1[i] ^ root2[i];
    }
}

// HW masked version of merkle_gen_root
void merkle_gen_root_hwmasked(unsigned char *root, const spx_ctx *ctx)
{
    SPX_VLA(uint8_t, auth_path, SPX_TREE_HEIGHT * SPX_N + SPX_WOTS_BYTES);

    uint32_t top_tree_addr[8] = {0};
    uint32_t wots_addr[8] = {0};

    set_layer_addr(top_tree_addr, SPX_D - 1);
    set_layer_addr(wots_addr, SPX_D - 1);

    merkle_sign_hwmasked(auth_path, root, ctx,
                         wots_addr, top_tree_addr,
                         (uint32_t)~0);
}
