#if !defined( masked_MERKLE_H_ )
#define masked_MERKLE_H_

#include <stdint.h>

#define masked_merkle_sign SPX_NAMESPACE(masked_merkle_sign)
void masked_merkle_sign(uint8_t *sig, unsigned char *root,
        const spx_ctx* ctx,
        uint32_t wots_addr[8], uint32_t tree_addr[8],
        uint32_t idx_leaf);

#define masked_merkle_gen_root SPX_NAMESPACE(masked_merkle_gen_root)
void masked_merkle_gen_root(unsigned char *root, const spx_ctx* ctx);

#endif /* masked_MERKLE_H_ */
