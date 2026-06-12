#if !defined( masked_WOTSX1_H_ )
#define masked_WOTSX1_H_

#include <string.h>

struct masked_leaf_info_x1 {
    unsigned char *wots_sig1;
    unsigned char *wots_sig2;
    uint32_t wots_sign_leaf;
    unsigned int *wots_steps;
    uint32_t leaf_addr[8];
    uint32_t pk_addr[8];
};

#define masked_INITIALIZE_LEAF_INFO_X1(info, addr, step_buffer) { \
    info.wots_sig1 = 0;             \
    info.wots_sig2 = 0;             \
    info.wots_sign_leaf = ~0u;      \
    info.wots_steps = step_buffer; \
    memcpy( &info.leaf_addr[0], addr, 32 ); \
    memcpy( &info.pk_addr[0], addr, 32 ); \
}

#define masked_wots_gen_leafx1 SPX_NAMESPACE(masked_wots_gen_leafx1)
void masked_wots_gen_leafx1(unsigned char *dest1, unsigned char *dest2,
                              const spx_ctx *ctx,
                              uint32_t leaf_idx, void *v_info);

#endif /* masked_WOTSX1_H_ */
