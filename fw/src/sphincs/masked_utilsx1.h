#ifndef masked_SPX_UTILSX4_H
#define masked_SPX_UTILSX4_H

#include <stdint.h>
#include "params.h"
#include "swmasked_context.h"

#define masked_treehashx1 SPX_NAMESPACE(masked_treehashx1)
void masked_treehashx1(unsigned char *root1, unsigned char *root2,
                         unsigned char *auth_path,
                         const spx_ctx* ctx,
                         uint32_t leaf_idx, uint32_t idx_offset,
                         uint32_t tree_height,
                         void (*masked_gen_leaf)(
                             unsigned char*,
                             unsigned char* ,
                             const spx_ctx*,
                             uint32_t idx, void *info),
                         uint32_t tree_addr[8],
                         void *info);

#endif
