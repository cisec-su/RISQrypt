#if !defined( MERKLE_swmasked_H_ )
#define MERKLE_swmasked_H_

#include <stdint.h>

/* Generate a Merkle signature (WOTS signature followed by the Merkle */
/* authentication path) */
#define merkle_sign_masked SPX_NAMESPACE(merkle_sign_masked)
void merkle_sign_masked(uint8_t *sig, unsigned char *root,
        const spx_ctx* ctx,
        uint32_t wots_addr[8], uint32_t tree_addr[8],
        uint32_t idx_leaf);
/* Compute the root node of the top-most subtree. */
#define merkle_gen_root_masked SPX_NAMESPACE(merkle_gen_root_masked)
void merkle_gen_root_masked(unsigned char *root, const spx_ctx* ctx);
#endif /* MERKLE_swmasked_H_ */