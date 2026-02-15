#if !defined( WOTSX1_HWMASKED_H_ )
#define WOTSX1_HWMASKED_H_

#include <string.h>

struct leaf_info_x1_hwmasked {
    unsigned char *wots_sig1;
    unsigned char *wots_sig2;
    uint32_t wots_sign_leaf;
    unsigned int *wots_steps;
    uint32_t leaf_addr[8];
    uint32_t pk_addr[8];
};

#define INITIALIZE_LEAF_INFO_X1_HWMASKED(info, addr, step_buffer) { \
    info.wots_sig1 = 0;             \
    info.wots_sig2 = 0;             \
    info.wots_sign_leaf = ~0u;      \
    info.wots_steps = step_buffer; \
    memcpy( &info.leaf_addr[0], addr, 32 ); \
    memcpy( &info.pk_addr[0], addr, 32 ); \
}

#define wots_gen_leafx1_hwmasked SPX_NAMESPACE(wots_gen_leafx1_hwmasked)
void wots_gen_leafx1_hwmasked(unsigned char *dest1, unsigned char *dest2,
                              const spx_ctx *ctx,
                              uint32_t leaf_idx, void *v_info);

#endif /* WOTSX1_HWMASKED_H_ */
