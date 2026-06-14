#include <stdint.h>
#include <string.h>

#include "masked_utils.h"
#include "masked_hash.h"
#include "masked_thash.h"
#include "masked_wots.h"
#include "masked_wotsx1.h"
#include "address.h"
#include "params.h"

// HW masked version of wots_gen_leafx1
void masked_wots_gen_leafx1(unsigned char *dest1, unsigned char *dest2,
                              const spx_ctx *ctx,
                              uint32_t leaf_idx, void *v_info)
{
    struct masked_leaf_info_x1 *info = (struct masked_leaf_info_x1 *)v_info;
    uint32_t *leaf_addr = info->leaf_addr;
    uint32_t *pk_addr = info->pk_addr;
    unsigned int i, k;

    unsigned char pk_buffer1[ SPX_WOTS_BYTES ];
    unsigned char pk_buffer2[ SPX_WOTS_BYTES ];

    unsigned char *buffer1;
    unsigned char *buffer2;
    uint32_t wots_k_mask;

    if (leaf_idx == info->wots_sign_leaf) {
        wots_k_mask = 0;
    } else {
        wots_k_mask = (uint32_t)~0;
    }

    set_keypair_addr( leaf_addr, leaf_idx );
    set_keypair_addr( pk_addr, leaf_idx );

    for (i = 0; i < SPX_WOTS_LEN; i++) {
        buffer1 = pk_buffer1 + i * SPX_N;
        buffer2 = pk_buffer2 + i * SPX_N;

        uint32_t wots_k = info->wots_steps[i] | wots_k_mask;

        set_chain_addr(leaf_addr, i);
        set_hash_addr(leaf_addr, 0);
        set_type(leaf_addr, SPX_ADDR_TYPE_WOTSPRF);

        masked_prf_addr(buffer1, buffer2, ctx, leaf_addr);

        set_type(leaf_addr, SPX_ADDR_TYPE_WOTS);

        for (k=0;; k++) {
            if (k == wots_k) {
                memcpy( info->wots_sig1 + i * SPX_N, buffer1, SPX_N );
                memcpy( info->wots_sig2 + i * SPX_N, buffer2, SPX_N );
            }

            if (k == SPX_WOTS_W - 1) break;

            set_hash_addr(leaf_addr, k);

            masked_thash(buffer1, buffer2,
                           buffer1, buffer2,
                           1, ctx, leaf_addr);
        }
    }

    masked_thash(dest1, dest2,
                   pk_buffer1, pk_buffer2,
                   SPX_WOTS_LEN, ctx, pk_addr);
}
