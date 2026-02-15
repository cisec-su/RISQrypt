#if !defined( MERKLE_HWMASKED_H_ )
#define MERKLE_HWMASKED_H_

#include <stdint.h>

#define merkle_sign_hwmasked SPX_NAMESPACE(merkle_sign_hwmasked)
void merkle_sign_hwmasked(uint8_t *sig, unsigned char *root,
        const spx_ctx* ctx,
        uint32_t wots_addr[8], uint32_t tree_addr[8],
        uint32_t idx_leaf);

#define merkle_gen_root_hwmasked SPX_NAMESPACE(merkle_gen_root_hwmasked)
void merkle_gen_root_hwmasked(unsigned char *root, const spx_ctx* ctx);

#endif /* MERKLE_HWMASKED_H_ */
