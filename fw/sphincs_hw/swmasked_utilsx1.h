#ifndef SPX_UTILSX4_swmasked_H
#define SPX_UTILSX4_swmasked_H

#include <stdint.h>
#include "params.h"
#include "swmasked_context.h"

/**
 * For a given leaf index, computes the authentication path and the resulting
 * root node using Merkle's TreeHash algorithm.
 * Expects the layer and tree parts of the tree_addr to be set, as well as the
 * tree type (i.e. SPX_ADDR_TYPE_HASHTREE or SPX_ADDR_TYPE_FORSTREE).
 * Applies the offset idx_offset to indices before building addresses, so that
 * it is possible to continue counting indices across trees.
 */
#define treehashx1_masked SPX_NAMESPACE(treehashx1_masked)
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
                       void *info);

#endif
