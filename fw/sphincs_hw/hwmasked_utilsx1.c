#include <string.h>

#include "hwmasked_utils.h"
#include "hwmasked_utilsx1.h"
#include "params.h"
#include "hwmasked_thash.h"
#include "address.h"

// HW Masked version of treehashx1
void treehashx1_hwmasked(unsigned char *root1, unsigned char *root2,
                         unsigned char *auth_path,
                         const spx_ctx* ctx,
                         uint32_t leaf_idx, uint32_t idx_offset,
                         uint32_t tree_height,
                         void (*gen_leaf_hwmasked)(
                             unsigned char*,
                             unsigned char* ,
                             const spx_ctx*,
                             uint32_t idx, void *info),
                         uint32_t tree_addr[8],
                         void *info)
{
    SPX_VLA(uint8_t, stack1, tree_height * SPX_N);
    SPX_VLA(uint8_t, stack2, tree_height * SPX_N);

    uint32_t idx;
    uint32_t max_idx = (uint32_t)((1 << tree_height) - 1);

    for (idx = 0;; idx++) {
        unsigned char current1[2 * SPX_N];
        unsigned char current2[2 * SPX_N];

        gen_leaf_hwmasked(&current1[SPX_N], &current2[SPX_N],
                          ctx, idx + idx_offset, info);

        uint32_t internal_idx_offset = idx_offset;
        uint32_t internal_idx = idx;
        uint32_t internal_leaf = leaf_idx;
        uint32_t h;

        for (h = 0;; h++, internal_idx >>= 1, internal_leaf >>= 1) {

            if (h == tree_height) {
                memcpy(root1, &current1[SPX_N], SPX_N);
                memcpy(root2, &current2[SPX_N], SPX_N);
                return;
            }

            if ((internal_idx ^ internal_leaf) == 0x01) {
                for(int j=0; j<SPX_N; j++) {
                    auth_path[h * SPX_N + j] = current1[SPX_N + j] ^ current2[SPX_N + j];
                }
            }

            if ((internal_idx & 1) == 0 && idx < max_idx) {
                break;
            }

            internal_idx_offset >>= 1;
            set_tree_height(tree_addr, h + 1);
            set_tree_index(tree_addr, internal_idx/2 + internal_idx_offset);

            unsigned char *left1 = &stack1[h * SPX_N];
            unsigned char *left2 = &stack2[h * SPX_N];

            memcpy(&current1[0], left1, SPX_N);
            memcpy(&current2[0], left2, SPX_N);

            hwmasked_thash(&current1[1 * SPX_N], &current2[1 * SPX_N],
                           &current1[0 * SPX_N], &current2[0 * SPX_N],
                           2, ctx, tree_addr);
        }

        memcpy(&stack1[h * SPX_N], &current1[SPX_N], SPX_N);
        memcpy(&stack2[h * SPX_N], &current2[SPX_N], SPX_N);
    }
}
